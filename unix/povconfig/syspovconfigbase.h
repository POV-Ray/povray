//******************************************************************************
///
/// @file unix/povconfig/syspovconfigbase.h
///
/// Unix-specific POV-Ray base compile-time configuration.
///
/// This header file configures aspects of POV-Ray's base module for running
/// properly on a Unix platform.
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

#ifndef POVRAY_UNIX_SYSPOVCONFIGBASE_H
#define POVRAY_UNIX_SYSPOVCONFIGBASE_H

#include "syspovconfig.h"

#define POV_PATH_SEPARATOR '/'

// Our Unix-specific implementation of the Delay() function currently relies on the presence of
// the nanosleep() or usleep() functions. If we have neither of those, we're falling back to
// POV-Ray's platform-independent default implementation.
#ifndef POV_USE_PLATFORM_DELAY // allow for flavour-specific override in `syspovconfig.h`.
    #if defined(HAVE_NANOSLEEP)
        // While the default implementation _should_ do exactly what we want,
        // for now we prefer `nanosleep()` because we presume to have a better
        // understanding of its limitations and pitfalls.
        #define POV_USE_PLATFORM_DELAY 1
    #elif defined(HAVE_USLEEP)
        // `usleep()` definitely has some unwieldy quirks, so we presume we'll
        // be better off with the default.
        #define POV_USE_PLATFORM_DELAY 0 // use 2 for `usleep()`
    #else
        // We have no other choice but the default.
        #define POV_USE_PLATFORM_DELAY 0
    #endif
#endif

// Our Unix-specific implementation of the Timer class currently relies on the presence of the
// clock_gettime() or gettimeofday() functions, in order to measure at least wall-clock time. If we
// have neither of those, we're falling back to POV-Ray's platform-independent default
// implementation.
// (Note that when it comes to measuring CPU time, we're still as good as the default if we fall
// back to reporting wall clock time.)
#if defined(HAVE_CLOCK_GETTIME) || defined(HAVE_GETTIMEOFDAY)
    #define POV_USE_DEFAULT_TIMER 0
#else
    #define POV_USE_DEFAULT_TIMER 1
#endif

// The default Path::ParsePathString() suits our needs perfectly.
#define POV_USE_DEFAULT_PATH_PARSER 1

// We want to implement a specialized Filesystem::DeleteFile.
#define POV_USE_DEFAULT_DELETEFILE 0

// We want to implement a specialized Filesystem::LargeFile.
#define POV_USE_DEFAULT_LARGEFILE 0

#endif // POVRAY_UNIX_SYSPOVCONFIGBASE_H
