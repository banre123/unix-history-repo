/*	kdb_output.c	7.1	86/11/20	*/

#include "../kdb/defs.h"

long	maxpos;
int	radix = 16;

char	printbuf[MAXLIN];
char	*printptr = printbuf;
char	*digitptr;

printc(c)
	char c;
{
	char d;
	register char *q;
	register posn, tabs, p;

	if (mkfault)
		return;
	if ((*printptr=c)==EOR) {
		tabs=0; posn=0; q=printbuf;
		for (p=0; p<printptr-printbuf; p++) {
			d=printbuf[p];
			if ((p&7)==0 && posn) {
				tabs++;
				posn=0;
			}
			if (d!=SP) {
				while (tabs>0)
					*q++=TB, tabs--;
				while (posn>0)
					*q++=SP, posn--;
				*q++=d;
			} else
				posn++;
		 }
		 *q++=EOR;
		 kdbwrite(printbuf,q-printbuf);
		 printptr=printbuf;
	} else if (c==TB) {
		*printptr++=SP;
		while ((printptr-printbuf)&7)
			*printptr++=SP;
	} else if (c)
		printptr++;
	if (printptr >= &printbuf[MAXLIN-9]) {
		kdbwrite(printbuf, printptr - printbuf);
		printptr = printbuf;
	}
}

charpos()
{

	return (printptr-printbuf);
}

flushbuf()
{

	if (printptr!=printbuf)
		printc(EOR);
}

/* VARARGS1 */
printf(fmat,a1)
	char *fmat, *a1;
{
	char *fptr;
	register char *s;
	register long *dptr;
	double *rptr;
	register width, prec;
	char c, adj;
	int x, n;
	register long lx;
	char digits[64];

	fptr = fmat; dptr = (long *)&a1;
	while (c = *fptr++) {
		if (c!='%') {
			printc(c);
			continue;
		}
		if (*fptr=='-') {
			adj='l'; fptr++;
		} else
			adj='r';
		width=convert(&fptr);
		if (*fptr=='.') {
			fptr++; prec=convert(&fptr);
		} else
			prec = -1;
		digitptr=digits;
		rptr=(double *)dptr; x = lx = *dptr++;
		s=0;
		switch (c = *fptr++) {
		case 'd':
			printnum(x, -10); break;
		case 'u':
			printnum((unsigned short)x, 10); break;
		case 'o':
			printnum((unsigned short)x, 8); break;
		case 'q':
			printnum(x, -8); break;
		case 'x':
			printnum((unsigned short)x, 16); break;
		case 'z':
			printnum((unsigned short)x, -16); break;
		case 'R':
			printnum(lx, radix); break;
		case 'D':
			printnum(lx, -10); break;
		case 'U':
			printnum(lx, 10); break;
		case 'O':
			printnum(lx, 8); break;
		case 'Q':
			printnum(lx, -8); break;
		case 'X':
			printnum(lx, 16); break;
		case 'Z':
			printnum(lx, -16); break;
		case 'c':
			printc(x); break;
		case 's':
			s=(char *)lx; break;
		case 'm':
			break;
		case 'M':
			width=x; break;
		case 'T': case 't':
			if (c=='T')
				width=x;
			else
				dptr--;
			if (width)
				width -= charpos()%width;
			break;
		default:
			printc(c); dptr--;
			break;
		}
		if (s==0) {
			*digitptr=0; s=digits;
		}
		n=strlen(s);
		n=(prec<n && prec>=0 ? prec : n);
		width -= n;
		if (adj=='r')
			while (width-- > 0)
				printc(SP);
		while (n--)
			printc(*s++);
		while (width-- > 0)
			printc(SP);
		digitptr=digits;
	}
}

static
convert(cp)
	register char **cp;
{
	register char c;
	int n;

	n=0;
	while (((c = *(*cp)++)>='0') && c<='9')
		n=n*10+c-'0';
	(*cp)--;
	return (n);
}

static
printnum(n, base)
	register POS n;
{
	register char *dptr;
	char digs[15];

	dptr=digs;
	if (base<0) {
		base = -base;
		if ((long)n<0) {
			n = -n;
			*digitptr++ = '-';
		}
	}
	while (n) {
		*dptr++ = n%base;
		n /= base;
	}
	if (dptr==digs)
		*dptr++=0;
	while (dptr!=digs) {
		n = *--dptr;
		*digitptr++ = (n+(n<=9 ? '0' : 'a'-10));
	}
}

endline()
{

	if (maxpos <= charpos())
		printf("\n");
}
