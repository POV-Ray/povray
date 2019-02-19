//******************************************************************************
///
/// @file windows/pvfrontend.cpp
///
/// This module contains the default C++ interface for render frontend.
///
/// Author: Christopher J. Cason and Thorsten Froelich.
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
//*******************************************************************************

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#include <windows.h>

#include "pvfrontend.h"
#include "pvengine.h"
#include "resource.h"
#include "pvdisplay.h"

// this must be the last file included
#include "syspovdebug.h"

namespace povwin
{
  bool                  ListenMode ;
  extern int            io_restrictions ;
  extern char           *WriteDirSpecs [] ;
  extern char           *ReadDirSpecs [] ;
  extern bool           first_frame ;
  extern bool           rendering_animation ;
  extern bool           preserve_bitmap ;
  extern bool           ErrorOccurred;
  extern bool           ErrorNotified;
  extern bool           no_shell_outs;
  extern bool           system_noactive;
  extern std::string    ErrorMessage ;
  extern std::string    ErrorFilename ;
  extern unsigned       ErrorLine ;
  extern unsigned       ErrorCol ;
}
// end of namespace povwin

namespace pov_frontend
{
using namespace povwin;
using namespace vfe;
using namespace vfePlatform;

static vfeWinSession    *gSession;

//////////////////////////////////////////////////////////////////////////////
// WinDisplayCreator
//
// This function is set as the session DisplayCreator by CreateFrontend below.
// Whenever the core POV code wants to create an output window, the below code
// will therefore be executed.
//////////////////////////////////////////////////////////////////////////////
vfeDisplay *WinDisplayCreator (unsigned int width, unsigned int height, vfeSession *session, bool visible)
{
  // we attempt to minimize 'flashing' of the window (destroy followed by a re-create)
  // by checking to see if the previous window (if any) had the same dimensions. if it
  // did then we effectively re-use it by taking over its resources. the actual instance
  // is however automatically destroyed once the global display shared pointer is reset.
  WinDisplay *display = GetRenderWindow () ;
  if (display != NULL && display->GetWidth() == width && display->GetHeight() == height)
  {
    WinDisplay *p ;
    p = new WinLegacyDisplay (width, height, session, false);
    if (p->TakeOver (display))
    {
      bool anim = gSession->RenderingAnimation();

      // if we're not running an animation, or if we are and it's the first
      // frame, OR the user doesn't want frame preservation, clear the window.
      if (!anim || !preserve_bitmap || first_frame)
        p->Clear () ;
      return p ;
    }
    delete p;
  }
  return new WinLegacyDisplay (width, height, session, visible);
}

//////////////////////////////////////////////////////////////////////////////
// CreateFrontend
//
// Calling the below function is one of the very first things that the main
// PVENGINE code does once it starts up. Since the legacy POVWIN code only
// supports one rendering session at once, we store the session pointer in
// a global variable (gSession). The session itself is an instance of the
// vfeWinSession class declared in the windows platform-specific portion of
// VFE, and is a descendant of vfeSession. Note that gSession is static and
// not directly available outside this file; however a reference to the
// current session may be obtained by the POVWIN code via GetSession() below.
//////////////////////////////////////////////////////////////////////////////
void CreateFrontend (void)
{
  if (gSession != NULL)
    throw POV_EXCEPTION_STRING ("Session already open");
  try
  {
    gSession = new vfeWinSession();
  }
  catch(vfeException& e)
  {
    throw POV_EXCEPTION_STRING (e.what());
  }
  if (gSession == NULL)
    throw POV_EXCEPTION_STRING ("Failed to create session");
  gSession->OptimizeForConsoleOutput(false);
  if (gSession->Initialize(NULL, NULL) != vfeNoError)
  {
    gSession->Shutdown();
    std::string str = gSession->GetErrorString();
    delete gSession;
    gSession = NULL;
    throw POV_EXCEPTION_STRING (str.c_str());
  }
  gSession->SetDisplayCreator(WinDisplayCreator);
}

//////////////////////////////////////////////////////////////////////////////
// SetupFrontend
//
// This function loads the IO restriction paths into the session. It is called
// initially upon startup by the main POVWIN code, and subsequently any time
// the user changes the IO restriction settings via the menu options.
//////////////////////////////////////////////////////////////////////////////
void SetupFrontend (void)
{
  // 0: no restrictions
  // 1: restrict write
  // 2: restrict read/write
  gSession->ClearPaths();
  if (io_restrictions != 0)
  {
    for (char **dirspec = WriteDirSpecs ; *dirspec != NULL ; dirspec++)
      gSession->AddWritePath (std::string(*dirspec), true);
    if (io_restrictions > 1)
    {
      gSession->AddReadPath(std::string(DocumentsPath), true);
      gSession->AddReadPath(std::string(FontPath) + "\\", false);
      for (char **dirspec = ReadDirSpecs ; *dirspec != NULL ; dirspec++)
        gSession->AddReadPath (std::string(*dirspec), true);
    }

    // allow write to the insert menu directory
    std::string str(DocumentsPath);
    str += "Insert Menu\\";
    gSession->AddWritePath (str, true);

    // now add excluded write paths (e.g. the path containing the restrictions INI file)
    str = DocumentsPath;
    str += "ini\\";
    gSession->AddExcludedPath(str, true);
  }
}

//////////////////////////////////////////////////////////////////////////////
// DeleteFrontend
//
// Destroys the global session after calling its Shutdown method.
// This is more or less the last thing done by PVENGINE before exiting.
//////////////////////////////////////////////////////////////////////////////
void DeleteFrontend (void)
{
  if (gSession != NULL)
  {
    gSession->Shutdown() ;
    delete gSession;
    gSession = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////////
// GetSession
//
// Returns a reference to the current vfeWinSession instance. Throws an
// exception if one does not exist. (Since sessions in the current POVWIN
// code last until exit, it is safe to return a reference).
//////////////////////////////////////////////////////////////////////////////
vfeWinSession &GetSession (void)
{
  if (gSession == NULL)
    throw POV_EXCEPTION_STRING ("No session open");
  return *gSession;
}

//////////////////////////////////////////////////////////////////////////////
// HaveSession
//
// Used by the POVWIN code to check that a session exists.
//////////////////////////////////////////////////////////////////////////////
bool HaveSession (void)
{
  return gSession != NULL;
}

#if 0
void DrawFrame()
{
  if (gDrawNow)
  {
    if (gDisplay)
    {
      WinLegacyDisplay *p = dynamic_cast<WinLegacyDisplay *>(gDisplay.get());
      p->DrawPixelBlock(0, 0, gDisplay->GetWidth() - 1, gDisplay->GetHeight() - 1, (pov_frontend::Display::RGBA8 *) gFrontBufferPtr);

      RECT rect;
      GetClientRect (p->GetHandle(), &rect);
      HDC dc=GetDC(p->GetHandle());
      BITMAPINFO bmi={p->GetBMIH()};
      StretchDIBits(dc,0,0,rect.right,rect.bottom,0,0,p->GetWidth(),p->GetHeight(),p->GetBitmapSurface(),&bmi,DIB_RGB_COLORS,SRCCOPY);
      ReleaseDC(p->GetHandle(), dc);
    }
    gDrawNow = false;
  }
}
#endif

//////////////////////////////////////////////////////////////////////////////
// ProcessSession
//
// This function is called regularly by the main POVWIN code, primarily from
// its message loop, but also in one or two other places where it might have
// to wait a while before returning to loop processing. The optional delay
// parameter (defaults to 0 if not specified) is as described for the
// vfeSession::GetStatus() method.
//
// The purpose of ProcessSession is to query vfe for any outstanding events
// (it does this by checking the status flags), and if any are available,
// processing them. Typically this involves picking up messages and routing
// them to the message output window or status bar, but it also handles change
// of state (e.g. rendering to not rendering, and so forth, in which case it
// will call back to the main POVWIN code to advise it of such).
//////////////////////////////////////////////////////////////////////////////
bool ProcessSession (int delay)
{
  if (gSession == NULL)
    throw POV_EXCEPTION_STRING ("No session open");
  if (gSession->BackendFailed())
    return (false);

  vfeStatusFlags flags = gSession->GetStatus (true, delay);

  if (flags == 0)
    return (false) ;

  if ((flags & stFailed) != 0)
    ErrorOccurred = true;
  if ((flags & stRenderingAnimation) != 0)
    rendering_animation = true;
  if ((flags & stAnimationFrameCompleted) != 0)
    first_frame = false;
  if ((flags & (stStatusMessage | stAnimationStatus)) != 0)
  {
    vfeSession::StatusMessage msg(*gSession) ;
    while (gSession->GetNextStatusMessage (msg))
    {
      if (msg.m_Type == vfeSession::mGenericStatus)
      {
        strncpy (status_buffer, msg.m_Message.c_str(), sizeof (status_buffer) - 1);
        if (delay_next_status == 0)
          delay_next_status = msg.m_Delay;
      }
      else if (msg.m_Type == vfeSession::mAnimationStatus)
      {
        buffer_stream_message (mDivider, "-");
        SetStatusPanelItemText (IDC_STATUS_DATA_FRAME, "%d/%d (#%d)", msg.m_Frame, msg.m_TotalFrames, msg.m_FrameId);
        sprintf (status_buffer, "Rendering frame %d of %d (#%d)", msg.m_Frame, msg.m_TotalFrames, msg.m_FrameId);
      }
      else
      {
        // huh?
        assert (false);
      }
    }
  }
  if ((flags & (stStreamMessage | stErrorMessage | stWarningMessage)) != 0)
  {
    int line;
    int col;
    char str[32];
    std::string errormsg;
    std::string message;
    std::string filename;
    vfeSession::MessageType type;

    while (gSession->GetNextNonStatusMessage (type, message, filename, line, col))
    {
      switch (type)
      {
        case vfeSession::mDebug:
             buffer_stream_message (mDebug, message.c_str());
             break;

        case vfeSession::mInformation:
             buffer_stream_message (mAll, message.c_str());
             break;

        case vfeSession::mWarning:
        case vfeSession::mPossibleError:
             if (filename.empty() == false)
             {
               errormsg = "\"";
               errormsg += filename + "\"";
               if (line > 0)
               {
                 sprintf(str, "%u", line);
                 errormsg += " line ";
                 errormsg += str;
               }
               errormsg += ": " + message;
               buffer_stream_message (mWarning, errormsg.c_str());
             }
             else
               buffer_stream_message (mWarning, message.c_str());
             break;

        case vfeSession::mError:
             buffer_stream_message (mDivider, "-");
             if (ErrorMessage.empty())
             {
               ErrorMessage = message;
               ErrorLine = line;
               ErrorCol = col;
               ErrorFilename = filename;
             }
             if (filename.empty() == false)
             {
               errormsg = "\"";
               errormsg += filename + "\"";
               if (line > 0)
               {
                 sprintf(str, "%u", line);
                 errormsg += " line ";
                 errormsg += str;
               }
               errormsg += ": " + message;
               buffer_stream_message (mFatal, errormsg.c_str());
             }
             else
               buffer_stream_message (mFatal, message.c_str());
             break;

        case vfeSession::mDivider:
             buffer_stream_message (mDivider, "-");
             break;

        default:
             buffer_stream_message (mUnknown, message.c_str());
             break;
      }
    }
  }
  if ((flags & stBackendStateChanged) != 0)
  {
    State state = gSession->GetBackendState();
    SetCaption (NULL) ;
    SetStatusPanelItemText (IDC_STATUS_DATA_STATE, gSession->GetBackendStateName ()) ;
    if (state == kReady)
      update_menu_for_render (false) ;
    else if (state == kStopping)
      PVEnableMenuItem (CM_STOPRENDER, MF_GRAYED) ;
    else if (state == kPausedRendering || state == kPausedParsing) // TODO: handle pause request when shellout is active
      ShowIsPaused();
    PVEnableMenuItem (CM_RENDERSLEEP, gSession->IsPausable () ? MF_ENABLED : MF_GRAYED) ;
  }
  if ((flags & stRenderShutdown) != 0)
    render_stopped () ;
  if ((flags & stShutdown) != 0)
    return (true);
  return (false);
}

////////////////////////////////
// Called from the shellout code
////////////////////////////////
bool MinimizeShellouts(void)
{
  return system_noactive;
}

bool ShelloutsPermitted(void)
{
  return !no_shell_outs;
}

}
// end of namespace pov_frontend
