/*-
 * Copyright (c) 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)printf.c	5.5 (Berkeley) %G%
 */

/*
 * Scaled down version of printf(3).
 *
 * One additional format:
 *
 * The format %b is supported to decode error registers.
 * Its usage is:
 *
 *	printf("reg=%b\n", regval, "<base><arg>*");
 *
 * where <base> is the output base expressed as a control character, e.g.
 * \10 gives octal; \20 gives hex.  Each arg is a sequence of characters,
 * the first of which gives the bit number to be inspected (origin 1), and
 * the next characters (up to a control character, i.e. a character <= 32),
 * give the name of the register.  Thus:
 *
 *	printf("reg=%b\n", 3, "\10\2BITTWO\1BITONE\n");
 *
 * would produce output:
 *
 *	reg=3<BITTWO,BITONE>
 */

#include <sys/cdefs.h>
#include <sys/types.h>

/*
 * Note that stdarg.h and the ANSI style va_start macro is used for both
 * ANSI and traditional C compilers.
 */
#define KERNEL
#include <machine/stdarg.h>
#undef KERNEL

static void kprintn __P((u_long, int));

void
#if __STDC__
printf(const char *fmt, ...)
#else
printf(fmt /* , va_alist */)
	char *fmt;
#endif
{
	register char *p;
	register int ch, n;
	unsigned long ul;
	int lflag, set;
	va_list ap;

	va_start(ap, fmt);
	for (;;) {
		while ((ch = *fmt++) != '%') {
			if (ch == '\0')
				return;
			putchar(ch);
		}
		lflag = 0;
reswitch:	switch (ch = *fmt++) {
		case 'l':
			lflag = 1;
			goto reswitch;
		case 'b':
			ul = va_arg(ap, int);
			p = va_arg(ap, char *);
			kprintn(ul, *p++);

			if (!ul)
				break;

			for (set = 0; n = *p++;) {
				if (ul & (1 << (n - 1))) {
					putchar(set ? ',' : '<');
					for (; (n = *p) > ' '; ++p)
						putchar(n);
					set = 1;
				} else
					for (; *p > ' '; ++p);
			}
			if (set)
				putchar('>');
			break;
		case 'c':
			ch = va_arg(ap, int);
				putchar(ch & 0x7f);
			break;
		case 's':
			p = va_arg(ap, char *);
			while (ch = *p++)
				putchar(ch);
			break;
		case 'D':
			lflag = 1;
			/* FALLTHROUGH */
		case 'd':
			ul = lflag ?
			    va_arg(ap, long) : va_arg(ap, int);
			if ((long)ul < 0) {
				putchar('-');
				ul = -(long)ul;
			}
			kprintn(ul, 10);
			break;
		case 'O':
			lflag = 1;
			/* FALLTHROUGH */
		case 'o':
			ul = lflag ?
			    va_arg(ap, u_long) : va_arg(ap, u_int);
			kprintn(ul, 8);
			break;
		case 'U':
			lflag = 1;
			/* FALLTHROUGH */
		case 'u':
			ul = lflag ?
			    va_arg(ap, u_long) : va_arg(ap, u_int);
			kprintn(ul, 10);
			break;
		case 'X':
			lflag = 1;
			/* FALLTHROUGH */
		case 'x':
			ul = lflag ?
			    va_arg(ap, u_long) : va_arg(ap, u_int);
			kprintn(ul, 16);
			break;
		default:
			putchar('%');
			if (lflag)
				putchar('l');
			putchar(ch);
		}
	}
	va_end(ap);
}

static void
kprintn(ul, base)
	unsigned long ul;
	int base;
{
					/* hold a long in base 8 */
	char *p, buf[(sizeof(long) * NBBY / 3) + 1];

	p = buf;
	do {
		*p++ = "0123456789abcdef"[ul % base];
	} while (ul /= base);
	do {
		putchar(*--p);
	} while (p > buf);
}
