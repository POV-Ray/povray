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

#ifndef POVWIN_VERSION_H
#define POVWIN_VERSION_H

/// @file
/// @note
///     This file _must not_ pull in any POV-Ray header whatsoever, except
///     @ref base/version.h

#include "base/version.h"

//------------------------------------------------------------------------------

#define POVWIN_APPLICATION_NAME         "POV-Ray for Windows"

#ifdef POVRAY_IS_BETA
#define POVWIN_GENERATION_STRING        OFFICIAL_GENERATION_STRING "-beta"
#else
#define POVWIN_GENERATION_STRING        OFFICIAL_GENERATION_STRING
#endif

#ifdef POV_RAY_PRERELEASE
#define POVWIN_VERSION_STRING           OFFICIAL_VERSION_STRING "-" POV_RAY_PRERELEASE
#else
#define POVWIN_VERSION_STRING           OFFICIAL_VERSION_STRING
#endif

//------------------------------------------------------------------------------

#define POVWIN_WINRC_COMPANY_NAME       "Persistence of Vision Raytracer Pty. Ltd."
#define POVWIN_WINRC_FILE_DESCRIPTION   POVWIN_APPLICATION_NAME " v" POVWIN_GENERATION_STRING
#define POVWIN_WINRC_FILE_VERSION       POVWIN_VERSION_STRING
#define POVWIN_WINRC_INTERNAL_NAME      "PVEngine"
#define POVWIN_WINRC_LEGAL_COPYRIGHT    POV_RAY_COPYRIGHT " All Rights Reserved. This software is licensed under the terms of the GNU Affero General Public License."
#define POVWIN_WINRC_LEGAL_TRADEMARKS   "POV-Ray(tm) is a trademark of Persistence of Vision Raytracer Pty. Ltd."
#define POVWIN_WINRC_ORIGINAL_FILENAME  "pvengine.exe"
#define POVWIN_WINRC_PRODUCT_NAME       POVWIN_APPLICATION_NAME
#define POVWIN_WINRC_PRODUCT_VERSION    POVWIN_VERSION_STRING

#endif // POVWIN_VERSION_H
