//******************************************************************************
///
/// @file windows/cmedit/settings.cpp
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
#include "editorinterface.h"
#include "..\pvedit.h"

CodemaxColors DefaultColours =
{
  DEF_CLRWINDOW,
  DEF_CLRLEFTMARGIN,
  DEF_CLRBOOKMARK,
  DEF_CLRBOOKMARKBK,
  DEF_CLRTEXT,
  DEF_CLRTEXTBK,
  DEF_CLRNUMBER,
  DEF_CLRNUMBERBK,
  DEF_CLRKEYWORD,
  DEF_CLRKEYWORDBK,
  DEF_CLROPERATOR,
  DEF_CLROPERATORBK,
  DEF_CLRSCOPEKEYWORD,
  DEF_CLRSCOPEKEYWORDBK,
  DEF_CLRCOMMENT,
  DEF_CLRCOMMENTBK,
  DEF_CLRSTRING,
  DEF_CLRSTRINGBK,
  DEF_CLRTAGTEXT,
  DEF_CLRTAGTEXTBK,
  DEF_CLRTAGENT,
  DEF_CLRTAGENTBK,
  DEF_CLRTAGELEMNAME,
  DEF_CLRTAGELEMNAMEBK,
  DEF_CLRTAGATTRNAME,
  DEF_CLRTAGATTRNAMEBK,
  DEF_CLRLINENUMBER,
  DEF_CLRLINENUMBERBK,
  DEF_CLRHDIVIDERLINES,
  DEF_CLRVDIVIDERLINES,
  DEF_CLRHIGHLIGHTEDLINE
} ;

CodemaxColors CJCColours =
{
  CJC_CLRWINDOW,
  CJC_CLRLEFTMARGIN,
  CJC_CLRBOOKMARK,
  CJC_CLRBOOKMARKBK,
  CJC_CLRTEXT,
  CJC_CLRTEXTBK,
  CJC_CLRNUMBER,
  CJC_CLRNUMBERBK,
  CJC_CLRKEYWORD,
  CJC_CLRKEYWORDBK,
  CJC_CLROPERATOR,
  CJC_CLROPERATORBK,
  CJC_CLRSCOPEKEYWORD,
  CJC_CLRSCOPEKEYWORDBK,
  CJC_CLRCOMMENT,
  CJC_CLRCOMMENTBK,
  CJC_CLRSTRING,
  CJC_CLRSTRINGBK,
  CJC_CLRTAGTEXT,
  CJC_CLRTAGTEXTBK,
  CJC_CLRTAGENT,
  CJC_CLRTAGENTBK,
  CJC_CLRTAGELEMNAME,
  CJC_CLRTAGELEMNAMEBK,
  CJC_CLRTAGATTRNAME,
  CJC_CLRTAGATTRNAMEBK,
  CJC_CLRLINENUMBER,
  CJC_CLRLINENUMBERBK,
  CJC_CLRHDIVIDERLINES,
  CJC_CLRVDIVIDERLINES,
  CJC_CLRHIGHLIGHTEDLINE
} ;

CodemaxFontStyles DefaultFontStyles =
{
  DEF_FSTEXT,
  DEF_FSNUMBER,
  DEF_FSKEYWORD,
  DEF_FSOPERATOR,
  DEF_FSSCOPEKEYWORD,
  DEF_FSCOMMENT,
  DEF_FSSTRING,
  DEF_FSTAGTEXT,
  DEF_FSTAGENT,
  DEF_FSTAGELEMNAME,
  DEF_FSTAGATTRNAME,
  DEF_FSLINENUMBER
} ;

//------------------------------------------------------------------------------------------------------------------------

int                     AutoReload ;
int                     PrintPointSize ;
int                     PrintUseBorder ;
int                     PrintTopMargin ;
int                     PrintLeftMargin ;
int                     PrintBottomMargin ;
int                     PrintRightMargin ;
bool                    CreateBackups ;
bool                    LastOverwrite ;
bool                    UndoAfterSave ;
bool                    PrintUseColor ;
bool                    PrintUseFancyText ;
bool                    PrintUsePageNumbers ;
bool                    PrintUseDateTime ;
bool                    PrintUseFileName ;
bool                    PrintMarginUnitsMetric ;
EditConfigStruct        EditConfig ;

extern int              EditDragOffset ;
extern int              EditStartDragOffset ;
extern int              EditorCount ;
extern int              AutoSaveDelay ;
extern HWND             hMainWindow ;
extern HWND             hTabWindow ;
extern CCodeMax         *Editors [MAX_EDITORS] ;
extern CCodeMax         *Editor ;
extern CStdString       DocumentsPath;
extern CStdString       BinariesPath ;
extern CStdString       InitialDir ;
extern CStdStringList   RecentFiles ;
extern CStdStringList   OlderFiles ;

//------------------------------------------------------------------------------------------------------------------------

CRegDef::CRegDef (LPCSTR Path)
{
  m_Path = Path ;
  if (m_Path [0] == '\\')
    m_Path.Delete (0, 1) ;
  m_Result = RegCreateKeyEx (HKEY_CURRENT_USER,
                             m_Path,
                             0,
                             NULL,
                             REG_OPTION_NON_VOLATILE,
                             KEY_ALL_ACCESS,
                             NULL,
                             &m_hKey,
                             NULL) == ERROR_SUCCESS ;
}

CRegDef::~CRegDef ()
{
  if (m_Result == ERROR_SUCCESS)
    RegCloseKey (m_hKey) ;
}

bool CRegDef::DeleteValue (LPCSTR Name)
{
  return (RegDeleteValue (m_hKey, Name) == ERROR_SUCCESS) ;
}

int CRegDef::ReadInt (LPCSTR Name, int defval)
{
  int         data ;
  bool        ok ;
  DWORD       datalen = sizeof (int) ;
  DWORD       datatype ;

  ok = RegQueryValueEx (m_hKey,
                        Name,
                        NULL,
                        &datatype,
                        (unsigned char *) &data,
                        &datalen) == ERROR_SUCCESS ;
  if (!ok || datalen != sizeof (int) || datatype != REG_DWORD)
    return (defval) ;
  return (data) ;
}

bool CRegDef::ReadBool (LPCSTR Name, bool defval)
{
  int         data ;
  bool        ok ;
  DWORD       datalen = sizeof (int) ;
  DWORD       datatype ;

  ok = RegQueryValueEx (m_hKey,
                        Name,
                        NULL,
                        &datatype,
                        (unsigned char *) &data,
                        &datalen) == ERROR_SUCCESS ;
  if (!ok || datalen != sizeof (int) || datatype != REG_DWORD)
    return (defval) ;
  return (data != 0) ;
}

int CRegDef::ReadBin (LPCSTR Name, char *data, int len, char *defval)
{
  bool        ok ;
  DWORD       datalen ;
  DWORD       datatype ;

  if (defval != NULL)
    memcpy (data, defval, len) ;
  if (RegQueryValueEx (m_hKey, Name, NULL, NULL, NULL, &datalen) == ERROR_SUCCESS)
  {
    if ((len != 0 && len != datalen) || datalen == -1)
      return (defval == NULL ? 0 : len) ;
    ok = RegQueryValueEx (m_hKey,
                          Name,
                          NULL,
                          &datatype,
                          (unsigned char *) data,
                          &datalen) == ERROR_SUCCESS ;
    if (ok && datatype == REG_BINARY)
      return (datalen) ;
  }
  return (defval == NULL ? 0 : len) ;
}

int CRegDef::ReadBin (LPCSTR Name, char **data, int len, char *defval)
{
  bool        ok ;
  DWORD       datalen ;
  DWORD       datatype ;

  *data = NULL ;
  if (RegQueryValueEx (m_hKey, Name, NULL, NULL, NULL, &datalen) != ERROR_SUCCESS || datalen == -1)
  {
    if (defval == NULL)
      return (0) ;
    *data = (char *) malloc (len) ;
    memcpy (*data, defval, len) ;
    return (len) ;
  }
  *data = (char *) malloc (datalen) ;
  ok = RegQueryValueEx (m_hKey,
                        Name,
                        NULL,
                        &datatype,
                        (unsigned char *) *data,
                        &datalen) == ERROR_SUCCESS ;
  if (ok && datatype == REG_BINARY)
    return (datalen) ;
  free (*data) ;
  *data = NULL ;
  if (defval == NULL)
    return (0) ;
  *data = (char *) malloc (len) ;
  memcpy (*data, defval, len) ;
  return (len) ;
}

LPCTSTR CRegDef::ReadString (LPCSTR Name, LPCTSTR defval)
{
  bool        ok ;
  DWORD       datalen ;
  DWORD       datatype ;
  static char buffer [2048] ;

  datalen = sizeof (buffer) ;
  ok = RegQueryValueEx (m_hKey,
                        Name,
                        NULL,
                        &datatype,
                        (unsigned char *) buffer,
                        &datalen) == ERROR_SUCCESS ;
  if (!ok || datatype != REG_SZ)
  {
    buffer [0] = '\0' ;
    if (defval != NULL)
      strcpy (buffer, defval) ;
  }
  return (buffer) ;
}

bool CRegDef::WriteInt (LPCSTR Name, int val)
{
  return (RegSetValueEx (m_hKey, Name, 0, REG_DWORD, (unsigned char *) &val, sizeof (int)) == ERROR_SUCCESS) ;
}

bool CRegDef::WriteBool (LPCSTR Name, bool val)
{
  int         data = val ? 1 : 0 ;

  return (RegSetValueEx (m_hKey, Name, 0, REG_DWORD, (unsigned char *) &data, sizeof (int)) == ERROR_SUCCESS) ;
}

bool CRegDef::WriteBin (LPCSTR Name, char *data, int len)
{
  return (RegSetValueEx (m_hKey, Name, 0, REG_BINARY, (unsigned char *) data, len) == ERROR_SUCCESS) ;
}

bool CRegDef::WriteString (LPCSTR Name, LPCTSTR data)
{
  return (RegSetValueEx (m_hKey, Name, 0, REG_SZ, (unsigned char *) data, (DWORD) strlen (data) + 1) == ERROR_SUCCESS) ;
}

//------------------------------------------------------------------------------------------------------------------------

CStdString DecodeFilename(CStdString str)
{
  CStdString    result(str);

  // when we save the settings, if the filename contains a comma, we replace
  // it with \x2c, since otherwise it will be counted as a delimiter when read.
  // we do a literal replace of these here (we do not support generic hex values).
  result.Replace("\\x2c", ",");
  return result;
}

CStdString EncodeFilename(CStdString str)
{
  CStdString    result(str);

  // when we save the settings, if the filename contains a comma, we replace
  // it with \x2c, since otherwise it will be counted as a delimiter when read.
  result.Replace(",", "\\x2c");
  return result;
}

CStdString GetNextField (CStdString& str)
{
  int                   pos = str.Find (",") ;
  CStdString            temp ;

  if (pos == -1)
  {
    temp = str ;
    str = "" ;
    return (temp) ;
  }
  temp = str.Left (pos) ;
  str.Delete (0, pos + 1) ;
  return (temp) ;
}

CStdString GetField (CStdString str, int FieldNo)
{
  CStdString            temp = str ;

  if (FieldNo == 0)
    return (DecodeFilename(GetNextField(temp)));
  while (FieldNo--)
    GetNextField (temp) ;
  return (GetNextField (temp)) ;
}

//------------------------------------------------------------------------------------------------------------------------

void MakeLogicalFont (HDC hdc, int size, char *facename, LOGFONT *lf)
{
  memset (lf, 0, sizeof (LOGFONT)) ;
  lf->lfHeight = -MulDiv (size, GetDeviceCaps (hdc, LOGPIXELSY), 72) ;
  lf->lfWeight = FW_NORMAL ;
  lf->lfPitchAndFamily = FIXED_PITCH | FF_MODERN ;
  lf->lfCharSet = DEFAULT_CHARSET ;
  lf->lfQuality = PROOF_QUALITY ;
  strncpy (lf->lfFaceName, facename, sizeof (lf->lfFaceName) - 1) ;
}

//------------------------------------------------------------------------------------------------------------------------

void GetSettings (CRegDef *t, EditConfigStruct *ec, bool RestoreFiles)
{
  char                  str [256] ;
  LOGFONT               lf ;
  CStdString            params ;
  CCodeMax              *e ;

  ec->AutoIndent = (TAutoIndent) t->ReadInt ("AutoIndent", CODEMAX_INDENT_PREVLINE) ;
  ec->SyntaxHighlighting = t->ReadBool ("SyntaxHighlighting", true) ;
  ec->WhiteSpaceDisplay = t->ReadBool ("WhiteSpaceDisplay", false) ;
  ec->TabExpand = t->ReadBool ("TabExpand", true) ;
  ec->SmoothScrolling = t->ReadBool ("SmoothScrolling", false) ;
  ec->LineToolTips = t->ReadBool ("LineToolTips", true) ;
  ec->LeftMarginVisible = t->ReadBool ("LeftMarginVisible", true) ;
  ec->CaseSensitive = t->ReadBool ("CaseSensitive", false) ;
  ec->PreserveCase = t->ReadBool ("PreserveCase", true) ;
  ec->WholeWordEnabled = t->ReadBool ("WholeWordEnabled", false) ;
  ec->DragDropEnabled = t->ReadBool ("DragDropEnabled", true) ;
  ec->HSplitterEnabled = t->ReadBool ("HSplitterEnabled", true) ;
  ec->VSplitterEnabled = t->ReadBool ("VSplitterEnabled", true) ;
  ec->ColumnSelEnabled = t->ReadBool ("ColumnSelEnabled", true) ;
  ec->RegexpEnabled = t->ReadBool ("RegexpEnabled", false) ;
  ec->OvertypeCaret = t->ReadBool ("OvertypeCaret", true) ;
  ec->SelBoundsEnabled = t->ReadBool ("SelBoundsEnabled", false) ;
  ec->TabKeywordExpansion = t->ReadBool ("TabKeywordExpansion", true) ;
  ec->ScrollBars = (TScrollStyle) t->ReadInt ("ScrollBars", tssBoth) ;
  ec->TabSize = t->ReadInt ("TabSize", 4) ;
  ec->UndoLimit = t->ReadInt ("UndoLimit", -1) ;
  t->ReadBin ("Colours", (char *) &ec->Colours, sizeof (CodemaxColors), (char *) &DefaultColours) ;
  t->ReadBin ("FontStyles", (char *) &ec->FontStyles, sizeof (CodemaxFontStyles), (char *) &DefaultFontStyles) ;
  ec->HotKeyLen = t->ReadBin ("KeyBindings", &ec->HotKeys, 0, NULL) ;

  // sanity-check the key bindings
  if (ec->HotKeyLen > 0 && ec->HotKeyLen < 512)
  {
    ec->HotKeyLen = 0 ;
    free (ec->HotKeys) ;
    ec->HotKeys = NULL ;
    ShowMessage ("Warning: Did not restore editor key bindings (possible corruption.)") ;
  }

  ec->FindMRUList = t->ReadString ("FindMRUList", "") ;
  if (ec->FindMRUList == "<empty>")
    ec->FindMRUList = "" ;
  ec->ReplaceMRUList = t->ReadString ("ReplaceMRUList", "") ;
  if (ec->ReplaceMRUList == "<empty>")
    ec->ReplaceMRUList = "" ;

  ec->HFont = NULL ;
  if (t->ReadBin ("Font", (char *) &lf, 0, NULL) == sizeof (lf))
  {
    lf.lfQuality = PROOF_QUALITY ;
    ec->HFont = CreateFontIndirect (&lf) ;
  }
  if (ec->HFont == NULL)
  {
    HDC hDC = GetDC (NULL) ;
    MakeLogicalFont (hDC, 8, "Lucida Console", &lf) ;
    ec->HFont = CreateFontIndirect (&lf) ;
    if (ec->HFont == NULL)
    {
      MakeLogicalFont (hDC, 8, "Courier New", &lf) ;
      ec->HFont = CreateFontIndirect (&lf) ;
    }
    if (ec->HFont == NULL)
    {
      MakeLogicalFont (hDC, 11, "Fixedsys", &lf) ;
      ec->HFont = CreateFontIndirect (&lf) ;
    }
    if (ec->HFont == NULL)
      ShowMessage ("Warning: could not create editor font. Set font manually from editor properties menu.") ;
    ReleaseDC (NULL, hDC) ;
  }

  UndoAfterSave = t->ReadBool ("UndoAfterSave", true) ;
  AutoReload = t->ReadInt ("AutoReload", 1) ;
  CreateBackups = t->ReadBool ("CreateBackups", true) ;
  EditStartDragOffset = EditDragOffset = t->ReadInt ("SmallMessageWindowSize", 96) ;

  CRegDef *macroReg = new CRegDef (t->m_Path + "\\Macro") ;
  for (int i = 0 ; i < CODEMAX_MACRO_LIMIT ; i++)
  {
    sprintf (str, "Macro%d", i) ;
    ec->MacroLen [i] = macroReg->ReadBin (str, ec->Macros + i, 0, NULL) ;
  }
  delete macroReg ;

  OlderFiles.Clear () ;
  CRegDef *olderReg = new CRegDef (t->m_Path + "\\Older") ;
  for (int i = 0 ; i < MAX_OLDER_FILES ; i++)
  {
    sprintf (str, "Older%d", i) ;
    CStdString older = olderReg->ReadString (str, "") ;
    if (older != "")
      OlderFiles.AppendItem (older) ;
  }
  delete olderReg ;

  RecentFiles.Clear () ;
  CRegDef *recentReg = new CRegDef (t->m_Path + "\\Recent") ;
  for (int i = 0 ; i < 9 ; i++)
  {
    sprintf (str, "Recent%d", i) ;
    CStdString recent = recentReg->ReadString (str, "") ;
    if (recent != "")
      RecentFiles.AppendItem (recent) ;
  }
  delete recentReg ;

  if (RestoreFiles)
  {
    CRegDef *openReg = new CRegDef (t->m_Path + "\\Open") ;
    for (int i = 0 ; i < MAX_EDITORS ; i++)
    {
      sprintf (str, "Open%d", i) ;
      params = openReg->ReadString (str, "") ;
      if (params == "" || params [1] == ',')
        continue ;
      if ((e = CreateNewEditor (DecodeFilename(GetNextField(params)), false, false, false)) == NULL)
        continue ;
      e->SetLineNo (atoi (GetNextField (params))) ;
      e->SetColNo (atoi (GetNextField (params))) ;
      e->SetTopLine (atoi (GetNextField (params))) ;
      e->SetLanguage ((TLanguage) atoi (GetNextField (params))) ;
    }
    delete openReg ;
  }

  UpdateRecent () ;

  InitialDir = t->ReadString ("InitialDir", DocumentsPath + "Scenes") ;
  TabCtrl_SetCurSel (hTabWindow, t->ReadInt ("CurrentTab", 0)) ;
  if (TabCtrl_GetCurSel (hTabWindow) < 0)
    TabCtrl_SetCurSel (hTabWindow, 0) ;

  CheckMenuItem (CM_CURSORBEYONDEOL, t->ReadBool ("CursorBeyondEOL", true)) ;
  CheckMenuItem (CM_CONSTRAINCARET, ec->SelBoundsEnabled) ;
  if (ec->SelBoundsEnabled)
  {
    CheckMenuItem (CM_CURSORBEYONDEOL, false) ;
    EnableMenuItem (CM_CURSORBEYONDEOL, false) ;
  }
  CheckMenuItem (CM_SHOWPARSEMESSAGES, t->ReadBool ("ShowParseMessages", true)) ;
  CheckMenuItem (CM_AUTOLOADERRORFILE, t->ReadBool ("AutoLoadErrorFile", true)) ;
  CheckMenuItem (CM_OVERLAYKEYWORDEXPANSION, ec->TabKeywordExpansion) ;
  AutoSaveDelay = t->ReadInt ("AutoSaveTime", 0) * 60 ;

  PrintPointSize = t->ReadInt ("PrintPointSize", 9) ;
  PrintUseBorder = t->ReadInt ("PrintUseBorder", 0) ;
  PrintLeftMargin = t->ReadInt ("PrintLeftMargin", -1) ;
  PrintTopMargin = t->ReadInt ("PrintTopMargin", -1) ;
  PrintRightMargin = t->ReadInt ("PrintRightMargin", -1) ;
  PrintBottomMargin = t->ReadInt ("PrintBottomMargin", -1) ;
  PrintUseColor = t->ReadBool ("PrintUseColor", true) ;
  PrintUseFancyText = t->ReadBool ("PrintUseFancyText", false) ;
  PrintUsePageNumbers = t->ReadBool ("PrintUsePageNumbers", true) ;
  PrintUseDateTime = t->ReadBool ("PrintUseDateTime", false) ;
  PrintUseFileName = t->ReadBool ("PrintUseFileName", true) ;
  PrintMarginUnitsMetric = t->ReadBool ("PrintMarginUnitsMetric", false) ;

  SetMenuState () ;
}

//------------------------------------------------------------------------------------------------------------------------

void PutSettings (CRegDef *t, EditConfigStruct *ec)
{
  int                   count ;
  char                  str [256] ;
  LOGFONT               lf ;
  CCodeMax              *e ;
  CStdString            params ;

  if (Editor != NULL)
    Editor->GetConfigFromInstance (ec) ;
  CCodeMax::GetConfigFromCommonSettings (ec) ;
  t->WriteInt ("AutoIndent", ec->AutoIndent) ;
  t->WriteBool ("SyntaxHighlighting", ec->SyntaxHighlighting) ;
  t->WriteBool ("WhiteSpaceDisplay", ec->WhiteSpaceDisplay) ;
  t->WriteBool ("TabExpand", ec->TabExpand) ;
  t->WriteBool ("SmoothScrolling", ec->SmoothScrolling) ;
  t->WriteBool ("LineToolTips", ec->LineToolTips) ;
  t->WriteBool ("LeftMarginVisible", ec->LeftMarginVisible) ;
  t->WriteBool ("CaseSensitive", ec->CaseSensitive) ;
  t->WriteBool ("PreserveCase", ec->PreserveCase) ;
  t->WriteBool ("WholeWordEnabled", ec->WholeWordEnabled) ;
  t->WriteBool ("DragDropEnabled", ec->DragDropEnabled) ;
  t->WriteBool ("HSplitterEnabled", ec->HSplitterEnabled) ;
  t->WriteBool ("VSplitterEnabled", ec->VSplitterEnabled) ;
  t->WriteBool ("ColumnSelEnabled", ec->ColumnSelEnabled) ;
  t->WriteBool ("RegexpEnabled", ec->RegexpEnabled) ;
  t->WriteBool ("OvertypeCaret", ec->OvertypeCaret) ;
  t->WriteBool ("SelBoundsEnabled", ec->SelBoundsEnabled) ;
  t->WriteBool ("TabKeywordExpansion", ec->TabKeywordExpansion) ;
  t->WriteInt ("ScrollBars", (int) ec->ScrollBars) ;
  t->WriteInt ("TabSize", ec->TabSize) ;
  t->WriteInt ("UndoLimit", ec->UndoLimit) ;
  t->WriteBin ("Colours", (char *) &ec->Colours, sizeof (CodemaxColors)) ;
  t->WriteBin ("FontStyles", (char *) &ec->FontStyles, sizeof (CodemaxFontStyles)) ;
  if (ec->HotKeys && ec->HotKeyLen >= 512)
    t->WriteBin ("KeyBindings", ec->HotKeys, ec->HotKeyLen) ;
  t->WriteString ("FindMRUList", ec->FindMRUList == "" ? "<empty>" : ec->FindMRUList) ;
  t->WriteString ("ReplaceMRUList", ec->ReplaceMRUList == "" ? "<empty>" : ec->ReplaceMRUList) ;
  t->WriteBool ("UndoAfterSave", UndoAfterSave) ;
  t->WriteBool ("CreateBackups", CreateBackups) ;
  t->WriteInt ("AutoReload", AutoReload) ;
  t->WriteInt ("SmallMessageWindowSize", EditDragOffset) ;
  if (GetObject (ec->HFont, sizeof (LOGFONT), &lf))
    t->WriteBin ("Font", (char *) &lf, sizeof (LOGFONT)) ;

  CRegDef *macroReg = new CRegDef (t->m_Path + "\\Macro") ;
  for (int i = 0 ; i < CODEMAX_MACRO_LIMIT ; i++)
  {
    sprintf (str, "Macro%d", i) ;
    if (ec->MacroLen [i] == 0)
    {
      macroReg->DeleteValue (str) ;
      continue ;
    }
    macroReg->WriteBin (str, ec->Macros [i], ec->MacroLen [i]) ;
  }
  delete macroReg ;

  UpdateRecent () ;

  count = RecentFiles.ItemCount () ;
  CRegDef *recentReg = new CRegDef (t->m_Path + "\\Recent") ;
  for (int i = 0 ; i < 9 ; i++)
  {
    sprintf (str, "Recent%d", i) ;
    if (i >= count)
    {
      recentReg->DeleteValue (str) ;
      continue ;
    }
    recentReg->WriteString (str, RecentFiles [i]) ;
  }
  delete recentReg ;

  count = OlderFiles.ItemCount () ;
  CRegDef *olderReg = new CRegDef (t->m_Path + "\\Older") ;
  for (int i = 0 ; i < MAX_OLDER_FILES ; i++)
  {
    sprintf (str, "Older%d", i) ;
    if (i >= count)
    {
      olderReg->DeleteValue (str) ;
      continue ;
    }
    olderReg->WriteString (str, OlderFiles [i]) ;
  }
  delete olderReg ;

  CRegDef *openReg = new CRegDef (t->m_Path + "\\Open") ;
  for (int i = 0 ; i < MAX_EDITORS ; i++)
  {
    sprintf (str, "Open%d", i) ;
    if (i >= EditorCount)
    {
      openReg->DeleteValue (str) ;
      continue ;
    }
    e = Editors [i] ;
    if (e->m_Tag.LongName [0] == '\0')
    {
      openReg->DeleteValue (str) ;
      continue ;
    }
    params.Format ("%s,%d,%d,%d,%d,%d,%d",
                    EncodeFilename(e->m_Tag.LongName).c_str(),
                    e->GetLineNo (),
                    e->GetColNo (),
                    e->GetTopLine (),
                    (int) e->GetLanguage (),
                    e->GetTabSize (),
                    (int) e->GetAutoIndent ()) ;
    openReg->WriteString (str, params) ;
  }
  delete openReg ;

  t->WriteString ("InitialDir", InitialDir) ;
  t->WriteInt ("CurrentTab", TabCtrl_GetCurSel (hTabWindow)) ;
  t->WriteInt ("AutoSaveTime", AutoSaveDelay / 60) ;
  t->WriteBool ("ShowParseMessages", IsMenuItemChecked (CM_SHOWPARSEMESSAGES)) ;
  t->WriteBool ("AutoLoadErrorFile", IsMenuItemChecked (CM_AUTOLOADERRORFILE)) ;
  t->WriteBool ("CursorBeyondEOL", IsMenuItemChecked (CM_CURSORBEYONDEOL)) ;

  t->WriteInt ("PrintPointSize", PrintPointSize) ;
  t->WriteInt ("PrintUseBorder", PrintUseBorder) ;
  t->WriteInt ("PrintLeftMargin", PrintLeftMargin) ;
  t->WriteInt ("PrintTopMargin", PrintTopMargin) ;
  t->WriteInt ("PrintRightMargin", PrintRightMargin) ;
  t->WriteInt ("PrintBottomMargin", PrintBottomMargin) ;
  t->WriteBool ("PrintUseColor", PrintUseColor) ;
  t->WriteBool ("PrintUseFancyText", PrintUseFancyText) ;
  t->WriteBool ("PrintUsePageNumbers", PrintUsePageNumbers) ;
  t->WriteBool ("PrintUseDateTime", PrintUseDateTime) ;
  t->WriteBool ("PrintUseFileName", PrintUseFileName) ;
  t->WriteBool ("PrintMarginUnitsMetric", PrintMarginUnitsMetric) ;
}

