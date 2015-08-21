//******************************************************************************
///
/// @file tests/source/tests_safemath.cpp
///
/// POV-Ray unit tests for the safemath module (@ref base/safemath.h).
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

#include <limits>

// configbase.h must always be the first POV file included within base *.cpp files
// (and as that's what we're testing, we should consider ourselves part of it);
// tests.h must follow suite.
#include "base/configbase.h"
#include "tests.h"

#include "base/safemath.h"

// this must be the last file included
#include "base/povdebug.h"

BOOST_AUTO_TEST_SUITE( SafeMath )

    BOOST_AUTO_TEST_CASE( SafeUnsignedProduct )
    {
        int m = std::numeric_limits<short>::max();
        int a;
        int b = 2;
        int c = 3;
        int d = 4;

        a = m/b;
        BOOST_CHECK_EQUAL( a*b, pov_base::SafeUnsignedProduct<short>(a,b) );
        a++;
        EXPECT_POV_EXCEPTION ( pov_base::SafeUnsignedProduct<short>(a,b) );

        a = m/(b*c);
        BOOST_CHECK_EQUAL( a*b*c, pov_base::SafeUnsignedProduct<short>(a,b,c) );
        a++;
        EXPECT_POV_EXCEPTION ( pov_base::SafeUnsignedProduct<short>(a,b,c) );

        a = m/(b*c*d);
        BOOST_CHECK_EQUAL( a*b*c*d, pov_base::SafeUnsignedProduct<short>(a,b,c,d) );
        a++;
        EXPECT_POV_EXCEPTION ( pov_base::SafeUnsignedProduct<short>(a,b,c,d) );
    }

BOOST_AUTO_TEST_SUITE_END()
