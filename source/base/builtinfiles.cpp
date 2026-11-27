//******************************************************************************
///
/// @file base/builtinfiles.cpp
///
/// Implementations related to the built-in copies of the standard include files.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2021 Persistence of Vision Raytracer Pty. Ltd.
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
//------------------------------------------------------------------------------
// SPDX-License-Identifier: AGPL-3.0-or-later
//******************************************************************************

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/builtinfiles.h"

// C++ variants of standard C header files
#include <cerrno>
#include <cstdio>
#include <cstring>

// Standard C++ header files
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

// other library headers
#include <zlib.h>

// POV-Ray header files (base module)
#include "base/filesystem.h"
#include "base/fileutil.h"
#include "base/path.h"
#include "base/platformbase.h"
#include "base/pov_err.h"
#include "base/stringutilities.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

extern const char kBuiltinFilePrefix[] = "<builtin>/";

namespace
{

const size_t kBuiltinFilePrefixLength = sizeof(kBuiltinFilePrefix) - 1;

typedef std::vector<unsigned char>      BuiltinFileData;
typedef std::shared_ptr<BuiltinFileData> BuiltinFileDataPtr;

/// Inflated copies, by table entry. Several parsers may open files at once.
std::mutex gBuiltinFileCacheMutex;
std::map<const BuiltinFileEntry*, BuiltinFileDataPtr> gBuiltinFileCache;

/// Texts registered by RegisterBuiltinText(), by bare name.
std::mutex gBuiltinTextMutex;
std::map<std::string, const char*> gBuiltinTexts;

/// Convert a name to ASCII if it is plain ASCII with no directory part.
bool ToBareAsciiName(const UCS2String& name, size_t start, std::string& result)
{
    result.clear();
    if (name.size() <= start)
        return false;
    result.reserve(name.size() - start);
    for (size_t i = start; i < name.size(); ++i)
    {
        UCS2 c = name[i];
        if ((c == 0) || (c >= 0x80) || (c == '/') || (c == '\\') || (c == ':'))
            return false;
        result += char(c);
    }
    return true;
}

const char* FindBuiltinText(const UCS2String& path)
{
    std::string bare;
    if (!IsBuiltinFilePath(path) || !ToBareAsciiName(path, kBuiltinFilePrefixLength, bare))
        return nullptr;
    std::lock_guard<std::mutex> lock(gBuiltinTextMutex);
    std::map<std::string, const char*>::const_iterator i = gBuiltinTexts.find(bare);
    return (i == gBuiltinTexts.end()) ? nullptr : i->second;
}

const BuiltinFileEntry* FindBareAsciiName(const std::string& name)
{
    // the table is sorted by byte value, as strcmp() compares
    size_t lo = 0;
    size_t hi = kBuiltinFileCount;
    while (lo < hi)
    {
        size_t mid = lo + (hi - lo) / 2;
        int cmp = std::strcmp(name.c_str(), kBuiltinFiles[mid].name);
        if (cmp == 0)
            return &kBuiltinFiles[mid];
        if (cmp < 0)
            hi = mid;
        else
            lo = mid + 1;
    }
    return nullptr;
}

BuiltinFileDataPtr Inflate(const BuiltinFileEntry& entry)
{
    BuiltinFileDataPtr data(new BuiltinFileData(entry.size));
    uLongf size = uLongf(entry.size);
    int err = uncompress(data->data(), &size, entry.data, uLong(entry.deflatedSize));
    if ((err != Z_OK) || (size != entry.size))
        throw POV_EXCEPTION(kCannotOpenFileErr, std::string("Built-in file '") + kBuiltinFilePrefix + entry.name + "' is damaged.");
    return data;
}

BuiltinFileDataPtr GetData(const BuiltinFileEntry& entry)
{
    std::lock_guard<std::mutex> lock(gBuiltinFileCacheMutex);
    BuiltinFileDataPtr& data = gBuiltinFileCache[&entry];
    if (!data)
        data = Inflate(entry);
    return data;
}

/// Holds the data, so that it is set up before, and outlives, the @ref IMemStream base.
struct BuiltinFileDataHolder
{
    BuiltinFileDataPtr data;
    explicit BuiltinFileDataHolder(const BuiltinFileDataPtr& d) : data(d) {}
};

class IBuiltinFileStream : private BuiltinFileDataHolder, public IMemStream
{
    public:
        IBuiltinFileStream(const BuiltinFileDataPtr& d, const UCS2String& name) :
            BuiltinFileDataHolder(d),
            IMemStream(data->data(), data->size(), name)
        {}
};

}
// end of anonymous namespace

const BuiltinFileEntry* FindBuiltinFile(const UCS2String& name)
{
    std::string bare;
    if (!ToBareAsciiName(name, 0, bare))
        return nullptr;
    return FindBareAsciiName(bare);
}

bool IsBuiltinFilePath(const UCS2String& name)
{
    if (name.size() < kBuiltinFilePrefixLength)
        return false;
    for (size_t i = 0; i < kBuiltinFilePrefixLength; ++i)
        if (name[i] != UCS2(kBuiltinFilePrefix[i]))
            return false;
    return true;
}

const BuiltinFileEntry* FindBuiltinFileByPath(const UCS2String& path)
{
    std::string bare;
    if (!IsBuiltinFilePath(path) || !ToBareAsciiName(path, kBuiltinFilePrefixLength, bare))
        return nullptr;
    return FindBareAsciiName(bare);
}

UCS2String BuiltinFilePath(const BuiltinFileEntry& entry)
{
    return ASCIItoUCS2String(kBuiltinFilePrefix) + ASCIItoUCS2String(entry.name);
}

bool IsBuiltinIncludeFile(const BuiltinFileEntry& entry)
{
    size_t length = std::strlen(entry.name);
    return (length > 4) && (std::strcmp(entry.name + length - 4, ".inc") == 0);
}

IMemStream* OpenBuiltinFile(const BuiltinFileEntry& entry)
{
    return new IBuiltinFileStream(GetData(entry), BuiltinFilePath(entry));
}

IMemStream* OpenBuiltinFile(const UCS2String& path)
{
    const BuiltinFileEntry* entry = FindBuiltinFileByPath(path);
    if (entry != nullptr)
        return OpenBuiltinFile(*entry);
    const char* text = FindBuiltinText(path);
    if (text != nullptr)
        return new IMemStream(reinterpret_cast<const unsigned char*>(text), std::strlen(text), path);
    return nullptr;
}

void RegisterBuiltinText(const char* name, const char* text)
{
    std::lock_guard<std::mutex> lock(gBuiltinTextMutex);
    gBuiltinTexts[name] = text;
}

bool IsBuiltinTextPath(const UCS2String& path)
{
    return (FindBuiltinText(path) != nullptr);
}

void ReleaseBuiltinFileCache()
{
    std::lock_guard<std::mutex> lock(gBuiltinFileCacheMutex);
    gBuiltinFileCache.clear();
}

namespace
{

/// Whether a name listed in a directory is taken by a file to be written there. Windows
/// finds names whatever their case, so `COLORS.INC` takes `colors.inc` there.
bool SameFileName(const UCS2String& listed, const char* name)
{
    std::string bare;
    if (!ToBareAsciiName(listed, 0, bare))
        return false;
#ifdef _WIN32
    return (pov_stricmp(bare.c_str(), name) == 0);
#else
    return (bare == name);
#endif
}

/// Remove the files written so far, after a failure.
void RemoveWritten(WriteBuiltinFilesResult& result, const std::vector<Path>& paths)
{
    for (size_t i = 0; i < result.written.size(); ++i)
        Filesystem::DeleteFile(paths[i]());
    result.written.clear();
    result.bytes = 0;
}

}
// end of anonymous namespace

WriteBuiltinFilesResult WriteBuiltinFiles(const UCS2String& dir, bool overwrite)
{
    WriteBuiltinFilesResult result;
    std::vector<Path> paths;
    size_t i = 0;

    try
    {
        UCS2String folder(dir);
        if ((folder.empty() == false) && (POV_IS_PATH_SEPARATOR(folder[folder.size() - 1]) == false))
        {
            // the name as given must be valid by itself: on Windows a bare `K:` is drive-relative,
            // which is not supported, and would otherwise become the root `K:\`
            (void)Path(folder);
            folder += POV_PATH_SEPARATOR;
        }
        Path base(folder);
        for (i = 0; i < kBuiltinFileCount; ++i)
            paths.push_back(Path(base, Path(ASCIItoUCS2String(kBuiltinFiles[i].name))));

        if (overwrite == false)
        {
            // Any entry of a file's name counts, even a directory or a link to nothing, as it
            // would stop the file from being created. A directory that cannot be listed is
            // probed for each name; the exclusive creation below catches anything missed.
            std::vector<UCS2String> listing;
            bool listed = PlatformBase::GetInstance().ListLocalDirectory(base(), listing);
            for (i = 0; i < kBuiltinFileCount; ++i)
            {
                bool exists = false;
                if (listed == true)
                {
                    for (std::vector<UCS2String>::const_iterator n(listing.begin()); (n != listing.end()) && (exists == false); ++n)
                        exists = SameFileName(*n, kBuiltinFiles[i].name);
                }
                else
                    exists = CheckIfFileExists(paths[i]);
                if (exists == true)
                    result.existing.push_back(UCS2toSysString(paths[i]()));
            }
            if (result.existing.empty() == false)
            {
                result.message = "Some of the files already exist; nothing was written.";
                return result;
            }
        }

        for (i = 0; i < kBuiltinFileCount; ++i)
        {
            std::string name(UCS2toSysString(paths[i]()));
            BuiltinFileDataPtr data(GetData(kBuiltinFiles[i]));

            // unless overwriting, create the file only if there is no entry of that name at all
            // (C11 and C++17 `x` mode: not through an existing file, nor a link to a missing one)
            errno = 0;
            FILE* f = PlatformBase::GetInstance().OpenLocalFile(paths[i](), overwrite ? "wb" : "wbx");
            if (f == nullptr)
            {
                int err = errno;
                if ((err == EEXIST) && (overwrite == false))
                    result.message = "'" + name + "' already exists";
                else
                    result.message = "Cannot create '" + name + "'" + ((err != 0) ? std::string(": ") + std::strerror(err) : std::string());
                break;
            }
            result.written.push_back(name);
            bool good = (data->empty() || (fwrite(data->data(), data->size(), 1, f) == 1));
            int err = errno;
            good = (fclose(f) == 0) && good;
            if (good == false)
            {
                if (err == 0)
                    err = errno;
                result.message = "Cannot write '" + name + "'" + ((err != 0) ? std::string(": ") + std::strerror(err) : std::string());
                break;
            }
            result.bytes += data->size();
        }
    }
    catch (pov_base::Exception& e)
    {
        result.message = e.what();
    }
    catch (std::bad_alloc&)
    {
        result.message = "Out of memory.";
    }
    catch (std::exception& e)
    {
        result.message = e.what();
    }

    if (i < kBuiltinFileCount)
    {
        // the message is completed below, so it must not end in a full stop yet
        if ((result.message.empty() == false) && (result.message[result.message.size() - 1] == '.'))
            result.message.erase(result.message.size() - 1);

        // nothing is left half done: a file this call created is removed again
        if ((overwrite == false) && (result.written.empty() == false))
        {
            RemoveWritten(result, paths);
            result.message += "; the files already written were removed again, so nothing was written.";
        }
        else if (result.written.empty() == true)
            result.message += "; nothing was written.";
        else
            result.message += ".";
        return result;
    }

    result.ok = true;
    return result;
}

}
// end of namespace pov_base
