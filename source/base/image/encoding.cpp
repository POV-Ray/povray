//******************************************************************************
///
/// @file base/image/encoding.cpp
///
/// Implementations related to generic image data encoding (quantization) and
/// decoding.
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
#include "base/image/encoding.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
#include "base/image/colourspace.h"
#include "base/image/dither.h"
#include "base/image/image.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

using std::min;
using std::max;

//*****************************************************************************

float IntDecode(const GammaCurvePtr& g, unsigned int x, unsigned int max)
{
    return GammaCurve::Decode(g, IntDecode(x, max));
}

unsigned int IntEncode(const GammaCurvePtr& g, float x, unsigned int max, float qOff, float& err)
{
    if (GammaCurve::IsNeutral(g))
        return IntEncode(x, max, qOff, err);

    float xEff = clip(x, 0.0f, 1.0f) + err;
    unsigned int v = IntEncodeDown(GammaCurve::Encode(g, xEff), max);
    float decoded = IntDecode(g, v, max);
    if (v >= max)
    {
        err = xEff - decoded;
        return v;
    }
    float decodedUp = IntDecode(g, v + 1, max);
    float threshold = (0.5 - qOff) * decoded + (0.5 + qOff) * decodedUp;
    if (xEff > threshold)
    {
        decoded = decodedUp;
        ++v;
    }
    err = xEff - decoded;
    return v;
}

/*******************************************************************************/

#define ALPHA_EPSILON 1.0e-6 ///< Smallest alpha value we dare to safely use with premultiplied alpha.

/*******************************************************************************/

inline void AlphaPremultiply(float& fGray, float fAlpha)
{
    fGray *= fAlpha;
}
inline void AlphaPremultiply(float& fRed, float& fGreen, float& fBlue, float fAlpha)
{
    fRed   *= fAlpha;
    fGreen *= fAlpha;
    fBlue  *= fAlpha;
}
inline void AlphaUnPremultiply(float& fGray, float fAlpha)
{
    if (fAlpha == 0)
        // This special case has no perfectly sane solution. We'll just pretend that fAlpha is very, very small but non-zero.
        fAlpha = ALPHA_EPSILON;
    fGray /= fAlpha;
}
inline void AlphaUnPremultiply(float& fRed, float& fGreen, float& fBlue, float fAlpha)
{
    if (fAlpha == 0)
        // This special case has no perfectly sane solution. We'll just pretend that fAlpha is very, very small but non-zero.
        fAlpha = ALPHA_EPSILON;
    fRed   /= fAlpha;
    fGreen /= fAlpha;
    fBlue  /= fAlpha;
}

void AlphaPremultiply(RGBColour& colour, float fAlpha)
{
    AlphaPremultiply(colour.red(), colour.green(), colour.blue(), fAlpha);
}

void AlphaUnPremultiply(RGBColour& colour, float fAlpha)
{
    AlphaUnPremultiply(colour.red(), colour.green(), colour.blue(), fAlpha);
}

void AlphaPremultiply(RGBFTColour& colour)
{
    AlphaPremultiply(colour.red(), colour.green(), colour.blue(), colour.FTtoA());
}

void AlphaUnPremultiply(RGBFTColour& colour)
{
    AlphaUnPremultiply(colour.red(), colour.green(), colour.blue(), colour.FTtoA());
}

void AlphaPremultiply(RGBTColour& colour)
{
    AlphaPremultiply(colour.red(), colour.green(), colour.blue(), colour.alpha());
}

void AlphaUnPremultiply(RGBTColour& colour)
{
    AlphaUnPremultiply(colour.red(), colour.green(), colour.blue(), colour.alpha());
}


void SetEncodedGrayValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, unsigned int max, unsigned int gray)
{
    if (!img->IsIndexed() && img->GetMaxIntValue() == max && GammaCurve::IsNeutral(g))
        // avoid potential re-quantization in case we have a pretty match between encoded data and container
        img->SetGrayValue(x, y, gray);
    else
        img->SetGrayValue(x, y, IntDecode(g,gray,max));
}
void SetEncodedGrayAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, unsigned int max, unsigned int gray, unsigned int alpha, bool premul)
{
    bool doPremultiply   = (alpha != max) && !premul && (img->IsPremultiplied() || !img->HasTransparency()); // need to apply premultiplication if encoded data isn't PM'ed but container content should be
    bool doUnPremultiply = (alpha != max) && premul && !img->IsPremultiplied() && img->HasTransparency(); // need to undo premultiplication if other way round
    if (!doPremultiply && !doUnPremultiply && !img->IsIndexed() && img->GetMaxIntValue() == max && GammaCurve::IsNeutral(g))
        // avoid potential re-quantization in case we have a pretty match between encoded data and container
        img->SetGrayAValue(x, y, gray, alpha);
    else
    {
        float fAlpha = IntDecode(alpha,max);
        float fGray  = IntDecode(g,gray,max);
        if (doPremultiply)
            AlphaPremultiply(fGray, fAlpha);
        else if (doUnPremultiply)
            AlphaUnPremultiply(fGray, fAlpha);
        // else no need to worry about premultiplication
        img->SetGrayAValue(x, y, fGray, fAlpha);
    }
}
void SetEncodedRGBValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, unsigned int max, unsigned int red, unsigned int green, unsigned int blue)
{
    if (!img->IsIndexed() && img->GetMaxIntValue() == max && GammaCurve::IsNeutral(g))
        // avoid potential re-quantization in case we have a pretty match between encoded data and container
        img->SetRGBValue(x, y, red, green, blue);
    else
        img->SetRGBValue(x, y, IntDecode(g,red,max), IntDecode(g,green,max), IntDecode(g,blue,max));
}
void SetEncodedRGBAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, unsigned int max, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha, bool premul)
{
    bool doPremultiply   = (alpha != max) && !premul && (img->IsPremultiplied() || !img->HasTransparency()); // need to apply premultiplication if encoded data isn't PM'ed but container content should be
    bool doUnPremultiply = (alpha != max) && premul && !img->IsPremultiplied() && img->HasTransparency(); // need to undo premultiplication if other way round
    if (!doPremultiply && !doUnPremultiply && !img->IsIndexed() && img->GetMaxIntValue() == max && GammaCurve::IsNeutral(g))
        // avoid potential re-quantization in case we have a pretty match between encoded data and container
        img->SetRGBAValue(x, y, red, green, blue, alpha);
    else
    {
        float fAlpha = IntDecode(alpha,  max);
        float fRed   = IntDecode(g,red,  max);
        float fGreen = IntDecode(g,green,max);
        float fBlue  = IntDecode(g,blue, max);
        if (doPremultiply)
            AlphaPremultiply(fRed, fGreen, fBlue, fAlpha);
        else if (doUnPremultiply)
            AlphaUnPremultiply(fRed, fGreen, fBlue, fAlpha);
        // else no need to worry about premultiplication
        img->SetRGBAValue(x, y, fRed, fGreen, fBlue, fAlpha);
    }
}
void SetEncodedGrayValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, float fGray)
{
    img->SetGrayValue(x, y, GammaCurve::Decode(g,fGray));
}
void SetEncodedGrayAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, float fGray, float fAlpha, bool premul)
{
    bool doPremultiply   = !premul && (img->IsPremultiplied() || !img->HasTransparency()); // need to apply premultiplication if encoded data isn't PM'ed but container content should be
    bool doUnPremultiply = premul && !img->IsPremultiplied() && img->HasTransparency(); // need to undo premultiplication if other way round
    fGray = GammaCurve::Decode(g,fGray);
    if (doPremultiply)
        AlphaPremultiply(fGray, fAlpha);
    else if (doUnPremultiply)
        AlphaUnPremultiply(fGray, fAlpha);
    // else no need to worry about premultiplication
    img->SetGrayAValue(x, y, fGray, fAlpha);
}
void SetEncodedRGBValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, float red, float green, float blue)
{
    img->SetRGBValue(x, y, GammaCurve::Decode(g,red), GammaCurve::Decode(g,green), GammaCurve::Decode(g,blue));
}
void SetEncodedRGBAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, float fRed, float fGreen, float fBlue, float fAlpha, bool premul)
{
    bool doPremultiply   = !premul && (img->IsPremultiplied() || !img->HasTransparency()); // need to apply premultiplication if encoded data isn't PM'ed but container content should be
    bool doUnPremultiply = premul && !img->IsPremultiplied() && img->HasTransparency(); // need to undo premultiplication if other way round
    fRed   = GammaCurve::Decode(g,fRed);
    fGreen = GammaCurve::Decode(g,fGreen);
    fBlue  = GammaCurve::Decode(g,fBlue);
    if (doPremultiply)
        AlphaPremultiply(fRed, fGreen, fBlue, fAlpha);
    else if (doUnPremultiply)
        AlphaUnPremultiply(fRed, fGreen, fBlue, fAlpha);
    // else no need to worry about premultiplication
    img->SetRGBAValue(x, y, fRed, fGreen, fBlue, fAlpha);
}
void SetEncodedRGBValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, const RGBColour& col)
{
    img->SetRGBValue(x, y, GammaCurve::Decode(g,col.red()), GammaCurve::Decode(g,col.green()), GammaCurve::Decode(g,col.blue()));
}

unsigned int GetEncodedGrayValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, unsigned int max, DitherStrategy& dh)
{
    float fGray;
    if (!img->IsPremultiplied() && img->HasTransparency())
    {
        // data has transparency and is stored non-premultiplied; precompose against a black background
        float fAlpha;
        img->GetGrayAValue(x, y, fGray, fAlpha);
        AlphaPremultiply(fGray, fAlpha);
    }
    else
    {
        // no need to worry about premultiplication
        fGray = img->GetGrayValue(x, y);
    }
    DitherStrategy::ColourOffset linOff, encOff;
    dh.GetOffset(x,y,linOff,encOff);
    unsigned int iGray = IntEncode(g, fGray, max, encOff.gray, linOff.gray);
    dh.SetError(x,y,linOff);
    return iGray;
}
void GetEncodedGrayAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, unsigned int max, unsigned int& gray, unsigned int& alpha, DitherStrategy& dh, bool premul)
{
    bool doPremultiply   = premul && !img->IsPremultiplied() && img->HasTransparency(); // need to apply premultiplication if encoded data should be premul'ed but container content isn't
    bool doUnPremultiply = !premul && img->IsPremultiplied() && img->HasTransparency(); // need to undo premultiplication if other way round
    float fGray, fAlpha;
    img->GetGrayAValue(x, y, fGray, fAlpha);
    if (doPremultiply)
    {
        AlphaPremultiply(fGray, fAlpha);
    }
    else if (doUnPremultiply)
    {
        // Data has been stored premultiplied, but should be encoded non-premultiplied.
        // Clipping will happen /before/ re-multiplying with alpha (because the latter is done in the viewer), which is equivalent to clipping
        // pre-multiplied components to be no greater than alpha, thereby "killing" highlights on transparent objects;
        // compensate for this by boosting opacity of any exceptionally bright pixels.
        if (fGray > fAlpha)
            fAlpha = min(1.0f, fGray);
        // Need to convert from premultiplied to non-premultiplied encoding.
        AlphaUnPremultiply(fGray, fAlpha);
    }
    else if (!premul)
    {
        // Data has been stored un-premultiplied and should be encoded that way.
        // Clipping will happen /before/ multiplying with alpha (because the latter is done in the viewer), which is equivalent to clipping
        // pre-multiplied components to be no greater than alpha, thereby "killing" highlights on transparent objects;
        // compensate for this by boosting opacity of any exceptionally bright pixels.
        if (fGray > 1.0)
        {
            float fFactor = fGray;
            if (fFactor * fAlpha > 1.0)
                fFactor = 1.0/fAlpha;
            // this keeps the product of alpha*color constant
            fAlpha *= fFactor;
            fGray  /= fFactor;
        }
        // No need for converting between premultiplied and un-premultiplied encoding.
    }
    // else no need to worry about premultiplication
    DitherStrategy::ColourOffset linOff, encOff;
    dh.GetOffset(x,y,linOff,encOff);
    gray  = IntEncode(g, fGray,  max, encOff.gray,  linOff.gray);
    alpha = IntEncode(   fAlpha, max, encOff.alpha, linOff.alpha);
    dh.SetError(x,y,linOff);
}
void GetEncodedRGBValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, unsigned int max, unsigned int& red, unsigned int& green, unsigned int& blue, DitherStrategy& dh)
{
    float fRed, fGreen, fBlue;
    if (!img->IsPremultiplied() && img->HasTransparency())
    {
        float fAlpha;
        // data has transparency and is stored non-premultiplied; precompose against a black background
        img->GetRGBAValue(x, y, fRed, fGreen, fBlue, fAlpha);
        AlphaPremultiply(fRed, fGreen, fBlue, fAlpha);
    }
    else
    {
        // no need to worry about premultiplication
        img->GetRGBValue(x, y, fRed, fGreen, fBlue);
    }
    DitherStrategy::ColourOffset linOff, encOff;
    dh.GetOffset(x,y,linOff,encOff);
    red   = IntEncode(g, fRed,   max, encOff.red,   linOff.red);
    green = IntEncode(g, fGreen, max, encOff.green, linOff.green);
    blue  = IntEncode(g, fBlue,  max, encOff.blue,  linOff.blue);
    dh.SetError(x,y,linOff);
}
void GetEncodedRGBAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, unsigned int max, unsigned int& red, unsigned int& green, unsigned int& blue, unsigned int& alpha, DitherStrategy& dh, bool premul)
{
    bool doPremultiply   = premul && !img->IsPremultiplied() && img->HasTransparency(); // need to apply premultiplication if encoded data should be premul'ed but container content isn't
    bool doUnPremultiply = !premul && img->IsPremultiplied() && img->HasTransparency(); // need to undo premultiplication if other way round
    float fRed, fGreen, fBlue, fAlpha;
    img->GetRGBAValue(x, y, fRed, fGreen, fBlue, fAlpha);
    if (doPremultiply)
    {
        // Data has been stored premultiplied, but should be encoded non-premultiplied.
        // No need for special handling of color components greater than alpha.
        // Need to convert from premultiplied to non-premultiplied encoding.
        AlphaPremultiply(fRed, fGreen, fBlue, fAlpha);
    }
    else if (doUnPremultiply)
    {
        // Data has been stored premultiplied, but should be encoded non-premultiplied.
        // Clipping will happen /before/ re-multiplying with alpha (because the latter is done in the viewer), which is equivalent to clipping
        // pre-multiplied components to be no greater than alpha, thereby "killing" highlights on transparent objects;
        // compensate for this by boosting opacity of any exceptionally bright pixels.
        float fBright = RGBColour(fRed, fGreen, fBlue).Greyscale();
        if (fBright > fAlpha)
            fAlpha = min(1.0f, fBright);
        // Need to convert from premultiplied to non-premultiplied encoding.
        AlphaUnPremultiply(fRed, fGreen, fBlue, fAlpha);
    }
    else if (!premul)
    {
        // Data has been stored un-premultiplied and should be encoded that way.
        // Clipping will happen /before/ multiplying with alpha (because the latter is done in the viewer), which is equivalent to clipping
        // pre-multiplied components to be no greater than alpha, thereby "killing" highlights on transparent objects;
        // compensate for this by boosting opacity of any exceptionally bright pixels.
        float fBright = RGBColour(fRed, fGreen, fBlue).Greyscale();
        if (fBright > 1.0)
        {
            float fFactor = fBright;
            if (fFactor * fAlpha > 1.0)
                fFactor = 1.0/fAlpha;
            // this keeps the product of alpha*color constant
            fAlpha *= fFactor;
            fRed   /= fFactor;
            fGreen /= fFactor;
            fBlue  /= fFactor;
        }
        // No need for converting between premultiplied and un-premultiplied encoding.
    }
    // else no need to worry about premultiplication
    DitherStrategy::ColourOffset linOff, encOff;
    dh.GetOffset(x,y,linOff,encOff);
    red   = IntEncode(g, fRed,   max, encOff.red,   linOff.red);
    green = IntEncode(g, fGreen, max, encOff.green, linOff.green);
    blue  = IntEncode(g, fBlue,  max, encOff.blue,  linOff.blue);
    alpha = IntEncode(   fAlpha, max, encOff.alpha, linOff.alpha);
    dh.SetError(x,y,linOff);
}

float GetEncodedGrayValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g)
{
    float fGray;
    if (!img->IsPremultiplied() && img->HasTransparency())
    {
        // data has transparency and is stored non-premultiplied; precompose against a black background
        float fAlpha;
        img->GetGrayAValue(x, y, fGray, fAlpha);
        AlphaPremultiply(fGray, fAlpha);
    }
    else
    {
        // no need to worry about premultiplication
        fGray = img->GetGrayValue(x, y);
    }
    return GammaCurve::Encode(g,fGray);
}
void GetEncodedGrayAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, float& fGray, float& fAlpha, bool premul)
{
    bool doPremultiply   = premul && !img->IsPremultiplied() && img->HasTransparency(); // need to apply premultiplication if encoded data should be premul'ed but container content isn't
    bool doUnPremultiply = !premul && img->IsPremultiplied() && img->HasTransparency(); // need to undo premultiplication if other way round
    img->GetGrayAValue(x, y, fGray, fAlpha);
    if (doPremultiply)
        AlphaPremultiply(fGray, fAlpha);
    else if (doUnPremultiply)
        AlphaUnPremultiply(fGray, fAlpha);
    // else no need to worry about premultiplication
    fGray = GammaCurve::Encode(g,fGray);
}
void GetEncodedRGBValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, float& fRed, float& fGreen, float& fBlue)
{
    if (!img->IsPremultiplied() && img->HasTransparency())
    {
        // data has transparency and is stored non-premultiplied; precompose against a black background
        float fAlpha;
        img->GetRGBAValue(x, y, fRed, fGreen, fBlue, fAlpha);
        AlphaPremultiply(fRed, fGreen, fBlue, fAlpha);
    }
    else
    {
        // no need to worry about premultiplication
        img->GetRGBValue(x, y, fRed, fGreen, fBlue);
    }
    fRed   = GammaCurve::Encode(g,fRed);
    fGreen = GammaCurve::Encode(g,fGreen);
    fBlue  = GammaCurve::Encode(g,fBlue);
}
void GetEncodedRGBAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, float& fRed, float& fGreen, float& fBlue, float& fAlpha, bool premul)
{
    bool doPremultiply   = premul && !img->IsPremultiplied() && img->HasTransparency(); // need to apply premultiplication if encoded data should be premul'ed but container content isn't
    bool doUnPremultiply = !premul && img->IsPremultiplied() && img->HasTransparency(); // need to undo premultiplication if other way round
    img->GetRGBAValue(x, y, fRed, fGreen, fBlue, fAlpha);
    if (doPremultiply)
        AlphaPremultiply(fRed, fGreen, fBlue, fAlpha);
    else if (doUnPremultiply)
        AlphaUnPremultiply(fRed, fGreen, fBlue, fAlpha);
    // else no need to worry about premultiplication
    fRed   = GammaCurve::Encode(g,fRed);
    fGreen = GammaCurve::Encode(g,fGreen);
    fBlue  = GammaCurve::Encode(g,fBlue);
}
void GetEncodedRGBValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr& g, RGBColour& col)
{
    if (!img->IsPremultiplied() && img->HasTransparency())
    {
        // data has transparency and is stored non-premultiplied; precompose against a black background
        float fAlpha;
        img->GetRGBAValue(x, y, col.red(), col.green(), col.blue(), fAlpha);
        AlphaPremultiply(col, fAlpha);
    }
    else
    {
        // no need to worry about premultiplication
        img->GetRGBValue(x, y, col.red(), col.green(), col.blue());
    }
    col.red()   = GammaCurve::Encode(g,col.red());
    col.green() = GammaCurve::Encode(g,col.green());
    col.blue()  = GammaCurve::Encode(g,col.blue());
}

}
// end of namespace pov_base
