//******************************************************************************
///
/// @file base/stringtypes.h
///
/// Declarations related to string types.
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

#ifndef POVRAY_BASE_STRINGTYPES_H
#define POVRAY_BASE_STRINGTYPES_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <string>

// POV-Ray header files (base module)
//  (none at the moment)

namespace pov_base
{

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

//******************************************************************************

//******************************************************************************
///
/// @name Character Types
/// Character types with encoding semantics.
///
/// The following types are used throughout POV-Ray instead of the fundamental
/// character types (`char`, `char16_t` and `char32_t`, respectively),
/// to more clearly identify the character encoding associated with them.
/// While they are normally aliased to the fundamental types, static code
/// analysis tools may still treat them as distinct types to ferret out
/// character encoding incompatibilities between different portions of the code.
///
/// @note
///     There are currently no aliases provided for `wchar_t`.
///
/// @{

/// UTF-16 code unit.
/// This type should be used instead of `char16_t` wherever UTF-16
/// encoding is guaranteed or required.
/// @note
///     Code handling data of this type must expect and properly handle
///     surrogate pairs. Code that can only handle the BMP subset of UTF-16
///     should use @ref UCS2Char instead.
/// @note
///     Code that is agnostic about the details of string encoding (including
///     the actual code unit size and whether it is dealing with code points or
///     actual characters) should use @ref POVChar instead.
using UTF16 = char16_t;

/// Genuine UCS-2 character.
/// This type should be used wherever UCS-2 encoding (aka "BMP-only UTF-16")
/// is required.
/// @deprecated
///     Code that is agnostic about the details of string encoding (including
///     the actual code unit size) should use @ref POVChar instead. Code that
///     requires 16-bit code units but can handle surrogate pairs should use
///     @ref UTF16Unit instead.
using UCS2 = char16_t;

/// Arbitrary UCS character.
/// This type should be used wherever code needs to operate on proper characters
/// (as opposed to code units), but is not bound to a particular character set
/// (or is bound to using UCS).
using UCS4 = char32_t;

//******************************************************************************
///
/// @name String Types
/// Character string types with encoding semantics.
///
/// The following types are used throughout POV-Ray instead of the standard C++
/// string types (`std::string`, `std::u8string`, `std::u16tring` etc.),
/// to more clearly identify the character encoding associated with them.
/// While they are effectively aliased to the standard types, static code
/// analysis tools may still treat them as distinct types to ferret out
/// character encoding incompatibilities between different portions of the code.
///
/// See @ref base/stringutilities.h for functions to convert between these
/// string types.
///
/// @note
///     There are currently no aliases provided for `std::wstring`.
///
/// @{

/// Type holding an @glossary{UTF8}-encoded string of characters.
///
/// @todo
///     Aliasing this as `std::string` may not be ideal, as it creates ambiguity
///     with the use of that same type for ASCII strings. On the other hand,
///     if we use `std::basic_string<unsigned char>`, it isn't compatible with
///     the C++11 `u8"..."` UTF-8 string literal notation, which is of type
///     `const char[]`.
///
using UTF8String = std::string;

/// Type holding an @glossary{UCS2}-encoded string of characters.
///
/// @todo
///     UCS-2 is a poor choice for internal string representation, as it cannot
///     encode the full UCS/Unicode character set; we should use either UTF-8
///     (the best space saver for the strings to be expected), or UTF-32
///     (allowing easiest processing). We shouldn't use UTF-16, as it needs
///     about just as much special processing as UTF-8 (but people tend to
///     forget that, confusing UTF-16 with UCS-2), and for the expected typical
///     ASCII-heavy use it is less memory-efficient than UTF-8.
///
using UCS2String = std::basic_string<UCS2>;

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_STRINGTYPES_H
