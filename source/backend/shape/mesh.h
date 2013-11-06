/*******************************************************************************
 * mesh.h
 *
 * This module contains all defines, typedefs, and prototypes for MESH.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/mesh.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef MESH_H
#define MESH_H

#include "backend/bounding/bbox.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define MESH_OBJECT (PATCH_OBJECT+HIERARCHY_OK_OBJECT) // NOTE: During parsing, the PATCH_OBJECT type flag may be cleared if an inside_vector is specified


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Mesh_Data_Struct MESH_DATA;
typedef struct Mesh_Triangle_Struct MESH_TRIANGLE;

typedef struct Hash_Table_Struct HASH_TABLE;
typedef struct UV_Hash_Table_Struct UV_HASH_TABLE;

struct Mesh_Data_Struct
{
	int References;                /* Number of references to the mesh. */
	long Number_Of_UVCoords;       /* Number of UV coords in the mesh.  */
	long Number_Of_Normals;        /* Number of normals in the mesh.    */
	long Number_Of_Triangles;      /* Number of trinagles in the mesh.  */
	long Number_Of_Vertices;       /* Number of vertices in the mesh.   */
	SNGL_VECT *Normals, *Vertices; /* Arrays of normals and vertices.   */
	UV_VECT *UVCoords;             /* Array of UV coordinates           */
	MESH_TRIANGLE *Triangles;      /* Array of triangles.               */
	BBOX_TREE *Tree;               /* Bounding box tree for mesh.       */
	VECTOR Inside_Vect;            /* vector to use to test 'inside'    */
};

struct Mesh_Triangle_Struct
{
	unsigned int Smooth:1;         /* Is this a smooth triangle.            */
	unsigned int Dominant_Axis:2;  /* Dominant axis.                        */
	unsigned int vAxis:2;          /* Axis for smooth triangle.             */
	unsigned int ThreeTex:1;       /* Color Triangle Patch.                 */
	long Normal_Ind;               /* Index of unsmoothed triangle normal.  */
	long P1, P2, P3;               /* Indices of triangle vertices.         */
	long Texture;                  /* Index of triangle texture.            */
	long Texture2, Texture3;       /* Color Triangle Patch.                 */
	long N1, N2, N3;               /* Indices of smoothed triangle normals. */
	long UV1, UV2, UV3;            /* Indicies of UV coordinate vectors     */
	SNGL Distance;                 /* Distance of triangle along normal.    */
	SNGL_VECT Perp;                /* Vector used for smooth triangles.     */
};

struct Hash_Table_Struct
{
	int Index;
	SNGL_VECT P;
	HASH_TABLE *Next;
};

struct UV_Hash_Table_Struct
{
	int Index;
	UV_VECT P;
	UV_HASH_TABLE *Next;
};

class Mesh : public ObjectBase
{
	public:
		MESH_DATA *Data;               /* Mesh data holding triangles.    */
		long Number_Of_Textures;       /* Number of textures in the mesh.   */
		TEXTURE **Textures;            /* Array of texture references.      */
		short has_inside_vector;

		Mesh();
		virtual ~Mesh();

		virtual ObjectPtr Copy();

		virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
		virtual bool Inside(const VECTOR, TraceThreadData *) const;
		virtual void Normal(VECTOR, Intersection *, TraceThreadData *) const;
		virtual void UVCoord(UV_VECT, const Intersection *, TraceThreadData *) const;
		virtual void Translate(const VECTOR, const TRANSFORM *);
		virtual void Rotate(const VECTOR, const TRANSFORM *);
		virtual void Scale(const VECTOR, const TRANSFORM *);
		virtual void Transform(const TRANSFORM *);
		virtual void Invert();
		virtual void Compute_BBox();

		void Test_Mesh_Opacity();

		void Create_Mesh_Hash_Tables();
		bool Compute_Mesh_Triangle(MESH_TRIANGLE *Triangle, int Smooth, VECTOR P1, VECTOR P2, VECTOR P3, VECTOR S_Normal);
		void Build_Mesh_BBox_Tree();
		bool Degenerate(const VECTOR P1, const VECTOR P2, const VECTOR P3);
		void Init_Mesh_Triangle(MESH_TRIANGLE *Triangle);
		void Destroy_Mesh_Hash_Tables();
		int Mesh_Hash_Vertex(int *Number_Of_Vertices, int *Max_Vertices, SNGL_VECT **Vertices, const VECTOR Vertex);
		int Mesh_Hash_Normal(int *Number_Of_Normals, int *Max_Normals, SNGL_VECT **Normals, const VECTOR Normal);
		int Mesh_Hash_Texture(int *Number_Of_Textures, int *Max_Textures, TEXTURE ***Textures, TEXTURE *Texture);
		int Mesh_Hash_UV(int *Number, int *Max, UV_VECT **Elements, const UV_VECT aPoint);
		void Smooth_Mesh_Normal(VECTOR Result, const MESH_TRIANGLE *Triangle, const VECTOR IPoint) const;

		void Determine_Textures(Intersection *, bool, WeightedTextureVector&, TraceThreadData *);
	protected:
		bool Intersect(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread);
		void Compute_Mesh_BBox();
		void MeshUV(const VECTOR P, const MESH_TRIANGLE *Triangle, UV_VECT Result) const;
		void compute_smooth_triangle(MESH_TRIANGLE *Triangle, const VECTOR P1, const VECTOR P2, const VECTOR P3);
		bool intersect_mesh_triangle(const Ray& ray, const MESH_TRIANGLE *Triangle, DBL *Depth) const;
		bool test_hit(const MESH_TRIANGLE *Triangle, const Ray& OrigRay, DBL Depth, DBL len, IStack& Depth_Stack, TraceThreadData *Thread);
		void get_triangle_bbox(const MESH_TRIANGLE *Triangle, BBOX *BBox) const;
		bool intersect_bbox_tree(const Ray& ray, const Ray& Orig_Ray, DBL len, IStack& Depth_Stack, TraceThreadData *Thread);
		bool inside_bbox_tree(const Ray& ray, TraceThreadData *Thread) const;
		void get_triangle_vertices(const MESH_TRIANGLE *Triangle, VECTOR P1, VECTOR P2, VECTOR P3) const;
		void get_triangle_normals(const MESH_TRIANGLE *Triangle, VECTOR N1, VECTOR N2, VECTOR N3) const;
		void get_triangle_uvcoords(const MESH_TRIANGLE *Triangle, UV_VECT U1, UV_VECT U2, UV_VECT U3) const;
		static int mesh_hash(HASH_TABLE **Hash_Table, int *Number, int *Max, SNGL_VECT **Elements, const VECTOR aPoint);

private:
		// these are used temporarily during parsing and are destroyed
		// when the parser has finished constructing the object
		static HASH_TABLE **Vertex_Hash_Table;
		static HASH_TABLE **Normal_Hash_Table;
		static UV_HASH_TABLE **UV_Hash_Table;
};

}

#endif
