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

  @(#)sigaction.S	3.14 11/8/00

*/

	.globl	pthread_sigaction
pthread_sigaction:
	movl	$0x2728,%eax
#ifdef _SCO_ELF
	pushl	%ebx
	call	.L32
.L32:
	popl	%ebx
	addl	$_GLOBAL_OFFSET_TABLE_+[.-.L32],%ebx
	movl	pthread_sigreturn@GOT(%ebx),%edx
	popl	%ebx
#else
	movl	$pthread_sigreturn,%edx
#endif
        lcall   $7, $0
	jae	1f
	SET_ERRNO()
	ret
1:
	xorl	%eax, %eax
	ret
        END0(pthread_sigaction)

	.globl	pthread_sigreturn
pthread_sigreturn:
        addl    $4,%esp
        lcall   $0xf, $0
        END0(pthread_sigreturn)
