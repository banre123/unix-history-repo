/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Mike Karels at Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)sysctl.h	8.1 (Berkeley) 6/2/93
 * $Id: sysctl.h,v 1.36 1995/12/02 20:40:20 phk Exp $
 */

#ifndef _SYS_SYSCTL_H_
#define	_SYS_SYSCTL_H_

/*
 * Definitions for sysctl call.  The sysctl call uses a hierarchical name
 * for objects that can be examined or modified.  The name is expressed as
 * a sequence of integers.  Like a file path name, the meaning of each
 * component depends on its place in the hierarchy.  The top-level and kern
 * identifiers are defined here, and other identifiers are defined in the
 * respective subsystem header files.
 */

#define CTL_MAXNAME	12	/* largest number of components supported */

/*
 * Each subsystem defined by sysctl defines a list of variables
 * for that subsystem. Each name is either a node with further
 * levels defined below it, or it is a leaf of some particular
 * type given below. Each sysctl level defines a set of name/type
 * pairs to be used by sysctl(1) in manipulating the subsystem.
 */
struct ctlname {
	char	*ctl_name;	/* subsystem name */
	int	ctl_type;	/* type of name */
};

#define CTLTYPE		0xf	/* Mask for the type */
#define	CTLTYPE_NODE	1	/* name is a node */
#define	CTLTYPE_INT	2	/* name describes an integer */
#define	CTLTYPE_STRING	3	/* name describes a string */
#define	CTLTYPE_QUAD	4	/* name describes a 64-bit number */
#define	CTLTYPE_OPAQUE	5	/* name describes a structure */
#define	CTLTYPE_STRUCT	CTLTYPE_OPAQUE	/* name describes a structure */

#define CTLFLAG_RD	0x80000000	/* Allow reads of variable */
#define CTLFLAG_WR	0x40000000	/* Allow writes to the variable */
#define CTLFLAG_RW	(CTLFLAG_RD|CTLFLAG_WR)
#define CTLFLAG_NOLOCK	0x20000000	/* XXX Don't Lock */

#define OID_AUTO	(-1)

#ifdef KERNEL
#define SYSCTL_HANDLER_ARGS (struct sysctl_oid *oidp, void *arg1, int arg2, \
	struct sysctl_req *req)

/*
 * This describes the access space for a sysctl request.  This is needed
 * so that we can use the interface from the kernel or from user-space.
 */
struct sysctl_req {
	struct proc	*p;
	int		lock;
	void		*oldptr;
	int		oldlen;
	int		oldidx;
	int		(*oldfunc)(struct sysctl_req *, void *, int);
	void		*newptr;
	int		newlen;
	int		newidx;
	int		(*newfunc)(struct sysctl_req *, void *, int);
};

/*
 * This describes one "oid" in the MIB tree.  Potentially more nodes can
 * be hidden behind it, expanded by the handler.
 */
struct sysctl_oid {
	int		oid_number;
	int		oid_kind;
	void		*oid_arg1;
	int		oid_arg2;
	const char	*oid_name;
	int 		(*oid_handler) SYSCTL_HANDLER_ARGS;
	const char	*oid_fmt;
};

#define SYSCTL_IN(r, p, l) (r->newfunc)(r, p, l)
#define SYSCTL_OUT(r, p, l) (r->oldfunc)(r, p, l)

int sysctl_handle_int SYSCTL_HANDLER_ARGS;
int sysctl_handle_string SYSCTL_HANDLER_ARGS;
int sysctl_handle_opaque SYSCTL_HANDLER_ARGS;

/* This is the "raw" function for a mib-oid */
#define SYSCTL_OID(parent, nbr, name, kind, a1, a2, handler, fmt, descr) \
	static const struct sysctl_oid sysctl__##parent##_##name = { \
		nbr, kind, a1, a2, #name, handler, fmt }; \
	TEXT_SET(sysctl_##parent, sysctl__##parent##_##name);

/* This makes a node from which other oids can hang */
#define SYSCTL_NODE(parent, nbr, name, access, handler, descr) \
	extern struct linker_set sysctl_##parent##_##name; \
	SYSCTL_OID(parent, nbr, name, CTLTYPE_NODE|access, \
		(void*)&sysctl_##parent##_##name, 0, handler, "N", descr); \
	TEXT_SET(sysctl_##parent##_##name, sysctl__##parent##_##name);

/* This is a string len can be 0 to indicate '\0' termination */
#define SYSCTL_STRING(parent, nbr, name, access, arg, len, descr) \
	SYSCTL_OID(parent, nbr, name, CTLTYPE_STRING|access, \
		arg, len, sysctl_handle_string, "A", descr);

/* This is a integer, if ptr is NULL, val is returned */
#define SYSCTL_INT(parent, nbr, name, access, ptr, val, descr) \
	SYSCTL_OID(parent, nbr, name, CTLTYPE_INT|access, \
		ptr, val, sysctl_handle_int, "I", descr);

/* This is anything, specified by a pointer and a lenth */
#define SYSCTL_OPAQUE(parent, nbr, name, access, ptr, len, descr) \
	SYSCTL_OID(parent, nbr, name, CTLTYPE_OPAQUE|access, \
		ptr, len, sysctl_handle_opaque, "O", descr);

/* This is a struct, specified by a pointer and type */
#define SYSCTL_STRUCT(parent, nbr, name, access, ptr, type, descr) \
	SYSCTL_OID(parent, nbr, name, CTLTYPE_OPAQUE|access, \
		ptr, sizeof(struct type), sysctl_handle_opaque, \
		"S," #type, descr);

/* Needs a proc.  Specify by pointer and arg */
#define SYSCTL_PROC(parent, nbr, name, access, ptr, arg, handler, fmt, descr) \
	SYSCTL_OID(parent, nbr, name, access, \
		ptr, arg, handler, fmt, descr);
#endif /* KERNEL */

/*
 * Top-level identifiers
 */
#define	CTL_UNSPEC	0		/* unused */
#define	CTL_KERN	1		/* "high kernel": proc, limits */
#define	CTL_VM		2		/* virtual memory */
#define	CTL_FS		3		/* file system, mount type is next */
#define	CTL_NET		4		/* network, see socket.h */
#define	CTL_DEBUG	5		/* debugging parameters */
#define	CTL_HW		6		/* generic cpu/io */
#define	CTL_MACHDEP	7		/* machine dependent */
#define	CTL_USER	8		/* user-level */
#define	CTL_MAXID	9		/* number of valid top-level ids */

#define CTL_NAMES { \
	{ 0, 0 }, \
	{ "kern", CTLTYPE_NODE }, \
	{ "vm", CTLTYPE_NODE }, \
	{ "fs", CTLTYPE_NODE }, \
	{ "net", CTLTYPE_NODE }, \
	{ "debug", CTLTYPE_NODE }, \
	{ "hw", CTLTYPE_NODE }, \
	{ "machdep", CTLTYPE_NODE }, \
	{ "user", CTLTYPE_NODE }, \
}

/*
 * CTL_KERN identifiers
 */
#define	KERN_OSTYPE	 	 1	/* string: system version */
#define	KERN_OSRELEASE	 	 2	/* string: system release */
#define	KERN_OSREV	 	 3	/* int: system revision */
#define	KERN_VERSION	 	 4	/* string: compile time info */
#define	KERN_MAXVNODES	 	 5	/* int: max vnodes */
#define	KERN_MAXPROC	 	 6	/* int: max processes */
#define	KERN_MAXFILES	 	 7	/* int: max open files */
#define	KERN_ARGMAX	 	 8	/* int: max arguments to exec */
#define	KERN_SECURELVL	 	 9	/* int: system security level */
#define	KERN_HOSTNAME		10	/* string: hostname */
#define	KERN_HOSTID		11	/* int: host identifier */
#define	KERN_CLOCKRATE		12	/* struct: struct clockrate */
#define	KERN_VNODE		13	/* struct: vnode structures */
#define	KERN_PROC		14	/* struct: process entries */
#define	KERN_FILE		15	/* struct: file entries */
#define	KERN_PROF		16	/* node: kernel profiling info */
#define	KERN_POSIX1		17	/* int: POSIX.1 version */
#define	KERN_NGROUPS		18	/* int: # of supplemental group ids */
#define	KERN_JOB_CONTROL	19	/* int: is job control available */
#define	KERN_SAVED_IDS		20	/* int: saved set-user/group-ID */
#define	KERN_BOOTTIME		21	/* struct: time kernel was booted */
#define KERN_DOMAINNAME		22	/* string: YP domain name */
#define KERN_UPDATEINTERVAL	23	/* int: update process sleep time */
#define KERN_OSRELDATE		24	/* int: OS release date */
#define KERN_NTP_PLL		25	/* node: NTP PLL control */
#define	KERN_BOOTFILE		26	/* string: name of booted kernel */
#define	KERN_MAXFILESPERPROC	27	/* int: max open files per proc */
#define	KERN_MAXPROCPERUID 	28	/* int: max processes per uid */
#define KERN_DUMPDEV		29	/* dev_t: device to dump on */
#define	KERN_SOMAXCONN		30	/* int: max connections in listen q */
#define	KERN_MAXSOCKBUF		31	/* int: max size of a socket buffer */
#define KERN_MAXID		32      /* number of valid kern ids */

#define CTL_KERN_NAMES { \
	{ 0, 0 }, \
	{ "ostype", CTLTYPE_STRING }, \
	{ "osrelease", CTLTYPE_STRING }, \
	{ "osrevision", CTLTYPE_INT }, \
	{ "version", CTLTYPE_STRING }, \
	{ "maxvnodes", CTLTYPE_INT }, \
	{ "maxproc", CTLTYPE_INT }, \
	{ "maxfiles", CTLTYPE_INT }, \
	{ "argmax", CTLTYPE_INT }, \
	{ "securelevel", CTLTYPE_INT }, \
	{ "hostname", CTLTYPE_STRING }, \
	{ "hostid", CTLTYPE_INT }, \
	{ "clockrate", CTLTYPE_STRUCT }, \
	{ "vnode", CTLTYPE_STRUCT }, \
	{ "proc", CTLTYPE_STRUCT }, \
	{ "file", CTLTYPE_STRUCT }, \
	{ "profiling", CTLTYPE_NODE }, \
	{ "posix1version", CTLTYPE_INT }, \
	{ "ngroups", CTLTYPE_INT }, \
	{ "job_control", CTLTYPE_INT }, \
	{ "saved_ids", CTLTYPE_INT }, \
	{ "boottime", CTLTYPE_STRUCT }, \
	{ "domainname", CTLTYPE_STRING }, \
	{ "update", CTLTYPE_INT }, \
	{ "osreldate", CTLTYPE_INT }, \
        { "ntp_pll", CTLTYPE_NODE }, \
	{ "bootfile", CTLTYPE_STRING }, \
	{ "maxfilesperproc", CTLTYPE_INT }, \
	{ "maxprocperuid", CTLTYPE_INT }, \
	{ "dumpdev", CTLTYPE_STRUCT }, /* we lie; don't print as int */ \
	{ "somaxconn", CTLTYPE_INT }, \
	{ "maxsockbuf", CTLTYPE_INT }, \
}

/*
 * CTL_FS identifiers
 */
#define FS_VFSCONF		0	/* get configured filesystems */
#define FS_MAXID		1	/* number of items */

#define CTL_FS_NAMES { \
	{ "vfsconf", CTLTYPE_STRUCT }, \
}

/*
 * KERN_PROC subtypes
 */
#define KERN_PROC_ALL		0	/* everything */
#define	KERN_PROC_PID		1	/* by process id */
#define	KERN_PROC_PGRP		2	/* by process group id */
#define	KERN_PROC_SESSION	3	/* by session of pid */
#define	KERN_PROC_TTY		4	/* by controlling tty */
#define	KERN_PROC_UID		5	/* by effective uid */
#define	KERN_PROC_RUID		6	/* by real uid */

/*
 * CTL_HW identifiers
 */
#define	HW_MACHINE	 1		/* string: machine class */
#define	HW_MODEL	 2		/* string: specific machine model */
#define	HW_NCPU		 3		/* int: number of cpus */
#define	HW_BYTEORDER	 4		/* int: machine byte order */
#define	HW_PHYSMEM	 5		/* int: total memory */
#define	HW_USERMEM	 6		/* int: non-kernel memory */
#define	HW_PAGESIZE	 7		/* int: software page size */
#define	HW_DISKNAMES	 8		/* strings: disk drive names */
#define	HW_DISKSTATS	 9		/* struct: diskstats[] */
#define HW_FLOATINGPT	10		/* int: has HW floating point? */
#define HW_DEVCONF	11		/* node: device configuration */
#define	HW_MAXID	12		/* number of valid hw ids */

#define CTL_HW_NAMES { \
	{ 0, 0 }, \
	{ "machine", CTLTYPE_STRING }, \
	{ "model", CTLTYPE_STRING }, \
	{ "ncpu", CTLTYPE_INT }, \
	{ "byteorder", CTLTYPE_INT }, \
	{ "physmem", CTLTYPE_INT }, \
	{ "usermem", CTLTYPE_INT }, \
	{ "pagesize", CTLTYPE_INT }, \
	{ "disknames", CTLTYPE_STRUCT }, \
	{ "diskstats", CTLTYPE_STRUCT }, \
	{ "floatingpoint", CTLTYPE_INT }, \
	{ "devconf", CTLTYPE_NODE }, \
}

/*
 * CTL_USER definitions
 */
#define	USER_CS_PATH		 1	/* string: _CS_PATH */
#define	USER_BC_BASE_MAX	 2	/* int: BC_BASE_MAX */
#define	USER_BC_DIM_MAX		 3	/* int: BC_DIM_MAX */
#define	USER_BC_SCALE_MAX	 4	/* int: BC_SCALE_MAX */
#define	USER_BC_STRING_MAX	 5	/* int: BC_STRING_MAX */
#define	USER_COLL_WEIGHTS_MAX	 6	/* int: COLL_WEIGHTS_MAX */
#define	USER_EXPR_NEST_MAX	 7	/* int: EXPR_NEST_MAX */
#define	USER_LINE_MAX		 8	/* int: LINE_MAX */
#define	USER_RE_DUP_MAX		 9	/* int: RE_DUP_MAX */
#define	USER_POSIX2_VERSION	10	/* int: POSIX2_VERSION */
#define	USER_POSIX2_C_BIND	11	/* int: POSIX2_C_BIND */
#define	USER_POSIX2_C_DEV	12	/* int: POSIX2_C_DEV */
#define	USER_POSIX2_CHAR_TERM	13	/* int: POSIX2_CHAR_TERM */
#define	USER_POSIX2_FORT_DEV	14	/* int: POSIX2_FORT_DEV */
#define	USER_POSIX2_FORT_RUN	15	/* int: POSIX2_FORT_RUN */
#define	USER_POSIX2_LOCALEDEF	16	/* int: POSIX2_LOCALEDEF */
#define	USER_POSIX2_SW_DEV	17	/* int: POSIX2_SW_DEV */
#define	USER_POSIX2_UPE		18	/* int: POSIX2_UPE */
#define	USER_STREAM_MAX		19	/* int: POSIX2_STREAM_MAX */
#define	USER_TZNAME_MAX		20	/* int: POSIX2_TZNAME_MAX */
#define	USER_MAXID		21	/* number of valid user ids */

#define	CTL_USER_NAMES { \
	{ 0, 0 }, \
	{ "cs_path", CTLTYPE_STRING }, \
	{ "bc_base_max", CTLTYPE_INT }, \
	{ "bc_dim_max", CTLTYPE_INT }, \
	{ "bc_scale_max", CTLTYPE_INT }, \
	{ "bc_string_max", CTLTYPE_INT }, \
	{ "coll_weights_max", CTLTYPE_INT }, \
	{ "expr_nest_max", CTLTYPE_INT }, \
	{ "line_max", CTLTYPE_INT }, \
	{ "re_dup_max", CTLTYPE_INT }, \
	{ "posix2_version", CTLTYPE_INT }, \
	{ "posix2_c_bind", CTLTYPE_INT }, \
	{ "posix2_c_dev", CTLTYPE_INT }, \
	{ "posix2_char_term", CTLTYPE_INT }, \
	{ "posix2_fort_dev", CTLTYPE_INT }, \
	{ "posix2_fort_run", CTLTYPE_INT }, \
	{ "posix2_localedef", CTLTYPE_INT }, \
	{ "posix2_sw_dev", CTLTYPE_INT }, \
	{ "posix2_upe", CTLTYPE_INT }, \
	{ "stream_max", CTLTYPE_INT }, \
	{ "tzname_max", CTLTYPE_INT }, \
}

#ifdef KERNEL

extern char	cpu_model[];
extern char	machine[];
extern char	osrelease[];
extern char	ostype[];

int userland_sysctl(struct proc *p, int *name, u_int namelen, void *old, size_t *oldlenp, int inkernel, void *new, size_t newlen, int *retval);
/*
int	sysctl_clockrate __P((char *, size_t*));
int	sysctl_file __P((char *, size_t*));
int	sysctl_doproc __P((int *, u_int, char *, size_t*));
int	sysctl_doprof __P((int *, u_int, void *, size_t *, void *, size_t));
*/

#else	/* !KERNEL */
#include <sys/cdefs.h>

__BEGIN_DECLS
int	sysctl __P((int *, u_int, void *, size_t *, void *, size_t));
__END_DECLS
#endif	/* KERNEL */

#endif	/* !_SYS_SYSCTL_H_ */
