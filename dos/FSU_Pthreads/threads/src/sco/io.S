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

  @(#)io.S	3.14 11/8/00

*/

	.text
	.align  2

	.globl	NAME(read)
NAME(read):
        movl    $3,%eax
        lcall   $7, $0
	jae	1f
	SET_ERRNO()
1:
	ret
        END(read)

	.globl NAME(write)
NAME(write):
        movl    $4,%eax
        lcall   $7, $0
	jae	1f
	SET_ERRNO()
1:
	ret
        END(write)

	.globl NAME(ioctl)
NAME(ioctl):
        movl    $0x36,%eax
        lcall   $7, $0
	jae	1f
	SET_ERRNO()
1:
	ret
        END(ioctl)

	.globl NAME(select)
NAME(select):
        movl    $0x2428,%eax
        lcall	$7, $0
	jae	1f
	SET_ERRNO()
1:
	ret
        END(select)

	.globl NAME(poll)
NAME(poll):
        movl    $0x57,%eax
        lcall	$7, $0
	jae	1f
	SET_ERRNO()
1:
	ret
        END(poll)

	.globl NAME(getmsg)
NAME(getmsg):
	movl	$0x55,%eax
	lcall   $7, $0
	jae	1f
	SET_ERRNO()
1:
	ret
        END(getmsg)
