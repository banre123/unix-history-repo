/*
 * Copyright (c) 1995-1998 John Birrell <jb@cimlogic.com.au>.
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
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JOHN BIRRELL AND CONTRIBUTORS ``AS IS'' AND
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
 * $FreeBSD$
 */

#include "namespace.h"
#include <errno.h>
#include <pthread.h>
#include <pthread_np.h>
#include "un-namespace.h"

#include "thr_private.h"

static int suspend_common(struct pthread *, struct pthread *,
		int);

__weak_reference(_pthread_suspend_np, pthread_suspend_np);
__weak_reference(_pthread_suspend_all_np, pthread_suspend_all_np);

/* Suspend a thread: */
int
_pthread_suspend_np(pthread_t thread)
{
	struct pthread *curthread = _get_curthread();
	int ret;

	/* Suspending the current thread doesn't make sense. */
	if (thread == _get_curthread())
		ret = EDEADLK;

	/* Add a reference to the thread: */
	else if ((ret = _thr_ref_add(curthread, thread, /*include dead*/0))
	    == 0) {
		/* Lock the threads scheduling queue: */
		THR_THREAD_LOCK(curthread, thread);
		suspend_common(curthread, thread, 1);
		/* Unlock the threads scheduling queue: */
		THR_THREAD_UNLOCK(curthread, thread);

		/* Don't forget to remove the reference: */
		_thr_ref_delete(curthread, thread);
	}
	return (ret);
}

void
_pthread_suspend_all_np(void)
{
	struct pthread *curthread = _get_curthread();
	struct pthread *thread;
	int ret;

	THREAD_LIST_LOCK(curthread);

	TAILQ_FOREACH(thread, &_thread_list, tle) {
		if (thread != curthread) {
			THR_THREAD_LOCK(curthread, thread);
			if (thread->state != PS_DEAD &&
	      		   !(thread->flags & THR_FLAGS_SUSPENDED))
			    thread->flags |= THR_FLAGS_NEED_SUSPEND;
			THR_THREAD_UNLOCK(curthread, thread);
		}
	}
	thr_kill(-1, SIGCANCEL);

restart:
	TAILQ_FOREACH(thread, &_thread_list, tle) {
		if (thread != curthread) {
			/* First try to suspend the thread without waiting */
			THR_THREAD_LOCK(curthread, thread);
			ret = suspend_common(curthread, thread, 0);
			if (ret == 0) {
				/* Can not suspend, try to wait */
				thread->refcount++;
				THREAD_LIST_UNLOCK(curthread);
				suspend_common(curthread, thread, 1);
				THR_THREAD_UNLOCK(curthread, thread);
				THREAD_LIST_LOCK(curthread);
				_thr_ref_delete_unlocked(curthread, thread);
				/*
				 * Because we were blocked, things may have
				 * been changed, we have to restart the
				 * process.
				 */
				goto restart;
			}
			THR_THREAD_UNLOCK(curthread, thread);
		}
	}

	THREAD_LIST_UNLOCK(curthread);
}

static int
suspend_common(struct pthread *curthread, struct pthread *thread,
	int waitok)
{
	umtx_t tmp;

	while (thread->state != PS_DEAD &&
	      !(thread->flags & THR_FLAGS_SUSPENDED)) {
		thread->flags |= THR_FLAGS_NEED_SUSPEND;
		tmp = thread->cycle;
		THR_THREAD_UNLOCK(curthread, thread);
		_thr_send_sig(thread, SIGCANCEL);
		if (waitok) {
			_thr_umtx_wait(&thread->cycle, tmp, NULL);
			THR_THREAD_LOCK(curthread, thread);
		} else {
			THR_THREAD_LOCK(curthread, thread);
			return (0);
		}
	}

	return (1);
}
