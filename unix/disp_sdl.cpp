//******************************************************************************
///
/// @file unix/disp_sdl.cpp
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

#include "config.h"

#ifdef HAVE_LIBSDL

#include "disp_sdl.h"

#include <algorithm>
#include <memory>

#include <boost/format.hpp>

// this must be the last file included
#include "syspovdebug.h"


namespace pov_frontend
{
    using namespace vfe;
    using namespace vfePlatform;

    extern std::shared_ptr<Display> gDisplay;

    const UnixOptionsProcessor::Option_Info UnixSDLDisplay::Options[] =
    {
        // command line/povray.conf/environment options of this display mode can be added here
        // section name, option name, default, has_param, command line parameter, environment variable name, help text
        UnixOptionsProcessor::Option_Info("display", "scaled", "on", false, "", "POV_DISPLAY_SCALED", "scale render view to fit screen"),
        UnixOptionsProcessor::Option_Info("", "", "", false, "", "", "") // has to be last
    };

    bool UnixSDLDisplay::Register(vfeUnixSession *session)
    {
        session->GetUnixOptions()->Register(Options);
        // TODO: correct display detection
        return true;
    }

    UnixSDLDisplay::UnixSDLDisplay(unsigned int w, unsigned int h, vfeSession *session, bool visible) :
        UnixDisplay(w, h, session, visible)
    {
        m_valid = false;
        m_display_scaled = false;
        m_display_scale = 1.;
        m_screen = nullptr;
        m_display = nullptr;
    }

    UnixSDLDisplay::~UnixSDLDisplay()
    {
        Close();
    }

    void UnixSDLDisplay::Initialise()
    {
        if (m_VisibleOnCreation)
            Show();
    }

    void UnixSDLDisplay::Hide()
    {
    }

    bool UnixSDLDisplay::TakeOver(UnixDisplay *display)
    {
        UnixSDLDisplay *p = dynamic_cast<UnixSDLDisplay *>(display);
        if (p == nullptr)
            return false;
        if ((GetWidth() != p->GetWidth()) || (GetHeight() != p->GetHeight()))
            return false;

        m_valid = p->m_valid;
        m_display_scaled = p->m_display_scaled;
        m_display_scale = p->m_display_scale;
        m_screen = p->m_screen;
        m_display = p->m_display;

        if (m_display_scaled)
        {
            int width = GetWidth();
            int height = GetHeight();
            // allocate a new pixel counters, dropping influence of previous picture
            m_PxCount.clear(); // not useful, vector was created empty, just to be sure
            m_PxCount.reserve(width*height); // we need that, and the loop!
            for(std::vector<unsigned char>::iterator iter = m_PxCount.begin(); iter != m_PxCount.end(); iter++)
                (*iter) = 0;
        }

        return true;
    }

    void UnixSDLDisplay::Close()
    {
        if (!m_valid)
            return;

// FIXME: should handle this correctly for the last frame
//      SDL_FreeSurface(m_display);
//      SDL_Quit();
        m_PxCount.clear();
        m_valid = false;
    }

    void UnixSDLDisplay::SetCaption(bool paused)
    {
        if (!m_valid)
            return;

        boost::format f;
        if (m_display_scaled)
            f = boost::format(PACKAGE_NAME " " VERSION_BASE " SDL display (scaled at %.0f%%)%s")
                % (m_display_scale*100)
                % (paused ? " [paused]" : "");
        else
            f = boost::format(PACKAGE_NAME " " VERSION_BASE " SDL display%s")
                % (paused ? " [paused]" : "");
        // FIXME: SDL_WM_SetCaption() causes locks on some distros, see http://bugs.povray.org/23
        // FIXME: SDL_WM_SetCaption(f.str().c_str(), PACKAGE_NAME);
    }

    void UnixSDLDisplay::Show()
    {
        if (gDisplay.get() != this)
            gDisplay = m_Session->GetDisplay();

        if (!m_valid)
        {
            // Initialize SDL
            if ( SDL_Init(SDL_INIT_VIDEO) != 0 )
            {
                fprintf(stderr, "Couldn't initialize SDL: %s.\n", SDL_GetError());
                return;
            }

            int desired_bpp = 0;
            Uint32 video_flags = 0;
            int width = GetWidth();
            int height = GetHeight();

            vfeUnixSession *UxSession = dynamic_cast<vfeUnixSession *>(m_Session);

            if (UxSession->GetUnixOptions()->isOptionSet("display", "scaled"))
            // determine maximum display area (wrong and ugly)
            {
                SDL_Rect **modes = SDL_ListModes(nullptr, SDL_FULLSCREEN);
                // [JG] about testing vs ...(-1), have a look at SDL_ListModes API (the return is very ugly).
                if ((modes != nullptr) && (reinterpret_cast<SDL_Rect**>(-1) != modes))
                {
                    width = std::min(modes[0]->w - 10, width);
                    height = std::min(modes[0]->h - 80, height);
                }
            }

            // calculate display area
            float AspectRatio = float(width)/float(height);
            float AspectRatio_Full = float(GetWidth())/float(GetHeight());
            if (AspectRatio > AspectRatio_Full)
                width = int(AspectRatio_Full*float(height));
            else if (AspectRatio != AspectRatio_Full)
                height = int(float(width)/AspectRatio_Full);

            // Initialize the display
            m_screen = SDL_SetVideoMode(width, height, desired_bpp, video_flags);
            if (m_screen == nullptr)
            {
                fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n", width, height, desired_bpp, SDL_GetError());
                return;
            }

            SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

            if (temp == nullptr)
            {
                fprintf(stderr, "Couldn't create render display surface: %s\n", SDL_GetError());
                return;
            }

            m_display = SDL_DisplayFormat(temp);
            SDL_FreeSurface(temp);

            if (m_display == nullptr)
            {
                fprintf(stderr, "Couldn't convert to optimized surface for repeated blitting: %s\n", SDL_GetError());
                return;
            }

            m_PxCount.clear();
            m_PxCount.reserve(width*height);
            for(std::vector<unsigned char>::iterator iter = m_PxCount.begin(); iter != m_PxCount.end(); iter++)
                (*iter) = 0;

            m_update_rect.x = 0;
            m_update_rect.y = 0;
            m_update_rect.w = width;
            m_update_rect.h = height;

            m_screen_rect.x = 0;
            m_screen_rect.y = 0;
            m_screen_rect.w = width;
            m_screen_rect.h = height;

            m_valid = true;
            m_PxCnt = UpdateInterval;

            if ((width == GetWidth()) && (height == GetHeight()))
            {
                m_display_scaled = false;
                m_display_scale = 1.;
            }
            else
            {
                m_display_scaled = true;
                /* [JG] the scaling factor between the requested resolution and the actual window is the same in both direction
                 * yet, the factor (as a float) need the smallest value to avoid an access out of the two buffers for the pixels.
                 * The difference is nearly invisible until the values of GetWidth and GetHeight are subtil (such as +W2596 +H1003 on a display of 1920 x 1080)
                 * where in such situation, the computed ratio is not exactly the same as the other.
                 */
                m_display_scale = std::min(float(width) / GetWidth(), float(height) / GetHeight());
            }

            SetCaption(false);
        }
    }

    inline void UnixSDLDisplay::SetPixel(unsigned int x, unsigned int y, const RGBA8& colour)
    {
        Uint8 *p = (Uint8 *) m_display->pixels + y * m_display->pitch + x * m_display->format->BytesPerPixel;

        Uint32 sdl_col = SDL_MapRGBA(m_display->format, colour.red, colour.green, colour.blue, colour.alpha);

        switch (m_display->format->BytesPerPixel)
        {
            case 1:
                *p = sdl_col;
                break;
            case 2:
                *(Uint16 *) p = sdl_col;
                break;
            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                {
                    p[0] = (sdl_col >> 16) & 0xFF;
                    p[1] = (sdl_col >>  8) & 0xFF;
                    p[2] =  sdl_col        & 0xFF;
                }
                else
                {
                    p[0] =  sdl_col        & 0xFF;
                    p[1] = (sdl_col >>  8) & 0xFF;
                    p[2] = (sdl_col >> 16) & 0xFF;
                }
                break;
            case 4:
                *(Uint32 *) p = sdl_col;
                break;
        }
    }

    inline void UnixSDLDisplay::SetPixelScaled(unsigned int x, unsigned int y, const RGBA8& colour)
    {
        unsigned int ix = x * m_display_scale;
        unsigned int iy = y * m_display_scale;

        Uint8 *p = (Uint8 *) m_display->pixels + iy * m_display->pitch + ix * m_display->format->BytesPerPixel;

        Uint8 r, g, b, a;
        Uint32 old = *(Uint32 *) p;

        SDL_GetRGBA(old, m_display->format, &r, &g, &b, &a);

        unsigned int ofs = ix + iy * m_display->w;
        r = (r*m_PxCount[ofs] + colour.red  ) / (m_PxCount[ofs]+1);
        g = (g*m_PxCount[ofs] + colour.green) / (m_PxCount[ofs]+1);
        b = (b*m_PxCount[ofs] + colour.blue ) / (m_PxCount[ofs]+1);
        a = (a*m_PxCount[ofs] + colour.alpha) / (m_PxCount[ofs]+1);

        Uint32 sdl_col = SDL_MapRGBA(m_display->format, r, g, b, a);

        switch (m_display->format->BytesPerPixel)
        {
            case 1:
                *p = sdl_col;
                break;
            case 2:
                *(Uint16 *) p = sdl_col;
                break;
            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                {
                    p[0] = (sdl_col >> 16) & 0xFF;
                    p[1] = (sdl_col >>  8) & 0xFF;
                    p[2] =  sdl_col        & 0xFF;
                }
                else
                {
                    p[0] =  sdl_col        & 0xFF;
                    p[1] = (sdl_col >>  8) & 0xFF;
                    p[2] = (sdl_col >> 16) & 0xFF;
                }
                break;
            case 4:
                *(Uint32 *) p = sdl_col;
                break;
        }

        ++m_PxCount[ofs];
    }

    void UnixSDLDisplay::UpdateCoord(unsigned int x, unsigned int y)
    {
        unsigned int rx2 = m_update_rect.x + m_update_rect.w;
        unsigned int ry2 = m_update_rect.y + m_update_rect.h;
        m_update_rect.x = std::min((unsigned int)m_update_rect.x, x);
        m_update_rect.y = std::min((unsigned int)m_update_rect.y, y);
        rx2 = std::max(rx2, x);
        ry2 = std::max(ry2, y);
        m_update_rect.w = rx2 - m_update_rect.x;
        m_update_rect.h = ry2 - m_update_rect.y;
    }

    void UnixSDLDisplay::UpdateCoord(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
    {
        unsigned int rx2 = m_update_rect.x + m_update_rect.w;
        unsigned int ry2 = m_update_rect.y + m_update_rect.h;
        m_update_rect.x = std::min((unsigned int)m_update_rect.x, x1);
        m_update_rect.y = std::min((unsigned int)m_update_rect.y, y1);
        rx2 = std::max(rx2, x2);
        ry2 = std::max(ry2, y2);
        m_update_rect.w = rx2 - m_update_rect.x;
        m_update_rect.h = ry2 - m_update_rect.y;
    }

    void UnixSDLDisplay::UpdateCoordScaled(unsigned int x, unsigned int y)
    {
        UpdateCoord(static_cast<unsigned int>(x * m_display_scale), static_cast<unsigned int>(y * m_display_scale));
    }

    void UnixSDLDisplay::UpdateCoordScaled(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
    {
        UpdateCoord(static_cast<unsigned int>(x1 * m_display_scale), static_cast<unsigned int>(y1 * m_display_scale),
                    static_cast<unsigned int>(x2 * m_display_scale), static_cast<unsigned int>(y2 * m_display_scale));
    }

    void UnixSDLDisplay::DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
    {
        if (!m_valid || x >= GetWidth() || y >= GetHeight())
            return;
        if (SDL_MUSTLOCK(m_display) && SDL_LockSurface(m_display) < 0)
            return;

        if (m_display_scaled)
        {
            SetPixelScaled(x, y, colour);
            UpdateCoordScaled(x, y);
        }
        else
        {
            SetPixel(x, y, colour);
            UpdateCoord(x, y);
        }

        m_PxCnt++;

        if (SDL_MUSTLOCK(m_display))
            SDL_UnlockSurface(m_display);
    }

    void UnixSDLDisplay::DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
    {
        if (!m_valid)
            return;

        int ix1 = std::min(x1, GetWidth()-1);
        int ix2 = std::min(x2, GetWidth()-1);
        int iy1 = std::min(y1, GetHeight()-1);
        int iy2 = std::min(y2, GetHeight()-1);

        if (SDL_MUSTLOCK(m_display) && SDL_LockSurface(m_display) < 0)
            return;

        if (m_display_scaled)
        {
            for(unsigned int x = ix1; x <= ix2; x++)
            {
                SetPixelScaled(x, iy1, colour);
                SetPixelScaled(x, iy2, colour);
            }

            for(unsigned int y = iy1; y <= iy2; y++)
            {
                SetPixelScaled(ix1, y, colour);
                SetPixelScaled(ix2, y, colour);
            }
            UpdateCoordScaled(ix1, iy1, ix2, iy2);
        }
        else
        {
            for(unsigned int x = ix1; x <= ix2; x++)
            {
                SetPixel(x, iy1, colour);
                SetPixel(x, iy2, colour);
            }

            for(unsigned int y = iy1; y <= iy2; y++)
            {
                SetPixel(ix1, y, colour);
                SetPixel(ix2, y, colour);
            }
            UpdateCoord(ix1, iy1, ix2, iy2);
        }

        if (SDL_MUSTLOCK(m_display))
            SDL_UnlockSurface(m_display);

        m_PxCnt = UpdateInterval;
    }

    void UnixSDLDisplay::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
    {
        if (!m_valid)
            return;

        unsigned int ix1 = std::min(x1, GetWidth()-1);
        unsigned int ix2 = std::min(x2, GetWidth()-1);
        unsigned int iy1 = std::min(y1, GetHeight()-1);
        unsigned int iy2 = std::min(y2, GetHeight()-1);

        if (m_display_scaled)
        {
            ix1 *= m_display_scale;
            iy1 *= m_display_scale;
            ix2 *= m_display_scale;
            iy2 *= m_display_scale;
        }

        UpdateCoord(ix1, iy1, ix2, iy2);

        Uint32 sdl_col = SDL_MapRGBA(m_display->format, colour.red, colour.green, colour.blue, colour.alpha);

        SDL_Rect tempRect;
        tempRect.x = ix1;
        tempRect.y = iy1;
        tempRect.w = ix2 - ix1 + 1;
        tempRect.h = iy2 - iy1 + 1;
        SDL_FillRect(m_display, &tempRect, sdl_col);

        m_PxCnt = UpdateInterval;
    }

    void UnixSDLDisplay::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
    {
        if (!m_valid)
            return;

        unsigned int ix1 = std::min(x1, GetWidth()-1);
        unsigned int ix2 = std::min(x2, GetWidth()-1);
        unsigned int iy1 = std::min(y1, GetHeight()-1);
        unsigned int iy2 = std::min(y2, GetHeight()-1);

        if (SDL_MUSTLOCK(m_display) && SDL_LockSurface(m_display) < 0)
            return;

        if (m_display_scaled)
        {
            for(unsigned int y = iy1, i = 0; y <= iy2; y++)
                for(unsigned int x = ix1; x <= ix2; x++, i++)
                    SetPixelScaled(x, y, colour[i]);
            UpdateCoordScaled(ix1, iy1, ix2, iy2);
        }
        else
        {
            for(unsigned int y = y1, i = 0; y <= iy2; y++)
                for(unsigned int x = ix1; x <= ix2; x++, i++)
                    SetPixel(x, y, colour[i]);
            UpdateCoord(ix1, iy1, ix2, iy2);
        }

        if (SDL_MUSTLOCK(m_display))
            SDL_UnlockSurface(m_display);

        m_PxCnt = UpdateInterval;
    }

    void UnixSDLDisplay::Clear()
    {
        for(std::vector<unsigned char>::iterator iter = m_PxCount.begin(); iter != m_PxCount.end(); iter++)
            (*iter) = 0;

        m_update_rect.x = 0;
        m_update_rect.y = 0;
        m_update_rect.w = m_display->w;
        m_update_rect.h = m_display->h;

        SDL_FillRect(m_display, &m_update_rect, (Uint32)0);

        m_PxCnt = UpdateInterval;
    }

    void UnixSDLDisplay::UpdateScreen(bool Force = false)
    {
        if (!m_valid)
            return;
        if (Force || m_PxCnt >= UpdateInterval)
        {
            SDL_BlitSurface(m_display, &m_update_rect, m_screen, &m_update_rect);
            SDL_UpdateRect(m_screen, m_update_rect.x, m_update_rect.y, m_update_rect.w, m_update_rect.h);
            m_PxCnt = 0;
        }
    }

    void UnixSDLDisplay::PauseWhenDoneNotifyStart()
    {
        if (!m_valid)
            return;
        fprintf(stderr, "Press p, q, enter or click the display to continue...");
        SetCaption(true);
    }

    void UnixSDLDisplay::PauseWhenDoneNotifyEnd()
    {
        if (!m_valid)
            return;
        SetCaption(false);
        fprintf(stderr, "\n\n");
    }

    bool UnixSDLDisplay::PauseWhenDoneResumeIsRequested()
    {
        if (!m_valid)
            return true;

        SDL_Event event;
        bool do_quit = false;

        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    if ( event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_q ||
                         event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER )
                        do_quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    do_quit = true;
                    break;
            }
        }

        return do_quit;
    }

    bool UnixSDLDisplay::HandleEvents()
    {
        if (!m_valid)
            return false;

        SDL_Event event;
        bool do_quit = false;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    if ( event.key.keysym.sym == SDLK_q )
                        do_quit = true;
                    else if ( event.key.keysym.sym == SDLK_p )
                    {
                        if (!m_Session->IsPausable())
                            break;

                        if (m_Session->Paused())
                        {
                            if (m_Session->Resume())
                                SetCaption(false);
                        }
                        else
                        {
                            if (m_Session->Pause())
                                SetCaption(true);
                        }
                    }
                    break;
                case SDL_QUIT:
                    do_quit = true;
                    break;
            }
            if (do_quit)
                break;
        }

        return do_quit;
    }

}
// end of namespace pov_frontend

#endif /* HAVE_LIBSDL */
