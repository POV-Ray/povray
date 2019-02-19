//******************************************************************************
///
/// @file platform/x86/cpuid.cpp
///
/// This file contains code for probing the capabilities of the CPU.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "cpuid.h"

// C++ variants of C standard header files
#include <cstring>

// C++ standard header files
#include <vector>

// other 3rd party library header files
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

#elif defined(__GNUC__) // Build environment: GCC (or Clang imitating GCC)

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
#error "Don't know how to invoke CPUID or read XCR0 register in this build environment."
#endif // Build environment

// Indices into the CPUID result table corresponding to the individual CPU registers.
#define CPUID_EAX 0
#define CPUID_EBX 1
#define CPUID_ECX 2
#define CPUID_EDX 3

// Masks for relevant CPUID result bits.
#define CPUID_00000001_ECX_FMA3_MASK    (0x1 << 12)
#define CPUID_00000001_ECX_XSAVE_MASK   (0x1 << 26)
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

enum CPUVendorId
{
    kCPUVendor_Unrecognized = 0,
    kCPUVendor_AMD,
    kCPUVendor_Intel,
    kCPUVendor_VM,
};

struct CPUVendorInfo final
{
    CPUVendorId id;
    const char* cpuidString;
};

CPUVendorInfo gCPUVendorInfo[] = {
    // Common CPU manufacturers.
    { kCPUVendor_AMD,           "AuthenticAMD" },
    { kCPUVendor_Intel,         "GenuineIntel" },
    // Known virtual machines.
    { kCPUVendor_VM,            "Microsoft Hv" },   // Microsoft Hyper-V or Windows Virtual PC
    { kCPUVendor_VM,            " lrpepyh vr"  },   // Parallels
    { kCPUVendor_VM,            "KVMKVMKVM"    },   // KVM
    { kCPUVendor_VM,            "VMwareVMware" },   // VMWare
    { kCPUVendor_VM,            "XenVMMXenVMM" },   // Xen HVM
    // End of list.
    { kCPUVendor_Unrecognized,  nullptr }
};

struct CPUIDInfo final
{
    CPUVendorId vendorId;
    bool        xsave  : 1;
    bool        osxsave: 1;
    bool        sse2   : 1;
    bool        avx    : 1;
    bool        avx2   : 1;
    bool        fma3   : 1;
    bool        fma4   : 1;
#if POV_CPUINFO_DEBUG
    char        vendor[13];
#endif
    CPUIDInfo();
};

CPUIDInfo::CPUIDInfo() :
    xsave(false),
    osxsave(false),
    sse2(false),
    avx(false),
    avx2(false),
    fma3(false),
    fma4(false),
    vendorId(kCPUVendor_Unrecognized)
{
    int info[4];
    CPUID(info, 0x0);
#if !POV_CPUINFO_DEBUG
    char vendor[13];
#endif
    std::memcpy(vendor,     &info[CPUID_EBX], 4);
    std::memcpy(vendor + 4, &info[CPUID_EDX], 4);
    std::memcpy(vendor + 8, &info[CPUID_ECX], 4);
    vendor[12] = '\0';
    for (CPUVendorInfo* p = gCPUVendorInfo; p->cpuidString != nullptr; ++p)
    {
        if (strcmp(vendor, p->cpuidString) == 0)
        {
            vendorId = p->id;
            break;
        }
    }
    int maxLeaf = info[CPUID_EAX];
    if (maxLeaf >= 0x1)
    {
        CPUID(info, 0x1);
        fma3    = ((info[CPUID_ECX] & CPUID_00000001_ECX_FMA3_MASK)    != 0);
        xsave   = ((info[CPUID_ECX] & CPUID_00000001_ECX_XSAVE_MASK)   != 0);
        osxsave = ((info[CPUID_ECX] & CPUID_00000001_ECX_OSXSAVE_MASK) != 0);
        avx     = ((info[CPUID_ECX] & CPUID_00000001_ECX_AVX_MASK)     != 0);
        sse2    = ((info[CPUID_EDX] & CPUID_00000001_EDX_SSE2_MASK)    != 0);
    }
    if (maxLeaf >= 0x7)
    {
        CPUID(info, 0x7);
        avx2    = ((info[CPUID_EBX] & CPUID_00000007_EBX_AVX2_MASK)    != 0);
    }
    CPUID(info, 0x80000000);
    int maxLeafExt = info[CPUID_EAX];
    if (maxLeafExt >= (int)0x80000001)
    {
        CPUID(info, 0x80000001);
        fma4    = ((info[CPUID_ECX] & CPUID_80000001_ECX_FMA4_MASK)    != 0);
    }
}

struct OSInfo final
{
    bool xcr0_sse : 1;
    bool xcr0_avx : 1;
    OSInfo(const CPUIDInfo& cpuinfo);
};

OSInfo::OSInfo(const CPUIDInfo& cpuinfo) :
    xcr0_sse(false),
    xcr0_avx(false)
{
    if (cpuinfo.xsave && cpuinfo.osxsave)
    {
        unsigned long long xcrFeatureMask = GET_XCR0();
        xcr0_sse = ((xcrFeatureMask & XCR0_SSE_MASK) != 0);
        xcr0_avx = ((xcrFeatureMask & XCR0_AVX_MASK) != 0);
    }
}

/// @warning
///     _Do not re-order the members of this structure!_
///     @par
///     The members of this data structure must be initialized in a particular order,
///     which according to the C++ standard (and contrary to naive expectations)
///     does _not_ depend on the order of member-initializers in the constructor,
///     but rather on the _order of declaration_.
///
struct CPUInfo::Data final
{
    CPUIDInfo   cpuidInfo;
    OSInfo      osInfo;
    Data() : cpuidInfo(), osInfo(cpuidInfo) {}
};

bool CPUInfo::SupportsSSE2()
{
    return gpData->cpuidInfo.osxsave
        && gpData->cpuidInfo.sse2
        && gpData->osInfo.xcr0_sse;
}

bool CPUInfo::SupportsAVX()
{
    return gpData->cpuidInfo.osxsave
        && gpData->cpuidInfo.avx
        && gpData->osInfo.xcr0_sse
        && gpData->osInfo.xcr0_avx;
}

bool CPUInfo::SupportsAVX2()
{
    return gpData->cpuidInfo.osxsave
        && gpData->cpuidInfo.avx
        && gpData->cpuidInfo.avx2
        && gpData->osInfo.xcr0_sse
        && gpData->osInfo.xcr0_avx;
}

bool CPUInfo::SupportsFMA3()
{
    return gpData->cpuidInfo.fma3;
}

bool CPUInfo::SupportsFMA4()
{
    return gpData->cpuidInfo.fma4;
}

bool CPUInfo::IsIntel()
{
    return gpData->cpuidInfo.vendorId == kCPUVendor_Intel;
}

bool CPUInfo::IsAMD()
{
    return gpData->cpuidInfo.vendorId == kCPUVendor_AMD;
}

bool CPUInfo::IsVM()
{
    return gpData->cpuidInfo.vendorId == kCPUVendor_VM;
}

std::string CPUInfo::GetFeatures()
{
    std::vector<const char*> features;

    if (IsAMD())
        features.push_back("AMD");
    else if (IsIntel())
        features.push_back("Intel");
    else if (IsVM())
        features.push_back("VM");

    if (SupportsSSE2())
        features.push_back("SSE2");
    if (SupportsAVX())
        features.push_back("AVX");
    if (SupportsAVX2())
        features.push_back("AVX2");
    if (SupportsFMA3())
        features.push_back("FMA3");
    if (SupportsFMA4())
        features.push_back("FMA4");

    std::string result;
    for (std::vector<const char*>::const_iterator i = features.begin(); i != features.end(); ++i)
    {
        if (!result.empty())
            result.append(",");
        result.append(*i);
    }

    return result;
}

#if POV_CPUINFO_DEBUG
std::string CPUInfo::GetDetails()
{
    std::vector<std::string> cpuidFeatures;

    std::string vendor = gpData->cpuidInfo.vendor;
    cpuidFeatures.push_back("'" + vendor + "'");
    if (gpData->cpuidInfo.avx)
        cpuidFeatures.push_back("AVX");
    if (gpData->cpuidInfo.avx2)
        cpuidFeatures.push_back("AVX2");
    if (gpData->cpuidInfo.fma3)
        cpuidFeatures.push_back("FMA");
    if (gpData->cpuidInfo.fma4)
        cpuidFeatures.push_back("FMA4");
    if (gpData->cpuidInfo.osxsave)
        cpuidFeatures.push_back("OSXSAVE");
    if (gpData->cpuidInfo.sse2)
        cpuidFeatures.push_back("SSE2");
    if (gpData->cpuidInfo.xsave)
        cpuidFeatures.push_back("XSAVE");

    std::vector<std::string> xcr0Features;

    if (gpData->osInfo.xcr0_avx)
        xcr0Features.push_back("AVX");
    if (gpData->osInfo.xcr0_sse)
        xcr0Features.push_back("SSE");

    std::string cpuidResult;
    for (std::vector<std::string>::const_iterator i = cpuidFeatures.begin(); i != cpuidFeatures.end(); ++i)
    {
        if (!cpuidResult.empty())
            cpuidResult.append(",");
        cpuidResult.append(*i);
    }

    std::string xcr0Result;
    for (std::vector<std::string>::const_iterator i = xcr0Features.begin(); i != xcr0Features.end(); ++i)
    {
        if (!xcr0Result.empty())
            xcr0Result.append(",");
        xcr0Result.append(*i);
    }

    return "CPUID:" + cpuidResult + ";XCR0:" + xcr0Result;
}
#endif

const CPUInfo::Data* CPUInfo::gpData(new CPUInfo::Data);
