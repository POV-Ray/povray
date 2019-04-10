//******************************************************************************
///
/// @file base/path.h
///
/// Declarations related to file paths.
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

#ifndef POVRAY_BASE_PATH_H
#define POVRAY_BASE_PATH_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"
#include "base/path_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <string>
#include <vector>

// POV-Ray header files (base module)
#include "base/stringtypes.h"

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBasePath File Path Handling
/// @ingroup PovBase
///
/// @{

class Path final
{
    public:

        enum class Encoding
        {
            kSystem,
            kURL
        };

        Path();
        Path(const char *p, Encoding e = Encoding::kSystem);
        Path(const std::string& p, Encoding e = Encoding::kSystem);
        Path(const UCS2 *p);
        Path(const UCS2String& p);

        /**
         *  Creates a path from a base path and a filename.
         *  Filename information in the base path is allowed but ignored.
         *  Relative path information in the filename is allowed.
         *
         *  @attention  Absolute path information in the filename may cause unexpected results.
         */
        Path(const Path& p1, const Path& p2);

        bool operator==(const Path& p) const;
        bool operator!=(const Path& p) const;

        UCS2String operator()() const;

        bool HasVolume() const;

        UCS2String GetVolume() const;
        UCS2String GetFolder() const;
        std::vector<UCS2String> GetAllFolders() const;
        UCS2String GetFile() const;

        void SetVolume(const char *p);
        void SetVolume(const std::string& p);
        void SetVolume(const UCS2 *p);
        void SetVolume(const UCS2String& p);

        void AppendFolder(const char *p);
        void AppendFolder(const std::string& p);
        void AppendFolder(const UCS2 *p);
        void AppendFolder(const UCS2String& p);

        void RemoveFolder();
        void RemoveAllFolders();

        void SetFile(const char *p);
        void SetFile(const std::string& p);
        void SetFile(const UCS2 *p);
        void SetFile(const UCS2String& p);

        void Clear();

        bool Empty() const;

    private:

        UCS2String volume;
        std::vector<UCS2String> folders;
        UCS2String file;

        /// Analyze a path string.
        ///
        /// This method interprets the supplied string as a path and/or file name, analyzes it for its components
        /// according to platform-specific conventions, and sets the object's data accordingly, or throws an exception
        /// if the string does not match the platform-specific conventions.
        ///
        /// @note
        ///     This method calls @ref ParsePathString(UCS2String&,std::vector<UCS2String>,UCS2String&,const UCS2String&)
        ///     to do the actual work.
        ///
        void ParsePathString(const UCS2String& p);

        /// Analyze a path string.
        ///
        /// This method interprets the supplied input string as a path and/or file name, analyzes it for its components
        /// according to platform-specific conventions, and sets the output parameters accordingly.
        ///
        /// @note
        ///     If the method returns `false`, the output parameters shall be empty.
        ///
        /// @note
        ///     If according to the platform-specific conventions the structure of the path string clearly identifies it
        ///     as a directory name (e.g. a trailing path separator character on Unix systems), the file name element
        ///     shall be set to an empty string. Otherwise, the last element of the path shall invariably be interpreted
        ///     as a regular file name.
        ///
        /// @note
        ///     The portable implementation of this method supports only local files on a single anonymous volume,
        ///     using the characters identified by @ref POV_IS_PATH_SEPARATOR as path separator characters.
        ///     To override this behaviour, set the @ref POV_USE_DEFAULT_PATH_PARSER compile-time configuration macro
        ///     to non-zero and provide a modified implementation of this method in a platform-specific source file.
        ///
        /// @param[out] v   The volume element of the path, including a trailing @ref POV_PATH_SEPARATOR if, and _only_
        ///                 if, the input string represents an absolute path.
        /// @param[out] d   The list of directory elements comprising the path, each _without_ any separator characters.
        /// @param[out] f   The file name element of the path, _without_ any separator character.
        /// @param[in]  p   The path string to analyze.
        /// @return         Whether the string constitutes a valid path and/or file name according to the
        ///                 platform-specific conventions.
        ///
        static bool ParsePathString (UCS2String& v, std::vector<UCS2String>& d, UCS2String& f, const UCS2String& p);

        UCS2String URLToUCS2String(const char *p) const;
        UCS2String URLToUCS2String(const std::string& p) const;
};

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_PATH_H
