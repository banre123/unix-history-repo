/*-
 * Copyright (c) 1993 Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)dec_exec.h	7.2 (Berkeley) %G%
 */

/*
 * Portions of this file are subject to the following copyright notice:
 *
 * Copyright (C) 1989 Digital Equipment Corporation.
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies.  
 * Digital Equipment Corporation makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/*
 * /sprite/src/kernel/proc/ds3100.md/RCS/procMach.h,v 9.3 90/02/20 15:35:50
 * shirriff Exp $ SPRITE (Berkeley)
 */

/* Description of the COFF section. */
struct coff_exec {
#define	COFF_MAGIC	0x0162
	u_short	magic;		/* The magic number. */

	u_short	numSections;	/* The number of sections. */
	long	timeDateStamp;	/* Time and date stamp. */		
	long	symPtr;		/* File pointer to symbolic header. */	
	long	numSyms;	/* Size of symbolic header. */
	u_short	optHeader;	/* Size of optional header. */
	u_short	flags;		/* Flags. */

/* Description of the a.out section. */
	short	aout_magic;	/* Magic number. */

	short	verStamp;	/* Version stamp. */
	long	codeSize;	/* Code size in bytes. */
	long	heapSize;	/* Initialized data size in bytes. */
	long	bssSize;	/* Uninitialized data size in bytes. */
	long	entry;		/* Entry point. */
	long	codeStart;	/* Base of code used for this file. */
	long	heapStart;	/* Base of heap used for this file. */
	long	bssStart;	/* Base of bss used for this file. */
	long	gprMask;	/* General purpose register mask. */
	long	cprMask[4];	/* Co-processor register masks. */
	long	gpValue;	/* The gp value for this object. */
};

/* Section header. */
typedef struct {
	char	name[8];	/* Section name. */
	long	physAddr;	/* Section physical address. */
	long	virtAddr;	/* Section virtual address. */
	long	size;		/* Section size. */
	long	sectionPtr;	/* File pointer to section data. */
	long	relocPtr;	/* File pointer to relocation data. */
	long	lnnoPtr;	/* File pointer to gp tables. */
	u_short	numReloc;	/* Number of relocation entries. */
	u_short	numLnno;	/* Numberof gp tables. */
	long	flags;		/* Section flags. */
} ProcSectionHeader;
