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
#include "parser/parsertypes.h"

// C++ variants of C standard header files
// C++ standard header files
// Boost header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"

// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

bool LexemePosition::operator==(const LexemePosition& o) const
{
    bool result = (offset == o.offset);
    POV_PARSER_ASSERT(result == ((line == o.line) && (column == o.column)));
    return result;
}

LexemePosition::LexemePosition() :
    line(1),
    column(1),
    offset(0)
{}

POV_OFF_T LexemePosition::operator-(const LexemePosition& o) const
{
    return offset - o.offset;
}

//******************************************************************************

TokenizerException::TokenizerException(const ConstSourcePtr & os, const LexemePosition & op) :
    offendingStreamName(os->Name()),
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

IncompleteCommentException::IncompleteCommentException(const ConstSourcePtr& os, const LexemePosition& op) :
    TokenizerException(os, op)
{}

IncompleteStringLiteralException::IncompleteStringLiteralException(const ConstSourcePtr& os, const LexemePosition& op) :
    TokenizerException(os, op)
{}

InvalidCharacterException::InvalidCharacterException(const ConstSourcePtr& os, const LexemePosition& op, UCS4 oc) :
    TokenizerException(os, op),
    offendingCharacter(oc)
{}

InvalidEscapeSequenceException::InvalidEscapeSequenceException(const ConstSourcePtr& os, const LexemePosition& op,
                                                               const UTF8String& ot) :
    TokenizerException(os, op),
    offendingText(ot)
{}

InvalidEscapeSequenceException::InvalidEscapeSequenceException(const ConstSourcePtr& os, const LexemePosition& op,
                                                               const UTF8String::const_iterator& otb,
                                                               const UTF8String::const_iterator& ote) :
    TokenizerException(os, op),
    offendingText(otb, ote)
{}

}
