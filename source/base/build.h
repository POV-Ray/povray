//******************************************************************************
///
/// @file base/build.h
///
/// POV-Ray build-specific information.
///
/// This header file contains macros specifying information for a specific
/// POV-Ray build, and is intended to be customized by people building their own
/// POV-Ray binaries, while generally remaining unchanged in the official source
/// code repository.
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

#ifndef POVRAY_BASE_BUILD_H
#define POVRAY_BASE_BUILD_H

#include "base/configbase.h"

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

//******************************************************************************
///
/// @name Build-Specific Information
///
/// @{

// Placeholders for macros injected during automated builds - do not remove or change!
//{POV_AUTOBUILD_A}
//{POV_AUTOBUILD_B}
//{POV_AUTOBUILD_C}
//{POV_AUTOBUILD_1}
//{POV_AUTOBUILD_2}
//{POV_AUTOBUILD_3}

/// @def BUILT_BY
/// Specifies the person or organization responsible for this build.
/// @attention Please set this to your real name, and/or include a working email or website address to contact you.
#ifndef BUILT_BY
#define BUILT_BY "YOUR NAME (YOUR EMAIL)"
#error "Please fill in BUILT_BY, then remove this line"
#endif

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_BASE_BUILD_H
