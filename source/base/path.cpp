//******************************************************************************
///
/// @file base/path.cpp
///
/// Implementations related to file paths.
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
#include "base/path.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"
#include "base/stringutilities.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

using std::string;
using std::vector;

Path::Path()
{
}

/// @todo
///     Indicate absolute vs. relative paths separately, rather than co-opting the volume field for this purpose.
///
Path::Path(const char *p, Encoding e)
{
    if(e == Encoding::kSystem)
        ParsePathString(SysToUCS2String(p));
    else
        ParsePathString(URLToUCS2String(p));
}

Path::Path(const string& p, Encoding e)
{
    if(e == Encoding::kSystem)
        ParsePathString(SysToUCS2String(p));
    else
        ParsePathString(URLToUCS2String(p));
}

Path::Path(const UCS2 *p)
{
    ParsePathString(UCS2String(p));
}

Path::Path(const UCS2String& p)
{
    ParsePathString(p);
}

Path::Path(const Path& p1, const Path& p2)
{
    // TODO: Handle case p2.HasVolume()==true

    vector<UCS2String> f1(p1.GetAllFolders());
    vector<UCS2String> f2(p2.GetAllFolders());

    volume = p1.GetVolume();

    for(vector<UCS2String>::iterator i(f1.begin()); i != f1.end(); i++)
        folders.push_back(*i);
    for(vector<UCS2String>::iterator i(f2.begin()); i != f2.end(); i++)
        folders.push_back(*i);

    file = p2.GetFile();
}

bool Path::operator==(const Path& p) const
{
    if(volume != p.GetVolume())
        return false;

    if(folders != p.GetAllFolders())
        return false;

    if(file != p.GetFile())
        return false;

    return true;
}

bool Path::operator!=(const Path& p) const
{
    if(volume != p.GetVolume())
        return true;

    if(folders != p.GetAllFolders())
        return true;

    if(file != p.GetFile())
        return true;

    return false;
}

UCS2String Path::operator()() const
{
    UCS2String p;

    p += volume;

    for(vector<UCS2String>::const_iterator i(folders.begin()); i != folders.end(); i++)
    {
        p += *i;
        p += POV_PATH_SEPARATOR;
    }

    p += file;

    return p;
}

bool Path::HasVolume() const
{
    return (volume.length() > 0);
}

UCS2String Path::GetVolume() const
{
    return volume;
}

UCS2String Path::GetFolder() const
{
    if(folders.empty() == false)
        return folders.back();
    else
        return UCS2String();
}

vector<UCS2String> Path::GetAllFolders() const
{
    return folders;
}

UCS2String Path::GetFile() const
{
    return file;
}

void Path::SetVolume(const char *p)
{
    volume = SysToUCS2String(p);
}

void Path::SetVolume(const string& p)
{
    volume = SysToUCS2String(p);
}

void Path::SetVolume(const UCS2 *p)
{
    volume = UCS2String(p);
}

void Path::SetVolume(const UCS2String& p)
{
    volume = p;
}

void Path::AppendFolder(const char *p)
{
    folders.push_back(SysToUCS2String(p));
}

void Path::AppendFolder(const string& p)
{
    folders.push_back(SysToUCS2String(p));
}

void Path::AppendFolder(const UCS2 *p)
{
    folders.push_back(UCS2String(p));
}

void Path::AppendFolder(const UCS2String& p)
{
    folders.push_back(p);
}

void Path::RemoveFolder()
{
    folders.pop_back();
}

void Path::RemoveAllFolders()
{
    folders.clear();
}

void Path::SetFile(const char *p)
{
    file = SysToUCS2String(p);
}

void Path::SetFile(const string& p)
{
    file = SysToUCS2String(p);
}

void Path::SetFile(const UCS2 *p)
{
    file = UCS2String(p);
}

void Path::SetFile(const UCS2String& p)
{
    file = p;
}

void Path::Clear()
{
    volume.clear();
    folders.clear();
    file.clear();
}

bool Path::Empty() const
{
    return (volume.empty() && folders.empty() && file.empty());
}

void Path::ParsePathString(const UCS2String& p)
{
    if (!ParsePathString (volume, folders, file, p))
        throw POV_EXCEPTION_STRING("Invalid path."); // TODO FIXME - properly report path string [trf]
}


#if POV_USE_DEFAULT_PATH_PARSER
// Platforms may replace this method with a custom implementation.
// Such an implementation should reside in `platform/foo/syspovpath.cpp`.

bool Path::ParsePathString (UCS2String& volume, vector<UCS2String>& dirnames, UCS2String& filename, const UCS2String& path)
{
    UCS2String stash;

    // Unless noted otherwise, all components are considered empty.
    volume.clear();
    dirnames.clear();
    filename.clear();

    // Empty strings are considered valid path names, too.
    if (path.empty() == true)
        return true;

    // Paths beginning with the path separator character are considered absolute path names.
    // Currently, we indicate those by appending a separator character to the volume (which in
    // this implementation is always empty otherwise).
    if(POV_IS_PATH_SEPARATOR(path[0]))
        volume = POV_PATH_SEPARATOR;

    // Walk through the path string, stashing any non-separator characters. Whenever we hit a separator
    // character, emit the stashed characters (if any) as a directory name and clear the stash.

    // NB since we do not emit "empty" directory names, any sequence of consecutive separator
    // characters is effectively treated as a single separator character.
    // NB in case of an absolute path name we're parsing the leading path separator again.
    // This does not hurt the algorithm, since in that case the buffer is still empty and
    // therefore no directory name will be emitted at that point.

    for(UCS2String::const_iterator i = path.begin(); i != path.end(); ++i)
    {
        if (POV_IS_PATH_SEPARATOR(*i))
        {
            if (!stash.empty())
            {
                dirnames.push_back(stash);
                stash.clear();
            }
        }
        else
            stash += *i;
    }

    // Whatever is left in the stash is presumably the actual file name.

    // NB as a consequence of the algorithm chosen, any path name ending in a path separator
    // character will be emitted as a list of directories only, with the file name left empty.

    filename = stash;

    return true;
}

#endif // POV_USE_DEFAULT_PATH_PARSER


UCS2String Path::URLToUCS2String(const char *p) const
{
    return URLToUCS2String(string(p));
}

UCS2String Path::URLToUCS2String(const string& p) const
{
    return SysToUCS2String(p); // TODO FIXME
}

}
// end of namespace pov_base
