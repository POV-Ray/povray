/*******************************************************************************
 * types.h
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
 * $File: //depot/povray/smp/source/base/types.h $
 * $Revision: #42 $
 * $Change: 6089 $
 * $DateTime: 2013/11/11 09:18:35 $
 * $Author: clipka $
 *******************************************************************************/

#ifndef POVRAY_BASE_TYPES_H
#define POVRAY_BASE_TYPES_H

#include <algorithm>
#include <vector>

#include "base/configbase.h"

namespace pov_base
{

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

// wrap value into the range [0..upperLimit);
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

	return tempVal;
}

// round up/down to a multiple of some value
template<typename T1, typename T2>
inline T1 RoundDownToMultiple(T1 x, T2 base) { return x - (x % base); }
template<typename T1, typename T2>
inline T1 RoundUpToMultiple(T1 x, T2 base) { return RoundDownToMultiple (x + base - 1, base); }

struct POVRect
{
	unsigned int top;
	unsigned int left;
	unsigned int bottom;
	unsigned int right;

	POVRect() : top(0), left(0), bottom(0), right(0) { }
	POVRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) :
		top(y1), left(x1), bottom(y2), right(x2) { }

	unsigned int GetArea() const { return ((bottom - top + 1) * (right - left + 1)); }
	unsigned int GetWidth() const { return (right - left + 1); }
	unsigned int GetHeight() const { return (bottom - top + 1); }
};

class GenericSetting
{
	public:
		explicit GenericSetting(bool set = false): set(set) {}
		void Unset() { set = false; }
		bool isSet() const { return set; }
	protected:
		bool set;
};

class FloatSetting : public GenericSetting
{
	public:
		explicit FloatSetting(double data = 0.0, bool set = false): data(data), GenericSetting(set) {}
		double operator=(double b)          { data = b; set = true; return data; }
		operator double() const             { return data; }
		double operator()(double def) const { if (set) return data; else return def; }
	private:
		double  data;
};

}

#endif // POVRAY_BASE_TYPES_H
