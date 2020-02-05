/****************************************************************************
*                   hfield.h
*
*  This module contains all defines, typedefs, and prototypes for HFIELD.C.
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

#ifndef HFIELD_H
#define HFIELD_H

#include "bbox.h"
#include "boxes.h"



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define HFIELD_OBJECT (BASIC_OBJECT+WATER_LEVEL_OK_OBJECT+SMOOTH_OK_OBJECT+HIERARCHY_OK_OBJECT)

#define HF_CACHE_SIZE 16

/* Generate additional height field statistics. */

#define HFIELD_EXTRA_STATS 1


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct HField_Struct HFIELD;
typedef struct HField_Data_Struct HFIELD_DATA;
typedef struct HField_Block_Struct HFIELD_BLOCK;
typedef struct HField_Normal_Struct HFIELD_NORMAL;
typedef short HF_Normals[3];
typedef unsigned short HF_VAL;

struct HField_Normal_Struct
{
  DBL fx, fz;
  VECTOR normal;
};

struct HField_Block_Struct
{
  int xmin, xmax;
  int zmin, zmax;
  DBL ymin, ymax;
};

struct HField_Data_Struct
{
  int References;
  int cache_pos;
  int Normals_Height;  /* Needed for Destructor */
  int max_x, max_z;
  HF_VAL min_y, max_y;
  int block_max_x, block_max_z;
  int block_width_x, block_width_z;
  HF_VAL **Map;
  HF_Normals **Normals;
  HFIELD_NORMAL Normal_Cache[HF_CACHE_SIZE];
  HFIELD_BLOCK **Block;
};

struct HField_Struct
{
  OBJECT_FIELDS
  TRANSFORM *Trans;
  BOX *bounding_box;
  HFIELD_DATA *Data;
};



/*****************************************************************************
* Global variables
******************************************************************************/

extern METHODS HField_Methods;


/*****************************************************************************
* Global functions
******************************************************************************/

HFIELD *Create_HField (void);
void   Compute_HField_BBox (HFIELD *HField);
void   Compute_HField (HFIELD *H_Field, IMAGE *Image);



#endif
