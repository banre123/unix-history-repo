/*	
 * $Id: dev_net.c,v 1.2 1998/08/22 10:31:01 dfr Exp $
 * From: $NetBSD: dev_net.c,v 1.12 1997/12/10 20:38:37 gwr Exp $
 */

/*-
 * Copyright (c) 1997 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Gordon W. Ross.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This module implements a "raw device" interface suitable for
 * use by the stand-alone I/O library NFS code.  This interface
 * does not support any "block" access, and exists only for the
 * purpose of initializing the network interface, getting boot
 * parameters, and performing the NFS mount.
 *
 * At open time, this does:
 *
 * find interface      - netif_open()
 * RARP for IP address - rarp_getipaddress()
 * RPC/bootparams      - callrpc(d, RPC_BOOTPARAMS, ...)
 * RPC/mountd          - nfs_mount(sock, ip, path)
 *
 * the root file handle from mountd is saved in a global
 * for use by the NFS open code (NFS/lookup).
 */

#include <machine/stdarg.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>

#include <stand.h>
#include <net.h>
#include <netif.h>
#include <bootparam.h>

#include "dev_net.h"
#include "bootstrap.h"

int debug = 0;

static int netdev_sock = -1;
static int netdev_opens;

static int	net_init(void);
static int	net_open(struct open_file *, void *vdev);
static int	net_close(struct open_file *);
static int	net_ioctl();
static int	net_strategy();

static int net_getparams(int sock);

struct devsw netdev = {
    "net", 
    DEVT_NET, 
    net_init,
    net_strategy, 
    net_open, 
    net_close, 
    noioctl
};

int
net_init(void)
{
    return 0;
}

/*
 * Called by devopen after it sets f->f_dev to our devsw entry.
 * This opens the low-level device and sets f->f_devdata.
 * This is declared with variable arguments...
 */
int
net_open(struct open_file *f, void *vdev)
{
    char *devname;		/* Device part of file name (or NULL). */
    int error = 0;

    devname = vdev;

    /* On first open, do netif open, mount, etc. */
    if (netdev_opens == 0) {
	/* Find network interface. */
	if (netdev_sock < 0) {
	    netdev_sock = netif_open(devname);
	    if (netdev_sock < 0) {
		printf("net_open: netif_open() failed\n");
		return (ENXIO);
	    }
	    if (debug)
		printf("net_open: netif_open() succeeded\n");
	}
	if (rootip.s_addr == 0) {
	    /* Get root IP address, and path, etc. */
	    error = net_getparams(netdev_sock);
	    if (error) {
				/* getparams makes its own noise */
		netif_close(netdev_sock);
		netdev_sock = -1;
		return (error);
	    }
	}
    }
    netdev_opens++;
    f->f_devdata = &netdev_sock;
    return (error);
}

int
net_close(f)
    struct open_file *f;
{

#ifdef	NETIF_DEBUG
    if (debug)
	printf("net_close: opens=%d\n", netdev_opens);
#endif

    /* On last close, do netif close, etc. */
    f->f_devdata = NULL;
    /* Extra close call? */
    if (netdev_opens <= 0)
	return (0);
    netdev_opens--;
    /* Not last close? */
    if (netdev_opens > 0)
	return(0);
    rootip.s_addr = 0;
    if (netdev_sock >= 0) {
	if (debug)
	    printf("net_close: calling netif_close()\n");
	netif_close(netdev_sock);
	netdev_sock = -1;
    }
    return (0);
}

int
net_ioctl()
{
    return EIO;
}

int
net_strategy()
{
    return EIO;
}

#define SUPPORT_BOOTP

/*
 * Get info for NFS boot: our IP address, our hostname,
 * server IP address, and our root path on the server.
 * There are two ways to do this:  The old, Sun way,
 * and the more modern, BOOTP way. (RFC951, RFC1048)
 *
 * The default is to use the Sun bootparams RPC
 * (because that is what the kernel will do).
 * MD code can make try_bootp initialied data,
 * which will override this common definition.
 */
#ifdef	SUPPORT_BOOTP
int try_bootp = 1;
int bootp(int sock);
#endif

static int
net_getparams(sock)
    int sock;
{
    char buf[MAXHOSTNAMELEN];
    n_long smask;

#ifdef	SUPPORT_BOOTP
    /*
     * Try to get boot info using BOOTP.  If we succeed, then
     * the server IP address, gateway, and root path will all
     * be initialized.  If any remain uninitialized, we will
     * use RARP and RPC/bootparam (the Sun way) to get them.
     */
    if (try_bootp)
	bootp(sock);
    if (myip.s_addr != 0)
	return (0);
    if (debug)
	printf("net_open: BOOTP failed, trying RARP/RPC...\n");
#endif

    /*
     * Use RARP to get our IP address.  This also sets our
     * netmask to the "natural" default for our address.
     */
    if (rarp_getipaddress(sock)) {
	printf("net_open: RARP failed\n");
	return (EIO);
    }
    printf("net_open: client addr: %s\n", inet_ntoa(myip));

    /* Get our hostname, server IP address, gateway. */
    if (bp_whoami(sock)) {
	printf("net_open: bootparam/whoami RPC failed\n");
	return (EIO);
    }
    printf("net_open: client name: %s\n", hostname);

    /*
     * Ignore the gateway from whoami (unreliable).
     * Use the "gateway" parameter instead.
     */
    smask = 0;
    gateip.s_addr = 0;
    if (bp_getfile(sock, "gateway", &gateip, buf) == 0) {
	/* Got it!  Parse the netmask. */
	smask = ip_convertaddr(buf);
    }
    if (smask) {
	netmask = smask;
	printf("net_open: subnet mask: %s\n", intoa(netmask));
    }
    if (gateip.s_addr)
	printf("net_open: net gateway: %s\n", inet_ntoa(gateip));

    /* Get the root server and pathname. */
    if (bp_getfile(sock, "root", &rootip, rootpath)) {
	printf("net_open: bootparam/getfile RPC failed\n");
	return (EIO);
    }

    printf("net_open: server addr: %s\n", inet_ntoa(rootip));
    printf("net_open: server path: %s\n", rootpath);

    return (0);
}
