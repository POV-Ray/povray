/*******************************************************************************
 * objects.h
 *
 * This module contains all defines, typedefs, and prototypes for OBJECTS.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/scene/objects.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

/* NOTE: FRAME.H contains other object stuff. */

#ifndef OBJECTS_H
#define OBJECTS_H

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/*
 * [DB 7/94]
 *
 * The flag field is used to store all possible flags that are
 * used for objects (up to 32).
 *
 * The flages are manipulated using the following macros:
 *
 *   Set_Flag    (Object, Flag) : set    specified Flag in Object
 *   Clear_Flag  (Object, Flag) : clear  specified Flag in Object
 *   Invert_Flag (Object, Flag) : invert specified Flag in Object
 *   Test_Flag   (Object, Flag) : test   specified Flag in Object
 *
 *   Copy_Flag   (Object1, Object2, Flag) : Set the Flag in Object1 to the
 *                                          value of the Flag in Object2.
 *   Bool_Flag   (Object, Flag, Bool)     : if(Bool) Set flag else Clear flag
 *
 * Object is a pointer to the object.
 * Flag is the number of the flag to test.
 *
 */

#define NO_SHADOW_FLAG            0x00000001L /* Object doesn't cast shadows            */
#define CLOSED_FLAG               0x00000002L /* Object is closed                       */
#define INVERTED_FLAG             0x00000004L /* Object is inverted                     */
#define SMOOTHED_FLAG             0x00000008L /* Object is smoothed                     */
#define CYLINDER_FLAG             0x00000010L /* Object is a cylinder                   */
#define DEGENERATE_FLAG           0x00000020L /* Object is degenerate                   */
#define STURM_FLAG                0x00000040L /* Object should use sturmian root solver */
#define OPAQUE_FLAG               0x00000080L /* Object is opaque                       */
#define MULTITEXTURE_FLAG         0x00000100L /* Object is multi-textured primitive     */
#define INFINITE_FLAG             0x00000200L /* Object is infinite                     */
#define HIERARCHY_FLAG            0x00000400L /* Object can have a bounding hierarchy   */
#define HOLLOW_FLAG               0x00000800L /* Object is hollow (atmosphere inside)   */
#define HOLLOW_SET_FLAG           0x00001000L /* Hollow explicitly set in scene file    */
#define UV_FLAG                   0x00002000L /* Object uses UV mapping                 */
#define DOUBLE_ILLUMINATE_FLAG    0x00004000L /* Illuminate both sides of the surface   */
#define NO_IMAGE_FLAG             0x00008000L /* Object doesn't catch camera rays     [ENB 9/97] */
#define NO_REFLECTION_FLAG        0x00010000L /* Object doesn't catch reflection rays [ENB 9/97] */
#define NO_GLOBAL_LIGHTS_FLAG     0x00020000L /* Object doesn't receive light from global lights */
#define NO_GLOBAL_LIGHTS_SET_FLAG 0x00040000L /* Object doesn't receive light from global lights explicitly set in scene file */
/* Photon-related flags */
#define PH_TARGET_FLAG            0x00080000L /* this object is a photons target */
#define PH_PASSTHRU_FLAG          0x00100000L /* this is pass through object (i.e. it may let photons pass on their way to the target) */
#define PH_RFL_ON_FLAG            0x00200000L /* this object explicitly reflects photons */
#define PH_RFL_OFF_FLAG           0x00400000L /* this object explicitly does not reflect photons */
#define PH_RFR_ON_FLAG            0x00800000L /* this object explicitly refracts photons */
#define PH_RFR_OFF_FLAG           0x01000000L /* this object explicitly does not refract photons */
#define PH_IGNORE_PHOTONS_FLAG    0x02000000L /* this object does not collect photons */
#define IGNORE_RADIOSITY_FLAG     0x04000000L /* Object doesn't receive ambient light from radiosity */
#define NO_RADIOSITY_FLAG         0x08000000L /* Object doesn't catch radiosity rays (i.e. is invisible to radiosity) */
#define CUTAWAY_TEXTURES_FLAG     0x10000000L /* Object (or any of its parents) has cutaway_textures set */



#define Set_Flag(Object, Flag)     \
	{ (Object)->Flags |=  (Flag); }

#define Clear_Flag(Object, Flag)   \
	{ (Object)->Flags &= ~(Flag); }

#define Invert_Flag(Object, Flag)  \
	{ (Object)->Flags ^=  (Flag); }

#define Test_Flag(Object, Flag)    \
	( (Object)->Flags & (Flag))

#define Copy_Flag(Object1, Object2, Flag) \
	{ (Object1)->Flags = (((Object1)->Flags) & (~Flag)) | \
	                     (((Object2)->Flags) &  (Flag)); }

#define Bool_Flag(Object, Flag, Bool) \
	{ if(Bool){ (Object)->Flags |=  (Flag); } else { (Object)->Flags &= ~(Flag); }}



/* Object types. */

#define BASIC_OBJECT                0
#define PATCH_OBJECT                1 /* Has no inside, no inverse */
#define TEXTURED_OBJECT             2 /* Has texture, possibly in children */
#define IS_COMPOUND_OBJECT          4 /* Has children field */
#define STURM_OK_OBJECT             8 /* STURM legal */
//#define WATER_LEVEL_OK_OBJECT      16 /* WATER_LEVEL legal */
#define LIGHT_SOURCE_OBJECT        32 /* link me in frame.light_sources */
#define BOUNDING_OBJECT            64 /* This is a holder for bounded object */
//#define SMOOTH_OK_OBJECT          128 /* SMOOTH legal */
#define IS_CHILD_OBJECT           256 /* Object is inside a COMPOUND */
/* NK 1998 - DOUBLE_ILLUMINATE is not used anymore - use DOUBLE_ILLUMINATE_FLAG */
#define HIERARCHY_OK_OBJECT       512 /* NO_HIERARCHY legal */
#define LT_SRC_UNION_OBJECT      1024 /* Union of light_source objects only */
#define LIGHT_GROUP_OBJECT       2048 /* light_group union object [trf] */
#define LIGHT_GROUP_LIGHT_OBJECT 4096 /* light in light_group object [trf] */
#define CSG_DIFFERENCE_OBJECT    8192 /* csg difference object */
#define IS_CSG_OBJECT           16384 /* object is a csg and not some other compound object */
#define CHILDREN_FLAGS (PATCH_OBJECT+TEXTURED_OBJECT)  /* Reverse inherited flags */


/*****************************************************************************
* Global functions
******************************************************************************/

bool Find_Intersection(Intersection *Ray_Intersection, ObjectPtr Object, const Ray& ray, TraceThreadData *Thread);
bool Find_Intersection(Intersection *Ray_Intersection, ObjectPtr Object, const Ray& ray, const RayObjectCondition& postcondition, TraceThreadData *Thread);
bool Find_Intersection(Intersection *isect, ObjectPtr object, const Ray& ray, ObjectBase::BBoxDirection variant, const BBOX_VECT& origin, const BBOX_VECT& invdir, TraceThreadData *ThreadData);
bool Find_Intersection(Intersection *isect, ObjectPtr object, const Ray& ray, ObjectBase::BBoxDirection variant, const BBOX_VECT& origin, const BBOX_VECT& invdir, const RayObjectCondition& postcondition, TraceThreadData *ThreadData);
bool Ray_In_Bound(const Ray& ray, const vector<ObjectPtr>& Bounding_Object, TraceThreadData *Thread);
bool Point_In_Clip(const VECTOR IPoint, const vector<ObjectPtr>& Clip, TraceThreadData *Thread);
ObjectPtr Copy_Object(ObjectPtr Old);
vector<ObjectPtr> Copy_Objects(vector<ObjectPtr>& Src);
void Translate_Object(ObjectPtr Object, const VECTOR Vector, const TRANSFORM *Trans);
void Rotate_Object(ObjectPtr Object, const VECTOR Vector, const TRANSFORM *Trans);
void Scale_Object(ObjectPtr Object, const VECTOR Vector, const TRANSFORM *Trans);
void Transform_Object(ObjectPtr Object, const TRANSFORM *Trans);
bool Inside_Object(const VECTOR IPoint, ObjectPtr Vector, TraceThreadData *Thread);
void Invert_Object(ObjectPtr Object);
ObjectPtr Invert_CSG_Object(ObjectPtr& Object); // deletes Object and returns new pointer
void Destroy_Object(vector<ObjectPtr>& Object);
void Destroy_Object(ObjectPtr Object);
void Destroy_Single_Object(ObjectPtr *ObjectPtr);

}

#endif
