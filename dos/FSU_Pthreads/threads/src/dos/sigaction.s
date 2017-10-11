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

  @(#)sigaction.s	3.14 11/8/00

*/

/* These are some simple hooks into the system library.  This is
   mainly a workaround for the name conflicts--we really want to call
   the system functions by their proper names, but we've redefined
   their names using a #define macro so we can't do it directly from C.
   Fortunately, since C function names are prepended with an underscore,
   we can call them directly from the assembly code. */

	.global	NAME(pthread_sigaction)
NAME(pthread_sigaction):
	jmp _sigaction

	.global	NAME(pthread_sigprocmask)
NAME(pthread_sigprocmask):
	jmp _sigprocmask

	.global	NAME(pthread_sigsuspend)
NAME(pthread_sigsuspend):
	jmp _sigsuspend

