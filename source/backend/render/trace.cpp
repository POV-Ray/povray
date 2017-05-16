/*******************************************************************************
 * trace.cpp
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
 * $File: //depot/public/povray/3.x/source/backend/render/trace.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <float.h>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/colour/colour.h"
#include "backend/math/vector.h"
#include "backend/math/matrices.h"
#include "backend/scene/objects.h"
#include "backend/pattern/pattern.h"
#include "backend/pattern/warps.h"
#include "backend/support/imageutil.h"
#include "backend/texture/normal.h"
#include "backend/texture/pigment.h"
#include "backend/texture/texture.h"
#include "backend/render/trace.h"
#include "backend/render/tracetask.h"
#include "backend/scene/scene.h"
#include "backend/scene/view.h"
#include "backend/lighting/point.h"
#include "backend/lighting/radiosity.h"
#include "backend/lighting/subsurface.h"
#include "backend/shape/csg.h"
#include "backend/shape/boxes.h"
#include "backend/support/bsptree.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

#define SHADOW_TOLERANCE 1.0e-3

#define MEDIA_AFTER_TEXTURE_INTERPOLATION 1

Trace::Trace(shared_ptr<SceneData> sd, TraceThreadData *td, unsigned int qf,
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
		*i = NULL;

	lightSourceOtherShadowCache.resize(max(1, (int) threadData->lightSources.size()));
	for(vector<ObjectPtr>::iterator i(lightSourceOtherShadowCache.begin()); i != lightSourceOtherShadowCache.end(); i++)
		*i = NULL;

	lightColorCache.resize(max(20U, sd->parsedMaxTraceLevel + 1));
	for(LightColorCacheListList::iterator it = lightColorCache.begin(); it != lightColorCache.end(); it++)
		it->resize(max(1, (int) threadData->lightSources.size()));

	if(sceneData->boundingMethod == 2)
		mailbox = BSPTree::Mailbox(sceneData->numberOfFiniteObjects);
}

Trace::~Trace()
{
}

double Trace::TraceRay(const Ray& ray, Colour& colour, COLC weight, TraceTicket& ticket, bool continuedRay, DBL maxDepth)
{
	Intersection bestisect;
	bool found;
	NoSomethingFlagRayObjectCondition precond;
	TrueRayObjectCondition postcond;

	POV_ULONG nrays = threadData->Stats()[Number_Of_Rays]++;
	if(ray.IsPrimaryRay() || (((unsigned char) nrays & 0x0f) == 0x00))
		cooperate();

	// Check for max. trace level or ADC bailout.
	if((ticket.traceLevel >= ticket.maxAllowedTraceLevel) || (weight < ticket.adcBailout))
	{
		if(weight < ticket.adcBailout)
			threadData->Stats()[ADC_Saves]++;

		colour.clear();
		return HUGE_VAL;
	}

	if (maxDepth >= EPSILON)
		bestisect.Depth = maxDepth;

	found = FindIntersection(bestisect, ray, precond, postcond);

	// Check if we're busy shooting too many radiosity sample rays at an unimportant object
	if (ticket.radiosityImportanceQueried >= 0.0)
	{
		if (found)
		{
			ticket.radiosityImportanceFound = bestisect.Object->RadiosityImportance(sceneData->radiositySettings.defaultImportance);
		}
		else
			ticket.radiosityImportanceFound = sceneData->radiositySettings.defaultImportance;

		if (ticket.radiosityImportanceFound < ticket.radiosityImportanceQueried)
		{
			if(found == false)
				return HUGE_VAL;
			else
				return bestisect.Depth;
		}
	}
	float oldRadiosityImportanceQueried = ticket.radiosityImportanceQueried;
	ticket.radiosityImportanceQueried = -1.0; // indicates that recursive calls to TraceRay() should not check for radiosity importance

	const bool traceLevelIncremented = !continuedRay;

	if(traceLevelIncremented)
	{
		// Set highest level traced.
		ticket.traceLevel++;
		ticket.maxFoundTraceLevel = (unsigned int) max(ticket.maxFoundTraceLevel, ticket.traceLevel);
	}

	if((qualityFlags & Q_VOLUME) && (ray.IsPhotonRay() == true) && (ray.IsHollowRay() == true))
	{
		// Note: this version of ComputeMedia does not deposit photons. This is
		// intentional.  Even though we're processing a photon ray, we don't want
		// to deposit photons in the infinite atmosphere, only in contained
		// media, which is processed later (in ComputeLightedTexture).  [nk]
		media.ComputeMedia(sceneData->atmosphere, ray, bestisect, colour, ticket);

		if(sceneData->fog != NULL)
			ComputeFog(ray, bestisect, colour);
	}

	if(found)
		ComputeTextureColour(bestisect, colour, ray, weight, false, ticket);
	else
		ComputeSky(ray, colour, ticket);

	if((qualityFlags & Q_VOLUME) && (ray.IsPhotonRay() == false) && (ray.IsHollowRay() == true))
	{
		if((sceneData->rainbow != NULL) && (ray.IsShadowTestRay() == false))
			ComputeRainbow(ray, bestisect, colour);

		media.ComputeMedia(sceneData->atmosphere, ray, bestisect, colour, ticket);

		if(sceneData->fog != NULL)
			ComputeFog(ray, bestisect, colour);
	}

	if(traceLevelIncremented)
		ticket.traceLevel--;
	maxFoundTraceLevel = (unsigned int) max(maxFoundTraceLevel, ticket.maxFoundTraceLevel);

	ticket.radiosityImportanceQueried = oldRadiosityImportanceQueried;

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
			if(sceneData->boundingSlabs != NULL)
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
			if(sceneData->boundingSlabs != NULL)
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
	if(object != NULL)
	{
		BBOX_VECT origin;
		BBOX_VECT invdir;
		ObjectBase::BBoxDirection variant;

		Vector3d tmp(1.0 / ray.GetDirection()[X], 1.0 / ray.GetDirection()[Y], 1.0 /ray.GetDirection()[Z]);
		Assign_Vector(origin, ray.Origin);
		Assign_Vector(invdir, *tmp);
		variant = (ObjectBase::BBoxDirection)((int(invdir[X] < 0.0) << 2) | (int(invdir[Y] < 0.0) << 1) | int(invdir[Z] < 0.0));

		if(object->Intersect_BBox(variant, origin, invdir, closest) == false)
			return false;

		if(object->Bound.empty() == false)
		{
			if(Ray_In_Bound(ray, object->Bound, threadData) == false)
				return false;
		}

		IStack depthstack(stackPool);
		assert(depthstack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

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

		assert(depthstack->empty()); // verify that the IStack is in a cleaned-up condition (again)
	}

	return false;
}

bool Trace::FindIntersection(ObjectPtr object, Intersection& isect, const Ray& ray, const RayObjectCondition& postcondition, double closest)
{
	if(object != NULL)
	{
		BBOX_VECT origin;
		BBOX_VECT invdir;
		ObjectBase::BBoxDirection variant;

		Vector3d tmp(1.0 / ray.GetDirection()[X], 1.0 / ray.GetDirection()[Y], 1.0 /ray.GetDirection()[Z]);
		Assign_Vector(origin, ray.Origin);
		Assign_Vector(invdir, *tmp);
		variant = (ObjectBase::BBoxDirection)((int(invdir[X] < 0.0) << 2) | (int(invdir[Y] < 0.0) << 1) | int(invdir[Z] < 0.0));

		if(object->Intersect_BBox(variant, origin, invdir, closest) == false)
			return false;

		if(object->Bound.empty() == false)
		{
			if(Ray_In_Bound(ray, object->Bound, threadData) == false)
				return false;
		}

		IStack depthstack(stackPool);
		assert(depthstack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

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

		assert(depthstack->empty()); // verify that the IStack is in a cleaned-up condition (again)
	}

	return false;
}

unsigned int Trace::GetHighestTraceLevel()
{
	return maxFoundTraceLevel;
}

void Trace::ComputeTextureColour(Intersection& isect, Colour& colour, const Ray& ray, COLC weight, bool photonPass, TraceTicket& ticket)
{
	// NOTE: when called during the photon pass this method is used to deposit photons
	// on the surface and not, per se, to compute texture color.
	WeightedTextureVector wtextures;
	double normaldirection;
	Colour tmpCol;
	Colour c1;
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
	isect.Object->Normal(*rawnormal, &isect, threadData);

	// I added this to flip the normal if the object is inverted (for CSG).
	// However, I subsequently commented it out for speed reasons - it doesn't
	// make a difference (no pun intended). The preexisting flip code below
	// produces a similar (though more extensive) result. [NK]
	// Actually, we should keep this code to guarantee that Normal_Direction
	// is set properly. [NK]
	if(Test_Flag(isect.Object, INVERTED_FLAG))
		rawnormal = -rawnormal;

	// if the surface normal points away, flip its direction
	normaldirection = dot(rawnormal, Vector3d(ray.Direction));
	if(normaldirection > 0.0)
		rawnormal = -rawnormal;

	Assign_Vector(isect.INormal, *rawnormal);
	Assign_Vector(isect.PNormal, *rawnormal);

	if(Test_Flag(isect.Object, UV_FLAG))
	{
		// TODO FIXME
		//  I think we have a serious problem here regarding bump mapping:
		//  The UV vector contains doesn't contain any information about the (local) *orientation* of U and V in our XYZ co-ordinate system!
		//  This causes slopes do be applied in the wrong directions.

		// get the UV vect of the intersection
		isect.Object->UVCoord(*uvcoords, &isect, threadData);
		// save the normal and UV coords into Intersection
		Assign_UV_Vect(isect.Iuv, *uvcoords);
	}

	// now switch to UV mapping if we need to
	if(Test_Flag(isect.Object, UV_FLAG))
		ipoint = Vector3d(uvcoords.u(), uvcoords.v(), 0.0);

	bool isMultiTextured = Test_Flag(isect.Object, MULTITEXTURE_FLAG) ||
	                       ((isect.Object->Texture == NULL) && Test_Flag(isect.Object, CUTAWAY_TEXTURES_FLAG));

	// get textures and weights
	if(isMultiTextured == true)
	{
		isect.Object->Determine_Textures(&isect, normaldirection > 0.0, wtextures, threadData);
	}
	else if(isect.Object->Texture != NULL)
	{
		if((normaldirection > 0.0) && (isect.Object->Interior_Texture != NULL))
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
		assert(warps->empty()); // verify that the TextureVector pulled from the pool is in a cleaned-up condition

		// if the contribution of this texture is neglectable skip ahead
		if((i->weight < ticket.adcBailout) || (i->texture == NULL))
			continue;

		if(photonPass == true)
		{
			// For the photon pass, colour (and thus c1) represents the
			// light energy being transmitted by the photon.  Because of this, we
			// compute the weighted energy value, then pass it to the texture for
			// processing.
			c1.red()   = colour.red()   * i->weight;
			c1.green() = colour.green() * i->weight;
			c1.blue()  = colour.blue()  * i->weight;

			// NOTE that ComputeOneTextureColor is being used for a secondary purpose, and
			// that to place photons on the surface and trigger recursive photon shooting
			ComputeOneTextureColour(c1, i->texture, *warps, ipoint, rawnormal, ray, weight, isect, false, true, ticket);
		}
		else
		{
			ComputeOneTextureColour(c1, i->texture, *warps, ipoint, rawnormal, ray, weight, isect, false, false, ticket);

			tmpCol.red()    += i->weight * c1.red();
			tmpCol.green()  += i->weight * c1.green();
			tmpCol.blue()   += i->weight * c1.blue();
			tmpCol.transm() += i->weight * c1.transm();
		}
	}

#if MEDIA_AFTER_TEXTURE_INTERPOLATION
	// [CLi] moved this here from Trace::ComputeShadowTexture() and Trace::ComputeLightedTexture(), respectively,
	// to avoid media to be computed twice when dealing with averaged textures.
	// TODO - For photon rays we're still potentially doing double work on media.
	// TODO - For shadow rays we're still potentially doing double work on distance-based attenuation.
	// Calculate participating media effects.
	if(!photonPass && (qualityFlags & Q_VOLUME) && (!ray.GetInteriors().empty()) && (ray.IsHollowRay() == true))
		media.ComputeMedia(ray.GetInteriors(), ray, isect, tmpCol, ticket);
#endif

	colour += tmpCol;

	lightColorCacheIndex--;
}

void Trace::ComputeOneTextureColour(Colour& resultcolour, const TEXTURE *texture, vector<const TEXTURE *>& warps, const Vector3d& ipoint,
                                    const Vector3d& rawnormal, const Ray& ray, COLC weight, Intersection& isect, bool shadowflag, bool photonPass, TraceTicket& ticket)
{
	// NOTE: this method is used by the photon pass to deposit photons on the surface
	// (and not, per se, to compute texture color)
	const BLEND_MAP *blendmap = texture->Blend_Map;
	const BLEND_MAP_ENTRY *prev, *cur;
	double value1, value2; // TODO FIXME - choose better names!
	Vector3d tpoint;
	Vector2d uvcoords;
	Colour c2;

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
				resultcolour = Colour(1.0, 1.0, 1.0, 1.0, 1.0);
				break;
			case AVERAGE_PATTERN:
				Warp_EPoint(*tpoint, *ipoint, reinterpret_cast<const TPATTERN *>(warps.back()));
				ComputeAverageTextureColours(resultcolour, texture, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass, ticket);
				break;
			case UV_MAP_PATTERN:
				// Don't bother warping, simply get the UV vect of the intersection
				isect.Object->UVCoord(*uvcoords, &isect, threadData);
				tpoint = Vector3d(uvcoords[U], uvcoords[V], 0.0);
				cur = &(texture->Blend_Map->Blend_Map_Entries[0]);
				ComputeOneTextureColour(resultcolour, cur->Vals.Texture, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass, ticket);
				break;
			case BITMAP_PATTERN:
				Warp_EPoint(*tpoint, *ipoint, reinterpret_cast<const TPATTERN *>(texture));
				ComputeOneTextureColour(resultcolour, material_map(*tpoint, texture), warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass, ticket);
				break;
			case PLAIN_PATTERN:
				if(shadowflag == true)
					ComputeShadowTexture(resultcolour, texture, warps, ipoint, rawnormal, ray, isect, ticket);
				else
					ComputeLightedTexture(resultcolour, texture, warps, ipoint, rawnormal, ray, weight, isect, ticket);
				break;
			default:
				throw POV_EXCEPTION_STRING("Bad texture type in ComputeOneTextureColour");
		}
	}
	else
	{
		// NK 19 Nov 1999 added Warp_EPoint
		Warp_EPoint(*tpoint, *ipoint, reinterpret_cast<const TPATTERN *>(texture));
		value1 = Evaluate_TPat(reinterpret_cast<const TPATTERN *>(texture), *tpoint, &isect, &ray, threadData);

		Search_Blend_Map(value1, blendmap, &prev, &cur);

		// NK phmap
		if(photonPass)
		{
			if(prev == cur)
				ComputeOneTextureColour(resultcolour, cur->Vals.Texture, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass, ticket);
			else
			{
				value1 = (value1 - prev->value) / (cur->value - prev->value);
				value2 = 1.0 - value1;
				VScale(*c2, *resultcolour, value1); // modifies RGB, but leaves Filter and Transmit unchanged
				ComputeOneTextureColour(c2, cur->Vals.Texture, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass, ticket);
				VScale(*c2, *resultcolour, value2); // modifies RGB, but leaves Filter and Transmit unchanged
				ComputeOneTextureColour(c2, prev->Vals.Texture, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass, ticket);
			}
		}
		else
		{
			ComputeOneTextureColour(resultcolour, cur->Vals.Texture, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass, ticket);

			if(prev != cur)
			{
				ComputeOneTextureColour(c2, prev->Vals.Texture, warps, tpoint, rawnormal, ray, weight, isect, shadowflag, photonPass, ticket);
				value1 = (value1 - prev->value) / (cur->value - prev->value);
				value2 = 1.0 - value1;
				resultcolour = value1 * resultcolour + value2 * c2;
			}
		}
	}
}

void Trace::ComputeAverageTextureColours(Colour& resultcolour, const TEXTURE *texture, vector<const TEXTURE *>& warps, const Vector3d& ipoint,
                                         const Vector3d& rawnormal, const Ray& ray, COLC weight, Intersection& isect, bool shadowflag, bool photonPass, TraceTicket& ticket)
{
	const BLEND_MAP *bmap = texture->Blend_Map;
	SNGL total = 0.0;
	Colour lc;

	if(photonPass == false)
	{
		resultcolour.clear();

		for(int i = 0; i < bmap->Number_Of_Entries; i++)
		{
			SNGL val = bmap->Blend_Map_Entries[i].value;

			ComputeOneTextureColour(lc, bmap->Blend_Map_Entries[i].Vals.Texture, warps, ipoint, rawnormal, ray, weight, isect, shadowflag, photonPass, ticket);

			resultcolour += lc * val;

			total += val;
		}

		resultcolour /= total;
	}
	else
	{
		for(int i = 0; i < bmap->Number_Of_Entries; i++)
			total += bmap->Blend_Map_Entries[i].value;

		for(int i = 0; i < bmap->Number_Of_Entries; i++)
		{
			VScale(*lc, *resultcolour, bmap->Blend_Map_Entries[i].value / total); // modifies RGB, but leaves Filter and Transmit unchanged

			ComputeOneTextureColour(lc, bmap->Blend_Map_Entries[i].Vals.Texture, warps, ipoint, rawnormal, ray, weight, isect, shadowflag, photonPass, ticket);
		}
	}
}

void Trace::ComputeLightedTexture(Colour& resultcolour, const TEXTURE *texture, vector<const TEXTURE *>& warps, const Vector3d& ipoint,
                                  const Vector3d& rawnormal, const Ray& ray, COLC weight, Intersection& isect, TraceTicket& ticket)
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
	RGBColour attCol;
	Colour layCol, rflCol, rfrCol;
	RGBColour filCol;
	RGBColour tmpCol, tmp;
	RGBColour ambCol; // Note that there is no gathering of filter or transparency
	RGBColour ambBackCol;
	bool one_colour_found, colour_found;
	bool tir_occured;
	std::auto_ptr<PhotonGatherer> surfacePhotonGatherer(NULL); // TODO FIXME - auto_ptr why?  [CLi] why, to auto-destruct it of course! (e.g. in case of exception)

	WNRXVector listWNRX(wnrxPool); // "Weight, Normal, Reflectivity, eXponent"
	assert(listWNRX->empty()); // verify that the WNRXVector pulled from the pool is in a cleaned-up condition

	// resultcolour builds up the apparent visible color of the point.
	// Note that besides the RGB components, this also includes Transmission
	// for alpha channel computation.
	resultcolour.clear();

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
	filCol = RGBColour(1.0, 1.0, 1.0);

	trans = 1.0;

	// Add in radiosity (stochastic interreflection-based ambient light) if desired
	radiosity_done = false;
	radiosity_back_done = false;

	// This block just sets up radiosity for the code inside the loop, which is first-time-through.
	radiosity_needed = (sceneData->radiositySettings.radiosityEnabled == true) &&
	                   (radiosity.CheckRadiosityTraceLevel(ticket) == true) &&
	                   (Test_Flag(isect.Object, IGNORE_RADIOSITY_FLAG) == false);

	// Loop through the layers and compute the ambient, diffuse,
	// phong and specular for these textures.
	one_colour_found = false;

	if(sceneData->photonSettings.photonsEnabled && sceneData->surfacePhotonMap.numPhotons > 0)
		surfacePhotonGatherer.reset(new PhotonGatherer(&sceneData->surfacePhotonMap, sceneData->photonSettings));

	for(layer_number = 0, layer = texture; (layer != NULL) && (trans > ticket.adcBailout); layer_number++, layer = reinterpret_cast<const TEXTURE *>(layer->Next))
	{
		// Get perturbed surface normal.
		layNormal = rawnormal;

		if((qualityFlags & Q_NORMAL) && (layer->Tnormal != NULL))
		{
			for(vector<const TEXTURE *>::iterator i(warps.begin()); i != warps.end(); i++)
				Warp_Normal(*layNormal, *layNormal, reinterpret_cast<const TPATTERN *>(*i), Test_Flag((*i), DONT_SCALE_BUMPS_FLAG));

			Perturb_Normal(*layNormal, layer->Tnormal, *ipoint, &isect, &ray, threadData);

			if((Test_Flag(layer->Tnormal, DONT_SCALE_BUMPS_FLAG)))
				layNormal.normalize();

			for(vector<const TEXTURE *>::reverse_iterator i(warps.rbegin()); i != warps.rend(); i++)
				UnWarp_Normal(*layNormal, *layNormal, reinterpret_cast<const TPATTERN *>(*i), Test_Flag((*i), DONT_SCALE_BUMPS_FLAG));
		}

		// Store top layer normal.
		if(layer_number == 0)
			topNormal = layNormal;

		// Get surface colour.
		new_Weight = weight * trans;
		colour_found = Compute_Pigment(layCol, layer->Pigment, *ipoint, &isect, &ray, threadData);

		// If a valid color was returned set one_colour_found to true.
		// An invalid color is returned if a surface point is outside
		// an image map used just once.
		one_colour_found = (one_colour_found || colour_found);

		// This section of code used to be the routine Compute_Reflected_Colour.
		// I copied it in here to rearrange some of it more easily and to
		// see if we could eliminate passing a zillion parameters for no
		// good reason. [CY 1/95]

		if(qualityFlags & Q_FULL_AMBIENT)
		{
			// Only use top layer and kill transparency if low quality.
			resultcolour = layCol;
			resultcolour.filter() = 0.0;
			resultcolour.transm() = 0.0;
		}
		else
		{
			// Store vital information for later reflection.
			listWNRX->push_back(WNRX(new_Weight, layNormal, RGBColour(), layer->Finish->Reflect_Exp));

			// angle-dependent reflectivity
			cos_Angle_Incidence = -dot(Vector3d(ray.Direction), layNormal);

			if((isect.Object->interior != NULL) || (layer->Finish->Reflection_Type != 1))
			{
				ComputeReflectivity(listWNRX->back().weight, listWNRX->back().reflec,
				                    layer->Finish->Reflection_Max, layer->Finish->Reflection_Min,
				                    layer->Finish->Reflection_Type, layer->Finish->Reflection_Falloff,
				                    cos_Angle_Incidence, ray, isect.Object->interior);
			}
			else
				throw POV_EXCEPTION_STRING("Reflection_Type 1 used with no interior."); // TODO FIXME - wrong place to report this [trf]

			// for metallic reflection, apply the surface color using the fresnel equation
			// (use the same equaltion as "metallic" in phong and specular
			if(layer->Finish->Reflect_Metallic != 0.0)
			{
				double R_M = layer->Finish->Reflect_Metallic;

				double x = fabs(acos(cos_Angle_Incidence)) / M_PI_2;
				double F = 0.014567225 / Sqr(x - 1.12) - 0.011612903;
				F = min(1.0, max(0.0, F));

				listWNRX->back().reflec.red()   *= (1.0 + R_M * (1.0 - F) * (layCol.red()   - 1.0));
				listWNRX->back().reflec.green() *= (1.0 + R_M * (1.0 - F) * (layCol.green() - 1.0));
				listWNRX->back().reflec.blue()  *= (1.0 + R_M * (1.0 - F) * (layCol.blue()  - 1.0));
			}

			// NK - I think we SHOULD do something like this: (to apply the layer's color) */
			// listWNRX->back().reflec.red()   *= filCol.red();
			// listWNRX->back().reflec.green() *= filCol.green();
			// listWNRX->back().reflec.blue()  *= filCol.blue();

			// We need to reduce the layer's own brightness if it is transparent.
			if (sceneData->EffectiveLanguageVersion() < 370)
				// this formula is bogus, but it has been around for a while so we're keeping it for compatibility with legacy scenes
				att = (1.0 - (layCol.filter() * max3(layCol.red(), layCol.green(), layCol.blue()) + layCol.transm()));
			else
				att = layCol.opacity();

			// now compute the BRDF or BSSRDF contribution
			tmpCol.clear();

			if(sceneData->useSubsurface && layer->Finish->UseSubsurface && (qualityFlags & Q_SUBSURFACE))
			{
				// Add diffuse & single scattering contribution.
				ComputeSubsurfaceScattering(layer->Finish, RGBColour(layCol), isect, ray, layNormal, tmpCol, att, ticket);
				// [CLi] moved multiplication with filCol to further below

				// Radiosity-style ambient may be subject to subsurface light transport.
				// In that case, the respective computations are handled by the BSSRDF code already.
				if (sceneData->subsurfaceUseRadiosity)
					radiosity_needed = false;
			}
			// Add radiosity ambient contribution.
			if(radiosity_needed)
			{
				// if radiosity calculation needed, but not yet done, do it now
				// TODO FIXME - [CLi] with "normal on", shouldn't we compute radiosity for each layer separately (if it has pertubed normals)?
				if(radiosity_done == false)
				{
					// calculate max possible contribution of radiosity, to see if calculating it is worthwhile
					// TODO FIXME - other layers may see a higher weight!
					// Maybe we should go along and compute *first* the total contribution radiosity will make,
					// and at the *end* apply it.
					max_Radiosity_Contribution = (filCol * RGBColour(layCol)).greyscale() * att * layer->Finish->RawDiffuse;

					if(max_Radiosity_Contribution > ticket.adcBailout)
					{
						radiosity.ComputeAmbient(Vector3d(isect.IPoint), rawnormal, layNormal, ambCol, weight * max_Radiosity_Contribution, ticket);
						radiosity_done = true;
					}
				}

				// [CLi] moved multiplication with filCol to further below
				tmpCol += (RGBColour(layCol) * ambCol) * (att * layer->Finish->RawDiffuse);

				// if backside radiosity calculation needed, but not yet done, do it now
				// TODO FIXME - [CLi] with "normal on", shouldn't we compute radiosity for each layer separately (if it has pertubed normals)?
				if(layer->Finish->DiffuseBack != 0.0)
				{
					if(radiosity_back_done == false)
					{
						// calculate max possible contribution of radiosity, to see if calculating it is worthwhile
						// TODO FIXME - other layers may see a higher weight!
						// Maybe we should go along and compute *first* the total contribution radiosity will make,
						// and at the *end* apply it.
						max_Radiosity_Contribution = (filCol * RGBColour(layCol)).greyscale() * att * layer->Finish->RawDiffuseBack;

						if(max_Radiosity_Contribution > ticket.adcBailout)
						{
							radiosity.ComputeAmbient(Vector3d(isect.IPoint), -rawnormal, -layNormal, ambBackCol, weight * max_Radiosity_Contribution, ticket);
							radiosity_back_done = true;
						}
					}

					// [CLi] moved multiplication with filCol to further below
					tmpCol += (RGBColour(layCol) * ambBackCol) * (att * layer->Finish->RawDiffuseBack);
				}
			}

			// Add emissive ("classic" ambient) contribution.
			// [CLi] moved multiplication with filCol to further below
			if (!sceneData->radiositySettings.radiosityEnabled || (sceneData->EffectiveLanguageVersion() < 370))
				// only use "ambient" setting when radiosity is disabled (or in legacy scenes)
				tmpCol += (RGBColour(layCol) * layer->Finish->Ambient * sceneData->ambientLight * att);
			tmpCol += (RGBColour(layCol) * layer->Finish->Emission * att);

			// set up the "litObjectIgnoresPhotons" flag (thread variable) so that
			// ComputeShadowColour will know whether or not this lit object is
			// ignoring photons, which affects partial-shadowing (i.e. filter and transmit)
			threadData->litObjectIgnoresPhotons = Test_Flag(isect.Object,PH_IGNORE_PHOTONS_FLAG);

			// Add diffuse, phong, specular, and iridescence contribution.
			// (We don't need to do this for (non-radiosity) rays during pretrace, as it does not affect radiosity sampling)
			if(!ray.IsPretraceRay())
			{
				Vector3d tmpIPoint(isect.IPoint);

				if((layer->Finish->Diffuse != 0.0) || (layer->Finish->DiffuseBack != 0.0) || (layer->Finish->Specular != 0.0) || (layer->Finish->Phong != 0.0))
					ComputeDiffuseLight(layer->Finish, tmpIPoint, ray, layNormal, RGBColour(layCol), tmpCol, att, isect.Object, ticket);
			}

			if(sceneData->photonSettings.photonsEnabled && sceneData->surfacePhotonMap.numPhotons > 0)
			{
				// NK phmap - now do the same for the photons in the area
				if(!Test_Flag(isect.Object, PH_IGNORE_PHOTONS_FLAG))
				{
					Vector3d tmpIPoint(isect.IPoint);
					ComputePhotonDiffuseLight(layer->Finish, tmpIPoint, ray, layNormal, rawnormal, RGBColour(layCol), tmpCol, att, isect.Object, *surfacePhotonGatherer);
				}
			}

			tmpCol *= filCol;
			VAddEq(*resultcolour, *tmpCol); // modifies RGB, but leaves Filter and Transmit unchanged
		}

		// Get new filter color.
		if(colour_found)
		{
			filCol *= layCol.rgbTransm();

			if(layer->Finish->Conserve_Energy != 0 && listWNRX->empty() == false)
			{
				// adjust filCol based on reflection
				// this would work so much better with r,g,b,rt,gt,bt
				filCol *= RGBColour(min(1.0, 1.0 - listWNRX->back().reflec.red()),
				                    min(1.0, 1.0 - listWNRX->back().reflec.green()),
				                    min(1.0, 1.0 - listWNRX->back().reflec.blue()));
			}
		}

		// Get new remaining translucency.
		// [CLi] changed filCol to RGB, as filter and transmit were always pinned to 1.0 and 0.0, respectively anyway
		// TODO CLARIFY - is this working properly if filCol.greyscale() is negative? (what would be the right thing then?)
		trans = min(1.0, (double)fabs(filCol.greyscale()));
	}

	// Calculate transmitted component.
	//
	// If the surface is translucent a transmitted ray is traced
	// and its contribution is added to the total ResCol after
	// filtering it by filCol.
	tir_occured = false;

	if(((interior = isect.Object->interior) != NULL) && (trans > ticket.adcBailout) && (qualityFlags & Q_REFRACT))
	{
		// [CLi] changed filCol to RGB, as filter and transmit were always pinned to 1.0 and 0.0, respectively anyway
		// TODO CLARIFY - is this working properly if some filCol component is negative? (what would be the right thing then?)
		w1 = max3(fabs(filCol.red()), fabs(filCol.green()), fabs(filCol.blue()));
		new_Weight = weight * w1;

		// Trace refracted ray.
		Vector3d tmpIPoint(isect.IPoint);
		Colour tempcolor;

		tir_occured = ComputeRefraction(texture->Finish, interior, tmpIPoint, ray, topNormal, rawnormal, tempcolor, new_Weight, ticket);

		if(tir_occured == true)
			rfrCol += tempcolor;
		else
			rfrCol = tempcolor;

		// Get distance based attenuation.
		// TODO - virtually the same code is used in ComputeShadowTexture().
		attCol.set(interior->Old_Refract);

		if((interior != NULL) && ray.IsInterior(interior) == true)
		{
			if(fabs(interior->Fade_Distance) > EPSILON)
			{
				// NK attenuate
				if(interior->Fade_Power >= 1000)
				{
					double depth = isect.Depth / interior->Fade_Distance;
					attCol *= exp(-(RGBColour(1.0) - interior->Fade_Colour) * depth);
				}
				else
				{
					att = 1.0 + pow(isect.Depth / interior->Fade_Distance, (double)interior->Fade_Power);
					attCol *= (interior->Fade_Colour + (RGBColour(1.0) - interior->Fade_Colour) / att);
				}
			}
		}

		// If total internal reflection occured the transmitted light is not filtered.
		if(tir_occured)
		{
			resultcolour.red()   += attCol.red()   * rfrCol.red();
			resultcolour.green() += attCol.green() * rfrCol.green();
			resultcolour.blue()  += attCol.blue()  * rfrCol.blue();
			// NOTE: pTRANSM (alpha channel) stays zero
		}
		else
		{
			if(one_colour_found)
			{
				// [CLi] changed filCol to RGB, as filter and transmit were always pinned to 1.0 and 0.0, respectively anyway
				resultcolour.red()   += attCol.red()   * rfrCol.red()   * filCol.red();
				resultcolour.green() += attCol.green() * rfrCol.green() * filCol.green();
				resultcolour.blue()  += attCol.blue()  * rfrCol.blue()  * filCol.blue();
				// We need to know the transmittance value for the alpha channel. [DB]
				resultcolour.transm() = attCol.greyscale() * rfrCol.transm() * trans;
			}
			else
			{
				resultcolour.red()   += attCol.red()   * rfrCol.red();
				resultcolour.green() += attCol.green() * rfrCol.green();
				resultcolour.blue()  += attCol.blue()  * rfrCol.blue();
				// We need to know the transmittance value for the alpha channel. [DB]
				resultcolour.transm() = attCol.greyscale() * rfrCol.transm();
			}
		}
	}

	// Calculate reflected component.
	//
	// If total internal reflection occured all reflections using
	// TopNormal are skipped.
	if(qualityFlags & Q_REFLECT)
	{
		layer = texture;
		for(i = 0; i < layer_number; i++)
		{
			if((!tir_occured) ||
			   (fabs(topNormal[X]-(*listWNRX)[i].normal[X]) > EPSILON) ||
			   (fabs(topNormal[Y]-(*listWNRX)[i].normal[Y]) > EPSILON) ||
			   (fabs(topNormal[Z]-(*listWNRX)[i].normal[Z]) > EPSILON))
			{
				if(!(*listWNRX)[i].reflec.isZero())
				{
					Vector3d tmpIPoint(isect.IPoint);

					rflCol.clear();
					ComputeReflection(layer->Finish, tmpIPoint, ray, (*listWNRX)[i].normal, rawnormal, rflCol, (*listWNRX)[i].weight, ticket);

					if((*listWNRX)[i].reflex != 1.0)
					{
						resultcolour.red()    += (*listWNRX)[i].reflec.red()   * pow(rflCol.red(),   (*listWNRX)[i].reflex);
						resultcolour.green()  += (*listWNRX)[i].reflec.green() * pow(rflCol.green(), (*listWNRX)[i].reflex);
						resultcolour.blue()   += (*listWNRX)[i].reflec.blue()  * pow(rflCol.blue(),  (*listWNRX)[i].reflex);
					}
					else
					{
						resultcolour.red()   += (*listWNRX)[i].reflec.red()   * rflCol.red();
						resultcolour.green() += (*listWNRX)[i].reflec.green() * rflCol.green();
						resultcolour.blue()  += (*listWNRX)[i].reflec.blue()  * rflCol.blue();
					}
				}
			}
			layer = reinterpret_cast<const TEXTURE *>(layer->Next);
		}
	}

#if MEDIA_AFTER_TEXTURE_INTERPOLATION
	// [CLi] moved this to Trace::ComputeTextureColour() and Trace::ComputeShadowColour(), respectively
	// to avoid media to be computed twice when dealing with averaged textures.
#else
	// Calculate participating media effects.
	if((qualityFlags & Q_VOLUME) && (!ray.GetInteriors().empty()) && (ray.IsHollowRay() == true))
		media.ComputeMedia(ray.GetInteriors(), ray, isect, resultcolour, ticket);
#endif
}

void Trace::ComputeShadowTexture(Colour& filtercolour, const TEXTURE *texture, vector<const TEXTURE *>& warps, const Vector3d& ipoint,
                                 const Vector3d& rawnormal, const Ray& ray, Intersection& isect, TraceTicket& ticket)
{
	Interior *interior = isect.Object->interior;
	const TEXTURE *layer;
	double caustics, dotval, k;
	Vector3d layer_Normal;
	RGBColour refraction;
	Colour layer_Pigment_Colour;
	bool one_colour_found, colour_found;

	RGBColour tmpCol = RGBColour(1.0, 1.0, 1.0);

	one_colour_found = false;

	// [CLI] removed obsolete test for filtercolour.filter() and filtercolour.transm(), as they remain unchanged during loop
	for(layer = texture; layer != NULL; layer = reinterpret_cast<TEXTURE *>(layer->Next))
	{
		colour_found = Compute_Pigment(layer_Pigment_Colour, layer->Pigment, *ipoint, &isect, &ray, threadData);

		if(colour_found)
		{
			one_colour_found = true;

			tmpCol *= layer_Pigment_Colour.rgbTransm();
		}

		// Get normal for faked caustics (will rewrite later to cache).
		if((interior != NULL) && ((caustics = interior->Caustics) != 0.0))
		{
			layer_Normal = rawnormal;

			if((qualityFlags & Q_NORMAL) && (layer->Tnormal != NULL))
			{
				for(vector<const TEXTURE *>::iterator i(warps.begin()); i != warps.end(); i++)
					Warp_Normal(*layer_Normal, *layer_Normal, reinterpret_cast<const TPATTERN *>(*i), Test_Flag((*i), DONT_SCALE_BUMPS_FLAG));

				Perturb_Normal(*layer_Normal, layer->Tnormal, *ipoint, &isect, &ray, threadData);

				if((Test_Flag(layer->Tnormal,DONT_SCALE_BUMPS_FLAG)))
					layer_Normal.normalize();

				for(vector<const TEXTURE *>::reverse_iterator i(warps.rbegin()); i != warps.rend(); i++)
					UnWarp_Normal(*layer_Normal, *layer_Normal, reinterpret_cast<const TPATTERN *>(*i), Test_Flag((*i), DONT_SCALE_BUMPS_FLAG));
			}

			// Get new filter/transmit values.
			dotval = dot(layer_Normal, Vector3d(ray.Direction));

			k = (1.0 + pow(fabs(dotval), caustics));

			tmpCol *= k;
		}
	}

	// TODO - [CLi] aren't spatial effects (distance attenuation, media) better handled in Trace::ComputeTextureColour()? We may be doing double work here!

	// Get distance based attenuation.
	// TODO - virtually the same code is used in ComputeLightedTexture().
	refraction = RGBColour(1.0, 1.0, 1.0);

	if((interior != NULL) && (ray.IsInterior(interior) == true))
	{
		if((interior->Fade_Power > 0.0) && (fabs(interior->Fade_Distance) > EPSILON))
		{
			// NK - attenuation
			if(interior->Fade_Power>=1000)
			{
				refraction *= exp( -(RGBColour(1.0) - interior->Fade_Colour) * (isect.Depth / interior->Fade_Distance) );
			}
			else
			{
				k = 1.0 + pow(isect.Depth / interior->Fade_Distance, (double)interior->Fade_Power);
				refraction *= (interior->Fade_Colour + (RGBColour(1.0) - interior->Fade_Colour) / k);
			}
		}
	}

	// Get distance based attenuation.
	filtercolour = Colour(tmpCol * refraction, 1.0, 0.0);

#if MEDIA_AFTER_TEXTURE_INTERPOLATION
	// [CLi] moved this to Trace::ComputeTextureColour() and Trace::ComputeShadowColour(), respectively
	// to avoid media to be computed twice when dealing with averaged textures.
#else
	// Calculate participating media effects.
	if((qualityFlags & Q_VOLUME) && (!ray.GetInteriors().empty()) && (ray.IsHollowRay() == true))
		media.ComputeMedia(ray.GetInteriors(), ray, isect, filtercolour, ticket);
#endif
}

void Trace::ComputeReflection(const FINISH* finish, const Vector3d& ipoint, const Ray& ray, const Vector3d& normal, const Vector3d& rawnormal, Colour& colour, COLC weight, TraceTicket& ticket)
{
	Ray nray(ray);
	double n, n2;

	nray.SetFlags(Ray::ReflectionRay, ray);

	// The rest of this is essentally what was originally here, with small changes.
	n = -2.0 * dot(Vector3d(ray.Direction), normal);
	VAddScaled(nray.Direction, ray.Direction, n, *normal);

	// Nathan Kopp & CEY 1998 - Reflection bugfix
	// if the new ray is going the opposite direction as raw normal, we
	// need to fix it.
	n = dot(Vector3d(nray.Direction), rawnormal);

	if(n < 0.0)
	{
		// It needs fixing. Which kind?
		n2 = dot(Vector3d(nray.Direction), normal);

		if(n2 < 0.0)
		{
			// reflected inside rear virtual surface. Reflect Ray using Raw_Normal
			n = -2.0 * dot(Vector3d(ray.Direction), rawnormal);
			VAddScaled(nray.Direction, ray.Direction, n, *rawnormal);
		}
		else
		{
			// Double reflect NRay using Raw_Normal
			// n = dot(Vector3d(New_Ray.Direction),Vector3d(Jitter_Raw_Normal)); - kept the old n around
			n *= -2.0;
			VAddScaledEq(nray.Direction, n, *rawnormal);
		}
	}

	VNormalizeEq(nray.Direction);
	Assign_Vector(nray.Origin, *ipoint);
	threadData->Stats()[Reflected_Rays_Traced]++;

	// Trace reflected ray.
	bool alphaBackground = ticket.alphaBackground;
	ticket.alphaBackground = false;
	if (!ray.IsPhotonRay() && (finish->Irid > 0.0))
	{
		Colour tmpCol;
		TraceRay(nray, tmpCol, weight, ticket, false);
		RGBColour tmpCol2(tmpCol);
		ComputeIridColour(finish, Vector3d(nray.Direction), Vector3d(ray.Direction), normal, ipoint, tmpCol2);
		colour += Colour(tmpCol2);
	}
	else
	{
		TraceRay(nray, colour, weight, ticket, false);
	}
	ticket.alphaBackground = alphaBackground;
}

bool Trace::ComputeRefraction(const FINISH* finish, Interior *interior, const Vector3d& ipoint, const Ray& ray, const Vector3d& normal, const Vector3d& rawnormal, Colour& colour, COLC weight, TraceTicket& ticket)
{
	Ray nray(ray);
	Vector3d localnormal;
	double n, ior, dispersion;
	unsigned int dispersionelements = interior->Disp_NElems;
	bool havedispersion = (dispersionelements > 0);

	nray.SetFlags(Ray::RefractionRay, ray);

	// Set up new ray.
	Assign_Vector(nray.Origin, *ipoint);

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
		if(havedispersion == true)
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
				if(havedispersion == true)
					dispersion = interior->Dispersion / sceneData->atmosphereDispersion;
			}
			else
			{
				// The ray is leaving into another object, i.e. (A (B B) ...
				// For the purpose of refraction, pretend that we weren't inside that other object,
				// i.e. pretend that we didn't encounter (A (B B) ... but (A A|B B|A ...
				ior = interior->IOR / nray.GetInteriors().back()->IOR;
				if(havedispersion == true)
				{
					dispersion = interior->Dispersion / nray.GetInteriors().back()->Dispersion;
					dispersionelements = max(dispersionelements, (unsigned int)(nray.GetInteriors().back()->Disp_NElems));
				}
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
			if(havedispersion == true)
				dispersion = nray.GetInteriors().back()->Dispersion / interior->Dispersion;

			nray.AppendInterior(interior);
		}
	}

	// Do the two mediums traversed have the same indices of refraction?
	if((fabs(ior - 1.0) < EPSILON) && (fabs(dispersion - 1.0) < EPSILON))
	{
		// Only transmit the ray.
		Assign_Vector(nray.Direction, ray.Direction);
		// Trace a transmitted ray.
		threadData->Stats()[Transmitted_Rays_Traced]++;

		colour.clear();
		TraceRay(nray, colour, weight, ticket, true);
	}
	else
	{
		// Refract the ray.
		n = dot(Vector3d(ray.Direction), normal);

		if(n <= 0.0)
		{
			localnormal = normal;
			n = -n;
		}
		else
			localnormal = -normal;


		// TODO FIXME: also for first radiosity pass ? (see line 3272 of v3.6 lighting.cpp)
		if(fabs (dispersion - 1.0) < EPSILON) // TODO FIXME - radiosity: || (!isFinalTrace)
			return TraceRefractionRay(finish, ipoint, ray, nray, ior, n, normal, rawnormal, localnormal, colour, weight, ticket);
		else if(ray.IsMonochromaticRay() == true)
			return TraceRefractionRay(finish, ipoint, ray, nray, ray.GetSpectralBand().GetDispersionIOR(ior, dispersion), n, normal, rawnormal, localnormal, colour, weight, ticket);
		else
		{
			RGBColour sumcol;

			for(unsigned int i = 0; i < dispersionelements; i++)
			{
				Colour tempcolour;

				// NB setting the dispersion factor also causes the MonochromaticRay flag to be set
				SpectralBand spectralBand(i, dispersionelements);
				nray.SetSpectralBand(spectralBand);

				(void)TraceRefractionRay(finish, ipoint, ray, nray, spectralBand.GetDispersionIOR(ior, dispersion), n, normal, rawnormal, localnormal, tempcolour, weight, ticket);

				sumcol += RGBColour(tempcolour) * spectralBand.GetHue();
			}

			colour = Colour(sumcol / double(dispersionelements));
		}
	}

	return false;
}

bool Trace::TraceRefractionRay(const FINISH* finish, const Vector3d& ipoint, const Ray& ray, Ray& nray, double ior, double n, const Vector3d& normal, const Vector3d& rawnormal, const Vector3d& localnormal, Colour& colour, COLC weight, TraceTicket& ticket)
{
	// Compute refrated ray direction using Heckbert's method.
	double t = 1.0 + Sqr(ior) * (Sqr(n) - 1.0);

	if(t < 0.0)
	{
		Colour tempcolour;

		// Total internal reflection occures.
		threadData->Stats()[Internal_Reflected_Rays_Traced]++;
		ComputeReflection(finish, ipoint, ray, normal, rawnormal, tempcolour, weight, ticket);
		colour += tempcolour;

		return true;
	}

	t = ior * n - sqrt(t);

	VLinComb2(nray.Direction, ior, ray.Direction, t, *localnormal);

	// Trace a refracted ray.
	threadData->Stats()[Refracted_Rays_Traced]++;

	colour.clear();
	TraceRay(nray, colour, weight, ticket, false);

	return false;
}

// see Diffuse in the 3.6 code (lighting.cpp)
void Trace::ComputeDiffuseLight(const FINISH *finish, const Vector3d& ipoint, const Ray& eye, const Vector3d& layer_normal, const RGBColour& layer_pigment_colour,
                                RGBColour& colour, double attenuation, ObjectPtr object, TraceTicket& ticket)
{
	Vector3d reye;

	// TODO FIXME - [CLi] why is this computed here? Not so exciting, is it?
	if(finish->Specular != 0.0)
		reye = -Vector3d(eye.Direction);

	// global light sources, if not turned off for this object
	if((object->Flags & NO_GLOBAL_LIGHTS_FLAG) != NO_GLOBAL_LIGHTS_FLAG)
	{
		for(int i = 0; i < threadData->lightSources.size(); i++)
			ComputeOneDiffuseLight(*threadData->lightSources[i], reye, finish, ipoint, eye, layer_normal, layer_pigment_colour, colour, attenuation, object, ticket, i);
	}

	// local light sources from a light group, if any
	if(!object->LLights.empty())
	{
		for(int i = 0; i < object->LLights.size(); i++)
			ComputeOneDiffuseLight(*object->LLights[i], reye, finish, ipoint, eye, layer_normal, layer_pigment_colour, colour, attenuation, object, ticket);
	}
}

void Trace::ComputePhotonDiffuseLight(const FINISH *Finish, const Vector3d& IPoint, const Ray& Eye, const Vector3d& Layer_Normal, const Vector3d& Raw_Normal,
                                      const RGBColour& Layer_Pigment_Colour, RGBColour& colour, double Attenuation, ConstObjectPtr Object, PhotonGatherer& gatherer)
{
	double Cos_Shadow_Angle;
	Ray Light_Source_Ray;
	Vector3d REye;
	RGBColour Light_Colour;
	RGBColour tmpCol, tmpCol2;
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

	if (Finish->Specular != 0.0)
	{
		REye[X] = -Eye.Direction[X];
		REye[Y] = -Eye.Direction[Y];
		REye[Z] = -Eye.Direction[Z];
	}

	if(gatherer.gathered)
		r = gatherer.alreadyGatheredRadius;
	else
		r = gatherer.gatherPhotonsAdaptive(*IPoint, *Layer_Normal, true);

	n = gatherer.gatheredPhotons.numFound;

	tmpCol.clear();

	// now go through these photons and add up their contribution
	for(j=0; j<n; j++)
	{
		// double theta,phi;
		int theta,phi;
		bool backside = false;

		// convert small color to normal color
		photonRgbe2colour(Light_Colour, gatherer.gatheredPhotons.photonGatherList[j]->colour);

		// convert theta/phi to vector direction
		// Use a pre-computed array of sin/cos to avoid many calls to the
		// sin() and cos() functions.  These arrays were initialized in
		// InitBacktraceEverything.
		theta = gatherer.gatheredPhotons.photonGatherList[j]->theta+127;
		phi = gatherer.gatheredPhotons.photonGatherList[j]->phi+127;

		Light_Source_Ray.Direction[Y] = sinCosData.sinTheta[theta];
		Light_Source_Ray.Direction[X] = sinCosData.cosTheta[theta];

		Light_Source_Ray.Direction[Z] = Light_Source_Ray.Direction[X]*sinCosData.sinTheta[phi];
		Light_Source_Ray.Direction[X] = Light_Source_Ray.Direction[X]*sinCosData.cosTheta[phi];

		VSub(Light_Source_Ray.Origin, gatherer.gatheredPhotons.photonGatherList[j]->Loc, Light_Source_Ray.Direction);

		// this compensates for real lambertian (diffuse) lighting (see paper)
		// use raw normal, not layer normal
		// att = dot(Layer_Normal, Vector3d(Light_Source_Ray.Direction));
		att = dot(Raw_Normal, Vector3d(Light_Source_Ray.Direction));
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
			Cos_Shadow_Angle = dot(Layer_Normal, Vector3d(Light_Source_Ray.Direction));
			if (Cos_Shadow_Angle < EPSILON)
			{
				if (Finish->DiffuseBack != 0.0)
					backside = true;
				else
					continue;
			}
		}

		// now add diffuse, phong, specular, irid contribution

		tmpCol2.clear();

		if (!(sceneData->useSubsurface && Finish->UseSubsurface))
			// (Diffuse contribution is not supported in combination with BSSRDF, to emphasize the fact that the BSSRDF
			// model is intended to provide for all the diffuse term by default. If users want to add some additional
			// surface-only diffuse term, they should use layered textures.
			ComputeDiffuseColour(Finish, Light_Source_Ray, Layer_Normal, tmpCol2, Light_Colour, Layer_Pigment_Colour, Attenuation, backside);

		// NK rad - don't compute highlights for radiosity gather rays, since this causes
		// problems with colors being far too bright
		// don't compute highlights for diffuse backside illumination
		if(!Eye.IsRadiosityRay() && !backside) // TODO FIXME radiosity - is this really the right way to do it (speaking of realism)?
		{
			if (Finish->Phong > 0.0)
			{
				Vector3d ed(Eye.Direction);
				ComputePhongColour(Finish, Light_Source_Ray, ed, Layer_Normal, tmpCol2, Light_Colour, Layer_Pigment_Colour);
			}
			if (Finish->Specular > 0.0)
			{
				ComputeSpecularColour(Finish, Light_Source_Ray, REye, Layer_Normal, tmpCol2, Light_Colour, Layer_Pigment_Colour);
			}
		}

		if (Finish->Irid > 0.0)
		{
			ComputeIridColour(Finish, Vector3d(Light_Source_Ray.Direction), Vector3d(Eye.Direction), Layer_Normal, IPoint, tmpCol2);
		}

		tmpCol += tmpCol2;
	}

	// finish the photons equation
	tmpCol /= M_PI*r*r;

	// add photon contribution to total lighting
	colour += tmpCol;
}

// see Diffuse_One_Light in the 3.6 code (lighting.cpp)
void Trace::ComputeOneDiffuseLight(const LightSource &lightsource, const Vector3d& reye, const FINISH *finish, const Vector3d& ipoint, const Ray& eye, const Vector3d& layer_normal,
                                   const RGBColour& layer_pigment_colour, RGBColour& colour, double attenuation, ConstObjectPtr object, TraceTicket& ticket, int light_index)
{
	double lightsourcedepth, cos_shadow_angle;
	Ray lightsourceray(eye);
	RGBColour lightcolour;
	bool backside = false;
	RGBColour tmpCol;

	// Get a colour and a ray.
	ComputeOneLightRay(lightsource, lightsourcedepth, lightsourceray, ipoint, lightcolour);

	// Don't calculate spotlights when outside of the light's cone.
	if((fabs(lightcolour.red())   < EPSILON) &&
	   (fabs(lightcolour.green()) < EPSILON) &&
	   (fabs(lightcolour.blue())  < EPSILON))
		return;

	// See if light on far side of surface from camera.
	if(!(Test_Flag(object, DOUBLE_ILLUMINATE_FLAG)) // NK 1998 double_illuminate - changed to Test_Flag
	   && !lightsource.Use_Full_Area_Lighting) // JN2007: Easiest way of getting rid of sharp shadow lines
	{
		cos_shadow_angle = dot(layer_normal, Vector3d(lightsourceray.Direction));
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
	if ((qualityFlags & Q_SHADOW) && ((lightsource.Projected_Through_Object != NULL) || (lightsource.Light_Type != FILL_LIGHT_SOURCE)))
	{
		if (lightColorCacheIndex != -1 && light_index != -1)
		{
			if (lightColorCache[lightColorCacheIndex][light_index].tested == false)
			{
				// note that lightColorCache may be re-sized during trace, so we don't store a reference to it across the call
				TraceShadowRay(lightsource, lightsourcedepth, lightsourceray, ipoint, lightcolour, ticket);
				lightColorCache[lightColorCacheIndex][light_index].tested = true;
				lightColorCache[lightColorCacheIndex][light_index].colour = lightcolour;
			}
			else
				lightcolour = lightColorCache[lightColorCacheIndex][light_index].colour;
		}
		else
			TraceShadowRay(lightsource, lightsourcedepth, lightsourceray, ipoint, lightcolour, ticket);
	}

	if((fabs(lightcolour.red())   > EPSILON) ||
	   (fabs(lightcolour.green()) > EPSILON) ||
	   (fabs(lightcolour.blue())  > EPSILON))
	{
		if(lightsource.Area_Light && lightsource.Use_Full_Area_Lighting &&
			(qualityFlags & Q_AREA_LIGHT)) // JN2007: Full area lighting
		{
			ComputeFullAreaDiffuseLight(lightsource, reye, finish, ipoint, eye,
				layer_normal, layer_pigment_colour, colour, attenuation,
				lightsourcedepth, lightsourceray, lightcolour,
				Test_Flag(object, DOUBLE_ILLUMINATE_FLAG));
			return;
		}

		if(!(sceneData->useSubsurface && finish->UseSubsurface))
			// (Diffuse contribution is not supported in combination with BSSRDF, to emphasize the fact that the BSSRDF
			// model is intended to provide for all the diffuse term by default. If users want to add some additional
			// surface-only diffuse term, they should use layered textures.
			ComputeDiffuseColour(finish, lightsourceray, layer_normal, tmpCol, lightcolour, layer_pigment_colour, attenuation, backside);

		// NK rad - don't compute highlights for radiosity gather rays, since this causes
		// problems with colors being far too bright
		// don't compute highlights for diffuse backside illumination
		if((lightsource.Light_Type != FILL_LIGHT_SOURCE) && !eye.IsRadiosityRay() && !backside) // TODO FIXME radiosity - is this really the right way to do it (speaking of realism)?
		{
			if(finish->Phong > 0.0)
			{
				Vector3d ed(eye.Direction);
				ComputePhongColour(finish, lightsourceray, ed, layer_normal, tmpCol, lightcolour, layer_pigment_colour);
			}

			if(finish->Specular > 0.0)
				ComputeSpecularColour(finish, lightsourceray, reye, layer_normal, tmpCol, lightcolour, layer_pigment_colour);
		}

		if(finish->Irid > 0.0)
			ComputeIridColour(finish, Vector3d(lightsourceray.Direction), Vector3d(eye.Direction), layer_normal, ipoint, tmpCol);
	}

	colour += tmpCol;
}

// JN2007: Full area lighting:
void Trace::ComputeFullAreaDiffuseLight(const LightSource &lightsource, const Vector3d& reye, const FINISH *finish, const Vector3d& ipoint, const Ray& eye,
                                        const Vector3d& layer_normal, const RGBColour& layer_pigment_colour, RGBColour& colour, double attenuation,
                                        double lightsourcedepth, Ray& lightsourceray, const RGBColour& lightcolour, bool isDoubleIlluminated)
{
	Vector3d temp;
	Vector3d axis1Temp, axis2Temp;
	double axis1_Length, cos_shadow_angle;

	axis1Temp = Vector3d(lightsource.Axis1);
	axis2Temp = Vector3d(lightsource.Axis2);

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

		axis1Temp = cross(Vector3d(lightsourceray.Direction), temp).normalized();

		// Make axis 2 be perpendicular with the light-ray and with Axis1.  A simple cross-product will do the trick.
		axis2Temp = cross(Vector3d(lightsourceray.Direction), axis1Temp).normalized();

		// make it square
		axis1Temp *= axis1_Length;
		axis2Temp *= axis1_Length;
	}

	RGBColour sampleLightcolour = lightcolour / (lightsource.Area_Size1 * lightsource.Area_Size2);
	RGBColour attenuatedLightcolour;

	for(int v = 0; v < lightsource.Area_Size2; ++v)
	{
		for(int u = 0; u < lightsource.Area_Size1; ++u)
		{
			Vector3d jitterAxis1, jitterAxis2;
			Ray lsr(lightsourceray);
			double jitter_u = (double)u;
			double jitter_v = (double)v;
			bool backside = false;
			RGBColour tmpCol;

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
			if(!isDoubleIlluminated)
			{
				cos_shadow_angle = dot(layer_normal, Vector3d(lsr.Direction));
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
				ComputeDiffuseColour(finish, lsr, layer_normal, tmpCol, attenuatedLightcolour, layer_pigment_colour, attenuation, backside);

			// NK rad - don't compute highlights for radiosity gather rays, since this causes
			// problems with colors being far too bright
			// don't compute highlights for diffuse backside illumination
			if((lightsource.Light_Type != FILL_LIGHT_SOURCE) && !eye.IsRadiosityRay() && !backside) // TODO FIXME radiosity - is this really the right way to do it (speaking of realism)?
			{
				if(finish->Phong > 0.0)
				{
					Vector3d ed(eye.Direction);
					ComputePhongColour(finish, lsr, ed, layer_normal, tmpCol, attenuatedLightcolour, layer_pigment_colour);
				}

				if(finish->Specular > 0.0)
					ComputeSpecularColour(finish, lsr, reye, layer_normal, tmpCol, attenuatedLightcolour, layer_pigment_colour);
			}

			if(finish->Irid > 0.0)
				ComputeIridColour(finish, Vector3d(lightsourceray.Direction), Vector3d(eye.Direction), layer_normal, ipoint, tmpCol);

			colour += tmpCol;
		}
	}
}

// see do_light in version 3.6's lighting.cpp
void Trace::ComputeOneLightRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray, const Vector3d& ipoint, RGBColour& lightcolour, bool forceAttenuate)
{
	double attenuation;
	ComputeOneWhiteLightRay(lightsource, lightsourcedepth, lightsourceray, ipoint);

	// Get the light source colour.
	lightcolour = lightsource.colour;

	// Attenuate light source color.
	if (lightsource.Area_Light && lightsource.Use_Full_Area_Lighting && (qualityFlags & Q_AREA_LIGHT) && !forceAttenuate)
		// for full area lighting we apply distance- and angle-based attenuation to each "lightlet" individually later
		attenuation = 1.0;
	else
		attenuation = Attenuate_Light(&lightsource, lightsourceray, lightsourcedepth);

	// Now scale the color by the attenuation
	lightcolour *= attenuation;
}

// see block_light_source in the version 3.6 source
void Trace::TraceShadowRay(const LightSource &lightsource, double depth, const Ray& lightsourceray, const Vector3d& point, RGBColour& colour, TraceTicket& ticket)
{
	// test and set highest level traced. We do it differently than TraceRay() does,
	// for compatibility with the way max_trace_level is tested and reported in v3.6
	// and earlier.
	if(ticket.traceLevel > ticket.maxAllowedTraceLevel)
	{
		colour.clear();
		return;
	}

	ticket.maxFoundTraceLevel = (unsigned int) max(ticket.maxFoundTraceLevel, ticket.traceLevel);
	ticket.traceLevel++;

	double newdepth;
	Intersection isect;
	Ray newray(lightsourceray);

	// Store current depth and ray because they will be modified.
	newdepth = depth;

	// NOTE: shadow rays are never photon rays, so flag can be hard-coded to false
	newray.SetFlags(Ray::OtherRay, true, false);

	// Get shadows from current light source.
	if((lightsource.Area_Light) && (qualityFlags & Q_AREA_LIGHT))
		TraceAreaLightShadowRay(lightsource, newdepth, newray, point, colour, ticket);
	else
		TracePointLightShadowRay(lightsource, newdepth, newray, colour, ticket);

	// If there's some distance left for the ray to reach the light source
	// we have to apply atmospheric stuff to this part of the ray.

	if((newdepth > SHADOW_TOLERANCE) && (lightsource.Media_Interaction) && (lightsource.Media_Attenuation))
	{
		isect.Depth = newdepth;
		isect.Object = NULL;
		ComputeShadowMedia(newray, isect, colour, (lightsource.Media_Interaction) && (lightsource.Media_Attenuation), ticket);
	}

	ticket.traceLevel--;
	maxFoundTraceLevel = (unsigned int) max(maxFoundTraceLevel, ticket.maxFoundTraceLevel);
}

// moved this here (was originally inside TracePointLightShadowRay) because
// for some reason the Intel compiler (version W_CC_PC_8.1.027) will fail
// to link the exe, complaining of an unresolved external.
//
// TODO: try moving it back in at some point in the future.
struct NoShadowFlagRayObjectCondition : public RayObjectCondition
{
	virtual bool operator()(const Ray&, const ObjectBase* object, double) const { return !Test_Flag(object, NO_SHADOW_FLAG); }
};

struct SmallToleranceRayObjectCondition : public RayObjectCondition
{
	virtual bool operator()(const Ray&, const ObjectBase*, double dist) const { return dist > SMALL_TOLERANCE; }
};

void Trace::TracePointLightShadowRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray, RGBColour& lightcolour, TraceTicket& ticket)
{
	Intersection boundedIntersection;
	ObjectPtr cacheObject = NULL;
	bool foundTransparentObjects = false;
	bool foundIntersection;

	// Projected through main tests
	double projectedDepth = 0.0;

	if(lightsource.Projected_Through_Object != NULL)
	{
		Intersection tempIntersection;

		if(FindIntersection(lightsource.Projected_Through_Object, tempIntersection, lightsourceray))
		{
			if((tempIntersection.Depth - lightsourcedepth) < 0.0)
				projectedDepth = lightsourcedepth - fabs(tempIntersection.Depth) + SMALL_TOLERANCE;
			else
			{
				lightcolour.clear();
				return;
			}
		}
		else
		{
			lightcolour.clear();
			return;
		}

		// Make sure we don't do shadows for fill light sources.
		// (Note that if Projected_Through_Object is NULL, the test for FILL_LIGHT_SOURCE has happened earlier already)
		if(lightsource.Light_Type == FILL_LIGHT_SOURCE)
			return;
	}

	NoShadowFlagRayObjectCondition precond;
	SmallToleranceRayObjectCondition postcond;

	// check for object in the light source shadow cache (object that fully shadowed during last test) first

	if(lightsource.lightGroupLight == false) // we don't cache for light groups
	{
		if((ticket.traceLevel == 2) && (lightSourceLevel1ShadowCache[lightsource.index] != NULL))
			cacheObject = lightSourceLevel1ShadowCache[lightsource.index];
		else if(lightSourceOtherShadowCache[lightsource.index] != NULL)
			cacheObject = lightSourceOtherShadowCache[lightsource.index];

		// if there was an object in the light source shadow cache, check that first
		if(cacheObject != NULL)
		{
			if(FindIntersection(cacheObject, boundedIntersection, lightsourceray, lightsourcedepth - projectedDepth) == true)
			{
				if(!Test_Flag(boundedIntersection.Object, NO_SHADOW_FLAG))
				{
					ComputeShadowColour(lightsource, boundedIntersection, lightsourceray, lightcolour, ticket);

					if((fabs(lightcolour.red())   < EPSILON) &&
					   (fabs(lightcolour.green()) < EPSILON) &&
					   (fabs(lightcolour.blue())  < EPSILON) &&
					   (Test_Flag(boundedIntersection.Object, OPAQUE_FLAG)))
					{
						threadData->Stats()[Shadow_Ray_Tests]++;
						threadData->Stats()[Shadow_Rays_Succeeded]++;
						threadData->Stats()[Shadow_Cache_Hits]++;
						return;
					}
				}
				else
					cacheObject = NULL;
			}
			else
				cacheObject = NULL;
		}
	}

	foundTransparentObjects = false;

	while(true)
	{
		boundedIntersection.Object = boundedIntersection.Csg = NULL;
		boundedIntersection.Depth = lightsourcedepth - projectedDepth;

		threadData->Stats()[Shadow_Ray_Tests]++;

		foundIntersection = FindIntersection(boundedIntersection, lightsourceray, precond, postcond);

		if((foundIntersection == true) && (boundedIntersection.Object != cacheObject) &&
		   (boundedIntersection.Depth < lightsourcedepth - SHADOW_TOLERANCE) &&
		   (lightsourcedepth - boundedIntersection.Depth > projectedDepth) &&
		   (boundedIntersection.Depth > SHADOW_TOLERANCE))
		{
			threadData->Stats()[Shadow_Rays_Succeeded]++;

			ComputeShadowColour(lightsource, boundedIntersection, lightsourceray, lightcolour, ticket);

			ObjectPtr testObject(boundedIntersection.Csg != NULL ? boundedIntersection.Csg : boundedIntersection.Object);

			if((fabs(lightcolour.red())   < EPSILON) &&
			   (fabs(lightcolour.green()) < EPSILON) &&
			   (fabs(lightcolour.blue())  < EPSILON) &&
			   (Test_Flag(testObject, OPAQUE_FLAG)))
			{
				// Hit a fully opaque object; cache that object, so that next time we can test for it first;
				// don't cache for light groups though (why not??)

				if((lightsource.lightGroupLight == false) && (foundTransparentObjects == false))
				{
					cacheObject = testObject;

					if(ticket.traceLevel == 2)
						lightSourceLevel1ShadowCache[lightsource.index] = cacheObject;
					else
						lightSourceOtherShadowCache[lightsource.index] = cacheObject;
				}
				break;
			}

			foundTransparentObjects = true;

			// Move the ray to the point of intersection, plus some
			lightsourcedepth -= boundedIntersection.Depth;

			Assign_Vector(lightsourceray.Origin, boundedIntersection.IPoint);
		}
		else
			// No further intersections in the direction of the ray.
			break;
	}
}

void Trace::TraceAreaLightShadowRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray,
                                    const Vector3d& ipoint, RGBColour& lightcolour, TraceTicket& ticket)
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
			lightGrid[i * lightsource.Area_Size2 + j].red() = -1.0; // TODO FIXME - Old bug: This will not work with negative color values! [trf]
	}
	*/
	for(size_t ind = 0; ind < lightGrid.size(); ++ind)
		lightGrid[ind].red() = -1.0;

	axis1Temp = Vector3d(lightsource.Axis1);
	axis2Temp = Vector3d(lightsource.Axis2);

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

		axis1Temp = cross(Vector3d(lightsourceray.Direction), temp).normalized();

		// Make axis 2 be perpendicular with the light-ray and with Axis1.  A simple cross-product will do the trick.
		axis2Temp = cross(Vector3d(lightsourceray.Direction), axis1Temp).normalized();

		// make it square
		axis1Temp *= axis1_Length;
		axis2Temp *= axis1_Length;
	}

	TraceAreaLightSubsetShadowRay(lightsource, lightsourcedepth, lightsourceray, ipoint, lightcolour, 0, 0, lightsource.Area_Size1 - 1, lightsource.Area_Size2 - 1, 0, axis1Temp, axis2Temp, ticket);
}

void Trace::TraceAreaLightSubsetShadowRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray,
                                    const Vector3d& ipoint, RGBColour& lightcolour, int u1, int  v1, int  u2, int  v2, int level, const Vector3d& axis1, const Vector3d& axis2, TraceTicket& ticket)
{
	RGBColour sample_Colour[4];
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

		if(lightGrid[u * lightsource.Area_Size2 + v].red() >= 0.0)
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

			TracePointLightShadowRay(lightsource, lightsourcedepth, lsr, sample_Colour[i], ticket);

			lightGrid[u * lightsource.Area_Size2 + v] = sample_Colour[i];
		}
	}

	if((u2 - u1 > 1) || (v2 - v1 > 1))
	{
		if((level < lightsource.Adaptive_Level) ||
		   (colourDistance(sample_Colour[0], sample_Colour[1]) > 0.1) ||
		   (colourDistance(sample_Colour[1], sample_Colour[3]) > 0.1) ||
		   (colourDistance(sample_Colour[3], sample_Colour[2]) > 0.1) ||
		   (colourDistance(sample_Colour[2], sample_Colour[0]) > 0.1))
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
				                              ipoint, sample_Colour[i], new_u1, new_v1, new_u2, new_v2, level + 1, axis1, axis2, ticket);
			}
		}
	}

	// Average up the light contributions
	lightcolour = (sample_Colour[0] + sample_Colour[1] + sample_Colour[2] + sample_Colour[3]) * 0.25;
}

// see filter_shadow_ray in version 3.6's lighting.cpp
void Trace::ComputeShadowColour(const LightSource &lightsource, Intersection& isect, Ray& lightsourceray, RGBColour& colour, TraceTicket& ticket)
{
	WeightedTextureVector wtextures;
	Vector3d ipoint;
	Vector3d raw_Normal;
	Colour fc1, temp_Colour;
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
		colour.clear();
		return;
	}

	ipoint = Vector3d(isect.IPoint);

	if(!(qualityFlags & Q_SHADOW))
		// no shadow
		return;

	// If the object is opaque there's no need to go any further. [DB 8/94]
	if(Test_Flag(isect.Object, OPAQUE_FLAG))
	{
		// full shadow
		colour.clear();
		return;
	}

	// Get the normal to the surface
	isect.Object->Normal(*raw_Normal, &isect, threadData);

	// I added this to flip the normal if the object is inverted (for CSG).
	// However, I subsequently commented it out for speed reasons - it doesn't
	// make a difference (no pun intended). The preexisting flip code below
	// produces a similar (though more extensive) result. [NK]
	//
	// Actually, we should keep this code to guarantee that normaldirection
	// is set properly. [NK]
	if(Test_Flag(isect.Object, INVERTED_FLAG))
		raw_Normal = -raw_Normal;

	// If the surface normal points away, flip its direction.
	normaldirection = dot(raw_Normal, Vector3d(lightsourceray.Direction));
	if(normaldirection > 0.0)
		raw_Normal = -raw_Normal;

	Assign_Vector(isect.INormal, *raw_Normal);
	// and save to intersection -hdf-
	Assign_Vector(isect.PNormal, *raw_Normal);

	if(Test_Flag(isect.Object, UV_FLAG))
	{
		// get the UV vect of the intersection
		isect.Object->UVCoord(*uv_Coords, &isect, threadData);
		// save the normal and UV coords into Intersection
		Assign_UV_Vect(isect.Iuv, *uv_Coords);
	}

	// now switch to UV mapping if we need to
	if(Test_Flag(isect.Object, UV_FLAG))
	{
		ipoint[X] = uv_Coords[U];
		ipoint[Y] = uv_Coords[V];
		ipoint[Z] = 0;
	}

	// NB the 3.6 code doesn't set the light cache's Tested flags to false after incrementing the level.
	if (++lightColorCacheIndex >= lightColorCache.size())
	{
		lightColorCache.resize(lightColorCacheIndex + 10);
		for (LightColorCacheListList::iterator it = lightColorCache.begin() + lightColorCacheIndex; it != lightColorCache.end(); it++)
			it->resize(lightColorCache[0].size());
	}

	bool isMultiTextured = Test_Flag(isect.Object, MULTITEXTURE_FLAG) ||
	                       ((isect.Object->Texture == NULL) && Test_Flag(isect.Object, CUTAWAY_TEXTURES_FLAG));

	// get textures and weights
	if(isMultiTextured == true)
	{
		isect.Object->Determine_Textures(&isect, normaldirection > 0.0, wtextures, threadData);
	}
	else if(isect.Object->Texture != NULL)
	{
		if((normaldirection > 0.0) && (isect.Object->Interior_Texture != NULL))
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

	temp_Colour.clear();

	for(WeightedTextureVector::iterator i(wtextures.begin()); i != wtextures.end(); i++)
	{
		TextureVector warps(texturePool);
		assert(warps->empty()); // verify that the TextureVector pulled from the pool is in a cleaned-up condition

		// If contribution of this texture is neglectable skip ahead.
		if((i->weight < ticket.adcBailout) || (i->texture == NULL))
			continue;

		ComputeOneTextureColour(fc1, i->texture, *warps, ipoint, raw_Normal, lightsourceray, 0.0, isect, true, false, ticket);

		temp_Colour += i->weight * fc1;
	}

	lightColorCacheIndex--;

	if(fabs(temp_Colour.filter()) + fabs(temp_Colour.transm()) < ticket.adcBailout)
	{
		// close enough to full shadow - bail out to avoid media computations
		colour.clear();
		return;
	}

#if MEDIA_AFTER_TEXTURE_INTERPOLATION
	// [CLi] moved this here from Trace::ComputeShadowTexture() and Trace::ComputeLightedTexture(), respectively,
	// to avoid media to be computed twice when dealing with averaged textures.
	// TODO - For photon rays we're still potentially doing double work on media.
	// TODO - For shadow rays we're still potentially doing double work on distance-based attenuation.
	// Calculate participating media effects.
	if((qualityFlags & Q_VOLUME) && (!lightsourceray.GetInteriors().empty()) && (lightsourceray.IsHollowRay() == true))
		media.ComputeMedia(lightsourceray.GetInteriors(), lightsourceray, isect, temp_Colour, ticket);
#endif

	colour *= temp_Colour.rgbTransm();

	// Get atmospheric attenuation.
	ComputeShadowMedia(lightsourceray, isect, colour, (lightsource.Media_Interaction) && (lightsource.Media_Attenuation), ticket);
}

void Trace::ComputeDiffuseColour(const FINISH *finish, const Ray& lightsourceray, const Vector3d& layer_normal, RGBColour& colour, const RGBColour& light_colour,
                                 const RGBColour& layer_pigment_colour, double attenuation, bool backside)
{
	double cos_angle_of_incidence, intensity;
	double diffuse = (backside? finish->DiffuseBack : finish->Diffuse);

	if (diffuse <= 0.0)
		return;

	cos_angle_of_incidence = dot(layer_normal, Vector3d(lightsourceray.Direction));

	// Brilliance is likely to be 1.0 (default value)
	if(finish->Brilliance != 1.0)
		intensity = pow(fabs(cos_angle_of_incidence), (double) finish->Brilliance);
	else
		intensity = fabs(cos_angle_of_incidence);

	intensity *= diffuse * attenuation;

	if(finish->Crand > 0.0)
		intensity -= POV_rand(crandRandomNumberGenerator) * finish->Crand;

	colour += intensity * layer_pigment_colour * light_colour;
}

void Trace::ComputeIridColour(const FINISH *finish, const Vector3d& lightsource, const Vector3d& eye, const Vector3d& layer_normal, const Vector3d& ipoint, RGBColour& colour)
{
	double rwl, gwl, bwl;
	double cos_angle_of_incidence_light, cos_angle_of_incidence_eye, interference;
	double film_thickness;
	double noise;
	TURB turb;

	film_thickness = finish->Irid_Film_Thickness;

	if(finish->Irid_Turb != 0)
	{
		// Uses hardcoded octaves, lambda, omega
		turb.Omega=0.5;
		turb.Lambda=2.0;
		turb.Octaves=5;

		// Turbulence() returns a value from 0..1, so noise will be in order of magnitude 1.0 +/- finish->Irid_Turb
		noise = Turbulence(*ipoint, &turb, sceneData->noiseGenerator);
		noise = 2.0 * noise - 1.0;
		noise = 1.0 + noise * finish->Irid_Turb;
		film_thickness *= noise;
	}

	// Approximate dominant wavelengths of primary hues.
	// Source: 3D Computer Graphics by John Vince (Addison Wesely)
	// These are initialized in parse.c (Parse_Frame)
	// and are user-adjustable with the irid_wavelength keyword.
	// Red = 700 nm  Grn = 520 nm Blu = 480 nm
	// Divided by 1000 gives: rwl = 0.70;  gwl = 0.52;  bwl = 0.48;
	//
	// However... I originally "guessed" at the values and came up with
	// the following, which I'm using as the defaults, since it seems
	// to work better:  rwl = 0.25;  gwl = 0.18;  bwl = 0.14;

	// Could avoid these assignments if we want to
	rwl = sceneData->iridWavelengths.red();
	gwl = sceneData->iridWavelengths.green();
	bwl = sceneData->iridWavelengths.blue();

	// NOTE: Shouldn't we compute Cos_Angle_Of_Incidence just once?
	cos_angle_of_incidence_light = std::abs(dot(layer_normal, lightsource));
	cos_angle_of_incidence_eye   = std::abs(dot(layer_normal, eye));

	// Calculate phase offset.
	interference = 2.0 * M_PI * film_thickness * (cos_angle_of_incidence_light + cos_angle_of_incidence_eye);

//	intensity = cos_angle_of_incidence * finish->Irid; // TODO CLARIFY - [CLi] note that this effectively gets finish->Irid squared; is this intentional?

	// Modify color by phase offset for each wavelength.
	colour *= RGBColour(1.0 + finish->Irid * cos(interference / rwl),
	                    1.0 + finish->Irid * cos(interference / gwl),
	                    1.0 + finish->Irid * cos(interference / bwl));
}

void Trace::ComputePhongColour(const FINISH *finish, const Ray& lightsourceray, const Vector3d& eye, const Vector3d& layer_normal, RGBColour& colour, const RGBColour& light_colour,
                               const RGBColour& layer_pigment_colour)
{
	double cos_angle_of_incidence, intensity;
	Vector3d reflect_direction;
	double ndotl, x, f;
	RGBColour cs;

	cos_angle_of_incidence = -2.0 * dot(eye, layer_normal);

	reflect_direction = eye + cos_angle_of_incidence * layer_normal;

	cos_angle_of_incidence = dot(reflect_direction, Vector3d(lightsourceray.Direction));

	if(cos_angle_of_incidence > 0.0)
	{
		if((finish->Phong_Size < 60) || (cos_angle_of_incidence > 0.0008)) // rgs
			intensity = finish->Phong * pow(cos_angle_of_incidence, (double)finish->Phong_Size);
		else
			intensity = 0.0; // ad

		if(finish->Metallic > 0.0)
		{
			// Calculate the reflected color by interpolating between
			// the light source color and the surface color according
			// to the (empirical) Fresnel reflectivity function. [DB 9/94]

			ndotl = dot(layer_normal, Vector3d(lightsourceray.Direction));

			x = fabs(acos(ndotl)) / M_PI_2;

			f = 0.014567225 / Sqr(x - 1.12) - 0.011612903;

			f = min(1.0, max(0.0, f));
			cs = light_colour * ( RGBColour(1.0) + (finish->Metallic * (1.0 - f)) * (layer_pigment_colour - RGBColour(1.0)) );

			colour += intensity * cs;
		}
		else
			colour += intensity * light_colour;
	}
}

void Trace::ComputeSpecularColour(const FINISH *finish, const Ray& lightsourceray, const Vector3d& eye, const Vector3d& layer_normal, RGBColour& colour,
                                  const RGBColour& light_colour, const RGBColour& layer_pigment_colour)
{
	double cos_angle_of_incidence, intensity, halfway_length;
	Vector3d halfway;
	double ndotl, x, f;
	RGBColour cs;

	VHalf(*halfway, *eye, lightsourceray.Direction);

	halfway_length = halfway.length();

	if(halfway_length > 0.0)
	{
		cos_angle_of_incidence = dot(halfway, layer_normal) / halfway_length;

		if(cos_angle_of_incidence > 0.0)
		{
			intensity = finish->Specular * pow(cos_angle_of_incidence, (double)finish->Roughness);

			if(finish->Metallic > 0.0)
			{
				// Calculate the reflected color by interpolating between
				// the light source color and the surface color according
				// to the (empirical) Fresnel reflectivity function. [DB 9/94]

				ndotl = dot(layer_normal, Vector3d(lightsourceray.Direction));

				x = fabs(acos(ndotl)) / M_PI_2;

				f = 0.014567225 / Sqr(x - 1.12) - 0.011612903;

				f = min(1.0, max(0.0, f));
				cs = light_colour * ( RGBColour(1.0) + (finish->Metallic * (1.0 - f)) * (layer_pigment_colour - RGBColour(1.0)) );

				colour += intensity * cs;
			}
			else
				colour += intensity * light_colour;
		}
	}
}

void Trace::ComputeRelativeIOR(const Ray& ray, const Interior* interior, double& ior)
{
	// Get ratio of iors depending on the interiors the ray is traversing.
	if (interior == NULL)
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

void Trace::ComputeReflectivity(double& weight, RGBColour& reflectivity, const RGBColour& reflection_max, const RGBColour& reflection_min,
                                int reflection_type, double reflection_falloff, double cos_angle, const Ray& ray, const Interior* interior)
{
	double temp_Weight_Min, temp_Weight_Max;
	double reflection_Frac;
	double g, f;
	double ior;

	if(reflection_type == 1)
		ComputeRelativeIOR(ray, interior, ior);

	switch(reflection_type)
	{
		case 0: // Standard reflection
		{
			temp_Weight_Max = max3(reflection_max.red(), reflection_max.green(), reflection_max.blue());
			temp_Weight_Min = max3(reflection_min.red(), reflection_min.green(), reflection_min.blue());
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
			break;
		}
		case 1:  // Fresnel
		{
			// NB: This is a special case of the Fresnel formula, presuming that incident light is unpolarized.
			//
			// The implemented formula is as follows:
			//
			//      1     ( g - cos Ti )^2           ( cos Ti (g + cos Ti) - 1 )^2
			// R = --- * ------------------ * ( 1 + ------------------------------- )
			//      2     ( g + cos Ti )^2           ( cos Ti (g - cos Ti) + 1 )^2
			//
			// where
			//
			//        /---------------------------
			// g = -\/ (n1/n2)^2 + (cos Ti)^2 - 1

			// Christoph's tweak to work around possible negative argument in sqrt
			double sqx = Sqr(ior) + Sqr(cos_angle) - 1.0;

			if(sqx > 0.0)
			{
				g = sqrt(sqx);
				f = 0.5 * (Sqr(g - cos_angle) / Sqr(g + cos_angle));
				f = f * (1.0 + Sqr(cos_angle * (g + cos_angle) - 1.0) / Sqr(cos_angle * (g - cos_angle) + 1.0));

				f = min(1.0, max(0.0, f));
				reflectivity = f * reflection_max + (1.0 - f) * reflection_min;
			}
			else
				reflectivity = reflection_max;

			weight = weight * max3(reflectivity.red(), reflectivity.green(), reflectivity.blue());
			break;
		}
		default:
			throw POV_EXCEPTION_STRING("Illegal reflection_type."); // TODO FIXME - wrong place to report this [trf]
	}
}

void Trace::ComputeOneWhiteLightRay(const LightSource &lightsource, double& lightsourcedepth, Ray& lightsourceray, const Vector3d& ipoint, const Vector3d& jitter)
{
	Vector3d center = Vector3d(lightsource.Center) + jitter;
	double a;
	Vector3d v1;

	// Get the light ray starting at the intersection point and pointing towards the light source.
	Assign_Vector(lightsourceray.Origin, *ipoint);
	// NK 1998 parallel beams for cylinder source - added 'if'
	if(lightsource.Light_Type == CYLINDER_SOURCE)
	{
		double distToPointsAt;
		Vector3d toLightCtr;

		// use new code to get ray direction - use center - points_at for direction
		VSub(lightsourceray.Direction, *center, lightsource.Points_At);

		// get vector pointing to center of light
		toLightCtr = center - ipoint;

		// project light_ctr-intersect_point onto light_ctr-point_at
		distToPointsAt = Vector3d(lightsourceray.Direction).length();
		lightsourcedepth = dot(toLightCtr, Vector3d(lightsourceray.Direction));

		// lenght of shadow ray is the length of the projection
		lightsourcedepth /= distToPointsAt;
		VNormalizeEq(lightsourceray.Direction);
	}
	else
	{
		// NK 1998 parallel beams for cylinder source - the stuff in this 'else'
		// block used to be all that there was... the first half of the if
		// statement (before the 'else') is new
		VSub(lightsourceray.Direction, *center, *ipoint);
		lightsourcedepth = Vector3d(lightsourceray.Direction).length();
		VInverseScaleEq(lightsourceray.Direction, lightsourcedepth);
	}

	// Attenuate light source color.
	// Attenuation = Attenuate_Light(lightsource, lightsourceray, *Light_Source_Depth);
	// Recalculate for Parallel light sources
	if(lightsource.Parallel)
	{
		if(lightsource.Area_Light)
		{
			v1 = (center - Vector3d(lightsource.Points_At)).normalized();
			a = dot(v1, Vector3d(lightsourceray.Direction));
			lightsourcedepth *= a;
			Assign_Vector(lightsourceray.Direction, *v1);
		}
		else
		{
			a = dot(Vector3d(lightsource.Direction), Vector3d(lightsourceray.Direction));
			lightsourcedepth *= (-a);
			Assign_Vector(lightsourceray.Direction, lightsource.Direction);
			VScaleEq(lightsourceray.Direction, -1.0);
		}
	}
}

void Trace::ComputeSky(const Ray& ray, Colour& colour, TraceTicket& ticket)
{
	if (sceneData->EffectiveLanguageVersion() < 370)
	{
		// this gives the same results regarding sky sphere filter as how v3.6 did it

		int i;
		double att, trans;
		RGBColour col;
		Colour col_Temp, filterc;
		Vector3d p;

		if (ticket.alphaBackground)
		{
			// If rendering with alpha channel, just return full transparency.
			// (As we're working with associated alpha internally, the respective color must be black here.)
			colour = Colour(0.0, 0.0, 0.0, 0.0, 1.0);
			return;
		}

		colour = sceneData->backgroundColour;

		if((sceneData->skysphere == NULL) || (sceneData->skysphere->Pigments == NULL))
			return;

		col.clear();
		filterc = Colour(1.0, 1.0, 1.0, 1.0, 1.0);
		trans = 1.0;

		// Transform point on unit sphere.
		if(sceneData->skysphere->Trans != NULL)
			MInvTransPoint(*p, ray.Direction, sceneData->skysphere->Trans);
		else
			p = Vector3d(ray.Direction);

		for(i = sceneData->skysphere->Count - 1; i >= 0; i--)
		{
			// Compute sky colour from colour map.

			// NK 1998 - added NULL as final parameter
			Compute_Pigment(col_Temp, sceneData->skysphere->Pigments[i], *p, NULL, NULL, threadData);

			att = trans * (1.0 - col_Temp.filter() - col_Temp.transm());

			col += RGBColour(col_Temp) * att;

			filterc *= col_Temp;

			trans = fabs(filterc.filter()) + fabs(filterc.transm());
		}

		col *= sceneData->skysphere->Emission;

		colour.red()    = col.red()    + colour.red()   * (filterc.red()   * filterc.filter() + filterc.transm());
		colour.green()  = col.green()  + colour.green() * (filterc.green() * filterc.filter() + filterc.transm());
		colour.blue()   = col.blue()   + colour.blue()  * (filterc.blue()  * filterc.filter() + filterc.transm());
		colour.filter() = colour.filter() * filterc.filter();
		colour.transm() = colour.transm() * filterc.transm();
	}
	else // i.e. sceneData->languageVersion >= 370
	{
		// this gives the same results regarding sky sphere filter as a layered-texture genuine sphere

		int i;
		RGBColour filCol(1.0);
		double att;
		RGBColour col;
		Colour col_Temp;
		Vector3d p;

		col.clear();

		if((sceneData->skysphere != NULL) && (sceneData->skysphere->Pigments != NULL))
		{
			// Transform point on unit sphere.
			if(sceneData->skysphere->Trans != NULL)
				MInvTransPoint(*p, ray.Direction, sceneData->skysphere->Trans);
			else
				p = Vector3d(ray.Direction);

			for(i = sceneData->skysphere->Count - 1; i >= 0; i--)
			{
				// Compute sky colour from colour map.
				Compute_Pigment(col_Temp, sceneData->skysphere->Pigments[i], *p, NULL, NULL, threadData);

				att = col_Temp.opacity();

				col += RGBColour(col_Temp) * att * filCol * sceneData->skysphere->Emission;
				filCol *= col_Temp.rgbTransm();
			}
		}

		// apply background as if it was another sky sphere with uniform pigment
		col_Temp = sceneData->backgroundColour;
		if (!ticket.alphaBackground)
		{
			// if rendering without alpha channel, ignore filter and transmit of background color.
			col_Temp.filter() = 0.0;
			col_Temp.transm() = 0.0;
		}

		att = col_Temp.opacity();

		col += RGBColour(col_Temp) * att * filCol;
		filCol *= col_Temp.rgbTransm();

		colour.red()    = col.red();
		colour.green()  = col.green();
		colour.blue()   = col.blue();
		colour.filter() = 0.0;
		colour.transm() = min(1.0f, std::fabs(filCol.greyscale()));
	}
}

void Trace::ComputeFog(const Ray& ray, const Intersection& isect, Colour& colour)
{
	double att, att_inv, width;
	Colour col_fog;
	RGBColour sum_att; // total attenuation.
	RGBColour sum_col; // total color.

	// Why are we here.
	if(sceneData->fog == NULL)
		return;

	// Init total attenuation and total color.
	sum_att = RGBColour(1.0, 1.0, 1.0);
	sum_col = RGBColour(0.0, 0.0, 0.0);

	// Loop over all fogs.
	for(FOG *fog = sceneData->fog; fog != NULL; fog = fog->Next)
	{
		// Don't care about fogs with zero distance.
		if(fabs(fog->Distance) > EPSILON)
		{
			width = isect.Depth;

			switch(fog->Type)
			{
				case GROUND_MIST:
					att = ComputeGroundFogColour(ray, 0.0, width, fog, col_fog);
					break;
				default:
					att = ComputeConstantFogColour(ray, 0.0, width, fog, col_fog);
					break;
			}

			// Check for minimum transmittance.
			if(att < col_fog.transm())
				att = col_fog.transm();

			// Get attenuation sum due to filtered/unfiltered translucency.
			// [CLi] removed computation of sum_att.filer() and sum_att.transm(), as they were discarded anyway
			sum_att.red()    *= att * ((1.0 - col_fog.filter()) + col_fog.filter() * col_fog.red());
			sum_att.green()  *= att * ((1.0 - col_fog.filter()) + col_fog.filter() * col_fog.green());
			sum_att.blue()   *= att * ((1.0 - col_fog.filter()) + col_fog.filter() * col_fog.blue());

			if(!ray.IsShadowTestRay())
			{
				att_inv = 1.0 - att;
				sum_col += att_inv * RGBColour(col_fog);
			}
		}
	}

	// Add light coming from background.
	colour.red()   = sum_col.red()   + sum_att.red()   * colour.red();
	colour.green() = sum_col.green() + sum_att.green() * colour.green();
	colour.blue()  = sum_col.blue()  + sum_att.blue()  * colour.blue();
	colour.transm() *= sum_att.greyscale();
}

double Trace::ComputeConstantFogColour(const Ray &ray, double depth, double width, const FOG *fog, Colour& colour)
{
	Vector3d p;
	double k;

	if(fog->Turb != NULL)
	{
		depth += width / 2.0;

		VEvaluateRay(*p, ray.Origin, depth, ray.Direction);
		VEvaluateEq(*p, fog->Turb->Turbulence);

		// The further away the less influence turbulence has.
		k = exp(-width / fog->Distance);

		width *= (1.0 - k * min(1.0, Turbulence(*p, fog->Turb, sceneData->noiseGenerator) * fog->Turb_Depth));
	}

	colour = fog->colour;

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

double Trace::ComputeGroundFogColour(const Ray& ray, double depth, double width, const FOG *fog, Colour& colour)
{
	double fog_density, delta;
	double start, end;
	double y1, y2, k;
	Vector3d p, p1, p2;

	// Get start point.
	VEvaluateRay(*p1, ray.Origin, depth, ray.Direction);

	// Get end point.
	p2 = p1 + Vector3d(ray.Direction) * width;

	// Could preform transfomation here to translate Start and End
	// points into ground fog space.
	y1 = dot(p1, Vector3d(fog->Up));
	y2 = dot(p2, Vector3d(fog->Up));

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
	if (fog->Turb != NULL)
	{
		p = (p1 + p2) * 0.5;
		VEvaluateEq(*p, fog->Turb->Turbulence);

		// The further away the less influence turbulence has.
		k = exp(-width / fog->Distance);
		width *= (1.0 - k * min(1.0, Turbulence(*p, fog->Turb, sceneData->noiseGenerator) * fog->Turb_Depth));
	}

	colour = fog->colour;

	return (exp(-width * fog_density / fog->Distance));
}

void Trace::ComputeShadowMedia(Ray& light_source_ray, Intersection& isect, RGBColour& resultcolour, bool media_attenuation_and_interaction, TraceTicket& ticket)
{
	if((resultcolour.red() < EPSILON) && (resultcolour.green() < EPSILON) && (resultcolour.blue() < EPSILON))
		return;

	// Calculate participating media effects.
	if(media_attenuation_and_interaction && (qualityFlags & Q_VOLUME) && ((light_source_ray.IsHollowRay() == true) || (isect.Object != NULL && isect.Object->interior != NULL)))
	{
		// we're using general-purpose media and fog handling code, which insists on computing a transmissive component (for alpha channel)
		Colour tmpCol = Colour(resultcolour);

		media.ComputeMedia(sceneData->atmosphere, light_source_ray, isect, tmpCol, ticket);

		if((sceneData->fog != NULL) && (light_source_ray.IsHollowRay() == true) && (light_source_ray.IsPhotonRay() == false))
			ComputeFog(light_source_ray, isect, tmpCol);

		// discard the transmissive component (alpha channel)
		resultcolour = RGBColour(tmpCol);
	}

	// If ray is entering from the atmosphere or the ray is currently *not* inside an object add it,
	// but it it is currently inside an object, the ray is leaving the current object and is removed
	if((isect.Object != NULL) && ((light_source_ray.GetInteriors().empty()) || (light_source_ray.RemoveInterior(isect.Object->interior) == false)))
		light_source_ray.AppendInterior(isect.Object->interior);
}



void Trace::ComputeRainbow(const Ray& ray, const Intersection& isect, Colour& colour)
{
	int n;
	double dot1, k, ki, index, x, y, l, angle, fade, f;
	Vector3d Temp;
	Colour Cr, Ct;

	// Why are we here.
	if(sceneData->rainbow == NULL)
		return;

	Ct = Colour(0.0, 0.0, 0.0, 1.0, 1.0);

	n = 0;

	for(RAINBOW *Rainbow = sceneData->rainbow; Rainbow != NULL; Rainbow = Rainbow->Next)
	{
		if((Rainbow->Pigment != NULL) && (Rainbow->Distance != 0.0) && (Rainbow->Width != 0.0))
		{
			// Get angle between ray direction and rainbow's up vector.
			x = dot(Vector3d(ray.Direction), Vector3d(Rainbow->Right_Vector));
			y = dot(Vector3d(ray.Direction), Vector3d(Rainbow->Up_Vector));

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
				dot1 = dot(Vector3d(ray.Direction), Vector3d(Rainbow->Antisolar_Vector));

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
						Compute_Pigment(Cr, Rainbow->Pigment, *Temp, &isect, &ray, threadData);

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
						k = max(k, fade * (1.0 - Cr.transm()) + Cr.transm());

						// Now interpolate the colours.
						ki = 1.0 - k;

						// Attenuate filter value.
						f = Cr.filter() * ki;

						Ct.red()    += k * colour.red()   * ((1.0 - f) + f * Cr.red())   + ki * Cr.red();
						Ct.green()  += k * colour.green() * ((1.0 - f) + f * Cr.green()) + ki * Cr.green();
						Ct.blue()   += k * colour.blue()  * ((1.0 - f) + f * Cr.blue())  + ki * Cr.blue();
						Ct.filter() *= k * Cr.filter();
						Ct.transm() *= k * Cr.transm();

						n++;
					}
				}
			}
		}
	}

	if(n > 0)
	{
		COLC tmp = 1.0 / n;

		colour.red()   = Ct.red()   * tmp;
		colour.green() = Ct.green() * tmp;
		colour.blue()  = Ct.blue()  * tmp;

		colour.filter() *= Ct.filter();
		colour.transm() *= Ct.transm();
	}
}

bool Trace::TestShadow(const LightSource &lightsource, double& depth, Ray& light_source_ray, const Vector3d& p, RGBColour& colour, TraceTicket& ticket)
{
	ComputeOneLightRay(lightsource, depth, light_source_ray, p, colour);

	// There's no need to test for shadows if no light
	// is coming from the light source.
	//
	// Test for PURE zero, because we only want to skip this if we're out
	// of the range of a spot light or cylinder light.  Very dim lights
	// should not be ignored.

	if((fabs(colour.red()) < EPSILON) && (fabs(colour.green()) < EPSILON) && (fabs(colour.blue()) < EPSILON))
	{
		colour.clear();
		return true;
	}

	// Test for shadows.
	if((qualityFlags & Q_SHADOW) && ((lightsource.Projected_Through_Object != NULL) || (lightsource.Light_Type != FILL_LIGHT_SOURCE)))
	{
		TraceShadowRay(lightsource, depth, light_source_ray, p, colour, ticket);

		if((fabs(colour.red()) < EPSILON) && (fabs(colour.green()) < EPSILON) && (fabs(colour.blue()) < EPSILON))
		{
			colour.clear();
			return true;
		}
	}

	return false;
}

bool Trace::IsObjectInCSG(const ObjectBase* object, const ObjectBase* parent)
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
#elif 0
	/*
	double x = fabs(acos(cos(phi))) / M_PI_2;

	double Fr = 0.014567225 / Sqr(x - 1.12) - 0.011612903;
	return 1.0 - Fr;
	*/
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
	Ray_Intersection.Object->Normal(*Raw_Normal, &Ray_Intersection, threadData);
	Assign_Vector(Ray_Intersection.INormal, *Raw_Normal);
	Assign_Vector(Ray_Intersection.PNormal, *Raw_Normal); // TODO FIXME - we should possibly take normal pertubation into account
}

bool Trace::IsSameSSLTObject(const ObjectBase* obj1, const ObjectBase* obj2)
{
	// TODO maybe use something smarter
	return (obj1 && obj2 && obj1->interior == obj2->interior);
}

void Trace::ComputeDiffuseSampleBase(Vector3d& basePoint, const Intersection& out, const Vector3d& vOut, double avgFreeDist)
{
	Vector3d pOut(out.IPoint);
	Vector3d nOut(out.INormal);

	// make sure to get the normal right; obviously, the observer must be "outside".
	// for algorithm simplicity, we want the normal to point inward
	double cos_phi = dot(nOut, vOut);
	if (cos_phi > 0)
		nOut = -nOut;

	// typically, place the base point the average free distance below the surface;
	// however, never place it closer to the "back side" than to the front
	Intersection backSide;
	Ray ray(*pOut, *nOut); // we're shooting from the surface, so SubsurfaceRay would do us no good (as it would potentially "re-discover" the current surface)
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

	Ray ray(*basePoint, *v, Ray::SubsurfaceRay);
	bool found = FindIntersection(in, ray);

	if (found)
	{
		ComputeSSLTNormal(in);

		Vector3d vDelta = Vector3d(in.IPoint) - basePoint;
		double dist = vDelta.length();
		double cos_phi = std::abs(dot(vDelta / dist, Vector3d(in.INormal)));
		if (cos_phi < 0)
		{
			VScaleEq(in.INormal, -1.0);
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
                                                   RGBColour& Lo, double eta, const Vector3d& bend_point, double phi_out, double cos_out_prime, TraceTicket& ticket)
{
	// TODO FIXME - part of this code is very alike to ComputeOneDiffuseLight()

	// Do Light source to get the correct lightsourceray
	// (note that for now we're mainly interested in the direction)
	Ray lightsourceray(Ray::SubsurfaceRay);
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
	RGBColour lightcolour;
	ComputeOneLightRay(lightsource, lightsourcedepth, lightsourceray, Vector3d(xi.IPoint), lightcolour, true);

	// Don't calculate spotlights when outside of the light's cone.
	if((fabs(lightcolour.red())   < EPSILON) &&
	   (fabs(lightcolour.green()) < EPSILON) &&
	   (fabs(lightcolour.blue())  < EPSILON))
		return;

	// See if light on far side of surface from camera.
	// [CLi] double_illuminate and diffuse backside illumination don't seem to make much sense with BSSRDF, so we ignore them here.
	// [CLi] BSSRDF always does "full area lighting", so we ignore it here.
	double cos_in = dot(Vector3d(xi.INormal), Vector3d(lightsourceray.Direction));
	// [CLi] we're coming from inside the object, so the surface /must/ be properly oriented towards the camera; if it isn't,
	// it must be the normal's fault
	if (cos_in < 0)
	{
		VScaleEq(xi.INormal, -1.0);
		cos_in = -cos_in;
	}
	// [CLi] light coming in almost parallel to the surface is a problem though
	if(cos_in < EPSILON)
		return;

	// If light source was not blocked by any intervening object, then
	// calculate it's contribution to the object's overall illumination.
	if ((qualityFlags & Q_SHADOW) && ((lightsource.Projected_Through_Object != NULL) || (lightsource.Light_Type != FILL_LIGHT_SOURCE)))
	{
		// [CLi] Not using lightColorCache because it's unsuited for BSSRDF
		TraceShadowRay(lightsource, lightsourcedepth, lightsourceray, Vector3d(xi.IPoint), lightcolour, ticket);
	}

	// Don't calculate anything more if we're in full shadow
	if((fabs(lightcolour.red())   < EPSILON) &&
	   (fabs(lightcolour.green()) < EPSILON) &&
	   (fabs(lightcolour.blue())  < EPSILON))
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

	//RGBColour lightColour = RGBColour(lightsource.colour);
	lightcolour *= cos_in; // TODO VERIFY - is this right? Where does this term come from??

	// compute si
	double si = (bend_point - Vector3d(xi.IPoint)).length() * sceneData->mmPerUnit;

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
	assert ((factor >= 0.0) && (factor <= DBL_MAX)); // verify factor is a non-negative, finite value (no #INF, no #IND, no #NAN)

	lightcolour *= factor;

	assert ((lightcolour.red() >= 0) &&
	        (lightcolour.green() >= 0) &&
	        (lightcolour.blue() >= 0));

	// add up the contribution
	Lo += lightcolour;
}

// call this once for each color
// out.INormal is calculated
void Trace::ComputeSingleScatteringContribution(const Intersection& out, double dist, double cos_out, const Vector3d& refractedREye, double sigma_prime_t, double sigma_prime_s, RGBColour& Lo, double eta,
                                                TraceTicket& ticket)
{
	double          g = 0; // the mean cosine of the scattering angle; for isotropic scattering, g = 0
	double          sigma_t_xo = sigma_prime_t / (1-g); // TODO FIXME - precompute
	double          sigma_s = sigma_prime_s / (1-g); // TODO FIXME - precompute
	double          epsilon;
	double          s_prime_out;

	Lo.clear();

	// TODO FIXME - a significant deal of this only needs to be computed once for any intersection point!

	// calculate s_prime_out
	while (ssltUniformNumberGenerator.size() <= ticket.subsurfaceRecursionDepth)
		ssltUniformNumberGenerator.push_back(GetRandomDoubleGenerator(0.0, 1.0, 32767));
	epsilon = (*(ssltUniformNumberGenerator[ticket.subsurfaceRecursionDepth]))(); // epsilon is a random floating point value in the range [0,1) {including 0, not including 1}
	s_prime_out = fabs(log(epsilon)) / sigma_t_xo;

	if (s_prime_out >= dist)
		return; // not within the object - this will be covered by a "zero scattering" term

	//compute bend_point wihch is s_prime_out distance away on refractedREye
	Vector3d bend_point = Vector3d(out.IPoint) + refractedREye * (s_prime_out / sceneData->mmPerUnit);

	double cos_out_prime = sqrt(1 - ((Sqr(1.0 / eta)) * (1 - Sqr(cos_out))));

	// global light sources, if not turned off for this object
	if((out.Object->Flags & NO_GLOBAL_LIGHTS_FLAG) != NO_GLOBAL_LIGHTS_FLAG)
	{
		for(int i = 0; i < threadData->lightSources.size(); i++)
			ComputeOneSingleScatteringContribution(*threadData->lightSources[i], out, sigma_t_xo, sigma_s, s_prime_out, Lo, eta, bend_point, acos(cos_out), cos_out_prime, ticket);
	}

	// local light sources from a light group, if any
	if(!out.Object->LLights.empty())
	{
		for(int i = 0; i < out.Object->LLights.size(); i++)
			ComputeOneSingleScatteringContribution(*out.Object->LLights[i], out, sigma_t_xo, sigma_s, s_prime_out, Lo, eta, bend_point, acos(cos_out), cos_out_prime, ticket);
	}

	assert ((Lo.red()   >= 0) &&
	        (Lo.green() >= 0) &&
	        (Lo.blue()  >= 0));

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
		unitN = -unitN;
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

	double  cos_phi_in  = clip(dot(vIn,  nIn),                      -1.0, 1.0); // (clip values to not run into trouble due to petty precision issues)
	double  cos_phi_out = clip(dot(vOut, Vector3d(out.INormal)),    -1.0, 1.0);
	double  phi_in  = acos(cos_phi_in);
	double  phi_out = acos(cos_phi_out);

	double  F = ComputeFt(phi_in, eta) * ComputeFt(phi_out, eta);

#if 1
	// full BSSRDF model

	double  distSqr = (pIn - Vector3d(out.IPoint)).lengthSqr() * Sqr(sceneData->mmPerUnit);

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
	assert ((sd >= 0.0) && (sd <= DBL_MAX)); // verify sd is a non-negative, finite value (no #INF, no #IND, no #NAN)
}

void Trace::ComputeDiffuseContribution1(const LightSource& lightsource, const Intersection& out, const Vector3d& vOut, const Intersection& in, RGBColour& Total_Colour,
                                        const DblRGBColour& sigma_prime_s, const DblRGBColour& sigma_a, double eta, double weight, TraceTicket& ticket)
{
	// TODO FIXME - part of this code is very alike to ComputeOneDiffuseLight()

	// Get a colour and a ray.
	Ray lightsourceray;
	double lightsourcedepth;
	RGBColour lightcolour;
	ComputeOneLightRay(lightsource, lightsourcedepth, lightsourceray, Vector3d(in.IPoint), lightcolour, true);

	// Don't calculate spotlights when outside of the light's cone.
	if((fabs(lightcolour.red())   < EPSILON) &&
	   (fabs(lightcolour.green()) < EPSILON) &&
	   (fabs(lightcolour.blue())  < EPSILON))
		return;

	Vector3d nIn = Vector3d(in.INormal);

	// See if light on far side of surface from camera.
	// [CLi] double_illuminate and diffuse backside illumination don't seem to make much sense with BSSRDF, so we ignore them here.
	// [CLi] BSSRDF always does "full area lighting", so we ignore it here.
	double cos_in = dot(nIn, Vector3d(lightsourceray.Direction));
	// [CLi] we're coming from inside the object, so the surface /must/ be properly oriented towards the camera; if it isn't,
	// it must be the normal's fault
	if (cos_in < 0)
	{
		nIn    = -nIn;
		cos_in = -cos_in;
	}
	// [CLi] light coming in almost parallel to the surface is a problem though
	if(cos_in < EPSILON)
		return;

	// If light source was not blocked by any intervening object, then
	// calculate it's contribution to the object's overall illumination.
	if ((qualityFlags & Q_SHADOW) && ((lightsource.Projected_Through_Object != NULL) || (lightsource.Light_Type != FILL_LIGHT_SOURCE)))
	{
		// [CLi] Not using lightColorCache because it's unsuited for BSSRDF
		TraceShadowRay(lightsource, lightsourcedepth, lightsourceray, Vector3d(in.IPoint), lightcolour, ticket);
	}

	// Don't calculate anything more if we're in full shadow
	if((fabs(lightcolour.red())   < EPSILON) &&
	   (fabs(lightcolour.green()) < EPSILON) &&
	   (fabs(lightcolour.blue())  < EPSILON))
		return;

	lightcolour *= cos_in;
	for (int j = 0; j < 3; j++)
	{
		double sd;
		ComputeDiffuseContribution(out, vOut, Vector3d(in.IPoint), nIn, Vector3d(lightsourceray.Direction), sd, sigma_prime_s[j], sigma_a[j], eta);
		sd *= weight;
		assert (sd >= 0);
		lightcolour[j] *= sd;
		assert (lightcolour[j] >= 0);
		Total_Colour[j] += lightcolour[j];
	}
}

void Trace::ComputeDiffuseAmbientContribution1(const Intersection& out, const Vector3d& vOut, const Intersection& in, RGBColour& Total_Colour,
                                               const DblRGBColour& sigma_prime_s, const DblRGBColour& sigma_a, double eta, double weight, TraceTicket& ticket)
{
#if 0
	// generate a random direction vector (using a distribution cosine-weighted along the normal)
	Vector3d axisU, axisV;
	ComputeSurfaceTangents(Vector3d(in.INormal), axisU, axisV);
	while (ssltCosWeightedDirectionGenerator.size() <= ticket.subsurfaceRecursionDepth)
		ssltCosWeightedDirectionGenerator.push_back(GetSubRandomCosWeightedDirectionGenerator(2, 32767));
	Vector3d direction = (*(ssltCosWeightedDirectionGenerator[ticket.subsurfaceRecursionDepth]))();
	double cos_in = direction.y(); // cosine of angle between normal and random vector
	Vector3d vIn = Vector3d(in.INormal)*cos_in + axisU*direction.x() + axisV*direction.z();

	assert(fabs(dot(Vector3d(in.INormal), axisU)) < EPSILON);
	assert(fabs(dot(Vector3d(in.INormal), axisV)) < EPSILON);
	assert(fabs(dot(axisU, axisV)) < EPSILON);

	// [CLi] light coming in almost parallel to the surface is a problem
	if(cos_in < EPSILON)
		return;

	Ray ambientray = Ray(in.IPoint, *vIn, Ray::OtherRay); // TODO FIXME - [CLi] check whether ray type is suitable
	Colour ambientcolour;
	TraceRay(ambientray, ambientcolour, weight, ticket, false);

	// Don't calculate anything more if there's no light input
	if((fabs(ambientcolour.red())   < EPSILON) &&
	   (fabs(ambientcolour.green()) < EPSILON) &&
	   (fabs(ambientcolour.blue())  < EPSILON))
		return;

	for (int j = 0; j < 3; j++)
	{
		double sd;
		// Note: radiosity data is already cosine-weighted, so we're passing the surface normal as incident light direction
		ComputeDiffuseContribution(out, vOut, Vector3d(in.IPoint), Vector3d(in.INormal),  vIn, sd, sigma_prime_s[j], sigma_a[j], eta);
		sd *= 0.5/cos_in; // the distribution is cosine-weighted, but sd was computed assuming neutral weighting, so compensate
		sd *= weight;
		assert (sd >= 0);
		ambientcolour[j] *= sd;
		assert (ambientcolour[j] >= 0);
		Total_Colour[j] += ambientcolour[j];
	}
#else
	RGBColour ambientcolour;
	// TODO FIXME - should support pertubed normals
	radiosity.ComputeAmbient(Vector3d(in.IPoint), Vector3d(in.INormal), Vector3d(in.INormal), ambientcolour, weight, ticket);
	for (int j = 0; j < 3; j++)
	{
		double sd;
		// Note: radiosity data is already cosine-weighted, so we're passing the surface normal as incident light direction
		ComputeDiffuseContribution(out, vOut, Vector3d(in.IPoint), Vector3d(in.INormal), Vector3d(in.INormal), sd, sigma_prime_s[j], sigma_a[j], eta);
		sd *= weight;
		assert (sd >= 0);
		ambientcolour[j] *= sd;
		assert (ambientcolour[j] >= 0);
		Total_Colour[j] += ambientcolour[j];
	}
#endif
}

void Trace::ComputeSubsurfaceScattering(const FINISH *Finish, const RGBColour& layer_pigment_colour, const Intersection& out, const Ray& Eye, const Vector3d& Layer_Normal, RGBColour& Final_Colour, double Attenuation, TraceTicket& ticket)
{
	int NumSamplesDiffuse = sceneData->subsurfaceSamplesDiffuse;
	int NumSamplesSingle  = sceneData->subsurfaceSamplesSingle;

	// TODO FIXME - this is hard-coded for now
	if (ticket.subsurfaceRecursionDepth >= 2)
		return;
	else if (ticket.subsurfaceRecursionDepth == 1)
	{
		NumSamplesDiffuse = 1;
		NumSamplesSingle  = 1;
		//NumSamplesDiffuse = (int)ceil(sqrt(NumSamplesDiffuse));
		//NumSamplesSingle  = (int)ceil(sqrt(NumSamplesSingle));
	}

	ticket.subsurfaceRecursionDepth++;

	LightSource Light_Source;

	Vector3d vOut = -Vector3d(Eye.Direction);

	RGBColour Total_Colour;

	double eta;

	ComputeRelativeIOR(Eye, out.Object->interior, eta);

#if 0
	// user setting specifies mean free path
	DblRGBColour   alpha_prime      = object->interior->subsurface->GetReducedAlbedo(layer_pigment_colour * Finish->Diffuse);
	DblRGBColour   sigma_tr         = DblRGBColour(1.0) / DblRGBColour(Finish->SubsurfaceTranslucency);

	DblRGBColour   sigma_prime_t    = sigma_tr / sqrt(3*(RGBColour(1.0)-alpha_prime));
	DblRGBColour   sigma_prime_s    = alpha_prime * sigma_prime_t;
	DblRGBColour   sigma_a          = sigma_prime_t - sigma_prime_s;
	DblRGBColour   sigma_tr_sqr     = sigma_tr * sigma_tr;
#else
	// user setting specifies reduced scattering coefficient
	DblRGBColour   alpha_prime      = out.Object->interior->subsurface->GetReducedAlbedo(layer_pigment_colour * Finish->RawDiffuse);
	DblRGBColour   sigma_prime_s    = DblRGBColour(1.0) / DblRGBColour(Finish->SubsurfaceTranslucency);

	DblRGBColour   sigma_prime_t    = sigma_prime_s / alpha_prime;
	DblRGBColour   sigma_a          = sigma_prime_t - sigma_prime_s;
	DblRGBColour   sigma_tr_sqr     = sigma_a * sigma_prime_t * 3.0;
	DblRGBColour   sigma_tr         = sqrt(sigma_tr_sqr);
#endif

#if 1

	// colour dependent diffuse contribution

	double      sampleArea;
	double      weight;
	double      weightSum;
	double      sigma_a_mean        = sigma_a.greyscale();
	double      sigma_prime_s_mean  = sigma_prime_s.greyscale();
	double      sigma_prime_t_mean  = sigma_a_mean + sigma_prime_s_mean;
	double      sigma_tr_mean_sqr   = sigma_a_mean * sigma_prime_t_mean * 3.0;
	double      sigma_tr_mean       = sqrt(sigma_tr_mean_sqr);
	int         trueNumSamples;

	bool radiosity_needed = (sceneData->radiositySettings.radiosityEnabled == true) &&
	                        (sceneData->subsurfaceUseRadiosity == true) &&
	                        (radiosity.CheckRadiosityTraceLevel(ticket) == true) &&
	                        (Test_Flag(out.Object, IGNORE_RADIOSITY_FLAG) == false);

	Vector3d sampleBase;
	ComputeDiffuseSampleBase(sampleBase, out, vOut, 1.0 / (sigma_prime_t_mean * sceneData->mmPerUnit));

	weightSum = 0.0;
	trueNumSamples = 0;

	for (int i = 0; i < NumSamplesDiffuse; i++)
	{
		Intersection in;
		ComputeDiffuseSamplePoint(sampleBase, in, sampleArea, ticket);

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
					ComputeDiffuseAmbientContribution1(out, vOut, in, Total_Colour, sigma_prime_s, sigma_a, eta, weight, ticket);

				// global light sources, if not turned off for this object
				if((out.Object->Flags & NO_GLOBAL_LIGHTS_FLAG) != NO_GLOBAL_LIGHTS_FLAG)
				{
					for(int k = 0; k < threadData->lightSources.size(); k++)
						ComputeDiffuseContribution1(*threadData->lightSources[k], out, vOut, in, Total_Colour, sigma_prime_s, sigma_a, eta, weight, ticket);
				}

				// local light sources from a light group, if any
				if(!out.Object->LLights.empty())
				{
					for(int k = 0; k < out.Object->LLights.size(); k++)
						ComputeDiffuseContribution1(*out.Object->LLights[k], out, vOut, in, Total_Colour, sigma_prime_s, sigma_a, eta, weight, ticket);
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
	if (SSLTComputeRefractedDirection(Vector3d(Eye.Direction), Vector3d(out.INormal), 1.0/eta, refractedEye))
	{
		Ray refractedEyeRay(out.IPoint, *refractedEye);
		Intersection unscatteredIn;

		double dist;

		// find the intersection of the refracted ray with the object
		// find the distance to this intersection
		bool found = FindIntersection(unscatteredIn, refractedEyeRay);
		if (found)
			dist = (Vector3d(out.IPoint) - Vector3d(unscatteredIn.IPoint)).length() * sceneData->mmPerUnit;
		else
			dist = HUGE_VAL;

		double cos_out = dot(vOut, Vector3d(out.INormal));

#if 1

		// colour dependent single scattering contribution

		for (int i = 0; i < NumSamplesSingle; i++)
		{
			for (int j = 0; j < 3; j ++)
			{
				RGBColour temp;
				ComputeSingleScatteringContribution(out, dist, cos_out, refractedEye, sigma_prime_t[j], sigma_prime_s[j], temp, eta, ticket);
				Total_Colour[j] += temp[j] / NumSamplesSingle;
			}
		}

#endif

#if 1

		// colour dependent unscattered contribution

		// Trace refracted ray.
		Colour tempcolor;

		// TODO FIXME - account for fresnel attenuation at interfaces
		DblRGBColour att = exp(-sigma_prime_t * dist); // TODO should be sigma_t
		weight = max3(att.red(), att.green(), att.blue());
		if (weight > ticket.adcBailout)
		{
			if (!found)
			{
				// TODO - trace the ray to the background?
			}
			else if (IsSameSSLTObject(unscatteredIn.Object, out.Object))
			{
				unscatteredIn.Object->Normal(unscatteredIn.INormal, &unscatteredIn, threadData);
				if (dot(refractedEye, Vector3d(unscatteredIn.INormal)) > 0)
					VScaleEq(unscatteredIn.INormal, -1.0);
				Vector3d doubleRefractedEye;
				if (SSLTComputeRefractedDirection(refractedEye, Vector3d(unscatteredIn.INormal), eta, doubleRefractedEye))
				{
					Ray doubleRefractedEyeRay(refractedEyeRay);
					doubleRefractedEyeRay.SetFlags(Ray::RefractionRay, refractedEyeRay);
					Assign_Vector(doubleRefractedEyeRay.Origin, unscatteredIn.IPoint);
					Assign_Vector(doubleRefractedEyeRay.Direction, *(doubleRefractedEye));
					TraceRay(doubleRefractedEyeRay, tempcolor, weight, ticket, false);
					Total_Colour += RGBColour(DblRGBColour(RGBColour(tempcolor)) * att);
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

	ticket.subsurfaceRecursionDepth--;
}

} // end of namespace
