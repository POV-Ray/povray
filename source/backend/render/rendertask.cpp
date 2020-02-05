//******************************************************************************
///
/// @file backend/render/rendertask.cpp
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
#include "backend/render/rendertask.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// Boost header files
#include <boost/bind.hpp>

// POV-Ray header files (base module)
#include "base/types.h"
#include "base/timer.h"

// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmsid.h"

// POV-Ray header files (backend module)
#include "backend/scene/backendscenedata.h"
#include "backend/scene/view.h"
#include "backend/scene/viewthreaddata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using namespace pov_base;

RenderTask::RenderTask(ViewData *vd, size_t seed, const char* sn, RenderBackend::ViewId vid) :
    SceneTask(new ViewThreadData(vd, seed), boost::bind(&RenderTask::SendFatalError, this, _1), sn, vd->GetSceneData(), vid),
    viewData(vd)
{
}

RenderTask::~RenderTask()
{
}

std::shared_ptr<BackendSceneData>& RenderTask::GetSceneData()
{
    return viewData->GetSceneData();
}

ViewData *RenderTask::GetViewData()
{
    return viewData;
}

void RenderTask::SendFatalError(Exception& e)
{
    // if the front-end has been told about this exception already, we don't tell it again
    if (e.frontendnotified(true))
        return;

    POVMS_Message msg(kPOVObjectClass_ControlData, kPOVMsgClass_ViewOutput, kPOVMsgIdent_Error);

    msg.SetString(kPOVAttrib_EnglishText, e.what());
    msg.SetInt(kPOVAttrib_Error, 0);
    msg.SetInt(kPOVAttrib_ViewId, viewData->GetViewId());
    msg.SetSourceAddress(viewData->GetSceneData()->backendAddress);
    msg.SetDestinationAddress(viewData->GetSceneData()->frontendAddress);

    POVMS_SendMessage(msg);
}

}
// end of namespace pov
