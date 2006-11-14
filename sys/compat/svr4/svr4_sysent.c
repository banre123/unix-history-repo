/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * $FreeBSD$
 * created from FreeBSD: src/sys/compat/svr4/syscalls.master,v 1.17.2.2 2005/07/20 17:42:15 jhb Exp 
 */

#include <bsm/audit_kevents.h>
#include <sys/types.h>
#include <sys/sysent.h>
#include <sys/sysproto.h>
#include <netinet/in.h>
#include <compat/svr4/svr4.h>
#include <compat/svr4/svr4_types.h>
#include <compat/svr4/svr4_signal.h>
#include <compat/svr4/svr4_proto.h>

#define AS(name) (sizeof(struct name) / sizeof(register_t))

/* The casts are bogus but will do for now. */
struct sysent svr4_sysent[] = {
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 0 = unused */
	{ SYF_MPSAFE | AS(sys_exit_args), (sy_call_t *)sys_exit, AUE_NULL },	/* 1 = exit */
	{ SYF_MPSAFE | 0, (sy_call_t *)fork, AUE_NULL },	/* 2 = fork */
	{ SYF_MPSAFE | AS(read_args), (sy_call_t *)read, AUE_NULL },	/* 3 = read */
	{ SYF_MPSAFE | AS(write_args), (sy_call_t *)write, AUE_NULL },	/* 4 = write */
	{ SYF_MPSAFE | AS(svr4_sys_open_args), (sy_call_t *)svr4_sys_open, AUE_NULL },	/* 5 = svr4_sys_open */
	{ SYF_MPSAFE | AS(close_args), (sy_call_t *)close, AUE_NULL },	/* 6 = close */
	{ SYF_MPSAFE | AS(svr4_sys_wait_args), (sy_call_t *)svr4_sys_wait, AUE_NULL },	/* 7 = svr4_sys_wait */
	{ SYF_MPSAFE | AS(svr4_sys_creat_args), (sy_call_t *)svr4_sys_creat, AUE_NULL },	/* 8 = svr4_sys_creat */
	{ SYF_MPSAFE | AS(link_args), (sy_call_t *)link, AUE_NULL },	/* 9 = link */
	{ SYF_MPSAFE | AS(unlink_args), (sy_call_t *)unlink, AUE_NULL },	/* 10 = unlink */
	{ SYF_MPSAFE | AS(svr4_sys_execv_args), (sy_call_t *)svr4_sys_execv, AUE_NULL },	/* 11 = svr4_sys_execv */
	{ SYF_MPSAFE | AS(chdir_args), (sy_call_t *)chdir, AUE_NULL },	/* 12 = chdir */
	{ SYF_MPSAFE | AS(svr4_sys_time_args), (sy_call_t *)svr4_sys_time, AUE_NULL },	/* 13 = svr4_sys_time */
	{ SYF_MPSAFE | AS(svr4_sys_mknod_args), (sy_call_t *)svr4_sys_mknod, AUE_NULL },	/* 14 = svr4_sys_mknod */
	{ SYF_MPSAFE | AS(chmod_args), (sy_call_t *)chmod, AUE_NULL },	/* 15 = chmod */
	{ SYF_MPSAFE | AS(chown_args), (sy_call_t *)chown, AUE_NULL },	/* 16 = chown */
	{ AS(svr4_sys_break_args), (sy_call_t *)svr4_sys_break, AUE_NULL },	/* 17 = svr4_sys_break */
	{ SYF_MPSAFE | AS(svr4_sys_stat_args), (sy_call_t *)svr4_sys_stat, AUE_NULL },	/* 18 = svr4_sys_stat */
	{ SYF_MPSAFE | AS(lseek_args), (sy_call_t *)lseek, AUE_NULL },	/* 19 = lseek */
	{ SYF_MPSAFE | 0, (sy_call_t *)getpid, AUE_NULL },	/* 20 = getpid */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 21 = old_mount */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 22 = sysv_umount */
	{ SYF_MPSAFE | AS(setuid_args), (sy_call_t *)setuid, AUE_NULL },	/* 23 = setuid */
	{ SYF_MPSAFE | 0, (sy_call_t *)getuid, AUE_NULL },	/* 24 = getuid */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 25 = stime */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 26 = ptrace */
	{ SYF_MPSAFE | AS(svr4_sys_alarm_args), (sy_call_t *)svr4_sys_alarm, AUE_NULL },	/* 27 = svr4_sys_alarm */
	{ SYF_MPSAFE | AS(svr4_sys_fstat_args), (sy_call_t *)svr4_sys_fstat, AUE_NULL },	/* 28 = svr4_sys_fstat */
	{ SYF_MPSAFE | 0, (sy_call_t *)svr4_sys_pause, AUE_NULL },	/* 29 = svr4_sys_pause */
	{ SYF_MPSAFE | AS(svr4_sys_utime_args), (sy_call_t *)svr4_sys_utime, AUE_NULL },	/* 30 = svr4_sys_utime */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 31 = stty */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 32 = gtty */
	{ SYF_MPSAFE | AS(svr4_sys_access_args), (sy_call_t *)svr4_sys_access, AUE_NULL },	/* 33 = svr4_sys_access */
	{ SYF_MPSAFE | AS(svr4_sys_nice_args), (sy_call_t *)svr4_sys_nice, AUE_NULL },	/* 34 = svr4_sys_nice */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 35 = statfs */
	{ SYF_MPSAFE | 0, (sy_call_t *)sync, AUE_NULL },	/* 36 = sync */
	{ SYF_MPSAFE | AS(svr4_sys_kill_args), (sy_call_t *)svr4_sys_kill, AUE_NULL },	/* 37 = svr4_sys_kill */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 38 = fstatfs */
	{ SYF_MPSAFE | AS(svr4_sys_pgrpsys_args), (sy_call_t *)svr4_sys_pgrpsys, AUE_NULL },	/* 39 = svr4_sys_pgrpsys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 40 = xenix */
	{ SYF_MPSAFE | AS(dup_args), (sy_call_t *)dup, AUE_NULL },	/* 41 = dup */
	{ SYF_MPSAFE | 0, (sy_call_t *)pipe, AUE_NULL },	/* 42 = pipe */
	{ SYF_MPSAFE | AS(svr4_sys_times_args), (sy_call_t *)svr4_sys_times, AUE_NULL },	/* 43 = svr4_sys_times */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 44 = profil */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 45 = plock */
	{ SYF_MPSAFE | AS(setgid_args), (sy_call_t *)setgid, AUE_NULL },	/* 46 = setgid */
	{ SYF_MPSAFE | 0, (sy_call_t *)getgid, AUE_NULL },	/* 47 = getgid */
	{ SYF_MPSAFE | AS(svr4_sys_signal_args), (sy_call_t *)svr4_sys_signal, AUE_NULL },	/* 48 = svr4_sys_signal */
	{ SYF_MPSAFE | AS(svr4_sys_msgsys_args), (sy_call_t *)svr4_sys_msgsys, AUE_NULL },	/* 49 = svr4_sys_msgsys */
	{ SYF_MPSAFE | AS(svr4_sys_sysarch_args), (sy_call_t *)svr4_sys_sysarch, AUE_NULL },	/* 50 = svr4_sys_sysarch */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 51 = acct */
	{ AS(svr4_sys_shmsys_args), (sy_call_t *)svr4_sys_shmsys, AUE_NULL },	/* 52 = svr4_sys_shmsys */
	{ AS(svr4_sys_semsys_args), (sy_call_t *)svr4_sys_semsys, AUE_NULL },	/* 53 = svr4_sys_semsys */
	{ AS(svr4_sys_ioctl_args), (sy_call_t *)svr4_sys_ioctl, AUE_NULL },	/* 54 = svr4_sys_ioctl */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 55 = uadmin */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 56 = exch */
	{ SYF_MPSAFE | AS(svr4_sys_utssys_args), (sy_call_t *)svr4_sys_utssys, AUE_NULL },	/* 57 = svr4_sys_utssys */
	{ SYF_MPSAFE | AS(fsync_args), (sy_call_t *)fsync, AUE_NULL },	/* 58 = fsync */
	{ SYF_MPSAFE | AS(svr4_sys_execve_args), (sy_call_t *)svr4_sys_execve, AUE_NULL },	/* 59 = svr4_sys_execve */
	{ SYF_MPSAFE | AS(umask_args), (sy_call_t *)umask, AUE_NULL },	/* 60 = umask */
	{ SYF_MPSAFE | AS(chroot_args), (sy_call_t *)chroot, AUE_NULL },	/* 61 = chroot */
	{ SYF_MPSAFE | AS(svr4_sys_fcntl_args), (sy_call_t *)svr4_sys_fcntl, AUE_NULL },	/* 62 = svr4_sys_fcntl */
	{ SYF_MPSAFE | AS(svr4_sys_ulimit_args), (sy_call_t *)svr4_sys_ulimit, AUE_NULL },	/* 63 = svr4_sys_ulimit */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 64 = reserved */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 65 = reserved */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 66 = reserved */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 67 = reserved */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 68 = reserved */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 69 = reserved */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 70 = advfs */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 71 = unadvfs */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 72 = rmount */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 73 = rumount */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 74 = rfstart */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 75 = sigret */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 76 = rdebug */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 77 = rfstop */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 78 = rfsys */
	{ SYF_MPSAFE | AS(rmdir_args), (sy_call_t *)rmdir, AUE_NULL },	/* 79 = rmdir */
	{ SYF_MPSAFE | AS(mkdir_args), (sy_call_t *)mkdir, AUE_NULL },	/* 80 = mkdir */
	{ AS(svr4_sys_getdents_args), (sy_call_t *)svr4_sys_getdents, AUE_NULL },	/* 81 = svr4_sys_getdents */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 82 = libattach */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 83 = libdetach */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 84 = sysfs */
	{ AS(svr4_sys_getmsg_args), (sy_call_t *)svr4_sys_getmsg, AUE_NULL },	/* 85 = svr4_sys_getmsg */
	{ AS(svr4_sys_putmsg_args), (sy_call_t *)svr4_sys_putmsg, AUE_NULL },	/* 86 = svr4_sys_putmsg */
	{ SYF_MPSAFE | AS(svr4_sys_poll_args), (sy_call_t *)svr4_sys_poll, AUE_NULL },	/* 87 = svr4_sys_poll */
	{ SYF_MPSAFE | AS(svr4_sys_lstat_args), (sy_call_t *)svr4_sys_lstat, AUE_NULL },	/* 88 = svr4_sys_lstat */
	{ SYF_MPSAFE | AS(symlink_args), (sy_call_t *)symlink, AUE_NULL },	/* 89 = symlink */
	{ SYF_MPSAFE | AS(readlink_args), (sy_call_t *)readlink, AUE_NULL },	/* 90 = readlink */
	{ SYF_MPSAFE | AS(getgroups_args), (sy_call_t *)getgroups, AUE_NULL },	/* 91 = getgroups */
	{ SYF_MPSAFE | AS(setgroups_args), (sy_call_t *)setgroups, AUE_NULL },	/* 92 = setgroups */
	{ SYF_MPSAFE | AS(fchmod_args), (sy_call_t *)fchmod, AUE_NULL },	/* 93 = fchmod */
	{ SYF_MPSAFE | AS(fchown_args), (sy_call_t *)fchown, AUE_NULL },	/* 94 = fchown */
	{ SYF_MPSAFE | AS(svr4_sys_sigprocmask_args), (sy_call_t *)svr4_sys_sigprocmask, AUE_NULL },	/* 95 = svr4_sys_sigprocmask */
	{ SYF_MPSAFE | AS(svr4_sys_sigsuspend_args), (sy_call_t *)svr4_sys_sigsuspend, AUE_NULL },	/* 96 = svr4_sys_sigsuspend */
	{ SYF_MPSAFE | AS(svr4_sys_sigaltstack_args), (sy_call_t *)svr4_sys_sigaltstack, AUE_NULL },	/* 97 = svr4_sys_sigaltstack */
	{ SYF_MPSAFE | AS(svr4_sys_sigaction_args), (sy_call_t *)svr4_sys_sigaction, AUE_NULL },	/* 98 = svr4_sys_sigaction */
	{ SYF_MPSAFE | AS(svr4_sys_sigpending_args), (sy_call_t *)svr4_sys_sigpending, AUE_NULL },	/* 99 = svr4_sys_sigpending */
	{ SYF_MPSAFE | AS(svr4_sys_context_args), (sy_call_t *)svr4_sys_context, AUE_NULL },	/* 100 = svr4_sys_context */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 101 = evsys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 102 = evtrapret */
	{ SYF_MPSAFE | AS(svr4_sys_statvfs_args), (sy_call_t *)svr4_sys_statvfs, AUE_NULL },	/* 103 = svr4_sys_statvfs */
	{ SYF_MPSAFE | AS(svr4_sys_fstatvfs_args), (sy_call_t *)svr4_sys_fstatvfs, AUE_NULL },	/* 104 = svr4_sys_fstatvfs */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 105 = whoknows */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 106 = nfssvc */
	{ AS(svr4_sys_waitsys_args), (sy_call_t *)svr4_sys_waitsys, AUE_NULL },	/* 107 = svr4_sys_waitsys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 108 = sigsendsys */
	{ SYF_MPSAFE | AS(svr4_sys_hrtsys_args), (sy_call_t *)svr4_sys_hrtsys, AUE_NULL },	/* 109 = svr4_sys_hrtsys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 110 = acancel */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 111 = async */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 112 = priocntlsys */
	{ SYF_MPSAFE | AS(svr4_sys_pathconf_args), (sy_call_t *)svr4_sys_pathconf, AUE_NULL },	/* 113 = svr4_sys_pathconf */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 114 = mincore */
	{ SYF_MPSAFE | AS(svr4_sys_mmap_args), (sy_call_t *)svr4_sys_mmap, AUE_NULL },	/* 115 = svr4_sys_mmap */
	{ SYF_MPSAFE | AS(mprotect_args), (sy_call_t *)mprotect, AUE_NULL },	/* 116 = mprotect */
	{ SYF_MPSAFE | AS(munmap_args), (sy_call_t *)munmap, AUE_NULL },	/* 117 = munmap */
	{ SYF_MPSAFE | AS(svr4_sys_fpathconf_args), (sy_call_t *)svr4_sys_fpathconf, AUE_NULL },	/* 118 = svr4_sys_fpathconf */
	{ SYF_MPSAFE | 0, (sy_call_t *)vfork, AUE_NULL },	/* 119 = vfork */
	{ SYF_MPSAFE | AS(fchdir_args), (sy_call_t *)fchdir, AUE_NULL },	/* 120 = fchdir */
	{ SYF_MPSAFE | AS(readv_args), (sy_call_t *)readv, AUE_NULL },	/* 121 = readv */
	{ SYF_MPSAFE | AS(writev_args), (sy_call_t *)writev, AUE_NULL },	/* 122 = writev */
	{ SYF_MPSAFE | AS(svr4_sys_xstat_args), (sy_call_t *)svr4_sys_xstat, AUE_NULL },	/* 123 = svr4_sys_xstat */
	{ SYF_MPSAFE | AS(svr4_sys_lxstat_args), (sy_call_t *)svr4_sys_lxstat, AUE_NULL },	/* 124 = svr4_sys_lxstat */
	{ SYF_MPSAFE | AS(svr4_sys_fxstat_args), (sy_call_t *)svr4_sys_fxstat, AUE_NULL },	/* 125 = svr4_sys_fxstat */
	{ SYF_MPSAFE | AS(svr4_sys_xmknod_args), (sy_call_t *)svr4_sys_xmknod, AUE_NULL },	/* 126 = svr4_sys_xmknod */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 127 = clocal */
	{ SYF_MPSAFE | AS(svr4_sys_setrlimit_args), (sy_call_t *)svr4_sys_setrlimit, AUE_NULL },	/* 128 = svr4_sys_setrlimit */
	{ SYF_MPSAFE | AS(svr4_sys_getrlimit_args), (sy_call_t *)svr4_sys_getrlimit, AUE_NULL },	/* 129 = svr4_sys_getrlimit */
	{ SYF_MPSAFE | AS(lchown_args), (sy_call_t *)lchown, AUE_NULL },	/* 130 = lchown */
	{ SYF_MPSAFE | AS(svr4_sys_memcntl_args), (sy_call_t *)svr4_sys_memcntl, AUE_NULL },	/* 131 = svr4_sys_memcntl */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 132 = getpmsg */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 133 = putpmsg */
	{ SYF_MPSAFE | AS(rename_args), (sy_call_t *)rename, AUE_NULL },	/* 134 = rename */
	{ SYF_MPSAFE | AS(svr4_sys_uname_args), (sy_call_t *)svr4_sys_uname, AUE_NULL },	/* 135 = svr4_sys_uname */
	{ SYF_MPSAFE | AS(setegid_args), (sy_call_t *)setegid, AUE_NULL },	/* 136 = setegid */
	{ SYF_MPSAFE | AS(svr4_sys_sysconfig_args), (sy_call_t *)svr4_sys_sysconfig, AUE_NULL },	/* 137 = svr4_sys_sysconfig */
	{ SYF_MPSAFE | AS(adjtime_args), (sy_call_t *)adjtime, AUE_NULL },	/* 138 = adjtime */
	{ SYF_MPSAFE | AS(svr4_sys_systeminfo_args), (sy_call_t *)svr4_sys_systeminfo, AUE_NULL },	/* 139 = svr4_sys_systeminfo */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 140 = notused */
	{ SYF_MPSAFE | AS(seteuid_args), (sy_call_t *)seteuid, AUE_NULL },	/* 141 = seteuid */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 142 = vtrace */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 143 = { */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 144 = sigtimedwait */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 145 = lwp_info */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 146 = yield */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 147 = lwp_sema_wait */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 148 = lwp_sema_post */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 149 = lwp_sema_trywait */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 150 = notused */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 151 = notused */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 152 = modctl */
	{ AS(svr4_sys_fchroot_args), (sy_call_t *)svr4_sys_fchroot, AUE_NULL },	/* 153 = svr4_sys_fchroot */
	{ SYF_MPSAFE | AS(svr4_sys_utimes_args), (sy_call_t *)svr4_sys_utimes, AUE_NULL },	/* 154 = svr4_sys_utimes */
	{ SYF_MPSAFE | 0, (sy_call_t *)svr4_sys_vhangup, AUE_NULL },	/* 155 = svr4_sys_vhangup */
	{ SYF_MPSAFE | AS(svr4_sys_gettimeofday_args), (sy_call_t *)svr4_sys_gettimeofday, AUE_NULL },	/* 156 = svr4_sys_gettimeofday */
	{ SYF_MPSAFE | AS(getitimer_args), (sy_call_t *)getitimer, AUE_NULL },	/* 157 = getitimer */
	{ SYF_MPSAFE | AS(setitimer_args), (sy_call_t *)setitimer, AUE_NULL },	/* 158 = setitimer */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 159 = lwp_create */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 160 = lwp_exit */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 161 = lwp_suspend */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 162 = lwp_continue */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 163 = lwp_kill */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 164 = lwp_self */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 165 = lwp_getprivate */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 166 = lwp_setprivate */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 167 = lwp_wait */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 168 = lwp_mutex_unlock */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 169 = lwp_mutex_lock */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 170 = lwp_cond_wait */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 171 = lwp_cond_signal */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 172 = lwp_cond_broadcast */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 173 = { */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 174 = { */
	{ SYF_MPSAFE | AS(svr4_sys_llseek_args), (sy_call_t *)svr4_sys_llseek, AUE_NULL },	/* 175 = svr4_sys_llseek */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 176 = inst_sync */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 177 = whoknows */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 178 = kaio */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 179 = whoknows */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 180 = whoknows */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 181 = whoknows */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 182 = whoknows */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 183 = whoknows */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 184 = tsolsys */
	{ SYF_MPSAFE | AS(svr4_sys_acl_args), (sy_call_t *)svr4_sys_acl, AUE_NULL },	/* 185 = svr4_sys_acl */
	{ SYF_MPSAFE | AS(svr4_sys_auditsys_args), (sy_call_t *)svr4_sys_auditsys, AUE_NULL },	/* 186 = svr4_sys_auditsys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 187 = processor_bind */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 188 = processor_info */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 189 = p_online */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 190 = sigqueue */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 191 = clock_gettime */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 192 = clock_settime */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 193 = clock_getres */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 194 = timer_create */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 195 = timer_delete */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 196 = timer_settime */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 197 = timer_gettime */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 198 = timer_overrun */
	{ SYF_MPSAFE | AS(nanosleep_args), (sy_call_t *)nanosleep, AUE_NULL },	/* 199 = nanosleep */
	{ SYF_MPSAFE | AS(svr4_sys_facl_args), (sy_call_t *)svr4_sys_facl, AUE_NULL },	/* 200 = svr4_sys_facl */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 201 = door */
	{ SYF_MPSAFE | AS(setreuid_args), (sy_call_t *)setreuid, AUE_NULL },	/* 202 = setreuid */
	{ SYF_MPSAFE | AS(setregid_args), (sy_call_t *)setregid, AUE_NULL },	/* 203 = setregid */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 204 = install_utrap */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 205 = signotify */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 206 = schedctl */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 207 = pset */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 208 = whoknows */
	{ AS(svr4_sys_resolvepath_args), (sy_call_t *)svr4_sys_resolvepath, AUE_NULL },	/* 209 = svr4_sys_resolvepath */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 210 = signotifywait */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 211 = lwp_sigredirect */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 212 = lwp_alarm */
	{ AS(svr4_sys_getdents64_args), (sy_call_t *)svr4_sys_getdents64, AUE_NULL },	/* 213 = svr4_sys_getdents64 */
	{ SYF_MPSAFE | AS(svr4_sys_mmap64_args), (sy_call_t *)svr4_sys_mmap64, AUE_NULL },	/* 214 = svr4_sys_mmap64 */
	{ SYF_MPSAFE | AS(svr4_sys_stat64_args), (sy_call_t *)svr4_sys_stat64, AUE_NULL },	/* 215 = svr4_sys_stat64 */
	{ SYF_MPSAFE | AS(svr4_sys_lstat64_args), (sy_call_t *)svr4_sys_lstat64, AUE_NULL },	/* 216 = svr4_sys_lstat64 */
	{ SYF_MPSAFE | AS(svr4_sys_fstat64_args), (sy_call_t *)svr4_sys_fstat64, AUE_NULL },	/* 217 = svr4_sys_fstat64 */
	{ SYF_MPSAFE | AS(svr4_sys_statvfs64_args), (sy_call_t *)svr4_sys_statvfs64, AUE_NULL },	/* 218 = svr4_sys_statvfs64 */
	{ SYF_MPSAFE | AS(svr4_sys_fstatvfs64_args), (sy_call_t *)svr4_sys_fstatvfs64, AUE_NULL },	/* 219 = svr4_sys_fstatvfs64 */
	{ SYF_MPSAFE | AS(svr4_sys_setrlimit64_args), (sy_call_t *)svr4_sys_setrlimit64, AUE_NULL },	/* 220 = svr4_sys_setrlimit64 */
	{ SYF_MPSAFE | AS(svr4_sys_getrlimit64_args), (sy_call_t *)svr4_sys_getrlimit64, AUE_NULL },	/* 221 = svr4_sys_getrlimit64 */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 222 = pread64 */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 223 = pwrite64 */
	{ SYF_MPSAFE | AS(svr4_sys_creat64_args), (sy_call_t *)svr4_sys_creat64, AUE_NULL },	/* 224 = svr4_sys_creat64 */
	{ SYF_MPSAFE | AS(svr4_sys_open64_args), (sy_call_t *)svr4_sys_open64, AUE_NULL },	/* 225 = svr4_sys_open64 */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 226 = rpcsys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 227 = whoknows */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 228 = whoknows */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 229 = whoknows */
	{ SYF_MPSAFE | AS(svr4_sys_socket_args), (sy_call_t *)svr4_sys_socket, AUE_NULL },	/* 230 = svr4_sys_socket */
	{ SYF_MPSAFE | AS(socketpair_args), (sy_call_t *)socketpair, AUE_NULL },	/* 231 = socketpair */
	{ SYF_MPSAFE | AS(bind_args), (sy_call_t *)bind, AUE_NULL },	/* 232 = bind */
	{ SYF_MPSAFE | AS(listen_args), (sy_call_t *)listen, AUE_NULL },	/* 233 = listen */
	{ SYF_MPSAFE | AS(accept_args), (sy_call_t *)accept, AUE_NULL },	/* 234 = accept */
	{ SYF_MPSAFE | AS(connect_args), (sy_call_t *)connect, AUE_NULL },	/* 235 = connect */
	{ SYF_MPSAFE | AS(shutdown_args), (sy_call_t *)shutdown, AUE_NULL },	/* 236 = shutdown */
	{ SYF_MPSAFE | AS(svr4_sys_recv_args), (sy_call_t *)svr4_sys_recv, AUE_NULL },	/* 237 = svr4_sys_recv */
	{ SYF_MPSAFE | AS(recvfrom_args), (sy_call_t *)recvfrom, AUE_NULL },	/* 238 = recvfrom */
	{ SYF_MPSAFE | AS(recvmsg_args), (sy_call_t *)recvmsg, AUE_NULL },	/* 239 = recvmsg */
	{ SYF_MPSAFE | AS(svr4_sys_send_args), (sy_call_t *)svr4_sys_send, AUE_NULL },	/* 240 = svr4_sys_send */
	{ SYF_MPSAFE | AS(sendmsg_args), (sy_call_t *)sendmsg, AUE_NULL },	/* 241 = sendmsg */
	{ SYF_MPSAFE | AS(svr4_sys_sendto_args), (sy_call_t *)svr4_sys_sendto, AUE_NULL },	/* 242 = svr4_sys_sendto */
	{ SYF_MPSAFE | AS(getpeername_args), (sy_call_t *)getpeername, AUE_NULL },	/* 243 = getpeername */
	{ SYF_MPSAFE | AS(getsockname_args), (sy_call_t *)getsockname, AUE_NULL },	/* 244 = getsockname */
	{ SYF_MPSAFE | AS(getsockopt_args), (sy_call_t *)getsockopt, AUE_NULL },	/* 245 = getsockopt */
	{ SYF_MPSAFE | AS(setsockopt_args), (sy_call_t *)setsockopt, AUE_NULL },	/* 246 = setsockopt */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 247 = sockconfig */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 248 = { */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 249 = { */
};
