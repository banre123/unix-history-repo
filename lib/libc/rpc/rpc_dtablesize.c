/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#if defined(LIBC_SCCS) && !defined(lint)
/*static char *sccsid = "from: @(#)rpc_dtablesize.c 1.2 87/08/11 Copyr 1987 Sun Micro";*/
/*static char *sccsid = "from: @(#)rpc_dtablesize.c	2.1 88/07/29 4.0 RPCSRC";*/
static char *rcsid = "rpc_dtablesize.c,v 1.1 1994/08/07 18:36:02 wollman Exp";
#endif

#include <sys/types.h>

/*
 * Cache the result of getdtablesize(), so we don't have to do an
 * expensive system call every time.
 */
/*
 * XXX In FreeBSD 2.x, you can have the maximum number of open file
 * descriptors be greater than FD_SETSIZE (which us 256 by default).
 * This can lead to many RPC functions getting back an EINVAL from
 * select() and bombing all over the place.
 *
 * You can apparently get select() to handle values larger than 256
 * by patching the kernel, but most people aren't likely to know
 * that. Clamping this function at 256 is a kludge, but it'll have to
 * do until select()'s descriptor table size can be adjusted dynamically.
 */
_rpc_dtablesize()
{
	static int size;

	if (size == 0) {
		size = getdtablesize();
		if (size > FD_SETSIZE)
			size = FD_SETSIZE;
	}
	return (size);
}
