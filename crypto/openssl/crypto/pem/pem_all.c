/* crypto/pem/pem_all.c */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
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
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
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

#include <stdio.h>
#undef SSLEAY_MACROS
#include "cryptlib.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pkcs7.h>
#include <openssl/pem.h>

#ifndef NO_RSA
static RSA *pkey_get_rsa(EVP_PKEY *key, RSA **rsa);
#endif
#ifndef NO_DSA
static DSA *pkey_get_dsa(EVP_PKEY *key, DSA **dsa);
#endif

IMPLEMENT_PEM_rw(X509, X509, PEM_STRING_X509, X509)

IMPLEMENT_PEM_rw(X509_AUX, X509, PEM_STRING_X509_TRUSTED, X509_AUX)

IMPLEMENT_PEM_rw(X509_REQ, X509_REQ, PEM_STRING_X509_REQ, X509_REQ)

IMPLEMENT_PEM_write(X509_REQ_NEW, X509_REQ, PEM_STRING_X509_REQ_OLD, X509_REQ)

IMPLEMENT_PEM_rw(X509_CRL, X509_CRL, PEM_STRING_X509_CRL, X509_CRL)

IMPLEMENT_PEM_rw(PKCS7, PKCS7, PEM_STRING_PKCS7, PKCS7)

IMPLEMENT_PEM_rw(NETSCAPE_CERT_SEQUENCE, NETSCAPE_CERT_SEQUENCE,
					PEM_STRING_X509, NETSCAPE_CERT_SEQUENCE)

IMPLEMENT_PEM_rw(PKCS8, X509_SIG, PEM_STRING_PKCS8, X509_SIG)
IMPLEMENT_PEM_rw(PKCS8_PRIV_KEY_INFO, PKCS8_PRIV_KEY_INFO, PEM_STRING_PKCS8INF,
							 PKCS8_PRIV_KEY_INFO)

#ifndef NO_RSA

/* We treat RSA or DSA private keys as a special case.
 *
 * For private keys we read in an EVP_PKEY structure with
 * PEM_read_bio_PrivateKey() and extract the relevant private
 * key: this means can handle "traditional" and PKCS#8 formats
 * transparently.
 */

static RSA *pkey_get_rsa(EVP_PKEY *key, RSA **rsa)
{
	RSA *rtmp;
	if(!key) return NULL;
	rtmp = EVP_PKEY_get1_RSA(key);
	EVP_PKEY_free(key);
	if(!rtmp) return NULL;
	if(rsa) {
		RSA_free(*rsa);
		*rsa = rtmp;
	}
	return rtmp;
}

RSA *PEM_read_bio_RSAPrivateKey(BIO *bp, RSA **rsa, pem_password_cb *cb,
								void *u)
{
	EVP_PKEY *pktmp;
	pktmp = PEM_read_bio_PrivateKey(bp, NULL, cb, u);
	return pkey_get_rsa(pktmp, rsa);
}

#ifndef NO_FP_API

RSA *PEM_read_RSAPrivateKey(FILE *fp, RSA **rsa, pem_password_cb *cb,
								void *u)
{
	EVP_PKEY *pktmp;
	pktmp = PEM_read_PrivateKey(fp, NULL, cb, u);
	return pkey_get_rsa(pktmp, rsa);
}

#endif

IMPLEMENT_PEM_write_cb(RSAPrivateKey, RSA, PEM_STRING_RSA, RSAPrivateKey)
IMPLEMENT_PEM_rw(RSAPublicKey, RSA, PEM_STRING_RSA_PUBLIC, RSAPublicKey)
IMPLEMENT_PEM_rw(RSA_PUBKEY, RSA, PEM_STRING_PUBLIC, RSA_PUBKEY)

#endif

#ifndef NO_DSA

static DSA *pkey_get_dsa(EVP_PKEY *key, DSA **dsa)
{
	DSA *dtmp;
	if(!key) return NULL;
	dtmp = EVP_PKEY_get1_DSA(key);
	EVP_PKEY_free(key);
	if(!dtmp) return NULL;
	if(dsa) {
		DSA_free(*dsa);
		*dsa = dtmp;
	}
	return dtmp;
}

DSA *PEM_read_bio_DSAPrivateKey(BIO *bp, DSA **dsa, pem_password_cb *cb,
								void *u)
{
	EVP_PKEY *pktmp;
	pktmp = PEM_read_bio_PrivateKey(bp, NULL, cb, u);
	return pkey_get_dsa(pktmp, dsa);
}

IMPLEMENT_PEM_write_cb(DSAPrivateKey, DSA, PEM_STRING_DSA, DSAPrivateKey)
IMPLEMENT_PEM_rw(DSA_PUBKEY, DSA, PEM_STRING_PUBLIC, DSA_PUBKEY)

#ifndef NO_FP_API

DSA *PEM_read_DSAPrivateKey(FILE *fp, DSA **dsa, pem_password_cb *cb,
								void *u)
{
	EVP_PKEY *pktmp;
	pktmp = PEM_read_PrivateKey(fp, NULL, cb, u);
	return pkey_get_dsa(pktmp, dsa);
}

#endif

IMPLEMENT_PEM_rw(DSAparams, DSA, PEM_STRING_DSAPARAMS, DSAparams)

#endif

#ifndef NO_DH

IMPLEMENT_PEM_rw(DHparams, DH, PEM_STRING_DHPARAMS, DHparams)

#endif


/* The PrivateKey case is not that straightforward.
 *   IMPLEMENT_PEM_rw_cb(PrivateKey, EVP_PKEY, PEM_STRING_EVP_PKEY, PrivateKey)
 * does not work, RSA and DSA keys have specific strings.
 * (When reading, parameter PEM_STRING_EVP_PKEY is a wildcard for anything
 * appropriate.)
 */
IMPLEMENT_PEM_read(PrivateKey, EVP_PKEY, PEM_STRING_EVP_PKEY, PrivateKey)
IMPLEMENT_PEM_write_cb(PrivateKey, EVP_PKEY, ((x->type == EVP_PKEY_DSA)?PEM_STRING_DSA:PEM_STRING_RSA), PrivateKey)

IMPLEMENT_PEM_rw(PUBKEY, EVP_PKEY, PEM_STRING_PUBLIC, PUBKEY)
