//******************************************************************************
///
/// @file core/shape/heightfield.h
///
/// Declarations related to the height field geometric primitive.
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

#ifndef POVRAY_CORE_HEIGHTFIELD_H
#define POVRAY_CORE_HEIGHTFIELD_H

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

#define HFIELD_OBJECT (BASIC_OBJECT+HIERARCHY_OK_OBJECT)

/// @}
///
//******************************************************************************

#define HFIELD_EXTRA_STATS 1


/*****************************************************************************
* Global typedefs
******************************************************************************/

struct HFData;
struct HFBlock;

class ImageData;

/// Height field geometric primitive.
///
/// The shape is implemented as a collection of triangles which are calculated as needed.
///
/// The basic intersection routine first computes the ray's intersection with the box marking the limits of the shape,
/// then follows the line from one intersection point to the other, testing the two triangles which form the pixel for
/// an intersection with the ray at each step.
///
class HField final : public ObjectBase
{
    public:
        Vector3d bounding_corner1;
        Vector3d bounding_corner2;
        HFData *Data;

        HField();
        virtual ~HField() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;

        void Compute_HField(const ImageData *image);
    protected:
        static DBL normalize(Vector3d& A, const Vector3d& B);
        void smooth_height_field(int xsize, int zsize);
        bool intersect_pixel(int x, int z, const BasicRay& ray, DBL height1, DBL height2, IStack &HField_Stack, const BasicRay &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread);
        static int add_single_normal(HF_VAL **data, int xsize, int zsize, int x0, int z0,int x1, int z1,int x2, int z2, Vector3d& N);
        bool dda_traversal(const BasicRay &ray, const Vector3d& Start, const HFBlock *Block, IStack &HField_Stack, const BasicRay &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread);
        bool block_traversal(const BasicRay &ray, const Vector3d& Start, IStack &HField_Stack, const BasicRay &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread);
        void build_hfield_blocks();
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_HEIGHTFIELD_H
