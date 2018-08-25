//******************************************************************************
///
/// @file base/types.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_BASE_TYPES_H
#define POVRAY_BASE_TYPES_H

#include "base/configbase.h"

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include "base/pov_mem.h"

namespace pov_base
{

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

/// A macro wrapping a sequence of statements into a single one.
///
/// This macro is intended to be used in the definition of other macros that should behave
/// syntactically like a single statement, while evaluating to something that would not normally
/// behave that way.
///
/// Example:
///
///     #declare FOO(x) SINGLE_STATEMENT( char buf[128]; foo(buf,x); )
///     ...
///     if (some_cond)
///         FOO(a);
///     else
///         ...
///
#define SINGLE_STATEMENT( block ) do { block } while (false)

//******************************************************************************
///
/// @name Theoretical Integer Limits
///
/// The following macros evaluate to the minimum and maximum values that could theoretically be
/// stored in an integer of the specified width, presuming that two's complement format is used for
/// negatives, regardless of what data type and negative number format the compiler and runtime
/// environment actually support.
///
/// @impl
/// @parblock
///     Care must be taken when specifying negative integer constants. According to the C++ standard
///     there is no such thing as a negative integer literal, only non-negative integer literals
///     negated by applying the unary minus operator applied; this is ok as long as the value is
///     given in decimal and the absolute value fits into a `long int` (which is guaranteed for
///     values up to 2^31-1), in which case the standard mandates that the literal is automatically
///     considered signed, and signed types must be able to hold negative values as least as small
///     as the negative of the largest positive value they can represent. However, if the absolute
///     value exceeds the capacity of a `long int` the behaviour is undefined, so the literal may
///     be interpreted as an unsigned value. In that case applying the unary minus operator will
///     still yield a positive result.
///
///     We work around this issue by first explicitly casting such literals to a fitting `POV_INTn`
///     type before applying the unary minus operator. Properly configured, those types also
///     guarantee that the expression (-POV_INTn(x)-1) does not overflow for positive x (as long as
///     x fits in the type), which is not generally guaranteed for signed integer types.
///
///     Note that as a consequence, such constants can not be utilized in preprocessor statements.
/// @endparblock
///
/// @{

/// Maximum unsigned 8-bit integer value.
/// @remark This value's type is unsigned.
#define UNSIGNED8_MAX   (255u)

/// Maximum unsigned 16-bit integer value.
/// @remark This value's type is unsigned.
#define UNSIGNED16_MAX  (65535u)

/// Maximum unsigned 32-bit integer value.
/// @remark This value's type is unsigned.
#define UNSIGNED32_MAX  (4294967295u)

/// Maximum unsigned 64-bit integer value.
/// @remark This value's type is unsigned.
#define UNSIGNED64_MAX  (18446744073709551615u)

/// Maximum signed 8-bit integer value.
/// @remark
///     As the C++ standard guarantees this value to fit into a `long int`, its type is signed despite the value
///     being positive.
#define SIGNED8_MAX     (127)

/// Maximum signed 16-bit integer value.
/// @remark
///     As the C++ standard guarantees this value to fit into a `long int`, its type is signed despite the value
///     being positive.
#define SIGNED16_MAX    (32767)

/// Maximum signed 32-bit integer value.
/// @remark
///     As the C++ standard guarantees this value to fit into a `long int`, its type is signed despite the value
///     being positive.
#define SIGNED32_MAX    (2147483647)

/// Maximum signed 64-bit integer value.
/// @note
///     This constant cannot be used in preprocessor statements.
/// @remark
///     This value's type is signed despite the value being positive.
#define SIGNED64_MAX    ((POV_INT64)(9223372036854775807))

/// Minimum two's complement signed 8-bit integer value.
#define SIGNED8_MIN     (-128)

/// Minimum two's complement signed 16-bit integer value.
#define SIGNED16_MIN    (-32768)

/// Minimum two's complement signed 32-bit integer value.
/// @note
///     This constant cannot be used in preprocessor statements.
#define SIGNED32_MIN    ((-(POV_INT32)(SIGNED32_MAX))-1)

// (there's no SIGNED64_MIN because some systems may be entirely unable to represent that value.

/// Modulus for unsigned 8-bit integer operations.
/// @remark This value's type is unsigned.
#define UNSIGNED8_MOD   (256u)

/// Modulus for unsigned 16-bit integer operations.
/// @remark This value's type is unsigned.
#define UNSIGNED16_MOD  (65536u)

/// Modulus for unsigned 32-bit integer operations.
/// @remark This value's type is unsigned.
#define UNSIGNED32_MOD  (4294967296u)

// (there's no UNSIGNED64_MOD because some systems may be entirely unable to represent that value.

/// (unsigned) 8-bit integer with only the bit set that indicates a negative value in signed
/// interpretation.
/// @remark This value's type is unsigned.
#define INTEGER8_SIGN_MASK  (0x80u)

/// (unsigned) 16-bit integer with only the bit set that indicates a negative value in signed
/// interpretation.
/// @remark This value's type is unsigned.
#define INTEGER16_SIGN_MASK (0x8000u)

/// (unsigned) 32-bit integer with only the bit set that indicates a negative value in signed
/// interpretation.
/// @remark This value's type is unsigned.
#define INTEGER32_SIGN_MASK (0x80000000u)

/// (unsigned) 64-bit integer with only the bit set that indicates a negative value in signed
/// interpretation.
/// @remark This value's type is unsigned.
#define INTEGER64_SIGN_MASK (0x8000000000000000u)

/// @}
///
//******************************************************************************

/// A macro that does nothing.
///
/// This macro is intended to be used in the definition of other macros that should behave
/// syntactically like a single statement, while evaluating to a no-operation.
///
/// Example:
///
///     #declare MY_ASSERT(x) NO_OP
///     ...
///     if (some_cond)
///         MY_ASSERT(some_test);
///     else
///         ...
///
#define NO_OP SINGLE_STATEMENT(;)

/// A macro that tests an expression and, if it evaluates false, throws an exception to allow the
/// application to fail gracefully.
///
#define POV_ASSERT_SOFT(expr) SINGLE_STATEMENT( if(!(expr)) throw POV_EXCEPTION_CODE(kUncategorizedError); )

/// A macro that tests an expression and, if it evaluates false, causes a hard crash to generate a
/// core dump or break to a debugger.
///
#define POV_ASSERT_HARD(expr) assert(expr)

/// A macro that does nothing, but is mapped to standard `assert()` during static code analysis.
///
#ifdef STATIC_CODE_ANALYSIS
    #define POV_ASSERT_DISABLE(expr) assert(expr)
#else
    #define POV_ASSERT_DISABLE(expr) NO_OP
#endif

// from <algorithm>; we don't want to always type the namespace for these.
using std::min;
using std::max;

// from <cmath>; we don't want to always type the namespace for these.
using std::abs;
using std::acos;
using std::asin;
using std::atan;
using std::atan2;
using std::ceil;
using std::cos;
using std::cosh;
using std::exp;
using std::fabs;
using std::floor;
using std::fmod;
using std::frexp;
using std::ldexp;
using std::log;
using std::log10;
using std::modf;
using std::pow;
using std::sin;
using std::sinh;
using std::sqrt;
using std::tan;
using std::tanh;

/// 5-dimensional vector type shared between parser and splines.
/// @todo Make this obsolete.
typedef DBL EXPRESS[5];

struct POVRect
{
    unsigned int top;
    unsigned int left;
    unsigned int bottom;
    unsigned int right;

    POVRect() : top(0), left(0), bottom(0), right(0) { }
    POVRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) :
        top(y1), left(x1), bottom(y2), right(x2) { }

    unsigned int GetArea() const { return ((bottom - top + 1) * (right - left + 1)); }
    unsigned int GetWidth() const { return (right - left + 1); }
    unsigned int GetHeight() const { return (bottom - top + 1); }
};

enum StringEncoding
{
    kStringEncoding_ASCII  = 0,
    kStringEncoding_UTF8   = 1,
    kStringEncoding_System = 2
};

typedef std::string UTF8String;

enum GammaMode
{
    /**
     *  No gamma handling.
     *  This model is based on the (wrong) presumption that image file pixel values are proportional to
     *  physical light intensities.
     *  This is the default for POV-Ray v3.6 and earlier.
     */
    kPOVList_GammaMode_None,
    /**
     *  Explicit assumed_gamma-based gamma handling model, v3.6 variant.
     *  This model is based on the (wrong) presumption that render engine maths works equally well with
     *  both linear and gamma-encoded light intensity values.
     *  Using assumed_gamma=1.0 gives physically realistic results.
     *  Input image files without implicit or explicit gamma information will be presumed to match assumed_gamma,
     *  i.e. they will not be gamma corrected.
     *  This is the mode used by POV-Ray v3.6 and earlier if assumed_gamma is specified.
     */
    kPOVList_GammaMode_AssumedGamma36,
    /**
     *  Explicit assumed_gamma-based gamma handling model, v3.7 variant.
     *  This model is based on the (wrong) presumption that render engine maths works equally well with
     *  both linear and gamma-encoded light intensity values.
     *  Using assumed_gamma=1.0 gives physically realistic results.
     *  Input image files without implicit or explicit gamma information will be presumed to match official
     *  recommendations for the respective file format; files for which no official recommendations exists
     *  will be presumed to match assumed_gamma.
     *  This is the mode used by POV-Ray v3.7 and later if assumed_gamma is specified.
     */
    kPOVList_GammaMode_AssumedGamma37,
    /**
     *  Implicit assumed_gamma-based gamma handling model, v3.7 variant.
     *  This model is functionally identical to kPOVList_GammaMode_AssumedGamma37 except that it also serves as a marker
     *  that assumed_gamma has not been set explicitly.
     *  This is the default for POV-Ray v3.7 and later.
     */
    kPOVList_GammaMode_AssumedGamma37Implied,
};

/// Common base class for all thread-specific data.
///
/// This class is used as the base for all thread-specific data to allow for safe dynamic casting.
///
class ThreadData
{
    public:
        virtual ~ThreadData() { }
};

/// @}
///
//##############################################################################

}

#endif // POVRAY_BASE_TYPES_H
