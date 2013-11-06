/*******************************************************************************
 * subsurface.h
 *
 * This module contains all defines, typedefs, and prototypes for subsurface.cpp.
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
 * $File: //depot/public/povray/3.x/source/backend/lighting/subsurface.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef SUBSURFACE_H
#define SUBSURFACE_H

#include <boost/version.hpp>
#if BOOST_VERSION >= 103800 // flyweight is unavailable prior to boost 1.38
#define HAVE_BOOST_FLYWEIGHT 1
#else
#define HAVE_BOOST_FLYWEIGHT 0
#endif

#if HAVE_BOOST_FLYWEIGHT
#include <boost/flyweight.hpp>
#include <boost/flyweight/key_value.hpp>
#endif // HAVE_BOOST_FLYWEIGHT

namespace pov
{

//using namespace pov_base;
#if HAVE_BOOST_FLYWEIGHT
using boost::flyweights::flyweight;
using boost::flyweights::key_value;
#endif // HAVE_BOOST_FLYWEIGHT

/// Class storing SSLT data precomputed based on index of refraction.
class SubsurfaceInterior {

	public:

		SubsurfaceInterior(double ior);
		DblRGBColour GetReducedAlbedo(const RGBColour& diffuseReflectance) const;

	protected:

		static const int ReducedAlbedoSamples = 100;

		// precomputed reduced albedo for selected values of diffuse reflectance
		struct PrecomputedReducedAlbedo {
			float reducedAlbedo[ReducedAlbedoSamples+1];
			PrecomputedReducedAlbedo(double ior);
			double operator()(double diffuseReflectance) const;
		};

#if HAVE_BOOST_FLYWEIGHT
		flyweight<key_value<double,PrecomputedReducedAlbedo> > precomputedReducedAlbedo;
#else // HAVE_BOOST_FLYWEIGHT
		// TODO - if flyweight is unavailable, we're currently resorting to a simple but memory-hungry fallback solution
		PrecomputedReducedAlbedo precomputedReducedAlbedo;
#endif // HAVE_BOOST_FLYWEIGHT
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

#endif // SUBSURFACE_H
