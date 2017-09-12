//******************************************************************************
///
/// @file windows/cmedit/dialogs.h
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

#ifndef __DIALOGS_H__
#define __DIALOGS_H__

#ifndef MB_CANCELTRYCONTINUE
#define MB_CANCELTRYCONTINUE      6
#define IDTRYAGAIN                10
#define IDCONTINUE                11
#endif

#ifndef SM_CMONITORS
#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define SM_CMONITORS            80
#endif

#define DONTASKAGAINFLAG        0x80000000

int ShowFileChangedDialog (char *FileName, bool HasSaveAll = true) ;
int ShowReloadDialog (char *FileName) ;
int ShowSaveBeforeRenderDialog (char *FileName) ;
int ShowEnterValueDialog (char *caption, unsigned short min, unsigned short max, unsigned int initial) ;
UINT_PTR CALLBACK PageSetupHook (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
UINT_PTR CALLBACK PrintHook (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;

#endif
