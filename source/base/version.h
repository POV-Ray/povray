//******************************************************************************
///
/// @file base/version.h
///
/// Primary version information.
///
/// This file serves as the primary source for version information pertaining to
/// the source code. It also defines a few macros providing this information in
/// alternative formats.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
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

#ifndef POVRAY_BASE_VERSION_H
#define POVRAY_BASE_VERSION_H

/// @file
/// @note
///     This file _must not_ pull in any POV-Ray header whatsoever.

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

//******************************************************************************
///
/// @name Primary Version and Copyright Information
///
/// @{

/// Copyright string.
#define POV_RAY_COPYRIGHT "Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd."

/// Official source code version string.
/// @note For pre-releases this should be _equal to_ the planned final version.
#define OFFICIAL_VERSION_STRING "3.7.2"

/// Official source code version as integer.
/// @note For pre-releases this should be _equal to_ the planned final version.
#define OFFICIAL_VERSION_NUMBER 372

/// Official source code version formatted for Windows resource files.
/// @note For pre-releases this should be _marginally less_ than the planned final version.
#define OFFICIAL_VERSION_WINRC  3,7,1,9999

/// @def POV_RAY_PRERELEASE
/// Pre-release identifier.
/// Leave undefined for final releases.
#define POV_RAY_PRERELEASE "alpha"

/// @def POVRAY_IS_BETA
/// Whether this version is a beta.
/// Leave undefined for pre-beta, release candidate or final releases.
//#define POVRAY_IS_BETA

//------------------------------------------------------------------------------

/// @def OFFICIAL_VERSION_WINRC_STRING
/// Official source code version string for Windows resource file.
#ifdef POV_RAY_PRERELEASE
    #define OFFICIAL_VERSION_WINRC_STRING OFFICIAL_VERSION_STRING "-" POV_RAY_PRERELEASE
#else
    #define OFFICIAL_VERSION_WINRC_STRING OFFICIAL_VERSION_STRING
#endif

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_BASE_VERSION_H
