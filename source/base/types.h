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
 * $File: //depot/public/povray/3.x/source/base/types.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
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


template<typename T>
class GenericRGBColour;

#define RED_INTENSITY   0.297
#define GREEN_INTENSITY 0.589
#define BLUE_INTENSITY  0.114

template<typename T>
class GenericColour
{
	public:
		typedef DBL EXPRESS[5];
		typedef COLC COLOUR[5];
		typedef T DATA[5];

		enum
		{
			RED    = 0,
			GREEN  = 1,
			BLUE   = 2,
			FILTER = 3,
			TRANSM = 4
		};

		GenericColour()
		{
			colour[RED] = 0.0;
			colour[GREEN] = 0.0;
			colour[BLUE] = 0.0;
			colour[FILTER] = 0.0;
			colour[TRANSM] = 0.0;
		}

		explicit GenericColour(T grey)
		{
			colour[RED] = grey;
			colour[GREEN] = grey;
			colour[BLUE] = grey;
			colour[FILTER] = 0.0;
			colour[TRANSM] = 0.0;
		}

		explicit inline GenericColour(const GenericRGBColour<T>& col);

		explicit inline GenericColour(const GenericRGBColour<T>& col, T nfilter, T ntransm);

		GenericColour(T nred, T ngreen, T nblue)
		{
			colour[RED] = nred;
			colour[GREEN] = ngreen;
			colour[BLUE] = nblue;
			colour[FILTER] = 0.0;
			colour[TRANSM] = 0.0;
		}

		GenericColour(T nred, T ngreen, T nblue, T nfilter, T ntransm)
		{
			colour[RED] = nred;
			colour[GREEN] = ngreen;
			colour[BLUE] = nblue;
			colour[FILTER] = nfilter;
			colour[TRANSM] = ntransm;
		}

		explicit GenericColour(const COLOUR col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
			colour[FILTER] = col[FILTER];
			colour[TRANSM] = col[TRANSM];
		}

		explicit GenericColour(const EXPRESS col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
			colour[FILTER] = col[FILTER];
			colour[TRANSM] = col[TRANSM];
		}

		GenericColour(const GenericColour& col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
			colour[FILTER] = col[FILTER];
			colour[TRANSM] = col[TRANSM];
		}

		template<typename T2>
		explicit GenericColour(const GenericColour<T2>& col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
			colour[FILTER] = col[FILTER];
			colour[TRANSM] = col[TRANSM];
		}

		GenericColour(vector<POVMSFloat>::const_iterator& it, bool filter = true, bool transmit = true)
		{
			colour[RED] = *it++;
			colour[GREEN] = *it++;
			colour[BLUE] = *it++;
			if (filter)
				colour[FILTER] = *it++;
			if (transmit)
				colour[TRANSM] = *it++;
		}

		GenericColour& operator=(const GenericColour& col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
			colour[FILTER] = col[FILTER];
			colour[TRANSM] = col[TRANSM];
			return *this;
		}

		GenericColour& operator=(const T& col)
		{
			colour[RED] = col;
			colour[GREEN] = col;
			colour[BLUE] = col;
			colour[FILTER] = 0.0f;
			colour[TRANSM] = 0.0f;
			return *this;
		}

		T operator[](int idx) const { return colour[idx]; }
		T& operator[](int idx) { return colour[idx]; }

		const DATA& operator*() const { return colour; }
		DATA& operator*() { return colour; }

		T red() const { return colour[RED]; }
		T& red() { return colour[RED]; }

		T green() const { return colour[GREEN]; }
		T& green() { return colour[GREEN]; }

		T blue() const { return colour[BLUE]; }
		T& blue() { return colour[BLUE]; }

		T filter() const { return colour[FILTER]; }
		T& filter() { return colour[FILTER]; }

		T transm() const { return colour[TRANSM]; }
		T& transm() { return colour[TRANSM]; }

		T opacity() const { return 1.0 - colour[FILTER] - colour[TRANSM]; }

		T greyscale() const { return RED_INTENSITY * colour[RED] + GREEN_INTENSITY * colour[GREEN] + BLUE_INTENSITY * colour[BLUE]; }

		// TODO: find a more correct way of handling alpha <-> filter/transmit
		static void AtoFT(T alpha, T& f, T& t) { f = 0.0f; t = 1.0f - alpha; }
		void AtoFT(T alpha) { colour[FILTER] = 0.0f; colour[TRANSM] = 1.0f - alpha; }
		static T FTtoA(T /*f*/, T t) { return 1.0f - t; }
		T FTtoA() const { return 1.0f - colour[TRANSM]; }

		void clear()
		{
			colour[RED] = 0.0;
			colour[GREEN] = 0.0;
			colour[BLUE] = 0.0;
			colour[FILTER] = 0.0;
			colour[TRANSM] = 0.0;
		}

		void set(T grey)
		{
			colour[RED] = grey;
			colour[GREEN] = grey;
			colour[BLUE] = grey;
			colour[FILTER] = 0.0;
			colour[TRANSM] = 0.0;
		}

		void set(T nred, T ngreen, T nblue)
		{
			colour[RED] = nred;
			colour[GREEN] = ngreen;
			colour[BLUE] = nblue;
			colour[FILTER] = 0.0;
			colour[TRANSM] = 0.0;
		}

		void set(T nred, T ngreen, T nblue, T nfilter, T ntransm)
		{
			colour[RED] = nred;
			colour[GREEN] = ngreen;
			colour[BLUE] = nblue;
			colour[FILTER] = nfilter;
			colour[TRANSM] = ntransm;
		}

		GenericColour clip(T minc, T maxc)
		{
			return GenericColour(pov_base::clip<T>(colour[RED], minc, maxc),
			                     pov_base::clip<T>(colour[GREEN], minc, maxc),
			                     pov_base::clip<T>(colour[BLUE], minc, maxc),
			                     pov_base::clip<T>(colour[FILTER], minc, maxc),
			                     pov_base::clip<T>(colour[TRANSM], minc, maxc));
		}

		inline GenericRGBColour<T> rgbTransm() const;

		GenericColour operator+(const GenericColour& b) const
		{
			return GenericColour(colour[RED] + b[RED], colour[GREEN] + b[GREEN], colour[BLUE] + b[BLUE], colour[FILTER] + b[FILTER], colour[TRANSM] + b[TRANSM]);
		}

		GenericColour operator-(const GenericColour& b) const
		{
			return GenericColour(colour[RED] - b[RED], colour[GREEN] - b[GREEN], colour[BLUE] - b[BLUE], colour[FILTER] - b[FILTER], colour[TRANSM] - b[TRANSM]);
		}

		GenericColour operator*(const GenericColour& b) const
		{
			return GenericColour(colour[RED] * b[RED], colour[GREEN] * b[GREEN], colour[BLUE] * b[BLUE], colour[FILTER] * b[FILTER], colour[TRANSM] * b[TRANSM]);
		}

		GenericColour operator/(const GenericColour& b) const
		{
			return GenericColour(colour[RED] / b[RED], colour[GREEN] / b[GREEN], colour[BLUE] / b[BLUE], colour[FILTER] / b[FILTER], colour[TRANSM] / b[TRANSM]);
		}

		GenericColour& operator+=(const GenericColour& b)
		{
			colour[RED] += b[RED];
			colour[GREEN] += b[GREEN];
			colour[BLUE] += b[BLUE];
			colour[FILTER] += b[FILTER];
			colour[TRANSM] += b[TRANSM];
			return *this;
		}

		GenericColour& operator-=(const GenericColour& b)
		{
			colour[RED] -= b[RED];
			colour[GREEN] -= b[GREEN];
			colour[BLUE] -= b[BLUE];
			colour[FILTER] -= b[FILTER];
			colour[TRANSM] -= b[TRANSM];
			return *this;
		}

		GenericColour& operator*=(const GenericColour& b)
		{
			colour[RED] *= b[RED];
			colour[GREEN] *= b[GREEN];
			colour[BLUE] *= b[BLUE];
			colour[FILTER] *= b[FILTER];
			colour[TRANSM] *= b[TRANSM];
			return *this;
		}

		GenericColour& operator/=(const GenericColour& b)
		{
			colour[RED] /= b[RED];
			colour[GREEN] /= b[GREEN];
			colour[BLUE] /= b[BLUE];
			colour[FILTER] /= b[FILTER];
			colour[TRANSM] /= b[TRANSM];
			return *this;
		}

		GenericColour operator-() const
		{
			return GenericColour(-colour[RED], -colour[GREEN], -colour[BLUE], -colour[FILTER], -colour[TRANSM]);
		}

		GenericColour operator+(DBL b) const
		{
			return GenericColour(colour[RED] + b, colour[GREEN] + b, colour[BLUE] + b, colour[FILTER] + b, colour[TRANSM] + b);
		}

		GenericColour operator-(DBL b) const
		{
			return GenericColour(colour[RED] - b, colour[GREEN] - b, colour[BLUE] - b, colour[FILTER] - b, colour[TRANSM] - b);
		}

		GenericColour operator*(DBL b) const
		{
			return GenericColour(colour[RED] * b, colour[GREEN] * b, colour[BLUE] * b, colour[FILTER] * b, colour[TRANSM] * b);
		}

		GenericColour operator/(DBL b) const
		{
			return GenericColour(colour[RED] / b, colour[GREEN] / b, colour[BLUE] / b, colour[FILTER] / b, colour[TRANSM] / b);
		}

		GenericColour& operator+=(DBL b)
		{
			colour[RED] += b;
			colour[GREEN] += b;
			colour[BLUE] += b;
			colour[FILTER] += b;
			colour[TRANSM] += b;
			return *this;
		}

		GenericColour& operator-=(DBL b)
		{
			colour[RED] -= b;
			colour[GREEN] -= b;
			colour[BLUE] -= b;
			colour[FILTER] -= b;
			colour[TRANSM] -= b;
			return *this;
		}

		GenericColour& operator*=(DBL b)
		{
			colour[RED] *= b;
			colour[GREEN] *= b;
			colour[BLUE] *= b;
			colour[FILTER] *= b;
			colour[TRANSM] *= b;
			return *this;
		}

		GenericColour& operator/=(DBL b)
		{
			colour[RED] /= b;
			colour[GREEN] /= b;
			colour[BLUE] /= b;
			colour[FILTER] /= b;
			colour[TRANSM] /= b;
			return *this;
		}
	private:
		DATA colour;
};

template<typename T>
class GenericRGBColour
{
	public:
		typedef DBL VECTOR[3];
		typedef COLC RGB[3];
		typedef T DATA[3];

		enum
		{
			RED    = 0,
			GREEN  = 1,
			BLUE   = 2
		};

		GenericRGBColour()
		{
			colour[RED] = 0.0;
			colour[GREEN] = 0.0;
			colour[BLUE] = 0.0;
		}

		explicit GenericRGBColour(T grey)
		{
			colour[RED] = grey;
			colour[GREEN] = grey;
			colour[BLUE] = grey;
		}

		GenericRGBColour(T nred, T ngreen, T nblue)
		{
			colour[RED] = nred;
			colour[GREEN] = ngreen;
			colour[BLUE] = nblue;
		}

		explicit GenericRGBColour(const RGB col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
		}

		explicit GenericRGBColour(const VECTOR col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
		}

		explicit GenericRGBColour(const GenericColour<T>& col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
		}

		GenericRGBColour(const GenericRGBColour& col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
		}

		template<typename T2>
		explicit GenericRGBColour(const GenericRGBColour<T2>& col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
		}

		GenericRGBColour(vector<POVMSFloat>::const_iterator& it)
		{
			colour[RED] = *it++;
			colour[GREEN] = *it++;
			colour[BLUE] = *it++;
		}

		GenericRGBColour& operator=(const GenericRGBColour& col)
		{
			colour[RED] = col[RED];
			colour[GREEN] = col[GREEN];
			colour[BLUE] = col[BLUE];
			return *this;
		}

		GenericRGBColour& operator=(const T& col)
		{
			colour[RED] = col;
			colour[GREEN] = col;
			colour[BLUE] = col;
			return *this;
		}

		T operator[](int idx) const { return colour[idx]; }
		T& operator[](int idx) { return colour[idx]; }

		const DATA& operator*() const { return colour; }
		DATA& operator*() { return colour; }

		T red() const { return colour[RED]; }
		T& red() { return colour[RED]; }

		T green() const { return colour[GREEN]; }
		T& green() { return colour[GREEN]; }

		T blue() const { return colour[BLUE]; }
		T& blue() { return colour[BLUE]; }

		T greyscale() const { return RED_INTENSITY * colour[RED] + GREEN_INTENSITY * colour[GREEN] + BLUE_INTENSITY * colour[BLUE]; }

		bool isZero() const { return (colour[RED] == 0) && (colour[GREEN] == 0) && (colour[BLUE] == 0); }

		void clear()
		{
			colour[RED] = 0.0;
			colour[GREEN] = 0.0;
			colour[BLUE] = 0.0;
		}

		void set(T grey)
		{
			colour[RED] = grey;
			colour[GREEN] = grey;
			colour[BLUE] = grey;
		}

		void set(T nred, T ngreen, T nblue)
		{
			colour[RED] = nred;
			colour[GREEN] = ngreen;
			colour[BLUE] = nblue;
		}

		GenericRGBColour clip(T minc, T maxc)
		{
			return GenericRGBColour(pov_base::clip<T>(colour[RED], minc, maxc),
			                        pov_base::clip<T>(colour[GREEN], minc, maxc),
			                        pov_base::clip<T>(colour[BLUE], minc, maxc));
		}

		GenericRGBColour operator+(const GenericRGBColour& b) const
		{
			return GenericRGBColour(colour[RED] + b[RED], colour[GREEN] + b[GREEN], colour[BLUE] + b[BLUE]);
		}

		GenericRGBColour operator-(const GenericRGBColour& b) const
		{
			return GenericRGBColour(colour[RED] - b[RED], colour[GREEN] - b[GREEN], colour[BLUE] - b[BLUE]);
		}

		GenericRGBColour operator*(const GenericRGBColour& b) const
		{
			return GenericRGBColour(colour[RED] * b[RED], colour[GREEN] * b[GREEN], colour[BLUE] * b[BLUE]);
		}

		GenericRGBColour operator/(const GenericRGBColour& b) const
		{
			return GenericRGBColour(colour[RED] / b[RED], colour[GREEN] / b[GREEN], colour[BLUE] / b[BLUE]);
		}

		GenericRGBColour& operator+=(const GenericRGBColour& b)
		{
			colour[RED] += b[RED];
			colour[GREEN] += b[GREEN];
			colour[BLUE] += b[BLUE];
			return *this;
		}

		GenericRGBColour& operator-=(const GenericRGBColour& b)
		{
			colour[RED] -= b[RED];
			colour[GREEN] -= b[GREEN];
			colour[BLUE] -= b[BLUE];
			return *this;
		}

		GenericRGBColour& operator*=(const GenericRGBColour& b)
		{
			colour[RED] *= b[RED];
			colour[GREEN] *= b[GREEN];
			colour[BLUE] *= b[BLUE];
			return *this;
		}

		GenericRGBColour& operator/=(const GenericRGBColour& b)
		{
			colour[RED] /= b[RED];
			colour[GREEN] /= b[GREEN];
			colour[BLUE] /= b[BLUE];
			return *this;
		}

		GenericRGBColour operator-() const
		{
			return GenericRGBColour(-colour[RED], -colour[GREEN], -colour[BLUE]);
		}

		GenericRGBColour operator+(DBL b) const
		{
			return GenericRGBColour(colour[RED] + b, colour[GREEN] + b, colour[BLUE] + b);
		}

		GenericRGBColour operator-(DBL b) const
		{
			return GenericRGBColour(colour[RED] - b, colour[GREEN] - b, colour[BLUE] - b);
		}

		GenericRGBColour operator*(DBL b) const
		{
			return GenericRGBColour(colour[RED] * b, colour[GREEN] * b, colour[BLUE] * b);
		}

		GenericRGBColour operator/(DBL b) const
		{
			return GenericRGBColour(colour[RED] / b, colour[GREEN] / b, colour[BLUE] / b);
		}

		GenericRGBColour& operator+=(DBL b)
		{
			colour[RED] += b;
			colour[GREEN] += b;
			colour[BLUE] += b;
			return *this;
		}

		GenericRGBColour& operator-=(DBL b)
		{
			colour[RED] -= b;
			colour[GREEN] -= b;
			colour[BLUE] -= b;
			return *this;
		}

		GenericRGBColour& operator*=(DBL b)
		{
			colour[RED] *= b;
			colour[GREEN] *= b;
			colour[BLUE] *= b;
			return *this;
		}

		GenericRGBColour& operator/=(DBL b)
		{
			colour[RED] /= b;
			colour[GREEN] /= b;
			colour[BLUE] /= b;
			return *this;
		}
	private:
		DATA colour;
};

template<typename T>
inline GenericColour<T>::GenericColour(const GenericRGBColour<T>& col)
{
	colour[RED] = col[RED];
	colour[GREEN] = col[GREEN];
	colour[BLUE] = col[BLUE];
	colour[FILTER] = 0.0;
	colour[TRANSM] = 0.0;
}

template<typename T>
inline GenericColour<T>::GenericColour(const GenericRGBColour<T>& col, T nfilter, T ntransm)
{
	colour[RED] = col[RED];
	colour[GREEN] = col[GREEN];
	colour[BLUE] = col[BLUE];
	colour[FILTER] = nfilter;
	colour[TRANSM] = ntransm;
}

template<typename T>
inline GenericRGBColour<T> GenericColour<T>::rgbTransm() const
{
	return GenericRGBColour<T>( colour[RED]   * colour[FILTER] + colour[TRANSM],
	                            colour[GREEN] * colour[FILTER] + colour[TRANSM],
	                            colour[BLUE]  * colour[FILTER] + colour[TRANSM] );
}

template<typename T>
inline GenericColour<T> operator* (double a, const GenericColour<T>& b) { return b * a; }
template<typename T>
inline GenericRGBColour<T> operator* (double a, const GenericRGBColour<T>& b) { return b * a; }

template<typename T>
inline double colourDistance (const GenericColour<T>& a, const GenericColour<T>& b) { return fabs(a.red() - b.red()) + fabs(a.green() - b.green()) + fabs(a.blue() - b.blue()); }
template<typename T>
inline double colourDistance (const GenericRGBColour<T>& a, const GenericRGBColour<T>& b) { return fabs(a.red() - b.red()) + fabs(a.green() - b.green()) + fabs(a.blue() - b.blue()); }

template<typename T>
inline GenericRGBColour<T> Sqr(const GenericRGBColour<T>& a) { return a * a; }
template<typename T>
inline GenericRGBColour<T> exp(const GenericRGBColour<T>& a) { return GenericRGBColour<T>(::exp(a.red()), ::exp(a.green()), ::exp(a.blue())); }
template<typename T>
inline GenericRGBColour<T> sqrt(const GenericRGBColour<T>& a) { return GenericRGBColour<T>(::sqrt(a.red()), ::sqrt(a.green()), ::sqrt(a.blue())); }

typedef GenericColour<COLC>    Colour;
typedef GenericColour<DBL>     DblColour;
typedef GenericRGBColour<COLC> RGBColour;
typedef GenericRGBColour<DBL>  DblRGBColour;

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
