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

#ifndef POVRAY_BASE_TYPES_H
#define POVRAY_BASE_TYPES_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"
#include "base/base_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <limits>

// POV-Ray header files (base module)
#include "base/pov_err.h"
#include "base/pov_mem.h"

namespace pov_base
{

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

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
///
/// @name Image Stuff
/// @{

// Image types.

#define IMAGE_FILE    GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define NORMAL_FILE   GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define MATERIAL_FILE GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define HF_FILE       GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+POT_FILE

/// @}
///
//******************************************************************************

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

struct POVRect final
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

/// Legacy (v3.5) `charset` setting.
enum LegacyCharset : int
{
    kUnspecified,   ///< Global settings `charset` not specified.
    kASCII,         ///< Global settings `charset ascii` specified.
    kUTF8,          ///< Global settings `charset utf8` specified.
    kSystem,        ///< Global settings `charset sys` specified.
};

/// Value identifying a character set.
///
/// Each value of this type represents a particular set of characters and associated mapping of
/// those characters to _code points_.
///
/// @note
///     The values of this type do _not_ identify any particular character _encoding_, i.e. a
///     scheme of representing streams of characters as a byte stream. For example, all of
///     UTF-8, UTF-16LE, UTF-16BE, UTF-32LE and UTF-32BE are encoding schemes for the UCS-4
///     character set.
///
/// @note
///     The numeric values chosen for the individual character sets are generally based on Windows
///     code page numbers. If you add more character sets, please stick to this scheme wherever
///     applicable, or use negative values.
///
enum class CharsetID : int
{
    kUndefined      = 0,        ///< Special value representing undefined character set.

    kUCS2           = 1200,     ///< UCS-2 (16-bit subset of UCS) aka Basic Multilingual Plane (BMP).
    kWindows1251    = 1251,     ///< Windows code page 1251 (Cyrillic).
    kWindows1252    = 1252,     ///< Windows code page 1252 (Western) aka [incorrectly] ANSI.
    kMacOSRoman     = 10000,    ///< Mac OS Roman (as used on classic Mac OS).
    kUCS4           = 12000,    ///< UCS-4 (full set of UCS) aka [not entirely correctly] Unicode.
    kLatin1         = 28591,    ///< ISO-8859-1 aka Latin-1.

    kLegacySymbols  = -1,       ///< Special value representing legacy remapping for Microsoft symbol fonts,
                                ///< remapping U+0000-U+00FF to U+F000-U+F0FF.
};

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
// end of namespace pov_base

#endif // POVRAY_BASE_TYPES_H
