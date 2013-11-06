/*******************************************************************************
 * lathe.h
 *
 * This module contains all defines, typedefs, and prototypes for LATHE.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/lathe.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef LATHE_H
#define LATHE_H

#include "backend/bounding/bcyl.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor definitions
******************************************************************************/

#define LATHE_OBJECT (STURM_OK_OBJECT)

#define LINEAR_SPLINE    1
#define QUADRATIC_SPLINE 2
#define CUBIC_SPLINE     3
#define BEZIER_SPLINE    4

/* Generate additional lathe statistics. */

#define LATHE_EXTRA_STATS 1



/*****************************************************************************
* Global typedefs
******************************************************************************/

typedef struct Lathe_Struct LATHE;
typedef struct Lathe_Spline_Struct LATHE_SPLINE;
typedef struct Lathe_Spline_Entry_Struct LATHE_SPLINE_ENTRY;

struct Lathe_Spline_Entry_Struct
{
	UV_VECT A, B, C, D;  /* Coefficients of segment */
};

struct Lathe_Spline_Struct
{
	int References;             /* Count references to this structure. */
	LATHE_SPLINE_ENTRY *Entry;  /* Array of spline segments.           */
	BCYL *BCyl;                 /* bounding cylinder.                  */
};

class Lathe : public ObjectBase
{
	public:
		int Spline_Type;          /* Spline type (linear, quadratic ...)  */
		int Number;               /* Number of segments!!!                */
		LATHE_SPLINE *Spline;     /* Pointer to spline array              */
		DBL Height1, Height2;     /* Min./Max. height                     */
		DBL Radius1, Radius2;     /* Min./Max. radius                     */

		Lathe();
		virtual ~Lathe();

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

		void Compute_Lathe(UV_VECT *P, TraceThreadData *);
	protected:
		bool Intersect(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread);
		bool test_hit(const Ray&, IStack&, DBL, DBL, int, TraceThreadData *Thread);
};

}

#endif
