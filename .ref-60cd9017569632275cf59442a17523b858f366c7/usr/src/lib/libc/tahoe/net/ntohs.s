/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 */

#if defined(LIBC_SCCS) && !defined(lint)
	.asciz "@(#)ntohs.s	8.1 (Berkeley) %G%"
#endif /* LIBC_SCCS and not lint */

/* hostorder = ntohs(netorder) */

#include "DEFS.h"

ENTRY(ntohs, 0)
	movzwl	6(fp),r0
	ret
