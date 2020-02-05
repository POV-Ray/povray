/****************************************************************************
*                   blob.h
*
*  This module contains all defines, typedefs, and prototypes for BLOB.C.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1998 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's GO POVRAY Forum or visit
*  http://www.povray.org. The latest version of POV-Ray may be found at these sites.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/


#ifndef BLOB_H
#define BLOB_H

#include "bsphere.h"



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define BLOB_OBJECT (STURM_OK_OBJECT+HIERARCHY_OK_OBJECT)

/* Do not use the first bit!!! (Used for enter/exit in intersection test) */

#define BLOB_SPHERE               2
#define BLOB_CYLINDER             4
#define BLOB_ELLIPSOID            8
#define BLOB_BASE_HEMISPHERE     16
#define BLOB_APEX_HEMISPHERE     32
#define BLOB_BASE_HEMIELLIPSOID  64
#define BLOB_APEX_HEMIELLIPSOID 128


/* Define max. number of blob components. */

#define MAX_BLOB_COMPONENTS 1000000

/* Generate additional blob statistics. */

#define BLOB_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Blob_Struct BLOB;
typedef struct Blob_Element_Struct BLOB_ELEMENT;
typedef struct Blob_Data_Struct BLOB_DATA;
typedef struct Blob_List_Struct BLOB_LIST;
typedef struct Blob_Interval_Struct BLOB_INTERVAL;

struct Blob_Element_Struct
{
  short Type;       /* Type of component: sphere, hemisphere, cylinder */
  int index;
  VECTOR O;         /* Element's origin                                */
  DBL len;          /* Cylinder's length                               */
  DBL rad2;         /* Sphere's/Cylinder's radius^2                    */
  DBL c[3];         /* Component's coeffs                              */
  DBL f[5];         /* Component's final coeffs                        */
  TEXTURE *Texture; /* Component's texture                             */
  TRANSFORM *Trans; /* Component's transformation                      */
};

struct Blob_Data_Struct
{
  int References;           /* Number of references     */
  int Number_Of_Components; /* Number of components     */
  DBL Threshold;            /* Blob threshold           */
  BLOB_ELEMENT *Entry;      /* Array of blob components */
  BLOB_INTERVAL* Intervals; /* Intervals used during intersection testing */
  BSPHERE_TREE *Tree;       /* Bounding hierarchy       */
};

struct Blob_Struct
{
  OBJECT_FIELDS
  TRANSFORM *Trans;
  BLOB_DATA *Data;    /* Pointer to blob data  */
  TEXTURE **Element_Texture;
};

struct Blob_List_Struct
{
  BLOB_ELEMENT elem;  /* Current element          */
  BLOB_LIST *next;    /* Pointer to next element  */
};

struct Blob_Interval_Struct
{
  int type;
  DBL bound;
  BLOB_ELEMENT *Element;
};



/*****************************************************************************
* Global variables
******************************************************************************/

extern METHODS Blob_Methods;



/*****************************************************************************
* Global functions
******************************************************************************/

void Set_Blob_Solver (OBJECT *obj, int Sturm_Flag);
void Init_Blob_Queue (void);
void BlobDelete (OBJECT *obj);
BLOB *Create_Blob (void);
void Make_Blob (BLOB *blob, DBL threshold, BLOB_LIST *bloblist, int npoints);
BLOB_LIST *Create_Blob_List_Element (void);
void Create_Blob_Element_Texture_List (BLOB *Blob, BLOB_LIST *BlobList, int npoints);
void Determine_Blob_Textures (BLOB *Blob, VECTOR P, int *Count, TEXTURE **Textures, DBL *Weights);
void Test_Blob_Opacity (BLOB *Blob);

void Translate_Blob_Element (BLOB_ELEMENT *Element, VECTOR Vector);
void Rotate_Blob_Element (BLOB_ELEMENT *Element, VECTOR Vector);
void Scale_Blob_Element (BLOB_ELEMENT *Element, VECTOR Vector);
void Invert_Blob_Element (BLOB_ELEMENT *Element);
void Transform_Blob_Element (BLOB_ELEMENT *Element, TRANSFORM *Trans);
void Destroy_Blob_Queue (void);



#endif
