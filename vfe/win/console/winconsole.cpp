//******************************************************************************
///
/// @file vfe/win/console/winconsole.cpp
///
/// This file contains a POV implementation using VFE.
///
/// @author Trevor SANDY<trevor.sandy@gmial.com>
/// @author Based on VFE proof-of-concept by Christopher J. Cason
/// and extensions adapted from vfe/unix/unixconsole.cpp
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Windows standard headers
#include <windows.h>
#include <stdio.h>

// boost headers
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

// version details
#include "base/version_info.h"

// from syspovconfig
#ifndef _CONSOLE
#error "You must define _CONSOLE in windows/povconfig/syspovconfig.h prior to building the console version, otherwise you will get link errors."
#endif

#ifdef HAVE_LIBSDL
// from libraries directory SDL include
#include <SDL.h>
#endif

// from directory "vfe"
#include "vfe.h"

// from directory "windows"
#include "disp.h"
#include "disp_text.h"
#include "disp_sdl.h"

#include "backend/povray.h"
#include "backend/control/benchmark.h"

// verbose tracing
using std::cerr;
using std::endl;

namespace pov_frontend
{
  shared_ptr<Display> gDisplay;

  ////////////////////////////////
  // Called from the shellout code
  ////////////////////////////////
  bool MinimizeShellouts(void) { return false; } // TODO
  bool ShelloutsPermitted(void) { return false; } // TODO
}

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

// for handling console events
BOOL WINAPI ConsoleHandler(DWORD);
HANDLE hStdin;
DWORD fdwSaveOldMode;

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
    switch(CEvent)
    {
    case CTRL_C_EVENT:
        fprintf(stderr, "\n%s: received CTRL_C_EVENT: CTRL+C; requested render cancel\n", PACKAGE);
        gCancelRender = true;
        break;
    case CTRL_BREAK_EVENT:
        fprintf(stderr, "\n%s: received CTRL_BREAK_EVENT: CTRL+BREAK; requested render cancel\n", PACKAGE);
        gCancelRender = true;
        break;
    case CTRL_CLOSE_EVENT:
        fprintf(stderr, "\n%s: received CTRL_CLOSE_EVENT: Program being closed; requested render cancel\n", PACKAGE);
        gCancelRender = true;
        break;
    case CTRL_LOGOFF_EVENT:
        fprintf(stderr, "\n%s: received CTRL_LOGOFF_EVENT: User is logging off; requested render cancel\n", PACKAGE);
        gCancelRender = true;
        break;
    case CTRL_SHUTDOWN_EVENT:
        fprintf(stderr, "\n%s: received CTRL_SHUTDOWN_EVENT: User shutting down; requested render cancel\n", PACKAGE);
        gCancelRender = true;
        break;
    default:
        gCancelRender = false;
    }
    return TRUE;
}

static vfeDisplay *WinConDisplayCreator(unsigned int width, unsigned int height, GammaCurvePtr gamma, vfeSession *session, bool visible)
{
	WinConDisplay *display = GetRenderWindow();
	switch (gDisplayMode)
	{
#ifdef HAVE_LIBSDL
	case DISP_MODE_SDL:
		if (display != NULL && display->GetWidth() == width && display->GetHeight() == height)
		{
			WinConDisplay *p = new WinConSDLDisplay(width, height, gamma, session, false);
			if (p->TakeOver(display))
				return p;
			delete p;
		}
		return new WinConSDLDisplay(width, height, gamma, session, visible);
		break;
#endif
	case DISP_MODE_TEXT:
		return new WinConTextDisplay(width, height, gamma, session, visible);
		break;
	default:
		return NULL;
	}
}

/* Show a message */
static void PrintMessage(const char *title, const char *message)
{
	fprintf(stderr, "%s: %s\n", title, message);
}

void PrintStatus(vfeSession *session)
{
	string str;
	vfeSession::MessageType type;
	static vfeSession::MessageType lastType = vfeSession::mUnclassified;

	while (session->GetNextCombinedMessage(type, str))
	{
		if (type != vfeSession::mGenericStatus)
		{
			if (lastType == vfeSession::mGenericStatus)
				fprintf(stderr, "\n");
			fprintf(stderr, "%s\n", str.c_str());
		}
		else
			fprintf(stderr, "%s\r", str.c_str());
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
            if ((gDisplay != NULL) && (gDisplayMode == DISP_MODE_SDL))
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
            if ((gDisplay != NULL) && (gDisplayMode == DISP_MODE_SDL))
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
      "  Supported image formats:   %s\n"
      "  Unsupported image formats: %s\n\n",
      BUILTIN_IO_RESTRICTIONS, BUILTIN_IMG_FORMATS, MISSING_IMG_FORMATS
  );
  fprintf(stderr,
      "Compilation settings:\n"
      "  Build architecture:  %s\n"
      "  Built/Optimized for: %s\n"
      "  Compiler vendor:     %s\n"
      "  Compiler version:    %d\n",
      BUILD_ARCH, BUILT_FOR, COMPILER_VENDOR, COMPILER_VERSION
  );
}

void ErrorExit(vfeSession *session)
{
  fprintf (stderr, "%s\n", session->GetErrorString());
  session->Shutdown();
  delete session;
  exit (1);
}

void BenchMarkErrorExit(LPSTR lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);
	// Restore input mode on exit.
	SetConsoleMode(hStdin, fdwSaveOldMode);
	ExitProcess(0);
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
		if (gCancelRender)
			break;
		else
			Delay(10);
	}
	GetRenderWindow()->PauseWhenDoneNotifyEnd();
}

static ReturnValue PrepareBenchmark(vfeSession *session, vfeRenderOptions& opts, string& ini, string& pov, int argc, char **argv)
{

  // parse command-line options
  while (*++argv)
  {
      string s = string(*argv);
      boost::algorithm::to_lower(s);
      // set number of threads to run the benchmark
      if (boost::starts_with(s, "+wt") || boost::starts_with(s, "-wt"))
      {
          s.erase(0, 3);
          int n = atoi(s.c_str());
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
Entering the standard " PACKAGE " %s benchmark version %x.%02x.\n\n\
This built-in benchmark requires " PACKAGE " to be installed on your system\n\
before running it.  There will be neither display nor file output, and\n\
any additional command-line option except setting the number of render\n\
threads (+wtN for N threads) and library paths (+Lpath) will be ignored.\n\
To get an accurate benchmark result you might consider running  " PACKAGE "\n\
with the Win 'time' command (e.g. 'time povray -benchmark').\n\n\
The benchmark will run using %d render thread(s).\n\
Press <Enter> to continue or <Ctrl-C> to abort.\n\
",
      PACKAGE_NAME, POV_RAY_VERSION_INFO,
      VERSION_BASE, benchversion / 256, benchversion % 256,
      opts.GetThreadCount()
  );

  DWORD cNumRead, fdwMode, i;
  INPUT_RECORD irInBuf[128];
  int counter = 0;

  // Get the standard input handle.
  hStdin = GetStdHandle(STD_INPUT_HANDLE);
  if (hStdin == INVALID_HANDLE_VALUE)
	  BenchMarkErrorExit("Invalid standard input handle.");

  // Save the current input mode, to be restored on exit.

  if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
	  BenchMarkErrorExit("Unable to get current console mode.");

  // Enable the window and mouse input events.

  fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
  if (!SetConsoleMode(hStdin, fdwMode))
	  BenchMarkErrorExit("Unable to set console mode with window and mouse input.");

  // wait for user input from stdin (including abort signals)
  while (true)
  {
	  if (gCancelRender)
	  {
		  fprintf(stderr, "Render cancelled by user\n");
		  return RETURN_USER_ABORT;
	  }

	  // Wait for user input events.
	  if (!ReadConsoleInput(
		  hStdin,      // input buffer handle
		  irInBuf,     // buffer to read into
		  128,         // size of read buffer
		  &cNumRead))  // number of records read
		  BenchMarkErrorExit("ReadConsoleInput");

	  if (cNumRead > 0)  // user input is available
	  {
		  for (i = 0; i < cNumRead; i++)     // read till <ENTER> is hit
		  {
			  if (irInBuf[i].EventType == KEY_EVENT && irInBuf[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
				  break;
		  }
	  }
	  Delay(20);
  }

  string basename = UCS2toASCIIString(session->CreateTemporaryFile());
  ini = basename + ".ini";
  pov = basename + ".pov";
  if (pov::Write_Benchmark_File(pov.c_str(), ini.c_str()))
  {
      fprintf(stderr, "%s: creating %s\n", PACKAGE, ini.c_str());
      fprintf(stderr, "%s: creating %s\n", PACKAGE, pov.c_str());
      fprintf(stderr, "Running standard " PACKAGE " benchmark version %x.%02x\n", benchversion / 256, benchversion % 256);
  }
  else
  {
      fprintf(stderr, "%s: failed to write temporary files for benchmark\n", PACKAGE);
      return RETURN_ERROR;
  }

  // Restore input mode on exit.
  SetConsoleMode(hStdin, fdwSaveOldMode);

  return RETURN_OK;
}

static void CleanupBenchmark(vfeWinSession *session, string& ini, string& pov)
{
    fprintf(stderr, "%s: removing %s\n", PACKAGE, ini.c_str());
    session->DeleteTemporaryFile(ASCIItoUCS2String(ini.c_str()));
    fprintf(stderr, "%s: removing %s\n", PACKAGE, pov.c_str());
    session->DeleteTemporaryFile(ASCIItoUCS2String(pov.c_str()));
}

// This is the console user interface build of POV-Ray under Windows
// using the VFE (virtual front-end) library. This implementation
// includes the same capabilities as the Unix console build.
// It is not officially supported.


/**
*  For SDL on Windows, declare main() function using C linkage like this:
*/
extern "C" int main(int argc, char **argv)
{
  char              *s;
  vfeWinSession     *session;
  vfeStatusFlags    flags;
  vfeRenderOptions  opts;
  ReturnValue       retval = RETURN_OK;
  bool              running_benchmark = false;
  string            bench_ini_name;
  string            bench_pov_name;
  char **           argv_copy=argv; /* because argv is updated later */
  int               argc_copy=argc; /* because it might also be updated */

  fprintf(stderr,
          "\nThis is the console user interface build of POV-Ray under Windows.\n\n"
          "Persistence of Vision(tm) Ray Tracer Version " POV_RAY_VERSION_INFO ".\n\n"
          DISTRIBUTION_MESSAGE_1 "\n"
          DISTRIBUTION_MESSAGE_2 "\n"
          DISTRIBUTION_MESSAGE_3 "\n"
          POV_RAY_COPYRIGHT "\n"
          DISCLAIMER_MESSAGE_1 "\n"
          DISCLAIMER_MESSAGE_2 "\n\n");

  // create handler to manage console signals
  if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE))
  {
     fprintf(stderr, "Unable to install console control handler!\n");
     return RETURN_ERROR;
  }

  // create display session
  session = new vfeWinSession();
  if (session->Initialize(NULL, NULL) != vfeNoError)
    ErrorExit(session);

  // display mode registration
#ifdef HAVE_LIBSDL
	if (WinConSDLDisplay::Register(session))
	{
		gDisplayMode = DISP_MODE_SDL;
#ifdef WIN_DEBUG
		PrintMessage("--INFO", "Display Mode: SDL.\n");
#endif
	}
	else
#endif
	if (WinConTextDisplay::Register(session))
	{
		gDisplayMode = DISP_MODE_TEXT;
#ifdef WIN_DEBUG
		PrintMessage("--INFO", "Display Mode: Text.\n");
#endif
	}
	else
	{
		gDisplayMode = DISP_MODE_NONE;
#ifdef WIN_DEBUG
		PrintMessage("--INFO", "Display Mode: None.\n");
#endif
	}

  // default number of work threads: number of CPUs or 4
  int nthreads = boost::thread::hardware_concurrency();
  if (nthreads < 2)
	  nthreads = 4;
  opts.SetThreadCount(nthreads);

  // process command-line options
  session->GetWinConOptions()->ProcessOptions(&argc, &argv);
  if (session->GetWinConOptions()->isOptionSet("general", "help"))
  {
    session->Shutdown() ;
    PrintStatus (session) ;
    // TODO: general usage display (not yet in core code)
    session->GetWinConOptions()->PrintOptions();
    delete session;
    return RETURN_OK;
  }
  else if (session->GetWinConOptions()->isOptionSet("general", "version"))
  {
    session->Shutdown() ;
    PrintVersion();
    delete session;
    return RETURN_OK;
  }
  else if (session->GetWinConOptions()->isOptionSet("general", "benchmark"))
  {
    retval = PrepareBenchmark(session, opts, bench_ini_name, bench_pov_name, argc, argv);
    if (retval == RETURN_OK)
      running_benchmark = true;
    else
    {
      session->Shutdown();
      delete session;
      return retval;
    }
  }

  // process INI settings
  if (running_benchmark)
  {
    // read only the provided INI file and set minimal lib paths
    opts.AddLibraryPath(string(POVLIBDIR "\\include"));
    opts.AddINI(bench_ini_name.c_str());
    opts.SetSourceFile(bench_pov_name.c_str());
  }
  else
  {
    s = getenv ("POVINC");
	session->SetDisplayCreator(WinConDisplayCreator);
    session->GetWinConOptions()->Process_povray_ini(opts);
    if (s != NULL)
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
	  if (gCancelRender)
	  {
		  CancelRender(session);
		  break;
	  }

	  if (flags & stAnimationStatus)
		  fprintf(stderr, "\nRendering frame %d of %d (#%d)\n", session->GetCurrentFrame(), session->GetTotalFrames(), session->GetCurrentFrameId());
	  if (flags & stAnyMessage)
		  PrintStatus(session);
	  if (flags & stBackendStateChanged)
		  PrintStatusChanged(session);

	  if (GetRenderWindow() != NULL)
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
			  if (!gCancelRender)
				  session->Resume();
		  }
	  }
  }

  // pause when done for single or last frame of an animation
  if (session->Failed() == false && GetRenderWindow() != NULL && session->GetBoolOption("Pause_When_Done", false))
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
  delete session;

  return retval;
}

