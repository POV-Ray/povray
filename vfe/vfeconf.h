//******************************************************************************
///
/// @file vfe/vfeconf.h
///
/// This file contains vfe specific defines.
///
/// @author Christopher J. Cason
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef __VFECONF_H__
#define __VFECONF_H__

//////////////////////////////////////////////////////////////
// POVMS support
/////////////////////////////////////////////////////////////

#define POVMS_Sys_Thread_Type                 unsigned long

#include <string>
#include <cstdio>

namespace vfe
{
  typedef class SysQNode POVMS_Sys_QueueNode ;
  void* /*POVMSAddress*/ vfe_POVMS_Sys_QueueToAddress (POVMS_Sys_QueueNode *q) ;
  POVMS_Sys_QueueNode *vfe_POVMS_Sys_AddressToQueue (void* /*POVMSAddress*/ a) ;
  POVMS_Sys_QueueNode *vfe_POVMS_Sys_QueueOpen (void) ;
  void vfe_POVMS_Sys_QueueClose (POVMS_Sys_QueueNode *q) ;
  void *vfe_POVMS_Sys_QueueReceive (POVMS_Sys_QueueNode *q, int *l, bool, bool) ;
  int vfe_POVMS_Sys_QueueSend(POVMS_Sys_QueueNode *q, void *p, int l) ;
  int Allow_File_Read (const unsigned short *Filename, const unsigned int FileType);
  int Allow_File_Write (const unsigned short *Filename, const unsigned int FileType);
  POVMS_Sys_Thread_Type POVMS_GetCurrentThread();
  void vfeAssert (const char *message, const char *filename, int line) ;
  FILE *vfeFOpen (const std::basic_string<unsigned short>& name, const char *mode);
  bool vfeRemove (const std::basic_string<unsigned short>& name);
}

#define POVMS_ASSERT_OUTPUT                   vfe::vfeAssert
#define POVMS_Sys_Queue_Type                  vfe::POVMS_Sys_QueueNode *
#define POVMS_Sys_Queue_Type                  vfe::POVMS_Sys_QueueNode *
#define POVMS_Sys_QueueToAddress              vfe::vfe_POVMS_Sys_QueueToAddress
#define POVMS_Sys_AddressToQueue              vfe::vfe_POVMS_Sys_AddressToQueue
#define POVMS_Sys_QueueOpen                   vfe::vfe_POVMS_Sys_QueueOpen
#define POVMS_Sys_QueueClose                  vfe::vfe_POVMS_Sys_QueueClose
#define POVMS_Sys_QueueReceive                vfe::vfe_POVMS_Sys_QueueReceive
#define POVMS_Sys_QueueSend                   vfe::vfe_POVMS_Sys_QueueSend
#define POVMS_Sys_GetCurrentThread            vfe::POVMS_GetCurrentThread

#endif // __VFECONF_H__
