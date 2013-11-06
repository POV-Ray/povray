/*******************************************************************************
 * bezier.h
 *
 * This module contains all defines, typedefs, and prototypes for BEZIER.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/bezier.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef BEZIER_H
#define BEZIER_H

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define BICUBIC_PATCH_OBJECT (PATCH_OBJECT)
/* NK 1998 double_illuminate - removed +DOUBLE_ILLUMINATE from bicubic_patch */

#define BEZIER_INTERIOR_NODE 0
#define BEZIER_LEAF_NODE 1

#define MAX_PATCH_TYPE 2



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef DBL DISTANCES[4][4];
typedef DBL WEIGHTS[4][4];
typedef struct Bezier_Node_Struct BEZIER_NODE;
typedef struct Bezier_Child_Struct BEZIER_CHILDREN;
typedef struct Bezier_Vertices_Struct BEZIER_VERTICES;

struct Bezier_Child_Struct
{
	BEZIER_NODE *Children[4];
};

struct Bezier_Vertices_Struct
{
	float uvbnds[4];
	VECTOR Vertices[4];
};

struct Bezier_Node_Struct
{
	int Node_Type;      /* Is this an interior node, or a leaf */
	VECTOR Center;      /* Center of sphere bounding the (sub)patch */
	DBL Radius_Squared; /* Radius of bounding sphere (squared) */
	int Count;          /* # of subpatches associated with this node */
	void *Data_Ptr;     /* Either pointer to vertices or pointer to children */
};

class BicubicPatch : public ObjectBase
{
	public:
		int Patch_Type, U_Steps, V_Steps;
		VECTOR Control_Points[4][4];
		UV_VECT ST[4];
		VECTOR Bounding_Sphere_Center;
		DBL Bounding_Sphere_Radius;
		DBL Flatness_Value;
		DBL accuracy;
		BEZIER_NODE *Node_Tree;
		WEIGHTS *Weights;

		BicubicPatch();
		virtual ~BicubicPatch();

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

		void Precompute_Patch_Values();
	protected:
		static void bezier_value(const VECTOR(*Control_Points)[4][4], DBL u0, DBL v0, VECTOR P, VECTOR N);
		bool intersect_subpatch(const Ray&, const VECTOR [3], const DBL [3], const DBL [3], DBL *, VECTOR, VECTOR, DBL *, DBL *) const;
		static void find_average(int, const VECTOR *, VECTOR, DBL *);
		static bool spherical_bounds_check(const Ray &, const VECTOR, DBL);
		int intersect_bicubic_patch0(const Ray& , IStack&);
		static DBL point_plane_distance(const VECTOR, const VECTOR, DBL);
		static DBL determine_subpatch_flatness(const VECTOR(*)[4][4]);
		bool flat_enough(const VECTOR(*)[4][4]) const;
		static void bezier_bounding_sphere(const VECTOR(*)[4][4], VECTOR, DBL *);
		int bezier_subpatch_intersect(const Ray &, const VECTOR(*)[4][4], DBL, DBL, DBL, DBL, IStack&);
		static void bezier_split_left_right(const VECTOR(*)[4][4], VECTOR(*)[4][4], VECTOR(*)[4][4]);
		static void bezier_split_up_down(const VECTOR(*)[4][4], VECTOR(*)[4][4], VECTOR(*)[4][4]);
		int bezier_subdivider(const Ray &, const VECTOR(*)[4][4], DBL, DBL, DBL, DBL, int, IStack&);
		static void bezier_tree_deleter(BEZIER_NODE *Node);
		BEZIER_NODE *bezier_tree_builder(const VECTOR(*Patch)[4][4], DBL u0, DBL u1, DBL v0, DBL v1, int depth, int& max_depth_reached);
		int bezier_tree_walker(const Ray &, const BEZIER_NODE *, IStack&);
		static BEZIER_NODE *create_new_bezier_node(void);
		static BEZIER_VERTICES *create_bezier_vertex_block(void);
		static BEZIER_CHILDREN *create_bezier_child_block(void);
		static bool subpatch_normal(const VECTOR v1, const VECTOR v2, const VECTOR v3, VECTOR Result, DBL *d);
		static void Compute_Texture_UV(const UV_VECT p, const UV_VECT st[4], UV_VECT t);
};



}

#endif
