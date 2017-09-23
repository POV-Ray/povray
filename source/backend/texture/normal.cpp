/*******************************************************************************
 * normal.cpp
 *
 * This module implements solid texturing functions that perturb the surface
 * normal to create a bumpy effect. 
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
 * $File: //depot/public/povray/3.x/source/backend/texture/normal.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

/*
 * Some texture ideas garnered from SIGGRAPH '85 Volume 19 Number 3,
 * "An Image Synthesizer" By Ken Perlin.
 *
 * Further Ideas Garnered from "The RenderMan Companion" (Addison Wesley)
 */

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/texture/normal.h"
#include "backend/texture/texture.h"
#include "backend/texture/pigment.h"
#include "backend/support/imageutil.h"
#include "backend/scene/objects.h"
#include "backend/scene/threaddata.h"
#include "backend/math/vector.h"
#include "base/pov_err.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local constants
******************************************************************************/

static const VECTOR Pyramid_Vect [4]= {{ 0.942809041,-0.333333333, 0.0},
                                       {-0.471404521,-0.333333333, 0.816496581},
                                       {-0.471404521,-0.333333333,-0.816496581},
                                       { 0.0        , 1.0        , 0.0}};

/*****************************************************************************
* Static functions
******************************************************************************/

static void ripples (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR Vector, const TraceThreadData *Thread);
static void waves (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR Vector, const TraceThreadData *Thread);
static void bumps (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal);
static void dents (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal, const TraceThreadData *Thread);
static void wrinkles (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal);
static void quilted (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal);
static DBL Hermite_Cubic (DBL T1, const UV_VECT UV1, const UV_VECT UV2);
static DBL Do_Slope_Map (DBL value, const BLEND_MAP *Blend_Map);
static void Do_Average_Normals (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal, Intersection *Inter, const Ray *ray, TraceThreadData *Thread);
static void facets (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal, TraceThreadData *Thread);

/*****************************************************************************
*
* FUNCTION
*
*   ripples
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void ripples (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal, const TraceThreadData *Thread)
{
	unsigned int i;
	DBL length, scalar, index;
	VECTOR point;

	for (i = 0 ; i < Thread->numberOfWaves ; i++)
	{
		VSub (point, EPoint, *Thread->waveSources[i]);
		VLength (length, point);

		if (length == 0.0)
			length = 1.0;

		index = length * Tnormal->Frequency + Tnormal->Phase;

		scalar = cycloidal(index) * Tnormal ->Amount;

		VAddScaledEq(normal, scalar / (length * (DBL)Thread->numberOfWaves), point);
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   waves
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void waves (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal, const TraceThreadData *Thread)
{
	unsigned int i;
	DBL length, scalar, index, sinValue ;
	VECTOR point;

	for (i = 0 ; i < Thread->numberOfWaves ; i++)
	{
		VSub (point, EPoint, *Thread->waveSources[i]);

		VLength (length, point);

		if (length == 0.0)
		{
			length = 1.0;
		}

		index = length * Tnormal->Frequency * Thread->waveFrequencies[i] + Tnormal->Phase;

		sinValue = cycloidal(index);

		scalar = sinValue * Tnormal->Amount / Thread->waveFrequencies[i];

		VAddScaledEq(normal, scalar / (length * (DBL)Thread->numberOfWaves), point);
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   bumps
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void bumps (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal)
{
	VECTOR bump_turb;

	/* Get normal displacement value. */

	DNoise (bump_turb, EPoint);

	/* Displace "normal". */

	VAddScaledEq(normal, Tnormal->Amount, bump_turb);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*   Dents is similar to bumps, but uses noise() to control the amount of
*   dnoise() perturbation of the object normal...
*
* CHANGES
*
******************************************************************************/

static void dents (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal, const TraceThreadData *Thread)
{
	DBL noise;
	VECTOR stucco_turb;

	noise = Noise (EPoint, GetNoiseGen(reinterpret_cast<const TPATTERN *>(Tnormal), Thread));

	noise = noise * noise * noise * Tnormal->Amount;

	/* Get normal displacement value. */

	DNoise(stucco_turb, EPoint);

	/* Displace "normal". */

	VAddScaledEq(normal, noise, stucco_turb);
}




/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Wrinkles - This is my implementation of the dented() routine, using
*   a surface iterative fractal derived from DTurbulence.
*
*   This is a 3-D version (thanks to DNoise()...) of the usual version
*   using the singular Noise()...
*
*   Seems to look a lot like wrinkles, however... (hmmm)
*
*   Idea garnered from the April 89 Byte Graphics Supplement on RenderMan,
*   refined from "The RenderMan Companion, by Steve Upstill of Pixar,
*   (C) 1990 Addison-Wesley.
*
* CHANGES
*
******************************************************************************/

static void wrinkles (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal)
{
	int i;
	DBL scale = 1.0;
	VECTOR result, value, value2;

	Make_Vector(result, 0.0, 0.0, 0.0);

	for (i = 0; i < 10; scale *= 2.0, i++)
	{
		VScale(value2,EPoint,scale);
		DNoise(value, value2);

		result[X] += fabs(value[X] / scale);
		result[Y] += fabs(value[Y] / scale);
		result[Z] += fabs(value[Z] / scale);
	}

	/* Displace "normal". */

	VAddScaledEq(normal, Tnormal->Amount, result);
}


/*****************************************************************************
*
* FUNCTION
*
*   quilted
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dan Farmer '94
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void quilted (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal)
{
	VECTOR value;
	DBL t;

	value[X] = EPoint[X]-FLOOR(EPoint[X])-0.5;
	value[Y] = EPoint[Y]-FLOOR(EPoint[Y])-0.5;
	value[Z] = EPoint[Z]-FLOOR(EPoint[Z])-0.5;

	t = sqrt(value[X]*value[X]+value[Y]*value[Y]+value[Z]*value[Z]);

	t = quilt_cubic(t, Tnormal->Vals.Quilted.Control0, Tnormal->Vals.Quilted.Control1);

	value[X] *= t;
	value[Y] *= t;
	value[Z] *= t;

	VAddScaledEq (normal, Tnormal->Amount,value);
}

/*****************************************************************************
*
* FUNCTION
*
*   facets
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Ronald Parker '98
*   
* DESCRIPTION
* 
*   This pattern is based on the "Crackle" pattern and creates a faceted
*   look on a curved surface.
*
* CHANGES
*
******************************************************************************/

static void facets (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal, TraceThreadData *Thread)
{
	int    i;
	int    thisseed;
	DBL    sum, minsum;
	VECTOR sv, tv, dv, t1, add, newnormal, pert;
	DBL    scale;
	int    UseSquare;
	int    UseUnity;
	DBL    Metric;

	VECTOR *cv = Thread->Facets_Cube;
	Metric = Tnormal->Vals.Facets.Metric;

	UseSquare = (Metric == 2 );
	UseUnity  = (Metric == 1 );

	VNormalize( normal, normal );

	if ( Tnormal->Vals.Facets.UseCoords )
	{
		Assign_Vector(tv,EPoint);
	}
	else
	{
		Assign_Vector(tv,normal);
	}

	if ( Tnormal->Vals.Facets.Size < 1e-6 )
	{
		scale = 1e6;
	}
	else
	{
		scale = 1. / Tnormal->Vals.Facets.Size;
	}

	VScaleEq( tv, scale );

	/*
	 * Check to see if the input point is in the same unit cube as the last
	 * call to this function, to use cache of cubelets for speed.
	 */

	thisseed = PickInCube(tv, t1);

	if (thisseed != Thread->Facets_Last_Seed)
	{
		/*
		 * No, not same unit cube.  Calculate the random points for this new
		 * cube and its 80 neighbours which differ in any axis by 1 or 2.
		 * Why distance of 2?  If there is 1 point in each cube, located
		 * randomly, it is possible for the closest random point to be in the
		 * cube 2 over, or the one two over and one up.  It is NOT possible
		 * for it to be two over and two up.  Picture a 3x3x3 cube with 9 more
		 * cubes glued onto each face.
		 */

		/* Now store a points for this cube and each of the 80 neighbour cubes. */

		int cvc = 0;

		for (add[X] = -2.0; add[X] < 2.5; add[X] +=1.0)
		{
			for (add[Y] = -2.0; add[Y] < 2.5; add[Y] += 1.0)
			{
				for (add[Z] = -2.0; add[Z] < 2.5; add[Z] += 1.0)
				{
					/* For each cubelet in a 5x5 cube. */

					if ((fabs(add[X])>1.5)+(fabs(add[Y])>1.5)+(fabs(add[Z])>1.5) <= 1.0)
					{
						/* Yes, it's within a 3d knight move away. */

						VAdd(sv, tv, add);

						PickInCube(sv, t1);

						cv[cvc][X] = t1[X];
						cv[cvc][Y] = t1[Y];
						cv[cvc][Z] = t1[Z];
						cvc++;
					}
				}
			}
		}

		Thread->Facets_Last_Seed = thisseed;
		Thread->Facets_CVC = cvc;
	}

	/*
	 * Find the point with the shortest distance from the input point.
	 */

	VSub(dv, cv[0], tv);
	if ( UseSquare )
	{
		minsum  = VSumSqr(dv);
	}
	else if ( UseUnity )
	{
		minsum = dv[X]+dv[Y]+dv[Z];
	}
	else
	{
		minsum = pow(fabs(dv[X]), Metric)+
		         pow(fabs(dv[Y]), Metric)+
		         pow(fabs(dv[Z]), Metric);
	}

	Assign_Vector( newnormal, cv[0] );

	/* Loop for the 81 cubelets to find closest. */

	for (i = 1; i < Thread->Facets_CVC; i++)
	{
		VSub(dv, cv[i], tv);

		if ( UseSquare )
		{
			sum  = VSumSqr(dv);
		}
		else
		{
			if ( UseUnity )
			{
				sum = dv[X]+dv[Y]+dv[Z];
			}
			else
			{
				sum = pow(fabs(dv[X]), Metric)+
				      pow(fabs(dv[Y]), Metric)+
				      pow(fabs(dv[Z]), Metric);
			}
		}

		if (sum < minsum)
		{
			minsum = sum;
			Assign_Vector( newnormal, cv[i] );
		}
	}

	if ( Tnormal->Vals.Facets.UseCoords )
	{
		DNoise( pert, newnormal );
		VDot( sum, pert, normal );
		VScale( newnormal, normal, sum );
		VSubEq( pert, newnormal );
		VAddScaledEq( normal, Tnormal->Vals.Facets.UseCoords, pert );
	}
	else
	{
		Assign_Vector( normal, newnormal );
	}
}

/*****************************************************************************
*
* FUNCTION
*
*   Create_Tnormal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*
*   pointer to the created Tnormal
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION   : Allocate memory for new Tnormal and initialize it to
*                 system default values.
*
* CHANGES
*
******************************************************************************/


TNORMAL *Create_Tnormal ()
{
	TNORMAL *New;

	New = reinterpret_cast<TNORMAL *>(POV_MALLOC(sizeof(TNORMAL), "normal"));

	Init_TPat_Fields(reinterpret_cast<TPATTERN *>(New));

	New->Amount = 0.5;

	/* NK delta */
	New->Delta = (float)0.02; /* this is a good starting point for delta */

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Tnormal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TNORMAL *Copy_Tnormal (const TNORMAL *Old)
{
	TNORMAL *New;

	if (Old != NULL)
	{
		New = Create_Tnormal();

		Copy_TPat_Fields (reinterpret_cast<TPATTERN *>(New), reinterpret_cast<const TPATTERN *>(Old));

		New->Amount = Old->Amount;
		New->Delta = Old->Delta;
	}
	else
	{
		New = NULL;
	}

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Tnormal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Destroy_Tnormal(TNORMAL *Tnormal)
{
	if (Tnormal != NULL)
	{
		Destroy_TPat_Fields (reinterpret_cast<TPATTERN *>(Tnormal));

		POV_FREE(Tnormal);
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   Post_Tnormal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Post_Tnormal (TNORMAL *Tnormal)
{
	int i;
	BLEND_MAP *Map;

	if (Tnormal != NULL)
	{
		if (Tnormal->Flags & POST_DONE)
		{
			return;
		}

		if (Tnormal->Type == NO_PATTERN)
		{
			throw POV_EXCEPTION_STRING("No normal type given.");
		}

		Tnormal->Flags |= POST_DONE;

		if ((Map = Tnormal->Blend_Map) != NULL)
		{
			for (i = 0; i < Map->Number_Of_Entries; i++)
			{
				switch (Map->Type)
				{
					case PIGMENT_TYPE:

						Post_Pigment(Map->Blend_Map_Entries[i].Vals.Pigment);

						break;

					case NORMAL_TYPE:
						Map->Blend_Map_Entries[i].Vals.Tnormal->Flags |=
							(Tnormal->Flags & DONT_SCALE_BUMPS_FLAG);

						Post_Tnormal(Map->Blend_Map_Entries[i].Vals.Tnormal);

						break;

					case TEXTURE_TYPE:

						Post_Textures(Map->Blend_Map_Entries[i].Vals.Texture);

						break;

					case SLOPE_TYPE:
					case COLOUR_TYPE:
					case PATTERN_TYPE:

						break;

					default:

						throw POV_EXCEPTION_STRING("Unknown pattern type in Post_Tnormal.");
				}
			}
		}
	}
}



/*****************************************************************************
*
* FUNCTION
*
*   Perturb_Normal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*    Added intersectin parameter for UV mapping - NK 1998
*
******************************************************************************/

void Perturb_Normal(VECTOR Layer_Normal, const TNORMAL *Tnormal, const VECTOR EPoint, Intersection *Intersection, const Ray *ray, TraceThreadData *Thread)
{
	VECTOR TPoint,P1;
	DBL value1,value2,Amount;
	int i;
	const BLEND_MAP *Blend_Map;
	const BLEND_MAP_ENTRY *Prev, *Cur;

	if (Tnormal==NULL)
	{
		return;
	}

	/* If normal_map present, use it and return */

	if ((Blend_Map=Tnormal->Blend_Map) != NULL)
	{
		if ((Blend_Map->Type == NORMAL_TYPE) && (Tnormal->Type == UV_MAP_PATTERN))
		{
			UV_VECT UV_Coords;

			Cur = &(Tnormal->Blend_Map->Blend_Map_Entries[0]);

			/* Don't bother warping, simply get the UV vect of the intersection */
			Intersection->Object->UVCoord(UV_Coords, Intersection, Thread);
			TPoint[X] = UV_Coords[U];
			TPoint[Y] = UV_Coords[V];
			TPoint[Z] = 0;

			Perturb_Normal(Layer_Normal,Cur->Vals.Tnormal,TPoint,Intersection,ray,Thread);
			VNormalizeEq(Layer_Normal);
			Assign_Vector(Intersection->PNormal, Layer_Normal); /* -hdf- June 98 */

			return;
		}
		else if ((Blend_Map->Type == NORMAL_TYPE) && (Tnormal->Type != AVERAGE_PATTERN))
		{
			/* NK 19 Nov 1999 added Warp_EPoint */
			Warp_EPoint (TPoint, EPoint, reinterpret_cast<const TPATTERN *>(Tnormal));
			value1 = Evaluate_TPat(reinterpret_cast<const TPATTERN *>(Tnormal),TPoint,Intersection,ray,Thread);

			Search_Blend_Map (value1,Blend_Map,&Prev,&Cur);

			Warp_Normal(Layer_Normal,Layer_Normal, reinterpret_cast<const TPATTERN *>(Tnormal), Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));
			Assign_Vector(P1,Layer_Normal);

			Warp_EPoint (TPoint, EPoint, reinterpret_cast<const TPATTERN *>(Tnormal));

			Perturb_Normal(Layer_Normal,Cur->Vals.Tnormal,TPoint,Intersection,ray,Thread);

			if (Prev != Cur)
			{
				Perturb_Normal(P1,Prev->Vals.Tnormal,TPoint,Intersection,ray,Thread);

				value2 = (value1-Prev->value)/(Cur->value-Prev->value);
				value1 = 1.0-value2;

				VLinComb2(Layer_Normal,value1,P1,value2,Layer_Normal);
			}

			UnWarp_Normal(Layer_Normal,Layer_Normal, reinterpret_cast<const TPATTERN *>(Tnormal), Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));

			VNormalizeEq(Layer_Normal);

			Assign_Vector(Intersection->PNormal, Layer_Normal); /* -hdf- June 98 */

			return;
		}
	}

	/* No normal_map. */

	if (Tnormal->Type <= LAST_NORM_ONLY_PATTERN)
	{
		Warp_Normal(Layer_Normal,Layer_Normal, reinterpret_cast<const TPATTERN *>(Tnormal),
		            Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));

		Warp_EPoint (TPoint, EPoint, reinterpret_cast<const TPATTERN *>(Tnormal));

		switch (Tnormal->Type)
		{
			case BITMAP_PATTERN:    bump_map    (TPoint, Tnormal, Layer_Normal);            break;
			case BUMPS_PATTERN:     bumps       (TPoint, Tnormal, Layer_Normal);            break;
			case DENTS_PATTERN:     dents       (TPoint, Tnormal, Layer_Normal, Thread);    break;
			case RIPPLES_PATTERN:   ripples     (TPoint, Tnormal, Layer_Normal, Thread);    break;
			case WAVES_PATTERN:     waves       (TPoint, Tnormal, Layer_Normal, Thread);    break;
			case WRINKLES_PATTERN:  wrinkles    (TPoint, Tnormal, Layer_Normal);            break;
			case QUILTED_PATTERN:   quilted     (TPoint, Tnormal, Layer_Normal);            break;
			case FACETS_PATTERN:    facets      (TPoint, Tnormal, Layer_Normal, Thread);    break;
			case AVERAGE_PATTERN:   Do_Average_Normals (TPoint, Tnormal, Layer_Normal, Intersection, ray, Thread); break;
			default:
				throw POV_EXCEPTION_STRING("Normal pattern not yet implemented.");
		}

		UnWarp_Normal(Layer_Normal,Layer_Normal, reinterpret_cast<const TPATTERN *>(Tnormal),
		              Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));
	}
	else
	{
		Warp_Normal(Layer_Normal,Layer_Normal, reinterpret_cast<const TPATTERN *>(Tnormal),
		            Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));

		Amount=Tnormal->Amount * -5.0; /*fudge factor*/
		Amount*=0.02/Tnormal->Delta; /* NK delta */

		/* warp the center point first - this is the last warp */
		Warp_EPoint(TPoint,EPoint,reinterpret_cast<const TPATTERN *>(Tnormal));

		for(i=0; i<=3; i++)
		{
			VAddScaled(P1,TPoint,Tnormal->Delta,Pyramid_Vect[i]); /* NK delta */
			value1 = Do_Slope_Map(Evaluate_TPat(reinterpret_cast<const TPATTERN *>(Tnormal),P1,Intersection,ray,Thread),Blend_Map);
			VAddScaledEq(Layer_Normal,value1*Amount,Pyramid_Vect[i]);
		}

		UnWarp_Normal(Layer_Normal,Layer_Normal,reinterpret_cast<const TPATTERN *>(Tnormal),
		              Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));

	}

	if ( Intersection )
		Assign_Vector(Intersection->PNormal, Layer_Normal); /* -hdf- June 98 */
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static DBL Do_Slope_Map (DBL value, const BLEND_MAP *Blend_Map)
{
	DBL Result;
	const BLEND_MAP_ENTRY *Prev, *Cur;

	if (Blend_Map == NULL)
	{
		return(value);
	}

	Search_Blend_Map (value,Blend_Map,&Prev,&Cur);

	if (Prev == Cur)
	{
		return(Cur->Vals.Point_Slope[0]);
	}

	Result = (value-Prev->value)/(Cur->value-Prev->value);

	return(Hermite_Cubic(Result, Prev->Vals.Point_Slope, Cur->Vals.Point_Slope));
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static DBL Hermite_Cubic(DBL T1, const UV_VECT UV1, const UV_VECT UV2)
{
	DBL TT=T1*T1;
	DBL TTT=TT*T1;
	DBL rv;        /* simplified equation for poor Symantec */

	rv  = TTT*(UV1[1]+UV2[1]+2.0*(UV1[0]-UV2[0]));
	rv += -TT*(2.0*UV1[1]+UV2[1]+3.0*(UV1[0]-UV2[0]));
	rv += T1*UV1[1] +UV1[0];

	return (rv);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*    Added intersectin parameter for UV mapping - NK 1998
*
******************************************************************************/

static void Do_Average_Normals (const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal, Intersection *Inter, const Ray *ray, TraceThreadData *Thread)
{
	int i;
	BLEND_MAP *Map = Tnormal->Blend_Map;
	SNGL Value;
	SNGL Total = 0.0;
	VECTOR V1,V2;

	Make_Vector (V1, 0.0, 0.0, 0.0);

	for (i = 0; i < Map->Number_Of_Entries; i++)
	{
		Value = Map->Blend_Map_Entries[i].value;

		Assign_Vector(V2,normal);

		Perturb_Normal(V2,Map->Blend_Map_Entries[i].Vals.Tnormal,EPoint,Inter,ray,Thread);

		VAddScaledEq(V1,Value,V2);

		Total += Value;
	}

	VInverseScale(normal,V1,Total);
}

}
