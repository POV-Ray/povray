//******************************************************************************
///
/// @file frontend/imagemessagehandler.h
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

#ifndef POVRAY_FRONTEND_IMAGEMESSAGEHANDLER_H
#define POVRAY_FRONTEND_IMAGEMESSAGEHANDLER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "frontend/configfrontend.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"

// POV-Ray header files (frontend module)
#include "frontend/renderfrontend_fwd.h"

namespace pov_frontend
{

using namespace pov_base;

class ImageMessageHandler
{
    public:
        ImageMessageHandler();
        virtual ~ImageMessageHandler();

        void HandleMessage(const SceneData&, const ViewData&, POVMSType, POVMS_Object&);
    protected:
        virtual void DrawPixelSet(const SceneData&, const ViewData&, POVMS_Object&, bool final);
        virtual void DrawPixelBlockSet(const SceneData&, const ViewData&, POVMS_Object&, bool final);
        virtual void DrawPixelRowSet(const SceneData&, const ViewData&, POVMS_Object&, bool final);
        virtual void DrawRectangleFrameSet(const SceneData&, const ViewData&, POVMS_Object&, bool final);
        virtual void DrawFilledRectangleSet(const SceneData&, const ViewData&, POVMS_Object&, bool final);
};

}
// end of namespace pov_frontend

#endif // POVRAY_FRONTEND_IMAGEMESSAGEHANDLER_H
