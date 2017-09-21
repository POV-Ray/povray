//******************************************************************************
///
/// @file vfe/win/console/winconsole.cpp
///
/// This file contains a basic proof-of-concept POV implementation using VFE.
///
/// @author Christopher J. Cason
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

#include "base/version_info.h"

#include "backend/povray.h"

#include "vfe.h"

#ifndef _CONSOLE
#error "You must define _CONSOLE in windows/povconfig/syspovconfig.h prior to building the console version, otherwise you will get link errors."
#endif

using namespace vfe;
using namespace vfePlatform;

namespace pov_frontend
{
    ////////////////////////////////
    // Called from the shellout code
    ////////////////////////////////
    bool MinimizeShellouts(void) { return false; } // TODO
    bool ShelloutsPermitted(void) { return false; } // TODO
}


void PrintStatus (vfeSession *session)
{
  string str;
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

void ErrorExit(vfeSession *session)
{
  fprintf (stderr, "%s\n", session->GetErrorString());
  session->Shutdown();
  delete session;
  exit (1);
}

const char* gSharedMemoryName = "leocad-povray";

struct lcSharedMemoryHeader
{
	uint32_t Version;
	uint32_t Width;
	uint32_t Height;
	uint32_t PixelsWritten;
	uint32_t PixelsRead;
};

class WinSharedMemoryDisplay : public vfeDisplay
{
public:
	WinSharedMemoryDisplay(unsigned int width, unsigned int height, GammaCurvePtr gamma, vfeSession *session, bool visible = false)
		: vfeDisplay( width, height, gamma, session, visible )
	{
		mBuffer = NULL;
		mMapFile = INVALID_HANDLE_VALUE;
	}

	virtual ~WinSharedMemoryDisplay()
	{
		Close();
	}

	virtual void Initialise()
	{
		if (mMapFile != INVALID_HANDLE_VALUE)
			return;

		int BufferSize = sizeof(lcSharedMemoryHeader) + GetWidth() * GetHeight() * sizeof(RGBA8);

		mMapFile = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BufferSize, gSharedMemoryName);

		if (!mMapFile)
			return;

		mBuffer = MapViewOfFile(mMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BufferSize);

		if (!mBuffer)
		{
			CloseHandle(mMapFile);
			mMapFile = INVALID_HANDLE_VALUE;
			return;
		}

		lcSharedMemoryHeader* Header = (lcSharedMemoryHeader*)mBuffer;
		Header->Version = 1;
		Header->Width = GetWidth();
		Header->Height = GetHeight();
		Header->PixelsWritten = 0;
		Header->PixelsRead = 0;
	}

	virtual void Close()
	{
		if (mBuffer)
		{
			lcSharedMemoryHeader* Header = (lcSharedMemoryHeader*)mBuffer;
			if (Header->PixelsWritten != Header->PixelsRead)
				Sleep(5000);

			UnmapViewOfFile(mBuffer);
			mBuffer = NULL;
		}

		if (mMapFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(mMapFile);
			mMapFile = INVALID_HANDLE_VALUE;
		}
	}

	virtual void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
	{
		lcSharedMemoryHeader* Header = (lcSharedMemoryHeader*)mBuffer;
		RGBA8* Pixels = (RGBA8*)(Header + 1);
		Pixels[y * GetWidth() + x] = colour;
		Header->PixelsWritten++;
	}

	virtual void DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
	{
		for (unsigned int x = x1; x <= x2; x++)
			DrawPixel(x, y1, colour);
		for (unsigned int x = x1; x <= x2; x++)
			DrawPixel(x, y2, colour);
		for (unsigned int y = y1; y <= y2; y++)
		{
			DrawPixel(x1, y, colour);
			DrawPixel(x2, y, colour);
		}
	}

	virtual void DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
	{
		for (unsigned int y = y1; y <= y2; y++)
			for (unsigned int x = x1; x <= x2; x++)
				DrawPixel(x, y, colour);
	}

	virtual void DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
	{
		for (int y = y1; y <= y2; y++)
			for (int x = x1; x <= x2; x++)
				DrawPixel(x, y, *colour++);
	}

protected:
	HANDLE mMapFile;
	void* mBuffer;
};

vfeDisplay *SharedMemoryDisplayCreator(unsigned int width, unsigned int height, GammaCurvePtr gamma, vfeSession *session, bool visible)
{
	return new WinSharedMemoryDisplay(width, height, gamma, session, visible);
}

int pause = 0;

// this is an example of a minimal console version of POV-Ray using the VFE
// (virtual front-end) library. it is NOT INTENDED TO BE A FULLY-FEATURED
// CONSOLE IMPLEMENTATION OF POV-RAY and is not officially supported. see
// the unix version for a example of a more comprehensive console build.
int main (int argc, char **argv)
{
  char              *s;
  vfeWinSession     *session = new vfeWinSession() ;
  vfeStatusFlags    flags;
  vfeRenderOptions  opts;

  session->SetDisplayCreator(SharedMemoryDisplayCreator);

  fprintf(stderr,
          "This is an example of a minimal console build of POV-Ray under Windows.\n\n"
          "Persistence of Vision(tm) Ray Tracer Version " POV_RAY_VERSION_INFO ".\n"
          DISTRIBUTION_MESSAGE_1 "\n"
          DISTRIBUTION_MESSAGE_2 "\n"
          DISTRIBUTION_MESSAGE_3 "\n"
          POV_RAY_COPYRIGHT "\n"
          DISCLAIMER_MESSAGE_1 "\n"
          DISCLAIMER_MESSAGE_2 "\n\n");

  if (session->Initialize(NULL, NULL) != vfeNoError)
    ErrorExit(session);

  if ((s = getenv ("POVINC")) != NULL)
    opts.AddLibraryPath (s);
  argv++;
  while (*argv)
  {
	  if (!strcmp(*argv, "-pause"))
	  {
		  pause = 1;
		  argv++;
	  }
	  else if (!strcmp(*argv, "-shared-memory"))
	  {
		  argv++;
		  if (*argv)
		  {
			  gSharedMemoryName = *argv;
			  argv++;
		  }
	  }
	  else
	  {
		  opts.AddCommand(*argv);
		  argv++;
	  }
  }

  if (pause)
	  MessageBox(0, "", "", MB_OK);

  if (session->SetOptions(opts) != vfeNoError)
    ErrorExit(session);
  if (session->StartRender() != vfeNoError)
    ErrorExit(session);

  bool pauseWhenDone = session->GetBoolOption("Pause_When_Done", false);
  while (((flags = session->GetStatus(true, 1000)) & stRenderShutdown) == 0)
    PrintStatus (session) ;
  session->Shutdown() ;
  PrintStatus (session) ;
  delete session;

  if (pauseWhenDone)
  {
    fprintf (stderr, "Press enter to continue ... ");
    fflush(stderr);
    getchar();
  }

  return 0;
}

