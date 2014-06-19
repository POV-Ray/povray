//******************************************************************************
///
/// @file base/colour.h
///
/// Declarations related to colour storage and computations.
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

template<typename T>
class GenericRGBColour;

/// @name Colour Channel Luminance
/// @{
/// @remark    These do not exactly match CCIR Recommendation 601-1, which specifies 0.299, 0.587 and
///            0.114 respectively.
/// @todo      For linear RGB with sRGB primaries this should be 0.2126, 0.7152 and 0.0722
///            respectively.
///
#define RED_INTENSITY   0.297
#define GREEN_INTENSITY 0.589
#define BLUE_INTENSITY  0.114
/// @}

/// Generic template class to hold an RGB colour plus a Filter and Transmit component.
///
/// @tparam T   Floating-point type to use for the individual colour components.
///
template<typename T>
class GenericColour
{
    public:
        typedef DBL EXPRESS[5];
        typedef T DATA[5];

        enum
        {
            RED    = 0,
            GREEN  = 1,
            BLUE   = 2,
            FILTER = 3,
            TRANSM = 4
        };

        GenericColour()
        {
            colour[RED] = 0.0;
            colour[GREEN] = 0.0;
            colour[BLUE] = 0.0;
            colour[FILTER] = 0.0;
            colour[TRANSM] = 0.0;
        }

        explicit GenericColour(T grey)
        {
            colour[RED] = grey;
            colour[GREEN] = grey;
            colour[BLUE] = grey;
            colour[FILTER] = 0.0;
            colour[TRANSM] = 0.0;
        }

        explicit GenericColour(const GenericRGBColour<T>& col);

        explicit GenericColour(const GenericRGBColour<T>& col, T nfilter, T ntransm);

        GenericColour(T nred, T ngreen, T nblue)
        {
            colour[RED] = nred;
            colour[GREEN] = ngreen;
            colour[BLUE] = nblue;
            colour[FILTER] = 0.0;
            colour[TRANSM] = 0.0;
        }

        GenericColour(T nred, T ngreen, T nblue, T nfilter, T ntransm)
        {
            colour[RED] = nred;
            colour[GREEN] = ngreen;
            colour[BLUE] = nblue;
            colour[FILTER] = nfilter;
            colour[TRANSM] = ntransm;
        }

        explicit GenericColour(const EXPRESS col)
        {
            colour[RED] = col[RED];
            colour[GREEN] = col[GREEN];
            colour[BLUE] = col[BLUE];
            colour[FILTER] = col[FILTER];
            colour[TRANSM] = col[TRANSM];
        }

        GenericColour(const GenericColour& col)
        {
            colour[RED] = col[RED];
            colour[GREEN] = col[GREEN];
            colour[BLUE] = col[BLUE];
            colour[FILTER] = col[FILTER];
            colour[TRANSM] = col[TRANSM];
        }

        template<typename T2>
        explicit GenericColour(const GenericColour<T2>& col)
        {
            colour[RED] = col[RED];
            colour[GREEN] = col[GREEN];
            colour[BLUE] = col[BLUE];
            colour[FILTER] = col[FILTER];
            colour[TRANSM] = col[TRANSM];
        }

        GenericColour& operator=(const GenericColour& col)
        {
            colour[RED] = col[RED];
            colour[GREEN] = col[GREEN];
            colour[BLUE] = col[BLUE];
            colour[FILTER] = col[FILTER];
            colour[TRANSM] = col[TRANSM];
            return *this;
        }

        GenericColour& operator=(const T& col)
        {
            colour[RED] = col;
            colour[GREEN] = col;
            colour[BLUE] = col;
            colour[FILTER] = 0.0f;
            colour[TRANSM] = 0.0f;
            return *this;
        }

        T operator[](int idx) const { return colour[idx]; }
        T& operator[](int idx) { return colour[idx]; }

        const DATA& operator*() const { return colour; }
        DATA& operator*() { return colour; }

        T red() const { return colour[RED]; }
        T& red() { return colour[RED]; }

        T green() const { return colour[GREEN]; }
        T& green() { return colour[GREEN]; }

        T blue() const { return colour[BLUE]; }
        T& blue() { return colour[BLUE]; }

        T filter() const { return colour[FILTER]; }
        T& filter() { return colour[FILTER]; }

        T transm() const { return colour[TRANSM]; }
        T& transm() { return colour[TRANSM]; }

        T opacity() const { return 1.0 - colour[FILTER] - colour[TRANSM]; }

        T greyscale() const { return RED_INTENSITY * colour[RED] + GREEN_INTENSITY * colour[GREEN] + BLUE_INTENSITY * colour[BLUE]; }

        // TODO: find a more correct way of handling alpha <-> filter/transmit
        static void AtoFT(T alpha, T& f, T& t) { f = 0.0f; t = 1.0f - alpha; }
        void AtoFT(T alpha) { colour[FILTER] = 0.0f; colour[TRANSM] = 1.0f - alpha; }
        static T FTtoA(T /*f*/, T t) { return 1.0f - t; }
        T FTtoA() const { return 1.0f - colour[TRANSM]; }

        void clear()
        {
            colour[RED] = 0.0;
            colour[GREEN] = 0.0;
            colour[BLUE] = 0.0;
            colour[FILTER] = 0.0;
            colour[TRANSM] = 0.0;
        }

        void set(T grey)
        {
            colour[RED] = grey;
            colour[GREEN] = grey;
            colour[BLUE] = grey;
            colour[FILTER] = 0.0;
            colour[TRANSM] = 0.0;
        }

        void set(T nred, T ngreen, T nblue)
        {
            colour[RED] = nred;
            colour[GREEN] = ngreen;
            colour[BLUE] = nblue;
            colour[FILTER] = 0.0;
            colour[TRANSM] = 0.0;
        }

        void set(T nred, T ngreen, T nblue, T nfilter, T ntransm)
        {
            colour[RED] = nred;
            colour[GREEN] = ngreen;
            colour[BLUE] = nblue;
            colour[FILTER] = nfilter;
            colour[TRANSM] = ntransm;
        }

        /// Sets the RGB components, but leaves filter and transmit unchanged.
        void setRGB(GenericRGBColour<T> rgb);

        /// Adds to the RGB components, but leaves filter and transmit unchanged.
        void addRGB(GenericRGBColour<T> rgb);

        GenericColour clip(T minc, T maxc)
        {
            return GenericColour(pov_base::clip<T>(colour[RED], minc, maxc),
                                 pov_base::clip<T>(colour[GREEN], minc, maxc),
                                 pov_base::clip<T>(colour[BLUE], minc, maxc),
                                 pov_base::clip<T>(colour[FILTER], minc, maxc),
                                 pov_base::clip<T>(colour[TRANSM], minc, maxc));
        }

        inline GenericRGBColour<T> rgbTransm() const;

        GenericColour operator+(const GenericColour& b) const
        {
            return GenericColour(colour[RED] + b[RED], colour[GREEN] + b[GREEN], colour[BLUE] + b[BLUE], colour[FILTER] + b[FILTER], colour[TRANSM] + b[TRANSM]);
        }

        GenericColour operator-(const GenericColour& b) const
        {
            return GenericColour(colour[RED] - b[RED], colour[GREEN] - b[GREEN], colour[BLUE] - b[BLUE], colour[FILTER] - b[FILTER], colour[TRANSM] - b[TRANSM]);
        }

        GenericColour operator*(const GenericColour& b) const
        {
            return GenericColour(colour[RED] * b[RED], colour[GREEN] * b[GREEN], colour[BLUE] * b[BLUE], colour[FILTER] * b[FILTER], colour[TRANSM] * b[TRANSM]);
        }

        GenericColour operator/(const GenericColour& b) const
        {
            return GenericColour(colour[RED] / b[RED], colour[GREEN] / b[GREEN], colour[BLUE] / b[BLUE], colour[FILTER] / b[FILTER], colour[TRANSM] / b[TRANSM]);
        }

        GenericColour& operator+=(const GenericColour& b)
        {
            colour[RED] += b[RED];
            colour[GREEN] += b[GREEN];
            colour[BLUE] += b[BLUE];
            colour[FILTER] += b[FILTER];
            colour[TRANSM] += b[TRANSM];
            return *this;
        }

        GenericColour& operator-=(const GenericColour& b)
        {
            colour[RED] -= b[RED];
            colour[GREEN] -= b[GREEN];
            colour[BLUE] -= b[BLUE];
            colour[FILTER] -= b[FILTER];
            colour[TRANSM] -= b[TRANSM];
            return *this;
        }

        GenericColour& operator*=(const GenericColour& b)
        {
            colour[RED] *= b[RED];
            colour[GREEN] *= b[GREEN];
            colour[BLUE] *= b[BLUE];
            colour[FILTER] *= b[FILTER];
            colour[TRANSM] *= b[TRANSM];
            return *this;
        }

        GenericColour& operator/=(const GenericColour& b)
        {
            colour[RED] /= b[RED];
            colour[GREEN] /= b[GREEN];
            colour[BLUE] /= b[BLUE];
            colour[FILTER] /= b[FILTER];
            colour[TRANSM] /= b[TRANSM];
            return *this;
        }

        GenericColour operator-() const
        {
            return GenericColour(-colour[RED], -colour[GREEN], -colour[BLUE], -colour[FILTER], -colour[TRANSM]);
        }

        GenericColour operator+(DBL b) const
        {
            return GenericColour(colour[RED] + b, colour[GREEN] + b, colour[BLUE] + b, colour[FILTER] + b, colour[TRANSM] + b);
        }

        GenericColour operator-(DBL b) const
        {
            return GenericColour(colour[RED] - b, colour[GREEN] - b, colour[BLUE] - b, colour[FILTER] - b, colour[TRANSM] - b);
        }

        GenericColour operator*(DBL b) const
        {
            return GenericColour(colour[RED] * b, colour[GREEN] * b, colour[BLUE] * b, colour[FILTER] * b, colour[TRANSM] * b);
        }

        GenericColour operator/(DBL b) const
        {
            return GenericColour(colour[RED] / b, colour[GREEN] / b, colour[BLUE] / b, colour[FILTER] / b, colour[TRANSM] / b);
        }

        GenericColour& operator+=(DBL b)
        {
            colour[RED] += b;
            colour[GREEN] += b;
            colour[BLUE] += b;
            colour[FILTER] += b;
            colour[TRANSM] += b;
            return *this;
        }

        GenericColour& operator-=(DBL b)
        {
            colour[RED] -= b;
            colour[GREEN] -= b;
            colour[BLUE] -= b;
            colour[FILTER] -= b;
            colour[TRANSM] -= b;
            return *this;
        }

        GenericColour& operator*=(DBL b)
        {
            colour[RED] *= b;
            colour[GREEN] *= b;
            colour[BLUE] *= b;
            colour[FILTER] *= b;
            colour[TRANSM] *= b;
            return *this;
        }

        GenericColour& operator/=(DBL b)
        {
            colour[RED] /= b;
            colour[GREEN] /= b;
            colour[BLUE] /= b;
            colour[FILTER] /= b;
            colour[TRANSM] /= b;
            return *this;
        }
    private:
        DATA colour;
};

/// Generic template class to hold an RGB colour.
///
/// @tparam T   Floating-point type to use for the individual colour components.
///
template<typename T>
class GenericRGBColour
{
    public:

        typedef T DATA[3];

        enum
        {
            RED    = 0,
            GREEN  = 1,
            BLUE   = 2
        };

        GenericRGBColour()
        {
            colour[RED] = 0.0;
            colour[GREEN] = 0.0;
            colour[BLUE] = 0.0;
        }

        explicit GenericRGBColour(T grey)
        {
            colour[RED] = grey;
            colour[GREEN] = grey;
            colour[BLUE] = grey;
        }

        GenericRGBColour(T nred, T ngreen, T nblue)
        {
            colour[RED] = nred;
            colour[GREEN] = ngreen;
            colour[BLUE] = nblue;
        }

        explicit GenericRGBColour(const GenericColour<T>& col)
        {
            colour[RED] = col[RED];
            colour[GREEN] = col[GREEN];
            colour[BLUE] = col[BLUE];
        }

        GenericRGBColour(const GenericRGBColour& col)
        {
            colour[RED] = col[RED];
            colour[GREEN] = col[GREEN];
            colour[BLUE] = col[BLUE];
        }

        template<typename T2>
        explicit GenericRGBColour(const GenericRGBColour<T2>& col)
        {
            colour[RED] = col[RED];
            colour[GREEN] = col[GREEN];
            colour[BLUE] = col[BLUE];
        }

        GenericRGBColour& operator=(const GenericRGBColour& col)
        {
            colour[RED] = col[RED];
            colour[GREEN] = col[GREEN];
            colour[BLUE] = col[BLUE];
            return *this;
        }

        GenericRGBColour& operator=(const T& col)
        {
            colour[RED] = col;
            colour[GREEN] = col;
            colour[BLUE] = col;
            return *this;
        }

        T operator[](int idx) const { return colour[idx]; }
        T& operator[](int idx) { return colour[idx]; }

        T red() const { return colour[RED]; }
        T& red() { return colour[RED]; }

        T green() const { return colour[GREEN]; }
        T& green() { return colour[GREEN]; }

        T blue() const { return colour[BLUE]; }
        T& blue() { return colour[BLUE]; }

        /// Computes the greyscale intensity of the colour.
        ///
        /// @note   Do _not_ use this function if you want to compute some kind of weight; that's
        ///         what @ref weightGreyscale() is for.
        ///
        T greyscale() const { return RED_INTENSITY * colour[RED] + GREEN_INTENSITY * colour[GREEN] + BLUE_INTENSITY * colour[BLUE]; }

        /// Computes a generic measure for the weight of the colour.
        T weight() const
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
            /// @remark For backward compatibility, @ref weightMax(), @ref weightMaxAbs(),
            ///         @ref weightGreyscale() and @ref weightGreyscaleAbs() are provided.

            return (fabs(colour[RED]) + fabs(colour[GREEN]) + fabs(colour[BLUE])) / 3.0;
        }

        /// Computes a measure for the weight of the colour based on the magnitude of its greyscale
        /// value.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref weight()
        ///             for consistency of colour math.
        ///
        T weightGreyscaleAbs() const
        {
            return fabs(greyscale());
        }

        /// Computes a measure for the weight of the colour based on its greyscale value.
        ///
        /// @note       Do _not_ use this function if you absolutely want to know the greyscale
        ///             intensity of the colour. For such cases, use @ref greyscale() instead.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to
        ///             @ref weightGreyscaleAbs() or @ref weight() for consistency of colour math.
        ///
        T weightGreyscale() const
        {
            return greyscale();
        }

        /// Computes a measure for the weight of the colour based on the colour channel with the
        /// greatest value.
        ///
        /// @note       Do _not_ use this function if you absolutely want to know the intensity of
        ///             the strongest colour channel. For such cases, use @ref max() instead.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to
        ///             @ref weightMaxAbs() or @ref weight() for consistency of colour math.
        ///
        T weightMax() const
        {
            return max();
        }

        /// Computes a measure for the weight of the colour based on the colour channel with the
        /// greatest magnitude.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref weight()
        ///             for consistency of colour math.
        ///
        T weightMaxAbs() const
        {
            return max3(fabs(colour[RED]), fabs(colour[GREEN]), fabs(colour[BLUE]));
        }

        /// Computes the intensity of the colour channel with the greatest value.
        ///
        /// @note       Do _not_ use this function if you want to compute some kind of weight;
        ///             that's what @ref weightMax() is for.
        ///
        T max() const
        {
            return max3(colour[RED], colour[GREEN], colour[BLUE]);
        }

        bool isZero() const { return (colour[RED] == 0) && (colour[GREEN] == 0) && (colour[BLUE] == 0); }

        void clear()
        {
            colour[RED] = 0.0;
            colour[GREEN] = 0.0;
            colour[BLUE] = 0.0;
        }

        void set(T grey)
        {
            colour[RED] = grey;
            colour[GREEN] = grey;
            colour[BLUE] = grey;
        }

        void set(T nred, T ngreen, T nblue)
        {
            colour[RED] = nred;
            colour[GREEN] = ngreen;
            colour[BLUE] = nblue;
        }

        GenericRGBColour clip(T minc, T maxc)
        {
            return GenericRGBColour(pov_base::clip<T>(colour[RED], minc, maxc),
                                    pov_base::clip<T>(colour[GREEN], minc, maxc),
                                    pov_base::clip<T>(colour[BLUE], minc, maxc));
        }

        GenericRGBColour operator+(const GenericRGBColour& b) const
        {
            return GenericRGBColour(colour[RED] + b[RED], colour[GREEN] + b[GREEN], colour[BLUE] + b[BLUE]);
        }

        GenericRGBColour operator-(const GenericRGBColour& b) const
        {
            return GenericRGBColour(colour[RED] - b[RED], colour[GREEN] - b[GREEN], colour[BLUE] - b[BLUE]);
        }

        GenericRGBColour operator*(const GenericRGBColour& b) const
        {
            return GenericRGBColour(colour[RED] * b[RED], colour[GREEN] * b[GREEN], colour[BLUE] * b[BLUE]);
        }

        GenericRGBColour operator/(const GenericRGBColour& b) const
        {
            return GenericRGBColour(colour[RED] / b[RED], colour[GREEN] / b[GREEN], colour[BLUE] / b[BLUE]);
        }

        GenericRGBColour& operator+=(const GenericRGBColour& b)
        {
            colour[RED] += b[RED];
            colour[GREEN] += b[GREEN];
            colour[BLUE] += b[BLUE];
            return *this;
        }

        GenericRGBColour& operator-=(const GenericRGBColour& b)
        {
            colour[RED] -= b[RED];
            colour[GREEN] -= b[GREEN];
            colour[BLUE] -= b[BLUE];
            return *this;
        }

        GenericRGBColour& operator*=(const GenericRGBColour& b)
        {
            colour[RED] *= b[RED];
            colour[GREEN] *= b[GREEN];
            colour[BLUE] *= b[BLUE];
            return *this;
        }

        GenericRGBColour& operator/=(const GenericRGBColour& b)
        {
            colour[RED] /= b[RED];
            colour[GREEN] /= b[GREEN];
            colour[BLUE] /= b[BLUE];
            return *this;
        }

        GenericRGBColour operator-() const
        {
            return GenericRGBColour(-colour[RED], -colour[GREEN], -colour[BLUE]);
        }

        GenericRGBColour operator+(DBL b) const
        {
            return GenericRGBColour(colour[RED] + b, colour[GREEN] + b, colour[BLUE] + b);
        }

        GenericRGBColour operator-(DBL b) const
        {
            return GenericRGBColour(colour[RED] - b, colour[GREEN] - b, colour[BLUE] - b);
        }

        GenericRGBColour operator*(DBL b) const
        {
            return GenericRGBColour(colour[RED] * b, colour[GREEN] * b, colour[BLUE] * b);
        }

        GenericRGBColour operator/(DBL b) const
        {
            return GenericRGBColour(colour[RED] / b, colour[GREEN] / b, colour[BLUE] / b);
        }

        GenericRGBColour& operator+=(DBL b)
        {
            colour[RED] += b;
            colour[GREEN] += b;
            colour[BLUE] += b;
            return *this;
        }

        GenericRGBColour& operator-=(DBL b)
        {
            colour[RED] -= b;
            colour[GREEN] -= b;
            colour[BLUE] -= b;
            return *this;
        }

        GenericRGBColour& operator*=(DBL b)
        {
            colour[RED] *= b;
            colour[GREEN] *= b;
            colour[BLUE] *= b;
            return *this;
        }

        GenericRGBColour& operator/=(DBL b)
        {
            colour[RED] /= b;
            colour[GREEN] /= b;
            colour[BLUE] /= b;
            return *this;
        }
    private:
        DATA colour;
};

template<typename T>
inline GenericColour<T>::GenericColour(const GenericRGBColour<T>& col)
{
    colour[RED] = col[RED];
    colour[GREEN] = col[GREEN];
    colour[BLUE] = col[BLUE];
    colour[FILTER] = 0.0;
    colour[TRANSM] = 0.0;
}

template<typename T>
inline GenericColour<T>::GenericColour(const GenericRGBColour<T>& col, T nfilter, T ntransm)
{
    colour[RED] = col[RED];
    colour[GREEN] = col[GREEN];
    colour[BLUE] = col[BLUE];
    colour[FILTER] = nfilter;
    colour[TRANSM] = ntransm;
}

template<typename T>
inline GenericRGBColour<T> GenericColour<T>::rgbTransm() const
{
    return GenericRGBColour<T>( colour[RED]   * colour[FILTER] + colour[TRANSM],
                                colour[GREEN] * colour[FILTER] + colour[TRANSM],
                                colour[BLUE]  * colour[FILTER] + colour[TRANSM] );
}

template<typename T>
inline void GenericColour<T>::setRGB(GenericRGBColour<T> rgb)
{
    colour[RED]   = rgb.red();
    colour[GREEN] = rgb.green();
    colour[BLUE]  = rgb.blue();
}

template<typename T>
inline void GenericColour<T>::addRGB(GenericRGBColour<T> rgb)
{
    colour[RED]   += rgb.red();
    colour[GREEN] += rgb.green();
    colour[BLUE]  += rgb.blue();
}

/// @relates GenericColour
template<typename T>
inline GenericColour<T> operator* (double a, const GenericColour<T>& b) { return b * a; }

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> operator* (double a, const GenericRGBColour<T>& b) { return b * a; }

/// @relates GenericColour
template<typename T>
inline GenericColour<T> operator+ (double a, const GenericColour<T>& b) { return b + a; }

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> operator+ (double a, const GenericRGBColour<T>& b) { return b + a; }

/// @relates GenericColour
template<typename T>
inline GenericColour<T> operator- (double a, const GenericColour<T>& b) { return GenericColour<T>(a) - b; }

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> operator- (double a, const GenericRGBColour<T>& b) { return GenericRGBColour<T>(a) - b; }

/// @relates GenericColour
template<typename T>
inline double colourDistance (const GenericColour<T>& a, const GenericColour<T>& b) { return fabs(a.red() - b.red()) + fabs(a.green() - b.green()) + fabs(a.blue() - b.blue()); }

/// @relates GenericRGBColour
template<typename T>
inline double colourDistance (const GenericRGBColour<T>& a, const GenericRGBColour<T>& b) { return fabs(a.red() - b.red()) + fabs(a.green() - b.green()) + fabs(a.blue() - b.blue()); }

/// @relates GenericColour
template<typename T>
inline double colourDistanceRGBT (const GenericColour<T>& a, const GenericColour<T>& b) { return colourDistance(a, b) + fabs(a.transm() - b.transm()); }

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> Sqr(const GenericRGBColour<T>& a) { return a * a; }

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> exp(const GenericRGBColour<T>& a) { return GenericRGBColour<T>(::exp(a.red()), ::exp(a.green()), ::exp(a.blue())); }

/// @relates GenericRGBColour
template<typename T>
inline GenericRGBColour<T> sqrt(const GenericRGBColour<T>& a) { return GenericRGBColour<T>(::sqrt(a.red()), ::sqrt(a.green()), ::sqrt(a.blue())); }

typedef GenericColour<COLC>    Colour;          ///< Single-Precision RGBFT Colour.
typedef GenericColour<DBL>     DblColour;       ///< Double-Precision RGBFT Colour.
typedef GenericRGBColour<COLC> RGBColour;       ///< Single-Precision RGB Colour.
typedef GenericRGBColour<DBL>  DblRGBColour;    ///< Double-Precision RGB Colour.

}

#endif
