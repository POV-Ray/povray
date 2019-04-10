//******************************************************************************
///
/// @file core/lighting/subsurface.cpp
///
/// Implementations related to subsurface light transport.
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
#include "core/lighting/subsurface.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/mathutil.h"

// POV-Ray header files (core module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/// Computes the BRDF approximation of the diffuse reflectance term.
inline double DiffuseReflectance(double A, double alphaPrime)
{
    double root = sqrt(3*(1-alphaPrime));
    return (alphaPrime/2) * ( 1 + exp(-(4.0/3.0)*A*root) ) * exp(-root);
}


SubsurfaceInterior::SubsurfaceInterior(double ior) :
    precomputedReducedAlbedo(ior)
{}

SubsurfaceInterior::PrecomputedReducedAlbedo::PrecomputedReducedAlbedo(float ior)
{
    reducedAlbedo[ReducedAlbedoSamples] = 1.0;
    double Fdr = FresnelDiffuseReflectance(ior);
    double A = (1+Fdr)/(1-Fdr);
    double alphaPrime = 1.0;
    double Rd = 1.0;
    int it = 0;
    for (int i = ReducedAlbedoSamples-1; i > 0; i --)
    {
        double Rd0 = 0.0;
        double Rd1 = Rd;
        double diffuseReflectance = double(i)/ReducedAlbedoSamples;
        double alphaPrime0 = 0.0;
        double alphaPrime1 = alphaPrime;
        while(abs(Rd-diffuseReflectance) >= EPSILON)
        {
            double p = (diffuseReflectance-Rd0)/(Rd1-Rd0);
            alphaPrime = alphaPrime0 + p*(alphaPrime1-alphaPrime0);
            Rd = DiffuseReflectance(A, alphaPrime);
            if (Rd < diffuseReflectance)
            {
                alphaPrime0 = alphaPrime;
                Rd0 = Rd;
            }
            else
            {
                alphaPrime1 = alphaPrime;
                Rd1 = Rd;
            }
            it ++;
        }
        reducedAlbedo[i] = alphaPrime;
    }
    reducedAlbedo[0] = 0.0;
}

PreciseColourChannel SubsurfaceInterior::PrecomputedReducedAlbedo::operator()(PreciseColourChannel diffuseReflectance) const
{
    PreciseColourChannel Rd = clip(diffuseReflectance, 0.0, 1.0);
    PreciseColourChannel i = diffuseReflectance * ReducedAlbedoSamples;
    int i0 = floor(i);
    int i1 = ceil(i);
    PreciseColourChannel p = (i-i0);
    return (1-p)*reducedAlbedo[i0] + p*reducedAlbedo[i1];
}

PreciseMathColour SubsurfaceInterior::GetReducedAlbedo(const MathColour& diffuseReflectance) const
{
    PreciseMathColour result;
    for (int i = 0; i < MathColour::channels; i ++)
        result[i] = (precomputedReducedAlbedo.get())(diffuseReflectance[i]);
    return result;
}

}
// end of namespace pov
