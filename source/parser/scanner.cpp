//******************************************************************************
///
/// @file parser/scanner.cpp
///
/// Implementation of the _scanner_ stage of the parser.
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
#include "parser/scanner.h"

// C++ variants of C standard header files
#include <cstdio>
#include <cstdlib>
#include <cstring>

// C++ standard header files
#include <initializer_list>
#include <limits>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/povassert.h"
#include "base/stringutilities.h"
#include "base/textstream.h"

// POV-Ray header files (core module)
// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

constexpr unsigned int  kOctetBits      = 8;                    ///< Number of bits in an octet.
constexpr unsigned int  kOctetValues    = 0x01u << kOctetBits;  ///< Number of different values an octet can represent.

//******************************************************************************

/// Abstract class representing a character encoding scheme.
///
/// This is the base for classes representing a character encoding scheme and
/// implementing the corresponding decoding.
///
/// @note
///     The current framework only supports character encoding schemes that are
///     stateless single-byte or multi-byte variable width extensions to ASCII.
///
struct CharacterEncoding
{
    using OctetSequenceLength = unsigned int;
    using CharacterEncodingPtr = Scanner::CharacterEncodingPtr;

    virtual ~CharacterEncoding() {}

    static bool IsASCII(Octet codeUnit)
    {
        return ((codeUnit & Octet(0x80)) == Octet(0x00));
    }

    /// Decode and discard next character.
    ///
    /// This method is called by @ref Scanner to skip over a single non-ASCII
    /// character.
    ///
    /// @note
    ///     This method shall test for encoding errors and throw an
    ///     @ref InvalidEncodingException upon encountering any such errors.
    ///     The stream position reported in the exception may be imprecise
    ///     in that it may point to any byte that constitutes part of the
    ///     offending code sequence.
    ///
    /// @note
    ///     This method is called in performance-critical sections, and should
    ///     therefore be implemented with execution speed in mind.
    ///
    /// @param[in]  scanner     The scanner to retrieve octets from.
    ///
    virtual void DecodeCharacter(Scanner& scanner) const = 0;

    /// Transcode next character to UTF-8.
    ///
    /// This method is called by @ref Scanner to transcode a single non-ASCII
    /// character to the encoding scheme used internally (UTF-8).
    ///
    /// @note
    ///     This method shall test for encoding errors and throw an
    ///     @ref InvalidEncodingException upon encountering any such errors.
    ///     The stream position reported in the exception may be imprecise
    ///     in that it may point to any byte that constitutes part of the
    ///     offending code sequence.
    ///
    /// @note
    ///     Characters that cannot be represented in UCS should be transcoded
    ///     to the UTF-8 representation of the UCS replacement character
    ///     (U+FFFD).
    ///
    /// @note
    ///     This method is called in performance-critical sections, and should
    ///     therefore be implemented with execution speed in mind. Direct
    ///     transcoding should be favoured over decoding (e.g. to UCS4) and
    ///     recoding where feasible.
    ///
    /// @param[in]  scanner     The scanner to retrieve octets from.
    /// @param[in]  pLexemeText Pointer to a string to append the transcoded
    ///                         character to, or `nullptr` to indicate that the
    ///                         character should be discarded.
    ///
    virtual void DecodeCharacter(Scanner& scanner, UTF8String* pLexemeText) const = 0;

    /// Decode next character as UCS4.
    ///
    /// This method is called by @ref Scanner when a non-ASCII character is
    /// encountered in a context where only ASCII characters are expected
    /// (most notably anywhere outside string literals or comments), to decode
    /// the character to the equivalent UCS code point for later use in an
    /// error message.
    ///
    /// @note
    ///     This method shall test for encoding errors and throw an
    ///     @ref InvalidEncodingException upon encountering any such errors.
    ///     The stream position reported in the exception may be imprecise
    ///     in that it may point to any byte that constitutes part of the
    ///     offending code sequence.
    ///
    /// @note
    ///     Characters that cannot be represented in UCS should be reported
    ///     as the UCS replacement character (U+FFFD).
    ///
    /// @note
    ///     This method is not intended to be called in performance-critical
    ///     sections. For easier maintenance of subclasses, its default
    ///     implementation invokes transcoding to UTF-8 and then decodes the
    ///     result to UCS4.
    ///
    virtual void DecodeCharacter(Scanner& scanner, UCS4& character) const
    {
        UTF8String utf8Character;
        DecodeCharacter(scanner, &utf8Character);
        auto utf8CharacterIter = utf8Character.begin();
        if (!UCS::DecodeUTF8Sequence(character, utf8CharacterIter, utf8Character.end()))
            POV_PARSER_PANIC();
    }

    /// Get a descriptive name of the encoding.
    virtual const char* GetName() const = 0;

protected:

    /// @name Helper function for subclasses
    /// @{

    /// Access next octet in buffer.
    static inline Octet GetNextOctet(Scanner& scanner)
    {
        return scanner.NextOctet();
    }

    /// Advance buffer by one octet.
    static inline bool AdvanceOctet(Scanner& scanner)
    {
        return scanner.AdvanceOctet();
    }

    /// Append UTF-8 code unit to UTF-8 string.
    static inline void Write(UTF8String* pLexemeText, Octet utf8CodeUnit)
    {
        if (pLexemeText != nullptr)
            *pLexemeText += utf8CodeUnit;
    }

    /// Append sequence of UTF-8 code units to UTF-8 string.
    static inline void Write(UTF8String* pLexemeText, const char* utf8CodeUnits)
    {
        if (pLexemeText != nullptr)
            *pLexemeText += utf8CodeUnits;
    }

    /// Change character encoding scheme.
    /// This function is called by auto-detecting implementations to switch to
    /// an implementation specialized for the detected encoding.
    static inline void SetCharacterEncoding(Scanner& scanner, CharacterEncodingPtr pEncoding)
    {
        scanner.mpCharacterEncoding = pEncoding;
    }

    /// Report invalid encoding.
    void ThrowInvalidEncodingException(Scanner& scanner, const char* details = nullptr) const
    {
        throw InvalidEncodingException(scanner.GetInputStreamName(), scanner.mCurrentPosition, GetName(), details);
    }

    /// @}
};

//------------------------------------------------------------------------------

/// Strict UTF-8 encoding scheme.
///
/// @note
///     This implementation insists on well-formed UTF-8, and will reject any overlong
///     encodings (including such encodings for the NUL character, as allowed in MUTF-8) or
///     surrogates (as allowed in CESU-8 and WTF-8).
///
struct UTF8Encoding final : CharacterEncoding
{
    virtual void DecodeCharacter(Scanner& scanner) const override
    {
        POV_EXPERIMENTAL_ASSERT(!IsASCII(GetNextOctet(scanner))); // ASCII would be ok but unexpected.
        UCS4 character;
        DecodeCharacter(scanner, character, nullptr);
    }

    virtual void DecodeCharacter(Scanner& scanner, UTF8String* pLexemeText) const override
    {
        POV_EXPERIMENTAL_ASSERT(!IsASCII(GetNextOctet(scanner))); // ASCII would be ok but unexpected.
        UCS4 character;
        DecodeCharacter(scanner, character, pLexemeText);
    }

    virtual void DecodeCharacter(Scanner& scanner, UCS4& character) const override
    {
        POV_EXPERIMENTAL_ASSERT(!IsASCII(GetNextOctet(scanner))); // ASCII would be ok but unexpected.
        DecodeCharacter(scanner, character, nullptr);
    }

    virtual const char* GetName() const override
    {
        return "UTF-8";
    }

    static CharacterEncodingPtr Instance()
    {
        static auto instance = CharacterEncodingPtr(new UTF8Encoding);
        return instance;
    }

protected:

    UTF8Encoding() = default;

    inline void DecodeCharacter(Scanner& scanner, UCS4& character, UTF8String* pLexemeText) const
    {
        UCS::UTF8SequenceLength length = UCS::DecodeUTF8LeadOctet(character, GetNextOctet(scanner));
        if (length == 0)
            ThrowInvalidEncodingException(scanner, "lead octet expected");
        Write(pLexemeText, GetNextOctet(scanner));
        for (UCS::UTF8SequenceLength i = 1; i < length; ++i)
        {
            if (!AdvanceOctet(scanner) || !UCS::DecodeUTF8ContinuationOctet(character, GetNextOctet(scanner)))
                ThrowInvalidEncodingException(scanner, "continuation octet expected");
            Write(pLexemeText, GetNextOctet(scanner));
        }
        if (UCS::IsUTF8Overlong(character, length))
            ThrowInvalidEncodingException(scanner, "overlong encoding");
        if (!UCS::IsUCSCodePoint(character))
            ThrowInvalidEncodingException(scanner, "invalid UCS code point");
        if (UCS::IsUCSSurrogate(character))
            ThrowInvalidEncodingException(scanner, "surrogate code point");
        AdvanceOctet(scanner);
    }
};

//------------------------------------------------------------------------------

/// Strict ASCII encoding scheme.
///
struct ASCIIEncoding final : CharacterEncoding
{
    virtual void DecodeCharacter(Scanner& scanner) const override
    {
        POV_EXPERIMENTAL_ASSERT(!IsASCII(GetNextOctet(scanner))); // ASCII would be ok but unexpected.
        if (!IsASCII(GetNextOctet(scanner)))
            ThrowInvalidEncodingException(scanner);
    }

    virtual void DecodeCharacter(Scanner& scanner, UTF8String* pLexemeText) const override
    {
        POV_EXPERIMENTAL_ASSERT(!IsASCII(GetNextOctet(scanner))); // ASCII would be ok but unexpected.
        if (!IsASCII(GetNextOctet(scanner)))
            ThrowInvalidEncodingException(scanner);
        Write(pLexemeText, GetNextOctet(scanner));
    }

    virtual void DecodeCharacter(Scanner& scanner, UCS4& character) const override
    {
        POV_EXPERIMENTAL_ASSERT(!IsASCII(GetNextOctet(scanner))); // ASCII would be ok but unexpected.
        if (!IsASCII(GetNextOctet(scanner)))
            ThrowInvalidEncodingException(scanner);
        character = GetNextOctet(scanner);
    }

    virtual const char* GetName() const override
    {
        return "ASCII";
    }

    static CharacterEncodingPtr Instance()
    {
        static auto instance = CharacterEncodingPtr(new ASCIIEncoding);
        return instance;
    }

protected:

    ASCIIEncoding() = default;
};

//------------------------------------------------------------------------------

const char* kaLatin1ToUTF8[kOctetValues] = {

    // 0x00-0x1F: C0 Controls.
    "\x00",     "\x01",     "\x02",     "\x03",     "\x04",     "\x05",     "\x06",     "\x07",
    "\x08",     "\x09",     "\x0A",     "\x0B",     "\x0C",     "\x0D",     "\x0E",     "\x0F",
    "\x10",     "\x11",     "\x12",     "\x13",     "\x14",     "\x15",     "\x16",     "\x17",
    "\x18",     "\x19",     "\x1A",     "\x1B",     "\x1C",     "\x1D",     "\x1E",     "\x1F",

    // 0x20-0x7F: Basic Latin.
    " ",        "!",        "\"",       "#",        "$",        "%",        "&",        "'",
    "(",        ")",        "*",        "+",        ",",        "-",        ".",        "/",
    "0",        "1",        "2",        "3",        "4",        "5",        "6",        "7",
    "8",        "9",        ":",        ";",        "<",        "=",        ">",        "?",
    "@",        "A",        "B",        "C",        "D",        "E",        "F",        "G",
    "H",        "I",        "J",        "K",        "L",        "M",        "N",        "O",
    "P",        "Q",        "R",        "S",        "T",        "U",        "V",        "W",
    "X",        "Y",        "Z",        "[",        "\\",       "]",        "^",        "_",
    "`",        "a",        "b",        "c",        "d",        "e",        "f",        "g",
    "h",        "i",        "j",        "k",        "l",        "m",        "n",        "o",
    "p",        "q",        "r",        "s",        "t",        "u",        "v",        "w",
    "x",        "y",        "z",        "{",        "|",        "}",        "~",        "\x7F",

    // 0x80-0x9F: C1 Controls.
    u8"\u0080", u8"\u0081", u8"\u0082", u8"\u0083", u8"\u0084", u8"\u0085", u8"\u0086", u8"\u0087",
    u8"\u0088", u8"\u0089", u8"\u008A", u8"\u008B", u8"\u008C", u8"\u008D", u8"\u008E", u8"\u008F",
    u8"\u0090", u8"\u0091", u8"\u0092", u8"\u0093", u8"\u0094", u8"\u0095", u8"\u0096", u8"\u0097",
    u8"\u0098", u8"\u0099", u8"\u009A", u8"\u009B", u8"\u009C", u8"\u009D", u8"\u009E", u8"\u009F",

    // 0xA0-0xFF: Latin-1 Supplement.
    u8"\u00A0", u8"\u00A1", u8"\u00A2", u8"\u00A3", u8"\u00A4", u8"\u00A5", u8"\u00A6", u8"\u00A7",
    u8"\u00A8", u8"\u00A9", u8"\u00AA", u8"\u00AB", u8"\u00AC", u8"\u00AD", u8"\u00AE", u8"\u00AF",
    u8"\u00B0", u8"\u00B1", u8"\u00B2", u8"\u00B3", u8"\u00B4", u8"\u00B5", u8"\u00B6", u8"\u00B7",
    u8"\u00B8", u8"\u00B9", u8"\u00BA", u8"\u00BB", u8"\u00BC", u8"\u00BD", u8"\u00BE", u8"\u00BF",
    u8"\u00C0", u8"\u00C1", u8"\u00C2", u8"\u00C3", u8"\u00C4", u8"\u00C5", u8"\u00C6", u8"\u00C7",
    u8"\u00C8", u8"\u00C9", u8"\u00CA", u8"\u00CB", u8"\u00CC", u8"\u00CD", u8"\u00CE", u8"\u00CF",
    u8"\u00D0", u8"\u00D1", u8"\u00D2", u8"\u00D3", u8"\u00D4", u8"\u00D5", u8"\u00D6", u8"\u00D7",
    u8"\u00D8", u8"\u00D9", u8"\u00DA", u8"\u00DB", u8"\u00DC", u8"\u00DD", u8"\u00DE", u8"\u00DF",
    u8"\u00E0", u8"\u00E1", u8"\u00E2", u8"\u00E3", u8"\u00E4", u8"\u00E5", u8"\u00E6", u8"\u00E7",
    u8"\u00E8", u8"\u00E9", u8"\u00EA", u8"\u00EB", u8"\u00EC", u8"\u00ED", u8"\u00EE", u8"\u00EF",
    u8"\u00F0", u8"\u00F1", u8"\u00F2", u8"\u00F3", u8"\u00F4", u8"\u00F5", u8"\u00F6", u8"\u00F7",
    u8"\u00F8", u8"\u00F9", u8"\u00FA", u8"\u00FB", u8"\u00FC", u8"\u00FD", u8"\u00FE", u8"\u00FF",
};

/// Fixed-width 8-bit encoding scheme.
///
struct Fixed8BitEncoding final : CharacterEncoding
{
    Fixed8BitEncoding(const char* name, Octet start, std::initializer_list<const char*> utf8Data) :
        mName(name)
    {
        POV_PARSER_ASSERT(start + utf8Data.size() <= kOctetValues);
        unsigned int i;
        for (i = 0; i < kOctetValues; ++i)
            mToUTF8[i] = kaLatin1ToUTF8[i];
        i = start;
        for (const char* toUTF8 : utf8Data)
            mToUTF8[i++] = toUTF8;
    }

    virtual void DecodeCharacter(Scanner& scanner) const override
    {
        // Call method directly rather than via virtual method lookup.
        Fixed8BitEncoding::DecodeCharacter(scanner, nullptr);
    }

    virtual void DecodeCharacter(Scanner& scanner, UTF8String* pLexemeText) const override
    {
        POV_EXPERIMENTAL_ASSERT(!IsASCII(GetNextOctet(scanner))); // ASCII would be ok but unexpected.
        const char* transcodedUTF8 = mToUTF8[GetNextOctet(scanner)];
        if (transcodedUTF8 == nullptr)
            ThrowInvalidEncodingException(scanner);
        Write(pLexemeText, transcodedUTF8);
        AdvanceOctet(scanner);
    }

    virtual const char* GetName() const override
    {
        return mName;
    }

protected:

    const char* mName;
    const char* mToUTF8[kOctetValues];
};

static auto kNA = nullptr;

static const auto kLatin1Encoding = Fixed8BitEncoding("Latin-1", 0x00, {});

static const auto kMacOSRomanEncoding = Fixed8BitEncoding("Mac OS Roman", 0x80, {
    // Adapted from https://www.unicode.org/Public/MAPPINGS/VENDORS/APPLE/ROMAN.TXT [2019-01-04]
    u8"\u00C4", u8"\u00C5", u8"\u00C7", u8"\u00C9", u8"\u00D1", u8"\u00D6", u8"\u00DC", u8"\u00E1",
    u8"\u00E0", u8"\u00E2", u8"\u00E4", u8"\u00E3", u8"\u00E5", u8"\u00E7", u8"\u00E9", u8"\u00E8",
    u8"\u00EA", u8"\u00EB", u8"\u00ED", u8"\u00EC", u8"\u00EE", u8"\u00EF", u8"\u00F1", u8"\u00F3",
    u8"\u00F2", u8"\u00F4", u8"\u00F6", u8"\u00F5", u8"\u00FA", u8"\u00F9", u8"\u00FB", u8"\u00FC",
    u8"\u2020", u8"\u00B0", u8"\u00A2", u8"\u00A3", u8"\u00A7", u8"\u2022", u8"\u00B6", u8"\u00DF",
    u8"\u00AE", u8"\u00A9", u8"\u2122", u8"\u00B4", u8"\u00A8", u8"\u2260", u8"\u00C6", u8"\u00D8",
    u8"\u221E", u8"\u00B1", u8"\u2264", u8"\u2265", u8"\u00A5", u8"\u00B5", u8"\u2202", u8"\u2211",
    u8"\u220F", u8"\u03C0", u8"\u222B", u8"\u00AA", u8"\u00BA", u8"\u03A9", u8"\u00E6", u8"\u00F8",
    u8"\u00BF", u8"\u00A1", u8"\u00AC", u8"\u221A", u8"\u0192", u8"\u2248", u8"\u2206", u8"\u00AB",
    u8"\u00BB", u8"\u2026", u8"\u00A0", u8"\u00C0", u8"\u00C3", u8"\u00D5", u8"\u0152", u8"\u0153",
    u8"\u2013", u8"\u2014", u8"\u201C", u8"\u201D", u8"\u2018", u8"\u2019", u8"\u00F7", u8"\u25CA",
    u8"\u00FF", u8"\u0178", u8"\u2044", u8"\u20AC", u8"\u2039", u8"\u203A", u8"\uFB01", u8"\uFB02",
    u8"\u2021", u8"\u00B7", u8"\u201A", u8"\u201E", u8"\u2030", u8"\u00C2", u8"\u00CA", u8"\u00C1",
    u8"\u00CB", u8"\u00C8", u8"\u00CD", u8"\u00CE", u8"\u00CF", u8"\u00CC", u8"\u00D3", u8"\u00D4",
    u8"\uF8FF", u8"\u00D2", u8"\u00DA", u8"\u00DB", u8"\u00D9", u8"\u0131", u8"\u02C6", u8"\u02DC",
    u8"\u00AF", u8"\u02D8", u8"\u02D9", u8"\u02DA", u8"\u00B8", u8"\u02DD", u8"\u02DB", u8"\u02C7",
});

static const auto kWindows1252Encoding = Fixed8BitEncoding("Windows-1252", 0x80, {
    // Adapted from https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WindowsBestFit/bestfit1252.txt [2019-01-04]
    u8"\u20AC", kNA,        u8"\u201A", u8"\u0192", u8"\u201E", u8"\u2026", u8"\u2020", u8"\u2021",
    u8"\u02C6", u8"\u2030", u8"\u0160", u8"\u2039", u8"\u0152", kNA,        u8"\u017D", kNA,
    kNA,        u8"\u2018", u8"\u2019", u8"\u201C", u8"\u201D", u8"\u2022", u8"\u2013", u8"\u2014",
    u8"\u02DC", u8"\u2122", u8"\u0161", u8"\u203A", u8"\u0153", kNA,        u8"\u017E", u8"\u0178",
});

//------------------------------------------------------------------------------

struct AutoDetectEncoding final : CharacterEncoding
{
    virtual void DecodeCharacter(Scanner& scanner) const override
    {
        AutoDetectEncoding::DecodeCharacter(scanner, nullptr); // Direct call, no virtual method lookup.
    }

    virtual void DecodeCharacter(Scanner& scanner, UTF8String* pLexemeText) const override
    {
        POV_EXPERIMENTAL_ASSERT(!IsASCII(GetNextOctet(scanner))); // ASCII would be ok but unexpected.

        if (IsASCII(GetNextOctet(scanner)))
        {
            Write(pLexemeText, GetNextOctet(scanner));
            return;
        }

        // Try decoding the next octets as UTF-8. If successful, switch over to UTF-8 for good,
        // otherwise rewind, switch over to Windows-1252 and decode again.

        // Mark current scanner position so we can rewind if we're mistaken.
        Scanner::HotBookmark startOfSequence = scanner.GetHotBookmark();
        try
        {
            // Not decoding directly to pLexemeText because it would be
            // difficult to roll back in case of a failure.
            UTF8String s;
            UTF8Encoding::Instance()->DecodeCharacter(scanner, &s);
            Write(pLexemeText, s.c_str());
            SetCharacterEncoding(scanner, UTF8Encoding::Instance());
        }
        catch (const InvalidEncodingException&)
        {
            scanner.GoToBookmark(startOfSequence);
            kWindows1252Encoding.DecodeCharacter(scanner, pLexemeText);
            SetCharacterEncoding(scanner, &kWindows1252Encoding);
        }
    }

    virtual const char* GetName() const override
    {
        POV_PARSER_ASSERT(false);
        return "(not-yet-identified format)";
    }

    static CharacterEncodingPtr Instance()
    {
        static auto instance = CharacterEncodingPtr(new AutoDetectEncoding);
        return instance;
    }

protected:

    AutoDetectEncoding() = default;
};

//------------------------------------------------------------------------------

/// Test whether character qualifies as ASCII end-of-line marker.
static inline bool IsASCIIEndOfLine(Scanner::Character c)
{
    return ((c == 0x0A) ||  // ASCII LF ("New Line")
            (c == 0x0D));   // ASCII CR ("Carriage Return")
}

/// Test whether character qualifies as ASCII whitespace.
static inline bool IsASCIIWhitespace(Scanner::Character c)
{
    return ((c == 0x09) ||  // ASCII HT ("Tab")
            (c == 0x0A) ||  // ASCII LF ("New Line")
            (c == 0x0D) ||  // ASCII CR ("Carriage Return")
            (c == 0x1A) ||  // DOS EOF character
            (c == 0x20));   // ASCII SP ("Space")
}

/// Test whether character qualifies as printable ASCII.
static bool IsPrintableASCII(Scanner::Character c)
{
    return ((c >= 0x20) && (c <= 0x7E));
}

/// Test whether character qualifies as decimal digit.
/// @note Decimal digits are guaranteed to be ASCII.
static bool IsDecimalDigit(Scanner::Character c)
{
    return ((c >= '0') && (c <= '9'));
}

/// Test whether character qualifies as ASCII first identifier character.
static bool IsASCIIIdentifierChar1(Scanner::Character c)
{
    return (((c >= 'a') && (c <= 'z')) ||
            ((c >= 'A') && (c <= 'Z')) ||
            (c == '_'));
}

/// Test whether character qualifies as subsequent identifier character.
static bool IsASCIIIdentifierChar2(Scanner::Character c)
{
    return IsASCIIIdentifierChar1(c) ||
           IsDecimalDigit(c);
}

//******************************************************************************

UCS2String Scanner::HotBookmark::GetFileName() const
{
    return pStream->Name();
}

//******************************************************************************

Scanner::Buffer::Buffer() :
    mpEnd(maData),
    mpPos(maData)
{}

void Scanner::Buffer::Clear()
{
    mpEnd = maData;
    mpPos  = maData;
}

void Scanner::Buffer::Advance(size_t delta)
{
    POV_PARSER_ASSERT(mpEnd >= mpPos);
    POV_PARSER_ASSERT(mpEnd - mpPos >= delta);
    mpPos += delta;
}

void Scanner::Buffer::AdvanceTo(size_t offset)
{
    POV_PARSER_ASSERT(mpEnd >= maData);
    POV_PARSER_ASSERT(mpEnd - maData >= offset);
    mpPos = maData + offset;
}

size_t Scanner::Buffer::Capacity() const
{
    return kCapacity;
}

size_t Scanner::Buffer::TotalCount() const
{
    POV_PARSER_ASSERT(mpEnd >= maData);
    return (mpEnd - maData);
}

size_t Scanner::Buffer::ProcessedCount() const
{
    POV_PARSER_ASSERT(mpPos >= maData);
    return (mpPos - maData);
}

size_t Scanner::Buffer::PendingCount() const
{
    POV_PARSER_ASSERT(mpEnd >= mpPos);
    return (mpEnd - mpPos);
}

bool Scanner::Buffer::IsLean() const
{
    return (TotalCount() < Capacity());
}

bool Scanner::Buffer::IsFresh() const
{
    return (ProcessedCount() == 0);
}

bool Scanner::Buffer::IsExhausted() const
{
    return (PendingCount() == 0);
}

Scanner::Octet Scanner::Buffer::CurrentPending() const
{
    return *mpPos;
}

bool Scanner::Buffer::ComparePending(const Octet* seq, size_t seqLen) const
{
    POV_PARSER_ASSERT(PendingCount() >= seqLen);
    return (std::memcmp(mpPos, seq, seqLen) == 0);
}

void Scanner::Buffer::GetBulk(Octet* dst, size_t count)
{
    POV_PARSER_ASSERT(PendingCount() >= count);
    std::memcpy(dst, mpPos, count);
    Advance(count);
}

void Scanner::Buffer::Refill(StreamPtr pStream)
{
    POV_PARSER_ASSERT(IsExhausted());
    mpPos = maData;
    mpEnd = maData + pStream->readUpTo(maData, Capacity());
}

//------------------------------------------------------------------------------

Scanner::BufferedSource::BufferedSource() :
    mpStream(nullptr),
    mBase(0),
    mExhausted(true)
{}

void Scanner::BufferedSource::SetInputStream(StreamPtr pStream)
{
    mpStream = pStream;
    mBase = mpStream->tellg();
    mBuffer.Clear();
    mExhausted = false;
    (void)RefillBuffer();
}

bool Scanner::BufferedSource::SetInputStream(StreamPtr pStream, POV_OFF_T pos)
{
    if ((mpStream == pStream) && (pos >= mBase) &&
        ((pos - mBase) < mBuffer.TotalCount()))
    {
        // Requested position is already loaded in the buffer.
        // Just advance/rewind the current buffer position accordingly.
        mBuffer.AdvanceTo(pos - mBase);
        POV_PARSER_ASSERT(!mBuffer.IsExhausted());
        mExhausted = false;
        return true;
    }
    else
    {
        // Requested position is not in the buffer at the moment.
        // Make sure we have the right stream, then refill the buffer.
        mpStream = pStream;
        mExhausted = false;
        return SeekAndRefill(pos);
    }
}

StreamPtr Scanner::BufferedSource::GetInputStream()
{
    return mpStream;
}

UCS2String Scanner::BufferedSource::GetInputStreamName() const
{
    return mpStream->Name();
}

bool Scanner::BufferedSource::SeekAndRefill(POV_OFF_T pos)
{
    mBuffer.Clear();
    bool seekOk = mpStream->seekg(pos);
    mBase = mpStream->tellg();
    POV_PARSER_ASSERT((mBase == pos) || !seekOk);
    if (!seekOk)
        return false; // TODO - Maybe report issue via an exception.
    mExhausted = false;
    (void)RefillBuffer();
    return true;
}

bool Scanner::BufferedSource::Advance()
{
    POV_PARSER_ASSERT(!IsExhausted());
    POV_PARSER_ASSERT(!mBuffer.IsExhausted());

    // Advance buffer.
    mBuffer.Advance();
    if (!mBuffer.IsExhausted())
        // Buffer not exhausted yet; ready for next octet.
        return true;

    // Buffer exhausted.

    // Quick-check whether the stream may still have pending octets.
    if (mBuffer.IsLean())
    {
        // Buffer only partially filled, implying exhausted stream.
        mExhausted = true;
        return false;
    }
    if (!IsValid())
        // No stream to refill from.
        return false; // TODO - Maybe report issue via an exception.

    // Refill buffer from stream.
    return RefillBuffer();
}

bool Scanner::BufferedSource::Advance(size_t delta)
{
    POV_PARSER_ASSERT(!IsExhausted());
    if (delta < mBuffer.PendingCount())
    {
        // Requested position is already loaded in the buffer.
        // Just advance the current buffer position accordingly.
        mBuffer.Advance(delta);
        POV_PARSER_ASSERT(!mBuffer.IsExhausted());
        return true;
    }
    else
    {
        // Requested position is not in the buffer at the moment.
        // Advance the buffer window and refill the buffer.
        (void)SeekAndRefill(mBase + mBuffer.ProcessedCount() + delta);
        return !IsExhausted();
    }
}

bool Scanner::BufferedSource::IsValid() const
{
    return (mpStream != nullptr);
}

bool Scanner::BufferedSource::IsFresh() const
{
    return (mBase == 0) && mBuffer.IsFresh();
}

bool Scanner::BufferedSource::IsExhausted() const
{
    return mExhausted;
}

Scanner::Octet Scanner::BufferedSource::CurrentPending() const
{
    return mBuffer.CurrentPending();
}

bool Scanner::BufferedSource::CompareSignature(const Octet* seq, size_t seqLen) const
{
    POV_PARSER_ASSERT(IsFresh());
    POV_PARSER_ASSERT(seqLen <= mBuffer.Capacity());
    if (mBuffer.PendingCount() < seqLen)
        return false;
    return mBuffer.ComparePending(seq, seqLen);

}

bool Scanner::BufferedSource::GetRaw(unsigned char* buffer, size_t size)
{
    POV_PARSER_ASSERT(!IsExhausted());

    unsigned char* pBufPos = buffer;
    size_t sizeToCopy = size;
    size_t sizeInBuffer = mBuffer.PendingCount();
    while (sizeToCopy >= sizeInBuffer)
    {
        mBuffer.GetBulk(pBufPos, sizeInBuffer);
        pBufPos += sizeInBuffer;
        POV_PARSER_ASSERT(mBuffer.IsExhausted());
        if (!RefillBuffer())
            return false;
        sizeToCopy -= sizeInBuffer;
        sizeInBuffer = mBuffer.PendingCount();
    }

    if (!IsExhausted() && (sizeToCopy > 0))
    {
        mBuffer.GetBulk(pBufPos, sizeToCopy);
        pBufPos += sizeToCopy;
        sizeToCopy -= sizeToCopy;
    }

    return (sizeToCopy == 0);
}

bool Scanner::BufferedSource::RefillBuffer()
{
    POV_PARSER_ASSERT(mBuffer.IsExhausted());
    POV_PARSER_ASSERT(!IsExhausted());
    mBase += mBuffer.TotalCount();
    mBuffer.Refill(mpStream);
    mExhausted = mBuffer.IsExhausted();
    return !mExhausted;
}

//------------------------------------------------------------------------------

Scanner::Scanner() :
    mpCharacterEncoding(AutoDetectEncoding::Instance()),
    mCurrentPosition(),
    mNominalEndOfLine('\0'),
    mAllowNestedBlockComments(true)
{}

void Scanner::SetInputStream(StreamPtr stream)
{
    mSource.SetInputStream(stream);

    mpCharacterEncoding = AutoDetectEncoding::Instance();
    mCurrentPosition = LexemePosition();
    mNominalEndOfLine = '\0';
}

bool Scanner::SetInputStream(StreamPtr stream, const Bookmark& bookmark)
{
    // Set the scanner state back to when the bookmark was created.
    mpCharacterEncoding = bookmark.characterEncoding;
    mCurrentPosition = bookmark;
    mNominalEndOfLine = bookmark.nominalEndOfLine;
    mAllowNestedBlockComments = bookmark.allowNestedBlockComments;

    // Set source back to when the bookmark was created.
    return mSource.SetInputStream(stream, bookmark.offset);
}

void Scanner::SetCharacterEncoding(CharacterEncodingID encoding)
{
    switch (encoding)
    {
        case CharacterEncodingID::kAutoDetect:      mpCharacterEncoding = AutoDetectEncoding::Instance();   break;
        case CharacterEncodingID::kASCII:           mpCharacterEncoding = ASCIIEncoding::Instance();        break;
        case CharacterEncodingID::kLatin1:          mpCharacterEncoding = &kLatin1Encoding;                 break;
        case CharacterEncodingID::kMacOSRoman:      mpCharacterEncoding = &kMacOSRomanEncoding;             break;
        case CharacterEncodingID::kWindows1252:     mpCharacterEncoding = &kWindows1252Encoding;            break;
        case CharacterEncodingID::kUTF8:            mpCharacterEncoding = UTF8Encoding::Instance();         break;
        default:                                    POV_PARSER_PANIC();                                     break;
    }
}

void Scanner::SetNestedBlockComments(bool allow)
{
    mAllowNestedBlockComments = allow;
}

//------------------------------------------------------------------------------

bool Scanner::GetNextLexeme(Lexeme& lexeme)
{
    if (!mSource.IsValid())
        return false;

    if (mSource.IsFresh())
    {
        // At the very start of the stream.
        // Check for file format signatures.

        /// @todo Clean up behaviour of signature detection vs. explicit encoding override.

        // Currently, only UTF-8 is supported.
        if (GetNextSignatureLexeme(lexeme, Lexeme::kUTF8SignatureBOM, u8"\uFEFF"))
            return true;
    }

    while (!mSource.IsExhausted())
    {
        // Skip over any whitespace (including blank lines).
        while (IsNextCharacterWhitespace())
        {
            if (!AdvanceCharacter())
                return false;
        }

        lexeme.text.clear();
        lexeme.position = mCurrentPosition;

        if (IsNextCharacterIdentifierChar1())
            return GetNextWordLexeme(lexeme);
        else if (IsNextCharacterDecimalDigit())
            return GetNextFloatLiteralLexeme(lexeme);
        else if (IsNextCharacterASCII('.'))
            return GetNextFloatLiteralOrDotLexeme(lexeme);
        else if (IsNextCharacterASCII('"'))
        {
            if (!GetNextStringLiteralLexeme(lexeme))
                throw IncompleteStringLiteralException(GetInputStreamName(), lexeme.position);
            return true;
        }
        else if (IsNextCharacterASCII('/'))
        {
            // Either division operator or start of comment.
            lexeme.category = Lexeme::kOther;
            if (!AdvanceASCII(&lexeme.text))
                return true;
            if (IsNextCharacterASCII('/'))
            {
                EatNextLineComment();
                continue; // Comments are not considered lexemes. Rinse & repeat.
            }
            else if (IsNextCharacterASCII('*'))
            {
                if (!EatNextBlockComment())
                    throw IncompleteCommentException(GetInputStreamName(), lexeme.position);
                continue; // Comments are not considered lexemes. Rinse & repeat.
            }
            return true;
        }
        else if (IsNextCharacterASCII('!','<','>'))
        {
            // Either single-character operator or comparison.
            lexeme.category = Lexeme::kOther;
            if (!AdvanceASCII(&lexeme.text))
                return true;
            if (IsNextCharacterASCII('='))
                (void)AdvanceASCII(&lexeme.text);
            return true;
        }
        else if (IsNextCharacterPrintableASCII())
        {
            // Single-character operator (or not a valid lexeme at all)
            lexeme.category = Lexeme::kOther;
            (void)AdvanceASCII(&lexeme.text);
            return true;
        }
        else
        {
            // Invalid character.
            UCS4 offendingCharacter;
            if (CharacterEncoding::IsASCII(NextCharacterASCII()))
                offendingCharacter = NextCharacterASCII();
            else
                mpCharacterEncoding->DecodeCharacter(*this, offendingCharacter);
            throw InvalidCharacterException(GetInputStreamName(), lexeme.position, offendingCharacter);
        }
    }

    return false;
}

bool Scanner::GetNextDirective(Lexeme& lexeme)
{
    if (!mSource.IsValid())
        return false;

    while (!mSource.IsExhausted())
    {
        // Skip over pretty much anything, except stuff that may
        // invalidate the lexeme we are looking for.
        while (!IsNextCharacterASCII('"','/','#'))
        {
            if (!AdvanceCharacter())
                return false;
        }

        lexeme.text.clear();
        lexeme.position = mCurrentPosition;

        if (IsNextCharacterASCII('"'))
        {
            if (!EatNextStringLiteral())
                throw IncompleteStringLiteralException(GetInputStreamName(), lexeme.position);
            continue; // Not a lexeme we are looking for. Rinse & repeat.
        }
        else if (IsNextCharacterASCII('/'))
        {
            // Either division operator or start of comment.
            if (!AdvanceASCII())
                return false;
            if (IsNextCharacterASCII('/'))
            {
                EatNextLineComment();
                continue; // Comments are not considered lexemes. Rinse & repeat.
            }
            else if (IsNextCharacterASCII('*'))
            {
                if (!EatNextBlockComment())
                    throw IncompleteCommentException(GetInputStreamName(), lexeme.position);
                continue; // Comments are not considered lexemes. Rinse & repeat.
            }
            continue; // Not a lexeme we are looking for. Rinse & repeat.
        }
        else
        {
            POV_PARSER_ASSERT(IsNextCharacterASCII('#'));
            // Found what we've been looking for.
            lexeme.category = Lexeme::kOther;
            (void)AdvanceASCII(&lexeme.text);
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

bool Scanner::GetNextWordLexeme(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    POV_PARSER_ASSERT(IsNextCharacterIdentifierChar1());

    lexeme.category = Lexeme::kWord;

    // Read identifier name.
    while (AdvanceCharacter(&lexeme.text) && IsNextCharacterIdentifierChar2())
    {}

    return true;
}

//------------------------------------------------------------------------------

bool Scanner::GetNextFloatLiteralLexeme(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());

    lexeme.category = Lexeme::kFloatLiteral;

    if (!GetNextFloatLiteralDigits(lexeme))
        POV_PARSER_PANIC();
    if (mSource.IsExhausted())
        return true;

    // Read fractional part if present.
    if (IsNextCharacterASCII('.'))
    {
        if (!AdvanceASCII(&lexeme.text))
            // No digits in fractional part, but we allow that.
            return true;
        if (!GetNextFloatLiteralDigits(lexeme))
            // No digits in fractional part, but we allow that.
            ;
        if (mSource.IsExhausted())
            return true;
    }

    // Read scientific notation exponent if present.
    (void)GetNextFloatLiteralExponent(lexeme);

    return true;
}

bool Scanner::GetNextFloatLiteralOrDotLexeme(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    POV_PARSER_ASSERT(IsNextCharacterASCII('.'));

    if (AdvanceASCII(&lexeme.text) && IsNextCharacterDecimalDigit())
    {
        // Valid start of a numeric literal, starting with the decimal point.
        lexeme.category = Lexeme::kFloatLiteral;

        // Read fractional part.
        if (!GetNextFloatLiteralDigits(lexeme))
            POV_PARSER_PANIC();
        if (mSource.IsExhausted())
            return true;

        // Read scientific notation exponent if present.
        (void)GetNextFloatLiteralExponent(lexeme);

        return true;
    }
    else
    {
        // Dot operator.
        lexeme.category = Lexeme::kOther;

        // Dot has already been copied to lexeme.

        return true;
    }
}

bool Scanner::GetNextFloatLiteralDigits(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    if (!IsNextCharacterDecimalDigit())
        return false;
    POV_PARSER_ASSERT(IsNextCharacterASCII());

    while (AdvanceASCII(&lexeme.text) && IsNextCharacterDecimalDigit())
        POV_PARSER_ASSERT(IsNextCharacterASCII());

    return true;
}

bool Scanner::GetNextFloatLiteralExponent(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    if (!IsNextCharacterASCII('E','e'))
        return false;

    // Copy exponent separator character.
    if (!AdvanceASCII(&lexeme.text))
        return true;

    // Copy sign if present.
    if (IsNextCharacterASCII('+','-'))
    {
        if (!AdvanceASCII(&lexeme.text))
            return true;
    }

    // Copy digits if present.
    if (!GetNextFloatLiteralDigits(lexeme))
        // No digits in exponent, but we allow that.
        return true;

    return true;
}

//------------------------------------------------------------------------------

bool Scanner::EatNextStringLiteral()
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    POV_PARSER_ASSERT(IsNextCharacterASCII('"'));

    if (!AdvanceASCII())
        return false;

    while (!IsNextCharacterASCII('"'))
    {
        if (IsNextCharacterASCII('\\'))
        {
            if (!AdvanceASCII())
                return false;
        }

        if (!AdvanceCharacter())
            return false;
    }

    (void)AdvanceASCII();

    return true;
}

bool Scanner::GetNextStringLiteralLexeme(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    POV_PARSER_ASSERT(IsNextCharacterASCII('"'));

    lexeme.category = Lexeme::kStringLiteral;

    if (!AdvanceASCII(&lexeme.text))
        return false;

    while (!IsNextCharacterASCII('"'))
    {
        if (IsNextCharacterASCII('\\'))
        {
            if (!AdvanceASCII(&lexeme.text))
                return false;
        }

        if (IsNextCharacterEndOfLine())
        {
            // Mimick POV-Ray v3.7 parser behaviour,
            // replacing line ending sequence with single LF
            if (IsNominalEndOfLine(NextCharacterASCII()))
                lexeme.text += '\n';
            if (!AdvanceCharacter())
                return false;
        }
        else
        {
            if (!AdvanceCharacter(&lexeme.text))
                return false;
        }
    }

    (void)AdvanceASCII(&lexeme.text);

    return true;
}

//------------------------------------------------------------------------------

void Scanner::EatNextLineComment()
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    POV_PARSER_ASSERT(IsNextCharacterASCII('/'));

    while (AdvanceCharacter() && !IsNextCharacterEndOfLine())
    {}
}

//------------------------------------------------------------------------------

bool Scanner::EatNextBlockComment()
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    POV_PARSER_ASSERT(IsNextCharacterASCII('*'));

    if (!AdvanceASCII())
        return false;
    unsigned int nestingLevel = 1;
    while (nestingLevel > 0)
    {
        if (IsNextCharacterASCII('*'))
        {
            if (!AdvanceASCII())
                return false;
            if (IsNextCharacterASCII('/'))
            {
                --nestingLevel;
                if (!AdvanceASCII() && (nestingLevel > 0))
                    return false;
            }
        }
        else if (mAllowNestedBlockComments && IsNextCharacterASCII('/'))
        {
            if (!AdvanceASCII())
                return false;
            if (IsNextCharacterASCII('*'))
            {
                ++nestingLevel;
                if (!AdvanceASCII())
                    return false;
            }
        }
        else
        {
            if (!AdvanceCharacter())
                return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

bool Scanner::GetNextSignatureLexeme(Lexeme& lexeme, Lexeme::Category sigId, const Octet* sigToTest, size_t sigLength)
{
    POV_PARSER_ASSERT(mSource.IsFresh());

    if (!mSource.CompareSignature(sigToTest, sigLength))
        return false;

    lexeme.text = UTF8String(reinterpret_cast<const char*>(sigToTest), sigLength);
    lexeme.position = mCurrentPosition;
    lexeme.category = sigId;
    mSource.Advance(sigLength);
    mCurrentPosition.offset += sigLength;
    return true;
}

bool Scanner::GetNextSignatureLexeme(Lexeme& lexeme, Lexeme::Category sigId, const char* sigToTest)
{
    return GetNextSignatureLexeme(lexeme, sigId, reinterpret_cast<const Octet*>(sigToTest), std::strlen(sigToTest));
}

//------------------------------------------------------------------------------

bool Scanner::GetRaw(unsigned char* buffer, size_t size)
{
    return mSource.GetRaw(buffer, size);
}

//------------------------------------------------------------------------------

ConstStreamPtr Scanner::GetInputStream() const
{
    return const_cast<Scanner*>(this)->mSource.GetInputStream();
}

UCS2String Scanner::GetInputStreamName() const
{
    return mSource.GetInputStreamName();
}

Scanner::HotBookmark Scanner::GetHotBookmark()
{
    return HotBookmark(mSource.GetInputStream(), mCurrentPosition, mpCharacterEncoding, mNominalEndOfLine, mAllowNestedBlockComments);
}

Scanner::ColdBookmark Scanner::GetColdBookmark() const
{
    return ColdBookmark(mSource.GetInputStreamName(), mCurrentPosition, mpCharacterEncoding, mNominalEndOfLine, mAllowNestedBlockComments);
}

bool Scanner::GoToBookmark(const Bookmark& bookmark)
{
    return SetInputStream(mSource.mpStream, bookmark);
}

bool Scanner::GoToBookmark(const HotBookmark& bookmark)
{
    return SetInputStream(bookmark.pStream, bookmark);
}

bool Scanner::GoToBookmark(const ColdBookmark& bookmark)
{
    if (bookmark.fileName != GetInputStreamName())
        return false;
    return SetInputStream(mSource.mpStream, bookmark);
}

//------------------------------------------------------------------------------

bool Scanner::AdvanceASCII(UTF8String* pLexemeText)
{
    POV_PARSER_ASSERT(IsNextCharacterASCII());
    if (pLexemeText != nullptr)
        *pLexemeText += NextCharacterASCII();
    AdvancePosition();
    return AdvanceOctet();
}

bool Scanner::AdvanceCharacter(UTF8String* pLexemeText)
{
    if (IsNextCharacterASCII())
    {
        return AdvanceASCII(pLexemeText);
    }
    else
    {
        mpCharacterEncoding->DecodeCharacter(*this, pLexemeText);
        ++mCurrentPosition.column;
        return !mSource.IsExhausted();
    }
}

bool Scanner::AdvanceOctet()
{
    if (mSource.IsExhausted())
        return false;
    ++mCurrentPosition.offset;
    return mSource.Advance();
}

void Scanner::AdvancePosition()
{
    if (mSource.IsExhausted())
        return;

    if (IsNextCharacterEndOfLine())
    {
        if (IsNominalEndOfLine(NextCharacterASCII()))
        {
            ++mCurrentPosition.line;
            mCurrentPosition.column = 1;
        }
    }
    else
        ++mCurrentPosition.column;
}

bool Scanner::IsNominalEndOfLine(Character endOfLineCharacter)
{
    if (mNominalEndOfLine == '\0')
        mNominalEndOfLine = endOfLineCharacter;
    return (endOfLineCharacter == mNominalEndOfLine);
}

//------------------------------------------------------------------------------

Scanner::Octet Scanner::NextOctet() const
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    return mSource.CurrentPending();
}

Scanner::ASCIICharacter Scanner::NextCharacterASCII() const
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    return ASCIICharacter(mSource.CurrentPending());
}

bool Scanner::IsNextCharacterASCII() const
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    return CharacterEncoding::IsASCII(mSource.CurrentPending());
}

bool Scanner::IsNextCharacterASCII(char character) const
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    return (mSource.CurrentPending() == character);
}

bool Scanner::IsNextCharacterASCII(char c1, char c2) const
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    return (mSource.CurrentPending() == c1) || (mSource.CurrentPending() == c2);
}

bool Scanner::IsNextCharacterASCII(char c1, char c2, char c3) const
{
    POV_PARSER_ASSERT(!mSource.IsExhausted());
    return (mSource.CurrentPending() == c1) || (mSource.CurrentPending() == c2) || (mSource.CurrentPending() == c3);
}

bool Scanner::IsNextCharacterEndOfLine() const
{
    // TODO - currently only supports ASCII end-of-line characters.
    return IsASCIIEndOfLine(NextCharacterASCII());
}

bool Scanner::IsNextCharacterNominalEndOfLine()
{
    // TODO - currently only supports ASCII end-of-line characters.
    return IsNominalEndOfLine(NextCharacterASCII());
}

bool Scanner::IsNextCharacterWhitespace() const
{
    // TODO - currently only supports ASCII whitespace.
    return IsASCIIWhitespace(NextCharacterASCII());
}

bool Scanner::IsNextCharacterPrintableASCII() const
{
    return IsPrintableASCII(NextCharacterASCII());
}

bool Scanner::IsNextCharacterDecimalDigit() const
{
    return IsDecimalDigit(NextCharacterASCII());
}

bool Scanner::IsNextCharacterIdentifierChar1() const
{
    // TODO - currently only supports ASCII identifier characters.
    return IsASCIIIdentifierChar1(NextCharacterASCII());
}

bool Scanner::IsNextCharacterIdentifierChar2() const
{
    // TODO - currently only supports ASCII identifier characters.
    return IsASCIIIdentifierChar2(NextCharacterASCII());
}

//------------------------------------------------------------------------------

}
// end of namespace pov_parser
