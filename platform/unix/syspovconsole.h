//******************************************************************************
///
/// @file platform/unix/syspovconsole.h
///
/// Unix-specific declaration of the @ref pov_base::GetTerminalWidth() function
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
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

#ifndef POVRAY_UNIX_SYSPOVCONSOLE_H
#define POVRAY_UNIX_SYSPOVCONSOLE_H

#include "base/configbase.h"

namespace pov_base
{

/// Detect the terminal width.
///
/// This Unix-specific function implements the specific (virtual) terminal
/// calls needed to know the width of the current console. This is required by
/// POV-Ray since some diagnostic routines use to wrap text messages at
/// terminal border in a clean way and not let the terminal cut words at any
/// position.
///

unsigned int GetTerminalWidth(void);

}

#endif // POVRAY_UNIX_SYSPOVCONSOLE_H
