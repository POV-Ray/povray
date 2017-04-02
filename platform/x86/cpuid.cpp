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

#include "cpuid.h"

#ifdef MACHINE_INTRINSICS_H
#include MACHINE_INTRINSICS_H
#endif

#if defined(_MSC_VER) // Build environment: MS Visual C++ (or imitation)

#define CPUID __cpuid

#if (_MSC_FULL_VER >= 160040219)
#define GET_XCR0() _xgetbv(_XCR_XFEATURE_ENABLED_MASK)
#else
#error "Don't know how to read XCR0 register in this build environment."
#endif

#elif defined(__linux__) // Build environment: GNU/Linux (presumably GCC or Clang)

#define CPUID cpuid
static void cpuid(int *out, int in) __attribute__((noinline));
static void cpuid(int *out, int in)
{
#if defined(__i386__) // Architecture: x86 (32 bit)
    __asm__ __volatile__("pushl %%ebx;              \
                          xorl %%eax, %%eax;        \
                          xorl %%ecx, %%ecx;        \
                          movl %%esi, %%eax;        \
                          cpuid;                    \
                          movl %%eax, 0x0(%%edi);   \
                          movl %%ebx, 0x4(%%edi);   \
                          movl %%ecx, 0x8(%%edi);   \
                          movl %%edx, 0xc(%%edi);   \
                          popl %%ebx;"              \
                          : : "D" (out), "S" (in)   \
                          : "%eax", "%ecx", "%edx");
#elif defined(__x86_64__) // Architecture: x86-64 (64 bit)
    __asm__ __volatile__("pushq %%rbx;              \
                          xorq %%rax, %%rax;        \
                          xorq %%rcx, %%rcx;        \
                          movl %%esi, %%eax;        \
                          cpuid;                    \
                          movl %%eax, 0x0(%%rdi);   \
                          movl %%ebx, 0x4(%%rdi);   \
                          movl %%ecx, 0x8(%%rdi);   \
                          movl %%edx, 0xc(%%rdi);   \
                          popq %%rbx;"              \
                          : : "D" (out), "S" (in)   \
                          : "%rax", "%rcx", "%rdx");
#else // Architecture
#error "Don't know how to invoke CPUID on this target architecture."
#endif // Architecture
}

#define GET_XCR0() getXCR0()
static unsigned long long getXCR0()
{
   int rEAX = 0, rEDX = 0;
    __asm__(".byte 0x0f, 0x01, 0xd0" : "=a" (rEAX), "=d" (rEDX) : "c" (0));
   return ((unsigned long long)(rEDX) << 32) | rEAX;
}

#else // Build environment
#error "Don't know how to invoke CPUID in this build environment."
#endif // Build environment

// Indices into the CPUID result table corresponding to the individual CPU registers.
#define CPUID_EAX 0
#define CPUID_EBX 1
#define CPUID_ECX 2
#define CPUID_EDX 3

// Masks for relevant CPUID result bits.
#define CPUID_00000001_ECX_FMA3_MASK    (0x1 << 12)
#define CPUID_00000001_ECX_OSXSAVE_MASK (0x1 << 27)
#define CPUID_00000001_ECX_AVX_MASK     (0x1 << 28)
#define CPUID_00000001_EDX_SSE2_MASK    (0x1 << 26)
#define CPUID_00000007_EBX_AVX2_MASK    (0x1 <<  5)
#define CPUID_80000001_ECX_FMA4_MASK    (0x1 << 16)

// Masks for relevant XCR0 register bits.
#define XCR0_SSE_MASK (0x1 << 1)
#define XCR0_AVX_MASK (0x1 << 2)

static bool OSSavesSSERegisters()
{
    // Check if the OS will save SSE registers
    unsigned long long xcrFeatureMask = GET_XCR0();
    return ((xcrFeatureMask & XCR0_SSE_MASK) == XCR0_SSE_MASK);
}

static bool OSSavesAVXRegisters()
{
    // Check if the OS will save full YMM registers
    unsigned long long xcrFeatureMask = GET_XCR0();
    return ((xcrFeatureMask & (XCR0_SSE_MASK|XCR0_AVX_MASK)) == (XCR0_SSE_MASK|XCR0_AVX_MASK));
}

bool HaveSSE2()
{
    int info[4];
    // Verify the relevant CPUID leaves are supported
    CPUID(info, 0x0);
    if (info[CPUID_EAX] < 0x1)
        return false;
    // Read the relevant CPUID leaves
    CPUID(info, 0x1);
    bool osxsave = ((info[CPUID_ECX] & CPUID_00000001_ECX_OSXSAVE_MASK) != 0);
    bool sse2    = ((info[CPUID_EDX] & CPUID_00000001_EDX_SSE2_MASK)    != 0);
    // Aggregate the information
    return osxsave && sse2 && OSSavesSSERegisters();
}

bool HaveAVX()
{
    int info[4];
    // Verify the relevant CPUID leaves are supported
    CPUID(info, 0x0);
    if (info[CPUID_EAX] < 0x1)
        return false;
    // Evaluate the relevant CPUID leaves
    CPUID(info, 0x1);
    bool osxsave = ((info[CPUID_ECX] & CPUID_00000001_ECX_OSXSAVE_MASK) != 0);
    bool avx     = ((info[CPUID_ECX] & CPUID_00000001_ECX_AVX_MASK)     != 0);
    // Aggregate the information
    return osxsave && avx && OSSavesAVXRegisters();
}

bool HaveAVX2()
{
    int info[4];
    // Verify the relevant CPUID leaves are supported
    CPUID(info, 0x0);
    if (info[CPUID_EAX] < 0x7)
        return false;
    // Evaluate the relevant CPUID leaves
    CPUID(info, 0x1);
    bool osxsave = ((info[CPUID_ECX] & CPUID_00000001_ECX_OSXSAVE_MASK) != 0);
    bool avx     = ((info[CPUID_ECX] & CPUID_00000001_ECX_AVX_MASK)     != 0);
    CPUID(info, 0x7);
    bool avx2    = ((info[CPUID_EBX] & CPUID_00000007_EBX_AVX2_MASK)    != 0);
    // Aggregate the information
    return osxsave && avx && avx2 && OSSavesAVXRegisters();
}

bool HaveAVXFMA4()
{
    int info[4];
    // Verify the relevant CPUID leaves are supported
    CPUID(info, 0x0);
    if (info[CPUID_EAX] < 0x1)
        return false;
    CPUID(info, 0x80000000);
    if (info[CPUID_EAX] < (int)0x80000001)
        return false;
    // Evaluate the relevant CPUID leaves
    CPUID(info, 0x1);
    bool osxsave = ((info[CPUID_ECX] & CPUID_00000001_ECX_OSXSAVE_MASK) != 0);
    bool avx     = ((info[CPUID_ECX] & CPUID_00000001_ECX_AVX_MASK)     != 0);
    CPUID(info, 0x80000001);
    bool fma4    = ((info[CPUID_ECX] & CPUID_80000001_ECX_FMA4_MASK)    != 0);
    // Aggregate the information
    return osxsave && avx && fma4 && OSSavesAVXRegisters();
}

bool HaveAVX2FMA3()
{
    int info[4];
    // Verify the relevant CPUID leaves are supported
    CPUID(info, 0x0);
    if (info[CPUID_EAX] < 0x7)
        return false;
    // Evaluate the relevant CPUID leaves
    CPUID(info, 0x1);
    bool osxsave = ((info[CPUID_ECX] & CPUID_00000001_ECX_OSXSAVE_MASK) != 0);
    bool avx     = ((info[CPUID_ECX] & CPUID_00000001_ECX_AVX_MASK)     != 0);
    bool fma3    = ((info[CPUID_ECX] & CPUID_00000001_ECX_FMA3_MASK)    != 0);
    CPUID(info, 0x7);
    bool avx2    = ((info[CPUID_EBX] & CPUID_00000007_EBX_AVX2_MASK)    != 0);
    // Aggregate the information
    return osxsave && avx && fma3 && avx2 && OSSavesAVXRegisters();
}
