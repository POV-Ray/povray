//******************************************************************************
///
/// @file povms/configpovms.h
///
/// This header file defines all types that can be configured by platform
/// specific code for parser layer use. It further allows insertion of platform
/// specific function prototypes making use of those types.
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

#ifndef POVRAY_POVMS_CONFIGPOVMS_H
#define POVRAY_POVMS_CONFIGPOVMS_H

#if !defined(__cplusplus)
    // When compiling as part of the POV-Ray project, povms.c must be compiled as a C++ file,
    // due to potential C++-isms in the base/configbase.h header included via this file.
    #error "povms.c must be compiled as a C++ file when used as part of the POV-Ray project."
#endif

// Pull in other compile-time config header files first
#include "base/configbase.h"
#include "syspovconfigpovms.h"

//##############################################################################
///
/// @defgroup PovMSConfig POVMS Compile-Time Configuration
/// @ingroup PovMS
/// @ingroup PovConfig
///
/// @{

//******************************************************************************
// The following override the defaults in povms.h and povms.c, as those are
// chosen for a C environment potentially detached from POV-Ray, while we have
// C++ and the POV-Ray base configuration at our disposal.

#ifndef POVMSType
    #define POVMSType               POV_UINT32
#endif

#ifndef POVMSLong
    #define POVMSLong               POV_INT64
    #define SetPOVMSLong(v,h,l)     *v = (((((POVMSLong)(h)) & 0x00000000ffffffff) << 32) | (((POVMSLong)(l)) & 0x00000000ffffffff))
    #define GetPOVMSLong(h,l,v)     *h = ((v) >> 32) & 0x00000000ffffffff; *l = (v) & 0x00000000ffffffff
#endif

#ifndef POVMSBool
    #define POVMSBool               bool
#endif

#ifndef POVMSTrue
    #define POVMSTrue               true
#endif

#ifndef POVMSFalse
    #define POVMSFalse              false
#endif

#ifndef POVMSUCS2
    #define POVMSUCS2               char16_t
#endif

#ifndef kDefaultTimeout
    #ifdef _DEBUG
        // a long timeout so we can break into the debugger
        #define kDefaultTimeout         100
    #endif
#endif

//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_POVMS_CONFIGPOVMS_H
