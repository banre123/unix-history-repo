/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Mike Olson.
 *
 * %sccs.include.redist.c%
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)bt_put.c	8.3 (Berkeley) %G%";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <db.h>
#include "btree.h"

static EPG *bt_fast __P((BTREE *, const DBT *, const DBT *, int *));

/*
 * __BT_PUT -- Add a btree item to the tree.
 *
 * Parameters:
 *	dbp:	pointer to access method
 *	key:	key
 *	data:	data
 *	flag:	R_NOOVERWRITE
 *
 * Returns:
 *	RET_ERROR, RET_SUCCESS and RET_SPECIAL if the key is already in the
 *	tree and R_NOOVERWRITE specified.
 */
int
__bt_put(dbp, key, data, flags)
	const DB *dbp;
	DBT *key;
	const DBT *data;
	u_int flags;
{
	BTREE *t;
	DBT tkey, tdata;
	EPG *e;
	PAGE *h;
	indx_t index, nxtindex;
	pgno_t pg;
	size_t nbytes;
	int dflags, exact, status;
	char *dest, db[NOVFLSIZE], kb[NOVFLSIZE];

	t = dbp->internal;

	/* Toss any page pinned across calls. */
	if (t->bt_pinned != NULL) {
		mpool_put(t->bt_mp, t->bt_pinned, 0);
		t->bt_pinned = NULL;
	}

	switch (flags) {
	case R_CURSOR:
		if (!ISSET(t, B_SEQINIT))
			goto einval;
		if (ISSET(t, B_DELCRSR))
			goto einval;
		break;
	case 0:
	case R_NOOVERWRITE:
		break;
	default:
einval:		errno = EINVAL;
		return (RET_ERROR);
	}

	if (ISSET(t, B_RDONLY)) {
		errno = EPERM;
		return (RET_ERROR);
	}

	/*
	 * If the key/data won't fit on a page, store it on indirect pages.
	 * Only store the key on the overflow page if it's too big after the
	 * data is on an overflow page.
	 *
	 * XXX
	 * If the insert fails later on, these pages aren't recovered.
	 */
	dflags = 0;
	if (key->size + data->size > t->bt_ovflsize) {
		if (key->size > t->bt_ovflsize) {
storekey:		if (__ovfl_put(t, key, &pg) == RET_ERROR)
				return (RET_ERROR);
			tkey.data = kb;
			tkey.size = NOVFLSIZE;
			memmove(kb, &pg, sizeof(pgno_t));
			memmove(kb + sizeof(pgno_t),
			    &key->size, sizeof(size_t));
			dflags |= P_BIGKEY;
			key = &tkey;
		}
		if (key->size + data->size > t->bt_ovflsize) {
			if (__ovfl_put(t, data, &pg) == RET_ERROR)
				return (RET_ERROR);
			tdata.data = db;
			tdata.size = NOVFLSIZE;
			memmove(db, &pg, sizeof(pgno_t));
			memmove(db + sizeof(pgno_t),
			    &data->size, sizeof(size_t));
			dflags |= P_BIGDATA;
			data = &tdata;
		}
		if (key->size + data->size > t->bt_ovflsize)
			goto storekey;
	}

	/* Replace the cursor. */
	if (flags == R_CURSOR) {
		if ((h = mpool_get(t->bt_mp, t->bt_bcursor.pgno, 0)) == NULL)
			return (RET_ERROR);
		index = t->bt_bcursor.index;
		goto delete;
	}

	/*
	 * Find the key to delete, or, the location at which to insert.  Bt_fast
	 * and __bt_search pin the returned page.
	 */
	if (t->bt_order == NOT || (e = bt_fast(t, key, data, &exact)) == NULL)
		if ((e = __bt_search(t, key, &exact)) == NULL)
			return (RET_ERROR);
	h = e->page;
	index = e->index;

	/*
	 * Add the specified key/data pair to the tree.  If an identical key
	 * is already in the tree, and R_NOOVERWRITE is set, an error is
	 * returned.  If R_NOOVERWRITE is not set, the key is either added (if
	 * duplicates are permitted) or an error is returned.
	 *
	 * Pages are split as required.
	 */
	switch (flags) {
	case R_NOOVERWRITE:
		if (!exact)
			break;
		/*
		 * One special case is if the cursor references the record and
		 * it's been flagged for deletion.  Then, we delete the record,
		 * leaving the cursor there -- this means that the inserted
		 * record will not be seen in a cursor scan.
		 */
		if (ISSET(t, B_DELCRSR) && t->bt_bcursor.pgno == h->pgno &&
		    t->bt_bcursor.index == index) {
			CLR(t, B_DELCRSR);
			goto delete;
		}
		mpool_put(t->bt_mp, h, 0);
		return (RET_SPECIAL);
	default:
		if (!exact || !ISSET(t, B_NODUPS))
			break;
delete:		if (__bt_dleaf(t, h, index) == RET_ERROR) {
			mpool_put(t->bt_mp, h, 0);
			return (RET_ERROR);
		}
		break;
	}

	/*
	 * If not enough room, or the user has put a ceiling on the number of
	 * keys permitted in the page, split the page.  The split code will
	 * insert the key and data and unpin the current page.  If inserting
	 * into the offset array, shift the pointers up.
	 */
	nbytes = NBLEAFDBT(key->size, data->size);
	if (h->upper - h->lower < nbytes + sizeof(indx_t)) {
		if ((status = __bt_split(t, h, key,
		    data, dflags, nbytes, index)) != RET_SUCCESS)
			return (status);
		goto success;
	}

	if (index < (nxtindex = NEXTINDEX(h)))
		memmove(h->linp + index + 1, h->linp + index,
		    (nxtindex - index) * sizeof(indx_t));
	h->lower += sizeof(indx_t);

	h->linp[index] = h->upper -= nbytes;
	dest = (char *)h + h->upper;
	WR_BLEAF(dest, key, data, dflags);

	if (t->bt_order == NOT)
		if (h->nextpg == P_INVALID) {
			if (index == NEXTINDEX(h) - 1) {
				t->bt_order = FORWARD;
				t->bt_last.index = index;
				t->bt_last.pgno = h->pgno;
			}
		} else if (h->prevpg == P_INVALID) {
			if (index == 0) {
				t->bt_order = BACK;
				t->bt_last.index = 0;
				t->bt_last.pgno = h->pgno;
			}
		}

	mpool_put(t->bt_mp, h, MPOOL_DIRTY);

success:
	if (flags == R_SETCURSOR) {
		t->bt_bcursor.pgno = e->page->pgno;
		t->bt_bcursor.index = e->index;
	}
	SET(t, B_MODIFIED);
	return (RET_SUCCESS);
}

#ifdef STATISTICS
u_long bt_cache_hit, bt_cache_miss;
#endif

/*
 * BT_FAST -- Do a quick check for sorted data.
 *
 * Parameters:
 *	t:	tree
 *	key:	key to insert
 *
 * Returns:
 * 	EPG for new record or NULL if not found.
 */
static EPG *
bt_fast(t, key, data, exactp)
	BTREE *t;
	const DBT *key, *data;
	int *exactp;
{
	PAGE *h;
	size_t nbytes;
	int cmp;

	if ((h = mpool_get(t->bt_mp, t->bt_last.pgno, 0)) == NULL) {
		t->bt_order = NOT;
		return (NULL);
	}
	t->bt_cur.page = h;
	t->bt_cur.index = t->bt_last.index;

	/*
	 * If won't fit in this page or have too many keys in this page, have
	 * to search to get split stack.
	 */
	nbytes = NBLEAFDBT(key->size, data->size);
	if (h->upper - h->lower < nbytes + sizeof(indx_t))
		goto miss;

	if (t->bt_order == FORWARD) {
		if (t->bt_cur.page->nextpg != P_INVALID)
			goto miss;
		if (t->bt_cur.index != NEXTINDEX(h) - 1)
			goto miss;
		if ((cmp = __bt_cmp(t, key, &t->bt_cur)) < 0)
			goto miss;
		t->bt_last.index = cmp ? ++t->bt_cur.index : t->bt_cur.index;
	} else {
		if (t->bt_cur.page->prevpg != P_INVALID)
			goto miss;
		if (t->bt_cur.index != 0)
			goto miss;
		if ((cmp = __bt_cmp(t, key, &t->bt_cur)) > 0)
			goto miss;
		t->bt_last.index = 0;
	}
	*exactp = cmp == 0;
#ifdef STATISTICS
	++bt_cache_hit;
#endif
	return (&t->bt_cur);

miss:
#ifdef STATISTICS
	++bt_cache_miss;
#endif
	t->bt_order = NOT;
	mpool_put(t->bt_mp, h, 0);
	return (NULL);
}
