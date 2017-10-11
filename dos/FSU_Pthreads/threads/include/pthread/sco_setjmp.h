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

  @(#)sco_setjmp.h	3.14 11/8/00

*/

#ifndef _PTHREAD_SETJMP_H_
#define _PTHREAD_SETJMP_H_

#ifdef __cplusplus
extern "C" {
#endif 

#if defined(SCO5_NP)
typedef int pthread_sigjmp_buf[_SIGJBLEN];
#else
typedef int pthread_sigjmp_buf[68];
#endif

extern int pthread_sigsetjmp (pthread_sigjmp_buf, int, int);
extern void pthread_siglongjmp (pthread_sigjmp_buf, int, int);

#define SYS_SIGSETJMP   pthread_sigsetjmp
#define SYS_SIGLONGJMP  pthread_siglongjmp
#define SYS_SIGJMP_BUF  pthread_sigjmp_buf

#ifdef __cplusplus
};
#endif 

#endif /* !_PTHREAD_SETJMP_H_ */
