//******************************************************************************
///
/// @file backend/support/task.cpp
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
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

#include <cassert>
#include <stdexcept>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/support/task.h"

#include "base/timer.h"
#include "base/types.h"

#include "backend/control/messagefactory.h"
#include "backend/scene/backendscenedata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using namespace pov_base;

Task::Task(ThreadData *td, const boost::function1<void, Exception&>& f) :
    taskData(td),
    fatalErrorHandler(f),
    stopRequested(false),
    paused(false),
    done(false),
    failed(kNoError),
    timer(nullptr),
    realTime(-1),
    cpuTime(-1),
    taskThread(nullptr),
    povmsContext(nullptr)
{
    if (td == nullptr)
        throw POV_EXCEPTION_STRING("Internal error: TaskData is NULL in Task constructor");
}

Task::~Task()
{
    Stop();

    // NOTE: Freeing taskData is the responsibility of the task creator!
    taskData = nullptr;
}

int Task::FailureCode(int defval)
{
    if(failed == kNoError)
        return defval;
    else
        return failed;
}

POV_LONG Task::ConsumedRealTime() const
{
    return realTime;
}

POV_LONG Task::ConsumedCPUTime() const
{
    return cpuTime;
}

void Task::Start(const boost::function0<void>& completion)
{
    if ((done == false) && (taskThread == nullptr))
        taskThread = NewBoostThread(boost::bind(&Task::TaskThread, this, completion), POV_THREAD_STACK_SIZE);
}

void Task::RequestStop()
{
    stopRequested = true;
}

void Task::Stop()
{
    stopRequested = true;

    if (taskThread != nullptr)
    {
        taskThread->join();
        delete taskThread;
        taskThread = nullptr;
    }
}

void Task::Pause()
{
    paused = true;
}

void Task::Resume()
{
    paused = false;
}

POV_LONG Task::ElapsedRealTime() const
{
    POV_TASK_ASSERT(timer != nullptr);
    return timer->ElapsedRealTime();
}

POV_LONG Task::ElapsedThreadCPUTime() const
{
    POV_TASK_ASSERT(timer != nullptr);
    return timer->ElapsedThreadCPUTime();
}

void Task::TaskThread(const boost::function0<void>& completion)
{
    int result;

    if((result = POVMS_OpenContext(&povmsContext)) != kNoErr)
    {
        failed = result;
        timer = nullptr;
        done = true;
        return;
    }

    Initialize();

    Timer tasktime;

    timer = &tasktime;

    try
    {
        Run();
    }
    catch(StopThreadException&)
    {
        try
        {
            Stopped();
        }
        catch(pov_base::Exception& e)
        {
            FatalErrorHandler(e);
            failed = e.code(kUncategorizedError);
        }
        catch(std::exception& e)
        {
            FatalErrorHandler(POV_EXCEPTION_STRING(e.what()));
            failed = kUncategorizedError;
        }
        catch(...)
        {
            FatalErrorHandler(POV_EXCEPTION_STRING("An unknown error occured stopping a task!"));
            failed = kUncategorizedError;
        }
    }
    catch(std::bad_alloc&)
    {
        // can't raise a new exception object here as the alloc will probably fail
#if 0
        FatalErrorHandler(POV_EXCEPTION_STRING("Out of memory! There is not enough memory available for\n"
                                               "POV-Ray to complete a task. Usually this suggests the scene\n"
                                               "you are trying to render is too complex for your hardware\n"
                                               "to handle. Refer to the platform-specific documentation for\n"
                                               "details, it might contain hints how to work around this.\n"));
#endif
        failed = kOutOfMemoryErr;
    }
    catch(pov_base::Exception& e)
    {
        FatalErrorHandler(e);
        failed = e.code(kUncategorizedError);
    }
    catch(std::exception& e)
    {
        FatalErrorHandler(POV_EXCEPTION_STRING(e.what()));
        failed = kUncategorizedError;
    }
    catch(...)
    {
        FatalErrorHandler(POV_EXCEPTION_STRING("An unknown error occured running a task!"));
        failed = kUncategorizedError;
    }

    realTime = tasktime.ElapsedRealTime();
    if(tasktime.HasValidThreadCPUTime() == true)
        cpuTime = tasktime.ElapsedThreadCPUTime();
    else
        cpuTime = -1;

    try
    {
        Finish();
    }
    catch(pov_base::Exception& e)
    {
        FatalErrorHandler(e);
    }
    catch(std::exception& e)
    {
        FatalErrorHandler(POV_EXCEPTION_STRING(e.what()));
    }
    catch(...)
    {
        FatalErrorHandler(POV_EXCEPTION_STRING("An unknown error occured finishing a task!"));
    }

    timer = nullptr;
    done = true;

    Cleanup();

    (void)POVMS_CloseContext(povmsContext);

    completion();
}


#if POV_USE_DEFAULT_TASK_INITIALIZE

void Task::Initialize ()
{
    // TODO
}

#endif // POV_USE_DEFAULT_TASK_INITIALIZE

#if POV_USE_DEFAULT_TASK_CLEANUP

void Task::Cleanup ()
{
    // TODO
}

#endif // POV_USE_DEFAULT_TASK_CLEANUP


SceneTask::SceneTask(ThreadData *td, const boost::function1<void, Exception&>& f, const char* sn, shared_ptr<BackendSceneData> sd, RenderBackend::ViewId vid) :
    Task(td, f),
    messageFactory(sd->warningLevel, sn, sd->backendAddress, sd->frontendAddress, sd->sceneId, vid)
{}

}
