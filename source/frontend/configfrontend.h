//******************************************************************************
///
/// @file frontend/configfrontend.h
///
/// This header file defines all types that can be configured by platform
/// specific code for frontend use. It further allows insertion of platform
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

#ifndef POVRAY_FRONTEND_CONFIGFRONTEND_H
#define POVRAY_FRONTEND_CONFIGFRONTEND_H

// Pull in other compile-time config header files first
#include "base/configbase.h"
#include "syspovconfigfrontend.h"

//##############################################################################
///
/// @defgroup PovFrontendConfig Front-End Compile-Time Configuration
/// @ingroup PovFrontend
/// @ingroup PovConfig
///
/// @{

/// @def DEFAULT_OUTPUT_FORMAT
/// The output file format used if the user doesn't specify one.
///
/// Must be one of the `kPOVList_FileType_*` enum values defined in
/// @ref povmsid.h
///
#ifndef DEFAULT_OUTPUT_FORMAT
    #define DEFAULT_OUTPUT_FORMAT   kPOVList_FileType_Targa
#endif

//******************************************************************************
///
/// @name Default Display Gamma
///
/// Default `Display_Gamma` INI setting.
///
/// The information from these settings is used for `Display_Gamma` when there
/// isn't one specified by the user in the POVRAY.INI.  For those systems that
/// are very savvy, these could be functions which return the current display
/// gamma.
///
/// @{

/// @def DEFAULT_DISPLAY_GAMMA_TYPE
/// General gamma curve type, as defined in @ref GammaTypeId.
///
#ifndef DEFAULT_DISPLAY_GAMMA_TYPE
    #define DEFAULT_DISPLAY_GAMMA_TYPE kPOVList_GammaType_SRGB
#endif

/// @def DEFAULT_DISPLAY_GAMMA
/// Gamma curve numerial parameter.
///
/// If @ref DEFAULT_DISPLAY_GAMMA_TYPE is set to @ref kPOVList_GammaType_PowerLaw,
/// this is the overall effective gamma of the display system.
///
#ifndef DEFAULT_DISPLAY_GAMMA
    #define DEFAULT_DISPLAY_GAMMA 2.2
#endif

/// @}
///
//******************************************************************************

/// @def POVMSLongToCDouble
/// Macro to convert values of type @ref POVMSLong to `double`.
///
/// This macro converts a `POVMSLong` 64 bit value to a double precision
/// floating point value, to allow further processing of such values (albeit at
/// potentially lower precision) even if it is a compound data type.
///
#ifndef POVMSLongToCDouble
    #define POVMSLongToCDouble(x) double(x)
#endif

//******************************************************************************
///
/// @name Debug Settings.
///
/// The following settings enable or disable certain debugging aids, such as run-time sanity checks
/// or additional log output.
///
/// Unless noted otherwise, a non-zero integer will enable the respective debugging aids, while a
/// zero value will disable them.
///
/// It is recommended that system-specific configurations leave these settings undefined in release
/// builds, in which case they will default to @ref POV_DEBUG unless noted otherwise.
///
/// @{

/// @def POV_FRONTEND_DEBUG
/// Enable run-time sanity checks for the @ref PovFrontend.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_FRONTEND_DEBUG
    #define POV_FRONTEND_DEBUG POV_DEBUG
#endif

/// @}
///
//******************************************************************************
///
/// @name Non-Configurable Macros
///
/// The following macros are configured automatically at compile-time; they cannot be overridden by
/// system-specific configuration.
///
/// @{

#if POV_FRONTEND_DEBUG
    #define POV_FRONTEND_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_FRONTEND_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_FRONTEND_CONFIGFRONTEND_H
