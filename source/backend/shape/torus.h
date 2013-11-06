/*******************************************************************************
 * torus.h
 *
 * This module contains all defines, typedefs, and prototypes for TORUS.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/torus.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef TORUS_H
#define TORUS_H

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define TORUS_OBJECT (STURM_OK_OBJECT)

/* Generate additional torus statistics. */

#define TORUS_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

/*
 * Torus structure.
 *
 *   R : Major radius
 *   r : Minor radius
 */

class Torus : public ObjectBase
{
	public:
		DBL MajorRadius, MinorRadius;

		Torus();
		virtual ~Torus();

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
	protected:
		int Intersect(const Ray& ray, DBL *Depth, TraceThreadData *Thread) const;
		bool Test_Thick_Cylinder(const VECTOR P, const VECTOR D, DBL h1, DBL h2, DBL r1, DBL r2) const;
		void CalcUV(const VECTOR IPoint, UV_VECT Result) const;
};

}

#endif
