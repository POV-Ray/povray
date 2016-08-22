//******************************************************************************
///
/// @file platform/x86/cpuid.h
///
/// This file contains declarations related to probing the capabilities of the
/// CPU.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_CPUID_H
#define POVRAY_CPUID_H

#include "syspovconfigbase.h"

#if defined(LINUX)
#define CPUID cpuid
#else
#define CPUID __cpuid // TODO it's a bit naive to presume this intrinsic to exist on any non-Linux platform
#endif

#define CPUID_00000001_OSXSAVE_MASK    (0x1 << 27)
#define CPUID_00000001_AVX_MASK        (0x1 << 28)
#define CPUID_80000001_FMA4_MASK       (0x1 << 16)

/// Tests whether AVX and FMA4 are supported.
bool HaveAVXFMA4();

#endif // POVRAY_CPUID_H
