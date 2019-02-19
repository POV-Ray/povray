//******************************************************************************
///
/// @file unix/disp_sdl.h
///
/// SDL (Simple direct media layer) based render display system.
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

#ifndef POVRAY_UNIX_DISP_SDL_H
#define POVRAY_UNIX_DISP_SDL_H

#ifdef HAVE_LIBSDL

#include <vector>

#include "vfe.h"
#include "unixoptions.h"
#include "disp.h"

#include <SDL/SDL.h>

namespace pov_frontend
{
    using namespace vfe;
    using namespace vfePlatform;

    class UnixSDLDisplay final : public UnixDisplay
    {
        public:
            static const UnixOptionsProcessor::Option_Info Options[];
            static bool Register(vfeUnixSession *session);

            UnixSDLDisplay(unsigned int w, unsigned int h, vfeSession *session, bool visible);
            virtual ~UnixSDLDisplay() override;
            virtual void Initialise() override;
            virtual void Close() override;
            virtual void Show() override;
            virtual void Hide() override;
            virtual bool TakeOver(UnixDisplay *display) override;
            virtual void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour) override;
            virtual void DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour) override;
            virtual void DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour) override;
            virtual void DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour) override;
            virtual void Clear() override;
            virtual bool HandleEvents() override;
            virtual void UpdateScreen(bool Force) override;
            virtual void PauseWhenDoneNotifyStart() override;
            virtual bool PauseWhenDoneResumeIsRequested() override;
            virtual void PauseWhenDoneNotifyEnd() override;

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
            SDL_Surface *m_screen;
            SDL_Surface *m_display;
            SDL_Rect m_screen_rect;
            SDL_Rect m_update_rect;
            /// for mixing colors in scaled down display
            std::vector<unsigned char> m_PxCount;
    };
}
// end of namespace pov_frontend

#endif /* HAVE_LIBSDL */

#endif // POVRAY_UNIX_DISP_SDL_H
