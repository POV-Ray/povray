/* Copyright (C) 1992-2000 1993, 1994, 1995, 1996 the Florida State University
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

  @(#)wait.c	3.14 11/8/00
  
*/

#include <pthread/config.h>
#include "signal_internals.h"
#include "internals.h"
#include <sys/types.h>
#if defined(SCO5)
#include <sys/resource.h>
#include <sys/wait.h>
#endif

#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
/*
 * aioread/write may cause a stack overflow in the UNIX kernel which cannot
 * be caught by sighandler. This seems to be a bug in SunOS. We get
 * around this problem by deliberately trying to access a storage
 * location on stack about a page ahead of where we are. This will
 * cause a premature stack overflow (SIGBUS) which *can* be caught
 * by sighandler.
 */
  extern int pthread_page_size;
  volatile static int pthread_access_dummy;
#define ACCESS_STACK \
  MACRO_BEGIN \
    if (*(int *) (pthread_get_sp() - pthread_page_size)) \
      pthread_access_dummy = 0; \
  MACRO_END

#else /* /* !STACK_CHECK || !SIGNAL_STACK */ */
#define ACCESS_STACK
#endif /* /* !STACK_CHECK || !SIGNAL_STACK */ */

#if defined(SCO5)
#include <sys/wait.h>

/*-------------------------------------------------------------
 * wait_handle()
 *
 * This routine is called by the interrupt handler when we received a
 * SIGCHLD. Awake all threads waiting for death of child process.
 * Returns TRUE when a thread is wakeup, FALSE otherwise.
 */
int wait_handle()
{
  pthread_t p;
  int count = 0;

  for(p = all.head; p; p = p->next[ALL_QUEUE]) {
    if (sigismember(&p->sigwaitset, SIGCHLD)) {
#ifdef DEBUG
       fprintf(stderr, "wake up\n");
#endif
       count++;
       pthread_q_wakeup_thread(NO_QUEUE, p, NO_QUEUE_INDEX);
       sigdelset(&p->sigwaitset, SIGCHLD);
       if (p->state & T_SYNCTIMER)
          pthread_cancel_timed_sigwait(p, FALSE, SYNC_TIME, TRUE);
#ifdef IO
        p->wait_on_select = FALSE;
#endif
    }
  }

  if (count)
    return(TRUE);
  else
    return(FALSE);
}

/*-------------------------------------------------------------
 * wait_for_child()
 * Put the thread signal wait for SIGCHLD.
 * Assumes: SET_KERNEL_FLAG
 */
void wait_for_child()

{
  pthread_t p;
/*
 * ACCESS_STACK;
 */
  
  p = mac_pthread_self();
  sigaddset(&p->sigwaitset, SIGCHLD);
  p->state &= ~T_RUNNING;
  p->state |= T_BLOCKED | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) &&
     !sigismember(&p->mask, SIGCANCEL))
     SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
     pthread_q_deq_head(&ready, PRIMARY_QUEUE);
     SIM_SYSCALL(TRUE);
     CLEAR_KERNEL_FLAG;
  }
  SET_KERNEL_FLAG;
}

/*-------------------------------------------------------------
 * waitpid()
 */
pid_t waitpid(pid_t pid, int *status, int options)
{
  pid_t ret;

  SET_KERNEL_FLAG;
  ret = WAITPID(pid, status, options | WNOHANG);
  /* If we are not doing nohang, try again, else return immediately */
  if (!(options & WNOHANG)) {
    while (ret == 0) {
      /* Wait for a child to terminate.
       */
      wait_for_child();
      ret = WAITPID(pid, status, options | WNOHANG);
    }
  }
  CLEAR_KERNEL_FLAG;
  return(ret);
}

#if !defined(_M_UNIX)
/*-------------------------------------------------------------
 * wait3()
 */
pid_t wait3(int* status, int options, struct rusage * rusage)
{
  pid_t ret;

  SET_KERNEL_FLAG;
  ret = WAIT3(status, options | WNOHANG, rusage);
  /* If we are not doing nohang, try again, else return immediately */
  if (!(options & WNOHANG)) {
    while (ret == 0) {
      /* wait for a child to terminate.
       */
      wait_for_child();
      ret = WAIT3(status, options | WNOHANG, rusage);
    }
  }
  CLEAR_KERNEL_FLAG;
  return(ret);
}
#endif

/* ==========================================================================
 * wait()
 */
pid_t wait(int* status)
{
	return(waitpid((pid_t)-1, (int *)status, 0));
}
#endif /* /* SCO5 */ */
