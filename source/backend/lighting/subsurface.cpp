/*******************************************************************************
 * subsurface.cpp
 *
 * This module implements subsurface light transport.
 *
 * Author: Christoph Lipka
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/lighting/subsurface.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"

#include "backend/math/vector.h"
#include "backend/lighting/subsurface.h"

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

SubsurfaceInterior::PrecomputedReducedAlbedo::PrecomputedReducedAlbedo(double ior)
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
		while(fabs(Rd-diffuseReflectance) >= EPSILON)
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

double SubsurfaceInterior::PrecomputedReducedAlbedo::operator()(double diffuseReflectance) const
{
	double Rd = clip(diffuseReflectance, 0.0, 1.0);
	double i = diffuseReflectance * ReducedAlbedoSamples;
	int i0 = floor(i);
	int i1 = ceil(i);
	double p = (i-i0);
	return (1-p)*reducedAlbedo[i0] + p*reducedAlbedo[i1];
}

DblRGBColour SubsurfaceInterior::GetReducedAlbedo(const RGBColour& diffuseReflectance) const
{
	return DblRGBColour(((const PrecomputedReducedAlbedo&)precomputedReducedAlbedo)(diffuseReflectance.red()),
	                    ((const PrecomputedReducedAlbedo&)precomputedReducedAlbedo)(diffuseReflectance.green()),
	                    ((const PrecomputedReducedAlbedo&)precomputedReducedAlbedo)(diffuseReflectance.blue()));
}

} // end of namespace
