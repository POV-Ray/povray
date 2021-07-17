//******************************************************************************
///
/// @file backend/lighting/photonshootingstrategy.h
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

#ifndef POVRAY_BACKEND_PHOTONSHOOTINGSTRATEGY_H
#define POVRAY_BACKEND_PHOTONSHOOTINGSTRATEGY_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"

// Boost header files
#if POV_MULTITHREADED
#include <boost/thread.hpp>
#endif

// POV-Ray header files (core module)
#include "core/coretypes.h"

namespace pov
{

using namespace pov_base;

class PhotonShootingUnit;
class SceneData;

class PhotonShootingStrategy
{
    public:
        ObjectPtr obj;
        LightSource *light;

        vector<PhotonShootingUnit*> units;

        void createUnitsForCombo(ObjectPtr obj, LightSource* light, shared_ptr<SceneData> sceneData);
        void start();
        PhotonShootingUnit* getNextUnit();

        virtual ~PhotonShootingStrategy();

    private:
        vector<PhotonShootingUnit*>::iterator iter;
        boost::mutex nextUnitMutex;

};

} // end of namespace

#endif // POVRAY_BACKEND_PHOTONSHOOTINGSTRATEGY_H
