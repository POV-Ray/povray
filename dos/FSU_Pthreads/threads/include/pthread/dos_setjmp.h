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

  @(#)dos_setjmp.h	1.1 07 Apr 1996

*/

#ifndef _PTHREAD_SETJMP_H_
#define _PTHREAD_SETJMP_H_

#ifndef _ANSI_SOURCE
#include <pthread/signal.h>

typedef int pthread_sigjmp_buf[44];  /* see src/dos/setjmp.S */
#endif /* !_ANSI_SOURCE */

#include <sys/cdefs.h>

__BEGIN_DECLS


/* For DOS, the libc exception handler routine must be free to call its
   own setjmp/longjmp functions--which use a different format buffer from
   ours.  Thus, we must not replace the existing setjmp/longjmp functions.

   We kludge our way around this by using macros to redefine the function
   names.  This is a potential sore spot if linked with binaries that do
   not include pthread.h! 

   These macros are also defined in src/dos/setjmp.S. */

#define setjmp pt_setjmp
#define longjmp pt_longjmp
#define sigsetjmp pt_sigsetjmp
#define siglongjmp pt_siglongjmp


#ifndef _ANSI_SOURCE
int     pthread_sigsetjmp __P((pthread_sigjmp_buf, int, int));
void    pthread_siglongjmp __P((pthread_sigjmp_buf, int, int));

#define SYS_SIGSETJMP   pthread_sigsetjmp
#define SYS_SIGLONGJMP  pthread_siglongjmp
#define SYS_SIGJMP_BUF  pthread_sigjmp_buf
#endif /* !_ANSI_SOURCE */
__END_DECLS

#endif /* !_PTHREAD_SETJMP_H_ */
