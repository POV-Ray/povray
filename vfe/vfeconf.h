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
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_VFE_VFECONF_H
#define POVRAY_VFE_VFECONF_H

//////////////////////////////////////////////////////////////
// POVMS support
/////////////////////////////////////////////////////////////

#define POVMS_Sys_Thread_Type                 unsigned long

namespace vfe
{
  class SysQNode;
  void* /*POVMSAddress*/ vfe_POVMS_Sys_QueueToAddress (SysQNode *q) ;
  SysQNode *vfe_POVMS_Sys_AddressToQueue (void* /*POVMSAddress*/ a) ;
  SysQNode *vfe_POVMS_Sys_QueueOpen (void) ;
  void vfe_POVMS_Sys_QueueClose (SysQNode *q) ;
  void *vfe_POVMS_Sys_QueueReceive (SysQNode *q, int *l, bool, bool) ;
  int vfe_POVMS_Sys_QueueSend(SysQNode *q, void *p, int l) ;
  POVMS_Sys_Thread_Type POVMS_GetCurrentThread();
  void vfeAssert (const char *message, const char *filename, int line) ;
}
// end of namespace vfe

#define POVMS_ASSERT_OUTPUT                   vfe::vfeAssert
#define POVMS_Sys_Queue_Type                  vfe::SysQNode *
#define POVMS_Sys_QueueToAddress              vfe::vfe_POVMS_Sys_QueueToAddress
#define POVMS_Sys_AddressToQueue              vfe::vfe_POVMS_Sys_AddressToQueue
#define POVMS_Sys_QueueOpen                   vfe::vfe_POVMS_Sys_QueueOpen
#define POVMS_Sys_QueueClose                  vfe::vfe_POVMS_Sys_QueueClose
#define POVMS_Sys_QueueReceive                vfe::vfe_POVMS_Sys_QueueReceive
#define POVMS_Sys_QueueSend                   vfe::vfe_POVMS_Sys_QueueSend
#define POVMS_Sys_GetCurrentThread            vfe::POVMS_GetCurrentThread

#endif // POVRAY_VFE_VFECONF_H
