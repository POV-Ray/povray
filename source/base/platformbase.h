//******************************************************************************
///
/// @file base/platformbase.h
///
/// Declarations related to the @ref pov_base::PlatformBase class.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_BASE_PLATFORMBASE_H
#define POVRAY_BASE_PLATFORMBASE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// Standard C++ header files
#include <string>

// POV-Ray base header files
#include "base/fileinputoutput.h"
#include "base/stringutilities.h"
#include "base/types.h"

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBasePlatformbase Platform-Specific Services Interface
/// @ingroup PovPlatform
/// @ingroup PovBase
///
/// @{

#define POV_PLATFORM_BASE PlatformBase::GetPlatformBaseReference()

/// Abstract class defining an interface to platform-specific services.
///
/// @note
///     Only one instance of this class (or any subclass thereof) shall ever exist in any process.
///
class PlatformBase
{
public:

    PlatformBase() { POV_ASSERT(!self); self = this; };
    virtual ~PlatformBase() { self = nullptr; };

    virtual UCS2String GetTemporaryPath() = 0;
    virtual UCS2String CreateTemporaryFile() = 0;
    virtual void DeleteTemporaryFile(const UCS2String& filename) = 0;

    virtual bool ReadFileFromURL(OStream *file, const UCS2String& url, const UCS2String& referrer = UCS2String()) = 0;

    /// Open a file for reading.
    ///
    /// This method opens a file on the local file system, returning a `FILE*`. The parameters correspond to `fopen()`,
    /// except that the file name may be a Unicode string.
    ///
    virtual FILE* OpenLocalFile (const UCS2String& name, const char *mode) = 0;

    /// Delete a file.
    ///
    /// This method removes a file from the local file system.
    ///
    virtual void DeleteLocalFile (const UCS2String& name) = 0;

    /// Check whether a file is allowed to be accessed.
    ///
    /// This method shall determine whether a file on the local file system may be accessed for reading, or created
    /// and/or accessed for writing, as configured by the user via a proprietary mechanism provided by the POV-Ray
    /// user interface.
    ///
    /// @note
    ///     This method does _not_ test whether the user has the required access rights at file system level.
    ///
    /// @param[in]  name        Name of the file to be accessed.
    /// @param[in]  fileType    Type of the file to be accessed (one of `POV_File_*` as defined in
    ///                         @ref fileinputoutput.h)
    /// @param[in]  write       `true` to check for full (read and write) access, `false` to check for read access only.
    /// @return                 `true` if access is granted, `false` otherwise.
    ///
    virtual bool AllowLocalFileAccess (const UCS2String& name, const unsigned int fileType, bool write) = 0;

    static PlatformBase& GetInstance() { return *self; };

private:

    static PlatformBase *self;
};

/// Default implementation of @ref PlatformBase.
///
class DefaultPlatformBase : public PlatformBase
{
public:
    DefaultPlatformBase();
    ~DefaultPlatformBase();

    virtual UCS2String GetTemporaryPath();
    virtual UCS2String CreateTemporaryFile();
    virtual void DeleteTemporaryFile(const UCS2String& filename);

    virtual bool ReadFileFromURL(OStream *file, const UCS2String& url, const UCS2String& referrer = UCS2String());

    /// @note
    ///     This implementation only supports ASCII filenames.
    virtual FILE* OpenLocalFile(const UCS2String& name, const char *mode);

    /// @note
    ///     This implementation only supports ASCII filenames.
    virtual void DeleteLocalFile(const UCS2String& name);

    /// @note
    ///     This implementation grants unrestricted access to any file.
    virtual bool AllowLocalFileAccess(const UCS2String& name, const unsigned int fileType, bool write);
};

/// @}
///
//##############################################################################

}

#endif // POVRAY_BASE_PLATFORMBASE_H
