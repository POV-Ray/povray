//******************************************************************************
///
/// @file unix/disp.h
///
/// Abstract base class for Unix systems render preview displays.
/// Based on Windows pvdisplay.h.
///
/// @author Christoph Hormann <chris_hormann@gmx.de>
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
//*******************************************************************************

#ifndef POVRAY_UNIX_DISP_H
#define POVRAY_UNIX_DISP_H

#include <memory>

#include "vfe.h"

namespace pov_frontend
{
    using namespace vfe;

    extern std::shared_ptr<Display> gDisplay;

    class UnixDisplay : public vfeDisplay
    {
        public:
            UnixDisplay(unsigned int w, unsigned int h, vfeSession *session, bool visible) :
                vfeDisplay(w, h, session, visible) {};
            virtual ~UnixDisplay() override {} ;
            virtual void Initialise() override = 0;
            virtual void Close() override = 0;
            virtual void Show() override = 0;
            virtual void Hide() override = 0;

            virtual bool TakeOver(UnixDisplay *display) = 0;

            /**
                 To read all pending events in an interactive display system
                 and interpret them.

                 @returns true if an abort request has been made, false otherwise
             */
            virtual bool HandleEvents() = 0;

            /**
                 Called regularly by the main event loop.

                 @param Force  Indicates the Update should be immediate
                 otherwise the display does not actually have to be updated
                 during every call.
             */
            virtual void UpdateScreen(bool Force = false) = 0;

            /**
                 The following methods are sequentially called when pausing after
                 a frame has been rendered, e.g. waiting for a keypress to continue.
                 The first can be used to print a message that the display will wait
                 for user input.  The second must poll (with no wait) for user input
                 and returns whether the pause is to be resumed; the method is called
                 within a loop by the main thread.  The last method allows to notify
                 that the pause is finished.
            */
            virtual void PauseWhenDoneNotifyStart() = 0;
            virtual bool PauseWhenDoneResumeIsRequested() = 0;
            virtual void PauseWhenDoneNotifyEnd() = 0;
    };

    inline UnixDisplay *GetRenderWindow (void)
    {
        Display *p = gDisplay.get();
        if (p == nullptr)
            return nullptr;
        return dynamic_cast<UnixDisplay *>(p) ;
    }
}
// end of namespace pov_frontend

#endif // POVRAY_UNIX_DISP_H
