//******************************************************************************
///
/// @file core/shape/lathe.h
///
/// Declarations related to the lathe geometric primitive.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_CORE_LATHE_H
#define POVRAY_CORE_LATHE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/scene/object.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor definitions
******************************************************************************/

#define LATHE_OBJECT (STURM_OK_OBJECT)

#define LINEAR_SPLINE    1
#define QUADRATIC_SPLINE 2
#define CUBIC_SPLINE     3
#define BEZIER_SPLINE    4

/* Generate additional lathe statistics. */

#define LATHE_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct BCyl_Struct BCYL;

typedef struct Lathe_Struct LATHE;
typedef struct Lathe_Spline_Struct LATHE_SPLINE;
typedef struct Lathe_Spline_Entry_Struct LATHE_SPLINE_ENTRY;

struct Lathe_Spline_Entry_Struct
{
    Vector2d A, B, C, D;  /* Coefficients of segment */
};

struct Lathe_Spline_Struct
{
    int References;             /* Count references to this structure. */
    LATHE_SPLINE_ENTRY *Entry;  /* Array of spline segments.           */
    BCYL *BCyl;                 /* bounding cylinder.                  */
};

class Lathe : public ObjectBase
{
    public:
        int Spline_Type;          /* Spline type (linear, quadratic ...)  */
        int Number;               /* Number of segments!!!                */
        LATHE_SPLINE *Spline;     /* Pointer to spline array              */
        DBL Height1, Height2;     /* Min./Max. height                     */
        DBL Radius1, Radius2;     /* Min./Max. radius                     */

        Lathe();
        virtual ~Lathe();

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

        void Compute_Lathe(Vector2d *P, TraceThreadData *);
    protected:
        bool Intersect(const BasicRay& ray, IStack& Depth_Stack, TraceThreadData *Thread);
        bool test_hit(const BasicRay&, IStack&, DBL, DBL, int, TraceThreadData *Thread);
};

}

#endif // POVRAY_CORE_LATHE_H
