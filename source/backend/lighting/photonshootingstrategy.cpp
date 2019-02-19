//******************************************************************************
///
/// @file backend/lighting/photonshootingstrategy.cpp
///
/// @todo   What's in here?
///
/// Author: Nathan Kopp
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
#include "backend/lighting/photonshootingstrategy.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/bounding/boundingbox.h"
#include "core/lighting/lightgroup.h"
#include "core/lighting/lightsource.h"
#include "core/lighting/photons.h"
#include "core/material/normal.h"
#include "core/material/pigment.h"
#include "core/material/texture.h"
#include "core/math/matrix.h"
#include "core/scene/object.h"
#include "core/shape/csg.h"
#include "core/support/octree.h"

// POV-Ray header files (POVMS module)
// POV-Ray header files (backend module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

void PhotonShootingStrategy::start()
{
    iter = units.begin();
}

PhotonShootingUnit* PhotonShootingStrategy::getNextUnit()
{
    std::lock_guard<std::mutex> lock(nextUnitMutex);
    if (iter == units.end())
        return nullptr;
    PhotonShootingUnit* unit = *iter;
    iter++;
    return unit;
}

void PhotonShootingStrategy::createUnitsForCombo(ObjectPtr obj, LightSource* light, std::shared_ptr<SceneData> sceneData)
{
    PhotonShootingUnit* unit = new PhotonShootingUnit(light, obj);
    unit->lightAndObject.computeAnglesAndDeltas(sceneData);
    units.push_back(unit);
}

PhotonShootingStrategy::~PhotonShootingStrategy()
{
    std::vector<PhotonShootingUnit*>::iterator delIter;
    for(delIter = units.begin(); delIter != units.end(); delIter++)
    {
        delete (*delIter);
    }
    units.clear();
}

}
// end of namespace pov
