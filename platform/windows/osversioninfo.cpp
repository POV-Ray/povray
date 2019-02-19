//******************************************************************************
///
/// @file platform/windows/osversioninfo.cpp
///
/// Implementation of functions to detect the Windows version at run-time.
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

#include "osversioninfo.h"

#include <windows.h>

namespace pov_base
{

WindowsVersionDetector::WindowsVersionDetector ()
{
    mVersionInfo.dwOSVersionInfoSize = sizeof (mVersionInfo);
    GetVersionEx (&mVersionInfo);
}

bool WindowsVersionDetector::IsNT () const
{
    return (mVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT);
}

bool WindowsVersionDetector::IsVersion (int major, int minor) const
{
    if (mVersionInfo.dwMajorVersion != major)
        return (mVersionInfo.dwMajorVersion >= major);
    else
        return (mVersionInfo.dwMinorVersion >= minor);
}

}
// end of namespace pov_base
