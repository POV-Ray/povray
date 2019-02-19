//******************************************************************************
///
/// @file base/image/encoding.h
///
/// Declarations related to generic image data encoding (quantization) and
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

#ifndef POVRAY_BASE_IMAGE_ENCODING_H
#define POVRAY_BASE_IMAGE_ENCODING_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of C standard header files
#include <cmath>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/colour.h"
#include "base/mathutil.h"
#include "base/image/colourspace_fwd.h"
#include "base/image/dither_fwd.h"
#include "base/image/image_fwd.h"

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBaseImageEncoding Basic Colour Encoding and Decoding
/// @ingroup PovBaseImage
///
/// @{

//*****************************************************************************
///
/// @name Basic Decoding
///
/// The following functions provide single-channel decoding.
///
/// @{

/// Linear decoding function.
///
/// This function maps an integer value in the range 0..max linearly to a floating-point value
/// in the range 0..1.
///
/// @param[in]  x       Value to decode.
/// @param[in]  max     Encoded value representing 1.0.
/// @return             Decoded value.
///
inline float IntDecode(unsigned int x, unsigned int max)
{
    return float(x) / float(max);
}

/// Generic decoding function.
///
/// This function maps an integer value in the range 0..max to a floating-point value
/// in the range 0..1 using a specific transfer function (gamma curve).
///
/// @param[in]  g       Transfer function (gamma curve) to use.
/// @param[in]  x       Value to decode.
/// @param      max     Encoded value representing 1.0.
///
float IntDecode(const GammaCurvePtr& g, unsigned int x, unsigned int max);

/// @}
///
//*****************************************************************************
///
/// @name Basic Encoding (Quantization)
///
/// The following functions provide single-channel encoding (quantization).
///
/// @{

/// Linear encoding function rounding down.
///
/// This function maps a floating-point value in the range 0..1 linearly to an integer value
/// in the range 0..max, rounding down.
///
/// @note
///     Floating-point values outside the range 0..1 are clipped, mapping them to 0 or max,
///     respectively.
///
/// @param[in]      x       Value to encode.
/// @param[in]      max     Encoded value representing 1.0.
/// @param[in]      qOff    Offset to add before quantization.
///
inline unsigned int IntEncodeDown(float x, unsigned int max, float qOff = 0.0f)
{
    // NB: Deliberately using `std::floor(float)` instead of `std::floorf(float)`
    // to work around GCC Bug 89279.
    return (unsigned int)clip(std::floor(x * float(max) + qOff), 0.0f, float(max));
}

/// Linear encoding function rounding to nearest.
///
/// This function maps a floating-point value in the range 0..1 linearly to an integer value
/// in the range 0..max, rounding to the nearest value.
///
/// @note
///     Floating-point values outside the range 0..1 are clipped, mapping them to 0 or max,
///     respectively.
///
/// @param[in]      x       Value to encode.
/// @param[in]      max     Encoded value representing 1.0.
/// @param[in]      qOff    Offset to add before quantization.
///
inline unsigned int IntEncode(float x, unsigned int max, float qOff = 0.0f)
{
    return IntEncodeDown(x, max, qOff + 0.5f);
}

/// Linear encoding function.
///
/// This function maps a floating-point value in the range 0..1 linearly to an integer value
/// in the range 0..max.
///
/// @note
///     Floating-point values outside the range 0..1 are clipped, mapping them to 0 or max,
///     respectively.
///
/// @param[in]      x       Value to encode.
/// @param[in]      max     Encoded value representing 1.0.
/// @param[in]      qOff    Offset to add before quantization.
/// @param[in,out]  err     Quantization error (including effects due to adding qOff).
///
inline unsigned int IntEncode(float x, unsigned int max, float qOff, float& err)
{
    float xEff = clip(x, 0.0f, 1.0f) + err;
    unsigned int v = IntEncode(xEff, max, qOff);
    err = xEff - IntDecode(v, max);
    return v;
}

/// Generic encoding function.
///
/// This function maps a floating-point value in the range 0..1 to an integer value
/// in the range 0..max.
///
/// @note
///     Floating-point values outside the range 0..1 are clipped, mapping them to 0 or max,
///     respectively.
/// @note
///     The transfer function is presumed to map values 0 and 1 to the respective value itself.
///
/// @param[in]      g       Transfer function (gamma curve) to use.
/// @param[in]      x       Value to encode.
/// @param[in]      max     Encoded value representing 1.0.
/// @param[in]      qOff    Offset to add before quantization.
/// @param[in,out]  err     Quantization error (including effects due to adding qOff).
///
unsigned int IntEncode(const GammaCurvePtr& g, float x, unsigned int max, float qOff, float& err);

/// Generic encoding function.
///
/// This function maps a floating-point value in the range 0..1 to an integer value
/// in the range 0..max.
///
/// @note
///     Floating-point values outside the range 0..1 are clipped, mapping them to 0 or max,
///     respectively.
/// @note
///     The transfer function is presumed to map values 0 and 1 to the respective value itself.
///
/// @param[in]      g       Transfer function (gamma curve) to use.
/// @param[in]      x       Value to encode.
/// @param[in]      max     Encoded value representing 1.0.
/// @param[in]      qOff    Offset to add before quantization.
///
inline unsigned int IntEncode(const GammaCurvePtr& g, float x, unsigned int max, float qOff = 0.0f)
{
    float err = 0.0f;
    return IntEncode(g, x, max, qOff, err);
}

/// @}
///
//*****************************************************************************
///
/// @name Alpha Premultiplication
///
/// The following functions are provided to convert between premultiplied and non-premultiplied alpha.
///
/// @{

/// Function to apply alpha premultiplication.
///
/// @param[in,out]  colour  RGBFT colour to premultiply
///
void AlphaPremultiply(RGBFTColour& colour);

/// Function to apply alpha premultiplication.
///
/// @param[in,out]  colour  RGBT colour to premultiply
///
void AlphaPremultiply(RGBTColour& colour);

/// Function to apply alpha premultiplication.
///
/// @param[in,out]  colour  RGB colour to premultiply
/// @param[in]      alpha   alpha to premultiply with
///
void AlphaPremultiply(RGBColour& colour, float alpha);

/// Function to undo alpha premultiplication.
///
/// @param[in,out]  colour  RGBFT colour to un-premultiply
///
void AlphaUnPremultiply(RGBFTColour& colour);

/// Function to undo alpha premultiplication.
///
/// @param[in,out]  colour  RGBT colour to un-premultiply
///
void AlphaUnPremultiply(RGBTColour& colour);

/// Function to undo alpha premultiplication.
/// @param[in,out]  colour  RGB colour to un-premultiply
/// @param[in]      alpha   alpha the colour is premultiplied with
///
void AlphaUnPremultiply(RGBColour& colour, float alpha);

/// @}
///
//*****************************************************************************
///
/// @name Convenience Decoding
///
/// The following functions are provided as a convenience for image file decoding.
///
/// @{

void SetEncodedGrayValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int gray);
void SetEncodedGrayAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int gray, unsigned int alpha, bool premul = false);
void SetEncodedRGBValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int red, unsigned int green, unsigned int blue);
void SetEncodedRGBAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha, bool premul = false);
void SetEncodedGrayValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float gray);
void SetEncodedGrayAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float gray, float alpha, bool premul = false);
void SetEncodedRGBValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float red, float green, float blue);
void SetEncodedRGBAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float red, float green, float blue, float alpha, bool premul = false);
void SetEncodedRGBValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, const RGBColour& rgb);

/// @}
///
//*****************************************************************************
///
/// @name Convenience Encoding (Quantization)
///
/// The following functions are provided as a convenience for image file encoding (quantization).
///
/// @{

unsigned int GetEncodedGrayValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, DitherStrategy& dh);
void GetEncodedGrayAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int& gray, unsigned int& alpha, DitherStrategy& dh, bool premul = false);
void GetEncodedRGBValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int& red, unsigned int& green, unsigned int& blue, DitherStrategy& dh);
void GetEncodedRGBAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int& red, unsigned int& green, unsigned int& blue, unsigned int& alpha, DitherStrategy& dh, bool premul = false);
float GetEncodedGrayValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&);
void GetEncodedGrayAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float& gray, float& alpha, bool premul = false);
void GetEncodedRGBValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float& red, float& green, float& blue);
void GetEncodedRGBAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float& red, float& green, float& blue, float& alpha, bool premul = false);
void GetEncodedRGBValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, RGBColour& rgb);

/// @}
///
//*****************************************************************************

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_IMAGE_ENCODING_H
