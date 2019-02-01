//******************************************************************************
///
/// @file vfe/win/vfeplatform.cpp
///
/// Platform-specific support code for the VFE.
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

#include <memory>

#include <direct.h>
#include <windows.h>

#include "vfe.h"
#include "base/filesystem.h"
#include "base/stringtypes.h"

#include <boost/algorithm/string.hpp>

namespace pov_frontend
{
  bool MinimizeShellouts(void);
  bool ShelloutsPermitted(void);
}
// end of namespace pov_frontend

namespace vfePlatform
{
  using namespace vfe;

  /////////////////////////////////////////////////////////////////////////
  // return a number that uniquely identifies the calling thread amongst
  // all other running threads in the process (and preferably in the OS).
  POVMS_Sys_Thread_Type GetThreadId ()
  {
    return (GetCurrentThreadId ());
  }

  vfeWinSession::vfeWinSession(int id) :
    m_LastTimestamp(0),
    m_TimestampOffset(0),
    vfeSession(id)
  {
    char str [MAX_PATH];

    if (GetTempPath (sizeof (str) - 7, str) == 0)
      throw vfeException("Could not get temp dir from Windows API");
    strcat (str, "povwin\\");
    // if we fail to creat our temp dir, just use the default one
    if ((CreateDirectory(str, nullptr) == 0) && (GetLastError() != ERROR_ALREADY_EXISTS))
      GetTempPath (sizeof (str), str);
    m_TempPathString = str;
  }

  void vfeWinSession::Clear(bool Notify)
  {
    m_ReadFiles.clear();
    m_WriteFiles.clear();
    vfeSession::Clear(Notify);
  }

  //////////////////////////////////////////////////////////////
  // User-interface functions
  //////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  // this method will get called when a render completes and the image writing
  // code is unable to write the requested output file for some reason (e.g.
  // disk full, existing output file is read-only, or whatever). the return
  // value can determine any one of several possible actions the code will take.
  // it's up to you what you do here, but a typical example would be to display
  // an error dialog showing the reason (which will be a short string) and the old
  // path, and allow the user to browse for a new path (you may want to auto-
  // suggest one). it is allowable to perform tests yourself on the path to
  // obtain some OS-specific information as to why it failed in order to better
  // determine what to suggest as the new path (e.g. the the output path is
  // not writable due to insufficient privileges, you may want to default the
  // new path to the user's home directory).
  //
  // any new path that is to be returned should be placed in NewPath. the parameter
  // CallCount will initially be 0, and represents the number of times the
  // method has been called for a given rendering; this allows you for example
  // to auto-default on the first call and prompt the user on subsequent ones.
  // (note that if you do this and the auto-default succeeds, it's up to you to
  // notify the user that the output file is not what they originally asked for).
  //
  // you may want to place a timeout on this dialog if the session is running
  // an animation and return in the case of a timeout a default value.
  //
  // the return values can be any of the following:
  //
  //   0 : don't try again, leave the render without an output file but preserve
  //       the state file (so a render with +c will reload the image data).
  //   1 : reserved for later support
  //   2 : don't try again and delete the state file (rendered data can't be
  //       recovered).
  //   3 : try again with the same file (NewPath is ignored).
  //   4 : try again with the new path returned in NewPath.
  //
  // note that if you choose to specify any of the "don't try again" options,
  // and the session happens to be an animation, and it's likely the same
  // error will occur again (e.g. invalid output path or something), then
  // you may want to call the render cancel API so the user isn't bombarded
  // with an error message for each frame of the render.
  int vfeWinSession::RequestNewOutputPath(int CallCount, const std::string& Reason, const UCS2String& OldPath, UCS2String& NewPath)
  {
    // TODO
    return 0;
  }

  //////////////////////////////////////////////////////////////
  // File-system related support functions
  //////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  // return an absolute path including trailing path separator.
  // *nix platforms might want to just return "/tmp/" here.
  // (NB This implementation actually returns UTF-16.)
  UCS2String vfeWinSession::GetTemporaryPath(void) const
  {
//    return m_TempPathString;
    return SysToUCS2String(m_TempPathString);
  }

  /////////////////////////////////////////////////////////////////////////
  // might be better called 'CreateTemporaryFileName()' since pov
  // doesn't actually want the file opened; just the full path and
  // name to one that it can use.
  UCS2String vfeWinSession::CreateTemporaryFile(void) const
  {
    pov_base::Filesystem::TemporaryFilePtr tempFile(new pov_base::Filesystem::TemporaryFile);
    m_TempFiles.push_back(tempFile);
    return tempFile->GetFileName();
  }

  /////////////////////////////////////////////////////////////////////////
  // you could check that the path given lies within the paths that
  // your platform gives out for temporary files if you want; this
  // example doesn't do that but it's not a bad idea to add.
  void vfeWinSession::DeleteTemporaryFile(const UCS2String& filename) const
  {
    pov_base::Filesystem::DeleteFile(filename);
  }

  //////////////////////////////////////////////////////////////
  // vfe or POVMS related support functions
  //////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  // This method will get called on POVMS critical errors (e.g. cannot
  // allocate memory, amongst other things). Note that you should not
  // assume that you can e.g. allocate memory when processing this call.
  //
  // Before calling this function vfe sets a flag that prevents the worker
  // thread from processing any more messages; it will wait for you to
  // call Shutdown(). Your UI thread can find out if this method has been
  // called by checking for the presence of the stCriticalError status bit
  // returned from vfeSession::GetStatus() (note that this bit is not
  // necessarily already set at the time the method is called though).
  //
  // If you are running a genuine virtual frontend (e.g. stateless HTTP
  // interface), we may want to set a flag to display the message later
  // rather than pop up a messagebox on the local windowstation. Otherwise
  // you would probably display the message immediately.
  void vfeWinSession::NotifyCriticalError (const char *message, const char *filename, int line)
  {
    MessageBox (nullptr, message, "POV-Ray Critical Error", MB_ICONERROR | MB_OK) ;
  }

  ////////////////////////////////////////////////////////////////////////
  // Return a timestamp to be used internally for queue sorting etc. The
  // value returned must be 64-bit and in milliseconds; the origin of the
  // count is not important (e.g. milliseconds since 1/1/1970, or whatever
  // it doesn't matter), as long as it is consistent value (milliseconds
  // since system boot is NOT a valid value since it will change each boot).
  // Also please don't call time() and multiply by 1000 since vfe wants at
  // least 100ms precision (so it can do sub-one-second event timing).
  //
  // It's also important that the count not go backwards during the life
  // of a vfeSession instance; this means you should attempt to detect wall
  // clock changes by caching the last value your implementation returns
  // and adding an appropriate offset if you calculate a lower value later
  // in the session.
  POV_LONG vfeWinSession::GetTimestamp(void) const
  {
    __int64           fileTime ;
    SYSTEMTIME        systemTime ;

    GetSystemTime (&systemTime) ;
    SystemTimeToFileTime (&systemTime, reinterpret_cast<FILETIME *>(&fileTime)) ;
    fileTime /= 10000;
    fileTime += m_TimestampOffset;
    if (fileTime < m_LastTimestamp)
    {
      // perhaps the system clock has been adjusted?
      m_TimestampOffset += m_LastTimestamp - fileTime;
      fileTime = m_LastTimestamp;
    }
    else
      m_LastTimestamp = fileTime;
    return (fileTime) ;
  }

  /////////////////////////////////////////////////////////////////////////
  // called when the worker thread starts - you could if you like set the
  // thread priority here.
  void vfeWinSession::WorkerThreadStartup()
  {
    // on the windows platform it's important to set this to a higher
    // value than the render threads, otherwise we have problems with
    // e.g. processing cancel render requests etc.
    SetThreadPriority (GetCurrentThread (), THREAD_PRIORITY_HIGHEST);
  }

  /////////////////////////////////////////////////////////////////////////
  // called just before the worker thread exits.
  void vfeWinSession::WorkerThreadShutdown()
  {
  }

  /////////////////////////////////////////////////////////////////////////
  // The following methods support the I/O permissions feature
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  // this really should take the codepage into consideration and not convert
  // to an ASCII/Latin-1 string.
  bool vfeWinSession::StrCompareIC (const UCS2String& lhs, const UCS2String& rhs) const
  {
    return (_stricmp (UCS2toSysString(lhs).c_str(), UCS2toSysString(rhs).c_str()) == 0);
  }

  /////////////////////////////////////////////////////////////////////////
  // return true if the path component of file is equal to the path component
  // of path. will also return true if recursive is true and path is a parent
  // of file. does not support relative paths, and will convert UCS2 paths to
  // Latin-1 and perform case-insensitive comparisons.
  bool vfeWinSession::TestPath (const Path& path, const Path& file, bool recursive) const
  {
    // we don't support relative paths
    if (path.HasVolume() == false || file.HasVolume() == false)
      return (false);
    if (StrCompareIC(path.GetVolume(), file.GetVolume()) == false)
      return (false);

    std::vector<UCS2String> pc = path.GetAllFolders();
    std::vector<UCS2String> fc = file.GetAllFolders();
    if (fc.size() < pc.size())
      return (false) ;
    for (int i = 0 ; i < pc.size(); i++)
      if (StrCompareIC(fc[i], pc[i]) == false)
        return (false) ;
    return (fc.size() == pc.size() ? true : recursive);
  }

  /////////////////////////////////////////////////////////////////////////
  // returns true if opening the given file in the specified mode is to
  // be permitted. it is allowed to ask the user, so this method could
  // take some time to return (especially if user is not at workstation ...)
  //
  // TODO: modify this to work fully with UCS2 strings (no conversion)
  bool vfeWinSession::TestAccessAllowed (const Path& file, bool isWrite) const
  {
    char buf[_MAX_PATH];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char longbuf[_MAX_PATH];
    char *s;
    bool returnOK = false;

    // if no paths are set we assume that the IO restrictions feature isn't being used
    if (GetExcludedPaths().empty() && GetReadPaths().empty() && GetWritePaths().empty())
      returnOK = true;

    // if it's a read and there are no read paths set, we assume all reads are allowed
    if (isWrite == false && GetReadPaths().empty() == true)
      returnOK = true;

    int n = GetFullPathName (UCS2toSysString(file()).c_str(), _MAX_PATH, buf, &s);
    if ((n == 0) || (n > sizeof(buf)) || (s == nullptr) || (s == buf))
    {
      if (returnOK == true)
        return (true);

      // TODO: issue appropriate error message
      return (false) ;
    }

    // GetLongPathName is available on win95 due to the inclusion of newapis.h
    n = GetLongPathName(buf, longbuf, _MAX_PATH) ;
    if ((n > 0) && (n < _MAX_PATH))
    {
      // we store the filename before doing anything else, since this may be used elsewhere
      // note that we convert it to lower case first
      _strlwr(longbuf);
      if (isWrite)
        m_WriteFiles.insert(longbuf);
      else
        m_ReadFiles.insert(longbuf);
    }

    // now we have stored the filename for later use, if there is no need to
    // continue testing, we can return success
    if (returnOK)
      return (true);

    // remove filename from buf
    *s = '\0';

    // check our temp dir; if it's within that, both read and write are permitted.
    if (_stricmp(m_TempPathString.c_str(), buf) == 0)
      return (true);

    n = GetLongPathName(buf, buf, _MAX_PATH);
    if ((n == 0) || (n > _MAX_PATH))
    {
      if (GetLastError() != ERROR_PATH_NOT_FOUND)
      {
        // TODO: issue appropriate error message
        return (false) ;
      }
      // if the path is not found we just continue (this is by design)
    }

    Path fullPath(buf);

    if (isWrite)
    {
      // we do special-case hard-coded exclusion test(s) here
      for (IOPathVector::const_iterator it = GetExcludedPaths().begin(); it != GetExcludedPaths().end(); it++)
      {
        if (TestPath(it->GetPath(), fullPath, it->IsRecursive()))
        {
          // TODO: issue appropriate error message
          return (false) ;
        }
      }
    }
    else
    {
      // it's a read
      for (IOPathVector::const_iterator it = GetReadPaths().begin(); it != GetReadPaths().end(); it++)
        if (TestPath(it->GetPath(), fullPath, it->IsRecursive()))
          return (true) ;

      // access for write implies access for read (this is by design).
      // so we check the write specs below instead of giving an error here.
    }

    for (IOPathVector::const_iterator it = GetWritePaths().begin(); it != GetWritePaths().end(); it++)
      if (TestPath(it->GetPath(), fullPath, it->IsRecursive()))
        return (true) ;

    // check to see if the file is in the scene source directory. if the scene source name
    // is not yet known, use the current working directory.
    if (m_InputFilename.empty() == true)
    {
      if (_getcwd(buf, _MAX_PATH - 2) == nullptr)
      {
        // TODO: issue appropriate error message
        return (false) ;
      }
    }
    else
      n = GetFullPathName (UCS2toSysString(m_InputFilename).c_str(), _MAX_PATH, buf, &s);
    if ((n == 0) || (n > _MAX_PATH) || (s == nullptr) || (s == buf))
    {
      // TODO: issue appropriate error message
      return (false) ;
    }

    // get the containing directory. we have to ensure that we don't test for file access here
    // as the source filename may be incomplete (e.g. 'c:\temp\test' for c:\temp\test.pov).
    _splitpath(buf, drive, dir, nullptr, nullptr);
    sprintf(buf, "%s%s", drive, dir);
    n = GetLongPathName(buf, buf, _MAX_PATH) ;
    if ((n == 0) || (n > _MAX_PATH))
    {
      if (GetLastError() == ERROR_PATH_NOT_FOUND || GetLastError() == ERROR_FILE_NOT_FOUND)
        throw POV_EXCEPTION(kCannotOpenFileErr, std::string("Input folder '") + buf + "' not found; cannot determine I/O permission for write.");

      // TODO: issue appropriate error message
      return (false) ;
    }
    // allow recursive test for read, but non-recursive for write
    if (TestPath(Path(buf), fullPath, !isWrite) == true)
      return (true);

    // TODO: issue error message and prompt for temporary access
    return (false);
  }

  /////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////

  WinShelloutProcessing::WinShelloutProcessing(POVMS_Object& opts, const std::string& scene, unsigned int width, unsigned int height): ShelloutProcessing(opts, scene, width, height)
  {
    m_ProcessRunning = false;
    m_ProcessId = m_LastError = m_ExitCode = 0;
    m_ProcessHandle = m_ThreadHandle = nullptr;

    // we need to re-init the actions so they will call our instance of ExtractCommand
    // this isn't necessary on platforms that don't implement a custom ExtractCommand
    shellouts[preFrame].reset(new ShelloutAction(this, kPOVAttrib_PreFrameCommand, opts));
    shellouts[postFrame].reset(new ShelloutAction(this, kPOVAttrib_PostFrameCommand, opts));
    shellouts[preScene].reset(new ShelloutAction(this, kPOVAttrib_PreSceneCommand, opts));
    shellouts[postScene].reset(new ShelloutAction(this, kPOVAttrib_PostSceneCommand, opts));
    shellouts[userAbort].reset(new ShelloutAction(this, kPOVAttrib_UserAbortCommand, opts));
    shellouts[fatalError].reset(new ShelloutAction(this, kPOVAttrib_FatalErrorCommand, opts));
  }

  WinShelloutProcessing::~WinShelloutProcessing()
  {
    if (WinShelloutProcessing::CommandRunning())
    {
      KillShellouts(1, true);
      CollectCommand();
    }
  }

  // in the windows version of ExtractCommand, we don't treat backslashes in the command itself specially
  bool WinShelloutProcessing::ExtractCommand(const std::string& src, std::string& command, std::string& parameters) const
  {
    bool inSQ = false;
    bool inDQ = false;
    bool treatAsPath = false;
    const char *s;

    command.clear();
    parameters.clear();
    std::string str = boost::trim_copy(src);
    std::string tmp = boost::to_lower_copy(str);

    for (s = str.c_str(); *s != '\0'; s++)
    {
      if (*s == '"')
        inDQ = !inDQ;
      else if (*s == '\'')
        inSQ = !inSQ;
      else if (isspace(*s) && !inSQ && !inDQ)
        break;
      command.push_back(*s);
    }

    boost::trim(command);
    if (command.empty())
      return false;

    if (command.size() > 1)
    {
      char ch1 = command[0];
      char ch2 = command[command.size() - 1];
      if ((ch1 == '\'' || ch1 == '"') && ch1 == ch2)
      {
        command = boost::trim_copy(command.substr(1, command.size() - 2));
        if (command.empty())
          return false;
      }
    }

    parameters = boost::trim_copy(std::string(s));
    return true;
  }

  bool WinShelloutProcessing::ExecuteCommand(const std::string& cmd, const std::string& params)
  {
    STARTUPINFO         startupInfo;
    PROCESS_INFORMATION procInfo;

    if (WinShelloutProcessing::CommandRunning())
      throw POV_EXCEPTION(kNotNowErr, "Cannot create new process as previous shellout has not terminated");

    CollectCommand();
    m_Command = cmd;
    m_Params = params;
    m_LastError = m_ExitCode = 0;
    memset(&procInfo, 0, sizeof(procInfo));
    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof (startupInfo);

    // this may change during a render, so we check it in real time
    startupInfo.dwFlags = pov_frontend::MinimizeShellouts() ? STARTF_USESHOWWINDOW : 0;
    startupInfo.wShowWindow = SW_SHOWMINNOACTIVE;

    std::shared_ptr<char> buf(new char[params.size() + 1]);
    strcpy(buf.get(), params.c_str());
    if ((m_ProcessRunning = CreateProcess(cmd.c_str(), buf.get(), nullptr, nullptr, false, 0, nullptr, nullptr, &startupInfo, &procInfo)))
    {
      m_ProcessHandle = procInfo.hProcess;
      m_ThreadHandle = procInfo.hThread;
      m_ProcessId = procInfo.dwProcessId;
    }
    else
      m_LastError = GetLastError();
    return m_ProcessRunning;
  }

  bool WinShelloutProcessing::CommandRunning(void)
  {
    if (!m_ProcessRunning)
      return false;
    return WaitForSingleObject(m_ProcessHandle, 0) == WAIT_TIMEOUT;
  }

  int WinShelloutProcessing::ProcessID(void)
  {
    if (!WinShelloutProcessing::CommandRunning())
      return 0;
    return m_ProcessId;
  }

  bool WinShelloutProcessing::KillCommand(int timeout, bool force)
  {
    if (!WinShelloutProcessing::CommandRunning())
      return true;
    TerminateProcess(m_ProcessHandle, 1);
    return WaitForSingleObject(m_ProcessHandle, timeout * 1000) != WAIT_TIMEOUT;
  }

  int WinShelloutProcessing::CollectCommand(void)
  {
    if (m_ProcessRunning == false)
      return -2;
    if (WinShelloutProcessing::CommandRunning())
      return -1;
    GetExitCodeProcess(m_ProcessHandle, &m_ExitCode);
    CloseHandle (m_ProcessHandle) ;
    CloseHandle (m_ThreadHandle) ;
    m_ProcessRunning = false;
    return m_ExitCode;
  }

  int WinShelloutProcessing::CollectCommand(std::string& output)
  {
    if (m_ProcessRunning == false && m_LastError != 0)
    {
      char    *buffer ;

      output = "Error: failed to run command '" + m_Command + "' - ";
      FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                     nullptr,
                     m_LastError,
                     MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US),
                     reinterpret_cast<char *>(&buffer),
                     0,
                     nullptr);
      output += buffer;
      LocalFree (buffer);
      return 1;
    }
    return CollectCommand();
  }

  bool WinShelloutProcessing::CommandPermitted(const std::string& command, const std::string& parameters)
  {
    // this may change during a render, so we check it in real time
    return pov_frontend::ShelloutsPermitted();
  }
}
// end of namespace vfePlatform
