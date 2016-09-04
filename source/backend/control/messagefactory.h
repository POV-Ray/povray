//******************************************************************************
///
/// @file backend/control/messagefactory.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#include "base/pov_err.h"

#include "backend/control/renderbackend.h"

namespace pov
{

using namespace pov_base;

enum WarningLevel
{
    /// Value used for general warnings.
    kWarningGeneral  = 1,
    /// Value used for general language version specific warning.
    kWarningLanguage = 6
};

class MessageFactory : public CoreMessenger
{
    public:

        MessageFactory(unsigned int wl, const char *sn, POVMSAddress saddr, POVMSAddress daddr, RenderBackend::SceneId sid, RenderBackend::ViewId vid);
        virtual ~MessageFactory();

        virtual void CoreMessage(CoreMessageClass mc, const char *format,...);
        virtual void CoreMessageAt(CoreMessageClass mc, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);

        void Warning(WarningLevel level, const char *format,...);
        void WarningAt(WarningLevel level, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);

        void PossibleError(const char *format,...);
        void PossibleErrorAt(const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);

        void Error(const char *format,...);
        void Error(const Exception& ex, const char *format,...);
        void Error(Exception& ex, const char *format,...);
        void ErrorAt(const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);
        void ErrorAt(const Exception& ex, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);
        void ErrorAt(Exception& ex, const UCS2 *filename, POV_LONG line, POV_LONG column, POV_LONG offset, const char *format, ...);
        void SetWarningLevel(unsigned int Val) { warningLevel = Val ; } // TODO FIXME - not here, not this way

    private:
        unsigned int warningLevel;
        const char *stageName;
        POVMSAddress sourceAddress;
        POVMSAddress destinationAddress;
        RenderBackend::SceneId sceneId;
        RenderBackend::ViewId viewId;

        void CleanupString(char *str);
        std::string SendError(const char *format, va_list arglist, const UCS2 *filename = NULL, POV_LONG line = -1, POV_LONG column = -1, POV_LONG offset = -1);
};

}

#endif // POVRAY_BACKEND_MESSAGEFACTORY_H
