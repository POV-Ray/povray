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

  @(#)get_offsets.c	3.14 11/8/00

*/

/* Writes the offsets (to be used in assembly code) to the file
 * "pthread_offsets.h".
 */

#define PTHREAD_KERNEL
#include "internals.h"
#include "setjmp.h"

main()
{
  pthread_t p = NULL;
  jmp_buf env;
#if defined (__FreeBSD__) || defined (_M_UNIX) || defined(__linux__) || defined (__dos__)
  SYS_SIGJMP_BUF sigenv;
#else
  sigjmp_buf sigenv;
#endif
  pthread_mutex_t mutex;
  pthread_cleanup_t cleanup;
#ifndef C_CONTEXT_SWITCH
  struct context_t *scp = NULL;
#endif
  
  printf("/* Copyright (C) 1992, 1993, 1994, 1995, 1996 the Florida State University\n");
  printf("   Distributed by the Florida State University under the terms of the\n");
  printf("   GNU Library General Public License.\n");
  printf("\n");
  printf("This file is part of Pthreads.\n");
  printf("\n");
  printf("Pthreads is free software; you can redistribute it and/or\n");
  printf("modify it under the terms of the GNU Library General Public\n");
  printf("License as published by the Free Software Foundation (version 2).\n");
  printf("\n");
  printf("Pthreads is distributed \"AS IS\" in the hope that it will be\n");
  printf("useful, but WITHOUT ANY WARRANTY; without even the implied\n");
  printf("warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
  printf("See the GNU Library General Public License for more details.\n");
  printf("\n");
  printf("You should have received a copy of the GNU Library General Public\n");
  printf("License along with Pthreads; see the file COPYING.  If not, write\n");
  printf("to the Free Software Foundation, 675 Mass Ave, Cambridge,\n");
  printf("MA 02139, USA.\n");
  printf("\n");
  printf("Report problems and direct all questions to:\n");
  printf("\n");
  printf("  pthreads-bugs@ada.cs.fsu.edu\n");
  printf("\n");
  printf("  %@(#)get_offsets.c	2.6% %6/16/95%\n");
  printf("*/\n");
  printf("\n");

  printf("#if defined(LOCORE) || defined(_ASM)\n");
  
  /* Offsets in the TCB */
#ifndef C_CONTEXT_SWITCH  
  /* Offsets in the context structure */
  printf("#define sp_offset %d\n", (int) &(p->context[JB_SP]) - (int) p);
  printf("#define pc_offset %d\n", (int) &(p->context[JB_PC]) - (int) p);

  printf("#define thread_errno %d\n", (int) &p->terrno - (int) p);
  printf("#define stack_base %d\n", (int) &p->stack_base - (int) p);
  printf("#define state %d\n", ((int) (&(p->state))) - ((int) p));
  printf("#define nscp %d\n", (int) &p->nscp - (int) p);
#endif
  printf("#define mask %d\n", (int) &p->mask - (int) p);
#ifndef C_CONTEXT_SWITCH  
  printf("#define pending %d\n", (int) &p->pending - (int) p);
#endif
#ifdef STAND_ALONE
  printf("#define queue %d\n", (int) &p->queue - (int) p);
  printf("#define tp_sec %d\n", (int) &p->tp.tv_sec - (int) p);
  printf("#define tp_nsec %d\n", (int) &p->tp.tv_nsec - (int) p);
#endif
  
  /* Offsets in kernel structure */
  printf("#define pthread_self %d\n", (int) &(mac_pthread_self()) - 
						(int) &pthread_kern);
#ifndef C_CONTEXT_SWITCH  
  printf("#define is_in_kernel %d\n", (int) &(is_in_kernel) - 
						(int) &pthread_kern);
  printf("#define is_updating_timer %d\n", (int) &(is_updating_timer) - 
						(int) &pthread_kern);
  printf("#define state_change %d\n", (int) &(state_change) - 
						(int) &pthread_kern);
  printf("#define new_signals %d\n", (int) &(new_signals) - (int)&pthread_kern);
  printf("#define pending_signals %d\n",(int) &(pending_signals) - 
						(int) &pthread_kern);
  printf("#define all_signals %d\n",(int) &(all_signals) - (int) &pthread_kern);
  printf("#define no_signals %d\n",(int) &(no_signals) - (int) &pthread_kern);
#endif
  printf("#define cantmask %d\n",(int) &(cantmask) - (int) &pthread_kern);
#ifndef C_CONTEXT_SWITCH
  printf("#define process_stack_base %d\n",
	 (int) &(process_stack_base) - (int) &pthread_kern);
  printf("#define ready %d\n", (int) &(ready) - (int) &pthread_kern);
  printf("#define ready_head %d\n", (int) &(ready.head) - (int) &pthread_kern);
  printf("#define sched %d\n",(int) &ready.head->attr.sched);
  printf("#define TV_SEC %d\n", (int)&(timeofday.tv_sec) - (int) &pthread_kern);
  printf("#define TV_NSEC %d\n",(int)&(timeofday.tv_nsec) - (int)&pthread_kern);

  /* Offsets in context_t structure */
  printf("#define sc_mask %d\n", (int) &scp->sc_mask - (int) scp);
  printf("#define sc_sp %d\n", (int) &scp->sc_sp - (int) scp);
  printf("#define sc_pc %d\n", (int) &scp->sc_pc - (int) scp);
#endif

  /* Offsets in jmp_buf structure */
  printf("#define jmp_svmask %d\n", (int) &env[JB_SVMASK] - (int) env);
  printf("#define jmp_pc %d\n", (int) &env[JB_PC] - (int) env);
  printf("#define jmp_sp %d\n", (int) &env[JB_SP] - (int) env);
  printf("#define jmp_mask %d\n", (int) &env[JB_MASK] - (int) env);

  /* Offsets in sigjmp_buf structure */
  printf("#define mutex_queue %d\n", (int) &mutex.queue - (int) &mutex);
  printf("#define mutex_lock  %d\n", (int) &mutex.lock  - (int) &mutex);
  printf("#define mutex_owner %d\n", (int) &mutex.owner - (int) &mutex);
#ifdef _POSIX_THREADS_PRIO_PROTECT
  printf("#define mutex_protocol %d\n", (int) &mutex.protocol - (int) &mutex);
#endif
  
  /* Size of cleanup structure */
  printf("#define cleanup_size %d\n", sizeof(*cleanup));

  /* Size of sigset_t structure */
  printf("#define sigset_t_size %d\n", sizeof(sigset_t));

  printf("#endif /* defined(LOCORE) || defined(_ASM) */\n");

  return(0);
}
