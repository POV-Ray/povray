//******************************************************************************
///
/// @file backend/lighting/photonstrategytask.cpp
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
#include "backend/lighting/photonstrategytask.h"

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
#include "backend/control/messagefactory.h"
#include "backend/lighting/photonshootingstrategy.h"
#include "backend/scene/backendscenedata.h"
#include "backend/scene/view.h"
#include "backend/scene/viewthreaddata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::vector;

PhotonStrategyTask::PhotonStrategyTask(ViewData *vd, PhotonShootingStrategy* strategy, size_t seed) :
    RenderTask(vd, seed, "Photon"),
    strategy(strategy),
    cooperate(*this)
{
    // do nothing
}

PhotonStrategyTask::~PhotonStrategyTask()
{
    //delete strategy;
}

void PhotonStrategyTask::SendProgress(void)
{
    if (timer.ElapsedRealTime() > 1000)
    {
        timer.Reset();
        POVMS_Object obj(kPOVObjectClass_PhotonProgress);
        obj.SetInt(kPOVAttrib_CurrentPhotonCount, GetSceneData()->surfacePhotonMap.numPhotons + GetSceneData()->mediaPhotonMap.numPhotons);
        RenderBackend::SendViewOutput(GetViewData()->GetViewId(), GetSceneData()->frontendAddress, kPOVMsgIdent_Progress, obj);
    }
}

void PhotonStrategyTask::Run()
{
    // quit right away if photons not enabled
    if (!GetSceneData()->photonSettings.photonsEnabled) return;

    Cooperate();

    /*  loop through global light sources  */
    GetViewDataPtr()->Light_Is_Global = true;
    for(vector<LightSource *>::iterator Light = GetSceneData()->lightSources.begin(); Light != GetSceneData()->lightSources.end(); Light++)
    {
        if ((*Light)->Light_Type != FILL_LIGHT_SOURCE)
        {
            if ((*Light)->Light_Type == CYLINDER_SOURCE && !(*Light)->Parallel)
                mpMessageFactory->Warning(kWarningGeneral,"Cylinder lights should be parallel when used with photons.");

            /* do object-specific lighting */
            SearchThroughObjectsCreateUnits(GetSceneData()->objects, (*Light));
        }

        Cooperate();
    }

    // loop through light_group light sources
/*
    TODO
    GetSceneData()->photonSettings.Light_Is_Global = false;
    for(vector<LightSource *>::iterator Light_Group_Light = GetSceneData()->lightGroupLights.begin(); Light_Group_Light != GetSceneData()->lightGroupLights.end(); Light_Group_Light++)
    {
        Light = Light_Group_Light->Light;

        if (Light->Light_Type == CYLINDER_SOURCE && !Light->Parallel)
            mpMessageFactory->Warning(kWarningGeneral,"Cylinder lights should be parallel when used with photons.");

        // do object-specific lighting
        SearchThroughObjectsCreateUnits(GetSceneData()->objects, Light);

        Cooperate();
        SendProgress();
    }
*/
    // good idea to make sure all warnings and errors arrive frontend now [trf]
    Cooperate();

    strategy->start();
}

void PhotonStrategyTask::Stopped()
{
    // nothing to do for now [trf]
}

void PhotonStrategyTask::Finish()
{
    GetViewDataPtr()->timeType = TraceThreadData::kPhotonTime;
    GetViewDataPtr()->realTime = ConsumedRealTime();
    GetViewDataPtr()->cpuTime = ConsumedCPUTime();
}


/*****************************************************************************

 FUNCTION

  SearchThroughObjectsCreateUnits()

  Searches through 'object' and all siblings  and children of 'object' to
  locate objects with PH_TARGET_FLAG set.  This flag means that the object
  receives photons.

  Preconditions:
    Photon mapping initialized (InitBacktraceEverything() called)
    'Object' is a object (with or without siblings)
    'Light' is a light source in the scene

  Postconditions:
    Work units (combination of light + target) have been added to the
    strategy.

******************************************************************************/

void PhotonStrategyTask::SearchThroughObjectsCreateUnits(vector<ObjectPtr>& Objects, LightSource *Light)
{
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
                strategy->createUnitsForCombo((*Sib), Light, sceneData);
            }

            Cooperate();
            SendProgress();
        }
        /* if it has children, check them too */
        else if(((*Sib)->Type & IS_COMPOUND_OBJECT))
        {
            SearchThroughObjectsCreateUnits((reinterpret_cast<CSG *>(*Sib))->children, Light);
        }
    }
}

}
// end of namespace pov
