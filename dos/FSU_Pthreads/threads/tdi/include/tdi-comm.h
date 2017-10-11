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
 * $Author: dschulz $ $Date: 1999/05/03 23:09:40 $ $Name: TDI-alpha-release-2 $ File: tdi-comm.h
 *
 */
#ifndef __comm_chan_h__
#define __comm_chan_h__

/* ------------------------------------------------- *
 * Description   : implementation of an abstract     *
 *                 communication channel             *
 * Author        : Daniel Schulz                     * 
 * Creation Date : 98-06-18                          *
 * Last modified :                                   *
 * ------------------------------------------------- */

/* defines */

/* this is the frame length sent over IPC */
#define TDI_COMM_MAX_PACKET_SIZE 8096

/* this is the overall size of a tdi query response */
#define TDI_MAX_RESPONSE_BYTES (TDI_COMM_MAX_PACKET_SIZE * 16)

#define IPC_UNKNOWN_ID -1

#define IPC_ERROR -1
#define IPC_OK     0

/* what kind of communication channel can be used */

typedef enum {
  UNDEF_IPC =0,
  SHARED_MEMORY_IPC,
  /* insert new entries above */
  LAST_IPC=SHARED_MEMORY_IPC
} CommChannelTypeT;

struct IpcChannelT {

  int ID;

  void * IpcData;

  /* the methods */
  int  (*init)    (struct IpcChannelT*);
  int  (*open)    (struct IpcChannelT*, void* spec);
  void (*close)   (struct IpcChannelT*);
  int  (*send)    (struct IpcChannelT*, char  *data, int);
  int  (*receive) (struct IpcChannelT*, char **data, int*);

};

typedef struct IpcChannelT IpcChannelT;

/* the communication channel is an abstract      * 
 * data type with a thin interface               */                          



struct CommChannelT {

  /* a key to get resources */ 
  int key;

  /* what type we have */
  CommChannelTypeT Type;

  /* the underlying Ipc channel */
  IpcChannelT* IpcChannel;
   
  /* the methods */
  int  (*init)    (struct CommChannelT*);
  int  (*open)    (struct CommChannelT*, void* spec);
  void (*close)   (struct CommChannelT*);
  int  (*send)    (struct CommChannelT*, char  *data, int);
  int  (*receive) (struct CommChannelT*, char **data, int*);


};

typedef struct CommChannelT CommChannelT;


/* a constructor */

void newTDIChannel (CommChannelT* channel, int IpcType);

#endif










