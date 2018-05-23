//******************************************************************************
///
/// @file parser/scanner.h
///
/// Declarations for the _scanner_ stage of the parser.
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

#ifndef POVRAY_PARSER_SCANNER_H
#define POVRAY_PARSER_SCANNER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "parser/configparser.h"

// C++ variants of C standard header files
#include <cmath>

// C++ standard header files
#include <map>

// Boost header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/messenger.h"
#include "base/types.h"
#include "base/textstream.h"

// POV-Ray header files (parser module)
#include "parser/parsertypes.h"

namespace pov_parser
{

using namespace pov_base;

using Octet = unsigned char;

struct SourceEncoding;

//******************************************************************************

struct Lexeme
{
    enum Category : unsigned char
    {
        kWord,
        kFloatLiteral,
        kStringLiteral,
        kOther,
    };
    UTF8String      text;
    LexemePosition  position;
    Category        category;
};

//******************************************************************************

/// Class implementing the parser's _scanner_ stage.
///
/// This class scans the input character stream for individual _lexemes_, i.e.
/// sequences of characters from the input stream that constitute atomic
/// syntactical elements, e.g. (potential) identifiers, numeric constants etc.
///
/// @note
///     This stage of the scanner skips over comments.
///
class Scanner
{
public:

    friend struct SourceEncoding;
    using SourceEncodingPtr = std::shared_ptr<SourceEncoding>;

    using Octet             = unsigned char;
    using Character         = Octet; ///< @todo Only supports plain 8-bit encodings for now.

    /// Structure representing a rewindable position in the input stream.
    struct Bookmark : LexemePosition, MessageContext
    {
        StringEncoding  stringEncoding;
        Character       nominalEndOfLine;
        bool            allowNestedBlockComments;

        Bookmark() = default;
        Bookmark(const LexemePosition& lp, StringEncoding se, Character neol, bool anbc) :
            LexemePosition(lp), stringEncoding(se), nominalEndOfLine(neol), allowNestedBlockComments(anbc)
        {}
        virtual POV_LONG GetLine() const override { return line; };
        virtual POV_LONG GetColumn() const override { return column; }
        virtual POV_OFF_T GetOffset() const override { return offset; }
    };

    /// Structure representing an open input stream and a rewindable position.
    struct HotBookmark : Bookmark
    {
        SourcePtr           pSource;
        HotBookmark() = default;
        HotBookmark(const SourcePtr& s, const LexemePosition& lp, StringEncoding se, Character neol, bool anbc) :
            Bookmark(lp, se, neol, anbc), pSource(s)
        {}
        virtual UCS2String GetFileName() const override { return pSource->Name(); }
    };

    /// Structure representing an input stream name and a rewindable position.
    struct ColdBookmark : Bookmark
    {
        UCS2String          sourceName;
        ColdBookmark() = default;
        ColdBookmark(const UCS2String& s, const LexemePosition& lp, StringEncoding se, Character neol, bool anbc) :
            Bookmark(lp, se, neol, anbc), sourceName(s)
        {}
        virtual UCS2String GetFileName() const override { return sourceName; }
    };

    Scanner();

    /// Set or change the input stream.
    /// @note
    ///     The input stream must already be opened.
    void SetInputStream(SourcePtr source);

    /// Change encoding setting.
    void SetStringEncoding(StringEncoding encoding);

    /// Change the behaviour with regards to nested block comments.
    void SetNestedBlockComments(bool allow);

    /// Get the next lexeme from the input stream.
    bool GetNextLexeme(Lexeme& lexeme);

    /// Advance to the next `#` lexeme in the input stream.
    bool GetNextDirective(Lexeme& lexeme);

    /// Read raw data.
    /// @deprecated
    ///     This method is only intended as a temporary measure to implement
    ///     binary-level macro caching, to provide a reference for performance
    ///     testing of the envisioned token-level macro caching.
    bool GetRaw(unsigned char* buffer, size_t size);

    /// Get current source for comparison.
    ConstSourcePtr GetSource() const;

    /// Get current source name for comparison.
    UCS2String GetSourceName() const;

    /// Bookmark current stream and position for later rewinding.
    HotBookmark GetHotBookmark() const;

    /// Bookmark current stream position for later rewinding.
    ColdBookmark GetColdBookmark() const;

    /// Go to bookmark.
    bool GoToBookmark(const Bookmark& bookmark);

    /// Go to bookmark.
    bool GoToBookmark(const HotBookmark& bookmark);

    /// Go to bookmark.
    bool GoToBookmark(const ColdBookmark& bookmark);

private:

    SourcePtr           mpSource;           ///< Input stream to read from.

    static constexpr size_t kBufferSize = 64 * 1024;
    Octet               maBuffer[kBufferSize];
    POV_OFF_T           mBase;
    Octet*              mpBufferEnd;
    Octet*              mpNextChar;

    StringEncoding      mStringEncoding;
    SourceEncodingPtr   mpStringEncodingImpl;
    LexemePosition      mCurrentPosition;           ///< Position (line, column, offset) in input stream.
    Character           mNominalEndOfLine;
    bool                mEndOfStream;

    bool                mAllowNestedBlockComments;  ///< Whether block comments are allowed to nest.

    /// Change the input stream and jump to a given bookmark.
    /// @note
    ///     The input stream must already be opened, and will _not_ be closed
    ///     upon reaching the end of the stream.
    bool SetInputStream(SourcePtr source, const Bookmark& bookmark);

    bool GetNextWordLexeme(Lexeme& lexeme);

    bool GetNextFloatLiteralLexeme(Lexeme& lexeme);
    bool GetNextFloatLiteralOrDotLexeme(Lexeme& lexeme);
    bool GetNextFloatLiteralDigits(Lexeme& lexeme);
    bool GetNextFloatLiteralExponent(Lexeme& lexeme);

    bool EatNextStringLiteral();
    bool GetNextStringLiteralLexeme(Lexeme& lexeme);

    void EatNextLineComment();
    bool EatNextBlockComment();

    /// Copy character to lexeme, then advance stream.
    /// @return `true` if another character is available.
    bool CopyAndAdvance(Lexeme& lexeme);

    /// Discard character and advance stream.
    /// @return `true` if another character is available.
    bool Advance();

    void RefillBuffer();

    bool GuaranteeBuffer();

    using RawPosition = POV_OFF_T;

    using RawStream = pov_base::IStream;
    using RawStreampPtr = std::shared_ptr<RawStream>;
};

}

#endif // POVRAY_PARSER_SCANNER_H
