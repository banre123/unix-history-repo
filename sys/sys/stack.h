/*-
 * Copyright (c) 2005 Antoine Brodin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef _SYS_STACK_H_
#define	_SYS_STACK_H_

#define	STACK_MAX	18	/* Don't change, stack_ktr relies on this. */

struct sbuf;

struct stack {
	int		depth;
	vm_offset_t	pcs[STACK_MAX];
};

/* MI Routines. */
struct stack	*stack_create(void);
void		 stack_destroy(struct stack *);
int		 stack_put(struct stack *, vm_offset_t);
void		 stack_copy(struct stack *, struct stack *);
void		 stack_zero(struct stack *);
void		 stack_print(struct stack *);
void		 stack_sbuf_print(struct sbuf *, struct stack *);
#ifdef KTR
void		 stack_ktr(u_int, const char *, int, struct stack *, int);
#define	CTRSTACK(m, st, cheap) do {					\
	if (KTR_COMPILE & (m))						\
		stack_ktr((m), __FILE__, __LINE__, st, cheap);		\
	} while(0)
#else
#define	CTRSTACK(m, st, cheap)
#endif

/* MD Routine. */
void		 stack_save(struct stack *);

#endif
