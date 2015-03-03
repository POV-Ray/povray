//******************************************************************************
///
/// @file tests/tests_main.cpp
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

#define BOOST_TEST_MODULE pov_test_main
#include <boost/test/included/unit_test.hpp>

#include <limits>

// configbase.h must always be the first POV file included within base *.cpp files
#include "base/configbase.h"

#include "base/safemath.h"

// this must be the last file included
#include "base/povdebug.h"


#define EXPECT_POV_EXCEPTION( expr ) do try { expr; BOOST_ERROR( "Exception expected."); } catch (pov_base::Exception&) {} while(0)

BOOST_AUTO_TEST_SUITE( safemath_h )

    BOOST_AUTO_TEST_CASE( SafeUnsignedProduct )
    {
        int m = std::numeric_limits<short>::max();
        int a;
        int b = 2;
        int c = 3;
        int d = 4;
        int e;

        a = m/b;
        BOOST_CHECK( a*b == pov_base::SafeUnsignedProduct<short>(a,b) );
        a++;
        EXPECT_POV_EXCEPTION ( e = pov_base::SafeUnsignedProduct<short>(a,b) );

        a = m/(b*c);
        BOOST_CHECK( a*b*c == pov_base::SafeUnsignedProduct<short>(a,b,c) );
        a++;
        EXPECT_POV_EXCEPTION ( e = pov_base::SafeUnsignedProduct<short>(a,b,c) );

        a = m/(b*c*d);
        BOOST_CHECK( a*b*c*d == pov_base::SafeUnsignedProduct<short>(a,b,c,d) );
        a++;
        EXPECT_POV_EXCEPTION ( e = pov_base::SafeUnsignedProduct<short>(a,b,c,d) );
    }

BOOST_AUTO_TEST_SUITE_END()

/*
BOOST_AUTO_TEST_CASE( test_case_on_file_scope )
{
    BOOST_CHECK( true );
}

BOOST_AUTO_TEST_SUITE( another_test_suite )

    BOOST_AUTO_TEST_CASE( another_test_case )
    {
        BOOST_ERROR( "some error 2" );
    }

BOOST_AUTO_TEST_SUITE_END()
*/
