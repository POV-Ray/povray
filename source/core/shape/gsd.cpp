/*******************************************************************************
 * gsd.cpp
 *
 * This module implements routines for generalised symmetric difference.
 *
 * ---------------------------------------------------------------------------
 *******************************************************************************/

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/shape/gsd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>
#include <vector>

// POV-Ray header files (base module)
#include "base/povassert.h"

// POV-Ray header files (core module)
#include "core/bounding/boundingbox.h"
#include "core/lighting/lightgroup.h"
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"
#include "core/shape/heightfield.h"
#include "core/shape/plane.h"
#include "core/shape/quadric.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::vector;
using std::max;

void GSDInterUnion::Translate(const Vector3d& Vector, const TRANSFORM *tr)
{
	for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
		Translate_Object (*Current_Sib, Vector, tr) ;

	Recompute_BBox(&BBox, tr);
}


void GSDInterUnion::Rotate(const Vector3d& Vector, const TRANSFORM *tr)
{
	for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
		Rotate_Object (*Current_Sib, Vector, tr) ;

	Recompute_BBox(&BBox, tr);
}



void GSDInterUnion::Transform(const TRANSFORM *tr)
{
	for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
		Transform_Object(*Current_Sib, tr);

	Recompute_BBox(&BBox, tr);
}

void GSDInterUnion::Scale(const Vector3d& Vector, const TRANSFORM *tr)
{
	for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
		Scale_Object (*Current_Sib, Vector, tr) ;

	Recompute_BBox(&BBox, tr);
}

ObjectPtr GSDInterUnion::Invert()
{
  selected.flip();
  return this;
}

GSDInterUnion::GSDInterUnion():CompoundObject(IS_COMPOUND_OBJECT)
{
}

GSDInterMerge::GSDInterMerge():GSDInterUnion()
{
}

ObjectPtr GSDInterUnion::Copy()
{
  GSDInterUnion *New = new GSDInterUnion();
  Destroy_Transform(New->Trans);
  *New = *this;

	New->children.clear();
	New->children.reserve(children.size());
	for(vector<ObjectPtr>::iterator i(children.begin()); i != children.end(); i++)
		New->children.push_back(Copy_Object(*i));

	return (New);
}
ObjectPtr GSDInterMerge::Copy()
{
  GSDInterMerge *New = new GSDInterMerge();
  Destroy_Transform(New->Trans);
  *New = *this;

	New->children.clear();
	New->children.reserve(children.size());
	for(vector<ObjectPtr>::iterator i(children.begin()); i != children.end(); i++)
		New->children.push_back(Copy_Object(*i));

	return (New);
}

void GSDInterUnion::Determine_Textures(Intersection *isect, bool hitinside, WeightedTextureVector& textures, TraceThreadData *threaddata)
{
	if(!children.empty())
	{
		size_t firstinserted = textures.size();

		for(vector<ObjectPtr>::const_iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
		{
			if((*Current_Sib)->Inside(isect->IPoint, threaddata))
			{
				if((*Current_Sib)->Type & IS_COMPOUND_OBJECT)
					(*Current_Sib)->Determine_Textures(isect, hitinside, textures, threaddata);
				else if((*Current_Sib)->Texture != NULL)
					textures.push_back(WeightedTexture(1.0, (*Current_Sib)->Texture));
			}
		}

		COLC weight = 1.0f / max(COLC(textures.size() - firstinserted), 1.0f);

		for(size_t i = firstinserted; i < textures.size(); i++)
			textures[i].weight = weight;
	}
}

bool GSDInterUnion::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
	size_t Count;
  bool Found;
	IStack Local_Stack(Thread->stackPool);
	assert(Local_Stack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

	Thread->Stats()[Ray_GSD_Interunion_Tests]++;

	Found = false;

	for(vector<ObjectPtr>::const_iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
	{
		if ((*Current_Sib)->Bound.empty() == true || Ray_In_Bound(ray, (*Current_Sib)->Bound, Thread))
		{
			if((*Current_Sib)->All_Intersections(ray, Local_Stack, Thread))
			{
				while(Local_Stack->size() > 0)
				{
					Count = 1;

					for(vector<ObjectPtr>::const_iterator Inside_Sib = children.begin(); Inside_Sib != children.end(); Inside_Sib++)
					{
						if(*Inside_Sib != *Current_Sib)
						{
							if(Inside_Object(Local_Stack->top().IPoint, *Inside_Sib, Thread))
							{
								++Count;
							}
						}
					}

					if(selected[Count])
					{
						if(Clip.empty() || Point_In_Clip(Local_Stack->top().IPoint, Clip, Thread))
						{
							Local_Stack->top().Csg = this;

							Depth_Stack->push(Local_Stack->top());

							Found = true;
						}
					}

					Local_Stack->pop();
				}
			}
		}
	}

	if(Found)
		Thread->Stats()[Ray_GSD_Interunion_Tests_Succeeded]++;

	assert(Local_Stack->empty()); // verify that the IStack is in a cleaned-up condition (again)
	return (Found);
}

bool GSDInterMerge::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
	size_t Count;
  bool Found;
	IStack Local_Stack(Thread->stackPool);
	assert(Local_Stack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

	Thread->Stats()[Ray_GSD_Interunion_Tests]++;

	Found = false;

	for(vector<ObjectPtr>::const_iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
	{
		if ((*Current_Sib)->Bound.empty() == true || Ray_In_Bound(ray, (*Current_Sib)->Bound, Thread))
		{
			if((*Current_Sib)->All_Intersections(ray, Local_Stack, Thread))
			{
				while(Local_Stack->size() > 0)
				{
					Count = 1;

					for(vector<ObjectPtr>::const_iterator Inside_Sib = children.begin(); Inside_Sib != children.end(); Inside_Sib++)
					{
						if(*Inside_Sib != *Current_Sib)
						{
							if(Inside_Object(Local_Stack->top().IPoint, *Inside_Sib, Thread))
							{
								++Count;
							}
						}
					}
					// merge: keep only the intersection on border
					if(
							(selected[Count])
							&& ( ( (Count>0) && !selected[Count-1])
								||( (Count<(selected.size()-1)) && !selected[Count+1])
								)
						)
					{
						if(Clip.empty() || Point_In_Clip(Local_Stack->top().IPoint, Clip, Thread))
						{
							Local_Stack->top().Csg = this;

							Depth_Stack->push(Local_Stack->top());

							Found = true;
						}
					}

					Local_Stack->pop();
				}
			}
		}
	}

	if(Found)
		Thread->Stats()[Ray_GSD_Interunion_Tests_Succeeded]++;

	assert(Local_Stack->empty()); // verify that the IStack is in a cleaned-up condition (again)
	return (Found);
}

bool GSDInterUnion::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
  size_t Count=0;
	for(vector<ObjectPtr>::const_iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
		if(Inside_Object(IPoint, (*Current_Sib), Thread))
		{
			++Count;
		}
	return selected[Count];
}


void GSDInterUnion::Compute_BBox()
{
	DBL Old_Volume, New_Volume;
	Vector3d NewMin, NewMax, TmpMin, TmpMax, Min, Max;

  NewMin = Vector3d(BOUND_HUGE);
  NewMax = Vector3d(-BOUND_HUGE);

	for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
	{
		Make_min_max_from_BBox(TmpMin, TmpMax, (*Current_Sib)->BBox);

    NewMin = min(NewMin, TmpMin);
    NewMax = max(NewMax, TmpMax);
	}

	if((NewMin[X] > NewMax[X]) || (NewMin[Y] > NewMax[Y]) || (NewMin[Z] > NewMax[Z]))
		;// TODO MESSAGE    Warning(0, "Degenerate GSD bounding box (not used!).");
	else
	{
		New_Volume = (NewMax[X] - NewMin[X]) * (NewMax[Y] - NewMin[Y]) * (NewMax[Z] - NewMin[Z]);

		BOUNDS_VOLUME(Old_Volume, BBox);

		if(New_Volume < Old_Volume)
		{
			Make_BBox_from_min_max(BBox, NewMin, NewMax);

			/* Beware of bounding boxes too large. */

      if((BBox.size[X] > CRITICAL_LENGTH) ||
          (BBox.size[Y] > CRITICAL_LENGTH) ||
          (BBox.size[Z] > CRITICAL_LENGTH))
				Make_BBox(BBox, -BOUND_HUGE/2, -BOUND_HUGE/2, -BOUND_HUGE/2, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
		}
	}
}

void GSDInterUnion::prepare( unsigned c)
{
    selected.resize( c+1, false);
}

void GSDInterUnion::setAsInside( unsigned v)
{
    selected[v] = true;
}

}
