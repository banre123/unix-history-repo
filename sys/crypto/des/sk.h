/*	$FreeBSD$	*/
/*	$KAME: sk.h,v 1.4 2001/09/10 04:03:58 itojun Exp $	*/

/* crypto/des/sk.h */
/* Copyright (C) 1995-1996 Eric Young (eay@mincom.oz.au)
 * All rights reserved.
 *
 * This file is part of an SSL implementation written
 * by Eric Young (eay@mincom.oz.au).
 * The implementation was written so as to conform with Netscapes SSL
 * specification.  This library and applications are
 * FREE FOR COMMERCIAL AND NON-COMMERCIAL USE
 * as long as the following conditions are aheared to.
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.  If this code is used in a product,
 * Eric Young should be given attribution as the author of the parts used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Eric Young (eay@mincom.oz.au)
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
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
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

static const DES_LONG des_skb[8][64]={
{
/* for C bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
0x00000000L,0x00000010L,0x20000000L,0x20000010L,
0x00010000L,0x00010010L,0x20010000L,0x20010010L,
0x00000800L,0x00000810L,0x20000800L,0x20000810L,
0x00010800L,0x00010810L,0x20010800L,0x20010810L,
0x00000020L,0x00000030L,0x20000020L,0x20000030L,
0x00010020L,0x00010030L,0x20010020L,0x20010030L,
0x00000820L,0x00000830L,0x20000820L,0x20000830L,
0x00010820L,0x00010830L,0x20010820L,0x20010830L,
0x00080000L,0x00080010L,0x20080000L,0x20080010L,
0x00090000L,0x00090010L,0x20090000L,0x20090010L,
0x00080800L,0x00080810L,0x20080800L,0x20080810L,
0x00090800L,0x00090810L,0x20090800L,0x20090810L,
0x00080020L,0x00080030L,0x20080020L,0x20080030L,
0x00090020L,0x00090030L,0x20090020L,0x20090030L,
0x00080820L,0x00080830L,0x20080820L,0x20080830L,
0x00090820L,0x00090830L,0x20090820L,0x20090830L,
},{
/* for C bits (numbered as per FIPS 46) 7 8 10 11 12 13 */
0x00000000L,0x02000000L,0x00002000L,0x02002000L,
0x00200000L,0x02200000L,0x00202000L,0x02202000L,
0x00000004L,0x02000004L,0x00002004L,0x02002004L,
0x00200004L,0x02200004L,0x00202004L,0x02202004L,
0x00000400L,0x02000400L,0x00002400L,0x02002400L,
0x00200400L,0x02200400L,0x00202400L,0x02202400L,
0x00000404L,0x02000404L,0x00002404L,0x02002404L,
0x00200404L,0x02200404L,0x00202404L,0x02202404L,
0x10000000L,0x12000000L,0x10002000L,0x12002000L,
0x10200000L,0x12200000L,0x10202000L,0x12202000L,
0x10000004L,0x12000004L,0x10002004L,0x12002004L,
0x10200004L,0x12200004L,0x10202004L,0x12202004L,
0x10000400L,0x12000400L,0x10002400L,0x12002400L,
0x10200400L,0x12200400L,0x10202400L,0x12202400L,
0x10000404L,0x12000404L,0x10002404L,0x12002404L,
0x10200404L,0x12200404L,0x10202404L,0x12202404L,
},{
/* for C bits (numbered as per FIPS 46) 14 15 16 17 19 20 */
0x00000000L,0x00000001L,0x00040000L,0x00040001L,
0x01000000L,0x01000001L,0x01040000L,0x01040001L,
0x00000002L,0x00000003L,0x00040002L,0x00040003L,
0x01000002L,0x01000003L,0x01040002L,0x01040003L,
0x00000200L,0x00000201L,0x00040200L,0x00040201L,
0x01000200L,0x01000201L,0x01040200L,0x01040201L,
0x00000202L,0x00000203L,0x00040202L,0x00040203L,
0x01000202L,0x01000203L,0x01040202L,0x01040203L,
0x08000000L,0x08000001L,0x08040000L,0x08040001L,
0x09000000L,0x09000001L,0x09040000L,0x09040001L,
0x08000002L,0x08000003L,0x08040002L,0x08040003L,
0x09000002L,0x09000003L,0x09040002L,0x09040003L,
0x08000200L,0x08000201L,0x08040200L,0x08040201L,
0x09000200L,0x09000201L,0x09040200L,0x09040201L,
0x08000202L,0x08000203L,0x08040202L,0x08040203L,
0x09000202L,0x09000203L,0x09040202L,0x09040203L,
},{
/* for C bits (numbered as per FIPS 46) 21 23 24 26 27 28 */
0x00000000L,0x00100000L,0x00000100L,0x00100100L,
0x00000008L,0x00100008L,0x00000108L,0x00100108L,
0x00001000L,0x00101000L,0x00001100L,0x00101100L,
0x00001008L,0x00101008L,0x00001108L,0x00101108L,
0x04000000L,0x04100000L,0x04000100L,0x04100100L,
0x04000008L,0x04100008L,0x04000108L,0x04100108L,
0x04001000L,0x04101000L,0x04001100L,0x04101100L,
0x04001008L,0x04101008L,0x04001108L,0x04101108L,
0x00020000L,0x00120000L,0x00020100L,0x00120100L,
0x00020008L,0x00120008L,0x00020108L,0x00120108L,
0x00021000L,0x00121000L,0x00021100L,0x00121100L,
0x00021008L,0x00121008L,0x00021108L,0x00121108L,
0x04020000L,0x04120000L,0x04020100L,0x04120100L,
0x04020008L,0x04120008L,0x04020108L,0x04120108L,
0x04021000L,0x04121000L,0x04021100L,0x04121100L,
0x04021008L,0x04121008L,0x04021108L,0x04121108L,
},{
/* for D bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
0x00000000L,0x10000000L,0x00010000L,0x10010000L,
0x00000004L,0x10000004L,0x00010004L,0x10010004L,
0x20000000L,0x30000000L,0x20010000L,0x30010000L,
0x20000004L,0x30000004L,0x20010004L,0x30010004L,
0x00100000L,0x10100000L,0x00110000L,0x10110000L,
0x00100004L,0x10100004L,0x00110004L,0x10110004L,
0x20100000L,0x30100000L,0x20110000L,0x30110000L,
0x20100004L,0x30100004L,0x20110004L,0x30110004L,
0x00001000L,0x10001000L,0x00011000L,0x10011000L,
0x00001004L,0x10001004L,0x00011004L,0x10011004L,
0x20001000L,0x30001000L,0x20011000L,0x30011000L,
0x20001004L,0x30001004L,0x20011004L,0x30011004L,
0x00101000L,0x10101000L,0x00111000L,0x10111000L,
0x00101004L,0x10101004L,0x00111004L,0x10111004L,
0x20101000L,0x30101000L,0x20111000L,0x30111000L,
0x20101004L,0x30101004L,0x20111004L,0x30111004L,
},{
/* for D bits (numbered as per FIPS 46) 8 9 11 12 13 14 */
0x00000000L,0x08000000L,0x00000008L,0x08000008L,
0x00000400L,0x08000400L,0x00000408L,0x08000408L,
0x00020000L,0x08020000L,0x00020008L,0x08020008L,
0x00020400L,0x08020400L,0x00020408L,0x08020408L,
0x00000001L,0x08000001L,0x00000009L,0x08000009L,
0x00000401L,0x08000401L,0x00000409L,0x08000409L,
0x00020001L,0x08020001L,0x00020009L,0x08020009L,
0x00020401L,0x08020401L,0x00020409L,0x08020409L,
0x02000000L,0x0A000000L,0x02000008L,0x0A000008L,
0x02000400L,0x0A000400L,0x02000408L,0x0A000408L,
0x02020000L,0x0A020000L,0x02020008L,0x0A020008L,
0x02020400L,0x0A020400L,0x02020408L,0x0A020408L,
0x02000001L,0x0A000001L,0x02000009L,0x0A000009L,
0x02000401L,0x0A000401L,0x02000409L,0x0A000409L,
0x02020001L,0x0A020001L,0x02020009L,0x0A020009L,
0x02020401L,0x0A020401L,0x02020409L,0x0A020409L,
},{
/* for D bits (numbered as per FIPS 46) 16 17 18 19 20 21 */
0x00000000L,0x00000100L,0x00080000L,0x00080100L,
0x01000000L,0x01000100L,0x01080000L,0x01080100L,
0x00000010L,0x00000110L,0x00080010L,0x00080110L,
0x01000010L,0x01000110L,0x01080010L,0x01080110L,
0x00200000L,0x00200100L,0x00280000L,0x00280100L,
0x01200000L,0x01200100L,0x01280000L,0x01280100L,
0x00200010L,0x00200110L,0x00280010L,0x00280110L,
0x01200010L,0x01200110L,0x01280010L,0x01280110L,
0x00000200L,0x00000300L,0x00080200L,0x00080300L,
0x01000200L,0x01000300L,0x01080200L,0x01080300L,
0x00000210L,0x00000310L,0x00080210L,0x00080310L,
0x01000210L,0x01000310L,0x01080210L,0x01080310L,
0x00200200L,0x00200300L,0x00280200L,0x00280300L,
0x01200200L,0x01200300L,0x01280200L,0x01280300L,
0x00200210L,0x00200310L,0x00280210L,0x00280310L,
0x01200210L,0x01200310L,0x01280210L,0x01280310L,
},{
/* for D bits (numbered as per FIPS 46) 22 23 24 25 27 28 */
0x00000000L,0x04000000L,0x00040000L,0x04040000L,
0x00000002L,0x04000002L,0x00040002L,0x04040002L,
0x00002000L,0x04002000L,0x00042000L,0x04042000L,
0x00002002L,0x04002002L,0x00042002L,0x04042002L,
0x00000020L,0x04000020L,0x00040020L,0x04040020L,
0x00000022L,0x04000022L,0x00040022L,0x04040022L,
0x00002020L,0x04002020L,0x00042020L,0x04042020L,
0x00002022L,0x04002022L,0x00042022L,0x04042022L,
0x00000800L,0x04000800L,0x00040800L,0x04040800L,
0x00000802L,0x04000802L,0x00040802L,0x04040802L,
0x00002800L,0x04002800L,0x00042800L,0x04042800L,
0x00002802L,0x04002802L,0x00042802L,0x04042802L,
0x00000820L,0x04000820L,0x00040820L,0x04040820L,
0x00000822L,0x04000822L,0x00040822L,0x04040822L,
0x00002820L,0x04002820L,0x00042820L,0x04042820L,
0x00002822L,0x04002822L,0x00042822L,0x04042822L,
}};
