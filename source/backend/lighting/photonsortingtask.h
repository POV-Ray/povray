//******************************************************************************
///
/// @file backend/lighting/photonsortingtask.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef PHOTONSORTINGTASK_H
#define PHOTONSORTINGTASK_H

#include "core/render/trace.h"

#include "backend/frame.h"
#include "backend/render/rendertask.h"

namespace pov
{

using namespace pov_base;

class PhotonMap;
class PhotonShootingStrategy;

class PhotonSortingTask : public RenderTask
{
    public:
        Timer timer;

        vector<PhotonMap*> surfaceMaps;
        vector<PhotonMap*> mediaMaps;
        PhotonShootingStrategy* strategy;

        PhotonSortingTask(ViewData *vd, const vector<PhotonMap*>& surfaceMaps, const vector<PhotonMap*>& mediaMaps,
                          PhotonShootingStrategy* strategy, size_t seed);
        ~PhotonSortingTask();

        void Run();
        void Stopped();
        void Finish();

        void SendProgress();

        void sortPhotonMap();
        bool save();
        bool load();
    private:
        class CooperateFunction : public Trace::CooperateFunctor
        {
            public:
                CooperateFunction(Task& t) : task(t) { }
                virtual void operator()() { task.Cooperate(); }
            private:
                Task& task;
        };

        CooperateFunction cooperate;
};

}
#endif
