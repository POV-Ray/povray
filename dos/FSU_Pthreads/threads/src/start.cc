/* Copyright (C) 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999 the Florida State University
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

  @(#)start.cc	3.8 2/3/99

*/

/*
 * automatic initialization of Pthreads in C++
 */

#ifdef AUTO_INIT
extern "C" void pthread_init (void);

struct __pthread_init_hack_t
{
  __pthread_init_hack_t () { pthread_init (); }
};

__pthread_init_hack_t __pthread_init_hack_x;
char __pthread_init_hack = 42;
#endif
