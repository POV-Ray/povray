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

  @(#)fd.h	3.14 11/8/00

*/

#ifndef MACRO_BEGIN

#define MACRO_BEGIN     do {

#ifndef lint
#define MACRO_END       } while (0)
#else /* lint */
extern int _NEVER_;
#define MACRO_END       } while (_NEVER_)
#endif /* lint */

#endif /* !MACRO_BEGIN */

#define OK 0
#define NOTOK -1

typedef struct count_sem {
  pthread_mutex_t lock;
  pthread_cond_t  cond;
  int             count;
} count_sem_t;

typedef struct bin_sem {
  pthread_mutex_t lock;
  pthread_cond_t  cond;
  int             flag;
} bin_sem_t;

typedef struct fd_queue_elem { 
  int                    fd;
  int                    count;
  bin_sem_t              sem;
  struct fd_queue_elem   *next;
} fd_queue_elem_t;

typedef struct fd_queue {
  fd_queue_elem_t * head;
  fd_queue_elem_t * tail;
} * fd_queue_t;

/*
 * New functions
 */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(PTHREAD_KERNEL)

#endif


#ifdef __cplusplus
};
#endif

