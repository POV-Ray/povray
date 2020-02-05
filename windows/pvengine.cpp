//******************************************************************************
///
/// @file windows/pvengine.cpp
///
/// This file implements Windows specific routines, WinMain, and message loops.
///
/// @author Christopher J. Cason (primary author).
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

/// @file
/// @deprecated Much of this source code (all of pvengine.exe) should be considered on
///             its last legs. The base code was originally written back in early 1995
///             (with some of the code even predating that), and targeted at Win32s under
///             Windows 3.1. An additional constraint was a desire for the code to be
///             able to be compiled on a DEC Alpha under NT for the Alpha using only a
///             'C' compiler, and for it to be able to run with or without the editor.
///
/// @par        With the introduction of VFE in version 3.7, creating a new UI for POVWIN
///             should be a lot less painful as almost all of the bindings with the core
///             POV-Ray renderer have been abstracted into the above class library.

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#if !defined( __BORLANDC__ ) && !defined( __DMC__ ) && !defined( __MINGW32__ )
  #pragma comment(lib, "htmlhelp")
  #pragma comment(lib, "winmm")
#endif

#define _CRT_RAND_S
#define PSAPI_VERSION 1
#pragma comment(lib, "psapi")

#include <windows.h>
#include <htmlhelp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <commctrl.h>
#include <crtdbg.h>
#include <psapi.h>
#include <dbghelp.h>
#include <io.h>

// #define DEVELOPMENT

#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <tchar.h>

#include <memory>
#include <mutex>

#include "pvengine.h"
#include "resource.h"
#include "pvdialog.h"
#include "pvguiext.h"
#include "pvedit.h"
#include "backend/control/benchmark.h"
#include "pvdisplay.h"
#include "backend/povray.h"

#ifdef RTR_SUPPORT
#include "rtrsupport.h"
#endif

#include "cpuid.h"

#ifdef TRY_OPTIMIZED_NOISE
// TODO - This is a hack; we should get the noise generator choice information via POVMS from the back-end.
#include "core/material/noise.h"
#endif


// this must be the last file included
#include "syspovdebug.h"

typedef HRESULT (CALLBACK* DLLGETVERSIONPROC) (DLLVERSIONINFO *);
typedef DWORD __stdcall shCopyType (HKEY, LPCTSTR, HKEY, DWORD) ;

#ifndef SM_CMONITORS
#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define SM_CMONITORS            80
#endif

#ifndef PROCESS_MODE_BACKGROUND_BEGIN
#define PROCESS_MODE_BACKGROUND_BEGIN     0x00100000
#define PROCESS_MODE_BACKGROUND_END       0x00200000
#endif

#define CONFIRM_STOP_THRESHOLD 900

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                         CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                         CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                         CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                                         );

char *WriteDump(struct _EXCEPTION_POINTERS *pExceptionInfo, bool full, long timestamp);

namespace pov_frontend
{
  extern std::shared_ptr<Display> gDisplay;
}
// end of namespace pov_frontend

using namespace pov;
using namespace pov_frontend;

void GenerateDumpMeta(bool brief);

namespace povwin
{

using std::map;
using std::set;
using std::pair;

typedef struct
{
  int         id ;
  HWND        hwnd ;
  SIZE        size ;
} StatusPanelItem ;

bool                    rendering ;
bool                    stop_rendering ;
bool                    rendersleep ;
bool                    render_cooperate = false ;
bool                    InFrontend = false ;
bool                    ErrorNotified ;
bool                    ErrorOccurred ;
bool                    HideRenderWithMain ;
bool                    UseAlpha;
bool                    IsExpired;
bool                    BackendFailedFlag = false;
bool                    PreventSleep = true;
char                    DumpMeta[65536];
std::string             ErrorMessage ;
std::string             ErrorFilename ;
unsigned                ErrorLine ;
unsigned                ErrorCol ;
unsigned                render_priority = CM_RENDERPRIORITY_NORMAL ;
unsigned                Duty_Cycle = 9 ;
unsigned                renderwin_8bits ;

extern int              renderwin_xoffset ;
extern int              renderwin_yoffset ;
extern int              renderwin_left ;
extern int              renderwin_top ;
extern char             PovLegacyRenderWinClass [] ;
extern unsigned         renderwin_max_width ;
extern unsigned         renderwin_max_height ;
extern unsigned         renderwin_flags ;
extern bool             MakeRenderwinActive ;

int                     render_bitmap_depth ;
unsigned                render_width ;
unsigned                render_height ;

int                     alert_sound ;
int                     run_count ;
int                     render_anim_count ;
int                     argc ;
int                     io_restrictions ;
int                     tb_combo_sel ;
int                     povray_return_code ;
int                     PerformanceScale = 1 ;
int                     seconds_for_last_line = -1 ;
int                     delay_next_status ;
int                     screen_origin_x ;
int                     screen_origin_y ;
int                     virtual_screen_width ;
int                     virtual_screen_height ;
int                     LastRenderPercentage ;
int                     renderwin_transparency ;
int                     cb_expect_selchange ;
char                    command_line [_MAX_PATH * 3] ;
char                    old_command_line [_MAX_PATH * 3] ;
char                    *argv [MAX_ARGV] ;
char                    source_file_name [_MAX_PATH] ;
char                    modulePath [_MAX_PATH] ;
char                    engineHelpPath [_MAX_PATH] ;
char                    lastRenderName [_MAX_PATH] ;
char                    lastBitmapName [_MAX_PATH] ;
char                    lastRenderPath [_MAX_PATH] ;
char                    lastBitmapPath [_MAX_PATH] ;
char                    lastQueuePath [_MAX_PATH] ;
char                    lastSecondaryIniFilePath [_MAX_PATH] ;
char                    DefaultRenderIniFileName [_MAX_PATH] ;
char                    SecondaryRenderIniFileName [_MAX_PATH] ;
char                    SecondaryRenderIniFileSection [64] ;
char                    background_file [_MAX_PATH] ;
char                    EngineIniFileName[_MAX_PATH] ;
char                    BinariesPath [_MAX_PATH] ;
char                    DocumentsPath [_MAX_PATH] ;
char                    LastInferredHome [_MAX_PATH] ;
char                    ToolIniFileName [_MAX_PATH] ;
char                    tool_commands [MAX_TOOLCMD] [MAX_TOOLCMDTEXT] ;
char                    tool_help [MAX_TOOLCMD] [MAX_TOOLHELPTEXT] ;
char                    requested_render_file [_MAX_PATH] ;
char                    RegionStr [128] ;
char                    TempRegionStr [128] ;
char                    demo_file_name [_MAX_PATH] ;
char                    demo_ini_name [_MAX_PATH] ;
char                    status_buffer [1024] ;
char                    render_complete_sound [_MAX_PATH] ;
char                    parse_error_sound [_MAX_PATH] ;
char                    render_error_sound [_MAX_PATH] ;
char                    FontPath [_MAX_PATH] ;
char                    *EditDLLPath ;
bool                    render_complete_sound_enabled ;
bool                    parse_error_sound_enabled  ;
bool                    render_error_sound_enabled ;
bool                    alert_on_completion ;
bool                    save_settings ;
bool                    IsW95UserInterface = true;
bool                    IsW98 ;
bool                    IsWNT ;
bool                    IsW2k ;
bool                    IsWXP ;
bool                    IsVista ;
bool                    running_demo ;
bool                    running_benchmark ;
bool                    benchmark_multithread ;
bool                    fast_scroll ;
bool                    no_shellout_wait ;
bool                    tile_background = false ;
bool                    debugging ;
bool                    no_palette_warn ;
bool                    render_lock_up ;
bool                    RenderwinIsChild = true ;
bool                    demo_mode ;
bool                    benchmark_mode;
bool                    ignore_auto_ini ;
bool                    newVersion ;
bool                    exit_after_render ;
bool                    system_noactive ;
bool                    one_instance = true ;
bool                    run_renderer ;
bool                    use_toolbar = true ;
bool                    use_tooltips = true ;
bool                    use_editors = true ;
bool                    editors_enabled = true;
bool                    resizing ;
bool                    drop_to_editor ;
bool                    restore_command_line ;
bool                    render_requested ;
bool                    render_auto_close ;
bool                    noexec ;
bool                    ExtensionsEnabled = true ;
bool                    use_taskbar = true ;
bool                    main_window_hidden ;
bool                    about_showing ;
bool                    NoRestore ;
bool                    IsComCtl5 = false ;
bool                    IsComCtl6 = false ;
bool                    allow_rw_source ;
bool                    no_shell_outs = true ;
bool                    hide_newuser_help ;
bool                    info_render_complete = false ;
bool                    no_status_output = false ;
bool                    temp_render_region = false ;
bool                    rendering_insert_menu = false ;
bool                    was_insert_render = false ;
bool                    rendering_animation = false ;
bool                    preserve_bitmap = false ;
bool                    first_frame = false ;
bool                    check_new_version ;
bool                    check_news ;
bool                    send_system_info ;
bool                    homeInferred = false ;
bool                    editSettingsCopied = false ;
bool                    FreshInstall = false;
bool                    output_to_file ;
bool                    UpdateCheckDone = false ;
bool                    AutoAppendPaths ;
HWND                    toolbar_window ;
HWND                    aux_toolbar_window ;
HWND                    window_list [MAX_WINDOWS] ;
HWND                    toolbar_combobox ;
HWND                    rebar_window ;
HWND                    StatusWindow ;
HWND                    StatusTooltip ;
HWND                    toolbar_cmdline ;
HWND                    tab_window ;
HICON                   ourIcon ;
HICON                   renderIcon ;
HFONT                   about_font ;
HANDLE                  hMainThread ;
std::string             InputFileName ;
time_t                  SecondCountStart ;
time_t                  quit ;
HBITMAP                 hBmpBackground ;
HBITMAP                 hBmpRendering ;
HBITMAP                 hBmpIcon ;
HBITMAP                 hBmpAbout ;
__int64                 PerformanceFrequency ;
__int64                 PerformanceCounter1 ;
__int64                 PerformanceCounter2 ;
__int64                 KernelTimeStart ;
__int64                 KernelTimeEnd ;
__int64                 UserTimeStart ;
__int64                 UserTimeEnd ;
__int64                 KernelTimeTotal ;
__int64                 UserTimeTotal ;
__int64                 CPUTimeTotal ;
clock_t                 ClockTimeStart ;
clock_t                 ClockTimeEnd ;
clock_t                 ClockTimeTotal ;
clock_t                 SleepTimeStart ;
clock_t                 SleepTimeEnd ;
clock_t                 SleepTimeTotal ;
unsigned                class_registered = 0 ;
unsigned                currentX = 0 ;
unsigned                currentY = 0 ;
unsigned                screen_width ;
unsigned                screen_height ;
unsigned                screen_depth ;
unsigned                background_width ;
unsigned                background_height ;
unsigned                seconds = 0 ;
unsigned                toolheight = 0 ;
unsigned                statusheight = 0 ;
unsigned                on_completion = CM_COMPLETION_NOTHING ;
unsigned                window_count = 0 ;
unsigned                ThreadCount = 2 ;
unsigned                NumberOfCPUs ;
HPALETTE                hPalApp ;
HPALETTE                hPalBitmap ;
COLORREF                background_colour ;
COLORREF                text_colours [3] ;
COLORREF                custom_colours [16] ;
COLORREF                background_shade = RGB (1, 1, 1) ;
HINSTANCE               hInstance ;
HH_AKLINK               hh_aklink ;
OSVERSIONINFO           version_info ;
StatusPanelItem         StatusPanelItems [IDC_STATUS_ID_LAST + 1] ;
CRITICAL_SECTION        critical_section ;

// key is the name of an included file (all lower case).
// content is the name of the most recent rendered file that caused it to be included.
map<std::string, std::string> IncludeToSourceMap;
map<std::string, bool>    IncludeAlternateDecisionMap;

char                    queued_files [MAX_QUEUE] [_MAX_PATH] ;
char                    dir [_MAX_PATH] ;
unsigned                queued_file_count = 0 ;
unsigned                auto_render = true ;
unsigned                timer_id ;
unsigned                about_timer_id ;
unsigned                timer_ticks;

unsigned                panel_size ;

char                    PovMainWinClass [] = CLASSNAMEPREFIX "MainWinClass" ;
unsigned                mainwin_xpos ;
unsigned                mainwin_ypos ;
HWND                    main_window ;
HWND                    message_window ;
WINDOWPLACEMENT         mainwin_placement ;

char                    PovAboutWinClass [] = CLASSNAMEPREFIX "AboutWinClass" ;
HWND                    about_window ;
HWND                    about_buttons [3] ;
WNDPROC                 about_button_wndproc ;
unsigned                about_width ;
unsigned                about_height ;
HPALETTE                about_palette ;

HWND                    statuspanel ;

char                    PovMessageWinClass [] = CLASSNAMEPREFIX "MessageWinClass" ;

#define NUM_ABOUT_LINKS 7
RECT                    AboutLinks[NUM_ABOUT_LINKS] =
                        {
                          {  23,  14, 188,  87 },
                          { 104, 397, 184, 409 },
                          { 185, 397, 262, 409 },
                          { 263, 397, 370, 409 },
                          { 371, 397, 480, 409 },
                          { 481, 397, 560, 409 },
                          {  32, 411, 154, 423 }
                        };
char                    *AboutURLs[NUM_ABOUT_LINKS] =
                        {
                          "http://www.povray.org/",
                          "http://www.digicert.com/",
                          "http://ntplx.net/",
                          "http://opensourcelaw.biz/",
                          "http://www.perforce.com/",
                          "http://softwarefreedom.org/",
                          "http://softwarefreedom.org/"
                        };

const char              *CanInheritFromVersions[] =
                        {
#ifdef POVRAY_IS_BETA
                          "v" POV_RAY_GENERATION,
#endif
                          "v3.7",
                          "v3.6",
                          NULL // end of list
                        };

bool handle_main_command (WPARAM wParam, LPARAM lParam) ;
void SetStatusPanelItemText (int id, LPCSTR format, ...) ;
void ShowAboutBox (void);

extern int              message_xchar ;
extern int              message_ychar ;
extern int              message_scroll_pos_x ;
extern int              message_scroll_pos_y ;
extern int              top_message_row ;
extern int              message_count ;
extern int              message_cols ;
extern int              message_rows ;
extern int              listbox_xchar ;
extern int              listbox_ychar ;
extern int              EditFileCount ;
extern int              message_output_x ;
extern char             message_font_name [256] ;
extern char             *EditFiles [] ;
extern bool             ListenMode ;
extern bool             keep_messages ;
extern bool             MenuBarDraw ;
extern bool             TrackMem;
extern __int64          PeakMem;
extern unsigned         message_font_size ;
extern unsigned         message_font_weight ;
extern HFONT            message_font ;
extern HFONT            tab_font ;
extern HMENU            hMenuBar ;
extern HMENU            hMainMenu ;
extern HMENU            hPopupMenus ;
extern HMENU            hVidcapMenu ;
extern HACCEL           hAccelerators ;
extern HINSTANCE        hLibPovEdit ;

#define MAX_INSERT_MENU_SECTIONS  8192

std::string selectedNoiseFunc;

typedef std::vector<int> InsMenuSecList;

int                     InsertMenuSection;
bool                    StartInsertRender;
InsMenuSecList          InsertMenuSections;

typedef struct
{
  WORD        wVirtkey ;
  int         iMessage ;
  WORD        wRequest ;
} SCROLLKEYS ;

SCROLLKEYS key2scroll [] =
{
  { VK_END,   WM_VSCROLL, SB_BOTTOM   },
  { VK_PRIOR, WM_VSCROLL, SB_PAGEUP   },
  { VK_NEXT,  WM_VSCROLL, SB_PAGEDOWN },
  { VK_UP,    WM_VSCROLL, SB_LINEUP   },
  { VK_DOWN,  WM_VSCROLL, SB_LINEDOWN },
  { VK_LEFT,  WM_HSCROLL, SB_PAGEUP   },
  { VK_RIGHT, WM_HSCROLL, SB_PAGEDOWN },
  { -1,       -1,         -1          }
} ;

std::string StripFilePath (const std::string& str)
{
  std::string::size_type pos  = str.find_last_of ("\\/") ;
  if (pos == std::string::npos)
    return (str) ;
  return std::string(str, pos + 1);
}

bool StripPathComponent (char *path, int number)
{
  if (number > 1)
    if (!StripPathComponent (path, number - 1))
      return (false) ;
  char *s = path + strlen (path) - 1 ;
  if (isPathSeparator(*s))
    *s-- = '\0' ;
  while (s > path && !isPathSeparator(*s))
    s-- ;
  *s = '\0' ;
  return (path [0] != '\0') ;
}

void debug_output (const char *format, ...)
{
  char                  str [2048] ;
  va_list               arg_ptr ;
  FILE                  *f ;

  if (format == NULL)
  {
    _unlink ("c:\\temp\\povdebug.txt") ;
    return ;
  }

  va_start (arg_ptr, format) ;
  vsprintf (str, format, arg_ptr) ;
  va_end (arg_ptr) ;
  OutputDebugString (str) ;

  if ((f = fopen ("c:\\temp\\povdebug.txt", "a+t")) != NULL)
  {
    fprintf (f, sizeof(time_t) == 4 ? "%u: %s" : "%I64u: %s", time (NULL), str) ;

    fclose (f) ;
  }
}

// this function is declared in syspovconfig.h ... use pov::WIN32_DEBUG_OUTPUT() from
// anywhere in the POV-Ray source, core or otherwise
void WIN32_DEBUG_OUTPUT (const char *format,...)
{
  char                  str [2048] ;
  va_list               arg_ptr ;

  sprintf (str, "%5d ", GetCurrentThreadId());
  va_start (arg_ptr, format) ;
  vsprintf (str + 6, format, arg_ptr) ;
  va_end (arg_ptr) ;
  OutputDebugString (str) ;
}

// this function is declared in syspovconfig.h ... use pov::WIN32_DEBUG_FILE_OUTPUT() from
// anywhere in the POV-Ray source, core or otherwise. it's also thread-safe.
void WIN32_DEBUG_FILE_OUTPUT (const char *format,...)
{
  va_list               arg_ptr ;
  static FILE           *f ;
  static std::mutex     mtx;

  if (format == NULL)
  {
    if (f != NULL)
    {
      fclose (f) ;
      f = NULL ;
    }
    return ;
  }

  if (f == NULL)
  {
    f = fopen ("c:\\temp\\povdebug.txt", "at") ;
    if (f == NULL)
      return ;
  }

  std::lock_guard<std::mutex> l(mtx);
  fprintf (f, "%u [%d]: ", GetTickCount (), GetCurrentThreadId ()) ;
  va_start (arg_ptr, format) ;
  vfprintf (f, format, arg_ptr) ;
  va_end (arg_ptr) ;
  fflush (f);
}

void SetCaption (LPCSTR str)
{
  char        buffer [1024] ;
  static char lastStr [1024] ;
  vfeSession& Session (GetSession());

  if (str != NULL)
    strcpy (lastStr, str) ;
  else
    str = lastStr ;

  if (Session.BackendFailed())
  {
    SetWindowText (main_window, "Backend Failed") ;
    return;
  }

#ifndef DEVELOPMENT
  if (Session.GetBackendState() == kRendering && InputFileName.size () > 0)
  {
    std::string filename = StripFilePath (InputFileName) ;
    if (strstr (str, filename.c_str ()) != NULL)
    {
      // if the filename is in the caption already, we're probably rendering
      // the current editor file (exception: if a file with the same name but
      // a different path is being viewed).
      sprintf (buffer, "%s [%s %d%%]", str, Session.GetBackendStateName(), Session.GetPercentComplete()) ;
    }
    else
    {
      // the render file name is not in the caption: in that case we are
      // probably viewing another file or the message window.
      sprintf (buffer, "%s [Rendering %s: %d%%]", str, filename.c_str (), Session.GetPercentComplete()) ;
    }
  }
  else
    sprintf (buffer, "%s [%s]", str, Session.GetBackendStateName ()) ;
  SetWindowText (main_window, buffer) ;
#else
  if (Session.GetBackendState() == kRendering && InputFileName.size () > 0)
  {
    std::string filename = StripFilePath (InputFileName) ;
    if (strstr (str, filename.c_str ()) != NULL)
    {
      // if the filename is in the caption already, we're probably rendering
      // the current editor file (exception: if a file with the same name but
      // a different path is being viewed).
      sprintf (buffer, CAPTIONPREFIX " %s [%s %d%%]", str, Session.GetBackendStateName(), Session.GetPercentComplete()) ;
    }
    else
    {
      // the render file name is not in the caption: in that case we are
      // probably viewing another file or the message window.
      sprintf (buffer, CAPTIONPREFIX " %s [Rendering %s:%d%%]", str, filename.c_str (), Session.GetPercentComplete()) ;
    }
  }
  else
    sprintf (buffer, CAPTIONPREFIX " %s [%s]", str, Session.GetBackendStateName()) ;
  SetWindowText (main_window, buffer) ;
#endif
}

// returned value is in microseconds
__int64 GetCPUTime (bool Kernel = true, bool User = true)
{
  __int64     kt ;
  __int64     ut ;
  __int64     total = 0 ;
  FILETIME    ct ;
  FILETIME    et ;

  if (IsWNT)
  {
    if (!GetProcessTimes (GetCurrentProcess (), &ct, &et, (FILETIME *) &kt, (FILETIME *) &ut))
    {
      assert (false) ;
      return (0) ;
    }
    if (Kernel)
      total += kt ;
    if (User)
      total += ut ;
    return (total / 10) ;
  }
  else
  {
    // have to simulate the results for now
    // TODO: handle pause time
    ut = clock () ;
    if (User)
      total += ut * 1000 ;
    return (total) ;
  }
}

char *PPS_String (unsigned pixels, unsigned renderseconds)
{
  static char str [128] ;

  if (rendersleep)
    return ("PAUSED") ;

  if (renderseconds == 0)
    return ("??? PPS") ;

  if (pixels / renderseconds < 5)
  {
    if (pixels * 60 / renderseconds < 5)
      sprintf (str, "%u PPH", pixels * 3600 / renderseconds) ;
    else
      sprintf (str, "%u PPM", pixels * 60 / renderseconds) ;
  }
  else
    sprintf (str, "%u PPS", pixels / renderseconds) ;
  return (str) ;
}

void PrintRenderTimes (int Finished, int NormalCompletion)
{
  int         PixelsRendered = GetSession().GetPixelsRendered();
  unsigned    STT = SleepTimeTotal ;

  if (rendersleep)
  {
    SleepTimeEnd = clock () ;
    if (Finished)
    {
      SleepTimeTotal += SleepTimeEnd - SleepTimeStart ;
      STT = SleepTimeTotal ;
      rendersleep = false ;
    }
    else
      STT += SleepTimeEnd - SleepTimeStart ;
  }
  KernelTimeEnd = GetCPUTime (true, false) ;
  UserTimeEnd = GetCPUTime (false, true) ;
  KernelTimeTotal = KernelTimeEnd - KernelTimeStart ;
  UserTimeTotal = UserTimeEnd - UserTimeStart ;
  CPUTimeTotal = UserTimeTotal + KernelTimeTotal ;
  ClockTimeEnd = clock () ;
  ClockTimeTotal = ClockTimeEnd - ClockTimeStart - STT ;
  if (ClockTimeTotal >= CLOCKS_PER_SEC)
    status_printf (StatusPPS, PPS_String (PixelsRendered, ClockTimeTotal / CLOCKS_PER_SEC)) ;
  say_status_message (StatusRendertime, get_elapsed_time (ClockTimeTotal / CLOCKS_PER_SEC)) ;
  if (IsWNT != 0 && Finished != 0)
  {
    message_printf ("CPU time used: kernel %.02f seconds, user %.02f seconds, total %.02f seconds.\n",
                     KernelTimeTotal / 1000000.0, UserTimeTotal / 1000000.0, CPUTimeTotal / 1000000.0) ;
    message_printf ("Elapsed time %.02f seconds", (double) ClockTimeTotal / CLOCKS_PER_SEC) ;
    if (NumberOfCPUs > 1 && ThreadCount > 1)
    {
      POV_ULONG cputotal = CPUTimeTotal * CLOCKS_PER_SEC / 1000000 ;
      if (cputotal > ClockTimeTotal * 10 / 9)
        message_printf (", CPU vs elapsed time ratio %.02f", (double) cputotal / ClockTimeTotal) ;
    }
    message_printf (".\n") ;
    if (PixelsRendered > 0 && CPUTimeTotal > 0 && NormalCompletion)
    {
      message_printf ("Render averaged %.02f PPS (%.02f PPS CPU time) over %u pixels.\n",
        (double) PixelsRendered * CLOCKS_PER_SEC / ClockTimeTotal, (double) PixelsRendered * 1000000 / CPUTimeTotal, PixelsRendered) ;
      message_printf ("----------------------------------------------------------------------------\n") ;
#ifndef _DEBUG
      char str [2048] ;
      if (running_benchmark)
      {
        char *s = str;
        s += sprintf(str, "CPU time used: kernel %.02f seconds, user %.02f seconds, total %.02f seconds.\n",
          KernelTimeTotal / 1000000.0, UserTimeTotal / 1000000.0, CPUTimeTotal / 1000000.0) ;
        s += sprintf(s, "Elapsed time %.02f seconds", (double) ClockTimeTotal / CLOCKS_PER_SEC) ;
        if (NumberOfCPUs > 1 && ThreadCount > 1)
        {
          POV_ULONG cputotal = CPUTimeTotal * CLOCKS_PER_SEC / 1000000 ;
          if (cputotal > ClockTimeTotal * 10 / 9)
            s += sprintf(s, ", CPU vs elapsed time ratio %.02f", (double) cputotal / ClockTimeTotal) ;
        }
        s += sprintf(s, ".\n") ;
        s += sprintf (s, "Render averaged %.02f PPS (%.02f PPS CPU time) over %u pixels using %u thread(s).\n",
          (double) PixelsRendered * CLOCKS_PER_SEC / ClockTimeTotal, (double) PixelsRendered * 1000000 / CPUTimeTotal, PixelsRendered, ThreadCount) ;
        copy_text_to_clipboard(str);
        if (benchmark_mode)
        {
          // special case: we are running the benchmark because of a command-line request.
          char fn[_MAX_PATH];
          char ts[256];
          time_t t = time(NULL);

          strftime(ts, sizeof(ts), "%Y%m%d.%H%M%S", gmtime(&t));
          sprintf(fn, "%sbenchmark-%s.txt", DocumentsPath, ts);
          FILE *f = fopen(fn, "wt");
          if (f != NULL)
          {
            int n = Get_Benchmark_Version();

            fprintf(f, "%s", str);
            strftime(str, sizeof(str), "%#c", gmtime(&t));
            fprintf(f, "\nRender of benchmark version %x.%02x completed at %s UTC.\n", n / 256, n % 256, str);
            fprintf(f, "----------------------------------------------------------------------------\n");
            GenerateDumpMeta(true);
            fwrite(DumpMeta, strlen(DumpMeta), 1, f);
            fprintf(f, "povversion=%s\n", POV_RAY_SOURCE_VERSION) ;
            fprintf(f, "compilerversion=%s\n", POV_COMPILER_VER) ;
            fprintf(f, "platformversion=%s\n", POVRAY_PLATFORM_NAME) ;
#ifdef TRY_OPTIMIZED_NOISE
            fprintf(f, "noisefunctions=%s\n", selectedNoiseFunc.c_str()) ;
#endif // TRY_OPTIMIZED_NOISE
            fclose(f);
          }
        }
        else
        {
          strcat(str, "\nThese results have been placed in the clipboard.\n");
          MessageBox (main_window, str, "Benchmark Complete.", MB_OK) ;
        }
      }
#endif
    }
    else
      message_printf ("----------------------------------------------------------------------------\n") ;
  }
}

bool OkToStopRendering (void)
{
  if (GetSession().BackendFailed())
    return (true);
  if (time (NULL) - SecondCountStart < CONFIRM_STOP_THRESHOLD)
    return (true) ;
  if (GetSession().GetRealTimeRaytracing() == true)
    return (true);
  if (MessageBox (main_window, "You've been running this render for quite a while - really cancel ?", "Stop rendering ?", MB_ICONQUESTION | MB_YESNO) == IDYES)
    return (true) ;
  return (false) ;
}

void menuhelp (UINT idCommand)
{
  switch (idCommand)
  {
    case CM_FILEMENUHELP :
         hh_aklink.pszKeywords = "File Menu" ;
         break ;

    case CM_EDITMENUHELP :
         hh_aklink.pszKeywords = "Edit Menu" ;
         break ;

    case CM_SEARCHMENUHELP :
         hh_aklink.pszKeywords = "Search Menu" ;
         break ;

    case CM_TEXTMENUHELP :
         hh_aklink.pszKeywords = "Text Menu" ;
         break ;

    case CM_EDITORMENUHELP :
         hh_aklink.pszKeywords = "Editor Menu" ;
         break ;

    case CM_RENDERMENUHELP :
         hh_aklink.pszKeywords = "Render Menu" ;
         break ;

    case CM_OPTIONSMENUHELP :
         hh_aklink.pszKeywords = "Options Menu" ;
         break ;

    case CM_TOOLSMENUHELP :
         hh_aklink.pszKeywords = "Tools Menu" ;
         break ;

    case CM_WINDOWMENUHELP :
         hh_aklink.pszKeywords = "Window Menu" ;
         break ;

    case CM_RENDERWINMENUHELP :
         hh_aklink.pszKeywords = "Render Window Menu" ;
         break ;

    case CM_MESSAGEWINMENUHELP :
         hh_aklink.pszKeywords = "Message Window Menu" ;
         break ;

    default :
         hh_aklink.pszKeywords = NULL ;
         break ;
  }
  if (hh_aklink.pszKeywords != NULL)
    HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
}

// example taken from MSDN documentation
DWORD GetDllVersion (LPCTSTR lpszDllName)
{
  HINSTANCE hinstDll;
  DWORD dwVersion = 0;

  hinstDll = LoadLibrary(lpszDllName) ;

  if (hinstDll)
  {
    DLLGETVERSIONPROC pDllGetVersion;

    pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress (hinstDll, "DllGetVersion");

    if (pDllGetVersion)
    {
      DLLVERSIONINFO dvi;
      HRESULT hr;

      ZeroMemory(&dvi, sizeof (dvi));
      dvi.cbSize = sizeof (dvi);

      hr = (*pDllGetVersion) (&dvi);

      if (SUCCEEDED (hr))
        dwVersion = MAKELONG (dvi.dwMajorVersion, dvi.dwMinorVersion);
    }

    FreeLibrary (hinstDll);
  }
  return dwVersion;
}

void getvars (ExternalVarStruct *v)
{
  strcpy (v->command_line, command_line) ;
  strcpy (v->source_file_name, source_file_name) ;
  strcpy (v->lastRenderName, lastRenderName) ;
  strcpy (v->lastRenderPath, lastRenderPath) ;
  strcpy (v->lastQueuePath, lastQueuePath) ;
  strcpy (v->lastSecondaryIniFilePath, lastSecondaryIniFilePath) ;
  strcpy (v->DefaultRenderIniFileName, DefaultRenderIniFileName) ;
  strcpy (v->SecondaryRenderIniFileName, SecondaryRenderIniFileName) ;
  strcpy (v->SecondaryRenderIniFileSection, SecondaryRenderIniFileSection) ;
  strcpy (v->ourPath, modulePath) ;
  strcpy (v->engineHelpPath, engineHelpPath) ;
  strcpy (v->rendererHelpPath, "") ;
  strcpy (v->BinariesPath, BinariesPath) ;
  strcpy (v->EngineIniFileName, EngineIniFileName) ;
  strcpy (v->ToolIniFileName, ToolIniFileName) ;
  memcpy (v->queued_files, queued_files, sizeof (v->queued_files)) ;
  v->loadRerun = false ;
  v->continueRerun = false ;
  v->povray_return_code = povray_return_code ;
  v->rendering = rendering ;
  v->IsWin32 = true ;
  v->IsW95UserInterface = IsW95UserInterface ;
  v->running_demo = running_demo ;
  v->debugging = debugging ;
  v->isMaxiMinimized = false ;
  v->newVersion = newVersion ;
  v->use_threads = true ;
  v->use_toolbar = use_toolbar ;
  v->use_tooltips = use_tooltips ;
  v->use_editors = use_editors ;
  v->drop_to_editor = drop_to_editor ;
  v->rendersleep = rendersleep ;
  v->ExtensionsEnabled = ExtensionsEnabled ;
  v->queued_file_count = queued_file_count > OLD_MAX_QUEUE ? OLD_MAX_QUEUE : queued_file_count ;
  v->auto_render = auto_render ;
}

void setvars (ExternalVarStruct *v)
{
  strncpy (command_line, v->command_line, sizeof (command_line) - 1) ;
  SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
  strncpy (source_file_name, v->source_file_name, sizeof (source_file_name) - 1) ;
  strncpy (lastRenderName, v->lastRenderName, sizeof (lastRenderName) - 1) ;
  strncpy (lastRenderPath, v->lastRenderPath, sizeof (lastRenderPath) - 1) ;
  strncpy (lastQueuePath, v->lastQueuePath, sizeof (lastQueuePath) - 1) ;
  strncpy (lastSecondaryIniFilePath, v->lastSecondaryIniFilePath, sizeof (lastSecondaryIniFilePath) - 1) ;
  strncpy (DefaultRenderIniFileName, v->DefaultRenderIniFileName, sizeof (DefaultRenderIniFileName) - 1) ;
  strncpy (SecondaryRenderIniFileName, v->SecondaryRenderIniFileName, sizeof (SecondaryRenderIniFileName) - 1) ;
  strncpy (SecondaryRenderIniFileSection, v->SecondaryRenderIniFileSection, sizeof (SecondaryRenderIniFileSection) - 1) ;
}

bool HaveWin95OrLater (void)
{
  return (version_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) ;
}

bool HaveWin98OrLater (void)
{
  if (version_info.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
    return (false) ;
  if (version_info.dwMajorVersion < 4)
    return (false) ;
  if (version_info.dwMajorVersion > 4)
    return (true) ;
  return (version_info.dwMinorVersion > 0) ;
}

bool HaveNT4OrLater (void)
{
  return (version_info.dwPlatformId == VER_PLATFORM_WIN32_NT && version_info.dwMajorVersion >= 4) ;
}

bool HaveWin2kOrLater (void)
{
  return (version_info.dwPlatformId == VER_PLATFORM_WIN32_NT && version_info.dwMajorVersion >= 5) ;
}

bool HaveWinXPOrLater (void)
{
  if (version_info.dwPlatformId != VER_PLATFORM_WIN32_NT || version_info.dwMajorVersion < 5)
    return (false) ;
  return (version_info.dwMajorVersion > 5 || (version_info.dwMajorVersion == 5 && version_info.dwMinorVersion > 0)) ;
}

bool HaveVistaOrLater (void)
{
  return version_info.dwPlatformId == VER_PLATFORM_WIN32_NT && version_info.dwMajorVersion > 5;
}

void set_render_priority (unsigned priority)
{
  switch (priority)
  {
    case CM_RENDERPRIORITY_BACKGROUND :
         SetPriorityClass (GetCurrentProcess(), PROCESS_MODE_BACKGROUND_BEGIN) ;
         break ;

    case CM_RENDERPRIORITY_LOW :
         if (IsVista)
           SetPriorityClass (GetCurrentProcess(), PROCESS_MODE_BACKGROUND_END);
         SetPriorityClass (GetCurrentProcess(), IDLE_PRIORITY_CLASS) ;
         break ;

    case CM_RENDERPRIORITY_NORMAL :
         if (IsVista)
           SetPriorityClass (GetCurrentProcess(), PROCESS_MODE_BACKGROUND_END);
         SetPriorityClass (GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS) ;
         break ;

    case CM_RENDERPRIORITY_HIGH :
         if (IsVista)
           SetPriorityClass (GetCurrentProcess(), PROCESS_MODE_BACKGROUND_END);
         SetPriorityClass (GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS) ;
         break ;
  }
}

// we can't allow LoadBitmap to load our background bitmaps 'cause if we're running
// a 256-colour mode, it will map the incoming resource to 16 colours ...
// LoadImage () doesn't exist under Win32s, either. sigh.
HBITMAP NonBogusLoadBitmap (HINSTANCE hInst, LPSTR lpszBitmap)
{
  void        *p ;
  HRSRC       hres ;
  HGLOBAL     hg ;
  HBITMAP     hBitmap ;

  if ((hres = FindResource (hInst, lpszBitmap, RT_BITMAP)) == NULL)
    return (NULL) ;
  if ((hg = LoadResource (hInst, hres)) == NULL)
    return (NULL) ;
  if ((p = LockResource (hg)) == NULL)
  {
    FreeResource (hg) ;
    return (NULL) ;
  }
  hBitmap = lpDIBToBitmap (p, hPalApp) ;
  FreeResource (hg) ;
  return (hBitmap) ;
}

HBITMAP NonBogusLoadBitmapAndPalette (HINSTANCE hInst, LPSTR lpszBitmap)
{
  void        *p ;
  HRSRC       hres ;
  HGLOBAL     hg ;
  HBITMAP     hBitmap ;

  if ((hres = FindResource (hInst, lpszBitmap, RT_BITMAP)) == NULL)
    return (NULL) ;
  if ((hg = LoadResource (hInst, hres)) == NULL)
    return (NULL) ;
  if ((p = LockResource (hg)) == NULL)
  {
    FreeResource (hg) ;
    return (NULL) ;
  }
  hBitmap = lpDIBToBitmapAndPalette (p) ;
  FreeResource (hg) ;
  return (hBitmap) ;
}

// finds fist separator character in a path
char *findFirstPathSeparator (char *s)
{
  size_t  pos = strcspn(s, "\\/");

  return pos >= strlen(s) ? NULL : s + pos;
}

// finds last separator character in a path
char *findLastPathSeparator (char *s)
{
  char    *s1 = strrchr (s, '\\');
  char    *s2 = strrchr (s1 ? s1 : s, '/');

  return s2 ? s2 : s1;
}

// append separator character to a path, if not present already
// does not append to empty strings.
// does append to solitary drive letters; this isn't quite legit but as we don't
// support the use of drive references without a path (i.e. references that rely
// on the uniquely-DOS/Windows concept of a CWD for *each* mounted device), it
// doesn't really matter.
void appendPathSeparator (char *str)
{
  if (str[0] != '\0') // only append to non-empty strings
    if (!hasTrailingPathSeparator(str))
      strcat(str, "\\");
}

// tests if path has trailing separator character
// does not return true if path is a solitary drive letter (e.g. "C:").
// the same consideration applies regarding relative drive references as
// mentioned in appendPathSeparator.
bool hasTrailingPathSeparator (const char *str)
{
  return isPathSeparator(str[strlen (str) - 1]);
}

// strips trailing separator character from path
// will strip a trailing separator right after drive letter (e.g. "C:\").
// calling functions must be careful not to let this trip them up if they
// are passed any user-specified input.
void trimTrailingPathSeparator (char *str)
{
  char *s = str + strlen (str) - 1 ;
  if (isPathSeparator(*s))
    *s = '\0' ;
}

// strips leading and trailing double-quotes from a string.
// does not check that both are present, just removes them
// if they are there. returns new start of string.
char *trimDoubleQuotes (char *str)
{
  if (str[0] == '"')
    str++;
  char *s = str + strlen (str) - 1 ;
  if (*s == '"')
    *s = '\0' ;
  return str;
}

// strips trailing separator character from path
void validatePath (char *s)
{
  if (s [1] == ':' && strlen (s) < 4)
    return ; // make sure we don't trim a trailing separator right after a drive letter (e.g. "C:\")
  trimTrailingPathSeparator (s);
}

int joinPath (char *out, const char *path, const char *name)
{
  strcpy (out, path) ;
  appendPathSeparator (out);
  strcat (out, name) ;
  return ((int) strlen (out)) ;
}

bool reg_printf (bool useHKCU, char *keyName, char *valName, char *format, ...)
{
  char                  str [2048] ;
  HKEY                  hKey ;
  va_list               arg_ptr ;

  if (strlen (format) > sizeof (str) - 256)
    return (false) ;
  if (RegCreateKeyEx (useHKCU ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, keyName, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
  {
    va_start (arg_ptr, format) ;
    vsprintf (str, format, arg_ptr) ;
    va_end (arg_ptr) ;
    RegSetValueEx (hKey, valName, 0, REG_SZ, (BYTE *) str, (int) strlen (str) + 1) ;
    RegCloseKey (hKey) ;
    return (true) ;
  }
  return (false) ;
}

// conditional version of reg_printf
bool cond_reg_printf (char *keyName, char *valName, char *format, ...)
{
  char                  str [2048] ;
  DWORD                 len = sizeof (str) ;
  HKEY                  hKey ;
  va_list               arg_ptr ;

  if (strlen (format) > sizeof (str) - 256)
    return (false) ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, keyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx (hKey, valName, 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
    {
      RegCloseKey (hKey) ;
      // it already exists - if it doesn't have zero length we don't update it
      if (str [0])
        return (true) ;
    }
    else
      RegCloseKey (hKey) ;
  }
  if (RegCreateKeyEx (HKEY_CURRENT_USER, keyName, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
  {
    va_start (arg_ptr, format) ;
    vsprintf (str, format, arg_ptr) ;
    va_end (arg_ptr) ;
    RegSetValueEx (hKey, valName, 0, REG_SZ, (BYTE *) str, (int) strlen (str) + 1) ;
    RegCloseKey (hKey) ;
    return (true) ;
  }
  return (false) ;
}

static bool reg_dword (char *keyName, char *valName, DWORD value)
{
  HKEY                  hKey ;

  if (RegCreateKeyEx (HKEY_CURRENT_USER, keyName, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
  {
    RegSetValueEx (hKey, valName, 0, REG_DWORD, (BYTE *) &value, 4) ;
    RegCloseKey (hKey) ;
    return (true) ;
  }
  return (false) ;
}

char *GetInstallTime (void)
{
  HKEY        key ;
  DWORD       len ;
  static char str [64] ;

  len = sizeof (str) ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY, 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx (key, INSTALLTIMEKEY, 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
    {
      RegCloseKey (key) ;
      return (str) ;
    }
    RegCloseKey (key) ;
  }
  return (NULL) ;
}

bool checkRegKey (void)
{
  char        str [_MAX_PATH] ;
  HKEY        key ;
  DWORD       val;
  DWORD       len = sizeof (str) ;
  FILETIME    file_time ;
  SYSTEMTIME  system_time ;

  if (GetInstallTime () == NULL)
  {
    GetSystemTime (&system_time) ;
    if (SystemTimeToFileTime (&system_time, &file_time))
      reg_printf (true, "Software\\" REGKEY, INSTALLTIMEKEY, "%I64u", ((__int64) file_time.dwHighDateTime << 32) | file_time.dwLowDateTime) ;
  }

  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx (key, "Home", 0, NULL, (BYTE *) str, &len) != 0)
      str[0] = '\0';
    if (str [0] == '\0')
    {
      RegCloseKey (key) ;
      return (false) ;
    }
    len = sizeof(val);
    if (RegQueryValueEx (key, "FreshInstall", 0, NULL, (BYTE *) &val, &len) == 0)
    {
      FreshInstall = val != 0;
      RegCloseKey (key) ;
      if (FreshInstall && RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", 0, KEY_READ | KEY_SET_VALUE, &key) == ERROR_SUCCESS)
      {
        RegDeleteValue(key, "FreshInstall");
        RegCloseKey (key) ;
      }
    }
    else
      RegCloseKey (key) ;
  }
  else
    return (false) ;

  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY "\\CurrentVersion\\Windows", 0, KEY_READ | KEY_WRITE, &key) == ERROR_SUCCESS)
  {
    len = sizeof (str) ;
    if (RegQueryValueEx (key, VERSIONVAL, 0, NULL, (BYTE *) str, &len) != 0 || strcmp (str, POV_RAY_SOURCE_VERSION) != 0)
        RegSetValueEx (key, VERSIONVAL, 0, REG_SZ, (BYTE *) POV_RAY_SOURCE_VERSION, (int) strlen (POV_RAY_SOURCE_VERSION) + 1) ;
    RegCloseKey (key) ;
  }

  return true;
}

bool getHome (void)
{
  HKEY        key ;
  DWORD       len ;

  if (debugging)
    debug_output ("querying registry\n") ;
  DocumentsPath [0] = BinariesPath [0] = '\0' ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    len = sizeof (LastInferredHome) ;
    RegQueryValueEx (key, "LastInferredHome", 0, NULL, (BYTE *) LastInferredHome, &len) ;

    len = sizeof (BinariesPath) ;
    RegQueryValueEx (key, "Home", 0, NULL, (BYTE *) BinariesPath, &len) ;
    if (debugging && BinariesPath[0] != '\0')
      debug_output("Win32 getHome() succeeded (HKCU::Home), BinariesPath is '%s'\n", BinariesPath) ;

    len = sizeof (DocumentsPath) ;
    RegQueryValueEx (key, "DocPath", 0, NULL, (BYTE *) DocumentsPath, &len) ;
    if (debugging && DocumentsPath[0] != '\0')
      debug_output("Win32 getHome() succeeded (HKCU::DocPath), DocumentsPath is '%s'\n", DocumentsPath) ;

    RegCloseKey (key) ;
    return (BinariesPath[0] != '\0') ;
  }
  if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    len = sizeof (BinariesPath) ;
    RegQueryValueEx (key, "Home", 0, NULL, (BYTE *) BinariesPath, &len) ;
    RegCloseKey (key) ;
    if (debugging && BinariesPath[0] != '\0')
      debug_output("Win32 getHome() succeeded (HKLM::Home), BinariesPath is '%s'\n", BinariesPath) ;
    return (BinariesPath[0] != '\0') ;
  }
  return (false) ;
}

bool inferHome (void)
{
  char        exePath [_MAX_PATH] ;
  char        *s ;

  if (GetModuleFileName (NULL, exePath, _MAX_PATH) == 0)
    return (false) ;

  // find path component
  if ((s = findLastPathSeparator (exePath)) == NULL)
    return (false) ;
  *s = '\0' ;

  // now step up one directory
  if ((s = findLastPathSeparator (exePath)) == NULL)
    return (false) ;
  *++s = '\0' ;

  // now look for some standard directories
  strcpy (s, "help") ;
  if (!dirExists (exePath))
  {
    strcpy (s, "sounds") ;
    if (!dirExists (exePath))
    {
      strcpy (s, "tiles") ;
      if (!dirExists (exePath))
        return (false) ;
    }
  }

  *s = '\0' ;
  strcpy (BinariesPath, exePath) ;
  homeInferred = true ;
  return (true) ;
}

std::string getHome(const char* ver)
{
  char        str[_MAX_PATH];
  HKEY        key ;
  DWORD       len = sizeof(str);

  std::string keyName = std::string("Software\\" REGKEY "\\") + ver + "\\Windows";

  if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, keyName.c_str(), 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx (key, "Home", 0, NULL, (BYTE *) str, &len) == 0)
    {
      RegCloseKey (key) ;
      appendPathSeparator(str);
      return std::string(str);
    }
    RegCloseKey (key) ;
  }
  if (RegOpenKeyEx (HKEY_CURRENT_USER, keyName.c_str(), 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx (key, "Home", 0, NULL, (BYTE *) str, &len) == 0)
    {
      RegCloseKey (key) ;
      appendPathSeparator(str);
      return std::string(str);
    }
    RegCloseKey (key) ;
  }
  return std::string();
}

bool copyEditSettings(const char* ver)
{
  HKEY        hKeySrc ;
  HKEY        hKeyDst ;
  DWORD       result ;
  HINSTANCE   hLib ;
  shCopyType  *shCopyKey ;

  if ((hLib = LoadLibrary ("shlwapi.dll")) == NULL)
    return (false) ;
  shCopyKey = (shCopyType *) GetProcAddress (hLib, "SHCopyKeyA") ;
  if (shCopyKey == NULL)
  {
    FreeLibrary (hLib) ;
    return (false) ;
  }

  std::string keyName = std::string("Software\\" REGKEY "\\") + ver;

  if (RegOpenKeyEx (HKEY_CURRENT_USER, keyName.c_str(), 0, KEY_READ, &hKeySrc) != ERROR_SUCCESS)
  {
    FreeLibrary (hLib) ;
    return (false) ;
  }

  if (RegCreateKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit", 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyDst, NULL) != ERROR_SUCCESS)
  {
    RegCloseKey (hKeySrc) ;
    FreeLibrary (hLib) ;
    return (false) ;
  }

  result = shCopyKey (hKeySrc, "POV-Edit", hKeyDst, NULL) ;
  RegCloseKey (hKeySrc) ;
  RegCloseKey (hKeyDst) ;
  FreeLibrary (hLib) ;

  return (result == ERROR_SUCCESS) ;
}

bool checkEditKey (const char* ver)
{
  HKEY        key ;

  std::string keyName = std::string("Software\\" REGKEY "\\") + ver + "\\POV-Edit";

  if (RegOpenKeyEx (HKEY_CURRENT_USER, keyName.c_str(), 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    RegCloseKey (key) ;
    return (true) ;
  }
  return (false) ;
}

// called if either our registry entries don't seem to be set up, or if they do exist and
// the FreshInstall option is set in the registry.
bool CloneOptions (void)
{
  if (debugging)
    debug_output("attempting to create registry entries\n") ;

  // don't do this if we're being called as a result of the FreshInstall registry setting
  if (!FreshInstall)
  {
    if (!reg_printf (true, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", "Home", "%s", BinariesPath))
      return (false) ;
    if (!homeInferred)
      reg_printf (true, "Software\\" REGKEY "\\CurrentVersion\\Windows", "Home", "%s", BinariesPath) ;
  }

  reg_printf (true, "Software\\" REGKEY "\\CurrentVersion\\Windows", VERSIONVAL, "%s", POV_RAY_SOURCE_VERSION) ;

  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Open",   "Open0",   "%sChanges.txt,1,1,0,0,8,2",                   DocumentsPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Recent", "Recent0", "%sChanges.txt,1,1,0,0,8,2",                   DocumentsPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Open",   "Open1",   "%sRevision.txt,1,1,0,0,8,2",                  DocumentsPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Recent", "Recent1", "%sRevision.txt,1,1,0,0,8,2",                  DocumentsPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Open",   "Open2",   "%sscenes\\advanced\\biscuit.pov,1,1,0,6,8,2", DocumentsPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Recent", "Recent2", "%sscenes\\advanced\\biscuit.pov,1,1,0,6,8,2", DocumentsPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Open",   "Open3",   "%sscenes\\advanced\\woodbox.pov,1,1,0,6,8,2", DocumentsPath) ;
  cond_reg_printf ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit\\Recent", "Recent3", "%sscenes\\advanced\\woodbox.pov,1,1,0,6,8,2", DocumentsPath) ;

  return (true) ;
}

int parse_commandline (char *s)
{
  char        *prevWord = NULL ;
  char        inQuote = '\0' ;
  static char str [_MAX_PATH * 3] ;
  static char filename [_MAX_PATH] ;

  argc = 0 ;
  GetModuleFileName (hInstance, filename, sizeof (filename) - 1) ;
  argv [argc++] = filename ;
  s = strncpy (str, s, sizeof (str) - 1) ;
  while (*s)
  {
    switch (*s)
    {
      case '"' :
      case '\'' :
           if (inQuote)
           {
             if (*s == inQuote)
               inQuote = 0 ;
           }
           else
           {
             inQuote = *s ;
             if (prevWord == NULL)
               prevWord = s ;
           }
           break ;

      case ' ' :
      case '\t' :
           if (!inQuote)
           {
             if (prevWord != NULL)
             {
               *s = '\0' ;
               argv [argc++] = prevWord ;
               prevWord = NULL ;
             }
           }
           break ;

      default :
           if (prevWord == NULL)
             prevWord = s ;
           break ;
    }
    if (argc >= MAX_ARGV - 1)
      break ;
    s++ ;
  }
  if (prevWord != NULL && argc < MAX_ARGV - 1)
    argv [argc++] = prevWord ;
  argv [argc] = NULL ;
  return (argc) ;
}

int InstallSettings (char *binpath, char *docpath, bool quiet)
{
  char        base [_MAX_PATH] ;
  char        str [_MAX_PATH] ;
  char        *s ;

  // we attempt to infer the install dir if it's not supplied
  if (binpath == NULL)
  {
    if (_getcwd (base, sizeof (base) - 1) == NULL)
    {
      if (!quiet)
        MessageBox (NULL, "Could not get current directory - cannot infer home path. Please supply it on the command-line",
                          "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
      return 5 ;
    }
    if (strlen (base) < 3 || (strlen (base) == 3 && base [1] == ':' && isPathSeparator(base [2])))
    {
      if (!quiet)
      {
        sprintf (str, "Current dir '%s' is root - cannot infer home path. Please supply it on the command-line.", base) ;
        MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
      }
      return 10 ;
    }
    // the strlen test covers the case where base is "\\" (i.e. a network path) or a bare drive (e.g. "c:").
    if (!StripPathComponent (base, 1) || strlen(base) < 3)
    {
      if (!quiet)
        MessageBox (NULL, "Cannot infer home path. Please supply it on the command-line.",
                          "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
      return 15 ;
    }
  }
  else
  {
    strcpy (str, binpath) ;
    s = trimDoubleQuotes(str);
    validatePath (s) ;
    if (GetFullPathName (s, sizeof (base), base, NULL) == 0)
    {
      sprintf (str, "GetFullPathName() for '%s' failed [0x%08x]", s, GetLastError ()) ;
      MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
    }
  }
  if (!dirExists (base))
  {
    if (!quiet)
    {
      sprintf (str, "Could not stat directory '%s'", base) ;
      MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
    }
    return 20 ;
  }

  if (docpath != NULL)
  {
    if (!dirExists(docpath))
    {
      // if the path doesn't exist and quiet is selected, we continue, assuming that
      // by the time povray is ready to be used, it will either exist or be able to be
      // created. note that we do auto-create the path if possible (i.e. the parent
      // directory exists and is writable by the user who launches POV-Ray).
      if (!quiet)
        if (MessageBox (NULL, "Could not verify supplied user files path: use it anyway?\n(Selecting NO will exit.)", "POV-Ray for Windows - running INSTALL option", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
          return 30;
    }
    reg_printf (true, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", "DocPath", "%s\\", docpath);
  }

  if (!reg_printf (true, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", "Home", "%s\\", base))
  {
    if (!quiet)
      MessageBox (NULL, "Failed to write to HKCU in registry", "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONSTOP) ;
    return 35 ;
  }

  // it's ok for this to fail as they may not have administrative rights
  reg_printf (false, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", "Home", "%s\\", base) ;

  if (!quiet)
  {
    sprintf (str, "[Home path is %s]\n\nSuccess!", base) ;
    MessageBox (NULL, str, "POV-Ray for Windows - running INSTALL option", MB_OK | MB_ICONINFORMATION) ;
  }
  return (0) ;
}

char *GetExceptionDescription (DWORD code)
{
  switch (code)
  {
    case EXCEPTION_ACCESS_VIOLATION :
         return ("a memory access violation") ;

    case EXCEPTION_DATATYPE_MISALIGNMENT :
         return ("a datatype misalignment") ;

    case EXCEPTION_FLT_DENORMAL_OPERAND :
         return ("a denormal floating point operand") ;

    case EXCEPTION_FLT_DIVIDE_BY_ZERO :
         return ("a floating point divide by zero") ;

    case EXCEPTION_FLT_INEXACT_RESULT :
         return ("an inexact floating-point result") ;

    case EXCEPTION_FLT_INVALID_OPERATION :
         return ("an invalid floating-point operation") ;

    case EXCEPTION_FLT_OVERFLOW :
         return ("a floating-point overflow") ;

    case EXCEPTION_FLT_STACK_CHECK :
         return ("a floating-point stack over/underflow") ;

    case EXCEPTION_FLT_UNDERFLOW :
         return ("a floating-point underflow") ;

    case EXCEPTION_INT_DIVIDE_BY_ZERO :
         return ("an integer divide by zero") ;

    case EXCEPTION_INT_OVERFLOW :
         return ("an integer overflow") ;

    case EXCEPTION_PRIV_INSTRUCTION :
         return ("the execution of a privileged instruction") ;

    case EXCEPTION_IN_PAGE_ERROR :
         return ("a page error") ;

    case EXCEPTION_ILLEGAL_INSTRUCTION :
         return ("the execution of an illegal instruction") ;

    case EXCEPTION_NONCONTINUABLE_EXCEPTION :
         return ("a continuation after a noncontinuable exception") ;

    case EXCEPTION_STACK_OVERFLOW :
         return ("a stack overflow") ;

    case EXCEPTION_INVALID_DISPOSITION :
         return ("an invalid disposition") ;

    case EXCEPTION_GUARD_PAGE :
         return ("a guard page exception") ;

    case EXCEPTION_INVALID_HANDLE :
         return ("an invalid handle exception") ;

    default :
         return NULL ;
  }
}

#if POV_RAY_HAS_CRASHDUMP_UPLOAD || POV_RAY_HAS_UPDATE_CHECK
// this pulls in the code for update checks and crash dump submission.
// it is only used in official releases made by the POV-Ray developers,
// so the source is not included in the public distribution.
#include "official.h"
#else
LONG WINAPI ExceptionHandler(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
  char                        *s;
  long                        timestamp = _time32(NULL);
  PCONTEXT                    c ;
  static char                 str[2048] ;
  static std::mutex           mtx;
  std::lock_guard<std::mutex> l(mtx);

  c = ExceptionInfo->ContextRecord ;
  const char *desc = GetExceptionDescription(ExceptionInfo->ExceptionRecord->ExceptionCode);
  if (desc == NULL)
    desc = "an unrecognized exception" ;
  sprintf (str,
           "Unfortunately, it appears that %s at address 0x%p has caused this unofficial POV-Ray build to crash. "
           "This dialog will allow you to choose whether or not a dump file (useful for diagnostics) is written.\n\n"
           "NOTE: If you were running a render, you should be able to recover the part that had already been generated. "
           "See the 'Continue' (+c) option in the documentation for more information about this.\n\n"
           "Would you like to write a dump file?",
           desc,
#ifdef _WIN64
           c->Rip);
#else
           c->Eip);
#endif
  DWORD result = MessageBox (NULL, str, "POV-Ray for Windows", MB_ICONSTOP | MB_TOPMOST | MB_TASKMODAL | MB_YESNO | MB_DEFBUTTON1 | MB_SETFOREGROUND) ;
  if (result == IDYES)
  {
    // write a full dump first, then a minidump. it's no big deal if the full dump write fails.
    WriteDump(ExceptionInfo, true, timestamp);
    if ((s = WriteDump(ExceptionInfo, false, timestamp)) != NULL)
    {
      MessageBox (main_window, "The dump was successfully saved.", "POV-Ray for Windows", MB_OK) ;
      sprintf(str, "/select,%s", s);
      ShellExecute (NULL, NULL, "explorer.exe", str, NULL, SW_SHOWNORMAL) ;
      ExitProcess (1) ;
    }
  }
  else
    MessageBox (main_window, "POV-Ray will now exit.", "POV-Ray for Windows", MB_OK) ;
  ExitProcess (1) ;
  return (EXCEPTION_CONTINUE_SEARCH) ; // make compiler happy
}
#endif // POV_RAY_HAS_CRASHDUMP_UPLOAD

int execute_tool (char *s)
{
  int                   error ;
  STARTUPINFO           startupInfo ;
  PROCESS_INFORMATION   procInfo ;

  if (strlen (s) == 0)
  {
    PovMessageBox ("No command to run!", "Tool Error") ;
    return (0) ;
  }

  if (*s == '$')
  {
    switch (toupper (s[1]))
    {
      case 'S' :
           s += 2 ;
           while (*s == ' ')
             s++ ;
           if (strlen (s) == 0)
           {
             PovMessageBox ("No file to open!", "Tool Error") ;
             return (0) ;
           }
           if ((error = PtrToInt (ShellExecute (main_window, "open", s, NULL, NULL, SW_SHOWNORMAL))) <= 32)
             PovMessageBox ("ShellExecute failed", "Tool Error") ;
           return (error) ;

      case 'E' :
           s += 2 ;
           while (*s == ' ')
             s++ ;
           if (strlen (s) == 0)
           {
             PovMessageBox ("No file to open!", "Tool Error") ;
             return (0) ;
           }
           return EditOpenFile(s) ? 0 : 1;
    }
  }

  startupInfo.cb               = sizeof (STARTUPINFO) ;
  startupInfo.lpReserved       = 0 ;
  startupInfo.lpDesktop        = NULL ;
  startupInfo.lpTitle          = NULL ;
  startupInfo.dwX              = 0 ;
  startupInfo.dwY              = 0 ;
  startupInfo.dwXSize          = 0 ;
  startupInfo.dwYSize          = 0 ;
  startupInfo.dwXCountChars    = 0 ;
  startupInfo.dwYCountChars    = 0 ;
  startupInfo.dwFillAttribute  = 0 ;
  startupInfo.dwFlags          = STARTF_USESHOWWINDOW ;
  startupInfo.wShowWindow      = SW_SHOW ;
  startupInfo.cbReserved2      = 0 ;
  startupInfo.lpReserved2      = 0 ;

  if (CreateProcess (NULL, s, NULL, NULL, false, 0, NULL, NULL, &startupInfo, &procInfo) == false)
  {
    error = GetLastError () ;
    PovMessageBox ("Could not run program", "Tool Error") ;
    return (error) ;
  }

  // clean up
  CloseHandle (procInfo.hProcess) ;
  CloseHandle (procInfo.hThread) ;

  return (0) ;
}

void RenderInsertMenu (void)
{
  int         val ;
  char        str [_MAX_PATH] ;
  char        *s1 ;
  char        *s2 ;
  FILE        *f ;

  stop_rendering = false ;
  sprintf (str, "%sInsert Menu\\Images.ini", DocumentsPath) ;
  if ((f = fopen (str, "rt")) == NULL)
  {
    MessageBox (main_window, "Cannot open 'Images.ini' in Insert Menu directory", "Insert Menu Images", MB_OK | MB_ICONEXCLAMATION) ;
    return ;
  }
  InsertMenuSection = 0 ;
  InsertMenuSections.clear();
  while (fgets (str, sizeof (str), f) != NULL)
  {
    s1 = clean (str) ;
    if (*s1 == '[')
    {
      if ((s2 = strchr (s1, ']')) != NULL)
      {
        *s2  = '\0' ;
        val = std::atoi (++s1) ;
        if (val == 0)
          continue ;
        InsertMenuSections.push_back(val);
        if (InsertMenuSections.size() == MAX_INSERT_MENU_SECTIONS)
          break ;
      }
    }
  }
  fclose (f) ;
  if (InsertMenuSections.empty())
  {
    MessageBox (main_window, "No insert menu sections found in 'Images.ini'", "Insert Menu Images", MB_OK | MB_ICONSTOP) ;
    return ;
  }
  sprintf (str, "There are %u insert menu images to render. Press OK to start rendering these now.\n\n"
                "Once the render has started you can press the 'Stop Rendering' button to cancel the render job.", (unsigned int) InsertMenuSections.size()) ;
  if (MessageBox (main_window, str, "Insert Menu Images", MB_OKCANCEL | MB_ICONINFORMATION) == IDCANCEL)
    return ;
  PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  update_menu_for_render (true) ;
  rendering_insert_menu = was_insert_render = no_status_output = true ;
  EditShowMessages (true) ;
  CalculateClientWindows (true) ;
  ShowWindow (message_window, SW_SHOW) ;
  sprintf (str, "%sInsert Menu", DocumentsPath) ;
  SetCurrentDirectory (str) ;
  StartInsertRender = true ;
}

int GetUCS2String(POVMSObjectPtr object, POVMSType key, char *result, int *maxlen)
{
  UCS2 *str = new UCS2 [*maxlen] ;
  int err = POVMSUtil_GetUCS2String (object, key, str, maxlen) ;
  if (err == kNoErr)
  {
    std::string abc = UCS2toSysString (str) ;
    strcpy (result, abc.c_str ()) ;
  }
  delete str ;
  return err ;
}

void ShowIsPaused(void)
{
  if (GetSession().BackendFailed() == false && rendersleep == false)
  {
    SleepTimeStart = clock () ;
    status_printf (StatusPPS, PPS_String (GetSession().GetPixelsRendered(), ClockTimeTotal / CLOCKS_PER_SEC)) ;
    say_status_message (StatusPPS, "PAUSED") ;
    rendersleep = true ;
    SendMessage (toolbar_window, TB_CHECKBUTTON, (WPARAM) CM_RENDERSLEEP, MAKELONG (1, 0)) ;
  }
}

char *getCommandLine (void)
{
  HKEY        key ;
  static char str [2048] ;
  DWORD       len = sizeof (str) ;

  str [0] = '\0' ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    RegQueryValueEx (key, "Command Line", 0, NULL, (BYTE *) str, &len) ;
    RegCloseKey (key) ;
  }
  return (str) ;
}

void setRunOnce (void)
{
#ifndef NOSETRUNONCE
  char        str [_MAX_PATH] ;
  HKEY        key ;
  DWORD       result ;

  if (RegCreateKeyEx (HKEY_CURRENT_USER,
                      "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
                      0,
                      "",
                      REG_OPTION_NON_VOLATILE,
                      KEY_WRITE,
                      NULL,
                      &key,
                      &result) == ERROR_SUCCESS)
  {
    GetModuleFileName (hInstance, str, sizeof (str)) ;
    RegSetValueEx (key, "POV-Ray for Windows", 0, REG_SZ, (BYTE *) str, (int) strlen (str) + 1) ;
    RegCloseKey (key) ;
  }
#endif
}

void display_cleanup (bool unconditional)
{
  if (unconditional)
  {
    gDisplay.reset();
    return;
  }
  Display *d = gDisplay.get();
  if (d == NULL)
    return;
  WinDisplay *wd = dynamic_cast<WinDisplay *>(d);
  if (wd == NULL)
    return;
  if (wd->GetWidth() != render_width || wd->GetHeight() != render_height)
  {
    gDisplay.reset();
    return;
  }
  wd->Clear();
}

void cancel_render (void)
{
  stop_rendering = true ;
  if (GetSession().BackendFailed())
  {
    render_stopped() ;
    return;
  }
  if (GetSession().CancelRender() == vfeNotRunning)
  {
    // we possibly have an anamolous situation
    MessageBox (NULL, "Warning: had to force state to stopped", "Cancel Render", MB_OK | MB_ICONEXCLAMATION) ;
    render_stopped() ;
  }
}

// TODO: we have a problem here - if the parse completes very quickly, it's
// possible for the VFE code to process the change to rendering mode before
// we exit start_rendering() [note that the VFE worker thread runs at a higher
// priority than the main UI thread]. as a result, on occasion, the VFE code
// will call the display's Show() method at the same time as we are executing
// display_cleanup(). display_cleanup() can cause the value of the pointer in
// gDisplay to change, and potentially destroy the old one ...
bool start_rendering (bool ignore_source_file)
{
  int                   threadCount = ThreadCount ;
  char                  str [sizeof (command_line)] ;
  char                  path [_MAX_PATH] ;
  char                  file [_MAX_PATH] ;
  vfeSession&           Session (GetSession());

  if (Session.BackendFailed())
  {
    MessageBox (main_window, "The render backend has shut down due to an error: please re-start POV-Ray", "Error", MB_OK | MB_ICONSTOP) ;
    return false;
  }

  Session.Clear();
  Session.ClearOptions();
  if (!keep_messages)
    clear_messages(false);

  if (running_benchmark)
    threadCount = benchmark_mode || benchmark_multithread ? ThreadCount : 1 ;

  ErrorOccurred = ErrorNotified = false ;
  ErrorMessage.clear() ;
  ErrorFilename.clear() ;
  status_buffer [0] = '\0' ;
  povray_return_code = 0 ;
  rendersleep = false ;
  SleepTimeTotal = 0 ;
  render_anim_count = 0 ;
  rendering_animation = false ;
  stop_rendering = false ;
  was_insert_render = false ;
  first_frame = true ;
  render_width = render_height = 0 ;
  KernelTimeStart = GetCPUTime (true, false) ;
  UserTimeStart = GetCPUTime (false, true) ;
  CPUTimeTotal = KernelTimeTotal = UserTimeTotal = 0 ;
  ClockTimeStart = clock () ;
  SecondCountStart = time (NULL) ;
  SleepTimeTotal = ClockTimeTotal = 0 ;
  status_printf (StatusPPS, "") ;
  say_status_message (StatusRendertime, "") ;
  output_to_file = false ;
  InputFileName.clear();
  LastRenderPercentage = 0;

  SendMessage (StatusPanelItems [IDC_STATUS_PROGRESS].hwnd, PBM_SETPOS, 0, 0) ;

  say_status_message (StatusMessage, "") ;

  PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  update_menu_for_render (true) ;
  SendMessage (toolbar_combobox, CB_GETLBTEXT, SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0), (LPARAM) SecondaryRenderIniFileSection) ;

  if (!StartInsertRender)
  {
    if (!temp_render_region)
      if (RegionStr [0] != '\0' && strstr (command_line, RegionStr + 1) == NULL)
        RegionStr [0] = '\0' ;
  }

  if (save_settings)
  {
    if (restore_command_line)
    {
      strcpy (str, command_line) ;
      strcpy (command_line, old_command_line) ;
    }
    write_INI_settings (true) ;
    if (restore_command_line)
      strcpy (command_line, str) ;
    EditSaveState () ;
  }

  try
  {
    // set up render options
    vfeRenderOptions opts ;

    opts.SetThreadCount (threadCount);
    opts.AddINI (DefaultRenderIniFileName) ;

    if (AutoAppendPaths || homeInferred)
    {
      sprintf (str, "%sinclude", DocumentsPath) ;
      opts.AddLibraryPath (str);
      opts.AddLibraryPath (FontPath);
    }

    if (!StartInsertRender)
    {
      if (running_demo == 0)
      {
        if (SecondaryRenderIniFileName [0] != '\0')
        {
          if (!hasTrailingPathSeparator(SecondaryRenderIniFileName))
          {
            splitpath (SecondaryRenderIniFileName, NULL, str) ;
            if (str [0] != '\0')
            {
              if (SecondaryRenderIniFileSection [0] == '\0')
                wrapped_printf ("Preset INI file is '%s'.", SecondaryRenderIniFileName) ;
              else
                wrapped_printf ("Preset INI file is '%s', section is '%s'.", SecondaryRenderIniFileName, SecondaryRenderIniFileSection) ;
              sprintf (str, "%s%s", SecondaryRenderIniFileName, SecondaryRenderIniFileSection) ;
              opts.AddINI (str);
            }
          }
        }

        if (!ignore_source_file && strlen (source_file_name) != 0)
        {
          wrapped_printf ("Preset source file is '%s'.", source_file_name) ;
          splitpath (source_file_name, dir, NULL) ;
          SetCurrentDirectory (dir) ;
          sprintf (str, "%s\\povray.ini", dir) ;
          if (fileExists (str))
          {
            wrapped_printf ("File '%s' exists - merging it.", str) ;
            opts.AddINI (str);
          }
          switch (get_file_type (source_file_name))
          {
            case filePOV :
            case fileINC :
                opts.SetSourceFile (source_file_name);
                break ;

            case fileINI :
                opts.AddINI (source_file_name);
                break ;

            default :
                message_printf ("POV-Ray for Windows doesn't recognize this file type ; assuming POV source.\n") ;
                opts.SetSourceFile (source_file_name);
                break ;
          }
        }
      }

      if (running_benchmark)
      {
        opts.AddINI (demo_ini_name) ;
        opts.SetSourceFile (demo_file_name) ;
        if (strlen (demo_file_name) != 0)
        {
          splitpath (demo_file_name, dir, NULL) ;
          SetCurrentDirectory (dir) ;
        }
      }
      else
      {
        if (RegionStr [0] != 0)
        {
          if (strstr (command_line, RegionStr) == NULL && strstr (command_line, RegionStr + 1) == NULL)
          {
            if (!running_demo)
              message_printf ("Selected render region is '%s'.\n", RegionStr + 1) ;
            opts.AddCommand (RegionStr);
          }
        }

        if (strlen (command_line))
        {
          if (!running_demo)
            wrapped_printf ("Rendering using command line '%s'.", command_line) ;
          opts.AddCommand (command_line);
        }
      }
    }
    else
    {
      // we are rendering the insert menu
      if (InsertMenuSection >= InsertMenuSections.size())
        throw POV_EXCEPTION_STRING("Insert menu render error - we should be stopped already!");
      int section = InsertMenuSections [InsertMenuSection] ;
      sprintf (str, "Images.ini[%d]", section) ;
      opts.AddINI (str);
    }

    int result = Session.SetOptions (opts) ;
    if (result == vfeNoInputFile)
      throw POV_EXCEPTION_STRING("No source file specified, either directly or via an INI file.");
    else if (result != vfeNoError)
      throw POV_EXCEPTION_STRING (Session.GetErrorString());

    // TODO FIXME - magic values
    if (opts.GetOptions().TryGetInt(kPOVAttrib_RenderBlockSize, 32) < 4)
      throw POV_EXCEPTION (kParseErr, "Minimum permitted render block size is 4 (+BS4 or Render_Block_Size=4)") ;

    InputFileName = UCS2toSysString (Session.GetInputFilename());
    output_to_file = opts.GetOptions().TryGetBool(kPOVAttrib_OutputToFile, true) ;
    render_width = Session.GetRenderWidth();
    render_height = Session.GetRenderHeight();
    PutHKCU("LastRender", "SceneFile", "") ;
    PutHKCU("LastRender", "OutputFile", "") ;
    PutHKCU("LastRender", "IniOutputFile", "") ;
    PutHKCU("LastRender", "IniOutputFile", "") ;

    UseAlpha = Session.GetBoolOption("Output_Alpha", false);
    if (Session.GetBoolOption("Display", true) == false)
      display_cleanup(true);

    if (StartInsertRender)
      status_printf (StatusMessage, "Rendering Insert Menu entry %d of %d", InsertMenuSection + 1, (unsigned int) InsertMenuSections.size()) ;
    else
      status_printf (StatusMessage, "Parsing %s", InputFileName.c_str()) ;

    if (!running_demo && !demo_mode && !benchmark_mode)
    {
      GetCurrentDirectory (sizeof (dir), dir) ;
      PutHKCU("LastRender", "CurrentDirectory", dir) ;
      splitpath ((char *) InputFileName.c_str (), path, file) ;
      PutHKCU("LastRender", "SourceFile", get_full_name (file)) ;
      splitfn (str, NULL, file, NULL) ;
      PutHKCU("LastRender", "SceneFile", file) ;
    }

    result = Session.StartRender();
    if (result < 0)
      throw POV_EXCEPTION_CODE (result);
    else if (result > 0)
      throw POV_EXCEPTION_STRING (Session.GetErrorString());
  }
  catch (std::exception& e)
  {
    int errorCode = 0 ;
    if (dynamic_cast<pov_base::Exception *> (&e) != NULL)
    {
      errorCode = dynamic_cast<pov_base::Exception *> (&e)->code() ;
      povray_return_code = errorCode ;
    }
    if (povray_return_code == 0)
      povray_return_code = -1 ;
    ErrorOccurred = true ;
    PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
    update_menu_for_render (false) ;
    if (restore_command_line)
    {
      strcpy (command_line, old_command_line) ;
      SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (UINT_PTR) command_line) ;
      restore_command_line = false ;
    }

    // make sure any outstanding messages are processed
    ProcessSession();

    EditShowMessages (true) ;

    if (ErrorNotified == false)
    {
      sprintf (str, "Failed to start render: %s", e.what()) ;
      say_status_message (StatusMessage, str) ;
      message_printf ("%s\n", str) ;
    }
    if (errorCode != kCannotOpenFileErr && errorCode != kParseErr && errorCode != kOutOfMemoryErr)
    {
      sprintf (str, "Failed to set render options (%s).\nSee message pane for more details.", e.what()) ;
      MessageBox (main_window, str, "Error", MB_OK | MB_ICONSTOP) ;
    }

    if (ErrorFilename.empty() == false)
      EditShowParseError (ErrorFilename.c_str(), ErrorMessage.c_str(), ErrorLine, ErrorCol) ;
    if (parse_error_sound_enabled)
    {
      PlaySound (parse_error_sound, NULL, SND_NOWAIT | SND_ASYNC | SND_NODEFAULT) ;
      if (!running_demo && !demo_mode && !benchmark_mode)
        FeatureNotify ("ParserErrorSound",
                        "POV-Ray - Parse Error Sound",
                        "You can change the sound played upon parse errors "
                        "from the Render Menu.\n\n"
                        "Click Help for more information.",
                        "sounds", false) ;
    }
    buffer_message (mDivider, "\n") ;
    EditShowMessages (true) ;

    return (false) ;
  }

  if (!StartInsertRender)
  {
    if (MenuBarDraw)
    {
      DrawMenuBar (main_window) ;
      MenuBarDraw = false ;
    }
    bool show = EditShowMessages (true) ;
    CalculateClientWindows (true) ;
    if (show)
      ShowWindow (message_window, SW_SHOW) ;
    PutHKCU ("Info", "Rendering", 1) ;
    display_cleanup (false) ;
  }

  currentX = seconds_for_last_line = -1 ;
  message_printf ("Rendering with %d thread%s.\n", threadCount, threadCount > 1 ? "s" : "") ;

  if (GetRenderWindow())
    GetRenderWindow()->SetRenderState (true);

  ExternalEvent (EventStartRendering, 0) ;

  set_render_priority (render_priority) ;

  // FIXME - ought to wait for the render to start here
  rendering = true ;
  // StartProfile () ;

  return (true) ;
}

void render_stopped (void)
{
  char            *s ;
  char            str [4096] ;
  vfeWinSession&  Session (GetSession());

  run_renderer = false ;
  rendering = false ;
  status_buffer[0] = '\0' ;
  delay_next_status = 0 ;
  SetStatusPanelItemText (IDC_STATUS_DATA_FRAME, "N/A") ;
  SendMessage (StatusPanelItems [IDC_STATUS_PROGRESS].hwnd, PBM_SETPOS, 0, 0) ;
  bool success = Session.Succeeded();

  if (message_output_x > 0)
    buffer_message (mIDE, "\n") ;

  s = EditGetFilename(true) ;
  if (s != NULL && *s != '\0')
  {
    sprintf (str, "POV-Ray - %s", s) ;
    SetCaption (str) ;
  }
  else
    SetCaption ("POV-Ray for Windows") ;

  if ((!success || ErrorOccurred) && povray_return_code == 0)
    povray_return_code = -1 ;
  PrintRenderTimes (true, !ErrorOccurred && !stop_rendering) ;
  ExternalEvent (EventStopRendering, povray_return_code) ;

  // EndProfile () ;

  if (restore_command_line && !rendering_insert_menu)
  {
    strcpy (command_line, old_command_line) ;
    SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
    restore_command_line = false ;
  }

  if (rendering_insert_menu)
  {
    if (InsertMenuSection < InsertMenuSections.size() - 1 && !(stop_rendering || quit))
    {
      wrapped_printf ("Completed rendering Insert Menu section %d. result = %d", InsertMenuSections [InsertMenuSection++], povray_return_code) ;
      StartInsertRender = true ;
      return ;
    }
    StartInsertRender = rendering_insert_menu = no_status_output = false ;
    was_insert_render = true ;
  }
  else if (running_benchmark || running_demo)
  {
    if (running_benchmark == false)
    {
      strcpy (command_line, old_command_line) ;
      restore_command_line = false ;
    }
    _unlink (demo_file_name) ;
    _unlink (demo_ini_name) ;
    running_benchmark = running_demo = false ;
    if (benchmark_mode) // only applies to benchmarks started from the command-line
      PostQuitMessage (0) ;
    if (demo_mode)
    {
      PovMessageBox ("Demonstration completed. POV-Ray will now exit.", "Finished test run") ;
      PostQuitMessage (0) ;
    }
  }

  PutHKCU("Info", "Rendering", 0U) ;
  set_render_priority (CM_RENDERPRIORITY_NORMAL) ;

  if (quit != 0 || exit_after_render)
  {
    DestroyWindow (main_window) ;
    return ;
  }

  WinDisplay *rw = GetRenderWindow();
  if (rw != NULL)
  {
    rw->SetCaption ("POV-Ray Render Window");
    rw->SetRenderState (false);
    if (render_auto_close)
      rw->Hide();
  }
  if (main_window_hidden)
    TaskBarModifyIcon (main_window, 0, "POV-Ray (Restore: DblClk ; Menu: Mouse2)") ;
  InvalidateRect (statuspanel, NULL, false) ;

  if (success == true && ErrorOccurred == false)
  {
    if (render_complete_sound_enabled)
    {
      PlaySound (render_complete_sound, NULL, SND_ASYNC | SND_NODEFAULT) ;
      if (!running_demo && !demo_mode && !benchmark_mode)
        FeatureNotify ("RenderCompleteSound",
                      "POV-Ray - Render Complete Sound",
                      "You can change the sound played upon completion of rendering "
                      "from the Render Menu.\n\nIt is also possible to tell POV-Ray "
                      "for Windows to do other things when a render stops (such as "
                      "display a message or exit.)",
                      "sounds", false) ;
    }
    say_status_message (StatusMessage, "") ;
    EditShowMessages (false) ;
    CalculateClientWindows (false) ;
    switch (on_completion)
    {
      case CM_COMPLETION_EXIT :
           DestroyWindow (main_window) ;
           break ;

      case CM_COMPLETION_MESSAGE :
           PovMessageBox ("Render completed", "Message from POV-Ray for Windows") ;
           break ;
    }
    if (!running_demo && !demo_mode && !benchmark_mode && !rendering_insert_menu && !running_benchmark && !was_insert_render)
    {
      std::string ofn = UCS2toSysString (Session.GetOutputFilename());
      if (output_to_file && ofn.size () != 0)
      {
        sprintf (str, "Output -> '%s'", ofn.c_str ()) ;
        say_status_message (StatusMessage, str) ;
        //buffer_stream_message (mIDE, str) ;
        sprintf (str, "Your output file has been written to the following location:\n\n"
                      "  %s\n\n"
                      "Press F1 to learn more about how to control where files are written.",
                      ofn.c_str ()) ;
        FeatureNotify ("OutputFileLocation", "POV-Ray - Output File Notification", str, "Output_File_Name", false) ;
        PutHKCU("LastRender", "OutputFile", ofn.c_str ()) ;
      }
      else
      {
        if (!running_benchmark)
        {
            FeatureNotify ("OutputFileOff",
                           "POV-Ray - No Output File",
                           "A render has completed but file output was turned off. No file "
                           "was written.\n\nPress F1 for help on output file control.",
                           "Output_To_File",
                           false) ;
        }
      }
    }
  }
  else
  {
    if (ErrorNotified == false && ErrorMessage.empty() == false)
      say_status_message (StatusMessage, ErrorMessage.c_str()) ;
    else if (stop_rendering)
      say_status_message (StatusMessage, "Render cancelled by user") ;
    else
      say_status_message (StatusMessage, "Render failed") ;
    if (ErrorFilename.empty() == false)
      EditShowParseError (ErrorFilename.c_str(), ErrorMessage.c_str(), ErrorLine, ErrorCol) ;
    if (stop_rendering)
    {
      if (render_error_sound_enabled)
      {
        PlaySound (render_error_sound, NULL, SND_ASYNC | SND_NODEFAULT) ;
        if (!running_demo && !demo_mode && !benchmark_mode)
          FeatureNotify ("RenderErrorSound",
                        "POV-Ray - Render Stopped Sound",
                        "You can change the sound played upon render errors/cancellation "
                        "from the Render Menu.",
                        "sounds", false) ;
      }
    }
    else
    {
      if (parse_error_sound_enabled)
      {
        PlaySound (parse_error_sound, NULL, SND_NOWAIT | SND_ASYNC | SND_NODEFAULT) ;
        if (!running_demo && !demo_mode && !benchmark_mode)
          FeatureNotify ("ParserErrorSound",
                        "POV-Ray - Parse Error Sound",
                        "You can change the sound played upon parse errors "
                        "from the Render Menu.\n\n"
                        "Click Help for more information.",
                        "sounds", false) ;
      }
    }
  }

  // update the mapping of included files to source files
  const vfeWinSession::FilenameSet& rf = Session.GetReadFiles();
  for (vfeWinSession::FilenameSet::const_iterator it = rf.begin(); it != rf.end(); it++)
  {
    if (_stricmp(it->c_str(), InputFileName.c_str()) == 0)
      continue;
    if (is_non_primary_file(it->c_str()))
    {
      FileType ft = get_file_type(it->c_str());
      if (ft < fileFirstImageType || ft > fileLastImageType)
      {
        pair<map<std::string, std::string>::iterator, bool> result = IncludeToSourceMap.insert(pair<std::string, std::string> (*it, InputFileName));
        if (result.second == false)
          result.first->second = InputFileName;
      }
    }
  }
}

UINT WINAPI ofn_hook_fn (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG :
         SetupExplorerDialog (hwnd) ;
         break ;
  }
  return (false) ;
}

void init_ofn (OPENFILENAME *ofn, HWND hWnd, char *title, char *name, int maxlen, char *lastPath, char *defaultExt)
{
  ofn->lStructSize = sizeof (OPENFILENAME) ;
  ofn->hwndOwner = hWnd ;
  ofn->hInstance = hInstance ;
  ofn->lpstrCustomFilter = NULL ;
  ofn->nMaxCustFilter = 0 ;
  ofn->nFilterIndex = 1 ;
  ofn->lpstrTitle = title ;
  ofn->lpstrFile = name ;
  ofn->nMaxFile = maxlen ;
  ofn->lpstrFileTitle = NULL ;
  ofn->nMaxFileTitle = 0 ;
  ofn->lpstrInitialDir = lastPath ;
  ofn->Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR ;
  if (IsW95UserInterface)
    ofn->Flags |= OFN_EXPLORER ;
  ofn->nFileOffset = 0 ;
  ofn->nFileExtension = 0 ;
  ofn->lpstrDefExt = defaultExt ;
  ofn->lCustData = 0L ;
  ofn->lpfnHook = NULL ;
  ofn->lpTemplateName = NULL ;
}

char *file_open (HWND hWnd)
{
  int           result ;
  OPENFILENAME  ofnTemp ;
  static char   name [_MAX_PATH] ;

  strcpy (name, lastRenderName) ;
  validatePath (lastRenderPath) ;
  init_ofn (&ofnTemp, hWnd, "Render File", name, sizeof (name), lastRenderPath, "pov") ;
  ofnTemp.lpstrFilter = "POV source and INI (*.pov;*.ini)\0*.pov;*.ini\0POV files (*.pov)\0*.pov\0INI files (*.ini)\0*.ini\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != 0)
  {
    strcpy (lastRenderPath, name) ;
    // this removes the name AND the trailing '\' [which is what we want]
    lastRenderPath [ofnTemp.nFileOffset - 1] = '\0' ;
    validatePath (lastRenderPath) ;
    strcpy (lastRenderName, name + ofnTemp.nFileOffset) ;
  }
  return (result ? name : NULL) ;
}

char *get_background_file (HWND hWnd)
{
  int           result ;
  OPENFILENAME  ofnTemp ;
  static char   name [_MAX_PATH] ;

  strcpy (name, lastBitmapName) ;
  validatePath (lastBitmapPath) ;
  init_ofn (&ofnTemp, hWnd, "Tile Bitmap File", name, sizeof (name), lastBitmapPath, "bmp") ;
  ofnTemp.lpstrFilter = "BMP files (*.bmp)\0*.bmp\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != 0)
  {
    strcpy (lastBitmapPath, name) ;
    lastBitmapPath [ofnTemp.nFileOffset - 1] = '\0' ;
    strcpy (lastBitmapName, name + ofnTemp.nFileOffset) ;
  }
  return (result ? name : NULL) ;
}

void get_font (void)
{
  HDC         hdc ;
  HFONT       hfont ;
  HFONT       hfontOld ;
  LOGFONT     lf ;
  CHOOSEFONT  cf ;
  TEXTMETRIC  tm ;

  hdc = GetDC (message_window) ;
  memset(&cf, 0, sizeof (CHOOSEFONT)) ;
  cf.lStructSize = sizeof (CHOOSEFONT) ;
  cf.hwndOwner = main_window ;
  cf.lpLogFont = &lf ;
  cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_FORCEFONTEXIST | CF_INITTOLOGFONTSTRUCT ;
  cf.nFontType = SCREEN_FONTTYPE ;
  get_logfont (hdc, &lf) ;
  if (ChooseFont (&cf))
  {
    if ((hfont = CreateFontIndirect (&lf)) == NULL)
    {
      PovMessageBox ("Failed to create message font", "Cannot change to selected font") ;
      ReleaseDC (message_window, hdc) ;
      return ;
    }
    hfontOld = (HFONT)SelectObject (hdc, hfont) ;
    GetTextMetrics (hdc, &tm) ;
    message_xchar = tm.tmAveCharWidth ;
    message_ychar = tm.tmHeight + tm.tmExternalLeading ;
    SelectObject (hdc, hfontOld) ;
    DeleteObject (message_font) ;
    message_font = hfont ;
    PovInvalidateRect (message_window, NULL, true) ;
    message_font_size = -MulDiv (lf.lfHeight, 72, GetDeviceCaps (hdc, LOGPIXELSY)) ;
    message_font_weight = lf.lfWeight ;
    strncpy (message_font_name, lf.lfFaceName, sizeof (message_font_name) - 1) ;
  }
  ReleaseDC (message_window, hdc) ;
}

void DragFunction (HDROP handle)
{
  int         cFiles ;
  int         i ;
  char        szFile [_MAX_PATH] ;
  HDIB        hDIB ;
  bool        calc = 0 ;
  BITMAP      bm ;

  cFiles = DragQueryFile (handle, -1, NULL, 0) ;
  if (rendering)
    message_printf ("\n") ;
  for (i = 0 ; i < cFiles ; i++)
  {
    DragQueryFile (handle, i, szFile, sizeof (szFile)) ;
    switch (get_file_type (szFile))
    {
      case filePOV :
      case fileINI :
      case fileINC :
           if (!use_editors || !drop_to_editor)
             break ;
           if ((EditGetFlags () & EDIT_CAN_OPEN) == 0)
           {
             say_status_message (StatusMessage, "Cannot open dropped file - max editor count reached") ;
             message_printf ("Cannot open dropped file - max editor count reached\n") ;
           }
           else
             EditOpenFile (szFile) ;
           continue ;

      case fileBMP :
           if (screen_depth < 8)
           {
             PovMessageBox ("Tiled bitmaps not supported in this color depth", "File ignored") ;
             continue ;
           }
           if ((hDIB = LoadDIB (szFile)) != NULL)
           {
             strcpy (background_file, szFile) ;
             DeleteObject (hBmpBackground) ;
             hBmpBackground = DIBToBitmap (hDIB, hPalApp) ;
             DeleteObject (hDIB) ;
             GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
             background_width = bm.bmWidth ;
             background_height = bm.bmHeight ;
             tile_background = true ;
             PovInvalidateRect (message_window, NULL, true) ;
           }
           else
             PovMessageBox ("Failed to load bitmap file", "Error") ;
           continue ;

      default :
           if (!ExternalDragFunction (szFile, dfRealDrop))
           {
             if (!use_editors || !drop_to_editor)
             {
               say_status_message (StatusMessage, "Dropped file ignored (must be .POV, .INC, or .INI if destination is renderer)") ;
               wrapped_printf ("Dropped file '%s' ignored (must be .POV, .INC, or .INI if destination is renderer).", szFile) ;
             }
             else
             {
               if ((EditGetFlags () & EDIT_CAN_OPEN) == 0)
               {
                 say_status_message (StatusMessage, "Cannot open dropped file - max editor count reached") ;
                 message_printf ("Cannot open dropped file - max editor count reached\n") ;
               }
               else
                 EditOpenFile (szFile) ;
             }
           }
           continue ;
    }
    if (queued_file_count < MAX_QUEUE)
    {
      strcpy (queued_files [queued_file_count++], szFile) ;
      wrapped_printf ("File '%s' dropped as queue entry %d.", szFile, queued_file_count) ;
    }
    else
      wrapped_printf ("render queue full ; file '%s' ignored.", szFile) ;
  }
  if (rendering)
    message_printf ("\n") ;
  DragFinish (handle) ;
  update_queue_status (true) ;
  if (calc)
    CalculateClientWindows (true) ;
  FeatureNotify ("DropFiles",
                 "POV-Ray - Drag and Drop",
                 "POV-Ray can do one of several things when you drop files onto it, "
                 "depending on the state of the 'Drop to Editor' option and the type "
                 "of file dropped. For example if the file is a .POV or .INI you can "
                 "chose whether POV-Ray opens it or renders it.\n\nPress F1 for more "
                 "information.",
                 "drag and drop",
                 true) ;
}

void WIN_Debug_Log (unsigned int from, const char *msg)
{
  if (debugging)
    OutputDebugString (msg) ;
}

#if !POV_RAY_IS_OFFICIAL
void WIN_PrintOtherCredits (void)
{
  char        *s = DISTRIBUTION_MESSAGE_2 ;

  while (*s == ' ' || *s == '\t')
    s++ ;
  message_printf ("This is an UNSUPPORTED UNOFFICIAL COMPILE by %s.\n", s) ;
}
#endif

void PovMessageBox (const char *message, char *title)
{
  MessageBox (main_window, message, title, MB_ICONEXCLAMATION) ;
}

void detect_graphics_config (void)
{
  HDC   hdc ;

  hdc = GetDC (NULL) ;
  screen_depth = GetDeviceCaps (hdc, BITSPIXEL) ;
  render_bitmap_depth = (GetDeviceCaps (hdc, BITSPIXEL) > 8 && renderwin_8bits == 0) ? 24 : 8 ;
  screen_width = GetDeviceCaps (hdc, HORZRES) ;
  screen_height = GetDeviceCaps (hdc, VERTRES) ;
  if (GetSystemMetrics (SM_CMONITORS) > 1)
  {
    screen_origin_x = GetSystemMetrics (SM_XVIRTUALSCREEN) ;
    screen_origin_y = GetSystemMetrics (SM_YVIRTUALSCREEN) ;
    virtual_screen_width = GetSystemMetrics (SM_CXVIRTUALSCREEN) ;
    virtual_screen_height = GetSystemMetrics (SM_CYVIRTUALSCREEN) ;
  }
  else
  {
    screen_origin_x = screen_origin_y = 0 ;
    virtual_screen_width = screen_width ;
    virtual_screen_height = screen_height ;
  }
  ReleaseDC (NULL, hdc) ;
}

// Clear the system palette when we start to ensure an identity palette mapping
void clear_system_palette (void)
{
  int         Counter ;
  HDC         ScreenDC ;
  LogPal      Palette = { 0x300, 256 } ;
  HPALETTE    ScreenPalette ;

  // Reset everything in the system palette to black
  for (Counter = 0 ; Counter < 256 ; Counter++)
  {
    Palette.pe [Counter].peRed = 0 ;
    Palette.pe [Counter].peGreen = 0 ;
    Palette.pe [Counter].peBlue = 0 ;
    Palette.pe [Counter].peFlags = PC_NOCOLLAPSE ;
  }

  // Create, select, realize, deselect, and delete the palette
  ScreenDC = GetDC (NULL) ;
  ScreenPalette = CreatePalette ((LOGPALETTE *) &Palette) ;
  if (ScreenPalette)
  {
    ScreenPalette = SelectPalette (ScreenDC, ScreenPalette, false) ;
    RealizePalette (ScreenDC) ;
    ScreenPalette = SelectPalette (ScreenDC, ScreenPalette, false) ;
    DeleteObject (ScreenPalette) ;
  }
  ReleaseDC (NULL, ScreenDC) ;
}

void create_about_font (void)
{
  LOGFONT     lf ;

  HDC hdc = GetDC (NULL) ;
  memset (&lf, 0, sizeof (LOGFONT)) ;
  lf.lfHeight = -12 ;
  lf.lfWeight = FW_REGULAR ;
  lf.lfPitchAndFamily = VARIABLE_PITCH ;
  lf.lfCharSet = DEFAULT_CHARSET ;
  lf.lfQuality = PROOF_QUALITY ;
  strcpy (lf.lfFaceName, "Trebuchet MS") ;
  if ((about_font = CreateFontIndirect (&lf)) == NULL)
  {
    strcpy (lf.lfFaceName, "Arial") ;
    about_font = CreateFontIndirect (&lf) ;
  }
  ReleaseDC (NULL, hdc) ;
}

void CalculateClientWindows (bool redraw)
{
  RECT        rect ;

  GetClientRect (main_window, &rect) ;
  rect.bottom -= toolheight + statusheight ;
  if (!use_editors)
    MoveWindow (message_window, 0, toolheight, rect.right, rect.bottom, redraw) ;
  else
    SetEditorPosition (0, toolheight, rect.right, rect.bottom) ;
}

// handle a WM_CHAR message destined for the toolbar commandline edit control.
// return true if the message is to be discarded.
bool handle_toolbar_cmdline (UINT wParam, UINT lParam)
{
  if (wParam == VK_RETURN)
  {
    SendMessage (main_window, WM_COMMAND, CM_FILERENDER, 0) ;
    return (true) ;
  }
  if (wParam == VK_ESCAPE)
  {
    EditSetFocus () ;
    return (true) ;
  }
  if (wParam == 0x01) // ctrl-a
  {
    SendMessage(toolbar_cmdline, EM_SETSEL, 0, -1);
    return (true) ;
  }
  return (false) ;
}

void SetupStatusPanel (HWND hDlg)
{
  int                   max = 0 ;
  int                   ypos = 0;
  char                  str [256] ;
  RECT                  rect ;
  StatusPanelItem       item ;

  GetWindowRect (hDlg, &rect) ;
  int x = rect.left ;
  int y = rect.top ;
  int width = rect.right - rect.left + 1 ;
  int height = rect.bottom - rect.top + 1 ;
  for (int id = IDC_STATUS_LABEL_FIRST ; id <= IDC_STATUS_ID_LAST ; id++)
  {
    item.id = id ;
    item.hwnd = GetDlgItem (hDlg, id) ;
    if (item.hwnd == NULL)
      continue ;
    if (id <= IDC_STATUS_DATA_LAST)
    {
      GetWindowText (item.hwnd, str, sizeof (str)) ;
      HDC hdc = GetDC (item.hwnd) ;
      HFONT hFont = (HFONT) SendMessage (item.hwnd, WM_GETFONT, 0, 0) ;
      HFONT oldFont = (HFONT) SelectObject (hdc, hFont) ;
      GetTextExtentPoint32 (hdc, str, (int) strlen (str), &item.size) ;
      SelectObject (hdc, oldFont) ;
      ReleaseDC (GetDlgItem (hDlg, id), hdc) ;
      if (id <= IDC_STATUS_LABEL_LAST)
        if (item.size.cx > max)
          max = item.size.cx ;
    }
    else
    {
      GetClientRect (item.hwnd, &rect) ;
      item.size.cx = rect.right ;
      item.size.cy = rect.bottom ;
    }
    StatusPanelItems [id] = item ;
  }
  for (int id = IDC_STATUS_LABEL_FIRST; id <= IDC_STATUS_LABEL_LAST ; id++)
  {
    item = StatusPanelItems [id] ;
    if (item.hwnd == NULL)
      continue ;
    MoveWindow (item.hwnd, 2, ypos, max + 2, item.size.cy, false) ;
    item = StatusPanelItems [id + IDC_STATUS_DATA_FIRST] ;
    MoveWindow (item.hwnd, max + 7, ypos, width - max - 8, item.size.cy, false) ;
    if (id >= IDC_STATUS_DATA_FIRST)
      SetWindowText (item.hwnd, "") ;
    ypos += item.size.cy ;
  }
  item = StatusPanelItems [IDC_STATUS_PROGRESS] ;

  if (height - ypos > 6)
  {
      // there's at least six rows available, so we show the progress bar
      item.size.cy = height - ypos - 1;
      MoveWindow (item.hwnd, 2, ypos + 1, width - 2, item.size.cy, false) ;
  }
  else
  {
      // not enough room for progress bar: hide it
      ShowWindow(item.hwnd, SW_HIDE);
  }
}

void SetStatusPanelItemText (int id, LPCSTR format, ...)
{
  char                  str [2048] ;
  va_list               arg_ptr ;

  va_start (arg_ptr, format) ;
  vsprintf (str, format, arg_ptr) ;
  va_end (arg_ptr) ;
  SetWindowText (StatusPanelItems [id].hwnd, str) ;
}

/**************************************************************************************/
/*                                  WINDOW PROCEDURES                                 */
/**************************************************************************************/

/*
 * Return true if we are to return 0 to Windows, false if we are to continue.
 */
bool handle_main_command (WPARAM wParam, LPARAM lParam)
{
  int         n ;
  char        *s ;
  char        filename [_MAX_PATH] ;
  HDIB        hDIB ;
  RECT        rect ;
  BITMAP      bm ;
  HBITMAP     hBMP ;
  CHOOSECOLOR cc ;
  struct stat st ;
  static char str [8192] ;
  vfeWinSession& Session(GetSession());

  if (process_toggles (wParam))
    return (true) ;

  if (LOWORD (wParam) >= CM_FIRSTTOOL && LOWORD (wParam) <= CM_LASTTOOL)
  {
    s = parse_tool_command (tool_commands [LOWORD (wParam) - CM_FIRSTTOOL]) ;
    if (GetHKCU("General", "Debug", 0))
      message_printf ("Tool request - in '%s', out '%s'\n", tool_commands [LOWORD (wParam) - CM_FIRSTTOOL], s) ;
    else
      execute_tool (s) ;
    return (true) ;
  }

  if (LOWORD (wParam) >= CM_FIRSTMENUHELP && LOWORD (wParam) <= CM_LASTMENUHELP)
  {
    menuhelp (LOWORD (wParam)) ;
    return (true) ;
  }

  if (LOWORD (wParam) >= CM_DUTYCYCLE_10 && LOWORD (wParam) <= CM_DUTYCYCLE_100)
  {
    PVCheckMenuRadioItem (CM_DUTYCYCLE_10, CM_DUTYCYCLE_100, LOWORD (wParam)) ;
    Duty_Cycle = LOWORD (wParam) - CM_DUTYCYCLE_10 ;
    return (true) ;
  }

#ifdef RTR_SUPPORT
  if (LOWORD (wParam) >= CM_FIRSTVIDEOSOURCE && LOWORD (wParam) <= CM_LASTVIDEOSOURCE)
  {
    char          str [256] ;
    MENUITEMINFO  mi ;

    mi.cbSize = sizeof (MENUITEMINFO) ;
    mi.fMask = MIIM_TYPE ;
    mi.dwTypeData = str ;
    mi.cch = sizeof (str) ;
    if (GetMenuItemInfo(hVidcapMenu, LOWORD (wParam), false, &mi))
    {
      PVCheckMenuRadioItem (CM_FIRSTVIDEOSOURCE, CM_LASTVIDEOSOURCE, LOWORD (wParam)) ;
      status_printf (StatusMessage, "Selected video source '%s'", str) ;
      SetVideoSourceName(str);
    }
    return (true) ;
  }
#endif

  if (LOWORD (wParam) >= CM_FIRSTEDITNOTIFY && LOWORD (wParam) <= CM_LASTEDITNOTIFY)
  {
    switch (LOWORD (wParam) - CM_FIRSTEDITNOTIFY)
    {
      case NotifyTabChange :
           if ((lParam & EDIT_MSG_SELECTED) == 0)
           {
             build_editor_menu (hMainMenu) ;
             PVEnableMenuItem (CM_FILESAVE, (lParam & EDIT_CURRENT_MODIFIED) ? MF_ENABLED : MF_GRAYED) ;
             PVEnableMenuItem (CM_FILECLOSE, MF_ENABLED) ;
             s = EditGetFilename(true) ;
             if (s != NULL && *s != '\0')
             {
               sprintf (str, rendersleep ? "POV-Ray (paused) - %s" : "POV-Ray - %s", s) ;
               SetCaption (str) ;
             }
             else
               SetCaption (rendersleep ? "POV-Ray for Windows (paused)" : "POV-Ray for Windows") ;
           }
           else
           {
             build_main_menu (hMainMenu, true) ;
             PVEnableMenuItem (CM_FILESAVE, MF_GRAYED) ;
             PVEnableMenuItem (CM_FILECLOSE, MF_GRAYED) ;
             SetCaption (rendersleep ? "POV-Ray for Windows (paused)" : "POV-Ray for Windows") ;
           }
           break ;

      case NotifyModifiedChange :
           PVEnableMenuItem (CM_FILESAVE, lParam ? MF_ENABLED : MF_GRAYED) ;
           s = EditGetFilename(true) ;
           if (s != NULL && *s != '\0')
           {
             sprintf (str, rendersleep ? "POV-Ray (paused) - %s" : "POV-Ray - %s", s) ;
             SetCaption (str) ;
           }
           else
             SetCaption (rendersleep ? "POV-Ray for Windows (paused)" : "POV-Ray for Windows") ;
           break ;

      case NotifyFocusSaveModified :
           FeatureNotify ("FocusSaveModified",
                          "POV-Ray - Modified Files Auto-Saved",
                          "Your modified files were automatically saved when you switched to another "
                          "application. This is necessary for the Auto-Reload feature to work. If you "
                          "do not want this to happen, turn auto-reload off via the Editor menu.\n\n"
                          "Click Help for more information.",
                          "auto-reload", false) ;
           break ;

      case NotifyExitRequest :
           handle_main_command (CM_FILEEXIT, 0) ;
           break ;
    }
    return (true) ;
  }

  switch (LOWORD (wParam))
  {
    case CM_HIDENEWUSERHELP :
         set_newuser_menus (hide_newuser_help) ;
         return (0) ;

    case CM_IO_NO_RESTRICTIONS :
    case CM_IO_RESTRICT_WRITE :
    case CM_IO_RESTRICT_READWRITE :
         io_restrictions = LOWORD (wParam) - CM_IO_NO_RESTRICTIONS ;
         PVCheckMenuRadioItem (CM_IO_NO_RESTRICTIONS, CM_IO_RESTRICT_READWRITE, LOWORD (wParam)) ;
         PutHKCU ("Scripting", "IO Restrictions", io_restrictions) ;
         SetupFrontend();
         return (0) ;

    case CM_SHOWMAINWINDOW :
         if (main_window_hidden)
         {
           if (ListenMode)
           {
             char *s = "POV-Ray is in network listen mode. Press OK to continue listening or "
                       "CANCEL to shut down and exit." ;
             if (MessageBox (main_window, s, "POV-Ray - Network Listen Mode", MB_OKCANCEL) == IDOK)
               return (0) ;
             TaskBarDeleteIcon (main_window, 0) ;
             DestroyWindow (main_window) ;
             return (0) ;
           }
           ShowWindow (main_window, SW_SHOW) ;
           ShowWindow (main_window, SW_RESTORE) ;
           if (RenderwinIsChild == false && HideRenderWithMain == true)
             if (GetRenderWindow() != NULL)
               GetRenderWindow()->Show();
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Minimize to System &Tray\tAlt+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
         }
         else
         {
           if (use_taskbar)
           {
             char *s = "POV-Ray (Restore: DblClk ; Menu: Mouse Btn 2)" ;
             if (ListenMode)
               s = "POV-Ray: Network Listen Mode - DblClick for Options" ;
             if (TaskBarAddIcon (main_window, 0, ourIcon, s))
             {
               ShowWindow (main_window, SW_MINIMIZE) ;
               ShowWindow (main_window, SW_HIDE) ;
               if (RenderwinIsChild == false && HideRenderWithMain == true)
                 if (GetRenderWindow() != NULL)
                   GetRenderWindow()->Hide();
               PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Restore &Main Window from System Tray") ;
               main_window_hidden = true ;
             }
           }
         }
         return (0) ;

    case CM_FILENEW :
         EditOpenFile (NULL) ;
         return (0) ;

    case CM_FILEOPEN :
         EditBrowseFile (true) ;
         return (0) ;

    case CM_FILESAVE :
         EditSaveFile (NULL) ;
         return (0) ;

    case CM_FILECLOSE :
         EditCloseFile (NULL) ;
         return (0) ;

    case CM_RENDERSLEEP :
         if (Session.BackendFailed())
           break ;
         if (!Session.IsPausable())
           break ;
         if (Session.Paused())
         {
           if (Session.Resume())
           {
             SetCaption ("POV-Ray for Windows") ;
             FlashWindow (main_window, 0) ;
             SleepTimeEnd = clock () ;
             SleepTimeTotal += SleepTimeEnd - SleepTimeStart ;
             say_status_message (StatusPPS, "") ;
             rendersleep = false ;
             SendMessage (toolbar_window, TB_CHECKBUTTON, (WPARAM) CM_RENDERSLEEP, 0) ;
           }
           else
             SendMessage (toolbar_window, TB_CHECKBUTTON, (WPARAM) CM_RENDERSLEEP, MAKELONG (1, 0)) ;
         }
         else
           if (Session.Pause () == false)
             SendMessage (toolbar_window, TB_CHECKBUTTON, (WPARAM) CM_RENDERSLEEP, 0) ;
         break ;

    case CM_DROPEDITOR :
    case CM_DROPRENDERER :
         PVCheckMenuItem (CM_DROPEDITOR, LOWORD (wParam) == CM_DROPEDITOR ? MF_CHECKED : MF_UNCHECKED) ;
         PVCheckMenuItem (CM_DROPRENDERER, LOWORD (wParam) == CM_DROPRENDERER ? MF_CHECKED : MF_UNCHECKED) ;
         drop_to_editor = LOWORD (wParam) == CM_DROPEDITOR ;
         break ;

    case CM_RENDERPRIORITY_BACKGROUND :
    case CM_RENDERPRIORITY_LOW :
    case CM_RENDERPRIORITY_NORMAL :
    case CM_RENDERPRIORITY_HIGH :
         render_priority = LOWORD (wParam) ;
         PVCheckMenuRadioItem (CM_RENDERPRIORITY_BACKGROUND, CM_RENDERPRIORITY_HIGH, render_priority) ;
         // only change process priority when the renderer is running
         if (rendering)
           set_render_priority (render_priority) ;
         return (true) ;

    case CM_COMPLETION_EXIT :
    case CM_COMPLETION_NOTHING :
    case CM_COMPLETION_MESSAGE :
         PVCheckMenuItem (on_completion, MF_UNCHECKED) ;
         on_completion = LOWORD (wParam) ;
         PVCheckMenuItem (on_completion, MF_CHECKED) ;
         return (true) ;

    case CM_PREVWINDOW :
         EditNextTab (false) ;
         return (true) ;

    case CM_NEXTWINDOW :
         EditNextTab (true) ;
         return (true) ;

    case CM_USETOOLBAR :
         if (rebar_window == NULL)
           return (true) ;
         ShowWindow (rebar_window, use_toolbar ? SW_SHOW : SW_HIDE) ;
         // this seems to be needed to get the rebar to redraw properly with v4.72 of comctrl32.dll.
         InvalidateRect (main_window, NULL, true) ;
         toolheight = 0 ;
         GetClientRect (main_window, &rect) ;
         SendMessage (main_window, WM_SIZE, SIZE_RESTORED, MAKELPARAM (rect.right, rect.bottom)) ;
         return (true) ;

    case CM_SINGLEINSTANCE :
         PutHKCU ("General", "OneInstance", one_instance) ;
         return (true) ;

    case CM_FILEEXIT :
         if (rendering)
         {
           if (MessageBox (main_window,
                           "POV-Ray is currently rendering - do you want to stop ?",
                           "Stop rendering ?",
                           MB_ICONQUESTION | MB_YESNO) == IDYES)
           {
             if (!EditCanClose (true))
               return (true) ;
             if (!quit)
               quit = time (NULL) ;
             cancel_render () ;
           }
         }
         else
         {
           if (!EditCanClose (true))
             return (true) ;
           DestroyWindow (main_window) ;
         }
         return (true) ;

    case CM_FILERENDER :
    case CM_STOPRENDER :
         if (Session.BackendFailed())
           return (true) ;
         if (!rendering)
         {
           if (EditSaveModified (NULL) == 0)
             return (true) ;
           // EDIT_MSG_SELECTED is only ever set if use_editors == true
           if ((EditGetFlags () & EDIT_MSG_SELECTED) == 0)
           {
             if ((s = EditGetFilename(false)) == NULL)
             {
               PovMessageBox ("No file to render in current editor tab!", "Cannot render") ;
               return (true) ;
             }
             n = get_file_type (s) ;
             if (n == filePOV || n == fileINI || !ExternalDragFunction (s, dfRenderEditor))
               PostMessage (main_window, EDITOR_RENDER_MESSAGE, 0, (LPARAM) s) ;
             return (true) ;
           }
           SetForegroundWindow (main_window) ;
           if (!ExternalDragFunction (source_file_name, dfRenderMessage))
             start_rendering (false) ;
         }
         else
         {
           if (Session.GetBackendState () >= kStopping)
             return (true) ;
           if (OkToStopRendering ())
           {
             if (rendersleep)
             {
               SleepTimeEnd = clock () ;
               SleepTimeTotal += SleepTimeEnd - SleepTimeStart ;
               rendersleep = false ;
             }
             stop_rendering = true ;
             cancel_render () ;
           }
         }
         return (true) ;

    case CM_SAVE_SETTINGS :
         PutHKCU ("General", "SaveSettingsOnExit", save_settings) ;
         return (true) ;

    case CM_DUMPPANE :
         dump_pane_to_clipboard () ;
         return (true) ;

    case CM_CLEARMESSAGES :
         clear_messages () ;
         PovInvalidateRect (message_window, NULL, false) ;
         UpdateWindow (message_window) ;
         return (true) ;

    case CM_FORCE8BITS :
         detect_graphics_config () ;
         if (hPalApp)
           DeleteObject (hPalApp) ;
         hPalApp = WinLegacyDisplay::CreatePalette (NULL, 0, render_bitmap_depth != 24) ;
         buffer_message (mIDE, render_bitmap_depth == 24 ? "Using 24-bit internal bitmap\n" :
                                                           renderwin_8bits ? "Using 8-bit dithered internal bitmap (menu setting)\n" :
                                                                             "Using 8-bit dithered internal bitmap (4 or 8-bit video mode)\n") ;
         return (true) ;

    case CM_RENDERABOVEMAIN :
         // TODO FIXME CJC
#if 0
         // simply re-parenting doesn't seem to have the desired effect. sigh.
         WinDisplay *rw = GetRenderWindow();
         if (rw != NULL)
         {
           oldHwnd = render_window ;
           render_window = NULL ;
           ShowWindow (oldHwnd, SW_HIDE) ;
           SetForegroundWindow (main_window) ;
           DestroyWindow (oldHwnd) ;
           renderwin_manually_closed = false ;
           create_render_window (true) ;
           ShowWindow (render_window, SW_SHOW) ;
         }
#endif
         PVEnableMenuItem (CM_RENDERHIDE, RenderwinIsChild ? MF_GRAYED : MF_ENABLED) ;
         PVEnableMenuItem (CM_RENDERACTIVE, RenderwinIsChild ? MF_GRAYED : MF_ENABLED) ;
         return (true) ;

    case CM_USEEDITOR :
         editors_enabled = !editors_enabled;
         PutHKCU ("General", "UseEditors", editors_enabled) ;
         PVCheckMenuItem (CM_USEEDITOR, editors_enabled ? MF_CHECKED : MF_UNCHECKED) ;
         if (editors_enabled == use_editors)
           return true;
         if (MessageBox (main_window,
                        "POV-Ray for Windows needs to re-start for this to take effect immediately.\n\n"
                        "Re-start POV-Ray ?",
                        "Re-start POV-Ray for Windows ?",
                        MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
         {
           GetModuleFileName (hInstance, filename, sizeof (filename) - 1) ;
           if (save_settings)
           {
             SendMessage (toolbar_combobox,
                          CB_GETLBTEXT,
                          SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0),
                          (LPARAM) SecondaryRenderIniFileSection) ;
             if (restore_command_line)
             {
               strcpy (command_line, old_command_line) ;
               SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
               restore_command_line = false ;
             }
             write_INI_settings () ;
             EditSaveState () ;
           }
           DestroyWindow (main_window) ;
           execute_tool (filename) ;
         }
         return (true) ;

    case CM_HELPABOUT :
         ShowAboutBox () ;
         return (true) ;

    case CM_TOOLBARCMDLINE :
         if (!rendering)
         {
           SetFocus (toolbar_cmdline) ;
           SendMessage (toolbar_cmdline, EM_SETSEL, 0, -1) ;
         }
         return (true) ;

    case CM_RENDERINSERT :
         if (!rendering)
           RenderInsertMenu () ;
         return (true) ;

    case CM_COMMANDLINE :
         if (!rendering)
         {
           if (DialogBoxParam (hInstance,
                               MAKEINTRESOURCE (IDD_COMMANDLINE),
                               main_window,
                               (DLGPROC) PovCommandLineDialogProc,
                               (LPARAM) main_window))
           {
             if (!ExternalDragFunction (source_file_name, dfRenderCommandLine))
               start_rendering (false) ;
           }
         }
         return (true) ;

    case CM_TILEDBACKGROUND :
         PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, tile_background ? "&Use Plain Background" : "&Use Tiled Background") ;
         if (tile_background && hBmpBackground == NULL)
         {
           if ((hBmpBackground = NonBogusLoadBitmap (hInstance, MAKEINTRESOURCE (BMP_BACKGROUND00))) != NULL)
           {
             GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
             background_width = bm.bmWidth ;
             background_height = bm.bmHeight ;
             tile_background = true ;
             PovInvalidateRect (message_window, NULL, true) ;
           }
           else
           {
             tile_background = false ;
             // make sure this messagebox is AFTER we set tile_background to false !
             PovMessageBox ("Failed to load internal bitmap", "Error") ;
             PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, "&Select Tiled Background") ;
             background_file [0] = '\0' ;
           }
           return (true) ;
         }
         else
           PovInvalidateRect (message_window, NULL, true) ;
         return (true) ;

    case CM_BACKGROUNDCOLOUR :
         memset (&cc, 0, sizeof (CHOOSECOLOR)) ;
         cc.lStructSize = sizeof (CHOOSECOLOR) ;
         cc.hwndOwner = main_window ;
         cc.rgbResult = background_colour ;
         cc.Flags = CC_RGBINIT | CC_FULLOPEN ;
         cc.lpCustColors = custom_colours ;
         if (ChooseColor (&cc))
         {
           background_colour = cc.rgbResult ;
           PovInvalidateRect (message_window, NULL, true) ;
         }
         return (true) ;

    case CM_BACKGROUNDBITMAP :
         if ((s = get_background_file (main_window)) != NULL)
         {
           if ((hDIB = LoadDIB (s)) != NULL)
           {
             strcpy (background_file, s) ;
             DeleteObject (hBmpBackground) ;
             hBmpBackground = DIBToBitmap (hDIB, hPalApp) ;
             DeleteObject (hDIB) ;
             GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
             background_width = bm.bmWidth ;
             background_height = bm.bmHeight ;
             tile_background = true ;
             PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, "&Select Plain Background") ;
             background_shade = RGB (1, 1, 1) ;
             PovInvalidateRect (message_window, NULL, true) ;
           }
           else
             PovMessageBox ("Failed to load bitmap file", "Error") ;
         }
         return (true) ;

    case CM_BACKGROUNDSTD + 0 :
    case CM_BACKGROUNDSTD + 1 :
    case CM_BACKGROUNDSTD + 2 :
    case CM_BACKGROUNDSTD + 3 :
    case CM_BACKGROUNDSTD + 4 :
    case CM_BACKGROUNDSTD + 5 :
    case CM_BACKGROUNDSTD + 6 :
    case CM_BACKGROUNDSTD + 7 :
    case CM_BACKGROUNDSTD + 8 :
    case CM_BACKGROUNDSTD + 9 :
         if ((hBMP = NonBogusLoadBitmap (hInstance, MAKEINTRESOURCE (BMP_BACKGROUND00 + (LOWORD (wParam) - CM_BACKGROUNDSTD)))) != NULL)
         {
           DeleteObject (hBmpBackground) ;
           hBmpBackground = hBMP ;
           if (GetObject (hBmpBackground, sizeof (BITMAP), (LPVOID) &bm) == 0)
           {
             PovMessageBox ("Failed to load internal bitmap", "Error") ;
             tile_background = false ;
             return (true) ;
           }
           background_width = bm.bmWidth ;
           background_height = bm.bmHeight ;
           background_file [0] = '0' + (char) (LOWORD (wParam) - CM_BACKGROUNDSTD) ;
           background_file [1] = '\0' ;
           switch (LOWORD (wParam))
           {
             case CM_BACKGROUNDSTD + 0 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colours[0] = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 1 :
                  background_shade = RGB (0, 0, 0) ;
                  if (lParam != 1)
                    text_colours[0] = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 2 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colours[0] = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 3 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colours[0] = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 4 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colours[0] = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 5 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colours[0] = RGB (0, 0, 0) ;
                  break ;
           }
           tile_background = true ;
           PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, "&Select Plain Background") ;
           PovInvalidateRect (message_window, NULL, true) ;
         }
         else
           PovMessageBox ("Failed to load internal bitmap", "Error") ;
         return (true) ;

    case CM_TEXTCOLOUR_NORMAL :
    case CM_TEXTCOLOUR_WARNING :
    case CM_TEXTCOLOUR_ERROR :
         memset (&cc, 0, sizeof (CHOOSECOLOR)) ;
         cc.lStructSize = sizeof (CHOOSECOLOR) ;
         cc.hwndOwner = main_window ;
         cc.rgbResult = text_colours[LOWORD (wParam) - CM_TEXTCOLOUR_FIRST] ;
         cc.Flags = CC_RGBINIT | CC_FULLOPEN ;
         cc.lpCustColors = custom_colours ;
         if (ChooseColor (&cc))
         {
           text_colours[LOWORD (wParam) - CM_TEXTCOLOUR_FIRST] = cc.rgbResult ;
           PovInvalidateRect (message_window, NULL, true) ;
         }
         return (true) ;

    case CM_FONT :
         get_font () ;
         return (true) ;

    case CM_RENDERSHOW :
         if (GetRenderWindow() != NULL)
         {
           GetRenderWindow()->Show();
           PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
           PVEnableMenuItem (CM_RENDERCLOSE, MF_ENABLED) ;
         }
         return (true) ;

    case CM_RENDERCLOSE :
         if (GetRenderWindow() != NULL)
         {
           if (GetForegroundWindow () == GetRenderwinHandle())
             if (RenderwinIsChild)
               SetForegroundWindow (main_window) ;
           GetRenderWindow()->Hide();
           PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
           PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
         }
         return (true) ;

    case CM_RENDERDESTROY :
         if (GetForegroundWindow () == (HWND) lParam)
           SetForegroundWindow (main_window) ;
         DestroyWindow ((HWND) lParam) ;
         return (true) ;

    case CM_CLEARQUEUE :
         queued_file_count = 0 ;
         update_queue_status (true) ;
         return (true) ;

    case CM_FILEQUEUE :
         DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_FILEQUEUE), main_window, (DLGPROC) PovFileQueueDialogProc, (LPARAM) main_window) ;
         return (true) ;

    case CM_SOURCEFILE :
         if (!rendering)
         {
           if ((s = file_open (main_window)) != NULL)
           {
             strcpy (source_file_name, s) ;
             splitpath (source_file_name, lastRenderPath, lastRenderName) ;
             validatePath (lastRenderPath) ;
             if (!ExternalDragFunction (source_file_name, dfRenderSourceFile))
               start_rendering (false) ;
           }
         }
         return (true) ;

    case CM_RENDERSOUNDS :
         hh_aklink.pszKeywords = "sounds" ;
         DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SOUNDS), main_window, (DLGPROC) PovSoundsDialogProc, (LPARAM) main_window) ;
         return (true) ;

    case CM_RENDERTHREADCOUNT :
         hh_aklink.pszKeywords = "thread count" ; // TODO
         DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_THREADS), main_window, (DLGPROC) PovThreadCountDialogProc, (LPARAM) main_window) ;
         return (true) ;

    case CM_DEMO :
         if (!rendering && !running_demo)
         {
           if (save_demo_file (demo_file_name, demo_ini_name) != NULL)
           {
             if (!demo_mode)
             {
               running_demo = true ;
               if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SHORTCOMMANDLINE), main_window, (DLGPROC) PovShortCommandLineDialogProc, (LPARAM) main_window))
               {
                 if (!restore_command_line)
                   strcpy (old_command_line, command_line) ;
                 restore_command_line = true ;
                 sprintf (command_line, "Include_Ini='%s' Input_File_Name='%s' ", demo_ini_name, demo_file_name) ;
                 _strupr (command_line) ;
                 strcat (command_line, old_command_line) ;
                 ignore_auto_ini = true ;
                 start_rendering (true) ;
               }
             }
             else
             {
               if (!restore_command_line)
                 strcpy (old_command_line, command_line) ;
               restore_command_line = true ;
               sprintf (command_line, "Include_Ini='%s' Input_File_Name='%s' ", demo_ini_name, demo_file_name) ;
               running_demo = true ;
               start_rendering (true) ;
             }
           }
         }
         return (true) ;

    case CM_BENCHMARK :
    case CM_BENCHMARKONETHREAD :
         if (rendering)
           return (0) ;
         if (!benchmark_mode)
         {
           n = Get_Benchmark_Version () ;
           hh_aklink.pszKeywords = "Run Benchmark" ;
           sprintf (str, "This command will run the standard POV-Ray benchmark version %x.%02x.\n", n / 256, n % 256) ;
           strcat (str, "This will take some time, and there will be no display or file output.\n\n") ;
           strcat (str, "Note that the benchmark has changed since v3.6 and cannot be compared with it.\n\n") ;
           if (LOWORD (wParam) == CM_BENCHMARK)
           {
             sprintf(str + strlen(str), "The benchmark will be rendered using %d threads. Continue?", ThreadCount) ;
             benchmark_multithread = true ;
           }
           else
           {
             strcat (str, "The benchmark will be rendered using a single thread. Continue?") ;
             benchmark_multithread = false ;
           }
           if (MessageBox (main_window, str, "Standard Benchmark", MB_YESNO | MB_ICONINFORMATION | MB_HELP) == IDNO)
             return (0) ;
         }
         GetTempPath (_MAX_PATH - 16, demo_file_name) ;
         appendPathSeparator(demo_file_name) ;
         strcpy (demo_ini_name, demo_file_name) ;
         strcat (demo_file_name, "POVBENCH.$$1") ;
         strcat (demo_ini_name, "POVBENCH.$$2") ;
         if (Write_Benchmark_File (demo_file_name, demo_ini_name))
         {
           running_benchmark = running_demo = true ;
           int threadCount = benchmark_mode || benchmark_multithread ? ThreadCount : 1 ;
           message_printf ("Running standard POV-Ray benchmark version %x.%02x using %d thread%s\n", n / 256, n % 256, threadCount, threadCount > 1 ? "s" : "") ;
           buffer_message (mDivider, "\n") ;
           status_printf (0, "Running standard POV-Ray benchmark version %x.%02x\n", n / 256, n % 256) ;
           start_rendering (true) ;
         }
         else
           PovMessageBox ("Failed to write temporary files", "Benchmark Failed") ;
         return (true) ;

    case CM_LOADTOOLMENU :
         ExternalEvent (EventLoadToolMenu, 0) ;
         load_tool_menu (ToolIniFileName) ;
         break ;

    case CM_HELPLOOKUP :
         if (GetFocus () == GetRenderwinHandle ())
         {
           hh_aklink.pszKeywords = "Render Window" ;
           HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
           return (true) ;
         }
         if (GetFocus () == toolbar_cmdline)
         {
           hh_aklink.pszKeywords = "Toolbar Command Line" ;
           HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
           return (true) ;
         }
         if (GetFocus () == toolbar_combobox)
         {
           hh_aklink.pszKeywords = "Preset Rendering Options" ;
           HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
           return (true) ;
         }
         if ((EditGetFlags () & EDIT_MSG_SELECTED) != 0)
           HtmlHelp (NULL, engineHelpPath, HH_DISPLAY_TOC, 0) ;
         else
           EditContextHelp () ;
         return (true) ;

    case CM_HELPPOVWIN :
         hh_aklink.pszKeywords = "welcome" ;
         HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
         return (true) ;

    case CM_HELPSCENE :
         hh_aklink.pszKeywords = "Scene Description Language" ;
         HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
         return (true) ;

    case CM_GOPOVRAYORG :
         ShellExecute (NULL, NULL, "http://www.povray.org/", NULL, NULL, SW_SHOWNORMAL) ;
         return (true) ;

    case CM_HELPBUGS :
         hh_aklink.pszKeywords = "Bug Reports" ;
         HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
         return (true) ;

    case CM_POVLEGAL :
         if (stat (engineHelpPath, &st) == 0)
         {
           hh_aklink.pszKeywords = "POV-Ray License" ;
           if (HtmlHelp (main_window, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink))
             return (true) ;
         }
         DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_VIEW), main_window, (DLGPROC) PovLegalDialogProc, (LPARAM) main_window) ;
         return (true) ;

    case CM_CHECKUPDATENOW:
#if POV_RAY_HAS_UPDATE_CHECK
         ManualUpdateCheck();
#endif
         return true;
   }
   return (false) ;
}

LRESULT CALLBACK PovMainWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   i ;
  char                  *s ;
  char                  str [4096] ;
  bool                  f ;
  HDC                   hdc ;
  RECT                  rect ;
  POINT                 pt ;
  NMHDR                 *nmh ;
  DWORD                 result = 0 ;
  HPALETTE              oldPalette ;
  TOOLTIPTEXT           *t  ;
  COPYDATASTRUCT        *cd ;
  vfeSession&           Session (GetSession());

  PROCESS_MEMORY_COUNTERS memInfo;

  switch (message)
  {
    case WM_COPYDATA :
         cd = (COPYDATASTRUCT *) lParam ;
         if (cd->dwData == EDIT_FILE)
         {
           strncpy (str, (char *) cd->lpData, sizeof (str) - 1) ;
           str [sizeof (str) - 1] = '\0' ;
           if (EditGetFlags () & EDIT_CAN_OPEN)
             EditOpenFile (str) ;
           return (0) ;
         }
         if (cd->dwData == RENDER_FILE)
         {
           strncpy (str, (char *) cd->lpData, sizeof (str) - 1) ;
           str [sizeof (str) - 1] = '\0' ;
           if (rendering)
             return (0) ;
           strcpy (source_file_name, str) ;
           start_rendering (false) ;
           return (0) ;
         }
         return (1) ;

    case COPY_COMMANDLINE_MESSAGE :
         command_line [sizeof (command_line) - 1] = '\0' ;
         strncpy (command_line, (LPCSTR) lParam, sizeof (command_line) - 1) ;
         SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
         SendMessage (toolbar_cmdline, EM_SETSEL, 0, strlen (command_line)) ;
         SetFocus (toolbar_cmdline) ;
         return (true) ;

    case WM_HELP :
         // we expect that whatever routine caused the WM_HELP would have set up the keyword
         HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
         return (true) ;

    case KEYWORD_LOOKUP_MESSAGE :
         hh_aklink.pszKeywords = (LPCSTR) lParam ;
         if (strncmp (hh_aklink.pszKeywords, "oooo", 4) == 0)
           hh_aklink.pszKeywords = ""  ;
         if (strncmp (hh_aklink.pszKeywords, "//", 2) == 0)
           hh_aklink.pszKeywords = ""  ;
         HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
         return (true) ;

    case TASKBAR_NOTIFY_MESSAGE :
         if (lParam == WM_LBUTTONDBLCLK)
         {
           if (ListenMode)
           {
             char *s = "POV-Ray is in network listen mode. Press OK to continue listening or "
                       "CANCEL to shut down and exit." ;
             if (MessageBox (main_window, s, "POV-Ray - Network Listen Mode", MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK)
               return (0) ;
             TaskBarDeleteIcon (main_window, 0) ;
             DestroyWindow (main_window) ;
             return (0) ;
           }
           ShowWindow (main_window, SW_SHOW) ;
           ShowWindow (main_window, SW_RESTORE) ;
           if (RenderwinIsChild == false && HideRenderWithMain == true)
             if (GetRenderWindow() != NULL)
               GetRenderWindow()->Show();
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Minimize to System &Tray\tAlt+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
           return (0) ;
         }
         if (lParam == WM_RBUTTONDOWN)
         {
           if (ListenMode)
             return (0) ;
           if (hPopupMenus != NULL)
           {
             GetCursorPos (&pt) ;
             SetForegroundWindow (main_window) ;
             TrackPopupMenu (GetSubMenu (hPopupMenus, 0), TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, main_window, NULL) ;
             return (0) ;
           }
         }
         return (0) ;

    case WM_ENTERSIZEMOVE :
         if (!IsW95UserInterface)
           break ;
         resizing = true ;
         break ;

    case WM_EXITSIZEMOVE :
         if (!IsW95UserInterface)
           break ;
         resizing = false ;
         InvalidateRect (message_window, NULL, true) ;
         break ;

    case WM_SETFOCUS :
         // After a dialog has been displayed, Windows will give the focus
         // back to our main window. We need to farm the focus off to whatever
         // window should have it. EditSetFocus () will handle this for us.
         EditSetFocus () ;
         return (0) ;

    case EDITOR_RENDER_MESSAGE :
         if (rendering)
         {
           if (OkToStopRendering ())
           {
             stop_rendering = true ;
             cancel_render () ;
           }
           return (0) ;
         }
         strcpy (source_file_name, (char *) lParam) ;
         if (is_non_primary_file(source_file_name))
         {
           char fn[_MAX_PATH + 1];

           strcpy(fn, source_file_name);
           _strlwr(fn);

           // see if we have a previous decision recorded for this file.
           map<std::string, bool>::const_iterator altit = IncludeAlternateDecisionMap.find(fn);
           if (altit == IncludeAlternateDecisionMap.end() || altit->second == true)
           {
             // either there is no decision recorded, or the decision was 'yes'
             // in either case we need to find the alternate filename
             map<std::string, std::string>::const_iterator it = IncludeToSourceMap.find(fn);
             if (it != IncludeToSourceMap.end())
             {
               // we've found the alternate filename. do we need to ask the user about it?
               if (altit == IncludeAlternateDecisionMap.end())
               {
                 // yes we do.
                 switch (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_RENDERALTERNATEFILE), main_window, (DLGPROC) RenderAlternativeFileDialogProc, (LPARAM) it->second.c_str()))
                 {
                   case IDYES:
                     // record the decision
                     IncludeAlternateDecisionMap.insert(pair<std::string, bool>(fn, true));
                     strcpy(source_file_name, it->second.c_str());
                     break;

                   case IDOK:
                     // we don't record this decision
                     strcpy(source_file_name, it->second.c_str());
                     break;

                   case IDNO:
                     // record the decision
                     IncludeAlternateDecisionMap.insert(pair<std::string, bool>(fn, false));
                     break;

                   case IDCANCEL:
                     return (0) ;
                 }
               }
               else
                 strcpy(source_file_name, it->second.c_str());
             }
           }
         }
         splitpath (source_file_name, lastRenderPath, lastRenderName) ;
         if (!ExternalDragFunction (source_file_name, dfRenderEditor))
           start_rendering (false) ;
         return (0) ;

    case CREATE_RENDERWIN_MESSAGE :
         if (dynamic_cast<WinDisplay *>(reinterpret_cast<WinDisplay *>(lParam)) != NULL)
           return (dynamic_cast<WinDisplay *>(reinterpret_cast<WinDisplay *>(lParam))->CreateRenderWindow());
         return (0) ;

    case GUIEXT_CREATE_EDITOR :
         if (wParam == 0)
         {
           message_printf ("External application or GUIEXT sent zero-length wParam for GUIEXT_CREATE_EDITOR\n") ;
           return (0) ;
         }
         if (IsBadReadPtr ((void *) lParam, wParam + 1))
         {
           message_printf ("External application or GUIEXT sent bad paramstr address for GUIEXT_CREATE_EDITOR\n") ;
           return (0) ;
         }
         if (((char *) lParam) [wParam] != '\0')
         {
           message_printf ("External application or GUIEXT sent non-NULL terminated paramstr for GUIEXT_CREATE_EDITOR\n") ;
           return (0) ;
         }
         return (EditExternalOpenFile ((char *) lParam)) ;

    case WM_NOTIFY :
         nmh = (NMHDR *) lParam ;
         if (nmh->hwndFrom == tab_window)
         {
           EditPassOnMessage (hwnd, message, wParam, lParam, &result) ;
           break ;
         }
         if (nmh->hwndFrom == rebar_window)
         {
           switch (nmh->code)
           {
             case RBN_HEIGHTCHANGE :
                  if (!use_toolbar)
                    break ;
                  GetClientRect (rebar_window, &rect) ;

                  // under XP with comctrl v6 it has been noticed that this event will occur
                  // when the main window is minimized, before the parent's WM_SIZE message
                  // is received, and that the return value from GetClientRect () seems to
                  // be rather strange (e.g. 0,0,202,0 where the actual height is about 75.)
                  if (rect.right == rect.left)
                    break ;

                  toolheight = rect.bottom ;
                  CalculateClientWindows (true) ;

                  // need this due to an issue with Windows 95
                  if (top_message_row)
                  {
                    ShowScrollBar (message_window, SB_VERT, false) ;
                    ShowScrollBar (message_window, SB_VERT, true) ;
                  }
                  if (need_hscroll ())
                  {
                    ShowScrollBar (message_window, SB_HORZ, false) ;
                    ShowScrollBar (message_window, SB_HORZ, true) ;
                  }
                  break ;
           }
           break ;
         }
         if (nmh->hwndFrom == StatusTooltip)
         {
           if (HandleStatusTooltip(nmh))
             return 0;
           break;
         }
         switch (nmh->code)
         {
           case TTN_NEEDTEXT :
                t = (TOOLTIPTEXT *) lParam ;
                if (use_tooltips == 0)
                {
                  t->lpszText = NULL ;
                  t->hinst = 0 ;
                  break ;
                }
                t->hinst = hInstance ;
                t->lpszText = MAKEINTRESOURCE (t->hdr.idFrom) ;
                return (0) ;
         }
         break ;

    case RENDER_MESSAGE :
         s = getCommandLine () ;
         if (rendering && (strlen (s) || wParam))
         {
           PovMessageBox ("Cannot accept new command - already rendering", "Warning") ;
           return (0) ;
         }
         if (main_window_hidden)
         {
           ShowWindow (main_window, SW_SHOW) ;
           ShowWindow (main_window, SW_RESTORE) ;
           if (RenderwinIsChild == false && HideRenderWithMain == true)
             if (GetRenderWindow() != NULL)
               GetRenderWindow()->Show();
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Minimize to System &Tray\tAlt+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
           return (0) ;
         }
         if (wParam == 0)
         {
           if (strlen (s) == 0)
             return (0) ;
           if (!restore_command_line)
             strcpy (old_command_line, command_line) ;
           restore_command_line = true ;
           strcpy (command_line, s) ;
           start_rendering (true) ;
           strcpy (command_line, old_command_line) ;
           SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
           restore_command_line = false ;
         }
         else
           handle_main_command (CM_DEMO, 0) ;
         return (0) ;

    case WM_CREATE :
         main_window = hwnd ;
         hMainMenu = CreateMenu () ;
         build_main_menu (hMainMenu, false) ;
         SetMenu (main_window, hMainMenu) ;
         break ;

    case WM_QUERYENDSESSION :
         if (rendering)
         {
           if (MessageBox (main_window, "POV-Ray is currently rendering - do you want to stop ?", "Stop rendering ?", MB_ICONQUESTION | MB_YESNO) != IDYES)
             return (false) ;
           if (!EditCanClose (true))
             return (false) ;
           if (!quit)
             quit = time (NULL) ;
           cancel_render () ;
           return (true) ;
         }
         return (EditCanClose (true)) ;

    case WM_ENDSESSION :
         if (wParam != 0)
         {
           setRunOnce () ;
           if (save_settings)
           {
             SendMessage (toolbar_combobox, CB_GETLBTEXT, SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0), (LPARAM) SecondaryRenderIniFileSection) ;
             if (restore_command_line)
             {
               strcpy (command_line, old_command_line) ;
               SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
               restore_command_line = false ;
             }
             write_INI_settings () ;
             EditSaveState () ;
           }
         }
         break ;

    case WM_COMMAND :
         if ((HANDLE) lParam == toolbar_cmdline)
         {
           // need to use EN_CHANGE rather than EN_KILLFOCUS as the command-line dialog
           // will grab and possibly modify command_line before focus is lost, should it
           // be activated while the edit control has focus.
           if (HIWORD (wParam) == EN_CHANGE)
           {
             SendMessage (toolbar_cmdline, WM_GETTEXT, sizeof (command_line) - 1, (LPARAM) command_line) ;
             return (0) ;
           }
         }
         if ((HANDLE) lParam == toolbar_combobox)
         {
           if (HIWORD (wParam) == CBN_CLOSEUP)
           {
             cb_expect_selchange++ ;
             return (0) ;
           }
           if (HIWORD (wParam) == CBN_SELCHANGE)
           {
             i = SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0) ;
             if (i == SendMessage (toolbar_combobox, CB_GETCOUNT, 0, 0) - 1)
             {
               SendMessage (toolbar_combobox, CB_SETCURSEL, tb_combo_sel, 0) ;
               if (cb_expect_selchange)
               {
                 hh_aklink.pszKeywords = "Adding New Resolutions" ;
                 HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
               }
             }
             else
               tb_combo_sel = i ;
             cb_expect_selchange = 0 ;
             return (0) ;
           }
         }
         if (EditPassOnMessage (hwnd, message, wParam, lParam, &result))
           return (result) ;
         if (ExtensionsEnabled)
           if (LOWORD (wParam) >= CM_FIRSTGUIEXT && LOWORD (wParam) <= CM_LASTGUIEXT)
             return (ExternalMenuSelect (LOWORD (wParam))) ;
         if (handle_main_command (wParam, lParam))
           return (0) ;
         break ;

    case WM_INITMENU :
         EditUpdateMenus ((HMENU) wParam) ;
         EditPassOnMessage (hwnd, message, wParam, lParam, &result) ;
         break ;

    case WM_ACTIVATEAPP :
         EditPassOnMessage (hwnd, message, wParam, lParam, &result) ;
         break ;

    case WM_TIMER :
         if (!BackendFailedFlag && Session.BackendFailed())
         {
           BackendFailedFlag = true;
           if (rendering)
           {
             rendering = false;
             update_menu_for_render (false) ;
             PVEnableMenuItem (CM_FILERENDER, MF_DISABLED) ;
             PVEnableMenuItem (CM_STOPRENDER, MF_DISABLED) ;
             SendMessage (toolbar_window, TB_HIDEBUTTON, (WPARAM) CM_FILERENDER, MAKELONG (0, 0)) ;
             SendMessage (toolbar_window, TB_HIDEBUTTON, (WPARAM) CM_STOPRENDER, MAKELONG (0, 0)) ;
             status_buffer [0] = '\0' ;
             say_status_message (StatusMessage, "Render backend Failed - please re-start POV-Ray");
             delay_next_status = 2500;
             buffer_message (mFatal, "Render backend Failed - please re-start POV-Ray\n") ;
             if (render_complete_sound_enabled)
               PlaySound (render_error_sound, NULL, SND_ASYNC | SND_NODEFAULT) ;
           }
         }
         if (rendering && !Session.BackendFailed() && Session.GetPercentComplete() != LastRenderPercentage)
         {
           LastRenderPercentage = Session.GetPercentComplete();
           if (GetRenderWindow() != NULL)
           {
             sprintf (str, "%d%% complete", LastRenderPercentage) ;
             GetRenderWindow()->SetCaption(str);
           }
           if (main_window_hidden)
           {
             sprintf (str, "POV-Ray [%d%% complete] (Restore: DblClk ; Menu: Mouse2)", LastRenderPercentage) ;
             TaskBarModifyIcon (main_window, 0, str) ;
           }
           SendMessage (StatusPanelItems [IDC_STATUS_PROGRESS].hwnd, PBM_SETPOS, (WPARAM) LastRenderPercentage, 0) ;
           SetCaption (NULL) ;
         }
         if (delay_next_status)
         {
           delay_next_status -= 250 ;
           if (delay_next_status < 0)
             delay_next_status = 0 ;
         }
         if (status_buffer [0] != '\0' && delay_next_status == 0)
         {
           say_status_message (StatusMessage, status_buffer) ;
           status_buffer [0] = '\0' ;
         }

         memInfo.cb = sizeof(memInfo);
         GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
         status_printf(StatusMem, "\t%uMB", (unsigned int) (memInfo.WorkingSetSize / 1048576));
         if (TrackMem && memInfo.WorkingSetSize > PeakMem)
           PeakMem = memInfo.WorkingSetSize;

         // NOTE: if we ever change the timer rate, we need to update the editor code too.
         EditPassOnMessage (hwnd, message, wParam, lParam, &result) ;
         if (timer_ticks++ % 4 != 3)
           break ;
         seconds++ ;

#if POV_RAY_HAS_UPDATE_CHECK
         if (seconds % 600 == 0)
           DoUpdateCheck () ;
#endif

         ExternalEvent (EventTimer, seconds) ;
         if (MenuBarDraw)
         {
           DrawMenuBar (main_window) ;
           MenuBarDraw = false ;
         }
         if (!rendering)
         {
           if (auto_render)
           {
             if (queued_file_count)
             {
               queued_file_count-- ;
               update_queue_status (true) ;
               strcpy (source_file_name, queued_files [0]) ;
               memcpy (queued_files [0], queued_files [1], sizeof (queued_files) - sizeof (queued_files [0])) ;
               splitpath (source_file_name, dir, NULL) ;
               SetCurrentDirectory (dir) ;
               if (seconds < 60 && GetHKCU("Info", "Rendering", 0) != 0)
               {
                 // don't run the file if we were rendering when POV exited.
                 // [Rendering should only be set if there was an abnormal exit.]
                 PutHKCU ("Info", "Rendering", 0U) ;
                 message_printf ("Skipping queued file '%s' (possible abnormal exit)\n", source_file_name) ;
                 buffer_message (mDivider, "\n") ;
               }
               else
                 if (!ExternalDragFunction (source_file_name, dfRenderFileQueue))
                   start_rendering (false) ;
             }
           }
         }
         else
         {
           PrintRenderTimes (false, false) ;
           if (seconds % 10 == 0)
           {
             if (PreventSleep && !rendersleep)
               // TODO FIXME - According to Microsoft's API documentation,
               // ES_AWAYMODE_REQUIRED has no effect if used without ES_CONTINUOUS.
               SetThreadExecutionState(ES_AWAYMODE_REQUIRED | ES_SYSTEM_REQUIRED);
             if (rendersleep)
               FlashWindow (main_window, true) ;
           }
         }
         return (0) ;

    case WM_PALETTECHANGED :
         // make sure it wasn't us who changed the palette, otherwise we can get into an infinite loop.
         if ((HWND) wParam == main_window)
           return (0) ;
         // FALL THROUGH to WM_QUERYNEWPALETTE

    case WM_QUERYNEWPALETTE :
         if (hPalApp)
         {
           hdc = GetDC (main_window) ;
           oldPalette = SelectPalette (hdc, hPalApp, false) ;
           f = (RealizePalette (hdc) != 0);
           SelectPalette (hdc, oldPalette, false) ;
           ReleaseDC (main_window, hdc) ;
           if (f)
           {
             PovInvalidateRect (hwnd, NULL, true) ;
             if ((EditGetFlags () & EDIT_MSG_SELECTED) == 0)
               PovInvalidateRect (message_window, NULL, true) ;
             if (GetRenderwinHandle() != NULL)
               PovInvalidateRect (GetRenderwinHandle(), NULL, true) ;
           }
         }
         return (0) ;

    case WM_SIZE :
         if (main_window_hidden)
         {
           // perhaps another process has sent us a SIZE_RESTORED or something
           if (GetRenderWindow() != NULL)
             if (RenderwinIsChild == false && HideRenderWithMain == true)
               GetRenderWindow()->Show();
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Minimize to System &Tray\tAlt+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
         }
         mainwin_placement.length = sizeof (WINDOWPLACEMENT) ;
         GetWindowPlacement (main_window, &mainwin_placement) ;
         SendMessage (rebar_window, WM_SIZE, wParam, lParam) ;
         SendMessage (StatusWindow, WM_SIZE, wParam, lParam) ;
         ResizeStatusBar (StatusWindow) ;
         switch (wParam)
         {
           case SIZE_MINIMIZED :
                SetCaption (rendersleep ? "POV-Ray for Windows (paused)" : "POV-Ray for Windows") ;
                if (GetRenderWindow() != NULL)
                  if (RenderwinIsChild == false && HideRenderWithMain == true)
                    GetRenderWindow()->Hide();
                ExternalEvent (EventSize, wParam) ;
                return (0) ;

           case SIZE_RESTORED :
                SendMessage (StatusWindow, WM_SIZE, wParam, lParam) ;
                s = EditGetFilename(true) ;
                if (s != NULL && *s != '\0')
                {
                  sprintf (str, rendersleep ? "POV-Ray (paused) - %s" : "POV-Ray - %s", s) ;
                  SetCaption (str) ;
                }
                else
                  SetCaption (rendersleep ? "POV-Ray for Windows (paused)" : "POV-Ray for Windows") ;
                // ***** fall through *****

           case SIZE_MAXIMIZED :
                if (GetRenderwinHandle() != NULL)
                  if (RenderwinIsChild == false && HideRenderWithMain == true)
                    ShowWindow (GetRenderwinHandle(), MakeRenderwinActive ? SW_SHOW : SW_SHOWNA) ;
                SendMessage (toolbar_window, TB_AUTOSIZE, 0, 0) ;
                if (use_toolbar && rebar_window != NULL)
                {
                  GetClientRect (rebar_window, &rect) ;
                  toolheight = rect.bottom ;
                }
                CalculateClientWindows (true) ;

                // need this due to an issue with Windows 95
                if (top_message_row)
                {
                  ShowScrollBar (message_window, SB_VERT, false) ;
                  ShowScrollBar (message_window, SB_VERT, true) ;
                }
                if (need_hscroll ())
                {
                  ShowScrollBar (message_window, SB_HORZ, false) ;
                  ShowScrollBar (message_window, SB_HORZ, true) ;
                }
                ExternalEvent (EventSize, wParam) ;
                break ;

           case SIZE_MAXHIDE :
           case SIZE_MAXSHOW :
           default :
                ExternalEvent (EventSize, wParam) ;
                return (0) ;
         }
         return (0) ;

    case WM_MOVE :
         mainwin_placement.length = sizeof (WINDOWPLACEMENT) ;
         GetWindowPlacement (main_window, &mainwin_placement) ;
         ExternalEvent (EventMove, lParam) ;
         return (0) ;

    case WM_ERASEBKGND :
         if (IsIconic (main_window))
         {
           BitBlt ((HDC) wParam, 0, 0, 36, 36, NULL, 0, 0, BLACKNESS) ;
           return (1) ;
         }
         break ;

    case WM_DROPFILES :
         DragFunction ((HDROP) wParam) ;
         return (0) ;

    case WM_CHAR :
         switch ((char) wParam)
         {
           case 0x0f : // ctrl-o
                EditBrowseFile (true) ;
                return (0) ;

           case 0x0e : // ctrl-n (close enough to shift-ctrl-n ;)
                EditOpenFile (NULL) ;
                return (0) ;
         }
         if (EditPassOnMessage (hwnd, message, wParam, lParam, &result))
           return (0) ;
         break ;

    case WM_KEYDOWN :
         for (i = 0 ; key2scroll [i].wVirtkey != 0xffff ; i++)
         {
           if (wParam == key2scroll [i].wVirtkey)
           {
             SendMessage (message_window, key2scroll [i].iMessage, key2scroll [i].wRequest, 0L) ;
             return (0) ;
           }
         }
         break ;

    case WM_MENUSELECT :
         if (EditPassOnMessage (hwnd, message, wParam, lParam, &result))
           return (result) ;
         handle_menu_select (wParam, lParam) ;
         return (0) ;

    case WM_SHOWWINDOW :
         break ;

    case WM_CLOSE :
         if (debugging)
           message_printf ("Close requested, rendering is %d, quit is %u\n", rendering, quit) ;
         if (rendering && !quit)
         {
           if (MessageBox (main_window,
                           "POV-Ray is currently rendering - do you want to stop ?",
                           "Stop rendering",
                           MB_ICONQUESTION | MB_YESNO) == IDNO)
           {
             if (debugging)
               message_printf ("User tells us we can't close\n") ;
             return (0) ;
           }
         }
         if (!EditCanClose (true))
         {
           if (debugging)
             message_printf ("Editor tells us we can't close\n") ;
           return (0) ;
         }
         ExternalEvent (EventClose, 0) ;
         if (timer_id != 0)
           KillTimer (main_window, timer_id) ;
         DragAcceptFiles (main_window, false) ;
         if (!rendering || quit)
         {
           DestroyWindow (main_window) ;
         }
         else
         {
           if (!quit)
             quit = time (NULL) ;
           cancel_render () ;
         }
         return (0) ;

    case WM_DESTROY :
         if (debugging)
           message_printf ("Destroy requested, rendering is %d, quit is %u\n", rendering, quit) ;
         if (!quit)
         {
           quit = time (NULL) ;
           if (rendering)
             cancel_render () ;
         }
         ExternalEvent (EventDestroy, 0) ;
         if (save_settings)
         {
           SendMessage (toolbar_combobox, CB_GETLBTEXT, SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0), (LPARAM) SecondaryRenderIniFileSection) ;
           if (restore_command_line)
           {
             strcpy (command_line, old_command_line) ;
             SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
             restore_command_line = false ;
           }
           write_INI_settings () ;
           EditSaveState () ;
         }
         PostQuitMessage (0) ;
         return (0) ;
  }

  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

LRESULT CALLBACK PovMessageWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   nhs ;
  int                   msg ;
  int                   mousewheel ;
  HDC                   hdc ;
  RECT                  rect ;
  POINT                 pt ;
  PAINTSTRUCT           ps ;
  static bool           captured = false ;
  static POINT          mbdownPoint ;

  switch (message)
  {
    case WM_MOUSEWHEEL :
         mousewheel = (short) (wParam >> 16) / WHEEL_DELTA ;
         if ((LOWORD (wParam) & (MK_MBUTTON | MK_CONTROL)) == 0)
         {
           msg = mousewheel < 0 ? SB_LINEDOWN : SB_LINEUP ;
           mousewheel *= 3 ;
         }
         else
           msg = mousewheel < 0 ? SB_PAGEDOWN : SB_PAGEUP ;
         mousewheel = abs (mousewheel) ;
         while (mousewheel--)
           PovMessageWndProc (hwnd, WM_VSCROLL, msg, 0) ;
         return (0) ;

    case WM_KEYDOWN :
         PostMessage (main_window, message, wParam, lParam) ;
         return (0) ;

    case WM_LBUTTONDOWN :
         SetCapture (hwnd) ;
         captured = true ;
         GetCursorPos (&mbdownPoint) ;
         return (0) ;

    case WM_LBUTTONUP :
         if (!captured)
           return (0) ;
         ReleaseCapture () ;
         captured = false ;
         GetCursorPos (&pt) ;
         GetWindowRect (hwnd, &rect) ;
         if (pt.x < rect.left || pt.y < rect.top || pt.x > rect.right || pt.y > rect.bottom)
           return (0) ;
         if (abs (mbdownPoint.x - pt.x) > 3 || abs (mbdownPoint.y - pt.y) > 3)
         {
           hh_aklink.pszKeywords = "text streams" ;
           MessageBox (hwnd,
                       "You may use the Edit menu to copy the contents of this message pane to the clipboard\n\n"
                       "Press Help to learn how to direct the POV-Ray text output streams to a file",
                       "Text Selection Not Supported In This Window",
                       MB_OK | MB_ICONINFORMATION | MB_HELP) ;
         }
         return (0) ;

    case WM_RBUTTONDOWN :
         if (hPopupMenus != NULL)
         {
           pt.x = LOWORD (lParam) ;
           pt.y = HIWORD (lParam) ;
           ClientToScreen (hwnd, &pt) ;
           TrackPopupMenu (GetSubMenu (hPopupMenus, 0),
                           TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                           pt.x,
                           pt.y,
                           0,
                           main_window,
                           NULL) ;
         }
         return (0) ;

    case WM_ERASEBKGND :
         return (1) ;

    case WM_PAINT :
         hdc = BeginPaint (hwnd, &ps) ;
         if (hPalApp)
         {
           SelectPalette (hdc, hPalApp, false) ;
           RealizePalette (hdc) ;
         }
         paint_display_window (hdc) ;
         EndPaint (hwnd, &ps) ;
         return (0) ;

    case WM_SIZE :
         if (message_count)
         {
           GetClientRect (hwnd, &rect) ;
           message_scroll_pos_x = 0 ;
           message_scroll_pos_y = message_count - rect.bottom / message_ychar ;
           if (message_scroll_pos_y < 0)
             message_scroll_pos_y = 0 ;
         }
         update_message_display (None) ;
         PovInvalidateRect (hwnd, NULL, true) ;
         return (0) ;

    case WM_VSCROLL :
         switch (LOWORD (wParam))
         {
           case SB_LINEDOWN :
                if (message_scroll_pos_y < message_count - message_rows)
                {
                  message_scroll_pos_y++ ;
                  ScrollWindow (hwnd, 0, -message_ychar, NULL, NULL) ;
                  update_message_display (None) ;
                  UpdateWindow (hwnd) ;
                }
                break ;

           case SB_LINEUP :
                if (message_scroll_pos_y > 0)
                {
                  message_scroll_pos_y-- ;
                  ScrollWindow (hwnd, 0, message_ychar, NULL, NULL) ;
                  update_message_display (None) ;
                  UpdateWindow (hwnd) ;
                }
                break ;

           case SB_PAGEDOWN :
                if (message_scroll_pos_y < message_count - message_rows)
                {
                  message_scroll_pos_y += message_rows ;
                  if (message_scroll_pos_y > message_count - message_rows)
                    message_scroll_pos_y = message_count - message_rows ;
                  PovInvalidateRect (hwnd, NULL, true) ;
                  update_message_display (None) ;
                }
                break ;

           case SB_PAGEUP :
                if (message_scroll_pos_y > 0)
                {
                  message_scroll_pos_y -= message_rows ;
                  if (message_scroll_pos_y < 0)
                    message_scroll_pos_y = 0 ;
                  PovInvalidateRect (hwnd, NULL, true) ;
                  update_message_display (None) ;
                }
                break ;

           case SB_THUMBPOSITION :
           case SB_THUMBTRACK :
                message_scroll_pos_y = HIWORD (wParam) ;
                PovInvalidateRect (hwnd, NULL, true) ;
                update_message_display (None) ;
                break ;
         }
         return (0) ;

    case WM_HSCROLL :
         nhs = need_hscroll () ;
         switch (LOWORD (wParam))
         {
           case SB_LINERIGHT :
                if (message_scroll_pos_x < nhs)
                {
                  message_scroll_pos_x++ ;
                  ScrollWindow (hwnd, -message_xchar, 0, NULL, NULL) ;
                  update_message_display (None) ;
                  UpdateWindow (hwnd) ;
                }
                break ;

           case SB_LINELEFT :
                if (message_scroll_pos_x > 0)
                {
                  message_scroll_pos_x-- ;
                  ScrollWindow (hwnd, message_xchar, 0, NULL, NULL) ;
                  update_message_display (None) ;
                  UpdateWindow (hwnd) ;
                }
                break ;

           case SB_PAGERIGHT :
                if (message_scroll_pos_x < nhs)
                {
                  message_scroll_pos_x += message_cols ;
                  if (message_scroll_pos_x > nhs)
                    message_scroll_pos_x = nhs ;
                  PovInvalidateRect (hwnd, NULL, true) ;
                  update_message_display (None) ;
                }
                break ;

           case SB_PAGELEFT :
                if (message_scroll_pos_x > 0)
                {
                  message_scroll_pos_x -= message_cols ;
                  if (message_scroll_pos_x < 0)
                    message_scroll_pos_x = 0 ;
                  PovInvalidateRect (hwnd, NULL, true) ;
                  update_message_display (None) ;
                }
                break ;

           case SB_THUMBPOSITION :
           case SB_THUMBTRACK :
                message_scroll_pos_x = HIWORD (wParam) ;
                PovInvalidateRect (hwnd, NULL, true) ;
                update_message_display (None) ;
                break ;
         }
         return (0) ;
  }
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

INT_PTR CALLBACK PovStatusPanelDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG :
         MoveWindow (hDlg, 0, 0, 204, 41, false) ;
         SetupStatusPanel (hDlg) ;
         SetStatusPanelItemText (IDC_STATUS_DATA_FRAME, "N/A") ;
         return (false) ;
  }
  return (false) ;
}

LRESULT CALLBACK PovAboutWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   button;
  int                   offset = 0;
  int                   i;
  HDC                   hdc ;
  HDC                   hdcMemory ;
  bool                  f ;
  DWORD                 threadId = 0 ;
  DWORD                 threadParam = 0 ;
  POINT                 pt;
  HBITMAP               oldBmp ;
  HPALETTE              oldPalette ;
  PAINTSTRUCT           ps ;
  struct stat           st ;
  LPDRAWITEMSTRUCT      lpdis ;
  static char           *url = NULL;

  switch (message)
  {
    case WM_CREATE :
         about_showing = true ;
         url = NULL;
         break ;

    case WM_DESTROY :
         about_showing = false ;
         EnableWindow (main_window, TRUE) ;
         DeleteObject (hBmpAbout) ;
         if (about_palette)
         {
           DeleteObject (about_palette) ;
           about_palette = NULL ;
         }
         hBmpAbout = NULL ;
         break ;

    case WM_MOUSEMOVE:
         pt.x = LOWORD (lParam);
         pt.y = HIWORD (lParam);
         for (i = 0; i < NUM_ABOUT_LINKS; i++)
         {
           if (PtInRect(AboutLinks + i, pt))
           {
             SetCursor(LoadCursor(NULL, IDC_HAND));
             url = AboutURLs[i];
             return 0;
           }
         }
         SetCursor(LoadCursor(NULL, IDC_ARROW));
         url = NULL;
         return 0;

    case WM_SETCURSOR:
         return 1;

    case WM_LBUTTONUP:
         if (url != NULL)
         {
           SetCursor(LoadCursor(NULL, IDC_WAIT)); // will get changed on next mouse move
           ShellExecute (NULL, NULL, url, NULL, NULL, SW_SHOWNORMAL) ;
         }
         return (0) ;

    case WM_COMMAND :
         if (HIWORD (wParam) != BN_CLICKED)
           return (false) ;
         for (button = 0 ; button < 3 ; button++)
           if (about_buttons [button] == (HWND) lParam)
             break ;
         if (button == 3)
           break ;
         switch (button)
         {
           case 0 :
                // if help file is missing or something, default to internal viewer
                if (stat (engineHelpPath, &st) == 0)
                {
                  hh_aklink.pszKeywords = "POV-Ray License" ;
                  if (HtmlHelp (main_window, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink))
                    return (0) ;
                }
                DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_VIEW), main_window, (DLGPROC) PovLegalDialogProc, (LPARAM) main_window) ;
                break ;

           case 1 :
                save_povlegal () ;
                break ;

           case 2 :
                about_showing = false ;
                EnableWindow (main_window, TRUE) ;
                SetFocus (main_window) ;
                DestroyWindow (hwnd) ;
                break ;
         }
         return (0) ;

    case WM_DRAWITEM :
         lpdis = (LPDRAWITEMSTRUCT) lParam ;
         for (button = 0 ; button < 3 ; button++)
           if (about_buttons [button] == lpdis->hwndItem)
             break ;
         if (button == 3)
           return (0) ;
         if ((lpdis->itemState & ODS_SELECTED) != 0)
           offset = 400;
         else if (GetWindowLongPtr(lpdis->hwndItem, GWLP_USERDATA) != 0)
           offset = 200;
         hdc = lpdis->hDC ;
         if (about_palette)
           SelectPalette (hdc, about_palette, true) ;
         hdcMemory = CreateCompatibleDC (hdc) ;
         oldBmp = (HBITMAP) SelectObject (hdcMemory, hBmpAbout) ;
         BitBlt (hdc, 0, 0, 165, 30, hdcMemory, offset, button * 30 + about_height, SRCCOPY) ;
         SelectObject (hdcMemory, oldBmp) ;
         DeleteDC (hdcMemory) ;
         return (1) ;

    case WM_KEYDOWN :
         if (wParam == VK_RETURN || wParam == VK_ESCAPE || wParam == VK_SPACE)
         {
           EnableWindow (main_window, TRUE) ;
           SetFocus (main_window) ;
           DestroyWindow (hwnd) ;
           return (0) ;
         }
         return (0) ;

    case WM_PAINT :
         hdc = BeginPaint (hwnd, &ps) ;
         if (about_palette)
         {
           SelectPalette (hdc, about_palette, false) ;
           RealizePalette (hdc) ;
         }
         hdcMemory = CreateCompatibleDC (hdc) ;
         oldBmp = (HBITMAP) SelectObject (hdcMemory, hBmpAbout) ;
         BitBlt (hdc, 0, 0, about_width, about_height, hdcMemory, 0, 0, SRCCOPY) ;
         SelectObject (hdcMemory, oldBmp) ;
         DeleteDC (hdcMemory) ;
         EndPaint (hwnd, &ps) ;
         return (0) ;

    case WM_PALETTECHANGED :
         // make sure it wasn't us who changed the palette, otherwise we can get into an infinite loop.
         if ((HWND) wParam == hwnd)
           return (0) ;
         // FALL THROUGH to WM_QUERYNEWPALETTE

    case WM_QUERYNEWPALETTE :
         if (about_palette)
         {
           hdc = GetDC (hwnd) ;
           oldPalette = SelectPalette (hdc, about_palette, false) ;
           f = (RealizePalette (hdc) != 0);
           SelectPalette (hdc, oldPalette, false) ;
           ReleaseDC (hwnd, hdc) ;
           if (f)
             PovInvalidateRect (hwnd, NULL, true) ;
         }
         return (0) ;
  }
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

LRESULT CALLBACK PovAboutButtonWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  TRACKMOUSEEVENT       tme;

  switch (message)
  {
    case WM_MOUSEMOVE:
         if (GetWindowLongPtr(hwnd, GWLP_USERDATA) != 0)
           break;
         tme.cbSize = sizeof(tme);
         tme.dwFlags = TME_LEAVE;
         tme.dwHoverTime = 0;
         tme.hwndTrack = hwnd;
         TrackMouseEvent(&tme);
         InvalidateRect (hwnd, NULL, false);
         SetWindowLongPtr(hwnd, GWLP_USERDATA, 1);
         break;

    case WM_MOUSELEAVE:
         InvalidateRect (hwnd, NULL, false);
         SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
         break;

    case WM_KEYDOWN:
         if (wParam != VK_ESCAPE)
           break;
         EnableWindow (main_window, TRUE) ;
         SetFocus (main_window) ;
         DestroyWindow (about_window) ;
         return (0) ;
  }
  return (CallWindowProc (about_button_wndproc, hwnd, message, wParam, lParam)) ;
}

void ShowAboutBox (void)
{
  int         oldMode ;
  MSG         msg ;
  HDC         hdcMemory ;
  char        *s = POV_RAY_VERSION ;
  SIZE        size ;
  HFONT       oldFont ;
  BITMAP      bm ;
  HBITMAP     oldBmp ;
  COLORREF    oldColour ;

  if ((hBmpAbout = NonBogusLoadBitmapAndPalette (hInstance, MAKEINTRESOURCE (BMP_ABOUT))) != NULL)
  {
    GetObject (hBmpAbout, sizeof (BITMAP), (LPSTR) &bm) ;
    about_width = bm.bmWidth ;
    about_height = bm.bmHeight - 180 ;
    about_palette = hPalBitmap ;

    if (about_font == NULL)
      create_about_font () ;

    hdcMemory = CreateCompatibleDC (NULL) ;
    oldFont = (HFONT) SelectObject (hdcMemory, about_font) ;
    oldMode = SetBkMode (hdcMemory, TRANSPARENT) ;
    oldColour = SetTextColor (hdcMemory, RGB (0x96, 0xD3, 0xFF)) ;
    oldBmp = (HBITMAP) SelectObject (hdcMemory, hBmpAbout) ;
    GetTextExtentPoint (hdcMemory, s, (int) strlen (s), &size) ;
    ExtTextOut (hdcMemory, 387, 14, 0, NULL, s, (int) strlen (s), NULL) ;
    SetTextColor (hdcMemory, oldColour) ;
    SetBkMode (hdcMemory, oldMode) ;
    SelectObject (hdcMemory, oldFont) ;
    SelectObject (hdcMemory, oldBmp) ;
    DeleteDC (hdcMemory) ;

    about_window = CreateWindowEx (0,//WS_EX_TOOLWINDOW,
                                   PovAboutWinClass,
                                   "POV-Ray",
                                   WS_POPUP,
                                   (screen_width - about_width) / 2,
                                   (screen_height - about_height) / 2,
                                   about_width,
                                   about_height,
                                   main_window,
                                   NULL,
                                   hInstance,
                                   NULL) ;
    for (int i = 0; i < 3; i++)
    {
      about_buttons[i] = CreateWindow ("BUTTON", "", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, i * 184 + 27, 451, 165, 30, about_window, NULL, hInstance, NULL) ;
      about_button_wndproc = (WNDPROC) (ULONG_PTR) SetWindowLongPtr (about_buttons[i], GWLP_WNDPROC, (ULONG_PTR) PovAboutButtonWndProc) ;
    }
    CenterWindowRelative (main_window, about_window, false, true) ;
    ShowWindow (about_window, SW_SHOWNORMAL) ;
    SetFocus (about_window) ;
    EnableWindow (main_window, FALSE) ;
    while (about_showing && GetMessage (&msg, NULL, 0, 0) != 0 && quit == 0)
    {
      if (!TranslateAccelerator (main_window, hAccelerators, &msg))
      {
        TranslateMessage (&msg) ;
        DispatchMessage (&msg) ;
      }
    }
  }
}

/**************************************************************************************/
/*                                 END OF WINDOW PROCEDURES                           */
/**************************************************************************************/

int register_classes (void)
{
  WNDCLASSEX  wc ;

  // the parameter to RegisterClass is considered CONST, so we
  // can assume that the structure is not changed by the calls.
  wc.cbSize        = sizeof (wc) ;
  wc.hIconSm       = NULL ;
  wc.cbClsExtra    = 0 ;
  wc.cbWndExtra    = 0 ;
  wc.hInstance     = hInstance ;
  wc.lpszMenuName  = NULL ;
  wc.hbrBackground = NULL ;

  // Register the main window class.
  wc.style         = CS_BYTEALIGNCLIENT ;
  wc.lpfnWndProc   = PovMainWndProc ;
  wc.hIcon         = ourIcon ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.lpszClassName = PovMainWinClass ;
  if (RegisterClassEx (&wc) == false)
    return (false) ;

  // Register the message window class.
  wc.style         = CS_BYTEALIGNCLIENT ;
  wc.lpfnWndProc   = PovMessageWndProc ;
  wc.hIcon         = NULL ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.lpszClassName = PovMessageWinClass ;
  if (RegisterClassEx (&wc) == false)
    return (false) ;

  // Register the about window class.
  wc.style         = CS_BYTEALIGNCLIENT ;
  wc.lpfnWndProc   = PovAboutWndProc ;
  wc.hIcon         = ourIcon ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.lpszClassName = PovAboutWinClass ;
  if (RegisterClassEx (&wc) == false)
    return (false) ;

  // Register the render window classes
  wc.style         = CS_BYTEALIGNCLIENT ;
  wc.lpfnWndProc   = WinLegacyDisplay::StaticWindowProc ;
  wc.hIcon         = renderIcon ;
  wc.hCursor       = LoadCursor (NULL, IDC_CROSS) ;
  wc.hbrBackground = NULL ;
  wc.lpszClassName = PovLegacyRenderWinClass ;
  wc.cbWndExtra    = 8 ;
  if (RegisterClassEx (&wc) == false)
    return (false) ;

  return (true) ;
}

void cleanup_all (void)
{
  // TODO: nuke any render threads still running
  ExternalCleanupAll () ;
  EditUnload () ;
  if (use_taskbar)
    TaskBarDeleteIcon (main_window, 0) ;
  clear_messages (false) ;
  clear_dir_restrictions () ;
  DeleteCriticalSection (&critical_section) ;
  display_cleanup (true) ;
  if (hBmpBackground != NULL)
    DeleteObject (hBmpBackground) ;
  if (hBmpRendering != NULL)
    DeleteObject (hBmpRendering) ;
  if (hBmpIcon != NULL)
    DeleteObject (hBmpIcon) ;
  if (hMenuBar)
    DestroyMenu (hMenuBar) ;
  if (hMainMenu)
    DestroyMenu (hMainMenu) ;
  if (hPopupMenus)
    DestroyMenu (hPopupMenus) ;
  if (hPalApp)
    DeleteObject (hPalApp) ;
  if (message_font)
    DeleteObject (message_font) ;
  if (about_font)
    DeleteObject (about_font) ;
  if (tab_font)
    DeleteObject (tab_font) ;
  if (ourIcon)
    DestroyIcon (ourIcon) ;
  if (renderIcon)
    DestroyIcon (renderIcon) ;

  UnregisterClass (PovLegacyRenderWinClass, hInstance) ;
  UnregisterClass (PovMessageWinClass, hInstance) ;
  UnregisterClass (PovMainWinClass, hInstance) ;
}

#ifdef _MSC_VER
void LZTimerOn (void)
{
  if (!QueryPerformanceCounter ((LARGE_INTEGER *) &PerformanceCounter1))
    PerformanceCounter1 = 0 ;
}

void LZTimerOff (void)
{
  if (!QueryPerformanceCounter ((LARGE_INTEGER *) &PerformanceCounter2))
    PerformanceCounter2 = 0 ;
}

unsigned long LZTimerCount (void)
{
  if (PerformanceCounter1 == 0 || PerformanceCounter2 < PerformanceCounter1)
    return (0) ;
  return ((unsigned long) ((PerformanceCounter2 - PerformanceCounter1) / PerformanceScale)) ;
}

__int64 LZTimerRawCount (void)
{
  if (PerformanceCounter1 == 0 || PerformanceCounter2 < PerformanceCounter1)
    return (0) ;
  return (PerformanceCounter2 - PerformanceCounter1) ;
}
#endif // #ifdef _MSC_VER

}
// end of namespace povwin

static void __cdecl newhandler (void)
{
  throw std::bad_alloc () ;
}

using namespace povwin ;

void GenerateDumpMeta(bool brief)
{
  char                  str[128];
  char                  *s = DumpMeta;
  char                  *InstalledOn ;
  HDC                   hdc ;
  HKEY                  key ;
  DWORD                 len;
  DWORD                 header = 0 ;
  DWORD                 n ;
  FILETIME              file_time ;
  SYSTEMTIME            system_time ;
  SYSTEM_INFO           sysinfo ;
  OSVERSIONINFO         version_info ;
  MEMORYSTATUSEX        mem_status ;

  GetNativeSystemInfo(&sysinfo) ;
  if (!brief)
  {
    if ((InstalledOn = GetInstallTime()) == NULL)
    {
      GetSystemTime (&system_time) ;
      if (SystemTimeToFileTime (&system_time, &file_time))
        reg_printf (true, "Software\\POV-Ray", INSTALLTIMEKEY, "%I64u", ((__int64) file_time.dwHighDateTime << 32) | file_time.dwLowDateTime) ;
      if ((InstalledOn = GetInstallTime ()) == NULL)
        InstalledOn = "Unknown" ;
    }
    s += sprintf(s, "installdate=%s\n", InstalledOn) ;
  }
  s += sprintf(s, "cpuarchitecture=%u\n", (DWORD) sysinfo.wProcessorArchitecture) ;
  s += sprintf(s, "numberofcpus=%u\n", sysinfo.dwNumberOfProcessors) ;
  s += sprintf(s, "processortype=%u\n", sysinfo.dwProcessorType) ;
  s += sprintf(s, "processorlevel=%u\n", (DWORD) sysinfo.wProcessorLevel) ;
  s += sprintf(s, "processorrevision=%u\n", (DWORD) sysinfo.wProcessorRevision) ;

  if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "HARDWARE\\Description\\System\\CentralProcessor\\0", 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    len = sizeof (n) ;
    if (RegQueryValueEx (key, "~MHZ", 0, NULL, (BYTE *) &n, &len) == ERROR_SUCCESS)
      s += sprintf(s, "cpufrequency=%u\n", n) ;

    len = sizeof (n) ;
    if (RegQueryValueEx (key, "FeatureSet", 0, NULL, (BYTE *) &n, &len) == ERROR_SUCCESS)
      s += sprintf(s, "cpufeatureset=%u\n", n) ;

    len = sizeof (str) ;
    if (RegQueryValueEx (key, "ProcessorNameString", 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
      s += sprintf(s, "cpuname=%s\n", str) ;

    len = sizeof (str) ;
    if (RegQueryValueEx (key, "Identifier", 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
      s += sprintf(s, "cpuidentifier=%s\n", str) ;

    len = sizeof (str) ;
    if (RegQueryValueEx (key, "VendorIdentifier", 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
      s += sprintf(s, "cpuvendoridentifier=%s\n", str) ;

    RegCloseKey (key) ;
  }

  version_info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO) ;
  GetVersionEx (&version_info) ;
  s += sprintf(s, "osversion=%u.%u\n", version_info.dwMajorVersion, version_info.dwMinorVersion) ;
  s += sprintf(s, "osbuild=%u\n", version_info.dwBuildNumber) ;
  s += sprintf(s, "csdversion=%s\n", version_info.szCSDVersion) ;

  if (!brief)
  {
    hdc = GetDC (NULL) ;
    s += sprintf(s, "bitsperpixel=%u\n", GetDeviceCaps (hdc, BITSPIXEL)) ;
    s += sprintf(s, "horzres=%u\n", GetDeviceCaps (hdc, HORZRES)) ;
    s += sprintf(s, "vertres=%u\n", GetDeviceCaps (hdc, VERTRES)) ;
    ReleaseDC (NULL, hdc) ;
  }

  mem_status.dwLength = sizeof (MEMORYSTATUSEX) ;
  GlobalMemoryStatusEx(&mem_status) ;
  s += sprintf(s, "physicalmemory=%I64u\n", mem_status.ullTotalPhys) ;
}

bool WriteDumpMeta(struct _EXCEPTION_POINTERS *ExceptionInfo, const char *filename)
{
  FILE          *f;
  PCONTEXT      c = ExceptionInfo->ContextRecord ;
  static char   fn[_MAX_PATH];

  strcpy(fn, filename);
  strcat(fn, ".metadata");

  if ((f = fopen(fn, "wt")) == NULL)
    return false;
  fwrite(DumpMeta, strlen(DumpMeta), 1, f);
  fprintf(f, "faulttype=%u\n", ExceptionInfo->ExceptionRecord->ExceptionCode);
#ifdef _WIN64
  fprintf(f, "faultaddress=%I64u\n", ExceptionInfo->ContextRecord->Rip);
  fprintf(f, "faultplatform=win64\n");
#else
  fprintf(f, "faultaddress=%u\n", ExceptionInfo->ContextRecord->Eip);
  fprintf(f, "faultplatform=win32\n");
#endif
  fprintf(f, "povversion=%s\n", POV_RAY_SOURCE_VERSION) ;
  fprintf(f, "compilerversion=%s\n", POV_COMPILER_VER) ;
  fprintf(f, "platformversion=%s\n", POVRAY_PLATFORM_NAME) ;
  fprintf(f, "remotesession=%u\n", GetSystemMetrics(SM_REMOTESESSION)) ;
  fclose(f);
  return true;
}

// TODO: re-write this!!!
char *WriteDump(struct _EXCEPTION_POINTERS *pExceptionInfo, bool full, long timestamp)
{
  // firstly see if dbghelp.dll is around and has the function we need
  // look next to the EXE first, as the one in System32 might be old
  // (e.g. Windows 2000)
  HMODULE hDll = NULL;
  static char szDbgHelpPath[_MAX_PATH];

  if (GetModuleFileName(NULL, szDbgHelpPath, _MAX_PATH ))
  {
    char *pSlash  = findLastPathSeparator( szDbgHelpPath );
    if (pSlash)
    {
      strcpy( pSlash+1, "DBGHELP.DLL" );
      hDll = ::LoadLibrary( szDbgHelpPath );
    }
  }

  if (hDll==NULL)
  {
    // load any version we can
    hDll = ::LoadLibrary( "DBGHELP.DLL" );
  }

  LPCTSTR szResult = NULL;

  if (hDll)
  {
    MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
    if (pDump)
    {
      char szScratch [_MAX_PATH];
      static char szDumpPath[_MAX_PATH];

      if (full)
        sprintf(szScratch, "POV-Ray-" POV_RAY_VERSION "-%08X.dmp", timestamp);
      else
        sprintf(szScratch, "POV-Ray-" POV_RAY_VERSION "-%08X.minidump", timestamp);

      // work out a good place for the dump file
      if (!GetTempPath( _MAX_PATH - 64, szDumpPath))
        _tcscpy( szDumpPath, "c:\\" );

      _tcscat( szDumpPath, szScratch );

      // create the file
      HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

      if (hFile!=INVALID_HANDLE_VALUE)
      {
        _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

        ExInfo.ThreadId = ::GetCurrentThreadId();
        ExInfo.ExceptionPointers = pExceptionInfo;
        ExInfo.ClientPointers = NULL;

        // write the dump
        BOOL bOK = pDump(GetCurrentProcess(),
                         GetCurrentProcessId(),
                         hFile,
                         full ? MiniDumpWithFullMemory :
                                (MINIDUMP_TYPE)(MiniDumpWithDataSegs|MiniDumpWithHandleData|MiniDumpWithIndirectlyReferencedMemory), &ExInfo, NULL, NULL );
        ::CloseHandle(hFile);
        if (bOK)
        {
          if (!WriteDumpMeta(pExceptionInfo, szDumpPath))
          {
            MessageBox (main_window, "Failed to save dump metadata file", "POV-Ray for Windows", MB_OK | MB_ICONEXCLAMATION);
            return NULL;
          }
          return szDumpPath;
        }
        sprintf( szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
        MessageBox (main_window, szScratch, "POV-Ray for Windows", MB_OK | MB_ICONEXCLAMATION);
        return NULL;
      }
      sprintf( szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
      MessageBox (main_window, szScratch, "POV-Ray for Windows", MB_OK | MB_ICONEXCLAMATION);
      return NULL;
    }
    MessageBox (main_window, "Sorry, DBGHELP.DLL is too old - cannot create dump.", "POV-Ray for Windows", MB_OK | MB_ICONEXCLAMATION);
    return NULL;
  }
  MessageBox (main_window, "Sorry, we can't locate DBGHELP.DLL - cannot create dump.", "POV-Ray for Windows", MB_OK | MB_ICONEXCLAMATION);
  return NULL;
}

#ifdef BUILD_AVX2
void NoAVX2 (void)
{
  MessageBox (NULL,
              "This build of POV-Ray requires that your processor provides AVX2 support.\n"
              "Please use the standard non-AVX2 version of POV-Ray on this computer.",
              "POV-Ray for Windows",
              MB_ICONSTOP | MB_OK) ;
  std::exit (-1) ;
}

inline void TestAVX2 (void)
{
  if (!CPUInfo::SupportsAVX2())
    NoAVX2();
}
#endif // BUILD_AVX2

#ifdef BUILD_AVX
void NoAVX (void)
{
  MessageBox (NULL,
              "This build of POV-Ray requires that your processor provides AVX support.\n"
              "Please use the standard non-AVX version of POV-Ray on this computer.",
              "POV-Ray for Windows",
              MB_ICONSTOP | MB_OK) ;
  std::exit (-1) ;
}

inline void TestAVX (void)
{
  if (!CPUInfo::SupportsAVX())
    NoAVX();
}
#endif // BUILD_AVX

#ifdef BUILD_SSE2
void NoSSE2 (void)
{
  MessageBox (NULL,
              "This build of POV-Ray requires that your processor provides SSE2 support.\n"
              "Please use the standard non-SSE2 version of POV-Ray on this computer.",
              "POV-Ray for Windows",
              MB_ICONSTOP | MB_OK) ;
  std::exit (-1) ;
}

inline void TestSSE2 (void)
{
  if (HaveVistaOrLater())
  {
    // Use the canonical test.
    if (!CPUInfo::SupportsSSE2())
      NoSSE2();
  }
  else
  {
    // On Windows XP (and presumably also Windows Server 2003), the canonical test does not seem
    // to work properly for yet unknown reasons, so we test for support the dirty way by trying to
    // actually execute an SSE2 instruction.
    __try
    {
      __asm { movapd xmm0, xmm1 }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
      NoSSE2();
    }
  }
}
#endif // BUILD_SSE2

void InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
{
  throw POV_EXCEPTION(kParamErr, "Run-Time Library Invalid Parameter Handler invoked");
}

int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
#ifdef BUILD_SSE2
  TestSSE2();
#endif
#ifdef BUILD_AVX
  TestAVX();
#endif
#ifdef BUILD_AVX2
  TestAVX2();
#endif

  int                   show_state ;
  int                   i ;
  int                   id = 0 ;
  int                   w, h ;
  char                  str [_MAX_PATH * 2] ;
  char                  *s = szCmdLine ;
  bool                  exit_loop = false ;
  unsigned              n ;
  MSG                   msg ;
  HDC                   hDC ;
  HDIB                  hDIB ;
  RECT                  rect ;
  HWND                  hwnd ;
  DWORD                 help_cookie ;
  DWORD                 threadId = 0 ;
  DWORD                 threadParam = 0 ;
  BITMAP                bm ;
  HBITMAP               hBMP ;
  struct stat           statbuf ;
  SYSTEM_INFO           sysinfo ;
  WINDOWPLACEMENT       placement ;

  // REMINDER: don't try to write to the message pane until initialise_message_display() has been called.

  _set_invalid_parameter_handler(InvalidParameterHandler);

#ifdef _DEBUG
  _unlink ("c:\\temp\\povdebug.txt") ;
  _unlink ("c:\\temp\\povmem.txt") ;
#endif

  GenerateDumpMeta(false);
  std::set_new_handler(newhandler) ;
  SetUnhandledExceptionFilter(ExceptionHandler);

  // need this now to set virtual_screen_width etc., in case we display a dialog
  // before the main setup (we call it again once we've read the INI file).
  detect_graphics_config () ;

  try
  {
    CreateFrontend();
  }
  catch (pov_base::Exception& e)
  {
    sprintf (str, "Failed to initialize frontend: %s", e.what()) ;
    MessageBox (NULL, str, "POV-Ray Critical Error", MB_ICONSTOP) ;
    return (1) ;
  }

  hInstance = hInst ;
  hMainThread = GetCurrentThread () ;

  GetSystemInfo (&sysinfo) ;
  ThreadCount = sysinfo.dwNumberOfProcessors ;
  NumberOfCPUs = sysinfo.dwNumberOfProcessors ;

  while (*s == ' ' || *s == '\t')
    s++ ;
  if (_stricmp (s, "/install") == 0 || _strnicmp (s, "/install ", 9) == 0 || _stricmp (s, "/qinstall") == 0 || _strnicmp (s, "/qinstall ", 10) == 0)
  {
    bool quiet = s [1] == 'q' || s [1] == 'Q' ;
    while (*s && *s != ' ')
      s++ ;
    argv[1] = argv[2] = NULL;
    parse_commandline(s);
    return (InstallSettings (argv[1], argv[2], quiet)) ;
  }

  getHome () ;
  if (BinariesPath [0] == '\0')
  {
    inferHome () ;
    if (BinariesPath [0] == '\0')
    {
      MessageBox (NULL,
                  "ERROR : Cannot find Home entry in registry (and cannot infer it).\n\n"
                  "This entry should have been set by the installation program.\n\n"
                  "POV-Ray can usually infer the installation path but that requires a\n"
                  "standard layout of directories, which also seems to be absent.\n\n"
                  "If you did not install using the correct installation procedure, please\n"
                  "do this before running POV-Ray for Windows. You can also try running\n"
                  "with the '/INSTALL' or '/INSTALL <bindir> [<docdir>]' option.",
                  "Critical Error",
                  MB_ICONSTOP) ;
      return (1) ;
    }
  }
  appendPathSeparator(BinariesPath) ;
  if (DocumentsPath[0] == '\0')
  {
    if (SHGetFolderPath (NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, DocumentsPath) != S_OK)
    {
      MessageBox (NULL,
                  "ERROR : Cannot find DocPath entry in registry (and cannot infer it).\n\n"
                  "This entry should have been set by the installation program.\n\n"
                  "POV-Ray can usually infer the installation path but that requires a\n"
                  "standard layout of directories, which also seems to be absent.\n\n"
                  "If you did not install using the correct installation procedure, please\n"
                  "do this before running POV-Ray for Windows. You can also try running\n"
                  "with the '/INSTALL' or '/INSTALL <bindir> <docdir>' option.",
                  "Critical Error",
                  MB_ICONSTOP) ;
    }
    strcat(DocumentsPath, "\\" REGKEY);
    if (!dirExists(DocumentsPath))
      CreateDirectory(DocumentsPath, NULL);
    strcat(DocumentsPath, "\\" REGVERKEY);
    if (!dirExists(DocumentsPath))
      CreateDirectory(DocumentsPath, NULL);
  }
  appendPathSeparator(DocumentsPath) ;
  sprintf(str, "%sini", DocumentsPath);
  if (!dirExists(str))
    CreateDirectory(str, NULL);
  sprintf(ToolIniFileName, "%sini\\pvtools.ini", DocumentsPath);
  sprintf(EngineIniFileName, "%sini\\pvengine.ini", DocumentsPath);

  if (!checkEditKey(REGVERKEY))
  {
    for (const char **oldVer = CanInheritFromVersions; *oldVer != NULL; ++oldVer)
    {
      if (checkEditKey(*oldVer))
      {
        copyEditSettings(*oldVer);
        break;
      }
    }
  }

  if (checkRegKey () == false || FreshInstall == true)
    if (!CloneOptions())
      MessageBox (NULL, "ERROR : Could not clone options - POV-Ray may not work correctly for this user.", "Critical Error", MB_ICONERROR) ;

#ifdef POVRAY_IS_BETA
  if (FreshInstall)
  {
    // on installing a new beta, we always turn on check new version now that we no longer
    // implement a short timeout. the user may switch it off if they like; we will not perform
    // an automatic version check in this session since FreshInstall is set, in order to give
    // them a chance to do it.
    check_new_version = true;
    PutHKCU("General", "CheckNewVersion", true);
  }
#endif

#ifndef MAP_INI_TO_REGISTRY
  if (!fileExists(EngineIniFileName))
  {
    bool foundOld = false;
    for (const char **oldVer = CanInheritFromVersions; *oldVer != NULL; ++oldVer)
    {
      // no INI file: see if we can copy an older version's INI options, should they exist
      if (debugging)
        debug_output("no pvengine.ini: seeing if there is a %s ini\n", *oldVer) ;

      std::string str(getHome(*oldVer));
      if (str.empty() == false)
      {
        std::string oldINIpath = str + "ini\\pvengine.ini";
        if (!fileExists(oldINIpath.c_str()))
          continue;

        if (debugging)
          debug_output("cloning INI file %s to %s\n", oldINIpath.c_str(), EngineIniFileName) ;
        cloneOldIni(str, DocumentsPath);
        foundOld = true;
        break;
      }
    }
    if (!foundOld)
    {
      if (debugging)
        debug_output("creating default INI file %s\n", EngineIniFileName) ;
      cloneOldIni("", DocumentsPath);
    }
  }
#endif

  if (homeInferred)
  {
    if (_stricmp (LastInferredHome, BinariesPath) != 0)
    {
      sprintf (str, "POV-Ray for Windows did not find the expected registry entries present.\n"
                    "This typically means that it has not been installed via the installation program.\n"
                    "You can correct this by running with the '/INSTALL' or '/INSTALL <installdir>' option.\n\n"
                    "POV-Ray has inferred the installation path to be the following:\n\n"
                    "\t%s\n\n"
                    "This message will be displayed each time the inferred path changes.",
                    BinariesPath) ;
      MessageBox (NULL, str, "Warning", MB_ICONINFORMATION) ;
    }
    reg_printf (true, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", "LastInferredHome", "%s", BinariesPath) ;
  }

  sprintf(ToolIniFileName, "%sini\\pvtools.ini", DocumentsPath);
  sprintf (DefaultRenderIniFileName, "%sini\\povray.ini", DocumentsPath) ;
  GetModuleFileName (hInst, str, sizeof (str) - 1) ;
  splitpath (str, modulePath, NULL) ;
  validatePath (modulePath) ;

  sprintf (engineHelpPath, "%shelp\\povray.chm", BinariesPath) ;
  HtmlHelp (NULL, NULL, HH_INITIALIZE, (DWORD_PTR) &help_cookie) ;
  memset (&hh_aklink, 0, sizeof (hh_aklink)) ;
  hh_aklink.cbStruct = sizeof (hh_aklink) ;
  hh_aklink.fIndexOnFail = true ;
  hh_aklink.pszWindow = "POV-Ray Help" ;

  SHGetFolderPath (NULL, CSIDL_FONTS, NULL, SHGFP_TYPE_CURRENT, FontPath) ;

  one_instance = GetHKCU("General", "OneInstance", 1) != 0 ;
  if ((hwnd = FindWindow (PovMainWinClass, NULL)) != NULL)
  {
    if (one_instance)
    {
      if (IsIconic (hwnd))
        ShowWindow (hwnd, SW_RESTORE) ;
      SetForegroundWindow (hwnd) ;
      FeatureNotify ("OneInstanceSet",
                     "POV-Ray - 'Keep Single Instance' Feature",
                     "You have started POV-Ray for Windows while another copy is running, "
                     "and the 'Keep Single Instance' option is turned on (see Options menu). "
                     "In this case the other copy is activated rather than starting a new "
                     "instance of the program.\n\nClick &Help for information on this feature.",
                     "Keep Single Instance",
                     false) ;

      // special case: there's a chance we're being called as a result of a windows file association
      // default, which allows users to associate POV-Ray with arbitrary files. in this case, no
      // /EDIT or /RENDER verb will be present; just the filename. what we do here is see if the
      // passed string is a real file by calling fileExists(). if it is, then we assume /EDIT is
      // desired.
      n = strlen(szCmdLine);
      if (n < _MAX_PATH && n > 2 && szCmdLine[0] == '"' && szCmdLine[n - 1] == '"')
      {
        memcpy(str, szCmdLine + 1, n - 2);
        str[n - 2] = '\0';
        if (fileExists(str))
        {
          FeatureNotify ("EditByDefault",
                          "POV-Ray - Filename passed on command-line",
                          "Quoted filenames which are the sole command-line parameter are now "
                          "opened in the editor by default. If you wish to render them instead "
                          "please pass the /RENDER switch, provide more than one parameter, or "
                          "use single quotes around the path.\n\n"
                          "Click &Help for more information.",
                          "associations", false) ;
          add_edit_file(str);
          szCmdLine = "";
        }
      }

      SetForegroundWindow (hwnd) ;
      if ((s = preparse_instance_commandline (szCmdLine)) != NULL)
      {
        parse_commandline(s);
        if (argc > 1)
        {
          PovMessageBox ("Only /EDIT and /RENDER may be passed to previous instance", "Commandline processing error") ;
          return (1) ;
        }
        COPYDATASTRUCT cd ;
        cd.dwData = EDIT_FILE ;
        for (i = 0 ; i < EditFileCount ; i++)
        {
          s = EditFiles [i] ;
          if ((isalpha (s [0]) && s [1] == ':') || (isPathSeparator(s [0]) && isPathSeparator(s [1])))
          {
            cd.cbData = (int) strlen (s) + 1 ;
            cd.lpData = s ;
          }
          else
          {
            GetCurrentDirectory (sizeof (str), str) ;
            if (!isPathSeparator(s [0]))
            {
              strcat (str, "\\") ;
              strcat (str, s) ;
            }
            else
              strcpy (str + 2, s) ;
            cd.cbData = (int) strlen (str) + 1 ;
            cd.lpData = str ;
          }
          SendMessage (hwnd, WM_COPYDATA, NULL, (LPARAM) &cd) ;
          free (EditFiles [i]) ;
          EditFiles [i] = NULL ;
        }
        EditFileCount = 0 ;
        if (render_requested)
        {
          cd.dwData = RENDER_FILE ;
          s = requested_render_file ;
          if ((isalpha (s [0]) && s [1] == ':') || (isPathSeparator(s [0]) && isPathSeparator(s [1])))
          {
            cd.cbData = (int) strlen (s) + 1 ;
            cd.lpData = s ;
          }
          else
          {
            GetCurrentDirectory (sizeof (str), str) ;
            if (!isPathSeparator(s [0]))
            {
              strcat (str, "\\") ;
              strcat (str, s) ;
            }
            else
              strcpy (str + 2, s) ;
            cd.cbData = (int) strlen (str) + 1 ;
            cd.lpData = str ;
          }
          SendMessage (hwnd, WM_COPYDATA, NULL, (LPARAM) &cd) ;
        }
        SetForegroundWindow (hwnd) ;
      }
      else
        return (1) ;
      return (0) ;
    }
    else
    {
      // one_instance isn't set. we should continue as per normal.
      // however see if we need to notify the user about this.
      FeatureNotify ("OneInstanceUnset",
                     "POV-Ray - 'Keep Single Instance' Feature",
                     "You have started POV-Ray for Windows while another copy is running, "
                     "and the 'Keep Single Instance' option is turned off (see Options menu). "
                     "In this case a new instance of the program is started rather than "
                     "activating the existing instance of the program.\n\nClick &Help for more "
                     "information on this feature.",
                     "Keep Single Instance",
                     false) ;
    }
  }

  if (_strnicmp (szCmdLine, "/DEBUG", 6) == 0)
  {
    debugging = true ;
    debug_output(NULL) ;
  }

  SetThreadPriority (hMainThread, THREAD_PRIORITY_ABOVE_NORMAL) ;
  version_info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO) ;
  GetVersionEx (&version_info) ;
  use_editors = editors_enabled ;

  IsW98 = HaveWin98OrLater () ;
  IsWNT = HaveNT4OrLater () ;
  IsW2k = HaveWin2kOrLater () ;
  IsWXP = HaveWinXPOrLater () ;
  IsVista = HaveVistaOrLater () ;

  // yes, we actually used to support the windows 3.1 UI ...
  IsW95UserInterface = true ;

  ourIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IsWXP ? IDI_PVENGINE_XP : IDI_PVENGINE)) ;
  renderIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IsWXP ? IDI_RENDERWINDOW_XP : IDI_RENDERWINDOW)) ;

  if (hPrev == NULL)
    if (register_classes () == false)
      MessageBox (NULL, "ERROR : Could not register classes", "Error", MB_ICONSTOP) ;

  IsComCtl5 = GetDllVersion (TEXT ("comctl32.dll")) >= MAKELONG (5,0) ;
  IsComCtl6 = GetDllVersion (TEXT ("comctl32.dll")) >= MAKELONG (6,0) ;

  // need to init menus before we read INI settings
  init_menus () ;

  // need to do this before we detect the graphics config
  read_INI_settings () ;

  detect_graphics_config () ;
  clear_system_palette () ;
  hPalApp = WinLegacyDisplay::CreatePalette (NULL, 0, render_bitmap_depth != 24) ;

  if (!QueryPerformanceFrequency ((LARGE_INTEGER *) &PerformanceFrequency))
    PerformanceFrequency = 0 ;
  if (PerformanceFrequency > 1999999)
    PerformanceScale = PerformanceFrequency / 1000000 ;

  // 'IsW95UserInterface' dates from when Windows 95 and NT4 were introduced
  // (at that time we still supported the old Windows 3.1 UI)
  // IsW95UserInterface = GetHKCU("General", "UseW95UserInterface", 1) != 0 ;
  IsW95UserInterface = true;
  info_render_complete = GetHKCU("Info", "RenderCompleteSound", 0) != 0 ;

  SetupFrontend();

  if (!IsW95UserInterface)
  {
    PVEnableMenuItem (CM_SHOWMAINWINDOW, MF_GRAYED) ;
    use_taskbar = false ;
  }

  create_about_font () ;

  InitializeCriticalSection (&critical_section) ;

  GetHKCU("General", "CommandLine", "", old_command_line, sizeof (old_command_line)) ;
  strcpy (command_line, old_command_line) ;

  // special case: POV may get associated with an arbitrary file type by the user.
  // in such cases we won't be given any verbs to indicate what to do with the file,
  // and in many instances our default action (which is to assume the file is either
  // SDL or INI) is not appropriate. so we make an exception here for such cases.
  //
  // if the sole command-line parameter is a QUOTED filename referring to a file which
  // exists, we assume /EDIT is desired (this may break some scripts that assume the
  // default; in such cases the script should explicitly pass /RENDER). the shell
  // always quotes files passed in this way; by testing for the quotes we can
  // eliminate some unnecessary tests.
  n = strlen(szCmdLine);
  if (n < _MAX_PATH && n > 2 && szCmdLine[0] == '"' && szCmdLine[n - 1] == '"')
  {
    memcpy(str, szCmdLine + 1, n - 2);
    str[n - 2] = '\0';
    if (fileExists(str))
    {
      FeatureNotify ("EditByDefault",
                      "POV-Ray - Filename passed on command-line",
                      "Quoted filenames which are the sole command-line parameter are now "
                      "opened in the editor by default. If you wish to render them instead "
                      "please pass the /RENDER switch (or provide more than one parameter).\n\n"
                      "Click &Help for more information.",
                      "associations", false) ;
      add_edit_file(str);
      szCmdLine = "";
    }
  }

  if ((szCmdLine = preparse_commandline (szCmdLine)) != NULL)
  {
    if (benchmark_mode)
      demo_mode = false;
    if (parse_commandline (szCmdLine) > 1 && !demo_mode && !benchmark_mode)
    {
      restore_command_line = true ;
      strncpy (command_line, szCmdLine, sizeof (command_line) - 1) ;
    }
  }

  if (debugging)
    if (_getcwd (str, sizeof (str) - 1) != NULL)
      debug_output("CWD is %s\n", str) ;

  if (editors_enabled)
  {
    if (EditDLLPath != NULL)
    {
#ifdef _DEBUG
      // Prefer debug DLL, but don't complain if it's not available.
      sprintf(str, "%s" EDITDLLNAME_DEBUG, EditDLLPath);
      if (!LoadEditorDLL(str, true))
      {
#endif
        sprintf(str, "%s" EDITDLLNAME, EditDLLPath);
        if (!LoadEditorDLL (str, false))
          use_editors = false ;
#ifdef _DEBUG
      }
#endif
    }
    else
    {
#ifdef _DEBUG
      // Prefer debug DLL, but don't complain if it's not available.
      sprintf (str, "%s\\" EDITDLLNAME_DEBUG, modulePath);
      if (!LoadEditorDLL(str, true))
      {
        sprintf (str, "%sbin\\" EDITDLLNAME_DEBUG, BinariesPath) ;
        if (!LoadEditorDLL (str, true))
        {
#endif
          sprintf (str, "%s\\" EDITDLLNAME, modulePath) ;
          if (!LoadEditorDLL (str, true))
          {
            sprintf (str, "%sbin\\" EDITDLLNAME, BinariesPath) ;
            if (!LoadEditorDLL (str, false))
              use_editors = false ;
          }
#ifdef _DEBUG
        }
      }
#endif
    }
  }
  else
    use_editors = false;

  GetHKCU("General", VERSIONVAL, "[unknown]", str, (DWORD) strlen (str)) ;
  if (debugging)
    // TODO REVIEW - that's not data from the registry, that's data from `pvengine.ini`.
    debug_output("Registry records version %s, and we are %s\n", str, POV_RAY_VERSION) ;

  // In determining whether we want to set the newVersion flag, we don't care about build-specific
  // stuff like compiler version or even the CPU architecture.
  s = strstr(str, "+");
  if (s != NULL)
    *s = '\0';
  if (strcmp (str, POV_RAY_SOURCE_VERSION) != 0)
    newVersion = true ;

  if ((run_count = GetHKCU("General", "RunCount", 0)) == 0 || newVersion)
  {
    if (screen_depth < 8)
    {
      MessageBox (NULL,
                  "NOTE : POV-Ray for Windows was not designed to run in 16-color mode. "
                  "While the program will operate, it is recommended that you use a minimum "
                  "graphics mode of 800x600x256.",
                  "Warning - running in 16-color mode",
                  MB_ICONEXCLAMATION) ;
      tile_background = false ;
    }
    if (screen_width < 800)
    {
      MessageBox (NULL,
                  "NOTE : POV-Ray for Windows was not designed to run at less than 800x600.\n\n"
                  "While the program will operate, it is recommended that you use a minimum "
                  "graphics mode of 800x600x256.",
                  "Warning - running at less than 800x600",
                  MB_ICONEXCLAMATION) ;
    }
  }
  PutHKCU ("General", "RunCount", ++run_count) ;

  if (screen_depth < 8)
    tile_background = false ;

  /* Create the main window */
  placement = mainwin_placement ;
  placement.length = sizeof (WINDOWPLACEMENT) ;
  w = mainwin_placement.rcNormalPosition.right - mainwin_placement.rcNormalPosition.left ;
  h = mainwin_placement.rcNormalPosition.bottom - mainwin_placement.rcNormalPosition.top ;
  if (w <= 128)
    w = 700 ;
  if (h <= 128)
    h = screen_height - 75 ;
  main_window = CreateWindowEx (0,
                                PovMainWinClass,
                                "POV-Ray for Windows",
                                WS_OVERLAPPEDWINDOW,
                                mainwin_placement.rcNormalPosition.left,
                                mainwin_placement.rcNormalPosition.top,
                                w,
                                h,
                                NULL,
                                NULL,
                                hInst,
                                NULL) ;

  if (main_window == NULL)
  {
    MessageBox (NULL, "ERROR : Could not create main window.", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }

  EditSetNotifyBase (main_window, CM_FIRSTEDITNOTIFY) ;

  if ((StatusWindow = CreateStatusbar (main_window)) == NULL)
  {
    MessageBox (main_window, "ERROR : Could not create statusbar", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }

  if (tile_background && background_file [1])
  {
    if ((hDIB = LoadDIB (background_file)) != NULL)
    {
      hBmpBackground = DIBToBitmap (hDIB, hPalApp) ;
      DeleteObject (hDIB) ;
      GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
      background_width = bm.bmWidth ;
      background_height = bm.bmHeight ;
    }
    else
    {
      PovMessageBox ("Failed to load bitmap file", "Error") ;
      strcpy (background_file, "0") ;
    }
  }

  if (tile_background && hBmpBackground == NULL && screen_depth >= 8)
  {
    if (isdigit (background_file [0]) && background_file [1] == '\0')
      id = background_file [0] - '0' ;
    SendMessage (main_window, WM_COMMAND, CM_BACKGROUNDSTD + id, 1L) ;
  }

  if ((hBMP = LoadBitmap (hInstance, MAKEINTRESOURCE (BMP_ICON))) != NULL)
    hBmpIcon = hBMP ;

  if (lastBitmapPath [0] == '\0')
    sprintf (lastBitmapPath, "%sTiles", BinariesPath) ;
  if (lastRenderPath [0] == '\0')
  {
    sprintf (lastRenderPath, "%sScenes\\Advanced", DocumentsPath) ;
    strcpy (lastRenderName, "Biscuit.pov") ;
  }
  if (lastQueuePath [0] == '\0')
    sprintf (lastQueuePath, "%sScenes", DocumentsPath) ;
  GetHKCU("Editor", "LastPath", "", str, sizeof (str)) ;
  validatePath (lastRenderPath) ;
  if (str [0] == '\0')
    PutHKCU("Editor", "LastPath", lastRenderPath) ;
  if (lastRenderName [0] != '\0' && !demo_mode && !benchmark_mode)
    joinPath (source_file_name, lastRenderPath, lastRenderName) ;

  if (use_editors)
    if ((tab_window = InitialiseEditor (main_window, StatusWindow, BinariesPath, DocumentsPath)) == NULL)
      use_editors = false ;

  message_window = CreateWindowEx (WS_EX_CLIENTEDGE,
                                   PovMessageWinClass,
                                   "",
                                   WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                   0,
                                   0,
                                   0,
                                   0,
                                   use_editors ? tab_window : main_window,
                                   NULL,
                                   hInst,
                                   NULL) ;
  if (message_window == NULL)
  {
    MessageBox (NULL, "ERROR : Could not create message window.", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }

  if (initialise_message_display ())
  {
    cleanup_all () ;
    return (1) ;
  }
  EditSetMessageWindow (message_window) ;

  if ((rebar_window = create_rebar (main_window)) == NULL)
  {
    MessageBox (main_window, "ERROR : Could not create internal window #1", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }
  if ((toolbar_window = create_toolbar (rebar_window)) == NULL)
  {
    MessageBox (main_window, "ERROR : Could not create internal window #2", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }
  if (!use_toolbar)
  {
    ShowWindow (rebar_window, SW_HIDE) ;
    toolheight = 0 ;
  }
  extract_ini_sections_ex (SecondaryRenderIniFileName, toolbar_combobox) ;
  SendMessage (toolbar_combobox, CB_ADDSTRING, 0, (LPARAM) "More Resolutions ...") ;
  tb_combo_sel = select_combo_item_ex (toolbar_combobox, SecondaryRenderIniFileSection) ;
  if (tb_combo_sel == -1)
    tb_combo_sel = 0 ;

  setup_menus (use_editors) ;
  build_main_menu (hMainMenu, use_editors) ;

  set_toggles () ;

  if (!use_editors)
  {
    SendMessage (toolbar_window, TB_ENABLEBUTTON, CM_FILENEW, 0L) ;
    SendMessage (toolbar_window, TB_ENABLEBUTTON, CM_FILEOPEN, 0L) ;
    SendMessage (toolbar_window, TB_ENABLEBUTTON, CM_FILESAVE, 0L) ;
  }
  else
    EditRestoreState (!NoRestore) ;

  if (editors_enabled)
    PVCheckMenuItem (CM_USEEDITOR, MF_CHECKED) ;
  PVEnableMenuItem (CM_RENDERHIDE, RenderwinIsChild ? MF_GRAYED : MF_ENABLED) ;
  PVEnableMenuItem (CM_RENDERACTIVE, RenderwinIsChild ? MF_GRAYED : MF_ENABLED) ;
  PVCheckMenuItem (on_completion, MF_CHECKED) ;
  PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  PVEnableMenuItem (CM_RENDERSLEEP, MF_GRAYED) ;

  PVCheckMenuRadioItem (CM_RENDERPRIORITY_BACKGROUND, CM_RENDERPRIORITY_HIGH, render_priority) ;

  PVCheckMenuItem (drop_to_editor ? CM_DROPEDITOR : CM_DROPRENDERER, MF_CHECKED) ;

  PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
  PVModifyMenu (CM_TILEDBACKGROUND,
                MF_STRING,
                CM_TILEDBACKGROUND,
                tile_background ? "&Select Plain Background" : "&Select Tiled Background") ;
  PVCheckMenuRadioItem (CM_DUTYCYCLE_10, CM_DUTYCYCLE_100, CM_DUTYCYCLE_10 + Duty_Cycle) ;
  if (screen_depth < 8)
  {
    PVEnableMenuItem (CM_TILEDBACKGROUND, MF_GRAYED) ;
    PVEnableMenuItem (CM_BACKGROUNDBITMAP, MF_GRAYED) ;
    for (i = 0 ; i < 16 ; i++)
      PVEnableMenuItem (CM_BACKGROUNDSTD + i, MF_GRAYED) ;
  }
  set_newuser_menus (hide_newuser_help) ;

  if (ThreadCount == 1)
  {
    PVDeleteMenuItem (CM_BENCHMARK) ;
    PVModifyMenu (CM_BENCHMARKONETHREAD, MF_STRING, CM_BENCHMARKONETHREAD, "Run &Benchmark") ;
  }

  switch (placement.showCmd)
  {
    case SW_SHOWNORMAL :
         show_state = SW_SHOW ;
         break ;

    case SW_SHOWMINIMIZED :
//       show_state = SW_SHOWMINNOACTIVE ;
         show_state = SW_SHOW ;
         break ;

    case SW_SHOWMAXIMIZED :
         show_state = SW_SHOWMAXIMIZED ;
         break ;

    default :
         show_state = SW_SHOW ;
         break ;
  }

  if (ListenMode)
    show_state = SW_HIDE ;

  placement.showCmd = show_state ;
  placement.flags = (placement.ptMinPosition.x == -1 && placement.ptMinPosition.y == -1) ? 0 : WPF_SETMINPOSITION ;
  if (placement.rcNormalPosition.right <= 0 || placement.rcNormalPosition.bottom <= 0)
  {
    placement.rcNormalPosition.right = placement.rcNormalPosition.left + message_xchar * 115 ;
    placement.rcNormalPosition.bottom = placement.rcNormalPosition.top + message_ychar * 75 ;
  }

  placement.length = sizeof (WINDOWPLACEMENT) ;
  SetWindowPlacement (main_window, &placement) ;
  if (show_state != SW_SHOWMAXIMIZED)
    FitWindowInWindow (NULL, main_window) ;

  if ((timer_id = SetTimer (main_window, 1, 250, NULL)) != 0)
    DragAcceptFiles (main_window, true) ;

  // only needed for earlier versions of common control DLL ...
  if (use_toolbar && !IsComCtl5)
  {
    hDC = GetDC (toolbar_window) ;
    GetClientRect (toolbar_window, &rect) ;
    FillRect (hDC, &rect, (HBRUSH) GetStockObject (LTGRAY_BRUSH)) ;
    ReleaseDC (toolbar_window, hDC) ;
  }

  // fixes visual glitch with ComCtl6
  if (use_toolbar && IsComCtl6)
  {
    hDC = GetDC (main_window) ;
    GetClientRect (rebar_window, &rect) ;
    FillRect (hDC, &rect, (HBRUSH) GetStockObject (LTGRAY_BRUSH)) ;
    ReleaseDC (main_window, hDC) ;
  }

  if (ExtensionsEnabled)
    LoadGUIExtensions () ;

  if (renderwin_left < 0 || renderwin_left > screen_width - 32 || renderwin_top < 0 || renderwin_top > screen_height - 32)
  {
    MONITORINFO mi;
    mi.cbSize = sizeof(MONITORINFO);
    if (GetMonitorInfo(MonitorFromWindow(main_window, MONITOR_DEFAULTTOPRIMARY), &mi))
    {
      // apply the principle of least astonishment: otherwise the render window could
      // turn up on a monitor which may be switched off or otherwise not being paid
      // attention to, with no (obvious) way for the user to move it. we make one
      // exception: if the render window is placed on the primary display, we won't
      // move it to the current display, since it's assumed the primary display is
      // always going to be visible.
        if (renderwin_left < mi.rcWork.left)
          renderwin_left = mi.rcWork.left ;
        if (renderwin_top < mi.rcWork.top)
          renderwin_top = mi.rcWork.top ;
        if (renderwin_left > mi.rcWork.right - 128)
          renderwin_left = mi.rcWork.right - 128 ;
        if (renderwin_top > mi.rcWork.bottom - 128)
          renderwin_top = mi.rcWork.bottom - 128 ;
    }
  }

  buffer_message (mIDE, "Persistence of Vision Raytracer(tm) for Windows.\n") ;
  buffer_message (mIDE, "POV-Ray for Windows is part of the POV-Ray(tm) suite of programs.\n") ;
  buffer_message (mIDE, "  This is version " POV_RAY_VERSION_INFO ".\n") ;
  buffer_message (mIDE, POV_RAY_COPYRIGHT "\n") ;
  buffer_message (mIDE, "  " DISCLAIMER_MESSAGE_1 "\n") ;
  buffer_message (mIDE, "  " DISCLAIMER_MESSAGE_2 "\n") ;
  buffer_message (mIDE, "  Select Help|About (or press Alt+B) for more information and a copy of the license.\n") ;
  buffer_message (mIDE, "The terms POV-Ray and Persistence of Vision Raytracer are trademarks of\n") ;
  buffer_message (mIDE, "  Persistence of Vision Raytracer Pty. Ltd.\n") ;
  if (render_bitmap_depth != 24)
  {
    buffer_message (mIDE, "\n") ;
    buffer_message (mIDE, renderwin_8bits ? "Using 8-bit dithered internal bitmap (menu setting)\n" :
                                            "Using 8-bit dithered internal bitmap (4 or 8-bit video mode)\n") ;
  }

  buffer_message (mIDE, "\n") ;
  strcpy (tool_commands [0], "notepad.exe \"%ipovray.ini\"") ;

#if !POV_RAY_IS_OFFICIAL
  WIN_PrintOtherCredits () ;
  buffer_message (mIDE, "This unofficial build is derived from the POV-Ray for Windows source code.\n") ;
#endif
  buffer_message (mDivider, "\n") ;

#ifdef TRY_OPTIMIZED_NOISE
  // TODO FIXME
  // technically we should ask the backend what it's using, but given this is not a remoted version
  // of POVWIN, we just call the test here.
  const OptimizedNoiseInfo* pNoise = GetRecommendedOptimizedNoise();
  selectedNoiseFunc = pNoise->name;
#endif // TRY_OPTIMIZED_NOISE

  load_tool_menu (ToolIniFileName) ;
  if (GetHKCU("FileQueue", "ReloadOnStartup", 0))
  {
    queued_file_count = GetHKCU("FileQueue", "QueueCount", 0) ;
    if (queued_file_count > MAX_QUEUE)
      queued_file_count = MAX_QUEUE ;
    for (i = 0 ; i < queued_file_count ; i++)
    {
      sprintf (str, "QueuedFile%d", i) ;
      GetHKCU("FileQueue", str, "", queued_files [i], sizeof (queued_files [0])) ;
    }
    if (queued_file_count != 0)
      message_printf ("Loaded %d entr%s into file queue\n", queued_file_count, queued_file_count == 1 ? "y" : "ies") ;
    update_queue_status (false) ;
  }

  buffer_message (mDivider, "\n") ;

  if (GetHKCU("General", "CheckColorsInc", 1) == 1)
  {
    sprintf (str, "%sinclude\\colors.inc", DocumentsPath) ;
    if (stat (str, &statbuf) != 0)
    {
      char temp[2048];
      sprintf(temp,
              "WARNING : Cannot find COLORS.INC in expected location:\n\n%s\n\n"
              "This file is important for the normal operation of POV-Ray. It is included "
              "with the POV-Ray for Windows distribution. If you did not install using the "
              "correct installation procedure please attend to this before running POV-Ray "
              "for Windows.\n\nIf, however, you have chosen to change the location of this file "
              "or do not need it, you may ignore this warning as long as you have updated "
              "POVRAY.INI to the new path, or do not use any standard scenes that require it.\n\n"
              "Do you want to see this warning again ?",
              str);
      if (MessageBox (NULL, temp, "Warning - COLORS.INC is missing", MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
        PutHKCU ("General", "CheckColorsInc", 0U) ;
    }
  }

  if (demo_mode)
  {
    message_printf ("Running demonstration\n") ;
    argc = 0 ;
    handle_main_command (CM_DEMO, 0) ;
  }

  if (benchmark_mode)
  {
    message_printf ("Running benchmark\n") ;
    argc = 0 ;
    handle_main_command (CM_BENCHMARK, 0) ;
  }

  SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;

  if (debugging)
  {
    message_printf ("My window handle is %p\n", main_window) ;

    if (HaveWin95OrLater ())
      message_printf ("Win95 or later detected\n") ;
    if (HaveWin98OrLater ())
      message_printf ("Win98 or later detected\n") ;
    if (HaveNT4OrLater ())
      message_printf ("WinNT or later detected\n") ;
    if (HaveWin2kOrLater ())
      message_printf ("Win2k or later detected\n") ;
    if (HaveWinXPOrLater ())
      message_printf ("WinXP or later detected\n") ;
    if (IsW95UserInterface)
      message_printf ("Windows 95 user interface flag is set\n") ;
  }

  for (i = 0 ; i < EditFileCount ; i++)
  {
    if (EditGetFlags () & EDIT_CAN_OPEN)
      EditOpenFile (EditFiles [i]) ;
    free (EditFiles [i]) ;
    EditFiles [i] = NULL ;
  }
  EditFileCount = 0 ;

  if (newVersion || FreshInstall)
  {
    if (EditGetFlags() & EDIT_CAN_OPEN)
    {
      sprintf(str, "%srevision.txt", DocumentsPath);
      if (fileExists(str))
        EditOpenFile(str) ;
      if (EditGetFlags() & EDIT_CAN_OPEN)
      {
        sprintf(str, "%schanges.txt", DocumentsPath);
        if (fileExists(str))
          EditOpenFile(str) ;
      }
    }

    // TODO: remove this after a few betas
    tile_background = false;
    text_colours[0] = RGB (255, 255, 255);
    text_colours[1] = RGB (255, 255, 0);
    text_colours[2] = RGB (0, 255, 255);
    background_colour = RGB (31, 0, 63);
  }

  if (run_count > 1 && !demo_mode && !benchmark_mode)
  {
    n = GetHKCU("General", "ItsAboutTime", 0) ;
    if (_time32 (NULL) > n /*|| newVersion*/)
    {
      ShowAboutBox () ;
      PutHKCU ("General", "ItsAboutTime", n ? _time32 (NULL) + DAYS(14) : _time32 (NULL) + DAYS(1)) ;
    }
  }

#if POV_RAY_HAS_UPDATE_CHECK
  DoUpdateCheck () ;
#endif

  // automatically call the rendering engine if there were any parameters on the command line
  if (!rendering && (argc > 1 || render_requested))
  {
    if (render_requested)
    {
      wrapped_printf ("Requested render file is '%s'", requested_render_file) ;
      strcpy (source_file_name, requested_render_file) ;
    }
    if (argc > 1)
      wrapped_printf ("Calling rendering engine with parameters '%s'", command_line) ;
    start_rendering (!render_requested) ;
  }

  if (homeInferred)
    message_printf ("Warning: running with inferred home path due to missing registry entry.\n\n") ;

#ifdef _DEBUG
  if (sizeof (ExternalVarStruct) != 0x9350)
    PovMessageBox ("Compatibility problem - ExternalVarStruct has changed size", "Warning") ;
#endif

  if (ListenMode)
    handle_main_command (CM_SHOWMAINWINDOW, 0) ;

  if (debugging)
    debug_output("Entering GetMessage loop\n") ;

  try
  {
    while (!exit_loop)
    {
      // since the render thread can really slow things down for the UI (this becomes
      // a problem if the user sets the render priority to high and then finds the
      // UI unresponsive when attempting to change it back), we only sleep here if
      // there's no events in the queue that we feel the need to handle immediately.
      // (if one of these messages turns up while we're sleeping we'll get woken).
      // N.B. longer wait times resulted in complaints from some testers, apparently
      // there's some machines out there that don't exit the wait for some types of
      // input.
      MsgWaitForMultipleObjects (0, NULL, FALSE, 10, QS_ALLINPUT) ;
  //  MsgWaitForMultipleObjects (0, NULL, FALSE, 10, QS_HOTKEY | QS_KEY | QS_MOUSEBUTTON) ;

      if (StartInsertRender)
      {
        if (!rendering)
          start_rendering (false) ;
        StartInsertRender = false ;
      }

      if (quit != 0)
        if (quit + 15 < time (NULL))
          break ;

      ProcessSession();
      while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
      {
        if (msg.message == WM_QUIT)
        {
          exit_loop = true ;
          break ;
        }

        // we have to disable all these calls because HTML Help has a bug (on some
        // platforms - I can't work out the exact conditions that trigger it) which
        // will cause the cursor to flash between normal and the busy state (normally
        // an hourglass) several times a second, continually (even if help isn't open).
        // See job #124.
    #ifdef HTMLHELP_FIXED
        if (HtmlHelp (NULL, NULL, HH_PRETRANSLATEMESSAGE, (DWORD) &msg))
          continue ;
    #else
        // we need to pass on these messages, otherwise help navigation messages
        // will go to us instead of the help window
        if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN)
          if (!IsChild (main_window, msg.hwnd))
            if (HtmlHelp (NULL, NULL, HH_PRETRANSLATEMESSAGE, (DWORD_PTR) &msg))
              continue ;
    #endif
        if (!TranslateAccelerator (main_window, hAccelerators, &msg))
        {
          TranslateMessage (&msg) ;
          if (msg.hwnd == toolbar_cmdline)
          {
            if (msg.message == WM_CHAR)
              if (handle_toolbar_cmdline (msg.wParam, msg.lParam))
                continue ;
          }
          DispatchMessage (&msg) ;
        }
        ProcessSession();
      }
    }
  }
  catch(std::exception& e)
  {
    sprintf (str, "Caught exception: %s", e.what()) ;
    if (debugging)
      debug_output ("%s\n", str) ;
    MessageBox (NULL, str, "POV-Ray Critical Error", MB_ICONSTOP) ;
    std::exit(1);
  }

  if (debugging)
    debug_output ("Dropping out of message loop\n") ;

  try
  {
    DeleteFrontend();
    cleanup_all () ;
  }
  catch(pov_base::Exception& e)
  {
    // don't do much about POV exceptions here: often, they will relate to the front end
    // not cleaning up fast enough (i.e. due to a lot of memory de-allocation)
    if (debugging)
      debug_output ("Caught exception: %s\n", e.what()) ;
  }
  catch(std::exception& e)
  {
    sprintf (str, "Caught exception: %s", e.what()) ;
    if (debugging)
      debug_output ("%s\n", str) ;
    MessageBox (NULL, str, "POV-Ray Critical Error", MB_ICONSTOP) ;
  }

  _fcloseall () ;
#ifndef _WIN64
  // win64 - get exception during HH_UNINITIALIZE
  HtmlHelp (NULL, NULL, HH_UNINITIALIZE, (DWORD) help_cookie) ;
#endif

  if (debugging)
    debug_output ("exiting WinMain()\n") ;
  return (0) ;
}
