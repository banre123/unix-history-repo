/*	kern_synch.c	3.2	%H%	*/

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/file.h"
#include "../h/inode.h"
#include "../h/vm.h"
#include "../h/pte.h"

#ifdef FASTVAX
asm(" .globl _eintr");
#endif

#define SQSIZE 0100	/* Must be power of 2 */
#define HASH(x)	(( (int) x >> 5) & (SQSIZE-1))
struct proc *slpque[SQSIZE];

/*
 * Give up the processor till a wakeup occurs
 * on chan, at which time the process
 * enters the scheduling queue at priority pri.
 * The most important effect of pri is that when
 * pri<=PZERO a signal cannot disturb the sleep;
 * if pri>PZERO signals will be processed.
 * Callers of this routine must be prepared for
 * premature return, and check that the reason for
 * sleeping has gone away.
 */
sleep(chan, pri)
caddr_t chan;
{
	register struct proc *rp;
	register s, h;

	rp = u.u_procp;
	s = spl6();
	if (chan==0 || rp->p_stat != SRUN || rp->p_rlink)
		panic("sleep");
	rp->p_stat = SSLEEP;
	rp->p_wchan = chan;
	rp->p_slptime = 0;
	rp->p_pri = pri;
	h = HASH(chan);
	rp->p_link = slpque[h];
	slpque[h] = rp;
	if(pri > PZERO) {
		if(rp->p_sig && issig()) {
			rp->p_wchan = 0;
			rp->p_stat = SRUN;
			slpque[h] = rp->p_link;
			VOID spl0();
			goto psig;
		}
		VOID spl0();
		if(runin != 0) {
			runin = 0;
			wakeup((caddr_t)&runin);
		}
		swtch();
		if(rp->p_sig && issig())
			goto psig;
	} else {
		VOID spl0();
		swtch();
	}
	splx(s);
	return;

	/*
	 * If priority was low (>PZERO) and
	 * there has been a signal,
	 * execute non-local goto to
	 * the qsav location.
	 * (see trap1/trap.c)
	 */
psig:
#ifndef FASTVAX
	longjmp(u.u_qsav);
#else
	asm(".set U_SSAV,140");
	asm("movl _u+U_SSAV,fp");
	asm("movl _u+U_SSAV+4,sp");
	asm("movl _u+U_SSAV+8,r11");
	u.u_error = EINTR;
	asm("jmp _eintr");
#endif
	/*NOTREACHED*/
}

/*
 * Wake up all processes sleeping on chan.
 */
wakeup(chan)
register caddr_t chan;
{
	register struct proc *p, *q;
	register i;
	int s;

	s = spl6();
	i = HASH(chan);
restart:
	p = slpque[i];
	q = NULL;
	while(p != NULL) {
		if (p->p_rlink || p->p_stat != SSLEEP)
			panic("wakeup");
		if (p->p_wchan==chan && p->p_stat!=SZOMB) {
			if (q == NULL)
				slpque[i] = p->p_link;
			else
				q->p_link = p->p_link;
			p->p_wchan = 0;
			p->p_slptime = 0;
			/* OPTIMIZED INLINE EXPANSION OF setrun(p) */
			p->p_stat = SRUN;
			if (p->p_flag & SLOAD) {
#ifndef FASTVAX
				p->p_link = runq;
				runq = p->p_link;
#else
				setrq(p);
#endif
			}
			if(p->p_pri < curpri)
				runrun++;
			if(runout != 0 && (p->p_flag&SLOAD) == 0) {
				runout = 0;
				wakeup((caddr_t)&runout);
			}
			/* END INLINE EXPANSION */
			goto restart;
		}
		q = p;
		p = p->p_link;
	}
	splx(s);
}

#ifdef FASTVAX
/*
 * Initialize the (doubly-linked) run queues
 * to be empty.
 */
rqinit()
{
	register int i;

	for (i = 0; i < NQS; i++)
		qs[i].ph_link = qs[i].ph_rlink = (struct proc *)&qs[i];
}
#endif

/*
 * Set the process running;
 * arrange for it to be swapped in if necessary.
 */
setrun(p)
register struct proc *p;
{
	register caddr_t w;
	register s;

	s = spl6();
	switch (p->p_stat) {

	case 0:
	case SWAIT:
	case SRUN:
	case SZOMB:
	default:
		panic("setrun");

	case SSLEEP:
		if (w = p->p_wchan) {
			wakeup(w);
			splx(s);
			return;
		}
		break;

	case SIDL:
	case SSTOP:
		break;
	}
	p->p_stat = SRUN;
	if (p->p_flag & SLOAD)
		setrq(p);
	splx(s);
	if(p->p_pri < curpri)
		runrun++;
	if(runout != 0 && (p->p_flag&SLOAD) == 0) {
		runout = 0;
		wakeup((caddr_t)&runout);
	}
}

/*
 * Set user priority.
 * The rescheduling flag (runrun)
 * is set if the priority is better
 * than the currently running process.
 */
setpri(pp)
register struct proc *pp;
{
	register p;

	p = (pp->p_cpu & 0377)/16;
	p += PUSER + pp->p_nice - NZERO;
	if(p > 127)
		p = 127;
	if(p < curpri)
		runrun++;
	pp->p_usrpri = p;
	return(p);
}

/*
 * Create a new process-- the internal version of
 * sys fork.
 * It returns 1 in the new process, 0 in the old.
 */
newproc(isvfork)
{
	register struct proc *p;
	register struct proc *rpp, *rip;
	register int n;

	p = NULL;
	/*
	 * First, just locate a slot for a process
	 * and copy the useful info from this process into it.
	 * The panic "cannot happen" because fork has already
	 * checked for the existence of a slot.
	 */
retry:
	mpid++;
	if(mpid >= 30000) {
		mpid = 0;
		goto retry;
	}
	for(rpp = &proc[0]; rpp < &proc[NPROC]; rpp++) {
		if(rpp->p_stat == NULL && p==NULL)
			p = rpp;
		if (rpp->p_pid==mpid || rpp->p_pgrp==mpid)
			goto retry;
	}
	if ((rpp = p)==NULL)
		panic("no procs");

	/*
	 * make proc entry for new proc
	 */

	rip = u.u_procp;
	rpp->p_stat = SIDL;
	rpp->p_clktim = 0;
	rpp->p_flag = SLOAD | (rip->p_flag & SPAGI);
	if (isvfork) {
		rpp->p_flag |= SVFORK;
		rpp->p_ndx = rip->p_ndx;
	} else
		rpp->p_ndx = rpp - proc;
	rpp->p_uid = rip->p_uid;
	rpp->p_pgrp = rip->p_pgrp;
	rpp->p_nice = rip->p_nice;
	rpp->p_textp = isvfork ? 0 : rip->p_textp;
	rpp->p_pid = mpid;
	rpp->p_ppid = rip->p_pid;
	rpp->p_time = 0;
	rpp->p_cpu = 0;
	if (isvfork) {
		rpp->p_tsize = rpp->p_dsize = rpp->p_ssize = 0;
		rpp->p_szpt = clrnd(ctopt(UPAGES));
		forkstat.cntvfork++;
		forkstat.sizvfork += rip->p_dsize + rip->p_ssize;
	} else {
		rpp->p_tsize = rip->p_tsize;
		rpp->p_dsize = rip->p_dsize;
		rpp->p_ssize = rip->p_ssize;
		rpp->p_szpt = rip->p_szpt;
		forkstat.cntfork++;
		forkstat.sizfork += rip->p_dsize + rip->p_ssize;
	}
	rpp->p_rssize = 0;
	rpp->p_wchan = 0;
	rpp->p_slptime = 0;
	rpp->p_aveflt = rip->p_aveflt;
	rate.v_pgin += rip->p_aveflt;
	rpp->p_faults = 0;
	n = PIDHASH(rpp->p_pid);
	p->p_idhash = pidhash[n];
	pidhash[n] = rpp - proc;

	/*
	 * make duplicate entries
	 * where needed
	 */

	multprog++;

	for(n=0; n<NOFILE; n++)
		if(u.u_ofile[n] != NULL) {
			u.u_ofile[n]->f_count++;
			if(!isvfork && u.u_vrpages[n])
				u.u_ofile[n]->f_inode->i_vfdcnt++;
		}

	u.u_cdir->i_count++;
	if (u.u_rdir)
		u.u_rdir->i_count++;
	/*
	 * Partially simulate the environment
	 * of the new process so that when it is actually
	 * created (by copying) it will look right.
	 */

	rip->p_flag |= SKEEP;	/* prevent parent from being swapped */

	if (procdup(rpp, isvfork))
		return (1);

	spl6();
	rpp->p_stat = SRUN;
	setrq(rpp);
	spl0();
	/* SSWAP NOT NEEDED IN THIS CASE AS u.u_pcb.pcb_sswap SUFFICES */
	/* rpp->p_flag |= SSWAP; */
	rip->p_flag &= ~SKEEP;
	if (isvfork) {
		u.u_procp->p_xlink = rpp;
		u.u_procp->p_flag |= SNOVM;
		while (rpp->p_flag & SVFORK)
			sleep((caddr_t)rpp, PZERO - 1);
		if ((rpp->p_flag & SLOAD) == 0)
			panic("newproc vfork");
		uaccess(rpp, Vfmap, &vfutl);
		u.u_procp->p_xlink = 0;
		vpassvm(rpp, u.u_procp, &vfutl, &u, Vfmap);
		for (n = 0; n < NOFILE; n++)
			if (vfutl.u_vrpages[n]) {
				if ((u.u_vrpages[n] = vfutl.u_vrpages[n] - 1) == 0)
					if (--u.u_ofile[n]->f_inode->i_vfdcnt < 0)
						panic("newproc i_vfdcnt");
				vfutl.u_vrpages[n] = 0;
			}
		u.u_procp->p_flag &= ~SNOVM;
		rpp->p_ndx = rpp - proc;
		rpp->p_flag |= SVFDONE;
		wakeup((caddr_t)rpp);
	}
	return (0);
}
