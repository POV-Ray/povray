//******************************************************************************
///
/// @file base/configbase.h
///
/// This header file defines all types that can be configured by platform
/// specific code for base-layer use. It further allows insertion of platform
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

#ifndef POVRAY_BASE_CONFIGBASE_H
#define POVRAY_BASE_CONFIGBASE_H

#include "syspovconfigbase.h"

#include <boost/version.hpp>

//******************************************************************************
///
/// @name Fundamental Data Types
///
/// The following macros define essential data types. It is recommended that system-specific
/// configurations always override the defaults; although POV-Ray will make a solid guess as to
/// which type fits the requirements, the algorithm may fail on some exotic systems.
///
/// @compat
///     The automatic type detection is almost certain fail on systems that employ padding bits
///     and/or do not use two's complement format for negative values.
///
/// @{

/// @def POV_INT8
/// The smallest integer data type that can handle values in the range from -2^7 to 2^7-1.
///
/// @attention
///     Some legacy portions of the code may rely on this type to be _exactly_ 8 bits wide,
///     without padding, and use two's complement format for negative values.
///
#ifndef POV_INT8
    #if defined INT8_MAX
        // data type exactly matching the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_INT8 int8_t
    #elif defined INT_LEAST8_MAX
        // data type matching or exceeding the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_INT8 int_least8_t
    #else
        // char is the smallest type guaranteed by the C++ standard to be at least 8 bits wide
        #define POV_INT8 signed char
    #endif
#endif

/// @def POV_UINT8
/// The smallest integer data type that can handle values in the range from 0 to 2^8-1.
///
/// @attention
///     Some legacy portions of the code may rely on this type to be _exactly_ 8 bits wide,
///     and have no padding bits.
///
#ifndef POV_UINT8
    #if defined UINT8_MAX
        // data type exactly matching the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_UINT8 uint8_t
    #elif defined UINT_LEAST8_MAX
        // data type matching or exceeding the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_UINT8 uint_least8_t
    #else
        // char is the smallest type guaranteed by the C++ standard to be at least 8 bits wide
        #define POV_UINT8 unsigned char
    #endif
#endif

/// @def POV_INT16
/// The smallest integer data type that can handle values in the range from -2^15 to 2^15-1.
///
/// @attention
///     Some legacy portions of the code may rely on this type to be _exactly_ 16 bits wide,
///     have no padding bits, and use two's complement format for negative values.
///
#ifndef POV_INT16
    #if defined INT16_MAX
        // data type exactly matching the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_INT16 int16_t
    #elif defined INT_LEAST16_MAX
        // data type matching or exceeding the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_INT16 int_least16_t
    #else
        // short is the smallest type guaranteed by the C++ standard to be at least 16 bits wide
        #define POV_INT16 signed short
    #endif
#endif

/// @def POV_UINT16
/// The smallest integer data type that can handle values in the range from 0 to 2^16-1.
///
/// @attention
///     Some legacy portions of the code may rely on this type to be _exactly_ 16 bits wide,
///     and have no padding bits.
///
#ifndef POV_UINT16
    #if defined UINT16_MAX
        // data type exactly matching the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_UINT16 uint16_t
    #elif defined UINT_LEAST16_MAX
        // data type matching or exceeding the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_UINT16 uint_least16_t
    #else
        // short is the smallest type guaranteed by the C++ standard to be at least 16 bits wide
        #define POV_UINT16 unsigned short
    #endif
#endif

/// @def POV_INT32
/// The smallest integer data type that can handle values in the range from -2^31 to 2^31-1.
///
/// @attention
///     Some legacy portions of the code may rely on this type to be _exactly_ 32 bits wide,
///     have no padding bits, and use two's complement format for negative values.
///
#ifndef POV_INT32
    #if defined INT32_MAX
        // data type exactly matching the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_INT32 int32_t
    #elif defined INT_LEAST32_MAX
        // data type matching or exceeding the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_INT32 int_least32_t
    #else
        // long is the smallest type guaranteed by the C++ standard to be at least 32 bits wide
        #define POV_INT32 signed long
    #endif
#endif

/// @def POV_UINT32
/// The smallest integer data type that can handle values in the range from 0 to 2^32-1.
///
/// @attention
///     Some legacy portions of the code may rely on this type to be _exactly_ 32 bits wide,
///     and have no padding bits.
///
#ifndef POV_UINT32
    #if defined UINT32_MAX
        // data type exactly matching the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_UINT32 uint32_t
    #elif defined UINT_LEAST32_MAX
        // data type matching or exceeding the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_UINT32 uint_least32_t
    #else
        // long is the smallest type guaranteed by the C++ standard to be at least 32 bits wide
        #define POV_UINT32 unsigned long
    #endif
#endif

/// @def POV_INT64
/// The smallest integer data type that can handle values in the range from -2^63 to 2^63-1.
///
/// @attention
///     Some legacy portions of the code may rely on this type to be _exactly_ 64 bits wide,
///     have no padding bits, and use two's complement format for negative values.
///
#ifndef POV_INT64
    #if defined INT64_MAX
        // data type exactly matching the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_INT64 int64_t
    #elif defined INT_LEAST64_MAX
        // data type matching or exceeding the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_INT64 int_least64_t
    #else
        // long long is the smallest type guaranteed by the C++ standard to be at least 64 bits wide
        #define POV_INT64 signed long long
    #endif
#endif

/// @def POV_UINT64
/// The smallest integer data type that can handle values in the range from 0 to 2^64-1.
///
/// @attention
///     Some legacy portions of the code may rely on this type to be _exactly_ 64 bits wide,
///     and have no padding bits.
///
#ifndef POV_UINT64
    #if defined UINT64_MAX
        // data type exactly matching the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_UINT64 uint64_t
    #elif defined UINT_LEAST64_MAX
        // data type matching or exceeding the requirements, as defined in the C <stdint.h> or C++11 <cstdint> header.
        #define POV_UINT64 uint_least64_t
    #else
        // long long is the smallest type guaranteed by the C++ standard to be at least 64 bits wide
        #define POV_UINT64 unsigned long long
    #endif
#endif

/// @def DBL
/// A floating-point data type providing high precision.
///
/// It is recommended to use a data type providing at least the same range and precision as the
/// IEEE 754 "double" type (15 decimal digits in the range from 1e-308 to 1e+308) and supporting
/// both infinities and quiet NaN values.
///
/// @attention
///     Many portions of the code currently presume this type to be an alias for `double`.
///
#ifndef DBL
    #define DBL double
#endif

/// @def SNGL
/// A floating-point data type providing low memory consumption.
///
/// It is recommended to use a data type providing about the same range and precision as the
/// IEEE 754 "single" type (7 decimal digits in the range from 1e-38 to 1e+38) and supporting
/// both infinities and quiet NaN values.
///
/// @attention
///     Many portions of the code currently presume this type to be an alias for `float`.
///
#ifndef SNGL
    #define SNGL float
#endif

/// @}
///
//******************************************************************************
///
/// @name Usage-Specific Data Types
///
/// The following macros define data types for particular uses. It is recommended that
/// system-specific configurations leave these at their defaults, unless building a special binary
/// to meet unusual requirements.
///
/// @{

/// @def COLC
/// Floating-point data type used to represent brightness.
///
/// This data type is used to represent monochromatic light intensity, or the light intensity of a
/// single colour channel, in computation and storage.
///
/// It is recommended to use a fast data type providing about the same range and precision as the
/// IEEE 754 "single" type (7 decimal digits in the range from 1e-38 to 1e+38) and supporting
/// both infinities and quiet NaN values.
///
/// @note
///     The "half" type might also suffice for this purpose, but you may want to make sure that the
///     implementation on your target system provides adequate performance.
///
/// @todo
///     Replace this type with one type for computation and another type for storage, allowing to
///     choose a precise and/or fast type for the former and a compact one for the latter.
///
#ifndef COLC
    #define COLC float
#endif

/// @def UCS2
/// Data type used to represent individual UCS2 characters.
///
/// This data type is used to represent characters from the UCS2 character set, i.e. the 16-bit
/// Base Multilingual Plane subset of Unicode.
///
/// This should be an unsigned integer type at least 16 bits wide.
///
/// @note
///     For clarity, this data type should _not_ be used as the base type for UTF-16 encoded
///     full-fledged Unicode strings. Use @ref UTF16 instead.
///
/// @attention
///     Some legacy portions of the code may improperly use this type where they should use
///     @ref UCS2 instead.
///
#ifndef UCS2
    #define UCS2 POV_UINT16
#endif

/// @def UCS4
/// Integer data type used to represent UCS4 or full-fledged Unicode characters.
///
/// This should be an unsigned integer type at least 21 (sic!) bits wide.
///
#ifndef UCS4
    #define UCS4 POV_UINT32
#endif

/// @def UTF16
/// Integer data type used as the base type to represent UTF-16 encoded Unicode strings.
/// This should be an unsigned integer type at least 16 bits wide.
///
/// @note
///     For clarity, this data type should _not_ be used to store regular UCS2 characters
///     (16-bit Base Multilingual Plane subset of Unicode). For that purpose, use @ref UCS2
///     instead.
///
/// @attention
///     Some legacy portions of the code may improperly use @ref UCS2 where they should use this
///     type instead.
///
#ifndef UTF16
    #define UTF16 POV_UINT16
#endif

/// @def POV_LONG
///
/// @deprecated
///     This data type is used in many places for different purposes; use either POV_INT64 there
///     or introduce some more specific type names.
///
#ifndef POV_LONG
    #define POV_LONG signed long long
#endif

/// @def POV_ULONG
///
/// @deprecated
///     This data type is used in many places for different purposes; use either POV_INT64 there
///     or introduce some more specific type names.
///
#ifndef POV_ULONG
    #define POV_ULONG unsigned long long
#endif

/// @}
///
//******************************************************************************
///
/// @name Mathematical Constants
///
/// The following macros define mathematical constants to the precision provided by the @ref DBL
/// data type. System-specific configurations providing a data type of higher precision than the
/// IEEE 754 "double" type should override these with more precise values.
///
/// @{

/// @def M_PI
/// The mathematical constant @f$ \pi @f$.
///
#ifndef M_PI
    #define M_PI   3.1415926535897932384626
#endif

/// @def M_PI_2
/// The mathematical constant @f$ ^{\pi}/_{2} @f$.
///
#ifndef M_PI_2
    #define M_PI_2 1.57079632679489661923
#endif

/// @def TWO_M_PI
/// The mathematical constant @f$ 2\pi @f$.
///
#ifndef TWO_M_PI
    #define TWO_M_PI 6.283185307179586476925286766560
#endif

/// @def M_PI_180
/// The mathematical constant @f$ ^{\pi}/_{180} @f$.
///
/// This constant is used for converting from degrees to radians (or vice versa).
///
#ifndef M_PI_180
    #define M_PI_180 0.01745329251994329576
#endif

/// @def M_PI_360
/// The mathematical constant @f$ ^{\pi}/_{360} @f$.
///
/// This constant is used for converting from degrees to radians (or vice versa) while at the
/// same time multiplying (or dividing, respectively) by 2.
///
#ifndef M_PI_360
    #define M_PI_360 0.00872664625997164788
#endif

/// @}
///
//******************************************************************************
///
/// @name Memory Allocation
///
/// The following macros are used for memory allocation and related stuff. Check existing code
/// before you change them, since they aren't simply replacements for malloc, realloc, and free.
///
/// @deprecated
///     New code should instead use the C++ `new` and `delete` operators to allocate and release memory.
///
/// @{

#ifndef POV_MALLOC
    #define POV_MALLOC(size,msg)        pov_malloc ((size), __FILE__, __LINE__, (msg))
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

/// @}
///
//******************************************************************************
///
/// @name Miscellaneous
///
/// @{

#ifndef POV_SYS_FILE_EXTENSION
    #define POV_SYS_FILE_EXTENSION ".tga"
#endif

#ifndef POV_FILE_SEPARATOR
    #define POV_FILE_SEPARATOR '/'
#endif

#ifndef POV_UCS2_FOPEN
    #define POV_UCS2_FOPEN(name, mode) fopen(UCS2toASCIIString(UCS2String(name)).c_str(), mode)
#endif

#ifndef POV_UCS2_REMOVE
    #define POV_UCS2_REMOVE(name) unlink(UCS2toASCIIString(UCS2String(name)).c_str())
#endif

#ifndef EXIST_FONT_FILE
    #define EXIST_FONT_FILE(name) (0)
#endif

#ifndef DEFAULT_ITEXTSTREAM_BUFFER_SIZE
    #define DEFAULT_ITEXTSTREAM_BUFFER_SIZE 512
#endif

#ifndef POV_ALLOW_FILE_READ
    #define POV_ALLOW_FILE_READ(f,t) (1)
#endif

#ifndef POV_ALLOW_FILE_WRITE
    #define POV_ALLOW_FILE_WRITE(f,t) (1)
#endif

#ifndef POV_TRACE_THREAD_PREINIT
    #define POV_TRACE_THREAD_PREINIT
#endif

#ifndef POV_TRACE_THREAD_POSTINIT
    #define POV_TRACE_THREAD_POSTINIT
#endif

// these should not be changed by platform-specific config
#define DEFAULT_WORKING_GAMMA_TYPE  kPOVList_GammaType_Neutral
#define DEFAULT_WORKING_GAMMA       1.0
#define DEFAULT_WORKING_GAMMA_TEXT  "1.0"

#ifndef CDECL
    #define CDECL
#endif

#ifndef ALIGN16
    #define ALIGN16
#endif

#ifndef FORCEINLINE
    #define FORCEINLINE inline
#endif

/// @def POV_MULTITHREADED
/// Enable multithreading in the core modules.
///
/// Define as non-zero integer to enable, or zero to disable.
/// Defaults to enabled.
///
#ifndef POV_MULTITHREADED
    #define POV_MULTITHREADED 1
#endif

/// @def POV_USE_DEFAULT_DELAY
/// Whether to use a default implementation for the millisecond-precision delay function.
///
/// Define as non-zero to use a default implementation for the @ref Delay() function, or zero if
/// the platform provides its own implementation.
///
/// @note
///     The default implementation is only provided as a last-ditch resort. Wherever possible,
///     implementations should provide their own implementation.
///
#ifndef POV_USE_DEFAULT_DELAY
    #define POV_USE_DEFAULT_DELAY 1
#endif

/// @def POV_USE_DEFAULT_TIMER
/// Whether to use a default implementation for the millisecond-precision timer.
///
/// Define as non-zero to use a default implementation for the @ref Timer class, or zero if the
/// platform provides its own implementation.
///
/// @note
///     The default implementation is only provided as a last-ditch resort. Wherever possible,
///     implementations should provide their own implementation.
///
#ifndef POV_USE_DEFAULT_TIMER
    #define POV_USE_DEFAULT_TIMER 1
#endif

/// @}
///
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

/// @def POV_DEBUG
/// Default setting for enabling or disabling debugging aids.
///
/// This setting specifies the default for all debugging switches throughout the entire POV-Ray
/// project that are not explicitly enabled or disabled by system-specific configurations.
///
/// If left undefined by system-specific configurations, this setting defaults to zero, disabling
/// any debugging aids not explicitly enabled by system-specific configurations.
///
#ifndef POV_DEBUG
    #define POV_DEBUG 0
#endif

/// @def POV_COLOURSPACE_DEBUG
/// Enable run-time sanity checks for colour space handling.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_COLOURSPACE_DEBUG
    #define POV_COLOURSPACE_DEBUG POV_DEBUG
#endif

/// @def POV_IMAGE_DEBUG
/// Enable run-time sanity checks for image handling.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_IMAGE_DEBUG
    #define POV_IMAGE_DEBUG POV_DEBUG
#endif

/// @def POV_MATHUTIL_DEBUG
/// Enable run-time sanity checks for scalar maths utility functions.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_MATHUTIL_DEBUG
    #define POV_MATHUTIL_DEBUG POV_DEBUG
#endif

/// @def POV_SAFEMATH_DEBUG
/// Enable run-time sanity checks for safe integer maths.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_SAFEMATH_DEBUG
    #define POV_SAFEMATH_DEBUG POV_DEBUG
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

/// @def POV_TIME_UTC
/// Alias for `boost::TIME_UTC` or `boost::TIME_UTC_`, whichever is applicable.
///
/// Boost 1.50 changed TIME_UTC to TIME_UTC_ to avoid a clash with C++11, which has
/// TIME_UTC as a define (in boost it's an enum). To allow compilation with earlier
/// versions of boost we now use POV_TIME_UTC in the code and define that here.
///
#if BOOST_VERSION >= 105000
    #define POV_TIME_UTC boost::TIME_UTC_
#else
    #ifdef TIME_UTC
        // clash between C++11 and boost detected, need to hard-code
        #define POV_TIME_UTC 1
    #else
        #define POV_TIME_UTC boost::TIME_UTC
    #endif
#endif

#if POV_DEBUG
    #define POV_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_ASSERT(expr) NO_OP
#endif

#if POV_COLOURSPACE_DEBUG
    #define POV_COLOURSPACE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_COLOURSPACE_ASSERT(expr) NO_OP
#endif

#if POV_IMAGE_DEBUG
    #define POV_IMAGE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_IMAGE_ASSERT(expr) NO_OP
#endif

#if POV_MATHUTIL_DEBUG
    #define POV_MATHUTIL_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_MATHUTIL_ASSERT(expr) NO_OP
#endif

#if POV_SAFEMATH_DEBUG
    #define POV_SAFEMATH_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_SAFEMATH_ASSERT(expr) NO_OP
#endif

#define M_TAU TWO_M_PI

/// @}
///
//******************************************************************************

#endif // POVRAY_BASE_CONFIGBASE_H
