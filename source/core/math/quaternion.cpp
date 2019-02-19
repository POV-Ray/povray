//******************************************************************************
///
/// @file core/math/quaternion.cpp
///
/// Implementation of Quaternion algebra julia fractals.
///
/// @author Pascal Massimino (original code)
/// @author Tim Wegner (revisions and updates for POV-Ray v3.x)
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/math/quaternion.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/shape/fractal.h"
#include "core/shape/sphere.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define Deriv_z2(n1,n2,n3,n4)               \
{                                           \
    tmp = (n1)*x - (n2)*y - (n3)*z - (n4)*w;  \
    (n2) = (n1)*y + x*(n2);                   \
    (n3) = (n1)*z + x*(n3);                   \
    (n4) = (n1)*w + x*(n4);                   \
    (n1) = tmp;                               \
}

#define Deriv_z3(n1,n2,n3,n4)              \
{                                          \
    dtmp = 2.0*((n2)*y + (n3)*z + (n4)*w);   \
    dtmp2 = 6.0*x*(n1) - dtmp;               \
    (n1) = ( (n1)*x3 - x*dtmp )*3.0;         \
    (n2) = (n2)*x4 + y*dtmp2;                \
    (n3) = (n3)*x4 + z*dtmp2;                \
    (n4) = (n4)*x4 + w*dtmp2;                \
}


/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

bool Z3FractalRules::Iterate(const Vector3d& point, const Fractal *Julia, DBL **IterStack) const
{
    int i;
    DBL x, y, z, w;
    DBL d, x2, tmp;
    DBL Exit_Value;

    IterStack[X][0] = x = point[X];
    IterStack[Y][0] = y = point[Y];
    IterStack[Z][0] = z = point[Z];
    IterStack[W][0] = w = (Julia->SliceDist
                         - Julia->Slice[X]*x
                         - Julia->Slice[Y]*y
                         - Julia->Slice[Z]*z)/Julia->Slice[T];

    Exit_Value = Julia->Exit_Value;

    for (i = 1; i <= Julia->Num_Iterations; ++i)
    {
        d = y * y + z * z + w * w;

        x2 = x * x;

        if ((d + x2) > Exit_Value)
        {
            return (false);
        }

        tmp = 3.0 * x2 - d;

        IterStack[X][i] = x = x * (x2 - 3.0 * d) + Julia->Julia_Parm[X];
        IterStack[Y][i] = y = y * tmp + Julia->Julia_Parm[Y];
        IterStack[Z][i] = z = z * tmp + Julia->Julia_Parm[Z];
        IterStack[W][i] = w = w * tmp + Julia->Julia_Parm[T];
    }

    return (true);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

bool JuliaFractalRules::Iterate(const Vector3d& point, const Fractal *Julia, DBL **IterStack) const
{
    int i;
    DBL x, y, z, w;
    DBL d, x2;
    DBL Exit_Value;

    IterStack[X][0] = x = point[X];
    IterStack[Y][0] = y = point[Y];
    IterStack[Z][0] = z = point[Z];
    IterStack[W][0] = w = (Julia->SliceDist
                         - Julia->Slice[X]*x
                         - Julia->Slice[Y]*y
                         - Julia->Slice[Z]*z)/Julia->Slice[T];

    Exit_Value = Julia->Exit_Value;

    for (i = 1; i <= Julia->Num_Iterations; ++i)
    {
        d = y * y + z * z + w * w;

        x2 = x * x;

        if ((d + x2) > Exit_Value)
        {
            return (false);
        }

        x *= 2.0;

        IterStack[Y][i] = y = x * y + Julia->Julia_Parm[Y];
        IterStack[Z][i] = z = x * z + Julia->Julia_Parm[Z];
        IterStack[W][i] = w = x * w + Julia->Julia_Parm[T];
        IterStack[X][i] = x = x2 - d + Julia->Julia_Parm[X];;

    }

    return (true);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
* D_Iteration puts in *Dist a lower bound for the distance from *point to the
* set
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

/*----------- Distance estimator + iterations ------------*/

bool Z3FractalRules::Iterate(const Vector3d& point, const Fractal *Julia, const Vector3d&, DBL *Dist, DBL **IterStack) const
{
    int i, j;
    DBL Norm, d;
    DBL xx, yy, zz;
    DBL x, y, z, w;
    DBL tmp, x2;
    DBL Exit_Value;
    DBL Pow;

    x = IterStack[X][0] = point[X];
    y = IterStack[Y][0] = point[Y];
    z = IterStack[Z][0] = point[Z];
    w = IterStack[W][0] = (Julia->SliceDist
                         - Julia->Slice[X]*x
                         - Julia->Slice[Y]*y
                         - Julia->Slice[Z]*z)/Julia->Slice[T];

    Exit_Value = Julia->Exit_Value;

    for (i = 1; i <= Julia->Num_Iterations; i++)
    {
        d = y * y + z * z + w * w;

        x2 = x * x;

        if ((Norm = d + x2) > Exit_Value)
        {
            /* Distance estimator */

            x = IterStack[X][0];
            y = IterStack[Y][0];
            z = IterStack[Z][0];
            w = IterStack[W][0];

            Pow = 1.0 / 3.0;

            for (j = 1; j < i; ++j)
            {
                xx = x * IterStack[X][j] - y * IterStack[Y][j] - z * IterStack[Z][j] - w * IterStack[W][j];
                yy = x * IterStack[Y][j] + y * IterStack[X][j] - z * IterStack[W][j] + w * IterStack[Z][j];
                zz = x * IterStack[Z][j] + y * IterStack[W][j] + z * IterStack[X][j] - w * IterStack[Y][j];
                w  = x * IterStack[W][j] - y * IterStack[Z][j] + z * IterStack[Y][j] + w * IterStack[X][j];

                x = xx;
                y = yy;
                z = zz;

                Pow /= 3.0;
            }

            *Dist = Pow * sqrt(Norm / (x * x + y * y + z * z + w * w)) * log(Norm);

            return (false);
        }

        tmp = 3.0 * x2 - d;

        IterStack[X][i] = x = x * (x2 - 3.0 * d) + Julia->Julia_Parm[X];
        IterStack[Y][i] = y = y * tmp + Julia->Julia_Parm[Y];
        IterStack[Z][i] = z = z * tmp + Julia->Julia_Parm[Z];
        IterStack[W][i] = w = w * tmp + Julia->Julia_Parm[T];
    }

    *Dist = Julia->Precision;

    return (true);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

bool JuliaFractalRules::Iterate(const Vector3d& point, const Fractal *Julia, const Vector3d&, DBL *Dist, DBL **IterStack) const
{
    int i, j;
    DBL Norm, d;
    DBL Exit_Value;
    DBL x, y, z, w;
    DBL xx, yy, zz, x2;
    DBL Pow;

    x = IterStack[X][0] = point[X];
    y = IterStack[Y][0] = point[Y];
    z = IterStack[Z][0] = point[Z];
    w = IterStack[W][0] = (Julia->SliceDist
                         - Julia->Slice[X]*x
                         - Julia->Slice[Y]*y
                         - Julia->Slice[Z]*z)/Julia->Slice[T];

    Exit_Value = Julia->Exit_Value;

    for (i = 1; i <= Julia->Num_Iterations; i++)
    {
        d = y * y + z * z + w * w;

        x2 = x * x;

        if ((Norm = d + x2) > Exit_Value)
        {
            /* Distance estimator */

            x = IterStack[X][0];
            y = IterStack[Y][0];
            z = IterStack[Z][0];
            w = IterStack[W][0];

            Pow = 1.0 / 2.0;

            for (j = 1; j < i; ++j)
            {
                xx = x * IterStack[X][j] - y * IterStack[Y][j] - z * IterStack[Z][j] - w * IterStack[W][j];
                yy = x * IterStack[Y][j] + y * IterStack[X][j] + w * IterStack[Z][j] - z * IterStack[W][j];
                zz = x * IterStack[Z][j] + z * IterStack[X][j] + y * IterStack[W][j] - w * IterStack[Y][j];
                w  = x * IterStack[W][j] + w * IterStack[X][j] + z * IterStack[Y][j] - y * IterStack[Z][j];

                x = xx;
                y = yy;
                z = zz;

                Pow /= 2.0;
            }

            *Dist = Pow * sqrt(Norm / (x * x + y * y + z * z + w * w)) * log(Norm);

            return (false);
        }

        x *= 2.0;

        IterStack[Y][i] = y = x * y + Julia->Julia_Parm[Y];
        IterStack[Z][i] = z = x * z + Julia->Julia_Parm[Z];
        IterStack[W][i] = w = x * w + Julia->Julia_Parm[T];
        IterStack[X][i] = x = x2 - d + Julia->Julia_Parm[X];

    }

    *Dist = Julia->Precision;

    return (true);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
* Provided the iterations sequence has been built, perform the computation of
* the Normal
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Z3FractalRules::CalcNormal(Vector3d& Result, int N_Max, const Fractal *, DBL **IterStack) const
{
    DBL
    n11 = 1.0, n12 = 0.0, n13 = 0.0, n14 = 0.0,
    n21 = 0.0, n22 = 1.0, n23 = 0.0, n24 = 0.0,
    n31 = 0.0, n32 = 0.0, n33 = 1.0, n34 = 0.0;

    DBL x, y, z, w;
    int i;
    DBL tmp, dtmp, dtmp2, x2, x3, x4;

    x = IterStack[X][0];
    y = IterStack[Y][0];
    z = IterStack[Z][0];
    w = IterStack[W][0];

    for (i = 1; i <= N_Max; i++)
    {
        tmp = y * y + z * z + w * w;

        x2 = x * x;
        x3 = x2 - tmp;
        x4 = 3.0 * x2 - tmp;

        Deriv_z3(n11, n12, n13, n14);
        Deriv_z3(n21, n22, n23, n24);
        Deriv_z3(n31, n32, n33, n34);

        x = IterStack[X][i];
        y = IterStack[Y][i];
        z = IterStack[Z][i];
        w = IterStack[W][i];
    }

    Result[X] = n11 * x + n12 * y + n13 * z + n14 * w;
    Result[Y] = n21 * x + n22 * y + n23 * z + n24 * w;
    Result[Z] = n31 * x + n32 * y + n33 * z + n34 * w;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void JuliaFractalRules::CalcNormal(Vector3d& Result, int N_Max, const Fractal *, DBL **IterStack) const
{
    DBL
    n11 = 1.0, n12 = 0.0, n13 = 0.0, n14 = 0.0,
    n21 = 0.0, n22 = 1.0, n23 = 0.0, n24 = 0.0,
    n31 = 0.0, n32 = 0.0, n33 = 1.0, n34 = 0.0;
    DBL tmp;
    DBL x, y, z, w;
    int i;

    x = IterStack[X][0];
    y = IterStack[Y][0];
    z = IterStack[Z][0];
    w = IterStack[W][0];

    for (i = 1; i <= N_Max; i++)
    {
        Deriv_z2(n11, n12, n13, n14);
        Deriv_z2(n21, n22, n23, n24);
        Deriv_z2(n31, n32, n33, n34);

        x = IterStack[X][i];
        y = IterStack[Y][i];
        z = IterStack[Z][i];
        w = IterStack[W][i];
    }

    Result[X] = n11 * x + n12 * y + n13 * z + n14 * w;
    Result[Y] = n21 * x + n22 * y + n23 * z + n24 * w;
    Result[Z] = n31 * x + n32 * y + n33 * z + n34 * w;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

bool QuaternionFractalRules::Bound(const BasicRay& ray, const Fractal *fractal, DBL *Depth_Min, DBL *Depth_Max) const
{
    return (Sphere::Intersect(ray, fractal->Center, fractal->Radius_Squared, Depth_Min, Depth_Max));
}

}
// end of namespace pov
