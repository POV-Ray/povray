//******************************************************************************
///
/// @file core/shape/polygon.h
///
/// Declarations related to the polygon geometric primitive.
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

#ifndef POVRAY_CORE_POLYGON_H
#define POVRAY_CORE_POLYGON_H

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

#define POLYGON_OBJECT (BASIC_OBJECT)

/// @}
///
//******************************************************************************


/*****************************************************************************
* Global typedefs
******************************************************************************/

struct Polygon_Data_Struct final
{
    int References;
    int Number;
    Vector2d *Points;
};
using POLYGON_DATA = Polygon_Data_Struct; ///< @deprecated

class Polygon final : public NonsolidObject
{
    public:
        Vector3d S_Normal;
        POLYGON_DATA *Data;

        Polygon();
        virtual ~Polygon() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        // virtual void UVCoord(Vector2d&, const Intersection *) const override; // TODO FIXME - does this use the default mapping? [trf]
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;

        void Compute_Polygon(int number, Vector3d *points);
    protected:
        bool Intersect(const BasicRay& ray, DBL *Depth, RenderStatistics& stats) const;
        static bool in_polygon(int number, Vector2d *points, DBL u, DBL  v);
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_POLYGON_H
