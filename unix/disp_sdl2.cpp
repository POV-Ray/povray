//******************************************************************************
///
/// @file unix/disp_sdl2.cpp
///
/// SDL (Simple direct media layer) based render display system, for libsdl2
///
/// @author Trevor SANDY<trevor.sandy@gmial.com>
/// @author Christoph Hormann <chris_hormann@gmx.de>
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

#include "config.h"

#ifdef HAVE_LIBSDL2

#include "disp_sdl2.h"
#include <SDL2/SDL.h>

#include <algorithm>

// this must be the last file included
#include "syspovdebug.h"


namespace pov_frontend
{
    using namespace vfe;
    using namespace vfePlatform;

    extern shared_ptr<Display> gDisplay;

    const UnixOptionsProcessor::Option_Info UnixSDL2Display::Options[] =
    {
        // command line/povray.conf/environment options of this display mode can be added here
        // section name, option name, default, has_param, command line parameter, environment variable name, help text
        // 
        // TODO option to get a window-id to use instead of default desktop
        UnixOptionsProcessor::Option_Info("", "", "", false, "", "", "") // has to be last
    };

    bool UnixSDL2Display::Register(vfeUnixSession *session)
    {
        session->GetUnixOptions()->Register(Options);
        // TODO: correct display detection
        return true;
    }

    UnixSDL2Display::UnixSDL2Display(unsigned int w, unsigned int h, vfeSession *session, bool visible) :
        UnixDisplay(w, h, session, visible)
    {
        m_valid = false;
        m_display_scaled = false;
        m_display_scale = 1.;
        m_screen = nullptr;
        m_window = nullptr;
    }

    UnixSDL2Display::~UnixSDL2Display()
    {
        Close();
    }

    void UnixSDL2Display::Initialise()
    {
        if (m_VisibleOnCreation)
            Show();
    }

    void UnixSDL2Display::Hide()
    {
      // not used, provided as sample
      SDL_HideWindow(m_window);
    }

    void UnixSDL2Display::Clear()
    {
      // not used, provided as sample
      if (m_valid)
      {
        SDL_FillRect(m_screen, NULL, (Uint32)0x7F7F7F7F);
        SDL_UpdateWindowSurface(m_window);
      }
    }

    bool UnixSDL2Display::TakeOver(UnixDisplay *display)
    {
        UnixSDL2Display *p = dynamic_cast<UnixSDL2Display *>(display);
        if (p == nullptr)
            return false;
        if ((GetWidth() != p->GetWidth()) || (GetHeight() != p->GetHeight()))
            return false;

        m_valid = p->m_valid;
        m_display_scaled = p->m_display_scaled;
        m_display_scale = p->m_display_scale;
		m_window = p->m_window;
		m_screen = p->m_screen;
    // protect against Close(), as the resources are transfered and not copied
    p->m_display_scaled=false;
    p->m_display_scale=1;
    p->m_screen = nullptr;
    p->m_window = nullptr;

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
        if (m_screen->pixels)
        {
          if (SDL_MUSTLOCK(m_screen))
            SDL_LockSurface(m_screen);
          Uint8 *p =(Uint8*)m_screen->pixels;
          uint_least64_t c = m_screen->pitch*m_screen->h;
          while(c)
          {
            // darken previous image
            *p++ >>= 2;
            --c;
          }
          if (SDL_MUSTLOCK(m_screen))
            SDL_UnlockSurface(m_screen);


          SDL_UpdateWindowSurface(m_window);


        }

        return true;
    }

    void UnixSDL2Display::Close()
    {
      if (!m_valid)
        return;

      m_PxCount.clear();
      m_valid = false;

      if(m_screen)
      {
        // Deallocate screen surface
        SDL_FreeSurface(m_screen);
        m_screen = nullptr;
      }

      if(m_window)
      {
        // Destroy window
        SDL_DestroyWindow(m_window);
        m_window = nullptr;

        // Quit subsystems
        SDL_Quit();
      }
    }

    void UnixSDL2Display::SetCaption(bool paused)
    {
        if (!m_valid)
            return;

        boost::format f;
        if (m_display_scaled)
            f = boost::format(PACKAGE_NAME " " VERSION_BASE " SDL2 (scaled at %.0f%%) %s")
                % (m_display_scale*100)
                % (paused ? " [paused]" : "");
        else
            f = boost::format(PACKAGE_NAME " " VERSION_BASE " SDL2 %s")
                % (paused ? " [paused]" : "");
		SDL_SetWindowTitle(m_window, f.str().c_str());
    }

    void UnixSDL2Display::Show()
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

        vfeUnixSession *UxSession = dynamic_cast<vfeUnixSession *>(m_Session);
        // determine desktop area
        // always scale when window is too big to fit
        {
          SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");			   // make the scaled rendering look smoother.

          SDL_DisplayMode mode;
          static int display_in_use = 0;										// only using first display
          if (SDL_GetDesktopDisplayMode(display_in_use, &mode) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't get desktop display mode: %s", SDL_GetError());
            return;
          }

          // tolerance for border, just hope the Window Manager is not larger than 10
          width = min(mode.w - 10, width);
          // tolerance for border and title bar, just hope the Window Manager is not larger than 80
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
        if (m_window == nullptr)
        {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create W%d x H%d SDL window: %s", width, height, SDL_GetError());
          return;
        }

        // Initialize the display
        m_screen = SDL_GetWindowSurface(m_window);
        if (m_screen == nullptr)
        {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't get SDL window surface: %s", SDL_GetError());
          return;
        }

        m_PxCount.clear();
        m_PxCount.reserve(width*height);
        for(vector<unsigned char>::iterator iter = m_PxCount.begin(); iter != m_PxCount.end(); iter++)
          (*iter) = 0;

        m_valid = true;
        /*
         * default background is pitch black
         */
        SDL_FillRect(m_screen, NULL, (Uint32)0x00000000);
        RGBA8 colour;
        colour.red = 255;
        colour.green = 255;
        colour.blue = 255;
        colour.alpha = 0;

        for(int i=0;i<height;++i)
        {
          for(int j = 0;j<width;++j)
          {
            if ((i^j)&4)
            {
              SetPixel( j,i, colour);
            }
          }
        }

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
           *
           * other inherent problem: m_display_scale is <= 1.0, which might be an issue
           * to represent all fraction in binary float (rounding error):
           * 1/2 is ok
           * 1/3 has an error
           * 1/4 is fine
           * 1/5 has an error
           * ... and so on
           * TODO: change for integer and use / instead of * in formula ?
           */
          m_display_scale = min(float(width) / GetWidth(), float(height) / GetHeight());
        }


        // early drawing, filled of the window, important when photons are used
        SDL_UpdateWindowSurface(m_window);
        SetCaption(false);
      }
      else
      {
        SDL_ShowWindow(m_window);
      }
    }

    void UnixSDL2Display::SetPixel(unsigned int x, unsigned int y, const RGBA8& colour)
    {
        Uint8 *p = (Uint8 *) m_screen->pixels + y * m_screen->pitch + x * m_screen->format->BytesPerPixel;

        Uint32 sdl_col = SDL_MapRGBA(m_screen->format, colour.red, colour.green, colour.blue, colour.alpha);

        switch (m_screen->format->BytesPerPixel)
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

    void UnixSDL2Display::SetPixelScaled(unsigned int x, unsigned int y, const RGBA8& colour)
    {
        unsigned int ix = x * m_display_scale;
        unsigned int iy = y * m_display_scale;

        Uint8 *p = (Uint8 *) m_screen->pixels + iy * m_screen->pitch + ix * m_screen->format->BytesPerPixel;

        Uint8 r, g, b, a;
        Uint32 old = *(Uint32 *) p;

        SDL_GetRGBA(old, m_screen->format, &r, &g, &b, &a);

        unsigned int ofs = ix + iy * m_screen->w;
        r = (r*m_PxCount[ofs] + colour.red  ) / (m_PxCount[ofs]+1);
        g = (g*m_PxCount[ofs] + colour.green) / (m_PxCount[ofs]+1);
        b = (b*m_PxCount[ofs] + colour.blue ) / (m_PxCount[ofs]+1);
        a = (a*m_PxCount[ofs] + colour.alpha) / (m_PxCount[ofs]+1);

        Uint32 sdl_col = SDL_MapRGBA(m_screen->format, r, g, b, a);

        switch (m_screen->format->BytesPerPixel)
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

    void UnixSDL2Display::DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
    {
        if (!m_valid || x >= GetWidth() || y >= GetHeight())
            return;
        if (SDL_MUSTLOCK(m_screen) && SDL_LockSurface(m_screen) < 0)
            return;

        if (m_display_scaled)
        {
            SetPixelScaled(x, y, colour);
        }
        else
        {
            SetPixel(x, y, colour);
        }


        if (SDL_MUSTLOCK(m_screen))
            SDL_UnlockSurface(m_screen);

        SDL_Rect rect;
        rect.x = x * m_display_scale;
        rect.y = y * m_display_scale;
        rect.w = 1;
        rect.h = 1;
        SDL_UpdateWindowSurfaceRects(m_window, &rect, 1);
    }

    void UnixSDL2Display::DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
    {
        if (!m_valid)
            return;

        int ix1 = min(x1, GetWidth()-1);
        int ix2 = min(x2, GetWidth()-1);
        int iy1 = min(y1, GetHeight()-1);
        int iy2 = min(y2, GetHeight()-1);

        if (SDL_MUSTLOCK(m_screen) && SDL_LockSurface(m_screen) < 0)
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
        }

        if (SDL_MUSTLOCK(m_screen))
            SDL_UnlockSurface(m_screen);

        SDL_Rect rect;
        rect.x = ix1 * m_display_scale;
        rect.y = iy1 * m_display_scale;
        // DO NOT FACTOR *m_display_scale or you will get holes
        rect.w = uint_least64_t(ix2 * m_display_scale)-uint_least64_t(ix1 * m_display_scale) +1;
        rect.h = uint_least64_t(iy2 * m_display_scale)-uint_least64_t(iy1 * m_display_scale) +1;
        SDL_UpdateWindowSurfaceRects(m_window, &rect, 1);
    }

    void UnixSDL2Display::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
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

        Uint32 sdl_col = SDL_MapRGBA(m_screen->format, colour.red, colour.green, colour.blue, colour.alpha);

        SDL_Rect tempRect;
        tempRect.x = ix1;
        tempRect.y = iy1;
        tempRect.w = ix2 - ix1 + 1;
        tempRect.h = iy2 - iy1 + 1;
        SDL_FillRect(m_screen, &tempRect, sdl_col);
        SDL_UpdateWindowSurfaceRects(m_window, &tempRect, 1);
    }

    void UnixSDL2Display::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
    {
        if (!m_valid)
            return;

        unsigned int ix1 = min(x1, GetWidth()-1);
        unsigned int ix2 = min(x2, GetWidth()-1);
        unsigned int iy1 = min(y1, GetHeight()-1);
        unsigned int iy2 = min(y2, GetHeight()-1);

        if (SDL_MUSTLOCK(m_screen) && SDL_LockSurface(m_screen) < 0)
            return;

        if (m_display_scaled)
        {
            for(unsigned int y = iy1, i = 0; y <= iy2; y++)
                for(unsigned int x = ix1; x <= ix2; x++, i++)
                    SetPixelScaled(x, y, colour[i]);
        }
        else
        {
            for(unsigned int y = y1, i = 0; y <= iy2; y++)
                for(unsigned int x = ix1; x <= ix2; x++, i++)
                    SetPixel(x, y, colour[i]);
        }

        if (SDL_MUSTLOCK(m_screen))
            SDL_UnlockSurface(m_screen);
        
        SDL_Rect rect;
        rect.x = ix1 * m_display_scale;
        rect.y = iy1 * m_display_scale;
        // DO NOT FACTOR *m_display_scale or you will get holes
        rect.w = uint_least64_t(ix2 * m_display_scale)-uint_least64_t(ix1 * m_display_scale) +1;
        rect.h = uint_least64_t(iy2 * m_display_scale)-uint_least64_t(iy1 * m_display_scale) +1;
        SDL_UpdateWindowSurfaceRects(m_window, &rect, 1);
    }

    void UnixSDL2Display::UpdateScreen(bool Force = false)
    {
        if (!m_valid)
            return;

        if (Force )
        { // test shows it does nothing... wrong thread ?
          //SDL_UpdateWindowSurface(m_window);
        }
    }

    void UnixSDL2Display::PauseWhenDoneNotifyStart()
    {
        if (!m_valid)
            return;
        fprintf(stderr, "Press p, q, enter or click the display to continue...");
        SetCaption(true);
    }

    void UnixSDL2Display::PauseWhenDoneNotifyEnd()
    {
        if (!m_valid)
            return;
        SetCaption(false);
        fprintf(stderr, "\n\n");
    }

    bool UnixSDL2Display::PauseWhenDoneResumeIsRequested()
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

    bool UnixSDL2Display::HandleEvents()
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

#endif /* HAVE_LIBSDL2 */
