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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "stringutilities.h"

// C++ variants of C standard header files
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// C++ standard header files
#include <initializer_list>

// Boost header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/types.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

UCS2String ASCIItoUCS2String(const std::string& s)
{
    UCS2String r;
    r.reserve(s.length());

    for(std::size_t i = 0; i < s.length(); i++)
    {
        r += (UCS2)(s[i]);
    }

    return r;
}

UCS2String SysToUCS2String(const char *s)
{
    UCS2String r;
    unsigned char ch;

    if (s != nullptr)
    {
        while(*s != u'\0')
        {
            ch = *s++;
            r += (UCS2)(ch);
        }
    }

    return r;
}

UCS2String SysToUCS2String(const std::string& s)
{
    UCS2String r;
    r.reserve(s.length());

    for(std::size_t i = 0; i < s.length(); i++)
    {
        r += (UCS2)(s[i]);
    }

    return r;
}

std::string UCS2toSysString(const UCS2String& s)
{
    std::string r;
    r.reserve(s.length());

    for(std::size_t i = 0; i < s.length(); i++)
    {
        if(s[i] > 0xFFu)
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

    while((*s1 != '\0') && (*s2 != '\0'))
    {
        c1 = *s1++;
        c2 = *s2++;

        c1 = (char)std::toupper(c1);
        c2 = (char)std::toupper(c2);

        if(c1 < c2)
            return -1;
        if(c1 > c2)
            return 1;
    }

    if(*s1 == '\0')
    {
        if(*s2 == '\0')
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

//------------------------------------------------------------------------------

int UCS2_strcmp(const UCS2 *s1, const UCS2 *s2)
{
    UCS2 t1, t2;

    while ((t1 = *s1++) == (t2 = *s2++))
    {
        if (t1 == '\0')
            return 0;
    }

    return (t1 - t2);
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
        case 2: return (c <= 0x007Fu);
        case 3: return (c <= 0x07FFu);
        case 4: return (c <= 0xFFFFu);
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

} // end of namespace UCS

//******************************************************************************

struct CharsetUCS4Subset : public Charset
{
    virtual bool Decode(UCS4& result, POV_UINT32 character) const override
    {
        result = UCS4(character);
        return true;
    }
};

//------------------------------------------------------------------------------

struct CharsetUCS4 final : public CharsetUCS4Subset
{
    virtual bool Encode(POV_UINT32& result, UCS4 character) const override
    {
        result = POV_UINT32(character);
        return true;
    }
};

static const auto kCharsetUCS4 = CharsetUCS4();

//------------------------------------------------------------------------------

struct CharsetUCS2 final : public CharsetUCS4Subset
{
    virtual bool Encode(POV_UINT32& result, UCS4 character) const override
    {
        if (character <= 0xFFFFu)
        {
            result = POV_UINT32(character);
            return true;
        }
        else
        {
            result = UCS::kReplacementCharacter;
            return false;
        }
    }
};

static const auto kCharsetUCS2 = CharsetUCS2();

//------------------------------------------------------------------------------

struct CharsetLatin1 final : public CharsetUCS4Subset
{
    virtual bool Encode(POV_UINT32& result, UCS4 character) const override
    {
        if (character <= 0xFFu)
        {
            result = POV_UINT32(character);
            return true;
        }
        else
        {
            result = '?';
            return false;
        }
    }
};

static const auto kCharsetLatin1 = CharsetLatin1();

//------------------------------------------------------------------------------

struct CharsetASCII final : public CharsetUCS4Subset
{
    virtual bool Encode(POV_UINT32& result, UCS4 character) const override
    {
        if (character <= 0x7Fu)
        {
            result = POV_UINT32(character);
            return true;
        }
        else
        {
            result = '?';
            return false;
        }
    }
};

static const auto kCharsetASCII = CharsetASCII();

//------------------------------------------------------------------------------

struct CharsetExtendedASCII final : public Charset
{
    CharsetExtendedASCII(std::initializer_list<UCS4> highCharacters)
    {
        int i = 0;
        for (UCS4 c : highCharacters)
            mHighCharacters[i++] = c;
        for (; i < kNumNonASCIICharacters; ++i)
            mHighCharacters[i] = kFirstNonASCIICharacter + i;
    }

    virtual bool Encode(POV_UINT32& result, UCS4 character) const override
    {
        if (character < kFirstNonASCIICharacter)
        {
            result = POV_UINT32(character);
            return true;
        }
        else
        {
            for (int i = 0; i < kNumNonASCIICharacters; ++i)
            {
                if (mHighCharacters[i] == character)
                {
                    result = kFirstNonASCIICharacter + i;
                    return true;
                }
            }
            result = '?';
            return false;
        }
    }

    virtual bool Decode(UCS4& result, POV_UINT32 character) const override
    {
        if (character < kFirstNonASCIICharacter)
        {
            result = UCS4(character);
            return true;
        }
        else if (character - kFirstNonASCIICharacter < kNumNonASCIICharacters)
        {
            result = mHighCharacters[character - kFirstNonASCIICharacter];
            return (result != UCS::kReplacementCharacter);
        }
        else
        {
            result = UCS::kReplacementCharacter;
            return false;
        }
    }

private:

    static constexpr auto kFirstNonASCIICharacter   = 0x80u;
    static constexpr auto kNumNonASCIICharacters    = 128;
    UCS4 mHighCharacters[kNumNonASCIICharacters];
};

static const CharsetExtendedASCII kCharsetWindows1251({
    // Adapted from https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WindowsBestFit/bestfit1251.txt [2019-01-10]
    0x0402,0x0403,0x201A,0x0453,0x201E,0x2026,0x2020,0x2021,0x20AC,0x2030,0x0409,0x2039,0x040A,0x040C,0x040B,0x040F,
    0x0452,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,0x0098,0x2122,0x0459,0x203A,0x045A,0x045C,0x045B,0x045F,
    0x00A0,0x040E,0x045E,0x0408,0x00A4,0x0490,0x00A6,0x00A7,0x0401,0x00A9,0x0404,0x00AB,0x00AC,0x00AD,0x00AE,0x0407,
    0x00B0,0x00B1,0x0406,0x0456,0x0491,0x00B5,0x00B6,0x00B7,0x0451,0x2116,0x0454,0x00BB,0x0458,0x0405,0x0455,0x0457,
    0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
    0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
    0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
    0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F,
});

static const CharsetExtendedASCII kCharsetWindows1252({
    // Adapted from https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WindowsBestFit/bestfit1252.txt [2019-01-04]
    0x20AC,0x0081,0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,0x02C6,0x2030,0x0160,0x2039,0x0152,0x008D,0x017D,0x008F,
    0x0090,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,0x02DC,0x2122,0x0161,0x203A,0x0153,0x009D,0x017E,0x0178,
});

static const CharsetExtendedASCII kCharsetMacOSRoman({
    // Adapted from https://www.unicode.org/Public/MAPPINGS/VENDORS/APPLE/ROMAN.TXT [2019-01-04]
    0x00C4,0x00C5,0x00C7,0x00C9,0x00D1,0x00D6,0x00DC,0x00E1,0x00E0,0x00E2,0x00E4,0x00E3,0x00E5,0x00E7,0x00E9,0x00E8,
    0x00EA,0x00EB,0x00ED,0x00EC,0x00EE,0x00EF,0x00F1,0x00F3,0x00F2,0x00F4,0x00F6,0x00F5,0x00FA,0x00F9,0x00FB,0x00FC,
    0x2020,0x00B0,0x00A2,0x00A3,0x00A7,0x2022,0x00B6,0x00DF,0x00AE,0x00A9,0x2122,0x00B4,0x00A8,0x2260,0x00C6,0x00D8,
    0x221E,0x00B1,0x2264,0x2265,0x00A5,0x00B5,0x2202,0x2211,0x220F,0x03C0,0x222B,0x00AA,0x00BA,0x03A9,0x00E6,0x00F8,
    0x00BF,0x00A1,0x00AC,0x221A,0x0192,0x2248,0x2206,0x00AB,0x00BB,0x2026,0x00A0,0x00C0,0x00C3,0x00D5,0x0152,0x0153,
    0x2013,0x2014,0x201C,0x201D,0x2018,0x2019,0x00F7,0x25CA,0x00FF,0x0178,0x2044,0x20AC,0x2039,0x203A,0xFB01,0xFB02,
    0x2021,0x00B7,0x201A,0x201E,0x2030,0x00C2,0x00CA,0x00C1,0x00CB,0x00C8,0x00CD,0x00CE,0x00CF,0x00CC,0x00D3,0x00D4,
    0xF8FF,0x00D2,0x00DA,0x00DB,0x00D9,0x0131,0x02C6,0x02DC,0x00AF,0x02D8,0x02D9,0x02DA,0x00B8,0x02DD,0x02DB,0x02C7,
});

//------------------------------------------------------------------------------

struct CharsetLegacySymbols final : public Charset
{
    CharsetLegacySymbols(UCS4 base) :
        mBase(base)
    {}

    virtual bool Encode(POV_UINT32& result, UCS4 character) const override
    {
        if (character <= kNumValidCharacters)
        {
            result = mBase + character;
            return true;
        }
        else
        {
            result = UCS::kReplacementCharacter;
            return false;
        }
    }

    virtual bool Decode(UCS4& result, POV_UINT32 character) const override
    {
        if ((character >= mBase) && (character - mBase <= kNumValidCharacters))
        {
            result = character - mBase;
            return true;
        }
        else
        {
            result = ' '; // there's no sane replacement character
            return false;
        }
    }

private:

    static constexpr auto kNumValidCharacters = 256;
    UCS4 mBase;
};

static const CharsetLegacySymbols kCharsetLegacySymbols(0xF000);

//------------------------------------------------------------------------------

Charset::~Charset()
{}

const Charset* Charset::Get(CharsetID charset)
{
    switch (charset)
    {
        case CharsetID::kUndefined:         //FALLTHROUGH
        case CharsetID::kUCS4:              return &kCharsetUCS4;
        case CharsetID::kUCS2:              return &kCharsetUCS2;
        case CharsetID::kLatin1:            return &kCharsetLatin1;

        case CharsetID::kWindows1251:       return &kCharsetWindows1251;
        case CharsetID::kWindows1252:       return &kCharsetWindows1252;
        case CharsetID::kMacOSRoman:        return &kCharsetMacOSRoman;

        case CharsetID::kLegacySymbols:     return &kCharsetLegacySymbols;

        default:                            return nullptr;
    }
}

}
// end of namespace pov_base
