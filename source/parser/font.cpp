//******************************************************************************
///
/// @file parser/font.cpp
///
/// Implementation of the font handling in the parser.
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
#include "parser/font.h"

#ifndef LIBFREETYPE_MISSING

// C++ variants of C standard header files
// C++ standard header files
// Boost header files
//  (none at the moment)

// other 3rd party library headers
#include FT_OUTLINE_H

// POV-Ray header files (base module)
#include "base/stringutilities.h"

// POV-Ray header files (core module)
// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

//******************************************************************************

FontReference::~FontReference()
{}

//------------------------------------------------------------------------------

NamedFontFileASCII::NamedFontFileASCII(const std::string& name) :
    mName(name)
{}

FT_Error NamedFontFileASCII::NewFace(FT_Library library, FT_Face* face)
{
    return FT_New_Face(library, mName.c_str(), 0, face);
}

//------------------------------------------------------------------------------

BufferedFontFile::BufferedFontFile(const void* buffer, size_t size, Mode mode) :
    mpData(reinterpret_cast<const unsigned char*>((mode == Mode::kCopy) ? std::memcpy(new unsigned char[size], buffer, size) : buffer)),
    mSize(size),
    mDelete(mode == Mode::kTransferOwnership)
{}

BufferedFontFile::~BufferedFontFile()
{
    if (mDelete && (mpData != nullptr))
        delete[] mpData;
}

FT_Error BufferedFontFile::NewFace(FT_Library library, FT_Face* face)
{
    return FT_New_Memory_Face(library, reinterpret_cast<const FT_Byte*>(mpData), mSize, 0, face);
}

//------------------------------------------------------------------------------

FontResolver::~FontResolver()
{}

//******************************************************************************

FontEngine::FontEngine() :
    mFTLibrary(nullptr)
{
    FT_Error error = FT_Init_FreeType(&mFTLibrary);
    if (error != FT_Err_Ok)
    {
        mFTLibrary = nullptr;
        // TODO throw tantrum
    }
}

FontEngine::~FontEngine()
{
    if (mFTLibrary != nullptr)
        (void)FT_Done_FreeType(FT_Library(mFTLibrary));
}

//******************************************************************************

FontFace::FontFace(const FontEnginePtr& pEngine, const FontReferencePtr& pFontRef) :
    mpEngine(pEngine),
    mFTFace(nullptr)
{
    FT_Error error = pFontRef->NewFace(mpEngine->mFTLibrary, &mFTFace);
    if (error != FT_Err_Ok)
    {
        mFTFace = nullptr;
        // TODO throw tantrum
    }
    else
    {
        error = FT_Set_Char_Size(mFTFace, mFTFace->units_per_EM, mFTFace->units_per_EM, 0, 0);
        if (error != FT_Err_Ok)
            ; // TODO throw tantrum
        mScalingFactor = 1.0 / mFTFace->units_per_EM;
    }
}

FontFace::~FontFace()
{
    if (mFTFace != nullptr)
        (void)FT_Done_Face(mFTFace);
}

Vector2d FontFace::GetKerning(const GlyphIndex g1, const GlyphIndex g2)
{
    if (FT_HAS_KERNING(mFTFace))
    {
        FT_Vector kerningOffset;
        FT_Error error = FT_Get_Kerning(mFTFace, g1, g2, FT_KERNING_UNFITTED, &kerningOffset);
        if (error != FT_Err_Ok)
            ; // TODO throw tantrum
        return Vector2d(kerningOffset.x * mScalingFactor, 0);
    }
    else
        return Vector2d(0, 0);
}

Vector2d FontFace::GetAdvance(const GlyphIndex g)
{
    FT_Error error = FT_Load_Glyph(mFTFace, g, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_NO_RECURSE);
    if (error != FT_Err_Ok)
        ; // TODO throw tantrum
    return Vector2d(mFTFace->glyph->linearHoriAdvance * mScalingFactor / 1024.0, 0);
}

//------------------------------------------------------------------------------

class FontFace::GlyphOutlineProcessor final
{
public:
    GlyphOutlineProcessor(OutlineControlPoints& controlPoints, double scaling, Vector2d offset);
    virtual ~GlyphOutlineProcessor() = default;
    bool MoveTo(const FT_Vector& to);
    bool LineTo(const FT_Vector& to);
    bool ConicTo(const FT_Vector& control, const FT_Vector& to);
    bool CubicTo(const FT_Vector& control1, const FT_Vector& control2, const FT_Vector& to);
protected:
    OutlineControlPoints&   mControlPoints;
    double                  mScaling;
    Vector2d                mOffset;
    Vector2d                mPreviousTo;
    Vector2d ConvertPoint(const FT_Vector& p) const;
};

//------------------------------------------------------------------------------

FontFace::GlyphOutlineProcessor::GlyphOutlineProcessor(OutlineControlPoints& controlPoints, double scaling, Vector2d offset) :
    mControlPoints(controlPoints),
    mScaling(scaling),
    mOffset(offset)
{}

bool FontFace::GlyphOutlineProcessor::MoveTo(const FT_Vector& to)
{
    Vector2d pTo        = ConvertPoint(to);
    mPreviousTo         = pTo;

    return true;
}

bool FontFace::GlyphOutlineProcessor::LineTo(const FT_Vector& to)
{
    Vector2d pFrom      = mPreviousTo;
    Vector2d pTo        = ConvertPoint(to);
    mPreviousTo         = pTo;

    mControlPoints.push_back(pFrom);
    mControlPoints.push_back((pFrom * 2 + pTo) / 3);
    mControlPoints.push_back((pFrom + pTo * 2) / 3);
    mControlPoints.push_back(pTo);

    return true;
}

bool FontFace::GlyphOutlineProcessor::ConicTo(const FT_Vector& control, const FT_Vector& to)
{
    Vector2d pFrom      = mPreviousTo;
    Vector2d pControl   = ConvertPoint(control);
    Vector2d pTo        = ConvertPoint(to);
    mPreviousTo         = pTo;

    mControlPoints.push_back(pFrom);
    mControlPoints.push_back((pFrom + pControl * 2) / 3);
    mControlPoints.push_back((pTo   + pControl * 2) / 3);
    mControlPoints.push_back(pTo);

    return true;
}

bool FontFace::GlyphOutlineProcessor::CubicTo(const FT_Vector& control1, const FT_Vector& control2, const FT_Vector& to)
{
    Vector2d pFrom      = mPreviousTo;
    Vector2d pControl1  = ConvertPoint(control1);
    Vector2d pControl2  = ConvertPoint(control2);
    Vector2d pTo        = ConvertPoint(to);
    mPreviousTo         = pTo;

    mControlPoints.push_back(pFrom);
    mControlPoints.push_back(pControl1);
    mControlPoints.push_back(pControl2);
    mControlPoints.push_back(pTo);

    return true;
}

Vector2d FontFace::GlyphOutlineProcessor::ConvertPoint(const FT_Vector& p) const
{
    return (Vector2d(p.x, p.y) * mScaling) + mOffset;
}

//------------------------------------------------------------------------------

int FontFace::MoveTo(const FT_Vector* to, void* user)
{
    if (reinterpret_cast<GlyphOutlineProcessor*>(user)->MoveTo(*to))
        return FT_Err_Ok;
    else
        return FT_Err_Cannot_Render_Glyph;
}

int FontFace::LineTo(const FT_Vector* to, void* user)
{
    if (reinterpret_cast<GlyphOutlineProcessor*>(user)->LineTo(*to))
        return FT_Err_Ok;
    else
        return FT_Err_Cannot_Render_Glyph;
}

int FontFace::ConicTo(const FT_Vector* control, const FT_Vector* to, void* user)
{
    if (reinterpret_cast<GlyphOutlineProcessor*>(user)->ConicTo(*control, *to))
        return FT_Err_Ok;
    else
        return FT_Err_Cannot_Render_Glyph;
}

int FontFace::CubicTo(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user)
{
    if (reinterpret_cast<GlyphOutlineProcessor*>(user)->CubicTo(*control1, *control2, *to))
        return FT_Err_Ok;
    else
        return FT_Err_Cannot_Render_Glyph;
}

//------------------------------------------------------------------------------

ShapedGlyphList FontFace::GetShapedGlyphs(const UCS2String& text, const Vector2d& advance)
{
    ShapedGlyphList glyphs;
    ShapedGlyph glyph;
    glyph.offset = Vector2d(0, 0);
    FT_UInt previousGlyphIndex = 0;
    bool firstCharacter = true;
    for (auto& c : text)
    {
        // TODO - Remap code point if applicable.

        glyph.codePoint = c;
        glyph.glyphIndex = FT_Get_Char_Index(mFTFace, c);

        if (!firstCharacter)
            glyph.offset += GetKerning(previousGlyphIndex, glyph.glyphIndex);

        glyphs.push_back(glyph);

        glyph.offset += GetAdvance(glyph.glyphIndex) + advance;
        previousGlyphIndex = glyph.glyphIndex;
        firstCharacter = false;
    }
    return glyphs;
}

OutlineControlPoints FontFace::GetCubicBezierOutline(const ShapedGlyph& glyph)
{
    OutlineControlPoints controlPoints;
    GlyphOutlineProcessor outlineProcessor(controlPoints, mScalingFactor, glyph.offset);

    FT_Error error = FT_Load_Glyph(mFTFace, glyph.glyphIndex, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);
    if (error != FT_Err_Ok)
        ; // TODO throw tantrum
    if (mFTFace->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
        ; // TODO throw tantrum
    FT_Outline& outline = mFTFace->glyph->outline;

    FT_Outline_Funcs ftFuncs;
    ftFuncs.move_to = MoveTo;
    ftFuncs.line_to = LineTo;
    ftFuncs.conic_to = ConicTo;
    ftFuncs.cubic_to = CubicTo;
    ftFuncs.shift = 0;
    ftFuncs.delta = 0;
    error = FT_Outline_Decompose(&outline, &ftFuncs, &outlineProcessor);
    if (error != FT_Err_Ok)
        ; // TODO throw tantrum

    return controlPoints;
}

//******************************************************************************

} // end of namespace pov_parser

#endif // LIBFREETYPE_MISSING
