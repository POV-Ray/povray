//******************************************************************************
///
/// @file windows/cmedit/editorinterface.cpp
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
#include "menusupport.h"
#include "eventhandlers.h"
#include "editorinterface.h"
#include "dialogs.h"
#include "..\pvedit.h"
#include "povlangdef.h"

#include <windowsx.h>
#include <gdiplus.h>
#include "sys/stat.h"

using namespace povwin;
using namespace Gdiplus;

typedef struct
{
  HMENU       hChild ;
  HMENU       hParent ;
  DWORD       index ;
} MenuParentInfo ;

LRESULT CALLBACK TabWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK DragPaneWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK HintWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) ;

int                     EditorCount ;
int                     TabIndex ;
int                     NotifyBase ;
int                     KeywordCount ;
int                     ExpansionAnchorCol ;
int                     ExpansionAnchorLine ;
int                     ExpansionIndex ;
int                     ExpansionLastCol ;
int                     AutoSaveDelay ;
int                     AutoSaveCountdown ;
int                     EditDragOffset ;
int                     EditStartDragOffset ;
int                     EditStartDragY ;
bool                    MessagePaneVisible ;
bool                    IgnoreNext ;
bool                    ExpansionHasCompleteMatch ;
bool                    AutoRenderSave ;
bool                    EditPaneDragActive ;
bool                    HadSaveModified;
bool                    WarnSaveModified = true;
char                    *Keywords [MAX_KEYWORDS] ;
char                    MessageWinTitle[] = "Messages";
HWND                    hMessageWindow ;
HWND                    hNotifyWindow ;
HWND                    hMainWindow ;
HWND                    hStatusWindow ;
HWND                    hTabWindow ;
HWND                    hTabWindowTooltip;
HWND                    hDividerWindow ;
HWND                    hCurrentHintWindow ;
HWND                    hCodeListWindow ;
HMENU                   hMainMenu ;
HMENU                   hPopupMenu ;
HMENU                   hFileMenu ;
HMENU                   hEditMenu ;
HMENU                   hSearchMenu ;
HMENU                   hTextMenu ;
HMENU                   hEditorMenu ;
HMENU                   hInsertMenu ;
HMENU                   hOlderFilesMenu ;
HMENU                   hWindowMenu ;
HMENU                   hTabMenu;
DWORD                   InsertThreadID ;
HANDLE                  hInsertThread ;
Bitmap                  *pCurrentHintBitmap;
CCodeMax                *Editors [MAX_EDITORS] ;
CCodeMax                *Editor ;
const char              *WindowList[MAX_EDITORS + 1];
CStdString              BinariesPath ;
CStdString              DocumentsPath;
CStdString              InitialDir ;
CStdString              ExpansionWord ;
CStdString              InsertPath ;
CStdString              POVRayIniPath ;
CStdString              IncludeFilename ;
CStdString              CommandLine ;
CStdStringList          RecentFiles ;
CStdStringList          OlderFiles ;
CStdStringList          InsertMenuItems ;
OSVERSIONINFO           VersionInfo ;

WNDPROC                 OldTabWndProc ;

extern int              AutoReload ;
extern bool             CreateBackups ;
extern bool             LastOverwrite ;
extern bool             UndoAfterSave ;
extern HINSTANCE        hInstance ;
extern EditConfigStruct EditConfig ;

enum
{
  cmOpenContainingFolder = 1,
  cmCopyFilenameToClipboard,
  cmSaveThisFile,
  cmCloseThisFile,
  cmCloseOtherFiles,
  cmShiftTabLeft,
  cmShiftTabRight
};

//------------------------------------------------------------------------------------------------------------------------

void debug (char *format, ...)
{
#ifdef DEVELOPMENT
  char                  str [2048] ;
  va_list               arg_ptr ;

  va_start (arg_ptr, format) ;
  vsprintf (str, format, arg_ptr) ;
  va_end (arg_ptr) ;

  OutputDebugString (str) ;
#endif
}

//------------------------------------------------------------------------------------------------------------------------

bool HaveWin98OrLater (void)
{
  if (VersionInfo.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
    return (false) ;
  if (VersionInfo.dwMajorVersion < 4)
    return (false) ;
  if (VersionInfo.dwMajorVersion > 4)
    return (true) ;
  return (VersionInfo.dwMinorVersion > 0) ;
}

bool HaveWin2kOrLater (void)
{
  return (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && VersionInfo.dwMajorVersion >= 5) ;
}

bool HaveWinXPOrLater (void)
{
  if (VersionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT || VersionInfo.dwMajorVersion < 5)
    return (false) ;
  return (VersionInfo.dwMajorVersion > 5 || (VersionInfo.dwMajorVersion == 5 && VersionInfo.dwMinorVersion > 0)) ;
}

//------------------------------------------------------------------------------------------------------------------------

bool CanFocus (HWND hWnd)
{
  while (hWnd)
  {
    if (!IsWindowVisible (hWnd) || !IsWindowEnabled (hWnd))
      return (false) ;
    hWnd = GetParent (hWnd) ;
  }
  return (true) ;
}

//------------------------------------------------------------------------------------------------------------------------

void ShowMessage (const char *str, int type)
{
  MessageBox (hMainWindow, str, "POV-Ray for Windows", MB_OK | type) ;
}

//------------------------------------------------------------------------------------------------------------------------

void PutStatusMessage (const char *Message)
{
  SendMessage (hStatusWindow, SB_SETTEXT, StatusMessage, (LPARAM) Message) ;
}

//------------------------------------------------------------------------------------------------------------------------

void SetStatusLine (void)
{
  char                  str [256] ;
  CodemaxRange          range ;

  if (Editor != NULL)
  {
    SendMessage (hStatusWindow, SB_SETTEXT, StatusIns, (LPARAM) (Editor->IsOvertypeMode () ? "\tOvr" : "\tIns")) ;
    SendMessage (hStatusWindow, SB_SETTEXT, StatusModified, (LPARAM) (Editor->IsModified () ? "\tMod" : "")) ;
    Editor->GetSel (&range, false) ;
    sprintf (str, "\tL: %d", range.End.Line + 1) ;
    SendMessage (hStatusWindow, SB_SETTEXT, StatusLine, (LPARAM) str) ;
    sprintf (str, "\tC: %d", range.End.Col + 1) ;
    SendMessage (hStatusWindow, SB_SETTEXT, StatusCol, (LPARAM) (LPCSTR) str) ;
  }
  else
  {
    SendMessage (hStatusWindow, SB_SETTEXT, StatusIns, (LPARAM) "") ;
    SendMessage (hStatusWindow, SB_SETTEXT, StatusModified, (LPARAM) "") ;
    SendMessage (hStatusWindow, SB_SETTEXT, StatusLine, (LPARAM) "") ;
    SendMessage (hStatusWindow, SB_SETTEXT, StatusCol, (LPARAM) "") ;
  }
}

//------------------------------------------------------------------------------------------------------------------------

void ShowErrorMessage (CStdString Title, const char *Msg, int ErrorCode)
{
  char                  *buffer = NULL;
  CStdString            str (Msg) ;

  if (ErrorCode == 0)
    ErrorCode = GetLastError () ;
  PutStatusMessage (Msg) ;
  Title = Title == "" ? CStdString ("POV-Ray Editor") : CStdString ("File '") + Title + "'" ;
  if (ErrorCode != 0)
  {
    FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
           NULL,
           ErrorCode,
           MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US),
           (char *) &buffer,
           0,
           NULL) ;
    if (buffer != NULL)
    {
      str += strlen (buffer) > 45 ? ":\n\n" : ": " ;
      str += buffer ;
      LocalFree (buffer) ;
    }
  }
  MessageBox (hMainWindow, str, Title, MB_OK | MB_ICONEXCLAMATION) ;
}


//------------------------------------------------------------------------------------------------------------------------

int GetFileLength (LPCSTR FileName)
{
  struct stat st ;

  return (stat (FileName, &st) == 0 ? st.st_size : -1) ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString GetBaseName (LPCSTR str)
{
  const char  *s;

  for (s = str + strlen (str) - 1 ; s >= str ; s--)
    if (*s == '\\' || *s == '/' || *s == ':')
      break ;
  if (s < str)
    return (str) ;
  return (CStdString (s + 1)) ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString GetFilePath (LPCSTR str)
{
  int         pos ;
  const char  *s;

  for (s = str + strlen (str) - 1 ; s >= str ; s--)
    if (*s == '\\' || *s == '/' || *s == ':')
      break ;
  if (s < str)
    return ("") ;
  pos = (int) (s - str) ;
  return (CStdString (str, pos)) ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString GetFileNameNoExt (LPCSTR str)
{
  CStdString  s1 = GetBaseName (str) ;
  const char  *s2 = strrchr (s1, '.') ;

  if (!s2)
    return (s1) ;
  return (s1.Left ((int) (s2 - (LPCSTR) s1))) ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString GetFileExt (LPCSTR str)
{
  CStdString  s1 = GetBaseName (str) ;
  const char  *s2 = strrchr (s1, '.') ;

  if (!s2)
    return ("") ;
  return (s2 + 1) ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString GetFullPath (LPCSTR str)
{
  char        path [_MAX_PATH] ;
  char        *filename ;

  if (GetFullPathName (str, _MAX_PATH, path, &filename))
    return (path) ;
  return (str) ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString FixPath (CStdString Name)
{
  bool        isUNC = false ;
  CStdString  result = Name.Trim () ;

  if (result.Left (2) == "\\\\")
  {
    result.Delete (0) ;
    isUNC = true ;
  }
  while (result.Replace ("\\\\", "\\")) ;
  if (isUNC)
    result.Insert (0, "\\") ;
  return (result) ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString UnquotePath (CStdString Name)
{
  CStdString  result = Name.Trim () ;

  result = result.TrimLeft ('"').TrimRight ('"').Trim () ;
  return (result) ;
}

//------------------------------------------------------------------------------------------------------------------------
// simple function to get the FILETIME for the underlying file on disk.
// If there is no file, then this function will set the time to 'zero'.
void GetFileTimeFromDisk (LPCSTR FileName, FILETIME& time)
{
  HANDLE      hFile ;

  time.dwLowDateTime = time.dwHighDateTime = 0 ;
  if (FileName [0] != '\0')
  {
    hFile = CreateFile (FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL) ;
    if (hFile != INVALID_HANDLE_VALUE)
    {
      GetFileTime (hFile, NULL, NULL, &time) ;
      CloseHandle (hFile) ;
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------

bool FileExists (LPCSTR FileName)
{
  struct stat st ;

  return (stat (FileName, &st) == 0 && (st.st_mode & _S_IFREG) != 0) ;
}

//------------------------------------------------------------------------------------------------------------------------

void UpdateWindowList (LPCSTR FileName)
{
  // it is legit to compare pointers here
  if (WindowList[0] == FileName)
    return;
  for (int i = 1; i < MAX_EDITORS + 1; i++)
  {
    if (WindowList[i] == NULL)
      break;
    if (WindowList[i] != FileName)
      continue;
    memmove(WindowList + 1, WindowList, sizeof(WindowList[0]) * i);
    WindowList[0] = FileName;
    UpdateWindowMenu();
    return;
  }

  memmove(WindowList + 1, WindowList, sizeof(WindowList) - sizeof(WindowList[0]));
  WindowList[0] = FileName;
  UpdateWindowMenu();
}

//------------------------------------------------------------------------------------------------------------------------

void RemoveFromWindowList (LPCSTR FileName)
{
  // it is legit to compare pointers here
  for (int i = 0; i < MAX_EDITORS + 1; i++)
  {
    if (WindowList[i] == FileName)
    {
      memmove(WindowList + i, WindowList + i + 1, sizeof(WindowList) - sizeof(WindowList[0]) * (i + 1));
      WindowList[MAX_EDITORS] = NULL;
      UpdateWindowMenu();
      break;
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------

void ResetWindowList (void)
{
  memset(WindowList, 0, sizeof(WindowList));
  WindowList[0] = MessageWinTitle;
  UpdateWindowMenu();
}

//------------------------------------------------------------------------------------------------------------------------

void BuildDirList (HMENU hMenu, CStdString Path)
{
  int                   count ;
  int                   flags ;
  HMENU                 m ;
  HANDLE                handle ;
  CStdString            str ;
  CStdString            name ;
  CStdString            newpath ;
  CStdString            caption ;
  CStdStringList        sl ;
  MenuParentInfo        mp ;
  WIN32_FIND_DATA       data ;

  newpath = Path + "\\*.*" ;
  mp.hParent = hMenu ;
  if ((handle = FindFirstFile (newpath, &data)) != INVALID_HANDLE_VALUE)
  {
    do
    {
      if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
      {
        name = data.cFileName ;
        if (name == "." || name == "..")
          continue ;
        sl.AppendItem (name) ;
      }
    } while (FindNextFile (handle, &data)) ;
  }
  FindClose (handle) ;
  count = sl.ItemCount () ;
  for (int i = 0 ; i < count ; i++)
  {
    str = sl [i] ;
    if (str.length () > 5 && str.Find (" - ") == 2)
      if (isalnum (str [0]) && isalnum (str [1]))
        str.Delete (0, 5) ;
    if (str != "-" && str.Left (10) != "----------")
    {
      m = CreateMenu () ;
      mp.hChild = m ;
      mp.index = GetMenuItemCount (hMenu) ;
      BuildDirList (m, Path + "\\" + sl [i]) ;
      flags = MF_POPUP ;
      if (GetMenuItemCount (hMenu) % 32 == 31)
        flags |= MF_MENUBARBREAK ;
      AppendMenu (hMenu, flags, (UINT_PTR) m, str) ;
      if (GetMenuItemCount (m) == 0)
        EnableMenuItem (hMenu, GetMenuItemCount (hMenu) - 1, MF_BYPOSITION | MF_GRAYED) ;
    }
    else
      AppendMenu (hMenu, MF_SEPARATOR, 0, 0) ;
  }
  newpath = Path + "\\*.txt" ;
  sl.Clear () ;
  if ((handle = FindFirstFile (newpath, &data)) != INVALID_HANDLE_VALUE)
    do
      if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        sl.AppendItem (data.cFileName) ;
    while (FindNextFile (handle, &data)) ;
  FindClose (handle) ;
  count = sl.ItemCount () ;
  for (int i = 0 ; i < count ; i++)
  {
    str = GetFileNameNoExt (sl [i]) ;
    if (str.length () > 5 && str.Find (" - ") == 2)
      if (isalnum (str [0]) && isalnum (str [1]))
        str.Delete (0, 5) ;
    if (str != "-" && str.Left (10) != "----------")
    {
      flags = MF_STRING ;
      if (GetMenuItemCount (hMenu) % 32 == 31)
        flags |= MF_MENUBARBREAK ;
      AppendMenu (hMenu, flags, CM_FIRSTINSERTMENUITEM + InsertMenuItems.ItemCount (), str) ;
      InsertMenuItems.AppendItem (Path + "\\" + GetFileNameNoExt (sl [i])) ;
    }
    else
      AppendMenu (hMenu, MF_SEPARATOR, 0, 0) ;
  }
}

//------------------------------------------------------------------------------------------------------------------------

DWORD InsertThread (LPDWORD lpdwParam)
{
  int         index ;
  HANDLE      handle ;

  // The Windows API documentation -claims- that if this function fails it
  // will return INVALID_HANDLE_VALUE. In actual fact, on rare instances,
  // it will return NULL, thus causing all sorts of problems later in this
  // code (because we didn't test for it - as per the documentation and even
  // Microsoft's own sample code).

  // We handle this situation now by re-acquiring it below rather than exiting.
  handle = FindFirstChangeNotification (InsertPath, true, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME) ;
  while (true)
  {
    if (handle == INVALID_HANDLE_VALUE || handle == NULL)
    {
      Sleep (5000) ;
      handle = FindFirstChangeNotification (InsertPath, true, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME) ;
      if (handle == INVALID_HANDLE_VALUE || handle == NULL)
        Sleep (60000) ;
    }
    if (WaitForSingleObject (handle, INFINITE) != WAIT_OBJECT_0 || FindNextChangeNotification (handle) == 0)
    {
      FindCloseChangeNotification (handle) ;
      handle = INVALID_HANDLE_VALUE ;
      Sleep (10000) ;
      continue ;
    }
    InsertMenuItems.Clear () ;
    index = GetMenuItemID (hInsertMenu, 0) == CM_INSERTMENUHELP ? 3 : 2 ;
    // should consider sending a WM_CANCELMODE message here.
    while (DeleteMenu (hInsertMenu, index, MF_BYPOSITION) != 0)
      { /* do nothing */ }

    BuildDirList (hInsertMenu, InsertPath) ;
    Sleep (5000) ;
  }
  FindCloseChangeNotification (handle) ;
  return (0) ;
}

//------------------------------------------------------------------------------------------------------------------------

void MakeFileNames (EditTagStruct *t, LPCSTR str)
{
  char        path [_MAX_PATH] ;
  char        *filename ;

  if (GetFullPathName (str, _MAX_PATH, path, &filename))
  {
    if (filename == NULL)
    {
      t->LongName[0] = '\0';
      t->ShortName[0] = '\0';
    }
    else
    {
      strcpy (t->LongName, path) ;
      strcpy (t->ShortName, filename) ;
    }
  }
  else
  {
    strcpy (t->LongName, str) ;
    strcpy (t->ShortName, GetBaseName (str)) ;
  }
}

//------------------------------------------------------------------------------------------------------------------------

int FindEditorIndex (LPCSTR FileName)
{
  CCodeMax    **e = Editors ;

  for (int i = 0 ; i < EditorCount ; i++, e++)
    if (_stricmp ((*e)->m_Tag.LongName, FileName) == 0)
      return (i) ;
  return (-1) ;
}

//------------------------------------------------------------------------------------------------------------------------

CCodeMax *FindEditor (LPCSTR FileName)
{
  CCodeMax    **e = Editors ;

  for (int i = 0 ; i < EditorCount ; i++, e++)
    if (_stricmp ((*e)->m_Tag.LongName, FileName) == 0)
      return (*e) ;
  return (NULL) ;
}

//------------------------------------------------------------------------------------------------------------------------

CCodeMax *CreateNewEditor (const char *FileName, bool ReadOnly, bool Show, bool IgnoreMissing)
{
  int                   lastError ;
  CCodeMax              *c ;
  CStdString            fullname ;
  struct stat           st ;
  EditTagStruct         tag ;

  debug ("CreateNewEditor: '%s', RO=%d, Show=%d, IgnoreMissing=%d\n", FileName, ReadOnly, Show, IgnoreMissing) ;
  if (FileName != NULL)
  {
    if (strlen(FileName) == 0)
      return (NULL) ;
    if (FileName[strlen(FileName) - 1] == '\\')
      return (NULL) ;
    fullname = GetFullPath (CStdString (FileName)) ;
    debug ("fullname = '%s'\n", (LPCSTR) fullname) ;
    if (FindEditorIndex (fullname) != -1)
    {
      SelectFile (fullname) ;
      return (NULL) ;
    }
    MakeFileNames (&tag, FileName) ;
    if (!IgnoreMissing && !FileExists (tag.LongName))
      return (NULL) ;
  }
  if (EditorCount == MAX_EDITORS)
  {
    ShowMessage ("Maximum number of editing sessions reached", MB_ICONEXCLAMATION) ;
    return (NULL) ;
  }

  c = new CCodeMax (hTabWindow) ;
  c->SetReadOnly (ReadOnly) ;
  c->EnableGlobalProps (false) ;

  if (FileName != NULL)
  {
    c->m_Tag = tag ;
    if (c->OpenFile (c->m_Tag.LongName) != CODEMAX_SUCCESS)
    {
      lastError = GetLastError () ;
      // CodeMax doesn't seem to like opening zero-length files.
      // so we check for ERROR_FILE_INVALID for a file with length 0
      // it's OK if that's the case
      if (lastError != ERROR_FILE_INVALID || stat (c->m_Tag.LongName, &st) != 0 || st.st_size > 0)
      {
        // file not found is OK, too
        if (lastError != ERROR_FILE_NOT_FOUND)
        {
          ShowErrorMessage (c->m_Tag.ShortName, "Failed to open file", lastError) ;
          delete c ;
          return (NULL) ;
        }
        else
          PutStatusMessage (CStdString ("File '") + c->m_Tag.LongName + "' not found ; new file assumed") ;
      }
      else
        PutStatusMessage (CStdString ("File '") + c->m_Tag.LongName + "' has zero length") ;
    }
    UpdateWindowList(c->m_Tag.LongName);
    c->UpdateFileTime () ;
  }
  else
    strcpy (c->m_Tag.ShortName, "Untitled") ;
  c->SetLanguageBasedOnFileType () ;
  c->m_Index = EditorCount++ ;
  Editors [c->m_Index] = c ;
  c->SetupEditor (&EditConfig, false, EditorCount == 1) ;
  c->GetConfigFromInstance (&EditConfig) ;

  InsertTab (c->m_Tag.ShortName) ;

  if (Show)
  {
    TabCtrl_SetCurSel (hTabWindow, EditorCount) ;
    TabIndexChanged () ;
  }
  else
    ShowWindow (c->m_hWnd, SW_HIDE) ;

  return (c) ;
}

//------------------------------------------------------------------------------------------------------------------------

void ShowMessagePane (void)
{
  SetWindowPosition () ;
  if (TabCtrl_GetCurSel (hTabWindow) > 0)
  {
    ShowWindow (hMessageWindow, MessagePaneVisible ? SW_SHOWNOACTIVATE : SW_HIDE) ;
    ShowWindow (hDividerWindow, MessagePaneVisible ? SW_SHOW : SW_HIDE) ;
  }
  else
  {
    ShowWindow (hMessageWindow, SW_SHOW) ;
    ShowWindow (hDividerWindow, SW_HIDE) ;
  }
}

//------------------------------------------------------------------------------------------------------------------------

void InsertTab (LPCSTR title)
{
  int         rows ;
  RECT        rect ;
  TCITEM      item ;

  rows = TabCtrl_GetRowCount (hTabWindow) ;
  item.mask = TCIF_TEXT ;
  item.pszText = (LPSTR) title ;
  TabCtrl_InsertItem (hTabWindow, TabCtrl_GetItemCount (hTabWindow), &item) ;
  if (rows != 0 && TabCtrl_GetRowCount (hTabWindow) != rows)
  {
    GetClientRect (hTabWindow, &rect) ;
    TabCtrl_AdjustRect (hTabWindow, FALSE, &rect) ;
    // what about when the small message window is showing ?
    MoveWindow (hMessageWindow, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE) ;
    for (int i = 0 ; i < EditorCount ; i++)
      MoveWindow (Editors [i]->m_hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE) ;
  }
}

//------------------------------------------------------------------------------------------------------------------------

void DeleteTab (int index)
{
  int         rows ;
  RECT        rect ;

  rows = TabCtrl_GetRowCount (hTabWindow) ;
  TabCtrl_DeleteItem (hTabWindow, index) ;
  if (rows != 1 && TabCtrl_GetRowCount (hTabWindow) != rows)
  {
    GetClientRect (hTabWindow, &rect) ;
    TabCtrl_AdjustRect (hTabWindow, FALSE, &rect) ;
    // what about when the small message window is showing ?
    MoveWindow (hMessageWindow, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE) ;
    for (int i = 0 ; i < EditorCount ; i++)
      MoveWindow (Editors [i]->m_hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE) ;
  }
}

//------------------------------------------------------------------------------------------------------------------------

void TabIndexChanged (void)
{
  TabIndex = TabCtrl_GetCurSel (hTabWindow) ;
  debug ("TabIndexChanged: %d\n", TabIndex) ;

  if (TabIndex == -1)
  {
    TabIndex = TabCtrl_GetItemCount (hTabWindow) - 1 ;
    TabCtrl_SetCurSel (hTabWindow, TabIndex) ;
  }
  if (TabIndex)
  {
    ShowWindow (hMessageWindow, MessagePaneVisible ? SW_SHOWNOACTIVATE : SW_HIDE) ;
    Editor = Editors [TabIndex - 1] ;
    ShowWindow (Editor->m_hWnd, SW_SHOW) ;
    BringWindowToTop (Editor->m_hWnd) ;
    SetActiveWindow (Editor->m_hWnd) ;
    if (IsWindowVisible (hMainWindow))
      if (CanFocus (Editor->m_hWnd))
        SetFocus (Editor->m_hWnd) ;
    Editor->GetConfigFromInstance (&EditConfig) ;
    for (int i = 0 ; i < EditorCount ; i++)
      if (Editors [i] != Editor)
        ShowWindow (Editors [i]->m_hWnd, SW_HIDE) ;
    UpdateWindowList(Editor->m_Tag.LongName);
  }
  else
  {
    if (TabIndex < 0)
    {
      MessageBox (hMainWindow, "Internal editor error: unknown tab destination", "POV-Ray for Windows", MB_OK | MB_ICONEXCLAMATION) ;
      TabIndex = 0 ;
      TabCtrl_SetCurSel (hTabWindow, TabIndex) ;
    }
    ShowWindow (hMessageWindow, SW_SHOW) ;
    SetActiveWindow (hMessageWindow) ;
    BringWindowToTop (hMessageWindow) ;
    SetFocus (hMessageWindow) ;
    for (int i = 0 ; i < EditorCount ; i++)
      ShowWindow (Editors [i]->m_hWnd, SW_HIDE) ;
    Editor = NULL ;
    UpdateWindowList(MessageWinTitle);
  }

  ShowMessagePane () ;
  SetStatusLine () ;
  SetMenuState () ;
  SendMessage (hNotifyWindow, WM_COMMAND, NotifyBase + NotifyTabChange, GetFlags ()) ;
}

//------------------------------------------------------------------------------------------------------------------------

HWND CreateTabWindow (HWND MainWindowHandle, HWND StatusWindowHandle, const char *pBinariesPath, const char *pDocumentsPath)
{
  RECT                            rect ;
  WNDCLASSEX                      wc ;
  NONCLIENTMETRICS                ncm ;
  INITCOMMONCONTROLSEX            icex ;

  debug ("CreateTabWindow: %p %p %s %s\n", hMainWindow, hStatusWindow, pBinariesPath, pDocumentsPath) ;

  CMRegisterControl () ;

  hMainWindow = MainWindowHandle ;
  hStatusWindow = StatusWindowHandle ;
  BinariesPath = pBinariesPath ;
  DocumentsPath = pDocumentsPath ;

  VersionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO) ;
  GetVersionEx (&VersionInfo) ;

  InsertPath = FixPath (DocumentsPath + "Insert Menu") ;
  POVRayIniPath = FixPath (DocumentsPath + "ini\\povray.ini") ;

  icex.dwSize = sizeof (INITCOMMONCONTROLSEX) ;
  icex.dwICC = ICC_WIN95_CLASSES ;
  InitCommonControlsEx (&icex) ;

  GetClientRect (hMainWindow, &rect) ;
  hTabWindow = CreateWindowEx (0L,
                               WC_TABCONTROL,
                               "",
                               WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | TCS_FOCUSNEVER | TCS_MULTILINE | TCS_TOOLTIPS,
                               0,
                               0,
                               rect.right,
                               rect.bottom,
                               hMainWindow,
                               NULL,
                               hInstance,
                               NULL) ;
  hTabWindowTooltip = (HWND) SendMessage(hTabWindow, TCM_GETTOOLTIPS, 0, 0);
  InsertTab ("Messages") ;
  TabIndex = 0 ;
  ResetWindowList();

  ncm.cbSize = sizeof (NONCLIENTMETRICS) ;
  if (SystemParametersInfo (SPI_GETNONCLIENTMETRICS, sizeof (NONCLIENTMETRICS), (void *) &ncm, 0) != 0)
  {
    // set the tab font to the same as that used for the status bar
    HFONT hfont = CreateFontIndirect (&ncm.lfStatusFont) ;
    if (hfont != NULL)
      SendMessage (hTabWindow, WM_SETFONT, (WPARAM) hfont, FALSE) ;
  }

  OldTabWndProc = (WNDPROC) SetWindowLongPtr (hTabWindow, GWLP_WNDPROC, (LONG_PTR) TabWndProc) ;

  hPopupMenu = GetSubMenu (LoadMenu (hInstance, "POPUPMENU"), 0) ;
  hMainMenu = LoadMenu (hInstance, "MAINMENU") ;
  hFileMenu = GetSubMenu (hMainMenu, 0) ;
  hEditMenu = GetSubMenu (hMainMenu, 1) ;
  hSearchMenu = GetSubMenu (hMainMenu, 2) ;
  hTextMenu = GetSubMenu (hMainMenu, 3) ;
  hEditorMenu = GetSubMenu (hMainMenu, 4) ;
  hInsertMenu = GetSubMenu (hMainMenu, 5) ;
  hWindowMenu = GetSubMenu (hMainMenu, 6) ;
  hOlderFilesMenu = GetSubMenu (hFileMenu, 17) ;

  hTabMenu = CreatePopupMenu();
  AppendMenu(hTabMenu, MF_STRING, cmOpenContainingFolder, "Open Containing &Folder");
  AppendMenu(hTabMenu, MF_STRING, cmCopyFilenameToClipboard, "&Copy Filename to Clipboard");
  AppendMenu(hTabMenu, MF_STRING, cmSaveThisFile, "&Save This File");
  AppendMenu(hTabMenu, MF_STRING, cmCloseThisFile, "Close &This File");
  AppendMenu(hTabMenu, MF_STRING, cmCloseOtherFiles, "Close &All But This");
  AppendMenu(hTabMenu, MF_STRING, cmShiftTabLeft, "Shift Tab &Left");
  AppendMenu(hTabMenu, MF_STRING, cmShiftTabRight, "Shift Tab &Right");

  CCodeMax::RegisterCommand (CM_SAVE, "Save", "Save the current file") ;
  CCodeMax::RegisterCommand (CM_SAVEAS, "SaveAs", "Save the current file under a different file name") ;
  CCodeMax::RegisterCommand (CM_SAVEALL, "SaveAll", "Save all modified files") ;
  CCodeMax::RegisterCommand (CM_EXIT, "Exit", "Exit POV-Ray for Windows") ;
  CCodeMax::RegisterCommand (CM_SHOWMESSAGES, "ShowMessages", "Show the mini message panel") ;
  CCodeMax::RegisterCommand (CM_NEWFILE, "NewFile", "Creates a new, unnamed file in the editor") ;
  CCodeMax::RegisterCommand (CM_OPENFILE, "OpenFile", "Opens an existing file via a file browse dialog") ;
  CCodeMax::RegisterCommand (CM_CLOSECURRENTFILE, "CloseFile", "Closes the current editor window, prompting for save if need be") ;
  CCodeMax::RegisterCommand (CM_CLOSEALLFILES, "CloseAllFiles", "Closes all editor windows, prompting for save as needed") ;
  CCodeMax::RegisterCommand (CM_CLOSEALLBUTTHIS, "CloseAllButThis", "Closes all editor windows other than the current one, prompting for save as needed") ;
  CCodeMax::RegisterCommand (CM_SHIFTLEFT, "ShiftTabLeft", "Shifts the current tab window towards the start of the tab list") ;
  CCodeMax::RegisterCommand (CM_SHIFTRIGHT, "ShiftTabRight", "Shifts the current tab window towards the end of the tab list") ;
  CCodeMax::RegisterCommand (CM_PRINT, "Print", "Displays the 'Print File' dialog") ;
  CCodeMax::RegisterCommand (CM_PAGESETUP, "PageSetup", "Displays the 'Page Setup' dialog") ;

  BuildDirList (hInsertMenu, InsertPath) ;

  hInsertThread = CreateThread (NULL, 32768, (LPTHREAD_START_ROUTINE) InsertThread, NULL, 0, &InsertThreadID) ;

  wc.cbSize = sizeof (wc) ;
  wc.hIconSm = NULL ;
  wc.cbClsExtra = 0 ;
  wc.cbWndExtra = 0 ;
  wc.hInstance = hInstance ;
  wc.lpszMenuName = NULL ;
  wc.hbrBackground = (HBRUSH) GetClassLongPtr (hTabWindow, GCLP_HBRBACKGROUND) ;
  wc.style = 0 ;
  wc.lpfnWndProc = HintWndProc;
  wc.hIcon = NULL ;
  wc.hCursor = NULL;
  wc.lpszClassName = "CMEDITINSERTMENUHINT" ;
  RegisterClassEx (&wc) ;

  hCurrentHintWindow = CreateWindowEx (WS_EX_TOPMOST | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE,
                                       "CMEDITINSERTMENUHINT",
                                       "",
                                       WS_POPUP | WS_DISABLED,
                                       0,
                                       0,
                                       32,
                                       32,
                                       hMainWindow,
                                       NULL,
                                       hInstance,
                                       NULL) ;

  wc.cbSize = sizeof (wc) ;
  wc.lpfnWndProc = DragPaneWndProc ;
  wc.hCursor = LoadCursor (hInstance, MAKEINTRESOURCE (IDC_VSPLIT)) ;
  wc.lpszClassName = "CMEDITDRAGPANE" ;
  RegisterClassEx (&wc) ;

  hDividerWindow = CreateWindowEx (WS_EX_TOPMOST | WS_EX_WINDOWEDGE,
                                   "CMEDITDRAGPANE",
                                   "",
                                   WS_CHILD,
                                   0,
                                   0,
                                   rect.right,
                                   4,
                                   hTabWindow,
                                   NULL,
                                   hInstance,
                                   NULL) ;

  return (hTabWindow) ;
}

//------------------------------------------------------------------------------------------------------------------------

DWORD GetDLLVersion (void)
{
  return (EDITDLLVERSION) ;
}

//------------------------------------------------------------------------------------------------------------------------

int CompareKeywords (const void *p1, const void *p2)
{
  return (strcmp (*((char **) p1), *((char **) p2))) ;
}

//------------------------------------------------------------------------------------------------------------------------

void SetWindowPosition (void)
{
  RECT        rect ;

  ShowWindow (hTabWindow, SW_SHOW) ;
  GetClientRect (hTabWindow, &rect) ;
  TabCtrl_AdjustRect (hTabWindow, FALSE, &rect) ;
  if (TabCtrl_GetCurSel (hTabWindow) > 0 && MessagePaneVisible)
  {
    if (EditDragOffset > rect.bottom - 64)
      EditDragOffset = rect.bottom - 64 ;
    if (EditDragOffset < 32)
    {
      EditDragOffset = 32 ;
      MessagePaneVisible = false ;
      MoveWindow (hMessageWindow, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE) ;
      ShowWindow (hMessageWindow, SW_HIDE) ;
      ShowWindow (hDividerWindow, SW_HIDE) ;
      if (EditPaneDragActive)
      {
        EditPaneDragActive = false ;
        EditDragOffset = EditStartDragOffset ;
        ReleaseCapture () ;
      }
    }
    else
    {
      rect.bottom -= EditDragOffset ;
      MoveWindow (hMessageWindow, rect.left, rect.bottom + 4, rect.right - rect.left, EditDragOffset - 4, TRUE) ;
      MoveWindow (hDividerWindow, rect.left, rect.bottom, rect.right - rect.left, 4, TRUE) ;
    }
  }
  else
    MoveWindow (hMessageWindow, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE) ;
  for (int i = 0 ; i < EditorCount ; i++)
    MoveWindow (Editors [i]->m_hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE) ;
  SetStatusLine () ;
}

void SetWindowPosition (int x, int y, int w, int h)
{
  debug ("SetWindowPosition: %d,%d w=%d h=%d\n", x, y, w, h) ;
  if (hMainWindow == NULL || hTabWindow == NULL)
    return ;
  MoveWindow (hTabWindow, x, y, w, h, TRUE) ;
  SetWindowPosition () ;
}

void SetMessageWindow (HWND MsgWindow)
{
  RECT        rect ;

  debug ("SetMessageWindow: %p\n", MsgWindow) ;
  hMessageWindow = MsgWindow ;
  if (hMainWindow == NULL || hTabWindow == NULL)
    return ;
  GetClientRect (hTabWindow, &rect) ;
  TabCtrl_AdjustRect (hTabWindow, FALSE, &rect) ;
  MoveWindow (hMessageWindow, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE) ;
}

void RestoreState (int RestoreFiles)
{
  debug ("RestoreState: %d\n", RestoreFiles) ;

  CRegDef *r = new CRegDef ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit") ;
  GetSettings (r, &EditConfig, RestoreFiles != 0) ;
  delete r ;

  TabIndexChanged () ;
  MakeRecentMenus () ;
  MakeOlderMenus () ;
  SetMenuShortcuts () ;
  SetStatusLine () ;
}

void SaveState (void)
{
  debug ("SaveState\n") ;

  CRegDef *r = new CRegDef ("Software\\" REGKEY "\\" REGVERKEY "\\POV-Edit") ;
  PutSettings (r, &EditConfig) ;
  delete r ;
}

bool SelectFile (const char *FileName)
{
  int         index ;

  debug ("SelectFile: %s\n", FileName) ;
  if (hMainWindow == NULL || hTabWindow == NULL)
    return (false) ;
  if (FileName == NULL)
    return (false) ;
  if ((index = FindEditorIndex (GetFullPath (FileName))) == -1)
    return (false) ;
  TabCtrl_SetCurSel (hTabWindow, index + 1) ;
  TabIndexChanged () ;
  return (true) ;
}

void CheckReload (void)
{
  bool                  cancel = false ;
  CCodeMax              *e ;
  CStdString            str ;
  static bool           active = false ;

  if (active)
    return ;
  active = true ;
  if (AutoReload != 0)
  {
    if (AutoReload == 2)
    {
      // always
      for (int i = 0 ; i < EditorCount ; i++)
      {
        e = Editors [i] ;
        if (!e->IsCodeUpToDate (false))
          e->OpenFile (e->m_Tag.LongName) ;
      }
    }
    else
    {
      // ask
      for (int i = 0 ; i < EditorCount ; i++)
      {
        e = Editors [i] ;
        if (!e->IsCodeUpToDate (false))
        {
          if (!cancel)
          {
            switch (ShowReloadDialog (e->m_Tag.ShortName))
            {
              case IDRELOAD :
                   e->OpenFile (e->m_Tag.LongName) ;
                   break ;

              case IDDISCARD :
                   break ;

              case IDCANCEL :
                   cancel = true ;
                   break ;

              default :
                   cancel = true ;
                   break ;
            }
          }
          e->UpdateFileTime () ;
        }
      }
    }
  }
  active = false ;
}

bool ShowParseError (char *FileName, char *Message, int Line, int Col)
{
  bool        isthis = false ;

  debug ("ShowParseError: '%s', '%s', %d, %d\n", FileName, Message, Line, Col) ;
  if (hMainWindow == NULL || hTabWindow == NULL)
    return (false) ;
  if (Editor != NULL)
    if (_stricmp (Editor->m_Tag.LongName, FileName) == 0)
      isthis = true ;
  if (!isthis)
  {
    if (!IsMenuItemChecked (CM_AUTOLOADERRORFILE))
      return (false) ;
    if (!SelectFile (FileName))
      if (CreateNewEditor (FileName, false, true, false) == NULL)
        return (false) ;
  }
  Editor->SetLineNo (Line) ;
  Editor->SetColNo (Col) ;
  Editor->SetErrorLine (Line) ;
  PutStatusMessage (Message) ;
  ShowMessages (false) ;
  return (true) ;
}

bool BrowseFile (bool CreateNewWindow)
{
  int                   ok = 0 ;
  char                  *s ;
  CStdString            fileName ;
  OPENFILENAME          ofn ;
  char                  szFile [8192] = "" ;
  static char           lastDir [_MAX_PATH] ;

  debug ("BrowseFile: %d\n", CreateNewWindow) ;
  if (hMainWindow == NULL || hTabWindow == NULL)
    return (false) ;
  ZeroMemory (&ofn, sizeof (ofn)) ;
  ofn.lStructSize = sizeof (ofn) ;
  ofn.hwndOwner = hMainWindow ;
  ofn.lpstrFile = szFile ;
  ofn.nMaxFile = sizeof (szFile) ;
  ofn.lpstrFilter = "POV-Ray Files (*.pov;*.inc;*.ini;*.mac;*.mcr)\0*.pov;*.inc;*.ini;*.mac;*.mcr\0"
                    "POV-Ray Source (*.pov)\0*.pov\0"
                    "Include Files (*.inc;*.mac;*.mcr)\0*.inc;*.mac;*.mcr\0"
                    "INI files (*.ini)\0*.ini\0"
                    "Text Files (*.txt)\0*.txt\0"
                    "All Files (*.*)\0*.*\0" ;
  ofn.nFilterIndex = 1 ;
  ofn.lpstrFileTitle = NULL ;
  ofn.nMaxFileTitle = 0 ;
  ofn.lpstrInitialDir = InitialDir ;
  ofn.lpstrDefExt = "pov" ;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_EXTENSIONDIFFERENT ;
  ofn.Flags |= OFN_CREATEPROMPT | OFN_ALLOWMULTISELECT | OFN_EXPLORER ;

  if (GetOpenFileName ((struct tagOFNA *) &ofn) != 0)
  {
    s = szFile + ofn.nFileOffset ;
    InitialDir = szFile ;
    if (!ofn.nFileOffset || szFile [ofn.nFileOffset - 1])
      InitialDir = GetFilePath (InitialDir) ;
    strcpy (lastDir, InitialDir) ;
    while (*s)
    {
      fileName = InitialDir + "\\" + s ;
      // skip up to and over the next '\0'
      while (*s++) ;
      if (FindEditorIndex (fileName) != -1)
      {
        SelectFile (fileName) ;
        continue ;
      }
      if (CreateNewEditor (fileName, false, false, false) == NULL)
        break ;
      AddToRecent (EncodeFilename(fileName)) ;
      ok++ ;
    }
  }
  if (ok)
  {
    TabCtrl_SetCurSel (hTabWindow, EditorCount) ;
    TabIndex = EditorCount ;
    TabIndexChanged () ;
  }
  return (ok != 0) ;
}

bool LoadFile (char *FileName)
{
  bool        result ;

  result = CreateNewEditor (FileName, false, true, true) != NULL ;
  if (result && FileName != NULL)
    AddToRecent (EncodeFilename(FileName)) ;
  return (result) ;
}

bool ExternalLoadFile (char *ParamString)
{
  int                   Line ;
  CCodeMax              *e ;
  CStdString            params = ParamString ;
  CStdString            FileName ;

  debug ("ExternalLoadFile: %s\n", ParamString) ;
  if (params == "" || params [1] == ',')
    return (false) ;
  FileName = GetFullPath (DecodeFilename(GetNextField(params))) ;
  e = FindEditor (FileName) ;
  if (e == NULL)
    if ((e = CreateNewEditor (FileName, false, false, true)) == NULL)
      return (false) ;
  SelectFile (FileName) ;
  if (params != "")
    e->SetLineNo (Line = atoi (GetNextField (params))) ;
  if (params != "")
    e->SetColNo (atoi (GetNextField (params))) ;
  if (params != "")
  {
    e->SetTopLine (atoi (GetNextField (params))) ;
    e->SetLineNo (Line) ;
  }
  if (params != "")
    e->SetLanguage ((TLanguage) atoi (GetNextField (params))) ;
  AddToRecent (EncodeFilename(FileName)) ;
  SetStatusLine () ;
  return (true) ;
}

bool CloseAll (CCodeMax *except /* = NULL */)
{
  bool                  saveAll = false ;
  bool                  cancel = false ;
  bool                  soundPlayed = false ;
  bool                  discard ;
  CCodeMax              *e ;
  TSaveType             result ;

  if (CCodeMax::SaveDialogActive)
    return (false) ;

  UpdateRecent () ;
  for (int i = EditorCount - 1 ; i >= 0 && !cancel ; i--)
  {
    e = Editors [i] ;
    if (e == except)
      continue;
    if (e->IsModified ())
    {
      discard = false ;
      if (!saveAll)
      {
        if (!soundPlayed)
        {
          PlaySound ("SystemQuestion", NULL, SND_ASYNC | SND_ALIAS | SND_NOWAIT) ;
          soundPlayed = true ;
        }
        switch (ShowFileChangedDialog (e->m_Tag.ShortName, true))
        {
          case IDSAVEALL :
               saveAll = true ;
               break ;

          case IDSAVE :
               break ;

          case IDDISCARD :
               discard = true ;
               break ;

          case IDCANCEL :
               cancel = true ;
               continue ;

          default :
               cancel = true ;
               continue ;
        }
      }
      if (!discard)
      {
        while ((result = e->TrySave (true)) == stRetry) ;
        if (result == stCancel)
        {
          cancel = true;
          continue ;
        }
      }
    }
    RemoveFromWindowList(e->m_Tag.LongName);
    if (e == Editor)
      Editor = NULL ;
    delete e ;
    memcpy (Editors + i, Editors + i + 1, (--EditorCount - i) * sizeof (e)) ;
    DeleteTab (i + 1) ;
  }
  if (Editor == NULL)
  {
    TabCtrl_SetCurSel (hTabWindow, EditorCount > 0 ? 1 : 0) ;
    TabIndexChanged () ;
  }
  return (!cancel) ;
}

void ShiftTab(bool left, int index)
{
  int substitute = left ? index - 1 : index + 1;
  CCodeMax *e = Editors[index - 1];
  TCITEM item;

  if (index < 1 || index > EditorCount || substitute < 1 || substitute > EditorCount)
    return;
  item.mask = TCIF_TEXT ;
  item.pszText = Editors[substitute - 1]->m_Tag.ShortName;
  TabCtrl_SetItem (hTabWindow, index, &item) ;
  item.pszText = Editors[index - 1]->m_Tag.ShortName;
  TabCtrl_SetItem (hTabWindow, substitute, &item) ;
  Editors[index - 1] = Editors[substitute - 1];
  Editors[index - 1]->m_Index = index - 1;
  Editors[substitute - 1] = e;
  Editors[substitute - 1]->m_Index = substitute - 1;
  if (Editor == Editors[index - 1] || Editor == Editors[substitute - 1])
  {
    TabCtrl_SetCurSel (hTabWindow, TabIndex = substitute) ;
    TabIndexChanged () ;
  }
}

bool CloseFile (int index)
{
  int                   result ;
  CCodeMax              *e ;

  debug ("CloseFile: %d\n", index) ;

  if (index-- < 1)
    return (false) ;
  if (CCodeMax::SaveDialogActive)
    return (false) ;

  UpdateRecent () ;
  e = Editors [index] ;
  if (e->IsModified ())
  {
    PlaySound ("SystemQuestion", NULL, SND_ASYNC | SND_ALIAS | SND_NOWAIT) ;
    switch (ShowFileChangedDialog (e->m_Tag.ShortName, false))
    {
      case IDSAVE :
      case IDSAVEALL :
           while ((result = e->TrySave (false)) == stRetry) ;
           if (result == stCancel)
             return (false) ;
           break ;

      case IDDISCARD :
           break ;

      case IDCANCEL :
           return (false) ;

      default :
           return (false) ;
    }
  }
  RemoveFromWindowList(e->m_Tag.LongName);
  if (e == Editor)
    Editor = NULL ;
  memcpy (Editors + index, Editors + index + 1, (--EditorCount - index) * sizeof (CCodeMax *)) ;
  for (int i = 0 ; i < EditorCount ; i++)
    Editors [i]->m_Index = i ;
  DeleteTab (index + 1) ;
  if (Editor == NULL)
  {
    TabCtrl_SetCurSel (hTabWindow, TabIndex = index + 1) ;
    TabIndexChanged () ;
  }
  delete e ;
  return (true) ;
}

bool CloseFile (char *FileName)
{
  int                   index ;
  int                   result ;
  CCodeMax              *e ;

  debug ("CloseFile: %s\n", FileName) ;

  if (CCodeMax::SaveDialogActive)
    return (false) ;

  UpdateRecent () ;
  if (FileName != NULL)
    index = FindEditorIndex (GetFullPath (FileName)) ;
  else
    index = TabCtrl_GetCurSel (hTabWindow) - 1 ;
  if (index == -1)
    return (false) ;
  e = Editors [index] ;
  if (e->IsModified ())
  {
    PlaySound ("SystemQuestion", NULL, SND_ASYNC | SND_ALIAS | SND_NOWAIT) ;
    switch (ShowFileChangedDialog (e->m_Tag.ShortName, false))
    {
      case IDSAVE :
      case IDSAVEALL :
           while ((result = e->TrySave (false)) == stRetry) ;
           if (result == stCancel)
             return (false) ;
           break ;

      case IDDISCARD :
           break ;

      case IDCANCEL :
           return (false) ;

      default :
           return (false) ;
    }
  }
  RemoveFromWindowList(e->m_Tag.LongName);
  if (e == Editor)
    Editor = NULL ;
  memcpy (Editors + index, Editors + index + 1, (--EditorCount - index) * sizeof (CCodeMax *)) ;
  for (int i = 0 ; i < EditorCount ; i++)
    Editors [i]->m_Index = i ;
  DeleteTab (index + 1) ;
  TabCtrl_SetCurSel (hTabWindow, TabIndex = index + 1) ;
  TabIndexChanged () ;
  delete e ;
  return (true) ;
}

bool SaveFile (char *FileName)
{
  CCodeMax              *e = Editor ;
  TSaveType             result = stDiscard ;

  debug ("SaveFile: %s\n", FileName) ;
  if (FileName != NULL)
    e = FindEditor (GetFullPath (FileName)) ;
  if (e != NULL)
    while ((result = e->TrySave (false)) == stRetry) ;
  return (result == stSaved) ;
}

DWORD GetTab (void)
{
  if (hTabWindow == NULL)
    return (0) ;
  return (TabIndex = TabCtrl_GetCurSel (hTabWindow)) ;
}

void NextTab (bool Forward)
{
  debug ("NextTab: %d\n", Forward) ;
  TabIndex = TabCtrl_GetCurSel (hTabWindow) ;
  if (Forward)
    TabCtrl_SetCurSel (hTabWindow, TabIndex < EditorCount ? ++TabIndex : 0) ;
  else
    TabCtrl_SetCurSel (hTabWindow, TabIndex ? --TabIndex : EditorCount) ;
  TabIndexChanged () ;
}

DWORD GetFlags (void)
{
  DWORD                 flags = 0 ;

  if (Editor != NULL)
  {
    if (Editor->IsModified ())
      flags |= EDIT_CURRENT_MODIFIED ;
    if (Editor->CanUndo ())
      flags |= EDIT_CAN_UNDO ;
    if (Editor->CanRedo ())
      flags |= EDIT_CAN_REDO ;
    if (!Editor->IsReadOnly () && Editor->m_Tag.LongName != "")
      flags |= EDIT_CAN_WRITE ;
    if (EditPaneDragActive)
      flags |= EDIT_DRAG_ACTIVE ;
  }
  else
    flags |= EDIT_MSG_SELECTED ;
  for (int i = 0 ; i < EditorCount ; i++)
  {
    if (Editors [i]->IsModified ())
    {
      flags |= EDIT_ANY_MODIFIED ;
      break ;
    }
  }
  if (EditorCount < MAX_EDITORS)
    flags |= EDIT_CAN_OPEN ;
  if (EditorCount > 0)
    flags |= EDIT_CAN_CLOSE ;
  return (flags) ;
}

char *GetFilename (void)
{
  static char           str [_MAX_PATH] ;

  if (Editor == NULL)
    return (NULL) ;
  strcpy (str, Editor->m_Tag.LongName) ;
  if (str [0] == '\0')
    strcpy (str, "Untitled") ;
  return (str) ;
}

bool SaveAllFiles (bool IncludeUntitled)
{
  bool                  callUR = true ;
  bool                  triedSave = false;
  CCodeMax              *e ;
  TSaveType             result ;
  static bool           active = false ;

  if (active || CCodeMax::SaveDialogActive)
    return (false) ;
  active = true ;

  for (int i = 0 ; i < EditorCount && !CCodeMax::SaveDialogActive ; i++)
  {
    e = Editors [i] ;
    if (e->m_Tag.LongName [0] == '\0')
      if (!IncludeUntitled)
        continue ;
    if (e->IsModified ())
    {
      if (callUR)
      {
        UpdateRecent () ;
        callUR = false ;
      }
      triedSave = true;
      while ((result = e->TrySave (true)) == stRetry) ;
      if (result == stCancel)
        break;
    }
  }
  if (triedSave)
    SetStatusLine () ;
  active = false ;
  return (triedSave) ;
}

bool CanClose (bool AllFiles)
{
  bool                  saveAll = false ;
  bool                  soundPlayed = false ;
  CCodeMax              **e = Editors ;
  TSaveType             result ;

  debug ("CanClose: %d\n", AllFiles) ;

  if (CCodeMax::SaveDialogActive)
    return (false) ;

  UpdateRecent () ;
  if (AllFiles)
  {
    for (int i = 0 ; i < EditorCount ; i++, e++)
    {
      if ((*e)->IsModified ())
      {
        if (!saveAll)
        {
          if (!soundPlayed)
          {
            PlaySound ("SystemQuestion", NULL, SND_ASYNC | SND_ALIAS | SND_NOWAIT) ;
            soundPlayed = true ;
          }
          switch (ShowFileChangedDialog ((*e)->m_Tag.ShortName, true))
          {
            case IDSAVE :
                 break ;

            case IDSAVEALL :
                 saveAll = true ;
                 break ;

            case IDDISCARD :
                 continue ;

            case IDCANCEL :
                 return (false) ;

            default :
                 return (false) ;
          }
        }
        while ((result = (*e)->TrySave (true)) == stRetry) ;
        if (result == stCancel)
          return (false) ;
      }
    }
  }
  else
  {
    if (Editor != NULL && Editor->IsModified ())
    {
      switch (ShowFileChangedDialog (Editor->m_Tag.ShortName, false))
      {
        case IDSAVE :
        case IDSAVEALL :
             while ((result = Editor->TrySave (false)) == stRetry) ;
             return (result != stCancel) ;

        case IDDISCARD :
             return (true) ;

        case IDCANCEL :
             return (false) ;

        default :
             return (false) ;
      }
    }
  }
  return (true) ;
}

bool SaveModified (char *FileName)
{
  int                   n ;
  bool                  saveAll ;
  CCodeMax              **e = Editors ;
  CCodeMax              *se = NULL ;
  TSaveType             result ;

  debug ("SaveModified: %s\n", FileName) ;

  if (CCodeMax::SaveDialogActive)
    return (false) ;

  UpdateRecent () ;
  saveAll = AutoRenderSave ;
  if (FileName != NULL)
    se = FindEditor (GetFullPath (FileName)) ;
  for (int i = 0 ; i < EditorCount ; i++, e++)
  {
    if ((*e)->IsModified ())
    {
      if (!saveAll)
      {
        if (se != NULL && *e != se)
          continue ;
        n = ShowSaveBeforeRenderDialog ((*e)->m_Tag.ShortName) ;
        if (n == -1)
          return (false) ;
        switch (n & 0xffff)
        {
          case IDSAVE :
               if ((n & DONTASKAGAINFLAG) != 0)
                 saveAll = AutoRenderSave = true ;
               break ;

          case IDSAVEALL :
               if ((n & DONTASKAGAINFLAG) != 0)
                 AutoRenderSave = true ;
               saveAll = true ;
               break ;

          case IDNO :
               if ((n & DONTASKAGAINFLAG) != 0)
                 AutoRenderSave = true ;
               continue ;

          case IDCANCEL :
               return (false) ;

          default :
               return (false) ;
        }
      }
      while ((result = (*e)->TrySave (true)) == stRetry) ;
      if (result == stCancel)
        return (false) ;
    }
  }
  return (true) ;
}

// return true if the caller should show the message window themselves
bool ShowMessages (bool on)
{
  debug ("ShowMessages: %d\n", on) ;
  if (!IsMenuItemChecked (CM_SHOWPARSEMESSAGES))
    return (false) ;
  MessagePaneVisible = on ;
  if (Editor != NULL)
    ShowMessagePane () ;
  return (Editor == NULL) ;
}

// not currently used
void DispatchMenuId (DWORD id)
{
  debug ("DispatchMenuId: %u\n", id) ;
}

HMENU GetMenuHandle (int which)
{
  switch (which)
  {
    case GetFileMenu :
         return (hFileMenu) ;

    case GetEditMenu :
         return (hEditMenu) ;

    case GetSearchMenu :
         return (hSearchMenu) ;

    case GetTextMenu :
         return (hTextMenu) ;

    case GetInsertMenu :
         return (hInsertMenu) ;

    case GetOptionsMenu :
         return (hEditorMenu) ;

    case GetWindowMenu :
         return (hWindowMenu) ;

    default :
         return (NULL) ;
  }
}

void SetNotifyBase (HWND WindowHandle, int MessageBase)
{
  debug ("SetNotifyBase: %p %d\n", WindowHandle, MessageBase) ;

  hNotifyWindow = WindowHandle ;
  NotifyBase = MessageBase ;
}

void UpdateMenus (HMENU MenuHandle)
{
  debug ("UpdateMenus: %p\n", MenuHandle) ;
  SetMenuState () ;
}

void GetContextHelp (void)
{
  static char str [256] ;

  if (Editor != NULL)
  {
    strncpy (str, Editor->GetCurrentKeyword (), 255) ;
    SendMessage (hMainWindow, KEYWORD_LOOKUP_MESSAGE, 0, (LPARAM) str) ;
  }
}

void SetTabFocus (void)
{
  if (hMainWindow == NULL)
    return ;
  if (IsWindowVisible (hMainWindow))
    SetFocus (Editor != NULL ? Editor->m_hWnd : hMessageWindow) ;
}

void AddNewuserHelp (HMENU hMenu, UINT id, bool hasSeparator)
{
  if (GetMenuItemID (hMenu, 0) != id)
  {
    if (hasSeparator)
      InsertMenu (hMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, NULL) ;
    InsertMenu (hMenu, 0, MF_BYPOSITION, id, "&Help On This Menu") ;
  }
}

void RemoveNewuserHelp (HMENU hMenu, UINT id, bool hasSeparator)
{
  if (GetMenuItemID (hMenu, 0) == id)
  {
    DeleteMenu (hMenu, 0, MF_BYPOSITION) ;
    if (hasSeparator)
      DeleteMenu (hMenu, 0, MF_BYPOSITION) ;
  }
}

void ShowNewuserHelp (bool Visible)
{
  if (Visible)
  {
    AddNewuserHelp (hFileMenu, CM_FILEMENUHELP, true) ;
    AddNewuserHelp (hEditMenu, CM_EDITMENUHELP, true) ;
    AddNewuserHelp (hSearchMenu, CM_SEARCHMENUHELP, true) ;
    AddNewuserHelp (hTextMenu, CM_TEXTMENUHELP, true) ;
    AddNewuserHelp (hEditorMenu, CM_EDITORMENUHELP, true) ;
    AddNewuserHelp (hInsertMenu, CM_INSERTMENUHELP, false) ;
    AddNewuserHelp (hWindowMenu, CM_WINDOWMENUHELP, true) ;
  }
  else
  {
    RemoveNewuserHelp (hFileMenu, CM_FILEMENUHELP, true) ;
    RemoveNewuserHelp (hEditMenu, CM_EDITMENUHELP, true) ;
    RemoveNewuserHelp (hSearchMenu, CM_SEARCHMENUHELP, true) ;
    RemoveNewuserHelp (hTextMenu, CM_TEXTMENUHELP, true) ;
    RemoveNewuserHelp (hEditorMenu, CM_EDITORMENUHELP, true) ;
    RemoveNewuserHelp (hInsertMenu, CM_INSERTMENUHELP, false) ;
    RemoveNewuserHelp (hWindowMenu, CM_WINDOWMENUHELP, true) ;
  }
}

const char **GetWindowList(void)
{
  return WindowList;
}

//------------------------------------------------------------------------------------------------------------------------

void SelectClosestItem (HWND hWnd, LPCSTR Item)
{
  int         index ;
  int         itemheight ;
  int         lvheight ;
  int         visitems ;
  int         desiredtop ;
  int         delta ;
  int         currenttop ;
  RECT        rect ;
  LVFINDINFO  lvfi ;

  if (Item != NULL)
  {
    lvfi.psz = Item ;
    lvfi.flags = LVFI_STRING | LVFI_PARTIAL ;
    index = ListView_FindItem (hWnd, -1, &lvfi) ;
  }
  else
    index = ListView_GetNextItem (hWnd, -1, LVIS_SELECTED) ;

  if (index != -1)
  {
    ListView_SetItemState (hWnd, index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED) ;
    ListView_EnsureVisible (hWnd, index, FALSE) ;
    ListView_GetItemRect (hWnd, index, &rect, LVIR_BOUNDS) ;
    itemheight = rect.bottom - rect.top ;
    if (itemheight != 0)
    {
      GetClientRect (hWnd, &rect) ;
      lvheight = rect.bottom ;
      visitems = lvheight / itemheight ;
      desiredtop = index - visitems / 2 ;
      if (desiredtop >= 0)
      {
        currenttop = ListView_GetTopIndex (hWnd) ;
        delta = (desiredtop - currenttop) * itemheight ;
        ListView_Scroll (hWnd, 0, delta) ;
        ListView_EnsureVisible (hWnd, index, FALSE) ;
      }
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------

void ActionSelection (CCodeMax *e, LPCSTR Text)
{
  int           col = e->GetColNo () - 1 ;
  CodemaxRange  range ;
  CStdString    ln = e->GetLine (e->GetLineNo ()) ;
  const char    *line = ln ;
  const char    *s = line + col ;

  range.Start.Line = range.End.Line = e->GetLineNo () - 1 ;
  if (isalnum (*s) || *s == '_')
  {
    // we seem to be within a word
    while (s-- > line)
      if (!(isalnum (*s) || *s == '_'))
        break ;
    range.Start.Col = (int) (++s - line) ;
    while (*++s)
      if (!(isalnum (*s) || *s == '_'))
        break ;
    range.End.Col = (int) (s - line) ;
  }
  else
  {
    // it's either a space or the end of the line.
    range.End.Col = col ;
    while (s-- > line)
      if (!(isalnum (*s) || *s == '_'))
        break ;
    range.Start.Col = (int) (++s - line) ;
  }

  e->ReplaceText (Text, &range) ;
  e->SetCaretPos (range.Start.Line + 1, range.Start.Col + (int) strlen (Text) + 1) ;
}

//------------------------------------------------------------------------------------------------------------------------

int KeywordMatches (LPCSTR Text)
{
  int         matches = 0 ;
  int         len = (int) strlen (Text) ;

  for (int i = 0 ; i < KeywordCount ; i++)
    if (strncmp (Keywords [i], Text, len) == 0)
      matches++ ;
  return (matches) ;
}

bool CompleteMatch (LPCSTR Text)
{
  for (int i = 0 ; i < KeywordCount ; i++)
    if (strcmp (Keywords [i], Text) == 0)
      return (true) ;
  return (false) ;
}

bool SingleKeywordMatch (CCodeMax *e, LPCSTR Text)
{
  int         matches = 0 ;
  int         index ;
  int         len = (int) strlen (Text) ;

  for (int i = 0 ; i < KeywordCount ; i++)
  {
    if (strncmp (Keywords [i], Text, len) == 0)
    {
      matches++ ;
      index = i ;
    }
  }
  if (matches != 1)
    return (false) ;
  ActionSelection (e, Keywords [index]) ;
  return (true) ;
}

bool KeywordExpansion (CCodeMax *e, bool Reverse, bool OverlayCommand)
{
  int           col = e->GetColNo () - 1 ;
  CodemaxRange  range ;
  const char    *keyword = NULL ;

  if (OverlayCommand)
  {
    e->GetSel (&range) ;
    if (memcmp (&range.Start, &range.End, sizeof (range.Start)) != 0)
      return (false) ;
  }

  CStdString ln = e->GetLine (e->GetLineNo ()) ;
  const char *line = ln ;

  if (ExpansionAnchorCol == -1)
  {
    const char *s = line + col ;
    if (isalnum (*s) || *s == '_')
    {
      // we are potentially in the middle of an existing keyword or identifier - bail out.
      if (!OverlayCommand)
      {
        FlashWindow (hMainWindow, TRUE) ;
        FlashWindow (hMainWindow, FALSE) ;
      }
      return (false) ;
    }
    while (s-- > line)
      if (!(isalnum (*s) || *s == '_'))
        break ;
    ExpansionWord = ++s ;
    if (OverlayCommand)
      if (s == line + col)
        return (false) ;
    ExpansionAnchorCol = (int) (s - line) ;
    ExpansionAnchorLine = e->GetLineNo () - 1 ;
    ExpansionLastCol = col ;
    ExpansionIndex = -1 ;
    ExpansionWord = ExpansionWord.Left (col - ExpansionAnchorCol) ;
    ExpansionHasCompleteMatch = CompleteMatch (ExpansionWord) ;
    if (ExpansionHasCompleteMatch)
    {
      if (OverlayCommand)
      {
        ExpansionAnchorCol = -1 ;
        return (false) ;
      }
      ExpansionIndex = 0 ;
    }
  }

  range.Start.Line = range.End.Line = ExpansionAnchorLine ;
  range.Start.Col = ExpansionAnchorCol ;
  range.End.Col = col ;

  line = ExpansionWord ;
  int len = (int) strlen (line) ;
  int total = KeywordMatches (line) ;
  if (total == 0)
  {
    if (!OverlayCommand)
    {
      FlashWindow (hMainWindow, TRUE) ;
      FlashWindow (hMainWindow, FALSE) ;
    }
    return (false) ;
  }
  int matches = 0 ;
  int minindex = ExpansionHasCompleteMatch ? 0 : -1 ;

  if (Reverse)
  {
    if (ExpansionIndex < minindex + 1)
    {
      FlashWindow (hMainWindow, TRUE) ;
      FlashWindow (hMainWindow, FALSE) ;
      return (true) ;
    }
    if (--ExpansionIndex == minindex)
      keyword = line ;
  }
  else
  {
    if (ExpansionIndex >= total - 1)
    {
      FlashWindow (hMainWindow, TRUE) ;
      FlashWindow (hMainWindow, FALSE) ;
      return (true) ;
    }
    ExpansionIndex++ ;
  }

  for (int i = 0 ; keyword == NULL && i < KeywordCount ; i++)
    if (strncmp (Keywords [i], line, len) == 0)
      if (matches++ == ExpansionIndex)
        keyword = Keywords [i] ;

  if (keyword != NULL)
  {
    ExpansionLastCol = range.Start.Col + (int) strlen (keyword) ;
    e->SetCaretPos (ExpansionAnchorLine + 1, ExpansionLastCol + 1) ;
    e->ReplaceText (keyword, &range) ;
  }
  else
  {
    FlashWindow (hMainWindow, TRUE) ;
    FlashWindow (hMainWindow, FALSE) ;
  }

  return (true) ;
}

//------------------------------------------------------------------------------------------------------------------------

bool IsHintMessage (UINT msg)
{
  return ((msg >= WM_KEYFIRST)   && (msg <= WM_KEYLAST))   ||
          ((msg > WM_MOUSEMOVE)  && (msg <= WM_MOUSELAST)) ||
          (msg == WM_ACTIVATE)   ||
          (msg == WM_KEYDOWN)    ||
          (msg == WM_SYSCOMMAND) ||
          (msg == WM_COMMAND)    ||
          (msg == WM_NCMOUSEMOVE) ;
}

void ClearBMPHint (void)
{
  if (pCurrentHintBitmap)
  {
    ShowWindow (hCurrentHintWindow, SW_HIDE) ;
    delete pCurrentHintBitmap;
    pCurrentHintBitmap = NULL;
  }
}

void ClearBMPHint (UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (hCurrentHintWindow != NULL)
    if (IsHintMessage (msg))
      ClearBMPHint () ;
}

void ShowHint (HMENU hMenu, DWORD id)
{
  int                   count ;
  int                   index ;
  int                   width ;
  int                   height ;
  char                  str [32] ;
  RECT                  rect ;
  POINT                 point ;
  CStdString            baseFile = InsertMenuItems [id - CM_FIRSTINSERTMENUITEM] ;
  CStdString            filename;

  if (baseFile == "")
    return ;
  if (FileExists (baseFile + ".png"))
    filename = baseFile + ".png";
  else if (FileExists (baseFile + ".jpg"))
    filename = baseFile + ".jpg";
  else if (FileExists (baseFile + ".bmp"))
    filename = baseFile + ".bmp";
  else
    return;

  count = GetMenuItemCount (hMenu) ;
  for (index = 0 ; index < count ; index++)
    if (GetMenuItemID (hMenu, index) == id)
      break ;
  if (index == count)
    return ;

  if (pCurrentHintBitmap != NULL)
    delete pCurrentHintBitmap;

  CStdStringW fn(filename);
  pCurrentHintBitmap = new Bitmap(fn, true);
  if (pCurrentHintBitmap->GetLastStatus() != Gdiplus::Ok)
  {
    delete pCurrentHintBitmap;
    pCurrentHintBitmap = NULL;
    return ;
  }
  width = pCurrentHintBitmap->GetWidth();
  height = pCurrentHintBitmap->GetHeight();
  if (width < 16 || height < 16 || width > 800 || height > 600)
    return ;

  // we -cannot- rely on GetMenuItemRect to return useful co-ordinates
  // if given a window handle (no matter what the Microsoft docs say).
  // therefore this only seems to work on W2K or later
  if (GetMenuItemRect (NULL, hMenu, index, &rect))
  {
    rect.right = rect.left - 4 ;
    rect.left = rect.right - width - 4 ;
    rect.top -= (height + 4 + rect.top - rect.bottom - 1) / 2 ;
    rect.bottom = rect.top + height + 4 ;
  }
  else
  {
    bool ok = false ;
    GetCursorPos (&point) ;
    HWND hwnd = WindowFromPoint (point) ;
    if (hwnd != NULL)
    {
      if (GetClassName (hwnd, str, sizeof (str)) && strcmp (str, "#32768") == 0)
      {
        RECT menuRect ;
        GetWindowRect (hwnd, &menuRect) ;
        int border = GetSystemMetrics (SM_CXEDGE) ;
        if (GetMenuItemRect (hMainWindow, hMenu, 0, &rect))
        {
          int x = rect.left ;
          int y = rect.top ;
          if (GetMenuItemRect (hMainWindow, hMenu, index, &rect))
          {
            x = rect.left - x ;
            OffsetRect (&rect, -rect.left, -y) ;
            OffsetRect (&rect, menuRect.left + x, menuRect.top) ;
            OffsetRect (&rect, border, border) ;
            rect.right = rect.left - 4 ;
            rect.left = rect.right - width - 4 ;
            rect.top -= (height + 4 + rect.top - rect.bottom - 1) / 2 ;
            rect.bottom = rect.top + height + 4 ;
            ok = true ;
          }
        }
      }
    }
    if (!ok)
    {
      // do the best we can here ... assume that the cursor is where the
      // user wants the image to be shown (not necessarily the case if
      // they are navigating with the keyboard, but what can we do ? ...)
      rect.left = point.x - width / 2 ;
      rect.right = rect.left + width + 4 ;
      rect.bottom = point.y - 5 ;
      rect.top = rect.bottom - height - 4 ;
    }
  }
  MoveWindow (hCurrentHintWindow, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE) ;
  ShowWindow (hCurrentHintWindow, SW_SHOWNA) ;
}

//------------------------------------------------------------------------------------------------------------------------

bool CopyTextToClipboard(CStdString str)
{
  char        *s ;
  HGLOBAL     hText ;

  if (OpenClipboard(NULL) == false)
    return false;
  EmptyClipboard() ;
  if ((hText = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, str.size() + 1)) == NULL)
    return false;
  if ((s = (char *) GlobalLock(hText)) == NULL)
  {
    GlobalFree(hText);
    return false;
  }
  strcpy(s, str.c_str());
  GlobalUnlock(hText) ;
  SetClipboardData(CF_TEXT, hText);
  CloseClipboard();
  return true;
}

//------------------------------------------------------------------------------------------------------------------------

bool PassOnMessage (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, DWORD *rVal)
{
  char                  str [1024] ;
  NMHDR                 *nmh ;
  TSaveType             result;
  static int            count ;

  switch (message)
  {
    case WM_TIMER :
         if (count++ % 4 != 3)
           break ;
         if (AutoSaveDelay != 0)
         {
           if (AutoSaveCountdown > AutoSaveDelay || AutoSaveCountdown == 0)
             AutoSaveCountdown = AutoSaveDelay ;
           if (--AutoSaveCountdown == 0)
           {
             AutoSaveCountdown = AutoSaveDelay ;
             if (SaveAllFiles (false))
               PutStatusMessage ("File(s) auto-saved") ;
           }
         }
         break ;

    case WM_NOTIFY :
         nmh = (NMHDR *) lParam ;
         if (nmh->hwndFrom == hTabWindow)
         {
           switch (nmh->code)
           {
             case TCN_SELCHANGE :
                  TabIndexChanged () ;
                  return (true) ;

             case NM_RCLICK :
                  {
                    DWORD pos = GetMessagePos();
                    POINT p = {GET_X_LPARAM(pos), GET_Y_LPARAM(pos)};
                    ScreenToClient(hTabWindow, &p);
                    TCHITTESTINFO ht = {p, 0};
                    int tab = SendMessage(hTabWindow, TCM_HITTEST, 0, (LPARAM) &ht);
                    if (tab > 0 && tab < EditorCount + 1)
                    {
                      CCodeMax *e = Editors[tab - 1];
                      CStdString str;
                      EnableMenuItem (hTabMenu, cmSaveThisFile, MF_BYCOMMAND | (e->IsModified() ? MF_ENABLED : MF_DISABLED)) ;
                      EnableMenuItem (hTabMenu, cmCloseThisFile, MF_BYCOMMAND | (CCodeMax::SaveDialogActive ? MF_DISABLED : MF_ENABLED)) ;
                      EnableMenuItem (hTabMenu, cmCloseOtherFiles, MF_BYCOMMAND | (CCodeMax::SaveDialogActive || EditorCount < 2 ? MF_DISABLED : MF_ENABLED)) ;
                      EnableMenuItem (hTabMenu, cmShiftTabLeft, MF_BYCOMMAND | (tab == 1 ? MF_DISABLED : MF_ENABLED)) ;
                      EnableMenuItem (hTabMenu, cmShiftTabRight, MF_BYCOMMAND | (tab == EditorCount ? MF_DISABLED : MF_ENABLED)) ;
                      int id = TrackPopupMenuEx(hTabMenu,
                                                GetSystemMetrics(SM_MENUDROPALIGNMENT) ? TPM_RIGHTALIGN : TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                                GET_X_LPARAM(pos),
                                                GET_Y_LPARAM(pos),
                                                hTabWindow,
                                                NULL) ;
                      switch(id)
                      {
                        case 0:
                          break;

                        case cmOpenContainingFolder:
                          str.Format("/select,%s", e->m_Tag.LongName);
                          ShellExecute (NULL, NULL, "explorer.exe", str.c_str(), NULL, SW_SHOWNORMAL) ;
                          break;

                        case cmCopyFilenameToClipboard:
                          CopyTextToClipboard(e->m_Tag.LongName);
                          break;

                        case cmSaveThisFile:
                          if (!CCodeMax::SaveDialogActive)
                            while ((result = e->TrySave (false)) == stRetry) ;
                          break;

                        case cmCloseThisFile:
                          if (!CCodeMax::SaveDialogActive)
                            CloseFile(tab);
                          break;

                        case cmCloseOtherFiles:
                          if (!CCodeMax::SaveDialogActive)
                            CloseAll(e);
                          break;

                        case cmShiftTabLeft:
                          if (tab > 1)
                            ShiftTab(true, tab);
                          break;

                        case cmShiftTabRight:
                          if (tab < EditorCount - 1)
                            ShiftTab(false, tab);
                          break;
                      }
                    }
                  }
                  return (true);
           }
         }
         break ;

    case WM_COMMAND :
         if (HandleCommand (wParam, lParam))
         {
           *rVal = 0 ;
           return (true) ;
         }
         break ;

    case WM_ACTIVATEAPP :
         if (IsWindowVisible (hMainWindow) && !IsIconic (hMainWindow))
         {
           if (wParam != 0)
           {
             if (HadSaveModified)
             {
               HadSaveModified = false;
               WarnSaveModified = false;
               SendMessage (hNotifyWindow, WM_COMMAND, NotifyBase + NotifyFocusSaveModified, 0) ;
             }
             CheckReload () ;
           }
           else if (AutoReload)
           {
             if (SaveAllFiles (false))
               if (WarnSaveModified)
                  HadSaveModified = true;
           }
         }
         break ;

    case HIDE_NEWUSER_HELP_MESSAGE :
         ShowNewuserHelp (wParam == 0) ;
         return (true) ;

    case WM_MENUSELECT :
         ClearBMPHint () ;
         if ((HIWORD (wParam) & MF_POPUP) == 0)
           if (LOWORD (wParam) >= CM_FIRSTINSERTMENUITEM && LOWORD (wParam) <= CM_LASTINSERTMENUITEM)
             ShowHint ((HMENU) lParam, LOWORD (wParam)) ;
         if (LoadString (hInstance, wParam, str, sizeof (str)) != 0)
         {
           *rVal = 0 ;
           PutStatusMessage (str) ;
           return (true) ;
         }
         // pass on to main application, which will clear the status bar if needed
         break ;

    case WM_INITMENU :
         SetMenuState () ;
         // pass on to main application
         break ;

    case WM_INITMENUPOPUP :
         if ((HMENU) wParam == hPopupMenu ||
             (HMENU) wParam == hFileMenu ||
             (HMENU) wParam == hEditMenu ||
             (HMENU) wParam == hSearchMenu ||
             (HMENU) wParam == hEditorMenu ||
             (HMENU) wParam == hTextMenu ||
             (HMENU) wParam == hInsertMenu)
         {
           // it's one of ours, but for now we don't do anything since
           // WM_INITMENU should have taken care of it.
           *rVal = 0 ;
           return (true) ;
         }
         break ;
  }
  return (false) ;
}

void SetKeywords (LPCSTR SDLKeywordList, LPCSTR INIKeywordList)
{
  LangPOVRay.Keywords = SDLKeywordList ;
  CCodeMax::RegisterLanguage ("POV-Ray", &LangPOVRay) ;
  LangPOVRay.Keywords = NULL ;

  char *str = _strdup (SDLKeywordList) ;
  char *s = strtok (str, "\n") ;
  while (s != NULL && KeywordCount < MAX_KEYWORDS)
  {
    if (strlen (s) > 1)
      Keywords [KeywordCount++] = s ;
    s = strtok (NULL, "\n") ;
  }
  // we don't free str - it's still being used.
  qsort (Keywords, KeywordCount, sizeof (char *), CompareKeywords) ;

  LangINI.Keywords = INIKeywordList ;
  CCodeMax::RegisterLanguage ("INI", &LangINI) ;
  LangINI.Keywords = NULL ;
}

//------------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK TabWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                           lastLine ;
  char                          str [256] ;
  NMHDR                         *nmh ;
  LVITEM                        item ;
  CodemaxRange                  range ;
  CodemaxKeydata                *keydata ;
  CodemaxCodeListdata           *codelist ;
  CodemaxRegisteredCommandData  *cmddata ;

  ClearBMPHint (message, wParam, lParam) ;

  if (message == WM_MBUTTONUP && wParam == 0)
  {
    POINT pt = {LOWORD(lParam), HIWORD(lParam)} ;
    TCHITTESTINFO hittestinfo = { pt, 0 } ;
    int tabindex = TabCtrl_HitTest(hWnd, &hittestinfo) ;
    if ((tabindex >= 0) && ((hittestinfo.flags & TCHT_ONITEMLABEL) == TCHT_ONITEMLABEL))
    {
      LRESULT result = CallWindowProc (OldTabWndProc, hWnd, message, wParam, lParam) ;
      if (tabindex > 0)
        CloseFile(tabindex);
      else
        PlaySound ("SystemExclamation", NULL, SND_ASYNC | SND_ALIAS | SND_NOWAIT) ;
      return result;
    }
  }

  if (message == WM_NOTIFY)
  {
    nmh = (NMHDR *) lParam ;
    if (nmh->hwndFrom == hTabWindowTooltip)
    {
      if (nmh->code == TTN_GETDISPINFOA)
      {
        NMTTDISPINFOA *ttdata = (NMTTDISPINFOA *) lParam ;

        if (nmh->idFrom > 0 && nmh->idFrom < (unsigned int) EditorCount + 1)
        {
          CStdString str(Editors[nmh->idFrom - 1]->m_Tag.LongName);
          strcpy(ttdata->szText, str.Right(79).c_str());
          return (true);
        }
      }
      else if (nmh->code == TTN_GETDISPINFOW)
      {
        NMTTDISPINFOW *ttdata = (NMTTDISPINFOW *) lParam ;

        if (nmh->idFrom > 0 && nmh->idFrom < (unsigned int) EditorCount + 1)
        {
          CStdStringW str(Editors[nmh->idFrom - 1]->m_Tag.LongName);
          wcscpy(ttdata->szText, str.Right(79).c_str());
          return (true);
        }
      }
    }
  }

  if (Editor != NULL)
  {
    if (message == WM_SETFOCUS)
    {
      SetFocus (Editor->m_hWnd) ;
      return (0) ;
    }
    if (message == WM_NOTIFY)
    {
      if (nmh->hwndFrom == Editor->m_hWnd)
      {
        switch (nmh->code)
        {
          case CODEMAX_NOTIFY_REGISTEREDCMD :
            cmddata = (CodemaxRegisteredCommandData *) lParam ;
               if (cmddata->Command >= CM_FIRST && cmddata->Command <= CM_LAST)
               {
                 // have to use PostMessage here as any command that could
                 // close the current editor window can make codemax fairly
                 // unhappy. (for that matter it's probably not advisable
                 // to re-enter codemax even if we're not closing an editor).
                 PostMessage (hMainWindow, WM_COMMAND, cmddata->Command, 0) ;
                 return (true) ;
               }
               switch (cmddata->Command)
               {
                 case CMD_NEXT_KEYWORD :
                      KeywordExpansion (Editor, false, false) ;
                      break ;

                 case CMD_PREV_KEYWORD :
                      KeywordExpansion (Editor, true, false) ;
                      break ;
               }
               return (true) ;

          case CODEMAX_NOTIFY_CODELISTPOSTCREATE :
               SelectClosestItem (hCodeListWindow, NULL) ;
               return (true) ;

          case CODEMAX_NOTIFY_OVERTYPECHANGE :
               SendMessage (hStatusWindow, SB_SETTEXT, StatusIns, (LPARAM) (Editor->IsOvertypeMode () ? "\tOvr" : "\tIns")) ;
               return (true) ;

          case CODEMAX_NOTIFY_KEYDOWN :
               keydata = (CodemaxKeydata *) lParam ;
               if ((keydata->KeyModifier & ~CODEMAX_KEY_SHIFT) != 0)
                 break ;
               if (hCodeListWindow == NULL)
                 if (EditConfig.TabKeywordExpansion && keydata->KeyCode == VK_TAB)
                   return (IgnoreNext = KeywordExpansion (Editor, keydata->KeyModifier == CODEMAX_KEY_SHIFT, true)) ;
               return (false) ;

          case CODEMAX_NOTIFY_KEYPRESS :
               if (IgnoreNext)
               {
                 IgnoreNext = false ;
                 return (true) ;
               }
               keydata = (CodemaxKeydata *) lParam ;
               if ((keydata->KeyModifier & ~CODEMAX_KEY_SHIFT) != 0)
                 return (false) ;
               if (hCodeListWindow != NULL && isprint (keydata->KeyCode))
               {
                 str [0] = (char) keydata->KeyCode ;
                 str [1] = '\0' ;
                 SelectClosestItem (hCodeListWindow, Editor->GetCurrentWord () + str) ;
               }
               return (false) ;

          case CODEMAX_NOTIFY_CODELIST :
               if (SingleKeywordMatch (Editor, Editor->GetCurrentWord ()))
                 return (false) ;
               codelist = (CodemaxCodeListdata *) lParam ;
               memset (&item, 0, sizeof (item)) ;
               item.mask = LVIF_TEXT ;
               item.iItem = 0 ;
               for (int i = 0 ; i < KeywordCount ; i++)
               {
                 item.pszText = Keywords [i] ;
                 ListView_InsertItem (codelist->hListCtrl, &item) ;
               }
               SelectClosestItem (codelist->hListCtrl, Editor->GetCurrentWord ()) ;
               hCodeListWindow = codelist->hListCtrl ;
               IgnoreNext = true ;
               return (true) ;

          case CODEMAX_NOTIFY_CODELISTSELMADE :
               codelist = (CodemaxCodeListdata *) lParam ;
               ListView_GetItemText (codelist->hListCtrl, ListView_GetNextItem (codelist->hListCtrl, -1, LVIS_SELECTED), 0, str, sizeof (str)) ;
               ActionSelection (Editor, str) ;
               hCodeListWindow = NULL ;
               return (false) ;

          case CODEMAX_NOTIFY_CODELISTCANCEL :
               hCodeListWindow = NULL ;
               return (false) ;

          case CODEMAX_NOTIFY_PROPSCHANGE :
               Editor->GetConfigFromInstance (&EditConfig) ;
               Editor->SetFontOwnership (false) ;
               for (int i = 0 ; i < EditorCount ; i++)
                 if (Editors [i] != Editor)
                   Editors [i]->SetupEditor (&EditConfig, true, false) ;
               CheckMenuItem (CM_CONSTRAINCARET, EditConfig.SelBoundsEnabled) ;
               if (EditConfig.SelBoundsEnabled)
               {
                 CheckMenuItem (CM_CURSORBEYONDEOL, false) ;
                 EnableMenuItem (CM_CURSORBEYONDEOL, false) ;
               }
               else
                 EnableMenuItem (CM_CURSORBEYONDEOL, true) ;
               SetMenuShortcuts () ;
               return (true) ;

          case CODEMAX_NOTIFY_MODIFIEDCHANGE :
               Editor->ClearErrorLine () ;
               SendMessage (hStatusWindow, SB_SETTEXT, StatusModified, (LPARAM) (Editor->IsModified () ? "\tMod" : "")) ;
               SendMessage (hNotifyWindow, WM_COMMAND, NotifyBase + NotifyModifiedChange, Editor->IsModified ()) ;
               return (true) ;

          case CODEMAX_NOTIFY_SELCHANGE :
               Editor->GetSel (&range, false) ;
               if (ExpansionAnchorCol != -1)
                 if (range.Start.Col != ExpansionLastCol || range.Start.Line != ExpansionAnchorLine)
                   ExpansionAnchorCol = ExpansionAnchorLine = -1 ;
               lastLine = range.End.Line + 1 ;
               wsprintf (str, "\tL:%d", lastLine) ;
               SendMessage (hStatusWindow, SB_SETTEXT, StatusLine, (LPARAM) str) ;
               wsprintf (str, "\tC:%d", ++range.End.Col) ;
               SendMessage (hStatusWindow, SB_SETTEXT, StatusCol, (LPARAM) str) ;
               return (true) ;

          case NM_RCLICK :
               // should never get this as we intercept WM_CONTEXTMENU in the window procedure.
               return (true) ;
        }
      }
    }
  }
  return (CallWindowProc (OldTabWndProc, hWnd, message, wParam, lParam)) ;
}

//------------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK DragPaneWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_LBUTTONDOWN :
         SetCapture (hWnd) ;
         EditPaneDragActive = true ;
         EditStartDragOffset = EditDragOffset ;
         EditStartDragY = GET_Y_LPARAM (lParam) ;
         return (0) ;

    case WM_LBUTTONUP :
         if (!EditPaneDragActive)
           break ;
         ReleaseCapture () ;
         EditPaneDragActive = false ;
         if (EditDragOffset < 32)
         {
           EditDragOffset = EditStartDragOffset ;
           MessagePaneVisible = false ;
           ShowMessagePane () ;
         }
         else
         {
           SetWindowPosition () ;
           InvalidateRect (hMessageWindow, NULL, FALSE) ;
         }
         return (0) ;

    case WM_MOUSEMOVE :
         if (EditPaneDragActive)
         {
           RECT rect ;
           GetClientRect (hTabWindow, &rect) ;
           TabCtrl_AdjustRect (hTabWindow, FALSE, &rect) ;
           int delta = EditStartDragY - GET_Y_LPARAM (lParam) ;
           if (EditDragOffset + delta < 0 || EditDragOffset + delta > rect.bottom - 64)
             return (0) ;
           EditDragOffset += delta ;
           MoveWindow (hDividerWindow, rect.left, rect.bottom - EditDragOffset, rect.right - rect.left, 4, TRUE) ;
           SetWindowPosition () ;
         }
         return (0) ;
  }
  return (DefWindowProc (hWnd, message, wParam, lParam)) ;
}

//------------------------------------------------------------------------------------------------------------------------

void HintWindowPaint(HDC hdc)
{
  Gdiplus::Graphics gr(hdc);

  // NB if we don't specify the width and height, DrawImage() will scale the bitmap if its DPI != HDC's DPI
  gr.DrawImage(pCurrentHintBitmap, 0, 0, pCurrentHintBitmap->GetWidth(), pCurrentHintBitmap->GetHeight());
}

LRESULT CALLBACK HintWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  HDC           hdc;
  PAINTSTRUCT   ps;

  if (pCurrentHintBitmap != NULL && message == WM_PAINT)
  {
    hdc = BeginPaint(hWnd, &ps);
    HintWindowPaint(hdc);
    EndPaint(hWnd, &ps);
    return(0);
  }
  return (DefWindowProc (hWnd, message, wParam, lParam));
}

//------------------------------------------------------------------------------------------------------------------------

void CleanUp (void)
{
}
