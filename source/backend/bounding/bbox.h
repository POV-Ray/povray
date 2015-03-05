//******************************************************************************
///
/// @file backend/bounding/bbox.h
///
/// This module contains all defines, typedefs, and prototypes for `bbox.cpp`.
///
/// @note   `frame.h` contains other bound stuff.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef BBOX_H
#define BBOX_H

#include "backend/frame.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* Generate additional bbox statistics. */

#define BBOX_EXTRA_STATS 1


/*****************************************************************************
* Global typedefs
******************************************************************************/

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
/// for inserting a new element and for removing the one with the smallest
/// depth.
///
/// @note   We're using a custom priority queue class for now rather than
///         `std::priority_queue` becase we make use of Clear(), an operation
///         which `std::priority_queue` does not support.
///
/// @todo   As we seem to be only reading from the queue after all elements
///         have been inserted, a simple sort-based approach might actually
///         be more efficient.
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
void Check_And_Enqueue(BBoxPriorityQueue& Queue, const BBOX_TREE *Node, const BoundingBox *BBox, const Rayinfo *rayinfo, TraceThreadData *Thread);
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

#endif
