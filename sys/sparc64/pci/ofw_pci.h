/*
 * Copyright (c) 1999, 2000 Matthew R. Green
 * All rights reserved.
 * Copyright 2001 by Thomas Moestl <tmm@FreeBSD.org>.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: NetBSD: psychoreg.h,v 1.8 2001/09/10 16:17:06 eeh Exp
 *
 * $FreeBSD$
 */

#ifndef _SPARC64_PCI_OFW_PCI_H_
#define _SPARC64_PCI_OFW_PCI_H_

/* PCI range child spaces. XXX: are these MI? */
#define	PCI_CS_CONFIG	0x00
#define	PCI_CS_IO	0x01
#define	PCI_CS_MEM32	0x02
#define	PCI_CS_MEM64	0x03

struct ofw_pci_imap {
	u_int32_t	phys_hi;
	u_int32_t	phys_mid;
	u_int32_t	phys_lo;
	u_int32_t	intr;
	int32_t		child_node;
	u_int32_t	child_intr;
};

struct ofw_pci_imap_msk {
	u_int32_t	phys_hi;
	u_int32_t	phys_mid;
	u_int32_t	phys_lo;
	u_int32_t	intr;
};

int ofw_pci_find_imap(phandle_t, struct ofw_pci_imap **,
    struct ofw_pci_imap_msk *);
int ofw_pci_route_intr2(int, struct ofw_pci_register *,
    struct ofw_pci_imap *, int, struct ofw_pci_imap_msk *);
int ofw_pci_route_intr(phandle_t, struct ofw_pci_register *,
    struct ofw_pci_imap *, int, struct ofw_pci_imap_msk *);
void ofw_pci_init_intr(device_t, phandle_t, struct ofw_pci_imap *, int,
    struct ofw_pci_imap_msk *);
phandle_t ofw_pci_find_node(int, int, int);
int ofw_pci_dev_iterate_node(device_t, phandle_t, uintptr_t *, int, int *,
    int *, uintptr_t *, uintptr_t *);
int ofw_pci_dev_iterate(device_t, uintptr_t *, int, int *, int *, uintptr_t *,
    uintptr_t *);

#endif /* ! _SPARC64_PCI_OFW_PCI_H_ */
