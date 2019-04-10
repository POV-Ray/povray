//******************************************************************************
///
/// @file windows/pvfiles.cpp
///
/// This module contains ASCII file related code.
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

#include <setjmp.h>
#include <string.h>

#include <windows.h>
#include <io.h>

#include "pvengine.h"
#include "resource.h"
#include "pvlegal.h"
#include "pvdemo.h"

// this must be the last file included
#include "syspovdebug.h"

namespace povwin
{

extern int              message_ychar ;
extern char             ourPath [] ;

void fill_listbox (HWND hwnd)
{
  char        *p = povlegal_text ;
  char        str [1024] = "" ;
  char        *s = str ;

  while (*p)
  {
    if (*p == '\n')
    {
      *s = '\0' ;
      SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM) str) ;
      s = str ;
      p++ ;
      continue ;
    }
    *s++ = *p++ ;
  }
  *s = '\0' ;
  SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM) str) ;
}

void save_povlegal (void)
{
  char        filename [_MAX_PATH + 64] ;
  FILE        *outF ;

  sprintf (filename, "%sagpl-3.0.txt", DocumentsPath) ;
  if ((outF = fopen (filename, "wt")) == NULL)
  {
    PovMessageBox ("Cannot create agpl-3.0.txt", "Cannot save document") ;
    return ;
  }
  if (fwrite (povlegal_text, 1, sizeof (povlegal_text) - 1, outF) != sizeof (povlegal_text) - 1)
  {
    PovMessageBox ("Cannot write to agpl-3.0.txt", "Cannot save document") ;
    fclose (outF) ;
    return ;
  }
  fclose (outF) ;
  sprintf (filename, "The POV-Ray license was written to the file %sagpl-3.0.txt", DocumentsPath) ;
  PovMessageBox (filename, "agpl-3.0.txt saved") ;
}

INT_PTR CALLBACK PovLegalDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  DRAWITEMSTRUCT        *d ;
  MEASUREITEMSTRUCT     *m ;
  static HBRUSH         hbr ;

  switch (message)
  {
    case WM_INITDIALOG :
         resize_listbox_dialog (hDlg, IDC_LISTBOX, 79) ;
         CenterWindowRelative ((HWND) lParam, hDlg, true, true) ;
         SetWindowText (hDlg, "POV-Ray License") ;
//       hbr = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
         fill_listbox (GetDlgItem (hDlg, IDC_LISTBOX)) ;
         return (true) ;

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
           case IDOK :
//              DeleteObject (hbr) ;
                EndDialog (hDlg, true) ;
                return (true) ;

           default :
                return (true) ;
         }

    case WM_MEASUREITEM :
         if (wParam == IDC_LISTBOX)
         {
           m = (MEASUREITEMSTRUCT *) lParam ;
           m->itemHeight = message_ychar ;
           return (true) ;
         }
         else
           return (false) ;

    case WM_DRAWITEM :
         if (wParam == IDC_LISTBOX)
         {
           d = (DRAWITEMSTRUCT *) lParam ;
           d->itemState &= ~ODS_SELECTED ;
           draw_ordinary_listbox (d, false) ;
           return (true) ;
         }
         else
           return (false) ;
  }
  return (false) ;
}

char *save_demo_file (char *s1, char *s2)
{
  char        **p = povdemo_scene ;
  FILE        *outF ;

  GetTempPath (_MAX_PATH - 16, s1) ;
  appendPathSeparator (s1) ;
  strcpy (s2, s1) ;
  strcat (s1, "POVDEMO.$$1") ;
  strcat (s2, "POVDEMO.$$2") ;
  if ((outF = fopen (s1, "wt")) == NULL)
  {
    PovMessageBox ("Cannot create temporary file", "Cannot run demo") ;
    return (NULL) ;
  }
  while (*p)
  {
    if (fprintf (outF, "%s\n", *p++) == EOF)
    {
      PovMessageBox ("Cannot write to temporary file", "Cannot run demo") ;
      fclose (outF) ;
      _unlink (s1) ;
      return (NULL) ;
    }
  }
  fclose (outF) ;
  p = povdemo_ini ;
  if ((outF = fopen (s2, "wt")) == NULL)
  {
    PovMessageBox ("Cannot create temporary file", "Cannot run demo") ;
    _unlink (s1) ;
    return (NULL) ;
  }
  while (*p)
  {
    if (fprintf (outF, "%s\n", *p++) == EOF)
    {
      PovMessageBox ("Cannot write to temporary file", "Cannot run demo") ;
      fclose (outF) ;
      _unlink (s1) ;
      _unlink (s2) ;
      return (NULL) ;
    }
  }
  fclose (outF) ;
  return (s1) ;
}

}
// end of namespace povwin
