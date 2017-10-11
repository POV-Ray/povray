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

  @(#)sigsetjmp.S	3.14 11/8/00

*/

#include <SYS.h>

/*
 * _pthread_sigsetjmp, _pthread_siglongjmp
 *
 *	pthread_siglongjmp(a,v,restore_float)
 * will generate a "return(v)" from the last call to
 *	pthread_sigsetjmp(a,m,save_float)
 * by restoring registers from the environment 'a'.
 * The previous signal state is NOT restored.
 * The state of the floating point unit is saved.
 */


	.global	_setjmp
_setjmp:
	popl	%eax
	popl	%edx
	pushl	$0
	pushl	$0
	pushl	%edx
	pushl	%eax
	jmp	1f

	.global	_sigsetjmp
_sigsetjmp:
	popl	%eax
	popl	%edx
	popl	%ecx
	pushl	$0
	pushl	%ecx
	pushl	%edx
	pushl	%eax

	.global	_pthread_sigsetjmp
_pthread_sigsetjmp:
	movl	4(%esp),%eax
        movl    $0,24(%eax)
        cmpl    $0,8(%esp)
        je      1f
        movl    $1,24(%eax)
        addl    $28,%eax
        pushl   %eax
        pushl   $0
        pushl   $0
        call    _sigprocmask
        addl    $12,%esp
1:      movl    4(%esp),%eax
        movl    %ebx, 4(%eax)
        movl    %esi,16(%eax)
        movl    %edi,20(%eax)
        movl    %ebp,12(%eax)
        popl    %edx
        movl    %esp, 8(%eax)
        movl    %edx, 0(%eax)
	cmpl	$0,8(%esp)
	je	2f
	fnsave	32(%eax)
2:
        subl    %eax,%eax
        jmp     *%edx

	.global	_siglongjmp
_siglongjmp:
	popl	%eax
	popl	%edx
	popl	%ecx
	pushl	$0
	pushl	%ecx
	pushl	%edx
	pushl	%eax

	.global _pthread_siglongjmp
_pthread_siglongjmp:
        pushl   %ebp
        movl    %esp,%ebp
        movl    8(%ebp),%eax
        cmpl    $0,24(%eax)
        je      1f
        pushl   $0
        addl    $28,%eax
        pushl   %eax
        pushl   $0
        call    _sigprocmask
        movl    %ebp,%esp
1:
	pushl   16(%ebp)
	pushl   12(%ebp)
        pushl   8(%ebp)
        call    _pthread_longjmp
        leave
        ret

	.global	_longjmp
_longjmp:
	popl	%eax
	popl	%edx
	popl	%ecx
	pushl	$0
	pushl	%ecx
	pushl	%edx
	pushl	%eax

	.global _pthread_longjmp
_pthread_longjmp:
        movl    4(%esp),%edx
        movl    8(%esp),%eax
	movl	12(%esp),%ecx
        cmpl    12(%edx),%ebp
        je      1f
        movl    4(%edx),%ebx
        movl    16(%edx),%esi
        movl    20(%edx),%edi
        movl    12(%edx),%ebp
1:      movl    8(%edx),%esp
	testl	%ecx,%ecx
	jz	2f
	frstor	32(%edx)
2:
	testl	%eax,%eax
	jnz	3f
	incl	%eax
3:	jmp     *0(%edx)
