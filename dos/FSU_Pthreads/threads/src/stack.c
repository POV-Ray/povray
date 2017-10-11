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

  @(#)stack.c	3.14 11/8/00

*/

/*
 * Thread stack allocation.
 * This handles all stack allocation including defined policies.
 */

#include "internals.h"
#include <sys/resource.h>

#if !defined (_M_UNIX) || defined(SCO5)
#include <sys/mman.h>
#endif

#ifdef __FreeBSD__
# include <machine/param.h>
# define  _SC_PAGESIZE PAGE_SIZE
# define  PROT_NONE 0
#endif

extern char *pthread_get_sp();
extern int *pthread_get_fp();
void pthread_set_sp();
void pthread_set_fp();

#ifdef STAND_ALONE
int pthread_page_size = 8192;
#else /* !STAND_ALONE */
#ifdef STACK_CHECK
int pthread_page_size;
#endif
#endif

#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
#ifdef SOLARIS
#include <sys/frame.h>
#else /* !SOLARIS */
#include <sparc/frame.h>
#endif /* !SOLARIS */

static struct frame f;
static int fp_offset = (int)&f.fr_savfp - (int)&f;
static int fp_index = ((int)&f.fr_savfp - (int)&f) / sizeof(int);
#endif

#ifdef STACK_CHECK
extern KERNEL_STACK pthread_tempstack;
#ifdef SIGNAL_STACK
#define NUM_PAGES 3
#else /* !SIGNAL_STACK */
#define NUM_PAGES 1
#endif /* !SIGNAL_STACK */
#else /* !STACK_CHECK */
#define NUM_PAGES -1
#endif /* !STACK_CHECK */

/*------------------------------------------------------------*/
/*
 * pthread_stack_init - initialize the main thread's stack
 */
void pthread_stack_init(p)
pthread_t p;
{
#ifndef __dos__
  struct rlimit rlim;
#endif
  extern int pthread_started;
  
#if defined(STACK_CHECK) && !defined(STAND_ALONE)
  pthread_page_size = (int) GETPAGESIZE(_SC_PAGESIZE);
#endif

  /*
   * Stack size of the main thread is the stack size of the process
   */
#ifdef STAND_ALONE
  p->attr.stacksize = 150*SA(MINFRAME);
#else
#if !defined (_M_UNIX) && !defined(SCO5) && !defined (__dos__)
  if (getrlimit(RLIMIT_STACK, &rlim) != 0) {
    perror("getrlimit");
    pthread_process_exit(1);
  }
  p->attr.stacksize = rlim.rlim_cur;
#else /* _M_UNIX */
  p->attr.stacksize = 12 * 1024 * 1024;  /* 12MB */
#endif
#endif

  /*
   * dummy stack base which can be freed
   */
#if defined(MALLOC) || defined(STAND_ALONE)
  if (!pthread_started)
    p->stack_base = (char *) pthread_malloc(sizeof(int));
  else
#endif /* MALLOC || STAND_ALONE */
    p->stack_base = (char *) malloc(sizeof(int));
#ifdef STACK_CHECK
  pthread_lock_stack_np(p);
#endif /* STACK_CHECK */
}

/*------------------------------------------------------------*/
/*
 * pthread_lock_stack_np - lock memory page on stack
 * lock page at lower bound of stack to cause segmentation violation
 * when stack overflows
 */
int pthread_lock_stack_np(p)
pthread_t p;
{
#ifndef STACK_CHECK
  set_errno(ENOSYS);
  return (-1);
#else /* STACK_CHECK */
#ifdef SIGNAL_STACK
  SIGSTACK_T ss;
#ifndef SVR4

#ifndef STAND_ALONE
  ss.ss_sp = (char *) SA((int) pthread_tempstack_top - STACK_OFFSET);
  CLR_SS_ONSTACK;
  if (SIGSTACK(&ss, (SIGSTACK_T *) NULL)) {
#ifdef DEBUG
    fprintf(stderr,
            "Pthreads: Could not specify signal stack, errno %d\n", errno);
#endif /* DEBUG */
    return(-1);
  }
#endif /* !STAND_ALONE */
#endif /* !SVR4 */
#endif /* SIGNAL_STACK */

  if (p->state & T_MAIN ||
      !mprotect((caddr_t) PA(p->stack_base),
                NUM_PAGES * pthread_page_size,
                PROT_NONE)) {
#ifdef SIGNAL_STACK
    p->state |= T_LOCKED;
#endif /* SIGNAL_STACK */
    return (0);
  }
#ifdef DEBUG
    fprintf(stderr,
            "Pthreads: Could not lock stack, errno %d\n", errno);
#endif /* DEBUG */
  return (-1);
#endif /* STACK_CHECK */
}

#ifdef STACK_CHECK
/*------------------------------------------------------------*/
/*
 * pthread_unlock_all_stack - unlock all pages of stack, called at thread
 *                    termination.
 */
int pthread_unlock_all_stack(p)
pthread_t p;
{
  if (p->state & T_MAIN)
    return(0);
  else
    return mprotect((caddr_t) PA(p->stack_base), 
                    NUM_PAGES * pthread_page_size,
                    PROT_READ | PROT_WRITE);
}
#endif /* STACK_CHECK */

#if defined(SIGNAL_STACK) && defined(STACK_CHECK)
/*------------------------------------------------------------*/
/*
 * pthread_unlock_stack - unlock memory pages on stack if overflow occurs
 */
int pthread_unlock_stack(p)
pthread_t p;
{
  if (p->state & T_MAIN ||
      !mprotect((caddr_t) PA(p->stack_base)+pthread_page_size,
                (NUM_PAGES-1) * pthread_page_size, 
                PROT_READ | PROT_WRITE)) {
    p->state &= ~T_LOCKED;
    return (0);
  }
  return (-1);
}
#endif /* SIGNAL_STACK && STACK_CHECK */

/*------------------------------------------------------------*/
/*
 * pthread_alloc_stack - allocate stack space on heap for a thread
 * Stacks are deallocated when the thread has returned and is detached.
 * In case, SIGNAL_STACK is defined, 2 extra pages are allocated and
 * locked to detect stack overflow condition. These pages are unlocked to
 * allow the stack overflow handling.
 */
int pthread_alloc_stack(p)
pthread_t p;
{
#ifndef _POSIX_THREAD_ATTR_STACKSIZE
  p->attr.stacksize = DEFAULT_STACKSIZE;
#endif

  p->stack_base = (char *)
#if defined(MALLOC) || defined(STAND_ALONE)
    pthread_malloc
#else /* !MALLOC && !STAND_ALONE */
    malloc
#endif /* !MALLOC && !STAND_ALONE */
      (p->attr.stacksize + PTHREAD_BODY_OFFSET
#ifdef STACK_CHECK
                                  + pthread_page_size * (NUM_PAGES+1)
#endif /* STACK_CHECK */
                                  );
  
  if ((int) p->stack_base == (int) NULL) {
#ifdef DEBUG
    fprintf(stderr, "\n*** Out of space for thread stacks. ***\n");
    fflush(stderr);
#endif
    return(FALSE);
  }
  
#ifdef STACK_CHECK
  if (pthread_lock_stack_np(p))
    return(FALSE);
#endif

  return(TRUE);
}

#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
/*------------------------------------------------------------*/
/*
 * switch_stacks - Flushes the windows and copies the stack frames from signal 
 *   stack to thread's stack and then sets the frame pointer links correctly for
 *   thread's stack and then finally switches to thread's stack.
 */

void pthread_switch_stacks(oldsp, sip, scp)
int oldsp, **sip, **scp;
{
  char *sp = pthread_get_sp();
  int *fp = pthread_get_fp();
  int size = (char *) SA((int) pthread_tempstack_top) - sp;
  int *target = (int *)(oldsp - size);
  int *user_fp = target;
  int *last_user_fp = NULL;
  int *nsip, *nscp;

  if (sip) {
    nsip = (int*) (oldsp - ((int) sip - (int) sp));
    *nsip = oldsp - ((int) *sip - (int) sp);
    *sip = nsip;
    **sip = *nsip;
  }
  if (scp) {
    nscp = (int*) (oldsp - ((int) scp - (int) sp));
    *nscp = oldsp - ((int) *scp - (int) sp);
    *scp = nscp;
    **scp = *nscp;
  }

  pthread_ST_FLUSH_WINDOWS();
  memcpy(target, sp, size);

  do {
    user_fp[fp_index] = (int)user_fp + (int)((char *)fp - sp);
    last_user_fp = user_fp;
    user_fp = (int *)user_fp[fp_index];
    sp = (char *)fp;
    fp = (int *)fp[fp_index];
  } while (fp && ((char *)fp < ((char *) SA((int) pthread_tempstack_top))));

  user_fp[fp_index] = oldsp;
  pthread_set_fp(target[fp_index]);
  pthread_set_sp(target);
}

void pthread_print_stack(sp)
int *sp;
{
  struct frame *fp;

  do {
    fp = (struct frame *) sp;
    printf("frame pc = %x, sp = %x, arg0 %x, arg1 %x\n",
	   fp->fr_savpc, sp, fp->fr_arg[0], fp->fr_arg[1]);
    fflush(stdout);
    sp = (int *)sp[fp_index];
  } while (sp);
}
/*------------------------------------------------------------*/
#endif
