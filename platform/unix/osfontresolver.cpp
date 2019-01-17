//******************************************************************************
///
/// @file platform/unix/osfontresolver.cpp
///
/// Unix-specific implementation of the @ref pov_base::OSFontResolver class.
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
#include "osfontresolver.h"

// POV-Ray header files (parser module)
#include "parser/parsertypes.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

using namespace pov_parser;

class UnixFontResolver final : public FontResolver
{
public:
    virtual FontReferencePtr GetFont(const UCS2String& name, FontStyle style) override;
};

FontReferencePtr UnixFontResolver::GetFont(const UCS2String& name, FontStyle style)
{
    return nullptr;
}

FontResolver& OSGetFontResolver()
{
    thread_local UnixFontResolver fontResolverInstance;
    return fontResolverInstance;
}

} // end of namespace pov_base
