//******************************************************************************
///
/// @file backend/control/scene.h
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

#ifndef POVRAY_BACKEND_SCENE_H
#define POVRAY_BACKEND_SCENE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"
#include "backend/control/scene_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <thread>
#include <vector>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/core_fwd.h"

// POV-Ray header files (backend module)
#include "backend/control/renderbackend.h"
#include "backend/scene/backendscenedata_fwd.h"
#include "backend/scene/view_fwd.h"
#include "backend/support/taskqueue.h"

namespace pov
{

using namespace pov_base;

/// Class governing the rendering of a scene.
///
/// @todo   Change the class name into something more expressive.
///
class Scene final
{
    public:
        /**
         *  Create a new POV-Ray scene. The scene is created with all
         *  default values and it empty. Only after a scene file has
         *  been parsed views can be created. Each scene may only be
         *  parsed once, and it is assumed that the scene structure
         *  remains static after parsing is complete. However, for
         *  each scene,once parsed, many views many be created, and
         *  each view may specify different render quality, size and
         *  camera parameters. This limitation may be lifted in the
         *  future to better support animations and for example
         *  motion blur, but this can only be supported in POV-Ray
         *  4.0 when all the rendering code is rewritten!
         *  @param  backendAddr     Address of the backend that owns this scene.
         *  @param  frontendAddr    Address of the frontend that owns this scene.
         *  @param  sid             Id of this scene to include with
         *                          POVMS messages sent to the frontend.
         */
        Scene(POVMSAddress backendAddr, POVMSAddress frontendAddr, RenderBackend::SceneId sid);

        /**
         *  Destructor. Parsing will be stopped as necessary.
         */
        ~Scene();

        /**
         *  Start parsing a POV-Ray scene. Be aware that this
         *  method is asynchronous! Threads will be started
         *  to perform the parsing and this method will return.
         *  The frontend is notified by messages of the state
         *  of parsing and all warnings and errors found.
         *  Options shall be in a kPOVObjectClass_ParserOptions
         *  POVMS object, which is created when parsing the INI
         *  file or command line in the frontend.
         *  @param  parseOptions    Options to use for parsing.
         */
        void StartParser(POVMS_Object& parseOptions);

        /**
         *  Stop parsing a POV-Ray scene. Parsing may take a few
         *  seconds to stop. Internally stopping is performed by
         *  throwing an exception at well-defined points.
         *  If parsing is not in progress, no action is taken.
         */
        void StopParser();

        /**
         *  Pause parsing. Parsing may take a few seconds to
         *  pause. Internally pausing is performed by checking
         *  flag at well-defined points, and if it is true, a
         *  loop will repeatedly set the parser thread to sleep
         *  for a few milliseconds until the pause flag is
         *  cleared again or parsing is stopped.
         *  If parsing is not in progress, no action is taken.
         */
        void PauseParser();

        /**
         *  Resume parsing that has previously been stopped.
         *  If parsing is not paused, no action is taken.
         */
        void ResumeParser();

        /**
         *  Determine if any parser thread is currently running.
         *  @return                 True if running, false otherwise.
         */
        bool IsParsing();

        /**
         *  Determine if parsing is paused.
         *  @return                 True if paused, false otherwise.
         */
        bool IsPaused();

        /**
         *  Determine if a previously run parser thread failed.
         *  @return                 True if failed, false otherwise.
         */
        bool Failed();

        /**
         *  Create a new view of a parsed scene. Note that this method
         *  may only be called after a scene has been parsed successfully.
         *  Note that the view does remain valid even if the scene that
         *  created it is deleted!
         *  @param  width           Width of the view in pixels.
         *  @param  height          Height of the view in pixels.
         *  @param  vid             Id of this view to include with
         *                          POVMS messages sent to the frontend.
         *  @return                 New view bound to the scene's data.
         */
        std::shared_ptr<View> NewView(unsigned int width, unsigned int height, RenderBackend::ViewId vid);

        /**
         *  Get the POVMS frontend address to send messages to the frontend.
         *  @return                 Frontend address.
         */
        POVMSAddress GetFrontendAddress() const;

        /**
         *  Get the current parser statistics for the scene.
         *  Note that this will query each thread, compute the total
         *  and return it.
         *  @param  stats           On return, the current statistics.
         */
        void GetStatistics(POVMS_Object& stats);
    private:
        /// running and pending parser tasks for this scene
        TaskQueue parserTasks;
        /// scene thread data (e.g. statistics)
        std::vector<TraceThreadData *> sceneThreadData;
        /// scene data
        std::shared_ptr<BackendSceneData> sceneData;
        /// stop request flag
        bool stopRequsted;
        /// parser control thread
        std::thread *parserControlThread;

        /**
         *  Send the parser statistics upon completion of a parsing.
         *  @param   taskq          The task queue that executed this method.
         */
        void SendStatistics(TaskQueue& taskq);

        /**
         *  Send done message (including compatibility data) after parsing.
         *  @param   taskq          The task queue that executed this method.
         */
        void SendDoneMessage(TaskQueue& taskq);

        /**
         *  Thread controlling the parser task queue.
         */
        void ParserControlThread();
};

}
// end of namespace pov

#endif // POVRAY_BACKEND_SCENE_H
