//******************************************************************************
///
/// @file base/image/metadata.cpp
///
/// Implementations related to image metadata.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/image/metadata.h"

// POV-Ray base header files
#include "base/version_info.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

Metadata::Metadata()
{
    boost::posix_time::ptime timestamp = boost::posix_time::second_clock::universal_time();
    date = timestamp.date();
    time = timestamp.time_of_day();
}

Metadata::~Metadata()
{}

string Metadata::getSoftware() const
{
    return "POV-Ray v" POV_RAY_SOURCE_VERSION;
}

string Metadata::getComment1() const
{
#ifdef METADATA_PLATFORM_STRING
    // METADATA_PLATFORM_STRING should be in a form similar to 'i686-pc-linux-gnu', 'i386-pc-win', etc.
    return string("Platform: ") + METADATA_PLATFORM_STRING;
#else
    return string();
#endif
}

string Metadata::getComment2() const
{
#ifdef METADATA_COMPILER_STRING
    // METADATA_COMPILER_STRING should be in a form similar to 'g++ 4.4.3', 'msvc 10.0', etc.
    return string("Compiler: ") + METADATA_COMPILER_STRING;
#else
    return string();
#endif
}

string Metadata::getComment3() const
{
#ifdef METADATA_COMMENT_3
    // NB it is legal for METADATA_COMMENT_3 to be a function returning string
    // Note that it may be called more than once
    return string(METADATA_COMMENT_3);
#else
    return string();
#endif
}

string Metadata::getComment4() const
{
#ifdef METADATA_COMMENT_4
    // NB it is legal for METADATA_COMMENT_4 to be a function returning string
    // Note that it may be called more than once
    return string(METADATA_COMMENT_4);
#else
    return string();
#endif
}

string Metadata::getDateTime() const
{
    return to_iso_extended_string(date) + " " + to_simple_string(time) + "Z";
}

int Metadata::getYear() const
{
    return date.year();
}

int Metadata::getMonth() const
{
    return date.month();
}

int Metadata::getDay() const
{
    return date.day();
}

int Metadata::getHour() const
{
    return time.hours();
}

int Metadata::getMin() const
{
    return time.minutes();
}

int Metadata::getSec() const
{
    return time.seconds();
}

}
