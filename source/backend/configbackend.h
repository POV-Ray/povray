//******************************************************************************
///
/// @file backend/configbackend.h
///
/// This header file defines all types that can be configured by platform
/// specific code for backend use. It further allows insertion of platform
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

#ifndef POVRAY_BACKEND_CONFIGBACKEND_H
#define POVRAY_BACKEND_CONFIGBACKEND_H

#include "base/configbase.h"
#include "syspovconfigbackend.h"

/*
 * Platform name default.
 */
#ifndef POVRAY_PLATFORM_NAME
    #define POVRAY_PLATFORM_NAME "Unknown Platform"
#endif

/*
 * To allow GUI platforms like the Mac to access a command line and provide
 * a command line only interface (for debugging) a different call to an
 * internal function of the standard library is required. This macro takes
 * both argc and argv and is expected to return argc.
 */
#ifndef GETCOMMANDLINE
    #define GETCOMMANDLINE(ac,av) ac
#endif

#ifndef CONFIG_MATH       // Macro for setting up any special FP options
    #define CONFIG_MATH
#endif

/* Specify number of source file lines printed before error line, their maximum length and
 * the error marker text that is appended to mark the error
 */
#ifndef POV_NUM_ECHO_LINES
    #define POV_NUM_ECHO_LINES 5
#endif

#ifndef POV_ERROR_MARKER_TEXT
    #define POV_ERROR_MARKER_TEXT " <----ERROR\n"
#endif

#ifndef POV_WHERE_ERROR
    #define POV_WHERE_ERROR(fn,ln,cl,ts)
#endif

#ifndef DBL_FORMAT_STRING
    #define DBL_FORMAT_STRING "%lf"
#endif

// Some implementations of scanf return 0 on failure rather than EOF
#ifndef SCANF_EOF
    #define SCANF_EOF EOF
#endif

#ifndef POV_SYS_THREAD_STARTUP
    #define POV_SYS_THREAD_STARTUP
#endif

#ifndef POV_SYS_THREAD_CLEANUP
    #define POV_SYS_THREAD_CLEANUP
#endif

#ifndef NEW_LINE_STRING
    // NEW_LINE_STRING remains undefined, optimizing the code for "\n" as used internally
#endif

// If compiler version is undefined, then make it 'u' for unknown
#ifndef COMPILER_VER
    #define COMPILER_VER ".u"
#endif

#ifndef POV_PARSE_PATH_STRING
    #error "A valid POV_PARSE_PATH_STRING macro is required!"
#endif


/*
 * Font related macros [trf]
 */
#ifndef POV_CONVERT_TEXT_TO_UCS2
    #define POV_CONVERT_TEXT_TO_UCS2(ts, tsl, as) (NULL)
#endif

#ifndef POV_ALLOW_FILE_READ
    #define POV_ALLOW_FILE_READ(f,t) (1)
#endif

#ifndef POV_ALLOW_FILE_WRITE
    #define POV_ALLOW_FILE_WRITE(f,t) (1)
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

/// @def POV_RTR_DEBUG
/// Enable run-time sanity checks for real-time rendering.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_RTR_DEBUG
    #define POV_RTR_DEBUG POV_DEBUG
#endif

/// @def POV_TASK_DEBUG
/// Enable run-time sanity checks for task handling.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_TASK_DEBUG
    #define POV_TASK_DEBUG POV_DEBUG
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

#if POV_RTR_DEBUG
    #define POV_RTR_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_RTR_ASSERT(expr) NO_OP
#endif

#if POV_TASK_DEBUG
    #define POV_TASK_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_TASK_ASSERT(expr) NO_OP
#endif

/// @def HAVE_BOOST_THREAD_ATTRIBUTES
/// Whether boost::thread::attributes is available (and can be used to set a thread's stack size).
///
#if BOOST_VERSION >= 105000
    #define HAVE_BOOST_THREAD_ATTRIBUTES 1
#else
    #define HAVE_BOOST_THREAD_ATTRIBUTES 0
#endif

/// @}
///
//******************************************************************************

#include "syspovprotobackend.h"

#endif // POVRAY_BACKEND_CONFIGBACKEND_H
