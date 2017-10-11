/****************************************************************************
*                   mesh.h
*
*  This module contains all defines, typedefs, and prototypes for MESH.C.
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


#ifndef MESH_H
#define MESH_H

#include "bbox.h"



/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define MESH_OBJECT (PATCH_OBJECT+HIERARCHY_OK_OBJECT)



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Mesh_Struct MESH;
typedef struct Mesh_Data_Struct MESH_DATA;
typedef struct Mesh_Triangle_Struct MESH_TRIANGLE;

struct Mesh_Struct
{
  OBJECT_FIELDS
  TRANSFORM *Trans;  /* Transformation for this object. */
  MESH_DATA *Data;   /* Mesh data holding triangles.    */
};

struct Mesh_Data_Struct
{
  int References;                /* Number of references to the mesh. */
  long Number_Of_Normals;        /* Number of normals in the mesh.    */
  long Number_Of_Textures;       /* Number of textures in the mesh.   */
  long Number_Of_Triangles;      /* Number of trinagles in the mesh.  */
  long Number_Of_Vertices;       /* Number of vertices in the mesh.   */
  SNGL_VECT *Normals, *Vertices; /* Arrays of normals and vertices.   */
  TEXTURE **Textures;            /* Array of texture references.      */
  MESH_TRIANGLE *Triangles;      /* Array of triangles.               */
  BBOX_TREE *Tree;               /* Bounding box tree for mesh.       */
};

struct Mesh_Triangle_Struct
{
  unsigned int Smooth:1;         /* Is this a smooth triangle.            */
  unsigned int Dominant_Axis:2;  /* Dominant axis.                        */
  unsigned int vAxis:2;          /* Axis for smooth triangle.             */
  long P1, P2, P3;               /* Indices of triangle vertices.         */
  long Normal_Ind;               /* Index of unsmoothed triangle normal.  */
  long Texture;                  /* Index of triangle texture.            */
  SNGL Distance;                 /* Distance of triangle along normal.    */
  long N1, N2, N3;               /* Indices of smoothed triangle normals. */
  SNGL_VECT Perp;                /* Vector used for smooth triangles.     */
};



/*****************************************************************************
* Global variables
******************************************************************************/

extern METHODS Mesh_Methods;


/*****************************************************************************
* Global functions
******************************************************************************/

MESH *Create_Mesh (void);
int Compute_Mesh_Triangle (MESH_TRIANGLE *Triangle, int Smooth, VECTOR P1, VECTOR P2, VECTOR P3, VECTOR S_Normal);
void Compute_Mesh_BBox (MESH *Mesh);
void Init_Mesh_Triangle (MESH_TRIANGLE *Triangle);
void Build_Mesh_BBox_Tree (MESH *Mesh);
void Test_Mesh_Opacity (MESH *Blob);

void Create_Mesh_Hash_Tables (void);
void Destroy_Mesh_Hash_Tables (void);
int Mesh_Hash_Vertex (int *Number_Of_Vertices, int *Max_Vertices, SNGL_VECT **Vertices, VECTOR Vertex);
int Mesh_Hash_Normal (int *Number_Of_Normals, int *Max_Normals, SNGL_VECT **Normals, VECTOR Normal);
int Mesh_Hash_Texture (int *Number_Of_Textures, int *Max_Textures, TEXTURE ***Textures, TEXTURE *Texture);
int Mesh_Degenerate (VECTOR P1, VECTOR P2, VECTOR P3);
void Initialize_Mesh_Code (void);
void Deinitialize_Mesh_Code (void);



#endif
