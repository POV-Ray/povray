//******************************************************************************
///
/// @file backend/scene/backendscenedata.h
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

#ifndef POVRAY_BACKEND_BACKENDSCENEDATA_H
#define POVRAY_BACKEND_BACKENDSCENEDATA_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"
#include "backend/scene/backendscenedata_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <map>

// POV-Ray header files (base module)
#include "base/stringtypes.h"

// POV-Ray header files (core module)
#include "core/scene/scenedata.h"

// POV-Ray header files (backend module)
#include "backend/control/renderbackend.h"

namespace pov
{

class BackendSceneData final : public SceneData
{
        // Scene needs access to the private scene data constructor!
        friend class Scene;

    public:

        typedef std::map<UCS2String, UCS2String> FilenameToFilenameMap;

        /// scene id
        RenderBackend::SceneId sceneId;
        /// backend address
        POVMSAddress backendAddress;
        /// frontend address
        POVMSAddress frontendAddress;

        /**
         *  Find a file for reading.
         *  If the file is not available locally, the frontend will be queried.
         *  Variants of the filename with extensions matching file type will
         *  be tried. Only the first file found is returned.
         *  @param  ctx             POVMS message context for the current thread.
         *  @param  filename        Name and optional (partial) path.
         *  @param  stype           File type.
         *  @return                 Name of found file or empty string.
         */
        UCS2String FindFile(POVMSContext ctx, const UCS2String& filename, unsigned int stype);

        /**
         *  Open a file for reading.
         *  If the file is not available locally, the frontend will be queried.
         *  If the frontend has the file, it will be assigned a local temporary
         *  name that is mapped to the specified file name (so repeated access
         *  does not require contacting the frontend) and the file will be
         *  transferred from the frontend to the local system as necessary.
         *  Note that for their first access the frontend will always be asked
         *  to provide the location of the file. Local files are only accessed
         *  within the system specific temporary directory. This prevents
         *  access to files on local systems in case of remote rendering.
         *  Returns `nullptr` if the file could not be found.
         *  @param  ctx             POVMS message context for the current thread.
         *  @param  origname        The original name of the file as in the scene file (could be relative). // TODO FIXME - not needed, just a hack, the source [trf]
         *  @param  filename        Name and optional (partial) path.
         *  @param  stype           File type.
         *  @return                 Pointer to the file or `nullptr`. The caller is
         *                          responsible for freeing the pointer!
         */
        IStream *ReadFile(POVMSContext ctx, const UCS2String& origname, const UCS2String& filename, unsigned int stype); // TODO FIXME - see above and source code [trf]

        /**
         *  Open a file given by name and optional (partial) path for writing.
         *  Rather than creating the file in the specified location, a temporary
         *  file will be created and the specified name will be mapped to that
         *  local file. Local files are only accessed within the system specific
         *  temporary directory. This prevents access to files on local systems
         *  in case of remote rendering. For each newly created file the
         *  frontend is notified and after rendering the frontend can decide
         *  which files to access. In addition, this allows parsing the same
         *  scene simultaneously more than once as each scene manages its own
         *  set of unique temporary files and thus at no time a file is written
         *  to or read from by more than one scene.
         *  @param  ctx             POVMS message context for the current thread.
         *  @param  filename        Name and optional (partial) path.
         *  @param  stype           File type.
         *  @param  append          True to append data to the file, false otherwise.
         *  @return                 Pointer to the file or `nullptr`. The caller is
         *                          responsible for freeing the pointer!
         */
        OStream *CreateFile(POVMSContext ctx, const UCS2String& filename, unsigned int stype, bool append);

    private:
#ifdef USE_SCENE_FILE_MAPPING
        /// maps scene file names to local file names
        FilenameToFilenameMap scene2LocalFiles;
        /// maps local file names to scene file names
        FilenameToFilenameMap local2SceneFiles;
        /// maps scene file names to temporary file names
        FilenameToFilenameMap scene2TempFiles;
        /// maps temporary file names to scene file names
        FilenameToFilenameMap temp2SceneFiles;
#endif

        /**
         *  Create new scene specific data.
         */
        BackendSceneData();

        BackendSceneData(const BackendSceneData&) = delete;
        BackendSceneData& operator=(const BackendSceneData&) = delete;
};

}
// end of namespace pov

#endif // POVRAY_BACKEND_BACKENDSCENEDATA_H
