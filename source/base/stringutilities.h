//******************************************************************************
///
/// @file base/stringutilities.h
///
/// Declarations related to string handling.
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

#ifndef POVRAY_BASE_STRINGUTILITIES_H
#define POVRAY_BASE_STRINGUTILITIES_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of C standard header files
#include <cstring>

// C++ standard header files
#include <string>

// POV-Ray header files (base module)
#include "base/base_fwd.h"
#include "base/povassert.h"
#include "base/stringtypes.h"

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBaseStringutilities String Utilities
/// @ingroup PovBase
///
/// @{

UCS2String ASCIItoUCS2String(const std::string& s);

UCS2String SysToUCS2String(const char *s);
UCS2String SysToUCS2String(const std::string& s);
std::string UCS2toSysString(const UCS2String& s);

UCS2String UTF8toUCS2String(const UTF8String& s);

int pov_stricmp(const char *, const char *);
const char *pov_tsprintf(const char *, ...);

std::size_t UCS2_strlen(const UCS2* str);
int UCS2_strcmp(const UCS2* s1, const UCS2* s2);

//******************************************************************************

/// UCS Decoding Primitives
///
/// The functions in this namespace serve as basic building blocks for code that needs to decode
/// UTF-8 and UTF-16 characters.
///
/// @note
///     To be as universally usable as possible, the actual decoding functions
///     implement the pre-2003 version of UTF-8 (allowing for code points as
///     high as 0x7FFFFFFF as well as UTF-16 surrogates), while providing
///     auxiliary functions to test for validity according to modern UTF-8.
///
namespace UCS
{

using UTF8SequenceLength    = unsigned int;
using UTF8CodeUnit          = unsigned char;

/// Replacement character used to represent characters not in UCS.
constexpr UCS4 kReplacementCharacter = 0xFFFDu;

/// Helper function to read next code unit from container-like source.
/// @tparam         ITER_T  Type representing the source and a position therein,
///                         e.g. an iterator or pointer.
/// @param[in,out]  src     Current position in the source.
/// @return                 Code unit read from source.
template<typename ITER_T> typename std::iterator_traits<ITER_T>::value_type GetNext(ITER_T& src)
{
    return *(src++);
}

/// Helper function to attempt to read next code unit from container-like source.
/// @tparam         UNIT_T  Type representing a source unit.
/// @tparam         ITER_T  Type representing the source and a position therein,
///                         e.g. an iterator or pointer.
/// @param[out]     cu      Code unit read from source.
/// @param[in,out]  src     Current position in the source.
/// @param[in]      end     End (exclusive) of the source.
template<typename UNIT_T, typename ITER_T>
bool GetNext(UNIT_T& cu, ITER_T& src, ITER_T end)
{
    if (src == end) return false; cu = UNIT_T(GetNext(src)); return true;
}

/// Helper function to attempt to read next code unit from buffer-like source.
/// @tparam         UNIT_T  Type representing a source unit.
/// @tparam         ITER_T  Type representing the source and a position therein,
///                         e.g. an iterator or pointer.
/// @param[out]     cu      Code unit read from source.
/// @param[in,out]  src     Current position in the source.
/// @param[in,out]  remain  Number of code units remaining in the source.
template<typename UNIT_T, typename ITER_T>
bool GetNext(UNIT_T& cu, ITER_T& src, size_t& remain)
{
    if (remain == 0) return false; --remain; cu = UNIT_T(GetNext(src)); return true;
}

/// Helper function to attempt to read next code unit from stream-like source.
/// @tparam         UNIT_T  Type representing a source unit.
/// @tparam         ITER_T  Type representing the source and a position therein,
///                         e.g. an iterator or pointer.
/// @param[out]     cu      Code unit read from source.
/// @param[in,out]  src     Current position in the source.
/// @param[in]      end     Value marking the end of the source.
template<typename UNIT_T, typename ITER_T>
bool GetNext(UNIT_T& cu, ITER_T& src, decltype(GetNext(src)) end)
{
    auto result = GetNext(src); if (result == end) return false; cu = UNIT_T(result); return true;
}

/// Decode UTF-8 code sequence lead octet.
/// @param[out]     character       Payload bits of the lead octet, or undefined if invalid.
/// @param[in]      codeUnit        Code unit to decode.
/// @return                         Total expected length of the sequence in code units,
///                                 or 0 if invalid as a first code unit in a sequence.
UTF8SequenceLength DecodeUTF8LeadOctet(UCS4& character, UTF8CodeUnit codeUnit);

/// Decode UTF-8 code sequence continuation octet.
/// @param[in,out]  character       Payload bits of the octets decoded so far, or undefined
///                                 in case of failure.
/// @param[in]      codeUnit        Code unit to decode.
/// @return                         `true` in case of success, or `false` in case of failure
///                                 (implying an invalid encoding).
bool DecodeUTF8ContinuationOctet(UCS4& character, UTF8CodeUnit codeUnit);

/// Generic function to decode UTF-8 code sequence.
///
/// Decodes a single UTF-8 code sequence, comprised of a lead octet and one or more continuation
/// octets.
///
/// The source may be an
///
/// @tparam         ITER_T  Type representing the source and a position therein,
///                         e.g. an iterator or pointer.
/// @tparam         END_T   Type representing the parameter for end-of-buffer detection.
/// @param[out]     c       Decoded UCS code point, or undefined in case of failure.
/// @param[in,out]  src     Iterator pointing to the first code unit in the sequence (in),
///                         the first code unit after the sequence in case of success (out),
///                         or the start of the next presumed good sequence in case of
///                         failure (out).
/// @param[in]      end     Parameter for end-of-buffer detection. May be either an end
///                         iterator/pointer, a reference to a `size_t`variable holding the
///                         number of remaining code units, a special code unit identifying the
///                         end of the buffer (e.g. NUL character), or a special non-code unit
///                         constant returned by the source to signal the end of the data
///                         stream.
/// @return                 `true` in case of success, or `false` in case of failure
///                         (implying an invalid encoding).
template<typename ITER_T, typename END_T>
bool DecodeUTF8Sequence(UCS4& c, ITER_T& iter, END_T end)
{
    UTF8CodeUnit o;

    if (!GetNext(o, iter, end))
        POV_STRING_ASSERT(false);

    auto sequenceLength = DecodeUTF8LeadOctet(c, o);
    if (sequenceLength == 0)
        return false;

    for (UTF8SequenceLength i = 1; i < sequenceLength; ++i)
    {
        if (!GetNext(o, iter, end))
            return false;
        if (!DecodeUTF8ContinuationOctet(c, o))
            return false;
    }

    return true;
}

/// Test for overlong UTF-8 sequence.
/// Tests whether the given character could have been encoded more efficiently, making the encoding
/// invalid according to strict UTF-8.
/// @param[in]  character       Decoded UCS code point.
/// @param[in]  sequenceLength  Total length of the sequence as returned by DecodeUTF8Sequence().
/// @return                     `true` if the character could have been encoded more efficiently.
bool IsUTF8Overlong(UCS4 character, UTF8SequenceLength sequenceLength);

/// Test for UCS valid range.
/// Tests whether the given value qualifies as a valid UCS code point (U+0000-U+10FFFF).
/// @param[in]  character       Candidate UCS code point.
/// @return                     `true` if the code point qualifies as a UCS code point.
bool IsUCSCodePoint(UCS4 character);

/// Test for UCS surrogate code point.
/// Tests whether the given UCS code point is reserved for use in UTF-16 surrogate pair encoding.
/// @param[in]  character       UCS code point.
/// @return                     `true` if the code point is reserved for surrogates.
bool IsUCSSurrogate(UCS4 character);

/// Test for UCS high surrogate code point.
/// Tests whether the given UCS code point is reserved for use as _high surrogate_ (being the first
/// in a pair) in UTF-16 surrogate pair encoding.
/// @param[in]  character       UCS code point.
/// @return                     `true` if the code point is reserved for high surrogates.
bool IsUCSHighSurrogate(UCS4 character);

/// Test for UCS low surrogate code point.
/// Tests whether the given UCS code point is reserved for use as _low surrogate_ (being the second
/// in a pair) in UTF-16 surrogate pair encoding.
/// @param[in]  character       UCS code point.
/// @return                     `true` if the code point is reserved for low surrogates.
bool IsUCSLowSurrogate(UCS4 character);

/// Test for UCS scalar value.
/// Tests whether the given value qualifies as a valid UCS scalar value, i.e. non-surrogate
/// code point (U+0000-U+10FFFF except U+D800-U+DFFF).
/// @param[in]  character       Candidate UCS code point.
/// @return                     `true` if the code point qualifies as a UCS scalar value.
bool IsUCSScalarValue(UCS4 character);

}
// end of namespace UCS

//******************************************************************************

struct Charset
{
    virtual ~Charset();

    /// @pre The supplied value shall be a valid non-surrogate code point.
    /// @return Whether the character can be represented in the character set.
    virtual bool Encode(POV_UINT32& result, UCS4 character) const = 0;

    /// @pre The supplied value shall be a valid character.
    /// @return Whether the character can be represented in UCS-4.
    virtual bool Decode(UCS4& result, POV_UINT32 character) const = 0;

    static const Charset* Get(CharsetID charset);
};

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_STRINGUTILITIES_H
