//******************************************************************************
///
/// @file platform/windows/osversioninfo.h
///
/// Declaration of functions to detect the Windows version at run-time.
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

#ifndef POVRAY_WINDOWS_OSVERSIONINFO_H
#define POVRAY_WINDOWS_OSVERSIONINFO_H

#include "base/configbase.h"

#include <windows.h>

namespace pov_base
{

class WindowsVersionDetector final
{
    public:

        WindowsVersionDetector();

        /// Test whether the operating system uses NT technology.
        ///
        /// This function returns `true` if the Windows version is based on the NT operating system
        /// architecture, which is the case for genuine Windows NT products, Windows 2000, and all
        /// newer products of both the server and client lines.
        ///
        bool IsNT () const;

        /// Test whether the operating system matches a given version number.
        ///
        /// This function returns `true` if the internal Windows version number is equal to or
        /// higher than the version number specified.
        ///
        bool IsVersion (int major, int minor) const;

        /// Test whether the operating system uses NT technology matching a given version number.
        ///
        inline bool IsNTVersion (int major, int minor) const { return IsNT() && IsVersion(4,0); }

    private:

        OSVERSIONINFO mVersionInfo;
};

}
// end of namespace pov_base

#endif // POVRAY_WINDOWS_OSVERSIONINFO_H
