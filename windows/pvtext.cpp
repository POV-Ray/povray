//******************************************************************************
///
/// @file windows/pvtext.cpp
///
/// This module implements message and message display routines for Windows.
///
/// @author Christopher J. Cason
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
#define _WIN32_IE COMMONCTRL_VERSION

#include <windows.h>
#include <commctrl.h>
#include <setjmp.h>
#include <string.h>

#include "pvengine.h"
#include "resource.h"
#include "pvguiext.h"
#include "pvedit.h"

// this must be the last file included
#include "syspovdebug.h"

namespace povwin
{

#define MAX_SEEK_INDEX            1024

typedef struct
{
  unsigned              Line ;
  unsigned short        Offset ;
} seekStruct ;

typedef struct
{
  int         id ;
  int         resid ;
  int         flags ;
  char        *text ;
} toolbarStruct ;

int                     message_xchar ;
int                     message_ychar ;
int                     listbox_xchar ;
int                     listbox_ychar ;
int                     top_message_row ;
int                     message_scroll_pos_x ;
int                     message_scroll_pos_y ;
int                     message_count ;
int                     message_cols ;
int                     message_rows ;
int                     EditFileCount ;
int                     toolbar_cmdline_width ;
int                     toolbar_combobox_width ;
int                     bandWidths [8] ;
int                     message_output_x = -1 ;
char                    *message_buffer [MAX_MESSAGE] ;
char                    **first_message ;
char                    **next_message ;
char                    **last_message ;
char                    str_buffer [2048] ;
char                    message_font_name [256] ;
char                    listbox_font_name [] = "Lucida Console" ;
char                    line_buffer [2048] ;
char                    *EditFiles [MAX_EDIT_FILES] ;
bool                    mem_warned ;
unsigned                message_font_size ;
unsigned                message_font_weight ;
unsigned                listbox_font_size = 8 ;
unsigned                listbox_font_weight = FW_NORMAL ;
unsigned                editor_line_number ;
unsigned                editor_char_pos ;
unsigned                editor_offset ;
unsigned                seek_entry_count ;
RECT                    current_rect ;
bool                    keep_messages ;
HFONT                   message_font ;
HFONT                   listbox_font ;
HFONT                   tab_font ;
char                    sbToolText[512];
TOOLINFO                sbToolInfo;
seekStruct              seek_entries [MAX_SEEK_INDEX] ;
HIMAGELIST              ToolImages1 ;
HIMAGELIST              ToolImages2 ;

extern int              statistics_count ;
extern int              delay_next_status ;
extern int              seconds_for_last_line ;
extern char             tool_help [MAX_TOOLCMD] [MAX_TOOLHELPTEXT] ;
extern char             requested_render_file [] ;
extern char             source_file_name [] ;
extern char             NetworkAddress [] ;
extern char             PovStatusPanelWinClass [] ;
extern char             *EditDLLPath ;
extern bool             ListenMode ;
extern bool             ControlMode ;
extern void             *CurrentEditor ;
extern time_t           quit ;
extern unsigned         background_width ;
extern unsigned         background_height ;
extern unsigned         window_count ;
extern unsigned         statusheight ;
extern unsigned         screen_depth ;
extern unsigned         render_width ;
extern unsigned         ThreadCount ;
extern HWND             main_window ;
extern HWND             message_window ;
extern HWND             hToolComboBox ;
extern HWND             window_list [MAX_WINDOWS] ;
extern HWND             toolbar_combobox ;
extern HWND             aux_toolbar_window ;
extern HWND             statuspanel ;
extern HWND             StatusWindow ;
extern HWND             StatusTooltip ;
extern HWND             toolbar_cmdline ;
extern bool             IsWin32 ;
extern bool             fast_scroll ;
extern bool             tile_background ;
extern bool             IsW95UserInterface ;
extern bool             use_toolbar ;
extern bool             resizing ;
extern bool             exit_after_render ;
extern bool             demo_mode ;
extern bool             debugging ;
extern bool             render_requested ;
extern bool             noexec ;
extern bool             NoRestore ;
extern bool             rendering ;
extern bool             run_renderer ;
extern bool             rendering_animation ;
extern bool             no_status_output ;
extern bool             stop_rendering ;
extern bool             benchmark_mode;
extern HBITMAP          hBmpBackground ;
extern HBITMAP          hBmpRendering ;
extern COLORREF         background_colour ;
extern COLORREF         background_shade ;
extern COLORREF         text_colours[3] ;
extern HPALETTE         hPalApp ;
extern CRITICAL_SECTION critical_section ;

int                     toolbar_ids [] = {
                                          CM_FILENEW,
                                          CM_FILEOPEN,
                                          CM_FILESAVE,
                                          CM_FILEQUEUE,
                                          CM_RENDERSHOW,
                                          CM_COMMANDLINE,
                                          CM_SOURCEFILE,
                                          CM_FILERENDER,
                                          CM_RENDERSLEEP | 0x8000,
                                         } ;

#define MAX_TOOLS       (sizeof (toolbar_ids) / sizeof (int))

toolbarStruct           maintools [] =
                        {
                          {CM_FILENEW,       BMP_TOOLFILENEW,              0x00, "New"},
                          {CM_FILEOPEN,      BMP_TOOLFILEOPEN,             0x00, "Open"},
                          {CM_FILESAVE,      BMP_TOOLFILESAVE,             0x00, "Save"},
                          {CM_FILECLOSE,     BMP_TOOLFILECLOSE,            0x00, "Close"},
                          {CM_FILEQUEUE,     BMP_TOOLFILEQUEUE,            0x00, "Queue"},
                          {CM_RENDERSHOW,    BMP_TOOLRENDERSHOW,           0x00, "Show"},
                          {CM_RENDERCLOSE,   BMP_TOOLRENDERCLOSE,          0x02, "Hide"},
                          {CM_COMMANDLINE,   BMP_TOOLCMDLINE,              0x00, "Ini"},
                          {CM_SOURCEFILE,    BMP_TOOLSOURCEFILE,           0x00, "Sel-Run"},
                          {CM_FILERENDER,    BMP_TOOLFILERENDER,           0x00, "Run"},
                          {CM_STOPRENDER,    BMP_TOOLSTOPRENDER,           0x02, "Stop"},
                          {CM_RENDERSLEEP,   BMP_TOOLSLEEP,                0x01, "Pause"},
                          {CM_SHOWMAINWINDOW,BMP_TOOLSYSTRAY,              0x00, "Tray"},
                          {0,                0,                            0x00, NULL}
                        } ;

#define MAX_MAIN_TOOLS  (sizeof (maintools) / sizeof (toolbarStruct) - 1)

toolbarStruct           auxtools [] =
                        {
                          {CM_HELPPOVWIN,    BMP_TOOLHELPCONT,             0x00, "POV-Win"},
                          {CM_HELPSCENE,     BMP_TOOLHELPPOVRAY,           0x00, "Scene"},
                          {CM_GOPOVRAYORG,   BMP_TOOLGOPOVRAY,             0x00, "POV Site"},
                          {0,                0,                            0x00, NULL}
                        } ;

#define MAX_AUX_TOOLS   (sizeof (auxtools) / sizeof (toolbarStruct) - 1)

bool is_horz_line (char *s)
{
  if (strlen (s) < 70)
    return (false) ;

  while (*s)
    if (*s++ != '-')
      return (false) ;

  return (true) ;
}

const char *buffer_str (const char *s, lftype *lf)
{
  char                  *ptr ;

  if (s == NULL)
  {
    message_output_x = -1 ;
    return (NULL) ;
  }
  *lf = None ;
  if (message_output_x == -1)
  {
    message_output_x = 0 ;
    memset (str_buffer, 0, sizeof (str_buffer)) ;
  }
  ptr = str_buffer + message_output_x ;
  while (*s)
  {
    switch (*s)
    {
      case '\r' : message_output_x = 0 ;
                  ptr = str_buffer ;
                  *lf = CR ;
                  break ;
      case '\n' : message_output_x = -1 ;
                  *lf = LF ;
                  return (++s) ;
      case '\f' : if (message_output_x != -1)
                  {
                    message_output_x = -1 ;
                    *lf = LF ;
                    return (++s) ;
                  }
                  s++ ;
                  break ;
      case '\b' : if (message_output_x > 0)
                  {
                    --message_output_x ;
                    *--ptr = '\0' ;
                  }
                  break ;
      default   : if (isprint (*s) && message_output_x < sizeof (str_buffer) - 1)
                  {
                    message_output_x++ ;
                    *ptr++ = *s ;
                  }
                  break ;
    }
    s++ ;
  }
  return (s) ;
}

int update_message_display (lftype lf)
{
  RECT        rect ;

  if (message_window == NULL)
    return (0) ;
  GetClientRect (message_window, &rect) ;
  message_cols = rect.right / message_xchar ;
  message_rows = rect.bottom / message_ychar ;
  if (lf == None || lf == LF)
  {
    EnterCriticalSection (&critical_section) ;
    top_message_row = message_count > message_rows ? message_count - message_rows : 0 ;
    LeaveCriticalSection (&critical_section) ;
    SetScrollRange (message_window, SB_HORZ, 0, need_hscroll (), false) ;
    SetScrollPos (message_window, SB_HORZ, message_scroll_pos_x, true) ;
    SetScrollRange (message_window, SB_VERT, 0, top_message_row, false) ;
    SetScrollPos (message_window, SB_VERT, message_scroll_pos_y, true) ;
  }
  if (lf == None)
    return (message_rows) ;
  if (lf == LF)
  {
    EnterCriticalSection (&critical_section) ;
    // is there room for another row ?
    if (current_rect.bottom + message_ychar <= rect.bottom)
    {
      // yes there is
      current_rect.top += message_ychar ;
      current_rect.bottom += message_ychar ;
    }
    else
      ScrollWindow (message_window, 0, -message_ychar, NULL, &rect) ;
    message_scroll_pos_y = top_message_row ;
    SetScrollPos (message_window, SB_VERT, message_scroll_pos_y, true) ;
    LeaveCriticalSection (&critical_section) ;
  }
  PovInvalidateRect (message_window, &current_rect, true) ;
  if (!fast_scroll && !rendering_animation && IsWindowVisible (message_window))
    UpdateWindow (message_window) ;
  return (message_rows) ;
}

void buffer_message (msgtype message_type, const char *s, bool addLF)
{
  char                  *s1 ;
  lftype                lf ;

  ExternalBufferMessage (message_type, (LPSTR) s) ;

  EnterCriticalSection (&critical_section) ;

  while (*s)
  {
    s1 = (char *)s ;
    s = buffer_str (s, &lf) ;
    if (lf == None)
      continue ;

    if ((s1 = (char *) malloc (strlen (str_buffer) + 2)) == NULL)
    {
      LeaveCriticalSection (&critical_section) ;
      if (rendering)
      {
        if (!mem_warned)
          PovMessageBox ("Failed to allocate memory for message string", "Fatal Error") ;
        stop_rendering = true ;
      }
      else
        if (!mem_warned)
          PovMessageBox ("Failed to allocate memory for message string", "Error") ;
      mem_warned = true ;
      return ;
    }

    if (is_horz_line (str_buffer))
      message_type = mHorzLine ;

    strcpy (s1 + 1, str_buffer) ;
    *s1 = (unsigned char) message_type ;

    if (lf == CR)
    {
      if (*last_message)
        free (*last_message) ;
      *last_message = s1 ;
      LeaveCriticalSection (&critical_section) ;
      update_message_display (CR) ;
      EnterCriticalSection (&critical_section) ;
    }
    else
    {
      if (*next_message)
      {
        free (*next_message) ;
        *next_message = NULL ;
        if (++first_message == message_buffer + MAX_MESSAGE)
          first_message = message_buffer ;
      }
      else
        message_count++ ;
      *next_message = s1 ;
      last_message = next_message ;
      if (++next_message == message_buffer + MAX_MESSAGE)
        next_message = message_buffer ;
      LeaveCriticalSection (&critical_section) ;
      update_message_display (LF) ;
      EnterCriticalSection (&critical_section) ;
    }
  }

  if (addLF)
    buffer_message (message_type, "\n", false) ;
  LeaveCriticalSection (&critical_section) ;
}

void add_single_line (msgtype message_type, const char *str)
{
  char                  *s1 ;
  RECT                  rect ;

  if (message_type == mWarning && strncmp (str, "File: ", 6) == 0)
    add_single_line (mDivider, "") ;
  if (message_type == mStatistics)
    if (strcmp (str, "Scene Statistics") == 0 || strcmp (str, "Render Statistics") == 0)
      add_single_line (mDivider, "") ;

  if (strncmp (str, "---------------", 15) == 0)
  {
    str = "" ;
    message_type = mDivider ;
  }

  if (message_type == mDivider || message_type == mHorzLine)
    if (last_message != NULL && (**last_message == mDivider || **last_message == mHorzLine))
      return ;

  if ((s1 = (char *) malloc (strlen (str) + 2)) == NULL)
  {
    if (rendering)
    {
      if (!mem_warned)
        PovMessageBox ("Failed to allocate memory for message string", "Fatal Error") ;
      stop_rendering = true ;
    }
    else
      if (!mem_warned)
        PovMessageBox ("Failed to allocate memory for message string", "Error") ;
    mem_warned = true ;
    return ;
  }

  strcpy (s1 + 1, str) ;
  *s1 = (unsigned char) message_type ;

  if (*next_message)
  {
    free (*next_message) ;
    *next_message = NULL ;
    if (++first_message == message_buffer + MAX_MESSAGE)
      first_message = message_buffer ;
  }
  else
    message_count++ ;
  *next_message = s1 ;
  last_message = next_message ;
  if (++next_message == message_buffer + MAX_MESSAGE)
    next_message = message_buffer ;

  GetClientRect (message_window, &rect) ;
  message_cols = rect.right / message_xchar ;
  message_rows = rect.bottom / message_ychar ;
  top_message_row = message_count > message_rows ? message_count - message_rows : 0 ;
  SetScrollRange (message_window, SB_HORZ, 0, need_hscroll (), false) ;
  SetScrollPos (message_window, SB_HORZ, message_scroll_pos_x, true) ;
  SetScrollRange (message_window, SB_VERT, 0, top_message_row, false) ;
  SetScrollPos (message_window, SB_VERT, message_scroll_pos_y, true) ;
  // is there room for another row ?
  if (current_rect.bottom + message_ychar <= rect.bottom)
  {
    // yes there is
    current_rect.top += message_ychar ;
    current_rect.bottom += message_ychar ;
  }
  else
    ScrollWindow (message_window, 0, -message_ychar, NULL, &rect) ;
  message_scroll_pos_y = top_message_row ;
  SetScrollPos (message_window, SB_VERT, message_scroll_pos_y, true) ;
  PovInvalidateRect (message_window, &current_rect, true) ;
  if (!fast_scroll && !rendering_animation && IsWindowVisible (message_window))
    UpdateWindow (message_window) ;
}

void wrap_message (const char *str, int width, msgtype message_type)
{
  int         col = 1 ;
  char        buffer [2048] ;
  char        *bp = buffer ;
  char        *ws_b = NULL ;
  const char  *wd_b = NULL ;
  const char  *s = str ;

  if (width < 80)
    width = 80 ;
  while (*s)
  {
    if (*s == '\r' || *s == '\n')
    {
      while (*s == '\r' && (s [1] == '\r' || s [1] == '\n'))
        s++ ;
      *bp = '\0' ;
      add_single_line (message_type, buffer) ;
      ws_b = NULL ;
      wd_b = NULL ;
      bp = buffer ;
      col = 1 ;
      s++ ;
      continue ;
    }

    col++ ;
    if (*s == ' ' || *s == '\t')
    {
      if (ws_b == NULL || ws_b < wd_b)
        ws_b = bp ;
      wd_b = NULL ;
      *bp++ = *s++ ;
      continue ;
    }

    if (wd_b == NULL)
      wd_b = bp ;
    *bp++ = *s++ ;

    if (col > width && ws_b != NULL)
    {
      *ws_b = '\0' ;
      add_single_line (message_type, buffer) ;
      if (wd_b)
      {
        col = bp - wd_b ;
        memcpy (buffer, wd_b, col) ;
        bp = buffer + col ;
        wd_b = buffer ;
        ws_b = NULL ;
      }
      else
      {
        col = 1 ;
        bp = buffer ;
        ws_b = NULL ;
        wd_b = NULL ;
        while (*s == ' ' || *s == '\t')
          s++ ;
      }
    }
  }
  if (ws_b != NULL && wd_b == NULL)
    bp = ws_b ;
  *bp = '\0' ;
  add_single_line (message_type, buffer) ;
}

void buffer_stream_message (msgtype message_type, const char *s)
{
  RECT        rect ;

  if (message_type == mUnknown)
  {
      if (strncmp (s, "File: ", 6) == 0)
        message_type = mWarning;
      else if (strncmp (s, "Shutdown", 8) == 0)
        message_type = mWarning;
  }

  // since the POVMS streams don't try fancy stuff with CR's and backspaces
  // we can simplify the above code (and also handle wordwrap). all data
  // data passed to us is implicitly terminated by an LF. embedded LF's are
  // permitted and a terminating LF implies two LF's (due to above).
  ExternalBufferMessage (message_type, (LPSTR) s) ;
  ExternalBufferMessage (message_type, "\n") ;

  GetClientRect (message_window, &rect) ;
  message_cols = rect.right / message_xchar ;

  EnterCriticalSection (&critical_section) ;

  // we don't want to wrap debug text - we assume the scene author wants to control this.
  wrap_message (s, message_type == mDebug ? 9999 : message_cols - 2, message_type) ;
  LeaveCriticalSection (&critical_section) ;
}

void message_printf (const char *format, ...)
{
  char                  str [2048] ;
  char                  *s ;
  time_t                t ;
  va_list               arg_ptr ;

  if (strlen (format) > sizeof (str) - 256)
    return ;
  if (debugging)
  {
    time (&t) ;
    s = ctime (&t) ;
    memcpy (str, s + 11, 9) ;
    va_start (arg_ptr, format) ;
    vsprintf (str + 9, format, arg_ptr) ;
    va_end (arg_ptr) ;
    OutputDebugString (str) ;
  }
  va_start (arg_ptr, format) ;
  vsprintf (str, format, arg_ptr) ;
  va_end (arg_ptr) ;
  buffer_message (mIDE, str) ;
}

void wrapped_printf (const char *format, ...)
{
  char                  str [2048] ;
  char                  *s ;
  time_t                t ;
  va_list               arg_ptr ;

  if (strlen (format) > sizeof (str) - 256)
    return ;
  if (debugging)
  {
    time (&t) ;
    s = ctime (&t) ;
    memcpy (str, s + 11, 9) ;
    va_start (arg_ptr, format) ;
    vsprintf (str + 9, format, arg_ptr) ;
    va_end (arg_ptr) ;
    OutputDebugString (str) ;
  }
  va_start (arg_ptr, format) ;
  vsprintf (str, format, arg_ptr) ;
  va_end (arg_ptr) ;
  s = str + strlen (str) - 1 ;
  if (*s == '\n')
    *s = '\0' ;
  buffer_stream_message (mIDE, str) ;
}

void status_printf (int nSection, const char *format, ...)
{
  char                  str [2048] ;
  va_list               arg_ptr ;

  va_start (arg_ptr, format) ;
  vsprintf (str, format, arg_ptr) ;
  va_end (arg_ptr) ;
  say_status_message (nSection, str) ;
}

char *clean_str (const char *s)
{
  char        *s1 ;
  static char str [512] ;

  if (strlen (s) > 511)
    return ("Internal error : string too long in clean_str") ;
  EnterCriticalSection (&critical_section) ;
  for (s1 = str ; *s ; s++)
    if (*s >= ' ')
      *s1++ = *s ;
  *s1 = '\0' ;
  LeaveCriticalSection (&critical_section) ;
  return (str) ;
}

void erase_display_window (HDC hdc, int xoffset, int yoffset)
{
  int         x, y ;
  HDC         hdcMemory ;
  RECT        rect ;
  HBRUSH      hbr ;
  HBITMAP     oldBmp ;

  if (message_window == NULL)
    return ;
  GetClientRect (message_window, &rect) ;
  if (tile_background)
  {
    hdcMemory = CreateCompatibleDC (hdc) ;
    oldBmp = (HBITMAP) SelectObject (hdcMemory, hBmpBackground) ;
    xoffset %= background_width ;
    yoffset %= background_height ;
    for (y = -yoffset ; y < rect.bottom ; y += background_height)
      for (x = -xoffset ; x < rect.right ; x += background_width)
        BitBlt (hdc, x, y, background_width, background_height, hdcMemory, 0, 0, SRCCOPY) ;
    SelectObject (hdcMemory, oldBmp) ;
    DeleteDC (hdcMemory) ;
  }
  else
  {
    hbr = CreateSolidBrush (background_colour) ;
    FillRect (hdc, &rect, hbr) ;
    DeleteObject (hbr) ;
  }
}

void paint_display_window (HDC hdc)
{
  int           x, y ;
  int           message_number ;
  int           oldMode ;
  int           dividerStep ;
  int           xoffset ;
  int           yoffset ;
  char          **message = first_message ;
  bool          erased = false ;
  RECT          rect ;
  HPEN          hpen1 ;
  HPEN          hpen2 ;
  HPEN          hpenOld ;
  HFONT         oldFont ;
  COLORREF      oldColour ;
  COLORREF      oldBkColour ;
  unsigned char lastType = 255;
  static RECT   oldRect ;

  EnterCriticalSection (&critical_section) ;
  GetClientRect (message_window, &rect) ;
  current_rect.left = 0 ;
  current_rect.right = rect.right ;
  current_rect.top = -message_ychar ;
  current_rect.bottom = 0 ;
  xoffset = (unsigned) message_scroll_pos_x * message_xchar ;
  yoffset = (unsigned) (first_message - message_buffer) * message_ychar ;

  if (*message == NULL || resizing || (EditGetFlags () & EDIT_DRAG_ACTIVE) != 0)
  {
    erase_display_window (hdc, xoffset, yoffset) ;
    LeaveCriticalSection (&critical_section) ;
    oldRect = rect ;
    return ;
  }

  oldRect = rect ;

  if (tile_background)
  {
    hpen1 = CreatePen (PS_SOLID, 1, RGB (192,192,192)) ;
    hpen2 = CreatePen (PS_SOLID, 1, RGB (64,64,64)) ;
  }
  else
  {
    int red = background_colour & 0xff;
    int green = (background_colour >> 8) & 0xff;
    int blue = (background_colour >> 16) & 0xff;

    hpen1 = CreatePen (PS_SOLID, 1, text_colours[0]) ;
    hpen2 = CreatePen (PS_SOLID, 1, RGB(red / 2, green / 2, blue / 2)) ;
  }

  hpenOld = (HPEN) SelectObject (hdc, hpen2) ;
  oldFont = (HFONT) SelectObject (hdc, message_font) ;
  oldMode = SetBkMode (hdc, TRANSPARENT) ;
  oldColour = SetTextColor (hdc, text_colours[0]) ;
  oldBkColour = SetBkColor (hdc, background_shade) ;
  x = message_scroll_pos_x * -message_xchar + message_xchar ;
  for (message_number = y = 0 ; y < rect.bottom ; message_number++)
  {
    if (*message == NULL)
      break ;
    if (message_number >= message_scroll_pos_y)
    {
      if (!erased)
      {
        erase_display_window (hdc, xoffset, yoffset) ;
        erased = true ;
      }
      if (**message != lastType)
      {
        lastType = **message;
        switch (**message)
        {
          case mDebug:
          case mWarning:
            SetTextColor (hdc, text_colours[1]);
            break;

          case mFatal:
            SetTextColor (hdc, text_colours[2]);
            break;

          default:
            SetTextColor (hdc, text_colours[0]);
            break;
        }
      }
      current_rect.top += message_ychar ;
      current_rect.bottom += message_ychar ;
      if (**message == mDivider || **message == mHorzLine)
      {
        if (background_shade != RGB (1, 1, 1) && tile_background)
          DRAWFASTRECT (hdc, &current_rect) ;
        SelectObject (hdc, hpen2) ;
        if (tile_background)
        {
          dividerStep = message_ychar / 3;
          MoveToEx (hdc, current_rect.left + message_xchar, y + message_ychar - dividerStep, NULL) ;
          LineTo (hdc, current_rect.left + message_xchar, y + dividerStep) ;
          LineTo (hdc, current_rect.right - message_xchar, y + dividerStep) ;
          SelectObject (hdc, hpen1) ;
          LineTo (hdc, current_rect.right - message_xchar, y + message_ychar - dividerStep) ;
          LineTo (hdc, current_rect.left + message_xchar, y + message_ychar - dividerStep) ;
        }
        else
        {
          MoveToEx (hdc, current_rect.left + message_xchar, y + message_ychar / 2, NULL) ;
          LineTo (hdc, current_rect.right - message_xchar, y + message_ychar / 2) ;
          SelectObject (hdc, hpen1) ;
          MoveToEx (hdc, current_rect.left + message_xchar, y + message_ychar / 2 + 1, NULL) ;
          LineTo (hdc, current_rect.right - message_xchar, y + message_ychar / 2 + 1) ;
        }
      }
      else
        ExtTextOut (hdc, x, y, ETO_CLIPPED, &current_rect, *message + 1, (int) strlen (*message + 1), NULL) ;
      y += message_ychar ;
    }
    if (message == last_message)
      break ;
    if (++message == message_buffer + MAX_MESSAGE)
      message = message_buffer ;
    yoffset += message_ychar ;
  }
  if (!erased)
    erase_display_window (hdc, xoffset, yoffset) ;
  SetTextColor (hdc, oldColour) ;
  SetBkColor (hdc, oldBkColour) ;
  SetBkMode (hdc, oldMode) ;
  SelectObject (hdc, oldFont) ;
  SelectObject (hdc, hpenOld) ;
  DeleteObject (hpen1) ;
  DeleteObject (hpen2) ;
  LeaveCriticalSection (&critical_section) ;
}

void get_logfont (HDC hdc, LOGFONT *lf)
{
  memset (lf, 0, sizeof (LOGFONT)) ;
  lf->lfHeight = -MulDiv (message_font_size, GetDeviceCaps (hdc, LOGPIXELSY), 72) ;
  lf->lfWeight = message_font_weight ;
  lf->lfPitchAndFamily = FIXED_PITCH | FF_MODERN ;
  lf->lfCharSet = DEFAULT_CHARSET ;
  lf->lfQuality = PROOF_QUALITY ;
  strncpy (lf->lfFaceName, message_font_name, sizeof (lf->lfFaceName) - 1) ;
}

void get_lblogfont (HDC hdc, LOGFONT *lf)
{
  memset (lf, 0, sizeof (LOGFONT)) ;
  lf->lfHeight = -MulDiv (listbox_font_size, GetDeviceCaps (hdc, LOGPIXELSY), 72) ;
  lf->lfWeight = listbox_font_weight ;
  lf->lfPitchAndFamily = FIXED_PITCH | FF_MODERN ;
  lf->lfCharSet = DEFAULT_CHARSET ;
  lf->lfQuality = PROOF_QUALITY ;
  strncpy (lf->lfFaceName, listbox_font_name, sizeof (lf->lfFaceName) - 1) ;
}

int create_message_font (HDC hdc, LOGFONT *lf)
{
  HFONT       hfontOld ;
  TEXTMETRIC  tm ;

  if ((message_font = CreateFontIndirect (lf)) == NULL)
    return (1) ;
  hfontOld = (HFONT) SelectObject (hdc, message_font) ;
  GetTextMetrics (hdc, &tm) ;
  message_xchar = tm.tmAveCharWidth ;
  message_ychar = tm.tmHeight + tm.tmExternalLeading ;
  SelectObject (hdc, hfontOld) ;
  return (0) ;
}

int create_listbox_font (HDC hdc, LOGFONT *lf)
{
  HFONT       hfontOld ;
  TEXTMETRIC  tm ;

  if ((listbox_font = CreateFontIndirect (lf)) == NULL)
    return (1) ;
  hfontOld = (HFONT)SelectObject (hdc, listbox_font) ;
  GetTextMetrics (hdc, &tm) ;
  listbox_xchar = tm.tmAveCharWidth ;
  listbox_ychar = tm.tmHeight + tm.tmExternalLeading ;
  SelectObject (hdc, hfontOld) ;
  return (0) ;
}

int initialise_message_display (void)
{
  HDC         hdc ;
  LOGFONT     lf ;

  if (message_window == NULL)
  {
    PovMessageBox ("Message Window does not exist", "Initialize Message Display - Fatal Error") ;
    return (1) ;
  }
  hdc = GetDC (message_window) ;
  get_logfont (hdc, &lf) ;
  if (create_message_font (hdc, &lf) != 0)
  {
    PovMessageBox ("Failed to create message font", "Initialize Message Display - Fatal Error") ;
    ReleaseDC (message_window, hdc) ;
    return (1) ;
  }
  get_lblogfont (hdc, &lf) ;
  if (create_listbox_font (hdc, &lf) != 0)
  {
    strcpy (lf.lfFaceName, "Courier New") ;
    if (create_listbox_font (hdc, &lf) != 0)
      PovMessageBox ("Failed to create listbox font", "Initialize Message Display") ;
  }
  first_message = next_message = message_buffer ;
  last_message = NULL ;
  buffer_str (NULL, NULL) ;
  top_message_row = message_count = message_scroll_pos_x = message_scroll_pos_y = 0 ;
  current_rect.left = current_rect.bottom = current_rect.right = 0 ;
  current_rect.top = -message_ychar ;
  ReleaseDC (message_window, hdc) ;
  return (0) ;
}

void clear_messages (bool print)
{
  int         i ;
  char        **p ;

  EnterCriticalSection (&critical_section) ;
  buffer_str (NULL, NULL) ;
  // free any buffered lines still around from a previous run of the renderer
  for (p = message_buffer, i = 0 ; i < MAX_MESSAGE ; p++, i++)
  {
    if (*p)
      free (*p) ;
    *p = NULL ;
  }
  first_message = next_message = message_buffer ;
  last_message = NULL ;
  top_message_row = message_count = message_scroll_pos_x = message_scroll_pos_y = 0 ;
  current_rect.left = current_rect.bottom = current_rect.right = 0 ;
  current_rect.top = -message_ychar ;
  LeaveCriticalSection (&critical_section) ;
  if (print)
    message_printf ("Messages cleared.\n") ;
}

int need_hscroll (void)
{
  int         x ;
  int         xchars ;
  int         width = 0 ;
  char        **message = first_message ;
  RECT        rect ;

  /* modified to return the scroll range if ANY line is long enough */
  if (message_window == NULL || *message == NULL)
    return (0) ;
  GetClientRect (message_window, &rect) ;
  xchars = rect.right / message_xchar - 1 ;
  while (*message)
  {
    x = (int) strlen (*message + 1) ;
    if (x >= xchars)
      if (x - xchars > width)
        width = x - xchars ;
    if (message == next_message)
      break ;
    if (++message == message_buffer + MAX_MESSAGE)
      message = message_buffer ;
  }
  return (width) ;
}

bool copy_text_to_clipboard(const char *text)
{
  char        *s ;
  HGLOBAL     hText ;

  if (!OpenClipboard(NULL))
    return false;
  if ((hText = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, strlen(text) + 1)) == NULL)
    return false;
  if ((s = (char *) GlobalLock (hText)) == NULL)
  {
    GlobalFree(hText);
    return false;
  }
  strcpy(s, text);
  GlobalUnlock(hText);
  EmptyClipboard() ;
  bool result = SetClipboardData(CF_TEXT, hText) != NULL;
  CloseClipboard() ;
  return result;
}

void dump_pane_to_clipboard (void)
{
  int         y ;
  int         message_number ;
  int         length = 0 ;
  char        **message = first_message ;
  char        *s ;
  RECT        rect ;
  HGLOBAL     hText ;
  static char *_s ;

  if (message_window == NULL || *message == NULL)
    return ;
  if (OpenClipboard (message_window) == false)
  {
    PovMessageBox ("Could not open clipboard", "Error") ;
    return ;
  }
  if ((hText = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, 33000)) == NULL)
    return ;
  if ((s = (char *) GlobalLock (hText)) == NULL)
  {
    GlobalFree(hText);
    CloseClipboard () ;
    return ;
  }
  _s = s ;
  GetClientRect (message_window, &rect) ;
  for (message_number = y = 0 ; y < rect.bottom ; message_number++)
  {
    if (*message == NULL)
      break ;
    if (message_number >= message_scroll_pos_y)
    {
      length += (int) strlen (*message + 1) + 2 ;
      if (length >= 32767)
        break ;
      s += sprintf (s, "%s\r\n", *message + 1) ;
      y += message_ychar ;
    }
    if (++message == message_buffer + MAX_MESSAGE)
      message = message_buffer ;
    if (message == next_message)
      break ;
  }
  GlobalUnlock (hText) ;
  GlobalReAlloc (hText, length + 1, GMEM_MOVEABLE | GMEM_DDESHARE) ;
  EmptyClipboard () ;
  SetClipboardData (CF_TEXT, hText) ;
  CloseClipboard () ;
}

void draw_ordinary_listbox (DRAWITEMSTRUCT *d, bool fitpath)
{
  int         oldMode ;
  int         dividerStep ;
  int         width ;
  int         length ;
  char        str [MAX_PATH] ; // TODO FIXME - this isn't safe
  RECT        rect ;
  HPEN        hpen1 ;
  HPEN        hpen2 ;
  HPEN        hpenOld ;
  HFONT       oldFont ;
  COLORREF    oldBackground ;
  COLORREF    oldForeground ;

  if (d->itemID == -1)
    return ;
  hpen1 = CreatePen (PS_SOLID, 1, GetSysColor (COLOR_BTNHIGHLIGHT)) ;
  hpen2 = CreatePen (PS_SOLID, 1, GetSysColor (COLOR_BTNSHADOW)) ;
  hpenOld = (HPEN) SelectObject (d->hDC, hpen2) ;
  oldFont = (HFONT) SelectObject (d->hDC, listbox_font) ;
  oldMode = SetBkMode (d->hDC, TRANSPARENT) ;
  oldForeground = SetTextColor (d->hDC, GetSysColor (d->itemState & ODS_SELECTED ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT)) ;
  oldBackground = SetBkColor (d->hDC, GetSysColor (d->itemState & ODS_SELECTED ? COLOR_HIGHLIGHT : COLOR_WINDOW)) ;
  dividerStep = listbox_ychar / 2 - 1 ;

  rect = d->rcItem ;
  rect.left += listbox_xchar ;
  rect.right -= listbox_xchar ;
  width = (rect.right - rect.left) / listbox_xchar ;
  SendMessage (d->hwndItem, LB_GETTEXT, d->itemID, (LPARAM) str) ;
  if (strncmp (str, "----------------", 16) == 0)
  {
    MoveToEx (d->hDC, rect.left, rect.top + listbox_ychar - dividerStep, NULL) ;
    SelectObject (d->hDC, hpen2) ;
    LineTo (d->hDC, rect.left, rect.top + dividerStep) ;
    LineTo (d->hDC, rect.right - listbox_xchar, rect.top + dividerStep) ;
    SelectObject (d->hDC, hpen1) ;
    LineTo (d->hDC, rect.right - listbox_xchar, rect.top + listbox_ychar - dividerStep) ;
    LineTo (d->hDC, rect.left, rect.top + listbox_ychar - dividerStep) ;
  }
  else
  {
    if (fitpath)
    {
      length = (int) strlen (str) ;
      if (length > width)
      {
        if (str [1] == ':' && isPathSeparator(str [2]))
        {
          memcpy (str + 3, "...", 3) ;
          strcpy (str + 6, str + length - width + 6) ;
        }
      }
    }
    ExtTextOut (d->hDC, rect.left, rect.top, ETO_CLIPPED | ETO_OPAQUE, &rect, str, (int) strlen (str), NULL) ;
  }

  SetBkColor (d->hDC, oldBackground) ;
  SetTextColor (d->hDC, oldForeground) ;
  SetBkMode (d->hDC, oldMode) ;
  SelectObject (d->hDC, oldFont) ;
  SelectObject (d->hDC, hpenOld) ;
  DeleteObject (hpen1) ;
  DeleteObject (hpen2) ;
}

void write_wrapped_text (HDC hdc, RECT *rect, const char *text)
{
  int         oldMode ;
  HFONT       hFont ;
  HFONT       hFontOld ;
  LOGFONT     lf ;
  COLORREF    oldForeground ;

  memset (&lf, 0, sizeof (LOGFONT)) ;
  lf.lfHeight = -MulDiv (8, GetDeviceCaps (hdc, LOGPIXELSY), 72) ;
  lf.lfWeight = FW_NORMAL ;
  lf.lfPitchAndFamily = VARIABLE_PITCH ;
  lf.lfCharSet = DEFAULT_CHARSET ;
  strcpy (lf.lfFaceName, "MS Sans Serif") ;
  if ((hFont = CreateFontIndirect (&lf)) != NULL)
  {
    hFontOld = (HFONT) SelectObject (hdc, hFont) ;
    oldMode = SetBkMode (hdc, TRANSPARENT) ;
    oldForeground = SetTextColor (hdc, RGB (0, 0, 0)) ;
    DrawText (hdc, text, -1, rect, DT_WORDBREAK | DT_EXPANDTABS) ;
    SetTextColor (hdc, oldForeground) ;
    SetBkMode (hdc, oldMode) ;
    SelectObject (hdc, hFontOld) ;
    DeleteObject (hFont) ;
  }
}

void tip_of_the_day (HDC hdc, RECT *rect, char *text)
{
  int         oldMode ;
  RECT        rc ;
  HFONT       hFont ;
  HFONT       hFontOld ;
  LOGFONT     lf ;
  COLORREF    oldForeground ;

  rc = *rect ;
  memset (&lf, 0, sizeof (LOGFONT)) ;
  lf.lfHeight = -MulDiv (9, GetDeviceCaps (hdc, LOGPIXELSY), 72) ;
  lf.lfWeight = FW_BOLD ;
  lf.lfPitchAndFamily = VARIABLE_PITCH ;
  lf.lfCharSet = DEFAULT_CHARSET ;
  strcpy (lf.lfFaceName, "MS Sans Serif") ;
  if ((hFont = CreateFontIndirect (&lf)) != NULL)
  {
//  hIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_BULB)) ;
//  DrawIcon (hdc, rc.left, rc.top, hIcon) ;
//  DestroyIcon (hIcon) ;
    hFontOld = (HFONT) SelectObject (hdc, hFont) ;
    oldMode = SetBkMode (hdc, TRANSPARENT) ;
    oldForeground = SetTextColor (hdc, RGB (0, 0, 0)) ;
    ExtTextOut (hdc, rc.left, rc.top, ETO_CLIPPED, &rc, "Did you know ... ? ", 19, NULL) ;
    rc.top += 25 ;
    SetTextColor (hdc, oldForeground) ;
    SetBkMode (hdc, oldMode) ;
    SelectObject (hdc, hFontOld) ;
    DeleteObject (hFont) ;
    write_wrapped_text (hdc, &rc, text) ;
  }
}

void handle_menu_select (WPARAM wParam, LPARAM lParam)
{
  char        str [128] ;
  static int  nLastID = -1 ;

  wParam = LOWORD (wParam) ;
  if (wParam != nLastID)
  {
    nLastID = wParam ;
    if (wParam >= CM_FIRSTGUIEXT && wParam <= CM_LASTGUIEXT)
    {
      say_status_message (StatusMessage, ExternalMenuTip (wParam)) ;
      return ;
    }
    if (wParam < CM_FIRSTTOOL || wParam > CM_LASTTOOL)
    {
      if (LoadString (hInstance, wParam, str, sizeof (str)) != 0)
        say_status_message (StatusMessage, str) ;
      else
        say_status_message (StatusMessage, "") ;
    }
    else
      say_status_message (StatusMessage, tool_help [wParam - CM_FIRSTTOOL]) ;
  }
}

void resize_windows (unsigned left, unsigned top, unsigned width, unsigned height)
{
  MoveWindow (message_window, left, top, width, height, false) ;
}

FILE *editor_stream_init (void)
{
  editor_line_number = 0 ;
  editor_char_pos = 0 ;
  editor_offset = 0 ;
  seek_entry_count = 0 ;
  line_buffer [0] = '\0' ;
  return (POV_INTERNAL_STREAM) ;
}

int editor_ftell (void)
{
  if (seek_entry_count >= MAX_SEEK_INDEX)
  {
    PovMessageBox ("Seek index overflow ; render scene from outside editor", "Error") ;
    if (!quit)
      quit = time (NULL) ;
    return (-1) ;
  }
  if (editor_char_pos)
  {
    seek_entries [seek_entry_count].Line = editor_line_number ;
    seek_entries [seek_entry_count++].Offset = editor_char_pos - 1 ;
  }
  seek_entries [seek_entry_count].Line = editor_line_number ;
  seek_entries [seek_entry_count].Offset = editor_char_pos ;
  return (seek_entry_count++) ;
}

int editor_fseek (long offset, int whence)
{
  if (whence != 0)
    return (-1) ;
  editor_line_number = seek_entries [offset].Line ;
  editor_char_pos = seek_entries [offset].Offset ;
// FIXME
//if (Edit.GetLine (CurrentEditor, editor_line_number, line_buffer, sizeof (line_buffer) - 1) == false)
//{
//  PovMessageBox ("Seek index error ; render scene from outside editor", "Error") ;
//  if (!quit)
//    quit = time (NULL) ;
//  return (-1) ;
//}
  return (0) ;
}

int editor_getc (void)
{
  if (line_buffer [editor_char_pos] == '\0')
  {
// FIXME
//  if (Edit.GetLine (CurrentEditor, ++editor_line_number, line_buffer, sizeof (line_buffer) - 2) == false)
//    return (EOF) ;
    editor_char_pos = 0 ;
    strcat (line_buffer, "\n") ;
  }
  return (line_buffer [editor_char_pos++]) ;
}

FILE *pov_fopen (const char *filename, const char *mode)
{
  if (filename == NULL)
    return (editor_stream_init ()) ;
  else
    return (fopen (filename, mode)) ;
}

int pov_fclose (FILE *stream)
{
  if (stream != POV_INTERNAL_STREAM)
    return (fclose (stream)) ;
  else
    return (0) ;
}

int pov_getc (FILE *stream)
{
  return (stream == POV_INTERNAL_STREAM ? editor_getc () : getc (stream)) ;
}

int pov_fseek (FILE *stream, long offset, int whence)
{
  return (stream == POV_INTERNAL_STREAM ? editor_fseek (offset, whence) : fseek (stream, offset, whence)) ;
}

int pov_ftell (FILE *stream)
{
  return (stream == POV_INTERNAL_STREAM ? editor_ftell () : ftell (stream)) ;
}

void add_edit_file (char *file)
{
  if (strlen (file) == 0)
  {
    PovMessageBox ("Empty filename after /EDIT", "Edit File") ;
    return ;
  }
  if (EditFileCount == MAX_EDIT_FILES)
    return ;
  if (strpbrk (file, "*?") != NULL)
  {
    PovMessageBox ("Filename may not contain wildcards", "Edit File") ;
    return ;
  }
  EditFiles [EditFileCount++] = _strdup (file) ;
}

void add_render_file (char *file)
{
  static bool first = true ;

  if (strlen (file) == 0)
  {
    PovMessageBox ("Empty filename after /RENDER", "Render File") ;
    return ;
  }
  if (!first)
  {
    PovMessageBox ("Only one /RENDER file may be specified", "Render File") ;
    return ;
  }
  if (strpbrk (file, "*?") != NULL)
  {
    PovMessageBox ("Filename may not contain wildcards", "Render File") ;
    return ;
  }
  first = false ;
  strcpy (requested_render_file, file) ;
  render_requested = true ;
}

char *extract_file (char *filename, char *s)
{
  bool        startsWithQuote = false ;
  bool        inDoubleQuote = false ;

  while (*s == ' ' || *s == '\t')
    s++ ;
  // a single quote is a legitimate part of a filename under windows, so we
  // treat it as a special case only if the first character is one.
  if (*s == '\'')
  {
    startsWithQuote = true;
    s++;
  }
  while (*s)
  {
    switch (*s)
    {
      case '"' :
           if (inDoubleQuote)
           {
             *filename = '\0' ;
             return (++s) ;
           }
           inDoubleQuote = true ;
           break ;

      case '\'' :
           if (startsWithQuote && (s[1] == ' ' || s[1] == '\t' || s[1] == '\0'))
           {
             *filename  = '\0' ;
             return (++s) ;
           }
           *filename++ = *s ;
           break ;

      case ' ' :
      case '\t' :
           if (!inDoubleQuote && !startsWithQuote)
           {
             *filename  = '\0' ;
             return (s) ;
           }
           *filename++ = *s ;
           break ;

      default :
           *filename++ = *s ;
           break ;
    }
    s++ ;
  }
  *filename  = '\0' ;
  return (s) ;
}

char *preparse_commandline (char *s)
{
  char        *out ;
  char        *command ;
  char        last = ' ' ;
  char        commandstr [256] ;
  char        filename [_MAX_PATH] ;
  static char outstr [_MAX_PATH * 3] ;

  out = outstr ;
  while (*s)
  {
    if (*s == '/' && (last == ' ' || last == '\t'))
    {
      command = commandstr ;
      while (*++s)
      {
        if (*s == ' ' || *s == '\t')
          break ;
        *command++ = *s ;
      }
      *command = '\0' ;
      last = *s ;
      if (strlen (commandstr) == 0)
      {
        PovMessageBox ("Empty command on commandline", "Commandline processing error") ;
        return (NULL) ;
      }
      if (_stricmp (commandstr, "EXIT") == 0)
      {
        exit_after_render = true ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "DEMO") == 0)
      {
        demo_mode = true ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "DEBUG") == 0)
      {
        debugging = true ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "EDIT") == 0)
      {
        s = extract_file (filename, s) ;
        add_edit_file (filename) ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "RENDER") == 0)
      {
        s = extract_file (filename, s) ;
        add_render_file (filename) ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "EDITDLLPATH") == 0)
      {
        s = extract_file (filename, s) ;
        appendPathSeparator (filename);
        EditDLLPath = _strdup(filename);
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "NOEXEC") == 0)
      {
        noexec = true ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "NORESTORE") == 0 || _stricmp (commandstr, "NR") == 0)
      {
        NoRestore = true ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "THREADS") == 0)
      {
        int n ;
        while (*s == ' ')
          s++ ;
        if (sscanf (s, "%d", &n) != 1)
        {
          PovMessageBox ("Invalid numerical value following '/THREADS'", "Commandline processing error") ;
          return (NULL) ;
        }
        if (n < 1 || n > 255)
        {
          PovMessageBox ("Invalid value for thread count (range 1-255)", "Commandline processing error") ;
          return (NULL) ;
        }
        ThreadCount = n ;
        while (isdigit (*s))
          s++ ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "BENCHMARK") == 0)
      {
        benchmark_mode = true ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
#ifdef POVMS_NETWORK_SUPPORT
      if (_stricmp (commandstr, "LISTEN") == 0)
      {
        ListenMode = true ;
        s = extract_file (NetworkAddress, s) ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "CONTROL") == 0)
      {
        ControlMode = true ;
        s = extract_file (NetworkAddress, s) ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
#endif
      sprintf (outstr, "Unrecognized command '%s' on commandline", commandstr) ;
      PovMessageBox (outstr, "Commandline processing error") ;
      return (NULL) ;
    }
    last = *out++ = *s++ ;
  }
  return (outstr) ;
}

char *preparse_instance_commandline (char *s)
{
  char        *out ;
  char        *command ;
  char        last = ' ' ;
  char        commandstr [256] ;
  char        filename [_MAX_PATH] ;
  static char outstr [_MAX_PATH * 3] ;

  out = outstr ;
  while (*s)
  {
    if (*s == '/' && (last == ' ' || last == '\t'))
    {
      command = commandstr ;
      while (*++s)
      {
        if (*s == ' ' || *s == '\t')
          break ;
        *command++ = *s ;
      }
      *command = '\0' ;
      last = *s ;
      if (strlen (commandstr) == 0)
      {
        PovMessageBox ("Empty command on commandline", "Commandline processing error") ;
        return (NULL) ;
      }
      if (_stricmp (commandstr, "EXIT") == 0)
      {
        PovMessageBox ("Only /EDIT and /RENDER may be passed to previous instance", "Commandline processing error") ;
        return (NULL) ;
      }
      if (_stricmp (commandstr, "DEMO") == 0)
      {
        PovMessageBox ("Only /EDIT and /RENDER may be passed to previous instance", "Commandline processing error") ;
        return (NULL) ;
      }
      if (_stricmp (commandstr, "DEBUG") == 0)
      {
        PovMessageBox ("Only /EDIT and /RENDER may be passed to previous instance", "Commandline processing error") ;
        return (NULL) ;
      }
      if (_stricmp (commandstr, "NOEXEC") == 0)
      {
        PovMessageBox ("Only /EDIT and /RENDER may be passed to previous instance", "Commandline processing error") ;
        return (NULL) ;
      }
      if (_stricmp (commandstr, "THREADS") == 0)
      {
        PovMessageBox ("Only /EDIT and /RENDER may be passed to previous instance", "Commandline processing error") ;
        return (NULL) ;
      }
      if (_stricmp (commandstr, "EDIT") == 0)
      {
        s = extract_file (filename, s) ;
        add_edit_file (filename) ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "RENDER") == 0)
      {
        s = extract_file (filename, s) ;
        add_render_file (filename) ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (_stricmp (commandstr, "NORESTORE") == 0 || _stricmp (commandstr, "NR") == 0)
      {
        while (*s == ' ')
          s++ ;
        continue ;
      }
      sprintf (outstr, "Unrecognized command '%s' on commandline", commandstr) ;
      PovMessageBox (outstr, "Commandline processing error") ;
      return (NULL) ;
    }
    last = *out++ = *s++ ;
  }
  return (outstr) ;
}

INT_PTR CALLBACK PovStatusPanelDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

HWND create_toolbar (HWND hwndParent)
{
  int                   fontWidth ;
  int                   fontHeight ;
  HDC                   hdc ;
  HWND                  hwnd ;
  RECT                  rect ;
  HFONT                 hFontOld ;
  HFONT                 hParentFont ;
  HBITMAP               hbmp ;
  TBBUTTON              main_tbb [MAX_MAIN_TOOLS] ;
  TBBUTTON              aux_tbb [MAX_AUX_TOOLS] ;
  TBBUTTON              *dt ;
  TEXTMETRIC            tm ;
  toolbarStruct         *st ;
  REBARBANDINFO         rbBand ;

  hParentFont = (HFONT) SendMessage (hwndParent, WM_GETFONT, 0, 0) ;
  hdc = GetDC (hwndParent) ;
  hFontOld = (HFONT) SelectObject (hdc, hParentFont) ;
  GetTextMetrics (hdc, &tm) ;
  SelectObject (hdc, hFontOld) ;
  ReleaseDC (hwndParent, hdc) ;

  fontWidth = tm.tmAveCharWidth ;
  fontHeight = tm.tmHeight ;

  toolbar_combobox_width = fontWidth * 30 ;
  toolbar_cmdline_width = fontWidth * 20 ;

  // Initialize REBARBANDINFO for all rebar bands
  rbBand.cbSize = sizeof (REBARBANDINFO) ;
  rbBand.clrFore = GetSysColor (COLOR_BTNTEXT) ;
  rbBand.clrBack = GetSysColor (COLOR_BTNFACE) ;
  rbBand.hbmBack = LoadBitmap (hInstance, MAKEINTRESOURCE (BMP_REBAR)) ;

  hwnd = CreateWindow (TOOLBARCLASSNAME,
                       "",
                       WS_CHILD | WS_VISIBLE | CCS_TOP | TBSTYLE_TOOLTIPS |
                       TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | WS_CLIPCHILDREN |
                       WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | CCS_ADJUSTABLE,
                       0,
                       0,
                       256,
                       50,
                       hwndParent,
                       NULL,
                       hInstance,
                       NULL) ;
  if (hwnd == NULL)
    return (NULL) ;
  SendMessage (hwnd, TB_BUTTONSTRUCTSIZE, sizeof (TBBUTTON), 0) ;
  SendMessage (hwnd, TB_SETBITMAPSIZE, 0, (LPARAM) MAKELONG (38, 21)) ;
  SendMessage (hwnd, TB_SETBUTTONSIZE, 0, (LPARAM) MAKELONG (38, 21)) ;

  if ((ToolImages1 = ImageList_Create (38, 21, (screen_depth < 8 ? ILC_COLOR4 : ILC_COLOR24) | ILC_MASK, 0, 16)) == NULL)
    return (NULL) ;
  for (dt = main_tbb, st = maintools ; st->text != NULL ; st++, dt++)
  {
    dt->iBitmap = ImageList_AddMasked (ToolImages1, hbmp = LoadBitmap (hInstance, MAKEINTRESOURCE (st->resid)), RGB (255, 0, 255)) ;
    dt->iString = SendMessage (hwnd, TB_ADDSTRING, 0, (LPARAM) st->text) ;
    dt->idCommand = st->id ;
    dt->fsState = TBSTATE_ENABLED ;
    dt->fsStyle = st->flags & 0x01 ? TBSTYLE_CHECK : TBSTYLE_BUTTON ;
    dt->fsState = st->flags & 0x02 ? TBSTATE_HIDDEN | TBSTATE_ENABLED : TBSTATE_ENABLED ;
    dt->dwData = 0 ;
    DeleteObject (hbmp) ;
  }
  SendMessage (hwnd, TB_SETIMAGELIST, 0, (LPARAM) ToolImages1) ;
  SendMessage (hwnd, TB_ADDBUTTONS, MAX_MAIN_TOOLS, (LPARAM) main_tbb) ;
  SendMessage (hwnd, TB_AUTOSIZE, 0, 0) ;

  rbBand.fMask = RBBIM_COLORS | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE ;
  rbBand.fStyle = RBBS_NOVERT | RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_GRIPPERALWAYS ;
  rbBand.hwndChild = hwnd ;
  SendMessage (hwnd, TB_GETITEMRECT, MAX_MAIN_TOOLS - 1, (LPARAM) &rect) ;
  rbBand.cxMinChild = 1 ;
  rbBand.cyMinChild = rect.bottom ;
  rbBand.cx = rect.right + 15 ;
  rbBand.wID = 0 ;
  SendMessage (hwndParent, RB_INSERTBAND, (UINT) -1, (LPARAM) (LPREBARBANDINFO) &rbBand) ;
  DeleteObject (hbmp) ;

  statuspanel = CreateDialog (hInstance, MAKEINTRESOURCE (IDD_STATUSPANEL), hwnd, PovStatusPanelDialogProc) ;
  rbBand.fMask = RBBIM_COLORS | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE ;
  rbBand.fStyle = RBBS_NOVERT | RBBS_CHILDEDGE | RBBS_FIXEDSIZE ;
  rbBand.hwndChild = statuspanel ;
  rbBand.cx = 200 ;
  rbBand.cxMinChild = 200 ;
  rbBand.cyMinChild = 42 ;
  rbBand.wID++;
  SendMessage (hwndParent, RB_INSERTBAND, (UINT) -1, (LPARAM) (LPREBARBANDINFO) &rbBand) ;

  toolbar_combobox = CreateWindow ("COMBOBOX", // WC_COMBOBOXEX,
                                   "",
                                   WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP |
                                   WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NORESIZE,
                                   0,
                                   0,
                                   toolbar_combobox_width,
                                   fontHeight * 20,
                                   hwndParent,
                                   NULL,
                                   hInstance,
                                   NULL) ;

  SendMessage (toolbar_combobox, WM_SETFONT, (WPARAM) hParentFont, true) ;
  rbBand.fMask = RBBIM_COLORS | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE ;
  rbBand.fStyle = RBBS_NOVERT | RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_GRIPPERALWAYS | RBBS_BREAK ;
  rbBand.hwndChild = toolbar_combobox ;
  GetWindowRect (toolbar_combobox, &rect) ;
  rbBand.cx = rect.right - rect.left ;
  rbBand.cxMinChild = rbBand.cx / 2 ;
  rbBand.cyMinChild = rect.bottom - rect.top ;
  rbBand.wID++;
  if (bandWidths[rbBand.wID] > 10)
    rbBand.cx = bandWidths[rbBand.wID] ;
  SendMessage (hwndParent, RB_INSERTBAND, (UINT) -1, (LPARAM) (LPREBARBANDINFO) &rbBand) ;

  toolbar_cmdline = CreateWindowEx (WS_EX_CLIENTEDGE,
                                    "EDIT",
                                    "",
                                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | ES_AUTOHSCROLL,
                                    0,
                                    0,
                                    toolbar_cmdline_width + GetSystemMetrics (SM_CXEDGE) * 2,
                                    fontHeight + GetSystemMetrics (SM_CYEDGE) * 2 + 3,
                                    hwndParent,
                                    NULL,
                                    hInstance,
                                    NULL) ;

  SendMessage (toolbar_cmdline, EM_LIMITTEXT, _MAX_PATH, 0) ;
  SendMessage (toolbar_cmdline, WM_SETFONT, (WPARAM) hParentFont, true) ;
  GetWindowRect (toolbar_cmdline, &rect) ;
  rbBand.fMask = RBBIM_COLORS | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE ;
  rbBand.fStyle = RBBS_NOVERT | RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_GRIPPERALWAYS ;
  rbBand.hwndChild = toolbar_cmdline ;
  rbBand.cx = rect.right - rect.left ;
  rbBand.cxMinChild = rbBand.cx / 2 ;
  rbBand.cyMinChild = rect.bottom - rect.top ;
  rbBand.wID++;
  if (bandWidths[rbBand.wID] > 10)
    rbBand.cx = bandWidths[rbBand.wID] ;
  SendMessage (hwndParent, RB_INSERTBAND, (UINT) -1, (LPARAM) &rbBand) ;

  HWND tt ;
  if ((tt = (HWND) SendMessage (hwnd, TB_GETTOOLTIPS, 0, 0)) != NULL)
  {
    // we borrow the toolbar's tooltip rather than create a new one
    TOOLINFO ti ;
    memset (&ti, 0, sizeof (TOOLINFO)) ;
    ti.cbSize = sizeof (TOOLINFO) ;
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS ;
    ti.hwnd = hwndParent ;
    ti.uId = (UINT_PTR) toolbar_cmdline ;
    ti.lpszText = "Command line (overrides preset render options)" ;
    SendMessage (tt, TTM_ADDTOOL, 0, (LPARAM) &ti) ;
    ti.uId = (UINT_PTR) toolbar_combobox ;
    ti.lpszText = "Preset render options" ;
    SendMessage (tt, TTM_ADDTOOL, 0, (LPARAM) &ti) ;
  }

  aux_toolbar_window = CreateWindow (TOOLBARCLASSNAME,
                                     "",
                                     WS_CHILD | WS_VISIBLE | CCS_TOP | TBSTYLE_TOOLTIPS |
                                     TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TRANSPARENT |
                                     WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE,
                                     0,
                                     0,
                                     256,
                                     50,
                                     hwndParent,
                                     NULL,
                                     hInstance,
                                     NULL) ;
  if (aux_toolbar_window == NULL)
    return (NULL) ;

  SendMessage (aux_toolbar_window, TB_BUTTONSTRUCTSIZE, sizeof (TBBUTTON), 0) ;
  SendMessage (aux_toolbar_window, TB_SETBITMAPSIZE, 0, (LPARAM) MAKELONG (14, 14)) ;
  SendMessage (aux_toolbar_window, TB_SETBUTTONSIZE, 0, (LPARAM) MAKELONG (14, 14)) ;

  if ((ToolImages2 = ImageList_Create (14, 14, (screen_depth < 8 ? ILC_COLOR4 : ILC_COLOR24) | ILC_MASK, 0, 8)) == NULL)
    return (hwnd) ;
  for (dt = aux_tbb, st = auxtools ; st->text != NULL ; st++, dt++)
  {
    dt->iBitmap = ImageList_AddMasked (ToolImages2, hbmp = LoadBitmap (hInstance, MAKEINTRESOURCE (st->resid)), RGB (255, 0, 255)) ;
    dt->iString = SendMessage (aux_toolbar_window, TB_ADDSTRING, 0, (LPARAM) st->text) ;
    dt->idCommand = st->id ;
    dt->fsState = TBSTATE_ENABLED ;
    dt->fsStyle = st->flags & 0x01 ? TBSTYLE_CHECK : TBSTYLE_BUTTON ;
    dt->dwData = 0 ;
    DeleteObject (hbmp) ;
  }
  SendMessage (aux_toolbar_window, TB_SETIMAGELIST, 0, (LPARAM) ToolImages2) ;
  SendMessage (aux_toolbar_window, TB_ADDBUTTONS, MAX_AUX_TOOLS, (LPARAM) aux_tbb) ;
  SendMessage (aux_toolbar_window, TB_AUTOSIZE, 0, 0) ;

  rbBand.fMask = RBBIM_COLORS | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE ;
  rbBand.fStyle = RBBS_NOVERT | RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_GRIPPERALWAYS ;
  rbBand.hwndChild = aux_toolbar_window ;
  SendMessage (aux_toolbar_window, TB_GETITEMRECT, MAX_AUX_TOOLS - 1, (LPARAM) &rect) ;
  rbBand.cxMinChild = 1 ;
  rbBand.cyMinChild = rect.bottom ;
  rbBand.cx = rect.right + 15 ;
  rbBand.wID++;
  if (bandWidths[rbBand.wID] > 10)
    rbBand.cx = bandWidths[rbBand.wID] ;
  SendMessage (hwndParent, RB_INSERTBAND, (UINT) -1, (LPARAM) (LPREBARBANDINFO) &rbBand) ;

  return (hwnd) ;
}

HWND create_rebar (HWND hwndParent)
{
  HWND                            hwnd ;
  INITCOMMONCONTROLSEX            icex ;

  icex.dwSize = sizeof (INITCOMMONCONTROLSEX) ;
  icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES ;
  InitCommonControlsEx (&icex) ;
  hwnd = CreateWindowEx (0L,
                         REBARCLASSNAME,
                         NULL,
                         WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN |
                         /*CCS_NOPARENTALIGN |*/ CCS_NODIVIDER | WS_CLIPSIBLINGS |
                         RBS_VARHEIGHT | RBS_BANDBORDERS | RBS_DBLCLKTOGGLE,
                         0,
                         0,
                         800,
                         128,
                         hwndParent,
                         (HMENU) ID_REBAR,
                         hInstance,
                         NULL) ;
  return (hwnd) ;
}

HWND CreateStatusbar (HWND hwndParent)
{
  HWND                            hwnd ;
  RECT                            rect ;

  hwnd = CreateWindowEx (0L,
                         STATUSCLASSNAME,
                         NULL,
                         WS_CHILD | SBARS_SIZEGRIP | WS_VISIBLE,
                         0,
                         0,
                         0,
                         0,
                         hwndParent,
                         (HMENU) ID_STATUS,
                         hInstance,
                         NULL) ;
  GetClientRect (hwnd, &rect) ;
  statusheight = rect.bottom - rect.top ;

  StatusTooltip = CreateWindowEx(NULL,
                                 TOOLTIPS_CLASS,
                                 NULL,
                                 WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 hwndParent,
                                 NULL,
                                 hInstance,
                                 NULL);
  SetWindowPos(StatusTooltip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

  memset (&sbToolInfo, 0, sizeof (TOOLINFO)) ;
  sbToolInfo.cbSize = sizeof (TOOLINFO) ;
  sbToolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS ;
  sbToolInfo.hwnd = hwndParent ;
  sbToolInfo.uId = (UINT_PTR) hwnd ;
  sbToolInfo.lpszText = LPSTR_TEXTCALLBACK;
  SendMessage (StatusTooltip, TTM_ADDTOOL, 0, (LPARAM) &sbToolInfo) ;

  return (hwnd) ;
}

// should really query the statusbar and get the font size, then
// work out the section sizes from that.
void ResizeStatusBar (HWND hwnd)
{
  int                   parts [] = {0, 0, 0, 0, 0, 0, 0, 0} ;
  int                   i ;
  int                   total = 0 ;
  RECT                  rect ;
  static const int      widths [] = {0, 65, 50, 50, 50, 50, 95, 135} ;
  static const int      all = 495 ;

  GetClientRect (hwnd, &rect) ;
  rect.right -= all ;
  for (i = 0 ; i < sizeof (parts) / sizeof (int) ; i++)
  {
    total += widths [i] ;
    parts [i] = total + rect.right ;
  }
  SendMessage (hwnd, SB_SETPARTS, sizeof (parts) / sizeof (int), (LPARAM) parts) ;
}

bool HandleStatusTooltip(NMHDR *nmh)
{
  int                   parts[32];
  int                   count;
  int                   off;
  char                  str[512];
  char                  *s;
  RECT                  rect ;
  POINT                 pos;
  NMTTDISPINFO          *di;

  switch (nmh->code)
  {
     case TTN_NEEDTEXT :
          di = (NMTTDISPINFO *) nmh;
          if ((count = SendMessage(StatusWindow, SB_GETPARTS, 32, (LPARAM) parts)) == 0)
            return false;
          GetCursorPos (&pos) ;
          GetWindowRect(StatusWindow, &rect);
          if (pos.x < rect.left || pos.y > rect.right)
            return false;
          off = pos.x - rect.left;
          di->hinst = hInstance ;
          di->lpszText = sbToolText;
          sbToolText[0] = '\0';
          for (int i = 0; i < count; i++)
          {
            if (parts[i] == -1 || off < parts[i])
            {
              SendMessage(StatusWindow, SB_GETTEXT, i, (LPARAM) str);
              if (str[0] == '\0')
                return true;
              switch (i)
              {
                case StatusMessage :
                     strcpy(sbToolText, str);
                     break ;

                case StatusMem :
                     sprintf(sbToolText, "Current memory usage: %s", str);
                     break ;

                case StatusLine:
                     sprintf(sbToolText, "Line: %s", str + 3);
                     break ;

                case StatusCol:
                     sprintf(sbToolText, "Column: %s", str + 3);
                     break ;

                case StatusIns:
                     strcpy(sbToolText, str[1] == 'I' ? "Insert mode" : "Overwrite mode");
                     break ;

                case StatusModified:
                     strcpy(sbToolText, "File has been modified");
                     break ;

                case StatusPPS :
                     if ((s = strchr(str, ' ')) == NULL)
                       break;
                     *s = '\0';
                     sprintf(sbToolText, "%s Pixels per Second", str);
                     break ;

                case StatusRendertime :
                     sprintf(sbToolText, "Elapsed Time: %s", str);
                     break ;
              }
              break;
            }
          }
          return true;
  }
  return false;
}

void say_status_message (int section, const char *message)
{
  char        str [256] = "\t" ;

  switch (section)
  {
    case StatusMessage :
         SendMessage (StatusWindow, SB_SETTEXT, StatusMessage, (LPARAM) message) ;
         break ;

    case StatusMem :
         SendMessage (StatusWindow, SB_SETTEXT, StatusMem, (LPARAM) message) ;
         break ;

    case StatusPPS :
         strncat (str, message, sizeof (str) - strlen (str) - 1) ;
         SendMessage (StatusWindow, SB_SETTEXT, StatusPPS, (LPARAM) str) ;
         break ;

    case StatusRendertime :
         SendMessage (StatusWindow, SB_SETTEXT, StatusRendertime, (LPARAM) message) ;
         break ;
  }
}

}
// end of namespace povwin
