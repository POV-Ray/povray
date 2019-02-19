//******************************************************************************
///
/// @file core/math/vector.h
///
/// Declarations related to vector arithmetics.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_CORE_VECTOR_H
#define POVRAY_CORE_VECTOR_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"
#include "core/math/vector_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
// POV-Ray header files (core module)
//  (none at the moment)

namespace pov
{

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

//******************************************************************************

typedef DBL VECTOR_4D[4]; ///< @todo       Make this obsolete.

VECTOR_4D *Create_Vector_4D ();

void Assign_Vector_4D(VECTOR_4D d, const VECTOR_4D s);

void Destroy_Vector_4D(VECTOR_4D *x);

// Inverse Scale - Divide Vector by a Scalar
void V4D_InverseScaleEq(VECTOR_4D a, DBL k);

// Dot Product - Gives Scalar angle (a) between two vectors (b) and (c)
void V4D_Dot(DBL& a, const VECTOR_4D b, const VECTOR_4D c);

//******************************************************************************

template<typename T>
class GenericVector3d;

/// Generic template class to hold a 2D vector.
///
/// @tparam T   Floating-point type to use for the individual vector components.
///
template<typename T>
class GenericVector2d final
{
    public:

        typedef T UV_VECT_T[2];

        inline GenericVector2d()
        {
            vect[X] = 0.0;
            vect[Y] = 0.0;
        }

        inline explicit GenericVector2d(T d)
        {
            vect[X] = d;
            vect[Y] = d;
        }

        inline GenericVector2d(T x, T y)
        {
            vect[X] = x;
            vect[Y] = y;
        }

        inline explicit GenericVector2d(const DBL* vi)
        {
            vect[X] = T(vi[X]);
            vect[Y] = T(vi[Y]);
        }

        inline explicit GenericVector2d(const GenericVector3d<T>& b)
        {
            vect[X] = b[X];
            vect[Y] = b[Y];
        }

        template<typename T2>
        inline explicit GenericVector2d(const GenericVector2d<T2>& b)
        {
            vect[X] = T(b[X]);
            vect[Y] = T(b[Y]);
        }

        inline GenericVector2d(const GenericVector2d& b)
        {
            vect[X] = b[X];
            vect[Y] = b[Y];
        }

        inline GenericVector2d& operator=(const GenericVector2d& b)
        {
            vect[X] = b[X];
            vect[Y] = b[Y];
            return *this;
        }

        inline T operator[](int idx) const { return vect[idx]; }
        inline T& operator[](int idx) { return vect[idx]; }

        inline GenericVector2d operator+(const GenericVector2d& b) const
        {
            return GenericVector2d(vect[X] + b[X], vect[Y] + b[Y]);
        }

        inline GenericVector2d operator-(const GenericVector2d& b) const
        {
            return GenericVector2d(vect[X] - b[X], vect[Y] - b[Y]);
        }

        inline GenericVector2d operator*(const GenericVector2d& b) const
        {
            return GenericVector2d(vect[X] * b[X], vect[Y] * b[Y]);
        }

        inline GenericVector2d operator/(const GenericVector2d& b) const
        {
            return GenericVector2d(vect[X] / b[X], vect[Y] / b[Y]);
        }

        inline GenericVector2d& operator+=(const GenericVector2d& b)
        {
            vect[X] += b[X];
            vect[Y] += b[Y];
            return *this;
        }

        inline GenericVector2d& operator-=(const GenericVector2d& b)
        {
            vect[X] -= b[X];
            vect[Y] -= b[Y];
            return *this;
        }

        inline GenericVector2d& operator*=(const GenericVector2d& b)
        {
            vect[X] *= b[X];
            vect[Y] *= b[Y];
            return *this;
        }

        inline GenericVector2d& operator/=(const GenericVector2d& b)
        {
            vect[X] /= b[X];
            vect[Y] /= b[Y];
            return *this;
        }

        inline GenericVector2d operator-() const
        {
            return GenericVector2d(-vect[X], -vect[Y]);
        }

        inline GenericVector2d operator+(T b) const
        {
            return GenericVector2d(vect[X] + b, vect[Y] + b);
        }

        inline GenericVector2d operator-(T b) const
        {
            return GenericVector2d(vect[X] - b, vect[Y] - b);
        }

        inline GenericVector2d operator*(T b) const
        {
            return GenericVector2d(vect[X] * b, vect[Y] * b);
        }

        inline GenericVector2d operator/(T b) const
        {
            return GenericVector2d(vect[X] / b, vect[Y] / b);
        }

        inline GenericVector2d& operator+=(T b)
        {
            vect[X] += b;
            vect[Y] += b;
            return *this;
        }

        inline GenericVector2d& operator-=(T b)
        {
            vect[X] -= b;
            vect[Y] -= b;
            return *this;
        }

        inline GenericVector2d& operator*=(T b)
        {
            vect[X] *= b;
            vect[Y] *= b;
            return *this;
        }

        inline GenericVector2d& operator/=(T b)
        {
            vect[X] /= b;
            vect[Y] /= b;
            return *this;
        }

        inline const UV_VECT_T& operator*() const { return vect; }
        inline UV_VECT_T& operator*() { return vect; }

        inline T x() const { return vect[X]; }
        inline T& x() { return vect[X]; }

        inline T y() const { return vect[Y]; }
        inline T& y() { return vect[Y]; }

        inline T u() const { return vect[X]; }
        inline T& u() { return vect[X]; }

        inline T v() const { return vect[Y]; }
        inline T& v() { return vect[Y]; }

        inline T length() const
        {
            return sqrt(vect[X] * vect[X] + vect[Y] * vect[Y]);
        }
        inline T lengthSqr() const
        {
            return vect[X] * vect[X] + vect[Y] * vect[Y];
        }
        inline bool IsNull() const
        {
            return (vect[X] == 0.0) &&
                   (vect[Y] == 0.0);
        }
        inline bool IsNearNull(T epsilon) const
        {
            return (fabs(vect[X]) < epsilon) &&
                   (fabs(vect[Y]) < epsilon);
        }
        inline GenericVector2d normalized() const
        {
            T l = length();
            if (l != 0)
                return *this / l;
            else
                return *this;
        }
        inline void normalize()
        {
            T l = length();
            if (l != 0)
                *this /= l;
            // no else
        }

    private:

        UV_VECT_T vect;
};

/// Generic template class to hold a 3D vector.
///
/// @tparam T   Floating-point type to use for the individual vector components.
///
template<typename T>
class GenericVector3d final
{
    public:

        typedef T VECTOR_T[3];

        inline GenericVector3d()
        {
            vect[X] = 0.0;
            vect[Y] = 0.0;
            vect[Z] = 0.0;
        }

        inline explicit GenericVector3d(T d)
        {
            vect[X] = d;
            vect[Y] = d;
            vect[Z] = d;
        }

        inline GenericVector3d(T x, T y, T z)
        {
            vect[X] = x;
            vect[Y] = y;
            vect[Z] = z;
        }

        inline explicit GenericVector3d(const DBL* vi)
        {
            vect[X] = T(vi[X]);
            vect[Y] = T(vi[Y]);
            vect[Z] = T(vi[Z]);
        }

        template<typename T2>
        inline explicit GenericVector3d(const GenericVector3d<T2>& b)
        {
            vect[X] = T(b[X]);
            vect[Y] = T(b[Y]);
            vect[Z] = T(b[Z]);
        }

        inline GenericVector3d(const GenericVector3d& b)
        {
            vect[X] = b[X];
            vect[Y] = b[Y];
            vect[Z] = b[Z];
        }

        inline GenericVector3d& operator=(const GenericVector3d& b)
        {
            vect[X] = b[X];
            vect[Y] = b[Y];
            vect[Z] = b[Z];
            return *this;
        }

        inline T operator[](int idx) const { return vect[idx]; }
        inline T& operator[](int idx) { return vect[idx]; }

        inline GenericVector3d operator+(const GenericVector3d& b) const
        {
            return GenericVector3d(vect[X] + b[X], vect[Y] + b[Y], vect[Z] + b[Z]);
        }

        inline GenericVector3d operator-(const GenericVector3d& b) const
        {
            return GenericVector3d(vect[X] - b[X], vect[Y] - b[Y], vect[Z] - b[Z]);
        }

        inline GenericVector3d operator*(const GenericVector3d& b) const
        {
            return GenericVector3d(vect[X] * b[X], vect[Y] * b[Y], vect[Z] * b[Z]);
        }

        inline GenericVector3d operator/(const GenericVector3d& b) const
        {
            return GenericVector3d(vect[X] / b[X], vect[Y] / b[Y], vect[Z] / b[Z]);
        }

        inline GenericVector3d& operator+=(const GenericVector3d& b)
        {
            vect[X] += b[X];
            vect[Y] += b[Y];
            vect[Z] += b[Z];
            return *this;
        }

        inline GenericVector3d& operator-=(const GenericVector3d& b)
        {
            vect[X] -= b[X];
            vect[Y] -= b[Y];
            vect[Z] -= b[Z];
            return *this;
        }

        inline GenericVector3d& operator*=(const GenericVector3d& b)
        {
            vect[X] *= b[X];
            vect[Y] *= b[Y];
            vect[Z] *= b[Z];
            return *this;
        }

        inline GenericVector3d& operator/=(const GenericVector3d& b)
        {
            vect[X] /= b[X];
            vect[Y] /= b[Y];
            vect[Z] /= b[Z];
            return *this;
        }

        inline GenericVector3d operator-() const
        {
            return GenericVector3d(-vect[X], -vect[Y], -vect[Z]);
        }

        inline GenericVector3d operator+(T b) const
        {
            return GenericVector3d(vect[X] + b, vect[Y] + b, vect[Z] + b);
        }

        inline GenericVector3d operator-(T b) const
        {
            return GenericVector3d(vect[X] - b, vect[Y] - b, vect[Z] - b);
        }

        inline GenericVector3d operator*(T b) const
        {
            return GenericVector3d(vect[X] * b, vect[Y] * b, vect[Z] * b);
        }

        inline GenericVector3d operator/(T b) const
        {
            return GenericVector3d(vect[X] / b, vect[Y] / b, vect[Z] / b);
        }

        inline GenericVector3d& operator+=(T b)
        {
            vect[X] += b;
            vect[Y] += b;
            vect[Z] += b;
            return *this;
        }

        inline GenericVector3d& operator-=(T b)
        {
            vect[X] -= b;
            vect[Y] -= b;
            vect[Z] -= b;
            return *this;
        }

        inline GenericVector3d& operator*=(T b)
        {
            vect[X] *= b;
            vect[Y] *= b;
            vect[Z] *= b;
            return *this;
        }

        inline GenericVector3d& operator/=(T b)
        {
            vect[X] /= b;
            vect[Y] /= b;
            vect[Z] /= b;
            return *this;
        }

        inline const VECTOR_T& operator*() const { return vect; }
        inline VECTOR_T& operator*() { return vect; }

        inline T x() const { return vect[X]; }
        inline T& x() { return vect[X]; }

        inline T y() const { return vect[Y]; }
        inline T& y() { return vect[Y]; }

        inline T z() const { return vect[Z]; }
        inline T& z() { return vect[Z]; }

        inline T length() const
        {
            return sqrt(vect[X] * vect[X] + vect[Y] * vect[Y] + vect[Z] * vect[Z]);
        }
        inline T lengthSqr() const
        {
            return vect[X] * vect[X] + vect[Y] * vect[Y] + vect[Z] * vect[Z];
        }
        inline bool IsNull() const
        {
            return (vect[X] == 0.0) &&
                   (vect[Y] == 0.0) &&
                   (vect[Z] == 0.0);
        }
        inline bool IsNearNull(T epsilon) const
        {
            return (fabs(vect[X]) < epsilon) &&
                   (fabs(vect[Y]) < epsilon) &&
                   (fabs(vect[Z]) < epsilon);
        }
        inline GenericVector3d normalized() const
        {
            T l = length();
            if (l != 0)
                return *this / l;
            else
                return *this;
        }
        inline void normalize()
        {
            T l = length();
            if (l != 0)
                *this /= l;
            // no else
        }
        inline void invert()
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
typedef GenericVector3d<int> IntVector3d;   ///< Integer 3D vector.

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

}
// end of namespace pov

#endif // POVRAY_CORE_VECTOR_H
