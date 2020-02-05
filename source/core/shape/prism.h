//******************************************************************************
///
/// @file core/shape/prism.h
///
/// Declarations related to the prism geometric primitive.
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

#ifndef POVRAY_CORE_PRISM_H
#define POVRAY_CORE_PRISM_H

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

#define PRISM_OBJECT (STURM_OK_OBJECT)

/// @}
///
//******************************************************************************

#define LINEAR_SPLINE    1
#define QUADRATIC_SPLINE 2
#define CUBIC_SPLINE     3
#define BEZIER_SPLINE    4

#define LINEAR_SWEEP 1
#define CONIC_SWEEP  2

/* Generate additional prism statistics. */

#define PRISM_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

struct Prism_Spline_Entry_Struct final
{
    DBL x1, y1, x2, y2;  /* Min./Max. coordinates of segment   */
    DBL v1, u2, v2;      /* Min./Max. coordinates of segment in <u,v>, u1 not needed  */
    Vector2d A, B, C, D; /* Coefficients of segment            */
};
using PRISM_SPLINE_ENTRY = Prism_Spline_Entry_Struct; ///< @deprecated

struct Prism_Spline_Struct final
{
    int References;
    PRISM_SPLINE_ENTRY *Entry;
};
using PRISM_SPLINE = Prism_Spline_Struct; ///< @deprecated

class Prism final : public ObjectBase
{
    public:
        int Number;
        int Spline_Type;          /* Spline type (linear, quadratic ...)        */
        int Sweep_Type;           /* Sweep type (linear, conic)                 */
        DBL Height1, Height2;
        DBL x1, y1, x2, y2;       /* Overall bounding rectangle of spline curve */
        PRISM_SPLINE *Spline;     /* Pointer to array of splines                */
        DBL u1, v1, u2, v2;       /* Overall <u,v> bounding rectangle of spline */

        Prism();
        virtual ~Prism() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;

        void Compute_Prism(Vector2d *P, RenderStatistics& stats);
    protected:
        int in_curve(DBL u, DBL v, RenderStatistics& stats) const;
        static bool test_rectangle(const Vector3d& P, const Vector3d& D, DBL x1, DBL y1, DBL x2, DBL y2);
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_PRISM_H
