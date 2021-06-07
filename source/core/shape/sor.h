//******************************************************************************
///
/// @file core/shape/sor.h
///
/// Declarations related to the surface of revolution (sor) geometric primitive.
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

#ifndef POVRAY_CORE_SOR_H
#define POVRAY_CORE_SOR_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/bounding/boundingcylinder_fwd.h"
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

#define SOR_OBJECT (STURM_OK_OBJECT)

/// @}
///
//******************************************************************************

/* Generate additional surface of revolution statistics. */

#define SOR_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

struct Sor_Spline_Entry_Struct final
{
    DBL A, B, C, D;
};
using SOR_SPLINE_ENTRY = Sor_Spline_Entry_Struct; ///< @deprecated

struct Sor_Spline_Struct final
{
    int References;
    SOR_SPLINE_ENTRY *Entry;
    BCYL *BCyl;                 /* bounding cylinder.                  */
};
using SOR_SPLINE = Sor_Spline_Struct; ///< @deprecated

class Sor final : public ObjectBase
{
    public:
        int Number;
        SOR_SPLINE *Spline;      /* List of spline segments     */
        DBL Height1, Height2;    /* Min./Max. height            */
        DBL Radius1, Radius2;    /* Min./Max. radius            */
        DBL Base_Radius_Squared; /* Radius**2 of the base plane */
        DBL Cap_Radius_Squared;  /* Radius**2 of the cap plane  */

        Sor();
        virtual ~Sor() override;

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

        void Compute_Sor(Vector2d *P, RenderStatistics& stats);
    protected:
        bool Intersect(const BasicRay& ray, IStack& Depth_Stack, TraceThreadData *Thread);
        bool test_hit(const BasicRay&, IStack&, DBL, DBL, int, int, TraceThreadData *Thread);
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_SOR_H
