/*
 * Streamer tape driver for 386bsd and FreeBSD.
 * Supports Archive QIC-02 and Wangtek QIC-02/QIC-36 boards.
 *
 * Copyright (C) 1993 by:
 *      Sergey Ryzhkov       <sir@kiae.su>
 *      Serge Vakulenko      <vak@zebub.msk.su>
 *
 * Version 1.1, Fri Sep 24 02:14:42 MSD 1993
 *
 * Placed in the public domain with NO WARRANTIES, not even the implied
 * warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Authors grant any other persons or organisations permission to use
 * or modify this software as long as this message is kept with the software,
 * all derivative works or modified versions.
 *
 * This driver is derived from the old 386bsd Wangtek streamer tape driver,
 * made by Robert Baron at CMU, based on Intel sources.
 * Authors thank Robert Baron, CMU and Intel and retain here
 * the original CMU copyright notice.
 */

/*
 * Copyright (c) 1989 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Authors: Robert Baron
 * 
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND 
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

/* ioctl for direct QIC commands */
#define WTQICMD         _IO('W', 0)

/* QIC-02 commands allowed for WTQICMD */
#define QIC_ERASE       0x22
#define QIC_RETENS      0x24

/* internal QIC-02 commands */
#define QIC_RDDATA      0x80    /* read data */
#define QIC_READFM      0xa0    /* read file mark */
#define QIC_WRTDATA     0x40    /* write data */
#define QIC_WRITEFM     0x60    /* write file mark */
#define QIC_RDSTAT      0xc0    /* read status command */
#define QIC_REWIND      0x21    /* rewind command (position+bot) */
#define QIC_FMT11       0x26    /* set format QIC-11 */
#define QIC_FMT24       0x27    /* set format QIC-24 */
#define QIC_FMT120      0x28    /* set format QIC-120 */
#define QIC_FMT150      0x29    /* set format QIC-150 */

/* tape driver flags */
#define TPINUSE         0x0001  /* tape is already open */
#define TPREAD          0x0002  /* tape is only open for reading */
#define TPWRITE         0x0004  /* tape is only open for writing */
#define TPSTART         0x0008  /* tape must be rewound and reset */
#define TPRMARK         0x0010  /* read file mark command outstanding */
#define TPWMARK         0x0020  /* write file mark command outstanding */
#define TPREW           0x0040  /* rewind command outstanding */
#define TPEXCEP         0x0080  /* i/o exception flag */
#define TPVOL           0x0100  /* read file mark or hit end of tape */
#define TPWO            0x0200  /* write command outstanding */
#define TPRO            0x0400  /* read command outstanding */
#define TPWANY          0x0800  /* write command requested */
#define TPRANY          0x1000  /* read command requested */
#define TPWP            0x2000  /* write protect error seen */
#define TPTIMER         0x4000  /* timer() is active */
#define TPACTIVE        0x8000  /* dma i/o active */

/* controller error register bits */
#define TP_FIL          0x0001  /* File mark detected */
#define TP_BNL          0x0002  /* Block not located */
#define TP_UDA          0x0004  /* Unrecoverable data error */
#define TP_EOM          0x0008  /* End of media */
#define TP_WRP          0x0010  /* Write protected cartridge */
#define TP_USL          0x0020  /* Unselected drive */
#define TP_CNI          0x0040  /* Cartridge not in place */
#define TP_ST0          0x0080  /* Status byte 0 bits */
#define TP_ST0MASK      0x00ff  /* Status byte 0 mask */
#define TP_POR          0x0100  /* Power on/reset occurred */
#define TP_RES1         0x0200  /* Reserved for end of media */
#define TP_RES2         0x0400  /* Reserved for bus parity */
#define TP_BOM          0x0800  /* Beginning of media */
#define	TP_MBD		0x1000	/* Marginal block detected */
#define	TP_NDT		0x2000	/* No data detected */
#define TP_ILL          0x4000  /* Illegal command - should not happen! */
#define	TP_ST1		0x8000	/* Status byte 1 bits */
#define TP_ST1MASK      0xff00  /* Status byte 1 mask */

/* formats for printing flags and error values */
#define WTDS_BITS "\20\1inuse\2read\3write\4start\5rmark\6wmark\7rew\10excep\11vol\12wo\13ro\14wany\15rany\16wp\17timer\20active"
#define WTER_BITS "\20\1eof\2bnl\3uda\4eom\5wrp\6usl\7cni\11por\12res1\13res2\14bom\15mbd\16ndt\17ill"
