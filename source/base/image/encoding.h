/*******************************************************************************
 * encoding.h
 *
 * This file contains code for handling image data quantization.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/povray/smp/source/base/image/encoding.h $
 * $Revision: #6 $
 * $Change: 6132 $
 * $DateTime: 2013/11/25 14:23:41 $
 * $Author: clipka $
 *******************************************************************************/

#ifndef POVRAY_BASE_IMAGE_ENCODING_H
#define POVRAY_BASE_IMAGE_ENCODING_H

#include <boost/thread.hpp>

#include "base/configbase.h"
#include "base/types.h"
#include "base/image/colourspace.h"

namespace pov_base
{

class Image;

/// Abstract class representing dithering rules and respective state information.
class DitherHandler
{
	public:
		struct OffsetInfo
		{
			union { float red, gray; };  float green, blue, alpha;
			OffsetInfo() : red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f) { }
			OffsetInfo(float r, float g, float b, float a) : red(r), green(g), blue(b), alpha(a) { }
			void clear() { red = 0.0f; green = 0.0f; blue = 0.0f; alpha = 0.0f; }
			void setAll(float v) { red = v; green = v; blue = v; alpha = v; }
			OffsetInfo operator*(float b) const { return OffsetInfo(red*b, green*b, blue*b, alpha*b); }
			OffsetInfo operator+(const OffsetInfo& b) const { return OffsetInfo(red+b.red, green+b.green, blue+b.blue, alpha+b.alpha); }
			void operator+=(const OffsetInfo& b) { red+=b.red; green+=b.green; blue+=b.blue; alpha+=b.alpha; }
		};

		virtual ~DitherHandler() {}

		/// Computes an offset to be added to the pixel value.
		/// @param[in]  x       X coordinate of the pixel (may or may not be relevant to the handler).
		/// @param[in]  y       Y coordinate of the pixel (may or may not be relevant to the handler).
		/// @param[out] offLin  Linear offset to add before any encoding steps
		/// @param[out] offQnt  Offset to add right before quantization (even after scaling)
		virtual void getOffset(unsigned int x, unsigned int y, OffsetInfo& offLin, OffsetInfo& offQnt) = 0;

		/// Informs the handler about the actual quantization error observed for one particular pixel.
		/// @param[in]  x       X coordinate of the pixel (may or may not be relevant to the handler).
		/// @param[in]  y       Y coordinate of the pixel (may or may not be relevant to the handler).
		/// @param[in]  err     Linear quantization error.
		virtual void setError(unsigned int x, unsigned int y, const OffsetInfo& err) {}
};

typedef shared_ptr<DitherHandler> DitherHandlerPtr;

/// Factory class to get a dithering rule and state.
DitherHandlerPtr GetDitherHandler(int method, unsigned int imageWidth);

/// Factory class to get a no-op dithering rule.
DitherHandlerPtr GetNoOpDitherHandler();

/// Computes an offset to be added to a pixel value right before quantization (even after scaling).
/// The value returned by this function is based on a default stateless dithering rule.
float GetDitherOffset(unsigned int x, unsigned int y);

/*******************************************************************************/

/**
 *  Linear decoding function.
 *  This function maps an integer value in the range 0..max linearly to a floating-point value in the range 0..1.
 *  @param  x       value to decode
 *  @param  max     encoded value representing 1.0
 */
inline float IntDecode(unsigned int x, unsigned int max) { return float(x) / float(max); }

/**
 *  Generic decoding function.
 *  This function maps an integer value in the range 0..max to a floating-point value in the range 0..1
 *  using a specific transfer function (gamma curve).
 *  @param  g       transfer function (gamma curve) to use
 *  @param  x       value to decode
 *  @param  max     encoded value representing 1.0
 */
inline float IntDecode(const GammaCurvePtr& g, unsigned int x, unsigned int max) { return GammaCurve::Decode(g,IntDecode(x,max)); }

/**
 *  Linear encoding function.
 *  This function maps a floating-point value in the range 0..1 linearly to in an integer value in the range 0..max.
 *  @note           Floating-point values outside the range 0..1 are clipped, mapping them to 0 or max, respectively.
 *  @param[in]      x       value to encode
 *  @param[in]      max     encoded value representing 1.0
 */
inline unsigned int IntEncode(float x, unsigned int max) { return (unsigned int)floor( clip(x,0.0f,1.0f) * float(max) + 0.5f ); }

/**
 *  Linear encoding function.
 *  This function maps a floating-point value in the range 0..1 linearly to in an integer value in the range 0..max.
 *  @note           Floating-point values outside the range 0..1 are clipped, mapping them to 0 or max, respectively.
 *  @param[in]      x       value to encode
 *  @param[in]      max     encoded value representing 1.0
 *  @param[in]      qOff    offset to add before quantization
 */
inline unsigned int IntEncode(float x, unsigned int max, float qOff)
{
	return IntEncode(x+qOff/float(max),max);
}

/**
 *  Linear encoding function.
 *  This function maps a floating-point value in the range 0..1 linearly to in an integer value in the range 0..max.
 *  @note           Floating-point values outside the range 0..1 are clipped, mapping them to 0 or max, respectively.
 *  @param[in]      x       value to encode
 *  @param[in]      max     encoded value representing 1.0
 *  @param[in]      qOff    offset to add before quantization
 *  @param[out]     err     quantization error (including effects due to adding qOff)
 */
inline unsigned int IntEncode(float x, unsigned int max, float qOff, float& err)
{
	unsigned int v = IntEncode(x,max,qOff);
	err = clip(x,0.0f,1.0f) - IntDecode(v,max);
	return v;
}

/**
 *  Generic encoding function.
 *  This function maps a floating-point value in the range 0..1 to in an integer value in the range 0..max.
 *  @note           Floating-point values outside the range 0..1 (after applying the transfer function) are clipped,
 *                  mapping them to 0 or max, respectively.
 *  @param[in]      g       transfer function (gamma curve) to use
 *  @param[in]      x       value to encode
 *  @param[in]      max     encoded value representing 1.0
 */
inline unsigned int IntEncode(const GammaCurvePtr& g, float x, unsigned int max) { return IntEncode(GammaCurve::Encode(g,x),max); }

/**
 *  Generic encoding function.
 *  This function maps a floating-point value in the range 0..1 to in an integer value in the range 0..max.
 *  @note           Floating-point values outside the range 0..1 (after applying the transfer function) are clipped,
 *                  mapping them to 0 or max, respectively.
 *  @param[in]      g       transfer function (gamma curve) to use
 *  @param[in]      x       value to encode
 *  @param[in]      max     encoded value representing 1.0
 *  @param[in]      qOff    offset to add before quantization
 */
inline unsigned int IntEncode(const GammaCurvePtr& g, float x, unsigned int max, float qOff)
{
	return IntEncode(GammaCurve::Encode(g,x)+qOff/float(max),max);
}

/**
 *  Generic encoding function.
 *  This function maps a floating-point value in the range 0..1 to in an integer value in the range 0..max.
 *  @note           Floating-point values outside the range 0..1 (after applying the transfer function) are clipped,
 *                  mapping them to 0 or max, respectively.
 *  @param[in]      g       transfer function (gamma curve) to use
 *  @param[in]      x       value to encode
 *  @param[in]      max     encoded value representing 1.0
 *  @param[in]      qOff    offset to add before quantization
 *  @param[out]     err     quantization error (including effects due to adding qOff)
 */
inline unsigned int IntEncode(const GammaCurvePtr& g, float x, unsigned int max, float qOff, float& err)
{
	unsigned int v = IntEncode(g,x,max,qOff);
	err = clip(x,0.0f,1.0f) - IntDecode(g,v,max);
	return v;
}

/*******************************************************************************/

// convenience functions for image file decoding

void SetEncodedGrayValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int gray);
void SetEncodedGrayAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int gray, unsigned int alpha, bool premul = false);
void SetEncodedRGBValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int red, unsigned int green, unsigned int blue);
void SetEncodedRGBAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha, bool premul = false);
void SetEncodedGrayValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float gray);
void SetEncodedGrayAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float gray, float alpha, bool premul = false);
void SetEncodedRGBValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float red, float green, float blue);
void SetEncodedRGBAValue(Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float red, float green, float blue, float alpha, bool premul = false);

// convenience functions for image file encoding
unsigned int GetEncodedGrayValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, DitherHandler& dh);
void GetEncodedGrayAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int& gray, unsigned int& alpha, DitherHandler& dh, bool premul = false);
void GetEncodedRGBValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int& red, unsigned int& green, unsigned int& blue, DitherHandler& dh);
void GetEncodedRGBAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, unsigned int max, unsigned int& red, unsigned int& green, unsigned int& blue, unsigned int& alpha, DitherHandler& dh, bool premul = false);
float GetEncodedGrayValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&);
void GetEncodedGrayAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float& gray, float& alpha, bool premul = false);
void GetEncodedRGBValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float& red, float& green, float& blue);
void GetEncodedRGBAValue(const Image* img, unsigned int x, unsigned int y, const GammaCurvePtr&, float& red, float& green, float& blue, float& alpha, bool premul = false);

}

#endif // POVRAY_BASE_IMAGE_ENCODING_H
