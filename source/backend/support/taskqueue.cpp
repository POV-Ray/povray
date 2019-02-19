//******************************************************************************
///
/// @file backend/support/taskqueue.cpp
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
#include "backend/support/taskqueue.h"

// C++ variants of C standard header files
// C++ standard header files

// Boost header files
#include <boost/bind.hpp>

// POV-Ray header files (base module)
// POV-Ray header files (core module)
// POV-Ray header files (POVMS module)
//  (none at the moment)

// POV-Ray header files (backend module)
#include "backend/support/task.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::list;
using std::shared_ptr;

TaskQueue::TaskQueue() : failed(kNoError)
{
}

TaskQueue::~TaskQueue()
{
    Stop();
}

void TaskQueue::Stop()
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    // we pass through this list twice; the first time through only sets the cancel
    // flag, and the second time through waits for the threads to exit. if we only
    // the the one pass (and wait), the cancel flag for later threads would remain
    // unset whilst we wait for earlier threads to reach a point where they check
    // the status of the flag, which tends to cause the shutdown to take longer
    // (particularly with a large number of threads vs a small number of CPU's).
    for(list<TaskEntry>::iterator i(activeTasks.begin()); i != activeTasks.end(); i++)
        i->GetTask()->RequestStop();
    for(list<TaskEntry>::iterator i(activeTasks.begin()); i != activeTasks.end(); i++)
        i->GetTask()->Stop();

    activeTasks.clear();
    while(queuedTasks.empty() == false)
        queuedTasks.pop();

    Notify();
}

void TaskQueue::Pause()
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    for(list<TaskEntry>::iterator i(activeTasks.begin()); i != activeTasks.end(); i++)
        i->GetTask()->Pause();
}

void TaskQueue::Resume()
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    for(list<TaskEntry>::iterator i(activeTasks.begin()); i != activeTasks.end(); i++)
        i->GetTask()->Resume();
}

bool TaskQueue::IsPaused()
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    bool paused = false;

    for(list<TaskEntry>::iterator i(activeTasks.begin()); i != activeTasks.end(); i++)
        paused = paused || i->GetTask()->IsPaused();

    return paused;
}

bool TaskQueue::IsRunning()
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    bool running = !queuedTasks.empty();

    for(list<TaskEntry>::iterator i(activeTasks.begin()); i != activeTasks.end(); i++)
        running = running || i->GetTask()->IsRunning();

    return running;
}

bool TaskQueue::IsDone()
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    bool done = queuedTasks.empty();

    for(list<TaskEntry>::iterator i(activeTasks.begin()); i != activeTasks.end(); i++)
        done = done && i->GetTask()->IsDone();

    return done;
}

bool TaskQueue::Failed()
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    return (failed != kNoError);
}

int TaskQueue::FailureCode(int defval)
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    if(failed == kNoError)
        return defval;
    else
        return failed;
}

ThreadData *TaskQueue::AppendTask(Task *task)
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    failed = false;

    queuedTasks.push(TaskEntry(shared_ptr<Task>(task)));

    Notify();

    return task->GetDataPtr();
}

void TaskQueue::AppendSync()
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    queuedTasks.push(TaskEntry::kSync);

    Notify();
}

void TaskQueue::AppendMessage(POVMS_Message& msg)
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    queuedTasks.push(TaskEntry(msg));

    Notify();
}

void TaskQueue::AppendFunction(const boost::function1<void, TaskQueue&>& fn)
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex);

    queuedTasks.push(TaskEntry(fn));

    Notify();
}

bool TaskQueue::Process()
{
    std::unique_lock<std::recursive_mutex> lock(queueMutex);

    for(list<TaskEntry>::iterator i(activeTasks.begin()); i != activeTasks.end();)
    {
        if(failed == kNoError)
            failed = i->GetTask()->FailureCode();

        if(i->GetTask()->IsDone() == true)
        {
            list<TaskEntry>::iterator e(i);
            i++;
            activeTasks.erase(e);
        }
        else
            i++;
    }

    if(failed != kNoError)
    {
        Stop();
        return false;
    }

    if(queuedTasks.empty() == false)
    {
        switch(queuedTasks.front().GetEntryType())
        {
            case TaskEntry::kTask:
            {
                activeTasks.push_back(queuedTasks.front());
                queuedTasks.front().GetTask()->Start(boost::bind(&TaskQueue::Notify, this));
                queuedTasks.pop();
                break;
            }
            case TaskEntry::kSync:
            {
                if(activeTasks.empty() == true)
                    queuedTasks.pop();
                else
                    return false;
                break;
            }
            case TaskEntry::kMessage:
            {
                try { POVMS_SendMessage(queuedTasks.front().GetMessage()); } catch(pov_base::Exception&) { }
                queuedTasks.pop();
                break;
            }
            case TaskEntry::kFunction:
            {
                try { queuedTasks.front().GetFunction()(*this); } catch(pov_base::Exception&) { }
                queuedTasks.pop();
                break;
            }
        }
    }

    if(queuedTasks.empty() == true)
        processCondition.wait(lock);

    return (queuedTasks.empty() == false);
}

void TaskQueue::Notify()
{
    processCondition.notify_one();
}

}
// end of namespace pov
