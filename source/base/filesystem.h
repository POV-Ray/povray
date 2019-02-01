//******************************************************************************
///
/// @file base/filesystem.h
///
/// Declaration of file system services (default implementation).
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

#ifndef POVRAY_BASE_FILESYSTEM_H
#define POVRAY_BASE_FILESYSTEM_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"
#include "base/filesystem_fwd.h"

// C++ variants of C standard header files
#include <cstdint>  // Pulled in for `std::int_least64_t`.
#include <cstddef>  // Pulled in for `std::size_t`.

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/stringtypes.h"

namespace pov_base
{

/// File System Handling.
/// @ingroup PovBase
///
namespace Filesystem
{

#ifdef DeleteFile
#undef DeleteFile // Shame on you, Windows!
#endif

/// Delete file.
///
/// This function shall try to delete (or, more specifically, "unlink") the
/// specified file.
///
/// @note
///     If the file does not exist, the return value is currently undefined.
///
/// @note
///     If the file is a directory, the function should fail (but callers are
///     advised to not rely on this property).
///
/// @note
///     The default implementation has some limitations; specifically, it only
///     supports file names comprised of the _narrow execution character set_,
///     and on some platforms may happily delete directories. Platforms are
///     strongly encouraged to provide their own implementation.
///
/// @param  fileName    Name of the file to delete.
/// @return             `true` if the file was deleted, `false` otherwise.
///
bool DeleteFile(const UCS2String& fileName);

/// Large file handling.
///
/// This class provides basic random access to large (>2 GiB) files.
///
/// @note
///     Implementations should be optimized for random access of several dozen
///     kByte at a time; typically, this means implementations should refrain
///     from buffering.
///
/// @note
///     The default implementation of this function may or may not support files
///     larger than 2 GiB, and performance may be less than optimal. Platforms
///     are encouraged to provide their own implementation.
///
class LargeFile final
{
public:

    LargeFile();
    ~LargeFile();

    /// Create file for read/write access.
    /// @note
    ///     If the file already exists, it should be truncated to zero size.
    bool CreateRW(const UCS2String& fileName);

    /// Seek in file.
    bool Seek(std::int_least64_t offset);

    /// Read from file.
    std::size_t Read(void* data, std::size_t maxSize);

    /// Write to file.
    bool Write(const void* data, std::size_t size);

    /// Close file.
    void Close();

private:

    struct Data;
    std::unique_ptr<Data> mpData;
};

/// Temporary file tracker.
///
/// This class can be used to make sure that a given file is automatically
/// deleted, based on the lifetime of this object.
///
class TemporaryFile final
{
public:

    /// Track a file with an auto-generated name.
    ///
    /// This constructor associates the object with a new unique file name in
    /// a location well-suited for temporary files. The name of the file can be
    /// queried via @ref GetFileName().
    ///
    /// @note
    ///     The constructor may or may not auto-create an empty file with the
    ///     generated name.
    ///
    explicit TemporaryFile();

    explicit TemporaryFile(const TemporaryFile&) = delete;

    /// Track a caller-specified file.
    ///
    /// This constructor associates the object with a caller-specified
    /// file name.
    ///
    explicit TemporaryFile(const UCS2String& name);

    /// Delete tracked file.
    ~TemporaryFile();

    /// Get associated file name.
    const UCS2String& GetFileName() const;

    /// Keep file.
    ///
    /// Calling this method will disassociate the file, such that it will _not_
    /// be deleted upon object destruction.
    /// @note
    ///     Once this method has been called, it is no longer possible to query
    ///     the name of the file originally associated.
    ///
    void Keep();

    /// Delete file.
    ///
    /// Calling this method will delete the file early.
    /// @note
    ///     Once this method has been called, it is no longer possible to query
    ///     the name of the file originally associated.
    ///
    void Delete();

private:

    /// Suggest name for temporary file.
    ///
    /// This method suggests a new unique file name in a location well-suited
    /// for temporary files.
    ///
    /// @note
    ///     The method may or may not auto-create an empty file with the
    ///     generated name, depending on what is easiest and/or best suited to
    ///     the algorithm by which names are generated.
    ///
    /// @note
    ///     The default implementation is less than optimal, potentially choosing
    ///     an unsuited location or generating duplicate names under certain
    ///     conditions. Platforms should provide their own implementation of this
    ///     method.
    ///
    static UCS2String SuggestName();

    UCS2String mFileName;   ///< Associated file name, or empty string if not applicable.
};

}
// end of namespace Filesystem

}
// end of namespace pov_base

#endif // POVRAY_BASE_FILESYSTEM_H
