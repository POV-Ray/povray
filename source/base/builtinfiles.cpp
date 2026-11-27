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
#include "base/pov_err.h"

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
    if (entry == nullptr)
        return nullptr;
    return OpenBuiltinFile(*entry);
}

void ReleaseBuiltinFileCache()
{
    std::lock_guard<std::mutex> lock(gBuiltinFileCacheMutex);
    gBuiltinFileCache.clear();
}

}
// end of namespace pov_base
