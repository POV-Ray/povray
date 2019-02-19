//******************************************************************************
///
/// @file unix/povconfig/syspovconfig_bsd.h
///
/// BSD Unix flavor-specific POV-Ray compile-time configuration.
///
/// This header file configures aspects of POV-Ray for running properly on a
/// BSD-style Unix platform.
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

#ifndef POVRAY_UNIX_SYSPOVCONFIG_BSD_H
#define POVRAY_UNIX_SYSPOVCONFIG_BSD_H

#include <sys/types.h>  // Pulled in for `off_t`.
#include <unistd.h>     // Pulled in for `_POSIX_V6_xxx`.

// BSD does not seem to provide an explicit 64-bit `lseek`, so we're stuck with
// whatever size `off_t` happens to have.
#define POVUNIX_LSEEK64(h,o,w) lseek(h,o,w)
#define POV_OFF_T off_t

/// @file
/// @todo The POV_LONG stuff is just copied from the Posix settings; someone needs to verify universal BSD compatibility.
#if defined(_POSIX_V6_LPBIG_OFFBIG) || defined(_POSIX_V6_LP64_OFF64)
    // long is at least 64 bits.
    #define POV_LONG long
#elif defined(_POSIX_V6_ILP32_OFFBIG) || defined(_POSIX_V6_ILP32_OFF32)
    // long is 32 bits.
    #define POV_LONG long long
#else
    // Unable to detect long size at compile-time, assuming less than 64 bits.
    #define POV_LONG long long
#endif

#define POV_ULONG unsigned POV_LONG

#endif // POVRAY_UNIX_SYSPOVCONFIG_BSD_H
