//******************************************************************************
///
/// @file core/configcore.h
///
/// This header file defines all types that can be configured by platform
/// specific code for core layer use. It further allows insertion of platform
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

#ifndef POVRAY_CORE_CONFIGCORE_H
#define POVRAY_CORE_CONFIGCORE_H

#include "base/configbase.h"
#include "syspovconfigcore.h"

namespace pov
{

//******************************************************************************
///
/// @name FixedSimpleVector Sizes
/// @{
///
/// These defines affect the maximum size of some types based on @ref pov::FixedSimpleVector.
///
/// @todo these sizes will need tweaking.

#ifndef MEDIA_VECTOR_SIZE
#define MEDIA_VECTOR_SIZE               256
#endif

#ifndef MEDIA_INTERVAL_VECTOR_SIZE
#define MEDIA_INTERVAL_VECTOR_SIZE      256
#endif

#ifndef LIT_INTERVAL_VECTOR_SIZE
#define LIT_INTERVAL_VECTOR_SIZE        512
#endif

#ifndef LIGHT_INTERSECTION_VECTOR_SIZE
#define LIGHT_INTERSECTION_VECTOR_SIZE  512 // TODO - I think this should be LIGHTSOURCE_VECTOR_SIZE*2 [CLi]
#endif

#ifndef LIGHTSOURCE_VECTOR_SIZE
#define LIGHTSOURCE_VECTOR_SIZE         1024
#endif

#ifndef WEIGHTEDTEXTURE_VECTOR_SIZE
#define WEIGHTEDTEXTURE_VECTOR_SIZE     512
#endif

#ifndef RAYINTERIOR_VECTOR_SIZE
#define RAYINTERIOR_VECTOR_SIZE         512
#endif

/// @}
///
//******************************************************************************

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

#ifndef INLINE_NOISE
    #define INLINE_NOISE
#endif

#ifndef USE_FASTER_NOISE
    #define USE_FASTER_NOISE 0
#endif

#ifndef QSORT
    #define QSORT(a,b,c,d) qsort((a),(b),(c),(d))
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

/// @def POV_BLEND_MAP_DEBUG
/// Enable run-time sanity checks for blend maps.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_BLEND_MAP_DEBUG
    #define POV_BLEND_MAP_DEBUG POV_DEBUG
#endif

/// @def POV_SHAPE_DEBUG
/// Enable run-time sanity checks for geometric shapes.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_SHAPE_DEBUG
    #define POV_SHAPE_DEBUG POV_DEBUG
#endif

/// @def POV_PATTERN_DEBUG
/// Enable run-time sanity checks for pattern handling.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_PATTERN_DEBUG
    #define POV_PATTERN_DEBUG POV_DEBUG
#endif

/// @def POV_RADIOSITY_DEBUG
/// Enable run-time sanity checks for radiosity.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_RADIOSITY_DEBUG
    #define POV_RADIOSITY_DEBUG POV_DEBUG
#endif

/// @def POV_RANDOMSEQUENCE_DEBUG
/// Enable run-time sanity checks for random sequence generation.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_RANDOMSEQUENCE_DEBUG
    #define POV_RANDOMSEQUENCE_DEBUG POV_DEBUG
#endif

/// @def POV_REFPOOL_DEBUG
/// Enable run-time sanity checks for reference pool mechanism.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_REFPOOL_DEBUG
    #define POV_REFPOOL_DEBUG POV_DEBUG
#endif

/// @def POV_SUBSURFACE_DEBUG
/// Enable run-time sanity checks for subsurface light transport.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_SUBSURFACE_DEBUG
    #define POV_SUBSURFACE_DEBUG POV_DEBUG
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

#if POV_BLEND_MAP_DEBUG
    #define POV_BLEND_MAP_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_BLEND_MAP_ASSERT(expr) NO_OP
#endif

#if POV_SHAPE_DEBUG
    #define POV_SHAPE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_SHAPE_ASSERT(expr) NO_OP
#endif

#if POV_PATTERN_DEBUG
    #define POV_PATTERN_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_PATTERN_ASSERT(expr) NO_OP
#endif

#if POV_RADIOSITY_DEBUG
    #define POV_RADIOSITY_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_RADIOSITY_ASSERT(expr) NO_OP
#endif

#if POV_RANDOMSEQUENCE_DEBUG
    #define POV_RANDOMSEQUENCE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_RANDOMSEQUENCE_ASSERT(expr) NO_OP
#endif

#if POV_REFPOOL_DEBUG
    #define POV_REFPOOL_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_REFPOOL_ASSERT(expr) NO_OP
#endif

#if POV_SUBSURFACE_DEBUG
    #define POV_SUBSURFACE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_SUBSURFACE_ASSERT(expr) NO_OP
#endif

/// @}
///
//******************************************************************************

}

#endif // POVRAY_CORE_CONFIGCORE_H
