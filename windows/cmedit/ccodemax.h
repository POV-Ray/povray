//******************************************************************************
///
/// @file windows/cmedit/ccodemax.h
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

#ifndef __CCODEMAX_H__
#define __CCODEMAX_H__

struct _EditConfigStruct;

#define CODEMAX_SUCCESS                         1
#define CODEMAX_MACRO_LIMIT                     10
#define CODEMAX_MAX_FINDREPLACE_TEXT            100
#define CODEMAX_FIND_REPLACE_MRU_MAX            10
#define CODEMAX_FIND_REPLACE_MRU_BUFF_SIZE      ((CODEMAX_MAX_FINDREPLACE_TEXT + 1) * CODEMAX_FIND_REPLACE_MRU_MAX)

#define CODEMAX_CMD_WORDUPPERCASE               100
#define CODEMAX_CMD_WORDTRANSPOSE               101
#define CODEMAX_CMD_WORDRIGHTEXTEND             102
#define CODEMAX_CMD_WORDRIGHT                   103
#define CODEMAX_CMD_WORDENDRIGHT                104
#define CODEMAX_CMD_WORDENDRIGHTEXTEND          105
#define CODEMAX_CMD_WORDLOWERCASE               106
#define CODEMAX_CMD_WORDLEFTEXTEND              107
#define CODEMAX_CMD_WORDLEFT                    108
#define CODEMAX_CMD_WORDENDLEFT                 109
#define CODEMAX_CMD_WORDENDLEFTEXTEND           110
#define CODEMAX_CMD_WORDDELETETOSTART           111
#define CODEMAX_CMD_WORDDELETETOEND             112
#define CODEMAX_CMD_WORDCAPITALIZE              113
#define CODEMAX_CMD_WINDOWSTART                 114
#define CODEMAX_CMD_WINDOWSCROLLUP              115
#define CODEMAX_CMD_WINDOWSCROLLTOTOP           116
#define CODEMAX_CMD_WINDOWSCROLLTOCENTER        117
#define CODEMAX_CMD_WINDOWSCROLLTOBOTTOM        118
#define CODEMAX_CMD_WINDOWSCROLLRIGHT           119
#define CODEMAX_CMD_WINDOWSCROLLLEFT            120
#define CODEMAX_CMD_WINDOWSCROLLDOWN            121
#define CODEMAX_CMD_WINDOWRIGHTEDGE             122
#define CODEMAX_CMD_WINDOWLEFTEDGE              123
#define CODEMAX_CMD_WINDOWEND                   124
#define CODEMAX_CMD_UPPERCASESELECTION          125
#define CODEMAX_CMD_UNTABIFYSELECTION           126
#define CODEMAX_CMD_UNINDENTSELECTION           127
#define CODEMAX_CMD_UNDOCHANGES                 128
#define CODEMAX_CMD_UNDO                        129
#define CODEMAX_CMD_TABIFYSELECTION             130
#define CODEMAX_CMD_SENTENCERIGHT               131
#define CODEMAX_CMD_SENTENCELEFT                132
#define CODEMAX_CMD_SENTENCECUT                 133
#define CODEMAX_CMD_SELECTSWAPANCHOR            134
#define CODEMAX_CMD_SELECTPARA                  135
#define CODEMAX_CMD_SELECTLINE                  136
#define CODEMAX_CMD_SELECTALL                   137
#define CODEMAX_CMD_REDOCHANGES                 138
#define CODEMAX_CMD_REDO                        139
#define CODEMAX_CMD_PASTE                       140
#define CODEMAX_CMD_PARAUP                      141
#define CODEMAX_CMD_PARADOWN                    142
#define CODEMAX_CMD_PAGEUPEXTEND                143
#define CODEMAX_CMD_PAGEUP                      144
#define CODEMAX_CMD_PAGEDOWNEXTEND              145
#define CODEMAX_CMD_PAGEDOWN                    146
#define CODEMAX_CMD_LOWERCASESELECTION          147
#define CODEMAX_CMD_LINEUPEXTEND                148
#define CODEMAX_CMD_LINEUP                      149
#define CODEMAX_CMD_LINETRANSPOSE               150
#define CODEMAX_CMD_LINESTART                   151
#define CODEMAX_CMD_LINEOPENBELOW               152
#define CODEMAX_CMD_LINEOPENABOVE               153
#define CODEMAX_CMD_LINEENDEXTEND               154
#define CODEMAX_CMD_LINEEND                     155
#define CODEMAX_CMD_LINEDOWNEXTEND              156
#define CODEMAX_CMD_LINEDOWN                    157
#define CODEMAX_CMD_LINEDELETETOSTART           158
#define CODEMAX_CMD_LINEDELETETOEND             159
#define CODEMAX_CMD_LINEDELETE                  160
#define CODEMAX_CMD_LINECUT                     161
#define CODEMAX_CMD_INDENTTOPREV                162
#define CODEMAX_CMD_INDENTSELECTION             163
#define CODEMAX_CMD_HOMEEXTEND                  164
#define CODEMAX_CMD_HOME                        165
#define CODEMAX_CMD_GOTOMATCHBRACE              166
#define CODEMAX_CMD_GOTOINDENTATION             167
#define CODEMAX_CMD_GOTOLINE                    168
#define CODEMAX_CMD_FINDREPLACE                 169
#define CODEMAX_CMD_REPLACE                     170
#define CODEMAX_CMD_REPLACEALLINBUFFER          171
#define CODEMAX_CMD_REPLACEALLINSELECTION       172
#define CODEMAX_CMD_FINDPREVWORD                173
#define CODEMAX_CMD_FINDPREV                    174
#define CODEMAX_CMD_FINDNEXTWORD                175
#define CODEMAX_CMD_FINDNEXT                    176
#define CODEMAX_CMD_FINDMARKALL                 177
#define CODEMAX_CMD_FIND                        178
#define CODEMAX_CMD_SETFINDTEXT                 179
#define CODEMAX_CMD_SETREPLACETEXT              180
#define CODEMAX_CMD_TOGGLEPRESERVECASE          181
#define CODEMAX_CMD_TOGGLEWHOLEWORD             182
#define CODEMAX_CMD_TOGGLECASESENSITIVE         183
#define CODEMAX_CMD_END                         184
#define CODEMAX_CMD_TOGGLEWHITESPACEDISPLAY     185
#define CODEMAX_CMD_TOGGLEOVERTYPE              186
#define CODEMAX_CMD_SETREPEATCOUNT              187
#define CODEMAX_CMD_DOCUMENTSTARTEXTEND         188
#define CODEMAX_CMD_DOCUMENTSTART               189
#define CODEMAX_CMD_DOCUMENTENDEXTEND           190
#define CODEMAX_CMD_DOCUMENTEND                 191
#define CODEMAX_CMD_DELETEHORIZONTALSPACE       192
#define CODEMAX_CMD_DELETEBLANKLINES            193
#define CODEMAX_CMD_DELETEBACK                  194
#define CODEMAX_CMD_DELETE                      195
#define CODEMAX_CMD_CUTSELECTION                196
#define CODEMAX_CMD_CUT                         197
#define CODEMAX_CMD_COPY                        198
#define CODEMAX_CMD_CHARTRANSPOSE               199
#define CODEMAX_CMD_CHARRIGHTEXTEND             200
#define CODEMAX_CMD_CHARRIGHT                   201
#define CODEMAX_CMD_CHARLEFTEXTEND              202
#define CODEMAX_CMD_CHARLEFT                    203
#define CODEMAX_CMD_BOOKMARKTOGGLE              204
#define CODEMAX_CMD_BOOKMARKPREV                205
#define CODEMAX_CMD_BOOKMARKNEXT                206
#define CODEMAX_CMD_BOOKMARKCLEARALL            207
#define CODEMAX_CMD_BOOKMARKJUMPTOFIRST         208
#define CODEMAX_CMD_BOOKMARKJUMPTOLAST          209
#define CODEMAX_CMD_APPENDNEXTCUT               210
#define CODEMAX_CMD_INSERTCHAR                  211
#define CODEMAX_CMD_NEWLINE                     212
#define CODEMAX_CMD_RECORDMACRO                 213
#define CODEMAX_CMD_PLAYMACRO1                  214
#define CODEMAX_CMD_PLAYMACRO2                  215
#define CODEMAX_CMD_PLAYMACRO3                  216
#define CODEMAX_CMD_PLAYMACRO4                  217
#define CODEMAX_CMD_PLAYMACRO5                  218
#define CODEMAX_CMD_PLAYMACRO6                  219
#define CODEMAX_CMD_PLAYMACRO7                  220
#define CODEMAX_CMD_PLAYMACRO8                  221
#define CODEMAX_CMD_PLAYMACRO9                  222
#define CODEMAX_CMD_PLAYMACRO10                 223
#define CODEMAX_CMD_PROPERTIES                  224
#define CODEMAX_CMD_BEGINUNDO                   225
#define CODEMAX_CMD_ENDUNDO                     226
#define CODEMAX_CMD_TOGGLEREGEXP                228
#define CODEMAX_CMD_CLEARSELECTION              229
#define CODEMAX_CMD_REGEXPON                    230
#define CODEMAX_CMD_REGEXPOFF                   231
#define CODEMAX_CMD_WHOLEWORDON                 232
#define CODEMAX_CMD_WHOLEWORDOFF                233
#define CODEMAX_CMD_PRESERVECASEON              234
#define CODEMAX_CMD_PRESERVECASEOFF             235
#define CODEMAX_CMD_CASESENSITIVEON             236
#define CODEMAX_CMD_CASESENSITIVEOFF            237
#define CODEMAX_CMD_WHITESPACEDISPLAYON         238
#define CODEMAX_CMD_WHITESPACEDISPLAYOFF        239
#define CODEMAX_CMD_OVERTYPEON                  240
#define CODEMAX_CMD_OVERTYPEOFF                 241
#define CODEMAX_CMD_CODELIST                    242
#define CODEMAX_CMD_CODETIP                     243
#define CODEMAX_CMD_LAST                        243
#define CODEMAX_USER_BASE                       1000

#define CODEMAX_NOTIFY_CHANGE                   100
#define CODEMAX_NOTIFY_HSCROLL                  110
#define CODEMAX_NOTIFY_VSCROLL                  120
#define CODEMAX_NOTIFY_SELCHANGE                130
#define CODEMAX_NOTIFY_VIEWCHANGE               140
#define CODEMAX_NOTIFY_MODIFIEDCHANGE           150
#define CODEMAX_NOTIFY_SHOWPROPS                160
#define CODEMAX_NOTIFY_PROPSCHANGE              170
#define CODEMAX_NOTIFY_CREATE                   180
#define CODEMAX_NOTIFY_DESTROY                  190
#define CODEMAX_NOTIFY_DRAWLINE                 200
#define CODEMAX_NOTIFY_DELETELINE               210
#define CODEMAX_NOTIFY_CMDFAILURE               220
#define CODEMAX_NOTIFY_REGISTEREDCMD            230
#define CODEMAX_NOTIFY_KEYDOWN                  240
#define CODEMAX_NOTIFY_KEYUP                    250
#define CODEMAX_NOTIFY_KEYPRESS                 260
#define CODEMAX_NOTIFY_MOUSEDOWN                270
#define CODEMAX_NOTIFY_MOUSEUP                  280
#define CODEMAX_NOTIFY_MOUSEMOVE                290
#define CODEMAX_NOTIFY_OVERTYPECHANGE           300
#define CODEMAX_NOTIFY_FINDWRAPPED              310
#define CODEMAX_NOTIFY_CODELIST                 320
#define CODEMAX_NOTIFY_CODELISTSELMADE          330
#define CODEMAX_NOTIFY_CODELISTCANCEL           340
#define CODEMAX_NOTIFY_CODELISTCHAR             350
#define CODEMAX_NOTIFY_CODETIP                  360
#define CODEMAX_NOTIFY_CODETIPCANCEL            370
#define CODEMAX_NOTIFY_CODETIPUPDATE            380
#define CODEMAX_NOTIFY_CODELISTPOSTCREATE       390

#define CODEMAX_INDENT_OFF                      0
#define CODEMAX_INDENT_SCOPE                    1
#define CODEMAX_INDENT_PREVLINE                 2

#define CODEMAX_FONT_NORMAL                     0
#define CODEMAX_FONT_BOLD                       1
#define CODEMAX_FONT_ITALIC                     2
#define CODEMAX_FONT_BOLDITALIC                 3
#define CODEMAX_FONT_UNDERLINE                  4

#define CODEMAX_KEY_SHIFT                       1
#define CODEMAX_KEY_CTRL                        2
#define CODEMAX_KEY_ALT                         4

#define CODEMAX_PRINT_PROMPTDLG                 0x0000
#define CODEMAX_PRINT_DEFAULTPRN                0x0001
#define CODEMAX_PRINT_HDC                       0x0002
#define CODEMAX_PRINT_RICHFONTS                 0x0004
#define CODEMAX_PRINT_COLOR                     0x0008
#define CODEMAX_PRINT_PAGENUMS                  0x0010
#define CODEMAX_PRINT_DATETIME                  0x0020
#define CODEMAX_PRINT_BORDERTHIN                0x0040
#define CODEMAX_PRINT_BORDERTHICK               0x0080
#define CODEMAX_PRINT_BORDERDOUBLE              0x0100
#define CODEMAX_PRINT_SELECTION                 0x0200
#define CODEMAX_PRINT_FILENAME                  0x0400
#define CODEMAX_PRINT_PAGERANGE                 0x0800

enum TLanguage
{
  tlNone,
  tlCCpp,
  tlBasic,
  tlJava,
  tlPascal,
  tlSQL,
  tlPOVRay,
  tlHTML,
  tlSGML,
  tlINI
};

enum TAutoIndent
{
  taiNone = CODEMAX_INDENT_OFF,
  taiScope = CODEMAX_INDENT_SCOPE,
  taiPrevLine = CODEMAX_INDENT_PREVLINE
};

enum TScrollStyle
{
  tssNone,
  tssHorizontal,
  tssVertical,
  tssBoth
};

typedef struct
{
  BYTE Text;
  BYTE Number;
  BYTE Keyword;
  BYTE Operator;
  BYTE ScopeKeyword;
  BYTE Comment;
  BYTE String;
  BYTE TagText;
  BYTE TagEntity;
  BYTE TagElementName;
  BYTE TagAttributeName;
  BYTE LineNumber;
} CodemaxFontStyles;

typedef struct
{
  COLORREF WindowBackground;
  COLORREF LeftMarginBackground;
  COLORREF BookmarkForeground;
  COLORREF BookmarkBackground;
  COLORREF TextForeground;
  COLORREF TextBackground;
  COLORREF NumberForeground;
  COLORREF NumberBackground;
  COLORREF KeywordForeground;
  COLORREF KeywordBackground;
  COLORREF OperatorForeground;
  COLORREF OperatorBackground;
  COLORREF ScopeKeywordForeground;
  COLORREF ScopeKeywordBackground;
  COLORREF CommentForeground;
  COLORREF CommentBackground;
  COLORREF StringForeground;
  COLORREF StringBackground;
  COLORREF TagTextForeground;
  COLORREF TagTextBackground;
  COLORREF TagEntityForeground;
  COLORREF TagEntityBackground;
  COLORREF TagElementNameForeground;
  COLORREF TagElementNameBackground;
  COLORREF TagAttributeNameForeground;
  COLORREF TagAttributeNameBackground;
  COLORREF LineNumberForeground;
  COLORREF LineNumberBackground;
  COLORREF HDividerLines;
  COLORREF VDividerLines;
  COLORREF HighlightedLine;
} CodemaxColors;

typedef struct
{
  DWORD   Style;
  BOOL    sCaseSensitive;
  LPCTSTR Keywords;
  LPCTSTR Operators;
  LPCTSTR SingleLineComments;
  LPCTSTR MultiLineComments1;
  LPCTSTR MultiLineComments2;
  LPCTSTR ScopeKeywords1;
  LPCTSTR ScopeKeywords2;
  LPCTSTR StringDelims;
  TCHAR   Escape;
  TCHAR   Terminator;
  LPCTSTR TagElementNames;
  LPCTSTR TagAttributeNames;
  LPCTSTR TagEntities;
} CodemaxLanguage;

typedef enum
{
  stSave,
  stSaved = stSave,
  stDiscard,
  stDiscarded = stDiscard,
  stSaveAll,
  stDiscardAll,
  stCancel,
  stContinue,
  stRetry,
  stError
} TSaveType;

typedef struct
{
  char                  ShortName [_MAX_PATH];
  char                  LongName [_MAX_PATH];
  FILETIME              TimeSaved;
} EditTagStruct;

typedef struct
{
  int Line;
  int Col;
} CodemaxPos;

typedef struct
{
  CodemaxPos      Start;
  CodemaxPos      End;
  BOOL            ColumnSel;
} CodemaxRange;

typedef struct
{
  BYTE      Modifier1;
  UINT      VirtKey1;
  BYTE      Modifiers2;
  UINT      VirtKey2;
} CodemaxHotkey;

typedef struct
{
    NMHDR   header;
    HWND    hListCtrl;
} CodemaxCodeListdata;

typedef struct
{
  NMHDR header;
  int   KeyCode;
  int   KeyModifier;
} CodemaxKeydata;

typedef struct
{
  NMHDR header;
  WORD  Command;
} CodemaxRegisteredCommandData;

typedef struct
{
  HDC     DC ;
  DWORD   Flags ;
  LPCTSTR FileName ;
  DWORD   FirstPage ;
  DWORD   LastPage ;
  DWORD   PointSize ;
  RECT    Margin ;
} CodemaxPrintEx;

extern "C" LRESULT CMRegisterControl(DWORD Version = 0x02100);
extern "C" LRESULT CMUnregisterControl();

extern "C" LRESULT CMRegisterCommand(WORD CommandID, LPCTSTR CommandName, LPCTSTR CommandDescription);
extern "C" LRESULT CMUnregisterCommand(WORD CommandID);
extern "C" void CMGetCommandString(WORD Command, BOOL Desc, LPTSTR Buffer, int BufferLength);

extern "C" int CMGetHotKeys(LPBYTE Buffer);
extern "C" int CMGetHotKeysForCmd(WORD Command, CodemaxHotkey *HotKeys);
extern "C" int CMLookupHotKey(CodemaxHotkey *HotKey ) ;
extern "C" LRESULT CMSetHotKeys(const LPBYTE Buffer);
extern "C" LRESULT CMRegisterHotKey(CodemaxHotkey *HotKey, WORD Command);
extern "C" LRESULT CMUnregisterHotKey(CodemaxHotkey *HotKey);
extern "C" void CMResetDefaultHotKeys();

extern "C" LRESULT CMUnregisterAllLanguages();
extern "C" LRESULT CMRegisterLanguage(LPCTSTR LanguageName, CodemaxLanguage *LanguageData);
extern "C" LRESULT CMUnregisterLanguage(LPCTSTR LanguageName);
extern "C" int CMGetLanguageDef(LPCTSTR LanguageName, CodemaxLanguage *LanguageData);

extern "C" int CMGetMacro(int MacroID, LPBYTE MacroBuffer);
extern "C" int CMSetMacro(int MacroID, const LPBYTE MacroBuffer);

extern "C" void CMGetFindReplaceMRUList(LPTSTR MRUList, BOOL Find);
extern "C" void CMSetFindReplaceMRUList(LPCTSTR MRUList, BOOL Find);

class CCodeMax
{
public:
  int                   m_Index;
  int                   m_RMBDownX;
  int                   m_RMBDownY;
  int                   m_RMBDownLine;
  int                   m_RMBDownCol;
  bool                  m_Opened;
  bool                  m_BackedUp;
  bool                  m_LButtonDown;
  HWND                  m_hWnd;
  char                  m_LanguageName [64];
  WNDPROC               m_OldWndProc;
  TLanguage             m_Language;
  EditTagStruct         m_Tag;

  static bool           SaveDialogActive;

  CCodeMax (HWND parent);
  ~CCodeMax ();

  operator const HWND () const { return this->m_hWnd; }

  void PopupMenuPopup (void);
  CStdString LocateIncludeFilename (int line, int col);
  CStdString LocateCommandLine (int line);
  LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam);
  bool IsCodeUpToDate (bool Stale);
  bool AskFileName (EditTagStruct *t = NULL);
  void SetLanguageBasedOnFileType (void);
  void SetupEditor (const struct _EditConfigStruct *ec, bool PropsChange, bool InitCM);
  TSaveType SaveEditorFile (void);
  TSaveType TrySave (bool ContinueOption);
  void UpdateFileTime (void);
  void SetFileName (LPCTSTR FileName);
  void SetOpened (bool IsOpened);
  void SetLanguage (LPCSTR Language) { SendMessage(m_hWnd, WM_USER + 1600, 0, (LPARAM) Language); }
  void GetConfigFromInstance (struct _EditConfigStruct *ec);
  CStdString GetCurrentKeyword (void);
  LPCTSTR GetLanguageName (void);
  void SetLanguage (TLanguage Language);
  TLanguage GetLanguage (void);
  void GetLanguage(LPCSTR name) { SendMessage(m_hWnd, WM_USER + 1601, 0, (LPARAM) name); }
  void SetAutoIndent (TAutoIndent AutoIndent) { SendMessage(m_hWnd, WM_USER + 3100, (WPARAM) (int (AutoIndent)), 0); }
  TAutoIndent GetAutoIndent (void) { return TAutoIndent (((int) SendMessage(m_hWnd, WM_USER + 3110, 0, 0))); }
  bool GetColourSyntax (void) { return SendMessage(m_hWnd, WM_USER + 1620, 0, 0) != 0; }
  void SetColourSyntax (bool enabled) { SendMessage(m_hWnd, WM_USER + 1610, (WPARAM) enabled, 0); }
  int GetLineLength (int line) { return ((int) SendMessage(m_hWnd, WM_USER + 2380, (WPARAM) (line - 1), (LPARAM) false)); }
  TScrollStyle GetScrollBars (void);
  void SetScrollBars (TScrollStyle style);
  void ShowScrollbar(bool horizontal, bool show) { SendMessage(m_hWnd, WM_USER + 3700, (WPARAM) horizontal, (LPARAM) show); }
  bool HasScrollBar(bool horizontal) { return SendMessage(m_hWnd, WM_USER + 3710, (WPARAM) horizontal, 0) != 0; }
  int GetHSplitterPos (void) { return (int) SendMessage(m_hWnd, WM_USER + 2901, (WPARAM) 1, 0); }
  int GetVSplitterPos (void) { return (int) SendMessage(m_hWnd, WM_USER + 2901, (WPARAM) 0, 0); }
  void SetHSplitterPos (int pos) { SendMessage(m_hWnd, WM_USER + 2900, (WPARAM) 1, (LPARAM) pos); }
  void SetVSplitterPos (int pos) { SendMessage(m_hWnd, WM_USER + 2900, (WPARAM) 0, (LPARAM) pos); }
  bool GetHSplitterEnable (void) { return SendMessage(m_hWnd, WM_USER + 3730, (WPARAM) 1, 0) != 0; }
  void SetHSplitterEnable (bool enable) { SendMessage(m_hWnd, WM_USER + 3720, (WPARAM) 1, (LPARAM) enable); }
  bool GetVSplitterEnable (void) { return SendMessage(m_hWnd, WM_USER + 3730, (WPARAM) 0, 0) != 0; }
  void SetVSplitterEnable (bool enable) { SendMessage(m_hWnd, WM_USER + 3720, (WPARAM) 0, (LPARAM) enable); }
  int GetLineNo (void);
  void SetLineNo (int LineNo);
  int GetColNo (void);
  void SetColNo (int ColNo);
  void SetPosition (int LineNo, int ColNo);
  int GetTopLine (void) { return ((int) SendMessage(m_hWnd, WM_USER + 1970, (WPARAM) 0, 0)) + 1; }
  void SetTopLine (int TopLine) { SendMessage(m_hWnd, WM_USER + 1960, (WPARAM) 0, (LPARAM) (TopLine - 1)); }

  LRESULT GetLine (int nLine, LPTSTR pszBuff) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2370, (WPARAM) (nLine - 1), (LPARAM) pszBuff)); }
  LRESULT InsertText (LPCTSTR Text, const CodemaxPos *pPos = NULL) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2330, (WPARAM) pPos, (LPARAM) Text)); }
  LRESULT Copy (void) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 3380, 0, 0)); }
  LRESULT Cut (void) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 3370, 0, 0)); }
  LRESULT Paste (void) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 3390, 0, 0)); }
  LRESULT ClearUndoBuffer (void) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 3351, 0, 0)); }
  LRESULT InsertFile (LPCTSTR FileName, const CodemaxPos *pPos = NULL) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2320, (WPARAM) pPos, (LPARAM) FileName)); }
  LRESULT SetColors (const CodemaxColors *pColors) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 1630, 0, (LPARAM) pColors)); }
  LRESULT GetColors (CodemaxColors *pColors) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 1640, 0, (LPARAM) pColors)); }
  LRESULT SetFontStyles (const CodemaxFontStyles *pFontStyles) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 3780, 0, (LPARAM) pFontStyles)); }
  LRESULT GetFontStyles (CodemaxFontStyles *pFontStyles) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 3790, 0, (LPARAM) pFontStyles)); }
  LRESULT ReplaceText (LPCTSTR pszText, const CodemaxRange *pRange = NULL) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2340, (WPARAM) pRange, (LPARAM) pszText)); }
  LRESULT SetText (LPCTSTR pszText) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2335, 0, (LPARAM) pszText)); }
  LRESULT GetText (LPTSTR pszBuff, const CodemaxRange *pRange = NULL) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2360, (WPARAM) pRange, (LPARAM) pszBuff)); }
  int GetTextLength (const CodemaxRange *pRange = NULL) { return ((int) SendMessage(m_hWnd, WM_USER + 2350, (WPARAM) pRange, (LPARAM) false)); }
  LRESULT SaveFile (LPCTSTR pszFileName, BOOL bClearUndo = TRUE) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2310, (WPARAM) bClearUndo, (LPARAM) pszFileName)); }
  LRESULT OpenFile (LPCTSTR File) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2300, 0, (LPARAM) File)); }
  LRESULT GetWord (LPTSTR pszBuff, CodemaxPos *pPos) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2381, (WPARAM) pPos, (LPARAM) pszBuff)); }
  int GetWordLength (CodemaxPos *pPos) { return ((int) SendMessage(m_hWnd, WM_USER + 2382, (WPARAM) pPos, (LPARAM) false)); }
  CStdString GetCurrentWord (void) { return GetWord (NULL); }
  LRESULT AddText (LPCTSTR pszText) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2390, 0, (LPARAM) pszText)); }
  LRESULT DeleteLine (int nLine) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2400, (WPARAM) (nLine - 1), 0)); }
  LRESULT InsertLine (int nLine, LPCTSTR pszText) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2410, (WPARAM) (nLine - 1), (LPARAM) pszText)); }
  LRESULT GetSel (CodemaxRange *pRange, BOOL bNormalized = TRUE) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2420, (WPARAM) (bNormalized), (LPARAM) pRange)); }
  LRESULT SetSel (const CodemaxRange *pRange, BOOL bMakeVisible = TRUE) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2430, (WPARAM) (bMakeVisible), (LPARAM) pRange)); }
  LRESULT DeleteSel (void) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2440, 0, 0)); }
  LRESULT ReplaceSel (LPCTSTR pszText) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2450, 0, (LPARAM) pszText)); }
  LRESULT ExecuteCmd (WORD wCmd, DWORD dwCmdData = 0) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2700, (WPARAM) (wCmd), (LPARAM) dwCmdData)); }
  LRESULT SetSplitterPos (BOOL bHorz, int nPos) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2900, (WPARAM) (bHorz), (LPARAM) nPos)); }
  int GetSplitterPos (BOOL bHorz) { return ((int) SendMessage(m_hWnd, WM_USER + 2901, (WPARAM) (bHorz), 0)); }
  LRESULT SetTopIndex (int nView, int nLine) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 1960, (WPARAM) (nView), (LPARAM) (nLine - 1))); }
  int GetTopIndex (int nView) { return ((int) SendMessage(m_hWnd, WM_USER + 1970, (WPARAM) (nView), 0)); }
  int GetVisibleLineCount (int nView, BOOL bFullyVisible = TRUE) { return ((int) SendMessage(m_hWnd, WM_USER + 1980, (WPARAM) (nView), (LPARAM) bFullyVisible)); }
  LRESULT SetFontOwnership (BOOL bEnable) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2485, (WPARAM) (bEnable), 0)); }
  bool GetFontOwnership (void) { return SendMessage(m_hWnd, WM_USER + 2486, 0, 0) != 0; }
  int GetCurrentView (void) { return ((int) SendMessage(m_hWnd, WM_USER + 3610, 0, 0)); }
  int GetViewCount (void) { return ((int) SendMessage(m_hWnd, WM_USER + 3600, 0, 0)); }
  LRESULT GetSelFromPoint (int xClient, int yClient, CodemaxPos *pPos) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2425, MAKEWPARAM((xClient), (yClient)), (LPARAM) pPos)); }
  LRESULT SelectLine (int nLine, BOOL bMakeVisible = TRUE) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 2435, (WPARAM) (nLine - 1), (LPARAM) bMakeVisible)); }
  int GetErrorLine (void) { return ((int) SendMessage(m_hWnd, WM_USER + 3960, 0, 0)) + 1; }
  void SetErrorLine (int LineNo) { SendMessage(m_hWnd, WM_USER + 3950, (WPARAM) (--LineNo), 0); }
  void ClearErrorLine (void) { ((int) SendMessage(m_hWnd, WM_USER + 4190, 0, 0)); }
  int HitTest (int xClient, int yClient) { return ((int) SendMessage(m_hWnd, WM_USER + 1990, MAKEWPARAM((xClient), (yClient)), 0)); }
  LRESULT SetDlgParent (HWND parent) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 3750, (WPARAM) (parent), 0)); }
  HFONT GetFont (void) { return (HFONT) SendMessage (m_hWnd, WM_GETFONT, 0, 0); }
  void SetFont (HFONT font) { SendMessage (m_hWnd, WM_SETFONT, (WPARAM) font, MAKELPARAM (true, 0)); }
  void ShowProperties (void) { SendMessage (m_hWnd, WM_USER + 2700, (100 + 124), 0); }
  void Undo (void) { SendMessage (m_hWnd, WM_USER + 2700, (100 + 29), 0); }
  void Redo (void) { SendMessage (m_hWnd, WM_USER + 2700, (100 + 39), 0); }
  void Find (void) { SendMessage (m_hWnd, WM_USER + 2700, (100 + 78), 0); }
  void FindNext (void) { SendMessage (m_hWnd, WM_USER + 2700, (100 + 76), 0); }
  void Replace (void) { SendMessage (m_hWnd, WM_USER + 2700, (100 + 69), 0); }
  void ReplaceNext (void) { SendMessage (m_hWnd, WM_USER + 2700, (100 + 70), 0); }
  void Record (void) { SendMessage (m_hWnd, WM_USER + 2700, (100 + 113), 0); }
  void GoToLine (int Line) { SendMessage (m_hWnd, WM_USER + 2700, (100 + 68), Line - 1); }
  void SetCaretPos (int Line, int Col) { SendMessage(m_hWnd, WM_USER + 4130, (WPARAM) (Line - 1), (LPARAM) (Col - 1)); }
  DWORD ExecuteCommand (WORD Command, int Param) { return (DWORD) SendMessage (m_hWnd, WM_USER + 2700, Command, Param); }
  bool IsModified (void) { return SendMessage(m_hWnd, WM_USER + 2460, 0, 0) != 0; }
  void SetModified (bool State) { SendMessage(m_hWnd, WM_USER + 2461, (WPARAM) (State), 0); }
  bool IsOvertypeMode (void) { return SendMessage(m_hWnd, WM_USER + 1910, 0, 0) != 0; }
  void EnableOvertypeMode (bool State) { SendMessage(m_hWnd, WM_USER + 1900, (WPARAM) (State), 0); }
  CStdString GetWord (CodemaxPos *pPos = NULL);
  CStdString GetText (const CodemaxRange *pRange);
  CStdString GetLine (int nLine);
  void GetPosition (CodemaxPos *Position);
  void SetPosition (const CodemaxPos *Position);
  int GetTabSize (void) { return ((int) SendMessage(m_hWnd, WM_USER + 1822, 0, 0)); }
  void SetTabSize (int TabSize) { SendMessage(m_hWnd, WM_USER + 1821, (WPARAM) (TabSize), 0); }
  int GetUndoLimit (void) { return ((int) SendMessage(m_hWnd, WM_USER + 3410, 0, 0)); }
  void SetUndoLimit (int UndoLimit) { SendMessage(m_hWnd, WM_USER + 3400, (WPARAM) (UndoLimit), 0); }
  bool CanUndo (void) { return SendMessage(m_hWnd, WM_USER + 3300, 0, 0) != 0; }
  bool CanRedo (void) { return SendMessage(m_hWnd, WM_USER + 3310, 0, 0) != 0; }
  bool CanCut (void) { return SendMessage(m_hWnd, WM_USER + 3320, 0, 0) != 0; }
  bool CanCopy (void) { return SendMessage(m_hWnd, WM_USER + 3330, 0, 0) != 0; }
  bool CanPaste (void) { return SendMessage(m_hWnd, WM_USER + 3340, 0, 0) != 0; }
  LRESULT Print (DWORD flags) { return (LRESULT) SendMessage(m_hWnd, WM_USER + 4120, (WPARAM) (NULL), (LPARAM) flags); }
  LRESULT Print (CodemaxPrintEx *cmpex) { return (LRESULT) SendMessage(m_hWnd, WM_USER + 4125, NULL, (LPARAM) cmpex); }

  bool IsTabExpandEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1812, 0, 0) != 0; }
  void EnableTabExpand (bool State) { SendMessage(m_hWnd, WM_USER + 1811, (WPARAM) (State), 0); }
  void EnableColorSyntax (bool State) { SendMessage(m_hWnd, WM_USER + 1610, (WPARAM) (State), 0); }
  bool IsColorSyntaxEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1620, 0, 0) != 0; }
  void EnableWhitespaceDisplay (bool State) { SendMessage(m_hWnd, WM_USER + 1800, (WPARAM) (State), 0); }
  bool IsWhitespaceDisplayEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1810, 0, 0) != 0; }
  void EnableSmoothScrolling (bool State) { SendMessage(m_hWnd, WM_USER + 1820, (WPARAM) (State), 0); }
  bool IsSmoothScrollingEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1830, 0, 0) != 0; }
  void EnableLineToolTips (bool State) { SendMessage(m_hWnd, WM_USER + 1860, (WPARAM) (State), 0); }
  bool IsLineToolTipsEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1870, 0, 0) != 0; }
  void EnableLeftMargin (bool State) { SendMessage(m_hWnd, WM_USER + 1880, (WPARAM) (State), 0); }
  bool IsLeftMarginEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1890, 0, 0) != 0; }
  void EnableOvertype (bool State) { SendMessage(m_hWnd, WM_USER + 1900, (WPARAM) (State), 0); }
  bool IsOvertypeEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1910, 0, 0) != 0; }
  void EnableCaseSensitive (bool State) { SendMessage(m_hWnd, WM_USER + 1920, (WPARAM) (State), 0); }
  bool IsCaseSensitiveEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1930, 0, 0) != 0; }
  void EnablePreserveCase (bool State) { SendMessage(m_hWnd, WM_USER + 1931, (WPARAM) (State), 0); }
  bool IsPreserveCaseEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1932, 0, 0) != 0; }
  void EnableWholeWord (bool State) { SendMessage(m_hWnd, WM_USER + 1940, (WPARAM) (State), 0); }
  bool IsWholeWordEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1950, 0, 0) != 0; }
  void EnableRegExp (bool State) { SendMessage(m_hWnd, WM_USER + 3800, (WPARAM) (State), 0); }
  bool IsRegExpEnabled (void) { return SendMessage(m_hWnd, WM_USER + 3810, 0, 0) != 0; }
  void EnableCRLF (bool State) { SendMessage(m_hWnd, WM_USER + 2470, (WPARAM) (State), 0); }
  bool IsCRLFEnabled (void) { return SendMessage(m_hWnd, WM_USER + 2480, 0, 0) != 0; }
  void EnableDragDrop (bool State) { SendMessage(m_hWnd, WM_USER + 1893, (WPARAM) (State), 0); }
  bool IsDragDropEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1894, 0, 0) != 0; }
  void EnableColumnSel (bool State) { SendMessage(m_hWnd, WM_USER + 1891, (WPARAM) (State), 0); }
  bool IsColumnSelEnabled (void) { return SendMessage(m_hWnd, WM_USER + 1892, 0, 0) != 0; }
  void EnableGlobalProps (bool State) { SendMessage(m_hWnd, WM_USER + 3740, (WPARAM) (State), 0); }
  bool IsGlobalPropsEnabled (void) { return ((LRESULT) SendMessage(m_hWnd, WM_USER + 3741, 0, 0)) != 0; }
  void EnableSelBounds (bool State) { SendMessage(m_hWnd, WM_USER + 3760, (WPARAM) (State), 0); }
  bool IsSelBoundsEnabled (void) { return SendMessage(m_hWnd, WM_USER + 3770, 0, 0) != 0; }
  void EnableHideSel (bool State) { SendMessage(m_hWnd, WM_USER + 3930, (WPARAM) (State), 0); }
  bool IsHideSelEnabled (void) { return SendMessage(m_hWnd, WM_USER + 3940, 0, 0) != 0; }
  void EnableOvertypeCaret (bool State) { SendMessage(m_hWnd, WM_USER + 4010, (WPARAM) (State), 0); }
  bool IsOvertypeCaretEnabled (void) { return SendMessage(m_hWnd, WM_USER + 4020, 0, 0) != 0; }
  void SetReadOnly (bool State) { SendMessage(m_hWnd, WM_USER + 1840, (WPARAM) (State), 0); }
  bool IsReadOnly (void) { return SendMessage(m_hWnd, WM_USER + 1850, 0, 0) != 0; }
  bool IsHSplitterEnabled (void) { return SendMessage(m_hWnd, WM_USER + 3730, (WPARAM) 1, 0) != 0; }
  void EnableHSplitter (bool State) { SendMessage(m_hWnd, WM_USER + 3720, (WPARAM) 1, (LPARAM) State); }
  bool IsVSplitterEnabled (void) { return SendMessage(m_hWnd, WM_USER + 3730, (WPARAM) 0, 0) != 0; }
  void EnableVSplitter (bool State) { SendMessage(m_hWnd, WM_USER + 3720, (WPARAM) 0, (LPARAM) State); }

  static int LookupHotKey (CodemaxHotkey *hotkey) { return CMLookupHotKey (hotkey); }
  static LRESULT RegisterCommand (WORD wCmd, LPCTSTR pszName, LPCTSTR pszDesc) { return CMRegisterCommand (wCmd, pszName, pszDesc); }
  static LRESULT UnregisterCommand (WORD wCmd) { return CMUnregisterCommand (wCmd); }
  static LRESULT RegisterLanguage (char *LangName, CodemaxLanguage *LangDef) { return CMRegisterLanguage (LangName, LangDef); }
  static int GetHotKeysForCmd (WORD Command, CodemaxHotkey *HotKeys) { return CMGetHotKeysForCmd (Command, HotKeys); }
  static LRESULT RegisterHotKey (CodemaxHotkey *HotKey, WORD Command) { return CMRegisterHotKey (HotKey, Command); }
  static LRESULT SetHotKeys (char *HotKeys);
  static void SetDefaultHotKeys (void);
  static LRESULT SetMacro (int Index, char *Macro);
  static void SetFindReplaceMRUList (LPCTSTR List, bool IsFind);
  static int GetHotKeys (char *HotKeys);
  static int GetMacro (int Index, char *Macro);
  static CStdString GetFindReplaceMRUList (bool IsFind);
  static CStdString GetHotKeyString (CodemaxHotkey &cmHotKey);
  static void GetConfigFromCommonSettings (struct _EditConfigStruct *ec);
  static LRESULT CALLBACK StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
