//******************************************************************************
///
/// @file windows/pvedit.cpp
///
/// This file contains editor support code.
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
//*******************************************************************************

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#include <windows.h>
#include <shellapi.h>
#include <cstring>
#include <sys/stat.h>
#include "pvengine.h"
#include "pvedit.h"
#include "parser/reservedwords.h"

// this must be the last file included
#include "syspovdebug.h"

namespace pov_frontend
{
  extern struct ProcessOptions::INI_Parser_Table RenderOptions_INI_Table[];
}
// end of namespace pov_frontend

namespace povwin
{

using namespace pov_parser;

HINSTANCE               hLibPovEdit ;

extern bool             debugging ;
extern HWND             main_window ;
extern bool             use_editors;
extern bool             editors_enabled;

static HWND (*CreateTabWindow) (HWND ParentWindow, HWND StatusWindow, const char *BinariesPath, const char *DocumentsPath) ;
static DWORD (*GetDLLVersion) (void) ;
static void (*SetWindowPosition) (int x, int y, int w, int h) ;
static void (*SetMessageWindow) (HWND MessageWindow) ;
static void (*RestoreState) (int RestoreFiles) ;
static void (*SaveState) (void) ;
static bool (*SelectFile) (char *FileName) ;
static bool (*BrowseFile) (bool CreateNewWindow) ;
static bool (*LoadFile) (char *FileName) ;
static bool (*CloseFile) (char *FileName) ;
static bool (*SaveFile) (char *FileName) ;
static bool (*ExternalLoadFile) (char *ParamString) ;
static DWORD (*GetTab) (void) ;
static DWORD (*GetFlags) (void) ;
static char *(*GetFilename) (void) ;
static void (*NextTab) (bool Forward) ;
static bool (*CanClose) (bool AllFiles) ;
static bool (*SaveModified) (char *FileName) ;
static bool (*ShowMessages) (bool on) ;
static void (*DispatchMenuId) (DWORD id) ;
static HMENU (*GetMenuHandle) (int which) ;
static void (*SetNotifyBase) (HWND WindowHandle, int MessageBase) ;
static void (*UpdateMenus) (HMENU MenuHandle) ;
static void (*GetContextHelp) (void) ;
static void (*SetTabFocus) (void) ;
static bool (*ShowParseError) (const char *FileName, const char *Message, int Line, int Col) ;
static bool (*PassOnMessage) (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, DWORD *rVal) ;
static void (*SetKeywords) (LPCSTR SDLKeywordList, LPCSTR INIKeywords) ;
static const char **(*GetWindowList) (void);

void                    **FunctionPtrs [] =
                        {
                         (void **) &GetWindowList,
                         (void **) &SetWindowPosition,
                         (void **) &SetMessageWindow,
                         (void **) &RestoreState,
                         (void **) &SaveState,
                         (void **) &SelectFile,
                         (void **) &BrowseFile,
                         (void **) &LoadFile,
                         (void **) &CloseFile,
                         (void **) &SaveFile,
                         (void **) &ExternalLoadFile,
                         (void **) &GetTab,
                         (void **) &GetFlags,
                         (void **) &GetFilename,
                         (void **) &NextTab,
                         (void **) &CanClose,
                         (void **) &SaveModified,
                         (void **) &ShowMessages,
                         (void **) &DispatchMenuId,
                         (void **) &GetMenuHandle,
                         (void **) &SetNotifyBase,
                         (void **) &UpdateMenus,
                         (void **) &GetContextHelp,
                         (void **) &SetTabFocus,
                         (void **) &ShowParseError,
                         (void **) &PassOnMessage,
                         (void **) &SetKeywords,
                         (void **) &CreateTabWindow,
                         NULL
                        } ;

char                    *FunctionNames [] =
                        {
                         "GetWindowList",
                         "SetWindowPosition",
                         "SetMessageWindow",
                         "RestoreState",
                         "SaveState",
                         "SelectFile",
                         "BrowseFile",
                         "LoadFile",
                         "CloseFile",
                         "SaveFile",
                         "ExternalLoadFile",
                         "GetTab",
                         "GetFlags",
                         "GetFilename",
                         "NextTab",
                         "CanClose",
                         "SaveModified",
                         "ShowMessages",
                         "DispatchMenuId",
                         "GetMenuHandle",
                         "SetNotifyBase",
                         "UpdateMenus",
                         "GetContextHelp",
                         "SetTabFocus",
                         "ShowParseError",
                         "PassOnMessage",
                         "SetKeywords",
                         "CreateTabWindow",
                         NULL
                        } ;

bool LoadEditorDLL (char *path, bool errorOK)
{
  int                   DllVersion ;
  int                   err ;
  char                  str [2048] ;
  char                  **s ;
  void                  ***f ;
  struct stat           statbuf ;

   if (!editors_enabled)
    return (false) ;
  if (debugging)
    debug_output ("Trying to load editor DLL from '%s' [%s]\n", path, stat (path, &statbuf) != 0 ? "missing" : "found") ;
  if (hLibPovEdit != NULL)
    FreeLibrary(hLibPovEdit);
  if ((hLibPovEdit = LoadLibraryEx (path, 0, LOAD_WITH_ALTERED_SEARCH_PATH)) == NULL)
  {
    err = GetLastError () ;
    if (debugging)
      debug_output ("Could not load editor DLL '%s', error code is %08lx\n", path, err) ;
    if (!errorOK)
    {
#ifdef _DEBUG
      sprintf (str,
               "Editor DLL initialisation failed [LoadLibrary failed, code is %08lx]\n\n"
               "Debug build: if you have renamed cmedit as cmeditd, keep in mind that it "
               "will still look for povcmax.dll, not povcmaxd.dll. Also, for debug builds "
               "the build output directory is checked for the DLL's before the install dir.\n\n"
               "Check the README in the source for instructions on how to copy the DLL's from "
               "the official distribution to the naming convention used by debug builds.",
               err) ;
      PovMessageBox (str, "POV-Ray Editor error") ;
      PovMessageBox ("See the 'Internal Editor Reference' section in the help file for\n"
                     "instructions on how to correct this or turn editor loading off.", "Important!") ;
#else
      if (MessageBox (main_window,
                      "POV-Ray could not load its internal editor. This is to be expected "
                      "if you installed the AGPL-licensed distribution, as the editor DLL "
                      "is not included. If you do not wish to use the editor you can turn "
                      "it off (and stop this message appearing) by unchecking 'Use Editor' "
                      "under the 'Other Settings' sub-menu of the Options menu.\n\n"
                      "If you wish to enable the editor, clicking 'Yes' below will take "
                      "you to its download page.",
                      "Could not load internal editor - download it?",
                      MB_ICONQUESTION | MB_YESNO) == IDYES)
                        ShellExecute (NULL, NULL, "http://www.povray.org/download/wineditdll/" POV_RAY_VERSION,
                                      NULL, NULL, SW_SHOWNORMAL) ;
#endif
    }
    return (false) ;
  }

  GetDLLVersion = (DWORD (*) (void)) GetProcAddress (hLibPovEdit, "GetDLLVersion") ;
  if (GetDLLVersion == NULL)
  {
    if (debugging)
      debug_output ("Could not resolve GetDLLVersion, error code is %08lx\n", GetLastError ()) ;
    if (!errorOK)
      PovMessageBox ("Editor DLL initialization failed [could not resolve GetDLLVersion]\n\n"
                     "See the 'Built-In Editors' section in the help file",
                     "POV-Ray Editor Error") ;
    return (false) ;
  }
  if ((DllVersion = GetDLLVersion ()) != EDITDLLVERSION)
  {
    sprintf (str, "ERROR : Wrong editor DLL version [expected %04x, got %04x].", EDITDLLVERSION, DllVersion) ;
    if (debugging)
      debug_output ("%s\n", str) ;
    if (!errorOK)
    {
      MessageBox (NULL, str, "POV-Ray Editor Error", MB_ICONSTOP) ;
      PovMessageBox ("Editor switched off.\n\nSee the 'Built-In Editors' section in the help file", "Important!") ;
    }
    return (false) ;
  }
  for (s = FunctionNames, f = FunctionPtrs ; *s ; s++, f++)
  {
    **f = (void *) GetProcAddress (hLibPovEdit, *s) ;
    if (**f == NULL)
    {
      if (debugging)
        debug_output ("Could not get address of '%s', error code is %08lx\n", *s, GetLastError ()) ;
      if (!errorOK)
      {
        sprintf (str, "Editor DLL initialization failed:\nCould not resolve %s.", *s) ;
        PovMessageBox (str, "Editor Error") ;
        PovMessageBox ("See the 'Built-In Editors' section in the help file", "Important!") ;
      }
      return (false) ;
    }
  }
  if (debugging)
    debug_output ("Loaded editor DLL '%s'\n", path) ;
  return (true) ;
}

char *Get_Reserved_Words (const char *additional_words)
{
  int length = 0 ;
  int i ;

  for (i = 0; Reserved_Words[i].Token_Number != TOKEN_COUNT; i++)
  {
    if (!isalpha (Reserved_Words [i].Token_Name [0]))
      continue ;
    if (strchr (Reserved_Words [i].Token_Name, ' ') != NULL)
      continue ;
    length += (int)strlen (Reserved_Words[i].Token_Name) + 1 ;
  }

  length += (int)strlen (additional_words) + 2;

  char *result = (char *) malloc (++length) ;
  strcpy (result, additional_words) ;
  strcat (result, "#\n");
  char *s = result + strlen (additional_words) + 2;

  for (i = 0; Reserved_Words[i].Token_Number != TOKEN_COUNT; i++)
  {
    if (!isalpha (Reserved_Words [i].Token_Name [0]))
      continue ;
    if (strchr (Reserved_Words [i].Token_Name, ' ') != NULL)
      continue ;
    s += sprintf (s, "%s\n", Reserved_Words[i].Token_Name) ;
  }
  *--s = '\0' ;

  return (result) ;
}

char *Get_INI_Keywords(void)
{
  int length = 0 ;

  for (struct pov_frontend::ProcessOptions::INI_Parser_Table *op = pov_frontend::RenderOptions_INI_Table; op->keyword != NULL; op++)
    length += strlen(op->keyword) + 1;
  char *result = (char *) malloc (++length) ;
  char *s = result;
  for (struct pov_frontend::ProcessOptions::INI_Parser_Table *op = pov_frontend::RenderOptions_INI_Table; op->keyword != NULL; op++)
    s += sprintf (s, "%s\n", op->keyword) ;
  *--s = '\0' ;
  return (result) ;
}

char *Get_User_Keywords(const char *DocumentsPath)
{
  char        str[_MAX_PATH];
  char        *s;
  FILE        *f;
  std::string result;

  sprintf(str, "%sini\\user-keywords.txt", DocumentsPath);
  if ((f = fopen(str, "rt")) == NULL)
    return _strdup("");
  while (fgets(str, sizeof (str), f) != NULL)
  {
    s = clean(str);
    if (*s != '\0' && *s != '#')
    {
      // don't allow any words with spaces
      if (strchr(s, ' ') != NULL)
        continue;
      result += s;
      result += '\n';
    }
  }
  fclose (f) ;
  return _strdup(result.c_str());
}

HWND InitialiseEditor (HWND ParentWindow, HWND StatusWindow, const char *BinariesPath, const char *DocumentsPath)
{
  HWND        hwnd ;

  if (!editors_enabled || !use_editors)
    return (NULL) ;

  char *s1 = Get_User_Keywords(DocumentsPath);
  char *s2 = Get_Reserved_Words(s1) ;
  char *s3 = Get_INI_Keywords() ;
  EditSetKeywords (s2, s3) ;
  free(s1) ;
  free(s2);
  free(s3);
  if ((hwnd = CreateTabWindow (ParentWindow, StatusWindow, BinariesPath, DocumentsPath)) == NULL)
  {
    PovMessageBox ("TabWindow error: see the 'Built-In Editors' section in the help file", "Important!") ;
    return (NULL) ;
  }
  return (hwnd) ;
}

void SetEditorPosition (int x, int y, int w, int h)
{
  if (!use_editors)
    return ;
  SetWindowPosition (x, y, w, h) ;
}

void EditSetMessageWindow (HWND MessageWindow)
{
  if (!use_editors)
    return ;
  SetMessageWindow (MessageWindow) ;
}

void EditRestoreState (int RestoreFiles)
{
  if (!use_editors)
    return ;
  RestoreState (RestoreFiles) ;
}

void EditSaveState (void)
{
  if (!use_editors)
    return ;
  SaveState () ;
}

bool EditSelectFile (char *FileName)
{
  if (!use_editors)
    return (false) ;
  return (SelectFile (FileName)) ;
}

bool EditBrowseFile (bool CreateNewWindow)
{
  if (!use_editors)
    return (false) ;
  return (BrowseFile (CreateNewWindow)) ;
}

// NULL means create a new, untitled, editor window.
bool EditOpenFile (char *FileName)
{
  if (!use_editors)
    return (false) ;
  return (LoadFile (FileName)) ;
}

// NULL means currently selected file
bool EditCloseFile (char *FileName)
{
  if (!use_editors)
    return (false) ;
  return (CloseFile (FileName)) ;
}

// NULL means currently selected file
bool EditSaveFile (char *FileName)
{
  if (!use_editors)
    return (false) ;
  return (SaveFile (FileName)) ;
}

bool EditExternalOpenFile (char *ParamString)
{
  if (!use_editors)
    return (false) ;
  return (ExternalLoadFile (ParamString)) ;
}

DWORD EditGetTab (void)
{
  if (!use_editors)
    return (0) ;
  return (GetTab ()) ;
}

DWORD EditGetFlags (void)
{
  if (!use_editors)
    return (EDIT_MSG_SELECTED) ;
  return (GetFlags ()) ;
}

char *EditGetFilename (bool IncludeModifiedIndicator)
{
  char                  *s;
  static char           str [_MAX_PATH + 2] ;

  if (!use_editors)
    return (NULL) ;
  s = GetFilename();
  if (IncludeModifiedIndicator == false || s == NULL || *s == '\0')
    return s;
  if ((GetFlags() & EDIT_CURRENT_MODIFIED) == 0)
    return s;
  strcpy(str, s);
  strcat(str, "*");
  return str;
}

void EditNextTab (bool Forward)
{
  if (!use_editors)
    return ;
  NextTab (Forward) ;
}

bool EditCanClose (bool AllFiles)
{
  if (!use_editors)
    return (true) ;
  return (CanClose (AllFiles)) ;
}

bool EditSaveModified (char *FileName)
{
  if (!use_editors)
    return (true) ;
  return (SaveModified (FileName)) ;
}

bool EditShowMessages (bool on)
{
  // return true if the caller should show the message window themselves
  if (!use_editors)
    return (true) ;
  return (ShowMessages (on)) ;
}

void EditDispatchMenuId (DWORD id)
{
  if (!use_editors)
    return ;
  DispatchMenuId (id) ;
}

void EditUpdateMenus (HMENU MenuHandle)
{
  if (!use_editors)
    return ;
  UpdateMenus (MenuHandle) ;
}

void EditSetFocus (void)
{
  if (!use_editors)
    return ;
  SetTabFocus () ;
}

HMENU EditGetMenu (int which)
{
  if (!use_editors)
    return (NULL) ;
  return (GetMenuHandle (which)) ;
}

void EditSetNotifyBase (HWND WindowHandle, int MessageBase)
{
  if (!use_editors)
    return ;
  SetNotifyBase (WindowHandle, MessageBase) ;
}

void EditContextHelp (void)
{
  if (!use_editors)
    return ;
  GetContextHelp () ;
}

bool EditShowParseError (const char *FileName, const char *Message, int Line, int Col)
{
  if (!use_editors)
    return (false) ;
  return (ShowParseError (FileName, Message, Line, Col)) ;
}

bool EditPassOnMessage (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, DWORD *rVal)
{
  if (!use_editors)
    return (false) ;
  return (PassOnMessage (hwnd, message, wParam, lParam, rVal)) ;
}

void EditUnload (void)
{
}

bool EditEnabled (void)
{
  return (use_editors) ;
}

void EditSetState (bool on)
{
  use_editors = on ;
}

void EditSetKeywords (LPCSTR SDLKeywordList, LPCSTR INIKeywordList)
{
  SetKeywords (SDLKeywordList, INIKeywordList) ;
}

const char **EditGetWindowList (void)
{
  if (!use_editors)
    return NULL;
  return GetWindowList();
}

}
// end of namespace povwin
