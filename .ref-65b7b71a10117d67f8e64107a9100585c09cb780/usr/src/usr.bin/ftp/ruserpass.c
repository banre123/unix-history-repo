/*
 * Copyright (c) 1985, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 */

#ifndef lint
static char sccsid[] = "@(#)ruserpass.c	8.4 (Berkeley) %G%";
#endif /* not lint */

#include <sys/types.h>
#include <sys/stat.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ftp_var.h"

static	int token __P((void));
static	FILE *cfile;

#define	DEFAULT	1
#define	LOGIN	2
#define	PASSWD	3
#define	ACCOUNT 4
#define MACDEF  5
#define	ID	10
#define	MACH	11

static char tokval[100];

static struct toktab {
	char *tokstr;
	int tval;
} toktab[]= {
	{ "default",	DEFAULT },
	{ "login",	LOGIN },
	{ "password",	PASSWD },
	{ "passwd",	PASSWD },
	{ "account",	ACCOUNT },
	{ "machine",	MACH },
	{ "macdef",	MACDEF },
	{ NULL,		0 }
};

int
ruserpass(host, aname, apass, aacct)
	char *host, **aname, **apass, **aacct;
{
	char *hdir, buf[BUFSIZ], *tmp;
	char myname[MAXHOSTNAMELEN], *mydomain;
	int t, i, c, usedefault = 0;
	struct stat stb;

	hdir = getenv("HOME");
	if (hdir == NULL)
		hdir = ".";
	(void) sprintf(buf, "%s/.netrc", hdir);
	cfile = fopen(buf, "r");
	if (cfile == NULL) {
		if (errno != ENOENT)
			warn("%s", buf);
		return (0);
	}
	if (gethostname(myname, sizeof(myname)) < 0)
		myname[0] = '\0';
	if ((mydomain = strchr(myname, '.')) == NULL)
		mydomain = "";
next:
	while ((t = token())) switch(t) {

	case DEFAULT:
		usedefault = 1;
		/* FALL THROUGH */

	case MACH:
		if (!usedefault) {
			if (token() != ID)
				continue;
			/*
			 * Allow match either for user's input host name
			 * or official hostname.  Also allow match of 
			 * incompletely-specified host in local domain.
			 */
			if (strcasecmp(host, tokval) == 0)
				goto match;
			if (strcasecmp(hostname, tokval) == 0)
				goto match;
			if ((tmp = strchr(hostname, '.')) != NULL &&
			    strcasecmp(tmp, mydomain) == 0 &&
			    strncasecmp(hostname, tokval, tmp-hostname) == 0 &&
			    tokval[tmp - hostname] == '\0')
				goto match;
			if ((tmp = strchr(host, '.')) != NULL &&
			    strcasecmp(tmp, mydomain) == 0 &&
			    strncasecmp(host, tokval, tmp - host) == 0 &&
			    tokval[tmp - host] == '\0')
				goto match;
			continue;
		}
	match:
		while ((t = token()) && t != MACH && t != DEFAULT) switch(t) {

		case LOGIN:
			if (token())
				if (*aname == 0) { 
					*aname = malloc((unsigned) strlen(tokval) + 1);
					(void) strcpy(*aname, tokval);
				} else {
					if (strcmp(*aname, tokval))
						goto next;
				}
			break;
		case PASSWD:
			if ((*aname == NULL || strcmp(*aname, "anonymous")) &&
			    fstat(fileno(cfile), &stb) >= 0 &&
			    (stb.st_mode & 077) != 0) {
	warnx("Error: .netrc file is readable by others.");
	warnx("Remove password or make file unreadable by others.");
				goto bad;
			}
			if (token() && *apass == 0) {
				*apass = malloc((unsigned) strlen(tokval) + 1);
				(void) strcpy(*apass, tokval);
			}
			break;
		case ACCOUNT:
			if (fstat(fileno(cfile), &stb) >= 0
			    && (stb.st_mode & 077) != 0) {
	warnx("Error: .netrc file is readable by others.");
	warnx("Remove account or make file unreadable by others.");
				goto bad;
			}
			if (token() && *aacct == 0) {
				*aacct = malloc((unsigned) strlen(tokval) + 1);
				(void) strcpy(*aacct, tokval);
			}
			break;
		case MACDEF:
			if (proxy) {
				(void) fclose(cfile);
				return (0);
			}
			while ((c=getc(cfile)) != EOF && c == ' ' || c == '\t');
			if (c == EOF || c == '\n') {
				printf("Missing macdef name argument.\n");
				goto bad;
			}
			if (macnum == 16) {
				printf("Limit of 16 macros have already been defined\n");
				goto bad;
			}
			tmp = macros[macnum].mac_name;
			*tmp++ = c;
			for (i=0; i < 8 && (c=getc(cfile)) != EOF &&
			    !isspace(c); ++i) {
				*tmp++ = c;
			}
			if (c == EOF) {
				printf("Macro definition missing null line terminator.\n");
				goto bad;
			}
			*tmp = '\0';
			if (c != '\n') {
				while ((c=getc(cfile)) != EOF && c != '\n');
			}
			if (c == EOF) {
				printf("Macro definition missing null line terminator.\n");
				goto bad;
			}
			if (macnum == 0) {
				macros[macnum].mac_start = macbuf;
			}
			else {
				macros[macnum].mac_start = macros[macnum-1].mac_end + 1;
			}
			tmp = macros[macnum].mac_start;
			while (tmp != macbuf + 4096) {
				if ((c=getc(cfile)) == EOF) {
				printf("Macro definition missing null line terminator.\n");
					goto bad;
				}
				*tmp = c;
				if (*tmp == '\n') {
					if (*(tmp-1) == '\0') {
					   macros[macnum++].mac_end = tmp - 1;
					   break;
					}
					*tmp = '\0';
				}
				tmp++;
			}
			if (tmp == macbuf + 4096) {
				printf("4K macro buffer exceeded\n");
				goto bad;
			}
			break;
		default:
			warnx("Unknown .netrc keyword %s", tokval);
			break;
		}
		goto done;
	}
done:
	(void) fclose(cfile);
	return (0);
bad:
	(void) fclose(cfile);
	return (-1);
}

static int
token()
{
	char *cp;
	int c;
	struct toktab *t;

	if (feof(cfile) || ferror(cfile))
		return (0);
	while ((c = getc(cfile)) != EOF &&
	    (c == '\n' || c == '\t' || c == ' ' || c == ','))
		continue;
	if (c == EOF)
		return (0);
	cp = tokval;
	if (c == '"') {
		while ((c = getc(cfile)) != EOF && c != '"') {
			if (c == '\\')
				c = getc(cfile);
			*cp++ = c;
		}
	} else {
		*cp++ = c;
		while ((c = getc(cfile)) != EOF
		    && c != '\n' && c != '\t' && c != ' ' && c != ',') {
			if (c == '\\')
				c = getc(cfile);
			*cp++ = c;
		}
	}
	*cp = 0;
	if (tokval[0] == 0)
		return (0);
	for (t = toktab; t->tokstr; t++)
		if (!strcmp(t->tokstr, tokval))
			return (t->tval);
	return (ID);
}
