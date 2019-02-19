//******************************************************************************
///
/// @file core/math/hypercomplex.cpp
///
/// Implementation of hypercomplex Julia fractals.
///
/// This file was written by Pascal Massimino.
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
#include "core/math/hypercomplex.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/shape/fractal.h" // TODO - Where should hcmplx.h/hcmplx.cpp go? Are they really math? [trf]
#include "core/shape/sphere.h" // TODO - Move sphere intersection function to math code! [trf]

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

const DBL Fractal_Tolerance = 1e-8;

#define HMult(xr,yr,zr,wr,x1,y1,z1,w1,x2,y2,z2,w2)        \
    (xr) = (x1) * (x2) - (y1) * (y2) - (z1) * (z2) + (w1) * (w2);   \
    (yr) = (y1) * (x2) + (x1) * (y2) - (w1) * (z2) - (z1) * (w2);   \
    (zr) = (z1) * (x2) - (w1) * (y2) + (x1) * (z2) - (y1) * (w2);   \
    (wr) = (w1) * (x2) + (z1) * (y2) + (y1) * (z2) + (x1) * (w2);

#define HSqr(xr,yr,zr,wr,x,y,z,w)         \
    (xr) = (x) * (x) - (y) * (y) - (z) * (z) + (w) * (w) ;  \
    (yr) = 2.0 * ( (x) * (y) - (z) * (w) );     \
    (zr) = 2.0 * ( (z) * (x) - (w) * (y) );       \
    (wr) = 2.0 * ( (w) * (x) + (z) * (y) );



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int HReciprocal (DBL * xr, DBL * yr, DBL * zr, DBL * wr, DBL x, DBL y, DBL z, DBL w);
static void HFunc (DBL * xr, DBL * yr, DBL * zr, DBL * wr, DBL x, DBL y, DBL z, DBL w, const Fractal *f);

/******** Computations with Hypercomplexes **********/

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
* CHANGES
*
******************************************************************************/

static int HReciprocal(DBL *xr, DBL  *yr, DBL  *zr, DBL  *wr, DBL  x, DBL  y, DBL  z, DBL  w)
{
    DBL det, mod, xt_minus_yz;

    det = ((x - w) * (x - w) + (y + z) * (y + z)) * ((x + w) * (x + w) + (y - z) * (y - z));

    if (det == 0.0)
    {
        return (-1);
    }

    mod = (x * x + y * y + z * z + w * w);

    xt_minus_yz = x * w - y * z;

    *xr = (x * mod - 2 * w * xt_minus_yz) / det;
    *yr = (-y * mod - 2 * z * xt_minus_yz) / det;
    *zr = (-z * mod - 2 * y * xt_minus_yz) / det;
    *wr = (w * mod - 2 * x * xt_minus_yz) / det;

    return (0);
}



/*****************************************************************************
*
* FUNCTION Hfunc
*
* INPUT 4D Hypercomplex number, pointer to fractal object
*
* OUTPUT  calculates the 4D generalization of fractal->Complex_Function
*
* RETURNS void
*
* AUTHOR
*
*   Pascal Massimino
*
* DESCRIPTION
*   Hypercomplex numbers allow generalization of any complex->complex
*   function in a uniform way. This function implements a general
*   unary 4D function based on the corresponding complex function.
*
* CHANGES
*  Generalized to use Complex_Function()   TW
*
******************************************************************************/

void HypercomplexFunctionFractalRules::HFunc(DBL *xr, DBL  *yr, DBL  *zr, DBL  *wr, DBL  x, DBL  y, DBL  z, DBL  w, const Fractal *f) const
{
    Complex a, b, ra, rb;

    /* convert to duplex form */
    a.x = x - w;
    a.y = y + z;
    b.x = x + w;
    b.y = y - z;

    /* apply function to each part */
    ComplexFunction(&ra, &a, &(f->exponent));
    ComplexFunction(&rb, &b, &(f->exponent));

    /* convert back */
    *xr = .5 * (ra.x + rb.x);
    *yr = .5 * (ra.y + rb.y);
    *zr = .5 * (ra.y - rb.y);
    *wr = .5 * (rb.x - ra.x);
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
* CHANGES
*
******************************************************************************/

/*------------------ z2 Iteration method ------------------*/

bool HypercomplexFractalRules::Iterate(const Vector3d& IPoint, const Fractal *HCompl, DBL **IterStack) const
{
    int i;
    DBL yz, xw;
    DBL Exit_Value;
    DBL x, y, z, w;

    x = IterStack[X][0] = IPoint[X];
    y = IterStack[Y][0] = IPoint[Y];
    z = IterStack[Z][0] = IPoint[Z];
    w = IterStack[W][0] = (HCompl->SliceDist
                         - HCompl->Slice[X]*x
                         - HCompl->Slice[Y]*y
                         - HCompl->Slice[Z]*z)/HCompl->Slice[T];

    Exit_Value = HCompl->Exit_Value;

    for (i = 1; i <= HCompl->Num_Iterations; ++i)
    {
        yz = y * y + z * z;
        xw = x * x + w * w;

        if ((xw + yz) > Exit_Value)
        {
            return (false);
        }

        IterStack[X][i] = xw - yz + HCompl->Julia_Parm[X];
        IterStack[Y][i] = 2.0 * (x * y - z * w) + HCompl->Julia_Parm[Y];
        IterStack[Z][i] = 2.0 * (x * z - w * y) + HCompl->Julia_Parm[Z];
        IterStack[W][i] = 2.0 * (x * w + y * z) + HCompl->Julia_Parm[T];

        w = IterStack[W][i];
        x = IterStack[X][i];

        z = IterStack[Z][i];
        y = IterStack[Y][i];
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
* CHANGES
*
******************************************************************************/

bool HypercomplexFractalRules::Iterate(const Vector3d& IPoint, const Fractal *HCompl, const Vector3d& Direction, DBL *Dist, DBL **IterStack) const
{
    int i;
    DBL yz, xw;
    DBL Exit_Value, F_Value, Step;
    DBL x, y, z, w;
    Vector3d H_Normal;

    x = IterStack[X][0] = IPoint[X];
    y = IterStack[Y][0] = IPoint[Y];
    z = IterStack[Z][0] = IPoint[Z];
    w = IterStack[W][0] = (HCompl->SliceDist
                         - HCompl->Slice[X]*x
                         - HCompl->Slice[Y]*y
                         - HCompl->Slice[Z]*z)/HCompl->Slice[T];

    Exit_Value = HCompl->Exit_Value;

    for (i = 1; i <= HCompl->Num_Iterations; ++i)
    {
        yz = y * y + z * z;
        xw = x * x + w * w;

        if ((F_Value = xw + yz) > Exit_Value)
        {
            CalcNormal(H_Normal, i - 1, HCompl, IterStack);

            Step = dot(H_Normal, Direction);

            if (Step < -Fractal_Tolerance)
            {
                Step = -2.0 * Step;

                if ((F_Value > HCompl->Precision * Step) && (F_Value < 30 * HCompl->Precision * Step))
                {
                    *Dist = F_Value / Step;

                    return (false);
                }
            }

            *Dist = HCompl->Precision;

            return (false);
        }

        IterStack[X][i] = xw - yz + HCompl->Julia_Parm[X];
        IterStack[Y][i] = 2.0 * (x * y - z * w) + HCompl->Julia_Parm[Y];
        IterStack[Z][i] = 2.0 * (x * z - w * y) + HCompl->Julia_Parm[Z];
        IterStack[W][i] = 2.0 * (x * w + y * z) + HCompl->Julia_Parm[T];

        w = IterStack[W][i];
        x = IterStack[X][i];

        z = IterStack[Z][i];
        y = IterStack[Y][i];
    }

    *Dist = HCompl->Precision;

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
* CHANGES
*
******************************************************************************/

void HypercomplexFractalRules::CalcNormal(Vector3d& Result, int N_Max, const Fractal *, DBL **IterStack) const
{
    DBL n1, n2, n3, n4;
    int i;
    DBL x, y, z, w;
    DBL xx, yy, zz, ww;
    DBL Pow;

    /*
     * Algebraic properties of hypercomplexes allows simplifications in
     * computations...
     */

    x = IterStack[X][0];
    y = IterStack[Y][0];
    z = IterStack[Z][0];
    w = IterStack[W][0];

    Pow = 2.0;

    for (i = 1; i < N_Max; ++i)
    {

        /*
         * For a map z->f(z), f depending on c, one must perform here :
         *
         * (x,y,z,w) * df/dz(IterStack[X][i],IterStack[Y][i],IterStack[Z][i],IterStack[W][i]) -> (x,y,z,w) ,
         *
         * up to a constant.
         */

        /******************* Case z->z^2+c *****************/

        /* the df/dz part needs no work */

        HMult(xx, yy, zz, ww, IterStack[X][i], IterStack[Y][i], IterStack[Z][i], IterStack[W][i], x, y, z, w);

        w = ww;
        z = zz;
        y = yy;
        x = xx;

        Pow *= 2.0;
    }

    n1 = IterStack[X][N_Max] * Pow;
    n2 = IterStack[Y][N_Max] * Pow;
    n3 = IterStack[Z][N_Max] * Pow;
    n4 = IterStack[W][N_Max] * Pow;

    Result[X] = x * n1 + y * n2 + z * n3 + w * n4;
    Result[Y] = -y * n1 + x * n2 - w * n3 + z * n4;
    Result[Z] = -z * n1 - w * n2 + x * n3 + y * n4;
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
* CHANGES
*
******************************************************************************/

bool HypercomplexBaseFractalRules::Bound(const BasicRay& ray, const Fractal *fractal, DBL *Depth_Min, DBL *Depth_Max) const
{
    return (Sphere::Intersect(ray, fractal->Center, fractal->Radius_Squared, Depth_Min, Depth_Max));
}

/****************************************************************/
/*--------------------------- z3 -------------------------------*/
/****************************************************************/



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
* CHANGES
*
******************************************************************************/

bool HypercomplexZ3FractalRules::Iterate(const Vector3d& IPoint, const Fractal *HCompl, DBL **IterStack) const
{
    int i;
    DBL Norm, xx, yy, zz, ww;
    DBL Exit_Value;
    DBL x, y, z, w;

    x = IterStack[X][0] = IPoint[X];
    y = IterStack[Y][0] = IPoint[Y];
    z = IterStack[Z][0] = IPoint[Z];
    w = IterStack[W][0] = (HCompl->SliceDist
                         - HCompl->Slice[X]*x
                         - HCompl->Slice[Y]*y
                         - HCompl->Slice[Z]*z)/HCompl->Slice[T];

    Exit_Value = HCompl->Exit_Value;

    for (i = 1; i <= HCompl->Num_Iterations; ++i)
    {
        Norm = x * x + y * y + z * z + w * w;

        /* is this test correct ? */
        if (Norm > Exit_Value)
        {
            return (false);
        }

        /*************** Case: z->z^2+c *********************/
        HSqr(xx, yy, zz, ww, x, y, z, w);

        x = IterStack[X][i] = xx + HCompl->Julia_Parm[X];
        y = IterStack[Y][i] = yy + HCompl->Julia_Parm[Y];
        z = IterStack[Z][i] = zz + HCompl->Julia_Parm[Z];
        w = IterStack[W][i] = ww + HCompl->Julia_Parm[T];

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
* CHANGES
*
******************************************************************************/

bool HypercomplexZ3FractalRules::Iterate(const Vector3d& IPoint, const Fractal *HCompl, const Vector3d& Direction, DBL *Dist, DBL **IterStack) const
{
    int i;
    DBL xx, yy, zz, ww;
    DBL Exit_Value, F_Value, Step;
    DBL x, y, z, w;
    Vector3d H_Normal;

    x = IterStack[X][0] = IPoint[X];
    y = IterStack[Y][0] = IPoint[Y];
    z = IterStack[Z][0] = IPoint[Z];
    w = IterStack[W][0] = (HCompl->SliceDist
                         - HCompl->Slice[X]*x
                         - HCompl->Slice[Y]*y
                         - HCompl->Slice[Z]*z)/HCompl->Slice[T];

    Exit_Value = HCompl->Exit_Value;

    for (i = 1; i <= HCompl->Num_Iterations; ++i)
    {
        F_Value = x * x + y * y + z * z + w * w;

        if (F_Value > Exit_Value)
        {
            CalcNormal(H_Normal, i - 1, HCompl, IterStack);

            Step = dot(H_Normal, Direction);

            if (Step < -Fractal_Tolerance)
            {
                Step = -2.0 * Step;

                if ((F_Value > HCompl->Precision * Step) && (F_Value < 30 * HCompl->Precision * Step))
                {
                    *Dist = F_Value / Step;

                    return (false);
                }
            }

            *Dist = HCompl->Precision;

            return (false);
        }

        /*************** Case: z->z^2+c *********************/

        HSqr(xx, yy, zz, ww, x, y, z, w);

        x = IterStack[X][i] = xx + HCompl->Julia_Parm[X];
        y = IterStack[Y][i] = yy + HCompl->Julia_Parm[Y];
        z = IterStack[Z][i] = zz + HCompl->Julia_Parm[Z];
        w = IterStack[W][i] = ww + HCompl->Julia_Parm[T];
    }

    *Dist = HCompl->Precision;

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
* CHANGES
*
******************************************************************************/

void HypercomplexZ3FractalRules::CalcNormal(Vector3d& Result, int N_Max, const Fractal *, DBL **IterStack) const
{
    DBL n1, n2, n3, n4;
    int i;
    DBL x, y, z, w;
    DBL xx, yy, zz, ww;

    /*
     * Algebraic properties of hypercomplexes allows simplifications in
     * computations...
     */

    x = IterStack[X][0];
    y = IterStack[Y][0];
    z = IterStack[Z][0];
    w = IterStack[W][0];

    for (i = 1; i < N_Max; ++i)
    {
        /*
         * For a map z->f(z), f depending on c, one must perform here :
         *
         * (x,y,z,w) * df/dz(IterStack[X][i],IterStack[Y][i],IterStack[Z][i],IterStack[W][i]) -> (x,y,z,w) ,
         *
         * up to a constant.
         */

        /******************* Case z->z^2+c *****************/

        /* the df/dz part needs no work */

        HMult(xx, yy, zz, ww, IterStack[X][i], IterStack[Y][i], IterStack[Z][i], IterStack[W][i], x, y, z, w);

        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }

    n1 = IterStack[X][N_Max];
    n2 = IterStack[Y][N_Max];
    n3 = IterStack[Z][N_Max];
    n4 = IterStack[W][N_Max];

    Result[X] = x * n1 + y * n2 + z * n3 + w * n4;
    Result[Y] = -y * n1 + x * n2 - w * n3 + z * n4;
    Result[Z] = -z * n1 - w * n2 + x * n3 + y * n4;
}



/*--------------------------- Inv -------------------------------*/


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
* CHANGES
*
******************************************************************************/

bool HypercomplexReciprocalFractalRules::Iterate(const Vector3d& IPoint, const Fractal *HCompl, DBL **IterStack) const
{
    int i;
    DBL Norm, xx, yy, zz, ww;
    DBL Exit_Value;
    DBL x, y, z, w;

    x = IterStack[X][0] = IPoint[X];
    y = IterStack[Y][0] = IPoint[Y];
    z = IterStack[Z][0] = IPoint[Z];
    w = IterStack[W][0] = (HCompl->SliceDist
                         - HCompl->Slice[X]*x
                         - HCompl->Slice[Y]*y
                         - HCompl->Slice[Z]*z)/HCompl->Slice[T];

    Exit_Value = HCompl->Exit_Value;

    for (i = 1; i <= HCompl->Num_Iterations; ++i)
    {
        Norm = x * x + y * y + z * z + w * w;

        if (Norm > Exit_Value)
        {
            return (false);
        }

        HReciprocal(&xx, &yy, &zz, &ww, x, y, z, w);

        x = IterStack[X][i] = xx + HCompl->Julia_Parm[X];
        y = IterStack[Y][i] = yy + HCompl->Julia_Parm[Y];
        z = IterStack[Z][i] = zz + HCompl->Julia_Parm[Z];
        w = IterStack[W][i] = ww + HCompl->Julia_Parm[T];

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
* CHANGES
*
******************************************************************************/

bool HypercomplexReciprocalFractalRules::Iterate(const Vector3d& IPoint, const Fractal *HCompl, const Vector3d& Direction, DBL *Dist, DBL **IterStack) const
{
    int i;
    DBL xx, yy, zz, ww;
    DBL Exit_Value, F_Value, Step;
    DBL x, y, z, w;
    Vector3d H_Normal;

    x = IterStack[X][0] = IPoint[X];
    y = IterStack[Y][0] = IPoint[Y];
    z = IterStack[Z][0] = IPoint[Z];
    w = IterStack[W][0] = (HCompl->SliceDist
                         - HCompl->Slice[X]*x
                         - HCompl->Slice[Y]*y
                         - HCompl->Slice[Z]*z)/HCompl->Slice[T];

    Exit_Value = HCompl->Exit_Value;

    for (i = 1; i <= HCompl->Num_Iterations; ++i)
    {
        F_Value = x * x + y * y + z * z + w * w;

        if (F_Value > Exit_Value)
        {
            CalcNormal(H_Normal, i - 1, HCompl, IterStack);

            Step = dot(H_Normal, Direction);

            if (Step < -Fractal_Tolerance)
            {
                Step = -2.0 * Step;

                if ((F_Value > HCompl->Precision * Step) && F_Value < (30 * HCompl->Precision * Step))
                {
                    *Dist = F_Value / Step;

                    return (false);
                }
            }

            *Dist = HCompl->Precision;

            return (false);
        }

        HReciprocal(&xx, &yy, &zz, &ww, x, y, z, w);

        x = IterStack[X][i] = xx + HCompl->Julia_Parm[X];
        y = IterStack[Y][i] = yy + HCompl->Julia_Parm[Y];
        z = IterStack[Z][i] = zz + HCompl->Julia_Parm[Z];
        w = IterStack[W][i] = ww + HCompl->Julia_Parm[T];

    }

    *Dist = HCompl->Precision;

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
* CHANGES
*
******************************************************************************/

void HypercomplexReciprocalFractalRules::CalcNormal(Vector3d& Result, int N_Max, const Fractal *, DBL **IterStack) const
{
    DBL n1, n2, n3, n4;
    int i;
    DBL x, y, z, w;
    DBL xx, yy, zz, ww;
    DBL xxx, yyy, zzz, www;

    /*
     * Algebraic properties of hypercomplexes allows simplifications in
     * computations...
     */

    x = IterStack[X][0];
    y = IterStack[Y][0];
    z = IterStack[Z][0];
    w = IterStack[W][0];

    for (i = 1; i < N_Max; ++i)
    {
        /******************* Case: z->1/z+c *****************/

        HReciprocal(&xx, &yy, &zz, &ww, IterStack[X][i], IterStack[Y][i], IterStack[Z][i], IterStack[W][i]);

        HSqr(xxx, yyy, zzz, www, xx, yy, zz, ww);

        HMult(xx, yy, zz, ww, x, y, z, w, -xxx, -yyy, -zzz, -www);

        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }

    n1 = IterStack[X][N_Max];
    n2 = IterStack[Y][N_Max];
    n3 = IterStack[Z][N_Max];
    n4 = IterStack[W][N_Max];

    Result[X] = x * n1 + y * n2 + z * n3 + w * n4;
    Result[Y] = -y * n1 + x * n2 - w * n3 + z * n4;
    Result[Z] = -z * n1 - w * n2 + x * n3 + y * n4;
}



/*--------------------------- Function -------------------------------*/


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
* CHANGES
*
******************************************************************************/

bool HypercomplexFunctionFractalRules::Iterate(const Vector3d& IPoint, const Fractal *HCompl, DBL **IterStack) const
{
    int i;
    DBL Norm, xx, yy, zz, ww;
    DBL Exit_Value;
    DBL x, y, z, w;

    x = IterStack[X][0] = IPoint[X];
    y = IterStack[Y][0] = IPoint[Y];
    z = IterStack[Z][0] = IPoint[Z];
    w = IterStack[W][0] = (HCompl->SliceDist
                         - HCompl->Slice[X]*x
                         - HCompl->Slice[Y]*y
                         - HCompl->Slice[Z]*z)/HCompl->Slice[T];

    Exit_Value = HCompl->Exit_Value;

    for (i = 1; i <= HCompl->Num_Iterations; ++i)
    {
        Norm = x * x + y * y + z * z + w * w;

        if (Norm > Exit_Value)
        {
            return (false);
        }

        HFunc(&xx, &yy, &zz, &ww, x, y, z, w, HCompl);

        x = IterStack[X][i] = xx + HCompl->Julia_Parm[X];
        y = IterStack[Y][i] = yy + HCompl->Julia_Parm[Y];
        z = IterStack[Z][i] = zz + HCompl->Julia_Parm[Z];
        w = IterStack[W][i] = ww + HCompl->Julia_Parm[T];

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
* CHANGES
*
******************************************************************************/

bool HypercomplexFunctionFractalRules::Iterate(const Vector3d& IPoint, const Fractal *HCompl, const Vector3d& Direction, DBL *Dist, DBL **IterStack) const
{
    int i;
    DBL xx, yy, zz, ww;
    DBL Exit_Value, F_Value, Step;
    DBL x, y, z, w;
    Vector3d H_Normal;

    x = IterStack[X][0] = IPoint[X];
    y = IterStack[Y][0] = IPoint[Y];
    z = IterStack[Z][0] = IPoint[Z];
    w = IterStack[W][0] = (HCompl->SliceDist
                         - HCompl->Slice[X]*x
                         - HCompl->Slice[Y]*y
                         - HCompl->Slice[Z]*z)/HCompl->Slice[T];

    Exit_Value = HCompl->Exit_Value;

    for (i = 1; i <= HCompl->Num_Iterations; ++i)
    {
        F_Value = x * x + y * y + z * z + w * w;

        if (F_Value > Exit_Value)
        {
            CalcNormal(H_Normal, i - 1, HCompl, IterStack);

            Step = dot(H_Normal, Direction);

            if (Step < -Fractal_Tolerance)
            {
                Step = -2.0 * Step;

                if ((F_Value > HCompl->Precision * Step) && F_Value < (30 * HCompl->Precision * Step))
                {
                    *Dist = F_Value / Step;

                    return (false);
                }
            }

            *Dist = HCompl->Precision;

            return (false);
        }

        HFunc(&xx, &yy, &zz, &ww, x, y, z, w, HCompl);

        x = IterStack[X][i] = xx + HCompl->Julia_Parm[X];
        y = IterStack[Y][i] = yy + HCompl->Julia_Parm[Y];
        z = IterStack[Z][i] = zz + HCompl->Julia_Parm[Z];
        w = IterStack[W][i] = ww + HCompl->Julia_Parm[T];

    }

    *Dist = HCompl->Precision;

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
* CHANGES
*
******************************************************************************/

void HypercomplexFunctionFractalRules::CalcNormal(Vector3d& Result, int N_Max, const Fractal *fractal, DBL **IterStack) const
{
    DBL n1, n2, n3, n4;
    int i;
    DBL x, y, z, w;
    DBL xx, yy, zz, ww;
    DBL xxx, yyy, zzz, www;

    /*
     * Algebraic properties of hypercomplexes allows simplifications in
     * computations...
     */

    x = IterStack[X][0];
    y = IterStack[Y][0];
    z = IterStack[Z][0];
    w = IterStack[W][0];

    for (i = 1; i < N_Max; ++i)
    {
        /**************** Case: z-> f(z)+c ************************/

        HFunc(&xx, &yy, &zz, &ww, IterStack[X][i], IterStack[Y][i], IterStack[Z][i], IterStack[W][i], fractal);

        HMult(xxx, yyy, zzz, www, xx, yy, zz, ww, x, y, z, w);

        x = xxx;
        y = yyy;
        z = zzz;
        w = www;
    }

    n1 = IterStack[X][N_Max];
    n2 = IterStack[Y][N_Max];
    n3 = IterStack[Z][N_Max];
    n4 = IterStack[W][N_Max];

    Result[X] = x * n1 + y * n2 + z * n3 + w * n4;
    Result[Y] = -y * n1 + x * n2 - w * n3 + z * n4;
    Result[Z] = -z * n1 - w * n2 + x * n3 + y * n4;
}



/*****************************************************************************
*
* FUNCTION  Complex transcental functions
*
* INPUT     pointer to source complex number
*
* OUTPUT    fn(input)
*
* RETURNS   void
*
* AUTHOR
*
*   Tim Wegner
*
* DESCRIPTION  Calculate common functions on complexes
*   Since our purpose is fractals, error checking is lax.
*
* CHANGES
*
******************************************************************************/

void Complex_Mult (Complex *target, const Complex *source1, const Complex *source2)
{
    DBL tmpx;
    tmpx = source1->x * source2->x - source1->y * source2->y;
    target->y = source1->x * source2->y + source1->y * source2->x;
    target->x = tmpx;
}

void Complex_Div (Complex *target, const Complex *source1, const Complex *source2)
{
    DBL mod,tmpx,yxmod,yymod;
    mod = Sqr(source2->x) + Sqr(source2->y);
    if (mod==0)
        return;
    yxmod = source2->x/mod;
    yymod = - source2->y/mod;
    tmpx = source1->x * yxmod - source1->y * yymod;
    target->y = source1->x * yymod + source1->y * yxmod;
    target->x = tmpx;
} /* End Complex_Mult() */

void Complex_Exp (Complex *target, const Complex *source, const Complex *)
{
    DBL expx;
    expx = exp(source->x);
    target->x = expx * cos(source->y);
    target->y = expx * sin(source->y);
} /* End Complex_Exp() */

void Complex_Sin (Complex *target, const Complex *source, const Complex *)
{
    target->x = sin(source->x) * cosh(source->y);
    target->y = cos(source->x) * sinh(source->y);
} /* End Complex_Sin() */

void Complex_Sinh (Complex *target, const Complex *source, const Complex *)
{
    target->x = sinh(source->x) * cos(source->y);
    target->y = cosh(source->x) * sin(source->y);
} /* End Complex_Sinh() */


void Complex_Cos (Complex *target, const Complex *source, const Complex *)
{
    target->x = cos(source->x) * cosh(source->y);
    target->y = -sin(source->x) * sinh(source->y);
} /* End Complex_Cos() */

void Complex_Cosh (Complex *target, const Complex *source, const Complex *)
{
    target->x = cosh(source->x) * cos(source->y);
    target->y = sinh(source->x) * sin(source->y);
} /* End Complex_Cosh() */


void Complex_Ln (Complex *target, const Complex *source, const Complex *)
{
    DBL mod,zx,zy;
    mod = sqrt(source->x * source->x + source->y * source->y);
    zx = log(mod);
    zy = atan2(source->y,source->x);

    target->x = zx;
    target->y = zy;
} /* End Complex_Ln() */

void Complex_Sqrt(Complex *target, const Complex *source)
{
    DBL mag;
    DBL theta;

    if(source->x == 0.0 && source->y == 0.0)
    {
        target->x = target->y = 0.0;
    }
    else
    {
        mag   = sqrt(sqrt(Sqr(source->x) + Sqr(source->y)));
        theta = atan2(source->y, source->x) / 2;
        target->y = mag * sin(theta);
        target->x = mag * cos(theta);
    }
} /* End Complex_Sqrt() */

/* rz=Arcsin(z)=-i*Log{i*z+sqrt(1-z*z)} */
void Complex_ASin(Complex *target, const Complex *source, const Complex *)
{
    Complex tempz1,tempz2;

    Complex_Mult(&tempz1, source, source);
    tempz1.x = 1 - tempz1.x; tempz1.y = -tempz1.y;
    Complex_Sqrt( &tempz1, &tempz1);

    tempz2.x = -source->y; tempz2.y = source->x;
    tempz1.x += tempz2.x;  tempz1.y += tempz2.y;

    Complex_Ln( &tempz1, &tempz1);
    target->x = tempz1.y;  target->y = -tempz1.x;
} /* End Complex_ASin() */


void Complex_ACos(Complex *target, const Complex *source, const Complex *)
{
    Complex temp;

    Complex_Mult(&temp, source, source);
    temp.x -= 1;
    Complex_Sqrt(&temp, &temp);

    temp.x += source->x; temp.y += source->y;

    Complex_Ln(&temp, &temp);
    target->x = temp.y;  target->y = -temp.x;
} /* End Complex_ACos() */

void Complex_ASinh(Complex *target, const Complex *source, const Complex *)
{
    Complex temp;

    Complex_Mult (&temp, source, source);
    temp.x += 1;
    Complex_Sqrt (&temp, &temp);
    temp.x += source->x; temp.y += source->y;
    Complex_Ln(target, &temp);
} /* End Complex_ASinh */

/* rz=Arccosh(z)=Log(z+sqrt(z*z-1)} */
void Complex_ACosh (Complex *target, const Complex *source, const Complex *)
{
    Complex tempz;
    Complex_Mult(&tempz, source, source);
    tempz.x -= 1;
    Complex_Sqrt (&tempz, &tempz);
    tempz.x = source->x + tempz.x; tempz.y = source->y + tempz.y;
    Complex_Ln (target, &tempz);
} /* End Complex_ACosh() */

/* rz=Arctanh(z)=1/2*Log{(1+z)/(1-z)} */
void Complex_ATanh(Complex *target, const Complex *source, const Complex *)
{
    Complex temp0,temp1,temp2;

    if( source->x == 0.0)
    {
        target->x = 0;
        target->y = atan( source->y);
        return;
    }
    else
    {
        if( fabs(source->x) == 1.0 && source->y == 0.0)
        {
            return;
        }
        else if( fabs( source->x) < 1.0 && source->y == 0.0)
        {
            target->x = log((1+source->x)/(1-source->x))/2;
            target->y = 0;
            return;
        }
        else
        {
            temp0.x = 1 + source->x; temp0.y = source->y;
            temp1.x = 1 - source->x; temp1.y = -source->y;
            Complex_Div(&temp2, &temp0, &temp1);
            Complex_Ln(&temp2, &temp2);
            target->x = .5 * temp2.x; target->y = .5 * temp2.y;
            return;
        }
    }
} /* End Complex_ATanh() */

/* rz=Arctan(z)=i/2*Log{(1-i*z)/(1+i*z)} */
void Complex_ATan(Complex *target, const Complex *source, const Complex *)
{
    Complex temp0,temp1,temp2,temp3;
    if( source->x == 0.0 && source->y == 0.0)
        target->x = target->y = 0;
    else if( source->x != 0.0 && source->y == 0.0){
        target->x = atan(source->x);
        target->y = 0;
    }
    else if( source->x == 0.0 && source->y != 0.0){
        temp0.x = source->y;  temp0.y = 0.0;
        Complex_ATanh(&temp0, &temp0, nullptr);
        target->x = -temp0.y; target->y = temp0.x;
    }
    else if( source->x != 0.0 && source->y != 0.0)
    {
        temp0.x = -source->y; temp0.y = source->x;
        temp1.x = 1 - temp0.x; temp1.y = -temp0.y;
        temp2.x = 1 + temp0.x; temp2.y = temp0.y;

        Complex_Div(&temp3, &temp1, &temp2);
        Complex_Ln(&temp3, &temp3);
        target->x = -temp3.y * .5; target->y = .5 * temp3.x;
    }
} /* End Complex_ATanz() */

void Complex_Tan(Complex *target, const Complex *source, const Complex *)
{
    DBL x, y, sinx, cosx, sinhy, coshy, denom;
    x = 2 * source->x;
    y = 2 * source->y;
    sinx = sin(x); cosx = cos(x);
    sinhy = sinh(y); coshy = cosh(y);
    denom = cosx + coshy;
    if(denom == 0)
        return;
    target->x = sinx/denom;
    target->y = sinhy/denom;
} /* End Complex_Tan() */

void Complex_Tanh(Complex *target, const Complex *source, const Complex *)
{
    DBL x, y, siny, cosy, sinhx, coshx, denom;
    x = 2 * source->x;
    y = 2 * source->y;
    siny = sin(y); cosy = cos(y);
    sinhx = sinh(x); coshx = cosh(x);
    denom = coshx + cosy;
    if(denom == 0)
        return;
    target->x = sinhx/denom;
    target->y = siny/denom;
} /* End Complex_Tanh() */


void Complex_Pwr (Complex *target, const Complex *source1, const Complex *source2)
{
    Complex cLog, t;
    DBL e2x;

    if(source1->x == 0 && source1->y == 0)
    {
        target->x = target->y = 0.0;
        return;
    }

    Complex_Ln (&cLog, source1);
    Complex_Mult (&t, &cLog, source2);

    if(t.x < -690)
        e2x = 0;
    else
        e2x = exp(t.x);
    target->x = e2x * cos(t.y);
    target->y = e2x * sin(t.y);
}

}
// end of namespace pov
