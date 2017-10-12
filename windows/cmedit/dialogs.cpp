//******************************************************************************
///
/// @file windows/cmedit/dialogs.cpp
///
/// This file is part of the CodeMax editor support code.
///
/// @author Christopher J. Cason
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

#include "cmedit.h"
#include "ccodemax.h"
#include "settings.h"
#include "eventhandlers.h"
#include "editorinterface.h"
#include "dialogs.h"
#include "..\pvedit.h"

extern int              AutoReload ;
extern int              EditorCount ;
extern int              PrintPointSize ;
extern int              PrintUseBorder ;
extern int              PrintTopMargin ;
extern int              PrintLeftMargin ;
extern int              PrintBottomMargin ;
extern int              PrintRightMargin ;
extern bool             MessagePaneVisible ;
extern bool             CreateBackups ;
extern bool             LastOverwrite ;
extern bool             UndoAfterSave ;
extern bool             PrintUseColor ;
extern bool             PrintUseFancyText ;
extern bool             PrintUsePageNumbers ;
extern bool             PrintUseDateTime ;
extern bool             PrintUseFileName ;
extern HWND             hMainWindow ;
extern HWND             hStatusWindow ;
extern CCodeMax         *Editors [MAX_EDITORS] ;
extern CCodeMax         *Editor ;
extern CodemaxColors    CJCColours ;
extern CodemaxColors    DefaultColours ;
extern HINSTANCE        hInstance ;
extern CStdString       InsertPath ;
extern EditConfigStruct EditConfig ;

typedef struct
{
  unsigned short        max ;
  unsigned short        min ;
  unsigned int          initial ;
  LPCSTR                caption ;
} EnterValueParams ;

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
  result = GetTextExtentPoint32 (hDC, text, (int) strlen (text), &extent) ;
  SelectObject (hDC, hOldFont) ;
  ReleaseDC (hWnd, hDC) ;
  if (!result)
    return (0) ;
  return (&extent) ;
}

int GetTextWidth (HWND hWnd, LPCSTR text)
{
  return (GetTextExtent (hWnd, text)->cx) ;
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

void CenterWindowRelative (HWND hRelativeTo, HWND hTarget, bool bRepaint)
{
  int         difference ;
  int         width ;
  int         height ;
  int         x ;
  int         y ;
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
  MoveWindow (hTarget, x, y, width, height, bRepaint) ;
}

INT_PTR CALLBACK FileChangedDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   width ;
  int                   diff ;
  char                  str [_MAX_FNAME + 64] ;
  RECT                  rect ;
  HWND                  hText ;

  switch (message)
  {
    case WM_INITDIALOG :
         CCodeMax::SaveDialogActive = true ;
         sprintf (str, "File '%s' has been modified", (LPCSTR) lParam) ;
         SetDlgItemText (hDlg, IDC_MESSAGE, str) ;
         hText = GetDlgItem (hDlg, IDC_MESSAGE) ;
         width = GetTextWidth (hText, str) ;
         if (width == 0)
           return (false) ;
         GetClientRect (hText, &rect) ;
         if (rect.right >= width + 10)
           return (false) ;
         diff = width - rect.right + 10 ;
         SetWindowPos (hText, NULL, 0, 0, rect.right + diff, rect.bottom, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER) ;
         GetWindowRect (hDlg, &rect) ;
         rect.right -= rect.left ;
         rect.bottom -= rect.top ;
         rect.right += diff ;
         SetWindowPos (hDlg, NULL, 0, 0, rect.right + 10, rect.bottom, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER) ;
         diff /= 2 ;
         NudgeChildWindow (hDlg, IDSAVE, diff) ;
         if (GetDlgItem (hDlg, IDSAVEALL) != NULL)
           NudgeChildWindow (hDlg, IDSAVEALL, diff) ;
         NudgeChildWindow (hDlg, IDDISCARD, diff) ;
         NudgeChildWindow (hDlg, IDCANCEL, diff) ;
         return (false) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDSAVE :
                EndDialog (hDlg, IDSAVE) ;
                CCodeMax::SaveDialogActive = false ;
                return (true) ;

           case IDSAVEALL :
                EndDialog (hDlg, IDSAVEALL) ;
                CCodeMax::SaveDialogActive = false ;
                return (true) ;

           case IDDISCARD :
                EndDialog (hDlg, IDDISCARD) ;
                CCodeMax::SaveDialogActive = false ;
                return (true) ;

           case IDCANCEL :
                EndDialog (hDlg, IDCANCEL) ;
                CCodeMax::SaveDialogActive = false ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

int ShowFileChangedDialog (char *FileName, bool HasSaveAll)
{
  int                   idDialog ;

  idDialog = HasSaveAll ? IDD_SAVEALLMODIFIED : IDD_SAVEMODIFIED ;
  return (DialogBoxParam (hInstance,
                          MAKEINTRESOURCE (idDialog),
                          hMainWindow,
                          (DLGPROC) FileChangedDialogProc,
                          (LPARAM) FileName)) ;
}

INT_PTR CALLBACK ReloadFileDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   width, width1, width2, width3 ;
  int                   diff ;
  char                  str [_MAX_FNAME + 64] ;
  RECT                  rect ;
  HWND                  hText ;
  SIZE                  extent ;

  switch (message)
  {
    case WM_INITDIALOG :
         CCodeMax::SaveDialogActive = true ;
         hText = GetDlgItem (hDlg, IDC_MESSAGE) ;
         sprintf (str, "File '%s' has changed on disk.", (LPCSTR) lParam) ;
         extent = *GetTextExtent (hText, str) ;
         width1 = extent.cx ;
         width2 = GetTextWidth (hText, "This file was modified outside POVWIN. Should it be reloaded ?") ;
         width3 = GetTextWidth (hText, "(See the help file for more information about this feature and how to control it.)") ;
         width = max (width3, max (width1, width2)) ;

         GetClientRect (hText, &rect) ;
         if (rect.bottom >= extent.cy * 4)
           strcat (str, "\n") ;

         strcat (str, "\nThis file was modified outside POVWIN. Should it be reloaded ?\n(See the help file for more information about this feature and how to control it.)") ;
         SetDlgItemText (hDlg, IDC_MESSAGE, str) ;

         if (width == 0)
           return (false) ;
         if (rect.right >= width + 10)
           return (false) ;
         diff = width - rect.right + 10 ;
         SetWindowPos (hText, NULL, 0, 0, rect.right + diff, rect.bottom, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER) ;
         GetWindowRect (hDlg, &rect) ;
         rect.right -= rect.left ;
         rect.bottom -= rect.top ;
         rect.right += diff ;
         SetWindowPos (hDlg, NULL, 0, 0, rect.right + 10, rect.bottom, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER) ;
         diff /= 2 ;
         NudgeChildWindow (hDlg, IDRELOAD, diff) ;
         NudgeChildWindow (hDlg, IDDISCARD, diff) ;
         NudgeChildWindow (hDlg, IDCANCEL, diff) ;
         NudgeChildWindow (hDlg, IDHELP, diff) ;
         return (false) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDRELOAD :
                EndDialog (hDlg, IDRELOAD) ;
                CCodeMax::SaveDialogActive = false ;
                return (true) ;

           case IDDISCARD :
                EndDialog (hDlg, IDDISCARD) ;
                CCodeMax::SaveDialogActive = false ;
                return (true) ;

           case IDCANCEL :
                EndDialog (hDlg, IDCANCEL) ;
                CCodeMax::SaveDialogActive = false ;
                return (true) ;

           case IDHELP :
                SendMessage (hMainWindow, KEYWORD_LOOKUP_MESSAGE, 0, (LPARAM) "auto-reload") ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

int ShowReloadDialog (char *FileName)
{
  return (DialogBoxParam (hInstance,
                          MAKEINTRESOURCE (IDD_RELOADFILE),
                          hMainWindow,
                          (DLGPROC) ReloadFileDialogProc,
                          (LPARAM) FileName)) ;
}

INT_PTR CALLBACK SaveBeforeRenderDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   width ;
  int                   diff ;
  int                   retval ;
  int                   checkWidth ;
  char                  str [_MAX_FNAME + 64] ;
  RECT                  rect ;
  HWND                  hText ;
  HWND                  hCheck ;

  switch (message)
  {
    case WM_INITDIALOG :
         hCheck = GetDlgItem (hDlg, IDC_CHECKBOX) ;
         GetWindowText (hCheck, str, sizeof (str) - 1) ;
         width = GetTextWidth (hCheck, str) ;
         if (width == 0)
         {
           GetWindowRect (hDlg, &rect) ;
           width = rect.right ;
         }
         GetClientRect (hCheck, &rect) ;
         checkWidth = rect.right + width ;
         SetWindowPos (hCheck, NULL, 0, 0, checkWidth, rect.bottom, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER) ;
         GetClientRect (hDlg, &rect) ;
         NudgeChildWindow (hDlg, IDC_CHECKBOX, (rect.right - checkWidth) / 2) ;

         sprintf (str, "File '%s' has been changed - save ?", (LPCSTR) lParam) ;
         SetDlgItemText (hDlg, IDC_MESSAGE, str) ;
         hText = GetDlgItem (hDlg, IDC_MESSAGE) ;
         width = GetTextWidth (hText, str) ;
         if (width == 0)
           return (false) ;
         GetClientRect (hText, &rect) ;
         if (rect.right >= width + 10)
           return (false) ;
         diff = width - rect.right + 10 ;
         SetWindowPos (hText, NULL, 0, 0, rect.right + diff, rect.bottom, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER) ;
         GetWindowRect (hDlg, &rect) ;
         rect.right -= rect.left ;
         rect.bottom -= rect.top ;
         rect.right += diff ;
         SetWindowPos (hDlg, NULL, 0, 0, rect.right + 10, rect.bottom, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER) ;
         diff /= 2 ;
         NudgeChildWindow (hDlg, IDSAVE, diff) ;
         NudgeChildWindow (hDlg, IDSAVEALL, diff) ;
         NudgeChildWindow (hDlg, IDNO, diff) ;
         NudgeChildWindow (hDlg, IDCANCEL, diff) ;
         NudgeChildWindow (hDlg, IDC_CHECKBOX, diff) ;

         return (false) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDSAVE :
           case IDSAVEALL :
           case IDNO :
           case IDCANCEL :
                retval = LOWORD (wParam) ;
                if (IsDlgButtonChecked (hDlg, IDC_CHECKBOX))
                  retval |= DONTASKAGAINFLAG ;
                EndDialog (hDlg, retval) ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

int ShowSaveBeforeRenderDialog (char *FileName)
{
  return (DialogBoxParam (hInstance,
                          MAKEINTRESOURCE (IDD_SAVEBEFORERENDER),
                          hMainWindow,
                          (DLGPROC) SaveBeforeRenderDialogProc,
                          (LPARAM) FileName)) ;
}

INT_PTR CALLBACK EnterValueDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   retval ;
  int                   rc ;
  HWND                  hSpin ;
  EnterValueParams      *p ;

  switch (message)
  {
    case WM_INITDIALOG :
         p = (EnterValueParams *) lParam ;
         hSpin = GetDlgItem (hDlg, IDC_SPIN) ;
         SendMessage (hSpin, UDM_SETRANGE, 0, (LPARAM) MAKELONG (p->max, p->min)) ;
         SendMessage (hSpin, UDM_SETPOS, 0, (LPARAM) p->initial) ;
         SetWindowText (hDlg, p->caption) ;
         return (false) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
                retval = GetDlgItemInt (hDlg, IDC_EDIT, &rc, FALSE) ;
                if (!rc)
                {
                  MessageBox (hDlg, "Invalid input value", "Invalid value", MB_OK | MB_ICONEXCLAMATION) ;
                  SetFocus (GetDlgItem (hDlg, IDC_EDIT)) ;
                }
                else
                  EndDialog (hDlg, retval) ;
                return (true) ;

           case IDCANCEL :
                EndDialog (hDlg, -1) ;
                return (true) ;
         }
         break ;
  }
  return (false) ;
}

int ShowEnterValueDialog (char *caption, unsigned short min, unsigned short max, unsigned int initial)
{
  EnterValueParams      params ;

  params.max = max ;
  params.min = min ;
  params.initial = initial ;
  params.caption = caption ;
  return (DialogBoxParam (hInstance,
                          MAKEINTRESOURCE (IDD_ENTERVALUE),
                          hMainWindow,
                          (DLGPROC) EnterValueDialogProc,
                          (LPARAM) &params)) ;
}

UINT_PTR CALLBACK PageSetupHook (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int         rc ;
  int         pointsize ;

  switch (message)
  {
    case WM_INITDIALOG :
         SendDlgItemMessage (hDlg, IDC_SPIN, UDM_SETRANGE, 0, (LPARAM) MAKELONG (72, 4)) ;
         SendDlgItemMessage (hDlg, IDC_SPIN, UDM_SETPOS, 0, (LPARAM) PrintPointSize) ;
         SendDlgItemMessage (hDlg, IDC_SPIN2, UDM_SETRANGE, 0, (LPARAM) MAKELONG (999, 0)) ;
         SendDlgItemMessage (hDlg, IDC_SPIN3, UDM_SETRANGE, 0, (LPARAM) MAKELONG (999, 0)) ;
         SendDlgItemMessage (hDlg, IDC_SPIN4, UDM_SETRANGE, 0, (LPARAM) MAKELONG (999, 0)) ;
         SendDlgItemMessage (hDlg, IDC_SPIN5, UDM_SETRANGE, 0, (LPARAM) MAKELONG (999, 0)) ;
         CheckDlgButton (hDlg, IDC_USECOLOR, PrintUseColor ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_USEFANCYTEXT, PrintUseFancyText ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_PAGENUMBERS, PrintUsePageNumbers ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_DATETIME, PrintUseDateTime ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_FILENAME, PrintUseFileName ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_BORDERFIRST + PrintUseBorder, BST_CHECKED) ;
         return (0) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
                pointsize = GetDlgItemInt (hDlg, IDC_POINTSIZE, &rc, FALSE) ;
                if (!rc || pointsize < 4 || pointsize > 72)
                {
                  MessageBox (hDlg, "Valid range is 4 to 72", "Invalid point size", MB_OK | MB_ICONEXCLAMATION) ;
                  SetFocus (GetDlgItem (hDlg, IDC_POINTSIZE)) ;
                  return (1) ;
                }
                PrintPointSize = pointsize ;
                PrintUseColor = IsDlgButtonChecked (hDlg, IDC_USECOLOR) == BST_CHECKED ;
                PrintUseFancyText = IsDlgButtonChecked (hDlg, IDC_USEFANCYTEXT) == BST_CHECKED ;
                PrintUsePageNumbers = IsDlgButtonChecked (hDlg, IDC_PAGENUMBERS) == BST_CHECKED ;
                PrintUseDateTime = IsDlgButtonChecked (hDlg, IDC_DATETIME) == BST_CHECKED ;
                PrintUseFileName = IsDlgButtonChecked (hDlg, IDC_FILENAME) == BST_CHECKED ;
                if (IsDlgButtonChecked (hDlg, IDC_BORDERNONE) == BST_CHECKED)
                  PrintUseBorder = 0 ;
                else if (IsDlgButtonChecked (hDlg, IDC_BORDERTHIN) == BST_CHECKED)
                  PrintUseBorder = 1 ;
                else if (IsDlgButtonChecked (hDlg, IDC_BORDERTHICK) == BST_CHECKED)
                  PrintUseBorder = 2 ;
                return (0) ;

           case IDCANCEL :
                return (0) ;
         }
         break ;
  }
  return (0) ;
}

UINT_PTR CALLBACK PrintHook (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int         rc ;
  int         pointsize ;

  switch (message)
  {
    case WM_INITDIALOG :
         SendDlgItemMessage (hDlg, IDC_SPIN, UDM_SETRANGE, 0, (LPARAM) MAKELONG (72, 4)) ;
         SendDlgItemMessage (hDlg, IDC_SPIN, UDM_SETPOS, 0, (LPARAM) PrintPointSize) ;
         CheckDlgButton (hDlg, IDC_USECOLOR, PrintUseColor ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_USEFANCYTEXT, PrintUseFancyText ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_PAGENUMBERS, PrintUsePageNumbers ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_DATETIME, PrintUseDateTime ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_FILENAME, PrintUseFileName ? BST_CHECKED : BST_UNCHECKED) ;
         CheckDlgButton (hDlg, IDC_BORDERFIRST + PrintUseBorder, BST_CHECKED) ;
         return (0) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
                pointsize = GetDlgItemInt (hDlg, IDC_POINTSIZE, &rc, FALSE) ;
                if (!rc || pointsize < 4 || pointsize > 72)
                {
                  MessageBox (hDlg, "Valid range is 4 to 72", "Invalid point size", MB_OK | MB_ICONEXCLAMATION) ;
                  SetFocus (GetDlgItem (hDlg, IDC_POINTSIZE)) ;
                  return (1) ;
                }
                PrintPointSize = pointsize ;
                PrintUseColor = IsDlgButtonChecked (hDlg, IDC_USECOLOR) == BST_CHECKED ;
                PrintUseFancyText = IsDlgButtonChecked (hDlg, IDC_USEFANCYTEXT) == BST_CHECKED ;
                PrintUsePageNumbers = IsDlgButtonChecked (hDlg, IDC_PAGENUMBERS) == BST_CHECKED ;
                PrintUseDateTime = IsDlgButtonChecked (hDlg, IDC_DATETIME) == BST_CHECKED ;
                PrintUseFileName = IsDlgButtonChecked (hDlg, IDC_FILENAME) == BST_CHECKED ;
                if (IsDlgButtonChecked (hDlg, IDC_BORDERNONE) == BST_CHECKED)
                  PrintUseBorder = 0 ;
                else if (IsDlgButtonChecked (hDlg, IDC_BORDERTHIN) == BST_CHECKED)
                  PrintUseBorder = 1 ;
                else if (IsDlgButtonChecked (hDlg, IDC_BORDERTHICK) == BST_CHECKED)
                  PrintUseBorder = 2 ;
                return (0) ;

           case IDCANCEL :
                return (0) ;
         }
         break ;
  }
  return (0) ;
}

