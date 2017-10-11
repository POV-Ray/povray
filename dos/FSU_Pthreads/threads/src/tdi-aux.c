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

  @(#)tdi-aux.c	3.14 11/8/00

*/

#ifdef TDI_SUPPORT

#include "internals.h"
#include "tdi-dl.h"
#include "tdi.h"
#include "tdi-server.h"
#include "pthread.h"
#include <pthread/asm.h>
#include "setjmp.h"

#define P(x)

/* FIXME: this should be replaced by a profile entity */

static int persistentID_support=0;

/* implementation type                                *
 * -> all user level threads are associated with the  *
 *    same kernel thread                              */

volatile int __pthread_debug_with_TDI = - K2UL_ONE_TO_MANY;


/* this variable is used to dis- or enable the signal *
 * handling before running the TDI code               */

volatile int  __pthread_debug_TDI_sig_ignore = -SIGALRM;

/* this bit-field contains the set of ignored signals *
 * (limted to sizeof(int)*8)                          */

volatile int __pthread_debug_TDI_ignored_signals = 0;


/* numbering functions used by the thread implementation */

int     (*TdiRegister)     (RelTypeT,ObjRefT)       = NULL;   
int     (*TdiUnregister)   (RelTypeT,ObjRefT)       = NULL;   
int     (*TdiIsRegistered) (RelTypeT,ObjRefT)       = NULL;   
ObjRefT (*TdiGetFirst)     (RelTypeT)              = NULL;           
ObjRefT (*TdiGetNext)      (RelTypeT,ObjRefT last)  = NULL; 



/* forward decl's for the class specific functions       */

int TdiThreadRegister     (ObjRefT); 
int TdiThreadUnregister   (ObjRefT); 
int TdiThreadIsRegistered (ObjRefT); 

int TdiMutexRegister      (ObjRefT); 
int TdiMutexUnregister    (ObjRefT); 
int TdiMutexIsRegistered  (ObjRefT); 
ObjRefT TdiGetFirstMutex  ();          
ObjRefT TdiGetNextMutex   (ObjRefT last);

int TdiCondRegister       (ObjRefT);   
int TdiCondUnregister     (ObjRefT);   
int TdiCondIsRegistered   (ObjRefT);   
ObjRefT TdiGetFirstCond   ();           
ObjRefT TdiGetNextCond    (ObjRefT last); 



/* forward decls */

pthread_mutex_t*  fsuGetFirstMutex ();
pthread_mutex_t*  fsuGetNextMutex  (pthread_mutex_t *last);
pthread_cond_t*   fsuGetFirstCond  ();
pthread_cond_t*   fsuGetNextCond   (pthread_cond_t* cond);
int               fsuGetCondID     (pthread_cond_t* cond);
int               fsuGetCondMutex  (pthread_cond_t* cond);

/* ----------------------------------------------- *
 * FSU Pthreads Implementation Wrapper for the TDI *
 * ----------------------------------------------- */

/* List access */

pthread_t  
fsuGetFirstThread() {
  return pthread_kern.k_all.head;
}

pthread_t 
fsuGetNextThread(pthread_t last) {
  return ((last)->next[ALL_QUEUE]);
}


/* thread entity attributes */

int 
fsuGetThreadID    (pthread_t thread) {

  /* 0 is not an ID */
  int tid = 0;


  if(thread == NULL)
    return 0;

  if (!persistentID_support) {
    /* if this implementation provides no persistentID support, we *
     * have to use the persistentID functions                     */

    return ((tid = TdiThreadIsRegistered(thread))?
	    tid :
	    TdiThreadRegister(thread));
  }
  else {
    return (int)thread;
  }
}

int 
fsuGetThreadAddress(pthread_t thread) {
  if (thread)
    return (int) thread;
}


int 
fsuGetThreadPrio  (pthread_t thread) {
  if (thread)
    return thread->base_prio;
};

int 
fsuGetThreadState (pthread_t thread) {
  int fsuState;

  if(!thread)
    return TDI_UNDEF_STATE;

  fsuState = thread->state; 

  /* state translation has to be done to fit the *
   * states of an abstract thread implementation */
  
  if (pthread_equal(pthread_self(),thread)) 
    return TDI_RUNNING_STATE;

  if (fsuState & T_RUNNING)
    return TDI_READY_STATE;

  if ((fsuState & T_BLOCKED) && !(fsuState & T_INTR_POINT)) 
    return TDI_BLOCKED_M_STATE;

  if ((thread)->cond)
    return TDI_BLOCKED_C_STATE;

  if (fsuState & (T_CONDTIMER | T_SYNCTIMER))
    return TDI_BLOCKED_T_STATE;

  if (fsuState & T_SUSPEND)
    return TDI_BLOCKED_S_STATE;

  if (fsuState & (T_RETURNED | T_EXITING))
    return TDI_EXITING_STATE;

  return TDI_BLOCKED_O_STATE;

}

int 
fsuSetThreadState(pthread_t thread, int NewState) {
  
  int retVal=TDI_UNDEF_STATE;


  if (!thread)
    return retVal;

  retVal = fsuGetThreadState(thread);

  switch (NewState) 
    {
      /* suspend */
    case TDI_BLOCKED_S_STATE:
      /* only if not suspended yet */
      if (retVal != TDI_BLOCKED_S_STATE) {
	pthread_suspend_np(thread);
      }
      break;

      /* resume */
    case TDI_READY_STATE:
      /* only if suspended */
      if (retVal == TDI_BLOCKED_S_STATE) {
	pthread_resume_np(thread);
      }
      break;
    default:
    }
  return retVal;
}

int 
fsuGetThreadRealState (pthread_t thread) {
  if(thread)
    return (thread)->state;
};

int 
fsuGetThreadEntry (pthread_t thread) {
  if (thread) 
    return (int)(thread->func);
};

int 
fsuGetThreadEntryArg (pthread_t thread) {
  if (thread)
    return (int)thread->arg; 
}

int 
fsuGetThreadNewPc (pthread_t thread) {
  
  if (thread)
    return (int)(thread->context[JB_PC]+RETURN_OFFSET); 
}

int
fsuGetThreadSp (pthread_t thread) {
  if (thread)
    /* on i386 machines we have to return BP */
    return (int)(thread->context[
#ifdef __linux__
				 JB_BP
#else
				 JB_SP
#endif
				 ]); 
}


int 
fsuGetThreadMutexBlockedOn (pthread_t thread) {
 
  pthread_mutex_t* mutex = fsuGetFirstMutex();

  pthread_t t;

  while (mutex) {
    t = mutex->queue.head;
    while (t) {
      if (pthread_equal(t,thread))
	return fsuGetMutexID(mutex);
      if (t==mutex->queue.tail)
	break;
      t=t->next[PRIMARY_QUEUE];
    }
    mutex = fsuGetNextMutex(mutex);
  }
  return 0;

} 


int 
fsuGetThreadCondWaitFor (pthread_t thread) {

  int state=TDI_UNDEF_STATE;

  if (!thread)
    return 0;
 


  /* the deletion of the cond entry takes very long, *
   * but the state is already set                    */

  state=fsuGetThreadState(thread);

  if (state==TDI_RUNNING_STATE ||  
	state==TDI_READY_STATE) {
    
    return 0;
  }
 
  return fsuGetCondID(thread->cond);
}

int
fsuGetThreadPID (pthread_t t) {

  return (int) getpid();

}


/* mutex access */
pthread_mutex_t*  
fsuGetFirstMutex() {

  /* get hidden mutexes from cond->mutex */
  {
    pthread_cond_t* cond;
    cond = fsuGetFirstCond();
    while (cond) {
      fsuGetCondMutex(cond); /* trigger it */
      cond = fsuGetNextCond(cond);
    }
  }

  /* only one list type */
  if (TdiGetFirstMutex) {
    return (pthread_mutex_t*)TdiGetFirstMutex();
  }

  return NULL;

}

pthread_mutex_t* 
fsuGetNextMutex(pthread_mutex_t *last) {

  /* only one list type */
  if (TdiGetNextMutex) {
    return (pthread_mutex_t*)TdiGetNextMutex(last);
  }

  return NULL;
}

int 
fsuGetMutexID(pthread_mutex_t* mutex) {
  /* all mutexes are registered */

  if (!mutex)
    return 0;

  return TdiMutexIsRegistered(mutex);
}

int 
fsuGetMutexAddress(pthread_mutex_t* mutex) {
  /* FIXME:
   * before release it, we have to find the correct *
   * return type with sizeof(void*)                 */
  return (int)mutex;
}

int 
fsuGetMutexOwner(pthread_mutex_t* mutex) {

  if (mutex) {
    return fsuGetThreadID(mutex->owner);
  }
  return 0;
}


/* condition variables */
pthread_cond_t*  
fsuGetFirstCond() {

  /* only one list type */
  if (TdiGetFirstCond) {
    return (pthread_cond_t*)TdiGetFirstCond();
  }

  return NULL;

}

pthread_cond_t* 
fsuGetNextCond(pthread_cond_t *last) {

  /* only one list type */
  if (TdiGetNextCond) {
    return (pthread_cond_t*)TdiGetNextCond(last);
  }

  return NULL;
}

int
fsuGetCondID(pthread_cond_t* cond) {

  int ID=0;

  if (!cond)
    return 0;
  
  if (!(ID=TdiCondIsRegistered(cond)))
    return TdiCondRegister(cond);

  return ID;

}

int 
fsuGetCondAddr(pthread_cond_t* cond) {
  return (int)cond;
}

int
fsuGetCondMutex(pthread_cond_t* cond) {
  int ID = 0;

  if (!cond->mutex)
    return 0;

  if (!(ID=TdiMutexIsRegistered(cond->mutex)))
    return TdiMutexRegister(cond->mutex);

  return ID;
}


#define REG_ATTR_READ(func,attr,entity) TDIAuxFuncs->SetAttrFunc(entity,attr,(int (*)(void*))func,NULL)

#define REG_ATTR_WRITE(func,attr,entity) TDIAuxFuncs->SetAttrFunc(entity,attr,NULL, (int (*)(ObjRefT,AttrDomainT)) func)

#define REG_ITERATORS(entity,gF,gN) TDIAuxFuncs->SetIterFuncs(entity,(void* (*)())gF,(void* (*)(void*))gN)


/* this function is mandatory */

void 
pthread_TDI_register(TDIAuxFuncs)       /* <IN>  TDI helper functions   */
     TDIAuxFuncsT* TDIAuxFuncs;
{
  
  /* get TDI helper funtions */ 

  TdiRegister       = TDIAuxFuncs->RegisterObject;
  TdiUnregister     = TDIAuxFuncs->UnregisterObject;
  TdiIsRegistered   = TDIAuxFuncs->IsRegistered;
  /* iterator */
  TdiGetFirst       = TDIAuxFuncs->GetFirstObject;
  TdiGetNext        = TDIAuxFuncs->GetNextObject;
  
  /* we register our attribute access functions */
  
  
  if (TDIAuxFuncs->SetAttrFunc && TDIAuxFuncs->SetIterFuncs) {
    
    /* register thread iterators */
    REG_ITERATORS(ET_THREAD,fsuGetFirstThread,fsuGetNextThread);
      
    /* register thread attribute access functions */
      
    REG_ATTR_READ (fsuGetThreadID,              TA_ID      ,ET_THREAD);    
    REG_ATTR_READ (fsuGetThreadAddress,         TA_ADDR    ,ET_THREAD);   
    REG_ATTR_READ (fsuGetThreadPrio,            TA_PRIO    ,ET_THREAD);   
    REG_ATTR_READ (fsuGetThreadState,           TA_STATE   ,ET_THREAD);   
    REG_ATTR_WRITE(fsuSetThreadState,           TA_STATE   ,ET_THREAD);   
    REG_ATTR_READ (fsuGetThreadRealState,       TA_RSTATE  ,ET_THREAD);   
    REG_ATTR_READ (fsuGetThreadEntry,           TA_ENTRY   ,ET_THREAD);   
    REG_ATTR_READ (fsuGetThreadEntryArg,        TA_ENTRYARG,ET_THREAD);   
    REG_ATTR_READ (fsuGetThreadNewPc,           TA_NEWPC   ,ET_THREAD);  
    REG_ATTR_READ (fsuGetThreadSp,              TA_SP      ,ET_THREAD);   
    REG_ATTR_READ (fsuGetThreadMutexBlockedOn,  TA_MBO     ,ET_THREAD);
    REG_ATTR_READ (fsuGetThreadCondWaitFor   ,  TA_CVWF    ,ET_THREAD);
    REG_ATTR_READ (fsuGetThreadPID           ,  TA_PID     ,ET_THREAD);
      
    /* register mutex iterators */
    REG_ITERATORS(ET_MUTEX,fsuGetFirstMutex,fsuGetNextMutex);
      
    /* register mutex attribute access functions */
    REG_ATTR_READ(fsuGetMutexID,      MA_ID    , ET_MUTEX);
    REG_ATTR_READ(fsuGetMutexAddress, MA_ADDR  , ET_MUTEX);
    REG_ATTR_READ(fsuGetMutexOwner,   MA_OWNER , ET_MUTEX);
      
    /* register CV iterators */
    REG_ITERATORS(ET_COND,fsuGetFirstCond,fsuGetNextCond);
      
    /* register CV attribute access functions */
    REG_ATTR_READ(fsuGetCondID  ,  CA_ID    , ET_COND);       
    REG_ATTR_READ(fsuGetCondAddr,  CA_ADDR  , ET_COND);
    REG_ATTR_READ(fsuGetCondMutex, CA_MUTEX , ET_COND);
      
  }
    
}


#define TDI_REG(RelType,ORef)   { if (TdiRegister) return TdiRegister(RelType,ORef); else return 0;}

#define TDI_UNREG(RelType,ORef) { if (TdiUnregister) return TdiUnregister(RelType,ORef); else return 0;}

#define TDI_ISREG(RelType,ORef) { if (TdiIsRegistered) return TdiIsRegistered(RelType,ORef); else return 0;}

#define TDI_GET_FIRST(RelType)  { if (TdiGetFirst) return TdiGetFirst(RelType); else return NULL;}

#define TDI_GET_NEXT(RelType,Last) { if (TdiGetNext) return TdiGetNext(RelType, Last); else return NULL; }

int 
TdiThreadRegister     (ObjRefT ObjRef) { TDI_REG(ET_THREAD,ObjRef); } 

int 
TdiThreadUnregister   (ObjRefT ObjRef) { TDI_UNREG(ET_THREAD,ObjRef);} 

int 
TdiThreadIsRegistered (ObjRefT ObjRef) { TDI_ISREG(ET_THREAD,ObjRef);} 

int 
TdiMutexRegister      (ObjRefT ObjRef) { TDI_REG(ET_MUTEX,ObjRef); } 

int 
TdiMutexUnregister    (ObjRefT ObjRef) { TDI_UNREG(ET_MUTEX,ObjRef);} 

int 
TdiMutexIsRegistered  (ObjRefT ObjRef) { TDI_ISREG(ET_MUTEX,ObjRef);} 

ObjRefT 
TdiGetFirstMutex  () { TDI_GET_FIRST(ET_MUTEX); }          

ObjRefT 
TdiGetNextMutex   (ObjRefT last) { TDI_GET_NEXT(ET_MUTEX,last)}

int 
TdiCondRegister       (ObjRefT ObjRef) { TDI_REG(ET_COND,ObjRef); }   

int 
TdiCondUnregister     (ObjRefT ObjRef) { TDI_UNREG(ET_COND,ObjRef);}   

int 
TdiCondIsRegistered   (ObjRefT ObjRef) { TDI_ISREG(ET_COND,ObjRef);}   

ObjRefT 
TdiGetFirstCond   () { TDI_GET_FIRST(ET_COND); }           

ObjRefT 
TdiGetNextCond    (ObjRefT last) { TDI_GET_NEXT(ET_COND,last); } 
#endif /* /* TDI_SUPPORT */ */
