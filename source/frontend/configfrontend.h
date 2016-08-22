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

#ifndef CONFIGFRONTEND_H
#define CONFIGFRONTEND_H

#include "base/configbase.h"
#include "syspovconfigfrontend.h"

// The output file format used if the user doesn't specify one
#ifndef DEFAULT_OUTPUT_FORMAT
    #define DEFAULT_OUTPUT_FORMAT   kPOVList_FileType_Targa
#endif

/*
 * The DEFAULT_DISPLAY_GAMMA is used when there isn't one specified by the
 * user in the POVRAY.INI.  For those systems that are very savvy, this
 * could be a function which returns the current display gamma.
 */
#ifndef DEFAULT_DISPLAY_GAMMA_TYPE
    #define DEFAULT_DISPLAY_GAMMA_TYPE kPOVList_GammaType_SRGB
#endif
#ifndef DEFAULT_DISPLAY_GAMMA
    #define DEFAULT_DISPLAY_GAMMA 2.2
#endif

#ifndef DEFAULT_FILE_GAMMA_TYPE
    #define DEFAULT_FILE_GAMMA_TYPE kPOVList_GammaType_SRGB
#endif
#ifndef DEFAULT_FILE_GAMMA
    #define DEFAULT_FILE_GAMMA 2.2
#endif

/// @def POVMSLongToCDouble
/// Macro to convert values of type @ref POVMSLong to `double`.
/// This macro converts a `POVMSLong` 64 bit value to a double precision floating point value, to allow further
/// processing of such values (albeit at potentially lower precision) even if it is a compound data type.
#ifndef POVMSLongToCDouble
    #define POVMSLongToCDouble(x) double(x)
#endif

#include "syspovprotofrontend.h"

#endif
