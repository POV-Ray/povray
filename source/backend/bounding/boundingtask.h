//******************************************************************************
///
/// @file backend/bounding/boundingtask.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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
//*******************************************************************************

#ifndef POVRAY_BACKEND_BOUNDINGTASK_H
#define POVRAY_BACKEND_BOUNDINGTASK_H

#include <vector>

#include <boost/thread.hpp>

#include "backend/frame.h"
#include "backend/render/rendertask.h"

namespace pov
{

class SceneData;
class SceneThreadData;

class BoundingTask : public SceneTask
{
    public:
        BoundingTask(shared_ptr<SceneData> sd, unsigned int bt);
        virtual ~BoundingTask();

        virtual void Run();
        virtual void Stopped();
        virtual void Finish();

        void AppendObject(ObjectPtr p);

        inline SceneThreadData *GetSceneDataPtr() { return reinterpret_cast<SceneThreadData *>(GetDataPtr()); }
    private:
        shared_ptr<SceneData> sceneData;
        unsigned int boundingThreshold;

        void SendFatalError(pov_base::Exception& e);
};

}

#endif // POVRAY_BACKEND_BOUNDINGTASK_H
