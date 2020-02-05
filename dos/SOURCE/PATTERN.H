/****************************************************************************
*                   pattern.h
*
*  This module contains all defines, typedefs, and prototypes for PATTERN.C.
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

/* NOTE: FRAME.H contains other pattern stuff. */

#ifndef PATTERN_H
#define PATTERN_H


/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define NO_PATTERN               0
#define PLAIN_PATTERN            1
#define AVERAGE_PATTERN          2
#define BITMAP_PATTERN           3

#define LAST_SPECIAL_PATTERN     BITMAP_PATTERN

/* These former normal patterns require special handling.  They too
   must be kep seperate for now.*/

#define WAVES_PATTERN            4
#define RIPPLES_PATTERN          5
#define WRINKLES_PATTERN         6
#define BUMPS_PATTERN            7
#define QUILTED_PATTERN          8
#define DENTS_PATTERN            9

#define LAST_NORM_ONLY_PATTERN    DENTS_PATTERN

/* These patterns return integer values.  They must be kept
   together in the list.  Any new integer functions added must be added
   here and the list renumbered. */
   
#define CHECKER_PATTERN         10
#define BRICK_PATTERN           11
#define HEXAGON_PATTERN         12

#define LAST_INTEGER_PATTERN     HEXAGON_PATTERN

/* These patterns return float values.  They must be kept together
   and seperate from those above. */

#define BOZO_PATTERN            13
#define MARBLE_PATTERN          14
#define WOOD_PATTERN            15
#define SPOTTED_PATTERN         16
#define AGATE_PATTERN           17
#define GRANITE_PATTERN         18
#define GRADIENT_PATTERN        19
#define ONION_PATTERN           20
#define LEOPARD_PATTERN         21
#define MANDEL_PATTERN          22
#define RADIAL_PATTERN          23
#define CRACKLE_PATTERN         24
#define SPIRAL1_PATTERN         25
#define SPIRAL2_PATTERN         26
#define PLANAR_PATTERN          27
#define SPHERICAL_PATTERN       28
#define BOXED_PATTERN           29
#define CYLINDRICAL_PATTERN     30
#define DENSITY_FILE_PATTERN    31


/* Pattern flags */

#define NO_FLAGS      0
#define HAS_FILTER    1
#define FULL_BLOCKING 2
#define POST_DONE     4

#define Destroy_Turb(t) if ((t)!=NULL) POV_FREE(t);

#define RAMP_WAVE     0
#define SINE_WAVE     1
#define TRIANGLE_WAVE 2
#define SCALLOP_WAVE  3
#define CUBIC_WAVE    4
#define POLY_WAVE     5

/* Interpolation types. */

#define NO_INTERPOLATION        0
#define TRILINEAR_INTERPOLATION 1


/*****************************************************************************
* Global typedefs
******************************************************************************/


/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global constants
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

DBL Evaluate_TPat (TPATTERN *TPat, VECTOR EPoint);
void Init_TPat_Fields (TPATTERN *Tpat);
void Copy_TPat_Fields (TPATTERN *New, TPATTERN *Old);
void Destroy_TPat_Fields (TPATTERN *Tpat);
void Translate_Tpattern (TPATTERN *Tpattern, VECTOR Vector);
void Rotate_Tpattern (TPATTERN *Tpattern, VECTOR Vector);
void Scale_Tpattern (TPATTERN *Tpattern, VECTOR Vector);
void Transform_Tpattern (TPATTERN *Tpattern, TRANSFORM *Trans);
DBL quilt_cubic (DBL t,DBL p1,DBL p2);
void Search_Blend_Map (DBL value,BLEND_MAP *Blend_Map,
 BLEND_MAP_ENTRY **Prev, BLEND_MAP_ENTRY **Cur);

DENSITY_FILE *Create_Density_File (void);
DENSITY_FILE *Copy_Density_File (DENSITY_FILE *);
void Destroy_Density_File (DENSITY_FILE *);
void Read_Density_File (DENSITY_FILE *df);



#endif
