/*******************************************************************************
 * photonshootingstrategy.cpp
 *
 * Author: Nathan Kopp
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/povray/smp/source/backend/lighting/photonshootingstrategy.cpp $
 * $Revision: #14 $
 * $Change: 6113 $
 * $DateTime: 2013/11/20 20:39:54 $
 * $Author: clipka $
 *******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "base/povms.h"
#include "base/povmsgid.h"
#include "backend/math/vector.h"
#include "backend/math/matrices.h"
#include "backend/scene/objects.h"
#include "backend/shape/csg.h"
#include "backend/support/octree.h"
#include "backend/bounding/bbox.h"
#include "backend/scene/threaddata.h"
#include "backend/scene/scene.h"
#include "backend/scene/view.h"
#include "backend/support/msgutil.h"
#include "backend/lighting/point.h"
#include "backend/lighting/photons.h"
#include "backend/texture/normal.h"
#include "backend/texture/pigment.h"
#include "backend/texture/texture.h"
#include "lightgrp.h"

#include "photonshootingstrategy.h"

#include <algorithm>

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
