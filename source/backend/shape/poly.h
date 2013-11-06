/*******************************************************************************
 * poly.h
 *
 * This module contains all defines, typedefs, and prototypes for POLY.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/poly.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef POLY_H
#define POLY_H

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define POLY_OBJECT    (STURM_OK_OBJECT)
#define CUBIC_OBJECT   (STURM_OK_OBJECT)
#define QUARTIC_OBJECT (STURM_OK_OBJECT)

/* Number of coefficients of a three variable polynomial of order x */

inline int term_counts(int x) { return ((x+1)*(x+2)*(x+3)/6); }



/*****************************************************************************
* Global typedefs
******************************************************************************/

class Poly : public ObjectBase
{
	public:
		int Order;
		DBL *Coeffs;

		Poly(int order);
		virtual ~Poly();

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
		virtual bool Intersect_BBox(BBoxDirection, const BBOX_VECT&, const BBOX_VECT&, BBOX_VAL) const;

		bool Set_Coeff(const unsigned int x,const unsigned int y, const unsigned int z, const DBL value);
	protected:
		static int intersect(const Ray &Ray, int Order, const DBL *Coeffs, int Sturm_Flag, DBL *Depths, TraceThreadData *Thread);
		static void normal0(VECTOR Result, int Order, const DBL *Coeffs, const VECTOR IPoint);
		static void normal1(VECTOR Result, int Order, const DBL *Coeffs, const VECTOR IPoint);
		static DBL inside(const VECTOR IPoint, int Order, const DBL *Coeffs);
		static int intersect_linear(const Ray &ray, const DBL *Coeffs, DBL *Depths);
		static int intersect_quadratic(const Ray &ray, const DBL *Coeffs, DBL *Depths);
		// static int factor_out(int n, int i, int *c, int *s);
		//static int binomial(int n, int r);
		//static void factor1(int n, int *c, int *s);
};

}

#endif
