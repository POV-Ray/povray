//******************************************************************************
///
/// @file core/shape/fractal.h
///
/// Declarations related to the fractal set geometric primitives.
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

#ifndef POVRAY_CORE_FRACTAL_H
#define POVRAY_CORE_FRACTAL_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/math/matrix.h"
#include "core/math/vector.h"
#include "core/scene/object.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCoreShape
///
/// @{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define QUATERNION_TYPE    0
#define HYPERCOMPLEX_TYPE  1

/* Hcmplx function stypes must come first */
#define EXP_STYPE          0
#define LN_STYPE           1
#define SIN_STYPE          2
#define ASIN_STYPE         3
#define COS_STYPE          4
#define ACOS_STYPE         5
#define TAN_STYPE          6
#define ATAN_STYPE         7
#define SINH_STYPE         8
#define ASINH_STYPE        9
#define COSH_STYPE        10
#define ACOSH_STYPE       11
#define TANH_STYPE        12
#define ATANH_STYPE       13
#define PWR_STYPE         14

/* end function stypes */
#define SQR_STYPE         15
#define CUBE_STYPE        16
#define RECIPROCAL_STYPE  17

/*****************************************************************************
* Global typedefs
******************************************************************************/

class Fractal;

struct Complex final
{
    DBL x,y;
};

class Fractal final : public ObjectBase
{
    public:
        Vector3d Center;
        VECTOR_4D Julia_Parm;
        VECTOR_4D Slice;              /* vector perpendicular to slice plane */
        DBL SliceDist;                /* distance from slice plane to origin */
        DBL Exit_Value;
        int Num_Iterations;           /* number of iterations */
        DBL Precision;                /* Precision value */
        int Algebra;                  /* Quaternion or Hypercomplex */
        int Sub_Type;
        Complex exponent;             /* exponent of power function */
        DBL Radius_Squared;           /* For F_Bound(), if needed */
        FractalRulesPtr Rules;

        Fractal();
        virtual ~Fractal() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;

        static void Free_Iteration_Stack(DBL **IStack);
        static void Allocate_Iteration_Stack(DBL **IStack, int Len);

        int SetUp_Fractal(void);
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_FRACTAL_H
