//******************************************************************************
///
/// @file backend/scene/view.h
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

#ifndef POVRAY_BACKEND_VIEW_H
#define POVRAY_BACKEND_VIEW_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"
#include "backend/scene/view_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// POV-Ray header files (base module)
#include "base/types.h" // TODO - only appears to be pulled in for POVRect - can we avoid this?

// POV-Ray header files (core module)
#include "core/core_fwd.h"
#include "core/bounding/bsptree.h"
#include "core/lighting/radiosity.h"
#include "core/scene/camera.h"

// POV-Ray header files (backend module)
#include "backend/control/scene_fwd.h"
#include "backend/scene/viewthreaddata_fwd.h"
#include "backend/support/taskqueue.h"

namespace pov
{

using namespace pov_base;

class RTRData final
{
    public:
        RTRData(ViewData& v, int mrt);
        ~RTRData();

        /// wait for other threads to complete the current frame. returns the new camera if it's to change.
        const Camera *CompletedFrame();
        /// number of frames rendered in real-time raytracing mode
        unsigned int numRTRframes;
        /// this holds the pixels rendered in real-time-raytracing mode
        std::vector<POVMSFloat> rtrPixels;
        /// the number of render threads to wait for
        int numRenderThreads;
        /// the number of render threads that have completed the current RTR frame
        volatile int numRenderThreadsCompleted;

    private:
        ViewData& viewData;
        std::mutex counterMutex;
        std::mutex eventMutex;
        std::condition_variable event;
        int width;
        int height;
        unsigned int numPixelsCompleted;
};

/**
 *  ViewData class representing holding view specific data.
 *  For private use by View and Renderer classes only!
 *  Unlike scene data, dependencies on direct access to
 *  view specific data members has been removed, and as
 *  such there are no public members but only accessor
 *  methods. Please do not add public data members!!!
 */
class ViewData final
{
        // View needs access to the private view data constructor as well
        // as some private data in order to initialise it properly!
        friend class View;
    public:

        typedef std::set<unsigned int> BlockIdSet;

        /**
         *  Container for information about a rectangle to be retained between passes.
         *  To be subclasses by trace tasks.
         */
        class BlockInfo
        {
            public:
                virtual ~BlockInfo() {}
        };

        /**
         *  Get the next sub-rectangle of the view to render (if any).
         *  This method is called by the render threads when they have
         *  completed rendering one block and are ready to start rendering
         *  the next block.
         *  @param  rect            Rectangle to render.
         *  @param  serial          Rectangle serial number.
         *  @return                 True if there is another rectangle to be dispatched, false otherwise.
         */
        bool GetNextRectangle(POVRect& rect, unsigned int& serial);

        /**
         *  Get the next sub-rectangle of the view to render (if any).
         *  This method is called by the render threads when they have
         *  completed rendering one block and are ready to start rendering
         *  the next block.
         *  Avoids rectangles with certain offsets from busy rectangles.
         *  @param  rect            Rectangle to render.
         *  @param  serial          Rectangle serial number.
         *  @param  blockInfo       Additional information about the rectangle.
         *                          `nullptr` if the block is being dispatched for the first time.
         *  @param  stride          Avoid-Busy stride. If this value is non-zero, any blocks following a busy block
         *                          with an offset of a multiple of this value will not be dispatched until the busy block
         *                          has been completed.
         *  @return                 True if there is another rectangle ready to be dispatched, false otherwise.
         */
        bool GetNextRectangle(POVRect& rect, unsigned int& serial, BlockInfo*& blockInfo, unsigned int stride);

        /**
         *  Called to (fully or partially) complete rendering of a specific sub-rectangle of the view.
         *  The pixel data is sent to the frontend and pixel progress information
         *  is updated and sent to the frontend.
         *  @param  rect            Rectangle just completed.
         *  @param  serial          Serial number of rectangle just completed.
         *  @param  pixels          Pixels of completed rectangle.
         *  @param  size            Size of each pixel (width and height).
         *  @param  relevant        Mark the block as relevant for the final image for continue-trace.
         *  @param  complete        Mark the block as completely rendered for continue-trace.
         *  @param  completion      Approximate contribution of current pass to completion of this rectangle.
         *  @param  blockInfo       Pointer to additional information about the rectangle. If this value is non-`nullptr`,
         *                          the rectangle will be scheduled to be re-dispatched for another pass, and the
         *                          data passed to whichever rendering thread the rectangle will be re-dispatched to.
         *                          If this value is `nullptr`, the rectangle will not be re-dispatched.
         */
        void CompletedRectangle(const POVRect& rect, unsigned int serial, const std::vector<RGBTColour>& pixels,
                                unsigned int size, bool relevant, bool complete, float completion = 1.0,
                                BlockInfo* blockInfo = nullptr);

        /**
         *  Called to (fully or partially) complete rendering of a specific sub-rectangle of the view.
         *  The pixel data is sent to the frontend and pixel progress information
         *  is updated and sent to the frontend.
         *  @param  rect            Rectangle just completed.
         *  @param  serial          Serial number of rectangle just completed.
         *  @param  positions       Pixel positions within rectangle.
         *  @param  colors          Pixel colors for each pixel position.
         *  @param  size            Size of each pixel (width and height).
         *  @param  relevant        Mark the block as relevant for the final image for continue-trace.
         *  @param  complete        Mark the block as completely rendered for continue-trace.
         *  @param  completion      Approximate contribution of current pass to completion of this rectangle.
         *  @param  blockInfo       Pointer to additional information about the rectangle. If this value is non-`nullptr`,
         *                          the rectangle will be scheduled to be re-dispatched for another pass, and the
         *                          data passed to whichever rendering thread the rectangle will be re-dispatched to.
         *                          If this value is `nullptr`, the rectangle will not be re-dispatched.
         */
        void CompletedRectangle(const POVRect& rect, unsigned int serial, const std::vector<Vector2d>& positions,
                                const std::vector<RGBTColour>& colors, unsigned int size, bool relevant, bool complete,
                                float completion = 1.0, BlockInfo* blockInfo = nullptr);

        /**
         *  Called to (fully or partially) complete rendering of a specific sub-rectangle of the view without updating pixel data.
         *  Pixel progress information is updated and sent to the frontend.
         *  @param  rect            Rectangle just completed.
         *  @param  serial          Serial number of rectangle just completed.
         *  @param  completion      Approximate contribution of current pass to completion of this rectangle.
         *  @param  blockInfo       Pointer to additional information about the rectangle. If this value is non-`nullptr`,
         *                          the rectangle will be scheduled to be re-dispatched for another pass, and the
         *                          data passed to whichever rendering thread the rectangle will be re-dispatched to.
         *                          If this value is `nullptr`, the rectangle will not be re-dispatched.
         */
        void CompletedRectangle(const POVRect& rect, unsigned int serial, float completion = 1.0, BlockInfo* blockInfo = nullptr);

        /**
         *  Set the blocks not to generate with GetNextRectangle because they have
         *  already been rendered.
         *  @param  bsl             Block serial numbers to skip.
         *  @param  fs              First block to start with checking with serial number.
         */
        void SetNextRectangle(const BlockIdSet& bsl, unsigned int fs);

        /**
         *  Get width of view in pixels.
         *  @return                 Width in pixels.
         */
        inline unsigned int GetWidth() const { return width; }

        /**
         *  Get height of view in pixels.
         *  @return                 Height in pixels.
         */
        inline unsigned int GetHeight() const { return height; }

        /**
         *  Get area of view to be rendered in pixels.
         *  @return                 Area rectangle in pixels.
         */
        inline const POVRect& GetRenderArea() const { return renderArea; }

        /**
         *  Get the camera for this view.
         *  @return                 Current camera.
         */
        inline const Camera& GetCamera() const { return camera; }

        /**
         *  Get the scene data for this view.
         *  @return                 Scene data.
         */
        inline std::shared_ptr<BackendSceneData>& GetSceneData() { return sceneData; }

        /**
         *  Get the view id for this view.
         *  @return                 View id.
         */
        inline RenderBackend::ViewId GetViewId() { return viewId; } // TODO FIXME - more like a hack, need a better way to do this

        /**
         *  Get the highest trace level found when last rendering this view.
         *  @return                 Highest trace level found so far.
         */
        unsigned int GetHighestTraceLevel();

        /**
         *  Set the highest trace level found while rendering this view.
         *  @param  htl             Highest trace level found so far.
         */
        void SetHighestTraceLevel(unsigned int htl);

        /**
         *  Get the render quality features to use when rendering this view.
         *  @return                 Quality feature flags.
         */
        const QualityFlags& GetQualityFeatureFlags() const;

        /**
         *  Get the radiosity cache.
         *  @return                 Radiosity cache.
         */
        RadiosityCache& GetRadiosityCache();

        /**
         *  Get the value of the real-time raytracing option
         *  @return                 true if RTR was requested in render options
         */
        bool GetRealTimeRaytracing() { return realTimeRaytracing; }

        /**
         *  Return a pointer to the real-time raytracing data
         *  @return                 pointer to instance of class RTRData, or `nullptr` if RTR is not enabled
         */
        RTRData *GetRTRData() { return rtrData; }

    private:

        struct BlockPostponedEntry final
        {
            unsigned int blockId;
            unsigned int pass;
            BlockPostponedEntry(unsigned int id, unsigned int p) : blockId(id), pass(p) {}
        };

        /// pixels pending
        volatile unsigned int pixelsPending;
        /// pixels completed
        volatile unsigned int pixelsCompleted;
        /// Next block counter for algorithm to distribute parts of the scene to render threads.
        /// @note   Blocks with higher serial numbers may be dispatched out-of-order for certain reasons;
        ///         in that case, the dispatched block must be entered into @ref blockSkipList instead of
        ///         advancing this variable.
        /// @note   When advancing this variable, the new value should be checked against @ref blockSkipList;
        ///         if the value is in the list, it should be removed, and this variable advanced again,
        ///         repeating the process until a value is reached that is not found in @ref blockSkipList.
        volatile unsigned int nextBlock;
        /// next block counter mutex
        std::mutex nextBlockMutex;
        /// set data mutex
        std::mutex setDataMutex;
        /// Whether all blocks have been dispatched at least once.
        bool completedFirstPass;
        /// highest reached trace level
        unsigned int highestTraceLevel;
        /// width of view
        unsigned int width;
        /// height of view
        unsigned int height;
        /// width of view in blocks
        unsigned int blockWidth;
        /// height of view in blocks
        unsigned int blockHeight;
        /// width and height of a block
        unsigned int blockSize;
        /// List of blocks already rendered out-of-order.
        /// This list holds the serial numbers of all blocks ahead of nextBlock
        /// that have already been rendered in a previous aborted render now being continued.
        BlockIdSet blockSkipList;
        /// list of blocks currently rendering
        BlockIdSet blockBusyList;
        /// list of blocks postponed for some reason
        BlockIdSet blockPostponedList;
        /// list of additional block information
        std::vector<BlockInfo*> blockInfoList;
        /// area of view to be rendered
        POVRect renderArea;
        /// camera of this view
        Camera camera;
        /// generated radiosity data
        RadiosityCache radiosityCache;
        /// scene data
        std::shared_ptr<BackendSceneData> sceneData;
        /// view id
        RenderBackend::ViewId viewId;

        /// true if real-time raytracing is requested (experimental feature)
        bool realTimeRaytracing;
        /// data specifically associated with the RTR feature
        RTRData *rtrData;

        /// functions to compute the X & Y block
        void getBlockXY(const unsigned int nb, unsigned int &x, unsigned int &y);

        /// pattern number to use for rendering
        unsigned int renderPattern;

        /// adjusted step size for renderering (using clock arithmetic)
        unsigned int renderBlockStep;

        QualityFlags qualityFlags; // TODO FIXME - put somewhere else or split up

        /**
         *  Create view data.
         *  @param  sd              Scene data associated with the view data.
         */
        ViewData(std::shared_ptr<BackendSceneData> sd);

        /**
         *  Destructor.
         */
        ~ViewData();
};

/**
 *  View class representing an view with a specific camera
 *  being rendered.
 */
class View final
{
        // Scene needs access to the private view constructor!
        friend class Scene;
    public:
        /**
         *  Destructor. Rendering will be stopped as necessary.
         */
        ~View();

        /**
         *  Render the view with the specified options. Be
         *  aware that this method is asynchronous! Threads
         *  will be started to perform the parsing and this
         *  method will return. The frontend is notified by
         *  messages of the state of rendering and all warnings
         *  and errors found.
         *  Options shall be in a kPOVObjectClass_RenderOptions
         *  POVMS obect, which is created when parsing the INI
         *  file or command line in the frontend.
         *  @param  renderOptions   Render options to use.
         */
        void StartRender(POVMS_Object& renderOptions);

        /**
         *  Stop rendering. Rendering may take a few seconds to
         *  stop. Internally stopping is performed by throwing
         *  an exception at well-defined points.
         *  If rendering is not in progress, no action is taken.
         */
        void StopRender();

        /**
         *  Pause rendering. Rendering may take a few seconds to
         *  pause. Internally pausing is performed by checking
         *  flag at well-defined points, and if it is true, a
         *  loop will repeatedly set the render threads to sleep
         *  for a few milliseconds until the pause flag is
         *  cleared again or rendering is stopped.
         *  If rendering is not in progress, no action is taken.
         */
        void PauseRender();

        /**
         *  Resume rendering that has previously been stopped.
         *  If rendering is not paussed, no action is taken.
         */
        void ResumeRender();

        /**
         *  Determine if any render thread is currently running.
         *  @return                 True if any is running, false otherwise.
         */
        bool IsRendering();

        /**
         *  Determine if rendering is paused. The rendering is considered
         *  paused if at least one render thread is paused.
         *  @return                 True if paused, false otherwise.
         */
        bool IsPaused();

        /**
         *  Determine if a previously run render thread failed.
         *  @return                 True if failed, false otherwise.
         */
        bool Failed();

        /**
         *  Get the current render statistics for the view.
         *  Note that this will query each thread, compute the total
         *  and return it.
         *  @param[out] renderStats On return, the current statistics.
         */
        void GetStatistics(POVMS_Object& renderStats);
    private:
        /// running and pending render tasks for this view
        TaskQueue renderTasks;
        /// view thread data (i.e. statistics)
        std::vector<ViewThreadData *> viewThreadData;
        /// view data
        ViewData viewData;
        /// stop request flag
        bool stopRequsted;
        /// render control thread
        std::thread *renderControlThread;
        /// BSP tree mailbox
        BSPTree::Mailbox mailbox;

        View() = delete;
        View(const View&) = delete;

        /**
         *  Create an view and associate a scene's data with it.
         *  @param  sd              Scene data to be associated with the view.
         *  @param  width           Width of view in pixels.
         *  @param  height          Height of view in pixels.
         *  @param  vid             Id of this view to include with
         *                          POVMS messages sent to the frontend.
         */
        explicit View(std::shared_ptr<BackendSceneData> sd, unsigned int width, unsigned int height, RenderBackend::ViewId vid);

        View& operator=(const View&) = delete;

        /**
         *  Dispatch any shutdown messages appropriate at the end of rendering a view (e.g. max_gradient).
         *  @param  taskq           The task queue that executed this method.
         */
        void DispatchShutdownMessages(TaskQueue&);

        /**
         *  Send the render statistics upon completion of a render.
         *  @param  taskq           The task queue that executed this method.
         */
        void SendStatistics(TaskQueue& taskq);

        /**
         *  Set the blocks not to generate with GetNextRectangle because they have
         *  already been rendered.
         *  @param  taskq           The task queue that executed this method.
         *  @param  bsl             Block serial numbers to skip.
         *  @param  fs              First block to start with checking with serial number.
         */
        void SetNextRectangle(TaskQueue& taskq, std::shared_ptr<ViewData::BlockIdSet> bsl, unsigned int fs);

        /**
         *  Thread controlling the render task queue.
         */
        void RenderControlThread();

        /**
         *  Checks whether or not the point (camera origin) is within a hollow object.
         *  returns true if so. comes in two versions, one for manual iteration of
         *  the object list, and one for a bounding tree.
         */
        bool CheckCameraHollowObject(const Vector3d& point); // TODO - comment incomplete - consider moving elsewhere [trf]
        bool CheckCameraHollowObject(const Vector3d& point, const BBOX_TREE *node); // TODO - comment missing - consider moving elsewhere [trf]
};

}
// end of namespace pov

#endif // POVRAY_BACKEND_VIEW_H
