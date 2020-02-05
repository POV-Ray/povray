//******************************************************************************
///
/// @file core/shape/parametric.h
///
/// Declarations related to the parametric geometric primitive.
///
/// @author D.Skarda, T.Bily (original code)
/// @author R.Suzuki (modifications)
/// @author Thorsten Froehlich (porting to POV-Ray v3.5)
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

#ifndef POVRAY_CORE_PARAMETRIC_H
#define POVRAY_CORE_PARAMETRIC_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/coretypes.h"
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

#define PARAMETRIC_OBJECT        (PATCH_OBJECT)

/// @}
///
//******************************************************************************


/*****************************************************************************
* Global typedefs
******************************************************************************/

struct PrecompParValues_Struct final
{
    int use, depth;
    char flags;
    DBL *Low[3], *Hi[3];     /*  X,Y,Z  */
};
using PRECOMP_PAR_DATA = PrecompParValues_Struct; ///< @deprecated

class Parametric final : public NonsolidObject
{
    public:

        GenericScalarFunctionPtr Function[3];
        DBL umin, umax, vmin, vmax;
        DBL accuracy;
        DBL max_gradient;

        std::shared_ptr<ContainedByShape> container;

        Parametric();
        virtual ~Parametric() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void UVCoord(Vector2d&, const Intersection *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;

        void Precompute_Parametric_Values(char flags, int depth, TraceThreadData *Thread);
    protected:
        void Precomp_Par_Int(int depth, DBL umin, DBL vmin, DBL umax, DBL vmax, GenericScalarFunctionInstance aFn[3]);
        PRECOMP_PAR_DATA *Copy_PrecompParVal();
        void Destroy_PrecompParVal();

        static inline void Evaluate_Function_Interval_UV(GenericScalarFunctionInstance& fn, DBL threshold, const Vector2d& fnvec_low, const Vector2d& fnvec_hi, DBL max_gradient, DBL& low, DBL& hi);
        static void Interval(DBL dx, DBL a, DBL b, DBL max_gradient, DBL *Min, DBL *Max);
    private:
        PRECOMP_PAR_DATA *PData;
        int PrecompLastDepth;
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_PARAMETRIC_H
