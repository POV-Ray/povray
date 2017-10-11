/* Copyright (C) 1992-2000 1993, 1994, 1995, 1996, 1997, 1998 the Florida State University
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

  @(#)cerror.S	3.14 11/8/00

*/

#ifdef _SCO_ELF

#define NAME(X) 	pthread_sys_##X
#define	SET_ERRNO() \
	pushl %ebx; \
	call 2f; \
2: \
	popl %ebx; \
	addl $_GLOBAL_OFFSET_TABLE_+[.-2b], %ebx; \
	movl errno@GOT(%ebx), %edx; \
	movl %eax, (%edx); \
	movl $-1,%eax; \
	popl %ebx
	
#define END(X)		1: ; .type NAME(X),@function ; .size NAME(X),1b - NAME(X)
#define END0(X)		1: ; .type X,@function ; .size X,1b - X

#else

#define NAME(X)		pthread_sys_##X
#define END(X)
#define END0(X)
#define	SET_ERRNO() \
	movl    %eax, errno; \
	movl	$-1,%eax

#endif

#ifdef __STDC__

#define	SYSCALL(x)					\
	.globl NAME(x);		\
									\
NAME(x):				\
	movl $(SYS_##x##), %eax;		\
	lcall	$7, $0; \
	jae	1f;							\
	cmp	$(ERESTART), %eax; \
	je	NAME(x); \
	neg		%eax;	\
1: \
	ret;							\
\
	END(x)


#else

#define	SYSCALL(x)					\
	.globl NAME(x);		\
									\
NAME(x):				\
									\
	movl $(SYS_/**/x), %eax;		\
	lcall	$7, $0; \
	jb	1b;							\
	ret;							\
	END(x)

#endif

	.globl	errno
