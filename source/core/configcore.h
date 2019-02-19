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

#ifndef POVRAY_CORE_CONFIGCORE_H
#define POVRAY_CORE_CONFIGCORE_H

// Pull in other compile-time config header files first
#include "base/configbase.h"
#include "syspovconfigcore.h"

//##############################################################################
///
/// @defgroup PovCoreConfig Core Compile-Time Configuration
/// @ingroup PovCore
/// @ingroup PovConfig
///
/// @{

//******************************************************************************
///
/// @name PooledSimpleVector Sizes
///
/// These defines affect the initial size of some types based on @ref pov::PooledSimpleVector.
///
/// @todo
///     These sizes may need tweaking.
///
/// @{

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

/// @def POV_VECTOR_POOL_SIZE
/// Initial size of @ref PooledSimpleVector pools.
#ifndef POV_VECTOR_POOL_SIZE
#define POV_VECTOR_POOL_SIZE            16
#endif

/// @def POV_SIMPLE_VECTOR
/// Vector type optimized for performance.
/// May be either `std::vector`, `pov::SimpleVector`, or a compatible template.
#ifndef POV_SIMPLE_VECTOR
#define POV_SIMPLE_VECTOR               pov::SimpleVector
#endif

/// @}
///
//******************************************************************************

/// @def MAX_TRACE_LEVEL_DEFAULT
/// Default for Max_Trace_Level.
///
#ifndef MAX_TRACE_LEVEL_DEFAULT
    #define MAX_TRACE_LEVEL_DEFAULT 5
#endif

/// @def MAX_TRACE_LEVEL_LIMIT
/// Upper bound for max_trace_level specified by the user.
///
#ifndef MAX_TRACE_LEVEL_LIMIT
    #define MAX_TRACE_LEVEL_LIMIT 256
#endif

//******************************************************************************
///
/// @name Various Numerical Constants
///
/// Various numerical constants that are used in the calculations.
///
/// @note
///     The ideal values for these constants depend on the numerical precision
///     of floating-point maths. The default values were chosen under the
///     presumption that double-precision IEEE format is used.
///
/// @todo
///     The algorithms using these constants need thorough reviewing. Ideally,
///     adaptive comparisons should be used that take into account the absolute
///     magnitude of the scene dimensions.
///
/// @{

/// @def EPSILON
/// A small value used to see if a value is nearly zero.
///
#ifndef EPSILON
    #define EPSILON 1.0e-10
#endif

/// @def HUGE_VAL
/// A very large value, can be considered infinity.
///
/// @deprecated
///     New code portions should use proper infinities or the corresponding
///     data types' `std::numeric_limits<>::max()` instead.
///
#ifndef HUGE_VAL
    #define HUGE_VAL 1.0e+17
#endif

/// @def CRITICAL_LENGTH
/// Bounding box critical length.
///
/// If the width of a bounding box in one dimension is greater than
/// the critical length, the bounding box should be set to infinite.
///
#ifndef CRITICAL_LENGTH
    #define CRITICAL_LENGTH 1.0e+6
#endif

/// @def BOUND_HUGE
/// Maximum lengths of a bounding box.
///
/// If the width of a bounding box in one dimension is greater than
/// the critical length, the bounding box should be set to infinite.
///
#ifndef BOUND_HUGE  // Maximum lengths of a bounding box.
    #define BOUND_HUGE 2.0e+10
#endif

/// @def SMALL_TOLERANCE
/// Minimum distance that qualifies as ray-object intersection.
///
/// @note
///     Some algorithms use @ref MIN_ISECT_DEPTH instead.
///
#define SMALL_TOLERANCE 0.001

/// @def MAX_DISTANCE
/// Maximum distance that qualifies as ray-object intersection.
///
#define MAX_DISTANCE 1.0e7

/// @def MIN_ISECT_DEPTH
/// Minimum distance that qualifies as ray-object intersection.
///
/// @note
///     Some algorithms use @ref SMALL_TOLERANCE instead.
///
#define MIN_ISECT_DEPTH 1.0e-4

/// @}
///
//******************************************************************************

/// @def TRY_OPTIMIZED_NOISE
/// Whether the platform provides dynamic optimized noise.
///
/// Define if the platform provides one or more alternative optimized implementations of the noise
/// generator, to be dispatched dynamically at run-time. Leave undefined otherwise.
///
/// @note
///     If this macro is defined, the platform must implement the function
///     @ref pov::TryOptimizedNoise() as declared in @ref core/material/texture.h.
///
#ifndef TRY_OPTIMIZED_NOISE
    // leave undefined
    #ifdef DOXYGEN
        // Doxygen cannot document undefined macros; also, we want to force declaration of the
        // TryOptimizedNoise() function.
        #define TRY_OPTIMIZED_NOISE
    #endif
#endif

/// @def C99_COMPATIBLE_RADIOSITY
/// @deprecated
///     This is effectively a legacy alias for @ref POV_PORTABLE_RADIOSITY,
///     which takes precedence if defined.
///
#ifndef C99_COMPATIBLE_RADIOSITY
    #define C99_COMPATIBLE_RADIOSITY 0
#endif

/// @def POV_PORTABLE_RADIOSITY
/// Whether to implement radiosity octree in a portable manner.
///
/// This setting selects one of several implementations of certain operations
/// in the octree code:
///
///   - @ref Pow2Floor(): Rounds a positive value down towards the next lower
///     power of 2.
///   - @ref BiasedIntLog2(): Computes the (biased) integer base-2 logarithm
///     of a positive value.
///   - @ref BiasedIntPow2(): Computes 2 to a (biased) integer ppwer.
///
/// The available implementations are based on the following primitives:
///
/// | Value | Pow2Floor                   | BiasedIntLog2 | BiasedIntPow2       |  Bias | Prerequisites                         |
/// | ----: | :---------------------------- | :-------- | :-------------------- | ----: | :------------------------------------ |
/// |     0 | `float` bit bashing                                             |||  +127 | `float` must be IEEE 754 "binary32"   |
/// |     1 | `logbf`, `pow`                | `logbf`   | `int` bit shifting    |     0 | `float` must be radix-2               |
/// |     2 | `ilogbf`, `int` bit shifting  | `ilogbf`  | ^                     |     ^ | ^                                     |
/// |     3 | `logb`, `pow`                 | `logb`    | ^                     |     ^ | `double` must be radix-2              |
/// |     4 | `ilogb`, `int` bit shifting   | `ilogb`   | ^                     |     ^ | ^                                     |
/// |     5 | `double` bit bashing                                            ||| +1023 | `double` must be IEEE 754 "binary64"  |
/// |     6 | `frexpf`, `ldexpf`            | `frexpf`  | `ldexpf`              |     0 | none                                  |
/// |     7 | `ilogbf`, `ldexpf`            | `ilogbf`  | `ldexpf`              |     ^ | `float` must be radix-2               |
/// |     8 | `frexp`, `ldexp`              | `frexp`   | `ldexp`               |     ^ | none                                  |
/// |     9 | `ilogb`, `ldexp`              | `ilogb`   | `ldexp`               |     ^ | `double` must be radix-2              |
///
/// @note
///     Settings 1-4 are deprecated, due to their restricted numeric range.
///
/// @note
///     This setting defaults to @ref C99_COMPATIBLE_RADIOSITY, which in turn
///     defaults to 0.
///
#ifndef POV_PORTABLE_RADIOSITY
    #define POV_PORTABLE_RADIOSITY C99_COMPATIBLE_RADIOSITY
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
/// builds, in which case they will default to @ref POV_CORE_DEBUG unless noted otherwise.
///
/// @{

/// @def POV_BOMB_ON_ERROR
/// Fail hard on all errors, allowing a debugger to kick in.
///
/// Define as non-zero integer to enable, or zero to disable.
///
/// If left undefined by system-specific configurations, this setting defaults to `0`.
///
/// @attention
///     This setting is _strictly_ for debugging purposes only, and should _never ever_ be enabled
///     in a release build!
///
/// @note
///     At present, this is not yet supported by all error conditions.
///
#ifndef POV_BOMB_ON_ERROR
    #define POV_BOMB_ON_ERROR 0
#endif

/// @def POV_CORE_DEBUG
/// Default setting for enabling or disabling @ref PovCore debugging aids.
///
/// This setting specifies the default for all debugging switches throughout the entire module
/// that are not explicitly enabled or disabled by system-specific configurations.
///
/// If left undefined by system-specific configurations, this setting defaults to @ref POV_DEBUG.
///
#ifndef POV_CORE_DEBUG
    #define POV_CORE_DEBUG POV_DEBUG
#endif

/// @def POV_BLEND_MAP_DEBUG
/// Enable run-time sanity checks for blend maps.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_BLEND_MAP_DEBUG
    #define POV_BLEND_MAP_DEBUG POV_CORE_DEBUG
#endif

/// @def POV_PATTERN_DEBUG
/// Enable run-time sanity checks for pattern handling.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_PATTERN_DEBUG
    #define POV_PATTERN_DEBUG POV_CORE_DEBUG
#endif

/// @def POV_PHOTONS_DEBUG
/// Enable run-time sanity checks for photons.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_PHOTONS_DEBUG
    #define POV_PHOTONS_DEBUG POV_CORE_DEBUG
#endif

/// @def POV_PIGMENT_DEBUG
/// Enable run-time sanity checks for pigment handling.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_PIGMENT_DEBUG
    #define POV_PIGMENT_DEBUG POV_CORE_DEBUG
#endif

/// @def POV_RADIOSITY_DEBUG
/// Enable run-time sanity checks for radiosity.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_RADIOSITY_DEBUG
    #define POV_RADIOSITY_DEBUG POV_CORE_DEBUG
#endif

/// @def POV_RANDOMSEQUENCE_DEBUG
/// Enable run-time sanity checks for random sequence generation.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_RANDOMSEQUENCE_DEBUG
    #define POV_RANDOMSEQUENCE_DEBUG POV_CORE_DEBUG
#endif

/// @def POV_REFPOOL_DEBUG
/// Enable run-time sanity checks for reference pool mechanism.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_REFPOOL_DEBUG
    #define POV_REFPOOL_DEBUG POV_CORE_DEBUG
#endif

/// @def POV_SHAPE_DEBUG
/// Enable run-time sanity checks for geometric shapes.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_SHAPE_DEBUG
    #define POV_SHAPE_DEBUG POV_CORE_DEBUG
#endif

/// @def POV_SUBSURFACE_DEBUG
/// Enable run-time sanity checks for subsurface light transport.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_SUBSURFACE_DEBUG
    #define POV_SUBSURFACE_DEBUG POV_CORE_DEBUG
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

#if POV_CORE_DEBUG
    #define POV_CORE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_CORE_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_BLEND_MAP_DEBUG
    #define POV_BLEND_MAP_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_BLEND_MAP_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_PATTERN_DEBUG
    #define POV_PATTERN_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_PATTERN_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_PHOTONS_DEBUG
    #define POV_PHOTONS_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_PHOTONS_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_PIGMENT_DEBUG
    #define POV_PIGMENT_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_PIGMENT_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_RADIOSITY_DEBUG
    #define POV_RADIOSITY_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_RADIOSITY_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_RANDOMSEQUENCE_DEBUG
    #define POV_RANDOMSEQUENCE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_RANDOMSEQUENCE_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_REFPOOL_DEBUG
    #define POV_REFPOOL_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_REFPOOL_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_SHAPE_DEBUG
    #define POV_SHAPE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_SHAPE_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_SUBSURFACE_DEBUG
    #define POV_SUBSURFACE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_SUBSURFACE_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_CORE_CONFIGCORE_H
