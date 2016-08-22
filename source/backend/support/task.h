//******************************************************************************
///
/// @file backend/support/task.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_BACKEND_TASK_H
#define POVRAY_BACKEND_TASK_H

#include <queue>
#include <vector>

#include <boost/thread.hpp>
#include <boost/function.hpp>

#include "backend/frame.h"

#include "backend/control/messagefactory.h"
#include "backend/control/renderbackend.h"
#include "base/timer.h"

namespace pov
{

using namespace pov_base;

class BackendSceneData;


class Task
{
    public:

        class TaskData
        {
                friend class Task;
            public:
                TaskData() { }
                virtual ~TaskData() { }
            private:
                Task *task;
        };

        Task(TaskData *td, const boost::function1<void, Exception&>& f);
        virtual ~Task();

        inline bool IsPaused() { return !done && paused; }
        inline bool IsRunning() { return !done && !paused && !stopRequested && (taskThread != NULL); }
        inline bool IsDone() { return done; }
        inline bool Failed() { return done && (failed != kNoError); }

        int FailureCode(int defval = kNoError);

        POV_LONG ConsumedRealTime() const;
        POV_LONG ConsumedCPUTime() const;

        void Start(const boost::function0<void>& completion);
        void RequestStop();
        void Stop();
        void Pause();
        void Resume();

        inline void Cooperate()
        {
            if(stopRequested == true)
                throw StopThreadException();
            else if(paused == true)
            {
                while(paused == true)
                {
                    boost::thread::yield();
                    Delay(100);
                    if(stopRequested == true)
                        throw StopThreadException();
                }
            }
        }

        inline TaskData *GetDataPtr() { return taskData; }

        inline POVMSContext GetPOVMSContext() { return povmsContext; }

        /// Start a new thread with a given stack size.
        template<typename CALLABLE_T>
        inline static boost::thread* NewBoostThread(CALLABLE_T func, int stackSize)
        {
#if HAVE_BOOST_THREAD_ATTRIBUTES
            // boost 1.50 and later provide an official mechanism to set the stack size.
            boost::thread::attributes attr;
            attr.set_stack_size (stackSize);
            return new boost::thread(attr, func);
#elif !defined(USE_OFFICIAL_BOOST)
            // Prior to boost 1.50, for some platforms we used an unofficial hacked version of boost to set the stack size.
            return new boost::thread(func, stackSize);
#else
            // For some platforms the default stack size of older boost versions may suffice.
            return new boost::thread(func);
#endif
        }

    protected:

        struct StopThreadException { };

        virtual void Run() = 0;
        virtual void Stopped() = 0;
        virtual void Finish() = 0;

        POV_LONG ElapsedRealTime() const;
        POV_LONG ElapsedCPUTime() const;

    private:

        /// task data pointer
        TaskData *taskData;
        /// task fatal error handler
        boost::function1<void, Exception&> fatalErrorHandler;
        /// stop request flag
        volatile bool stopRequested;
        /// paused flag
        volatile bool paused;
        /// done flag
        volatile bool done;
        /// failed code
        volatile int failed;
        // pointer to timer or NULL
        Timer *timer;
        // real time spend in task
        POV_LONG realTime;
        // CPU time spend in task
        POV_LONG cpuTime;
        /// task thread
        boost::thread *taskThread;
        /// POVMS message receiving context
        POVMSContext povmsContext;

        inline void FatalErrorHandler(const Exception& e)
        {
            Exception pe(e);
            fatalErrorHandler(pe);
        }

        inline void FatalErrorHandler(Exception& e) { fatalErrorHandler(e); }

        /// not available
        Task();

        /// not available
        Task(const Task&);

        /// not available
        Task& operator=(const Task&);

        void TaskThread(const boost::function0<void>& completion);
};


class SceneTask : public Task
{
    public:
        SceneTask(TaskData *td, const boost::function1<void, Exception&>& f, const char* sn, shared_ptr<BackendSceneData> sd, RenderBackend::ViewId vid = 0);

    protected:
        MessageFactory messageFactory;
};

}

#endif // POVRAY_BACKEND_TASK_H
