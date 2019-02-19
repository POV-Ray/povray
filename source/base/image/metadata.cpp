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
#include "base/image/metadata.h"

// C++ variants of C standard header files
#include <cstdio>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/version_info.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

Metadata::Metadata()
{
    mTimestamp = boost::posix_time::second_clock::universal_time();
}

Metadata::~Metadata()
{}

std::string Metadata::getSoftware() const
{
    return "POV-Ray v" POV_RAY_SOURCE_VERSION;
}

std::string Metadata::getComment1() const
{
#ifdef METADATA_PLATFORM_STRING
    // METADATA_PLATFORM_STRING should be in a form similar to 'i686-pc-linux-gnu', 'i386-pc-win', etc.
    return std::string("Platform: ") + METADATA_PLATFORM_STRING;
#else
    return std::string();
#endif
}

std::string Metadata::getComment2() const
{
#ifdef METADATA_COMPILER_STRING
    // METADATA_COMPILER_STRING should be in a form similar to 'g++ 4.4.3', 'msvc 10.0', etc.
    return std::string("Compiler: ") + METADATA_COMPILER_STRING;
#else
    return std::string();
#endif
}

std::string Metadata::getComment3() const
{
#ifdef METADATA_COMMENT_3
    // NB it is legal for METADATA_COMMENT_3 to be a function returning string
    // Note that it may be called more than once
    return std::string(METADATA_COMMENT_3);
#else
    return std::string();
#endif
}

std::string Metadata::getComment4() const
{
#ifdef METADATA_COMMENT_4
    // NB it is legal for METADATA_COMMENT_4 to be a function returning string
    // Note that it may be called more than once
    return std::string(METADATA_COMMENT_4);
#else
    return std::string();
#endif
}

std::string Metadata::getDateTime() const
{
    // Not using boost's `to_iso_extended_string` because that would mean we couldn't reliably
    // get away with using the boost date_time library in header-only mode.
    char s[21]; // 10 (date) + 1 (blank) + 8 (time) + 1 (timezone "Z") + 1 (trailing NUL)
    std::snprintf(s, sizeof(s), "%04d-%02d-%02d %02d:%02d:%02dZ", getYear(), getMonth(), getDay(), getHour(), getMin(), getSec());
    return std::string(s);
}

int Metadata::getYear() const
{
    return mTimestamp.date().year();
}

int Metadata::getMonth() const
{
    return mTimestamp.date().month();
}

int Metadata::getDay() const
{
    return mTimestamp.date().day();
}

int Metadata::getHour() const
{
    return mTimestamp.time_of_day().hours();
}

int Metadata::getMin() const
{
    return mTimestamp.time_of_day().minutes();
}

int Metadata::getSec() const
{
    return mTimestamp.time_of_day().seconds();
}

}
// end of namespace pov_base
