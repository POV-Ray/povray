/****************************************************************************
*                   octree.h
*
*  Oct-tree routine prototypes.  Use by Radiosity calculation routies.
*  Implemented by and (c) 1994 Jim McElhiney, mcelhiney@acm.org or cserve 71201,1326
*  All standard POV distribution rights granted.  All other rights reserved.
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
*************************************************************************/

#ifndef OCTREE_H
#define OCTREE_H


/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define OT_BIAS 10000000.

#define MAX3(a,b,c) ( ((a)>(b)) ? max((a),(c)) : max((b),(c)) )


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct ot_block_struct OT_BLOCK;
typedef struct ot_id_struct OT_ID;
typedef struct ot_node_struct OT_NODE;

/* Each node in the oct-tree has a (possibly null) linked list of these
   data blocks off it.  */
struct ot_block_struct
{
  OT_BLOCK *next;
  VECTOR Point, S_Normal;
  float  drdx, dgdx, dbdx,  drdy, dgdy, dbdy,  drdz, dgdz, dbdz;
  RGB    Illuminance;
  float  Harmonic_Mean_Distance, Nearest_Distance;
  VECTOR To_Nearest_Surface;
  short  Bounce_Depth;
};

/* This is the information necessary to name an oct-tree node. */
struct ot_id_struct
{
  long x, y, z;
  long Size;
};

/* These are the structures that make up the oct-tree itself, known as nodes */
struct ot_node_struct
{
  OT_ID    Id;
  OT_BLOCK *Values;
  OT_NODE  *Kids[8];
};


/*****************************************************************************
* Global variables
******************************************************************************/



/*****************************************************************************
* Global functions
******************************************************************************/

void ot_ins (OT_NODE **root, OT_BLOCK *new_block, OT_ID *new_id);
void ot_list_insert (OT_BLOCK **list_ptr, OT_BLOCK *item);
void ot_newroot (OT_NODE **root_ptr);
long ot_dist_traverse (OT_NODE *subtree, VECTOR point, int bounce_depth,  \
               long (*func)(OT_BLOCK *block, void *handle1), void *handle2);
long ot_point_in_node (VECTOR point, OT_ID *node);
void ot_index_sphere (VECTOR point, DBL radius, OT_ID *id);
void ot_index_box (VECTOR min_point, VECTOR max_point, OT_ID *id);
void ot_parent (OT_ID *dad, OT_ID *kid);
long ot_save_tree (OT_NODE *rootptr, FILE *fd);
long ot_write_block (OT_BLOCK *bl, void * handle);
long ot_free_tree (OT_NODE **ppRoot);
long ot_read_file (FILE * fd);


/* a trunc function which always returns the floor integer */
long Trunc (DBL value);



#endif
