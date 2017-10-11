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

  @(#)pthread.c	3.14 11/8/00

*/

/*
 * Implementation of fork, join, exit, etc.
 */

#if defined (__FreeBSD__) || defined (_M_UNIX) || defined (__dos__)
# include<stdlib.h> /* why was this here for __linux__ ? */
#endif

#include "internals.h"
#include "setjmp.h"
#ifdef TDI_SUPPORT
#include "tdi.h"
#include "tdi-aux.h"
#include "tdi-dl.h"
#endif
#include <pthread/asm.h>

#ifdef STAND_ALONE
extern struct pthread tcb[MAX_THREADS];
#endif

volatile int pthread_started = FALSE;
static volatile int n_pthreads = 0;
static volatile pthread_key_t n_keys = 0;
static void (*key_destructor[_POSIX_DATAKEYS_MAX])();

/*------------------------------------------------------------*/
/*
 * pthread_alloc - allocate pthread structure
 */
static pthread_t pthread_alloc(func, arg)
     pthread_func_t func;
     any_t arg;
{
  pthread_t t;
  extern char *pthread_calloc();

#ifdef STAND_ALONE
  int i;
  char *c;

  for (i = 0; i < MAX_THREADS; i++) {
    t = (pthread_t) SA((int) &tcb[i]);
    if (t->state & T_RETURNED && t->state & T_DETACHED) {
      t->state &= ~(T_RETURNED | T_DETACHED);
      c = (char *) &tcb[i];
      for (i = sizeof(*t); i > 0; i--, c++)
	*c = '\0';

      break; 
   }
  }
  if (i >= MAX_THREADS)
    t = NO_PTHREAD;
#else /* !STAND_ALONE */
#ifdef MALLOC
  if (!pthread_started)
    t = (pthread_t) pthread_calloc(1, sizeof(struct pthread));
  else
#endif /* MALLOC */
    t = (pthread_t) calloc(1, sizeof(struct pthread));
#endif /* STAND_ALONE */
  if (t != NO_PTHREAD) {
    t->func = func;
    t->arg = arg;
    t->state = T_RUNNING | T_CONTROLLED;
  }
  return(t);
}

/*------------------------------------------------------------*/
/*
 * pthread_self - returns immutable thread ID of calling thread
 */
pthread_t pthread_self()
{
  return(mac_pthread_self());
}

/*------------------------------------------------------------*/
/*
 * pthread_init - initialize the threads package. This function is 
 * the first function that be called by any program using this package.
 * It initializes the main thread and sets up a stackspace for all the
 * threads to use. Its main purpose is to setup the mutexes and 
 * condition variables and allow one to access them.
 */

#if defined(__FreeBSD__) || defined(_M_UNIX) || defined(__linux__) || defined(__dos__)
/*
 * the `constructor' attribute causes `pthread_init()' to be called 
 * automatically before execution enters `main()'.
 * (For all ports except SunOS and Solaris: see p_aux.S for init hook)
 */
void pthread_init(void) __attribute__ ((constructor));
#endif

void pthread_init()
{
  pthread_t t;
  int i;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  SYS_SIGJMP_BUF env;
#else
  sigjmp_buf env;
#endif
#ifdef MALLOC
/*  extern int __malloc_initialized;*/
#ifdef MALLOC_DEBUG
  pthread_malloc_debug_init();
#endif
#endif;

#ifdef SOLARIS
  /*
   * This dummy reference ensures that the initialization routine
   * is always linked.
   */
  extern void pthread_dummy();
  if (FALSE)
    pthread_dummy();
#endif /* SOLARIS */

  if (pthread_started)
    return;

#ifdef MALLOC
/*  __malloc_initialized = 0;*/
#endif
  pthread_init_signals();

  pthread_mutexattr_init(&pthread_mutexattr_default);
  pthread_condattr_init(&pthread_condattr_default);
  pthread_attr_init(&pthread_attr_default);

  t = pthread_alloc((pthread_func_t) 0, (any_t *) NULL);
  t->state |= T_MAIN | T_CONTROLLED;

  t->num_timers = 0;
  t->interval.tv_sec = 0;
  t->interval.tv_usec = 0;

#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  SYS_SIGSETJMP(env, FALSE, TRUE);
#else
  sigsetjmp(env, FALSE);
#endif

  process_stack_base = (char *) env[JB_SP];
  pthread_stack_init(t);

  pthread_attr_init(&t->attr);
  t->attr.param.sched_priority = MIN_PRIORITY;
#ifdef _POSIX_THREADS_PRIO_PROTECT
  t->base_prio = MIN_PRIORITY;
#ifdef SRP
  t->max_ceiling_prio = NO_PRIO;
  t->new_prio = NO_PRIO;
#endif
#endif
#ifdef DEF_RR
  t->attr.sched = SCHED_RR;
#else
  t->attr.sched = SCHED_FIFO;
#endif
  /*
   * 1st thread inherits signal mask from process
   */
#ifdef STAND_ALONE
  pthread_sigcpyset2set(&t->mask, &proc_mask);
#else
  SIGPROCMASK(SIG_SETMASK, (struct sigaction *) NULL, &t->mask);
#endif
  mac_pthread_self() = t;	
  pthread_q_all_enq(&all, t);
  pthread_q_primary_enq(&ready, t);
  state_change = FALSE;
  pthread_started = TRUE;
#ifdef DEF_RR
  pthread_timed_sigwait(t, NULL, RR_TIME, NULL, t);
#endif
#if defined(IO) && (defined(__FreeBSD__) || defined(_M_UNIX) || defined(__linux__))
  fd_init();
#endif

#ifdef TDI_SUPPORT
  /* ------------------------------------------------------- *
   * TDI:                                                    *
   *      - if we are attached by a debugger, we load the    *
   *        TDI library                                      *
   * ------------------------------------------------------- */

#ifndef TDI_STATIC
  if (__pthread_debug_with_TDI>0)
#endif
    {
      /* load the function code for tdi_server */
      if (pthread_load_TDI_Server("libTDISrv.so","tdi_server") == TDI_LOAD_OK) {
	/* if TDI_Server_Func is assigned, we can ping *
	 * the TDI kernel to register                  */
	if (TDI_Server_Func) {
	  /* let the TDI call our register func */
	  TDI_Server_Func(-1,-2);
	}
      } else {
	printf("TDI-ERROR: libTDISrv.so not found\n");
	fflush(stdout);
      }
    }
  /* ------------------------------------------------------- */
#endif

  n_pthreads = 1; /* main thread counts, fd_server does not count */
#ifdef GNAT
  {
    extern int gnat_argc;
    extern char **gnat_argv;

    dsm_init(gnat_argc, gnat_argv);
  }
#endif /* GNAT */
}

/*------------------------------------------------------------*/
/*
 * pthread_body - base of the pthreads implementation.
 * Procedure invoked at the base of each pthread (as a wrapper).
 */
void pthread_body()
{
  pthread_t t = mac_pthread_self();
#ifdef REAL_TIME
  struct timespec tp;
#endif /* REAL_TIME */
  static void pthread_terminate();
#ifdef DEBUG
#ifdef STAND_ALONE
pthread_timer_q_t pthread_timer;              /* timer queue                 */
#else
pthread_timer_q pthread_timer;                /* timer queue                 */
#endif
#endif /* DEBUG */

#ifdef C_CONTEXT_SWITCH
  CLEAR_KERNEL_FLAG;
#endif

#ifdef REAL_TIME
  NTIMERCLEAR(tp);
  if (ISNTIMERSET(t->attr.starttime)) {
    tp.tv_sec = t->attr.starttime.tv_sec;
    tp.tv_nsec = t->attr.starttime.tv_nsec;
  }

  if (ISNTIMERSET(t->attr.deadline)) {
    SET_KERNEL_FLAG;
    pthread_timed_sigwait(t, &t->attr.deadline, ABS_TIME, pthread_exit, -1);
    CLEAR_KERNEL_FLAG;
  }

  if (ISNTIMERSET(tp))
    pthread_absnanosleep(&tp);

  do {
    if (ISNTIMERSET(tp) && !LE0_NTIME(t->attr.period)) {
      PLUS_NTIME(tp, tp, t->attr.period);
      SET_KERNEL_FLAG;
      pthread_timed_sigwait(t, &tp, ABS_TIME, pthread_exit, -1);
      CLEAR_KERNEL_FLAG;
    }
#ifdef DEBUG
    fprintf(stderr, "body time Q head %x\n", pthread_timer.head);
#endif /* DEBUG */
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
    if (!SYS_SIGSETJMP(t->body, FALSE, TRUE))
#else
    if (!sigsetjmp(t->body, FALSE))
#endif
#endif /* REAL_TIME */

      t->result = (*(t->func))(t->arg);

#ifdef REAL_TIME
    if (ISNTIMERSET(tp) && !LE0_NTIME(t->attr.period)) {
      SET_KERNEL_FLAG;
      pthread_cancel_timed_sigwait(t, FALSE, ALL_TIME, FALSE);
      CLEAR_KERNEL_FLAG;
      pthread_absnanosleep(&tp);
    }
  } while (!LE0_NTIME(t->attr.period));
#endif /* REAL_TIME */

  pthread_terminate();
}

/*------------------------------------------------------------*/
/*
 * pthread_terminate - terminate thread: call cleanup handlers and
 * destructor functions, allow no more signals, dequeue from ready,
 * and switch context
 */
static void pthread_terminate()
{
  register pthread_t p, t = mac_pthread_self();
  register pthread_key_t i;
  register pthread_cleanup_t new;
#ifdef CLEANUP_HEAP
  register pthread_cleanup_t old;
#endif
  sigset_t abs_all_signals;

  SET_KERNEL_FLAG;
  if (!(t->state & T_EXITING)) {
    t->state |= T_EXITING;
    CLEAR_KERNEL_FLAG;
  }
  else {
    CLEAR_KERNEL_FLAG;
    return;
  }

  /*
   * call cleanup handlers in LIFO
   */
#ifdef CLEANUP_HEAP
  for (old = (pthread_cleanup_t) NULL, new = t->cleanup_top; new;
       old = new, new = new->next) {
    (new->func)(new->arg);
    if (old)
      free(old);
  }
  if (old)
    free(old);
#else
  for (new = t->cleanup_top; new; new = new->next)
    (new->func)(new->arg);
#endif

  /*
   * call destructor functions for data keys (if both are defined)
   */
  for (i = 0; i < n_keys; i++)
    if (t->key[i] && key_destructor[i])
      (key_destructor[i])(t->key[i]);

  /*
   * No more signals, also remove from queue of all threads
   */
  SET_KERNEL_FLAG;
  pthread_sigcpyset2set(&abs_all_signals, &all_signals);
  sigaddset(&abs_all_signals, SIGCANCEL);
  if (!pthread_siggeset2set(&t->mask, &abs_all_signals)) {

    if (t->state & (T_SIGWAIT | T_SIGSUSPEND)) {
      t->state &= ~(T_SIGWAIT | T_SIGSUSPEND);
      sigemptyset(&t->sigwaitset);
    }

    pthread_q_deq(&all, t, ALL_QUEUE);
    pthread_sigcpyset2set(&t->mask, &abs_all_signals);

  }

  /*
   * dequeue thread and schedule someone else
   */

  if (t->state & (T_SYNCTIMER | T_ASYNCTIMER)) {
#ifdef TIMER_DEBUG
    fprintf(stderr, "pthread_terminate: pthread_cancel_timed_sigwait\n");
#endif
    pthread_cancel_timed_sigwait(t, FALSE, ALL_TIME, FALSE);
  }
  t->state &= ~T_RUNNING;
  t->state &= ~T_EXITING;
  t->state |= T_RETURNED;

  /*
   * Terminating thread has to be detached if anyone tries to join
   * but the memory is not freed until the dispatcher is called.
   * This is required by pthread_join().
   * The result is copied into the TCB of the joining threads to
   * allow the memory of the current thread to be reclaimed before
   * the joining thread accesses the result.
   */
  if (t->joinq.head) {
    t->state |= T_DETACHED;
    for (p = t->joinq.head; p; p = p->next[PRIMARY_QUEUE])
      p->result = t->result;
    pthread_q_wakeup_all(&t->joinq, PRIMARY_QUEUE);
  }
  
#ifdef STACK_CHECK
  if (!(t->state & T_MAIN))
    pthread_unlock_all_stack(t);
#endif

  /*
   * The last threads switches off the light and calls UNIX exit
   */
  SIM_SYSCALL(TRUE);
  if (--n_pthreads) {
    pthread_q_deq(&ready, t, PRIMARY_QUEUE);
    CLEAR_KERNEL_FLAG;
  }
  else {
#ifdef STAND_ALONE
    exit();
#else
    pthread_clear_sighandler();
    pthread_process_exit(0);
#endif
  }
}

/*------------------------------------------------------------*/
/*
 * pthread_create - Create a new thread of execution. the thread 
 * structure and a queue and bind them together.
 * The completely created pthread is then put on the active list before
 * it is allowed to execute. Caution: The current implementation uses 
 * pointers to the thread structure as thread ids. If a thread is not 
 * valid it's pointer becomes a dangling reference but may still be 
 * used for thread operations. It's up to the user to make sure he 
 * never uses dangling thread ids. If, for example, the created thread
 * has a higher priority than the caller of pthread_create() and the 
 * created thread does not block, the caller will suspend until the 
 * child has terminated and receives a DANGLING REFERENCE as the 
 * thread id in the return value of pthread_create()! This 
 * implementation could be enhanced by modifying the type pthread_t of
 * a pointer to the thread control block and a serial number which had
 * to be compared with the serial number in the thread control block 
 * for each thread operation. Also, threads had to be allocated from a
 * fixed-size thread control pool or the serial number would become a 
 * "magic number".
 */
int pthread_create(thread, attr, func, arg)
     pthread_t *thread;
     pthread_attr_t *attr;
     pthread_func_t func;
     any_t arg;
{
  register pthread_t t;
  pthread_t parent_t = mac_pthread_self();

  if (!attr)
    attr = &pthread_attr_default;

  if (!attr->flags || thread == NULL)
    return(EINVAL);

#ifdef REAL_TIME
  {
    struct timespec now;

    clock_gettime(CLOCK_REALTIME, &now);
    if ((ISNTIMERSET(attr->starttime) && !GTEQ_NTIME(attr->starttime, now)) ||
	(ISNTIMERSET(attr->deadline)  && !GTEQ_NTIME(attr->deadline , now)) ||
	(ISNTIMERSET(attr->period) && !ISNTIMERSET(attr->starttime)))
      return(EINVAL);
  }
#endif /* REAL_TIME */

  t = pthread_alloc(func, arg);
  if (t == NO_PTHREAD)
    return(EAGAIN);

  t->attr.stacksize = attr->stacksize;
  t->attr.flags = attr->flags;
#ifdef _POSIX_THREADS_PRIO_PROTECT
#ifdef SRP
  t->max_ceiling_prio = NO_PRIO;
  t->new_prio = NO_PRIO;
#endif
#endif
  if (attr->inheritsched == PTHREAD_EXPLICIT_SCHED) {
    t->attr.contentionscope = attr->contentionscope;
    t->attr.inheritsched = attr->inheritsched;
    t->attr.sched = attr->sched;
    t->attr.param.sched_priority = attr->param.sched_priority;
#ifdef _POSIX_THREADS_PRIO_PROTECT
    t->base_prio = attr->param.sched_priority;
#endif
  }
  else {
    t->attr.contentionscope = parent_t->attr.contentionscope;
    t->attr.inheritsched = parent_t->attr.inheritsched;
    t->attr.sched = parent_t->attr.sched;
#ifdef _POSIX_THREADS_PRIO_PROTECT
    t->attr.param.sched_priority = t->base_prio = parent_t->base_prio;
#else
    t->attr.param.sched_priority = parent_t->attr.param.sched_priority;
#endif
  }
#ifdef REAL_TIME
  t->attr.starttime.tv_sec = attr->starttime.tv_sec;
  t->attr.starttime.tv_nsec = attr->starttime.tv_nsec;
  t->attr.deadline.tv_sec = attr->deadline.tv_sec;
  t->attr.deadline.tv_nsec = attr->deadline.tv_nsec;
  t->attr.period.tv_sec = attr->period.tv_sec;
  t->attr.period.tv_nsec = attr->period.tv_nsec;
#endif /* REAL_TIME */
#ifdef DEF_RR
  t->num_timers = 0;
  t->interval.tv_sec = 0;
  t->interval.tv_usec = 0;
#endif
  pthread_queue_init(&t->joinq);
  /*
   * inherit the parent's signal mask
   */
  pthread_sigcpyset2set(&t->mask, &parent_t->mask);
  if (attr->detachstate == PTHREAD_CREATE_DETACHED)
    t->state |= T_DETACHED;
  *thread= t;

  SET_KERNEL_FLAG;
  ++n_pthreads; 
  if (!pthread_alloc_stack(t)) {
    CLEAR_KERNEL_FLAG;
    return(ENOMEM);
  }
  pthread_initialize(t);
  pthread_q_all_enq(&all, t);
  pthread_q_primary_enq(&ready, t);
  SIM_SYSCALL(TRUE);
  CLEAR_KERNEL_FLAG;
	
#ifdef TDI_SUPPORT
  TdiThreadRegister(*thread);
#endif
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_suspend_internal() - suspend thread indefinitely,
 * assumes SET_KERNEL_FLAG
 */
void pthread_suspend_internal(t)
     pthread_t t;
{
  t->suspend.state = t->state & (T_RUNNING | T_BLOCKED);
  t->suspend.queue = t->queue;
  t->suspend.cond = t->cond;
  
  if (t->state & (T_RUNNING | T_BLOCKED))
    pthread_q_deq(t->queue, t, PRIMARY_QUEUE);
  if (t->cond) {
    if (!--t->cond->waiters)
      t->cond->mutex = NO_MUTEX;
    t->cond = NO_COND;
  }
  t->state |= T_SUSPEND;
  t->state &= ~(T_RUNNING | T_BLOCKED);
  pthread_q_primary_enq(&suspend_q, t);
}

/*------------------------------------------------------------*/
/*
 * pthread_suspend_np() - suspend thread indefinitely
 */
int pthread_suspend_np(t)
     pthread_t t;
{
  if (is_in_kernel)
    return(EINVAL);

  SET_KERNEL_FLAG;
  if (!t || t->state & (T_SUSPEND | T_EXITING | T_RETURNED)) {
    CLEAR_KERNEL_FLAG;
    return(EINVAL);
  }

  if (t->state & T_RUNNING) {
    pthread_pending_sigaction.func = pthread_suspend_internal;
    pthread_pending_sigaction.arg = t;
    /* issue a SIGALRM to force a suspend on the current thread, if requested */
    __pthread_debug_TDI_ignored_signals |= 0x1 << (SIGALRM-1);
  }
  else
    pthread_suspend_internal(t);
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_resume_np() - resume suspended thread in saved state
 */
int pthread_resume_np(t)
     pthread_t t;
{
  int was_in_kernel = is_in_kernel;

  if (was_in_kernel && mac_pthread_self() != NULL)
    return(EINVAL);

  if (!was_in_kernel)
    SET_KERNEL_FLAG;
  if (!t || !(t->state & T_SUSPEND)) {
    if (!was_in_kernel)
      CLEAR_KERNEL_FLAG;
    return(EINVAL);
  }

 /*
  * assumption: Q ist 1st component in pthread_mutex_t structure !!!
  * check that cond is still associated with mutex
  */
  if (t->suspend.cond) {
    pthread_mutex_t *mutex = (pthread_mutex_t *) t->queue;
    if (t->suspend.cond->mutex != mutex) {
      CLEAR_KERNEL_FLAG;
      return(EINVAL);
    }
    t->suspend.cond->waiters++;
  }      

  pthread_q_deq(t->queue, t, PRIMARY_QUEUE);
  t->state = t->suspend.state;
  t->queue = t->suspend.queue;
  t->cond = t->suspend.cond;
  pthread_q_primary_enq(t->queue, t);
  if (!was_in_kernel)
    CLEAR_KERNEL_FLAG;
  if (ready.head == ready.tail)
    kill(getpid(), SIGALRM); /* ignore signal but re-check ready queue */
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_equal() - Cmpares two threads. Returns
 *      0 if t1 <> t2
 *      1 if t1 == t2
 */
int pthread_equal(t1, t2)
     pthread_t t1, t2;
{
  return(t1 == t2);
}

/*------------------------------------------------------------*/
/*
 * pthread_detach - Detaching a running thread simply consists of 
 * marking it as such. If the thread has returned then the resources 
 * are also freed.
 */
int pthread_detach(thread)
     pthread_t thread;
{
  if (thread == NO_PTHREAD)
    return(EINVAL);

  SET_KERNEL_FLAG;

  if (thread->state & T_DETACHED) {
    CLEAR_KERNEL_FLAG;
    return(ESRCH);
  }
  
  thread->state |= T_DETACHED;

#ifdef TDI_SUPPORT
    TdiThreadUnregister(thread);
#endif

  if (thread->state & T_RETURNED) {
#if defined(MALLOC) || defined(STAND_ALONE)
    pthread_free(thread->stack_base);
#ifndef STAND_ALONE
    pthread_free(thread);
#endif /* !STAND_ALONE */
#else /* !(MALLOC || STAND_ALONE) */
    free(thread->stack_base);
    free(thread);
#endif /* MALLOC || STAND_ALONE */
    SIM_SYSCALL(TRUE);
  }

  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_join - Wait for a thread to exit. If the status parameter is
 * non-NULL then that threads exit status is stored in it.
 */
int pthread_join(thread, status)
     pthread_t thread;
     any_t *status;
{
  register pthread_t p = mac_pthread_self();

  if (thread == NO_PTHREAD)
    return(EINVAL);
  
  if (thread == p)
    return(EDEADLK);

  SET_KERNEL_FLAG;
  
  if (thread->state & T_RETURNED) {
    if (thread->state & T_DETACHED) {
      CLEAR_KERNEL_FLAG;
      return(ESRCH);
    }

    if (status)
      *status = thread->result;

    thread->state |= T_DETACHED;

#ifdef TDI_SUPPORT
    TdiThreadUnregister(thread);
#endif

#if defined(MALLOC) || defined(STAND_ALONE)
    pthread_free(thread->stack_base);
#ifndef STAND_ALONE
    pthread_free(thread);
#endif /* !STAND_ALONE */
#else /* !(MALLOC || STAND_ALONE) */
    free(thread->stack_base);
    free(thread);
#endif /* MALLOC || STAND_ALONE */
    
    CLEAR_KERNEL_FLAG;
    return(0);
  }
  
  /*
   * clear error number before suspending
   */
  set_errno(0);

  pthread_q_sleep(&thread->joinq, PRIMARY_QUEUE);
  p->state |= T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }
  
  if (get_errno() == EINTR)
    return(EINTR);

  /*
   * status was copied into result field of current TCB by other thread
   */
  if (status)
    *status = p->result;
  return(0);
}

/*------------------------------------------------------------*/
/* Function:
 *    sched_yield -  Yield the processor to another thread.
 *    The current process is taken off the queue and another executes
 *    Simply put oneself at the tail of the queue.
 */
int sched_yield(void)
{
  SET_KERNEL_FLAG;
  if (ready.head != ready.tail) {
    /*
     * Place ourself at the end of the ready queue.
     * This allows the other ready processes to execute thus
     * in effect yielding the processor.
     */
    pthread_q_primary_enq(&ready, pthread_q_deq_head(&ready, PRIMARY_QUEUE));
    SIM_SYSCALL(TRUE);
  }
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_exit -  Save the exit status of the thread so that other 
 * threads joining with this thread can find it.
 */
void pthread_exit(status)
     any_t status;
{
  register pthread_t t = mac_pthread_self();
  t->result = status;
  pthread_terminate();
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_init - Initializes the attr (thread attribute object)
 * with the default values.
 */
int pthread_attr_init(attr)
     pthread_attr_t *attr;
{
  if (!attr)
    return(EINVAL);

  attr->flags = TRUE;
  attr->contentionscope = PTHREAD_SCOPE_PROCESS;
  attr->inheritsched = PTHREAD_EXPLICIT_SCHED;
  attr->detachstate = PTHREAD_CREATE_JOINABLE;
#ifdef DEF_RR
  attr->sched = SCHED_RR;
#else
  attr->sched = SCHED_FIFO;
#endif
  attr->stacksize = DEFAULT_STACKSIZE;
  attr->param.sched_priority = DEFAULT_PRIORITY;
#ifdef REAL_TIME
  NTIMERCLEAR(attr->starttime);
  NTIMERCLEAR(attr->deadline);
  NTIMERCLEAR(attr->period);
#endif /* REAL_TIME */
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_destroy - Destroys the thread attribute object.
 */
int pthread_attr_destroy(attr)
     pthread_attr_t *attr;
{
  if (!attr || !attr->flags) {
    return(EINVAL);
  }
  attr->flags = FALSE;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_getschedparam - get the thread scheduling policy and params
 */
int pthread_getschedparam(thread, policy, param)
     pthread_t thread;
     int *policy;
     struct sched_param *param;
{
  if (thread == NO_PTHREAD) {
    return(ESRCH);
  }
  
  if (!policy || !param)
    return(EINVAL);

  *policy = thread->attr.sched;
#ifdef _POSIX_THREADS_PRIO_PROTECT
  param->sched_priority = thread->base_prio;
#else
  param->sched_priority = thread->attr.param.sched_priority;
#endif
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_setschedparam - Set the thread specific scheduling policy and params
 */
int pthread_setschedparam(thread, policy, param)
     pthread_t thread;
     int policy;
     struct sched_param *param;
{
  pthread_t p = mac_pthread_self();
  pthread_queue_t q;
  int oldsched, run_prio;

  if (thread == NO_PTHREAD) 
    return(ESRCH);

  if (!param)
    return(EINVAL);
  
#ifdef DEF_RR
  if (policy != SCHED_FIFO && policy != SCHED_RR) {
#else
  if (policy != SCHED_FIFO) {
#endif
    return(ENOTSUP);
  }

  if (param->sched_priority < MIN_PRIORITY ||
      param->sched_priority > MAX_PRIORITY)
    return(EINVAL);

#ifdef REAL_TIME
  if (ISNTIMERSET(attrs.starttime) ||
      ISNTIMERSET(attrs.deadline) ||
      ISNTIMERSET(attrs.period))
    return(EINVAL);
#endif /* REAL_TIME */

  SET_KERNEL_FLAG;
 
  if (thread->state & T_RETURNED) {
    CLEAR_KERNEL_FLAG;
    return(EINVAL);
  }

  oldsched = thread->attr.sched;
  thread->attr.sched = policy;
#ifdef DEF_RR
  if (policy != oldsched && p == thread)
    switch (oldsched) {
    case SCHED_FIFO:
      pthread_timed_sigwait(thread, NULL, RR_TIME, NULL, thread);
      break;
    case SCHED_RR:
      pthread_cancel_timed_sigwait(thread, FALSE, RR_TIME,
					 thread->queue != &ready);
      break;
    default:
      ;
    }
#endif
#ifdef _POSIX_THREADS_PRIO_PROTECT
#ifdef SRP
  if (thread->new_prio == MUTEX_WAIT) {
    thread->new_prio = param->sched_priority;
    CLEAR_KERNEL_FLAG;
    return (0);
  }
#endif
  run_prio = thread->attr.param.sched_priority;
  thread->base_prio = param->sched_priority;
  if (thread->max_ceiling_prio != NO_PRIO)
    thread->attr.param.sched_priority =
      MAX(param->sched_priority, thread->max_ceiling_prio);
  else
    thread->attr.param.sched_priority = param->sched_priority;
#else
  run_prio = thread->attr.param.sched_priority;
  thread->attr.param.sched_priority = param.sched_priority;
#endif
  q = thread->queue;
  if (q->head != thread ||
      (q->head != q->tail && thread->attr.param.sched_priority < run_prio &&
       thread->attr.param.sched_priority <
       thread->next[PRIMARY_QUEUE]->attr.param.sched_priority)) {
    pthread_q_deq(q, thread, PRIMARY_QUEUE);
    pthread_q_primary_enq_first(q, thread);
  }
 
  SIM_SYSCALL(TRUE);
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setstacksize - Aligns the "stacksize" to double
 * word boundary and then sets the size of the stack to "stacksize"
 * in thread attribute object "attr".
 */
int pthread_attr_setstacksize(attr, stacksize)
     pthread_attr_t *attr;
     size_t stacksize;
{
  if (!attr || !attr->flags)
    return(EINVAL);

  attr->stacksize = SA(stacksize); /* stack align, see asm_linkage.h */
  return(0);    
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getstacksize - gets the stacksize from an pthread 
 * attribute object.
 */
int pthread_attr_getstacksize(attr, stacksize)
     pthread_attr_t *attr;
     size_t *stacksize;
{
  if (!attr || !attr->flags)
    return(EINVAL);

  *stacksize = attr->stacksize;    
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setscope - Set the contentionscope attribute in a 
 * thread attribute object.
 */
int pthread_attr_setscope(attr,contentionscope)
     pthread_attr_t *attr;
     int contentionscope;
{
  if (!attr || !attr->flags) {
    return(EINVAL);
  }
  if (contentionscope == PTHREAD_SCOPE_PROCESS) {
    attr->contentionscope=contentionscope;
    return(0);    
  }
  else
    return(EINVAL);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setinheritsched - Set the inheritsched attribute.
 */
int pthread_attr_setinheritsched(attr, inheritsched)
     pthread_attr_t *attr;
     int inheritsched;
{
  if (!attr || !attr->flags)
    return(EINVAL);
  if (inheritsched == PTHREAD_INHERIT_SCHED ||
      inheritsched == PTHREAD_EXPLICIT_SCHED) {
    attr->inheritsched = inheritsched;
    return(0);    
  }
  else
    return(EINVAL);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setschedpolicy - set the sched attribute
 */
int pthread_attr_setschedpolicy(attr, policy)
     pthread_attr_t *attr;
     int policy;
{
  if (!attr || !attr->flags)
    return(EINVAL);

#ifdef DEF_RR
  if (policy == SCHED_FIFO || policy == SCHED_RR) {
#else
  if (policy == SCHED_FIFO) {
#endif
    attr->sched = policy;
    return(0);    
  }
  else
    return(EINVAL);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setschedparam - set the sched param attribute
 */
int pthread_attr_setschedparam(attr, param)
     pthread_attr_t *attr;
     struct sched_param *param;
{
  if (!attr || !attr->flags || !param)
    return(EINVAL);

  if (param->sched_priority >= MIN_PRIORITY &&
      param->sched_priority <= MAX_PRIORITY) {
    attr->param.sched_priority = param->sched_priority;
    return(0);    
  }
  else
    return(EINVAL);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getscope - return contentionscope attribute in "contentionscope"
 */
int pthread_attr_getscope(attr, contentionscope)
     pthread_attr_t *attr;
     int *contentionscope;
{
  if (!attr || !attr->flags || !contentionscope)
    return(EINVAL);
  *contentionscope = attr->contentionscope;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getinheritsched - return inheritsched attr in "inheritsched"
 */
int pthread_attr_getinheritsched(attr, inheritsched)
     pthread_attr_t *attr;
     int *inheritsched;
{
  if (!attr || !attr->flags || !inheritsched)
    return(EINVAL);
  *inheritsched = attr->inheritsched;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getschedpolicy - get the sched attribute
 */
int pthread_attr_getschedpolicy(attr, policy)
     pthread_attr_t *attr;
     int *policy;
{
  if (!attr || !attr->flags || !policy)
    return(EINVAL);
  *policy = attr->sched;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getschedparam - return the sched param attr in "param"
 */
int pthread_attr_getschedparam(attr, param)
     pthread_attr_t *attr;
     struct sched_param *param;
{
  if (!attr || !attr->flags || !param)
    return(EINVAL);
  param->sched_priority = attr->param.sched_priority;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setstarttime_np - set the absolute start time
 */
int pthread_attr_setstarttime_np(attr, tp)
     pthread_attr_t *attr;
     struct timespec *tp;
{
#ifndef REAL_TIME
  return(ENOSYS);
#else /* REAL_TIME */
  if (!attr || !attr->flags || !tp || !ISNTIMERSET(*tp))
    return(EINVAL);

  attr->starttime.tv_sec = tp->tv_sec;
  attr->starttime.tv_nsec = tp->tv_nsec;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getstarttime_np - get the absolute start time
 */
int pthread_attr_getstarttime_np(attr, tp)
     pthread_attr_t *attr;
     struct timespec *tp;
{
#ifndef REAL_TIME
  return(ENOSYS);
#else /* REAL_TIME */
  if (!attr || !attr->flags || !tp || !ISNTIMERSET(attr->starttime))
    return(EINVAL);

  tp->tv_sec = attr->starttime.tv_sec;
  tp->tv_nsec = attr->starttime.tv_nsec;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setdeadline_np - set the absolute deadline (XOR period)
 */
int pthread_attr_setdeadline_np(attr, tp, func)
     pthread_attr_t *attr;
     struct timespec *tp;
     pthread_func_t func;
{
#ifndef REAL_TIME
  return(ENOSYS);
#else /* REAL_TIME */
  extern struct sigaction pthread_user_handler[NNSIG];

  if (!attr || !attr->flags || !tp || !ISNTIMERSET(*tp) ||
      ISNTIMERSET(attr->period))
    return(EINVAL);

  attr->deadline.tv_sec = tp->tv_sec;
  attr->deadline.tv_nsec = tp->tv_nsec;
  sigaddset(&handlerset, TIMER_SIG);
  pthread_user_handler[TIMER_SIG].sa_handler = (void(*)()) func;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getdeadline_np - get the absolute deadline
 */
int pthread_attr_getdeadline_np(attr, tp)
     pthread_attr_t *attr;
     struct timespec *tp;
{
#ifndef REAL_TIME
  return(ENOSYS);
#else /* REAL_TIME */
  if (!attr || !attr->flags || !tp || !ISNTIMERSET(attr->deadline))
    return(EINVAL);

  tp->tv_sec = attr->deadline.tv_sec;
  tp->tv_nsec = attr->deadline.tv_nsec;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setperiod_np - set the relative period (XOR deadline)
 */
int pthread_attr_setperiod_np(attr, tp, func)
     pthread_attr_t *attr;
     struct timespec *tp;
     pthread_func_t func;
{
#ifndef REAL_TIME
  return(ENOSYS);
#else /* REAL_TIME */
  extern struct sigaction pthread_user_handler[NNSIG];

  if (!attr || !attr->flags || !tp || !ISNTIMERSET(*tp) ||
      ISNTIMERSET(attr->deadline))
    return(EINVAL);

  attr->period.tv_sec = tp->tv_sec;
  attr->period.tv_nsec = tp->tv_nsec;
  sigaddset(&handlerset, TIMER_SIG);
  pthread_user_handler[TIMER_SIG].sa_handler = (void(*)()) func;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getperiod_np - get the relative period
 */
int pthread_attr_getperiod_np(attr, tp)
     pthread_attr_t *attr;
     struct timespec *tp;
{
#ifndef REAL_TIME
  return(ENOSYS);
#else /* REAL_TIME */
  if (!attr || !attr->flags || !tp || !ISNTIMERSET(attr->period))
    return(EINVAL);

  tp->tv_sec = attr->period.tv_sec;
  tp->tv_nsec = attr->period.tv_nsec;

  return(0);
#endif /* REAL_TIME */
}

/*------------------------------------------------------------*/
/*
 * pthread_key_create - creates a new global key and spefies destructor call
 * returns 0 or upon error ENOMEM if name space exhausted,
 *                         EAGAIN if insufficient memory.
 */
int pthread_key_create(key, destructor)
     pthread_key_t *key;
     void (*destructor)();
{
  SET_KERNEL_FLAG;

  if (n_keys >= _POSIX_DATAKEYS_MAX) {
    CLEAR_KERNEL_FLAG;
    return(ENOMEM);
  }

  key_destructor[n_keys] = destructor;
  *key = n_keys++;
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_key_delete - deletes a thread-specific data key previously
 * returned by pthread_key_create().
 */
int pthread_key_delete(key)
  pthread_key_t key;
{
  if (key < 0 || key >= n_keys) {
    return(EINVAL);
  }

  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_setspecific - associate a value with a data key for some thread
 * return 0 or upon error EINVAL if the key is invalid.
 */
int pthread_setspecific(key, value)
     pthread_key_t key;
     any_t value;
{
  if (key < 0 || key >= n_keys) {
    return(EINVAL);
  }

  mac_pthread_self()->key[key] = value;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_getspecific - retrieve a value from a data key for some thread
 * returns NULL upon error if the key is invalid.
 */
any_t pthread_getspecific(key)
     pthread_key_t key;
{
  if (key < 0 || key >= n_keys) {
    return(NULL);
  }

  return mac_pthread_self()->key[key];
}

/*------------------------------------------------------------*/
/*
 * pthread_cleanup_push - push function on current thread's cleanup stack
 * and execute it with the specified argument when the thread.
 * Notice: pthread_cleanup_push_body() receives the address of the first
 * cleanup structure in an additional parameter "new".
 * (a) exits;
 * (b) is cancelled;
 * (c) calls pthread_cleanup_pop(0 with a non-zero argument;
 * (d) NOT IMPLEMENTED, CONTROVERSIAL: when a longjump reaches past the scope.
 */
#ifdef CLEANUP_HEAP
void pthread_cleanup_push(func, arg)
#else
void pthread_cleanup_push_body(func, arg, new)
#endif
     void (*func)();
     any_t arg;
#ifdef CLEANUP_HEAP
{
  pthread_cleanup_t new;
#else
     pthread_cleanup_t new;
{
#endif
  pthread_t p = mac_pthread_self();

  if (!func) {
    return;
  }

#ifdef CLEANUP_HEAP
  if (!(new = (pthread_cleanup_t) malloc(sizeof(*new))))
    return;
#endif

  new->func = func;
  new->arg = arg;

  SET_KERNEL_FLAG;
  new->next = p->cleanup_top;
  p->cleanup_top = new;
  CLEAR_KERNEL_FLAG;

  return;
}


/*------------------------------------------------------------*/
/*
 * pthread_cleanup_pop - pop function off current thread's cleanup stack
 * and execute it with the specified argument if non-zero
 */
#ifdef CLEANUP_HEAP
void pthread_cleanup_pop(execute)
#else
void pthread_cleanup_pop_body(execute)
#endif
     int execute;
{
  pthread_t p = mac_pthread_self();
  pthread_cleanup_t new;

  SET_KERNEL_FLAG;
  if (!(new = p->cleanup_top)) {
    CLEAR_KERNEL_FLAG;
    return;
  }
  p->cleanup_top = new->next;
  CLEAR_KERNEL_FLAG;

  if (execute)
    (new->func)(new->arg);
#ifdef CLEANUP_HEAP
  free(new);
#endif

  return;
}

/*------------------------------------------------------------*/
/*
 * sched_get_priority_max - inquire maximum priority value (part of .4)
 */
int sched_get_priority_max(policy)
     int policy;
{
  return(MAX_PRIORITY);
}

/*------------------------------------------------------------*/
/*
 * sched_get_priority_min - inquire minimum priority value (part of .4)
 */
int sched_get_priority_min(policy)
     int policy;
{
  return(MIN_PRIORITY);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_setdetachstate - Sets the detachstate attribute in 
 *                               attr object
 */
int pthread_attr_setdetachstate(attr, detachstate)
     pthread_attr_t *attr;
     int detachstate;
{
  if (!attr || !attr->flags || detachstate > PTHREAD_CREATE_DETACHED || detachstate < PTHREAD_CREATE_JOINABLE)
    return(EINVAL);
  attr->detachstate = detachstate;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_attr_getdetachstate - Gets the value of detachstate attribute
 *                               from attr object
 */
int pthread_attr_getdetachstate(attr, detachstate)
     pthread_attr_t *attr;
     int *detachstate;
{
  if (!attr || !attr->flags || !detachstate)
    return(EINVAL);
  *detachstate = attr->detachstate; 
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_once - The first call to this will call the init_routine()
 *                with no arguments. Subsequent calls to pthread_once()
 *                will not call the init_routine.
 */
int pthread_once(once_control, init_routine)
     pthread_once_t *once_control;
     void (*init_routine)();
{
  SET_KERNEL_FLAG;
  if (!once_control->init) {
    once_control->init = TRUE;
    pthread_mutex_init(&once_control->mutex, NULL);
  }
  CLEAR_KERNEL_FLAG;

  pthread_mutex_lock(&once_control->mutex);
  if (!once_control->exec) {
    once_control->exec = TRUE;
    (*init_routine)();
  }
  pthread_mutex_unlock(&once_control->mutex);
  return(0);
}

#if defined(MALLOC) || defined(STAND_ALONE)

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ size_t
#endif

/*------------------------------------------------------------*/
malloc_t malloc (size)
     __SIZE_TYPE__ size;
{
  malloc_t ret;
#if !defined (__FreeBSD__)
  extern char *pthread_malloc();
#endif

  /*
   * kludge: X malloc convention: for X, even malloc(0) must not return NULL!
   */
  if (size == 0) 
    size = 1;

  SET_KERNEL_FLAG;
  ret = (malloc_t) pthread_malloc(size);
  CLEAR_KERNEL_FLAG;
#ifdef MALLOC_DEBUG
  MALLOC_DEBUG_MSG(ret, "alloc malloc");
#endif
  return(ret);
}

/*------------------------------------------------------------*/
#if defined (__cplusplus) || defined (__STDC__)
void free(ptr)
#else
int free (ptr)
#endif
     malloc_t ptr;
{
#if !defined (__FreeBSD__)
  int ret;
  extern int pthread_free();
#endif

#ifdef MALLOC_DEBUG
  MALLOC_DEBUG_MSG(ptr, "free free");
#endif
  SET_KERNEL_FLAG;
#if !(defined (__cplusplus) || defined (__STDC__))
  ret = pthread_free(ptr);
#else
  pthread_free(ptr);
#endif
  CLEAR_KERNEL_FLAG;
#if !(defined (__cplusplus) || defined (__STDC__))
  return(ret);
#endif
}

#endif /* MALLOC || STAND_ALONE */

#ifdef MALLOC

/*------------------------------------------------------------*/
malloc_t realloc (ptr, size)
     malloc_t ptr;
     __SIZE_TYPE__ size;
{
  malloc_t ret;
#if !defined (__FreeBSD__)
  extern char *pthread_realloc();
#endif

#ifdef MALLOC_DEBUG
  MALLOC_DEBUG_MSG(ptr, "free realloc");
#endif
  /*
   * kludge: X malloc convention: for X, even malloc(0) must not return NULL!
   */
  if (size == 0) 
    size = 1;

  SET_KERNEL_FLAG;
  ret = (malloc_t) pthread_realloc(ptr, size);
  CLEAR_KERNEL_FLAG;
#ifdef MALLOC_DEBUG
  MALLOC_DEBUG_MSG(ret, "alloc realloc");
#endif
  return(ret);
}

/*------------------------------------------------------------*/
malloc_t calloc (nmemb, elsize)
     __SIZE_TYPE__ nmemb, elsize;
{
  malloc_t ret;
#if !defined (__FreeBSD__)
  extern char *pthread_calloc();
#endif

  /*
   * kludge: X malloc convention: for X, even malloc(0) must not return NULL!
   */
  if (elsize == 0) 
    elsize = 1;
  if (nmemb == 0)
    nmemb = 1;

  SET_KERNEL_FLAG;
  ret = (malloc_t) pthread_calloc(nmemb, elsize);
  CLEAR_KERNEL_FLAG;
#ifdef MALLOC_DEBUG
  MALLOC_DEBUG_MSG(ret, "alloc calloc");
#endif
  return(ret);
}

/*------------------------------------------------------------*/
void cfree (ptr)
     malloc_t ptr;
{
#if !defined (__FreeBSD__)
  extern pthread_cfree();
#endif

#ifdef MALLOC_DEBUG
  MALLOC_DEBUG_MSG(ptr, "free cfree");
#endif
  SET_KERNEL_FLAG;
  pthread_cfree(ptr);
  CLEAR_KERNEL_FLAG;
}

/*------------------------------------------------------------*/
malloc_t memalign(alignment, size)
     __SIZE_TYPE__ alignment, size;
{
  malloc_t ret;
#if !defined (__FreeBSD__)
  extern char *pthread_memalign();
#endif

  SET_KERNEL_FLAG;
  ret = (malloc_t) pthread_memalign(alignment, size);
  CLEAR_KERNEL_FLAG;
#ifdef MALLOC_DEBUG
  MALLOC_DEBUG_MSG(ret, "alloc memalign");
#endif
  return(ret);
}

/*------------------------------------------------------------*/
malloc_t valloc(size)
     __SIZE_TYPE__ size;
{
  malloc_t ret;
#if !defined (__FreeBSD__)
  extern char *pthread_valloc();
#endif

  /*
   * kludge: X malloc convention: for X, even malloc(0) must not return NULL!
   */
  if (size == 0) 
    size = 1;

  SET_KERNEL_FLAG;
  ret = (malloc_t) pthread_valloc(size);
  CLEAR_KERNEL_FLAG;
#ifdef MALLOC_DEBUG
  MALLOC_DEBUG_MSG(ret, "alloc valloc");
#endif
  return(ret);
}

#endif /* MALLOC */

/*------------------------------------------------------------*/

#ifdef IO
/* foo - The following is intentionally dead code since foo is never called.
 * It ensures that the new versions of read and write are always linked in.
 */
static void pthread_dead_code()
{
  char *buf;

  READ(0, buf, 0);
  WRITE(1, buf, 0);
}
#endif /* IO */
