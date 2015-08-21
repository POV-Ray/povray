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

    BOOST_AUTO_TEST_SUITE( SafeUnsignedProduct )

        // We use very fine-grained test cases here, as much of them is about throwing exceptions,
        // and we don't want a stray unexpected exception to spoil the whole party.

        BOOST_AUTO_TEST_CASE( SafeUnsignedProductShort2 )
        {
            int m = std::numeric_limits<short>::max();
            int a;
            int b = 2;

            a = m/b; // largest value that, when multiplied with b, fits in our data type
            BOOST_CHECK_EQUAL( a*b, pov_base::SafeUnsignedProduct<short>(a,b) );
            a++; // now the product can't possibly fit
            BOOST_CHECK_THROW( pov_base::SafeUnsignedProduct<short>(a,b), pov_base::Exception );
        }

        BOOST_AUTO_TEST_CASE( SafeUnsignedProductShort3 )
        {
            int m = std::numeric_limits<short>::max();
            int a;
            int b = 2;
            int c = 3;

            a = m/(b*c); // largest value that, when multiplied with b*c, fits in our data type
            BOOST_CHECK_EQUAL( a*b*c, pov_base::SafeUnsignedProduct<short>(a,b,c) );
            a++; // now the product can't possibly fit
            BOOST_CHECK_THROW( pov_base::SafeUnsignedProduct<short>(a,b,c), pov_base::Exception );
        }

        BOOST_AUTO_TEST_CASE( SafeUnsignedProductShort4 )
        {
            int m = std::numeric_limits<short>::max();
            int a;
            int b = 2;
            int c = 3;
            int d = 4;

            a = m/(b*c*d); // largest value that, when multiplied with b*c*d, fits in our data type
            BOOST_CHECK_EQUAL( a*b*c*d, pov_base::SafeUnsignedProduct<short>(a,b,c,d) );
            a++; // now the product can't possibly fit
            BOOST_CHECK_THROW( pov_base::SafeUnsignedProduct<short>(a,b,c,d), pov_base::Exception );
        }

    BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
