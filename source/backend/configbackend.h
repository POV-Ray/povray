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
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef CONFIGBACKEND_H
#define CONFIGBACKEND_H

#include "syspovconfigbackend.h"
#include "base/configbase.h"

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

// Adjust to match floating-point parameter(s) of functions in math.h/cmath
#ifndef SYS_MATH_PARAM
    #define SYS_MATH_PARAM double
#endif

// Adjust to match floating-point return value of functions in math.h/cmath
#ifndef SYS_MATH_RETURN
    #define SYS_MATH_RETURN double
#endif

// Function that executes functions, the parameter is the function index
#ifndef POVFPU_Run
    #define POVFPU_Run(ctx, fn) POVFPU_RunDefault(ctx, fn)
#endif

// Adjust to add system specific handling of functions like just-in-time compilation
#if (SYS_FUNCTIONS == 0)

// Note that if SYS_FUNCTIONS is 1, it will enable the field dblstack
// in FPUContext_Struct and corresponding calculations in POVFPU_SetLocal
// as well as POVFPU_NewContext.
#define SYS_FUNCTIONS 0

// Called after a function has been added, parameter is the function index
#define SYS_ADD_FUNCTION(fe)
// Called before a function is deleted, parameter is a pointer to the FunctionEntry_Struct
#define SYS_DELETE_FUNCTION(fe)
// Called inside POVFPU_Init after everything else has been inited
#define SYS_INIT_FUNCTIONS()
// Called inside POVFPU_Terminate before anything else is deleted
#define SYS_TERM_FUNCTIONS()
// Called inside POVFPU_Reset before anything else is reset
#define SYS_RESET_FUNCTIONS()

// Adjust to add system specific fields to FunctionEntry_Struct
#define SYS_FUNCTION_ENTRY

#endif // SYS_FUNCTIONS

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

#include "syspovprotobackend.h"

#endif
