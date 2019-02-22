//******************************************************************************
///
/// @file core/render/trace.cpp
///
/// Implementations related to the @ref pov::Trace class.
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
#include "core/render/trace.h"

// C++ variants of C standard header files
#include <cfloat>

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
#include "base/povassert.h"

// POV-Ray header files (core module)
#include "core/bounding/bsptree.h"
#include "core/lighting/lightsource.h"
#include "core/lighting/radiosity.h"
#include "core/lighting/subsurface.h"
#include "core/material/interior.h"
#include "core/material/noise.h"
#include "core/material/normal.h"
#include "core/material/pattern.h"
#include "core/material/pigment.h"
#include "core/material/texture.h"
#include "core/material/warp.h"
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/atmosphere.h"
#include "core/scene/object.h"
#include "core/scene/scenedata.h"
#include "core/scene/tracethreaddata.h"
#include "core/shape/box.h"
#include "core/shape/csg.h"
#include "core/support/imageutil.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::min;
using std::max;
using std::vector;

#define SHADOW_TOLERANCE 1.0e-3


bool NoSomethingFlagRayObjectCondition::operator()(const Ray& ray, ConstObjectPtr object, double) const
{
    if(ray.IsImageRay() && Test_Flag(object, NO_IMAGE_FLAG))
        return false;
    if(ray.IsReflectionRay() && Test_Flag(object, NO_REFLECTION_FLAG))
        return false;
    if(ray.IsRadiosityRay() && Test_Flag(object, NO_RADIOSITY_FLAG))
        return false;
    if(ray.IsPhotonRay() && Test_Flag(object, NO_SHADOW_FLAG))
        return false;
    return true;
}

Trace::Trace(std::shared_ptr<SceneData> sd, TraceThreadData *td, const QualityFlags& qf,
             CooperateFunctor& cf, MediaFunctor& mf, RadiosityFunctor& rf) :
    threadData(td),
    sceneData(sd),
    maxFoundTraceLevel(0),
    qualityFlags(qf),
    mailbox(0),
    crandRandomNumberGenerator(0),
    randomNumbers(0.0, 1.0, 32768),
    randomNumberGenerator(&randomNumbers),
    ssltUniformDirectionGenerator(),
    ssltUniformNumberGenerator(),
    ssltCosWeightedDirectionGenerator(),
    cooperate(cf),
    media(mf),
    radiosity(rf),
    lightColorCacheIndex(-1)
{
    lightSourceLevel1ShadowCache.resize(max(1, (int) threadData->lightSources.size()));
    for(vector<ObjectPtr>::iterator i(lightSourceLevel1ShadowCache.begin()); i != lightSourceLevel1ShadowCache.end(); i++)
        *i = nullptr;

    lightSourceOtherShadowCache.resize(max(1, (int) threadData->lightSources.size()));
    for(vector<ObjectPtr>::iterator i(lightSourceOtherShadowCache.begin()); i != lightSourceOtherShadowCache.end(); i++)
        *i = nullptr;

    lightColorCache.resize(max(20U, sd->parsedMaxTraceLevel + 1));
    for(LightColorCacheListList::iterator it = lightColorCache.begin(); it != lightColorCache.end(); it++)
        it->resize(max(1, (int) threadData->lightSources.size()));

    if(sceneData->boundingMethod == 2)
        mailbox = BSPTree::Mailbox(sceneData->numberOfFiniteObjects);
}

Trace::~Trace()
{
}

double Trace::TraceRay(Ray& ray, MathColour& colour, ColourChannel& transm, COLC weight, bool continuedRay, DBL maxDepth)
{
    Intersection bestisect;
    bool found;
    NoSomethingFlagRayObjectCondition precond;
    TrueRayObjectCondition postcond;

    POV_ULONG nrays = threadData->Stats()[Number_Of_Rays]++;
    if(ray.IsPrimaryRay() || (((unsigned char) nrays & 0x0f) == 0x00))
        cooperate();

    // Check for max. trace level or ADC bailout.
    if((ray.GetTicket().traceLevel >= ray.GetTicket().maxAllowedTraceLevel) || (weight < ray.GetTicket().adcBailout))
    {
        if(weight < ray.GetTicket().adcBailout)
            threadData->Stats()[ADC_Saves]++;

        colour.Clear();
        transm = 0.0;
        return HUGE_VAL;
    }

    if (maxDepth >= EPSILON)
        bestisect.Depth = maxDepth;

    found = FindIntersection(bestisect, ray, precond, postcond);

    // Check if we're busy shooting too many radiosity sample rays at an unimportant object
    if (ray.GetTicket().radiosityImportanceQueried >= 0.0)
    {
        if (found && bestisect.Object->RadiosityImportanceSet)
            ray.GetTicket().radiosityImportanceFound = bestisect.Object->RadiosityImportance;
        else
            ray.GetTicket().radiosityImportanceFound = sceneData->radiositySettings.defaultImportance;

        if (ray.GetTicket().radiosityImportanceFound < ray.GetTicket().radiosityImportanceQueried)
        {
            if(found == false)
                return HUGE_VAL;
            else
                return bestisect.Depth;
        }
    }
    float oldRadiosityImportanceQueried = ray.GetTicket().radiosityImportanceQueried;
    ray.GetTicket().radiosityImportanceQueried = -1.0; // indicates that recursive calls to TraceRay() should not check for radiosity importance

    const bool traceLevelIncremented = !continuedRay;

    if(traceLevelIncremented)
    {
        // Set highest level traced.
        ray.GetTicket().traceLevel++;
        ray.GetTicket().maxFoundTraceLevel = (unsigned int) max(ray.GetTicket().maxFoundTraceLevel, ray.GetTicket().traceLevel);
    }

    if(qualityFlags.media && (ray.IsPhotonRay() == true) && (ray.IsHollowRay() == true))
    {
        // Note: this version of ComputeMedia does not deposit photons. This is
        // intentional.  Even though we're processing a photon ray, we don't want
        // to deposit photons in the infinite atmosphere, only in contained
        // media, which is processed later (in ComputeLightedTexture).  [nk]
        media.ComputeMedia(sceneData->atmosphere, ray, bestisect, colour, transm);

        if (sceneData->fog != nullptr)
            ComputeFog(ray, bestisect, colour, transm);
    }

    if(found)
        ComputeTextureColour(bestisect, colour, transm, ray, weight, false);
    else
        ComputeSky(ray, colour, transm);

    if(qualityFlags.media && (ray.IsPhotonRay() == false) && (ray.IsHollowRay() == true))
    {
        if ((sceneData->rainbow != nullptr) && (ray.IsShadowTestRay() == false))
            ComputeRainbow(ray, bestisect, colour, transm);

        media.ComputeMedia(sceneData->atmosphere, ray, bestisect, colour, transm);

        if (sceneData->fog != nullptr)
            ComputeFog(ray, bestisect, colour, transm);
    }

    if(traceLevelIncremented)
        ray.GetTicket().traceLevel--;
    maxFoundTraceLevel = (unsigned int) max(maxFoundTraceLevel, ray.GetTicket().maxFoundTraceLevel);

    ray.GetTicket().radiosityImportanceQueried = oldRadiosityImportanceQueried;

    if(found == false)
        return HUGE_VAL;
    else
        return bestisect.Depth;
}

bool Trace::FindIntersection(Intersection& bestisect, const Ray& ray)
{
    switch(sceneData->boundingMethod)
    {
        case 2:
        {
            BSPIntersectFunctor ifn(bestisect, ray, sceneData->objects, threadData);
            bool found = false;

            mailbox.clear();

            found = (*(sceneData->tree))(ray, ifn, mailbox, bestisect.Depth);

            // test infinite objects
            for(vector<ObjectPtr>::iterator it = sceneData->objects.begin() + sceneData->numberOfFiniteObjects; it != sceneData->objects.end(); it++)
            {
                Intersection isect;

                if(FindIntersection(*it, isect, ray) && (isect.Depth < bestisect.Depth))
                {
                    bestisect = isect;
                    found = true;
                }
            }

            return found;
        }
        case 1:
        {
            if (sceneData->boundingSlabs != nullptr)
                return (Intersect_BBox_Tree(priorityQueue, sceneData->boundingSlabs, ray, &bestisect, threadData));
        }
        // FALLTHROUGH
        case 0:
        {
            bool found = false;

            for(vector<ObjectPtr>::iterator it = sceneData->objects.begin(); it != sceneData->objects.end(); it++)
            {
                Intersection isect;

                if(FindIntersection(*it, isect, ray) && (isect.Depth < bestisect.Depth))
                {
                    bestisect = isect;
                    found = true;
                }
            }

            return found;
        }
    }

    return false;
}

bool Trace::FindIntersection(Intersection& bestisect, const Ray& ray, const RayObjectCondition& precondition, const RayObjectCondition& postcondition)
{
    switch(sceneData->boundingMethod)
    {
        case 2:
        {
            BSPIntersectCondFunctor ifn(bestisect, ray, sceneData->objects, threadData, precondition, postcondition);
            bool found = false;

            mailbox.clear();

            found = (*(sceneData->tree))(ray, ifn, mailbox, bestisect.Depth);

            // test infinite objects
            for(vector<ObjectPtr>::iterator it = sceneData->objects.begin() + sceneData->numberOfFiniteObjects; it != sceneData->objects.end(); it++)
            {
                if(precondition(ray, *it, 0.0) == true)
                {
                    Intersection isect;

                    if(FindIntersection(*it, isect, ray, postcondition) && (isect.Depth < bestisect.Depth))
                    {
                        bestisect = isect;
                        found = true;
                    }
                }
            }

            return found;
        }
        case 1:
        {
            if (sceneData->boundingSlabs != nullptr)
                return (Intersect_BBox_Tree(priorityQueue, sceneData->boundingSlabs, ray, &bestisect, precondition, postcondition, threadData));
        }
        // FALLTHROUGH
        case 0:
        {
            bool found = false;

            for(vector<ObjectPtr>::iterator it = sceneData->objects.begin(); it != sceneData->objects.end(); it++)
            {
                if(precondition(ray, *it, 0.0) == true)
                {
                    Intersection isect;

                    if(FindIntersection(*it, isect, ray, postcondition) && (isect.Depth < bestisect.Depth))
                    {
                        bestisect = isect;
                        found = true;
                    }
                }
            }

            return found;
        }
    }

    return false;
}

bool Trace::FindIntersection(ObjectPtr object, Intersection& isect, const Ray& ray, double closest)
{
    if (object != nullptr)
    {
        BBoxVector3d origin;
        BBoxVector3d invdir;
        BBoxDirection variant;

        Vector3d tmp(1.0 / ray.GetDirection()[X], 1.0 / ray.GetDirection()[Y], 1.0 /ray.GetDirection()[Z]);
        origin = BBoxVector3d(ray.Origin);
        invdir = BBoxVector3d(tmp);
        variant = (BBoxDirection)((int(invdir[X] < 0.0) << 2) | (int(invdir[Y] < 0.0) << 1) | int(invdir[Z] < 0.0));

        if(object->Intersect_BBox(variant, origin, invdir, closest) == false)
            return false;

        if(object->Bound.empty() == false)
        {
            if(Ray_In_Bound(ray, object->Bound, threadData) == false)
                return false;
        }

        IStack depthstack(stackPool);
        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

        if(object->All_Intersections(ray, depthstack, threadData))
        {
            bool found = false;
            double tmpDepth = 0;

            while(depthstack->size() > 0)
            {
                tmpDepth = depthstack->top().Depth;
                // TODO FIXME - This was SMALL_TOLERANCE, but that's too rough for some scenes [cjc] need to check what it was in the old code [trf]
                if(tmpDepth < closest && (ray.IsSubsurfaceRay() || tmpDepth >= MIN_ISECT_DEPTH))
                {
                    isect = depthstack->top();
                    closest = tmpDepth;
                    found = true;
                }

                depthstack->pop();
            }

            return (found == true);
        }

        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack is in a cleaned-up condition (again)
    }

    return false;
}

bool Trace::FindIntersection(ObjectPtr object, Intersection& isect, const Ray& ray, const RayObjectCondition& postcondition, double closest)
{
    if (object != nullptr)
    {
        BBoxVector3d origin;
        BBoxVector3d invdir;
        BBoxDirection variant;

        Vector3d tmp(1.0 / ray.GetDirection()[X], 1.0 / ray.GetDirection()[Y], 1.0 /ray.GetDirection()[Z]);
        origin = BBoxVector3d(ray.Origin);
        invdir = BBoxVector3d(tmp);
        variant = (BBoxDirection)((int(invdir[X] < 0.0) << 2) | (int(invdir[Y] < 0.0) << 1) | int(invdir[Z] < 0.0));

        if(object->Intersect_BBox(variant, origin, invdir, closest) == false)
            return false;

        if(object->Bound.empty() == false)
        {
            if(Ray_In_Bound(ray, object->Bound, threadData) == false)
                return false;
        }

        IStack depthstack(stackPool);
        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

        if(object->All_Intersections(ray, depthstack, threadData))
        {
            bool found = false;
            double tmpDepth = 0;

            while(depthstack->size() > 0)
            {
                tmpDepth = depthstack->top().Depth;
                // TODO FIXME - This was SMALL_TOLERANCE, but that's too rough for some scenes [cjc] need to check what it was in the old code [trf]
                if(tmpDepth < closest && (ray.IsSubsurfaceRay() || tmpDepth >= MIN_ISECT_DEPTH) && postcondition(ray, object, tmpDepth))
                {
                    isect = depthstack->top();
                    closest = tmpDepth;
                    found = true;
                }

                depthstack->pop();
            }

            return (found == true);
        }

        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack is in a cleaned-up condition (again)
    }

    return false;
}

unsigned int Trace::GetHighestTraceLevel()
{
    return maxFoundTraceLevel;
}

void Trace::ComputeTextureColour(Intersection& isect, MathColour& colour, ColourChannel& transm, Ray& ray, COLC weight, bool photonPass)
{
    // NOTE: when called during the photon pass this method is used to deposit photons
    // on the surface and not, per se, to compute texture color.
    WeightedTextureVector wtextures;
    double normaldirection;
    MathColour tmpCol;
    ColourChannel tmpTransm = 0.0;
    MathColour c1;
    ColourChannel t1 = 0.0;
    Vector2d uvcoords;
    Vector3d rawnormal;
    Vector3d ipoint(isect.IPoint);

    if (++lightColorCacheIndex >= lightColorCache.size())
    {
        lightColorCache.resize(lightColorCacheIndex + 10);
        for (LightColorCacheListList::iterator it = lightColorCache.begin() + lightColorCacheIndex; it != lightColorCache.end(); it++)
            it->resize(lightColorCache[0].size());
    }
    for (LightColorCacheList::iterator it = lightColorCache[lightColorCacheIndex].begin(); it != lightColorCache[lightColorCacheIndex].end(); it++)
        it->tested = false;

    // compute the surface normal
    isect.Object->Normal(rawnormal, &isect, threadData);

    // I added this to flip the normal if the object is inverted (for CSG).
    // However, I subsequently commented it out for speed reasons - it doesn't
    // make a difference (no pun intended). The preexisting flip code below
    // produces a similar (though more extensive) result. [NK]
    // Actually, we should keep this code to guarantee that Normal_Direction
    // is set properly. [NK]
    if(Test_Flag(isect.Object, INVERTED_FLAG))
        rawnormal.invert();

    // if the surface normal points away, flip its direction
    normaldirection = dot(rawnormal, ray.Direction);
    if(normaldirection > 0.0)
        rawnormal.invert();

    isect.INormal = rawnormal;
    isect.PNormal = rawnormal;

    // now switch to UV mapping if we need to
    if(Test_Flag(isect.Object, UV_FLAG))
    {
        // TODO FIXME
        //  I think we have a serious problem here regarding bump mapping:
        //  The UV vector doesn't contain any information about the (local) *orientation* of U and V in our XYZ co-ordinate system!
        //  This causes slopes do be applied in the wrong directions.

        // get the UV vect of the intersection
        isect.Object->UVCoord(uvcoords, &isect);
        // save the normal and UV coords into Intersection
        isect.Iuv = uvcoords;

        ipoint = Vector3d(uvcoords.u(), uvcoords.v(), 0.0);
    }

    bool isMultiTextured = Test_Flag(isect.Object, MULTITEXTURE_FLAG) ||
                           ((isect.Object->Texture == nullptr) && Test_Flag(isect.Object, CUTAWAY_TEXTURES_FLAG));

    // get textures and weights
    if(isMultiTextured == true)
    {
        isect.Object->Determine_Textures(&isect, normaldirection > 0.0, wtextures, threadData);
    }
    else if (isect.Object->Texture != nullptr)
    {
        if ((normaldirection > 0.0) && (isect.Object->Interior_Texture != nullptr))
            wtextures.push_back(WeightedTexture(1.0, isect.Object->Interior_Texture)); /* Chris Huff: Interior Texture patch */
        else
            wtextures.push_back(WeightedTexture(1.0, isect.Object->Texture));
    }
    else
    {
        // don't need to do anything as the texture list will be empty.
        // TODO: could we perform these tests earlier ? [cjc]
        lightColorCacheIndex--;
        return;
    }

    // Now, we perform the lighting calculations by stepping through
    // the list of textures and summing the weighted color.

    for(WeightedTextureVector::iterator i(wtextures.begin()); i != wtextures.end(); i++)
    {
        TextureVector warps(texturePool);
        POV_REFPOOL_ASSERT(warps->empty()); // verify that the TextureVector pulled from the pool is in a cleaned-up condition

        // if the contribution of this texture is negligible skip ahead
        if ((i->weight < ray.GetTicket().adcBailout) || (i->texture == nullptr))
            continue;

        if(photonPass == true)
        {
            // For the photon pass, colour (and thus c1) represents the
            // light energy being transmitted by the photon.  Because of this, we
            // compute the weighted energy value, then pass it to the texture for
            // processing.
            c1 = colour * i->weight;

            // NOTE that ComputeOneTextureColor is being used for a secondary purpose, and
            // that to place photons on the surface and trigger recursive photon shooting
            ComputeOneTextureColour(c1, t1, i->texture, *warps, ipoint, rawnormal, ray, weight, isect, false, true);
        }
        else
        {
            ComputeOneTextureColour(c1, t1, i->texture, *warps, ipoint, rawnormal, ray, weight, isect, false, false);

            tmpCol    += i->weight * c1;
            tmpTransm += i->weight * t1;
        }
    }

    // [CLi] moved this here from Trace::ComputeShadowTexture() and Trace::ComputeLightedTexture(), respectively,
    // to avoid media to be computed twice when dealing with averaged textures.
    // TODO - For photon rays we're still potentially doing double work on media.
    // TODO - For shadow rays we're still potentially doing double work on distance-based attenuation.
    // Calculate participating media effects.
    if(!photonPass && qualityFlags.media && (!ray.GetInteriors().empty()) && (ray.IsHollowRay() == true))
    {
        media.ComputeMedia(ray.GetInteriors(), ray, isect, tmpCol, tmpTransm);
    }

    colour += tmpCol;
    transm += tmpTransm;

    lightColorCacheIndex--;
}

void Trace::ComputeOneTextureColour(MathColour& resultColour, ColourChannel& resultTransm, const TEXTURE *texture, vector<const TEXTURE *>& warps, const Vector3d& ipoint,
                                    const Vector3d& rawnormal, Ray& ray, COLC weight, Intersection& isect, bool shadowflag, bool photonPass)
{
    // NOTE: this method is used by the photon pass to deposit photons on the surface
    // (and not, per se, to compute texture color)
    const TextureBlendMapPtr& blendmap = texture->Blend_Map;
    const TextureBlendMapEntry *prev, *cur;
    DBL prevWeight, curWeight;
    double value1; // TODO FIXME - choose better name!
    Vector3d tpoint;
    Vector2d uvcoords;
    MathColour c2;
    ColourChannel t2;

    switch(texture->Type)
    {
        case NO_PATTERN:
        case PLAIN_PATTERN:
            break;
        case AVERAGE_PATTERN:
        case UV_MAP_PATTERN:
        case BITMAP_PATTERN:
        default:
            warps.push_back(texture);
            break;
    }

    // ipoint - interseciton point (and evaluation point)
    // epoint - evaluation point
    // tpoint - turbulated/transformed point

    if(texture->Type <= LAST_SPECIAL_PATTERN)
    {
        switch(texture->Type)
        {
            case NO_PATTERN:
                POV_PATTERN_ASSERT(false);  // in Create_Texture(), TEXTURE->Type is explicitly set to PLAIN_PATTERN (in deviation
                                            // from the default TPat settings), and there is no piece of code ever setting it to
                                            // NO_PATTERN (except during parsing of magnet patterns, but that code makes sure it
                                            // doesn't remain set to NO_PATTERN).
                resultColour.Clear();
                resultTransm = 1.0;
                break;
            case AVERAGE_PATTERN:
                Warp_EPoint(tpoint, ipoint, warps.back());
                ComputeAverageTextureColours(resultColour, resultTransm, texture, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass);
                break;
            case UV_MAP_PATTERN:
                // TODO FIXME
                //  I think we have a serious problem here regarding bump mapping:
                //  The UV vector doesn't contain any information about the (local) *orientation* of U and V in our XYZ co-ordinate system!
                //  This causes slopes do be applied in the wrong directions.

                // Don't bother warping, simply get the UV vect of the intersection
                isect.Object->UVCoord(uvcoords, &isect);
                tpoint = Vector3d(uvcoords[U], uvcoords[V], 0.0);
                cur = &(texture->Blend_Map->Blend_Map_Entries[0]);
                ComputeOneTextureColour(resultColour, resultTransm, cur->Vals, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass);
                break;
            case BITMAP_PATTERN:
                Warp_EPoint(tpoint, ipoint, texture);
                ComputeOneTextureColour(resultColour, resultTransm, material_map(tpoint, texture), warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass);
                break;
            case PLAIN_PATTERN:
                if(shadowflag == true)
                    ComputeShadowTexture(resultColour, texture, warps, ipoint, rawnormal, ray, isect);
                    // NB: filter and transmit components are ignored by the caller when tracing shadow rays, so no need to set them
                else
                    ComputeLightedTexture(resultColour, resultTransm, texture, warps, ipoint, rawnormal, ray, weight, isect);
                break;
            default:
                throw POV_EXCEPTION_STRING("Bad texture type in ComputeOneTextureColour");
        }
    }
    else
    {
        // NK 19 Nov 1999 added Warp_EPoint
        Warp_EPoint(tpoint, ipoint, texture);
        value1 = Evaluate_TPat(texture, tpoint, &isect, &ray, threadData);

        blendmap->Search(value1, prev, cur, prevWeight, curWeight);

        // NK phmap
        if(photonPass)
        {
            if(prev == cur)
                ComputeOneTextureColour(resultColour, resultTransm, cur->Vals, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass);
            else
            {
                c2 = resultColour * curWeight;
                ComputeOneTextureColour(c2, t2, cur->Vals, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass);
                c2 = resultColour * prevWeight; // modifies RGB, but leaves Filter and Transmit unchanged
                ComputeOneTextureColour(c2, t2, prev->Vals, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass);
            }
        }
        else
        {
            ComputeOneTextureColour(resultColour, resultTransm, cur->Vals, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass);

            if(prev != cur)
            {
                ComputeOneTextureColour(c2, t2, prev->Vals, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass);
                resultColour = curWeight * resultColour + prevWeight * c2;
                resultTransm = curWeight * resultTransm + prevWeight * t2;
            }
        }
    }
}

void Trace::ComputeAverageTextureColours(MathColour& resultColour, ColourChannel& resultTransm, const TEXTURE *texture, vector<const TEXTURE *>& warps, const Vector3d& ipoint,
                                         const Vector3d& rawnormal, Ray& ray, COLC weight, Intersection& isect, bool shadowflag, bool photonPass)
{
    const TextureBlendMapPtr& bmap = texture->Blend_Map;
    SNGL total = 0.0;
    MathColour lc;
    ColourChannel lt;

    if(photonPass == false)
    {
        resultColour.Clear();
        resultTransm = 0.0;

        for(vector<TextureBlendMapEntry>::const_iterator i = bmap->Blend_Map_Entries.begin(); i != bmap->Blend_Map_Entries.end(); i++)
        {
            SNGL val = i->value;

            ComputeOneTextureColour(lc, lt, i->Vals, warps, ipoint, rawnormal, ray, weight, isect, shadowflag, photonPass);

            resultColour += lc * val;
            resultTransm += lt * val;

            total += val;
        }

        resultColour /= total;
        resultTransm /= total;
    }
    else
    {
        for(vector<TextureBlendMapEntry>::const_iterator i = bmap->Blend_Map_Entries.begin(); i != bmap->Blend_Map_Entries.end(); i++)
            total += i->value;

        for(vector<TextureBlendMapEntry>::const_iterator i = bmap->Blend_Map_Entries.begin(); i != bmap->Blend_Map_Entries.end(); i++)
        {
            lc = resultColour * (i->value / total);

            ComputeOneTextureColour(lc, lt, i->Vals, warps, ipoint, rawnormal, ray, weight, isect, shadowflag, photonPass);
        }
    }
}

void Trace::ComputeLightedTexture(MathColour& resultColour, ColourChannel& resultTransm, const TEXTURE *texture, vector<const TEXTURE *>& warps, const Vector3d& ipoint,
                                  const Vector3d& rawnormal, Ray& ray, COLC weight, Intersection& isect)
{
    Interior *interior;
    const TEXTURE *layer;
    int i;
    bool radiosity_done, radiosity_back_done, radiosity_needed;
    int layer_number;
    double w1;
    double new_Weight;
    double att, trans, max_Radiosity_Contribution;
    double cos_Angle_Incidence;
    Vector3d layNormal, topNormal;
    MathColour attCol;
    TransColour layCol;
    MathColour rflCol;
    MathColour rfrCol;
    ColourChannel rfrTransm;
    MathColour filCol;
    MathColour tmpCol, tmp;
    MathColour ambCol; // Note that there is no gathering of filter or transparency
    MathColour ambBackCol;
    bool one_colour_found, colour_found;
    bool tir_occured;
    std::unique_ptr<PhotonGatherer> surfacePhotonGatherer(nullptr);

    double relativeIor;
    ComputeRelativeIOR(ray, isect.Object->interior.get(), relativeIor);

    WNRXVector listWNRX(wnrxPool); // "Weight, Normal, Reflectivity, eXponent"
    POV_REFPOOL_ASSERT(listWNRX->empty()); // verify that the WNRXVector pulled from the pool is in a cleaned-up condition

    // resultColour builds up the apparent visible color of the point.
    // resultTransm builds up the apparent visible color of whatever is behind the point (presuming 100% white background).
    resultColour.Clear();
    resultTransm = 0.0;

    // filCol serves two purposes. It accumulates the filter properties
    // of a multi-layer texture so that if a ray makes it all the way through
    // all layers, the color of object behind is filtered by this object.
    // It also is used to attenuate how much of an underlayer you
    // can see in a layered texture.  Note that when computing the reflective
    // properties of a layered texture, the upper layers don't filter the
    // light from the lower layers -- the layer colors add together (even
    // before we added additive transparency via the "transmit" 5th
    // color channel).  However when computing the transmitted rays, all layers
    // filter the light from any objects behind this object. [CY 1/95]

    // NK layers - switched transmit component to zero
    // [CLi] changed filCol to RGB, as filter and transmit were always pinned to 1.0 and 0.0 respectively anyway
    filCol = MathColour(1.0);

    trans = 1.0;

    // Add in radiosity (stochastic interreflection-based ambient light) if desired
    radiosity_done = false;
    radiosity_back_done = false;

    // This block just sets up radiosity for the code inside the loop, which is first-time-through.
    radiosity_needed = (sceneData->radiositySettings.radiosityEnabled == true) &&
                       (radiosity.CheckRadiosityTraceLevel(ray.GetTicket()) == true) &&
                       (Test_Flag(isect.Object, IGNORE_RADIOSITY_FLAG) == false);

    // Loop through the layers and compute the ambient, diffuse,
    // phong and specular for these textures.
    one_colour_found = false;

    if(sceneData->photonSettings.photonsEnabled && sceneData->surfacePhotonMap.numPhotons > 0)
        surfacePhotonGatherer.reset(new PhotonGatherer(&sceneData->surfacePhotonMap, sceneData->photonSettings));

    for (layer_number = 0, layer = texture; (layer != nullptr) && (trans > ray.GetTicket().adcBailout); layer_number++, layer = layer->Next)
    {
        // Get perturbed surface normal.
        layNormal = rawnormal;

        if (qualityFlags.normals && (layer->Tnormal != nullptr))
        {
            for(vector<const TEXTURE *>::iterator i(warps.begin()); i != warps.end(); i++)
                Warp_Normal(layNormal, layNormal, *i, Test_Flag(*i, DONT_SCALE_BUMPS_FLAG));

            Perturb_Normal(layNormal, layer->Tnormal, ipoint, &isect, &ray, threadData);

            if((Test_Flag(layer->Tnormal, DONT_SCALE_BUMPS_FLAG)))
                layNormal.normalize();

            // TODO - Reverse iterator may be less performant than forward iterator; we might want to
            //        compare performance with using forward iterators and decrement, or using random access.
            for(vector<const TEXTURE *>::reverse_iterator i(warps.rbegin()); i != warps.rend(); i++)
                UnWarp_Normal(layNormal, layNormal, *i, Test_Flag(*i, DONT_SCALE_BUMPS_FLAG));
        }

        // Store top layer normal.
        if(layer_number == 0)
            topNormal = layNormal;

        // Get surface colour.
        new_Weight = weight * trans;
        colour_found = Compute_Pigment(layCol, layer->Pigment, ipoint, &isect, &ray, threadData);

        // If a valid color was returned set one_colour_found to true.
        // An invalid color is returned if a surface point is outside
        // an image map used just once.
        one_colour_found = (one_colour_found || colour_found);

        // This section of code used to be the routine Compute_Reflected_Colour.
        // I copied it in here to rearrange some of it more easily and to
        // see if we could eliminate passing a zillion parameters for no
        // good reason. [CY 1/95]

        if(qualityFlags.ambientOnly)
        {
            // Only use top layer and kill transparency if low quality.
            resultColour = layCol.colour();
            resultTransm = 0.0;
        }
        else
        {
            // Store vital information for later reflection.
            listWNRX->push_back(WNRX(new_Weight, layNormal, MathColour(), layer->Finish->Reflect_Exp));

            // angle-dependent reflectivity
            cos_Angle_Incidence = -dot(ray.Direction, layNormal);

            if ((isect.Object->interior == nullptr) && layer->Finish->Reflection_Fresnel)
                throw POV_EXCEPTION_STRING("fresnel reflection used with no interior."); // TODO FIXME - wrong place to report this [trf]

            ComputeReflectivity(listWNRX->back().weight, listWNRX->back().reflec,
                                layer->Finish->Reflection_Max, layer->Finish->Reflection_Min,
                                layer->Finish->Reflection_Fresnel, layer->Finish->Reflection_Falloff,
                                cos_Angle_Incidence, relativeIor);

            ComputeMetallic(listWNRX->back().reflec, layer->Finish->Reflect_Metallic, layCol.colour(), cos_Angle_Incidence);

            // We need to reduce the layer's own brightness if it is transparent.
            if (sceneData->EffectiveLanguageVersion() < 370)
                att = layCol.LegacyOpacity();
            else
                att = layCol.Opacity();

            if (layer->Finish->AlphaKnockout)
                listWNRX->back().reflec *= att;

            // now compute the BRDF or BSSRDF contribution
            tmpCol.Clear();

            if(sceneData->useSubsurface && layer->Finish->UseSubsurface && qualityFlags.subsurface)
            {
                // Add diffuse & single scattering contribution.
                ComputeSubsurfaceScattering(layer->Finish, layCol.colour(), isect, ray, layNormal, tmpCol, att);
                // [CLi] moved multiplication with filCol to further below

                // Radiosity-style ambient may be subject to subsurface light transport.
                // In that case, the respective computations are handled by the BSSRDF code already.
                if (sceneData->subsurfaceUseRadiosity)
                    radiosity_needed = false;
            }
            // Add radiosity ambient contribution.
            if(radiosity_needed)
            {
                DBL diffuse    = layer->Finish->Diffuse;
                DBL brilliance = 1.0;

                if (sceneData->radiositySettings.brilliance)
                {
                    diffuse    *= layer->Finish->BrillianceAdjustRad;
                    brilliance =  layer->Finish->Brilliance;
                }

                // if radiosity calculation needed, but not yet done, do it now
                // TODO FIXME - [CLi] with "normal on", shouldn't we compute radiosity for each layer separately (if it has pertubed normals)?
                // TODO FIXME - [CLi] with "brilliance on", shouldn't we compute radiosity for each layer separately (if it has non-default brilliance)?
                if(radiosity_done == false)
                {
                    // calculate max possible contribution of radiosity, to see if calculating it is worthwhile
                    // TODO FIXME - other layers may see a higher weight!
                    // Maybe we should go along and compute *first* the total contribution radiosity will make,
                    // and at the *end* apply it.
                    max_Radiosity_Contribution = (filCol * layCol.colour()).WeightGreyscale() * att * diffuse;

                    if(max_Radiosity_Contribution > ray.GetTicket().adcBailout)
                    {
                        radiosity.ComputeAmbient(isect.IPoint, rawnormal, layNormal, brilliance, ambCol, weight * max_Radiosity_Contribution, ray.GetTicket());
                        radiosity_done = true;
                    }
                }

                // [CLi] moved multiplication with filCol to further below
                MathColour radiosityContribution = (layCol.colour() * ambCol) * (att * diffuse);

                diffuse = layer->Finish->DiffuseBack;
                if (sceneData->radiositySettings.brilliance)
                    diffuse *= layer->Finish->BrillianceAdjustRad;

                // if backside radiosity calculation needed, but not yet done, do it now
                // TODO FIXME - [CLi] with "normal on", shouldn't we compute radiosity for each layer separately (if it has pertubed normals)?
                // TODO FIXME - [CLi] with "brilliance on", shouldn't we compute radiosity for each layer separately (if it has non-default brilliance)?
                if(layer->Finish->DiffuseBack != 0.0)
                {
                    if(radiosity_back_done == false)
                    {
                        // calculate max possible contribution of radiosity, to see if calculating it is worthwhile
                        // TODO FIXME - other layers may see a higher weight!
                        // Maybe we should go along and compute *first* the total contribution radiosity will make,
                        // and at the *end* apply it.
                        max_Radiosity_Contribution = (filCol * layCol.colour()).WeightGreyscale() * att * diffuse;

                        if(max_Radiosity_Contribution > ray.GetTicket().adcBailout)
                        {
                            radiosity.ComputeAmbient(isect.IPoint, -rawnormal, -layNormal, brilliance, ambBackCol, weight * max_Radiosity_Contribution, ray.GetTicket());
                            radiosity_back_done = true;
                        }
                    }

                    // [CLi] moved multiplication with filCol to further below
                    radiosityContribution += (layCol.colour() * ambBackCol) * (att * diffuse);
                }

#if POV_PARSER_EXPERIMENTAL_BRILLIANCE_OUT
                if((sceneData->radiositySettings.brilliance) && (layer->Finish->BrillianceOut != 1.0))
                    radiosityContribution *= pow(fabs(cos_Angle_Incidence), layer->Finish->BrillianceOut-1.0) * (layer->Finish->BrillianceOut+7.0)/8.0;
#endif

                if (layer->Finish->Fresnel != 0.0)
                {
                    // In diffuse reflections (which includes radiosity), the Fresnel effect is
                    // that we _lose_ the reflected component as the light enters the material
                    // (where it changes direction randomly), and then _again lose_ another
                    // reflected component as the light leaves the material. However, taking into
                    // account the former of these losses requires knowledge of the incoming light
                    // direction, which must be accounted for in radiosity sampling (TODO - not implemented yet),
                    // so we only do the latter here.
                    // NB: One might think that to properly compute the outgoing loss we would have
                    // to compute the Fresnel term for the _internal_ angle of incidence and the
                    // _inverse_ of the relative refractive index; however, the Fresnel formula
                    // is such that we can just as well plug in the external angle of incidence
                    // and straight relative refractive index.
                    double f = layer->Finish->Fresnel * FresnelR(cos_Angle_Incidence, relativeIor);
                    radiosityContribution *= (1.0 - f);
                }

                tmpCol += radiosityContribution;
            }

            // Add emissive ("classic" ambient) contribution.
            // [CLi] moved multiplication with filCol to further below
            MathColour emission = layer->Finish->Emission;
            if (!sceneData->radiositySettings.radiosityEnabled || (sceneData->EffectiveLanguageVersion() < 370))
                // only use "ambient" setting when radiosity is disabled (or in legacy scenes)
                emission += layer->Finish->Ambient * sceneData->ambientLight;
            if (layer->Finish->Fresnel != 0.0)
            {
                // Light emitted from the material itself is subject to the Fresnel effect as it
                // leaves the material _losing_ light reflected at the interface.
                // In diffuse reflections (which includes ambient), the Fresnel effect is
                // that we _lose_ the reflected component as the light enters the material
                // (where it changes direction randomly), and then _again lose_ another
                // reflected component as the light leaves the material. The former of these
                // losses needs to be accounted for when choosing the `ambient` setting,
                // so we only do the latter here.
                // NB: One might think that to properly compute the outgoing loss we would have
                // to compute the Fresnel term for the _internal_ angle of incidence and the
                // _inverse_ of the relative refractive index; however, the Fresnel formula
                // is such that we can just as well plug in the external angle of incidence
                // and straight relative refractive index.
                double f = layer->Finish->Fresnel * FresnelR(cos_Angle_Incidence, relativeIor);
                emission *= (1.0 - f);
            }
            tmpCol += (layCol.colour() * emission * att);

            // set up the "litObjectIgnoresPhotons" flag (thread variable) so that
            // ComputeShadowColour will know whether or not this lit object is
            // ignoring photons, which affects partial-shadowing (i.e. filter and transmit)
            threadData->litObjectIgnoresPhotons = Test_Flag(isect.Object,PH_IGNORE_PHOTONS_FLAG);

            // Add diffuse, phong, specular, and iridescence contribution.
            // (We don't need to do this for (non-radiosity) rays during pretrace, as it does not affect radiosity sampling)
            if(!ray.IsPretraceRay())
            {
                if (((layer->Finish->Diffuse     != 0.0) ||
                     (layer->Finish->DiffuseBack != 0.0) ||
                     (layer->Finish->Specular    != 0.0) ||
                     (layer->Finish->Phong       != 0.0)) &&
                    ((!layer->Finish->AlphaKnockout) ||
                     (att != 0.0)))
                {
                    MathColour classicContribution;

                    ComputeDiffuseLight(layer->Finish, isect.IPoint, ray, layNormal, layCol.colour(), classicContribution, att, isect.Object, relativeIor);

#if POV_PARSER_EXPERIMENTAL_BRILLIANCE_OUT
                    if(layer->Finish->BrillianceOut != 1.0)
                    {
                        double cos_angle_of_incidence = dot(ray.Direction, layNormal);
                        classicContribution *= pow(fabs(cos_angle_of_incidence), layer->Finish->BrillianceOut-1.0)* (layer->Finish->BrillianceOut+7.0)/8.0;
                    }
#endif

                    tmpCol += classicContribution;
                }
            }

            if(sceneData->photonSettings.photonsEnabled && sceneData->surfacePhotonMap.numPhotons > 0)
            {
                // NK phmap - now do the same for the photons in the area
                if(!Test_Flag(isect.Object, PH_IGNORE_PHOTONS_FLAG))
                {
                    MathColour photonsContribution;

                    ComputePhotonDiffuseLight(layer->Finish, isect.IPoint, ray, layNormal, rawnormal, layCol.colour(), photonsContribution, att, isect.Object, relativeIor, *surfacePhotonGatherer);

#if POV_PARSER_EXPERIMENTAL_BRILLIANCE_OUT
                    if(layer->Finish->BrillianceOut != 1.0)
                    {
                        double cos_angle_of_incidence = dot(ray.Direction, layNormal);
                        photonsContribution *= pow(fabs(cos_angle_of_incidence), layer->Finish->BrillianceOut-1.0) * (layer->Finish->BrillianceOut+7.0)/8.0;
                    }
#endif

                    tmpCol += photonsContribution;
                }
            }

            tmpCol *= filCol;
            resultColour += tmpCol;
        }

        // Get new filter color.
        if(colour_found)
        {
            filCol *= layCol.TransmittedColour();

            if(layer->Finish->Conserve_Energy != 0 && listWNRX->empty() == false)
            {
                // adjust filCol based on reflection
                // this would work so much better with r,g,b,rt,gt,bt
                filCol *= (1.0 - listWNRX->back().reflec).ClippedUpper(1.0);
            }
        }

        // Get new remaining translucency.
        // [CLi] changed filCol to RGB, as filter and transmit were always pinned to 1.0 and 0.0, respectively anyway
        // TODO CLARIFY - is this working properly if filCol.greyscale() is negative? (what would be the right thing then?)
        trans = min(1.0, (double)fabs(filCol.Greyscale()));
    }

    // Calculate transmitted component.
    //
    // If the surface is translucent a transmitted ray is traced
    // and its contribution is added to the total ResCol after
    // filtering it by filCol.
    tir_occured = false;

    if (((interior = isect.Object->interior.get()) != nullptr) && (trans > ray.GetTicket().adcBailout) && qualityFlags.refractions)
    {
        // [CLi] changed filCol to RGB, as filter and transmit were always pinned to 1.0 and 0.0, respectively anyway
        // TODO CLARIFY - is this working properly if some filCol component is negative? (what would be the right thing then?)
        w1 = filCol.WeightMaxAbs();
        new_Weight = weight * w1;

        // Trace refracted ray.
        tir_occured = ComputeRefraction(texture->Finish, interior, isect.IPoint, ray, topNormal, rawnormal, rfrCol, rfrTransm, new_Weight);

        // Get distance based attenuation.
        // TODO - virtually the same code is used in ComputeShadowTexture().
        attCol.Set(interior->Old_Refract);

        if ((interior != nullptr) && ray.IsInterior(interior) == true)
        {
            if(fabs(interior->Fade_Distance) > EPSILON)
            {
                // NK attenuate
                if(interior->Fade_Power >= 1000)
                {
                    double depth = isect.Depth / interior->Fade_Distance;
                    attCol *= Exp(-(1.0 - interior->Fade_Colour) * depth);
                }
                else
                {
                    att = 1.0 + pow(isect.Depth / interior->Fade_Distance, (double)interior->Fade_Power);
                    attCol *= (interior->Fade_Colour + (1.0 - interior->Fade_Colour) / att);
                }
            }
        }

        // If total internal reflection occured the transmitted light is not filtered.
        if(tir_occured)
        {
            resultColour += attCol * rfrCol;
            // NOTE: transm() (alpha channel) stays zero
        }
        else
        {
            if(one_colour_found)
            {
                // [CLi] changed filCol to RGB, as filter and transmit were always pinned to 1.0 and 0.0, respectively anyway
                resultColour += attCol * rfrCol * filCol;
                // We need to know the transmittance value for the alpha channel. [DB]
                resultTransm = attCol.Greyscale() * rfrTransm * trans;
            }
            else
            {
                resultColour += attCol * rfrCol;
                // We need to know the transmittance value for the alpha channel. [DB]
                resultTransm = attCol.Greyscale() * rfrTransm;
            }
        }
    }

    // Calculate reflected component.
    //
    // If total internal reflection occured all reflections using
    // TopNormal are skipped.
    if(qualityFlags.reflections)
    {
        layer = texture;
        for(i = 0; i < layer_number; i++)
        {
            if((!tir_occured) ||
               (fabs(topNormal[X]-(*listWNRX)[i].normal[X]) > EPSILON) ||
               (fabs(topNormal[Y]-(*listWNRX)[i].normal[Y]) > EPSILON) ||
               (fabs(topNormal[Z]-(*listWNRX)[i].normal[Z]) > EPSILON))
            {
                if(!(*listWNRX)[i].reflec.IsZero())
                {
                    rflCol.Clear();
                    ComputeReflection(layer->Finish, isect.IPoint, ray, (*listWNRX)[i].normal, rawnormal, rflCol, (*listWNRX)[i].weight);

                    if((*listWNRX)[i].reflex != 1.0)
                    {
                        resultColour += (*listWNRX)[i].reflec * Pow(rflCol, (*listWNRX)[i].reflex);
                    }
                    else
                    {
                        resultColour += (*listWNRX)[i].reflec * rflCol;
                    }
                }
            }
            layer = layer->Next;
        }
    }
}

void Trace::ComputeShadowTexture(MathColour& filtercolour, const TEXTURE *texture, vector<const TEXTURE *>& warps, const Vector3d& ipoint,
                                 const Vector3d& rawnormal, const Ray& ray, Intersection& isect)
{
    Interior *interior = isect.Object->interior.get();
    const TEXTURE *layer;
    double caustics, dotval, k;
    Vector3d layer_Normal;
    MathColour refraction;
    TransColour layer_Pigment_Colour;
    bool one_colour_found, colour_found;

    MathColour tmpCol = MathColour(1.0);

    one_colour_found = false;

    for (layer = texture; layer != nullptr; layer = layer->Next)
    {
        colour_found = Compute_Pigment(layer_Pigment_Colour, layer->Pigment, ipoint, &isect, &ray, threadData);

        if(colour_found)
        {
            one_colour_found = true;

            tmpCol *= layer_Pigment_Colour.TransmittedColour();
        }

        // Get normal for faked caustics (will rewrite later to cache).
        if ((interior != nullptr) && ((caustics = interior->Caustics) != 0.0))
        {
            layer_Normal = rawnormal;

            if (qualityFlags.normals && (layer->Tnormal != nullptr))
            {
                for(vector<const TEXTURE *>::iterator i(warps.begin()); i != warps.end(); i++)
                    Warp_Normal(layer_Normal, layer_Normal, *i, Test_Flag(*i, DONT_SCALE_BUMPS_FLAG));

                Perturb_Normal(layer_Normal, layer->Tnormal, ipoint, &isect, &ray, threadData);

                if((Test_Flag(layer->Tnormal,DONT_SCALE_BUMPS_FLAG)))
                    layer_Normal.normalize();

                // TODO - Reverse iterator may be less performant than forward iterator; we might want to
                //        compare performance with using forward iterators and decrement, or using random access.
                for(vector<const TEXTURE *>::reverse_iterator i(warps.rbegin()); i != warps.rend(); i++)
                    UnWarp_Normal(layer_Normal, layer_Normal, *i, Test_Flag(*i, DONT_SCALE_BUMPS_FLAG));
            }

            // Get new filter/transmit values.
            dotval = dot(layer_Normal, ray.Direction);

            k = (1.0 + pow(fabs(dotval), caustics));

            tmpCol *= k;
        }
    }

    // TODO - [CLi] aren't spatial effects (distance attenuation, media) better handled in Trace::ComputeTextureColour()? We may be doing double work here!

    // Get distance based attenuation.
    // TODO - virtually the same code is used in ComputeLightedTexture().
    refraction = MathColour(1.0);

    if ((interior != nullptr) && (ray.IsInterior(interior) == true))
    {
        if((interior->Fade_Power > 0.0) && (fabs(interior->Fade_Distance) > EPSILON))
        {
            // NK - attenuation
            if(interior->Fade_Power>=1000)
            {
                refraction *= Exp( -(1.0 - interior->Fade_Colour) * (isect.Depth / interior->Fade_Distance) );
            }
            else
            {
                k = 1.0 + pow(isect.Depth / interior->Fade_Distance, (double)interior->Fade_Power);
                refraction *= (interior->Fade_Colour + (1.0 - interior->Fade_Colour) / k);
            }
        }
    }

    // Get distance based attenuation.
    filtercolour = tmpCol * refraction;
}

void Trace::ComputeReflection(const FINISH* finish, const Vector3d& ipoint, Ray& ray, const Vector3d& normal, const Vector3d& rawnormal, MathColour& colour, COLC weight)
{
    Ray nray(ray);
    double n, n2;

    nray.SetFlags(Ray::ReflectionRay, ray);

    // The rest of this is essentally what was originally here, with small changes.
    n = -2.0 * dot(ray.Direction, normal);
    nray.Direction = ray.Direction + n * normal;

    // Nathan Kopp & CEY 1998 - Reflection bugfix
    // if the new ray is going the opposite direction as raw normal, we
    // need to fix it.
    n = dot(nray.Direction, rawnormal);

    if(n < 0.0)
    {
        // It needs fixing. Which kind?
        n2 = dot(nray.Direction, normal);

        if(n2 < 0.0)
        {
            // reflected inside rear virtual surface. Reflect Ray using Raw_Normal
            n = -2.0 * dot(ray.Direction, rawnormal);
            nray.Direction = ray.Direction + n * rawnormal;
        }
        else
        {
            // Double reflect NRay using Raw_Normal
            // n = dot(nray.Direction, rawnormal); - kept the old n around
            n *= -2.0;
            nray.Direction += n * rawnormal;
        }
    }

    nray.Direction.normalize();
    nray.Origin = ipoint;
    threadData->Stats()[Reflected_Rays_Traced]++;

    // Trace reflected ray.
    bool alphaBackground = ray.GetTicket().alphaBackground;
    ray.GetTicket().alphaBackground = false;
    if (!ray.IsPhotonRay() && (finish->Irid > 0.0))
    {
        MathColour tmpCol;
        ColourChannel dummyTransm;
        TraceRay(nray, tmpCol, dummyTransm, weight, false);
        ComputeIridColour(finish, nray.Direction, ray.Direction, normal, ipoint, tmpCol);
        colour += tmpCol;
    }
    else
    {
        ColourChannel dummyTransm;
        TraceRay(nray, colour, dummyTransm, weight, false);
    }
    ray.GetTicket().alphaBackground = alphaBackground;
}

bool Trace::ComputeRefraction(const FINISH* finish, Interior *interior, const Vector3d& ipoint, Ray& ray, const Vector3d& normal, const Vector3d& rawnormal, MathColour& colour, ColourChannel& transm, COLC weight)
{
    Ray nray(ray);
    Vector3d localnormal;
    double n, ior, dispersion;
    unsigned int dispersionelements = interior->Disp_NElems;
    POV_ASSERT(dispersionelements >= DISPERSION_ELEMENTS_MIN);
    bool totalReflection = false;

    nray.SetFlags(Ray::RefractionRay, ray);

    // Set up new ray.
    nray.Origin = ipoint;

    // Get ratio of iors depending on the interiors the ray is traversing.

    // Note:
    // For the purpose of refraction, the space occupied by "nested" objects is considered to be "outside" the containing objects,
    // i.e. when encountering (A (B B) A) we pretend that it's (A A|B B|A A).
    // (Here "(X" and "X)" denote the entering and leaving of object X, and "X|Y" denotes an interface between objects X and Y.)
    // In case of overlapping objects, the intersecting region is considered to be part of whatever object is encountered last,
    // i.e. when encountering (A (B A) B) we pretend that it's (A A|B B|B B).

    if(nray.GetInteriors().empty())
    {
        // The ray is entering from the atmosphere.
        nray.AppendInterior(interior);

        ior = sceneData->atmosphereIOR / interior->IOR;
        dispersion = sceneData->atmosphereDispersion / interior->Dispersion;
    }
    else
    {
        // The ray is currently inside an object.
        if(interior == nray.GetInteriors().back()) // The ray is leaving the "innermost" object
        {
            nray.RemoveInterior(interior);
            if(nray.GetInteriors().empty())
            {
                // The ray is leaving into the atmosphere
                ior = interior->IOR / sceneData->atmosphereIOR;
                dispersion = interior->Dispersion / sceneData->atmosphereDispersion;
            }
            else
            {
                // The ray is leaving into another object, i.e. (A (B B) ...
                // For the purpose of refraction, pretend that we weren't inside that other object,
                // i.e. pretend that we didn't encounter (A (B B) ... but (A A|B B|A ...
                ior = interior->IOR / nray.GetInteriors().back()->IOR;
                dispersion = interior->Dispersion / nray.GetInteriors().back()->Dispersion;
                dispersionelements = max(dispersionelements, (unsigned int)(nray.GetInteriors().back()->Disp_NElems));
            }
        }
        else if(nray.RemoveInterior(interior) == true) // The ray is leaving the intersection of overlapping objects, i.e. (A (B A) ...
        {
            // For the purpose of refraction, pretend that we had already left the other member of the intersection when we entered the overlap,
            // i.e. pretend that we didn't encounter (A (B A) ... but (A A|B B|B ...
            ior = 1.0;
            dispersion = 1.0;
        }
        else
        {
            // The ray is entering a new object.
            // For the purpose of refraction, pretend that we're leaving any containing objects,
            // i.e. pretend that we didn't encounter (A (B ... but (A A|B ...
            ior = nray.GetInteriors().back()->IOR / interior->IOR;
            dispersion = nray.GetInteriors().back()->Dispersion / interior->Dispersion;

            nray.AppendInterior(interior);
        }
    }

    bool haveDispersion = (fabs(dispersion - 1.0) >= EPSILON);

    // Do the two mediums traversed have the same indices of refraction?
    if((fabs(ior - 1.0) < EPSILON) && !haveDispersion)
    {
        // Only transmit the ray.
        nray.Direction = ray.Direction;
        // Trace a transmitted ray.
        threadData->Stats()[Transmitted_Rays_Traced]++;

        colour.Clear();
        transm = 0.0;
        TraceRay(nray, colour, transm, weight, true);
    }
    else
    {
        // Refract the ray.
        n = dot(ray.Direction, normal);

        if(n <= 0.0)
        {
            localnormal = normal;
            n = -n;
        }
        else
            localnormal = -normal;


        // TODO FIXME: also for first radiosity pass ? (see line 3272 of v3.6 lighting.cpp)
        if(!haveDispersion) // TODO FIXME - radiosity: || (!isFinalTrace)
            totalReflection = TraceRefractionRay(finish, ipoint, ray, nray, ior, n, normal, rawnormal, localnormal, colour, transm, weight);
        else if(ray.IsMonochromaticRay())
            totalReflection = TraceRefractionRay(finish, ipoint, ray, nray, ray.GetSpectralBand().GetDispersionIOR(ior, dispersion), n, normal, rawnormal, localnormal, colour, transm, weight);
        else
        {
            colour.Clear();
            transm = 0.0;

            for(unsigned int i = 0; i < dispersionelements; i++)
            {
                MathColour tempColour;
                ColourChannel tempTransm;

                // NB setting the dispersion factor also causes the MonochromaticRay flag to be set
                SpectralBand spectralBand(i, dispersionelements);
                nray.SetSpectralBand(spectralBand);

                (void)TraceRefractionRay(finish, ipoint, ray, nray, spectralBand.GetDispersionIOR(ior, dispersion), n, normal, rawnormal, localnormal, tempColour, tempTransm, weight);

                colour += tempColour * spectralBand.GetHue();
                transm += tempTransm;
            }

            colour /= ColourChannel(dispersionelements);
            transm /= ColourChannel(dispersionelements);
        }
    }

    return totalReflection;
}

bool Trace::TraceRefractionRay(const FINISH* finish, const Vector3d& ipoint, Ray& ray, Ray& nray, double ior, double n, const Vector3d& normal, const Vector3d& rawnormal, const Vector3d& localnormal, MathColour& colour, ColourChannel& transm, COLC weight)
{
    // Compute refrated ray direction using Heckbert's method.
    double t = 1.0 + Sqr(ior) * (Sqr(n) - 1.0);

    if(t < 0.0)
    {
        MathColour tempcolour;

        // Total internal reflection occures.
        threadData->Stats()[Internal_Reflected_Rays_Traced]++;
        ComputeReflection(finish, ipoint, ray, normal, rawnormal, tempcolour, weight);
        colour += tempcolour;

        return true;
    }

    t = ior * n - sqrt(t);

    nray.Direction = ior * ray.Direction + t * localnormal;

    // Trace a refracted ray.
    threadData->Stats()[Refracted_Rays_Traced]++;

    colour.Clear();
    transm = 0.0;
    TraceRay(nray, colour, transm, weight, false);

    return false;
}

// see Diffuse in the v3.6 code (lighting.cpp)
void Trace::ComputeDiffuseLight(const FINISH *finish, const Vector3d& ipoint, const Ray& eye, const Vector3d& layer_normal, const MathColour& layer_pigment_colour,
                                MathColour& colour, double attenuation, ObjectPtr object, double relativeIor)
{
    Vector3d reye;

    // TODO FIXME - [CLi] why is this computed here? Not so exciting, is it?
    if(finish->Specular != 0.0)
        reye = -eye.Direction;

    // global light sources, if not turned off for this object
    if((object->Flags & NO_GLOBAL_LIGHTS_FLAG) != NO_GLOBAL_LIGHTS_FLAG)
    {
        for(int i = 0; i < threadData->lightSources.size(); i++)
            ComputeOneDiffuseLight(*threadData->lightSources[i], reye, finish, ipoint, eye, layer_normal, layer_pigment_colour, colour, attenuation, object, relativeIor, i);
    }

    // local light sources from a light group, if any
    if(!object->LLights.empty())
    {
        for(int i = 0; i < object->LLights.size(); i++)
            ComputeOneDiffuseLight(*object->LLights[i], reye, finish, ipoint, eye, layer_normal, layer_pigment_colour, colour, attenuation, object, relativeIor);
    }
}

void Trace::ComputePhotonDiffuseLight(const FINISH *Finish, const Vector3d& IPoint, const Ray& Eye, const Vector3d& Layer_Normal, const Vector3d& Raw_Normal,
                                      const MathColour& Layer_Pigment_Colour, MathColour& colour, double Attenuation, ConstObjectPtr Object, double relativeIor, PhotonGatherer& gatherer)
{
    double Cos_Shadow_Angle;
    Vector3d lightDirection;
    MathColour Light_Colour;
    MathColour tmpCol, tmpCol2;
    double r;
    int n;
    int j;
    double thisDensity=0;
    double prevDensity=0.0000000000000001; // avoid div-by-zero error
    int expanded = false;
    double att;  // attenuation for lambertian compensation & filters

    if (!sceneData->photonSettings.photonsEnabled || sceneData->surfacePhotonMap.numPhotons<1)
        return;

    if ((Finish->Diffuse == 0.0) && (Finish->DiffuseBack == 0.0) && (Finish->Specular == 0.0) && (Finish->Phong == 0.0))
        return;

    // statistics
    threadData->Stats()[Gather_Performed_Count]++;

    if(gatherer.gathered)
        r = gatherer.alreadyGatheredRadius;
    else
        r = gatherer.gatherPhotonsAdaptive(&IPoint, &Layer_Normal, true);

    n = gatherer.gatheredPhotons.numFound;

    tmpCol.Clear();

    // now go through these photons and add up their contribution
    for(j=0; j<n; j++)
    {
        // double theta,phi;
        int theta,phi;
        bool backside = false;

        // convert small color to normal color
        Light_Colour = ToMathColour(RGBColour(gatherer.gatheredPhotons.photonGatherList[j]->colour));

        // convert theta/phi to vector direction
        // Use a pre-computed array of sin/cos to avoid many calls to the
        // sin() and cos() functions.  These arrays were initialized in
        // InitBacktraceEverything.
        theta = gatherer.gatheredPhotons.photonGatherList[j]->theta+127;
        phi = gatherer.gatheredPhotons.photonGatherList[j]->phi+127;

        lightDirection[Y] = sinCosData.sinTheta[theta];
        lightDirection[X] = sinCosData.cosTheta[theta];

        lightDirection[Z] = lightDirection[X]*sinCosData.sinTheta[phi];
        lightDirection[X] = lightDirection[X]*sinCosData.cosTheta[phi];

        // this compensates for real lambertian (diffuse) lighting (see paper)
        // use raw normal, not layer normal
        // att = dot(Layer_Normal, Light_Source_Ray.Direction);
        att = dot(Raw_Normal, lightDirection);
        if (att>1) att=1.0;
        if (att<.1) att = 0.1; // limit to 10x - otherwise we get bright dots
        att = 1.0 / fabs(att);

        // do gaussian filter
        //att *= 0.918*(1.0-(1.0-exp((-1.953) * gatherer.photonDistances[j])) / (1.0-exp(-1.953)) );
        // do cone filter
        //att *= 1.0-(sqrt(gatherer.photonDistances[j])/(4.0 * r)) / (1.0-2.0/(3.0*4.0));

        Light_Colour *= att;

        // See if light on far side of surface from camera.
        if (!(Test_Flag(Object, DOUBLE_ILLUMINATE_FLAG)))
        {
            Cos_Shadow_Angle = dot(Layer_Normal, lightDirection);
            if (Cos_Shadow_Angle < EPSILON)
            {
                if (Finish->DiffuseBack != 0.0)
                    backside = true;
                else
                    continue;
            }
        }

        // now add diffuse, phong, specular, irid contribution

        tmpCol2.Clear();

        if (!(sceneData->useSubsurface && Finish->UseSubsurface))
            // (Diffuse contribution is not supported in combination with BSSRDF, to emphasize the fact that the BSSRDF
            // model is intended to provide for all the diffuse term by default. If users want to add some additional
            // surface-only diffuse term, they should use layered textures.
            ComputeDiffuseColour(Finish, lightDirection, Eye.Direction, Layer_Normal, tmpCol2, Light_Colour, Layer_Pigment_Colour, relativeIor, Attenuation, backside);

        // NK rad - don't compute highlights for radiosity gather rays, since this causes
        // problems with colors being far too bright
        // don't compute highlights for diffuse backside illumination
        if(!Eye.IsRadiosityRay() && !backside) // TODO FIXME radiosity - is this really the right way to do it (speaking of realism)?
        {
            if (Finish->Phong > 0.0)
            {
                ComputePhongColour(Finish, lightDirection, Eye.Direction, Layer_Normal, tmpCol2, Light_Colour, Layer_Pigment_Colour, relativeIor);
            }
            if (Finish->Specular > 0.0)
            {
                ComputeSpecularColour(Finish, lightDirection, -Eye.Direction, Layer_Normal, tmpCol2, Light_Colour, Layer_Pigment_Colour, relativeIor);
            }
        }

        if (Finish->Irid > 0.0)
        {
            ComputeIridColour(Finish, lightDirection, Eye.Direction, Layer_Normal, IPoint, tmpCol2);
        }

        tmpCol += tmpCol2;
    }

    // finish the photons equation
    tmpCol /= M_PI*r*r;

    // add photon contribution to total lighting
    colour += tmpCol;
}

// see Diffuse_One_Light in the v3.6 code (lighting.cpp)
void Trace::ComputeOneDiffuseLight(const LightSource &lightsource, const Vector3d& reye, const FINISH *finish, const Vector3d& ipoint, const Ray& eye, const Vector3d& layer_normal,
                                   const MathColour& layer_pigment_colour, MathColour& colour, double attenuation, ConstObjectPtr object, double relativeIor, int light_index)
{
    double lightsourcedepth, cos_shadow_angle;
    Ray lightsourceray(eye);
    MathColour lightcolour;
    bool backside = false;
    MathColour tmpCol;

    // Get a colour and a ray.
    ComputeOneLightRay(lightsource, lightsourcedepth, lightsourceray, ipoint, lightcolour);

    // Don't calculate spotlights when outside of the light's cone.
    if(lightcolour.IsNearZero(EPSILON))
        return;

    // See if light on far side of surface from camera.
    if(!(Test_Flag(object, DOUBLE_ILLUMINATE_FLAG)) // NK 1998 double_illuminate - changed to Test_Flag
       && !lightsource.Use_Full_Area_Lighting) // JN2007: Easiest way of getting rid of sharp shadow lines
    {
        cos_shadow_angle = dot(layer_normal, lightsourceray.Direction);
        if(cos_shadow_angle < EPSILON)
        {
            if (finish->DiffuseBack != 0.0)
                backside = true;
            else
                return;
        }
    }

    // If light source was not blocked by any intervening object, then
    // calculate it's contribution to the object's overall illumination.
    if (qualityFlags.shadows && ((lightsource.Projected_Through_Object != nullptr) || (lightsource.Light_Type != FILL_LIGHT_SOURCE)))
    {
        if (lightColorCacheIndex != -1 && light_index != -1)
        {
            if (lightColorCache[lightColorCacheIndex][light_index].tested == false)
            {
                // note that lightColorCache may be re-sized during trace, so we don't store a reference to it across the call
                TraceShadowRay(lightsource, lightsourcedepth, lightsourceray, ipoint, lightcolour);
                lightColorCache[lightColorCacheIndex][light_index].tested = true;
                lightColorCache[lightColorCacheIndex][light_index].colour = lightcolour;
            }
            else
                lightcolour = lightColorCache[lightColorCacheIndex][light_index].colour;
        }
        else
            TraceShadowRay(lightsource, lightsourcedepth, lightsourceray, ipoint, lightcolour);
    }

    if(!lightcolour.IsNearZero(EPSILON))
    {
        if(lightsource.Area_Light && lightsource.Use_Full_Area_Lighting &&
            qualityFlags.areaLights) // JN2007: Full area lighting
        {
            ComputeFullAreaDiffuseLight(lightsource, reye, finish, ipoint, eye,
                layer_normal, layer_pigment_colour, colour, attenuation,
                lightsourcedepth, lightsourceray, lightcolour,
                object, relativeIor);
            return;
        }

        if(!(sceneData->useSubsurface && finish->UseSubsurface))
            // (Diffuse contribution is not supported in combination with BSSRDF, to emphasize the fact that the BSSRDF
            // model is intended to provide for all the diffuse term by default. If users want to add some additional
            // surface-only diffuse term, they should use layered textures.
            ComputeDiffuseColour(finish, lightsourceray.Direction, eye.Direction, layer_normal, tmpCol, lightcolour, layer_pigment_colour, relativeIor, attenuation, backside);

        MathColour tempLightColour = (finish->AlphaKnockout ? lightcolour * attenuation : lightcolour);

        // NK rad - don't compute highlights for radiosity gather rays, since this causes
        // problems with colors being far too bright
        // don't compute highlights for diffuse backside illumination
        if((lightsource.Light_Type != FILL_LIGHT_SOURCE) && !eye.IsRadiosityRay() && !backside) // TODO FIXME radiosity - is this really the right way to do it (speaking of realism)?
        {
            if(finish->Phong > 0.0)
            {
                ComputePhongColour (finish, lightsourceray.Direction, eye.Direction, layer_normal, tmpCol,
                                    tempLightColour, layer_pigment_colour, relativeIor);
            }

            if(finish->Specular > 0.0)
                ComputeSpecularColour (finish, lightsourceray.Direction, -eye.Direction, layer_normal, tmpCol,
                                       tempLightColour, layer_pigment_colour, relativeIor);
        }

        if(finish->Irid > 0.0)
            ComputeIridColour(finish, lightsourceray.Direction, eye.Direction, layer_normal, ipoint, tmpCol);
    }

    colour += tmpCol;
}

// JN2007: Full area lighting:
void Trace::ComputeFullAreaDiffuseLight(const LightSource &lightsource, const Vector3d& reye, const FINISH *finish, const Vector3d& ipoint, const Ray& eye,
                                        const Vector3d& layer_normal, const MathColour& layer_pigment_colour, MathColour& colour, double attenuation,
                                        double lightsourcedepth, Ray& lightsourceray, const MathColour& lightcolour, ConstObjectPtr object, double relativeIor)
{
    Vector3d temp;
    Vector3d axis1Temp, axis2Temp;
    double axis1_Length, cos_shadow_angle;

    axis1Temp = lightsource.Axis1;
    axis2Temp = lightsource.Axis2;

    if(lightsource.Orient == true)
    {
        // Orient the area light to face the intersection point [ENB 9/97]

        // Do Light source to get the correct lightsourceray
        ComputeOneWhiteLightRay(lightsource, lightsourcedepth, lightsourceray, ipoint);

        // Save the lengths of the axises
        axis1_Length = axis1Temp.length();

        // Make axis 1 be perpendicular with the light-ray
        if(fabs(fabs(lightsourceray.Direction[Z]) - 1.0) < 0.01)
            // too close to vertical for comfort, so use cross product with horizon
            temp = Vector3d(0.0, 1.0, 0.0);
        else
            temp = Vector3d(0.0, 0.0, 0.1);

        axis1Temp = cross(lightsourceray.Direction, temp).normalized();

        // Make axis 2 be perpendicular with the light-ray and with Axis1.  A simple cross-product will do the trick.
        axis2Temp = cross(lightsourceray.Direction, axis1Temp).normalized();

        // make it square
        axis1Temp *= axis1_Length;
        axis2Temp *= axis1_Length;
    }

    MathColour sampleLightcolour = lightcolour / (lightsource.Area_Size1 * lightsource.Area_Size2);
    MathColour attenuatedLightcolour;

    for(int v = 0; v < lightsource.Area_Size2; ++v)
    {
        for(int u = 0; u < lightsource.Area_Size1; ++u)
        {
            Vector3d jitterAxis1, jitterAxis2;
            Ray lsr(lightsourceray);
            double jitter_u = (double)u;
            double jitter_v = (double)v;
            bool backside = false;
            MathColour tmpCol;

            if(lightsource.Jitter)
            {
                jitter_u += randomNumberGenerator() - 0.5;
                jitter_v += randomNumberGenerator() - 0.5;
            }

            // Create circular are lights [ENB 9/97]
            // First, make jitter_u and jitter_v be numbers from -1 to 1
            // Second, set scaleFactor to the abs max (jitter_u,jitter_v) (for shells)
            // Third, divide scaleFactor by the length of <jitter_u,jitter_v>
            // Fourth, scale jitter_u & jitter_v by scaleFactor
            // Finally scale Axis1 by jitter_u & Axis2 by jitter_v
            if(lightsource.Circular == true)
            {
                jitter_u = jitter_u / (lightsource.Area_Size1 - 1) - 0.5 + 0.001;
                jitter_v = jitter_v / (lightsource.Area_Size2 - 1) - 0.5 + 0.001;
                double scaleFactor = ((fabs(jitter_u) > fabs(jitter_v)) ? fabs(jitter_u) : fabs(jitter_v));
                scaleFactor /= sqrt(jitter_u * jitter_u + jitter_v * jitter_v);
                jitter_u *= scaleFactor;
                jitter_v *= scaleFactor;
                jitterAxis1 = axis1Temp * jitter_u;
                jitterAxis2 = axis2Temp * jitter_v;
            }
            else
            {
                if(lightsource.Area_Size1 > 1)
                {
                    double scaleFactor = jitter_u / (double)(lightsource.Area_Size1 - 1) - 0.5;
                    jitterAxis1 = axis1Temp * scaleFactor;
                }
                else
                    jitterAxis1 = Vector3d(0.0, 0.0, 0.0);

                if(lightsource.Area_Size2 > 1)
                {
                    double scaleFactor = jitter_v / (double)(lightsource.Area_Size2 - 1) - 0.5;
                    jitterAxis2 = axis2Temp * scaleFactor;
                }
                else
                    jitterAxis2 = Vector3d(0.0, 0.0, 0.0);
            }

            // Recalculate the light source ray but not the colour
            ComputeOneWhiteLightRay(lightsource, lightsourcedepth, lsr, ipoint, jitterAxis1 + jitterAxis2);
            // Calculate distance- and angle-based light attenuation
            attenuatedLightcolour = sampleLightcolour * Attenuate_Light(&lightsource, lsr, lightsourcedepth);

            // If not double-illuminated, check if the normal is pointing away:
            if(!Test_Flag(object, DOUBLE_ILLUMINATE_FLAG))
            {
                cos_shadow_angle = dot(layer_normal, lsr.Direction);
                if(cos_shadow_angle < EPSILON)
                {
                    if (finish->DiffuseBack != 0.0)
                        backside = true;
                    else
                        continue;
                }
            }

            if(!(sceneData->useSubsurface && finish->UseSubsurface))
                // (Diffuse contribution is not supported in combination with BSSRDF, to emphasize the fact that the BSSRDF
                // model is intended to provide for all the diffuse term by default. If users want to add some additional
                // surface-only diffuse term, they should use layered textures.
                ComputeDiffuseColour(finish, lsr.Direction, eye.Direction, layer_normal, tmpCol, attenuatedLightcolour, layer_pigment_colour, relativeIor, attenuation, backside);

            // NK rad - don't compute highlights for radiosity gather rays, since this causes
            // problems with colors being far too bright
            // don't compute highlights for diffuse backside illumination
            if((lightsource.Light_Type != FILL_LIGHT_SOURCE) && !eye.IsRadiosityRay() && !backside) // TODO FIXME radiosity - is this really the right way to do it (speaking of realism)?
            {
                if(finish->Phong > 0.0)
                {
                    ComputePhongColour(finish, lsr.Direction, eye.Direction, layer_normal, tmpCol, attenuatedLightcolour, layer_pigment_colour, relativeIor);
                }

                if(finish->Specular > 0.0)
                    ComputeSpecularColour(finish, lsr.Direction, -eye.Direction, layer_normal, tmpCol, attenuatedLightcolour, layer_pigment_colour, relativeIor);
            }

            if(finish->Irid > 0.0)
                ComputeIridColour(finish, lsr.Direction, eye.Direction, layer_normal, ipoint, tmpCol);

            colour += tmpCol;
        }
    }
}

// see do_light in v3.6's lighting.cpp
void Trace::ComputeOneLightRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray, const Vector3d& ipoint, MathColour& lightcolour, bool forceAttenuate)
{
    double attenuation;
    ComputeOneWhiteLightRay(lightsource, lightsourcedepth, lightsourceray, ipoint);

    // Get the light source colour.
    lightcolour = lightsource.colour;

    // Attenuate light source color.
    if (lightsource.Area_Light && lightsource.Use_Full_Area_Lighting && qualityFlags.areaLights && !forceAttenuate)
        // for full area lighting we apply distance- and angle-based attenuation to each "lightlet" individually later
        attenuation = 1.0;
    else
        attenuation = Attenuate_Light(&lightsource, lightsourceray, lightsourcedepth);

    // Now scale the color by the attenuation
    lightcolour *= attenuation;
}

// see block_light_source in the v3.6 source
void Trace::TraceShadowRay(const LightSource &lightsource, double depth, Ray& lightsourceray, const Vector3d& point, MathColour& colour)
{
    // test and set highest level traced. We do it differently than TraceRay() does,
    // for compatibility with the way max_trace_level is tested and reported in v3.6
    // and earlier.
    if(lightsourceray.GetTicket().traceLevel > lightsourceray.GetTicket().maxAllowedTraceLevel)
    {
        colour.Clear();
        return;
    }

    lightsourceray.GetTicket().maxFoundTraceLevel = (unsigned int) max(lightsourceray.GetTicket().maxFoundTraceLevel, lightsourceray.GetTicket().traceLevel);
    lightsourceray.GetTicket().traceLevel++;

    double newdepth;
    Intersection isect;
    Ray newray(lightsourceray);

    // Store current depth and ray because they will be modified.
    newdepth = depth;

    // NOTE: shadow rays are never photon rays, so flag can be hard-coded to false
    newray.SetFlags(Ray::OtherRay, true, false);

    // Get shadows from current light source.
    if(lightsource.Area_Light && qualityFlags.areaLights)
        TraceAreaLightShadowRay(lightsource, newdepth, newray, point, colour);
    else
        TracePointLightShadowRay(lightsource, newdepth, newray, colour);

    // If there's some distance left for the ray to reach the light source
    // we have to apply atmospheric stuff to this part of the ray.

    if((newdepth > SHADOW_TOLERANCE) && (lightsource.Media_Interaction) && (lightsource.Media_Attenuation))
    {
        isect.Depth = newdepth;
        isect.Object = nullptr;
        ComputeShadowMedia(newray, isect, colour, (lightsource.Media_Interaction) && (lightsource.Media_Attenuation));
    }

    lightsourceray.GetTicket().traceLevel--;
    maxFoundTraceLevel = (unsigned int) max(maxFoundTraceLevel, lightsourceray.GetTicket().maxFoundTraceLevel);
}

// moved this here (was originally inside TracePointLightShadowRay) because
// for some reason the Intel compiler (version W_CC_PC_8.1.027) will fail
// to link the exe, complaining of an unresolved external.
//
// TODO: try moving it back in at some point in the future.
struct NoShadowFlagRayObjectCondition final : public RayObjectCondition
{
    virtual bool operator()(const Ray&, ConstObjectPtr object, double) const override { return !Test_Flag(object, NO_SHADOW_FLAG); }
};

struct SmallToleranceRayObjectCondition final  : public RayObjectCondition
{
    virtual bool operator()(const Ray&, ConstObjectPtr, double dist) const override { return dist > SMALL_TOLERANCE; }
};

void Trace::TracePointLightShadowRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray, MathColour& lightcolour)
{
    Intersection boundedIntersection;
    ObjectPtr cacheObject = nullptr;
    bool foundTransparentObjects = false;
    bool foundIntersection;

    // Projected through main tests
    double projectedDepth = 0.0;

    if (lightsource.Projected_Through_Object != nullptr)
    {
        Intersection tempIntersection;

        if(FindIntersection(lightsource.Projected_Through_Object, tempIntersection, lightsourceray))
        {
            if((tempIntersection.Depth - lightsourcedepth) < 0.0)
                projectedDepth = lightsourcedepth - fabs(tempIntersection.Depth) + SMALL_TOLERANCE;
            else
            {
                lightcolour.Clear();
                return;
            }
        }
        else
        {
            lightcolour.Clear();
            return;
        }

        // Make sure we don't do shadows for fill light sources.
        // (Note that if Projected_Through_Object is `nullptr`, the test for FILL_LIGHT_SOURCE has happened earlier already)
        if(lightsource.Light_Type == FILL_LIGHT_SOURCE)
            return;
    }

    NoShadowFlagRayObjectCondition precond;
    SmallToleranceRayObjectCondition postcond;

    // check for object in the light source shadow cache (object that fully shadowed during last test) first

    if(lightsource.lightGroupLight == false) // we don't cache for light groups
    {
        if ((lightsourceray.GetTicket().traceLevel == 2) && (lightSourceLevel1ShadowCache[lightsource.index] != nullptr))
            cacheObject = lightSourceLevel1ShadowCache[lightsource.index];
        else if (lightSourceOtherShadowCache[lightsource.index] != nullptr)
            cacheObject = lightSourceOtherShadowCache[lightsource.index];

        // if there was an object in the light source shadow cache, check that first
        if (cacheObject != nullptr)
        {
            if(FindIntersection(cacheObject, boundedIntersection, lightsourceray, lightsourcedepth - projectedDepth) == true)
            {
                if(!Test_Flag(boundedIntersection.Object, NO_SHADOW_FLAG))
                {
                    ComputeShadowColour(lightsource, boundedIntersection, lightsourceray, lightcolour);

                    if(lightcolour.IsNearZero(EPSILON) &&
                       (Test_Flag(boundedIntersection.Object, OPAQUE_FLAG)))
                    {
                        threadData->Stats()[Shadow_Ray_Tests]++;
                        threadData->Stats()[Shadow_Rays_Succeeded]++;
                        threadData->Stats()[Shadow_Cache_Hits]++;
                        return;
                    }
                }
                else
                    cacheObject = nullptr;
            }
            else
                cacheObject = nullptr;
        }
    }

    foundTransparentObjects = false;

    while(true)
    {
        boundedIntersection.Object = boundedIntersection.Csg = nullptr;
        boundedIntersection.Depth = lightsourcedepth - projectedDepth;

        threadData->Stats()[Shadow_Ray_Tests]++;

        foundIntersection = FindIntersection(boundedIntersection, lightsourceray, precond, postcond);

        if((foundIntersection == true) && (boundedIntersection.Object != cacheObject) &&
           (boundedIntersection.Depth < lightsourcedepth - SHADOW_TOLERANCE) &&
           (lightsourcedepth - boundedIntersection.Depth > projectedDepth) &&
           (boundedIntersection.Depth > SHADOW_TOLERANCE))
        {
            threadData->Stats()[Shadow_Rays_Succeeded]++;

            ComputeShadowColour(lightsource, boundedIntersection, lightsourceray, lightcolour);

            ObjectPtr testObject(boundedIntersection.Csg != nullptr ? boundedIntersection.Csg : boundedIntersection.Object);

            if(lightcolour.IsNearZero(EPSILON) &&
               (Test_Flag(testObject, OPAQUE_FLAG)))
            {
                // Hit a fully opaque object; cache that object, so that next time we can test for it first;
                // don't cache for light groups though (why not??)

                if((lightsource.lightGroupLight == false) && (foundTransparentObjects == false))
                {
                    cacheObject = testObject;

                    if(lightsourceray.GetTicket().traceLevel == 2)
                        lightSourceLevel1ShadowCache[lightsource.index] = cacheObject;
                    else
                        lightSourceOtherShadowCache[lightsource.index] = cacheObject;
                }
                break;
            }

            foundTransparentObjects = true;

            // Move the ray to the point of intersection, plus some
            lightsourcedepth -= boundedIntersection.Depth;

            lightsourceray.Origin = boundedIntersection.IPoint;
        }
        else
            // No further intersections in the direction of the ray.
            break;
    }
}

void Trace::TraceAreaLightShadowRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray,
                                    const Vector3d& ipoint, MathColour& lightcolour)
{
    Vector3d temp;
    Vector3d axis1Temp, axis2Temp;
    double axis1_Length;

    lightGrid.resize(lightsource.Area_Size1 * lightsource.Area_Size2);

    // Flag uncalculated points with a negative value for Red
    /*
    for(i = 0; i < lightsource.Area_Size1; i++)
    {
        for(j = 0; j < lightsource.Area_Size2; j++)
            lightGrid[i * lightsource.Area_Size2 + j].Invalidate();
    }
    */
    for(size_t ind = 0; ind < lightGrid.size(); ++ind)
        lightGrid[ind].Invalidate();

    axis1Temp = lightsource.Axis1;
    axis2Temp = lightsource.Axis2;

    if(lightsource.Orient == true)
    {
        // Orient the area light to face the intersection point [ENB 9/97]

        // Do Light source to get the correct lightsourceray
        ComputeOneWhiteLightRay(lightsource, lightsourcedepth, lightsourceray, ipoint);

        // Save the lengths of the axes
        axis1_Length = axis1Temp.length();

        // Make axis 1 be perpendicular with the light-ray
        if(fabs(fabs(lightsourceray.Direction[Z]) - 1.0) < 0.01)
            // too close to vertical for comfort, so use cross product with horizon
            temp = Vector3d(0.0, 1.0, 0.0);
        else
            temp = Vector3d(0.0, 0.0, 1.0);

        axis1Temp = cross(lightsourceray.Direction, temp).normalized();

        // Make axis 2 be perpendicular with the light-ray and with Axis1.  A simple cross-product will do the trick.
        axis2Temp = cross(lightsourceray.Direction, axis1Temp).normalized();

        // make it square
        axis1Temp *= axis1_Length;
        axis2Temp *= axis1_Length;
    }

    TraceAreaLightSubsetShadowRay(lightsource, lightsourcedepth, lightsourceray, ipoint, lightcolour, 0, 0, lightsource.Area_Size1 - 1, lightsource.Area_Size2 - 1, 0, axis1Temp, axis2Temp);
}

void Trace::TraceAreaLightSubsetShadowRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray,
                                          const Vector3d& ipoint, MathColour& lightcolour, int u1, int  v1, int  u2, int  v2, int level, const Vector3d& axis1, const Vector3d& axis2)
{
    MathColour sample_Colour[4];
    int i, u, v, new_u1, new_v1, new_u2, new_v2;
    double jitter_u, jitter_v, scaleFactor;

    // Sample the four corners of the region
    for(i = 0; i < 4; i++)
    {
        Vector3d center(lightsource.Center);
        Ray lsr(lightsourceray);

        switch(i)
        {
            case 0: u = u1; v = v1; break;
            case 1: u = u2; v = v1; break;
            case 2: u = u1; v = v2; break;
            case 3: u = u2; v = v2; break;
            default: u = v = 0;  // Should never happen!
        }

        if(lightGrid[u * lightsource.Area_Size2 + v].IsValid())
            // We've already calculated this point, reuse it
            sample_Colour[i] = lightGrid[u * lightsource.Area_Size2 + v];
        else
        {
            Vector3d jitterAxis1, jitterAxis2;

            jitter_u = (double)u;
            jitter_v = (double)v;

            if(lightsource.Jitter)
            {
                jitter_u += randomNumberGenerator() - 0.5;
                jitter_v += randomNumberGenerator() - 0.5;
            }

            // Create circular are lights [ENB 9/97]
            // First, make jitter_u and jitter_v be numbers from -1 to 1
            // Second, set scaleFactor to the abs max (jitter_u,jitter_v) (for shells)
            // Third, divide scaleFactor by the length of <jitter_u,jitter_v>
            // Fourth, scale jitter_u & jitter_v by scaleFactor
            // Finally scale Axis1 by jitter_u & Axis2 by jitter_v
            if(lightsource.Circular == true)
            {
                jitter_u = jitter_u / (lightsource.Area_Size1 - 1) - 0.5 + 0.001;
                jitter_v = jitter_v / (lightsource.Area_Size2 - 1) - 0.5 + 0.001;
                scaleFactor = ((fabs(jitter_u) > fabs(jitter_v)) ? fabs(jitter_u) : fabs(jitter_v));
                scaleFactor /= sqrt(jitter_u * jitter_u + jitter_v * jitter_v);
                jitter_u *= scaleFactor;
                jitter_v *= scaleFactor;
                jitterAxis1 = axis1 * jitter_u;
                jitterAxis2 = axis2 * jitter_v;
            }
            else
            {
                if(lightsource.Area_Size1 > 1)
                {
                    scaleFactor = jitter_u / (double)(lightsource.Area_Size1 - 1) - 0.5;
                    jitterAxis1 = axis1 * scaleFactor;
                }
                else
                    jitterAxis1 = Vector3d(0.0, 0.0, 0.0);

                if(lightsource.Area_Size2 > 1)
                {
                    scaleFactor = jitter_v / (double)(lightsource.Area_Size2 - 1) - 0.5;
                    jitterAxis2 = axis2 * scaleFactor;
                }
                else
                    jitterAxis2 = Vector3d(0.0, 0.0, 0.0);
            }

            // Recalculate the light source ray but not the colour
            ComputeOneWhiteLightRay(lightsource, lightsourcedepth, lsr, ipoint, jitterAxis1 + jitterAxis2);

            sample_Colour[i] = lightcolour;

            TracePointLightShadowRay(lightsource, lightsourcedepth, lsr, sample_Colour[i]);

            lightGrid[u * lightsource.Area_Size2 + v] = sample_Colour[i];
        }
    }

    if((u2 - u1 > 1) || (v2 - v1 > 1))
    {
        if((level < lightsource.Adaptive_Level) ||
           (ColourDistance(sample_Colour[0], sample_Colour[1]) > 0.1) ||
           (ColourDistance(sample_Colour[1], sample_Colour[3]) > 0.1) ||
           (ColourDistance(sample_Colour[3], sample_Colour[2]) > 0.1) ||
           (ColourDistance(sample_Colour[2], sample_Colour[0]) > 0.1))
        {
            Vector3d center(lightsource.Center);

            for (i = 0; i < 4; i++)
            {
                switch (i)
                {
                    case 0:
                        new_u1 = u1;
                        new_v1 = v1;
                        new_u2 = (int)floor((u1 + u2)/2.0);
                        new_v2 = (int)floor((v1 + v2)/2.0);
                        break;
                    case 1:
                        new_u1 = (int)ceil((u1 + u2)/2.0);
                        new_v1 = v1;
                        new_u2 = u2;
                        new_v2 = (int)floor((v1 + v2)/2.0);
                        break;
                    case 2:
                        new_u1 = u1;
                        new_v1 = (int)ceil((v1 + v2)/2.0);
                        new_u2 = (int)floor((u1 + u2)/2.0);
                        new_v2 = v2;
                        break;
                    case 3:
                        new_u1 = (int)ceil((u1 + u2)/2.0);
                        new_v1 = (int)ceil((v1 + v2)/2.0);
                        new_u2 = u2;
                        new_v2 = v2;
                        break;
                    default:  // Should never happen!
                        new_u1 = new_u2 = new_v1 = new_v2 = 0;
                }

                // Recalculate the light source ray but not the colour
                ComputeOneWhiteLightRay(lightsource, lightsourcedepth, lightsourceray, ipoint, center);

                sample_Colour[i] = lightcolour;

                TraceAreaLightSubsetShadowRay(lightsource, lightsourcedepth, lightsourceray,
                                              ipoint, sample_Colour[i], new_u1, new_v1, new_u2, new_v2, level + 1, axis1, axis2);
            }
        }
    }

    // Average up the light contributions
    lightcolour = (sample_Colour[0] + sample_Colour[1] + sample_Colour[2] + sample_Colour[3]) * 0.25;
}

// see filter_shadow_ray in v3.6's lighting.cpp
void Trace::ComputeShadowColour(const LightSource &lightsource, Intersection& isect, Ray& lightsourceray, MathColour& colour)
{
    WeightedTextureVector wtextures;
    Vector3d ipoint;
    Vector3d raw_Normal;
    MathColour fc1;
    ColourChannel ft1;
    MathColour temp_Colour;
    Vector2d uv_Coords;
    double normaldirection;

    // Here's the issue:
    // Imagine "LightA" shoots photons at "GlassSphereB", which refracts light and
    // hits "PlaneC".
    // When computing Diffuse/Phong/etc lighting for PlaneC, if there were no
    // photons, POV would compute a filtered shadow ray from PlaneC through
    // GlassSphereB to LightA.  If photons are used for the combination of objects,
    // this filtered shadow ray should be completely black.  The filtered shadow
    // ray should be forced to black UNLESS any of the following conditions are
    // true (which would indicate that photons were not shot from LightA through
    // GlassSphereB to PlaneC):
    // 1) PlaneC has photon collection set to "off"
    // 2) GlassSphereB is not a photon target
    // 3) GlassSphereB has photon refraction set to "off"
    // 4) LightA has photon refraction set to "off"
    // 5) Neither GlassSphereB nor LightA has photon refraction set to "on"
    if((sceneData->photonSettings.photonsEnabled == true) &&
        (sceneData->surfacePhotonMap.numPhotons > 0) &&
        (!threadData->litObjectIgnoresPhotons) &&
        (Test_Flag(isect.Object,PH_TARGET_FLAG)) &&
        (!Test_Flag(isect.Object,PH_RFR_OFF_FLAG)) &&
        (!Test_Flag(&lightsource,PH_RFR_OFF_FLAG)) &&
        ((Test_Flag(isect.Object,PH_RFR_ON_FLAG) || Test_Flag(&lightsource,PH_RFR_ON_FLAG)))
        )
    {
        // full shadow (except for photon-based illumination)
        colour.Clear();
        return;
    }

    ipoint = isect.IPoint;

    if(!qualityFlags.shadows)
        // no shadow
        return;

    // If the object is opaque there's no need to go any further. [DB 8/94]
    if(Test_Flag(isect.Object, OPAQUE_FLAG))
    {
        // full shadow
        colour.Clear();
        return;
    }

    // Get the normal to the surface
    isect.Object->Normal(raw_Normal, &isect, threadData);

    // I added this to flip the normal if the object is inverted (for CSG).
    // However, I subsequently commented it out for speed reasons - it doesn't
    // make a difference (no pun intended). The preexisting flip code below
    // produces a similar (though more extensive) result. [NK]
    //
    // Actually, we should keep this code to guarantee that normaldirection
    // is set properly. [NK]
    if(Test_Flag(isect.Object, INVERTED_FLAG))
        raw_Normal.invert();

    // If the surface normal points away, flip its direction.
    normaldirection = dot(raw_Normal, lightsourceray.Direction);
    if(normaldirection > 0.0)
        raw_Normal.invert();

    isect.INormal = raw_Normal;
    // and save to intersection -hdf-
    isect.PNormal = raw_Normal;

    // now switch to UV mapping if we need to
    if(Test_Flag(isect.Object, UV_FLAG))
    {
        // TODO FIXME
        //  I think we have a serious problem here regarding bump mapping:
        //  The UV vector doesn't contain any information about the (local) *orientation* of U and V in our XYZ co-ordinate system!
        //  This causes slopes do be applied in the wrong directions.

        // get the UV vect of the intersection
        isect.Object->UVCoord(uv_Coords, &isect);
        // save the normal and UV coords into Intersection
        isect.Iuv = uv_Coords;

        ipoint[X] = uv_Coords[U];
        ipoint[Y] = uv_Coords[V];
        ipoint[Z] = 0;
    }

    // NB the v3.6 code doesn't set the light cache's Tested flags to false after incrementing the level.
    if (++lightColorCacheIndex >= lightColorCache.size())
    {
        lightColorCache.resize(lightColorCacheIndex + 10);
        for (LightColorCacheListList::iterator it = lightColorCache.begin() + lightColorCacheIndex; it != lightColorCache.end(); it++)
            it->resize(lightColorCache[0].size());
    }

    bool isMultiTextured = Test_Flag(isect.Object, MULTITEXTURE_FLAG) ||
                           ((isect.Object->Texture == nullptr) && Test_Flag(isect.Object, CUTAWAY_TEXTURES_FLAG));

    // get textures and weights
    if(isMultiTextured == true)
    {
        isect.Object->Determine_Textures(&isect, normaldirection > 0.0, wtextures, threadData);
    }
    else if (isect.Object->Texture != nullptr)
    {
        if ((normaldirection > 0.0) && (isect.Object->Interior_Texture != nullptr))
            wtextures.push_back(WeightedTexture(1.0, isect.Object->Interior_Texture)); /* Chris Huff: Interior Texture patch */
        else
            wtextures.push_back(WeightedTexture(1.0, isect.Object->Texture));
    }
    else
    {
        // don't need to do anything as the texture list will be empty.
        // TODO: could we perform these tests earlier ? [cjc]
        lightColorCacheIndex--;
        return;
    }

    temp_Colour.Clear();

    for(WeightedTextureVector::iterator i(wtextures.begin()); i != wtextures.end(); i++)
    {
        TextureVector warps(texturePool);
        POV_REFPOOL_ASSERT(warps->empty()); // verify that the TextureVector pulled from the pool is in a cleaned-up condition

        // If contribution of this texture is negligible skip ahead.
        if ((i->weight < lightsourceray.GetTicket().adcBailout) || (i->texture == nullptr))
            continue;

        ComputeOneTextureColour(fc1, ft1, i->texture, *warps, ipoint, raw_Normal, lightsourceray, 0.0, isect, true, false);

        temp_Colour += i->weight * fc1;
    }

    lightColorCacheIndex--;

    if(fabs(temp_Colour.Weight()) < lightsourceray.GetTicket().adcBailout)
    {
        // close enough to full shadow - bail out to avoid media computations
        colour.Clear();
        return;
    }

    // [CLi] moved this here from Trace::ComputeShadowTexture() and Trace::ComputeLightedTexture(), respectively,
    // to avoid media to be computed twice when dealing with averaged textures.
    // TODO - For photon rays we're still potentially doing double work on media.
    // TODO - For shadow rays we're still potentially doing double work on distance-based attenuation.
    // Calculate participating media effects.
    if(qualityFlags.media && (!lightsourceray.GetInteriors().empty()) && (lightsourceray.IsHollowRay() == true))
    {
        ColourChannel dummyTransm;
        media.ComputeMedia(lightsourceray.GetInteriors(), lightsourceray, isect, temp_Colour, dummyTransm);
    }

    colour *= temp_Colour;

    // Get atmospheric attenuation.
    ComputeShadowMedia(lightsourceray, isect, colour, (lightsource.Media_Interaction) && (lightsource.Media_Attenuation));
}

void Trace::ComputeDiffuseColour(const FINISH *finish, const Vector3d& lightDirection, const Vector3d& eyeDirection, const Vector3d& layer_normal, MathColour& colour, const MathColour& light_colour,
                                 const MathColour& layer_pigment_colour, double relativeIor, double attenuation, bool backside)
{
    double cos_angle_of_incidence, intensity;
    double diffuse = (backside? finish->DiffuseBack : finish->Diffuse) * finish->BrillianceAdjust;

    if (diffuse <= 0.0)
        return;

    cos_angle_of_incidence = dot(layer_normal, lightDirection);

    // Brilliance is likely to be 1.0 (default value)
    if(finish->Brilliance != 1.0)
        intensity = pow(fabs(cos_angle_of_incidence), (double) finish->Brilliance);
    else
        intensity = fabs(cos_angle_of_incidence);

    intensity *= diffuse * attenuation;

    if(finish->Crand > 0.0)
        intensity -= POV_rand(crandRandomNumberGenerator) * finish->Crand;

    if (finish->Fresnel != 0.0)
    {
        // In diffuse reflections, the Fresnel effect is that we _lose_ the reflected component
        // as the light enters the material (where it changes direction randomly), and then
        // _again lose_ another reflected component as the light leaves the material.
        // ComputeFresnel() gives us the reflected component.

        double f1 = finish->Fresnel * FresnelR(cos_angle_of_incidence, relativeIor);

        // NB: One might think that to properly compute the outgoing loss we would have
        // to compute the Fresnel term for the _internal_ angle of incidence and the
        // _inverse_ of the relative refractive index; however, the Fresnel formula
        // is such that we can just as well plug in the external angle of incidence
        // and straight relative refractive index.
        cos_angle_of_incidence = -dot(layer_normal, eyeDirection);
        double f2 = finish->Fresnel * FresnelR(cos_angle_of_incidence, relativeIor);

        colour += intensity * layer_pigment_colour * light_colour * (1.0 - f1) * (1.0 - f2);
    }
    else
        colour += intensity * layer_pigment_colour * light_colour;
}

void Trace::ComputeIridColour(const FINISH *finish, const Vector3d& lightDirection, const Vector3d& eyeDirection, const Vector3d& layer_normal, const Vector3d& ipoint, MathColour& colour)
{
    double cos_angle_of_incidence_light, cos_angle_of_incidence_eye, interference;
    double film_thickness;
    double noise;
    TurbulenceWarp turb;

    film_thickness = finish->Irid_Film_Thickness;

    if(finish->Irid_Turb != 0)
    {
        // Uses hardcoded octaves, lambda, omega
        turb.Omega=0.5;
        turb.Lambda=2.0;
        turb.Octaves=5;

        // Turbulence() returns a value from 0..1, so noise will be in order of magnitude 1.0 +/- finish->Irid_Turb
        noise = Turbulence(ipoint, &turb, sceneData->noiseGenerator);
        noise = 2.0 * noise - 1.0;
        noise = 1.0 + noise * finish->Irid_Turb;
        film_thickness *= noise;
    }

    // NOTE: Shouldn't we compute Cos_Angle_Of_Incidence just once?
    cos_angle_of_incidence_light = abs(dot(layer_normal, lightDirection));
    cos_angle_of_incidence_eye   = abs(dot(layer_normal, eyeDirection));

    // Calculate phase offset.
    interference = 2.0 * M_PI * film_thickness * (cos_angle_of_incidence_light + cos_angle_of_incidence_eye);

    // Modify color by phase offset for each wavelength.
    colour *= 1.0 + finish->Irid * Cos(interference / sceneData->iridWavelengths);
}

void Trace::ComputePhongColour(const FINISH *finish, const Vector3d& lightDirection, const Vector3d& eyeDirection, const Vector3d& layer_normal, MathColour& colour, const MathColour& light_colour,
                               const MathColour& layer_pigment_colour, double relativeIor)
{
    double cos_angle_of_incidence, intensity;
    Vector3d reflect_direction;

    cos_angle_of_incidence = -2.0 * dot(eyeDirection, layer_normal);

    reflect_direction = eyeDirection + cos_angle_of_incidence * layer_normal;

    cos_angle_of_incidence = dot(reflect_direction, lightDirection);

    if(cos_angle_of_incidence > 0.0)
    {
        if((finish->Phong_Size < 60) || (cos_angle_of_incidence > 0.0008)) // rgs
        {
            intensity = finish->Phong * pow(cos_angle_of_incidence, (double)finish->Phong_Size);

            if ((finish->Fresnel != 0.0) || (finish->Metallic != 0.0))
            {
                MathColour cs(1.0);
                // NB: The following dot product should technically be done with the vector halfway
                // between the in- and outgoing direction, rather than, the surface normal.
                // But since the Phong model specifically takes shortcuts to not compute that
                // vector, we'll take the surface normal as an approximation. Also, the choice to
                // compute the dot with the light rather than viewing direction is arbitrary.
                double ndotl = dot(layer_normal, lightDirection);
                if (finish->Fresnel != 0.0)
                    // In specular reflections (which includes highlights), the Fresnel effect is
                    // that we only get the reflected component.
                    cs *= finish->Fresnel * FresnelR(ndotl, relativeIor);
                ComputeMetallic(cs, finish->Metallic, layer_pigment_colour, ndotl);
                colour += intensity * light_colour * cs;
            }
            else
                colour += intensity * light_colour;
        }
    }
}

void Trace::ComputeSpecularColour(const FINISH *finish, const Vector3d& lightDirection, const Vector3d& reyeDirection, const Vector3d& layer_normal, MathColour& colour,
                                  const MathColour& light_colour, const MathColour& layer_pigment_colour, double relativeIor)
{
    double cos_angle_of_incidence, intensity, halfway_length;
    Vector3d halfway;

    halfway = (reyeDirection + lightDirection) * 0.5;

    halfway_length = halfway.length();

    if(halfway_length > 0.0)
    {
        cos_angle_of_incidence = dot(halfway, layer_normal) / halfway_length;

        if(cos_angle_of_incidence > 0.0)
        {
            intensity = finish->Specular * pow(cos_angle_of_incidence, (double)finish->Roughness);

            if ((finish->Fresnel != 0.0) || (finish->Metallic != 0.0))
            {
                MathColour cs(1.0);
                double ndotl = dot(halfway, lightDirection) / halfway_length;
                if (finish->Fresnel != 0.0)
                    // In specular reflections (which includes highlights), the Fresnel effect is
                    // that we only get the reflected component.
                    cs *= finish->Fresnel * FresnelR(ndotl, relativeIor);
                ComputeMetallic(cs, finish->Metallic, layer_pigment_colour, ndotl);
                colour += intensity * light_colour * cs;
            }
            else
                colour += intensity * light_colour;
        }
    }
}

void Trace::ComputeRelativeIOR(const Ray& ray, const Interior *interior, double& ior)
{
    // Get ratio of iors depending on the interiors the ray is traversing.
    if (interior == nullptr)
    {
        // TODO VERIFY - is this correct?
        ior = 1.0;
    }
    else
    {
        if(ray.GetInteriors().empty())
            // The ray is entering from the atmosphere.
            ior = interior->IOR / sceneData->atmosphereIOR;
        else
        {
            // The ray is currently inside an object.
            if(ray.IsInterior(interior) == true)
            {
                if(ray.GetInteriors().size() == 1)
                    // The ray is leaving into the atmosphere.
                    ior = sceneData->atmosphereIOR / interior->IOR;
                else
                    // The ray is leaving into another object.
                    ior = ray.GetInteriors().back()->IOR / interior->IOR;
            }
            else
                // The ray is entering a new object.
                ior = interior->IOR / ray.GetInteriors().back()->IOR;
        }
    }
}

void Trace::ComputeReflectivity(double& weight, MathColour& reflectivity, const MathColour& reflection_max, const MathColour& reflection_min,
                                bool fresnel, double reflection_falloff, double cos_angle, double relativeIor)
{
    double temp_Weight_Min, temp_Weight_Max;
    double reflection_Frac;

    if (fresnel == false)
    {
        temp_Weight_Max = reflection_max.WeightMax();
        temp_Weight_Min = reflection_min.WeightMax();
        weight = weight * max(temp_Weight_Max, temp_Weight_Min);

        if(fabs(reflection_falloff - 1.0) > EPSILON)
            reflection_Frac = pow(1.0 - cos_angle, reflection_falloff);
        else
            reflection_Frac = 1.0 - cos_angle;

        if(fabs(reflection_Frac) < EPSILON)
            reflectivity = reflection_min;
        else if (fabs(reflection_Frac - 1.0)<EPSILON)
            reflectivity = reflection_max;
        else
            reflectivity = reflection_Frac * reflection_max + (1.0 - reflection_Frac) * reflection_min;
    }
    else
    {
        ComputeFresnel(reflectivity, reflection_max, reflection_min, cos_angle, relativeIor);
        weight = weight * reflectivity.WeightMax();
    }
}

void Trace::ComputeMetallic(MathColour& colour, double metallic, const MathColour& metallicColour, double cosAngle)
{
    // Calculate the reflected color by interpolating between
    // the light source color and the surface color according
    // to the (empirical) Fresnel reflectivity function. [DB 9/94]
    if(metallic != 0.0)
    {
        double x = fabs(acos(cosAngle)) / M_PI_2;
        double F = 0.014567225 / Sqr(x - 1.12) - 0.011612903;
        F = min(1.0, max(0.0, F));

        colour *= (1.0 + (metallic * (1.0 - F)) * (metallicColour - 1.0));
    }
}

void Trace::ComputeFresnel(MathColour& reflectivity, const MathColour& rMax, const MathColour& rMin, double cos_angle, double ior)
{
    double f = FresnelR(cos_angle, ior);
    reflectivity = f * rMax + (1.0 - f) * rMin;
}

double Trace::FresnelR(double cosTi, double n)
{
    // NB: This is a special case of the Fresnel formula, presuming that incident light is unpolarized.
    //
    // The implemented formula is as follows:
    //
    //      1     / g - cos Ti \ 2    /     / cos Ti (g + cos Ti) - 1 \ 2 \
    // R = --- * ( ------------ )  * ( 1 + ( ------------------------- )   )
    //      2     \ g + cos Ti /      \     \ cos Ti (g - cos Ti) + 1 /   /
    //
    // where
    //
    //        /---------------------------
    // g = -\/ (n1/n2)^2 + (cos Ti)^2 - 1

    double sqrg = Sqr(n) + Sqr(cosTi) - 1.0;

    if(sqrg <= 0.0)
        // Total reflection.
        return 1.0;

    double g = sqrt(sqrg);

    double quot1 = (g - cosTi) / (g + cosTi);
    double quot2 = (cosTi * (g + cosTi) - 1.0) / (cosTi * (g - cosTi) + 1.0);

    double f = 0.5 * Sqr(quot1) * (1.0 + Sqr(quot2));

    return clip(f, 0.0, 1.0);
}

void Trace::ComputeOneWhiteLightRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray, const Vector3d& ipoint, const Vector3d& jitter)
{
    Vector3d center = lightsource.Center + jitter;
    double a;
    Vector3d v1;

    // Get the light ray starting at the intersection point and pointing towards the light source.
    lightsourceray.Origin = ipoint;
    // NK 1998 parallel beams for cylinder source - added 'if'
    if(lightsource.Light_Type == CYLINDER_SOURCE)
    {
        double distToPointsAt;
        Vector3d toLightCtr;

        // use new code to get ray direction - use center - points_at for direction
        lightsourceray.Direction = center - lightsource.Points_At;

        // get vector pointing to center of light
        toLightCtr = center - ipoint;

        // project light_ctr-intersect_point onto light_ctr-point_at
        distToPointsAt = lightsourceray.Direction.length();
        lightsourcedepth = dot(toLightCtr, lightsourceray.Direction);

        // lenght of shadow ray is the length of the projection
        lightsourcedepth /= distToPointsAt;
        lightsourceray.Direction.normalize(); // TODO - lightsourceray.Direction /= distToPointsAt  should also work
    }
    else
    {
        // NK 1998 parallel beams for cylinder source - the stuff in this 'else'
        // block used to be all that there was... the first half of the if
        // statement (before the 'else') is new
        lightsourceray.Direction = center - ipoint;
        lightsourcedepth = lightsourceray.Direction.length();
        lightsourceray.Direction /= lightsourcedepth;
    }

    // Attenuate light source color.
    // Attenuation = Attenuate_Light(lightsource, lightsourceray, *Light_Source_Depth);
    // Recalculate for Parallel light sources
    if(lightsource.Parallel)
    {
        if(lightsource.Area_Light)
        {
            v1 = (center - lightsource.Points_At).normalized();
            a = dot(v1, lightsourceray.Direction);
            lightsourcedepth *= a;
            lightsourceray.Direction = v1;
        }
        else
        {
            a = dot(lightsource.Direction, lightsourceray.Direction);
            lightsourcedepth *= (-a);
            lightsourceray.Direction = -lightsource.Direction;
        }
    }
}

void Trace::ComputeSky(const Ray& ray, MathColour& colour, ColourChannel& transm)
{
    if (sceneData->EffectiveLanguageVersion() < 370)
    {
        // this gives the same results regarding sky sphere filter as how v3.6 did it
        // NB the use of the RGBFTColour data type here is intentional, and required to achieve backward compatibility

        ColourChannel filter;
        double att, trans;
        MathColour col;
        TransColour col_Temp;
        MathColour filterc_colour;
        ColourChannel filterc_filter;
        ColourChannel filterc_transm;
        Vector3d p;

        if (ray.GetTicket().alphaBackground)
        {
            // If rendering with alpha channel, just return full transparency.
            // (As we're working with associated alpha internally, the respective color must be black here.)
            colour.Clear();
            transm = 1.0;
            return;
        }

        colour = sceneData->backgroundColour.colour();
        sceneData->backgroundColour.GetFT(filter, transm);

        if (sceneData->skysphere == nullptr)
            return;

        col.Clear();
        filterc_colour = MathColour(1.0);
        filterc_filter = 1.0;
        filterc_transm = 1.0;
        trans = 1.0;

        // Transform point on unit sphere.
        if (sceneData->skysphere->Trans != nullptr)
            MInvTransPoint(p, ray.Direction, sceneData->skysphere->Trans);
        else
            p = ray.Direction;

        // TODO - Reverse iterator may be less performant than forward iterator; we might want to
        //        compare performance with using forward iterators and decrement, or using random access.
        //        Alternatively, reversing the vector after parsing might be another option.
        for(vector<PIGMENT*>::const_reverse_iterator i = sceneData->skysphere->Pigments.rbegin(); i != sceneData->skysphere->Pigments.rend(); ++ i)
        {
            // Compute sky colour from colour map.

            Compute_Pigment(col_Temp, *i, p, nullptr, nullptr, threadData);

            att = trans * col_Temp.Opacity();

            col += col_Temp.colour() * att;

            RGBFTColour col_Temp2 = ToRGBFTColour(col_Temp);
            filterc_colour *= col_Temp.colour();
            filterc_filter *= col_Temp2.filter();
            filterc_transm *= col_Temp2.transm();

            trans = fabs(filterc_filter) + fabs(filterc_transm);
        }

        col *= sceneData->skysphere->Emission;

        MathColour transColour = filterc_colour * filterc_filter + filterc_transm;
        colour = colour * transColour + col;
        transm *= filterc_transm;
    }
    else // i.e. sceneData->EffectiveLanguageVersion() >= 370
    {
        // this gives the same results regarding sky sphere filter as a layered-texture genuine sphere

        MathColour filCol(1.0);
        double att;
        MathColour col;
        TransColour col_Temp;
        Vector3d p;

        col.Clear();

        if (sceneData->skysphere != nullptr)
        {
            // Transform point on unit sphere.
            if (sceneData->skysphere->Trans != nullptr)
                MInvTransPoint(p, ray.Direction, sceneData->skysphere->Trans);
            else
                p = ray.Direction;

            // TODO - Reverse iterator may be less performant than forward iterator; we might want to
            //        compare performance with using forward iterators and decrement, or using random access.
            //        Alternatively, reversing the vector after parsing might be another option.
            for(vector<PIGMENT*>::const_reverse_iterator i = sceneData->skysphere->Pigments.rbegin(); i != sceneData->skysphere->Pigments.rend(); ++ i)
            {
                // Compute sky colour from colour map.
                Compute_Pigment(col_Temp, *i, p, nullptr, nullptr, threadData);

                att = col_Temp.Opacity();

                col += col_Temp.colour() * att * filCol * sceneData->skysphere->Emission;
                filCol *= col_Temp.TransmittedColour();
            }
        }

        // apply background as if it was another sky sphere with uniform pigment
        col_Temp = sceneData->backgroundColour;
        if (!ray.GetTicket().alphaBackground)
        {
            // if rendering without alpha channel, ignore filter and transmit of background color.
            col_Temp.SetFT(0.0, 0.0);
        }

        att = col_Temp.Opacity();

        col += col_Temp.colour() * att * filCol;
        filCol *= col_Temp.TransmittedColour();

        colour = col;
        transm = min(1.0f, fabs(filCol.Greyscale()));
    }
}

void Trace::ComputeFog(const Ray& ray, const Intersection& isect, MathColour& colour, ColourChannel& transm)
{
    double att, width;
    MathColour col_fog;
    ColourChannel filter_fog, transm_fog;
    MathColour sum_att; // total attenuation.
    MathColour sum_col; // total color.

    // Why are we here.
    if (sceneData->fog == nullptr)
        return;

    // Init total attenuation and total color.
    sum_att = MathColour(1.0);
    sum_col = MathColour(0.0);

    // Loop over all fogs.
    for (FOG *fog = sceneData->fog; fog != nullptr; fog = fog->Next)
    {
        // Don't care about fogs with zero distance.
        if(fabs(fog->Distance) > EPSILON)
        {
            width = isect.Depth;

            switch(fog->Type)
            {
                case GROUND_MIST:
                    att = ComputeGroundFogDepth(ray, 0.0, width, fog);
                    break;
                default:
                    att = ComputeConstantFogDepth(ray, 0.0, width, fog);
                    break;
            }

            col_fog = fog->colour.colour();
            fog->colour.GetFT(filter_fog, transm_fog);

            // Check for minimum transmittance.
            if(att < transm_fog)
                att = transm_fog;

            // Get attenuation sum due to filtered/unfiltered translucency.
            // [CLi] removed computation of sum_att.filer() and sum_att.transm(), as they were discarded anyway
            sum_att *= att * ((1.0 - filter_fog) + filter_fog * col_fog);

            if(!ray.IsShadowTestRay())
                sum_col += (1.0 - att) * col_fog;
        }
    }

    // Add light coming from background.
    colour = sum_col + sum_att * colour;
    transm *= sum_att.Greyscale();
}

double Trace::ComputeConstantFogDepth(const Ray &ray, double depth, double width, const FOG *fog)
{
    Vector3d p;
    double k;

    if (fog->Turb != nullptr)
    {
        depth += width / 2.0;

        p = ray.Evaluate(depth);
        p *= fog->Turb->Turbulence;

        // The further away the less influence turbulence has.
        k = exp(-width / fog->Distance);

        width *= (1.0 - k * min(1.0, Turbulence(p, fog->Turb, sceneData->noiseGenerator) * fog->Turb_Depth));
    }

    return (exp(-width / fog->Distance));
}

/*****************************************************************************
*   Here is an ascii graph of the ground fog density, it has a maximum
*   density of 1.0 at Y <= 0, and approaches 0.0 as Y goes up:
*
*   ***********************************
*        |           |            |    ****
*        |           |            |        ***
*        |           |            |           ***
*        |           |            |            | ****
*        |           |            |            |     *****
*        |           |            |            |          *******
*   -----+-----------+------------+------------+-----------+-----
*       Y=-2        Y=-1         Y=0          Y=1         Y=2
*
*   ground fog density is 1 / (Y*Y+1) for Y >= 0 and equals 1.0 for Y <= 0.
*   (It behaves like regular fog for Y <= 0.)
*
*   The integral of the density is atan(Y) (for Y >= 0).
******************************************************************************/

double Trace::ComputeGroundFogDepth(const Ray& ray, double depth, double width, const FOG *fog)
{
    double fog_density, delta;
    double start, end;
    double y1, y2, k;
    Vector3d p, p1, p2;

    // Get start point.
    p1 = ray.Evaluate(depth);

    // Get end point.
    p2 = p1 + ray.Direction * width;

    // Could preform transfomation here to translate Start and End
    // points into ground fog space.
    y1 = dot(p1, fog->Up);
    y2 = dot(p2, fog->Up);

    start = (y1 - fog->Offset) / fog->Alt;
    end   = (y2 - fog->Offset) / fog->Alt;

    // Get integral along y-axis from start to end.
    if(start <= 0.0)
    {
        if(end <= 0.0)
            fog_density = 1.0;
        else
            fog_density = (atan(end) - start) / (end - start);
    }
    else
    {
        if(end <= 0.0)
            fog_density = (atan(start) - end) / (start - end);
        else
        {
            delta = start - end;

            if(fabs(delta) > EPSILON)
                fog_density = (atan(start) - atan(end)) / delta;
            else
                fog_density = 1.0 / (Sqr(start) + 1.0);
        }
    }

    // Apply turbulence.
    if (fog->Turb != nullptr)
    {
        p = (p1 + p2) * 0.5;
        p *= fog->Turb->Turbulence;

        // The further away the less influence turbulence has.
        k = exp(-width / fog->Distance);
        width *= (1.0 - k * min(1.0, Turbulence(p, fog->Turb, sceneData->noiseGenerator) * fog->Turb_Depth));
    }

    return (exp(-width * fog_density / fog->Distance));
}

void Trace::ComputeShadowMedia(Ray& light_source_ray, Intersection& isect, MathColour& resultcolour, bool media_attenuation_and_interaction)
{
    if(resultcolour.IsNearZero(EPSILON))
        return;

    // Calculate participating media effects.
    if (media_attenuation_and_interaction && qualityFlags.media &&
        ((light_source_ray.IsHollowRay() == true) || ((isect.Object != nullptr) && (isect.Object->interior != nullptr))))
    {
        // we're using general-purpose media and fog handling code, which insists on computing a transmissive component (for alpha channel)
        ColourChannel tmpTransm = 0.0;
        media.ComputeMedia(sceneData->atmosphere, light_source_ray, isect, resultcolour, tmpTransm);

        if ((sceneData->fog != nullptr) && (light_source_ray.IsHollowRay() == true) && (light_source_ray.IsPhotonRay() == false))
            ComputeFog(light_source_ray, isect, resultcolour, tmpTransm);

        // discard the transmissive component (alpha channel)
    }

    // If ray is entering from the atmosphere or the ray is currently *not* inside an object add it,
    // but if it is currently inside an object, the ray is leaving the current object and is removed
    if ((isect.Object != nullptr) && ((light_source_ray.GetInteriors().empty()) || (light_source_ray.RemoveInterior(isect.Object->interior.get()) == false)))
        light_source_ray.AppendInterior(isect.Object->interior.get());
}



void Trace::ComputeRainbow(const Ray& ray, const Intersection& isect, MathColour& colour, ColourChannel& transm)
{
    int n;
    double dot1, k, ki, index, x, y, l, angle, fade, f;
    Vector3d Temp;
    TransColour Cr;
    ColourChannel CrFilter, CrTransm;
    MathColour CtColour;
    ColourChannel CtFilter, CtTransm;

    // Why are we here.
    if (sceneData->rainbow == nullptr)
        return;

    // TODO - get rid of the use of the RGBFT colour model

    CtColour = MathColour(0.0);
    CtFilter = 1.0;
    CtTransm = 1.0;

    n = 0;

    for (RAINBOW *Rainbow = sceneData->rainbow; Rainbow != nullptr; Rainbow = Rainbow->Next)
    {
        if ((Rainbow->Pigment != nullptr) && (Rainbow->Distance != 0.0) && (Rainbow->Width != 0.0))
        {
            // Get angle between ray direction and rainbow's up vector.
            x = dot(ray.Direction, Rainbow->Right_Vector);
            y = dot(ray.Direction, Rainbow->Up_Vector);

            l = Sqr(x) + Sqr(y);

            if(l > 0.0)
            {
                l = sqrt(l);
                y /= l;
            }

            angle = fabs(acos(y));

            if(angle <= Rainbow->Arc_Angle)
            {
                // Get dot product between ray direction and antisolar vector.
                dot1 = dot(ray.Direction, Rainbow->Antisolar_Vector);

                if(dot1 >= 0.0)
                {
                    // Get index ([0;1]) into rainbow's colour map.
                    index = (acos(dot1) - Rainbow->Angle) / Rainbow->Width;

                    // Jitter index.
                    if(Rainbow->Jitter > 0.0)
                        index += (2.0 * randomNumberGenerator() - 1.0) * Rainbow->Jitter;

                    if((index >= 0.0) && (index <= 1.0 - EPSILON))
                    {
                        // Get colour from rainbow's colour map.
                        Temp = Vector3d(index, 0.0, 0.0);
                        Compute_Pigment(Cr, Rainbow->Pigment, Temp, &isect, &ray, threadData);
                        Cr.GetFT(CrFilter, CrTransm);

                        // Get fading value for falloff.
                        if((Rainbow->Falloff_Width > 0.0) && (angle > Rainbow->Falloff_Angle))
                        {
                            fade = (angle - Rainbow->Falloff_Angle) / Rainbow->Falloff_Width;
                            fade = (3.0 - 2.0 * fade) * fade * fade;
                        }
                        else
                            fade = 0.0;

                        // Get attenuation factor due to distance.
                        k = exp(-isect.Depth / Rainbow->Distance);

                        // Colour's transm value is used as minimum attenuation value.
                        k = max(k, fade * (1.0 - CrTransm) + CrTransm);

                        // Now interpolate the colours.
                        ki = 1.0 - k;

                        // Attenuate filter value.
                        f = CrFilter * ki;

                        CtColour += k * colour * ((1.0 - f) + f * Cr.colour()) + ki * Cr.colour();
                        CtFilter *= k * CrFilter;
                        CtTransm *= k * CrTransm;

                        n++;
                    }
                }
            }
        }
    }

    if(n > 0)
    {
        colour =  CtColour / n;
        transm *= CtTransm;
    }
}

bool Trace::TestShadow(const LightSource &lightsource, double& depth, Ray& light_source_ray, const Vector3d& p, MathColour& colour)
{
    ComputeOneLightRay(lightsource, depth, light_source_ray, p, colour);

    // There's no need to test for shadows if no light
    // is coming from the light source.
    //
    // Test for PURE zero, because we only want to skip this if we're out
    // of the range of a spot light or cylinder light.  Very dim lights
    // should not be ignored.

    if(colour.IsNearZero(EPSILON))
    {
        colour.Clear();
        return true;
    }

    // Test for shadows.
    if (qualityFlags.shadows && ((lightsource.Projected_Through_Object != nullptr) || (lightsource.Light_Type != FILL_LIGHT_SOURCE)))
    {
        TraceShadowRay(lightsource, depth, light_source_ray, p, colour);

        if(colour.IsNearZero(EPSILON))
        {
            colour.Clear();
            return true;
        }
    }

    return false;
}

bool Trace::IsObjectInCSG(ConstObjectPtr object, ConstObjectPtr parent)
{
    bool found = false;

    if(object == parent)
        return true;

    if(parent->Type & IS_COMPOUND_OBJECT)
    {
        for(vector<ObjectPtr>::const_iterator Sib = (reinterpret_cast<const CSG *>(parent))->children.begin(); Sib != (reinterpret_cast<const CSG *>(parent))->children.end(); Sib++)
        {
            if(IsObjectInCSG(object, *Sib))
                found = true;
        }
    }

    return found;
}

// SSLT code by Sarah Tariq and Lawrence (Lorenzo) Ibarria

double Trace::ComputeFt(double phi, double eta)
{
#if 0
    double sin_phi   = sin(phi);
    double sin_theta = sin_phi / eta;
    if ((sin_theta < -1.0) || (sin_theta > 1.0))
        return 0; // total reflection, i.e. no transmission at all

    double  theta = asin(sin_theta);

    return 1 - 0.5 * (Sqr(sin(phi-theta)) / Sqr(sin(phi+theta)) + Sqr(tan(phi-theta)) / Sqr(tan(phi+theta)));
#else
    double cos_angle = cos(phi);
    double g = sqrt(Sqr(eta) + Sqr(cos_angle) - 1);
    double F = 0.5 * (Sqr(g - cos_angle) / Sqr(g + cos_angle));
    F = F * (1 + Sqr(cos_angle * (g + cos_angle) - 1) / Sqr(cos_angle * (g - cos_angle) + 1));

    return 1.0 - min(1.0,max(0.0,F));
#endif
}

void Trace::ComputeSurfaceTangents(const Vector3d& normal, Vector3d& u, Vector3d& v)
{
#if 1
    if (fabs(normal[0]) <= fabs(normal[1]) && fabs(normal[0]) <= fabs(normal[2]))
        // if x co-ordinate is smallest, creating a tangent in the yz plane is a piece of cake;
        // the following code is equivalent to u = cross(normal, Vector3d(1,0,0)).normalized();
        u = Vector3d(0, normal[2], -normal[1]).normalized();
    else if (fabs(normal[1]) <= fabs(normal[2]))
        // if y co-ordinate is smallest, creating a tangent in the xz plane is a piece of cake;
        // the following code is equivalent to u = cross(normal, Vector3d(0,1,0)).normalized();
        u = Vector3d(-normal[2], 0, normal[0]).normalized();
    else
        // if z co-ordinate is smallest, creating a tangent in the xy plane is a piece of cake;
        // the following code is equivalent to u = cross(normal, Vector3d(0,0,1)).normalized();
        u = Vector3d(normal[1], -normal[0], 0).normalized();
#else
    if (fabs(normal[0]) <= fabs(normal[1]) && fabs(normal[0]) <= fabs(normal[2]))
        // if x co-ordinate is smallest, creating a tangent in the yz plane is a piece of cake
        u = cross(normal, Vector3d(1,0,0)).normalized();
    else if (fabs(normal[1]) <= fabs(normal[0]) && fabs(normal[1]) <= fabs(normal[2]))
        // if y co-ordinate is smallest, creating a tangent in the xz plane is a piece of cake
        u = cross(normal, Vector3d(0,1,0)).normalized();
    else
        // if z co-ordinate is smallest, creating a tangent in the xy plane is a piece of cake
        u = cross(normal, Vector3d(0,0,1)).normalized();
#endif

    v = cross(normal, u);
}

void Trace::ComputeSSLTNormal(Intersection& Ray_Intersection)
{
    Vector3d Raw_Normal;

    /* Get the normal to the surface */
    Ray_Intersection.Object->Normal(Raw_Normal, &Ray_Intersection, threadData);
    Ray_Intersection.INormal = Raw_Normal;
    Ray_Intersection.PNormal = Raw_Normal; // TODO FIXME - we should possibly take normal pertubation into account
}

bool Trace::IsSameSSLTObject(ConstObjectPtr obj1, ConstObjectPtr obj2)
{
    // TODO maybe use something smarter
    return (obj1 && obj2 && obj1->interior == obj2->interior);
}

void Trace::ComputeDiffuseSampleBase(Vector3d& basePoint, const Intersection& out, const Vector3d& vOut, double avgFreeDist, TraceTicket& ticket)
{
    Vector3d pOut = out.IPoint;
    Vector3d nOut = out.INormal;

    // make sure to get the normal right; obviously, the observer must be "outside".
    // for algorithm simplicity, we want the normal to point inward
    double cos_phi = dot(nOut, vOut);
    if (cos_phi > 0)
        nOut.invert();

    // typically, place the base point the average free distance below the surface;
    // however, never place it closer to the "back side" than to the front
    Intersection backSide;
    Ray ray(ticket, pOut, nOut); // we're shooting from the surface, so SubsurfaceRay would do us no good (as it would potentially "re-discover" the current surface)
    backSide.Depth = avgFreeDist * 2; // max distance we're looking at
    bool found = FindIntersection(backSide, ray);
    if (found)
    {
        if (IsSameSSLTObject(out.Object, backSide.Object))
            basePoint = pOut + nOut * (backSide.Depth / 2);
        else
            basePoint = pOut + nOut * min(avgFreeDist, backSide.Depth - EPSILON);
    }
    else
        basePoint = pOut + nOut * avgFreeDist;
}

void Trace::ComputeDiffuseSamplePoint(const Vector3d& basePoint, Intersection& in, double& sampleArea, TraceTicket& ticket)
{
    // generate a vector in a random direction
    // TODO FIXME - a suitably weighted distribution (oriented according to the surface normal) would possibly be better
    while (ssltUniformDirectionGenerator.size() <= ticket.subsurfaceRecursionDepth)
        ssltUniformDirectionGenerator.push_back(GetSubRandomDirectionGenerator(0, 32767));
    Vector3d v = (*(ssltUniformDirectionGenerator[ticket.subsurfaceRecursionDepth]))();

    Ray ray(ticket, basePoint, v, Ray::SubsurfaceRay);
    bool found = FindIntersection(in, ray);

    if (found)
    {
        ComputeSSLTNormal(in);

        Vector3d vDelta = in.IPoint - basePoint;
        double dist = vDelta.length();
        double cos_phi = abs(dot(vDelta / dist, in.INormal));
        if (cos_phi < 0)
        {
            in.INormal.invert();
            cos_phi = -cos_phi;
        }
        if (cos_phi < 0.01)
            cos_phi = 0.01; // TODO FIXME - rather arbitrary limit
        sampleArea = 4.0 * M_PI * Sqr(dist * sceneData->mmPerUnit) / cos_phi;
    }
    else
    {
        sampleArea = 0.0;
    }
}

void Trace::ComputeOneSingleScatteringContribution(const LightSource& lightsource, const Intersection& out, double sigma_t_xo, double sigma_s, double s_prime_out,
                                                   MathColour& Lo, double eta, const Vector3d& bend_point, double phi_out, double cos_out_prime, TraceTicket& ticket)
{
    // TODO FIXME - part of this code is very alike to ComputeOneDiffuseLight()

    // Do Light source to get the correct lightsourceray
    // (note that for now we're mainly interested in the direction)
    Ray lightsourceray(ticket, Ray::SubsurfaceRay);
    double lightsourcedepth;
    ComputeOneWhiteLightRay(lightsource, lightsourcedepth, lightsourceray, bend_point);

    // We're below the surface; determine where a light ray from the source would be intersecting this object's surface
    // (and, more importantly, what the surface normal is there; notice that this intersection is an approximation,
    // ignoring refraction)
    Intersection xi;
    if (!FindIntersection(xi, lightsourceray))
        return;

    if (!IsSameSSLTObject(xi.Object, out.Object))
        return; // TODO - what if the other object is transparent?

    ComputeSSLTNormal(xi);

    // Get a colour and a ray (also recomputes all the lightsourceray stuff).
    MathColour lightcolour;
    ComputeOneLightRay(lightsource, lightsourcedepth, lightsourceray, xi.IPoint, lightcolour, true);

    // Don't calculate spotlights when outside of the light's cone.
    if(lightcolour.IsNearZero(EPSILON))
        return;

    // See if light on far side of surface from camera.
    // [CLi] double_illuminate and diffuse backside illumination don't seem to make much sense with BSSRDF, so we ignore them here.
    // [CLi] BSSRDF always does "full area lighting", so we ignore it here.
    double cos_in = dot(xi.INormal, lightsourceray.Direction);
    // [CLi] we're coming from inside the object, so the surface /must/ be properly oriented towards the camera; if it isn't,
    // it must be the normal's fault
    if (cos_in < 0)
    {
        xi.INormal.invert();
        cos_in = -cos_in;
    }
    // [CLi] light coming in almost parallel to the surface is a problem though
    if(cos_in < EPSILON)
        return;

    // If light source was not blocked by any intervening object, then
    // calculate it's contribution to the object's overall illumination.
    if (qualityFlags.shadows && ((lightsource.Projected_Through_Object != nullptr) || (lightsource.Light_Type != FILL_LIGHT_SOURCE)))
    {
        // [CLi] Not using lightColorCache because it's unsuited for BSSRDF
        TraceShadowRay(lightsource, lightsourcedepth, lightsourceray, xi.IPoint, lightcolour);
    }

    // Don't calculate anything more if we're in full shadow
    if(lightcolour.IsNearZero(EPSILON))
        return;

    double sigma_t_xi = sigma_t_xo; // TODO FIXME - theoretically this should be taken from point where light comes in

    double cos_in_sqr       = Sqr(cos_in);
    double sin_in_sqr       = 1 - cos_in_sqr;
    double eta_sqr          = Sqr(eta);
    double sin_in_prime_sqr = sin_in_sqr / eta_sqr;
    double cos_in_prime_sqr = 1 - sin_in_prime_sqr;
    if (cos_in_prime_sqr < 0.0)
        return; // total reflection
    double cos_in_prime  = sqrt(cos_in_prime_sqr);

    if (cos_in_prime <= EPSILON)
        return; // close enough to total reflection to give us trouble

    //MathColour lightColour = MathColour(lightsource.colour);
    lightcolour *= cos_in; // TODO VERIFY - is this right? Where does this term come from??

    // compute si
    double si = (bend_point - xi.IPoint).length() * sceneData->mmPerUnit;

    // calculate s_prime_i
    double s_prime_i = si * cos_in / cos_in_prime;

    // calculate F
    double phi_in  = acos(cos_in);
    double F = ComputeFt(phi_in, eta) * ComputeFt(phi_out, eta);

    // calculate sigma_tc
    double G = fabs(cos_out_prime / cos_in_prime); // TODO FIXME - theoretically this is only valid for comparatively flat surfaces
    double sigma_tc = sigma_t_xo + G * sigma_t_xi;

    // calculate the phase function
    // NOTE: We're leaving out the 1/pi factor because in POV-Ray, by convention,
    // light intensity is normalized to imply this factor already.
    double p = 1.0 / 4.0; // asume isotropic scattering (normally this would be 1/(4*M_PI))

    // multiply with the e terms
    double eTerms = exp(-s_prime_i * sigma_t_xi) * exp(-s_prime_out * sigma_t_xo);    // TODO FIXME - theoretically first sigma_t should be taken from from xi.IPoint

    double factor = (sigma_s * F * p / sigma_tc) * eTerms;
    if (factor >= DBL_MAX)
        factor = DBL_MAX;
    POV_SUBSURFACE_ASSERT((factor >= 0.0) && (factor <= DBL_MAX)); // verify factor is a non-negative, finite value (no #INF, no #IND, no #NAN)

    lightcolour *= factor;

    // add up the contribution
    Lo += lightcolour;
}

// call this once for each color
// out.INormal is calculated
void Trace::ComputeSingleScatteringContribution(const Intersection& out, double dist, double theta_out, double cos_out_prime, const Vector3d& refractedREye, double sigma_t_xo, double sigma_s, MathColour& Lo, double eta,
                                                TraceTicket& ticket)
{
    Lo.Clear();

    // calculate s_prime_out
    while (ssltUniformNumberGenerator.size() <= ticket.subsurfaceRecursionDepth)
        ssltUniformNumberGenerator.push_back(GetRandomDoubleGenerator(0.0, 1.0, 32767));
    double epsilon = 1.0 - (*(ssltUniformNumberGenerator[ticket.subsurfaceRecursionDepth]))(); // epsilon is a random floating point value in the range (0,1] (i.e. not including 0, but including 1)
    double s_prime_out = fabs(log(epsilon)) / sigma_t_xo;

    if (s_prime_out >= dist)
        return; // not within the object - this is covered by a "zero scattering" term instead

    //compute bend_point wihch is s_prime_out distance away on refractedREye
    Vector3d bend_point = out.IPoint + refractedREye * (s_prime_out / sceneData->mmPerUnit);

    // global light sources, if not turned off for this object
    if((out.Object->Flags & NO_GLOBAL_LIGHTS_FLAG) != NO_GLOBAL_LIGHTS_FLAG)
    {
        for(int i = 0; i < threadData->lightSources.size(); i++)
            ComputeOneSingleScatteringContribution(*threadData->lightSources[i], out, sigma_t_xo, sigma_s, s_prime_out, Lo, eta, bend_point, theta_out, cos_out_prime, ticket);
    }

    // local light sources from a light group, if any
    if(!out.Object->LLights.empty())
    {
        for(int i = 0; i < out.Object->LLights.size(); i++)
            ComputeOneSingleScatteringContribution(*out.Object->LLights[i], out, sigma_t_xo, sigma_s, s_prime_out, Lo, eta, bend_point, theta_out, cos_out_prime, ticket);
    }

    // TODO FIXME - radiosity should also be taken into account
}

bool Trace::SSLTComputeRefractedDirection(const Vector3d& v, const Vector3d& n, double eta, Vector3d& refracted)
{
    // Phi: angle between normal and -incoming_ray (REye in this case, since it points to Eye)
    // Theta: angle between -normal and outgoing_ray
    double          cosPhi;
    Vector3d        unitV = v.normalized();
    Vector3d        unitN = n.normalized();

    cosPhi = dot(unitV, unitN);
    if (cosPhi > 0)
        unitN.invert();
    else
        cosPhi = -cosPhi;

    double cosThetaSqr = 1.0 + Sqr(eta) * (Sqr(cosPhi) - 1.0);
    if (cosThetaSqr < 0.0)
        return false;

    double cosTheta = sqrt(cosThetaSqr);
    refracted = (unitV * eta + unitN * (eta * cosPhi - cosTheta)).normalized();

    return true;
}

void Trace::ComputeDiffuseContribution(const Intersection& out, const Vector3d& vOut, const Vector3d& pIn, const Vector3d& nIn, const Vector3d& vIn, double& sd, double sigma_prime_s, double sigma_a, double eta)
{
    // TODO FIXME - a great deal of this can be precomputed
    double  sigma_prime_t = sigma_prime_s + sigma_a;
    double  alpha_prime = sigma_prime_s / sigma_prime_t;
    double  F_dr = FresnelDiffuseReflectance(eta);
    double  Aconst = ((1 + F_dr) / (1 - F_dr));
    double  Rd;

    double  cos_phi_in  = clip(dot(vIn,  nIn),         -1.0, 1.0); // (clip values to not run into trouble due to petty precision issues)
    double  cos_phi_out = clip(dot(vOut, out.INormal), -1.0, 1.0);
    double  phi_in  = acos(cos_phi_in);
    double  phi_out = acos(cos_phi_out);

    double  F = ComputeFt(phi_in, eta) * ComputeFt(phi_out, eta);

#if 1
    // full BSSRDF model

    double  distSqr = (pIn - out.IPoint).lengthSqr() * Sqr(sceneData->mmPerUnit);

    double  Dconst = 1 / (3 * sigma_prime_t);
    double  sigma_tr = sqrt(3 * sigma_a * sigma_prime_t);

    double  z_r = 1.0 / sigma_prime_t;
    double  dSqr_r = Sqr(z_r) + distSqr;
    double  d_r = sqrt(dSqr_r);
    double  z_v = z_r * (1.0 + Aconst * 4.0/3.0);
    double  dSqr_v = Sqr(z_v) + distSqr;
    double  d_v = sqrt(dSqr_v);

    double common_term  = alpha_prime / (4.0 * M_PI);               // dimensionless
    double C1           = z_r * (sigma_tr + 1.0/d_r);               // dimensionless
    double C2           = z_v * (sigma_tr + 1.0/d_v);               // dimensionless
    double r_term       = C1 * exp(-sigma_tr * d_r) / dSqr_r;     // dimension 1/area
    double v_term       = C2 * exp(-sigma_tr * d_v) / dSqr_v;     // dimension 1/area

    Rd = common_term * (r_term + v_term);

#else
    // uniform illumination BRDF approximation
    // TODO - use this for radiosity?

    // calculate Rd
    double root_term = sqrt(3.0 * (1.0 - alpha_prime));
    Rd = (alpha_prime / 2.0) * (1 + exp((-4.0/3.0) * Aconst * root_term)) * exp(-root_term);

#endif

    // NOTE: We're leaving out the 1/pi factor because in POV-Ray, by convention,
    // light intensity is normalized to imply this factor already.
    sd = F * Rd; // (normally this would be F*Rd/M_PI)
    POV_SUBSURFACE_ASSERT((sd >= 0.0) && (sd <= DBL_MAX)); // verify sd is a non-negative, finite value (no #INF, no #IND, no #NAN)
}

void Trace::ComputeDiffuseContribution1(const LightSource& lightsource, const Intersection& out, const Vector3d& vOut, const Intersection& in, MathColour& Total_Colour,
                                        const PreciseMathColour& sigma_prime_s, const PreciseMathColour& sigma_a, double eta, double weight, TraceTicket& ticket)
{
    // TODO FIXME - part of this code is very alike to ComputeOneDiffuseLight()

    // Get a colour and a ray.
    Ray lightsourceray(ticket);
    double lightsourcedepth;
    MathColour lightcolour;
    ComputeOneLightRay(lightsource, lightsourcedepth, lightsourceray, in.IPoint, lightcolour, true);

    // Don't calculate spotlights when outside of the light's cone.
    if(lightcolour.IsNearZero(EPSILON))
        return;

    Vector3d nIn = in.INormal;

    // See if light on far side of surface from camera.
    // [CLi] double_illuminate and diffuse backside illumination don't seem to make much sense with BSSRDF, so we ignore them here.
    // [CLi] BSSRDF always does "full area lighting", so we ignore it here.
    double cos_in = dot(nIn, lightsourceray.Direction);
    // [CLi] we're coming from inside the object, so the surface /must/ be properly oriented towards the camera; if it isn't,
    // it must be the normal's fault
    if (cos_in < 0)
    {
        nIn.invert();
        cos_in = -cos_in;
    }
    // [CLi] light coming in almost parallel to the surface is a problem though
    if(cos_in < EPSILON)
        return;

    // If light source was not blocked by any intervening object, then
    // calculate it's contribution to the object's overall illumination.
    if (qualityFlags.shadows && ((lightsource.Projected_Through_Object != nullptr) || (lightsource.Light_Type != FILL_LIGHT_SOURCE)))
    {
        // [CLi] Not using lightColorCache because it's unsuited for BSSRDF
        TraceShadowRay(lightsource, lightsourcedepth, lightsourceray, in.IPoint, lightcolour);
    }

    // Don't calculate anything more if we're in full shadow
    if(lightcolour.IsNearZero(EPSILON))
        return;

    lightcolour *= cos_in;
    for (int j = 0; j < MathColour::channels; j++)
    {
        double sd;
        ComputeDiffuseContribution(out, vOut, in.IPoint, nIn, lightsourceray.Direction, sd, sigma_prime_s[j], sigma_a[j], eta);
        sd *= weight;
        POV_SUBSURFACE_ASSERT(sd >= 0);
        lightcolour[j] *= sd;
        Total_Colour[j] += lightcolour[j];
    }
}

void Trace::ComputeDiffuseAmbientContribution1(const Intersection& out, const Vector3d& vOut, const Intersection& in, MathColour& Total_Colour,
                                               const PreciseMathColour& sigma_prime_s, const PreciseMathColour& sigma_a, double eta, double weight, TraceTicket& ticket)
{
#if 0
    // generate a random direction vector (using a distribution cosine-weighted along the normal)
    Vector3d axisU, axisV;
    ComputeSurfaceTangents(in.INormal, axisU, axisV);
    while (ssltCosWeightedDirectionGenerator.size() <= ticket.subsurfaceRecursionDepth)
        ssltCosWeightedDirectionGenerator.push_back(GetSubRandomCosWeightedDirectionGenerator(2, 32767));
    Vector3d direction = (*(ssltCosWeightedDirectionGenerator[ticket.subsurfaceRecursionDepth]))();
    double cos_in = direction.y(); // cosine of angle between normal and random vector
    Vector3d vIn = in.INormal*cos_in + axisU*direction.x() + axisV*direction.z();

    POV_SUBSURFACE_ASSERT(fabs(dot(in.INormal, axisU)) < EPSILON);
    POV_SUBSURFACE_ASSERT(fabs(dot(in.INormal, axisV)) < EPSILON);
    POV_SUBSURFACE_ASSERT(fabs(dot(axisU, axisV)) < EPSILON);

    // [CLi] light coming in almost parallel to the surface is a problem
    if(cos_in < EPSILON)
        return;

    Ray ambientray = Ray(ticket, in.IPoint, vIn, Ray::OtherRay); // TODO FIXME - [CLi] check whether ray type is suitable
    MathColour ambientcolour;
    ColourChannel dummyTransm;
    TraceRay(ambientray, ambientcolour, dummyTransm, weight, false);

    // Don't calculate anything more if there's no light input
    if(ambientcolour.IsNearZero(EPSILON))
        return;

    for (int j = 0; j < 3; j++)
    {
        double sd;
        // Note: radiosity data is already cosine-weighted, so we're passing the surface normal as incident light direction
        ComputeDiffuseContribution(out, vOut, in.IPoint, in.INormal,  vIn, sd, sigma_prime_s[j], sigma_a[j], eta);
        sd *= 0.5/cos_in; // the distribution is cosine-weighted, but sd was computed assuming neutral weighting, so compensate
        sd *= weight;
        POV_SUBSURFACE_ASSERT(sd >= 0);
        ambientcolour[j] *= sd;
        POV_SUBSURFACE_ASSERT(ambientcolour[j] >= 0);
        Total_Colour[j] += ambientcolour[j];
    }
#else
    MathColour ambientcolour;
    // TODO FIXME - should support pertubed normals
    radiosity.ComputeAmbient(in.IPoint, in.INormal, in.INormal, 1.0 /* TODO - brilliance */, ambientcolour, weight, ticket);
    for (int j = 0; j < MathColour::channels; j++)
    {
        double sd;
        // Note: radiosity data is already cosine-weighted, so we're passing the surface normal as incident light direction
        ComputeDiffuseContribution(out, vOut, in.IPoint, in.INormal, in.INormal, sd, sigma_prime_s[j], sigma_a[j], eta);
        sd *= weight;
        POV_SUBSURFACE_ASSERT(sd >= 0);
        ambientcolour[j] *= sd;
        POV_SUBSURFACE_ASSERT(ambientcolour[j] >= 0);
        Total_Colour[j] += ambientcolour[j];
    }
#endif
}

void Trace::ComputeSubsurfaceScattering(const FINISH *Finish, const MathColour& layer_pigment_colour, const Intersection& out, Ray& Eye, const Vector3d& Layer_Normal, MathColour& Final_Colour, double Attenuation)
{
    int NumSamplesDiffuse = sceneData->subsurfaceSamplesDiffuse;
    int NumSamplesSingle  = sceneData->subsurfaceSamplesSingle;

    // TODO FIXME - this is hard-coded for now
    if (Eye.GetTicket().subsurfaceRecursionDepth >= 2)
        return;
    else if (Eye.GetTicket().subsurfaceRecursionDepth == 1)
    {
        NumSamplesDiffuse = 1;
        NumSamplesSingle  = 1;
        //NumSamplesDiffuse = (int)ceil(sqrt(NumSamplesDiffuse));
        //NumSamplesSingle  = (int)ceil(sqrt(NumSamplesSingle));
    }

    Eye.GetTicket().subsurfaceRecursionDepth++;

    LightSource Light_Source;

    Vector3d vOut = -Eye.Direction;

    MathColour Total_Colour;

    double eta;

    ComputeRelativeIOR(Eye, out.Object->interior.get(), eta);

#if 0
    // user setting specifies mean free path
    PreciseMathColour   alpha_prime     = object->interior->subsurface->GetReducedAlbedo(layer_pigment_colour * Finish->Diffuse);
    PreciseMathColour   sigma_tr        = 1.0 / PreciseMathColour(Finish->SubsurfaceTranslucency);

    PreciseMathColour   sigma_prime_t   = sigma_tr / sqrt(3*(1.0-alpha_prime));
    PreciseMathColour   sigma_prime_s   = alpha_prime * sigma_prime_t;
    PreciseMathColour   sigma_a         = sigma_prime_t - sigma_prime_s;
    PreciseMathColour   sigma_tr_sqr    = sigma_tr * sigma_tr;
#else
    // user setting specifies reduced scattering coefficient
    PreciseMathColour   alpha_prime     = out.Object->interior->subsurface->GetReducedAlbedo(layer_pigment_colour * Finish->Diffuse);
    PreciseMathColour   sigma_prime_s   = 1.0 / PreciseMathColour(Finish->SubsurfaceTranslucency);

    PreciseMathColour   sigma_prime_t   = sigma_prime_s / alpha_prime;
    PreciseMathColour   sigma_a         = sigma_prime_t - sigma_prime_s;
    PreciseMathColour   sigma_tr_sqr    = sigma_a * sigma_prime_t * 3.0;
    PreciseMathColour   sigma_tr        = Sqrt(sigma_tr_sqr);
#endif

    PreciseMathColour   g(0.0); // the mean cosine of the scattering angle; for isotropic scattering, g = 0
    PreciseMathColour   sigma_t_xo      = sigma_prime_t / (1.0-g);
    PreciseMathColour   sigma_s         = sigma_prime_s / (1.0-g);

#if 1

    // colour dependent diffuse contribution

    double      sampleArea;
    double      weight;
    double      weightSum;
    double      sigma_a_mean        = sigma_a.Greyscale(); // TODO FIXME - use a "fair" average of all three color channels
    double      sigma_prime_s_mean  = sigma_prime_s.Greyscale(); // TODO FIXME - use a "fair" average of all three color channels
    double      sigma_prime_t_mean  = sigma_a_mean + sigma_prime_s_mean;
    double      sigma_tr_mean_sqr   = sigma_a_mean * sigma_prime_t_mean * 3.0;
    double      sigma_tr_mean       = sqrt(sigma_tr_mean_sqr);
    int         trueNumSamples;

    bool radiosity_needed = (sceneData->radiositySettings.radiosityEnabled == true) &&
                            (sceneData->subsurfaceUseRadiosity == true) &&
                            (radiosity.CheckRadiosityTraceLevel(Eye.GetTicket()) == true) &&
                            (Test_Flag(out.Object, IGNORE_RADIOSITY_FLAG) == false);

    Vector3d sampleBase;
    ComputeDiffuseSampleBase(sampleBase, out, vOut, 1.0 / (sigma_prime_t_mean * sceneData->mmPerUnit), Eye.GetTicket());

    weightSum = 0.0;
    trueNumSamples = 0;

    for (int i = 0; i < NumSamplesDiffuse; i++)
    {
        Intersection in;
        ComputeDiffuseSamplePoint(sampleBase, in, sampleArea, Eye.GetTicket());

        // avoid pathological cases
        if (sampleArea != 0)
        {
            weight = sampleArea;
            weightSum += weight;
            trueNumSamples ++;

            if (IsSameSSLTObject(in.Object, out.Object))
            {
                // radiosity-alike ambient illumination
                if (radiosity_needed)
                    // shoot just one random ray to account for ambient illumination (we're averaging stuff anyway)
                    ComputeDiffuseAmbientContribution1(out, vOut, in, Total_Colour, sigma_prime_s, sigma_a, eta, weight, Eye.GetTicket());

                // global light sources, if not turned off for this object
                if((out.Object->Flags & NO_GLOBAL_LIGHTS_FLAG) != NO_GLOBAL_LIGHTS_FLAG)
                {
                    for(int k = 0; k < threadData->lightSources.size(); k++)
                        ComputeDiffuseContribution1(*threadData->lightSources[k], out, vOut, in, Total_Colour, sigma_prime_s, sigma_a, eta, weight, Eye.GetTicket());
                }

                // local light sources from a light group, if any
                if(!out.Object->LLights.empty())
                {
                    for(int k = 0; k < out.Object->LLights.size(); k++)
                        ComputeDiffuseContribution1(*out.Object->LLights[k], out, vOut, in, Total_Colour, sigma_prime_s, sigma_a, eta, weight, Eye.GetTicket());
                }
            }
            else
            {
                // TODO - what's the proper thing to do?
            }
        }
    }
    if (trueNumSamples > 0)
        Total_Colour /= trueNumSamples;

#endif

    Vector3d refractedEye;
    if (SSLTComputeRefractedDirection(Eye.Direction, out.INormal, 1.0/eta, refractedEye))
    {
        Ray refractedEyeRay(Eye.GetTicket(), out.IPoint, refractedEye);
        Intersection unscatteredIn;

        double dist;

        // find the intersection of the refracted ray with the object
        // find the distance to this intersection
        bool found = FindIntersection(unscatteredIn, refractedEyeRay);
        if (found)
            dist = (out.IPoint - unscatteredIn.IPoint).length() * sceneData->mmPerUnit;
        else
            dist = HUGE_VAL;

        double cos_out = dot(vOut, out.INormal);
        double cos_out_prime = sqrt(1 - ((Sqr(1.0 / eta)) * (1 - Sqr(cos_out))));
        double theta_out = acos(cos_out);

#if 1

        // colour dependent single scattering contribution

        for (int i = 0; i < NumSamplesSingle; i++)
        {
            for (int j = 0; j < MathColour::channels; j ++)
            {
                MathColour temp;
                ComputeSingleScatteringContribution(out, dist, theta_out, cos_out_prime, refractedEye, sigma_t_xo[j], sigma_s[j], temp, eta, Eye.GetTicket());
                Total_Colour[j] += temp[j] / NumSamplesSingle;
            }
        }

#endif

#if 1

        // colour dependent unscattered contribution

        // Trace refracted ray.
        MathColour tempColour;
        ColourChannel tempTransm;

        // TODO FIXME - account for fresnel attenuation at interfaces
        PreciseMathColour att = Exp(-sigma_prime_t * dist); // TODO should be sigma_t
        weight = att.WeightMax();
        if (weight > Eye.GetTicket().adcBailout)
        {
            if (!found)
            {
                // TODO - trace the ray to the background?
            }
            else if (IsSameSSLTObject(unscatteredIn.Object, out.Object))
            {
                unscatteredIn.Object->Normal(unscatteredIn.INormal, &unscatteredIn, threadData);
                if (dot(refractedEye, unscatteredIn.INormal) > 0)
                    unscatteredIn.INormal.invert();
                Vector3d doubleRefractedEye;
                if (SSLTComputeRefractedDirection(refractedEye, unscatteredIn.INormal, eta, doubleRefractedEye))
                {
                    Ray doubleRefractedEyeRay(refractedEyeRay);
                    doubleRefractedEyeRay.SetFlags(Ray::RefractionRay, refractedEyeRay);
                    doubleRefractedEyeRay.Origin = unscatteredIn.IPoint;
                    doubleRefractedEyeRay.Direction = doubleRefractedEye;
                    TraceRay(doubleRefractedEyeRay, tempColour, tempTransm, weight, false);
                    Total_Colour += MathColour(PreciseMathColour(tempColour) * att);
                }
            }
            else
            {
                // TODO - trace the ray into that object (if it is transparent)
            }
        }

#endif

    }

    Final_Colour += Total_Colour;

    Eye.GetTicket().subsurfaceRecursionDepth--;
}

}
// end of namespace pov
