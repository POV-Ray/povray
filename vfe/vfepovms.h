//******************************************************************************
///
/// @file vfe/vfepovms.h
///
/// This file contains prototypes for functions found in `vfepovms.cpp`.
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

#ifndef POVRAY_VFE_VFEPOVMS_H
#define POVRAY_VFE_VFEPOVMS_H

namespace vfe
{
  bool POVMS_Init (void);
  void POVMS_Shutdown (void);
}

////////////////////////////////////////////////////////////////////////////////////////
// templates to turn a type into a POVMS typeid. note not all POVMS types are supported.
////////////////////////////////////////////////////////////////////////////////////////

template<typename ET> struct GetPOVMSTypeID { enum { type_id = 0 } ; } ;
template<> struct GetPOVMSTypeID<bool> { enum { type_id = kPOVMSType_Bool } ; } ;
template<> struct GetPOVMSTypeID<int> { enum { type_id = kPOVMSType_Int } ; } ;
template<> struct GetPOVMSTypeID<long> { enum { type_id = kPOVMSType_Long } ; } ;
template<> struct GetPOVMSTypeID<float> { enum { type_id = kPOVMSType_Float } ; } ;
template<> struct GetPOVMSTypeID<char *> { enum { type_id = kPOVMSType_CString } ; } ;
template<> struct GetPOVMSTypeID<pov_base::UCS2String&> { enum { type_id = kPOVMSType_UCS2String } ; } ;

#endif // POVRAY_VFE_VFEPOVMS_H
