//******************************************************************************
///
/// @file windows/cmedit/ccodemax.cpp
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

bool                    CCodeMax::SaveDialogActive ;
static char             LanguageNames [] [32] = {"", "C/C++", "Basic", "Java", "Pascal", "SQL", "POV-Ray", "HTML", "XML", "INI"} ;

extern int              AutoReload ;
extern int              NotifyBase ;
extern bool             CreateBackups ;
extern bool             LastOverwrite ;
extern bool             UndoAfterSave ;
extern bool             MessagePaneVisible ;
extern HWND             hMainWindow ;
extern HWND             hTabWindow ;
extern HWND             hNotifyWindow ;
extern HMENU            hPopupMenu ;
extern HMENU            hInsertMenu ;
extern HINSTANCE        hInstance ;
extern CCodeMax         *Editor ;
extern CStdString       BinariesPath ;
extern CStdString       DocumentsPath;
extern CStdString       InitialDir ;
extern CStdString       IncludeFilename ;
extern CStdString       CommandLine ;

typedef struct
{
  WORD          cmd ;
  CodemaxHotkey key ;
} DefHotkeyRec ;

DefHotkeyRec DefHotKeys [] =
{
  { CM_SAVE,                          HOTKEYF_CONTROL,                    'S',        0, 0 },
  { CM_SAVEAS,                        HOTKEYF_CONTROL | HOTKEYF_SHIFT,    'A',        0, 0 },
  { CM_SAVEALL,                       HOTKEYF_CONTROL | HOTKEYF_SHIFT,    'V',        0, 0 },
  { CM_EXIT,                          HOTKEYF_ALT,                        'X',        0, 0 },
  { CM_SHOWMESSAGES,                  HOTKEYF_ALT,                        'M',        0, 0 },
  { CM_NEWFILE,                       HOTKEYF_CONTROL,                    'N',        0, 0 },
  { CM_OPENFILE,                      HOTKEYF_CONTROL,                    'O',        0, 0 },
  { CM_CLOSECURRENTFILE,              HOTKEYF_CONTROL,                    'W',        0, 0 },
  { CM_PRINT,                         HOTKEYF_CONTROL,                    'P',        0, 0 },
  { 0,                                0,                                  '\0',       0, 0 },
} ;

CCodeMax::CCodeMax (HWND parent)
{
  RECT        rect ;

  m_Opened = false ;
  m_Language = tlNone ;
  m_Index = 0 ;
  m_BackedUp = false ;
  m_LButtonDown = false ;
  m_RMBDownX = m_RMBDownY = 0 ;
  m_RMBDownLine = m_RMBDownCol = -1 ;
  m_Tag.LongName [0] = '\0' ;
  m_Tag.ShortName [0] = '\0' ;

  GetClientRect (parent, &rect) ;
  TabCtrl_AdjustRect (parent, FALSE, &rect) ;
  m_hWnd = CreateWindowEx (0,
                           "CodeMax",
                           "",
                           WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                           rect.left,
                           rect.top,
                           rect.right - rect.left + 1,
                           rect.bottom - rect.top + 1,
                           parent,
                           (HMENU) this,
                           hInstance,
                           NULL) ;
  m_OldWndProc = (WNDPROC) SetWindowLongPtr (m_hWnd, GWLP_WNDPROC, (LONG_PTR) StaticWndProc) ;
}

CCodeMax::~CCodeMax ()
{
  if (m_hWnd)
    DestroyWindow (m_hWnd) ;
}

LRESULT CCodeMax::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
{
  POINT                 point ;
  CodemaxRange          range ;
  CodemaxPos            pos ;

  if (message == WM_LBUTTONDOWN)
  {
    m_LButtonDown = true ;
    LRESULT result = CallWindowProc (m_OldWndProc, m_hWnd, message, wParam, lParam) ;
    if (GetSel (&range, false) == CODEMAX_SUCCESS)
    {
      int len = GetLineLength (range.Start.Line + 1) ;
      if (range.Start.Col > len)
        if (!IsMenuItemChecked (CM_CURSORBEYONDEOL))
          SetCaretPos (range.Start.Line + 1, len + 1) ;
    }
    return (result) ;
  }
  if (message == WM_LBUTTONUP)
  {
    LRESULT result = CallWindowProc (m_OldWndProc, m_hWnd, message, wParam, lParam) ;
    m_LButtonDown = false ;

    if (GetSel (&range, false) == CODEMAX_SUCCESS)
    {
      int len = GetLineLength (range.End.Line + 1) ;
      if (range.End.Col > len && !IsMenuItemChecked (CM_CURSORBEYONDEOL))
      {
        range.End.Col = len ;
        SetSel (&range, false) ;
      }
    }
    return (result) ;
  }
  if (message == WM_RBUTTONDOWN)
  {
    m_RMBDownX = LOWORD (lParam) ;
    m_RMBDownY = HIWORD (lParam) ;
    if (GetSelFromPoint (m_RMBDownX, m_RMBDownY, &pos) == CODEMAX_SUCCESS)
    {
      m_RMBDownLine = pos.Line + 1 ;
      m_RMBDownCol = pos.Col + 1 ;
    }
    else
      m_RMBDownLine = m_RMBDownCol = -1 ;
    // don't pass it on (so the caret doesn't move)
    return (0) ;
  }
  if (message == WM_CONTEXTMENU)
  {
    if (Editor != this)
      return (0) ;
    GetCursorPos (&point) ;
    PopupMenuPopup () ;
    TrackPopupMenuEx (hPopupMenu, 0, point.x, point.y, hMainWindow, NULL) ;
    m_RMBDownLine = m_RMBDownCol = -1 ;
    return (0) ;
  }
  if (message == WM_CHAR && MessagePaneVisible && (char) wParam == 0x1b)
  {
    MessagePaneVisible = false ;
    ShowMessagePane () ;
    return (0) ;
  }
  if (message == WM_ERASEBKGND)
    return (0) ;
  return (CallWindowProc (m_OldWndProc, m_hWnd, message, wParam, lParam)) ;
}

LRESULT CALLBACK CCodeMax::StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  try
  {
    CCodeMax *e = (CCodeMax *) GetWindowLongPtr (hWnd, GWLP_ID) ;
    ASSERT (hWnd == e->m_hWnd) ;
    if (hWnd != e->m_hWnd)
      return DefWindowProc (hWnd, message, wParam, lParam);
    return (e->WndProc (message, wParam, lParam)) ;
  }
  catch (...)
  {
    ShowMessage ("ERROR: The editor code threw an exception handling message %08lx. Please report this as a bug.", message) ;
    return DefWindowProc (hWnd, message, wParam, lParam);
  }
}

void CCodeMax::SetFileName (LPCTSTR Filename)
{
  if (m_Tag.LongName != NULL)
    if (strcmp (Filename, m_Tag.LongName) == 0)
      return ;
  strcpy (m_Tag.LongName, Filename) ;
  m_Opened = false ;
}

void CCodeMax::SetOpened (bool IsOpened)
{
  if (!IsOpened)
  {
    m_Opened = false ;
    return ;
  }
  m_Opened = OpenFile (m_Tag.LongName) != 0 ;
}

LPCTSTR CCodeMax::GetLanguageName (void)
{
  GetLanguage (m_LanguageName) ;
  for (int i = 0 ; i < sizeof (LanguageNames) / sizeof (LanguageNames [0]) ; i++)
    if (_stricmp (m_LanguageName, LanguageNames [i]) == 0)
      m_Language = TLanguage (i) ;
  return (m_LanguageName) ;
}

TLanguage CCodeMax::GetLanguage (void)
{
  GetLanguageName () ;
  return (m_Language) ;
}

TScrollStyle CCodeMax::GetScrollBars (void)
{
  TScrollStyle          result = tssNone ;

  if (HasScrollBar (true))
    result = tssHorizontal ;
  if (HasScrollBar (false))
    result = result == tssNone ? tssVertical : tssBoth ;
  return (result) ;
}

void CCodeMax::SetScrollBars (TScrollStyle style)
{
  switch (style)
  {
    case tssNone :
         ShowScrollbar (false, false) ;
         ShowScrollbar (true, false) ;
         break ;

    case tssHorizontal :
         ShowScrollbar (false, false) ;
         ShowScrollbar (true, true) ;
         break ;

    case tssVertical :
         ShowScrollbar (false, true) ;
         ShowScrollbar (true, false) ;
         break ;

    case tssBoth :
         ShowScrollbar (false, true) ;
         ShowScrollbar (true, true) ;
         break ;
  }
}

CStdString CCodeMax::GetText (const CodemaxRange *pRange)
{
  int         len = GetTextLength (pRange) ;
  CStdString  result ;

  if (len != -1)
  {
    char *buffer = (char *) malloc (len + 1) ;
    GetText (buffer, pRange) ;
    result = buffer ;
    free (buffer) ;
    return (result) ;
  }
  return ("") ;
}

CStdString CCodeMax::GetLine (int nLine)
{
  int         len = GetLineLength (nLine) ;
  CStdString  result ;

  if (len != -1)
  {
    char *buffer = (char *) malloc (len + 1) ;
    GetLine (nLine, buffer) ;
    result = buffer ;
    free (buffer) ;
    return (result) ;
  }
  return ("(error getting line)") ;
}

CStdString CCodeMax::GetWord (CodemaxPos *pPos)
{
  int         len = GetWordLength (pPos) ;
  CStdString  result ;

  if (len != -1)
  {
    char *buffer = (char *) malloc (len + 1) ;
    GetWord (buffer, pPos) ;
    result = buffer ;
    free (buffer) ;
    return (result) ;
  }
  return ("(error getting word)") ;
}

void CCodeMax::SetDefaultHotKeys (void)
{
  // we do it this way to avoid nuking any identical hotkeys
  // that may have been added by the user.
  for (DefHotkeyRec *p = DefHotKeys ; p->cmd ; p++)
    if (LookupHotKey (&p->key) == -1)
      RegisterHotKey (&p->key, p->cmd) ;
}

LRESULT CCodeMax::SetHotKeys (char *HotKeys)
{
  return (CMSetHotKeys ((unsigned char *) HotKeys)) ;
}

LRESULT CCodeMax::SetMacro (int Index, char *Macro)
{
  return (CMSetMacro (Index, (unsigned char *) Macro)) ;
}

void CCodeMax::SetFindReplaceMRUList (LPCTSTR List, bool IsFind)
{
  CMSetFindReplaceMRUList (List, IsFind) ;
}

int CCodeMax::GetHotKeys (char *HotKeys)
{
  return (CMGetHotKeys ((unsigned char *) HotKeys)) ;
}

int CCodeMax::GetMacro (int Index, char *Macro)
{
  return (CMGetMacro (Index, (unsigned char *) Macro)) ;
}

CStdString CCodeMax::GetFindReplaceMRUList (bool IsFind)
{
  char        buffer [CODEMAX_FIND_REPLACE_MRU_BUFF_SIZE] ;

  CMGetFindReplaceMRUList (buffer, IsFind) ;
  return (buffer) ;
}

int CCodeMax::GetLineNo (void)
{
  CodemaxRange              range ;

  GetSel (&range, false) ;
  return (++range.End.Line) ;
}

void CCodeMax::SetLineNo (int LineNo)
{
  CodemaxRange              range ;

  if (LineNo == 0)
    LineNo++ ;
  GetSel (&range, false) ;
  SetCaretPos (LineNo, range.Start.Col + 1) ;
}

int CCodeMax::GetColNo (void)
{
  CodemaxRange              range ;

  GetSel (&range, false) ;
  return (++range.End.Col) ;
}

void CCodeMax::SetColNo (int ColNo)
{
  CodemaxRange              range ;

  if (ColNo == 0)
    ColNo++ ;
  GetSel (&range, false) ;
  SetCaretPos (range.Start.Line + 1, ColNo) ;
}

void CCodeMax::SetPosition (int LineNo, int ColNo)
{
  CodemaxRange              range ;

  if (LineNo == 0)
    LineNo++ ;
  if (ColNo == 0)
    ColNo++ ;
  range.Start.Line = --LineNo ;
  range.Start.Col = --ColNo ;
  range.End = range.Start ;
  SetSel (&range, true) ;
}

void CCodeMax::GetPosition (CodemaxPos *Position)
{
  CodemaxRange              range ;

  GetSel (&range, false) ;
  *Position = range.End ;
}

void CCodeMax::SetPosition (const CodemaxPos *Position)
{
  CodemaxRange              range ;

  range.Start = range.End = *Position ;
  SetSel (&range, true) ;
}

void CCodeMax::SetLanguage (TLanguage Language)
{
  strcpy (m_LanguageName, LanguageNames [Language]) ;
  SetLanguage (m_LanguageName) ;
}

//---------------------------------------------------------------------------
CStdString CCodeMax::GetHotKeyString (CodemaxHotkey &cmHotKey)
{
  char        str [256] = "" ;
  UINT        nVirtKey = cmHotKey.VirtKey1 ;
  BYTE        byModifiers = cmHotKey.Modifier1 ;
  BYTE        byOrigModifiers = byModifiers ;

  for (int i = 0 ; nVirtKey && (i < 2) ; i++)
  {
    if (i == 0 || (i != 0 && byModifiers != byOrigModifiers))
    {
      if ((byModifiers & HOTKEYF_CONTROL) == HOTKEYF_CONTROL)
        strcat (str, "Ctrl + ") ;
      if ((byModifiers & HOTKEYF_SHIFT) == HOTKEYF_SHIFT)
        strcat (str, "Shift + ") ;
      if ((byModifiers & HOTKEYF_ALT) == HOTKEYF_ALT)
        strcat (str, "Alt + ") ;
    }

    if (nVirtKey)
    {
      char    szTemp [2] ;
      LPTSTR  pszChar ;

      switch (nVirtKey)
      {
        case VK_NUMLOCK:        pszChar = "Lock";           break;
        case VK_BACK:           pszChar = "Backspace";      break;
        case VK_INSERT:         pszChar = "Insert";         break;
        case VK_DELETE:         pszChar = "Delete";         break;
        case VK_HOME:           pszChar = "Home";           break;
        case VK_END:            pszChar = "End";            break;
        case VK_PRIOR:          pszChar = "Page Up";        break;
        case VK_NEXT:           pszChar = "Page Down";      break;
        case VK_LEFT:           pszChar = "Left";           break;
        case VK_RIGHT:          pszChar = "Right";          break;
        case VK_UP:             pszChar = "Up";             break;
        case VK_DOWN:           pszChar = "Down";           break;
        case VK_SCROLL:         pszChar = "Scroll Lock";    break;
        case VK_TAB:            pszChar = "Tab";            break;
        case VK_ESCAPE:         pszChar = "Esc";            break;
        case VK_RETURN:         pszChar = "Enter";          break;
        case VK_F1:             pszChar = "F1";             break;
        case VK_F2:             pszChar = "F2";             break;
        case VK_F3:             pszChar = "F3";             break;
        case VK_F4:             pszChar = "F4";             break;
        case VK_F5:             pszChar = "F5";             break;
        case VK_F6:             pszChar = "F6";             break;
        case VK_F7:             pszChar = "F7";             break;
        case VK_F8:             pszChar = "F8";             break;
        case VK_F9:             pszChar = "F9";             break;
        case VK_F10:            pszChar = "F10";            break;
        case VK_F11:            pszChar = "F11";            break;
        case VK_F12:            pszChar = "F12";            break;
        case VK_SPACE:          pszChar = "Space";          break;
        case VK_ADD:            pszChar = "Plus";           break;
        case 0x3b:              pszChar = "Plus";           break;
        case 0xbd:              pszChar = "Minus";          break;
        case VK_SUBTRACT:       pszChar = "Minus";          break;
        case 0x3d:              pszChar = "Minus";          break;

        default:
        {
          if (nVirtKey >= 0x60 && nVirtKey <= 0x6f)
            strcat (str, "Num ") ;
          switch (nVirtKey)
          {
            case 0xc0:          { nVirtKey = '`'; break; }
            case 0x30:          { nVirtKey = '0'; break; }
            case 0x31:          { nVirtKey = '1'; break; }
            case 0x32:          { nVirtKey = '2'; break; }
            case 0x33:          { nVirtKey = '3'; break; }
            case 0x34:          { nVirtKey = '4'; break; }
            case 0x35:          { nVirtKey = '5'; break; }
            case 0x36:          { nVirtKey = '6'; break; }
            case 0x37:          { nVirtKey = '7'; break; }
            case 0x38:          { nVirtKey = '8'; break; }
            case 0x39:          { nVirtKey = '9'; break; }
            case 0xbb:          { nVirtKey = '='; break; }
            case 0xdb:          { nVirtKey = '['; break; }
            case 0xdd:          { nVirtKey = ']'; break; }
            case 0xdc:          { nVirtKey = '\\'; break; }
            case 0xba:          { nVirtKey = ';'; break; }
            case 0xde:          { nVirtKey = '\''; break; }
            case 0xbc:          { nVirtKey = ','; break; }
            case 0xbe:          { nVirtKey = '.'; break; }
            case 0xbf:          { nVirtKey = '/'; break; }
            case VK_NUMPAD0:    { nVirtKey = '0'; break; }
            case VK_NUMPAD1:    { nVirtKey = '1'; break; }
            case VK_NUMPAD2:    { nVirtKey = '2'; break; }
            case VK_NUMPAD3:    { nVirtKey = '3'; break; }
            case VK_NUMPAD4:    { nVirtKey = '4'; break; }
            case VK_NUMPAD5:    { nVirtKey = '5'; break; }
            case VK_NUMPAD6:    { nVirtKey = '6'; break; }
            case VK_NUMPAD7:    { nVirtKey = '7'; break; }
            case VK_NUMPAD8:    { nVirtKey = '8'; break; }
            case VK_NUMPAD9:    { nVirtKey = '9'; break; }
            case VK_MULTIPLY:   { nVirtKey = '*'; break; }
            case VK_DECIMAL:    { nVirtKey = '.'; break; }
            case VK_DIVIDE :    { nVirtKey = '/'; break; }
          }
          szTemp [0] = (char) nVirtKey ;
          szTemp [1] = '\0' ;
          pszChar = szTemp ;
        }
      }

      strcat (str, pszChar) ;

      nVirtKey = cmHotKey.VirtKey2 ;
      byModifiers = cmHotKey.Modifiers2 ;

      if (nVirtKey && (i == 0))
        strcat (str, ", ") ;
    }
  }
  return (str) ;
}

//------------------------------------------------------------------------------------------------------------------------

void CCodeMax::SetLanguageBasedOnFileType (void)
{
  CStdString            ext = GetFileExt (m_Tag.ShortName) ;

  m_Language = tlNone ;
  ext.MakeUpper () ;
  if (ext == "C" || ext == "CPP" || ext == "H" || ext == "HPP"  || ext == "JS")
    m_Language = tlCCpp ;
  else if (ext == "JAVA")
    m_Language = tlJava ;
  else if (ext == "BAS")
    m_Language = tlBasic ;
  else if (ext == "PAS")
    m_Language = tlPascal ;
  else if (ext == "SQL" || ext == "DDL")
    m_Language = tlSQL ;
  else if (ext == "HTML" || ext == "HTM" || ext == "PHP" || ext == "PHP3" || ext == "PHP4"  || ext == "PHP5"  || ext == "ASP")
    m_Language = tlHTML ;
  else if (ext == "SGML" || ext == "XML")
    m_Language = tlSGML ;
  else if (ext == "POV" || ext == "INC" || ext == "MAC" || ext == "MCR")
    m_Language = tlPOVRay ;
  else if (ext == "INI")
    m_Language = tlINI ;
  SetLanguage (m_Language) ;
}

//------------------------------------------------------------------------------------------------------------------------

void CCodeMax::SetupEditor (const EditConfigStruct *ec, bool PropsChange, bool InitCM)
{
  //if (!PropsChange)
  //{
    SetAutoIndent (ec->AutoIndent) ;
    SetTabSize (ec->TabSize) ;
  //}
  if (ec->HFont != NULL)
    SetFont (ec->HFont) ;
  SetFontOwnership (false) ;
  EnableTabExpand (ec->TabExpand) ;
  EnableColorSyntax (ec->SyntaxHighlighting) ;
  EnableWhitespaceDisplay (ec->WhiteSpaceDisplay) ;
  EnableSmoothScrolling (ec->SmoothScrolling) ;
  EnableLineToolTips (ec->LineToolTips) ;
  EnableLeftMargin (ec->LeftMarginVisible) ;
  EnableCaseSensitive (ec->CaseSensitive) ;
  EnablePreserveCase (ec->PreserveCase) ;
  EnableWholeWord (ec->WholeWordEnabled) ;
  EnableDragDrop (ec->DragDropEnabled) ;
  EnableColumnSel (ec->ColumnSelEnabled) ;
  EnableRegExp (ec->RegexpEnabled) ;
  EnableOvertypeCaret (ec->OvertypeCaret) ;
  EnableSelBounds (ec->SelBoundsEnabled) ;
  SetUndoLimit (ec->UndoLimit) ;

  // set scroll bars before splitters
  SetScrollBars (ec->ScrollBars) ;

  // splitters must be set up after scroll bars
  EnableHSplitter (ec->HSplitterEnabled) ;
  EnableVSplitter (ec->VSplitterEnabled) ;

  SetColors (&ec->Colours) ;
  SetFontStyles (&ec->FontStyles) ;
  if (InitCM)
  {
    RegisterCommand (CMD_NEXT_KEYWORD, "ExpandNextKeyword", "Expands the next keyword in the keyword list") ;
    RegisterCommand (CMD_PREV_KEYWORD, "ExpandPrevKeyword", "Expands the previous keyword in the keyword list") ;
    if (ec->HotKeyLen > 0 && ec->HotKeys != NULL)
      SetHotKeys (ec->HotKeys) ;
    SetDefaultHotKeys () ;
    SetFindReplaceMRUList (ec->FindMRUList, true) ;
    SetFindReplaceMRUList (ec->ReplaceMRUList, false) ;
    for (int i = 0 ; i < CODEMAX_MACRO_LIMIT ; i++)
      if (ec->Macros [i] != NULL)
        SetMacro (i, ec->Macros [i]) ;
  }
}

//------------------------------------------------------------------------------------------------------------------------

void CCodeMax::GetConfigFromInstance (EditConfigStruct *ec)
{
  ec->AutoIndent = GetAutoIndent () ;
  ec->SyntaxHighlighting = IsColorSyntaxEnabled () ;
  ec->WhiteSpaceDisplay = IsWhitespaceDisplayEnabled () ;
  ec->TabExpand = IsTabExpandEnabled () ;
  ec->LeftMarginVisible = IsLeftMarginEnabled () ;
  ec->SmoothScrolling = IsSmoothScrollingEnabled () ;
  ec->LineToolTips = IsLineToolTipsEnabled () ;
  ec->CaseSensitive = IsCaseSensitiveEnabled () ;
  ec->PreserveCase = IsPreserveCaseEnabled () ;
  ec->WholeWordEnabled = IsWholeWordEnabled () ;
  ec->DragDropEnabled = IsDragDropEnabled () ;
  ec->HSplitterEnabled = IsHSplitterEnabled () ;
  ec->VSplitterEnabled = IsVSplitterEnabled () ;
  ec->ColumnSelEnabled = IsColumnSelEnabled () ;
  ec->RegexpEnabled = IsRegExpEnabled () ;
  ec->OvertypeCaret = IsOvertypeCaretEnabled () ;
  ec->SelBoundsEnabled = IsSelBoundsEnabled () ;
  ec->ScrollBars = GetScrollBars () ;
  ec->TabSize = GetTabSize () ;
  ec->UndoLimit = GetUndoLimit () ;
  ec->HFont = GetFont () ;
  GetColors (&ec->Colours) ;
  GetFontStyles (&ec->FontStyles) ;
}

//------------------------------------------------------------------------------------------------------------------------

void CCodeMax::GetConfigFromCommonSettings (EditConfigStruct *ec)
{
  ec->FindMRUList = CCodeMax::GetFindReplaceMRUList (true) ;
  ec->ReplaceMRUList = CCodeMax::GetFindReplaceMRUList (false) ;
  if (ec->HotKeys != NULL)
    free (ec->HotKeys) ;
  ec->HotKeys = NULL ;
  if ((ec->HotKeyLen = CCodeMax::GetHotKeys (NULL)) > 0)
  {
    ec->HotKeys = (char *) malloc (ec->HotKeyLen) ;
    if ((ec->HotKeyLen = CCodeMax::GetHotKeys (ec->HotKeys)) == 0)
    {
      free (ec->HotKeys) ;
      ec->HotKeys = NULL ;
      PutStatusMessage ("Failed to get key bindings from Editor DLL") ;
    }
  }
  for (int i = 0 ; i < CODEMAX_MACRO_LIMIT ; i++)
  {
    free (ec->Macros [i]) ;
    ec->Macros [i] = NULL ;
    if ((ec->MacroLen [i] = CCodeMax::GetMacro (i, NULL)) <= 0)
      continue ;
    ec->Macros [i] = (char *) malloc (ec->MacroLen [i]) ;
    CCodeMax::GetMacro (i, ec->Macros [i]) ;
  }
}

//------------------------------------------------------------------------------------------------------------------------

bool CCodeMax::AskFileName (EditTagStruct *t)
{
  char                  szFile [_MAX_PATH] = "" ;
  CStdString            str ;
  CStdString            ext ;
  OPENFILENAME          ofn ;
  EditTagStruct         tag ;

  memset(&tag, 0, sizeof(tag));
  if (t == NULL)
    t = &m_Tag ;
  str = GetFilePath (t->LongName) ;
  if (str == "")
    str = InitialDir ;

  ZeroMemory (&ofn, sizeof (OPENFILENAME)) ;
  ofn.lStructSize = sizeof (OPENFILENAME) ;
  if (t->LongName [0] != 0)
    strcpy (szFile, t->ShortName) ;
  ofn.hwndOwner = hMainWindow ;
  ofn.lpstrFile = szFile ;
  ofn.nMaxFile = sizeof (szFile) ;
  ofn.lpstrFilter = "POV-Ray Files (*.pov;*.inc;*.ini;*.mac;*.mcr)\0*.pov;*.inc;*.ini;*.mac;*.mcr\0"
                    "POV-Ray Source (*.pov)\0*.pov\0"
                    "Include Files (*.inc;*.mac;*.mcr)\0*.inc;*.mac;*.mcr\0"
                    "INI files (*.ini)\0*.ini\0"
                    "Text Files (*.txt)\0*.txt\0"
                    "All Files (*.*)\0*.*\0" ;

  ext = GetFileExt (t->ShortName) ;
  ext.MakeUpper () ;
  if (ext == "")
    ofn.nFilterIndex = 1 ;
  else if (ext == "POV")
    ofn.nFilterIndex = 2 ;
  else if (ext == "INC")
    ofn.nFilterIndex = 3 ;
  else if (ext == "INI")
    ofn.nFilterIndex = 4 ;
  else if (ext == "TXT")
    ofn.nFilterIndex = 5 ;
  else
    ofn.nFilterIndex = 6 ;

  ofn.lpstrFileTitle = NULL ;
  ofn.nMaxFileTitle = 0 ;
  ofn.lpstrInitialDir = str ;
  ofn.lpstrDefExt = NULL ;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT ;

  SaveDialogActive = true ;
  while (true)
  {
    if (GetSaveFileName (&ofn) != 0)
    {
      if (szFile [strlen (szFile) - 1] != '.')
      {
        if (GetFileExt (szFile) == "")
        {
          switch (ofn.nFilterIndex)
          {
            case 1 :
            case 2 :
                strcat (szFile, ".pov") ;
                break ;

            case 3 :
                strcat (szFile, ".inc") ;
                break ;

            case 4 :
                strcat (szFile, ".ini") ;
                break ;

            case 5 :
                strcat (szFile, ".txt") ;
                break ;

            case 6 :
                break ;
          }
        }
      }
      else
        szFile [strlen (szFile) - 1] = '\0' ;
      MakeFileNames (&tag, szFile) ;
      if (_stricmp(tag.LongName, t->LongName) != 0 && FindEditor (tag.LongName) != NULL)
      {
        ShowMessage ("This file is open in another editor tab") ;
        continue ;
      }
      *t = tag ;
      InitialDir = szFile ;
      if (!ofn.nFileOffset || szFile [ofn.nFileOffset - 1])
        InitialDir = GetFilePath (InitialDir) ;
      SaveDialogActive = false ;
      return (true) ;
    }
    SaveDialogActive = false ;
    return (false) ;
  }
  return (false) ;
}

//------------------------------------------------------------------------------------------------------------------------

TSaveType CCodeMax::SaveEditorFile (void)
{
  char                  str [_MAX_PATH + 5] ;
  char                  oldName [_MAX_PATH] ;
  TCITEM                item ;
  EditTagStruct         tag = m_Tag ;

  PutStatusMessage ("") ;
  ClearErrorLine () ;
  strcpy (oldName, m_Tag.LongName) ;
  if (m_Tag.LongName [0] == '\0')
    if (!AskFileName (&tag))
      return (stCancel) ;
  if (CreateBackups && !m_BackedUp)
  {
    if (FileExists (tag.LongName))
    {
      if ((GetFileAttributes (tag.LongName) & FILE_ATTRIBUTE_READONLY) != 0)
      {
        MessageBox (hMainWindow, "Cannot overwrite read-only file", tag.ShortName, MB_ICONEXCLAMATION | MB_OK) ;
        return (stError) ;
      }
      sprintf (str, "%s.bak", tag.LongName) ;
      if (FileExists (str) && !DeleteFile (str))
      {
        ShowErrorMessage (GetBaseName (str), "Failed to delete old backup file") ;
        return (stError) ;
      }
      if (!MoveFile (tag.LongName, str))
      {
        ShowErrorMessage (tag.ShortName, "Failed to rename original file to .bak") ;
        return (stError) ;
      }
      m_BackedUp = true ;
    }
  }
  if (SaveFile (tag.LongName, !UndoAfterSave) == CODEMAX_SUCCESS)
  {
    if (strcmp (tag.LongName, oldName) != 0)
    {
      m_Tag = tag ;
      item.mask = TCIF_TEXT ;
      item.dwState = item.dwStateMask = 0 ;
      item.pszText = tag.ShortName ;
      TabCtrl_SetItem (hTabWindow, m_Index + 1, &item) ;
      if (_stricmp (tag.LongName, oldName) != 0)
      {
        AddToRecent (EncodeFilename(tag.LongName)) ;
        SetLanguageBasedOnFileType () ;
        UpdateRecent () ;

        // pretend the tab has changed so the window caption is updated
        SendMessage (hNotifyWindow, WM_COMMAND, NotifyBase + povwin::NotifyTabChange, GetFlags ()) ;
      }
    }
    UpdateFileTime () ;
    PutStatusMessage ("File saved") ;
    return (stSaved) ;
  }
  ShowErrorMessage (m_Tag.ShortName, "Failed to save file") ;
  return (stError) ;
}

//------------------------------------------------------------------------------------------------------------------------

TSaveType CCodeMax::TrySave (bool ContinueOption)
{
  int         flags = MB_ICONEXCLAMATION | MB_DEFBUTTON1 ;
  char        str [_MAX_FNAME + 64] ;

  switch (SaveEditorFile ())
  {
    case stSaved :
         return (stSaved) ;

    case stCancel :
         return (stCancel) ;

    default :
         if (ContinueOption)
           flags |= HaveWin2kOrLater () ? MB_CANCELTRYCONTINUE : MB_ABORTRETRYIGNORE ;
         else
           flags |= MB_RETRYCANCEL ;
         sprintf (str, "Failed to save file '%s'", m_Tag.ShortName) ;
         switch (MessageBox (hMainWindow, str, "POV-Ray editor error", flags))
         {
           case IDABORT :
           case IDCANCEL :
                return (stCancel) ;

           case IDRETRY :
           case IDTRYAGAIN :
                return (stRetry) ;

           case IDIGNORE :
           case IDCONTINUE :
                return (stContinue) ;
         }
         return (stDiscard) ;
  }
}

//------------------------------------------------------------------------------------------------------------------------

void CCodeMax::UpdateFileTime (void)
{
  GetFileTimeFromDisk (m_Tag.LongName, m_Tag.TimeSaved) ;
}

// IsCodeUpToDate() will check to see the file on disk is newer than the window contents.
// If Stale is TRUE, then the code is not up to date if the file on disk is newer.
// If Stale is FALSE, then the code is not up to date if the file on disk is newer or older
bool CCodeMax::IsCodeUpToDate (bool Stale)
{
  int                   compare ;
  bool                  upToDate = true ;
  FILETIME              time ;

  if (m_Tag.LongName [0] != '\0')
  {
    GetFileTimeFromDisk (m_Tag.LongName, time) ;
    compare = CompareFileTime (&time, &m_Tag.TimeSaved) ;
    upToDate = Stale ? (compare <= 0) : (compare == 0) ;
  }
  return (upToDate) ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString CCodeMax::GetCurrentKeyword (void)
{
  int         col = GetColNo () - 1 ;
  CStdString  ln = GetLine (GetLineNo ()) ;
  const char  *line = ln ;
  const char  *s = line + col ;

  // special case - if the cursor is immediately after the end of a word,
  // and it's on whitespace or a non-alpha, then we go back one column
  if (col > 0 && !isalpha (*s) && *s != '_')
    if (isalnum (line [col - 1]) || line [col - 1] == '_' || line [col - 1] == '#')
      s-- ;

  // backtrack to the start of the word
  while ((isalnum (*s) || *s == '_' || *s == '#') && s > line)
    s-- ;

  // now we need to track forward
  while (*s && !isalnum (*s) && *s != '_' && *s != '#')
    s++ ;

  // return if the line is blank
  if (*s == '\0')
    return ("") ;

  // now find the end of the word
  const char *start = s ;
  while (*s && (isalnum (*s) || *s == '_' || *s == '#'))
    s++ ;
  return (ln.Mid (start - line, s - start)) ;
}

//------------------------------------------------------------------------------------------------------------------------

void CCodeMax::PopupMenuPopup (void)
{
  int                   line ;
  int                   col ;

  if (GetSubMenu (hPopupMenu, 0) != hInsertMenu)
    InsertMenu (hPopupMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT_PTR) hInsertMenu, "Insert") ;
  SetMenuState () ;
  SetMenuItemText (CM_OPENFILEUNDERCURSOR, "Open Include File/Insert Command Line") ;
  EnableMenuItem (CM_OPENFILEUNDERCURSOR, false) ;
  if ((line = m_RMBDownLine) == -1 || (col = m_RMBDownCol) == -1)
    return ;
  IncludeFilename = LocateIncludeFilename (line, col) ;
  if (IncludeFilename != "")
  {
    CStdString str = IncludeFilename ;
    if (str.length () > 32)
      str = str.Left (29) + "..." ;
    SetMenuItemText (CM_OPENFILEUNDERCURSOR, CStdString ("Open \"") + str + "\"") ;
    EnableMenuItem (CM_OPENFILEUNDERCURSOR, true) ;
  }
  else
  {
    CommandLine = LocateCommandLine (line) ;
    if (CommandLine != "")
    {
      CStdString str = CommandLine ;
      if (str.length () > 32)
        str = str.Left (29) + "..." ;
      SetMenuItemText (CM_OPENFILEUNDERCURSOR, CStdString ("Cop&y \"") + str + "\" to Command-Line") ;
      EnableMenuItem (CM_OPENFILEUNDERCURSOR, true) ;
    }
  }
}

//------------------------------------------------------------------------------------------------------------------------

CStdString CCodeMax::LocateIncludeFilename (int line, int col)
{
  int                   pos ;
  int                   len ;
  CStdString            str = "" ;
  CStdString            name ;

  // N.B. line and col as passed in are 1-based
  if (line <= 0 || col <= 0)
    return (str) ;
  str = GetLine (line) ;
  if ((len = (int) str.length ()) == 0)
    return (str) ;
  col-- ;
  if (col >= len)
    col = len - 1 ;
  if ((pos = str.Find ('"')) >= col)
    col = pos + 1 ;
  for (pos = col ; pos < len ; pos++)
    if (str [pos] == '"')
      break ;
  if (pos >= len)
    return ("") ;
  str = str.Left (pos) ;
  for (pos = pos - 1 ; pos > 0 ; pos--)
    if (str [pos] == '"')
      break ;
  if (pos == 0)
    return ("") ;
  return (str.Mid (pos + 1)) ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString CCodeMax::LocateCommandLine (int line)
{
  int                   pos ;
  CStdString            str ;
  CStdString            tmpstr ;

  // N.B. line and col as passed in are 1-based
  if (line <= 0)
    return ("") ;
  str = GetLine (line) ;
  if (str.length () < 3)
    return ("") ;
  if ((pos = str.Find ("//")) == -1)
    return ("") ;
  str.Delete (0, pos + 2) ;
  tmpstr = str = str.Trim () ;
  tmpstr.MakeLower () ;
  if ((pos = tmpstr.Find ("cmd:")) != -1)
  {
    str.Delete (0, pos + 4) ;
    return (str.Trim ()) ;
  }
  if (str [0] != '+' && str [0] != '-')
    return ("") ;
  if (!isalpha (str [1]))
    return ("") ;
  return (str) ;
}

