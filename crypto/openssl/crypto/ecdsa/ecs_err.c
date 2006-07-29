/* crypto/ecdsa/ecs_err.c */
/* ====================================================================
 * Copyright (c) 1999-2005 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

/* NOTE: this file was auto generated by the mkerr.pl script: any changes
 * made to it will be overwritten when the script next updates this file,
 * only reason strings will be preserved.
 */

#include <stdio.h>
#include <openssl/err.h>
#include <openssl/ecdsa.h>

/* BEGIN ERROR CODES */
#ifndef OPENSSL_NO_ERR

#define ERR_FUNC(func) ERR_PACK(ERR_LIB_ECDSA,func,0)
#define ERR_REASON(reason) ERR_PACK(ERR_LIB_ECDSA,0,reason)

static ERR_STRING_DATA ECDSA_str_functs[]=
	{
{ERR_FUNC(ECDSA_F_ECDSA_DATA_NEW_METHOD),	"ECDSA_DATA_new_method"},
{ERR_FUNC(ECDSA_F_ECDSA_DO_SIGN),	"ECDSA_do_sign"},
{ERR_FUNC(ECDSA_F_ECDSA_DO_VERIFY),	"ECDSA_do_verify"},
{ERR_FUNC(ECDSA_F_ECDSA_SIGN_SETUP),	"ECDSA_sign_setup"},
{0,NULL}
	};

static ERR_STRING_DATA ECDSA_str_reasons[]=
	{
{ERR_REASON(ECDSA_R_BAD_SIGNATURE)       ,"bad signature"},
{ERR_REASON(ECDSA_R_DATA_TOO_LARGE_FOR_KEY_SIZE),"data too large for key size"},
{ERR_REASON(ECDSA_R_ERR_EC_LIB)          ,"err ec lib"},
{ERR_REASON(ECDSA_R_MISSING_PARAMETERS)  ,"missing parameters"},
{ERR_REASON(ECDSA_R_RANDOM_NUMBER_GENERATION_FAILED),"random number generation failed"},
{ERR_REASON(ECDSA_R_SIGNATURE_MALLOC_FAILED),"signature malloc failed"},
{0,NULL}
	};

#endif

void ERR_load_ECDSA_strings(void)
	{
	static int init=1;

	if (init)
		{
		init=0;
#ifndef OPENSSL_NO_ERR
		ERR_load_strings(0,ECDSA_str_functs);
		ERR_load_strings(0,ECDSA_str_reasons);
#endif

		}
	}
