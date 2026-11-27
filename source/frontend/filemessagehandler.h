//******************************************************************************
///
/// @file frontend/filemessagehandler.h
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

#ifndef POVRAY_FRONTEND_FILEMESSAGEHANDLER_H
#define POVRAY_FRONTEND_FILEMESSAGEHANDLER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "frontend/configfrontend.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <list>
#include <string>
#include <vector>

// POV-Ray header files (base module)
#include "base/path_fwd.h"
#include "base/stringtypes.h"

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"

// POV-Ray header files (frontend module)
#include "frontend/renderfrontend_fwd.h"

namespace pov_frontend
{

class FileMessageHandler
{
    public:
        FileMessageHandler();
        virtual ~FileMessageHandler();

        void HandleMessage(const SceneData&, POVMSType, POVMS_Object&, POVMS_Object&);
    protected:
        /// Looks for a file named in a scene, offered under one or more names (the name
        /// as given and with customary extensions appended), and returns the name to read
        /// it by in the reply.
        ///
        /// Each name is looked for on disk as by @ref FindFilePath(). If none is found,
        /// the built-in copy of a standard include file of that name is used, if the
        /// render job uses the built-in files (see @ref BuiltinIncludesJob); its name in
        /// the reply is then its pseudo-path, e.g. `<builtin>/colors.inc`. A name that is
        /// itself a pseudo-path is never looked for on disk. In disk mode, a name of a
        /// built-in file included by a built-in file (which can only have been named by its
        /// pseudo-path) means the built-in copy, so that it is not mixed with files on disk.
        /// A probe (`file_exists()`) prints no notes.
        ///
        virtual bool FindFile(const SceneData&, POVMS_Object&, POVMS_Object&);
        virtual bool ReadFile(const std::list<pov_base::Path>&, const pov_base::Path&, const pov_base::UCS2String&, POVMS_Object&, POVMS_Object&);
        virtual void CreatedFile(POVMS_Object&);

        /// Looks for a file named in a scene.
        ///
        /// The name is tried as given (i.e. relative to the current working directory, or as
        /// an absolute path), then, if it is relative, in the scene file's directory, and then
        /// in each library path in turn.
        ///
        /// @param  lps         Library paths.
        /// @param  scenepath   Directory of the scene file, as given (may be empty).
        /// @param  f           Name of the file to look for.
        /// @return             Path to the file, or an empty path if not found.
        ///
        pov_base::Path FindFilePath(const std::list<pov_base::Path>& lps, const pov_base::Path& scenepath, const pov_base::Path& f);

        /// What a FindFile request asks for.
        struct FindFileRequest
        {
            const std::vector<pov_base::UCS2String>& names;   ///< Names the file is offered under.
            bool sceneFile;     ///< The file is the scene file itself, which can come from the built-in files only by its pseudo-path.
            bool probe;         ///< Only whether the file exists is asked (`file_exists()`); no error follows, so no notes are printed.

            explicit FindFileRequest(const std::vector<pov_base::UCS2String>& n) : names(n), sceneFile(false), probe(false) {}
        };

        /// Looks for a file on disk under each of its names in turn, as by @ref FindFilePath().
        ///
        /// @param  sd          The scene.
        /// @param  lps         Library paths to search.
        /// @param  request     The request.
        /// @return             Path of the file found, or empty if none is found. A path that
        ///                     would start like a pseudo-path is given as `./<builtin>/...`.
        ///
        pov_base::UCS2String FindOnDisk(const SceneData& sd, const std::list<pov_base::Path>& lps, const FindFileRequest& request);

        /// Looks for the built-in copy of a file not found on disk, printing a note
        /// if there is one that is not to be used (unless the request is a probe).
        ///
        /// @param  sd          The scene.
        /// @param  request     The request.
        /// @return             Pseudo-path of the built-in file, or empty if none is to be used.
        ///
        pov_base::UCS2String FindBuiltinCopy(const SceneData& sd, const FindFileRequest& request);

        /// Prints a note if a file found on disk in the current directory or the
        /// scene's directory takes the place of a built-in file of the same name.
        void NoteBuiltinOverride(const SceneData& sd, const pov_base::UCS2String& name, const pov_base::Path& found, const FindFileRequest& request);
};

}
// end of namespace pov_frontend

#endif // POVRAY_FRONTEND_FILEMESSAGEHANDLER_H
