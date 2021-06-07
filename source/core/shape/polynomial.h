//******************************************************************************
///
/// @file core/shape/polynomial.h
///
/// Declarations related to the 3-variable polynomial geometric primitive.
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

#ifndef POVRAY_CORE_POLYNOMIAL_H
#define POVRAY_CORE_POLYNOMIAL_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/scene/object.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCoreShape
///
/// @{

//******************************************************************************
///
/// @name Object Types
///
/// @{

#define POLY_OBJECT    (STURM_OK_OBJECT)
#define CUBIC_OBJECT   (STURM_OK_OBJECT)
#define QUARTIC_OBJECT (STURM_OK_OBJECT)

/// @}
///
//******************************************************************************

/* Number of coefficients of a three variable polynomial of order x */

inline int term_counts(int x) { return ((x+1)*(x+2)*(x+3)/6); }



/*****************************************************************************
* Global typedefs
******************************************************************************/

class Poly final : public ObjectBase
{
    public:
        int Order;
        DBL *Coeffs;

        Poly(int order);
        virtual ~Poly() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;
        virtual bool Intersect_BBox(BBoxDirection, const BBoxVector3d&, const BBoxVector3d&, BBoxScalar) const override;

        bool Set_Coeff(const unsigned int x,const unsigned int y, const unsigned int z, const DBL value);
    protected:
        static int intersect(const BasicRay &Ray, int Order, const DBL *Coeffs, int Sturm_Flag, DBL *Depths, RenderStatistics& stats);
        static void normal0(Vector3d& Result, int Order, const DBL *Coeffs, const Vector3d& IPoint);
        static void normal1(Vector3d& Result, int Order, const DBL *Coeffs, const Vector3d& IPoint);
        static DBL inside(const Vector3d& IPoint, int Order, const DBL *Coeffs);
        static int intersect_linear(const BasicRay &ray, const DBL *Coeffs, DBL *Depths);
        static int intersect_quadratic(const BasicRay &ray, const DBL *Coeffs, DBL *Depths);
        // static int factor_out(int n, int i, int *c, int *s);
        //static int binomial(int n, int r);
        //static void factor1(int n, int *c, int *s);
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_POLYNOMIAL_H
