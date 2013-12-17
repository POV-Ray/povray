/*******************************************************************************
 * blob.h
 *
 * This module contains all defines, typedefs, and prototypes for BLOB.CPP.
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
 * $File: N/A $
 * $Revision: N/A $
 * $Change: N/A $
 * $DateTime: N/A $
 * $Author: N/A $
 *******************************************************************************/

#ifndef BLOB_H
#define BLOB_H

#include "backend/bounding/bsphere.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define BLOB_OBJECT (STURM_OK_OBJECT+HIERARCHY_OK_OBJECT)

// TODO - the following values probably don't have to be all discrete bits, except for BLOB_ENTER_EXIT_FLAG
#define BLOB_ENTER_EXIT_FLAG      1 // internal use only
#define BLOB_SPHERE               2
#define BLOB_CYLINDER             4
#define BLOB_ELLIPSOID            8
#define BLOB_BASE_HEMISPHERE     16
#define BLOB_APEX_HEMISPHERE     32
#define BLOB_BASE_HEMIELLIPSOID  64
#define BLOB_APEX_HEMIELLIPSOID 128


/* Define max. number of blob components. */
// [CLi] un-comment the following line if you want a hard limit of blob components; should be obsolete by now.
// #define MAX_BLOB_COMPONENTS 1000000

/* Generate additional blob statistics. */

#define BLOB_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

class Blob_Element
{
	public:
		short Type;       /* Type of component: sphere, hemisphere, cylinder */
		int index;
		VECTOR O;         /* Element's origin                                */
		DBL len;          /* Cylinder's length                               */
		DBL rad2;         /* Sphere's/Cylinder's radius^2                    */
		DBL c[3];         /* Component's coeffs                              */
		TEXTURE *Texture; /* Component's texture                             */
		TRANSFORM *Trans; /* Component's transformation                      */

		Blob_Element();
		~Blob_Element();
};

class Blob_Data
{
	public:
		int Number_Of_Components;   /* Number of components     */
		DBL Threshold;              /* Blob threshold           */
		vector<Blob_Element> Entry; /* Array of blob components */
		BSPHERE_TREE *Tree;         /* Bounding hierarchy       */

		Blob_Data(int count = 0);
		~Blob_Data();

		Blob_Data *AcquireReference(void);
		void ReleaseReference(void);

	private:
		int References;             /* Number of references     */
};

struct Blob_List_Struct
{
	Blob_Element elem;  /* Current element          */
	Blob_List_Struct *next;    /* Pointer to next element  */
};

struct Blob_Interval_Struct
{
	int type;
	DBL bound;
	const Blob_Element *Element;
};

class Blob : public ObjectBase
{
	public:
		Blob_Data *Data;
		TEXTURE **Element_Texture;

		Blob();
		virtual ~Blob();

		virtual ObjectPtr Copy();

		virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
		virtual bool Inside(const VECTOR, TraceThreadData *) const;
		virtual void Normal(VECTOR, Intersection *, TraceThreadData *) const;
		virtual void Translate(const VECTOR, const TRANSFORM *);
		virtual void Rotate(const VECTOR, const TRANSFORM *);
		virtual void Scale(const VECTOR, const TRANSFORM *);
		virtual void Transform(const TRANSFORM *);
		virtual void Invert();
		virtual void Compute_BBox();

		void Determine_Textures(Intersection *, bool, WeightedTextureVector&, TraceThreadData *);

		Blob_List_Struct *Create_Blob_List_Element();
		void Create_Blob_Element_Texture_List(Blob_List_Struct *BlobList, int npoints);
		int Make_Blob(DBL threshold, Blob_List_Struct *bloblist, int npoints, TraceThreadData *Thread);

		void Test_Blob_Opacity();

		static void Translate_Blob_Element(Blob_Element *Element, const VECTOR Vector);
		static void Rotate_Blob_Element(Blob_Element *Element, const VECTOR Vector);
		static void Scale_Blob_Element(Blob_Element *Element, const VECTOR Vector);
		static void Invert_Blob_Element(Blob_Element *Element);
		static void Transform_Blob_Element(Blob_Element *Element, const TRANSFORM *Trans);
	private:
		static void element_normal(VECTOR Result, const VECTOR P, const Blob_Element *Element);
		static int intersect_element(const VECTOR P, const VECTOR D, const Blob_Element *Element, DBL mindist, DBL *t0, DBL *t1, TraceThreadData *Thread);
		static void insert_hit(const Blob_Element *Element, DBL t0, DBL t1, Blob_Interval_Struct *intervals, unsigned int *cnt);
		int determine_influences(const VECTOR P, const VECTOR D, DBL mindist, Blob_Interval_Struct *intervals, TraceThreadData *Thread) const;
		DBL calculate_field_value(const VECTOR P, TraceThreadData *Thread) const;
		static DBL calculate_element_field(const Blob_Element *Element, const VECTOR P);

		static int intersect_cylinder(const Blob_Element *Element, const VECTOR P, const VECTOR D, DBL mindist, DBL *tmin, DBL *tmax);
		static int intersect_hemisphere(const Blob_Element *Element, const VECTOR P, const VECTOR D, DBL mindist, DBL *tmin, DBL *tmax);
		static int intersect_sphere(const Blob_Element *Element, const VECTOR P, const VECTOR D, DBL mindist, DBL *tmin, DBL *tmax);
		static int intersect_ellipsoid(const Blob_Element *Element, const VECTOR P, const VECTOR D, DBL mindist, DBL *tmin, DBL *tmax);

		static void get_element_bounding_sphere(const Blob_Element *Element, VECTOR Center, DBL *Radius2);
		void build_bounding_hierarchy();

		void determine_element_texture(const Blob_Element *Element, TEXTURE *Texture, const VECTOR P, WeightedTextureVector&);

		static bool insert_node(BSPHERE_TREE *Node, unsigned int *size, TraceThreadData *Thread);

		void getLocalIPoint(VECTOR lip, Intersection *isect) const;
};

}

#endif
