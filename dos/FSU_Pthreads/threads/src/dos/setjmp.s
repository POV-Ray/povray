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

  @(#)setjmp.s	3.14 11/8/00

*/


/* For DOS, the libc exception handler routine must be free to call its
   own setjmp/longjmp functions--which use a different format buffer from
   ours.  Thus, we must not replace the existing setjmp/longjmp functions.

   We kludge our way around this by using macros to redefine the function
   names.  This is a potential sore spot if linked with binaries that do
   not include pthread.h! 

   These macros are also defined in include/pthread/dos_setjmp.h. */

#define setjmp pt_setjmp
#define longjmp pt_longjmp
#define sigsetjmp pt_sigsetjmp
#define siglongjmp pt_siglongjmp

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

/* setjmp buffer stores the following information:
	0 %ebx
	1 %esi
	2 %edi
	3 %ebp
	4 %esp
	5 %eip
	6 1 if signal mask saved, 0 otherwise
    7..16 signal mask (10 words needed, 320 signals.. per dj's signal.h)
   17..43 floating point state (27 words needed, 108 bytes)
*/



	.global	NAME(pt_setjmp)
NAME(pt_setjmp):
	popl	%eax
	popl	%edx
	pushl	$0
	pushl	$0
	pushl	%edx
	pushl	%eax
	jmp	NAME(pthread_sigsetjmp)

	.global	NAME(pt_sigsetjmp)
NAME(pt_sigsetjmp):
	popl	%eax
	popl	%edx
	popl	%ecx
	pushl	$0
	pushl	%ecx
	pushl	%edx
	pushl	%eax

	.global	NAME(pthread_sigsetjmp)
NAME(pthread_sigsetjmp):		/* (buffer, savemask, savefp) */
	movl	4(%esp),%eax		/* %eax = buffer pointer */
        movl    $0,24(%eax)		/* buf[6] = 0 */
        cmpl    $0,8(%esp)		/* if (savemask) { */
        je      1f
        movl    $1,24(%eax)		/*   buf[6] = 1 (signal mask saved) */
        addl    $28,%eax
        pushl   %eax
        pushl   $0
        pushl   $0
        call    NAME(sigprocmask)	/*   sigprocmask(0, NULL, &buf[7]) */
        addl    $12,%esp
1:					/* } */
	movl    4(%esp),%eax
        movl    %ebx,0(%eax)		/* buf[0] = %ebx */
        movl    %esi,4(%eax)		/* buf[1] = %esi */
        movl    %edi,8(%eax)		/* buf[2] = %edi */
        movl    %ebp,12(%eax)		/* buf[3] = %ebp */
        popl    %edx			/* stack is clean */
        movl    %esp,16(%eax)		/* buf[4] = %esp */
        movl    %edx,20(%eax)		/* buf[5] = %eip */

	cmpl	$0,8(%esp)
	je	2f			/* if (savefp) */
	fnsave	68(%eax)		/*   buf[17..] = fp state */
2:					/* } */
        subl    %eax,%eax
        jmp     *%edx

	.global	NAME(pt_siglongjmp)
NAME(pt_siglongjmp):
	popl	%eax
	popl	%edx
	popl	%ecx
	pushl	$0
	pushl	%ecx
	pushl	%edx
	pushl	%eax

	.global NAME(pthread_siglongjmp)
NAME(pthread_siglongjmp):		/* (buffer, retval, restorefp) */
        pushl   %ebp
        movl    %esp,%ebp
        movl    8(%ebp),%eax		/* %eax = buffer pointer */
        cmpl    $0,24(%eax)
        je      1f			/* if (buf[6]) {
        pushl   $0
        addl    $28,%eax
        pushl   %eax
        pushl   $0
        call    NAME(sigprocmask)	/*   sigprocmask(0, &buf[7], NULL) */
        movl    %ebp,%esp
1:					/* } */
	pushl   16(%ebp)
	pushl   12(%ebp)
        pushl   8(%ebp)
        call    NAME(pthread_longjmp)	/* pthread_longjmp(...) */
        leave
        ret

	.global	NAME(pt_longjmp)
NAME(pt_longjmp):
	popl	%eax
	popl	%edx
	popl	%ecx
	pushl	$0
	pushl	%ecx
	pushl	%edx
	pushl	%eax

	.global NAME(pthread_longjmp)
NAME(pthread_longjmp):		/* (buffer, retval, restorefp) */
        movl    4(%esp),%edx		/* %edx = buffer pointer */
        movl    8(%esp),%eax		/* %eax = return value */
	movl	12(%esp),%ecx		
        cmpl    12(%edx),%ebp
        je      1f			/* if (%ebp != buf[3]) { */
        movl    0(%edx),%ebx		/*   %ebx = buf[0] */
        movl    4(%edx),%esi		/*   %esi = buf[1] */
        movl    8(%edx),%edi		/*   %edi = buf[2] */
        movl    12(%edx),%ebp		/*   %ebp = buf[3] */
1:					/* } */
        movl    16(%edx),%esp		/* %esp = buf[4] */
	testl	%ecx,%ecx		
	jz	2f			/* if (restorefp) { */
	frstor	68(%edx)		/*   fp_state = buf[17..] */
2:					/* } */
	testl	%eax,%eax		
	jnz	3f
	incl	%eax
3:
	jmp     *20(%edx)		/* %eip = buf[5] */
