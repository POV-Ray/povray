//******************************************************************************
///
/// @file web/src/webconsole.cpp
///
/// Entry point for the WebAssembly (Emscripten) build of POV-Ray.
///
/// This is a variant of `vfe/unix/unixconsole.cpp`. Instead of rendering one
/// scene and exiting, `main()` keeps a single POV-Ray session alive and runs
/// render jobs submitted from JavaScript, much like the Windows GUI renders
/// many scenes in one process. (Starting a fresh WebAssembly instance for
/// every render leaks memory in browsers, since each instance reserves a
/// large shared memory that is not reliably reclaimed.)
///
/// Instead of an SDL or text display it renders into an RGBA framebuffer that
/// lives in (shared) WebAssembly memory, so that the JavaScript side can poll
/// it and show a live preview on an HTML canvas. Cancellation is requested
/// from JavaScript via an exported function instead of POSIX signals.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2021 Persistence of Vision Raytracer Pty. Ltd.
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
/// @endparblock
///
//******************************************************************************

// C++ variants of C standard header files
#include <cstdint>
#include <cstdio>
#include <cstdlib>

// C++ standard header files
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

// Other library header files
#include <emscripten.h>
#include <unistd.h>

// from directory "vfe"
#include "vfe.h"
#include "unixoptions.h"

// from directory "source"
#include "backend/povray.h"

namespace pov_frontend
{
    // Referenced by the Unix display code; unused here but kept for parity.
    std::shared_ptr<Display> gDisplay;
}
// end of namespace pov_frontend

using namespace vfe;
using namespace vfePlatform;

enum ReturnValue
{
    RETURN_OK=0,
    RETURN_ERROR,
    RETURN_USER_ABORT
};

//******************************************************************************
// State shared with JavaScript.
//
// The JavaScript side runs on a different thread than the renderer, and reads
// these values directly out of the shared WebAssembly memory, so everything
// here is atomic.
//******************************************************************************

static std::atomic<bool>     gCancelRender(false);
static std::atomic<uint8_t*> gFramePixels(nullptr);
static std::atomic<int>      gFrameWidth(0);
static std::atomic<int>      gFrameHeight(0);
static std::atomic<uint32_t> gFrameSerial(0);
static std::atomic<int>      gCurrentFrame(0);
static std::atomic<int>      gTotalFrames(0);

// Framebuffers of the current job, one per display. They are freed when the
// next job starts rather than when their display is destroyed, because
// JavaScript may still read the latest one after the render ends.
static std::vector<std::unique_ptr<uint8_t[]>> gFrameBuffers;

// Job queue: JavaScript writes the command-line arguments of a render to
// kJobArgsFile (one per line) and calls povweb_submit().
static const char kJobArgsFile[] = "/povweb/job.args";
static std::mutex              gJobMutex;
static std::condition_variable gJobPending;
static unsigned int            gJobsSubmitted = 0;

extern "C"
{
    /// Start a render job, see kJobArgsFile.
    EMSCRIPTEN_KEEPALIVE void povweb_submit()
    {
        std::lock_guard<std::mutex> lock(gJobMutex);
        ++gJobsSubmitted;
        gJobPending.notify_one();
    }

    /// Ask the current render to stop.
    EMSCRIPTEN_KEEPALIVE void povweb_cancel() { gCancelRender = true; }

    /// RGBA8 preview framebuffer (`povweb_width() * povweb_height() * 4` bytes), or null.
    EMSCRIPTEN_KEEPALIVE uint8_t* povweb_framebuffer() { return gFramePixels; }
    EMSCRIPTEN_KEEPALIVE int povweb_width() { return gFrameWidth; }
    EMSCRIPTEN_KEEPALIVE int povweb_height() { return gFrameHeight; }

    /// Incremented whenever the preview framebuffer changes.
    EMSCRIPTEN_KEEPALIVE uint32_t povweb_frame_serial() { return gFrameSerial; }

    /// Animation progress.
    EMSCRIPTEN_KEEPALIVE int povweb_current_frame() { return gCurrentFrame; }
    EMSCRIPTEN_KEEPALIVE int povweb_total_frames() { return gTotalFrames; }
}

//******************************************************************************
// vfeDisplay that writes into the shared framebuffer.
//******************************************************************************

class WebDisplay final : public vfeDisplay
{
    public:

        WebDisplay(unsigned int w, unsigned int h, vfeSession *session, bool visible) :
            vfeDisplay(w, h, session, visible),
            mPixels(nullptr)
        {}

        virtual ~WebDisplay() override {}

        virtual void Initialise() override
        {
            const size_t size = size_t(GetWidth()) * size_t(GetHeight()) * 4;
            gFrameBuffers.emplace_back(new uint8_t[size]);
            mPixels = gFrameBuffers.back().get();
            std::fill(mPixels, mPixels + size, 0);
            gFrameWidth = int(GetWidth());
            gFrameHeight = int(GetHeight());
            gFramePixels = mPixels;
            ++gFrameSerial;
        }

        virtual void Close() override {}
        virtual void Show() override {}
        virtual void Hide() override {}

        virtual void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour) override
        {
            if (x >= GetWidth() || y >= GetHeight())
                return;
            Store(x, y, colour);
            ++gFrameSerial;
        }

        virtual void DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour) override
        {
            if (!Clip(x1, y1, x2, y2))
                return;
            for (unsigned int x = x1; x <= x2; ++x)
            {
                Store(x, y1, colour);
                Store(x, y2, colour);
            }
            for (unsigned int y = y1; y <= y2; ++y)
            {
                Store(x1, y, colour);
                Store(x2, y, colour);
            }
            ++gFrameSerial;
        }

        virtual void DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour) override
        {
            if (!Clip(x1, y1, x2, y2))
                return;
            for (unsigned int y = y1; y <= y2; ++y)
                for (unsigned int x = x1; x <= x2; ++x)
                    Store(x, y, colour);
            ++gFrameSerial;
        }

        virtual void DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour) override
        {
            const unsigned int w = x2 - x1 + 1;
            const unsigned int ox1 = x1, oy1 = y1;
            if (!Clip(x1, y1, x2, y2))
                return;
            for (unsigned int y = y1; y <= y2; ++y)
                for (unsigned int x = x1; x <= x2; ++x)
                    Store(x, y, colour[(y - oy1) * w + (x - ox1)]);
            ++gFrameSerial;
        }

        virtual void Clear() override
        {
            if (mPixels != nullptr)
                std::fill(mPixels, mPixels + size_t(GetWidth()) * size_t(GetHeight()) * 4, 0);
            ++gFrameSerial;
        }

    private:

        uint8_t* mPixels;

        inline void Store(unsigned int x, unsigned int y, const RGBA8& colour)
        {
            uint8_t* p = mPixels + (size_t(y) * GetWidth() + x) * 4;
            p[0] = colour.red;
            p[1] = colour.green;
            p[2] = colour.blue;
            p[3] = colour.alpha;
        }

        inline bool Clip(unsigned int& x1, unsigned int& y1, unsigned int& x2, unsigned int& y2)
        {
            if (mPixels == nullptr || x1 >= GetWidth() || y1 >= GetHeight() || x2 < x1 || y2 < y1)
                return false;
            if (x2 >= GetWidth())
                x2 = GetWidth() - 1;
            if (y2 >= GetHeight())
                y2 = GetHeight() - 1;
            return true;
        }
};

static vfeDisplay *WebDisplayCreator (unsigned int width, unsigned int height, vfeSession *session, bool visible)
{
    return new WebDisplay(width, height, session, visible);
}

//******************************************************************************
// Console output.
//******************************************************************************

static void PrintStatus (vfeSession *session)
{
    std::string str;
    vfeSession::MessageType type;

    while (session->GetNextCombinedMessage (type, str))
    {
        // The JavaScript front-end shows status lines separately from the
        // message log; prefix them so they can be told apart.
        if (type == vfeSession::mGenericStatus)
            fprintf (stdout, "\x01STATUS %s\n", str.c_str());
        else
            fprintf (stderr, "%s\n", str.c_str());
    }
}

static void PrintStatusChanged (vfeSession *session, State force = kUnknown)
{
    if (force == kUnknown)
        force = session->GetBackendState();
    switch (force)
    {
        case kParsing:
            fprintf (stderr, "==== [Parsing...] ==========================================================\n");
            fprintf (stdout, "\x01STATE parsing\n");
            break;
        case kRendering:
            fprintf (stderr, "==== [Rendering...] ========================================================\n");
            fprintf (stdout, "\x01STATE rendering\n");
            break;
        case kPausedRendering:
            fprintf (stderr, "==== [Paused...] ===========================================================\n");
            fprintf (stdout, "\x01STATE paused\n");
            break;
        default:
            break;
    }
}

static void PrintVersion(void)
{
    fprintf(stderr,
        "%s %s\n\n"
        "%s\n%s\n%s\n"
        "%s\n%s\n%s\n\n",
        PACKAGE_NAME, POV_RAY_VERSION,
        DISTRIBUTION_MESSAGE_1, DISTRIBUTION_MESSAGE_2, DISTRIBUTION_MESSAGE_3,
        POV_RAY_COPYRIGHT, DISCLAIMER_MESSAGE_1, DISCLAIMER_MESSAGE_2
    );
    fprintf(stderr,
        "Built-in features:\n"
        "  I/O restrictions:          %s\n"
        "  Display:                   %s\n"
        "  Supported image formats:   %s\n"
        "  Unsupported image formats: %s\n\n",
        BUILTIN_IO_RESTRICTIONS, BUILTIN_XWIN_DISPLAY, BUILTIN_IMG_FORMATS, MISSING_IMG_FORMATS
    );
    fprintf(stderr,
        "Compilation settings:\n"
        "  Build architecture:  %s\n"
        "  Built/Optimized for: %s\n"
        "  Compiler vendor:     %s\n"
        "  Compiler version:    %s\n"
        "  Compiler flags:      %s\n",
        BUILD_ARCH, BUILT_FOR, COMPILER_VENDOR, COMPILER_VERSION, CXXFLAGS
    );
}

static void CancelRender(vfeSession *session)
{
    session->CancelRender();  // request the backend to cancel
    PrintStatus (session);
    while (session->GetBackendState() != kReady)  // wait for the render to effectively shut down
        Delay(10);
    PrintStatus (session);
}

static std::vector<std::string> WaitForJob()
{
    static unsigned int jobsTaken = 0;
    {
        std::unique_lock<std::mutex> lock(gJobMutex);
        gJobPending.wait(lock, [] { return gJobsSubmitted != jobsTaken; });
        ++jobsTaken;
    }
    std::vector<std::string> args;
    std::ifstream file(kJobArgsFile);
    std::string line;
    while (std::getline(file, line))
        if (!line.empty())
            args.push_back(line);
    return args;
}

static ReturnValue RenderJob(vfeUnixSession *session, const std::vector<std::string>& args)
{
    vfeStatusFlags    flags;
    vfeRenderOptions  opts;

    gCancelRender = false;
    gCurrentFrame = 0;
    gTotalFrames = 0;
    gFramePixels = nullptr;
    gFrameWidth = 0;
    gFrameHeight = 0;
    gFrameBuffers.clear();
    ++gFrameSerial;

    for (const std::string& arg : args)
    {
        if (arg == "--version" || arg == "-version")
        {
            PrintVersion();
            return RETURN_OK;
        }
    }

    session->Reset();

    // Default number of render threads: the number of logical CPUs reported
    // by the browser (`navigator.hardwareConcurrency`).
    int nthreads = int(sysconf(_SC_NPROCESSORS_ONLN));
    if (nthreads < 1)
        nthreads = 4;
    opts.SetThreadCount(nthreads);

    // process INI settings
    session->GetUnixOptions()->Process_povray_ini(opts);
    for (const std::string& arg : args)
        opts.AddCommand (arg);

    // set all options and start rendering
    if ((session->SetOptions(opts) != vfeNoError) || (session->StartRender() != vfeNoError))
    {
        PrintStatus (session);
        fprintf(stderr, "%s\n", session->GetErrorString());
        return RETURN_ERROR;
    }

    // main render loop
    session->SetEventMask(stBackendStateChanged);  // immediately notify this event
    while (((flags = session->GetStatus(true, 200)) & stRenderShutdown) == 0)
    {
        if (gCancelRender)
        {
            fprintf(stderr, "\n%s: render cancelled by user\n", PACKAGE);
            CancelRender(session);
            break;
        }

        if (flags & stAnimationStatus)
        {
            gCurrentFrame = session->GetCurrentFrame();
            gTotalFrames = session->GetTotalFrames();
            fprintf(stderr, "\nRendering frame %d of %d (#%d)\n", session->GetCurrentFrame(), session->GetTotalFrames(), session->GetCurrentFrameId());
            fprintf(stdout, "\x01" "FRAME %d %d\n", session->GetCurrentFrame(), session->GetTotalFrames());
        }
        if (flags & stAnyMessage)
            PrintStatus (session);
        if (flags & stBackendStateChanged)
            PrintStatusChanged (session);
    }
    PrintStatus (session);

    if (session->Succeeded() == false)
        return gCancelRender ? RETURN_USER_ABORT : RETURN_ERROR;
    return RETURN_OK;
}

int main (int argc, char **argv)
{
    vfeUnixSession *session = new vfeUnixSession();
    if (session->Initialize(nullptr, nullptr) != vfeNoError)
    {
        fprintf(stderr, "%s\n", session->GetErrorString());
        fflush(stderr);
        MAIN_THREAD_ASYNC_EM_ASM({ Module["povwebReady"] && Module["povwebReady"](false); });
        delete session;
        return RETURN_ERROR;
    }
    session->SetDisplayCreator(WebDisplayCreator);
    MAIN_THREAD_ASYNC_EM_ASM({ Module["povwebReady"] && Module["povwebReady"](true); });

    // Serve render jobs for as long as the page lives.
    while (true)
    {
        std::vector<std::string> args = WaitForJob();
        ReturnValue retval = RenderJob(session, args);
        fflush(stdout);
        fflush(stderr);
        MAIN_THREAD_ASYNC_EM_ASM({ Module["povwebJobDone"] && Module["povwebJobDone"]($0); }, int(retval));
    }
}
