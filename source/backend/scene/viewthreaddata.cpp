//******************************************************************************
///
/// @file backend/scene/viewthreaddata.cpp
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

#include <limits>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/scene/viewthreaddata.h"

#include "backend/scene/backendscenedata.h"
#include "backend/scene/view.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

ViewThreadData::ViewThreadData(ViewData *vd) :
    TraceThreadData(dynamic_pointer_cast<SceneData>(vd->GetSceneData())),
    viewData(vd)
{
}

ViewThreadData::~ViewThreadData()
{
}

unsigned int ViewThreadData::GetWidth() const
{
    return viewData->GetWidth();
}

unsigned int ViewThreadData::GetHeight() const
{
    return viewData->GetHeight();
}

const POVRect& ViewThreadData::GetRenderArea()
{
    return viewData->GetRenderArea();
}

}
