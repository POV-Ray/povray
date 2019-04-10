//******************************************************************************
///
/// @file backend/support/taskqueue.h
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

#ifndef POVRAY_BACKEND_TASKQUEUE_H
#define POVRAY_BACKEND_TASKQUEUE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <queue>

// Boost header files
#include <boost/function.hpp>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"

// POV-Ray header files (backend module)
#include "backend/support/task.h"

namespace pov
{

class TaskQueue final
{
        class TaskEntry final
        {
            public:
                enum EntryType
                {
                    kTask,
                    kSync,
                    kMessage,
                    kFunction,
                };

                TaskEntry(EntryType et) : entryType(et) { }
                TaskEntry(std::shared_ptr<Task> rt) : entryType(kTask), task(rt) { }
                TaskEntry(POVMS_Message& m) : entryType(kMessage), msg(m) { }
                TaskEntry(const boost::function1<void, TaskQueue&>& f) : entryType(kFunction), fn(f) { }
                ~TaskEntry() { }

                std::shared_ptr<Task> GetTask() { return task; }
                POVMS_Message& GetMessage() { return msg; }
                boost::function1<void, TaskQueue&>& GetFunction() { return fn; }

                EntryType GetEntryType() { return entryType; }
            private:
                EntryType entryType;
                std::shared_ptr<Task> task;
                POVMS_Message msg;
                boost::function1<void, TaskQueue&> fn;
        };
    public:
        TaskQueue();
        ~TaskQueue();

        void Stop();
        void Pause();
        void Resume();

        bool IsPaused();
        bool IsRunning();
        bool IsDone();
        bool Failed();

        int FailureCode(int defval = kNoError);

        ThreadData *AppendTask(Task *task);
        void AppendSync();
        void AppendMessage(POVMS_Message& msg);
        void AppendFunction(const boost::function1<void, TaskQueue&>& fn);

        bool Process();

        void Notify();
    private:
        /// queued task list
        std::queue<TaskEntry> queuedTasks;
        /// active task list
        std::list<TaskEntry> activeTasks;
        /// queue mutex
        std::recursive_mutex queueMutex;
        /// failed code
        int failed;
        /// wait for data in queue or related operation to be processed
        std::condition_variable_any processCondition;

        TaskQueue(const TaskQueue&) = delete;
        TaskQueue& operator=(const TaskQueue&) = delete;
};

}
// end of namespace pov

#endif // POVRAY_BACKEND_TASKQUEUE_H
