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

  @(#)stdtypes.h	3.14 11/8/00

*/

#ifndef _pthread_stdtypes_h
#define _pthread_stdtypes_h

#if !defined(__FreeBSD__) && !defined(_M_UNIX) && !defined(__linux__) && !defined(__dos__)
/* we don't want any of these since we already have them
 */
#ifndef	__sys_stdtypes_h
#define	__sys_stdtypes_h

#if defined(STAND_ALONE_NP) && defined(sun4e_NP)
#ifndef	_TYPES_
#define	_TYPES_

/*
 * substitutes used types of #include <sys/types.h>
 * to avoid wrong definition of size_t
 */

typedef unsigned char u_char;
typedef unsigned int u_int;
typedef	char *	caddr_t;
typedef char *  addr_t;

#endif /* !_TYPES_ */
#endif /* STAND_ALONE_NP && sun4e_NP */

typedef	unsigned int	sigset_t;	/* signal mask - may change */

typedef	unsigned int	speed_t;	/* tty speeds */
typedef	unsigned long	tcflag_t;	/* tty line disc modes */
typedef	unsigned char	cc_t;		/* tty control char */
typedef	int		pid_t;		/* process id */

typedef	unsigned short	mode_t;		/* file mode bits */
typedef	short		nlink_t;	/* links to a file */

typedef	long		clock_t;	/* units=ticks (typically 60/sec) */
typedef	long		time_t;		/* value = secs since epoch */

#ifdef __GNUC__
typedef	long unsigned int	size_t;		/* ??? */
#else /* !__GNUC__ */
typedef	unsigned int	size_t;		/* ??? */
#endif /* !__GNUC__ */
typedef int		ptrdiff_t;	/* result of subtracting two pointers */

typedef	unsigned short	wchar_t;	/* big enough for biggest char set */

#endif /* !__sys_stdtypes_h */
#elif defined (_M_UNIX)
#include <sys/types.h>
#endif

#endif /* !_pthread_stdtypes_h */
