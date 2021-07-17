//******************************************************************************
///
/// @file backend/render/rendertask.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2021 Persistence of Vision Raytracer Pty. Ltd.
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
//------------------------------------------------------------------------------
// SPDX-License-Identifier: AGPL-3.0-or-later
//******************************************************************************

#ifndef POVRAY_BACKEND_RENDERTASK_H
#define POVRAY_BACKEND_RENDERTASK_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"

// POV-Ray header files (backend module)
#include "backend/support/task.h"

namespace pov
{

class SceneData;
class ViewData;
class ViewThreadData;

class RenderTask : public SceneTask
{
    public:
        RenderTask(ViewData *vd, size_t seed, const char* sn, RenderBackend::ViewId vid = 0);
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

} // end of namespace

#endif // POVRAY_BACKEND_RENDERTASK_H
