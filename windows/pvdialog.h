//******************************************************************************
///
/// @file windows/pvdialog.h
///
/// This module implements dialog-box routines for the Windows build of POV.
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

#ifndef PVDIALOG_H_INCLUDED
#define PVDIALOG_H_INCLUDED

namespace povwin
{

INT_PTR CALLBACK PovCommandLineDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
INT_PTR CALLBACK PovShortCommandLineDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
INT_PTR CALLBACK PovFileQueueDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
INT_PTR CALLBACK PovFeatureAdviceDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
INT_PTR CALLBACK PovSoundsDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
INT_PTR CALLBACK PovThreadCountDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
INT_PTR CALLBACK PovStatusPanelDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
INT_PTR CALLBACK RenderAlternativeFileDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void FeatureNotify (const char *labelStr, const char *titleStr, const char *textStr, const char *helpStr, bool checked) ;

}
// end of namespace povwin

#endif // PVDIALOG_H_INCLUDED
