
/*
 * ng_one2many.c
 *
 * Copyright (c) 2000 Whistle Communications, Inc.
 * All rights reserved.
 * 
 * Subject to the following obligations and disclaimer of warranty, use and
 * redistribution of this software, in source or object code forms, with or
 * without modifications are expressly permitted by Whistle Communications;
 * provided, however, that:
 * 1. Any and all reproductions of the source or object code must include the
 *    copyright notice above and the following disclaimer of warranties; and
 * 2. No rights are granted, in any manner or form, to use Whistle
 *    Communications, Inc. trademarks, including the mark "WHISTLE
 *    COMMUNICATIONS" on advertising, endorsements, or otherwise except as
 *    such appears in the above copyright notice or in the software.
 * 
 * THIS SOFTWARE IS BEING PROVIDED BY WHISTLE COMMUNICATIONS "AS IS", AND
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, WHISTLE COMMUNICATIONS MAKES NO
 * REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, REGARDING THIS SOFTWARE,
 * INCLUDING WITHOUT LIMITATION, ANY AND ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 * WHISTLE COMMUNICATIONS DOES NOT WARRANT, GUARANTEE, OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF, OR THE RESULTS OF THE USE OF THIS
 * SOFTWARE IN TERMS OF ITS CORRECTNESS, ACCURACY, RELIABILITY OR OTHERWISE.
 * IN NO EVENT SHALL WHISTLE COMMUNICATIONS BE LIABLE FOR ANY DAMAGES
 * RESULTING FROM OR ARISING OUT OF ANY USE OF THIS SOFTWARE, INCLUDING
 * WITHOUT LIMITATION, ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * PUNITIVE, OR CONSEQUENTIAL DAMAGES, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES, LOSS OF USE, DATA OR PROFITS, HOWEVER CAUSED AND UNDER ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF WHISTLE COMMUNICATIONS IS ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Archie Cobbs <archie@freebsd.org>
 *
 * $FreeBSD$
 */

/*
 * ng_one2many(4) netgraph node type
 *
 * Packets received on the "one" hook are sent out each of the
 * "many" hooks accoring to an algorithm. Packets received on any
 * "many" hook are always delivered to the "one" hook.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/ctype.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/errno.h>

#include <net/if.h>
#include <net/if_media.h>

#include <netgraph/ng_message.h>
#include <netgraph/netgraph.h>
#include <netgraph/ng_parse.h>
#include <netgraph/ng_ether.h>
#include <netgraph/ng_one2many.h>

/* Per-link private data */
struct ng_one2many_link {
	hook_p				hook;	/* netgraph hook */
	struct ng_one2many_link_stats	stats;	/* link stats */
};

/* Per-node private data */
struct ng_one2many_private {
	struct ng_one2many_config	conf;		/* node configuration */
	struct ng_one2many_link		one;		/* "one" hook */
	struct ng_one2many_link		many[NG_ONE2MANY_MAX_LINKS];
	u_int16_t			nextMany;	/* next round-robin */
	u_int16_t			numActiveMany;	/* # active "many" */
	u_int16_t			activeMany[NG_ONE2MANY_MAX_LINKS];
	struct callout_handle		callout;
};
typedef struct ng_one2many_private *priv_p;

/* Netgraph node methods */
static ng_constructor_t	ng_one2many_constructor;
static ng_rcvmsg_t	ng_one2many_rcvmsg;
static ng_shutdown_t	ng_one2many_shutdown;
static ng_newhook_t	ng_one2many_newhook;
static ng_rcvdata_t	ng_one2many_rcvdata;
static ng_disconnect_t	ng_one2many_disconnect;

/* Other functions */
static void		ng_one2many_scan(node_p node, hook_p hook __unused,
			    void *arg1 __unused, int arg2 __unused);
static void		ng_one2many_update_many(priv_p priv);

/* Store each hook's link number in the private field */
#define LINK_NUM(hook)		(*(int16_t *)(&(hook)->private))

/******************************************************************
		    NETGRAPH PARSE TYPES
******************************************************************/

/* Parse type for struct ng_one2many_config */
static const struct ng_parse_fixedarray_info
    ng_one2many_enableLinks_array_type_info = {
	&ng_parse_uint8_type,
	NG_ONE2MANY_MAX_LINKS
};
static const struct ng_parse_type ng_one2many_enableLinks_array_type = {
	&ng_parse_fixedarray_type,
	&ng_one2many_enableLinks_array_type_info,
};
static const struct ng_parse_struct_field ng_one2many_config_type_fields[]
	= NG_ONE2MANY_CONFIG_TYPE_INFO(&ng_one2many_enableLinks_array_type);
static const struct ng_parse_type ng_one2many_config_type = {
	&ng_parse_struct_type,
	&ng_one2many_config_type_fields
};

/* Parse type for struct ng_one2many_link_stats */
static const struct ng_parse_struct_field ng_one2many_link_stats_type_fields[]
	= NG_ONE2MANY_LINK_STATS_TYPE_INFO;
static const struct ng_parse_type ng_one2many_link_stats_type = {
	&ng_parse_struct_type,
	&ng_one2many_link_stats_type_fields
};

/* List of commands and how to convert arguments to/from ASCII */
static const struct ng_cmdlist ng_one2many_cmdlist[] = {
	{
	  NGM_ONE2MANY_COOKIE,
	  NGM_ONE2MANY_SET_CONFIG,
	  "setconfig",
	  &ng_one2many_config_type,
	  NULL
	},
	{
	  NGM_ONE2MANY_COOKIE,
	  NGM_ONE2MANY_GET_CONFIG,
	  "getconfig",
	  NULL,
	  &ng_one2many_config_type
	},
	{
	  NGM_ONE2MANY_COOKIE,
	  NGM_ONE2MANY_GET_STATS,
	  "getstats",
	  &ng_parse_int32_type,
	  &ng_one2many_link_stats_type
	},
	{
	  NGM_ONE2MANY_COOKIE,
	  NGM_ONE2MANY_CLR_STATS,
	  "clrstats",
	  &ng_parse_int32_type,
	  NULL,
	},
	{
	  NGM_ONE2MANY_COOKIE,
	  NGM_ONE2MANY_GETCLR_STATS,
	  "getclrstats",
	  &ng_parse_int32_type,
	  &ng_one2many_link_stats_type
	},
	{ 0 }
};

/* Node type descriptor */
static struct ng_type ng_one2many_typestruct = {
	NG_ABI_VERSION,
	NG_ONE2MANY_NODE_TYPE,
	NULL,
	ng_one2many_constructor,
	ng_one2many_rcvmsg,
	ng_one2many_shutdown,
	ng_one2many_newhook,
	NULL,
	NULL,
	ng_one2many_rcvdata,
	ng_one2many_disconnect,
	ng_one2many_cmdlist,
};
NETGRAPH_INIT(one2many, &ng_one2many_typestruct);

/******************************************************************
		    NETGRAPH NODE METHODS
******************************************************************/

/*
 * Node constructor
 */
static int
ng_one2many_constructor(node_p node)
{
	priv_p priv;

	/* Allocate and initialize private info */
	MALLOC(priv, priv_p, sizeof(*priv), M_NETGRAPH, M_NOWAIT | M_ZERO);
	if (priv == NULL)
		return (ENOMEM);
	priv->conf.xmitAlg = NG_ONE2MANY_XMIT_ROUNDROBIN;
	priv->conf.failAlg = NG_ONE2MANY_FAIL_MANUAL;
	priv->conf.interval = 5;

	NG_NODE_SET_PRIVATE(node, priv);

	if (priv->conf.failAlg == NG_ONE2MANY_FAIL_IFACE_LINK) {
		priv->callout = ng_timeout(node, NULL, priv->conf.interval * hz,
		    ng_one2many_scan, NULL, 0);
	}

	/* Done */
	return (0);
}

/*
 * Method for attaching a new hook
 */
static	int
ng_one2many_newhook(node_p node, hook_p hook, const char *name)
{
	const priv_p priv = NG_NODE_PRIVATE(node);
	struct ng_one2many_link *link;
	int linkNum;
	u_long i;

	/* Which hook? */
	if (strncmp(name, NG_ONE2MANY_HOOK_MANY_PREFIX,
	    strlen(NG_ONE2MANY_HOOK_MANY_PREFIX)) == 0) {
		const char *cp;
		char *eptr;

		cp = name + strlen(NG_ONE2MANY_HOOK_MANY_PREFIX);
		if (!isdigit(*cp) || (cp[0] == '0' && cp[1] != '\0'))
			return (EINVAL);
		i = strtoul(cp, &eptr, 10);
		if (*eptr != '\0' || i < 0 || i >= NG_ONE2MANY_MAX_LINKS)
			return (EINVAL);
		linkNum = (int)i;
		link = &priv->many[linkNum];
	} else if (strcmp(name, NG_ONE2MANY_HOOK_ONE) == 0) {
		linkNum = NG_ONE2MANY_ONE_LINKNUM;
		link = &priv->one;
	} else
		return (EINVAL);

	/* Is hook already connected? (should never happen) */
	if (link->hook != NULL)
		return (EISCONN);

	/* Setup private info for this link */
	NG_HOOK_SET_PRIVATE(hook, (void *)(intptr_t)linkNum);
	link->hook = hook;
	bzero(&link->stats, sizeof(link->stats));
	if (linkNum != NG_ONE2MANY_ONE_LINKNUM) {
		priv->conf.enabledLinks[linkNum] = 1;	/* auto-enable link */
		ng_one2many_update_many(priv);
	}

	/* Done */
	return (0);
}

/*
 * Receive a control message
 */
static int
ng_one2many_rcvmsg(node_p node, item_p item, hook_p lasthook)
{
	const priv_p priv = NG_NODE_PRIVATE(node);
	struct ng_mesg *resp = NULL;
	int error = 0;
	struct ng_mesg *msg;

	NGI_GET_MSG(item, msg);
	switch (msg->header.typecookie) {
	case NGM_ONE2MANY_COOKIE:
		switch (msg->header.cmd) {
		case NGM_ONE2MANY_SET_CONFIG:
		    {
			struct ng_one2many_config *conf;
			int i;

			/* Check that new configuration is valid */
			if (msg->header.arglen != sizeof(*conf)) {
				error = EINVAL;
				break;
			}
			conf = (struct ng_one2many_config *)msg->data;
			switch (conf->xmitAlg) {
			case NG_ONE2MANY_XMIT_ROUNDROBIN:
			case NG_ONE2MANY_XMIT_ALL:
				break;
			default:
				error = EINVAL;
				break;
			}
			switch (conf->failAlg) {
			case NG_ONE2MANY_FAIL_MANUAL:
			case NG_ONE2MANY_FAIL_IFACE_LINK:
				break;
			default:
				error = EINVAL;
				break;
			}
			if (error != 0)
				break;

			/* Normalized many link enabled bits */ 
			for (i = 0; i < NG_ONE2MANY_MAX_LINKS; i++)
				conf->enabledLinks[i] = !!conf->enabledLinks[i];

			/* Copy config and reset */
			bcopy(conf, &priv->conf, sizeof(*conf));
			ng_one2many_update_many(priv);

			ng_untimeout(priv->callout, node);
			if (priv->conf.failAlg == NG_ONE2MANY_FAIL_IFACE_LINK) {
				priv->callout = ng_timeout(node, NULL,
				    priv->conf.interval * hz, ng_one2many_scan,
				    NULL, 0);
			}
			break;
		    }
		case NGM_ONE2MANY_GET_CONFIG:
		    {
			struct ng_one2many_config *conf;

			NG_MKRESPONSE(resp, msg, sizeof(*conf), M_NOWAIT);
			if (resp == NULL) {
				error = ENOMEM;
				break;
			}
			conf = (struct ng_one2many_config *)resp->data;
			bcopy(&priv->conf, conf, sizeof(priv->conf));
			break;
		    }
		case NGM_ONE2MANY_GET_STATS:
		case NGM_ONE2MANY_CLR_STATS:
		case NGM_ONE2MANY_GETCLR_STATS:
		    {
			struct ng_one2many_link *link;
			int linkNum;

			/* Get link */
			if (msg->header.arglen != sizeof(int32_t)) {
				error = EINVAL;
				break;
			}
			linkNum = *((int32_t *)msg->data);
			if (linkNum == NG_ONE2MANY_ONE_LINKNUM)
				link = &priv->one;
			else if (linkNum == 0
			    && linkNum < NG_ONE2MANY_MAX_LINKS) {
				link = &priv->many[linkNum];
			} else {
				error = EINVAL;
				break;
			}

			/* Get/clear stats */
			if (msg->header.cmd != NGM_ONE2MANY_CLR_STATS) {
				NG_MKRESPONSE(resp, msg,
				    sizeof(link->stats), M_NOWAIT);
				if (resp == NULL) {
					error = ENOMEM;
					break;
				}
				bcopy(&link->stats,
				    resp->data, sizeof(link->stats));
			}
			if (msg->header.cmd != NGM_ONE2MANY_GET_STATS)
				bzero(&link->stats, sizeof(link->stats));
			break;
		    }
		default:
			error = EINVAL;
			break;
		}
		break;
	default:
		error = EINVAL;
		break;
	}

	/* Done */
	NG_RESPOND_MSG(error, node, item, resp);
	NG_FREE_MSG(msg);
	return (error);
}

/*
 * Receive data on a hook
 */
static int
ng_one2many_rcvdata(hook_p hook, item_p item)
{
	const node_p node = NG_HOOK_NODE(hook);
	const priv_p priv = NG_NODE_PRIVATE(node);
	struct ng_one2many_link *src;
	struct ng_one2many_link *dst = NULL;
	int error = 0;
	int linkNum;
	int i;
	struct mbuf *m;
	meta_p meta;

	m = NGI_M(item); /* just peaking, mbuf still owned by item */
	/* Get link number */
	linkNum = (intptr_t)NG_HOOK_PRIVATE(hook);
	KASSERT(linkNum == NG_ONE2MANY_ONE_LINKNUM
	    || (linkNum >= 0 && linkNum < NG_ONE2MANY_MAX_LINKS),
	    ("%s: linkNum=%d", __func__, linkNum));

	/* Figure out source link */
	src = (linkNum == NG_ONE2MANY_ONE_LINKNUM) ?
	    &priv->one : &priv->many[linkNum];
	KASSERT(src->hook != NULL, ("%s: no src%d", __func__, linkNum));

	/* Update receive stats */
	src->stats.recvPackets++;
	src->stats.recvOctets += m->m_pkthdr.len;

	/* Figure out destination link */
	if (linkNum == NG_ONE2MANY_ONE_LINKNUM) {
		if (priv->numActiveMany == 0) {
			NG_FREE_ITEM(item);
			return (ENOTCONN);
		}
		switch(priv->conf.xmitAlg) {
		case NG_ONE2MANY_XMIT_ROUNDROBIN:
			dst = &priv->many[priv->activeMany[priv->nextMany]];
			priv->nextMany = (priv->nextMany + 1) % priv->numActiveMany;
			break;
		case NG_ONE2MANY_XMIT_ALL:
			meta = NGI_META(item); /* peek.. */
			/* no need to copy data for the 1st one */
			dst = &priv->many[priv->activeMany[0]];

			/* make copies of data and send for all links
			 * except the first one, which we'll do last 
			 */
			for (i = 1; i < priv->numActiveMany; i++) {
				meta_p meta2 = NULL;
				struct mbuf *m2;
				struct ng_one2many_link *mdst;

				mdst = &priv->many[priv->activeMany[i]];
				m2 = m_dup(m, M_DONTWAIT);        /* XXX m_copypacket() */
				if (m2 == NULL) {
					mdst->stats.memoryFailures++;
					NG_FREE_ITEM(item);
					NG_FREE_M(m);
					return (ENOBUFS);
				}
				if (meta != NULL
				    && (meta2 = ng_copy_meta(meta)) == NULL) {
					mdst->stats.memoryFailures++;
					m_freem(m2);
					NG_FREE_ITEM(item);
					NG_FREE_M(m);
					return (ENOMEM);
				}
				/* Update transmit stats */
				mdst->stats.xmitPackets++;
				mdst->stats.xmitOctets += m->m_pkthdr.len;
				NG_SEND_DATA(error, mdst->hook, m2, meta2);
			}
			break;
#ifdef INVARIANTS
		default:
			panic("%s: invalid xmitAlg", __func__);
#endif
		}
	} else {
		dst = &priv->one;
	}

	/* Update transmit stats */
	dst->stats.xmitPackets++;
	dst->stats.xmitOctets += m->m_pkthdr.len;

	/* Deliver packet */
	NG_FWD_ITEM_HOOK(error, item, dst->hook);
	return (error);
}

/*
 * Shutdown node
 */
static int
ng_one2many_shutdown(node_p node)
{
	const priv_p priv = NG_NODE_PRIVATE(node);

	KASSERT(priv->numActiveMany == 0,
	    ("%s: numActiveMany=%d", __func__, priv->numActiveMany));
	if (priv->conf.failAlg == NG_ONE2MANY_FAIL_IFACE_LINK)
		ng_untimeout(priv->callout, node);
	FREE(priv, M_NETGRAPH);
	NG_NODE_SET_PRIVATE(node, NULL);
	NG_NODE_UNREF(node);
	return (0);
}

/*
 * Hook disconnection.
 */
static int
ng_one2many_disconnect(hook_p hook)
{
	const priv_p priv = NG_NODE_PRIVATE(NG_HOOK_NODE(hook));
	int linkNum;

	/* Get link number */
	linkNum = (intptr_t)NG_HOOK_PRIVATE(hook);
	KASSERT(linkNum == NG_ONE2MANY_ONE_LINKNUM
	    || (linkNum >= 0 && linkNum < NG_ONE2MANY_MAX_LINKS),
	    ("%s: linkNum=%d", __func__, linkNum));

	/* Nuke the link */
	if (linkNum == NG_ONE2MANY_ONE_LINKNUM)
		priv->one.hook = NULL;
	else {
		priv->many[linkNum].hook = NULL;
		priv->conf.enabledLinks[linkNum] = 0;
		ng_one2many_update_many(priv);
	}

	/* If no hooks left, go away */
	if ((NG_NODE_NUMHOOKS(NG_HOOK_NODE(hook)) == 0)
	&& (NG_NODE_IS_VALID(NG_HOOK_NODE(hook))))
		ng_rmnode_self(NG_HOOK_NODE(hook));
	return (0);
}

/******************************************************************
		    	OTHER FUNCTIONS
******************************************************************/

#if 0
/*
 * Get interface name.
 */
static const char *
ng_one2many_ifname(struct ng_one2many_link *link)
{
	node_p node;

	node = link->hook->hk_peer->hk_node;
	if (strcmp(node->nd_type->name, "ether") != 0)
		return ("unknown");
	return (node->nd_name);
}
#endif

/*
 * Check if interface related to given node is active.
 */
static int
ng_one2many_active(struct ng_one2many_link *link)
{
	struct ng_ether_private *ethpriv;
	struct ifmediareq ifmr;
	struct ifnet *ifp;
	node_p node;
	int error;

	node = link->hook->hk_peer->hk_node;
	if (strcmp(node->nd_type->name, "ether") != 0)
		return (0);
	ethpriv = NG_NODE_PRIVATE(node);
	ifp = ethpriv->ifp;
	bzero(&ifmr, sizeof(ifmr));
	error = ifp->if_ioctl(ifp, SIOCGIFMEDIA, (char *)&ifmr);
	if (error != 0)
		return (0);
	if ((ifmr.ifm_status & IFM_ACTIVE) == 0)
		return (0);
	return (1);
}

/*
 * Check every priv->conf.interval seconds for active links.
 */
static void
ng_one2many_scan(node_p node, hook_p hook __unused, void *arg1 __unused,
    int arg2 __unused)
{
	const priv_p priv = NG_NODE_PRIVATE(node);

	ng_one2many_update_many(priv);

	priv->callout = ng_timeout(node, NULL, priv->conf.interval * hz,
	    ng_one2many_scan, NULL, 0);
}

/*
 * Update internal state after the addition or removal of a "many" link
 */
static void
ng_one2many_update_many(priv_p priv)
{
	int linkNum;

	/* Update list of which "many" links are up */
	priv->numActiveMany = 0;
	for (linkNum = 0; linkNum < NG_ONE2MANY_MAX_LINKS; linkNum++) {
		switch (priv->conf.failAlg) {
		case NG_ONE2MANY_FAIL_MANUAL:
			if (priv->many[linkNum].hook != NULL
			    && priv->conf.enabledLinks[linkNum]) {
				priv->activeMany[priv->numActiveMany] = linkNum;
				priv->numActiveMany++;
			}
			break;
		case NG_ONE2MANY_FAIL_IFACE_LINK:
			if (priv->many[linkNum].hook != NULL &&
			    ng_one2many_active(&priv->many[linkNum])) {
				priv->activeMany[priv->numActiveMany] = linkNum;
				priv->numActiveMany++;
			}
			break;
#ifdef INVARIANTS
		default:
			panic("%s: invalid failAlg", __func__);
#endif
		}
	}

	/* Update transmit algorithm state */
	switch (priv->conf.xmitAlg) {
	case NG_ONE2MANY_XMIT_ROUNDROBIN:
		if (priv->numActiveMany > 0)
			priv->nextMany %= priv->numActiveMany;
		break;
	case NG_ONE2MANY_XMIT_ALL:
		break;
#ifdef INVARIANTS
	default:
		panic("%s: invalid xmitAlg", __func__);
#endif
	}
}


