#define DOOP(ppname) PUTBACK; PL_op = ppname(ARGS); SPAGAIN

#define PP_LIST(g) do {			\
	dMARK;				\
	if (g != G_ARRAY) {		\
	    if (++MARK <= SP)		\
		*MARK = *SP;		\
	    else			\
		*MARK = &PL_sv_undef;	\
	    SP = MARK;			\
	}				\
   } while (0)

#define MAYBE_TAINT_SASSIGN_SRC(sv) \
    if (PL_tainting && PL_tainted && (!SvGMAGICAL(left) || !SvSMAGICAL(left) || \
                                !((mg=mg_find(left, 't')) && mg->mg_len & 1)))\
        TAINT_NOT

#define PP_PREINC(sv) do {	\
	if (SvIOK(sv)) {	\
            ++SvIVX(sv);	\
	    SvFLAGS(sv) &= ~(SVf_NOK|SVf_POK|SVp_NOK|SVp_POK); \
	}			\
	else			\
	    sv_inc(sv);		\
	SvSETMAGIC(sv);		\
    } while (0)

#define PP_UNSTACK do {		\
	TAINT_NOT;		\
	PL_stack_sp = PL_stack_base + cxstack[cxstack_ix].blk_oldsp;	\
	FREETMPS;		\
	oldsave = PL_scopestack[PL_scopestack_ix - 1]; \
	LEAVE_SCOPE(oldsave);	\
	SPAGAIN;		\
    } while(0)

/* Anyone using eval "" deserves this mess */
#define PP_EVAL(ppaddr, nxt) do {		\
	dJMPENV;				\
	int ret;				\
	PUTBACK;				\
	JMPENV_PUSH(ret);			\
	switch (ret) {				\
	case 0:					\
	    PL_op = ppaddr(ARGS);			\
	    PL_retstack[PL_retstack_ix - 1] = Nullop;	\
	    if (PL_op != nxt) runops();		\
	    JMPENV_POP;				\
	    break;				\
	case 1: JMPENV_POP; JMPENV_JUMP(1);	\
	case 2: JMPENV_POP; JMPENV_JUMP(2);	\
	case 3:					\
	    JMPENV_POP;				\
	    if (PL_restartop != nxt)		\
		JMPENV_JUMP(3);			\
	}					\
	PL_op = nxt;				\
	SPAGAIN;				\
    } while (0)

#define PP_ENTERTRY(jmpbuf,label) do {		\
	dJMPENV;				\
	int ret;				\
	JMPENV_PUSH(ret);			\
	switch (ret) {				\
	case 1: JMPENV_POP; JMPENV_JUMP(1);	\
	case 2: JMPENV_POP; JMPENV_JUMP(2);	\
	case 3: JMPENV_POP; SPAGAIN; goto label;\
	}					\
    } while (0)
