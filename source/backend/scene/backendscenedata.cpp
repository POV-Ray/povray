//******************************************************************************
///
/// @file backend/scene/backendscenedata.cpp
///
/// @todo   What's in here?
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
#include "backend/scene/backendscenedata.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <sstream>
#include <vector>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/fileutil.h"
#include "base/path.h"
#include "base/stringutilities.h"

// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmsid.h"

// POV-Ray header files (backend module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

BackendSceneData::BackendSceneData() :
    SceneData()
{}

UCS2String BackendSceneData::FindFile(POVMSContext ctx, const UCS2String& filename, unsigned int stype)
{
    std::vector<UCS2String> filenames;
    UCS2String foundfile;
    bool tryExactFirst;

    // if filename extension, matches one of the standard ones, try the exact name first
    // (otherwise, try it last)
    UCS2String::size_type pos = filename.find_last_of('.');
    tryExactFirst = false;
    if (pos != UCS2String::npos)
    {
        for (size_t i = 0; i < POV_FILE_EXTENSIONS_PER_TYPE; i++)
        {
            if ( ( strlen(gPOV_File_Extensions[stype].ext[i]) > 0 ) &&
                 ( filename.compare(pos,filename.length()-pos, SysToUCS2String(gPOV_File_Extensions[stype].ext[i])) == 0 ) )
            {
                // match
                tryExactFirst = true;
                break;
            }
        }
    }

    // build list of files to search for
    if (tryExactFirst)
        filenames.push_back(filename);

    // add filename with extension variants to list of files to search for
    for (size_t i = 0; i < POV_FILE_EXTENSIONS_PER_TYPE; i++)
    {
        if (strlen(gPOV_File_Extensions[stype].ext[i]) > 0)
        {
            UCS2String fn(filename);
            fn += SysToUCS2String(gPOV_File_Extensions[stype].ext[i]);
            filenames.push_back(fn);
        }
    }

    if (!tryExactFirst)
        filenames.push_back(filename);

#ifdef USE_SCENE_FILE_MAPPING
    // see if the file is available locally
    for(std::vector<UCS2String>::const_iterator i(filenames.begin()); i != filenames.end(); i++)
    {
        FilenameToFilenameMap::iterator ilocalfile(scene2LocalFiles.find(*i));

        if(ilocalfile != scene2LocalFiles.end())
            return *i;
    }

    // see if the file is available as temporary file
    for(std::vector<UCS2String>::const_iterator i(filenames.begin()); i != filenames.end(); i++)
    {
        FilenameToFilenameMap::iterator itempfile(scene2TempFiles.find(*i));

        if(itempfile != scene2TempFiles.end())
            return *i;
    }
#endif

    // otherwise, request to find the file
    RenderBackend::SendFindFile(ctx, sceneId, frontendAddress, filenames, foundfile);

    return foundfile;
}

IStream *BackendSceneData::ReadFile(POVMSContext ctx, const UCS2String& origname, const UCS2String& filename, unsigned int stype)
{
    UCS2String scenefile(filename);
    UCS2String localfile;
    UCS2String fileurl;

#ifdef USE_SCENE_FILE_MAPPING
    // see if the file is available locally
    FilenameToFilenameMap::iterator ilocalfile(scene2LocalFiles.find(scenefile));

    // if available locally, open it end return
    if(ilocalfile != scene2LocalFiles.end())
        return NewIStream(ilocalfile->second.c_str(), stype);

    // now try the original name as given in the scene
    if((ilocalfile = scene2LocalFiles.find(origname)) != scene2LocalFiles.end())
        return NewIStream(ilocalfile->second.c_str(), stype);

    // see if the file is available as temporary file
    FilenameToFilenameMap::iterator itempfile(scene2TempFiles.find(scenefile));

    // if available as temporary file, open it end return
    if(itempfile != scene2TempFiles.end())
        return NewIStream(itempfile->second.c_str(), stype);

    // otherwise, request the file
    RenderBackend::SendReadFile(ctx, sceneId, frontendAddress, scenefile, localfile, fileurl);

    // if it is available locally, add it to the map and then open it
    if(localfile.length() > 0)
    {
        scene2LocalFiles[scenefile] = localfile;
        local2SceneFiles[localfile] = scenefile;

        // yes this is a hack
        scene2LocalFiles[origname] = localfile;

        return NewIStream(localfile.c_str(), stype);
    }

    // if it is available remotely ...
    if(fileurl.length() > 0)
    {
        // create a temporary file
        UCS2String tempname = PlatformBase::GetInstance().CreateTemporaryFile();
        OStream *tempfile = NewOStream(tempname.c_str(), stype, false);

        if (tempfile == nullptr)
        {
            PlatformBase::GetInstance().DeleteTemporaryFile(tempname);
            throw POV_EXCEPTION_CODE(kCannotOpenFileErr);
        }

        // download the file from the URL
        // TODO - handle referrer
        if(PlatformBase::GetInstance().ReadFileFromURL(tempfile, fileurl) == false)
        {
            delete tempfile;
            PlatformBase::GetInstance().DeleteTemporaryFile(tempname);
            throw POV_EXCEPTION_CODE(kNetworkConnectionErr);
        }

        delete tempfile;

        // add the temporary file to the map
        scene2TempFiles[scenefile] = tempname;
        temp2SceneFiles[tempname] = scenefile;

        return NewIStream(tempname.c_str(), stype);
    }

    // file not found
    return nullptr;
#else
    return NewIStream(filename.c_str(), stype);
#endif
}

/* TODO FIXME - this is the correct code but it has a bug. The code above is just a hack [trf]
IStream *BackendSceneData::ReadFile(POVMSContext ctx, const UCS2String& filename, unsigned int stype)
{
    UCS2String scenefile(filename);
    UCS2String localfile;
    UCS2String fileurl;

    // see if the file is available locally
    FilenameToFilenameMap::iterator ilocalfile(scene2LocalFiles.find(scenefile));

    // if available locally, open it end return
    if(ilocalfile != scene2LocalFiles.end())
        return NewIStream(ilocalfile->second.c_str(), stype);

    // see if the file is available as temporary file
    FilenameToFilenameMap::iterator itempfile(scene2TempFiles.find(scenefile));

    // if available as temporary file, open it end return
    if(itempfile != scene2TempFiles.end())
        return NewIStream(itempfile->second.c_str(), stype);

    // otherwise, request the file
    RenderBackend::SendReadFile(ctx, sceneId, frontendAddress, scenefile, localfile, fileurl);

    // if it is available locally, add it to the map and then open it
    if(localfile.length() > 0)
    {
        scene2LocalFiles[scenefile] = localfile;
        local2SceneFiles[localfile] = scenefile;

        return NewIStream(localfile.c_str(), stype);
    }

    // if it is available remotely ...
    if(fileurl.length() > 0)
    {
        // create a temporary file
        UCS2String tempname = PlatformBase::GetInstance().CreateTemporaryFile();
        OStream *tempfile = NewOStream(tempname.c_str(), stype, false);

        if (tempfile == nullptr)
        {
            PlatformBase::GetInstance().DeleteTemporaryFile(tempname);
            throw POV_EXCEPTION_CODE(kCannotOpenFileErr);
        }

        // download the file from the URL
        // TODO - handle referrer
        if(PlatformBase::GetInstance().ReadFileFromURL(tempfile, fileurl) == false)
        {
            delete tempfile;
            PlatformBase::GetInstance().DeleteTemporaryFile(tempname);
            throw POV_EXCEPTION_CODE(kNetworkConnectionErr);
        }

        delete tempfile;

        // add the temporary file to the map
        scene2TempFiles[scenefile] = tempname;
        temp2SceneFiles[tempname] = scenefile;

        return NewIStream(tempname.c_str(), stype);
    }

    // file not found
    return nullptr;
}
*/
OStream *BackendSceneData::CreateFile(POVMSContext ctx, const UCS2String& filename, unsigned int stype, bool append)
{
    UCS2String scenefile(filename);

#ifdef USE_SCENE_FILE_MAPPING
    // see if the file is available as temporary file
    FilenameToFilenameMap::iterator itempfile(scene2TempFiles.find(scenefile));

    // if available as temporary file, open it end return
    if(itempfile != scene2TempFiles.end())
        return NewOStream(itempfile->second.c_str(), stype, append);

    // otherwise, create a temporary file ...
    UCS2String tempname = PlatformBase::GetInstance().CreateTemporaryFile();
    OStream *tempfile = NewOStream(tempname.c_str(), stype, append);

    // failed to open file
    if (tempfile == nullptr)
        return nullptr;

    // add the temporary file to the map
    scene2TempFiles[scenefile] = tempname;
    temp2SceneFiles[tempname] = scenefile;
#else
    // this is a workaround for the incomplete scene temp file support
    // until someone has time to finish it.

    OStream *tempfile = NewOStream(scenefile.c_str(), stype, append);
    if (tempfile == nullptr)
        return nullptr;
#endif

    // let the frontend know that a new file was created
    RenderBackend::SendCreatedFile(ctx, sceneId, frontendAddress, scenefile);

    return tempfile;
}

}
// end of namespace pov
