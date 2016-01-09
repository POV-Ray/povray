//******************************************************************************
///
/// @file windows/povconfig/syspovconfig_msvc.h
///
/// MSVC compiler-specific POV-Ray compile-time configuration.
///
/// This header file configures aspects of POV-Ray for compiling properly in a
/// Microsoft Visual C++ build environment.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_WINDOWS_SYSPOVCONFIG_MSVC_H
#define POVRAY_WINDOWS_SYSPOVCONFIG_MSVC_H

#if _MSC_VER < 1400
  #error minimum Visual C++ version supported is 14.0 (supplied with VS 2005)
#endif

#undef TRY_OPTIMIZED_NOISE
#if _MSC_VER >= 1600
  #define TRY_OPTIMIZED_NOISE
  #define OPTIMIZED_NOISE_SUPPORTED (AVXFMA4NoiseSupported())
  #define OPTIMIZED_NOISE           AVXFMA4Noise
  #define OPTIMIZED_DNOISE          AVXFMA4DNoise
  #define OPTIMIZED_NOISE_H         "avxfma4noise.h"
#endif

#include <cstdio>
#include <direct.h>
#include <intrin.h>

#pragma auto_inline(on)
#pragma warning(disable : 4018) /* signed/unsigned mismatch */
#pragma warning(disable : 4305) /* truncation from 'type1' to 'type2' (mostly double to float) */
#pragma warning(disable : 4244) /* possible loss of data (converting ints to shorts) */

#ifdef __INTEL_COMPILER

  #error Intel C++ compiler currently not supported.
  // Compiling POV-Ray for Windows using the Intel C++ compiler has not been tested for a long time, and the following
  // settings are probably outdated. You may proceed at your own risk by removing the above line, but be prepared to run
  // into problems further down the road.

  #pragma warning(disable : 1899) /* multicharacter character literal */

  #if __INTEL_COMPILER < 1010
    #error minimum Intel C++ version supported is 10.1
  #endif

  #if __INTEL_COMPILER >= 1000 && __INTEL_COMPILER < 1100
    #define COMPILER_VER                      ".icl10"
    #define METADATA_COMPILER_STRING          "icl 10"
  #elif __INTEL_COMPILER >= 1100 && __INTEL_COMPILER < 1200
    #define COMPILER_VER                      ".icl11"
    #define METADATA_COMPILER_STRING          "icl 11"
  #else
    #error Please update syspovconfig_msvc.h to include this version of ICL
  #endif
  #define COMPILER_NAME                       "Intel C++ Compiler"
  #define COMPILER_VERSION                    __INTEL_COMPILER

  #ifdef BUILD_SSE2
    #define METADATA_X86_FPU_STRING           "-sse2"
  #else
    #define METADATA_X86_FPU_STRING           ""
  #endif

#else

  #pragma inline_recursion(on)
  #pragma inline_depth(255)

  #if _MSC_VER >= 1400 && _MSC_VER < 1500 && !defined (_WIN64)
    // MS Visual C++ 2005 (aka 8.0), compiling for 32 bit target
    #define COMPILER_VER                      ".msvc8"
    #define METADATA_COMPILER_STRING          "msvc 8"
    #define NEED_INVHYP
  #elif _MSC_VER >= 1400 && _MSC_VER < 1500 && defined (_WIN64)
    // MS Visual C++ 2005 (aka 8.0), compiling for 64 bit target
    #define COMPILER_VER                      ".msvc8"
    #define METADATA_COMPILER_STRING          "msvc 8"
    #define ALIGN16                           __declspec(align(16))
    inline const int& max(const int& _X, const int& _Y) {return (_X < _Y ? _Y : _X); }
    inline const int& min(const int& _X, const int& _Y) {return (_Y < _X ? _Y : _X); }
    inline const unsigned int& max(const unsigned int& _X, const unsigned int& _Y) {return (_X < _Y ? _Y : _X); }
    inline const unsigned int& min(const unsigned int& _X, const unsigned int& _Y) {return (_Y < _X ? _Y : _X); }
    inline const long& max(const long& _X, const long& _Y) {return (_X < _Y ? _Y : _X); }
    inline const long& min(const long& _X, const long& _Y) {return (_Y < _X ? _Y : _X); }
    inline const unsigned long& max(const unsigned long& _X, const unsigned long& _Y) {return (_X < _Y ? _Y : _X); }
    inline const unsigned long& min(const unsigned long& _X, const unsigned long& _Y) {return (_Y < _X ? _Y : _X); }
    #define NEED_INVHYP
  #elif _MSC_VER >= 1500 && _MSC_VER < 1600
    // MS Visual C++ 2008 (aka 9.0)
    #define COMPILER_VER                      ".msvc9"
    #define METADATA_COMPILER_STRING          "msvc 9"
    #define NEED_INVHYP
  #elif _MSC_VER >= 1600 && _MSC_VER < 1700
    // MS Visual C++ 2010 (aka 10.0)
    #define COMPILER_VER                      ".msvc10"
    #define METADATA_COMPILER_STRING          "msvc 10"
    // msvc10 defines std::hash<> as a class, while boost's flyweight_fwd.hpp may forward-declare it as a struct;
    // this is valid according to the C++ standard, but causes msvc10 to issue warnings.
    #pragma warning(disable : 4099)
    #define NEED_INVHYP
  #elif _MSC_VER >= 1700 && _MSC_VER < 1800
    // MS Visual C++ 2012 (aka 11.0)
    #define COMPILER_VER                      ".msvc11"
    #define METADATA_COMPILER_STRING          "msvc 11"
    #error Please update syspovconfig_msvc.h to include this version of MSVC
    // The following settings are just guesswork, and have never been tested:
    #define NEED_INVHYP
  #elif _MSC_VER >= 1800 && _MSC_VER < 1900
    // MS Visual C++ 2013 (aka 12.0)
    #define COMPILER_VER                      ".msvc12"
    #define METADATA_COMPILER_STRING          "msvc 12"
    #error Please update syspovconfig_msvc.h to include this version of MSVC
    // The following settings are just guesswork, and have never been tested:
    // (no special settings)
  // NB: The Microsoft Visual Studio developers seem to have skipped internal version number 13 entirely.
  #elif _MSC_VER >= 1900 && _MSC_VER < 2000
    // MS Visual C++ 2015 (aka 14.0)
    #define COMPILER_VER                      ".msvc14"
    #define METADATA_COMPILER_STRING          "msvc 14"
  #else
    #error Please update syspovconfig_msvc.h to include this version of MSVC
  #endif
  #define COMPILER_NAME                       "Microsoft Visual C++"
  #define COMPILER_VERSION                    _MSC_VER

#endif

#ifdef _WIN64
  #if defined(_M_X64)
    #define METADATA_PLATFORM_STRING        "x86_64-pc-win"
  #else
    #error Please update msvc.h to include this 64-bit architecture
  #endif
#elif defined _WIN32
  #if !defined(METADATA_X86_FPU_STRING)
    #if defined(_M_IX86_FP)
      #if (_M_IX86_FP == 0)
        #define METADATA_X86_FPU_STRING     ""
      #elif (_M_IX86_FP == 1)
        #define METADATA_X86_FPU_STRING     "-sse"
      #elif (_M_IX86_FP == 2)
        #define METADATA_X86_FPU_STRING     "-sse2"
      #else
        #error Please update msvc.h to include this x86 FPU generation
      #endif
    #else
      #error Please update msvc.h to detect x86 FPU generation for your compiler
    #endif
  #endif
  #if defined(_M_IX86)
    #if (_M_IX86 == 300)
      #define METADATA_PLATFORM_STRING      "i386-pc-win" METADATA_X86_FPU_STRING
    #elif (_M_IX86 == 400)
      #define METADATA_PLATFORM_STRING      "i486-pc-win" METADATA_X86_FPU_STRING
    #elif (_M_IX86 == 500)
      #define METADATA_PLATFORM_STRING      "i586-pc-win" METADATA_X86_FPU_STRING
    #elif (_M_IX86 == 600)
      #define METADATA_PLATFORM_STRING      "i686-pc-win" METADATA_X86_FPU_STRING
    #elif (_M_IX86 == 700)
      #define METADATA_PLATFORM_STRING      "i786-pc-win" METADATA_X86_FPU_STRING
    #else
      #error Please update msvc.h to include this x86 CPU generation
    #endif
  #else
    #error Please update msvc.h to include this 32-bit architecture
  #endif
#endif

#define QSORT(a,b,c,d)                      qsort(reinterpret_cast<void *>(a), (size_t) b, (size_t) c, d)
#define POV_LONG                            signed __int64
#define POV_ULONG                           unsigned __int64
#define FORCEINLINE                         __forceinline

#define POV_INT8                            signed __int8
#define POV_UINT8                           unsigned __int8
#define POV_INT16                           signed __int16
#define POV_UINT16                          unsigned __int16
#define POV_INT32                           signed __int32
#define POV_UINT32                          unsigned __int32
#define POV_INT64                           signed __int64
#define POV_UINT64                          unsigned __int64

#undef ReturnAddress
#define ReturnAddress()                     _ReturnAddress()

#endif // POVRAY_WINDOWS_SYSPOVCONFIG_MSVC_H
