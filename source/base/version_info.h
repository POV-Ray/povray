//******************************************************************************
///
/// @file base/version_info.h
///
/// Secondary version information.
///
/// This file defines version information macros derived from the primary
/// version information defined in @ref base/version.h, build information defined
/// in @ref base/build.h, and other compile-time configuration pulled in via
/// @ref base/configbase.h.
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

#ifndef POVRAY_BASE_VERSION_INFO_H
#define POVRAY_BASE_VERSION_INFO_H

#include "base/configbase.h"
#include "base/build.h"
#include "base/version.h"

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

//******************************************************************************
///
/// @name Secondary Version and Copyright Information
///
/// @{

/// @def POV_RAY_SOURCE_VERSION
/// Source code version string.
#if POV_RAY_IS_OFFICIAL
    #define POV_RAY_SOURCE_VERSION POV_RAY_FULL_VERSION
#else
    #ifdef POV_RAY_PRERELEASE
        #define POV_RAY_SOURCE_VERSION POV_RAY_FULL_VERSION ".unofficial"
    #else
        #define POV_RAY_SOURCE_VERSION POV_RAY_FULL_VERSION "-unofficial"
    #endif
#endif

/// @def POV_BUILD_IDENTIFIER
/// Full build identifier.
#if defined(POV_RAY_BUILD_ID) && defined(POV_BUILD_INFO)
    #define POV_BUILD_IDENTIFIER POV_RAY_BUILD_ID "." POV_BUILD_INFO
#elif defined(POV_RAY_BUILD_ID)
    #define POV_BUILD_IDENTIFIER POV_RAY_BUILD_ID
#elif defined(POV_BUILD_INFO)
    #define POV_BUILD_IDENTIFIER POV_BUILD_INFO
#else
    // leave POV_BUILD_IDENTIFIER undefined
#endif

/// @def POV_RAY_VERSION
/// Full version string.
#ifdef POV_BUILD_IDENTIFIER
    #define POV_RAY_VERSION POV_RAY_SOURCE_VERSION "+" POV_BUILD_IDENTIFIER
#else
    #define POV_RAY_VERSION POV_RAY_SOURCE_VERSION
#endif

/// @def POV_RAY_VERSION_INFO
/// Verbose version information.
#ifdef POV_COMPILER_INFO
    #define POV_RAY_VERSION_INFO POV_RAY_VERSION " (" POV_COMPILER_INFO ")"
#else
    #define POV_RAY_VERSION_INFO POV_RAY_VERSION
#endif

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_BASE_VERSION_INFO_H
