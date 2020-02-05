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

#ifndef POVRAY_PARSER_SCANNER_H
#define POVRAY_PARSER_SCANNER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "parser/configparser.h"

// C++ variants of C standard header files
#include <cmath>

// C++ standard header files
#include <initializer_list>
#include <memory>

// POV-Ray header files (base module)
#include "base/messenger_fwd.h"
#include "base/stringtypes.h"
#include "base/textstream_fwd.h"

// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (parser module)
#include "parser/parsertypes.h"

namespace pov_parser
{

using namespace pov_base;

using Octet = unsigned char;

struct CharacterEncoding;

//******************************************************************************

struct Lexeme final
{
    enum Category : unsigned char
    {
        kWord,
        kFloatLiteral,
        kStringLiteral,
        kOther,
        kUTF8SignatureBOM,
    };
    UTF8String      text;
    LexemePosition  position;
    Category        category;
};

//******************************************************************************

/// Class implementing the parser's _scanner_ stage.
///
/// This class _scans_ the input character stream for individual _lexemes_, i.e.
/// sequences of characters from the input stream that constitute atomic
/// syntactical elements, e.g. (potential) identifiers, numeric constants etc.
///
/// While the main purpose of this parser stage is to identify the boundaries
/// of the lexemes, the following tasks are also performed along the way:
///   - Handle input stream character encoding; this includes auto-detecting
///     the encoding scheme (unless overridden by the user) and line ending
///     style, reporting any non-payload file signature (e.g. UTF-8 signature
///     BOM), and transcoding the payload to UCS (more specifically, UTF-8).
///   - Keep track of line number, column, and binary offset in the stream.
///   - Skipp over whitespace and comments.
///   - Roughly categorize lexemes, earmarking them as either words (candidate
///     keywords/identifiers), numeric literals, string literals, or other (e.g.
///     operators, braces etc.).
///
/// @note
///     To provide optimal performance for ASCII files, the current
///     implementation's internal design provides only limited support for
///     other encoding schemes. Specifically, the implementation imposes the
///     following requirements on supported encoding schemes:
///       - Characters (both printable and otherwise) from the ASCII set must
///         always be encoded as a single octet in the range 0x00 through 0x7F
///         as per ASCII encoding.
///       - Multi-octet sequences must not start with an octet in the range 0x00
///         through 0x7F.
///       - Single octets in the range 0x00 0x7F must always encode the
///         corresponding ASCII character.
///
class Scanner final
{
public:

    friend struct CharacterEncoding;
    using CharacterEncodingPtr = const CharacterEncoding*;

    using Octet             = unsigned char;
    using Character         = UCS4;

    /// Structure representing a rewindable position in the input stream.
    struct Bookmark : LexemePosition, MessageContext
    {
        CharacterEncodingPtr    characterEncoding;
        Character               nominalEndOfLine;
        bool                    allowNestedBlockComments;

        Bookmark() = default;
        Bookmark(const LexemePosition& lp, CharacterEncodingPtr se, Character neol, bool anbc) :
            LexemePosition(lp), characterEncoding(se), nominalEndOfLine(neol), allowNestedBlockComments(anbc)
        {}
        virtual POV_LONG GetLine() const override { return line; };
        virtual POV_LONG GetColumn() const override { return column; }
        virtual POV_OFF_T GetOffset() const override { return offset; }
    };

    /// Structure representing an open input stream and a rewindable position.
    struct HotBookmark final : Bookmark
    {
        StreamPtr           pStream;
        HotBookmark() = default;
        HotBookmark(const StreamPtr& s, const LexemePosition& lp, CharacterEncodingPtr se, Character neol, bool anbc) :
            Bookmark(lp, se, neol, anbc), pStream(s)
        {}
        virtual UCS2String GetFileName() const override;
    };

    /// Structure representing an input stream name and a rewindable position.
    struct ColdBookmark final : Bookmark
    {
        UCS2String          fileName;
        ColdBookmark() = default;
        ColdBookmark(const UCS2String& s, const LexemePosition& lp, CharacterEncodingPtr se, Character neol, bool anbc) :
            Bookmark(lp, se, neol, anbc), fileName(s)
        {}
        virtual UCS2String GetFileName() const override { return fileName; }
    };

    Scanner();

    /// Set or change the input stream.
    /// @note
    ///     The input stream must already be opened, and will _not_ be closed
    ///     upon reaching the end of the stream, closing the scanner or changing
    ///     to yet another stream.
    void SetInputStream(StreamPtr pStream);

    /// Change encoding setting.
    void SetCharacterEncoding(CharacterEncodingID encoding);

    /// Change the behaviour with regards to nested block comments.
    /// This setting governs the scanner's behaviour when encountering a block
    /// comment start sequence (`/*`) inside another block comment: If allowed
    /// (default), such a sequence will start a nested block comment, and the
    /// next comment end sequence (`*/`) will only close the nested comment
    /// (matching the behaviour of POV-Ray v3.7's parser); otherwise, such a
    /// sequence will be ignored, and the next comment end sequence (`*/`) will
    /// close the outer comment (matching the behaviour of the POV-Ray v3.7 for
    /// Windows editor, as well as most C-inspired programming languages).
    void SetNestedBlockComments(bool allow);

    /// Try to retrieve the next lexeme from the input stream.
    /// @return `true` if a lexeme could be successfully retrieved,
    ///         `false` if the end of the input stream was encountered instead.
    bool GetNextLexeme(Lexeme& lexeme);

    /// Try to retrieve the next `#` lexeme from the input stream.
    /// @return `true` if a `#` lexeme could be successfully retrieved,
    ///         `false` if the end of the input stream was encountered instead.
    bool GetNextDirective(Lexeme& lexeme);

    /// Read raw data.
    /// @deprecated
    ///     This method is only intended as a temporary measure to implement
    ///     binary-level macro caching, to provide a reference for performance
    ///     testing of the envisioned token-level macro caching.
    /// @attention
    ///     This method does _not_ update position information (binary offset,
    ///     line and column counter).
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
    bool GoToBookmark(const Bookmark& bookmark);

    /// Go to bookmark.
    bool GoToBookmark(const HotBookmark& bookmark);

    /// Go to bookmark.
    bool GoToBookmark(const ColdBookmark& bookmark);

private:

    using ASCIICharacter = char;

    //------------------------------------------------------------------------------

    struct Buffer final
    {
        Buffer();

        /// Clear buffer.
        inline void Clear();

        /// Advance current position.
        /// @pre Buffer shall have enough pending octets.
        inline void Advance(size_t delta = 1);

        /// Set current position relative to start of buffer.
        /// @pre Buffer shall have enough total octets.
        inline void AdvanceTo(size_t offset);

        /// Get maximum number of octets in buffer.
        inline size_t Capacity() const;

        /// Get number of octets total in buffer.
        inline size_t TotalCount() const;

        /// Get number of octets processed in buffer.
        inline size_t ProcessedCount() const;

        /// Get number of octets pending in buffer.
        inline size_t PendingCount() const;

        /// Test whether buffer is "lean" (capacity not fully used).
        inline bool IsLean() const;

        /// Test whether buffer is "fresh" (all octets still pending).
        inline bool IsFresh() const;

        /// Test whether buffer is exhausted (no more pending octets).
        inline bool IsExhausted() const;

        /// Get current pending octet.
        /// @pre Buffer shall have at least one pending octet.
        inline Octet CurrentPending() const;

        /// Test whether next pending octets match given octet sequence.
        /// @pre Buffer shall have enough pending octets.
        inline bool ComparePending(const Octet* seq, size_t seqLen) const;

        /// Get bulk octets.
        /// @pre Buffer shall have enough pending octets.
        /// @post Current position shall be advanced.
        inline void GetBulk(Octet* dst, size_t count);

        /// Refill from stream.
        /// @pre Buffer shall be exhausted.
        inline void Refill(StreamPtr pStream);

    protected:

        static constexpr size_t kCapacity = 64 * 1024; ///< Maximum capacity.

        Octet   maData[kCapacity];  ///< Array holding the buffered data.
        Octet*  mpEnd;              ///< Pointer to end of data (first unoccupied octet).
        Octet*  mpPos;              ///< Pointer to current pending octet.
    };

    //------------------------------------------------------------------------------

    struct BufferedSource final
    {
        BufferedSource();
        void SetInputStream(StreamPtr pInputStream);
        bool SetInputStream(StreamPtr pInputStream, POV_OFF_T pos);

        /// Get current stream.
        StreamPtr GetInputStream();

        /// Get current stream name for comparison.
        UCS2String GetInputStreamName() const;

        /// Change buffer window position and refill buffer.
        /// @return `true` if file seek was successful.
        inline bool SeekAndRefill(POV_OFF_T pos);

        /// Increment current position by one octet.
        /// @pre Source shall not be exhausted.
        /// @return `true` if another octet is available.
        inline bool Advance();

        /// Increment current position by multiple octets.
        /// @return `true` if another octet is available.
        inline bool Advance(size_t delta);

        /// Test whether source is valid (has a stream assigned).
        inline bool IsValid() const;

        /// Test whether source is "fresh" (all octets still pending).
        inline bool IsFresh() const;

        /// Test whether source is exhausted (no more pending octets).
        inline bool IsExhausted() const;

        /// Get current pending octet.
        /// @pre Source shall have at least one pending octet.
        inline Octet CurrentPending() const;

        /// Test whether first octets match given octet sequence.
        ///
        /// @pre
        ///     The source shall be "fresh" (all octets still pending).
        ///
        /// @return `true` in case of a complete match.
        ///
        inline bool CompareSignature(const Octet* seq, size_t seqLen) const;

        /// Read raw data.
        /// @deprecated
        /// @return `true` if enough octet were available.
        inline bool GetRaw(unsigned char* buffer, size_t size);

        /// Fill buffer with new data from source.
        ///
        /// @pre
        ///     The buffer shall be exhausted.
        /// @post
        ///     The buffer shall hold as many pending octets as can fit in the
        ///     buffer or had been available from the source, whichever is lower.
        ///
        /// @return `true` if another octet is available.
        ///
        inline bool RefillBuffer();

    //protected:

        StreamPtr   mpStream;   ///< Input data stream to read from.

    protected:

        Buffer      mBuffer;    ///< Buffer to hold chunks of data from stream.
        POV_OFF_T   mBase;      ///< Offset of current buffer window from start of stream.
        bool        mExhausted; ///< Whether both buffer and stream have been exhausted.
    };

    //------------------------------------------------------------------------------

    BufferedSource          mSource;                    ///< Input data stream and associated buffer.

    CharacterEncodingPtr    mpCharacterEncoding;        ///< Character encoding expected in input stream.

    LexemePosition          mCurrentPosition;           ///< Position (line, column, offset) in input stream.

    Character               mNominalEndOfLine;          ///< Nominal end-of-line character.
                                                        ///< This member holds the character code of the
                                                        ///< first end-of-line class character in the stream

    bool                    mAllowNestedBlockComments;  ///< Whether block comments are allowed to nest.

    /// Change the input stream and jump to a given bookmark.
    ///
    /// @note
    ///     The input stream must already be opened, and will _not_ be closed
    ///     upon reaching the end of the stream, closing the scanner or changing
    ///     to yet another stream.
    ///
    bool SetInputStream(StreamPtr pStream, const Bookmark& bookmark);

    /// Try to retrieve the next word-type lexeme.
    ///
    /// This method recognizes lexemes having the following structure:
    ///
    ///     WORD_LEXEME ::= WORD_CHAR1 [ WORD_CHARS ]
    ///
    /// where
    ///
    ///     WORD_CHAR1  ::= ( LETTER | `_` )
    ///     WORD_CHARS  ::= ( LETTER | `_` | DIGIT ) [ WORD_CHARS ]
    ///
    /// @pre
    ///     The buffer position shall point to the first character of the
    ///     lexeme, which shall be a valid character to begin such a lexeme.
    /// @post
    ///     The buffer position shall point immediately after the lexeme.
    ///
    /// @return always `true` (for convenience)
    ///
    bool GetNextWordLexeme(Lexeme& lexeme);

    /// Try to retrieve the next numeric literal lexeme.
    ///
    /// This method recognizes lexemes having the following structure:
    ///
    ///     NUMERIC_LEXEME ::= DIGIT_SEQUENCE [ `.` [ DIGIT_SEQUENCE ] ] [ EXPONENT ]
    ///
    /// where `DIGIT_SEQUENCE` follows the rules implemented by
    /// @ref GetNextFloatLiteralDigits() and `EXPONENT` follows the rules
    /// implemented by @ref GetNextFloatLiteralExponent().
    ///
    /// @pre
    ///     The buffer position shall point to the first character of the
    ///     lexeme, which shall be a base-10 digit.
    /// @post
    ///     The buffer position shall point immediately after the lexeme.
    ///
    /// @return always `true` (for convenience)
    ///
    bool GetNextFloatLiteralLexeme(Lexeme& lexeme);

    /// Try to retrieve the next numeric literal or dot lexeme.
    ///
    /// This method recognizes lexemes having any of the following structures:
    ///
    ///     NUMERIC_LEXEME ::= `.` DIGIT_SEQUENCE [ EXPONENT ]
    ///     OTHER_LEXEME   ::= `.`
    ///
    /// where `DIGIT_SEQUENCE` follows the rules implemented by
    /// @ref GetNextFloatLiteralDigits() and `EXPONENT` follows the rules
    /// implemented by @ref GetNextFloatLiteralExponent().
    ///
    /// @pre
    ///     The buffer position shall point to the first character of the
    ///     lexeme, which shall be a dot (`.`).
    /// @post
    ///     The buffer position shall point immediately after the lexeme.
    ///
    /// @return always `true` (for convenience)
    ///
    bool GetNextFloatLiteralOrDotLexeme(Lexeme& lexeme);

    /// Try to retrieve a sequence of digits.
    ///
    /// This method recognizes sub-lexemes having the following structure:
    ///
    ///     DIGIT_SEQUENCE ::= DIGIT [ DIGIT_SEQUENCE ]
    ///
    /// @pre
    ///     The buffer shall be non-empty.
    /// @post
    ///     The buffer position shall point immediately after the sequence of
    ///     digits, or to the original position if no digits were found.
    ///
    /// @return `true` if the buffer initially pointed to a non-digit.
    ///
    bool GetNextFloatLiteralDigits(Lexeme& lexeme);

    /// Try to retrieve a floating-point exponent portion.
    ///
    /// This method recognizes sub-lexemes having the following structure:
    ///
    ///     EXPONENT ::= (`E`|`e`) [`+`|`-`] [ DIGIT_SEQUENCE ]
    ///
    /// @note
    ///     The single characters `E` or `e` do indeed qualify as valid
    ///     floating-point exponent portions.
    ///
    /// @pre
    ///     The buffer shall be non-empty.
    /// @post
    ///     The buffer position shall point immediately after the floating-point
    ///     exponent portion, or to the original position if no such valid
    ///     sequence was found.
    ///
    /// @return `true` if the buffer initially pointed to the start of a valid
    ///         floating-point exponent portion.
    ///
    bool GetNextFloatLiteralExponent(Lexeme& lexeme);

    /// Try to skip over the next string literal.
    ///
    /// This is an optimized method having the same effect as calling
    /// @ref GetNextStringLiteralLexeme(Lexeme& lexeme) and discarding the
    /// lexeme.
    ///
    bool EatNextStringLiteral();

    /// Try to retrieve the next string literal lexeme.
    ///
    /// This method recognizes lexemes having the following structure:
    ///
    ///     STRING_LEXEME  ::= `"` STRING_PAYLOAD `"`
    ///
    /// where
    ///
    ///     STRING_PAYLOAD ::= [ ( STRING_CHAR | ESCAPE_PAIR ) STRING_PAYLOAD ]
    ///     STRING_CHAR    ::= ( any character except `"` or `\` )
    ///     ESCAPE_PAIR    ::= `\` ( STRING_CHAR | `"` | `\` )
    ///
    /// @note
    ///     "any character" above may also be a non-printable character.
    ///
    /// @pre
    ///     The buffer position shall point to the first character of the
    ///     lexeme, which shall be a double quote (`"`).
    /// @post
    ///     The buffer position shall point immediately after the closing
    ///     double quote (`"`) if present, or to the end of file otherwise.
    ///
    /// @return `true` if the string literal is properly closed.
    ///
    bool GetNextStringLiteralLexeme(Lexeme& lexeme);


    /// Skip to the end of a line comment.
    ///
    /// This method skips to the end of the current line (or the end of the
    /// stream, whichever comes first).
    ///
    /// @pre
    ///     The buffer position shall point to the _second_ `/` character of a
    ///     line comment start sequence (`//`).
    /// @post
    ///     The buffer position shall point to the end of the line in which the
    ///     comment started.
    ///
    void EatNextLineComment();

    /// Skip to the end of a block comment.
    ///
    /// This method skips to the end of the current block comment.
    ///
    /// @note
    ///     The exact operation depends on whether nested block comments are
    ///     allowed or not.
    ///
    /// @pre
    ///     The buffer position shall point to the `*` character of a
    ///     block comment start sequence (`/*`).
    /// @post
    ///     The buffer position shall point immediately after the corresponding
    ///     block comment end sequence (`*/`) if present, or to the end of file
    ///     otherwise.
    ///
    /// @return `true` if the block comment is properly closed.
    ///
    bool EatNextBlockComment();

    /// Check if the start of the stream matches a particular fixed signature.
    ///
    /// This method skips to the end of the current block comment.
    ///
    /// @note
    ///     The exact operation depends on whether nested block comments are
    ///     allowed or not.
    ///
    /// @pre
    ///     Both the buffer window and buffer position shall point to the start
    ///     of the stream.
    /// @post
    ///     The buffer position shall point immediately after the signature in
    ///     case of a match, or the buffer shall remain unaffected in case of
    ///     a mismatch.
    ///
    /// @return `true` if the signature matches.
    ///
    bool GetNextSignatureLexeme(Lexeme& lexeme, Lexeme::Category sigId, const Octet* sigToTest, size_t sigLength);

    /// Check if the start of the stream matches a particular fixed signature.
    ///
    /// This method skips to the end of the current block comment.
    ///
    /// @note
    ///     The exact operation depends on whether nested block comments are
    ///     allowed or not.
    ///
    /// @pre
    ///     Both the buffer window and buffer position shall point to the start
    ///     of the stream.
    /// @post
    ///     The buffer position shall point immediately after the signature in
    ///     case of a match, or the buffer shall remain unaffected in case of
    ///     a mismatch.
    ///
    /// @return `true` if the signature matches.
    ///
    bool GetNextSignatureLexeme(Lexeme& lexeme, Lexeme::Category sigId, const char* sigToTest);

    /// Copy or discard ASCII character, advancing buffer accordingly.
    ///
    /// @pre
    ///     The buffer shall point to an ASCII character.
    ///
    /// @param[in]  pLexemeText     Pointer to string to receive the copied
    ///                             character, or `nullptr` to discard.
    /// @return                     `true` if another character is available
    ///                             after the operation.
    ///
    inline bool AdvanceASCII(UTF8String* pLexemeText = nullptr);

    /// Transcode or discard arbitrary character, advancing buffer accordingly.
    ///
    /// @pre
    ///     The buffer shall point to the first octet of the character to
    ///     decode.
    ///
    /// @param[in]  pLexemeText     Pointer to string to receive the copied
    ///                             character, or `nullptr` to discard.
    /// @return                     `true` if another character is available
    ///                             after the operation.
    ///
    inline bool AdvanceCharacter(UTF8String* pLexemeText = nullptr);

    /// Advance buffer by one octet.
    ///
    /// @note
    ///     This method does not update the column or line counter.
    ///
    /// @pre
    ///     The buffer shall not be empty.
    ///
    /// @return `true` if another octet is available.
    ///
    inline bool AdvanceOctet();

    /// Advance column or line counter.
    ///
    /// This method increments the column counter by 1, unless the buffer
    /// points to a nominal end-of-line character, in which case the column
    /// counter is reset to 1 and the line counter incremented instead.
    ///
    /// @note
    ///     This method does not advance the buffer.
    ///
    /// @pre
    ///     The buffer shall point to the first octet of the character according
    ///     to which the column or line counter is to be advanced.
    ///
    inline void AdvancePosition();

    /// Test whether an end-of-line character is the first in the sequence.
    ///
    /// This method tests whether the specified end-of-line character is the
    /// stream's nominal end-of-line character (if already defined). If the
    /// nominal end-of-line character is undefined (i.e. set to NUL), it will
    /// be set to the specified character.
    ///
    /// This method is central to end-of-line handling in this class, which
    /// works as follows:
    ///   - The first end-of-line type character ever encountered in the stream
    ///     will be defined as the nominal end-of-line character.
    ///   - Only the nominal end-of-line character will be treated as such
    ///     (e.g. advancing the line count).
    ///   - Any other end-of-line characters will be ignored entirely (e.g.
    ///     not even advancing the column count).
    ///
    /// @note
    ///     This end-of-line style handling was designed based on the premise
    ///     that line ending style is either CR, LF, CR+LF or LF+CR, that it
    ///     is consistent throughout a stream, and that neither CR nor LF appear
    ///     anywhere else in the stream.
    ///
    /// @pre
    ///     The supplied character shall be either CR or LF.
    /// @post
    ///     The stream's nominal end-of-line character shall be defined
    ///     (i.e. set to non-NUL).
    ///
    inline bool IsNominalEndOfLine(Character endOfLineCharacter);

    /// Probe next octet in the buffer.
    inline Octet NextOctet() const;

    /// Probe next character in the buffer for ASCII-specific processing.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    /// @return The ASCII representation of the next character in the stream
    ///         or an arbitrary non-ASCII character otherwise.
    ///
    inline ASCIICharacter NextCharacterASCII() const;

    /// Test whether next character in buffer is any ASCII character.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterASCII() const;

    /// Test whether next character in buffer is a particular ASCII character.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterASCII(char character) const;

    /// Test whether next character in buffer is any of two particular ASCII characters.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterASCII(char c1, char c2) const;

    /// Test whether next character in buffer is any of three particular ASCII characters.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterASCII(char c1, char c2, char c3) const;

    /// Test whether next character in buffer is any end-of-line character.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterEndOfLine() const;

    /// Test whether next character in buffer is any end-of-line character.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterNominalEndOfLine();

    /// Test whether next character in buffer is any whitespace character.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterWhitespace() const;

    /// Test whether next character in buffer is any printable ASCII character.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterPrintableASCII() const;

    /// Test whether next character in buffer is any valid decimal digit.
    ///
    /// @note
    ///     Decimal digits are guaranteed to be ASCII.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterDecimalDigit() const;

    /// Test whether next character in buffer is any valid start-of-identifier character.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterIdentifierChar1() const;

    /// Test whether next character in buffer is any valid identifier character.
    ///
    /// @pre
    ///     The buffer shall not be exhausted.
    ///
    inline bool IsNextCharacterIdentifierChar2() const;

    using RawPosition = POV_OFF_T;

    using RawStream = pov_base::IStream;
    using RawStreampPtr = std::shared_ptr<RawStream>;
};

}
// end of namespace pov_parser

#endif // POVRAY_PARSER_SCANNER_H
