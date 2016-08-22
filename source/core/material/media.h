//******************************************************************************
///
/// @file core/material/media.h
///
/// Declarations related to participating media.
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

#ifndef POVRAY_CORE_MEDIA_H
#define POVRAY_CORE_MEDIA_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/render/trace.h"

namespace pov
{

// Scattering types.
enum
{
    ISOTROPIC_SCATTERING            = 1,
    MIE_HAZY_SCATTERING             = 2,
    MIE_MURKY_SCATTERING            = 3,
    RAYLEIGH_SCATTERING             = 4,
    HENYEY_GREENSTEIN_SCATTERING    = 5,
    SCATTERING_TYPES                = 5
};

void Transform_Density(vector<PIGMENT*>& Density, const TRANSFORM *Trans);

class MediaFunction : public Trace::MediaFunctor
{
    public:

        MediaFunction(TraceThreadData *td, Trace *t, PhotonGatherer *pg);

        virtual void ComputeMedia(LightColour& colour, ColourChannel& transm, vector<Media>& mediasource, const Ray& ray, Intersection& isect);
        virtual void ComputeMedia(LightColour& colour, ColourChannel& transm, const RayInteriorVector& mediasource, const Ray& ray, Intersection& isect);
        virtual void ComputeMedia(LightColour& colour, ColourChannel& transm, MediaVector& medias, const Ray& ray, Intersection& isect);

    protected:

        /// pseudo-random number sequence
        RandomDoubleSequence randomNumbers;
        /// pseudo-random number generator based on random number sequence
        RandomDoubleSequence::Generator randomNumberGenerator;
        /// thread data
        TraceThreadData *threadData;
        /// tracing functions
        Trace *trace;
        /// photon gather functions
        PhotonGatherer *photonGatherer;

        void ComputeMediaRegularSampling(MediaVector& medias, LightSourceEntryVector& lights, MediaIntervalVector& mediaintervals,
                                         const Ray& ray, const Media *IMedia, int minsamples, bool ignore_photons, bool use_scattering,
                                         bool all_constant_and_light_ray);
        void ComputeMediaAdaptiveSampling(MediaVector& medias, LightSourceEntryVector& lights, MediaIntervalVector& mediaintervals,
                                          const Ray& ray, const Media *IMedia, DBL aa_threshold, int minsamples, bool ignore_photons, bool use_scattering);
        void ComputeMediaColour(MediaIntervalVector& mediaintervals, LightColour& colour, ColourChannel& transm);
        void ComputeMediaSampleInterval(LitIntervalVector& litintervals, MediaIntervalVector& mediaintervals, const Media *media);
        void ComputeMediaLightInterval(LightSourceEntryVector& lights, LitIntervalVector& litintervals, const Ray& ray, const Intersection& isect);
        void ComputeOneMediaLightInterval(LightSource *light, LightSourceEntryVector&lights, const Ray& ray, const Intersection& isect);
        bool ComputeSpotLightInterval(const Ray &ray, const LightSource *Light, DBL *d1, DBL *d2);
        bool ComputeCylinderLightInterval(const Ray &ray, const LightSource *Light, DBL *d1, DBL *d2);
        void ComputeOneMediaSample(MediaVector& medias, LightSourceEntryVector& lights, MediaInterval& mediainterval, const Ray &ray, DBL d0, LightColour& SampCol,
                                   AttenuatingColour& SampOptDepth, int sample_method, bool ignore_photons, bool use_scattering, bool photonPass);
        void ComputeOneMediaSampleRecursive(MediaVector& medias, LightSourceEntryVector& lights, MediaInterval& mediainterval, const Ray& ray,
                                            DBL d1, DBL d3, LightColour& Result, const LightColour& C1, const LightColour& C3,
                                            AttenuatingColour& ODResult, const AttenuatingColour& od1, const AttenuatingColour& od3,
                                            int depth, DBL Jitter, DBL aa_threshold, bool ignore_photons, bool use_scattering, bool photonPass);
        void ComputeMediaPhotons(MediaVector& medias, LightColour& Te, const AttenuatingColour& Sc, const BasicRay& ray, const Vector3d& H);
        void ComputeMediaScatteringAttenuation(MediaVector& medias, LightColour& OutputColor, const AttenuatingColour& Sc, const LightColour& Light_Colour, const BasicRay &ray, const BasicRay &Light_Ray);
};

}

#endif // POVRAY_CORE_MEDIA_H
