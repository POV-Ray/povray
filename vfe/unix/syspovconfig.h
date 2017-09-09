/*******************************************************************************
 * syspovconfig.h
 *
 * This file contains most unix-specific defines for compiling the VFE.
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
 * $File: //depot/povray/smp/vfe/unix/syspovconfig.h $
 * $Revision: #17 $
 * $Change: 6133 $
 * $DateTime: 2013/11/25 15:28:53 $
 * $Author: clipka $
 *******************************************************************************/

#ifndef __SYSPOVCONFIG_H__
#define __SYSPOVCONFIG_H__

#define _FILE_OFFSET_BITS	64

#define fseek64(stream,offset,whence) fseeko(stream,offset,whence)

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# error "!!!!! config.h is required !!!!!"
#endif


#include <algorithm>
#include <stdarg.h>
#include <math.h>

#include "../vfeconf.h"


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
// with Boost 1.65.0 or later and a non-C++11-compliant compiler. We currently do not support such a
// combination.)
using POV_TR1_NAMESPACE::shared_ptr;
using POV_TR1_NAMESPACE::weak_ptr;

#endif // STD_POV_TYPES_DECLARED

// After Stroustrop in _The C++ Programming Language, 3rd Ed_ p. 88
#ifndef NULL
const int NULL=0;
#endif

#define POV_LONG long long

#define DELETE_FILE(name)  unlink(name)

#if defined (PATH_MAX)
# define FILE_NAME_LENGTH   PATH_MAX
#elif defined (_POSIX_PATH_MAX)
# define FILE_NAME_LENGTH   _POSIX_PATH_MAX
#else
# define FILE_NAME_LENGTH   200
#endif
#define MAX_PATH FILE_NAME_LENGTH  // FIXME: remove later

#define DEFAULT_OUTPUT_FORMAT       kPOVList_FileType_PNG
#define DEFAULT_DISPLAY_GAMMA_TYPE  kPOVList_GammaType_SRGB
#define DEFAULT_DISPLAY_GAMMA       2.2
#define DEFAULT_FILE_GAMMA_TYPE     kPOVList_GammaType_SRGB
#define DEFAULT_FILE_GAMMA          2.2

#define METADATA_PLATFORM_STRING BUILD_ARCH
#define METADATA_COMPILER_STRING COMPILER_VERSION

#define DECLARE_THREAD_LOCAL_PTR(ptrType, ptrName)                __thread ptrType *ptrName
#define IMPLEMENT_THREAD_LOCAL_PTR(ptrType, ptrName, ignore)      __thread ptrType *ptrName
#define GET_THREAD_LOCAL_PTR(ptrName)                             (ptrName)
#define SET_THREAD_LOCAL_PTR(ptrName, ptrValue)                   (ptrName = ptrValue)

#if defined(_AIX)
    // IBM AIX detected.
    // Not officially supported yet; comment-out the following line to try with default POSIX settings.
    #error IBM AIX detected, but not explicitly supported yet; proceed at your own risk.
#elif defined(__hpux)
    // Hewlett-Packard HP-UX detected.
    // Not officially supported yet; comment-out the following line to try with default POSIX settings.
    #error Hewlett-Packard HP-UX detected, but not explicitly supported yet; proceed at your own risk.
#elif defined(__linux__)
    // GNU/Linux detected.
#elif defined(__APPLE__) && defined(__MACH__)
    // Apple Mac OS X detected.
    #include <unistd.h>
    #define lseek64(handle,offset,whence) lseek(handle,offset,whence)
#elif defined(__sun) && defined(__SVR4)
    // Sun/Oracle Solaris detected.
    // Not officially supported yet; comment-out the following line to try with default POSIX settings.
    #error Sun/Oracle Solaris detected, but not explicitly supported yet; proceed at your own risk.
#elif defined(__CYGWIN__)
    // Cygwin detected.
    // Not officially supported yet; comment-out the following line to try with default POSIX settings.
    #error Cygwin detected, but not explicitly supported yet; proceed at your own risk.
#elif defined(__unix__)
    // Some Unix other than the above detected.
    #include <sys/param.h>
    #if defined(BSD)
        // BSD-style Unix detected.
        #error BSD-style Unix detected, but not explicitly supported yet; proceed at your own risk.
    #else
        // Not officially supported yet; comment-out the following line to try with default POSIX settings.
        #error Unix detected, but flavor not identified; proceed at your own risk.
    #endif
#else
    // Doesn't look like a Unix at all.
    // Comment-out the following line to try with default POSIX settings.
    #error No Unix detected; proceed at your own risk.
#endif

#endif
