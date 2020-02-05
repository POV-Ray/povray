//******************************************************************************
///
/// @file windows/pvdialog.cpp
///
/// This module implements dialog-box routines for the Windows build of POV.
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
#include <commdlg.h>
#include <commctrl.h>
#include <htmlhelp.h>
#include <setjmp.h>
#include <string.h>
#include "pvengine.h"
#include "resource.h"
#include "pvedit.h"
#include "pvguiext.h"
#include "pvdisplay.h"
#include "pvdialog.h"

// this must be the last file included
#include "syspovdebug.h"

namespace povwin
{

bool                    otaChecked ;
const char              *otaTitle ;
const char              *otaText ;
const char              *otaHelpString ;


extern int              listbox_ychar ;
extern int              tb_combo_sel ;
extern char             engineHelpPath [_MAX_PATH] ;
extern char             lastRenderName [_MAX_PATH] ;
extern char             lastRenderPath [_MAX_PATH] ;
extern char             lastBitmapPath [_MAX_PATH] ;
extern char             lastQueuePath [_MAX_PATH] ;
extern char             lastSecondaryIniFilePath [_MAX_PATH] ;
extern char             DefaultRenderIniFileName [_MAX_PATH] ;
extern char             SecondaryRenderIniFileName [_MAX_PATH] ;
extern char             RegionStr [128] ;
extern char             TempRegionStr [128] ;
extern char             command_line [_MAX_PATH * 3] ;
extern char             old_command_line [_MAX_PATH * 3] ;
extern char             SecondaryRenderIniFileSection [64] ;
extern char             source_file_name [_MAX_PATH] ;
extern char             render_complete_sound [_MAX_PATH] ;
extern char             parse_error_sound [_MAX_PATH] ;
extern char             render_error_sound [_MAX_PATH] ;
extern char             queued_files [MAX_QUEUE] [_MAX_PATH] ;
extern bool             IsW95UserInterface ;
extern bool             use_editors;
extern bool             running_demo ;
extern bool             render_complete_sound_enabled ;
extern bool             parse_error_sound_enabled  ;
extern bool             render_error_sound_enabled ;
extern HWND             render_window ;
extern HWND             toolbar_window ;
extern HWND             aux_toolbar_window ;
extern HWND             window_list [MAX_WINDOWS] ;
extern HWND             toolbar_combobox ;
extern HWND             rebar_window ;
extern HWND             StatusWindow ;
extern HWND             toolbar_cmdline ;
extern HWND             tab_window ;
extern HWND             main_window ;
extern HMENU            hMainMenu ;
extern unsigned         screen_width ;
extern unsigned         screen_height ;
extern unsigned         renderwin_8bits ;
extern unsigned         auto_render ;
extern unsigned         queued_file_count ;
extern unsigned         ThreadCount ;
extern HH_AKLINK        hh_aklink ;

// include prototype here to avoid the need to include commdlg.h in all files
extern void init_ofn (OPENFILENAME *ofn, HWND hWnd, char *title, char *name, int maxlen, char *lastPath, char *defaultExt);

char *GetINIFile (HWND hWnd, char *path)
{
  int           result ;
  OPENFILENAME  ofnTemp ;
  static char   name [_MAX_PATH] ;

  validatePath (path) ;
  init_ofn (&ofnTemp, hWnd, "Choose INI File", name, sizeof (name), path, "ini") ;
  ofnTemp.lpstrFilter = "INI files (*.ini)\0*.ini\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != 0)
  {
    strcpy (path, name) ;
    path [ofnTemp.nFileOffset - 1] = '\0' ;
  }
  return (result ? name : NULL) ;
}

void AddQueue (HWND hWnd, HWND hlb)
{
  int           queue_count ;
  char          name [8192] ;
  char          *s ;
  char          str [_MAX_PATH] ;
  OPENFILENAME  ofnTemp ;

  queue_count = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
  if (queue_count >= MAX_QUEUE)
  {
    PovMessageBox ("File queue is full", "Cannot add any more files!") ;
    return ;
  }
  strcpy (name, lastRenderName) ;
  name [strlen (name) + 1] = '\0' ;
  validatePath (lastQueuePath) ;
  init_ofn (&ofnTemp, hWnd, "Add to Queue", name, sizeof (name), lastQueuePath, "pov") ;
  ofnTemp.lpstrFilter = "POV source and INI (*.pov;*.ini)\0*.pov;*.ini\0POV files (*.pov)\0*.pov\0INI files (*.ini)\0*.ini\0All Files (*.*)\0*.*\0" ;
  ofnTemp.Flags |= OFN_ALLOWMULTISELECT ;
  if (GetOpenFileName (&ofnTemp) != false)
  {
    // convert spaces into NULL's if we're not using the new interface so it works with the below code
    if (!IsW95UserInterface)
      for (s = name ; *s ; s++)
        if (*s == ' ')
          *s = '\0' ;
    if (ofnTemp.nFileOffset < strlen (name))
    {
      strcpy (lastQueuePath, name) ;
      lastQueuePath [ofnTemp.nFileOffset - 1] = '\0' ;
      SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) name) ;
    }
    else
    {
      s = name ;
      strcpy (lastQueuePath, name) ;
      for (s += strlen (s) + 1 ; *s ; s += strlen (s) + 1)
      {
        if (queue_count++ >= MAX_QUEUE)
        {
          PovMessageBox ("File queue is full", "Cannot add any more files!") ;
          return ;
        }
        joinPath (str, lastQueuePath, s) ;
        _strlwr (str) ;
        SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) str) ;
      }
    }
  }
}

char *SelectSound (HWND hWnd, char *currentSound)
{
  int           result ;
  char          path [_MAX_PATH] ;
  static char   name [_MAX_PATH] ;
  OPENFILENAME  ofnTemp ;

  splitpath (currentSound, path, name) ;
  validatePath (path) ;
  init_ofn (&ofnTemp, hWnd, "Select Sound File", name, sizeof (name), path, "wav") ;
  ofnTemp.Flags &= ~OFN_HIDEREADONLY ;
  ofnTemp.lpstrFilter = "Sound Files (*.wav)\0*.wav\0All Files (*.*)\0*.*\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != 0)
    return (name) ;
  return (NULL) ;
}

SIZE *GetTextExtent (HWND hWnd, LPCSTR text)
{
  int         result ;
  HDC         hDC ;
  HFONT       hFont = (HFONT) SendMessage (hWnd, WM_GETFONT, 0, 0) ;
  static SIZE extent ;

  if (hFont == NULL)
    return (0) ;
  hDC = GetDC (hWnd) ;
  HFONT hOldFont = (HFONT) SelectObject (hDC, hFont);
  result = GetTextExtentPoint32 (hDC, text, (int) strlen(text), &extent) ;
  SelectObject (hDC, hOldFont) ;
  ReleaseDC (hWnd, hDC) ;
  if (!result)
    return (0) ;
  return (&extent) ;
}

int GetTextWidth (HWND hWnd, LPCSTR text)
{
  int         width = 0;
  int         result = 0;
  SIZE        extent;
  HDC         hDC ;
  HFONT       hFont = (HFONT) SendMessage (hWnd, WM_GETFONT, 0, 0) ;
  const char  *s;

  if (hFont == NULL)
    return (0) ;
  hDC = GetDC (hWnd) ;
  HFONT hOldFont = (HFONT) SelectObject (hDC, hFont);
  while (strlen(text) > 0)
  {
    if ((s = strchr(text, '\n')) == NULL)
      s = text + strlen(text) - 1;
    if ((result = GetTextExtentPoint32 (hDC, text, (int) (s - text), &extent)) == 0)
      return -1;
    text = s + 1;
    if (extent.cx > width)
      width = extent.cx;
  }
  SelectObject (hDC, hOldFont) ;
  ReleaseDC (hWnd, hDC) ;
  return width;
}

void NudgeChildWindow (HWND hDlg, int idItem, int pixels)
{
  RECT        rect ;
  HWND        hWnd ;

  hWnd = GetDlgItem (hDlg, idItem) ;
  GetWindowRect (hWnd, &rect) ;
  ScreenToClient (hDlg, (LPPOINT) &rect) ;
  SetWindowPos (hWnd, NULL, rect.left + pixels, rect.top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER) ;
}

INT_PTR CALLBACK PovCommandLineDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  char        *s ;
  char        str [_MAX_PATH * 2] ;
  char        str1 [_MAX_PATH] ;
  char        str2 [_MAX_PATH] ;
  static char editINIname [_MAX_PATH] ;
  static char editFilename [_MAX_PATH] ;
  static char lastIniFilename [_MAX_PATH] ;
  static bool had_region = false ;
  static struct stat statbuf1 ;
  static struct stat statbuf2 ;

  switch (message)
  {
    case WM_INITDIALOG :
         had_region = false ;
         hh_aklink.pszKeywords = "command-line options" ;
         if (use_editors)
         {
           if ((EditGetFlags () & EDIT_MSG_SELECTED) == 0)
           {
             if ((s = EditGetFilename(false)) != NULL)
             {
               EnableWindow (GetDlgItem (hDlg, IDC_PRESETSOURCEPATH), false) ;
               EnableWindow (GetDlgItem (hDlg, IDC_PRESETSOURCENAME), false) ;
               EnableWindow (GetDlgItem (hDlg, IDC_SOURCEDEFAULT), false) ;
               EnableWindow (GetDlgItem (hDlg, IDC_BROWSESOURCEFILE), false) ;
               EnableWindow (GetDlgItem (hDlg, IDC_EDITRENDER), false) ;
               splitpath (s, lastRenderPath, lastRenderName) ;
               EnableWindow (GetDlgItem (hDlg, IDC_EDITRENDER), false) ;
             }
           }
           SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, lastRenderPath) ;
           SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, lastRenderName) ;
         }
         else
         {
           SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, lastRenderPath) ;
           SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, lastRenderName) ;
           EnableWindow (GetDlgItem (hDlg, IDC_EDITRENDER), false) ;
           EnableWindow (GetDlgItem (hDlg, IDC_EDITINI), false) ;
         }
         SendDlgItemMessage (hDlg, IDC_PRESETSOURCENAME, EM_LIMITTEXT, 64, 0L) ;
         SendDlgItemMessage (hDlg, IDC_INIFILENAME, EM_LIMITTEXT, 64, 0L) ;
         _strupr (SecondaryRenderIniFileName) ;
         validatePath (lastRenderPath) ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         if (strlen (TempRegionStr))
         {
           if (strlen (command_line))
           {
             strcpy (str, command_line) ;
             strcat (str, TempRegionStr) ;
           }
           else
             strcpy (str, TempRegionStr + 1) ;
           strcpy (RegionStr, TempRegionStr) ;
           TempRegionStr [0] = '\0' ;
           SetDlgItemText (hDlg, IDC_COMMANDLINE, str) ;
         }
         else
         {
           SetDlgItemText (hDlg, IDC_COMMANDLINE, command_line) ;
           if (RegionStr [0])
             if (strstr (command_line, RegionStr + 1) != NULL)
               had_region = true ;
         }
         if (SecondaryRenderIniFileName [0] != '\0')
         {
           if (!hasTrailingPathSeparator(SecondaryRenderIniFileName))
           {
             splitpath (SecondaryRenderIniFileName, str1, str2) ;
             validatePath (str1) ;
             strcpy (editINIname, str2) ;
             SetDlgItemText (hDlg, IDC_INIFILEPATH, str1) ;
             SetDlgItemText (hDlg, IDC_INIFILENAME, str2) ;
             extract_ini_sections (SecondaryRenderIniFileName, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
             SendMessage (toolbar_combobox, CB_GETLBTEXT, SendMessage (toolbar_combobox, CB_GETCURSEL, 0, 0), (LPARAM) SecondaryRenderIniFileSection) ;
             SendDlgItemMessage (hDlg, IDC_INIFILESECTION, CB_SELECTSTRING, -1, (LPARAM) SecondaryRenderIniFileSection) ;
             strcpy (lastIniFilename, SecondaryRenderIniFileName) ;
             stat (SecondaryRenderIniFileName, &statbuf1) ;
           }
           else
             SetDlgItemText (hDlg, IDC_INIFILEPATH, SecondaryRenderIniFileName) ;
         }
         SetFocus (GetDlgItem (hDlg, IDC_COMMANDLINE)) ;
         return (false) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return (DefWindowProc (hDlg, message, wParam, lParam)) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDC_INIFILESECTION :
                if (HIWORD (wParam) == CBN_SETFOCUS)
                {
                  stat (lastIniFilename, &statbuf2) ;
                  if (statbuf1.st_atime != statbuf2.st_atime)
                  {
                    statbuf1 = statbuf2 ;
                    GetDlgItemText (hDlg, IDC_INIFILESECTION, str, sizeof (str)) ;
                    extract_ini_sections (lastIniFilename, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
                    SendDlgItemMessage (hDlg, IDC_INIFILESECTION, CB_SELECTSTRING, -1, (LPARAM) str) ;
                  }
                  return (true) ;
                }
                return (false) ;

           case IDC_EDITRENDER :
                GetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, str1, sizeof (str1)) ;
                GetDlgItemText (hDlg, IDC_PRESETSOURCENAME, str2, sizeof (str2)) ;
                validatePath (str1) ;
                strcat (str1, "\\") ;
                strcat (str1, str2) ;
                if (EditOpenFile (str1))
                {
                  EndDialog (hDlg, false) ;
                  return (true) ;
                }
                return (true) ;

           case IDC_EDITINI :
                GetDlgItemText (hDlg, IDC_INIFILEPATH, str1, sizeof (str1)) ;
                GetDlgItemText (hDlg, IDC_INIFILENAME, str2, sizeof (str2)) ;
                validatePath (str1) ;
                strcat (str1, "\\") ;
                strcat (str1, str2) ;
                if (EditOpenFile (str1))
                {
                  EndDialog (hDlg, false) ;
                  return (true) ;
                }
                return (true) ;

           case IDC_BROWSEINIFILE :
                if ((s = GetINIFile (hDlg, lastSecondaryIniFilePath)) != NULL)
                {
                  _strupr (s) ;
                  splitpath (s, str1, str2) ;
                  validatePath (str1) ;
                  SetDlgItemText (hDlg, IDC_INIFILEPATH, str1) ;
                  SetDlgItemText (hDlg, IDC_INIFILENAME, str2) ;
                  if (strcmp (s, lastIniFilename))
                  {
                    extract_ini_sections (s, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
                    strcpy (lastIniFilename, s) ;
                    stat (lastIniFilename, &statbuf1) ;
                  }
                }
                return (true) ;

           case IDC_INIFILENAME :
                if (HIWORD (wParam) == EN_KILLFOCUS)
                {
                  GetDlgItemText (hDlg, IDC_INIFILEPATH, str1, sizeof (str1)) ;
                  GetDlgItemText (hDlg, IDC_INIFILENAME, str2, sizeof (str2)) ;
                  validatePath (str1) ;
                  joinPath (str, str1, str2) ;
                  if (_stricmp (str, lastIniFilename))
                  {
                    extract_ini_sections (str, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
                    strcpy (lastIniFilename, str) ;
                  }
                  return (true) ;
                }
                if (HIWORD (wParam) == EN_UPDATE)
                {
                  GetDlgItemText (hDlg, IDC_INIFILENAME, str, sizeof (str)) ;
                  if (strchr (str, '\\') != NULL) // TODO FIXME - shouldn't we also reject '/' and strings starting drive letter?
                    SetDlgItemText (hDlg, IDC_INIFILENAME, editINIname) ;
                  else
                    strcpy (editINIname, str) ;
                }
                return (true) ;

           case IDC_RESETINI :
                SetDlgItemText (hDlg, IDC_INIFILENAME, "") ;
                GetDlgItemText (hDlg, IDC_INIFILEPATH, lastIniFilename, sizeof (lastIniFilename)) ;
                SendMessage (GetDlgItem (hDlg, IDC_INIFILESECTION), CB_RESETCONTENT, 0, 0L) ;
                return (true) ;

           case IDC_INIDEFAULT :
                sprintf (str, "%sini", DocumentsPath) ;
                SetDlgItemText (hDlg, IDC_INIFILEPATH, str) ;
                SetDlgItemText (hDlg, IDC_INIFILENAME, "quickres.ini") ;
                SendMessage (hDlg, WM_COMMAND, (EN_KILLFOCUS << 16) | IDC_INIFILENAME, 0L) ;
                return (true) ;

           case IDC_PRESETSOURCENAME :
                if (HIWORD (wParam) == EN_UPDATE)
                {
                  GetDlgItemText (hDlg, IDC_PRESETSOURCENAME, str, sizeof (str)) ;
                  if (strchr (str, '\\') != NULL) // TODO FIXME - shouldn't we also reject '/' and strings starting drive letter?
                    SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, editFilename) ;
                  else
                    strcpy (editFilename, str) ;
                }
                return (true) ;

           case IDC_BROWSESOURCEFILE :
                if ((s = file_open (hDlg)) != NULL)
                {
                  splitpath (s, str1, str2) ;
                  validatePath (str1) ;
                  SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, str1) ;
                  SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, str2) ;
                }
                return (true) ;

           case IDC_SOURCEDEFAULT :
                sprintf (str, "%sScenes\\advanced", DocumentsPath) ;
                SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, str) ;
                SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, "Biscuit.pov") ;
                return (true) ;

           case IDC_SET :
           case IDC_RENDER :
           case IDC_RENDERa:    // IDC_RENDERa is defined because the render dialog
                                // has 2 render buttons on it, and VC++ complains
                if (!running_demo)
                {
                  GetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, lastRenderPath, sizeof (lastRenderPath)) ;
                  GetDlgItemText (hDlg, IDC_PRESETSOURCENAME, lastRenderName, sizeof (lastRenderName)) ;
                  validatePath (lastRenderPath) ;
//                _strupr (lastRenderPath) ;
//                _strupr (lastRenderName) ;
                  joinPath (source_file_name, lastRenderPath, lastRenderName) ;
                }
                GetDlgItemText (hDlg, IDC_INIFILEPATH, str1, sizeof (str1)) ;
                GetDlgItemText (hDlg, IDC_INIFILENAME, str2, sizeof (str2)) ;
                validatePath (str1) ;
                strcpy (lastSecondaryIniFilePath, str1) ;
                joinPath (SecondaryRenderIniFileName, str1, str2) ;
                _strupr (SecondaryRenderIniFileName) ;
                GetDlgItemText (hDlg, IDC_INIFILESECTION, SecondaryRenderIniFileSection, sizeof (SecondaryRenderIniFileSection)) ;
                GetDlgItemText (hDlg, IDC_COMMANDLINE, command_line, sizeof (command_line)) ;
                SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;
                extract_ini_sections_ex (SecondaryRenderIniFileName, toolbar_combobox) ;
                SendMessage (toolbar_combobox, CB_ADDSTRING, 0, (LPARAM) "More Resolutions ...") ;
                tb_combo_sel = select_combo_item_ex (toolbar_combobox, SecondaryRenderIniFileSection) ;
                if (tb_combo_sel == -1)
                  tb_combo_sel = 0 ;

                // was there a region string on the command line before it was edited ?
                if (had_region)
                {
                  // if so, is it still there now, in an unchanged form ?
                  // (we look at RegionStr + 1 since they always start with a space).
                  if (strstr (command_line, RegionStr + 1) == NULL)
                  {
                    // it's not, so we remove the region string.
                    RegionStr [0] = '\0' ;
                  }
                }

                if (LOWORD (wParam) == IDC_RENDER || LOWORD(wParam) == IDC_RENDERa)
                {
                  if (EditSaveModified (NULL) == 0)
                    return (true) ;
                  EndDialog (hDlg, true) ;
                }
                else
                  EndDialog (hDlg, false) ;
                return (true) ;

           case IDC_COMMANDHELP :
                hh_aklink.pszKeywords = "command-line dialog" ;
                HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
                return (true) ;

           case IDCANCEL :
                EndDialog (hDlg, false) ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

INT_PTR CALLBACK PovShortCommandLineDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  static bool had_region = false ;

  switch (message)
  {
    case WM_INITDIALOG :
         had_region = false ;
         hh_aklink.pszKeywords = "command-line options" ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         SetDlgItemText (hDlg, IDC_COMMANDLINE, command_line) ;
         if (RegionStr [0])
           if (strstr (command_line, RegionStr + 1) != NULL)
             had_region = true ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return (DefWindowProc (hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDC_RENDER :
           case IDC_RENDERa:
                GetDlgItemText (hDlg, IDC_COMMANDLINE, command_line, sizeof (command_line) - 1) ;
                SendMessage (toolbar_cmdline, WM_SETTEXT, 0, (LPARAM) command_line) ;

                // was there a region string on the command line before it was edited ?
                if (had_region)
                {
                  // if so, is it still there now, in an unchanged form ?
                  // (we look at RegionStr + 1 since they always start with a space).
                  if (strstr (command_line, RegionStr + 1) == NULL)
                  {
                    // it's not, so we remove the region string.
                    RegionStr [0] = '\0' ;
                  }
                }

                if (LOWORD (wParam) == IDC_RENDER || LOWORD(wParam) == IDC_RENDERa)
                {
                  if (EditSaveModified (NULL) == 0)
                    return (true) ;
                  EndDialog (hDlg, true) ;
                }
                else
                  EndDialog (hDlg, false) ;

                return (true) ;

           case IDCANCEL :
                EndDialog (hDlg, false) ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

INT_PTR CALLBACK PovFileQueueDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   i ;
  char                  str [64] ;
  HWND                  hlb ;
  DRAWITEMSTRUCT        *d ;
  MEASUREITEMSTRUCT     *m ;
  static HBRUSH         hbr ;

  switch (message)
  {
    case WM_INITDIALOG :
         hh_aklink.pszKeywords = "File Queue" ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
         hbr = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
         for (i = 0 ; i < queued_file_count ; i++)
           SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) queued_files [i]) ;
         sprintf (str, "Queue has %d entr%s", queued_file_count, queued_file_count != 1 ? "ies" : "y") ;
         SetDlgItemText (hDlg, IDC_QUEUEENTRIES, str) ;
         CheckDlgButton (hDlg, IDC_RELOADQUEUE, GetHKCU("FileQueue", "ReloadOnStartup", 0)) ;
         CheckDlgButton (hDlg, IDC_AUTORENDER, auto_render) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return(DefWindowProc(hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
                hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
                queued_file_count = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
                if (queued_file_count > MAX_QUEUE)
                  queued_file_count = MAX_QUEUE ;
                for (i = 0 ; i < queued_file_count ; i++)
                  SendMessage (hlb, LB_GETTEXT, i, (LPARAM) queued_files [i]) ;
                auto_render = IsDlgButtonChecked (hDlg, IDC_AUTORENDER) ;
                PVCheckMenuItem (CM_AUTORENDER, auto_render ? MF_CHECKED : MF_UNCHECKED) ;
                PutHKCU ("FileQueue", "ReloadOnStartup", IsDlgButtonChecked (hDlg, IDC_RELOADQUEUE)) ;
                update_queue_status (true) ;
                DeleteObject (hbr) ;
                EndDialog (hDlg, true) ;
                return (true) ;

           case IDCANCEL :
                DeleteObject (hbr) ;
                EndDialog (hDlg, false) ;
                return (true) ;

           case IDC_DELETEFILE :
                hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
                if ((i = SendMessage (hlb, LB_GETCURSEL, 0, 0)) != LB_ERR)
                {
                  SendMessage (hlb, LB_DELETESTRING, i, 0) ;
                  if (i)
                    i-- ;
                  SendMessage (hlb, LB_SETCURSEL, i, 0) ;
                }
                i = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
                sprintf (str, "Queue will have %d entr%s", i, i != 1 ? "ies" : "y") ;
                SetDlgItemText (hDlg, IDC_QUEUEENTRIES, str) ;
                return (true) ;

           case IDC_ADDFILE :
                hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
                AddQueue (hDlg, hlb) ;
                i = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
                sprintf (str, "Queue will have %d entr%s", i, i != 1 ? "ies" : "y") ;
                SetDlgItemText (hDlg, IDC_QUEUEENTRIES, str) ;
                return (true) ;

           case IDC_CONTEXTHELP :
                hh_aklink.pszKeywords = "File Queue" ;
                HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
                return (true) ;
         }
         break ;

    case WM_MEASUREITEM :
         if (wParam == IDC_FILEQUEUE)
         {
           m = (MEASUREITEMSTRUCT *) lParam ;
           m->itemHeight = listbox_ychar ;
           return (true) ;
         }
         else
           return (false) ;

    case WM_DRAWITEM :
         if (wParam == IDC_FILEQUEUE)
         {
           d = (DRAWITEMSTRUCT *) lParam ;
           draw_ordinary_listbox (d, true) ;
           return (true) ;
         }
         else
           return (false) ;
  }
  return (false) ;
}

INT_PTR CALLBACK PovFeatureAdviceDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG :
         hh_aklink.pszKeywords = otaHelpString ;
         SetWindowText (hDlg, otaTitle) ;
         SetDlgItemText (hDlg, IDC_ADVICETEXT, otaText) ;
         CheckDlgButton (hDlg, IDC_DONTTELLMEAGAIN, otaChecked ? BST_CHECKED : BST_UNCHECKED) ;
         if (lParam == 0)
           lParam = (LPARAM) GetDesktopWindow () ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return (DefWindowProc (hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
                otaChecked = IsDlgButtonChecked (hDlg, IDC_DONTTELLMEAGAIN) == BST_CHECKED ;
                EndDialog (hDlg, true) ;
                return (true) ;

           case IDCANCEL :
                otaChecked = false ;
                EndDialog (hDlg, false) ;
                return (true) ;

           case IDC_CONTEXTHELP :
                hh_aklink.pszKeywords = otaHelpString ;
                HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
                return (true) ;
         }
         break ;

  }
  return (false) ;
}

INT_PTR CALLBACK PovSoundsDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  char        *s ;

  switch (message)
  {
    case WM_INITDIALOG :
         hh_aklink.pszKeywords = "sounds" ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         SetDlgItemText (hDlg, IDC_SOUND_RENDERCOMPLETE, render_complete_sound) ;
         SetDlgItemText (hDlg, IDC_SOUND_PARSEERROR, parse_error_sound) ;
         SetDlgItemText (hDlg, IDC_SOUND_RENDERERROR, render_error_sound) ;
         CheckDlgButton (hDlg, IDC_ENABLE_RENDERCOMPLETESOUND, render_complete_sound_enabled ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_ENABLE_PARSEERRORSOUND, parse_error_sound_enabled ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_ENABLE_RENDERERRORSOUND, render_error_sound_enabled ? BST_CHECKED : BST_UNCHECKED) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return(DefWindowProc(hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDC_BROWSE_RENDERCOMPLETESOUND:
                if ((s = SelectSound (hDlg, render_complete_sound)) != NULL)
                  SetDlgItemText (hDlg, IDC_SOUND_RENDERCOMPLETE, s) ;
                return (true) ;

           case IDC_BROWSE_PARSEERRORSOUND:
                if ((s = SelectSound (hDlg, parse_error_sound)) != NULL)
                  SetDlgItemText (hDlg, IDC_SOUND_PARSEERROR, s) ;
                return (true) ;

           case IDC_BROWSE_RENDERERRORSOUND:
                if ((s = SelectSound (hDlg, render_error_sound)) != NULL)
                  SetDlgItemText (hDlg, IDC_SOUND_RENDERERROR, s) ;
                return (true) ;

           case IDOK :
                render_complete_sound_enabled = IsDlgButtonChecked (hDlg, IDC_ENABLE_RENDERCOMPLETESOUND) == BST_CHECKED ;
                parse_error_sound_enabled = IsDlgButtonChecked (hDlg, IDC_ENABLE_PARSEERRORSOUND) == BST_CHECKED ;
                render_error_sound_enabled = IsDlgButtonChecked (hDlg, IDC_ENABLE_RENDERERRORSOUND) == BST_CHECKED ;
                GetDlgItemText (hDlg, IDC_SOUND_RENDERCOMPLETE, render_complete_sound, _MAX_PATH) ;
                GetDlgItemText (hDlg, IDC_SOUND_PARSEERROR, parse_error_sound, _MAX_PATH) ;
                GetDlgItemText (hDlg, IDC_SOUND_RENDERERROR, render_error_sound, _MAX_PATH) ;
                EndDialog (hDlg, true) ;
                return (true) ;

           case IDCANCEL :
                EndDialog (hDlg, false) ;
                return (true) ;

           case IDC_CONTEXTHELP :
                hh_aklink.pszKeywords = "sounds" ;
                HtmlHelp (NULL, engineHelpPath, HH_KEYWORD_LOOKUP, (DWORD_PTR) &hh_aklink) ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

INT_PTR CALLBACK PovThreadCountDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int         val;
  BOOL        result;

  switch (message)
  {
    case WM_INITDIALOG :
         hh_aklink.pszKeywords = "thread count" ; // TODO
         SetDlgItemInt (hDlg, IDC_THREADCOUNT, ThreadCount, FALSE) ;
         SendDlgItemMessage (hDlg, IDC_THREADCOUNT, EM_LIMITTEXT, 3, 0L) ;
         SendDlgItemMessage (hDlg, IDC_THREADSPIN, UDM_SETRANGE, 0, MAKELONG(255, 1)) ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         FitWindowInWindow (NULL, hDlg) ;
         return (true) ;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
         return (DefWindowProc (hDlg, message, wParam, lParam));

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDC_THREADCOUNT :
                if (HIWORD (wParam) == EN_CHANGE)
                {
                  if (SendDlgItemMessage (hDlg, IDC_THREADCOUNT, EM_LINELENGTH, 0, 0) > 0)
                  {
                    val = GetDlgItemInt (hDlg, IDC_THREADCOUNT, &result, FALSE) ;
                    if (result == FALSE || val < 1 || val > 255)
                    {
                      if (result == FALSE)
                        SetDlgItemInt (hDlg, IDC_THREADCOUNT, ThreadCount, FALSE) ;
                      else if (val < 1)
                        SetDlgItemInt (hDlg, IDC_THREADCOUNT, 1, FALSE) ;
                      else if (val > 255)
                        SetDlgItemInt (hDlg, IDC_THREADCOUNT, 255, FALSE) ;
                      SendDlgItemMessage (hDlg, IDC_THREADCOUNT, EM_SETSEL, 0, -1);
                    }
                    EnableWindow (GetDlgItem (hDlg, IDOK), TRUE);
                  }
                  else
                    EnableWindow (GetDlgItem (hDlg, IDOK), FALSE);
                  return (true);
                }
                return (false) ;

           case IDOK :
                val = GetDlgItemInt (hDlg, IDC_THREADCOUNT, &result, FALSE) ;
                if (result == TRUE && val > 0 && val < 256)
                  ThreadCount = val ;
                EndDialog (hDlg, true) ;
                return (true) ;

           case IDCANCEL :
                EndDialog (hDlg, false) ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

INT_PTR CALLBACK RenderAlternativeFileDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   width ;
  int                   max = 0;
  int                   diff ;
  char                  str [8192] ;
  char                  *s = str;
  char                  *s2;
  char                  srcname[_MAX_PATH];
  char                  altname[_MAX_PATH];
  RECT                  rect ;
  HWND                  hText ;
  const char            *src = source_file_name;
  const char            *alt = (LPCSTR) lParam;

  switch (message)
  {
    case WM_INITDIALOG :
         splitpath(src, NULL, srcname);
         splitpath(alt, NULL, altname);
         hText = GetDlgItem (hDlg, IDC_ALTERNATEFILETEXT) ;
         s += sprintf(s, "You have requested to render '%s'.\n\n", src);
         max = GetTextExtent(hText, str)->cx;
         s += sprintf(s, "This is an include file that has been used by another render during this session. ");
         s += sprintf(s, "The most recent use of %s was when rendering the below file:\n\n", srcname);
         s += sprintf(s2 = s, "\t%s\n\n", alt);
         if ((width = GetTextExtent(hText, s2)->cx) > max)
           max = width;
         s += sprintf(s, "If you prefer, POV-Ray will render %s instead of %s.\n\n", altname, srcname);
         s += sprintf(s, "If you select \"Yes, For This Session\", POV will render %s and remember your choice for this session. ", altname);
         s += sprintf(s, "If you select \"Yes, This Time\", you will be asked next time you render %s. ", srcname);
         s += sprintf(s, "If you select \"No, Don't Ask Again\", POV will render %s and won't bug you about it for the remainder of this session.\n", srcname);
         SetDlgItemText (hDlg, IDC_ALTERNATEFILETEXT, str) ;
         if (max <= 0)
           return (false) ;
         GetClientRect (hText, &rect) ;
         if (rect.right >= max + 10)
           return (false) ;
         diff = max - rect.right + 10 ;
         SetWindowPos (hText, NULL, 0, 0, rect.right + diff, rect.bottom, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER) ;
         GetWindowRect (hDlg, &rect) ;
         rect.right -= rect.left ;
         rect.bottom -= rect.top ;
         rect.right += diff ;
         SetWindowPos (hDlg, NULL, 0, 0, rect.right + 10, rect.bottom, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER) ;
         diff /= 2 ;
         NudgeChildWindow (hDlg, IDYES, diff) ;
         NudgeChildWindow (hDlg, IDOK, diff) ;
         NudgeChildWindow (hDlg, IDNO, diff) ;
         NudgeChildWindow (hDlg, IDHELP, diff) ;
         return (false) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDYES :
           case IDOK :
           case IDNO :
           case IDCANCEL :
                EndDialog (hDlg, LOWORD(wParam)) ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

void FeatureNotify (const char *labelStr, const char *titleStr, const char *textStr, const char *helpStr, bool checked)
{
  bool        result = false ;

  if (GetDontShowAgain (labelStr))
    return ;
  otaTitle = titleStr ;
  otaText = textStr ;
  hh_aklink.pszKeywords = otaHelpString = helpStr ;
  otaChecked = checked ;
  if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_FEATUREADVICE), main_window, (DLGPROC) PovFeatureAdviceDialogProc, (LPARAM) main_window) > 0)
    if (otaChecked)
      PutDontShowAgain (labelStr, true) ;
}

}
// end of namespace povwin
