/*******************************************************************************
 * quadrics.h
 *
 * This module contains all defines, typedefs, and prototypes for QUADRICS.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/quadrics.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef QUADRICS_H
#define QUADRICS_H

#include "planes.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define QUADRIC_OBJECT (BASIC_OBJECT)



/*****************************************************************************
* Global typedefs
******************************************************************************/

class Quadric : public ObjectBase
{
	public:
		VECTOR Square_Terms;
		VECTOR Mixed_Terms;
		VECTOR Terms;
		DBL Constant;
		bool Automatic_Bounds;

		Quadric();
		virtual ~Quadric();

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

		static void Compute_Plane_Min_Max(const Plane *plane, VECTOR Min, VECTOR Max);
		void Compute_BBox(VECTOR ClipMin, VECTOR  ClipMax);
	protected:
		bool Intersect(const Ray& ray, DBL *Depth1, DBL *Depth2) const;
		void Quadric_To_Matrix(MATRIX Matrix) const;
		void Matrix_To_Quadric(const MATRIX Matrix);
};

}

#endif
