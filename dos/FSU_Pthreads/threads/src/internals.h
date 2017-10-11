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

  @(#)internals.h	3.14 11/8/00

*/

#ifndef _pthread_pthread_internals_h
#define _pthread_pthread_internals_h

#include <pthread/config.h>
#include <../src/config_internals.h>

#include "signal_internals.h"

/*
 * Pthreads interface internals
 */

/*
 * We needed for MINFRAME and WINDOWSIZE. For SVR4, get it from include files
 * or figure it out and plug it in.
 */
#ifdef SOLARIS
#include <pthread/types.h>
#include <sys/asm_linkage.h>
#else /* !SOLARIS */
#ifdef SVR4
#define MINFRAME ???
#define WINDOWSIZE MINFRAME
#else /* !SVR4 */
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined(__dos__)
#if defined(SCO5)
# define WINDOWSIZE (MINSIGSTKSZ)
# define MINFRAME   ((20 * 4))
#else
# define WINDOWSIZE (0)
# define MINFRAME   (WINDOWSIZE + (7 * 4))
#endif
# define STACK_ALIGN 4
# define SA(X)       (((X)+(STACK_ALIGN-1)) & ~(STACK_ALIGN-1))
  typedef void* malloc_t;
#else
#include <sparc/asm_linkage.h>
#endif /* !__FreeBSD__ */
#endif /* !SVR4 */
#endif /* !SOLARIS */

/* #include "signal_internals.h" moved up */
#include <pthread.h>

#ifndef SRP
#ifdef _POSIX_THREADS_PRIO_PROTECT
 ERROR: undefine _POSIX_THREADS_PRIO_PROTECT in unistd.h when SRP is undefined!
#endif 
#else /* SRP */
#ifndef _POSIX_THREADS_PRIO_PROTECT
 ERROR: define _POSIX_THREADS_PRIO_PROTECT in unistd.h when SRP is defined!
#endif
#endif

#if defined(ASM_SETJMP) && defined(C_CONTEXT_SWITCH)
 ERROR: either ASM_SETJMP or C_CONTEXT_SWITCH can be defined, not both
#endif

#ifdef STAND_ALONE
#define MAX_THREADS 200
#endif /* STAND_ALONE */

/* Other Program Specific Constants */
#define MAX_PRIORITY        101
#define MIN_PRIORITY        0
#define DEFAULT_PRIORITY    MIN_PRIORITY
#define DEFAULT_STACKSIZE   12288
#define MAX_STACKSIZE       2097152
#define PTHREAD_BODY_OFFSET 200

#ifdef DEF_RR
#define TIMER_QUEUE   0
#else
#define TIMER_QUEUE   2
#endif

#ifdef SOLARIS
#define READ  _read
#define WRITE _write
#else /* !SOALRIS */
#define READ  read
#define WRITE write
#endif /* !SOLARIS */

/*
 * page alignment
 */
#define PA(X) ((((int)X)+((int)pthread_page_size-1)) & \
				~((int)pthread_page_size-1))

#include <sys/param.h>
#ifndef MAX
#define MAX(x, y) ((x > y)? x : y)
#endif

/*
 * timer queue
 */
#ifdef DEF_RR
typedef struct timer_ent {
        struct timeval tp;                     /* wake-up time                */
        pthread_t thread;                      /* thread                      */
        int mode;                              /* mode of timer (ABS/REL/RR)  */
        struct timer_ent *next[TIMER_QUEUE+1]; /* next request in the queue   */
} *timer_ent_t;
#else
typedef pthread_t timer_ent_t;
#endif
 
#ifdef DEF_RR
typedef struct pthread_timer_q_s {
	struct timer_ent *head;
	struct timer_ent *tail;
} pthread_timer_q;
#elif defined(STAND_ALONE)
typedef struct pthread pthread_timer_q;
#else
typedef struct pthread_queue pthread_timer_q;
#endif
typedef pthread_timer_q *pthread_timer_q_t;

#define NO_QUEUE          ((pthread_queue_t) NULL)

#define NO_TIMER_QUEUE    ((pthread_timer_q_t) NULL)

#define NO_TIMER	  ((timer_ent_t) NULL)

#define NO_QUEUE_INDEX    0

#define	NO_QUEUE_ITEM	  ((struct pthread *) NULL)

#define	QUEUE_INITIALIZER { NO_QUEUE_ITEM, NO_QUEUE_ITEM }

#define	pthread_queue_init(q) \
	((q)->head = (q)->tail = NO_QUEUE_ITEM)

#ifdef STAND_ALONE
#define	pthread_timer_queue_init(q) \
	(q = NO_TIMER_QUEUE)
#else
#define	pthread_timer_queue_init(q) \
	((q)->head = (q)->tail = NO_TIMER)
#endif

pthread_mutexattr_t pthread_mutexattr_default;

#ifdef _POSIX_THREADS_PRIO_PROTECT 
#ifdef SRP 
#define MUTEX_WAIT -2
#define NO_PRIO -1
#endif
#endif

#define	MUTEX_INITIALIZER	{ 0, QUEUE_INITIALIZER, 0, 1 }
#define NO_MUTEX      ((pthread_mutex_t *)0)
#define MUTEX_VALID 0x1

pthread_condattr_t pthread_condattr_default;

#define	CONDITION_INITIALIZER	{ QUEUE_INITIALIZER, 1, 0 }
#define NO_COND       ((pthread_cond_t *) 0)
#define COND_VALID 0x1

/*
 * SIGKILL, SIGSTOP cannot be masked; therefore reused as masks
 */
#define TIMER_SIG       SIGKILL
#define AIO_SIG         SIGSTOP

#ifndef TIMER_MAX
#define TIMER_MAX   _POSIX_TIMER_MAX+2
#endif

#define	NO_PTHREAD	((pthread_t) 0)

pthread_attr_t pthread_attr_default;

#define NO_ATTRIBUTE ((pthread_attr_t *)0)

struct pthread_action {
  void (*func)();
  any_t arg;
};

#ifdef PTHREAD_KERNEL
struct pthread_action pthread_pending_sigaction = { NULL, NULL};
#else
extern struct pthread_action  pthread_pending_sigaction;
#endif

struct pthread_cleanup {
  void (*func)();
  any_t arg;
  struct pthread_cleanup *next;
};

typedef struct kernel {
  pthread_t k_pthread_self;            /* thread that is currently running   */
  volatile int k_is_in_kernel;         /* flag to test if in kernel          */
  int k_is_updating_timer;             /* flag to test if timeout is handled */
  volatile int k_state_change;         /* dispatcher state (run q/signals)   */
  volatile sigset_t k_new_signals;     /* bit set of new signals to handle   */
  sigset_t k_pending_signals;          /* bit set of pending signals         */
  sigset_t k_all_signals;              /* mask of all (maskable) signals     */
  sigset_t k_no_signals;               /* mask of no signals                 */
  sigset_t k_cantmask;                 /* mask of signals (cannot be caught) */
  char *k_process_stack_base;          /* stack base of process              */
  struct pthread_queue k_ready;        /* ready queue                        */
  struct pthread_queue k_all;          /* queue of all threads               */
  struct pthread_queue k_suspend_q;    /* queue of suspended threads         */
  sigset_t k_handlerset;               /* set of signals with user handler   */
  char *k_set_warning;                 /* pointer to set warning message     */
  char *k_clear_warning;               /* pointer to clear warning message   */
  char *k_prio_warning;                /* pointer to prio warning message    */
                                       /* for STAND_ALONE                    */
  sigset_t k_proc_mask;		       /* Mask for process                   */
  int  k_cur_heap;		       /* current break                      */
  volatile struct timespec k_timeofday;/* Time of Day                        */
#if defined(IO) && !defined(__FreeBSD__) && !defined(__dos__)
#if defined(USE_POLL)
  volatile int k_gnfds;                /* global width                       */
  volatile int k_gmaxnfds;		/* global max. number fds in k_gfds  */
  struct pollfd* k_gfds;		/* global poll events wait set */
  struct pollfd* k_gafds[2];		/* global poll events wait set */
#else /* !USE_POLL */
  volatile int k_gwidth;               /* global width                       */
  fd_set k_greadfds;                   /* global read file descriptor set    */
  fd_set k_gwritefds;                  /* global write file descriptor set   */ 
  fd_set k_gexceptfds;                 /* global except file descriptor set  */ 
#endif /* !USE_POLL */
#endif /* IO && !__FreeBSD__ && !__dos__ */
} kernel_t;

#ifdef PTHREAD_KERNEL
kernel_t pthread_kern;
#else
extern kernel_t pthread_kern;
#endif

/* Internal Functions */

/*
 * changed for speed-up and interface purposes -
 * pthread_self() is now a function, but mac_pthread_self() is used internally
 * #define pthread_self()  (pthread_kern.k_pthread_self == 0? \
 *                          NO_PTHREAD : pthread_kern.k_pthread_self)
 */
#define mac_pthread_self() pthread_kern.k_pthread_self
#define state_change       pthread_kern.k_state_change
#define is_in_kernel       pthread_kern.k_is_in_kernel
#define is_updating_timer  pthread_kern.k_is_updating_timer
#define new_signals        pthread_kern.k_new_signals
#define pending_signals    pthread_kern.k_pending_signals
#define all_signals        pthread_kern.k_all_signals
#define no_signals         pthread_kern.k_no_signals
#define cantmask           pthread_kern.k_cantmask
#define process_stack_base pthread_kern.k_process_stack_base
#define ready              pthread_kern.k_ready
#define all                pthread_kern.k_all
#define suspend_q          pthread_kern.k_suspend_q
#define handlerset         pthread_kern.k_handlerset
#define set_warning        pthread_kern.k_set_warning
#define clear_warning      pthread_kern.k_clear_warning
#define prio_warning       pthread_kern.k_prio_warning

#define proc_mask	   pthread_kern.k_proc_mask
#define cur_heap           pthread_kern.k_cur_heap
#define timeofday          pthread_kern.k_timeofday

#if defined(IO) && !defined(__FreeBSD__) && !defined(__dos__)
#if defined(USE_POLL)
#define	gnfds		   pthread_kern.k_gnfds
#define gmaxnfds	   pthread_kern.k_gmaxnfds
#define gfds		   pthread_kern.k_gfds
#define gafds		   pthread_kern.k_gafds
#else /* !USE_POLL */
#define gwidth             pthread_kern.k_gwidth    
#define greadfds  	   pthread_kern.k_greadfds  
#define gwritefds 	   pthread_kern.k_gwritefds 
#define gexceptfds	   pthread_kern.k_gexceptfds
#endif /* !USE_POLL */
#endif /* IO && !__FreeBSD__ && !__dos__ */

/*
 * Errno is mapped on process' _errno and swapped upon context switch
 * #define set_errno(e)    (mac_pthread_self()->context[JB_ERRNO] = e)
 * #define get_errno()     (mac_pthread_self()->context[JB_ERRNO])
 */

#define set_errno(e)    (errno = (e))
#define get_errno()     (errno)

#ifdef STAND_ALONE
#ifdef PTHREAD_KERNEL
int errno;
#else /* !PTHREAD_KERNEL */
extern int errno;
#endif /* !PTHREAD_KERNE */
#endif /* STAND_ALONE */

/*
 * context switching macros, implemented via setjmp/longjmp plus saving errno
 */
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
#define SAVE_CONTEXT(t) \
  (((t)->terrno = errno) == errno && \
   SYS_SIGSETJMP((t)->context, FALSE, TRUE) && \
   (errno = (t)->terrno) == errno)

#define RESTORE_CONTEXT(t) SYS_SIGLONGJMP((t)->context, TRUE, TRUE)
#else
#define SAVE_CONTEXT(t) \
  (((t)->terrno = errno) == errno && \
   sigsetjmp((t)->context, FALSE) && \
   (errno = (t)->terrno) == errno)

#define RESTORE_CONTEXT(t) siglongjmp((t)->context, TRUE)
#endif /* __FreeBSD__ */

/*
 * set/clear Pthread kernel flag
 */

#if defined(DEBUG) && !defined(IO)
#define SET_KERNEL_FLAG \
  MACRO_BEGIN \
    if (is_in_kernel) \
      fprintf(stderr, set_warning); \
    else \
      is_in_kernel = TRUE; \
  MACRO_END
#else
#define SET_KERNEL_FLAG is_in_kernel = TRUE
#endif

#ifdef C_CONTEXT_SWITCH

#define SHARED_CLEAR_KERNEL_FLAG \
  MACRO_BEGIN \
    is_in_kernel = FALSE; \
    if (state_change) { \
      is_in_kernel = TRUE; \
      if ((pthread_signonemptyset(&new_signals) || \
	   mac_pthread_self() != ready.head) && \
          !SAVE_CONTEXT(mac_pthread_self())) \
        pthread_sched(); \
      state_change = FALSE; \
      is_in_kernel = FALSE; \
      while (pthread_signonemptyset(&new_signals)) { \
        is_in_kernel = TRUE; \
        pthread_sched_new_signals(mac_pthread_self(), TRUE); \
        if (!SAVE_CONTEXT(mac_pthread_self())) \
          pthread_sched(); \
        state_change = FALSE; \
        is_in_kernel = FALSE; \
      } \
    } \
  MACRO_END

#else /* !C_CONTEXT_SWITCH */
#ifdef NO_INLINE

#define CLEAR_KERNEL_FLAG pthread_sched()

#else /* !NO_INLINE */

#define SHARED_CLEAR_KERNEL_FLAG \
  MACRO_BEGIN \
    is_in_kernel = FALSE; \
    if (state_change) \
      pthread_sched(); \
  MACRO_END
#endif /* NO_INLINE */

#endif /* C_CONTEXT_SWITCH */


#ifdef RR_SWITCH
#define CLEAR_KERNEL_FLAG \
  MACRO_BEGIN \
    if ((mac_pthread_self()->queue == &ready) && (ready.head != ready.tail)) { \
      pthread_q_deq(&ready,mac_pthread_self(),PRIMARY_QUEUE); \
      pthread_q_enq_tail(&ready); \
    } \
    SHARED_CLEAR_KERNEL_FLAG; \
  MACRO_END

#elif RAND_SWITCH
#define CLEAR_KERNEL_FLAG \
  MACRO_BEGIN \
    if ((mac_pthread_self()->queue == &ready) && (ready.head != ready.tail) \
        && ((int)random()&01)) { \
      pthread_q_exchange_rand(&ready); \
    } \
    SHARED_CLEAR_KERNEL_FLAG; \
  MACRO_END

#else
#if defined(DEBUG) && !defined(IO)
#define CLEAR_KERNEL_FLAG \
  MACRO_BEGIN \
    if (!is_in_kernel) \
      fprintf(stderr, clear_warning); \
    SHARED_CLEAR_KERNEL_FLAG; \
    if (ready.head->attr.prio < ready.tail->attr.prio) \
      fprintf(stderr, prio_warning); \
  MACRO_END

#else /* !DEBUG || IO */
#define CLEAR_KERNEL_FLAG SHARED_CLEAR_KERNEL_FLAG
#endif /* DEBUG && !IO */
#endif

#ifdef C_CONTEXT_SWITCH
#define SIG_CLEAR_KERNEL_FLAG(b) \
  MACRO_BEGIN \
    if(!SAVE_CONTEXT(mac_pthread_self())) \
      pthread_handle_pending_signals_wrapper(); \
    state_change = FALSE; \
    is_in_kernel = FALSE; \
    while (pthread_signonemptyset(&new_signals)) { \
      is_in_kernel = TRUE; \
      pthread_sched_new_signals(mac_pthread_self(), TRUE); \
      if (!SAVE_CONTEXT(mac_pthread_self())) \
        pthread_sched(); \
      state_change = FALSE; \
      is_in_kernel = FALSE; \
    } \
  MACRO_END
#else /* !C_CONTEXT_SWITCH */
#define SIG_CLEAR_KERNEL_FLAG(b) pthread_handle_pending_signals_wrapper(b)
#endif /* C_CONTEXT_SWITCH */

#ifdef SIM_KERNEL
#define SIM_SYSCALL(cond) if (cond) getpid()
#else /* !SIM_KERNEL */
#define SIM_SYSCALL(cond)
#endif /* !SIM_KERNEL */

#ifdef C_CONTEXT_SWITCH

struct kernel_stack {
  char body[TEMPSTACK_SIZE];
  char stack[SA(MINFRAME)];
};
#define KERNEL_STACK struct kernel_stack
#define pthread_tempstack_top pthread_tempstack.stack

#else /* !C_CONTEXT_SWITCH */

#define KERNEL_STACK char
#define pthread_tempstack_top &pthread_tempstack

#endif /* !C_CONTEXT_SWITCH */


#ifndef	MACRO_BEGIN

#define	MACRO_BEGIN	do {

#ifndef	lint
#define	MACRO_END	} while (0)
#else /* lint */
extern int _NEVER_;
#define	MACRO_END	} while (_NEVER_)
#endif /* lint */

#endif /* !MACRO_BEGIN */

#endif /* !_pthread_pthread_internals_h */
