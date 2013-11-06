/*******************************************************************************
 * bbox.h
 *
 * This module contains all defines, typedefs, and prototypes for BBOX.CPP.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/bounding/bbox.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

/* NOTE: FRAME.H contains other bound stuff. */

#ifndef BBOX_H
#define BBOX_H

#include <map>

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

typedef int VECTORI[3];

class Rayinfo
{
	public:
		BBOX_VECT slab_num;
		BBOX_VECT slab_den;
		VECTORI nonzero;
		VECTORI positive;

		explicit Rayinfo(const Ray& ray)
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

/*****************************************************************************
* Global functions
******************************************************************************/

struct PriorityQueue
{
	struct Qelem
	{
		DBL depth;
		const BBOX_TREE *node;
	};

	unsigned QSize;
	unsigned Max_QSize;
	Qelem *Queue;

	PriorityQueue();
	~PriorityQueue();
};

void Build_BBox_Tree(BBOX_TREE **Root, size_t numOfFiniteObjects, BBOX_TREE **&Finite, size_t numOfInfiniteObjects, BBOX_TREE **Infinite, size_t& maxfinitecount);
void Build_Bounding_Slabs(BBOX_TREE **Root, vector<ObjectPtr>& objects, unsigned int& numberOfFiniteObjects, unsigned int& numberOfInfiniteObjects, unsigned int& numberOfLightSources);

void Recompute_BBox(BBOX *bbox, const TRANSFORM *trans);
void Recompute_Inverse_BBox(BBOX *bbox, const TRANSFORM *trans);
bool Intersect_BBox_Tree(PriorityQueue& pqueue, const BBOX_TREE *Root, const Ray& ray, Intersection *Best_Intersection, TraceThreadData *Thread);
bool Intersect_BBox_Tree(PriorityQueue& pqueue, const BBOX_TREE *Root, const Ray& ray, Intersection *Best_Intersection, const RayObjectCondition& precondition, const RayObjectCondition& postcondition, TraceThreadData *Thread);
void Check_And_Enqueue(PriorityQueue& Queue, const BBOX_TREE *Node, const BBOX *BBox, const Rayinfo *rayinfo, TraceThreadData *Thread);
void Priority_Queue_Delete(PriorityQueue& Queue, DBL *key, const BBOX_TREE **Node);
void Destroy_BBox_Tree(BBOX_TREE *Node);


/*****************************************************************************
* Inline functions
******************************************************************************/

// Calculate the volume of a bounding box. [DB 8/94]
inline void BOUNDS_VOLUME(DBL& a, const BBOX& b)
{
	a = b.Lengths[X] * b.Lengths[Y] * b.Lengths[Z];
}

}

#endif
