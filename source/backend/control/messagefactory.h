//******************************************************************************
///
/// @file backend/control/messagefactory.h
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

#ifndef POVRAY_BACKEND_MESSAGEFACTORY_H
#define POVRAY_BACKEND_MESSAGEFACTORY_H

#include "povms/povmscpp.h"
#include "povms/povmsid.h"

#include "base/messenger.h"

#include "backend/control/renderbackend.h"

namespace pov
{

using namespace pov_base;

class MessageFactory : public GenericMessenger
{
    public:

        MessageFactory(unsigned int wl, const char *sn, POVMSAddress saddr, POVMSAddress daddr, RenderBackend::SceneId sid, RenderBackend::ViewId vid);
        virtual ~MessageFactory();

    private:

        POVMSAddress sourceAddress;
        POVMSAddress destinationAddress;
        RenderBackend::SceneId sceneId;
        RenderBackend::ViewId viewId;

        virtual void SendMessage(MessageClass mc, WarningLevel level, const char *text,
                                 const UCS2 *filename = NULL, POV_LONG line = -1, POV_LONG column = -1, POV_OFF_T offset = -1);
};

}

#endif // POVRAY_BACKEND_MESSAGEFACTORY_H
