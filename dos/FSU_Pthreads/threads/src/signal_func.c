/* Copyright (C) 1992, the Florida State University
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

  @(#)signal_func.c	1.1 2/3/99

*/

/* 
 * Function signal, separated from signal.c to allow overwriting
 */

#include "internals.h"

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
#else !SOLARIS || !__dos__
  act.sa_mask = 0;
#endif !SOLARIS
#if defined(__linux__) || defined(__FreeBSD__) || defined(_M_UNIX)
  act.sa_flags = SA_ONESHOT | SA_NOMASK;
#endif
#ifdef __dos__
  act.sa_flags = 0;
#endif
#ifdef __linux__ 
  act.sa_restorer = NULL;
#endif __linux__ || __FreeBSD__ || _M_UNIX
  if (!sigaction(sig, &act, (struct sigaction *) NULL))
    return(handler);
  else
    return((pthread_sighandler_t)-get_errno());
}
#endif

