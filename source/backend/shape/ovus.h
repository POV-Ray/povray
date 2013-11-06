/*******************************************************************************
 * ovus.h
 *
 * This module implements the header for the ovus primitive.
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
 * $File: //depot/public/povray/3.x/source/backend/shape/ovus.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef OVUS_H
#define OVUS_H

namespace pov
{

/*****************************************************************************
 * Global preprocessor defines
 ******************************************************************************/

#define OVUS_OBJECT (STURM_OK_OBJECT)


/*****************************************************************************
 * Global typedefs
 ******************************************************************************/

class Ovus : public ObjectBase
{
	public:

		Ovus();
		virtual ~Ovus();

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

		/// radius of bottom sphere (provided in SDL)
		DBL BottomRadius;
		/// radius of top sphere (provided in SDL)
		DBL TopRadius;

		/// horizontal position of center of connecting surface (computed)
		DBL HorizontalPosition;
		/// vertical position of center of connecting surface (computed)
		DBL VerticalPosition;
		/// lowest vertical for the connecting surface (computed)
		DBL BottomVertical;
		/// highest vertical for the connecting surface (computed)
		DBL TopVertical;
		/// Radius of the connecting surface (computed)
		DBL ConnectingRadius;

	private:
		void CalcUV(const VECTOR IPoint, UV_VECT Result) const;
		void Intersect_Ovus_Spheres(const VECTOR&, const VECTOR&,
		                            DBL * Depth1,DBL *Depth2, DBL * Depth3,
		                            DBL * Depth4, DBL * Depth5, DBL * Depth6,
		                            SceneThreadData *Thread) const;

};

}

#endif
