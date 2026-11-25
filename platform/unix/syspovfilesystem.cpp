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
#include <cerrno>

// C++ standard header files
#include <limits>
#include <system_error>

// POSIX standard header files
#include <fcntl.h>
#if !POV_USE_DEFAULT_MAPPEDFILE
    #include <sys/mman.h>
#endif
#include <sys/stat.h>
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

#if !POV_USE_DEFAULT_MAPPEDFILE

struct MappedFile::Data final
{
    void* data;
    std::size_t size;
    std::string errorText;
    Data() : data(nullptr), size(0) {}
};

static std::string ErrnoText(const char* what, int err)
{
    return std::string(what) + ": " + std::generic_category().message(err);
}

MappedFile::MappedFile() :
    mpData(new Data)
{}

MappedFile::~MappedFile()
{
    Close();
}

bool MappedFile::Open(const UCS2String& fileName, std::uint_least64_t size)
{
    Close();
    mpData->errorText.clear();

    if (size == 0)
    {
        mpData->errorText = "cannot map an empty file";
        return false;
    }
    if ((size > std::numeric_limits<std::size_t>::max()) ||
        (size > std::uint_least64_t(std::numeric_limits<off_t>::max())))
    {
        mpData->errorText = "file too large for this build";
        return false;
    }

    const std::string name = UCS2toSysString(fileName);

    // O_NOCTTY and O_NONBLOCK make opening a device or FIFO harmless; such files
    // are refused below.
    int openFlags = O_RDWR | O_NOCTTY | O_NONBLOCK;
#ifdef O_CLOEXEC
    openFlags |= O_CLOEXEC; // Don't leak the descriptor into shell-out commands.
#endif
    // Note whether we create the file, so that a failure can remove it again.
    bool created = false;
    int handle = open(name.c_str(), openFlags);
    if ((handle == -1) && (errno == ENOENT))
    {
        handle = open(name.c_str(), openFlags | O_CREAT | O_EXCL,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
        if (handle != -1)
            created = true;
        else if (errno == EEXIST)
            handle = open(name.c_str(), openFlags); // Someone else was faster.
    }
    if (handle == -1)
    {
        mpData->errorText = ErrnoText("cannot open file", errno);
        return false;
    }

    struct stat info;
    if (fstat(handle, &info) != 0)
    {
        mpData->errorText = ErrnoText("cannot query file", errno);
        close(handle);
        if (created)
            (void)unlink(name.c_str());
        return false;
    }
    if (!S_ISREG(info.st_mode))
    {
        mpData->errorText = "not a regular file";
        close(handle);
        return false;
    }
    const off_t originalSize = info.st_size;
    const bool grow = (std::uint_least64_t(originalSize) < size);

    // Reserve the disk space for the whole mapped range now if we can, also when
    // the file is long enough already (it may be sparse): writing to a page that
    // has no disk space behind it on a full file system raises SIGBUS.
    int err = EINVAL;
#ifdef HAVE_POSIX_FALLOCATE
    err = posix_fallocate(handle, 0, off_t(size));
#endif
    if ((err == EINVAL) || (err == EOPNOTSUPP))
    {
        // No reservation possible; just set the size.
        err = 0;
        if (grow && (ftruncate(handle, off_t(size)) != 0))
            err = errno;
    }
    void* data = MAP_FAILED;
    if (err != 0)
        mpData->errorText = ErrnoText("cannot set file size", err);
    else
    {
        data = mmap(nullptr, std::size_t(size), PROT_READ | PROT_WRITE, MAP_SHARED, handle, 0);
        if (data == MAP_FAILED)
            mpData->errorText = ErrnoText("cannot map file", errno);
    }

    if (data == MAP_FAILED)
    {
        // Leave no trace of the attempt: remove a file we created, and give a
        // file we grew its old size back (releasing the disk space reserved).
        if (created)
            (void)unlink(name.c_str());
        else if (grow && (ftruncate(handle, originalSize) != 0))
            {} // Nothing more we can do; the original error is the one to report.
        close(handle);
        return false;
    }

    // The mapping keeps its own reference to the file.
    close(handle);

    mpData->data = data;
    mpData->size = std::size_t(size);
    return true;
}

void MappedFile::Close()
{
    if (mpData->data != nullptr)
    {
        (void)munmap(mpData->data, mpData->size);
        mpData->data = nullptr;
        mpData->size = 0;
    }
}

void* MappedFile::GetData() const
{
    return mpData->data;
}

std::size_t MappedFile::GetSize() const
{
    return mpData->size;
}

const std::string& MappedFile::GetErrorText() const
{
    return mpData->errorText;
}

#endif // POV_USE_DEFAULT_MAPPEDFILE

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
