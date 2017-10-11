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

  @(#)mutex.c	3.14 11/8/00

*/

/* 
 * Functions for the support of mutual exclusion - mutexes and their 
 * attributes.  
 */

#include "internals.h"
#include "mutex.h"
#ifdef TDI_SUPPORT
#include "tdi-aux.h"
#endif

/*------------------------------------------------------------*/
/*
 * pthread_mutex_lock - Checks are made to see if the mutex is 
 * currently in use or not.  If the mutex is not in use, then its 
 * locked. Otherwise the currently executing thread is put in the 
 * wait queue and a new thread is selected. (Fast mutex_lock without
 * error checks can be found in pthread_sched.S.)
 */
#ifdef NOERR_CHECK
int slow_mutex_lock(mutex)
#else
int pthread_mutex_lock(mutex)
#endif
     pthread_mutex_t *mutex;
{
  register pthread_t p = mac_pthread_self();

#ifndef NOERR_CHECK
  if (mutex == NO_MUTEX)
    return(EINVAL);

  if (mutex->owner == p)
    return(EDEADLK);

#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (mutex->protocol == PTHREAD_PRIO_PROTECT && p->attr.param.sched_priority > mutex->prioceiling)
    return(EINVAL);
#endif /* _POSIX_THREADS_PRIO_PROTECT */
#endif /* !NOERR_CHECK */

  SET_KERNEL_FLAG;
  SIM_SYSCALL(mutex->lock);
  mac_mutex_lock(mutex, p);
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_trylock - Just try to lock the mutex. If 
 * lock succeeds return. Otherwise return right away without 
 * getting the lock. (Fast mutex_trylock without
 * error checks can be found in pthread_sched.S.)
 */
#ifdef NOERR_CHECK
int slow_mutex_trylock(mutex)
#else
int pthread_mutex_trylock(mutex)
#endif
     pthread_mutex_t *mutex;
{

pthread_t p = mac_pthread_self();

#ifndef NOERR_CHECK
  if (mutex == NO_MUTEX)
    return(EINVAL);

#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (mutex->protocol == PTHREAD_PRIO_PROTECT &&
      p->attr.param.sched_priority > mutex->prioceiling)
    return(EINVAL);
#endif /* _POSIX_THREADS_PRIO_PROTECT */
#endif /* !NOERR_CHECK */
 
  SET_KERNEL_FLAG;
  if (mutex->lock) {
    CLEAR_KERNEL_FLAG;
    return(EBUSY);
  }
 
  mutex->lock = TRUE;
  mutex->owner = p;
#ifdef NOERR_CHECK 
  mac_change_lock_prio(mutex, p); 
#else 
#ifdef _POSIX_THREADS_PRIO_PROTECT 
 if (mutex->protocol == PTHREAD_PRIO_PROTECT) 
   mac_change_lock_prio(mutex, p);
#endif                  
#endif
 
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_unlock - Called by the owner of mutex to release
 * the mutex. (Fast mutex_unlock without
 * error checks can be found in pthread_sched.S.)
 */
#ifdef NOERR_CHECK
int slow_mutex_unlock(mutex)
#else
int pthread_mutex_unlock(mutex)
#endif
     pthread_mutex_t *mutex;
{
pthread_t p = mac_pthread_self();

#ifndef NOERR_CHECK
  if (mutex == NO_MUTEX || mutex->owner != mac_pthread_self())
    return(EINVAL);
#endif

  SET_KERNEL_FLAG;
  SIM_SYSCALL(mutex->queue.head != mutex->queue.tail);
  mac_mutex_unlock(mutex, p,  /* NULL */);
  CLEAR_KERNEL_FLAG;
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_init - Initialize the mutex and at the same time 
 * ensure that it's usaable.  Set the attribute values from attr 
 * specified. No check is made to see if the attributes are right 
 * as yet.
 */
int pthread_mutex_init(mutex, attr)
     pthread_mutex_t *mutex;
     pthread_mutexattr_t *attr;
{
  if (mutex == NO_MUTEX)
    return(EINVAL);

  if (!attr)
    attr = &pthread_mutexattr_default;

#ifdef TDI_SUPPORT
  TdiMutexRegister(mutex);
#endif

  mutex->owner = NO_PTHREAD;
  mutex->flags = attr->flags;
  pthread_queue_init(&mutex->queue);
#ifdef _POSIX_THREADS_PRIO_PROTECT
  mutex->prioceiling = attr->prioceiling;
  mutex->protocol = attr->protocol;
#endif
  mutex->lock = FALSE;
  return(0);
}  

/*------------------------------------------------------------*/
/*
 * pthread_mutex_destroy - Destroys the mutex.
 */
int pthread_mutex_destroy(mutex) 
     pthread_mutex_t *mutex;
{
  if (mutex == NO_MUTEX)
    return(EINVAL);

  /*
   * free mutex only if not locked and not associated with any cond var
   */
#ifdef NOERR_CHECK
  if (pthread_test_and_set(&mutex->lock))
    return(EBUSY);
#else /* !NOERR_CHECK */
  SET_KERNEL_FLAG;
  if (mutex->lock) {
    CLEAR_KERNEL_FLAG;
    return(EBUSY);
  }
  CLEAR_KERNEL_FLAG;
#endif /* !NOERR_CHECK */

  mutex->flags = 0;

#ifdef TDI_SUPPORT
  TdiMutexUnregister(mutex);
#endif
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_setprioceiling - locks the mutex, changes the
 * mutex's priority ceiling and releases the mutex.
 */
int pthread_mutex_setprioceiling(mutex, prioceiling)
pthread_mutex_t *mutex;
int prioceiling;
{
int oldprio;

  if (mutex == NO_MUTEX || !mutex->flags)
    return(ESRCH);
#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (prioceiling >= MIN_PRIORITY && prioceiling <= MAX_PRIORITY) {
    if (!pthread_mutex_trylock(mutex)) {
      oldprio = mutex->prioceiling;
      mutex->prioceiling = prioceiling;
      pthread_mutex_unlock(mutex);
      return(oldprio);
    }
    else
      return(EPERM);
  }
  else
    return(EINVAL);
#else
  return(ENOTSUP);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutex_getprioceiling - Returns the current priority
 * ceiling of the mutex in "prioceiling".
 */
int pthread_mutex_getprioceiling(mutex, prioceiling)
pthread_mutex_t mutex;
int *prioceiling;
{

  if (!mutex.flags || !prioceiling)
    return(ESRCH);
  
#ifdef _POSIX_THREADS_PRIO_PROTECT
  if (mutex.prioceiling >= MIN_PRIORITY &&
      mutex.prioceiling <= MAX_PRIORITY) {
    *prioceiling = mutex.prioceiling;
    return(0);
  }
  else
    return(EINVAL);
#else
  return(ENOTSUP);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_init - Initializes the mutex attribute object
 * with default values.
 */
int pthread_mutexattr_init(attr)
pthread_mutexattr_t *attr;
{
  if (!attr)
 
    return(EINVAL);

  attr->flags = TRUE;
#ifdef _POSIX_THREADS_PRIO_PROTECT
  attr->prioceiling = DEFAULT_PRIORITY;
  attr->protocol = PTHREAD_PRIO_NONE;
#endif
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_destroy - Destroys the mutex attribute object.
 */
int pthread_mutexattr_destroy(attr)
pthread_mutexattr_t *attr;
{

  if (!attr || !attr->flags)
    return(EINVAL);
  attr->flags = FALSE;
#ifdef _POSIX_THREADS_PRIO_PROTECT
  attr->prioceiling = DEFAULT_PRIORITY;
  attr->protocol = PTHREAD_PRIO_NONE;
#endif  
  return(0);
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_setprotocol - Sets the protocol (value can be
 * PTHREAD_PRIO_NONE, PTHREAD_PRIO_INHERIT or PTHREAD_PRIO_PROTECT)
 * for the mutex attr.
 */
int pthread_mutexattr_setprotocol(attr,protocol)
pthread_mutexattr_t *attr;
pthread_protocol_t protocol;
{
  
  if (!attr || !attr->flags)
    return(EINVAL);
#ifndef _POSIX_THREADS_PRIO_PROTECT
  return(ENOSYS);
#else
  if (protocol < PTHREAD_PRIO_NONE || protocol > PTHREAD_PRIO_PROTECT)
    return(EINVAL);
  if (protocol == PTHREAD_PRIO_INHERIT)
    return(ENOTSUP);
  attr->protocol = protocol;
  return(0);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_getprotocol - Gets the current protcol.
 */
int pthread_mutexattr_getprotocol(attr, protocol)
pthread_mutexattr_t *attr;
int *protocol;
{
  if (!attr || !attr->flags || !protocol)
    return(EINVAL);
#ifdef _POSIX_THREADS_PRIO_PROTECT
  *protocol = attr->protocol;
  return(0);
#else
  return(ENOSYS);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_setprioceiling - Sets the priority ceiling
 * for the mutex attribute object.
 */
int pthread_mutexattr_setprioceiling(attr, prioceiling)
pthread_mutexattr_t *attr;
int prioceiling;
{
  if (!attr || !attr->flags)
    return(EINVAL);
#ifndef _POSIX_THREADS_PRIO_PROTECT
  return(ENOSYS);
#else
  if (prioceiling > MAX_PRIORITY || prioceiling < MIN_PRIORITY)
    return(EINVAL);
  attr->prioceiling = prioceiling;
  return(0);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_getprioceiling - returns the current priority
 * ceiling of the mutex attribute object.
 */
int pthread_mutexattr_getprioceiling(attr, prioceiling)
pthread_mutexattr_t *attr;
int *prioceiling;
{
  if (!attr || !attr->flags || !prioceiling)
    return(EINVAL);
#ifdef _POSIX_THREADS_PRIO_PROTECT
  *prioceiling = attr->prioceiling;
  return(0);
#else
  return(ENOSYS);
#endif
}

/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_getpshared - Not Implemented. Returns ENOSYS.
 */
 
int pthread_mutexattr_getpshared(attr, pshared)
pthread_mutexattr_t *attr;
int *pshared;
{
  return(ENOSYS);
}
 
/*------------------------------------------------------------*/
/*
 * pthread_mutexattr_setpshared - Not Implemented. Returns ENOSYS.
 */
 
int pthread_mutexattr_setpshared(attr, pshared)
pthread_mutexattr_t *attr;
int pshared;
{
  return(ENOSYS);
}
