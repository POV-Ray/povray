//******************************************************************************
///
/// @file base/path.h
///
/// Declarations related to file paths.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

// Standard C++ header files
#include <string>
#include <vector>

// POV-Ray base header files
#include "base/stringutilities.h"
#include "base/types.h"

namespace pov_base
{

class Path
{
    public:
        enum Encoding
        {
            ASCII,
            URL
        };

        Path();
        Path(const char *p, Encoding e = ASCII);
        Path(const string& p, Encoding e = ASCII);
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
        vector<UCS2String> GetAllFolders() const;
        UCS2String GetFile() const;

        void SetVolume(const char *p);
        void SetVolume(const string& p);
        void SetVolume(const UCS2 *p);
        void SetVolume(const UCS2String& p);

        void AppendFolder(const char *p);
        void AppendFolder(const string& p);
        void AppendFolder(const UCS2 *p);
        void AppendFolder(const UCS2String& p);

        void RemoveFolder();
        void RemoveAllFolders();

        void SetFile(const char *p);
        void SetFile(const string& p);
        void SetFile(const UCS2 *p);
        void SetFile(const UCS2String& p);

        void Clear();

        bool Empty() const;
    private:
        UCS2String volume;
        vector<UCS2String> folders;
        UCS2String file;

        void ParsePathString(const UCS2String& p);

        UCS2String URLToUCS2String(const char *p) const;
        UCS2String URLToUCS2String(const string& p) const;
};

}

#endif // POVRAY_BASE_PATH_H
