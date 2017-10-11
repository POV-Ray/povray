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

  @(#)sigprocmask.S	3.14 11/8/00

*/

#include <SYS.h>

xerr:
	jmp	_pthread_cerror

	.globl  _pthread_sigprocmask
_pthread_sigprocmask:
	movl	8(%esp),%ecx		# fetch new sigset pointer
	cmpl	$0,%ecx			# check new sigset pointer
	jne	1f			# if not null, indirect
/*	movl	$0,8(%esp)		# null mask pointer: block empty set */
	movl	$1,4(%esp)		# SIG_BLOCK
	jmp	2f
1:	movl	(%ecx),%ecx		# fetch indirect  ...
	movl	%ecx,8(%esp)		# to new mask arg
2:	movl	$ SYS_sigprocmask , %eax
	LCALL(0x7,0)
	jb	xerr
	movl	12(%esp),%ecx		# fetch old mask requested
	cmpl	$0,%ecx			# test if old mask requested
	je	out
	movl	%eax,(%ecx)		# store old mask
out:
	xorl	%eax,%eax
	ret
