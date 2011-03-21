/*
 * Copyright (c) 2006-2007 Mellanox Technologies. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/pci.h>
#include "wc.h"

#if defined(__i386__) || defined(__x86_64__)

pgprot_t pgprot_wc(pgprot_t _prot)
{
	return pgprot_writecombine(_prot);
}

int mlx4_wc_enabled(void)
{
	return 1;
}

#elif defined(CONFIG_PPC64)

pgprot_t pgprot_wc(pgprot_t _prot)
{
	return __pgprot((pgprot_val(_prot) | _PAGE_NO_CACHE) &
				     ~(pgprot_t)_PAGE_GUARDED);
}

int mlx4_wc_enabled(void)
{
	return 1;
}

#else	/* !(defined(__i386__) || defined(__x86_64__)) */

pgprot_t pgprot_wc(pgprot_t _prot)
{
	return pgprot_noncached(_prot);
}

int mlx4_wc_enabled(void)
{
	return 0;
}

#endif

