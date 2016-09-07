//******************************************************************************
///
/// @file vfe/win/vfeplatform.h
///
/// Defines a platform-specific session class derived from vfeSession.
///
/// @author Christopher J. Cason
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef __VFEPLATFORM_H__
#define __VFEPLATFORM_H__

#include "frontend/shelloutprocessing.h"
#include "vfesession.h"

namespace vfePlatform
{
  using namespace vfe;

  class WinShelloutProcessing: public pov_frontend::ShelloutProcessing
  {
  public:
    WinShelloutProcessing(POVMS_Object& opts, const string& scene, unsigned int width, unsigned int height);
    ~WinShelloutProcessing();

    virtual int ProcessID(void);
    virtual bool ShelloutsSupported(void) { return true; }

  protected:
    virtual bool ExecuteCommand(const string& cmd, const string& params);
    virtual bool KillCommand(int timeout, bool force = false);
    virtual bool CommandRunning(void);
    virtual int CollectCommand(string& output);
    virtual int CollectCommand(void);
    virtual bool CommandPermitted(const string& command, const string& parameters);
    virtual bool ExtractCommand(const string& src, string& command, string& parameters) const;

    bool m_ProcessRunning;
    string m_Command;
    string m_Params;
    void *m_ProcessHandle;
    void *m_ThreadHandle;
    unsigned long m_ExitCode;
    unsigned long m_LastError;
    unsigned long m_ProcessId;

  private:
    WinShelloutProcessing();
  };

  ///////////////////////////////////////////////////////////////////////
  // most of the methods in vfeWinSession are derived from vfeSession.
  // see vfeSession for documentation for those cases.
  class vfeWinSession : public vfeSession
  {
    public:

      typedef std::set<string> FilenameSet;

      vfeWinSession(int id = 0);
      virtual ~vfeWinSession();

      virtual UCS2String GetTemporaryPath(void) const;
      virtual UCS2String CreateTemporaryFile(void) const;
      virtual void DeleteTemporaryFile(const UCS2String& filename) const;
      virtual POV_LONG GetTimestamp(void) const ;
      virtual void NotifyCriticalError(const char *message, const char *file, int line);
      virtual int RequestNewOutputPath(int CallCount, const string& Reason, const UCS2String& OldPath, UCS2String& NewPath);
      virtual bool TestAccessAllowed(const Path& file, bool isWrite) const;
      virtual bool ImageOutputToStdoutSupported(void) const { return m_OptimizeForConsoleOutput; }
      virtual ShelloutProcessing *CreateShelloutProcessing(POVMS_Object& opts, const string& scene, unsigned int width, unsigned int height) { return new WinShelloutProcessing(opts, scene, width, height); }

      virtual void Clear(bool Notify = true);

      const FilenameSet& GetReadFiles(void) const { return m_ReadFiles; }
      const FilenameSet& GetWriteFiles(void) const { return m_WriteFiles; }

    protected:
      virtual void WorkerThreadStartup();
      virtual void WorkerThreadShutdown();

      ///////////////////////////////////////////////////////////////////////
      // return true if the path component of file is equal to the path component
      // of path. will also return true if recursive is true and path is a parent
      // of file. does not support relative paths, and will convert UCS2 paths to
      // ASCII and perform case-insensitive comparisons.
      virtual bool TestPath(const Path& path, const Path& file, bool recursive) const;

      ///////////////////////////////////////////////////////////////////////
      // perform case-insensitive UCS2 string comparison. does not take code-
      // page into account.
      virtual bool StrCompareIC (const UCS2String& lhs, const UCS2String& rhs) const;

      ///////////////////////////////////////////////////////////////////////
      // used to detect wall clock changes to prevent GetTimeStamp()
      // returning a value less than that of a previous call during the
      // current session.
      mutable __int64 m_LastTimestamp;
      mutable __int64 m_TimestampOffset;

      ////////////////////////////////////////////////////////////////////
      // used to store the location of the temp path. this is used by both
      // GetTemporaryPath() and TestAccessAllowed().
      Path m_TempPath;
      string m_TempPathString;
      mutable vector<string> m_TempFilenames;
      mutable FilenameSet m_ReadFiles;
      mutable FilenameSet m_WriteFiles;
  } ;

  ///////////////////////////////////////////////////////////////////////
  // return a number that uniquely identifies the calling thread amongst
  // all other running threads in the process (and preferably in the OS).
  POVMS_Sys_Thread_Type GetThreadId();
}

#endif
