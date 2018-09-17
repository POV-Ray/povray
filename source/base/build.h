//******************************************************************************
///
/// @file base/build.h
///
/// POV-Ray build-specific information.
///
/// This header file contains macros specifying information for a specific
/// POV-Ray build, and is intended to be customized by people building their own
/// POV-Ray binaries, while generally remaining unchanged in the official source
/// code repository.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_BASE_BUILD_H
#define POVRAY_BASE_BUILD_H

#include "base/configbase.h"

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

//******************************************************************************
///
/// @name Build-Specific Information
///
/// @{

/// @def POV_BUILD_INFO
/// Additional version information pertaining to the build.
///
/// This optional macro should concisely identify the target runtime environment
/// as well as the build environment used. For instance, POV-Ray for Windows,
/// 32-bit binary with SSE2 enabled, built with Visual Studio 2015, might be
/// identified as `msvc14.sse2.win32`.
///
/// @note
///     If defined, this macro must evaluate to an ASCII string containing only
///     alphanumeric characters or dots.
///
/// The content of this macro will be included in the binary's version string,
/// separated from the source code version by a `+` character.
///
/// The intent is to be able to distinguish binaries generated from the same
/// source code version but for different target platforms or using different
/// build environments.
///
#if defined(DOXYGEN) && !defined(POV_BUILD_INFO)
    // Work around doxygen being unable to document undefined macros.
    #define POV_BUILD_INFO (undefined)
    #undef POV_BUILD_INFO
#endif

/// @def POV_RAY_BUILD_ID
/// Unique build identifier string.
///
/// This optional macro should be set to a different string each time the build
/// process is run, for example by setting it to a timestamp or the value of a
/// counter automatically incremented before each build.
///
/// @note
///     If defined, this macro must evaluate to an ASCII string containing only
///     alphanumeric characters or dots.
///
/// The content of this macro will effectively be appended to the
/// @ref POV_BUILD_INFO, separated by a `.` character.
///
/// The intent is to be able to distinguish binaries generated from the same
/// source code version for different target platforms and using similar build
/// environments, but with potential minute differences that may or may not
/// cause differences in the binaries.
///
/// @note
///     This macro should never be defined in any source code, but rather
///     injected via other means as part of the build process, e.g. via a
///     compiler option (`-DPOV_RAY_BUILD_ID=...` in gcc).
///
#if defined(DOXYGEN) && !defined(POV_RAY_BUILD_ID)
    // Work around doxygen being unable to document undefined macros.
    #define POV_RAY_BUILD_ID (undefined)
    #undef POV_RAY_BUILD_ID
#endif

/// @def BUILT_BY
/// Specifies the person or organization responsible for this build.
///
/// @attention
///     Please set this to your real name, and/or include a working email or
///     website address to contact you.
///
/// @note
///     It is recommended to avoid editing this macro in the source code,
///     and instead inject the macro via other means. If this is not feasible
///     and you are working in a git workspace, it is recommended to run
///     `git update-index --skip-worktree source/base/build.h` to avoid
///     undesired interference between the repository and your local changes.
///
#ifndef BUILT_BY
    #define BUILT_BY "YOUR NAME (YOUR EMAIL)"
    #error "Please fill in BUILT_BY, then remove this line"
#endif

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_BASE_BUILD_H
