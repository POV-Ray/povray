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

#ifndef POVRAY_CPUID_H
#define POVRAY_CPUID_H

#include "syspovconfigbase.h"

/// Test whether SSE2 is supported by both CPU and OS.
bool HaveSSE2();

/// Test whether AVX is supported by both CPU and OS.
bool HaveAVX();

/// Test whether AVX2 is supported by both CPU and OS.
bool HaveAVX2();

/// Test whether AVX and FMA4 are supported by both CPU and OS.
bool HaveAVXFMA4();

/// Test whether AVX2 and FMA3 are supported by both CPU and OS.
bool HaveAVX2FMA3();

#endif // POVRAY_CPUID_H
