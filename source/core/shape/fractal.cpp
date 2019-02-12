//******************************************************************************
///
/// @file core/shape/fractal.cpp
///
/// Implementation of the fractal set geometric primitives.
///
/// @author Pascal Massimino
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
#include "core/shape/fractal.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"

// POV-Ray header files (core module)
#include "core/bounding/boundingbox.h"
#include "core/math/hypercomplex.h"
#include "core/math/matrix.h"
#include "core/math/quaternion.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"
#include "core/shape/sphere.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

const DBL Fractal_Tolerance = 1e-7;

#define Iteration(V,F,IS) ( (F)->Rules->Iterate(V,F,IS) )
#define Normal_Calc(F,V,IS) ( (F)->Rules->CalcNormal(V,(F)->Num_Iterations,F,IS) )
#define F_Bound(R,F,dm,dM) ( (F)->Rules->Bound(R,F,dm,dM) )
#define D_Iteration(V,F,I,D,IS) ( (F)->Rules->Iterate(V,F,I,D,IS) )

/*****************************************************************************
* Local variables
******************************************************************************/

const COMPLEX_FUNCTION_METHOD Complex_Function_List[] =
{
    /* must match STYPE list in fractal.h */
    Complex_Exp,
    Complex_Ln,
    Complex_Sin,
    Complex_ASin,
    Complex_Cos,
    Complex_ACos,
    Complex_Tan,
    Complex_ATan,
    Complex_Sinh,
    Complex_ASinh,
    Complex_Cosh,
    Complex_ACosh,
    Complex_Tanh,
    Complex_ATanh,
    Complex_Pwr
};



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

bool Fractal::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    bool Intersection_Found;
    bool LastIsInside = false;
    bool CurrentIsInside, NextIsInside;
    DBL Depth, Depth_Max;
    DBL Dist, Dist_Next, LenSqr, LenInv;

    Vector3d IPoint, Mid_Point, Next_Point, Real_Pt;
    Vector3d Real_Normal, F_Normal;
    Vector3d Direction;
    BasicRay New_Ray;

    Thread->Stats()[Ray_Fractal_Tests]++;

    Intersection_Found = false;

    /* Get into Fractal's world. */

    if (Trans != nullptr)
    {
        MInvTransDirection(Direction, ray.Direction, Trans);
        LenSqr = Direction.lengthSqr();

        if (LenSqr == 0.0)
        {
            return (false);
        }

        if (LenSqr != 1.0)
        {
            LenInv = 1.0 / sqrt(LenSqr);
            Direction *= LenInv;
        }
        else
            LenInv = 1.0;

        New_Ray.Direction = Direction;
        MInvTransPoint(New_Ray.Origin, ray.Origin, Trans);
    }
    else
    {
        Direction = ray.Direction;
        New_Ray = ray;
        LenInv = 1.0;
    }

    /* Bound fractal. */

    if (!F_Bound(New_Ray, this, &Depth, &Depth_Max))
    {
        return (false);
    }

    if (Depth_Max < Fractal_Tolerance)
    {
        return (false);
    }

    if (Depth < Fractal_Tolerance)
    {
        Depth = Fractal_Tolerance;
    }

    /* Jump to starting point */

    Next_Point = New_Ray.Origin + Direction * Depth;

    CurrentIsInside = D_Iteration(Next_Point, this, Direction, &Dist, Thread->Fractal_IStack);

    /* Light ray starting inside ? */

    if (CurrentIsInside)
    {
        Next_Point += (2.0 * Fractal_Tolerance) * Direction;

        Depth += 2.0 * Fractal_Tolerance;

        if (Depth > Depth_Max)
        {
            return (false);
        }

        CurrentIsInside = D_Iteration(Next_Point, this, Direction, &Dist, Thread->Fractal_IStack);
    }

    /* Ok. Trace it */

    while (Depth < Depth_Max)
    {
        /*
         * Get close to the root: Advance with Next_Point, keeping track of last
         * position in IPoint...
         */

        while (1)
        {
            if (Dist < Precision)
                Dist = Precision;

            Depth += Dist;

            if (Depth > Depth_Max)
            {
                if (Intersection_Found)
                    Thread->Stats()[Ray_Fractal_Tests_Succeeded]++;
                return (Intersection_Found);
            }

            IPoint = Next_Point;
            Next_Point += Dist * Direction;

            NextIsInside = D_Iteration(Next_Point, this, Direction, &Dist_Next, Thread->Fractal_IStack);

            if (NextIsInside != CurrentIsInside)
            {
                /* Set surface was crossed... */

                Depth -= Dist;
                break;
            }
            else
            {
                Dist = Dist_Next; /* not reached */
            }
        }

        /* then, polish the root via bisection method... */

        while (Dist > Fractal_Tolerance)
        {
            Dist *= 0.5;
            Mid_Point = IPoint + Dist * Direction;

            LastIsInside = Iteration(Mid_Point, this, Thread->Fractal_IStack);

            if (LastIsInside == CurrentIsInside)
            {
                IPoint = Mid_Point;

                Depth += Dist;

                if (Depth > Depth_Max)
                {
                    if (Intersection_Found)
                        Thread->Stats()[Ray_Fractal_Tests_Succeeded]++;
                    return (Intersection_Found);
                }
            }
        }

        if (!CurrentIsInside) /* Mid_Point isn't inside the set */
        {
            IPoint += Dist * Direction;

            Depth += Dist;

            Iteration(IPoint, this, Thread->Fractal_IStack);
        }
        else
        {
            if (LastIsInside != CurrentIsInside)
            {
                Iteration(IPoint, this, Thread->Fractal_IStack);
            }
        }

        if (Trans != nullptr)
        {
            MTransPoint(Real_Pt, IPoint, Trans);
            Normal_Calc(this, F_Normal, Thread->Fractal_IStack);
            MTransNormal(Real_Normal, F_Normal, Trans);
        }
        else
        {
            Real_Pt = IPoint;
            Normal_Calc(this, Real_Normal, Thread->Fractal_IStack);
        }

        if (Clip.empty() || Point_In_Clip(Real_Pt, Clip, Thread))
        {
            Real_Normal.normalize();
            Depth_Stack->push(Intersection(Depth * LenInv, Real_Pt, Real_Normal, this));
            Intersection_Found = true;

            /* If fractal isn't used with CSG we can exit now. */

            if (!(Type & IS_CHILD_OBJECT))
            {
                break;
            }
        }

        /* Start over where work was left */

        IPoint = Next_Point;
        Dist = Dist_Next;
        CurrentIsInside = NextIsInside;

    }

    if (Intersection_Found)
        Thread->Stats()[Ray_Fractal_Tests_Succeeded]++;
    return (Intersection_Found);
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

bool Fractal::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    bool Result;
    Vector3d New_Point;

    if (Trans != nullptr)
    {
        MInvTransPoint(New_Point, IPoint, Trans);

        Result = Iteration(New_Point, this, Thread->Fractal_IStack);
    }
    else
    {
        Result = Iteration(IPoint, this, Thread->Fractal_IStack);
    }

    if (Test_Flag(this, INVERTED_FLAG))
    {
        return (!Result);
    }
    else
    {
        return (Result);
    }
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

void Fractal::Normal(Vector3d& Result, Intersection *Intersect, TraceThreadData *) const
{
    Result = Intersect->INormal;
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

void Fractal::Translate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
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

void Fractal::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
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

void Fractal::Scale(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
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
*   Mar 1996 : Moved call to Recompute_BBox to Compute_Fractal_BBox() (TW)
*
******************************************************************************/

void Fractal::Transform(const TRANSFORM *tr)
{
    if (Trans == nullptr)
        Trans = Create_Transform();

    Compose_Transforms(Trans, tr);

    Compute_BBox();
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
*   Mar 1996 : Added call to recompute_BBox() to bottom (TW)
*
******************************************************************************/

void Fractal::Compute_BBox()
{
    DBL R;

    switch (Algebra)
    {
        case QUATERNION_TYPE:

            R = 1.0 + sqrt(Sqr(Julia_Parm[X]) + Sqr(Julia_Parm[Y]) + Sqr(Julia_Parm[Z]) + Sqr(Julia_Parm[T]));
            R += Fractal_Tolerance; /* fix bug when Julia_Parameter exactly 0 */

            if (R > 2.0)
            {
                R = 2.0;
            }

            Exit_Value = Sqr(R) + Fractal_Tolerance;

            break;

        case HYPERCOMPLEX_TYPE:
        default:

            R = 4.0;

            Exit_Value = 16.0;

            break;
    }

    Radius_Squared = Sqr(R);

    Make_BBox(BBox, -R, -R, -R, 2.0 * R, 2.0 * R, 2.0 * R);

    Recompute_BBox(&BBox, Trans);
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

Fractal::Fractal() : ObjectBase(BASIC_OBJECT)
{
    Trans = nullptr;

    Center = Vector3d(0.0, 0.0, 0.0);

    Julia_Parm[X] = 1.0;
    Julia_Parm[Y] = 0.0;
    Julia_Parm[Z] = 0.0;
    Julia_Parm[T] = 0.0;

    Slice[X] = 0.0;
    Slice[Y] = 0.0;
    Slice[Z] = 0.0;
    Slice[T] = 1.0;
    SliceDist = 0.0;

    Exit_Value = 4.0;

    Num_Iterations = 20;

    Precision = 1.0 / 20.0;

    Algebra = QUATERNION_TYPE;

    Sub_Type = SQR_STYPE;

    Rules.reset();

    Radius_Squared = 0.0;
    exponent.x = 0.0;
    exponent.y = 0.0;
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

ObjectPtr Fractal::Copy()
{
    Fractal *New = new Fractal();
    Destroy_Transform(New->Trans);
    *New = *this;
    New->Trans = Copy_Transform(Trans);
    New->Rules = Rules;

    return (New);
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

Fractal::~Fractal()
{}

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

int Fractal::SetUp_Fractal(void)
{
    switch (Algebra)
    {
        case QUATERNION_TYPE:

            switch(Sub_Type)
            {
                case CUBE_STYPE:
                    Rules = FractalRulesPtr(new Z3FractalRules());
                    break;
                case SQR_STYPE:
                    Rules = FractalRulesPtr(new JuliaFractalRules());
                    break;
                default:
                    throw POV_EXCEPTION_STRING("Illegal function: quaternion only supports sqr and cube");
            }

            break;

        case HYPERCOMPLEX_TYPE:

            switch (Sub_Type)
            {
                case RECIPROCAL_STYPE:

                    Rules = FractalRulesPtr(new HypercomplexReciprocalFractalRules());
                    break;

                case EXP_STYPE:
                case LN_STYPE:
                case SIN_STYPE:
                case ASIN_STYPE:
                case COS_STYPE:
                case ACOS_STYPE:
                case TAN_STYPE:
                case ATAN_STYPE:
                case SINH_STYPE:
                case ASINH_STYPE:
                case COSH_STYPE:
                case ACOSH_STYPE:
                case TANH_STYPE:
                case ATANH_STYPE:
                case PWR_STYPE:

                    Rules = FractalRulesPtr(new HypercomplexFunctionFractalRules(Complex_Function_List[Sub_Type]));
                    break;

                case CUBE_STYPE:

                    Rules = FractalRulesPtr(new HypercomplexZ3FractalRules());
                    break;

                case SQR_STYPE:

                    Rules = FractalRulesPtr(new HypercomplexFractalRules());
                    break;

                default:

                    throw POV_EXCEPTION_STRING("Subtype unknown in fractal.");
            }

            break;

        default:

            throw POV_EXCEPTION_STRING("Algebra unknown in fractal.");
    }

    Compute_BBox();

    return Num_Iterations;
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

void Fractal::Allocate_Iteration_Stack(DBL **IStack, int Len)
{
    Free_Iteration_Stack(IStack);
    if (Len == 0)
        return ;
    const int len = (Len + 1) * sizeof(DBL);
    for (int i = 0 ; i < 4 ; i++)
        IStack [i] = reinterpret_cast<DBL *>(POV_MALLOC(len, "fractal iteration stack"));
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

void Fractal::Free_Iteration_Stack(DBL **IStack)
{
    for (int i = 0 ; i < 4 ; i++)
    {
        if (IStack [i] != nullptr)
        {
            POV_FREE (IStack [i]) ;
            IStack [i] = nullptr;
        }
    }
}

}
// end of namespace pov
