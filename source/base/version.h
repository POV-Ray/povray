//******************************************************************************
///
/// @file base/version.h
///
/// Version information.
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

#ifndef POVRAY_BASE_VERSION_H
#define POVRAY_BASE_VERSION_H

#include "base/configbase.h"
#include "base/build.h"

// POV-Ray version and copyright message macros

#define POV_RAY_COPYRIGHT "Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd."
#define OFFICIAL_VERSION_STRING "3.7.1"
#define OFFICIAL_VERSION_NUMBER 371

#define POV_RAY_PRERELEASE "alpha.8772266"

#if (POV_RAY_IS_AUTOBUILD == 1) && ((POV_RAY_IS_OFFICIAL == 1) || (POV_RAY_IS_SEMI_OFFICIAL == 1))
#ifdef POV_RAY_PRERELEASE
#define POV_RAY_VERSION OFFICIAL_VERSION_STRING "-" POV_RAY_PRERELEASE "+" POV_RAY_AUTOBUILD_ID
#else
#define POV_RAY_VERSION OFFICIAL_VERSION_STRING "+" POV_RAY_AUTOBUILD_ID
#endif
#elif (POV_RAY_IS_OFFICIAL == 1)
#ifdef POV_RAY_PRERELEASE
#define POV_RAY_VERSION OFFICIAL_VERSION_STRING "-" POV_RAY_PRERELEASE
#else
#define POV_RAY_VERSION OFFICIAL_VERSION_STRING
#endif
#else
#ifdef POV_RAY_PRERELEASE
#define POV_RAY_VERSION OFFICIAL_VERSION_STRING "-" POV_RAY_PRERELEASE ".unofficial"
#else
#define POV_RAY_VERSION OFFICIAL_VERSION_STRING "-unofficial"
#endif
#endif

#endif // POVRAY_BASE_VERSION_H
