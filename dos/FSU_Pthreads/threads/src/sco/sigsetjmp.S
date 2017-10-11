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

  @(#)sigsetjmp.S	3.14 11/8/00

*/

/*
 * pthread_sigsetjmp, pthread_siglongjmp
 *
 *	pthread_siglongjmp(a,v,restore_float)
 * will generate a "return(v)" from the last call to
 *	pthread_sigsetjmp(a,m,save_float)
 * by restoring registers from the environment 'a'.
 * The previous signal state is NOT restored.
 * The state of the floating point unit is saved.
 */


	.global	setjmp
setjmp:
        movl    4(%esp),%eax
        movl    %ebx,0(%eax)
        movl    %esi,4(%eax)
        movl    %edi,8(%eax)
        movl    %ebp,12(%eax)
        popl    %edx
        movl    %esp,16(%eax)
        movl    %edx,20(%eax)
        subl    %eax,%eax
        jmp     *%edx
        END0(setjmp)

	.global	sigsetjmp
sigsetjmp:
	movl	4(%esp),%eax
        movl    $0,24(%eax)
        cmpl    $0,8(%esp)
        je      1f
#ifdef _SCO_ELF
	pushl	%ebx
#endif
        movl    $1,24(%eax)
        addl    $28,%eax
        pushl   %eax
        pushl   $0
        pushl   $0
#ifdef _SCO_ELF
	call	.L1
.L1:
	popl	%ebx
	addl    $_GLOBAL_OFFSET_TABLE_+[.-.L1],%ebx
        call    sigprocmask@PLT
#else
	call	sigprocmask
#endif
        addl    $12,%esp
#ifdef _SCO_ELF
	popl	%ebx
#endif
1:      movl    4(%esp),%eax
        movl    %ebx,0(%eax)
        movl    %esi,4(%eax)
        movl    %edi,8(%eax)
        movl    %ebp,12(%eax)
        popl    %edx
        movl    %esp,16(%eax)
        movl    %edx,20(%eax)
        subl    %eax,%eax
        jmp     *%edx
        END0(sigsetjmp)

	.global	pthread_sigsetjmp
pthread_sigsetjmp:
	movl	4(%esp),%eax
        movl    $0,24(%eax)
        cmpl    $0,8(%esp)
        je      1f
#ifdef _SCO_ELF
	pushl	%ebx
#endif
        movl    $1,24(%eax)
        addl    $28,%eax
        pushl   %eax
        pushl   $0
        pushl   $0
#ifdef _SCO_ELF
	call	.L2
.L2:
	popl	%ebx
	addl    $_GLOBAL_OFFSET_TABLE_+[.-.L2],%ebx
        call    sigprocmask@PLT
#else
        call    sigprocmask
#endif
        addl    $12,%esp
#ifdef _SCO_ELF
	popl	%ebx
#endif
1:      movl    4(%esp),%eax
        movl    %ebx,0(%eax)
        movl    %esi,4(%eax)
        movl    %edi,8(%eax)
        movl    %ebp,12(%eax)
	cmpl	$0,12(%esp)
	je	2f
	fnsave	32(%eax)
2:
        popl    %edx
        movl    %esp,16(%eax)
        movl    %edx,20(%eax)
        subl    %eax,%eax
        jmp     *%edx
        END0(pthread_sigsetjmp)

	.global	longjmp
longjmp:
        movl    4(%esp),%edx
        movl    8(%esp),%eax
        movl    0(%edx),%ebx
        movl    4(%edx),%esi
        movl    8(%edx),%edi
        movl    12(%edx),%ebp
        movl    16(%edx),%esp
	testl	%eax,%eax
	jnz	1f
	incl	%eax
1:	jmp     *20(%edx)
	END0(longjmp)

	.global	siglongjmp
siglongjmp:
	pushl   %ebp
	movl	%esp, %ebp
	movl	8(%ebp), %edx
	cmpl	$0, 24(%edx)
	jz	1f
#ifdef _SCO_ELF
	pushl	%ebx
#endif
        pushl   $0
        addl    $28,%edx
        pushl   %edx
        pushl   $0
#ifdef _SCO_ELF
	call	.L3
.L3:
	popl	%ebx
	addl    $_GLOBAL_OFFSET_TABLE_+[.-.L3],%ebx
	call	sigprocmask@PLT
#else
	call	sigprocmask
#endif
        addl    $12,%esp
#ifdef _SCO_ELF
	popl	%ebx
#endif
1:
#ifdef _SCO_ELF
	pushl	%ebx
#endif
	pushl	12(%ebp)
	pushl	8(%ebp)
#ifdef _SCO_ELF
	call	.L31
.L31:
	popl	%ebx
	addl    $_GLOBAL_OFFSET_TABLE_+[.-.L31],%ebx
	call    longjmp@PLT
#else
	call    longjmp
#endif
        addl    $8,%esp
#ifdef _SCO_ELF
	popl	%ebx
#endif
        movl    %ebp, %esp
        popl    %ebp
        ret
        END0(siglongjmp)

	.global pthread_siglongjmp
pthread_siglongjmp:
	pushl   %ebp
	movl	%esp, %ebp
	movl	8(%ebp), %edx
	cmpl	$0, 24(%edx)
	jz	1f
#ifdef _SCO_ELF
	pushl	%ebx
#endif
        pushl   $0
        addl    $28,%edx
        pushl   %edx
        pushl   $0
#ifdef _SCO_ELF
	call	.L4
.L4:
	popl	%ebx
	addl    $_GLOBAL_OFFSET_TABLE_+[.-.L4],%ebx
	call	sigprocmask@PLT
#else
	call	sigprocmask
#endif
        addl    $12,%esp
#ifdef _SCO_ELF
	popl	%ebx
#endif
1:
	cmpl	$0, 16(%ebp)
	jz	2f
	movl	8(%ebp), %edx
	frstor	32(%edx)
2:
#ifdef _SCO_ELF
	pushl	%ebx
#endif
	pushl	12(%ebp)
	pushl	8(%ebp)
#ifdef _SCO_ELF
	call	.L5
.L5:
	popl	%ebx
	addl    $_GLOBAL_OFFSET_TABLE_+[.-.L5],%ebx
	call    longjmp@PLT
#else
	call    longjmp
#endif
        addl    $8,%esp
#ifdef _SCO_ELF
	popl	%ebx
#endif
        movl    %ebp, %esp
        popl    %ebp
        ret
        END0(pthread_siglongjmp)
