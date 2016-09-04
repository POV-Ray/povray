//******************************************************************************
///
/// @file base/mathutil.h
///
/// Declaration of various utility functions for scalar math.
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

#ifndef POVRAY_BASE_MATHUTIL_H
#define POVRAY_BASE_MATHUTIL_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

#include <assert.h>

#include "base/types.h"

namespace pov_base
{

#ifdef NEED_INVHYP
DBL asinh(DBL x);
DBL acosh(DBL x);
DBL atanh(DBL x);
#endif

// Get minimum/maximum of three values.
template<typename T>
inline T max3(T x, T y, T z) { return max(x, max(y, z)); }
template<typename T>
inline T min3(T x, T y, T z) { return min(x, min(y, z)); }

template<typename T>
inline T clip(T val, T minv, T maxv);

template<typename T>
inline T clip(T val, T minv, T maxv)
{
    if(val < minv)
        return minv;
    else if(val > maxv)
        return maxv;
    else
        return val;
}

// clip a value to the range of an integer type
template<typename T, typename T2>
inline T clipToType(T2 val)
{
    return (T)clip<T2>(val, std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
}

// force a value's precision to a given type, even if computations are normally done with extended precision
// (such as GNU Linux on 32-bit CPU, which uses 80-bit extended double precision)
// TODO - we might make this code platform-specific
template<typename T>
inline T forcePrecision(T val)
{
    volatile T tempVal;
    tempVal = val;
    return tempVal;
}

// wrap floating-point value into the range [0..upperLimit);
// (this is equivalent to fmod() for positive values, but not for negative ones)
template<typename T>
inline T wrap(T val, T upperLimit)
{
    T tempVal = fmod(val, upperLimit);
    // NB: The range of the value computed by fmod() should be in the range [0..upperLimit) already,
    // but on some architectures may actually be in the range [0..upperLimit].

    if (tempVal < T(0.0))
    {
        // For negative values, fmod() returns a value in the range [-upperLimit..0];
        // transpose it into the range [0..upperLimit].
        tempVal += upperLimit;
    }

    // for negative values (and also for positive values on systems that internally use higher precision
    // than double for computations) we may end up with value equal to upperLimit (in double precision);
    // make sure to wrap these special cases to the range [0..upperLimit) as well.
    if (forcePrecision<double>(tempVal) >= upperLimit)
        tempVal = T(0.0);

    // sanity check; this should never kick in, unless wrap() has an implementation error.
    POV_MATHUTIL_ASSERT((tempVal >= 0.0) && (tempVal < upperLimit));

    return tempVal;
}

// wrap signed integer value into the range [0..upperLimit);
// (this is equivalent to the modulus operator for positive values, but not for negative ones)
template<typename T>
inline T wrapInt(T val, T upperLimit)
{
    T tempVal = val % upperLimit;

    if (tempVal < T(0))
    {
        // For negative values, the modulus operator may return a value in the range [1-upperLimit..-1];
        // transpose such results into the range [1..upperLimit-1].
        tempVal += upperLimit;
    }

    // sanity check; this should never kick in, unless wrapInt() has an implementation error.
    POV_MATHUTIL_ASSERT((tempVal >= 0) && (tempVal < upperLimit));

    return tempVal;
}

// round up/down to a multiple of some value
template<typename T1, typename T2>
inline T1 RoundDownToMultiple(T1 x, T2 base) { return x - (x % base); }
template<typename T1, typename T2>
inline T1 RoundUpToMultiple(T1 x, T2 base) { return RoundDownToMultiple (x + base - 1, base); }

}

#endif // POVRAY_BASE_MATHUTIL_H
