//******************************************************************************
///
/// @file windows/cmedit/settings.h
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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define MAX_OLDER_FILES           32
#define MAX_KEYWORDS              1024
#define CMD_NEXT_KEYWORD          (CODEMAX_USER_BASE)
#define CMD_PREV_KEYWORD          (CODEMAX_USER_BASE + 1)

#define DEF_CLRWINDOW             CLR_INVALID
#define DEF_CLRLEFTMARGIN         RGB (0, 255, 255)
#define DEF_CLRBOOKMARK           CLR_INVALID
#define DEF_CLRBOOKMARKBK         CLR_INVALID
#define DEF_CLRTEXT               RGB (0, 0, 0)
#define DEF_CLRTEXTBK             CLR_INVALID
#define DEF_CLRNUMBER             RGB (0, 128, 128)
#define DEF_CLRNUMBERBK           CLR_INVALID
#define DEF_CLRKEYWORD            RGB (128, 0, 128)
#define DEF_CLRKEYWORDBK          CLR_INVALID
#define DEF_CLROPERATOR           RGB (255, 0, 0)
#define DEF_CLROPERATORBK         CLR_INVALID
#define DEF_CLRSCOPEKEYWORD       RGB (0, 0, 255)
#define DEF_CLRSCOPEKEYWORDBK     CLR_INVALID
#define DEF_CLRCOMMENT            RGB (0, 128, 0)
#define DEF_CLRCOMMENTBK          CLR_INVALID
#define DEF_CLRSTRING             RGB (255, 0, 0)
#define DEF_CLRSTRINGBK           CLR_INVALID
#define DEF_CLRTAGTEXT            RGB (0, 0, 0)
#define DEF_CLRTAGTEXTBK          CLR_INVALID
#define DEF_CLRTAGENT             RGB (255, 0, 0)
#define DEF_CLRTAGENTBK           CLR_INVALID
#define DEF_CLRTAGELEMNAME        RGB (128, 0, 0)
#define DEF_CLRTAGELEMNAMEBK      CLR_INVALID
#define DEF_CLRTAGATTRNAME        RGB (0, 0, 255)
#define DEF_CLRTAGATTRNAMEBK      CLR_INVALID
#define DEF_CLRLINENUMBER         RGB (0, 0, 0)
#define DEF_CLRLINENUMBERBK       RGB (255, 255, 255)
#define DEF_CLRHDIVIDERLINES      CLR_INVALID
#define DEF_CLRVDIVIDERLINES      CLR_INVALID
#define DEF_CLRHIGHLIGHTEDLINE    RGB (255, 255, 0)

#define CJC_CLRWINDOW             RGB (0, 0, 0)
#define CJC_CLRLEFTMARGIN         RGB (128, 0, 0)
#define CJC_CLRBOOKMARK           RGB (255, 255, 0)
#define CJC_CLRBOOKMARKBK         CLR_INVALID
#define CJC_CLRTEXT               RGB (255, 255, 255)
#define CJC_CLRTEXTBK             CLR_INVALID
#define CJC_CLRNUMBER             RGB (0, 255, 0)
#define CJC_CLRNUMBERBK           CLR_INVALID
#define CJC_CLRKEYWORD            RGB (0, 255, 255)
#define CJC_CLRKEYWORDBK          CLR_INVALID
#define CJC_CLROPERATOR           RGB (0, 255, 255)
#define CJC_CLROPERATORBK         CLR_INVALID
#define CJC_CLRSCOPEKEYWORD       RGB (0, 255, 0)
#define CJC_CLRSCOPEKEYWORDBK     CLR_INVALID
#define CJC_CLRCOMMENT            RGB (255, 255, 0)
#define CJC_CLRCOMMENTBK          CLR_INVALID
#define CJC_CLRSTRING             RGB (255, 255, 0)
#define CJC_CLRSTRINGBK           CLR_INVALID
#define CJC_CLRTAGTEXT            RGB (0, 255, 255)
#define CJC_CLRTAGTEXTBK          CLR_INVALID
#define CJC_CLRTAGENT             RGB (255, 0, 0)
#define CJC_CLRTAGENTBK           CLR_INVALID
#define CJC_CLRTAGELEMNAME        RGB (255, 255, 0)
#define CJC_CLRTAGELEMNAMEBK      CLR_INVALID
#define CJC_CLRTAGATTRNAME        RGB (0, 255, 255)
#define CJC_CLRTAGATTRNAMEBK      CLR_INVALID
#define CJC_CLRLINENUMBER         RGB (255, 255, 255)
#define CJC_CLRLINENUMBERBK       RGB (0, 0, 0)
#define CJC_CLRHDIVIDERLINES      CLR_INVALID
#define CJC_CLRVDIVIDERLINES      CLR_INVALID
#define CJC_CLRHIGHLIGHTEDLINE    RGB (0, 0, 255)

#define DEF_FSKEYWORD             CODEMAX_FONT_NORMAL
#define DEF_FSCOMMENT             CODEMAX_FONT_NORMAL
#define DEF_FSOPERATOR            CODEMAX_FONT_NORMAL
#define DEF_FSSCOPEKEYWORD        CODEMAX_FONT_NORMAL
#define DEF_FSSTRING              CODEMAX_FONT_NORMAL
#define DEF_FSTEXT                CODEMAX_FONT_NORMAL
#define DEF_FSNUMBER              CODEMAX_FONT_NORMAL
#define DEF_FSTAGTEXT             CODEMAX_FONT_NORMAL
#define DEF_FSTAGENT              CODEMAX_FONT_NORMAL
#define DEF_FSTAGELEMNAME         CODEMAX_FONT_NORMAL
#define DEF_FSTAGATTRNAME         CODEMAX_FONT_NORMAL
#define DEF_FSLINENUMBER          CODEMAX_FONT_NORMAL

class CRegDef
{
public:
  CStdString m_Path ;

public:
  CRegDef (LPCSTR Path) ;
  ~CRegDef () ;
  int ReadInt (LPCSTR Name, int defval) ;
  bool ReadBool (LPCSTR Name, bool defval) ;
  int ReadBin (LPCSTR Name, char *data, int len, char *defval) ;
  int ReadBin (LPCSTR Name, char **data, int len, char *defval) ;
  LPCTSTR ReadString (LPCSTR Name, LPCTSTR defval) ;
  bool WriteInt (LPCSTR Name, int val) ;
  bool WriteBool (LPCSTR Name, bool val) ;
  bool WriteBin (LPCSTR Name, char *data, int len) ;
  bool WriteString (LPCSTR Name, LPCTSTR data) ;
  bool DeleteValue (LPCSTR Name) ;

  bool IsOK (void) { return m_Result ; }

protected:
  HKEY m_hKey ;
  bool m_Result ;
} ;

typedef struct _EditConfigStruct
{
  TAutoIndent           AutoIndent ;
  bool                  SyntaxHighlighting ;
  bool                  WhiteSpaceDisplay ;
  bool                  TabExpand ;
  bool                  SmoothScrolling ;
  bool                  LineToolTips ;
  bool                  LeftMarginVisible ;
  bool                  CaseSensitive ;
  bool                  PreserveCase ;
  bool                  WholeWordEnabled ;
  bool                  DragDropEnabled ;
  bool                  HSplitterEnabled ;
  bool                  VSplitterEnabled ;
  bool                  ColumnSelEnabled ;
  bool                  RegexpEnabled ;
  bool                  OvertypeCaret ;
  bool                  SelBoundsEnabled ;
  bool                  TabKeywordExpansion ;
  TScrollStyle          ScrollBars ;
  int                   TabSize ;
  int                   UndoLimit ;
  CodemaxColors         Colours ;
  CodemaxFontStyles     FontStyles ;
  HFONT                 HFont ;
  char                  *HotKeys ;
  int                   HotKeyLen ;
  CStdString            FindMRUList ;
  CStdString            ReplaceMRUList ;
  char                  *Macros [CODEMAX_MACRO_LIMIT] ;
  int                   MacroLen [CODEMAX_MACRO_LIMIT] ;
} EditConfigStruct ;

CStdString GetNextField (CStdString& str) ;
CStdString GetField (CStdString str, int FieldNo = 0) ;
CStdString EncodeFilename(CStdString str);
CStdString DecodeFilename(CStdString str);
void GetSettings (CRegDef *t, EditConfigStruct *ec, bool RestoreFiles) ;
void PutSettings (CRegDef *t, EditConfigStruct *ec) ;

#endif
