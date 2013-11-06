/*******************************************************************************
 * csg.h
 *
 * This module contains all defines, typedefs, and prototypes for CSG.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/csg.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef CSG_H
#define CSG_H

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* CSG types */

#define CSG_UNION_TYPE             1
#define CSG_INTERSECTION_TYPE      2
#define CSG_DIFFERENCE_TYPE        4
#define CSG_MERGE_TYPE             8
#define CSG_SINGLE_TYPE           16



/*****************************************************************************
* Global typedefs
******************************************************************************/

class CSG : public CompoundObject
{
	public:
		CSG(int t) : CompoundObject(t) {}
		CSG(int t, CompoundObject& o, bool transplant) : CompoundObject(t, o, transplant) {}

		int do_split;

		virtual void Normal(VECTOR, Intersection *, TraceThreadData *) const { }
		virtual void Translate(const VECTOR, const TRANSFORM *);
		virtual void Rotate(const VECTOR, const TRANSFORM *);
		virtual void Scale(const VECTOR, const TRANSFORM *);
		virtual void Transform(const TRANSFORM *);
		virtual void Compute_BBox();
		virtual void Invert();

		void Determine_Textures(Intersection *isect, bool hitinside, WeightedTextureVector& textures, TraceThreadData *Threaddata);
		virtual CSG *Morph(void) = 0;
};

class CSGUnion : public CSG
{
	public:
		CSGUnion();
		CSGUnion(int t);
		CSGUnion(int t, CompoundObject& o, bool transplant) : CSG(t, o, transplant) {}

		virtual ObjectPtr Copy();

		virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
		virtual bool Inside(const VECTOR, TraceThreadData *) const;
		virtual CSG *Morph(void);
};

class CSGMerge : public CSGUnion
{
	public:
		CSGMerge();
		CSGMerge(CompoundObject& o, bool transplant);

		virtual ObjectPtr Copy();

		virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
		virtual CSG *Morph(void);
};

class CSGIntersection : public CSG
{
	public:
		CSGIntersection(bool diff);
		CSGIntersection(bool diff, CompoundObject& o, bool transplant);

		virtual ObjectPtr Copy();

		virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *);
		virtual bool Inside(const VECTOR, TraceThreadData *) const;
		virtual CSG *Morph(void);

		bool isDifference;
};

}

#endif
