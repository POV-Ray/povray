//******************************************************************************
///
/// @file vfe/vfesession.cpp
///
/// This module contains the default C++ interface for render frontend.
///
/// @author Christopher J. Cason
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

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

#include "vfe.h"

#include <boost/bind.hpp>

#include "backend/povray.h"

static POVMSContext POVMS_Output_Context = nullptr;

namespace pov
{
  static volatile POVMSContext POVMS_GUI_Context = nullptr;
  static volatile POVMSAddress RenderThreadAddr = POVMSInvalidAddress ;
  static volatile POVMSAddress GUIThreadAddr = POVMSInvalidAddress ;
}
// end of namespace pov

namespace vfe
{

using std::min;
using std::max;
using std::string;

bool vfeSession::m_Initialized = false;
vfeSession *vfeSession::m_CurrentSessionTemporaryHack = nullptr;

vfeSession::vfeSession(int id)
{
  m_Id = id ;
  m_Frontend = nullptr;
  m_BackendThread = m_WorkerThread = nullptr;
  m_WorkerThreadShutdownRequest = false;
  m_HadCriticalError = false;
  m_CurrentSessionTemporaryHack = this;
  m_BackendThreadExited = m_WorkerThreadExited = false;
  m_BackendState = kUnknown;
  m_MessageCount = 0;
  m_LastError = vfeNoError;
  m_MaxStatusMessages = -1;
  m_MaxGenericMessages = -1;
  m_MaxConsoleMessages = -1;
  m_OptimizeForConsoleOutput = true;
  m_ConsoleWidth = 80;
  m_RequestFlag = rqNoRequest;
  m_RequestResult = 0;
  m_StartTime = 0;
  m_DisplayCreator = boost::bind(&vfe::vfeSession::DefaultDisplayCreator, this, _1, _2, _3, _4);
  Reset();
}

vfeSession::~vfeSession()
{
  // note: shouldn't delete m_Frontend here since it will cause a POVMS context error
  m_Initialized = false;
  m_CurrentSessionTemporaryHack = nullptr;
}

// Clears many of the internal state information held by VFE regarding
// a render. Typically called before starting a new render. Included in
// the clear are the failure/success status, cancel request state,
// error code, percent complete, number of pixels rendered, total pixels
// in render, current animation frame, and total animation frame count.
// If the optional Notify flag is set (defaults to true), a status event
// of type stClear is generated. Note that this method does not empty the
// message queues (see Reset() for that).
void vfeSession::Clear(bool Notify)
{
  m_Failed = false;
  m_Succeeded = false;
  m_HadErrorMessage = false;
  m_RenderCancelled = false;
  m_RenderCancelRequested = false;
  m_PauseWhenDone = false;
  m_RenderErrorCode = 0;
  m_PercentComplete = 0;
  m_PixelsRendered = 0;
  m_TotalPixels = 0;
  m_CurrentFrameId = 0;
  m_CurrentFrame = 0;
  m_TotalFrames = 0;
  if (Notify)
    NotifyEvent(stClear);
}

// Resets a session - this not only does the same work as vfeSession::Clear
// (without a stClear notification) but it also empties all the message
// queues, clears the input and output filenames, clears any animation
// status information, resets the status flags and clears the event mask.
// It will then generate a stReset notification.
void vfeSession::Reset()
{
  Clear(false);
  while (m_MessageQueue.empty() == false)
    m_MessageQueue.pop();
  while (m_StatusQueue.empty() == false)
    m_StatusQueue.pop();
  while (m_ConsoleQueue.empty() == false)
    m_ConsoleQueue.pop();
  m_OutputFilename.clear();
  m_InputFilename.clear();
  m_RenderingAnimation = false;
  m_RealTimeRaytracing = false;
  m_ClocklessAnimation = false;
  m_DontWriteImage = false;
  m_StatusFlags = stNone;
  m_EventMask = vfeStatusFlags(~stNone);
  NotifyEvent(stReset);
}

void vfeSession::SetFailed()
{
  if (m_Failed)
    return;
  m_Failed = true ;
  NotifyEvent(stFailed);
}

void vfeSession::SetSucceeded (bool ok)
{
  if (m_Succeeded == ok)
    return;
  m_Succeeded = ok;
  NotifyEvent(stSucceeded);
}

// Clears all messages from the status message queue.
void vfeSession::ClearStatusMessages()
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  while (m_StatusQueue.empty() == false)
    m_StatusQueue.pop();
  NotifyEvent(stStatusMessagesCleared);
}

void vfeSession::AdviseOutputFilename (const UCS2String& Filename)
{
  m_OutputFilename = Filename;
  NotifyEvent(stOutputFilenameKnown);
}

void vfeSession::AdviseFrameCompleted()
{
  NotifyEvent(stAnimationFrameCompleted);
}

void vfeSession::SetRenderingAnimation()
{
  m_RenderingAnimation = true ;
  NotifyEvent(stRenderingAnimation);
}

void vfeSession::AppendStreamMessage (MessageType type, const char *message, bool chompLF)
{
  // messages may have embedded LF's, in which case we split them at
  // the LF into seperate entries. however if chompLF is true, and the
  // message ends with an LF, we remove it (but only one). if chompLF
  // is false and the message ends with an LF, *or* there are LF's left
  // after we chomp one, we must append a blank line for each one.

  const char *begin = message ;
  const char *end = begin + strlen (message) - 1;
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  for (const char *s = begin ; s <= end ; s++)
  {
    if (*s == '\n')
    {
      if (chompLF && s == end)
        break ;
      m_ConsoleQueue.push (MessageBase (*this, type, string (begin, s - begin)));
      begin = s + 1 ;
    }
  }

  m_ConsoleQueue.push (MessageBase (*this, type, string (begin)));
  if (m_MaxConsoleMessages != -1)
    while (m_ConsoleQueue.size() > m_MaxConsoleMessages)
      m_ConsoleQueue.pop();
  NotifyEvent(stStreamMessage);
}

void vfeSession::AppendStreamMessage (MessageType type, const boost::format& fmt, bool chompLF)
{
  AppendStreamMessage(type, fmt.str().c_str(), chompLF);
}

void vfeSession::AppendErrorMessage (const string& Msg)
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);
  bool possibleError = Msg.find("Possible ") == 0 ;

  // for the purpose of setting m_HadErrorMessage, we don't consider a
  // 'possible parse error' to be an error (i.e. we look for fatal errors).
  if (possibleError == false)
    m_HadErrorMessage = true;
  m_MessageQueue.push (GenericMessage (*this, possibleError ? mPossibleError : mError, Msg));
  if (m_MaxGenericMessages != -1)
    while (m_MessageQueue.size() > m_MaxGenericMessages)
      m_MessageQueue.pop();
  NotifyEvent(stErrorMessage);
}

void vfeSession::AppendErrorMessage (const string& Msg, const UCS2String& File, int Line, int Col)
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);
  bool possibleError = Msg.find("Possible ") == 0 ;

  // for the purpose of setting m_HadErrorMessage, we don't consider a
  // 'possible parse error' to be an error (i.e. we look for fatal errors).
  if (possibleError == false)
    m_HadErrorMessage = true;
  m_MessageQueue.push (GenericMessage (*this, possibleError ? mPossibleError : mError, Msg, File, Line, Col));
  if (m_MaxGenericMessages != -1)
    while (m_MessageQueue.size() > m_MaxGenericMessages)
      m_MessageQueue.pop();
  NotifyEvent(stErrorMessage);
}

void vfeSession::AppendWarningMessage (const string& Msg)
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  m_MessageQueue.push (GenericMessage (*this, mWarning, Msg));
  if (m_MaxGenericMessages != -1)
    while (m_MessageQueue.size() > m_MaxGenericMessages)
      m_MessageQueue.pop();
  NotifyEvent(stWarningMessage);
}

void vfeSession::AppendWarningMessage (const string& Msg, const UCS2String& File, int Line, int Col)
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  m_MessageQueue.push (GenericMessage (*this, mWarning, Msg, File, Line, Col));
  if (m_MaxGenericMessages != -1)
    while (m_MessageQueue.size() > m_MaxGenericMessages)
      m_MessageQueue.pop();
  NotifyEvent(stWarningMessage);
}

void vfeSession::AppendStatusMessage (const string& Msg, int RecommendedPause)
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  m_StatusQueue.push (StatusMessage (*this, Msg, RecommendedPause));
  m_StatusLineMessage = Msg;
  if (m_MaxStatusMessages != -1)
    while (m_StatusQueue.size() > m_MaxStatusMessages)
      m_StatusQueue.pop();
  NotifyEvent(stStatusMessage);
}

void vfeSession::AppendStatusMessage (const boost::format& fmt, int RecommendedPause)
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  m_StatusQueue.push (StatusMessage (*this, fmt.str(), RecommendedPause));
  m_StatusLineMessage = fmt.str();
  if (m_MaxStatusMessages != -1)
    while (m_StatusQueue.size() > m_MaxStatusMessages)
      m_StatusQueue.pop();
  NotifyEvent(stStatusMessage);
}

void vfeSession::AppendAnimationStatus (int FrameId, int SubsetFrame, int SubsetTotal, const UCS2String& Filename)
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  m_CurrentFrameId = FrameId;
  m_CurrentFrame = SubsetFrame;
  m_TotalFrames = SubsetTotal;
  m_StatusQueue.push (StatusMessage (*this, Filename, SubsetFrame, SubsetTotal, FrameId));
  m_StatusLineMessage = (boost::format("Rendering frame %d of %d (#%d)") % SubsetFrame % SubsetTotal % FrameId).str();
  if (m_MaxStatusMessages != -1)
    while (m_StatusQueue.size() > m_MaxStatusMessages)
      m_StatusQueue.pop();
  NotifyEvent(stAnimationStatus);
}

////////////////////////////////////////////////////////////////////////////////////////
//
// Message fetching and management code
//
////////////////////////////////////////////////////////////////////////////////////////

// Fetches one message from either the generic, status, or console message
// queues, whichever has the earliest timestamp. Returns true if a message
// is fetched, otherwise false (meaning all the queues are empty). It expects
// two parameters - a reference to a MessageType, in which the type of the
// returned message is stored, and a std::string, which will be set to the
// actual message text. Note that linefeeds are not appended.
//
// This method is primarily useful for console-style displays as it discards
// any additional meta-information the message may have had, such as the
// line number of an error. (Note that this does not mean the line number
// cannot be in the message string; it may very well be).
bool vfeSession::GetNextCombinedMessage (MessageType &Type, string& Message)
{
  POV_LONG                    mqTime = 0x7fffffffffffffffLL ;
  POV_LONG                    sqTime = 0x7fffffffffffffffLL ;
  POV_LONG                    cqTime = 0x7fffffffffffffffLL ;
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  if (m_MessageQueue.empty() && m_StatusQueue.empty() && m_ConsoleQueue.empty())
    return (false);

  if (m_MessageQueue.empty() == false)
    mqTime = m_MessageQueue.front().m_TimeStamp ;
  if (m_StatusQueue.empty() == false)
    sqTime = m_StatusQueue.front().m_TimeStamp ;
  if (m_ConsoleQueue.empty() == false)
    cqTime = m_ConsoleQueue.front().m_TimeStamp ;
  POV_LONG oldest = min (min (sqTime, cqTime), mqTime);
  if (oldest == mqTime)
  {
    GenericMessage msg = m_MessageQueue.front();
    m_MessageQueue.pop();
    Type = msg.m_Type;
    Message = msg.m_Message;
    return (true);
  }
  if (oldest == sqTime)
  {
    StatusMessage msg = m_StatusQueue.front();
    m_StatusQueue.pop();
    Type = msg.m_Type;
    Message = msg.m_Message;
    return (true);
  }
  MessageBase msg = m_ConsoleQueue.front();
  m_ConsoleQueue.pop();
  Type = msg.m_Type;
  Message = msg.m_Message;
  return (true);
}

// Gets the next non-status message (meaning generic or console messages)
// from the aforementioned queues; whichever is the earliest. Returns false
// if there is no message to fetch, otherwise will set the message type,
// filename, line, and column parameters supplied. If the message retrieved
// did not contain this information, the relevant entry is either set to 0
// (line and column) or the empty string (filename). The filename parameter
// is a UCS2String.
bool vfeSession::GetNextNonStatusMessage (MessageType &Type, string& Message, UCS2String& File, int& Line, int& Col)
{
  POV_LONG                    mqTime = 0x7fffffffffffffffLL ;
  POV_LONG                    cqTime = 0x7fffffffffffffffLL ;
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  if (m_MessageQueue.empty() && m_ConsoleQueue.empty())
    return (false);

  if (m_MessageQueue.empty() == false)
    mqTime = m_MessageQueue.front().m_TimeStamp ;
  if (m_ConsoleQueue.empty() == false)
    cqTime = m_ConsoleQueue.front().m_TimeStamp ;
  POV_LONG oldest = min (cqTime, mqTime);

  // if equal we give preference to the console queue
  if (oldest == cqTime)
  {
    MessageBase msg = m_ConsoleQueue.front();
    m_ConsoleQueue.pop();
    Type = msg.m_Type;
    Message = msg.m_Message;
    File.clear();
    Line = 0;
    Col = 0;
  }
  else
  {
    GenericMessage msg = m_MessageQueue.front();
    m_MessageQueue.pop();
    Type = msg.m_Type;
    Message = msg.m_Message;
    File = msg.m_Filename;
    Line = msg.m_Line;
    Col = msg.m_Col;
  }
  return (true);
}

// Gets the next non-status message (meaning generic or console messages)
// from the aforementioned queues; whichever is the earliest. Returns false
// if there is no message to fetch, otherwise will set the message type,
// filename, line, and column parameters supplied. If the message retrieved
// did not contain this information, the relevant entry is either set to 0
// (line and column) or the empty string (filename). The filename parameter
// is a std::string.
bool vfeSession::GetNextNonStatusMessage (MessageType &Type, string& Message, string& File, int& Line, int& Col)
{
  UCS2String str;
  bool result = GetNextNonStatusMessage (Type, Message, str, Line, Col);
  if (result)
    File = UCS2toSysString(str);
  return result;
}

// Gets the next non-status message (meaning generic or console messages)
// from the aforementioned queues; whichever is the earliest. Returns false
// if there is no message to fetch, otherwise will set the message type
// and text content parameters supplied. Any additional meta-information
// that may have been contained in the message is discarded.
bool vfeSession::GetNextNonStatusMessage (MessageType &Type, string& Message)
{
  UCS2String str;
  int Line;
  int Col;
  return (GetNextNonStatusMessage (Type, Message, str, Line, Col));
}

// Returns false if there are no messages in the status message
// queue, otherwise removes the oldest status message from the
// queue and copies it into the StatusMessage reference supplied
// as a parameter, then returns true.
bool vfeSession::GetNextStatusMessage (StatusMessage& Message)
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  if (m_StatusQueue.empty())
    return (false);
  Message = m_StatusQueue.front();
  m_StatusQueue.pop();
  return (true) ;
}

// Returns false if there are no messages in the generic message
// queue, otherwise removes the oldest message from the queue and
// copies it into the GenericMessage reference supplied as a
// parameter, then returns true.
bool vfeSession::GetNextGenericMessage (GenericMessage& Message)
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  if (m_MessageQueue.empty())
    return (false);
  Message = m_MessageQueue.front();
  m_MessageQueue.pop();
  return (true) ;
}

// Returns false if there are no messages in the console message
// queue, otherwise removes the oldest message from the queue and
// copies it into the MessageBase reference supplied as a parameter,
// then returns true.
bool vfeSession::GetNextConsoleMessage (MessageBase& Message)
{
  std::lock_guard<std::mutex> lock(m_MessageMutex);

  if (m_ConsoleQueue.empty())
    return (false);
  Message = m_ConsoleQueue.front();
  m_ConsoleQueue.pop();
  return (true) ;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// Status related code
//
////////////////////////////////////////////////////////////////////////////////////////

bool vfeSession::ProcessFrontend (void)
{
  if (m_Frontend == nullptr)
    return (false);
  m_Frontend->Process () ;
  if (m_Frontend->GetState () == m_BackendState)
    return (false);
  StateChanged (m_Frontend->GetState ()) ;
  return (true);
}

void vfeSession::StateChanged (pov_frontend::State NewState)
{
  if (NewState == kReady || NewState == kStopping)
    if (m_BackendState > kReady)
      RenderStopped () ;
  if (NewState == kRendering)
    m_StartTime = GetTimestamp();
  m_BackendState = NewState ;
  NotifyEvent(stBackendStateChanged);
}

const char *vfeSession::GetBackendStateName (void) const
{
  switch (m_BackendState)
  {
    case kUnknown:                 return ("Unknown state") ;
    case kReady:                   return ("Idle") ;
    case kStarting:                return ("Starting") ;
    case kPreSceneShellout:        return ("Running pre-scene shellout");
    case kPreFrameShellout:        return ("Running pre-frame shellout");
    case kParsing:                 return ("Parsing") ;
    case kPausedParsing:           return ("Parse Paused") ;
    case kRendering:               return ("Rendering") ;
    case kPausedRendering:         return ("Render Paused") ;
    case kStopping:                return ("Stopping") ;
    case kStopped:                 return ("Stopped") ;
    case kPostFrameShellout:       return ("Running post-frame shellout");
    case kPostSceneShellout:       return ("Running post-scene shellout");
    case kPostShelloutPause:       return ("Paused after running shellout");
    case kFailed:                  return ("Failed") ;
    case kDone:                    return ("Done") ;
    default :                      return ("Invalid State") ;
  }
}

// Returns a copy of the shared pointer containing the current instance
// of a pov_frontend::Display-derived render preview instance, which may
// be `nullptr`.
std::shared_ptr<Display> vfeSession::GetDisplay() const
{
  if (m_Frontend == nullptr)
    return (std::shared_ptr<Display>());
  return m_Frontend->GetDisplay();
}

void vfeSession::BackendThreadNotify()
{
  m_BackendThreadExited = true;
  pov::RenderThreadAddr = POVMSInvalidAddress;
  if (m_Frontend != nullptr)
    m_Frontend->InvalidateBackend();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// worker (i.e. message handling) thread
//
////////////////////////////////////////////////////////////////////////////////////////

void vfeSession::WorkerThread()
{
  WorkerThreadStartup();

  if (POVMS_Init() == false)
    m_LastError = vfePOVMSInitFailed ;
  else if (POVMS_OpenContext (const_cast<void **>(&pov::POVMS_GUI_Context)) != kNoError)
    m_LastError = vfeOpenContextFailed ;
  else if (POVMS_GetContextAddress (pov::POVMS_GUI_Context, const_cast<void **>(&pov::GUIThreadAddr)) != kNoErr)
    m_LastError = vfeConnectFailed ;

  if (m_LastError != vfeNoError)
  {
    m_InitializeEvent.notify_all ();
    return;
  }

  m_BackendThread = povray_init (boost::bind(&vfeSession::BackendThreadNotify, this), const_cast<void **>(&pov::RenderThreadAddr)) ;
  POVMS_Output_Context = pov::POVMS_GUI_Context ;

  m_Console = std::shared_ptr<vfeConsole> (new vfeConsole(this, m_ConsoleWidth)) ;

  POVMS_Object obj ;
  m_Frontend = new VirtualFrontEnd (*this, POVMS_Output_Context, (POVMSAddress) pov::RenderThreadAddr, obj, nullptr, m_Console);
  if (m_Frontend == nullptr)
    throw POV_EXCEPTION_STRING ("Worker thread failed to create frontend");
  m_BackendState = m_Frontend->GetState();
  m_InitializeEvent.notify_all ();

  try
  {
    while (m_WorkerThreadShutdownRequest == false)
    {
      if (m_BackendThreadExited)
      {
        // the main thread (created by pov_init()) has exited. this is not good.
        // we could re-start it, but given it's only supposed to exit under critical
        // circumstances, we won't risk it and instead will alert the user. note that
        // if we are implementing a detached frontend and are not attached, this may
        // not be possible, but there's not a lot we can do about that.
        throw vfeCriticalError("Backend worker thread shut down prematurely: please re-start POV-Ray.");
      }
      try
      {
        while (POVMS_ProcessMessages (pov::POVMS_GUI_Context, true, true) == kFalseErr)
        {
          m_MessageCount++ ;
          ProcessFrontend () ;
          if (m_RenderCancelRequested == true || m_RequestFlag != rqNoRequest || m_WorkerThreadShutdownRequest == true)
            break ;
        }
        ProcessFrontend () ;
        if (m_RenderCancelRequested)
        {
          POV_LONG ts = GetTimestamp() + 2500;
          m_RenderCancelled = true;
          while (POVMS_ProcessMessages (pov::POVMS_GUI_Context, false, false) == kFalseErr)
          {
            ProcessFrontend () ;
            // don't allow it to take more than 2.5 seconds to process remaining messages
            // (this should not happen unless our thread priority is too low)
            if (GetTimestamp() > ts)
              break;
          }
          ProcessFrontend () ;
          if (ProcessCancelRender() == true)
            m_RenderCancelRequested = false;
        }
        if (m_RequestFlag != rqNoRequest)
        {
          int rq = m_RequestFlag;
          m_RequestFlag = rqNoRequest;
          switch (rq)
          {
            case rqPauseRequest:
              m_RequestResult = m_Frontend->Pause() ? 1 : 0;
              break;
            case rqResumeRequest:
              m_RequestResult = m_Frontend->Resume() ? 1 : 0;
              break;
          }
          m_RequestEvent.notify_all ();
        }
        std::this_thread::yield();
      }
      catch (pov_base::Exception& e)
      {
        SetFailed();
        if (StopRender(e.what()) == false)
          m_RenderCancelRequested = true;
      }
      catch (vfeException& e)
      {
        SetFailed();
        if (StopRender(e.what()) == false)
          m_RenderCancelRequested = true;
      }
      catch (std::exception& e)
      {
        SetFailed();
        if (StopRender(e.what()) == false)
          m_RenderCancelRequested = true;
      }
    }
  }
  catch (vfeCriticalError& e)
  {
    m_HadCriticalError = true;
    m_Failed = true;
    m_LastError = vfeCaughtCriticalError;
    NotifyCriticalError (e.what(), e.Filename().c_str(), e.Line()) ;
  }
  catch (vfeException&)
  {
    m_Failed = true;
    m_LastError = vfeCaughtException;
  }
  catch (std::exception&)
  {
    m_Failed = true;
    m_LastError = vfeCaughtException;
  }
  if (m_LastError == vfeCaughtException || m_LastError == vfeCaughtCriticalError)
  {
    while (POVMS_ProcessMessages (pov::POVMS_GUI_Context, false, false) == kFalseErr)
      ProcessFrontend () ;
    ProcessFrontend () ;
    ProcessCancelRender() ;
  }
  if (m_LastError == vfeCaughtException || m_LastError == vfeCaughtCriticalError)
    NotifyEvent(stCriticalError|stRenderShutdown);

  try
  {
    delete m_Frontend;
    m_Frontend = nullptr;
    WorkerThreadShutdown();
    m_Initialized = false;
    m_BackendThread = nullptr;
    povray_terminate();
    POVMS_CloseContext (POVMS_Output_Context);
    m_CurrentSessionTemporaryHack = nullptr;
    m_WorkerThreadExited = true;
    m_ShutdownEvent.notify_all ();
  }
  catch (std::runtime_error)
  {
    // typical cause of an exception here (at least if it is a POVException) is if the destruction
    // of m_Frontend takes too long. this can happen if there's a lot of memory blocks to free.
  }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// startup and management code
//
////////////////////////////////////////////////////////////////////////////////////////

// This static method is used to return the vfeSession associated with
// the calling thread (if any). In a full multi-session-enabled
// implementation of vfe, it would look up the session from a list of
// active sessions keyed with the value returned from GetThreadID().
// Currently as only one session is supported (this is enforced in the
// vfeSession::Initialize() method) it simply returns the value of a
// global variable.
vfeSession *vfeSession::GetSessionFromThreadID()
{
  if (m_CurrentSessionTemporaryHack == nullptr)
    throw vfeCriticalError("connection to backend has been terminated");
  return m_CurrentSessionTemporaryHack ;
}

vfeDisplay *vfeSession::DefaultDisplayCreator (unsigned int width, unsigned int height, vfeSession *session, bool visible)
{
  return new vfeDisplay (width, height, session, visible) ;
}

// If a VFE implementation has provided the address of a display creator
// function via vfeSession::SetDisplayCreator(), this method will call it
// with the width, height, gamma factor, and default visibility flag (false
// if not specified). Otherwise it will return `nullptr`. It is used when the
// core POV-Ray code requests that a render preview window be created.
// If a display instance is returned, it is expected to conform to the
// definition of the pov_frontend::Display class, but will typically be
// a platform-specific derivative of that.
vfeDisplay *vfeSession::CreateDisplay (unsigned int width, unsigned int height, bool visible)
{
  return m_DisplayCreator (width, height, this, visible);
}

// This method causes a shutdown of the vfeSession instance. Specifically
// it will set a flag asking the worker thread to exit, issue a
// stShutdown notification, and then wait for the worker thread to exit.
// The optional boolean parameter (default false) is used by the caller
// to indicate a forced shutdown of the worker thread is permissible
// (e.g. a kill if it doesn't shutdown gracefully within a reasonable
// period of time). It is not mandatory to implement the forced feature.
void vfeSession::Shutdown(bool forced)
{
  if ((m_Initialized == false) || (m_WorkerThread == nullptr))
    return ;

  // TODO: implement forced if possible (may need to be platform-specific)
  m_WorkerThreadShutdownRequest = true;
  NotifyEvent(stShutdown);
  m_WorkerThread->join();
  delete m_WorkerThread;
  m_WorkerThread = nullptr;
}

// Returns a string giving a short English description of the error code
// supplied as the only parameter. If no parameter is supplied, the default
// value (-1) instructs the method to instead use the value of m_LastError.
const char *vfeSession::GetErrorString(int code) const
{
  if (code == -1)
    code = m_LastError;

  switch (code)
  {
    case vfeNoError:
      return "No error";

    case vfeNoInputFile:
      return "No input file provided";

    case vfeUnsupportedOptionCombination:
      return "Unsupported combination of options";

    case vfeRenderBlockSizeTooSmall:
      return "Specified block size is too small";

    case vfeFailedToWriteINI:
      return "Failed to write output INI file";

    case vfeFailedToSetSource:
      return "Failed to set source file";

    case vfeFailedToParseINI:
      return "Failed to parse INI file";

    case vfeIORestrictionDeny:
      return "I/O restriction settings prohibit access to file";

    case vfeFailedToParseCommand:
      return "Failed to parse command-line option";

    case vfeFailedToSetMaxThreads:
      return "Failed to set number of render threads";

    case vfeFailedToSendRenderStart:
      return "Failed to send render start request";

    case vfeRenderOptionsNotSet:
      return "Render options not set, cannot start render";

    case vfeAlreadyStopping:
      return "Renderer is already stopping";

    case vfeNotRunning:
      return "Renderer is not running";

    case vfeInvalidParameter:
      return "Something broke but we're not sure what";

    case vfeSessionExists:
      return "Only one session at once permitted in this version";

    case vfePOVMSInitFailed:
      return "Failed to initialize local messaging subsystem";

    case vfeOpenContextFailed:
      return "Failed to open context with core messaging subsystem";

    case vfeConnectFailed:
      return "Failed to connect to core messaging subsystem";

    case vfeInitializeTimedOut:
      return "Timed out waiting for worker thread startup";

    case vfeFailedToInitObject:
      return "Failed to initialize internal options storage";

    case vfeCaughtException:
      return "Caught exception of unexpected type";

    case vfeCaughtCriticalError:
      return "Caught critical error";

    case vfeDisplayGammaTooSmall:
      return "Specified display gamma is too small";

    case vfeFileGammaTooSmall:
      return "Specified file gamma is too small";
  }

  return "Unknown error code";
}

// This method returns the current set of status flags as set by the worker
// thread and various other parts of vfeSession. The status flags returned
// are not affected by any event mask set (see vfeSession::SetEventMask()
// for more information), but whether or not a wait occurs.
//
// Two parameters are accepted (both optional). The first one, a bool,
// determines whether or not the internal copy of the status flags is
// cleared prior to returning. By default, this is the case. Note that
// flags identified by the stNoClear mask cannot be cleared by this method
// and will remain set until vfeSession::Reset() is called. See the definition
// of the status flags enum in vfesession.h for more information on flags.
// Note that even if the flags are cleared, an stClear notification does not
// get set.
//
// The second parameter - an int - sets the wait time. If not specified, it
// defaults to -1, which means wait indefinitely for an event. If set to 0,
// it effectively turns this method into a status polling function, since
// there will never be a wait. Otherwise the parameter specifies the maximum
// number of milliseconds to wait on the status notification event before
// returning the status flags (note that there is no indication of timeout).
//
// Typical usage of this method by a client that has its own status handling
// thread would be for the thread to call this method with an indefinite
// timeout, and depend on the stShutdown event and status notification to
// determine if its host application wants it to exit. stShutdown events can
// be generated by the host calling vfeSession::Shutdown() (it is safe for
// this to be called from any thread multiple times).
//
// A client that does not want to devote a thread to status processing
// would typically call this method in polling mode, with perhaps a short
// timeout, depending on the implementation. (The sample console example
// provided for VFE does this).
vfeStatusFlags vfeSession::GetStatus(bool Clear, int WaitTime)
{
  std::unique_lock<std::mutex> lock (m_SessionMutex);

  if ((m_StatusFlags & m_EventMask) != 0)
  {
    if (Clear == false)
      return m_StatusFlags;
    vfeStatusFlags m_OldFlags = m_StatusFlags;
    m_StatusFlags = vfeStatusFlags(m_StatusFlags & stNoClear);
    return m_OldFlags;
  }

  if (WaitTime > 0)
  {
    m_SessionEvent.wait_for (lock, std::chrono::milliseconds(WaitTime));
  }
  else if (WaitTime == -1)
    m_SessionEvent.wait (lock);

  if (Clear == false)
    return m_StatusFlags;
  vfeStatusFlags m_OldFlags = m_StatusFlags;
  m_StatusFlags = vfeStatusFlags(m_StatusFlags & stNoClear);
  return m_OldFlags;
}

// Returns true if the POV-Ray code is in a state where a pause request
// both makes sense and can be accepted. Typical use of this method is
// to call it each time a stBackendStateChanged event occurs, and to use
// the returned value to configure a client user interface (e.g. to gray
// out or enable a pause button or menu item).
//
// Throws a pov_base::Exception if the frontend does not exist (i.e.
// vfeSession::Initialize() hasn't been called or failed when called).
bool vfeSession::IsPausable() const
{
  CheckFrontend();
  return m_Frontend->IsPausable();
}

// Used similarly to vfeFrontend::IsPausable(), but returns the actual
// pause/not paused state.
//
// Throws a pov_base::Exception if the frontend does not exist (i.e.
// vfeSession::Initialize() hasn't been called or failed when called).
bool vfeSession::Paused() const
{
  CheckFrontend();
  return m_Frontend->Paused();
}

// Requests the POV-Ray backend code to pause whatever it is doing by entering
// an idle loop with calls to pov_base::Delay(). This method is synchronous
// and thus a delay of up to three seconds can occur waiting for the backend
// to respond. If a timeout occurs, m_LastError is set to vfeRequestTimedOut
// and this method returns false. Otherwise, m_LastError is set to vfeNoError
// and the return value is set according to whether the backend accepted the
// request (true) or rejected it (false). Typically it will only reject a
// pause request if it is not in a pausable state (e.g. it's not rendering).
//
// Implementation note: the pause request itself is actually proxied via the
// session's worker thread, since the POV-Ray messaging system requires that
// all such requests come from the thread that created the session (this is
// to allow internal dispatching of message replies to the right place). The
// net effect of this is that the worker thread won't be doing anything else
// (such as dispatching messages) once it starts processing this request.
//
// Throws a pov_base::Exception if the frontend does not exist (i.e.
// vfeSession::Initialize() hasn't been called or failed when called).
bool vfeSession::Pause()
{
  std::unique_lock<std::mutex> lock (m_RequestMutex);

  CheckFrontend();

  // we can't call pause directly since it will result in a thread context
  // error. pause must be called from the context of the worker thread.
  m_RequestFlag = rqPauseRequest;
  if (m_RequestEvent.wait_for(lock, std::chrono::seconds(3)) == std::cv_status::timeout)
  {
    m_RequestFlag = rqNoRequest;
    m_LastError = vfeRequestTimedOut;
    return (false);
  }
  m_LastError = vfeNoError;
  return (m_RequestResult != 0);
}

// The converse of vfeSession::Pause(). All the same considerations apply.
bool vfeSession::Resume()
{
  std::unique_lock<std::mutex> lock (m_RequestMutex);

  CheckFrontend();

  // we can't call resume directly since it will result in a thread context
  // error. it must be called from the context of the worker thread.
  m_RequestFlag = rqResumeRequest;
  if (m_RequestEvent.wait_for(lock, std::chrono::seconds(3)) == std::cv_status::timeout)
  {
    m_RequestFlag = rqNoRequest;
    m_LastError = vfeRequestTimedOut;
    return (false);
  }
  m_LastError = vfeNoError;
  return (m_RequestResult != 0);
}

// Internal method used to generate an event notification.
void vfeSession::NotifyEvent(vfeStatusFlags Status)
{
  std::lock_guard<std::mutex> lock (m_SessionMutex);

  m_StatusFlags = vfeStatusFlags(m_StatusFlags | Status);
  if ((m_StatusFlags & m_EventMask) != 0)
    m_SessionEvent.notify_one();
}

// Used to set up a session with a POV backend. Accepts two
// parameters - a destination (vfeDestInfo) and authorization
// (vfeAuthInfo), both pointers. Currently these must be `nullptr`.
//
// Intialize() will call the Reset() method, and then create
// the session's worker thread, at which time it will wait on
// an event for the worker thread to signal that it has completed
// its own initialization. By default the worker thread setup is
// considered to have failed if it has not signalled within three
// seconds (elapsed time). However to aid debugging this is extended
// to 123 seconds if _DEBUG is defined.
//
// If the startup is successful, this method returns vfeNoError after
// issuing a stBackendStateChanged status notification. Otherwise it
// will either set m_LastError to vfeInitializeTimeout and return that
// same value, or it will retrieve the error code set by the worker
// thread, wait for the worker thread to exit, and return that code.
//
// The worker thread itself is responsible for creating the actual
// connection with the backend code.
int vfeSession::Initialize(vfeDestInfo *Dest, vfeAuthInfo *Auth)
{
  std::unique_lock<std::mutex> lock (m_InitializeMutex);

  // params must be `nullptr` in this version
  if ((Dest != nullptr) || (Auth != nullptr))
    return (m_LastError = vfeInvalidParameter);

  // only one session at once permitted in this version
  if (m_Initialized == true)
    return (m_LastError = vfeSessionExists);

  Reset();
  m_WorkerThreadExited = false;
  m_HadCriticalError = false;
  m_WorkerThreadShutdownRequest = false;
  m_Initialized = true ;
  m_StatusFlags = stNone;
  m_EventMask = vfeStatusFlags(~stNone);
  m_MessageCount = 0;
  m_LastError = vfeNoError;

  auto timeout = 3;
#ifdef _DEBUG
  timeout += 120;
#endif
  m_WorkerThread = new std::thread(vfeSessionWorker(*this));

  // TODO FIXME: see thread <47ca756c$1@news.povray.org>
  if ((m_BackendState == kUnknown) && (m_InitializeEvent.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout))
  {
    m_WorkerThreadShutdownRequest = true ;
    m_Initialized = false ;
    return (m_LastError = vfeInitializeTimedOut);
  }
  if (m_LastError != vfeNoError)
  {
    m_WorkerThread->join();
    delete m_WorkerThread;
    m_WorkerThread = nullptr;
    return (m_LastError);
  }

  NotifyEvent (stBackendStateChanged);
  AppendStreamMessage (mDivider, "");
  return (vfeNoError) ;
}

}
// end of namespace vfe
