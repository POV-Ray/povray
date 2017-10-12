//******************************************************************************
///
/// @file windows/disp_sdl.h
///
/// SDL (Simple direct media layer) based render display system.
///
/// @author Trevor SANDY<trevor.sandy@gmial.com>
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

#ifdef HAVE_LIBSDL

#ifndef _DISP_SDL_H
#define _DISP_SDL_H

#include "vfe.h"
#include "console\winoptions.h"
#include "disp.h"

#include <SDL.h>
/**
*  On Windows, to launch an SDL window without the console window you have to declare WinMain()
*  instead of main() so SDL looks for and replaces main() with SDL_main() and then provides
*  a WinMain() in a static library that calls SDL_main().
*
*  So for SDL on Windows, a main() function must be declared using C linkage like this:
*/
#ifdef __cplusplus
extern "C" {
#endif
    int main(int argc, char **argv);
#ifdef __cplusplus
};
#endif

namespace pov_frontend
{
    using namespace vfe;
    using namespace vfePlatform;

    class WinConSDLDisplay : public WinConDisplay
    {
    public:
        static const WinConOptionsProcessor::Option_Info Options[];
        static bool Register(vfeWinSession *session);

        WinConSDLDisplay(unsigned int w, unsigned int h, GammaCurvePtr gamma, vfeSession *session, bool visible);
        virtual ~WinConSDLDisplay();
        void Initialise();
        void Close();
        void Show();
        void Hide();
        bool TakeOver(WinConDisplay *display);
        void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour);
        void DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
        void DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
        void DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour);
        void Clear();
        bool HandleEvents();
        void UpdateScreen(bool Force);
        void PauseWhenDoneNotifyStart();
        bool PauseWhenDoneResumeIsRequested();
        void PauseWhenDoneNotifyEnd();
        protected:
            /// Number of Pixels before the display is updated
            static const unsigned int UpdateInterval = 100;

            void SetCaption(bool paused);

            /// Sets the color of a pixel in a non-scaled image.
            inline void SetPixel(unsigned int x, unsigned int y, const RGBA8& colour);
            /**
                 @brief Sets the color of a pixel in a scaled-down image.

                 If the pixel is already filled the color is mixed.
            */
            inline void SetPixelScaled(unsigned int x, unsigned int y, const RGBA8& colour);
            /// Makes a pixel coordinate part of the update rectangle
            void UpdateCoord(unsigned int x, unsigned int y);
            /// Makes a rectangle part of the update rectangle
            void UpdateCoord(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
            /// Makes a pixel coordinate part of the update rectangle (scaled down image version)
            void UpdateCoordScaled(unsigned int x, unsigned int y);
            /// Makes a rectangle part of the update rectangle (scaled down image version)
            void UpdateCoordScaled(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

            bool m_Initialized;
            bool m_valid;
            bool m_display_scaled;
            float m_display_scale;
            /// for update interval
            unsigned int m_PxCnt;
            SDL_Window   *m_window;
            SDL_Surface  *m_screen;
            SDL_Surface  *m_display;
            SDL_Rect m_screen_rect;
            SDL_Rect m_update_rect;
            /// for mixing colors in scaled down display
            vector<unsigned char> m_PxCount;
    };
}

#endif /* _DISP_SDL_H */

#endif /* HAVE_LIBSDL */
