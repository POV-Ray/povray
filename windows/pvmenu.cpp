//******************************************************************************
///
/// @file windows/pvmenu.cpp
///
/// This module implements menu-related routines for the Windows build of POV.
///
/// @author Christopher J. Cason.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

// REMOVAL OF NON-EXPERT MENUS (August 1998) ... the 'non-expert menus' option
// was removed. This leaves some of this code redundant, but here it is anyhow ...

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#include <windows.h>
#include <commctrl.h>
#include <setjmp.h>
#include <string.h>

#include "pvengine.h"
#include "resource.h"
#include "pvedit.h"
#include "pvdisplay.h"

#ifdef RTR_SUPPORT
  #include "rtrsupport.h"
#endif

// this must be the last file included
#include "syspovdebug.h"

namespace povwin
{

bool                    MenuBarDraw = false ;
char                    WindowList[MAX_EDITORS + 1][_MAX_PATH];
HMENU                   hMainMenu ;
HMENU                   hMenuBar ;
HMENU                   hPopupMenus ;
HMENU                   hFileMenu ;
HMENU                   hEditMenu ;
HMENU                   hRenderMenu ;
HMENU                   hOptionsMenu ;
HMENU                   hToolsMenu ;
HMENU                   hPluginsMenu ;
HMENU                   hHelpMenu ;
HMENU                   hOldRenderwinMenu ;
HMENU                   hNewRenderwinMenu ;
HMENU                   hVidcapMenu ;
HACCEL                  hAccelerators ;

extern HWND             main_window ;
extern HWND             toolbar_window ;
extern bool             RenderwinIsChild ;
extern bool             preserve_bitmap ;
extern bool             rendering ;
extern bool             render_auto_close ;
extern bool             IsVista ;
extern unsigned         renderwin_8bits ;

bool PVEnableMenuItem (UINT idItem, UINT state)
{
  EnableMenuItem (hPopupMenus, idItem, state) ;
  EnableMenuItem (hMenuBar, idItem, state) ;
  SendMessage (toolbar_window, TB_ENABLEBUTTON, idItem, MAKELONG (state == MF_ENABLED, 0)) ;
  if (idItem == CM_RENDERCLOSE)
  {
    SendMessage (toolbar_window, TB_HIDEBUTTON, CM_RENDERSHOW, MAKELONG (state == MF_ENABLED, 0)) ;
    SendMessage (toolbar_window, TB_HIDEBUTTON, CM_RENDERCLOSE, MAKELONG (state != MF_ENABLED, 0)) ;
  }
  return (true) ;
}

bool PVCheckMenuItem (UINT idItem, UINT state)
{
  CheckMenuItem (hPopupMenus, idItem, state) ;
  CheckMenuItem (hMenuBar, idItem, state) ;
  return (true) ;
}

bool PVCheckMenuRadioItem (UINT idFirst, UINT idLast, UINT idItem)
{
  CheckMenuRadioItem (hPopupMenus, idFirst, idLast, idItem, MF_BYCOMMAND) ;
  CheckMenuRadioItem (hMenuBar, idFirst, idLast, idItem, MF_BYCOMMAND) ;
  return (true) ;
}

bool PVModifyMenu (UINT idItem, UINT flags, UINT idNewItem, LPCSTR lpNewItem)
{
  ModifyMenu (hPopupMenus, idItem, flags, idNewItem, lpNewItem) ;
  ModifyMenu (hMenuBar, idItem, flags, idNewItem, lpNewItem) ;
  MenuBarDraw = true ;
  return (true) ;
}

bool PVDeleteMenuItem (UINT idItem)
{
  DeleteMenu (hPopupMenus, idItem, MF_BYCOMMAND) ;
  DeleteMenu (hMenuBar, idItem, MF_BYCOMMAND) ;
  return (true) ;
}

int find_menuitem (HMENU hMenu, LPCSTR title)
{
  int         max = GetMenuItemCount (hMenu) ;
  char        str [256] ;

  if (title[0] == '\0')
    return (-1);
  for (int i = 0 ; i < 64 ; i++)
    if (GetMenuString (hMenu, i, str, sizeof (str) - 1, MF_BYPOSITION) > 0)
      if (strcmp (title, str) == 0)
        return (i) ;
  return (-1) ;
}

void init_menus (void)
{
  hPopupMenus = LoadMenu (hInstance, MAKEINTRESOURCE (POPUP_MENUS32)) ;
  hAccelerators = LoadAccelerators (hInstance, MAKEINTRESOURCE (PVENGINE_MENU)) ;
  hMenuBar = LoadMenu (hInstance, MAKEINTRESOURCE (PVENGINE_MENU32)) ;
  hFileMenu = GetSubMenu (hMenuBar, 0) ;
  hEditMenu = GetSubMenu (hMenuBar, 1) ;
  hRenderMenu = GetSubMenu (hMenuBar, 2) ;
  hOptionsMenu = GetSubMenu (hMenuBar, 3) ;
  hToolsMenu = GetSubMenu (hMenuBar, 4) ;
  hHelpMenu = GetSubMenu (hMenuBar, 5) ;
  hOldRenderwinMenu = GetSubMenu (hPopupMenus, 2) ;
  hNewRenderwinMenu = GetSubMenu (hPopupMenus, 3) ;

  int n = find_menuitem(hOptionsMenu, "&Other Settings") ;
  assert(n != -1) ;
  hVidcapMenu = GetSubMenu(GetSubMenu(hOptionsMenu, n), 0) ;

  n = find_menuitem(hOptionsMenu, "GU&I-Extensions") ;
  assert(n != -1) ;
  hPluginsMenu = GetSubMenu(hOptionsMenu, n);

  if (!IsVista)
  {
    RemoveMenu (hRenderMenu, CM_RENDERPRIORITY_BACKGROUND, MF_BYCOMMAND);
    RemoveMenu (hPopupMenus, CM_RENDERPRIORITY_BACKGROUND, MF_BYCOMMAND);
  }

#if POV_RAY_HAS_OFFICIAL_FEATURES != 1
  n = find_menuitem(hOptionsMenu, "&Update Checks") ;
  assert(n != -1) ;
  RemoveMenu (hOptionsMenu, n, MF_BYPOSITION);
  RemoveMenu (hHelpMenu, CM_CHECKUPDATENOW, MF_BYCOMMAND);
#endif
}

void set_newuser_menu (HMENU hMenu, UINT ID, bool hide, bool separator)
{
  if (GetMenuItemID (hMenu, 0) != ID)
  {
    if (hide)
      return ;
    if (separator)
      InsertMenu (hMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, NULL) ;
    InsertMenu (hMenu, 0, MF_BYPOSITION, ID, "&Help On This Menu") ;
  }
  else
  {
    if (!hide)
      return ;
    DeleteMenu (hMenu, 0, MF_BYPOSITION) ;
    if (separator)
      DeleteMenu (hMenu, 0, MF_BYPOSITION) ;
  }
}

void set_newuser_menus (bool hide)
{
  set_newuser_menu (hFileMenu, CM_FILEMENUHELP, hide, true) ;
  set_newuser_menu (hEditMenu, CM_EDITMENUHELP, hide, true) ;
  set_newuser_menu (hRenderMenu, CM_RENDERMENUHELP, hide, true) ;
  set_newuser_menu (hOptionsMenu, CM_OPTIONSMENUHELP, hide, true) ;
  set_newuser_menu (hToolsMenu, CM_TOOLSMENUHELP, hide, false) ;
  set_newuser_menu (hToolsMenu, CM_WINDOWMENUHELP, hide, true) ;
  set_newuser_menu (GetSubMenu (hPopupMenus, 0), CM_MESSAGEWINMENUHELP, hide, true) ;
  set_newuser_menu (GetSubMenu (hPopupMenus, 1), CM_RENDERWINMENUHELP, hide, true) ;
  EditPassOnMessage (NULL, HIDE_NEWUSER_HELP_MESSAGE, hide, 0, NULL) ;
}

void clear_menu (HMENU hMenu)
{
  while (RemoveMenu (hMenu, 0, MF_BYPOSITION)) ;
}

void setup_menus (bool have_editor)
{
  if (have_editor)
    AppendMenu (hOptionsMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetOptionsMenu), "&Editor Window") ;

#ifdef RTR_SUPPORT
  std::vector<std::string> vidSources;
  size_t numSources = GetVideoSourceNames(vidSources);
  if (numSources > 0)
  {
    DeleteMenu (hVidcapMenu, 0, MF_BYPOSITION) ;
    if (numSources > CM_LASTVIDEOSOURCE - CM_FIRSTVIDEOSOURCE + 1)
      numSources = CM_LASTVIDEOSOURCE - CM_FIRSTVIDEOSOURCE + 1 ;
    for (int i = 0; i < numSources; i++)
      InsertMenu (hVidcapMenu, -1, MF_BYPOSITION, CM_FIRSTVIDEOSOURCE + i, vidSources[i].c_str()) ;
    CheckMenuRadioItem(hVidcapMenu, CM_FIRSTVIDEOSOURCE, CM_LASTVIDEOSOURCE, CM_FIRSTVIDEOSOURCE, MF_BYCOMMAND);
    int n = find_menuitem (hVidcapMenu, GetVideoSourceName().c_str());
    if (n != -1)
      CheckMenuRadioItem(hVidcapMenu, CM_FIRSTVIDEOSOURCE, CM_LASTVIDEOSOURCE, CM_FIRSTVIDEOSOURCE + n, MF_BYCOMMAND);
    else
      SetVideoSourceName(vidSources[0].c_str());
  }
#endif

  DrawMenuBar (main_window) ;
}

// build the menu displayed when the message window is selected
void build_main_menu (HMENU hMenu, bool have_editor)
{
  clear_menu (hMenu) ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) (have_editor ? EditGetMenu (GetFileMenu) : hFileMenu), "&File") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hEditMenu, "&Edit") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hRenderMenu, "&Render") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hOptionsMenu, "&Options") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hToolsMenu, "&Tools") ;
  if (EditGetMenu (GetWindowMenu) != NULL)
    AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetWindowMenu), "&Window") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hHelpMenu, "&Help") ;
  DrawMenuBar (main_window) ;
}

// build the menu displayed when an editor window is selected
void build_editor_menu (HMENU hMenu)
{
  clear_menu (hMenu) ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetFileMenu), "&File") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetEditMenu), "&Edit") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetSearchMenu), "Se&arch") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetTextMenu), "&Text") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetOptionsMenu), "E&ditor") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetInsertMenu), "&Insert") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hRenderMenu, "&Render") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hOptionsMenu, "&Options") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hToolsMenu, "Too&ls") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) EditGetMenu (GetWindowMenu), "&Window") ;
  AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hHelpMenu, "&Help") ;
  DrawMenuBar (main_window) ;
}

}
