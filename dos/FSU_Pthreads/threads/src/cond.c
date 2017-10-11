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

  @(#)cond.c	3.14 11/8/00

*/

/*
 * Functions supporting condition variables and their attributes. 
 * Condition variables block the q that the thread is running on by waiting
 * for an event.
 */

#include "internals.h"
#ifdef TDI_SUPPORT
#include "tdi-aux.h"
#endif
#ifdef NOERR_CHECK
#undef NOERR_CHECK
#include "mutex.h"
#define NOERR_CHECK
#else
#include "mutex.h"
#endif

/*------------------------------------------------------------*/
/*
 * pthread_cond_destroy - Destroys the condition variable.
 * 
 */
int pthread_cond_destroy(cond) 
pthread_cond_t *cond;
{
  if (cond == NO_COND)
    return(EINVAL);

  /*
   *   Check to see if anyone is one the queue waiting to be signalled
   *   If so we return an error.
   */
  
  if (cond->waiters)
    return(EBUSY);

  /*
   * No one is waiting. Make the condition structure invalid
   * so future calls with this handle fail and then unlock it.
   */
  cond->flags = FALSE;
  cond = NO_COND;
#ifdef TDI_SUPPORT
  TdiCondUnregister(cond);
#endif
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_cond_init - Initializes the condition variable referred
 * by cond with attributes specified by attr. If attr is NULL, 
 * default values are used.
 */
int pthread_cond_init(cond, attr)
     pthread_cond_t *cond;
     pthread_condattr_t *attr;
{
  if (cond == NO_COND)
    return(EINVAL);

#ifdef TDI_SUPPORT
  TdiCondRegister(cond);
#endif /* TDI_SUPPORT */

  if (!attr) 
    attr = &pthread_condattr_default;

  cond->flags = attr->flags;
  cond->waiters = 0;
  cond->mutex = NO_MUTEX;
  pthread_queue_init(&cond->queue);
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_condattr_init - Initializes a condition attributes
 * object with the default values.
 *
 */
int pthread_condattr_init(attr)
pthread_condattr_t *attr;
{
  if (!attr)
    return(EINVAL);

  attr->flags = TRUE;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_condattr_destroy - Destoys a condition attribute object.
 */
int pthread_condattr_destroy(attr)
pthread_condattr_t *attr;
{
  if (!attr || !attr->flags)
    return(EINVAL);

  attr->flags = FALSE;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_cond_wait - Atomically releases the mutex and causes the 
 * thread to block on condition variable cond. The thread is taken 
 * off the thread queues and queued onto condition variable with the 
 * state changed to indicate the wait. The thread is then blocked 
 * waiting for an event. This will be a signal event meaning that the 
 * condition has been signalled or broadcast.  If it was signalled 
 * the thread has already been reactivated and so the mutex is reaquired 
 * and return.
 */
int pthread_cond_wait(cond, mutex)
pthread_cond_t *cond;
pthread_mutex_t *mutex;
{
  register pthread_t p = mac_pthread_self();
  
  if (cond == NO_COND || mutex == NO_MUTEX ||
      (cond->mutex != mutex && cond->mutex != NO_MUTEX) ||
      !(mutex->lock) || mutex->owner != p)
    return(EINVAL);

  SET_KERNEL_FLAG;

  /*
   * clear error number before suspending
   */
  set_errno(0);

  cond->mutex = mutex;
  cond->waiters++;
  p->cond = cond;

  mac_mutex_unlock(mutex, p, pthread_q_sleep(&cond->queue, PRIMARY_QUEUE));
  p->state |= T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  /*
   * relock the mutex under contention, but only if it was not already
   * locked as a side-effect of executing an signal handler and thereby
   * pthread_cond_wait_terminate(). Notice that locking the mutex before
   * executing the signal handler is a feature not required by the standard.
   */
  SET_KERNEL_FLAG;
  SIM_SYSCALL(mutex->lock);
  if (mutex->owner != p) {
    mac_mutex_lock(mutex, p);
    if (!--p->cond->waiters)
      cond->mutex = NO_MUTEX;
    p->cond = NO_COND;
  }
  CLEAR_KERNEL_FLAG;

  return(get_errno());
}

/*------------------------------------------------------------*/
/*
 * pthread_cond_timedwait - This function works in the same way as 
 * pthread_cond_wait with the exception of the added complexity of
 * the timeout. The timeout is delivered as a timeout event. If the
 * timeout happened then the thread is put back on the active queue.
 */
int pthread_cond_timedwait(cond, mutex, timeout)
pthread_cond_t *cond;
pthread_mutex_t *mutex;
struct timespec *timeout;
{
  int error;
  register pthread_t p = mac_pthread_self();

  if (cond == NO_COND || mutex == NO_MUTEX ||
      (cond->mutex != mutex && cond->mutex != NO_MUTEX) ||
      !(mutex->lock) || mutex->owner != p)
    return(EINVAL);

  SET_KERNEL_FLAG;
  if (pthread_timed_sigwait(p, timeout, ABS_TIME, NULL, p) == -1) {
    error = get_errno();
#ifdef ETIMEDOUT
    if (error == EAGAIN)
      error = ETIMEDOUT;
#endif
    CLEAR_KERNEL_FLAG;
    return(error);
  }

  /*
   * clear error number before suspending
   */
  set_errno(0);

  cond->mutex = mutex;
  cond->waiters++;
  p->cond = cond;

  mac_mutex_unlock(mutex, p, pthread_q_sleep(&cond->queue, PRIMARY_QUEUE));
  p->state |= T_CONDTIMER | T_SYNCTIMER | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  /*
   * relock the mutex under contention, but only if it was not already
   * locked as a side-effect of executing an signal handler and thereby
   * pthread_cond_wait_terminate(). Notice that locking the mutex before
   * executing the signal handler is a feature not required by the standard.
   */
  SET_KERNEL_FLAG;
  SIM_SYSCALL(mutex->lock);
  if (mutex->owner != p) {
    mac_mutex_lock(mutex, p);
    if (!--p->cond->waiters)
      cond->mutex = NO_MUTEX;
    p->cond = NO_COND;
  }
  CLEAR_KERNEL_FLAG;

  error = get_errno();
#ifdef ETIMEDOUT
  if (error == EAGAIN)
    error = ETIMEDOUT;
#endif

  return(error);
}

/*------------------------------------------------------------*/
/*
 * pthread_cond_wait_terminate - terminate conditional wait prematurely,
 * relocks the mutex and disassociates the condition variable
 * (used when signal received while in conditional wait)
 * Notice: This is a feature. Pthreads does not require that the mutex be
 * locked inside a user signal handler when a signal interrupts
 * pthread_cond_wait().
 * assumes SET_KERNEL_FLAG for C_CONTEXT_SWITCH
 */
void pthread_cond_wait_terminate(cond)
     pthread_cond_t *cond;
{
  pthread_t p = mac_pthread_self();

#ifndef C_CONTEXT_SWITCH
  SET_KERNEL_FLAG;
#endif
  /*
   * done only once by innermost user handler if handlers are nested
   * cond must therefore be retrieved out of TCB
   */
  if (cond != NO_COND) {
    mac_mutex_lock(cond->mutex, p);
    if (!--cond->waiters)
      cond->mutex = NO_MUTEX;
    p->cond = NO_COND;
  }
#ifndef C_CONTEXT_SWITCH
  CLEAR_KERNEL_FLAG;
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_cond_signal - Unblocks (by def. at least) one thread, blocked on
 * this condition. Scan through the waiter queue of threads looking 
 * for one. The first one found is removed from the list, put on 
 * the active thread list and sent the signal event.
 * The mutex ownership is tranferred right away, unless the mutex protocol
 * is the ceiling protocol.
 */
int pthread_cond_signal(cond)
pthread_cond_t *cond;
{
  pthread_t p;

  if (cond == NO_COND)
    return(EINVAL);

  SET_KERNEL_FLAG;
  if ((p = cond->queue.head) != NO_PTHREAD) {
    if (p->state & T_SYNCTIMER)
      pthread_cancel_timed_sigwait(p, FALSE, SYNC_TIME, TRUE);
    else {
      p->state &= ~T_CONDTIMER;
      pthread_q_wakeup(&cond->queue, PRIMARY_QUEUE);
    }
    SIM_SYSCALL(TRUE);
  }
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_cond_broadcast - Step through the queue of waiting threads 
 * and send a signal event to every thread.
 * The mutex ownership is tranferred right away, unless the mutex protocol
 * is the ceiling protocol.
 */
int pthread_cond_broadcast(cond)
pthread_cond_t *cond;
{
  pthread_t p;

  if (cond == NO_COND)
    return(EINVAL);

  SET_KERNEL_FLAG;
  SIM_SYSCALL(cond->queue.head != NO_PTHREAD);
  while ((p = cond->queue.head) != NO_PTHREAD)
    if (p->state & T_SYNCTIMER)
      pthread_cancel_timed_sigwait(p, FALSE, SYNC_TIME, TRUE);
    else {
      p->state &= ~T_CONDTIMER;
      pthread_q_wakeup(&cond->queue, PRIMARY_QUEUE);
    }
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_condattr_getpshared - Not Implemented. Returns ENOSYS.
 */

int pthread_condattr_getpshared(attr, pshared)
pthread_condattr_t *attr;
int *pshared;
{
  return(ENOSYS);
}

/*------------------------------------------------------------*/
/*
 * pthread_condattr_setpshared - Not Implemented. Returns ENOSYS.
 */

int pthread_condattr_setpshared(attr, pshared)
pthread_condattr_t *attr;
int pshared;
{
  return(ENOSYS);
}
/*------------------------------------------------------------*/
