//******************************************************************************
///
/// @file backend/support/task.h
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

#ifndef POVRAY_BACKEND_TASK_H
#define POVRAY_BACKEND_TASK_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <queue>
#include <thread>

// Boost header files
#include <boost/function.hpp>

// POV-Ray header files (base module)
#include "base/base_fwd.h"
#include "base/timer.h"

// POV-Ray header files (backend module)
#include "backend/control/messagefactory_fwd.h"
#include "backend/control/renderbackend.h"
#include "backend/scene/backendscenedata_fwd.h"

namespace pov
{

using namespace pov_base;

class Task
{
    public:

        Task(ThreadData *td, const boost::function1<void, Exception&>& f);
        virtual ~Task();

        inline bool IsPaused() { return !done && paused; }
        inline bool IsRunning() { return !done && !paused && !stopRequested && (taskThread != nullptr); }
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
        void Cooperate();

        inline ThreadData *GetDataPtr() { return taskData; }

        inline POVMSContext GetPOVMSContext() { return povmsContext; }

    protected:

        struct StopThreadException final {}; // TODO - consider subclassing from std::exception hierarchy.

        virtual void Run() = 0;
        virtual void Stopped() = 0;
        virtual void Finish() = 0;

        POV_LONG ElapsedRealTime() const;
        POV_LONG ElapsedThreadCPUTime() const;

    private:

        /// task data pointer
        ThreadData *taskData;
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
        // pointer to timer or `nullptr`
        Timer *timer;
        // real time spend in task
        POV_LONG realTime;
        // CPU time spend in task
        POV_LONG cpuTime;
        /// task thread
        std::thread *taskThread;
        /// POVMS message receiving context
        POVMSContext povmsContext;

        inline void FatalErrorHandler(const Exception& e)
        {
            Exception pe(e);
            fatalErrorHandler(pe);
        }

        inline void FatalErrorHandler(Exception& e) { fatalErrorHandler(e); }

        Task() = delete;
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        /// Execute the thread.
        void TaskThread(const boost::function0<void>& completion);

        /// Called by @ref TaskThread() before Run() is invoked.
        ///
        /// This method is intended as a hook to inject platform-specific thread initialization code, to be run by every
        /// task at thread startup. To make use of this mechanism, set @ref POV_USE_DEFAULT_TASK_INITIALIZE to zero to
        /// knock out the default implementation and provide a platform-specific implementation somewhere else.
        ///
        void Initialize();

        /// Called by @ref TaskThread() after Run() returns.
        ///
        /// This method is intended as a hook to inject platform-specific thread cleanup code, to be run by every
        /// task at thread shutdown. To make use of this mechanism, set @ref POV_USE_DEFAULT_TASK_CLEANUP to zero to
        /// knock out the default implementation and provide a platform-specific implementation somewhere else.
        ///
        void Cleanup();
};


class SceneTask : public Task
{
    public:
        SceneTask(ThreadData *td, const boost::function1<void, Exception&>& f, const char* sn, std::shared_ptr<BackendSceneData> sd, RenderBackend::ViewId vid = 0);
        virtual ~SceneTask() override;

    protected:
        MessageFactory* mpMessageFactory;
};

}
// end of namespace pov

#endif // POVRAY_BACKEND_TASK_H
