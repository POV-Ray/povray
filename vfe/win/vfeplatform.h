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

#ifndef POVRAY_VFE_WIN_VFEPLATFORM_H
#define POVRAY_VFE_WIN_VFEPLATFORM_H

#include <vector>

#include "base/stringtypes.h"
#include "base/filesystem_fwd.h"

#include "frontend/shelloutprocessing.h"
#include "vfesession.h"

namespace vfePlatform
{
  using namespace vfe;

  class WinShelloutProcessing final : public pov_frontend::ShelloutProcessing
  {
  public:
    WinShelloutProcessing(POVMS_Object& opts, const std::string& scene, unsigned int width, unsigned int height);
    virtual ~WinShelloutProcessing() override;

    virtual int ProcessID(void) override;
    virtual bool ShelloutsSupported(void) override { return true; }

  protected:
    virtual bool ExecuteCommand(const std::string& cmd, const std::string& params) override;
    virtual bool KillCommand(int timeout, bool force = false) override;
    virtual bool CommandRunning(void) override;
    virtual int CollectCommand(std::string& output) override;
    virtual int CollectCommand(void) override;
    virtual bool CommandPermitted(const std::string& command, const std::string& parameters) override;
    virtual bool ExtractCommand(const std::string& src, std::string& command, std::string& parameters) const override;

    bool m_ProcessRunning;
    std::string m_Command;
    std::string m_Params;
    void *m_ProcessHandle;
    void *m_ThreadHandle;
    unsigned long m_ExitCode;
    unsigned long m_LastError;
    unsigned long m_ProcessId;

  private:

    WinShelloutProcessing() = delete;
  };

  ///////////////////////////////////////////////////////////////////////
  // most of the methods in vfeWinSession are derived from vfeSession.
  // see vfeSession for documentation for those cases.
  class vfeWinSession final : public vfeSession
  {
    public:

      typedef std::set<std::string> FilenameSet;

      vfeWinSession(int id = 0);

      virtual UCS2String GetTemporaryPath(void) const override;
      virtual UCS2String CreateTemporaryFile(void) const override;
      virtual void DeleteTemporaryFile(const UCS2String& filename) const override;
      virtual POV_LONG GetTimestamp(void) const override;
      virtual void NotifyCriticalError(const char *message, const char *file, int line) override;
      virtual int RequestNewOutputPath(int CallCount, const std::string& Reason, const UCS2String& OldPath, UCS2String& NewPath) override;
      virtual bool TestAccessAllowed(const Path& file, bool isWrite) const override;
      virtual bool ImageOutputToStdoutSupported(void) const override { return m_OptimizeForConsoleOutput; }
      virtual ShelloutProcessing *CreateShelloutProcessing(POVMS_Object& opts, const std::string& scene, unsigned int width, unsigned int height) override { return new WinShelloutProcessing(opts, scene, width, height); }

      virtual void Clear(bool Notify = true) override;

      const FilenameSet& GetReadFiles(void) const { return m_ReadFiles; }
      const FilenameSet& GetWriteFiles(void) const { return m_WriteFiles; }

    protected:
      virtual void WorkerThreadStartup() override;
      virtual void WorkerThreadShutdown() override;

      ///////////////////////////////////////////////////////////////////////
      // return true if the path component of file is equal to the path component
      // of path. will also return true if recursive is true and path is a parent
      // of file. does not support relative paths, and will convert UCS2 paths to
      // Latin-1 and perform case-insensitive comparisons.
      bool TestPath(const Path& path, const Path& file, bool recursive) const;

      ///////////////////////////////////////////////////////////////////////
      // perform case-insensitive UCS2 string comparison. does not take code-
      // page into account.
      bool StrCompareIC (const UCS2String& lhs, const UCS2String& rhs) const;

      ///////////////////////////////////////////////////////////////////////
      // used to detect wall clock changes to prevent GetTimeStamp()
      // returning a value less than that of a previous call during the
      // current session.
      mutable __int64 m_LastTimestamp;
      mutable __int64 m_TimestampOffset;

      ////////////////////////////////////////////////////////////////////
      // used to store the location of the temp path. this is used by both
      // GetTemporaryPath() and TestAccessAllowed().
      std::string m_TempPathString;
      mutable std::vector<pov_base::Filesystem::TemporaryFilePtr> m_TempFiles;
      mutable FilenameSet m_ReadFiles;
      mutable FilenameSet m_WriteFiles;
  } ;

  ///////////////////////////////////////////////////////////////////////
  // return a number that uniquely identifies the calling thread amongst
  // all other running threads in the process (and preferably in the OS).
  POVMS_Sys_Thread_Type GetThreadId();
}
// end of namespace vfePlatform

#endif // POVRAY_VFE_WIN_VFEPLATFORM_H
