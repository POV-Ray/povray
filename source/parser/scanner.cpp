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
#include "parser/scanner.h"

// C++ variants of C standard header files
#include <cstdio>
#include <cstdlib>

// C++ standard header files
#include <limits>

// Boost header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/textstream.h"

// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

constexpr unsigned int  kOctetBits = 8;                         ///< Number of bits in an octet.
constexpr unsigned int  kOctetValues = 0x01u << kOctetBits;     ///< Number of different values an octet can represent.
constexpr unsigned char kOctetMask = Octet(kOctetValues - 1);

static auto kUTF8Replacement = u8"\uFFFD";

//******************************************************************************

struct SourceEncoding
{
    using OctetSequenceLength = unsigned int;
    using SourceEncodingPtr = Scanner::SourceEncodingPtr;

    static constexpr UCS4 kInvalid = 0x1FFFFFu;     ///< Returned by @ref Decode() if encoding is invalid.

    virtual ~SourceEncoding() {}

    static bool IsASCII(Octet codeUnit)
    {
        return ((codeUnit & Octet(0x80)) == Octet(0x00));
    }

    virtual void DecodeCharacter(Scanner& scanner, Lexeme& lexeme) = 0;

protected:

    static Octet GetNextOctet(Scanner& scanner)
    {
        return *scanner.mpNextChar;
    }

    static bool Advance(Scanner& scanner)
    {
        return scanner.Advance();
    }

    static void Write(Lexeme& lexeme, Octet utf8CodeUnit)
    {
        lexeme.text += utf8CodeUnit;
    }

    static void Write(Lexeme& lexeme, const char* utf8CodeUnits)
    {
        lexeme.text += utf8CodeUnits;
    }

    SourceEncoding() = default;
};

//------------------------------------------------------------------------------

struct ASCIISourceEncoding : SourceEncoding
{
    virtual void DecodeCharacter(Scanner& scanner, Lexeme& lexeme) override
    {
        POV_PARSER_ASSERT(!IsASCII(GetNextOctet(scanner)));
        Write(lexeme, kUTF8Replacement);
        Advance(scanner);
    }

    static SourceEncodingPtr Instance()
    {
        static SourceEncodingPtr instance = SourceEncodingPtr(new ASCIISourceEncoding);
        return instance;
    }

protected:

    ASCIISourceEncoding() = default;
};

//------------------------------------------------------------------------------

template<const char* TABLE[kOctetValues]>
struct Generic8BitSourceEncoding : SourceEncoding
{
    virtual void DecodeCharacter(Scanner& scanner, Lexeme& lexeme) override
    {
        POV_PARSER_ASSERT(!IsASCII(GetNextOctet(scanner)));
        Write(lexeme, TABLE[GetNextOctet(scanner)]);
        Advance(scanner);
    }

    static SourceEncodingPtr Instance()
    {
        static SourceEncodingPtr instance = SourceEncodingPtr(new Generic8BitSourceEncoding);
        return instance;
    }

protected:

    Generic8BitSourceEncoding() = default;
};

static auto kNA = kUTF8Replacement;

//------------------------------------------------------------------------------

const char* kaWindows1252DecodingTable[kOctetValues] {

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

    // 0x80-0x9F: Windows 1252 specific.
    u8"\u20AC", kNA,        u8"\u201A", u8"\u0192", u8"\u201E", u8"\u2026", u8"\u2020", u8"\u2021",
    u8"\u02C6", u8"\u2030", u8"\u0160", u8"\u2039", u8"\u0152", kNA,        u8"\u017D", kNA,
    kNA,        u8"\u2018", u8"\u2019", u8"\u201C", u8"\u201D", u8"\u2022", u8"\u2013", u8"\u2014",
    u8"\u02DC", u8"\u2122", u8"\u0161", u8"\u203A", u8"\u0153", kNA,        u8"\u017E", u8"\u0178",

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

const char* kaLatin1DecodingTable[kOctetValues] {

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

using Windows1252Encoding   = Generic8BitSourceEncoding<kaWindows1252DecodingTable>;
using Latin1SourceEncoding  = Generic8BitSourceEncoding<kaLatin1DecodingTable>;

//------------------------------------------------------------------------------

/// Test whether character qualifies as end-of-line marker.
static bool IsEndOfLine(Scanner::Character c)
{
    return ((c == 0x0A) ||  // ASCII LF ("New Line")
            (c == 0x0D));   // ASCII CR ("Carriage Return")
}

/// Test whether character qualifies as whitespace.
static bool IsWhitespace(Scanner::Character c)
{
    return ((c == 0x09) ||  // ASCII HT ("Tab")
            (c == 0x0A) ||  // ASCII LF ("New Line")
            (c == 0x0D) ||  // ASCII CR ("Carriage Return")
            (c == 0x1A) ||  // DOS EOF character
            (c == 0x20));   // ASCII SP ("Space")
}

/// Test whether character qualifies as printable.
static bool IsPrintable(Scanner::Character character)
{
    // TODO - implement non-ASCII character encodings
    return ((character >= 0x20) && (character <= 0x7E));
}

/// Test whether character qualifies as decimal digit.
static bool IsDecimalDigit(Scanner::Character character)
{
    return ((character >= '0') && (character <= '9'));
}

/// Test whether character qualifies as exponent separator.
static bool IsExponentSeparator(Scanner::Character character)
{
    return ((character == 'e') ||
            (character == 'E'));
}

/// Test whether character qualifies as exponent separator.
static bool IsSign(Scanner::Character character)
{
    return ((character == '-') ||
            (character == '+'));
}

/// Test whether character qualifies as first identifier character.
static bool IsIdentifierChar1(Scanner::Character character)
{
    return (((character >= 'a') && (character <= 'z')) ||
            ((character >= 'A') && (character <= 'Z')) ||
             (character == '_'));
}

/// Test whether character qualifies as subsequent identifier character.
static bool IsIdentifierChar2(Scanner::Character character)
{
    return IsIdentifierChar1(character) ||
           IsDecimalDigit(character);
}

//******************************************************************************

Scanner::Scanner() :
    mpSource(nullptr),
    mBase(0),
    mpBufferEnd(maBuffer),
    mpNextChar(maBuffer),
    mStringEncoding(kStringEncoding_ASCII),
    mpStringEncodingImpl(ASCIISourceEncoding::Instance()),
    mCurrentPosition(),
    mNominalEndOfLine('\0'),
    mEndOfStream(true),
    mAllowNestedBlockComments(true)
{}

void Scanner::SetInputStream(SourcePtr stream)
{
    mpSource = stream;

    mBase = mpSource->tellg();
    mpBufferEnd = maBuffer;
    mpNextChar = maBuffer;
    RefillBuffer();

    mCurrentPosition = LexemePosition();
    mEndOfStream = (mpNextChar >= mpBufferEnd);
    mNominalEndOfLine = '\0';
}

bool Scanner::SetInputStream(SourcePtr stream, const Bookmark& bookmark)
{
    mpSource = stream;

    mEndOfStream = !mpSource->seekg(bookmark.offset);
    mBase = mpSource->tellg();
    mpBufferEnd = maBuffer;
    mpNextChar = maBuffer;
    if (!mEndOfStream)
        RefillBuffer();

    SetStringEncoding(bookmark.stringEncoding);

    mCurrentPosition = bookmark;
    mEndOfStream = (mpNextChar >= mpBufferEnd);
    mNominalEndOfLine = bookmark.nominalEndOfLine;
    mAllowNestedBlockComments = bookmark.allowNestedBlockComments;

    return !mEndOfStream;
}

void pov_parser::Scanner::SetStringEncoding(StringEncoding encoding)
{
    mStringEncoding = encoding;
    switch (mStringEncoding)
    {
        case kStringEncoding_ASCII:         mpStringEncodingImpl = ASCIISourceEncoding::Instance(); break;
        case kStringEncoding_UTF8:          mpStringEncodingImpl = nullptr;                         break;
        case kStringEncoding_System:        mpStringEncodingImpl = Windows1252Encoding::Instance(); break; /// @todo Fix system-specific encoding.
        default: POV_PARSER_ASSERT(false);  mpStringEncodingImpl = ASCIISourceEncoding::Instance(); break;
    }
}

void Scanner::SetNestedBlockComments(bool allow)
{
    mAllowNestedBlockComments = allow;
}

//------------------------------------------------------------------------------

bool Scanner::GetNextLexeme(Lexeme& lexeme)
{
    if (mpSource == nullptr)
        return false;

    while (!mEndOfStream)
    {
        // Skip over any whitespace (including blank lines).
        while (IsWhitespace(*mpNextChar))
        {
            if (!Advance())
                return false;
        }

        lexeme.text.clear();
        lexeme.position = mCurrentPosition;

        if (IsIdentifierChar1(*mpNextChar))
            return GetNextWordLexeme(lexeme);
        else if (IsDecimalDigit(*mpNextChar))
            return GetNextFloatLiteralLexeme(lexeme);
        else if (*mpNextChar == '.')
            return GetNextFloatLiteralOrDotLexeme(lexeme);
        else if (*mpNextChar == '"')
        {
            if (!GetNextStringLiteralLexeme(lexeme))
                throw IncompleteStringLiteralException(mpSource, lexeme.position);
            return true;
        }
        else if (*mpNextChar == '/')
        {
            // Either division operator or start of comment.
            lexeme.category = Lexeme::Category::kOther;
            if (!CopyAndAdvance(lexeme))
                return true;
            if (*mpNextChar == '/')
            {
                EatNextLineComment();
                continue; // Comments are not considered lexemes. Rinse & repeat.
            }
            else if (*mpNextChar == '*')
            {
                if (!EatNextBlockComment())
                    throw IncompleteCommentException(mpSource, lexeme.position);
                continue; // Comments are not considered lexemes. Rinse & repeat.
            }
            return true;
        }
        else if ((*mpNextChar == '!') || (*mpNextChar == '<') || (*mpNextChar == '>'))
        {
            // Either single-character operator or comparison.
            lexeme.category = Lexeme::Category::kOther;
            if (!CopyAndAdvance(lexeme))
                return true;
            if (*mpNextChar == '=')
                (void)CopyAndAdvance(lexeme);
            return true;
        }
        else if (!IsPrintable(*mpNextChar))
        {
            // Invalid character.
            throw InvalidCharacterException(mpSource, lexeme.position, *mpNextChar);
        }
        else
        {
            // Single-character operator (or not a valid lexeme at all)
            lexeme.category = Lexeme::Category::kOther;
            (void)CopyAndAdvance(lexeme);
            return true;
        }
    }

    return false;
}

bool Scanner::GetNextDirective(Lexeme& lexeme)
{
    if (mpSource == nullptr)
        return false;

    while (!mEndOfStream)
    {
        // Skip over any whitespace (including blank lines).
        while (IsWhitespace(*mpNextChar))
        {
            if (!Advance())
                return false;
        }

        lexeme.text.clear();
        lexeme.position = mCurrentPosition;

        if (*mpNextChar == '"')
        {
            if (!EatNextStringLiteral())
                throw IncompleteStringLiteralException(mpSource, lexeme.position);
            continue; // Not a lexeme we are looking for. Rinse & repeat.
        }
        else if (*mpNextChar == '/')
        {
            // Either division operator or start of comment.
            if (!CopyAndAdvance(lexeme))
                return false;
            if (*mpNextChar == '/')
            {
                EatNextLineComment();
                continue; // Comments are not considered lexemes. Rinse & repeat.
            }
            else if (*mpNextChar == '*')
            {
                if (!EatNextBlockComment())
                    throw IncompleteCommentException(mpSource, lexeme.position);
                continue; // Comments are not considered lexemes. Rinse & repeat.
            }
            continue; // Not a lexeme we are looking for. Rinse & repeat.
        }
        else if (*mpNextChar == '#')
        {
            // Found what we've been looking for.
            lexeme.category = Lexeme::Category::kOther;
            (void)CopyAndAdvance(lexeme);
            return true;
        }
        else
        {
            if (!Advance())
                return false;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

bool Scanner::GetNextWordLexeme(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mEndOfStream);
    POV_PARSER_ASSERT(IsIdentifierChar1(*mpNextChar));

    lexeme.category = Lexeme::Category::kWord;

    // Read identifier name.
    while (CopyAndAdvance(lexeme) && IsIdentifierChar2(*mpNextChar))
    {}

    return true;
}

//------------------------------------------------------------------------------

bool Scanner::GetNextFloatLiteralLexeme(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mEndOfStream);

    lexeme.category = Lexeme::Category::kFloatLiteral;

    if (!GetNextFloatLiteralDigits(lexeme))
        POV_PARSER_ASSERT(false);
    if (mEndOfStream)
        return true;

    // Read fractional part if present.
    if (*mpNextChar == '.')
    {
        if (!CopyAndAdvance(lexeme))
            // No digits in fractional part, but we allow that.
            return true;
        if (!GetNextFloatLiteralDigits(lexeme))
            // No digits in fractional part, but we allow that.
            ;
        if (mEndOfStream)
            return true;
    }

    // Read scientific notation exponent if present.
    (void)GetNextFloatLiteralExponent(lexeme);

    return true;
}

bool Scanner::GetNextFloatLiteralOrDotLexeme(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mEndOfStream);

    if (CopyAndAdvance(lexeme) && IsDecimalDigit(*mpNextChar))
    {
        // Valid start of a numeric literal, starting with the decimal point.
        lexeme.category = Lexeme::Category::kFloatLiteral;

        // Read fractional part.
        if (!GetNextFloatLiteralDigits(lexeme))
            POV_PARSER_ASSERT(false);
        if (mEndOfStream)
            return true;

        // Read scientific notation exponent if present.
        (void)GetNextFloatLiteralExponent(lexeme);

        return true;
    }
    else
    {
        // Dot operator.
        lexeme.category = Lexeme::Category::kOther;

        // Dot has already been copied to lexeme.

        return true;
    }
}

bool Scanner::GetNextFloatLiteralDigits(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mEndOfStream);
    if (!IsDecimalDigit(*mpNextChar))
        return false;

    lexeme.text += *mpNextChar;
    while (Advance() && IsDecimalDigit(*mpNextChar))
        lexeme.text += *mpNextChar;

    return true;
}

bool Scanner::GetNextFloatLiteralExponent(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mEndOfStream);
    if (!IsExponentSeparator(*mpNextChar))
        return false;

    // Copy exponent separator character.
    if (!CopyAndAdvance(lexeme))
        return true;

    // Copy sign if present.
    if (IsSign(*mpNextChar) && !CopyAndAdvance(lexeme))
        return true;

    // Copy digits if present.
    if (!GetNextFloatLiteralDigits(lexeme))
        // No digits in exponent, but we allow that.
        return true;

    return true;
}

//------------------------------------------------------------------------------

bool Scanner::EatNextStringLiteral()
{
    POV_PARSER_ASSERT(!mEndOfStream);
    POV_PARSER_ASSERT(*mpNextChar == '"');

    if (!Advance())
        return false;

    while (*mpNextChar != '"')
    {
        if (*mpNextChar == '\\')
        {
            if (!Advance())
                return false;
        }
        if (!Advance())
            return false;
    }

    (void)Advance();

    return true;
}

bool Scanner::GetNextStringLiteralLexeme(Lexeme& lexeme)
{
    POV_PARSER_ASSERT(!mEndOfStream);
    POV_PARSER_ASSERT(*mpNextChar == '"');

    lexeme.category = Lexeme::Category::kStringLiteral;

    if (!CopyAndAdvance(lexeme))
        return false;

    while (*mpNextChar != '"')
    {
        if (*mpNextChar == '\\')
        {
            if (!CopyAndAdvance(lexeme))
                return false;
        }

        if ((mpStringEncodingImpl == nullptr) || SourceEncoding::IsASCII(*mpNextChar))
        {
            if (!CopyAndAdvance(lexeme))
                return false;
        }
        else
        {
            mpStringEncodingImpl->DecodeCharacter(*this, lexeme);
            if (mEndOfStream)
                return false;
        }

        /// @todo Add support for non-UTF8 character encodings.
    }

    (void)CopyAndAdvance(lexeme);

    return true;
}

//------------------------------------------------------------------------------

void Scanner::EatNextLineComment()
{
    POV_PARSER_ASSERT(!mEndOfStream);
    POV_PARSER_ASSERT(*mpNextChar == '/');

    while (Advance() && !IsEndOfLine(*mpNextChar))
    {}
}

//------------------------------------------------------------------------------

bool Scanner::EatNextBlockComment()
{
    POV_PARSER_ASSERT(!mEndOfStream);
    POV_PARSER_ASSERT(*mpNextChar == '*');

    // block ("C style") comment
    if (!Advance())
        return false;
    unsigned int nestingLevel = 1;
    while (nestingLevel > 0)
    {
        if (*mpNextChar == '*')
        {
            if (!Advance())
                return false;
            if (*mpNextChar == '/')
            {
                --nestingLevel;
                if (!Advance() && (nestingLevel > 0))
                    return false;
            }
        }
        else if (mAllowNestedBlockComments && (*mpNextChar == '/'))
        {
            if (!Advance())
                return false;
            if (*mpNextChar == '*')
            {
                ++nestingLevel;
                if (!Advance())
                    return false;
            }
        }
        else
        {
            if (!Advance())
                return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

bool Scanner::GetRaw(unsigned char* buffer, size_t size)
{
    POV_PARSER_ASSERT(!mEndOfStream);

    unsigned char* pBufPos = buffer;
    size_t sizeToCopy = size;
    size_t sizeInBuffer = (mpBufferEnd - mpNextChar);
    while (!mEndOfStream && (sizeToCopy >= sizeInBuffer))
    {
        memcpy(pBufPos, mpNextChar, sizeInBuffer);
        pBufPos += sizeInBuffer;
        mpNextChar += sizeInBuffer;
        POV_PARSER_ASSERT(mpNextChar == mpBufferEnd);
        RefillBuffer();
        sizeToCopy -= sizeInBuffer;
        sizeInBuffer = (mpBufferEnd - mpNextChar);
    }

    if (!mEndOfStream && (sizeToCopy > 0))
    {
        POV_PARSER_ASSERT(sizeToCopy < sizeInBuffer);
        memcpy(pBufPos, mpNextChar, sizeToCopy);
        pBufPos += sizeToCopy;
        mpNextChar += sizeToCopy;
        sizeToCopy -= sizeToCopy;
    }

    return (sizeToCopy == 0);
}

//------------------------------------------------------------------------------

ConstSourcePtr Scanner::GetSource() const
{
    return mpSource;
}

UCS2String Scanner::GetSourceName() const
{
    return mpSource->Name();
}

Scanner::HotBookmark Scanner::GetHotBookmark() const
{
    return HotBookmark(mpSource, mCurrentPosition, mStringEncoding, mNominalEndOfLine, mAllowNestedBlockComments);
}

Scanner::ColdBookmark Scanner::GetColdBookmark() const
{
    return ColdBookmark(mpSource->Name(), mCurrentPosition, mStringEncoding, mNominalEndOfLine, mAllowNestedBlockComments);
}

bool Scanner::GoToBookmark(const Bookmark& bookmark)
{
    return SetInputStream(mpSource, bookmark);
}

bool Scanner::GoToBookmark(const HotBookmark& bookmark)
{
    return SetInputStream(bookmark.pSource, bookmark);
}

bool Scanner::GoToBookmark(const ColdBookmark& bookmark)
{
    if (bookmark.sourceName != GetSourceName())
        return false;
    return SetInputStream(mpSource, bookmark);
}

//------------------------------------------------------------------------------

bool Scanner::CopyAndAdvance(Lexeme& lexeme)
{
    lexeme.text += *mpNextChar;
    return Advance();
}

bool Scanner::Advance()
{
    if (mEndOfStream)
        return false;

    if (IsEndOfLine(*mpNextChar))
    {
        if (mNominalEndOfLine == '\0')
            mNominalEndOfLine = *mpNextChar;
        if (*mpNextChar == mNominalEndOfLine)
        {
            ++mCurrentPosition.line;
            mCurrentPosition.column = 1;
        }
    }
    else
        ++mCurrentPosition.column;
    ++mCurrentPosition.offset;

    ++mpNextChar;
    mEndOfStream = !GuaranteeBuffer();
    return !mEndOfStream;
}

void Scanner::RefillBuffer()
{
    POV_PARSER_ASSERT(mpNextChar == mpBufferEnd);
    mBase += (mpBufferEnd - maBuffer);
    mpNextChar = maBuffer;
    mpBufferEnd = maBuffer + mpSource->readUpTo(maBuffer, kBufferSize);
}
bool Scanner::GuaranteeBuffer()
{
    if (mpNextChar < mpBufferEnd)
        return true;
    if (mpBufferEnd < maBuffer + kBufferSize)
        return false;
    if (mpSource == nullptr)
        return false;
    RefillBuffer();
    return (mpBufferEnd > maBuffer);
}

}
