/*
 * sysentvec for native FreeBSD a.out executable format.
 *
 * $Id: init_sysvec.c,v 1.4 1997/02/22 09:38:59 peter Exp $
 */

#include <sys/param.h>
#include <sys/sysent.h>
#include <sys/syscall.h>
#include <sys/signalvar.h>
#include <machine/md_var.h>

struct sysentvec aout_sysvec = {
	SYS_MAXSYSCALL,
	sysent,
	0,
	0,
	0,
	0,
	0,
	0,
	sendsig,
	sigcode,
	&szsigcode,
	0,
	"FreeBSD a.out"
};
