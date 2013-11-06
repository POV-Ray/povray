/*******************************************************************************
 * hfield.h
 *
 * This module contains all defines, typedefs, and prototypes for HFIELD.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/hfield.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef HFIELD_H
#define HFIELD_H

#include "backend/bounding/bbox.h"
#include "backend/shape/boxes.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define HFIELD_OBJECT (BASIC_OBJECT+HIERARCHY_OK_OBJECT)

/* Generate additional height field statistics. */

#define HFIELD_EXTRA_STATS 1


/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct HField_Data_Struct HFIELD_DATA;
typedef struct HField_Block_Struct HFIELD_BLOCK;
typedef struct HField_Normal_Struct HFIELD_NORMAL;
typedef short HF_Normals[3];

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
	int Normals_Height;  /* Needed for Destructor */
	int max_x, max_z;
	HF_VAL min_y, max_y;
	int block_max_x, block_max_z;
	int block_width_x, block_width_z;
	HF_VAL **Map;
	HF_Normals **Normals;
	HFIELD_BLOCK **Block;
};

class HField : public ObjectBase
{
	public:
		VECTOR bounding_corner1;
		VECTOR bounding_corner2;
		HFIELD_DATA *Data;

		HField();
		virtual ~HField();

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

		void Compute_HField(const ImageData *image);
	protected:
		static DBL normalize(VECTOR A, const VECTOR B);
		void smooth_height_field(int xsize, int zsize);
		bool intersect_pixel(int x, int z, const Ray& ray, DBL height1, DBL height2, IStack &HField_Stack, const Ray &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread);
		static int add_single_normal(HF_VAL **data, int xsize, int zsize, int x0, int z0,int x1, int z1,int x2, int z2, VECTOR N);
		bool dda_traversal(const Ray &ray, const VECTOR Start, const HFIELD_BLOCK *Block, IStack &HField_Stack, const Ray &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread);
		bool block_traversal(const Ray &ray, const VECTOR Start, IStack &HField_Stack, const Ray &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread);
		void build_hfield_blocks();
};

}

#endif
