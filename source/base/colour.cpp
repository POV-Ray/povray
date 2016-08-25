//******************************************************************************
///
/// @file base/colour.cpp
///
/// Implementations related to colour storage and computations.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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
#include "base/colour.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

const LightColour IlluminantD65::Colour (ColourModelInternal::GetWhitepointVector<IlluminantD65>());
const LightColour IlluminantE  ::Colour (ColourModelInternal::GetWhitepointVector<IlluminantE>());

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


template<> const PseudoColour        PseudoColour       ::DominantWavelengths (ColourModelInternal::GetDominantWavelengths());
template<> const PrecisePseudoColour PrecisePseudoColour::DominantWavelengths (ColourModelInternal::GetDominantWavelengths());

template<typename CHANNEL_T, typename MODEL_FROM_T> void ColourModelInternal4::MetamericNormalize(CHANNEL_T* col)
{
    const ColourChannel* pWhitepoint = GetWhitepointVector<typename MODEL_FROM_T::Whitepoint>();
    const ColourChannel *pNeutral    = GetMetamericNeutral();

    CHANNEL_T minAdd, minSub, maxAdd, maxSub, adjust;
    /*
    // if we're in the red to green domain, boost yellow
    if ((col[0] < col[1]) && (col[0] < col[2]) && (col[0] < col[3]))
    {
        CHANNEL_T upperDiff = pWhitepoint[2] - col[2];
        if (upperDiff > 0)
        {
            adjust = upperDiff / pNeutral[2];
            for (unsigned int i = 0; i < kChannels; i ++)
                col[i] += adjust * pNeutral[i];
        }
    }
    */
    // force all channels below upper limit (if possible)
    minAdd =  0.0;
    minSub =  0.0;
    maxAdd =  std::numeric_limits<CHANNEL_T>::max();
    maxSub = -std::numeric_limits<CHANNEL_T>::max();
    for (unsigned int i = 0; i < kChannels; i ++)
    {
        CHANNEL_T upperDiff = pWhitepoint[i] - col[i];
        if (upperDiff < 0)
        {
            // channel needs to be adjusted downwards
            CHANNEL_T adjust = upperDiff / pNeutral[i];
            minAdd = max(minAdd, adjust);
            minSub = min(minSub, adjust);
        }
        else
        {
            // no need to adjust channel, make sure to not over-adjust it for other channels
            CHANNEL_T headroom = upperDiff / pNeutral[i];
            if (headroom >= 0)
                maxAdd = min(maxAdd, headroom);
            if (headroom <= 0)
                maxSub = max(maxSub, headroom);
        }
    }
    if ((minAdd != 0.0) && (minSub == 0.0))
        // we can compensate by adding neutral
        adjust = min(minAdd, maxAdd);
    else if ((minAdd == 0.0) && (minSub != 0.0))
        // we can compensate by adding neutral
        adjust = max(minSub, maxSub);
    else
        // we cannot compensate
        adjust = 0.0;
    if (adjust != 0.0)
        for (unsigned int i = 0; i < kChannels; i ++)
            col[i] += adjust * pNeutral[i];

    // force all channels above lower limit (if possible)
    minAdd =  0.0;
    minSub =  0.0;
    maxAdd =  std::numeric_limits<CHANNEL_T>::max();
    maxSub = -std::numeric_limits<CHANNEL_T>::max();
    for (unsigned int i = 0; i < kChannels; i ++)
    {
        CHANNEL_T lowerDiff = -col[i];
        if (lowerDiff > 0)
        {
            // channel needs to be adjusted upwards
            CHANNEL_T adjust = lowerDiff / pNeutral[i];
            minAdd = max(minAdd, adjust);
            minSub = min(minSub, adjust);
        }
        else
        {
            // no need to adjust channel, make sure to not over-adjust it for other channels
            CHANNEL_T headroom = lowerDiff / pNeutral[i];
            if (headroom >= 0)
                maxAdd = min(maxAdd, headroom);
            if (headroom <= 0)
                maxSub = max(maxSub, headroom);
        }
    }
    if ((minAdd != 0.0) && (minSub == 0.0))
        // we can compensate by adding neutral
        adjust = min(minAdd, maxAdd);
    else if ((minAdd == 0.0) && (minSub != 0.0))
        // we can compensate by adding neutral
        adjust = max(minSub, maxSub);
    else
        // we cannot compensate
        adjust = 0.0;
    if (adjust != 0.0)
        for (unsigned int i = 0; i < kChannels; i ++)
            col[i] += adjust * pNeutral[i];
}

template void ColourModelInternal4::MetamericNormalize<ColourChannel,        ColourModelRGB>(ColourChannel*        col);
template void ColourModelInternal4::MetamericNormalize<PreciseColourChannel, ColourModelRGB>(PreciseColourChannel* col);

}
