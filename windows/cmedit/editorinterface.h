//******************************************************************************
///
/// @file windows/cmedit/editorinterface.h
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

#ifndef __EDITORINTERFACE_H__
#define __EDITORINTERFACE_H__

extern "C" __declspec (dllexport) HWND CreateTabWindow (HWND mainWindow, HWND StatusWindow, const char *BinariesPath, const char *DocumentsPath) ;
extern "C" __declspec (dllexport) DWORD GetDLLVersion (void) ;
extern "C" __declspec (dllexport) void SetWindowPosition (int x, int y, int w, int h) ;
extern "C" __declspec (dllexport) void SetMessageWindow (HWND MsgWindow) ;
extern "C" __declspec (dllexport) void RestoreState (int RestoreFiles) ;
extern "C" __declspec (dllexport) void SaveState (void) ;
extern "C" __declspec (dllexport) bool SelectFile (const char *FileName) ;
extern "C" __declspec (dllexport) bool ShowParseError (char *FileName, char *Message, int Line, int Col) ;
extern "C" __declspec (dllexport) bool BrowseFile (bool CreateNewWindow) ;
extern "C" __declspec (dllexport) bool LoadFile (char *FileName) ;
extern "C" __declspec (dllexport) bool ExternalLoadFile (char *ParamString) ;
extern "C" __declspec (dllexport) bool CloseFile (char *FileName) ;
extern "C" __declspec (dllexport) bool SaveFile (char *FileName) ;
extern "C" __declspec (dllexport) DWORD GetTab (void) ;
extern "C" __declspec (dllexport) DWORD GetFlags (void) ;
extern "C" __declspec (dllexport) char *GetFilename (void) ;
extern "C" __declspec (dllexport) void NextTab (bool Forward) ;
extern "C" __declspec (dllexport) bool CanClose (bool AllFiles) ;
extern "C" __declspec (dllexport) bool SaveModified (char *FileName) ;
extern "C" __declspec (dllexport) bool ShowMessages (bool on) ;
extern "C" __declspec (dllexport) void DispatchMenuId (DWORD id) ;
extern "C" __declspec (dllexport) HMENU GetMenuHandle (int which) ;
extern "C" __declspec (dllexport) void SetNotifyBase (HWND WindowHandle, int MessageBase) ;
extern "C" __declspec (dllexport) void UpdateMenus (HMENU MenuHandle) ;
extern "C" __declspec (dllexport) void GetContextHelp (void) ;
extern "C" __declspec (dllexport) void SetTabFocus (void) ;
extern "C" __declspec (dllexport) bool PassOnMessage (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, DWORD *rVal) ;
extern "C" __declspec (dllexport) void SetKeywords (LPCSTR SDLKeywordList, LPCSTR INIKeywords) ;
extern "C" __declspec (dllexport) const char **GetWindowList (void);

void PutStatusMessage (LPCSTR Message) ;
void debug (char *format, ...) ;
void TabIndexChanged (void) ;
void ShowMessage (LPCSTR str, int type = 0) ;
void ShowErrorMessage (CStdString Title, const char *Msg, int ErrorCode = 0) ;
void GetFileTimeFromDisk (LPCSTR Filename, FILETIME& time) ;
bool FileExists (LPCSTR FileName) ;
void MakeFileNames (EditTagStruct *t, LPCSTR str) ;
CStdString GetFilePath (LPCSTR str) ;
CStdString GetFileExt (LPCSTR str) ;
CStdString GetFullPath (LPCSTR str) ;
CStdString GetBaseName (LPCSTR str) ;
CStdString GetFileNameNoExt (LPCSTR str) ;
CStdString FixPath (CStdString Name) ;
CStdString UnquotePath (CStdString Name) ;
int GetFileLength (LPCSTR FileName) ;
void AddToRecent (LPCSTR FileName) ;
void UpdateRecent (void) ;
void ShiftTab(bool left, int index);
CCodeMax *CreateNewEditor (const char *FileName, bool ReadOnly, bool Show, bool IgnoreMissing) ;
bool CloseAll (CCodeMax *except = NULL) ;
bool CloseFile (char *FileName) ;
bool SaveAllFiles (bool IncludeUntitled) ;
void ShowMessagePane (void) ;
bool HaveWin98OrLater (void) ;
bool HaveWin2kOrLater (void) ;
bool HaveWinXPOrLater (void) ;
CCodeMax *FindEditor (LPCSTR FileName) ;
void InsertTab (LPCSTR title) ;
void DeleteTab (int index) ;
void SetWindowPosition (void) ;

#endif
