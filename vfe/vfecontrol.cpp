//******************************************************************************
///
/// @file vfe/vfecontrol.cpp
///
/// This module contains the render setup and control logic for the VFE.
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

#include <cstring>

#include <list>

#include "vfe.h"

/***************************************************************************************/
/* TODO FIXME: we should call into the POVMS code to do this lookup, but as Thorsten's */
/* away right now I can't ask him how he would prefer it be implemented. So the below  */
/* code is only temporary!                                                             */
/***************************************************************************************/
namespace pov_frontend
{
  extern struct ProcessOptions::INI_Parser_Table RenderOptions_INI_Table[];
  extern struct ProcessRenderOptions::Output_FileType_Table FileTypeTable[];
}
// end of namespace pov_frontend

static struct pov_frontend::ProcessOptions::INI_Parser_Table *GetPT(const char *OptionName)
{
  for (struct pov_frontend::ProcessOptions::INI_Parser_Table *op = pov_frontend::RenderOptions_INI_Table; op->keyword != nullptr; op++)
    if (strcmp(op->keyword, OptionName) == 0)
      return op;
  return nullptr;
}

/***************************************************************************************/

namespace vfe
{

static int GetUCS2String(POVMSObjectPtr object, POVMSType key, char *result, int *maxlen)
{
  UCS2 *str = new UCS2 [*maxlen] ;
  int err = POVMSUtil_GetUCS2String (object, key, str, maxlen) ;
  if (err == kNoErr)
    strcpy (result, UCS2toSysString (str).c_str ()) ;
  delete[] str ;
  return err ;
}

bool vfeSession::ProcessCancelRender (void)
{
  if (m_Frontend->GetState () > kReady)
  {
    try
    {
      m_Frontend->Stop () ;
    }
    catch (pov_base::Exception& e)
    {
      if (e.codevalid() && e.code() == kNotNowErr)
      {
        Delay (100) ;
        return (m_Frontend->GetState() == kReady);
      }
      if (m_Frontend->GetState () > kReady)
      {
        // TODO FIXME
        // char str [256] ;
        // sprintf (str, "Failed to send stop rendering message (%s)", e.what()) ;
        // MessageBox (nullptr, str, "POVMS error", MB_OK | MB_ICONEXCLAMATION) ;
        Delay (100) ;
        return (m_Frontend->GetState () == kReady);
      }
    }
  }
  else
  {
    if (m_Frontend->GetState () == kReady)
    {
      // we possibly have an anomalous situation
      // TODO FIXME
      // MessageBox (nullptr, "Warning: had to force state to stopped", "Cancel Render", MB_OK | MB_ICONEXCLAMATION) ;
      RenderStopped();
    }
  }
  return (true);
}

// Requests the backend to cancel the current render. Returns vfeNotRunning
// if there is no current render, vfeAlreadyStopping if a stop has already
// been requested, or vfeNoError otherwise. Also sets m_LastError in all
// cases. Note that this request is handled by the worker thread, like for
// vfeSession::Pause(), but unlike that request this one is handled
// asynchronously. This means you need to be prepared to handle the situation
// that even though you've called CancelRender(), the session hasn't caught
// up with the processing of the request yet.
int vfeSession::CancelRender()
{
  pov_frontend::State state = m_Frontend->GetState();

  if (state == kReady)
    return (m_LastError = vfeNotRunning) ;
  if (state >= kStopping)
    return (m_LastError = vfeAlreadyStopping) ;
  m_RenderCancelRequested = true;
  return (m_LastError = vfeNoError);
}

bool vfeSession::StopRender(const std::string& reason)
{
  if (m_Frontend->GetState() <= kReady)
    return true;
  if (reason.empty() == false)
  {
    AppendStatusMessage (reason);
    AppendErrorMessage (reason);
  }
  try
  {
    m_Frontend->Stop() ;
  }
  catch (pov_base::Exception& e)
  {
    if (e.codevalid() && e.code() == kNotNowErr)
    {
      Delay (100) ;
      return (m_Frontend->GetState() == kReady);
    }
    if (m_Frontend->GetState () > kReady)
    {
      Delay (100) ;
      return (m_Frontend->GetState () == kReady);
    }
  }
  return (true);
}

// This method allows your platform code to perform platform-specific action
// when a render stops (whether it succeeds or fails). A good example would
// be to shrink the heap. Be aware that it is called in the context of the
// worker thread, and that therefore on windowing systems where thread context
// is relevent to making certain UI calls, it may not be possible to perform
// certain UI actions here. For most platforms the default implementation
// should be sufficient.
void vfeSession::RenderStopped (void)
{
  // TODO: we should check the state of the various stream output options
  // before writing to the streams
  if (m_Succeeded)
  {
    AppendStreamMessage (vfeSession::mUnclassified, "POV-Ray finished");
    AppendStreamMessage (vfeSession::mDivider, "");
  }
  else
  {
    AppendStreamMessage (vfeSession::mUnclassified, m_RenderCancelled ? "Render cancelled by user" : "Render failed");
    AppendStreamMessage (vfeSession::mDivider, "");
  }
  ShrinkHeap();
  NotifyEvent(stRenderShutdown);
}

// Sets the options to be used on the next render. Accepts a vfeRenderOptions
// instance as its only parameter, and returns any one of a number of possible
// error codes (and sets m_LastError), as documented below:
//
//   vfeFailedToInitObject           - this is an internal error
//   vfeFailedToSetMaxThreads        - self-explanatory
//   vfeFailedToParseINI             - an INI file specified could not be parsed
//   vfeFailedToSetSource            - the source file specified could not be set
//   vfeFailedToParseCommand         - a command-line option was invalid
//   vfeNoInputFile                  - no input file specified either directly or via INI
//   vfeRenderBlockSizeTooSmall      - self-explanatory
//   vfeFailedToWriteINI             - a request to write the render options to an INI file failed
//   vfeUnsupportedOptionCombination - unsupported option combination
//
// If vfeRenderOptions explicitly specifies a source file, it will override
// any set via a parsed INI file. Furthermore, any source file set via a
// command-line option overrides both of the above.
//
// Note that it is your responsibility to add any default INI files that should
// be processed to the INI file list; neither SetOptions() nor any other part
// of the VFE or POV-Ray code will do that for you. This includes non-platform
// specific files such as a potential povray.ini in the CWD.
int vfeSession::SetOptions (vfeRenderOptions& opts)
{
  int                     err;
  UCS2                    str[POV_FILENAME_BUFFER_CHARS+1]; // TODO FIXME - use a C++ style string instead.
  POVMSObject             obj;
  vfeProcessRenderOptions options(this);

  m_OutputToFileSet = false;
  m_UsingAlpha = false;
  m_RenderingAnimation = false;
  m_RealTimeRaytracing = false;
  m_ClocklessAnimation = false;
  m_RenderWidth = m_RenderHeight = 0;
  ClearOptions();

  if ((err = POVMSObject_New (&obj, kPOVObjectClass_RenderOptions)) != kNoErr)
    return (m_LastError = vfeFailedToInitObject) ;

  if ((err = POVMSUtil_SetInt (&obj, kPOVAttrib_MaxRenderThreads, opts.m_ThreadCount)) != kNoErr)
    return (m_LastError = vfeFailedToSetMaxThreads) ;

  // we set this here for potential use by the IO permissions path checking code
  m_InputFilename = opts.m_SourceFile;

  // most likely povray.ini will be the first INI file processed here (as it's included by default)
  for (std::vector<UCS2String>::iterator i = opts.m_IniFiles.begin(); i != opts.m_IniFiles.end(); i++)
  {
    // we call TestAccessAllowed() here, even though ParseFile() will do it also, since if
    // access is denied, the reason will not be obvious (ParseFile() just returns kCannotOpenFileErr).
    if (!TestAccessAllowed (Path(*i), false))
      return (m_LastError = vfeIORestrictionDeny);

    if ((err = options.ParseFile (UCS2toSysString(*i).c_str(), &obj)) != kNoErr)
      return (m_LastError = vfeFailedToParseINI) ;

    // we keep this up to date since the IO permissions feature will use the current input
    // filename to determine the path for default read/write permission in the scene dir.
    int n = sizeof (str) ;
    if ((err = POVMSUtil_GetUCS2String (&obj, kPOVAttrib_InputFile, str, &n)) == kNoErr)
      if (m_InputFilename != str)
        m_InputFilename = str;
  }

  // m_SourceFile overrides any source file set by the INI files
  if (opts.m_SourceFile.empty() == false)
  {
    m_InputFilename = opts.m_SourceFile;
    if ((err = POVMSUtil_SetUCS2String (&obj, kPOVAttrib_InputFile, opts.m_SourceFile.c_str())) != kNoErr)
      return (m_LastError = vfeFailedToSetSource);
  }

  // any source file set on the command-line overrides a source file set another way
  for (std::vector<std::string>::iterator i = opts.m_Commands.begin(); i != opts.m_Commands.end(); i++)
  {
    if ((err = options.ParseString (i->c_str(), &obj)) != kNoErr)
      return (m_LastError = vfeFailedToParseCommand) ;
    int n = sizeof (str) ;
    if ((err = POVMSUtil_GetUCS2String (&obj, kPOVAttrib_InputFile, str, &n)) == kNoErr)
      if (m_InputFilename != str)
        m_InputFilename = str;
  }

  int n = sizeof (str) ;
  if ((err = POVMSUtil_GetUCS2String (&obj, kPOVAttrib_InputFile, str, &n)) != kNoErr)
    return (m_LastError = vfeNoInputFile);
  m_InputFilename = str;

  POVMSUtil_GetInt (&obj, kPOVAttrib_Width, &m_RenderWidth) ;
  POVMSUtil_GetInt (&obj, kPOVAttrib_Height, &m_RenderHeight) ;

  std::list<Path> libpaths;
  POVMS_Object ropts (obj) ;
  if (ropts.Exist (kPOVAttrib_LibraryPath))
  {
    POVMS_List pathlist;
    ropts.Get (kPOVAttrib_LibraryPath, pathlist) ;

    // we take the opportunity to remove any duplicates that are in the path list.
    // it's cleaner to do that here, rather than in the INI parser, since it's table-
    // driven and doesn't have an explicit function for adding library paths per se.
    //
    // we use the Path equivalence operator rather than a string compare since
    // using Path should handle platform-specific issues like case-sensitivity (or,
    // rather, lack thereof). note that at the time of writing, the Path class did
    // not yet implement case-insensitive comparisons.
    //
    // NB while it would of course be more efficient to sort the list so searches are
    // faster, we'd have to make a copy of it to do that, as we can't change the order
    // of existing entries (that would change the include path search order). it's not
    // common to have a lot of include paths, so we just use linear searches.
    for (int i = 1; i <= pathlist.GetListSize(); i++)
    {
      POVMS_Attribute lp;

      pathlist.GetNth(i, lp);
      Path path(lp.GetUCS2String());
      if (find(libpaths.begin(), libpaths.end(), path) == libpaths.end())
        libpaths.push_back(path);
    }
  }

  if (opts.m_LibraryPaths.empty() == false)
  {
    for (std::vector<UCS2String>::const_iterator i = opts.m_LibraryPaths.begin(); i != opts.m_LibraryPaths.end(); i++)
    {
      Path path(*i);

      if (find(libpaths.begin(), libpaths.end(), path) == libpaths.end())
        libpaths.push_back(path);
    }
  }

  if (libpaths.empty() == false)
  {
    POVMS_List pathlist;
    for (std::list<Path>::iterator i = libpaths.begin(); i != libpaths.end(); i++)
    {
      POVMS_Attribute attr((*i)().c_str());
      pathlist.Append(attr);
    }
    ropts.Set (kPOVAttrib_LibraryPath, pathlist) ;
  }

  if (ropts.TryGetBool(kPOVAttrib_RealTimeRaytracing, false) == true)
    ropts.SetBool(kPOVAttrib_OutputToFile, false);

  m_OutputToFileSet = ropts.TryGetBool(kPOVAttrib_OutputToFile, true);

  // this is a bit messy: Grayscale_Output or OutputAlpha may be specified
  // in an INI file or elsewhere prior to the output file type being set.
  // so we can't check to see if it is supported with that file type
  // until all options have been parsed.
  if (m_OutputToFileSet)
  {
    int oft = ropts.TryGetInt(kPOVAttrib_OutputFileType, DEFAULT_OUTPUT_FORMAT);
    bool has16BitGrayscale = false;
    bool hasAlpha = false;
    for (int i = 0; FileTypeTable[i].internalId != 0; i ++)
    {
      if (oft == FileTypeTable[i].internalId)
      {
        has16BitGrayscale = FileTypeTable[i].has16BitGrayscale;
        hasAlpha          = FileTypeTable[i].hasAlpha;
        break;
      }
    }
    if (ropts.TryGetBool(kPOVAttrib_GrayscaleOutput, false) && !has16BitGrayscale)
    {
      AppendStatusMessage ("Grayscale output not currently supported with selected output file type.");
      AppendErrorMessage ("Grayscale output not currently supported with selected output file type.") ;
      return (m_LastError = vfeUnsupportedOptionCombination);
    }
    if (oft == kPOVList_FileType_PPM)
    {
        if (!ropts.Exist(kPOVAttrib_FileGammaType))
        {
            AppendWarningMessage ("Warning: Output image gamma not specified for Netpbm (PGM/PPM) file; POV-Ray will default to the\n"
                                  "official standard, but competing de-facto standards exist. To get rid of this warning,\n"
                                  "explicitly specify \"File_Gamma=bt709\". If the results do not match your expectations, try\n"
                                  "\"File_Gamma=srgb\" or \"File_Gamma=1.0\".");
        }
    }
    if (ropts.TryGetBool(kPOVAttrib_OutputAlpha, false) && !hasAlpha)
    {
      AppendWarningMessage ("Warning: Alpha channel output currently not (or not officially) supported with selected output file type.") ;
    }
  }

  if (ropts.TryGetInt(kPOVAttrib_RenderBlockSize, 32) < 4)
    return (m_LastError = vfeRenderBlockSizeTooSmall);

  if ((ropts.TryGetInt(kPOVAttrib_DisplayGammaType, DEFAULT_DISPLAY_GAMMA_TYPE) == kPOVList_GammaType_PowerLaw) &&
      (ropts.TryGetFloat(kPOVAttrib_DisplayGamma, DEFAULT_DISPLAY_GAMMA) < 0.001f))
    return (m_LastError = vfeDisplayGammaTooSmall);
  if (ropts.Exist(kPOVAttrib_FileGammaType))
  {
    if ((ropts.GetInt(kPOVAttrib_FileGammaType) == kPOVList_GammaType_PowerLaw) &&
        (ropts.GetFloat(kPOVAttrib_FileGamma) < 0.001f))
      return (m_LastError = vfeFileGammaTooSmall);
  }

  n = sizeof (str) ;
  if ((err = POVMSUtil_GetUCS2String (&obj, kPOVAttrib_CreateIni, str, &n)) == kNoErr && str [0] != '\0')
    if ((err = options.WriteFile (UCS2toSysString(str).c_str(), &obj)) != kNoErr)
      return (m_LastError = vfeFailedToWriteINI);

  opts.m_Options = ropts;
  m_RenderOptions = opts ;
  m_OptionsSet = true;

  return (m_LastError = vfeNoError) ;
}

bool vfeSession::OptionPresent(const char *OptionName)
{
  struct ProcessOptions::INI_Parser_Table *op = GetPT(OptionName);
  if (op == nullptr)
    throw POV_EXCEPTION_STRING("Invalid option");

  if (m_OptionsSet == false)
    throw POV_EXCEPTION_STRING("Options not set");

  return m_RenderOptions.m_Options.Exist(op->key);
}

bool vfeSession::GetBoolOption(const char *OptionName, bool DefaultVal)
{
  struct ProcessOptions::INI_Parser_Table *op = GetPT(OptionName);
  if (op == nullptr)
    throw POV_EXCEPTION_STRING("Invalid option");
  if (m_OptionsSet == false)
    throw POV_EXCEPTION_STRING("Options not set");

  return m_RenderOptions.m_Options.TryGetBool(op->key, DefaultVal);
}

int vfeSession::GetIntOption(const char *OptionName, int DefaultVal)
{
  struct ProcessOptions::INI_Parser_Table *op = GetPT(OptionName);
  if (op == nullptr)
    throw POV_EXCEPTION_STRING("Invalid option");
  if (m_OptionsSet == false)
    throw POV_EXCEPTION_STRING("Options not set");

  return m_RenderOptions.m_Options.TryGetInt(op->key, DefaultVal);
}

float vfeSession::GetFloatOption(const char *OptionName, float DefaultVal)
{
  struct ProcessOptions::INI_Parser_Table *op = GetPT(OptionName);
  if (op == nullptr)
    throw POV_EXCEPTION_STRING("Invalid option");
  if (m_OptionsSet == false)
    throw POV_EXCEPTION_STRING("Options not set");

  return m_RenderOptions.m_Options.TryGetFloat(op->key, DefaultVal);
}

std::string vfeSession::GetStringOption(const char *OptionName, const char *DefaultVal)
{
  struct ProcessOptions::INI_Parser_Table *op = GetPT(OptionName);
  if (op == nullptr)
    throw POV_EXCEPTION_STRING("Invalid option");
  if (m_OptionsSet == false)
    throw POV_EXCEPTION_STRING("Options not set");

  return m_RenderOptions.m_Options.TryGetString(op->key, DefaultVal);
}

UCS2String vfeSession::GetUCS2StringOption(const char *OptionName, const UCS2String& DefaultVal)
{
  struct ProcessOptions::INI_Parser_Table *op = GetPT(OptionName);
  if (op == nullptr)
    throw POV_EXCEPTION_STRING("Invalid option");
  if (m_OptionsSet == false)
    throw POV_EXCEPTION_STRING("Options not set");

  return m_RenderOptions.m_Options.TryGetUCS2String(op->key, DefaultVal);
}

// Start a render. Will set m_LastError and return vfeRenderOptionsNotSet
// if you didn't call vfeSession::SetOptions() first. Otherwise will attempt
// to start the render; if this fails vfeFailedToSendRenderStart is returned.
// If the start attempt succeeds (note this is distinct from the start itself
// actually succeeding - all that the lack of a vfeFailedToSendRenderStart
// error means is that the start request was successfully sent to the backend)
// then a stRenderStartup notification is sent and the state is changed to
// kStarting.
//
// If the start attempt fails (signified by the receipt of a std::exception),
// the exception code will, in the case of a pov_base::Exception, be extracted
// and stored in m_LastError and returned; otherwise (not pov_base::Exception),
// m_LastError is set to -1. In either case, a failed status is set (so that
// vfeSession::Failed() will return true), and, if a fatal error message has
// not already been queued, the text of the exception will be added to both
// the status and error message queues.
int vfeSession::StartRender()
{
  if (m_OptionsSet == false)
    return (m_LastError = vfeRenderOptionsNotSet);

  if (m_RenderOptions.m_Options.Exist(kPOVAttrib_CreateIni))
  {
    // we deliberately do this twice - once here and once in SetOptions
    POVMSUCS2String fn = m_RenderOptions.m_Options.TryGetUCS2String(kPOVAttrib_CreateIni, "");
    if (fn.empty() == false)
    {
      vfeProcessRenderOptions options(this);
      POVMSObject obj = *m_RenderOptions.GetOptions();
      if (options.WriteFile (UCS2toSysString(fn).c_str(), &obj) != kNoErr)
        return (m_LastError = vfeFailedToWriteINI);
    }
  }

  try
  {
    if (!m_Frontend->Start (m_RenderOptions.m_Options))
      return (m_LastError = vfeFailedToSendRenderStart) ;
  }
  catch (std::exception& e)
  {
    if (dynamic_cast<pov_base::Exception *> (&e) != nullptr)
      m_RenderErrorCode = dynamic_cast<pov_base::Exception *> (&e)->code() ;
    if (m_RenderErrorCode == 0)
      m_RenderErrorCode = -1 ;
    m_Failed = true ;
    if (m_HadErrorMessage == false)
    {
      AppendErrorMessage (e.what()) ;
      AppendStatusMessage (e.what()) ;
    }
    return (m_LastError = m_RenderErrorCode) ;
  }

  NotifyEvent(stRenderStartup);

  // set this now in case an error causes the frontend state to return to
  // kReady before we pick up kStarting in vfeSession::Process().
  StateChanged(kStarting) ;

  return (m_LastError = vfeNoError) ;
}

}
// end of namespace vfe
