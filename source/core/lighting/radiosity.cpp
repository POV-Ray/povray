//******************************************************************************
///
/// @file core/lighting/radiosity.cpp
///
/// Implementation of radiosity.
///
/// @author Jim McElhiney (original code)
/// @author Christoph Lipka (revisions and updates for POV-Ray v3.7)
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

/************************************************************************
*  Radiosity calculation routies.
*
*  (This does not work the way that most radiosity programs do, but it accomplishes
*  the diffuse interreflection integral the hard way and produces similar results. It
*  is called radiosity here to avoid confusion with ambient and diffuse, which
*  already have well established meanings within POV).
*  Inspired by the paper "A Ray Tracing Solution for Diffuse Interreflection"
*  by Ward, Rubinstein, and Clear, in Siggraph '88 proceedings.
*
*  Basic Idea:  Never use a constant ambient term.  Instead,
*     - For first pixel, cast a whole bunch of rays in different directions
*       from the object intersection point to see what the diffuse illumination
*       really is.  Save this value, after estimating its
*       degree of reusability.  (Method 1)
*     - For second and subsequent pixels,
*         - If there are one or more nearby values already computed,
*           average them and use the result (Method 2), else
*         - Use method 1.
*
*  Implemented by and (c) 1994-6 Jim McElhiney, mcelhiney@acm.org or 71201,1326
*  All standard POV distribution rights granted.  All other rights reserved.
*************************************************************************/

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/lighting/radiosity.h"

// C++ variants of C standard header files
#include <cstring>

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/povassert.h"

// POV-Ray header files (core module)
#include "core/lighting/photons.h"
#include "core/render/ray.h"
#include "core/scene/scenedata.h"
#include "core/scene/tracethreaddata.h"
#include "core/support/octree.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using namespace pov_base;

using std::min;
using std::max;

// #define RAD_GRADIENT 1 // [CLi] gradient seems to provide no gain at best, and may actually cause artifacts
// #define SAW_METHOD 1
// #define SAW_METHOD_ROOT 2
// #define SIGMOID_METHOD 1
#define PSEUDO_SIGMOID_METHOD 1
#define IN_FRONT_LIMIT (-0.05)

// #define SHOW_SAMPLE_SPOTS 1 // try this!  bright spots at sample pts
// #define LOW_COUNT_BRIGHT 1  // this will highlight areas of low density if no extra samples are taken in the final pass

// #define RADDEBUG 1

#define OCTREE_PERFORMANCE_DEBUG 1

const DBL AVG_NEAR_EPSILON = 0.000001;
const DBL RAD_EPSILON = 0.001;
const DBL WEIGHT_ERROR_BOUND_OFFSET = 0.25;

const int PRETRACE_STEP_FINAL  = 0;                                         // dummy value to use instead of pretrace step during final render
const int PRETRACE_STEP_LOADED = std::numeric_limits<signed char>::max();   // dummy value to use instead of pretrace step for samples loaded from file

#define BRILLIANCE_EPSILON 1e-5

// structure used to gather weighted average during tree traversal
struct WT_AVG final
{
    MathColour Weights_Times_Illuminances;  // Aggregates during traversal
    DBL Weights;                            // Aggregates during traversal
    int Weights_Count;                      // Count of points used, aggregates during trav
    int Good_Count;                         // Count of points used, aggregates during trav
    Vector3d P, N;                          // Point and Normal:  input to traverse
    DBL Brilliance;                         // Surface brilliance
    DBL Current_Error_Bound;                // see Radiosity_Error_Bound
    int Pass;                               // Current pass (FINAL_TRACE for final render)
    int TileId;                             // Current tile

    /* [CLi] obsolete
    MathColour Weight_Times_Illuminance[MAX_NEAREST_COUNT];
    DBL Weight[MAX_NEAREST_COUNT];
    DBL Distance[MAX_NEAREST_COUNT];
    */

#ifdef OCTREE_PERFORMANCE_DEBUG
    int Lookup_Count;           // Count of points supplied by tree lookup
    int AcceptPass_Count;       // Count of points accepted by test for pass & tile ID
    int AcceptQuick_Count;      // Count of points accepted by quick range test
    int AcceptGeometry_Count;   // Count of points accepted by test for interfering geometry
    int AcceptNormal_Count;     // Count of points accepted by surface curvature test
    int AcceptInFront_Count;    // Count of points accepted by "in front" test
    int AcceptEpsilon_Count;    // Count of points accepted by borderline-case test (weight < EPSILON)
#endif
};

inline QualityFlags GetRadiosityQualityFlags(const SceneRadiositySettings& rs, const QualityFlags& basicQualityFlags)
{
    QualityFlags qf = basicQualityFlags;

    qf.areaLights = false; // TODO - in some cases we might want this anyway

    if(!rs.media)
        qf.media = false;

    if(!rs.subsurface)
        qf.subsurface = false;

    return qf;
}

static const unsigned int BLOCK_POOL_UNIT_SIZE = 32;

struct RadiosityCache::BlockPool::PoolUnit final
{
    PoolUnit *next;
    ot_block_struct blocks[BLOCK_POOL_UNIT_SIZE];
    PoolUnit(PoolUnit *n) : next(n) { }
};


// --------------------------------------------------------------------------------
//  Compute secondary parameters for the radiosity algorithm
// --------------------------------------------------------------------------------

RadiosityRecursionSettings* SceneRadiositySettings::GetRecursionSettings(bool final) const
{
    RadiosityRecursionSettings* recSettings = new RadiosityRecursionSettings[recursionLimit];

    for (unsigned int depth = 0; depth < recursionLimit; depth ++)
    {
        // --------------------------------------------------------------------------------
        // Number of rays to shoot per sample:
        //  Reduce by factor of 2 per recursion; shoot at least 5 rays
        // Rationale:
        //  The deeper we recurse, the higher the error we can accept; the original paper
        //  by Ward et al. suggests to reduce the number of rays by 50% per bounce, based
        //  on an estimated average reflectivity of 50% throughout the scene; the version
        //  v3.6 code enforced a minimum of 5 rays, possibly for some hidden reason, so we
        //  follow this example.
        // Compatibility:
        //  POV-Ray v3.6 reduced by factor 3 for 1st recursion, and again by factor 2 for
        //  2nd recursion, using that same value for all consecutive recursions; in any
        //  case, at least 5 rays were shot.

        recSettings[depth].raysPerSample = max(5, int(count * pow(0.5, (double)depth)));

        // --------------------------------------------------------------------------------
        // Minimum number of samples to re-use to compute a point's diffuse illumination:
        //  Reduce to 2 for 1st recursion, to 1 for all consecutive recursions
        // Rationale:
        //  The rays picking up deeper bounce samples will be more or less random and
        //  averaged anyway, so we can be lazy about this at deeper bounces.
        // Compatibility:
        //  POV-Ray v3.6 reduced to 2 for 1st recursion, 1 for all consecutive recursions

        switch (depth)
        {
            case 0:     recSettings[depth].reuseCount = nearestCount;             break;
            case 1:     recSettings[depth].reuseCount = min(2,(int)nearestCount); break;
            default:    recSettings[depth].reuseCount = 1;                        break;
        }

        // --------------------------------------------------------------------------------
        // Factor governing spacing of samples in general:
        //  Increase by factor of 2.0 per recursion
        // Rationale:
        //  The deeper we recurse, the higher the error we can accept; the original paper
        //  from Ward et al. suggests to increase the error bound by 40% per bounce, based
        //  on an estimated average reflectivity of 50% throughout the scene; however, we
        //  follow the more radical example of POV-Ray v3.6.
        // Compatibility:
        //  POV-Ray v3.6 increased by factor 2 per recursion

        recSettings[depth].errorBoundFactor = 1.0 * pow(2.0, (double)depth);

        // --------------------------------------------------------------------------------
        // Factor governing minimum & maximum spacing of samples:
        //  Increase by factor of 2.0 per recursion
        // Rationale:
        //  The deeper we recurse, the less we want to go into details; The factor is an
        //  arbitrary value. (NOTE: The effect of this *multiplies* with that of
        //  errorBoundFactor!)
        // Compatibility:
        //  POV-Ray v3.6 increased by factor 2 per recursion

        recSettings[depth].minReuseFactor = minimumReuse * pow(2.0, (double)depth);
        recSettings[depth].maxReuseFactor = maximumReuse * pow(2.0, (double)depth);

        // --------------------------------------------------------------------------------
        // Factor governing octree lookup performance:
        //  Set to 1 for top-level samples; set to 8 for 1st and higher recursion
        // Rationale:
        //  Octree lookup performance must be well-balanced between "false positives"
        //  (samples produced by lookup but actually unsuitable for re-use) and "false
        //  negatives" (re-usable samples not found by lookup); false positives will cost
        //  performance, while false negatives will cause artifacts at octree cell bounds.
        //  The higher this number, the more false negatives (but the fewer false positives)
        //  will occur; the lowest sensible value is 1, preventing false negatives
        //  altogether.
        //  For top-level samples we are going for artifact-free render; for any other
        //  recursion depth, we are going for optimum performance. 8 has proven a good
        //  value in this respect.
        // Compatibility:
        //  POV-Ray v3.6 used 1 for top-level samples, increasing by factor of 2 [?] per
        //  recursion; there is reason to believe that this was unintentional.

        if (depth == 0)
            recSettings[depth].octreeOverfillFactor = 1.0;
        else
            recSettings[depth].octreeOverfillFactor = 8.0;

        // --------------------------------------------------------------------------------
        // Base trace level to use for secondary rays:
        //  Set to ~1.5 for top-level samples; increase by ~1.5 per recursion
        // Base weight to use for secondary rays:
        //  Set to 50% * brightness for top-level samples; reduce by factor of
        //  50% * brightness per recursion
        // Rationale:
        //  Radiosity secondary rays should take into account trace level and weight of
        //  primary rays; however, these may be different depending on the path the primary
        //  ray has taken, so estimates must be used instead of the actual values.
        //  Primary rays are expected to come in after 0 or 1 reflections on average, at
        //  an average weight of 50%. The values additionally take into account one trace
        //  level increment per radiosity recursion, and the basic radiosity brightness
        //  factor.
        // Compatibility:
        //  POV-Ray v3.6 used the trace level of the primary ray that happened to cause the
        //  sample to be taken, causing artifacts in scenes with reflective surfaces.

        recSettings[depth].traceLevel = int((1.5 * ((double)depth + 1)));
        recSettings[depth].weight     = pow(0.5 * brightness, (double)depth + 1);

        // --------------------------------------------------------------------------------
        // Precomputed Values:
        //  These are not "tweakables", but instead are just values pre-computed from the
        //  above settings

        recSettings[depth].maxErrorBound       = errorBound * recSettings[depth].errorBoundFactor;
        recSettings[depth].octreeAddressFactor = recSettings[depth].maxErrorBound / recSettings[depth].octreeOverfillFactor;
    }

    return recSettings;
}

RadiosityFunction::RadiosityFunction(std::shared_ptr<SceneData> sd, TraceThreadData *td, const SceneRadiositySettings& rs,
                                     RadiosityCache& rc, Trace::CooperateFunctor& cf, bool ft, const Vector3d& camera) :
    threadData(td),
    trace(sd, td, GetRadiosityQualityFlags(rs, QualityFlags(9)), cf, media, *this), // TODO FIXME - the only reason we can safely hard-code level-9 quality here is because radiosity happens to be disabled at lower settings
    media(td, &trace, &photonGatherer),
    photonGatherer(&sd->surfacePhotonMap, sd->photonSettings),
    radiosityCache(rc),
    errorBound(rs.errorBound),
    isFinalTrace(ft),
    cameraPosition(camera),
    pretraceStep(PRETRACE_INVALID),
    recursionParameters(new RecursionParameters[rs.recursionLimit]),
    topLevelQueryCount(0),
    topLevelReuse(0.0),
    tileId(0),
    cacheBlockPool(nullptr),
    settings(rs),
    recursionSettings(rs.GetRecursionSettings(ft))
{
    if (!isFinalTrace)
        errorBound *= rs.lowErrorFactor;
}

RadiosityFunction::~RadiosityFunction()
{
    if (cacheBlockPool != nullptr) // shouldn't happen normally, but does happen when render is aborted
    {
        radiosityCache.ReleaseBlockPool(cacheBlockPool);
        cacheBlockPool = nullptr;
    }

    delete[] recursionSettings;
    delete[] recursionParameters;
}

void RadiosityFunction::GetTopLevelStats(long& queryCount, float& reuse)
{
    queryCount = topLevelQueryCount;
    reuse      = topLevelReuse;
}

void RadiosityFunction::ResetTopLevelStats()
{
    topLevelQueryCount = 0;
    topLevelReuse      = 0.0;
}

void RadiosityFunction::BeforeTile(int id, unsigned int pts)
{
    if (isFinalTrace)
        POV_RADIOSITY_ASSERT(pts == FINAL_TRACE);
    else
        POV_RADIOSITY_ASSERT((pts >= PRETRACE_FIRST) && (pts <= PRETRACE_MAX));

    // different pretrace step than last tile
    if (pts != pretraceStep)
    {
        // Recursion Level 0
        recursionParameters[0].statsId              = (isFinalTrace ? Radiosity_SamplesTaken_Final_R0 : (IntStatsIndex)(Radiosity_SamplesTaken_PTS1_R0 + min(4u,pts-PRETRACE_FIRST)*5));
        recursionParameters[0].queryCountStatsId    = Radiosity_QueryCount_R0;
        recursionParameters[0].weightStatsId        = Radiosity_Weight_R0;
        // Recursion Level 1+
        for (unsigned int depth = 1; depth < settings.recursionLimit; depth ++)
        {
            recursionParameters[depth].statsId              = (IntStatsIndex)(recursionParameters[0].statsId            + min(4u,depth));
            recursionParameters[depth].queryCountStatsId    = (IntStatsIndex)(recursionParameters[0].queryCountStatsId  + min(4u,depth));
            recursionParameters[depth].weightStatsId        = (FPStatsIndex) (recursionParameters[0].weightStatsId      + min(4u,depth));
        }
    }

    pretraceStep = pts;
    tileId = id;

    // next tile, so we start the sample direction pattern all over again
    for (unsigned int depth = 0; depth < settings.recursionLimit; depth ++)
        recursionParameters[depth].directionGenerator.Reset(settings.directionPoolSize);

    POV_RADIOSITY_ASSERT(cacheBlockPool == nullptr);
    cacheBlockPool = radiosityCache.AcquireBlockPool();
}

void RadiosityFunction::AfterTile()
{
    // release block pool, just in case this happens to be the last tile for this thread
    radiosityCache.ReleaseBlockPool(cacheBlockPool);
    cacheBlockPool = nullptr;
}

void RadiosityFunction::ComputeAmbient(const Vector3d& ipoint, const Vector3d& raw_normal, const Vector3d& layer_normal, DBL brilliance, MathColour& ambient_colour, DBL weight, TraceTicket& ticket)
{
    DBL temp_error_bound = errorBound;
    const RecursionParameters& param = recursionParameters[ticket.radiosityRecursionDepth];
    const RadiosityRecursionSettings& recSettings = recursionSettings[ticket.radiosityRecursionDepth];
    DBL reuse;
    DBL tmpBrilliance = (settings.brilliance? brilliance : 1.0);

    Vector3d effectiveNormal(settings.normal ? layer_normal : raw_normal);

    threadData->Stats()[param.queryCountStatsId]    ++;
    threadData->Stats()[param.weightStatsId]        += weight;

    // TODO CLARIFY - what exactly is the rationale behind this formula?
    if(weight < WEIGHT_ERROR_BOUND_OFFSET)
        temp_error_bound += (WEIGHT_ERROR_BOUND_OFFSET - weight);

    reuse = radiosityCache.FindReusableBlock(threadData->Stats(), temp_error_bound * recSettings.errorBoundFactor, ipoint, effectiveNormal, tmpBrilliance, ambient_colour, ticket.radiosityRecursionDepth, pretraceStep, tileId);

    if (ticket.radiosityRecursionDepth == 0)
    {
        topLevelQueryCount ++;
        topLevelReuse += reuse*4;
    }

    // allow more samples on final trace (rather than radiosity pretrace) - unless user says not to
    if((reuse*4 >= recSettings.reuseCount) || ((isFinalTrace == true) && (settings.alwaysSample == false) && (reuse > 0)))
    {
        threadData->Stats()[Radiosity_ReuseCount]++;
        if (ticket.radiosityRecursionDepth == 0)
        {
            threadData->Stats()[Radiosity_TopLevel_ReuseCount]++;
        }
        if (isFinalTrace)
        {
            threadData->Stats()[Radiosity_Final_ReuseCount]++;
        }

        #ifdef LOW_COUNT_BRIGHT // this will highlight areas of low density if no extra samples are taken in the final pass - not on by default [trf]
            // use this for testing - it will tell you where too few are found
            if(reuse*4 < param.reuseCount)
                ambient_colour.set(4.0f);
        #endif
    }
    else
    {
        MathColour tmpColour;
        double quality = GatherLight(ipoint, raw_normal, effectiveNormal, tmpBrilliance, tmpColour, ticket);

        // If we already found samples nearby (and we just decided to take more), make use of them.
        if (reuse > 0)
            ambient_colour = (ambient_colour * reuse + tmpColour * quality) / (reuse + quality);
        else
            ambient_colour = tmpColour;

        reuse += quality;

        threadData->Stats()[Radiosity_GatherCount]++;
        threadData->Stats()[param.statsId]++;
        if (ticket.radiosityRecursionDepth == 0)
        {
            threadData->Stats()[Radiosity_TopLevel_GatherCount]++;
        }
        if (isFinalTrace)
        {
            threadData->Stats()[Radiosity_Final_GatherCount]++;
        }
    }

    ticket.radiosityQuality = min((float)(4*reuse)/recSettings.reuseCount, ticket.radiosityQuality);

    // note grey spelling:  american options structure with worldbeat calculations!
    ambient_colour = (ambient_colour * (1.0f - settings.grayThreshold)) + (settings.grayThreshold * ambient_colour.Greyscale());

    // Scale up by current brightness factor prior to return
    ambient_colour *= settings.brightness;
}

// returns true if radiosity can be traced, false otherwise (that is, if the radiosity max trace level was already reached)
bool RadiosityFunction::CheckRadiosityTraceLevel(const TraceTicket& ticket)
{
    return (ticket.radiosityRecursionDepth < settings.recursionLimit);
}

/*****************************************************************************
*
* FUNCTION
*
*   ra_gather
*
* INPUT
*   ipoint - a point at which the illumination is needed
*   raw_normal - the surface normal (not perturbed by the current layer) at that point
*   illuminance - a place to put the return result
*   weight - the weight of this point in final output, to drive ADC_Bailout
*
* OUTPUT
*   The average colour of light of objects visible from the specified point.
*   The colour is returned in the illuminance parameter.
*
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*    Gather up the incident light and average it.
*    Return the results in illuminance, and also cache them for later.
*    Note that last parameter is similar to weight parameter used
*    to control ADC_Bailout as a parameter to Trace(), but it also
*    takes into account that this subsystem calculates only ambient
*    values.  Therefore, coming in at the top level, the value might
*    be 0.3 if the first object hit had an ambient of 0.3, whereas
*    Trace() would have been passed a parameter of 1.0 (since it
*    calculates the whole pixel value).
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

double RadiosityFunction::GatherLight(const Vector3d& ipoint, const Vector3d& raw_normal, const Vector3d& layer_normal, DBL brilliance, MathColour& illuminance, TraceTicket& ticket)
{
    unsigned int cur_sample_count;

    Vector3d direction, up, min_dist_vec;
    int save_Max_Trace_Level;
    MathColour dxs, dys, dzs;
    MathColour colour_sums, temp_colour;
    DBL inverse_distance_sum, mean_dist,
        smallest_dist,
        sum_of_inverse_dist, sum_of_dist, gradient_count;
    DBL save_adc_bailout;
    DBL save_radiosityQuality;
    unsigned int save_trace_level;
    RecursionParameters& param = recursionParameters[ticket.radiosityRecursionDepth];
    const RadiosityRecursionSettings& recSettings = recursionSettings[ticket.radiosityRecursionDepth];

    DBL to_eye = (this->cameraPosition - ipoint).length();
    DBL reuse_dist_min      = to_eye * recSettings.minReuseFactor;
    DBL maximum_distance    = to_eye * recSettings.maxReuseFactor;
    if (recSettings.maxReuseFactor >= HUGE_VAL)
        maximum_distance = HUGE_VAL;

    cur_sample_count        = recSettings.raysPerSample;

    /* Save some global stuff which we have to change for now */
    save_Max_Trace_Level    = ticket.maxAllowedTraceLevel;
    save_trace_level        = ticket.traceLevel;
    save_adc_bailout        = ticket.adcBailout;
    save_radiosityQuality   = ticket.radiosityQuality;

    // adjust the max_trace_level
    // [CLi] Set max trace level to a value independent of "ray history" (except for the current radiosity bounce depth of course),
    // and basically start a new ray from scratch
    ticket.traceLevel           = recSettings.traceLevel;
    ticket.maxAllowedTraceLevel = max(ticket.maxAllowedTraceLevel, ticket.traceLevel + 1);
    ticket.adcBailout           = settings.adcBailout;

    // Since we'll be calculating averages, zero the accumulators
    inverse_distance_sum = 0.0;

    smallest_dist = BOUND_HUGE;

    DBL weight = max(ticket.adcBailout + EPSILON, recSettings.weight);

    // Initialized the accumulators for the integrals which will be come the rad gradient
    sum_of_inverse_dist = sum_of_dist = gradient_count = 0.0;

    unsigned int okCount = 0;
    unsigned int okCountRaw = 0;
    bool use_raw_normal = similar(raw_normal, layer_normal); // if the normal isn't pertubed, go for the raw normal right away because it makes life easier
    double qualitySum = 0.0;
    param.directionGenerator.InitSequence(cur_sample_count, raw_normal, layer_normal, use_raw_normal, brilliance);
    for(unsigned int i = 0, hit = 0; i < cur_sample_count; i++)
    {
        bool ray_ok = param.directionGenerator.GetDirection(direction);
        if (!ray_ok && !use_raw_normal)
        {
            // out of good sample directions, but we may still re-try with the raw normal
            use_raw_normal = true;
            param.directionGenerator.InitSequence(cur_sample_count, raw_normal, layer_normal, use_raw_normal, brilliance);
            ray_ok = param.directionGenerator.GetDirection(direction);
        }
        if (!ray_ok)
            // out of good sample directions, this time really
            break;
        okCount ++;
        if (use_raw_normal) okCountRaw ++;
        ticket.radiosityQuality = 1.0;
        Ray nray(ticket, ipoint, direction, Ray::OtherRay, false, false, true); // Build a ray pointing in the chosen direction
        ticket.radiosityRecursionDepth++;
        ticket.radiosityImportanceQueried = (float)i / (float)(cur_sample_count-1);
        bool alphaBackground = ticket.alphaBackground;
        ticket.alphaBackground = false;
        MathColour temp_full_colour;
        ColourChannel dummyTransm;
        DBL depth = trace.TraceRay(nray, temp_full_colour, dummyTransm, weight, false); // Go down in recursion, trace the result, and come back up
        MathColour temp_colour = temp_full_colour;
        ticket.radiosityRecursionDepth--;
        ticket.alphaBackground = alphaBackground;

        // only post-process the current sample ray if it has the appropriate importance
        if (ticket.radiosityImportanceFound >= ticket.radiosityImportanceQueried)
        {
            DBL quality = ticket.radiosityQuality;
            if (ticket.radiosityImportanceFound < 1.0)
            {
                unsigned int lastI = floor(ticket.radiosityImportanceFound * (cur_sample_count-1));
                quality *= (float)(cur_sample_count) / (float)(lastI+1);
            }

            // NK rad - each sample is limited to a user-specified brightness
            // this is necessary to fix problems splotchiness caused by very
            // bright objects
            // changed lighting.c to ignore phong/specular if tracing radiosity beam
            // TODO FIXME - while the following line is required for backward compatibility,
            //              we might consider replacing .max() with .weight(), .weightMax() or .weightMaxAbs()
            //              in the future
            ColourChannel max_ill = temp_colour.Max();

            if((max_ill > settings.maxSample) && (settings.maxSample > 0.0))
                temp_colour *= (settings.maxSample / max_ill);

            // suppress rays having encountered low-quality radiosity samples
            qualitySum += quality;
            temp_colour *= quality;

#ifdef RAD_GRADIENT
            // Add into illumination gradient integrals
            double deemed_depth = depth;
            if(deemed_depth < maximum_distance * 10.0)
            {
                DBL depth_weight_for_this_gradient = 1.0 / deemed_depth;

                sum_of_inverse_dist += 1.0 / deemed_depth;
                sum_of_dist += deemed_depth;
                gradient_count++;

                dxs += (temp_colour * depth_weight_for_this_gradient * direction[X] * fabs(direction[X]));
                dys += (temp_colour * depth_weight_for_this_gradient * direction[Y] * fabs(direction[Y]));
                dzs += (temp_colour * depth_weight_for_this_gradient * direction[Z] * fabs(direction[Z]));
            }
#endif

            // Add into total illumination integral
            colour_sums += temp_colour;
        }

        // we always get the distance, so we'll use it
        if(depth > HUGE_VAL)
            depth = HUGE_VAL;
        else
        {
#ifdef RADSTATS
            hit++;
#endif
        }

        if(depth < smallest_dist)
        {
            smallest_dist = depth;
            min_dist_vec = direction;
        }
        inverse_distance_sum += 1.0 / depth;

    } // end ray sampling loop

    threadData->Stats()[Radiosity_RayCount] += okCount;
    if (ticket.radiosityRecursionDepth == 0)
        threadData->Stats()[Radiosity_TopLevel_RayCount] += okCount;
    if (isFinalTrace)
        threadData->Stats()[Radiosity_Final_RayCount] += okCount;

    // Use the accumulated values to calculate the averages needed. The sphere
    // of influence of this primary-method sample point is based on the
    // harmonic mean distance to the points encountered. (An harmonic mean is
    // the inverse of the mean of the inverses).
    if (qualitySum == 0)
        illuminance = colour_sums;
    else
        illuminance = colour_sums / qualitySum;

    mean_dist = okCount / inverse_distance_sum;

    // Keep a running total of the final Illuminances we calculated
    if(ticket.radiosityRecursionDepth == 0)
    {
        // TODO FIXME - stats: Gather_Total += illuminance;
        // TODO FIXME - stats: Gather_Total_Count++;
    }

    // We want to cached this block for later reuse.  But,
    // if ground units not big enough, meaning that the value has very
    // limited reuse potential, forget it.
    // [CLi] an exceptionally low distance indicates that we've almost hit two objects at once,
    // so that the sampled rays may be flawed with numeric precision issues
    if(smallest_dist > (maximum_distance * 0.0001)) // TODO FIXME - Should this be similar to RAD_EPSILON? Otherwise select some other *meaningful* constant! [trf]
    {
        // Theory:  We don't want to calculate a primary method ray loop at every
        // point along the inside edges, so a minimum effectivity is practical.
        // It is expressed as a fraction of the distance to the eyepoint.  1/2%
        // is a good number.  This enhancement was Greg Ward's idea, but the use
        // of % units is my idea.  [JDM]

        if(mean_dist < reuse_dist_min)
            mean_dist = reuse_dist_min;
        if(mean_dist > maximum_distance)
            mean_dist = maximum_distance;

#ifdef RADSTATS
        ot_blockcount++; // TODO FIXME - I guess this is duplicate
#endif

#ifdef RAD_GRADIENT
        // beta
        // TODO FIXME - this has gradient kick in abruptly
        if(gradient_count > 10)
        {
            DBL constant_term = gradient_count / (sum_of_inverse_dist * sum_of_dist); // TODO - check validity of this change [trf]

            dxs *= constant_term;
            dys *= constant_term;
            dzs *= constant_term;
        }
        else
        {
            dxs = 0;
            dys = 0;
            dzs = 0;
        }
#endif

        // After end of ray loop, we've decided that this point is worth storing
        // Allocate a block, and fill it with values for reuse in cacheing later

        // TODO CLARIFY - [CLi] not perfectly sure yet when to use raw_normal instead of layer_normal; maybe just interpolate
        unsigned int okCountNonRaw = okCount - okCountRaw;
        bool fileUnderRawNormal = (okCountRaw > okCountNonRaw);
        radiosityCache.AddBlock(cacheBlockPool, &(threadData->Stats()), ipoint, (fileUnderRawNormal ? raw_normal : layer_normal), brilliance, min_dist_vec,
                                dxs, dys, dzs, illuminance, mean_dist, smallest_dist, qualitySum/okCount,
                                ticket.radiosityRecursionDepth, pretraceStep, tileId);
    }
    else
    {
        threadData->Stats()[Radiosity_UnsavedCount]++;
    }

    // Put things back where they were in recursion depth
    ticket.maxAllowedTraceLevel = save_Max_Trace_Level;
    ticket.traceLevel           = save_trace_level;
    ticket.adcBailout           = save_adc_bailout;
    ticket.radiosityQuality     = max(save_radiosityQuality, qualitySum/okCount);

    return qualitySum/okCount;
}

/*****************************************************************************
*
* DESCRIPTION
*    A bit of theory: The goal is to create a set of "random" direction rays
*    so that the probability of close-to-normal versus close-to-tangent rolls
*    off in a cos-theta curve, where theta is the deviation from normal.
*    That is, lots of rays close to normal, and very few close to tangent.
*    You also want to have all of the rays be evenly spread, no matter how
*    many you want to use.  The lookup array has an array of points carefully
*    chosen to meet all of these criteria.
*
******************************************************************************/

RadiosityFunction::SampleDirectionGenerator::SampleDirectionGenerator() :
    rawNormalMode(false),
    rawNormal(0,1,0),
    frameX(1,0,0),
    frameY(0,1,0),
    frameZ(0,0,1)
{}

void RadiosityFunction::SampleDirectionGenerator::Reset(unsigned int samplePoolCount)
{
    if (!sampleDirections)
        sampleDirections = GetSubRandomCosWeightedDirectionGenerator(0, samplePoolCount);
}

void RadiosityFunction::SampleDirectionGenerator::InitSequence(unsigned int& sample_count, const Vector3d& raw_normal, const Vector3d& layer_normal, bool use_raw_normal, DBL br)
{
    size_t sequenceSize = sampleDirections->CycleLength();
    sample_count = (unsigned int)min((size_t)sample_count, sequenceSize);

    if (use_raw_normal)
        // when working with the raw normal, everything should work smooth (and we don't have any fallback solution anyway). No limits.
        remainingDirections = sequenceSize;
    else
        // when working with the pertubed normal, in pathological cases we may want to abort and try with the raw normal instead,
        // so limit the number of tries to something sensible.
        // TODO OPTIMIZE
        //  Is it really possible that we find less than (sample_count) "good" directions among (sample_count*5) directions?
        //  By how much can raw_normal and layer_normal differ? Even at 90 degree tilt, we could expect to find (sample_count)
        //  "good" directions among (sample_count*2).
        remainingDirections = min(((size_t)sample_count) * 5, sequenceSize);

    rawNormalMode = use_raw_normal;
    rawNormal = raw_normal;

    // set up a co-ordinate system to map our pre-computed sampling directions to:
    // - pre-computed "X" will be mapped to some direction we'll call "frameX"
    // - pre-computed "Y" will be mapped to layer_normal ("frameY")
    // - pre-computed "Z" will be mapped to some direction we'll call "frameZ"
    // we choose "frameX" and "frameZ" as follows:
    // - "frameX" to be perpendicular to layer_normal and Z axis
    // - "frameZ" to be perpendicular to layer_normal and "frameX"
    // in case layer_normal and Z axis are uncomfortably close, we fallback to the following choice:
    // - "frameX" to be perpendicular to layer_normal and Y axis
    // - "frameZ" to be perpendicular to layer_normal and "frameX"

    frameY = (use_raw_normal ? raw_normal : layer_normal);
    Vector3d offY;
    if(fabs(frameY[Z]) > 0.9)
        offY = Vector3d(0,1,0); // too close to "Z" for comfort
    else
        offY = Vector3d(0,0,1);
    frameX = cross(frameY, offY).normalized();
    frameZ = cross(frameX, frameY).normalized();

    brilliance = br;
}

bool RadiosityFunction::SampleDirectionGenerator::GetDirection(Vector3d& direction)
{
    if (!remainingDirections)
        // we're out of samples for sure
        return false;

    Vector3d random_vec;
    DBL ray_ok = -1.0;

    // loop through here choosing rays until we get one that is not behind the surface
    // TODO OPTIMIZE
    //  - Checking for almost-exact match with other axes might be beneficial as well, because we could just swap the co-ordinates;
    //    the -Y direction would be the "hottest" candidate again (think roofs); the others might be more common than other directions
    //    as well (think walls or boxes)
    do
    {
        //Increase_Counter(stats[Gather_Performed_Count]);
        random_vec = (*sampleDirections)();

        // Tweak the direction vector according to the brilliance specified.
        random_vec.y() = fabs(random_vec.y());
        if (brilliance != 1.0)
        {
            DBL yOld        =  random_vec.y();
            DBL yOldSqr     =  Sqr(yOld);
            DBL yNewSqr     =  pow(yOldSqr, 2.0/(1.0 + brilliance));
            DBL rOldSqr     =  1.0 - yOldSqr;
            DBL rNewSqr     =  1.0 - yNewSqr;
            DBL rFactor     =  sqrt(rNewSqr/rOldSqr);
            random_vec.x()  *= rFactor;
            random_vec.z()  *= rFactor;
            random_vec.y()  =  sqrt(yNewSqr);
        }

        if(frameY[Y] > 1.0 - RAD_EPSILON)
            // within 2.56 degree of Y, so we'll cheat a bit by using precomputed vectors as-is
            direction = random_vec;
        else if(frameY[Y] < -1.0 + RAD_EPSILON)
            // within 2.56 degree of -Y, so we'll cheat a bit by using precomputed vectors simply inverted
            direction = -random_vec;
        else
            // somewhere else, we need to do some math
            direction = ((frameX * random_vec[X]) + (frameY * random_vec[Y]) + (frameZ * random_vec[Z]));

        if (rawNormalMode)
            ray_ok = 1.0; // no need to check - we know it's good
        else
            ray_ok = dot(direction, rawNormal); // make sure we don't go behind raw_normal
        remainingDirections --;
    }
    while((ray_ok <= 0.0) && (remainingDirections));

    return (ray_ok > 0.0);
}


/*****************************************************************************
*
* FUNCTION  Initialize_Radiosity_Code
*
* INPUT     Nothing.
*
* OUTPUT    Sets various global states used by radiosity.  Notably,
*           ot_fd - the file identifier of the file used to save radiosity values
*
* RETURNS   1 for Success, 0 for failure  (e.g., could not open cache file)
*
* AUTHOUR   Jim McElhiney
*
* DESCRIPTION
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/

RadiosityCache::RadiosityCache(const SceneRadiositySettings& radset) :
    ra_reuse_count(0),
    ra_gather_count(0),
    ot_fd(nullptr),
    Gather_Total_Count(0),
    recursionSettings(radset.GetRecursionSettings(true)) // be prepared for the main render
{
    #ifdef RADSTATS
        ot_seenodecount = 0;
        ot_seeblockcount = 0;
        ot_doblockcount = 0;
        ot_dotokcount = 0;
        ot_lastcount = 0;
        ot_lowerrorcount = 0;
    #endif
}

bool RadiosityCache::Load(const Path& inputFile)
{
    bool ok = false;
    IStream* fd = NewIStream(inputFile, POV_File_Data_RCA);
    if (fd != nullptr)
    {
        BlockPool* pool = AcquireBlockPool();

        bool got_eof;
        int line_num = 0;
        int depth, tx, ty, tz;
        Vector3d point;
        Vector3d normal;
        Vector3d to_nearest;
        MathColour dx, dy, dz;
        MathColour illuminance;
        double harmonic_mean;
        double nearest;
        int goodreads = 0;
        int count;
        DBL brightness;
        char normal_string[30], to_nearest_string[30];
        char line[101];

        //info->Gather_Total.clear();
        //info->Gather_Total_Count = 0;

        while (!(got_eof = !fd->getline (line, 99)))
        {
            switch ( line[0] )
            {
                case 'B':    // the file contains the old radiosity_brightness value
                {
                    if ( sscanf(line, "B%lf\n", &brightness) == 1 )
                    {
                        //info->Brightness = brightness;
                    }
                    break;
                }
                case 'P':    // the file made it to the point that the Preview was done
                {
                    //info->FirstRadiosityPass = true;
                    break;
                }
                case 'C':
                {
#if (NUM_COLOUR_CHANNELS == 3)
                    RGBColour tempCol;
                    count = sscanf(line, "C%d %lf %lf %lf %s %f %f %f %lf %lf %s\n", // tw
                        &depth,
                        &point[X], &point[Y], &point[Z],
                        normal_string,
                        &tempCol.red(), &tempCol.green(), &tempCol.blue(),
                        &harmonic_mean,
                        &nearest, to_nearest_string
                    );
                    illuminance = ToMathColour(tempCol);
#else
                    #error "TODO!"
#endif
                    if ( count == 11 )
                    {
                        depth = depth - 1; // file format still uses 1-based bounce depth counting

                        // normals aren't very critical for direction precision, so they are packed
                        sscanf(normal_string, "%02x%02x%02x", &tx, &ty, &tz);
                        normal[X] = ((double)tx * (1./ 254.))*2.-1.;
                        normal[Y] = ((double)ty * (1./ 254.))*2.-1.;
                        normal[Z] = ((double)tz * (1./ 254.))*2.-1.;
                        normal.normalize();

                        sscanf(to_nearest_string, "%02x%02x%02x", &tx, &ty, &tz);
                        to_nearest[X] = ((double)tx * (1./ 254.))*2.-1.;
                        to_nearest[Y] = ((double)ty * (1./ 254.))*2.-1.;
                        to_nearest[Z] = ((double)tz * (1./ 254.))*2.-1.;
                        to_nearest.normalize();

                        line_num++;

                        AddBlock(pool, nullptr, point, normal, 1.0 /* TODO FIXME - brilliance */, to_nearest, dx, dy, dz, illuminance, harmonic_mean, nearest, 1.0 /* TODO FIXME - quality */, depth, PRETRACE_STEP_LOADED, 0);
                        goodreads++;
                    }
                    break;
                }

                default:
                {
                // wrong leading character on line, just try again on next line
                }

            } // end switch
        } // end while-reading loop

        if ( goodreads > 0 )
            ;// TODO MESSAGE         Debug_Info("Reloaded %d values from radiosity cache file.\n", goodreads);
        else
            ;// TODO MESSAGE         PossibleError("Unable to read any values from the radiosity cache file.");
        ok = true;

        ReleaseBlockPool(pool);

        delete fd;
    }
    return ok;
}

void RadiosityCache::InitAutosave(const Path& outputFile, bool append)
{
    ot_fd = NewOStream(outputFile, POV_File_Data_RCA, append);
}

/*****************************************************************************
*
* FUNCTION  Deinitialize_Radiosity_Code()
*
* INPUT     Nothing.
*
* OUTPUT    Sets various global states used by radiosity.  Notably,
*           ot_fd - the file identifier of the file used to save radiosity values
*
* RETURNS   1 for total success, 0 otherwise (e.g., could not save cache tree)
*
* AUTHOUR   Jim McElhiney
*
* DESCRIPTION
*   Wrap up and free any radiosity-specific features.
*   Note that this function is safe to call even if radiosity was not on.
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/

RadiosityCache::~RadiosityCache()
{
    // TODO FIXME - I guess the mutexing shouldn't be necessary here

    { // mutex scope
#if POV_MULTITHREADED
        std::lock_guard<std::mutex> lock(fileMutex);
#endif
        // finish up cache file
        if (ot_fd != nullptr)
        {
            // close cache file
            delete ot_fd;
            ot_fd = nullptr;
        }
    }

    { // mutex scope
#if POV_MULTITHREADED
        std::lock_guard<std::mutex> lockTree(octree.treeMutex);
        std::lock_guard<std::mutex> lockBlock(octree.blockMutex);
#endif
        if (octree.root != nullptr)
            ot_free_tree(&octree.root);
    }

    { // mutex scope
#if POV_MULTITHREADED
        std::lock_guard<std::mutex> lock(blockPoolsMutex);
#endif
        while (!blockPools.empty())
        {
            delete blockPools.back();
            blockPools.pop_back();
        }
    }

    delete[] recursionSettings;
}


RadiosityCache::BlockPool* RadiosityCache::AcquireBlockPool()
{
#if POV_MULTITHREADED
    std::lock_guard<std::mutex> lock(blockPoolsMutex);
#endif
    if (blockPools.empty())
        return new BlockPool();
    else
    {
        BlockPool* pool = blockPools.back();
        blockPools.pop_back();
        return pool;
    }
}

void RadiosityCache::ReleaseBlockPool(RadiosityCache::BlockPool* pool)
{
    { // mutex scope
#if POV_MULTITHREADED
        std::lock_guard<std::mutex> lock(fileMutex);
#endif
        pool->Save(ot_fd);
    }

    { // mutex scope
#if POV_MULTITHREADED
        std::lock_guard<std::mutex> lock(blockPoolsMutex);
#endif
        blockPools.push_back(pool);
    }
}

ot_block_struct *RadiosityCache::BlockPool::NewBlock()
{
    ot_block_struct *block = nullptr;

    if (head == nullptr || nextFreeBlock >= BLOCK_POOL_UNIT_SIZE)
    {
        head = new PoolUnit(head);
        nextFreeBlock = 0;
    }

    block = &(head->blocks[nextFreeBlock]);

    nextFreeBlock++;

    return block;
}

RadiosityCache::BlockPool::BlockPool() :
    head(nullptr),
    savedHead(nullptr),
    nextFreeBlock(0),
    nextUnsavedBlock(0)
{
    // nothing else to do
}

void RadiosityCache::BlockPool::Save(OStream* fd)
{
    if (fd != nullptr)
    {
        PoolUnit* unit = head;
        while ((unit != nullptr) && (unit != savedHead))
        {
            unsigned int from = 0;
            unsigned int to   = BLOCK_POOL_UNIT_SIZE;
            if (unit->next == savedHead)
                // last unsaved pool unit in chain, maybe already partially saved
                from = nextUnsavedBlock;
            if (unit == head)
                // first pool unit in chain, maybe only partially filled
                to = nextFreeBlock;

            // save current pool unit
            for (int i = from; i < to; i ++)
                ot_write_block(&(unit->blocks[i]), fd);

            unit = unit->next;
        }
    }
    // no else; if we're not writing to a file, still pretend we saved so the destructor doesn't assert

    if (head != nullptr)
    {
        // update the variables indicating how far we have saved
        savedHead = head->next; // the head is incomplete, so it cannot be saved completely...
        nextUnsavedBlock = nextFreeBlock; // ... but all blocks in it so far have been saved.
    }
    else
    {
        POV_RADIOSITY_ASSERT(savedHead == nullptr);
        POV_RADIOSITY_ASSERT(nextUnsavedBlock == 0);
    }
}

RadiosityCache::BlockPool::~BlockPool()
{
    // require that block has been saved by now
    POV_RADIOSITY_ASSERT((head == nullptr) || ((savedHead == head->next) && (nextUnsavedBlock == nextFreeBlock)));

    while (head != nullptr)
    {
        PoolUnit *b = head;
        head = head->next;
        delete b;
    }
}


void RadiosityCache::AddBlock(BlockPool* pool, RenderStatistics* stats, const Vector3d& point, const Vector3d& normal, DBL brilliance, const Vector3d& toNearestSurface,
                              const MathColour& dx, const MathColour& dy, const MathColour& dz, const MathColour& illuminance,
                              DBL harmonicMeanDistance, DBL nearestDistance, DBL quality, int bounceDepth, int pretraceStep, int tileId)
{
    ot_block_struct*    block = pool->NewBlock();
    ot_id_struct        id;
    ot_node_struct*     node;
    const RadiosityRecursionSettings& recSettings = recursionSettings[bounceDepth];

    POV_RADIOSITY_ASSERT((bounceDepth >= 0) && (bounceDepth  <= OT_DEPTH_MAX));
    POV_RADIOSITY_ASSERT(((pretraceStep >= OT_PASS_FIRST) && (pretraceStep <= OT_PASS_MAX)) || (pretraceStep == OT_PASS_FINAL));
    // An overflow in tileId will only impact reproducibility, so we're not asserting on it.

    block->Illuminance = illuminance;
    block->To_Nearest_Surface = toNearestSurface;
#ifdef RAD_GRADIENT
    block->dx = dx;
    block->dy = dy;
    block->dz = dz;
#endif
    block->Brilliance = SNGL(brilliance);
    block->Harmonic_Mean_Distance = SNGL(harmonicMeanDistance);
    block->Nearest_Distance = SNGL(nearestDistance);
    block->Quality = SNGL(quality);
    block->Bounce_Depth = OT_DEPTH(bounceDepth);
    block->Pass = OT_PASS(pretraceStep);
    block->TileId = OT_TILE(tileId);
    block->Point = point;
    block->S_Normal = normal;
    block->next = nullptr;

    // figure out the block id
    ot_index_sphere(point, harmonicMeanDistance * recSettings.octreeAddressFactor, &id);

    // get the corresponding node
    node = RadiosityCache::GetNode(stats, id);

    // add the info block
    InsertBlock(node, block);
}

ot_node_struct *RadiosityCache::GetNode(RenderStatistics* stats, const ot_id_struct& id)
{
    int target_size, dx, dy, dz, index;
    ot_node_struct *temp_node, *this_node, *temp_root;
    ot_id_struct temp_id;

#if POV_MULTITHREADED
    std::unique_lock<std::mutex> treeLock(octree.treeMutex, std::defer_lock); // we may need to lock this mutex - but not now.
#endif

#ifdef RADSTATS
    ot_inscount++;
#endif

    // If there is no root yet, create one.  This is a first-time-through
    if (octree.root == nullptr)
    {
        // now is the time to lock the tree for modification
#if POV_MULTITHREADED
        treeLock.lock();
#endif

        // Now that we have exclusive write access, make sure we REALLY don't have a root
        // (some other thread might have created it just as we were waiting to get the lock)
        if (octree.root == nullptr)
        {
            octree.root = new ot_node_struct;
#ifdef OCTREE_PERFORMANCE_DEBUG
            if (stats != nullptr)
                (*stats)[Radiosity_OctreeNodes]++;
#endif

#ifdef RADSTATS
            ot_nodecount = 1;
#endif

            // Might as well make it the right size for our first data block
            octree.root->Id = id;

            // Having constructed the node to match our needs, we're already in the right place;
            // let's take the shortest route out of here
            return octree.root;
        }
        // no else

        // Still here? Well, fooled by the pitfalls of multithreading, are we!
        // The root is there now, but we didn't create it ourselves, so we need to go the long way

        // As this is an exceptional case (happens at most once per task), we pay the price of releasing
        // and possibly re-acquiring the lock, for the sake of code simplicity
    }
    // no else

    // What if the thing we're inserting is bigger than the biggest node in the
    // existing tree?  Add a new top to the tree till it's big enough.

    if (octree.root->Id.Size < id.Size)
    {
        // now is the time to lock the tree for modification, in case we haven't yet
#if POV_MULTITHREADED
        if (!treeLock.owns_lock())
            treeLock.lock();
#endif

        // (Note that the following can't be a do...while() loop because we may not have had a lock when we first tested,
        // and some other task may have modified the root while we were not looking)
        while (octree.root->Id.Size < id.Size)
        {
            // root too small
            ot_newroot(&octree.root);
        }
    }

    // What if the new block is the right size, but for an area of space which
    // does not overlap with the current tree?  New bigger root, until the
    // areas overlap.

    // Build a temp id, like a cursor to move around with
    temp_id = id;

    // make sure we're using a stable root to work with
    temp_root = octree.root;

    // First, find the parent of our new node which is as big as root
    while (temp_id.Size < temp_root->Id.Size)
    {
        ot_parent(&temp_id, &temp_id);
    }

    if((temp_id.x != temp_root->Id.x) ||
       (temp_id.y != temp_root->Id.y) ||
       (temp_id.z != temp_root->Id.z))
    {
#if POV_MULTITHREADED
        // now is the time to lock the tree for modification, in case we haven't yet
        if (!treeLock.owns_lock())
        {
            treeLock.lock();

            // Acquired the lock just now, so some other task may have changed the root since last time we looked
            while (temp_id.Size < octree.root->Id.Size)
            {
                ot_parent(&temp_id, &temp_id);
            }
        }
#endif

        // (Note that the following can't be a do...while() loop because we may not have had a lock when we first tested,
        // and some other task may have modified the root while we were not looking)
        while((temp_id.x != octree.root->Id.x) ||
              (temp_id.y != octree.root->Id.y) ||
              (temp_id.z != octree.root->Id.z))
        {
            // while separate subtrees...
            ot_newroot(&octree.root);           // create bigger root
            ot_parent(&temp_id, &temp_id);  // and move cursor up one, too
        }
    }

    // At this point, the new node is known to fit under the current tree
    // somewhere.  Go back down the tree to the right level, making new nodes
    // as you go.

    this_node = octree.root; // start at the root

    while (this_node->Id.Size > id.Size)
    {
        // First, pick the node id of the child we are talking about

        target_size = this_node->Id.Size - 1;       // this is the size we want

        temp_id = id;  // start with the new one

        while (temp_id.Size < target_size)
        {
            ot_parent(&temp_id, &temp_id);    // climb up till one below here
        }

        // Now we have to pick which child number we are talking about

        dx = (temp_id.x & 1) * 4;
        dy = (temp_id.y & 1) * 2;
        dz = (temp_id.z & 1);

        index = dx + dy + dz;

        if (this_node->Kids[index] == nullptr)
        {
            // Next level down doesn't exist yet, so create it

#if POV_MULTITHREADED
            // now is the time to lock the tree for modification, in case we haven't yet
            if (!treeLock.owns_lock())
                treeLock.lock();
#endif

            // We may have acquired the lock just now, so some other task may have changed the root since last time we looked
            if (this_node->Kids[index] == nullptr)
            {
                temp_node = new ot_node_struct;
#ifdef OCTREE_PERFORMANCE_DEBUG
                if (stats!= nullptr)
                    (*stats)[Radiosity_OctreeNodes]++;
#endif

#ifdef RADSTATS
                ot_nodecount++;
#endif

                // Fill in the data
                temp_node->Id = temp_id;
                // (all other data fields are automatically zeroed by the allocation function)

                // Add it onto the tree
                this_node->Kids[index] = temp_node;
            }
        }

        // Now follow it down and repeat
        this_node = this_node->Kids[index];
    }

    // Finally, we're in the right place, so return a pointer to the block
    return this_node;
}

void RadiosityCache::InsertBlock(ot_node_struct *node, ot_block_struct *block)
{
#if POV_MULTITHREADED
    std::lock_guard<std::mutex> lock(octree.blockMutex);
#endif

    block->next = node->Values;
    node->Values = block;
}

/*****************************************************************************
*
* FUNCTION
*
*   ra_reuse
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Returns whether or not there were some prestored values close enough to
*   reuse.
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

DBL RadiosityCache::FindReusableBlock(RenderStatistics& stats, DBL errorbound, const Vector3d& ipoint, const Vector3d& snormal, DBL brilliance, MathColour& illuminance, int recursionDepth, int pretraceStep, int tileId)
{
    if (octree.root != nullptr)
    {
        WT_AVG gather;

        gather.Weights = 0.0;

        gather.P = ipoint;
        gather.N = snormal;
        gather.Brilliance = brilliance;

        gather.Weights_Count = 0;
        gather.Good_Count = 0;
        gather.Current_Error_Bound = errorbound;
        gather.Pass = pretraceStep;
        gather.TileId = tileId;

#ifdef OCTREE_PERFORMANCE_DEBUG
        gather.Lookup_Count = 0;
        gather.AcceptPass_Count = 0;
        gather.AcceptQuick_Count = 0;
        gather.AcceptGeometry_Count = 0;
        gather.AcceptNormal_Count = 0;
        gather.AcceptInFront_Count = 0;
        gather.AcceptEpsilon_Count = 0;
#endif

        // Go through the tree calculating a weighted average of all of the usable points near this one
        // [CLi] inspection of octree.cpp tree code indicates that tree traversal is perfectly safe
        // regarding insertions by other threads, so no locking is needed
        ot_dist_traverse(octree.root, ipoint, recursionDepth, AverageNearBlock, reinterpret_cast<void *>(&gather));

#ifdef OCTREE_PERFORMANCE_DEBUG
        stats[Radiosity_OctreeLookups]  += gather.Lookup_Count;
        stats[Radiosity_OctreeAccepts0] += gather.AcceptPass_Count;
        stats[Radiosity_OctreeAccepts1] += gather.AcceptQuick_Count;
        stats[Radiosity_OctreeAccepts2] += gather.AcceptGeometry_Count;
        stats[Radiosity_OctreeAccepts3] += gather.AcceptNormal_Count;
        stats[Radiosity_OctreeAccepts4] += gather.AcceptInFront_Count;
        stats[Radiosity_OctreeAccepts5] += gather.AcceptEpsilon_Count;
#endif

        // Did we get any nearby points we could reuse?
        if(gather.Weights > 0)
        {
            // NK rad - Average together all of the samples (sums were returned by
            // ot_dist_traverse).  We are using nearest_count as a lower bound,
            // not an upper bound.
            illuminance = gather.Weights_Times_Illuminances / gather.Weights;
        }

        return gather.Weights;
    }
    else
    {
        return 0; // No tree, so no reused values
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   ra_average_near
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Tree traversal function used by ra_reuse()
*   Calculate the weight of this cached value, taking into account how far
*   it is from our test point, and the difference in surface normal angles.
*
*   Given a node with an old cached value, check to see if it is reusable, and
*   aggregate its info into the weighted average being built during the tree
*   traversal. block contains Point, Normal, Illuminance,
*   Harmonic_Mean_Distance
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

bool RadiosityCache::AverageNearBlock(ot_block_struct *block, void *void_info)
{
    WT_AVG *info = reinterpret_cast<WT_AVG *>(void_info);

#ifdef OCTREE_PERFORMANCE_DEBUG
    info->Lookup_Count ++;
#endif

    // for the sake of reproducibility, do not use samples gathered during the same pass in other tiles
    if ((block->Pass == info->Pass) && (block->TileId != info->TileId))
        return true; // we always return true

    // do not use samples gathered for a different surface brilliance
    if (fabs(block->Brilliance - info->Brilliance) > BRILLIANCE_EPSILON) // TODO FIXME - make this a relative value
        return true; // we always return true

    Vector3d delta(info->P - block->Point);   // a = b - c, which is test p minus old pt
    DBL square_dist = delta.lengthSqr();
    DBL quickcheck_rad = (DBL)block->Harmonic_Mean_Distance * info->Current_Error_Bound;

#ifdef RADSTATS
    ot_doblockcount++;
#endif

#ifdef OCTREE_PERFORMANCE_DEBUG
    info->AcceptPass_Count ++;
#endif

    // first we do a tuning test--this func gets called a LOT
    if(square_dist < (quickcheck_rad * quickcheck_rad))
    {
#ifdef OCTREE_PERFORMANCE_DEBUG
        info->AcceptQuick_Count ++;
#endif

        DBL dist = sqrt(square_dist);
        DBL ri = (DBL)block->Harmonic_Mean_Distance;
        bool dist_greater_epsilon = (dist > AVG_NEAR_EPSILON);
        Vector3d delta_unit;

        if(dist_greater_epsilon == true)
        {
            delta_unit = delta / dist; // normalise

            // This block reduces the radius of influence when it points near the nearest
            // surface found during sampling.
            // TODO FIXME
            //  This is a good idea, but what if there are multiple objects that close?
            //  This is probably what leads to light seeping through walls at corners.
            //  Maybe a well-chosen mean (arithmetic? geometric? harmonic?) of all the sample vectors
            //  will give us a better idea what directions to be careful about.
            DBL cos_diff_from_nearest = dot(block->To_Nearest_Surface, delta_unit);
            if(cos_diff_from_nearest > 0.0)
                ri = (cos_diff_from_nearest * (DBL)block->Nearest_Distance) + ((1.0 - cos_diff_from_nearest) * ri);
        }

        if(dist < (ri * info->Current_Error_Bound))
        {
#ifdef OCTREE_PERFORMANCE_DEBUG
            info->AcceptGeometry_Count ++;
#endif

            DBL dir_diff = dot(info->N, block->S_Normal);

            // NB error_reuse varies from 0 to 3.82 (1+ 2 root 2)
            DBL error_reuse_translate = dist / ri;
            DBL error_reuse_rotate = 2.0 * sqrt(fabs(1.0 - dir_diff));
            DBL error_reuse = error_reuse_translate + error_reuse_rotate;

            // is this old point within a reasonable error distance?
            if(error_reuse < info->Current_Error_Bound)
            {
#ifdef OCTREE_PERFORMANCE_DEBUG
                info->AcceptNormal_Count ++;
#endif

                DBL in_front = 1.0;

#ifdef RADSTATS
                ot_lowerrorcount++;
#endif

                // TODO
                //  The test for "in-front" points, as described by Greg Ward et al.,
                //  seems to be problematic in practice; can a better solution be found
                //  to address the "potentially shadowed" issue?

                if(dist_greater_epsilon == true)
                {
                    // Make sure that the old point is not in front of this point, the
                    // old surface might shadow this point and make the result  meaningless
                    Vector3d half(info->N + block->S_Normal);

                    // [CLi] the following statement is equivalent to normalizing "half", then computing the dot product with "delta_unit",
                    // making sure that in_front is in the range of -1..1:
                    in_front = dot(delta_unit, half) / half.length();
                }

                // Theory:        eliminate the use of old points well in front of our
                // new point we are calculating, but not ones which are just a little
                // tiny bit in front.  This (usually) avoids eliminating points on the
                // same surface by accident.

                if(in_front > IN_FRONT_LIMIT)
                {
#ifdef OCTREE_PERFORMANCE_DEBUG
                    info->AcceptInFront_Count ++;
#endif

                    DBL weight;

#ifdef RADSTATS
                    ot_dotokcount++;
#endif

                    if(info->Pass != RadiosityFunction::FINAL_TRACE || block->Bounce_Depth > 0)
                    {
                        // this is not final trace recursion 0, so a simple averaging method will do - use linear averaging.
                        weight = 1.0 - (error_reuse / info->Current_Error_Bound); // 0 < t < 1
                    }
                    else
                    {

                        // this is final trace recursion 0, so we want a nice and smooth averaging.
#ifdef SIGMOID_METHOD
                        weight = error_reuse / info->Current_Error_Bound;  // 0 < t < 1
                        weight = (cos(weight * M_PI) + 1.0) * 0.5;         // 0 < w < 1
#endif

#ifdef PSEUDO_SIGMOID_METHOD
                        weight = error_reuse / info->Current_Error_Bound;  // 0 < t < 1
                        if (weight < 0.5)
                            weight = 1.0 - Sqr(weight*2.0)/2.0;
                        else
                            weight = Sqr(( 1.0-weight )*2.0)/2.0;
#endif

#ifdef SAW_METHOD
                        weight = 1.0 - (error_reuse / info->Current_Error_Bound); // 0 < t < 1
#ifdef SAW_METHOD_ROOT
#if (SAW_METHOD_ROOT == 1)
                        // no modification
#elif (SAW_METHOD_ROOT == 2)
                        weight = sqrt(weight);
#elif (SAW_METHOD_ROOT == 4)
                        // TODO OPTIMIZE - maybe pow(weight,1.0/4) is more efficient here
                        weight = sqrt(sqrt(weight));  // less splotchy
#elif (SAW_METHOD_ROOT == 8)
                        // TODO OPTIMIZE - maybe pow(weight,1.0/8) is more efficient here
                        weight = sqrt(sqrt(sqrt(weight)));   // maybe even less splotchy
#else
                        weight = pow(weight, 1.0/SAW_METHOD_ROOT);
#endif
#endif
                        //weight = weight*weight*weight*weight*weight;  more splotchy
#endif
                    }

                    if (in_front <= 0) // avoid hard break at in_front value of -0.05
                    {
                        DBL in_front_weight = 1 - (in_front / IN_FRONT_LIMIT); // [IN_FRONT_LIMIT..0] -> [0..1]
                        weight = weight * in_front_weight;
                    }

                    if(weight > RAD_EPSILON) // avoid floating point oddities near zero
                    {
#ifdef OCTREE_PERFORMANCE_DEBUG
                        info->AcceptEpsilon_Count ++;
#endif

                        // This is the block where we use the gradient to improve the prediction
#ifdef RAD_GRADIENT
                        MathColour d((block->dx * delta[X]) + (block->dy * delta[Y]) + (block->dz * delta[Z]));

                        // NK 6-May-2003 removed clipping - not sure why it was here in the
                        // first place, but it sure causes problems for HDR scenes, and removing
                        // it doesn't seem to cause problems for non-HRD scenes.
                        // But we want to make sure that our deltas don't cause a positive illumination
                        // to go below zero, while allowing negative illuminations to stay negative.
                        if((d.red() + block->Illuminance.red() < 0.0) && (block->Illuminance.red()>  0.0))
                            d.red() = -block->Illuminance.red();

                        if((d.green() + block->Illuminance.green() < 0.0) && (block->Illuminance.green() > 0.0))
                            d.green() = -block->Illuminance.green();

                        if((d.blue() + block->Illuminance.blue() < 0.0) && (block->Illuminance.blue() > 0.0))
                            d.blue() = -block->Illuminance.blue();

                        MathColour prediction = block->Illuminance + d;
#else
                        MathColour prediction = block->Illuminance;
#endif

#ifdef SHOW_SAMPLE_SPOTS
                        // TODO FIXME - distance_maximum no longer exists
                        if(dist < radset.Dist_Max * 0.015)
                            prediction.set(3.0);
#endif

                        weight *= block->Quality;

                        // The predicted colour is an extrapolation based on the old value
                        info->Weights_Times_Illuminances += (prediction * weight);

                        info->Weights += weight;
                        info->Weights_Count++;
                        info->Good_Count++;

                        // NK rad - it fit in the error bound, so keep it.  We use all
                        // that fit the error bounding criteria.  There is no need to put
                        // a maximum on the number of samples that are averaged.
                    }
                }
            }
        }
    }

    return true;
}

}
// end of namespace pov
