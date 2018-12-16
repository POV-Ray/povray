//******************************************************************************
///
/// @file parser/parsertypes.h
///
/// Essential types and forward declarations used throughout the parser.
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

#ifndef POVRAY_PARSER_PARSERTYPES_H
#define POVRAY_PARSER_PARSERTYPES_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "parser/configparser.h"

// C++ variants of C standard header files
// C++ standard header files
// Boost header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/types.h"
#include "base/messenger.h"

// POV-Ray header files (core module)
#include "core/coretypes.h"

namespace pov_base
{
class IStream;
}

namespace pov_parser
{

using namespace pov_base;

enum TokenId : int;

//------------------------------------------------------------------------------

using SourcePtr = shared_ptr<pov_base::IStream>;
using ConstSourcePtr = shared_ptr<const pov_base::IStream>;

//------------------------------------------------------------------------------

struct LexemePosition : pov::SourcePosition
{
    LexemePosition();
    bool operator==(const LexemePosition& o) const;
    POV_OFF_T operator-(const LexemePosition& o) const;
};

//------------------------------------------------------------------------------

struct TokenizerException : std::exception, MessageContext
{
    UCS2String      offendingStreamName;
    LexemePosition  offendingPosition;

    TokenizerException(const ConstSourcePtr& os, const LexemePosition& op);

    virtual UCS2String GetFileName() const override;
    virtual POV_LONG GetLine() const override;
    virtual POV_LONG GetColumn() const override;
    virtual POV_OFF_T GetOffset() const override;
};

struct IncompleteCommentException : TokenizerException
{
    IncompleteCommentException(const ConstSourcePtr& os, const LexemePosition& op);
};

struct IncompleteStringLiteralException : TokenizerException
{
    IncompleteStringLiteralException(const ConstSourcePtr& os, const LexemePosition& op);
};

struct InvalidCharacterException : TokenizerException
{
    UCS4 offendingCharacter;
    InvalidCharacterException(const ConstSourcePtr& os, const LexemePosition& op, UCS4 oc);
};

struct InvalidEscapeSequenceException : TokenizerException
{
    UTF8String offendingText;
    InvalidEscapeSequenceException(const ConstSourcePtr& os, const LexemePosition& op, const UTF8String& ot);
    InvalidEscapeSequenceException(const ConstSourcePtr& os, const LexemePosition& op,
                                   const UTF8String::const_iterator& otb, const UTF8String::const_iterator& ote);
};

//------------------------------------------------------------------------------

// Base class for miscellaneous things that can be assigned to a symbol.
struct Assignable
{
    virtual ~Assignable() {}
    virtual Assignable* Clone() const = 0;
};

} // end of namespace

#endif // POVRAY_PARSER_PARSERTYPES_H
