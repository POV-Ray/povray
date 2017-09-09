/*******************************************************************************
 * syspovconfig.h
 *
 * This file contains most Windows specific defines for compiling the VFE.
 *
 * Author: Christopher J. Cason
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
 * $File: //depot/povray/smp/vfe/win/syspovconfig.h $
 * $Revision: #41 $
 * $Change: 6132 $
 * $DateTime: 2013/11/25 14:23:41 $
 * $Author: clipka $
 *******************************************************************************/

#ifndef __SYSPOVCONFIG_H__
#define __SYSPOVCONFIG_H__

#ifdef BUILDING_AMD64
  #if !defined(_M_AMD64) && !defined(_M_X64)
    #error you are compiling the x64 project using a 32-bit compiler
  #endif
#else
  #if defined(_M_AMD64) || defined(_M_X64)
    #error you are compiling the 32-bit project using a 64-bit compiler
  #endif
#endif

// _CONSOLE must be defined when building the console version of POVWIN.
// failure to do so will lead to link errors.
// #define _CONSOLE

#include <math.h>
#include <algorithm>

using std::max;
using std::min;

#ifndef STD_TYPES_DECLARED
#define STD_TYPES_DECLARED

// the following types are used extensively throughout the POV source and hence are
// included and named here for reasons of clarity and convenience.

#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include <list>

#include <boost/version.hpp>
#if BOOST_VERSION < 106500
    // Pulling in smart pointers is easy with Boost versions prior to 1.65.0, with the
    // `boost/tr1/*.hpp` set of headers simply pulling in whatever is available (C++11, TR1 or
    // boost's own implementation) and making it available in the `std::tr1` namespace.
    #include <boost/tr1/memory.hpp>
    #define POV_TR1_NAMESPACE std::tr1
#else
    // With `boost/tr1/*.hpp` unavailable, we're currently blindly relying on the compiler to
    // be compliant with C++11.
    #include <memory>
    #define POV_TR1_NAMESPACE std
#endif

// when we say 'string' we mean std::string
using std::string;

// and vector is a std::vector
using std::vector;

// yup, list too
using std::list;

// runtime_error is the base of our Exception class, plus is referred
// to in a few other places.
using std::runtime_error;

// these may actually be the boost implementations, depending on what boost/tr1/memory.hpp has pulled in
// (NOTE: If you're running into a compile error here, you're probably trying to compile POV-Ray
// for Windows with Boost 1.65.0 or later and a non-C++11-compliant compiler. We currently do not
// support such a combination. Please use the Boost version that came bundled with the POV-Ray
// source code.)
using POV_TR1_NAMESPACE::shared_ptr;
using POV_TR1_NAMESPACE::weak_ptr;

#endif // STD_POV_TYPES_DECLARED

// the build command-line is expected to declare WIN32_LEAN_AND_MEAN, which will
// prevent Window's objidl.h from being pulled in (which dupes IStream)
// #include <windows.h>

//#define PROFILE_CALLS

#ifdef _WIN64
  #define POVRAY_PLATFORM_NAME "win64"
#else
  #define POVRAY_PLATFORM_NAME "win32"
#endif

#define ReturnAddress()           NULL

#include "../vfeconf.h"

#if defined(__MINGW32__)                    /* MinGW GCC */
  #include "compilers/mingw32.h"
#elif defined(__WATCOMC__)                  /* Watcom C/C++ C32 */
  #include "compilers/watcom.h"
#elif defined(__BORLANDC__)                 /* Borland C/C++ */
  #include "compilers/borland.h"
#elif defined(_MSC_VER)                     /* Microsoft and Intel C++ */
  #include "compilers/msvc.h"
#else
  #error unknown compiler configuration
#endif

#ifdef BUILD_SSE2
  #define SSE2_INCLUDED "-sse2"
#else
  #define SSE2_INCLUDED ""
#endif

/////////////////////////////////////////////////////////////

#ifndef MAX
  #define MAX(a,b) ((a>b)?a:b)
#endif
#ifndef MIN
  #define MIN(a,b) ((a>b)?b:a)
#endif

#ifndef HEAPSHRINK
  #define HEAPSHRINK
#endif

#ifndef NAN
  #define NAN (10E100)
#endif

#ifndef __GENDEFS
  #define __GENDEFS
  typedef unsigned char     uchar;
  typedef unsigned short    ushort;
  typedef unsigned int      uint;
  typedef unsigned long     ulong;
  typedef unsigned __int64  uint64;
  typedef __int64           int64;
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>

#ifdef __INTEL_COMPILER
// Intel C++ whines about the lack of a return on RDTSC() for each and every file
// it is included in. VC++ is smarter (or dumber, depending on your point of view).
#pragma warning(push)
#pragma warning(disable : 1011)
#endif

namespace povwin
{
  void WIN_Debug_Log(unsigned int from, const char *msg) ;
  void WIN32_DEBUG_OUTPUT(const char *format,...) ;
  void WIN32_DEBUG_FILE_OUTPUT(const char *format,...) ;

#ifndef _CONSOLE
  void *win_malloc(size_t size);
  void *win_malloc(size_t size, const void *ptr, int line);
  void *win_calloc(size_t nelem, size_t size);
  void *win_calloc(size_t nelem, size_t size, const void *ptr, int line);
  void *win_realloc(void *p, size_t size);
  void *win_realloc(void *p, size_t size, const void *ptr, int line);
  void win_free(void *p);
  void win_free(void *p, const void *ptr, int line);
  char *win_strdup(const char *s);
  char *win_strdup(const char *s, const void *ptr, int line);
  bool WinMemReport(bool global, uint64& allocs, uint64& frees, int64& current, uint64& peak, uint64& smallest, uint64& largest);
  void WinMemStage(bool BeginRender, void *cookie = NULL);
#endif

#ifndef _WIN64
  inline void DebugBreak() { _asm _emit 0cch } // rather than use the windows one
  inline POV_LONG RDTSC(){ _asm _emit 0Fh _asm _emit 31h }
  #define READ_PROFILE_TIMER RDTSC()
#else
  inline void DebugBreak() {}
  #define READ_PROFILE_TIMER 0
#endif
}
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif

#define fseek64(stream,offset,whence)       _fseeki64(stream,offset,whence)
#define lseek64(handle,offset,whence)       _lseeki64(handle,offset,whence)

namespace pov_base
{
  // declare these to avoid warnings in image.cpp, rather than turn off the deprecation warnings.
  static inline int open(const char *name, int flags, int mode) { return _open(name, flags|_O_BINARY, mode); }
  static inline int close(int handle) { return _close(handle); }
  static inline int write(int handle, const void *data, int count) { return _write(handle, data, count); }
  static inline int read(int handle, void *data, int count) { return _read(handle, data, count); }
}

#define S_IRUSR                             _S_IREAD
#define S_IWUSR								_S_IWRITE

#define ALTMAIN
#define LITTLE_ENDIAN
#define FILENAME_SEPARATOR                  '\\'
#define POV_FILE_SEPARATOR                  '\\'
#define POV_FILE_SEPARATOR_2                '/'
#define DEFAULT_OUTPUT_FORMAT               kPOVList_FileType_PNG
#define POV_IS1                             ".bmp"
#define POV_IS2                             ".BMP"
#define DEFAULT_DISPLAY_GAMMA_TYPE          kPOVList_GammaType_SRGB
#define DEFAULT_DISPLAY_GAMMA               2.2
#define DEFAULT_FILE_GAMMA_TYPE             kPOVList_GammaType_SRGB
#define DEFAULT_FILE_GAMMA                  2.2
#define RENAME_FILE(orig,new)               rename(orig,new)
#define DELETE_FILE(name)                   _unlink(name)
#define NEW_LINE_STRING                     "\r\n"
#define TEXTSTREAM_CRLF                     1
#define POV_SYS_FILE_EXTENSION              ".bmp"
#define SYS_TO_STANDARD                     BMP
#define vsnprintf                           _vsnprintf
#define snprintf                            _snprintf
#define FILE_NAME_LENGTH                    _MAX_PATH
#define POV_NAME_MAX                        _MAX_FNAME
#define IFF_SWITCH_CAST                     (long)
#define USE_OFFICIAL_BOOST                  1

#define POV_MEMMOVE(dst,src,len)            memmove((dst),(src),(len))
#define POV_MEMCPY(dst,src,len)             memcpy((dst),(src),(len))

#ifdef _CONSOLE

#define POV_MALLOC(size,msg)                malloc (size)
#define POV_CALLOC(nitems,size,msg)         calloc ((nitems), (size))
#define POV_REALLOC(ptr,size,msg)           realloc ((ptr), (size))
#define POV_FREE(ptr)                       do { free (static_cast<void *>(ptr)); (ptr) = NULL; } while(false)
#define POV_STRDUP(str)                     strdup(str)

#define NO_RTR                              1
#define MEM_STATS                           0

#else // not _CONSOLE

#define MEM_STATS                           0
#define POV_MEM_STATS                       0
#define WIN_MEM_TRACKING                    0

#ifdef _DEBUG
  #define POV_MALLOC(size,msg)              povwin::win_malloc ((size), __FILE__, __LINE__)
  #define POV_CALLOC(nitems,size,msg)       povwin::win_calloc ((nitems), (size), __FILE__, __LINE__)
  #define POV_REALLOC(ptr,size,msg)         povwin::win_realloc ((ptr), (size), __FILE__, __LINE__)
  #define POV_FREE(ptr)                     do { povwin::win_free (static_cast<void *>(ptr), __FILE__, __LINE__); (ptr) = NULL; } while(false)
  #define POV_STRDUP(str)                   povwin::win_strdup(str, __FILE__, __LINE__)
#else
  #define POV_MALLOC(size,msg)              povwin::win_malloc (size)
  #define POV_CALLOC(nitems,size,msg)       povwin::win_calloc ((nitems), (size))
  #define POV_REALLOC(ptr,size,msg)         povwin::win_realloc ((ptr), (size))
  #define POV_FREE(ptr)                     do { povwin::win_free (static_cast<void *>(ptr)); (ptr) = NULL; } while(false)
  #define POV_STRDUP(str)                   povwin::win_strdup(str)
#endif

#define POV_GLOBAL_MEM_STATS(a,f,c,p,s,l)   povwin::WinMemReport(true, a, f, c, p, s, l)
#define POV_THREAD_MEM_STATS(a,f,c,p,s,l)   povwin::WinMemReport(false, a, f, c, p, s, l)
#define POV_MEM_STATS_RENDER_BEGIN()        povwin::WinMemStage(true)
#define POV_MEM_STATS_RENDER_END()          povwin::WinMemStage(false)
#define POV_MEM_STATS_COOKIE                void *

#define POV_IMPLEMENT_RTR                   1

#define SYS_IMAGE_HEADER                    "syspovimage.h"
#define POV_VIDCAP_IMPL                     pov::VideoCaptureImpl
namespace pov
{
  class VideoCaptureImpl;
}
#endif // end of not _CONSOLE

// see RLP comment in 3.6 windows config.h
#undef HUGE_VAL

// use a larger buffer for more efficient parsing
#define DEFAULT_ITEXTSTREAM_BUFFER_SIZE 65536

// this adds some useful debugging information to each POV-Ray SDL object
#if defined _DEBUG
  #define OBJECT_DEBUG_HELPER
#endif

#ifndef MAX_PATH
  #define MAX_PATH _MAX_PATH
#endif

#define NEED_INVHYP

#ifndef NO_RTR
  #define RTR_HACK
  #define RTR_SUPPORT
#endif

#endif // __SYSPOVCONFIG_H__
