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

#ifndef POVRAY_BASE_PLATFORMBASE_H
#define POVRAY_BASE_PLATFORMBASE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of C standard header files
#include <cstdio>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/fileinputoutput_fwd.h"
#include "base/stringtypes.h"

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBasePlatformbase Platform-Specific Services Interface
/// @ingroup PovPlatform
/// @ingroup PovBase
///
/// @{

/// Abstract class defining an interface to platform-specific services.
///
/// @note
///     Only one instance of this class (or any subclass thereof) shall ever exist in any process.
///
class PlatformBase
{
public:

    PlatformBase();
    virtual ~PlatformBase();

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

    static PlatformBase& GetInstance();

private:

    static PlatformBase *self;
};

/// Default implementation of @ref PlatformBase.
///
class DefaultPlatformBase final : public PlatformBase
{
public:
    DefaultPlatformBase();
    virtual ~DefaultPlatformBase() override;

    virtual UCS2String GetTemporaryPath() override;
    virtual UCS2String CreateTemporaryFile() override;
    virtual void DeleteTemporaryFile(const UCS2String& filename) override;

    virtual bool ReadFileFromURL(OStream *file, const UCS2String& url, const UCS2String& referrer = UCS2String()) override;

    /// @note
    ///     This implementation only supports ASCII filenames.
    virtual FILE* OpenLocalFile(const UCS2String& name, const char *mode) override;

    /// @note
    ///     This implementation grants unrestricted access to any file.
    virtual bool AllowLocalFileAccess(const UCS2String& name, const unsigned int fileType, bool write) override;
};

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_PLATFORMBASE_H
