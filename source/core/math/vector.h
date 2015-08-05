//******************************************************************************
///
/// @file backend/math/vector.h
///
/// This module contains macros to perform operations on vectors.
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
//*******************************************************************************

#ifndef VECTOR_H
#define VECTOR_H

namespace pov
{

typedef DBL VECTOR_4D[4]; ///< @todo       Make this obsolete.

/*****************************************************************************
* Inline functions
******************************************************************************/

inline VECTOR_4D *Create_Vector_4D ()
{
    VECTOR_4D *New;

    New = reinterpret_cast<VECTOR_4D *>(POV_MALLOC(sizeof (VECTOR_4D), "4d vector"));

    (*New)[0]= 0.0;
    (*New)[1]= 0.0;
    (*New)[2]= 0.0;
    (*New)[3]= 0.0;

    return (New);
}

inline void Assign_Vector_4D(VECTOR_4D d, const VECTOR_4D s)
{
    d[X] = s[X];
    d[Y] = s[Y];
    d[Z] = s[Z];
    d[T] = s[T];
}

inline void Destroy_Vector_4D(VECTOR_4D *x)
{
    if(x != NULL)
        POV_FREE(x);
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
