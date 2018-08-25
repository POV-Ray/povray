//******************************************************************************
///
/// @file windows/pvdisplay.h
///
/// @todo   What's in here?
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
//******************************************************************************

#ifndef __PVDISPLAY_H__
#define __PVDISPLAY_H__

#include <windows.h>

#include "vfe.h"
#include "pvengine.h"

namespace pov_frontend
{
  using namespace vfe;
  using namespace povwin;

  extern shared_ptr<Display> gDisplay;

  class WinDisplay : public vfeDisplay
  {
    public:
      WinDisplay(unsigned int w, unsigned int h, GammaCurvePtr gamma, vfeSession *session, bool visible) :
          vfeDisplay(w, h, gamma, session, visible), m_Handle (NULL) {};
      virtual ~WinDisplay() {} ;
      virtual bool CreateRenderWindow (void) = 0;
      virtual void Close() = 0;
      virtual void Show() = 0;
      virtual void Hide() = 0;
      virtual bool TakeOver(WinDisplay *display) = 0;
      virtual bool IsVisible() { return (m_Handle != NULL) && (IsWindowVisible (m_Handle)) ; }
      virtual HWND GetHandle() { return m_Handle; }
      virtual void SetCaption(LPCTSTR Caption) { if (m_Handle != NULL) SetWindowText (m_Handle, Caption); }
      virtual void SetRenderState(bool IsRendering) = 0 ;

    protected:
      virtual LRESULT WindowProc (UINT message, WPARAM wParam, LPARAM lParam) = 0;
      HWND m_Handle;
      HWND m_AuxHandle;
  };

  class WinLegacyDisplay : public WinDisplay
  {
    public:
      WinLegacyDisplay(unsigned int w, unsigned int h, GammaCurvePtr gamma, vfeSession *session, bool visible);
      virtual ~WinLegacyDisplay();

      void Initialise();
      void Close();
      void Show();
      void Hide();
      bool TakeOver(WinDisplay *display);
      void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour);
      void DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
      void DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
      void DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour);
      void Clear();
      void SetRenderState(bool IsRendering);
      bool CreateRenderWindow (void);
      static LRESULT CALLBACK StaticWindowProc (HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
      static HPALETTE CreatePalette (RGBQUAD *rgb, int entries, bool use8bpp);
      BITMAPINFOHEADER& GetBMIH(void) { return m_Bitmap.header; }
      unsigned char *GetBitmapSurface() { return m_BitmapSurface; }

    protected:
      LRESULT WindowProc (UINT message, WPARAM wParam, LPARAM lParam);
      void InvalidatePixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
      inline void SetPixel (unsigned int x, unsigned int y, uchar colour);
      inline void SetPixel (unsigned int x, unsigned int y, const RGBA8 *colour);
      unsigned char *m_BitmapSurface ;
      BitmapInfo m_Bitmap ;
      static BitmapInfo m_BitmapTemplate ;
      int m_LastY;
      int m_BytesPerLine;
      HBITMAP m_ErrorBitmap ;
      int m_RBand;
      POINT m_RB1;
      POINT m_RB2;
      int m_LastMouseX;
      int m_LastMouseY;
      bool m_Depth8Bit;
      unsigned m_MaxWidth;
      unsigned m_MaxHeight;
      bool m_Rendering;
      bool m_Initialised;
  };

  inline HWND GetRenderwinHandle (void)
  {
    Display *p = gDisplay.get();
    if (p == NULL)
      return 0;
    WinDisplay *wd = dynamic_cast<WinDisplay *>(p) ;
    if (wd == NULL)
      return 0;
    return wd->GetHandle();
  }

  inline WinDisplay *GetRenderWindow (void)
  {
    Display *p = gDisplay.get();
    if (p == NULL)
      return NULL;
    return dynamic_cast<WinDisplay *>(p) ;
  }
}

#endif
