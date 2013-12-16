/*******************************************************************************
 * sphsweep.cpp
 *
 * Contributed by Jochen Lippert
 *
 * This module implements the sphere sweep primitive. Idea: J.J. Van Wijk,
 * "Raytracing Objects Defined by Sweeping a Sphere", Eurographics 1984.
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
 * $File: //depot/povray/smp/source/backend/shape/sphsweep.cpp $
 * $Revision: #37 $
 * $Change: 6085 $
 * $DateTime: 2013/11/10 07:39:29 $
 * $Author: clipka $
 *******************************************************************************/

/*****************************************************************************
*
*  Version history (most recent changes first):
*
*  24. Feb. 1998: Fixed a bug in Compute_Sphere_Sweep_BBox that could
*                 result in much too large bounding boxes for
*                 Catmull-Rom-Spline Sphere Sweeps. Added statistics
*                 output for Sphere Sweep primitive.
*
*  19. Jan. 1998: Corrected a problem with the calculation of single
*                 spheres in Compute_Sphere_Sweep which I introduced
*                 in the previous version, and cleaned up the code in
*                 this function a bit. All_Sphere_Sweep_Intersections
*                 now uses QSORT to sort the intersection list.
*
*  6.  Jan. 1998: Fixed a bug that affected the geometry of
*                 Catmull-Rom-Spline Sphere Sweeps, fixed a bug in
*                 Inside_Sphere_Sweep which affected non-proportionally
*                 scaled Sphere Sweeps.
*
*  22. Dec. 1997: Simplyfied code for finding invalid intersections,
*                 fixed some bugs related to this.
*
*  8.  Dec. 1997: Some cleanup & bug fixes.
*
*  1.  Dec. 1997: Made depth tolerance user-selectable.
*
*  29. Nov. 1997: Added support for Catmull-Rom-Splines.
*
*  26. Nov. 1997: Fixed a bug where the wrong value was returned by
*                 All_Sphere_Sweep_Intersections when the clipped_by
*                 statement was used with a sphere sweep.
*
*  24. Nov. 1997: Fixed a pretty silly bug in Copy_Sphere_Sweep where
*                 the sphere sweep wasn't copied completely.
*
*  21. Nov. 1997: Origin release.
*
******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/math/vector.h"
#include "backend/bounding/bbox.h"
#include "backend/math/matrices.h"
#include "backend/scene/objects.h"
#include "backend/math/polysolv.h"
#include "backend/shape/sphsweep.h"
#include "backend/scene/threaddata.h"

#include <algorithm>

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

const DBL DEPTH_TOLERANCE       = 1.0e-6;
const DBL ZERO_TOLERANCE        = 1.0e-4;

/* Just to be sure use twice as much plus old maximum.  However,
   from looking at the code this may still not always be enough!
   It seems this depends on Num_Poly_Roots... [trf] */
#define SPHSWEEP_MAX_ISECT  ((Num_Spheres + Num_Segments) * 2 + 64)



/*****************************************************************************
* Local variables
******************************************************************************/

const MATRIX Catmull_Rom_Matrix =
{
	{ 0.0 / 2.0,  2.0 / 2.0,  0.0 / 2.0,  0.0 / 2.0},
	{-1.0 / 2.0,  0.0 / 2.0,  1.0 / 2.0,  0.0 / 2.0},
	{ 2.0 / 2.0, -5.0 / 2.0,  4.0 / 2.0, -1.0 / 2.0},
	{-1.0 / 2.0,  3.0 / 2.0, -3.0 / 2.0,  1.0 / 2.0}
};

const MATRIX B_Matrix =
{
	{ 1.0 / 6.0,  4.0 / 6.0,  1.0 / 6.0,  0.0 / 6.0},
	{-3.0 / 6.0,  0.0 / 6.0,  3.0 / 6.0,  0.0 / 6.0},
	{ 3.0 / 6.0, -6.0 / 6.0,  3.0 / 6.0,  0.0 / 6.0},
	{-1.0 / 6.0,  3.0 / 6.0, -3.0 / 6.0,  1.0 / 6.0}
};



/*****************************************************************************
*
* FUNCTION
*
*   All_Sphere_Sweep_Intersections
*
* INPUT
*
*   Object, Ray, Depth stack
*
* OUTPUT
*
*   Depth stack
*
* RETURNS
*
*   Boolean - found any intersections?
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Find all intersections of ray with a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool SphereSweep::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
	SPHSWEEP_INT    *Isect = reinterpret_cast<SPHSWEEP_INT *>(POV_MALLOC((sizeof(SPHSWEEP_INT) * SPHSWEEP_MAX_ISECT), "sphere sweep intersections"));
	SPHSWEEP_INT    *Sphere_Isect = reinterpret_cast<SPHSWEEP_INT *>(POV_MALLOC(sizeof(SPHSWEEP_INT) * 2 * Num_Spheres, "Sphere sweep sphere intersections"));
	SPHSWEEP_INT    *Segment_Isect = reinterpret_cast<SPHSWEEP_INT *>(POV_MALLOC(sizeof(SPHSWEEP_INT) * 12 * Num_Segments, "Sphere sweep segment intersections"));
	Ray             New_Ray;
	DBL             len;
	bool            Intersection_Found = false;
	int             Num_Isect = 0;
	int             Num_Seg_Isect;
	int             i, j;

	Thread->Stats()[Ray_Sphere_Sweep_Tests]++;

	if(Trans == NULL)
	{
		Assign_Vector(New_Ray.Origin, ray.Origin);
		Assign_Vector(New_Ray.Direction, ray.Direction);
	}
	else
	{
		MInvTransPoint(New_Ray.Origin, ray.Origin, Trans);
		MInvTransDirection(New_Ray.Direction, ray.Direction, Trans);

		VLength(len, New_Ray.Direction);
		VInverseScaleEq(New_Ray.Direction, len);
	}

	// Intersections with single spheres
	for(i = 0; i < Num_Spheres; i++)
	{
		// Are there intersections with this sphere?
		if(Intersect_Sphere(New_Ray, &Sphere[i], Sphere_Isect))
		{
			// Test for end of vector
			if(Num_Isect + 2 <= SPHSWEEP_MAX_ISECT)
			{
				// Valid intersection at Sphere_Isect[0]?
				if((Sphere_Isect[0].t > -MAX_DISTANCE) && (Sphere_Isect[0].t < MAX_DISTANCE))
				{
					// Add intersection
					Isect[Num_Isect] = Sphere_Isect[0];
					Num_Isect++;
				}

				// Valid intersection at Sphere_Isect[1]?
				if((Sphere_Isect[1].t > -MAX_DISTANCE) && (Sphere_Isect[1].t < MAX_DISTANCE))
				{
					// Add intersection
					Isect[Num_Isect] = Sphere_Isect[1];
					Num_Isect++;
				}
			}
		}
	}

	// Intersections with segments
	for(i = 0; i < Num_Segments; i++)
	{
		// Are there intersections with this segment?
		Num_Seg_Isect = Intersect_Segment(New_Ray, &Segment[i], Segment_Isect, Thread);

		// Test for end of vector
		if(Num_Isect + Num_Seg_Isect <= SPHSWEEP_MAX_ISECT)
		{
			for (j = 0; j < Num_Seg_Isect; j++)
			{
				// Add intersection
				Isect[Num_Isect] = Segment_Isect[j];
				Num_Isect++;
			}
		}
	}

	// Any intersections?
	if(Num_Isect > 0)
	{
		// Sort intersections
		QSORT(reinterpret_cast<void *>(Isect), Num_Isect, sizeof(SPHSWEEP_INT), Comp_Isects);

		// Delete invalid intersections inside the sphere sweep
		Num_Isect = Find_Valid_Points(Isect, Num_Isect, New_Ray);

		// Push valid intersections
		for (i = 0; i < Num_Isect; i++)
		{
			// Was the ray transformed?
			if (Trans != NULL)
			{
				// Yes, invert the transformation
				Isect[i].t /= len;
				MTransPoint(Isect[i].Point, Isect[i].Point, Trans);
				MTransNormal(Isect[i].Normal, Isect[i].Normal, Trans);
				VNormalizeEq(Isect[i].Normal);
			}

			// Check for positive values of t (it's a ray after all...)
			if(Isect[i].t > Depth_Tolerance)
			{
				// Test for clipping volume
				if (Clip.empty() || Point_In_Clip(Isect[i].Point, Clip, Thread))
				{
					Depth_Stack->push(Intersection(Isect[i].t, Isect[i].Point, Isect[i].Normal, this));
					Intersection_Found = true;
				}
			}
		}

		if(Intersection_Found)
			Thread->Stats()[Ray_Sphere_Sweep_Tests_Succeeded]++;
	}

	POV_FREE(Isect);
	POV_FREE(Sphere_Isect);
	POV_FREE(Segment_Isect);

	return Intersection_Found;
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Sphere_Sweep_Sphere
*
* INPUT
*
*   Ray     - Ray to test intersection with
*   Sphere  - Sphere
*   Inter   - Intersections (two element vector)
*   
* OUTPUT
*
*   -
*
* RETURNS
*
*   Boolean - is there at least one intersection?
*
* AUTHOR
*
*   Jochen Lippert
*   
* DESCRIPTION
*
*   Find intersections of ray (line in fact) with a sphere.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool SphereSweep::Intersect_Sphere(const Ray &ray, const SPHSWEEP_SPH *Sphere, SPHSWEEP_INT *Inter)
{
	DBL     Radius2;
	DBL     OCSquared;
	DBL     t_Closest_Approach;
	DBL     Half_Chord;
	DBL     t_Half_Chord_Squared;
	VECTOR  Origin_To_Center;

	Radius2 = Sqr(Sphere->Radius);

	VSub(Origin_To_Center, Sphere->Center, ray.Origin);

	VDot(OCSquared, Origin_To_Center, Origin_To_Center);

	VDot(t_Closest_Approach, Origin_To_Center, ray.Direction);

	if((OCSquared >= Radius2) && (t_Closest_Approach < EPSILON))
		return false;

	t_Half_Chord_Squared = Radius2 - OCSquared + Sqr(t_Closest_Approach);

	if(t_Half_Chord_Squared > EPSILON)
	{
		Half_Chord = sqrt(t_Half_Chord_Squared);

		// Calculate smaller depth
		Inter[0].t = t_Closest_Approach - Half_Chord;

		// Calculate point
		VEvaluateRay(Inter[0].Point, ray.Origin, Inter[0].t, ray.Direction);

		// Calculate normal
		VSub(Inter[0].Normal, Inter[0].Point, Sphere->Center);
		VInverseScaleEq(Inter[0].Normal, Sphere->Radius);

		// Calculate bigger depth
		Inter[1].t = t_Closest_Approach + Half_Chord;

		// Calculate point
		VEvaluateRay(Inter[1].Point, ray.Origin, Inter[1].t, ray.Direction);

		// Calculate normal
		VSub(Inter[1].Normal, Inter[1].Point, Sphere->Center);
		VInverseScaleEq(Inter[1].Normal, Sphere->Radius);

		return true;
	}

	return false;
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_Sphere_Sweep_Segment
*
* INPUT
*
*   Ray     - Ray to test intersection with
*   Segment - Segment of a sphere sweep
*   Isect   - intersection list (depth, point, normal)
*
* OUTPUT
*
*   Isect   - intersection list (depth, point, normal)
*
* RETURNS
*
*   Number of intersections
*
* AUTHOR
*
*   Jochen Lippert
*   
* DESCRIPTION
*
*   Find all intersections of a ray (line in fact) with one segment
*   of a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

int SphereSweep::Intersect_Segment(const Ray &ray, const SPHSWEEP_SEG *Segment, SPHSWEEP_INT *Isect, TraceThreadData *Thread)
{
	int     Isect_Count;
	DBL     Dot1, Dot2;
	DBL     t1, t2;
	VECTOR  Vector;
	VECTOR  IPoint;
	VECTOR  DCenter;
	DBL     DCenterDot;
	DBL     temp;
	DBL     b, c, d, e, f, g, h, i, j, k, l;
	DBL     Coef[11];
	DBL     Root[10];
	int     Num_Poly_Roots, m, n;
	DBL     fp0, fp1;
	DBL     t;
	SPHSWEEP_SPH    Temp_Sphere;
	SPHSWEEP_INT    Temp_Isect[2];
	VECTOR  Center;

	Isect_Count = 0;

	// Calculate intersections with closing surface for u = 0
	VDot(Dot1, ray.Direction, Segment->Center_Deriv[0]);
	if(fabs(Dot1) > EPSILON)
	{
		VSub(Vector, ray.Origin, Segment->Closing_Sphere[0].Center);
		VDot(Dot2, Vector, Segment->Center_Deriv[0]);
		t1 = -(Dot2 + Segment->Closing_Sphere[0].Radius * Segment->Radius_Deriv[0]) / Dot1;
		if ((t1 > -MAX_DISTANCE) && (t1 < MAX_DISTANCE))
		{
			// Calculate point
			VEvaluateRay(IPoint, ray.Origin, t1, ray.Direction);

			// Is the point inside the closing sphere?
			VSub(DCenter, IPoint, Segment->Closing_Sphere[0].Center);
			VDot(DCenterDot, DCenter, DCenter);
			if(DCenterDot < Sqr(Segment->Closing_Sphere[0].Radius))
			{
				// Add intersection
				Isect[Isect_Count].t = t1;

				// Copy point
				Assign_Vector(Isect[Isect_Count].Point, IPoint);

				// Calculate normal
				Assign_Vector(Isect[Isect_Count].Normal, Segment->Center_Deriv[0]);
				VScaleEq(Isect[Isect_Count].Normal, -1.0);
				VNormalizeEq(Isect[Isect_Count].Normal);

				Isect_Count++;
			}
		}
	}

	// Calculate intersections with closing surface for u = 1
	VDot(Dot1, ray.Direction, Segment->Center_Deriv[1]);
	if(fabs(Dot1) > EPSILON)
	{
		VSub(Vector, ray.Origin, Segment->Closing_Sphere[1].Center);
		VDot(Dot2, Vector, Segment->Center_Deriv[1]);
		t2 = -(Dot2 + Segment->Closing_Sphere[1].Radius * Segment->Radius_Deriv[1]) / Dot1;
		if((t2 > -MAX_DISTANCE) && (t2 < MAX_DISTANCE))
		{
			// Calculate point
			VEvaluateRay(IPoint, ray.Origin, t2, ray.Direction);

			// Is the point inside the closing sphere?
			VSub(DCenter, IPoint, Segment->Closing_Sphere[1].Center);
			VDot(DCenterDot, DCenter, DCenter);
			if(DCenterDot < Sqr(Segment->Closing_Sphere[1].Radius))
			{
				// Add intersection
				Isect[Isect_Count].t = t2;

				// Copy point
				Assign_Vector(Isect[Isect_Count].Point, IPoint);

				// Calculate normal
				Assign_Vector(Isect[Isect_Count].Normal, Segment->Center_Deriv[1]);
				VNormalizeEq(Isect[Isect_Count].Normal);

				Isect_Count++;
			}
		}
	}

	// Calculate intersections with sides of the segment

	switch (Segment->Num_Coefs)
	{
		case 2:   // First order Polynomial

			VSub(Vector, ray.Origin, Segment->Center_Coef[0]);

			// a is always 1.0

			VDot(b, Segment->Center_Coef[1], ray.Direction);
			b *= -2.0;

			VDot(c, Vector, ray.Direction);
			c *= 2.0;

			VDot(d, Segment->Center_Coef[1], Segment->Center_Coef[1]);
			d -= Sqr(Segment->Radius_Coef[1]);

			VDot(e, Vector, Segment->Center_Coef[1]);
			e += Segment->Radius_Coef[0] * Segment->Radius_Coef[1];
			e *= -2.0;

			VDot(f, Vector, Vector);
			f -= Sqr(Segment->Radius_Coef[0]);

			Coef[0] = 4.0 * Sqr(d) - Sqr(b) * d;
			Coef[1] = 4.0 * d * e - 2.0 * b * c * d;
			Coef[2] = Sqr(e) - b * c * e + Sqr(b) * f;

			Num_Poly_Roots = Solve_Polynomial(2, Coef, Root, true, 1e-10, Thread);
			break;
		case 4:   // Third order polynomial
			VSub(Vector, ray.Origin, Segment->Center_Coef[0]);

			// a is always 1.0

			VDot(b, Segment->Center_Coef[3], ray.Direction);
			b *= -2.0;

			VDot(c, Segment->Center_Coef[2], ray.Direction);
			c *= -2.0;

			VDot(d, Segment->Center_Coef[1], ray.Direction);
			d *= -2.0;

			VDot(e, Vector, ray.Direction);
			e *= 2.0;

			VDot(f, Segment->Center_Coef[3], Segment->Center_Coef[3]);
			f -= Sqr(Segment->Radius_Coef[3]);

			VDot(g, Segment->Center_Coef[3], Segment->Center_Coef[2]);
			g -= Segment->Radius_Coef[3] * Segment->Radius_Coef[2];
			g *= 2.0;

			VDot(h, Segment->Center_Coef[3], Segment->Center_Coef[1]);
			h *= 2.0;
			VDot(temp, Segment->Center_Coef[2], Segment->Center_Coef[2]);
			h += temp;
			h -= 2.0 * Segment->Radius_Coef[3] * Segment->Radius_Coef[1];
			h -= Sqr(Segment->Radius_Coef[2]);

			VDot(i, Segment->Center_Coef[3], Vector);
			VDot(temp, Segment->Center_Coef[2], Segment->Center_Coef[1]);
			i -= temp;
			i += Segment->Radius_Coef[3] * Segment->Radius_Coef[0];
			i += Segment->Radius_Coef[2] * Segment->Radius_Coef[1];
			i *= -2.0;

			VDot(j, Segment->Center_Coef[2], Vector);
			j += Segment->Radius_Coef[2] * Segment->Radius_Coef[0];
			j *= -2.0;
			VDot(temp, Segment->Center_Coef[1], Segment->Center_Coef[1]);
			j += temp;
			j -= Sqr(Segment->Radius_Coef[1]);

			VDot(k, Segment->Center_Coef[1], Vector);
			k += Segment->Radius_Coef[1] * Segment->Radius_Coef[0];
			k *= -2.0;

			VDot(l, Vector, Vector);
			l -= Sqr(Segment->Radius_Coef[0]);

			Coef[0] = 36.0 * Sqr(f) - 9.0 * f * Sqr(b);
			Coef[1] = 60.0 * f * g - 6.0 * g * Sqr(b) - 18.0 * b * c * f;
			Coef[2] = 48.0 * f * h + 25.0 * Sqr(g) - 3.0 * h * Sqr(b)
			        - 13.0 * b * c * g - 8.0 * f * Sqr(c) - 18.0 * b * d * f;
			Coef[3] = 36.0 * f * i + 40.0 * g * h - 18.0 * b * f * e - 8.0 * b * c * h
			        - 6.0 * g * Sqr(c) - 14.0 * b * d * g - 14.0 * c * d * f;
			Coef[4] = 24.0 * f * j + 30.0 * g * i + 16.0 * Sqr(h) - 15.0 * b * g * e
			        - 12.0 * c * f * e + 3.0 * j * Sqr(b) - 3.0 * b * c * i - 4.0 * h * Sqr(c)
			        - 10.0 * b * d * h - 11.0 * c * d * g - 5.0 * f * Sqr(d);
			Coef[5] = 12.0 * f * k + 20.0 * g * j + 24.0 * h * i - 12.0 * b * h * e
			        - 10.0 * c * g * e - 6.0 * d * f * e + 6.0 * k * Sqr(b) + 2.0 * b * c * j
			        - 2.0 * i * Sqr(c) - 6.0 * b * d * i - 8.0 * c * d * h - 4.0 * g * Sqr(d);
			Coef[6] = 10.0 * g * k + 16.0 * h * j + 9.0 * Sqr(i) - 9.0 * b * i * e
			        - 8.0 * c * h * e - 5.0 * d * g * e + 9.0 * l * Sqr(b) + 7.0 * b * c * k
			        - 2.0 * b * d * j - 5.0 * c * d * i - 3.0 * h * Sqr(d);
			Coef[7] = 8.0 * h * k + 12.0 * i * j - 6.0 * b * j * e - 6.0 * c * i * e
			        - 4.0 * d * h * e + 12.0 * b * c * l + 2.0 * k * Sqr(c) + 2.0 * b * d * k
			        - 2.0 * c * d * j - 2.0 * i * Sqr(d);
			Coef[8] = 6.0 * i * k + 4.0 * Sqr(j) - 3.0 * b * k * e - 4.0 * c * j * e
			        - 3.0 * d * i * e + 4.0 * l * Sqr(c) + 6.0 * b * d * l
			        + c * d * k - j * Sqr(d);
			Coef[9] = 4.0 * j * k - 2.0 * c * k * e - 2.0 * d * j * e + 4.0 * c * d * l;
			Coef[10] = Sqr(k) - d * k * e + l * Sqr(d);

			Num_Poly_Roots = bezier_01(10, Coef, Root, true, 1e-10, Thread);
			break;
	}

	// Remove roots not in interval [0, 1]

	m = 0;
	while(m < Num_Poly_Roots)
	{
		if(Root[m] < 0.0 || Root[m] > 1.0)
		{
			for(n = m; n < Num_Poly_Roots - 1; n++)
				Root[n] = Root[n + 1];
			Num_Poly_Roots--;
		}
		else
			m++;
	}

	switch(Segment->Num_Coefs)
	{
		case 2:
			for(m = 0; m < Num_Poly_Roots; m++)
			{
				fp0 = 2.0 * d * Root[m]
					+ e;
				fp1 = b;

				if(fabs(fp1) > ZERO_TOLERANCE)
				{
					t = -fp0 / fp1;

					if((t > -MAX_DISTANCE) && (t < MAX_DISTANCE))
					{
						// Add intersection
						Isect[Isect_Count].t = t;

						// Calculate point
						VEvaluateRay(Isect[Isect_Count].Point, ray.Origin, t, ray.Direction);

						// Calculate normal
						VAddScaled(Center, Segment->Center_Coef[0], Root[m], Segment->Center_Coef[1]);
						VSub(Isect[Isect_Count].Normal, Isect[Isect_Count].Point, Center);
						VNormalizeEq(Isect[Isect_Count].Normal);

						Isect_Count++;
					}
				}
				else
				{
					// Calculate center of single sphere
					VAddScaled(Temp_Sphere.Center, Segment->Center_Coef[0], Root[m], Segment->Center_Coef[1]);

					// Calculate radius of single sphere
					Temp_Sphere.Radius = Segment->Radius_Coef[1] * Root[m] + Segment->Radius_Coef[0];

					// Calculate intersections
					if(Intersect_Sphere(ray, &Temp_Sphere, Temp_Isect))
					{
						// Add intersections
						Isect[Isect_Count] = Temp_Isect[0];
						Isect_Count++;

						Isect[Isect_Count] = Temp_Isect[1];
						Isect_Count++;
					}
				}
			}
			break;
		case 4:
			for (m = 0; m < Num_Poly_Roots; m++)
			{
				fp0 = 6.0 * f * Root[m] * Root[m] * Root[m] * Root[m] * Root[m]
				    + 5.0 * g * Root[m] * Root[m] * Root[m] * Root[m]
				    + 4.0 * h * Root[m] * Root[m] * Root[m]
				    + 3.0 * i * Root[m] * Root[m]
				    + 2.0 * j * Root[m]
				    + k;
				fp1 = 3.0 * b * Root[m] * Root[m]
				    + 2.0 * c * Root[m]
				    + d;

#if 0 // [CLi] preliminary workaround for FS#81
				if(fabs(fp1) > ZERO_TOLERANCE)
				{
					t = -fp0 / fp1;

					if((t > -MAX_DISTANCE) && (t < MAX_DISTANCE))
					{
						// Add intersection
						Isect[Isect_Count].t = t;

						// Calculate point
						VEvaluateRay(Isect[Isect_Count].Point, ray.Origin, t, ray.Direction);

						// Calculate normal
						VAddScaled(Center, Segment->Center_Coef[0], Root[m], Segment->Center_Coef[1]);
						VAddScaledEq(Center, Root[m] * Root[m], Segment->Center_Coef[2]);
						VAddScaledEq(Center, Root[m] * Root[m] * Root[m], Segment->Center_Coef[3]);
						VSub(Isect[Isect_Count].Normal, Isect[Isect_Count].Point, Center);
						VNormalizeEq(Isect[Isect_Count].Normal);

						Isect_Count++;
					}
				}
				else
#endif
				{
					// Calculate center of single sphere
					VAddScaled(Temp_Sphere.Center, Segment->Center_Coef[0], Root[m], Segment->Center_Coef[1]);
					VAddScaledEq(Temp_Sphere.Center, Root[m] * Root[m], Segment->Center_Coef[2]);
					VAddScaledEq(Temp_Sphere.Center, Root[m] * Root[m] * Root[m], Segment->Center_Coef[3]);

					// Calculate radius of single sphere
					Temp_Sphere.Radius = Segment->Radius_Coef[3] * Root[m] * Root[m] * Root[m]
					                   + Segment->Radius_Coef[2] * Root[m] * Root[m]
					                   + Segment->Radius_Coef[1] * Root[m]
					                   + Segment->Radius_Coef[0];

					// Calculate intersections
					if(Intersect_Sphere(ray, &Temp_Sphere, Temp_Isect))
					{
						// Add intersections
						Isect[Isect_Count] = Temp_Isect[0];
						Isect_Count++;

						Isect[Isect_Count] = Temp_Isect[1];
						Isect_Count++;
					}
				}
			}
			break;
	}

	return Isect_Count;
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Sphere_Sweep
*
* INPUT
*
*   Point, Object
*
* OUTPUT
*
*   -
*
* RETURNS
*
*   Boolean - is the point inside the sphere sweep?
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Test if point is inside sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

bool SphereSweep::Inside(const VECTOR IPoint, TraceThreadData *Thread) const
{
	int     inside;
	VECTOR  New_Point;
	int     i, j;
	VECTOR  Vector;
	DBL     temp;
	DBL     Coef[7];
	DBL     Root[6];
	int     Num_Poly_Roots;

	inside = false;

	if(Trans == NULL)
		Assign_Vector(New_Point, IPoint);
	else
		MInvTransPoint(New_Point, IPoint, Trans);

	switch(Interpolation)
	{
		case LINEAR_SPHERE_SWEEP:
			// For each segment...
			for(i = 0; i < Num_Segments; i++)
			{
				// Pre-calculate vector
				VSub(Vector, New_Point, Segment[i].Center_Coef[0]);

				// Coefficient for u^2
				VDot(Coef[0], Segment[i].Center_Coef[1],
				              Segment[i].Center_Coef[1]);
				Coef[0] -= Sqr(Segment[i].Radius_Coef[1]);

				// Coefficient for u^1
				VDot(Coef[1], Vector, Segment[i].Center_Coef[1]);
				Coef[1] += Segment[i].Radius_Coef[0]
				         * Segment[i].Radius_Coef[1];
				Coef[1] *= -2.0;

				// Coefficient for u^0
				VDot(Coef[2], Vector, Vector);
				Coef[2] -= Sqr(Segment[i].Radius_Coef[0]);

				// Find roots
				Num_Poly_Roots = Solve_Polynomial(2, Coef, Root, true, 1e-10, Thread);

				// Test for interval [0, 1]
				for(j = 0; j < Num_Poly_Roots; j++)
				{
					if(Root[j] >= 0.0 && Root[j] <= 1.0)
					{
						// At least one root inside interval,
						// so we are inside sphere sweep
						inside = true;
						break;
					}
				}
			}
			break;
		case CATMULL_ROM_SPLINE_SPHERE_SWEEP:
		case B_SPLINE_SPHERE_SWEEP:
			// For each segment...
			for(i = 0; i < Num_Segments; i++)
			{
				// Pre-calculate vector
				VSub(Vector, New_Point, Segment[i].Center_Coef[0]);

				// Coefficient for u^6
				VDot(Coef[0], Segment[i].Center_Coef[3],
				              Segment[i].Center_Coef[3]);
				Coef[0] -= Sqr(Segment[i].Radius_Coef[3]);

				// Coefficient for u^5
				VDot(Coef[1], Segment[i].Center_Coef[3],
				              Segment[i].Center_Coef[2]);
				Coef[1] -= Segment[i].Radius_Coef[3]
				         * Segment[i].Radius_Coef[2];
				Coef[1] *= 2.0;

				// Coefficient for u^4
				VDot(Coef[2], Segment[i].Center_Coef[3],
				              Segment[i].Center_Coef[1]);
				Coef[2] *= 2.0;
				VDot(temp, Segment[i].Center_Coef[2],
				           Segment[i].Center_Coef[2]);
				Coef[2] += temp;
				Coef[2] -= 2.0 * Segment[i].Radius_Coef[3]
				               * Segment[i].Radius_Coef[1];
				Coef[2] -= Sqr(Segment[i].Radius_Coef[2]);

				// Coefficient for u^3
				VDot(Coef[3], Segment[i].Center_Coef[3], Vector);
				VDot(temp, Segment[i].Center_Coef[2],
				           Segment[i].Center_Coef[1]);
				Coef[3] -= temp;
				Coef[3] += Segment[i].Radius_Coef[3]
				         * Segment[i].Radius_Coef[0];
				Coef[3] += Segment[i].Radius_Coef[2]
				         * Segment[i].Radius_Coef[1];
				Coef[3] *= -2.0;

				// Coefficient for u^2
				VDot(Coef[4], Segment[i].Center_Coef[2], Vector);
				Coef[4] += Segment[i].Radius_Coef[2]
				         * Segment[i].Radius_Coef[0];
				Coef[4] *= -2.0;
				VDot(temp, Segment[i].Center_Coef[1],
				           Segment[i].Center_Coef[1]);
				Coef[4] += temp;
				Coef[4] -= Sqr(Segment[i].Radius_Coef[1]);

				// Coefficient for u^1
				VDot(Coef[5], Segment[i].Center_Coef[1], Vector);
				Coef[5] += Segment[i].Radius_Coef[1]
				         * Segment[i].Radius_Coef[0];
				Coef[5] *= -2.0;

				// Coefficient for u^0
				VDot(Coef[6], Vector, Vector);
				Coef[6] -= Sqr(Segment[i].Radius_Coef[0]);

				// Find roots
				Num_Poly_Roots = bezier_01(6, Coef, Root, true, 1e-10, Thread);

				// Test for interval [0, 1]
				for(j = 0; j < Num_Poly_Roots; j++)
				{
					if(Root[j] >= 0.0 && Root[j] <= 1.0)
					{
						// At least one root inside interval,
						// so we are inside the sphere sweep
						inside = true;
						break;
					}
				}
			}
			break;
	}

	if(Test_Flag(this, INVERTED_FLAG))
		inside = !inside;

	return inside;
}



/*****************************************************************************
*
* FUNCTION
*
*   Sphere_Sweep_Normal
*
* INPUT
*
*   Object, Intersection
*
* OUTPUT
*
*   Normal
*
* RETURNS
*
*	-
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Calculate the surface normal of a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

void SphereSweep::Normal(VECTOR Result, Intersection *Inter, TraceThreadData *) const
{
	Assign_Vector(Result, Inter->INormal);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Sphere_Sweep
*
* INPUT
*
*   Object
*
* OUTPUT
*
*   -
*
* RETURNS
*
*   Copy of sphere sweep
*
* AUTHOR
*
*   Jochen Lippert
*   
* DESCRIPTION
*
*   Copy a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

ObjectPtr SphereSweep::Copy()
{
	SphereSweep *New = new SphereSweep();
	int         i;

	Destroy_Transform(New->Trans);
	*New = *this;

	New->Segment = NULL;
	New->Sphere = NULL;
	New->Interpolation = Interpolation;

	New->Num_Modeling_Spheres = Num_Modeling_Spheres;
	New->Modeling_Sphere = reinterpret_cast<SPHSWEEP_SPH *>(POV_MALLOC(Num_Modeling_Spheres * sizeof(SPHSWEEP_SPH), "modeling sphere"));
	for(i = 0; i < New->Num_Modeling_Spheres; i++)
		New->Modeling_Sphere[i] = Modeling_Sphere[i];

	New->Depth_Tolerance = Depth_Tolerance;

	New->Compute();

	New->Trans = Copy_Transform(Trans);

	return New;
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Sphere_Sweep
*
* INPUT
*
*   Object, Vector, Transformation
*
* OUTPUT
*
*   Object
*
* RETURNS
*
*   -
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Translate a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

void SphereSweep::Translate(const VECTOR Vector, const TRANSFORM *tr)
{
	if(Trans == NULL)
	{
		for(int i = 0; i < Num_Modeling_Spheres; i++)
			VAddEq(Modeling_Sphere[i].Center, Vector);
		Compute();
		Compute_BBox();
	}
	else
		Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Sphere_Sweep
*
* INPUT
*
*   Object, Vector, Transformation
*
* OUTPUT
*
*   Object
*
* RETURNS
*
*   -
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Rotate a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

void SphereSweep::Rotate(const VECTOR, const TRANSFORM *tr)
{
	if(Trans == NULL)
	{
		for (int i = 0; i < Num_Modeling_Spheres; i++)
			MTransPoint(Modeling_Sphere[i].Center, Modeling_Sphere[i].Center, tr);
		Compute();
		Compute_BBox();
	}
	else
		Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Sphere_Sweep
*
* INPUT
*
*   Object, Vector, Transformation
*
* OUTPUT
*
*   Object
*
* RETURNS
*
*   -
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Scale a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

void SphereSweep::Scale(const VECTOR Vector, const TRANSFORM *tr)
{
	if((Vector[X] != Vector[Y]) || (Vector[X] != Vector[Z]))
	{
		if(Trans == NULL)
			Trans = Create_Transform();
	}

	if(Trans == NULL)
	{
		for(int i = 0; i < Num_Modeling_Spheres; i++)
		{
			VScaleEq(Modeling_Sphere[i].Center, Vector[X]);
			Modeling_Sphere[i].Radius *= Vector[X];
		}
		Compute();
		Compute_BBox();
	}
	else
		Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Invert_Sphere_Sweep
*
* INPUT
*
*   Object
*
* OUTPUT
*
*   Object
*
* RETURNS
*
*   -
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Invert a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

void SphereSweep::Invert()
{
	Invert_Flag(this, INVERTED_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Sphere_Sweep
*
* INPUT
*
*	-
*
* OUTPUT
*
*	-
*
* RETURNS
*
*	Sphere_Sweep
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Create a new, "empty" sphere sweep (no modeling spheres).
*
* CHANGES
*
*   -
*
******************************************************************************/

SphereSweep::SphereSweep() : ObjectBase(SPHERE_SWEEP_OBJECT)
{
	Interpolation = -1;

	Num_Modeling_Spheres = 0;
	Modeling_Sphere = NULL;

	Num_Spheres = 0;
	Sphere = NULL;

	Num_Segments = 0;
	Segment = NULL;

	Depth_Tolerance = DEPTH_TOLERANCE;

	Trans = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Sphere_Sweep
*
* INPUT
*
*   Object, Transformation
*
* OUTPUT
*
*   Object
*
* RETURNS
*
*   -
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Transform a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

void SphereSweep::Transform(const TRANSFORM *tr)
{
	if(Trans == NULL)
		Trans = Create_Transform();

	Compose_Transforms(Trans, tr);

	Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Sphere_Sweep
*
* INPUT
*
*   Object
*
* OUTPUT
*
*   -
*
* RETURNS
*
*   -
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Free memory allocated for a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

SphereSweep::~SphereSweep()
{
	Destroy_Transform(Trans);

	POV_FREE(Modeling_Sphere);
	POV_FREE(Sphere);
	POV_FREE(Segment);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Sphere_Sweep_BBox
*
* INPUT
*
*   Sphere Sweep
*
* OUTPUT
*
*   Sphere Sweep
*
* RETURNS
*
*   -
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Calculate the bounding box of a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

void SphereSweep::Compute_BBox()
{
	VECTOR  mins;
	VECTOR  maxs;
	int     i;

	mins[X] = mins[Y] = mins[Z] = BOUND_HUGE;
	maxs[X] = maxs[Y] = maxs[Z] = -BOUND_HUGE;

	for(i = 0; i < Num_Modeling_Spheres; i++)
	{
		if(Interpolation == CATMULL_ROM_SPLINE_SPHERE_SWEEP)
		{
		// Make box a bit larger for Catmull-Rom-Spline sphere sweeps
			mins[X] = min(mins[X], Modeling_Sphere[i].Center[X]
			                 - 2 * Modeling_Sphere[i].Radius);
			mins[Y] = min(mins[Y], Modeling_Sphere[i].Center[Y]
			                 - 2 * Modeling_Sphere[i].Radius);
			mins[Z] = min(mins[Z], Modeling_Sphere[i].Center[Z]
			                 - 2 * Modeling_Sphere[i].Radius);
			maxs[X] = max(maxs[X], Modeling_Sphere[i].Center[X]
			                 + 2 * Modeling_Sphere[i].Radius);
			maxs[Y] = max(maxs[Y], Modeling_Sphere[i].Center[Y]
			                 + 2 * Modeling_Sphere[i].Radius);
			maxs[Z] = max(maxs[Z], Modeling_Sphere[i].Center[Z]
			                 + 2 * Modeling_Sphere[i].Radius);
		}
		else
		{
			mins[X] = min(mins[X], Modeling_Sphere[i].Center[X]
			                     - Modeling_Sphere[i].Radius);
			mins[Y] = min(mins[Y], Modeling_Sphere[i].Center[Y]
			                     - Modeling_Sphere[i].Radius);
			mins[Z] = min(mins[Z], Modeling_Sphere[i].Center[Z]
			                     - Modeling_Sphere[i].Radius);
			maxs[X] = max(maxs[X], Modeling_Sphere[i].Center[X]
			                     + Modeling_Sphere[i].Radius);
			maxs[Y] = max(maxs[Y], Modeling_Sphere[i].Center[Y]
			                     + Modeling_Sphere[i].Radius);
			maxs[Z] = max(maxs[Z], Modeling_Sphere[i].Center[Z]
			                     + Modeling_Sphere[i].Radius);
		}
	}

	Make_BBox_from_min_max(BBox, mins, maxs);

	if(Trans != NULL)
		Recompute_BBox(&BBox, Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Sphere_Sweep
*
* INPUT
*
*   Sphere sweep
*
* OUTPUT
*
*   Sphere sweep
*
* RETURNS
*
*   -
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Calculate the internal representation of a sphere sweep.
*
* CHANGES
*
*   -
*
******************************************************************************/

void SphereSweep::Compute()
{
	size_t  size;
	int     i;
	int     coef;
	int     msph;
	int     last_sph;
	int     last_seg;

	switch(Interpolation)
	{
		case LINEAR_SPHERE_SWEEP:
			// Allocate memory if neccessary
			if(Segment == NULL)
			{
				Num_Segments = Num_Modeling_Spheres - 1;
				size = Num_Segments * sizeof(SPHSWEEP_SEG);
				Segment = reinterpret_cast<SPHSWEEP_SEG *>(POV_MALLOC(size, "sphere sweep segments"));
			}

			// Calculate polynomials for each segment
			for(i = 0; i < Num_Segments; i++)
			{
				// Polynomial has two coefficients
				Segment[i].Num_Coefs = 2;

				// Coefficients for u^1
				VSub(Segment[i].Center_Coef[1],
				     Modeling_Sphere[i + 1].Center,
				     Modeling_Sphere[i].Center);

				Segment[i].Radius_Coef[1] =
				     Modeling_Sphere[i + 1].Radius -
				     Modeling_Sphere[i].Radius;

				// Coefficients for u^0
				Assign_Vector(Segment[i].Center_Coef[0],
				     Modeling_Sphere[i].Center);

				Segment[i].Radius_Coef[0] =
				     Modeling_Sphere[i].Radius;
			}
			break;
		case CATMULL_ROM_SPLINE_SPHERE_SWEEP:
			// Allocate memory if neccessary
			if(Segment == NULL)
			{
				Num_Segments = Num_Modeling_Spheres - 3;
				size = Num_Segments * sizeof(SPHSWEEP_SEG);
				Segment = reinterpret_cast<SPHSWEEP_SEG *>(POV_MALLOC(size, "sphere sweep segments"));
			}

			// Calculate polynomials for each segment
			for(i = 0; i < Num_Segments; i++)
			{
				// Polynomial has four coefficients
				Segment[i].Num_Coefs = 4;

				// Calculate coefficients
				for(coef = 0; coef < 4; coef++)
				{
					// Center
					VScale(Segment[i].Center_Coef[coef],
					       Modeling_Sphere[i].Center,
					       Catmull_Rom_Matrix[coef][0]);

					// Radius
					Segment[i].Radius_Coef[coef] =
					       Modeling_Sphere[i].Radius *
					       Catmull_Rom_Matrix[coef][0];

					for(msph = 1; msph < 4; msph++)
					{
						// Center
						VAddScaledEq(Segment[i].Center_Coef[coef],
						             Catmull_Rom_Matrix[coef][msph],
						             Modeling_Sphere[i + msph].Center);

						// Radius
						Segment[i].Radius_Coef[coef] +=
						             Catmull_Rom_Matrix[coef][msph] *
						             Modeling_Sphere[i + msph].Radius;
					}
				}
			}
			break;
		case B_SPLINE_SPHERE_SWEEP:
			// Allocate memory if neccessary
			if(Segment == NULL)
			{
				Num_Segments = Num_Modeling_Spheres - 3;
				size = Num_Segments * sizeof(SPHSWEEP_SEG);
				Segment = reinterpret_cast<SPHSWEEP_SEG *>(POV_MALLOC(size, "sphere sweep segments"));
			}

			// Calculate polynomials for each segment
			for(i = 0; i < Num_Segments; i++)
			{
				// Polynomial has four coefficients
				Segment[i].Num_Coefs = 4;

				// Calculate coefficients
				for(coef = 0; coef < 4; coef++)
				{
					// Center
					VScale(Segment[i].Center_Coef[coef],
					       Modeling_Sphere[i].Center,
					       B_Matrix[coef][0]);

					// Radius
					Segment[i].Radius_Coef[coef] =
					       Modeling_Sphere[i].Radius *
					       B_Matrix[coef][0];

					for(msph = 1; msph < 4; msph++)
					{
						// Center
						VAddScaledEq(Segment[i].Center_Coef[coef],
						             B_Matrix[coef][msph],
						             Modeling_Sphere[i + msph].Center);

						// Radius
						Segment[i].Radius_Coef[coef] +=
						             B_Matrix[coef][msph] *
						             Modeling_Sphere[i + msph].Radius;
					}
				}
			}
			break;
	}

	// Pre-calculate several constants

	for(i = 0; i < Num_Segments; i++)
	{
		// Calculate closing sphere for u = 0

		// Center
		Assign_Vector(Segment[i].Closing_Sphere[0].Center,
		              Segment[i].Center_Coef[0]);

		// Radius
		Segment[i].Closing_Sphere[0].Radius =
		              Segment[i].Radius_Coef[0];

		// Calculate derivatives for u = 0

		// Center
		Assign_Vector(Segment[i].Center_Deriv[0],
		              Segment[i].Center_Coef[1]);

		// Radius
		Segment[i].Radius_Deriv[0] =
		              Segment[i].Radius_Coef[1];

		// Calculate closing sphere for u = 1

		// Center
		Assign_Vector(Segment[i].Closing_Sphere[1].Center,
		              Segment[i].Center_Coef[0]);

		// Radius
		Segment[i].Closing_Sphere[1].Radius =
		              Segment[i].Radius_Coef[0];

		for(coef = 1; coef < Segment[i].Num_Coefs; coef++)
		{
			// Center
			VAddEq(Segment[i].Closing_Sphere[1].Center,
			       Segment[i].Center_Coef[coef]);

			// Radius
			Segment[i].Closing_Sphere[1].Radius +=
			       Segment[i].Radius_Coef[coef];
		}

		// Calculate derivatives for u = 1

		// Center
		Assign_Vector(Segment[i].Center_Deriv[1],
		              Segment[i].Center_Coef[1]);

		// Radius
		Segment[i].Radius_Deriv[1] =
		              Segment[i].Radius_Coef[1];

		for(coef = 2; coef < Segment[i].Num_Coefs; coef++)
		{
			// Center
			VAddScaledEq(Segment[i].Center_Deriv[1], coef,
			             Segment[i].Center_Coef[coef]);

			// Radius
			Segment[i].Radius_Deriv[1] += coef *
			             Segment[i].Radius_Coef[coef];
		}
	}

	// Calculate single spheres

	// Allocate memory if neccessary
	if(Sphere == NULL)
	{
		Num_Spheres = Num_Segments + 1;
		size = Num_Spheres * sizeof(SPHSWEEP_SPH);
		Sphere = reinterpret_cast<SPHSWEEP_SPH *>(POV_MALLOC(size, "sphere sweep spheres"));
	}

	// Calculate first sphere of every segment

	for(i = 0; i < Num_Segments; i++)
	{
		// Center
		Assign_Vector(Sphere[i].Center,
		              Segment[i].Center_Coef[0]);

		// Radius
		Sphere[i].Radius =
		              Segment[i].Radius_Coef[0];
	}

	// Calculate last sphere of last segment

	last_sph = Num_Spheres - 1;
	last_seg = Num_Segments - 1;

	// Center
	Assign_Vector(Sphere[last_sph].Center,
	              Segment[last_seg].Center_Coef[0]);
	// Radius
	Sphere[last_sph].Radius =
	              Segment[last_seg].Radius_Coef[0];

	for(coef = 1; coef < Segment[last_seg].Num_Coefs; coef++)
	{
		// Center
		VAddEq(Sphere[last_sph].Center,
		       Segment[last_seg].Center_Coef[coef]);

		// Radius
		Sphere[last_sph].Radius +=
		       Segment[last_seg].Radius_Coef[coef];
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   Find_Valid_Points
*
* INPUT
*
*   Intersection list, number of intersections, ray
*
* OUTPUT
*
*   Intersection list
*
* RETURNS
*
*   Number of valid intersections
*
* AUTHOR
*
*   Jochen Lippert
*
* DESCRIPTION
*
*   Delete invalid intersections.
*
* CHANGES
*
*   -
*
******************************************************************************/

int SphereSweep::Find_Valid_Points(SPHSWEEP_INT *Inter, int Num_Inter, const Ray &ray)
{
	int     i;
	int     j;
	int     Inside;
	int     Keep;
	DBL     NormalDotDirection;

	Inside = 1;
	i = 1;
	while(i < Num_Inter - 1)
	{
		// Angle between normal and ray
		VDot(NormalDotDirection, Inter[i].Normal, ray.Direction);

		// Does the ray enter the part?
		if(NormalDotDirection < 0.0)
		{
			// Ray enters part, keep intersection if ray was outside any part
			Keep = (Inside == 0);

			// increase inside counter
			Inside++;
		}
		else
		{
			// Ray exits part, keep intersection if ray was inside one part
			Keep = (Inside == 1);

			// decrease inside counter
			Inside--;
		}

		// Keep intersection?
		if(Keep)
			i++; // Yes, advance to next one
		else
		{
			// No, delete it
			for (j = i + 1; j < Num_Inter; j++)
				Inter[j - 1] = Inter[j];
			Num_Inter--;
		}
	}

	return Num_Inter;
}



/*****************************************************************************
*
* FUNCTION
*
*   Comp_Isects
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Jochen Lippert
*   
* DESCRIPTION
*
*   Compare the depths of two sphere sweep intersections.
*
* CHANGES
*
*   -
*
******************************************************************************/

int SphereSweep::Comp_Isects(const void *Intersection_1, const void *Intersection_2)
{
	const SPHSWEEP_INT *Int_1;
	const SPHSWEEP_INT *Int_2;

	Int_1 = reinterpret_cast<const SPHSWEEP_INT *>(Intersection_1);
	Int_2 = reinterpret_cast<const SPHSWEEP_INT *>(Intersection_2);

	if(Int_1->t < Int_2->t)
		return -1;

	if (Int_1->t == Int_2->t)
		return 0;
	else
		return 1;
}



/*****************************************************************************
*
* FUNCTION
*
*   bezier_01
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Massimo Valentini
*   
* DESCRIPTION
*
*   Optimization to exclude the presence of a root of a polynomial in (0 1).
*   The rendering time decreases impressively.
*
* CHANGES
*
*   -
*
******************************************************************************/

const int lcm_bezier_01[] =
{
	60,  10,  4,  3,  4, 10, 60,
	2520, 252, 56, 21, 12, 10, 12, 21, 56, 252, 2520
};

int SphereSweep::bezier_01(int degree, const DBL* Coef, DBL* Roots, bool sturm, DBL tolerance, TraceThreadData *Thread)
{
	DBL d[11];
	bool non_negative = true, non_positive = true;
	int i, j;
	const int *lcm = &(lcm_bezier_01[degree == 6 ? 0 : 7]);

	for(i = 0; i <= degree; ++i)
		d[i] = Coef[i] * lcm[i];

	for(i = 0; i <= degree; ++i)
	{
		non_negative = (non_negative && (d[degree - i] >= 0));
		non_positive = (non_positive && (d[degree - i] <= 0));

		if(!(non_negative || non_positive))
			return Solve_Polynomial(degree, Coef, Roots, sturm, tolerance, Thread);

		for(j = 0; j < degree - i; ++j)
			d[j] += d[j+1];
	}

	return 0;
}

}
