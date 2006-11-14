/*-
 * Copyright (c) 2002-2004 Tim J. Robbins
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
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
 */

/*
 * Test program for wcsnrtombs().
 *
 * The function is tested with both the "C" ("POSIX") LC_CTYPE setting and
 * "ja_JP.eucJP". Other encodings are not tested.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

int
main(int argc, char *argv[])
{
	wchar_t srcbuf[128];
	char dstbuf[128];
	wchar_t *src;
	mbstate_t s;

	/*
	 * C/POSIX locale.
	 */

	printf("1..1\n");

	/* Simple null terminated string. */
	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	wcscpy(srcbuf, L"hello");
	memset(dstbuf, 0xcc, sizeof(dstbuf));
	src = srcbuf;
	memset(&s, 0, sizeof(s));
	assert(wcsnrtombs(dstbuf, (const wchar_t **)&src, 6, sizeof(dstbuf),
	    &s) == 5);
	assert(strcmp(dstbuf, "hello") == 0);
	assert((unsigned char)dstbuf[6] == 0xcc);
	assert(src == NULL);

	/* Simple null terminated string, stopping early. */
	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	wcscpy(srcbuf, L"hello");
	memset(dstbuf, 0xcc, sizeof(dstbuf));
	src = srcbuf;
	memset(&s, 0, sizeof(s));
	assert(wcsnrtombs(dstbuf, (const wchar_t **)&src, 4, sizeof(dstbuf),
	    &s) == 4);
	assert(memcmp(dstbuf, "hell", 4) == 0);
	assert((unsigned char)dstbuf[5] == 0xcc);
	assert(src == srcbuf + 4);

	/* Not enough space in destination buffer. */
	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	wcscpy(srcbuf, L"hello");
	memset(dstbuf, 0xcc, sizeof(dstbuf));
	src = srcbuf;
	memset(&s, 0, sizeof(s));
	assert(wcsnrtombs(dstbuf, (const wchar_t **)&src, 6, 4,
	    &s) == 4);
	assert(memcmp(dstbuf, "hell", 4) == 0);
	assert((unsigned char)dstbuf[5] == 0xcc);
	assert(src == srcbuf + 4);

	/* Null terminated string, internal dest. buffer */
	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	wcscpy(srcbuf, L"hello");
	src = srcbuf;
	memset(&s, 0, sizeof(s));
	assert(wcsnrtombs(NULL, (const wchar_t **)&src, 6, sizeof(dstbuf),
	    &s) == 5);

	/* Null terminated string, internal dest. buffer, stopping early. */
	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	wcscpy(srcbuf, L"hello");
	src = srcbuf;
	memset(&s, 0, sizeof(s));
	assert(wcsnrtombs(NULL, (const wchar_t **)&src, 4, sizeof(dstbuf),
	    &s) == 4);

	/* Null terminated string, internal state. */
	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	wcscpy(srcbuf, L"hello");
	memset(dstbuf, 0xcc, sizeof(dstbuf));
	src = srcbuf;
	assert(wcsnrtombs(dstbuf, (const wchar_t **)&src, 6, sizeof(dstbuf),
	    NULL) == 5);
	assert(strcmp(dstbuf, "hello") == 0);
	assert((unsigned char)dstbuf[6] == 0xcc);
	assert(src == NULL);

	/* Null terminated string, internal state, internal dest. buffer. */
	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	wcscpy(srcbuf, L"hello");
	src = srcbuf;
	assert(wcsnrtombs(NULL, (const wchar_t **)&src, 6, 0, NULL) == 5);

	/* Empty source buffer. */
	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	srcbuf[0] = L'\0';
	memset(dstbuf, 0xcc, sizeof(dstbuf));
	src = srcbuf;
	memset(&s, 0, sizeof(s));
	assert(wcsnrtombs(dstbuf, (const wchar_t **)&src, 1, sizeof(dstbuf),
	    &s) == 0);
	assert(dstbuf[0] == L'\0');

	/* Zero length destination buffer. */
	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	wcscpy(srcbuf, L"hello");
	memset(dstbuf, 0xcc, sizeof(dstbuf));
	src = srcbuf;
	memset(&s, 0, sizeof(s));
	assert(wcsnrtombs(dstbuf, (const wchar_t **)&src, 6, 0, &s) == 0);
	assert((unsigned char)dstbuf[0] == 0xcc);

	/* Zero length source buffer. */
	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	memset(dstbuf, 0xcc, sizeof(dstbuf));
	src = srcbuf;
	memset(&s, 0, sizeof(s));
	assert(wcsnrtombs(dstbuf, (const wchar_t **)&src, 0, sizeof(dstbuf),
	    &s) == 0);
	assert((unsigned char)dstbuf[0] == 0xcc);
	assert(src == srcbuf);

	/*
	 * Japanese (EUC) locale.
	 */

	assert(strcmp(setlocale(LC_CTYPE, "ja_JP.eucJP"), "ja_JP.eucJP") == 0);
	assert(MB_CUR_MAX > 1);

	wmemset(srcbuf, 0xcc, sizeof(srcbuf) / sizeof(*srcbuf));
	srcbuf[0] = 0xA3C1;
	srcbuf[1] = 0x0020;
	srcbuf[2] = 0x0042;
	srcbuf[3] = 0x0020;
	srcbuf[4] = 0xA3C3;
	srcbuf[5] = 0x0000;
	memset(dstbuf, 0xcc, sizeof(dstbuf));
	src = srcbuf;
	memset(&s, 0, sizeof(s));
	assert(wcsnrtombs(dstbuf, (const wchar_t **)&src, 6, sizeof(dstbuf),
	    &s) == 7);
	assert(strcmp(dstbuf, "\xA3\xC1 B \xA3\xC3") == 0);
	assert((unsigned char)dstbuf[8] == 0xcc);
	assert(src == NULL);

	/* Stopping early. */
	memset(dstbuf, 0xcc, sizeof(dstbuf));
	src = srcbuf;
	memset(&s, 0, sizeof(s));
	assert(wcsnrtombs(dstbuf, (const wchar_t **)&src, 6, 6,
	    &s) == 5);
	assert(memcmp(dstbuf, "\xA3\xC1 B ", 5) == 0);
	assert((unsigned char)dstbuf[5] == 0xcc);
	assert(src == srcbuf + 4);

	printf("ok 1 - wcsnrtombs()\n");

	return (0);
}
