//******************************************************************************
///
/// @file vfe/unix/vfeplatform.h
///
/// Defines a *nix platform-specific session class derived from vfeSession.
///
/// Based on @ref vfe/win/vfeplatform.h.
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

#ifndef POVRAY_VFE_UNIX_VFEPLATFORM_H
#define POVRAY_VFE_UNIX_VFEPLATFORM_H

#include <memory>

#include "base/path_fwd.h"
#include "base/stringtypes.h"
#include "base/filesystem_fwd.h"

#include "frontend/shelloutprocessing.h"

#include "vfesession.h"

namespace vfePlatform
{
    using namespace pov_base;
    using namespace vfe;

    class UnixOptionsProcessor;

    class UnixShelloutProcessing: public pov_frontend::ShelloutProcessing
    {
        public:
            UnixShelloutProcessing(POVMS_Object& opts, const std::string& scene, unsigned int width, unsigned int height);
            virtual ~UnixShelloutProcessing() override;

            virtual int ProcessID(void) override;
            virtual bool ShelloutsSupported(void) override { return true; }

        protected:
            virtual bool ExecuteCommand(const std::string& cmd, const std::string& params) override;
            virtual bool KillCommand(int timeout, bool force = false) override;
            virtual bool CommandRunning(void) override;
            virtual int CollectCommand(std::string& output) override;
            virtual int CollectCommand(void) override;
            virtual bool CommandPermitted(const std::string& command, const std::string& parameters) override;

            bool m_ProcessRunning;
            std::string m_Command;
            std::string m_Params;
            unsigned long m_ExitCode;
            unsigned long m_LastError;
            unsigned long m_ProcessId;

        private:

            UnixShelloutProcessing() = delete;
    };

    ///////////////////////////////////////////////////////////////////////
    // most of the methods in vfeUnixSession are derived from vfeSession.
    // see vfeSession for documentation for those cases.
    class vfeUnixSession : public vfeSession
    {
        public:
            vfeUnixSession(int id = 0);
            virtual ~vfeUnixSession() override {}

            virtual UCS2String GetTemporaryPath(void) const override;
            virtual UCS2String CreateTemporaryFile(void) const override;
            virtual void DeleteTemporaryFile(const UCS2String& filename) const override;
            virtual POV_LONG GetTimestamp(void) const override;
            virtual void NotifyCriticalError(const char *message, const char *file, int line) override;
            virtual int RequestNewOutputPath(int CallCount, const std::string& Reason, const UCS2String& OldPath, UCS2String& NewPath) override;
            virtual bool TestAccessAllowed(const Path& file, bool isWrite) const override;
            virtual ShelloutProcessing *CreateShelloutProcessing(POVMS_Object& opts, const std::string& scene, unsigned int width, unsigned int height) override
                { return new UnixShelloutProcessing(opts, scene, width, height); }

            std::shared_ptr<UnixOptionsProcessor> GetUnixOptions(void) { return m_OptionsProc; }

        protected:
            virtual void WorkerThreadStartup() override;
            virtual void WorkerThreadShutdown() override;

            ///////////////////////////////////////////////////////////////////////
            // return true if the path component of file is equal to the path component
            // of path. will also return true if recursive is true and path is a parent
            // of file. does not support relative paths, and will
            // perform case-sensitive comparisons.
            virtual bool TestPath(const Path& path, const Path& file, bool recursive) const;

            ///////////////////////////////////////////////////////////////////////
            // perform case-sensitive UCS2 string comparison. does not take code-
            // page into account.
            virtual bool StrCompare (const UCS2String& lhs, const UCS2String& rhs) const;

            ///////////////////////////////////////////////////////////////////////
            // used to detect wall clock changes to prevent GetTimeStamp()
            // returning a value less than that of a previous call during the
            // current session.
            mutable POV_LONG m_LastTimestamp;
            mutable POV_LONG m_TimestampOffset;

            // platform specific configuration options
            std::shared_ptr<UnixOptionsProcessor> m_OptionsProc;

            // Temporary files to be deleted when session closes.
            mutable std::vector<pov_base::Filesystem::TemporaryFilePtr> m_TempFiles;
    } ;

    ///////////////////////////////////////////////////////////////////////
    // return a number that uniquely identifies the calling thread amongst
    // all other running threads in the process (and preferably in the OS).
    POVMS_Sys_Thread_Type GetThreadId();
}
// end of namespace vfePlatform

#endif // POVRAY_VFE_UNIX_VFEPLATFORM_H
