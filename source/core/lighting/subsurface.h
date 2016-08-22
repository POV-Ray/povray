//******************************************************************************
///
/// @file core/lighting/subsurface.h
///
/// Declarations related to subsurface light transport.
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

#ifndef POVRAY_CORE_SUBSURFACE_H
#define POVRAY_CORE_SUBSURFACE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include <boost/flyweight.hpp>
#include <boost/flyweight/key_value.hpp>

#include "core/coretypes.h"

namespace pov
{

using boost::flyweights::flyweight;
using boost::flyweights::key_value;

/// Class storing SSLT data precomputed based on index of refraction.
class SubsurfaceInterior {

    public:

        SubsurfaceInterior(double ior);
        PrecisePseudoColour GetReducedAlbedo(const AttenuatingColour& diffuseReflectance) const;

    protected:

        static const int ReducedAlbedoSamples = 100;

        // precomputed reduced albedo for selected values of diffuse reflectance
        struct PrecomputedReducedAlbedo {
            float reducedAlbedo[ReducedAlbedoSamples+1];
            PrecomputedReducedAlbedo(ColourChannel ior);
            PreciseColourChannel operator()(PreciseColourChannel diffuseReflectance) const;
        };

        flyweight<key_value<float,PrecomputedReducedAlbedo> > precomputedReducedAlbedo;
};

/// Approximation to the Fresnel diffuse reflectance.
inline double FresnelDiffuseReflectance(double eta)
{
#if 0
    // This is the original formula as per the 2001 Jensen et al. paper;
    // however, this breaks down for large values of eta, or values < 1.0,
    // and comes with some other bogosities.
    return clip( -1.440/Sqr(eta) + 0.710/eta + 0.668 + 0.0636*eta, 0.0, 1.0-EPSILON );
#else
    // My own approximation; maybe it's utterly wrong, but at least it is stable.
    if (eta < 1.0)
        return Sqr(eta)-pow(eta,2.25);
    else
        return ( (1.0-1.0/eta) + 3*pow(1.0-1.0/eta,4.5) ) / 4.0;
#endif
}

} // end of namespace

#endif // POVRAY_CORE_SUBSURFACE_H
