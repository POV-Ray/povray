//******************************************************************************
///
/// @file platform/windows/syspovfilesystem.cpp
///
/// Windows-specific implementation of file system services.
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
#include <cwchar>

// C++ standard header files
#include <limits>
#include <system_error>

// POSIX standard header files
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

// Windows API header files
#include <windows.h>
#include <winioctl.h>

// POV-Ray header files (base module)
#include "base/pov_err.h"
#include "base/stringutilities.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

namespace Filesystem
{

#ifdef DeleteFile
#undef DeleteFile // Shame on you, Windows!
#endif

// TODO - Consider using Windows API calls instead of POSIX-style functions
//        (e.g. `CreateFileW()` instead of `_open()` or `_wopen()`).

//******************************************************************************

#if !POV_USE_DEFAULT_DELETEFILE

bool DeleteFile(const UCS2String& fileName)
{
    // TODO - use `_wunlink()` instead.
    return (_unlink(UCS2toSysString(fileName).c_str()) == 0);
}

#endif // POV_USE_DEFAULT_DELETEFILE

//******************************************************************************

#if !POV_USE_DEFAULT_LARGEFILE

using Offset = decltype(_lseeki64(0,0,0));

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
    // TODO - use `_wopen()` instead.
    mpData->handle = _open(UCS2toSysString(fileName).c_str(),
                           _O_CREAT | _O_RDWR | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);
    return (mpData->handle != -1);
}

bool LargeFile::Seek(std::int_least64_t offset)
{
    if (mpData->handle == -1)
        return false;
    if ((offset < 0) || (offset > std::numeric_limits<Offset>::max()))
        return false;
    return (_lseeki64(mpData->handle, Offset(offset), SEEK_SET) == offset);
}

std::size_t LargeFile::Read(void* data, std::size_t maxSize)
{
    if (mpData->handle == -1)
        return false;
    return _read(mpData->handle, data, int(maxSize));
}

bool LargeFile::Write(const void* data, std::size_t size)
{
    if (mpData->handle == -1)
        return false;
    return (_write(mpData->handle, data, int(size)) == size);
}

void LargeFile::Close()
{
    if (mpData->handle != -1)
    {
        _close(mpData->handle);
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

static std::string WindowsErrorText(const char* what, DWORD err)
{
    return std::string(what) + ": " + std::system_category().message(int(err));
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
    if ((size > std::numeric_limits<SIZE_T>::max()) ||
        (size > std::uint_least64_t(std::numeric_limits<LONGLONG>::max())))
    {
        mpData->errorText = "file too large for this build";
        return false;
    }

    // The name is passed to the system exactly as `TestAccessAllowed()` checks it
    // (and as the output image is opened): as a narrow string. Opening the wide
    // name instead would let a name with characters outside Latin-1 be checked
    // as one path and opened as another.
    // TODO - use the wide name once the I/O restriction check works with it.
    const std::string name = UCS2toSysString(fileName);

    // Other processes must be able to open, map and eventually delete the file
    // while we have it open. The security flags stop a named pipe server from
    // acting on our behalf; such files are refused below anyway.
    HANDLE file = CreateFileA(name.c_str(),
                              GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL | SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE)
    {
        mpData->errorText = WindowsErrorText("cannot open file", GetLastError());
        return false;
    }
    // Note whether we created the file, so that a failure can remove it again.
    const bool existed = (GetLastError() == ERROR_ALREADY_EXISTS);
    const bool created = !existed && (GetFileType(file) == FILE_TYPE_DISK);

    LARGE_INTEGER originalSize;
    originalSize.QuadPart = 0;
    bool grow = false;
    DWORD err = ERROR_SUCCESS;
    const char* what = nullptr;
    void* data = nullptr;

    BY_HANDLE_FILE_INFORMATION info;
    if (GetFileType(file) != FILE_TYPE_DISK)
        mpData->errorText = "not a regular file";
    else if (!GetFileSizeEx(file, &originalSize) || !GetFileInformationByHandle(file, &info))
        what = "cannot query file";
    else
    {
        grow = (std::uint_least64_t(originalSize.QuadPart) < size);
        DWORD bytesReturned;
        FILE_SET_SPARSE_BUFFER notSparse;
        notSparse.SetSparse = FALSE;
        LARGE_INTEGER newSize;
        newSize.QuadPart = LONGLONG(size);
        // Writing to a page of a sparse file that has no disk space behind it on
        // a full disk raises an exception; making the file non-sparse allocates
        // the space, as growing a non-sparse file does.
        if ((info.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) &&
            !DeviceIoControl(file, FSCTL_SET_SPARSE, &notSparse, sizeof(notSparse), nullptr, 0, &bytesReturned, nullptr))
            what = "cannot allocate sparse file";
        else if (grow && (!SetFilePointerEx(file, newSize, nullptr, FILE_BEGIN) || !SetEndOfFile(file)))
            what = "cannot set file size";
        else
        {
            ULARGE_INTEGER mapSize;
            mapSize.QuadPart = size;
            HANDLE mapping = CreateFileMappingW(file, nullptr, PAGE_READWRITE, mapSize.HighPart, mapSize.LowPart, nullptr);
            if (mapping == nullptr)
                what = "cannot map file";
            else
            {
                data = MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, SIZE_T(size));
                if (data == nullptr)
                    what = "cannot map file";
                err = GetLastError();
                // The view keeps its own references to the mapping and the file.
                CloseHandle(mapping);
            }
        }
        if (what != nullptr && err == ERROR_SUCCESS)
            err = GetLastError();
    }
    if (what != nullptr)
        mpData->errorText = WindowsErrorText(what, err);

    if (data == nullptr)
    {
        // Leave no trace of the attempt: remove a file we created, and give a
        // file we grew its old size back.
        if (!created && grow)
        {
            if (SetFilePointerEx(file, originalSize, nullptr, FILE_BEGIN))
                (void)SetEndOfFile(file);
        }
        CloseHandle(file);
        if (created)
            (void)DeleteFileA(name.c_str());
        return false;
    }

    CloseHandle(file);

    mpData->data = data;
    mpData->size = std::size_t(size);
    return true;
}

void MappedFile::Close()
{
    if (mpData->data != nullptr)
    {
        (void)UnmapViewOfFile(mpData->data);
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

static std::wstring GetTempFilePath()
{
    static const auto maxTempPathLength = MAX_PATH - 14; // Leave space for the file names to be created.
    static const auto* subdir = L"povwin\\";
    static const auto subdirLength = std::wcslen(subdir);

    // Get a maximum(!) estimate of the buffer size required (excluding terminating NUL).
    auto bufferSizeRequired = GetTempPathW(0, nullptr);
    if (bufferSizeRequired == 0)
        throw POV_EXCEPTION(kUncategorizedError, "Could not get temp dir from Windows API");

    // Now get the actual temp directory path.
    std::wstring tempPath(bufferSizeRequired - 1, '\0');
    auto tempPathLength = GetTempPathW(bufferSizeRequired, const_cast<wchar_t*>(tempPath.c_str()));
    if (tempPathLength == 0)
        throw POV_EXCEPTION(kUncategorizedError, "Could not get temp dir from Windows API");
    if (tempPathLength >= bufferSizeRequired)
        throw POV_EXCEPTION(kUncategorizedError, "Could not get temp dir from Windows API");
    tempPath.resize(tempPathLength);

    // Try to create a dedicated sub-directory for POV-Ray.
    if (tempPathLength <= maxTempPathLength - subdirLength)
    {
        // Resulting path should be short enough for a sub-directory.
        tempPath += subdir;
        if ((CreateDirectoryW(tempPath.c_str(), nullptr) == 0) && (GetLastError() != ERROR_ALREADY_EXISTS))
            // We can't create the sub-directory, and it doesn't already exist either.
            // Well, tough luck - we'll just dump our stuff in the main temp directory then.
            // Roll back the addition of the sub-directory name.
            tempPath.resize(tempPathLength);
    }

    // That's it, we've officially chosen a directory to plonk our temporary files in.
    return tempPath;
}

UCS2String TemporaryFile::SuggestName()
{
    static const auto* prefix = L"pv";

    static std::wstring tempPath(GetTempFilePath());
    wchar_t buffer[MAX_PATH];
    if (GetTempFileNameW(tempPath.c_str(), prefix, 0, buffer) == 0)
        throw POV_EXCEPTION(kUncategorizedError, "Could not get temp file name from Windows API");
    return UCS2String(reinterpret_cast<UCS2*>(buffer)); // TODO - this is actually UTF-16, not UCS-2.
}

#endif // POV_USE_DEFAULT_TEMPORARYFILE

//******************************************************************************

}
// end of namespace Filesystem

}
// end of namespace pov_base
