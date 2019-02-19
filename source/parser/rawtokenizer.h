//******************************************************************************
///
/// @file parser/rawtokenizer.h
///
/// Declarations for the _raw tokenizer_ stage of the parser.
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

#ifndef POVRAY_PARSER_RAWTOKENIZER_H
#define POVRAY_PARSER_RAWTOKENIZER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "parser/configparser.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <unordered_map>

// POV-Ray header files (base module)
#include "base/stringtypes.h"

// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (parser module)
#include "parser/parsertypes.h"
#include "parser/scanner.h"

namespace pov_parser
{

using namespace pov_base;

//------------------------------------------------------------------------------

/// Abstract structure representing an arbitrary literal or variable value.
struct Value
{
    virtual ~Value() {}
protected:
    Value() {}
};

using ValuePtr = std::shared_ptr<Value>;
using ConstValuePtr = std::shared_ptr<const Value>;

/// Structure representing a string value.
struct StringValue : Value
{
    UCS2String data;
    virtual const UCS2String& GetData() const { return data; }      ///< Get value in generic context.
    virtual const UCS2String& GetFileName() const { return data; }  ///< Get value in file name context.
    virtual bool IsAmbiguous() const { return false; }              ///< Whether value depends on context.
    virtual void Append(UCS2 codeUnit) { data += codeUnit; }        ///< Append an unambiguous character.
};

/// Structure representing a string value with backslashes.
struct AmbiguousStringValue final : StringValue
{
    struct InvalidEscapeSequenceInfo final
    {
        ConstStreamPtr stream;
        LexemePosition position;
        UTF8String text;
        InvalidEscapeSequenceInfo(ConstStreamPtr s, LexemePosition p, UTF8String t) : stream(s), position(p), text(t) {}
        InvalidEscapeSequenceInfo(ConstStreamPtr s, LexemePosition p, const UTF8String::const_iterator& b, const UTF8String::const_iterator& e) :
            stream(s), position(p), text(b, e) {}
        void Throw() const;
    };

    UCS2String data;
    UCS2String fileName;
    InvalidEscapeSequenceInfo* invalidEscapeSequence;
    AmbiguousStringValue(const StringValue& o) : data(o.GetData()), fileName(o.GetFileName()), invalidEscapeSequence(nullptr) {}
    AmbiguousStringValue(const AmbiguousStringValue& o) : data(o.data), fileName(o.fileName), invalidEscapeSequence(o.invalidEscapeSequence) {}
    virtual ~AmbiguousStringValue() override { if (invalidEscapeSequence != nullptr) delete invalidEscapeSequence; }
    virtual const UCS2String& GetData() const override { if (invalidEscapeSequence != nullptr) invalidEscapeSequence->Throw(); return data; }
    virtual const UCS2String& GetFileName() const override { return fileName; }
    virtual bool IsAmbiguous() const override { return true; }
    virtual void Append(UCS2 codeUnit) override { data += codeUnit; fileName += codeUnit; }
};

//------------------------------------------------------------------------------

/// Structure representing an individual raw token.
struct RawToken final
{
    /// The original lexeme from which this raw token was created.
    Lexeme lexeme;

    /// A numeric ID representing the token.
    /// For reserved words, operators etc. this is the corresponding value from
    /// @ref TokenId. For literals, this is @ref FLOAT_TOKEN or
    /// @ref STRING_LITERAL_TOKEN, respectively. For identifiers, this is
    /// a numeric value larger than @ref TOKEN_COUNT uniquely identifying the
    /// identifier word.
    int id;

    /// A numeric ID representing the "expression type" of the token.
    /// For reserved word tokens (but not operators) that may occur at the start of a numeric expression, this is
    /// @ref FLOAT_TOKEN_CATEGORY. For reserved word tokens (but not operators) that may occur at the start of a
    /// vector expression, this is @ref VECTOR_TOKEN_CATEGORY. For reserved word tokens that may
    /// occur at the start of a colour expression, this is
    /// @ref COLOUR_TOKEN_CATEGORY. For identifiers, this is @ref IDENTIFIER_TOKEN.
    /// For file signature tokens, this is @ref SIGNATURE_TOKEN_CATEGORY.
    /// For other tokens, this is the corresponding value from @ref TokenId.
    TokenId expressionId;

    /// Associated numeric value.
    /// For float literal tokens this is the parsed value of the token. For
    /// other tokens this is undefined.
    /// @note
    ///     Since numeric values are used so frequently, we reserve a place for
    ///     their values here, rather than using the @ref value field.
    DBL floatValue;

    /// Associated non-numeric value.
    /// For string literal tokens, this value is set by the _raw tokenizer_ to
    /// hold the parsed string. For identifiers, this value may be set by the
    /// _cooked tokenizer_ to carry the value of the identifier.
    ConstValuePtr value;

    /// Whether the token is a reserved word.
    /// @note
    ///     This flag is _not_ set for operators.
    bool isReservedWord : 1;

    /// Whether the token is a keyword with identifier-like properties.
    bool isPseudoIdentifier : 1;

    /// Helper function to get the token ID.
    /// For identifiers, this function returns @ref IDENTIFIER_TOKEN. For all
    /// other tokens, this function returns @ref id as a @ref TokenId value.
    TokenId GetTokenId() const;
};

//******************************************************************************

/// Class implementing the parser's _raw tokenizer_ stage.
///
/// The parser's _raw tokenizer_ stage processes individual _lexemes_ from the
/// _scanner_ stage into so-called _raw tokens_, by earmarking each lexeme
/// with a numeric ID:
///   - Word-type (candidate keyword/identifier) lexemes matching a reserved
///     word are earmarked with an ID uniquely identifying that reserved word.
///   - Word-type lexemes _not_ matching any reserved word are earmarked with
///     an ID larger than @ref TOKEN_COUNT uniquely identifying that
///     non-reserved word for later reference. A mapping function exists that
///     maps such IDs to @ref IDENTIFIER_TOKEN.
///   - Numeric literal lexemes are earmarked with @ref FLOAT_TOKEN.
///   - String literal lexemes are earmarked with @ref STRING_LITERAL_TOKEN.
///   - Other lexemes found in the list of reserved words are earmarked with an
///     ID uniquely identifying that reserved word.
///
/// In addition, literal lexemes are evaluated, converting their textual
/// representation into the corresponding internal value representation.
///
class RawTokenizer final
{
public:

    using HotBookmark = Scanner::HotBookmark;
    using ColdBookmark = Scanner::ColdBookmark;

    RawTokenizer();

    /// Set or change the input stream.
    /// @note
    ///     The input stream must already be opened.
    void SetInputStream(StreamPtr pStream);

    /// Change encoding setting.
    void SetStringEncoding(CharacterEncodingID encoding);

    /// Change the behaviour with regards to nested block comments.
    void SetNestedBlockComments(bool allow);

    /// Get the next token from the input stream.
    bool GetNextToken(RawToken& token);

    /// Advance to the next `#` token in the input stream.
    bool GetNextDirective(RawToken& token);

    /// Read raw data.
    /// @deprecated
    ///     This method is only intended as a temporary measure to implement
    ///     binary-level macro caching, to provide a reference for performance
    ///     testing of the envisioned token-level macro caching.
    bool GetRaw(unsigned char* buffer, size_t size);

    /// Get current stream for comparison.
    ConstStreamPtr GetInputStream() const;

    /// Get current stream name for comparison.
    UCS2String GetInputStreamName() const;

    /// Bookmark current stream and position for later rewinding.
    HotBookmark GetHotBookmark();

    /// Bookmark current stream position for later rewinding.
    ColdBookmark GetColdBookmark() const;

    /// Go to bookmark.
    bool GoToBookmark(const HotBookmark& bookmark);

    /// Go to bookmark.
    bool GoToBookmark(const ColdBookmark& bookmark);

private:

    struct KnownWordInfo final
    {
        int     id;
        TokenId expressionId;
        bool    isReservedWord     : 1;
        bool    isPseudoIdentifier : 1;
        KnownWordInfo();
    };

    Scanner                                         mScanner;
    std::unordered_map<UTF8String, KnownWordInfo>   mKnownWords;
    unsigned int                                    mNextIdentifierId;

    bool ProcessWordLexeme(RawToken& token);
    bool ProcessOtherLexeme(RawToken& token);
    bool ProcessFloatLiteralLexeme(RawToken& token);
    bool ProcessStringLiteralLexeme(RawToken& token);
    bool ProcessSignatureLexeme(RawToken& token);

    bool ProcessUCSEscapeDigits(UCS4& c, UTF8String::const_iterator& i, UTF8String::const_iterator& escapeSequenceEnd, unsigned int digits);
};

}
// end of namespace pov_parser

#endif // POVRAY_PARSER_RAWTOKENIZER_H
