//******************************************************************************
///
/// @file platform/unix/syspovfilesystem.cpp
///
/// Unix-specific implementation of file system services.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/filesystem.h"
#include "syspovfilesystem.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <limits>

// POSIX standard header files
#include <fcntl.h>
#include <unistd.h>

// POV-Ray header files (base module)
#include "base/stringutilities.h"

// this must be the last file included
#include "base/povassert.h"
#include "base/povdebug.h"

namespace pov_base
{

namespace Filesystem
{

//******************************************************************************

#if !POV_USE_DEFAULT_DELETEFILE

bool DeleteFile(const UCS2String& fileName)
{
    return (unlink(UCS2toSysString(fileName).c_str()) == 0);
}

#endif // POV_USE_DEFAULT_DELETEFILE

//******************************************************************************

#if !POV_USE_DEFAULT_LARGEFILE

#ifndef POVUNIX_LSEEK64
#define POVUNIX_LSEEK64(h,b,o) lseek(h,b,o)
#endif

using Offset = decltype(POVUNIX_LSEEK64(0,0,0));

static_assert(
    std::numeric_limits<Offset>::digits > 32,
    "Large files (> 2 GiB) not supported, limiting image size to approx. 100 Megapixels. Proceed at your own risk."
    );

struct LargeFile::Data final
{
    int handle;
    Data() : handle(-1) {}
};

LargeFile::LargeFile() :
    mpData(new Data)
{}

LargeFile::~LargeFile()
{
    Close();
}

bool LargeFile::CreateRW(const UCS2String& fileName)
{
    mpData->handle = open(UCS2toSysString(fileName).c_str(),
                          O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    return (mpData->handle != -1);
}

bool LargeFile::Seek(std::int_least64_t offset)
{
    if (mpData->handle == -1)
        return false;
    if ((offset < 0) || (offset > std::numeric_limits<Offset>::max()))
        return false;
    return (POVUNIX_LSEEK64(mpData->handle, Offset(offset), SEEK_SET) == offset);
}

std::size_t LargeFile::Read(void* data, std::size_t maxSize)
{
    if (mpData->handle == -1)
        return false;
    return read(mpData->handle, data, int(maxSize));
}

bool LargeFile::Write(const void* data, std::size_t size)
{
    if (mpData->handle == -1)
        return false;
    return (write(mpData->handle, data, int(size)) == size);
}

void LargeFile::Close()
{
    if (mpData->handle != -1)
    {
        close(mpData->handle);
        mpData->handle = -1;
    }
}

#endif // POV_USE_DEFAULT_LARGEFILE

//******************************************************************************

#if !POV_USE_DEFAULT_TEMPORARYFILE

static UCS2String gTempPath;

void SetTempFilePath(const UCS2String& tempPath)
{
    gTempPath = tempPath;
}

UCS2String TemporaryFile::SuggestName()
{
    POV_ASSERT(!gTempPath.empty());

    // TODO FIXME - This allows only one temporary file per process!
    // TODO FIXME - Avoid converting back and forth between UCS-2 and system-specific encoding.
    char str [POV_FILENAME_BUFFER_CHARS + 1] = "";
    std::snprintf(str, POV_FILENAME_BUFFER_CHARS + 1, "%spov%d", UCS2toSysString(gTempPath).c_str(), int(getpid()));
    return SysToUCS2String(str);
}

#endif // POV_USE_DEFAULT_TEMPORARYFILE

//******************************************************************************

}
// end of namespace Filesystem

}
// end of namespace pov_base
