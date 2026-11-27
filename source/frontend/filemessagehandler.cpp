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
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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

#include <new>

#include "base/builtinfiles.h"
#include "base/fileinputoutput.h"
#include "base/stringutilities.h"

#include "frontend/renderfrontend.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

FileMessageHandler::FileMessageHandler()
{
}

FileMessageHandler::~FileMessageHandler()
{
}

void FileMessageHandler::HandleMessage(const SceneData& sd, POVMSType ident, POVMS_Object& msg, POVMS_Object& result)
{
    // The back-end waits for our reply to a find or read request, so an exception must not
    // escape from here: it would leave the request unanswered until it times out, and the
    // reason would be lost. Instead the error is returned in the reply (see
    // RenderBackend::SendFindFile and RenderBackend::SendReadFile).
    try
    {
        switch(ident)
        {
            case kPOVMsgIdent_FindFile:
                (void)FindFile(sd, msg, result);
                break;
            case kPOVMsgIdent_ReadFile:
                if(ReadFile(sd.searchpaths, sd.scenepath, sd.inputfile, msg, result) == true)
                    sd.readfiles.push_back(result);
                break;
            case kPOVMsgIdent_CreatedFile:
                CreatedFile(msg);
                sd.createdfiles.push_back(msg);
                break;
        }
    }
    catch(pov_base::Exception& e)
    {
        if (ident == kPOVMsgIdent_CreatedFile)
            throw;
        result.SetInt(kPOVAttrib_ErrorNumber, ((e.codevalid() == true) && (e.code() != kNoErr)) ? e.code() : kCannotOpenFileErr);
        result.SetString(kPOVAttrib_EnglishText, e.what());
    }
    catch(std::bad_alloc&)
    {
        if (ident == kPOVMsgIdent_CreatedFile)
            throw;
        result.SetInt(kPOVAttrib_ErrorNumber, kOutOfMemoryErr);
        result.SetString(kPOVAttrib_EnglishText, "Out of memory.");
    }
    catch(std::exception& e)
    {
        if (ident == kPOVMsgIdent_CreatedFile)
            throw;
        result.SetInt(kPOVAttrib_ErrorNumber, kCannotOpenFileErr);
        result.SetString(kPOVAttrib_EnglishText, e.what());
    }
}

bool FileMessageHandler::FindFile(const SceneData& sd, POVMS_Object& msg, POVMS_Object& result)
{
    POVMS_List files;
    vector<UCS2String> names;

    msg.Get(kPOVAttrib_ReadFile, files);

    for(int i = 1; i <= files.GetListSize(); i++)
    {
        POVMS_Attribute attr;

        files.GetNth(i, attr);
        names.push_back(attr.GetUCS2String());
    }

    // The back-end asks for the scene file itself in the same way as for the files the scene
    // names, offering the name as given and with each customary extension appended. The scene's
    // directory is derived from that same name, so it must not be searched for the scene file:
    // for a scene named `sub/main.pov` that would try `sub/sub/main.pov`.
    FindFileRequest request(names);
    for(vector<UCS2String>::const_iterator i(names.begin()); (i != names.end()) && (request.sceneFile == false); i++)
        request.sceneFile = (sd.inputfile.empty() == false) && (*i == sd.inputfile);

    // a `file_exists()` probe reports no error, so it gets no notes either
    request.probe = msg.TryGetBool(kPOVAttrib_FindFileProbe, false);

    UCS2String found;

    // In disk mode, a file read from the built-in set (which can only happen by its explicit
    // pseudo-path) takes the standard files it includes by their bare names from the built-in
    // set too, so that it is never mixed with an older or partial set on disk. In built-in mode
    // the usual order below already ends with the built-in set, and a project's own copy in
    // the current directory or the scene's directory still comes first.
    BuiltinIncludesJob* job = sd.builtinIncludes.get();
    if((job != nullptr) && (job->mode == BuiltinIncludesJob::kDisk) && msg.TryGetBool(kPOVAttrib_FromBuiltinFile, false))
    {
        for(vector<UCS2String>::const_iterator i(names.begin()); (i != names.end()) && found.empty(); i++)
        {
            const BuiltinFileEntry* entry = pov_base::FindBuiltinFile(*i);
            if(entry != nullptr)
                found = BuiltinFilePath(*entry);
        }
    }

    if(found.empty() == true)
        found = FindOnDisk(sd, sd.searchpaths, request);

    // only if none of the names is found on disk
    if(found.empty() == true)
        found = FindBuiltinCopy(sd, request);

    result.SetUCS2String(kPOVAttrib_ReadFile, found.c_str());

    return (found.empty() == false);
}

UCS2String FileMessageHandler::FindOnDisk(const SceneData& sd, const list<Path>& lps, const FindFileRequest& request)
{
    const vector<UCS2String>& names(request.names);

    for(vector<UCS2String>::const_iterator i(names.begin()); i != names.end(); i++)
    {
        const UCS2String& name(*i);

        // a pseudo-path such as `<builtin>/colors.inc` always means the built-in copy
        if(IsBuiltinFilePath(name) == true)
            continue;

        Path path;

        try
        {
            path = FindFilePath(lps, request.sceneFile ? Path() : sd.scenepath, Path(name));
        }
        catch(pov_base::Exception&)
        {
            // If the name is not a valid path, report it as the scene gave it rather than with an
            // extension appended: that is the shortest of the names offered, a prefix of the others.
            for(vector<UCS2String>::const_iterator j(names.begin()); j != names.end(); j++)
            {
                if((j->size() < name.size()) && (name.compare(0, j->size(), *j) == 0))
                    (void)Path(*j); // throws if it is invalid too, naming it
            }
            throw;
        }

        if(path.Empty() == false)
        {
            NoteBuiltinOverride(sd, name, path, request);

            // A file on disk is never to be mistaken for a built-in one: on Unix `<builtin>`
            // is a valid directory name, so a relative library path or scene directory of that
            // name gives a path that starts like a pseudo-path. (On Windows `<` is not valid in
            // a file name, so this cannot happen there.)
            UCS2String found(path());
            if(IsBuiltinFilePath(found) == true)
                found = ASCIItoUCS2String("./") + found;
            return found;
        }
    }

    return UCS2String();
}

bool FileMessageHandler::ReadFile(const list<Path>& lps, const Path& scenepath, const UCS2String& inputfile, POVMS_Object& msg, POVMS_Object& result)
{
    UCS2String name(msg.GetUCS2String(kPOVAttrib_ReadFile));
    bool sceneFile = (inputfile.empty() == false) && (name == inputfile);
    Path path(FindFilePath(lps, sceneFile ? Path() : scenepath, Path(name)));

    if(path.Empty() == false)
        result.SetUCS2String(kPOVAttrib_LocalFile, path().c_str());

    return (path.Empty() == false);
}

void FileMessageHandler::CreatedFile(POVMS_Object&)
{
}

// Tells whether a directory path is empty or names the current directory trivially,
// i.e. has no volume and consists of `.` components only.
static bool IsTrivialCurrentDir(const Path& dir)
{
    if (dir.HasVolume() == true)
        return false;

    const UCS2String dot(ASCIItoUCS2String("."));
    vector<UCS2String> folders(dir.GetAllFolders());
    for (vector<UCS2String>::const_iterator i(folders.begin()); i != folders.end(); i++)
    {
        if (*i != dot)
            return false;
    }

    return dir.GetFile().empty();
}

/// Prints a note, unless the file is only being probed for (`file_exists()`, which reports no error).
static void NoteUnlessProbe(BuiltinIncludesJob* job, const SceneData& sd, bool probe, const std::string& key, const std::string& text)
{
    if (probe == false)
        job->Note(sd.console, key, text);
}

UCS2String FileMessageHandler::FindBuiltinCopy(const SceneData& sd, const FindFileRequest& request)
{
    const vector<UCS2String>& names(request.names);
    const bool probe = request.probe;
    BuiltinIncludesJob* job = sd.builtinIncludes.get();
    if (job == nullptr)
        return UCS2String();

    // A pseudo-path names the built-in copy explicitly: it is used in disk mode too, but not
    // if the built-in files are switched off. Any directory part after the prefix, or a name
    // that is not that of a built-in file, matches nothing.
    const UCS2String* given = nullptr;
    for (vector<UCS2String>::const_iterator i(names.begin()); i != names.end(); i++)
    {
        if (IsBuiltinFilePath(*i) == false)
            continue;
        if ((given == nullptr) || (i->size() < given->size()))
            given = &(*i); // the name as given is the shortest, a prefix of the others

        // a text registered in memory (the benchmark) is not a stand-in for a file on
        // disk, so it is always available
        if (IsBuiltinTextPath(*i) == true)
            return *i;

        const BuiltinFileEntry* entry = FindBuiltinFileByPath(*i);
        if (entry == nullptr)
            continue;

        string path(UCS2toASCIIString(BuiltinFilePath(*entry)));
        if (job->mode == BuiltinIncludesJob::kOff)
        {
            NoteUnlessProbe(job, sd, probe, "off:" + path,
                            "Note: '" + path + "' is not available because the option Builtin_Includes is off.");
            return UCS2String();
        }
        return BuiltinFilePath(*entry);
    }
    if (given != nullptr)
    {
        string path(UCS2toASCIIString(*given));
        NoteUnlessProbe(job, sd, probe, "none:" + path, "Note: '" + path + "' does not name a built-in file.");
        return UCS2String();
    }

    // the scene file itself is taken from the built-in files only if named explicitly
    if (request.sceneFile == true)
        return UCS2String();

    for (vector<UCS2String>::const_iterator i(names.begin()); i != names.end(); i++)
    {
        const BuiltinFileEntry* entry = pov_base::FindBuiltinFile(*i);
        if (entry == nullptr)
            continue;

        string name(entry->name);
        switch (job->mode)
        {
            case BuiltinIncludesJob::kBuiltin:
                return BuiltinFilePath(*entry);

            case BuiltinIncludesJob::kDisk:
                NoteUnlessProbe(job, sd, probe, "disk:" + name,
                                "Note: the built-in copy of '" + name + "' was not used because an include directory was found at '" +
                                UCS2toASCIIString(job->includeDir) + "' (see the option Builtin_Includes).");
                break;

            case BuiltinIncludesJob::kOff:
                NoteUnlessProbe(job, sd, probe, "off:" + name,
                                "Note: the built-in copy of '" + name + "' was not used because the option Builtin_Includes is off.");
                break;

            default:
                break;
        }
        return UCS2String();
    }

    return UCS2String();
}

void FileMessageHandler::NoteBuiltinOverride(const SceneData& sd, const UCS2String& name, const Path& found, const FindFileRequest& request)
{
    BuiltinIncludesJob* job = sd.builtinIncludes.get();
    if ((job == nullptr) || (job->mode != BuiltinIncludesJob::kBuiltin) || (request.sceneFile == true) || (request.probe == true))
        return;

    const BuiltinFileEntry* entry = pov_base::FindBuiltinFile(name);
    if (entry == nullptr)
        return;

    // only the user's own copy counts, in the current directory or the scene's directory
    // (FindFilePath() returns these paths in exactly this form), not one on a library path
    string where;
    if (found() == Path(name)())
        where = "the current directory";
    else if ((IsTrivialCurrentDir(sd.scenepath) == false) && (found() == Path(sd.scenepath, Path(name))()))
        where = "the scene's directory '" + UCS2toASCIIString(sd.scenepath()) + "'";
    else
        return;

    string bare(entry->name);
    job->Note(sd.console, "override:" + bare,
              "Note: using '" + bare + "' from " + where + " instead of the built-in copy.");
}

Path FileMessageHandler::FindFilePath(const list<Path>& lps, const Path& scenepath, const Path& f)
{
    // check the current working dir (or full path if supplied) first
    // note that if the file doesn't have a path and it is found in the
    // CWD, the CWD is not returned as part of the path.
    if(CheckIfFileExists(f) == true)
        return f;

    // if the path is absolute there's no point in checking the scene directory or
    // the include paths; given it wasn't found above we can just return an empty path
    if(f.HasVolume() == true)
        return Path();

    // next check the directory of the scene file, unless that is the current
    // working dir (which we have already checked above)
    if (IsTrivialCurrentDir(scenepath) == false)
    {
        Path path(scenepath, f);

        if(CheckIfFileExists(path) == true)
            return path;
    }

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
