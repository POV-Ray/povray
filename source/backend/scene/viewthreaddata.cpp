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
#include "backend/scene/viewthreaddata.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <limits>
#include <memory>

// POV-Ray header files (base module)
// POV-Ray header files (core module)
// POV-Ray header files (POVMS module)
//  (none at the moment)

// POV-Ray header files (backend module)
#include "backend/scene/backendscenedata.h"
#include "backend/scene/view.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

ViewThreadData::ViewThreadData(ViewData *vd, size_t seed) :
    TraceThreadData(std::dynamic_pointer_cast<SceneData>(vd->GetSceneData()), seed),
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
// end of namespace pov
