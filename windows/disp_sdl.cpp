//******************************************************************************
///
/// @file windows/disp_sdl.cpp
///
/// SDL (Simple direct media layer) based render display system.
///
/// @author Christoph Hormann <chris_hormann@gmx.de>
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
//*******************************************************************************

#include "syspovconfig.h"

#ifdef HAVE_LIBSDL

#include "disp_sdl.h"

#include <algorithm>

// this must be the last file included
#include "syspovdebug.h"

namespace pov_frontend
{
    using namespace vfe;
    using namespace vfePlatform;

    extern shared_ptr<Display> gDisplay;

    const WinConOptionsProcessor::Option_Info WinConSDLDisplay::Options[] =
    {
        // command line/povray.conf/environment options of this display mode can be added here
        // section name, option name, default, has_param, command line parameter, environment variable name, help text
        WinConOptionsProcessor::Option_Info("display", "scaled", "on", false, "", "POV_DISPLAY_SCALED", "scale render view to fit screen"),
        WinConOptionsProcessor::Option_Info("", "", "", false, "", "", "") // has to be last
    };

    bool WinConSDLDisplay::Register(vfeWinSession *session)
    {
        session->GetWinConOptions()->Register(Options);
        // TODO: correct display detection
        return true;
    }

    WinConSDLDisplay::WinConSDLDisplay(unsigned int w, unsigned int h, GammaCurvePtr gamma, vfeSession *session, bool visible) :
        WinConDisplay(w, h, gamma, session, visible)
    {
        m_valid = false;
        m_display_scaled = false;
        m_display_scale = 1.;
        m_screen = NULL;
        m_display = NULL;
		m_window = NULL;
    }

    WinConSDLDisplay::~WinConSDLDisplay()
    {
        Close();
    }

    void WinConSDLDisplay::Initialise()
    {
        if (m_VisibleOnCreation)
            Show();
    }

    void WinConSDLDisplay::Hide()
    {
    }

    bool WinConSDLDisplay::TakeOver(WinConDisplay *display)
    {
        WinConSDLDisplay *p = dynamic_cast<WinConSDLDisplay *>(display);
        if (p == NULL)
            return false;
        if ((GetWidth() != p->GetWidth()) || (GetHeight() != p->GetHeight()))
            return false;

        m_valid = p->m_valid;
        m_display_scaled = p->m_display_scaled;
        m_display_scale = p->m_display_scale;
		m_window = p->m_window;
        m_display = p->m_display;
		m_screen = p->m_screen;

        if (m_display_scaled)
        {
            int width = GetWidth();
            int height = GetHeight();
            // allocate a new pixel counters, dropping influence of previous picture
            m_PxCount.clear(); // not useful, vector was created empty, just to be sure
            m_PxCount.reserve(width*height); // we need that, and the loop!
            for(vector<unsigned char>::iterator iter = m_PxCount.begin(); iter != m_PxCount.end(); iter++)
                (*iter) = 0;
        }

        return true;
    }

    void WinConSDLDisplay::Close()
    {
        if (!m_valid)
            return;

        m_PxCount.clear();
        m_valid = false;

		// Deallocate surface
		SDL_FreeSurface(m_display);
		m_display = NULL;

		// Destroy window
		SDL_DestroyWindow(m_window);
		m_window = NULL;

		// Quit subsystems
		SDL_Quit();
    }

    void WinConSDLDisplay::SetCaption(bool paused)
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
		SDL_SetWindowTitle(m_window, f.str().c_str());
    }

    void WinConSDLDisplay::Show()
    {
        if (gDisplay.get() != this)
            gDisplay = m_Session->GetDisplay();

        if (!m_valid)
        {
            // Initialize SDL
			if (SDL_Init(SDL_INIT_VIDEO) < 0)
            {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
                return;
            }

            int width = GetWidth();
            int height = GetHeight();

            vfeWinSession *UxSession = dynamic_cast<vfeWinSession *>(m_Session);

			// determine desktop area
			if (UxSession->GetWinConOptions()->isOptionSet("display", "scaled"))
			{
				SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");			   // make the scaled rendering look smoother.

				SDL_DisplayMode mode;
				static int display_in_use = 0;										// only using first display
				if (SDL_GetDesktopDisplayMode(display_in_use, &mode) < 0) {
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't get desktop display mode: %s", SDL_GetError());
					return;
				}

				//if the window was created with SDL_WINDOW_ALLOW_HIGHDPI on a
				//platform with high-dpi support (e.g. iOS or OS X). Use SDL_GL_GetDrawableSize() or
				//SDL_GetRendererOutputSize() to get the real client area size in pixels.

				width = min(mode.w - 10, width);
				height = min(mode.h - 80, height);
			}

            // calculate display area
            float AspectRatio = float(width)/float(height);
            float AspectRatio_Full = float(GetWidth())/float(GetHeight());
            if (AspectRatio > AspectRatio_Full)
                width = int(AspectRatio_Full*float(height));
            else if (AspectRatio != AspectRatio_Full)
                height = int(float(width)/AspectRatio_Full);

			// create display window
			m_window = SDL_CreateWindow(PACKAGE_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
			if (m_window == NULL)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create W%d x H%d SDL window: %s", width, height, SDL_GetError());
				return;
			}

            // Initialize the display
			m_screen = SDL_GetWindowSurface(m_window);
			if (m_screen == NULL)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't get SDL window surface: %s", SDL_GetError());
				return;
			}

			int depth = 32;
			int unused = 0;
			Uint32 Rmask, Gmask, Bmask, Amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			Rmask = 0xff000000;
			Gmask = 0x00ff0000;
			Bmask = 0x0000ff00;
			Amask = 0x000000ff;
#else
			Rmask = 0x000000ff;
			Gmask = 0x0000ff00;
			Bmask = 0x00ff0000;
			Amask = 0xff000000;
#endif
			SDL_Surface *optimized_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, depth, Rmask, Gmask, Bmask, Amask);
			if (optimized_surface == NULL)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create optimized RGB surface for repeated blitting:: %s", SDL_GetError());
				return;
			}

			SDL_PixelFormat* pixelFormat = m_screen->format;
			Uint32 pixelFormatEnum = pixelFormat->format;
			m_display = SDL_ConvertSurfaceFormat(optimized_surface, pixelFormatEnum, unused);
			SDL_FreeSurface(optimized_surface);
			if (m_display == NULL)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't convert to optimized RGB display surface for repeated blitting:: %s", SDL_GetError());
				return;
			}

            m_PxCount.clear();
            m_PxCount.reserve(width*height);
            for(vector<unsigned char>::iterator iter = m_PxCount.begin(); iter != m_PxCount.end(); iter++)
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
                m_display_scale = min(float(width) / GetWidth(), float(height) / GetHeight());
            }

            SetCaption(false);
        }
    }

    inline void WinConSDLDisplay::SetPixel(unsigned int x, unsigned int y, const RGBA8& colour)
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

    inline void WinConSDLDisplay::SetPixelScaled(unsigned int x, unsigned int y, const RGBA8& colour)
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

    void WinConSDLDisplay::UpdateCoord(unsigned int x, unsigned int y)
    {
        unsigned int rx2 = m_update_rect.x + m_update_rect.w;
        unsigned int ry2 = m_update_rect.y + m_update_rect.h;
        m_update_rect.x = min((unsigned int)m_update_rect.x, x);
        m_update_rect.y = min((unsigned int)m_update_rect.y, y);
        rx2 = max(rx2, x);
        ry2 = max(ry2, y);
        m_update_rect.w = rx2 - m_update_rect.x;
        m_update_rect.h = ry2 - m_update_rect.y;
    }

    void WinConSDLDisplay::UpdateCoord(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
    {
        unsigned int rx2 = m_update_rect.x + m_update_rect.w;
        unsigned int ry2 = m_update_rect.y + m_update_rect.h;
        m_update_rect.x = min((unsigned int)m_update_rect.x, x1);
        m_update_rect.y = min((unsigned int)m_update_rect.y, y1);
        rx2 = max(rx2, x2);
        ry2 = max(ry2, y2);
        m_update_rect.w = rx2 - m_update_rect.x;
        m_update_rect.h = ry2 - m_update_rect.y;
    }

    void WinConSDLDisplay::UpdateCoordScaled(unsigned int x, unsigned int y)
    {
        UpdateCoord(static_cast<unsigned int>(x * m_display_scale), static_cast<unsigned int>(y * m_display_scale));
    }

    void WinConSDLDisplay::UpdateCoordScaled(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
    {
        UpdateCoord(static_cast<unsigned int>(x1 * m_display_scale), static_cast<unsigned int>(y1 * m_display_scale),
                    static_cast<unsigned int>(x2 * m_display_scale), static_cast<unsigned int>(y2 * m_display_scale));
    }

    void WinConSDLDisplay::DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
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

    void WinConSDLDisplay::DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
    {
        if (!m_valid)
            return;

        int ix1 = min(x1, GetWidth()-1);
        int ix2 = min(x2, GetWidth()-1);
        int iy1 = min(y1, GetHeight()-1);
        int iy2 = min(y2, GetHeight()-1);

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

    void WinConSDLDisplay::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
    {
        if (!m_valid)
            return;

        unsigned int ix1 = min(x1, GetWidth()-1);
        unsigned int ix2 = min(x2, GetWidth()-1);
        unsigned int iy1 = min(y1, GetHeight()-1);
        unsigned int iy2 = min(y2, GetHeight()-1);

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

    void WinConSDLDisplay::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
    {
        if (!m_valid)
            return;

        unsigned int ix1 = min(x1, GetWidth()-1);
        unsigned int ix2 = min(x2, GetWidth()-1);
        unsigned int iy1 = min(y1, GetHeight()-1);
        unsigned int iy2 = min(y2, GetHeight()-1);

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

    void WinConSDLDisplay::Clear()
    {
        for(vector<unsigned char>::iterator iter = m_PxCount.begin(); iter != m_PxCount.end(); iter++)
            (*iter) = 0;

        m_update_rect.x = 0;
        m_update_rect.y = 0;
        m_update_rect.w = m_display->w;
        m_update_rect.h = m_display->h;

        SDL_FillRect(m_display, &m_update_rect, (Uint32)0);

        m_PxCnt = UpdateInterval;
    }

    void WinConSDLDisplay::UpdateScreen(bool Force = false)
    {
        if (!m_valid)
            return;
        if (Force || m_PxCnt >= UpdateInterval)
        {
			if (SDL_BlitSurface(m_display, &m_update_rect, m_screen, &m_update_rect) < 0)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't copy display surface to screen surface: %s", SDL_GetError());
				exit(1);
			}

			if (SDL_UpdateWindowSurface(m_window) < 0)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't update window surface: %s", SDL_GetError());
				exit(1);
			}

            m_PxCnt = 0;
        }
    }

    void WinConSDLDisplay::PauseWhenDoneNotifyStart()
    {
        if (!m_valid)
            return;
        fprintf(stderr, "Press p, q, enter or click the display to continue...");
        SetCaption(true);
    }

    void WinConSDLDisplay::PauseWhenDoneNotifyEnd()
    {
        if (!m_valid)
            return;
        SetCaption(false);
        fprintf(stderr, "\n\n");
    }

    bool WinConSDLDisplay::PauseWhenDoneResumeIsRequested()
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

    bool WinConSDLDisplay::HandleEvents()
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
                    else if ( event.key.keysym.sym == SDLK_p)
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

#endif /* HAVE_LIBSDL */
