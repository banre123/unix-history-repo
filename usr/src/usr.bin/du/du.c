/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Newcomb.
 *
 * %sccs.include.redist.c%
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1989 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)du.c	5.19 (Berkeley) %G%";
#endif /* not lint */

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <dirent.h>
#include <stdio.h>
#include <fts.h>
#include <string.h>
#include <stdlib.h>

void	 err __P((const char *, ...));
char	*getbsize __P((char *, int *, long *));
int	 linkchk __P((FTSENT *));
void	 usage __P((void));

main(argc, argv)
	int argc;
	char *argv[];
{
	register FTS *fts;
	register FTSENT *p;
	register int listdirs, listfiles;
	long blocksize;
	int aflag, ch, ftsoptions, notused, sflag;
	char **save;

	ftsoptions = FTS_PHYSICAL;
	save = argv;
	aflag = sflag = 0;
	while ((ch = getopt(argc, argv, "Hahsx")) != EOF)
		switch(ch) {
		case 'H':
			ftsoptions |= FTS_COMFOLLOW;
			break;
		case 'a':
			aflag = 1;
			break;
		case 'h':
			ftsoptions &= ~FTS_PHYSICAL;
			ftsoptions |= FTS_LOGICAL;
			break;
		case 's':
			sflag = 1;
			break;
		case 'x':
			ftsoptions |= FTS_XDEV;
			break;
		case '?':
		default:
			usage();
		}
	argv += optind;

	if (aflag) {
		if (sflag)
			usage();
		listdirs = listfiles = 1;
	} else if (sflag)
		listdirs = listfiles = 0;
	else {
		listfiles = 0;
		listdirs = 1;
	}

	if (!*argv) {
		argv = save;
		argv[0] = ".";
		argv[1] = NULL;
	}

	(void)getbsize("du", &notused, &blocksize);
	blocksize /= 512;

	if ((fts = fts_open(argv, ftsoptions, NULL)) == NULL)
		err("%s", strerror(errno));

	while (p = fts_read(fts))
		switch(p->fts_info) {
		case FTS_D:
			break;
		case FTS_DP:
			p->fts_parent->fts_number += 
			    p->fts_number += p->fts_statp->st_blocks;
			/*
			 * If listing each directory, or not listing files
			 * or directories and this is post-order of the
			 * root of a traversal, display the total.
			 */
			if (listdirs || !listfiles && !p->fts_level)
				(void)printf("%ld\t%s\n",
				    howmany(p->fts_number, blocksize),
				    p->fts_path);
			break;
		case FTS_DNR:
		case FTS_ERR:
		case FTS_NS:
			(void)fprintf(stderr,
			    "du: %s: %s\n", p->fts_path, strerror(errno));
			break;
		default:
			if (p->fts_statp->st_nlink > 1 && linkchk(p))
				break;
			/*
			 * If listing each file, or a non-directory file was
			 * the root of a traversal, display the total.
			 */
			if (listfiles || !p->fts_level)
				(void)printf("%qd\t%s\n",
				    howmany(p->fts_statp->st_blocks, blocksize),
				    p->fts_path);
			p->fts_parent->fts_number += p->fts_statp->st_blocks;
		}
	if (errno)
		err("%s", strerror(errno));
	exit(0);
}

typedef struct _ID {
	dev_t	dev;
	ino_t	inode;
} ID;

int
linkchk(p)
	register FTSENT *p;
{
	static ID *files;
	static int maxfiles, nfiles;
	register ID *fp, *start;
	register ino_t ino;
	register dev_t dev;

	ino = p->fts_statp->st_ino;
	dev = p->fts_statp->st_dev;
	if (start = files)
		for (fp = start + nfiles - 1; fp >= start; --fp)
			if (ino == fp->inode && dev == fp->dev)
				return(1);

	if (nfiles == maxfiles && (files = realloc((char *)files,
	    (u_int)(sizeof(ID) * (maxfiles += 128)))) == NULL)
		err("%s", strerror(errno));
	files[nfiles].inode = ino;
	files[nfiles].dev = dev;
	++nfiles;
	return(0);
}

void
usage()
{
	(void)fprintf(stderr, "usage: du [-a | -s] [-Hhx] [file ...]\n");
	exit(1);
}

#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

void
#if __STDC__
err(const char *fmt, ...)
#else
err(fmt, va_alist)
	char *fmt;
        va_dcl
#endif
{
	va_list ap;
#if __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
#endif
	(void)fprintf(stderr, "du: ");
	(void)vfprintf(stderr, fmt, ap);
	va_end(ap);
	(void)fprintf(stderr, "\n");
	exit(1);
	/* NOTREACHED */
}
