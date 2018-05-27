/*******************************************************************************
 * configbackend.h
 *
 * This header file defines all types that can be configured by platform
 * specific code for backend use. It further allows insertion of platform
 * specific function prototypes making use of those types.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/configbackend.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef CONFIGBACKEND_H
#define CONFIGBACKEND_H

#include "syspovconfigbackend.h"

/*
 * Platform name default.
 */
#ifndef POVRAY_PLATFORM_NAME
	#define POVRAY_PLATFORM_NAME "Unknown Platform"
#endif

/*
 * These functions define macros which do checking for memory allocation,
 * and can also do other things.  Check existing code before you change them,
 * since they aren't simply replacements for malloc, calloc, realloc, and free.
 */
#ifndef POV_MALLOC
#define POV_MALLOC(size,msg)        pov_malloc ((size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_CALLOC
	#define POV_CALLOC(nitems,size,msg) pov_calloc ((nitems), (size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_REALLOC
	#define POV_REALLOC(ptr,size,msg)   pov_realloc ((ptr), (size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_FREE
	#define POV_FREE(ptr)               do { pov_free (static_cast<void *>(ptr), __FILE__, __LINE__); (ptr) = NULL; } while(false)
#endif

#ifndef POV_MEM_INIT
	#define POV_MEM_INIT()              mem_init()
#endif

#ifndef POV_MEM_RELEASE_ALL
	#define POV_MEM_RELEASE_ALL()       mem_release_all()
#endif

#ifndef POV_STRDUP
	#define POV_STRDUP(str)             pov_strdup(str)
#endif

// For those systems that don't have memmove, this can also be pov_memmove
#ifndef POV_MEMMOVE
	#define POV_MEMMOVE(dst,src,len)    pov_memmove((dst),(src),(len))
#endif

#ifndef POV_MEMCPY
	#define POV_MEMCPY(dst,src,len)     memcpy((dst),(src),(len))
#endif

#ifndef POV_MEM_STATS
	#define POV_MEM_STATS                       0
	#define POV_GLOBAL_MEM_STATS(a,f,c,p,s,l)   (false)
	#define POV_THREAD_MEM_STATS(a,f,c,p,s,l)   (false)
	#define POV_MEM_STATS_RENDER_BEGIN()
	#define POV_MEM_STATS_RENDER_END()
	#define POV_MEM_STATS_COOKIE                void *
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

// Default for Max_Trace_Level
#ifndef MAX_TRACE_LEVEL_DEFAULT
	#define MAX_TRACE_LEVEL_DEFAULT 5
#endif

// Upper bound for max_trace_level specified by the user
#ifndef MAX_TRACE_LEVEL_LIMIT
	#define MAX_TRACE_LEVEL_LIMIT 256
#endif

// Various numerical constants that are used in the calculations
#ifndef EPSILON     // A small value used to see if a value is nearly zero
	#define EPSILON 1.0e-10
#endif

#ifndef HUGE_VAL    // A very large value, can be considered infinity
	#define HUGE_VAL 1.0e+17
#endif

/*
 * If the width of a bounding box in one dimension is greater than
 * the critical length, the bounding box should be set to infinite.
 */

#ifndef CRITICAL_LENGTH
	#define CRITICAL_LENGTH 1.0e+6
#endif

#ifndef BOUND_HUGE  // Maximum lengths of a bounding box.
	#define BOUND_HUGE 2.0e+10
#endif

/*
 * These values determine the minimum and maximum distances
 * that qualify as ray-object intersections.
 */

//#define SMALL_TOLERANCE 1.0e-6 // TODO FIXME #define SMALL_TOLERANCE 0.001
//#define MAX_DISTANCE 1.0e+10 // TODO FIXME #define MAX_DISTANCE 1.0e7
#define SMALL_TOLERANCE 0.001
#define MAX_DISTANCE 1.0e7

#define MIN_ISECT_DEPTH 1.0e-4

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

#ifndef CDECL
	#define CDECL
#endif

#ifndef ALIGN16
	#define ALIGN16
#endif

#ifndef FORCEINLINE
	#define FORCEINLINE inline
#endif

#ifndef INLINE_NOISE
	#define INLINE_NOISE
#endif

#ifndef USE_FASTER_NOISE
	#define USE_FASTER_NOISE 0
#endif

#ifndef NEW_LINE_STRING
	#define NEW_LINE_STRING "\n"
#endif

// If compiler version is undefined, then make it 'u' for unknown
#ifndef COMPILER_VER
	#define COMPILER_VER ".u"
#endif

#ifndef QSORT
	#define QSORT(a,b,c,d) qsort((a),(b),(c),(d))
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

/// @def POV_THREAD_STACK_SIZE
/// Default thread stack size.
///
#ifndef POV_THREAD_STACK_SIZE
	#define POV_THREAD_STACK_SIZE (2 * 1024 * 1024)
#endif

/// @def HAVE_BOOST_THREAD_ATTRIBUTES
/// Whether boost::thread::attributes is available (and can be used to set a thread's stack size).
///
#if BOOST_VERSION >= 105000
	#define HAVE_BOOST_THREAD_ATTRIBUTES 1
#else
	#define HAVE_BOOST_THREAD_ATTRIBUTES 0
#endif

#include "syspovprotobackend.h"

#endif
