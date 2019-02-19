//******************************************************************************
///
/// @file parser/parsertypes.cpp
///
/// Implementation of essential types used throughout the parser.
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
#include "parser/parsertypes.h"

// C++ variants of C standard header files
// C++ standard header files

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/povassert.h"

// POV-Ray header files (core module)
// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

LexemePosition::LexemePosition() : SourcePosition(1, 1, 0)
{}

bool LexemePosition::operator==(const LexemePosition& o) const
{
    bool result = (offset == o.offset);
    POV_PARSER_ASSERT(result == ((line == o.line) && (column == o.column)));
    return result;
}

POV_OFF_T LexemePosition::operator-(const LexemePosition& o) const
{
    return offset - o.offset;
}

//******************************************************************************

TokenizerException::TokenizerException(const UCS2String& osn, const LexemePosition & op) :
    offendingStreamName(osn),
    offendingPosition(op)
{}

UCS2String TokenizerException::GetFileName() const
{
    return offendingStreamName;
}

POV_LONG TokenizerException::GetLine() const
{
    return offendingPosition.line;
}

POV_LONG TokenizerException::GetColumn() const
{
    return offendingPosition.column;
}

POV_OFF_T TokenizerException::GetOffset() const
{
    return offendingPosition.offset;
}

IncompleteCommentException::IncompleteCommentException(const UCS2String& osn, const LexemePosition& op) :
    TokenizerException(osn, op)
{}

IncompleteStringLiteralException::IncompleteStringLiteralException(const UCS2String& osn, const LexemePosition& op) :
    TokenizerException(osn, op)
{}

InvalidEncodingException::InvalidEncodingException(const UCS2String& osn, const LexemePosition& op,
                                                   const char* encodingName, const char* details) :
    TokenizerException(osn, op),
    encodingName(encodingName),
    details(details)
{}

InvalidCharacterException::InvalidCharacterException(const UCS2String& osn, const LexemePosition& op, UCS4 oc) :
    TokenizerException(osn, op),
    offendingCharacter(oc)
{}

InvalidEscapeSequenceException::InvalidEscapeSequenceException(const UCS2String& osn, const LexemePosition& op,
                                                               const UTF8String& ot) :
    TokenizerException(osn, op),
    offendingText(ot)
{}

InvalidEscapeSequenceException::InvalidEscapeSequenceException(const UCS2String& osn, const LexemePosition& op,
                                                               const UTF8String::const_iterator& otb,
                                                               const UTF8String::const_iterator& ote) :
    TokenizerException(osn, op),
    offendingText(otb, ote)
{}

}
// end of namespace pov_parser
