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

  @(#)sched.S	3.14 11/8/00

*/

#include <pthread/config.h>
#include <../src/config_internals.h>

#ifndef C_CONTEXT_SWITCH
/*
 * Context switch supporting assembly routines and other assembly routines
 * Portability notes:
 * The assembly code in this file is only needed for the following conditional
 * compilation:
 * !C_CONTEXT_SWITCH: for all routines (slow version of context switch etc.
 *                    if C_CONTEXT_SWITCH defined).
 */

#define LOCORE
#define _ASM

#ifdef SOLARIS
#include <sys/asm_linkage.h>
#define NAME(x) x
#else /* !SOLARIS */
#include <sparc/asm_linkage.h>
#ifdef NAME	
#undef NAME
#endif /* NAME */
#define	IDENTITY(x) x
#define NAME(x) IDENTITY(_)x
#endif /* !SOLARIS */

#include <sys/errno.h>

#ifdef STAND_ALONE
#define DISABLE_INTERRUPTS	ta 0x08
#define ENABLE_INTERRUPTS	call NAME(enable_traps)
#ifdef sun4e
#include <sun4e/trap.h>
#elif defined(sun4c)
#include <sun4c/trap.h>
#elif defined(sun4m)
#include <sun4m/trap.h>
#endif
#else /* !STAND_ALONE */
#define DISABLE_INTERRUPTS	call NAME(pthread_p_sigprocmask)
#define ENABLE_INTERRUPTS	call NAME(pthread_p_sigprocmask)
#ifdef SOLARIS
#include <sys/trap.h>
#define GLOBAL
#else /* !SORLARIS */
#include <sparc/trap.h>
#endif /* !SOLARIS */
#endif /* !STAND_ALONE */

#include <pthread/unistd.h>
#include "offsets.h"
#include <pthread/asm.h>
#include "signal_internals.h"

#define SCHED_WINDOW MINFRAME

	.seg	"text"

!	void pthread_sched
!
        ENTRY(pthread_sched)
	set	NAME(pthread_kern),%g1	! Get kernel address
#ifdef NO_INLINE
	st	%g0,[%g1+is_in_kernel]	! Clear kernel flag
	ld	[%g1+state_change],%g2	! Get kernel state
	tst	%g2			! check if state changed
	bne	state_changed
	nop
	retl
	nop
state_changed:
#endif
	st	%i7,[%g1+is_in_kernel]	! Enter kernel again
	save	%sp,-SA(SCHED_WINDOW),%sp	! Get a new window
pthread_sched_no_save:
	ld	[%g1+pthread_self],%l0	! Get current thread
	sethi	%hi(NAME(errno)),%l5		! Get high addr of process errno
	ld	[%g1+ready_head],%i0	! Load new thread

	ld	[%l0+state],%l2		! Get the current context state

	mov	%g1,%l3			! Do not lose address of kernel
	call	NAME(pthread_signonemptyset)!	pthread_signonemptyset(
	add	%g1, new_signals, %o0	! Delay: &new_signals);
	mov	%l3,%g1			! Do not lose address of kernel
	
	btst	T_RETURNED,%l2		! Returned?
	be	test_old		! No, save old state and restore new
	mov	%o0, %l4		! Delay: pending signals

	ta	ST_FLUSH_WINDOWS

	btst	T_DETACHED,%l2		! Both returned and detached?
	be	test_new		! No, restore the new state
	tst	%l4			! Delay: Test if signals pending
	mov	%g1,%l3			! Delay: Do not lose address of kernel

! Free the stack of the current context.
! But before, switch to a temporary stack in case the OS really takes away
! our thread stack (which it does when sbrk() is called with a negative
! argument). The temporary stack must be large enough to accommodate the
! calling chain of the free() routines itself.

dealloc:
	sethi	%hi(NAME(pthread_tempstack)-SA(WINDOWSIZE)),%l1
	add	%l1,%lo(NAME(pthread_tempstack)-SA(WINDOWSIZE)),%sp
#if defined(MALLOC) || defined(STAND_ALONE)
	call	NAME(pthread_free)
#else /* !(MALLOC || STAND_ALONE) */
	call	NAME(free)
#endif /* MALLOC || STAND_ALONE */
	ld	[%l0+stack_base],%o0	! Delay slot - Free the stack

! Free the thread structure for the current thread.  After this, we must be
! careful not to store a context into it.
#ifndef STAND_ALONE
#ifdef MALLOC
	call	NAME(pthread_free)
#else /* !MALLOC */
	call	NAME(free)
#endif /* MALLOC */
	mov	%l0,%o0			! Delay slot
#endif /* !STAND_ALONE */

	mov	%l3, %g1		! Do not lose address of kernel

	st	%g0, [%g1+pthread_self]	! mac_pthread_self() =
	clr	%l0			!   old = NO_PTHREAD;

	b	test_new		! Skip save
	tst	%l4			! Delay: Test if signals pending
test_old:
	cmp	%l0,%i0
	bne	save_old		! Jump if new thread on head of ready
	tst	%l4			! Delay: Test if signals pending
	be	no_switch		! Jump if no pending signals
					! Delay: Load high interrupt ret addr
        sethi	%hi(NAME(called_from_sighandler)),%g2
save_old:
	ta	ST_FLUSH_WINDOWS

	ld	[%l5+%lo(NAME(errno))],%l6	! Get errno
	st	%l6,[%l0+thread_errno]	! Save errno of current thread
	std	%fp,[%l0+sp_offset]	! Save the current stack pointer
!impl.	st	%i7,[%l0+pc_offset]	! Save the current return pc
test_new:
					! Jump if signals pending
	bne	handle_pending_signals
	tst	%i0
	be	no_threads		! Jump if no thread on ready queue
#ifdef EMPTY
	.empty				! Label Ok in delay slot
#endif
restore_new:
					! (Delay:) Load high interrupt ret addr
        sethi	%hi(NAME(called_from_sighandler)),%g2
	ld	[%i0+thread_errno],%l6	! Errno of new thread
	st	%l6,[%l5+%lo(NAME(errno))]	! Save errno
        ldd     [%i0+sp_offset],%fp     ! Load the new stack pointer
!impl.	ld	[%i0+pc_offset],%i7	! Load the new pc
no_switch:
#ifdef DEF_RR
	ld	[%i0+sched],%l0		! Load schedular attribute
 	cmp	%l0,SCHED_RR		! Is it RR (round-robin) ?
 	bne	no_rr			! Do not set timer for RR
					! Delay: Load low interrupt ret address
        or	%g2,%lo(NAME(called_from_sighandler)),%g2
	mov	%g1,%l3			! Do not lose the address of kernel
 	mov	%i0,%o0			! Move parameter - thread_t
 	call	NAME(pthread_timed_sigwait)	! Set the timer for RR
 	mov	RR_TIME,%o2		! Move parameter - Mode
	mov	%l3,%g1			! Do not lose the address of kernel
                                        ! Load high interrupt return addr
        sethi	%hi(NAME(called_from_sighandler)),%g2
#endif
					! Load low interrupt return address
        or	%g2,%lo(NAME(called_from_sighandler)),%g2
#ifdef DEF_RR
no_rr:
#endif
	cmp	%g2,%i7			! Compare pc-map with new pc
	bne	skip_sig1		! Jump if mask not interrupt return
	st	%i0,[%g1+pthread_self]	! Delay: Store new thread
	mov	%g1, %l3		! Do not lose address of kernel
	mov	SIG_BLOCK, %o0		! param 0: block
	add	%g1, all_signals, %o1	! param 1: all signals
	DISABLE_INTERRUPTS		! Disable signals
	clr	%o2			! Delay: param 2
	mov	%l3, %g1		! Do not lose address of kernel


! Return into the new context.  Since all windows have been flushed, the
! RESTORE will cause a window underflow trap, restoring the registers of the
! new context from its stack.
skip_sig1:
	restore
	st	%g0,[%g1+state_change]	! Clear state_change flag
	st	%g0,[%g1+is_in_kernel]	! Clear kernel flag

	save	%sp,-SA(SCHED_WINDOW),%sp	! Get a new window
	call	NAME(pthread_signonemptyset)!	pthread_signonemptyset(
	add	%g1, new_signals, %o0	! Delay: &new_signals);

	tst	%o0			! Any signals received while in kernel?
	bne	sig_pending
	restore

	retl				! Activate new thread
	nop
sig_pending:
	set	NAME(pthread_kern), %g1
	st	%i7,[%g1+is_in_kernel]	! Enter kernel again
	save	%sp,-SA(SCHED_WINDOW),%sp	! Get a new window

	ld	[%g1+pthread_self],%l0	! Get current thread

	sethi	%hi(NAME(errno)),%l5		! Get (high) address of process errno
	ld	[%l5+%lo(NAME(errno))],%l6	! Get errno
	st	%l6,[%l0+thread_errno]	! Save errno of current thread
	std	%fp,[%l0+sp_offset]	! Save the current stack pointer
!impl.	st	%i7,[%l0+pc_offset]	! Save the current return pc

                                        ! Load interrupt return address
        set     NAME(called_from_sighandler),%g2
	cmp	%g2,%i7			! Compare pc-map w/ new pc again
	bne	skip_sig2		! Jump if mask not interrupt return
	mov	%g1,%l3			! Delay: Do not lose address of kernel

	mov	SIG_UNBLOCK, %o0	! param 0: unblock
	add	%g1, all_signals, %o1	! param 1: all signals
	ENABLE_INTERRUPTS		! Enable signals
	clr	%o2			! Delay: param 2

skip_sig2:
	ta	ST_FLUSH_WINDOWS

	ba,a	kern_saved		! Ignore Delay: Jump always

handle_pending_signals:
no_threads:
	mov     %g1,%l3			! Do not lose address of kernel

kern_saved:
					! Get temp. stack address
	sethi	%hi(NAME(pthread_tempstack)-SA(WINDOWSIZE)),%l1
	call	NAME(pthread_handle_many_process_signals)
	add	%l1,%lo(NAME(pthread_tempstack)-SA(WINDOWSIZE)),%sp
	cmp	%g0,%l0			! test if deallocated
	bne	restore_stack		! No
	nop				! Delay:
	mov	%o0,%l0			! Copy return Val (ready.head) to %l0
restore_stack:
	ld	[%l0+sp_offset],%l2	! Switch back to stack of thread
	add	%l2,-SA(SCHED_WINDOW),%sp
	mov	%o0,%i0			! Move ret val from call into i0

	cmp	%l0,%i0			! Thread at the head of ready?
	be	restore_new		! Jump if so
	mov	%l3, %g1		! Do not lose address of kernel
	ld	[%l0+state],%l2		! Get the current context state
	btst	T_RETURNED,%l2		! Returned?
	be	restore_new		! No, restore the new state
	btst	T_DETACHED,%l2		! Delay: both returned and detached?
	be	restore_new		! No, restore the new state
	nop

	call	NAME(pthread_signonemptyset)!	pthread_signonemptyset(
	add	%g1, new_signals, %o0	! Dealy: &new_signals);
	
	b	dealloc			! Free structures
	mov	%o0, %l4		! Delay: pending signals

#ifndef STAND_ALONE
!	void pthread_sched_wrapper(sig, code)
!	int sig;
!	int code;
!
!	wrapper for pthread_sched() to determine the pc
!	called from the sighandler()
!	calls pthread_sched() and provides a global address for this call
!	which can be used by pthread_sched() to determine if it is about
!	to switch context to an interrupted thread (by comparing the
!	return value with the global address NAME(called_from_sighandler).
        .global  NAME(called_from_sighandler)
        .global  NAME(pthread_sched_wrapper)
NAME(pthread_sched_wrapper):
	save	%sp,-SA(MINFRAME),%sp	! Get a new window.

	mov	%i1, %o1		! Delay: Pass parameter 2
NAME(called_from_sighandler):		! Address of call instruction
	call	NAME(pthread_signal_sched) ! Call 2nd dispatcher (handle signal)
	mov	%i0, %o0		! Delay: Pass parameter 1
is_longjmp:
	ret
	restore
#endif /* !STAND_ALONE */

!       int pthread_not_called_from_sighandler(addr)
!       int addr;
!
!       returns FALSE iff the address passed in corresponds to the sighandler
        ENTRY(pthread_not_called_from_sighandler)
        set     NAME(called_from_sighandler),%o1
        retl
        sub     %o0,%o1,%o0             ! Delay: return difference of param
                                        ! and sighandler return address

!	void pthread_fake_call_wrapper(user_handler, smask, sig, infop, scp,
!			       restore_context, oscp, cond)
!	void (*user_handler)();
!	sigset_t *smask;
!	int sig;
!	struct siginfo *infop;
!	struct sigcontext *scp, *oscp;
!	int new_context;
!	pthread_cond_t *cond;
!
!       If the conditional variable pointer is non-null, the pending
!	conditional wait terminates and the mutex is relocked
!	before the user handler is called. This is only done once for
!	nested handlers by the innermost handler (see check for zero-value
!	of the condition variable).
!	Then the user handler is called with parameters sig, infop, scp.
!	The errno is saved across the user handler call.

!	Notice that the frame of this wrapper is already put the
!	stack by fake_call, therefore, we should NOT use "save" to
!	get a new window.
!	Notice that the address of the condition variable is
!       passed(!) in %l6 if the signal came in during a conditional wait.
!	Notice that oscp is passed(!) in %l7 and is restored as p->nscp
!	upon return from the wrapper.
	ENTRY(pthread_fake_call_wrapper)
	sethi	%hi(NAME(errno)), %l0! Get process errno (high)
	tst	%l6		! Cond. var. == 0 ?
	be	no_cond_wait
				! Delay: Get process errno (low)
	ld	[%l0+%lo(NAME(errno))], %l3
				! Terminate cond. wait
	call	NAME(pthread_cond_wait_terminate)
	mov	%l6, %o0
no_cond_wait:
				! Delay: omask = smask + sizeof(sigset_t);
	add	%i1, sigset_t_size, %l2

	mov	%l2, %o0	! pthread_sigcpyset2set(omask, &scp->mask);
	call	NAME(pthread_sigcpyset2set)
	add	%i4, sc_mask, %o1

	mov	%i2, %o0
	mov	%i3, %o1
	call	%i0		! user_signal_handler(sig, infop, scp);
	mov	%i4, %o2

				! Get base address of kernel structure
	set	NAME(pthread_kern), %l4
				! Enter kernel again
	st	%i7, [%l4+is_in_kernel]
	ld	[%l4+pthread_self], %i3
	tst	%i5		! Check if context needs to be restored
	be	UNIX_restore
				! Delay: Restore errno
	st	%l3, [%l0+%lo(NAME(errno))]
	mov	%i1, %o0	! pthread_sigcpyset2set(smask, &scp->sc_mask);
	call	NAME(pthread_sigcpyset2set)
	add	%i4, sc_mask, %o1

	ld	[%i4+sc_sp], %fp! Load stack pointer to be restored
	ld	[%i4+sc_pc], %i7! Load program counter to be restored
	ba	context_restored
				! Delay: Subtract offset added by ret instr
	add	%i7, -RETURN_OFFSET, %i7

UNIX_restore:
	mov	%i1, %o0	! pthread_sigcpyset2set(smask, &scp->sc_mask);
	call	NAME(pthread_sigcpyset2set)
	add	%i4, sc_mask, %o1
				! pthread_sigcpyset2set(&scp->sc_mask, omask);
	add	%i4, sc_mask, %o0
	call	NAME(pthread_sigcpyset2set)
	mov	%l2, %o1
	
context_restored:
	add	%i3, mask, %o0	! pthread_sigcpyset2set(&p->mask, smask);
	call	NAME(pthread_sigcpyset2set)
	mov	%i1, %o1

	mov	%i1, %o1	! pthread_sigcpyset2set(smask,&p->pending);
	call	NAME(pthread_sigcpyset2set)
	add	%i3, pending, %o1

	mov	%i1, %o1	! pthread_sigaddset2set(smask,&pending_signals);
	call	NAME(pthread_sigaddset2set)
	add	%l4, pending_signals, %o1

	mov	%i1, %o1	! pthread_sigdelset2set(smask,&p->mask);
	call	NAME(pthread_sigdelset2set)
	add	%i3, mask, %o1
				! if (pthread_signonemptyset(smask))
	call	NAME(pthread_signonemptyset)
	mov	%i1, %o1

	tst	%o0
        be      not_pending     ! Any pending signal cleared ?
	mov	%l4, %g1	! save kernel address
				! If branch, leave kernel but stick
				! with current frame

				! Handle signals pending on thread
	call	NAME(pthread_handle_pending_signals_wrapper)
	clr	%o0		! Delay: parameter no initial save
				! Should never return to this point
not_pending:                    ! Leave kernel but stick with current frame
        ba      pthread_sched_no_save
	st	%l7, [%i3+nscp]	! Delay: Copy prev sigcontext into current one	

!	void pthread_handle_pending_signals_wrapper(initial_save)
!	int initial_save;
!
!	change to temp stack and call handle_pending_signals()
!	then jumps into regular scheduler
!	creates own frame at beginning or after save thread context
!	depending on initial_save in %i0/%o0
!	assumes SET_KERNEL_FLAG

	ENTRY(pthread_handle_pending_signals_wrapper)
				! Get the kernel address
	set	NAME(pthread_kern),%g3
	tst	%o0
	be	no_initial_save
	ld	[%g3+pthread_self],%g4	! Delay: get current thread

	save	%sp,-SA(SCHED_WINDOW),%sp	! Get a new window
no_initial_save:
	sethi	%hi(NAME(errno)),%g1		! Save the thread context
	ld	[%g1+%lo(NAME(errno))],%g2	! Get errno
	st	%g2,[%g4+thread_errno]	! Save errno of current thread
	std	%fp,[%g4+sp_offset]	! Save the current stack pointer
!impl.	st	%i7,[%g4+pc_offset]	! Save the current return pc

	bne	initial_save
					! Delay: load temp. stack address
	sethi	%hi(NAME(pthread_tempstack)-SA(WINDOWSIZE)),%g2

	save	%sp,-SA(SCHED_WINDOW),%sp	! Get a new window
initial_save:
	ta	ST_FLUSH_WINDOWS

	mov	%g3, %l3		! Save kernel address
	mov	%g4, %l0		! Save pthread_self
	mov	%g1, %l5		! Save errno address

	call	NAME(pthread_handle_pending_signals)
					! Delay: switch to _tempstack
	add	%g2,%lo(NAME(pthread_tempstack)-SA(WINDOWSIZE)),%sp

	ba	restore_stack
	ld	[%l3+ready_head],%o0		! Delay: parameter ready.head



!	void pthread_signal_sched(sig, code)
!	int sig;
!	int code;
!
!	change to temp stack and call pthread_handle_one_process_signal(sig)
!	then jumps into regular scheduler
!	This is called by the universal signal handler to minimize calls
!	to set the process signal mask which is an expensive UNIX system call.
!	assumes SET_KERNEL_FLAG

	ENTRY(pthread_signal_sched)
	save	%sp,-SA(SCHED_WINDOW),%sp	! Get a new window

	ta	ST_FLUSH_WINDOWS

	set	NAME(pthread_kern),%l3	! Get kernel address
	ld	[%l3+pthread_self],%l0	! Get current thread

	sethi	%hi(NAME(errno)),%l5		! Save the thread context
	ld	[%l5+%lo(NAME(errno))],%g2	! Get errno
	st	%g2,[%l0+thread_errno]	! Save errno of current thread
	std	%fp,[%l0+sp_offset]	! Save the current stack pointer
!impl.	st	%i7,[%l0+pc_offset]	! Save the current return pc

					! Load temp. stack address
	sethi	%hi(NAME(pthread_tempstack)-SA(WINDOWSIZE)),%g2	
	mov	%i0, %o0		! Parameter 1: signal number
	mov	%i1, %o1		! Parameter 2: signal code
	call	NAME(pthread_handle_one_process_signal)
					! Delay: switch to _tempstack
	add	%g2,%lo(NAME(pthread_tempstack)-SA(WINDOWSIZE)),%sp

	ba	restore_stack
	ld	[%l3+ready_head],%o0	! Delay: parameter ready.head


	.seg	"bss"			! Could also use data segment
	.skip   50*SA(MINFRAME)
#ifdef STAND_ALONE
	.skip   100*SA(MINFRAME)	! Add up to 150 * MINFRAME
        .align  STACK_ALIGN
	.global	NAME(newstack)
NAME(newstack):
#endif /* STAND_ALONE */
!	Temporary stack space

	.skip	SA(MINFRAME)		! Spare window in case lower windows
					!   take up more space (e.g. locals)
	.skip	SA(MINFRAME)		! Universal handler window
	.skip	SA(MINFRAME)		! UNIX sigtramp window
	.skip	TEMPSTACK_SIZE		! Calls by handle_many_process_signals
!	.skip	SA(WINDOWSIZE)		! Handle_many_process_signals window
!	.skip	SA(WINDOWSIZE)		! Dispatcher window
	.align	STACK_ALIGN
#ifdef STACK_CHECK
	.global	NAME(pthread_tempstack)
#endif /* STACK_CHECK */
NAME(pthread_tempstack):		! Stack base for dispatcher
	.skip	SA(MINFRAME)		! Empty previous frame (callee save)

#ifdef STAND_ALONE
	.skip	SA(64)			! Kernel Stack Area for Globals
        .align  STACK_ALIGN
	.global	NAME(global_save_area)
NAME(global_save_area):
	.skip	32
	
	.global	NAME(heap_start)
NAME(heap_start):
	.skip	HEAP_SIZE
#endif /* STAND_ALONE */

	.seg	"text"
	.seg	"data"

#endif /* !C_CONTEXT_SWITCH */
