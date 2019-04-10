//******************************************************************************
///
/// @file backend/bounding/boundingtask.h
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

#ifndef POVRAY_BACKEND_BOUNDINGTASK_H
#define POVRAY_BACKEND_BOUNDINGTASK_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// POV-Ray header files (base module)
#include "base/base_fwd.h"

// POV-Ray header files (core module)
#include "core/core_fwd.h"

// POV-Ray header files (backend module)
#include "backend/scene/backendscenedata_fwd.h"
#include "backend/support/task.h"

namespace pov
{

class BoundingTask final : public SceneTask
{
    public:
        BoundingTask(std::shared_ptr<BackendSceneData> sd, unsigned int bt, size_t seed);
        virtual ~BoundingTask() override;

        virtual void Run() override;
        virtual void Stopped() override;
        virtual void Finish() override;

        void AppendObject(ObjectPtr p);

        inline TraceThreadData *GetSceneDataPtr() { return reinterpret_cast<TraceThreadData *>(GetDataPtr()); }
    private:
        std::shared_ptr<BackendSceneData> sceneData;
        unsigned int boundingThreshold;

        void SendFatalError(pov_base::Exception& e);
};

}
// end of namespace pov

#endif // POVRAY_BACKEND_BOUNDINGTASK_H
