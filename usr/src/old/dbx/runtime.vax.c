
/* Copyright (c) 1982 Regents of the University of California */

static	char sccsid[] = "@(#)runtime.vax.c	1.12 (Berkeley) %G%";

/*
 * Runtime organization dependent routines, mostly dealing with
 * activation records.
 */

#include "defs.h"
#include "runtime.h"
#include "process.h"
#include "machine.h"
#include "events.h"
#include "mappings.h"
#include "symbols.h"
#include "tree.h"
#include "eval.h"
#include "operators.h"
#include "object.h"
#include <sys/param.h>

#ifndef public
typedef struct Frame *Frame;

#include "machine.h"
#endif

#define NSAVEREG 12

struct Frame {
    Integer condition_handler;
    Integer mask;
    Address save_ap;		/* argument pointer */
    Address save_fp;		/* frame pointer */
    Address save_pc;		/* program counter */
    Word save_reg[NSAVEREG];	/* not necessarily there */
};

private Frame curframe = nil;
private struct Frame curframerec;
private Boolean walkingstack = false;

#define frameeq(f1, f2) ((f1)->save_fp == (f2)->save_fp)

/*
 * Set a frame to the current activation record.
 */

private getcurframe(frp)
register Frame frp;
{
    register int i;

    checkref(frp);
    frp->mask = reg(NREG);
    frp->save_ap = reg(ARGP);
    frp->save_fp = reg(FRP);
    frp->save_pc = reg(PROGCTR) + 1;
    for (i = 0; i < NSAVEREG; i++) {
	frp->save_reg[i] = reg(i);
    }
}

/*
 * Return a pointer to the next activation record up the stack.
 * Return nil if there is none.
 * Writes over space pointed to by given argument.
 */

#define bis(b, n) ((b & (1 << (n))) != 0)

private Frame nextframe(frp)
Frame frp;
{
    register Frame newfrp;
    struct Frame frame;
    register Integer i, j, mask;
    Address prev_frame, callpc; 
    static Integer ntramp = 0;

    newfrp = frp;
    prev_frame = frp->save_fp;

/*
 *  The check for interrupt generated frames is taken from adb with only
 *  partial understanding.  If you're in "sub" and on a sigxxx "sigsub"
 *  gets control, then the stack does NOT look like <main, sub, sigsub>.
 *
 *  As best I can make out it looks like:
 *
 *     <main, (machine check exception block + sub), sysframe, sigsub>.
 *
 *  When the signal occurs an exception block and a frame for the routine
 *  in which it occured are pushed on the user stack.  Then another frame
 *  is pushed corresponding to a call from the kernel to sigsub.
 *
 *  The addr in sub at which the exception occured is not in sub.save_pc
 *  but in the machine check exception block.  It is at the magic address
 *  fp + 84.
 *
 *  The current approach ignores the sys_frame (what adb reports as sigtramp)
 *  and takes the pc for sub from the exception block.  This allows the
 *  "where" command to report <main, sub, sigsub>, which seems reasonable.
 */

nextf:
    dread(&frame, prev_frame, sizeof(struct Frame));
    if (ntramp == 1) {
	dread(&callpc, prev_frame + 84, sizeof(callpc));
    } else {
	callpc = frame.save_pc;
    }
    if (frame.save_fp == nil) {
	newfrp = nil;
    } else if (callpc > 0x80000000 - 0x200 * UPAGES ) {
	 ntramp++;
	 prev_frame = frame.save_fp;
	 goto nextf;
    } else {
	frame.save_pc = callpc;
        ntramp = 0;
	mask = ((frame.mask >> 16) & 0x0fff);
	j = 0;
	for (i = 0; i < NSAVEREG; i++) {
	    if (bis(mask, i)) {
		newfrp->save_reg[i] = frame.save_reg[j];
		++j;
	    }
	}
	newfrp->condition_handler = frame.condition_handler;
	newfrp->mask = mask;
	newfrp->save_ap = frame.save_ap;
	newfrp->save_fp = frame.save_fp;
	newfrp->save_pc = frame.save_pc;
    }
    return newfrp;
}

/*
 * Get the current frame information in the given Frame and store the
 * associated function in the given value-result parameter.
 */

private getcurfunc (frp, fp)
Frame frp;
Symbol *fp;
{
    getcurframe(frp);
    *fp = whatblock(frp->save_pc);
}

/*
 * Return the frame associated with the next function up the call stack, or
 * nil if there is none.  The function is returned in a value-result parameter.
 * For "inline" functions the statically outer function and same frame
 * are returned.
 */

private Frame nextfunc (frp, fp)
Frame frp;
Symbol *fp;
{
    Symbol t;
    Frame nfrp;

    t = *fp;
    checkref(t);
    if (isinline(t)) {
	t = container(t);
	nfrp = frp;
    } else {
	nfrp = nextframe(frp);
	if (nfrp == nil) {
	    t = nil;
	} else {
	    t = whatblock(nfrp->save_pc);
	}
    }
    *fp = t;
    return nfrp;
}

/*
 * Return the frame associated with the given function.
 * If the function is nil, return the most recently activated frame.
 *
 * Static allocation for the frame.
 */

public Frame findframe(f)
Symbol f;
{
    register Frame frp;
    static struct Frame frame;
    Symbol p;

    frp = &frame;
    getcurframe(frp);
    if (f == nil)
	return (frp);
    /*
     * Starting at the current stack frame,
     * walk backwards looking for a symbol
     * match.  Beware of local blocks which
     * have a back pointer but no stack frame.
     */
    p = whatblock(frp->save_pc);
    while (p != f) {
	if (p == program) {
	    frp = nil;
	    break;
	}
	if (isinline(p)) {
	    p = container(p);
	    continue;
	}
	frp = nextframe(frp);
	if (frp == nil)
	   break;
	p = whatblock(frp->save_pc);
    }
    return (frp);
}

/*
 * Find the return address of the current procedure/function.
 */

public Address return_addr()
{
    Frame frp;
    Address addr;
    struct Frame frame;

    frp = &frame;
    getcurframe(frp);
    frp = nextframe(frp);
    if (frp == nil) {
	addr = 0;
    } else {
	addr = frp->save_pc;
    }
    return addr;
}

/*
 * Push the value associated with the current function.
 */

public pushretval(len, isindirect)
Integer len;
Boolean isindirect;
{
    Word r0;

    r0 = reg(0);
    if (isindirect) {
	rpush((Address) r0, len);
    } else {
	switch (len) {
	    case sizeof(char):
		push(char, r0);
		break;

	    case sizeof(short):
		push(short, r0);
		break;

	    default:
		if (len == sizeof(Word)) {
		    push(Word, r0);
		} else if (len == 2*sizeof(Word)) {
		    push(Word, r0);
		    push(Word, reg(1));
		} else {
		    panic("not indirect in pushretval?");
		}
		break;
	}
    }
}

/*
 * Return the base address for locals in the given frame.
 */

public Address locals_base(frp)
register Frame frp;
{
    return (frp == nil) ? reg(FRP) : frp->save_fp;
}

/*
 * Return the base address for arguments in the given frame.
 */

public Address args_base(frp)
register Frame frp;
{
    return (frp == nil) ? reg(ARGP) : frp->save_ap;
}

/*
 * Return saved register n from the given frame.
 */

public Word savereg(n, frp)
register Integer n;
register Frame frp;
{
    register Word w;

    if (frp == nil) {
	w = reg(n);
    } else {
	switch (n) {
	    case ARGP:
		w = frp->save_ap;
		break;

	    case FRP:
		w = frp->save_fp;
		break;

	    case STKP:
		w = reg(STKP);
		break;

	    case PROGCTR:
		w = frp->save_pc;
		break;

	    default:
		assert(n >= 0 and n < NSAVEREG);
		w = frp->save_reg[n];
		break;
	}
    }
    return w;
}

/*
 * Return the nth argument to the current procedure.
 */

public Word argn(n, frp)
Integer n;
Frame frp;
{
    Word w;

    dread(&w, args_base(frp) + (n * sizeof(Word)), sizeof(w));
    return w;
}

/*
 * Calculate the entry address for a procedure or function parameter,
 * given the address of the descriptor.
 */

public Address fparamaddr(a)
Address a;
{
    Address r;

    dread(&r, a, sizeof(r));
    return r;
}

/*
 * Print a list of currently active blocks starting with most recent.
 */

public wherecmd()
{
    walkstack(false);
}

/*
 * Dump the world to the given file.
 * Like "where", but variables are dumped also.
 */

public dump()
{
    walkstack(true);
}

/*
 * Walk the stack of active procedures printing information
 * about each active procedure.
 */

private walkstack(dumpvariables)
Boolean dumpvariables;
{
    register Frame frp;
    register Boolean save;
    register Lineno line;
    Symbol f;
    struct Frame frame;

    if (notstarted(process)) {
	error("program is not active");
    } else {
	save = walkingstack;
	walkingstack = true;
	frp = &frame;
	getcurfunc(frp, &f);
	do {
	    printf("%s", symname(f));
	    if (not isinline(f)) {
		printparams(f, frp);
	    }
	    line = srcline(frp->save_pc - 1);
	    if (line != 0) {
		printf(", line %d", line);
		printf(" in \"%s\"\n", srcfilename(frp->save_pc - 1));
	    } else {
		printf(" at 0x%x\n", frp->save_pc);
	    }
	    if (dumpvariables) {
		dumpvars(f, frp);
		putchar('\n');
	    }
	    frp = nextfunc(frp, &f);
	} while (frp != nil and f != program);
	if (dumpvariables) {
	    printf("in \"%s\":\n", symname(program));
	    dumpvars(program, nil);
	    putchar('\n');
	}
	walkingstack = save;
    }
}

/*
 * Set the current function to the given symbol.
 * We must adjust "curframe" so that subsequent operations are
 * not confused; for simplicity we simply clear it.
 */

public setcurfunc (f)
Symbol f;
{
    curfunc = f;
    curframe = nil;
}

/*
 * Set curfunc to be N up/down the stack from its current value.
 */

public up (n)
integer n;
{
    integer i;
    Symbol f;
    Frame frp;
    boolean done;

    if (not isactive(program)) {
	error("program is not active");
    } else if (curfunc == nil) {
	error("no current function");
    } else {
	i = 0;
	f = curfunc;
	if (curframe != nil) {
	    frp = curframe;
	} else {
	    frp = findframe(f);
	}
	done = false;
	do {
	    if (frp == nil) {
		done = true;
		error("not that many levels");
	    } else if (i >= n) {
		done = true;
		curfunc = f;
		curframe = &curframerec;
		*curframe = *frp;
	    } else if (f == program) {
		done = true;
		error("not that many levels");
	    } else {
		frp = nextfunc(frp, &f);
	    }
	    ++i;
	} while (not done);
    }
}

public down (n)
integer n;
{
    integer i, depth;
    register Frame frp;
    Symbol f;
    struct Frame frame;

    if (not isactive(program)) {
	error("program is not active");
    } else if (curfunc == nil) {
	error("no current function");
    } else {
	depth = 0;
	frp = &frame;
	getcurfunc(frp, &f);
	if (curframe == nil) {
	    curframe = &curframerec;
	    *curframe = *(findframe(curfunc));
	}
	while ((f != curfunc or !frameeq(frp, curframe)) and f != nil) {
	    frp = nextfunc(frp, &f);
	    ++depth;
	}
	if (f == nil or n > depth) {
	    error("not that many levels");
	} else {
	    depth -= n;
	    frp = &frame;
	    getcurfunc(frp, &f);
	    for (i = 0; i < depth; i++) {
		frp = nextfunc(frp, &f);
		assert(frp != nil);
	    }
	    curfunc = f;
	    *curframe = *frp;
	}
    }
}

/*
 * Find the entry point of a procedure or function.
 */

public findbeginning(f)
Symbol f;
{
    if (isinternal(f)) {
	f->symvalue.funcv.beginaddr += 15;
    } else {
	f->symvalue.funcv.beginaddr += 2;
    }
}

/*
 * Return the address corresponding to the first line in a function.
 */

public Address firstline(f)
Symbol f;
{
    Address addr;

    addr = codeloc(f);
    while (linelookup(addr) == 0 and addr < objsize) {
	++addr;
    }
    if (addr == objsize) {
	addr = -1;
    }
    return addr;
}

/*
 * Catcher drops strike three ...
 */

public runtofirst()
{
    Address addr;

    addr = pc;
    while (linelookup(addr) == 0 and addr < objsize) {
	++addr;
    }
    if (addr < objsize) {
	stepto(addr);
    }
}

/*
 * Return the address corresponding to the end of the program.
 *
 * We look for the entry to "exit".
 */

public Address lastaddr()
{
    register Symbol s;

    s = lookup(identname("exit", true));
    if (s == nil) {
	panic("can't find exit");
    }
    return codeloc(s);
}

/*
 * Decide if the given function is currently active.
 *
 * We avoid calls to "findframe" during a stack trace for efficiency.
 * Presumably information evaluated while walking the stack is active.
 */

public Boolean isactive(f)
Symbol f;
{
    register Boolean b;

    if (isfinished(process)) {
	b = false;
    } else {
	if (walkingstack or f == program or
	  (ismodule(f) and isactive(container(f)))) {
	    b = true;
	} else {
	    b = (Boolean) (findframe(f) != nil);
	}
    }
    return b;
}

/*
 * Evaluate a call to a procedure.
 */

public callproc(procnode, arglist)
Node procnode;
Node arglist;
{
    Symbol proc;
    Integer argc;

    if (procnode->op != O_SYM) {
	beginerrmsg();
	fprintf(stderr, "can't call \"");
	prtree(stderr, procnode);
	fprintf(stderr, "\"");
	enderrmsg();
    }
    assert(procnode->op == O_SYM);
    proc = procnode->value.sym;
    if (not isblock(proc)) {
	error("\"%s\" is not a procedure or function", symname(proc));
    }
    pushenv();
    pc = codeloc(proc);
    argc = pushargs(proc, arglist);
    beginproc(proc, argc);
    isstopped = true;
    event_once(build(O_EQ, build(O_SYM, pcsym), build(O_SYM, retaddrsym)),
	buildcmdlist(build(O_PROCRTN, proc)));
    cont(0);
    /* NOTREACHED */
}

/*
 * Push the arguments on the process' stack.  We do this by first
 * evaluating them on the "eval" stack, then copying into the process'
 * space.
 */

private Integer pushargs(proc, arglist)
Symbol proc;
Node arglist;
{
    Stack *savesp;
    int argc, args_size;

    savesp = sp;
    argc = evalargs(proc, arglist);
    args_size = sp - savesp;
    setreg(STKP, reg(STKP) - args_size);
    dwrite(savesp, reg(STKP), args_size);
    sp = savesp;
    return argc;
}

/*
 * Check to see if an expression is correct for a given parameter.
 * If the given parameter is false, don't worry about type inconsistencies.
 *
 * Return whether or not it is ok.
 */

private boolean chkparam (actual, formal, chk)
Node actual;
Symbol formal;
boolean chk;
{
    boolean b;

    b = true;
    if (chk) {
	if (formal == nil) {
	    beginerrmsg();
	    fprintf(stderr, "too many parameters");
	    b = false;
	} else if (not compatible(formal->type, actual->nodetype)) {
	    beginerrmsg();
	    fprintf(stderr, "type mismatch for %s", symname(formal));
	    b = false;
	}
    }
    if (b and formal != nil and isvarparam(formal) and
	not isopenarray(formal->type) and actual->op != O_RVAL)
    {
	beginerrmsg();
	fprintf(stderr, "expected variable, found \"");
	prtree(stderr, actual);
	fprintf(stderr, "\"");
	b = false;
    }
    return b;
}

/*
 * Pass an expression to a particular parameter.
 *
 * Normally we pass either the address or value, but in some cases
 * (such as C strings) we want to copy the value onto the stack and
 * pass its address.
 */

private passparam (actual, formal)
Node actual;
Symbol formal;
{
    boolean b;
    Address addr;
    Stack *savesp;
    integer paramsize;

    if (isvarparam(formal) and not isopenarray(formal->type)) {
	addr = lval(actual->value.arg[0]);
	push(Address, addr);
    } else if (passaddr(formal, actual->nodetype)) {
	savesp = sp;
	eval(actual);
	paramsize = sp - savesp;
	setreg(STKP, reg(STKP) - paramsize);
	dwrite(savesp, reg(STKP), paramsize);
	sp = savesp;
	push(Address, reg(STKP));
	if (formal != nil and isopenarray(formal->type)) {
	    push(integer, paramsize div size(formal->type->type));
	}
    } else {
	eval(actual);
    }
}

/*
 * Evaluate an argument list left-to-right.
 */

private Integer evalargs(proc, arglist)
Symbol proc;
Node arglist;
{
    Node p, actual;
    Symbol formal;
    Stack *savesp;
    Integer count;
    boolean chk;

    savesp = sp;
    count = 0;
    formal = proc->chain;
    chk = (boolean) (not nosource(proc));
    for (p = arglist; p != nil; p = p->value.arg[1]) {
	assert(p->op == O_COMMA);
	actual = p->value.arg[0];
	if (not chkparam(actual, formal, chk)) {
	    fprintf(stderr, " in call to %s", symname(proc));
	    sp = savesp;
	    enderrmsg();
	}
	passparam(actual, formal);
	if (formal != nil) {
	    formal = formal->chain;
	}
	++count;
    }
    if (chk) {
	if (formal != nil) {
	    sp = savesp;
	    error("not enough parameters to %s", symname(proc));
	}
    }
    return count;
}

public procreturn(f)
Symbol f;
{
    flushoutput();
    putchar('\n');
    printname(stdout, f);
    printf(" returns successfully\n");
    popenv();
    erecover();
}

/*
 * Push the current environment.
 */

private pushenv()
{
    push(Address, pc);
    push(Lineno, curline);
    push(String, cursource);
    push(Boolean, isstopped);
    push(Symbol, curfunc);
    push(Frame, curframe);
    push(struct Frame, curframerec);
    push(Word, reg(PROGCTR));
    push(Word, reg(STKP));
}

/*
 * Pop back to the real world.
 */

public popenv()
{
    register String filename;

    setreg(STKP, pop(Word));
    setreg(PROGCTR, pop(Word));
    curframerec = pop(struct Frame);
    curframe = pop(Frame);
    curfunc = pop(Symbol);
    isstopped = pop(Boolean);
    filename = pop(String);
    curline = pop(Lineno);
    pc = pop(Address);
    setsource(filename);
}

/*
 * Flush the debuggee's standard output.
 *
 * This is VERY dependent on the use of stdio.
 */

public flushoutput()
{
    register Symbol p, iob;
    register Stack *savesp;

    p = lookup(identname("fflush", true));
    while (p != nil and not isblock(p)) {
	p = p->next_sym;
    }
    if (p != nil) {
	iob = lookup(identname("_iob", true));
	if (iob != nil) {
	    pushenv();
	    pc = codeloc(p);
	    savesp = sp;
	    push(long, address(iob, nil) + sizeof(struct _iobuf));
	    setreg(STKP, reg(STKP) - sizeof(long));
	    dwrite(savesp, reg(STKP), sizeof(long));
	    sp = savesp;
	    beginproc(p, 1);
	    stepto(return_addr());
	    popenv();
	}
    }
}
