//******************************************************************************
///
/// @file backend/lighting/photonsortingtask.cpp
///
/// This module implements Photon Mapping.
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
#include "backend/lighting/photonsortingtask.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

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

/*
    If you pass a nullptr for the "strategy" parameter, then this will
    load the photon map from a file.
    Otherwise, it will:
      1) merge
      2) sort
      3) compute gather options
      4) clean up memory (delete the non-merged maps and delete the strategy)
*/
PhotonSortingTask::PhotonSortingTask(ViewData *vd, const std::vector<PhotonMap*>& surfaceMaps,
                                     const std::vector<PhotonMap*>& mediaMaps, PhotonShootingStrategy* strategy,
                                     size_t seed) :
    RenderTask(vd, seed, "Photon"),
    surfaceMaps(surfaceMaps),
    mediaMaps(mediaMaps),
    strategy(strategy),
    cooperate(*this)
{
}

PhotonSortingTask::~PhotonSortingTask()
{
}

void PhotonSortingTask::SendProgress(void)
{
#if 0
    // TODO FIXME PHOTONS
    // for now, we won't send this, as it can be confusing on the front-end due to out-of-order delivery from multiple threads.
    // we need to create a new progress message for sorting.
    if (timer.ElapsedRealTime() > 1000)
    {
        timer.Reset();
        POVMS_Object obj(kPOVObjectClass_PhotonProgress);
        obj.SetInt(kPOVAttrib_CurrentPhotonCount, (GetSceneData()->surfacePhotonMap.numPhotons + GetSceneData()->mediaPhotonMap.numPhotons));
        RenderBackend::SendViewOutput(GetViewData()->GetViewId(), GetSceneData()->frontendAddress, kPOVMsgIdent_Progress, obj);
    }
#endif
}

void PhotonSortingTask::Run()
{
    // quit right away if photons not enabled
    if (!GetSceneData()->photonSettings.photonsEnabled) return;

    Cooperate();

    if (strategy != nullptr)
    {
        delete strategy;
        sortPhotonMap();
    }
    else
    {
        if (!this->load())
            mpMessageFactory->Error(POV_EXCEPTION_STRING("Failed to load photon map from disk"), "Could not load photon map (%s)",GetSceneData()->photonSettings.fileName.c_str());

        // set photon options automatically
        if (GetSceneData()->surfacePhotonMap.numPhotons>0)
            GetSceneData()->surfacePhotonMap.setGatherOptions(GetSceneData()->photonSettings,false);
        if (GetSceneData()->mediaPhotonMap.numPhotons>0)
            GetSceneData()->mediaPhotonMap.setGatherOptions(GetSceneData()->photonSettings,true);
    }

    // good idea to make sure all warnings and errors arrive frontend now [trf]
    SendProgress();
    Cooperate();
}

void PhotonSortingTask::Stopped()
{
    // nothing to do for now [trf]
}

void PhotonSortingTask::Finish()
{
    GetViewDataPtr()->timeType = TraceThreadData::kPhotonTime;
    GetViewDataPtr()->realTime = ConsumedRealTime();
    GetViewDataPtr()->cpuTime = ConsumedCPUTime();
}


void PhotonSortingTask::sortPhotonMap()
{
    std::vector<PhotonMap*>::iterator mapIter;
    for(mapIter = surfaceMaps.begin(); mapIter != surfaceMaps.end(); mapIter++)
    {
        GetSceneData()->surfacePhotonMap.mergeMap(*mapIter);
        //delete (*mapIter);
    }
    for(mapIter = mediaMaps.begin(); mapIter != mediaMaps.end(); mapIter++)
    {
        GetSceneData()->mediaPhotonMap.mergeMap(*mapIter);
        //delete (*mapIter);
    }

    /* now actually build the kd-tree by sorting the array of photons */
    if (GetSceneData()->surfacePhotonMap.numPhotons>0)
    {
    //povwin::WIN32_DEBUG_FILE_OUTPUT("\n\nsurfacePhotonMap.buildTree about to be called\n");

        GetSceneData()->surfacePhotonMap.buildTree();
        GetSceneData()->surfacePhotonMap.setGatherOptions(GetSceneData()->photonSettings,false);
//      povwin::WIN32_DEBUG_FILE_OUTPUT("gatherNumSteps: %d\n",GetSceneData()->surfacePhotonMap.gatherNumSteps);
//      povwin::WIN32_DEBUG_FILE_OUTPUT("gatherRadStep: %lf\n",GetSceneData()->surfacePhotonMap.gatherRadStep);
//      povwin::WIN32_DEBUG_FILE_OUTPUT("minGatherRad: %lf\n",GetSceneData()->surfacePhotonMap.minGatherRad);
//      povwin::WIN32_DEBUG_FILE_OUTPUT("minGatherRadMult: %lf\n",GetSceneData()->surfacePhotonMap.minGatherRadMult);
//      povwin::WIN32_DEBUG_FILE_OUTPUT("numBlocks: %d\n",GetSceneData()->surfacePhotonMap.numBlocks);
//      povwin::WIN32_DEBUG_FILE_OUTPUT("numPhotons: %d\n",GetSceneData()->surfacePhotonMap.numPhotons);
    }

#ifdef GLOBAL_PHOTONS
    /* ----------- global photons ------------- */
    if (globalPhotonMap.numPhotons>0)
    {
        globalPhotonMap.buildTree();
        globalPhotonMap.setGatherOptions(false);
    }
#endif

    /* ----------- media photons ------------- */
    if (GetSceneData()->mediaPhotonMap.numPhotons>0)
    {
        GetSceneData()->mediaPhotonMap.buildTree();
        GetSceneData()->mediaPhotonMap.setGatherOptions(GetSceneData()->photonSettings,true);
    }

    if (GetSceneData()->surfacePhotonMap.numPhotons+
#ifdef GLOBAL_PHOTONS
        globalPhotonMap.numPhotons+
#endif
        GetSceneData()->mediaPhotonMap.numPhotons > 0)
    {
        /* should we save the photon map now that it is built? */
        if (!GetSceneData()->photonSettings.fileName.empty() && !GetSceneData()->photonSettings.loadFile)
        {
            /* status bar for user */
//          Send_Progress("Saving Photon Maps", PROGRESS_SAVING_PHOTON_MAPS);
            if (!this->save())
                mpMessageFactory->Warning(kWarningGeneral,"Could not save photon map.");
        }
    }
    else
    {
        if (!GetSceneData()->photonSettings.fileName.empty() && !GetSceneData()->photonSettings.loadFile)
            mpMessageFactory->Warning(kWarningGeneral,"Could not save photon map - no photons!");
    }
}


/* savePhotonMap()

  Saves the caustic photon map to a file.

  Preconditions:
    InitBacktraceEverything was called
    the photon map has been built and balanced
    photonSettings.fileName contains the filename to save

  Postconditions:
    Returns true if success, false if failure.
    If success, the photon map has been written to the file.
*/
bool PhotonSortingTask::save()
{
    Photon *ph;
    FILE *f;
    int i;
    size_t err;
    int numph;

    f = fopen(GetSceneData()->photonSettings.fileName.c_str(), "wb");
    if (!f)
        return false;

    /* caustic photons */
    numph = GetSceneData()->surfacePhotonMap.numPhotons;
    fwrite(&numph, sizeof(numph),1,f);
    if ((numph > 0) && !GetSceneData()->surfacePhotonMap.mBlockList.empty())
    {
        for(i=0; i<numph; i++)
        {
            ph = &GetSceneData()->surfacePhotonMap.GetPhoton(i);
            err = fwrite(ph, sizeof(Photon), 1, f);

            if (err<=0)
            {
                /* fwrite returned an error! */
                fclose(f);
                return false;
            }
        }
    }
    else
    {
        mpMessageFactory->PossibleError("Photon map for surface is empty.");
    }

#ifdef GLOBAL_PHOTONS
    /* global photons */
    numph = globalPhotonMap.numPhotons;
    fwrite(&numph, sizeof(numph),1,f);
    if ((numph > 0) && !globalPhotonMap.mBlockList.empty())
    {
        for(i=0; i<numph; i++)
        {
            ph = &globalPhotonMap.GetPhoton(i);
            err = fwrite(ph, sizeof(Photon), 1, f);

            if (err<=0)
            {
                /* fwrite returned an error! */
                fclose(f);
                return false;
            }
        }
    }
    else
    {
        mpMessageFactory->PossibleError("Global photon map is empty.");
    }
#endif

    /* media photons */
    numph = GetSceneData()->mediaPhotonMap.numPhotons;
    fwrite(&numph, sizeof(numph),1,f);
    if ((numph > 0) && !GetSceneData()->mediaPhotonMap.mBlockList.empty())
    {
        for(i=0; i<numph; i++)
        {
            ph = &GetSceneData()->mediaPhotonMap.GetPhoton(i);
            err = fwrite(ph, sizeof(Photon), 1, f);

            if (err<=0)
            {
                /* fwrite returned an error! */
                fclose(f);
                return false;
            }
        }
    }
    else
    {
        mpMessageFactory->PossibleError("Photon map for media is empty.");
    }

    fclose(f);
    return true;
}

/* loadPhotonMap()

  Loads the caustic photon map from a file.

  Preconditions:
    InitBacktraceEverything was called
    the photon map is empty
    renderer->sceneData->photonSettings.fileName contains the filename to load

  Postconditions:
    Returns true if success, false if failure.
    If success, the photon map has been loaded from the file.
    If failure then the render should stop with an error
*/
bool PhotonSortingTask::load()
{
    int i;
    size_t err;
    Photon *ph;
    FILE *f;
    int numph;

    if (!GetSceneData()->photonSettings.photonsEnabled) return false;

    mpMessageFactory->Warning(kWarningGeneral,"Starting the load of photon file %s\n",GetSceneData()->photonSettings.fileName.c_str());

    f = fopen(GetSceneData()->photonSettings.fileName.c_str(), "rb");
    if (!f)
        return false;

    fread(&numph, sizeof(numph),1,f);

    for(i=0; i<numph; i++)
    {
        ph = GetSceneData()->surfacePhotonMap.AllocatePhoton();
        err = fread(ph, sizeof(Photon), 1, f);

        if (err<=0)
        {
            /* fread returned an error! */
            fclose(f);
            return false;
        }
    }

    if (!feof(f)) /* for backwards file format compatibility */
    {

#ifdef GLOBAL_PHOTONS
        /* global photons */
        fread(&numph, sizeof(numph),1,f);
        for(i=0; i<numph; i++)
        {
            ph = GetSceneData()->globalPhotonMap.AllocatePhoton();
            err = fread(ph, sizeof(Photon), 1, f);

            if (err<=0)
            {
                /* fread returned an error! */
                fclose(f);
                return false;
            }
        }
#endif

        /* media photons */
        fread(&numph, sizeof(numph),1,f);
        for(i=0; i<numph; i++)
        {
            ph = GetSceneData()->mediaPhotonMap.AllocatePhoton();
            err = fread(ph, sizeof(Photon), 1, f);

            if (err<=0)
            {
                /* fread returned an error! */
                fclose(f);
                return false;
            }
        }

    }

    fclose(f);
    return true;
}

}
// end of namespace pov
