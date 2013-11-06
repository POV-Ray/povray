/*******************************************************************************
 * super.h
 *
 * This module contains all defines, typedefs, and prototypes for SUPEREL.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/super.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef SUPER_H
#define SUPER_H

namespace pov
{

/*****************************************************************************
* Global preprocessor definitions
******************************************************************************/

#define SUPERELLIPSOID_OBJECT (BASIC_OBJECT)



/*****************************************************************************
* Global typedefs
******************************************************************************/

class Superellipsoid : public ObjectBase
{
	public:
		VECTOR Power;

		Superellipsoid();
		virtual ~Superellipsoid();

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
	protected:
		bool Intersect(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread);
		static bool intersect_box(const VECTOR P, const VECTOR D, DBL *dmin, DBL *dmax);
		static DBL power(DBL x, DBL e);
		static DBL evaluate_g(DBL x, DBL y, DBL e);
		DBL evaluate_superellipsoid(const VECTOR P) const;
		static int compdists(const void *in_a, const void *in_b);
		int find_ray_plane_points(const VECTOR P, const VECTOR D, int cnt, DBL *dists, DBL mindist, DBL maxdist) const;
		void solve_hit1(DBL v0, const VECTOR tP0, DBL v1, const VECTOR tP1, VECTOR P) const;
		bool check_hit2(const VECTOR P, const VECTOR D, DBL t0, VECTOR P0, DBL v0, DBL t1, DBL *t, VECTOR Q) const;
		bool insert_hit(const Ray& ray, DBL Depth, IStack& Depth_Stack, TraceThreadData *Thread);
};

}

#endif
