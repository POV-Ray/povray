//******************************************************************************
///
/// @file windows/povwin_version.h
///
/// Secondary version information for POV-Ray for Windows.
///
/// This file defines version information (and related) macros specific to
/// POV-Ray for Windows, derived from the primary version information defined in
/// @ref base/version.h.
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

#ifndef POVWIN_VERSION_H
#define POVWIN_VERSION_H

/// @file
/// @note
///     This file _must not_ pull in any POV-Ray header whatsoever, except
///     @ref base/version.h, the path of which must be specified relative to
///     this file.

#include "../source/base/version.h"

//------------------------------------------------------------------------------

#define POVWIN_APPLICATION_NAME         "POV-Ray for Windows"

// format "X.Y[-beta]"
// used for directories and similar names
#ifdef POV_RAY_HOST_VERSION
#define POVWIN_DIR_VERSION_STRING       POV_RAY_HOST_VERSION
#else
#define POVWIN_DIR_VERSION_STRING       POV_RAY_GENERATION POV_RAY_BETA_SUFFIX
#endif

/// @def POVWIN_BETA_PREFIX
/// Prefix to distinguish beta- from non-beta versions.
/// For beta releases, this macro evaluates to `Beta`. Otherwise, this macro evaluates to an
/// empty string.
#if defined(POVRAY_IS_BETA)
#define POVWIN_BETA_PREFIX "Beta"
#else
#define POVWIN_BETA_PREFIX ""
#endif

#ifdef POV_RAY_PRERELEASE
#if POV_RAY_PATCHLEVEL_INT > 0
#define POVWIN_VERSION_RC               POV_RAY_MAJOR_VERSION_INT, POV_RAY_MINOR_VERSION_INT, POV_RAY_REVISION_INT, POV_RAY_PATCHLEVEL_INT-1
#elif POV_RAY_REVISION_INT > 0
#define POVWIN_VERSION_RC               POV_RAY_MAJOR_VERSION_INT, POV_RAY_MINOR_VERSION_INT, POV_RAY_REVISION_INT-1, 9999
#elif POV_RAY_MINOR_VERSION_INT > 0
#define POVWIN_VERSION_RC               POV_RAY_MAJOR_VERSION_INT, POV_RAY_MINOR_VERSION_INT-1, 9999, 9999
#else
#define POVWIN_VERSION_RC               POV_RAY_MAJOR_VERSION_INT-1, 9999, 9999, 9999
#endif
#else
#define POVWIN_VERSION_RC               POV_RAY_MAJOR_VERSION_INT, POV_RAY_MINOR_VERSION_INT, POV_RAY_REVISION_INT, POV_RAY_PATCHLEVEL_INT
#endif

//------------------------------------------------------------------------------

#define POVWIN_WINRC_COMPANY_NAME       "Persistence of Vision Raytracer Pty. Ltd."
#define POVWIN_WINRC_FILE_DESCRIPTION   POVWIN_APPLICATION_NAME " v" POV_RAY_GENERATION POV_RAY_BETA_SUFFIX
#define POVWIN_WINRC_FILE_VERSION       POV_RAY_FULL_VERSION
#define POVWIN_WINRC_INTERNAL_NAME      "PVEngine"
#define POVWIN_WINRC_LEGAL_COPYRIGHT    POV_RAY_COPYRIGHT " All Rights Reserved. This software is licensed under the terms of the GNU Affero General Public License."
#define POVWIN_WINRC_LEGAL_TRADEMARKS   "POV-Ray(tm) is a trademark of Persistence of Vision Raytracer Pty. Ltd."
#define POVWIN_WINRC_ORIGINAL_FILENAME  "pvengine.exe"
#define POVWIN_WINRC_PRODUCT_NAME       POVWIN_APPLICATION_NAME
#define POVWIN_WINRC_PRODUCT_VERSION    POV_RAY_FULL_VERSION

#endif // POVWIN_VERSION_H
