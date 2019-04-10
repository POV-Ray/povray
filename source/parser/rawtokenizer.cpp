//******************************************************************************
///
/// @file parser/rawtokenizer.cpp
///
/// Implementation of the _tokenizer_ stage of the parser.
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
#include "parser/rawtokenizer.h"

// C++ variants of C standard header files
// C++ standard header files

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/povassert.h"
#include "base/stringutilities.h"

// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (parser module)
#include "parser/reservedwords.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

//******************************************************************************

static int HexDigitToInt(UTF8String::value_type c)
{
    if ((c >= '0') && (c <= '9')) return c - '0' + 0x00;
    if ((c >= 'A') && (c <= 'F')) return c - 'A' + 0x0A;
    if ((c >= 'a') && (c <= 'f')) return c - 'a' + 0x0A;
    return -1;
}

static bool IsUCS4ScalarValue(UCS4 c)
{
    return (c <= 0x10FFFFu) && ((c < 0xD800u) || (c > 0xDFFFu));
}

//******************************************************************************

void AmbiguousStringValue::InvalidEscapeSequenceInfo::Throw() const
{
    throw InvalidEscapeSequenceException(stream->Name(), position, text);
}

//******************************************************************************

TokenId RawToken::GetTokenId() const
{
    if (id <= TOKEN_COUNT)
        return TokenId(id);
    else
        return IDENTIFIER_TOKEN;
}

//******************************************************************************

RawTokenizer::KnownWordInfo::KnownWordInfo() :
    id(int(NOT_A_TOKEN)),
    expressionId(NOT_A_TOKEN),
    isReservedWord(false),
    isPseudoIdentifier(false)
{}

//******************************************************************************

RawTokenizer::RawTokenizer() :
    mNextIdentifierId(TOKEN_COUNT+1)
{
    for (auto i = Reserved_Words; i->Token_Name != nullptr; ++i)
    {
        if (!isalpha(i->Token_Name[0]))
            continue;
        if (strchr(i->Token_Name, ' ') != nullptr)
            continue;
        KnownWordInfo& knownWord        = mKnownWords[i->Token_Name];
        knownWord.id                    = i->Token_Number;
        knownWord.expressionId          = GetCategorizedTokenId(i->Token_Number);
        knownWord.isReservedWord        = true;
        knownWord.isPseudoIdentifier    = ((knownWord.id == GLOBAL_TOKEN) || (knownWord.id == LOCAL_TOKEN));
    }
}

void RawTokenizer::SetInputStream(StreamPtr pStream)
{
    mScanner.SetInputStream(pStream);
}

void RawTokenizer::SetStringEncoding(CharacterEncodingID encoding)
{
    mScanner.SetCharacterEncoding(encoding);
}

void pov_parser::RawTokenizer::SetNestedBlockComments(bool allow)
{
    mScanner.SetNestedBlockComments(allow);
}

//------------------------------------------------------------------------------

bool RawTokenizer::GetNextToken(RawToken& token)
{
    if (!mScanner.GetNextLexeme(token.lexeme))
        return false;

    switch (token.lexeme.category)
    {
        case Lexeme::kWord:             if (ProcessWordLexeme(token))           return true;
        case Lexeme::kFloatLiteral:     if (ProcessFloatLiteralLexeme(token))   return true;
        case Lexeme::kStringLiteral:    if (ProcessStringLiteralLexeme(token))  return true;
        case Lexeme::kOther:            if (ProcessOtherLexeme(token))          return true;
        case Lexeme::kUTF8SignatureBOM: if (ProcessSignatureLexeme(token))      return true;
        default:                        POV_PARSER_PANIC();                     return true;
    }
}

bool RawTokenizer::GetNextDirective(RawToken& token)
{
    if (!mScanner.GetNextDirective(token.lexeme))
        return false;

    POV_PARSER_ASSERT(token.lexeme.category == Lexeme::kOther);
    POV_PARSER_ASSERT(token.lexeme.text == "#");

    token.id = TokenId::HASH_TOKEN;
    token.expressionId = TokenId::HASH_TOKEN;
    token.value = nullptr;
    token.isReservedWord = false;
    token.isPseudoIdentifier = false;

    return true;
}

bool RawTokenizer::ProcessWordLexeme(RawToken& token)
{
    POV_PARSER_ASSERT(token.lexeme.category == Lexeme::kWord);
    POV_PARSER_ASSERT(token.lexeme.text.size() > 0);

    auto& i = mKnownWords[token.lexeme.text];
    if (i.id == int(NOT_A_TOKEN))
    {
        i.id = ++mNextIdentifierId;
        i.expressionId = IDENTIFIER_TOKEN;
    }
    token.id = i.id;
    token.expressionId = i.expressionId;
    token.value = nullptr;
    token.isReservedWord = i.isReservedWord;
    token.isPseudoIdentifier = i.isPseudoIdentifier;

    return true;
}

bool RawTokenizer::ProcessFloatLiteralLexeme(RawToken& token)
{
    POV_PARSER_ASSERT(token.lexeme.category == Lexeme::kFloatLiteral);

    token.id = int(FLOAT_TOKEN);
    token.expressionId = FLOAT_TOKEN_CATEGORY;

    if (sscanf(token.lexeme.text.c_str(), POV_DBL_FORMAT_STRING, &token.floatValue) == 0)
        return false;

    token.isReservedWord = false;
    token.isPseudoIdentifier = false;

    return true;
}

bool RawTokenizer::ProcessStringLiteralLexeme(RawToken& token)
{
    POV_PARSER_ASSERT(token.lexeme.category == Lexeme::kStringLiteral);
    POV_PARSER_ASSERT(token.lexeme.text.size() >= 2);
    POV_PARSER_ASSERT(token.lexeme.text.front() == '"');
    POV_PARSER_ASSERT(token.lexeme.text.back() == '"');

    token.id = int(STRING_LITERAL_TOKEN);
    token.expressionId = STRING_LITERAL_TOKEN;

    std::shared_ptr<StringValue> pValue(std::make_shared<StringValue>());
    std::shared_ptr<AmbiguousStringValue> pAmbiguousValue;
    UCS4 c;

    pValue->data.reserve(token.lexeme.text.size() - 2);
    auto payloadBegin = token.lexeme.text.cbegin() + 1;
    auto payloadEnd = token.lexeme.text.cend() - 1;
    auto i = payloadBegin;
    while (i != payloadEnd)
    {
        if (*i == '\\')
        {
            // For now, presume the escape sequence to be ambiguous.
            bool isAmbiguous = true;
            bool isInvalid = false;

            auto escapeSequenceBegin = i;
            POV_PARSER_ASSERT(payloadEnd - escapeSequenceBegin >= 2); // Bare `\` at end of string should have escaped the end-of-string quote char.
            auto escapeSequenceEnd = escapeSequenceBegin + 2; // Typical length of escape sequence.

            ++i;
            switch (*i)
            {
                case '\"':
                    isAmbiguous = false;
                    // FALLTHROUGH
                case '\'':
                case '\\':
                    c = UCS4(*i);
                    ++i;
                    break;

                case 'a': c = 0x0007u; ++i; break;   // "Alert"         = BEL
                case 'b': c = 0x0008u; ++i; break;   // "Backspace"     = BS
                case 't': c = 0x0009u; ++i; break;   // "Tab"           = HT
                case 'n': c = 0x000Au; ++i; break;   // "New line"      = LF
                case 'v': c = 0x000Bu; ++i; break;   // "Vertical tab"  = VT
                case 'f': c = 0x000Cu; ++i; break;   // "Form feed"     = FF
                case 'r': c = 0x000Du; ++i; break;   // "Return"        = CR

                case 'u':
                    ++i;
                    escapeSequenceEnd = payloadEnd;
                    if (!ProcessUCSEscapeDigits(c, i, escapeSequenceEnd, 4))
                        isInvalid = true;
                    /// @todo Do we want to add support for surrogate pairs?
                    break;

                case 'U':
                    ++i;
                    escapeSequenceEnd = payloadEnd;
                    if (!ProcessUCSEscapeDigits(c, i, escapeSequenceEnd, 6))
                        isInvalid = true;
                    /// @todo Do we want to add support for surrogate pairs?
                    break;

                default:
                    isInvalid = true;
                    c = UCS4(*i);
                    ++i;
                    break;
            }
            if (isAmbiguous)
            {
                if (!pAmbiguousValue)
                {
                    // String has been unambiguous -- until now.
                    pAmbiguousValue = std::make_shared<AmbiguousStringValue>(*pValue);
                    pValue = pAmbiguousValue;
                }

                /// @todo Add support for non-BMP characters (requires UTF16String instead of UCS2String).
                pAmbiguousValue->data += UCS2(c);

                for (auto iEscapeChar = escapeSequenceBegin; iEscapeChar != escapeSequenceEnd; ++iEscapeChar)
                {
                    /// @todo Add support for non-BMP characters (requires UTF16String instead of UCS2String).
                    pAmbiguousValue->fileName += UCS2(*iEscapeChar);
                }

                if (isInvalid && (pAmbiguousValue->invalidEscapeSequence == nullptr))
                    pAmbiguousValue->invalidEscapeSequence = new AmbiguousStringValue::InvalidEscapeSequenceInfo(
                        mScanner.GetInputStream(), token.lexeme.position, escapeSequenceBegin, escapeSequenceEnd);
            }
            else
            {
                pValue->Append(UCS2(c));
            }
        }
        else if (Octet(*i) <= 0x7F)
        {
            pValue->Append(UCS2(*i));
            ++i;
        }
        else
        {
            if (!pov_base::UCS::DecodeUTF8Sequence(c, i, payloadEnd))
                c = pov_base::UCS::kReplacementCharacter;

            /// @todo Add support for non-BMP characters (requires UTF16String instead of UCS2String).
            pValue->Append(UCS2(c));
        }
    }

    token.value = pValue;
    token.isReservedWord = false;
    token.isPseudoIdentifier = false;

    return true;
}

bool RawTokenizer::ProcessUCSEscapeDigits(UCS4& c, UTF8String::const_iterator& i, UTF8String::const_iterator& escapeSequenceEnd, unsigned int digits)
{
    POV_PARSER_ASSERT(digits <= 8);

    if ((escapeSequenceEnd - i) < digits)
        return false;
    escapeSequenceEnd = i + digits;

    c = 0x0000u;
    while (i != escapeSequenceEnd)
    {
        int hexDigit = HexDigitToInt(*i);
        if (hexDigit < 0)
            return false;
        c = (c << 4) + hexDigit;
        ++i;
    }

    /// @todo Do we want to add support for surrogate pairs?
    return IsUCS4ScalarValue(c);
}

bool RawTokenizer::ProcessOtherLexeme(RawToken& token)
{
    POV_PARSER_ASSERT(token.lexeme.category == Lexeme::kOther);
    POV_PARSER_ASSERT(token.lexeme.text.size() > 0);

    TokenId tokenId = NOT_A_TOKEN;

    if (token.lexeme.text.size() == 1)
    {
        switch (token.lexeme.text[0])
        {
            // 0x00 through 0x1F should have been interpreted as control characters or rejected as non-printable.
            // ' ' should have been interpreted as whitespace.
            case '!':   tokenId = EXCLAMATION_TOKEN;    break;
            // '"' should have been interpreted as start of string literal.
            case '#':   tokenId = HASH_TOKEN;           break;
            case '$':   tokenId = DOLLAR_TOKEN;         break;
            case '%':   tokenId = PERCENT_TOKEN;        break;
            case '&':   tokenId = AMPERSAND_TOKEN;      break;
            case '\'':  tokenId = SINGLE_QUOTE_TOKEN;   break;
            case '(':   tokenId = LEFT_PAREN_TOKEN;     break;
            case ')':   tokenId = RIGHT_PAREN_TOKEN;    break;
            case '*':   tokenId = STAR_TOKEN;           break;
            case '+':   tokenId = PLUS_TOKEN;           break;
            case ',':   tokenId = COMMA_TOKEN;          break;
            case '-':   tokenId = DASH_TOKEN;           break;
            case '.':   tokenId = PERIOD_TOKEN;         break;
            case '/':   tokenId = SLASH_TOKEN;          break;
            // '0' through '9' should have been interpreted as (start of or minimal) float literal.
            case ':':   tokenId = COLON_TOKEN;          break;
            case ';':   tokenId = SEMI_COLON_TOKEN;     break;
            case '<':   tokenId = LEFT_ANGLE_TOKEN;     break;
            case '=':   tokenId = EQUALS_TOKEN;         break;
            case '>':   tokenId = RIGHT_ANGLE_TOKEN;    break;
            case '?':   tokenId = QUESTION_TOKEN;       break;
            case '@':   tokenId = AT_TOKEN;             break;
            // 'A' through 'Z' should have been interpreted as (start of or minimal) word literal.
            case '[':   tokenId = LEFT_SQUARE_TOKEN;    break;
            case '\\':  tokenId = BACK_SLASH_TOKEN;     break;
            case ']':   tokenId = RIGHT_SQUARE_TOKEN;   break;
            case '^':   tokenId = HAT_TOKEN;            break;
            // '_' should have been interpreted as (start of or minimal) word literal.
            case '`':   tokenId = BACK_QUOTE_TOKEN;     break;
            // 'a' through 'z' should have been interpreted as (start of or minimal) word literal.
            case '{':   tokenId = LEFT_CURLY_TOKEN;     break;
            case '|':   tokenId = BAR_TOKEN;            break;
            case '}':   tokenId = RIGHT_CURLY_TOKEN;    break;
            case '~':   tokenId = TILDE_TOKEN;          break;
            // 0x7F should have been rejected as non-printable.
            // 0x80 through 0xFF should have been rejected as non-ASCII.
            default:    POV_PARSER_PANIC();             break;
        }
    }
    else if (token.lexeme.text == "!=")
        tokenId = REL_NE_TOKEN;
    else if (token.lexeme.text == "<=")
        tokenId = REL_LE_TOKEN;
    else if (token.lexeme.text == ">=")
        tokenId = REL_GE_TOKEN;
    else
        POV_PARSER_PANIC(); // Should not have been produced by scanner.

    token.id                    = int(tokenId);
    token.expressionId          = tokenId;
    token.value                 = nullptr;
    token.isReservedWord        = false;
    token.isPseudoIdentifier    = false;

    POV_EXPERIMENTAL_ASSERT(GetCategorizedTokenId(tokenId) == tokenId);

    return true;
}

bool RawTokenizer::ProcessSignatureLexeme(RawToken& token)
{
    POV_PARSER_ASSERT(token.lexeme.text.size() > 0);

    TokenId tokenId = NOT_A_TOKEN;

    switch (token.lexeme.category)
    {
        case Lexeme::kUTF8SignatureBOM: tokenId = UTF8_SIGNATURE_TOKEN; break;
        default:                        POV_PARSER_PANIC();             break;
    }

    token.id                    = int(tokenId);
    token.expressionId          = SIGNATURE_TOKEN_CATEGORY;
    token.value                 = nullptr;
    token.isReservedWord        = false;
    token.isPseudoIdentifier    = false;

    return true;
}

//------------------------------------------------------------------------------

bool RawTokenizer::GetRaw(unsigned char* buffer, size_t size)
{
    return mScanner.GetRaw(buffer, size);
}

//------------------------------------------------------------------------------

pov_parser::ConstStreamPtr RawTokenizer::GetInputStream() const
{
    return mScanner.GetInputStream();
}

pov_base::UCS2String RawTokenizer::GetInputStreamName() const
{
    return mScanner.GetInputStreamName();
}

pov_parser::RawTokenizer::HotBookmark RawTokenizer::GetHotBookmark()
{
    return mScanner.GetHotBookmark();
}

pov_parser::RawTokenizer::ColdBookmark RawTokenizer::GetColdBookmark() const
{
    return mScanner.GetColdBookmark();
}

bool RawTokenizer::GoToBookmark(const HotBookmark& bookmark)
{
    return mScanner.GoToBookmark(bookmark);
}

bool RawTokenizer::GoToBookmark(const ColdBookmark& bookmark)
{
    return mScanner.GoToBookmark(bookmark);
}

}
// end of namespace pov_parser
