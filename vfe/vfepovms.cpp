//******************************************************************************
///
/// @file vfe/vfepovms.cpp
///
/// This module implements POVMS message handling functionality
///
/// @author Christopher J. Cason
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#include "vfe.h"

#include "povms/povmscpp.h"

// this must be the last file included
#include "syspovdebug.h"

namespace vfe
{

using namespace pov_base;

class SysQNode
{
  public:
    SysQNode();
    ~SysQNode();

    int Send (void *pData, int Len) ;
    void *Receive (int *pLen, bool Blocking) ;

  private:
    typedef struct _DataNode
    {
      unsigned int              Len;
      void                      *Data;
      _DataNode                 *Next;
    } DataNode ;

    unsigned int                m_Sanity ;
    unsigned int                m_Count ;
    unsigned int                m_ID ;
    DataNode                    *m_First ;
    DataNode                    *m_Last ;
    boost::mutex                m_EventMutex ;
    boost::condition            m_Event ;

    static unsigned int         QueueID ;
} ;

////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////

unsigned int SysQNode::QueueID ;

extern volatile POVMSContext POVMS_Render_Context ;

////////////////////////////////////////////////////////////////////
// error handling
////////////////////////////////////////////////////////////////////

void vfeAssert (const char *message, const char *filename, int line)
{
  throw vfeCriticalError(message, filename, line);
}

////////////////////////////////////////////////////////////////////
// class SysQNode
////////////////////////////////////////////////////////////////////

SysQNode::SysQNode (void)
{
  m_Sanity = 0xEDFEEFBE ;
  m_Count = 0 ;
  m_First = NULL ;
  m_Last = NULL ;
  m_ID = QueueID++ ;
}

SysQNode::~SysQNode ()
{
  assert (m_Sanity == 0xEDFEEFBE) ;
  m_Event.notify_all ();
  boost::mutex::scoped_lock lock (m_EventMutex);
  if (m_Count > 0)
  {
    DataNode *current = m_First ;
    DataNode *next = NULL ;

    while (current != NULL)
    {
      next = current->Next ;
      POVMS_Sys_Free (current) ;
      current = next ;
    }
  }
}

int SysQNode::Send (void *pData, int Len)
{
  assert (m_Sanity == 0xEDFEEFBE) ;
  if (m_Sanity == 0xEDFEEFBE)
  {
    DataNode *dNode = reinterpret_cast<DataNode *>(POVMS_Sys_Malloc (sizeof (DataNode))) ;
    if (dNode == NULL)
      return (-3) ;

    dNode->Data = pData ;
    dNode->Len = Len ;
    dNode->Next = NULL ;

    boost::mutex::scoped_lock lock (m_EventMutex) ;

    if (m_Last != NULL)
      m_Last->Next = dNode ;
    if (m_First == NULL)
      m_First = dNode ;
    m_Last = dNode ;
    m_Count++ ;
  }
  else
    return (-2) ;

  m_Event.notify_one ();
  return (0) ;
}

void *SysQNode::Receive (int *pLen, bool Blocking)
{
  boost::mutex::scoped_lock lock (m_EventMutex);

  assert (m_Sanity == 0xEDFEEFBE) ;
  if (m_Sanity != 0xEDFEEFBE)
    throw vfeInvalidDataError("Invalid sanity field in SysQNode::Receive");

  if (m_Count == 0)
  {
    if (Blocking == false)
      return (NULL);

    // TODO: have a shorter wait but loop, and check for system shutdown
    // TODO FIXME - boost::xtime has been deprecated since boost 1.34.
    boost::xtime t;
    boost::xtime_get (&t, POV_TIME_UTC);
    t.nsec += 50000000 ;
    m_Event.timed_wait (lock, t);

    if (m_Count == 0)
      return (NULL) ;
  }

  DataNode *dNode = m_First ;
  if (dNode == NULL)
    throw vfeInvalidDataError("NULL data node in SysQNode::Receive");

  void *dPtr = dNode->Data ;
  *pLen = dNode->Len ;
  if (dNode == m_Last)
    m_Last = NULL ;
  m_First = dNode->Next ;
  m_Count-- ;
  assert (m_Count != 0 || (m_First == NULL && m_Last == NULL)) ;
  POVMS_Sys_Free (dNode) ;
  return (dPtr) ;
}

////////////////////////////////////////////////////////////////////
// POVMS queue support code
////////////////////////////////////////////////////////////////////

bool POVMS_Init (void)
{
  return (true) ;
}

void POVMS_Shutdown (void)
{
  // TODO: should keep track of open queues and delete them here
}

POVMSAddress vfe_POVMS_Sys_QueueToAddress (SysQNode *Node)
{
  return ((POVMSAddress) Node) ;
}

SysQNode *vfe_POVMS_Sys_AddressToQueue (POVMSAddress Addr)
{
  return (reinterpret_cast<SysQNode *>(Addr)) ;
}

SysQNode *vfe_POVMS_Sys_QueueOpen (void)
{
  return (new SysQNode) ;
}

void vfe_POVMS_Sys_QueueClose (SysQNode *SysQ)
{
  delete SysQ ;
}

int vfe_POVMS_Sys_QueueSend (SysQNode *SysQ, void *pData, int Len)
{
  if (SysQ == NULL)
    return (-1) ;
  return (SysQ->Send (pData, Len)) ;
}

void *vfe_POVMS_Sys_QueueReceive (SysQNode *SysQ, int *pLen, bool Blocking, bool Yielding)
{
  if (pLen == NULL)
    return (NULL) ;
  *pLen = 0 ;
  if (SysQ == NULL)
  {
    if (Yielding)
      Delay (1) ;
    return (NULL) ;
  }
  return (SysQ->Receive (pLen, Blocking)) ;
}

POVMS_Sys_Thread_Type POVMS_GetCurrentThread (void)
{
  return (vfePlatform::GetThreadId ()) ;
}

}
