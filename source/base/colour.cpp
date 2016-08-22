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
