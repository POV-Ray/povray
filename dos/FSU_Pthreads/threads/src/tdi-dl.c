/* Copyright (C) 1992 the Florida State University
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

  @(#)tdi-dl.c	1.2 7/26/99

*/

#ifdef TDI_SUPPORT
/*  dynamic load of the TDI is splitted up from tdi-aux.c,
 *  because the dlfcn.h file includes the system pthread.h
 *  file under Solaris -> to avoid declaration errors, we 
 *  keep the files apart                                 
 */

#include <stdio.h>
#include <dlfcn.h> 

/* if shared libraries are supported, TDI_Server will be *
 * the function used by the Debugger                     */

int (*TDI_Server_Func) (int,int) = NULL;

/* the debugger has to call pthread_load_TDI_Server before *
 * interacting with it                                     */

void* libhandle = NULL; 
char  shared_libname[100]; /* for debugging */

int
pthread_load_TDI_Server(char* shlibname, char* server_symname) {
  /* libhandle should survive this function */
  static int sucessfully_loaded = -1;
  char *i;
  char *ld_library_path = (char *) NULL;
  int k = 0;

#ifdef TDI_STATIC
 extern tdi_server();

 TDI_Server_Func = &tdi_server;
#else
#if defined(__sun__) && !defined(__svr4__)
  ld_library_path = (char *) getenv("LD_LIBRARY_PATH");
#endif

  do {
#if defined(__sun__) && !defined(__svr4__)
    /*
     * SunOS libdl does not recognize the LD_LIBRARY_PATH, so
     * we need to do this ourselves.
     */
    shared_libname[0] = '\0';
    if (*ld_library_path != '/') {
      getwd(shared_libname);
      strcat(shared_libname, "/");
    }
    k = strlen(shared_libname);
    for (; *ld_library_path != '\0' && *ld_library_path != ':'; k++)
      shared_libname[k]=*ld_library_path++;
    if (*ld_library_path == ':')
      ld_library_path++;
#endif      

    i = shlibname;
    /* this is a safe strcpy(shared_libname,shlibname) */
    while(*i) {
      shared_libname[k+(int)i-(int)shlibname]=*i;
      i++;
    }
    shared_libname[k+(int)i-(int)shlibname]='\0';
    
    /* return values:                             *
     * 0 - o.k.                                   *
     * 1 - this is a thread implementation, that  *
     *     does'nt use a TDI-Hook                 *
     * 2 - error while opening the shared library */
    
#if 1
    if (sucessfully_loaded != -1)
      return sucessfully_loaded;
#endif
    
    /* open the shared library */
    libhandle = dlopen(shared_libname,
		       RTLD_LAZY
		       /* under LINUX one has to compile *
			* the threaded app with -rshared */
#ifdef SOLARIS
		       /* SOLARIS provides a special feature *
			* to make the own symbol visible     *
			* to the dynamic linked library      */
		       | RTLD_PARENT
#endif
		       ); 
    if (!libhandle && (!ld_library_path || *ld_library_path == '\0'))
      return (sucessfully_loaded=2);
  } while (!libhandle);
#ifdef SOLARIS
  {
    int *TDI_register_func;
    extern void pthread_TDI_register();
    TDI_register_func = dlsym(libhandle, "pthread_TDI_register_func");
    *TDI_register_func = pthread_TDI_register;
  }
#endif
  TDI_Server_Func = dlsym(libhandle,server_symname);
  if (dlerror()!=NULL)
    return (sucessfully_loaded=2);
#endif

  return (sucessfully_loaded=0);
}     

int TDI_Server (int a,int b) {

 return TDI_Server_Func(a,b);

}
#endif
