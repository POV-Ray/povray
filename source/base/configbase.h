/*******************************************************************************
 * configbase.h
 *
 * This header file defines all types that can be configured by platform
 * specific code for base-layer use. It further allows insertion of platform
 * specific function prototypes making use of those types.
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
 * $File: //depot/public/povray/3.x/source/base/configbase.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef CONFIGBASE_H
#define CONFIGBASE_H

#include <boost/version.hpp>
#include "syspovconfigbase.h"

#ifndef DBL
	#define DBL double
#endif

#ifndef SNGL
	#define SNGL float
#endif

#ifndef COLC
	#define COLC float
#endif

#ifndef UCS2
	#define UCS2 unsigned short
#endif

#ifndef UCS4
	#define UCS4 unsigned int
#endif

#ifndef POV_LONG
	#define POV_LONG long long
	#define POV_ULONG unsigned long long
#endif

#ifndef POV_ULONG
	#define POV_ULONG unsigned POV_LONG
#endif

#ifndef M_PI
	#define M_PI   3.1415926535897932384626
#endif

#ifndef M_PI_2
	#define M_PI_2 1.57079632679489661923
#endif

#ifndef TWO_M_PI
	#define TWO_M_PI 6.283185307179586476925286766560
#endif

#ifndef M_PI_180
	#define M_PI_180 0.01745329251994329576
#endif

#ifndef M_PI_360
	#define M_PI_360 0.00872664625997164788
#endif

#ifndef POV_SYS_FILE_EXTENSION
	#define POV_SYS_FILE_EXTENSION ".tga"
#endif

#ifndef POV_FILE_SEPARATOR
	#define POV_FILE_SEPARATOR '/'
#endif

#ifndef POV_UCS2_FOPEN
	#define POV_UCS2_FOPEN(name, mode) fopen(UCS2toASCIIString(UCS2String(name)).c_str(), mode)
#endif

#ifndef POV_UCS2_REMOVE
	#define POV_UCS2_REMOVE(name) unlink(UCS2toASCIIString(UCS2String(name)).c_str())
#endif

#ifndef EXIST_FONT_FILE
	#define EXIST_FONT_FILE(name) (0)
#endif

#ifndef DEFAULT_ITEXTSTREAM_BUFFER_SIZE
	#define DEFAULT_ITEXTSTREAM_BUFFER_SIZE 512
#endif

#ifndef POV_ALLOW_FILE_READ
	#define POV_ALLOW_FILE_READ(f,t) (1)
#endif

#ifndef POV_ALLOW_FILE_WRITE
	#define POV_ALLOW_FILE_WRITE(f,t) (1)
#endif

#ifndef POV_TRACE_THREAD_PREINIT
	#define POV_TRACE_THREAD_PREINIT
#endif

#ifndef POV_TRACE_THREAD_POSTINIT
	#define POV_TRACE_THREAD_POSTINIT
#endif

// these should not be changed by platform-specific config
#define DEFAULT_WORKING_GAMMA_TYPE  kPOVList_GammaType_Neutral
#define DEFAULT_WORKING_GAMMA       1.0
#define DEFAULT_WORKING_GAMMA_TEXT  "1.0"

// boost 1.50 changed TIME_UTC to TIME_UTC_ to avoid a clash with C11, which has
// TIME_UTC as a define (in boost it's an enum). To allow compilation with earlier
// versions of boost we now use POV_TIME_UTC in the code and define that here.
// unfortunately we do have to hard-code the value.
#if BOOST_VERSION >= 105000
	#define POV_TIME_UTC boost::TIME_UTC_
#else
	#ifdef TIME_UTC
		// clash between C11 and boost detected, need to hard-code
		#define POV_TIME_UTC 1
	#else
		#define POV_TIME_UTC boost::TIME_UTC
	#endif
#endif

#include "base/povms.h"
#include "base/povmscpp.h"

namespace pov_base
{

typedef POVMSUCS2String UCS2String;

inline UCS2String ASCIItoUCS2String(const char *s)
{
	return POVMS_ASCIItoUCS2String(s);
}

inline string UCS2toASCIIString(const UCS2String& s)
{
	return POVMS_UCS2toASCIIString(s);
}

}

#include "syspovprotobase.h"

#endif
