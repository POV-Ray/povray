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

  @(#)mutex.h	3.14 11/8/00

*/

#ifndef _pthread_mutex_h
#define _pthread_mutex_h
    
/*
 * Macros to lock and unlock a mutex
 * precondition: SET_KERNEL_FLAG
 * postcondition: call CLEAR_KERNEL_FLAG
 */

#ifdef _POSIX_THREADS_PRIO_PROTECT
#ifdef SRP
#define mac_change_lock_prio(mutex, p) \
  MACRO_BEGIN \
    if (mutex->protocol == PTHREAD_PRIO_PROTECT) { \
      mutex->prev_max_ceiling_prio = p->max_ceiling_prio; \
      p->max_ceiling_prio = MAX(mutex->prioceiling, p->max_ceiling_prio); \
      if (p->attr.param.sched_priority != MAX(p->max_ceiling_prio, p->base_prio)) { \
        p->attr.param.sched_priority = MAX(p->max_ceiling_prio, p->base_prio); \
        if (p != ready.head) { \
          pthread_q_deq(&ready, p, PRIMARY_QUEUE); \
          pthread_q_primary_enq(&ready, p); \
	} \
      } \
    } \
  MACRO_END
#endif
#else
#define mac_change_lock_prio(mutex, p)
#endif

#ifdef _POSIX_THREADS_PRIO_PROTECT
#ifdef SRP
#define mac_change_unlock_prio_wake(mutex, p, locked) \
  MACRO_BEGIN \
    if (mutex->protocol == PTHREAD_PRIO_PROTECT) { \
      p->max_ceiling_prio = mutex->prev_max_ceiling_prio; \
      p->attr.param.sched_priority = MAX(p->max_ceiling_prio, p->base_prio); \
      if (locked) { \
        pthread_t t = mutex->queue.head; \
        if (t->new_prio != MUTEX_WAIT) \
          t->base_prio = t->new_prio; \
        t->new_prio = NO_PRIO; \
        pthread_q_wakeup(&mutex->queue, PRIMARY_QUEUE); \
        mac_change_lock_prio(mutex, t); \
      } \
      if (ready.head != ready.tail) \
        pthread_mutex_q_adjust(p); \
    } \
    else \
      if (locked) \
        pthread_q_wakeup(&mutex->queue, PRIMARY_QUEUE); \
  MACRO_END
#endif
#else
#define mac_change_unlock_prio_wake(mutex, p, locked) \
  MACRO_BEGIN \
    if (locked) \
      pthread_q_wakeup(&mutex->queue, PRIMARY_QUEUE); \
  MACRO_END
#endif

#ifdef _POSIX_THREADS_PRIO_PROTECT
#ifdef SRP
#define mac_mutex_wait(mutex, p) \
  MACRO_BEGIN \
    if (mutex->protocol == PTHREAD_PRIO_PROTECT) \
      p->new_prio = MUTEX_WAIT; \
  MACRO_END
#endif
#else
#define mac_mutex_wait(mutex, p)
#endif

#ifdef NOERR_CHECK
#ifdef _POSIX_THREADS_PRIO_PROTECT
#define mac_mutex_noerr_lock(mutex,p) \
  MACRO_BEGIN \
    if (mutex->protocol == PTHREAD_PRIO_NONE) \
      pthread_q_sleep(&mutex->queue, PRIMARY_QUEUE); \
    else { \
      if (!mutex->lock) { \
        mutex->lock = TRUE; \
        mutex->owner = p; \
        mac_change_lock_prio(mutex, p); \
      } \
      else { \
        mac_mutex_wait(mutex, p); \
        pthread_q_sleep_thread(&mutex->queue, p, PRIMARY_QUEUE); \
      } \
    } \
  MACRO_END
#else
#define mac_mutex_noerr_lock(mutex,p) \
  MACRO_BEGIN \
    pthread_q_sleep(&mutex->queue, PRIMARY_QUEUE); \
  MACRO_END
#endif
#endif

/*
 * locking mutex doesn't need test_and_set; it's protected by kernel flag
 */
#ifdef NOERR_CHECK
#define mac_mutex_lock(mutex, p) \
  MACRO_BEGIN \
    mac_mutex_noerr_lock(mutex,p); \
  MACRO_END
#else
#ifdef MUT_SWITCH
#define mac_mutex_lock(mutex, p) \
  MACRO_BEGIN \
    if (!mutex->lock) { \
      mutex->lock = TRUE; \
      mutex->owner = p; \
      mac_change_lock_prio(mutex, p); \
      pthread_q_deq(&ready, p, PRIMARY_QUEUE); \
      pthread_q_primary_enq(&ready, p); \
    } \
    else { \
      mac_mutex_wait(mutex, p); \
      pthread_q_sleep_thread(&mutex->queue, p, PRIMARY_QUEUE); \
    } \
  MACRO_END
#else
#define mac_mutex_lock(mutex, p) \
  MACRO_BEGIN \
    if (!mutex->lock) { \
      mutex->lock = TRUE; \
      mutex->owner = p; \
      mac_change_lock_prio(mutex, p); \
    } \
    else { \
      mac_mutex_wait(mutex, p); \
      pthread_q_sleep_thread(&mutex->queue, p, PRIMARY_QUEUE); \
    } \
  MACRO_END
#endif
#endif

#ifdef NOERR_CHECK
#ifdef _POSIX_THREADS_PRIO_PROTECT
#define mac_mutex_noerr_unlock(mutex, p, cmd) \
  MACRO_BEGIN \
    if (mutex->protocol == PTHREAD_PRIO_NONE) { \
      cmd; \
      mutex->owner = mutex->queue.head; \
      pthread_q_wakeup(&mutex->queue, PRIMARY_QUEUE); \
    } \
    else { \
      if (mutex->queue.head != NO_PTHREAD) { \
        cmd; \
        mutex->owner = mutex->queue.head; \
        mac_change_unlock_prio_wake(mutex, p, TRUE); \
      } \
      else { \
        cmd; \
        mutex->owner = NO_PTHREAD; \
        mutex->lock = FALSE; \
        mac_change_unlock_prio_wake(mutex, p, FALSE); \
      } \
    } \
  MACRO_END
#else
#define mac_mutex_noerr_unlock(mutex, p, cmd) \
  MACRO_BEGIN \
    cmd; \
    mutex->owner = mutex->queue.head; \
    pthread_q_wakeup(&mutex->queue, PRIMARY_QUEUE); \
  MACRO_END
#endif
#endif

/*
 * third parameter is any C statement to be executed before pthread_q_wakeup()
 * preconditions: current thread owns the mutex, SET_KERNEL_FLAG
 * postcondition: call CLEAR_KERNEL_FLAG
 */
#ifdef NOERR_CHECK
#define mac_mutex_unlock(mutex, p, cmd) \
  MACRO_BEGIN \
    mac_mutex_noerr_unlock(mutex, p, cmd); \
  MACRO_END
#else
#define mac_mutex_unlock(mutex, p, cmd) \
  MACRO_BEGIN \
    if (mutex->queue.head != NO_PTHREAD) { \
      cmd; \
      mutex->owner = mutex->queue.head; \
      mac_change_unlock_prio_wake(mutex, p, TRUE); \
    } \
    else { \
      cmd; \
      mutex->owner = NO_PTHREAD; \
      mutex->lock = FALSE; \
      mac_change_unlock_prio_wake(mutex, p, FALSE); \
    } \
  MACRO_END
#endif

#endif /* !_pthread_mutex_h */
