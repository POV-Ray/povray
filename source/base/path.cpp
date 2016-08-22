//******************************************************************************
///
/// @file base/path.cpp
///
/// Implementations related to file paths.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/path.h"

#ifdef USE_SYSPROTO
#include "syspovprotobase.h" // TODO FIXME - need to resolve structural dependencies between config.h, configbase.h, frame.h and sysproto.h
#endif

// POV-Ray base header files
#include "base/pov_err.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

Path::Path()
{
}

Path::Path(const char *p, Encoding e)
{
    if(e == ASCII)
        ParsePathString(ASCIItoUCS2String(p));
    else
        ParsePathString(URLToUCS2String(p));
}

Path::Path(const string& p, Encoding e)
{
    if(e == ASCII)
        ParsePathString(ASCIItoUCS2String(p.c_str()));
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
        p += POV_FILE_SEPARATOR;
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
    volume = ASCIItoUCS2String(p);
}

void Path::SetVolume(const string& p)
{
    volume = ASCIItoUCS2String(p.c_str());
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
    folders.push_back(ASCIItoUCS2String(p));
}

void Path::AppendFolder(const string& p)
{
    folders.push_back(ASCIItoUCS2String(p.c_str()));
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
    file = ASCIItoUCS2String(p);
}

void Path::SetFile(const string& p)
{
    file = ASCIItoUCS2String(p.c_str());
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
    if(POV_PARSE_PATH_STRING(p, volume, folders, file) == false)
        throw POV_EXCEPTION_STRING("Invalid path."); // TODO FIXME - properly report path string [trf]
}

UCS2String Path::URLToUCS2String(const char *p) const
{
    return URLToUCS2String(string(p));
}

UCS2String Path::URLToUCS2String(const string& p) const
{
    return ASCIItoUCS2String(p.c_str()); // TODO FIXME
}

}
