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

  @(#)disp.c	3.14 11/8/00

*/

#include "internals.h"
#include "setjmp.h"

static int old_stack_ptr;

#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined(__dos__)
static SYS_SIGJMP_BUF stack_env;
#else
static sigjmp_buf stack_env;
#endif

static int status;

extern pthread_cond_t *new_cond[NNSIG];     /* cond for user handlers         */
extern struct context_t *new_scp[NNSIG];    /* info for user handlers         */

/*
 * SWITCH_TO_STACK - macro to switch stacks
 * CAUTION: Variables used across calls to this macro must be declared as static
 */
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined(__dos__)
#define SWITCH_TO_STACK(new_sp, val) \
  { \
    static int ret; \
    if (!(ret = SYS_SIGSETJMP(stack_env, FALSE, TRUE))) { \
      ret = new_sp; \
      old_stack_ptr = stack_env[JB_SP]; \
      stack_env[JB_SP] = ret; \
      SYS_SIGLONGJMP(stack_env, (int) val, TRUE); \
    } \
    val = (pthread_t) ret; \
  }
#else
#define SWITCH_TO_STACK(new_sp, val) \
  { \
    static int ret; \
    if (!(ret = sigsetjmp(stack_env, FALSE))) { \
      ret = new_sp; \
      old_stack_ptr = stack_env[JB_SP]; \
      stack_env[JB_SP] = ret; \
      siglongjmp(stack_env, (int) val); \
    } \
    val = (pthread_t) ret; \
  }
#endif
    
#ifdef C_CONTEXT_SWITCH
/*
 * Context switch (preemptive), now coded in C instead of assembly
 *
 * Do NOT compile with -O3 or more. This file contains several signal
 * handling routines which modify global data. Thus, the last optimization
 * which is safe is -O2!
 *
 * Notice that functions within ifdef _ASM are still not to be
 * compiled. The code of these functions simply serves as pseudo-
 * code to better understand the assembly code.
 *
 * Portability notes:
 * System calls to BSD routines have to be changed to SVR4.
 * Some system calls (e.g. sigprocmask) are redefined by Pthreads.
 * Thus, uses of sigsetmask cannot be replaced by sigprocmask
 * but rather have to be translated into direct syscall()'s.
 */

/*
 * temporary stack for dispatcher, pthread_handle_many_process_signals,
 * N nested function calls by pthread_handle_many_process_signals,
 * a possible sigtramp and universal handler, plus 1 window spare
 */
KERNEL_STACK pthread_tempstack;

/*------------------------------------------------------------*/
/* 
 * pthread_sched - dispatcher
 * assumes SET_KERNEL_FLAG
 */
void pthread_sched()
{
  static volatile pthread_t old, new;
  pthread_t pthread_sched_new_signals();
  
  old = mac_pthread_self();
#if defined(DEF_RR)
  if (old->state & T_ASYNCTIMER) {
    /*
     * was: pthread_cancel_timed_sigwait(old, FALSE, RR_TIME, TRUE);
     * optimized for speed; timer removed from queue by no syscalls
     * to setitimer since we call timed_sigwait before switching contexts
     */
    timer_ent_t tmr;
#ifdef STAND_ALONE
    extern pthread_timer_q_t pthread_timer;
#else
    extern pthread_timer_q pthread_timer;
#endif
    for (tmr = pthread_timer.head; tmr; tmr = tmr->next[TIMER_QUEUE])
      if (tmr->thread == old && (tmr->mode & DEF_RR))
	break;
    if (tmr) {
      old->state &= ~T_ASYNCTIMER;
      pthread_q_timer_deq(&pthread_timer, tmr);
    }
  }
#endif

  do {
    if (old && old->state & T_RETURNED) {
      SWITCH_TO_STACK(SA((int) pthread_tempstack_top) - SA(WINDOWSIZE), old);
      if (old->state & T_DETACHED) {
#ifdef MALLOC
	pthread_free(old->stack_base);
	pthread_free(old);
#else /* !MALLOC */
	free(old->stack_base);
	free(old);
#endif /* MALLOC */
      }
      mac_pthread_self() = old = NO_PTHREAD;
    }
    
    if ((new = ready.head) == NO_PTHREAD ||
        pthread_signonemptyset(&new_signals)) {
      mac_pthread_self() = new;
      new = pthread_sched_new_signals(old, FALSE);
    }
  } while ((new = ready.head) == NO_PTHREAD ||
           pthread_signonemptyset(&new_signals));

  mac_pthread_self() = new;
#ifdef DEF_RR
  if (new->attr.sched == SCHED_RR && !(new->state & T_ASYNCTIMER))
    pthread_timed_sigwait(new, (struct timespec *) NULL, RR_TIME, NULL, new);
#endif /* DEF_RR */
  
  if (!pthread_not_called_from_sighandler(new->context[JB_PC]))
    SIGPROCMASK(SIG_BLOCK, &all_signals, (struct sigset_t *) NULL);
  
  mac_pthread_self() = new; /* paranoia ??? */
  RESTORE_CONTEXT(new);
}

/*------------------------------------------------------------*/
/*
 * pthread_sched_new_signals - handle signals which came in while inside
 * the kernel by switching over to the temporary stack
 */
pthread_t pthread_sched_new_signals(p, masked)
     pthread_t p;
     int masked;
{
  static pthread_t old, new;
  extern pthread_t pthread_handle_many_process_signals();

  old = p;

  if (masked && old && !pthread_not_called_from_sighandler(old->context[JB_PC]))
    SIGPROCMASK(SIG_UNBLOCK, &all_signals, (struct sigset_t *) NULL);
    
  /*
   * always flush windows before the stack is changed to preserve the proper
   * linking of frame pointers
   */
  if (old)
    SWITCH_TO_STACK(SA((int) pthread_tempstack_top) - SA(WINDOWSIZE), old);
  new = pthread_handle_many_process_signals();
  if (old)
    SWITCH_TO_STACK(old_stack_ptr, new);

  return(new);
}

/*------------------------------------------------------------*/
/*
 * pthread_sched_wrapper -
 * assumes the following actions before/after call:
 * PRE:  save errno(p);
 * POST: restore errno(p);
 */
#ifdef C_CONTEXT_SWITCH
void pthread_sched_wrapper(sig, code, p)
int sig, code;
pthread_t p;
#else /* !C_CONTEXT_SWITCH */
void pthread_sched_wrapper(sig, code)
int sig, code;
#endif /* !C_CONTEXT_SWITCH */
{
  void pthread_signal_sched();

#ifdef C_CONTEXT_SWITCH
  if (!SAVE_CONTEXT(p))
#endif /* C_CONTEXT_SWITCH */
    pthread_signal_sched(sig, code);
}

/*------------------------------------------------------------*/
/*
 * pthread_not_called_from_sighandler -
 * This routine must textually follow pthread_sched_wrapper!!!
 */
int pthread_not_called_from_sighandler(addr)
int addr;
{
  return(addr < (int) pthread_sched_wrapper ||
         addr >= (int) pthread_not_called_from_sighandler);
}

/*
 * NOTICE: THE FOLLOWING C CODE IS ONLY TO ANNOTATE THE CORRESPONDING
 * ASSEMBLY CODE. THE C CODE DOES NOT COMPILE AND NEVER WILL!
 */
#ifdef _ASM
/*------------------------------------------------------------*/
/*
 * pthread_test_and_set -
 */
int pthread_test_and_set(flag)
int *flag;
{
  return(ldstub(flag));
}

/*------------------------------------------------------------*/
/*
 * pthread_get_sp -
 */
char *pthread_get_sp()
{
  return(sp);
}

/*------------------------------------------------------------*/
/*
 * pthread_set_sp -
 */
void pthread_set_sp(new_sp)
char *new_sp;
{
  sp = new_sp;
}

/*------------------------------------------------------------*/
/*
 * pthread_get_fp -
 */
char *pthread_get_fp()
{
  return(fp);
}

/*------------------------------------------------------------*/
/*
 * pthread_set_fp -
 */
void pthread_set_fp(new_fp)
char *new_fp;
{
  fp = new_fp;
}

/*------------------------------------------------------------*/
/*
 * pthread_ST_FLUSH_WINDOWS -
 */
void pthread_ST_FLUSH_WINDOWS()
{
  ST_FLUSH_WINDOWS();
}
#endif /* _ASM */

/*------------------------------------------------------------*/
/*
 * pthread_fake_call_wrapper_wrapper -
 */
void pthread_fake_call_wrapper_wrapper()
{
  pthread_t p = mac_pthread_self();
  int sig = p->sig;
  sigset_t smask;
  struct context_t scp;
  int new_context = p->nscp == (struct context_t *) DIRECTED_AT_THREAD;
  void pthread_fake_call_wrapper();
  extern struct sigaction pthread_user_handler[];

#if defined(_M_UNIX)
  set_errno(p->terrno);
#endif
  if (new_context) {
#if defined(SCO5)
    getcontext(&scp);
#endif
    pthread_sigcpyset2set(&scp.sc_mask, &p->mask);
    scp.sc_sp = p->osp;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined(__dos__)
    scp.sc_fp = p->obp;
#endif
    scp.sc_pc = p->opc;
  }
  pthread_sigcpyset2set(&smask, &p->mask);
  if (sig != -1) {
    pthread_sigcpyset2set(&p->mask, &pthread_user_handler[sig].sa_mask);
    sigaddset(&p->mask, sig);
  }
  pthread_fake_call_wrapper(sig == (int) PTHREAD_CANCELED ?
                              (void (*)()) pthread_exit:
                              pthread_user_handler[sig].sa_handler,
                            &smask,
                            sig,
                            &p->sig_info[sig==(int) PTHREAD_CANCELED ? 0 : sig],
                            new_context ? &scp : p->nscp,
                            new_context,
			    sig == (int) PTHREAD_CANCELED ? 0 : new_scp[sig],
			    (sig != (int) PTHREAD_CANCELED &&
			     new_cond[sig] != NULL) ?
			    new_cond[sig] : p->cond);
  new_scp[sig] = NULL;
  new_cond[sig] = NULL;
}

/*------------------------------------------------------------*/
/*
 * pthread_clear_kernel_flag_wrapper - after a fake call with modified pc in the
 * context structure, return though this wrapper which clears the kernel
 * flag before jumping into user code.
 */
void pthread_clear_kernel_flag_wrapper()
{
  register pthread_t p = mac_pthread_self();
  register int osp = p->osp;
  register int opc = p->opc;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined(__dos__)
  register int obp = p->obp;
#endif

  CLEAR_KERNEL_FLAG;

  p->context[JB_SP] = osp;
  p->context[JB_PC] = opc;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  p->context[JB_BP] = obp;
#endif
  RESTORE_CONTEXT(p);
}

/*------------------------------------------------------------*/
/*
 * pthread_fake_call_wrapper - invoke a fake call on a thread's stack
 * fake_call already puts the address of a user-defined handler
 * in %i0, the signal mask (to be restored) in %i1, and a flag
 * restore_context in %i5 which indicates if the context has to be
 * copied back by the wrapper (o.w. it is done by UNIX).
 * It calls the user handler with parameters sig, infop, scp.
 * Notice that the address of the condition variable is
 * passed on stack if the signal came in during a conditional wait.
 * In this case, the conditional wait terminates and the mutex is relocked
 * before the user handler is called. This is only done once for
 * nested handlers by the innermost handler (see check for zero-value
 * of the condition variable).
 * Notice that oscp is passed on stack and is restored as p->nscp
 * upon return from the wrapper.
 * The errno is saved across the user handler call.
 * assumes SET_KERNEL_FLAG still set from context switch after pushing fake call
 */
void pthread_fake_call_wrapper(user_handler, smask, sig, infop, scp,
                               restore_context, oscp, cond)
     void (*user_handler)();
     sigset_t *smask;
     int sig;
     struct siginfo *infop;
     struct context_t *scp;
     int restore_context;
     struct context_t *oscp;
     pthread_cond_t *cond;
{
  register pthread_t p = mac_pthread_self();
  sigset_t omask;
  register int saved_errno = errno;
  register struct frame *framep;
  register int old_pc = scp->sc_pc;
  register int old_sp = scp->sc_sp;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  register int old_bp = scp->sc_fp;
#endif
  register int new_sp = p->osp;
  register int new_pc = p->opc;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  register int new_bp = p->obp;
#endif

  void pthread_handle_pending_signals_wrapper();

  pthread_sigcpyset2set(&omask, &scp->sc_mask);

  if (cond)
    pthread_cond_wait_terminate(cond);

  CLEAR_KERNEL_FLAG;
  (*user_handler)(sig, infop, scp);
  SET_KERNEL_FLAG;

  errno = saved_errno;

  if (restore_context) {
    if (old_pc != scp->sc_pc) {
      p->context[JB_SP] = old_sp;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
      p->context[JB_BP] = old_bp;
#endif
      p->context[JB_PC] = (int) pthread_clear_kernel_flag_wrapper;
      new_sp = scp->sc_sp;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
      new_bp = scp->sc_fp;
#endif
      new_pc = scp->sc_pc;
#ifdef ASM_SETJMP
      new_pc -= RETURN_OFFSET;
#endif /* ASM_SETMP */
    }
    else {
      p->context[JB_SP] = scp->sc_sp;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
      p->context[JB_BP] = scp->sc_fp;
#endif
      p->context[JB_PC] = scp->sc_pc;
    }
#ifdef ASM_SETJMP
    p->context[JB_PC] -= RETURN_OFFSET;
#endif /* ASM_SETMP */
    pthread_sigcpyset2set(smask, &scp->sc_mask);
  }
  else {
    pthread_sigcpyset2set(smask, &scp->sc_mask);
    pthread_sigcpyset2set(&scp->sc_mask, &omask);
  }

  pthread_sigcpyset2set(&p->mask, smask);

  pthread_sigcpyset2set(smask, &p->pending);
  pthread_sigaddset2set(smask, &pending_signals);
  pthread_sigdelset2set(smask, &p->mask);

  p->terrno = errno;

  if (restore_context) {
    old_sp = p->context[JB_SP];
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
    old_bp = p->context[JB_BP];
#endif
    old_pc = p->context[JB_PC];
  }

  if (pthread_signonemptyset(smask))
    if (!SAVE_CONTEXT(p))
      pthread_handle_pending_signals_wrapper();
      /* never returns from call */
    else if (restore_context) {
      p->context[JB_SP] = old_sp;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
      p->context[JB_BP] = old_bp;
#endif
      p->context[JB_PC] = old_pc;
    }

  p->nscp = oscp;

  if (restore_context) {
    p->osp = new_sp;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
    p->obp = new_bp;
#endif
    p->opc = new_pc;
  }
  else {
    p->context[JB_SP] = new_sp;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
    p->context[JB_BP] = new_bp;
#endif
    p->context[JB_PC] = new_pc;
#ifdef ASM_SETJMP
    p->context[JB_PC] -= RETURN_OFFSET;
#endif /* ASM_SETJMP */
  }

  pthread_sched();
  /* never returns from call */
}
          
/*------------------------------------------------------------*/
/*
 * pthread_handle_pending_signals_wrapper - 
 * change to temp stack and call pthread_handle_pending_signals()
 * then jumps into regular scheduler
 * assumes SET_KERNEL_FLAG
 */
void pthread_handle_pending_signals_wrapper()
{
  static pthread_t old;

  old = mac_pthread_self();

  /*
   * always flush windows before the stack is changed to preserve the proper
   * linking of frame pointers
   */
  SWITCH_TO_STACK(SA((int) pthread_tempstack_top) - SA(WINDOWSIZE), old);
  pthread_handle_pending_signals();
  SWITCH_TO_STACK(old_stack_ptr, old);

  pthread_sched();
  /* never returns from call */
}

/*------------------------------------------------------------*/
/*
 * pthread_signal_sched - 
 * change to temp stack and call pthread_handle_one_process_signal(sig)
 * then jumps into regular scheduler
 * This is called by the universal signal handler to minimize calls
 * to set the process mask which is an expensive UNIX system call.
 * assumes SET_KERNEL_FLAG
 */
void pthread_signal_sched(sig, code)
     int sig, code;
{
  static pthread_t old;

  old = mac_pthread_self();
  old->sig = sig;
  old->code = code;

  /*
   * always flush windows before the stack is changed to preserve the proper
   * linking of frame pointers
   */
  SWITCH_TO_STACK(SA((int) pthread_tempstack_top) - SA(WINDOWSIZE), old);
  pthread_handle_one_process_signal(old->sig, old->code);
  SWITCH_TO_STACK(old_stack_ptr, old);
  
  pthread_sched();
  /* never returns from call */
}
  
#ifdef _ASM
/*------------------------------------------------------------*/
/*
 * setjmp - 
 */
int setjmp(env)
     jmp_buf env;
{
  return(sigsetjmp(env, TRUE));
}

/*------------------------------------------------------------*/
/*
 * longjmp - 
 */
void longjmp(env, val)
     jmp_buf env;
     int val;
{
  siglongjmp(env, val);
}

/*------------------------------------------------------------*/
/*
 * sigsetjmp - 
 */
int sigsetjmp(env, savemask)
     sigjmp_buf env;
     int savemask;
{
  env[JB_SP] = sp;
  env[JB_PC] = pc;
  env[JB_SVMASK] = savemask;
  if (env[JB_SVMASK])
    pthread_sigcpyset2set(&env[JB_MASK], &mac_pthread_self()->mask);
  return(0);
}

/*------------------------------------------------------------*/
/*
 * siglongjmp - 
 */
void siglongjmp(env, val)
     sigjmp_buf env;
     int val;
{
  int new_sp;

  if (env[JB_SVMASK])
    sigprocmask(SIG_SETMASK, &env[SV_MASK], (struct sigaction *) NULL);
  ST_FLUSH_WINDOWS();
  fp = env[JB_SP];
  pc = env[JB_PC];
  if (val == 0)
    val = 1;
  ret;
}

#ifdef NOERR_CHECK
/*------------------------------------------------------------*/
/*
 * pthread_mutex_lock - 
 */
int pthread_mutex_lock(mutex)
     pthread_mutex_t *mutex;
{
#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (mutex->protocol == PTHREAD_PRIO_PROTECT)
    goto slow_lock;
#endif
  if (test_and_set(&mutex->lock)) {
    mutex->owner = mac_pthread_self();
    return(0);
  }
  /*
   * if the queue is not empty or if someone holds the mutex,
   * we need to enter the kernel to queue up.
   */
#ifdef _POSIX_THREADS_PRIO_PROTECT
slow_lock:
#endif
  return(slow_mutex_lock(mutex));
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_trylock - 
 */
int pthread_mutex_trylock(mutex)
     pthread_mutex_t *mutex;
{
#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (mutex->protocol == PTHREAD_PRIO_PROTECT)
    return slow_mutex_trylock(mutex);
#endif
  if (test_and_set(&mutex->lock)) {
    mutex->owner = mac_pthread_self();
    return(EBUSY);
  }
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_unlock - 
 */
int pthread_mutex_unlock(mutex)
     pthread_mutex_t *mutex;
{

#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (mutex->protocol == PTHREAD_PRIO_PROTECT)
    goto slow_unlock;
#endif

  mutex->owner = NO_PTHREAD;
  if (mutex->queue.head == NULL) {
    mutex->lock = FALSE;
    /*
     * We have to test the queue again since there is a window
     * between the previous test and the unlocking of the mutex
     * where someone could have queued up.
     */
    if (mutex->queue.head == NULL)
      return(0);
    if (test_and_set(&mutex->lock))
      /*
       * if the test & set is not successful, someone else must
       * have acquired the mutex and will handle proper queueing,
       * so we're done.
       */
      return(0);
  }
  /*
   * if the queue is not empty, we need to enter the kernel to unqueue.
   */
#ifdef _POSIX_THREADS_PRIO_PROTECT
slow_unlock:
#endif
  return(slow_mutex_unlock(mutex));
}
#endif /* NOERR_CHECK */

#ifndef CLEANUP_HEAP
/*------------------------------------------------------------*/
/*
 * pthread_cleanup_push - 
 */
int pthread_cleanup_push(func, arg)
     pthread_func_t func;
     any_t arg;
{
  cleanup_t *new;

#ifndef C_INTERFACE
  sp -= SA(sizeof(*new)+SA(MINFRAME)-WINDOWSIZE);
#else
  sp -= SA(sizeof(*new));
#endif
  new = sp + SA(MINFRAME);
  pthread_cleanup_push_body(func, arg, new);
}

/*------------------------------------------------------------*/
/*
 * pthread_cleanup_pop - 
 */
int pthread_cleanup_pop(execute)
     int execute;
{
  pthread_cleanup_pop_body(execute);
#ifndef C_INTERFACE
  sp += SA(sizeof(*new)+SA(MINFRAME)-WINDOWSIZE);
#else
  sp += SA(sizeof(*new));
#endif
}
#endif /* !CLEANUP_HEAP */

#ifndef SOLARIS
/*------------------------------------------------------------*/
/*
 * start_float -
 */
void start_float()
{
  pthread_init();
}
#endif /* !SOLARIS */
#endif /* _ASM */
#endif /* C_CONTEXT_SWITCH */

/*------------------------------------------------------------*/
/*
 * process_exit - switches stacks to process stack and
 * calls UNIX exit with parameter status
 */
static void process_exit()
{
  CLEAR_KERNEL_FLAG;
  exit((int) status);
}

/*------------------------------------------------------------*/
/*
 * pthread_process_exit - switches stacks to process stack and
 * calls UNIX exit with parameter status
 */
void pthread_process_exit(p_status)
     int p_status;
{
  status = p_status;

  /*
   * always flush windows before the stack is changed to preserve the proper
   * linking of frame pointers
   */
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  SYS_SIGSETJMP(stack_env, FALSE, TRUE);
#else
  sigsetjmp(stack_env, FALSE);
#endif
  stack_env[JB_SP] = (int) process_stack_base;
  stack_env[JB_PC] = (int) process_exit;
#if defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH)
  stack_env[JB_PC] -= RETURN_OFFSET;
#endif /* defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH) */
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  SYS_SIGLONGJMP(stack_env, p_status, TRUE);
#else
  siglongjmp(stack_env, p_status);
#endif
}
