/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)mtree.h	5.1 (Berkeley) %G%
 */

typedef struct _id {
	struct _id *next;
	u_int id;
	u_long cnt;
} ID;

typedef struct _info {
#define	F_CKSUM	0x001				/* check sum */
#define	F_DMODE	0x002				/* directory mode */
#define	F_FMODE	0x004				/* directory mode */
#define	F_GROUP	0x008				/* group */
#define	F_MODE	0x010				/* directory mode */
#define	F_NLINK	0x020				/* number of links */
#define	F_OWNER	0x040				/* owner */
#define	F_SIZE	0x080				/* size */
#define	F_SLINK	0x100				/* link count */
#define	F_TYPE	0x200				/* file type */
	u_int	flags;				/* items set */

#define	F_BLOCK	0x001				/* block special */
#define	F_CHAR	0x002				/* char special */
#define	F_DIR	0x004				/* directory */
#define	F_FILE	0x008				/* regular file */
#define	F_LINK	0x010				/* symbolic link */
#define	F_SOCK	0x020				/* socket */
	u_int	type;				/* file type */

	uid_t	st_uid;
	gid_t	st_gid;
	off_t	st_size;
	mode_t	st_mode;
	nlink_t	st_nlink;
	u_long	cksum;				/* check sum */
	char	*slink;				/* symbolic link reference */
} INFO;

typedef struct _entry {
	struct _entry	*next, *child, *parent;	/* tree links */
	INFO	info;				/* node info structure */
#define	F_DONE	0x01				/* directory done */
#define	F_IGN	0x02				/* ignore this node on down */
#define	F_VISIT	0x04				/* visited this node */
	u_int	flags;				/* flags */
	char	*name;				/* node name */
	int	nlen;				/* name length */
} ENTRY;
