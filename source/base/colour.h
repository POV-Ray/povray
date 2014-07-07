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
#include <limits>

#include "base/configbase.h"
#include "base/types.h"

#define NUM_COLOUR_CHANNELS 3

namespace pov_base
{

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

template<typename MODEL_T, typename CHANNEL_T, typename DERIVED_T>
class GenericLinearColour;

template<typename CHANNEL_T>
class GenericRGBColour;

template<typename T>
class GenericRGBFTColour;

template<typename T>
class GenericRGBTColour;

template<typename CHANNEL_T>
class GenericXYZColour;

template<typename T>
class GenericColour;

template<typename T>
class GenericTransColour;

template<typename MODEL_T, typename BIAS_T, typename CHANNEL_T = unsigned char>
class GenericCompactColour;

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

struct ColourModelRGB
{
    static const unsigned int kChannels = 3;

    enum ChannelId
    {
        kRed   = 0,
        kGreen = 1,
        kBlue  = 2
    };
};

struct ColourModelXYZ
{
    static const unsigned int kChannels = 3;

    enum ChannelId
    {
        kX = 0,
        kY = 1,
        kZ = 2
    };
};

struct ColourModelInternal
{
    static const unsigned int kChannels = NUM_COLOUR_CHANNELS;
};

/// Matrix to convert from CIE XYZ colour space to linear sRGB.
///
/// @note   The matrix coefficients were re-computed from the primaries' xy coordinates and the whitepoint's xyY
///         coordinates (rather than blindly relying on the contradicting values found on the internet) using the
///         following [Sage](http://www.sagemath.org/) script:
///         @include colour-matrices-srgb.sage
///         Rounded to four decimals they match the coefficients from ITU-R BT.709, which uses the same primaries and
///         white point as sRGB.
///
const ColourChannel kaColourConversionMatrixXYZtoRGB[ColourModelRGB::kChannels * ColourModelXYZ::kChannels] = {
    3.24096994190452,   -1.53738317757009, -0.498610760293003,
   -0.969243636280880,   1.87596750150772,  0.0415550574071757,
    0.0556300796969937, -0.203976958888977, 1.05697151424288
};

/// Matrix to convert linear sRGB colour space to CIE XYZ.
///
/// @note   The matrix coefficients were re-computed from the primaries' xy coordinates and the whitepoint's xyY
///         coordinates (rather than blindly relying on the -- partially contradicting -- values found on the internet)
///         using the following [Sage](http://www.sagemath.org/) script:
///         @include colour-matrices-srgb.sage
///         Rounded to four decimals they match the coefficients from ITU-R BT.709, which uses the same primaries and
///         white point as sRGB.
///
const ColourChannel kaColourConversionMatrixRGBtoXYZ[ColourModelXYZ::kChannels * ColourModelRGB::kChannels] = {
    0.412390799265959,  0.357584339383878, 0.180480788401834,
    0.212639005871510,  0.715168678767756, 0.0721923153607337,
    0.0193308187155918, 0.119194779794626, 0.950532152249661
};

template<int BIAS>
struct CompactColourBias
{
    static const unsigned int kBias = BIAS;
};

typedef CompactColourBias<250>  PhotonBias;
typedef CompactColourBias<128>  RadianceHDRBias;

/// Generic template class to hold and manipulate a colour.
///
/// Any colour model can be used as long as it is based on a linear combination of multiple coefficients.
///
/// @note   This colour type is provided solely for use in the front-end and image handling code. Use
///         @ref GenericColour in the render engine instead.
///
/// @remark This class is mainly a collection of mathematical operations to be inherited by derived classes.
///         In order for those operations to return the correct derived type, the Curiously Recurring Template Pattern
///         (CRTP) is employed.
///
/// @tparam MODEL_T     Colour model to use.
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour components.
/// @tparam DERIVED_T   Type that derives from this one.
///
template<typename MODEL_T, typename CHANNEL_T, typename DERIVED_T>
class GenericLinearColour
{
    public:

        typedef MODEL_T   Model;
        typedef CHANNEL_T Channel;
        typedef DERIVED_T DerivedColour;
        static const unsigned int kChannels = Model::kChannels;

        template<typename MODEL_T2, typename CHANNEL_T2, typename RETURN_T2>
        friend class GenericLinearColour;

        /// Default constructor.
        inline GenericLinearColour()
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = 0.0;
        }

        /// Copy constructor.
        inline GenericLinearColour(const GenericLinearColour& col)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = col.mColour[i];
        }

        template<typename CHANNEL_T2, typename DERIVED_T2>
        inline explicit GenericLinearColour(const GenericLinearColour<Model,CHANNEL_T2,DERIVED_T2>& col)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = col.mColour[i];
        }

        template<typename BIAS_T2, typename CHANNEL_T2>
        inline explicit GenericLinearColour(const GenericCompactColour<Model,BIAS_T2,CHANNEL_T2>& col)
        {
            typedef GenericCompactColour<Model,BIAS_T2,CHANNEL_T2> CompactColour;
            if (col.mData[CompactColour::kExp] > std::numeric_limits<typename CompactColour::Channel>::min())
            {
                double expFactor = ldexp(1.0,(int)col.mData[CompactColour::kExp]-(int)(CompactColour::kBias+8));
                for (unsigned int i = 0; i < kChannels; i ++)
                    mColour[i] = (col.mData[i]) * expFactor;
            }
            else
            {
                for (unsigned int i = 0; i < kChannels; i ++)
                    mColour[i] = 0.0;
            }
        }

        inline explicit GenericLinearColour(Channel grey)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = grey;
        }

        template<typename T2>
        inline explicit GenericLinearColour(const T2& col, const ColourChannel *pConversionMatrix)
        {
            const ColourChannel *pMatrixElement = pConversionMatrix;
            for (unsigned int i = 0; i < kChannels; i ++)
            {
                mColour[i] = 0.0;
                for (unsigned int j = 0; j < T2::Model::kChannels; j ++)
                    mColour[i] += *(pMatrixElement++) * col.mColour[j];
            }
        }

        inline GenericLinearColour& operator=(const GenericLinearColour& col)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = col.mColour[i];
            return *this;
        }

        inline Channel  operator[](unsigned int idx) const { assert(idx < kChannels); return mColour[idx]; }
        inline Channel& operator[](unsigned int idx)       { assert(idx < kChannels); return mColour[idx]; }

        /// Computes the sum of the channels' values.
        inline Channel Sum() const
        {
            Channel result = 0.0;
            for (unsigned int i = 0; i < kChannels; i ++)
                result += mColour[i];
            return result;
        }

        /// Computes the sum of the channels' magnitudes.
        inline Channel SumAbs() const
        {
            Channel result = 0.0;
            for (unsigned int i = 0; i < kChannels; i ++)
                result += fabs(mColour[i]);
            return result;
        }

        /// Computes the intensity of the colour channel with the greatest value.
        inline Channel Max() const
        {
            Channel result = mColour[0];
            for (unsigned int i = 1; i < kChannels; i ++)
                result = max(result, mColour[i]);
            return result;
        }

        /// Computes the intensity of the colour channel with the greatest magnitude.
        inline Channel MaxAbs() const
        {
            Channel result = fabs(mColour[0]);
            for (unsigned int i = 1; i < kChannels; i ++)
                result = max(result, fabs(mColour[i]));
            return result;
        }

        /// Computes the intensity of the colour channel with the smallest value.
        inline Channel Min() const
        {
            Channel result = mColour[0];
            for (unsigned int i = 1; i < kChannels; i ++)
                result = min(result, mColour[i]);
            return result;
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
            bool result = true;
            for (unsigned int i = 0; i < kChannels; i ++)
                result = result && (mColour[i] == 0.0);
            return result;
        }

        inline bool IsNearZero(Channel epsilon) const
        {
            bool result = true;
            for (unsigned int i = 0; i < kChannels; i ++)
                result = result && (fabs(mColour[i]) < epsilon);
            return result;
        }

        inline void Clear()
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = 0.0;
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

        inline void Set(Channel grey)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = grey;
        }

        inline void Clip(Channel minc, Channel maxc)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = clip(mColour[i], minc, maxc);
        }

        inline void ClipUpper(Channel maxc)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = min(mColour[i], maxc);
        }

        inline void ClipLower(Channel minc)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = max(mColour[i], minc);
        }

        inline DerivedColour Clipped(Channel minc, Channel maxc)
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = clip(mColour[i], minc, maxc);
            return result;
        }

        inline DerivedColour ClippedUpper(Channel maxc)
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = min(mColour[i], maxc);
            return result;
        }

        inline DerivedColour ClippedLower(Channel minc)
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = max(mColour[i], minc);
            return result;
        }

        inline DerivedColour operator+(const GenericLinearColour& b) const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = mColour[i] + b.mColour[i];
            return result;
        }

        inline DerivedColour operator-(const GenericLinearColour& b) const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = mColour[i] - b.mColour[i];
            return result;
        }

        inline DerivedColour operator*(const GenericLinearColour& b) const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = mColour[i] * b.mColour[i];
            return result;
        }

        inline DerivedColour operator/(const GenericLinearColour& b) const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = mColour[i] / b.mColour[i];
            return result;
        }

        inline DerivedColour& operator+=(const GenericLinearColour& b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] += b.mColour[i];
            return *(static_cast<DerivedColour*>(this));
        }

        inline DerivedColour& operator-=(const GenericLinearColour& b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] -= b.mColour[i];
            return *(static_cast<DerivedColour*>(this));
        }

        inline DerivedColour& operator*=(const GenericLinearColour& b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] *= b.mColour[i];
            return *(static_cast<DerivedColour*>(this));
        }

        inline DerivedColour& operator/=(const GenericLinearColour& b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] /= b.mColour[i];
            return *(static_cast<DerivedColour*>(this));
        }

        inline DerivedColour operator-() const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = -mColour[i];
            return result;
        }

        inline DerivedColour operator+(Channel b) const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = mColour[i] + b;
            return result;
        }

        inline DerivedColour operator-(Channel b) const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = mColour[i] - b;
            return result;
        }

        inline DerivedColour operator*(Channel b) const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = mColour[i] * b;
            return result;
        }

        inline DerivedColour operator/(Channel b) const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = mColour[i] / b;
            return result;
        }

        inline DerivedColour& operator+=(Channel b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] += b;
            return *(static_cast<DerivedColour*>(this));
        }

        inline DerivedColour& operator-=(Channel b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] -= b;
            return *(static_cast<DerivedColour*>(this));
        }

        inline DerivedColour& operator*=(Channel b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] *= b;
            return *(static_cast<DerivedColour*>(this));
        }

        inline DerivedColour& operator/=(Channel b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] /= b;
            return *(static_cast<DerivedColour*>(this));
        }

        inline DerivedColour Exp() const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = exp(mColour[i]);
            return result;
        }

        inline DerivedColour Pow(Channel b) const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = pow(mColour[i], b);
            return result;
        }

        inline DerivedColour Cos() const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = cos(mColour[i]);
            return result;
        }

        inline DerivedColour Sqrt() const
        {
            DerivedColour result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result.mColour[i] = sqrt(mColour[i]);
            return result;
        }

    protected:

        Channel mColour[kChannels];
};

/// @relates GenericLinearColour
template<typename MT, typename CT, typename RT, typename T>
inline RT operator* (T a, const GenericLinearColour<MT,CT,RT>& b) { return b * a; }

/// @relates GenericLinearColour
template<typename MT, typename CT, typename RT, typename T>
inline RT operator/ (T a, const GenericLinearColour<MT,CT,RT>& b) { return GenericLinearColour<MT,CT,RT>(a) / b; }

/// @relates GenericLinearColour
template<typename MT, typename CT, typename RT, typename T>
inline RT operator+ (T a, const GenericLinearColour<MT,CT,RT>& b) { return b + a; }

/// @relates GenericLinearColour
template<typename MT, typename CT, typename RT, typename T>
inline RT operator- (T a, const GenericLinearColour<MT,CT,RT>& b) { return GenericLinearColour<MT,CT,RT>(a) - b; }

/// @relates GenericLinearColour
template<typename MT, typename CT, typename RT>
inline CT ColourDistance (const GenericLinearColour<MT,CT,RT>& a, const GenericLinearColour<MT,CT,RT>& b) { return (a - b).SumAbs(); }

/// @relates GenericLinearColour
template<typename MT, typename CT, typename RT>
inline RT Sqr(const GenericLinearColour<MT,CT,RT>& a) { return a * a; }

/// @relates GenericLinearColour
template<typename MT, typename CT, typename RT>
inline RT Exp(const GenericLinearColour<MT,CT,RT>& a) { return a.Exp(); }

/// @relates GenericLinearColour
template<typename MT, typename CT, typename RT, typename T>
inline RT Pow(const GenericLinearColour<MT,CT,RT>& a, T b) { return a.Pow(b); }

/// @relates GenericLinearColour
template<typename MT, typename CT, typename RT>
inline RT Sqrt(const GenericLinearColour<MT,CT,RT>& a) { return a.Sqrt(); }

/// @relates GenericLinearColour
template<typename MT, typename CT, typename RT>
inline RT Cos(const GenericLinearColour<MT,CT,RT>& a) { return a.Cos(); }


/// Generic template class to hold and manipulate an RGB colour.
///
/// @note   This colour type is provided solely for use in the front-end and image handling code. Use
///         @ref GenericColour in the render engine instead.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour components.
///
template<typename CHANNEL_T>
class GenericRGBColour : public GenericLinearColour<ColourModelRGB, CHANNEL_T, GenericRGBColour<CHANNEL_T> >
{
    public:

        typedef GenericLinearColour<ColourModelRGB, CHANNEL_T, GenericRGBColour<CHANNEL_T> > Parent;
        typedef ColourModelRGB      Model;
        typedef CHANNEL_T           Channel;
        typedef GenericRGBColour    DerivedColour;
        using Parent::kChannels;

        using Parent::Max;
        using Parent::MaxAbs;

        template<typename MODEL_T2, typename CHANNEL_T2, typename DERIVED_T2>
        friend class GenericLinearColour;

        template<typename T2>
        friend class GenericRGBColour;

        friend class GenericColour<Channel>;
        friend class GenericRGBTColour<Channel>;
        friend class GenericRGBFTColour<Channel>;

        friend GenericRGBColour ToRGBColour(const GenericColour<Channel>& col);

        /// Default constructor.
        inline GenericRGBColour() : Parent() {}

        /// Copy constructor.
        inline GenericRGBColour(const GenericRGBColour& col) : Parent(col) {}

        inline explicit GenericRGBColour(const GenericLinearColour<Model,Channel,DerivedColour>& col) : Parent(col) {}

        template<typename CHANNEL_T2>
        inline explicit GenericRGBColour(const GenericLinearColour<Model,CHANNEL_T2,DerivedColour>& col) : Parent(col) {}

        template<typename CHANNEL_T2>
        inline explicit GenericRGBColour(const GenericRGBColour<CHANNEL_T2>& col) : Parent(col) {}

        inline explicit GenericRGBColour(Channel grey) : Parent(grey) {}

        inline explicit GenericRGBColour(Channel red, Channel green, Channel blue) :
            Parent()
        {
            mColour[ColourModelRGB::kRed]   = red;
            mColour[ColourModelRGB::kGreen] = green;
            mColour[ColourModelRGB::kBlue]  = blue;
        }

        inline explicit GenericRGBColour(const GenericRGBFTColour<Channel>& col) : Parent(col.rgb()) {}

        template<typename BIAS_T2, typename CHANNEL_T2>
        inline explicit GenericRGBColour(const GenericCompactColour<Model,BIAS_T2,CHANNEL_T2>& col) : Parent(col) {}

        template<typename DERIVED_T2>
        inline explicit GenericRGBColour(const GenericLinearColour<ColourModelXYZ,Channel,DERIVED_T2>& col) : Parent(col, kaColourConversionMatrixXYZtoRGB) {}

        inline Channel  red()   const { return mColour[ColourModelRGB::kRed]; }
        inline Channel& red()         { return mColour[ColourModelRGB::kRed]; }

        inline Channel  green() const { return mColour[ColourModelRGB::kGreen]; }
        inline Channel& green()       { return mColour[ColourModelRGB::kGreen]; }

        inline Channel  blue()  const { return mColour[ColourModelRGB::kBlue]; }
        inline Channel& blue()        { return mColour[ColourModelRGB::kBlue]; }

        /// Computes the greyscale intensity of the colour.
        ///
        /// @note   Do _not_ use this function if you want to compute some kind of weight; that's
        ///         what @ref WeightGreyscale() is for.
        ///
        inline Channel Greyscale() const
        {
            return kRedIntensity   * mColour[ColourModelRGB::kRed]   +
                   kGreenIntensity * mColour[ColourModelRGB::kGreen] +
                   kBlueIntensity  * mColour[ColourModelRGB::kBlue];
        }

        /// Computes a generic measure for the weight of the colour.
        inline Channel Weight() const
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
            ///         @ref WeightGreyscale() and @ref WeightAbsGreyscale() are provided.

            return (fabs(mColour[ColourModelRGB::kRed])   +
                    fabs(mColour[ColourModelRGB::kGreen]) +
                    fabs(mColour[ColourModelRGB::kBlue])) / 3.0;
        }

        /// Computes a measure for the weight of the colour based on the magnitude of its greyscale
        /// value.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref Weight()
        ///             for consistency of colour math.
        ///
        inline Channel WeightAbsGreyscale() const
        {
            return fabs(Greyscale());
        }

        /// Computes a measure for the weight of the colour based on its greyscale value.
        ///
        /// @note       Do _not_ use this function if you absolutely want to know the greyscale
        ///             intensity of the colour. For such cases, use @ref Greyscale() instead.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to
        ///             @ref WeightAbsGreyscale() or @ref Weight() for consistency of colour math.
        ///
        inline Channel WeightGreyscale() const
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
        inline Channel WeightMax() const
        {
            return Max();
        }

        /// Computes a measure for the weight of the colour based on the colour channel with the
        /// greatest magnitude.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref Weight()
        ///             for consistency of colour math.
        ///
        inline Channel WeightMaxAbs() const
        {
            return MaxAbs();
        }

#if (NUM_COLOUR_CHANNELS == 3)
        inline explicit GenericRGBColour(const GenericColour<Channel>& col)
        {
            mColour[ColourModelRGB::kRed]   = col.mColour[0];
            mColour[ColourModelRGB::kGreen] = col.mColour[1];
            mColour[ColourModelRGB::kBlue]  = col.mColour[2];
        }
#else
        inline explicit GenericRGBColour(const GenericColour<Channel>& col)
        {
            mColour[ColourModelRGB::kRed]   = col.Red();
            mColour[ColourModelRGB::kGreen] = col.Green();
            mColour[ColourModelRGB::kBlue]  = col.Blue();
        }
#endif

    protected:

        using Parent::mColour;
};

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
class GenericRGBFTColour
{
    public:

        template<typename T2>
        friend class GenericRGBFTColour;

        friend GenericRGBFTColour ToRGBFTColour(const GenericTransColour<T>& col);

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
    return ColourDistance(a.rgb(), b.rgb()) + fabs(a.transm() - b.transm());
}

typedef GenericRGBFTColour<ColourChannel>           RGBFTColour;        ///< Standard precision RGBFT colour.
typedef GenericRGBFTColour<PreciseColourChannel>    PreciseRGBFTColour; ///< High precision RGBFT colour.


/// Generic template class to hold and manipulate an RGB colour plus a Transmit component.
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
                   (fabs(mTransm) < epsilon);
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
    return ColourDistance(a.rgb(), b.rgb()) + fabs(a.transm() - b.transm());
}

/// @relates GenericRGBTColour
template<typename T>
inline GenericRGBTColour<T> Sqr(const GenericRGBTColour<T>& a) { return a * a; }

typedef GenericRGBTColour<ColourChannel>        RGBTColour;         ///< Standard precision RGBxT colour.
typedef GenericRGBTColour<PreciseColourChannel> PreciseRGBTColour;  ///< High precision RGBxT colour.


/// Generic template class to hold and manipulate an XYZ colour.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour components.
///
template<typename CHANNEL_T>
class GenericXYZColour : public GenericLinearColour<ColourModelXYZ, CHANNEL_T, GenericXYZColour<CHANNEL_T> >
{
    public:

        typedef GenericLinearColour<ColourModelXYZ, CHANNEL_T, GenericXYZColour<CHANNEL_T> > Parent;
        typedef ColourModelXYZ      Model;
        typedef CHANNEL_T           Channel;
        typedef GenericXYZColour    DerivedColour;
        using Parent::kChannels;

        template<typename MODEL_T2, typename CHANNEL_T2, typename DERIVED_T2>
        friend class GenericLinearColour;

        template<typename T2>
        friend class GenericXYZColour;

        /// Default constructor.
        inline GenericXYZColour() : Parent() {}

        /// Copy constructor.
        inline GenericXYZColour(const GenericXYZColour& col) : Parent(col) {}

        inline explicit GenericXYZColour(const GenericLinearColour<Model,Channel,DerivedColour>& col) : Parent(col) {}

        template<typename CHANNEL_T2>
        inline explicit GenericXYZColour(const GenericLinearColour<Model,CHANNEL_T2,DerivedColour>& col) : Parent(col) {}

        template<typename CHANNEL_T2>
        inline explicit GenericXYZColour(const GenericXYZColour<CHANNEL_T2>& col) : Parent(col) {}

        inline explicit GenericXYZColour(Channel grey) : Parent(grey) {}

        inline explicit GenericXYZColour(Channel x, Channel y, Channel z) :
            Parent()
        {
            mColour[ColourModelXYZ::kX] = x;
            mColour[ColourModelXYZ::kY] = y;
            mColour[ColourModelXYZ::kZ] = z;
        }

        template<typename DERIVED_T2>
        inline explicit GenericXYZColour(const GenericLinearColour<ColourModelRGB,Channel,DERIVED_T2>& col) : Parent(col, kaColourConversionMatrixRGBtoXYZ) {}

        inline Channel  x() const { return mColour[ColourModelXYZ::kX]; }
        inline Channel& x()       { return mColour[ColourModelXYZ::kX]; }

        inline Channel  y() const { return mColour[ColourModelXYZ::kY]; }
        inline Channel& y()       { return mColour[ColourModelXYZ::kY]; }

        inline Channel  z() const { return mColour[ColourModelXYZ::kZ]; }
        inline Channel& z()       { return mColour[ColourModelXYZ::kZ]; }

    protected:

        using Parent::mColour;
};

typedef GenericXYZColour<ColourChannel>         XYZColour;          ///< Standard precision RGB colour.
typedef GenericXYZColour<PreciseColourChannel>  PreciseXYZColour;   ///< High precision RGB colour.


/// Generic template class to hold and manipulate a colour.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour channels.
///
template<typename CHANNEL_T>
class GenericColour : public GenericLinearColour<ColourModelInternal,CHANNEL_T,GenericColour<CHANNEL_T> >
{
    public:

        typedef GenericLinearColour<ColourModelInternal, CHANNEL_T, GenericColour<CHANNEL_T> > Parent;
        typedef ColourModelInternal Model;
        typedef CHANNEL_T           Channel;
        typedef GenericColour       DerivedColour;
        using Parent::kChannels;

        using Parent::SumAbs;
        using Parent::Max;
        using Parent::MaxAbs;

        template<typename MODEL_T2, typename CHANNEL_T2, typename DERIVED_T2>
        friend class GenericLinearColour;

        template<typename T2>
        friend class GenericColour;

        friend class GenericRGBColour<Channel>;
        friend class GenericRGBTColour<Channel>;
        friend class GenericTransColour<Channel>;

        friend GenericColour ToMathColour(const GenericRGBColour<Channel>& col);

        /// Default constructor.
        inline GenericColour() : Parent() {}

        /// Copy constructor.
        inline GenericColour(const GenericColour& col) : Parent(col) {}

        inline explicit GenericColour(const GenericLinearColour<Model,Channel,DerivedColour>& col) : Parent(col) {}

        template<typename CHANNEL_T2>
        inline explicit GenericColour(const GenericLinearColour<Model,CHANNEL_T2,DerivedColour>& col) : Parent(col) {}

        template<typename CHANNEL_T2>
        inline explicit GenericColour(const GenericColour<CHANNEL_T2>& col) : Parent(col) {}

        inline explicit GenericColour(Channel grey) : Parent(grey) {}

        inline explicit GenericColour(const GenericTransColour<Channel>& col) : Parent(col.colour()) {}

        template<typename BIAS_T2, typename CHANNEL_T2>
        inline explicit GenericColour(const GenericCompactColour<Model,BIAS_T2,CHANNEL_T2>& col) : Parent(col) {}

        inline Channel Red() const
        {
#if (NUM_COLOUR_CHANNELS == 3)
            return mColour[0];
#else
            Channel result = 0.0;
            for (unsigned int i = 0; i < kChannels; i ++)
                result += GenericColour<Channel>::mkR[i] * mColour[i];
            return result;
#endif
        }

        inline Channel Green() const
        {
#if (NUM_COLOUR_CHANNELS == 3)
            return mColour[1];
#else
            Channel result = 0.0;
            for (unsigned int i = 0; i < kChannels; i ++)
                result += GenericColour<Channel>::mkG[i] * mColour[i];
            return result;
#endif
        }

        inline Channel Blue() const
        {
#if (NUM_COLOUR_CHANNELS == 3)
            return mColour[2];
#else
            Channel result = 0.0;
            for (unsigned int i = 0; i < kChannels; i ++)
                result += GenericColour<Channel>::mkB[i] * mColour[i];
            return result;
#endif
        }

        /// Computes the greyscale intensity of the colour.
        ///
        /// @note   Do _not_ use this function if you want to compute some kind of weight; that's
        ///         what @ref WeightGreyscale() is for.
        ///
        inline Channel Greyscale() const
        {
#if (NUM_COLOUR_CHANNELS == 3)
            return kRedIntensity   * mColour[0] +
                   kGreenIntensity * mColour[1] +
                   kBlueIntensity  * mColour[2];
#else
            Channel result;
            for (unsigned int i = 0; i < kChannels; i ++)
                result += mkY[i] * mColour[i];
            return result;
#endif
        }

        /// Computes a generic measure for the weight of the colour.
        inline Channel Weight() const
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
            ///         @ref WeightGreyscale() and @ref WeightAbsGreyscale() are provided.

            return SumAbs() / (Channel)kChannels;
        }

        /// Computes a measure for the weight of the colour based on the magnitude of its greyscale
        /// value.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref Weight()
        ///             for consistency of colour math.
        ///
        inline Channel WeightAbsGreyscale() const
        {
            return fabs(Greyscale());
        }

        /// Computes a measure for the weight of the colour based on its greyscale value.
        ///
        /// @note       Do _not_ use this function if you absolutely want to know the greyscale
        ///             intensity of the colour. For such cases, use @ref Greyscale() instead.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to
        ///             @ref WeightAbsGreyscale() or @ref Weight() for consistency of colour math.
        ///
        inline Channel WeightGreyscale() const
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
        inline Channel WeightMax() const
        {
            return Max();
        }

        /// Computes a measure for the weight of the colour based on the colour channel with the
        /// greatest magnitude.
        ///
        /// @deprecated Calls to this function should probably be replaced by calls to @ref Weight()
        ///             for consistency of colour math.
        ///
        inline Channel WeightMaxAbs() const
        {
            return MaxAbs();
        }

        inline static const GenericColour& DefaultWavelengths()
        {
            return mkDefaultWavelengths;
        }

    protected:

        using Parent::mColour;

    private:

        static const GenericColour mkDefaultWavelengths;

#if (NUM_COLOUR_CHANNELS != 3)

        static const Channel mkY[kChannels];
        static const GenericRGBColour<Channel> mkRGB[kChannels];

#endif

        template<typename T2>
        inline explicit GenericColour(const T2* col)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = (Channel)col[i];
        }

#if (NUM_COLOUR_CHANNELS == 3)
        inline explicit GenericColour(const GenericRGBColour<Channel>& col)
        {
            mColour[0] = col.red();
            mColour[1] = col.green();
            mColour[2] = col.blue();
        }
#else
        #error TODO!
#endif
};

typedef GenericColour<ColourChannel>        MathColour;         ///< Standard precision colour.
typedef GenericColour<PreciseColourChannel> PreciseMathColour;  ///< High precision colour.


/// Generic template class to hold and manipulate a colour plus transparency information.
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

        friend class GenericRGBFTColour<T>;

        friend GenericTransColour ToTransColour(const GenericRGBFTColour<T>& col);

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

        inline explicit GenericTransColour(const GenericColour<T>& col) :
            mColour(col),
            mFilter(0.0),
            mTransm(0.0)
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
/*
        inline T  filter() const { return mFilter; }
        inline T& filter()       { return mFilter; }

        inline T  transm() const { return mTransm; }
        inline T& transm()       { return mTransm; }
*/
        inline T Opacity() const { return 1.0 - mFilter - mTransm; }

        /// Legacy opacity computation.
        ///
        /// @deprecated This fomula was used instead of @ref Opacity() in POV-Ray 3.6 and earlier texture computations.
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

        inline void SetFT(T f, T t)
        {
            mFilter = f;
            mTransm = t;
        }

        inline void GetFT(T& f, T& t) const
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

        inline explicit GenericTransColour(const GenericColour<T>& col, T filter, T transm) :
            mColour(col),
            mFilter(filter),
            mTransm(transm)
        {}

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


/// Generic template class to store a colour in a compact format.
///
/// This class uses RGBE format for compact storage of high dynamic range colours, as originally
/// proposed by Greg Ward.
///
/// @author Christoph Lipka
/// @author Based on MegaPOV HDR code written by Mael and Christoph Hormann
///
/// @tparam MODEL_T     Colour model to use.
/// @tparam BIAS_T      Bias type to use for the exponent.
///                     CompactColourBias<128> matches Greg Ward's original proposal.
/// @tparam CHANNEL_T   Type to use for the colour components.
///                     Defaults to unsigned char.
///
template<typename MODEL_T, typename BIAS_T, typename CHANNEL_T>
class GenericCompactColour
{
    public:

        typedef MODEL_T   Model;
        typedef BIAS_T    Bias;
        typedef CHANNEL_T Channel;
        static const unsigned int kBias         = Bias::kBias;
        static const unsigned int kChannels     = Model::kChannels;
        static const unsigned int kCoefficients = Model::kChannels + 1;
        static const unsigned int kExp          = Model::kChannels;

        template<typename MODEL_T2, typename CHANNEL_T2, typename DERIVED_T2>
        friend class GenericLinearColour;

        typedef Channel Data[kCoefficients+1];

        inline GenericCompactColour()
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mData[i] = 0;
            mData[kExp] = std::numeric_limits<Channel>::min();
        }

        inline GenericCompactColour(const GenericCompactColour& col)
        {
            for (unsigned int i = 0; i < kCoefficients; i ++)
                mData[i] = col.mData[i];
        }

        template<typename DERIVED_T2>
        inline explicit GenericCompactColour(const GenericLinearColour<Model,ColourChannel,DERIVED_T2>& col, ColourChannel dither = 0.0)
        {
            double scaleFactor;
            if (ComputeExponent(col, mData[kExp], scaleFactor))
            {
                for (unsigned int i = 0; i < kChannels; i ++)
                    mData[i] = clipToType<Channel>(floor(col[i] * scaleFactor + 0.5 + dither));
            }
            else
            {
                for (unsigned int i = 0; i < kChannels; i ++)
                    mData[i] = 0;
            }
        }

        template<typename DERIVED_T2>
        inline explicit GenericCompactColour(const GenericLinearColour<Model,ColourChannel,DERIVED_T2>& col, const GenericLinearColour<Model,ColourChannel,DERIVED_T2>& dither)
        {
            double scaleFactor;
            if (ComputeExponent(col, mData[kExp], scaleFactor))
            {
                for (unsigned int i = 0; i < kChannels; i ++)
                    mData[i] = clipToType<Channel>(floor(col[i] * scaleFactor + 0.5 + dither[i]));
            }
            else
            {
                for (unsigned int i = 0; i < kChannels; i ++)
                    mData[i] = 0;
            }
        }

        inline const Data& operator*() const
        {
            return mData;
        }

        inline Data& operator*()
        {
            return mData;
        }

    private:

        Data mData;

        template<typename DERIVED_T2>
        inline static bool ComputeExponent(const GenericLinearColour<Model,ColourChannel,DERIVED_T2>& col, Channel& biasedExponent, double& scaleFactor)
        {
            ColourChannel maxChannel;
            if (std::numeric_limits<Channel>::is_signed)
                maxChannel = col.MaxAbs();
            else
                maxChannel = col.Max();

            if (maxChannel <= 1.0e-32) // TODO - magic number
            {
                biasedExponent = std::numeric_limits<Channel>::min();
                return false;
            }

            int exponent;
            double maxChannelMantissa = frexp(maxChannel, &exponent);
            biasedExponent = clipToType<Channel>(exponent + kBias);

            if (biasedExponent != exponent + kBias)
                maxChannelMantissa = ldexp(maxChannelMantissa, exponent + kBias - biasedExponent);

            scaleFactor = (std::numeric_limits<Channel>::max() + 1.0) * maxChannelMantissa / maxChannel;
            return true;
        }
};

typedef GenericCompactColour<ColourModelRGB,RadianceHDRBias>    RadianceHDRColour;  ///< RGBE format as originally proposed by Greg Ward.
typedef GenericCompactColour<ColourModelInternal,PhotonBias>    PhotonColour;       ///< RGBE format as adapted by Nathan Kopp for photon mapping.

}

#endif
