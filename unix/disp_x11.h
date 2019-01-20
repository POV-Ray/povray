//******************************************************************************
///
/// @file unix/disp_x11.h
///
/// X11 based render display system
///
/// @author Jerome Grimbert <jgrimbert@free.fr>
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
//*******************************************************************************

#ifndef POVRAY_UNIX_DISP_X11_H
#define POVRAY_UNIX_DISP_X11_H

#ifndef X_DISPLAY_MISSING

#include "vfe.h"
#include "unixoptions.h"
#include "disp.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#ifdef USE_CURSOR
#include <X11/cursorfont.h>
#endif


namespace pov_frontend
{
    using namespace vfe;
    using namespace vfePlatform;

    /**
     * X11 display for rendering window
     *
     * \note there is no handling of Alpha in core X11, XRender does handle alpha
     * but that would be another display.
     */
    class UnixX11Display : public UnixDisplay
    {
        public:
            static const UnixOptionsProcessor::Option_Info Options[];
            static bool Register(vfeUnixSession *session);

            UnixX11Display(unsigned int w, unsigned int h, vfeSession *session, bool visible);
            virtual ~UnixX11Display();
            virtual void Initialise();
            virtual void Close();
            virtual void Show();
            virtual void Hide();
            virtual bool TakeOver(UnixDisplay *display);
            virtual bool HandleEvents();
            virtual void UpdateScreen(bool Force);
            virtual void PauseWhenDoneNotifyStart();
            virtual bool PauseWhenDoneResumeIsRequested();
            virtual void PauseWhenDoneNotifyEnd();
            // from vfeDisplay
            virtual void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour);
            virtual void DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
            virtual void DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
            virtual void DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour);
            virtual void Clear();

        protected:
            void SetCaption(bool paused);

            /// Sets the color of a pixel in a non-scaled image.
            void SetPixel(unsigned int x, unsigned int y, const RGBA8& colour);
            /**
                 @brief Sets the color of a pixel in a scaled-down image.

                 If the pixel is already filled the color is mixed.
            */
            void SetPixelScaled(unsigned int x, unsigned int y, const RGBA8& colour);

            bool m_valid;
            bool m_display_scaled;
            float m_display_scale;
            // X11 specific data
            // ::Display * theDisplay;
            Window    theWindow;
            GC        theGC;
            Colormap  theColormap;
            XImage *  theImage;
#ifdef USE_CURSOR
            Cursor theCursor;
#endif
            // shifting amount to compute colours
            uint_least8_t rs, gs, bs;
            /// for mixing colors in scaled down display
            vector<unsigned char> m_PxCount;
    };
}

#endif /* X_DISPLAY_MISSING */

#endif // POVRAY_UNIX_DISP_SDL_H
