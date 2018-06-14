//******************************************************************************
///
/// @file core/shape/polyline.h
///
/// Declarations related to the winding polyline geometric primitive.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_CORE_POLYLINE_H
#define POVRAY_CORE_POLYLINE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

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

#define POLYLINE_OBJECT (BASIC_OBJECT)

/// @}
///
//******************************************************************************


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Polyline_Data_Struct POLYLINE_DATA;

struct Polyline_Data_Struct
{
    std::vector<Vector2d> Points;
    std::vector<bool> SelectedWindingNumber;
};

class Polyline : public NonsolidObject
{
    public:
        Vector3d S_Normal;
        std::shared_ptr<POLYLINE_DATA> Data;

        Polyline();
        virtual ~Polyline();

        virtual ObjectPtr Copy();

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
        virtual bool Inside(const Vector3d&, TraceThreadData *) const;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;
        virtual void Translate(const Vector3d&, const TRANSFORM *);
        virtual void Rotate(const Vector3d&, const TRANSFORM *);
        virtual void Scale(const Vector3d&, const TRANSFORM *);
        virtual void Transform(const TRANSFORM *);
        virtual void Compute_BBox();

        bool Compute_Polyline(std::vector<Vector3d>& points, std::vector<bool>& range);
    protected:
        bool Intersect(const BasicRay& ray, DBL *Depth, TraceThreadData *Thread) const;
        bool in_polyline(DBL u, DBL  v)const;
        enum class RELATIVE
        {
          LEFT, ON, RIGHT
        };
        RELATIVE relative( const double* a, const double* b, DBL tx, DBL ty)const;
};

/// @}
///
//##############################################################################

}

#endif // POVRAY_CORE_POLYLINE_H
