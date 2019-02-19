//******************************************************************************
///
/// @file vfe/unix/unixconsole.cpp
///
/// Adapted from @ref vfe/win/console/winconsole.cpp
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

// C++ variants of C standard header files
#include <csignal>
#include <cstdlib>

// C++ standard header files
#include <memory>
#include <mutex>
#include <string>
#include <thread>

// Boost library files
#include <boost/algorithm/string.hpp>

// Other library header files
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

// from directory "vfe"
#include "vfe.h"

// from directory "unix"
#include "disp.h"
#include "disp_text.h"
#include "disp_sdl.h"

// from directory "source"
#include "backend/povray.h"
#include "backend/control/benchmark.h"

namespace pov_frontend
{
    std::shared_ptr<Display> gDisplay;
}
// end of namespace pov_frontend

using namespace vfe;
using namespace vfePlatform;

enum DispMode
{
    DISP_MODE_NONE,
    DISP_MODE_TEXT,
    DISP_MODE_SDL
};

static DispMode gDisplayMode;

enum ReturnValue
{
    RETURN_OK=0,
    RETURN_ERROR,
    RETURN_USER_ABORT
};

static bool gCancelRender = false;

// for handling asynchronous (external) signals
static int gSignalNumber = 0;
static std::mutex gSignalMutex;
static volatile bool gTerminateSignalHandler = false;


// TODO FIXME - This way to handle signals seems rather wonky, as it is subject
// to a potential race condition on `gSignalNumber` between `ProcessSignal()`
// polling the signal and `SignalHandler()` processing the next signal.
// Wouldn't it be smarter to register proper signal handlers?

static void SignalHandler (void)
{
    sigset_t  sigset;
    timespec  timeout;
    int       signum;

    sigfillset(&sigset);
    timeout.tv_sec  = 0;
    timeout.tv_nsec = 10000000; // 10 ms

    while(!gTerminateSignalHandler)
    {
        // Wait till a signal is caught.
#ifdef HAVE_SIGTIMEDWAIT
        signum = sigtimedwait(&sigset, nullptr, &timeout);
        if (signum == -1)
            continue; // Error. Presumably timed out; check whether to end the task.
#else
        (void)sigwait(&sigset, &signum);
#endif

        // Got a signal.
        std::lock_guard<std::mutex> lock(gSignalMutex);
        gSignalNumber = signum;
    }
}


static void ProcessSignal (void)
{
    std::lock_guard<std::mutex> lock(gSignalMutex);

    switch (gSignalNumber)
    {
        case  0:
            break;
#ifdef SIGQUIT
        case SIGQUIT:
            fprintf(stderr, "\n%s: received signal SIGQUIT: Quit; requested render cancel\n", PACKAGE);
            gCancelRender = true;
            break;
#endif
#ifdef SIGTERM
        case SIGTERM:
            fprintf(stderr, "\n%s: received signal SIGTERM: Termination; requested render cancel\n", PACKAGE);
            gCancelRender = true;
            break;
#endif
#ifdef SIGINT
        case SIGINT:
            fprintf(stderr, "\n%s: received signal SIGINT: Interrupt; requested render cancel\n", PACKAGE);
            gCancelRender = true;
            break;
#endif
#ifdef SIGPIPE
        case SIGPIPE:
            fprintf(stderr, "\n%s: received signal SIGPIPE: Broken pipe; requested render cancel\n", PACKAGE);
            gCancelRender = true;
            break;
#endif
        case SIGCHLD:
            // for now, ignore this (side-effect of the shell-out code).
            // once properly implemented, the shell-out code would want to know this has happened, though.
            break;

        default:
            // fprintf(stderr, "\n%s: received signal %d\n", PACKAGE, gSignalNumber);
            break;
    }
    gSignalNumber = 0;
}

static vfeDisplay *UnixDisplayCreator (unsigned int width, unsigned int height, vfeSession *session, bool visible)
{
    UnixDisplay *display = GetRenderWindow () ;
    switch (gDisplayMode)
    {
#ifdef HAVE_LIBSDL
        case DISP_MODE_SDL:
            if (display != nullptr && display->GetWidth() == width && display->GetHeight() == height)
            {
                UnixDisplay *p = new UnixSDLDisplay (width, height, session, false) ;
                if (p->TakeOver (display))
                    return p;
                delete p;
            }
            return new UnixSDLDisplay (width, height, session, visible) ;
            break;
#endif
        case DISP_MODE_TEXT:
            return new UnixTextDisplay (width, height, session, visible) ;
            break;
        default:
            return nullptr;
    }
}

static void PrintStatus (vfeSession *session)
{
    // TODO -- when invoked while processing "--help" command-line switch,
    //         GNU/Linux customs would be to print to stdout (among other differences).

    std::string str;
    vfeSession::MessageType type;
    static vfeSession::MessageType lastType = vfeSession::mUnclassified;

    while (session->GetNextCombinedMessage (type, str))
    {
        if (type != vfeSession::mGenericStatus)
        {
            if (lastType == vfeSession::mGenericStatus)
                fprintf (stderr, "\n") ;
            fprintf (stderr, "%s\n", str.c_str());
        }
        else
            fprintf (stderr, "%s\r", str.c_str());
        lastType = type;
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
            break;
        case kRendering:
#ifdef HAVE_LIBSDL
            if ((gDisplay != nullptr) && (gDisplayMode == DISP_MODE_SDL))
            {
                fprintf (stderr, "==== [Rendering... Press p to pause, q to quit] ============================\n");
            }
            else
            {
                fprintf (stderr, "==== [Rendering...] ========================================================\n");
            }
#else
            fprintf (stderr, "==== [Rendering...] ========================================================\n");
#endif
            break;
        case kPausedRendering:
#ifdef HAVE_LIBSDL
            if ((gDisplay != nullptr) && (gDisplayMode == DISP_MODE_SDL))
            {
                fprintf (stderr, "==== [Paused... Press p to resume] =========================================\n");
            }
            else
            {
                fprintf (stderr, "==== [Paused...] ===========================================================\n");
            }
#else
            fprintf (stderr, "==== [Paused...] ===========================================================\n");
#endif
            break;
        default:
            // Do nothing special.
            break;
    }
}

static void PrintVersion(void)
{
    // TODO -- GNU/Linux customs would be to print to stdout (among other differences).

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
        "  X Window display:          %s\n"
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

static void PrintGeneration(void)
{
    fprintf(stdout, "%s\n", POV_RAY_GENERATION POV_RAY_BETA_SUFFIX);
}

static void ErrorExit(vfeSession *session)
{
    fprintf(stderr, "%s\n", session->GetErrorString());
    session->Shutdown();
    delete session;
    std::exit(RETURN_ERROR);
}

static void CancelRender(vfeSession *session)
{
    session->CancelRender();  // request the backend to cancel
    PrintStatus (session);
    while (session->GetBackendState() != kReady)  // wait for the render to effectively shut down
        Delay(10);
    PrintStatus (session);
}

static void PauseWhenDone(vfeSession *session)
{
    GetRenderWindow()->UpdateScreen(true);
    GetRenderWindow()->PauseWhenDoneNotifyStart();
    while (GetRenderWindow()->PauseWhenDoneResumeIsRequested() == false)
    {
        ProcessSignal();
        if (gCancelRender)
            break;
        else
            Delay(10);
    }
    GetRenderWindow()->PauseWhenDoneNotifyEnd();
}

static ReturnValue PrepareBenchmark(vfeSession *session, vfeRenderOptions& opts, std::string& ini, std::string& pov, int argc, char **argv)
{
    // parse command-line options
    while (*++argv)
    {
        std::string s = std::string(*argv);
        boost::to_lower(s);
        // set number of threads to run the benchmark
        if (boost::starts_with(s, "+wt") || boost::starts_with(s, "-wt"))
        {
            s.erase(0, 3);
            int n = std::atoi(s.c_str());
            if (n)
                opts.SetThreadCount(n);
            else
                fprintf(stderr, "%s: ignoring malformed '%s' command-line option\n", PACKAGE, *argv);
        }
        // add library path
        else if (boost::starts_with(s, "+l") || boost::starts_with(s, "-l"))
        {
            s.erase(0, 2);
            opts.AddLibraryPath(s);
        }
    }

    int benchversion = pov::Get_Benchmark_Version();
    fprintf(stderr, "\
%s %s\n\n\
Entering the standard POV-Ray %s benchmark version %x.%02x.\n\n\
This built-in benchmark requires POV-Ray to be installed on your system\n\
before running it.  There will be neither display nor file output, and\n\
any additional command-line option except setting the number of render\n\
threads (+wtN for N threads) and library paths (+Lpath) will be ignored.\n\
To get an accurate benchmark result you might consider running POV-Ray\n\
with the Unix 'time' command (e.g. 'time povray -benchmark').\n\n\
The benchmark will run using %d render thread(s).\n\
Press <Enter> to continue or <Ctrl-C> to abort.\n\
",
        PACKAGE_NAME, POV_RAY_VERSION_INFO,
        VERSION_BASE, benchversion / 256, benchversion % 256,
        opts.GetThreadCount()
    );

    // wait for user input from stdin (including abort signals)
    while (true)
    {
        ProcessSignal();
        if (gCancelRender)
        {
            fprintf(stderr, "Render cancelled by user\n");
            return RETURN_USER_ABORT;
        }

        fd_set readset;
        struct timeval tv = {0,0};  // no timeout
        FD_ZERO(&readset);
        FD_SET(STDIN_FILENO, &readset);
        if (select(STDIN_FILENO+1, &readset, nullptr, nullptr, &tv) < 0)
            break;
        if (FD_ISSET(STDIN_FILENO, &readset))  // user input is available
        {
            char s[3];
            read(STDIN_FILENO, s, 1);         // read till <ENTER> is hit
            tcflush(STDIN_FILENO, TCIFLUSH);  // discard unread data
            break;
        }
        Delay(20);
    }

    std::string basename = UCS2toSysString(session->CreateTemporaryFile());
    ini = basename + ".ini";
    pov = basename + ".pov";
    if (pov::Write_Benchmark_File(pov.c_str(), ini.c_str()))
    {
        fprintf(stderr, "%s: creating %s\n", PACKAGE, ini.c_str());
        fprintf(stderr, "%s: creating %s\n", PACKAGE, pov.c_str());
        fprintf(stderr, "Running standard POV-Ray benchmark version %x.%02x\n", benchversion / 256, benchversion % 256);
    }
    else
    {
        fprintf(stderr, "%s: failed to write temporary files for benchmark\n", PACKAGE);
        return RETURN_ERROR;
    }

    return RETURN_OK;
}

static void CleanupBenchmark(vfeUnixSession *session, std::string& ini, std::string& pov)
{
    fprintf(stderr, "%s: removing %s\n", PACKAGE, ini.c_str());
    session->DeleteTemporaryFile(SysToUCS2String(ini.c_str()));
    fprintf(stderr, "%s: removing %s\n", PACKAGE, pov.c_str());
    session->DeleteTemporaryFile(SysToUCS2String(pov.c_str()));
}

static void TerminateSignalHandler(std::thread* sigthread)
{
    gTerminateSignalHandler = true;
#ifdef HAVE_SIGTIMEDWAIT
    // `std::thread`s must be `join`ed or `detach`ed before destruction,
    // otherwise their destructor causes ungraceful termination of the entire
    // program.
    sigthread->join();
#else
    // `std::thread`s must be `join`ed or `detach`ed before destruction,
    // otherwise their destructor causes ungraceful termination of the entire
    // program. `join` is not an option because we can't reliably make the
    // thread terminate itself, so we must trust the OS that it will kill any
    // `detach`ed threads when their parent process exits.
    // TODO - This is a hackish solution.
    sigthread->detach();
#endif
}

int main (int argc, char **argv)
{
    vfeUnixSession   *session;
    vfeStatusFlags    flags;
    vfeRenderOptions  opts;
    ReturnValue       retval = RETURN_OK;
    bool              running_benchmark = false;
    std::string       bench_ini_name;
    std::string       bench_pov_name;
    sigset_t          sigset;
    std::thread*      sigthread;
    char **           argv_copy=argv; /* because argv is updated later */
    int               argc_copy=argc; /* because it might also be updated */

    /*fprintf(stderr, "%s: This is a RELEASE CANDIDATE version of POV-Ray. General distribution is discouraged.\n", PACKAGE);*/

    // block some signals for this thread as well as those created afterwards
    sigemptyset(&sigset);

#ifdef SIGQUIT
    sigaddset(&sigset, SIGQUIT);
#endif
#ifdef SIGTERM
    sigaddset(&sigset, SIGTERM);
#endif
#ifdef SIGINT
    sigaddset(&sigset, SIGINT);
#endif
#ifdef SIGPIPE
    sigaddset(&sigset, SIGPIPE);
#endif
#ifdef SIGCHLD
    sigaddset(&sigset, SIGCHLD);
#endif

    pthread_sigmask(SIG_BLOCK, &sigset, nullptr);

    // create the signal handling thread
    sigthread = new std::thread(SignalHandler);

    session = new vfeUnixSession();
    if (session->Initialize(nullptr, nullptr) != vfeNoError)
        ErrorExit(session);

    // display mode registration
#ifdef HAVE_LIBSDL
    if (UnixSDLDisplay::Register(session))
        gDisplayMode = DISP_MODE_SDL;
    else
#endif
    if (UnixTextDisplay::Register(session))
        gDisplayMode = DISP_MODE_TEXT;
    else
        gDisplayMode = DISP_MODE_NONE;

    // default number of work threads: number of CPUs or 4
    int nthreads = 1;
#ifdef _SC_NPROCESSORS_ONLN  // online processors
    nthreads = sysconf(_SC_NPROCESSORS_ONLN);
#endif
#ifdef _SC_NPROCESSORS_CONF  // configured processors
    if (nthreads < 2)
        nthreads = sysconf(_SC_NPROCESSORS_CONF);
#endif
    if (nthreads < 2)
        nthreads = 4;
    opts.SetThreadCount(nthreads);

    // process command-line options
    session->GetUnixOptions()->ProcessOptions(&argc, &argv);
    if (session->GetUnixOptions()->isOptionSet("general", "help"))
    {
        session->Shutdown() ;
        PrintStatus (session) ;
        // TODO: general usage display (not yet in core code)
        session->GetUnixOptions()->PrintOptions();
        TerminateSignalHandler(sigthread);
        delete sigthread;
        delete session;
        return RETURN_OK;
    }
    else if (session->GetUnixOptions()->isOptionSet("general", "version"))
    {
        session->Shutdown() ;
        PrintVersion();
        TerminateSignalHandler(sigthread);
        delete sigthread;
        delete session;
        return RETURN_OK;
    }
    else if (session->GetUnixOptions()->isOptionSet("general", "generation"))
    {
        session->Shutdown();
        PrintGeneration();
        TerminateSignalHandler(sigthread);
        delete sigthread;
        delete session;
        return RETURN_OK;
    }
    else if (session->GetUnixOptions()->isOptionSet("general", "benchmark"))
    {
        retval = PrepareBenchmark(session, opts, bench_ini_name, bench_pov_name, argc, argv);
        if (retval == RETURN_OK)
            running_benchmark = true;
        else
        {
            session->Shutdown();
            TerminateSignalHandler(sigthread);
            delete sigthread;
            delete session;
            return retval;
        }
    }

    // process INI settings
    if (running_benchmark)
    {
        // read only the provided INI file and set minimal lib paths
        opts.AddLibraryPath(std::string(POVLIBDIR "/include"));
        opts.AddINI(bench_ini_name.c_str());
        opts.SetSourceFile(bench_pov_name.c_str());
    }
    else
    {
        char *s = std::getenv ("POVINC");
        session->SetDisplayCreator(UnixDisplayCreator);
        session->GetUnixOptions()->Process_povray_ini(opts);
        if (s != nullptr)
            opts.AddLibraryPath (s);
        while (*++argv)
            opts.AddCommand (*argv);
    }

    // set all options and start rendering
    if (session->SetOptions(opts) != vfeNoError)
    {
        fprintf(stderr,"\nProblem with option setting\n");
        for(int loony=0;loony<argc_copy;loony++)
        {
            fprintf(stderr,"%s%c",argv_copy[loony],loony+1<argc_copy?' ':'\n');
        }
        ErrorExit(session);
    }
    if (session->StartRender() != vfeNoError)
        ErrorExit(session);

    // set inter-frame pause for animation
    if (session->RenderingAnimation() && session->GetBoolOption("Pause_When_Done", false))
        session->PauseWhenDone(true);

    // main render loop
    session->SetEventMask(stBackendStateChanged);  // immediately notify this event
    while (((flags = session->GetStatus(true, 200)) & stRenderShutdown) == 0)
    {
        ProcessSignal();
        if (gCancelRender)
        {
            CancelRender(session);
            break;
        }

        if (flags & stAnimationStatus)
            fprintf(stderr, "\nRendering frame %d of %d (#%d)\n", session->GetCurrentFrame(), session->GetTotalFrames(), session->GetCurrentFrameId());
        if (flags & stAnyMessage)
            PrintStatus (session);
        if (flags & stBackendStateChanged)
            PrintStatusChanged (session);

        if (GetRenderWindow() != nullptr)
        {
            // early exit
            if (GetRenderWindow()->HandleEvents())
            {
                gCancelRender = true;  // will set proper return value
                CancelRender(session);
                break;
            }

            GetRenderWindow()->UpdateScreen();

            // inter-frame pause
            if (session->GetCurrentFrame() < session->GetTotalFrames()
            && session->GetPauseWhenDone()
            && (flags & stAnimationFrameCompleted) != 0
            && session->Failed() == false)
            {
                PauseWhenDone(session);
                if (! gCancelRender)
                    session->Resume();
            }
        }
    }

    // pause when done for single or last frame of an animation
    if (session->Failed() == false && GetRenderWindow() != nullptr && session->GetBoolOption("Pause_When_Done", false))
    {
        PrintStatusChanged(session, kPausedRendering);
        PauseWhenDone(session);
        gCancelRender = false;
    }

    if (running_benchmark)
        CleanupBenchmark(session, bench_ini_name, bench_pov_name);

    if (session->Succeeded() == false)
        retval = gCancelRender ? RETURN_USER_ABORT : RETURN_ERROR;
    session->Shutdown();
    PrintStatus (session);
    TerminateSignalHandler(sigthread);
    delete sigthread;
    delete session;

    return retval;
}
