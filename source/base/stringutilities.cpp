//******************************************************************************
///
/// @file base/stringutilities.cpp
///
/// Implementations related to string handling.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "stringutilities.h"

// C++ variants of standard C header files
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// C++ header files
// Boost header files
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

UCS2String ASCIItoUCS2String(const char *s)
{
    UCS2String r;
    unsigned char ch;

    if (s != nullptr)
    {
        while(*s != 0)
        {
            ch = *s++;
            r += (UCS2)(ch);
        }
    }

    return r;
}

std::string UCS2toASCIIString(const UCS2String& s)
{
    std::string r;
    r.reserve(s.length());

    for(std::size_t i = 0; i < s.length(); i++)
    {
        if(s[i] >= 256)
            r += ' '; // TODO - according to most encoding conventions, this should be '?'
        else
            r += (char)(s[i]);
    }

    return r;
}

/// Implementation of the essential UTF-8 decoding algorithm.
///
/// This function decodes one single character-ish unit from an UTF-8-ish byte sequence, gracefully
/// accepting any quirks thrown at it, such as overlong encodings, code points outside the Unicode
/// range, or code points reserved for surrogate characters, except for fundamentally broken
/// sequences which will be converted to a benign replacement character.
///
UCS4 DecodeUTF8CharacterRaw(UTF8String::const_iterator& i, UTF8String::const_iterator end)
{
    UCS4 c;

    if (!UCS::DecodeUTF8Sequence(c, i, end))
        return UCS::kReplacementCharacter;

    return c;
}

/// Decodes a single UCS4 character from a UTF-8-ish source.
///
/// This function decodes one single UCS4 character from a UTF-8 byte sequence, gracefully
/// accepting various quirks. Sequences that decode to code points outside the valid UTF4 range
/// will be replaced by a benign replacement character, except for surrogate pairs which will
/// be decoded further.
///
UCS4 DecodeUTF8Character(UTF8String::const_iterator& i, UTF8String::const_iterator end)
{
    UCS4 c = DecodeUTF8CharacterRaw(i, end);

    if ((c >= 0xD800u) && (c <= 0xDBFFu))
    {
        // High surrogate; may be the start of a surrogate pair.
        UTF8String::const_iterator mark = i;
        UCS4 c2 = DecodeUTF8CharacterRaw(i, end);
        if ((c2 >= 0xDC00u) && (c2 <= 0xDFFFu))
        {
            // Low surrogate; it's indeed a pair. Decode it.
            c = (((c & 0x3FF) << 10) | (c2 & 0x3FF)) + 0x10000;
        }
        else
        {
            // Not a low surrogate. Forget that we even dared to ask.
            i = mark;
        }
    }

    if (((c >= 0xD800u) && (c <= 0xDFFFu)) || (c > 0x10FFFFu))
        // Isolated surrogate, or outside range of valid UCS4 code points.
        return UCS::kReplacementCharacter;

    // Perfectly good UCS4.
    return c;
}

UCS2String UTF8toUCS2String(const UTF8String& s)
{
    UCS2String result;
    UCS4 c;
    int len = 0;

    // Compute the encoded length by simply counting all non-continuation bytes.
    // In case of malformed sequences we might get the number wrong, but this is just an estimate
    // used to pre-allocate memory.
    for (UTF8String::const_iterator i = s.begin(); i != s.end(); ++i)
        if (((*i) & 0xC0) != 0x80)
            ++len;

    result.reserve(len);

    for(UTF8String::const_iterator i = s.begin(); i != s.end(); )
    {
        c = DecodeUTF8Character(i, s.end());

        if ((c > 0xFFFFu) || (c == 0x0000u))
            // Not valid in UCS2, or we can't deal with it; substitute with a replacement character.
            c = 0xFFFDu;

        result.push_back(c);
    }

    return result;
}

/*****************************************************************************
*
* FUNCTION
*
*   pov_stricmp
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Since the stricmp function isn't available on all systems, we've
*   provided a simplified version of it here.
*
* CHANGES
*
*   -
*
******************************************************************************/

int pov_stricmp(const char *s1, const char *s2)
{
    char c1, c2;

    while((*s1 != 0) && (*s2 != 0))
    {
        c1 = *s1++;
        c2 = *s2++;

        c1 = (char)toupper(c1);
        c2 = (char)toupper(c2);

        if(c1 < c2)
            return -1;
        if(c1 > c2)
            return 1;
    }

    if(*s1 == 0)
    {
        if(*s2 == 0)
            return 0;
        else
            return -1;
    }
    else
        return 1;
}

/*****************************************************************************
*
* FUNCTION
*
*   pov_tsprintf
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Since the stricmp function isn't available on all systems, we've
*   provided a simplified version of it here.
*
* CHANGES
*
*   -
*
******************************************************************************/

const char *pov_tsprintf(const char *format,...)
{
    va_list marker;

    // TODO FIXME -- this is not thread-safe!
    static char pov_tsprintf_buffer[1024];

    va_start(marker, format);
    std::vsnprintf(pov_tsprintf_buffer, sizeof(pov_tsprintf_buffer), format, marker);
    va_end(marker);

    return pov_tsprintf_buffer;
}

//******************************************************************************

std::size_t UCS2_strlen(const UCS2* str)
{
    const UCS2* end = str;
    while (*end != u'\0')
        ++end;
    return (end - str);
}

//******************************************************************************

namespace UCS
{

constexpr unsigned int kUTF8CodeUnitBits    = 8;                            ///< Number of bits in UTF-8 code unit.
constexpr unsigned int kUTF8CodeUnitValues  = 0x01u << kUTF8CodeUnitBits;   ///< Number of different values an UTF-8 code unit can represent.

/// Expected number of UTF-8 code units, based on first code unit.
/// This lookup table gives the number of code units constituting an
/// @glossary{UTF8}-encoded character, including the lead octet.
const unsigned char kaUTF8SequenceLength[kUTF8CodeUnitValues] =
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    // continuation octets
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    // continuation octets
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    // continuation octets
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    // continuation octets
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,5,5,5,5,6,6,0,0,
};

UTF8SequenceLength DecodeUTF8LeadOctet(UCS4& c, UTF8CodeUnit codeUnit)
{
    auto sequenceLength = kaUTF8SequenceLength[codeUnit];
    if (sequenceLength == 0)
        return 0;

    // Technically, the following should be `0x7F >> sequenceLength` except for single-octet
    // sequences, but we can get away with this because the least significant non-payload bit in a
    // lead octet is always zero.
    UTF8CodeUnit leadMask = (UTF8CodeUnit(0xFF >> sequenceLength));
    c = codeUnit & leadMask;

    return sequenceLength;
}

bool DecodeUTF8ContinuationOctet(UCS4& c, UTF8CodeUnit codeUnit)
{
    if ((codeUnit & UTF8CodeUnit(0xC0)) != UTF8CodeUnit(0x80))
        return false;

    c = (c << 6) + (codeUnit & UTF8CodeUnit(0x3F));

    return true;
}

bool IsUTF8Overlong(UCS4 c, UTF8SequenceLength sequenceLength)
{
    switch (sequenceLength)
    {
        case 1: return false;
        case 2: return (c > 0x007Fu);
        case 3: return (c > 0x07FFu);
        case 4: return (c > 0xFFFFu);
        default: POV_STRING_ASSERT(false); return true;
    }
}

bool IsUCSCodePoint(UCS4 c)
{
    return (c >= 0) && (c <= 0x10FFFFu);
}

bool IsUCSSurrogate(UCS4 c)
{
    return (c >= 0xD800u) && (c <= 0xDFFFu);
}

bool IsUCSHighSurrogate(UCS4 c)
{
    return (c >= 0xD800u) && (c <= 0xDBFFu);
}

bool IsUCSLowSurrogate(UCS4 c)
{
    return (c >= 0xDC00u) && (c <= 0xDFFFu);
}

bool IsUCSScalarValue(UCS4 c)
{
    return IsUCSCodePoint(c) && !IsUCSSurrogate(c);
}

} // namespace UCS

}
