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

  @(#)init.c	3.14 11/8/00

*/

#include "internals.h"
#include "setjmp.h"

#ifndef C_CONTEXT_SWITCH

#ifdef SOLARIS
#include <sys/frame.h>
#else /* !SOLARIS */
#include <sparc/frame.h>
#endif /* !SOLARIS */
/*
 * additional parameters passed on stack by fake call
 */
#define PARAM_OFFSET 0+2*sizeof(sigset_t)

#endif /* !C_CONTEXT_SWITCH */

extern pthread_body();
#ifdef STACK_CHECK
extern int pthread_page_size;
#endif

extern pthread_cond_t *new_cond[NNSIG];     /* cond for user handlers         */
extern struct context_t *new_scp[NNSIG];    /* info for user handlers         */

/*
 * Stack layout:
 * high +-----------+ + +
 *      |           | | | WINDOWSIZE (prev. window for callee save area)
 *      +-----------+ | +
 *      |           | PTHREAD_BODY_OFFSET (space for pthread_body())
 *      |           | |
 *      +-----------+ + start of user stack space
 *      |           | |
 *      |           | |
 *      |           | |
 *      |           | |
 *      |           | attr.stacksize (space for user functions)
 *      |           | |
 *      |           | |
 *      |           | |
 *      |           | |
 *      +-----------+ + end of user stack space
 *      | --------- | | +
 *      |  locked   | | pthread_page_size: locked page, bus error on overflow
 *      | --------- | | + PA(stack_base): next smallest page aligned address
 *      |           | 2*pthread_page_size
 *      |           | |
 * low  +-----------+ + stack_base
 *
 * ifdef STACK_CHECK:
 * Lock the closest page to the end of the stack to cause a bus error
 * (or and illegal instruction if no signal handler cannot be pushed
 * because the stack limit is exceeded, causes bad signal stack message)
 * on stack overflow. We allocate space conservatively (two pages) to
 * ensure that the locked page does not cross into the allocated stack
 * stack due to page alignment. Notice that only whole pages can be
 * locked, i.e. smaller amounts will be extended to the next page
 * boundary.
 */

/*------------------------------------------------------------*/
/*
 * pthread_initialize - associate stack space with thread
 */
void pthread_initialize(t)
pthread_t t;
{
  struct frame *sp;

#ifdef STACK_CHECK
  sp = (struct frame *) (t->stack_base + 
#ifdef SIGNAL_STACK
     pthread_page_size * 4 +
#else /* !SIGNAL_STACK */
     pthread_page_size * 2 +
#endif /* !SIGNAL_STACK */
     SA(t->attr.stacksize + PTHREAD_BODY_OFFSET - WINDOWSIZE));
#else /* !STACK_CHECK */
  sp = (struct frame *)
    (t->stack_base + SA(t->attr.stacksize + PTHREAD_BODY_OFFSET - WINDOWSIZE));
#endif /* !STACK_CHECK */

  /*
   * set up a jump buffer environment, then manipulate the pc and stack
   */
#ifdef C_CONTEXT_SWITCH
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined(__dos__)
  SYS_SIGSETJMP(t->context, FALSE, TRUE);
#else
  sigsetjmp(t->context, FALSE);
#endif /* __FreeBSD__ */
  t->terrno = errno;
#endif
  t->context[JB_SP] = (int) sp;
  t->context[JB_PC] = (int) pthread_body;
#if defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH)
  t->context[JB_PC] -= RETURN_OFFSET;
#endif /* defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH) */
}

#ifndef C_CONTEXT_SWITCH
/*------------------------------------------------------------*/
/*
 * pthread_push_fake_call - push a user handler for a signal on some thread's 
 * stack. The user handler will be the first code executed when control
 * returns to the thread.
 */
void pthread_push_fake_call(p, handler_addr, sig, scp, handler_mask)
     pthread_t p;
     void (*handler_addr)();
     int sig;
     struct context_t *scp;
     sigset_t *handler_mask;
{
  extern void pthread_fake_call_wrapper();
  struct frame *framep;
  int new_context = scp == (struct context_t *) DIRECTED_AT_THREAD;

  /*
   * create a new frame for the wrapper, and bind sp of the new 
   * frame to the frame structure.
   */
  if (new_context) {                /* init context structure if neccessary */
                                    /* allocate space on stack */
    scp = (struct context_t *)
      (p->context[JB_SP] - SA(sizeof(struct context_t)));
    /*
     * need space for new window and 2 params on stack
     */
    framep = (struct frame *) ((int) scp - SA(MINFRAME + PARAM_OFFSET));
    pthread_sigcpyset2set(&scp->sc_mask, &p->mask);
    scp->sc_sp = p->context[JB_SP];
    /*
     * The offset will be subtracted in the wrapper to hide this issue
     * from the user (in case he changes the return address)
     */
    scp->sc_pc = p->context[JB_PC];
#if defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH)
    scp->sc_pc += RETURN_OFFSET;
#endif /* defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH) */
  }
  else
    framep = (struct frame *) (p->context[JB_SP] - SA(MINFRAME + PARAM_OFFSET));

  framep->fr_savfp = (struct frame *) p->context[JB_SP];
                                    /* put fp in saved area of i6. */
  framep->fr_savpc = p->context[JB_PC];
                                    /* put pc in saved area of i7. */
#ifndef ASM_SETJMP
  framep->fr_savpc -= RETURN_OFFSET;
#endif /* !ASM_SETJMP */
  framep->fr_arg[0] = (int) handler_addr;
                                    /* save handler's address as i0. */
  framep->fr_arg[1] = (int) &framep->fr_argx[0]; /* save ptr->sig mask as i1. */
  pthread_sigcpyset2set(&framep->fr_argx[0], &p->mask); /* sig mask on stack. */
  framep->fr_arg[2] = sig;          /* arg0 to user handler in i2. */
  framep->fr_arg[3] = (int) &p->sig_info[sig == -1 ? 0 : sig];
				    /* arg1 to user handler in i3. */
  framep->fr_arg[4] = (int) scp;    /* arg2 to user handler in i4. */
  framep->fr_arg[5] = new_context;  /* used by wrapper to restore context */
  framep->fr_local[6] = (int) (p->cond ? p->cond : (sig == -1 ? NULL : new_cond[sig])); /* addr. of inter. cond. wait in l6 */
  if (sig != -1)
    new_cond[sig] = NULL;
  framep->fr_local[7] = (int) (sig == -1 ? NULL : new_scp[sig]);   /* old context pointer in l7 */
  if (sig != -1)
    new_scp[sig] = NULL;
  p->context[JB_SP] = (int) framep;     /* store new sp */
  p->context[JB_PC] = (int) pthread_fake_call_wrapper; /* store new pc */
#if defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH)
  p->context[JB_PC] -= RETURN_OFFSET;
#endif /* defined(ASM_SETJMP) || !defined(C_CONTEXT_SWITCH) */

  if (handler_mask)
    pthread_sigcpyset2set(&p->mask, handler_mask); /* new thread mask */
  if (sig > 0)
    sigaddset(&p->mask, sig);
}
#endif /* !C_CONTEXT_SWITCH */

/*------------------------------------------------------------*/
