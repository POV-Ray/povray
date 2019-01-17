//******************************************************************************
///
/// @file platform/windows/osfontresolver.cpp
///
/// Windows-specific implementation of the @ref pov_base::OSFontResolver class.
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

// C++ standard header files
#include <vector>

// platform-specific library headers
#include <windows.h>

// POV-Ray header files (parser module)
#include "parser/font.h"
#include "parser/parsertypes.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

using namespace pov_parser;

class WindowsFontResolver final : public FontResolver
{
public:
    virtual FontReferencePtr GetFont(const UCS2String& name, FontStyle style) override;
};

FontReferencePtr WindowsFontResolver::GetFont(const UCS2String& name, FontStyle style)
{
    if (name.length() >= LF_FACESIZE)
        // Too long to be a valid system font name.
        return nullptr;

    char* buffer = nullptr;

    HDC hdc = CreateCompatibleDC(nullptr);
    if (hdc == nullptr)
        ; // TODO throw tantrum

    int     weight;
    bool    italic;
    switch (style)
    {
        case pov_parser::kRegular:      weight = 400; italic = false; break;
        case pov_parser::kBold:         weight = 700; italic = false; break;
        case pov_parser::kItalic:       weight = 400; italic = true;  break;
        case pov_parser::kBoldItalic:   weight = 700; italic = true;  break;
    }

    HFONT fontHandle = CreateFontW(
        0,                      // cHeight
        0,                      // cWidth
        0,                      // cEscapement
        0,                      // cOrientation
        (int)weight,            // cWeight
        (DWORD)italic,          // bItalic
        FALSE,                  // bUnderline
        FALSE,                  // bStrikeOut
        DEFAULT_CHARSET,        // iCharSet
        OUT_DEFAULT_PRECIS,     // iOutPrecision
        CLIP_DEFAULT_PRECIS,    // iClipPrecision
        DEFAULT_QUALITY,        // iQuality
        DEFAULT_PITCH | FF_DONTCARE, // iPitchAndFamily
        (LPCWSTR)name.c_str()   // pszFaceName
    );

    SelectObject(hdc, fontHandle);
    auto size = ::GetFontData(hdc, 0, 0, nullptr, 0);

    if (size == 0)
        return nullptr;

    buffer = new char[size];
    if (GetFontData(hdc, 0, 0, buffer, size) != size)
    {
        delete[] buffer;
        return nullptr;
    }

    DeleteDC(hdc);

    return std::make_shared<BufferedFontFile>(buffer, size, BufferedFontFile::Mode::kTransferOwnership);
}

FontResolver& OSGetFontResolver()
{
    thread_local WindowsFontResolver fontResolverInstance;
    return fontResolverInstance;
}

} // end of namespace pov_base
