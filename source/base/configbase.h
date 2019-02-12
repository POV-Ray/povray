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

#ifndef POVRAY_BASE_CONFIGBASE_H
#define POVRAY_BASE_CONFIGBASE_H

// Pull in other compile-time config header files first
#include "syspovconfigbase.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

//##############################################################################
///
/// @defgroup PovBaseConfig Base Compile-Time Configuration
/// @ingroup PovBase
/// @ingroup PovConfig
/// Compile-Time Configuration of the @ref PovBase.
///
/// In addition to the configuration settings listed below, platform-specific compile-time
/// configuration needs to define various symbols to have the same semantics as those of the same
/// name from the namespaces of common standard libraries. Typically this will be done by including
/// the corresponding header files and specifying `using NAMESPACE::SYMBOL`. However, alternative
/// implementations may also be provided unless noted otherwise.
///
/// @todo
///     The following macros currently default to unexpected values; also, the implementations
///     they default to are currently not part of the base module:
///       - `POV_MALLOC`
///       - `POV_REALLOC`
///       - `POV_FREE`
///
/// @{

//******************************************************************************
///
/// @name C++ Language Standard
///
/// @{

/// @def POV_CPP11_SUPPORTED
/// Whether the compiler supports C++11.
///
/// Define as non-zero if the compiler is known to support all the C++11 language features
/// required to build POV-Ray, or zero if you are sure it doesn't. If in doubt, leave undefined.
///
#ifndef POV_CPP11_SUPPORTED
    #define POV_CPP11_SUPPORTED (__cplusplus >= 201103L)
#endif


/// @}
///
//******************************************************************************
///
/// @name Fundamental Data Types
///
/// The following macros define essential data types. It is recommended that system-specific
/// configurations always override the defaults; although POV-Ray will make a solid guess as to
/// which type fits the requirements, the algorithm may fail on some exotic systems.
///
/// @compat
///     The automatic type detection is almost certain to fail on systems that employ padding bits
///     and/or do not use two's complement format for negative values.
///
/// @{

/// @def POV_INT8
/// The smallest integer data type that can handle values in the range from @f$ -2^7 @f$ to @f$ 2^7-1 @f$.
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
/// The smallest integer data type that can handle values in the range from @f$ 0 @f$ to @f$ 2^8-1 @f$.
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
/// The smallest integer data type that can handle values in the range from @f$ -2^{15} @f$ to @f$ 2^{15}-1 @f$.
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
/// The smallest integer data type that can handle values in the range from @f$ 0 @f$ to @f$ 2^{16}-1 @f$.
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
/// The smallest integer data type that can handle values in the range from @f$ -2^{31} @f$ to @f$ 2^{31}-1 @f$.
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
/// The smallest integer data type that can handle values in the range from @f$ 0 @f$ to @f$ 2^{32}-1 @f$.
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
/// The smallest integer data type that can handle values in the range from @f$ -2^{63} @f$ to @f$ 2^{63}-1 @f$.
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
/// The smallest integer data type that can handle values in the range from @f$ 0 @f$ to @f$ 2^{64}-1 @f$.
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
/// @note
///     When overriding this macro, make sure to also override @ref POV_DBL_FORMAT_STRING.
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

// The following character types are no longer defined as macros,
// but instead use aliased or dedicated types.
#ifdef UCS2
    #error "Platform-specific configurations should no longer define UCS2 as a macro."
#endif
#ifdef UCS4
    #error "Platform-specific configurations should no longer define UCS4 as a macro."
#endif
#ifdef UTF16
    #error "Platform-specific configurations should no longer define UTF16 as a macro."
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
/// @name Floating-Point Special Values
///
/// The following macros are used to identify certain special floating-point values.
///
/// @{

/// @def POV_ISINF(x)
/// Test whether floating-point value x denotes positive or negative infinity.
///
/// The default implementation tests whether the absolute of the value is larger than the largest
/// finite value representable by the data type.
///
/// @note
///     The macro must be implemented in a manner that it works properly for all floating-point
///     data types.
///
#ifndef POV_ISINF
    template<typename T>
    inline bool pov_isinf(T x) { volatile T v = std::numeric_limits<T>::max(); return std::fabs(x) > v; }
    #define POV_ISINF(x) pov_isinf(x)
#endif

/// @def POV_ISNAN(x)
/// Test whether floating-point value x denotes "not-a-number" (NaN).
///
/// "Not-a-number" is any special value denoting neither a finite number nor positive or negative
/// infinity. Examples include the result of dividing 0 by 0, or raising a negative value to a
/// non-integer power.
///
/// The default implementation exploits the property that (on most systems) NaNs (and only NaNs)
/// are considered non-equal to any value including themselves.
///
/// @note
///     The macro must be implemented in a manner that it works properly for all floating-point
///     data types.
///
#ifndef POV_ISNAN
    template<typename T>
    inline bool pov_isnan(T x) { volatile T v = x; return (v != x); }
    #define POV_ISNAN(x) pov_isnan(x)
#endif

/// @def POV_ISFINITE(x)
/// Test whether floating-point value x denotes a proper finite number.
///
/// The default implementation tests whether the value is neither infinity nor "not-a-number",
/// using the @ref POV_ISINF and @ref POV_ISNAN macros.
///
/// @note
///     The macro must be implemented in a manner that it works properly for all floating-point
///     data types.
///
#ifndef POV_ISFINITE
    #define POV_ISFINITE(x) (!POV_ISINF(x) && !POV_ISNAN(x))
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
    #define POV_MALLOC(size,msg)        pov_base::pov_malloc ((size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_REALLOC
    #define POV_REALLOC(ptr,size,msg)   pov_base::pov_realloc ((ptr), (size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_FREE
    #define POV_FREE(ptr)               do { pov_base::pov_free (static_cast<void *>(ptr), __FILE__, __LINE__); (ptr) = nullptr; } while(false)
#endif

#ifndef POV_MEM_INIT
    #define POV_MEM_INIT()              pov_base::mem_init()
#endif

#ifndef POV_MEM_RELEASE_ALL
    #define POV_MEM_RELEASE_ALL()       pov_base::mem_release_all()
#endif

#ifndef POV_STRDUP
    #define POV_STRDUP(str)             pov_base::pov_strdup(str)
#endif

#ifndef POV_MEM_STATS
    #define POV_MEM_STATS                       0
    #define POV_GLOBAL_MEM_STATS(a,f,c,p,s,l)   (false)
    #define POV_MEM_STATS_RENDER_BEGIN()
    #define POV_MEM_STATS_RENDER_END()
#endif

/// @}
///
//******************************************************************************
///
/// @name Miscellaneous
///
/// @{

/// @def POV_BUILD_INFO
/// Additional build information.
///
/// An ASCII string containing only alphanumeric and/or hyphen characters (`A`-`Z`, `a`-`z`,
/// `0`-`9`, `-`), intended to differentiate builds created from the same source code but using
/// different build tools or settings.
///
#ifndef POV_BUILD_INFO
    // leave undefined
    // The following two lines work around doxygen being unable to document undefined macros.
    #define POV_BUILD_INFO (undefined)
    #undef POV_BUILD_INFO
#endif

/// @def POV_RAY_BUILD_ID
/// Unique build identifier.
///
/// An ASCII string containing only alphanumeric and/or hyphen characters (`A`-`Z`, `a`-`z`,
/// `0`-`9`, `-`), intended to further differentiate different builds created from the same source
/// code in cases where @ref POV_BUILD_INFO would be the same.
///
/// @note
///     This macro is _not_ intended to be set via hard-coded compile-time configuration files,
///     but rather injected during the actual build via compiler parameters.
///
#ifndef POV_RAY_BUILD_ID
    // leave undefined
    // The following two lines work around doxygen being unable to document undefined macros.
    #define POV_RAY_BUILD_ID (undefined)
    #undef POV_RAY_BUILD_ID
#endif

/// @def POV_COMPILER_INFO
/// Verbose compiler name.
///
/// @note
///     If the compiler information is already encoded in @ref POV_BUILD_INFO, this macro should be
///     left undefined.
///
#ifndef POV_COMPILER_INFO
    // leave undefined
    // The following two lines work around doxygen being unable to document undefined macros.
    #define POV_COMPILER_INFO (undefined)
    #undef POV_COMPILER_INFO
#endif

/// @def POV_SYS_IMAGE_TYPE
/// The system's canonical image file format.
///
/// Set this to the file type, as defined in @ref pov_base::Image::ImageFileType, corresponding to the
/// input image file format selected by the `sys` keyword.
///
/// @note
///     When overriding this setting, make sure to also override @ref POV_SYS_IMAGE_EXTENSION.
///
#ifndef POV_SYS_IMAGE_TYPE
    // leave undefined by default
    // The following two lines work around doxygen being unable to document undefined macros.
    #define POV_SYS_IMAGE_TYPE (undefined)
    #undef POV_SYS_IMAGE_TYPE
#endif

/// @def POV_SYS_IMAGE_EXTENSION
/// The system's canonical image file format extension.
///
/// Set this to the file extension string (including leading dot) corresponding to the
/// input image file format selected by the `sys` keyword.
///
/// @note
///     When overriding this setting, make sure to also override @ref POV_SYS_IMAGE_TYPE.
///
#ifndef POV_SYS_IMAGE_EXTENSION
    // leave undefined by default
    // The following two lines work around doxygen being unable to document undefined macros.
    #define POV_SYS_IMAGE_EXTENSION (undefined)
    #undef POV_SYS_IMAGE_EXTENSION
#endif

/// @def POV_FILENAME_BUFFER_CHARS
/// The number of characters to reserve for file name buffers.
///
/// This setting is used in allocating temporary buffers to construct file names, and should be set
/// to the maximum number of ASCII characters in a file name the system can handle safely.
/// The value is understood to exclude any terminating NUL character.
///
#ifndef POV_FILENAME_BUFFER_CHARS
    #define POV_FILENAME_BUFFER_CHARS 199
#endif

/// @def POV_PATH_SEPARATOR
/// The system's canonical path separator character.
///
#ifndef POV_PATH_SEPARATOR
    #define POV_PATH_SEPARATOR '/'
#endif

/// @def POV_IS_PATH_SEPARATOR(c)
/// Test whether `c` is any of the system's supported path separator characters.
///
/// If the operating system does not support alternative path separator characters, leave this undefined, in which
/// case it will evaluate to a test for @ref POV_PATH_SEPARATOR.
///
/// @note
///     This macro must also test for the canonical path separator.
/// @note
///     When the macro parameter is a character constant, this macro must expand to a _constant expression_, i.e. it
///     must be usable in preprocessor directives.
///
#ifndef POV_IS_PATH_SEPARATOR
    #define POV_IS_PATH_SEPARATOR(c) ((c) == POV_PATH_SEPARATOR)
#endif

/// @def POV_SLASH_IS_SWITCH_CHARACTER
/// Whether the forward slash character should be recognized as the start of a command-line switch.
///
#ifndef POV_SLASH_IS_SWITCH_CHARACTER
    #define POV_SLASH_IS_SWITCH_CHARACTER 0
#endif

/// @def POV_DBL_FORMAT_STRING
/// `scanf` format string to read a value of type @ref DBL.
///
#ifndef POV_DBL_FORMAT_STRING
    #define POV_DBL_FORMAT_STRING "%lf"
#endif

/// @def POV_NEW_LINE_STRING
/// The system's canonical end-of-line string.
///
#ifndef POV_NEW_LINE_STRING
    // leave undefined, optimizing the code for "\n" as used internally
    // The following two lines work around doxygen being unable to document undefined macros.
    #define POV_NEW_LINE_STRING (undefined)
    #undef POV_NEW_LINE_STRING
#endif

#ifndef EXIST_FONT_FILE
    #define EXIST_FONT_FILE(name) (0)
#endif

#ifndef DEFAULT_ITEXTSTREAM_BUFFER_SIZE
    #define DEFAULT_ITEXTSTREAM_BUFFER_SIZE 512
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

/// @def DEFAULT_AUTO_BOUNDINGTHRESHOLD
/// Define the default auto-bounding threshold used and reported.
///
/// The shipped povray.ini file defines this with:
/// Bounding_Threshold=3
/// and the definition here should be aligned so when users run
/// without the bounding threshold setting by ini file or command
/// line option the default is still the documented value of 3.
///
#ifndef DEFAULT_AUTO_BOUNDINGTHRESHOLD
    #define DEFAULT_AUTO_BOUNDINGTHRESHOLD 3
#endif

#ifndef CDECL
    #define CDECL
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

/// @def POV_USE_PLATFORM_DELAY
/// Whether to use a platform-specific implementation for the millisecond-precision delay function.
///
/// Define as zero to use a portable default implementation for the @ref pov_base::Delay() function, or non-zero if
/// the platform provides its own implementation.
///
/// The actual value may carry additional semantics based on the platform
/// family. The following are currently known (but see the respective platform
/// specific code for authoritative information):
///
/// | Value | Windows       | Unix              |
/// | ----: | :------------ | :---------------- |
/// |     0 | `std::this_thread::sleep_for()`   |
/// |     1 | `Sleep()`     | `nanosleep()`     |
/// |     2 | ^             | `usleep()`        |
/// | other | ^             | undefined         |
///
#ifndef POV_USE_PLATFORM_DELAY
    #define POV_USE_PLATFORM_DELAY 0
#endif

#ifdef POV_USE_DEFAULT_DELAY
#error "POV_USE_DEFAULT_DELAY has been superseded by POV_USE_PLATFORM_DELAY. Note that the new setting has inverse semantics."
#endif

/// @def POV_USE_DEFAULT_TIMER
/// Whether to use a default implementation for the millisecond-precision timer.
///
/// Define as non-zero to use a default implementation for the @ref pov_base::Timer class, or zero if the
/// platform provides its own implementation.
///
/// @note
///     The default implementation is only provided as a last-ditch resort. Wherever possible,
///     implementations should provide their own implementation.
///
#ifndef POV_USE_DEFAULT_TIMER
    #define POV_USE_DEFAULT_TIMER 1
#endif

/// @def POV_USE_DEFAULT_PATH_PARSER
/// Whether to use a default implementation for the path string parser.
///
/// Define as non-zero to use a default implementation for the @ref pov_base::Path::ParsePathString() method,
/// or zero if the platform provides its own implementation.
///
/// @note
///     The default implementation supports only local files on a single anonymous volume.
///
#ifndef POV_USE_DEFAULT_PATH_PARSER
    #define POV_USE_DEFAULT_PATH_PARSER 1
#endif

/// @def POV_USE_DEFAULT_DELETEFILE
/// Whether to use a default implementation to delete a file.
///
/// Define as non-zero to use a default implementation for the @ref pov_base::Filesystem::DeleteFile() method,
/// or zero if the platform provides its own implementation.
///
/// @note
///     The default implementation is only provided as a last-ditch resort. Wherever possible,
///     implementations should provide their own implementation.
///
#ifndef POV_USE_DEFAULT_DELETEFILE
    #define POV_USE_DEFAULT_DELETEFILE 1
#endif

/// @def POV_USE_DEFAULT_LARGEFILE
/// Whether to use a default implementation for large file handling.
///
/// Define as non-zero to use a default implementation for the @ref pov_base::Filesystem::LargeFile class,
/// or zero if the platform provides its own implementation.
///
/// @note
///     The current default implementation may or may not actually support large
///     files, and may provide less than optimal performance.
///
#ifndef POV_USE_DEFAULT_LARGEFILE
    #define POV_USE_DEFAULT_LARGEFILE 1
#endif

/// @def POV_OFF_T
/// Type representing a particular absolute or relative location in a (large) file.
///
/// Define this to a signed integer wide enough to hold the maximum file size
/// supported by the operating system. On most modern systems, this should be
/// at least 64 bits (including the sign bit).
///
#ifndef POV_OFF_T
    #define POV_OFF_T std::int_least64_t
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

/// @def POV_CPUINFO_DEBUG
/// Enable CPU detection run-time sanity checks and debugging aids.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_CPUINFO_DEBUG
    #define POV_CPUINFO_DEBUG POV_DEBUG
#endif

/// @def POV_FILE_DEBUG
/// Enable run-time sanity checks for file handling.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_FILE_DEBUG
    #define POV_FILE_DEBUG POV_DEBUG
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

/// @def POV_RTR_DEBUG
/// Enable run-time sanity checks for real-time rendering.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_RTR_DEBUG
    #define POV_RTR_DEBUG POV_DEBUG
#endif

/// @def POV_SAFEMATH_DEBUG
/// Enable run-time sanity checks for safe integer maths.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_SAFEMATH_DEBUG
    #define POV_SAFEMATH_DEBUG POV_DEBUG
#endif

/// @def POV_STRING_DEBUG
/// Enable run-time sanity checks for string handling.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_STRING_DEBUG
    #define POV_STRING_DEBUG POV_DEBUG
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

/// @def POV_BACKSLASH_IS_PATH_SEPARATOR
/// Whether the system supports the backslash as a separator character.
///
#if POV_IS_PATH_SEPARATOR('\\')
    #define POV_BACKSLASH_IS_PATH_SEPARATOR 1
#else
    #define POV_BACKSLASH_IS_PATH_SEPARATOR 0
#endif

/// @def POV_ASSERT
/// Assert a condition that should hold true by design.
/// In debug builds, this macro evaluates the specified expression, and halts execution if the
/// expression does not hold true.
/// In release builds, this macro evaluates to an empty statement.
#if POV_DEBUG
    #define POV_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

/// @def POV_EXPERIMENTAL_ASSERT
/// Assert a condition that is expected to hold true by current understanding of the code.
/// This macro evaluates the specified expression, and halts execution (in debug builds) or throws
/// an exception (in release builds) if the expression does not hold true.
/// @attention
///     This macro is intended for special builds only, and should _never_ be used in mainline versions!
#if POV_DEBUG
    #define POV_EXPERIMENTAL_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_EXPERIMENTAL_ASSERT(expr) POV_ASSERT_SOFT(expr)
#endif

#if POV_COLOURSPACE_DEBUG
    #define POV_COLOURSPACE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_COLOURSPACE_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_CPUINFO_DEBUG
    #define POV_CPUINFO_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_CPUINFO_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_FILE_DEBUG
    #define POV_FILE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_FILE_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_IMAGE_DEBUG
    #define POV_IMAGE_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_IMAGE_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_MATHUTIL_DEBUG
    #define POV_MATHUTIL_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_MATHUTIL_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_RTR_DEBUG
    #define POV_RTR_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_RTR_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_SAFEMATH_DEBUG
    #define POV_SAFEMATH_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_SAFEMATH_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_STRING_DEBUG
    #define POV_STRING_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_STRING_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#define M_TAU TWO_M_PI

#define POVMSAddress        void*
#define POVMSInvalidAddress nullptr

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

//******************************************************************************
// Sanity Checks

#if !POV_CPP11_SUPPORTED
    #error "This version of POV-Ray requires C++11, which your compiler does not seem to support."
#endif

#endif // POVRAY_BASE_CONFIGBASE_H
