//******************************************************************************
///
/// @file backend/control/scene.cpp
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
#include "backend/control/scene.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>
#include <sstream>

// Boost header files
#include <boost/bind.hpp>

// POV-Ray header files (base module)
#include "base/image/colourspace.h"

// POV-Ray header files (core module)
#include "core/scene/tracethreaddata.h"

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"
#include "povms/povmsid.h"

// POV-Ray header files (parser module)
#include "parser/parsertypes.h"

// POV-Ray header files (backend module)
#include "backend/bounding/boundingtask.h"
#include "backend/control/parsertask.h"
#include "backend/scene/backendscenedata.h"
#include "backend/scene/view.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::min;
using std::max;

Scene::Scene(POVMSAddress backendAddr, POVMSAddress frontendAddr, RenderBackend::SceneId sid) :
    sceneData(new BackendSceneData()),
    stopRequsted(false),
    parserControlThread(nullptr)
{
    sceneData->tree = nullptr;
    sceneData->sceneId = sid;
    sceneData->backendAddress = backendAddr;
    sceneData->frontendAddress = frontendAddr;
}

Scene::~Scene()
{
    stopRequsted = true; // NOTE: Order is important here, set this before stopping the queue!
    parserTasks.Stop();

    if (parserControlThread != nullptr)
        parserControlThread->join();
    delete parserControlThread;

    for(std::vector<TraceThreadData*>::iterator i(sceneThreadData.begin()); i != sceneThreadData.end(); i++)
        delete (*i);
    sceneThreadData.clear();
}

void Scene::StartParser(POVMS_Object& parseOptions)
{
    size_t seed = 0; // TODO

    // A scene can only be parsed once
    if (parserControlThread == nullptr)
        parserControlThread = new std::thread(boost::bind(&Scene::ParserControlThread, this));
    else
        return;

    if (parseOptions.Exist(kPOVAttrib_Version))
    {
        sceneData->languageVersion = clip(int(parseOptions.GetFloat(kPOVAttrib_Version) * 100.0f + .5f), 100, 10000);
        sceneData->languageVersionSet = true;
    }

    sceneData->warningLevel = clip(parseOptions.TryGetInt(kPOVAttrib_WarningLevel, 9), 0, 9);

    sceneData->inputFile = parseOptions.TryGetUCS2String(kPOVAttrib_InputFile, "object.pov");
    sceneData->headerFile = parseOptions.TryGetUCS2String(kPOVAttrib_IncludeHeader, "");

    DBL outputWidth  = parseOptions.TryGetFloat(kPOVAttrib_Width, 160);
    DBL outputHeight = parseOptions.TryGetFloat(kPOVAttrib_Height, 120);
    sceneData->aspectRatio = outputWidth / outputHeight;

    sceneData->defaultFileType = parseOptions.TryGetInt(kPOVAttrib_OutputFileType, DEFAULT_OUTPUT_FORMAT); // TODO - should get DEFAULT_OUTPUT_FORMAT from the front-end
    sceneData->clocklessAnimation = parseOptions.TryGetBool(kPOVAttrib_ClocklessAnimation, false); // TODO - experimental code

    sceneData->splitUnions = parseOptions.TryGetBool(kPOVAttrib_SplitUnions, false);
    sceneData->removeBounds = parseOptions.TryGetBool(kPOVAttrib_RemoveBounds, true);
    sceneData->boundingMethod = clip<int>(parseOptions.TryGetInt(kPOVAttrib_BoundingMethod, 1), 1, 2);
    if(parseOptions.TryGetBool(kPOVAttrib_Bounding, true) == false)
        sceneData->boundingMethod = 0;

    sceneData->outputAlpha = parseOptions.TryGetBool(kPOVAttrib_OutputAlpha, false);
    if (!sceneData->outputAlpha)
        // if we're not outputting an alpha channel, precompose the scene background against a black "background behind the background"
        // (NB: Here, background color is still at its default of <0,0,0,0,1> = full transparency; we're changing that to opaque black.)
        sceneData->backgroundColour.Clear();

    // NB a value of '0' for any of the BSP parameters tells the BSP code to use its internal default
    sceneData->bspMaxDepth = parseOptions.TryGetInt(kPOVAttrib_BSP_MaxDepth, 0);
    sceneData->bspObjectIsectCost = clip<float>(parseOptions.TryGetFloat(kPOVAttrib_BSP_ISectCost, 0.0f), 0.0f, HUGE_VAL);
    sceneData->bspBaseAccessCost = clip<float>(parseOptions.TryGetFloat(kPOVAttrib_BSP_BaseAccessCost, 0.0f), 0.0f, HUGE_VAL);
    sceneData->bspChildAccessCost = clip<float>(parseOptions.TryGetFloat(kPOVAttrib_BSP_ChildAccessCost, 0.0f), 0.0f, HUGE_VAL);
    sceneData->bspMissChance = clip<float>(parseOptions.TryGetFloat(kPOVAttrib_BSP_MissChance, 0.0f), 0.0f, 1.0f - EPSILON);

    sceneData->realTimeRaytracing = parseOptions.TryGetBool(kPOVAttrib_RealTimeRaytracing, false);

    if(parseOptions.Exist(kPOVAttrib_Declare) == true)
    {
        POVMS_List ds;

        parseOptions.Get(kPOVAttrib_Declare, ds);
        for(int i = 1; i <= ds.GetListSize(); i++)
        {
            std::ostringstream sstr;
            POVMS_Attribute a;
            POVMS_Object d;

            ds.GetNth(i, d);
            d.Get(kPOVAttrib_Value, a);
            switch (a.Type())
            {
                case kPOVMSType_CString:
                    sstr << "\"" + d.TryGetString(kPOVAttrib_Value, "") + "\"";
                    break;

                case kPOVMSType_Float:
                    sstr << d.TryGetFloat(kPOVAttrib_Value, 0.0);
                    break;

                default:
                    // shouldn't happen unless we make a coding error
                    throw POV_EXCEPTION(kParamErr, "Invalid type passed in declare list");
            }

            sceneData->declaredVariables.insert(make_pair(d.GetString(kPOVAttrib_Identifier), sstr.str()));
        }
    }

    // do parsing
    sceneThreadData.push_back(dynamic_cast<TraceThreadData *>(parserTasks.AppendTask(new ParserTask(
        sceneData, pov_parser::ParserOptions(bool(parseOptions.Exist(kPOVAttrib_Clock)), parseOptions.TryGetFloat(kPOVAttrib_Clock, 0.0), seed)
        ))));

    // wait for parsing
    parserTasks.AppendSync();

    // do bounding - we always call this even if the bounding is turned off
    // because it also generates object statistics
    sceneThreadData.push_back(dynamic_cast<TraceThreadData *>(parserTasks.AppendTask(new BoundingTask(
        sceneData,
        clip<int>(parseOptions.TryGetInt(kPOVAttrib_BoundingThreshold, DEFAULT_AUTO_BOUNDINGTHRESHOLD),1,SIGNED16_MAX),
        seed
        ))));

    // wait for bounding
    parserTasks.AppendSync();

    // wait for bounding to finish
    parserTasks.AppendSync();

    // send statistics
    parserTasks.AppendFunction(boost::bind(&Scene::SendStatistics, this, _1));

    // send done message and compatibility data
    parserTasks.AppendFunction(boost::bind(&Scene::SendDoneMessage, this, _1));
}

void Scene::StopParser()
{
    parserTasks.Stop();

    RenderBackend::SendSceneFailedResult(sceneData->sceneId, kUserAbortErr, sceneData->frontendAddress);
}

void Scene::PauseParser()
{
    parserTasks.Pause();
}

void Scene::ResumeParser()
{
    parserTasks.Resume();
}

bool Scene::IsParsing()
{
    return parserTasks.IsRunning();
}

bool Scene::IsPaused()
{
    return parserTasks.IsPaused();
}

bool Scene::Failed()
{
    return parserTasks.Failed();
}

std::shared_ptr<View> Scene::NewView(unsigned int width, unsigned int height, RenderBackend::ViewId vid)
{
    if(parserTasks.IsDone() == false)
        throw POV_EXCEPTION_CODE(kNotNowErr);

    if((parserTasks.IsDone() == false) || (parserTasks.Failed() == true))
        throw POV_EXCEPTION_CODE(kNotNowErr);

    return std::shared_ptr<View>(new View(sceneData, width, height, vid));
}

POVMSAddress Scene::GetFrontendAddress() const
{
    return sceneData->frontendAddress;
}

void Scene::GetStatistics(POVMS_Object& parserStats)
{
    struct TimeData final
    {
        POV_LONG cpuTime;
        POV_LONG realTime;
        size_t samples;

        TimeData() : cpuTime(0), realTime(0), samples(0) { }
    };

    TimeData timeData[TraceThreadData::kMaxTimeType];

    for(std::vector<TraceThreadData*>::iterator i(sceneThreadData.begin()); i != sceneThreadData.end(); i++)
    {
        timeData[(*i)->timeType].realTime = max(timeData[(*i)->timeType].realTime, (*i)->realTime);
        if ((*i)->cpuTime >= 0)
            timeData[(*i)->timeType].cpuTime += (*i)->cpuTime;
        else
            timeData[(*i)->timeType].cpuTime = -1;
        timeData[(*i)->timeType].samples++;
    }

    for(size_t i = TraceThreadData::kUnknownTime; i < TraceThreadData::kMaxTimeType; i++)
    {
        if(timeData[i].samples > 0)
        {
            POVMS_Object elapsedTime(kPOVObjectClass_ElapsedTime);

            elapsedTime.SetLong(kPOVAttrib_RealTime, timeData[i].realTime);
            if (timeData[i].cpuTime >= 0)
                elapsedTime.SetLong(kPOVAttrib_CPUTime, timeData[i].cpuTime);
            elapsedTime.SetInt(kPOVAttrib_TimeSamples, POVMSInt(timeData[i].samples));

            switch(i)
            {
                case TraceThreadData::kParseTime:
                    parserStats.Set(kPOVAttrib_ParseTime, elapsedTime);
                    break;
                case TraceThreadData::kBoundingTime:
                    parserStats.Set(kPOVAttrib_BoundingTime, elapsedTime);
                    break;
            }
        }
    }

    parserStats.SetInt(kPOVAttrib_FiniteObjects, sceneData->numberOfFiniteObjects);
    parserStats.SetInt(kPOVAttrib_InfiniteObjects, sceneData->numberOfInfiniteObjects);
    parserStats.SetInt(kPOVAttrib_LightSources, POVMSInt(sceneData->lightSources.size()));
    parserStats.SetInt(kPOVAttrib_Cameras, POVMSInt(sceneData->cameras.size()));

    if(sceneData->boundingMethod == 2)
    {
        parserStats.SetInt(kPOVAttrib_BSPNodes, sceneData->nodes);
        parserStats.SetInt(kPOVAttrib_BSPSplitNodes, sceneData->splitNodes);
        parserStats.SetInt(kPOVAttrib_BSPObjectNodes, sceneData->objectNodes);
        parserStats.SetInt(kPOVAttrib_BSPEmptyNodes, sceneData->emptyNodes);
        parserStats.SetInt(kPOVAttrib_BSPMaxObjects, sceneData->maxObjects);
        parserStats.SetFloat(kPOVAttrib_BSPAverageObjects, sceneData->averageObjects);
        parserStats.SetInt(kPOVAttrib_BSPMaxDepth, sceneData->maxDepth);
        parserStats.SetFloat(kPOVAttrib_BSPAverageDepth, sceneData->averageDepth);
        parserStats.SetInt(kPOVAttrib_BSPAborts, sceneData->aborts);
        parserStats.SetFloat(kPOVAttrib_BSPAverageAborts, sceneData->averageAborts);
        parserStats.SetFloat(kPOVAttrib_BSPAverageAbortObjects, sceneData->averageAbortObjects);
    }
}

void Scene::SendStatistics(TaskQueue&)
{
    POVMS_Message parserStats(kPOVObjectClass_ParserStatistics, kPOVMsgClass_SceneOutput, kPOVMsgIdent_ParserStatistics);

    GetStatistics(parserStats);

    parserStats.SetInt(kPOVAttrib_SceneId, sceneData->sceneId);
    parserStats.SetSourceAddress(sceneData->backendAddress);
    parserStats.SetDestinationAddress(sceneData->frontendAddress);

    POVMS_SendMessage(parserStats);

    for(std::vector<TraceThreadData*>::iterator i(sceneThreadData.begin()); i != sceneThreadData.end(); i++)
        delete (*i);
    sceneThreadData.clear();
}

void Scene::SendDoneMessage(TaskQueue&)
{
    POVMS_Message doneMessage(kPOVObjectClass_ResultData, kPOVMsgClass_SceneOutput, kPOVMsgIdent_Done);
    doneMessage.SetInt(kPOVAttrib_SceneId, sceneData->sceneId);
    doneMessage.SetSourceAddress(sceneData->backendAddress);
    doneMessage.SetDestinationAddress(sceneData->frontendAddress);
    doneMessage.SetInt(kPOVAttrib_LegacyGammaMode, sceneData->gammaMode);
    if (sceneData->workingGamma)
    {
        doneMessage.SetInt(kPOVAttrib_WorkingGammaType, sceneData->workingGamma->GetTypeId());
        doneMessage.SetFloat(kPOVAttrib_WorkingGamma, sceneData->workingGamma->GetParam());
    }
    POVMS_SendMessage(doneMessage);
}

void Scene::ParserControlThread()
{
    bool sentFailedResult = false;

    while(stopRequsted == false)
    {
        while((parserTasks.Process() == true) && (stopRequsted == false)) { }

        if((parserTasks.IsDone() == true) && (parserTasks.Failed() == true) && (sentFailedResult == false))
        {
            RenderBackend::SendSceneFailedResult(sceneData->sceneId, parserTasks.FailureCode(kUncategorizedError), sceneData->frontendAddress);
            sentFailedResult = true;
        }

        if(stopRequsted == false)
        {
            std::this_thread::yield();
            Delay(10);
        }
    }
}

}
// end of namespace pov
