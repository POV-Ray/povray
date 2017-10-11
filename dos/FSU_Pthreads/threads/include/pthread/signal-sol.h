/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * Copyright (c) 1998-1999, by Sun Microsystems, Inc.
 * All rights reserved.
 */


#ifndef _SIGNAL_H
#define	_SIGNAL_H

#pragma ident	"@(#)signal.h	1.38	99/08/10 SMI"	/* SVr4.0 1.5.3.4 */

#include <sys/feature_tests.h>

#if defined(__EXTENSIONS__) || __STDC__ == 0 || \
	defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)
#include <sys/types.h>	/* need pid_t/uid_t/size_t/clock_t/caddr_t/pthread_t */
#endif

#if defined(SOLARIS_NP) && #if RELEASE_NP > 56
#include <iso/signal_iso.h>
#endif
#include <sys/signal.h>

/*
 * Allow global visibility for symbols defined in
 * C++ "std" namespace in <iso/signal_iso.h>.
 */
#if __cplusplus >= 199711L
using std::sig_atomic_t;
using std::signal;
using std::raise;
#endif

#ifdef	__cplusplus
extern "C" {
#endif


#if defined(__STDC__)

extern const char	**_sys_siglistp;	/* signal descriptions */
extern const int	_sys_siglistn;		/* # of signal descriptions */

#if defined(__EXTENSIONS__) || \
	(!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE))
#define	_sys_siglist	_sys_siglistp
#define	_sys_nsig	_sys_siglistn
#endif

#if defined(__EXTENSIONS__) || __STDC__ == 0 || \
	defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)
extern int kill(pid_t, int);
extern int sigaction(int, const struct sigaction *, struct sigaction *);
#ifndef	_KERNEL
extern int sigaddset(sigset_t *, int);
extern int sigdelset(sigset_t *, int);
extern int sigemptyset(sigset_t *);
extern int sigfillset(sigset_t *);
extern int sigismember(const sigset_t *, int);
#endif
extern int sigpending(sigset_t *);
extern int sigprocmask(int, const sigset_t *, sigset_t *);
extern int sigsuspend(const sigset_t *);
#endif /* /* defined(__EXTENSIONS__) || __STDC__ == 0 ... */ */

#if defined(__EXTENSIONS__) || (__STDC__ == 0 && \
	!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE))
#include <sys/procset.h>
extern int gsignal(int);
extern int (*ssignal(int, int (*)(int)))(int);
extern int sigsend(idtype_t, id_t, int);
extern int sigsendset(const procset_t *, int);
extern int sig2str(int, char *);
extern int str2sig(const char *, int *);
#define	SIG2STR_MAX	32
#endif /* /* defined(__EXTENSIONS__) || (__STDC__ == 0 ... */ */

#if defined(__EXTENSIONS__) || (__STDC__ == 0 && \
	!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
	defined(_XPG4_2)
extern void (*bsd_signal(int, void (*)(int)))(int);
extern int killpg(pid_t, int);
extern int siginterrupt(int, int);
extern int sigaltstack(const stack_t *, stack_t *);
extern int sighold(int);
extern int sigignore(int);
extern int sigpause(int);
extern int sigrelse(int);
extern void (*sigset(int, void (*)(int)))(int);
extern int sigstack(struct sigstack *, struct sigstack *);
#endif /* /* defined(__EXTENSIONS__) || (__STDC__ == 0 ... */ */

#if defined(__EXTENSIONS__) || (__STDC__ == 0 && \
	!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
	(_POSIX_C_SOURCE > 2)
#include <sys/siginfo.h>
#include <time.h>
#ifndef SOLARIS_NP
extern int pthread_kill(pthread_t, int);
#endif
extern int pthread_sigmask(int, const sigset_t *, sigset_t *);
extern int sigwaitinfo(const sigset_t *, siginfo_t *);
extern int sigtimedwait(const sigset_t *, siginfo_t *, const struct timespec *);
extern int sigqueue(pid_t, int, const union sigval);
#endif /* /* defined(__EXTENSIONS__) || (__STDC__ == 0 ... */ */

#else /* /* __STDC__ */ */

extern char	**_sys_siglistp;	/* signal descriptions */
extern int	_sys_siglistn;		/* # of signal descriptions */

#define	_sys_siglist	_sys_siglistp
#define	_sys_nsig	_sys_siglistn

extern	void(*signal())();
extern int raise();

extern int kill();
extern int sigaction();
#ifndef	_KERNEL
extern int sigaddset();
extern int sigdelset();
extern int sigemptyset();
extern int sigfillset();
extern int sigismember();
#endif
extern int sigpending();
extern int sigprocmask();
extern int sigsuspend();

#if defined(__EXTENSIONS__) || \
	(!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
	defined(_XPG4_2)
extern void (*bsd_signal())();
extern int killpg();
extern int siginterrupt();
extern int sigstack();
#endif /* /* defined(__EXTENSIONS__) ... */ */

#if defined(__EXTENSIONS__) || \
	(!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE))
extern int gsignal();
extern int (*ssignal)();
extern int sigsend();
extern int sigsendset();
extern int sig2str();
extern int str2sig();
#define	SIG2STR_MAX	32
#endif

#if defined(__EXTENSIONS__) || \
	(!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
	defined(_XPG4_2)
extern int sigaltstack();
extern int sighold();
extern int sigignore();
extern int sigpause();
extern int sigrelse();
extern void (*sigset())();
#endif

#if defined(__EXTENSIONS__) || \
	(!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
	(_POSIX_C_SOURCE > 2)
#include <sys/siginfo.h>
#include <sys/time.h>
extern int pthread_kill();
extern int pthread_sigmask();
extern int sigwaitinfo();
extern int sigtimedwait();
extern int sigqueue();
#endif

#endif /* /* __STDC__ */ */

/*
 * sigwait() prototype is defined here.
 */

#if	defined(__EXTENSIONS__) || (__STDC__ == 0 && \
	!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
	(_POSIX_C_SOURCE - 0 >= 199506L) || defined(_POSIX_PTHREAD_SEMANTICS)

#if	defined(__STDC__)

#if	(_POSIX_C_SOURCE - 0 >= 199506L) || defined(_POSIX_PTHREAD_SEMANTICS)

#ifdef __PRAGMA_REDEFINE_EXTNAME
extern int sigwait(const sigset_t *, int *);
#pragma redefine_extname sigwait __posix_sigwait
#else /* /* __PRAGMA_REDEFINE_EXTNAME */ */

static int
sigwait(const sigset_t *__setp, int *__signo)
{
	extern int __posix_sigwait(const sigset_t *, int *);
	return (__posix_sigwait(__setp, __signo));
}
#endif /* /* __PRAGMA_REDEFINE_EXTNAME */ */

#else /* /* (_POSIX_C_SOURCE - 0 >= 199506L) || ... */ */

#ifndef SOLARIS_NP
extern int sigwait(sigset_t *);
#endif

#endif /* /* (_POSIX_C_SOURCE - 0 >= 199506L) || ... */ */


#else /* /* __STDC__ */ */


#if	(_POSIX_C_SOURCE - 0 >= 199506L) || defined(_POSIX_PTHREAD_SEMANTICS)

#ifdef __PRAGMA_REDEFINE_EXTNAME
extern int sigwait();
#pragma redefine_extname sigwait __posix_sigwait
#else /* /* __PRAGMA_REDEFINE_EXTNAME */ */

static int
sigwait(__setp, __signo)
	sigset_t *__setp;
	int *__signo;
{
	extern int __posix_sigwait();
	return (__posix_sigwait(__setp, __signo));
}
#endif /* /* __PRAGMA_REDEFINE_EXTNAME */ */

#else /* /* (_POSIX_C_SOURCE - 0 >= 199506L) || ... */ */

extern int sigwait();

#endif /* /* (_POSIX_C_SOURCE - 0 >= 199506L) || ... */ */

#endif /* /* __STDC__ */ */

#endif /* /* defined(__EXTENSIONS__) || (__STDC__ == 0 ... */ */

#ifdef	__cplusplus
}
#endif

#endif /* /* _SIGNAL_H */ */
