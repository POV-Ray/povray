//******************************************************************************
///
/// @file core/shape/csg.cpp
///
/// Implementation of constructive solid geometry (csg) shapes.
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
#include "core/shape/csg.h"

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

using std::min;
using std::max;
using std::vector;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define UNION_OBJECT        (IS_COMPOUND_OBJECT | IS_CSG_OBJECT)
#define MERGE_OBJECT        (IS_COMPOUND_OBJECT | IS_CSG_OBJECT)
#define INTERSECTION_OBJECT (IS_COMPOUND_OBJECT | IS_CSG_OBJECT)



inline bool Test_Ray_Flags(const Ray& ray, ConstObjectPtr obj)
{
    // CJC 2005 if ray is primary ray ignore NO_IMAGE_FLAG to support the trace() SDL function
    // TODO FIXME - I guess it would be better to have the trace() function use a different ray type [CLi]
    return ( ( !ray.IsPhotonRay() &&
               (!Test_Flag(obj, NO_IMAGE_FLAG) || ray.IsImageRay() == false || ray.IsPrimaryRay() == true) &&
               (!Test_Flag(obj, NO_REFLECTION_FLAG) || ray.IsReflectionRay() == false) &&
               (!Test_Flag(obj, NO_RADIOSITY_FLAG) || ray.IsRadiosityRay() == false) ) ||
             ( ray.IsPhotonRay() && !Test_Flag(obj, NO_SHADOW_FLAG) ) );
}

inline bool Test_Ray_Flags_Shadow(const Ray& ray, ConstObjectPtr obj)
{
    // TODO CLARIFY - why does this function not ignore NO_IMAGE_FLAG for primary rays, as Test_Ray_Flags() does? [CLi]
    return ( ( !ray.IsPhotonRay() &&
               (!Test_Flag(obj, NO_IMAGE_FLAG) || ray.IsImageRay() == false) &&
               (!Test_Flag(obj, NO_REFLECTION_FLAG) || ray.IsReflectionRay() == false) &&
               (!Test_Flag(obj, NO_RADIOSITY_FLAG) || ray.IsRadiosityRay() == false) ) ||
             ( ray.IsPhotonRay() && !Test_Flag(obj, NO_SHADOW_FLAG) ) ||
             ( ray.IsShadowTestRay() && !Test_Flag(obj, NO_SHADOW_FLAG) ) );
}

/*****************************************************************************
*
* FUNCTION
*
*   All_CSG_Union_Intersections
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 1994 : Added code to count intersection tests. [DB]
*
******************************************************************************/

bool CSGUnion::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int Found;

    Thread->Stats()[Ray_CSG_Union_Tests]++;

    Found = false;

    // Use shortcut if no clip.

    if(Clip.empty())
    {
        for(vector<ObjectPtr>::const_iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
        {
            if(Test_Ray_Flags(ray, (*Current_Sib))) // TODO CLARIFY - why does CSGUnion use Test_Ray_Flags(), while CSGMerge uses Test_Ray_Flags_Shadow(), and CSGIntersection uses neither?
            {
                if((*Current_Sib)->Bound.empty() == true || Ray_In_Bound(ray, (*Current_Sib)->Bound, Thread))
                {
                    if((*Current_Sib)->All_Intersections(ray, Depth_Stack, Thread))
                        Found = true;
                }
            }
        }
    }
    else
    {
        IStack Local_Stack(Thread->stackPool);
        POV_REFPOOL_ASSERT(Local_Stack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

        for(vector<ObjectPtr>::const_iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
        {
            if(Test_Ray_Flags(ray, (*Current_Sib))) // TODO CLARIFY - why does CSGUnion use Test_Ray_Flags(), while CSGMerge uses Test_Ray_Flags_Shadow(), and CSGIntersection uses neither?
            {
                if((*Current_Sib)->Bound.empty() == true || Ray_In_Bound(ray, (*Current_Sib)->Bound, Thread))
                {
                    if((*Current_Sib)->All_Intersections (ray, Local_Stack, Thread))
                    {
                        while(Local_Stack->size() > 0)
                        {
                            if(Clip.empty() || Point_In_Clip(Local_Stack->top().IPoint, Clip, Thread))
                            {
                                Local_Stack->top().Csg = this;

                                Depth_Stack->push(Local_Stack->top());

                                Found = true;
                            }

                            Local_Stack->pop();
                        }
                    }
                }
            }
        }
        POV_REFPOOL_ASSERT(Local_Stack->empty()); // verify that the IStack is in a cleaned-up condition (again)
    }

    if(Found)
        Thread->Stats()[Ray_CSG_Union_Tests_Succeeded]++;

    return (Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   All_CSG_Intersection_Intersections
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 1994 : Added code to count intersection tests. [DB]
*
******************************************************************************/

bool CSGIntersection::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int Maybe_Found, Found;
    IStack Local_Stack(Thread->stackPool);
    POV_REFPOOL_ASSERT(Local_Stack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

    Thread->Stats()[Ray_CSG_Intersection_Tests]++;

    Found = false;

    for(vector<ObjectPtr>::const_iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
    {
        if ((*Current_Sib)->Bound.empty() == true || Ray_In_Bound(ray, (*Current_Sib)->Bound, Thread))
        {
            if((*Current_Sib)->All_Intersections(ray, Local_Stack, Thread))
            {
                while(Local_Stack->size() > 0)
                {
                    Maybe_Found = true;

                    for(vector<ObjectPtr>::const_iterator Inside_Sib = children.begin(); Inside_Sib != children.end(); Inside_Sib++)
                    {
                        if(*Inside_Sib != *Current_Sib)
                        {
                            if(!((*Inside_Sib)->Type & LIGHT_SOURCE_OBJECT) || (!(reinterpret_cast<LightSource *>(*Inside_Sib))->children.empty()))
                            {
                                if(!Inside_Object(Local_Stack->top().IPoint, *Inside_Sib, Thread))
                                {
                                    Maybe_Found = false;
                                    break;
                                }
                            }
                        }
                    }

                    if(Maybe_Found)
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
        Thread->Stats()[Ray_CSG_Intersection_Tests_Succeeded]++;

    POV_REFPOOL_ASSERT(Local_Stack->empty()); // verify that the IStack is in a cleaned-up condition (again)
    return (Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   All_CSG_Merge_Intersections
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 1994 : Added code to count intersection tests. [DB]
*
******************************************************************************/

bool CSGMerge::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int Found;
    bool inside_flag;
    IStack Local_Stack(Thread->stackPool);
    POV_REFPOOL_ASSERT(Local_Stack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

    Thread->Stats()[Ray_CSG_Merge_Tests]++;

    Found = false;

    // FIXME - though the name is misleading, the OPTIMISE_SHADOW_TEST flag can be used to
    //  determine if we're in a shadow ray, but it SHOULD be renamed.
    // We should probably change Optimization_Flags to a "ray-type" variable, that will tell
    // us if it is primary, reflection, refraction, shadow, primary photon, photon refleciton, or photon refraction ray.
    int shadow_flag = ray.IsShadowTestRay(); // TODO FIXME - why is this flag not used?!

    for(vector<ObjectPtr>::const_iterator Sib1 = children.begin(); Sib1 != children.end(); Sib1++)
    {
        if ( Test_Ray_Flags_Shadow(ray, (*Sib1)) )// TODO CLARIFY - why does CSGUnion use Test_Ray_Flags(), while CSGMerge uses Test_Ray_Flags_Shadow(), and CSGIntersection uses neither?
        {
            if ((*Sib1)->Bound.empty() == true || Ray_In_Bound (ray, (*Sib1)->Bound, Thread))
            {
                if ((*Sib1)->All_Intersections (ray, Local_Stack, Thread))
                {
                    while (Local_Stack->size() > 0)
                    {
                        if (Clip.empty() || Point_In_Clip(Local_Stack->top().IPoint, Clip, Thread))
                        {
                            inside_flag = true;

                            for(vector<ObjectPtr>::const_iterator Sib2 = children.begin(); (Sib2 != children.end()) && (inside_flag == true); Sib2++)
                            {
                                if (*Sib1 != *Sib2)
                                {
                                    if (!((*Sib2)->Type & LIGHT_SOURCE_OBJECT) || (!(reinterpret_cast<LightSource *>(*Sib2))->children.empty()))
                                    {
                                        if ( Test_Ray_Flags_Shadow(ray, (*Sib2)) )// TODO CLARIFY - why does CSGUnion use Test_Ray_Flags(), while CSGMerge uses Test_Ray_Flags_Shadow(), and CSGIntersection uses neither?
                                        {
                                            if (Inside_Object(Local_Stack->top().IPoint, *Sib2, Thread))
                                                inside_flag = false;
                                        }
                                    }
                                }
                            }

                            if (inside_flag == true)
                            {
                                Local_Stack->top().Csg = this;

                                Found = true;

                                Depth_Stack->push(Local_Stack->top());
                            }
                        }

                        Local_Stack->pop();
                    }
                }
            }
        }
    }

    if (Found)
        Thread->Stats()[Ray_CSG_Merge_Tests_Succeeded]++;

    POV_REFPOOL_ASSERT(Local_Stack->empty()); // verify that the IStack is in a cleaned-up condition (again)
    return (Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_CSG_Union
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool CSGUnion::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    for(vector<ObjectPtr>::const_iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
    {
        if(!((*Current_Sib)->Type & LIGHT_SOURCE_OBJECT) || (!(reinterpret_cast<LightSource *>(*Current_Sib))->children.empty()))
        {
            if(Inside_Object(IPoint, *Current_Sib, Thread))
                return (true);
        }
    }

    return (false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_CSG_Intersection
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool CSGIntersection::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    for(vector<ObjectPtr>::const_iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
        if(!((*Current_Sib)->Type & LIGHT_SOURCE_OBJECT) || (!(reinterpret_cast<LightSource *>(*Current_Sib))->children.empty()))
            if(!Inside_Object(IPoint, (*Current_Sib), Thread))
                return (false);
    return (true);
}




/*****************************************************************************
*
* FUNCTION
*
*   Translate_CSG
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/


void CSG::Translate(const Vector3d& Vector, const TRANSFORM *tr)
{
    for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
        Translate_Object (*Current_Sib, Vector, tr) ;

    Recompute_BBox(&BBox, tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_CSG
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/


void CSG::Rotate(const Vector3d& Vector, const TRANSFORM *tr)
{
    for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
        Rotate_Object (*Current_Sib, Vector, tr) ;

    Recompute_BBox(&BBox, tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_CSG
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/


void CSG::Scale(const Vector3d& Vector, const TRANSFORM *tr)
{
    for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
        Scale_Object (*Current_Sib, Vector, tr) ;

    Recompute_BBox(&BBox, tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_CSG
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/


void CSG::Transform(const TRANSFORM *tr)
{
    for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
        Transform_Object(*Current_Sib, tr);

    Recompute_BBox(&BBox, tr);
}

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

ObjectPtr CSGUnion::Invert()
{
    ObjectPtr p = CompoundObject::Invert();
    POV_SHAPE_ASSERT(p == this);

    CSGIntersection *New = new CSGIntersection(false, *this, true);
    delete this;
    return (New);
}

ObjectPtr CSGIntersection::Invert()
{
    ObjectPtr p = CompoundObject::Invert();
    POV_SHAPE_ASSERT(p == this);

    CSGMerge *New = new CSGMerge(*this, true);
    delete this;
    return (New);
}

/*****************************************************************************
*
* FUNCTION
*
*   Create_CSG_Union
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   2000 : NK phmap
*
******************************************************************************/

CSGUnion::CSGUnion() : CSG(UNION_OBJECT)
{
    do_split = true;
}

CSGUnion::CSGUnion(int t) : CSG(t)
{
    do_split = true;
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_CSG_Merge
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

CSGMerge::CSGMerge() : CSGUnion(MERGE_OBJECT)
{
}

CSGMerge::CSGMerge(CompoundObject& o, bool transplant) : CSGUnion(MERGE_OBJECT, o, transplant)
{
}

/*****************************************************************************
*
* FUNCTION
*
*   Create_CSG_Intersection
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

CSGIntersection::CSGIntersection(bool diff) : CSG(INTERSECTION_OBJECT), isDifference(diff)
{
    do_split = false; // TODO - not necessary but makes debugging clearer
}

CSGIntersection::CSGIntersection(bool diff, CompoundObject& o, bool transplant) : CSG(INTERSECTION_OBJECT, o, transplant), isDifference(diff)
{
    do_split = false; // TODO - not necessary but makes debugging clearer
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_CSG
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/


ObjectPtr CSGUnion::Copy()
{
    CSGUnion *New = new CSGUnion();
    Destroy_Transform(New->Trans);
    *New = *this;

    New->children.clear();
    New->children.reserve(children.size());
    for(vector<ObjectPtr>::iterator i(children.begin()); i != children.end(); i++)
        New->children.push_back(Copy_Object(*i));

    if(Type & LIGHT_GROUP_OBJECT)
    {
        New->LLights.clear();
        Promote_Local_Lights(New);
    }

    return (New);
}

ObjectPtr CSGMerge::Copy()
{
    CSGMerge *New = new CSGMerge();
    Destroy_Transform(New->Trans);
    *New = *this;

    New->children.clear();
    New->children.reserve(children.size());
    for(vector<ObjectPtr>::iterator i(children.begin()); i != children.end(); i++)
        New->children.push_back(Copy_Object(*i));

    if(Type & LIGHT_GROUP_OBJECT)
    {
        New->LLights.clear();
        Promote_Local_Lights(New);
    }

    return (New);
}

ObjectPtr CSGIntersection::Copy()
{
    CSGIntersection *New = new CSGIntersection(false);
    Destroy_Transform(New->Trans);
    *New = *this;

    New->children.clear();
    New->children.reserve(children.size());
    for(vector<ObjectPtr>::iterator i(children.begin()); i != children.end(); i++)
        New->children.push_back(Copy_Object(*i));

    if(Type & LIGHT_GROUP_OBJECT)
    {
        New->LLights.clear();
        Promote_Local_Lights(New);
    }

    return (New);
}

/*****************************************************************************
*
* FUNCTION
*
*   Compute_CSG_BBox
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 1994 : Improved bounding of quadrics used in CSG intersections. [DB]
*
******************************************************************************/

void CSG::Compute_BBox()
{
    DBL Old_Volume, New_Volume;
    Vector3d NewMin, NewMax, TmpMin, TmpMax, Min, Max;

    if (dynamic_cast<CSGIntersection *>(this) != nullptr) // FIXME
    {
        /*
         * Calculate the bounding box of a CSG intersection
         * by intersecting the bounding boxes of all children.
         */

        NewMin = Vector3d(-BOUND_HUGE);
        NewMax = Vector3d(BOUND_HUGE);

        vector<Quadric *> Quadrics;

        /* Process all children. */

        for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
        {
            /* Inverted objects and height fields mustn't be considered */

            if (!Test_Flag((*Current_Sib), INVERTED_FLAG) && (dynamic_cast<HField *>(*Current_Sib) == nullptr)) // FIXME
            {
                /* We store quadrics since they'll be processed last, to benefit from confining them to a certain range */
                if (dynamic_cast<Quadric *>(*Current_Sib) == nullptr) // FIXME
                {
                    if (dynamic_cast<Plane *>(*Current_Sib) != nullptr) // FIXME
                        Quadric::Compute_Plane_Min_Max(dynamic_cast<Plane *>(*Current_Sib), TmpMin, TmpMax);
                    else
                        Make_min_max_from_BBox(TmpMin, TmpMax, (*Current_Sib)->BBox);

                    NewMin = max(NewMin, TmpMin);
                    NewMax = min(NewMax, TmpMax);
                }
                else
                    Quadrics.push_back(dynamic_cast<Quadric *>(*Current_Sib));
            }
        }

        /* Process any quadrics. */

        for(vector<Quadric *>::iterator i = Quadrics.begin(); i != Quadrics.end(); i++)
        {
            Quadric *q = *i;

            Min = NewMin;
            Max = NewMax;

            q->Compute_BBox(Min, Max);

            Make_min_max_from_BBox(TmpMin, TmpMax, q->BBox);

            NewMin = max(NewMin, TmpMin);
            NewMax = min(NewMax, TmpMax);
        }
    }
    else
    {
        /* Calculate the bounding box of a CSG merge/union object. */

        NewMin = Vector3d(BOUND_HUGE);
        NewMax = Vector3d(-BOUND_HUGE);

        for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
        {
            Make_min_max_from_BBox(TmpMin, TmpMax, (*Current_Sib)->BBox);

            NewMin = min(NewMin, TmpMin);
            NewMax = max(NewMax, TmpMax);
        }
    }

    if((NewMin[X] > NewMax[X]) || (NewMin[Y] > NewMax[Y]) || (NewMin[Z] > NewMax[Z]))
        ;// TODO MESSAGE    Warning("Degenerate CSG bounding box (not used!).");
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


/*****************************************************************************
*
* FUNCTION
*
*   Determine_CSG_Textures
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void CSG::Determine_Textures(Intersection *isect, bool hitinside, WeightedTextureVector& textures, TraceThreadData *threaddata)
{
    if(!children.empty())
    {
        if(Type & CSG_DIFFERENCE_OBJECT)
        {
            // For CSG Differences, use only the first object in the chain
            // (which is the first object in the POV file.  All other objects
            // are the ones that were "removed" from the first one, so their
            // textures should NOT be used.
            if(children[0]->Inside(isect->IPoint, threaddata))
            {
                if(children[0]->Type & IS_COMPOUND_OBJECT)
                    children[0]->Determine_Textures(isect, hitinside, textures, threaddata);
                else if (children[0]->Texture != nullptr)
                    textures.push_back(WeightedTexture(1.0, children[0]->Texture));
            }
        }
        else
        {
            size_t firstinserted = textures.size();

            for(vector<ObjectPtr>::const_iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
            {
                if((*Current_Sib)->Inside(isect->IPoint, threaddata))
                {
                    if((*Current_Sib)->Type & IS_COMPOUND_OBJECT)
                        (*Current_Sib)->Determine_Textures(isect, hitinside, textures, threaddata);
                    else if ((*Current_Sib)->Texture != nullptr)
                        textures.push_back(WeightedTexture(1.0, (*Current_Sib)->Texture));
                }
            }

            COLC weight = 1.0f / max(COLC(textures.size() - firstinserted), 1.0f);

            for(size_t i = firstinserted; i < textures.size(); i++)
                textures[i].weight = weight;
        }
    }
}

}
// end of namespace pov
