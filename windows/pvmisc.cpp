//******************************************************************************
///
/// @file windows/pvmisc.cpp
///
/// This module implements miscellaneous routines for the Windows build of POV.
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

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#include <windows.h>
#include <htmlhelp.h>
#include <shellapi.h>
#include <csetjmp>
#include <cstring>
#include <direct.h>
#include <io.h>
#include <commctrl.h>
#include <cctype>
#include <sys/stat.h>
#include <crtdbg.h>
#include <fstream>
#include <iostream>
// #include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "pvengine.h"
#include "pvedit.h"
#include "resource.h"
#include "pvguiext.h"
#include "backend/povray.h"

#ifdef RTR_SUPPORT
  #include "rtrsupport.h"
#endif

// this must be the last file included
#include "syspovdebug.h"

namespace povwin
{

using std::map;
using std::endl;
using std::string;

extern int                                  alert_sound ;
extern int                                  message_xchar ;
extern int                                  message_ychar ;
extern int                                  renderwin_left ;
extern int                                  renderwin_top ;
extern int                                  io_restrictions ;
extern int                                  bandWidths [8] ;
extern int                                  screen_origin_x ;
extern int                                  screen_origin_y ;
extern int                                  virtual_screen_width ;
extern int                                  virtual_screen_height ;
extern int                                  renderwin_transparency ;
extern char                                 message_font_name [256] ;
extern char                                 ourPath [_MAX_PATH] ;
extern char                                 helpPath [_MAX_PATH] ;
extern char                                 lastRenderName [_MAX_PATH] ;
extern char                                 lastBitmapName [_MAX_PATH] ;
extern char                                 lastRenderPath [_MAX_PATH] ;
extern char                                 lastBitmapPath [_MAX_PATH] ;
extern char                                 lastQueuePath [_MAX_PATH] ;
extern char                                 lastSecondaryIniFilePath [_MAX_PATH] ;
extern char                                 SecondaryRenderIniFileName [_MAX_PATH] ;
extern char                                 SecondaryRenderIniFileSection [64] ;
extern char                                 background_file [_MAX_PATH ] ;
extern char                                 tool_commands [MAX_TOOLCMD] [MAX_TOOLCMDTEXT] ;
extern char                                 tool_help [MAX_TOOLCMD] [MAX_TOOLHELPTEXT] ;
extern char                                 source_file_name [_MAX_PATH] ;
extern char                                 ToolIniFileName [_MAX_PATH] ;
extern char                                 command_line [_MAX_PATH * 3] ;
extern char                                 queued_files [MAX_QUEUE] [_MAX_PATH] ;
extern char                                 RegionStr [] ;
extern char                                 engineHelpPath [_MAX_PATH] ;
extern char                                 render_complete_sound [_MAX_PATH] ;
extern char                                 parse_error_sound [_MAX_PATH] ;
extern char                                 render_error_sound [_MAX_PATH] ;
extern char                                 FontPath [_MAX_PATH] ;
extern void                                 *CurrentEditor ;
extern bool                                 render_complete_sound_enabled ;
extern bool                                 parse_error_sound_enabled  ;
extern bool                                 render_error_sound_enabled ;
extern bool                                 keep_messages ;
extern bool                                 alert_on_completion ;
extern bool                                 save_settings ;
extern bool                                 running_demo ;
extern bool                                 fast_scroll ;
extern bool                                 MakeRenderwinActive ;
extern bool                                 renderwin_destroyed ;
extern bool                                 no_shellout_wait ;
extern bool                                 tile_background ;
extern bool                                 debugging ;
extern bool                                 no_palette_warn ;
extern bool                                 HideRenderWithMain ;
extern bool                                 RenderwinIsChild ;
extern bool                                 IsW95UserInterface ;
extern bool                                 use_16bit_editor ;
extern bool                                 system_noactive ;
extern bool                                 IsWin32 ;
extern bool                                 one_instance ;
extern bool                                 use_toolbar ;
extern bool                                 use_tooltips ;
extern bool                                 editors_enabled ;
extern bool                                 expert_menus ;
extern bool                                 drop_to_editor ;
extern bool                                 render_auto_close ;
extern bool                                 ExtensionsEnabled ;
extern bool                                 use_taskbar ;
extern bool                                 allow_rw_source ;
extern bool                                 no_shell_outs ;
extern bool                                 hide_newuser_help ;
extern bool                                 IsW98 ;
extern bool                                 IsW2k ;
extern bool                                 preserve_bitmap ;
extern bool                                 check_new_version ;
extern bool                                 check_news ;
extern bool                                 send_system_info ;
extern bool                                 homeInferred ;
extern bool                                 AutoAppendPaths ;
extern bool                                 PreventSleep;
extern unsigned                             message_font_size ;
extern unsigned                             message_font_weight ;
extern unsigned                             screen_width ;
extern unsigned                             screen_height ;
extern unsigned                             screen_depth ;
extern unsigned                             renderwin_8bits ;
extern unsigned                             auto_render ;
extern unsigned                             queued_file_count ;
extern unsigned                             renderwin_flags ;
extern unsigned                             render_priority ;
extern unsigned                             Duty_Cycle ;
extern unsigned                             on_completion ;
extern unsigned                             window_count ;
extern HWND                                 main_window ;
extern HWND                                 toolbar_window ;
extern HWND                                 toolbar_cmdline ;
extern HWND                                 toolbar_combobox ;
extern HWND                                 rebar_window ;
extern HMENU                                hMainMenu ;
extern HMENU                                hToolsMenu ;
extern COLORREF                             background_colour ;
extern COLORREF                             text_colours[3] ;
extern HH_AKLINK                            hh_aklink ;
extern WINDOWPLACEMENT                      mainwin_placement ;
extern CRITICAL_SECTION                     critical_section ;

#define MAX_DIRSPEC 70
char *WriteDirSpecs [MAX_DIRSPEC] ;
char *ReadDirSpecs [MAX_DIRSPEC] ;

typedef struct
{
  unsigned    id ;
  bool        *varptr ;
  bool        rval ;
  bool        autowrite ;
  char        *section ;
  char        *entry ;
  bool        defval ;
} toggle_struct ;

toggle_struct toggles [] =
{
// rval should be true if no special processing is required within handle_main_command
//  ID                       VarPtr                         RVal AutoWrite  Section        Name                       DefVal
  { CM_SAVE_SETTINGS,       (bool *) &save_settings,        false, true,  "General",       "SaveSettingsOnExit"      , true  },
  { CM_PRESERVEMESSAGES,    (bool *) &keep_messages,        true , true,  "Messages",      "KeepMessages"            , false },
  { CM_FORCE8BITS,          (bool *) &renderwin_8bits,      false, true,  "RenderWindow",  "Use8BitMode"             , false },
  { CM_RENDERACTIVE,        (bool *) &MakeRenderwinActive,  true , true,  "RenderWindow",  "MakeActive"              , true  },
  { CM_RENDERABOVEMAIN,     (bool *) &RenderwinIsChild,     false, true,  "RenderWindow",  "KeepAboveMain"           , true  },
  { CM_RENDERHIDE,          (bool *) &HideRenderWithMain,   true , true,  "RenderWindow",  "HideWhenMainMinimized"   , true  },
  { CM_ALERT,               (bool *) &alert_on_completion,  true , true,  "Renderer",      "AlertOnCompletion"       , true  },
  { CM_AUTORENDER,          (bool *) &auto_render,          true , true,  "Renderer",      "AutoRender"              , true  },
  { CM_PREVENTSLEEP,        (bool *) &PreventSleep,         true , true,  "Renderer",      "PreventSleep"            , true  },
  { CM_SHELLOUTWAIT,        (bool *) &no_shellout_wait,     true , true,  "Renderer",      "NoShelloutWait"          , false },
  { CM_TILEDBACKGROUND,     (bool *) &tile_background,      false, true,  "General",       "TileBackground"          , false },
  { CM_SYSTEMNOACTIVE,      (bool *) &system_noactive,      true,  true,  "Renderer",      "SystemNoActive"          , false },
  { CM_SINGLEINSTANCE,      (bool *) &one_instance,         false, false, "General",       "OneInstance"             , true  },
  { CM_USETOOLBAR,          (bool *) &use_toolbar,          false, true,  "MainWindow",    "UseToolbar"              , true  },
  { CM_USETOOLTIPS,         (bool *) &use_tooltips,         true,  true,  "MainWindow",    "UseTooltips"             , true  },
  { CM_RENDERAUTOCLOSE,     (bool *) &render_auto_close,    true,  true,  "RenderWindow",  "AutoClose"               , false },
  { CM_USEEXTENSIONS,       (bool *) &ExtensionsEnabled,    true,  true,  "GUIExtensions", "UseExtensions"           , true  },
  { CM_RW_SOURCE,           (bool *) &allow_rw_source,      true,  true,  "Scripting",     "ReadWriteSourceDir"      , true  },
  { CM_NO_SHELLOUTS,        (bool *) &no_shell_outs,        true,  true,  "Scripting",     "NoShellOuts"             , true  },
  { CM_HIDENEWUSERHELP,     (bool *) &hide_newuser_help,    false, true,  "General",       "HideNewUserHelp"         , false },
  { CM_PRESERVERENDERBITMAP,(bool *) &preserve_bitmap,      true,  true,  "RenderWindow",  "PreserveBitmap"          , true  },
  { CM_CHECKNEWVERSION,     (bool *) &check_new_version,    true,  true,  "General",       "CheckNewVersion"         , true  },
  { CM_SENDSYSTEMINFO,      (bool *) &send_system_info,     true,  true,  "General",       "SendSystemInfo"          , true  },
  { -1,                     (bool *) NULL,                  false, true,  "",              ""                        , false }
} ;

#ifdef MAP_INI_TO_REGISTRY

bool PutHKCU(const char *Section, const char *Name, const char *Value)
{
  char                  path[1024] = "Software\\" REGKEY "\\" REGVERKEY "\\Windows\\Engine\\";
  HKEY                  hKey ;

  strcat(path, Section);
  if (RegCreateKeyEx(HKEY_CURRENT_USER, path, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
    return (false) ;
  if (Value != NULL)
    RegSetValueEx(hKey, Name, 0, REG_SZ, (BYTE *) Value, (DWORD) strlen(Value) + 1) ;
  else
    RegDeleteValue(hKey, Name);
  RegCloseKey(hKey) ;
  return true;
}

bool PutHKCU(const char *Section, const char *Name, const string& Value)
{
  return PutHKCU(Section, Name, Value.c_str());
}

bool PutHKCU(const char *Section, const char *Name, unsigned Value)
{
  char                  path[1024] = "Software\\" REGKEY "\\" REGVERKEY "\\Windows\\Engine\\";
  HKEY                  hKey ;

  strcat(path, Section);
  if (RegCreateKeyEx (HKEY_CURRENT_USER, path, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
    return (false) ;
  RegSetValueEx(hKey, Name, 0, REG_DWORD, (BYTE *) &Value, sizeof(DWORD)) ;
  RegCloseKey(hKey);
  return true;
}

unsigned GetHKCU(const char *Section, const char *Name, unsigned DefaultValue)
{
  char      path[1024] = "Software\\" REGKEY "\\" REGVERKEY "\\Windows\\Engine\\";
  HKEY      key;
  DWORD     type;
  DWORD     len = sizeof(DWORD);
  DWORD     result = DefaultValue;

  strcat(path, Section);
  if (RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx(key, Name, 0, &type, (BYTE *) &result, &len) != ERROR_SUCCESS || type != REG_DWORD)
      result = DefaultValue;
    RegCloseKey(key) ;
  }
  return result;
}

size_t GetHKCU(const char *Section, const char *Name, const char *DefaultValue, char *Buffer, unsigned MaxLength)
{
  char      path[1024] = "Software\\" REGKEY "\\" REGVERKEY "\\Windows\\Engine\\";
  HKEY      key;
  DWORD     type;
  DWORD     len = MaxLength;

  strcat(path, Section);
  if (RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx(key, Name, 0, &type, (BYTE *) Buffer, &len) != ERROR_SUCCESS || type != REG_SZ)
      strcpy(Buffer, DefaultValue);
    RegCloseKey(key) ;
  }
  else
    strcpy(Buffer, DefaultValue);
  return strlen(Buffer);
}

#else // MAP_INI_TO_REGISTRY

bool PutHKCU(const char *section, const char *entry, const char *value)
{
  WritePrivateProfileString(section, entry, value, EngineIniFileName);
  return true;
}

bool PutHKCU(const char *section, const char *entry, const string& value)
{
  WritePrivateProfileString(section, entry, value.c_str(), EngineIniFileName);
  return true;
}

bool PutHKCU(const char *section, const char *entry, unsigned int value)
{
  char        str [32] ;

  sprintf (str, "%u", value) ;
  WritePrivateProfileString(section, entry, str, EngineIniFileName);
  return true;
}

unsigned GetHKCU(const char *section, const char *name, unsigned defval)
{
  return GetPrivateProfileInt(section, name, defval, EngineIniFileName);
}

size_t GetHKCU(const char *section, const char *name, const char *defval, char *buffer, unsigned maxlen)
{
  return GetPrivateProfileString(section, name, defval, buffer, maxlen, EngineIniFileName);
}

#endif

bool GetDontShowAgain (const char *Name)
{
  return (GetHKCU("DontShowAgain", Name, 0) != 0) ;
}

void PutDontShowAgain (const char *Name, bool dontShow)
{
  PutHKCU("DontShowAgain", Name, dontShow ? 1 : 0) ;
}

bool process_toggles (WPARAM wParam)
{
  toggle_struct         *t ;

  for (t = toggles ; t->id != -1 ; t++)
  {
    if (t->id == LOWORD (wParam))
    {
      *t->varptr = !*t->varptr ;
      PVCheckMenuItem (t->id, *t->varptr ? MF_CHECKED : MF_UNCHECKED) ;
      return (t->rval) ;
    }
  }
  return (false) ;
}

void set_toggles (void)
{
  toggle_struct         *t ;

  for (t = toggles ; (int) t->id != -1 ; t++)
    PVCheckMenuItem (t->id, *t->varptr ? MF_CHECKED : MF_UNCHECKED) ;
}

void read_toggles (void)
{
  toggle_struct         *t ;

  for (t = toggles ; (int) t->id != -1 ; t++)
    *t->varptr = (GetHKCU(t->section, t->entry, t->defval) != 0);
}

void write_toggles (void)
{
  toggle_struct         *t ;

  for (t = toggles ; (int) t->id != -1 ; t++)
    if (t->autowrite)
      PutHKCU(t->section, t->entry, *t->varptr) ;
}

bool fileExists (const char *filename)
{
  struct stat statBuf ;

  if ((stat (filename, &statBuf) != 0) || ((statBuf.st_mode & _S_IFREG) == 0))
    return (false) ;
  return (true) ;
}

bool dirExists (const char *filename)
{
  char        str[_MAX_PATH];
  struct stat statBuf ;

  if (filename[0] == '\0')
    return false;
  if (hasTrailingPathSeparator(filename))
  {
    strcpy(str, filename);
    str[strlen(filename) - 1] = '\0';
    return stat(str, &statBuf) == 0 && (statBuf.st_mode & _S_IFDIR) != 0;
  }

  return stat(filename, &statBuf) == 0 && (statBuf.st_mode & _S_IFDIR) != 0;
}

void read_dir_restriction (char *iniSection, char **store)
{
  int         i ;
  char        EntryName [16] ;
  char        str1 [_MAX_PATH] ;
  char        str2 [_MAX_PATH] ;
  char        *s ;

  // we can afford to be generous with respect to errors here - if an entry
  // doesn't make it into the list, it won't reduce security, since the system
  // defaults to denying access.
  for (i = 0 ; i < MAX_DIRSPEC - 1 ; i++)
  {
    sprintf (EntryName, "%d", i) ;
    GetHKCU(iniSection, EntryName, "", str1, sizeof (str1)) ;
    if (str1 [0] == '\0')
      continue ;
    if (_strnicmp (str1, "%INSTALLDIR%", 12) == 0)
    {
      strcpy (str2, str1) ;
      strcpy (str1, BinariesPath) ;
      trimTrailingPathSeparator (str1) ;
      strcat (str1, str2 + 12) ;
    }
    else if (_strnicmp (str1, "%PROFILEDIR%", 12) == 0)
    {
      strcpy (str2, str1) ;
      strcpy (str1, DocumentsPath) ;
      trimTrailingPathSeparator (str1) ;
      strcat (str1, str2 + 12) ;
    }
    else if (_strnicmp (str1, "%FONTDIR%", 9) == 0)
    {
      strcpy (str2, str1 + 9) ;
      strcpy (str1, FontPath) ;
      trimTrailingPathSeparator (str1) ;
      strcat (str1, str2) ;
    }
    appendPathSeparator (str1) ;
    if (GetFullPathName (str1, sizeof (str2), str2, &s) == 0)
      continue ;
    _strupr (str2) ;
    if ((s = (char *) malloc (strlen (str2) + 1)) == NULL)
      continue ;
    strcpy (s, str2) ;
    *store++ = s ;
  }
}

void read_dir_restrictions (void)
{
  io_restrictions = GetHKCU("Scripting", "IO Restrictions", 1) ;
  PVCheckMenuRadioItem (CM_IO_NO_RESTRICTIONS, CM_IO_RESTRICT_READWRITE, io_restrictions + CM_IO_NO_RESTRICTIONS) ;
  read_dir_restriction ("Permitted Input Paths", ReadDirSpecs) ;
  read_dir_restriction ("Permitted Output Paths", WriteDirSpecs) ;
}

void clear_dir_restrictions (void)
{
  for (char **dirspec = ReadDirSpecs ; *dirspec != NULL ; dirspec++)
    free (*dirspec) ;
  for (char **dirspec = WriteDirSpecs ; *dirspec != NULL ; dirspec++)
    free (*dirspec) ;
  memset (ReadDirSpecs, 0, sizeof (ReadDirSpecs)) ;
  memset (WriteDirSpecs, 0, sizeof (WriteDirSpecs)) ;
}

void read_INI_settings (void)
{
  char        str [_MAX_PATH] ;

  mainwin_placement.showCmd = GetHKCU("MainWindow", "ShowCmd", SW_SHOWNORMAL) ;
  mainwin_placement.ptMinPosition.x = GetHKCU("MainWindow", "MinPositionX", -1) ;
  mainwin_placement.ptMinPosition.y = GetHKCU("MainWindow", "MinPositionY", -1) ;
  mainwin_placement.ptMaxPosition.x = GetHKCU("MainWindow", "MaxPositionX", -1) ;
  mainwin_placement.ptMaxPosition.y = GetHKCU("MainWindow", "MaxPositionY", -1) ;
  mainwin_placement.rcNormalPosition.left = GetHKCU("MainWindow", "NormalPositionLeft", 128) ;
  mainwin_placement.rcNormalPosition.top = GetHKCU("MainWindow", "NormalPositionTop", 128) ;
  mainwin_placement.rcNormalPosition.right = GetHKCU("MainWindow", "NormalPositionRight", -1) ;
  mainwin_placement.rcNormalPosition.bottom = GetHKCU("MainWindow", "NormalPositionBottom", -1) ;
  renderwin_left = GetHKCU("RenderWindow", "NormalPositionX", 256) ;
  renderwin_top = GetHKCU("RenderWindow", "NormalPositionY", 256) ;
  renderwin_flags = GetHKCU("RenderWindow", "Flags", 0) ;
  renderwin_transparency = GetHKCU("RenderWindow", "Transparency", 128) ;
  GetHKCU("Messages", "Font", "Lucida Console", message_font_name, sizeof (message_font_name)) ;
  message_font_size = GetHKCU("Messages", "FontSize", 8) ;
  message_font_weight = GetHKCU("Messages", "FontWeight", FW_NORMAL) ;
  fast_scroll = (GetHKCU("Messages", "FastScroll", false) != 0);
  alert_sound = GetHKCU("Renderer", "AlertSound", MB_ICONASTERISK) ;
  render_priority = GetHKCU("Renderer", "Priority", CM_RENDERPRIORITY_NORMAL) ;
  Duty_Cycle = GetHKCU("Renderer", "DutyCycle", 9) ;
  on_completion = GetHKCU("Renderer", "Completion", CM_COMPLETION_NOTHING) ;
  no_palette_warn = (GetHKCU("General", "NoPaletteWarn", false) != 0);
  GetHKCU("General", "SecondaryINISection", "[512x384, No AA]", SecondaryRenderIniFileSection, sizeof (SecondaryRenderIniFileSection)) ;

  // some paths change between betas; since the INI file is not created or removed
  // by the installer, it may refer to old paths. fix this here and further on by
  // checking to make sure the path exists.
  GetHKCU("General", "LastRenderName", "", lastRenderName, sizeof (lastRenderName)) ;
  GetHKCU("General", "LastRenderPath", "", lastRenderPath, sizeof (lastRenderPath)) ;
  if (!dirExists(lastRenderPath))
    lastRenderPath[0] = '\0';
  GetHKCU("General", "LastQueuePath", "", lastQueuePath, sizeof (lastQueuePath)) ;
  if (!dirExists(lastQueuePath))
    lastQueuePath[0] = '\0';
  GetHKCU("General", "LastBitmapName", "*.bmp", lastBitmapName, sizeof (lastBitmapName)) ;
  if (strchr(lastBitmapName, '*') == NULL && !fileExists(lastBitmapName))
    strcpy(lastBitmapName, "*.bmp");
  GetHKCU("General", "LastBitmapPath", "", lastBitmapPath, sizeof (lastBitmapPath)) ;
  if (!dirExists(lastBitmapPath))
    lastBitmapPath[0] = '\0';

  sprintf (str, "%sini", DocumentsPath) ;
  GetHKCU("General", "LastINIPath", str, lastSecondaryIniFilePath, sizeof (lastSecondaryIniFilePath)) ;
  if (!dirExists (lastSecondaryIniFilePath))
    strcpy (lastSecondaryIniFilePath, str) ;
  sprintf (str, "%sini\\quickres.ini", DocumentsPath) ;
  GetHKCU("General", "SecondaryINIFile", str, SecondaryRenderIniFileName, sizeof (SecondaryRenderIniFileName)) ;
  if (!fileExists (SecondaryRenderIniFileName))
    strcpy (SecondaryRenderIniFileName, str) ;
  GetHKCU("General", "BackgroundFile", screen_depth > 8 ? "0" : "1", background_file, sizeof (background_file)) ;

  text_colours[0] = GetHKCU("General", "TextColour", RGB (255, 255, 255)) ;
  text_colours[1] = GetHKCU("General", "WarningColour", RGB (255, 255, 0)) ;
  text_colours[2] = GetHKCU("General", "ErrorColour", RGB (0, 255, 255)) ;
  background_colour = GetHKCU("General", "BackgroundColour", RGB (31, 0, 63)) ;
  if (!debugging)
    debugging = (GetHKCU("General", "Debug", 0) != 0);
  drop_to_editor = (GetHKCU("General", "DropToEditor", 1) != 0);
  editors_enabled = (GetHKCU("General", "UseEditors", 1) != 0);
  for (int i = 0 ; i < 6 ; i++)
  {
    sprintf (str, "Band%dWidth", i) ;
    bandWidths [i] = GetHKCU("ToolBar", str, 0) ;
  }
  read_toggles() ;
  read_dir_restrictions() ;

  sprintf (str, "%ssounds\\Render Finished.wav", BinariesPath) ;
  GetHKCU("Sounds", "RenderCompleteSound", str, render_complete_sound, _MAX_PATH) ;
  if (!fileExists(render_complete_sound))
    strcpy(render_complete_sound, str);

  sprintf (str, "%ssounds\\Parse Error.wav", BinariesPath) ;
  GetHKCU("Sounds", "ParseErrorSound", str, parse_error_sound, _MAX_PATH) ;
  if (!fileExists(parse_error_sound))
    strcpy(parse_error_sound, str);

  sprintf (str, "%ssounds\\Render Cancelled.wav", BinariesPath) ;
  GetHKCU("Sounds", "RenderErrorSound", str, render_error_sound, _MAX_PATH) ;
  if (!fileExists(render_error_sound))
    strcpy(render_error_sound, str);

  render_complete_sound_enabled = GetHKCU("Sounds", "RenderCompleteSoundEnabled", TRUE) != 0 ;
  parse_error_sound_enabled = GetHKCU("Sounds", "ParseErrorSoundEnabled", TRUE) != 0 ;
  render_error_sound_enabled = GetHKCU("Sounds", "RenderErrorSoundEnabled", TRUE) != 0 ;

  AutoAppendPaths = GetHKCU("General", "AutoAppendPaths", TRUE) != 0 ;

#ifdef RTR_SUPPORT
  GetHKCU("RTR", "VideoSource", "", str, sizeof (str)) ;
  SetVideoSourceName(str);
#endif
}

void write_INI_settings (bool noreset)
{
  char                  *s ;
  char                  str [sizeof (command_line)] ;
  REBARBANDINFO         rebarInfo ;

  if (noreset)
    strcpy (str, command_line) ;
  if (RegionStr [0] != '\0')
  {
    if ((s = strstr (command_line, RegionStr)) != NULL)
      strcpy (s, s + strlen (RegionStr)) ;
    else if ((s = strstr (command_line, RegionStr + 1)) != NULL)
      strcpy (s, s + strlen (RegionStr) - 1) ;
    if (!noreset)
      SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
  }
  PutHKCU("General", "CommandLine", command_line) ;
  if (noreset)
    strcpy (command_line, str) ;

  rebarInfo.cbSize = sizeof (REBARBANDINFO) ;
  rebarInfo.fMask = RBBIM_SIZE ;
  for (int i = 0 ; i < 6 ; i++)
  {
    sprintf (str, "Band%dWidth", i) ;
    // under XP64 the SendMessage() call returns TRUE but the cx value is not altered.
    // so we detect that and avoid writing the data unless the problem is fixed.
    rebarInfo.cx = 0x12345678 ;
    if (SendMessage (rebar_window, RB_GETBANDINFO, i, (LPARAM) (LPREBARBANDINFO) &rebarInfo))
      if (rebarInfo.cx != 0x12345678)
        PutHKCU("ToolBar", str, rebarInfo.cx) ;
  }

  PutHKCU("MainWindow", "ShowCmd", mainwin_placement.showCmd) ;
  PutHKCU("MainWindow", "NormalPositionLeft", mainwin_placement.rcNormalPosition.left) ;
  PutHKCU("MainWindow", "NormalPositionTop", mainwin_placement.rcNormalPosition.top) ;
  PutHKCU("MainWindow", "NormalPositionRight", mainwin_placement.rcNormalPosition.right) ;
  PutHKCU("MainWindow", "NormalPositionBottom", mainwin_placement.rcNormalPosition.bottom) ;
  PutHKCU("RenderWindow", "NormalPositionX", renderwin_left) ;
  PutHKCU("RenderWindow", "NormalPositionY",  renderwin_top) ;
  PutHKCU("RenderWindow", "Flags",  renderwin_flags) ;
  PutHKCU("RenderWindow", "Transparency",  renderwin_transparency) ;
  PutHKCU("Messages", "FontSize", message_font_size) ;
  PutHKCU("Messages", "FontWeight", message_font_weight) ;
  PutHKCU("Renderer", "AlertSound", alert_sound) ;
  PutHKCU("Renderer", "Completion", on_completion) ;
  PutHKCU("Renderer", "Priority", render_priority) ;
  PutHKCU("Renderer", "DutyCycle", Duty_Cycle) ;
  PutHKCU("General", "TextColour", text_colours[0]) ;
  PutHKCU("General", "WarningColour", text_colours[1]) ;
  PutHKCU("General", "ErrorColour", text_colours[2]) ;
  PutHKCU("General", "BackgroundColour", background_colour) ;
  PutHKCU("General", "DropToEditor", drop_to_editor) ;
  PutHKCU("Messages", "Font", message_font_name) ;
  PutHKCU("General", "LastRenderName",  lastRenderName) ;
  PutHKCU("General", "LastRenderPath", lastRenderPath) ;
  PutHKCU("General", "LastQueuePath", lastQueuePath) ;
  PutHKCU("General", "SecondaryINISection", SecondaryRenderIniFileSection) ;
  PutHKCU("General", VERSIONVAL, POV_RAY_VERSION) ;
  PutHKCU("Sounds", "RenderCompleteSoundEnabled", render_complete_sound_enabled) ;
  PutHKCU("Sounds", "ParseErrorSoundEnabled", parse_error_sound_enabled) ;
  PutHKCU("Sounds", "RenderErrorSoundEnabled", render_error_sound_enabled) ;
  PutHKCU("General", "LastBitmapName",  lastBitmapName) ;
  PutHKCU("General", "LastBitmapPath", lastBitmapPath) ;
  PutHKCU("General", "LastINIPath", lastSecondaryIniFilePath) ;
  PutHKCU("General", "SecondaryINIFile", SecondaryRenderIniFileName) ;
  PutHKCU("General", "BackgroundFile", background_file) ;
  PutHKCU("Sounds", "RenderCompleteSound", render_complete_sound) ;
  PutHKCU("Sounds", "ParseErrorSound", parse_error_sound) ;
  PutHKCU("Sounds", "RenderErrorSound", render_error_sound) ;
  write_toggles () ;

#ifdef RTR_SUPPORT
  PutHKCU("RTR", "VideoSource", GetVideoSourceName().c_str()) ;
#endif
}

#ifdef BOOST_REGEX_NO_LONGER_BROKEN

// can't use regex right now: on exit from povwin, the atexit handler registered by
// boost calls something in the regex library that causes an exception. this happens
// even if all we ever did was declare a single regex (e.g. 'boost::regex foo("bar");')
// and never use it.
//
// TODO: add replace regex to alter paths from old to new where appropriate
void cloneOldIni(const string& oldPath, const string& newPath)
{
  bool                      keep = false;
  smatch                    what;
  string                    sectionName;
  string                    line;
  const regex               section("^\\[\\s*([^\\]]*)\\s*\\].*");
  const regex               entry("^([^=\\s]+)\\s*=.*");
  const regex               skipEntries("runcount|itsabouttime|commandline|lastinipath|secondaryinifile|version|backgroundfile|.*sound");
  const regex               copiedSections("general|editor|tipoftheday|toolbar|mainwindow|renderwindow|messages|renderer|lastrender|dontshowagain|sounds");
  map<string, bool>         hadSection;

  line = oldPath + "ini\\pvengine.ini";
  std::ifstream inF(line.c_str());
  if (inF.bad())
    return;
  line = newPath + "ini\\pvengine.ini";
  ofstream outF(line.c_str());
  if (outF.bad())
    return;

  outF << "[Permitted Input Paths]" << endl;
  outF << "1=%INSTALLDIR%" << endl;
  outF << "2=%PROFILEDIR%" << endl;
  outF << "3=%FONTDIR%" << endl;
  outF << endl << "[Permitted Output Paths]" << endl;
  outF << "1=%PROFILEDIR%\\Insert Menu" << endl;

  // some INI files I've seen have duplicate sections. during the copy we filter out any such.
  while (getline(inF, line))
  {
    trim(line);
    string str(to_lower_copy(line));
    if (str.empty())
      continue;
    if (regex_match(str, what, section))
    {
      keep = !hadSection[sectionName = what[1]] && regex_match(sectionName, copiedSections);
      hadSection[sectionName] = true;
      if (keep)
        outF << endl << line << endl;
      continue;
    }
    if (keep && regex_match(str, what, entry) && regex_match(static_cast<string>(what[1]), skipEntries))
      continue;
    if (keep)
      outF << line << endl;
  }
  inF.close();

  outF << "[GUIExtensions]" << endl << endl;
  outF << "[Scripting]" << endl << endl;
  outF << "[RTR]" << endl << endl;
  outF << "[Info]" << endl << endl;
  outF.close();
}

#else

void cloneOldIni(const string& oldPath, const string& newPath)
{
  bool                      keep = false;
  size_t                    pos;
  string                    sectionName;
  string                    line;
  string                    oldPathLC = boost::to_lower_copy(oldPath);
  const string              keepSections("general^toolbar^mainwindow^renderwindow^renderer^lastrender^dontshowagain^editor^");
  const string              skipEntries("runcount^itsabouttime^commandline^version^backgroundcolour^textcolour^"
                                        "tilebackground^bigsplash^savesettingsonexit^");
  std::ifstream             inF;
  map<string, bool>         hadSection;

  if (oldPath != "")
  {
    line = oldPath + "ini\\pvengine.ini";
    inF.open(line.c_str());
    if (inF.bad())
      return;
  }
  line = newPath + "ini\\pvengine.ini";
  std::ofstream outF(line.c_str());
  if (outF.bad())
  {
    if (oldPath != "")
      inF.close();
    return;
  }

  outF << "[Permitted Input Paths]" << endl;
  outF << "1=%INSTALLDIR%" << endl;
  outF << "2=%PROFILEDIR%" << endl;
  outF << "3=%FONTDIR%" << endl;
  outF << endl << "[Permitted Output Paths]" << endl;
  outF << "1=%PROFILEDIR%\\Insert Menu" << endl;
  outF << endl << "[GUIExtensions]" << endl << endl;
  outF << "[Scripting]" << endl << endl;
  outF << "[RTR]" << endl << endl;
  outF << "[Info]" << endl << endl;

  if (oldPath == "")
  {
    outF.close();
    return;
  }

  // some INI files I've seen have duplicate sections. during the copy we filter out any such.
  while (getline(inF, line))
  {
    boost::trim(line);
    string str(boost::to_lower_copy(line));
    if (str.empty())
      continue;
    if (str[0] == '[' && (pos = str.find(']')) != string::npos)
    {
      sectionName = boost::trim_copy(str.substr(1, pos - 1));
      keep = !hadSection[sectionName] && keepSections.find(sectionName + "^") != string::npos;
      hadSection[sectionName] = true;
      if (keep)
        outF << endl << line << endl;
      continue;
    }
    if (keep)
    {
      // any INI entry that refers to the previous install dir is ignored
      if (str.find(oldPathLC) != string::npos)
        continue;

      if ((pos = str.find('=')) != string::npos)
      {
        str.erase(pos);
        boost::trim(str);

        // skip other entries we don't want to copy into the new install's INI
        if (skipEntries.find(str + "^") != string::npos)
          continue;
      }
      outF << line << endl;
    }
  }
  inF.close();
  outF << endl;
  outF.close();
}
#endif

void GetRelativeClientRect (HWND hParent, HWND hChild, RECT *rect)
{
  POINT       *points = (POINT *) rect ;

  GetWindowRect (hChild, rect) ;
  ScreenToClient (hParent, points++) ;
  ScreenToClient (hParent, points) ;
}

void CenterWindowRelative (HWND hRelativeTo, HWND hTarget, bool bRepaint, bool checkBorders)
{
  int         difference ;
  int         width ;
  int         height ;
  int         x ;
  int         y ;
  int         twidth ;
  int         theight ;
  RECT        relativeToRect ;
  RECT        targetRect ;

  if (hRelativeTo != NULL && IsWindowVisible (hRelativeTo))
  {
    WINDOWPLACEMENT wp ;
    wp.length = sizeof (WINDOWPLACEMENT) ;
    GetWindowPlacement (hRelativeTo, &wp) ;
    if (wp.showCmd == SW_SHOWMINIMIZED)
      hRelativeTo = GetDesktopWindow () ;
  }
  else
    hRelativeTo = GetDesktopWindow () ;
  GetWindowRect (hRelativeTo, &relativeToRect) ;
  GetWindowRect (hTarget, &targetRect) ;
  width = targetRect.right - targetRect.left ;
  height = targetRect.bottom - targetRect.top ;
  difference = relativeToRect.right - relativeToRect.left - width ;
  x = relativeToRect.left + difference / 2 ;
  difference = relativeToRect.bottom - relativeToRect.top - height ;
  y = relativeToRect.top + difference / 2 ;
  MoveWindow (hTarget, x, y, width, height, bRepaint && !checkBorders) ;
  if (checkBorders)
  {
    GetWindowRect (hTarget, &targetRect) ;
    if (targetRect.left < screen_origin_x)
      OffsetRect (&targetRect, screen_origin_x - targetRect.left, 0) ;
    if (targetRect.top < screen_origin_y)
      OffsetRect (&targetRect, 0, screen_origin_y - targetRect.top) ;
    if (targetRect.right > virtual_screen_width + screen_origin_x)
      OffsetRect (&targetRect, -(targetRect.right - (virtual_screen_width + screen_origin_x)), 0) ;
    if (targetRect.bottom > virtual_screen_height + screen_origin_y)
      OffsetRect (&targetRect, 0, -(targetRect.bottom - (virtual_screen_height + screen_origin_y))) ;
    twidth = targetRect.right - targetRect.left ;
    theight = targetRect.bottom - targetRect.top ;
    MoveWindow (hTarget, targetRect.left, targetRect.top, twidth, theight, bRepaint) ;
  }
}

void FitWindowInWindow (HWND hRelativeTo, HWND hTarget)
{
  int         rwidth ;
  int         rheight ;
  int         twidth ;
  int         theight ;
  int         x ;
  int         y ;
  RECT        relativeToRect ;
  RECT        targetRect ;

  GetWindowRect (hTarget, &targetRect) ;

  if (hRelativeTo == NULL)
  {
    if (targetRect.right > virtual_screen_width + screen_origin_x)
      OffsetRect (&targetRect, -(targetRect.right - (virtual_screen_width + screen_origin_x)), 0) ;
    if (targetRect.bottom > virtual_screen_height + screen_origin_y)
      OffsetRect (&targetRect, 0, -(targetRect.bottom - (virtual_screen_height + screen_origin_y))) ;
    if (targetRect.left < screen_origin_x)
      OffsetRect (&targetRect, screen_origin_x - targetRect.left, 0) ;
    if (targetRect.top < screen_origin_y)
      OffsetRect (&targetRect, 0, screen_origin_y - targetRect.top) ;
    twidth = targetRect.right - targetRect.left ;
    theight = targetRect.bottom - targetRect.top ;
    MoveWindow (hTarget, targetRect.left, targetRect.top, twidth, theight, true) ;
    return ;
  }

  // if window is not visible GetWindowRect() is not reliable.
  if (!IsWindowVisible (hRelativeTo))
    hRelativeTo = GetDesktopWindow () ;
  GetWindowRect (hRelativeTo, &relativeToRect) ;

  twidth = targetRect.right - targetRect.left ;
  theight = targetRect.bottom - targetRect.top ;
  rwidth = relativeToRect.right - relativeToRect.left ;
  rheight = relativeToRect.bottom - relativeToRect.top ;
  x = targetRect.left ;
  y = targetRect.top ;

  if (twidth > rwidth)
    twidth = rwidth ;
  if (theight > rheight)
    theight = rheight ;
  if (x < relativeToRect.left)
    x = relativeToRect.left ;
  if (y < relativeToRect.top)
    y = relativeToRect.top ;
  if (x + twidth > relativeToRect.right)
    x = relativeToRect.right - twidth ;
  if (y + theight > relativeToRect.bottom)
    y = relativeToRect.bottom - theight ;

  MoveWindow (hTarget, x, y, twidth, theight, true) ;
}

void CenterOffset (HWND win, int id, int offx, int offy, int offw, int offh)
{
  int         x ;
  int         y ;
  int         w ;
  int         h ;
  RECT        R ;
  HWND        dlg = GetDlgItem (win, id) ;
  POINT       P ;

  if (dlg)
  {
    GetWindowRect (dlg, &R) ;

    P.x = R.left ;
    P.y = R.top ;

    ScreenToClient (win, &P) ;

    x = P.x ;
    y = P.y ;
    w = R.right - R.left ;
    h = R.bottom - R.top  ;

    SetWindowPos (dlg, NULL, x + offx, y + offy, w + offw, h + offh, SWP_NOZORDER) ;
  }
}

void SetupExplorerDialog (HWND win)
{
  int         dx ;
  int         dy ;
  RECT        winPos ;

  win = GetParent (win) ;
  GetWindowRect (win, &winPos) ;

  dx = screen_width * 2 / 3 - (winPos.right  - winPos.left) ;
  dy = screen_height * 2 / 3 - (winPos.bottom - winPos.top) ;

  SetWindowPos (win, NULL, 0, 0, screen_width * 2 / 3, screen_height * 2 / 3, SWP_NOZORDER | SWP_NOACTIVATE) ;
  CenterWindowRelative (main_window, win, true, true) ;
  FitWindowInWindow (NULL, win) ;

  CenterOffset (win, 1091,    0,  0,   0,  0) ;
  CenterOffset (win, 1137,    0,  0,   0,  0) ;
  CenterOffset (win, 1088,    0,  0,   0,  0) ;
  CenterOffset (win, 1120,    0,  0,  dx, dy) ;
  CenterOffset (win, 1090,    0, dy,   0,  0) ;
  CenterOffset (win, 1152,    0, dy,  dx,  0) ;
  CenterOffset (win, 1089,    0, dy,   0,  0) ;
  CenterOffset (win, 1136,    0, dy,  dx,  0) ;
  CenterOffset (win, 1040,    0, dy,   0,  0) ;
  CenterOffset (win,    1, dx  , dy,   0,  0) ;
  CenterOffset (win,    2, dx  , dy,   0,  0) ;
  CenterOffset (win, 1038, dx  , dy,   0,  0) ;
}

FileType get_file_type (const char *filename)
{
  char        ext [_MAX_EXT] ;

  splitfn (filename, NULL, NULL, ext) ;
  _strupr (ext) ;

  if (strcmp (ext, ".POV") == 0)
    return (filePOV) ;
  else if (strcmp (ext, ".INC") == 0)
    return (filePOV) ;
  else if (strcmp (ext, ".MCR") == 0)
    return (filePOV) ;
  else if (strcmp (ext, ".MAC") == 0)
    return (filePOV) ;
  else if (strcmp (ext, ".INI") == 0)
    return (fileINI) ;
  else if (strcmp (ext, ".PPM") == 0)
    return (filePPM) ;
  else if (strcmp (ext, ".PGM") == 0)
    return (filePGM) ;
  else if (strcmp (ext, ".PBM") == 0)
    return (filePBM) ;
  else if (strcmp (ext, ".PNG") == 0)
    return (filePNG) ;
  else if (strcmp (ext, ".GIF") == 0)
    return (fileGIF) ;
  else if (strcmp (ext, ".BMP") == 0)
    return (fileBMP) ;
  else if (strcmp (ext, ".EXR") == 0)
    return (fileEXR) ;
  else
    return (fileUnknown) ;
}

bool is_non_primary_file(const char *filename)
{
  char        ext [_MAX_EXT] ;

  splitfn (filename, NULL, NULL, ext) ;
  _strupr (ext) ;

  if (strcmp (ext, ".POV") == 0)
    return (false) ;
  if (strcmp (ext, ".INI") == 0)
    return (false) ;
  return true;
}

char *get_full_name (char *s)
{
  char                  dir [_MAX_PATH + 1] ;
  static char           str [_MAX_PATH] ;

  if (*s == 0)
    return (s) ;
  splitpath (s, str, NULL) ;
  if (str [0] == '\0')
  {
#ifdef TIME_WARP
    // workaround for suspected Win32s bug
    SetCurrentDirectory (".") ;
#endif
    GetCurrentDirectory (sizeof (dir), dir) ;
    joinPath (str, dir, s) ;
    return (str) ;
  }
  return (s) ;
}

void update_menu_for_render (bool rendering)
{
  PVEnableMenuItem (CM_FILERENDER, MF_ENABLED) ;
  PVEnableMenuItem (CM_STOPRENDER, MF_ENABLED) ;

  if (rendering)
  {
    PVModifyMenu (CM_FILERENDER, MF_STRING, CM_FILERENDER, "&Stop Rendering\tAlt+G") ;
    PVEnableMenuItem (CM_COMMANDLINE, MF_GRAYED) ;
    PVEnableMenuItem (CM_RENDERINSERT, MF_GRAYED) ;
    PVEnableMenuItem (CM_SOURCEFILE, MF_GRAYED) ;
    PVEnableMenuItem (CM_DEMO, MF_GRAYED) ;
    PVEnableMenuItem (CM_BENCHMARK, MF_GRAYED) ;
    PVEnableMenuItem (CM_FORCE8BITS, MF_GRAYED) ;
    PVEnableMenuItem (CM_RENDERTHREADCOUNT, MF_GRAYED) ;
    SendMessage (toolbar_window, TB_CHECKBUTTON, (WPARAM) CM_RENDERSLEEP, 0L) ;
    SendMessage (toolbar_window, TB_HIDEBUTTON, (WPARAM) CM_FILERENDER, MAKELONG (1, 0)) ;
    SendMessage (toolbar_window, TB_HIDEBUTTON, (WPARAM) CM_STOPRENDER, MAKELONG (0, 0)) ;
    EnableWindow (toolbar_cmdline, false) ;
  }
  else
  {
    PVEnableMenuItem (CM_SOURCEFILE, MF_ENABLED) ;
    PVEnableMenuItem (CM_COMMANDLINE, MF_ENABLED) ;
    PVEnableMenuItem (CM_RENDERINSERT, MF_ENABLED) ;
    PVEnableMenuItem (CM_DEMO, MF_ENABLED) ;
    PVEnableMenuItem (CM_BENCHMARK, MF_ENABLED) ;
    PVModifyMenu (CM_FILERENDER, MF_STRING, CM_FILERENDER, "&Start Rendering\tAlt+G") ;
    PVEnableMenuItem (CM_FORCE8BITS, MF_ENABLED) ;
    PVEnableMenuItem (CM_RENDERSLEEP, MF_GRAYED) ;
    PVEnableMenuItem (CM_RENDERTHREADCOUNT, MF_ENABLED) ;
    SendMessage (toolbar_window, TB_CHECKBUTTON, (WPARAM) CM_RENDERSLEEP, 0L) ;
    SendMessage (toolbar_window, TB_HIDEBUTTON, (WPARAM) CM_FILERENDER, MAKELONG (0, 0)) ;
    SendMessage (toolbar_window, TB_HIDEBUTTON, (WPARAM) CM_STOPRENDER, MAKELONG (1, 0)) ;
    EnableWindow (toolbar_cmdline, true) ;
  }
  DrawMenuBar (main_window) ;
}

void update_queue_status (bool write_files)
{
  int         i ;
  char        str [64] ;

  if (queued_file_count == 0)
  {
    PVModifyMenu (CM_CLEARQUEUE, MF_STRING, CM_CLEARQUEUE, "C&lear Queue (no entries)") ;
    PVEnableMenuItem (CM_CLEARQUEUE, MF_GRAYED) ;
  }
  else
  {
    sprintf (str, "C&lear Queue (%d %s)", queued_file_count, queued_file_count == 1 ? "entry" : "entries") ;
    PVModifyMenu (CM_CLEARQUEUE, MF_STRING, CM_CLEARQUEUE, str) ;
    PVEnableMenuItem (CM_CLEARQUEUE, queued_file_count ? MF_ENABLED : MF_GRAYED) ;
  }
  if (write_files)
  {
    PutHKCU("FileQueue", "QueueCount", queued_file_count) ;
    for (i = 0 ; i < MAX_QUEUE ; i++)
    {
      sprintf (str, "QueuedFile%d", i) ;
      PutHKCU("FileQueue", str, i < queued_file_count ? queued_files [i] : NULL) ;
    }
  }
}

void resize_listbox_dialog (HWND hDlg, int idLb, int chars)
{
  int         difference ;
  HWND        hLb ;
  HWND        hBtn ;
  RECT        lbRect ;
  RECT        btnRect ;
  RECT        dlgRect ;

  hLb = GetDlgItem (hDlg, idLb) ;
  hBtn = GetDlgItem (hDlg, IDOK) ;
  GetRelativeClientRect (hDlg, hLb, &lbRect) ;
  GetRelativeClientRect (hDlg, hBtn, &btnRect) ;
  GetWindowRect (hDlg, &dlgRect) ;
  difference = message_xchar * (chars + 2) - (lbRect.right - lbRect.left) ;
  lbRect.right += difference ;
  MoveWindow (hLb, lbRect.left, lbRect.top, lbRect.right - lbRect.left, lbRect.bottom - lbRect.top, true) ;
  btnRect.left += difference / 2 ;
  btnRect.right += difference / 2 ;
  MoveWindow (hBtn, btnRect.left, btnRect.top, btnRect.right - btnRect.left, btnRect.bottom - btnRect.top, true) ;
  dlgRect.right += difference ;
  MoveWindow (hDlg, dlgRect.left, dlgRect.top, dlgRect.right - dlgRect.left, dlgRect.bottom - dlgRect.top, true) ;
}

#if 0
char *save_demo_file (void)
{
  GetTempPath (sizeof (filename), filename) ;
  strcat (filename, "POVDEMO.$$$") ;
  if ((hrsc = FindResource (hInst, MAKEINTRESOURCE (ID_DEMOFILE), RT_RCDATA)) == NULL)
  {
    PovMessageBox ("Cannot locate file resource\r\n(internal error)", "Cannot run demo") ;
    return (NULL) ;
  }
  if ((hglobal = LoadResource (hInst, hrsc)) == NULL)
  {
    PovMessageBox ("Cannot load file resource", "Cannot run demo") ;
    return (NULL) ;
  }
  if ((s = LockResource (hglobal)) == NULL)
  {
    PovMessageBox ("Cannot lock file resource", "Cannot run demo") ;
    return (NULL) ;
  }
  size = SizeofResource (hInst, hrsc) ;
  if ((outH = _lcreat (filename, 0)) == HFILE_ERROR)
  {
    PovMessageBox ("Cannot create temporary file", "Cannot run demo") ;
    return (NULL) ;
  }

  if (_lwrite (outH, s, size) != size)
  {
    PovMessageBox ("Cannot write temporary file", "Cannot run demo") ;
    return (NULL) ;
  }
  _lclose (outH) ;
}
#endif

int splitfn (const char *filename, char *path, char *name, char *ext)
{
  char        *s ;
  char        str [_MAX_PATH] ;
  char        *fn = strncpy (str, filename, _MAX_PATH) ;

  str[_MAX_PATH - 1] = '\0';
  if (path != NULL)
    *path = '\0' ;
  if (name != NULL)
    *name = '\0' ;
  if (ext != NULL)
    *ext = '\0' ;

  if ((s = findLastPathSeparator(fn)) != NULL)
  {
    *s++ = '\0' ;
    if (path)
    {
      strcpy (path, fn) ;
      strcat (path, "\\");
    }
    fn = s;
  }

  if ((s = strrchr (fn, '.')) != NULL)
  {
    if (ext)
      strcpy (ext, s) ;
    *s = '\0' ;
  }

  if (name)
    strcpy (name, fn) ;

  return (0) ;
}

void splitpath (const char *filename, char *path, char *name)
{
  char        str [_MAX_PATH] ;

  splitfn (filename, path, name, str) ;
  if (name != NULL)
    strcat (name, str) ;
}

void load_tool_menu (char *iniFilename)
{
  int         i ;
  int         count ;
  char        str [32] ;
  char        entry [256] ;
  char        *s ;

  memset (tool_commands, 0, sizeof (tool_commands)) ;
  memset (tool_help, 0, sizeof (tool_help)) ;

  DeleteMenu (hToolsMenu, 1, MF_BYCOMMAND) ;
  for (i = 0 ; i < MAX_TOOLCMD ; i++)
    DeleteMenu (hToolsMenu, CM_FIRSTTOOL + i, MF_BYCOMMAND) ;

  for (i = count = 0 ; i < MAX_TOOLCMD ; i++)
  {
    sprintf (str, "Item%d", i) ;

    GetPrivateProfileString ("Command", str, "", entry, sizeof (entry) - 1, iniFilename) ;
    if (strlen (entry) == 0)
      continue ;
    s = entry ;
    while (*s == ' ')
      s++ ;
    if (strlen (s) >= MAX_TOOLCMDTEXT)
    {
      message_printf ("Tool command %s is too long\n", str) ;
      s [MAX_TOOLCMDTEXT - 1] = '\0' ;
    }
    strcpy (tool_commands [count], s) ;

    GetPrivateProfileString ("Menu", str, "", entry, sizeof (entry) - 1, iniFilename) ;
    if (strlen (entry) == 0)
    {
      message_printf ("Tool menu entry %s is missing\n", str) ;
      continue ;
    }
    s = entry ;
    while (*s == ' ')
      s++ ;
    if (strlen (s) > 31)
    {
      message_printf ("Tool menu entry %s is too long\n", str) ;
      continue ;
    }
    if (count == 0)
      AppendMenu (hToolsMenu, MF_SEPARATOR, 1, "-") ;
    AppendMenu (hToolsMenu, MF_STRING, CM_FIRSTTOOL + count, s) ;

    GetPrivateProfileString ("Help", str, "", entry, sizeof (entry) - 1, iniFilename) ;
    if (strlen (entry) == 0)
    {
      count++ ;
      continue ;
    }
    s = entry ;
    while (*s == ' ')
      s++ ;
    if (strlen (s) >= MAX_TOOLHELPTEXT)
    {
      message_printf ("Tool help %s is too long\n", str) ;
      s [MAX_TOOLHELPTEXT - 1] = '\0' ;
    }
    strcpy (tool_help [count++], s) ;
  }
  message_printf ("Loaded %d %s into Tool Menu.\n", count, count != 1 ? "tools" : "tool") ;
}

char *parse_tool_command (char *command)
{
  char                  *s ;
  char                  ExternalStr [512] ;
  static char           str [512] ;

  str [0] = '\0' ;
  while (*command == ' ')
    command++ ;
  strcpy (ExternalStr, command) ;
  ExternalParseToolCommand (ExternalStr) ;
  if (strlen (ExternalStr))
    command = ExternalStr ;
  for (s = str ; *command ; command++)
  {
    if (strlen (str) >= sizeof (str) - _MAX_PATH)
      break ;
    if (*command == '%')
    {
      if (*++command == '%')
      {
        *s++ = *command ;
        continue ;
      }
      switch (toupper (*command))
      {
        case 'I' :
             s += sprintf (s, "%sini\\", DocumentsPath) ;
             break ;

        case 'T' :
             s += sprintf (s, "%s", ToolIniFileName) ;
             break ;

        case 'H' :
             s += sprintf (s, "%s", BinariesPath) ;
             break ;

        case 'P' :
             s += sprintf (s, "%s", DocumentsPath) ;
             break ;

        case 'R' :
             s += joinPath (s, lastRenderPath, lastRenderName) ;
             break ;

        case 'S' :
             s += sprintf (s, "%s", source_file_name) ;
             break ;

        case 'N' :
             s += sprintf (s, "%s", SecondaryRenderIniFileName) ;
             break ;

        case 'D' :
             s += GetHKCU("LastRender", "CurrentDirectory", "", s, _MAX_PATH) ;
             break ;

        case '0' :
             s += GetHKCU("LastRender", "SourceFile", "", s, _MAX_PATH) ;
             break ;

        case '1' :
             s += GetHKCU("LastRender", "OutputFile", "", s, _MAX_PATH) ;
             break ;

        case '2' :
             s += GetHKCU("LastRender", "SceneFile", "", s, _MAX_PATH) ;
             break ;

        case '4' :
             s += GetHKCU("LastRender", "IniOutputFile", "", s, _MAX_PATH) ;
             break ;
      }
      continue ;
    }
    if (s == str && isspace (*command))
      continue ;
    *s++ = *command ;
  }
  *s = '\0' ;
  return (str) ;
}

char *get_elapsed_time (int seconds)
{
  static char str [19] ;

  str [0] = '\0' ;
  sprintf (str, "%ud %02uh %02um %02us",
           seconds / 86400,
           seconds % 86400 / 3600,
           seconds % 3600 / 60,
           seconds % 60) ;
  return (str) ;
}

char *clean (char *s)
{
  static char           str [_MAX_PATH] ;

  while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n' )
    s++ ;
  s = strncpy (str, s, sizeof (str) - 1) ;
  if (*s == '\0')
    return (str) ;
  for (s += strlen (s) - 1 ; *s == ' ' || *s == '\t' || *s == '\r' || *s == '\n' ; s--)
    *s = '\0' ;
  return (str) ;
}

void extract_ini_sections (char *filename, HWND hwnd)
{
  char                  str [256] ;
  char                  *s1 ;
  char                  *s2 ;
  FILE                  *inF ;
  static char           inbuf [16384] ;

  /*
  ** flush the INI file cache
  */
  SendMessage (hwnd, CB_RESETCONTENT, 0, 0L) ;

  if ((inF = fopen (filename, "rt")) == NULL)
    return ;

  setvbuf (inF, inbuf, _IOFBF, sizeof (inbuf)) ;
  while (fgets (str, sizeof (str), inF) != NULL)
  {
    s1 = clean (str) ;
    if (*s1 == '[')
    {
      if ((s2 = strchr (s1, ']')) != NULL)
      {
        *++s2  = '\0' ;
        SendMessage (hwnd, CB_ADDSTRING, 0, (LPARAM) s1) ;
      }
    }
  }
  fclose (inF) ;
  SendMessage (hwnd, CB_SETCURSEL, 0, 0L) ;
}

int select_combo_item_ex (HWND hwnd, char *s)
{
  int         i ;
  char        str [256] ;

  if ((i = SendMessage (hwnd, CB_GETCOUNT, 0, 0)) < 0)
    return (i) ;
  while (i--)
  {
    SendMessage (hwnd, CB_GETLBTEXT, i, (LPARAM) str) ;
    if (strcmp (s, str) == 0)
    {
      SendMessage (hwnd, CB_SETCURSEL, i, 0) ;
      return (i) ;
    }
  }
  return (-1) ;
}

void extract_ini_sections_ex (char *filename, HWND hwnd)
{
  extract_ini_sections (filename, hwnd) ;
}

bool PovInvalidateRect (HWND hWnd, CONST RECT *lpRect, bool bErase)
{
  if (hWnd != NULL)
    return (InvalidateRect (hWnd, lpRect, bErase) != 0) ;
  return (0) ;
}

bool TaskBarAddIcon (HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip)
{
  NOTIFYICONDATA        tnid ;

  memset (&tnid, 0, sizeof (tnid)) ;
  tnid.cbSize = sizeof(NOTIFYICONDATA) ;
  tnid.hWnd = hwnd ;
  tnid.uID = uID ;
  tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP ;
  tnid.uCallbackMessage = TASKBAR_NOTIFY_MESSAGE ;
  tnid.hIcon = hicon ;
  lstrcpyn (tnid.szTip, lpszTip, sizeof(tnid.szTip)) ;

  return (Shell_NotifyIcon (NIM_ADD, &tnid) != 0) ;
}

bool TaskBarModifyIcon (HWND hwnd, UINT uID, LPSTR lpszTip)
{
  NOTIFYICONDATA        tnid ;

  memset (&tnid, 0, sizeof (tnid)) ;
  tnid.cbSize = sizeof(NOTIFYICONDATA) ;
  tnid.hWnd = hwnd ;
  tnid.uID = uID ;
  tnid.uFlags = NIF_TIP ;
  lstrcpyn (tnid.szTip, lpszTip, sizeof(tnid.szTip)) ;

  return (Shell_NotifyIcon (NIM_MODIFY, &tnid) != 0) ;
}

bool TaskBarDeleteIcon (HWND hwnd, UINT uID)
{
  NOTIFYICONDATA        tnid ;

  memset (&tnid, 0, sizeof (tnid)) ;
  tnid.cbSize = sizeof(NOTIFYICONDATA) ;
  tnid.hWnd = hwnd ;
  tnid.uID = uID ;

  return (Shell_NotifyIcon (NIM_DELETE, &tnid) != 0) ;
}

bool ShowRestrictionMessage (char *Message, char *Dir)
{
  int         i ;
  char        str [8192] ;

  // the main window code will expect this to be already set up if it gets a WM_HELP
  hh_aklink.pszKeywords = "I/O Restrictions" ;

  if (Dir == NULL)
  {
    sprintf (str, "%s\nPress the HELP button to learn about I/O Restrictions.", Message) ;
    MessageBox (main_window, str, "I/O Restriction Activated", MB_ICONERROR | MB_OK | MB_HELP) ;
    return (false) ;
  }

  sprintf (str, "%s\n"
                "Press OK to grant temporary read/write permission in that directory (and its subdirectories).\n"
                "Press CANCEL to halt the render.\n\n"
                "Press the HELP button to learn about I/O Restrictions.",
                Message) ;

  if (MessageBox (main_window, str, "I/O Restriction Activated", MB_ICONSTOP | MB_OKCANCEL | MB_HELP | MB_DEFBUTTON2) != IDOK)
    return (false) ;

  for (i = 0 ; i < MAX_DIRSPEC - 1 ; i++)
    if (WriteDirSpecs [i] == NULL)
      break ;
  if (i == MAX_DIRSPEC - 1)
  {
    MessageBox (main_window, "Ran out of room to store directory grant permission", "I/O Restriction Error", MB_ICONERROR | MB_OK) ;
    // return true anyhow
    return (true) ;
  }

  WriteDirSpecs [i] = _strdup (Dir) ;
  return (true) ;
}

}
// end of namespace povwin
