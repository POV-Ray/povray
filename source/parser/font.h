//******************************************************************************
///
/// @file parser/font.h
///
/// Declarations for the font handling in the parser.
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

#ifndef POVRAY_PARSER_FONT_H
#define POVRAY_PARSER_FONT_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "parser/configparser.h"

#ifndef LIBFREETYPE_MISSING

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// Boost header files
//  (none at the moment)

// other 3rd party library headers
#include <ft2build.h>
#include FT_FREETYPE_H

// POV-Ray header files (base module)
#include "base/pov_err.h"
#include "base/types.h"

// POV-Ray header files (core module)
#include "core/math/vector.h"

// POV-Ray header files (parser module)
#include "parser/parsertypes.h"

namespace pov_parser
{

using namespace pov;
using namespace pov_base;

//------------------------------------------------------------------------------

struct FontProcessingException : std::exception
{
    FontProcessingException(FT_Error ftErr);
    virtual const char* what() const override;
protected:
    FT_Error mFreeTypeError;
};

//------------------------------------------------------------------------------

class FontReference
{
public:
    virtual ~FontReference();
    virtual FT_Error NewFace(FT_Library library, FT_Face* face) = 0;
};

class NamedFontFileASCII final : public FontReference
{
public:
    NamedFontFileASCII(const std::string& name);
    virtual FT_Error NewFace(FT_Library library, FT_Face* face) override;
protected:
    std::string mName;
};

class BufferedFontFile final : public FontReference
{
public:
    enum Mode { kPermanent, kCopy, kTransferOwnership };
    BufferedFontFile(const void* buffer, size_t size, Mode mode);
    virtual ~BufferedFontFile() override;
    virtual FT_Error NewFace(FT_Library library, FT_Face* face) override;
protected:
    const unsigned char*    mpData;
    size_t                  mSize;
    bool                    mDelete;
};

//------------------------------------------------------------------------------

using GlyphIndex = POV_UINT32;

struct ShapedGlyph final
{
    UCS4        codePoint;
    GlyphIndex  glyphIndex;
    Vector2d    offset;
};

using ShapedGlyphList = std::vector<ShapedGlyph>;
using OutlineControlPoints = std::vector<Vector2d>;

//------------------------------------------------------------------------------

class FontEngine final
{
public:

    static constexpr auto kAnyCMAP = 0xFFFFFFFFu;

    FontEngine();
    ~FontEngine();

protected:

    friend class    FontFace;
    FT_Library      mFTLibrary;
};

using FontEnginePtr = std::shared_ptr<FontEngine>;

//------------------------------------------------------------------------------

class FontFace final
{
public:

    class GlyphOutlineProcessor;

    FontFace(const FontEnginePtr& pEngine, const FontReferencePtr& pFontRef);
    ~FontFace();

    ShapedGlyphList GetShapedGlyphs(const UCS2String& text, const Vector2d& advance);
    OutlineControlPoints GetCubicBezierOutline(const ShapedGlyph& shapedGlyph);

protected:

    FontEnginePtr       mpEngine;
    FontReferencePtr    mpFontRef;
    FT_Face             mFTFace;
    DBL                 mScalingFactor;

    Vector2d GetKerning(const GlyphIndex g1, const GlyphIndex g2);
    Vector2d GetAdvance(const GlyphIndex g);

    static int MoveTo(const FT_Vector* to, void* user);
    static int LineTo(const FT_Vector* to, void* user);
    static int ConicTo(const FT_Vector* control, const FT_Vector* to, void* user);
    static int CubicTo(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user);
};

using FontFacePtr = std::shared_ptr<FontFace>;

//------------------------------------------------------------------------------

}

#endif // LIBFREETYPE_MISSING

#endif // POVRAY_PARSER_FONT_H
