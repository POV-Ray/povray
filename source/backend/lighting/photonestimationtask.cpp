//******************************************************************************
///
/// @file backend/lighting/photonestimationtask.cpp
///
/// This module implements Photon Mapping.
///
/// Author: Nathan Kopp
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
#include "backend/lighting/photonestimationtask.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/bounding/boundingbox.h"
#include "core/lighting/lightgroup.h"
#include "core/lighting/lightsource.h"
#include "core/math/matrix.h"
#include "core/scene/object.h"
#include "core/shape/csg.h"
#include "core/support/octree.h"

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"
#include "povms/povmsid.h"
#include "povms/povmsutil.h"

// POV-Ray header files (backend module)
#include "backend/scene/backendscenedata.h"
#include "backend/scene/view.h"
#include "backend/scene/viewthreaddata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::vector;

PhotonEstimationTask::PhotonEstimationTask(ViewData *vd, size_t seed) :
    RenderTask(vd, seed, "Photon"),
    cooperate(*this)
{
    photonCountEstimate = 0;
}

PhotonEstimationTask::~PhotonEstimationTask()
{
}


void PhotonEstimationTask::Run()
{
    // quit right away if photons not enabled
    if (!GetSceneData()->photonSettings.photonsEnabled) return;

    if (GetSceneData()->photonSettings.surfaceCount==0) return;

    Cooperate();

    //  COUNT THE PHOTONS
    DBL factor;
    photonCountEstimate = 0.0;

    // global lights
    GetViewDataPtr()->Light_Is_Global = true;
    for(vector<LightSource *>::iterator Light = GetSceneData()->lightSources.begin(); Light != GetSceneData()->lightSources.end(); Light++)
    {
        if((*Light)->Light_Type != FILL_LIGHT_SOURCE)
            SearchThroughObjectsEstimatePhotons(GetSceneData()->objects, *Light);
    }

    // light_group lights
    /*
    TODO
    renderer->sceneData->photonSettings.Light_Is_Global = false;
    for(vector<LightSource *>::iterator Light_Group_Light = renderer->sceneData->lightGroupLights.begin(); Light_Group_Light != renderer->sceneData->lightGroupLights.end(); Light_Group_Light++)
    {
        Light = Light_Group_Light->Light;
        if (Light->Light_Type != FILL_LightSource)
        {
            SearchThroughObjectsEstimatePhotons(GetSceneData()->objects, *Light);
        }
    }
    */

    factor = (DBL)photonCountEstimate/GetSceneData()->photonSettings.surfaceCount;
    factor = sqrt(factor);
    GetSceneData()->photonSettings.surfaceSeparation *= factor;


    // good idea to make sure all warnings and errors arrive frontend now [trf]
    Cooperate();
}

void PhotonEstimationTask::Stopped()
{
    // nothing to do for now [trf]
}

void PhotonEstimationTask::Finish()
{
    GetViewDataPtr()->timeType = TraceThreadData::kPhotonTime;
    GetViewDataPtr()->realTime = ConsumedRealTime();
    GetViewDataPtr()->cpuTime = ConsumedCPUTime();
}


void PhotonEstimationTask::SearchThroughObjectsEstimatePhotons(vector<ObjectPtr>& Objects, LightSource *Light)
{
    ViewThreadData *renderDataPtr = GetViewDataPtr();
    std::shared_ptr<SceneData> sceneData = GetSceneData();

    /* check this object and all siblings */
    for(vector<ObjectPtr>::iterator Sib = Objects.begin(); Sib != Objects.end(); Sib++)
    {
        if(Test_Flag((*Sib), PH_TARGET_FLAG) &&
            !((*Sib)->Type & LIGHT_SOURCE_OBJECT))
        {
            /* do not shoot photons if global lights are turned off for ObjectPtr */
            if(!Test_Flag((*Sib), NO_GLOBAL_LIGHTS_FLAG))
            {
                EstimatePhotonsForObjectAndLight((*Sib), Light);
            }

            Cooperate();
        }
        /* if it has children, check them too */
        else if(((*Sib)->Type & IS_COMPOUND_OBJECT))
        {
            SearchThroughObjectsEstimatePhotons((reinterpret_cast<CSG *>(*Sib))->children, Light);
        }
    }
}

void PhotonEstimationTask::EstimatePhotonsForObjectAndLight(ObjectPtr Object, LightSource *Light)
{
    int mergedFlags=0;             /* merged flags to see if we should shoot photons */
    ViewThreadData *renderDataPtr = GetViewDataPtr();

    /* first, check on various flags... make sure all is a go for this ObjectPtr */
    LightTargetCombo combo(Light,Object);
    mergedFlags = combo.computeMergedFlags();

    if (!( ((mergedFlags & PH_RFR_ON_FLAG) && !(mergedFlags & PH_RFR_OFF_FLAG)) ||
           ((mergedFlags & PH_RFL_ON_FLAG) && !(mergedFlags & PH_RFL_OFF_FLAG)) ))
        /* it is a no-go for this object... bail out now */
        return;

    if(!Object) return;

    ShootingDirection shootingDirection(Light,Object);
    shootingDirection.compute();

    /* calculate the spacial separation (spread) */
    renderDataPtr->photonSpread = combo.target->Ph_Density*GetSceneData()->photonSettings.surfaceSeparation;

    /* if rays aren't parallel, divide by dist so we get separation at a distance of 1 unit */
    if (!combo.light->Parallel)
    {
        renderDataPtr->photonSpread /= shootingDirection.dist;
    }

    /* try to guess the number of photons */
    DBL x=shootingDirection.rad / (combo.target->Ph_Density*GetSceneData()->photonSettings.surfaceSeparation);
    x=x*x*M_PI;

    if ( ((mergedFlags & PH_RFR_ON_FLAG) && !(mergedFlags & PH_RFR_OFF_FLAG)) &&
         ((mergedFlags & PH_RFL_ON_FLAG) && !(mergedFlags & PH_RFL_OFF_FLAG)) )
    {
        x *= 1.5;  /* assume 2 times as many photons with both reflection & refraction */
    }

    if ( !Test_Flag(combo.target, PH_IGNORE_PHOTONS_FLAG) )
    {
        if ( ((mergedFlags & PH_RFR_ON_FLAG) && !(mergedFlags & PH_RFR_OFF_FLAG)) )
        {
            if ( ((mergedFlags & PH_RFL_ON_FLAG) && !(mergedFlags & PH_RFL_OFF_FLAG)) )
                x *= 3;  /* assume 3 times as many photons if ignore_photons not used */
            else
                x *= 2;  /* assume less for only refraction */
        }
    }

    x *= 0.5;  /* assume 1/2 of photons hit target ObjectPtr */

    photonCountEstimate += x;
}

}
// end of namespace pov
