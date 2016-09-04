//******************************************************************************
///
/// @file backend/render/rendertask.h
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
//******************************************************************************

#ifndef POVRAY_BACKEND_RENDERTASK_H
#define POVRAY_BACKEND_RENDERTASK_H

#include "backend/support/task.h"

namespace pov
{

class SceneData;
class ViewData;
class ViewThreadData;

class RenderTask : public SceneTask
{
    public:
        RenderTask(ViewData *vd, const char* sn, RenderBackend::ViewId vid = 0);
        virtual ~RenderTask();
    protected:
        virtual void Run() = 0;
        virtual void Stopped() = 0;

        shared_ptr<BackendSceneData>& GetSceneData();
        ViewData *GetViewData();

        inline ViewThreadData *GetViewDataPtr() { return reinterpret_cast<ViewThreadData *>(GetDataPtr()); }
    private:
        /// view data
        ViewData *viewData;

        void SendFatalError(pov_base::Exception& e);
};

}

#endif // POVRAY_BACKEND_RENDERTASK_H
