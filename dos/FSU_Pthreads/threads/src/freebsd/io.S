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

  @(#)io.S	3.14 11/8/00

*/

#include <SYS.h>

ioerr:
	jmp	_pthread_cerror

	.globl  _pthread_sys_read
_pthread_sys_read:
	movl	$ SYS_read , %eax
	LCALL(0x7,0)
	jb	ioerr
	ret

	.globl  _pthread_sys_write
_pthread_sys_write:
	movl	$ SYS_write , %eax
	LCALL(0x7,0)
	jb	ioerr
	ret

		.globl  _pthread_sys_readv
_pthread_sys_readv:
	movl	$ SYS_readv , %eax
	LCALL(0x7,0)
	jb	ioerr
	ret

	.globl  _pthread_sys_writev
_pthread_sys_writev:
	movl	$ SYS_writev , %eax
	LCALL(0x7,0)
	jb	ioerr
	ret
