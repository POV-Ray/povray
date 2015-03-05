//******************************************************************************
///
/// @file backend/scene/threaddata.cpp
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#include <limits>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/scene/threaddata.h"

#include "core/material/texture.h"
#include "core/shape/blob.h"
#include "core/shape/fractal.h"
#include "core/shape/isosurface.h"

#include "backend/bounding/bcyl.h"
#include "backend/scene/objects.h"
#include "backend/scene/scene.h"
#include "backend/scene/view.h"
#include "backend/support/statistics.h"
#include "backend/vm/fnpovfpu.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

SceneThreadData::SceneThreadData(shared_ptr<SceneData> sd): sceneData(sd), qualityFlags(9)
{
    for(int i = 0; i < 4; i++)
        Fractal_IStack[i] = NULL;
    Fractal::Allocate_Iteration_Stack(Fractal_IStack, sceneData->Fractal_Iteration_Stack_Length);
    Max_Blob_Queue_Size = 1;
    Blob_Coefficient_Count = sceneData->Max_Blob_Components * 5;
    Blob_Interval_Count = sceneData->Max_Blob_Components * 2;
    Blob_Queue = reinterpret_cast<void **>(POV_MALLOC(sizeof(void **), "Blob Queue"));
    Blob_Coefficients = reinterpret_cast<DBL *>(POV_MALLOC(sizeof(DBL) * Blob_Coefficient_Count, "Blob Coefficients"));
    Blob_Intervals = new Blob_Interval_Struct [Blob_Interval_Count];
    isosurfaceData = reinterpret_cast<ISO_ThreadData *>(POV_MALLOC(sizeof(ISO_ThreadData), "Isosurface Data"));
    isosurfaceData->ctx = NULL;
    isosurfaceData->current = NULL;
    isosurfaceData->cache = false;
    isosurfaceData->Inv3 = 1;
    isosurfaceData->fmax = 0.0;
    isosurfaceData->tl = 0.0;
    isosurfaceData->Vlength = 0.0;

    functionContext = sceneData->functionVM->NewContext(this);
    functionPatternContext.resize(sceneData->functionPatternCount);

    BCyl_Intervals.reserve(4*sceneData->Max_Bounding_Cylinders);
    BCyl_RInt.reserve(2*sceneData->Max_Bounding_Cylinders);
    BCyl_HInt.reserve(2*sceneData->Max_Bounding_Cylinders);

    Facets_Last_Seed = 0x80000000;

    timeType = kUnknownTime;
    cpuTime = 0;
    realTime = 0;

    for(vector<LightSource *>::iterator it = sceneData->lightSources.begin(); it != sceneData->lightSources.end(); it++)
        lightSources.push_back(static_cast<LightSource *> (Copy_Object(*it)));

    // all of these are for photons
    LightSource *photonLight = NULL;
    ObjectPtr photonObject = NULL;
    litObjectIgnoresPhotons = false;
    hitObject = false;    // did we hit the target object? (for autostop)
    photonSpread = 0.0; // photon spread (in radians)
    photonDepth = 0.0;  // total distance from light to intersection
    passThruThis = false;           // is this a pass-through object?
    passThruPrev = false;           // was the previous object pass-through?
    Light_Is_Global = false;       // is the current light global? (not part of a light_group?)

    CrCache_MaxAge = 1;
    progress_index = 0;

    surfacePhotonMap = new PhotonMap();
    mediaPhotonMap = new PhotonMap();

    // advise the crackle cache's unordered_map that we don't mind hash collisions
    // while this is a very high load factor, the simple fact is that the cost of
    // allocating memory at render time (each insert into the table requires an alloc
    // as the container doesn't pre-emptively allocate, unlike e.g. std::vector) is
    // quite high, particularly when we have multiple threads contending for the heap
    // lock.
    mCrackleCache.max_load_factor(50.0);

    numberOfWaves = sd->numberOfWaves;
    Initialize_Waves(waveFrequencies, waveSources, numberOfWaves);
}

SceneThreadData::~SceneThreadData()
{
    sceneData->functionVM->DeleteContext(functionContext);

    POV_FREE(Blob_Coefficients);
    POV_FREE(Blob_Queue);
    POV_FREE(isosurfaceData);
    Fractal::Free_Iteration_Stack(Fractal_IStack);
    delete surfacePhotonMap;
    delete mediaPhotonMap;
    delete[] Blob_Intervals;
    for(vector<LightSource *>::iterator it = lightSources.begin(); it != lightSources.end(); it++)
        Destroy_Object(*it);
}

void SceneThreadData::AfterTile()
{
    CrackleCache::iterator it;

    // this serves as a render block index
    progress_index++;

    // probably we ought to have a means for the end-user to choose the preferred maximum bytes reserved for the cache
    // for now, we have hard-coded values. we also do not discard any entries that are from the current block, even if
    // the cache size is exceeded. also, note that the cache size is per-thread. finally, don't forget that erasing
    // elements doesn't in and of itself return the freed memory to the heap.
    if (mCrackleCache.size() * sizeof(CrackleCache::value_type) < 15 * 1024 * 1024)
        return;
    while (mCrackleCache.size() * sizeof(CrackleCache::value_type) > 10 * 1024 * 1024)
    {
        // search the cache for the oldest entries
        int oldest = std::numeric_limits<int>::max();
        for (it = mCrackleCache.begin(); it != mCrackleCache.end(); it++)
            if (it->second.lastUsed < oldest)
                oldest = (int) it->second.lastUsed;

        // don't remove any entries from the most recent block
        if (oldest == progress_index - 1)
            break;

        for (it = mCrackleCache.begin(); it != mCrackleCache.end(); )
        {
            if (it->second.lastUsed == oldest)
            {
                it = mCrackleCache.erase(it);
                continue;
            }
            it++;
        }
    }
}

ViewThreadData::ViewThreadData(ViewData *vd) :
    SceneThreadData(vd->GetSceneData()),
    viewData(vd)
{
}

ViewThreadData::~ViewThreadData()
{
}

unsigned int ViewThreadData::GetWidth() const
{
    return viewData->GetWidth();
}

unsigned int ViewThreadData::GetHeight() const
{
    return viewData->GetHeight();
}

const POVRect& ViewThreadData::GetRenderArea()
{
    return viewData->GetRenderArea();
}

}
