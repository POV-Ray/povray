//******************************************************************************
///
/// @file core/scene/object.cpp
///
/// Basic implementations related to geometric shapes.
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
#include "core/scene/object.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <string>

// POV-Ray header files (base module)
#include "base/povassert.h"

// POV-Ray header files (core module)
#include "core/material/interior.h"
#include "core/material/texture.h"
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"
#include "core/shape/box.h"
#include "core/shape/csg.h"
#include "core/shape/sphere.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::vector;

template<int BX, int BY, int BZ>
FORCEINLINE bool Intersect_BBox_Dir(const BoundingBox& bbox, const BBoxVector3d& origin, const BBoxVector3d& invdir, BBoxScalar mind, BBoxScalar maxd);

/*****************************************************************************
 * ObjectDebugHelper class support code
 *****************************************************************************/

int ObjectDebugHelper::ObjectIndex = 0;

std::string& ObjectDebugHelper::SimpleDesc(std::string& result)
{
    char str[256];

    sprintf(str, "%u: ", Index);
    result = str;
    if(IsCopy)
        result += "Copy of ";
    if(Tag.empty())
        result += "Unnamed object";
    else
        result += Tag;

    return result;
}

/*****************************************************************************
*
* FUNCTION
*
*   Find_Intersection
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

bool Find_Intersection(Intersection *isect, ObjectPtr object, const Ray& ray, TraceThreadData *threadData)
{
    if (object != nullptr)
    {
        DBL closest = HUGE_VAL;
        BBoxVector3d origin;
        BBoxVector3d invdir;
        BBoxDirection variant;

        Vector3d tmp(1.0 / ray.GetDirection()[X], 1.0 / ray.GetDirection()[Y], 1.0 /ray.GetDirection()[Z]);
        origin = BBoxVector3d(ray.Origin);
        invdir = BBoxVector3d(tmp);
        variant = (BBoxDirection)((int(invdir[X] < 0.0) << 2) | (int(invdir[Y] < 0.0) << 1) | int(invdir[Z] < 0.0));

        if(object->Intersect_BBox(variant, origin, invdir, closest) == false)
            return false;

        if(object->Bound.empty() == false)
        {
            if(Ray_In_Bound(ray, object->Bound, threadData) == false)
                return false;
        }

        IStack depthstack(threadData->stackPool);
        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

        if(object->All_Intersections(ray, depthstack, threadData))
        {
            bool found = false;
            double tmpDepth = 0;

            while(depthstack->size() > 0)
            {
                tmpDepth = depthstack->top().Depth;
                // TODO FIXME - This was SMALL_TOLERANCE, but that's too rough for some scenes [cjc] need to check what it was in the old code [trf]
                if(tmpDepth < closest && (ray.IsSubsurfaceRay() || tmpDepth >= MIN_ISECT_DEPTH))
                {
                    *isect = depthstack->top();
                    closest = tmpDepth;
                    found = true;
                }

                depthstack->pop();
            }

            return (found == true);
        }

        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack is in a cleaned-up condition (again)
    }

    return false;
}

bool Find_Intersection(Intersection *isect, ObjectPtr object, const Ray& ray, const RayObjectCondition& postcondition, TraceThreadData *threadData)
{
    if (object != nullptr)
    {
        DBL closest = HUGE_VAL;
        BBoxVector3d origin;
        BBoxVector3d invdir;
        BBoxDirection variant;

        Vector3d tmp(1.0 / ray.GetDirection()[X], 1.0 / ray.GetDirection()[Y], 1.0 /ray.GetDirection()[Z]);
        origin = BBoxVector3d(ray.Origin);
        invdir = BBoxVector3d(tmp);
        variant = (BBoxDirection)((int(invdir[X] < 0.0) << 2) | (int(invdir[Y] < 0.0) << 1) | int(invdir[Z] < 0.0));

        if(object->Intersect_BBox(variant, origin, invdir, closest) == false)
            return false;

        if(object->Bound.empty() == false)
        {
            if(Ray_In_Bound(ray, object->Bound, threadData) == false)
                return false;
        }

        IStack depthstack(threadData->stackPool);
        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

        if(object->All_Intersections(ray, depthstack, threadData))
        {
            bool found = false;
            double tmpDepth = 0;

            while(depthstack->size() > 0)
            {
                tmpDepth = depthstack->top().Depth;
                // TODO FIXME - This was SMALL_TOLERANCE, but that's too rough for some scenes [cjc] need to check what it was in the old code [trf]
                if(tmpDepth < closest && (ray.IsSubsurfaceRay() || tmpDepth >= MIN_ISECT_DEPTH) && postcondition(ray, object, tmpDepth))
                {
                    *isect = depthstack->top();
                    closest = tmpDepth;
                    found = true;
                }

                depthstack->pop();
            }

            return (found == true);
        }

        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack is in a cleaned-up condition (again)
    }

    return false;
}

bool Find_Intersection(Intersection *isect, ObjectPtr object, const Ray& ray, BBoxDirection variant, const BBoxVector3d& origin, const BBoxVector3d& invdir, TraceThreadData *threadData)
{
    if (object != nullptr)
    {
        DBL closest = HUGE_VAL;

        if(object->Intersect_BBox(variant, origin, invdir, closest) == false)
            return false;

        if(object->Bound.empty() == false)
        {
            if(Ray_In_Bound(ray, object->Bound, threadData) == false)
                return false;
        }

        IStack depthstack(threadData->stackPool);
        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

        if(object->All_Intersections(ray, depthstack, threadData))
        {
            bool found = false;
            double tmpDepth = 0;

            while(depthstack->size() > 0)
            {
                tmpDepth = depthstack->top().Depth;
                // TODO FIXME - This was SMALL_TOLERANCE, but that's too rough for some scenes [cjc] need to check what it was in the old code [trf]
                if(tmpDepth < closest && (ray.IsSubsurfaceRay() || tmpDepth >= MIN_ISECT_DEPTH))
                {
                    *isect = depthstack->top();
                    closest = tmpDepth;
                    found = true;
                }

                depthstack->pop();
            }

            return (found == true);
        }

        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack is in a cleaned-up condition (again)
    }

    return false;
}

bool Find_Intersection(Intersection *isect, ObjectPtr object, const Ray& ray, BBoxDirection variant, const BBoxVector3d& origin, const BBoxVector3d& invdir, const RayObjectCondition& postcondition, TraceThreadData *threadData)
{
    if (object != nullptr)
    {
        DBL closest = HUGE_VAL;

        if(object->Intersect_BBox(variant, origin, invdir, closest) == false)
            return false;

        if(object->Bound.empty() == false)
        {
            if(Ray_In_Bound(ray, object->Bound, threadData) == false)
                return false;
        }

        IStack depthstack(threadData->stackPool);
        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack pulled from the pool is in a cleaned-up condition

        if(object->All_Intersections(ray, depthstack, threadData))
        {
            bool found = false;
            double tmpDepth = 0;

            while(depthstack->size() > 0)
            {
                tmpDepth = depthstack->top().Depth;
                // TODO FIXME - This was SMALL_TOLERANCE, but that's too rough for some scenes [cjc] need to check what it was in the old code [trf]
                if(tmpDepth < closest && (ray.IsSubsurfaceRay() || tmpDepth >= MIN_ISECT_DEPTH) && postcondition(ray, object, tmpDepth))
                {
                    *isect = depthstack->top();
                    closest = tmpDepth;
                    found = true;
                }

                depthstack->pop();
            }

            return (found == true);
        }

        POV_REFPOOL_ASSERT(depthstack->empty()); // verify that the IStack is in a cleaned-up condition (again)
    }

    return false;
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Object
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

bool Inside_Object (const Vector3d& IPoint, ObjectPtr Object, TraceThreadData *Thread)
{
    for (vector<ObjectPtr>::iterator Sib = Object->Clip.begin(); Sib != Object->Clip.end(); Sib++)
    {
        if(!Inside_Object(IPoint, *Sib, Thread))
            return false;
    }

    return (Object->Inside(IPoint, Thread));
}



/*****************************************************************************
*
* FUNCTION
*
*   Ray_In_Bound
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

bool Ray_In_Bound (const Ray& ray, const vector<ObjectPtr>& Bounding_Object, TraceThreadData *Thread)
{
    Intersection Local;

    for(vector<ObjectPtr>::const_iterator Bound = Bounding_Object.begin(); Bound != Bounding_Object.end(); Bound++)
    {
        Thread->Stats()[Bounding_Region_Tests]++;

        if((!Find_Intersection (&Local, *Bound, ray, Thread)) && (!Inside_Object(ray.Origin, *Bound, Thread)))
            return false;

        Thread->Stats()[Bounding_Region_Tests_Succeeded]++;
    }

    return true;
}



/*****************************************************************************
*
* FUNCTION
*
*   Point_In_Clip
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

bool Point_In_Clip (const Vector3d& IPoint, const vector<ObjectPtr>& Clip, TraceThreadData *Thread)
{
    for(vector<ObjectPtr>::const_iterator Local_Clip = Clip.begin(); Local_Clip != Clip.end(); Local_Clip++)
    {
        Thread->Stats()[Clipping_Region_Tests]++;

        if(!Inside_Object(IPoint, *Local_Clip, Thread))
            return false;

        Thread->Stats()[Clipping_Region_Tests_Succeeded]++;
    }

    return true;
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Object
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

void Translate_Object (ObjectPtr Object, const Vector3d& Vector, const TRANSFORM *Trans)
{
    if (Object == nullptr)
        return;

    for(vector<ObjectPtr>::iterator Sib = Object->Bound.begin(); Sib != Object->Bound.end(); Sib++)
    {
        Translate_Object(*Sib, Vector, Trans);
    }

    if(Object->Clip != Object->Bound)
    {
        for(vector<ObjectPtr>::iterator Sib = Object->Clip.begin(); Sib != Object->Clip.end(); Sib++)
            Translate_Object(*Sib, Vector, Trans);
    }

    /* NK 1998 added if */
    if(!Test_Flag(Object, UV_FLAG))
    {
        Transform_Textures(Object->Texture, Trans);
        Transform_Textures(Object->Interior_Texture, Trans);
    }

    if (Object->interior != nullptr)
        Object->interior->Transform(Trans);

    Object->Translate(Vector, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Object
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

void Rotate_Object (ObjectPtr Object, const Vector3d& Vector, const TRANSFORM *Trans)
{
    if (Object == nullptr)
        return;

    for(vector<ObjectPtr>::iterator Sib = Object->Bound.begin(); Sib != Object->Bound.end(); Sib++)
    {
        Rotate_Object(*Sib, Vector, Trans);
    }

    if (Object->Clip != Object->Bound)
    {
        for(vector<ObjectPtr>::iterator Sib = Object->Clip.begin(); Sib != Object->Clip.end(); Sib++)
        {
            Rotate_Object(*Sib, Vector, Trans);
        }
    }

    /* NK 1998 added if */
    if (!Test_Flag(Object, UV_FLAG))
    {
        Transform_Textures(Object->Texture, Trans);
        Transform_Textures(Object->Interior_Texture, Trans);
    }

    if (Object->interior != nullptr)
        Object->interior->Transform(Trans);

    Object->Rotate(Vector, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Object
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

void Scale_Object (ObjectPtr Object, const Vector3d& Vector, const TRANSFORM *Trans)
{
    if (Object == nullptr)
        return;

    for(vector<ObjectPtr>::iterator Sib = Object->Bound.begin(); Sib != Object->Bound.end(); Sib++)
    {
        Scale_Object(*Sib, Vector, Trans);
    }

    if (Object->Clip != Object->Bound)
    {
        for(vector<ObjectPtr>::iterator Sib = Object->Clip.begin(); Sib != Object->Clip.end(); Sib++)
            Scale_Object(*Sib, Vector, Trans);
    }

    /* NK 1998 added if */
    if (!Test_Flag(Object, UV_FLAG))
    {
        Transform_Textures(Object->Texture, Trans);
        Transform_Textures(Object->Interior_Texture, Trans);
    }

    if (Object->interior != nullptr)
        Object->interior->Transform(Trans);

    Object->Scale(Vector, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Object
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

void Transform_Object (ObjectPtr Object, const TRANSFORM *Trans)
{
    if (Object == nullptr)
        return;

    for(vector<ObjectPtr>::iterator Sib = Object->Bound.begin(); Sib != Object->Bound.end(); Sib++)
    {
        Transform_Object(*Sib, Trans);
    }

    if (Object->Clip != Object->Bound)
    {
        for(vector<ObjectPtr>::iterator Sib = Object->Clip.begin(); Sib != Object->Clip.end(); Sib++)
        {
            Transform_Object(*Sib, Trans);
        }
    }

    /* NK 1998 added if */
    if (!Test_Flag(Object, UV_FLAG))
    {
        Transform_Textures(Object->Texture, Trans);
        Transform_Textures(Object->Interior_Texture, Trans);
    }

    if (Object->interior != nullptr)
        Object->interior->Transform(Trans);

    Object->Transform(Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Object
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

ObjectPtr Copy_Object (ObjectPtr Old)
{
    ObjectPtr New;

    if (Old == nullptr)
        return nullptr;

    New = Old->Copy();

    /*
     * The following copying of OBJECT_FIELDS is redundant if Copy
     * did *New = *Old but we cannot assume it did. It is safe for
     * Copy to do *New = *Old but it should not otherwise
     * touch OBJECT_FIELDS.
     */

    New->Type    = Old->Type;
    New->Bound   = Old->Bound;
    New->Clip    = Old->Clip;
    New->BBox    = Old->BBox;
    New->Flags   = Old->Flags;

    New->Ph_Density             = Old->Ph_Density;
    New->RadiosityImportance    = Old->RadiosityImportance;
    New->RadiosityImportanceSet = Old->RadiosityImportanceSet;

    // TODO FIXME - An explanation WHY this is important would be nice [CLi]
    New->LLights.clear(); // important

    New->Texture = Copy_Textures (Old->Texture);
    New->Interior_Texture = Copy_Textures (Old->Interior_Texture);
    if (Old->interior != nullptr)
        New->interior = InteriorPtr(new Interior(*(Old->interior)));
    else
        New->interior.reset();

    // TODO: we really ought to decide whether or not it's useful to maintain
    //       the overhead of having multiple clip and bound objects ... it is
    //       after all possible for the user to use CSG and give us one object
    //       meaning we could use a plain pointer here.
    if (Old->Bound.empty() == false)
        New->Bound = Copy_Objects(Old->Bound);
    if (Old->Clip.empty() == false)
    {
        // note that in this case the objects are shared and should only be
        // destroyed the once !!! ... to be frank POV really needs a reference-
        // counted system for sharing objects with copy-on-write semantics.
        if(Old->Bound != Old->Clip)
            New->Clip = Copy_Objects(Old->Clip);
        else
            New->Clip = New->Bound;
    }

    return New;
}

vector<ObjectPtr> Copy_Objects (vector<ObjectPtr>& Src)
{
    vector<ObjectPtr> Dst;

    for(vector<ObjectPtr>::iterator it = Src.begin(); it != Src.end(); it++)
        Dst.push_back(Copy_Object(*it));
    return (Dst);
}

/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Object
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

void Destroy_Single_Object (ObjectPtr *objectPtr)
{
    ObjectPtr object = *objectPtr;

    Destroy_Textures(object->Texture);

    Destroy_Object(object->Bound);

    Destroy_Object(object->Bound);

    if(object->Bound != object->Clip)
        Destroy_Object(object->Clip);

    delete object;
}

void Destroy_Object(vector<ObjectPtr>& Objects)
{
    for(vector<ObjectPtr>::iterator Sib = Objects.begin(); Sib != Objects.end(); Sib++)
        Destroy_Object (*Sib);
    Objects.clear();
}

void Destroy_Object(ObjectPtr Object)
{
    if (Object != nullptr)
    {
        bool DestroyClip = true;
        if (!Object->Bound.empty() && !Object->Clip.empty())
            if (*Object->Bound.begin() == *Object->Clip.begin())
                DestroyClip = false;
        Destroy_Textures(Object->Texture);
        Destroy_Textures(Object->Interior_Texture);
        Destroy_Object(Object->Bound);

        if (DestroyClip)
            Destroy_Object(Object->Clip);

        if (dynamic_cast<CompoundObject *>(Object) != nullptr)
            Destroy_Object ((dynamic_cast<CompoundObject *> (Object))->children);

        delete Object;
    }
}

ObjectBase::~ObjectBase()
{
    Destroy_Transform(Trans);
}


double ObjectBase::GetPotential (const Vector3d& p, bool subtractThreshold, TraceThreadData *threaddata) const
{
    POV_SHAPE_ASSERT (false);
    return 0.0;
}

/*****************************************************************************
*
* FUNCTION
*
*   UVCoord
*
* INPUT
*
*   Object  - Pointer to blob structure
*   Inter   - Pointer to intersection
*
* OUTPUT
*
*
* RETURNS
*
* AUTHOR
*
*   Nathan Kopp
*
* DESCRIPTION
*   This is used as a default UVCoord function for objects where UVCoordinates
*   are not defined.  It instead returns the XY coordinates of the intersection.
*
* CHANGES
*
*
******************************************************************************/

void ObjectBase::UVCoord(Vector2d& Result, const Intersection *Inter) const
{
    Result[U] = Inter->IPoint[X];
    Result[V] = Inter->IPoint[Y];
}

void ObjectBase::Determine_Textures(Intersection *isect, bool hitinside, WeightedTextureVector& textures, TraceThreadData *threaddata)
{
    if ((Interior_Texture != nullptr) && (hitinside == true))
        textures.push_back(WeightedTexture(1.0, Interior_Texture));
    else if (Texture != nullptr)
        textures.push_back(WeightedTexture(1.0, Texture));
    else if (isect->Csg != nullptr)
        isect->Csg->Determine_Textures(isect, hitinside, textures, threaddata);
}

ObjectPtr ObjectBase::Invert()
{
    Invert_Flag(this, INVERTED_FLAG);
    return this;
}

ObjectPtr NonsolidObject::Invert()
{
    return this;
}

ObjectPtr CompoundObject::Invert()
{
    for(vector<ObjectPtr>::iterator Current_Sib = children.begin(); Current_Sib != children.end(); Current_Sib++)
        *Current_Sib = (*Current_Sib)->Invert();
    Invert_Flag(this, INVERTED_FLAG);
    return this;
}

bool ObjectBase::Intersect_BBox(BBoxDirection variant, const BBoxVector3d& origin, const BBoxVector3d& invdir, BBoxScalar maxd) const
{
    // TODO FIXME - This was SMALL_TOLERANCE, but that's too rough for some scenes [cjc] need to check what it was in the old code [trf]
    switch(variant)
    {
        case BBOX_DIR_X0Y0Z0: // 000
            return Intersect_BBox_Dir<0, 0, 0>(BBox, origin, invdir, MIN_ISECT_DEPTH, maxd);
        case BBOX_DIR_X0Y0Z1: // 001
            return Intersect_BBox_Dir<0, 0, 1>(BBox, origin, invdir, MIN_ISECT_DEPTH, maxd);
        case BBOX_DIR_X0Y1Z0: // 010
            return Intersect_BBox_Dir<0, 1, 0>(BBox, origin, invdir, MIN_ISECT_DEPTH, maxd);
        case BBOX_DIR_X0Y1Z1: // 011
            return Intersect_BBox_Dir<0, 1, 1>(BBox, origin, invdir, MIN_ISECT_DEPTH, maxd);
        case BBOX_DIR_X1Y0Z0: // 100
            return Intersect_BBox_Dir<1, 0, 0>(BBox, origin, invdir, MIN_ISECT_DEPTH, maxd);
        case BBOX_DIR_X1Y0Z1: // 101
            return Intersect_BBox_Dir<1, 0, 1>(BBox, origin, invdir, MIN_ISECT_DEPTH, maxd);
        case BBOX_DIR_X1Y1Z0: // 110
            return Intersect_BBox_Dir<1, 1, 0>(BBox, origin, invdir, MIN_ISECT_DEPTH, maxd);
        case BBOX_DIR_X1Y1Z1: // 111
            return Intersect_BBox_Dir<1, 1, 1>(BBox, origin, invdir, MIN_ISECT_DEPTH, maxd);
    }

    return false; // unreachable
}

bool ObjectBase::IsOpaque() const
{
    return Test_Opacity(Texture) &&
           ((Interior_Texture == nullptr) || Test_Opacity(Interior_Texture));
}

void ContainedByBox::ComputeBBox(BoundingBox& rBbox) const
{
    rBbox.lowerLeft = BBoxVector3d(corner1);
    rBbox.size      = BBoxVector3d(corner2 - corner1);
}

void ContainedBySphere::ComputeBBox(BoundingBox& rBbox) const
{
    Make_BBox(rBbox, center.x() - radius,
                     center.y() - radius,
                     center.z() - radius,
                     radius * 2,
                     radius * 2,
                     radius * 2);
}

bool ContainedByBox::Intersect(const Ray& ray, const TRANSFORM* pTrans, DBL& rDepth1, DBL& rDepth2, int& rSide1, int& rSide2) const
{
    return Box::Intersect(ray, pTrans, corner1, corner2, &rDepth1, &rDepth2, &rSide1, &rSide2);
}

bool ContainedBySphere::Intersect(const Ray& ray, const TRANSFORM* pTrans, DBL& rDepth1, DBL& rDepth2, int& rSide1, int& rSide2) const
{
    bool intersects = false;
    DBL len;
    BasicRay newRay;

    if (pTrans != nullptr)
    {
        MInvTransRay(newRay, ray, pTrans);
        len = newRay.Direction.length();
        newRay.Direction /= len;
        intersects = Sphere::Intersect(newRay, center, radius * radius, &rDepth1, &rDepth2);
        rDepth1 /= len;
        rDepth2 /= len;
    }
    else
    {
        intersects = Sphere::Intersect(ray, center, radius * radius, &rDepth1, &rDepth2);
    }

    return intersects;
}

bool ContainedByBox::Inside(const Vector3d& point) const
{
    return ((point.x() >= corner1.x()) && (point.x() <= corner2.x()) &&
            (point.y() >= corner1.y()) && (point.y() <= corner2.y()) &&
            (point.z() >= corner1.z()) && (point.z() <= corner2.z()));
}

bool ContainedBySphere::Inside(const Vector3d& point) const
{
    Vector3d originToCenter = center - point;
    DBL ocSquared = originToCenter.lengthSqr();
    return (ocSquared <= Sqr(radius));
}

void ContainedByBox::Normal(const Vector3d& point, const TRANSFORM* pTrans, int side, Vector3d& rNormal) const
{
    switch (side)
    {
        case Box::kSideHit_X0:
            rNormal = Vector3d(-1.0, 0.0, 0.0);
            break;
        case Box::kSideHit_X1:
            rNormal = Vector3d( 1.0, 0.0, 0.0);
            break;
        case Box::kSideHit_Y0:
            rNormal = Vector3d( 0.0,-1.0, 0.0);
            break;
        case Box::kSideHit_Y1:
            rNormal = Vector3d( 0.0, 1.0, 0.0);
            break;
        case Box::kSideHit_Z0:
            rNormal = Vector3d( 0.0, 0.0,-1.0);
            break;
        case Box::kSideHit_Z1:
            rNormal = Vector3d( 0.0, 0.0, 1.0);
            break;
        default:
            POV_SHAPE_ASSERT(false);
    }

    /* Transform the normal into the world space. */
    if (pTrans != nullptr)
    {
        MTransNormal(rNormal, rNormal, pTrans);

        rNormal.normalize();
    }
}

void ContainedBySphere::Normal(const Vector3d& point, const TRANSFORM* pTrans, int side, Vector3d& rNormal) const
{
    Vector3d newPoint;

    /* Transform the point into the isosurface space */
    if (pTrans != nullptr)
        MInvTransPoint(newPoint, point, pTrans);
    else
        newPoint = point;

    rNormal = (newPoint - center) / radius;

    /* Transform the normal into the world space. */
    if (pTrans != nullptr)
    {
        MTransNormal(rNormal, rNormal, pTrans);

        rNormal.normalize();
    }
}

ContainedByShape* ContainedByBox::Copy() const
{
    return new ContainedByBox(*this);
}

ContainedByShape* ContainedBySphere::Copy() const
{
    return new ContainedBySphere(*this);
}


template<int BX, int BY, int BZ>
FORCEINLINE bool Intersect_BBox_Dir(const BoundingBox& bbox, const BBoxVector3d& origin, const BBoxVector3d& invdir, BBoxScalar mind, BBoxScalar maxd)
{
    BBoxScalar tmin, tmax, tymin, tymax, tzmin, tzmax;
    BBoxVector3d bounds[2];

    Make_min_max_from_BBox(bounds[0], bounds[1], bbox);

    tmin = (bounds[BX][X] - origin[X]) * invdir[X];
    tmax = (bounds[1 - BX][X] - origin[X]) * invdir[X];
    tymin = (bounds[BY][Y] - origin[Y]) * invdir[Y];
    tymax = (bounds[1 - BY][Y] - origin[Y]) * invdir[Y];

    if((tmin > tymax) || (tymin > tmax))
        return false;

    if(tymin > tmin)
        tmin = tymin;

    if(tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[BZ][Z] - origin[Z]) * invdir[Z];
    tzmax = (bounds[1 - BZ][Z] - origin[Z]) * invdir[Z];

    if((tmin > tzmax) || (tzmin > tmax))
        return false;

    if(tzmin > tmin)
        tmin = tzmin;

    if(tzmax < tmax)
        tmax = tzmax;

    return ((tmin < maxd) && (tmax > mind));
}

}
// end of namespace pov
