//******************************************************************************
///
/// @file platform/x86/cpuid.cpp
///
/// This file contains code for probing the capabilities of the CPU.
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

#include "cpuid.h"

#if defined(LINUX)
#include <x86intrin.h>

static void cpuid(int *out, int in) __attribute__((noinline));
static void cpuid(int *out, int in)
{
    __asm__ __volatile__("pushq %%rbx;              \
                          xorq %%rax, %%rax;        \
                          movl %%esi, %%eax;        \
                          cpuid;                    \
                          movl %%eax, 0x0(%%rdi);   \
                          movl %%ebx, 0x4(%%rdi);   \
                          movl %%ecx, 0x8(%%rdi);   \
                          movl %%edx, 0xc(%%rdi);   \
                          popq %%rbx;"              \
                          : : "D" (out), "S" (in)   \
                          : "%rax", "%rcx", "%rdx" );
}
#endif

bool HaveAVXFMA4()
{
    int info[4];
    CPUID(info, 0x1);
    bool osxsave = ((info[2] & CPUID_00000001_OSXSAVE_MASK) != 0);
    bool avx     = ((info[2] & CPUID_00000001_AVX_MASK)     != 0);
    CPUID(info, 0x80000001);
    bool fma4    = ((info[2] & CPUID_80000001_FMA4_MASK)    != 0);
    return (osxsave && avx && fma4);
}
