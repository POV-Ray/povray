/****************************************************************************
*                   objects.h
*
*  This module contains all defines, typedefs, and prototypes for OBJECTS.C.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1999 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by email to team-coord@povray.org or visit us on the web at
*  http://www.povray.org. The latest version of POV-Ray may be found at this site.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

/* NOTE: FRAME.H contains other object stuff. */

#ifndef OBJECTS_H
#define OBJECTS_H



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/*
 * [DB 7/94]
 *
 * The flag field is used to store all possible flags that are
 * used for objects (up to 16).
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

#define NO_SHADOW_FLAG                0x00001 /* Object doesn't cast shadows            */
#define CLOSED_FLAG                   0x00002 /* Object is closed                       */
#define INVERTED_FLAG                 0x00004 /* Object is inverted                     */
#define SMOOTHED_FLAG                 0x00008 /* Object is smoothed                     */
#define CYLINDER_FLAG                 0x00010 /* Object is a cylinder                   */
#define DEGENERATE_FLAG               0x00020 /* Object is degenerate                   */
#define STURM_FLAG                    0x00040 /* Object should use sturmian root solver */
#define OPAQUE_FLAG                   0x00080 /* Object is opaque                       */
#define MULTITEXTURE_FLAG             0x00100 /* Object is multi-textured               */
#define INFINITE_FLAG                 0x00200 /* Object is infinite                     */
#define HIERARCHY_FLAG                0x00400 /* Object can have a bounding hierarchy   */
#define HOLLOW_FLAG                   0x00800 /* Object is hollow (atmosphere inside)   */
#define HOLLOW_SET_FLAG               0x01000 /* Hollow explicitly set in scene file    */

#define Set_Flag(Object, Flag)     \
  { (Object)->Flags |=  (Flag); }

#define Clear_Flag(Object, Flag)   \
  { (Object)->Flags &= ~(Flag); }

#define Invert_Flag(Object, Flag)  \
  { (Object)->Flags ^=  (Flag); }

#define Test_Flag(Object, Flag)    \
  ((Object)->Flags & (Flag))

#define Copy_Flag(Object1, Object2, Flag) \
  { (Object1)->Flags = (((Object1)->Flags) & (!Flag)) | \
                       (((Object2)->Flags) &  (Flag)); }

#define Bool_Flag(Object1, Flag, Bool) \
  { if(Bool){ (Object)->Flags |=  (Flag); } else { (Object)->Flags &= ~(Flag); }}



/* Object types. */

#define BASIC_OBJECT            0
#define PATCH_OBJECT            1 /* Has no inside, no inverse */
#define TEXTURED_OBJECT         2 /* Has texture, possibly in children */
#define COMPOUND_OBJECT         4 /* Has children field */
#define STURM_OK_OBJECT         8 /* STRUM legal */
#define WATER_LEVEL_OK_OBJECT  16 /* WATER_LEVEL legal */
#define LIGHT_SOURCE_OBJECT    32 /* link me in frame.light_sources */
#define BOUNDING_OBJECT        64 /* This is a holder for bounded object */
#define SMOOTH_OK_OBJECT      128 /* SMOOTH legal */
#define IS_CHILD_OBJECT       256 /* Object is inside a COMPOUND */
#define DOUBLE_ILLUMINATE     512 /* Illuminate both sides of surface to avoid normal purturb bug */
#define HIERARCHY_OK_OBJECT  1024 /* NO_HIERARCHY legal */
#define LT_SRC_UNION_OBJECT  2048 /* Union of light_source objects only */

#define CHILDREN_FLAGS (PATCH_OBJECT+TEXTURED_OBJECT)  /* Reverse inherited flags */



/*****************************************************************************
* Global typedefs
******************************************************************************/



/*****************************************************************************
* Global variables
******************************************************************************/

extern unsigned int Number_of_istacks;
extern unsigned int Max_Intersections;
extern ISTACK *free_istack;



/*****************************************************************************
* Global functions
******************************************************************************/

int Intersection (INTERSECTION *Ray_Intersection, OBJECT *Object, RAY *Ray);
int Ray_In_Bound (RAY *Ray, OBJECT *Bounding_Object);
int Point_In_Clip (VECTOR IPoint, OBJECT *Clip);
OBJECT *Copy_Object (OBJECT *Old);
void Translate_Object (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
void Rotate_Object (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
void Scale_Object (OBJECT *Object, VECTOR Vector, TRANSFORM *Trans);
void Transform_Object (OBJECT *Object, TRANSFORM *Trans);
int Inside_Object (VECTOR IPoint, OBJECT *Vector);
void Invert_Object (OBJECT *Object);
void Destroy_Object (OBJECT *Object);
ISTACK *open_istack (void);
void close_istack (ISTACK *istk);
void incstack (ISTACK *istk);
void Destroy_IStacks (void);



#endif
