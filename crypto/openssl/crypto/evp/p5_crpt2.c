/* p5_crpt2.c */
/* Written by Dr Stephen N Henson (shenson@bigfoot.com) for the OpenSSL
 * project 1999.
 */
/* ====================================================================
 * Copyright (c) 1999 The OpenSSL Project.  All rights reserved.
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
 *    licensing@OpenSSL.org.
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
#if !defined(OPENSSL_NO_HMAC) && !defined(OPENSSL_NO_SHA)
#include <stdio.h>
#include <stdlib.h>
#include "cryptlib.h"
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

/* set this to print out info about the keygen algorithm */
/* #define DEBUG_PKCS5V2 */

#ifdef DEBUG_PKCS5V2
	static void h__dump (const unsigned char *p, int len);
#endif

/* This is an implementation of PKCS#5 v2.0 password based encryption key
 * derivation function PBKDF2 using the only currently defined function HMAC
 * with SHA1. Verified against test vectors posted by Peter Gutmann
 * <pgut001@cs.auckland.ac.nz> to the PKCS-TNG <pkcs-tng@rsa.com> mailing list.
 */

int PKCS5_PBKDF2_HMAC_SHA1(const char *pass, int passlen,
			   unsigned char *salt, int saltlen, int iter,
			   int keylen, unsigned char *out)
{
	unsigned char digtmp[SHA_DIGEST_LENGTH], *p, itmp[4];
	int cplen, j, k, tkeylen;
	unsigned long i = 1;
	HMAC_CTX hctx;

	HMAC_CTX_init(&hctx);
	p = out;
	tkeylen = keylen;
	if(!pass) passlen = 0;
	else if(passlen == -1) passlen = strlen(pass);
	while(tkeylen) {
		if(tkeylen > SHA_DIGEST_LENGTH) cplen = SHA_DIGEST_LENGTH;
		else cplen = tkeylen;
		/* We are unlikely to ever use more than 256 blocks (5120 bits!)
		 * but just in case...
		 */
		itmp[0] = (unsigned char)((i >> 24) & 0xff);
		itmp[1] = (unsigned char)((i >> 16) & 0xff);
		itmp[2] = (unsigned char)((i >> 8) & 0xff);
		itmp[3] = (unsigned char)(i & 0xff);
		HMAC_Init_ex(&hctx, pass, passlen, EVP_sha1(), NULL);
		HMAC_Update(&hctx, salt, saltlen);
		HMAC_Update(&hctx, itmp, 4);
		HMAC_Final(&hctx, digtmp, NULL);
		memcpy(p, digtmp, cplen);
		for(j = 1; j < iter; j++) {
			HMAC(EVP_sha1(), pass, passlen,
				 digtmp, SHA_DIGEST_LENGTH, digtmp, NULL);
			for(k = 0; k < cplen; k++) p[k] ^= digtmp[k];
		}
		tkeylen-= cplen;
		i++;
		p+= cplen;
	}
	HMAC_CTX_cleanup(&hctx);
#ifdef DEBUG_PKCS5V2
	fprintf(stderr, "Password:\n");
	h__dump (pass, passlen);
	fprintf(stderr, "Salt:\n");
	h__dump (salt, saltlen);
	fprintf(stderr, "Iteration count %d\n", iter);
	fprintf(stderr, "Key:\n");
	h__dump (out, keylen);
#endif
	return 1;
}

#ifdef DO_TEST
main()
{
	unsigned char out[4];
	unsigned char salt[] = {0x12, 0x34, 0x56, 0x78};
	PKCS5_PBKDF2_HMAC_SHA1("password", -1, salt, 4, 5, 4, out);
	fprintf(stderr, "Out %02X %02X %02X %02X\n",
					 out[0], out[1], out[2], out[3]);
}

#endif

/* Now the key derivation function itself. This is a bit evil because
 * it has to check the ASN1 parameters are valid: and there are quite a
 * few of them...
 */

int PKCS5_v2_PBE_keyivgen(EVP_CIPHER_CTX *ctx, const char *pass, int passlen,
                         ASN1_TYPE *param, const EVP_CIPHER *c, const EVP_MD *md,
                         int en_de)
{
	unsigned char *pbuf, *salt, key[EVP_MAX_KEY_LENGTH];
	int saltlen, keylen, iter, plen;
	PBE2PARAM *pbe2 = NULL;
	const EVP_CIPHER *cipher;
	PBKDF2PARAM *kdf = NULL;

	pbuf = param->value.sequence->data;
	plen = param->value.sequence->length;
	if(!param || (param->type != V_ASN1_SEQUENCE) ||
				   !(pbe2 = d2i_PBE2PARAM(NULL, &pbuf, plen))) {
		EVPerr(EVP_F_PKCS5_V2_PBE_KEYIVGEN,EVP_R_DECODE_ERROR);
		return 0;
	}

	/* See if we recognise the key derivation function */

	if(OBJ_obj2nid(pbe2->keyfunc->algorithm) != NID_id_pbkdf2) {
		EVPerr(EVP_F_PKCS5_V2_PBE_KEYIVGEN,
				EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION);
		goto err;
	}

	/* lets see if we recognise the encryption algorithm.
	 */

	cipher = EVP_get_cipherbyname(
			OBJ_nid2sn(OBJ_obj2nid(pbe2->encryption->algorithm)));

	if(!cipher) {
		EVPerr(EVP_F_PKCS5_V2_PBE_KEYIVGEN,
						EVP_R_UNSUPPORTED_CIPHER);
		goto err;
	}

	/* Fixup cipher based on AlgorithmIdentifier */
	EVP_CipherInit_ex(ctx, cipher, NULL, NULL, NULL, en_de);
	if(EVP_CIPHER_asn1_to_param(ctx, pbe2->encryption->parameter) < 0) {
		EVPerr(EVP_F_PKCS5_V2_PBE_KEYIVGEN,
					EVP_R_CIPHER_PARAMETER_ERROR);
		goto err;
	}
	keylen = EVP_CIPHER_CTX_key_length(ctx);
	OPENSSL_assert(keylen <= sizeof key);

	/* Now decode key derivation function */

	pbuf = pbe2->keyfunc->parameter->value.sequence->data;
	plen = pbe2->keyfunc->parameter->value.sequence->length;
	if(!pbe2->keyfunc->parameter ||
		 (pbe2->keyfunc->parameter->type != V_ASN1_SEQUENCE) ||
				!(kdf = d2i_PBKDF2PARAM(NULL, &pbuf, plen)) ) {
		EVPerr(EVP_F_PKCS5_V2_PBE_KEYIVGEN,EVP_R_DECODE_ERROR);
		goto err;
	}

	PBE2PARAM_free(pbe2);
	pbe2 = NULL;

	/* Now check the parameters of the kdf */

	if(kdf->keylength && (ASN1_INTEGER_get(kdf->keylength) != keylen)){
		EVPerr(EVP_F_PKCS5_V2_PBE_KEYIVGEN,
						EVP_R_UNSUPPORTED_KEYLENGTH);
		goto err;
	}

	if(kdf->prf && (OBJ_obj2nid(kdf->prf->algorithm) != NID_hmacWithSHA1)) {
		EVPerr(EVP_F_PKCS5_V2_PBE_KEYIVGEN, EVP_R_UNSUPPORTED_PRF);
		goto err;
	}

	if(kdf->salt->type != V_ASN1_OCTET_STRING) {
		EVPerr(EVP_F_PKCS5_V2_PBE_KEYIVGEN,
						EVP_R_UNSUPPORTED_SALT_TYPE);
		goto err;
	}

	/* it seems that its all OK */
	salt = kdf->salt->value.octet_string->data;
	saltlen = kdf->salt->value.octet_string->length;
	iter = ASN1_INTEGER_get(kdf->iter);
	PKCS5_PBKDF2_HMAC_SHA1(pass, passlen, salt, saltlen, iter, keylen, key);
	EVP_CipherInit_ex(ctx, NULL, NULL, key, NULL, en_de);
	OPENSSL_cleanse(key, keylen);
	PBKDF2PARAM_free(kdf);
	return 1;

	err:
	PBE2PARAM_free(pbe2);
	PBKDF2PARAM_free(kdf);
	return 0;
}

#ifdef DEBUG_PKCS5V2
static void h__dump (const unsigned char *p, int len)
{
        for (; len --; p++) fprintf(stderr, "%02X ", *p);
        fprintf(stderr, "\n");
}
#endif
#endif
