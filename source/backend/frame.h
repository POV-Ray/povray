//******************************************************************************
///
/// @file backend/frame.h
///
/// Generic header for all back-end modules.
///
/// This header file is included by all C++ modules in the POV-Ray back-end.
/// It defines all globally-accessible types and constants.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2014 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef FRAME_H
#define FRAME_H

/// @file
///
/// @todo   The size of this file, and it being grouped into various thematical subsections,
///         indicate that its content should be split up into multiple files, to be included from
///         this one; furthermore, some contents may not really be that ubiquitous to warrant
///         including them in every module.

#include <new>

#include <cstdio>
#include <cstring>
#include <climits>

#include <vector>
#include <stack>

// TODO - what is this conditional for?
#ifndef MATH_H_INCLUDED
#include <cmath>
#endif

#include "base/configbase.h"
#include "base/types.h"
#include "base/colour.h"

#include "backend/configbackend.h"
#include "backend/support/simplevector.h"
#include "backend/colour/spectral.h"

#include "pov_mem.h"

namespace pov
{

using namespace pov_base;
using std::min;
using std::max;

/// @name FixedSimpleVector Sizes
/// @{
///
/// These defines affect the maximum size of some types based on @ref pov::FixedSimpleVector.

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

/// @}
///
//******************************************************************************
///
/// @name Forward Declarations
/// @{

class ObjectBase;
typedef ObjectBase * ObjectPtr;
typedef const ObjectBase * ConstObjectPtr;

class CompoundObject;

/// @}
///
//******************************************************************************
///
/// @name Scalar, Colour and Vector Stuff
///
/// @{

typedef DBL UV_VECT[2];   ///< @deprecated Use @ref pov::Vector2d instead.
                          ///< @todo       Get entirely rid of this.
typedef DBL VECTOR_4D[4]; ///< @todo       Make this obsolete.
typedef DBL MATRIX[4][4]; ///< @todo       Make this obsolete.
typedef DBL EXPRESS[5];   ///< @todo       Make this obsolete.
typedef COLC COLOUR[5];   ///< @deprecated Use @ref pov_base::Colour instead.
                          ///< @todo       Get entirely rid of this.
typedef COLC RGB[3];      ///< @deprecated Use @ref pov_base::RGBColour instead.
                          ///< @todo       Get entirely rid of this.

/// 2D Vector array elements.
/// @deprecated When using @ref pov::GenericVector2d, call the x() and y() access functions
///             instead of using the index operator with one of these as parameter.
enum
{
    U = 0,
    V = 1
};

/// 3D and 4D Vector array elements.
/// @deprecated When using @ref pov::GenericVector3d, call the x(), y() and z() access functions
///             instead of using the index operator with one of these as parameter.
enum
{
    X = 0,
    Y = 1,
    Z = 2,
    T = 3,
    W = 3
};

/// RGB and RGBFT Colour array elements.
/// @deprecated When using @ref pov_base::GenericColour or @ref pov_base::GenericRGBColour, call the
///             red(), green(), blue(), filter() and transm() access functions instead of using the
///             index operator with one of these as parameter.
enum
{
    pRED    = 0,
    pGREEN  = 1,
    pBLUE   = 2,
    pFILTER = 3,
    pTRANSM = 4
};

/// @deprecated See @ref pov::UV_VECT.
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

/// @deprecated See @ref pov::COLOUR.
inline void Assign_Colour(COLOUR d, const COLOUR s)
{
    d[pRED] = s[pRED];
    d[pGREEN] = s[pGREEN];
    d[pBLUE] = s[pBLUE];
    d[pFILTER] = s[pFILTER];
    d[pTRANSM] = s[pTRANSM];
}

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

/// @deprecated See @ref pov::COLOUR.
inline void Make_ColourA(COLOUR c, COLC r, COLC g, COLC b, COLC a, COLC t)
{
    c[pRED] = r;
    c[pGREEN] = g;
    c[pBLUE] = b;
    c[pFILTER] = a;
    c[pTRANSM] = t;
}

/// @deprecated See @ref pov::UV_VECT.
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

/// @deprecated See @ref pov::COLOUR.
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

/// Generic template class to hold a 2D vector.
///
/// @tparam T   Floating-point type to use for the individual vector components.
///
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

/// Generic template class to hold a 3D vector.
///
/// @tparam T   Floating-point type to use for the individual vector components.
///
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

        explicit GenericVector3d(const EXPRESS vi)
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

typedef GenericVector2d<DBL> Vector2d;      ///< Double-precision 2D vector.
typedef GenericVector2d<SNGL> SnglVector2d; ///< Single-precision 2D vector.

typedef GenericVector3d<DBL> Vector3d;      ///< Double-precision 3D vector.
typedef GenericVector3d<SNGL> SnglVector3d; ///< Single-precision 3D vector.

typedef Vector3d Matrix3x3[3];              ///< Double-precision 3x3 matrix.

///@relates GenericVector2d
template<typename T>
inline T dot(const GenericVector2d<T>& a, const GenericVector2d<T>& b)
{
    return (a.x() * b.x()) + (a.y() * b.y());
}

///@relates GenericVector3d
template<typename T>
inline T dot(const GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
    return ((a.x() * b.x()) + (a.y() * b.y()) + (a.z() * b.z()));
}

///@relates GenericVector3d
template<typename T>
inline GenericVector3d<T> cross(const GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
    return GenericVector3d<T>( ((a.y() * b.z()) - (a.z() * b.y())),
                               ((a.z() * b.x()) - (a.x() * b.z())),
                               ((a.x() * b.y()) - (a.y() * b.x())) );
}

///@relates GenericVector3d
template<typename T>
inline GenericVector3d<T> midpoint(const GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
    return 0.5 * (a + b);
}

///@relates GenericVector3d
template<typename T>
inline bool similar(const GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
    return ( fabs(a.x()-b.x()) + fabs(a.y()-b.y()) + fabs(a.z()-b.z()) < EPSILON );
}

///@relates GenericVector2d
template<typename T>
inline GenericVector2d<T> operator* (T a, const GenericVector2d<T>& b) { return b * a; }

///@relates GenericVector3d
template<typename T>
inline GenericVector3d<T> operator* (T a, const GenericVector3d<T>& b) { return b * a; }

///@relates GenericVector3d
template<typename T>
inline GenericVector3d<T> min(GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
    return GenericVector3d<T>( std::min(a[X], b[X]),
                               std::min(a[Y], b[Y]),
                               std::min(a[Z], b[Z]) );
}

///@relates GenericVector3d
template<typename T>
inline GenericVector3d<T> min(GenericVector3d<T>& a, const GenericVector3d<T>& b, const GenericVector3d<T>& c)
{
    return GenericVector3d<T>( std::min(a[X], std::min(b[X], c[X])),
                               std::min(a[Y], std::min(b[Y], c[Y])),
                               std::min(a[Z], std::min(b[Z], c[Z])) );
}

///@relates GenericVector3d
template<typename T>
inline GenericVector3d<T> min(GenericVector3d<T>& a, const GenericVector3d<T>& b, const GenericVector3d<T>& c, const GenericVector3d<T>& d)
{
    return GenericVector3d<T>( std::min(a[X], std::min(b[X], std::min(c[X], d[X]))),
                               std::min(a[Y], std::min(b[Y], std::min(c[Y], d[Y]))),
                               std::min(a[Z], std::min(b[Z], std::min(c[Z], d[Z]))) );
}

///@relates GenericVector3d
template<typename T>
inline GenericVector3d<T> max(GenericVector3d<T>& a, const GenericVector3d<T>& b)
{
    return GenericVector3d<T>( std::max(a[X], b[X]),
                               std::max(a[Y], b[Y]),
                               std::max(a[Z], b[Z]) );
}

///@relates GenericVector3d
template<typename T>
inline GenericVector3d<T> max(GenericVector3d<T>& a, const GenericVector3d<T>& b, const GenericVector3d<T>& c)
{
    return GenericVector3d<T>( std::max(a[X], std::max(b[X], c[X])),
                               std::max(a[Y], std::max(b[Y], c[Y])),
                               std::max(a[Z], std::max(b[Z], c[Z])) );
}

///@relates GenericVector3d
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

/// @}
///
//******************************************************************************
///
/// @name Bounding Box Stuff
/// @{

#if 0
#pragma mark * Bounding
#endif

typedef SNGL BBoxScalar;
typedef GenericVector3d<BBoxScalar> BBoxVector3d;

/// Structure holding bounding box data.
///
/// @note       The current implementation stores the data in lowerLeft/size format.
///
/// @todo       The reliability of the bounding mechanism could probably be improved by storing the
///             bounding data in min/max format rather than lowerLeft/size, and making sure
///             high-precision values are rounded towards positive/negative infinity as appropriate.
///
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

    bool isEmpty() const { return (size[X] < 0) || (size[Y] < 0) || (size[Z] < 0); }
};

/// @relates BoundingBox
inline void Make_BBox(BoundingBox& BBox, const BBoxScalar llx, const BBoxScalar lly, const BBoxScalar llz, const BBoxScalar lex, const BBoxScalar ley, const BBoxScalar lez)
{
    BBox.lowerLeft = BBoxVector3d(llx, lly, llz);
    BBox.size      = BBoxVector3d(lex, ley, lez);
}

/// @relates BoundingBox
inline void Make_BBox_from_min_max(BoundingBox& BBox, const BBoxVector3d& mins, const BBoxVector3d& maxs)
{
    BBox.lowerLeft = mins;
    BBox.size      = maxs - mins;
}

/// @relates BoundingBox
inline void Make_BBox_from_min_max(BoundingBox& BBox, const Vector3d& mins, const Vector3d& maxs)
{
    BBox.lowerLeft = BBoxVector3d(mins);
    BBox.size      = BBoxVector3d(maxs - mins);
}

/// @relates BoundingBox
inline void Make_min_max_from_BBox(BBoxVector3d& mins, BBoxVector3d& maxs, const BoundingBox& BBox)
{
    mins = BBox.lowerLeft;
    maxs = mins + BBox.size;
}

/// @relates BoundingBox
inline void Make_min_max_from_BBox(Vector3d& mins, Vector3d& maxs, const BoundingBox& BBox)
{
    mins = Vector3d(BBox.lowerLeft);
    maxs = mins + Vector3d(BBox.size);
}

/// @relates BoundingBox
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

/// Structure holding bounding box data in min/max format.
///
struct MinMaxBoundingBox
{
    BBoxVector3d pmin;
    BBoxVector3d pmax;
};

/// @}
///
//******************************************************************************
///
/// @name Transformation Stuff
/// @{

#if 0
#pragma mark * Transform
#endif

typedef struct Transform_Struct TRANSFORM;

struct Transform_Struct
{
    MATRIX matrix;
    MATRIX inverse;
};

/// @}
///
//******************************************************************************
///
/// @name Colour Map Stuff
/// @{

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

/// @}
///
//******************************************************************************
///
/// @name Media and Interior Stuff
/// @{

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

/// @}
///
//******************************************************************************
///
/// @name Spline Stuff
/// @{

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

/// @}
///
//******************************************************************************
///
/// @name Image Stuff
/// @{

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

/// @}
///
//******************************************************************************
///
/// @name Pigment, Tnormal, Finish, Texture and Warps Stuff
/// @{

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
   TEXTURE *Interior_Texture;
   Interior *interior;
};

/// @}
///
//******************************************************************************
///
/// @name Object Stuff
///
/// @{

#if 0
#pragma mark * Object
#endif

#ifndef DUMP_OBJECT_DATA
#define DUMP_OBJECT_DATA 0
#endif

// TODO FIXME
class SceneThreadData;
typedef SceneThreadData TraceThreadData;

class LightSource;
// These fields are common to all objects.
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

typedef unsigned short HF_VAL;

/// @}
///
//******************************************************************************
///
/// @name Intersection Stack Stuff
///
/// @{

#if 0
#pragma mark * Intersection
#endif

/// Ray-object intersection data.
///
/// This class holds various information on a ray-object intersection.
///
class Intersection
{
    public:

        /// Distance from the intersecting ray's origin.
        DBL Depth;
        /// Point of the intersection in global coordinate space.
        Vector3d IPoint;
        /// Unpertubed surface normal at the intersection point.
        /// @attention This is not necessarily the true geometric surface normal, as it may include fake smoothing.
        /// @note This value is invalid if haveNormal is false.
        /// @todo We should have two distinct vectors: A true geometric one, and a separate one for faked smoothing.
        Vector3d INormal;
        /// Perturbed normal vector (set during texture evaluation).
        Vector3d PNormal;
        /// UV texture coordinate.
        Vector2d Iuv;
        /// Intersected object.
        ObjectPtr Object;

        /// @name Object-Specific Auxiliary Data
        /// These members hold information specific to particular object types, typically generated during
        /// intersection testing, to be re-used later for normal and/or UV coordinate computation.
        /// @note These values may be invalid or meaningless depending on the type of object intersected.
        /// @{

        /// Point of the intersection in local coordinate space (used by Blob)
        /// @note This value is invalid if haveLocalIPoint is false.
        Vector3d LocalIPoint;
        /// Flag to indicate whether INormal was computed during intersection testing (used by HField)
        /// @note Objects either always or never computing INormal during intersection testing don't use this flag.
        bool haveNormal;
        /// Flag to indicate whether LocalIPoint has already been computed.
        bool haveLocalIPoint;
        /// Generic auxiliary integer data #1 (used by Sor, Prism, Isosurface, Lathe, Cones, Boxes)
        int i1;
        /// Generic auxiliary integer data #2 (used by Sor, Prism, Isosurface)
        int i2;
        /// Generic auxiliary float data #1 (used by Prism, Lathe)
        DBL d1;
        /// Generic auxiliary pointer data (used by Mesh)
        const void *Pointer;

        /// @}

        /// Root-level parent CSG object for cutaway textures.
        ObjectPtr Csg;

        Intersection() :
            Depth(BOUND_HUGE), Object(NULL), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o) :
            Depth(d), Object(o), IPoint(v), Iuv(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector3d& n, ObjectPtr o) :
            Depth(d), Object(o), IPoint(v), Iuv(v), INormal(n), haveNormal(true), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector2d& uv, ObjectPtr o) :
            Depth(d), Object(o), IPoint(v), Iuv(uv), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector3d& n, const Vector2d& uv, ObjectPtr o) :
            Depth(d), Object(o), IPoint(v), INormal(n), Iuv(uv), haveNormal(true), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, const void *a) :
            Depth(d), Object(o), Pointer(a), IPoint(v), Iuv(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, const Vector2d& uv, ObjectPtr o, const void *a) :
            Depth(d), Object(o), Pointer(a), IPoint(v), Iuv(uv), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a) :
            Depth(d), Object(o), i1(a), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, DBL a) :
            Depth(d), Object(o), d1(a), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a, int b) :
            Depth(d), Object(o), i1(a), i2(b), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a, DBL b) :
            Depth(d), Object(o), i1(a), d1(b), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        Intersection(DBL d, const Vector3d& v, ObjectPtr o, int a, int b, DBL c) :
            Depth(d), Object(o), i1(a), i2(b), d1(c), IPoint(v), haveNormal(false), haveLocalIPoint(false), Csg(NULL)
        {}

        ~Intersection() { }
};

typedef std::stack<Intersection, vector<Intersection> > IStackData;
typedef RefPool<IStackData> IStackPool;
typedef Ref<IStackData> IStack;

/// @}
///
//******************************************************************************
///
/// @name Ray Stuff
///
/// @{

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
    virtual bool operator()(const Ray& ray, ConstObjectPtr object, DBL data) const = 0;
};

struct TrueRayObjectCondition : public RayObjectCondition
{
    virtual bool operator()(const Ray&, ConstObjectPtr, DBL) const { return true; }
};

struct PointObjectCondition
{
    virtual bool operator()(const Vector3d& point, ConstObjectPtr object) const = 0;
};

struct TruePointObjectCondition : public PointObjectCondition
{
    virtual bool operator()(const Vector3d&, ConstObjectPtr) const { return true; }
};

/// @}
///
//******************************************************************************
///
/// @name Frame tracking information
///
/// @{

struct FrameSettings
{
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

/// @}
///
//******************************************************************************

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
