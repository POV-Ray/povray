//******************************************************************************
///
/// @file windows/cmedit/cmedit.cpp
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

#pragma comment(lib, "gdiplus.lib")
#include <gdiplus.h>

HINSTANCE     hInstance ;
ULONG_PTR     gdipT ;

void CleanUp (void) ;

using namespace Gdiplus;

BOOL APIENTRY DllMain (HINSTANCE hInstDLL, DWORD reason, LPVOID reserved)
{
  GdiplusStartupInput    gdipSI;

  switch (reason)
  {
    case DLL_PROCESS_ATTACH :
         hInstance = hInstDLL ;
         GdiplusStartup(&gdipT, &gdipSI, NULL);
         break ;

    case DLL_THREAD_ATTACH :
         break ;

    case DLL_THREAD_DETACH :
         break ;

    case DLL_PROCESS_DETACH :
         CleanUp () ;
         GdiplusShutdown(gdipT);
         break ;
  }
  return (true) ;
}
