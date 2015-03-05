//******************************************************************************
///
/// @file core/shape/parametric.h
///
/// This module contains all defines, typedefs, and prototypes for
/// @ref parametric.cpp.
///
/// This module was written by D.Skarda&T.Bily and modified by R.Suzuki.
/// Ported to POV-Ray 3.5 by Thorsten Froehlich.
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
//******************************************************************************

#ifndef FPMETRIC_H
#define FPMETRIC_H

#include "parser/parser.h" // TODO - avoid this (pulled in for function stuff)
#include "backend/scene/objects.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define PARAMETRIC_OBJECT        (PATCH_OBJECT)



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct PrecompParValues_Struct PRECOMP_PAR_DATA;

struct PrecompParValues_Struct
{
    int use, depth;
    char flags;
    DBL *Low[3], *Hi[3];     /*  X,Y,Z  */
};

class Parametric : public NonsolidObject
{
    public:

        FunctionVM *vm;
        FUNCTION_PTR Function[3];
        DBL umin, umax, vmin, vmax;
        DBL accuracy;
        DBL max_gradient;

        shared_ptr<ContainedByShape> container;

        Parametric();
        virtual ~Parametric();

        virtual ObjectPtr Copy();

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
        virtual bool Inside(const Vector3d&, TraceThreadData *) const;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;
        virtual void UVCoord(Vector2d&, const Intersection *, TraceThreadData *) const;
        virtual void Translate(const Vector3d&, const TRANSFORM *);
        virtual void Rotate(const Vector3d&, const TRANSFORM *);
        virtual void Scale(const Vector3d&, const TRANSFORM *);
        virtual void Transform(const TRANSFORM *);
        virtual void Compute_BBox();

        void Precompute_Parametric_Values(char flags, int depth, FPUContext *ctx);
    protected:
        void Precomp_Par_Int(int depth, DBL umin, DBL vmin, DBL umax, DBL vmax, FPUContext *ctx);
        PRECOMP_PAR_DATA *Copy_PrecompParVal();
        void Destroy_PrecompParVal();

        static inline DBL Evaluate_Function_UV(FPUContext *ctx, FUNCTION funct, const Vector2d& fnvec);
        static inline void Evaluate_Function_Interval_UV(FPUContext *ctx, FUNCTION funct, DBL threshold, const Vector2d& fnvec_low, const Vector2d& fnvec_hi, DBL max_gradient, DBL& low, DBL& hi);
        static void Interval(DBL dx, DBL a, DBL b, DBL max_gradient, DBL *Min, DBL *Max);
    private:
        PRECOMP_PAR_DATA *PData;
        int PrecompLastDepth;
};

}

#endif
