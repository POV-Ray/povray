//******************************************************************************
///
/// @file core/material/media.cpp
///
/// Implementations related to participating media.
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
#include "core/material/media.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/lighting/lightsource.h"
#include "core/lighting/photons.h"
#include "core/material/pattern.h"
#include "core/material/pigment.h"
#include "core/math/chi2.h"
#include "core/render/ray.h"
#include "core/scene/object.h"
#include "core/scene/tracethreaddata.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::min;
using std::max;
using std::vector;

Media::Media()
{
    Type = ISOTROPIC_SCATTERING;

    Intervals      = 10;
    Min_Samples    = 1;
    Max_Samples    = 1;
    Eccentricity   = 0.0;

    Absorption.Clear();
    Emission.Clear();
    Extinction.Clear();
    Scattering.Clear();

    is_constant = false;

    use_absorption = false;
    use_emission   = false;
    use_extinction = false;
    use_scattering = false;

    ignore_photons = false;

    sc_ext     = 1.0;
    Ratio      = 0.9;
    Confidence = 0.9;
    Variance   = 1.0 / 128.0;

    Sample_Threshold = nullptr;

    Sample_Method = 1;
    AA_Threshold = 0.1;
    AA_Level = 3;
    Jitter = 0.0;
}

Media::Media(const Media& source)
{
    Sample_Threshold = nullptr;

    *this = source;
}

Media::~Media()
{
    if (Sample_Threshold != nullptr)
        delete[] Sample_Threshold;

    for (vector<PIGMENT*>::iterator i = Density.begin(); i != Density.end(); ++ i)
        Destroy_Pigment(*i);
}

Media& Media::operator=(const Media& source)
{
    if(&source != this)
    {
        Type = source.Type;
        Intervals = source.Intervals;
        Min_Samples = source.Min_Samples;
        Max_Samples = source.Max_Samples;
        Sample_Method = source.Sample_Method;
        is_constant = source.is_constant;
        use_absorption = source.use_absorption;
        use_emission = source.use_emission;
        use_extinction = source.use_extinction;
        use_scattering = source.use_scattering;
        ignore_photons = source.ignore_photons;
        Jitter = source.Jitter;
        Eccentricity = source.Eccentricity;
        sc_ext = source.sc_ext;
        Absorption = source.Absorption;
        Emission = source.Emission;
        Extinction = source.Extinction;
        Scattering = source.Scattering;
        Ratio = source.Ratio;
        Confidence = source.Confidence;
        Variance = source.Variance;
        AA_Threshold = source.AA_Threshold;
        AA_Level = source.AA_Level;

        if (Sample_Threshold != nullptr)
            delete[] Sample_Threshold;
        Sample_Threshold = nullptr;

        for (vector<PIGMENT*>::iterator i = Density.begin(); i != Density.end(); ++ i)
            Destroy_Pigment(*i);
        Density.resize(0);
        Density.reserve(source.Density.size());
        for (vector<PIGMENT*>::const_iterator i = source.Density.begin(); i != source.Density.end(); ++ i)
            Density.push_back(Copy_Pigment(*i));

        if (source.Sample_Threshold != nullptr)
        {
            if(Intervals > 0)
            {
                Sample_Threshold = new DBL[Intervals];

                for(int i = 0; i < Intervals; i++)
                    Sample_Threshold[i] =  source.Sample_Threshold[i];
            }
        }
    }

    return *this;
}

void Media::Transform(const TRANSFORM *Trans)
{
    Transform_Density(Density, Trans);
}

void Media::PostProcess()
{
    int i;
    DBL t;

    // Get extinction coefficient.
    Extinction = Absorption + sc_ext * Scattering;

    // Determine used effects.

    is_constant = Density.empty();

    use_absorption = !Absorption.IsZero();
    use_emission   = !Emission.IsZero();
    use_scattering = !Scattering.IsZero();
    use_extinction = use_absorption || use_scattering;

    // Init sample threshold array.
    if (Sample_Threshold != nullptr)
        delete[] Sample_Threshold;

    // Create list of thresholds for confidence test.
    Sample_Threshold = new DBL[Max_Samples];

    if(Max_Samples > 1)
    {
        t = chdtri((DBL)(Max_Samples-1), Confidence);

        if(t > 0.0)
            t = Variance / t;
        else
            t = Variance * EPSILON;

        for(i = 0; i < Max_Samples; i++)
            Sample_Threshold[i] = t * chdtri((DBL)(i+1), Confidence);
    }
    else
        Sample_Threshold[0] = 0.0;

    for (vector<PIGMENT*>::iterator i = Density.begin(); i != Density.end(); ++ i)
        Post_Pigment(*i);
}

void Transform_Density(vector<PIGMENT*>& Density, const TRANSFORM *Trans)
{
    for (vector<PIGMENT*>::iterator i = Density.begin(); i != Density.end(); ++ i)
        Transform_Tpattern(*i, Trans);
}

MediaFunction::MediaFunction(TraceThreadData *td, Trace *t, PhotonGatherer *pg) :
    randomNumbers(0.0, 1.0, 32768),
    randomNumberGenerator(&randomNumbers),
    threadData(td),
    trace(t),
    photonGatherer(pg)
{
}

void MediaFunction::ComputeMedia(vector<Media>& mediasource, const Ray& ray, Intersection& isect, MathColour& colour, ColourChannel& transm)
{
    if(!mediasource.empty())
    {
        MediaVector medialist;

        for(vector<Media>::iterator im(mediasource.begin()); im != mediasource.end(); im++)
            medialist.push_back(&(*im));

        // Note: this version of ComputeMedia does not deposit photons. This is
        // intentional.  Even though we're processing a photon ray, we don't want
        // to deposit photons in the infinite atmosphere, only in contained
        // media, which is processed later (in ComputeLightedTexture).  [nk]
        if(!medialist.empty())
            ComputeMedia(medialist, ray, isect, colour, transm);
    }
}

void MediaFunction::ComputeMedia(const RayInteriorVector& mediasource, const Ray& ray, Intersection& isect, MathColour& colour, ColourChannel& transm)
{
    if(!mediasource.empty())
    {
        MediaVector medialist;

        for(RayInteriorVector::const_iterator i(mediasource.begin()); i != mediasource.end(); i++)
        {
            for(vector<Media>::iterator im((*i)->media.begin()); im != (*i)->media.end(); im++)
                medialist.push_back(&(*im));
        }

        // Note: this version of ComputeMedia does not deposit photons. This is
        // intentional.  Even though we're processing a photon ray, we don't want
        // to deposit photons in the infinite atmosphere, only in contained
        // media, which is processed later (in ComputeLightedTexture).  [nk]
        if(!medialist.empty())
            ComputeMedia(medialist, ray, isect, colour, transm);
    }
}

/*****************************************************************************
* INPUT
*   Ray       - Current ray, start point P0
*   Inter     - Current intersection, end point P1
*   Colour    - Color emitted at P1 towards P0
*   light_ray - true if we are looking at a light source ray
* OUTPUT
*   Colour    - Color arriving at the end point
******************************************************************************/

void MediaFunction::ComputeMedia(MediaVector& medias, const Ray& ray, Intersection& isect, MathColour& colour, ColourChannel& transm)
{
    LightSourceEntryVector lights;
    LitIntervalVector litintervals;
    MediaIntervalVector mediaintervals;
    Media *IMedia;
    bool all_constant_and_light_ray = ray.IsShadowTestRay();  // is all the media constant?
    bool ignore_photons = true;
    bool use_extinction = false;
    bool use_scattering = false;
    int minSamples;
    DBL aa_threshold = HUGE_VAL;

    // Find media with the largest number of intervals.
    IMedia = medias.front();

    for(MediaVector::iterator i(medias.begin()); i != medias.end(); i++)
    {
        // find media with the most intervals
        if((*i)->Intervals > IMedia->Intervals)
            IMedia = (*i);

        // find smallest AA_Threshold
        if((*i)->AA_Threshold < aa_threshold)
            aa_threshold = (*i)->AA_Threshold;

        // do not ignore photons if at least one media wants photons
        ignore_photons = ignore_photons && (*i)->ignore_photons;

        // use extinction if at leeast one media wants extinction
        use_extinction = use_extinction || (*i)->use_extinction;

        // use scattering if at leeast one media wants scattering
        use_scattering = use_scattering || (*i)->use_scattering;

        // NK fast light_ray media calculation for constant media
        for (vector<PIGMENT*>::iterator ii = (*i)->Density.begin(); ii != (*i)->Density.end(); ++ ii)
            all_constant_and_light_ray = all_constant_and_light_ray && ((*ii)->Type == PLAIN_PATTERN);
    }

    // If this is a light ray and no extinction is used we can return.
    if((ray.IsShadowTestRay()) && (!use_extinction))
        return;

    // Prepare the Monte Carlo integration along the ray from P0 to P1.
    if(!ray.IsShadowTestRay())
        ComputeMediaLightInterval(lights, litintervals, ray, isect);

    if(litintervals.empty())
        litintervals.push_back(LitInterval(false, 0.0, isect.Depth, 0, 0));

    // Set up sampling intervals (makes sure we will always have enough intervals)
    ComputeMediaSampleInterval(litintervals, mediaintervals, IMedia);

    if(mediaintervals.front().s0 > 0.0)
        mediaintervals.insert(mediaintervals.begin(),
                              MediaInterval(false, 0,
                              0.0,
                              mediaintervals.front().s0,
                              mediaintervals.front().s0,
                              0, 0));
    if(mediaintervals.back().s1 < isect.Depth)
        mediaintervals.push_back(MediaInterval(false, 0,
                                 mediaintervals.back().s1,
                                 isect.Depth,
                                 isect.Depth - mediaintervals.back().s1,
                                 0, 0));

    minSamples = IMedia->Min_Samples;

    // Sample all intervals.
    if((IMedia->Sample_Method == 3) && !all_constant_and_light_ray) //  adaptive sampling
        ComputeMediaAdaptiveSampling(medias, lights, mediaintervals, ray, IMedia, aa_threshold, minSamples, ignore_photons, use_scattering);
    else
        ComputeMediaRegularSampling(medias, lights, mediaintervals, ray, IMedia, minSamples, ignore_photons, use_scattering, all_constant_and_light_ray);

    ComputeMediaColour(mediaintervals, colour, transm);
}

void MediaFunction::ComputeMediaRegularSampling(MediaVector& medias, LightSourceEntryVector& lights, MediaIntervalVector& mediaintervals,
                                                const Ray& ray, const Media *IMedia, int minsamples, bool ignore_photons, bool use_scattering, bool all_constant_and_light_ray)
{
    int j;
    DBL n;
    MathColour Va;
    DBL d0;
    MathColour C0;
    MathColour od0;

    threadData->Stats()[Media_Intervals] += mediaintervals.size();
    for(MediaIntervalVector::iterator i(mediaintervals.begin()); i != mediaintervals.end(); i++)
    {
        // Sample current interval.

        for(j = 0; j < minsamples; j++)
        {
            if(IMedia->Sample_Method == 2)
            {
                d0 = (j + 0.5) / minsamples + (randomNumberGenerator() * IMedia->Jitter / minsamples);
                ComputeOneMediaSample(medias, lights, *i, ray, d0, C0, od0, 2, ignore_photons, use_scattering, false);
            }
            else
            {
                // we may get here with media method 3
                d0 = randomNumberGenerator();
                ComputeOneMediaSample(medias, lights, *i, ray, d0, C0, od0, 1, ignore_photons, use_scattering, false);
            }

            if(all_constant_and_light_ray)
                j = minsamples;
        }
    }

    // Cast additional samples if necessary.
    if((!ray.IsShadowTestRay()) && (IMedia->Max_Samples > minsamples))
    {
        for(MediaIntervalVector::iterator i(mediaintervals.begin()); i != mediaintervals.end(); i++)
        {
            if(i->samples < IMedia->Max_Samples)
            {
                // Get variance of samples.
                n = 1.0 / (DBL)i->samples;

                Va = ((i->te2 * n) - Sqr(i->te * n)) * n;

                // Take additional samples until variance is small enough.
                while(!Va.IsNearZero(IMedia->Sample_Threshold[i->samples - 1]))
                {
                    // Sample current interval again.
                    ComputeOneMediaSample(medias, lights, *i, ray, randomNumberGenerator(), C0, od0, 1, ignore_photons, use_scattering, false);

                    // Have we reached maximum number of samples.
                    if(i->samples > IMedia->Max_Samples)
                        break;

                    // Get variance of samples.
                    n = 1.0 / (DBL)i->samples;

                    Va = ((i->te2 * n) - Sqr(i->te * n)) * n;
                }
            }
        }
    }
}

void MediaFunction::ComputeMediaAdaptiveSampling(MediaVector& medias, LightSourceEntryVector& lights, MediaIntervalVector& mediaintervals,
                                                 const Ray& ray, const Media *IMedia, DBL aa_threshold, int minsamples, bool ignore_photons, bool use_scattering)
{
    // adaptive sampling
    int subIntervalCount;
    int j;
    DBL d0, d1, dd;
    MathColour C0, C1, Result;
    MathColour ODResult;
    MathColour od0, od1;

    for(MediaIntervalVector::iterator i(mediaintervals.begin()); i != mediaintervals.end(); i++)
    {
        // Sample current interval.

        threadData->Stats()[Media_Intervals]++;

        subIntervalCount = (minsamples + 1) / 2;

        // TODO - if minsamples is guaranteed to be >=1, the following is redundant:
        if(subIntervalCount < 1)
            subIntervalCount = 1;

        dd = 1.0 / (DBL)subIntervalCount;

        ComputeOneMediaSample(medias, lights, *i, ray, dd * IMedia->Jitter * (randomNumberGenerator() - 0.5), C0, od0, 3, ignore_photons, use_scattering, false);

        // clear out od & te
        i->te.Clear();
        i->od.Clear();

        d0 = 0.0;
        for(j = 1; j <= subIntervalCount; j++)
        {
            d1 = d0 + dd;
            ComputeOneMediaSample(medias, lights, *i, ray, d1 + dd * IMedia->Jitter * (randomNumberGenerator() - 0.5), C1, od1, 3, ignore_photons, use_scattering, false);
            ComputeOneMediaSampleRecursive(medias, lights, *i, ray, d0, d1, Result, C0, C1, ODResult, od0, od1, IMedia->AA_Level - 1,
                                           IMedia->Jitter, aa_threshold, ignore_photons, use_scattering, false);

            // keep a sum of the results
            // do some attenuation, too, since we are doing samples in order
            // TODO - we could do even better if we handled attenuation on a per-sample basis

            // Compute attenuation due to earlier sub-intervals.
            Result *= Exp(-(i->od) * dd);
            // Compute attenuation due to the sub-interval itself.
            // NB: This formula is mathematically precise under the presumption that the ratio of emission to absorbtion
            // remains constant throughout the entire sub-interval.
            for (int iChannel = 0; iChannel < Result.channels; ++iChannel)
            {
                if (ODResult[iChannel] != 0.0)
                    Result[iChannel] *= (1.0 - exp(-ODResult[iChannel] * dd)) / (ODResult[iChannel] * dd);
            }
            i->te += Result;

            // move c1 to c0 to go on to next sample/interval
            C0 = C1;

            // now do the same for optical depth
            i->od += ODResult;

            // move od1 to od0 to go on to the next sample/interval
            od0 = od1;

            d0 = d1;
        }

        i->samples = subIntervalCount;
    }
}

void MediaFunction::ComputeMediaColour(MediaIntervalVector& mediaintervals, MathColour& colour, ColourChannel& transm)
{
    MathColour Od, Te;
    DBL n;

    // Sum the influences of all intervals.
    for(MediaIntervalVector::iterator i(mediaintervals.begin()); i != mediaintervals.end(); i++)
    {
        n = 1.0 / (DBL)i->samples;

        // Add total emission.
        Te += i->te * n * Exp(-Od);

        // Add optical depth of ient interval.
        Od += i->od * n;
    }

    // Add contribution estimated for the participating media.
    Od = Exp(-Od);

    colour = colour * Od + Te;
    transm *= Od.Greyscale(); // TODO - in the long run, we should make transm a full-fledged RGB term
}

void MediaFunction::ComputeMediaSampleInterval(LitIntervalVector& litintervals, MediaIntervalVector& mediaintervals, const Media *media)
{
    size_t i, j, n, r, remaining, intervals;
    DBL delta, sum, weight;

    // Set up sampling intervals.
    //
    // NK samples - we will always have enough intervals
    // we always use the larger of the two numbers
    intervals = max(size_t(media->Intervals), litintervals.size());

    // Choose intervals.
    if(litintervals.size() == 1)
    {
        // Use one interval if no lit intervals and constant media.
        if((litintervals[0].lit == false) && (media->is_constant == true))
        {
            mediaintervals.push_back(MediaInterval(false, 0,
                                                   litintervals[0].s0,
                                                   litintervals[0].s1,
                                                   litintervals[0].ds,
                                                   0, 0));
        }
        else // Use uniform intervals.
        {
            delta = litintervals[0].ds / (DBL)intervals;

            for(i = 0; i < intervals; i++)
            {
                mediaintervals.push_back(MediaInterval(litintervals[0].lit, 0,
                                                       litintervals[0].s0 + delta * (DBL)i,
                                                       litintervals[0].s0 + delta * (DBL)(i + 1),
                                                       delta,
                                                       litintervals[0].l0, litintervals[0].l1));
            }
        }
    }
    else // Choose intervals according to the specified ratio.
    {
        sum = 0.0;

        for(i = 0; i < litintervals.size(); i++)
            sum += ((litintervals[i].lit) ? (media->Ratio) : (1.0 - media->Ratio));

        remaining = intervals;

        for(i = 0; i < litintervals.size(); i++)
        {
            weight = ((litintervals[i].lit) ? (media->Ratio) : (1.0 - media->Ratio));
            n = size_t(weight / sum * (DBL)intervals) + 1;
            r = remaining - litintervals.size() + i + 1;

            if(n > r)
                n = r;

            delta = litintervals[i].ds / (DBL)n;

            for (j = 0; j < n; j++)
            {
                mediaintervals.push_back(MediaInterval(litintervals[i].lit, 0,
                                                       litintervals[i].s0 + delta * (DBL)j,
                                                       litintervals[i].s0 + delta * (DBL)(j + 1),
                                                       delta,
                                                       litintervals[i].l0, litintervals[i].l1));
            }

            remaining -= n;
        }
    }
}

void MediaFunction::ComputeMediaLightInterval(LightSourceEntryVector& lights, LitIntervalVector& litintervals, const Ray& ray, const Intersection& isect)
{
    if (isect.Object != nullptr)
    {
        if((isect.Object->Flags & NO_GLOBAL_LIGHTS_FLAG) != NO_GLOBAL_LIGHTS_FLAG)
        {
            for(vector<LightSource *>::iterator i(threadData->lightSources.begin()); i != threadData->lightSources.end(); i++)
            {
                if((*i)->Media_Interaction == true)
                    ComputeOneMediaLightInterval(*i, lights, ray, isect);
            }
        }

        for(vector<LightSource *>::iterator i(isect.Object->LLights.begin()); i != isect.Object->LLights.end(); i++)
        {
            if((*i)->Media_Interaction == true)
                ComputeOneMediaLightInterval(*i, lights, ray, isect);
        }
    }
    else
    {
        for(vector<LightSource *>::iterator i(threadData->lightSources.begin()); i != threadData->lightSources.end(); i++)
        {
            if((*i)->Media_Interaction == true)
                ComputeOneMediaLightInterval(*i, lights, ray, isect);
        }
    }

    if(lights.empty() == false)
    {
#if 1
        // Using thread storage duration for the following temporary lists to avoid repeated
        // cycles of allocation, construction, upsizing and destruction. Of course we still
        // need to make sure we start with a clean slate each time around. We also set an
        // initial minimum capacity.
        thread_local POV_SIMPLE_VECTOR<DBL> s0;
        thread_local POV_SIMPLE_VECTOR<DBL> s1;
        s0.clear();
        s1.clear();
        s0.reserve(LIGHTSOURCE_VECTOR_SIZE);
        s1.reserve(LIGHTSOURCE_VECTOR_SIZE);

        for (LightSourceEntryVector::iterator i (lights.begin()); i != lights.end(); i++)
        {
            s0.push_back(i->s0);
            s1.push_back(i->s1);
        }
        std::sort(s0.begin(), s0.end());
        std::sort(s1.begin(), s1.end());

        if (s0[0] > 0.0)
            litintervals.push_back(LitInterval(false, 0.0, s0[0], 0, lights.size() - 1));
        litintervals.push_back(LitInterval(true, s0[0], s1[0], 0, lights.size() - 1));
        for (int i = 1; i < lights.size(); i++)
        {
            if (s0[i] > litintervals.back().s1)
            {
                litintervals.push_back(LitInterval(false, litintervals.back().s1, s0[i], 0, lights.size() - 1));
                litintervals.push_back(LitInterval(true, s0[i], s1[i], 0, lights.size() - 1));
            }
            else
            {
                if (s1[i] > litintervals.back().s1)
                    litintervals.back().s1 = s1[i];
            }
        }

        if (litintervals.back().s1 < isect.Depth)
            litintervals.push_back(LitInterval(false, litintervals.back().s1, isect.Depth, 0, lights.size() - 1));
        for (LitIntervalVector::iterator i(litintervals.begin()); i != litintervals.end(); i++)
            i->ds = i->s1 - i->s0;
#else
        // After sorting the following holds true for the whole array:
        // l[i].s <= l[i + 1].s
        // Where i is the index and s is the start of the interval
        // lit by the light source in the array l.
        sort(lights.begin(), lights.end());

        LightSourceIntersectionVector lsie;

        for(size_t i = 0; i < lights.size(); i++)
        {
            lsie.push_back(LightSourceIntersectionEntry(lights[i].s0, i, true));
            lsie.push_back(LightSourceIntersectionEntry(lights[i].s1, i, false));
        }

        sort(lsie.begin(), lsie.end());

        // TODO - Everything below this line can be merged such that no LitIntervals are needed
        // because ComputeMediaLightInterval just iterates over this LitIntervals with ++ and
        // thus we can generate them on the fly and do not need the temporary storage for all
        // the LitIntervals! [trf]

        // if there is at least one interval (two values in lsie)
        if(lsie.size() > 1)
        {
            size_t lits = 0;
            if(lsie[0].lit == true)
                lits++;
            for(size_t i = 1, maxl = 0, minl = lsie[0].l; i < lsie.size(); i++)
            {
                maxl = max(maxl, lsie[i].l);
                litintervals.push_back(LitInterval(lits > 0, lsie[i - 1].s, lsie[i].s, minl, maxl));
                if(lsie[i].lit == false)
                    lits--;
                else
                {
                    if(lits == 0)
                        minl = lsie[i].l;
                    lits++;
                }
            }
        }
#endif
    }
}

void MediaFunction::ComputeOneMediaLightInterval(LightSource *light, LightSourceEntryVector&lights, const Ray& ray, const Intersection& isect)
{
    LightSourceEntry lse;
    DBL t1 = 0.0, t2 = 0.0;
    bool insert = false;

    lse.light = light;

    // Init interval.
    lse.s0 = 0.0;
    lse.s1 = MAX_DISTANCE;

    switch(light->Light_Type)
    {
        case CYLINDER_SOURCE:
            if(ComputeCylinderLightInterval(ray, light, &t1, &t2))
                insert = ((t1 < isect.Depth) && (t2 > SMALL_TOLERANCE));
            break;
        case POINT_SOURCE:
            t1 = 0.0;
            t2 = isect.Depth;
            insert = true;
            break;
        case SPOT_SOURCE:
            if(ComputeSpotLightInterval(ray, light, &t1, &t2))
                insert = ((t1 < isect.Depth) && (t2 > SMALL_TOLERANCE));
            break;
    }

    if(insert == true)
    {
        lse.s0 = max(t1, 0.0);
        lse.s1 = min(t2, isect.Depth);

        lights.push_back(lse);
    }
}

bool MediaFunction::ComputeSpotLightInterval(const Ray &ray, const LightSource *Light, DBL *d1, DBL *d2)
{
    int viewpoint_is_in_cone;
    DBL a, b, c, d, m, l, l1, l2, t, t1, t2, k1, k2, k3, k4;
    Vector3d V1;

    // Get cone's slope. Note that cos(falloff) is stored in Falloff!
    m = 1 / (Light->Falloff * Light->Falloff);

    V1 = ray.Origin - Light->Center;
    k1 = dot(ray.Direction, Light->Direction);
    k2 = dot(V1, Light->Direction);
    l = V1.length();

    if(l > EPSILON)
        viewpoint_is_in_cone = (k2 / l >= Light->Falloff);
    else
        viewpoint_is_in_cone = false;

    if((k1 <= 0.0) && (k2 < 0.0))
        return false;

    k3 = dot(V1, ray.Direction);
    k4 = V1.lengthSqr();

    a = 1.0 - Sqr(k1) * m;
    b = k3 - k1 * k2 * m;
    c = k4 - Sqr(k2) * m;

    if(a != 0.0)
    {
        d = Sqr(b) - a * c;

        if(d > EPSILON)
        {
            d = sqrt(d);

            t1 = (-b + d) / a;
            t2 = (-b - d) / a;

            if(t1 > t2)
            {
                t = t1;
                t1 = t2;
                t2 = t;
            }

            l1 = k2 + t1 * k1;
            l2 = k2 + t2 * k1;

            if((l1 <= 0.0) && (l2 <= 0.0))
                return false;

            if((l1 <= 0.0) || (l2 <= 0.0))
            {
                if(l1 <= 0.0)
                {
                    if(viewpoint_is_in_cone)
                    {
                        t1 = 0.0;
                        t2 = (t2 > 0.0) ? (t2) : (MAX_DISTANCE);
                    }
                    else
                    {
                        t1 = t2;
                        t2 = MAX_DISTANCE;
                    }
                }
                else
                {
                    if(viewpoint_is_in_cone)
                    {
                        t2 = t1;
                        t1 = 0.0;
                    }
                    else
                        t2 = MAX_DISTANCE;
                }
            }

            *d1 = t1;
            *d2 = t2;

            return true;
        }
        else if(d > -EPSILON)
        {
            if(viewpoint_is_in_cone)
            {
                *d1 = 0.0;
                *d2 = -b / a;
            }
            else
            {
                *d1 = -b / a;
                *d2 = MAX_DISTANCE;
            }

            return true;
        }
    }
    else if(viewpoint_is_in_cone)
    {
        *d1 = 0.0;
        *d2 = -c/b;

        return true;
    }

    return false;
}

bool MediaFunction::ComputeCylinderLightInterval(const Ray &ray, const LightSource *Light, DBL *d1, DBL *d2)
{
    DBL a, b, c, d, l1, l2, t, t1, t2, k1, k2, k3, k4;
    Vector3d V1;

    V1 = ray.Origin - Light->Center;
    k1 = dot(ray.Direction, Light->Direction);
    k2 = dot(V1, Light->Direction);

    if((k1 <= 0.0) && (k2 < 0.0))
        return false;

    a = 1.0 - Sqr(k1);

    if(a != 0.0)
    {
        k3 = dot(V1, ray.Direction);
        k4 = V1.lengthSqr();

        b = k3 - k1 * k2;
        c = k4 - Sqr(k2) - Sqr(Light->Falloff);
        d = Sqr(b) - a * c;

        if(d > EPSILON)
        {
            d = sqrt(d);

            t1 = (-b + d) / a;
            t2 = (-b - d) / a;

            if(t1 > t2)
            {
                t = t1;
                t1 = t2;
                t2 = t;
            }

            l1 = k2 + t1 * k1;
            l2 = k2 + t2 * k1;

            if((l1 <= 0.0) && (l2 <= 0.0))
                return false;

            if((l1 <= 0.0) || (l2 <= 0.0))
            {
                if(l1 <= 0.0)
                    t1 = 0.0;
                else
                    t2 = (MAX_DISTANCE - k2) / k1;
            }

            *d1 = t1;
            *d2 = t2;

            return true;
        }
    }

    return false;
}

/*****************************************************************************
* INPUT
*   dist  - distance of current sample
*   Ray   - pointer to ray
*   IMedia - pointer to media to use
* OUTPUT
*   Col          - color of current sample
******************************************************************************/

void MediaFunction::ComputeOneMediaSample(MediaVector& medias, LightSourceEntryVector& lights, MediaInterval& mediainterval, const Ray &ray, DBL d0, MathColour& SampCol,
                                          MathColour& SampOptDepth, int sample_method, bool ignore_photons, bool use_scattering, bool photonPass)
{
    // NK samples - moved d0 to parameter list
    DBL d1, len;
    Vector3d P, H;
    MathColour C0, Light_Colour;
    MathColour Emission, Extinction, Scattering;
    Ray Light_Ray(ray);

    threadData->Stats()[Media_Samples]++;

    // Set up sampling location.
    d0 *= mediainterval.ds;
    d1 = mediainterval.s0 + d0;
    H = ray.Evaluate(d1);

    // Get coefficients in current sample location.
    for(MediaVector::iterator i(medias.begin()); i != medias.end(); i++)
    {
        P = H;

        Evaluate_Density_Pigment((*i)->Density, P, C0, threadData);

        Extinction += C0 * (*i)->Extinction;

        if(!ray.IsShadowTestRay())
        {
            Emission   += C0 * (*i)->Emission;
            Scattering += C0 * (*i)->Scattering;
        }
    }

    // Get estimate for the total optical depth of the current interval.
    SampOptDepth = Extinction * mediainterval.ds;

    if(sample_method != 3)
        mediainterval.od += SampOptDepth;

    if(!ray.IsShadowTestRay() && use_scattering && !ray.IsPhotonRay())
    {
        if(mediainterval.lit)
        {
            // note for performance: we could skip this if there are no photons (surface or media)

            // determine whether or not this media is ignoring photons
            // save this in the thread data... it will be used by ComputeShadowColour
            // TODO - maybe this should be (or already is?) computed elsewhere and passed in
            //        as a parameter ( see the ignore_photons parameter! )
            //        I need to look closer at the new code to clean that up [NK]
            // assume true, set to false if we find even one
            threadData->litObjectIgnoresPhotons = true;
            for(MediaVector::iterator i(medias.begin()); i != medias.end(); i++)
            {
                if(!(*i)->ignore_photons)
                {
                    threadData->litObjectIgnoresPhotons = false;
                    break;
                }
            }

            // Process all light sources.
            for(size_t i = mediainterval.l0; i <= mediainterval.l1; i++)
            {
                // Use light only if active and within it's boundaries.
                if((d1 >= lights[i].s0) && (d1 <= lights[i].s1))
                {
                    if(!(trace->TestShadow(*lights[i].light, len, Light_Ray, P, Light_Colour)))
                        ComputeMediaScatteringAttenuation(medias, Emission, Scattering, Light_Colour, ray, Light_Ray);
                }
            }
        }

        // process media photons whether or not the interval is directly lit
        if((photonGatherer != nullptr) && (photonGatherer->map->numPhotons > 0))
        {
            ComputeMediaPhotons(medias, Emission, Scattering, ray, H);
        }
    }

    if(sample_method == 3)
    {
        // We're doing the samples in order, so we can attenuate correctly
        // instead of assuming a constant absorption/extinction.
        // Therefore, we do the attenuation later (back up in Simulate_Media).
        Emission *=  mediainterval.ds;
    }
    else
    {
        // NOTE: this assumes constant absorption+extinction over the length of the interval
        Emission *=  mediainterval.ds * Exp(-Extinction * d0);
    }

    SampCol = Emission;

    if(sample_method != 3)
    {
        // Add emission.
        mediainterval.te  += Emission;
        mediainterval.te2 += Sqr(Emission);
    }

    mediainterval.samples++;
}

void MediaFunction::ComputeOneMediaSampleRecursive(MediaVector& medias, LightSourceEntryVector& lights, MediaInterval& mediainterval, const Ray& ray,
                                                   DBL d1, DBL d3, MathColour& Result, const MathColour& C1, const MathColour& C3, MathColour& ODResult, const MathColour& od1, const MathColour& od3,
                                                   int depth, DBL Jitter, DBL aa_threshold, bool ignore_photons, bool use_scattering, bool photonPass)
{
    MathColour C2, Result2;
    MathColour od2, ODResult2;
    DBL d2, jdist;

    // d2 is between d1 and d3 (all in range of 0..1
    d2 = 0.5 * (d1 + d3);
    jdist = d2 + Jitter * (d3 - d1) * (randomNumberGenerator() - 0.5);

    ComputeOneMediaSample(medias, lights, mediainterval, ray, jdist, C2, od2, 3, ignore_photons, use_scattering, photonPass);

    // TODO FIXME - this gives C1, C2 and C3 a weigt of 1:1:1,
    // which is no good as C1 and C3 are on the border of the interval, and may influence the neighboring interval as well.
    // (see individual comments for how to fix this.)

    // if we're at max depth, then let's just use this last sample and average it with the two end points
    if(depth <= 0)
    {
        // average colors & optical depth
        Result   = (C1  + C2  + C3)  / 3.0;
        ODResult = (od1 + od2 + od3) / 3.0;
        // TODO FIXME - this should be
        // Result   = (C1  + 2*C2 + C3)  / 4.0;
        // ODResult = (od1 + 2*C2 + od3) / 4.0;
        // (because C1 and C3 also affect adjacent intervals, while C2 only affects this one)

        // bail out - we're done now
        return;
    }

    // check if we should sample between points 1 and 2
    if(ColourDistance(C1, C2) > aa_threshold)
    {
        // recurse again
        ComputeOneMediaSampleRecursive(medias, lights, mediainterval, ray, d1, d2, Result2, C1, C2, ODResult2, od1, od2,
                                       depth - 1, Jitter, aa_threshold, ignore_photons, use_scattering, photonPass);

        // average colors & optical depth (well, actually do half of the averaging; we'll ad another "half a color" later)
        Result   = Result2   / 2.0;
        ODResult = ODResult2 / 2.0;
        // TODO FIXME - this is actually ok, no fixing required
    }
    else
    {
        // no new points needed - just average what we've got.
        // (we're giving c1 and c2 a relative weight of 2:1, as c2 - the middle point - will make another appearance later)

        // average colors & optical depth (well, actually do half of the averaging; we'll ad another "half a color" later)
        Result   = C1  / 3.0 + C2  / 6.0;
        ODResult = od1 / 3.0 + od2 / 6.0;
        // TODO FIXME - this should be
        // Result   = (C1  + C2)  / 4.0;
        // ODResult = (od1 + od2) / 4.0;
        // (because C1 also affects the adjacent interval)
    }

    // check if we should sample between points 2 and 3
    if(ColourDistance(C2, C3) > aa_threshold)
    {
        // recurse again
        ComputeOneMediaSampleRecursive(medias, lights, mediainterval, ray,  d2, d3, Result2, C2, C3, ODResult2, od2, od3,
                                       depth - 1, Jitter, aa_threshold, ignore_photons, use_scattering, photonPass);

        // average colors & optical depth (well, actually do half of the averaging; we already did "half a color" earlier)
        Result   += Result2   / 2.0;
        ODResult += ODResult2 / 2.0;
        // TODO FIXME - this is actually ok, no fixing required
    }
    else
    {
        // no new points needed - just average what we've got.
        // (we're giving c2 and c3 a relative weight of 1:2, as c2 - the middle point - already made an appearance earlier)

        // average colors & optical depth (well, actually do half of the averaging; we already did "half a color" earlier)
        Result   += C2  / 6.0 + C3  / 3.0;
        ODResult += od2 / 6.0 + od3 / 3.0;
        // TODO FIXME - this should be
        // Result   = (C2  + C3)  / 4.0;
        // ODResult = (od2 + od3) / 4.0;
        // (because C3 also affects the adjacent interval)
    }
}


void MediaFunction::ComputeMediaPhotons(MediaVector& medias, MathColour& Te, const MathColour& Sc, const BasicRay& ray, const Vector3d& H)
{
    BasicRay Light_Ray;
    DBL r;
    int j;
    MathColour Light_Colour;
    MathColour Colour2;

    if((photonGatherer != nullptr) && (photonGatherer->map->numPhotons > 0))
    {
        //PhotonGatherer gatherer2(photonGatherer->map,photonGatherer->photonSettings);
        photonGatherer->gathered = false;
        // statistics
        threadData->Stats()[Gather_Performed_Count]++;

        if(photonGatherer->gathered)
            r = photonGatherer->alreadyGatheredRadius;
        else
            r = photonGatherer->gatherPhotonsAdaptive(&H, nullptr, false);

        Colour2.Clear();

        // now go through these photons and add up their contribution
        for(j = 0; j < photonGatherer->gatheredPhotons.numFound; j++)
        {
            // DBL theta,phi;
            int theta,phi;

            // convert small color to normal color
            Light_Colour = ToMathColour(RGBColour(photonGatherer->gatheredPhotons.photonGatherList[j]->colour));

            // convert theta/phi to vector direction
            // Use a pre-computed array of sin/cos to avoid many calls to the
            // sin() and cos() functions.  These arrays were initialized in
            // InitBacktraceEverything.
            theta = photonGatherer->gatheredPhotons.photonGatherList[j]->theta + 127;
            phi = photonGatherer->gatheredPhotons.photonGatherList[j]->phi + 127;

            Light_Ray.Direction[Y] = sinCosData.sinTheta[theta];
            Light_Ray.Direction[X] = sinCosData.cosTheta[theta];

            Light_Ray.Direction[Z] = Light_Ray.Direction[X]*sinCosData.sinTheta[phi];
            Light_Ray.Direction[X] = Light_Ray.Direction[X]*sinCosData.cosTheta[phi];

            Light_Ray.Origin = Vector3d(photonGatherer->gatheredPhotons.photonGatherList[j]->Loc) - Light_Ray.Direction;

            ComputeMediaScatteringAttenuation(medias, Colour2, Sc, Light_Colour, ray, Light_Ray);
        }

        // finish the photons equation
        Colour2 *= ( 3.0 / (M_PI * r*r*r * 4.0) );

        Te += Colour2;
    }
}

void MediaFunction::ComputeMediaScatteringAttenuation(MediaVector& medias, MathColour& OutputColor, const MathColour& Sc, const MathColour& Light_Colour, const BasicRay& ray, const BasicRay& Light_Ray)
{
    DBL k = 0.0, g = 0.0, g2 = 0.0, alpha = 0.0;

    for(MediaVector::iterator i(medias.begin()); i != medias.end(); i++)
    {
        switch((*i)->Type)
        {
            case RAYLEIGH_SCATTERING:
                alpha = dot(Light_Ray.Direction, ray.Direction);
                k += 0.799372013 * (1.0 + Sqr(alpha));
                break;
            case MIE_HAZY_SCATTERING:
                alpha = dot(Light_Ray.Direction, ray.Direction);
                k += 0.576655375 * (1.0 + 9.0 * pow(0.5 * (1.0 + alpha), 8.0));
                break;
            case MIE_MURKY_SCATTERING:
                alpha = dot(Light_Ray.Direction, ray.Direction);
                k += 0.495714547 * (1.0 + 50.0 * pow(0.5 * (1.0 + alpha), 32.0));
                break;
            case HENYEY_GREENSTEIN_SCATTERING:
                alpha = dot(Light_Ray.Direction, ray.Direction);
                g = (*i)->Eccentricity;
                g2 = Sqr(g);
                k += (1.0 - g2) / pow(1.0 + g2 - 2.0 * g * alpha, 1.5);
                break;
            case ISOTROPIC_SCATTERING:
            default:
                k += 1.0;
                break;
        }
    }

    k /= (DBL)(medias.size());

    OutputColor += k * Sc * Light_Colour;
}

}
// end of namespace pov
