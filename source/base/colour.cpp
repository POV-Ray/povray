//******************************************************************************
///
/// @file base/colour.cpp
///
/// Implementations related to colour storage and computations.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/colour.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

#if (NUM_COLOUR_CHANNELS == 3)

    // Approximate dominant wavelengths of primary hues.
    // Source: 3D Computer Graphics by John Vince (Addison Wesley)
    // These are user-adjustable with the irid_wavelength keyword.
    // Red = 700 nm  Grn = 520 nm Blu = 480 nm
    // Divided by 1000 gives: rwl = 0.70;  gwl = 0.52;  bwl = 0.48;
    template<typename T>
    const GenericColour<T> GenericColour<T>::mkDefaultWavelengths = GenericColour<T>(GenericRGBColour<T>(0.70, 0.52, 0.48));

#else

    #error "TODO!"

#endif

template class GenericColour<ColourChannel>;
template class GenericColour<PreciseColourChannel>;

}
// end of namespace pov_base
