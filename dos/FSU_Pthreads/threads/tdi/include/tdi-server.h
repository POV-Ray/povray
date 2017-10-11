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
 * $Author: dschulz $ $Date: 1999/05/03 23:09:40 $ $Name: TDI-alpha-release-2 $ File: tdi-server.h
 *
 */
/* -------------------------------------------------- *
 * Description: TED/TDI interface                     *
 * Date       : 1999-Apr-4                            *
 * -------------------------------------------------- */

#ifndef __tdi_server_h__
#define __tdi_server_h__

typedef struct {

  /* registering the TED functions for: */ 
  /* i) attribute access                */  
  
  int  (*SetAttrFunc) (RelTypeT   Rel, 
                       AttrTypeT Attr, 
                       AttrDomainT (*GetFunc) (ObjRefT), 
                       AttrDomainT (*SetFunc) (ObjRefT, AttrDomainT));
  
  /* ii) set iteration              */
  
  int  (*SetIterFuncs) (RelTypeT Rel, 
			ObjRefT (*getFirst) (), 
			ObjRefT (*getNext)  (ObjRefT));
  
  /* functions to access the object containers */
  
  int     (*RegisterObject)     (RelTypeT Rel, ObjRefT ObjRef);
  
  int     (*UnregisterObject)   (RelTypeT Rel, ObjRefT ObjRef);
  
  int     (*IsRegistered)       (RelTypeT Rel, ObjRefT ObjRef); 
  
  ObjRefT (*GetFirstObject)     (RelTypeT Rel);
  
  ObjRefT (*GetNextObject)      (RelTypeT Rel, ObjRefT Last);
  
} TDIAuxFuncsT;

#endif
















