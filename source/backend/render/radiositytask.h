//******************************************************************************
///
/// @file backend/render/radiositytask.h
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

#ifndef POVRAY_BACKEND_RADIOSITYTASK_H
#define POVRAY_BACKEND_RADIOSITYTASK_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/lighting/photons.h"
#include "core/lighting/radiosity.h"
#include "core/material/media.h"
#include "core/render/tracepixel.h"

// POV-Ray header files (backend module)
#include "backend/render/rendertask.h"
#include "backend/scene/view.h"

namespace pov
{

class RadiosityTask final : public RenderTask
{
    public:
        RadiosityTask(ViewData *vd, DBL ptsz, DBL ptesz, unsigned int pts, unsigned int ptsc, unsigned int nt,
                      size_t seed);
        virtual ~RadiosityTask() override;

        virtual void Run() override;
        virtual void Stopped() override;
        virtual void Finish() override;
    private:
        class CooperateFunction final : public Trace::CooperateFunctor
        {
            public:
                CooperateFunction(Task& t) : task(t) { }
                virtual void operator()() override { task.Cooperate(); }
            private:
                Task& task;
        };
        struct RadiositySubBlockInfo final
        {
            RadiositySubBlockInfo(unsigned short x, unsigned short y): subBlockPosX(x), subBlockPosY(y) {}
            unsigned short subBlockPosX, subBlockPosY;
        };
        class RadiosityBlockInfo final : public ViewData::BlockInfo
        {
            public:
                RadiosityBlockInfo(): pass(0), subBlockCountX(1), subBlockCountY(1), completion(0.0)
                {
                    incompleteSubBlocks.push_back(RadiositySubBlockInfo(0,0));
                }
                unsigned short pass;
                unsigned short subBlockCountX;
                unsigned short subBlockCountY;
                std::deque<RadiositySubBlockInfo> incompleteSubBlocks;
                float completion;
        };

        /// tracing core
        TracePixel trace;
        /// pretrace start size
        DBL pretraceStartSize;
        /// pretrace end size
        DBL pretraceEndSize;
        /// pretrace coverage threshold for adaptive pretrace
        float pretraceCoverage;

        CooperateFunction cooperate;
        MediaFunction media;
        RadiosityFunction radiosity;
        PhotonGatherer photonGatherer;

        unsigned int pretraceStep;
        unsigned int pretraceStepCount;
        int nominalThreads;
};

}
// end of namespace pov

#endif // POVRAY_BACKEND_RADIOSITYTASK_H
