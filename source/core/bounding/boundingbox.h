//******************************************************************************
///
/// @file core/bounding/boundingbox.h
///
/// Declarations related to bounding boxes.
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

#ifndef POVRAY_CORE_BOUNDINGBOX_H
#define POVRAY_CORE_BOUNDINGBOX_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/coretypes.h"
#include "core/math/matrix.h"

namespace pov
{

class Intersection;
class Ray;
struct RayObjectCondition;
class RenderStatistics;
class TraceThreadData;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Generate additional bbox statistics. */

#define BBOX_EXTRA_STATS 1


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef SNGL BBoxScalar;
typedef GenericVector3d<BBoxScalar> BBoxVector3d;

/// Structure holding bounding box data.
///
/// @note       The current implementation stores the data in lowerLeft/size format.
///
/// @todo       The reliability of the bounding mechanism could probably be improved by storing the
///             bounding data in min/max format rather than lowerLeft/size, and making sure
///             high-precision values are rounded towards positive/negative infinity as appropriate.
///
struct BoundingBox
{
    BBoxVector3d lowerLeft;
    BBoxVector3d size;

    SNGL GetMinX() const { return lowerLeft.x(); }
    SNGL GetMinY() const { return lowerLeft.y(); }
    SNGL GetMinZ() const { return lowerLeft.z(); }

    SNGL GetMaxX() const { return lowerLeft.x() + size.x(); }
    SNGL GetMaxY() const { return lowerLeft.y() + size.y(); }
    SNGL GetMaxZ() const { return lowerLeft.z() + size.z(); }

    bool isEmpty() const { return (size.x() < 0) || (size.y() < 0) || (size.z() < 0); }
};

/// @relates BoundingBox
inline void Make_BBox(BoundingBox& BBox, const BBoxScalar llx, const BBoxScalar lly, const BBoxScalar llz, const BBoxScalar lex, const BBoxScalar ley, const BBoxScalar lez)
{
    BBox.lowerLeft = BBoxVector3d(llx, lly, llz);
    BBox.size      = BBoxVector3d(lex, ley, lez);
}

/// @relates BoundingBox
inline void Make_BBox_from_min_max(BoundingBox& BBox, const BBoxVector3d& mins, const BBoxVector3d& maxs)
{
    BBox.lowerLeft = mins;
    BBox.size      = maxs - mins;
}

/// @relates BoundingBox
inline void Make_BBox_from_min_max(BoundingBox& BBox, const Vector3d& mins, const Vector3d& maxs)
{
    BBox.lowerLeft = BBoxVector3d(mins);
    BBox.size      = BBoxVector3d(maxs - mins);
}

/// @relates BoundingBox
inline void Make_min_max_from_BBox(BBoxVector3d& mins, BBoxVector3d& maxs, const BoundingBox& BBox)
{
    mins = BBox.lowerLeft;
    maxs = mins + BBox.size;
}

/// @relates BoundingBox
inline void Make_min_max_from_BBox(Vector3d& mins, Vector3d& maxs, const BoundingBox& BBox)
{
    mins = Vector3d(BBox.lowerLeft);
    maxs = mins + Vector3d(BBox.size);
}

/// @relates BoundingBox
inline bool Inside_BBox(const Vector3d& point, const BoundingBox& bbox)
{
    if (point.x() < (DBL)bbox.lowerLeft.x())
        return(false);
    if (point.y() < (DBL)bbox.lowerLeft.y())
        return(false);
    if (point.z() < (DBL)bbox.lowerLeft.z())
        return(false);
    if (point.x() > (DBL)bbox.lowerLeft.x() + (DBL)bbox.size.x())
        return(false);
    if (point.y() > (DBL)bbox.lowerLeft.y() + (DBL)bbox.size.y())
        return(false);
    if (point.z() > (DBL)bbox.lowerLeft.z() + (DBL)bbox.size.z())
        return(false);

    return(true);
}

/// Structure holding bounding box data in min/max format.
///
struct MinMaxBoundingBox
{
    BBoxVector3d pmin;
    BBoxVector3d pmax;
};

typedef struct BBox_Tree_Struct BBOX_TREE;
typedef BBOX_TREE* BBoxTreePtr;
typedef const BBOX_TREE* ConstBBoxTreePtr;

struct BBox_Tree_Struct
{
    short Infinite;   // Flag if node is infinite
    short Entries;    // Number of sub-nodes in this node
    BoundingBox BBox; // Bounding box of this node
    BBOX_TREE **Node; // If node: children; if leaf: element
};

typedef bool VECTORB[3];

class Rayinfo
{
    public:
        BBoxVector3d slab_num;
        BBoxVector3d slab_den;
        VECTORB nonzero;
        VECTORB positive;

        explicit Rayinfo(const BasicRay& ray)
        {
            DBL t;

            slab_num[X] = ray.Origin[X];
            slab_num[Y] = ray.Origin[Y];
            slab_num[Z] = ray.Origin[Z];

            if((nonzero[X] = ((t = ray.Direction[X]) != 0.0)) != 0)
            {
                slab_den[X] = 1.0 / t;
                positive[X] = (ray.Direction[X] > 0.0);
            }

            if((nonzero[Y] = ((t = ray.Direction[Y]) != 0.0)) != 0)
            {
                slab_den[Y] = 1.0 / t;
                positive[Y] = (ray.Direction[Y] > 0.0);
            }

            if((nonzero[Z] = ((t = ray.Direction[Z]) != 0.0)) != 0)
            {
                slab_den[Z] = 1.0 / t;
                positive[Z] = (ray.Direction[Z] > 0.0);
            }
        }
};

enum BBoxDirection
{
    BBOX_DIR_X0Y0Z0 = 0,
    BBOX_DIR_X0Y0Z1 = 1,
    BBOX_DIR_X0Y1Z0 = 2,
    BBOX_DIR_X0Y1Z1 = 3,
    BBOX_DIR_X1Y0Z0 = 4,
    BBOX_DIR_X1Y0Z1 = 5,
    BBOX_DIR_X1Y1Z0 = 6,
    BBOX_DIR_X1Y1Z1 = 7
};


/*****************************************************************************
* Global functions
******************************************************************************/

/// Container for BBox subtrees prioritized by depth (distance to ray origin).
///
/// The current implementation is based on a so-called _min heap_ stored in a
/// (resizable) array, obeying the following rules:
///   - Element 0 is unused.
///   - If both element N and element 2*N are non-empty, element N has the
///     smaller depth of the two.
///   - If both element N and element 2*N+1 are non-empty, element N has the
///     smaller depth of the two.
///   - If element N is empty, so is element N+1.
/// This implementation guarantees execution time of O(log N) or better both
/// for inserting a new element and for extracting the one with the smallest
/// depth.
///
/// @note   We're using a custom priority queue class for now rather than
///         `std::priority_queue` becase we make use of Clear(), an operation
///         which `std::priority_queue` does not support.
///
class BBoxPriorityQueue
{
    public:

        BBoxPriorityQueue();
        ~BBoxPriorityQueue();

        void Insert(DBL depth, ConstBBoxTreePtr node);
        bool RemoveMin(DBL& depth, ConstBBoxTreePtr& node);
        bool IsEmpty() const;
        void Clear();

    protected:

        struct Qelem
        {
            DBL depth;
            ConstBBoxTreePtr node;
        };

        vector<Qelem> mQueue;
};

void Build_BBox_Tree(BBOX_TREE **Root, size_t numOfFiniteObjects, BBOX_TREE **&Finite, size_t numOfInfiniteObjects, BBOX_TREE **Infinite, size_t& maxfinitecount);
void Build_Bounding_Slabs(BBOX_TREE **Root, vector<ObjectPtr>& objects, unsigned int& numberOfFiniteObjects, unsigned int& numberOfInfiniteObjects, unsigned int& numberOfLightSources);

void Recompute_BBox(BoundingBox *bbox, const TRANSFORM *trans);
void Recompute_Inverse_BBox(BoundingBox *bbox, const TRANSFORM *trans);
bool Intersect_BBox_Tree(BBoxPriorityQueue& pqueue, const BBOX_TREE *Root, const Ray& ray, Intersection *Best_Intersection, TraceThreadData *Thread);
bool Intersect_BBox_Tree(BBoxPriorityQueue& pqueue, const BBOX_TREE *Root, const Ray& ray, Intersection *Best_Intersection, const RayObjectCondition& precondition, const RayObjectCondition& postcondition, TraceThreadData *Thread);
void Check_And_Enqueue(BBoxPriorityQueue& Queue, const BBOX_TREE *Node, const BoundingBox *BBox, const Rayinfo *rayinfo, RenderStatistics& Stats);
void Destroy_BBox_Tree(BBOX_TREE *Node);


/*****************************************************************************
* Inline functions
******************************************************************************/

// Calculate the volume of a bounding box. [DB 8/94]
inline void BOUNDS_VOLUME(DBL& a, const BoundingBox& b)
{
    a = b.size[X] * b.size[Y] * b.size[Z];
}

}

#endif // POVRAY_CORE_BOUNDINGBOX_H
