//******************************************************************************
///
/// @file core/scene/tracethreaddata.cpp
///
/// @todo   What's in here?
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
#include "core/scene/tracethreaddata.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <limits>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/material/noise.h"
#include "core/scene/scenedata.h"
#include "core/shape/blob.h"
#include "core/shape/fractal.h"
#include "core/support/cracklecache.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

TraceThreadData::TraceThreadData(std::shared_ptr<SceneData> sd, size_t seed) :
    sceneData(sd),
    qualityFlags(9),
    stochasticRandomGenerator(GetRandomDoubleGenerator(0.0,1.0)),
    stochasticRandomSeedBase(seed),
    mpCrackleCache(new CrackleCache),
    mpRenderStats(new RenderStatistics)
{
    for(int i = 0; i < 4; i++)
        Fractal_IStack[i] = nullptr;
    Fractal::Allocate_Iteration_Stack(Fractal_IStack, sceneData->Fractal_Iteration_Stack_Length);
    Max_Blob_Queue_Size = 1;
    Blob_Coefficient_Count = sceneData->Max_Blob_Components * 5;
    Blob_Interval_Count = sceneData->Max_Blob_Components * 2;
    Blob_Queue = reinterpret_cast<void **>(POV_MALLOC(sizeof(void *), "Blob Queue"));
    Blob_Coefficients = reinterpret_cast<DBL *>(POV_MALLOC(sizeof(DBL) * Blob_Coefficient_Count, "Blob Coefficients"));
    Blob_Intervals = new Blob_Interval_Struct [Blob_Interval_Count];

    BCyl_Intervals.reserve(4*sceneData->Max_Bounding_Cylinders);
    BCyl_RInt.reserve(2*sceneData->Max_Bounding_Cylinders);
    BCyl_HInt.reserve(2*sceneData->Max_Bounding_Cylinders);

    Facets_Last_Seed = 0x80000000;

    timeType = kUnknownTime;
    cpuTime = 0;
    realTime = 0;

    stochasticRandomGenerator->Seed(stochasticRandomSeedBase);

    for(std::vector<LightSource *>::iterator it = sceneData->lightSources.begin(); it != sceneData->lightSources.end(); it++)
        lightSources.push_back(static_cast<LightSource *> (Copy_Object(*it)));

    // all of these are for photons
    LightSource *photonLight = nullptr;
    ObjectPtr photonObject = nullptr;
    litObjectIgnoresPhotons = false;
    hitObject = false;    // did we hit the target object? (for autostop)
    photonSpread = 0.0; // photon spread (in radians)
    photonDepth = 0.0;  // total distance from light to intersection
    passThruThis = false;           // is this a pass-through object?
    passThruPrev = false;           // was the previous object pass-through?
    Light_Is_Global = false;       // is the current light global? (not part of a light_group?)

    progress_index = 0;

    surfacePhotonMap = new PhotonMap();
    mediaPhotonMap = new PhotonMap();

    numberOfWaves = sd->numberOfWaves;
    Initialize_Waves(waveFrequencies, waveSources, numberOfWaves);
}

TraceThreadData::~TraceThreadData()
{
    for(std::vector<GenericFunctionContext*>::iterator i = functionContextPool.begin(); i != functionContextPool.end(); ++i)
        delete *i;

    POV_FREE(Blob_Coefficients);
    POV_FREE(Blob_Queue);
    Fractal::Free_Iteration_Stack(Fractal_IStack);
    delete surfacePhotonMap;
    delete mediaPhotonMap;
    delete[] Blob_Intervals;
    for(std::vector<LightSource *>::iterator it = lightSources.begin(); it != lightSources.end(); it++)
        Destroy_Object(*it);
    delete mpCrackleCache;
    delete mpRenderStats;
}

void TraceThreadData::AfterTile()
{
    mpCrackleCache->Prune();
}

}
// end of namespace pov
