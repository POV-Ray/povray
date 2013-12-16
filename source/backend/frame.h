/*******************************************************************************
 * frame.h
 *
 * This header file is included by all C modules in POV-Ray. It defines all
 * globally-accessible types and constants.
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
 * $File: //depot/povray/smp/source/backend/frame.h $
 * $Revision: #138 $
 * $Change: 6162 $
 * $DateTime: 2013/12/07 19:55:09 $
 * $Author: clipka $
 *******************************************************************************/

#ifndef FRAME_H
#define FRAME_H

// Generic header for all modules

#include <new>

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <vector>
#include <stack>

#ifndef MATH_H_INCLUDED
#include <math.h>
#endif

#include "base/configbase.h"
#include "base/types.h"
#include "base/colour.h"

#include "backend/configbackend.h"
#include "backend/support/simplevector.h"
#include "backend/control/messagefactory.h"
#include "backend/colour/spectral.h"

#include "pov_mem.h"

namespace pov
{

using namespace pov_base;
using std::min;
using std::max;

// these defines affect the maximum size of some types based on FixedSimpleVector

#ifndef MEDIA_VECTOR_SIZE
#define MEDIA_VECTOR_SIZE               256
#endif

#ifndef MEDIA_INTERVAL_VECTOR_SIZE
#define MEDIA_INTERVAL_VECTOR_SIZE      256
#endif

#ifndef LIT_INTERVAL_VECTOR_SIZE
#define LIT_INTERVAL_VECTOR_SIZE        512
#endif

#ifndef LIGHT_INTERSECTION_VECTOR_SIZE
#define LIGHT_INTERSECTION_VECTOR_SIZE  512
#endif

#ifndef LIGHTSOURCE_VECTOR_SIZE
#define LIGHTSOURCE_VECTOR_SIZE         1024
#endif

#ifndef WEIGHTEDTEXTURE_VECTOR_SIZE
#define WEIGHTEDTEXTURE_VECTOR_SIZE     512
#endif

#ifndef RAYINTERIOR_VECTOR_SIZE
#define RAYINTERIOR_VECTOR_SIZE         512
#endif

/*****************************************************************************
 *
 * Typedefs that need to be known here.
 *
 *****************************************************************************/

class ObjectBase;
typedef ObjectBase * ObjectPtr;
typedef const ObjectBase * ConstObjectPtr;

class CompoundObject;


/*****************************************************************************
 *
 * Scalar, color and vector stuff.
 *
 *****************************************************************************/

typedef DBL UV_VECT[2];
typedef DBL VECTOR[3];
typedef DBL VECTOR_4D[4];
typedef DBL MATRIX[4][4];
typedef DBL EXPRESS[5];
typedef COLC COLOUR[5];
typedef COLC RGB[3];

// Vector array elements.
enum
{
	U = 0,
	V = 1
};

enum
{
	X = 0,
	Y = 1,
	Z = 2,
	T = 3,
	W = 3
};

// Color array elements.
enum
{
	pRED    = 0,
	pGREEN  = 1,
	pBLUE   = 2,
	pFILTER = 3,
	pTRANSM = 4
};

// Macros to manipulate scalars, vectors, and colors.

inline void Assign_Vector(VECTOR d, const VECTOR s)
{
	d[X] = s[X];
	d[Y] = s[Y];
	d[Z] = s[Z];
}

inline void Assign_UV_Vect(UV_VECT d, const UV_VECT s)
{
	d[X] = s[X];
	d[Y] = s[Y];
}

inline void Assign_Vector_4D(VECTOR_4D d, const VECTOR_4D s)
{
	d[X] = s[X];
	d[Y] = s[Y];
	d[Z] = s[Z];
	d[T] = s[T];
}

// TODO - obsolete
inline void Assign_Colour(COLOUR d, const COLOUR s)
{
	d[pRED] = s[pRED];
	d[pGREEN] = s[pGREEN];
	d[pBLUE] = s[pBLUE];
	d[pFILTER] = s[pFILTER];
	d[pTRANSM] = s[pTRANSM];
}

// TODO - obsolete
inline void Assign_Colour_Express(COLOUR d, const EXPRESS s)
{
	d[pRED] = s[pRED];
	d[pGREEN] = s[pGREEN];
	d[pBLUE] = s[pBLUE];
	d[pFILTER] = s[pFILTER];
	d[pTRANSM] = s[pTRANSM];
}

inline void Assign_Express(EXPRESS d, const EXPRESS s)
{
	d[pRED] = s[pRED];
	d[pGREEN] = s[pGREEN];
	d[pBLUE] = s[pBLUE];
	d[pFILTER] = s[pFILTER];
	d[pTRANSM] = s[pTRANSM];
}

// TODO - obsolete
inline void Make_ColourA(COLOUR c, COLC r, COLC g, COLC b, COLC a, COLC t)
{
	c[pRED] = r;
	c[pGREEN] = g;
	c[pBLUE] = b;
	c[pFILTER] = a;
	c[pTRANSM] = t;
}

inline void Make_Vector(VECTOR v, DBL a, DBL b, DBL c)
{
	v[X] = a;
	v[Y] = b;
	v[Z] = c;
}

inline void Make_UV_Vector(UV_VECT v, DBL a, DBL b)
{
	v[U] = a;
	v[V] = b;
}

inline void Destroy_Float(DBL *x)
{
	if(x != NULL)
		POV_FREE(x);
}

inline void Destroy_Vector_4D(VECTOR_4D *x)
{
	if(x != NULL)
		POV_FREE(x);
}

// TODO - obsolete
inline void Destroy_Colour(COLOUR *x)
{
	if(x != NULL)
		POV_FREE(x);
}

#if 0
#pragma mark * Vector2d
#endif

template<typename T>
class GenericVector3d;

template<typename T>
class GenericVector2d
{
	public:

		typedef T UV_VECT_T[2];

		GenericVector2d()
		{
			vect[X] = 0.0;
			vect[Y] = 0.0;
		}

		explicit GenericVector2d(T d)
		{
			vect[X] = d;
			vect[Y] = d;
		}

		GenericVector2d(T x, T y)
		{
			vect[X] = x;
			vect[Y] = y;
		}

		explicit GenericVector2d(const UV_VECT vi)
		{
			vect[X] = T(vi[X]);
			vect[Y] = T(vi[Y]);
		}

		explicit GenericVector2d(const GenericVector3d<T>& b);

		template<typename T2>
		explicit GenericVector2d(const GenericVector2d<T2>& b)
		{
			vect[X] = T(b[X]);
			vect[Y] = T(b[Y]);
		}

		GenericVector2d(const GenericVector2d& b)
		{
			vect[X] = b[X];
			vect[Y] = b[Y];
		}

		GenericVector2d& operator=(const GenericVector2d& b)
		{
			vect[X] = b[X];
			vect[Y] = b[Y];
			return *this;
		}

		T operator[](int idx) const { return vect[idx]; }
		T& operator[](int idx) { return vect[idx]; }

		GenericVector2d operator+(const GenericVector2d& b) const
		{
			return GenericVector2d(vect[X] + b[X], vect[Y] + b[Y]);
		}

		GenericVector2d operator-(const GenericVector2d& b) const
		{
			return GenericVector2d(vect[X] - b[X], vect[Y] - b[Y]);
		}

		GenericVector2d operator*(const GenericVector2d& b) const
		{
			return GenericVector2d(vect[X] * b[X], vect[Y] * b[Y]);
		}

		GenericVector2d operator/(const GenericVector2d& b) const
		{
			return GenericVector2d(vect[X] / b[X], vect[Y] / b[Y]);
		}

		GenericVector2d& operator+=(const GenericVector2d& b)
		{
			vect[X] += b[X];
			vect[Y] += b[Y];
			return *this;
		}

		GenericVector2d& operator-=(const GenericVector2d& b)
		{
			vect[X] -= b[X];
			vect[Y] -= b[Y];
			return *this;
		}

		GenericVector2d& operator*=(const GenericVector2d& b)
		{
			vect[X] *= b[X];
			vect[Y] *= b[Y];
			return *this;
		}

		GenericVector2d& operator/=(const GenericVector2d& b)
		{
			vect[X] /= b[X];
			vect[Y] /= b[Y];
			return *this;
		}

		GenericVector2d operator-() const
		{
			return GenericVector2d(-vect[X], -vect[Y]);
		}

		GenericVector2d operator+(T b) const
		{
			return GenericVector2d(vect[X] + b, vect[Y] + b);
		}

		GenericVector2d operator-(T b) const
		{
			return GenericVector2d(vect[X] - b, vect[Y] - b);
		}

		GenericVector2d operator*(T b) const
		{
			return GenericVector2d(vect[X] * b, vect[Y] * b);
		}

		GenericVector2d operator/(T b) const
		{
			return GenericVector2d(vect[X] / b, vect[Y] / b);
		}

		GenericVector2d& operator+=(T b)
		{
			vect[X] += b;
			vect[Y] += b;
			return *this;
		}

		GenericVector2d& operator-=(T b)
		{
			vect[X] -= b;
			vect[Y] -= b;
			return *this;
		}

		GenericVector2d& operator*=(T b)
		{
			vect[X] *= b;
			vect[Y] *= b;
			return *this;
		}

		GenericVector2d& operator/=(T b)
		{
			vect[X] /= b;
			vect[Y] /= b;
			return *this;
		}

		const UV_VECT_T& operator*() const { return vect; }
		UV_VECT_T& operator*() { return vect; }

		T x() const { return vect[X]; }
		T& x() { return vect[X]; }

		T y() const { return vect[Y]; }
		T& y() { return vect[Y]; }

		T u() const { return vect[X]; }
		T& u() { return vect[X]; }

		T v() const { return vect[Y]; }
		T& v() { return vect[Y]; }

		T length() const
		{
			return sqrt(vect[X] * vect[X] + vect[Y] * vect[Y]);
		}
		T lengthSqr() const
		{
			return vect[X] * vect[X] + vect[Y] * vect[Y];
		}
		GenericVector2d normalized() const
		{
			T l = length();
			if (l != 0)
				return *this / l;
			else
				return *this;
		}
		void normalize()
		{
			T l = length();
			if (l != 0)
				*this /= l;
			// no else
		}

	private:
		UV_VECT_T vect;
};

#if 0
#pragma mark * Vector3d
#endif

template<typename T>
class GenericVector3d
{
	public:

		typedef T VECTOR_T[3];

		GenericVector3d()
		{
			vect[X] = 0.0;
			vect[Y] = 0.0;
			vect[Z] = 0.0;
		}

		explicit GenericVector3d(T d)
		{
			vect[X] = d;
			vect[Y] = d;
			vect[Z] = d;
		}

		GenericVector3d(T x, T y, T z)
		{
			vect[X] = x;
			vect[Y] = y;
			vect[Z] = z;
		}

		explicit GenericVector3d(const VECTOR vi)
		{
			vect[X] = T(vi[X]);
			vect[Y] = T(vi[Y]);
			vect[Z] = T(vi[Z]);
		}

		template<typename T2>
		explicit GenericVector3d(const GenericVector3d<T2>& b)
		{
			vect[X] = T(b[X]);
			vect[Y] = T(b[Y]);
			vect[Z] = T(b[Z]);
		}

		GenericVector3d(const GenericVector3d& b)
		{
			vect[X] = b[X];
			vect[Y] = b[Y];
			vect[Z] = b[Z];
		}

		GenericVector3d& operator=(const GenericVector3d& b)
		{
			vect[X] = b[X];
			vect[Y] = b[Y];
			vect[Z] = b[Z];
			return *this;
		}

		T operator[](int idx) const { return vect[idx]; }
		T& operator[](int idx) { return vect[idx]; }

		GenericVector3d operator+(const GenericVector3d& b) const
		{
			return GenericVector3d(vect[X] + b[X], vect[Y] + b[Y], vect[Z] + b[Z]);
		}

		GenericVector3d operator-(const GenericVector3d& b) const
		{
			return GenericVector3d(vect[X] - b[X], vect[Y] - b[Y], vect[Z] - b[Z]);
		}

		GenericVector3d operator*(const GenericVector3d& b) const
		{
			return GenericVector3d(vect[X] * b[X], vect[Y] * b[Y], vect[Z] * b[Z]);
		}

		GenericVector3d operator/(const GenericVector3d& b) const
		{
			return GenericVector3d(vect[X] / b[X], vect[Y] / b[Y], vect[Z] / b[Z]);
		}

		GenericVector3d& operator+=(const GenericVector3d& b)
		{
			vect[X] += b[X];
			vect[Y] += b[Y];
			vect[Z] += b[Z];
			return *this;
		}

		GenericVector3d& operator-=(const GenericVector3d& b)
		{
			vect[X] -= b[X];
			vect[Y] -= b[Y];
			vect[Z] -= b[Z];
			return *this;
		}

		GenericVector3d& operator*=(const GenericVector3d& b)
		{
			vect[X] *= b[X];
			vect[Y] *= b[Y];
			vect[Z] *= b[Z];
			return *this;
		}

		GenericVector3d& operator/=(const GenericVector3d& b)
		{
			vect[X] /= b[X];
			vect[Y] /= b[Y];
			vect[Z] /= b[Z];
			return *this;
		}

		GenericVector3d operator-() const
		{
			return GenericVector3d(-vect[X], -vect[Y], -vect[Z]);
		}

		GenericVector3d operator+(T b) const
		{
			return GenericVector3d(vect[X] + b, vect[Y] + b, vect[Z] + b);
		}

		GenericVector3d operator-(T b) const
		{
			return GenericVector3d(vect[X] - b, vect[Y] - b, vect[Z] - b);
		}

		GenericVector3d operator*(T b) const
		{
			return GenericVector3d(vect[X] * b, vect[Y] * b, vect[Z] * b);
		}

		GenericVector3d operator/(T b) const
		{
			return GenericVector3d(vect[X] / b, vect[Y] / b, vect[Z] / b);
		}

		GenericVector3d& operator+=(T b)
		{
			vect[X] += b;
			vect[Y] += b;
			vect[Z] += b;
			return *this;
		}

		GenericVector3d& operator-=(T b)
		{
			vect[X] -= b;
			vect[Y] -= b;
			vect[Z] -= b;
			return *this;
		}

		GenericVector3d& operator*=(T b)
		{
			vect[X] *= b;
			vect[Y] *= b;
			vect[Z] *= b;
			return *this;
		}

		GenericVector3d& operator/=(T b)
		{
			vect[X] /= b;
			vect[Y] /= b;
			vect[Z] /= b;
			return *this;
		}

		const VECTOR_T& operator*() const { return vect; }
		VECTOR_T& operator*() { return vect; }

		T x() const { return vect[X]; }
		T& x() { return vect[X]; }

		T y() const { return vect[Y]; }
		T& y() { return vect[Y]; }

		T z() const { return vect[Z]; }
		T& z() { return vect[Z]; }

		T length() const
		{
			return sqrt(vect[X] * vect[X] + vect[Y] * vect[Y] + vect[Z] * vect[Z]);
		}
		T lengthSqr() const
		{
			return vect[X] * vect[X] + vect[Y] * vect[Y] + vect[Z] * vect[Z];
		}
		GenericVector3d normalized() const
		{
			T l = length();
			if (l != 0)
				return *this / l;
			else
				return *this;
		}
		void normalize()
		{
			T l = length();
			if (l != 0)
				*this /= l;
			// no else
		}
		void invert()
		{
			vect[X] = -vect[X];
			vect[Y] = -vect[Y];
			vect[Z] = -vect[Z];
		}

	private:
		VECTOR_T vect;
};

typedef GenericVector2d<DBL> Vector2d;
typedef GenericVector2d<SNGL> SnglVector2d;

typedef GenericVector3d<DBL> Vector3d;
typedef GenericVector3d<SNGL> SnglVector3d;

typedef Vector3d Matrix3x3[3];

template<typename T>
inline T dot(const GenericVector2d<T>& a, const GenericVector2d<T>& b)
{
	return (a.x() * b.x()) + (a.y() * b.y());
}

template<typename T>
inline T dot(const GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
	return ((a.x() * b.x()) + (a.y() * b.y()) + (a.z() * b.z()));
}

template<typename T>
inline GenericVector3d<T> cross(const GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
	return GenericVector3d<T>( ((a.y() * b.z()) - (a.z() * b.y())),
	                           ((a.z() * b.x()) - (a.x() * b.z())),
	                           ((a.x() * b.y()) - (a.y() * b.x())) );
}

template<typename T>
inline GenericVector3d<T> midpoint(const GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
	return 0.5 * (a + b);
}

template<typename T>
inline bool similar(const GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
	return ( fabs(a.x()-b.x()) + fabs(a.y()-b.y()) + fabs(a.z()-b.z()) < EPSILON );
}

template<typename T>
inline GenericVector2d<T> operator* (T a, const GenericVector2d<T>& b) { return b * a; }
template<typename T>
inline GenericVector3d<T> operator* (T a, const GenericVector3d<T>& b) { return b * a; }

template<typename T>
inline GenericVector3d<T> min(GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
	return GenericVector3d<T>( std::min(a[X], b[X]),
	                           std::min(a[Y], b[Y]),
	                           std::min(a[Z], b[Z]) );
}

template<typename T>
inline GenericVector3d<T> min(GenericVector3d<T>& a, const GenericVector3d<T>& b, const GenericVector3d<T>& c)
{
	return GenericVector3d<T>( std::min(a[X], std::min(b[X], c[X])),
	                           std::min(a[Y], std::min(b[Y], c[Y])),
	                           std::min(a[Z], std::min(b[Z], c[Z])) );
}

template<typename T>
inline GenericVector3d<T> min(GenericVector3d<T>& a, const GenericVector3d<T>& b, const GenericVector3d<T>& c, const GenericVector3d<T>& d)
{
	return GenericVector3d<T>( std::min(a[X], std::min(b[X], std::min(c[X], d[X]))),
	                           std::min(a[Y], std::min(b[Y], std::min(c[Y], d[Y]))),
	                           std::min(a[Z], std::min(b[Z], std::min(c[Z], d[Z]))) );
}

template<typename T>
inline GenericVector3d<T> max(GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
	return GenericVector3d<T>( std::max(a[X], b[X]),
	                           std::max(a[Y], b[Y]),
	                           std::max(a[Z], b[Z]) );
}

template<typename T>
inline GenericVector3d<T> max(GenericVector3d<T>& a, const GenericVector3d<T>& b, const GenericVector3d<T>& c)
{
	return GenericVector3d<T>( std::max(a[X], std::max(b[X], c[X])),
	                           std::max(a[Y], std::max(b[Y], c[Y])),
	                           std::max(a[Z], std::max(b[Z], c[Z])) );
}

template<typename T>
inline GenericVector3d<T> max(GenericVector3d<T>& a, const GenericVector3d<T>& b, const GenericVector3d<T>& c, const GenericVector3d<T>& d)
{
	return GenericVector3d<T>( std::max(a[X], std::max(b[X], std::max(c[X], d[X]))),
	                           std::max(a[Y], std::max(b[Y], std::max(c[Y], d[Y]))),
	                           std::max(a[Z], std::max(b[Z], std::max(c[Z], d[Z]))) );
}

template<typename T>
inline GenericVector2d<T>::GenericVector2d(const GenericVector3d<T>& b)
{
	vect[X] = b[X];
	vect[Y] = b[Y];
}

typedef pov_base::Colour Colour;

/*****************************************************************************
 *
 * Bounding box stuff (see also BOUND.H).
 *
 *****************************************************************************/

#if 0
#pragma mark * Bounding
#endif

typedef SNGL BBoxScalar;
typedef GenericVector3d<BBoxScalar> BBoxVector3d;

struct BoundingBox
{
	BBoxVector3d lowerLeft;
	BBoxVector3d size;

	SNGL GetMinX() const { return lowerLeft[X]; }
	SNGL GetMinY() const { return lowerLeft[Y]; }
	SNGL GetMinZ() const { return lowerLeft[Z]; }

	SNGL GetMaxX() const { return lowerLeft[X] + size[X]; }
	SNGL GetMaxY() const { return lowerLeft[Y] + size[Y]; }
	SNGL GetMaxZ() const { return lowerLeft[Z] + size[Z]; }
};

inline void Make_BBox(BoundingBox& BBox, const BBoxScalar llx, const BBoxScalar lly, const BBoxScalar llz, const BBoxScalar lex, const BBoxScalar ley, const BBoxScalar lez)
{
	BBox.lowerLeft = BBoxVector3d(llx, lly, llz);
	BBox.size      = BBoxVector3d(lex, ley, lez);
}

inline void Make_BBox_from_min_max(BoundingBox& BBox, const BBoxVector3d& mins, const BBoxVector3d& maxs)
{
	BBox.lowerLeft = mins;
	BBox.size      = maxs - mins;
}

inline void Make_BBox_from_min_max(BoundingBox& BBox, const Vector3d& mins, const Vector3d& maxs)
{
	BBox.lowerLeft = BBoxVector3d(mins);
	BBox.size      = BBoxVector3d(maxs - mins);
}

inline void Make_min_max_from_BBox(BBoxVector3d& mins, BBoxVector3d& maxs, const BoundingBox& BBox)
{
	mins = BBox.lowerLeft;
	maxs = mins + BBox.size;
}

inline void Make_min_max_from_BBox(Vector3d& mins, Vector3d& maxs, const BoundingBox& BBox)
{
	mins = Vector3d(BBox.lowerLeft);
	maxs = mins + Vector3d(BBox.size);
}

inline bool Inside_BBox(const Vector3d& point, const BoundingBox& bbox)
{
	if (point[X] < (DBL)bbox.lowerLeft[X])
		return(false);
	if (point[Y] < (DBL)bbox.lowerLeft[Y])
		return(false);
	if (point[Z] < (DBL)bbox.lowerLeft[Z])
		return(false);
	if (point[X] > (DBL)bbox.lowerLeft[X] + (DBL)bbox.size[X])
		return(false);
	if (point[Y] > (DBL)bbox.lowerLeft[Y] + (DBL)bbox.size[Y])
		return(false);
	if (point[Z] > (DBL)bbox.lowerLeft[Z] + (DBL)bbox.size[Z])
		return(false);

	return(true);
}


struct MinMaxBoundingBox
{
	BBoxVector3d pmin;
	BBoxVector3d pmax;
};

/*****************************************************************************
 *
 * Transformation stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Transform
#endif

typedef struct Transform_Struct TRANSFORM;

struct Transform_Struct
{
	MATRIX matrix;
	MATRIX inverse;
};



/*****************************************************************************
 *
 * Color map stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Blend Map
#endif

const int MAX_BLEND_MAP_ENTRIES = 256;

typedef struct Blend_Map_Entry BLEND_MAP_ENTRY;
typedef struct Blend_Map_Struct BLEND_MAP;
typedef struct Pattern_Struct TPATTERN;
typedef struct Texture_Struct TEXTURE;
typedef struct Pigment_Struct PIGMENT;
typedef struct Tnormal_Struct TNORMAL;
typedef struct Finish_Struct FINISH;
typedef struct Turb_Struct TURB;
typedef struct Warps_Struct WARP;
typedef struct Spline_Entry SPLINE_ENTRY;
typedef struct Spline_Struct SPLINE;

struct Blend_Map_Entry
{
	SNGL value;
	unsigned char Same;
	union
	{
		COLOUR colour;
		PIGMENT *Pigment;
		TNORMAL *Tnormal;
		TEXTURE *Texture;
		UV_VECT Point_Slope;
	} Vals;
};

struct Blend_Map_Struct
{
	int Users;                              ///< -1 for default blend maps (prevents them from being destroyed)
	short Number_Of_Entries;
	char Transparency_Flag, Type;
	BLEND_MAP_ENTRY *Blend_Map_Entries;
};

inline void Make_Blend_Map_Entry(BLEND_MAP_ENTRY& entry, SNGL v, unsigned char s, COLC r, COLC g, COLC b, COLC a, COLC t)
{
	entry.value = v;
	entry.Same = s;
	Make_ColourA(entry.Vals.colour, r, g, b, a, t);
}


/*****************************************************************************
 *
 * Media and Interior stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Media, Interior
#endif

class Media
{
	public:
		int Type;
		int Intervals;
		int Min_Samples;
		int Max_Samples;
		unsigned Sample_Method : 8;
		bool is_constant : 1;
		bool use_absorption : 1;
		bool use_emission : 1;
		bool use_extinction : 1;
		bool use_scattering : 1;
		bool ignore_photons : 1;
		DBL Jitter;
		DBL Eccentricity;
		DBL sc_ext;
		RGBColour Absorption;
		RGBColour Emission;
		RGBColour Extinction;
		RGBColour Scattering;

		DBL Ratio;
		DBL Confidence;
		DBL Variance;
		DBL *Sample_Threshold;

		DBL AA_Threshold;
		int AA_Level;

		vector<PIGMENT*> Density;

		Media();
		Media(const Media&);
		~Media();

		Media& operator=(const Media&);

		void Transform(const TRANSFORM *trans);

		void PostProcess();
};

class SubsurfaceInterior;
class Interior
{
	public:
		int References;
		int  hollow, Disp_NElems;
		SNGL IOR, Dispersion;
		SNGL Caustics, Old_Refract;
		SNGL Fade_Distance, Fade_Power;
		RGBColour Fade_Colour;
		vector<Media> media;
		shared_ptr<SubsurfaceInterior> subsurface;

		Interior();
		Interior(const Interior&);
		~Interior();

		void Transform(const TRANSFORM *trans);

		void PostProcess();
	private:
		Interior& operator=(const Interior&);
};



/*****************************************************************************
 *
 * Spline stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Spline
#endif

struct Spline_Entry
{
	DBL par;      // Parameter
	DBL vec[5];   // Value at the parameter
	DBL coeff[5]; // Interpolating coefficients at the parameter
};

struct Spline_Struct
{
	int Number_Of_Entries, Type;
	int Max_Entries;
	SPLINE_ENTRY *SplineEntries;
	int Coeffs_Computed;
	int Terms;
/* [JG] flyspray #294 : cache is not thread-safe 
 * (and seems useless, as slower than without it)
	bool Cache_Valid;
	int Cache_Type;
	DBL Cache_Point;
	EXPRESS Cache_Data;
 */
	int ref_count;
};

typedef struct Spline_Entry SPLINE_ENTRY;


/*****************************************************************************
 *
 * Image stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Image
#endif

// Legal image attributes.

#define NO_FILE         0x00000000
#define GIF_FILE        0x00000001
#define POT_FILE        0x00000002
#define SYS_FILE        0x00000004
#define IFF_FILE        0x00000008
#define TGA_FILE        0x00000010
#define GRAD_FILE       0x00000020
#define PGM_FILE        0x00000040
#define PPM_FILE        0x00000080
#define PNG_FILE        0x00000100
#define JPEG_FILE       0x00000200
#define TIFF_FILE       0x00000400
#define BMP_FILE        0x00000800
#define EXR_FILE        0x00001000
#define HDR_FILE        0x00002000

// Image types.

#define IMAGE_FILE    GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define NORMAL_FILE   GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define MATERIAL_FILE GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+IFF_FILE+GRAD_FILE
#define HF_FILE       GIF_FILE+SYS_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE+JPEG_FILE+TIFF_FILE+BMP_FILE+EXR_FILE+HDR_FILE+POT_FILE

#define PIGMENT_TYPE  0
#define NORMAL_TYPE   1
#define PATTERN_TYPE  2
#define TEXTURE_TYPE  4
#define COLOUR_TYPE   5
#define SLOPE_TYPE    6
#define DENSITY_TYPE  7

#define DEFAULT_FRACTAL_EXTERIOR_TYPE 1
#define DEFAULT_FRACTAL_INTERIOR_TYPE 0
#define DEFAULT_FRACTAL_EXTERIOR_FACTOR 1
#define DEFAULT_FRACTAL_INTERIOR_FACTOR 1


/*****************************************************************************
 *
 * Pigment, Tnormal, Finish, Texture & Warps stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Pigment, Normal, Finish, Texture, Warp
#endif

struct BasicPattern;

typedef shared_ptr<BasicPattern> PatternPtr;
typedef shared_ptr<const BasicPattern> ConstPatternPtr;


struct Pattern_Struct
{
	unsigned short Type;
	unsigned short Flags;
	PatternPtr pattern;
	BLEND_MAP *Blend_Map;
};

struct Pigment_Struct : public Pattern_Struct
{
	Colour colour;       // may have a filter/transmit component
	Colour Quick_Colour; // may have a filter/transmit component    // TODO - can't we decide between regular colour and quick_colour at parse time already?
};

struct Tnormal_Struct : public Pattern_Struct
{
	SNGL Amount;
	SNGL Delta; // NK delta
};

struct Texture_Struct : public Pattern_Struct
{
	int References;
	TEXTURE *Next;
	PIGMENT *Pigment;
	TNORMAL *Tnormal;
	FINISH *Finish;
	vector<TEXTURE*> Materials; // used for BITMAP_PATTERN (and only there)
};

struct Finish_Struct
{
	SNGL Diffuse, DiffuseBack, RawDiffuse, RawDiffuseBack, Brilliance;
	SNGL Specular, Roughness;
	SNGL Phong, Phong_Size;
	SNGL Irid, Irid_Film_Thickness, Irid_Turb;
	SNGL Temp_Caustics, Temp_IOR, Temp_Dispersion, Temp_Refract, Reflect_Exp;
	SNGL Crand, Metallic;
	RGBColour Ambient, Emission, Reflection_Max, Reflection_Min;
	RGBColour SubsurfaceTranslucency, SubsurfaceAnisotropy;
	//RGBColour SigmaPrimeS, SigmaA;
	SNGL Reflection_Falloff;  // Added by MBP 8/27/98
	int Reflection_Type;  // Added by MBP 9/5/98
	SNGL Reflect_Metallic; // MBP
	int Conserve_Energy;  // added by NK Dec 19 1999
	bool UseSubsurface;   // whether to use subsurface light transport
};

struct Warps_Struct
{
	unsigned short Warp_Type;
	WARP *Prev_Warp;
	WARP *Next_Warp;
};

struct Turb_Struct : public Warps_Struct
{
	Vector3d Turbulence;
	int Octaves;
	SNGL Lambda, Omega;
};

#define Destroy_Finish(x) if ((x)!=NULL) POV_FREE(x)

typedef struct Material_Struct MATERIAL;

struct Material_Struct
{
   TEXTURE *Texture;
   TEXTURE * Interior_Texture;
   Interior *interior;
};

/*****************************************************************************
 *
 * Object stuff (see also OBJECTS.H and primitive include files).
 *
 *****************************************************************************/

#if 0
#pragma mark * Object
#endif

#ifndef DUMP_OBJECT_DATA
#define DUMP_OBJECT_DATA 0
#endif

// TODO FIXME
class SceneThreadData;
typedef SceneThreadData TraceThreadData;

// These fields are common to all objects.
class LightSource;
class Ray;
class Intersection;

template<typename T>
class RefPool
{
	public:
		RefPool() { }
		~RefPool() { for(typename vector<T*>::iterator i(pool.begin()); i != pool.end(); i++) delete *i; pool.clear(); }

		T *alloc() { if(pool.empty()) return new T(); T *ptr(pool.back()); pool.pop_back(); return ptr; }
		void release(T *ptr) { pool.push_back(ptr); }
	private:
		vector<T*> pool;

		RefPool(const RefPool&);
		RefPool& operator=(RefPool&);
};

template<typename T>
struct RefClearDefault
{
	void operator()(T&) { }
};

template<typename T>
struct RefClearContainer
{
	void operator()(T& p) { p.clear(); }
};

template<typename T, class C = RefClearDefault<T> >
class Ref
{
	public:
		Ref(RefPool<T>& p) : pool(p), ptr(p.alloc()) { }
		~Ref() { C c; c(*ptr); pool.release(ptr); }

		T& operator*() { return *ptr; }
		const T& operator*() const { return *ptr; }

		T *operator->() { return ptr; }
		const T *operator->() const { return ptr; }
	private:
		RefPool<T>& pool;
		T *ptr;

		Ref();
		Ref(const Ref&);
		Ref& operator=(Ref&);
};

typedef stack<Intersection, vector<Intersection> > IStackData;
typedef RefPool<IStackData> IStackPool;
typedef Ref<IStackData> IStack;

struct WeightedTexture
{
	COLC weight;
	TEXTURE *texture;

	WeightedTexture(COLC w, TEXTURE *t) :
		weight(w), texture(t) { }
};

// TODO: these sizes will need tweaking.
typedef FixedSimpleVector<WeightedTexture, WEIGHTEDTEXTURE_VECTOR_SIZE> WeightedTextureVector;
typedef FixedSimpleVector<Interior *, RAYINTERIOR_VECTOR_SIZE> RayInteriorVector;

class ObjectDebugHelper
{
	public:
		int Index;
		bool IsCopy;
		std::string Tag;

		ObjectDebugHelper() { Index = ObjectIndex++; IsCopy = false; }
		ObjectDebugHelper& operator=(const ObjectDebugHelper& src) { Index = ObjectIndex++; IsCopy = true; Tag = src.Tag; return *this; }

		std::string& SimpleDesc (std::string& result);
	private:
		static int ObjectIndex;
		ObjectDebugHelper(const ObjectDebugHelper& src) { Index = ObjectIndex++; IsCopy = true; Tag = src.Tag; }
};

class ObjectBase
{
	public:
		int Type;
		TEXTURE *Texture;
		TEXTURE *Interior_Texture;
		Interior *interior;
		vector<ObjectPtr> Bound;
		vector<ObjectPtr> Clip;
		vector<LightSource *> LLights;  ///< Used for light groups.
		BoundingBox BBox;
		TRANSFORM *Trans;
		TRANSFORM *UV_Trans;
		SNGL Ph_Density;
		FloatSetting RadiosityImportance;
		unsigned int Flags;

#ifdef OBJECT_DEBUG_HELPER
		ObjectDebugHelper Debug;
#endif
		enum BBoxDirection
		{
			BBOX_DIR_X0Y0Z0 = 0,
			BBOX_DIR_X0Y0Z1 = 1,
			BBOX_DIR_X0Y1Z0 = 2,
			BBOX_DIR_X0Y1Z1 = 3,
			BBOX_DIR_X1Y0Z0 = 4,
			BBOX_DIR_X1Y0Z1 = 5,
			BBOX_DIR_X1Y1Z0 = 6,
			BBOX_DIR_X1Y1Z1 = 7
		};

		ObjectBase(int t) :
			Type(t),
			Texture(NULL), Interior_Texture(NULL), interior(NULL), Trans(NULL), UV_Trans(NULL),
			Ph_Density(0), RadiosityImportance(0.0), Flags(0)
		{
			Make_BBox(BBox, -BOUND_HUGE/2.0, -BOUND_HUGE/2.0, -BOUND_HUGE/2.0, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
		}

		ObjectBase(int t, ObjectBase& o, bool transplant) :
			Type(t),
			Texture(o.Texture), Interior_Texture(o.Interior_Texture), interior(o.interior), Trans(o.Trans), UV_Trans(o.UV_Trans),
			Ph_Density(o.Ph_Density), RadiosityImportance(o.RadiosityImportance), Flags(o.Flags),
			Bound(o.Bound), Clip(o.Clip), LLights(o.LLights), BBox(o.BBox)
		{
			if (transplant)
			{
				o.Texture = NULL;
				o.Interior_Texture = NULL;
				o.interior = NULL;
				o.Trans = NULL;
				o.UV_Trans = NULL;
				o.Bound.clear();
				o.Clip.clear();
				o.LLights.clear();
			}
		}
		virtual ~ObjectBase() { }

		virtual ObjectPtr Copy() = 0;

		virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) = 0; // could be "const", if it wasn't for isosurface max_gradient estimation stuff
		virtual bool Inside(const Vector3d&, TraceThreadData *) const = 0;
		virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const = 0;
		virtual void UVCoord(Vector2d&, const Intersection *, TraceThreadData *) const;
		virtual void Translate(const Vector3d&, const TRANSFORM *) = 0;
		virtual void Rotate(const Vector3d&, const TRANSFORM *) = 0;
		virtual void Scale(const Vector3d&, const TRANSFORM *) = 0;
		virtual void Transform(const TRANSFORM *) = 0;
		virtual void Invert() = 0;
		virtual void Compute_BBox() = 0;
		virtual void Determine_Textures(Intersection *, bool, WeightedTextureVector&, TraceThreadData *Thread); // could be "(const Intersection*...) const" if it wasn't for blob specials

		/// Checks whether a given ray intersects the object's bounding box.
		/// Primitives with low-cost intersection tests may override this to always return true
		virtual bool Intersect_BBox(BBoxDirection, const BBoxVector3d&, const BBoxVector3d&, BBoxScalar = HUGE_VAL) const;

		// optional post-render message dispatcher; will be called upon completion
		// of rendering a view. this is the appropriate place to send messages that
		// would traditionally have been sent at the end of a render or at object
		// destruction - e.g. IsoSurface max_gradient warnings. (object destruction
		// isn't the place to do that anymore since a scene may persist between views).
		virtual void DispatchShutdownMessages(MessageFactory& messageFactory) {};

	protected:
		explicit ObjectBase(const ObjectBase&) { }
};

/* This is an abstract structure that is never actually used.
   All other objects are descendents of this primitive type */

class CompoundObject : public ObjectBase
{
	public:
		CompoundObject(int t) : ObjectBase(t) {}
		CompoundObject(int t, CompoundObject& o, bool transplant) : ObjectBase(t, o, transplant), children(o.children) { if (transplant) o.children.clear(); }
		vector<ObjectPtr> children;
};

typedef struct BBox_Tree_Struct BBOX_TREE;

struct BBox_Tree_Struct
{
	short Infinite;   // Flag if node is infinite
	short Entries;    // Number of sub-nodes in this node
	BoundingBox BBox; // Bounding box of this node
	BBOX_TREE **Node; // If node: children; if leaf: element
};

typedef struct Project_Struct PROJECT;
typedef struct Project_Tree_Node_Struct PROJECT_TREE_NODE;

struct Project_Struct
{
	int x1, y1, x2, y2;
};

/*
 * The following structure represent the bounding box hierarchy in 2d space.
 * Because is_leaf, Object and Project are the first elements in both
 * structures they can be accessed without knowing at which structure
 * a pointer is pointing.
 */

struct Project_Tree_Node_Struct
{
	unsigned short is_leaf;
	BBOX_TREE *Node;
	PROJECT Project;
	unsigned short Entries;
	PROJECT_TREE_NODE **Entry;
};

#if 0
#pragma mark * Light Source
#endif

class LightSource : public CompoundObject
{
	public:
		size_t index;
		RGBColour colour;
		Vector3d Direction, Center, Points_At, Axis1, Axis2;
		DBL Coeff, Radius, Falloff;
		DBL Fade_Distance, Fade_Power;
		int Area_Size1, Area_Size2;
		int Adaptive_Level;
		ObjectBase *Projected_Through_Object;
		BLEND_MAP *blend_map;// NK for dispersion

		unsigned Light_Type : 8;
		bool Area_Light : 1;
		bool Use_Full_Area_Lighting : 1; // JN2007: Full area lighting
		bool Jitter : 1;
		bool Orient : 1;
		bool Circular : 1;
		bool Parallel : 1;
		bool Photon_Area_Light : 1;
		bool Media_Attenuation : 1;
		bool Media_Interaction : 1;
		bool lightGroupLight : 1;

		LightSource();
		virtual ~LightSource();

		virtual ObjectPtr Copy();

		virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
		virtual bool Inside(const Vector3d&, TraceThreadData *) const;
		virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;
		virtual void UVCoord(Vector2d&, const Intersection *, TraceThreadData *) const;
		virtual void Translate(const Vector3d&, const TRANSFORM *);
		virtual void Rotate(const Vector3d&, const TRANSFORM *);
		virtual void Scale(const Vector3d&, const TRANSFORM *);
		virtual void Transform(const TRANSFORM *);
		virtual void Invert();
		virtual void Compute_BBox() { }
};

typedef unsigned short HF_VAL;


/*****************************************************************************
 *
 * Intersection stack stuff.
 *
 *****************************************************************************/

#if 0
#pragma mark * Intersection
#endif

/**
 *  Ray-object intersection data.
 *  This class holds various information on a ray-object intersection.
 */
class Intersection
{
	public:
		/// Distance from the intersecting ray's origin.
		DBL Depth;
		/// Point of the intersection in global coordinate space.
		Vector3d IPoint;
		/// Unpertubed surface normal at the intersection point.
		/// @note This is not necessarily the true geometric surface normal, as it may include fake smoothing.
		/// @note This value is invalid if haveNormal is false.
		/// @todo We should have two distinct vectors: A true geometric one, and a separate one for faked smoothing.
		Vector3d INormal;
		/// Perturbed normal vector (set during texture evaluation).
		Vector3d PNormal;
		/// UV texture coordinate.
		Vector2d Iuv;
		/// Intersected object.
		ObjectBase *Object;

		/// @name Object-Specific Auxiliary Data
		/// These members hold information specific to particular object types, typically generated during
		/// intersection testing, to be re-used later for normal and/or UV coordinate computation.
		/// @note These values may be invalid or meaningless depending on the type of object intersected.
		//@{
		/// Point of the intersection in local coordinate space (used by Blob)
		/// @note This value is invalid if haveLocalIPoint is false.
		Vector3d LocalIPoint;
		/// Flag to indicate whether INormal was computed during intersection testing (used by HField)
		/// @note Objects either always or never computing INormal during intersection testing don't use this flag.
		bool haveNormal;
		/// Flag to indicate whether LocalIPoint has already been computed.
		bool haveLocalIPoint;
		/// Generic auxiliary integer data #1 (used by Sor, Prism, Lathe, Cones, Boxes)
		int i1;
		/// Generic auxiliary integer data #2 (used by Sor, Prism)
		int i2;
		/// Generic auxiliary float data #1 (used by Prism, Lathe)
		DBL d1;
		/// Generic auxiliary pointer data (used by Mesh)
		const void *Pointer;
		//@}

		/// Root-level parent CSG object for cutaway textures.
		ObjectBase *Csg;

		Intersection() :
			Depth(BOUND_HUGE), Object(NULL), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, ObjectBase *o) :
			Depth(d), Object(o), IPoint(v), Iuv(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, const Vector3d& n, ObjectBase *o) :
			Depth(d), Object(o), IPoint(v), Iuv(v), INormal(n), haveNormal(true), haveLocalIPoint(false), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, const Vector2d& uv, ObjectBase *o) :
			Depth(d), Object(o), IPoint(v), Iuv(uv), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, const Vector3d& n, const Vector2d& uv, ObjectBase *o) :
			Depth(d), Object(o), IPoint(v), INormal(n), Iuv(uv), haveNormal(true), haveLocalIPoint(false), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, ObjectBase *o, const void *a) :
			Depth(d), Object(o), Pointer(a), IPoint(v), Iuv(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, const Vector2d& uv, ObjectBase *o, const void *a) :
			Depth(d), Object(o), Pointer(a), IPoint(v), Iuv(uv), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, ObjectBase *o, int a) :
			Depth(d), Object(o), i1(a), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, ObjectBase *o, DBL a) :
			Depth(d), Object(o), d1(a), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, ObjectBase *o, int a, int b) :
			Depth(d), Object(o), i1(a), i2(b), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, ObjectBase *o, int a, DBL b) :
			Depth(d), Object(o), i1(a), d1(b), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
		{}

		Intersection(DBL d, const Vector3d& v, ObjectBase *o, int a, int b, DBL c) :
			Depth(d), Object(o), i1(a), i2(b), d1(c), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
		{}

		~Intersection() { }
};

/*****************************************************************************
 *
 * Ray stuff (see also RAY.H).
 *
 *****************************************************************************/

#if 0
#pragma mark * Ray
#endif

struct BasicRay
{
	Vector3d Origin;
	Vector3d Direction;

	inline BasicRay() {}
	inline BasicRay(const BasicRay& obj) : Origin(obj.Origin), Direction(obj.Direction) {}
	inline BasicRay(const Vector3d& ov, const Vector3d& dv) : Origin(ov), Direction(dv) {}
	inline Vector3d Evaluate(double depth) const { return Origin + Direction * depth; }

	/// Make sure the ray's direction is normalized.
	/// @return     The length of the direction vector before normalization.
	inline DBL Normalize() { DBL len = Direction.length(); Direction /= len; return len; }

	inline const Vector3d& GetOrigin() const { return Origin; }
	inline const Vector3d& GetDirection() const { return Direction; }

	inline Vector3d& GetOrigin() { return Origin; }
	inline Vector3d& GetDirection() { return Direction; }
};

struct TraceTicket;

class Ray : public BasicRay
{
	public:
		enum RayType
		{
			OtherRay = 0,
			PrimaryRay = 1,
			ReflectionRay = 2,
			RefractionRay = 3,
			SubsurfaceRay = 4,  ///< Ray is shot from just below a surface; very close intersections shall not be suppressed.
		};

		Ray(TraceTicket& ticket, RayType rt = PrimaryRay, bool shadowTest = false, bool photon = false, bool radiosity = false, bool monochromatic = false, bool pretrace = false);
		Ray(TraceTicket& ticket, const Vector3d& ov, const Vector3d& dv, RayType rt = PrimaryRay, bool shadowTest = false, bool photon = false, bool radiosity = false, bool monochromatic = false, bool pretrace = false);
		~Ray();

		void AppendInterior(Interior *i);
		void AppendInteriors(RayInteriorVector&);
		bool RemoveInterior(const Interior *i);
		void ClearInteriors() { interiors.clear(); }

		bool IsInterior(const Interior *i) const;
		const RayInteriorVector& GetInteriors() const { return interiors; }
		RayInteriorVector& GetInteriors() { return interiors; }

		void SetSpectralBand(const SpectralBand&);
		const SpectralBand& GetSpectralBand() const;

		void SetFlags(RayType rt, bool shadowTest = false, bool photon = false, bool radiosity = false, bool monochromatic = false, bool pretrace = false);
		void SetFlags(RayType rt, const Ray& other);

		bool IsPrimaryRay() const { return primaryRay; }
		bool IsImageRay() const { return primaryRay || (refractionRay && !reflectionRay); }
		bool IsReflectionRay() const { return reflectionRay; }
		bool IsRefractionRay() const { return refractionRay; }
		bool IsSubsurfaceRay() const { return subsurfaceRay; }
		bool IsShadowTestRay() const { return shadowTestRay; }
		bool IsPhotonRay() const { return photonRay; }
		bool IsRadiosityRay() const { return radiosityRay; }
		bool IsMonochromaticRay() const { return monochromaticRay; }
		bool IsHollowRay() const { return hollowRay; }
		bool IsPretraceRay() const { return pretraceRay; }

		bool Inside(const BoundingBox& bbox) const { return Inside_BBox(Origin, bbox); }

		inline TraceTicket& GetTicket() { return ticket; }
		inline const TraceTicket& GetTicket() const { return ticket; }

	private:
		RayInteriorVector interiors;
		SpectralBand spectralBand;
		TraceTicket& ticket;

		bool primaryRay : 1;
		bool reflectionRay : 1;
		bool refractionRay : 1;
		bool subsurfaceRay : 1;
		bool shadowTestRay : 1;
		bool photonRay : 1;
		bool radiosityRay : 1;
		bool monochromaticRay : 1;
		bool hollowRay : 1;
		bool pretraceRay : 1;
};

struct RayObjectCondition
{
	virtual bool operator()(const Ray& ray, const ObjectBase* object, DBL data) const = 0;
};

struct TrueRayObjectCondition : public RayObjectCondition
{
	virtual bool operator()(const Ray&, const ObjectBase*, DBL) const { return true; }
};

struct PointObjectCondition
{
	virtual bool operator()(const Vector3d& point, const ObjectBase* object) const = 0;
};

struct TruePointObjectCondition : public PointObjectCondition
{
	virtual bool operator()(const Vector3d&, const ObjectBase*) const { return true; }
};


/*****************************************************************************
 *
 * Frame tracking information
 *
 *****************************************************************************/

enum FRAMETYPE
{
	FT_SINGLE_FRAME,
	FT_MULTIPLE_FRAME
};

#define INT_VALUE_UNSET (-1)
#define DBL_VALUE_UNSET (-1.0)

struct FrameSettings
{
	FRAMETYPE FrameType;
	DBL Clock_Value;      // May change between frames of an animation
	int FrameNumber;      // May change between frames of an animation

	int InitialFrame;
	DBL InitialClock;

	int FinalFrame;
	int FrameNumWidth;
	DBL FinalClock;

	int SubsetStartFrame;
	DBL SubsetStartPercent;
	int SubsetEndFrame;
	DBL SubsetEndPercent;

	bool Field_Render_Flag;
	bool Odd_Field_Flag;
};


/*****************************************************************************
 *
 * Miscellaneous stuff.
 *
 *****************************************************************************/

typedef struct complex_block complex;

struct Chunk_Header_Struct
{
	int name;
	int size;
};

struct complex_block
{
	DBL r, c;
};

struct BYTE_XYZ
{
	unsigned char x, y, z;
};

inline void VUnpack(Vector3d& dest_vec, const BYTE_XYZ * pack_vec)
{
	dest_vec[X] = ((double)pack_vec->x * (1.0 / 255.0)) * 2.0 - 1.0;
	dest_vec[Y] = ((double)pack_vec->y * (1.0 / 255.0));
	dest_vec[Z] = ((double)pack_vec->z * (1.0 / 255.0)) * 2.0 - 1.0;

	dest_vec.normalize(); // already good to about 1%, but we can do better
}

class Fractal;

class FractalRules
{
	public:
		virtual ~FractalRules() {}
		virtual void CalcNormal (Vector3d&, int, const Fractal *, DBL **) const = 0;
		virtual bool Iterate (const Vector3d&, const Fractal *, DBL **) const = 0;
		virtual bool Iterate (const Vector3d&, const Fractal *, const Vector3d&, DBL *, DBL **) const = 0;
		virtual bool Bound (const BasicRay&, const Fractal *, DBL *, DBL *) const = 0;
};

typedef shared_ptr<FractalRules> FractalRulesPtr;


// platform-specific headers should have provided DECLARE_THREAD_LOCAL_PTR.
// if not, we default to using the boost thread_specific_ptr class.
#ifndef DECLARE_THREAD_LOCAL_PTR
#define DECLARE_THREAD_LOCAL_PTR(ptrType, ptrName)                boost::thread_specific_ptr<ptrType> ptrName
#define IMPLEMENT_THREAD_LOCAL_PTR(ptrType, ptrName, ptrCleanup)  boost::thread_specific_ptr<ptrType> ptrName(ptrCleanup)
#define GET_THREAD_LOCAL_PTR(ptrName)                             (ptrName.get())
#define SET_THREAD_LOCAL_PTR(ptrName, ptrValue)                   (ptrName.reset(ptrValue))
#endif

}

#endif
