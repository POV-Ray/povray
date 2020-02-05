/****************************************************************************
*                   radiosit.h
*
*  Include file for radiosit.c, Radiosity calculation routies.
*
*  Implemented by and (c) 1994 Jim McElhiney, mcelhiney@acm.org or 71201,1326
*  All standard POV distribution rights granted.  All other rights reserved.
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
*************************************************************************/


#ifndef RADIOSIT_H
#define RADIOSIT_H
#define RADIOSITY_CACHE_EXTENSION ".rca"



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* #define RADDEBUG 1 */

#define MAX_NEAREST_COUNT 10




/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct wt_avg_struct WT_AVG;

/* quickie structure used to gather weighted average during tree traversal */
struct wt_avg_struct
{
  COLOUR Weights_Times_Illuminances; /* Aggregates during traversal */
  DBL    Weights;   /* Aggregates during traversal */
  long   Weights_Count;    /* Count of points used, aggregates during trav */
  long   Good_Count;    /* Count of points used, aggregates during trav */
  VECTOR P, N;     /* Point and Normal:  input to traverse */
  DBL    Current_Error_Bound;   /* see Radiosity_Error_Bound */

  COLOUR Weight_Times_Illuminance[MAX_NEAREST_COUNT];
  DBL    Weight[MAX_NEAREST_COUNT];
  DBL    Distance[MAX_NEAREST_COUNT];
  long   Close_Count;
};

typedef struct byte_xyz BYTE_XYZ;

struct byte_xyz {
  unsigned char x, y, z;
};


/*****************************************************************************
* Global variables
******************************************************************************/

extern long ra_reuse_count;
extern long ra_gather_count;
extern OT_NODE *ot_root;
extern FILE *ot_fd;
extern COLOUR Radiosity_Gather_Total;
extern long Radiosity_Gather_Total_Count;
extern int Radiosity_Trace_Level;
extern BYTE_XYZ rad_samples[];


/*****************************************************************************
* Global functions
******************************************************************************/

int Compute_Ambient (VECTOR IPoint, VECTOR Normal, COLOUR Ambient_Colour, DBL Weight);
long Initialize_Radiosity_Code (void);
long Deinitialize_Radiosity_Code (void);


#endif
