//******************************************************************************
///
/// @file core/shape/heightfield.h
///
/// Declarations related to the height field geometric primitive.
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

#ifndef POVRAY_CORE_HEIGHTFIELD_H
#define POVRAY_CORE_HEIGHTFIELD_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/scene/object.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define HFIELD_OBJECT (BASIC_OBJECT+HIERARCHY_OK_OBJECT)

/* Generate additional height field statistics. */

#define HFIELD_EXTRA_STATS 1


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct HField_Data_Struct HFIELD_DATA;
typedef struct HField_Block_Struct HFIELD_BLOCK;
typedef short HF_Normals[3];

struct HField_Block_Struct
{
    int xmin, xmax;
    int zmin, zmax;
    DBL ymin, ymax;
};

struct HField_Data_Struct
{
    int References;
    int Normals_Height;  /* Needed for Destructor */
    int max_x, max_z;
    HF_VAL min_y, max_y;
    int block_max_x, block_max_z;
    int block_width_x, block_width_z;
    HF_VAL **Map;
    HF_Normals **Normals;
    HFIELD_BLOCK **Block;
};

class ImageData;

/// Height field geometric primitive.
///
/// The shape is implemented as a collection of triangles which are calculated as needed.
///
/// The basic intersection routine first computes the ray's intersection with the box marking the limits of the shape,
/// then follows the line from one intersection point to the other, testing the two triangles which form the pixel for
/// an intersection with the ray at each step.
///
class HField : public ObjectBase
{
    public:
        Vector3d bounding_corner1;
        Vector3d bounding_corner2;
        HFIELD_DATA *Data;

        HField();
        virtual ~HField();

        virtual ObjectPtr Copy();

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
        virtual bool Inside(const Vector3d&, TraceThreadData *) const;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const;
        virtual void Translate(const Vector3d&, const TRANSFORM *);
        virtual void Rotate(const Vector3d&, const TRANSFORM *);
        virtual void Scale(const Vector3d&, const TRANSFORM *);
        virtual void Transform(const TRANSFORM *);
        virtual void Compute_BBox();

        void Compute_HField(const ImageData *image);
    protected:
        static DBL normalize(Vector3d& A, const Vector3d& B);
        void smooth_height_field(int xsize, int zsize);
        bool intersect_pixel(int x, int z, const BasicRay& ray, DBL height1, DBL height2, IStack &HField_Stack, const BasicRay &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread);
        static int add_single_normal(HF_VAL **data, int xsize, int zsize, int x0, int z0,int x1, int z1,int x2, int z2, Vector3d& N);
        bool dda_traversal(const BasicRay &ray, const Vector3d& Start, const HFIELD_BLOCK *Block, IStack &HField_Stack, const BasicRay &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread);
        bool block_traversal(const BasicRay &ray, const Vector3d& Start, IStack &HField_Stack, const BasicRay &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread);
        void build_hfield_blocks();
};

}

#endif // POVRAY_CORE_HEIGHTFIELD_H
