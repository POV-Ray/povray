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

  @(#)config_header.c	3.14 11/8/00

*/

/*
 * Create configuration header files depending on compile options
 */

#ifdef INTERNALS
#define PTHREAD_KERNEL
#include "internals.h"
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef MIN
#define MIN(x,y) (x < y ? x : y)
#endif

main(argc, argv)
     int argc;
     char *argv[];
{
  short local = (argv[1][0] == '-' && argv[1][1] == 'l');
  int release_index = (local ? 2 : 1);
  short old_local;
  short restore_local = FALSE;

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
  printf("  %@(#)config_header.c	3.14% %11/8/00%\n");
  printf("*/\n");
  printf("\n");

  printf("/*\n");
  printf(" * configuration header file to identify compile options\n");
  printf(" */\n");
  printf("\n");

  for (argc--; argc; argc--)
    if (argv[argc][0] == '-' && argv[argc][1] == 'D') {
      if (strcmp(&argv[argc][2], "__dos__") == 0 ||
          strcmp(&argv[argc][2], "_POSIX") == 0) {
        old_local = local;
        restore_local = TRUE;
        local = TRUE;
      }
      if (argc == release_index) {
	printf("#ifndef RELEASE%s\n", local ? "" : "_NP");
	printf("#define RELEASE%s %s\n", local ? "" : "_NP", &argv[argc][2]);
      } else {
	printf("#ifndef %s%s\n", &argv[argc][2], local ? "" : "_NP");
	printf("#define %s%s\n", &argv[argc][2], local ? "" : "_NP");
      }
      printf("#endif\n\n");
      if (restore_local) {
        local = old_local;
        restore_local = FALSE;
      }
    }

  printf("#ifndef _M_UNIX\n");
  printf("#if defined(M_UNIX) || defined(__M_UNIX)\n");
  printf("#define _M_UNIX\n");
  printf("#endif\n");
  printf("#endif\n");

#ifdef INTERNALS
  if (local)
  {
    struct context_t *scp;
    int sigset_t_size = sizeof(sigset_t);
    int sigcontext_mask_t_size = sizeof(scp->sc_mask);

    printf("\n");
    printf("#define PTHREAD_SIGSET_T_SIZE_NP %d\n", sigset_t_size);
    printf("#define PTHREAD_SIGCONTEXT_MASK_T_SIZE_NP %d\n",
	   sigcontext_mask_t_size);
    printf("#define PTHREAD_SIGSET2SET_SIZE_NP %d\n",
	   MIN(sigset_t_size, sigcontext_mask_t_size));
  }
#endif

  return(0);
}
