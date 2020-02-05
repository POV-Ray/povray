//******************************************************************************
///
/// @file windows/pvdisplay.cpp
///
/// @todo   What's in here?
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
//******************************************************************************

#define POVWIN_FILE
#define DECLARE_TABLES

#include <windows.h>

#include "pvengine.h"
#include "pvfrontend.h"
#include "pvdisplay.h"
#include "resource.h"
#include "pvedit.h"
#include "pvguiext.h"
#include "pvdialog.h"

#ifdef RTR_SUPPORT
  #include "rtrsupport.h"
#endif

// this must be the last file included
#include "syspovdebug.h"

#ifndef MAX
    #define MAX(a,b) ((a>b)?a:b)
#endif
#ifndef MIN
    #define MIN(a,b) ((a>b)?b:a)
#endif

namespace povwin
{

char                    PovLegacyRenderWinClass [] = CLASSNAMEPREFIX "LegacyRenderWinClass" ;

int                     renderwin_xoffset ;
int                     renderwin_yoffset ;
int                     renderwin_left = CW_USEDEFAULT ;
int                     renderwin_top = CW_USEDEFAULT ;
bool                    MakeRenderwinActive ;
unsigned                renderwin_flags = 0 ;

extern int              screen_origin_x ;
extern int              screen_origin_y ;
extern int              virtual_screen_width ;
extern int              virtual_screen_height ;
extern int              render_bitmap_depth ;
extern bool             rendering_animation ;
extern bool             preserve_bitmap ;
extern bool             first_frame ;
extern bool             RenderwinIsChild ;
extern bool             main_window_hidden ;
extern bool             running_demo ;
extern bool             demo_mode ;
extern bool             benchmark_mode;
extern bool             temp_render_region ;
extern bool             UseAlpha ;
extern char             RegionStr [128] ;
extern char             TempRegionStr [128] ;
extern char             command_line [_MAX_PATH * 3] ;
extern HWND             main_window ;
extern HWND             toolbar_cmdline ;
extern HMENU            hPopupMenus ;
extern unsigned         screen_width ;
extern unsigned         screen_height ;
extern HPALETTE         hPalApp ;
extern HPALETTE         hPalBitmap ;
extern WINDOWPLACEMENT  mainwin_placement ;

}
// end of namespace povwin

////////////////////////////////////////////////////////////////////////////////////////
//
// class WinLegacyDisplay
//
////////////////////////////////////////////////////////////////////////////////////////

namespace pov_frontend
{

using namespace povwin;

std::shared_ptr<Display> gDisplay;

BitmapInfo WinLegacyDisplay::m_BitmapTemplate;

WinLegacyDisplay::WinLegacyDisplay(unsigned int w, unsigned int h, vfeSession *session, bool visible) :
  WinDisplay(w, h, session, visible)
{
  m_BitmapSurface = NULL;
  m_LastY = 0;
  m_BytesPerLine = 0;
  m_ErrorBitmap = NULL;
  m_RBand = 0;
  m_LastMouseX = -1;
  m_LastMouseY = -1;
  m_Depth8Bit = render_bitmap_depth <= 8 ;
  m_MaxWidth = 0;
  m_MaxHeight = 0;
  m_Rendering = true;
  m_Initialised = false;
}

WinLegacyDisplay::~WinLegacyDisplay()
{
  Close();
}

bool WinLegacyDisplay::TakeOver(WinDisplay *display)
{
  WinLegacyDisplay *p = dynamic_cast<WinLegacyDisplay *>(display);
  if (p == NULL)
    return (false);
  if (m_Initialised == true)
    return (false);
  if (m_Depth8Bit != p->m_Depth8Bit)
    return (false);
  if ((GetWidth() != p->GetWidth()) || (GetHeight() != p->GetHeight()))
    return (false);

  // TODO: protect this with a mutex
  m_Bitmap = p->m_Bitmap;
  m_BitmapSurface = p->m_BitmapSurface;
  m_Handle = p->m_Handle;
  m_RBand = p->m_RBand;
  m_RB1 = p->m_RB1;
  m_RB2 = p->m_RB2;
  m_LastMouseX = p->m_LastMouseX;
  m_LastMouseY = p->m_LastMouseY;
  m_BytesPerLine = p->m_BytesPerLine;
  m_Depth8Bit = p->m_Depth8Bit;
  m_MaxWidth = p->m_MaxWidth;
  m_MaxHeight = p->m_MaxHeight;
  m_Initialised = true;

  SetWindowLongPtr (m_Handle, 0, LONG_PTR (this));
  p->m_Handle = NULL ;
  p->m_BitmapSurface = NULL;

  return (true) ;
}

bool WinLegacyDisplay::CreateRenderWindow (void)
{
  int                   width ;
  int                   height ;
  RECT                  rect ;
  unsigned              flags ;

  if (m_Handle != NULL)
    DestroyWindow (m_Handle) ;
  m_Handle = NULL ;

  renderwin_xoffset = renderwin_yoffset = 0 ;
  renderwin_flags = 0 ;
  rect.left = 0 ;
  rect.top = 0 ;
  rect.right = GetWidth() ;
  rect.bottom = GetHeight() ;
  flags = WS_OVERLAPPEDWINDOW ;
  AdjustWindowRectEx (&rect, flags, false, 0) ;

  // left and top will probably be negative
  width = m_MaxWidth = rect.right - rect.left ;
  height = m_MaxHeight = rect.bottom - rect.top ;

  if (width > screen_width - 64)
    width = screen_width - 64 ;
  if (height > screen_height - 48)
    height = screen_height - 48 ;
  if (renderwin_left < screen_origin_x)
    renderwin_left = screen_origin_x ;
  if (renderwin_top < screen_origin_y)
    renderwin_top = screen_origin_y ;
  if (renderwin_left + width > virtual_screen_width + screen_origin_x)
    renderwin_left = virtual_screen_width - width + screen_origin_x ;
  if (renderwin_top + height > virtual_screen_height + screen_origin_y)
    renderwin_top = virtual_screen_height - height + screen_origin_y ;

  if (m_BitmapSurface != NULL)
  {
    free (m_BitmapSurface) ;
    m_BitmapSurface = NULL ;
  }
  m_Bitmap = m_BitmapTemplate ;
  m_Bitmap.header.biSize = sizeof (BITMAPINFOHEADER) ;
  m_Bitmap.header.biWidth = GetWidth() ;
  m_Bitmap.header.biHeight = GetHeight() ;
  m_Bitmap.header.biPlanes = 1 ;
  m_Bitmap.header.biBitCount = m_Depth8Bit ? 8 : 24 ;
  m_Bitmap.header.biCompression = BI_RGB ;
  if (m_Depth8Bit)
  {
    // round out the bits per line to a multiple of four
    m_BytesPerLine = (GetWidth() + 3) & ~3 ;
    m_Bitmap.header.biClrUsed = 256 ;
  }
  else
  {
    m_BytesPerLine = (GetWidth() * 3 + 3) & ~3 ;
    m_Bitmap.header.biClrUsed = 0 ;
  }
  m_Bitmap.header.biSizeImage = m_BytesPerLine * GetHeight() ;
  m_Bitmap.header.biClrImportant = 0 ;
  m_BitmapSurface = new unsigned char [m_Bitmap.header.biSizeImage];
  if (m_BitmapSurface == NULL)
    return (false);

  Clear () ;

  if ((m_Handle = CreateWindowEx (0,
                                  PovLegacyRenderWinClass,
                                  "POV-Ray",
                                  flags | renderwin_flags,
                                  renderwin_left,
                                  renderwin_top,
                                  width,
                                  height,
                                  RenderwinIsChild ? main_window : NULL,
                                  NULL,
                                  hInstance,
                                  NULL)) == NULL)
  {
    return (false) ;
  }

  // store a pointer to this instance for the window procedure
  SetWindowLongPtr (m_Handle, 0, LONG_PTR (this));

  if (width < m_MaxWidth)
  {
    SetScrollRange (m_Handle, SB_HORZ, 0, GetWidth() - width, false) ;
    SetScrollPos (m_Handle, SB_HORZ, 0, true) ;
  }

  if (height < m_MaxHeight)
  {
    SetScrollRange (m_Handle, SB_VERT, 0, GetHeight() - height, false) ;
    SetScrollPos (m_Handle, SB_VERT, 0, true) ;
  }

  if (!main_window_hidden)
  {
    mainwin_placement.length = sizeof (WINDOWPLACEMENT) ;
    GetWindowPlacement (main_window, &mainwin_placement) ;
    if (mainwin_placement.showCmd != SW_SHOWMINIMIZED)
    {
      // we don't set SWP_SHOWWINDOW at this point
      flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOCOPYBITS | SWP_NOACTIVATE ;
      if ((GetForegroundWindow () == main_window) && RenderwinIsChild)
        flags &= ~SWP_NOACTIVATE ;
      if (MakeRenderwinActive)
        flags &= ~SWP_NOACTIVATE ;
      SetWindowPos (m_Handle, main_window, 0, 0, 0, 0, flags) ;
    }
    else
      renderwin_flags = WS_MINIMIZE ;
  }

  SetClassLongPtr (m_Handle, GCLP_HCURSOR, (LONG_PTR) LoadCursor (NULL, m_Rendering ? IDC_ARROW : IDC_CROSS)) ;

  InvalidateRect (m_Handle, NULL, false) ;
  return (true) ;
}

void WinLegacyDisplay::Initialise()
{
  if (m_Initialised)
    return;
  m_Initialised = true;
  // we do this so that the window is created in the context of the main UI thread
  if (SendMessage (main_window, CREATE_RENDERWIN_MESSAGE, 0, LPARAM (this)) == false)
    return ;
  if (m_VisibleOnCreation)
    Show();
  ExternalEvent (EventDisplayInit, MAKELONG (GetWidth(), GetHeight())) ;
}

void WinLegacyDisplay::Close()
{
  if (m_Handle != NULL)
  {
    // we can't destroy the window ourselves as we most likely aren't the owning thread
    SendMessage (main_window, WM_COMMAND, CM_RENDERDESTROY, (LPARAM) m_Handle);
    m_Handle = NULL ;
    PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
    PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
  }
  if (m_BitmapSurface != NULL)
  {
    delete[] m_BitmapSurface ;
    m_BitmapSurface = NULL ;
    PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  }
  if (m_ErrorBitmap != NULL)
  {
    DeleteObject (m_ErrorBitmap) ;
    m_ErrorBitmap = NULL ;
  }
}

void WinLegacyDisplay::Show()
{
  if (gDisplay.get() != this)
    gDisplay = m_Session->GetDisplay();
  if (m_Handle != NULL)
  {
    ShowWindow (m_Handle, SW_SHOW) ;
    PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
    PVEnableMenuItem (CM_RENDERCLOSE, MF_ENABLED) ;
  }
}

void WinLegacyDisplay::Hide()
{
  if (m_Handle != NULL)
  {
    ShowWindow (m_Handle, SW_HIDE) ;
    PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
    PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
  }
}

inline void WinLegacyDisplay::SetPixel (unsigned int x, unsigned int y, const pov_frontend::Display::RGBA8 *colour)
{
  if (m_Depth8Bit)
  {
    unsigned char *p = m_BitmapSurface + (m_Bitmap.header.biHeight - 1 - y) * m_BytesPerLine + x ;
    unsigned char dither = dither8x8 [((x & 7) << 3) | (y & 7)] ;
    *p = 20 + div51 [colour->red] + (mod51 [colour->red] > dither) +
              mul6 [div51 [colour->green] + (mod51 [colour->green] > dither)] +
              mul36 [div51 [colour->blue] + (mod51 [colour->blue] > dither)] ;
  }
  else
  {
    unsigned char *p = m_BitmapSurface + (m_Bitmap.header.biHeight - 1 - y) * m_BytesPerLine + x * 3 ;
    if (pov_frontend::UseAlpha && colour->alpha < 255)
    {
      unsigned int backColor = (x & 8) == (y & 8) ? (unsigned int) (255 - colour->alpha) * 0xff :
                                                    (unsigned int) (255 - colour->alpha) * 0xc0 ;
      *p++ = (unsigned char) (((unsigned int) colour->blue * colour->alpha + backColor) / 255) ;
      *p++ = (unsigned char) (((unsigned int) colour->green * colour->alpha + backColor) / 255) ;
      *p++ = (unsigned char) (((unsigned int) colour->red * colour->alpha + backColor) / 255) ;
    }
    else
    {
      *p++ = colour->blue ;
      *p++ = colour->green ;
      *p++ = colour->red ;
    }
  }
}

inline void WinLegacyDisplay::SetPixel (unsigned int x, unsigned int y, unsigned char colour)
{
  if (m_Depth8Bit)
  {
    unsigned char *p = m_BitmapSurface + (m_Bitmap.header.biHeight - 1 - y) * m_BytesPerLine + x ;
    unsigned char dither = dither8x8 [((x & 7) << 3) | (y & 7)] ;
    *p = 20 + div51 [colour] + (mod51 [colour] > dither) +
              mul6 [div51 [colour] + (mod51 [colour] > dither)] +
              mul36 [div51 [colour] + (mod51 [colour] > dither)] ;
  }
  else
  {
    unsigned char *p = m_BitmapSurface + (m_Bitmap.header.biHeight - 1 - y) * m_BytesPerLine + x * 3 ;
    *p++ = colour ;
    *p++ = colour ;
    *p++ = colour ;
  }
}

void WinLegacyDisplay::DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
{
  unsigned char         Red = colour.red ;
  unsigned char         Green = colour.green ;
  unsigned char         Blue = colour.blue ;
  unsigned char         Alpha = colour.alpha ;

  ExternalDisplayPlot (x, y, Red, Green, Blue, Alpha) ;
  if ((int) x == -1 || (int) y == -1)
  {
    m_LastY = 0 ;
    return ;
  }
  if (x >= GetWidth() || y >= GetHeight())
    return ;
  if (m_BitmapSurface == NULL)
    return ;
  InvalidatePixelBlock (0, m_LastY, GetWidth(), m_LastY) ;
  m_LastY = y ;
  SetPixel (x, y, &colour) ;
}

void WinLegacyDisplay::DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
  if (m_BitmapSurface == NULL)
    return ;

  // try to keep the lines coherent to avoid unnecessary calls to InvalidatePixelBlock()
  for (unsigned int x = x1; x <= x2; x++)
    SetPixel(x, y1, &colour);
  for (unsigned int x = x1; x <= x2; x++)
    SetPixel(x, y2, &colour);
  for (unsigned int y = y1; y <= y2; y++)
  {
    SetPixel(x1, y, &colour);
    SetPixel(x2, y, &colour);
  }
  InvalidatePixelBlock (x1, y1, x2, y1) ;
  InvalidatePixelBlock (x1, y1, x1, y2) ;
  InvalidatePixelBlock (x1, y2, x2, y2) ;
  InvalidatePixelBlock (x2, y1, x2, y2) ;
}

void WinLegacyDisplay::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
  if (GetWidth() == 0 || GetHeight() == 0)
    return;

  if (x2 >= GetWidth())
    x2 = GetWidth() - 1 ;

  if (y2 >= GetHeight())
    y2 = GetHeight() - 1 ;

  if (x1 > x2 || y1 > y2)
    return ;

  unsigned char         Red = colour.red ;
  unsigned char         Green = colour.green ;
  unsigned char         Blue = colour.blue ;
  unsigned char         Alpha = colour.alpha ;

  ExternalDisplayPlotRect (x1, y1, x2, y2, Red, Green, Blue, Alpha) ;

  if (m_BitmapSurface == NULL)
    return ;

  for (unsigned int y = y1 ; y <= y2; y++)
    for (unsigned int x = x1; x <= x2; x++)
      SetPixel (x, y, &colour) ;
  InvalidatePixelBlock (x1, y1, x2, y2) ;
}

void WinLegacyDisplay::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
{
#ifdef RTR_SUPPORT
  if (m_Session->GetRealTimeRaytracing() == true)
    NextFrame();
#endif
  if (m_BitmapSurface == NULL)
    return ;
  if (x1 >= GetWidth() || x2 >= GetWidth())
    return ;
  if (y1 >= GetHeight() || y2 >= GetHeight())
    return ;
  ExternalDisplayPlotBlock (x1, y1, x2, y2, colour) ;
  if (gDisplay.get() != this)
    gDisplay = m_Session->GetDisplay();
  for (unsigned int y = y1 ; y <= y2; y++)
    for (unsigned int x = x1; x <= x2; x++)
      SetPixel (x, y, colour++) ;
  if (m_Session->GetRealTimeRaytracing() == true)
  {
    // TODO: move painting code into common routine shared with message handler
    RECT rect;
    int dest_width ;
    int dest_height ;
    int dest_xoffset ;
    int dest_yoffset ;
    HDC hdc = GetDC (m_Handle);
    if (hdc == NULL)
      return ;
    int oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
    if (hPalApp)
    {
      SelectPalette (hdc, hPalApp, false) ;
      RealizePalette (hdc) ;
    }
    GetClientRect (m_Handle, &rect) ;
    if (IsZoomed(m_Handle))
    {
      double aspect_ratio = (double) GetWidth() / GetHeight() ;
      double screen_aspect = (double) rect.right / rect.bottom ;
      if (aspect_ratio >= screen_aspect)
      {
        dest_width = rect.right ;
        dest_height = (int) ((double) rect.right / aspect_ratio) ;
      }
      else
      {
        dest_width = (int) ((double) rect.bottom * aspect_ratio) ;
        dest_height = rect.bottom ;
      }
      dest_xoffset = (rect.right - dest_width) / 2 ;
      dest_yoffset = (rect.bottom - dest_height) / 2 ;
      if (dest_width < rect.right)
      {
        BitBlt (hdc, 0, 0, dest_xoffset + 1, rect.bottom, NULL, 0, 0, BLACKNESS) ;
        BitBlt (hdc, dest_width + dest_xoffset, 0, dest_xoffset + 1, rect.bottom + 1, NULL, 0, 0, BLACKNESS) ;
      }
      if (dest_height < rect.bottom)
      {
        BitBlt (hdc, 0, 0, rect.right, dest_yoffset + 1, NULL, 0, 0, BLACKNESS) ;
        BitBlt (hdc, 0, dest_height + dest_yoffset, rect.right, dest_yoffset + 1, NULL, 0, 0, BLACKNESS) ;
      }
    }
    else
    {
      dest_xoffset = -renderwin_xoffset ;
      dest_yoffset = -renderwin_yoffset ;
      dest_width = GetWidth() ;
      dest_height = GetHeight() ;
      GetClientRect (m_Handle, &rect) ;
      if (rect.right > dest_width)
        BitBlt (hdc, dest_width, 0, rect.right - dest_width, rect.bottom, NULL, 0, 0, BLACKNESS) ;
      if (rect.bottom > dest_height)
        BitBlt (hdc, 0, dest_height, rect.right, rect.bottom - dest_height, NULL, 0, 0, BLACKNESS) ;
    }
    StretchDIBits (hdc, dest_xoffset, dest_yoffset, dest_width, dest_height, 0, 0, m_Bitmap.header.biWidth, m_Bitmap.header.biHeight, m_BitmapSurface, (LPBITMAPINFO) &m_Bitmap, DIB_RGB_COLORS, SRCCOPY);
    SetStretchBltMode (hdc, oldMode) ;
    ReleaseDC (m_Handle, hdc);
  }
  else
    InvalidatePixelBlock (x1, y1, x2, y2) ;
}

void WinLegacyDisplay::InvalidatePixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  if (m_Handle != NULL && !IsIconic (m_Handle))
  {
    RECT        rect ;

    if (IsZoomed (m_Handle))
    {
      int dest_width ;
      int dest_height ;

      GetClientRect  (m_Handle, &rect) ;
      double aspect_ratio = (double) GetWidth() / GetHeight() ;
      double screen_aspect = (double) rect.right / rect.bottom ;
      if (aspect_ratio >= screen_aspect)
      {
        dest_width = rect.right ;
        dest_height = (int) ((double) rect.right / aspect_ratio) ;
      }
      else
      {
        dest_width = (int) ((double) rect.bottom * aspect_ratio) ;
        dest_height = rect.bottom ;
      }
      int x = dest_width * x1 / GetWidth() ;
      int y = dest_height * y1 / GetHeight() ;
      int width = (dest_width * (x2 - x1 + 1) + GetWidth() - 1) / GetWidth() ;
      int height = (dest_height * (y2 - y1 + 1) + GetHeight() - 1) / GetHeight() ;
      int dest_xoffset = (rect.right - dest_width) / 2 ;
      int dest_yoffset = (rect.bottom - dest_height) / 2 ;
      rect.left = x + dest_xoffset ;
      rect.top = y + dest_yoffset ;
      rect.right = rect.left + width + 1 ;
      rect.bottom = rect.top + height + 1 ;
    }
    else
    {
      rect.left = x1 - renderwin_xoffset ;
      rect.top = y1 - renderwin_yoffset ;
      rect.right = x2 + 1 ;
      rect.bottom = y2 + 1 ;
    }
    InvalidateRect (m_Handle, &rect, false) ;
  }
}

void WinLegacyDisplay::Clear()
{
  if (m_BitmapSurface == NULL)
    return ;
  if (m_Depth8Bit)
  {
    for (int y = 0 ; y < m_Bitmap.header.biHeight ; y++)
    {
      for (int x = 0 ; x < m_Bitmap.header.biWidth ; x++)
      {
        int colour = (x & 8) == (y & 8) ? 0xff : 0xc0 ;
        unsigned char *p = m_BitmapSurface + (m_Bitmap.header.biHeight - 1 - y) * m_BytesPerLine + x ;
        unsigned char dither = dither8x8 [((x & 7) << 3) | (y & 7)] ;
        *p = 20 + div51 [colour] + (mod51 [colour] > dither) +
                  mul6 [div51 [colour] + (mod51 [colour] > dither)] +
                  mul36 [div51 [colour] + (mod51 [colour] > dither)] ;
      }
    }
  }
  else
  {
    for (int y = 0 ; y < m_Bitmap.header.biHeight ; y++)
    {
      unsigned char *p = m_BitmapSurface + (m_Bitmap.header.biHeight - 1 - y) * m_BytesPerLine ;
      for (int x = 0 ; x < m_Bitmap.header.biWidth ; x += 8)
      {
        if (x + 8 <= m_Bitmap.header.biWidth)
        {
          memset (p, (x & 8) == (y & 8) ? 0xff : 0xc0, 24) ;
          p += 24 ;
        }
        else
          memset (p, (x & 8) == (y & 8) ? 0xff : 0xc0, (m_Bitmap.header.biWidth - x) * 3) ;
      }
    }
  }
  if (m_Handle != NULL)
    InvalidateRect (m_Handle, NULL, false) ;
}

void WinLegacyDisplay::SetRenderState(bool IsRendering)
{
  if (m_Rendering != IsRendering)
  {
    m_Rendering = IsRendering;
    SetClassLongPtr (m_Handle, GCLP_HCURSOR, (LONG_PTR) LoadCursor (NULL, m_Rendering ? IDC_ARROW : IDC_CROSS)) ;
  }
}

LRESULT WinLegacyDisplay::WindowProc (UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   dest_width ;
  int                   dest_height ;
  int                   dest_xoffset ;
  int                   dest_yoffset ;
  int                   oldMode ;
  int                   width;
  int                   height;
  HDC                   hdc ;
  HDC                   hdcMemory ;
  RECT                  rect ;
  HPEN                  hpen ;
  char                  str [512] ;
  char                  *s ;
  bool                  zoomed = IsZoomed (m_Handle) != 0 ;
  POINT                 pt ;
  POINT                 pts [5] ;
  short                 x1 ;
  short                 y1 ;
  short                 x2 ;
  short                 y2 ;
  HBITMAP               oldBmp ;
  MINMAXINFO            *pInfo ;
  PAINTSTRUCT           ps ;

  // only initialize these variables if we need to
  switch (message)
  {
    case WM_LBUTTONDOWN :
    case WM_LBUTTONUP :
    case WM_MOUSEMOVE :
    case WM_PAINT :
         GetClientRect (m_Handle, &rect) ;
         if (zoomed)
         {
           double aspect_ratio = (double) GetWidth() / GetHeight() ;
           double screen_aspect = (double) rect.right / rect.bottom ;
           if (aspect_ratio >= screen_aspect)
           {
             dest_width = rect.right ;
             dest_height = (int) ((double) rect.right / aspect_ratio) ;
           }
           else
           {
             dest_width = (int) ((double) rect.bottom * aspect_ratio) ;
             dest_height = rect.bottom ;
           }
           dest_xoffset = (rect.right - dest_width) / 2 ;
           dest_yoffset = (rect.bottom - dest_height) / 2 ;
         }
         else
         {
           dest_xoffset = dest_yoffset = 0 ;
           dest_width = rect.right ;
           dest_height = rect.bottom ;
         }
  }

  switch (message)
  {
    case WM_SETFOCUS :
         // this will handle the annoying situation where another window is
         // in the z-order below the render window but above the main window.
         if (RenderwinIsChild)
           SetWindowPos (main_window, m_Handle, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE) ;
         break ;

    case WM_CHAR :
         // if any character is pressed, pass focus to the main window
         EditSetFocus () ;
         return (0) ;

    case WM_CLOSE :
         if (!running_demo && !demo_mode && !benchmark_mode)
           FeatureNotify ("RenderwinClose",
                          "POV-Ray - Render Window",
                          "If you find that the render window gets in your way during "
                          "or after rendering, there are a number of options to control it.\n\n"
                          "Press F1 to learn more.",
                          "Render Window Menu",
                          true) ;
         SendMessage (main_window, WM_COMMAND, CM_RENDERCLOSE, 0L) ;
         return (0) ;

    case WM_COMMAND :
         if (handle_main_command (wParam, lParam))
           return (0) ;
         break ;

    case WM_LBUTTONDOWN :
         if (m_Rendering)
         {
           FlashWindow (m_Handle, TRUE) ;
           MessageBeep (-1) ;
           FlashWindow (m_Handle, FALSE) ;
           return (0) ;
         }
         if (GetWidth() < 2 || GetHeight() < 2)
           return (0) ;
         m_RBand = 1 ;
         if ((wParam & MK_SHIFT) != 0)
           m_RBand++ ;
         if (zoomed)
         {
           m_RB1.x = MAX (MIN (LOWORD (lParam), dest_xoffset + dest_width - 1), dest_xoffset) ;
           m_RB1.y = MAX (MIN (HIWORD (lParam), dest_yoffset + dest_height - 1), dest_yoffset) ;
         }
         else
         {
           m_RB1.x = LOWORD (lParam) ;
           m_RB1.y = HIWORD (lParam) ;
         }
         m_RB2 = m_RB1 ;
         SetCapture (m_Handle) ;
         return (0) ;

    case WM_LBUTTONUP :
    case WM_MOUSEMOVE :
         x2 = LOWORD (lParam) ;
         y2 = HIWORD (lParam) ;
         if (message == WM_MOUSEMOVE)
           if (x2 == m_LastMouseX && y2 == m_LastMouseY)
             return (0) ;
         m_LastMouseX = x2 ;
         m_LastMouseY = y2 ;
         if (m_RBand)
         {
           hdc = GetDC (m_Handle) ;
           pts [0] = m_RB1 ;
           pts [1] = m_RB1 ;
           pts [1].x = m_RB2.x ;
           pts [2] = m_RB2 ;
           pts [3] = m_RB2 ;
           pts [3].x = m_RB1.x ;
           pts [4] = m_RB1 ;
           hpen = CreatePen (PS_DOT, 1, RGB (192, 192, 192)) ;
           hpen = (HPEN) SelectObject (hdc, hpen) ;
           oldMode = SetROP2 (hdc, R2_XORPEN) ;
           Polyline (hdc, pts, 5) ;
           if (zoomed)
           {
             m_RB2.x = x2 = MAX (MIN (x2, dest_xoffset + dest_width - 1), dest_xoffset) ;
             m_RB2.y = y2 = MAX (MIN (y2, dest_yoffset + dest_height - 1), dest_yoffset) ;

             x2 -= dest_xoffset ;
             y2 -= dest_yoffset ;
             x2 = MulDivNoRound (x2, GetWidth(), dest_width) ;
             y2 = MulDivNoRound (y2, GetHeight(), dest_height) ;

             // the values in m_RB1 have already been clipped
             x1 = m_RB1.x - dest_xoffset ;
             y1 = m_RB1.y - dest_yoffset ;
             x1 = MulDivNoRound (x1, GetWidth(), dest_width) ;
             y1 = MulDivNoRound (y1, GetHeight(), dest_height) ;
           }
           else
           {
             m_RB2.x = x2 = MAX (MIN (x2, dest_width - 1), 0) ;
             m_RB2.y = y2 = MAX (MIN (y2, dest_height - 1), 0) ;
             x2 += renderwin_xoffset ;
             y2 += renderwin_yoffset ;
             x1 = m_RB1.x + renderwin_xoffset ;
             y1 = m_RB1.y + renderwin_yoffset ;
           }
           if (x1 > x2)
             x1 ^= x2 ^= x1 ^= x2 ;
           if (y1 > y2)
             y1 ^= y2 ^= y1 ^= y2 ;
           if (message != WM_LBUTTONUP)
           {
             sprintf (str, "%d,%d - %d,%d", x1, y1, x2, y2) ;
             SetWindowText (m_Handle, str) ;
             pts [1].x = m_RB2.x ;
             pts [2] = m_RB2 ;
             pts [3] = m_RB2 ;
             pts [3].x = m_RB1.x ;
             Polyline (hdc, pts, 5) ;
             SetROP2 (hdc, oldMode) ;
             DeleteObject (SelectObject (hdc, hpen)) ;
             ReleaseDC (m_Handle, hdc) ;
           }
           else
           {
             ReleaseCapture () ;
             SetROP2 (hdc, oldMode) ;
             DeleteObject (SelectObject (hdc, hpen)) ;
             ReleaseDC (m_Handle, hdc) ;
             SetWindowText (m_Handle, "Render Window") ;
             if ((x2 - x1 > 2) && (y2 - y1 > 2))
             {
               sprintf (str, "Selection is %d,%d to %d,%d\n\n", x1, y1, x2, y2) ;
               if (m_RBand == 1)
               {
                 m_RBand = 0 ;
                 strcat (str, "Press OK to render this region now.\n") ;
                 strcat (str, "(You may shift-drag to set a permanent region next time).") ;
                 if (MessageBox (main_window, str, "Render region", MB_OKCANCEL) == IDOK)
                 {
                   if (EditSaveModified (NULL) == 0)
                     return (true) ;
                   if (RegionStr [0])
                   {
                     if ((s = strstr (command_line, RegionStr)) != NULL)
                       strcpy (s, s + strlen (RegionStr)) ;
                     else if ((s = strstr (command_line, RegionStr + 1)) != NULL)
                       strcpy (s, s + strlen (RegionStr) - 1) ;
                     SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
                     RegionStr [0] = '\0' ;
                   }
                   sprintf (RegionStr,
                            " +sc%f +sr%f +ec%f +er%f",
                            (float) x1 / (GetWidth() - 1),
                            (float) y1 / (GetHeight() - 1),
                            (float) x2 / (GetWidth() - 1),
                            (float) y2 / (GetHeight() - 1)) ;
                   temp_render_region = true ;
                   start_rendering (false) ;
                   temp_render_region = false ;
                 }
               }
               else
               {
                 m_RBand = 0 ;
                 strcat (str, "Press OK to append this region to the command-line.\n") ;
                 if (MessageBox (main_window, str, "Render region", MB_OKCANCEL) == IDOK)
                 {
                   if (RegionStr [0])
                   {
                     if ((s = strstr (command_line, RegionStr)) != NULL)
                       strcpy (s, s + strlen (RegionStr)) ;
                     else if ((s = strstr (command_line, RegionStr + 1)) != NULL)
                       strcpy (s, s + strlen (RegionStr) - 1) ;
                     SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
                   }
                   sprintf (TempRegionStr,
                            " +sc%f +sr%f +ec%f +er%f",
                            (float) x1 / (GetWidth() - 1),
                            (float) y1 / (GetHeight() - 1),
                            (float) x2 / (GetWidth() - 1),
                            (float) y2 / (GetHeight() - 1)) ;
                   PostMessage (main_window, WM_COMMAND, CM_COMMANDLINE, 0) ;
                 }
               }
             }
             else
               m_RBand = 0 ;
           }
         }
         else
         {
           if (m_Rendering == false)
           {
             width = GetWidth();
             height = GetHeight();
             if (width > 1 && height > 1)
             {
               x2 += renderwin_xoffset - dest_xoffset ;
               y2 += renderwin_yoffset - dest_yoffset ;
               if (zoomed)
               {
                 x2 = MulDivNoRound (x2, width, dest_width) ;
                 y2 = MulDivNoRound (y2, height, dest_height) ;
                 x2 = MAX (MIN (x2, width - 1), 0) ;
                 y2 = MAX (MIN (y2, height - 1), 0) ;
               }
               sprintf (str, "%d,%d [%.3f,%.3f]", x2, y2, (float) x2 / (width - 1), (float) y2 / (height - 1)) ;
               SetWindowText (m_Handle, str) ;
             }
           }
         }
         return (0) ;

    case WM_RBUTTONDOWN :
         if (hPopupMenus != NULL)
         {
           pt.x = LOWORD (lParam) ;
           pt.y = HIWORD (lParam) ;
           ClientToScreen (m_Handle, &pt) ;
           TrackPopupMenu (GetSubMenu (hPopupMenus, 1), TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, main_window, NULL) ;
         }
         return (0) ;

    case WM_GETMINMAXINFO :
         if (zoomed)
           break ;
         pInfo = (MINMAXINFO *) lParam ;
         pInfo->ptMaxTrackSize.x = m_MaxWidth ;
         pInfo->ptMaxTrackSize.y = m_MaxHeight ;
         break ;

    case WM_MOVE :
         if (m_Handle == NULL)
           break ;
         if (!IsIconic (m_Handle) && !zoomed)
         {
           GetWindowRect (m_Handle, &rect) ;
           renderwin_left = rect.left ;
           renderwin_top = rect.top ;
         }
         return (0) ;

    case WM_SIZE :
         if (m_Handle == NULL)
           break ;
         switch (wParam)
         {
           case SIZE_MINIMIZED :
                renderwin_flags = WS_MINIMIZE ;
                return (0) ;

           case SIZE_MAXIMIZED :
                renderwin_flags = WS_MAXIMIZE ;
                SetScrollRange (m_Handle, SB_HORZ, 0, 0, true) ;
                SetScrollRange (m_Handle, SB_VERT, 0, 0, true) ;
                PovInvalidateRect (m_Handle, NULL, false) ;
                UpdateWindow (m_Handle) ;
                SetClassLongPtr (m_Handle, GCLP_HCURSOR, (LONG_PTR) LoadCursor (NULL, m_Rendering ? IDC_ARROW : IDC_CROSS)) ;
                return (0) ;

           case SIZE_RESTORED :
                renderwin_flags = 0 ;
                PovInvalidateRect (m_Handle, NULL, false) ;
                UpdateWindow (m_Handle) ;
                break ;

           default :
                return (0) ;
         }

         // to get here we must be handling SIZE_RESTORED.

         GetWindowRect (m_Handle, &rect) ;
         width = rect.right - rect.left;
         height = rect.bottom - rect.top;

         // one problem we have here is that if we create one scroll bar, it takes away some of the client
         // area of the other direction (i.e. if we create a scroll bar for the X direction, it takes away
         // some of the Y client area). therefore we should create a scroll bar for that direction also.
         if (width < m_MaxWidth || height < m_MaxHeight)
         {
           if (renderwin_xoffset >= GetWidth() - LOWORD (lParam))
             renderwin_xoffset = GetWidth() - LOWORD (lParam) ;
           SetScrollRange (m_Handle, SB_HORZ, 0, GetWidth() - LOWORD (lParam), false) ;
           SetScrollPos (m_Handle, SB_HORZ, renderwin_xoffset, true) ;

           if (renderwin_yoffset >= GetHeight() - HIWORD (lParam))
             renderwin_yoffset = GetHeight() - HIWORD (lParam) ;
           SetScrollRange (m_Handle, SB_VERT, 0, GetHeight() - HIWORD (lParam), false) ;
           SetScrollPos (m_Handle, SB_VERT, renderwin_yoffset, true) ;
         }
         else
         {
           renderwin_xoffset = 0 ;
           renderwin_yoffset = 0 ;
           SetScrollRange (m_Handle, SB_VERT, 0, 0, true) ;
           SetScrollRange (m_Handle, SB_HORZ, 0, 0, true) ;
         }
         return (0) ;

    case WM_VSCROLL :
         GetClientRect (m_Handle, &rect) ;
         switch (LOWORD (wParam))
         {
           case SB_LINEDOWN :
                if (renderwin_yoffset >= GetHeight() - rect.bottom) break ;
                SetScrollRange (m_Handle, SB_VERT, 0, GetHeight() - rect.bottom, false) ;
                SetScrollPos (m_Handle, SB_VERT, ++renderwin_yoffset, true) ;
                ScrollWindow (m_Handle, 0, -1, NULL, NULL) ;
                break ;

           case SB_LINEUP :
                if (renderwin_yoffset == 0) break ;
                SetScrollRange (m_Handle, SB_VERT, 0, GetHeight() - rect.bottom, false) ;
                SetScrollPos (m_Handle, SB_VERT, --renderwin_yoffset, true) ;
                ScrollWindow (m_Handle, 0, 1, NULL, NULL) ;
                break ;

           case SB_PAGEDOWN :
                renderwin_yoffset += rect.bottom ;
                if (renderwin_yoffset > GetHeight() - rect.bottom)
                  renderwin_yoffset = GetHeight() - rect.bottom ;
                SetScrollPos (m_Handle, SB_VERT, renderwin_yoffset, true) ;
                PovInvalidateRect (m_Handle, NULL, false) ;
                break ;

           case SB_PAGEUP :
                renderwin_yoffset -= rect.bottom ;
                if (renderwin_yoffset < 0)
                  renderwin_yoffset = 0 ;
                SetScrollPos (m_Handle, SB_VERT, renderwin_yoffset, true) ;
                PovInvalidateRect (m_Handle, NULL, false) ;
                break ;

           case SB_THUMBPOSITION :
           case SB_THUMBTRACK :
                renderwin_yoffset = HIWORD (wParam) ;
                SetScrollPos (m_Handle, SB_VERT, renderwin_yoffset, true) ;
                PovInvalidateRect (m_Handle, NULL, false) ;
                break ;
         }
         return (0) ;

    case WM_HSCROLL :
         GetClientRect (m_Handle, &rect) ;
         switch (LOWORD (wParam))
         {
           case SB_LINERIGHT :
                if (renderwin_xoffset >= GetWidth() - rect.right) break ;
                SetScrollRange (m_Handle, SB_HORZ, 0, GetWidth() - rect.right, false) ;
                SetScrollPos (m_Handle, SB_HORZ, ++renderwin_xoffset, true) ;
                ScrollWindow (m_Handle, -1, 0, NULL, NULL) ;
                break ;

             case SB_LINELEFT :
                if (renderwin_xoffset == 0) break ;
                SetScrollRange (m_Handle, SB_HORZ, 0, GetWidth() - rect.right, false) ;
                SetScrollPos (m_Handle, SB_HORZ, --renderwin_xoffset, true) ;
                ScrollWindow (m_Handle, 1, 0, NULL, NULL) ;
                break ;

           case SB_PAGERIGHT :
                renderwin_xoffset += rect.right ;
                if (renderwin_xoffset > GetWidth() - rect.right)
                  renderwin_xoffset = GetWidth() - rect.right ;
                SetScrollPos (m_Handle, SB_HORZ, renderwin_xoffset, true) ;
                PovInvalidateRect (m_Handle, NULL, false) ;
                break ;

           case SB_PAGELEFT :
                renderwin_xoffset -= rect.right ;
                if (renderwin_xoffset < 0)
                  renderwin_xoffset = 0 ;
                SetScrollPos (m_Handle, SB_HORZ, renderwin_xoffset, true) ;
                PovInvalidateRect (m_Handle, NULL, false) ;
                break ;

           case SB_THUMBPOSITION :
           case SB_THUMBTRACK :
                renderwin_xoffset = HIWORD (wParam) ;
                SetScrollPos (m_Handle, SB_HORZ, renderwin_xoffset, true) ;
                PovInvalidateRect (m_Handle, NULL, false) ;
                break ;
         }
         return (0) ;

    case WM_PAINT :
         hdc = BeginPaint (m_Handle, &ps) ;
         if (IsIconic (m_Handle))
         {
           EndPaint (m_Handle, &ps) ;
           return (0) ;
         }
         oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
         if (hPalApp)
         {
           SelectPalette (hdc, hPalApp, false) ;
           RealizePalette (hdc) ;
         }
         if (zoomed)
         {
           GetClientRect (m_Handle, &rect) ;
           if (dest_width < rect.right)
           {
             BitBlt (hdc, 0, 0, dest_xoffset + 1, rect.bottom, NULL, 0, 0, BLACKNESS) ;
             BitBlt (hdc, dest_width + dest_xoffset, 0, dest_xoffset + 1, rect.bottom + 1, NULL, 0, 0, BLACKNESS) ;
           }
           if (dest_height < rect.bottom)
           {
             BitBlt (hdc, 0, 0, rect.right, dest_yoffset + 1, NULL, 0, 0, BLACKNESS) ;
             BitBlt (hdc, 0, dest_height + dest_yoffset, rect.right, dest_yoffset + 1, NULL, 0, 0, BLACKNESS) ;
           }
         }
         else
         {
           dest_xoffset = -renderwin_xoffset ;
           dest_yoffset = -renderwin_yoffset ;
           dest_width = GetWidth() ;
           dest_height = GetHeight() ;
           GetClientRect (m_Handle, &rect) ;
           if (rect.right > dest_width)
             BitBlt (hdc, dest_width, 0, rect.right - dest_width, rect.bottom, NULL, 0, 0, BLACKNESS) ;
           if (rect.bottom > dest_height)
             BitBlt (hdc, 0, dest_height, rect.right, rect.bottom - dest_height, NULL, 0, 0, BLACKNESS) ;
         }
         if (StretchDIBits (hdc,
                            dest_xoffset,
                            dest_yoffset,
                            dest_width,
                            dest_height,
                            0,
                            0,
                            m_Bitmap.header.biWidth,
                            m_Bitmap.header.biHeight,
                            m_BitmapSurface,
                            (LPBITMAPINFO) &m_Bitmap,
                            DIB_RGB_COLORS,
                            SRCCOPY) <= 0)
         {
           // hmmmm ... it seems we've run into a Windows bug of some form. When rendering a
           // large scene file (it used some 200mb of swap plus 80+mb of real memory on a 128mb
           // box) at a resolution of 1280x1024 (same as screen resolution) on Windows NT 4.0,
           // StretchDIBits () was observed to return zero (which is not failure, but not success
           // either :).

           GetClientRect (m_Handle, &rect) ;
           BitBlt (hdc, 0, 0, dest_width, dest_height, NULL, 0, 0, WHITENESS) ;
           if (m_ErrorBitmap == NULL)
             m_ErrorBitmap = LoadBitmap (hInstance, MAKEINTRESOURCE (BMP_STRETCHDIBITS)) ;
           hdcMemory = CreateCompatibleDC (hdc) ;
           oldBmp = (HBITMAP)SelectObject (hdcMemory, m_ErrorBitmap) ;
           BitBlt (hdc, rect.right / 2 - 157, rect.bottom / 2 - 10, 315, 21, hdcMemory, 0, 0, SRCCOPY) ;
           SelectObject (hdcMemory, oldBmp) ;
           DeleteDC (hdcMemory) ;
         }
         SetStretchBltMode (hdc, oldMode) ;
         EndPaint (m_Handle, &ps) ;
         return (0) ;

    case WM_DESTROY :
         m_Handle = NULL ;
         PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
         PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
         return (0) ;
  }
  return (DefWindowProc (m_Handle, message, wParam, lParam)) ;
}

LRESULT CALLBACK WinLegacyDisplay::StaticWindowProc (HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
#pragma warning(push)
#pragma warning(disable : 4312)
  WinLegacyDisplay *p = reinterpret_cast<WinLegacyDisplay *> (GetWindowLongPtr (handle, 0)) ;
#pragma warning(pop)

  if (p == NULL)
  {
    // this is possible during window creation (we haven't yet set the value)
    return (DefWindowProc (handle, message, wParam, lParam)) ;
  }

  if (p->m_Handle == NULL)
    p->m_Handle = handle ;

  return p->WindowProc (message, wParam, lParam);
}

HPALETTE WinLegacyDisplay::CreatePalette (RGBQUAD *rgb, int entries, bool use8bpp)
{
  int         i ;
  HDC         hdc ;
  LogPal      Palette = { 0x300, 256 } ;

  if (use8bpp)
  {
    if (rgb)
    {
      Palette.entries = entries ;
      for (i = 0 ; i < entries ; i++, rgb++)
      {
        Palette.pe [i].peRed = rgb->rgbRed ;
        Palette.pe [i].peGreen = rgb->rgbGreen ;
        Palette.pe [i].peBlue = rgb->rgbBlue ;
        Palette.pe [i].peFlags = PC_NOCOLLAPSE ;
      }
    }
    else
    {
      // Copy the halftone palette into the DIB palette entries, and read the
      // current system palette entries to ensure we have an identity palette mapping.
      hdc = GetDC (NULL) ;
      memcpy (m_BitmapTemplate.colors, halftonePal, sizeof (halftonePal)) ;
      GetSystemPaletteEntries (hdc, 0, 256, Palette.pe) ;
      for (i = 0 ; i < 10 ; i++)
      {
        m_BitmapTemplate.colors [i].rgbRed = Palette.pe [i].peRed ;
        m_BitmapTemplate.colors [i].rgbGreen = Palette.pe [i].peGreen ;
        m_BitmapTemplate.colors [i].rgbBlue = Palette.pe [i].peBlue ;
        Palette.pe [i].peFlags = 0 ;
        m_BitmapTemplate.colors [i + 246].rgbRed = Palette.pe [i + 246].peRed ;
        m_BitmapTemplate.colors [i + 246].rgbGreen = Palette.pe [i + 246].peGreen ;
        m_BitmapTemplate.colors [i + 246].rgbBlue = Palette.pe [i + 246].peBlue ;
        Palette.pe [i + 246].peFlags = 0 ;
      }
      while (i < 246)
      {
        Palette.pe [i].peRed = m_BitmapTemplate.colors [i].rgbRed ;
        Palette.pe [i].peGreen = m_BitmapTemplate.colors [i].rgbGreen ;
        Palette.pe [i].peBlue = m_BitmapTemplate.colors [i].rgbBlue ;
        Palette.pe [i++].peFlags = PC_NOCOLLAPSE ;
      }
      ReleaseDC (NULL, hdc) ;
    }
    return (::CreatePalette ((LOGPALETTE *) &Palette)) ;
  }
  return (NULL) ;
}

}
// end of namespace pov_frontend
