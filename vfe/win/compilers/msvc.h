/*******************************************************************************
 * msvc.h
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/vfe/win/compilers/msvc.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef __MSVC_H__
#define __MSVC_H__

#if _MSC_VER < 1400
  #error minimum Visual C++ version supported is 14.0 (supplied with VS 2005)
#endif

#undef USE_AVX_FMA4_FOR_NOISE
#if _MSC_FULL_VER >= 160040219
  // MS Visual C++ 2010 (aka 10.0) SP1
  #define USE_AVX_FMA4_FOR_NOISE
#endif

#include <direct.h>
#include <stdio.h>
#include <intrin.h>

#pragma auto_inline(on)
#pragma warning(disable : 4018) /* signed/unsigned mismatch */
#pragma warning(disable : 4305) /* truncation from 'type1' to 'type2' (mostly double to float) */
#pragma warning(disable : 4244) /* possible loss of data (converting ints to shorts) */

#ifdef __INTEL_COMPILER

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
    #error Please update msvc.h to include this version of ICL
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
  #elif _MSC_VER >= 1500 && _MSC_VER < 1600
    // MS Visual C++ 2008 (aka 9.0)
    #define COMPILER_VER                      ".msvc9"
    #define METADATA_COMPILER_STRING          "msvc 9"
  #elif _MSC_VER >= 1600 && _MSC_VER < 1700
    // MS Visual C++ 2010 (aka 10.0)
    #define COMPILER_VER                      ".msvc10"
    #define METADATA_COMPILER_STRING          "msvc 10"
    // msvc10 defines std::hash<> as a class, while boost's flyweight_fwd.hpp may declare it as a struct;
    // this is valid according to the C++ standard, but causes msvc10 to issue warnings.
    #pragma warning(disable : 4099)
  #elif _MSC_VER >= 1900 && _MSC_VER < 2000
    // MS Visual C++ 2015 (aka 14.0)
    #define COMPILER_VER                      ".msvc14"
    #define METADATA_COMPILER_STRING          "msvc 14"
  #else
    #error Please update msvc.h to include this version of MSVC
  #endif
  #define COMPILER_NAME                       "Microsoft Visual C++"
  #define COMPILER_VERSION                    _MSC_VER

  #define NEED_INVHYP

  // boost will define these for us otherwise
  #ifdef NOT_USING_BOOST
    #if !defined (_WIN64)
      extern "C"
      {
        __declspec(dllimport) long __stdcall _InterlockedIncrement(long volatile *Addend);
        __declspec(dllimport) long __stdcall _InterlockedDecrement(long volatile *Addend);
        __declspec(dllimport) long __stdcall _InterlockedCompareExchange(long volatile *Dest, long Exchange, long Comp);
        __declspec(dllimport) long __stdcall _InterlockedExchange(long volatile *Target, long Value);
        __declspec(dllimport) long __stdcall _InterlockedExchangeAdd(long volatile *Addend, long Value);
      }

      #pragma intrinsic (_InterlockedCompareExchange)
      #define InterlockedCompareExchange _InterlockedCompareExchange

      #pragma intrinsic (_InterlockedExchange)
      #define InterlockedExchange _InterlockedExchange 

      #pragma intrinsic (_InterlockedExchangeAdd)
      #define InterlockedExchangeAdd _InterlockedExchangeAdd

      #pragma intrinsic (_InterlockedIncrement)
      #define InterlockedIncrement _InterlockedIncrement

      #pragma intrinsic (_InterlockedDecrement)
      #define InterlockedDecrement _InterlockedDecrement
    #endif
  #endif

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
#define POV_LONG                            __int64
#define FORCEINLINE                         __forceinline

#undef ReturnAddress
#define ReturnAddress()                     _ReturnAddress()

#define DECLARE_THREAD_LOCAL_PTR(ptrType, ptrName)                __declspec(thread) ptrType *ptrName
#define IMPLEMENT_THREAD_LOCAL_PTR(ptrType, ptrName, ignore)      __declspec(thread) ptrType *ptrName
#define GET_THREAD_LOCAL_PTR(ptrName)                             (ptrName)
#define SET_THREAD_LOCAL_PTR(ptrName, ptrValue)                   (ptrName = ptrValue)

#endif
