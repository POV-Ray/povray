/*    Copyright (C) 1999,  Daniel Schulz	   
 * 
 *    This file is part of the TDI library. The TDI library implements  
 *    the generic part of the Thread Debug Interface for POSIX Threads
 *    Implementations - TDI.
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Library General Public
 *    License as published by the Free Software Foundation; either
 *    version 2 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Library General Public License for more details.
 *
 *    You should have received a copy of the GNU Library General Public
 *    License along with this library; if not, write to the Free
 *    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *    Report problems and direct all questions to:
 *
 *    tdi-bugs@informatik.hu-berlin.de
 *	
 * $Author: dschulz $ $Date: 1999/05/03 23:09:39 $ $Name: TDI-alpha-release-2 $ File: tdi-client.h
 *
 */
#ifndef __tdi_client_h__
#define __tdi_client_h__

#include "tdi-comm.h"


#undef TDI_DEBUG
#ifdef TDI_DEBUG
#include <stdio.h>
#define TDI_MSG(string) {fprintf(stderr,"[%s:%d] %s",__FILE__,__LINE__,string);}
#else
#define TDI_MSG(string)
#endif

#define REQUEST_HANDLER_CALL_STRING "TDI_Server"

struct ThreadDebugInterface {

  /* methods */
  int (*init)   (struct ThreadDebugInterface* );
  int (*send)   (struct ThreadDebugInterface*, char*,int);
  int (*receive)(struct ThreadDebugInterface*, char**,int*);
  int (*error)  (struct ThreadDebugInterface*, char *);

  /* attributes */
  CommChannelT* channel;

};

typedef struct ThreadDebugInterface ThreadDebugInterfaceT;

/* that is the global accessible TDI Object */
extern ThreadDebugInterfaceT TDI;

#endif

























