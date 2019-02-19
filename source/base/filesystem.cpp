//******************************************************************************
///
/// @file base/filesystem.cpp
///
/// Implementation of file system services (default implementation).
///
/// @note
///     This is a default implementation, provided primarily as a template for
///     creating platform-specific implementations.
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

// C++ variants of C standard header files
#if POV_USE_DEFAULT_DELETEFILE
#include <cstdio>
#endif

// C++ standard header files
#if POV_USE_DEFAULT_LARGEFILE
#include <fstream>
#include <ios>
#include <limits>
#endif
#if POV_USE_DEFAULT_TEMPORARYFILE
#include <atomic>
#endif

// POV-Ray header files (base module)
#if POV_USE_DEFAULT_DELETEFILE || POV_USE_DEFAULT_LARGEFILE || POV_USE_DEFAULT_TEMPORARYFILE
#include "base/stringutilities.h"
#endif

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

namespace Filesystem
{

//******************************************************************************

#if POV_USE_DEFAULT_DELETEFILE

bool DeleteFile(const UCS2String& fileName)
{
    // Note: The C++ standard does not specify whether `remove` will or will not
    // delete directories. On POSIX systems it will, which is not ideal for our
    // purposes.
    return (std::remove(UCS2toSysString(fileName).c_str()) == 0);
}

#endif // POV_USE_DEFAULT_DELETEFILE

//******************************************************************************

#if POV_USE_DEFAULT_LARGEFILE

using Offset = std::streamoff;

static_assert(
    std::numeric_limits<Offset>::digits > 32,
    "Large files (> 2 GiB) not supported, limiting image size to approx. 100 Megapixels. Proceed at your own risk."
    );

struct LargeFile::Data final
{
    std::fstream stream;
};

LargeFile::LargeFile() :
    mpData(new Data)
{
    // Disable stream buffering.
    mpData->stream.rdbuf()->pubsetbuf(nullptr, 0);
}

LargeFile::~LargeFile()
{
    Close();
}

bool LargeFile::CreateRW(const UCS2String& fileName)
{
    mpData->stream.open(UCS2toSysString(fileName),
                        std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
    return mpData->stream.is_open();
}

bool LargeFile::Seek(std::int_least64_t offset)
{
    if (!mpData->stream.is_open() || mpData->stream.bad())
        return false;
    if ((offset < 0) || (offset > std::numeric_limits<Offset>::max()))
        return false;
    return (mpData->stream.rdbuf()->pubseekpos(Offset(offset),
                                               std::ios_base::in | std::ios_base::out) == offset);
}

std::size_t LargeFile::Read(void* data, std::size_t maxSize)
{
    if (!mpData->stream.is_open() || mpData->stream.bad())
        return false;
    return mpData->stream.read(reinterpret_cast<char*>(data), maxSize).gcount();
}

bool LargeFile::Write(const void* data, std::size_t size)
{
    if (!mpData->stream.is_open() || mpData->stream.bad())
        return false;
    return !mpData->stream.write(reinterpret_cast<const char*>(data), size).bad();
}

void LargeFile::Close()
{
    if (mpData->stream.is_open())
        mpData->stream.close();
}

#endif // POV_USE_DEFAULT_LARGEFILE

//******************************************************************************

TemporaryFile::TemporaryFile() :
    mFileName(SuggestName())
{}

TemporaryFile::TemporaryFile(const UCS2String& name) :
    mFileName(name)
{}

TemporaryFile::~TemporaryFile()
{
    Delete();
}

const UCS2String& TemporaryFile::GetFileName() const
{
    return mFileName;
}

void TemporaryFile::Keep()
{
    mFileName.clear();
}

void TemporaryFile::Delete()
{
    if (!mFileName.empty())
    {
        (void)DeleteFile(mFileName);
        mFileName.clear();
    }
}

#if POV_USE_DEFAULT_TEMPORARYFILE

UCS2String TemporaryFile::SuggestName()
{
    static std::atomic_int globalIndex = 0;
    int index = globalIndex++;

    char buffer[32];
    sprintf(buffer, "/tmp/pov%08x.dat", index);

    return ASCIItoUCS2String(buffer);
}

#endif // POV_USE_DEFAULT_TEMPORARYFILE

//******************************************************************************

}
// end of namespace Filesystem

}
// end of namespace pov_base
