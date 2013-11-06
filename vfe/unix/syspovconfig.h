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
 * $File: //depot/public/povray/3.x/vfe/unix/syspovconfig.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
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
#include <boost/shared_ptr.hpp>

// when we say 'string' we mean std::string
using std::string;

// and vector is a std::vector
using std::vector;

// yup, list too
using std::list;

// runtime_error is the base of our Exception class, plus is referred
// to in a few other places.
using std::runtime_error;

// C++0x has a shared_ptr, but we currently use the boost one.
using boost::shared_ptr;

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

#endif
