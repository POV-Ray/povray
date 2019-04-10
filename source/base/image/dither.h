//******************************************************************************
///
/// @file base/image/dither.h
///
/// Declarations related to image dithering.
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

#ifndef POVRAY_BASE_DITHER_H
#define POVRAY_BASE_DITHER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"
#include "base/image/dither_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// POV-Ray header files (base module)
#include "base/colour.h"
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
/// @name Dithering
///
/// The following types and functions provide dithering functionality.
///
/// @{

enum class DitherMethodId
{
    kNone,
    kDiffusion1D,
    kSierraLite,
    kFloydSteinberg,
    kBayer2x2,
    kBayer3x3,
    kBayer4x4,
    kBlueNoise,
    kBlueNoiseX,
    kAtkinson,
    kBurkes,
    kJarvisJudiceNinke,
    kSierra2,
    kSierra3,
    kStucki,
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
        struct ColourOffset;

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

struct DitherStrategy::ColourOffset final
{
    union { ColourChannel red, gray; };
    ColourChannel green, blue, alpha;

    inline ColourOffset() : red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f) {}
    inline ColourOffset(ColourChannel r, ColourChannel g, ColourChannel b, ColourChannel a) : red(r), green(g), blue(b), alpha(a) {}
    inline void clear() { red = 0.0f; green = 0.0f; blue = 0.0f; alpha = 0.0f; }
    inline void setAll(ColourChannel v) { red = v; green = v; blue = v; alpha = v; }
    inline void setRGB(RGBColour& v) { red = v.red(); green = v.green(); blue = v.blue(); alpha = 0.0; }
    inline RGBColour getRGB() { return RGBColour(red, green, blue); }
    inline ColourOffset operator*(ColourChannel b) const { return ColourOffset(red*b, green*b, blue*b, alpha*b); }
    inline ColourOffset operator+(const ColourOffset& b) const { return ColourOffset(red + b.red, green + b.green, blue + b.blue, alpha + b.alpha); }
    inline ColourOffset& operator+=(const ColourOffset& b) { red += b.red; green += b.green; blue += b.blue; alpha += b.alpha; return *this; }
};

typedef std::shared_ptr<DitherStrategy> DitherStrategySPtr;

/// Factory function to get a dithering algorithm and state.
DitherStrategySPtr GetDitherStrategy(DitherMethodId method, unsigned int imageWidth);

/// Factory function to get a no-op dithering algorithm.
DitherStrategySPtr GetNoOpDitherStrategy();

/// Function providing simple stateless dithering.
///
/// This function is provided as a fallback from the @ref DitherStrategy mechanism to provide basic
/// dithering functionality in cases where stateful operation is impractical, such as the render
/// preview.
///
/// The current implementation is based on blue noise dithering.
///
/// @param[in]  x   Image x coordinate.
/// @param[in]  y   Image y coordinate.
/// @return         Offset to add right before quantization (even after scaling).
///
ColourChannel GetDitherOffset(unsigned int x, unsigned int y);

//-------------------------------------------------------------------------------

/// "no-op" dithering strategy.
///
/// This stateless dithering strategy serves as a placeholder when dithering
/// is not desired.
///
class NoDither final : public DitherStrategy
{
public:
    virtual void GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt) override;
};

//-------------------------------------------------------------------------------

/// Generalized ordered dithering strategy.
///
/// This stateless dithering strategy implements a generalized ordered
/// dithering filter. The specifics of the filter are defined by a matrix.
///
class OrderedDither final : public DitherStrategy
{
public:
    class Pattern;
    OrderedDither(const Pattern& matrix, unsigned int width, bool invertRB = false);
    virtual void GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt) override;
protected:
    const Pattern& mPattern;
    unsigned int mImageWidth;
    bool mInvertRB;
};

/// 2x2 Bayer ordered dithering matrix.
///
/// This matrix is based on principles proposed by B.E. Bayer in 1973.
///
extern const OrderedDither::Pattern BayerMatrix2;

/// 3x3 ordered dithering matrix.
///
extern const OrderedDither::Pattern BayerMatrix3;

/// 4x4 Bayer ordered dithering matrix.
///
/// This matrix is based on principles proposed by B.E. Bayer in 1973.
///
extern const OrderedDither::Pattern BayerMatrix4;

/// 64x64 blue noise dithering matrix.
///
/// This matrix was generated using the void-and-cluster method proposed by
/// R. Ulichney in 1993.
///
extern const OrderedDither::Pattern BlueNoise64a;

//-------------------------------------------------------------------------------

/// Simple 1D error diffusion dithering strategy.
///
/// This stateful dithering strategy implements the simplest error diffusion
/// dithering filter possible, propagating all of the quantization error to the
/// next pixel.
///
/// This dithering strategy is equivalent to the following @ref DiffusionDither::Filter:
///
///     DiffusionDither::Filter(
///         {{    1 }});
///
class DiffusionDither1D final : public DitherStrategy
{
public:
    virtual void GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt) override;
    virtual void SetError(unsigned int x, unsigned int y, const ColourOffset& err) override;
protected:
    ColourOffset lastErr;
};

//-------------------------------------------------------------------------------

/// Sierra Lite error diffusion dithering strategy.
///
/// This stateful dithering strategy implements the error diffusion dithering
/// filter proposed by F. Sierra in 1990 as "Filter Lite" (aka "Sierra Lite"
/// or "Sierra-2-4A"), distributing the quantization error non-uniformly between
/// the pixel on the right and the pixels to the bottom left and straight below.
///
/// @note   This implementation uses an additional 1-line pixel buffer to avoid manipulating the original image.
///
/// This dithering strategy is equivalent to the following @ref DiffusionDither::Filter:
///
///     DiffusionDither::Filter(
///         {{       2 },
///          { 1, 1, 0 }});
///
class SierraLiteDither final : public DitherStrategy
{
public:
    SierraLiteDither(unsigned int width);
    virtual ~SierraLiteDither() override;
    virtual void GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt) override;
    virtual void SetError(unsigned int x, unsigned int y, const ColourOffset& err) override;
protected:
    unsigned int imageWidth;
    ColourOffset* maErr;
};

//-------------------------------------------------------------------------------

/// Floyd-Steinberg error diffusion dithering strategy.
///
/// This stateful dithering strategy implements the error diffusion dithering
/// filter proposed by R.W. Floyd and L. Steinberg in 1976.
///
/// The Floyd-Steinberg filter distributes the error non-uniformly among the
/// pixel on the right as well as the three pixels below.
///
/// @note   This implementation uses an additional 1-line pixel buffer to avoid manipulating the original image.
///
/// This dithering strategy is equivalent to the following @ref DiffusionDither::Filter:
///
///     DiffusionDither::Filter(
///         {{       7 },
///          { 3, 5, 1 }});
///
class FloydSteinbergDither final : public DitherStrategy
{
public:
    FloydSteinbergDither(unsigned int width);
    virtual ~FloydSteinbergDither() override;
    virtual void GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt) override;
    virtual void SetError(unsigned int x, unsigned int y, const ColourOffset& err) override;
protected:
    unsigned int imageWidth;
    ColourOffset* maErr;
    ColourOffset mErrX;
};

//-------------------------------------------------------------------------------

/// Generalized error diffusion dithering strategy.
///
/// This stateful dithering strategy implements a generalized error diffusion
/// dithering filter. The specifics of the filter are defined by a matrix.
///
/// @note   This implementation uses an additional multi-line pixel buffer to avoid manipulating the original image.
///
class DiffusionDither final : public DitherStrategy
{
public:
    class Filter;
    DiffusionDither(const Filter& matrix, unsigned int width);
    virtual ~DiffusionDither() override;
    virtual void GetOffset(unsigned int x, unsigned int y, ColourOffset& offLin, ColourOffset& offQnt) override;
    virtual void SetError(unsigned int x, unsigned int y, const ColourOffset& err) override;
protected:
    const Filter& mMatrix;
    unsigned int mImageWidth;
    ColourOffset** maaErrorBuffer;
};

/// Atkinson error diffusion dithering matrix.
///
/// This matrix corresponds to the filter originally implemented by B. Atkinson
/// in Apple's HyperScan software.
///
/// @note   This filter propagates only 75% of the quantization error.
///
extern const DiffusionDither::Filter AtkinsonMatrix;

/// Burkes error diffusion dithering matrix.
///
/// This matrix corresponds to the filter proposed by D. Burkes in 1988,
/// distributing the quantization error across five pixel columns and three
/// pixel rows.
///
extern const DiffusionDither::Filter BurkesMatrix;

/// Jarvis-Judice-Ninke error diffusion dithering matrix.
///
/// This matrix corresponds to the filter proposed by J.F. Jarvis, C.N.Judice
/// and W.H. Ninke in 1976, distributing the quantization error across five
/// pixel columns and three pixel rows.
///
extern const DiffusionDither::Filter JarvisJudiceNinkeMatrix;

/// Two-Row Sierra error diffusion dithering matrix.
///
/// This matrix corresponds to the filter proposed by F. Sierra in 1990
/// (aka "Sierra-2"), distributing the quantization error across five pixel
/// columns and two pixel rows.
///
extern const DiffusionDither::Filter Sierra2Matrix;

/// Sierra error diffusion dithering matrix.
///
/// This matrix corresponds to the filter proposed by F. Sierra in 1989
/// (aka "Sierra-3"), distributing the quantization error across five pixel
/// columns and three pixel rows.
///
extern const DiffusionDither::Filter Sierra3Matrix;

/// Stucki error diffusion dithering matrix.
///
/// This matrix corresponds to the filter proposed by P. Stucki in 1981,
/// distributing the quantization error across five pixel columns and three
/// pixel rows.
///
extern const DiffusionDither::Filter StuckiMatrix;

/// @}
///
//*****************************************************************************

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_IMAGE_DITHER_H
