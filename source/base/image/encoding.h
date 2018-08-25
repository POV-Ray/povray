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
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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

// POV-Ray base header files
#include "base/mathutil.h"
#include "base/types.h"
#include "base/image/colourspace.h"

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBaseImageEncoding Basic Colour Encoding and Decoding
/// @ingroup PovBaseImage
///
/// @{

class Image;

enum DitherMethodId
{
    kPOVList_DitherMethod_None,
    kPOVList_DitherMethod_Diffusion1D,
    kPOVList_DitherMethod_Diffusion2D,
    kPOVList_DitherMethod_FloydSteinberg,
    kPOVList_DitherMethod_Bayer2x2,
    kPOVList_DitherMethod_Bayer3x3,
    kPOVList_DitherMethod_Bayer4x4,
};

/// Abstract class representing a dithering algorithm and state.
///
/// @note
///     The interface is designed to be used in quantization of a single complete image, with the
///     image processed line by line and pixel by pixel, starting at (x=0,y=0). Failure to adhere
///     to this processing order may lead to undefined behaviour in stateful dithering algorithms.
///
class DitherStrategy
{
    public:

        /// Represents an offset to a colour.
        struct ColourOffset
        {
            union { float red, gray; };  float green, blue, alpha;
            ColourOffset() : red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f) { }
            ColourOffset(float r, float g, float b, float a) : red(r), green(g), blue(b), alpha(a) { }
            void clear() { red = 0.0f; green = 0.0f; blue = 0.0f; alpha = 0.0f; }
            void setAll(float v) { red = v; green = v; blue = v; alpha = v; }
            void setRGB(RGBColour& v) { red = v.red(); green = v.green(); blue = v.blue(); alpha = 0.0; }
            RGBColour getRGB() { return RGBColour(red, green, blue); }
            ColourOffset operator*(float b) const { return ColourOffset(red*b, green*b, blue*b, alpha*b); }
            ColourOffset operator+(const ColourOffset& b) const { return ColourOffset(red+b.red, green+b.green, blue+b.blue, alpha+b.alpha); }
            void operator+=(const ColourOffset& b) { red+=b.red; green+=b.green; blue+=b.blue; alpha+=b.alpha; }
        };

        virtual ~DitherStrategy() {}

        /// Queries a colour offset from the algorithm.
        ///
        /// This function computes an offset to be added to the colour of a given pixel, based on
        /// the pixel location and/or the algorithm's state.
        ///
        /// @param[in]  x       X coordinate of the pixel (may or may not be relevant to the algorithm).
        /// @param[in]  y       Y coordinate of the pixel (may or may not be relevant to the algorithm).
        /// @param[out] offLin  Linear offset to add before any encoding steps.
        ///                     This is typically based on carried-over quantization errors from neighboring pixels, as
        ///                     used in stateful dither algorithms.
        /// @param[out] offQnt  Offset to add right before quantization (even after scaling).
        ///                     This is typically more or less random noise in the range [-0.5, 0.5], as used in
        ///                     stateless dither algorithms.
        ///
        virtual void GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt) = 0;

        /// Reports the actual quantization error to the algorithm.
        ///
        /// This function feeds back the actual quantization error to the algorithm, allowing a
        /// stateful algorithm to update its state accordingly.
        ///
        /// @param[in]  x       X coordinate of the pixel (may or may not be relevant to the algorithm).
        /// @param[in]  y       Y coordinate of the pixel (may or may not be relevant to the algorithm).
        /// @param[in]  err     Linear quantization error (may or may not be relevant to the algorithm).
        ///
        virtual void SetError(unsigned int x, unsigned int y, const ColourOffset& err) {}
};

typedef shared_ptr<DitherStrategy> DitherStrategySPtr;

//*****************************************************************************
///
/// @name Dithering
///
/// The following functions provide dithering functionality.
///
/// @{

/// Factory function to get a dithering algorithm and state.
DitherStrategySPtr GetDitherStrategy(DitherMethodId method, unsigned int imageWidth);

/// Factory function to get a no-op dithering algorithm.
DitherStrategySPtr GetNoOpDitherStrategy();

/// Function providing simple stateless dithering.
///
/// This function is provided as a fallback from the `DitherStrategy` mechanism to provide basic
/// dithering functionality in cases where stateful operation is impractical, such as the render
/// preview.
///
/// The current implementation is based on 4x4 Bayer dithering.
///
/// @param[in]  x   Image x coordinate.
/// @param[in]  y   Image y coordinate.
/// @return         Offset to add right before quantization (even after scaling).
///
float GetDitherOffset(unsigned int x, unsigned int y);

/// @}
///
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
inline float IntDecode(const GammaCurvePtr& g, unsigned int x, unsigned int max)
{
    return GammaCurve::Decode(g, IntDecode(x, max));
}

/// @}
///
//*****************************************************************************
///
/// @name Basic Encoding (Quantization)
///
/// The following functions provide single-channel encoding (quantization).
///
/// @{

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
///
inline unsigned int IntEncode(float x, unsigned int max, float qOff = 0.0f)
{
    return (unsigned int)clip(floor(x * float(max) + qOff + 0.5f), 0.0f, float(max));
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
/// @param[out]     err     Quantization error (including effects due to adding qOff).
///
inline unsigned int IntEncode(float x, unsigned int max, float qOff, float& err)
{
    unsigned int v = IntEncode(x, max, qOff);
    err = clip(x, 0.0f, 1.0f) - IntDecode(v, max);
    return v;
}

/// Generic encoding function.
///
/// This function maps a floating-point value in the range 0..1 to an integer value
/// in the range 0..max.
///
/// @note
///     Floating-point values outside the range 0..1 (after applying the transfer function)
///     are clipped, mapping them to 0 or max, respectively.
///
/// @param[in]      g       Transfer function (gamma curve) to use.
/// @param[in]      x       Value to encode.
/// @param[in]      max     Encoded value representing 1.0.
/// @param[in]      qOff    Offset to add before quantization.
///
inline unsigned int IntEncode(const GammaCurvePtr& g, float x, unsigned int max, float qOff = 0.0f)
{
    return IntEncode(GammaCurve::Encode(g, x), max, qOff);
}

/// Generic encoding function.
///
/// This function maps a floating-point value in the range 0..1 to an integer value
/// in the range 0..max.
///
/// @note
///     Floating-point values outside the range 0..1 (after applying the transfer function)
///     are clipped, mapping them to 0 or max, respectively.
///
/// @param[in]      g       Transfer function (gamma curve) to use.
/// @param[in]      x       Value to encode.
/// @param[in]      max     Encoded value representing 1.0.
/// @param[in]      qOff    Offset to add before quantization.
/// @param[out]     err     Quantization error (including effects due to adding qOff).
///
inline unsigned int IntEncode(const GammaCurvePtr& g, float x, unsigned int max, float qOff, float& err)
{
    unsigned int v = IntEncode(g, x, max, qOff);
    err = clip(x, 0.0f, 1.0f) - IntDecode(g, v, max);
    return v;
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

#endif // POVRAY_BASE_IMAGE_ENCODING_H
