//******************************************************************************
///
/// @file core/bounding/boundingbox.cpp
///
/// Implementations related to bounding boxes.
///
/// @author Mark VandeWettering (original idea)
/// @author Alexander Enzmann (adaptation to POV-Ray)
/// @author Eric Haines (optimizations)
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/bounding/boundingbox.h"

// C++ variants of C standard header files
#include <cstdlib>
#include <cstring>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"

// POV-Ray header files (core module)
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/object.h"
#include "core/scene/tracethreaddata.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::vector;

const int BUNCHING_FACTOR = 4;
// Initial number of entries in a priority queue.
const int INITIAL_PRIORITY_QUEUE_SIZE = 256;
const int BBQ_FIRST_ELEMENT = 1;

BBOX_TREE *create_bbox_node(int size);

int find_axis(BBOX_TREE **Finite, ptrdiff_t first, ptrdiff_t last);
void calc_bbox(BoundingBox *BBox, BBOX_TREE **Finite, ptrdiff_t first, ptrdiff_t last);
void build_area_table(BBOX_TREE **Finite, ptrdiff_t a, ptrdiff_t b, BBoxScalar *areas);
bool sort_and_split(BBOX_TREE **Root, BBOX_TREE **&Finite, size_t *numOfFiniteObjects, ptrdiff_t first, ptrdiff_t last, size_t& maxfinitecount, BBoxScalar **areaCache);

BBoxPriorityQueue::BBoxPriorityQueue()
{
    mQueue.resize(BBQ_FIRST_ELEMENT); // element 0 is reserved
}

BBoxPriorityQueue::~BBoxPriorityQueue()
{}

void BBoxPriorityQueue::Insert(DBL depth, ConstBBoxTreePtr node)
{
    vector<Qelem>::size_type size;
    vector<Qelem>::size_type i;

    size = mQueue.size();
    mQueue.resize(size+1);

    i = size;
    while((i > BBQ_FIRST_ELEMENT) && (depth < mQueue[i/2].depth))
    {
        mQueue[i] = mQueue[i/2];
        i /= 2;
    }
    mQueue[i].depth = depth;
    mQueue[i].node  = node;
}

bool BBoxPriorityQueue::RemoveMin(DBL& depth, ConstBBoxTreePtr& node)
{
    vector<Qelem>::size_type size = mQueue.size() - 1;
    vector<Qelem>::size_type i, j;

    if (size == 0)
        return false;

    depth = mQueue[BBQ_FIRST_ELEMENT].depth;
    node  = mQueue[BBQ_FIRST_ELEMENT].node;

    i = BBQ_FIRST_ELEMENT;

    while (i <= size/2) // equivalent to 2*i <= size, but more robust
    {
        if ((2*i == size) || (mQueue[2*i].depth < mQueue[2*i+1].depth))
            j = 2*i;
        else
            j = 2*i+1;

        if (mQueue[size].depth <= mQueue[j].depth)
            break;

        mQueue[i] = mQueue[j];
        i = j;
    }
    if (i != size)
        mQueue[i] = mQueue[size];
    mQueue.pop_back();

    return true;
}

bool BBoxPriorityQueue::IsEmpty() const
{
    return (mQueue.size() == BBQ_FIRST_ELEMENT);
}

void BBoxPriorityQueue::Clear()
{
    mQueue.resize(BBQ_FIRST_ELEMENT);
}

void Destroy_BBox_Tree(BBOX_TREE *Node)
{
    if (Node != nullptr)
    {
        if(Node->Entries > 0)
        {
            for(short i = 0; i < Node->Entries; i++)
                Destroy_BBox_Tree(Node->Node[i]);

            POV_FREE(Node->Node);

            Node->Entries = 0;
            Node->Node = nullptr;
        }

        POV_FREE(Node);
    }
}

void Recompute_BBox(BoundingBox *bbox, const TRANSFORM *trans)
{
    int i;
    Vector3d lower_left, lengths, corner;
    Vector3d mins, maxs;

    if (trans == nullptr)
        return;

    lower_left = Vector3d(bbox->lowerLeft);
    lengths    = Vector3d(bbox->size);

    mins = Vector3d(BOUND_HUGE);
    maxs = Vector3d(-BOUND_HUGE);

    for(i = 1; i <= 8; i++)
    {
        corner = lower_left;

        corner[X] += ((i & 1) ? lengths[X] : 0.0);
        corner[Y] += ((i & 2) ? lengths[Y] : 0.0);
        corner[Z] += ((i & 4) ? lengths[Z] : 0.0);

        MTransPoint(corner, corner, trans);

        if(corner[X] < mins[X]) { mins[X] = corner[X]; }
        if(corner[X] > maxs[X]) { maxs[X] = corner[X]; }
        if(corner[Y] < mins[Y]) { mins[Y] = corner[Y]; }
        if(corner[Y] > maxs[Y]) { maxs[Y] = corner[Y]; }
        if(corner[Z] < mins[Z]) { mins[Z] = corner[Z]; }
        if(corner[Z] > maxs[Z]) { maxs[Z] = corner[Z]; }
    }

    // Clip bounding box at the largest allowed bounding box.
    if(mins[X] < -BOUND_HUGE / 2) { mins[X] = -BOUND_HUGE / 2; }
    if(mins[Y] < -BOUND_HUGE / 2) { mins[Y] = -BOUND_HUGE / 2; }
    if(mins[Z] < -BOUND_HUGE / 2) { mins[Z] = -BOUND_HUGE / 2; }
    if(maxs[X] >  BOUND_HUGE / 2) { maxs[X] =  BOUND_HUGE / 2; }
    if(maxs[Y] >  BOUND_HUGE / 2) { maxs[Y] =  BOUND_HUGE / 2; }
    if(maxs[Z] >  BOUND_HUGE / 2) { maxs[Z] =  BOUND_HUGE / 2; }

    Make_BBox_from_min_max(*bbox, mins, maxs);
}

void Recompute_Inverse_BBox(BoundingBox *bbox, const TRANSFORM *trans)
{
    int i;
    Vector3d lower_left, lengths, corner;
    Vector3d mins, maxs;

    if (trans == nullptr)
        return;

    lower_left = Vector3d(bbox->lowerLeft);
    lengths    = Vector3d(bbox->size);

    mins = Vector3d(BOUND_HUGE);
    maxs = Vector3d(-BOUND_HUGE);

    for(i = 1; i <= 8; i++)
    {
        corner = lower_left;

        corner[X] += ((i & 1) ? lengths[X] : 0.0);
        corner[Y] += ((i & 2) ? lengths[Y] : 0.0);
        corner[Z] += ((i & 4) ? lengths[Z] : 0.0);

        MInvTransPoint(corner, corner, trans);

        if(corner[X] < mins[X]) { mins[X] = corner[X]; }
        if(corner[X] > maxs[X]) { maxs[X] = corner[X]; }
        if(corner[Y] < mins[Y]) { mins[Y] = corner[Y]; }
        if(corner[Y] > maxs[Y]) { maxs[Y] = corner[Y]; }
        if(corner[Z] < mins[Z]) { mins[Z] = corner[Z]; }
        if(corner[Z] > maxs[Z]) { maxs[Z] = corner[Z]; }
    }

    // Clip bounding box at the largest allowed bounding box.
    if(mins[X] < -BOUND_HUGE / 2) { mins[X] = -BOUND_HUGE / 2; }
    if(mins[Y] < -BOUND_HUGE / 2) { mins[Y] = -BOUND_HUGE / 2; }
    if(mins[Z] < -BOUND_HUGE / 2) { mins[Z] = -BOUND_HUGE / 2; }
    if(maxs[X] >  BOUND_HUGE / 2) { maxs[X] =  BOUND_HUGE / 2; }
    if(maxs[Y] >  BOUND_HUGE / 2) { maxs[Y] =  BOUND_HUGE / 2; }
    if(maxs[Z] >  BOUND_HUGE / 2) { maxs[Z] =  BOUND_HUGE / 2; }

    Make_BBox_from_min_max(*bbox, mins, maxs);
}

// Create a bounding box hierarchy from a given list of finite and
// infinite elements. Each element consists of
//
// - an infinite flag
// - a bounding box enclosing the element
// - a pointer to the structure representing the element (e.g an object)
void Build_BBox_Tree(BBOX_TREE **Root, size_t numOfFiniteObjects, BBOX_TREE **&Finite, size_t numOfInfiniteObjects, BBOX_TREE **Infinite, size_t& maxfinitecount)
{
    ptrdiff_t low, high;
    BBOX_TREE *cd, *root;

    // This is a resonable guess at the number of finites needed.
    // This array will be reallocated as needed if it isn't.
    maxfinitecount = 2 * numOfFiniteObjects;

    // Now do a sort on the objects, with the end result being
    // a tree of objects sorted along the x, y, and z axes.
    if(numOfFiniteObjects > 0)
    {
        low = 0;
        high = numOfFiniteObjects;

        BBoxScalar *areaCache = new BBoxScalar[numOfFiniteObjects*2];

        while (sort_and_split(Root, Finite, &numOfFiniteObjects, low, high, maxfinitecount, &areaCache))
        {
            low = high;
            high = numOfFiniteObjects;
        }

        delete[] areaCache;

        // Move infinite objects in the first leaf of Root.
        if(numOfInfiniteObjects > 0)
        {
            root = *Root;
            root->Node = reinterpret_cast<BBOX_TREE **>(POV_REALLOC(root->Node, (root->Entries + 1) * sizeof(BBOX_TREE *), "composite"));
            std::memmove(&(root->Node[1]), &(root->Node[0]), root->Entries * sizeof(BBOX_TREE *));
            root->Entries++;
            cd = create_bbox_node(numOfInfiniteObjects);
            for(size_t i = 0; i < numOfInfiniteObjects; i++)
                cd->Node[i] = Infinite[i];

            calc_bbox(&(cd->BBox), Infinite, 0, numOfInfiniteObjects);
            root->Node[0] = cd;
            calc_bbox(&(root->BBox), root->Node, 0, root->Entries);

            // Root and first node are infinite.
            root->Infinite = true;
            root->Node[0]->Infinite = true;
        }
    }
    else
    {
        // There are no finite objects and no Root was created.
        // Create it now and put all infinite objects into it.

        if(numOfInfiniteObjects > 0)
        {
            cd = create_bbox_node(numOfInfiniteObjects);
            for(size_t i = 0; i < numOfInfiniteObjects; i++)
                cd->Node[i] = Infinite[i];
            calc_bbox(&(cd->BBox), Infinite, 0, numOfInfiniteObjects);
            *Root = cd;
            (*Root)->Infinite = true;
        }
    }
}

void Build_Bounding_Slabs(BBOX_TREE **Root, vector<ObjectPtr>& objects, unsigned int& numberOfFiniteObjects, unsigned int& numberOfInfiniteObjects, unsigned int& numberOfLightSources)
{
    ptrdiff_t iFinite, iInfinite;
    BBOX_TREE **Finite, **Infinite;
    ObjectPtr Temp;
    size_t maxfinitecount = 0;

    // Count frame level and infinite objects.
    numberOfFiniteObjects = numberOfInfiniteObjects = numberOfLightSources = 0;

    for(vector<ObjectPtr>::iterator i(objects.begin()); i != objects.end(); i++)
    {
        if((*i)->Type & LIGHT_SOURCE_OBJECT)
        {
            if((reinterpret_cast<LightSource *>(*i))->children.size() > 0)
            {
                Temp = (reinterpret_cast<LightSource *>(*i))->children[0];
                numberOfLightSources++;
            }
            else
                Temp = nullptr;
        }
        else
            Temp = (*i);

        if (Temp != nullptr)
        {
            if(Test_Flag(Temp, INFINITE_FLAG))
                numberOfInfiniteObjects++;
            else
                numberOfFiniteObjects++;
        }
    }

    // If bounding boxes aren't used we can return.
    if(numberOfFiniteObjects + numberOfInfiniteObjects < 1)
        return;

    // This is a reasonable guess at the number of finites needed.
    // This array will be reallocated as needed if it isn't.
    maxfinitecount = 2 * numberOfFiniteObjects;

    // Now allocate an array to hold references to these finites and
    // any new composite objects we may generate.
    Finite = Infinite = nullptr;

    if(numberOfFiniteObjects > 0)
        Finite = new BBOX_TREE* [maxfinitecount];

    // Create array to hold pointers to infinite objects.
    if(numberOfInfiniteObjects > 0)
        Infinite = new BBOX_TREE* [numberOfInfiniteObjects];

    // Init lists.
    for(int i = 0; i < numberOfFiniteObjects; i++)
        Finite[i] = create_bbox_node(0);

    for(int i = 0; i < numberOfInfiniteObjects; i++)
        Infinite[i] = create_bbox_node(0);

    // Set up finite and infinite object lists.
    iFinite = iInfinite = 0;

    for(vector<ObjectPtr>::iterator i(objects.begin()); i != objects.end(); i++)
    {
        if((*i)->Type & LIGHT_SOURCE_OBJECT)
        {
            if((reinterpret_cast<LightSource *>(*i))->children.size() > 0)
                Temp = (reinterpret_cast<LightSource *>(*i))->children[0];
            else
                Temp = nullptr;
        }
        else
            Temp = (*i);

        if (Temp != nullptr)
        {
            // Add object to the appropriate list.
            if(Test_Flag(Temp, INFINITE_FLAG))
            {
                Infinite[iInfinite]->Infinite = true;
                Infinite[iInfinite]->BBox     = Temp->BBox;
                Infinite[iInfinite]->Node     = reinterpret_cast<BBOX_TREE **>(Temp);

                iInfinite++;
            }
            else
            {
                Finite[iFinite]->BBox = Temp->BBox;
                Finite[iFinite]->Node = reinterpret_cast<BBOX_TREE **>(Temp);

                iFinite++;
            }
        }
    }

    // Now build the bounding box tree.
    Build_BBox_Tree(Root, numberOfFiniteObjects, Finite, numberOfInfiniteObjects, Infinite, maxfinitecount);

    // Get rid of the Finite and Infinite arrays and just use Root.
    if (Finite != nullptr)
        delete[] Finite;

    if (Infinite != nullptr)
        delete[] Infinite;
}

bool Intersect_BBox_Tree(BBoxPriorityQueue& pqueue, const BBOX_TREE *Root, const Ray& ray, Intersection *Best_Intersection, TraceThreadData *Thread)
{
    int i, found;
    DBL Depth;
    const BBOX_TREE *Node;
    Intersection New_Intersection;

    // Create the direction vectors for this ray.
    Rayinfo rayinfo(ray);

    // Start with an empty priority queue.
    pqueue.Clear();
    New_Intersection.Object = nullptr;
    found = false;

    // Check top node.
    Check_And_Enqueue(pqueue, Root, &Root->BBox, &rayinfo, Thread->Stats());

    // Check elements in the priority queue.
    while(!pqueue.IsEmpty())
    {
        pqueue.RemoveMin(Depth, Node);

        // If current intersection is larger than the best intersection found
        // so far our task is finished, because all other bounding boxes in
        // the priority queue are further away.
        if(Depth > Best_Intersection->Depth)
            break;

        // Check current node.
        if(Node->Entries)
        {
            // This is a node containing leaves to be checked.
            for (i = 0; i < Node->Entries; i++)
                Check_And_Enqueue(pqueue, Node->Node[i], &Node->Node[i]->BBox, &rayinfo, Thread->Stats());
        }
        else
        {
            // This is a leaf so test contained object.
            if(Find_Intersection(&New_Intersection, reinterpret_cast<ObjectPtr>(Node->Node), ray, Thread))
            {
                if(New_Intersection.Depth < Best_Intersection->Depth)
                {
                    *Best_Intersection = New_Intersection;
                    found = true;
                }
            }
        }
    }

    return (found);
}

bool Intersect_BBox_Tree(BBoxPriorityQueue& pqueue, const BBOX_TREE *Root, const Ray& ray, Intersection *Best_Intersection, const RayObjectCondition& precondition, const RayObjectCondition& postcondition, TraceThreadData *Thread)
{
    int i, found;
    DBL Depth;
    const BBOX_TREE *Node;
    Intersection New_Intersection;

    // Create the direction vectors for this ray.
    Rayinfo rayinfo(ray);

    // Start with an empty priority queue.
    pqueue.Clear();
    New_Intersection.Object = nullptr;
    found = false;

    // Check top node.
    Check_And_Enqueue(pqueue, Root, &Root->BBox, &rayinfo, Thread->Stats());

    // Check elements in the priority queue.
    while(!pqueue.IsEmpty())
    {
        pqueue.RemoveMin(Depth, Node);

        // If current intersection is larger than the best intersection found
        // so far our task is finished, because all other bounding boxes in
        // the priority queue are further away.
        if(Depth > Best_Intersection->Depth)
            break;

        // Check current node.
        if(Node->Entries)
        {
            // This is a node containing leaves to be checked.
            for (i = 0; i < Node->Entries; i++)
                Check_And_Enqueue(pqueue, Node->Node[i], &Node->Node[i]->BBox, &rayinfo, Thread->Stats());
        }
        else
        {
            if(precondition(ray, reinterpret_cast<ObjectPtr>(Node->Node), 0.0) == true)
            {
                // This is a leaf so test contained object.
                if(Find_Intersection(&New_Intersection, reinterpret_cast<ObjectPtr>(Node->Node), ray, postcondition, Thread))
                {
                    if(New_Intersection.Depth < Best_Intersection->Depth)
                    {
                        *Best_Intersection = New_Intersection;
                        found = true;
                    }
                }
            }
        }
    }

    return (found);
}

void Check_And_Enqueue(BBoxPriorityQueue& Queue, const BBOX_TREE *Node, const BoundingBox *BBox, const Rayinfo *rayinfo, RenderStatistics& Stats)
{
    DBL dmin, dmax;

    if(Node->Infinite == false)
    {
        Stats[nChecked]++;

        // Test whether the bounding box is being hit.

        // The bounding box can be thought of as an intersection of three "slabs", by which we mean slices of 3D space
        // bounded by parallel axis-aligned planes; we have an "X slab" bounding the box in the X dimension, an
        // "Y slab", and a "Z slab".

        // With a few exceptions that we need to test for, any ray will intersect all three slabs, defining an interval
        // along the ray in which the ray is inside the slab.

        // Where the intervals for the individual slabs overlap, the ray is inside bounding box.

        // We proceed one dimension at a time.

        // These will keep track of the overlap between the intervals.
        dmin = -BOUND_HUGE;
        dmax =  BOUND_HUGE;

        for (int dim = X; dim <= Z; ++dim)
        {
            if(rayinfo->nonzero[dim])
            {
                // These will hold the distance to the near and far plane, respectively, for this slab.
                DBL tmin, tmax;

                if (rayinfo->positive[dim])
                {
                    // Far plane is "upper" plane, near plane is the "lower" one.
                    tmax = (BBox->lowerLeft[dim] + BBox->size[dim] - rayinfo->origin[dim]) * rayinfo->invDirection[dim];
                    if(tmax < EPSILON)
                        // The far plane is (at least almost) behind the observer,
                        // so the ray is heading away from the box and can't possibly intersect it.
                        return;
                    tmin = (BBox->lowerLeft[dim] - rayinfo->origin[dim]) * rayinfo->invDirection[dim];
                }
                else
                {
                    // Far plane is "lower" plane, near plane is the "upper" one.
                    tmax = (BBox->lowerLeft[dim] - rayinfo->origin[dim]) * rayinfo->invDirection[dim];
                    if(tmax < EPSILON)
                        // The far plane is (at least almost) behind the observer,
                        // so the ray is heading away from the box and can't possibly intersect it.
                        return;
                    tmin = (BBox->lowerLeft[dim] + BBox->size[dim] - rayinfo->origin[dim]) * rayinfo->invDirection[dim];
                }

                // The next portion of code is essentially the same as the following
                // (presuming dmin <= dmax initially), with a lot of shortcuts to bail out early:
                //
                //  if (tmax < dmax) dmax = tmax;   // update the overlap lower bound
                //  if (tmin > dmin) dmin = tmin;   // update the overlap upper bound
                //  if (dmin > dmax) return;        // detect whether there is no overlap

                if (tmax < dmax)
                {
                    if (tmin > dmin)
                    {
                        if(tmin > tmax)
                            return;
                        dmin = tmin;
                    }
                    else
                    {
                        if(dmin > tmax)
                            return;
                    }
                    dmax = tmax;
                }
                else
                {
                    if(tmin > dmin)
                    {
                        if(tmin > dmax)
                            return;
                        dmin = tmin;
                    }
                }
            }
            else
            {
                // Special case: The ray runs parallel to this slab; there ray is either entirely inside the slab,
                // or it is entirely outside; we can easily check this by testing the ray origin.

                if (!IsInRange (rayinfo->origin[dim], BBox->lowerLeft[dim], BBox->lowerLeft[dim] + BBox->size[dim]))
                    // The ray is entirely outside the slab, so it can't possibly hit the bounding box.
                    return;

                // The ray is entirely inside the slab, so this slab has no effect on the end result.
            }
        }

        // If we've made it through to here, the ray does hit the box.

        Stats[nEnqueued]++;
    }
    else
        // Set intersection depth to -Max_Distance.
        dmin = -MAX_DISTANCE;

    Queue.Insert (dmin, Node);
}

BBOX_TREE *create_bbox_node(int size)
{
    BBOX_TREE *New;

    New = reinterpret_cast<BBOX_TREE *>(POV_MALLOC(sizeof(BBOX_TREE), "bounding box node"));

    New->Infinite = false;
    New->Entries = size;

    Make_BBox(New->BBox, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    if(size)
        New->Node = reinterpret_cast<BBOX_TREE **>(POV_MALLOC(size*sizeof(BBOX_TREE *), "bounding box node"));
    else
        New->Node = nullptr;

    return (New);
}

template<int Axis>
int CDECL compboxes(const void *in_a, const void *in_b)
{
    const BoundingBox *a, *b;
    BBoxScalar am, bm;
    typedef const BBOX_TREE *CONST_BBOX_TREE_PTR;

    a = &((*reinterpret_cast<const CONST_BBOX_TREE_PTR *>(in_a))->BBox);
    b = &((*reinterpret_cast<const CONST_BBOX_TREE_PTR *>(in_b))->BBox);

    am = 2.0 * a->lowerLeft[Axis] + a->size[Axis];
    bm = 2.0 * b->lowerLeft[Axis] + b->size[Axis];

    if(am < bm)
        return -1;
    else
    {
        if(am == bm)
            return 0;
        else
            return 1;
    }
}

int find_axis(BBOX_TREE **Finite, ptrdiff_t first, ptrdiff_t last)
{
    int which = X;
    ptrdiff_t i;
    SNGL e, d = -BOUND_HUGE;
    BBoxVector3d mins, maxs;
    BoundingBox *bbox;

    mins = BBoxVector3d(BOUND_HUGE);
    maxs = BBoxVector3d(-BOUND_HUGE);

    for(i = first; i < last; i++)
    {
        bbox = &(Finite[i]->BBox);

        if(bbox->lowerLeft[X] < mins[X])
            mins[X] = bbox->lowerLeft[X];

        if(bbox->lowerLeft[X] + bbox->size[X] > maxs[X])
            maxs[X] = bbox->lowerLeft[X] + bbox->size[X];

        if(bbox->lowerLeft[Y] < mins[Y])
            mins[Y] = bbox->lowerLeft[Y];

        if(bbox->lowerLeft[Y] + bbox->size[Y] > maxs[Y])
            maxs[Y] = bbox->lowerLeft[Y] + bbox->size[Y];

        if(bbox->lowerLeft[Z] < mins[Z])
            mins[Z] = bbox->lowerLeft[Z];

        if(bbox->lowerLeft[Z] + bbox->size[Z] > maxs[Z])
            maxs[Z] = bbox->lowerLeft[Z] + bbox->size[Z];
    }

    e = maxs[X] - mins[X];

    if(e > d)
    {
        d = e;
        which = X;
    }

    e = maxs[Y] - mins[Y];

    if(e > d)
    {
        d = e;
        which = Y;
    }

    e = maxs[Z] - mins[Z];

    if(e > d)
        which = Z;

    return (which);
}

void calc_bbox(BoundingBox *BBox, BBOX_TREE **Finite, ptrdiff_t first, ptrdiff_t last)
{
    ptrdiff_t i;
    DBL tmin, tmax;
    Vector3d bmin, bmax;
    BoundingBox *bbox;

    bmin = Vector3d(BOUND_HUGE);
    bmax = Vector3d(-BOUND_HUGE);

    for(i = first; i < last; i++)
    {
        bbox = &(Finite[i]->BBox);

        tmin = bbox->lowerLeft[X];
        tmax = tmin + bbox->size[X];

        if(tmin < bmin[X]) { bmin[X] = tmin; }
        if(tmax > bmax[X]) { bmax[X] = tmax; }

        tmin = bbox->lowerLeft[Y];
        tmax = tmin + bbox->size[Y];

        if(tmin < bmin[Y]) { bmin[Y] = tmin; }
        if(tmax > bmax[Y]) { bmax[Y] = tmax; }

        tmin = bbox->lowerLeft[Z];
        tmax = tmin + bbox->size[Z];

        if(tmin < bmin[Z]) { bmin[Z] = tmin; }
        if(tmax > bmax[Z]) { bmax[Z] = tmax; }
    }

    Make_BBox_from_min_max(*BBox, bmin, bmax);
}

void build_area_table(BBOX_TREE **Finite, ptrdiff_t a, ptrdiff_t b, BBoxScalar *areas)
{
    ptrdiff_t i, imin, dir;
    BBoxScalar tmin, tmax;
    BBoxVector3d bmin, bmax, len;
    BoundingBox *bbox;

    if (a < b)
    {
        imin = a;  dir =  1;
    }
    else
    {
        imin = b;  dir = -1;
    }

    bmin = BBoxVector3d(BOUND_HUGE);
    bmax = BBoxVector3d(-BOUND_HUGE);

    for(i = a; i != (b + dir); i += dir)
    {
        bbox = &(Finite[i]->BBox);

        tmin = bbox->lowerLeft[X];
        tmax = tmin + bbox->size[X];

        if (tmin < bmin[X]) { bmin[X] = tmin; }
        if (tmax > bmax[X]) { bmax[X] = tmax; }

        tmin = bbox->lowerLeft[Y];
        tmax = tmin + bbox->size[Y];

        if (tmin < bmin[Y]) { bmin[Y] = tmin; }
        if (tmax > bmax[Y]) { bmax[Y] = tmax; }

        tmin = bbox->lowerLeft[Z];
        tmax = tmin + bbox->size[Z];

        if (tmin < bmin[Z]) { bmin[Z] = tmin; }
        if (tmax > bmax[Z]) { bmax[Z] = tmax; }

        len = bmax - bmin;

        areas[i - imin] = len[X] * (len[Y] + len[Z]) + len[Y] * len[Z];
    }
}

bool sort_and_split(BBOX_TREE **Root, BBOX_TREE **&Finite, size_t *numOfFiniteObjects, ptrdiff_t first, ptrdiff_t last, size_t& maxfinitecount, BBoxScalar **areaCache)
{
    ptrdiff_t i, best_loc = -1;
    ptrdiff_t size = last - first;

    if(size <= 0)
        return false;

    // Don't bother to do any further examinations if the BUNCHING_FACTOR is reached.
    if (size > BUNCHING_FACTOR)
    {
        BBoxScalar *area_left, *area_right;
        BBoxScalar best_index, new_index;

        int Axis = find_axis(Finite, first, last);

        // Actually, we could do this faster in several ways. We could use a
        // logn algorithm to find the median along the given axis, and then a
        // linear algorithm to partition along the axis. Oh well.

        switch(Axis)
        {
            case X:
                std::qsort(Finite + first, size, sizeof(BBOX_TREE*), compboxes<X>);
                break;
            case Y:
                std::qsort(Finite + first, size, sizeof(BBOX_TREE*), compboxes<Y>);
                break;
            case Z:
                std::qsort(Finite + first, size, sizeof(BBOX_TREE*), compboxes<Z>);
                break;
        }

        // area_left[] and area_right[] hold the surface areas of the bounding
        // boxes to the left and right of any given point. E.g. area_left[i] holds
        // the surface area of the bounding box containing Finite 0 through i and
        // area_right[i] holds the surface area of the box containing Finite
        // i through size-1.

        area_left  = *areaCache;
        area_right = area_left + size;

        // Precalculate the areas for speed.
        build_area_table(Finite, first, last - 1, area_left);
        build_area_table(Finite, last - 1, first, area_right);
        best_index = area_right[0] * float(size-3); // estimated cost of _not_ subdividing

        for(i = 1; i < size; i++)
        {
            new_index = float(i) * area_left[i-1] + float(size-i) * area_right[i];

            if(new_index < best_index)
            {
                best_index = new_index;
                best_loc = i + first;
            }
        }
    }

    // Stop splitting if splitting stops being effective.
    if(best_loc < 0)
    {
        BBOX_TREE *cd = create_bbox_node(size);

        for(i = 0; i < size; i++)
            cd->Node[i] = Finite[first+i];

        calc_bbox(&(cd->BBox), Finite, first, last);
        *Root = cd;
        if(*numOfFiniteObjects >= maxfinitecount)
        {
            // Prim array overrun, increase array by 50%.
            maxfinitecount = 1.5 * maxfinitecount;

            // For debugging only.
            // TODO MESSAGE      Debug_Info("Reallocing Finite to %d\n", maxfinitecount);
            Finite = reinterpret_cast<BBOX_TREE **>(POV_REALLOC(Finite, maxfinitecount * sizeof(BBOX_TREE *), "bounding boxes"));
            delete[] *areaCache;
            *areaCache = new BBoxScalar[maxfinitecount];
        }

        Finite[*numOfFiniteObjects] = cd;
        (*numOfFiniteObjects)++;

        return false;
    }
    else
    {
        sort_and_split(Root, Finite, numOfFiniteObjects, first, best_loc, maxfinitecount, areaCache);
        sort_and_split(Root, Finite, numOfFiniteObjects, best_loc, last, maxfinitecount, areaCache);

        return true;
    }
}

}
// end of namespace pov
