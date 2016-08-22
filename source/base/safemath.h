//******************************************************************************
///
/// @file base/safemath.h
///
/// Overflow-safe integer math operations.
///
/// The Functions in this file implement basic integer operations in a manner
/// that is robust with respect to numeric limits, reporting overflow by
/// throwing an exception.
///
/// The functions are designed for portability and robustness rather than
/// performance, and should therefore be used sparingly.
///
/// The showcase scenario for these operations is the total size computation for
/// some dynamic block of memory, which in case of a numeric overflow would
/// inadvertently be computed too small, leading to accesses beyond the block's
/// boundaries and potential data corruption or hard crashes further down the
/// road. Computation of pointers into that memory block, on the other hand,
/// would typically not need to be guarded in the same manner, as successful
/// computation of the block's total size usually guarantees that corresponding
/// computations of pointers will not overflow either.
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

#ifndef POVRAY_BASE_SAFEMATH_H
#define POVRAY_BASE_SAFEMATH_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// Standard C++ header files
#include <limits>

// POV-Ray base header files
#include "base/pov_err.h"

namespace pov_base
{

/// Multiply four (unsigned integer) factors, throwing an exception in case of numerical overflow.
template<typename T, typename T1, typename T2, typename T3, typename T4>
static inline T SafeUnsignedProduct(T1 p1, T2 p2, T3 p3, T4 p4)
{
    // the function is intended for use with unsigned integer parameters only
    // (NB: Instead of testing for (pN >= 0) we could also test for (!std::numeric_limits<TN>::is_signed),
    //  but this would make passing constant factors more cumbersome)
    POV_SAFEMATH_ASSERT(std::numeric_limits<T>::is_integer);
    POV_SAFEMATH_ASSERT(std::numeric_limits<T1>::is_integer && (p1 >= 0));
    POV_SAFEMATH_ASSERT(std::numeric_limits<T2>::is_integer && (p2 >= 0));
    POV_SAFEMATH_ASSERT(std::numeric_limits<T3>::is_integer && (p3 >= 0));
    POV_SAFEMATH_ASSERT(std::numeric_limits<T4>::is_integer && (p4 >= 0));

    // avoid divide-by-zero issues
    if ((p1==0) || (p2==0) || (p3==0) || (p4==0))
        return 0;

    if ( (((std::numeric_limits<T>::max() / p4) / p3) / p2) < p1 )
        throw POV_EXCEPTION_CODE(kNumericalLimitErr);

    return T(p1) * T(p2) * T(p3) * T(p4);
}

/// Multiply three (unsigned integer) factors, throwing an exception in case of numerical overflow.
template<typename T, typename T1, typename T2, typename T3>
static inline T SafeUnsignedProduct(T1 p1, T2 p2, T3 p3)
{
    return SafeUnsignedProduct<T,T1,T2,T3,unsigned int>(p1, p2, p3, 1u);
}

/// Multiply two (unsigned integer) factors, throwing an exception in case of numerical overflow.
template<typename T, typename T1, typename T2>
static inline T SafeUnsignedProduct(T1 p1, T2 p2)
{
    return SafeUnsignedProduct<T,T1,T2,unsigned int,unsigned int>(p1, p2, 1u, 1u);
}

#if 0 // not currently used, but I hesitate to throw it away [CLi]

/// Multiply up to four (signed integer) values, throwing an exception in case of numerical overflow.
/// @note: The function will also throw an exception if negating the result would overflow.
template<typename T>
static inline T SafeSignedProduct(T p1, T p2, T p3 = 1, T p4 = 1)
{
    // the function is intended for use with signed integer types only
    POV_SAFEMATH_ASSERT(std::numeric_limits<T>::is_integer);
    POV_SAFEMATH_ASSERT(std::numeric_limits<T>::is_signed);

    // avoid divide-by-zero issues
    if ((p1==0) || (p2==0) || (p3==0) || (p4==0))
        return 0;

    if (std::numeric_limits<T>::min() + std::numeric_limits<T>::max() == 0)
    {
        // integer representation appears to be sign-and-magnitude or one's complement; at any rate,
        // abs(pN) is guaranteed to be a safe operation, and so is x/abs(pN) (as we've made sure that
        // pN are all nonzero), and |::min()|==|::max()| is also guaranteed, i.e. the limits in the positive
        // and negative domain are equally stringent.
        if ( (((std::numeric_limits<T>::max() / abs(p4)) / abs(p3)) / abs(p2)) < abs(p1) )
            throw POV_EXCEPTION_CODE(kNumericalLimitErr);
    }
    else if (std::numeric_limits<T>::min() + std::numeric_limits<T>::max() < 0)
    {
        // integer representation appears to be two's complement; at any rate, abs(pN) is a potentially
        // unsafe operation, while -x is a safe operation for positive x; |::max()| > |::min()| is guaranteed,
        // i.e. the limits in the positive domain are more stringent than those in the negative one.

        // specifically handle situations in which abs(pN) would overflow
        // NB we're deliberately not testing for pN == std::numeric_limits<T>::min(), in order to make the test robust
        // against exotic integer representations
        if ((p1 < -std::numeric_limits<T>::max()) ||
            (p2 < -std::numeric_limits<T>::max()) ||
            (p3 < -std::numeric_limits<T>::max()) ||
            (p4 < -std::numeric_limits<T>::max()))
            throw POV_EXCEPTION_CODE(kNumericalLimitErr);

        // we've made sure that abs(pN) is a safe operation, and hence also x/abs(pN) (as we've also made sure that
        // all pN are nonzero); we also know that whatever is safe in the positive domain is also safe in the
        // negative domain
        if ( (((std::numeric_limits<T>::max() / abs(p4)) / abs(p3)) / abs(p2)) < abs(p1) )
            throw POV_EXCEPTION_CODE(kNumericalLimitErr);
    }
    else
    {
        // integer representation is exotic; abs(pN) is guaranteed to be a safe operation, and |::min()| > |::max()|
        // is guaranteed, i.e. the limits in the negative domain are more stringent than those in the positive one.

        // with abs(pN) a safe operation and having made sure all pN are non-zero, x/abs(pN) is guaranteed to be a safe
        // operation as well; we also know that whatever is safe in the negative domain is also safe in the
        // positive domain
        if ( (((abs(std::numeric_limits<T>::min()) / abs(p4)) / abs(p3)) / abs(p2)) < abs(p1) )
            throw POV_EXCEPTION_CODE(kNumericalLimitErr);
    }

    // product is safe, go ahead
    return p1 * p2 * p3 * p4;
}

#endif

}

#endif // POVRAY_BASE_SAFEMATH_H
