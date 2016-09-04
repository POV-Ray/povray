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

#include <algorithm>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/lighting/photonshootingstrategy.h"

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
    boost::mutex::scoped_lock lock(nextUnitMutex);
    if(iter == units.end()) return NULL;
    PhotonShootingUnit* unit = *iter;
    iter++;
    return unit;
}

void PhotonShootingStrategy::createUnitsForCombo(ObjectPtr obj, LightSource* light, shared_ptr<SceneData> sceneData)
{
    PhotonShootingUnit* unit = new PhotonShootingUnit(light, obj);
    unit->lightAndObject.computeAnglesAndDeltas(sceneData);
    units.push_back(unit);
}

PhotonShootingStrategy::~PhotonShootingStrategy()
{
    vector<PhotonShootingUnit*>::iterator delIter;
    for(delIter = units.begin(); delIter != units.end(); delIter++)
    {
        delete (*delIter);
    }
    units.clear();
}


}
