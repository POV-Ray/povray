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

// configbase.h must always be the first POV file included within base *.cpp files
#include "base/configbase.h"
#include "base/colour.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

#if (NUM_COLOUR_CHANNELS == 3)

    // Approximate dominant wavelengths of primary hues in micrometers.
    // Source: 3D Computer Graphics by John Vince (Addison Wesely)
    // These are user-adjustable with the irid_wavelength keyword.
    // Red = 700 nm, Green = 520 nm, Blue = 480 nm
    static const ColourChannel gaDefaultWavelengths[NUM_COLOUR_CHANNELS] = { 0.700, 0.520, 0.480 };

#elif (NUM_COLOUR_CHANNELS == 4)

    // Approximate dominant wavelengths of primary hues in micrometers.
    static const ColourChannel gaDefaultWavelengths[NUM_COLOUR_CHANNELS] = { 0.6150, 0.5725, 0.5200, 0.4650 };

    // Relative brightness of the primary hues.
    template<> const ColourChannel MathColour::mkY[NUM_COLOUR_CHANNELS] =
        { 0.2053179932, 0.3806513958, 0.3705274008, 0.0435032102 };

    // Location of the primary hues in RGB space.
    template<> const RGBColour MathColour::mkRGB[NUM_COLOUR_CHANNELS] =
        { RGBColour( 0.0084075942,-0.0404444319, 0.9783939079),
          RGBColour(-0.3789518761, 0.6301106785, 0.0062451983),
          RGBColour( 0.4848742224, 0.3939794183, 0.0580130026),
          RGBColour( 1.0905645527,-0.0353094890,-0.0175721295) };

#else

    #error TODO!

#endif

    template<> const MathColour        MathColour::mkDefaultWavelengths        = MathColour(gaDefaultWavelengths);
    template<> const PreciseMathColour PreciseMathColour::mkDefaultWavelengths = PreciseMathColour(gaDefaultWavelengths);

}
