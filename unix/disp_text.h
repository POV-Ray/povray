//******************************************************************************
///
/// @file unix/disp_text.h
///
/// Template for a text mode render display system.
///
/// @author Christoph Hormann <chris_hormann@gmx.de>
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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
//*******************************************************************************

#ifndef POVRAY_UNIX_DISP_TEXT_H
#define POVRAY_UNIX_DISP_TEXT_H

#include "vfe.h"
#include "unixoptions.h"
#include "disp.h"

namespace pov_frontend
{
    using namespace vfe;
    using namespace vfePlatform;

    class UnixTextDisplay : public UnixDisplay
    {
        public:
            static const UnixOptionsProcessor::Option_Info Options[];
            static bool Register(vfeUnixSession *session);

            UnixTextDisplay(unsigned int w, unsigned int h, GammaCurvePtr gamma, vfeSession *session, bool visible) :
                UnixDisplay(w, h, gamma, session, visible) {};
            virtual ~UnixTextDisplay() {} ;
            void Initialise() {};
            void Close() {};
            void Show() {};
            void Hide() {};
            bool TakeOver(UnixDisplay *display) { return false; };
            void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour);
            bool HandleEvents() { return false; };
            void UpdateScreen(bool Force = false) {};
            void PauseWhenDoneNotifyStart() {};
            bool PauseWhenDoneResumeIsRequested() { return true; };
            void PauseWhenDoneNotifyEnd() {};
    };
}

#endif // POVRAY_UNIX_DISP_TEXT_H
