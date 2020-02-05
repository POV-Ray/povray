//******************************************************************************
///
/// @file base/colour.h
///
/// Declarations and inline implementations related to colour storage and
/// computations.
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

#ifndef POVRAY_BASE_COLOUR_H
#define POVRAY_BASE_COLOUR_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of C standard header files
#include <cmath>

// C++ standard header files
#include <algorithm>
#include <limits>

// POV-Ray header files (base module)
#include "base/base_fwd.h"
#include "base/mathutil.h"

#define NUM_COLOUR_CHANNELS 3

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBaseColour Colour Data Types
/// @ingroup PovBase
///
/// @{

typedef COLC ColourChannel;
typedef DBL  PreciseColourChannel;

#if defined(HAVE_NAN)
    template<typename T>
    inline bool ColourChannelIsValid(T c) { return !POV_ISNAN(c); }
    template<typename T>
    inline void ColourChannelInvalidate(T& c) { c = std::numeric_limits<T>::quiet_NaN(); }
#elif defined(HAVE_INF)
    template<typename T>
    inline bool ColourChannelIsValid(T c) { return !POV_ISINF(c); }
    template<typename T>
    inline void ColourChannelInvalidate(T& c) { c = -std::numeric_limits<T>::infinity(); }
#else
    template<typename T>
    inline bool ColourChannelIsValid(T c) { return c != -std::numeric_limits<ColourChannel>::max(); }
    template<typename T>
    inline void ColourChannelInvalidate(T& c) { c = -std::numeric_limits<ColourChannel>::max(); }
#endif

template<typename T>
class GenericRGBFTColour;

template<typename T>
class GenericRGBTColour;

template<typename T>
class GenericColour;

template<typename T>
class GenericTransColour;

template<int BIAS, bool QUANTIZE_TO_NEAREST, typename T = unsigned char>
class GenericRGBEColour;

/// @name Colour Channel Luminance
/// @{
/// @remark    These do not exactly match CCIR Recommendation 601-1, which specifies 0.299, 0.587 and
///            0.114 respectively.
/// @remark    We choose a high-precision type for these because otherwise Greyscale(<1,1,1>) won't
///            properly sum up to 1 on some systems.
/// @todo      For linear RGB with sRGB primaries this should be 0.2126, 0.7152 and 0.0722
///            respectively.
///
const PreciseColourChannel kRedIntensity   = 0.297;
const PreciseColourChannel kGreenIntensity = 0.589;
const PreciseColourChannel kBlueIntensity  = 0.114;
/// @}

/// Generic template class to hold and manipulate an RGB colour.
///
/// @note       This colour type is provided solely for use in the front-end and image handling code. Use
///             @ref GenericColour in the render engine instead.
///
/// @tparam T   Floating-point type to use for the individual colour components.
///
template<typename T>
class GenericRGBColour final
{
    public:

        template<typename T2>
        friend class GenericRGBColour;

        friend class GenericColour<T>;
        friend class GenericRGBTColour<T>;
        friend class GenericRGBFTColour<T>;

        friend GenericRGBColour ToRGBColour(const GenericColour<T>& col);

        /// Default constructor.
        inline GenericRGBColour()
        {
            mColour[RED]   = 0.0;
            mColour[GREEN] = 0.0;
            mColour[BLUE]  = 0.0;
        }

        /// Copy constructor.
        inline GenericRGBColour(const GenericRGBColour& col)
        {
            mColour[RED]   = col.mColour[RED];
            mColour[GREEN] = col.mColour[GREEN];
            mColour[BLUE]  = col.mColour[BLUE];
        }

        template<typename T2>
        inline explicit GenericRGBColour(const GenericRGBColour<T2>& col)
        {
            mColour[RED]   = col.mColour[RED];
            mColour[GREEN] = col.mColour[GREEN];
            mColour[BLUE]  = col.mColour[BLUE];
        }

        inline explicit GenericRGBColour(T grey)
        {
            mColour[RED]   = grey;
            mColour[GREEN] = grey;
            mColour[BLUE]  = grey;
        }

        inline explicit GenericRGBColour(T red, T green, T blue)
        {
            mColour[RED]   = red;
            mColour[GREEN] = green;
            mColour[BLUE]  = blue;
        }

        inline explicit GenericRGBColour(const GenericRGBFTColour<T>& col)
        {
            mColour[RED]   = col.red();
            mColour[GREEN] = col.green();
            mColour[BLUE]  = col.blue();
        }

        template<int BIAS, bool QUANTIZE_TO_NEAREST, typename T2>
        inline explicit GenericRGBColour(const GenericRGBEColour<BIAS,QUANTIZE_TO_NEAREST,T2>& col)
        {
            if (col.mData[GenericRGBEColour<BIAS,QUANTIZE_TO_NEAREST,T2>::EXP] > std::numeric_limits<T2>::min())
            {
                double expFactor = ldexp(1.0,(int)col.mData[GenericRGBEColour<BIAS,QUANTIZE_TO_NEAREST,T2>::EXP]-(int)(BIAS+8));
                double quantizationFix = (QUANTIZE_TO_NEAREST? 0.0 : 0.5);
                mColour[RED]   = (col.mData[GenericRGBEColour<BIAS,QUANTIZE_TO_NEAREST,T2>::RED]   + quantizationFix) * expFactor;
                mColour[GREEN] = (col.mData[GenericRGBEColour<BIAS,QUANTIZE_TO_NEAREST,T2>::GREEN] + quantizationFix) * expFactor;
                mColour[BLUE]  = (col.mData[GenericRGBEColour<BIAS,QUANTIZE_TO_NEAREST,T2>::BLUE]  + quantizationFix) * expFactor;
            }
            else
            {
                mColour[RED]   = 0.0;
                mColour[GREEN] = 0.0;
                mColour[BLUE]  = 0.0;
            }
        }
/*
        inline explicit GenericRGBColour(const GenericColour<T>& col)
        {
            mColour[RED]   = col.Red();
            mColour[GREEN] = col.Green();
            mColour[BLUE]  = col.Blue();
        }
*/
        inline GenericRGBColour& operator=(const GenericRGBColour& col)
        {
            mColour[RED]   = col.mColour[RED];
            mColour[GREEN] = col.mColour[GREEN];
            mColour[BLUE]  = col.mColour[BLUE];
            return *this;
        }

        inline T  operator[](int idx) const { return mColour[idx]; }
        inline T& operator[](int idx)       { return mColour[idx]; }

        inline T  red()   const { return mColour[RED]; }
        inline T& red()         { return mColour[RED]; }

        inline T  green() const { return mColour[GREEN]; }
        inline T& green()       { return mColour[GREEN]; }

        inline T  blue()  const { return mColour[BLUE]; }
        inline T& blue()        { return mColour[BLUE]; }

        /// Computes the greyscale intensity of the colour.
        ///
        /// @note   Do _not_ use this function if you want to compute some kind of weight; that's
        ///         what @ref WeightGreyscale() is for.
        ///
        inline T Greyscale() const
        {
            return kRedIntensity   * mColour[RED]   +
                   kGreenIntensity * mColour[GREEN] +
                   kBlueIntensity  * mColour[BLUE];
        }

        /// Computes a generic measure for the weight of the colour.
        inline T Weight() const
        {
            /// @remark This used to be implemented differently at different places in the code;
            ///         variations were:
            ///           - `max3(r,g,b)`
            ///           - `max3(std::fabs(r),std::fabs(g),std::fabs(b))`
            ///           - `std::fabs(greyscale)` [1]
            ///           - `std::max(0.0,greyscale)`
            /// @remark [1] A variant of this was `std::max(0.0,std::fabs(greyscale))`; note the superfluous
            ///             `std::max()`.
            /// @remark The rationale for choosing the current implementation is as follows:
            ///           - In general, the weight should scale proportionally with the colour
            ///             brightness. [2]
            ///           - White should have a weight of 1.0.
            ///           - The weight should be non-negative in any case.
            ///           - A change in any colour component should affect the weight, whether it is
            ///             the brightest one or not.
            ///           - Negative colour components should increase the weight.
            ///           - The individual colour components should have the same weight. [3]
            /// @remark [2] It might be argued that the weight should instead scale according to a
            ///             power law, reflecting the human visual perception of brightness;
            ///             however, this would make the weight meaningless for colour deltas.
            ///             In addition, chroma is also important and doesn't follow a power law.
            /// @remark [3] It might be argued that the individual colour components should be
            ///             weighted according to their perceived brightness; however, chroma is
            ///             also important and has entirely different weights per component.
            /// @remark For backward compatibility, @ref WeightMax(), @ref WeightMaxAbs(),
            ///         @ref WeightGreyscale() and @ref WeightAbsGreyscale() are provided.

            return (std::fabs(mColour[RED])   +
                    std::fabs(mColour[GREEN]) +
                    std::fabs(mColour[BLUE])) / 3.0;
        }

        /// Computes a measure for the weight of the colour based on the magnitude of its greyscale
        /// value.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref Weight()
        ///             for consistency of colour math.
        ///
        inline T WeightAbsGreyscale() const
        {
            return std::fabs(Greyscale());
        }

        /// Computes a measure for the weight of the colour based on its greyscale value.
        ///
        /// @note       Do _not_ use this function if you absolutely want to know the greyscale
        ///             intensity of the colour. For such cases, use @ref Greyscale() instead.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to
        ///             @ref WeightAbsGreyscale() or @ref Weight() for consistency of colour math.
        ///
        inline T WeightGreyscale() const
        {
            return Greyscale();
        }

        /// Computes a measure for the weight of the colour based on the colour channel with the
        /// greatest value.
        ///
        /// @note       Do _not_ use this function if you absolutely want to know the intensity of
        ///             the strongest colour channel. For such cases, use @ref Max() instead.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to
        ///             @ref WeightMaxAbs() or @ref Weight() for consistency of colour math.
        ///
        inline T WeightMax() const
        {
            return Max();
        }

        /// Computes a measure for the weight of the colour based on the colour channel with the
        /// greatest magnitude.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref Weight()
        ///             for consistency of colour math.
        ///
        inline T WeightMaxAbs() const
        {
            return MaxAbs();
        }

        /// Computes the intensity of the colour channel with the greatest value.
        ///
        /// @note       Do _not_ use this function if you want to compute some kind of weight;
        ///             that's what @ref WeightMax() is for.
        ///
        inline T Max() const
        {
            return max3(mColour[RED],
                        mColour[GREEN],
                        mColour[BLUE]);
        }

        /// Computes the intensity of the colour channel with the greatest magnitude.
        ///
        /// @note       Do _not_ use this function if you want to compute some kind of weight;
        ///             that's what @ref WeightMaxAbs() is for.
        ///
        inline T MaxAbs() const
        {
            return max3(std::fabs(mColour[RED]),
                        std::fabs(mColour[GREEN]),
                        std::fabs(mColour[BLUE]));
        }

        /// Computes the intensity of the colour channel with the smallest value.
        ///
        inline T Min() const
        {
            return min3(mColour[RED],
                        mColour[GREEN],
                        mColour[BLUE]);
        }

        /// Test whether the colour is valid.
        ///
        /// This function is used in combination with @ref Invalidate() -- which flags a colour as invalid -- to test
        /// whether or not a colour has been flagged in this way.
        ///
        /// The current implementation of Invalidate() sets the first channel to a (quiet) NaN for this purpose;
        /// if NaNs are unavailable or dysfunctional, the implementation falls back to using negative infinity;
        /// if infinities are unavailable or dysfunctional as well, the implementation falls back to using the negative
        /// of the maximum value representable by the @ref ColourChannel type.
        ///
        /// @return `false` if the colour has been flagged as invalid.
        ///
        inline bool IsValid() const
        {
            return ColourChannelIsValid(mColour[0]);
        }

        inline bool IsZero() const
        {
            return (mColour[RED]   == 0) &&
                   (mColour[GREEN] == 0) &&
                   (mColour[BLUE]  == 0);
        }

        inline bool IsNearZero(T epsilon) const
        {
            return (std::fabs(mColour[RED])   < epsilon) &&
                   (std::fabs(mColour[GREEN]) < epsilon) &&
                   (std::fabs(mColour[BLUE])  < epsilon);
        }

        inline void Clear()
        {
            mColour[RED]   = 0.0;
            mColour[GREEN] = 0.0;
            mColour[BLUE]  = 0.0;
        }

        /// Invalidate the colour.
        ///
        /// This function is used to flag a colour as invalid, which can later be tested for using @ref IsInvalid().
        ///
        /// The current implementation of Invalidate() sets the first channel to a (quiet) NaN for this purpose;
        /// if NaNs are unavailable or dysfunctional, the implementation falls back to using negative infinity;
        /// if infinities are unavailable or dysfunctional as well, the implementation falls back to using the negative
        /// of the maximum value representable by the @ref ColourChannel type.
        ///
        inline void Invalidate()
        {
            ColourChannelInvalidate(mColour[0]);
        }

        inline void Set(T grey)
        {
            mColour[RED]   = grey;
            mColour[GREEN] = grey;
            mColour[BLUE]  = grey;
        }

        inline void Set(T red, T green, T blue)
        {
            mColour[RED]   = red;
            mColour[GREEN] = green;
            mColour[BLUE]  = blue;
        }

        inline GenericRGBColour Clipped(T minc, T maxc) const
        {
            return GenericRGBColour(pov_base::clip<T>(mColour[RED],   minc, maxc),
                                    pov_base::clip<T>(mColour[GREEN], minc, maxc),
                                    pov_base::clip<T>(mColour[BLUE],  minc, maxc));
        }

        inline GenericRGBColour ClippedUpper(T maxc) const
        {
            return GenericRGBColour(std::min(mColour[RED],   maxc),
                                    std::min(mColour[GREEN], maxc),
                                    std::min(mColour[BLUE],  maxc));
        }

        inline GenericRGBColour ClippedLower(T minc) const
        {
            return GenericRGBColour(std::max(mColour[RED],   minc),
                                    std::max(mColour[GREEN], minc),
                                    std::max(mColour[BLUE],  minc));
        }

        inline GenericRGBColour operator+(const GenericRGBColour& b) const
        {
            return GenericRGBColour(mColour[RED]   + b.mColour[RED],
                                    mColour[GREEN] + b.mColour[GREEN],
                                    mColour[BLUE]  + b.mColour[BLUE]);
        }

        inline GenericRGBColour operator-(const GenericRGBColour& b) const
        {
            return GenericRGBColour(mColour[RED]   - b.mColour[RED],
                                    mColour[GREEN] - b.mColour[GREEN],
                                    mColour[BLUE]  - b.mColour[BLUE]);
        }

        inline GenericRGBColour operator*(const GenericRGBColour& b) const
        {
            return GenericRGBColour(mColour[RED]   * b.mColour[RED],
                                    mColour[GREEN] * b.mColour[GREEN],
                                    mColour[BLUE]  * b.mColour[BLUE]);
        }

        inline GenericRGBColour operator/(const GenericRGBColour& b) const
        {
            return GenericRGBColour(mColour[RED]   / b.mColour[RED],
                                    mColour[GREEN] / b.mColour[GREEN],
                                    mColour[BLUE]  / b.mColour[BLUE]);
        }

        inline GenericRGBColour& operator+=(const GenericRGBColour& b)
        {
            mColour[RED]   += b.mColour[RED];
            mColour[GREEN] += b.mColour[GREEN];
            mColour[BLUE]  += b.mColour[BLUE];
            return *this;
        }

        inline GenericRGBColour& operator-=(const GenericRGBColour& b)
        {
            mColour[RED]   -= b.mColour[RED];
            mColour[GREEN] -= b.mColour[GREEN];
            mColour[BLUE]  -= b.mColour[BLUE];
            return *this;
        }

        inline GenericRGBColour& operator*=(const GenericRGBColour& b)
        {
            mColour[RED]   *= b.mColour[RED];
            mColour[GREEN] *= b.mColour[GREEN];
            mColour[BLUE]  *= b.mColour[BLUE];
            return *this;
        }

        inline GenericRGBColour& operator/=(const GenericRGBColour& b)
        {
            mColour[RED]   /= b.mColour[RED];
            mColour[GREEN] /= b.mColour[GREEN];
            mColour[BLUE]  /= b.mColour[BLUE];
            return *this;
        }

        inline GenericRGBColour operator-() const
        {
            return GenericRGBColour(-mColour[RED],
                                    -mColour[GREEN],
                                    -mColour[BLUE]);
        }

        inline GenericRGBColour operator+(double b) const
        {
            return GenericRGBColour(mColour[RED]   + b,
                                    mColour[GREEN] + b,
                                    mColour[BLUE]  + b);
        }

        inline GenericRGBColour operator-(double b) const
        {
            return GenericRGBColour(mColour[RED]   - b,
                                    mColour[GREEN] - b,
                                    mColour[BLUE]  - b);
        }

        inline GenericRGBColour operator*(double b) const
        {
            return GenericRGBColour(mColour[RED]   * b,
                                    mColour[GREEN] * b,
                                    mColour[BLUE]  * b);
        }

        inline GenericRGBColour operator/(double b) const
        {
            return GenericRGBColour(mColour[RED]   / b,
                                    mColour[GREEN] / b,
                                    mColour[BLUE]  / b);
        }

        inline GenericRGBColour& operator+=(double b)
        {
            mColour[RED]   += b;
            mColour[GREEN] += b;
            mColour[BLUE]  += b;
            return *this;
        }

        inline GenericRGBColour& operator-=(double b)
        {
            mColour[RED]   -= b;
            mColour[GREEN] -= b;
            mColour[BLUE]  -= b;
            return *this;
        }

        inline GenericRGBColour& operator*=(double b)
        {
            mColour[RED]   *= b;
            mColour[GREEN] *= b;
            mColour[BLUE]  *= b;
            return *this;
        }

        inline GenericRGBColour& operator/=(double b)
        {
            mColour[RED]   /= b;
            mColour[GREEN] /= b;
            mColour[BLUE]  /= b;
            return *this;
        }

    private:

        enum
        {
            RED    = 0,
            GREEN  = 1,
            BLUE   = 2
        };

        T mColour[3];

#if (NUM_COLOUR_CHANNELS == 3)
        inline explicit GenericRGBColour(const GenericColour<T>& col)
        {
            mColour[RED]   = col.mColour[0];
            mColour[GREEN] = col.mColour[1];
            mColour[BLUE]  = col.mColour[2];
        }
#else
        #error "TODO!"
#endif
};

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> operator* (double a, const GenericRGBColour<T>& b) { return b * a; }

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> operator/ (double a, const GenericRGBColour<T>& b)
{
    return GenericRGBColour<T>(a / b.red(),
                               a / b.green(),
                               a / b.blue());
}

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> operator+ (double a, const GenericRGBColour<T>& b) { return b + a; }

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> operator- (double a, const GenericRGBColour<T>& b) { return GenericRGBColour<T>(a) - b; }

/// @relates GenericRGBColour
template<typename T>
inline T ColourDistance (const GenericRGBColour<T>& a, const GenericRGBColour<T>& b)
{
    return std::fabs(a.red()   - b.red())   +
           std::fabs(a.green() - b.green()) +
           std::fabs(a.blue()  - b.blue());
}

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> Sqr(const GenericRGBColour<T>& a) { return a * a; }

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> Exp(const GenericRGBColour<T>& a)
{
    return GenericRGBColour<T>(exp(a.red()),
                               exp(a.green()),
                               exp(a.blue()));
}

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> Pow(const GenericRGBColour<T>& a, T b)
{
    return GenericRGBColour<T>(pow(a.red(),   b),
                               pow(a.green(), b),
                               pow(a.blue(),  b));
}

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> Sqrt(const GenericRGBColour<T>& a)
{
    return GenericRGBColour<T>(sqrt(a.red()),
                               sqrt(a.green()),
                               sqrt(a.blue()));
}

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> Cos(const GenericRGBColour<T>& a)
{
    return GenericRGBColour<T>(cos(a.red()),
                               cos(a.green()),
                               cos(a.blue()));
}

typedef GenericRGBColour<ColourChannel>         RGBColour;          ///< Standard precision RGB colour.
typedef GenericRGBColour<PreciseColourChannel>  PreciseRGBColour;   ///< High precision RGB colour.


/// Generic template class to hold and manipulate an RGB colour plus a Filter and Transmit component.
///
/// @deprecated This colour type provides the legacy RGBFT transparent colour model exposed in the scene description
///             language, and should not be used anywhere else. Instead, use @ref GenericTransColour in the render
///             engine, and @ref GenericRGBTColour in the front-end.
///
/// @tparam T   Floating-point type to use for the individual colour components.
///
template<typename T>
class GenericRGBFTColour final
{
    public:

        template<typename T2>
        friend class GenericRGBFTColour;

        friend GenericRGBFTColour ToRGBFTColour(const GenericTransColour<T>& col);

        /// Default constructor.
        inline GenericRGBFTColour() :
            mColour(0.0),
            mFilter(0.0),
            mTransm(0.0)
        {}

        /// Copy constructor.
        inline GenericRGBFTColour(const GenericRGBFTColour& col) :
            mColour(col.mColour),
            mFilter(col.mFilter),
            mTransm(col.mTransm)
        {}

        template<typename T2>
        inline explicit GenericRGBFTColour(const GenericRGBFTColour<T2>& col) :
            mColour(col.mColour),
            mFilter(col.mFilter),
            mTransm(col.mTransm)
        {}

        inline explicit GenericRGBFTColour(const GenericRGBColour<T>& col) :
            mColour(col),
            mFilter(0.0),
            mTransm(0.0)
        {}

        inline explicit GenericRGBFTColour(const GenericRGBColour<T>& col, T filter, T transm) :
            mColour(col),
            mFilter(filter),
            mTransm(transm)
        {}

        inline explicit GenericRGBFTColour(const GenericRGBTColour<T>& col) :
            mColour(col.rgb()),
            mFilter(0.0),
            mTransm(col.transm())
        {}

        inline explicit GenericRGBFTColour(T red, T green, T blue, T filter, T transm) :
            mColour(red, green, blue),
            mFilter(filter),
            mTransm(transm)
        {}

        inline explicit GenericRGBFTColour(const EXPRESS& expr) :
            mColour(expr[0], expr[1], expr[2]),
            mFilter(expr[3]),
            mTransm(expr[4])
        {}

        inline GenericRGBFTColour& operator=(const GenericRGBFTColour& col)
        {
            mColour = col.mColour;
            mFilter = col.mFilter;
            mTransm = col.mTransm;
            return *this;
        }

        inline GenericRGBColour<T>  rgb() const { return mColour; }
        inline GenericRGBColour<T>& rgb()       { return mColour; }

        inline T  red()    const { return mColour.red(); }
        inline T& red()          { return mColour.red(); }

        inline T  green()  const { return mColour.green(); }
        inline T& green()        { return mColour.green(); }

        inline T  blue()   const { return mColour.blue(); }
        inline T& blue()         { return mColour.blue(); }

        inline T  filter() const { return mFilter; }
        inline T& filter()       { return mFilter; }

        inline T  transm() const { return mTransm; }
        inline T& transm()       { return mTransm; }

        inline T opacity() const { return 1.0 - mFilter - mTransm; }

        inline T Greyscale() const
        {
            return mColour.Greyscale();
        }

        // TODO: find a more correct way of handling alpha <-> filter/transmit
        inline static void AtoFT(T alpha, T& f, T& t) { f = 0.0f; t = 1.0f - alpha; }
        inline void AtoFT(T alpha) { mFilter = 0.0f; mTransm = 1.0f - alpha; }
        inline static T FTtoA(T /*f*/, T t) { return 1.0f - t; }
        inline T FTtoA() const { return 1.0f - mTransm; }

        inline void Clear()
        {
            mColour.Clear();
            mFilter = 0.0;
            mTransm = 0.0;
        }

        inline void Get(EXPRESS& expr, unsigned int n) const
        {
            if (n > 0) expr[0] = mColour.red();
            if (n > 1) expr[1] = mColour.green();
            if (n > 2) expr[2] = mColour.blue();
            if (n > 3) expr[3] = mFilter;
            if (n > 4) expr[4] = mTransm;
        }

        inline void Set(const EXPRESS& expr, unsigned int n)
        {
            if (n > 0) mColour.red()   = expr[0];
            if (n > 1) mColour.green() = expr[1];
            if (n > 2) mColour.blue()  = expr[2];
            if (n > 3) mFilter         = expr[3];
            if (n > 4) mTransm         = expr[4];
        }

        inline GenericRGBFTColour Clipped(T minc, T maxc)
        {
            return GenericRGBFTColour(mColour.Clipped(minc, maxc),
                                      pov_base::clip<T>(mFilter, minc, maxc),
                                      pov_base::clip<T>(mTransm, minc, maxc));
        }

        inline GenericRGBColour<T> TransmittedColour() const
        {
            return mColour * mFilter + mTransm;
        }

        inline GenericRGBFTColour operator+(const GenericRGBFTColour& b) const
        {
            return GenericRGBFTColour(mColour + b.mColour,
                                      mFilter + b.mFilter,
                                      mTransm + b.mTransm);
        }

        inline GenericRGBFTColour operator-(const GenericRGBFTColour& b) const
        {
            return GenericRGBFTColour(mColour - b.mColour,
                                      mFilter - b.mFilter,
                                      mTransm - b.mTransm);
        }

        inline GenericRGBFTColour operator*(const GenericRGBFTColour& b) const
        {
            return GenericRGBFTColour(mColour * b.mColour,
                                      mFilter * b.mFilter,
                                      mTransm * b.mTransm);
        }

        inline GenericRGBFTColour operator/(const GenericRGBFTColour& b) const
        {
            return GenericRGBFTColour(mColour / b.mColour,
                                      mFilter / b.mFilter,
                                      mTransm / b.mTransm);
        }

        inline GenericRGBFTColour& operator+=(const GenericRGBFTColour& b)
        {
            mColour += b.mColour;
            mFilter += b.mFilter;
            mTransm += b.mTransm;
            return *this;
        }

        inline GenericRGBFTColour& operator-=(const GenericRGBFTColour& b)
        {
            mColour -= b.mColour;
            mFilter -= b.mFilter;
            mTransm -= b.mTransm;
            return *this;
        }

        inline GenericRGBFTColour& operator*=(const GenericRGBFTColour& b)
        {
            mColour *= b.mColour;
            mFilter *= b.mFilter;
            mTransm *= b.mTransm;
            return *this;
        }

        inline GenericRGBFTColour& operator/=(const GenericRGBFTColour& b)
        {
            mColour /= b.mColour;
            mFilter /= b.mFilter;
            mTransm /= b.mTransm;
            return *this;
        }

        inline GenericRGBFTColour operator-() const
        {
            return GenericRGBFTColour(-mColour,
                                      -mFilter,
                                      -mTransm);
        }

        inline GenericRGBFTColour operator+(double b) const
        {
            return GenericRGBFTColour(mColour + b,
                                      mFilter + b,
                                      mTransm + b);
        }

        inline GenericRGBFTColour operator-(double b) const
        {
            return GenericRGBFTColour(mColour - b,
                                      mFilter - b,
                                      mTransm - b);
        }

        inline GenericRGBFTColour operator*(double b) const
        {
            return GenericRGBFTColour(mColour * b,
                                      mFilter * b,
                                      mTransm * b);
        }

        inline GenericRGBFTColour operator/(double b) const
        {
            return GenericRGBFTColour(mColour / b,
                                      mFilter / b,
                                      mTransm / b);
        }

        inline GenericRGBFTColour& operator+=(double b)
        {
            mColour += b;
            mFilter += b;
            mTransm += b;
            return *this;
        }

        inline GenericRGBFTColour& operator-=(double b)
        {
            mColour -= b;
            mFilter -= b;
            mTransm -= b;
            return *this;
        }

        inline GenericRGBFTColour& operator*=(double b)
        {
            mColour *= b;
            mFilter *= b;
            mTransm *= b;
            return *this;
        }

        inline GenericRGBFTColour& operator/=(double b)
        {
            mColour /= b;
            mFilter /= b;
            mTransm /= b;
            return *this;
        }

    private:

        GenericRGBColour<T> mColour;
        T                   mFilter;
        T                   mTransm;

        inline explicit GenericRGBFTColour(const GenericTransColour<T>& col) :
            mColour(col.colour())
        {
            col.GetFT(mFilter, mTransm);
        }
};

/// @relates GenericRGBFTColour
template<typename T>
inline GenericRGBFTColour<T> operator* (double a, const GenericRGBFTColour<T>& b) { return b * a; }

/// @relates GenericRGBFTColour
template<typename T>
inline GenericRGBFTColour<T> operator+ (double a, const GenericRGBFTColour<T>& b) { return b + a; }

/// @relates GenericRGBFTColour
template<typename T>
inline GenericRGBFTColour<T> operator- (double a, const GenericRGBFTColour<T>& b) { return GenericRGBFTColour<T>(a) - b; }

/// @relates GenericRGBFTColour
template<typename T>
inline T ColourDistanceRGBT (const GenericRGBFTColour<T>& a, const GenericRGBFTColour<T>& b)
{
    return ColourDistance(a.rgb(), b.rgb()) + std::fabs(a.transm() - b.transm());
}

typedef GenericRGBFTColour<ColourChannel>           RGBFTColour;        ///< Standard precision RGBFT colour.
typedef GenericRGBFTColour<PreciseColourChannel>    PreciseRGBFTColour; ///< High precision RGBFT colour.

/// RGB and RGBFT Colour array elements.
/// @deprecated When using @ref pov_base::GenericRGBColour, @ref pov_base::GenericRGBTColour,
///             @ref pov_base::GenericRGBFTColour or  @ref pov_base::GenericTransColour, call the
///             red(), green(), blue(), filter() and transm() access functions instead of using the
///             index operator with one of these as parameter.
enum
{
    pRED    = 0,
    pGREEN  = 1,
    pBLUE   = 2,
    pFILTER = 3,
    pTRANSM = 4
};


/// Generic template class to hold and manipulate an RGB colour plus a Transmit component.
///
/// @note       This colour type is provided solely for use in the front-end. Use @ref GenericTransColour in the render
///             engine instead.
///
/// @tparam T   Floating-point type to use for the individual colour components.
///
template<typename T>
class GenericRGBTColour final
{
    public:

        template<typename T2>
        friend class GenericRGBTColour;

        /// Default constructor.
        inline GenericRGBTColour() :
            mColour(0.0),
            mTransm(0.0)
        {}

        /// Copy constructor.
        inline GenericRGBTColour(const GenericRGBTColour& col) :
            mColour(col.mColour),
            mTransm(col.mTransm)
        {}

        template<typename T2>
        inline explicit GenericRGBTColour(const GenericRGBTColour<T2>& col) :
            mColour(col.mColour),
            mTransm(col.mTransm)
        {}

        inline explicit GenericRGBTColour(const GenericRGBColour<T>& col, T transm) :
            mColour(col),
            mTransm(transm)
        {}

        inline explicit GenericRGBTColour(T red, T green, T blue, T transm) :
            mColour(red, green, blue),
            mTransm(transm)
        {}

/*
        inline explicit GenericRGBTColour(const GenericTransColour<T>& col) :
            mColour(col.colour()),
            mTransm(col.transm())
        {}
*/

        inline GenericRGBTColour& operator=(const GenericRGBTColour& col)
        {
            mColour = col.mColour;
            mTransm = col.mTransm;
            return *this;
        }

        inline GenericRGBColour<T>  rgb() const { return mColour; }
        inline GenericRGBColour<T>& rgb()       { return mColour; }

        inline T  red()    const { return mColour.red(); }
        inline T& red()          { return mColour.red(); }

        inline T  green()  const { return mColour.green(); }
        inline T& green()        { return mColour.green(); }

        inline T  blue()   const { return mColour.blue(); }
        inline T& blue()         { return mColour.blue(); }

        inline T  transm() const { return mTransm; }
        inline T& transm()       { return mTransm; }

        inline T  alpha()  const { return 1.0 - mTransm; }

        inline T Greyscale() const
        {
            return mColour.Greyscale();
        }

        inline bool IsNearZero(T epsilon) const
        {
            return mColour.IsNearZero(epsilon) &&
                   (std::fabs(mTransm) < epsilon);
        }

        inline void Clear()
        {
            mColour.Clear();
            mTransm = 0.0;
        }

        inline GenericRGBTColour Clipped(T minc, T maxc)
        {
            return GenericRGBTColour(mColour.Clipped(minc, maxc),
                                     pov_base::clip<T>(mTransm, minc, maxc));
        }

        inline GenericRGBColour<T> TransmittedColour() const
        {
            return GenericRGBColour<T>(mTransm);
        }

        inline GenericRGBTColour operator+(const GenericRGBTColour& b) const
        {
            return GenericRGBTColour(mColour + b.mColour,
                                     mTransm + b.mTransm);
        }

        inline GenericRGBTColour operator-(const GenericRGBTColour& b) const
        {
            return GenericRGBTColour(mColour - b.mColour,
                                     mTransm - b.mTransm);
        }

        inline GenericRGBTColour operator*(const GenericRGBTColour& b) const
        {
            return GenericRGBTColour(mColour * b.mColour,
                                     mTransm * b.mTransm);
        }

        inline GenericRGBTColour operator/(const GenericRGBTColour& b) const
        {
            return GenericRGBTColour(mColour / b.mColour,
                                     mTransm / b.mTransm);
        }

        inline GenericRGBTColour& operator+=(const GenericRGBTColour& b)
        {
            mColour += b.mColour;
            mTransm += b.mTransm;
            return *this;
        }

        inline GenericRGBTColour& operator-=(const GenericRGBTColour& b)
        {
            mColour -= b.mColour;
            mTransm -= b.mTransm;
            return *this;
        }

        inline GenericRGBTColour& operator*=(const GenericRGBTColour& b)
        {
            mColour *= b.mColour;
            mTransm *= b.mTransm;
            return *this;
        }

        inline GenericRGBTColour& operator/=(const GenericRGBTColour& b)
        {
            mColour /= b.mColour;
            mTransm /= b.mTransm;
            return *this;
        }

        inline GenericRGBTColour operator-() const
        {
            return GenericRGBTColour(-mColour,
                                     -mTransm);
        }

        inline GenericRGBTColour operator+(double b) const
        {
            return GenericRGBTColour(mColour + b,
                                     mTransm + b);
        }

        inline GenericRGBTColour operator-(double b) const
        {
            return GenericRGBTColour(mColour - b,
                                     mTransm - b);
        }

        inline GenericRGBTColour operator*(double b) const
        {
            return GenericRGBTColour(mColour * b,
                                     mTransm * b);
        }

        inline GenericRGBTColour operator/(double b) const
        {
            return GenericRGBTColour(mColour / b,
                                     mTransm / b);
        }

        inline GenericRGBTColour& operator+=(double b)
        {
            mColour += b;
            mTransm += b;
            return *this;
        }

        inline GenericRGBTColour& operator-=(double b)
        {
            mColour -= b;
            mTransm -= b;
            return *this;
        }

        inline GenericRGBTColour& operator*=(double b)
        {
            mColour *= b;
            mTransm *= b;
            return *this;
        }

        inline GenericRGBTColour& operator/=(double b)
        {
            mColour /= b;
            mTransm /= b;
            return *this;
        }

    private:

        GenericRGBColour<T> mColour;
        T                   mTransm;
};

/// @relates GenericRGBTColour
template<typename T>
inline GenericRGBTColour<T> operator* (double a, const GenericRGBTColour<T>& b) { return b * a; }

/// @relates GenericRGBTColour
template<typename T>
inline GenericRGBTColour<T> operator+ (double a, const GenericRGBTColour<T>& b) { return b + a; }

/// @relates GenericRGBTColour
template<typename T>
inline GenericRGBTColour<T> operator- (double a, const GenericRGBTColour<T>& b) { return GenericRGBTColour<T>(a) - b; }

/// @relates GenericRGBTColour
template<typename T>
inline T ColourDistanceRGBT (const GenericRGBTColour<T>& a, const GenericRGBTColour<T>& b)
{
    return ColourDistance(a.rgb(), b.rgb()) + std::fabs(a.transm() - b.transm());
}

/// @relates GenericRGBTColour
template<typename T>
inline GenericRGBTColour<T> Sqr(const GenericRGBTColour<T>& a) { return a * a; }

/// @relates GenericRGBTColour
template<typename T>
inline GenericRGBTColour<T> Sqrt(const GenericRGBTColour<T>& a)
{
    return GenericRGBTColour<T>(Sqrt(a.rgb()),
                                sqrt(a.transm()));
}

typedef GenericRGBTColour<ColourChannel>        RGBTColour;         ///< Standard precision RGBxT colour.
typedef GenericRGBTColour<PreciseColourChannel> PreciseRGBTColour;  ///< High precision RGBxT colour.


/// Generic template class to hold and manipulate a colour.
///
/// @tparam T   Floating-point type to use for the individual colour channels.
///
template<typename T>
class GenericColour final
{
    public:

        static const int channels = NUM_COLOUR_CHANNELS;

        template<typename T2>
        friend class GenericColour;

        friend class GenericRGBColour<T>;
        friend class GenericRGBTColour<T>;
        friend class GenericTransColour<T>;

        friend GenericColour ToMathColour(const GenericRGBColour<T>& col);

        /// Default constructor.
        inline GenericColour()
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] = 0.0;
        }

        /// Copy constructor.
        inline GenericColour(const GenericColour& col)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] = col.mColour[i];
        }

        template<typename T2>
        inline explicit GenericColour(const GenericColour<T2>& col)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] = col.mColour[i];
        }

        inline explicit GenericColour(T grey)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] = grey;
        }

        inline explicit GenericColour(const GenericTransColour<T>& col)
        {
            const GenericColour& colour = col.colour();
            for (int i = 0; i < channels; i ++)
                mColour[i] = colour.mColour[i];
        }
/*
        template<int BIAS, typename T2>
        inline explicit GenericColour(const GenericCompactColour<BIAS,T2>& col)
        {
            double exponent = ldexp(1.0,col.mData[channels]-(int)(BIAS+8));
            for (int i = 0; i < channels; i ++)
                mColour[i] = (col.mData[i] + 0.5) * exponent;
        }
*/
        inline GenericColour& operator=(const GenericColour& col)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] = col.mColour[i];
            return *this;
        }

        inline T  operator[](int idx) const { return mColour[idx]; }
        inline T& operator[](int idx)       { return mColour[idx]; }

/*
        inline T  red()   const { return mColour[RED]; }
        inline T& red()         { return mColour[RED]; }

        inline T  green() const { return mColour[GREEN]; }
        inline T& green()       { return mColour[GREEN]; }

        inline T  blue()  const { return mColour[BLUE]; }
        inline T& blue()        { return mColour[BLUE]; }
*/

        inline T Red() const
        {
#if (NUM_COLOUR_CHANNELS == 3)
            return mColour[0];
#else
            T result = 0.0;
            for (int i = 0; i < channels; i ++)
                result += GenericColour<T>::mkR[i] * mColour[i];
            return result;
#endif
        }

        inline T Green() const
        {
#if (NUM_COLOUR_CHANNELS == 3)
            return mColour[1];
#else
            T result = 0.0;
            for (int i = 0; i < channels; i ++)
                result += GenericColour<T>::mkG[i] * mColour[i];
            return result;
#endif
        }

        inline T Blue() const
        {
#if (NUM_COLOUR_CHANNELS == 3)
            return mColour[2];
#else
            T result = 0.0;
            for (int i = 0; i < channels; i ++)
                result += GenericColour<T>::mkB[i] * mColour[i];
            return result;
#endif
        }

        /// Computes the greyscale intensity of the colour.
        ///
        /// @note   Do _not_ use this function if you want to compute some kind of weight; that's
        ///         what @ref WeightGreyscale() is for.
        ///
        inline T Greyscale() const
        {
#if (NUM_COLOUR_CHANNELS == 3)
            return kRedIntensity   * mColour[0] +
                   kGreenIntensity * mColour[1] +
                   kBlueIntensity  * mColour[2];
#else
            T result;
            for (int i = 0; i < channels; i ++)
                result += mkY[i] * mColour[i];
            return result;
#endif
        }

        /// Computes a generic measure for the weight of the colour.
        inline T Weight() const
        {
            /// @remark This used to be implemented differently at different places in the code;
            ///         variations were:
            ///           - `max3(r,g,b)`
            ///           - `max3(std::fabs(r),std::fabs(g),std::fabs(b))`
            ///           - `std::fabs(greyscale)` [1]
            ///           - `std::max(0.0,greyscale)`
            /// @remark [1] A variant of this was `std::max(0.0,std::fabs(greyscale))`; note the superfluous
            ///             `std::max()`.
            /// @remark The rationale for choosing the current implementation is as follows:
            ///           - In general, the weight should scale proportionally with the colour
            ///             brightness. [2]
            ///           - White should have a weight of 1.0.
            ///           - The weight should be non-negative in any case.
            ///           - A change in any colour component should affect the weight, whether it is
            ///             the brightest one or not.
            ///           - Negative colour components should increase the weight.
            ///           - The individual colour components should have the same weight. [3]
            /// @remark [2] It might be argued that the weight should instead scale according to a
            ///             power law, reflecting the human visual perception of brightness;
            ///             however, this would make the weight meaningless for colour deltas.
            ///             In addition, chroma is also important and doesn't follow a power law.
            /// @remark [3] It might be argued that the individual colour components should be
            ///             weighted according to their perceived brightness; however, chroma is
            ///             also important and has entirely different weights per component.
            /// @remark For backward compatibility, @ref WeightMax(), @ref WeightMaxAbs(),
            ///         @ref WeightGreyscale() and @ref WeightAbsGreyscale() are provided.

            return SumAbs() / 3.0;
        }

        /// Computes a measure for the weight of the colour based on the magnitude of its greyscale
        /// value.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref Weight()
        ///             for consistency of colour math.
        ///
        inline T WeightAbsGreyscale() const
        {
            return std::fabs(Greyscale());
        }

        /// Computes a measure for the weight of the colour based on its greyscale value.
        ///
        /// @note       Do _not_ use this function if you absolutely want to know the greyscale
        ///             intensity of the colour. For such cases, use @ref Greyscale() instead.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to
        ///             @ref WeightAbsGreyscale() or @ref Weight() for consistency of colour math.
        ///
        inline T WeightGreyscale() const
        {
            return Greyscale();
        }

        /// Computes a measure for the weight of the colour based on the colour channel with the
        /// greatest value.
        ///
        /// @note       Do _not_ use this function if you absolutely want to know the intensity of
        ///             the strongest colour channel. For such cases, use @ref Max() instead.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to
        ///             @ref WeightMaxAbs() or @ref Weight() for consistency of colour math.
        ///
        inline T WeightMax() const
        {
            return Max();
        }

        /// Computes a measure for the weight of the colour based on the colour channel with the
        /// greatest magnitude.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref Weight()
        ///             for consistency of colour math.
        ///
        inline T WeightMaxAbs() const
        {
            return MaxAbs();
        }

        /// Computes the sum of the channels' magnitudes.
        inline T SumAbs() const
        {
            T result = 0.0;
            for (int i = 0; i < channels; i ++)
                result += std::fabs(mColour[i]);
            return result;
        }

        /// Computes the intensity of the colour channel with the greatest value.
        ///
        /// @note       Do _not_ use this function if you want to compute some kind of weight;
        ///             that's what @ref WeightMax() is for.
        ///
        inline T Max() const
        {
            T result = mColour[0];
            for (int i = 1; i < channels; i ++)
                result = std::max(result, mColour[i]);
            return result;
        }

        /// Computes the intensity of the colour channel with the greatest magnitude.
        ///
        /// @note       Do _not_ use this function if you want to compute some kind of weight;
        ///             that's what @ref WeightMaxAbs() is for.
        ///
        inline T MaxAbs() const
        {
            T result = mColour[0];
            for (int i = 1; i < channels; i ++)
                result = std::max(result, std::fabs(mColour[i]));
            return result;
        }

        /// Computes the intensity of the colour channel with the smallest value.
        ///
        inline T Min() const
        {
            T result = mColour[0];
            for (int i = 1; i < channels; i ++)
                result = std::min(result, mColour[i]);
            return result;
        }

        inline bool IsValid() const
        {
            return ColourChannelIsValid(mColour[0]);
        }

        inline bool IsZero() const
        {
            bool result = true;
            for (int i = 0; i < channels; i ++)
                result = result && (mColour[i] == 0.0);
            return result;
        }

        inline bool IsNearZero(T epsilon) const
        {
            bool result = true;
            for (int i = 0; i < channels; i ++)
                result = result && (std::fabs(mColour[i]) < epsilon);
            return result;
        }

        inline void Clear()
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] = 0.0;
        }

        inline void Invalidate()
        {
            ColourChannelInvalidate(mColour[0]);
        }

        inline void Set(T grey)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] = grey;
        }
/*
        inline void Set(T red, T green, T blue)
        {
            mColour[RED]   = red;
            mColour[GREEN] = green;
            mColour[BLUE]  = blue;
        }
*/
        inline GenericColour Clipped(T minc, T maxc) const
        {
            GenericColour result;
            for (int i = 0; i < channels; i ++)
                result.mColour[i] = pov_base::clip<T>(mColour[i], minc, maxc);
            return result;
        }

        inline GenericColour ClippedUpper(T maxc) const
        {
            GenericColour result;
            for (int i = 0; i < channels; i ++)
                result.mColour[i] = std::min(mColour[i], maxc);
            return result;
        }

        inline GenericColour ClippedLower(T minc) const
        {
            GenericColour result;
            for (int i = 0; i < channels; i ++)
                result.mColour[i] = std::max(mColour[i], minc);
            return result;
        }

        inline GenericColour operator+(const GenericColour& b) const
        {
            GenericColour result = *this;
            result += b;
            return result;
        }

        inline GenericColour operator-(const GenericColour& b) const
        {
            GenericColour result = *this;
            result -= b;
            return result;
        }

        inline GenericColour operator*(const GenericColour& b) const
        {
            GenericColour result = *this;
            result *= b;
            return result;
        }

        inline GenericColour operator/(const GenericColour& b) const
        {
            GenericColour result = *this;
            result /= b;
            return result;
        }

        inline GenericColour& operator+=(const GenericColour& b)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] += b.mColour[i];
            return *this;
        }

        inline GenericColour& operator-=(const GenericColour& b)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] -= b.mColour[i];
            return *this;
        }

        inline GenericColour& operator*=(const GenericColour& b)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] *= b.mColour[i];
            return *this;
        }

        inline GenericColour& operator/=(const GenericColour& b)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] /= b.mColour[i];
            return *this;
        }

        inline GenericColour operator-() const
        {
            GenericColour result;
            result -= *this;
            return result;
        }

        inline GenericColour operator+(double b) const
        {
            GenericColour result = *this;
            result += b;
            return result;
        }

        inline GenericColour operator-(double b) const
        {
            GenericColour result = *this;
            result -= b;
            return result;
        }

        inline GenericColour operator*(double b) const
        {
            GenericColour result = *this;
            result *= b;
            return result;
        }

        inline GenericColour operator/(double b) const
        {
            GenericColour result = *this;
            result /= b;
            return result;
        }

        inline GenericColour& operator+=(double b)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] += b;
            return *this;
        }

        inline GenericColour& operator-=(double b)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] -= b;
            return *this;
        }

        inline GenericColour& operator*=(double b)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] *= b;
            return *this;
        }

        inline GenericColour& operator/=(double b)
        {
            for (int i = 0; i < channels; i ++)
                mColour[i] /= b;
            return *this;
        }

        inline GenericColour Exp() const
        {
            GenericColour result;
            for (int i = 0; i < channels; i ++)
                result.mColour[i] = exp(mColour[i]);
            return result;
        }

        inline GenericColour Pow(double b) const
        {
            GenericColour result;
            for (int i = 0; i < channels; i ++)
                result.mColour[i] = pow(mColour[i], T(b));
            return result;
        }

        inline GenericColour Cos() const
        {
            GenericColour result;
            for (int i = 0; i < channels; i ++)
                result.mColour[i] = cos(mColour[i]);
            return result;
        }

        inline GenericColour Sqrt() const
        {
            GenericColour result;
            for (int i = 0; i < channels; i ++)
                result.mColour[i] = sqrt(mColour[i]);
            return result;
        }

        inline static const GenericColour& DefaultWavelengths()
        {
            return mkDefaultWavelengths;
        }

    private:

        static const GenericColour mkDefaultWavelengths;

#if (NUM_COLOUR_CHANNELS != 3)

        static const T mkY[channels];
        static const GenericRGBColour<T> mkRGB[channels];

#endif

        T mColour[channels];

#if (NUM_COLOUR_CHANNELS == 3)
        inline explicit GenericColour(const GenericRGBColour<T>& col)
        {
            mColour[0] = col.red();
            mColour[1] = col.green();
            mColour[2] = col.blue();
        }
#else
        #error "TODO!"
#endif
};


/// @relates GenericColour
template<typename T>
inline GenericColour<T> operator* (double a, const GenericColour<T>& b) { return b * a; }

/// @relates GenericColour
template<typename T>
inline GenericColour<T> operator/ (double a, const GenericColour<T>& b) { return GenericColour<T>(a) / b; }

/// @relates GenericColour
template<typename T>
inline GenericColour<T> operator+ (double a, const GenericColour<T>& b) { return b + a; }

/// @relates GenericColour
template<typename T>
inline GenericColour<T> operator- (double a, const GenericColour<T>& b) { return GenericColour<T>(a) - b; }

/// @relates GenericColour
template<typename T>
inline T ColourDistance (const GenericColour<T>& a, const GenericColour<T>& b) { return (a - b).SumAbs(); }

/// @relates GenericColour
template<typename T>
inline GenericColour<T> Sqr(const GenericColour<T>& a) { return a * a; }

/// @relates GenericColour
template<typename T>
inline GenericColour<T> Exp(const GenericColour<T>& a) { return a.Exp(); }

/// @relates GenericColour
template<typename T>
inline GenericColour<T> Pow(const GenericColour<T>& a, T b) { return a.Pow(b); }

/// @relates GenericColour
template<typename T>
inline GenericColour<T> Sqrt(const GenericColour<T>& a) { return a.Sqrt(); }

/// @relates GenericColour
template<typename T>
inline GenericColour<T> Cos(const GenericColour<T>& a) { return a.Cos(); }

typedef GenericColour<ColourChannel>         MathColour;        ///< Standard precision colour.
typedef GenericColour<PreciseColourChannel>  PreciseMathColour; ///< High precision colour.

extern template class GenericColour<ColourChannel>;
extern template class GenericColour<PreciseColourChannel>;


/// Generic template class to hold and manipulate a colour plus transparency information.
///
/// @note   The current implementation uses RGBFT format; future implementations may vary.
///
/// @tparam T   Floating-point type to use for the individual colour components.
///
template<typename T>
class GenericTransColour final
{
    public:

        template<typename T2>
        friend class GenericTransColour;

        friend class GenericRGBFTColour<T>;

        friend GenericTransColour ToTransColour(const GenericRGBFTColour<T>& col);

        /// Default constructor.
        inline GenericTransColour() :
            mColour(0.0),
            mFilter(0.0),
            mTransm(0.0)
        {}

        /// Copy constructor.
        inline GenericTransColour(const GenericTransColour& col) :
            mColour(col.mColour),
            mFilter(col.mFilter),
            mTransm(col.mTransm)
        {}

        template<typename T2>
        inline explicit GenericTransColour(const GenericTransColour<T2>& col) :
            mColour(col.mColour),
            mFilter(col.mFilter),
            mTransm(col.mTransm)
        {}

        inline explicit GenericTransColour(const GenericColour<T>& col) :
            mColour(col),
            mFilter(0.0),
            mTransm(0.0)
        {}

        inline explicit GenericTransColour(const GenericColour<T>& col, T filter, T transm) :
            mColour(col),
            mFilter(filter),
            mTransm(transm)
        {}

        inline GenericTransColour& operator=(const GenericTransColour& col)
        {
            mColour = col.mColour;
            mFilter = col.mFilter;
            mTransm = col.mTransm;
            return *this;
        }

        inline GenericColour<T>  colour() const { return mColour; }
        inline GenericColour<T>& colour()       { return mColour; }

        inline T  filter() const { return mFilter; }
        inline T& filter()       { return mFilter; }

        inline T  transm() const { return mTransm; }
        inline T& transm()       { return mTransm; }

        inline T Opacity() const { return 1.0 - mFilter - mTransm; }

        /// Legacy opacity computation.
        ///
        /// @deprecated This fomula was used instead of @ref Opacity() in POV-Ray v3.6 and earlier texture computations.
        ///             Do not use it - it is bogus, and we're only keeping it around for compatibility with legacy
        ///             scenes.
        ///
        inline T LegacyOpacity() const { return (1.0 - (mFilter * mColour.Max() + mTransm)); }

        inline T Greyscale() const
        {
            return mColour.Greyscale();
        }

        inline bool IsValid() const
        {
            return (mColour.IsValid());
        }

        inline void Clear()
        {
            mColour.Clear();
            mFilter = 0.0;
            mTransm = 0.0;
        }

        inline void Invalidate()
        {
            mColour.Invalidate();
        }

        inline GenericTransColour Clipped(T minc, T maxc)
        {
            return GenericTransColour(mColour.Clipped(minc, maxc),
                                      pov_base::clip<T>(mFilter, minc, maxc),
                                      pov_base::clip<T>(mTransm, minc, maxc));
        }

        inline GenericColour<T> TransmittedColour() const
        {
            return mColour * mFilter + mTransm;
        }

        inline void SetFT(ColourChannel f, ColourChannel t)
        {
            mFilter = f;
            mTransm = t;
        }

        inline void GetFT(ColourChannel& f, ColourChannel& t) const
        {
            f = mFilter;
            t = mTransm;
        }

        inline GenericTransColour operator+(const GenericTransColour& b) const
        {
            return GenericTransColour(mColour + b.mColour,
                                      mFilter + b.mFilter,
                                      mTransm + b.mTransm);
        }

        inline GenericTransColour operator-(const GenericTransColour& b) const
        {
            return GenericTransColour(mColour - b.mColour,
                                      mFilter - b.mFilter,
                                      mTransm - b.mTransm);
        }

        inline GenericTransColour operator*(const GenericTransColour& b) const
        {
            return GenericTransColour(mColour * b.mColour,
                                      mFilter * b.mFilter,
                                      mTransm * b.mTransm);
        }

        inline GenericTransColour operator/(const GenericTransColour& b) const
        {
            return GenericTransColour(mColour / b.mColour,
                                      mFilter / b.mFilter,
                                      mTransm / b.mTransm);
        }

        inline GenericTransColour& operator+=(const GenericTransColour& b)
        {
            mColour += b.mColour;
            mFilter += b.mFilter;
            mTransm += b.mTransm;
            return *this;
        }

        inline GenericTransColour& operator-=(const GenericTransColour& b)
        {
            mColour -= b.mColour;
            mFilter -= b.mFilter;
            mTransm -= b.mTransm;
            return *this;
        }

        inline GenericTransColour& operator*=(const GenericTransColour& b)
        {
            mColour *= b.mColour;
            mFilter *= b.mFilter;
            mTransm *= b.mTransm;
            return *this;
        }

        inline GenericTransColour& operator/=(const GenericTransColour& b)
        {
            mColour /= b.mColour;
            mFilter /= b.mFilter;
            mTransm /= b.mTransm;
            return *this;
        }

        inline GenericTransColour operator-() const
        {
            return GenericTransColour(-mColour,
                                      -mFilter,
                                      -mTransm);
        }

        inline GenericTransColour operator+(double b) const
        {
            return GenericTransColour(mColour + b,
                                      mFilter + b,
                                      mTransm + b);
        }

        inline GenericTransColour operator-(double b) const
        {
            return GenericTransColour(mColour - b,
                                      mFilter - b,
                                      mTransm - b);
        }

        inline GenericTransColour operator*(double b) const
        {
            return GenericTransColour(mColour * b,
                                      mFilter * b,
                                      mTransm * b);
        }

        inline GenericTransColour operator/(double b) const
        {
            return GenericTransColour(mColour / b,
                                      mFilter / b,
                                      mTransm / b);
        }

        inline GenericTransColour& operator+=(double b)
        {
            mColour += b;
            mFilter += b;
            mTransm += b;
            return *this;
        }

        inline GenericTransColour& operator-=(double b)
        {
            mColour -= b;
            mFilter -= b;
            mTransm -= b;
            return *this;
        }

        inline GenericTransColour& operator*=(double b)
        {
            mColour *= b;
            mFilter *= b;
            mTransm *= b;
            return *this;
        }

        inline GenericTransColour& operator/=(double b)
        {
            mColour /= b;
            mFilter /= b;
            mTransm /= b;
            return *this;
        }

    private:

        GenericColour<T>    mColour;
        T                   mFilter;
        T                   mTransm;

        inline explicit GenericTransColour(const GenericRGBFTColour<T>& col) :
            mColour(col.rgb())
        {
            SetFT(col.filter(), col.transm());
        }
};

/// @relates GenericTransColour
template<typename T>
inline GenericTransColour<T> operator* (double a, const GenericTransColour<T>& b) { return b * a; }

/// @relates GenericTransColour
template<typename T>
inline GenericTransColour<T> operator+ (double a, const GenericTransColour<T>& b) { return b + a; }

/// @relates GenericTransColour
template<typename T>
inline GenericTransColour<T> operator- (double a, const GenericTransColour<T>& b) { return GenericTransColour<T>(a) - b; }

typedef GenericTransColour<ColourChannel>           TransColour;        ///< Standard precision transparent colour.
typedef GenericTransColour<PreciseColourChannel>    PreciseTransColour; ///< High precision transparent colour.


/// Generic template class to store a RGB colour in four bytes (RGBE).
///
/// This class uses RGBE format for compact storage of high dynamic range colours, as originally
/// proposed by Greg Ward.
///
/// @author Christoph Lipka
/// @author Based on MegaPOV HDR code written by Mael and Christoph Hormann
///
/// @tparam BIAS                Bias to use for the exponent.
///                             A value of 128 matches Greg Ward's original proposal.
/// @tparam QUANTIZE_TO_NEAREST Whether quantization should use round-to-nearest mode, as opposed to rounding
///                             towards zero and compensating upon decoding as in Greg Ward's original proposal.
/// @tparam T                   Type to use for the colour components.
///                             Defaults to unsigned char.
///
template<int BIAS, bool QUANTIZE_TO_NEAREST, typename T>
class GenericRGBEColour final
{
    public:

        template<typename T2>
        friend class GenericRGBColour;

        enum
        {
            RED    = 0,
            GREEN  = 1,
            BLUE   = 2,
            EXP    = 3
        };

        typedef T DATA[4];

        inline GenericRGBEColour()
        {
            SetToZero(mData);
        }

        inline GenericRGBEColour(const GenericRGBEColour& col)
        {
            mData[RED]   = col.mData[RED];
            mData[GREEN] = col.mData[GREEN];
            mData[BLUE]  = col.mData[BLUE];
            mData[EXP]   = col.mData[EXP];
        }

        inline explicit GenericRGBEColour(ColourChannel red, ColourChannel green, ColourChannel blue)
        {
            if (QUANTIZE_TO_NEAREST)
                Quantize(mData, RGBColour(red, green, blue), RGBColour(0.5));
            else
                Quantize(mData, RGBColour(red, green, blue));
        }

        inline explicit GenericRGBEColour(const RGBColour& col)
        {
            if (QUANTIZE_TO_NEAREST)
                Quantize(mData, col, RGBColour(0.5));
            else
                Quantize(mData, col);
        }

        inline explicit GenericRGBEColour(const RGBColour& col, const RGBColour& dither)
        {
            if (QUANTIZE_TO_NEAREST)
                Quantize(mData, col, RGBColour(0.5) + dither);
            else
                Quantize(mData, col, dither);
        }

        inline const DATA& operator*() const
        {
            return mData;
        }

        inline DATA& operator*()
        {
            return mData;
        }

    private:

        DATA mData;

        inline static bool ComputeExponent(const RGBColour& col, T& biasedExponent, double& scaleFactor)
        {
            // Determine the magnitude of the colour value.
            ColourChannel maxChannel = (std::numeric_limits<T>::is_signed ? col.MaxAbs() : col.Max());

            if (maxChannel <= 1.0e-32) // TODO - magic number
                return false;

            int exponent;
            double maxChannelMantissa = frexp(maxChannel, &exponent);
            biasedExponent = clipToType<T>(exponent + BIAS);
            scaleFactor = ldexp(std::numeric_limits<T>::max() + 1.0, BIAS-biasedExponent);
            return true;
        }

        inline static void SetToZero(DATA& data)
        {
            data[RED] = data[GREEN] = data[BLUE] = 0;
            data[EXP] = std::numeric_limits<T>::min();
        }

        /// @param[out] data    The quantized data.
        /// @param[in]  col     The colour to quantize.
        inline static void Quantize(DATA& data, const RGBColour& col)
        {
            double scaleFactor;
            if (ComputeExponent(col, data[EXP], scaleFactor))
            {
                RGBColour colMantissa = col * scaleFactor;
                data[RED]   = clipToType<T>(floor(colMantissa.red()));
                data[GREEN] = clipToType<T>(floor(colMantissa.green()));
                data[BLUE]  = clipToType<T>(floor(colMantissa.blue()));
            }
            else
                SetToZero(data);
        }

        /// @param[out] data    The quantized data.
        /// @param[in]  col     The colour to quantize.
        /// @param[in]  encOff  An offset to add to the mantissa before quantization.
        inline static void Quantize(DATA& data, const RGBColour& col, const RGBColour& encOff)
        {
            double scaleFactor;
            if (ComputeExponent(col, data[EXP], scaleFactor))
            {
                RGBColour colMantissa = col * scaleFactor + encOff;

                // The additional encoding offset might have resulted in one of the mantissas to exceed the maximum,
                // or make all drop below half the maximum; in both cases we want to adjust the exponent and mantissas
                // accordingly.
                ColourChannel maxChannel = (std::numeric_limits<T>::is_signed ? colMantissa.MaxAbs() : colMantissa.Max());
                if (maxChannel > std::numeric_limits<T>::max())
                {
                    if (data[EXP] < std::numeric_limits<T>::max())
                    {
                        data[EXP] ++;
                        scaleFactor *= 0.5;
                        colMantissa = col * scaleFactor + encOff;
                    }
                }
                else if (maxChannel * 2.0 <= std::numeric_limits<T>::max())
                {
                    if (data[EXP] > std::numeric_limits<T>::min())
                    {
                        data[EXP] --;
                        scaleFactor *= 2.0;
                        colMantissa = col * scaleFactor + encOff;
                    }
                }

                data[RED]   = clipToType<T>(floor(colMantissa.red()));
                data[GREEN] = clipToType<T>(floor(colMantissa.green()));
                data[BLUE]  = clipToType<T>(floor(colMantissa.blue()));
            }
            else
                SetToZero(data);
        }
};

typedef GenericRGBEColour<128,false>    RadianceHDRColour;  ///< RGBE format as originally proposed by Greg Ward.
typedef GenericRGBEColour<250,true>     PhotonColour;       ///< RGBE format as adapted by Nathan Kopp for photon mapping.

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_COLOUR_H
