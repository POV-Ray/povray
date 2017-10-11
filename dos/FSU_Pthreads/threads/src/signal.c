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

  @(#)signal.c	3.14 11/8/00

*/

/* 
 * Functions for the handling of signals and timers.
 */

/*
 * The DEBUG flag causes a message to be printed out during signal handling.
 * The IO flag handles I/O requests asynchronously such that a signal is
 * delivered to the process upon completion of the operation.
 * If both flags are set at the same time, the signal handler would issue
 * an I/O request for each invocation which in turns causes another signal
 * to be delivered to yet another instance of the signal handler.
 * To avoid this, messages are only printed if DEBUG is defined but not IO.
 */

#define PTHREAD_KERNEL
#include "signal_internals.h"
#include "internals.h"
#include "setjmp.h"
#include "offsets.h"
#ifdef TDI_SUPPORT
#include "tdi-aux.h"
#endif

#if !defined (_M_UNIX) && !defined (__dos__)
#include <sys/syscall.h>
#endif

#if defined(__FreeBSD__)
#include "freebsd/signal.h"
#elif defined(__dos__)
#include <sys/exceptn.h>   /* for definition of __djgpp_exception_state */
#include <sys/movedata.h>  /* for definition of dosmemput/dosmemget */
#include <go32.h>
#include "dos/signal.h"
#elif defined(__linux__)
#include "linux/signal.h"
#elif defined (_M_UNIX)
#include "sco/signal.h"
#endif

#if defined(_M_UNIX)
#include <sys/socket.h>
#endif

#if defined (__FreeBSD__) || defined(__linux__)
#ifndef SYS_ACCEPT
#define SYS_ACCEPT      SOCKOP_accept
#define SYS_RECVFROM    SOCKOP_recvfrom
#define SYS_SENDTO      SOCKOP_sendto
#define SYS_CONNECT     SOCKOP_connect
#define SYS_GETPEERNAME SOCKOP_getpeername
#define SYS_GETSOCKOPT  SOCKOP_getsockopt
#endif
#endif

#if defined(SOLARIS) && defined(IO)
#include <dlfcn.h>
#endif /* SOLARIS && IO */

#if defined(SCO5)
#include <sys/resource.h>
#include <sys/wait.h>
#endif

#ifdef NOERR_CHECK
#undef NOERR_CHECK
#include "mutex.h"
#define NOERR_CHECK
#else /* !NOERR_CHECK */
#include "mutex.h"
#endif /* NOERR_CHECK */

#if defined(IO) && defined(USE_POLL)
#include <stropts.h>
#include <poll.h>
#endif

#ifndef FC_CODE
#define FC_CODE(c) (c & 0xff)
#endif

#ifndef  __FDS_BITS
#define __FDS_BITS(set) ((set)->fds_bits)
#endif

#ifdef STAND_ALONE
#ifdef sun4e
#include <sun4e/counter.h>
#elif defined(sun4c)
#include <sun4c/clock.h>
#elif defined(sun4m)
#include <sun4m/clock.h>
#define   limit14 cpu[0].timer_msw
#define counter14 cpu[0].timer_lsw
#endif /* SUN4x */
extern char heap_start;
#endif /* STAND_ALONE */

#ifdef RAND_SWITCH 
extern int pthread_n_ready;
#endif

#ifdef TIMER_DEBUG
static struct timeval last_alarm;
#endif

#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
extern int pthread_page_size;
extern KERNEL_STACK pthread_tempstack;
#endif

#ifdef STAND_ALONE
pthread_timer_q_t pthread_timer;              /* timer queue                 */
#else
pthread_timer_q pthread_timer;                /* timer queue                 */
static struct itimerval it;                   /* timer structure             */
#endif
#if defined(IO) && defined(_M_UNIX)
#if defined(USE_POLL)
int gtimeout = 0;
#else
struct timeval gtimeout = { 0, 0};
#endif
#endif
struct sigaction pthread_user_handler[NNSIG]; /* user signal handlers        */
volatile int new_code[NNSIG];                 /* UNIX signal code (new sigs) */
pthread_cond_t *new_cond[NNSIG];              /* cond for user handlers      */
struct context_t *new_scp[NNSIG];             /* info for user handlers      */
static int pending_code[NNSIG];               /* UNIX signal code (pending)  */
static sigset_t synchronous;                  /* set of synchronous signals  */
static sigset_t sig_handling;                 /* set of signals being handled*/

/*------------------------------------------------------------*/
/*
 * pthread_sigcpyset2set - add a signal set to another one
 */
void pthread_sigcpyset2set(dst, src)
int *dst, *src;
{
  int i;

  for (i = 0; i < PTHREAD_SIGSET2SET_SIZE_NP; i += sizeof(int))
    *(dst++) = *(src++);
}

/*------------------------------------------------------------*/
/*
 * pthread_sigaddset2set - add a signal set to another one
 */
void pthread_sigaddset2set(dst, src)
int *dst, *src;
{
  int i;

  for (i = 0; i < PTHREAD_SIGSET2SET_SIZE_NP; i += sizeof(int))
    *(dst++) |= *(src++);
}

/*------------------------------------------------------------*/
/*
 * pthread_sigdelset2set - delete a signal set from another one
 */
void pthread_sigdelset2set(dst, src)
int *dst, *src;
{
  int i;

  for (i = 0; i < PTHREAD_SIGSET2SET_SIZE_NP; i += sizeof(int))
    *(dst++) &= ~(*(src++));
}

/*------------------------------------------------------------*/
/*
 * pthread_sigismemberset2set - check if two sets overlap
 */
int pthread_sigismemberset2set(dst, src)
int *dst, *src;
{
  int i;

  for (i = 0; i < PTHREAD_SIGSET2SET_SIZE_NP; i += sizeof(int))
    if (*(dst++) & *(src++))
      return(TRUE);

  return(FALSE);
}

/*------------------------------------------------------------*/
/*
 * pthread_signonemptyset - check if a set has any bits set
 */
int pthread_signonemptyset(set)
int *set;
{
  int i;

  for (i = 0; i < PTHREAD_SIGSET2SET_SIZE_NP; i += sizeof(int))
    if (*(set++))
      return(TRUE);

  return(FALSE);
}

/*------------------------------------------------------------*/
/*
 * pthread_siggeset2set - check if dst set is >= src set, i.e.
 * dst has at least the bits set which src has set
 */
int pthread_siggeset2set(dst, src)
int *dst, *src;
{
  int i;

  for (i = 0; i < PTHREAD_SIGSET2SET_SIZE_NP; i += sizeof(int))
    if ((*(dst++) & *(src)) != *(src))
      return(FALSE);
    else
      src++;

  return(TRUE);
}

/*------------------------------------------------------------*/
/*
 * pthread_sigeqset2set - check if dst set is == src set
 */
int pthread_sigeqset2set(dst, src)
int *dst, *src;
{
  int i;

  for (i = 0; i < PTHREAD_SIGSET2SET_SIZE_NP; i += sizeof(int))
    if (*(dst++) != *(src++))
      return(FALSE);

  return(TRUE);
}

#ifndef STAND_ALONE
/*------------------------------------------------------------*/
/*
 * pthread_clear_sighandler - get rid of universal signal handler for all
 * signals except for those which cannot be masked;
 * also invalidate the timer if still active
 */
void pthread_clear_sighandler()
{
  struct sigaction vec;
  register int sig;
  struct itimerval it;

  vec.sa_handler = SIG_DFL;
  pthread_sigcpyset2set(&vec.sa_mask, &all_signals);
  vec.sa_flags = 0;

  for (sig = 1; sig < NSIG; sig++)
    if (sig != SIGPROF && sig != SIGKILL && sig != SIGSTOP)
      if (SIGACTION(sig, &vec, (struct sigaction *) NULL))
#ifdef DEBUG
        fprintf(stderr,
                "Pthreads: Could not install handler for signal %d\n", sig)
#endif
          ;

  if (!getitimer(ITIMER_REAL, &it) && timerisset(&it.it_value)) {
    it.it_value.tv_sec = it.it_value.tv_usec = 0;
    it.it_interval.tv_sec = it.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &it, (struct itimerval *) NULL);
  }
}
#endif /* !STAND_ALONE */

/*------------------------------------------------------------*/
/*
 * default_action - take default action on process
 * Notice: SIGSTOP and SIGKILL can never be received (unmaskable)
 * but they are included anyway.
 */
static void default_action(sig)
     int sig;
{
  switch (sig) {
#if !defined (__dos__)
#if !defined (_M_UNIX)
  case SIGURG:
  case SIGIO:
#endif
  case SIGCONT:
  case SIGCHLD:
  case SIGWINCH:
    break; /* ignore or continue */

  case SIGSTOP:
  case SIGTSTP:
  case SIGTTIN:
  case SIGTTOU:
#ifdef STAND_ALONE
    exit();
#else
    kill(getpid(), SIGSTOP);  /* stop process -> UNIX may generate SIGCHLD */
#endif
    break;
#endif /* __dos__ */

  default:
#ifdef TIMER_DEBUG
    fprintf(stderr, "last alarm is %d.%d\n",
            last_alarm.tv_sec, last_alarm.tv_usec);
    if (pthread_timer.head != NO_TIMER) {
      fprintf(stderr, "wakeup  time is %d.%d\n", pthread_timer.head->tp.tv_sec,
              pthread_timer.head->tp.tv_usec);
      gettimeofday(&pthread_timer.head->tp, (struct timezone *) NULL);
      fprintf(stderr, "current time is %d.%d\n", pthread_timer.head->tp.tv_sec,
                                                 pthread_timer.head->tp.tv_usec);
    }

#endif
#ifndef STAND_ALONE
    pthread_clear_sighandler();
    kill(getpid(), sig); /* reissue signal to terminate process: UNIX default */
    {
      sigset_t empty;

      sigemptyset(&empty);
      SIGPROCMASK(SIG_SETMASK, &empty, (struct sigset_t *) NULL);
    }
#endif /* !STAND_ALONE */
#ifdef DEBUG
#ifndef IO
    fprintf(stderr, "RUNAWAY: should produce core dump now\n");
#endif /* DEBUG */
#endif /* IO */
#ifdef STAND_ALONE
    exit();
#else /* !STAND_ALONE */
    pthread_process_exit(-1); /* abnormal termination, but no core dump */
#endif /* STAND_ALONE */
  }
}

/*------------------------------------------------------------*/
/*
 * handle_thread_signal - try to handle one signal on a thread and
 * return TRUE if it was handled, otherwise return FALSE
 */
static int handle_thread_signal(p, sig, code)
pthread_t p;
int sig;
int code;
{
  register struct context_t *scp;
#if defined(IO) && defined(_M_UNIX) && defined(USE_POLL)
  static int aio_calculate();
#endif
#ifdef C_CONTEXT_SWITCH
  extern void pthread_fake_call_wrapper_wrapper();
#endif /* C_CONTEXT_SWITCH */

#ifndef STAND_ALONE
  /*
   * handle timer signals
   */
  if (sig == TIMER_SIG) {
#ifdef TIMER_DEBUG
    fprintf(stderr, "handle_thread_signal: timer signal\n");
#endif
#ifdef DEF_RR
#ifdef REAL_TIME
    if (pthread_timer.head->mode == DL_TIME)
      pthread_cancel_timed_sigwait(p, TRUE, ANY_TIME, p->queue != &ready);
    else {
#endif /* REAL_TIME */
      if (pthread_timer.head->mode == RR_TIME && p->queue == &ready) {
        pthread_cancel_timed_sigwait(p, TRUE, ANY_TIME, FALSE);
        pthread_q_deq(&ready, p, PRIMARY_QUEUE);
        pthread_q_primary_enq(&ready, p);
      } else
#endif /* DEF_RR */
        pthread_cancel_timed_sigwait(p, TRUE, ANY_TIME, p->queue != &ready);
      return(TRUE);
#if defined(REAL_TIME) && defined(DEF_RR)
    }
#endif /* REAL_TIME && DEF_RR */
  }
#endif /* !STAND_ALONE */
  
  /*
   * handle signals for sigwait
   */
  if (p->state & T_SIGWAIT && sigismember(&p->sigwaitset, sig)) {
    pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
    p->state &= ~T_SIGWAIT;
    pthread_sigaddset2set(&p->mask, &p->sigwaitset);
    pthread_sigdelset2set(&p->mask, &cantmask);
    sigdelset(&p->sigwaitset, sig);
    return(TRUE);
  }
  
  /*
   * handler set to ignore
   */
  if (pthread_user_handler[sig].sa_handler == SIG_IGN && sig != SIGCANCEL)
    return(TRUE);
    
  /*
   * handler set to default action
   */
  if (pthread_user_handler[sig].sa_handler == SIG_DFL && sig != SIGCANCEL) {
    default_action(sig);
    return(TRUE);
  }

  /*
   * handle signals for sigsuspend and user handlers
   */
  if (sigismember(&handlerset, sig)) {
    if (p->state & T_BLOCKED) {
#if defined(IO) && defined(_M_UNIX)
      if (sigismember(&p->sigwaitset, AIO_SIG))
       {
         p->wait_on_select = 0;
         sigdelset(&p->sigwaitset, AIO_SIG);
         aio_calculate();
         p->how_many = -1;
       }
#endif
      p->terrno = EINTR;
    }
      
    if (!(p->state & T_RUNNING)) {
      if (p->state & T_SYNCTIMER)
        pthread_cancel_timed_sigwait(p, FALSE, SYNC_TIME, TRUE);
      else {
        pthread_q_wakeup_thread(p->queue, p, PRIMARY_QUEUE);
        if (p->state & (T_SIGWAIT | T_SIGSUSPEND)) {
          p->state &= ~(T_SIGWAIT | T_SIGSUSPEND);
          sigemptyset(&p->sigwaitset);
        }
      }
    }
      
    p->sig_info[sig].si_signo = sig;
#if (defined(STACK_CHECK) && defined(SIGNAL_STACK)) || defined(__linux__)
    if ((sig == SIGSEGV || sig == SIGBUS) && FC_CODE(code) != code) {
      p->sig_info[sig].si_value.sigval_ptr = (caddr_t) code;
      p->sig_info[sig].si_code = FC_PROT;
    }
    else
#endif /* STACK_CHECK && SIGNAL_STACK */
      p->sig_info[sig].si_code = code;

#ifdef C_CONTEXT_SWITCH
#ifdef TRASH
    /*
     * return avoids a loop in thread signal processing, no other solution
     */
    if (p->context[JB_PC] == (int) pthread_fake_call_wrapper_wrapper)
     {
       /*
	* We must mark the signal as pending!!!
        */
       return TRUE;
     }
#endif
    if (pthread_not_called_from_sighandler(p->context[JB_PC]))
      p->nscp = (struct context_t *) DIRECTED_AT_THREAD;
    p->sig = sig;
    p->osp = p->context[JB_SP];
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
    p->obp = p->context[JB_BP];
#endif
    p->opc = p->context[JB_PC];
    p->context[JB_PC] = (int) pthread_fake_call_wrapper_wrapper;
#ifdef ASM_SETJMP
    p->opc += RETURN_OFFSET;
    p->context[JB_PC] -= RETURN_OFFSET;
#endif /* ASM_SETJMP */
#else /* !C_CONTEXT_SWITCH */
    if (pthread_not_called_from_sighandler(p->context[JB_PC]))
      scp = (struct context_t *) DIRECTED_AT_THREAD;
    else
      scp = p->nscp;
    
    pthread_push_fake_call(p, pthread_user_handler[sig].sa_handler, sig,
                           scp, &pthread_user_handler[sig].sa_mask);
#endif /* !C_CONTEXT_SWITCH */
    return(TRUE);
  }

  /*
   * handle cancel signal
   */
  if (sig == SIGCANCEL) {
    if (p->state & T_SYNCTIMER)
      pthread_cancel_timed_sigwait(p, FALSE, ALL_TIME, TRUE);
    else if (p->state & (T_SIGWAIT | T_SIGSUSPEND)) {
      p->state &= ~(T_SIGWAIT | T_SIGSUSPEND);
      sigemptyset(&p->sigwaitset);
    }

    if (p->queue && !(p->state & T_RUNNING))
      pthread_q_deq(p->queue, p, PRIMARY_QUEUE);
    
    pthread_q_deq(&all, p, ALL_QUEUE);
    
    /*
     * no more signals for this thread, not even cancellation signal
     */
    pthread_sigcpyset2set(&p->mask, &all_signals);
    sigaddset(&p->mask, SIGCANCEL);
#ifdef C_CONTEXT_SWITCH
    p->nscp = (struct context_t *) DIRECTED_AT_THREAD;
    p->sig = (int) PTHREAD_CANCELED;
    p->context[JB_PC] = (int) pthread_fake_call_wrapper_wrapper;
#ifdef ASM_SETJMP
    p->context[JB_PC] -= RETURN_OFFSET;
#endif /* ASM_SETJMP */
#else /* !C_CONTEXT_SWITCH */
    pthread_push_fake_call(p, pthread_exit, PTHREAD_CANCELED,
                           (struct context_t *) DIRECTED_AT_THREAD,
                           (struct sigset_t *) NULL);
#endif /* !C_CONTEXT_SWITCH */
    if (!(p->state & T_RUNNING))
      pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
    
    return(TRUE);
  }

  return (FALSE);
}

/*------------------------------------------------------------*/
/*
 * handle_one_signal - handle one signal on the process level
 * assumes SET_KERNEL_FLAG
 */
static void handle_one_signal(sig, code)
int sig;
int code;
{
  register pthread_t p = mac_pthread_self();
  struct itimerval it;
  struct timespec now;
  extern pthread_t pthread_q_all_find_receiver();
  static int aio_handle();

  /*
   * Determine who needs to get the signal (in the following order):
   * (1) signal directed at specific thread: take this thread
   * (2) signal at process level:
   * (2a) synchronous signal: direct at current thread
   * (2b) SIGALRM, timer queue not empty, timer expired: take head off timer q
   * (2c) SIGIO, asynchronous I/O requested: determine receiver and make ready
   * (2c) handler defined: take first thread in all queue with signal unmasked
   * (3) no handler defined: pend signal on process till thread unmasks signal
   *      if signal already pending, it's lost
   */
  if (p != NO_PTHREAD &&
      (p->nscp == DIRECTED_AT_THREAD ||
       pthread_not_called_from_sighandler(p->context[JB_PC])) &&
      (p = (pthread_t) code))
    code = 0;
  else if (p != NO_PTHREAD && sigismember(&synchronous, sig))
    /* p = p */;
#ifdef STAND_ALONE
  else if (sig == SIGALRM) {
    if ((p = pthread_timer) && 
        !clock_gettime(CLOCK_REALTIME, &now) && GTEQ_NTIME(now, p->tp))
      pthread_cancel_timed_sigwait(p, TRUE, ANY_TIME, p->queue != &ready);
    return;
  }
  /* SIGIO is being used to invoke pthread_select_isr which takes care of 
   * suspending and waking up threads waiting on I/O.
   * The parameter 2 is used instead of UART_ISR
   */
     
  else if (sig == SIGIO) {
    pthread_select_isr(SIGIO);
    return;
  }
#else /* !STAND_ALONE */
  else if (sig == SIGALRM) {
    if (pthread_pending_sigaction.arg) {
      pthread_pending_sigaction.func(pthread_pending_sigaction.arg);
      pthread_pending_sigaction.func = NULL;
      pthread_pending_sigaction.arg = NULL;
    }
    if (
#ifdef DEF_RR
        pthread_timer.head != NO_TIMER && (p = pthread_timer.head->thread)
#else /* !DEF_RR */
        (p = pthread_timer.head)
#endif /* DEF_RR */
#if !defined(SOLARIS) && !defined(_M_UNIX)
        && !getitimer(ITIMER_REAL, &it) && !timerisset(&it.it_value)
#endif /* !SOALRIS && !_M_UNIX */
	) {
#if defined(SOLARIS) || defined(_M_UNIX)
      /*
       * ignore early wakeups
       */
      struct timeval now;

      gettimeofday(&now, (struct timezone *) NULL);
      if (GT_TIME(p->tp, now)) {
        struct itimerval it;

        it.it_interval.tv_sec = it.it_interval.tv_usec = 0;
        MINUS_TIME(it.it_value, p->tp, now);
        setitimer(ITIMER_REAL, &it, (struct itimerval *) NULL);
        return;
      }
#endif /* SOLARIS || _M_UNIX */
      sig = TIMER_SIG;
    }
    else {
      return; /* signal already processed (timer not yet expired) */
    }
  }
#endif /* !STAND_ALONE */
#if defined(IO) && !defined(__FreeBSD__) && !defined(_M_UNIX) && !defined(__dos__)
  else if (sig == SIGIO && aio_handle())
    return;
#endif /* IO && !__FreeBSD__ && !_M_UNIX && !__dos__ */
#if defined(SCO5)
  else if (sig == SIGCHLD) {
    extern int wait_handle();
    wait_handle();
    if (p == NO_PTHREAD)
      return;
  }
#endif
  else if (!(p = pthread_q_all_find_receiver(&all, sig))) {
    if (!sigismember(&pending_signals, sig)) {
      sigaddset(&pending_signals, sig);
      pending_code[sig] = code;
    }
    return;
  }
  
  if (p->state & T_RETURNED)
    return;
  
  /*
   * Pend signal on thread if it's masked out OR
   * if the signal is SIGCANCEL, the interrupt state CONTROLLED, and
   * we are not at an interruption point.
   */
  if (sigismember(&p->mask, sig) ||
      sig == SIGCANCEL &&
      p->state & T_CONTROLLED && !(p->state & T_INTR_POINT)) {
    sigaddset(&p->pending, sig);
    p->sig_info[sig].si_code = code;
    return;
  }

  if (handle_thread_signal(p, sig, code))
    return;
  
  default_action(sig);
}
  
/*------------------------------------------------------------*/
/*
 * pthread_handle_many_process_signals - determine pending signal(s).
 * if no thread ready, suspend process;
 * returns the head of the ready queue.
 * assumes SET_KERNEL_FLAG
 */
pthread_t pthread_handle_many_process_signals()
{
  register int sig;
#if defined(_M_UNIX)
  static int aio_wait();
#endif

  do {
    while (pthread_signonemptyset(&new_signals)) {
      /*
       * start critical section
       */
      SIGPROCMASK(SIG_BLOCK, &all_signals, (struct sigset_t *) NULL);
      
      pthread_sigcpyset2set(&sig_handling, &new_signals);
      pthread_sigdelset2set(&new_signals, &sig_handling);
      
      SIGPROCMASK(SIG_UNBLOCK, &all_signals, (struct sigset_t *) NULL);
      /*
       * end of critical section
       */

      for (sig = 1; sig < NNSIG; sig++)
        if (sigismember(&sig_handling, sig))
          handle_one_signal(sig, new_code[sig]);
    }

    /*
     * No thread, no action: suspend waiting for signal at process level
     */
    if (ready.head == NO_PTHREAD) {
      SIGPROCMASK(SIG_BLOCK, &all_signals, (struct sigset_t *) NULL);
      if (!pthread_signonemptyset(&new_signals)) {
#ifdef DEBUG
#ifndef IO
        fprintf(stderr, "suspending process waiting for signal\n");
#endif
#endif
#if defined(IO) && defined(_M_UNIX)
        while (!pthread_signonemptyset(&new_signals) && ready.head==NO_PTHREAD) {
#if defined(USE_POLL)
	  if (gnfds>0) {
#else
          if (gwidth>0) {
#endif
             SIGPROCMASK(SIG_UNBLOCK, &all_signals, (struct sigset_t *) NULL);
             aio_wait();
             SIGPROCMASK(SIG_BLOCK, &all_signals, (struct sigset_t *) NULL);
          }
          else
             SIGSUSPEND(&no_signals);
        }
#else
#ifdef STAND_ALONE
        while(!pthread_signonemptyset(&new_signals))
#endif
          SIGSUSPEND(&no_signals);
#endif /* IO && _M_UNIX */
      }
      SIGPROCMASK(SIG_UNBLOCK, &all_signals, (struct sigset_t *) NULL);
    }

  } while (ready.head == NO_PTHREAD);

  return(ready.head);
}

/*------------------------------------------------------------*/
/*
 * pthread_handle_one_process_signal - handle latest signal caught by 
 * universal handler while not in kernel
 * returns the head of the ready queue.
 * assumes SET_KERNEL_FLAG
 */
void pthread_handle_one_process_signal(sig, code)
int sig;
int code;
{
  handle_one_signal(sig, code);

  if (pthread_signonemptyset(&new_signals) ||
      ready.head == NO_PTHREAD)
    pthread_handle_many_process_signals();
}

/*------------------------------------------------------------*/
/*
 * pthread_handle_pending_signals - handle unmasked pending signals of 
 * current thread assumes SET_KERNEL_FLAG
 */
void pthread_handle_pending_signals()
{
  pthread_t p = mac_pthread_self();
  int sig;

  /*
   * handle signals pending on threads if they are unmasked and
   * SIGCANCEL only on an interruption point.
   */
  if (!pthread_siggeset2set(&p->mask, &p->pending))
    for (sig = 1; sig < NNSIG; sig++)
      if (sigismember(&p->pending, sig) && !sigismember(&p->mask, sig) && 
          (sig != SIGCANCEL || p->state & T_INTR_POINT)) {
        sigdelset(&p->pending, sig);
        
        handle_thread_signal(p, sig, p->sig_info[sig].si_code);
      }

  /*
   * handle signals pending on process
   */
  if (!pthread_siggeset2set(&p->mask, &pending_signals))
    for (sig = 1; sig < NNSIG; sig++)
      if (sigismember(&pending_signals, sig) && !sigismember(&p->mask, sig)) {
        sigdelset(&pending_signals, sig);
        
        handle_thread_signal(p, sig, pending_code[sig]);
      }
}

#ifndef STAND_ALONE
/*------------------------------------------------------------*/
/*
 * sighandler - wrapper for all signals, defers signals for later processing
 * Notice: All maskable signals are caught and re-multiplexed by Pthreads.
 */
#ifdef SVR4
static void sighandler(sig, sip, scp)
     struct siginfo *sip;
     struct context_t *scp;
#define SIPP &sip
#elif defined(SCO5)
static void sighandler(sig, sip, scp)
     struct siginfo *sip;
     struct context_t *scp;
#define SIPP &sip
#elif defined(_M_UNIX) || defined(__linux__)
static void sighandler(sig, scp)
     struct context_t scp;
#define SIPP NULL
#elif defined(__dos__)
static void sighandler(sig)
#define SIPP NULL
#else /* !_M_UNIX && !__linux__ && !__dos__ */
static void sighandler(sig, code, scp, addr)
     int code;
     char *addr;
     struct context_t *scp;
#define SIPP NULL
#endif /* !_M_UNIX */
     int sig;
{
  extern int pthread_started;
  register pthread_t p = mac_pthread_self();
#if (defined(_M_UNIX) && !defined(SCO5)) || defined(__linux__) || defined(__dos__)
  int code = 0;
#endif
#ifdef SVR4
  sigset_t omask;
  greg_t opc = scp->sc_pc;
  greg_t osp = scp->sc_sp;
#endif /* SVR4 */
#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
  void pthread_io_end();
  SIGSTACK_T ss;

#ifdef TDI_SUPPORT
  /* the initial value of __pthread_debug_TDI_sig_ignore is    *
   * the negative signal number of the sched signal            *
   * ---> (-SIGALRM) in case of the FSU impl                   *
   *                                                           *
   * if the debugger wants to switch off signals, it sets the  *
   * value of __pthread_debug_TDI_sig_ignore to somewhat > 0   *
   *                                                           */

  if (__pthread_debug_TDI_sig_ignore > 0) {
#ifdef TDI_DEBUG   
    char out[100];
    sprintf(out,"ignore signal %d\n",sig);
    write(1,out,strlen(out));
#endif /* /* TDI_DEBUG */ */
    
    /* set the bit for this signal */
    __pthread_debug_TDI_ignored_signals |= 0x1<<(sig-1);
    
    /* we simply return :                                   *
     * - the debugger have to send the ignored signal again */
    return;
  }
#endif /* /* TDI_SUPPORT */ */

  if (sig == SIGILL || sig == SIGBUS || sig == SIGSEGV) {
    if (p != NO_PTHREAD && !(p->state & T_LOCKED))
      default_action(sig);

    if (((sig == SIGILL && SCP_CODE == ILL_STACK) || 
#ifdef SVR4
         (sig == SIGSEGV &&
          (FC_CODE(SCP_CODE) == FC_HWERR || FC_CODE(SCP_CODE) == FC_ALIGN ||
	   FC_CODE(SCP_CODE) == FC_PROT) && !(p->state & T_MAIN)) ||
	  !(p->state & T_MAIN) &&
#else
	 (sig == SIGSEGV && FC_CODE(SCP_CODE) == FC_PROT &&
	  !(p->state & T_MAIN)) ||
	 (sig == SIGBUS && BUS_CODE(SCP_CODE) == BUS_OBJERR &&
	  !(p->state & T_MAIN)) ||
	  !(p->state & T_MAIN) &&
#endif
         (sig == SIGBUS && BUS_CODE(SCP_CODE) == BUS_OBJERR)) &&
        p != NO_PTHREAD &&
        ((scp->sc_sp < PA(p->stack_base+3*pthread_page_size))
#if defined(IO) && !defined(__FreeBSD__) && !defined(_M_UNIX) && !defined(__linux__) && !defined(__dos__)
         || (scp->sc_pc > (int)read && scp->sc_pc < (int)pthread_io_end)
#endif /* IO && !__FreeBSD__ && !_M_UNIX && !__linux__ && !__dos__ */
         )) {
      pthread_unlock_stack(p);

      pthread_switch_stacks(scp->sc_sp, SIPP, &scp);
    }
    /*
     * for DSM page fault detection
     */
    else if (sig == SIGSEGV
#ifndef SOLARIS
	     && FC_CODE(SCP_CODE) == FC_PROT
#endif
	     || sig == SIGBUS
#ifndef SOLARIS
	     && BUS_CODE(SCP_CODE) == BUS_OBJERR
#endif
	     ) {
      pthread_switch_stacks(scp->sc_sp, SIPP, &scp);

#ifndef SOLARIS
      SCP_CODE = (int) SIG_ADDR;
#else
      sip->si_code = (int) sip->si_addr;
#endif
    }

#ifndef SVR4
    /*
     * Let UNIX know that we are not on signal stack anymore. Does not work
     * for SVR4, does work for BSD.
     */
    if (SCP_ONSTACK)
      scp->sc_onstack = FALSE;
    else {
      ss.ss_sp = (char *) SA((int) pthread_tempstack_top - STACK_OFFSET);
      scp->sc_onstack = FALSE;
      if (SIGSTACK(&ss, (SIGSTACK_T *) NULL))
#ifdef DEBUG
        fprintf(stderr,
            "Pthreads: Could not specify signal stack, errno %d\n", errno)
#endif /* DEBUG */
                ;
    }
#endif /* !SVR4 */
  }
#endif /* STACK_CHECK && SIGNAL_STACK */

#ifdef __linux__

#ifdef TDI_SUPPORT
  /* the initial value of __pthread_debug_TDI_sig_ignore is    *
   * the negative signal number of the sched signal            *
   * ---> (-SIGALRM) in case of the FSU impl                   *
   *                                                           *
   * if the debugger wants to switch off signals, it sets the  *
   * value of __pthread_debug_TDI_sig_ignore to somewhat > 0   *
   *                                                           */

  if (__pthread_debug_TDI_sig_ignore > 0) {
#ifdef TDI_DEBUG   
    char out[100];
    sprintf(out,"ignore signal %d\n",sig);
    write(1,out,strlen(out));
#endif /* /* TDI_DEBUG */ */
   
    /* set the bit for this signal */
    __pthread_debug_TDI_ignored_signals |= 0x1<<(sig-1);

    /* we simply return :                                   *
     * - the debugger have to send the ignored signal again */
    return;
  }
#endif /* /* TDI_SUPPORT */ */

  if (sig == SIGSEGV)
    SCP_CODE = (int) SIG_ADDR;
#endif /* __linux__ */

#ifdef SVR4
  pthread_sigcpyset2set(&omask, &scp->sc_mask);
#endif /* SVR4 */

#if defined(IO) && defined(_M_UNIX) 
#if defined(USE_POLL)
  gtimeout = 0;
#else
  gtimeout.tv_sec = 0;
  gtimeout.tv_usec = 0;
#endif
#endif
  /*
   * add signal to queue of pending signals
   */
#ifdef DEBUG
#ifndef IO
#ifdef DEF_RR
  if (sig != SIGALRM)
#endif
    fprintf(stderr, "signal %d caught\n", sig);
#endif
#endif

  if (!sigismember(&new_signals, sig) && pthread_started)
#if (defined(_M_UNIX) && !defined(SCO5)) || defined(__linux__) || defined(__dos__)
    if (!is_in_kernel) {
#else
    if (!is_in_kernel && scp) {
#endif
      new_cond[sig] = p->cond;
#if (defined(_M_UNIX) && !defined(SCO5)) || defined(__linux__)
      new_scp[sig] = &scp;
#elif defined (__dos__)
      new_scp[sig] = __djgpp_exception_state;
#else
      new_scp[sig] = scp;
#endif
      SET_KERNEL_FLAG;
      SIGPROCMASK(SIG_UNBLOCK, &all_signals, (struct sigset_t *) NULL);

      /*
       * Associate UNIX context with current thread
       */
      p->scp = p->nscp;
#if (defined(_M_UNIX) && !defined(SCO5)) || defined(__linux__)
      p->nscp = &scp;
#elif defined (__dos__)
      p->nscp = __djgpp_exception_state;
#else
      p->nscp = scp;
#endif

      /*
       * A signal is pending so that the signal dispatcher calls
       * pthread_handle_one_process_signals()
       * and then switches to highest priority thread. Thus, we act as if no
       * context switch is initiated, i.e. as if we switched back to thread
       * which was running when the signal came in.
       */
#ifndef C_CONTEXT_SWITCH
      pthread_sched_wrapper(sig, SCP_CODE);
#else
      p->terrno = errno;
      pthread_sched_wrapper(sig, SCP_CODE, p);
      errno = p->terrno;
      CLEAR_KERNEL_FLAG;
#endif
    }
    else {
      sigaddset(&new_signals, sig);
      new_code[sig] = SCP_CODE;
      new_cond[sig] = (p ? p->cond : NULL);
#if (defined(_M_UNIX) && !defined(SCO5)) || defined(__linux__)
      new_scp[sig] = &scp;
#elif defined (__dos__)
      new_scp[sig] = __djgpp_exception_state;
#else
      new_scp[sig] = scp;
#endif
      state_change = TRUE;
    }
#ifdef SVR4
  /*
   * Solaris does not seem to have a _sigtramp() wrapper for signal handlers.
   * In fact, it is not quite understood how the signal mask is reset after
   * returning from the signal handler. This is fixed by explicitly setting
   * the context.
   */
  setcontext((struct ucontext *) scp);
#endif /* SVR4 */
}
#endif /* !STAND_ALONE */

#if defined(IO)
int pthread_getfdtablesize()

{
#if defined(SCO5)
  struct rlimit rl;
  getrlimit (RLIMIT_NOFILE, &rl);
  return rl.rlim_cur;
#else
  return FD_SETSIZE;
#endif
}
#endif

/*------------------------------------------------------------*/
/*
 * pthread_init_signals - initialize signal package
 */
void pthread_init_signals()
{
  int sig;
  struct sigaction vec;
#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
  SIGSTACK_T ss;

  ss.ss_sp = (char *) SA((int) pthread_tempstack_top - STACK_OFFSET);
  CLR_SS_ONSTACK;
#ifndef STAND_ALONE
  if (SIGSTACK(&ss, (SIGSTACK_T *) NULL))
#ifdef DEBUG
    fprintf(stderr,
            "Pthreads: Could not specify signal stack, errno %d\n", errno)
#endif /* DEBUG */
    ;
#endif /* !STAND_ALONE */
#endif

  /*
   * initialize kernel structure
   */
  is_in_kernel = is_updating_timer = FALSE;

  sigemptyset(&synchronous);
  sigaddset(&synchronous, SIGILL);
  sigaddset(&synchronous, SIGABRT);
#if !defined (__linux__) && !defined(__dos__)
  sigaddset(&synchronous, SIGEMT);
#endif /* !__linux__ && !__dos__ */
  sigaddset(&synchronous, SIGFPE);
  sigaddset(&synchronous, SIGBUS);
  sigaddset(&synchronous, SIGSEGV);
  
  sigemptyset((sigset_t *) &new_signals);
  sigemptyset(&pending_signals);
  sigemptyset(&handlerset);
  sigemptyset(&sig_handling);

  sigemptyset(&cantmask);
  sigaddset(&cantmask, SIGKILL);
  sigaddset(&cantmask, SIGSTOP);
  sigaddset(&cantmask, SIGCANCEL);

  sigfillset(&all_signals);
  sigdelset(&all_signals, SIGKILL);
  sigdelset(&all_signals, SIGSTOP);

  pthread_queue_init(&ready);
  pthread_queue_init(&all);
  pthread_queue_init(&suspend_q);
#ifdef STAND_ALONE
  pthread_timer_queue_init(pthread_timer);
#else
  pthread_timer_queue_init(&pthread_timer);
#endif
  set_warning = "CAUTION: entering kernel again\n";
  clear_warning = "CAUTION: leaving kernel again\n";
  prio_warning = "CAUTION: prio violation when leaving kernel\n";
#ifdef RAND_SWITCH
  srandom(1);
  pthread_n_ready = 0;
#endif

#ifdef STAND_ALONE
  sigemptyset(&proc_mask);
  cur_heap = (int)(&heap_start);
#else /* !STAND_ALONE */
  /*
   * no signal requests
   */
  for (sig = 0; sig < NNSIG; sig++) {
    pthread_user_handler[sig].sa_handler = SIG_DFL;
    sigemptyset(&pthread_user_handler[sig].sa_mask);
    pthread_user_handler[sig].sa_flags = 0;
    new_code[sig] = 0;
    new_cond[sig] = NULL;
    new_scp[sig] = NULL;
  }

  /*
   * install universal signal handler for all signals
   * except for those which cannot be masked
   */
  vec.sa_handler = sighandler;
  pthread_sigcpyset2set(&vec.sa_mask, &all_signals);
#if (defined(_M_UNIX) && !defined(SCO5)) || defined(__dos__)
  vec.sa_flags = SA_SIGINFO;
#else
  vec.sa_flags = SA_SIGINFO | SA_RESTART;
#endif
#ifdef __linux__
  vec.sa_restorer = (void (*)(void)) NULL;
#endif
#ifdef __linux__
  /*
   * Fix for Linux 2.2.x kernels: SA_SIGINFO causes POSIX-like
   * signal handler parameter (sig, *sip, *scp) but we still assume
   * old parameters (sig, scp). Also, sip->si_addr lacks the fault
   * address for SIGSEGV at this point. So let's change to the new
   * layout some time later when Linux matches the standard.
   * Unfortunately, the assigment to a const parameter below causes a
   * warning. Also ignored for now.
   */
  if (vec.sa_flags & SA_SIGINFO)
    vec.sa_flags &= ~SA_SIGINFO;
#endif
    
  for (sig = 1; sig < NSIG; sig++)
    if (sig != SIGPROF && !sigismember(&cantmask, sig)) {
#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
      if (sig == SIGBUS || sig == SIGILL || sig == SIGSEGV)
        vec.sa_flags |= SA_ONSTACK;
#endif
      if (SIGACTION(sig, &vec, (struct sigaction *) NULL))
#ifdef DEBUG
        fprintf(stderr, "Pthreads (signal): \
          Could not install handler for signal %d\n", sig)
#endif
          ;
#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
      if (sig == SIGBUS || sig == SIGILL || sig == SIGSEGV)
        vec.sa_flags &= ~SA_ONSTACK;
#endif
    }
#endif /* STAND_ALONE */

#if defined(IO) && !defined(__FreeBSD__) && !defined(__linux__) && !defined(__dos__)
#if defined(USE_POLL)
  gnfds = 0;
  gmaxnfds = pthread_getfdtablesize();
#if defined(IO_DEBUG)
  kprintf("fdtablesize=%d\n", gmaxnfds);
#endif
#if defined(MALLOC)
  gafds[0] = (struct pollfd*)pthread_malloc(sizeof(struct pollfd)* gmaxnfds);
  gafds[1] = (struct pollfd*)pthread_malloc(sizeof(struct pollfd)* gmaxnfds);
#else
  gafds[0] = (struct pollfd*)malloc(sizeof(struct pollfd)* gmaxnfds);
  gafds[1] = (struct pollfd*)malloc(sizeof(struct pollfd)* gmaxnfds);
#endif
  gfds = gafds[0];
#else
  gwidth = 0;
  FD_ZERO(&greadfds);
  FD_ZERO(&gwritefds);
  FD_ZERO(&gexceptfds);
#endif
#endif /* IO && !__FreeBSD__ && !__linux__ && !__dos__ */
}

/*------------------------------------------------------------*/
/*
 * sigwait - suspend thread until signal becomes pending
 * Return: signal number if o.k., otherwise -1
 * Notice: cannot mask SIGKILL, SIGSTOP, SIGCANCEL
 */
int sigwait(set, sigp)
     int *sigp;
     SIGWAIT_CONST sigset_t *set;
{
  register int sig;
  sigset_t new, more;
  register pthread_t p = mac_pthread_self();

  pthread_sigcpyset2set(&new, set);
  pthread_sigdelset2set(&new, &cantmask);
  pthread_sigcpyset2set(&more, &new);
  SET_KERNEL_FLAG;

  /*
   * Are the signals in set blocked by the current thread?
   */
  if (!pthread_siggeset2set(&p->mask, &more)) {
    set_errno(EINVAL);
    CLEAR_KERNEL_FLAG;
    return(-1);
  }

  /*
   * Any of the signals in set pending on thread?
   */
  if (pthread_sigismemberset2set(&p->pending, &more))
    for (sig = 1; sig < NNSIG; sig++)
      if (sigismember(&p->pending, sig) && sigismember(&more, sig)) {
        sigdelset(&p->pending, sig);
        CLEAR_KERNEL_FLAG;
	*sigp = sig;
        return(0);
      }
    
  /*
   * Any of the signals in set pending on process?
   */
  if (pthread_sigismemberset2set(&pending_signals, &more))
    for (sig = 1; sig < NNSIG; sig++)
      if (sigismember(&pending_signals, sig) && sigismember(&more, sig)) {
        sigdelset(&pending_signals, sig);
        CLEAR_KERNEL_FLAG;
	*sigp = sig;
        return(0);
      }
    
  /*
   * suspend thread and wait for any of the signals
   */
  pthread_sigaddset2set(&p->sigwaitset, &new);
  pthread_sigdelset2set(&p->mask, &new);
  p->state &= ~T_RUNNING;
  p->state |= T_SIGWAIT | T_BLOCKED | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) && !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  if (get_errno() == EINTR)
    return(-1);

  /*
   * determine the received signal
   */
  for (sig = 1; sig < NNSIG; sig++)
    if (sigismember(&new, sig) && !sigismember(&p->sigwaitset, sig))
      break;

  /*
   * Clear signal mask
   */
  SET_KERNEL_FLAG;
  sigemptyset(&p->sigwaitset);
  CLEAR_KERNEL_FLAG;

  /*
   * If no signal woke us up directly, a user handler (hence interrupt) must
   * have been activated via a different signal.
   */
  if (sig < 0) {
    set_errno(EINVAL);
    return(-1);
  }

  *sigp = sig;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * sigprocmask - change or examine signal mask of thread
 * return 0 or -1 if error
 * cannot mask SIGKILL, SIGSTOP, SIGCANCEL
 */
int sigprocmask(how, set, oset)
int how;
SIGPROCMASK_CONST sigset_t *set;
sigset_t *oset;
{
  sigset_t new, old, pending;
  register pthread_t p = mac_pthread_self();
  register already_in_kernel = is_in_kernel;

#if defined(SOLARIS) && defined(IO)
  if (pthread_get_retaddr() < 0) /* call from dynamic library */
    return(pthread_p_sigprocmask(how, set, oset));
#endif /* SOLARIS && IO */

  if (!already_in_kernel)
    SET_KERNEL_FLAG;

  pthread_sigcpyset2set(&old, &p->mask);
  if (oset)
    pthread_sigcpyset2set(oset, &old);
  if (!set) {
    if (!already_in_kernel)
      CLEAR_KERNEL_FLAG;
    return(0);
  }

  pthread_sigcpyset2set(&new, set);
  pthread_sigdelset2set(&new, &cantmask);

  switch (how) {
  case SIG_BLOCK:
    pthread_sigaddset2set(&p->mask, &new);
    break;
  case SIG_UNBLOCK:
    pthread_sigdelset2set(&p->mask, &new);
    break;
  case SIG_SETMASK:
    pthread_sigcpyset2set(&p->mask, &new);
    break;
  default:
    set_errno(EINVAL);
    if (!already_in_kernel)
      CLEAR_KERNEL_FLAG;
    return(-1);
  }

  pthread_sigcpyset2set(&pending, &p->pending);
  pthread_sigaddset2set(&pending, &pending_signals);
  pthread_sigdelset2set(&old, &p->mask);
  if (pthread_sigismemberset2set(&pending, &old))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    SIM_SYSCALL(TRUE);
    if (!already_in_kernel)
      CLEAR_KERNEL_FLAG;
  }
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_sigmask - change or examine signal mask of thread
 * return 0 or error value if error
 * cannot mask SIGKILL, SIGSTOP, SIGCANCEL
 */
int pthread_sigmask(how, set, oset)
int how;
SIGPROCMASK_CONST sigset_t *set;
sigset_t *oset;
{
  int ret;
  ret = sigprocmask(how, set, oset);
  if (ret != 0)
    ret = get_errno();
  return ret;
}

/*------------------------------------------------------------*/
/*
 * sigpending - inquire about pending signals which are blocked, i.e. applies
 * to only those signals which are explicitly pending on the current thread
 * return 0 if o.k., -1 otherwise
 */
int sigpending(set)
sigset_t *set;
{
  if (!set) {
    set_errno(EINVAL);
    return(-1);
  }

  SIM_SYSCALL(TRUE);
  pthread_sigcpyset2set(set, &mac_pthread_self()->pending);
  pthread_sigaddset2set(set, &pending_signals);
  pthread_sigdelset2set(set, &cantmask);
  return(0);
}

/*------------------------------------------------------------*/
/*
 * sigsuspend - suspend thread,
 * set replaces the masked signals for the thread temporarily,
 * suspends thread, and resumes execution when a user handler is invoked
 * Return: -1 and EINTR if interrupted or EINVAL if wrong parameters
 * Notice: cannot mask SIGKILL, SIGSTOP
 */
int sigsuspend(set)
SIGSUSPEND_CONST sigset_t *set;
{
  register int sig;
  sigset_t old, pending;
  register pthread_t p = mac_pthread_self();

  if (!set) {
    set_errno(EINVAL);
    return(-1);
  }

  SET_KERNEL_FLAG;
  pthread_sigcpyset2set(&old, &p->mask);
  pthread_sigcpyset2set(&p->mask, set);
  pthread_sigdelset2set(&p->mask, &cantmask);
  pthread_sigaddset2set(&p->sigwaitset, &p->mask);

  p->state &= ~T_RUNNING;
  p->state |= T_SIGSUSPEND | T_BLOCKED | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) && !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  /*
   * restore the initial signal mask
   */
  SET_KERNEL_FLAG;
  pthread_sigcpyset2set(&p->mask, &old);

  pthread_sigcpyset2set(&pending, &p->pending);
  pthread_sigaddset2set(&pending, &pending_signals);
  pthread_sigcpyset2set(&old, set);
  pthread_sigdelset2set(&old, &cantmask);
  pthread_sigdelset2set(&old, &p->mask);
  if (pthread_sigismemberset2set(&pending, &old))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else
    CLEAR_KERNEL_FLAG;

  return(-1);
}

/*------------------------------------------------------------*/
/*
 * pause - suspend thread until any signal is caught,
 * same as sigsuspend except that the signal mask doesn't change
 */
int pause()
{
  register int sig;
  register pthread_t p = mac_pthread_self();

  SET_KERNEL_FLAG;
  pthread_sigcpyset2set(&p->sigwaitset, &p->mask);
  
  p->state &= ~T_RUNNING;
  p->state |= T_SIGSUSPEND | T_BLOCKED | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) && !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }
  
  return(-1);
}

#ifdef STAND_ALONE

/*------------------------------------------------------------*/
/*
 * pthread_timed_sigwait - suspend running thread until specified time
 * Return -1 if error, 0 otherwise
 * assumes SET_KERNEL_FLAG
 */
int pthread_timed_sigwait(p, timeout, mode, func, arg)
     pthread_t p;
     struct timespec *timeout;
     int mode;
     pthread_func_t func;
     any_t arg;
{
  struct timespec now;
  
  if (!timeout || timeout->ts_sec < 0 || 
      timeout->ts_nsec < 0 || timeout->ts_nsec >= 1000000000)
    return(EINVAL);
  clock_gettime(CLOCK_REALTIME, &now);
  
  if (mode == ABS_TIME) {
    if (GTEQ_NTIME(now, *timeout)) {
      set_errno(EAGAIN);
      return(-1);
    }
  }
  else if (mode == REL_TIME) {
    if (LE0_NTIME(*set_errno)) {
      set_errno(EAGAIN);
      return(-1);
    }
    PLUS_NTIME(*timeout, now, *timeout);
  }
  else {
    set_errno(EINVAL);
    return(-1);
  }
  /*   
   * queue up current thread on the timer queue
   */
  p->timer_func = func;
  p->timer_arg = arg;
  p->tp.ts_sec = timeout->ts_sec;
  p->tp.ts_nsec = timeout->ts_nsec;
  return pthread_q_timed_enq(&pthread_timer, p);
}

/*------------------------------------------------------------*/
/*
 * pthread_cancel_timed_sigwait - dequeue thread waiting on timer only
 * "signaled" indicates if the thread "p" received a SIGALRM
 * Notice: set error in both thread structure and global UNIX errno
 *         since this may be called from pthread_handle_many_process_signals
 * assumes SET_KERNEL_FLAG
 */
int pthread_cancel_timed_sigwait(first_p, signaled, mode, activate)
     pthread_t first_p;
     int signaled, mode, activate;
{
  pthread_t p = first_p;
  struct timespec now;
  int time_read = 0;
  
  if (pthread_timer == NO_PTHREAD) {
    is_updating_timer = FALSE;
    return(0);
  }
  
  do {
#ifdef TRASH
    if (!p->timer_func) {
#endif
      if (p->state & T_CONDTIMER) {
        p->state &= ~(T_CONDTIMER | T_SYNCTIMER);
        pthread_q_deq(p->queue, p, PRIMARY_QUEUE);
        pthread_q_timed_wakeup_thread(&pthread_timer, p,
#ifndef DEF_RR
                                      (p == first_p ? activate : TRUE));
#else
                                      ((p == first_p && (tmr->mode & mode)) ? activate : TRUE));
#endif
        if (p != first_p || signaled) {
          p->terrno = EAGAIN;
          
          if (p == mac_pthread_self())
            set_errno(EAGAIN);
	}
      }
      else {
        p->state &= ~T_SYNCTIMER;
        pthread_q_timed_wakeup_thread(&pthread_timer, p,
#ifndef DEF_RR
                                      (p == first_p ? activate : TRUE));
#else
                                      ((p == first_p && (tmr->mode & mode)) ? activate : TRUE));
#endif
      }
#ifdef TRASH
    }
    else
      (*p->timer_func)(p->timer_arg);
#endif

    p = pthread_timer;
    if (p != NO_PTHREAD && !time_read) {
      clock_gettime(CLOCK_REALTIME, &now);
      time_read++;
    }
  } while (p && GTEQ_NTIME(now, p->tp));
  
  is_updating_timer = FALSE;

  /*
   * timer signal received while cancelling => ignore it
   */
  if (!signaled && (sigismember(&new_signals, SIGALRM)))
    sigdelset(&new_signals, SIGALRM);
  
  return(0);
}

#else /* !STAND_ALONE */

/*------------------------------------------------------------*/
/*
 * pthread_timed_sigwait - suspend running thread until specified time
 * Return -1 if error, 0 otherwise
 * assumes SET_KERNEL_FLAG
 */
int pthread_timed_sigwait(p, timeout, mode, func, arg)
     pthread_t p;
     struct timespec *timeout;
     int mode;
     pthread_func_t func;
     any_t arg;
{
  struct itimerval it;
  struct timeval now, in;
  register timer_ent_t phead;

#ifdef TIMER_DEBUG
    fprintf(stderr, "timed_sigwait: enter\n");
#endif

#ifdef DEF_RR
  if (p->num_timers >= TIMER_MAX) {
    set_errno(EAGAIN);
    return(-1);
  }
#endif

  if (mode != RR_TIME) {
    if (!timeout || timeout->ts_nsec < 0 || timeout->ts_nsec >= 1000000000) {
      set_errno(EINVAL);
      return(-1);
    }
    else
      P2U_TIME(in, (*timeout));
  }

  if (gettimeofday(&now, (struct timezone *) NULL)) {
    set_errno(EINVAL);
    return(-1);
  }

  it.it_interval.tv_sec = it.it_interval.tv_usec = 0;

  if (mode == ABS_TIME) {
    /*
     * time has already passed
     */
    if (GTEQ_TIME(now, in)) {
      set_errno(EAGAIN);
      return(-1);
    }

    MINUS_TIME(it.it_value, in, now);
  }
  else if (mode == REL_TIME) {
    /*
     * time has already passed
     */
    if (LE0_TIME(in)) {
      set_errno(EAGAIN);
      return(-1);
    }

    it.it_value.tv_sec = in.tv_sec;
    it.it_value.tv_usec = in.tv_usec;
    PLUS_TIME(in, in, now);
  }
#ifdef DEF_RR
  else if (mode == RR_TIME) {
    p->state |= T_ASYNCTIMER;
    if ((p->interval.tv_sec == 0) && (p->interval.tv_usec == 0)) {
      in.tv_sec = it.it_value.tv_sec = 0;
      in.tv_usec = it.it_value.tv_usec = TIME_SLICE;
    }
    else {
      in.tv_sec = it.it_value.tv_sec = p->interval.tv_sec; 
      in.tv_usec = it.it_value.tv_usec = p->interval.tv_usec;
    }
    PLUS_TIME(in, in, now);
  }
#endif
  else {
    set_errno(EINVAL);
    return(-1);
  }

  /*
   * if no timer set, set timer to current request; otherwise
   * overwrite timer if current request needs to be served next
   */
  if (!(phead = pthread_timer.head) || GT_TIME(phead->tp, in)) {
    it.it_interval.tv_sec = it.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &it, (struct itimerval *) NULL)) {
#ifdef DEBUG
      fprintf(stderr, "ERROR: setitimer in timed_sigwait\n");
#endif
      set_errno(EINVAL);
      return(-1);
    }
#ifdef TIMER_DEBUG
    fprintf(stderr, "timed_sigwait: setitimer %d.%d sec.usec\n",
            it.it_value.tv_sec, it.it_value.tv_usec);
#endif
  }
#ifdef TIMER_DEBUG
  else
    fprintf(stderr, "timed_sigwait: timer not set up, pthread_timer.head=%x\n", phead);
#endif
    
  /*
   * queue up current thread on the timer queue
   */
  return pthread_q_timed_enq(&pthread_timer,
                      in,
                      func == (pthread_func_t) pthread_exit ? DL_TIME : mode,
                      p);
}

/*------------------------------------------------------------*/
/*
 * pthread_cancel_timed_sigwait - dequeue thread waiting for alarm only
 * "signaled" indicates if the thread "p" received a SIGALRM
 * Notice: set error in both thread structure and global UNIX errno
 *         since this may be called from pthread_handle_many_process_signals
 * assumes SET_KERNEL_FLAG
 */
int pthread_cancel_timed_sigwait(first_p, signaled, mode, activate)
     pthread_t first_p;
     int signaled, mode, activate;
{
  pthread_t p = first_p;
  timer_ent_t tmr;
  timer_ent_t old_tmr_head = pthread_timer.head;
  struct itimerval it;
  struct timeval now;
  int time_read = FALSE;
#if defined(IO) && defined(_M_UNIX)
  int recalculate = 0;
  static int aio_calculate();
#endif

  /*
   * find the first instance of this thread in timer queue
   */
#ifdef DEF_RR
  for (tmr = pthread_timer.head; tmr; tmr = tmr->next[TIMER_QUEUE])
    if (tmr->thread == p && (tmr->mode & mode))
      break;
#else
  tmr = p;
#endif

  if (!tmr) {
#ifdef TIMER_DEBUG
    fprintf(stderr, "pthread_cancel_timed_sigwait: exit0\n");
#endif
    return(0);
  }

  /*
   * for each occurrence, remove the timer entry
   */
  do {
#ifdef DEF_RR
    if (tmr->mode == RR_TIME) {
      p->state &= ~T_ASYNCTIMER;
      pthread_q_timer_deq(&pthread_timer, tmr);
    }
#ifdef REAL_TIME
    else if (tmr->mode == DL_TIME)
      pthread_q_timer_deq(&pthread_timer, tmr);
#endif /* REAL_TIME */
    else
#endif /* DEF_RR */
    if (p->state & T_CONDTIMER) {
      p->state &= ~(T_CONDTIMER | T_SYNCTIMER);
      pthread_q_deq(p->queue, p, PRIMARY_QUEUE);
#ifndef DEF_RR
      pthread_q_timed_wakeup_thread(&pthread_timer, p, 
                                      (p == first_p ? activate : TRUE));
#else
      pthread_q_timer_deq(&pthread_timer, tmr);
      pthread_q_timed_wakeup_thread(NULL, p, 
                      ((p == first_p && (tmr->mode & mode)) ? activate : TRUE));
#endif
      if (p != first_p || signaled) {
        p->terrno = EAGAIN;
#ifdef __dos__
        p->terrno = 0; /* To pass c95040b and c9a008a */
#endif

        if (p == mac_pthread_self())
          set_errno(EAGAIN);
      }
    }  
    else if (p->state & T_SYNCTIMER) {
      p->state &= ~T_SYNCTIMER;
#ifndef DEF_RR
      pthread_q_timed_wakeup_thread(&pthread_timer, p,
                     ((p == first_p) ? activate : TRUE));
#else
      pthread_q_timer_deq(&pthread_timer, tmr);
      pthread_q_timed_wakeup_thread(NULL, p,
                     ((p == first_p && (tmr->mode & mode)) ? activate : TRUE));
#endif
#if defined(IO) && defined(_M_UNIX)
      if (sigismember(&p->sigwaitset, AIO_SIG)) {
       p->wait_on_select = 0;
       p->how_many = 0;
       p->terrno = 0;
       if (p==mac_pthread_self())
        set_errno(0); 
       recalculate++;
       sigdelset(&p->sigwaitset, AIO_SIG);
      }
#endif
     }
#if defined(TIMER_DEBUG)
    else {
#if defined(DEF_RR)
       fprintf(stderr, "pthread_cancel_timed_sigwait: thread=%d invalid timer=%d return(0)\n", p, tmr->mode);
#else
       fprintf(stderr, "pthread_cancel_timed_sigwait: thread=%d invalid timer=%d return(0)\n", p, mode);
#endif
       fflush(stderr);
       _exit(1);
     }
#endif

#ifdef DEF_RR
    /*
     * find next instance of this thread in timer queue
     */
    if (mode == ALL_TIME) {
      tmr = pthread_timer.head;
      while (tmr && tmr->thread != p)
        tmr = tmr->next[TIMER_QUEUE];
    }
    else
      tmr = NO_TIMER;

    /*
     * check if head of timer queue can be woken up, i.e. now > tmr->tp
     */
    if (tmr == NO_TIMER && !time_read)
#else
    if (!time_read)
#endif
      if (gettimeofday(&now, (struct timezone *) NULL)) {
#ifdef TIMER_DEBUG
        fprintf(stderr, "pthread_cancel_timed_sigwait: exit1\n");
#endif
        set_errno(EINVAL);
	return(-1);
      }
      else
        time_read = TRUE;

    if (time_read) {
      tmr = pthread_timer.head;
#ifdef DEF_RR
      if (tmr)
        p = tmr->thread;
#else
      p = tmr;
#endif
    }
      
  } while (tmr && (!time_read || GTEQ_TIME(now, tmr->tp)));

  /*
   * We need recalculate the threads waiting for synchronous I/O
   */
#if defined(IO) && defined(_M_UNIX)
  if (recalculate)
    aio_calculate();
#endif

  /*
   * if head of timer queue hasn't changed, no action
   */
  if (tmr == old_tmr_head && !signaled) {
#ifdef TIMER_DEBUG
    fprintf(stderr, "pthread_cancel_timed_sigwait: exit2\n");
#endif
    return(0);
  }

  /*
   * delete SIGALRM from new_signals if it came in meanwhile
   */
  sigdelset(&new_signals, SIGALRM);

  /*
   * overwrite timer if current request needs to be served next or invalidate
   */
  if (tmr != NO_TIMER)
    MINUS_TIME(it.it_value, tmr->tp, now);
  else
    it.it_value.tv_sec = it.it_value.tv_usec = 0;

  it.it_interval.tv_sec = it.it_interval.tv_usec = 0;
  if (setitimer(ITIMER_REAL, &it, (struct itimerval *) NULL)) {
#ifdef DEBUG
    fprintf(stderr, "ERROR: setitimer in pthread_cancel_timed_sigwait\n");
#endif
    set_errno(EINVAL);
    return(-1);
  }

#ifdef TIMER_DEBUG
  last_alarm.tv_sec = it.it_value.tv_sec;
  last_alarm.tv_usec = it.it_value.tv_usec;
#endif

#ifdef TIMER_DEBUG
  fprintf(stderr, "pthread_cancel_timed_sigwait: setitimer %d.%d sec.usec\n",
          it.it_value.tv_sec, it.it_value.tv_usec);
#endif
  
  return(0);
}

#endif /* STAND_ALONE */

/*------------------------------------------------------------*/
/*
 * raise - send a signal to the current process;
 * NOT directed to any particular thread,
 * any thread waiting for the signal may pick it up.
 */
#ifdef _M_UNIX
#ifdef raise
#undef raise
#endif
#endif

int raise(sig)
int sig;
{
  SET_KERNEL_FLAG;
  SIM_SYSCALL(TRUE);
#ifdef C_CONTEXT_SWITCH
  if (!SAVE_CONTEXT(mac_pthread_self()))
#endif
    pthread_signal_sched(sig, (int) NO_PTHREAD);

#ifdef C_CONTEXT_SWITCH
  CLEAR_KERNEL_FLAG;
#endif

  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_kill - send signal to thread
 */
int pthread_kill(thread, sig)
pthread_t thread;
int sig;
{
  if (thread == NO_PTHREAD || thread->state & T_RETURNED ||
      sigismember(&cantmask, sig) || !sigismember(&all_signals, sig))
    return(EINVAL);

  /*
   * queue up signal associated with thread
   */
  SET_KERNEL_FLAG;
  SIM_SYSCALL(TRUE);
#ifdef C_CONTEXT_SWITCH
  if (!SAVE_CONTEXT(mac_pthread_self()))
#endif
    pthread_signal_sched(sig, (int) thread);

#ifdef C_CONTEXT_SWITCH
  CLEAR_KERNEL_FLAG;
#endif

  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_cancel - cancel thread
 * Open question: is this an interruption point if a thread cancels itself?
 * As of now, it isn't!
 */
int pthread_cancel(thread)
pthread_t thread;
{
  if (thread == NO_PTHREAD || thread->state & T_RETURNED)
    return(EINVAL);

  /*
   * queue up signal associated with thread
   */
  SET_KERNEL_FLAG;
  SIM_SYSCALL(TRUE);
#ifdef C_CONTEXT_SWITCH
  if (!SAVE_CONTEXT(mac_pthread_self()))
#endif
    pthread_signal_sched(SIGCANCEL, (int) thread);

#ifdef C_CONTEXT_SWITCH
  CLEAR_KERNEL_FLAG;
#endif

  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_setcancelstate - set interruptablility state for thread cancellation
 */
int pthread_setcancelstate(state, oldstate)
     int state, *oldstate;
{
  int old;
  sigset_t new;

  if (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE)
    return(EINVAL);
  
  SIM_SYSCALL(TRUE);
  old = (sigismember(&mac_pthread_self()->mask, SIGCANCEL) ?
         PTHREAD_CANCEL_DISABLE : PTHREAD_CANCEL_ENABLE);
  sigemptyset(&new);
  sigaddset(&new, SIGCANCEL);
  if (sigprocmask(state, &new, (sigset_t *) NULL) == 0) {
    if (oldstate)
      *oldstate = old;
    return(0);
  }
  else
    return(get_errno());
}

/*------------------------------------------------------------*/
/*
 * pthread_setcanceltype - set interruptablility type for thread cancellation
 */
int pthread_setcanceltype(type, oldtype)
     int type, *oldtype;
{
  register pthread_t p = mac_pthread_self();
  int old;
  
  old = (p->state & T_CONTROLLED ?
         PTHREAD_CANCEL_DEFERRED : PTHREAD_CANCEL_ASYNCHRONOUS);
  switch (type) {
  case PTHREAD_CANCEL_DEFERRED:
    SET_KERNEL_FLAG;
    p->state |= T_CONTROLLED;
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
    if (oldtype)
      *oldtype = old;
    return(0);
  case PTHREAD_CANCEL_ASYNCHRONOUS:
    SET_KERNEL_FLAG;
    p->state &= ~T_CONTROLLED;
    if (sigismember(&p->pending, SIGCANCEL) &&
        !sigismember(&p->mask, SIGCANCEL)) {
      p->state |= T_INTR_POINT;
      SIG_CLEAR_KERNEL_FLAG(TRUE);
    }
    else {
      SIM_SYSCALL(TRUE);
      CLEAR_KERNEL_FLAG;
    }
    if (oldtype)
      *oldtype = old;
    return(0);
  default:
    return(EINVAL);
  }
}

/*------------------------------------------------------------*/
/*
 * pthread_testcancel - act upon pending cancellation (creates interruption point)
 */
void pthread_testcancel()
{
  register pthread_t p = mac_pthread_self();

  SET_KERNEL_FLAG;
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL)) {
    p->state |= T_INTR_POINT;
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  }
  else {
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }
}

#ifdef GNAT
int segv = FALSE;
#endif /* GNAT */

/*------------------------------------------------------------*/
/*
 * sigaction - install interrupt handler for a thread on a signal
 * return 0 if o.k., -1 otherwise
 * Notice: cannot mask SIGKILL, SIGSTOP, SIGCANCEL
 */
#if defined(SCO5)
int _sigaction(sig, act, oact)
#else
int sigaction(sig, act, oact)
#endif
int sig;
SIGACTION_CONST struct sigaction *act;
struct sigaction *oact;
{
  register pthread_t p = mac_pthread_self();
  struct sigaction vec;

#if defined(SOLARIS) && defined(IO)
  /*
   * Handling for asynchronous process level IO under Solaris.
   * Under certain conditions, call sigaction() in libaio.so.
   * The aio library seems to contain some bugs. When stdin and stdout
   * are pending, the stdin seems to lock stdout at some point, until
   * it gets more input. This may be a result of mutually exclusive file
   * access under Solaris through mutexes. As a result, the Pthreads library
   * can no longer guarantee that the process continues to execute while
   * there is a runnable thread. It tries to do its best until Solaris
   * gets the upper hand and blocks an otherwise runnable LWP performing IO.
   * This problem may not occur when the accessed devices are distict.
   * (Notice that stdin requires an echo, this accessing stdout. Thus, the
   * two devices are not necessarily distinct.)
   * This problem does not occur under SunOS 4.1.x.
   */
  void *handle;
  extern int pthread_started;
  int (*clib_sigaction)
    _C_PROTOTYPE((int __sig,
                  const struct sigaction *__act,
                  struct sigaction *__oact));

  if (!pthread_started && sig == SIGIO || /* called from pthread_init_signals */
      pthread_get_retaddr() < 0) {        /* call from dynamic library */
    handle = dlopen("libaio.so", RTLD_LAZY);
    clib_sigaction = (int (*) _C_PROTOTYPE((int __sig,
                                            const struct sigaction *__act,
                                            struct sigaction *__oact)))
      dlsym(handle, "sigaction");
    return((*clib_sigaction)(sig, act, oact));
  }
#endif /* SOLARIS && IO */

#ifdef GNAT
  /*
   * kludge to prevent SIGSEGV handler installation by GNAT
   */
  if (sig == SIGSEGV)
    if (!segv)
      segv = TRUE;
    else
      return;
#endif /* GNAT */

  if (sig == SIGPROF)
    return(SIGACTION(sig, act, oact));

  if ((sigismember(&cantmask, sig) && act) || sig < 1 || sig >= NSIG) {
    set_errno(EINVAL);
    return(-1);
  }

  if (!act) {
    if (oact)
      *oact = pthread_user_handler[sig];
    return(0);
  }

  if (pthread_sigismemberset2set(&act->sa_mask, &cantmask)) {
    set_errno(EINVAL);
    return(-1);
  }

  SET_KERNEL_FLAG;
  if (oact)
    *oact = pthread_user_handler[sig];

  pthread_user_handler[sig] = *act;

  /*
   * queue up mac_pthread_self() in the signal queue indicated
   */
  if (!sigismember(&handlerset, sig))
    sigaddset(&handlerset, sig);

  /*
   * dequeue pending signals on process and threads if to be ignored
   * or perform default action on process if default action chosen
   */
  if (act->sa_handler == SIG_IGN || act->sa_handler == SIG_DFL) {
    if (sigismember(&pending_signals, sig)) {
      sigdelset(&pending_signals, sig);
      if (act->sa_handler == SIG_DFL)
        default_action(sig);
    }

    for (p = all.head; p; p = p->next[ALL_QUEUE])
      if (sigismember(&p->pending, sig)) {
        sigdelset(&p->pending, sig);
        if (act->sa_handler == SIG_DFL)
          default_action(sig);
      }
  }

#ifndef STAND_ALONE
  /*
   * let UNIX know about sa_flags by reinstalling process handler
   * for signals that have a defined sa_flags bit
   */
  if (sig == SIGCHLD) {
    vec.sa_handler = act->sa_handler;
    vec.sa_mask = act->sa_mask;
    vec.sa_flags = 
#ifdef SA_NOCLDSTOP
      (act->sa_flags & SA_NOCLDSTOP ? SA_NOCLDSTOP : 0);
#else
      0;
#endif
#ifdef __linux__
    vec.sa_restorer = NULL;
#endif /* __linux__ */
    SIGACTION(sig, &vec, (struct sigaction *) NULL);
  }
#endif /* !STAND_ALONE */

#ifdef __dos__
  signal (sig, act->sa_handler);
#else
  SIM_SYSCALL(TRUE);
#endif
  CLEAR_KERNEL_FLAG;

  return(0);
}

#ifdef SCO5
int sigaction(sig, act, oact)
int sig;
SIGACTION_CONST struct sigaction *act;
struct sigaction *oact;

{
   return _sigaction(sig, act, oact);
}
#endif

#ifdef SCO5
int _sigrelse(sig)
int sig;

{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig);
    return sigprocmask(SIG_UNBLOCK,&mask,NULL);
}

int _sighold(sig)
int sig;

{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig);
    return sigprocmask(SIG_BLOCK,&mask,NULL);
}
#endif

#ifdef _M_UNIX			/* Added by monty for SCO 3.2V4.2 */
pthread_sighandler_t sigset(sig,handler)
     int sig;
     pthread_sighandler_t handler;
{
    return signal(sig, handler);
}
#endif

#ifndef __dos__
/*------------------------------------------------------------*/
/*
 * signal - install signal handler
 */
pthread_sighandler_t signal(sig, handler)
     int sig;
     pthread_sighandler_t handler;
{
  struct sigaction act;

  act.sa_handler = handler;
#if defined(SOLARIS) || defined(__dos__) || defined(__USE_POSIX)
  sigemptyset(&act.sa_mask);
#else /* !SOLARIS || !__dos__ */
  act.sa_mask = 0;
#endif /* !SOLARIS */
#if defined(__linux__) || defined(__FreeBSD__) || defined(_M_UNIX)
  act.sa_flags = SA_ONESHOT | SA_NOMASK;
#endif
#ifdef __dos__
  act.sa_flags = 0;
#endif
#ifdef __linux__ 
  act.sa_restorer = NULL;
#endif /* __linux__ || __FreeBSD__ || _M_UNIX */
  if (!sigaction(sig, &act, (struct sigaction *) NULL))
    return(handler);
  else
    return((pthread_sighandler_t)-get_errno());
}
#endif

/*------------------------------------------------------------*/
/*
 * pthread_absnanosleep - suspend until abs. time interval specified by "rqtp"
 */
int pthread_absnanosleep(rqtp)
const struct timespec *rqtp;
{
  register pthread_t p = mac_pthread_self();
  struct timespec rmtp;
#ifdef STAND_ALONE
  struct timespec now;
#else
  timer_ent_t tmr;
  struct timeval now;
#endif
 
  rmtp.ts_sec = rqtp->ts_sec;
  rmtp.ts_nsec = rqtp->ts_nsec;

  do {
    SET_KERNEL_FLAG;
    
    if (pthread_timed_sigwait(p, &rmtp, ABS_TIME, NULL, NULL) == -1) {
      CLEAR_KERNEL_FLAG;
      return(-1);
    }

    /*
     * clear error number before suspending
     */
    set_errno(0);
    
    p->state &= ~T_RUNNING;
    p->state |= T_BLOCKED | T_SYNCTIMER | T_INTR_POINT;
    if (sigismember(&p->pending, SIGCANCEL) &&
        !sigismember(&p->mask, SIGCANCEL))
      SIG_CLEAR_KERNEL_FLAG(TRUE);
    else {
      pthread_q_deq_head(&ready, PRIMARY_QUEUE);
      SIM_SYSCALL(TRUE);
      CLEAR_KERNEL_FLAG;
    }
    
    /*
     * Check if condition was signaled or time-out was exceeded.
     */
    if (get_errno() == EINTR) {
#ifdef STAND_ALONE
      clock_gettime(CLOCK_REALTIME, &now);
      if (pthread_timer = p)
        if (GT_NTIME(p->tp, now))
          MINUS_NTIME(rmtp, p->tp, now);
#else /* !STAND_ALONE */
      if (gettimeofday(&now, (struct timezone *) NULL))
        return(EINVAL);

#ifdef DEF_RR
      if (tmr = pthread_timer.head) {
        while (tmr->thread != p)
          tmr = tmr->next[TIMER_QUEUE];
#else
      if (tmr = p) {
#endif
    
        if (GT_TIME(tmr->tp, now)) {
          MINUS_TIME(now, tmr->tp, now);
          U2P_TIME((rmtp), now);
        }
      }
#endif /* !STAND_ALONE */
    }
    else
      NTIMERCLEAR(rmtp);
  } while (!LE0_NTIME(rmtp));

  return(0);
}

/*------------------------------------------------------------*/
/*
 * nanosleep - suspend until time interval specified by "rqtp" has passed
 * or a user handler is invoked or the thread is canceled
 */
int nanosleep(rqtp, rmtp)
const struct timespec *rqtp;
struct timespec *rmtp;
{
  register pthread_t p = mac_pthread_self();
#ifdef STAND_ALONE
  struct timespec now;
#else
  timer_ent_t tmr;
  struct timeval now;
#endif
 
  if (rmtp)
    rmtp->ts_sec = rmtp->ts_nsec = 0;

  SET_KERNEL_FLAG;

  if (pthread_timed_sigwait(p, rqtp, REL_TIME, NULL, NULL) != 0) {
    CLEAR_KERNEL_FLAG;
    return(-1);
  }

  /*
   * clear error number before suspending
   */
  set_errno(0);

  p->state &= ~T_RUNNING;
  p->state |= T_BLOCKED | T_SYNCTIMER | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  /*
   * Check if condition was signaled or time-out was exceeded.
   */
  if (get_errno() == EINTR) {
#ifdef STAND_ALONE
    clock_gettime(CLOCK_REALTIME, &now);
    if (pthread_timer = p)
      if (GT_NTIME(p->tp, now)) {
        if (rmtp)
          MINUS_NTIME(*rmtp, p->tp, now);
        return(-1);
      }
#else /* !STAND_ALONE */
    if (gettimeofday(&now, (struct timezone *) NULL)) {
      set_errno(EINVAL);
      return(-1);
    }

#ifdef DEF_RR
    if (tmr = pthread_timer.head) {
      while (tmr->thread != p)
        tmr = tmr->next[TIMER_QUEUE];
#else
    if (tmr = p) {
#endif
      if (GT_TIME(tmr->tp, now)) {
        if (rmtp) {
          MINUS_TIME(now, tmr->tp, now);
          U2P_TIME((*rmtp), now);
        }
        return(-1);
      }
    }
#endif /* STAND_ALONE */
  }

  return(0);
}

/*------------------------------------------------------------*/
/*
 * sleep - suspend thread for time interval (in seconds)
 */
unsigned int sleep(seconds)
unsigned int seconds;
{
  struct timespec rqtp, rmtp;

  if (rqtp.ts_sec = seconds) {
    rqtp.ts_nsec = 0;
    nanosleep(&rqtp, &rmtp);
    if (get_errno() == EINTR)
      return(rmtp.ts_sec + (rmtp.ts_nsec ? 1 : 0)); /* pessimistic round-up */
  }
  return(0);
}

#ifdef STAND_ALONE

/*------------------------------------------------------------*/
/*
 * clock_gettime - reads the clock
 */
int clock_gettime(clock_id, tp)
     int clock_id;
     struct timespec *tp;
{
  time_t secs;
  unsigned int nsecs;

  if (clock_id != CLOCK_REALTIME || !tp) {
    set_errno(EINVAL);
    return(-1);
  }
  do {
    do {
      tp->ts_sec =  timeofday.ts_sec;
      tp->ts_nsec =  timeofday.ts_nsec;
      secs = timeofday.ts_sec;
    } while (tp->ts_sec != secs);
    nsecs = (COUNTER->counter14 & CTR_USEC_MASK) >> CTR_USEC_SHIFT;
  } while (tp->ts_nsec != timeofday.tv_nsec);
  tp->ts_nsec += nsecs * 1000;
  return(0);
}

#else /* !STAND_ALONE */

/*------------------------------------------------------------*/
/*
 * clock_gettime - reads the clock
 */
int clock_gettime(clock_id, tp)
int clock_id;
struct timespec *tp;
{
  struct timeval now;

  if (clock_id != CLOCK_REALTIME || !tp ||
      gettimeofday(&now, (struct timezone *) NULL)) {
    set_errno(EINVAL);
    return(-1);
  }

  U2P_TIME((*tp), now);
  return(0);
}

#endif /* STAND_ALONE */

#if defined(IO) && !defined(__FreeBSD__) && !defined(__dos__)

#if !defined(USE_POLL)
/*-------------------------------------------------------------*/
/*
 * fds_update - update fields of file descriptors
 */
static int fds_update(lrfds, lwfds, lefds, grfds, gwfds, gefds, width)
     fd_set *lrfds, *lwfds, *lefds, *grfds, *gwfds, *gefds;
     int width;
{
  int i;
  register int result = 0;
#if !defined(_M_UNIX)
  fd_set crfds, cwfds, cefds, gfds;

  FD_ZERO(&crfds);
  FD_ZERO(&cwfds);
  FD_ZERO(&cefds);

  for (i=0; i < howmany(width, NFDBITS); i++){
    unsigned long mask;
    __FDS_BITS(&crfds)[i] = __FDS_BITS(lrfds)[i] & __FDS_BITS(grfds)[i];
    __FDS_BITS(&cwfds)[i] = __FDS_BITS(lwfds)[i] & __FDS_BITS(gwfds)[i];
    __FDS_BITS(&cefds)[i] = __FDS_BITS(lefds)[i] & __FDS_BITS(gefds)[i];
    mask = __FDS_BITS(&crfds)[i] | __FDS_BITS(&cwfds)[i] |
      __FDS_BITS(&cefds)[i];
    while (mask) {				/* Count number of bits set */
      result++;
      mask &= mask - 1;
    }
  }

  if (result > 0) {
    *lrfds = crfds;
    *lwfds = cwfds;
    *lefds = cefds;
  }
#else
  fd_set crfds, cwfds, cefds, gfds;

  for (i=0; i < howmany(width, NFDBITS); i++){
    unsigned long mask;
    __FDS_BITS(&crfds)[i] = __FDS_BITS(lrfds)[i] & __FDS_BITS(grfds)[i];
    __FDS_BITS(&cwfds)[i] = __FDS_BITS(lwfds)[i] & __FDS_BITS(gwfds)[i];
    __FDS_BITS(&cefds)[i] = __FDS_BITS(lefds)[i] & __FDS_BITS(gefds)[i];
    mask = __FDS_BITS(&crfds)[i] | __FDS_BITS(&cwfds)[i] |
      __FDS_BITS(&cefds)[i];
    while (mask) {
      result++;
      mask &= mask - 1;
    }
  }

  if (result > 0) {
    for (i=0; i<howmany(width, NFDBITS); i++) {
      __FDS_BITS(lrfds)[i] = crfds.__FDS_BITS[i];
      __FDS_BITS(grfds)[i] ^= __FDS_BITS(lrfds)[i];
      __FDS_BITS(lwfds)[i] = __FDS_BITS(&cwfds)[i];
      __FDS_BITS(gwfds)[i] ^= __FDS_BITS(lwfds)[i];
      __FDS_BITS(lefds)[i] = __FDS_BITS(&cefds)[i];
      __FDS_BITS(gefds)[i] ^= __FDS_BITS(lefds)[i];
    }
  }
#endif
  return(result);
}

/*-------------------------------------------------------------*/
/*
 * fds_subtract - delete fields of file descriptors
 */
static void fds_subtract(l, r, width)
     fd_set *l,*r;
     int width;
{
  int i;

  for (i=0; i < howmany(width, NFDBITS); i++) 
    __FDS_BITS(l)[i] = __FDS_BITS(l)[i] & ~__FDS_BITS(r)[i];
}

#else

/*-------------------------------------------------------------*/
/*
 * fds_update - update fields of file descriptors
 */
static int fds_update(nfds, fds, cnfds, cfds)
     int nfds;
     struct pollfd* fds;
     int cnfds;
     struct pollfd* cfds;
{
  register int i, j;
  int result = 0;
  
  for (i=0; i < nfds; i++)
   {
      fds[i].revents = 0;
      for (j=0; j<cnfds; j++) 
       {
          if (cfds[j].revents!=0 && cfds[j].fd==fds[i].fd)
           {
             fds[i].revents = ((fds[i].events|POLLERR|POLLHUP|POLLNVAL)&cfds[j].revents);
             if (fds[i].revents!=0)
              {
#ifdef NOT_NEEDED
                cfds[j].revents ^= fds[i].revents;
#endif
                result++;
              }
             break;
           }
       }
   }
 
  return(result);
}

#endif /* !USE_POLL */

#if defined(_M_UNIX) && !defined(USE_POLL)

/*-------------------------------------------------------------*/
/*
 * bad_fds - test sets with a bad file descriptor
 */
static int bad_fds(lrfds, lwfds, lefds, width)
     fd_set *lrfds, *lwfds, *lefds;
     int width;
{
  register int i;
  struct stat s;

  for (i=0; i < width; i++){
    if ((FD_ISSET(i, lrfds) || FD_ISSET(i, lwfds) || FD_ISSET(i, lefds)) &&
        fstat(i, &s) == -1)
       return -1;
  }

  return(0);
}

/*-------------------------------------------------------------*/
/*
 * aio_badf - handle bad file descriptor error in aio_select
 * Update g???? with pending threads.
 * Wakeup threads with EBADF error.
 * Return -1
 */
static int aio_badf()
{
  pthread_t p;
  int flag;

  /* We recalculate g???? because a descriptor can be closed for other
   * thread due to synchronous TIMER or any other event. Or two threads
   * select on same file, this is not prohibited by current implementation.
   */ 
  gwidth = 0;
  for(p = all.head; p; p = p->next[ALL_QUEUE]) {
    if (sigismember(&p->sigwaitset, AIO_SIG))
      if (p->wait_on_select) {
        p->how_many = bad_fds(&p->readfds, &p->writefds, &p->exceptfds, p->width);
        if (p->how_many == -1) {
#ifdef DEBUG
          kprintf("wake up select\n");
#endif
          p->wait_on_select = FALSE;
          sigdelset(&p->sigwaitset, AIO_SIG);
          if (p->state & T_SYNCTIMER)
            pthread_cancel_timed_sigwait(p, FALSE, SYNC_TIME, TRUE);
          else
            pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
          p->errno = EBADF;
          if (p == mac_pthread_self())
            set_errno(EBADF);
        }
        else if (gwidth == 0) {
          gwidth = p->width;
          pthread_fds_set(&greadfds, &p->readfds, gwidth);
          pthread_fds_set(&gwritefds, &p->writefds, gwidth);
          pthread_fds_set(&gexceptfds, &p->exceptfds, gwidth);
        }
	else {
          gwidth = MAX(gwidth, p->width);
          pthread_fds_union(&greadfds, &p->readfds, p->width);
          pthread_fds_union(&gwritefds, &p->writefds, p->width);
          pthread_fds_union(&gexceptfds, &p->exceptfds, p->width);
	}
      }
      else
	{
          pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
          sigdelset(&p->sigwaitset, AIO_SIG);
      }
  }

  return (-1);
}

/*-------------------------------------------------------------*/
/*
 * aio_select - handler for synchronous I/O on process level,
 * demultiplexes select events for threads.
 * Update g???? with pending threads.
 * Return number of wake up threads.
 */
static int aio_select(struct timeval* timeout)
{
  pthread_t p;
  int count = 0;
  int cwidth;
  fd_set creadfds, cwritefds, cexceptfds;
  int flag;

  if (gwidth == 0)
     return 0;

  gtimeout = *timeout;
  SIGPROCMASK(SIG_UNBLOCK, &all_signals, (struct sigset_t *) NULL);
  pthread_fds_set(&creadfds, &greadfds, gwidth);
  pthread_fds_set(&cwritefds, &gwritefds, gwidth);
  pthread_fds_set(&cexceptfds, &gexceptfds, gwidth);
  /* No problem with wait indefinity in SCO, because SCO set errno to
   * ERESTART in case of interrupts.
   */
  if ((cwidth = SELECT(gwidth, &creadfds, &cwritefds, &cexceptfds, &gtimeout)) <= 0) {
     /* Here we must handle the case when errno different to EINTR, EAGAIN,
      * or ERESTART. Can exist a kernel deadlock with EBADF by example.
      */
     switch(errno) {
     case EBADF:
          return aio_badf();

     case EINTR:
#ifdef ERESTART
     case ERESTART:
#endif
          return -1;

     default:
          return -1;
     }
  }

  /* We recalculate g???? because a descriptor can be closed for other
   * thread due to synchronous TIMER or any other event. Or two threads
   * select on same file, this is not prohibited by current implementation.
   */ 
  gwidth = 0;
  for(p = all.head; p; p = p->next[ALL_QUEUE]) {
    if (sigismember(&p->sigwaitset, AIO_SIG))
      if (p->wait_on_select) {
	if (cwidth>0)
           p->how_many = fds_update(&p->readfds, &p->writefds, &p->exceptfds,
                                 &creadfds, &cwritefds, &cexceptfds, p->width);
        else
           p->how_many = 0;
        if (p->how_many > 0) {
#ifdef DEBUG
          kprintf("wake up select\n");
#endif
          count++;
          p->wait_on_select = FALSE;
          sigdelset(&p->sigwaitset, AIO_SIG);
          if (p->state & T_SYNCTIMER)
            pthread_cancel_timed_sigwait(p, FALSE, SYNC_TIME, TRUE);
          else
            pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
          cwidth -= p->how_many;
        }
        else if (gwidth == 0) {
          gwidth = p->width;
          pthread_fds_set(&greadfds, &p->readfds, gwidth);
          pthread_fds_set(&gwritefds, &p->writefds, gwidth);
          pthread_fds_set(&gexceptfds, &p->exceptfds, gwidth);
        }
	else {
          gwidth = MAX(gwidth, p->width);
          pthread_fds_union(&greadfds, &p->readfds, p->width);
          pthread_fds_union(&gwritefds, &p->writefds, p->width);
          pthread_fds_union(&gexceptfds, &p->exceptfds, p->width);
	}
      }
      else
	{
          count++;
          pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
          sigdelset(&p->sigwaitset, AIO_SIG);
      }
  }

  return (count);
}

/*-------------------------------------------------------------*/
/*
 * aio_poll - handler for synchronous I/O on process level,
 * demultiplexes select events for threads
 * make a poll only not wait.
 */
int aio_poll()

{
   struct timeval timeout = {0, 0};
   return aio_select(&timeout);
}

/*
 * aio_wait - handler for synchronous I/O on process level,
 * demultiplexes select events for threads
 * make a wait.
 */
static int aio_wait()

{
   struct timeval timeout = {86400, 0};
   return aio_select(&timeout);
}

#endif /* _M_UNIX && !USE_POLL */

#if defined(_M_UNIX) && defined(USE_POLL)

/*------------------------------------------------------------*/
/*
 * gpoll_fds_union - 
 */
void gpoll_fds_union(nfds, fds)
     int nfds;
     struct pollfd fds[];
{
  register int i, j;

  for (i = 0; i < nfds; i++)
   {
     for (j=0; j< gnfds; j++)
      {
        if (gfds[j].fd==fds[i].fd)
           {
             gfds[j].events |= fds[i].events;
             goto next;
           }
      }
      gfds[gnfds].fd = fds[i].fd;
      gfds[gnfds].events = fds[i].events;
      gnfds++;
next:
   }
}

/*-------------------------------------------------------------*/
/*
 * aio_calculate - handler for synchronous I/O on process level,
 * demultiplexes select events for threads.
 * Update g???? with pending threads.
 * Return number of wake up threads.
 */
static int aio_calculate()
{
  register pthread_t p;
  register int count = 0;

  /* We recalculate g???? because a descriptor can be closed for other
   * thread due to synchronous TIMER or any other event. Or two threads
   * select on same file, this is not prohibited by current implementation.
   */ 
  gnfds = 0;
  for(p = all.head; p; p = p->next[ALL_QUEUE]) {
    if (sigismember(&p->sigwaitset, AIO_SIG))
      if (p->wait_on_select) {
        if (gnfds == 0) {
          gnfds = p->nfds;
	  memcpy(gfds, p->fds, p->nfds * sizeof(struct pollfd));
        }
	else {
          gpoll_fds_union(p->nfds, p->fds);
	}
      }
     else {
        sigdelset(&p->sigwaitset, AIO_SIG);
        count++;
      }
  }

  return (count);
}

/*-------------------------------------------------------------*/
/*
 * aio_select - handler for synchronous I/O on process level,
 * demultiplexes select events for threads.
 * Update g???? with pending threads.
 * Return number of wake up threads.
 */
static int aio_select(int timeout, int unblock)
{
  register pthread_t p = mac_pthread_self();
  int cnfds;
  int i, count = 0;
  struct pollfd* cfds;
  static int aio_calculate();

  if (gnfds == 0)
   {
#if defined(IO_DEBUG)
     kprintf("gnfds is zero!!!\n", gnfds);
#endif
     return 0;
   }

  gtimeout = timeout;

  if (unblock)
     SIGPROCMASK(SIG_UNBLOCK, &all_signals, (struct sigset_t *) NULL);

  /* No problem with wait indefinity in SCO, because SCO set errno to
   * ERESTART in case of interrupts.
   */
#if defined(DEBUG)
  kprintf("gnfds=%d gtimeout=%d\n", gnfds, gtimeout);
#endif
  switch ((i=POLL(gfds, gnfds, gtimeout))) {
  case 0:
     return 0;     
  case -1:
     switch(errno)
     {
     case EINTR:
#ifdef ERESTART
     case ERESTART:
#endif
#if defined(DEBUG)
          kprintf("POLL: errno=%d\n", errno);
#endif
          return 0;
     case EINVAL:
     case EFAULT:
     case EAGAIN:
     default:
#if defined(DEBUG)
          kprintf("POLL: errno=%d\n", errno);
#endif
	  return 0;
     }
  default:
     break;
  }

  /* We recalculate g???? because a descriptor can be closed for other
   * thread due to synchronous TIMER or any other event. Or two threads
   * select on same file, this is not prohibited by current implementation.
   */ 
  cnfds = gnfds;
  cfds = gfds;
  gnfds = 0;
  if (gfds == gafds[0])
   gfds = gafds[1];
  else
   gfds = gafds[0];
  for(p = all.head; p; p = p->next[ALL_QUEUE]) {
    if (sigismember(&p->sigwaitset, AIO_SIG))
      if (p->wait_on_select)
       {
        p->how_many = fds_update(p->nfds, p->fds, cnfds, cfds);
        if (p->how_many > 0)
         {
#ifdef DEBUG
           kprintf("wake up select\n");
#endif
           p->wait_on_select = FALSE;
           sigdelset(&p->sigwaitset, AIO_SIG);
           if (p->state & T_SYNCTIMER)
             pthread_cancel_timed_sigwait(p, FALSE, SYNC_TIME, TRUE);
           else
             pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
	   count++;
         }
       }
      else
       {
         p->how_many = 0;
         sigdelset(&p->sigwaitset, AIO_SIG);
         if (p->state & T_SYNCTIMER)
          pthread_cancel_timed_sigwait(p, FALSE, SYNC_TIME, TRUE);
         else
          pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
	 count++;
       }
  }

  aio_calculate();
  return (count);
}

/*-------------------------------------------------------------*/
/*
 * aio_poll - handler for synchronous I/O on process level,
 * demultiplexes select events for threads
 * make a poll only not wait.
 */
static int aio_poll()

{
   return aio_select(0, FALSE);
}

/*
 * aio_wait - handler for synchronous I/O on process level,
 * demultiplexes select events for threads
 * make a wait.
 */
static int aio_wait()

{
   return aio_select(3600000, TRUE);
}

#endif /* _M_UNIX && USE_POLL */

#if !defined(USE_POLL)
/*-------------------------------------------------------------*/
/*
 * aio_handle - handler for asynchronous I/O on process level,
 * demultiplexes SIGIO for threads
 */
static int aio_handle()
{
  pthread_t p;
  int count = 0;

#ifndef SVR4
  static struct timeval mintimeout = {0, 0};
  fd_set creadfds, cwritefds, cexceptfds;
  int cwidth = gwidth;
  int flag;

  if (gwidth > 0) {
    pthread_fds_set(&creadfds, &greadfds, gwidth);
    pthread_fds_set(&cwritefds, &gwritefds, gwidth);
    pthread_fds_set(&cexceptfds, &gexceptfds, gwidth);
    if (SELECT(gwidth, &creadfds, &cwritefds, &cexceptfds, &mintimeout) > 0) {
#if !defined(_M_UNIX)
      fds_subtract(&greadfds, &creadfds, gwidth);
      fds_subtract(&gwritefds, &cwritefds, gwidth);
      fds_subtract(&gexceptfds, &cexceptfds, gwidth);
      while (gwidth != 0 && 
             !FD_ISSET(gwidth-1, &greadfds) &&
             !FD_ISSET(gwidth-1, &gwritefds) &&
             !FD_ISSET(gwidth-1, &gexceptfds))
        gwidth--;
#endif
    }
#if 0
    else {
       /* Here, we must to be handling errors as:
        * EBADF, EINTR, EINVAL
        */
    }
#endif
  }
#endif /* !SVR4 */
  
#if defined(_M_UNIX)
  /* We want to recalculate g?????, because spurious select, or handle being
   * used for other thread prevent wakeup the news threads. It is an error
   * programming, but aha, robustness is robustness.
   */
  gwidth = 0;
#endif 

  for(p = all.head; p; p = p->next[ALL_QUEUE]) {
    if (p->state & T_IO_OVER) {
      p->state &= ~T_IO_OVER;
      count++;
      continue;
    }
    else if (sigismember(&p->sigwaitset, AIO_SIG)) {
#ifndef SVR4
      p->how_many = fds_update(&p->readfds, &p->writefds, &p->exceptfds,
			       &creadfds, &cwritefds, &cexceptfds, p->width);
      if (cwidth > 0 && p->wait_on_select) {
	if (p->how_many > 0) {
#ifdef DEBUG
          fprintf(stderr, "wake up select\n");
#endif
          count++;
          pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
          sigdelset(&p->sigwaitset, AIO_SIG);
          p->wait_on_select = FALSE;
          if (p->state & T_SYNCTIMER)
            pthread_cancel_timed_sigwait(p, FALSE, SYNC_TIME, TRUE);
        }
#if defined(_M_UNIX)
        /* A litte optimization, if two threads waiting the second with not
         * event will be the next to handle.
         */
        else if (gwidth == 0) {
          pthread_fds_set(&greadfds, &p->readfds, gwidth);
          pthread_fds_set(&gwritefds, &p->writefds, gwidth);
          pthread_fds_set(&gexceptfds, &p->exceptfds, gwidth);
        }
	else {
          gwidth = MAX(gwidth, p->width);
          pthread_fds_union(&greadfds, &p->readfds, p->width);
          pthread_fds_union(&gwritefds, &p->writefds, p->width);
          pthread_fds_union(&gexceptfds, &p->exceptfds, p->width);
	}
#endif
      }
#if defined(_M_UNIX)
      else if (!sigismember(&p->sigwaitset, AIO_SIG))
#else
      else
#endif
#endif /* !SVR4 */
	{
#if !defined(__linux__) && !defined(_M_UNIX)
        if (p->resultp.aio_return != AIO_INPROGRESS && p->width == 0 ||
	    p->how_many > 0 && p->width != 0) {
#else
	if (p->how_many > 0)
#endif /* !__linux__ && _M_UNIX */
          count++;
          pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
          sigdelset(&p->sigwaitset, AIO_SIG);
#if !defined(__linux__) && !defined(_M_UNIX)
        }
#endif /* !__linux__ && !_M_UNIX */
      }
    }
  }

  if (count)
    return(TRUE);
  else
    return(FALSE);
}
#endif /* !USE_POLL */

#endif /* IO && !__FreeBSD__ && !__dos__ */
/*-------------------------------------------------------------*/

#if defined(STAND_ALONE) || defined(SCO5)
/*------------------------------------------------------------*/
/*
 * alarm - Deliver SIGALARM after "seconds"
 */
unsigned int alarm(seconds)
     unsigned int seconds;
{
  set_errno(ENOSYS);
  return(-1);
}
#endif /* STAND_ALONE || SCO5 */

#if !defined(__linux__) && !defined(__dos__)
/*------------------------------------------------------------*/
/*
 * pthread_setsigcontext_np - modify the signal context to return to a setjmp()
 * call location, i.e. simulate a longjmp() but leave the signal handler
 * properly.
 */
void pthread_setsigcontext_np(scp, env_decl, val)
     struct context_t *scp;
     jmp_buf env_decl;
     int val;
{
  scp->sc_pc = env_use[JB_PC];
#if !defined (__FreeBSD__) && !defined (_M_UNIX) && !defined(__linux__)
  scp->sc_npc = env_use[JB_PC] + 4;
#endif
#if defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH)
  scp->sc_pc += RETURN_OFFSET;
#if !defined (__FreeBSD__) && !defined (_M_UNIX) && !defined(__linux__)
  scp->sc_npc += RETURN_OFFSET;
#endif
#endif /* defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH) */
  scp->sc_sp = env_use[JB_SP];
  if (env_use[JB_SVMASK])
    pthread_sigcpyset2set(&scp->sc_mask, &env_use[JB_MASK]); /* copy sigmasks */
#if !defined (__FreeBSD__) && !defined (_M_UNIX) && !defined(__linux__)
  scp->sc_o0 = val; 
#endif
}
#endif /* !__linux__ */

/*------------------------------------------------------------*/
/*
 * SIGPROCMASK - change or examine signal mask of process
 */
int SIGPROCMASK(how, set, oset)
int how;
const sigset_t *set;
sigset_t *oset;
{
#ifdef SVR4
  return(syscall(SYS_sigprocmask, how, set, oset));
#else /* !SVR4 */
  sigset_t old;

#ifndef STAND_ALONE
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  SYS_SIGPROCMASK (how, NULL, &proc_mask);
#else
  proc_mask = sigblock(0);
#endif
#endif /* !STAND_ALONE */

  if (oset)
    pthread_sigcpyset2set(oset, &proc_mask);

  if (!set)
    return(0);

  switch (how) {
  case SIG_BLOCK:
    pthread_sigaddset2set(&proc_mask, set);
    break;
  case SIG_UNBLOCK:
    pthread_sigdelset2set(&proc_mask, set);
    break;
  case SIG_SETMASK:
    pthread_sigcpyset2set(&proc_mask, set);
    break;
  default:
    set_errno(EINVAL);
    return(-1);
  }

  pthread_sigdelset2set(&proc_mask, &cantmask);
#ifndef STAND_ALONE
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  SYS_SIGPROCMASK (SIG_SETMASK, &proc_mask, NULL);
#else
  sigsetmask(proc_mask);
#endif
#endif /* !STAND_ALONE */
  
  return(0);
#endif /* !SVR4 */
}

/*------------------------------------------------------------*/
/*
 * SIGACTION - change or examine signal handlers of process
 */
int SIGACTION(sig, act, oact)
int sig;
const struct sigaction *act;
struct sigaction *oact;
{
#ifdef SVR4
#if defined(SOLARIS) && defined(IO)
  if (sig == SIGIO)
    sigaction(sig, act, oact);
  else
#endif /* SOLARIS && IO */
    return(syscall(SYS_sigaction, sig, act, oact));
#else /* !SVR4 */
#ifndef STAND_ALONE
#if defined(__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
#ifdef TRASH
#ifdef __USE_POSIX
  return(__sigaction(sig, act, oact));
#else
  return(SYS_SIGACTION (sig, act, oact));
#endif /* __USE_POSIX */
#else
  return(SYS_SIGACTION (sig, act, oact));
#endif
#else
  return(sigvec(sig, act, oact)); /* cheating: structs distinct / same layout */
#endif
#else /* !STAND_ALONE */
  set_errno(EINVAL);
  return(-1);
#endif /* !STAND_ALONE */
#endif /* !SVR4 */
}

/*------------------------------------------------------------*/
/*
 * SIGSUSPEND - suspend process waiting for signals
 */
int SIGSUSPEND(set)
sigset_t *set;
{
#ifdef SVR4
  return(syscall(SYS_sigsuspend, set));
#else /* !SVR4 */
#ifndef STAND_ALONE
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  return (SYS_SIGSUSPEND (set));
#else
  return(sigpause(*set));
#endif
#else /* !STAND_ALONE */
  /* busy wait */;
#endif /* !STAND_ALONE */
#endif /* !SVR4 */
}

#if defined(IO) && !defined(__FreeBSD__) && !defined(__dos__)
/*------------------------------------------------------------*/
/*
 * ACCEPT - accept call
 */
int ACCEPT(s, addr, addrlen)
     int s;
     struct sockaddr *addr;
     int *addrlen;
{
#if defined (__FreeBSD__) || defined(__linux__)
  int param[3];

  param[0] = (int) s;
  param[1] = (int) addr;
  param[2] = (int) addrlen;
  return (SYS_SOCKETCALL (SYS_ACCEPT, param));
#elif defined(_M_UNIX)
  struct socksysreq req;

  req.args[0] = SO_ACCEPT;
  req.args[1] = (int)s;
  req.args[2] = (int)addr;
  req.args[3] = (int)addrlen;

  return(ioctl(s, SIOCSOCKSYS, &req));
#elif defined(SOLARIS)
  void *handle;
  int (*clib_accept)
    _C_PROTOTYPE((int __s,
                  struct sockaddr *__addr,
                  int *__addrlen));

  handle = dlopen("libsocket.so", RTLD_LAZY);
  clib_accept = (int (*) _C_PROTOTYPE((int __s,
				       struct sockaddr *__addr,
				       int *__addrlen)))
    dlsym(handle, "accept");
  return((*clib_accept)(s, addr, addrlen));
#else /* !SOLARIS */
  return(syscall(SYS_accept, s, addr, addrlen));
#endif /* !SOLARIS */
}

/*------------------------------------------------------------*/
/*
 * RECVFROM - recvfrom call
 */
int RECVFROM(s, buf, len, flags, from, fromlen)
     int s;
     char *buf;
     int len, flags;
     struct sockaddr *from;
     int *fromlen;
{
#if defined (__FreeBSD__) || defined(__linux__)
  int param[6];

  param[0] = (int) s;
  param[1] = (int) buf;
  param[2] = (int) len;
  param[3] = (int) flags;
  param[4] = (int) from;
  param[5] = (int) fromlen;
  return (SYS_SOCKETCALL (SYS_RECVFROM, param));
#elif defined(_M_UNIX)
  struct socksysreq req;

  req.args[0] = SO_RECVFROM;
  req.args[1] = (int)s;
  req.args[2] = (int)buf;
  req.args[3] = (int)len;
  req.args[4] = (int)flags;
  req.args[5] = (int)from;
  req.args[6] = (int)fromlen;

  return(ioctl(s, SIOCSOCKSYS, &req));
#elif defined(SOLARIS)
  return(syscall(SYS_recvfrom, s, buf, len, flags, from, fromlen));
#else /* !SOLARIS */
  return(syscall(SYS_recvfrom, s, buf, len, flags, from, fromlen));
#endif /* !SOLARIS */
}

/*------------------------------------------------------------*/
/*
 * SENDTO - sendto call
 */
int SENDTO(s, msg, len, flags, to, tolen)
     int s;
     char *msg;
     int len, flags;
     struct sockaddr *to;
     int tolen;
{
#if defined (__FreeBSD__) || defined(__linux__)
  int param[6];

  param[0] = (int) s;
  param[1] = (int) msg;
  param[2] = (int) len;
  param[3] = (int) flags;
  param[4] = (int) to;
  param[5] = (int) tolen;
  return (SYS_SOCKETCALL (SYS_SENDTO, param));
#elif defined(_M_UNIX)
  struct socksysreq req;

  req.args[0] = SO_SENDTO;
  req.args[1] = (int)s;
  req.args[2] = (int)msg;
  req.args[3] = (int)len;
  req.args[4] = (int)flags;
  req.args[5] = (int)to;
  req.args[6] = (int)tolen;

  return(ioctl(s, SIOCSOCKSYS, &req));
#elif defined(SOLARIS)
  return(syscall(SYS_sendto, s, msg, len, flags, to, tolen));
#else /* !SOLARIS */
  return(syscall(SYS_sendto, s, msg, len, flags, to, tolen));
#endif /* !SOLARIS */
}

#ifndef SVR4
/*------------------------------------------------------------*/
/*
 * CONNECT - connect call
 */
int CONNECT(s, addr, addrlen)
     int s;
     const struct sockaddr *addr;
     int addrlen;
{
#if defined (__FreeBSD__) || defined(__linux__)
  int param[3];

  param[0] = (int) s;
  param[1] = (int) addr;
  param[2] = (int) addrlen;
  return (SYS_SOCKETCALL (SYS_CONNECT, param));
#elif defined(_M_UNIX)
  struct socksysreq req;

  req.args[0] = SO_CONNECT;
  req.args[1] = (int)s;
  req.args[2] = (int)addr;
  req.args[3] = (int)addrlen;

  return(ioctl(s, SIOCSOCKSYS, &req));
#elif defined(SOLARIS)
  void *handle;
  int (*clib_connect)
    _C_PROTOTYPE((int __s,
                  const struct sockaddr *__addr,
                  int __addrlen));

  handle = dlopen("libsocket.so", RTLD_LAZY);
  clib_connect = (int (*) _C_PROTOTYPE((int __s,
				       const struct sockaddr *__addr,
				       int __addrlen)))
    dlsym(handle, "connect");
  return((*clib_connect)(s, addr, addrlen));
#else
  return(syscall(SYS_connect, s, addr, addrlen));
#endif
}

/*------------------------------------------------------------*/
/*
 * GETPEERNAME - getpeername call
 */
int GETPEERNAME(s, addr, addrlen)
     int s;
     struct sockaddr *addr;
     int *addrlen;

{
#if defined (__FreeBSD__) || defined(__linux__)
  int param[3];

  param[0] = (int) s;
  param[1] = (int) addr;
  param[2] = (int) addrlen;
  return (SYS_SOCKETCALL (SYS_GETPEERNAME, param));
#elif defined(_M_UNIX)
  struct socksysreq req;

  req.args[0] = SO_GETPEERNAME;
  req.args[1] = (int)s;
  req.args[2] = (int)addr;
  req.args[3] = (int)addrlen;

  return(ioctl(s, SIOCSOCKSYS, &req));
#elif defined(SOLARIS)
  void *handle;
  int (*clib_getpeername)
    _C_PROTOTYPE((int __s,
                  struct sockaddr *__addr,
                  int *__addrlen));

  handle = dlopen("libsocket.so", RTLD_LAZY);
  clib_getpeername = (int (*) _C_PROTOTYPE((int __s,
				       struct sockaddr *__addr,
				       int *__addrlen)))
    dlsym(handle, "getpeername");
  return((*clib_getpeername)(s, addr, addrlen));
#else
  return(syscall(SYS_getpeername, s, addr, addrlen));
#endif
}

/*------------------------------------------------------------*/
/*
 * GETSOCKOPT - getsockopt call
 */
int GETSOCKOPT(s, level, optname, optval, optlen)
     int s;
     int level;
     int optname;
     void* optval;
     int* optlen;

{
#if defined (__FreeBSD__) || defined(__linux__)
  int param[5];

  param[0] = (int) s;
  param[1] = (int) level;
  param[2] = (int) optname;
  param[3] = (int) optval;
  param[4] = (int) optlen;
  return (SYS_SOCKETCALL (SYS_GETSOCKOPT, param));
#elif defined(_M_UNIX)
  struct socksysreq req;

  req.args[0] = SO_GETSOCKOPT;
  req.args[1] = (int)s;
  req.args[2] = (int)level;
  req.args[3] = (int)optname;
  req.args[4] = (int)optval;
  req.args[5] = (int)optlen;

  return(ioctl(s, SIOCSOCKSYS, &req));
#elif defined(SOLARIS)
  void *handle;
  int (*clib_getsockopt)
    _C_PROTOTYPE((int __s,
                  int __level,
                  int __optname,
                  void*__optval,
                  int *__optlen));

  handle = dlopen("libsocket.so", RTLD_LAZY);
  clib_getsockopt = (int (*) _C_PROTOTYPE((int __s,
				       int __level,
				       int *__optname,
					void *__optval,
					int *__optlen)))
    dlsym(handle, "getsockopt");
  return((*clib_getsockopt)(s, level, optname, optval, optlen));
#else
  return(syscall(SYS_getsockopt, s, level, optname, optval, optlen));
#endif
}

/*------------------------------------------------------------*/
/*
 * SELECT - select call
 */
int SELECT(width, readfds, writefds, exceptfds, timeout)
     int width;
     fd_set *readfds, *writefds, *exceptfds;
     struct timeval *timeout;
{
#ifdef __linux__
  return(pthread_select(width, readfds, writefds, exceptfds, timeout));
#elif defined(_M_UNIX)
  return(pthread_sys_select(width, readfds, writefds, exceptfds, timeout));
#else
  return(syscall(SYS_select, width, readfds, writefds, exceptfds, timeout));
#endif
}

#if defined(USE_POLL)
/*------------------------------------------------------------*/
/*
 * POLL - poll call
 */
int POLL(fds, nfds, timeout)
     int nfds;
     struct pollfd fds[];
     int timeout;
{
#if defined(__linux__)
  return(pthread_poll(fds, nfds, timeout));
#elif defined(_M_UNIX)
  return(pthread_sys_poll(fds, nfds, timeout));
#else
  return(syscall(SYS_poll, fds, nfds, timeout));
#endif
}
#endif
#endif /* !SVR4 */
#endif /* IO && !__FreeBSD__ && !__linux__ && !__dos__ */

#if defined(SCO5)
/*------------------------------------------------------------*/
/*
 * WAITPID - waitpid call
 */
pid_t WAITPID(pid_t pid, int *status, int options)
{
   return pthread_sys_waitpid(pid, status, options);
}

/*------------------------------------------------------------*/
/*
 * WAIT3 - wait3 call
 */
pid_t WAIT3(int* status, int options, struct rusage * rusage)

{
#if defined(_M_UNIX)
   errno = ENOSYS;
   return -1;
#endif
}

#endif /* SCO5 */
