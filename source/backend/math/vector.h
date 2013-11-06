/*******************************************************************************
 * vector.h
 *
 * This module contains macros to perform operations on vectors.
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
 * $File: //depot/public/povray/3.x/source/backend/math/vector.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include "backend/frame.h"

namespace pov
{

/*****************************************************************************
* Inline functions
******************************************************************************/

// Vector Add
inline void VAdd(VECTOR a, const VECTOR b, const VECTOR c)
{
	a[X] = b[X] + c[X];
	a[Y] = b[Y] + c[Y];
	a[Z] = b[Z] + c[Z];
}

inline void VAdd(SNGL_VECT a, const VECTOR b, const VECTOR c)
{
	a[X] = b[X] + c[X];
	a[Y] = b[Y] + c[Y];
	a[Z] = b[Z] + c[Z];
}

inline void VAdd(SNGL_VECT a, const SNGL_VECT b, const SNGL_VECT c)
{
	a[X] = b[X] + c[X];
	a[Y] = b[Y] + c[Y];
	a[Z] = b[Z] + c[Z];
}

inline void VAddEq(VECTOR a, const VECTOR b)
{
	a[X] += b[X];
	a[Y] += b[Y];
	a[Z] += b[Z];
}

inline void VAddEq(SNGL_VECT a, const VECTOR b)
{
	a[X] += b[X];
	a[Y] += b[Y];
	a[Z] += b[Z];
}

inline void VAddEq(SNGL_VECT a, const SNGL_VECT b)
{
	a[X] += b[X];
	a[Y] += b[Y];
	a[Z] += b[Z];
}

// Vector Subtract
inline void VSub(VECTOR a, const VECTOR b, const VECTOR c)
{
	a[X] = b[X] - c[X];
	a[Y] = b[Y] - c[Y];
	a[Z] = b[Z] - c[Z];
}

inline void VSub(SNGL_VECT a, const VECTOR b, const VECTOR c)
{
	a[X] = b[X] - c[X];
	a[Y] = b[Y] - c[Y];
	a[Z] = b[Z] - c[Z];
}

inline void VSub(VECTOR a, const SNGL_VECT b, const VECTOR c)
{
	a[X] = b[X] - c[X];
	a[Y] = b[Y] - c[Y];
	a[Z] = b[Z] - c[Z];
}

inline void VSub(VECTOR a, const VECTOR b, const SNGL_VECT c)
{
	a[X] = b[X] - c[X];
	a[Y] = b[Y] - c[Y];
	a[Z] = b[Z] - c[Z];
}

inline void VSub(VECTOR a, const SNGL_VECT b, const SNGL_VECT c)
{
	a[X] = b[X] - c[X];
	a[Y] = b[Y] - c[Y];
	a[Z] = b[Z] - c[Z];
}

inline void VSub(SNGL_VECT a, const SNGL_VECT b, const SNGL_VECT c)
{
	a[X] = b[X] - c[X];
	a[Y] = b[Y] - c[Y];
	a[Z] = b[Z] - c[Z];
}

inline void VSubEq(VECTOR a, const VECTOR b)
{
	a[X] -= b[X];
	a[Y] -= b[Y];
	a[Z] -= b[Z];
}

inline void VSubEq(SNGL_VECT a, const VECTOR b)
{
	a[X] -= b[X];
	a[Y] -= b[Y];
	a[Z] -= b[Z];
}

inline void VSubEq(SNGL_VECT a, const SNGL_VECT b)
{
	a[X] -= b[X];
	a[Y] -= b[Y];
	a[Z] -= b[Z];
}

// Scale - Multiply Vector by a Scalar
inline void VScale(VECTOR a, const VECTOR b, DBL k)
{
	a[X] = b[X] * k;
	a[Y] = b[Y] * k;
	a[Z] = b[Z] * k;
}

// Scale - Multiply Vector by a Scalar
inline void VScale(SNGL_VECT a, const VECTOR b, DBL k)
{
	a[X] = b[X] * k;
	a[Y] = b[Y] * k;
	a[Z] = b[Z] * k;
}

inline void VScale(SNGL_VECT a, const SNGL_VECT b, SNGL k)
{
	a[X] = b[X] * k;
	a[Y] = b[Y] * k;
	a[Z] = b[Z] * k;
}

inline void VScaleEq(VECTOR a, DBL k)
{
	a[X] *= k;
	a[Y] *= k;
	a[Z] *= k;
}

inline void VScaleEq(SNGL_VECT a, SNGL k)
{
	a[X] *= k;
	a[Y] *= k;
	a[Z] *= k;
}

// Inverse Scale - Divide Vector by a Scalar
inline void VInverseScale(VECTOR a, const VECTOR b, DBL k)
{
	DBL tmp = 1.0 / k;
	a[X] = b[X] * tmp;
	a[Y] = b[Y] * tmp;
	a[Z] = b[Z] * tmp;
}

inline void VInverseScale(SNGL_VECT a, const SNGL_VECT b, SNGL k)
{
	SNGL tmp = 1.0 / k;
	a[X] = b[X] * tmp;
	a[Y] = b[Y] * tmp;
	a[Z] = b[Z] * tmp;
}

inline void VInverseScaleEq(VECTOR a, DBL k)
{
	DBL tmp = 1.0 / k;
	a[X] *= tmp;
	a[Y] *= tmp;
	a[Z] *= tmp;
}

inline void VInverseScaleEq(SNGL_VECT a, SNGL k)
{
	SNGL tmp = 1.0 / k;
	a[X] *= tmp;
	a[Y] *= tmp;
	a[Z] *= tmp;
}

// Dot Product - Gives Scalar angle (a) between two vectors (b) and (c)
inline void VDot(DBL& a, const VECTOR b, const VECTOR c)
{
	a = b[X] * c[X] + b[Y] * c[Y] + b[Z] * c[Z];
}

inline void VDot(SNGL& a, const VECTOR b, const VECTOR c)
{
	a = b[X] * c[X] + b[Y] * c[Y] + b[Z] * c[Z];
}

inline void VDot(DBL& a, const VECTOR b, const SNGL_VECT c)
{
	a = b[X] * c[X] + b[Y] * c[Y] + b[Z] * c[Z];
}

inline void VDot(DBL& a, const SNGL_VECT b, const VECTOR c)
{
	a = b[X] * c[X] + b[Y] * c[Y] + b[Z] * c[Z];
}

inline void VDot(DBL& a, const SNGL_VECT b, const SNGL_VECT c)
{
	a = b[X] * c[X] + b[Y] * c[Y] + b[Z] * c[Z];
}

inline void VDot(SNGL& a, const SNGL_VECT b, const SNGL_VECT c)
{
	a = b[X] * c[X] + b[Y] * c[Y] + b[Z] * c[Z];
}

// Cross Product - returns Vector (a) = (b) x (c)
inline void VCross(VECTOR a, const VECTOR b, const VECTOR c)
{
	VECTOR tmp;

	tmp[X] = b[Y] * c[Z] - b[Z] * c[Y];
	tmp[Y] = b[Z] * c[X] - b[X] * c[Z];
	tmp[Z] = b[X] * c[Y] - b[Y] * c[X];

	Assign_Vector(a, tmp);
}

// Evaluate - returns Vector (a) = Multiply Vector (b) by Vector (c)
inline void VEvaluate(VECTOR a, const VECTOR b, const VECTOR c)
{
	a[X] = b[X] * c[X];
	a[Y] = b[Y] * c[Y];
	a[Z] = b[Z] * c[Z];
}

inline void VEvaluateEq(VECTOR a, const VECTOR b)
{
	a[X] *= b[X];
	a[Y] *= b[Y];
	a[Z] *= b[Z];
}

// Divide - returns Vector (a) = Divide Vector (a) by Vector (b)
inline void VDivEq(VECTOR a, const VECTOR b)
{
	a[X] /= b[X];
	a[Y] /= b[Y];
	a[Z] /= b[Z];
}

// Simple Scalar Square Macro
inline DBL Sqr(DBL a)
{
	return a * a;
}

inline SNGL Sqr(SNGL a)
{
	return a * a;
}

// Vector Length - returs Scalar Euclidean Length (a) of Vector (b)
inline void VLength(DBL& a, const VECTOR b)
{
	a = sqrt(b[X] * b[X] + b[Y] * b[Y] + b[Z] * b[Z]);
}

inline void VLength(SNGL& a, const SNGL_VECT b)
{
	a = sqrt(b[X] * b[X] + b[Y] * b[Y] + b[Z] * b[Z]);
}

// Vector Distance - returs Scalar Euclidean Distance (a) between two points/Vectors (b) and (c)
inline void VDist(DBL& a, const VECTOR b, const VECTOR c)
{
	VECTOR tmp;
	VSub(tmp, b, c);
	VLength(a, tmp);
}

// Normalize a Vector - returns a vector (length of 1) that points at (b)
inline void VNormalize(VECTOR a, const VECTOR b)
{
	DBL tmp;
	VLength(tmp, b);
	VInverseScale(a, b, tmp);
}

inline void VNormalize(SNGL_VECT a, const SNGL_VECT b)
{
	SNGL tmp;
	VLength(tmp, b);
	VInverseScale(a, b, tmp);
}

inline void VNormalizeEq(VECTOR a)
{
	DBL tmp;
	VLength(tmp, a);
	VInverseScaleEq(a, tmp);
}

// Compute a Vector (a) Halfway Between Two Given Vectors (b) and (c)
inline void VHalf(VECTOR a, const VECTOR b, const VECTOR c)
{
	a[X] = 0.5 * (b[X] + c[X]);
	a[Y] = 0.5 * (b[Y] + c[Y]);
	a[Z] = 0.5 * (b[Z] + c[Z]);
}

// Calculate the sum of the sqares of the components of a vector.  (the square of its length)
inline DBL VSumSqr(const VECTOR a)
{
	return a[X] * a[X] + a[Y] * a[Y] + a[Z] * a[Z];
}

// Linear combination of 2 vectors. [DB 7/94]
//   v = k1 * v1 + k2 * v2
inline void VLinComb2(VECTOR v, DBL k1, const VECTOR v1, DBL k2, const VECTOR v2)
{
	v[X] = k1 * v1[X] + k2 * v2[X];
	v[Y] = k1 * v1[Y] + k2 * v2[Y];
	v[Z] = k1 * v1[Z] + k2 * v2[Z];
}

// Linear combination of 3 vectors. [DB 7/94]
//   v = k1 * v1 + k2 * v2 + k3 * v3
inline void VLinComb3(VECTOR v, DBL k1, const VECTOR v1, DBL k2, const VECTOR v2, DBL k3, const VECTOR v3)
{
	v[X] = k1 * v1[X] + k2 * v2[X] + k3 * v3[X];
	v[Y] = k1 * v1[Y] + k2 * v2[Y] + k3 * v3[Y];
	v[Z] = k1 * v1[Z] + k2 * v2[Z] + k3 * v3[Z];
}

// Evaluate a ray equation. [DB 7/94]
//   IPoint = Origin + depth * Direction
inline void VEvaluateRay(VECTOR IPoint, const VECTOR Origin, DBL depth, const VECTOR Direction)
{
	IPoint[X] = Origin[X] + depth * Direction[X];
	IPoint[Y] = Origin[Y] + depth * Direction[Y];
	IPoint[Z] = Origin[Z] + depth * Direction[Z];
}

// Add a scaled vector. [DB 7/94]
//   v  = v1 + k * v2;
//   v += k * v2;
inline void VAddScaled(VECTOR v, const VECTOR v1, DBL k, const VECTOR v2)
{
	v[X] = v1[X] + k * v2[X];
	v[Y] = v1[Y] + k * v2[Y];
	v[Z] = v1[Z] + k * v2[Z];
}

inline void VAddScaledEq(VECTOR v, DBL k, const VECTOR v2)
{
	v[X] += k * v2[X];
	v[Y] += k * v2[Y];
	v[Z] += k * v2[Z];
}

// Inverse Scale - Divide Vector by a Scalar
inline void V4D_InverseScaleEq(VECTOR_4D a, DBL k)
{
	DBL tmp = 1.0 / k;
	a[X] *= tmp;
	a[Y] *= tmp;
	a[Z] *= tmp;
	a[T] *= tmp;
}

// Dot Product - Gives Scalar angle (a) between two vectors (b) and (c)
inline void V4D_Dot(DBL& a, const VECTOR_4D b, const VECTOR_4D c)
{
	a = b[X] * c[X] + b[Y] * c[Y] + b[Z] * c[Z] + b[T] * c[T];
}

}

#endif


