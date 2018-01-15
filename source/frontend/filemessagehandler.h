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

#ifndef POVRAY_FRONTEND_FILEMESSAGEHANDLER_H
#define POVRAY_FRONTEND_FILEMESSAGEHANDLER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "frontend/configfrontend.h"

#include <list>

#include "povms/povmscpp.h"

namespace pov_base
{
class Path;
}

namespace pov_frontend
{

using namespace pov_base;

struct SceneData;

class FileMessageHandler
{
    public:
        FileMessageHandler();
        virtual ~FileMessageHandler();

        void HandleMessage(const SceneData&, POVMSType, POVMS_Object&, POVMS_Object&);
    protected:
        virtual bool FindFile(const list<Path>&, POVMS_Object&, POVMS_Object&);
        virtual bool ReadFile(const list<Path>&, POVMS_Object&, POVMS_Object&);
        virtual void CreatedFile(POVMS_Object&);

        Path FindFilePath(const list<Path>&, const Path&);
};

}

#endif // POVRAY_FRONTEND_FILEMESSAGEHANDLER_H
