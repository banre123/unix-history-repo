/*-
 * Copyright (c) 2011 Michihiro NAKAJIMA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "test.h"
__FBSDID("$FreeBSD$");
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

/*
 * Test UFS file flags with/without use-set option.
 */
#if defined(UF_IMMUTABLE) && defined(UF_NODUMP)

static char buff[4096];
static struct {
	const char	*path;
	unsigned long	 fflags;
} entries[] = {
	{ "./f1", 	UF_IMMUTABLE | UF_NODUMP },
	{ "./f11", 	UF_IMMUTABLE | UF_NODUMP },
	{ "./f2", 	0 },
	{ "./f3", 	UF_NODUMP },
	{ NULL, 0 }
};

static void
test_write_format_mtree_sub(int use_set)
{
	struct archive_entry *ae;
	struct archive* a;
	size_t used;
	int i;

	/* Create a mtree format archive. */
	assert((a = archive_write_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_write_set_format_mtree(a));
	if (use_set)
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_write_set_options(a, "use-set,!all,flags,type"));
	else
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_write_set_options(a, "!all,flags,type"));
	assertEqualIntA(a, ARCHIVE_OK,
	    archive_write_open_memory(a, buff, sizeof(buff)-1, &used));

	/* Write entries */
	for (i = 0; entries[i].path != NULL; i++) {
		assert((ae = archive_entry_new()) != NULL);
		archive_entry_set_fflags(ae, entries[i].fflags, 0);
		archive_entry_copy_pathname(ae, entries[i].path);
		archive_entry_set_size(ae, 0);
		assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
		archive_entry_free(ae);
	}
	assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
        assertEqualInt(ARCHIVE_OK, archive_write_free(a));

	if (use_set) {
		const char *p;

		buff[used] = '\0';
		assert(NULL != (p = strstr(buff, "\n/set ")));
		if (p != NULL) {
			char *r;
			const char *o;
			p++;
			r = strchr(p, '\n');
			if (r != NULL)
				*r = '\0';
			o = "/set type=file flags=uchg,nodump";
			assertEqualString(o, p);
			if (r != NULL)
				*r = '\n';
		}
	}

	/*
	 * Read the data and check it.
	 */
	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_open_memory(a, buff, used));

	/* Read entries */
	for (i = 0; entries[i].path != NULL; i++) {
		unsigned long fset, fclr;

		assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
		archive_entry_fflags(ae, &fset, &fclr);
		assertEqualInt((int)entries[i].fflags, (int)fset);
		assertEqualInt(0, (int)fclr);
		assertEqualString(entries[i].path, archive_entry_pathname(ae));
	}
	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

#endif

DEFINE_TEST(test_write_format_mtree_fflags)
{
#if defined(UF_IMMUTABLE) && defined(UF_NODUMP)
	/* Default setting */
	test_write_format_mtree_sub(0);
	/* Use /set keyword */
	test_write_format_mtree_sub(1);
#else
	skipping("This platform does not support UFS file flags");
#endif
}
