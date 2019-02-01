//******************************************************************************
///
/// @file frontend/filemessagehandler.cpp
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
#include "frontend/filemessagehandler.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"

// POV-Ray header files (POVMS module)
//  (none at the moment)

// POV-Ray header files (frontend module)
#include "frontend/renderfrontend.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

using namespace pov_base;

using std::list;

FileMessageHandler::FileMessageHandler()
{
}

FileMessageHandler::~FileMessageHandler()
{
}

void FileMessageHandler::HandleMessage(const SceneData& sd, POVMSType ident, POVMS_Object& msg, POVMS_Object& result)
{
    switch(ident)
    {
        case kPOVMsgIdent_FindFile:
            (void)FindFile(sd.searchpaths, msg, result);
            break;
        case kPOVMsgIdent_ReadFile:
            if(ReadFile(sd.searchpaths, msg, result) == true)
                sd.readfiles.push_back(result);
            break;
        case kPOVMsgIdent_CreatedFile:
            CreatedFile(msg);
            sd.createdfiles.push_back(msg);
            break;
    }
}

bool FileMessageHandler::FindFile(const list<Path>& lps, POVMS_Object& msg, POVMS_Object& result)
{
    POVMS_List files;
    Path path;

    msg.Get(kPOVAttrib_ReadFile, files);

    for(int i = 1; i <= files.GetListSize(); i++)
    {
        POVMS_Attribute attr;

        files.GetNth(i, attr);

        path = FindFilePath(lps, Path(attr.GetUCS2String()));

        if(path.Empty() == false)
            break;
    }

    result.SetUCS2String(kPOVAttrib_ReadFile, path().c_str());

    return (path.Empty() == false);
}

bool FileMessageHandler::ReadFile(const list<Path>& lps, POVMS_Object& msg, POVMS_Object& result)
{
    Path path(FindFilePath(lps, Path(msg.GetUCS2String(kPOVAttrib_ReadFile))));

    if(path.Empty() == false)
        result.SetUCS2String(kPOVAttrib_LocalFile, path().c_str());

    return (path.Empty() == false);
}

void FileMessageHandler::CreatedFile(POVMS_Object&)
{
}

Path FileMessageHandler::FindFilePath(const list<Path>& lps, const Path& f)
{
    // check the current working dir (or full path if supplied) first
    // note that if the file doesn't have a path and it is found in the
    // CWD, the CWD is not returned as part of the path.
    if(CheckIfFileExists(f) == true)
        return f;

    // if the path is absolute there's no point in checking the include paths;
    // given it wasn't found above we can just return an empty path
    if(f.HasVolume() == true)
        return Path();

    Path path(f);

    for(list<Path>::const_iterator i(lps.begin()); i != lps.end(); i++)
    {
        Path path (*i, f);

        if(CheckIfFileExists(path) == true)
            return path;
    }

    // not found so return empty path
    return Path();
}

}
// end of namespace pov_frontend
