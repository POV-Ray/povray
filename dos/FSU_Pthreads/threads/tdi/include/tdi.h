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
 * $Author: dschulz $ $Date: 1999/05/03 23:09:40 $ $Name: TDI-alpha-release-2 $ File: tdi.h
 *
 */
/* -------------------------------------------------- *
 * Description: common TDI definitions and types      *
 * Date       : 1999-Apr-4                            *
 * -------------------------------------------------- */

#ifndef __tdi_h__
#define __tdi_h__


#define TDI_ERROR 1
#define TDI_OK    0

/* the basic types */

typedef int       AttrTypeT;
typedef int     AttrDomainT;
typedef void*       ObjRefT;

/* relations */
typedef enum {
  ET_UNDEF=-1,
  ET_THREAD = 0,
  ET_MUTEX,
  ET_COND,
  /******/
  ET_COUNT
}                  RelTypeT;


typedef enum {
  TA_UNDEF=-1,
  TA_ID = 0,
  TA_ADDR,
  TA_PRIO,
  TA_STATE,
  TA_RSTATE,
  TA_ENTRY,
  TA_ENTRYARG,
  TA_NEWPC,
  TA_SP,
  TA_MBO,  /* mbo = mutex blocked on     */
  TA_CVWF, /* cvwf = cond var waiting for */
  TA_PID,  /* id of the the task or process the thread is running in */
  /******/
  TA_COUNT
} ThreadAttributeTypeT;

typedef enum {
  MA_UNDEF=-1,
  MA_ID = 0,
  MA_ADDR,
  MA_OWNER,
  /******/
  MA_COUNT
} MutexAttributeTypeT;

typedef enum {
  CA_UNDEF=-1,
  CA_ID = 0,
  CA_ADDR,
  CA_MUTEX,
  /******/
  CA_COUNT
} CondAttributeTypeT;


/* ------------------------------------- *
 *        thread states                  *
 * ------------------------------------- */

enum {
  TDI_UNDEF_STATE=0,    /* out of range */
  TDI_RUNNING_STATE,    /* running threads with CPU time */
  TDI_READY_STATE,      /* scheduled threads, not blocked and not waiting */
  TDI_BLOCKED_M_STATE,  /* blocked on a mutex */
  TDI_BLOCKED_C_STATE,  /* cond_wait */
  TDI_BLOCKED_T_STATE,  /* any timed wait */
  TDI_BLOCKED_S_STATE,  /* suspended */
  TDI_BLOCKED_O_STATE,  /* other blocked states  */
  TDI_EXITING_STATE     /* terminating */
}; 

/* implementation type:                           *
 *                                                *
 * classification criterion:                      *
 *  cardinality ratio of user level and kernel    *
 *  threads                                       */

enum {
  K2UL_UNDEF = 0,   /* undefined mapping                         */
  K2UL_ONE_TO_ONE,  /* every UL-thread is mapped                 *
		     * to on kernel thread (i.e. Linuxthreads)   */
  K2UL_ONE_TO_MANY, /* all UL-threads are associated with        *
		     * the same kernel thread (UNIX-process)     *
		     * (i.e. FSU, MIT)                           */
  K2UL_MANY_TO_MANY /* a UL-thread can be asociated with one or  *
		     * more kernel threads (i.e. Solaris threads)*/
}; 

#endif /* /* __tdi_h__ */ */






