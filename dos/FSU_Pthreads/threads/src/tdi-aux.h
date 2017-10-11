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

  @(#)tdi-aux.h	3.14 11/8/00

*/

#ifndef _tdi_aux_h
#define _tdi_aux_h

/* implementation type */

extern int __pthread_debug_with_TDI;

/* this is for dis-/enabling signal handling *
 * by the debugger                           */

extern int __pthread_debug_TDI_sig_ignore; 

/* this is the mask of all signals arrived *
 * during TDI invocation                   */
  
extern int __pthread_debug_TDI_ignored_signals;


/* -------------------------------------------------------- *
 * in this implementation we register pthread_t's, because  *
 * there are pointers                                       *
 * -------------------------------------------------------- */

extern int TdiThreadRegister     (void*);
extern int TdiThreadUnregister   (void*);
extern int TdiThreadIsRegistered (void*);

extern int TdiMutexRegister      (void*);
extern int TdiMutexUnregister    (void*);
extern int TdiMutexIsRegistered  (void*);

extern int TdiCondRegister       (void*);
extern int TdiCondUnregister     (void*);
extern int TdiCondIsRegistered   (void*);

#endif
