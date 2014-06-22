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
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2014 Persistence of Vision Raytracer Pty. Ltd.
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

#include <cmath>

#include "base/configbase.h"
#include "base/types.h"

namespace pov_base
{

typedef COLC ColourChannel;
typedef DBL  PreciseColourChannel;

template<typename T>
class GenericRGBFTColour;

template<typename T>
class GenericRGBTColour;

template<typename T>
class GenericTransColour;

template<int BIAS, typename T = unsigned char>
class GenericRGBEColour;

/// @name Colour Channel Luminance
/// @{
/// @remark    These do not exactly match CCIR Recommendation 601-1, which specifies 0.299, 0.587 and
///            0.114 respectively.
/// @todo      For linear RGB with sRGB primaries this should be 0.2126, 0.7152 and 0.0722
///            respectively.
///
const float kRedIntensity   = 0.297;
const float kGreenIntensity = 0.589;
const float kBlueIntensity  = 0.114;
/// @}

/// Generic template class to hold and manipulate an RGB colour.
///
/// @tparam T   Floating-point type to use for the individual colour components.
///
template<typename T>
class GenericRGBColour
{
    public:

        template<typename T2>
        friend class GenericRGBColour;

        typedef T DATA[3];

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

        inline explicit GenericRGBColour(const GenericTransColour<T>& col)
        {
            const GenericRGBColour& colour = col.colour();
            mColour[RED]   = colour.mColour[RED];
            mColour[GREEN] = colour.mColour[GREEN];
            mColour[BLUE]  = colour.mColour[BLUE];
        }

        template<int BIAS, typename T2>
        inline explicit GenericRGBColour(const GenericRGBEColour<BIAS,T2>& col)
        {
            double exponent = ldexp(1.0,col.mData[GenericRGBEColour<BIAS>::EXP]-(int)(BIAS+8));
            mColour[RED]   = (col.mData[GenericRGBEColour<BIAS>::RED]   + 0.5) * exponent;
            mColour[GREEN] = (col.mData[GenericRGBEColour<BIAS>::GREEN] + 0.5) * exponent;
            mColour[BLUE]  = (col.mData[GenericRGBEColour<BIAS>::BLUE]  + 0.5) * exponent;
        }

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
            ///           - `max3(fabs(r),fabs(g),fabs(b))`
            ///           - `fabs(greyscale)` [1]
            ///           - `max(0.0,greyscale)`
            /// @remark [1] A variant of this was `max(0.0,fabs(greyscale))`; note the superfluous
            ///             `max()`.
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
            ///         @ref WeightGreyscale() and @ref WeightGreyscaleAbs() are provided.

            return (fabs(mColour[RED])   +
                    fabs(mColour[GREEN]) +
                    fabs(mColour[BLUE])) / 3.0;
        }

        /// Computes a measure for the weight of the colour based on the magnitude of its greyscale
        /// value.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref Weight()
        ///             for consistency of colour math.
        ///
        inline T WeightGreyscaleAbs() const
        {
            return fabs(Greyscale());
        }

        /// Computes a measure for the weight of the colour based on its greyscale value.
        ///
        /// @note       Do _not_ use this function if you absolutely want to know the greyscale
        ///             intensity of the colour. For such cases, use @ref Greyscale() instead.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to
        ///             @ref WeightGreyscaleAbs() or @ref Weight() for consistency of colour math.
        ///
        inline T WeightGreyscale() const
        {
            return Greyscale();
        }

        /// Computes a measure for the weight of the colour based on the colour channel with the
        /// greatest value.
        ///
        /// @note       Do _not_ use this function if you absolutely want to know the intensity of
        ///             the strongest colour channel. For such cases, use @ref max() instead.
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
            return max3(fabs(mColour[RED]),
                        fabs(mColour[GREEN]),
                        fabs(mColour[BLUE]));
        }

        /// Computes the intensity of the colour channel with the smallest value.
        ///
        inline T Min() const
        {
            return min3(mColour[RED],
                        mColour[GREEN],
                        mColour[BLUE]);
        }

        /// Test if all components of the colour are valid.
        inline bool IsValid() const
        {
            // test whether any component is NaN, exploiting the property that
            // a NaN always compares non-equal, even to itself
            return ((mColour[RED]   == mColour[RED])   &&
                    (mColour[GREEN] == mColour[GREEN]) &&
                    (mColour[BLUE]  == mColour[BLUE]));
        }

        inline bool IsZero() const
        {
            return (mColour[RED]   == 0) &&
                   (mColour[GREEN] == 0) &&
                   (mColour[BLUE]  == 0);
        }

        inline bool IsNearZero(T epsilon) const
        {
            return (fabs(mColour[RED])   < epsilon) &&
                   (fabs(mColour[GREEN]) < epsilon) &&
                   (fabs(mColour[BLUE])  < epsilon);
        }

        inline void Clear()
        {
            mColour[RED]   = 0.0;
            mColour[GREEN] = 0.0;
            mColour[BLUE]  = 0.0;
        }

        inline void Invalidate()
        {
            mColour[RED]   = std::numeric_limits<T>::quiet_NaN();
            mColour[GREEN] = std::numeric_limits<T>::quiet_NaN();
            mColour[BLUE]  = std::numeric_limits<T>::quiet_NaN();
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

        inline GenericRGBColour Clipped(T minc, T maxc)
        {
            return GenericRGBColour(pov_base::clip<T>(mColour[RED],   minc, maxc),
                                    pov_base::clip<T>(mColour[GREEN], minc, maxc),
                                    pov_base::clip<T>(mColour[BLUE],  minc, maxc));
        }

        inline GenericRGBColour ClippedUpper(T maxc)
        {
            return GenericRGBColour(min(mColour[RED],   maxc),
                                    min(mColour[GREEN], maxc),
                                    min(mColour[BLUE],  maxc));
        }

        inline GenericRGBColour ClippedLower(T minc)
        {
            return GenericRGBColour(max(mColour[RED],   minc),
                                    max(mColour[GREEN], minc),
                                    max(mColour[BLUE],  minc));
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

        DATA mColour;
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
    return fabs(a.red()   - b.red())   +
           fabs(a.green() - b.green()) +
           fabs(a.blue()  - b.blue());
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

/*
typedef GenericRGBColour<ColourChannel>         Colour;             ///< Standard precision colour.
typedef GenericRGBColour<PreciseColourChannel>  PreciseColour;      ///< High precision colour.
*/


/// Generic template class to hold and manipulate an RGB colour plus a Filter and Transmit component.
///
/// @deprecated This colour type provides the legacy RGBFT transparent colour model exposed in the scene description
///             language, and should not be used anywhere else. Instead, use @ref GenericTransColour in the render
///             engine, and @ref GenericRGBTColour in the front-end.
///
/// @tparam T   Floating-point type to use for the individual colour components.
///
template<typename T>
class GenericRGBFTColour
{
    public:

        template<typename T2>
        friend class GenericRGBFTColour;

        typedef DBL EXPRESS[5];

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

        inline explicit GenericRGBFTColour(const EXPRESS expr) :
            mColour(expr[0], expr[1], expr[2]),
            mFilter(expr[3]),
            mTransm(expr[4])
        {}

        inline explicit GenericRGBFTColour(const GenericTransColour<T>& col) :
            mColour(col.colour()),
            mFilter(col.filter()),
            mTransm(col.transm())
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

        /// Test if all components of the colour are valid.
        inline bool IsValid() const
        {
            // test whether any component is NaN, exploiting the property that
            // a NaN always compares non-equal, even to itself
            return ((mColour.IsValid())  &&
                    (mFilter == mFilter) &&
                    (mTransm == mTransm));
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

        inline void Invalidate()
        {
            mColour.Invalidate();
            mFilter = std::numeric_limits<T>::quiet_NaN();
            mTransm = std::numeric_limits<T>::quiet_NaN();
        }

        inline void Get(EXPRESS expr, unsigned int n) const
        {
            if (n > 0) expr[0] = mColour.red();
            if (n > 1) expr[1] = mColour.green();
            if (n > 2) expr[2] = mColour.blue();
            if (n > 3) expr[3] = mFilter;
            if (n > 4) expr[4] = mTransm;
        }

        inline void Set(const EXPRESS expr, unsigned int n)
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
    return ColourDistance(a.rgb(), b.rgb()) + fabs(a.transm() - b.transm());
}

typedef GenericRGBFTColour<ColourChannel>           RGBFTColour;        ///< Standard precision RGBFT colour.
typedef GenericRGBFTColour<PreciseColourChannel>    PreciseRGBFTColour; ///< High precision RGBFT colour.


/// Generic template class to hold and manipulate an RGB colour plus an unused component and a Transmit component.
///
/// @note       This colour type is provided solely for use in the front-end. Use @ref GenericTransColour in the render
///             engine instead.
///
/// @tparam T   Floating-point type to use for the individual colour components.
///
template<typename T>
class GenericRGBTColour
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

        inline explicit GenericRGBTColour(const GenericTransColour<T>& col) :
            mColour(col.colour()),
            mTransm(col.transm())
        {}

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

        /// Test if all components of the colour are valid.
        inline bool IsValid() const
        {
            // test whether any component is NaN, exploiting the property that
            // a NaN always compares non-equal, even to itself
            return ((mColour.IsValid()) &&
                    (mTransm == mTransm));
        }

        inline bool IsNearZero(T epsilon) const
        {
            return mColour.IsNearZero(epsilon) &&
                   (fabs(mTransm) < epsilon);
        }

        inline void Clear()
        {
            mColour.Clear();
            mTransm = 0.0;
        }

        inline void Invalidate()
        {
            mColour.Invalidate();
            mTransm = std::numeric_limits<T>::quiet_NaN();
        }

        inline GenericRGBTColour Clipped(T minc, T maxc)
        {
            return GenericRGBTColour(mColour.Clipped(minc, maxc),
                                     pov_base::clip<T>(mTransm, minc, maxc));
        }

        inline GenericRGBColour<T> TransmittedColour() const
        {
            return mColour * mFilter + mTransm;
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
    return ColourDistance(a.rgb(), b.rgb()) + fabs(a.transm() - b.transm());
}

/// @relates GenericRGBTColour
template<typename T>
inline GenericRGBTColour<T> Sqr(const GenericRGBTColour<T>& a) { return a * a; }

typedef GenericRGBTColour<ColourChannel>        RGBTColour;         ///< Standard precision RGBxT colour.
typedef GenericRGBTColour<PreciseColourChannel> PreciseRGBTColour;  ///< High precision RGBxT colour.


/// Generic template class to hold and manipulate an RGB colour plus transparency information.
///
/// @note   The current implementation uses RGBFT format; future implementations may vary.
///
/// @tparam T   Floating-point type to use for the individual colour components.
///
template<typename T>
class GenericTransColour
{
    public:

        template<typename T2>
        friend class GenericTransColour;

        typedef DBL EXPRESS[5];

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

        inline explicit GenericTransColour(const GenericRGBColour<T>& col) :
            mColour(col),
            mFilter(0.0),
            mTransm(0.0)
        {}

        inline explicit GenericTransColour(const GenericRGBFTColour<T>& col) :
            mColour(col.rgb()),
            mFilter(col.filter()),
            mTransm(col.transm())
        {}

        inline GenericTransColour& operator=(const GenericTransColour& col)
        {
            mColour = col.mColour;
            mFilter = col.mFilter;
            mTransm = col.mTransm;
            return *this;
        }

        inline GenericRGBColour<T>  colour() const { return mColour; }
        inline GenericRGBColour<T>& colour()       { return mColour; }

        inline T  filter() const { return mFilter; }
        inline T& filter()       { return mFilter; }

        inline T  transm() const { return mTransm; }
        inline T& transm()       { return mTransm; }

        inline T Opacity() const { return 1.0 - mFilter - mTransm; }

        inline T Greyscale() const
        {
            return mColour.Greyscale();
        }

        /// Test if all components of the colour are valid.
        inline bool IsValid() const
        {
            // test whether any component is NaN, exploiting the property that
            // a NaN always compares non-equal, even to itself
            return ((mColour.IsValid())  &&
                    (mFilter == mFilter) &&
                    (mTransm == mTransm));
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
            mFilter = std::numeric_limits<T>::quiet_NaN();
            mTransm = std::numeric_limits<T>::quiet_NaN();
        }

        inline GenericTransColour Clipped(T minc, T maxc)
        {
            return GenericTransColour(mColour.Clipped(minc, maxc),
                                      pov_base::clip<T>(mFilter, minc, maxc),
                                      pov_base::clip<T>(mTransm, minc, maxc));
        }

        inline GenericRGBColour<T> TransmittedColour() const
        {
            return mColour * mFilter + mTransm;
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

        GenericRGBColour<T> mColour;
        T                   mFilter;
        T                   mTransm;

        inline explicit GenericTransColour(const GenericRGBColour<T>& col, T filter, T transm) :
            mColour(col),
            mFilter(filter),
            mTransm(transm)
        {}
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

typedef GenericTransColour<ColourChannel>           TransColour;        ///< Standard precision RGBFT colour.
typedef GenericTransColour<PreciseColourChannel>    PreciseTransColour; ///< High precision RGBFT colour.


/// Generic template class to store a RGB colour in four bytes (RGBE).
///
/// This class uses RGBE format for compact storage of high dynamic range colours, as originally
/// proposed by Greg Ward.
///
/// @author Christoph Lipka
/// @author Based on MegaPOV HDR code written by Mael and Christoph Hormann
///
/// @tparam BIAS    Bias to use for the exponent.
///                 A value of 128 matches Greg Ward's original proposal.
/// @tparam T       Type to use for the colour components.
///                 Defaults to unsigned char.
///
template<int BIAS, typename T>
class GenericRGBEColour
{
    public:

        template<typename T2> friend class GenericRGBColour;

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
            mData[RED]   = 0;
            mData[GREEN] = 0;
            mData[BLUE]  = 0;
            mData[EXP]   = 0;
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
            RGBColour col (red, green, blue);
            double scaleFactor;
            if (ComputeExponent(col, mData[EXP], scaleFactor))
            {
                mData[RED]   = clipToType<T>(floor(col.red()   * scaleFactor));
                mData[GREEN] = clipToType<T>(floor(col.green() * scaleFactor));
                mData[BLUE]  = clipToType<T>(floor(col.blue()  * scaleFactor));
            }
            else
            {
                mData[RED] = mData[GREEN] = mData[BLUE] = mData[EXP] = 0;
            }
        }

        inline explicit GenericRGBEColour(const RGBColour& col)
        {
            double scaleFactor;
            if (ComputeExponent(col, mData[EXP], scaleFactor))
            {
                mData[RED]   = clipToType<T>(floor(col.red()   * scaleFactor));
                mData[GREEN] = clipToType<T>(floor(col.green() * scaleFactor));
                mData[BLUE]  = clipToType<T>(floor(col.blue()  * scaleFactor));
            }
            else
            {
                mData[RED] = mData[GREEN] = mData[BLUE] = mData[EXP] = 0;
            }
        }

        inline explicit GenericRGBEColour(const RGBColour& col, const RGBColour& dither)
        {
            double scaleFactor;
            if (ComputeExponent(col, mData[EXP], scaleFactor))
            {
                mData[RED]   = clip<T>(floor(col.red()   * scaleFactor + dither.red()));
                mData[GREEN] = clip<T>(floor(col.green() * scaleFactor + dither.green()));
                mData[BLUE]  = clip<T>(floor(col.blue()  * scaleFactor + dither.blue()));
            }
            else
            {
                mData[RED] = mData[GREEN] = mData[BLUE] = mData[EXP] = 0;
            }
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
            ColourChannel maxChannel;
            if (std::numeric_limits<T>::is_signed)
                maxChannel = col.MaxAbs();
            else
                maxChannel = col.Max();

            if (maxChannel <= 1.0e-32) // TODO - magic number
                return false;

            int exponent;
            double maxChannelMantissa = frexp(maxChannel, &exponent);
            biasedExponent = clipToType<T>(exponent + BIAS);

            if (biasedExponent != exponent + BIAS)
                maxChannelMantissa = ldexp(maxChannelMantissa, exponent + BIAS - biasedExponent);

            scaleFactor = (std::numeric_limits<T>::max() + 1.0) * maxChannelMantissa / maxChannel;
            return true;
        }
};

typedef GenericRGBEColour<128>  RadianceHDRColour;  ///< RGBE format as originally proposed by Greg Ward.
typedef GenericRGBEColour<250>  PhotonColour;       ///< RGBE format as adapted by Nathan Kopp for photon mapping.

}

#endif
