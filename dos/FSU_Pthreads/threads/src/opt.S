/* Copyright (C) 1992-2000 the Florida State University
   Distributed by the Florida State University under the terms of the
   GNU Library General Public License.

This file is part of Pthreads.

Pthreads is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation (version 2).

Pthreads is distributed "AS IS" in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with Pthreads; see the file COPYING.  If not, write
to the Free Software Foundation, 675 Mass Ave, Cambridge,
MA 02139, USA.

Report problems and direct all questions to:

  pthreads-bugs@ada.cs.fsu.edu

  @(#)opt.S	3.14 11/8/00

*/

/*
 * Optional assembly routines for speed
 *
 * Portability notes:
 * The assembly code in this file is only needed for the following conditional
 * compilation:
 * ASM_SETJMP:    [sig]set/longjmp (if not defined, C lib set/longjmp is used).
 * NOERR_CHECK:   fast mutex [un]locking (for slower version in C,
 *                do NOT defined -DNOERR_CHECK).
 * !CLEANUP_HEAP: cleanup handlers on stack (for slow version on heap,
 *                define CLEANUP_HEAP).
 */

#include <pthread/config.h>
#include <../src/config_internals.h>

#define LOCORE
#define _ASM

#ifdef SOLARIS
#include <sys/asm_linkage.h>
#define NAME(x) x
#else /* !SOLARIS */
#if defined(__FreeBSD__) || defined(_M_UNIX) || defined(__linux__) || defined(__dos__)
#else
#include <sparc/asm_linkage.h>
#ifdef NAME	
#undef NAME
#endif /* NAME */
#define	IDENTITY(x) x
#define NAME(x) IDENTITY(_)x
#endif
#endif /* !SOLARIS */

#if defined(__FreeBSD__) || defined(_M_UNIX) || defined(__linux__) || defined(__dos__)
#else
#include <sys/errno.h>
#endif

#include <pthread/unistd.h>
#include "offsets.h"
#include <pthread/asm.h>

#ifdef SOLARIS
#include <sys/trap.h>
#else /* !SORLARIS */
#if defined(__FreeBSD__)  || defined(_M_UNIX) || defined(__linux__) || defined(__dos__)
#else
#include <sparc/trap.h>
#endif /* !SOLARIS */
#endif /* !STAND_ALONE */

#ifdef ASM_SETJMP
!	int setjmp(env)
!	jmp_buf env;
!
!	setjmp - set up jump environment across procedures, preserve signal mask
!	longjmp() will return to this place and supply a return value
	ENTRY(setjmp)
	b	NAME(sigsetjmp)		! sigsetjmp(env, TRUE);
	mov	%sp,%o1

!	int sigsetjmp(env, savemask)
!	sigjmp_buf env;
!	int savemask;
!
!	sigsetjmp - set up jump environment across procedures,
!	preserve signal mask if savemask non-zero
!	siglongjmp() will return to this place and supply a return value
	ENTRY(sigsetjmp)
	save	%sp,-SA(MINFRAME),%sp	! Get a new window
	st	%fp,[%i0+jmp_sp]	! Save stack pointer of previous frame
	set	NAME(pthread_kern),%l2	! Get kernel address
	st	%i1,[%i0+jmp_svmask]	! env[JB_SVMASK] = TRUE;
	tst	%i1			! savemask == 0 ?
	be	sigsetjmp_nomask
	st	%i7,[%i0+jmp_pc]	! Delay: Save return PC
	ld	[%l2+pthread_self],%l2	! Get current thread
	add	%l2, mask, %o1		! Copy thread mask
sigsetjmp_savemask:
	call	NAME(pthread_sigcpyset2set)
	add	%i0, jmp_mask, %o0
sigsetjmp_nomask:
	clr	%i0
	ret
	restore

!	void longjmp(env, val)
!	jmp_buf env;
!	int val;
!
!	longjmp - execute jump across procedures according to save environment
!	and return specified value at corresponding setjmp().
!
!	implemented as alias og siglongjmp (see below)

!	void siglongjmp(env, val)
!	sigjmp_buf env;
!	int val;
!
!	siglongjmp - exec. jump across procedures according to save environment
!	and return specified value at corresponding sigsetjmp().
	ENTRY(longjmp)
	ENTRY(siglongjmp)
	save	%sp,-SA(MINFRAME),%sp	! Get a new window
	ta	ST_FLUSH_WINDOWS
	ld	[%i0+jmp_sp],%g1	! Saved stack pointer, so we think
	mov	%sp, %l2		! Current stack pointer
1:
	mov	%l2, %l3		! Follow chain of frame pointers
	ld	[%l2+(14*4)],%l2	! until fp(x) ==  fp(saved), then done.
	cmp	%g1, %l2		! Or we find a fp(x) < fp(saved), then
	bg	1b			! take the fp(x+1), i.e. one frame up
	nop				! Someone must have changed the stacks!
	be	2f
	mov	%l2, %fp		! Delay: Restore stack pointer fp(x)
	mov	%l3, %fp		! Cond. restore stack pointer (fp(x+1)
2:
	ld	[%i0+jmp_svmask],%l1	! if (env[JB_SVMASK])
	tst	%l1
	bz	siglongjmp_nomask
	mov	SIG_SETMASK, %o0	! Delay: Parameter 0: SIG_SETMASK
	add	%i0, jmp_mask, %o1	! Parameter 1: &env->mask
	call	NAME(sigprocmask)
	clr	%o2			! Delay: Parameter 2: NULL
siglongjmp_nomask:
	orcc	%i1,%g0,%o0		! val == 0 ?
	bne	siglongjmp_ret
	ld	[%i0+jmp_pc],%i7	! Delay: Restore return PC
	mov	1,%o0			! val = 1
siglongjmp_ret:
	restore	%o0, 0, %o0
	mov	%g1, %sp
	retl				! return(val);
	nop
#endif /* ASM_SETJMP */

#ifdef NOERR_CHECK
!	int pthread_mutex_lock(mutex)
!	pthread_mutex_t *mutex;
!
!	When mutex can be locked immediately, execute as leaf;
!	otherwise call C routine to suspend.
	ENTRY(pthread_mutex_lock)
#ifdef _POSIX_THREADS_PRIO_PROTECT
                                        ! load protocol type of mutex
	ld      [%o0+mutex_protocol],%o1
	cmp     %o1, PTHREAD_PRIO_PROTECT	! test if PTHREAD_PRIO_PROTECT
	be      slow_lock               ! yes
	nop                             ! Delay
#endif /* _POSIX_THREADS_PRIO_PROTECT */
	ldstub	[%o0+mutex_lock],%o1	! test_and_set(&mutex->lock)
	tst	%o1			! prev. lock == 0 ?
	bne	mutex_locked
	sethi	%hi(NAME(pthread_kern)),%o1	! Get kernel address	
	or	%o1,%lo(NAME(pthread_kern)),%o1
	ld	[%o1+pthread_self],%o1
	st	%o1,[%o0+mutex_owner]	! mutex->owner = mac_pthread_self()
	retl
	clr	%o0			! return(0);
#ifdef _POSIX_THREADS_PRIO_PROTECT
slow_lock:
#endif /* _POSIX_THREADS_PRIO_PROTECT */
mutex_locked:
	save	%sp,-SA(MINFRAME),%sp	! Get a new window
	call	NAME(slow_mutex_lock)	! return(slow_mutex_lock(mutex));
	mov	%i0, %o0		! Delay: Pass parameter
	mov	%o0, %i0		! Return value
	ret
	restore

!	int pthread_mutex_trylock(mutex)
!	pthread_mutex_t *mutex;
!
!	When mutex can be locked immediately, execute as leaf and return 1;
!	otherwise return 0
	ENTRY(pthread_mutex_trylock)
#ifdef _POSIX_THREADS_PRIO_PROTECT
                                        ! load protocol type of mutex
	ld      [%o0+mutex_protocol],%o1
	cmp     %o1, PTHREAD_PRIO_PROTECT	! test if PTHREAD_PRIO_PROTECT
	bne     fast_trylock		! yes
	nop                             ! Delay
	save	%sp,-SA(MINFRAME),%sp	! Get a new window
	call	NAME(slow_mutex_trylock)! return(slow_mutex_lock(mutex));
	mov	%i0, %o0		! Delay: Pass parameter
	mov	%o0, %i0		! Return value
	ret
	restore
fast_trylock:
#endif /* _POSIX_THREADS_PRIO_PROTECT */
	ldstub	[%o0+mutex_lock],%o1	! test_and_set(&mutex->lock)
	tst	%o1			! prev. lock == 0 ?
	bne	mutex_trylocked
	sethi	%hi(NAME(pthread_kern)),%o1	! Get kernel address	
	or	%o1,%lo(NAME(pthread_kern)),%o1
	ld	[%o1+pthread_self],%o1
	st	%o1,[%o0+mutex_owner]	! mutex->owner = mac_pthread_self()
	retl
	clr	%o0			! return(0);
mutex_trylocked:
	retl
	mov	EBUSY,%o0		! return(EBUSY);

!	int pthread_mutex_unlock(mutex)
!	pthread_mutex_t *mutex;
!
!	When mutex can be unlocked immediately, execute as leaf;
!	otherwise call C routine to wakeup a thread blocked on the mutex
	ENTRY(pthread_mutex_unlock)
#ifdef _POSIX_THREADS_PRIO_PROTECT
                                        ! load protocol type of mutex
	ld      [%o0+mutex_protocol],%o1
	cmp     %o1, PTHREAD_PRIO_PROTECT	! test if PTHREAD_PRIO_PROTECT
	be      slow_unlock             ! yes
	nop                             ! Delay
#endif /* _POSIX_THREADS_PRIO_PROTECT */
	ld	[%o0+mutex_queue],%o1
	tst	%o1			! mutex->queue.head == NULL ?
	bne	mutex_unlocked
	st	%g0,[%o0+mutex_owner]	! Delay: mutex->owner = NO_PTHREAD;
	st	%g0,[%o0+mutex_lock]	! mutex->lock = FALSE;
	ld	[%o0+mutex_queue],%o1
	tst	%o1			! mutex->queue.head == NULL ?
	bne	queue_unlocked
	nop
ret_unlocked:
	retl
	clr	%o0			! return(0);
queue_unlocked:
	ldstub	[%o0+mutex_lock],%o1	! test_and_set(&mutex->lock)
	tst	%o1			! prev. lock == 0 ?
	bne	ret_unlocked
	nop
#ifdef _POSIX_THREADS_PRIO_PROTECT
slow_unlock:
#endif /* _POSIX_THREADS_PRIO_PROTECT */
mutex_unlocked:
	save	%sp,-SA(MINFRAME),%sp	! Get a new window
	call	NAME(slow_mutex_unlock)	! return(slow_mutex_unlock(mutex));
	mov	%i0, %o0		! Pass parameter
	mov	%o0, %i0		! Return value
	ret
	restore
#endif /* NOERR_CHECK */

#ifndef CLEANUP_HEAP
!	int pthread_cleanup_push(func, arg)
!	void (*func)();
!	any_t arg;
!
!	Get space on stack for cleanup structure (new) and then call
!	pthread_cleanup_push_body with this "new"
	ENTRY(pthread_cleanup_push)
					! Get space for cleanup 	
#ifndef C_INTERFACE
	add	%sp,-SA(cleanup_size+SA(MINFRAME)-WINDOWSIZE),%sp
#else /* !C_INTERFACE */
	add	%sp,-SA(cleanup_size),%sp
#endif /* !C_INTERFACE */
	b	NAME(pthread_cleanup_push_body)
	add	%sp,SA(MINFRAME),%o2	! Delay: param 3 "new"
! 	Should never reach here

!	int pthread_cleanup_pop(execute)
!	int execute;
!
!	Remove space from stack for cleanup structure (new) and 
!	then call pthread_cleanup_pop_body.
	ENTRY(pthread_cleanup_pop)
	save	%sp,-SA(MINFRAME),%sp	! Get a new frame
	call	NAME(pthread_cleanup_pop_body)
	mov	%i0, %o0		! Delay: param 1
					! Remove space for cleanup 	
	restore				! Restore here, otherwise %sp
					!   increment does not work.
	retl				! Leaf return - already restored.
#ifndef C_INTERFACE
					! Delay: restore prev. stack pointer
	add	%sp,SA(cleanup_size+SA(MINFRAME)-WINDOWSIZE),%sp
#else /* !C_INTERFACE */
	add	%sp,SA(cleanup_size),%sp
#endif /* !C_INTERFACE */
#endif /* !CLEANUP_HEAP */

#if defined(_M_UNIX)
#include "sco/cerror.S"
#include "sco/sigaction.S"
#include "sco/sigprocmask.S"
#include "sco/sigsetjmp.S"
#include "sco/sigsuspend.S"
#include "sco/io.S"
#include "sco/wait.S"
#endif

#if defined(__FreeBSD__)
#include "freebsd/cerror.S"
#include "freebsd/sigaction.S"
#include "freebsd/sigprocmask.S"
#include "freebsd/sigsetjmp.S"
#include "freebsd/sigsuspend.S"
#include "freebsd/io.S"
#endif

#if defined(__dos__)
#include "dos/sigaction.S"
#include "dos/setjmp.S"
#endif

#if defined(__linux__)
#include "linux/cerror.S"
#include "linux/sigaction.S"
#include "linux/sigprocmask.S"
#include "linux/sigsetjmp.S"
#include "linux/sigsuspend.S"
#include "linux/io.S"
#include "linux/socketcall.S"
#endif
