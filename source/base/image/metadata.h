//******************************************************************************
///
/// @file base/image/metadata.h
///
/// Declarations related to image metadata.
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

#ifndef POVRAY_BASE_METADATA_H
#define POVRAY_BASE_METADATA_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// Standard C++ header files
#include <string>

// Boost header files
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace pov_base
{

/// Generates metadata to write into output images.
class Metadata
{
    public:
        Metadata();
        ~Metadata();

        /**
         *  Get software string.
         *  @note   This method should return at most 40 ascii bytes, otherwise it may become truncated by some file formats.
         *          non-printable characters, line feeds and tabs are not allowed.
         */
        string getSoftware() const;

        /* Up to 4 comments, each at most 80 ascii bytes, no line feed, no tab
         * if it's longer, it can either fit anyway or get truncated, it's the
         * image format which choose what happen.
         */

        /**
         *  Get comment string #1.
         *  @note   This method should return at most 80 ascii bytes, otherwise it may become truncated by some file formats.
         *          non-printable characters, line feeds and tabs are not allowed.
         */
        string getComment1() const;

        /**
         *  Get comment string #2.
         *  @note   This method should return at most 80 ascii bytes, otherwise it may become truncated by some file formats.
         *          non-printable characters, line feeds and tabs are not allowed.
         */
        string getComment2() const;

        /**
         *  Get comment string #3.
         *  @note   This method should return at most 80 ascii bytes, otherwise it may become truncated by some file formats.
         *          non-printable characters, line feeds and tabs are not allowed.
         */
        string getComment3() const;

        /**
         *  Get comment string #4.
         *  @note   This method should return at most 80 ascii bytes, otherwise it may become truncated by some file formats.
         *          non-printable characters, line feeds and tabs are not allowed.
         */
        string getComment4() const;

        /// Get date string in ISO 8601 format.
        string getDateTime() const;

        /// Get year (including full century)
        int getYear() const;
        /// Get month (1..12)
        int getMonth() const;
        /// Get day of month (1..31)
        int getDay() const;
        /// Get hour of day (0..23)
        int getHour() const;
        /// Get minute of hour (0..59)
        int getMin() const;
        /// Get second of minute (0..59)
        int getSec() const;

    protected:
        boost::gregorian::date              date;
        boost::posix_time::time_duration    time;
};

}
#endif
