//******************************************************************************
///
/// @file tests/source/tests_main.cpp
///
/// Unit tests for POV-Ray.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#include <climits>
#include <limits>

#define BOOST_TEST_MODULE "POV-Ray Unit Tests"
#include <boost/test/included/unit_test.hpp>

// configbase.h must always be the first POV file included within base *.cpp files
// (and as that's what we're testing, we should consider ourselves part of it);
// tests.h must follow suite.
#include "base/configbase.h"
#include "tests.h"

#include "base/types.h"

// this must be the last file included
#include "base/povdebug.h"

#define BITS(t) (sizeof(t)*CHAR_BIT)

#define POV_CHECK_MESSAGE(c,m) BOOST_CHECK_MESSAGE ( c, m << " POV-Ray will not work properly; fix your configuration.")
#define POV_WARN_MESSAGE(c,m)  BOOST_WARN_MESSAGE  ( c, m << " Some pieces of code in POV-Ray may choke on this; use at your own risk.")

#define TEST_TYPE_SIZE(t,w) SINGLE_STATEMENT( \
    POV_CHECK_MESSAGE ( BITS(t) >= (w), #t << " has fewer than the required " << w << " bits." ); \
    POV_WARN_MESSAGE  ( BITS(t) <= (w), #t << " has more than the expected "  << w << " bits." ); \
    )

#define TEST_TWOS_COMPLEMENT(st,ut) \
    POV_WARN_MESSAGE ( (ut)((st)(-1)) == std::numeric_limits<ut>::max(), #st << " does not use 2's complement representation for negative values." )

#define TEST_UNSIGNED(t) \
    POV_WARN_MESSAGE ( !std::numeric_limits<t>::is_signed, #t << " is a signed type." );

#define TEST_SIGNED(t) \
    POV_WARN_MESSAGE ( std::numeric_limits<t>::is_signed, #t << " is an unsigned type." );

#define TEST_RANGE(t,mx) SINGLE_STATEMENT( \
    if (std::numeric_limits<t>::is_signed) \
    POV_CHECK_MESSAGE ( std::numeric_limits<t>::min() + (mx) <= (-1), #t << " does not support values as small as -" << mx << "-1" ); \
    else \
    POV_CHECK_MESSAGE ( std::numeric_limits<t>::min() <= (0),         #t << " does not support values as small as zero (WTF?!)." ); \
    POV_CHECK_MESSAGE ( std::numeric_limits<t>::max() >= (mx),        #t << " does not support values as large as " << mx << "." ); \
    if (std::numeric_limits<t>::is_signed) \
    POV_WARN_MESSAGE  ( std::numeric_limits<t>::min() + (mx) >= (-1), #t << " supports values smaller than -" << mx << "-1" ); \
    else \
    POV_WARN_MESSAGE  ( std::numeric_limits<t>::min() >= (0),         #t << " supports values smaller than zero (WTF?!)." ); \
    POV_WARN_MESSAGE  ( std::numeric_limits<t>::max() <= (mx),        #t << " supports values larger than " << mx << "." ); \
    )

BOOST_AUTO_TEST_SUITE( Sanity )

    BOOST_AUTO_TEST_CASE( TypeSizes )
    {
        TEST_TYPE_SIZE( POV_INT8,    8 );
        TEST_TYPE_SIZE( POV_UINT8,   8 );
        TEST_TYPE_SIZE( POV_INT16,  16 );
        TEST_TYPE_SIZE( POV_UINT16, 16 );
        TEST_TYPE_SIZE( POV_INT32,  32 );
        TEST_TYPE_SIZE( POV_UINT32, 32 );
        TEST_TYPE_SIZE( POV_INT64,  64 );
        TEST_TYPE_SIZE( POV_UINT64, 64 );

        TEST_TWOS_COMPLEMENT( POV_INT8,  POV_UINT8 );
        TEST_TWOS_COMPLEMENT( POV_INT16, POV_UINT16 );
        TEST_TWOS_COMPLEMENT( POV_INT32, POV_UINT32 );
        TEST_TWOS_COMPLEMENT( POV_INT64, POV_UINT64 );

        TEST_RANGE( POV_INT8,                 0x7F );
        TEST_RANGE( POV_UINT8,                0xFF );
        TEST_RANGE( POV_INT16,              0x7FFF );
        TEST_RANGE( POV_UINT16,             0xFFFF );
        TEST_RANGE( POV_INT32,          0x7FFFFFFF );
        TEST_RANGE( POV_UINT32,         0xFFFFFFFF );
        TEST_RANGE( POV_INT64,  0x7FFFFFFFFFFFFFFF );
        TEST_RANGE( POV_UINT64, 0xFFFFFFFFFFFFFFFF );
    }

BOOST_AUTO_TEST_SUITE_END()
