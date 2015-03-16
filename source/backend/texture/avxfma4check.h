//******************************************************************************
///
/// @file backend/texture/avxfma4check.h
///
/// This header file contains code to test whether the CPU supports the AVX and
/// FMA4 instruction set.
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

#ifndef __AVXFMA4_H__
#define __AVXFMA4_H__

/*****************************************************************************
*
* FUNCTION
*
*   CPU_FMA4_DETECT
*
* INPUT
*
*  None
*
* OUTPUT
*
* RETURNS
*
*   Returns the status of the AVX and FMA4 bits
*
* AUTHOR
*
*   AMD
*
* DESCRIPTION
*
* CHANGES
*
*
******************************************************************************/
#define CPUID __cpuid

#if defined(USE_AVX_FMA4_FOR_NOISE)

#if defined(LINUX)
#include <x86intrin.h>

static void __cpuid(int *out, int in) __attribute__((noinline));
static void __cpuid(int *out, int in)
{
    __asm__ __volatile__("    pushq %%rbx;          \
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

static int CPU_FMA4_DETECT(void)
{
    int avx = 0, fma4 = 0;
    int info[4];
    CPUID(info, 0x1);

    if((info[2] & (0x1 << 27)) ? 1 : 0)  /* check for osxsave*/
    {
        avx = (info[2] & (0x1 << 28)) ? 1 : 0;
        CPUID(info, 0x80000001);
        fma4 = (info[2] & (1 << 16)) ? 1 : 0;
        return (avx & fma4);
    }
    else
    {
        return 0;
    }
}

#endif

#endif //
