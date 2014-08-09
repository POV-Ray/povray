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

#include <boost/utility.hpp>
#include <boost/tr1/type_traits.hpp>

#include "base/configbase.h"
#include "base/types.h"

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

template<typename MT, typename CT>  class GenericLinearColour;
template<typename CT>               class GenericRGBColour;
template<typename CT>               class GenericRGBFTColour;
template<typename CT>               class GenericRGBTColour;
template<typename CT>               class GenericXYZColour;
template<typename CT>               class GenericLightColour;
template<typename CT>               class GenericAttenuatingColour;
template<typename CT>               class GenericPseudoColour;
template<typename CT>               class GenericTransColour;
template<typename MT, typename BT, typename CT = unsigned char> class GenericCompactColour;


/// @name Colour Channel Luminance
/// @{
/// @remark    These do not exactly match CCIR Recommendation 601-1, which specifies 0.299, 0.587 and
///            0.114 respectively.
/// @todo      For linear scRGB this should be 0.2126, 0.7152 and 0.0722 respectively.
///
const float kRedIntensity   = 0.297;
const float kGreenIntensity = 0.589;
const float kBlueIntensity  = 0.114;
/// @}


/// Template tagging type identifying the CIE standard illuminant D65.
struct IlluminantD65
{
    static const GenericLightColour<ColourChannel> Colour;
};

/// Template tagging type identifying the CIE standard illuminant E.
struct IlluminantE
{
    static const GenericLightColour<ColourChannel> Colour;
};


/// Template tagging type identifying the linear scRGB colour model.
struct ColourModelRGB
{
    static const unsigned int kChannels = 3;
    typedef IlluminantD65 Whitepoint;

    enum ChannelId
    {
        kRed   = 0,
        kGreen = 1,
        kBlue  = 2
    };

    template<typename MODEL_FROM_T> static inline const ColourChannel* GetConversionMatrix();
    template<typename WHITEPOINT_T> static inline const ColourChannel* GetWhitepointVector();
    static inline const ColourChannel* GetDominantWavelengths();
};

/// Template tagging type identifying the CIE XYZ colour model.
struct ColourModelXYZ
{
    static const unsigned int kChannels = 3;

    enum ChannelId
    {
        kX = 0,
        kY = 1,
        kZ = 2
    };

    template<typename MODEL_FROM_T> static inline const ColourChannel* GetConversionMatrix();
};

/// Template tagging type identifying POV-Ray's custom 3-channel colour model.
struct ColourModelInternal3
{
    static const unsigned int kChannels = 3;
    typedef IlluminantE Whitepoint;

    template<typename MODEL_FROM_T> static inline const ColourChannel* GetConversionMatrix();
    template<typename WHITEPOINT_T> static inline const ColourChannel* GetWhitepointVector();
    static inline const ColourChannel* GetBrightnessVector();
    static inline const ColourChannel* GetDominantWavelengths();
};

/// Template tagging type identifying POV-Ray's custom 4-channel colour model.
struct ColourModelInternal4
{
    static const unsigned int kChannels = 4;
    typedef IlluminantE Whitepoint;

    template<typename MODEL_FROM_T> static inline const ColourChannel* GetConversionMatrix();
    template<typename WHITEPOINT_T> static inline const ColourChannel* GetWhitepointVector();
    static inline const ColourChannel* GetBrightnessVector();
    static inline const ColourChannel* GetMetamericNeutral();
    static inline const ColourChannel* GetDominantWavelengths();

    template<typename CHANNEL_T, typename MODEL_FROM_T> static void MetamericNormalize(CHANNEL_T* col);
};

#if (POV_COLOUR_MODEL == 0)
    typedef ColourModelRGB ColourModelInternal;
#elif (POV_COLOUR_MODEL == 3)
    typedef ColourModelInternal3 ColourModelInternal;
#elif (POV_COLOUR_MODEL == 4)
    typedef ColourModelInternal4 ColourModelInternal;
#else
    #error Colour model not implemented.
#endif

/// @name Colour Space Conversion Matrices
/// @{
///
/// @note   The matrix coefficients for conversion between XYZ and scRGB were re-computed from the primaries' xy
///         coordinates and the whitepoint's xyY coordinates (rather than blindly relying on the contradicting values
///         found on the internet) using the following [Sage](http://www.sagemath.org/) script:
///         @include colour-matrices-srgb.sage
///         Rounded to four decimals they match the coefficients from ITU-R BT.709, which uses the same primaries and
///         white point as s(c)RGB.
///
/// @par    The matrix coefficients for conversion between the 3-channel internal colour space and other colour spaces
///         were computed using the following [Sage](http://www.sagemath.org/) script:
///         @include colour-matrices-internal-3.sage
///
/// @par    The matrix coefficients for conversion between the 4-channel internal colour space and other colour spaces
///         were computed using the following [Sage](http://www.sagemath.org/) script:
///         @include colour-matrices-internal-4.sage
///

/// Matrix to convert from CIE XYZ colour space to scRGB.
const ColourChannel kaColourConversionMatrixRGBfromXYZ[ColourModelRGB::kChannels * ColourModelXYZ::kChannels] = {
    3.24096994190452,    -1.53738317757009,    -0.498610760293003,
   -0.969243636280880,    1.87596750150772,     0.0415550574071757,
    0.0556300796969937,  -0.203976958888977,    1.05697151424288
};

/// Matrix to convert from scRGB colour space to CIE XYZ.
const ColourChannel kaColourConversionMatrixXYZfromRGB[ColourModelXYZ::kChannels * ColourModelRGB::kChannels] = {
    0.412390799265959,    0.357584339383878,    0.180480788401834,
    0.212639005871510,    0.715168678767756,    0.0721923153607337,
    0.0193308187155918,   0.119194779794626,    0.950532152249661
};

/// The D65 illuminant in sRGB colour space
const ColourChannel kaColourVectorD65RGB[ColourModelRGB::kChannels] = {
    1.00000000000000,     1.00000000000000,     1.00000000000000
};

/// The E illuminant in sRGB colour space
const ColourChannel kaColourVectorERGB[ColourModelRGB::kChannels] = {
    1.20497600404143,     0.948278922634016,    0.908624635050896
};

// Approximate dominant wavelengths of primary hues in micrometers.
// Source: 3D Computer Graphics by John Vince (Addison Wesely)
// These are user-adjustable with the irid_wavelength keyword.
// Red = 700 nm, Green = 520 nm, Blue = 480 nm
const ColourChannel kaColourVectorWavelengthsRGB[ColourModelRGB::kChannels] = { 0.700, 0.520, 0.480 };


/// Matrix to convert from CIE XYZ to 3-channel internal colour space.
const ColourChannel kaColourConversionMatrixInternal3fromXYZ[ColourModelInternal3::kChannels * ColourModelXYZ::kChannels] = {
    0.0653372535555397,  -0.129548808774667,    1.06421031298295,
   -0.957019660615546,    1.88478885965394,     0.0722488692764050,
    2.57053180230045,    -1.17476157938952,    -0.395780006749969
};

/// Matrix to convert from scRGB to 3-channel internal colour space.
const ColourChannel kaColourConversionMatrixInternal3fromRGB[ColourModelInternal3::kChannels * ColourModelRGB::kChannels] = {
    0.0199694089414646,   0.0575626421536714,   1.01400580982201,
    0.00751015643441863,  1.01433840346012,     0.0320184820914912,
    0.802612778550075,    0.0318543190400475,   0.00292122624477675
};

/// Matrix to convert from 3-channel internal colour space to CIE XYZ.
const ColourChannel kaColourConversionMatrixXYZfromInternal3[ColourModelXYZ::kChannels * ColourModelInternal3::kChannels] = {
    0.166197054587550,    0.327188401023043,    0.506613795730209,
    0.0485330490923911,   0.694226995340532,    0.257229989041501,
    0.935368251422218,    0.0644221240022947,   0.000209624575486839
};

/// Matrix to convert from 3-channel internal colour space to scRGB
const ColourChannel kaColourConversionMatrixRGBfromInternal3[ColourModelRGB::kChannels * ColourModelInternal3::kChannels] = {
   -0.00235890987508186, -0.0390066952245556,   1.24635450512786,
   -0.0311697332965042,   0.987899071432897,   -0.00846838674729583,
    0.988003528713091,   -0.0553124445059018,  -0.0240644578626873
};

/// The D65 illuminant in 3-channel internal colour space
const ColourChannel kaColourVectorD65Internal3[ColourModelInternal3::kChannels] = {
    1.09153786091715,     1.05386704198602,     0.837388323834899
};

/// The E illuminant in 3-channel internal colour space
const ColourChannel kaColourVectorEInternal3[ColourModelInternal3::kChannels] = {
    1.00000000000000,     1.00000000000000,     1.00000000000000
};

// TODO
const ColourChannel kaColourVectorWavelengthsInternal3[ColourModelInternal3::kChannels] = { 0.480, 0.520, 0.700 };


/// Matrix to convert from CIE XYZ to 4-channel internal colour space.
const ColourChannel kaColourConversionMatrixInternal4fromXYZ[ColourModelInternal4::kChannels * ColourModelXYZ::kChannels] = {
    0.129204920286312,   -0.197075661344053,    1.06786887362857,
   -1.61899270878818,     2.44475000212442,     0.174265860253402,
    0.249860116439757,    0.834269091560271,   -0.0841207061757157,
    2.11871294166769,    -0.776211819003875,   -0.342507272604833
};

/// Matrix to convert from scRGB to 4-channel internal colour space.
const ColourChannel kaColourConversionMatrixInternal4fromRGB[ColourModelInternal4::kChannels * ColourModelRGB::kChannels] = {
    0.0320197272444340,   0.0325437109667125,   1.02413535635558,
   -0.144439785373934,    1.19025377153608,     0.0499403858429128,
    0.278812041260671,    0.675962439574642,    0.0253633322688891,
    0.702063867895276,    0.161671107613207,    0.000786278694865472
};

/// Matrix to convert from 4-channel internal colour space to CIE XYZ.
const ColourChannel kaColourConversionMatrixXYZfromInternal4[ColourModelXYZ::kChannels * ColourModelInternal4::kChannels] = {
    0.165448161433383,    0.0705307151473408,   0.281417483251090,    0.482602891508988,
    0.0436662963923049,   0.370396096865270,    0.346408120331993,    0.239519519884856,
    0.924485086240860,    0.0741951679663702,   0.00113585637721942,  0.000183889415549839
};

/// Matrix to convert from 4-channel internal colour space to scRGB.
const ColourChannel kaColourConversionMatrixRGBfromInternal4[ColourModelRGB::kChannels * ColourModelInternal4::kChannels] = {
    0.00812247691854345, -0.377847309694507,    0.378937237359290,    1.19577649544490,
   -0.0400259938296935,   0.629572778049831,    0.377135471798856,   -0.0184213046298963,
    0.977451377561174,    0.00679353891672695, -0.0538034300633890,  -0.0218148600700092
};

/// The D65 illuminant in 4-channel internal colour space (canonical form)
const ColourChannel kaColourVectorD65Internal4[ColourModelInternal4::kChannels] = {
    1.09040555996188,     1.07398736922403,     1.01576775863718,     0.846340616087617
};

/// The E illuminant in 4-channel internal colour space
const ColourChannel kaColourVectorEInternal4[ColourModelInternal4::kChannels] = {
    1.00000000000000,     1.00000000000000,     1.00000000000000,     1.00000000000000
};

/// The metamerically neutral colour in 4-channel internal colour space (canonical form)
const ColourChannel kaColourVectorNeutralInternal4[ColourModelInternal4::kChannels] = {
   -0.0479025541469982,   0.610918777882433,   -1.00000000000000,     0.510262865793667
};

// TODO
const ColourChannel kaColourVectorWavelengthsInternal4[ColourModelInternal4::kChannels] = { 0.4650, 0.5200, 0.5725, 0.6150 };

/// @}


template<> inline const ColourChannel* ColourModelXYZ::GetConversionMatrix<ColourModelRGB>()        { return kaColourConversionMatrixXYZfromRGB; }
template<> inline const ColourChannel* ColourModelXYZ::GetConversionMatrix<ColourModelInternal3>()  { return kaColourConversionMatrixXYZfromInternal3; }
template<> inline const ColourChannel* ColourModelXYZ::GetConversionMatrix<ColourModelInternal4>()  { return kaColourConversionMatrixXYZfromInternal4; }

template<> inline const ColourChannel* ColourModelRGB::GetConversionMatrix<ColourModelXYZ>()        { return kaColourConversionMatrixRGBfromXYZ; }
template<> inline const ColourChannel* ColourModelRGB::GetConversionMatrix<ColourModelInternal3>()  { return kaColourConversionMatrixRGBfromInternal3; }
template<> inline const ColourChannel* ColourModelRGB::GetConversionMatrix<ColourModelInternal4>()  { return kaColourConversionMatrixRGBfromInternal4; }
template<> inline const ColourChannel* ColourModelRGB::GetWhitepointVector<IlluminantD65>()         { return kaColourVectorD65RGB; }
template<> inline const ColourChannel* ColourModelRGB::GetWhitepointVector<IlluminantE>()           { return kaColourVectorERGB; }
           inline const ColourChannel* ColourModelRGB::GetDominantWavelengths()                     { return kaColourVectorWavelengthsRGB; }

template<> inline const ColourChannel* ColourModelInternal3::GetConversionMatrix<ColourModelXYZ>()  { return kaColourConversionMatrixInternal3fromXYZ; }
template<> inline const ColourChannel* ColourModelInternal3::GetConversionMatrix<ColourModelRGB>()  { return kaColourConversionMatrixInternal3fromRGB; }
template<> inline const ColourChannel* ColourModelInternal3::GetWhitepointVector<IlluminantD65>()   { return kaColourVectorD65Internal3; }
template<> inline const ColourChannel* ColourModelInternal3::GetWhitepointVector<IlluminantE>()     { return kaColourVectorEInternal3; }
           inline const ColourChannel* ColourModelInternal3::GetBrightnessVector()                  { return kaColourConversionMatrixXYZfromInternal3 + kChannels * ColourModelXYZ::kY; }
           inline const ColourChannel* ColourModelInternal3::GetDominantWavelengths()               { return kaColourVectorWavelengthsInternal3; }

template<> inline const ColourChannel* ColourModelInternal4::GetConversionMatrix<ColourModelXYZ>()  { return kaColourConversionMatrixInternal4fromXYZ; }
template<> inline const ColourChannel* ColourModelInternal4::GetConversionMatrix<ColourModelRGB>()  { return kaColourConversionMatrixInternal4fromRGB; }
template<> inline const ColourChannel* ColourModelInternal4::GetWhitepointVector<IlluminantD65>()   { return kaColourVectorD65Internal4; }
template<> inline const ColourChannel* ColourModelInternal4::GetWhitepointVector<IlluminantE>()     { return kaColourVectorEInternal4; }
           inline const ColourChannel* ColourModelInternal4::GetBrightnessVector()                  { return kaColourConversionMatrixXYZfromInternal4 + kChannels * ColourModelXYZ::kY; }
           inline const ColourChannel* ColourModelInternal4::GetMetamericNeutral()                  { return kaColourVectorNeutralInternal4; }
           inline const ColourChannel* ColourModelInternal4::GetDominantWavelengths()               { return kaColourVectorWavelengthsInternal4; }


/// Generic template tagging type identifying the bias for a @ref GenericCompactColour specialization.
template<int BIAS>
struct CompactColourBias
{
    static const unsigned int kBias = BIAS;
};

typedef CompactColourBias<128>  RadianceHDRBias;    ///< Template tagging type identifying the bias used in the @ref GenericCompactColour specialization originally proposed by Gred Ward.
typedef CompactColourBias<250>  PhotonBias;         ///< Template tagging type identifying the bias used in the @ref GenericCompactColour specialization for photons.


/// Highly generic template class to hold and process colours.
///
/// Any colour model can be used as long as it is based on a linear combination of multiple coefficients.
///
/// This type is intended to be used exclusively as the base for more specialized types. All functionality is protected,
/// and derived types should expose only whatever arithmetic and conversion operations they actually need to provide.
///
/// @author Christoph Lipka
///
/// @tparam MODEL_T     Tagging type identifying the colour model to use.
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour components.
///
template<typename MODEL_T, typename CHANNEL_T>
class GenericLinearColour
{
    protected:

        typedef MODEL_T   Model;
        typedef CHANNEL_T Channel;
        static const unsigned int kChannels = Model::kChannels;

        Channel mColour[kChannels];

        template<typename MODEL_T2, typename CHANNEL_T2>
        friend class GenericLinearColour;

        template<typename MODEL_T2, typename BIAS_T2, typename CHANNEL_T2>
        friend class GenericCompactColour;

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

        /// Construct from sibling type with different precision.
        template<typename CHANNEL_T2>
        inline explicit GenericLinearColour(const GenericLinearColour<Model,CHANNEL_T2>& col)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = col.mColour[i];
        }

        /// Construct from sibling type with different colour model.
        /// Whitepoint compensation can be applied optionally.
        template<typename MODEL_T2>
        inline explicit GenericLinearColour(const GenericLinearColour<MODEL_T2, Channel>& col)
        {
            const ColourChannel *pMatrixElement = Model::template GetConversionMatrix<MODEL_T2>();
            for (unsigned int i = 0; i < kChannels; i ++)
            {
                mColour[i] = 0.0;
                for (unsigned int j = 0; j < MODEL_T2::kChannels; j ++)
                    mColour[i] += *(pMatrixElement++) * col.mColour[j];
            }
        }

        /// Construct from shared-exponent format.
        template<typename BIAS_T2, typename CHANNEL_T2>
        inline explicit GenericLinearColour(const GenericCompactColour<Model,BIAS_T2,CHANNEL_T2>& col)
        {
            typedef GenericCompactColour<Model,BIAS_T2,CHANNEL_T2> CompactColour;
            if (col[CompactColour::kExp] > std::numeric_limits<typename CompactColour::Channel>::min())
            {
                double expFactor = ldexp(1.0,(int)col[CompactColour::kExp]-(int)(CompactColour::kBias+8));
                for (unsigned int i = 0; i < kChannels; i ++)
                    mColour[i] = col[i] * expFactor;
            }
            else
            {
                for (unsigned int i = 0; i < kChannels; i ++)
                    mColour[i] = 0.0;
            }
        }

        /// Construct from a C-style array of values.
        template<typename CHANNEL_T2>
        inline explicit GenericLinearColour(const CHANNEL_T2* col)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = (Channel)col[i];
        }

        /// Construct with all channels set to same value.
        inline explicit GenericLinearColour(Channel grey)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = grey;
        }

        /// Default assignment operator.
        inline GenericLinearColour& operator=(const GenericLinearColour& col) { SetEqual(col); return *this; }

        /// Provides direct access to an individual channel.
        inline Channel  channel(unsigned int idx) const { assert(idx < kChannels); return mColour[idx]; }

        /// Provides direct access to an individual channel.
        inline Channel& channel(unsigned int idx)       { assert(idx < kChannels); return mColour[idx]; }


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

        /// Computes the intensity of the colour channel with the smallest magnitude.
        inline Channel MinAbs() const
        {
            Channel result = fabs(mColour[0]);
            for (unsigned int i = 1; i < kChannels; i ++)
                result = min(result, fabs(mColour[i]));
            return result;
        }

        /// Computes the average of the channels' values.
        inline Channel Average() const
        {
            return Sum() / Model::kChannels;
        }

        /// Computes the average of the channels' magnitudes.
        inline Channel AverageAbs() const
        {
            return SumAbs() / Model::kChannels;
        }

        /// Computes the brightness of the colour.
        inline Channel Greyscale() const
        {
            const ColourChannel *pMatrixElement = Model::GetBrightnessVector();
            Channel result = 0;
            for (unsigned int i = 0; i < kChannels; i ++)
                result += pMatrixElement[i] * mColour[i];
            return result;
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

            return AverageAbs();
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

        inline void SetEqual(const GenericLinearColour& src)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = src.mColour[i];
        }

        inline void Clear()
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = 0.0;
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


        inline void SetClipped(const GenericLinearColour& src, Channel minc, Channel maxc)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = clip(src.mColour[i], minc, maxc);
        }

        inline void SetClippedUpper(const GenericLinearColour& src, Channel maxc)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = min(src.mColour[i], maxc);
        }

        inline void SetClippedLower(const GenericLinearColour& src, Channel minc)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = max(src.mColour[i], minc);
        }


        inline void SetNegative(const GenericLinearColour& src)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = -src.mColour[i];
        }

        inline void SetInverse(const GenericLinearColour& src)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = Channel(1.0) / src.mColour[i];
        }


        inline void SetSum(const GenericLinearColour& a, const GenericLinearColour& b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = a.mColour[i] + b.mColour[i];
        }

        inline void SetDifference(const GenericLinearColour& a, const GenericLinearColour& b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = a.mColour[i] - b.mColour[i];
        }

        inline void SetProduct(const GenericLinearColour& a, const GenericLinearColour& b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = a.mColour[i] * b.mColour[i];
        }

        inline void SetQuotient(const GenericLinearColour& a, const GenericLinearColour& b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = a.mColour[i] / b.mColour[i];
        }


        inline void SetSum(const GenericLinearColour& a, Channel b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = a.mColour[i] + b;
        }

        inline void SetDifference(const GenericLinearColour& a, Channel b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = a.mColour[i] - b;
        }

        inline void SetDifference(Channel a, const GenericLinearColour& b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = a - b.mColour[i];
        }

        inline void SetProduct(const GenericLinearColour& a, Channel b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = a.mColour[i] * b;
        }

        inline void SetQuotient(const GenericLinearColour& a, Channel b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = a.mColour[i] / b;
        }

        inline void SetQuotient(Channel a, const GenericLinearColour& b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = a / b.mColour[i];
        }


        inline void SetSqrt(const GenericLinearColour& src)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = sqrt(src.mColour[i]);
        }

        inline void SetExp(const GenericLinearColour& src)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = exp(src.mColour[i]);
        }

        inline void SetPow(const GenericLinearColour& a, Channel b)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = pow(a.mColour[i], b);
        }

        inline void SetCos(const GenericLinearColour& src)
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mColour[i] = cos(src.mColour[i]);
        }
};

/// Specialized greyscale computation for scRGB colour model using bogus legacy weighting.
template<>
inline ColourChannel GenericLinearColour<ColourModelRGB, ColourChannel>::Greyscale() const
{
    return kRedIntensity   * mColour[Model::kRed]   +
           kGreenIntensity * mColour[Model::kGreen] +
           kBlueIntensity  * mColour[Model::kBlue];
}

/// Specialized greyscale computation for scRGB colour model using bogus legacy weighting.
template<>
inline PreciseColourChannel GenericLinearColour<ColourModelRGB, PreciseColourChannel>::Greyscale() const
{
    return kRedIntensity   * mColour[Model::kRed]   +
           kGreenIntensity * mColour[Model::kGreen] +
           kBlueIntensity  * mColour[Model::kBlue];
}

/// Specialized greyscale computation for XYZ colour model.
template<>
inline ColourChannel GenericLinearColour<ColourModelXYZ, ColourChannel>::Greyscale() const
{
    return mColour[Model::kY];
}

/// Specialized greyscale computation for XYZ colour model.
template<>
inline PreciseColourChannel GenericLinearColour<ColourModelXYZ, PreciseColourChannel>::Greyscale() const
{
    return mColour[Model::kY];
}


/// Generic template class to hold and process RGB colours.
///
/// @note       This colour type is provided solely for use in the front-end and image handling code. Use
///             @ref GenericLightColour or @ref GenericAttenuatingColour in the render engine instead.
///
/// @deprecated This type is intended for use with linear scRGB colours. Use for other gamuts or non-linear colour
///             representations is discouraged.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour components.
///
template<typename CHANNEL_T>
class GenericRGBColour : public GenericLinearColour<ColourModelRGB, CHANNEL_T>
{
    public:

        typedef ColourModelRGB      Model;
        typedef CHANNEL_T           Channel;
        typedef Model::Whitepoint   Whitepoint;
        typedef GenericLinearColour<Model, Channel> Parent;
        using Parent::kChannels;

        /// Default constructor.
        inline GenericRGBColour() : Parent() {}

        /// Copy constructor.
        inline GenericRGBColour(const GenericRGBColour& col) : Parent(col) {}

        /// Construct from sibling type with different precision.
        template<typename CHANNEL_T2>
        inline explicit GenericRGBColour(const GenericRGBColour<CHANNEL_T2>& col) : Parent(col) {}

        inline explicit GenericRGBColour(Channel grey) : Parent(grey) {}

        inline explicit GenericRGBColour(Channel red, Channel green, Channel blue) : Parent()
        {
            mColour[Model::kRed]   = red;
            mColour[Model::kGreen] = green;
            mColour[Model::kBlue]  = blue;
        }

        inline explicit GenericRGBColour(const GenericRGBFTColour<Channel>& col) : Parent(col.rgb()) {}

        /// Convert from CIE XYZ
        inline explicit GenericRGBColour(const GenericXYZColour<Channel>& col) : Parent(col) {}

        /// Construct from shared-exponent format.
        template<typename BIAS_T2, typename CHANNEL_T2>
        inline explicit GenericRGBColour(const GenericCompactColour<Model,BIAS_T2,CHANNEL_T2>& col) : Parent(col) {}

        inline explicit GenericRGBColour(const GenericLightColour<Channel>& col) : Parent(col) {}

        inline explicit GenericRGBColour(const GenericAttenuatingColour<Channel>& col) :
#if (POV_COLOUR_MODEL == 0)
            Parent(col)
#else
            Parent(col * ColourModelRGB::Whitepoint::Colour)
#endif
        {}

        inline Channel  operator[](unsigned int i) const { return this->channel(i); }
        inline Channel& operator[](unsigned int i)       { return this->channel(i); }

        inline Channel  red()   const { return mColour[Model::kRed]; }
        inline Channel& red()         { return mColour[Model::kRed]; }

        inline Channel  green() const { return mColour[Model::kGreen]; }
        inline Channel& green()       { return mColour[Model::kGreen]; }

        inline Channel  blue()  const { return mColour[Model::kBlue]; }
        inline Channel& blue()        { return mColour[Model::kBlue]; }

        using Parent::Sum;
        using Parent::SumAbs;
        using Parent::Max;
        using Parent::MaxAbs;
        using Parent::Min;
        using Parent::MinAbs;
        using Parent::Average;
        using Parent::AverageAbs;
        using Parent::Greyscale;

        using Parent::Clear;
        using Parent::IsZero;
        using Parent::IsNearZero;
        using Parent::Invalidate;
        using Parent::IsValid;

        inline void Clip      (Channel minc, Channel maxc) { this->SetClipped     (*this, minc, maxc); }
        inline void ClipUpper (Channel maxc)               { this->SetClippedUpper(*this, maxc); }
        inline void ClipLower (Channel minc)               { this->SetClippedLower(*this, minc); }

        inline GenericRGBColour Clipped      (Channel minc, Channel maxc) const { GenericRGBColour result; result.SetClipped     (*this, minc, maxc); return result; }
        inline GenericRGBColour ClippedUpper (Channel maxc) const               { GenericRGBColour result; result.SetClippedUpper(*this, maxc);       return result; }
        inline GenericRGBColour ClippedLower (Channel minc) const               { GenericRGBColour result; result.SetClippedLower(*this, minc);       return result; }

        inline GenericRGBColour& operator=  (const GenericRGBColour& b) { this->SetEqual     (b);        return *this; }
        inline GenericRGBColour& operator+= (const GenericRGBColour& b) { this->SetSum       (*this, b); return *this; }
        inline GenericRGBColour& operator-= (const GenericRGBColour& b) { this->SetDifference(*this, b); return *this; }
        inline GenericRGBColour& operator*= (Channel b)                 { this->SetProduct   (*this, b); return *this; }
        inline GenericRGBColour& operator/= (Channel b)                 { this->SetQuotient  (*this, b); return *this; }

        inline GenericRGBColour operator+ (const GenericRGBColour& b) const { GenericRGBColour result; result.SetSum       (*this, b); return result; }
        inline GenericRGBColour operator- (const GenericRGBColour& b) const { GenericRGBColour result; result.SetDifference(*this, b); return result; }
        inline GenericRGBColour operator* (Channel b) const                 { GenericRGBColour result; result.SetProduct   (*this, b); return result; }
        inline GenericRGBColour operator/ (Channel b) const                 { GenericRGBColour result; result.SetQuotient  (*this, b); return result; }

        template<typename T> friend inline typename boost::enable_if<std::tr1::is_arithmetic<T>, GenericRGBColour>::type operator* (T a, const GenericRGBColour& b) { return b * a; }

        friend inline GenericRGBColour Sqr(const GenericRGBColour& a) { GenericRGBColour result; result.SetProduct(a, a); return result; }

        friend inline Channel ColourDistance (const GenericRGBColour& a, const GenericRGBColour& b) { GenericRGBColour diff; diff.SetDifference(a, b); return diff.SumAbs(); }

    protected:

        using Parent::mColour;
};

typedef GenericRGBColour<ColourChannel>         RGBColour;          ///< Standard precision RGB colour.
typedef GenericRGBColour<PreciseColourChannel>  PreciseRGBColour;   ///< High precision RGB colour.

/// Generic template class to hold transparency information using Filter/Transmit format.
template<typename CHANNEL_T>
class GenericFilterTransm
{
    public:
        
        typedef CHANNEL_T Channel;

        /// Default constructor. 
        inline GenericFilterTransm() :
            mFilter(0.0),
            mTransm(0.0)
        {}

        /// Copy constructor.
        inline GenericFilterTransm(const GenericFilterTransm& ft) :
            mFilter(ft.mFilter),
            mTransm(ft.mTransm)
        {}

        /// Construct from sibling type with different precision.
        template<typename CHANNEL_T2>
        inline explicit GenericFilterTransm(const GenericFilterTransm<CHANNEL_T2>& ft) :
            mFilter(ft.filter()),
            mTransm(ft.transm())
        {}

        /// Construct from filter and transmit coefficients
        inline explicit GenericFilterTransm(Channel filter, Channel transm) :
            mFilter(filter),
            mTransm(transm)
        {}

        inline void Clear()
        {
            mFilter = 0.0;
            mTransm = 0.0;
        }

        template<typename COLOUR_T2>
        inline Channel Opacity(const COLOUR_T2&) const
        {
            return 1.0 - mFilter - mTransm;
        }

        /// Legacy opacity computation.
        ///
        /// @deprecated This fomula was used instead of @ref Opacity() in POV-Ray 3.6 and earlier texture computations.
        ///             Do not use it - it is bogus, and we're only keeping it around for compatibility with legacy
        ///             scenes.
        ///
        template<typename COLOUR_T2>
        inline Channel LegacyOpacity(const COLOUR_T2& col) const
        {
            return 1.0 - mFilter * col.Max() - mTransm;
        }

        template<typename COLOUR_T2>
        inline COLOUR_T2 TransmittedColour(const COLOUR_T2& col) const
        {
            return col * mFilter + COLOUR_T2(mTransm);
        }

        inline GenericFilterTransm Clipped(Channel minc, Channel maxc)
        {
            return GenericFilterTransm(pov_base::clip<Channel>(mFilter, minc, maxc),
                                       pov_base::clip<Channel>(mTransm, minc, maxc));
        }

        inline Channel  filter() const { return mFilter; }
        inline Channel& filter()       { return mFilter; }

        inline Channel  transm() const { return mTransm; }
        inline Channel& transm()       { return mTransm; }

        inline GenericFilterTransm& operator+= (const GenericFilterTransm& b) { mFilter += b.mFilter; mTransm += b.mTransm; return *this; }
        inline GenericFilterTransm& operator-= (const GenericFilterTransm& b) { mFilter -= b.mFilter; mTransm -= b.mTransm; return *this; }
        inline GenericFilterTransm& operator*= (Channel b)                    { mFilter *= b;         mTransm *= b;         return *this; }
        inline GenericFilterTransm& operator/= (Channel b)                    { mFilter /= b;         mTransm /= b;         return *this; }

        inline GenericFilterTransm operator+ (const GenericFilterTransm& b) const { GenericFilterTransm result(*this); result += b; return result; }
        inline GenericFilterTransm operator- (const GenericFilterTransm& b) const { GenericFilterTransm result(*this); result -= b; return result; }
        inline GenericFilterTransm operator* (Channel b) const                    { GenericFilterTransm result(*this); result *= b; return result; }
        inline GenericFilterTransm operator/ (Channel b) const                    { GenericFilterTransm result(*this); result /= b; return result; }

    protected:

        Channel mFilter;
        Channel mTransm;
};

typedef GenericFilterTransm<ColourChannel> FilterTransm;


/// Generic template class to hold transparency information using a full fledged colour format.
template<typename COLOUR_T>
class GenericTrans
{
    public:

        typedef COLOUR_T                    Colour;
        typedef typename COLOUR_T::Channel  Channel;

        template<typename T2> friend class GenericTrans;

        /// Default constructor. 
        inline GenericTrans() :
            mData()
        {}

        /// Copy constructor.
        inline GenericTrans(const GenericTrans& t) :
            mData(t.mData)
        {}

        /// Construct from sibling type with different precision.
        template<typename COLOUR_T2>
        inline explicit GenericTrans(const GenericTrans<COLOUR_T2>& t) :
            mData(t.mData)
        {}

        inline explicit GenericTrans(const Colour& col) :
            mData(col)
        {}

        inline explicit GenericTrans(const Colour& col, Channel filter, Channel transm) :
            mData(col * filter + transm)
        {}

        inline explicit GenericTrans(const Colour& col, const GenericTrans<Colour>& t) :
            mData(t.mData)
        {}

        inline explicit GenericTrans(const Colour& col, const GenericFilterTransm<Channel>& t) :
            mData(t.mData)
        {}

        inline void Clear()
        {
            mData.Clear();
        }

        inline Channel Opacity(const Colour&) const
        {
            return 1.0 - mData.Greyscale();
        }

        inline Channel LegacyOpacity(const Colour&) const
        {
            return Opacity(col);
        }

        inline const Colour& TransmitColour(const Colour&) const
        {
            return mData;
        }

        inline Colour TransmitColour(const Colour&)
        {
            return COLOUR_T2(mData);
        }

        inline GenericTrans Clipped(Channel minc, Channel maxc)
        {
            return GenericTrans(mData.Clipped());
        }

    protected:

        Colour mData;
};


/// Generic template class to hold and process RGB colours with associated Filter and Transmit components.
///
/// @deprecated This colour type provides the legacy RGBFT transparent colour model exposed in the scene description
///             language, and should not be used anywhere else. Instead, use @ref GenericTransColour in the render
///             engine, and @ref GenericRGBTColour in the front-end.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour components.
///
template<typename CHANNEL_T>
class GenericRGBFTColour
{
    public:

        typedef CHANNEL_T Channel;

        typedef DBL EXPRESS[5];

        /// Default constructor.
        inline GenericRGBFTColour() :
            mColour(),
            mTrans()
        {}

        /// Copy constructor.
        inline GenericRGBFTColour(const GenericRGBFTColour& col) :
            mColour(col.mColour),
            mTrans(col.mTrans)
        {}

        /// Construct from sibling type with different precision.
        template<typename CHANNEL_T2>
        inline explicit GenericRGBFTColour(const GenericRGBFTColour<CHANNEL_T2>& col) :
            mColour(col.rgb()),
            mTrans(col.trans())
        {}

        inline explicit GenericRGBFTColour(const GenericRGBColour<Channel>& col) :
            mColour(col),
            mTrans()
        {}

        inline explicit GenericRGBFTColour(const GenericRGBColour<Channel>& col, const GenericFilterTransm<Channel>& ft) :
            mColour(col),
            mTrans(ft)
        {}

        inline explicit GenericRGBFTColour(const GenericRGBColour<Channel>& col, Channel filter, Channel transm) :
            mColour(col),
            mTrans(filter, transm)
        {}

        inline explicit GenericRGBFTColour(const GenericRGBTColour<Channel>& col) :
            mColour(col.rgb()),
            mTrans(0.0, col.transm())
        {}

        inline explicit GenericRGBFTColour(Channel red, Channel green, Channel blue, Channel filter, Channel transm) :
            mColour(red, green, blue),
            mTrans(filter, transm)
        {}

        inline explicit GenericRGBFTColour(const EXPRESS expr) :
            mColour(expr[0], expr[1], expr[2]),
            mTrans(expr[3], expr[4])
        {}

        inline explicit GenericRGBFTColour(const GenericTransColour<Channel>& col) :
            mColour(col.colour()),
            mTrans(col.trans())
        {}

        inline GenericRGBColour<Channel>  rgb() const { return mColour; }
        inline GenericRGBColour<Channel>& rgb()       { return mColour; }

        inline GenericFilterTransm<Channel>  trans() const { return mTrans; }
        inline GenericFilterTransm<Channel>& trans()       { return mTrans; }

        inline Channel  red()    const { return mColour.red(); }
        inline Channel& red()          { return mColour.red(); }

        inline Channel  green()  const { return mColour.green(); }
        inline Channel& green()        { return mColour.green(); }

        inline Channel  blue()   const { return mColour.blue(); }
        inline Channel& blue()         { return mColour.blue(); }

        inline Channel  filter() const { return mTrans.filter(); }
        inline Channel& filter()       { return mTrans.filter(); }

        inline Channel  transm() const { return mTrans.transm(); }
        inline Channel& transm()       { return mTrans.transm(); }

        inline Channel Opacity()   const { return mTrans.Opacity(mColour); }

        // TODO: find a more correct way of handling alpha <-> filter/transmit
        inline static void AtoFT(Channel alpha, Channel& f, Channel& t) { f = Channel(0.0); t = Channel(1.0) - alpha; }
        inline void AtoFT(Channel alpha) { mTrans = GenericFilterTransm<Channel>(Channel(0.0), Channel(1.0) - alpha); }
        inline static Channel FTtoA(Channel /*f*/, Channel t) { return Channel(1.0) - t; }
        inline Channel FTtoA() const { return Channel(1.0) - mTrans.transm(); }

        inline void Clear()
        {
            mColour.Clear();
            mTrans.Clear();
        }

        inline void Get(EXPRESS expr, unsigned int n) const
        {
            if (n > 0) expr[0] = mColour.red();
            if (n > 1) expr[1] = mColour.green();
            if (n > 2) expr[2] = mColour.blue();
            if (n > 3) expr[3] = mTrans.filter();
            if (n > 4) expr[4] = mTrans.transm();
        }

        inline void Set(const EXPRESS expr, unsigned int n)
        {
            if (n > 0) mColour.red()   = expr[0];
            if (n > 1) mColour.green() = expr[1];
            if (n > 2) mColour.blue()  = expr[2];
            if (n > 3) mTrans.filter() = expr[3];
            if (n > 4) mTrans.transm() = expr[4];
        }

        inline GenericRGBFTColour Clipped(Channel minc, Channel maxc) const
        {
            return GenericRGBFTColour(mColour.Clipped(minc, maxc),
                                      mTrans .Clipped(minc, maxc));
        }

        inline GenericRGBColour<Channel> TransmittedColour() const
        {
            return mTrans.TransmittedColour(mColour);
        }

        inline GenericRGBFTColour operator- () const { return GenericRGBFTColour(-mColour, -mTrans); }


        inline GenericRGBFTColour& operator=  (const GenericRGBFTColour& b) { mColour =  b.mColour; mTrans =  b.mTrans; return *this; }
        inline GenericRGBFTColour& operator+= (const GenericRGBFTColour& b) { mColour += b.mColour; mTrans += b.mTrans; return *this; }
        inline GenericRGBFTColour& operator-= (const GenericRGBFTColour& b) { mColour -= b.mColour; mTrans -= b.mTrans; return *this; }
        inline GenericRGBFTColour& operator*= (Channel b)                   { mColour *= b;         mTrans *= b;        return *this; }
        inline GenericRGBFTColour& operator/= (Channel b)                   { mColour /= b;         mTrans /= b;        return *this; }

        inline GenericRGBFTColour operator+ (const GenericRGBFTColour& b) const { GenericRGBFTColour result(*this); result += b; return result; }
        inline GenericRGBFTColour operator- (const GenericRGBFTColour& b) const { GenericRGBFTColour result(*this); result -= b; return result; }
        inline GenericRGBFTColour operator* (Channel b) const                   { GenericRGBFTColour result(*this); result *= b; return result; }
        inline GenericRGBFTColour operator/ (Channel b) const                   { GenericRGBFTColour result(*this); result /= b; return result; }

        template<typename T> friend inline typename boost::enable_if<std::tr1::is_arithmetic<T>, GenericRGBFTColour>::type operator* (T a, const GenericRGBFTColour& b) { return  b * a; }

        friend inline Channel ColourDistanceRGBT (const GenericRGBFTColour& a, const GenericRGBFTColour& b)
        { return ColourDistance(a.mColour, b.mColour) + fabs(a.mTrans.transm() - b.mTrans.transm()); }

    protected:

        GenericRGBColour<Channel>       mColour;
        GenericFilterTransm<Channel>    mTrans;
};

typedef GenericRGBFTColour<ColourChannel>           RGBFTColour;        ///< Standard precision RGBFT colour.
typedef GenericRGBFTColour<PreciseColourChannel>    PreciseRGBFTColour; ///< High precision RGBFT colour.


/// Generic template class to hold and process RGB colours with an associated Transmit component.
///
/// @note       This colour type is provided solely for use in the front-end. Use @ref GenericTransColour in the render
///             engine instead.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour components.
///
template<typename CHANNEL_T>
class GenericRGBTColour
{
    public:

        typedef CHANNEL_T Channel;

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

        /// Construct from sibling type with different precision.
        template<typename CHANNEL_T2>
        inline explicit GenericRGBTColour(const GenericRGBTColour<CHANNEL_T2>& col) :
            mColour(col.rgb()),
            mTransm(col.transm())
        {}

        inline explicit GenericRGBTColour(const GenericRGBColour<Channel>& col, Channel transm) :
            mColour(col),
            mTransm(transm)
        {}

        inline explicit GenericRGBTColour(Channel red, Channel green, Channel blue, Channel transm) :
            mColour(red, green, blue),
            mTransm(transm)
        {}

/*
        inline explicit GenericRGBTColour(const GenericTransColour<Channel>& col) :
            mColour(col.colour()),
            mTransm(col.transm())
        {}
*/

        inline GenericRGBColour<Channel>  rgb() const { return mColour; }
        inline GenericRGBColour<Channel>& rgb()       { return mColour; }

        inline Channel  red()    const { return mColour.red(); }
        inline Channel& red()          { return mColour.red(); }

        inline Channel  green()  const { return mColour.green(); }
        inline Channel& green()        { return mColour.green(); }

        inline Channel  blue()   const { return mColour.blue(); }
        inline Channel& blue()         { return mColour.blue(); }

        inline Channel  transm() const { return mTransm; }
        inline Channel& transm()       { return mTransm; }

        inline Channel  Alpha()  const { return Channel(1.0) - mTransm; }

        inline bool IsNearZero(Channel epsilon) const
        {
            return mColour.IsNearZero(epsilon) &&
                   (fabs(mTransm) < epsilon);
        }

        inline void Clear()
        {
            mColour.Clear();
            mTransm = 0.0;
        }

        inline GenericRGBTColour Clipped(Channel minc, Channel maxc) const
        {
            return GenericRGBTColour(mColour.Clipped(minc, maxc),
                                     pov_base::clip<Channel>(mTransm, minc, maxc));
        }

        inline GenericRGBColour<Channel> TransmittedColour() const
        {
            return GenericRGBColour<Channel>(mTransm);
        }

        inline GenericRGBTColour& operator=  (const GenericRGBTColour& b) { mColour =  b.mColour; mTransm =  b.mTransm; return *this; }
        inline GenericRGBTColour& operator+= (const GenericRGBTColour& b) { mColour += b.mColour; mTransm += b.mTransm; return *this; }
        inline GenericRGBTColour& operator-= (const GenericRGBTColour& b) { mColour -= b.mColour; mTransm -= b.mTransm; return *this; }
        inline GenericRGBTColour& operator*= (double b)                   { mColour *= b;         mTransm *= b;         return *this; }
        inline GenericRGBTColour& operator/= (double b)                   { mColour /= b;         mTransm /= b;         return *this; }

        inline GenericRGBTColour operator+ (const GenericRGBTColour& b) const { GenericRGBTColour result(*this); result += b; return result; }
        inline GenericRGBTColour operator- (const GenericRGBTColour& b) const { GenericRGBTColour result(*this); result -= b; return result; }
        inline GenericRGBTColour operator* (double b) const                   { GenericRGBTColour result(*this); result *= b; return result; }
        inline GenericRGBTColour operator/ (double b) const                   { GenericRGBTColour result(*this); result /= b; return result; }

        template<typename T> friend inline typename boost::enable_if<std::tr1::is_arithmetic<T>, GenericRGBTColour>::type operator* (T a, const GenericRGBTColour& b) { return  b * a; }

        friend inline GenericRGBTColour Sqr(const GenericRGBTColour& a) { return GenericRGBTColour(Sqr(a.mColour), Sqr(a.mTransm)); }

        friend inline Channel ColourDistanceRGBT (const GenericRGBTColour& a, const GenericRGBTColour& b)
        {
            return ColourDistance(a.mColour, b.mColour) + fabs(a.mTransm - b.mTransm);
        }

    protected:

        GenericRGBColour<Channel>   mColour;
        Channel                     mTransm;
};

typedef GenericRGBTColour<ColourChannel>        RGBTColour;         ///< Standard precision RGBxT colour.
typedef GenericRGBTColour<PreciseColourChannel> PreciseRGBTColour;  ///< High precision RGBxT colour.


/// Generic template class to hold and process CIE XYZ colours.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour components.
///
template<typename CHANNEL_T>
class GenericXYZColour : public GenericLinearColour<ColourModelXYZ, CHANNEL_T>
{
    public:

        typedef ColourModelXYZ      Model;
        typedef CHANNEL_T           Channel;
        typedef GenericLinearColour<Model, Channel> Parent;
        using Parent::kChannels;

        /// Default constructor.
        inline GenericXYZColour() : Parent() {}

        /// Copy constructor.
        inline GenericXYZColour(const GenericXYZColour& col) : Parent(col) {}

        /// Construct from sibling type with different precision.
        template<typename CHANNEL_T2>
        inline explicit GenericXYZColour(const GenericXYZColour<CHANNEL_T2>& col) : Parent(col) {}

        inline explicit GenericXYZColour(Channel x, Channel y, Channel z) :
            Parent()
        {
            mColour[Model::kX] = x;
            mColour[Model::kY] = y;
            mColour[Model::kZ] = z;
        }

        inline explicit GenericXYZColour(const GenericRGBColour<Channel>& col) : Parent(col) {}

        inline Channel  operator[](unsigned int i) const { return this->channel(i); }
        inline Channel& operator[](unsigned int i)       { return this->channel(i); }

        inline Channel  x() const { return mColour[Model::kX]; }
        inline Channel& x()       { return mColour[Model::kX]; }

        inline Channel  y() const { return mColour[Model::kY]; }
        inline Channel& y()       { return mColour[Model::kY]; }

        inline Channel  z() const { return mColour[Model::kZ]; }
        inline Channel& z()       { return mColour[Model::kZ]; }

    protected:

        using Parent::mColour;
};

typedef GenericXYZColour<ColourChannel>         XYZColour;          ///< Standard precision RGB colour.
typedef GenericXYZColour<PreciseColourChannel>  PreciseXYZColour;   ///< High precision RGB colour.


/// Generic template class to hold and process light colours.
///
/// @note   This type deliberately exposes only a subset of the functionality implemented by the base class, restricting
///         it to arithmetic operations that really make sense for light.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour channels.
///
template<typename CHANNEL_T>
class GenericLightColour : public GenericLinearColour<ColourModelInternal,CHANNEL_T>
{
    public:

        typedef ColourModelInternal Model;
        typedef CHANNEL_T           Channel;
        typedef Model::Whitepoint   Whitepoint;
        typedef GenericLinearColour<Model, Channel> Parent;
        using Parent::kChannels;

        friend class GenericAttenuatingColour<Channel>;

        friend struct IlluminantD65;
        friend struct IlluminantE;

        /// Default constructor.
        inline GenericLightColour() : Parent() {}

        /// Copy constructor.
        inline GenericLightColour(const GenericLightColour& col) : Parent(col) {}

        /// Construct from sibling type with different precision.
        template<typename CHANNEL_T2>
        inline explicit GenericLightColour(const GenericLightColour<CHANNEL_T2>& col) : Parent(col) {}

        inline explicit GenericLightColour(const GenericRGBColour<Channel>& col) : 
#if ((POV_COLOUR_MODEL == 0) || (POV_COLOUR_MODEL == 3))
            Parent(col)
        {}
#elif (POV_COLOUR_MODEL == 4)
            Parent(col)
        {
            Model::MetamericNormalize<Channel, ColourModelRGB>(mColour);
        }
#else
        #error Colour model not implemented.
#endif

        template<typename BIAS_T2, typename CHANNEL_T2>
        inline explicit GenericLightColour(const GenericCompactColour<Model,BIAS_T2,CHANNEL_T2>& col) : Parent(col) {}

        inline Channel  operator[](unsigned int i) const { return this->channel(i); }
        inline Channel& operator[](unsigned int i)       { return this->channel(i); }

        using Parent::Sum;
        using Parent::SumAbs;
        using Parent::Max;
        using Parent::MaxAbs;
        using Parent::Min;
        using Parent::MinAbs;
        using Parent::Average;
        using Parent::AverageAbs;
        using Parent::Greyscale;

        using Parent::Weight;
        using Parent::WeightAbsGreyscale;
        using Parent::WeightGreyscale;
        using Parent::WeightMax;
        using Parent::WeightMaxAbs;

        using Parent::Clear;
        using Parent::IsZero;
        using Parent::IsNearZero;
        using Parent::Invalidate;
        using Parent::IsValid;

        inline GenericLightColour                operator* (Channel b) const                                  { GenericLightColour                 result; result.SetProduct (*this, b); return result; }
        inline GenericLightColour                operator/ (Channel b) const                                  { GenericLightColour                 result; result.SetQuotient(*this, b); return result; }
        inline GenericAttenuatingColour<Channel> operator/ (const GenericLightColour& b) const                { GenericAttenuatingColour<Channel>  result; result.SetQuotient(*this, b); return result; }
        inline GenericLightColour                operator+ (const GenericLightColour& b) const                { GenericLightColour                 result; result.SetSum     (*this, b); return result; }
        inline GenericLightColour                operator* (const GenericAttenuatingColour<Channel>& b) const { GenericLightColour                 result; result.SetProduct (*this, b); return result; }

        inline GenericLightColour& operator*= (Channel b)                                  { this->SetProduct (*this, b); return *this; }
        inline GenericLightColour& operator/= (Channel b)                                  { this->SetQuotient(*this, b); return *this; }
        inline GenericLightColour& operator+= (const GenericLightColour& b)                { this->SetSum     (*this, b); return *this; }
        inline GenericLightColour& operator*= (const GenericAttenuatingColour<Channel>& b) { this->SetProduct (*this, b); return *this; }

        inline GenericLightColour& operator*= (const GenericPseudoColour<Channel>& b)      { this->SetProduct   (*this, b); return *this; }
        inline GenericLightColour& operator/= (const GenericPseudoColour<Channel>& b)      { this->SetQuotient  (*this, b); return *this; }

        template<typename T> friend inline typename boost::enable_if<std::tr1::is_arithmetic<T>, GenericLightColour>::type operator* (T a, const GenericLightColour& b) { GenericLightColour result; result.SetProduct   (b, a); return result; }

        friend inline GenericLightColour           Pow  (const GenericLightColour& a, Channel b) { GenericLightColour           result; result.SetPow    (a, b); return result; } ///< @deprecated Only allowed for the physically bogus reflection exponent feature.
        friend inline GenericPseudoColour<Channel> Sqr  (const GenericLightColour& a)            { GenericPseudoColour<Channel> result; result.SetProduct(a, a); return result; } ///< @note       Squaring a colour is allowed, but the result is not really a colour anymore.

        friend inline Channel ColourDistance (const GenericLightColour& a, const GenericLightColour& b) { GenericLightColour diff; diff.SetDifference(a, b); return diff.SumAbs(); }

    protected:

        using Parent::mColour;

        template<typename CHANNEL_T2>
        inline explicit GenericLightColour(const CHANNEL_T2* col) : Parent(col) {}
};

typedef GenericLightColour<ColourChannel>           LightColour;        ///< Standard precision light colour.
typedef GenericLightColour<PreciseColourChannel>    PreciseLightColour; ///< High precision light colour.


/// Generic template class to hold and process attenuating colours.
///
/// @note   This type deliberately exposes only a subset of the functionality implemented by the base class, restricting
///         it to arithmetic operations that really make sense for attenuating colours.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour channels.
///
template<typename CHANNEL_T>
class GenericAttenuatingColour : public GenericLinearColour<ColourModelInternal,CHANNEL_T>
{
    public:

        typedef ColourModelInternal Model;
        typedef CHANNEL_T           Channel;
        typedef Model::Whitepoint   Whitepoint;
        typedef GenericLinearColour<Model, Channel> Parent;
        using Parent::kChannels;

        friend class GenericLightColour<Channel>;

        /// Default constructor.
        inline GenericAttenuatingColour() : Parent() {}

        /// Copy constructor.
        inline GenericAttenuatingColour(const GenericAttenuatingColour& col) : Parent(col) {}

        /// Construct from sibling type with different precision.
        template<typename CHANNEL_T2>
        inline explicit GenericAttenuatingColour(const GenericAttenuatingColour<CHANNEL_T2>& col) : Parent(col) {}

        inline explicit GenericAttenuatingColour(const GenericTransColour<Channel>& col) : Parent(col.colour()) {}

        template<typename CHANNEL_T2>
        inline explicit GenericAttenuatingColour(const GenericPseudoColour<CHANNEL_T2>& col) : Parent(col) {}

        inline explicit GenericAttenuatingColour(const GenericRGBColour<Channel>& col) :
#if (POV_COLOUR_MODEL == 0)
            Parent(col)
#else
            Parent(GenericLightColour<Channel>(col) / ColourModelRGB::Whitepoint::Colour)
#endif
        {}

        inline explicit GenericAttenuatingColour(Channel att) : Parent(att) {}

        inline Channel  operator[](unsigned int i) const { return this->channel(i); }
        inline Channel& operator[](unsigned int i)       { return this->channel(i); }

        using Parent::Sum;
        using Parent::SumAbs;
        using Parent::Max;
        using Parent::MaxAbs;
        using Parent::Min;
        using Parent::MinAbs;
        using Parent::Average;
        using Parent::AverageAbs;
        using Parent::Greyscale;

        using Parent::Weight;
        using Parent::WeightAbsGreyscale;
        using Parent::WeightGreyscale;
        using Parent::WeightMax;
        using Parent::WeightMaxAbs;

        using Parent::Clear;
        using Parent::IsZero;
        using Parent::IsNearZero;
        using Parent::Invalidate;
        using Parent::IsValid;

        inline void Clip      (Channel minc, Channel maxc) { this->SetClipped     (*this, minc, maxc); }
        inline void ClipUpper (Channel maxc)               { this->SetClippedUpper(*this, maxc); }
        inline void ClipLower (Channel minc)               { this->SetClippedLower(*this, minc); }

        inline GenericAttenuatingColour Clipped      (Channel minc, Channel maxc) const { GenericAttenuatingColour result; result.SetClipped     (*this, minc, maxc); return result; }
        inline GenericAttenuatingColour ClippedUpper (Channel maxc) const               { GenericAttenuatingColour result; result.SetClippedUpper(*this, maxc);       return result; }
        inline GenericAttenuatingColour ClippedLower (Channel minc) const               { GenericAttenuatingColour result; result.SetClippedLower(*this, minc);       return result; }

        inline GenericAttenuatingColour    operator- () const                                     { GenericAttenuatingColour    result; result.SetNegative  (*this);    return result; } // TODO eliminate
        inline GenericAttenuatingColour    operator* (Channel b) const                            { GenericAttenuatingColour    result; result.SetProduct   (*this, b); return result; }
        inline GenericAttenuatingColour    operator/ (Channel b) const                            { GenericAttenuatingColour    result; result.SetQuotient  (*this, b); return result; }
        inline GenericAttenuatingColour    operator+ (const GenericAttenuatingColour& b) const    { GenericAttenuatingColour    result; result.SetSum       (*this, b); return result; }
        inline GenericAttenuatingColour    operator- (const GenericAttenuatingColour& b) const    { GenericAttenuatingColour    result; result.SetDifference(*this, b); return result; } // TODO eliminate
        inline GenericAttenuatingColour    operator* (const GenericAttenuatingColour& b) const    { GenericAttenuatingColour    result; result.SetProduct   (*this, b); return result; }
        inline GenericLightColour<Channel> operator* (const GenericLightColour<Channel>& b) const { GenericLightColour<Channel> result; result.SetProduct   (*this, b); return result; }

        inline GenericAttenuatingColour& operator*= (Channel b)                         { this->SetProduct (*this, b); return *this; }
        inline GenericAttenuatingColour& operator/= (Channel b)                         { this->SetQuotient(*this, b); return *this; }
        inline GenericAttenuatingColour& operator+= (const GenericAttenuatingColour& b) { this->SetSum     (*this, b); return *this; }
        inline GenericAttenuatingColour& operator*= (const GenericAttenuatingColour& b) { this->SetProduct (*this, b); return *this; }

        template<typename T> friend inline typename boost::enable_if<std::tr1::is_arithmetic<T>, GenericAttenuatingColour>::type operator* (T a, const GenericAttenuatingColour& b) { GenericAttenuatingColour result; result.SetProduct(b, a); return result; }

        friend inline GenericAttenuatingColour     Exp  (const GenericAttenuatingColour& a) { GenericAttenuatingColour     result; result.SetExp    (a);    return result; } ///< @deprecated Encapsulate this in a more specialized function to compute distance-based attenuation.
        friend inline GenericPseudoColour<Channel> Sqr  (const GenericAttenuatingColour& a) { GenericPseudoColour<Channel> result; result.SetProduct(a, a); return result; } ///< @note       Squaring a colour is allowed, but the result is not really a colour anymore.

        friend inline Channel ColourDistance (const GenericAttenuatingColour& a, const GenericAttenuatingColour& b) { GenericAttenuatingColour diff; diff.SetDifference(a, b); return diff.SumAbs(); }

    protected:

        using Parent::mColour;
};

typedef GenericAttenuatingColour<ColourChannel>         AttenuatingColour;          ///< Standard precision light colour.
typedef GenericAttenuatingColour<PreciseColourChannel>  PreciseAttenuatingColour;   ///< High precision light colour.


/// Generic template class to hold and process generic wavelength-dependent colour-related data.
///
/// @note   This type is _not_ intended to represent actual colours; use @ref GenericLightColour or
///         @ref GenericAttenuatingColour for that purpose instead.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour channels.
///
template<typename CHANNEL_T>
class GenericPseudoColour : public GenericLinearColour<ColourModelInternal,CHANNEL_T>
{
    public:

        typedef ColourModelInternal Model;
        typedef CHANNEL_T           Channel;
        typedef GenericLinearColour<Model, Channel> Parent;
        using Parent::kChannels;

        static const GenericPseudoColour DominantWavelengths;

        /// Default constructor.
        inline GenericPseudoColour() : Parent() {}

        /// Copy constructor.
        inline GenericPseudoColour(const GenericPseudoColour& col) : Parent(col) {}

        /// Construct from sibling type with different precision.
        template<typename CHANNEL_T2>
        inline explicit GenericPseudoColour(const GenericPseudoColour<CHANNEL_T2>& col) : Parent(col) {}

        inline explicit GenericPseudoColour(const GenericLightColour<Channel>&   col) : Parent(col) {}
        inline explicit GenericPseudoColour(const GenericAttenuatingColour<Channel>& col) : Parent(col) {}

        inline explicit GenericPseudoColour(const GenericRGBColour<Channel>& col) :
#if (POV_COLOUR_MODEL == 0)
            Parent(col)
        {}
#else
            Parent()
        {
            // Perform cubic interpolation, based on Compute f(x) = a x^2 + b x + c with:
            //      a =   [ x0   (y1-y2) + x1   (y2-y0) + x2   (y0-y1) ] / [ (x1-x2) (x2-x0) (x0-x1) ]
            //      b = - [ x0^2 (y1-y2) + x1^2 (y2-y0) + x2^2 (y0-y1) ] / [ (x1-x2) (x2-x0) (x0-x1) ]
            //      c =   [ (x1-x2) (x2-x0) (x0-x1) (y0 + y1 + y2) + (x0 + x1 + x2) [ x0^2 (y1-y2) + x1^2 (y2-y0) + x2^2 (y0-y1) ] - (x0^2 + x1^2 + x2^2) [ x0 (y1-y2) + x1 (y2-y0) + x2 (y0-y1) ] ] / 3;

            const ColourChannel* x = ColourModelRGB::GetDominantWavelengths();
            const GenericRGBColour<Channel>& y = col;
            double xSqr;            // x[n]^2
            double xDiff;           // x[n+1] - x[n+2]
            double yDiff;           // y[n+1] - y[n+2]
            double xProd  = 1.0;    // (x1-x2) (x2-x0) (x0-x1)
            double xySum  = 0;      // x0   (y1-y2) + x1   (y2-y0) + x2   (y0-y1)
            double x2ySum = 0;      // x0^2 (y1-y2) + x1^2 (y2-y0) + x2^2 (y0-y1)
            double xSum   = 0;      // x0   + x1   + x2
            double x2Sum  = 0;      // x0^2 + x1^2 + x2^2
            double ySum   = 0;      // y0   + y1   + y2
            for (unsigned int i = 0; i < ColourModelRGB::kChannels; i ++)
            {
                xDiff  =  x[(i+1)%3] - x[(i+2)%3];
                yDiff  =  y[(i+1)%3] - y[(i+2)%3];
                xSqr   =  Sqr(x[i]);
                xProd  *= xDiff;
                xySum  += x[i] * yDiff;
                x2ySum += xSqr * yDiff;
                ySum   += y[i];
                xSum   += x[i];
                x2Sum  += x[i];
            }
            double a = xySum  / xProd;
            double b = x2ySum / xProd;
            double c = (xProd * ySum + xSum * x2ySum - x2Sum * xySum) / 3;

            x = Model::GetDominantWavelengths();
            for (unsigned int i = 0; i < Model::kChannels; i ++)
                mColour[i] = a * Sqr(x[i]) + b * x[i] + c;
        }
#endif

        inline explicit GenericPseudoColour(Channel grey) : Parent(grey) {}

        inline Channel  operator[](unsigned int i) const { return this->channel(i); }
        inline Channel& operator[](unsigned int i)       { return this->channel(i); }

        using Parent::Sum;
        using Parent::SumAbs;
        using Parent::Max;
        using Parent::MaxAbs;
        using Parent::Min;
        using Parent::MinAbs;
        using Parent::Average;
        using Parent::AverageAbs;
        using Parent::Greyscale;

        using Parent::Weight;
        using Parent::WeightAbsGreyscale;
        using Parent::WeightGreyscale;
        using Parent::WeightMax;
        using Parent::WeightMaxAbs;

        using Parent::Clear;
        using Parent::IsZero;
        using Parent::IsNearZero;
        using Parent::Invalidate;
        using Parent::IsValid;

        inline void Clip      (Channel minc, Channel maxc) { this->SetClipped     (*this, minc, maxc); }
        inline void ClipUpper (Channel maxc)               { this->SetClippedUpper(*this, maxc); }
        inline void ClipLower (Channel minc)               { this->SetClippedLower(*this, minc); }

        inline GenericPseudoColour Clipped      (Channel minc, Channel maxc) const { GenericPseudoColour result; result.SetClipped     (*this, minc, maxc); return result; }
        inline GenericPseudoColour ClippedUpper (Channel maxc) const               { GenericPseudoColour result; result.SetClippedUpper(*this, maxc);       return result; }
        inline GenericPseudoColour ClippedLower (Channel minc) const               { GenericPseudoColour result; result.SetClippedLower(*this, minc);       return result; }

        inline GenericPseudoColour operator- () const                             { GenericPseudoColour result; result.SetNegative  (*this);    return result; }
        inline GenericPseudoColour operator+ (Channel b) const                    { GenericPseudoColour result; result.SetSum       (*this, b); return result; }
        inline GenericPseudoColour operator- (Channel b) const                    { GenericPseudoColour result; result.SetDifference(*this, b); return result; }
        inline GenericPseudoColour operator* (Channel b) const                    { GenericPseudoColour result; result.SetProduct   (*this, b); return result; }
        inline GenericPseudoColour operator/ (Channel b) const                    { GenericPseudoColour result; result.SetQuotient  (*this, b); return result; }
        inline GenericPseudoColour operator+ (const GenericPseudoColour& b) const { GenericPseudoColour result; result.SetSum       (*this, b); return result; }
        inline GenericPseudoColour operator- (const GenericPseudoColour& b) const { GenericPseudoColour result; result.SetDifference(*this, b); return result; }
        inline GenericPseudoColour operator* (const GenericPseudoColour& b) const { GenericPseudoColour result; result.SetProduct   (*this, b); return result; }
        inline GenericPseudoColour operator/ (const GenericPseudoColour& b) const { GenericPseudoColour result; result.SetQuotient  (*this, b); return result; }

        inline GenericPseudoColour& operator+= (Channel b)                    { this->SetSum       (*this, b); return *this; }
        inline GenericPseudoColour& operator-= (Channel b)                    { this->SetDifference(*this, b); return *this; }
        inline GenericPseudoColour& operator*= (Channel b)                    { this->SetProduct   (*this, b); return *this; }
        inline GenericPseudoColour& operator/= (Channel b)                    { this->SetQuotient  (*this, b); return *this; }
        inline GenericPseudoColour& operator=  (const GenericPseudoColour& b) { this->SetEqual     (b);        return *this; }
        inline GenericPseudoColour& operator+= (const GenericPseudoColour& b) { this->SetSum       (*this, b); return *this; }
        inline GenericPseudoColour& operator-= (const GenericPseudoColour& b) { this->SetDifference(*this, b); return *this; }
        inline GenericPseudoColour& operator*= (const GenericPseudoColour& b) { this->SetProduct   (*this, b); return *this; }
        inline GenericPseudoColour& operator/= (const GenericPseudoColour& b) { this->SetQuotient  (*this, b); return *this; }

        template<typename T> friend inline typename boost::enable_if<std::tr1::is_arithmetic<T>, GenericPseudoColour>::type operator+ (T a, const GenericPseudoColour& b) { GenericPseudoColour result; result.SetSum       (b, a); return result; }
        template<typename T> friend inline typename boost::enable_if<std::tr1::is_arithmetic<T>, GenericPseudoColour>::type operator- (T a, const GenericPseudoColour& b) { GenericPseudoColour result; result.SetDifference(a, b); return result; }
        template<typename T> friend inline typename boost::enable_if<std::tr1::is_arithmetic<T>, GenericPseudoColour>::type operator* (T a, const GenericPseudoColour& b) { GenericPseudoColour result; result.SetProduct   (b, a); return result; }
        template<typename T> friend inline typename boost::enable_if<std::tr1::is_arithmetic<T>, GenericPseudoColour>::type operator/ (T a, const GenericPseudoColour& b) { GenericPseudoColour result; result.SetQuotient  (a, b); return result; }

        friend inline GenericPseudoColour Cos  (const GenericPseudoColour& a)            { GenericPseudoColour result; result.SetCos    (a);    return result; }
        friend inline GenericPseudoColour Exp  (const GenericPseudoColour& a)            { GenericPseudoColour result; result.SetExp    (a);    return result; }
        friend inline GenericPseudoColour Pow  (const GenericPseudoColour& a, Channel b) { GenericPseudoColour result; result.SetPow    (a, b); return result; }
        friend inline GenericPseudoColour Sqr  (const GenericPseudoColour& a)            { GenericPseudoColour result; result.SetProduct(a, a); return result; }
        friend inline GenericPseudoColour Sqrt (const GenericPseudoColour& a)            { GenericPseudoColour result; result.SetSqrt   (a);    return result; }

        friend inline GenericPseudoColour Sqr  (const GenericLightColour<Channel>& a);
        friend inline GenericPseudoColour Sqr  (const GenericAttenuatingColour<Channel>& a);

        friend inline Channel ColourDistance (const GenericPseudoColour& a, const GenericPseudoColour& b) { return (a-b).SumAbs(); }

    protected:

        using Parent::mColour;

        template<typename CHANNEL_T2>
        inline explicit GenericPseudoColour(const CHANNEL_T2* col) : Parent(col) {}
};

typedef GenericPseudoColour<ColourChannel>          PseudoColour;           ///< Standard precision generic per-colour-channel data.
typedef GenericPseudoColour<PreciseColourChannel>   PrecisePseudoColour;    ///< High precision generic per-colour-channel data.


/// Generic template class to hold and process colours with associated transparency information.
///
/// @note   The current implementation uses filter/transmit format for the transparency information; future
///         implementations may vary.
///
/// @tparam CHANNEL_T   Floating-point type to use for the individual colour channels.
///
template<typename CHANNEL_T>
class GenericTransColour
{
    public:

        typedef CHANNEL_T                           Channel;
        typedef GenericAttenuatingColour<Channel>   Colour;
        typedef GenericFilterTransm<Channel>        TransData;

        typedef DBL EXPRESS[5];

        /// Default constructor.
        inline GenericTransColour() :
            mColour(),
            mTrans()
        {}

        /// Copy constructor.
        inline GenericTransColour(const GenericTransColour& col) :
            mColour(col.mColour),
            mTrans(col.mTrans)
        {}

        /// Construct from sibling type with different precision.
        template<typename CHANNEL_T2>
        inline explicit GenericTransColour(const GenericTransColour<CHANNEL_T2>& col) :
            mColour(col.colour()),
            mTrans(col.trans())
        {}

        inline explicit GenericTransColour(const Colour& col, const TransData& trans) :
            mColour(col),
            mTrans(trans)
        {}

        inline explicit GenericTransColour(const GenericRGBFTColour<Channel>& col) :
            mColour(col.rgb()),
            mTrans(col.trans())
        {}

        inline explicit GenericTransColour(const Colour& col, Channel filter, Channel transm) :
            mColour(col),
            mFilter(filter),
            mTransm(transm)
        {}

        inline Colour     colour() const { return mColour; }
        inline Colour&    colour()       { return mColour; }

        inline TransData  trans() const  { return mTrans; }
        inline TransData& trans()        { return mTrans; }

        inline Channel Opacity() const       { return mTrans.Opacity(*this); }
        inline Channel LegacyOpacity() const { return mTrans.LegacyOpacity(mColour); }

        inline void Clear()         { mColour.Clear(); mTrans.Clear(); }
        inline void Invalidate()    { mColour.Invalidate(); }
        inline bool IsValid() const { return mColour.IsValid(); }

        inline Colour TransmittedColour() const { return mTrans.TransmittedColour(mColour); }

        inline GenericTransColour& operator=  (const GenericTransColour& b) { mColour =  b.mColour; mTrans =  b.mTrans; return *this; }
        inline GenericTransColour& operator+= (const GenericTransColour& b) { mColour += b.mColour; mTrans += b.mTrans; return *this; }
        inline GenericTransColour& operator-= (const GenericTransColour& b) { mColour -= b.mColour; mTrans -= b.mTrans; return *this; }
        inline GenericTransColour& operator*= (Channel b)                   { mColour *= b;         mTrans *= b;        return *this; }
        inline GenericTransColour& operator/= (Channel b)                   { mColour /= b;         mTrans /= b;        return *this; }

        inline GenericTransColour operator+ (const GenericTransColour& b) const { GenericTransColour result(*this); result += b; return result; }
        inline GenericTransColour operator- (const GenericTransColour& b) const { GenericTransColour result(*this); result -= b; return result; }
        inline GenericTransColour operator* (Channel b) const                   { GenericTransColour result(*this); result *= b; return result; }
        inline GenericTransColour operator/ (Channel b) const                   { GenericTransColour result(*this); result /= b; return result; }

        template<typename T> friend inline typename boost::enable_if<std::tr1::is_arithmetic<T>, GenericTransColour>::type operator* (T a, const GenericTransColour& b) { return b * a; }

    protected:

        Colour      mColour;
        TransData   mTrans;
};

typedef GenericTransColour<ColourChannel>           TransColour;        ///< Standard precision transparent colour.
typedef GenericTransColour<PreciseColourChannel>    PreciseTransColour; ///< High precision transparent colour.


/// Generic template class to store a colour in a compact format.
///
/// This class uses RGBE-style format for compact storage of high dynamic range colours, as originally
/// proposed by Greg Ward.
///
/// @author Christoph Lipka
/// @author Based on MegaPOV HDR code written by Mael and Christoph Hormann
///
/// @tparam MODEL_T     Tagging type identifying the colour model to use.
/// @tparam BIAS_T      Tagging type identifying the bias to use for the exponent.
///                     Should be a specialization of @ref CompactColourBias.
/// @tparam CHANNEL_T   Type to use for the colour components.
///                     Defaults to unsigned char.
///
/// @remark While it would at first seem possible and easier to parameterize this template by the bias value itself,
///         there are some subtle pitfalls associated with using integer template parameters that would make life more
///         difficult in other places, so we're using a type instead to identify the bias to use.
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

        typedef Channel Data[kCoefficients+1];

        /// Default constructor.
        inline GenericCompactColour()
        {
            for (unsigned int i = 0; i < kChannels; i ++)
                mData[i] = 0;
            mData[kExp] = std::numeric_limits<Channel>::min();
        }

        /// Copy constructor.
        inline GenericCompactColour(const GenericCompactColour& col)
        {
            for (unsigned int i = 0; i < kCoefficients; i ++)
                mData[i] = col.mData[i];
        }

        /// Construct from standard-precision colour.
        inline explicit GenericCompactColour(const GenericLinearColour<Model,ColourChannel>& col, ColourChannel dither = 0.0)
        {
            double scaleFactor;
            if (ComputeExponent(col, mData[kExp], scaleFactor))
            {
                for (unsigned int i = 0; i < kChannels; i ++)
                    mData[i] = clipToType<Channel>(floor(col.channel(i) * scaleFactor + 0.5 + dither));
            }
            else
            {
                for (unsigned int i = 0; i < kChannels; i ++)
                    mData[i] = 0;
            }
        }

        /// Construct from standard-precision colour.
        inline explicit GenericCompactColour(const GenericLinearColour<Model,ColourChannel>& col, const GenericLinearColour<Model,ColourChannel>& dither)
        {
            double scaleFactor;
            if (ComputeExponent(col, mData[kExp], scaleFactor))
            {
                for (unsigned int i = 0; i < kChannels; i ++)
                    mData[i] = clipToType<Channel>(floor(col.channel(i) * scaleFactor + 0.5 + dither.channel(i)));
            }
            else
            {
                for (unsigned int i = 0; i < kChannels; i ++)
                    mData[i] = 0;
            }
        }

        inline const Data& operator*() const { return mData; } ///< Get direct data access for reading.
        inline       Data& operator*()       { return mData; } ///< Get direct data access for reading & writing.

        inline Channel  operator[](unsigned int i) const { return mData[i]; } ///< Get direct single-channel access for reading.
        inline Channel& operator[](unsigned int i)       { return mData[i]; } ///< Get direct single-channel access for reading & writing.

    protected:

        Data mData;

        inline static bool ComputeExponent(const GenericLinearColour<Model,ColourChannel>& col, Channel& biasedExponent, double& scaleFactor)
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
