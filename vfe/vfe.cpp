//******************************************************************************
///
/// @file vfe/vfe.cpp
///
/// This module contains the C++ implementation for the virtual frontend.
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

#include <cstdarg>
#include <cstdio>

#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "base/filesystem.h"
#include "base/povassert.h"
#include "base/textstream.h"

#include "frontend/animationprocessing.h"
#include "frontend/imageprocessing.h"

// this must be the last file included
#include "base/povdebug.h"

namespace vfe
{

using namespace pov_base;
using namespace pov_frontend;
using boost::format;
using std::shared_ptr;
using std::string;

static int Allow_File_Read(const UCS2 *Filename, const unsigned int FileType);
static int Allow_File_Write(const UCS2 *Filename, const unsigned int FileType);
static FILE *vfeFOpen(const UCS2String& name, const char *mode);

////////////////////////////////////////////////////////////////////////////////////////
//
// class POVMSMessageDetails
//
////////////////////////////////////////////////////////////////////////////////////////

class POVMSMessageDetails
{
  public:
    POVMSMessageDetails (POVMS_Object &Obj);
    virtual ~POVMSMessageDetails () {} ;
    string GetContext (int NumLines) ;

  protected:
    string File ;
    UCS2String UCS2File;
    string URL ;
    string Message ;
    POVMSInt Line ;
    POVMSInt Col ;
    POVMSLong Offset ;
} ;

POVMSMessageDetails::POVMSMessageDetails (POVMS_Object& Obj)
{
  char                  buffer [2048] = "";
  UCS2                  ubuffer [2048];
  POVMSInt              l = sizeof (ubuffer);
  POVMSLong             ll ;
  POVMSObject           msgobj (Obj());
  POVMSObjectPtr        msg = &msgobj;

  Line = Col = 0 ;
  Offset = -1 ;
  ubuffer[0] = '\0';

  if (POVMSUtil_GetUCS2String (msg, kPOVAttrib_FileName, ubuffer, &l) == kNoErr)
  {
    UCS2File = ubuffer ;
    File = UCS2toSysString(UCS2File);
  }
  if (POVMSUtil_GetLong (msg, kPOVAttrib_Line, &ll) == kNoErr)
    Line = POVMSInt(ll) ;
  if (POVMSUtil_GetLong (msg, kPOVAttrib_Column, &ll) == kNoErr)
    Col = POVMSInt(ll) ;
  if(POVMSUtil_GetLong(msg, kPOVAttrib_FilePosition, &ll) == kNoErr)
    Offset = ll ;
  l = sizeof (buffer) ;
  if (POVMSUtil_GetString (msg, kPOVAttrib_EnglishText, buffer, &l) == kNoErr)
    Message = buffer ;

  POVMSObject_Delete(msg);
}

string POVMSMessageDetails::GetContext (int NumLines)
{
  return ("") ;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// class ParseErrorDetails, ParseWarningDetails
//
////////////////////////////////////////////////////////////////////////////////////////

class ParseWarningDetails : public POVMSMessageDetails
{
  public:
    ParseWarningDetails (POVMS_Object &Obj) : POVMSMessageDetails (Obj) {} ;
    virtual ~ParseWarningDetails () override {} ;

  public:
    using POVMSMessageDetails::File ;
    using POVMSMessageDetails::UCS2File ;
    using POVMSMessageDetails::Message ;
    using POVMSMessageDetails::Line ;
    using POVMSMessageDetails::Col ;
    using POVMSMessageDetails::Offset ;
} ;

class ParseErrorDetails : public POVMSMessageDetails
{
  public:
    ParseErrorDetails (POVMS_Object &Obj) : POVMSMessageDetails (Obj) {} ;
    virtual ~ParseErrorDetails () override {} ;

  public:
    using POVMSMessageDetails::File ;
    using POVMSMessageDetails::UCS2File ;
    using POVMSMessageDetails::Message ;
    using POVMSMessageDetails::Line ;
    using POVMSMessageDetails::Col ;
    using POVMSMessageDetails::Offset ;
} ;

////////////////////////////////////////////////////////////////////////////////////////
//
// class vfeConsole
//
////////////////////////////////////////////////////////////////////////////////////////

vfeConsole::vfeConsole(vfeSession *session, int width) : Console(width == -1 ? session->GetConsoleWidth() : width)
{
  m_Session = session;
  Initialise();
}

vfeConsole::~vfeConsole()
{
}

void vfeConsole::Initialise()
{
  rawBuffer [0] = '\0' ;
}

void vfeConsole::BufferOutput(const char *str, unsigned int chars, vfeSession::MessageType mType)
{
  char                  *s ;

  // HACK FIXME - this is to prevent duplicate messages
  if (m_Session->HadErrorMessage() && strncmp (str, "Fatal error in parser: ", 23) == 0)
    return ;

  if (str [0] == '\n' && str [1] == '\0')
  {
    m_Session->AppendStreamMessage (mType, rawBuffer) ;
    rawBuffer [0] = '\0' ;
    return ;
  }

  size_t sLen = chars ;
  if (sLen == 0)
    sLen = strlen (str) ;
  size_t bLen = strlen (rawBuffer) ;
  if (sLen > sizeof (rawBuffer) - bLen - 1)
    sLen = sizeof (rawBuffer) - bLen - 1 ;
  strncat (rawBuffer, str, sLen) ;
  if ((s = strrchr (rawBuffer, '\n')) != nullptr)
  {
    *s++ = '\0' ;
    m_Session->AppendStreamMessage (mType, rawBuffer) ;
    strcpy (rawBuffer, s) ;
  }
}

void vfeConsole::Output(const char *str, vfeSession::MessageType mType)
{
  BufferOutput (str, (unsigned int) strlen (str), mType) ;
  BufferOutput ("\n", 1, mType) ;
}

void vfeConsole::Output(const string& str, vfeSession::MessageType mType)
{
  Output (str.c_str(), mType) ;
}

void vfeConsole::Output(const string& str)
{
  Output (str.c_str()) ;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// class vfePlatformBase
//
////////////////////////////////////////////////////////////////////////////////////////

vfePlatformBase::vfePlatformBase(vfeSession& session) : m_Session(&session), PlatformBase()
{
}

vfePlatformBase::~vfePlatformBase()
{
}

UCS2String vfePlatformBase::GetTemporaryPath(void)
{
  return m_Session->GetTemporaryPath();
}

UCS2String vfePlatformBase::CreateTemporaryFile(void)
{
  return m_Session->CreateTemporaryFile();
}

void vfePlatformBase::DeleteTemporaryFile(const UCS2String& filename)
{
  m_Session->DeleteTemporaryFile(filename);
}

bool vfePlatformBase::ReadFileFromURL(OStream *file, const UCS2String& url, const UCS2String& referrer)
{
  return false;
}

FILE* vfePlatformBase::OpenLocalFile (const UCS2String& name, const char *mode)
{
  return vfeFOpen (name, mode);
}

bool vfePlatformBase::AllowLocalFileAccess (const UCS2String& name, const unsigned int fileType, bool write)
{
    if (write)
        return Allow_File_Write (name.c_str(), fileType);
    else
        return Allow_File_Read (name.c_str(), fileType);
}

////////////////////////////////////////////////////////////////////////////////////////
//
// class vfeParserMessageHandler
//
////////////////////////////////////////////////////////////////////////////////////////

vfeParserMessageHandler::vfeParserMessageHandler() : ParserMessageHandler()
{
  m_Session = vfeSession::GetSessionFromThreadID();
}

vfeParserMessageHandler::~vfeParserMessageHandler()
{
}

void vfeParserMessageHandler::Options(Console *Con, POVMS_Object& Obj, bool conout)
{
  if (Obj.TryGetBool (kPOVAttrib_OutputAlpha, false))
    m_Session->SetUsingAlpha();
  if (Obj.TryGetBool (kPOVAttrib_ClocklessAnimation, false))
    m_Session->SetClocklessAnimation();
  if (Obj.TryGetBool (kPOVAttrib_RealTimeRaytracing, false))
    m_Session->SetRealTimeRaytracing();
  ParserMessageHandler::Options (Con, Obj, conout) ;
}

void vfeParserMessageHandler::Statistics(Console *Con, POVMS_Object& Obj, bool conout)
{
  ParserMessageHandler::Statistics (Con, Obj, conout) ;
}

void vfeParserMessageHandler::Progress(Console *Con, POVMS_Object& Obj, bool verbose)
{
  switch(Obj.GetType(kPOVMSObjectClassID))
  {
    case kPOVObjectClass_ParserProgress:
    {
      m_Session->AppendStatusMessage (format ("Parsing %uK tokens") % (Obj.GetLong (kPOVAttrib_CurrentTokenCount) / 1000));
      break;
    }
    case kPOVObjectClass_BoundingProgress:
    {
      m_Session->AppendStatusMessage (format ("Constructed %uK BSP nodes") % (Obj.GetLong (kPOVAttrib_CurrentNodeCount) / 1000));
      break;
    }
  }
}

void vfeParserMessageHandler::Warning(Console *Con, POVMS_Object& Obj, bool conout)
{
  ParseWarningDetails   d (Obj) ;

  if (d.Message == "")
    return ;

  // as we provide special treatment for warning messages here if we're not
  // optimized for console output, we don't duplicate them to the console
  // regardless of whether or not conout is set.
  if (m_Session->m_OptimizeForConsoleOutput == false)
    m_Session->AppendWarningMessage (d.Message, d.UCS2File, d.Line, d.Col) ;

  if (!d.File.empty() && (d.Line > 0))
  {
    format f = format ("File '%s' line %d: %s") % d.File % d.Line % d.Message ;
    if (m_Session->m_OptimizeForConsoleOutput == false)
      m_Session->AppendStatusMessage (f) ;
    else if (conout)
      Con->puts (f.str().c_str()) ;
  }
  else
  {
    if (m_Session->m_OptimizeForConsoleOutput == false)
      m_Session->AppendStatusMessage (d.Message) ;
    else if (conout)
      Con->puts (d.Message.c_str()) ;
  }
}

void vfeParserMessageHandler::Error(Console *Con, POVMS_Object& Obj, bool conout)
{
  ParseErrorDetails     d (Obj) ;

  if (d.Message == "" && (d.File == "" || d.Line <= 0))
    return ;

  // as we provide special treatment for parser errors here if we're not
  // optimized for console output, we don't duplicate them to the console
  // regardless of whether or not conout is set.
  if (m_Session->m_OptimizeForConsoleOutput == false)
    m_Session->AppendErrorMessage (d.Message, d.UCS2File, d.Line, d.Col) ;

  if (!d.Message.empty())
  {
    if (!d.File.empty() && (d.Line > 0))
    {
      format f = format ("File '%s' line %d: %s") % d.File % d.Line % d.Message ;
      if (m_Session->m_OptimizeForConsoleOutput == false)
        m_Session->AppendStatusMessage (f) ;
      else if (conout)
        Con->puts (f.str().c_str()) ;
    }
    else
    {
      if (m_Session->m_OptimizeForConsoleOutput == false)
        m_Session->AppendStatusMessage (d.Message) ;
      if (conout)
        if (m_Session->m_OptimizeForConsoleOutput == true)
          Con->puts (d.Message.c_str()) ;
    }
  }
  else
  {
    format f = format ("Parse error in file '%s' at line %d") % d.File % d.Line ;
    if (m_Session->m_OptimizeForConsoleOutput == false)
      m_Session->AppendStatusMessage (f) ;
    if (conout)
      if (m_Session->m_OptimizeForConsoleOutput == true)
        Con->puts (f.str().c_str()) ;
  }
}

void vfeParserMessageHandler::FatalError(Console *Con, POVMS_Object& Obj, bool conout)
{
  m_Session->SetFailed();
  Error (Con, Obj, conout) ;
}

void vfeParserMessageHandler::DebugInfo(Console *Con, POVMS_Object& Obj, bool conout)
{
  string str(Obj.GetString(kPOVAttrib_EnglishText));
  if (m_Session->m_OptimizeForConsoleOutput == true)
  {
    if (conout)
        Con->puts (str.c_str()) ;
  }
  else
    m_Session->AppendStreamMessage (vfeSession::mDebug, str.c_str()) ;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// class vfeRenderMessageHandler
//
////////////////////////////////////////////////////////////////////////////////////////

vfeRenderMessageHandler::vfeRenderMessageHandler() : RenderMessageHandler()
{
  m_Session = vfeSession::GetSessionFromThreadID();
}

vfeRenderMessageHandler::~vfeRenderMessageHandler()
{
}

void vfeRenderMessageHandler::Options(Console *Con, POVMS_Object& Obj, bool conout)
{
  RenderMessageHandler::Options (Con, Obj, conout) ;
}

void vfeRenderMessageHandler::Statistics(Console *Con, POVMS_Object& Obj, bool conout)
{
  RenderMessageHandler::Statistics (Con, Obj, conout) ;
}

void vfeRenderMessageHandler::Progress(Console *Con, POVMS_Object& Obj, bool verbose)
{
  switch (Obj.GetType(kPOVMSObjectClassID))
  {
    case kPOVObjectClass_PhotonProgress:
    {
      int cpc (Obj.GetInt (kPOVAttrib_CurrentPhotonCount)) ;
      m_Session->AppendStatusMessage (format ("Photon count %u") % cpc, 250) ;
      break;
    }
    case kPOVObjectClass_RadiosityProgress:
    {
      int pc (Obj.GetInt (kPOVAttrib_Pixels)) ;
      int cc (Obj.GetInt (kPOVAttrib_PixelsCompleted)) ;
      m_Session->SetPixelsRendered(cc, pc);
      int percent = pc > 0 ? int ((cc * 100.0) / pc) : 0 ;
      m_Session->SetPercentComplete (percent);
      m_Session->AppendStatusMessage (format ("Performing radiosity pretrace: %d of %d pixels (%d%%)") % cc % pc % percent, 250) ;
      break;
    }
    case kPOVObjectClass_RenderProgress:
    {
      int pc (Obj.GetInt (kPOVAttrib_Pixels)) ;
      int cc (Obj.GetInt (kPOVAttrib_PixelsCompleted)) ;

      if (m_Session->GetRealTimeRaytracing() == false)
      {
        m_Session->SetPixelsRendered(cc, pc);
        int percent = pc > 0 ? (int) ((cc * 100.0) / pc) : 0 ;
        m_Session->SetPercentComplete (percent);
        if (verbose == true || m_Session->m_OptimizeForConsoleOutput == false)
          m_Session->AppendStatusMessage (format ("Rendered %u of %u pixels (%d%%)") % cc % pc % percent, 250) ;
      }
      else
      {
        m_Session->SetPixelsRendered(cc % pc, pc);
        float elapsed = m_Session->GetElapsedTime() / 1000.0f;
        float frames = (float) cc / pc;
        float fps = frames / elapsed;
        if (verbose == true || m_Session->m_OptimizeForConsoleOutput == false)
          m_Session->AppendStatusMessage (format ("Rendered %g frames over %g seconds (%g FPS)") % frames % elapsed % fps, 250) ;
      }
      break;
    }
  }
}

void vfeRenderMessageHandler::Warning(Console *Con, POVMS_Object& Obj, bool conout)
{
  RenderMessageHandler::Warning (Con, Obj, conout) ;
}

void vfeRenderMessageHandler::Error(Console *Con, POVMS_Object& Obj, bool conout)
{
  m_Session->SetFailed();
  RenderMessageHandler::Error (Con, Obj, conout) ;
}

void vfeRenderMessageHandler::FatalError(Console *Con, POVMS_Object& Obj, bool conout)
{
  m_Session->SetFailed();
  RenderMessageHandler::FatalError (Con, Obj, conout) ;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// class vfeProcessRenderOptions
//
////////////////////////////////////////////////////////////////////////////////////////

vfeProcessRenderOptions::vfeProcessRenderOptions(vfeSession *Session) : ProcessRenderOptions(), m_Session(Session)
{
}

vfeProcessRenderOptions::~vfeProcessRenderOptions()
{
}

int vfeProcessRenderOptions::ReadSpecialOptionHandler(INI_Parser_Table *Table, char *Param, POVMSObjectPtr Obj)
{
  return ProcessRenderOptions::ReadSpecialOptionHandler (Table, Param, Obj);
}

int vfeProcessRenderOptions::ReadSpecialSwitchHandler(Cmd_Parser_Table *Table, char *Param, POVMSObjectPtr Obj, bool On)
{
  return ProcessRenderOptions::ReadSpecialSwitchHandler (Table, Param, Obj, On);
}

int vfeProcessRenderOptions::WriteSpecialOptionHandler(INI_Parser_Table *Table, POVMSObjectPtr Obj, OTextStream *S)
{
  return ProcessRenderOptions::WriteSpecialOptionHandler (Table, Obj, S);
}

int vfeProcessRenderOptions::ProcessUnknownString(char *String, POVMSObjectPtr Obj)
{
  return ProcessRenderOptions::ProcessUnknownString (String, Obj);
}

ITextStream *vfeProcessRenderOptions::OpenFileForRead(const char *Name, POVMSObjectPtr Obj)
{
  return (ProcessRenderOptions::OpenFileForRead (Name, Obj)) ;
}

OTextStream *vfeProcessRenderOptions::OpenFileForWrite(const char *Name, POVMSObjectPtr Obj)
{
  return (ProcessRenderOptions::OpenFileForWrite (Name, Obj)) ;
}

void vfeProcessRenderOptions::ParseError(const char *format, ...)
{
  char str[1024];
  va_list marker;

  va_start(marker, format);
  std::vsnprintf(str, sizeof(str), format, marker);
  va_end(marker);

  m_Session->AppendStatusMessage (str);
  m_Session->AppendErrorMessage (str) ;
  m_Session->SetFailed();
}

void vfeProcessRenderOptions::ParseErrorAt(ITextStream *file, const char *format, ...)
{
  char str[1024];
  va_list marker;

  va_start(marker, format);
  std::vsnprintf(str, sizeof(str), format, marker);
  va_end(marker);

  m_Session->AppendStatusMessage (str);
  m_Session->AppendErrorMessage (str, file->name(), file->line(), 0) ;
  m_Session->SetFailed();
}

void vfeProcessRenderOptions::WriteError(const char *format, ...)
{
  char str[1024];
  va_list marker;

  va_start(marker, format);
  std::vsnprintf(str, sizeof(str), format, marker);
  va_end(marker);

  m_Session->AppendStatusMessage (str);
  m_Session->AppendErrorMessage (str) ;
  m_Session->SetFailed();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// class VirtualFrontEnd
//
////////////////////////////////////////////////////////////////////////////////////////

VirtualFrontEnd::VirtualFrontEnd(vfeSession& session, POVMSContext ctx, POVMSAddress addr, POVMS_Object& msg, POVMS_Object *result, shared_ptr<Console>& console) :
  m_Session(&session), m_PlatformBase(session), renderFrontend (ctx)
{
  backendAddress = addr ;
  state = kReady ;
  m_PostPauseState = kReady;
  consoleResult = nullptr;
  displayResult = nullptr;
  m_PauseRequested = m_PausedAfterFrame = false;
  renderFrontend.ConnectToBackend(backendAddress, msg, result, console);
}

VirtualFrontEnd::~VirtualFrontEnd()
{
  // file-backed images may require a reference to PlatformBase to delete temporary files
  // we need to explicitly delete it here since otherwise PlatformBase will have been destroyed
  // before the shared_ptr does its cleanup
  imageProcessing.reset();
  if (backendAddress != POVMSInvalidAddress)
    renderFrontend.DisconnectFromBackend(backendAddress);
  state = kUnknown;
}

bool VirtualFrontEnd::Start(POVMS_Object& opts)
{
  if (state != kReady)
    return false;

  m_Session->Clear();
  animationProcessing.reset() ;
  m_PauseRequested = m_PausedAfterFrame = false;
  m_PostPauseState = kReady;

  Path ip (m_Session->GetInputFilename());
  shelloutProcessing.reset(m_Session->CreateShelloutProcessing(opts, UCS2toSysString(ip.GetFile()), m_Session->GetRenderWidth(), m_Session->GetRenderHeight())) ;
  shelloutProcessing->SetCancelMessage("Render halted because the %1% shell-out ('%6%') requested POV-Ray to %5%.");
  shelloutProcessing->SetSkipMessage("The %1% shell-out ('%3%') requested POV-Ray to %2%.");

  POVMS_List declares;
  if(opts.Exist(kPOVAttrib_Declare) == true)
    opts.Get(kPOVAttrib_Declare, declares);

  POVMS_Object image_width(kPOVMSType_WildCard);
  image_width.SetString(kPOVAttrib_Identifier, "image_width");
  image_width.SetFloat(kPOVAttrib_Value, opts.TryGetInt(kPOVAttrib_Width, 160));
  declares.Append(image_width);

  POVMS_Object image_height(kPOVMSType_WildCard);
  image_height.SetString(kPOVAttrib_Identifier, "image_height");
  image_height.SetFloat(kPOVAttrib_Value, opts.TryGetInt(kPOVAttrib_Height, 120));
  declares.Append(image_height);

  POVMS_Object input_file_name(kPOVMSType_WildCard);
  input_file_name.SetString(kPOVAttrib_Identifier, "input_file_name");
  input_file_name.SetString(kPOVAttrib_Value, UCS2toSysString(ip.GetFile()).c_str());
  declares.Append(input_file_name);

  int initialFrame = opts.TryGetInt (kPOVAttrib_InitialFrame, 0) ;
  int finalFrame = opts.TryGetInt (kPOVAttrib_FinalFrame, 0) ;
  if ((initialFrame == 0 && finalFrame == 0) || (initialFrame == 1 && finalFrame == 1))
  {
    POVMS_Object clock_delta(kPOVMSType_WildCard);
    clock_delta.SetString(kPOVAttrib_Identifier, "clock_delta");
    clock_delta.SetFloat(kPOVAttrib_Value, 0.0f);
    declares.Append(clock_delta);

    POVMS_Object final_clock(kPOVMSType_WildCard);
    final_clock.SetString(kPOVAttrib_Identifier, "final_clock");
    final_clock.SetFloat(kPOVAttrib_Value, 0.0f);
    declares.Append(final_clock);

    POVMS_Object final_frame(kPOVMSType_WildCard);
    final_frame.SetString(kPOVAttrib_Identifier, "final_frame");
    final_frame.SetFloat(kPOVAttrib_Value, 0.0f);
    declares.Append(final_frame);

    POVMS_Object frame_number(kPOVMSType_WildCard);
    frame_number.SetString(kPOVAttrib_Identifier, "frame_number");
    frame_number.SetFloat(kPOVAttrib_Value, 0.0f);
    declares.Append(frame_number);

    POVMS_Object initial_clock(kPOVMSType_WildCard);
    initial_clock.SetString(kPOVAttrib_Identifier, "initial_clock");
    initial_clock.SetFloat(kPOVAttrib_Value, 0.0f);
    declares.Append(initial_clock);

    POVMS_Object initial_frame(kPOVMSType_WildCard);
    initial_frame.SetString(kPOVAttrib_Identifier, "initial_frame");
    initial_frame.SetFloat(kPOVAttrib_Value, 0.0f);
    declares.Append(initial_frame);

    opts.Set(kPOVAttrib_Declare, declares);
    // optimization: reset imageProcessing now even though the following assign
    // will free the old pointer (if it exists). this can potentially free a
    // significant amount of memory and may in some circumstances prevent the
    // image allocation from failing. TODO: it may be useful to check whether
    // we can re-use an old imageProcessing instance (e.g. if image options are
    // the same).
    imageProcessing.reset();

    // TODO: update ImageProcessing with the means of accepting and caching
    // blocks of pixels as opposed to individual ones, with a back-end that
    // can serialize completed rows to the final image output file.
    options = opts;

    if (m_Session->OutputToFileSet())
    {
      imageProcessing = shared_ptr<ImageProcessing> (new ImageProcessing (opts));
      UCS2String filename = imageProcessing->GetOutputFilename (opts, 0, 0);
      options.SetUCS2String (kPOVAttrib_OutputFile, filename.c_str());

      if ((imageProcessing->OutputIsStdout() || imageProcessing->OutputIsStderr()) && m_Session->ImageOutputToStdoutSupported() == false)
        throw POV_EXCEPTION(kCannotOpenFileErr, "Image output to STDOUT/STDERR not supported on this platform");

      // test access permission now to avoid surprise later after waiting for
      // the render to complete.
      if (imageProcessing->OutputIsStdout() == false && imageProcessing->OutputIsStderr() == false && m_Session->TestAccessAllowed(filename, true) == false)
      {
        string str ("IO Restrictions prohibit write access to '") ;
        str += UCS2toSysString(filename);
        str += "'";
        throw POV_EXCEPTION(kCannotOpenFileErr, str);
      }
      shelloutProcessing->SetOutputFile(UCS2toSysString(filename));
      m_Session->AdviseOutputFilename (filename);
    }
  }
  else
  {
    // the output filename is set in Process()
    m_Session->SetRenderingAnimation();
    opts.Set(kPOVAttrib_Declare, declares);
    imageProcessing.reset();
    if (m_Session->OutputToFileSet())
      imageProcessing = shared_ptr<ImageProcessing> (new ImageProcessing (opts)) ;
    animationProcessing = shared_ptr<AnimationProcessing> (new AnimationProcessing (opts)) ;
    options = animationProcessing->GetFrameRenderOptions () ;
  }

  state = kStarting;

  return true;
}

bool VirtualFrontEnd::Stop()
{
  bool result = false;

  try
  {
    switch(state)
    {
      case kStarting:
        state = kStopped;
        m_Session->SetFailed();
        result = true;
        break;

      case kPreSceneShellout:
      case kPreFrameShellout:
      case kPostFrameShellout:
      case kPostSceneShellout:
        if (shelloutProcessing->ShelloutRunning())
          if (!shelloutProcessing->KillShellouts(2, false) && !shelloutProcessing->KillShellouts(1, true))
            m_Session->AppendErrorAndStatusMessage("Failed to terminate currently-running shellout process") ;
        if (state == kPostSceneShellout)
        {
          state = kDone;
          return true;
        }
        m_Session->SetFailed();
        state = kStopped;
        result = true;
        break;

      case kPostShelloutPause:
        m_Session->SetFailed();
        state = kStopping;
        result = true;
        break;

      case kParsing:
      case kPausedParsing:
        // the parser could be already in a finished state, even if it accepted a pause earlier
        try { renderFrontend.StopParser(sceneId); } catch (pov_base::Exception&) { }
        m_Session->SetFailed();
        state = kStopping;
        result = true;
        break;

      case kRendering:
      case kPausedRendering:
        m_Session->SetFailed();
        if (m_PausedAfterFrame == true)
        {
          m_PausedAfterFrame = false;
          state = kStopped;
        }
        else
        {
          // the renderer could be already in a finished state, even if it accepted a pause earlier
          try { renderFrontend.StopRender(viewId); } catch (pov_base::Exception&) { }
          state = kStopping;
        }
        result = true;
        break;

      default:
        // Do nothing special.
        break;
    }
  }
  catch (pov_base::Exception& e)
  {
    m_Session->SetFailed();
    m_Session->AppendErrorAndStatusMessage (e.what()) ;
  }
  try
  {
    shelloutProcessing->ProcessEvent(ShelloutProcessing::userAbort);
  }
  catch (pov_base::Exception& e)
  {
    // if it's a kCannotOpenFileErr, it means permission to run the process was denied
    // we don't set failed in that case as we allow shelloutprocessing to handle it
    if (!e.codevalid() || (e.code() != kCannotOpenFileErr))
      m_Session->SetFailed();
    m_Session->AppendErrorAndStatusMessage (e.what()) ;
  }

  return result;
}

bool VirtualFrontEnd::Pause()
{
  try
  {
    switch(state)
    {
      case kParsing:
        renderFrontend.PauseParser(sceneId);
        state = kPausedParsing;
        return true;

      case kPreSceneShellout:
      case kPreFrameShellout:
      case kPostFrameShellout:
        m_PauseRequested = true;
        return true;

      case kRendering:
        renderFrontend.PauseRender(viewId);
        state = kPausedRendering;
        return true;

      default:
        break;
    }
  }
  catch (pov_base::Exception&)
  {
    return false;
  }
  return false;
}

bool VirtualFrontEnd::Resume()
{
  try
  {
    switch(state)
    {
      case kPostShelloutPause:
        state = m_PostPauseState;
        return true;

      case kPausedParsing:
        if (renderFrontend.GetSceneState(sceneId) == SceneData::Scene_Paused)
          renderFrontend.ResumeParser(sceneId);
        state = kParsing;
        return true;

      case kPausedRendering:
        if (m_PausedAfterFrame)
        {
          state = kStarting;
          m_PausedAfterFrame = false;
          return true;
        }
        if (renderFrontend.GetViewState(viewId) == ViewData::View_Paused)
          renderFrontend.ResumeRender(viewId);
        state = kRendering;
        return true;

      default:
        break;
    }
  }
  catch (pov_base::Exception&)
  {
    return (false) ;
  }
  return false;
}

bool VirtualFrontEnd::HandleShelloutCancel()
{
  if (!shelloutProcessing->RenderCancelled())
    return false;

  int code(shelloutProcessing->ExitCode());
  string str(shelloutProcessing->GetCancelMessage());
  if (code)
  {
    state = kFailed;
    m_Session->SetFailed();
  }
  else
  {
    state = kStopped;
    m_Session->SetSucceeded(true);
  }
  m_Session->AppendErrorMessage(str) ;
  m_Session->AppendStatusMessage(str) ;
  return true;
}

State VirtualFrontEnd::Process()
{
  if (state == kReady)
    return kReady;

  switch(state)
  {
    case kStarting:
      try
      {
        m_Session->SetSucceeded (false);
        if (animationProcessing != nullptr)
        {
          shelloutProcessing->SetFrameClock(animationProcessing->GetNominalFrameNumber(), animationProcessing->GetClockValue());
          if (shelloutProcessing->SkipNextFrame() == false)
          {
            UCS2String filename;
            int frameId = animationProcessing->GetNominalFrameNumber();
            int frame = animationProcessing->GetRunningFrameNumber();
            options = animationProcessing->GetFrameRenderOptions ();
            if (m_Session->OutputToFileSet())
            {
              filename = imageProcessing->GetOutputFilename (options, frameId, animationProcessing->GetFrameNumberDigits());
              options.SetUCS2String (kPOVAttrib_OutputFile, filename.c_str());

              // test access permission now to avoid surprise later after waiting for
              // the render to complete.
              if (m_Session->TestAccessAllowed(filename, true) == false)
              {
                string str ("IO Restrictions prohibit write access to '");
                str += UCS2toSysString(filename);
                str += "'";
                throw POV_EXCEPTION(kCannotOpenFileErr, str);
              }
              shelloutProcessing->SetOutputFile(UCS2toSysString(filename));
              m_Session->AdviseOutputFilename (filename);
            }
            m_Session->AppendAnimationStatus (frameId, frame, animationProcessing->GetTotalFramesToRender(), filename);
          }
        }

        bool hadPreScene = shelloutProcessing->HadPreScene();

        // will do pre-scene instead if it hasn't yet been done
        try
        {
          shelloutProcessing->ProcessEvent(ShelloutProcessing::preFrame);
        }
        catch (pov_base::Exception& e)
        {
          // if it's a kCannotOpenFileErr, it means permission to run the process was denied
          // we don't set failed in that case as we allow shelloutprocessing to handle it
          m_Session->AppendErrorAndStatusMessage (e.what()) ;
          if (!e.codevalid() || (e.code() != kCannotOpenFileErr))
          {
            m_Session->SetFailed();
            return state = kFailed;
          }
        }

        // returns true if cancel has been requested
        if (HandleShelloutCancel())
          return state;

        state = hadPreScene ? kPreFrameShellout : kPreSceneShellout;
        return state;
      }
      catch(pov_base::Exception& e)
      {
        m_Session->SetFailed();
        m_Session->AppendErrorAndStatusMessage (e.what()) ;
        return state = kFailed;
      }

    case kPreSceneShellout:
      if (shelloutProcessing->ShelloutRunning() || HandleShelloutCancel())
        return state;

      // if a pause was requested by the user whilst the shellout was still running, do it now
      if (m_PauseRequested)
      {
        m_PostPauseState = kStarting;
        m_PauseRequested = false;
        return state = kPostShelloutPause;
      }

      // go back to kStarting; it won't run pre-scene again
      return state = kStarting;

    case kPreFrameShellout:
      if (shelloutProcessing->ShelloutRunning() || HandleShelloutCancel())
        return state;

      if (shelloutProcessing->SkipNextFrame())
      {
        string str(shelloutProcessing->GetSkipMessage());
        m_Session->AppendStatusMessage (str) ;
        m_Session->AppendStreamMessage (vfeSession::mInformation, str.c_str()) ;
        if ((animationProcessing != nullptr) && (animationProcessing->MoreFrames() == true))
        {
          animationProcessing->ComputeNextFrame();
          m_Session->SetPixelsRendered(0, m_Session->GetTotalPixels());
          m_Session->SetPercentComplete(0);
          if (m_PauseRequested)
          {
            m_PostPauseState = kStarting;
            m_PauseRequested = false;
            return state = kPostShelloutPause;
          }
          return state = kStarting;
        }
        else
        {
          m_Session->SetSucceeded (true);
          if (m_PauseRequested)
          {
            m_PostPauseState = kStopped;
            m_PauseRequested = false;
            return state = kPostShelloutPause;
          }
          return state = kStopped;
        }
      }

      // now set up the scene in preparation for parsing, then start the parser
      try { sceneId = renderFrontend.CreateScene(backendAddress, options, boost::bind(&vfe::VirtualFrontEnd::CreateConsole, this)); }
      catch(pov_base::Exception& e)
      {
        m_Session->SetFailed();
        m_Session->AppendErrorMessage (e.what()) ;
        m_Session->AppendStatusMessage (e.what()) ;
        return state = kFailed;
      }
      try { renderFrontend.StartParser(sceneId, options); }
      catch(pov_base::Exception& e)
      {
        m_Session->SetFailed();
        m_Session->AppendErrorMessage (e.what()) ;
        m_Session->AppendStatusMessage (e.what()) ;
        return state = kFailed;
      }
      if (m_PauseRequested)
      {
        m_PostPauseState = kParsing;
        m_PauseRequested = false;
        return state = kPostShelloutPause;
      }
      return state = kParsing;

    case kParsing:
    case kPausedParsing:
      switch(renderFrontend.GetSceneState(sceneId))
      {
        case SceneData::Scene_Paused:
          return state = kPausedParsing;

        case SceneData::Scene_Failed:
          m_Session->SetFailed();
          return state = kStopped;

        case SceneData::Scene_Stopping:
          return state = kStopping;

        case SceneData::Scene_Ready:
          if (state == kPausedParsing)
          {
            // it's possible for the parser to transition to Scene_Ready after a successful pause request.
            // this typically happens if the request comes in very close to the end of a parse, since the
            // task thread only checks the pause state intermittently. we don't start the renderer in this
            // case.
            return state;
          }
          try { viewId = renderFrontend.CreateView(sceneId, options, imageProcessing, boost::bind(&vfe::VirtualFrontEnd::CreateDisplay, this, _1, _2)); }
          catch(pov_base::Exception& e)
          {
            m_Session->SetFailed();
            m_Session->AppendErrorMessage (e.what()) ;
            m_Session->AppendStatusMessage (e.what()) ;
            return state = kFailed;
          }
          try { renderFrontend.StartRender(viewId, options); }
          catch(pov_base::Exception& e)
          {
            m_Session->ClearStatusMessages();
            if (e.codevalid() && e.code() == kImageAlreadyRenderedErr)
            {
              // this is not a failure; continue has been requested and
              // the file has already been rendered, so we skip it.
              m_Session->AppendStatusMessage ("File already rendered and continue requested; skipping.") ;
              m_Session->AppendStreamMessage (vfeSession::mInformation, "File already rendered and continue requested; skipping.") ;

              /* [JG] the block here is a duplicate of actions done after
               * the post frame shellout (that won't be reached because
               * the image was already there).
               */
              try { renderFrontend.CloseView(viewId); }
              catch (pov_base::Exception&) { /* Ignore any error here! */ }
              try { renderFrontend.CloseScene(sceneId); }
              catch (pov_base::Exception&) { /* Ignore any error here! */ }

              if ((animationProcessing != nullptr) && (animationProcessing->MoreFrames() == true))
              {
                animationProcessing->ComputeNextFrame();
                m_Session->SetPixelsRendered(0, m_Session->GetTotalPixels());
                m_Session->SetPercentComplete(0);
                return state = kStarting;
              }
              else
              {
                m_Session->SetSucceeded (true);
                return state = kStopped;
              }
            }

            m_Session->SetFailed();
            m_Session->AppendErrorMessage (e.what()) ;
            m_Session->AppendStatusMessage (e.what()) ;
            return state = kFailed;
          }

          // now we display the render window, if enabled
          {
            shared_ptr<Display> display(GetDisplay());
            if (display != nullptr)
            {
              vfeDisplay *disp = dynamic_cast<vfeDisplay *>(display.get());
              if (disp != nullptr)
                disp->Show () ;
            }
          }
          return state = kRendering;

        default:
          // Do nothing special.
          return state;
      }
      POV_ASSERT(false); // All cases of the preceding switch should return.

    case kRendering:
    case kPausedRendering:
      switch(renderFrontend.GetViewState(viewId))
      {
        case ViewData::View_Paused:
          return state = kPausedRendering;

        case ViewData::View_Failed:
          m_Session->SetFailed();
          return state = kStopped;

        case ViewData::View_Stopping:
          return state = kStopping;

        case ViewData::View_Rendered:
          if (state == kPausedRendering)
          {
            // it's possible for the renderer to transition to View_Rendered after a successful pause request.
            return kPausedRendering;
          }
          try
          {
            if (animationProcessing != nullptr)
            {
              if (m_Session->OutputToFileSet())
                m_Session->AdviseOutputFilename (imageProcessing->WriteImage(options, animationProcessing->GetNominalFrameNumber(), animationProcessing->GetFrameNumberDigits()));
              m_Session->AdviseFrameCompleted();
            }
            else
              if (m_Session->OutputToFileSet())
                m_Session->AdviseOutputFilename (imageProcessing->WriteImage(options));
          }
          catch (pov_base::Exception& e)
          {
            m_Session->SetFailed();
            m_Session->AppendErrorMessage (e.what()) ;
            m_Session->AppendStatusMessage (e.what()) ;
            // TODO: perhaps we should allow them to pause the queue/insert render
            //       here if need be.
            return state = kFailed;
          }
          try
          {
            shelloutProcessing->ProcessEvent(ShelloutProcessing::postFrame);
          }
          catch (pov_base::Exception& e)
          {
            // if it's a kCannotOpenFileErr, it means permission to run the process was denied
            // we don't set failed in that case as we allow shelloutprocessing to handle it
            m_Session->AppendErrorAndStatusMessage (e.what());
            if (!e.codevalid() || (e.code() != kCannotOpenFileErr))
            {
              m_Session->SetFailed();
              return state = kFailed;
            }
          }

          // check for cancel here: if the return value is true, state has already been changed
          if (HandleShelloutCancel())
            return state;

          return state = kPostFrameShellout;

        default:
          break;
      }
      return kRendering;

    case kPostFrameShellout:
      if (shelloutProcessing->ShelloutRunning() || HandleShelloutCancel())
        return state;
      if ((animationProcessing == nullptr) || animationProcessing->MoreFrames() == false)
      {
        m_Session->SetSucceeded (true);
        if (m_PauseRequested)
        {
          m_PostPauseState = kStopped;
          m_PauseRequested = false;
          return state = kPostShelloutPause;
        }
        return state = kStopped;
      }
      if (shelloutProcessing->SkipAllFrames())
      {
        string str(shelloutProcessing->GetSkipMessage());
        m_Session->SetSucceeded (true);
        m_Session->AppendStatusMessage (str) ;
        m_Session->AppendStreamMessage (vfeSession::mInformation, str.c_str()) ;
        if (m_PauseRequested)
        {
          m_PostPauseState = kStopped;
          m_PauseRequested = false;
          return state = kPostShelloutPause;
        }
        return state = kStopped;
      }
      /* [JG] the actions hereafter should be also done
       * when the image already existed: tidy up the data before next frame or stop
       */
      try { renderFrontend.CloseView(viewId); }
      catch (pov_base::Exception&) { /* Ignore any error here! */ }
      try { renderFrontend.CloseScene(sceneId); }
      catch (pov_base::Exception&) { /* Ignore any error here! */ }
      animationProcessing->ComputeNextFrame();
      if (m_Session->GetPauseWhenDone())
      {
        // wait for a manual continue
        m_PausedAfterFrame = true;
        m_PauseRequested = false;
        return state = kPausedRendering;
      }
      if (m_PauseRequested)
      {
        m_PostPauseState = kStarting;
        m_PauseRequested = false;
        return state = kPostShelloutPause;
      }
      return state = kStarting;

    case kPostSceneShellout:
      if (shelloutProcessing->ShelloutRunning())
        return state;
      return state = kDone;

    case kPostShelloutPause:
      break;

    case kStopping:
      if (renderFrontend.GetSceneState(sceneId) == SceneData::Scene_Ready || renderFrontend.GetSceneState(sceneId) == SceneData::Scene_Failed)
        return state = kStopped;
      if (renderFrontend.GetViewState(viewId) == ViewData::View_Rendered || renderFrontend.GetViewState(viewId) == ViewData::View_Failed)
        return state = kStopped;
      return kStopping;

    case kFailed:
      m_Session->SetFailed();
      // ShelloutProcessing ignores the fatal error event if it requested a cancel
      try { shelloutProcessing->ProcessEvent(ShelloutProcessing::fatalError); }
      catch (pov_base::Exception&) { /* Ignore any error here */ }
      return state = kStopped;

    case kStopped:
      try { renderFrontend.CloseView(viewId); }
      catch (pov_base::Exception&) { /* Ignore any error here! */ }
      try { renderFrontend.CloseScene(sceneId); }
      catch (pov_base::Exception&) { /* Ignore any error here! */ }
      animationProcessing.reset();
      imageProcessing.reset();

      // we only run the post-scene or failed action if we have passed the pre-scene point
      // i.e. if we stop before pre-scene, we don't run post-scene
      if (shelloutProcessing->HadPreScene())
      {
        if (m_Session->Failed())
        {
          // it is possible for us to get to kStopped without going through kFailed
          // so we call the failed shellout event here just in case: ShelloutProcessing
          // can handle being called twice for the same event.
          try { shelloutProcessing->ProcessEvent(ShelloutProcessing::fatalError); }
          catch (pov_base::Exception&) { /* Ignore any error here */ }
        }

        if (!shelloutProcessing->HadPostScene())
        {
          try { shelloutProcessing->ProcessEvent(ShelloutProcessing::postScene); }
          catch (pov_base::Exception&) { /* Ignore any error here! */ }
          return state = kPostSceneShellout;
        }
      }

      return state = kDone;

    case kDone:
      return state = kReady;

    default:
      return state;
  }
  POV_ASSERT(false); // All cases of the preceding switch should return.

  return state;
}

void VirtualFrontEnd::SetResultPointers(Console **cr, Image **ir, Display **dr)
{
  consoleResult = cr;
  displayResult = dr;
}

bool VirtualFrontEnd::IsPausable (void)
{
  switch (GetState ())
  {
    case kParsing :
    case kPausedParsing :
    case kRendering :
    case kPausedRendering :
    case kPreSceneShellout:
    case kPreFrameShellout:
    case kPostFrameShellout:
    case kPostShelloutPause:
         return (true) ;

    default :
         return (false) ;
  }
}

bool VirtualFrontEnd::Paused (void)
{
  switch (GetState ())
  {
    case kPausedParsing :
    case kPausedRendering :
    case kPostShelloutPause:
         return (true) ;

    case kPreSceneShellout:
    case kPreFrameShellout:
    case kPostFrameShellout:
         return m_PauseRequested;

    default :
         return (false) ;
  }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// helper funtions
//
////////////////////////////////////////////////////////////////////////////////////////

int Allow_File_Write (const UCS2 *Filename, const unsigned int FileType)
{
  if (strcmp(UCS2toSysString(Filename).c_str(), "stdout") == 0 || strcmp(UCS2toSysString(Filename).c_str(), "stderr") == 0)
    return true;
  return (vfeSession::GetSessionFromThreadID()->TestAccessAllowed(Filename, true));
}

int Allow_File_Read (const UCS2 *Filename, const unsigned int FileType)
{
  return (vfeSession::GetSessionFromThreadID()->TestAccessAllowed(Filename, false));
}

FILE *vfeFOpen (const UCS2String& name, const char *mode)
{
  return (fopen (UCS2toSysString (name).c_str(), mode)) ;
}

}
// end of namespace vfe
