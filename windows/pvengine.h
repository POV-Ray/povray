//******************************************************************************
///
/// @file windows/pvengine.h
///
/// This file contains PVENGINE specific defines.
///
/// @author Christopher J. Cason.
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

#ifndef PVENGINE_H_INCLUDED
#define PVENGINE_H_INCLUDED

#ifdef BUILDING_AMD64
  #if !defined(_M_AMD64) && !defined(_M_X64)
    #error "you are compiling the x64 project using a 32-bit compiler"
  #endif
#else
  #if defined(_M_AMD64) || defined(_M_X64)
    #error "you are compiling the 32-bit project using a 64-bit compiler"
  #endif
#endif

#ifdef _CONSOLE
#error "You are building the GUI platform with _CONSOLE defined (check windows/povconfig/syspovconfig.h)."
#endif

#include <string.h>
#include <malloc.h>
#include <direct.h>
#include <io.h>
#include <process.h>
#include <assert.h>
#include <sys/stat.h>
#include <excpt.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "pvfrontend.h"
#include "backend/povray.h"

#include <time.h>

#define MAX_MESSAGE               1024
#define MAX_ARGV                  256
#define MAX_TOOLCMD               32
#define MAX_TOOLCMDTEXT           128
#define MAX_TOOLHELPTEXT          128
#define MAX_WINDOWS               16
#define MAX_EDIT_FILES            32
#define POV_INTERNAL_STREAM       ((FILE *) 1L)

#define EDIT_FILE                 1
#define RENDER_FILE               2

#if POV_RAY_IS_OFFICIAL
  #ifdef _WIN64
    #define CLASSNAMEPREFIX "Pov" POV_RAY_MAJOR_VERSION POV_RAY_MINOR_VERSION "-Win64-"
  #else
    #define CLASSNAMEPREFIX "Pov" POV_RAY_MAJOR_VERSION POV_RAY_MINOR_VERSION "-Win32-"
  #endif
#else
  #ifdef _WIN64
    #define CLASSNAMEPREFIX "Unofficial-Pov" POV_RAY_MAJOR_VERSION POV_RAY_MINOR_VERSION "-Win64-"
  #else
    #define CLASSNAMEPREFIX "Unofficial-Pov" POV_RAY_MAJOR_VERSION POV_RAY_MINOR_VERSION "-Win32-"
  #endif
#endif

#ifdef DEVELOPMENT
  #ifdef _WIN64
    #define CAPTIONPREFIX "[WIN64]"
  #else
    #define CAPTIONPREFIX "[WIN32]"
  #endif
#else
  #define CAPTIONPREFIX ""
#endif

#ifdef _WIN64
  #define INSTALLTIMEKEY        "InstallTime64"
  // NB: We're using the standard editor DLLs regardless of architecture optimization (e.g. AVX)
  #define EDITDLLNAME           "cmedit64.dll"
  #define EDITDLLNAME_DEBUG     "cmedit64d.dll"
  #define VERSIONVAL            POVWIN_BETA_PREFIX "VersionNo64"
#else
  #define INSTALLTIMEKEY        "InstallTime32"
  // TODO - use the standard editor DLLs regardless of architecture optimization (e.g. SSE2)
  #ifdef BUILD_SSE2
    #define EDITDLLNAME         "cmedit32-sse2.dll"
  #else
    #define EDITDLLNAME         "cmedit32.dll"
  #endif
  #define EDITDLLNAME_DEBUG     "cmedit32d.dll"
  #define VERSIONVAL            POVWIN_BETA_PREFIX "VersionNo32"
#endif

// ----------------------------------------------------------------------
// message definitions used to be here but have been moved to pvedit.h.
// ----------------------------------------------------------------------

#define HDIB            HANDLE
#define SEPARATOR       '\\'

#define DRAWFASTRECT(hdc,lprc) ExtTextOut(hdc,0,0,ETO_OPAQUE,lprc,NULL,0,NULL)

namespace povwin
{

// WARNING: also declared in pvguiext.h (for a reason)
// KEEP THESE IN SYNC
typedef enum
{
  mUnknown = 0,
  mAll = 1,
  All = 1,
  mIDE,
  mBanner,
  mWarning,
  mRender,
  mStatus,
  mDebug,
  mFatal,
  mStatistics,
  mDivider,
  mHorzLine,
} msgtype ;

typedef enum
{
  None,
  CR,
  LF
} lftype ;

typedef enum
{
  filePOV,
  fileINC,
  fileINI,
  fileFirstImageType,
  fileTGA = fileFirstImageType,
  filePPM,
  filePGM,
  filePBM,
  filePNG,
  fileGIF,
  fileBMP,
  fileEXR,
  fileLastImageType = fileEXR,
  fileUnknown
} FileType ;

// Bitmap header info with palette included

typedef struct
{
  BITMAPINFOHEADER      header ;
  RGBQUAD               colors [256] ;
} BitmapInfo ;

// Windows LOGPALETTE palette structure

typedef struct
{
  WORD                  version ;
  WORD                  entries ;
  PALETTEENTRY          pe [256] ;
} LogPal ;

inline int MulDivNoRound (int value, int mul_by, int div_by)
{
  return ((int) ((__int64) value * mul_by / div_by)) ;
}

extern int              delay_next_status ;
extern char             EngineIniFileName[];
extern char             BinariesPath[];
extern char             DocumentsPath[];
extern char             FontPath[];
extern char             status_buffer [1024] ;
extern HINSTANCE        hInstance;

void ShowIsPaused(void);
void SetCaption (LPCSTR str) ;
void debug_output (const char *format, ...) ;
void PovMessageBox (const char *message, char *title) ;
int initialise_message_display (void) ;
void erase_display_window (HDC hdc, int xoffset, int yoffset) ;
void paint_display_window (HDC hdc) ;
void buffer_message (msgtype message_type, const char *s, bool addLF = false) ;
void buffer_stream_message (msgtype message_type, const char *s) ;
void clear_messages (bool print = true) ;
int update_message_display (lftype lf) ;
void message_printf (const char *format, ...) ;
void wrapped_printf (const char *format, ...) ;
void dump_pane_to_clipboard (void) ;
bool copy_text_to_clipboard(const char *text);
void get_logfont (HDC hdc, LOGFONT *lf) ;
int create_message_font (HDC hdc, LOGFONT *lf) ;
void status_printf (int nSection, const char *format, ...) ;
void SetupExplorerDialog (HWND win) ;
char *findFirstPathSeparator (char *s) ;
char *findLastPathSeparator (char *s) ;
void appendPathSeparator (char *s) ;
bool hasTrailingPathSeparator (const char *s) ;
void trimTrailingPathSeparator (char *s) ;
void validatePath (char *s) ;
int joinPath (char *out, const char *path, const char *name) ;
void CalculateClientWindows (bool redraw) ;
bool start_rendering (bool ignore_source_file) ;
void render_stopped (void) ;
void cancel_render (void) ;
void SetStatusPanelItemText (int id, LPCSTR format, ...) ;
bool handle_main_command (WPARAM wParam, LPARAM lParam) ;
char *file_open (HWND hWnd) ;
bool OkToStopRendering (void);
bool reg_printf (bool useHKCU, char *keyName, char *valName, char *format, ...);
static inline bool isPathSeparator (char c) { return (c == '\\' || c == '/'); }

// file PVMISC.C

FileType get_file_type (const char *filename) ;
bool is_non_primary_file(const char *filename) ;
void read_INI_settings (void) ;
void write_INI_settings (bool noreset = false) ;
void cloneOldIni(const std::string& oldPath, const std::string& newPath);
void update_menu_for_render (bool rendering) ;
void update_queue_status (bool write_files) ;
void draw_ordinary_listbox (DRAWITEMSTRUCT *d, bool fitpath) ;
void resize_listbox_dialog (HWND hDlg, int idLb, int chars) ;
void CenterWindowRelative (HWND hRelativeTo, HWND hTarget, bool bRepaint, bool checkBorders) ;
void FitWindowInWindow (HWND hRelativeTo, HWND hTarget) ;
int splitfn (const char *filename, char *path, char *name, char *ext) ;
void splitpath (const char *filename, char *path, char *name) ;
bool process_toggles (WPARAM wParam) ;
void set_toggles (void) ;
void load_tool_menu (char *iniFilename) ;
char *parse_tool_command (char *command) ;
char *get_elapsed_time (int seconds) ;
void extract_ini_sections (char *filename, HWND hwnd) ;
void extract_ini_sections_ex (char *filename, HWND hwnd) ;
int select_combo_item_ex (HWND hwnd, char *s) ;
char *get_full_name (char *s) ;
bool PovInvalidateRect (HWND hWnd, CONST RECT *lpRect, bool bErase) ;
bool TaskBarAddIcon (HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip) ;
bool TaskBarModifyIcon (HWND hwnd, UINT uID, LPSTR lpszTip) ;
bool TaskBarDeleteIcon (HWND hwnd, UINT uID) ;
char *clean (char *s) ;
bool fileExists (const char *filename) ;
bool dirExists (const char *filename) ;
bool GetDontShowAgain (const char *Name) ;
void PutDontShowAgain (const char *Name, bool dontShow) ;
void clear_dir_restrictions (void) ;
bool PutHKCU(const char *Section, const char *Name, const char *Value);
bool PutHKCU(const char *Section, const char *Name, const std::string& Value);
bool PutHKCU(const char *Section, const char *Name, unsigned Value);
unsigned GetHKCU(const char *Section, const char *Name, unsigned DefaultValue);
size_t GetHKCU(const char *Section, const char *Name, const char *DefaultValue, char *Buffer, unsigned MaxLength);

// file PVFILES.C

INT_PTR CALLBACK PovLegalDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
char *save_demo_file (char *s1, char *s2) ;
void save_povlegal (void) ;

// file PVMENU.C

bool PVEnableMenuItem (UINT idItem, UINT state) ;
bool PVCheckMenuItem (UINT idItem, UINT state) ;
bool PVCheckMenuRadioItem (UINT idFirst, UINT idLast, UINT idItem) ;
bool PVModifyMenu (UINT idItem, UINT flags, UINT idNewItem, LPCSTR lpNewItem) ;
bool PVDeleteMenuItem (UINT idItem) ;
void init_menus (void) ;
void setup_menus (bool have_editor) ;
void clear_menu (HMENU hMenu) ;
void build_main_menu (HMENU hMenu, bool have_editor) ;
void build_editor_menu (HMENU hMenu) ;
void set_newuser_menus (bool hide) ;
int find_menuitem (HMENU hMenu, LPCSTR title) ;

// file PVTEXT.C

void write_wrapped_text (HDC hdc, RECT *rect, const char *text) ;
void tip_of_the_day (HDC hdc, RECT *rect, char *text) ;
void say_status_message (int section, const char *message) ;
void handle_menu_select (WPARAM wParam, LPARAM lParam) ;
char *clean_str (const char *s) ;
HWND create_toolbar (HWND hwndParent) ;
void resize_windows (unsigned left, unsigned top, unsigned width, unsigned height) ;
char *preparse_commandline (char *s) ;
char *preparse_instance_commandline (char *s) ;
void add_edit_file (char *file);
char *extract_file (char *filename, char *s);
int need_hscroll (void) ;
HWND create_rebar (HWND hwndParent) ;
HWND CreateStatusbar (HWND hwndParent) ;
void ResizeStatusBar (HWND hwnd) ;
bool HandleStatusTooltip(NMHDR *nmh);

// file PVBITMAP.CPP

HDIB      FAR  BitmapToDIB (HBITMAP hBitmap, HPALETTE hPal);
HDIB      FAR  ChangeBitmapFormat (HBITMAP  hBitmap,
                                   WORD     wBitCount,
                                   DWORD    dwCompression,
                                   HPALETTE hPal);
HDIB      FAR  ChangeDIBFormat (HDIB hDIB, WORD wBitCount,
                                DWORD dwCompression);
HBITMAP   FAR  CopyScreenToBitmap (LPRECT);
HDIB      FAR  CopyScreenToDIB (LPRECT);
HBITMAP   FAR  CopyWindowToBitmap (HWND, WORD);
HDIB      FAR  CopyWindowToDIB (HWND, WORD);
HPALETTE  FAR  CreateDIBPalette (HDIB hDIB);
HDIB      FAR  CreateDIB(DWORD, DWORD, WORD);
WORD      FAR  DestroyDIB (HDIB);
DWORD     FAR  DIBHeight (LPSTR lpDIB);
WORD      FAR  DIBNumColors (LPSTR lpDIB);
HBITMAP   FAR  DIBToBitmap (HDIB hDIB, HPALETTE hPal);
DWORD     FAR  DIBWidth (LPSTR lpDIB);
LPSTR     FAR  FindDIBBits (LPSTR lpDIB);
HPALETTE  FAR  GetSystemPalette (void);
HDIB      FAR  LoadDIB (LPSTR);
bool      FAR  PaintBitmap (HDC, LPRECT, HBITMAP, LPRECT, HPALETTE);
bool      FAR  PaintDIB (HDC, LPRECT, HDIB, LPRECT, HPALETTE);
int       FAR  PalEntriesOnDevice (HDC hDC);
WORD      FAR  PaletteSize (LPSTR lpDIB);
WORD      FAR  PrintDIB (HDIB, WORD, WORD, WORD, LPSTR);
WORD      FAR  PrintScreen (LPRECT, WORD, WORD, WORD, LPSTR);
WORD      FAR  PrintWindow (HWND, WORD, WORD, WORD, WORD, LPSTR);
WORD      FAR  SaveDIB (HDIB, LPSTR);
HANDLE         AllocRoomForDIB(BITMAPINFOHEADER bi, HBITMAP hBitmap);
HBITMAP        lpDIBToBitmap(void *lpDIBHdr, HPALETTE hPal);
HBITMAP        lpDIBToBitmapAndPalette(void *lpDIBHdr);

// file PVUPDATE.CPP

bool InternetConnected(void);
int IsUpdateAvailable (bool SendSysinfo, char *CurrentVersion, std::string& NewVersion, std::string& Info);

#ifdef DECLARE_TABLES

// Default windows compatible halftone palette. This includes the default
// Windows system colors in the first 10 and last 10 entries in the
// palette.

RGBQUAD halftonePal [256] =
{
  {0x00,0x00,0x00,0}, {0xA8,0x00,0x00,0}, {0x00,0xA8,0x00,0}, {0xA8,0xA8,0x00,0},
  {0x00,0x00,0xA8,0}, {0xA8,0x00,0xA8,0}, {0x00,0x54,0xA8,0}, {0xA8,0xA8,0xA8,0},
  {0x54,0x54,0x54,0}, {0xFC,0x54,0x54,0}, {0x54,0xFC,0x54,0}, {0xFC,0xFC,0x54,0},
  {0x54,0x54,0xFC,0}, {0xFC,0x54,0xFC,0}, {0x54,0xFC,0xFC,0}, {0xFC,0xFC,0xFC,0},
  {0x00,0x00,0x00,0}, {0x14,0x14,0x14,0}, {0x20,0x20,0x20,0}, {0x2C,0x2C,0x2C,0},
  {0x00,0x00,0x00,0}, {0x00,0x00,0x33,0}, {0x00,0x00,0x66,0}, {0x00,0x00,0x99,0},
  {0x00,0x00,0xCC,0}, {0x00,0x00,0xFF,0}, {0x00,0x33,0x00,0}, {0x00,0x33,0x33,0},
  {0x00,0x33,0x66,0}, {0x00,0x33,0x99,0}, {0x00,0x33,0xCC,0}, {0x00,0x33,0xFF,0},
  {0x00,0x66,0x00,0}, {0x00,0x66,0x33,0}, {0x00,0x66,0x66,0}, {0x00,0x66,0x99,0},
  {0x00,0x66,0xCC,0}, {0x00,0x66,0xFF,0}, {0x00,0x99,0x00,0}, {0x00,0x99,0x33,0},
  {0x00,0x99,0x66,0}, {0x00,0x99,0x99,0}, {0x00,0x99,0xCC,0}, {0x00,0x99,0xFF,0},
  {0x00,0xCC,0x00,0}, {0x00,0xCC,0x33,0}, {0x00,0xCC,0x66,0}, {0x00,0xCC,0x99,0},
  {0x00,0xCC,0xCC,0}, {0x00,0xCC,0xFF,0}, {0x00,0xFF,0x00,0}, {0x00,0xFF,0x00,0},
  {0x00,0xFF,0x66,0}, {0x00,0xFF,0x99,0}, {0x00,0xFF,0xCC,0}, {0x00,0xFF,0xFF,0},
  {0x33,0x00,0x00,0}, {0x33,0x00,0x33,0}, {0x33,0x00,0x66,0}, {0x33,0x00,0x99,0},
  {0x33,0x00,0xCC,0}, {0x33,0x00,0xFF,0}, {0x33,0x33,0x00,0}, {0x33,0x33,0x33,0},
  {0x33,0x33,0x66,0}, {0x33,0x33,0x99,0}, {0x33,0x33,0xCC,0}, {0x33,0x33,0xFF,0},
  {0x33,0x66,0x00,0}, {0x33,0x66,0x33,0}, {0x33,0x66,0x66,0}, {0x33,0x66,0x99,0},
  {0x33,0x66,0xCC,0}, {0x33,0x66,0xFF,0}, {0x33,0x99,0x00,0}, {0x33,0x99,0x33,0},
  {0x33,0x99,0x66,0}, {0x33,0x99,0x99,0}, {0x33,0x99,0xCC,0}, {0x33,0x99,0xFF,0},
  {0x33,0xCC,0x00,0}, {0x33,0xCC,0x33,0}, {0x33,0xCC,0x66,0}, {0x33,0xCC,0x99,0},
  {0x33,0xCC,0xCC,0}, {0x33,0xCC,0xFF,0}, {0x00,0xFF,0x00,0}, {0x33,0xFF,0x33,0},
  {0x33,0xFF,0x66,0}, {0x33,0xFF,0x99,0}, {0x33,0xFF,0xCC,0}, {0x33,0xFF,0xFF,0},
  {0x66,0x00,0x00,0}, {0x66,0x00,0x33,0}, {0x66,0x00,0x66,0}, {0x66,0x00,0x99,0},
  {0x66,0x00,0xCC,0}, {0x66,0x00,0xFF,0}, {0x66,0x33,0x00,0}, {0x66,0x33,0x33,0},
  {0x66,0x33,0x66,0}, {0x66,0x33,0x99,0}, {0x66,0x33,0xCC,0}, {0x66,0x33,0xFF,0},
  {0x66,0x66,0x00,0}, {0x66,0x66,0x33,0}, {0x66,0x66,0x66,0}, {0x66,0x66,0x99,0},
  {0x66,0x66,0xCC,0}, {0x66,0x66,0xFF,0}, {0x66,0x99,0x00,0}, {0x66,0x99,0x33,0},
  {0x66,0x99,0x66,0}, {0x66,0x99,0x99,0}, {0x66,0x99,0xCC,0}, {0x66,0x99,0xFF,0},
  {0x66,0xCC,0x00,0}, {0x66,0xCC,0x33,0}, {0x66,0xCC,0x66,0}, {0x66,0xCC,0x99,0},
  {0x66,0xCC,0xCC,0}, {0x66,0xCC,0xFF,0}, {0x66,0xFF,0x00,0}, {0x66,0xFF,0x33,0},
  {0x66,0xFF,0x66,0}, {0x66,0xFF,0x99,0}, {0x66,0xFF,0xCC,0}, {0x66,0xFF,0xFF,0},
  {0x99,0x00,0x00,0}, {0x99,0x00,0x33,0}, {0x99,0x00,0x66,0}, {0x99,0x00,0x99,0},
  {0x99,0x00,0xCC,0}, {0x99,0x00,0xFF,0}, {0x99,0x33,0x00,0}, {0x99,0x33,0x33,0},
  {0x99,0x33,0x66,0}, {0x99,0x33,0x99,0}, {0x99,0x33,0xCC,0}, {0x99,0x33,0xFF,0},
  {0x99,0x66,0x00,0}, {0x99,0x66,0x33,0}, {0x99,0x66,0x66,0}, {0x99,0x66,0x99,0},
  {0x99,0x66,0xCC,0}, {0x99,0x66,0xFF,0}, {0x99,0x99,0x00,0}, {0x99,0x99,0x33,0},
  {0x99,0x99,0x66,0}, {0x99,0x99,0x99,0}, {0x99,0x99,0xCC,0}, {0x99,0x99,0xFF,0},
  {0x99,0xCC,0x00,0}, {0x99,0xCC,0x33,0}, {0x99,0xCC,0x66,0}, {0x99,0xCC,0x99,0},
  {0x99,0xCC,0xCC,0}, {0x99,0xCC,0xFF,0}, {0x99,0xFF,0x00,0}, {0x99,0xFF,0x33,0},
  {0x99,0xFF,0x66,0}, {0x99,0xFF,0x99,0}, {0x99,0xFF,0xCC,0}, {0x99,0xFF,0xFF,0},
  {0xCC,0x00,0x00,0}, {0xCC,0x00,0x33,0}, {0xCC,0x00,0x66,0}, {0xCC,0x00,0x99,0},
  {0xCC,0x00,0xCC,0}, {0xCC,0x00,0xFF,0}, {0xCC,0x33,0x00,0}, {0xCC,0x33,0x33,0},
  {0xCC,0x33,0x66,0}, {0xCC,0x33,0x99,0}, {0xCC,0x33,0xCC,0}, {0xCC,0x33,0xFF,0},
  {0xCC,0x66,0x00,0}, {0xCC,0x66,0x33,0}, {0xCC,0x66,0x66,0}, {0xCC,0x66,0x99,0},
  {0xCC,0x66,0xCC,0}, {0xCC,0x66,0xFF,0}, {0xCC,0x99,0x00,0}, {0xCC,0x99,0x33,0},
  {0xCC,0x99,0x66,0}, {0xCC,0x99,0x99,0}, {0xCC,0x99,0xCC,0}, {0xCC,0x99,0xFF,0},
  {0xCC,0xCC,0x00,0}, {0xCC,0xCC,0x33,0}, {0xCC,0xCC,0x66,0}, {0xCC,0xCC,0x99,0},
  {0xCC,0xCC,0xCC,0}, {0xCC,0xCC,0xFF,0}, {0xCC,0xFF,0x00,0}, {0xCC,0xFF,0x33,0},
  {0xCC,0xFF,0x66,0}, {0xCC,0xFF,0x99,0}, {0xCC,0xFF,0xCC,0}, {0xCC,0xFF,0xFF,0},
  {0xFF,0x00,0x00,0}, {0xFF,0x00,0x00,0}, {0xFF,0x00,0x66,0}, {0xFF,0x00,0x99,0},
  {0xFF,0x00,0xCC,0}, {0xFF,0x00,0xFF,0}, {0xFF,0x00,0x00,0}, {0xFF,0x33,0x33,0},
  {0xFF,0x33,0x66,0}, {0xFF,0x33,0x99,0}, {0xFF,0x33,0xCC,0}, {0xFF,0x33,0xFF,0},
  {0xFF,0x66,0x00,0}, {0xFF,0x66,0x33,0}, {0xFF,0x66,0x66,0}, {0xFF,0x66,0x99,0},
  {0xFF,0x66,0xCC,0}, {0xFF,0x66,0xFF,0}, {0xFF,0x99,0x00,0}, {0xFF,0x99,0x33,0},
  {0xFF,0x99,0x66,0}, {0xFF,0x99,0x99,0}, {0xFF,0x99,0xCC,0}, {0xFF,0x99,0xFF,0},
  {0xFF,0xCC,0x00,0}, {0xFF,0xCC,0x33,0}, {0xFF,0xCC,0x66,0}, {0xFF,0xCC,0x99,0},
  {0xFF,0xCC,0xCC,0}, {0xFF,0xCC,0xFF,0}, {0xFF,0xFF,0x00,0}, {0xFF,0xFF,0x33,0},
  {0xFF,0xFF,0x66,0}, {0xFF,0xFF,0x99,0}, {0xFF,0xFF,0xCC,0}, {0xFF,0xFF,0xFF,0},
  {0x2C,0x40,0x40,0}, {0x2C,0x40,0x3C,0}, {0x2C,0x40,0x34,0}, {0x2C,0x40,0x30,0},
  {0x2C,0x40,0x2C,0}, {0x30,0x40,0x2C,0}, {0x34,0x40,0x2C,0}, {0x3C,0x40,0x2C,0},
  {0x40,0x40,0x2C,0}, {0x40,0x3C,0x2C,0}, {0x40,0x34,0x2C,0}, {0x40,0x30,0x2C,0},
  {0x54,0x54,0x54,0}, {0xFC,0x54,0x54,0}, {0x54,0xFC,0x54,0}, {0xFC,0xFC,0x54,0},
  {0x54,0x54,0xFC,0}, {0xFC,0x54,0xFC,0}, {0x54,0xFC,0xFC,0}, {0xFC,0xFC,0xFC,0}
} ;

// Division lookup tables.  These tables compute 0-255 divided by 51 and
// modulo 51.  These tables could approximate gamma correction.

unsigned char div51 [256] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5
} ;

unsigned char mod51 [256] =
{
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
  38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6,
  7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
  44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
  36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3,
  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0
} ;

// Multiplication lookup tables. These compute 0-5 times 6 and 36.

unsigned char mul6 [6] = {0, 6, 12, 18, 24, 30} ;
unsigned char mul36 [6] = {0, 36, 72, 108, 144, 180} ;

// Ordered 8x8 dither matrix for 8 bit to 2.6 bit halftones.

unsigned char dither8x8 [64] =
{
   0, 38,  9, 47,  2, 40, 11, 50,
  25, 12, 35, 22, 27, 15, 37, 24,
   6, 44,  3, 41,  8, 47,  5, 43,
  31, 19, 28, 15, 34, 21, 31, 18,
   1, 39, 11, 49,  0, 39, 10, 48,
  27, 14, 36, 23, 26, 13, 35, 23,
   7, 46,  4, 43,  7, 45,  3, 42,
  33, 20, 30, 17, 32, 19, 29, 16,
} ;

#endif // #if DECLARE_TABLES

}
// end of namespace povwin

#endif // PVENGINE_H_INCLUDED
