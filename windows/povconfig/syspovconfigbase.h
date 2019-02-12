//******************************************************************************
///
/// @file windows/povconfig/syspovconfigbase.h
///
/// Windows-specific POV-Ray base compile-time configuration.
///
/// This header file configures aspects of POV-Ray's base module for running
/// properly on a Windows platform.
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

#ifndef POVRAY_WINDOWS_SYSPOVCONFIGBASE_H
#define POVRAY_WINDOWS_SYSPOVCONFIGBASE_H

#include "syspovconfig.h"

#ifdef _DEBUG
    #define POV_DEBUG 1
    #define POV_BOMB_ON_ERROR 0
#else
    #define POV_DEBUG 0
#endif

// Windows provides a platform-specific mechanism to let a task wait for a specified time.
// However, the C++11 standard mechanism should be fine.
#define POV_USE_PLATFORM_DELAY 0

// Windows provides platform-specific mechanisms to measure both wall-clock and CPU time.
#define POV_USE_DEFAULT_TIMER 0

// Windows requires platform-specific parsing of path name strings.
#define POV_USE_DEFAULT_PATH_PARSER 0

// Windows requires a platform-specific function to delete a file.
#define POV_USE_DEFAULT_DELETEFILE 0

// Windows gets a platform-specific implementation of large file handling.
#define POV_USE_DEFAULT_LARGEFILE 0

#endif // POVRAY_WINDOWS_SYSPOVCONFIGBASE_H
