//******************************************************************************
///
/// @file base/stringutilities.h
///
/// Declarations related to string handling.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_BASE_STRINGUTILITIES_H
#define POVRAY_BASE_STRINGUTILITIES_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

namespace pov_base
{

/// Type holding an UCS2-encoded string of characters.
///
/// @todo   UCS2 is a poor choice for internal string representation, as it cannot encode the full UCS/Unicode character
///         set; we should use either UTF8 (the best space saver for the strings to be expected), or UTF32 (allowing
///         easiest processing). We shouldn't use UTF16, as it needs about just as much special processing as UTF8 (but
///         people tend to forget that, confusing UTF16 with UCS2).
///
typedef std::basic_string<UCS2> UCS2String;

UCS2String ASCIItoUCS2String(const char *s);
std::string UCS2toASCIIString(const UCS2String& s);

int pov_stricmp(const char *, const char *);
const char *pov_tsprintf(const char *, ...);

}

#endif // POVRAY_BASE_STRINGUTILITIES_H
