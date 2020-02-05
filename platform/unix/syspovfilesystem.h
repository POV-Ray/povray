//******************************************************************************
///
/// @file platform/unix/syspovfilesystem.h
///
/// Unix-specific declarations related to file system services.
///
/// @note
///     Most of @ref platform/unix/syspovfilesystem.cpp is declared in
///     @ref base/filesystem.h. This file contains only a few extra
///     platform-specific declarations.
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

#ifndef POVRAY_UNIX_SYSPOVFILESYSTEM_H
#define POVRAY_UNIX_SYSPOVFILESYSTEM_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/stringtypes.h"

namespace pov_base
{

namespace Filesystem
{

#if !POV_USE_DEFAULT_TEMPORARYFILE

void SetTempFilePath(const UCS2String& tempPath);

#endif // POV_USE_DEFAULT_TEMPORARYFILE

}
// end of namespace Filesystem

}
// end of namespace pov_base

#endif // POVRAY_UNIX_SYSPOVFILESYSTEM_H
