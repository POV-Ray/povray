/*******************************************************************************
 * pattern.cpp
 *
 * This module implements texturing functions that return a value to be
 * used in a pigment or normal.
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
 * $File: N/A $
 * $Revision: N/A $
 * $Change: N/A $
 * $DateTime: N/A $
 * $Author: N/A $
 *******************************************************************************/

/*
 * Some texture ideas garnered from SIGGRAPH '85 Volume 19 Number 3,
 * "An Image Synthesizer" By Ken Perlin.
 * Further Ideas Garnered from "The RenderMan Companion" (Addison Wesley).
 */

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/pattern/pattern.h"
#include "backend/texture/texture.h"
#include "backend/texture/pigment.h"
#include "backend/scene/objects.h"
#include "backend/scene/scene.h"
#include "backend/support/imageutil.h"
#include "backend/colour/colour_old.h"
#include "backend/parser/parse.h"
#include "backend/math/vector.h"
#include "backend/math/matrices.h"
#include "backend/vm/fnpovfpu.h"
#include "backend/support/fileutil.h"
#include "base/fileinputoutput.h"

#include <algorithm>
#include <climits>

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using namespace pov_base;

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define CLIP_DENSITY(r) { if((r) < 0.0) { (r) = 1.0; } else if((r) > 1.0) { (r) = 0.0; } else { (r) = 1.0 - (r); } }

const int FRACTAL_MAX_EXPONENT = 33;

/*****************************************************************************
* Local variables
******************************************************************************/

static RandomDoubleSequence PatternRands(0.0, 1.0, 32768);
static int BinomialCoefficients[((FRACTAL_MAX_EXPONENT+1)*(FRACTAL_MAX_EXPONENT+2))/2]; // GLOBAL VARIABLE
boost::hash<Crackle_Cell_Coord> Crackle_Cell_Hasher;
static int CrackleCubeTable[81*3];

/*****************************************************************************
* Static functions
******************************************************************************/

static DBL agate_pattern (const VECTOR EPoint, const TPATTERN *TPat, int noise_generator);
static DBL boxed_pattern (const VECTOR EPoint);
static DBL brick_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL cells_pattern (const VECTOR EPoint);
static DBL checker_pattern (const VECTOR EPoint);
static DBL crackle_pattern (const VECTOR EPoint, const TPATTERN *TPat, TraceThreadData *Thread);
static DBL cylindrical_pattern (const VECTOR EPoint);
static DBL dents_pattern (const VECTOR EPoint, int noise_generator);
static DBL density_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL function_pattern (const VECTOR EPoint, const TPATTERN *TPat, TraceThreadData *Thread);
static DBL gradient_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL granite_pattern (const VECTOR EPoint, int noise_generator);
static DBL hexagon_pattern (const VECTOR EPoint);
static DBL square_pattern (const VECTOR EPoint);
static DBL triangular_pattern (const VECTOR EPoint);
static DBL cubic_pattern (const VECTOR EPoint);
static DBL julia_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL julia3_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL julia4_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL juliax_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL leopard_pattern (const VECTOR EPoint);
static DBL magnet1m_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL magnet1j_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL magnet2m_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL magnet2j_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL mandel_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL mandel3_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL mandel4_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL mandelx_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL marble_pattern (const VECTOR EPoint, const TPATTERN *TPat, int noise_generator);
static DBL object_pattern (const VECTOR EPoint, const TPATTERN *TPat, TraceThreadData *Thread);
static DBL onion_pattern (const VECTOR EPoint);
static DBL pavement_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL pigment_pattern (const VECTOR EPoint, const TPATTERN *TPat, const Intersection *isect, const Ray *ray, TraceThreadData *Thread);
static DBL planar_pattern (const VECTOR EPoint);
static DBL quilted_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL radial_pattern (const VECTOR EPoint);
static DBL ripples_pattern (const VECTOR EPoint, const TPATTERN *TPat, const TraceThreadData *Thread);
static DBL slope_pattern (const VECTOR EPoint, const TPATTERN *TPat, const Intersection *Intersection);
static DBL aoi_pattern (const Intersection *Intersection, const Ray *ray);
static DBL spiral1_pattern (const VECTOR EPoint, const TPATTERN *TPat, int noise_generator);
static DBL spiral2_pattern (const VECTOR EPoint, const TPATTERN *TPat, int noise_generator);
static DBL spherical_pattern (const VECTOR EPoint);
static DBL tiling_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL waves_pattern (const VECTOR EPoint, const TPATTERN *TPat, const TraceThreadData *Thread);
static DBL wood_pattern (const VECTOR EPoint, const TPATTERN *TPat);
static DBL wrinkles_pattern (const VECTOR EPoint, int noise_generator);

static DBL fractal_exterior_color(const TPATTERN *TPat, int iters, DBL a, DBL b);
static DBL fractal_interior_color(const TPATTERN *TPat, int iters, DBL a, DBL b, DBL mindist2);
static const TURB *Search_For_Turb(const WARP *Warps);
static unsigned short readushort(IStream *infile);
static unsigned int readuint(IStream *infile);

#define SQRT3_2     0.86602540378443864676372317075294  ///< sqrt(3)/2
#define SQRT3       1.7320508075688772935274463415059   ///< sqrt(3)
#define SQRT2       1.4142135623730950488016887242097   ///< sqrt(2)
#define SQRT2_2     0.70710678118654752440084436210485  ///< sqrt(2)/2

#define SIN18       0.30901699437494742410229341718282  ///< sin(18 deg)
#define SIN36       0.58778525229247312916870595463907  ///< sin(36 deg)
#define SIN54       0.80901699437494742410229341718282  ///< sin(54 deg)
#define SIN72       0.95105651629515357211643933337938  ///< sin(72 deg)
#define SIN108      SIN72                               ///< sin(108 deg)
#define SIN144      SIN36                               ///< sin(144 deg)
#define SIN162      SIN18                               ///< sin(162 deg)
#define COS18       SIN72                               ///< cos(18 deg)
#define COS36       SIN54                               ///< cos(36 deg)
#define COS54       SIN36                               ///< cos(54 deg)
#define COS72       SIN18                               ///< cos(72 deg)
#define COS108      (-COS72)                            ///< cos(108 deg)
#define COS126      (-COS54)                            ///< cos(126 deg)
#define COS144      (-COS36)                            ///< cos(144 deg)
#define COS162      (-COS18)                            ///< cos(162 deg)
#define TAN18       0.32491969623290632615587141221513  ///< tan(18 deg)
#define TAN36       0.72654252800536088589546675748062  ///< tan(36 deg)
#define TAN54       1.3763819204711735382072095819109   ///< tan(54 deg)
#define TAN72       3.0776835371752534025702905760369   ///< tan(72 deg)
#define TAN108      (-TAN72)                            ///< tan(108 deg)
#define TAN126      (-TAN54)                            ///< tan(126 deg)
#define TAN144      (-TAN36)                            ///< tan(144 deg)
#define TAN162      (-TAN18)                            ///< tan(162 deg)

#define PHI         1.6180339887498948482045868343656   ///< golden ratio = (1+sqrt(5))/2
#define INVPHI      0.61803398874989484820458683436564  ///< inverse of golden ratio (= golden ratio -1)
#define SQRPHI      2.6180339887498948482045868343656   ///< square of golden ratio (= golden ratio +1)
#define INVSQRPHI   0.38196601125010515179541316563436  ///< inverse square of golden ratio (= 2 - golden ratio)

#define TILING_EPSILON 1e-6

/*****************************************************************************/

int GetNoiseGen (const TPATTERN *TPat, const TraceThreadData *Thread)
{
	int noise_gen = (TPat->Flags & NOISE_FLAGS) / NOISE_FLAG_1;
	if (!noise_gen)
		noise_gen = Thread->GetSceneData()->noiseGenerator;
	return noise_gen;
}

/*****************************************************************************
*
* FUNCTION
*
*   Evaluate_Pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*   Intersection - intersection structure
*   
* OUTPUT
*   
* RETURNS
*
*   DBL result usual 0.0 to 1.0 but may be 2.0 in hexagon
*   
* AUTHOR
*
*   Adapted from Add_Pigment by Chris Young
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Evaluate_TPat (const TPATTERN *TPat, const VECTOR EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread)
{
	DBL value = 0.0;

	/* NK 19 Nov 1999 removed Warp_EPoint call */

	switch(TPat->Type)
	{
		case AGATE_PATTERN:       value = agate_pattern      (EPoint, TPat, GetNoiseGen(TPat, Thread)); break;
		case BOZO_PATTERN:
		case SPOTTED_PATTERN:
		case BUMPS_PATTERN:       value = Noise              (EPoint,       GetNoiseGen(TPat, Thread)); break;
		case BRICK_PATTERN:       value = brick_pattern      (EPoint, TPat);                            break;
		case CELLS_PATTERN:       value = cells_pattern      (EPoint);                                  break;
		case CHECKER_PATTERN:     value = checker_pattern    (EPoint);                                  break;
		case CRACKLE_PATTERN:     value = crackle_pattern    (EPoint, TPat, Thread);                    break;
		case GRADIENT_PATTERN:    value = gradient_pattern   (EPoint, TPat);                            break;
		case GRANITE_PATTERN:     value = granite_pattern    (EPoint,       GetNoiseGen(TPat, Thread)); break;
		case HEXAGON_PATTERN:     value = hexagon_pattern    (EPoint);                                  break;
		case SQUARE_PATTERN:      value = square_pattern     (EPoint);                                  break;
		case TRIANGULAR_PATTERN:  value = triangular_pattern (EPoint);                                  break;
		case CUBIC_PATTERN:       value = cubic_pattern      (EPoint);                                  break;
		case JULIA_PATTERN:       value = julia_pattern      (EPoint, TPat);                            break;
		case JULIA3_PATTERN:      value = julia3_pattern     (EPoint, TPat);                            break;
		case JULIA4_PATTERN:      value = julia4_pattern     (EPoint, TPat);                            break;
		case JULIAX_PATTERN:      value = juliax_pattern     (EPoint, TPat);                            break;
		case LEOPARD_PATTERN:     value = leopard_pattern    (EPoint);                                  break;
		case MAGNET1M_PATTERN:    value = magnet1m_pattern   (EPoint, TPat);                            break;
		case MAGNET1J_PATTERN:    value = magnet1j_pattern   (EPoint, TPat);                            break;
		case MAGNET2M_PATTERN:    value = magnet2m_pattern   (EPoint, TPat);                            break;
		case MAGNET2J_PATTERN:    value = magnet2j_pattern   (EPoint, TPat);                            break;
		case MANDEL_PATTERN:      value = mandel_pattern     (EPoint, TPat);                            break;
		case MANDEL3_PATTERN:     value = mandel3_pattern    (EPoint, TPat);                            break;
		case MANDEL4_PATTERN:     value = mandel4_pattern    (EPoint, TPat);                            break;
		case MANDELX_PATTERN:     value = mandelx_pattern    (EPoint, TPat);                            break;
		case MARBLE_PATTERN:      value = marble_pattern     (EPoint, TPat, GetNoiseGen(TPat, Thread)); break;
		case ONION_PATTERN:       value = onion_pattern      (EPoint);                                  break;
		case RADIAL_PATTERN:      value = radial_pattern     (EPoint);                                  break;
		case SPIRAL1_PATTERN:     value = spiral1_pattern    (EPoint, TPat, GetNoiseGen(TPat, Thread)); break;
		case SPIRAL2_PATTERN:     value = spiral2_pattern    (EPoint, TPat, GetNoiseGen(TPat, Thread)); break;
		case WOOD_PATTERN:        value = wood_pattern       (EPoint, TPat);                            break;
		case WAVES_PATTERN:       value = waves_pattern      (EPoint, TPat, Thread);                    break;
		case RIPPLES_PATTERN:     value = ripples_pattern    (EPoint, TPat, Thread);                    break;
		case WRINKLES_PATTERN:    value = wrinkles_pattern   (EPoint,       GetNoiseGen(TPat, Thread)); break;
		case DENTS_PATTERN:       value = dents_pattern      (EPoint,       GetNoiseGen(TPat, Thread)); break;
		case QUILTED_PATTERN:     value = quilted_pattern    (EPoint, TPat);                            break;
		case FUNCTION_PATTERN:    value = function_pattern   (EPoint, TPat, Thread);                    break;
		case PLANAR_PATTERN:      value = planar_pattern     (EPoint);                                  break;
		case BOXED_PATTERN:       value = boxed_pattern      (EPoint);                                  break;
		case SPHERICAL_PATTERN:   value = spherical_pattern  (EPoint);                                  break;
		case CYLINDRICAL_PATTERN: value = cylindrical_pattern(EPoint);                                  break;
		case DENSITY_FILE_PATTERN:value = density_pattern    (EPoint, TPat);                            break;
		case IMAGE_PATTERN:       value = image_pattern      (EPoint, TPat);                            break;
		case SLOPE_PATTERN:       value = slope_pattern      (EPoint, TPat, Isection);                  break;
		case AOI_PATTERN:         value = aoi_pattern        (Isection, ray);                           break;
		case PAVEMENT_PATTERN:    value = pavement_pattern   (EPoint, TPat);                            break;
		case TILING_PATTERN:      value = tiling_pattern     (EPoint, TPat);                            break;
		case PIGMENT_PATTERN:     value = pigment_pattern    (EPoint, TPat, Isection, ray, Thread);     break;
		case OBJECT_PATTERN:      value = object_pattern     (EPoint, TPat, Thread);                    break;

		default: throw POV_EXCEPTION_STRING("Problem in Evaluate_TPat.");
	}

	if(TPat->Frequency != 0.0)
		value = fmod(value * TPat->Frequency + TPat->Phase, 1.00001); // TODO FIXME - magic number! Should be 1.0+SOME_EPSILON (or maybe actually 1.0?)

	/* allow negative Frequency */
	if(value < 0.0)
		value -= floor(value);

	switch(TPat->Wave_Type)
	{
		case RAMP_WAVE:
			break;
		case SINE_WAVE:
			value = (1.0 + cycloidal(value)) * 0.5;
			break;
		case TRIANGLE_WAVE:
			value = Triangle_Wave(value);
			break;
		case SCALLOP_WAVE:
			value = fabs(cycloidal(value * 0.5));
			break;
		case CUBIC_WAVE:
			value = Sqr(value) * ((-2.0 * value) + 3.0);
			break;
		case POLY_WAVE:
			value = pow(value, (DBL) TPat->Exponent);
			break;
		default:
			throw POV_EXCEPTION_STRING("Unknown Wave Type.");
	}

	return value;
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

void Init_TPat_Fields (TPATTERN *Tpat)
{
	Tpat->Type       = NO_PATTERN;
	Tpat->Wave_Type  = RAMP_WAVE;
	Tpat->Flags      = NO_FLAGS;
	Tpat->References = 1;
	Tpat->Exponent   = 1.0;
	Tpat->Frequency  = 1.0;
	Tpat->Phase      = 0.0;
	Tpat->Warps      = NULL;
	Tpat->Next       = NULL;
	Tpat->Blend_Map  = NULL;
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

void Copy_TPat_Fields (TPATTERN *New, const TPATTERN *Old)
{
	*New = *Old;

	/* Copy warp chain */
	New->Warps = Copy_Warps(Old->Warps);

	New->Blend_Map = Copy_Blend_Map(Old->Blend_Map);

	/* Note, cannot copy Old->Next because we don't know what kind of
	   thing this is.  It must be copied by Copy_Pigment, Copy_Tnormal etc.
	*/

	/* NK 1998 - added IMAGE_PATTERN */
	if ((Old->Type == BITMAP_PATTERN) || (Old->Type == IMAGE_PATTERN))
	{
		New->Vals.image = Copy_Image(Old->Vals.image);
	}

	if (Old->Type == DENSITY_FILE_PATTERN)
	{
		New->Vals.Density_File = Copy_Density_File(Old->Vals.Density_File);
	}

	if (Old->Type == PIGMENT_PATTERN )
	{
		New->Vals.Pigment = Copy_Pigment(Old->Vals.Pigment);
	}

	if (Old->Type == OBJECT_PATTERN)
	{
		if(Old->Vals.Object != NULL)
		{
			New->Vals.Object = reinterpret_cast<ObjectPtr>(Copy_Object(Old->Vals.Object));
		}
	}

	if (Old->Type == FUNCTION_PATTERN)
	{
		if (Old->Vals.Function.Fn != NULL)
		{
			New->Vals.Function.Fn = reinterpret_cast<void *>(Parser::Copy_Function( Old->Vals.Function.vm, (FUNCTION_PTR)(Old->Vals.Function.Fn) ));
		}
	}
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

void Destroy_TPat_Fields(TPATTERN *Tpat)
{
	Destroy_Warps(Tpat->Warps);
	Destroy_Blend_Map(Tpat->Blend_Map);
	/* Note, cannot destroy Tpat->Next nor pattern itself because we don't
	   know what kind of thing this is.  It must be destroied by Destroy_Pigment, etc.
	*/

	if ((Tpat->Type == BITMAP_PATTERN) || (Tpat->Type == IMAGE_PATTERN))
	{
		Destroy_Image(Tpat->Vals.image);
	}

	if (Tpat->Type == DENSITY_FILE_PATTERN)
	{
		Destroy_Density_File(Tpat->Vals.Density_File);
	}

	if (Tpat->Type == OBJECT_PATTERN)
	{
		if(Tpat->Vals.Object != NULL)
		{
			Destroy_Object(reinterpret_cast<ObjectPtr>(Tpat->Vals.Object));
		}
	}

	if (Tpat->Type == PIGMENT_PATTERN)
	{
		if (Tpat->Vals.Pigment != NULL)
		{
			Destroy_Pigment( Tpat->Vals.Pigment );
		}
	}

	if (Tpat->Type == FUNCTION_PATTERN)
	{
		if (Tpat->Vals.Function.Fn != NULL)
		{
			Parser::Destroy_Function(Tpat->Vals.Function.vm, (FUNCTION_PTR)(Tpat->Vals.Function.Fn));
		}
	}
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

TURB *Create_Turb()
{
	TURB *New;

	New = reinterpret_cast<TURB *>(POV_MALLOC(sizeof(TURB),"turbulence struct"));

	Make_Vector(New->Turbulence, 0.0, 0.0, 0.0);

	New->Octaves = 6;
	New->Omega = 0.5;
	New->Lambda = 2.0;

	return(New);
}


/*****************************************************************************
*
* FUNCTION
*
*   Translate_Tpattern
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

void Translate_Tpattern(TPATTERN *Tpattern, const VECTOR Vector)
{
	TRANSFORM Trans;

	if (Tpattern != NULL)
	{
		Compute_Translation_Transform (&Trans, Vector);

		Transform_Tpattern (Tpattern, &Trans);
	}
}


/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Tpattern
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

void Rotate_Tpattern(TPATTERN *Tpattern, const VECTOR Vector)
{
	TRANSFORM Trans;

	if (Tpattern != NULL)
	{
		Compute_Rotation_Transform (&Trans, Vector);

		Transform_Tpattern (Tpattern, &Trans);
	}
}


/*****************************************************************************
*
* FUNCTION
*
*   Scale_Tpattern
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

void Scale_Tpattern(TPATTERN *Tpattern, const VECTOR Vector)
{
	TRANSFORM Trans;

	if (Tpattern != NULL)
	{
		Compute_Scaling_Transform (&Trans, Vector);

		Transform_Tpattern (Tpattern, &Trans);
	}
}


/*****************************************************************************
*
* FUNCTION
*
*   Transform_Tpattern
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

void Transform_Tpattern(TPATTERN *Tpattern, const TRANSFORM *Trans)
{
	WARP *Temp;

	if (Tpattern != NULL)
	{
		if (Tpattern->Warps == NULL)
		{
			Tpattern->Warps = Create_Warp(TRANSFORM_WARP);
		}
		else
		{
			if (Tpattern->Warps->Warp_Type != TRANSFORM_WARP)
			{
				Temp = Tpattern->Warps;

				Tpattern->Warps = Create_Warp(TRANSFORM_WARP);

				Tpattern->Warps->Next_Warp = Temp;
				if(Tpattern->Warps->Next_Warp != NULL)
					Tpattern->Warps->Next_Warp->Prev_Warp = Tpattern->Warps;
			}
		}

		Compose_Transforms (&( (reinterpret_cast<TRANS *>(Tpattern->Warps))->Trans), Trans);
	}
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

void Search_Blend_Map (DBL value, const BLEND_MAP *Blend_Map, const BLEND_MAP_ENTRY **Prev, const BLEND_MAP_ENTRY **Cur)
{
	BLEND_MAP_ENTRY *P, *C;
	int Max_Ent=Blend_Map->Number_Of_Entries-1;

	/* if greater than last, use last. */

	if (value >= Blend_Map->Blend_Map_Entries[Max_Ent].value)
	{
		P = C = &(Blend_Map->Blend_Map_Entries[Max_Ent]);
	}
	else
	{
		P = C = &(Blend_Map->Blend_Map_Entries[0]);

		while (value > C->value)
		{
			P = C++;
		}
	}

	if (value == C->value)
	{
		P = C;
	}

	*Prev = P;
	*Cur  = C;
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

static const TURB *Search_For_Turb(const WARP *Warps)
{
	const WARP* Temp=Warps;

	if (Temp!=NULL)
	{
		while (Temp->Next_Warp != NULL)
		{
			Temp=Temp->Next_Warp;
		}

		if (Temp->Warp_Type != CLASSIC_TURB_WARP)
		{
			Temp=NULL;
		}
	}

	return (reinterpret_cast<const TURB *>(Temp));
}

/* Tiling & Pavement */


/*****************************************************************************
*
* FUNCTIONS
*
*   related to Tiling & Pavement
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   J. Grimbert
*
* DESCRIPTION
*
*   New colour function by J. Grimbert
*
*   Fill the X-Z plane with repeating pattern
*   When there is only one kind of form, 1 is the outside, 0 is most inside
*   When there is two kind of form, 0 & 1 are the most inside, 1/2 is
*    the outside
*
******************************************************************************/

static DBL tiling_square (const VECTOR EPoint)
{
	/* 
	 ** Classical square tiling
	 */
	DBL x,z;
	x = fabs(EPoint[X]);
	x -= floor(x);
	x = 2*fabs( x-0.5 );

	z = fabs(EPoint[Z]);
	z -= floor(z);
	z = 2*fabs( z-0.5 );

	return max(x,z);
}

static DBL tiling_hexagon (const VECTOR EPoint)
{
	/* 
	 ** Classical Hexagon tiling
	 */
	DBL x,z;
	DBL dist1,dist2;
	DBL answer;

	x=EPoint[X];
	z=EPoint[Z];
	x += 0.5;
	x -= 3.0*floor(x/3.0);
	z -= SQRT3*floor(z/(SQRT3));
	/* x,z is in { [0.0, 3.0 [, [0.0, SQRT3 [ } 
	 ** but there is some symmetry to simplify the testing
	 */
	if (z > SQRT3_2 )
	{
		z = SQRT3 - z;
	}
	/* 
	 ** Now only [0,3[,[0,SQRT3/2[
	 */
	if (x > 1.5)
	{
		x -= 1.5 ; /* translate */
		z = SQRT3_2 -z; /* mirror */
	}
	/*
	 ** And now, it is even simpler :  [0,1.5],[0,SQRT3/2]
	 ** on the bottom left corner, part of some other hexagon
	 ** on the top right corner, center of the hexagon
	 */
	if ((SQRT3*x+z)<SQRT3_2)
	{
		x = 0.5 - x;
		z = SQRT3_2 -z; /* mirror */
	}
	if (x > 1.0)
	{
		x = 2.0 -x; /* mirror */
	}
	/* Hexagon */
	dist1 = 1.0 - ( z / SQRT3_2 );
	dist2 = 1.0 - (((SQRT3 * x + z - SQRT3_2) ) / SQRT3 );
	answer = max(dist1,dist2);
	answer = min(1.0,answer);
	answer = max(0.0,answer);
	return answer;
}

static DBL tiling_triangle (const VECTOR EPoint)
{
	DBL x,z;
	DBL slop1;
	DBL dist1,dist2;
	int delta;
	x=EPoint[X];
	z=EPoint[Z];
	delta = 0;
	x -= floor(x);
	z -= SQRT3*floor(z/SQRT3);
	/* x,z is in { [0.0, 1.0 [, [0.0, SQRT3 [ } 
	 ** but there is some symmetry to simplify the testing
	 */
	if (z > SQRT3_2 )
	{
		z = SQRT3 - z; /* mirror */
		delta = 1 - delta;
	}
	if (x > 0.5)
	{
		x = 1.0 - x; /* mirror */
	}
	if (x != 0.0)
	{
		slop1 = z/x;
		if (slop1>SQRT3)
		{
			z = SQRT3_2 - z;
			x = 0.5 -x;
			delta = 1 - delta;
		}
	}
	else
	{
		z = SQRT3_2 -z;
		x = 0.5;
	}
	dist1 = 1.0 - (z * 2 * SQRT3 );
	dist2 = 1.0 - ((SQRT3 * x - z) * SQRT3 );
	return delta/2.0+(0.5)*max(dist1,dist2);
}

static DBL tiling_lozenge (const VECTOR EPoint)
{
	DBL x,z;
	DBL slop1;
	DBL dist1,dist2;

	x=EPoint[X];
	z=EPoint[Z];

	x -= floor(x);
	z -= SQRT3*floor(z/SQRT3);
	/* x,z is in { [0.0, 1.0 [, [0.0, SQRT3 [ } 
	 ** There is some mirror to reduce the problem
	 */
	if ( z > SQRT3_2 )
	{
		z -= SQRT3_2;
		x += 0.5;
	}
	if ( (2.0*z) > SQRT3_2 )
	{
		z = SQRT3_2 - z;
		x = 1.5 - x;
	}
	if (x > 0.75)
	{
		x -= 1.0;
	}
	if (x != 0.0)
	{
		slop1 = z/x;
		if (slop1>SQRT3)
		{
			z = SQRT3_2 - z;
			x = 0.5 -x;
		}
	}
	dist1 = 1.0 - (z * 4.0 * SQRT3 / 3.0 );
	dist2 = 1.0 - (fabs(SQRT3 * x - z) * SQRT3 *2.0 / 3.0);
	return max(dist1,dist2);
}

static DBL tiling_rhombus (const VECTOR EPoint)
{
	DBL x,z;
	DBL answer;
	DBL slop1;
	DBL dist1,dist2;
	int delta;
	x=EPoint[X];
	z=EPoint[Z];
	delta = 0;
	x += 0.5;
	x -= 3.0*floor(x/3.0);
	z -= SQRT3*floor(z/SQRT3);
	/* x,z is in { [0.0, 3.0 [, [0.0, SQRT3 [ } 
	 ** There is some mirror to reduce the problem
	 */
	if ( z > SQRT3_2 )
	{
		z = SQRT3 -z; /* mirror */
		delta = 2 - delta;
	}
	if (x > 1.5)
	{
		x -= 1.5 ; /* translate */
		z = SQRT3_2 -z; /* mirror */
		delta = 2 - delta;
	}
	/* Now in [0,1.5],[0,SQRT3/2] 
	 ** from left to right
	 ** part of a horizontal (z=0)
	 ** half a vertical 
	 ** part of a horizontal 
	 */
	if (x < 0.5)
	{
		/* mirrror */
		x = 1.0 - x;
		delta = 2 - delta;
	}
	/* 
	 ** Let shift the [0.5,1.5],[0,SQRT3/2] to [0,1]....
	 */
	x -= 0.5;
	if (x != 0.0)
	{
		slop1 = z/x;
		if (slop1>SQRT3)
		{ /* rotate the vertical to match the horizontal on the right */
			dist1 = ( x / 2.0 ) + ( z * SQRT3_2 );
			dist2 = ( z / 2.0 ) - ( x * SQRT3_2 );
			z = dist2;
			x = dist1;
			delta = 1;
		}
	}
	else
	{
		/* rotate the vertical to match the horizontal on the right */
		dist1 = ( x / 2.0 ) + ( z * SQRT3_2 );
		dist2 = ( z / 2.0 ) - ( x * SQRT3_2 );
		z = dist2;
		x = dist1;
		delta = 1;
	}
	/* It may be similar to lozenge (in fact, IT IS !), now */

	if ( (2.0*z) > SQRT3_2 )
	{
		z = SQRT3_2 - z;
		x = 1.5 - x;
	}
	if (x > 0.75)
	{
		x -= 1.0;
	}
	if (x != 0.0)
	{
		slop1 = z/x;
		if (slop1>SQRT3)
		{
			z = SQRT3_2 - z;
			x = 0.5 -x;
		}
	}
	dist1 = 1.0 - (z * 4.0 * SQRT3 / 3.0 );
	dist2 = 1.0 - (fabs(SQRT3 * x - z) * SQRT3 *2.0 / 3.0);
	answer = max(dist1,dist2);
	answer /= 3.0;
	answer += delta/3.0;
	answer = min(1.0,answer);
	answer = max(0.0,answer);
	return answer;
}

static DBL tiling_rectangle (const VECTOR EPoint)
{
	/*
	 ** Tiling with rectangles
	 ** resolve to square [0,4][0,4]
	 ** then 16 cases
	 **
	 **  +-----+--+  +
	 **  |     |  |  |
	 **  +--+--+  +--+
	 **     |  |  |
	 **  +--+  +--+--+
	 **  |  |  |     |
	 **  +  +--+--+--+
	 **  |  |     |  |
	 **  +--+-----+  +
	 */
	DBL x,z;
	DBL delta;
	DBL answer;
	int valueX,valueZ;
	x=EPoint[X];
	z=EPoint[Z];
	x -= 4.0*floor(x/4.0);
	z -= 4.0*floor(z/4.0);
	valueX = (int)x;
	valueZ = (int)z;
	delta = 1.0;
	switch((valueX+valueZ*4))
	{
		case 0:
		case 4:
			z -= 1.0;
			break;
		case 1:
		case 2:
			x -= 2.0;
			delta = 0.0;
			break;
		case 3:
			x -= 3.0;
			break;
		case 5:
		case 9:
			x -= 1.0;
			z -= 2.0;
			break;
		case 6:
		case 7:
			x -= 3.0;
			z -= 1.0;
			delta = 0.0;
			break;
		case 8:
			z -= 2.0;
			delta = 0.0;
			break;
		case 10:
		case 14:
			x -= 2.0;
			z -= 3.0;
			break;
		case 11:
			x -= 4.0;
			z -= 2.0;
			delta = 0.0;
			break;
		case 12:
		case 13:
			x -= 1.0;
			z -= 3.0;
			delta = 0.0;
			break;
		case 15:
			x -= 3.0;
			z -= 4.0;
			break;
	}
	if (delta == 1.0)
	{
		x = 2*fabs( x-0.5 );
		z = 2*( max(fabs( z),0.5) -0.5 );
	}
	else
	{
		x = 2*( max(fabs( x),0.5) -0.5 );
		z = 2*fabs( z -0.5 );
	}
	answer = fabs(max(x,z)/2.0 + delta/2);
	return answer;
}

static DBL tiling_octa_square (const VECTOR EPoint)
{
	/*
	 ** Tiling with a square and an octagon
	 */
	DBL answer;
	DBL x,z;
	DBL dist1,dist2;
	z=EPoint[Z];
	z -= (SQRT2+1.0)*floor(z/(SQRT2+1.0));
	z -= SQRT2_2+0.5;
	z = fabs(z);
	x=EPoint[X];
	x -= (SQRT2+1.0)*floor(x/(SQRT2+1.0));
	x -= SQRT2_2+0.5;
	x = fabs(x);
	if (z > x)
	{
		answer = x;
		x = z;
		z = answer;
	}
	if ( (x+z) < SQRT2_2)
	{
		/* Square tile */
		return ( (x+z)/SQRT2 );
	}
	dist1 = 1.0-z;
	dist2 = (SQRT2+SQRT2_2-(x+z))/SQRT2;

	return max(0.500000000001,max(dist1,dist2)); // TODO FIXME - magic number! Use nextafter() instead (or maybe actually 0.5)
}

static DBL tiling_square_triangle (const VECTOR EPoint)
{
	DBL x,z;
	DBL slop1;
	DBL dist1,dist2;
	int delta;
	x=EPoint[X];
	z=EPoint[Z];
	delta = 0;
	x -= floor(x);
	z -= (2.0+SQRT3)*floor(z/(SQRT3+2.0));
	/* x,z is in { [0.0, 1.0 [, [0.0, 2+SQRT3 [ } 
	 ** but there is some symmetry to simplify the testing
	 */
	if (z > SQRT3_2+1.0 )
	{
		z -= SQRT3_2+1.0;
		x += (x>0.5)?-0.5:0.5;
	}
	if (x > 0.5)
	{
		x = 1.0 - x; /* mirror */
	}
	z -= 1.0;
	if (z > 0.0)
	{ /* triangle */
		if (x != 0.0)
		{
			slop1 = z/x;
			if (slop1>SQRT3)
			{
				z = SQRT3_2 - z;
				x = 0.5 -x;
				delta = 1 - delta;
			}
		}
		else
		{
			z = SQRT3_2 -z;
			x = 0.5;
			delta = 1 - delta;
		}
		dist1 = 1.0 - (2 * z * SQRT3 );
		dist2 = 1.0 - ((SQRT3 * x - z) * SQRT3 );
		return (delta+max(dist1,dist2))/3.0;
	}
	else
	{ /* square */
		if (z < -0.5)
		{
			z = -1.0 -z;
		}
		if (x > 0.5)
		{
			x = 1.0 - x;
		}
		dist1 = 2 + 2 * SQRT3 * fabs( x ) ;
		dist2 = 2 + 2 * SQRT3 * fabs( z ) ;
		dist1 = min(dist1,3.0);
		dist2 = min(dist2,3.0);
		return (5.0000001-min(dist1,dist2))/3.0; // TODO FIXME - magic number! Should use nextafter()
	}
}

static DBL tiling_hexa_triangle (const VECTOR EPoint)
{
	/* 
	 ** Tiling with a hexagon and 2 triangles
	 */
	DBL x,z;
	DBL dist1,dist2;
	DBL answer=0;
	int delta;
	x=EPoint[X];
	z=EPoint[Z];
	delta = 0;
	x += 0.5;
	x -= 2.0*floor(x/2.0);
	z -= 2.0*SQRT3*floor(z/(SQRT3*2.0));
	/* x,z is in { [0.0, 2.0 [, [0.0, 2*SQRT3 [ } 
	 ** but there is some symmetry to simplify the testing
	 */
	if (z > SQRT3 )
	{
		z -= SQRT3;
		x += (x<1.0)?1.0:-1.0;
	}
	/* 
	 ** Now only [0,2[,[0,SQRT3[
	 */
	if (z > SQRT3_2 )
	{
		z = SQRT3 - z; /* mirror */
		delta = 1 - delta;
	}

	if (x > 1.0)
	{
		x = 2.0 - x; /* mirror */
	}
	/*
	 ** And now, it is even simpler :  [0,1],[0,SQRT3/2]
	 ** on the bottom left corner, part of the triangle
	 ** on the top right corner, center of the hexagon
	 */
	if ((SQRT3*x+z)<SQRT3_2)
	{
		/* Triangle */
		dist1 = 1.0 - (z * 2 * SQRT3 );
		dist2 = 1.0 + ((SQRT3 * x + z) - SQRT3_2 ) * SQRT3; /* really substracting */
		answer = (delta + max(dist1,dist2))/3.0;
	}
	else
	{
		/* Hexagon */
		dist1 = 2 + 2* (z * SQRT3 );
		dist2 = 2 + 2* ((SQRT3 * x + z - SQRT3_2) ) * SQRT3_2 ;
		answer = 5.0-min(dist1,dist2);
		answer = max(answer,2.0000001); // TODO FIXME - magic number! Should use nextafter()
		answer /= 3.0;
	}
	return answer;
}

static DBL tiling_square_offset (const VECTOR EPoint)
{
	/*
	 ** Tiling with a square, offset of half size
	 ** Reduce to rectangle [0,1][0,2]
	 ** move x,[1,2] to [0,1][0,1] with new x = x+1/2
	 */
	DBL x,z;
	z = EPoint[Z];
	z -= 2.0*floor(z/2.0);
	x = EPoint[X];
	if (z > 1.0)
	{
		x += 0.5;
		z -= 1.0;
	}
	x -= floor(x);
	x = 2*fabs( x-0.5 );
	z = 2*fabs( z-0.5 );

	return max(x,z);
}

static DBL tiling_square_rectangle (const VECTOR EPoint)
{
	/*
	 ** tiling with a central square and 4 rectangle (2x1)
	 ** orbiting around the square
	 ** Reduce to [0,3][0,3]
	 ** then 9 cases
	 **
	 **  +-----+--+
	 **  |     |  |
	 **  +--+--+  |
	 **  |  |  |  |
	 **  |  +--+--+
	 **  |  |     |
	 **  +--+-----+
	 */
	DBL x,z;
	DBL delta;
	int valueX,valueZ;
	x = EPoint[X];
	x -= 3.0*floor(x/3.0);
	z = EPoint[Z];
	z -= 3.0*floor(z/3.0);
	valueX = (int)x;
	valueZ = (int)z;
	delta = 2.0;
	switch((valueX+valueZ*3))
	{
		case 0:
		case 3:
			z -= 1.0;
			break;
		case 1:
		case 2:
			x -= 2.0;
			delta = 1.0;
			break;
		case 4:
			x -= 1.0;
			z -= 1.0;
			delta = 0.0;
			break;
		case 5:
		case 8:
			x -= 2.0;
			z -= 2.0;
			break;
		case 6:
		case 7:
			x -= 1.0;
			z -= 2.0;
			delta = 1.0;
			break;
	}
	if (delta == 1.0)
	{
		x = fabs( x);
		x = 2.0*( max( x,0.5) -0.5 );
		z = 2.0*fabs( z -0.5 );
	}
	if (delta == 2.0)
	{
		x = 2.0*fabs( x-0.5 );
		z = fabs(z);
		z = 2.0*( max(z,0.5) - 0.5 );
	}
	if (delta == 0.0)
	{
		x = 2.0*fabs( x-0.5 );
		z = 2.0*fabs( z-0.5 );
	}
	return ((max(x,z))+delta)/3.0 ;
}

static DBL tiling_rectangle_square (const VECTOR EPoint)
{
	/*
	 ** Tiling with a central square and 4 rectangles (2x1)
	 ** which turns around the square in both directions
	 ** Reduce to [0,6][0,6], fold in four and back
	 ** to 9 cases.
	 **
	 **  +-----+--+
	 **  |     |  |
	 **  +--+--+  |
	 **  |  |  |  |
	 **  |  +--+--+
	 **  |  |     |
	 **  +--+--+--+
	 */
	DBL x,z;
	DBL delta;
	int valueX,valueZ;
	x = EPoint[X];
	x -= 6.0*floor(x/6.0);
	x -= 3.0;
	x = fabs(x);
	z = EPoint[Z];
	z -= 6.0*floor(z/6.0);
	z -= 3.0;
	z = fabs(z);
	valueX = (int)x;
	valueZ = (int)z;
	delta = 2.0;
	switch((valueX+valueZ*3))
	{
		case 0:
		case 3:
			z -= 1.0;
			break;
		case 1:
		case 2:
			x -= 2.0;
			delta = 1.0;
			break;
		case 4:
			x -= 1.0;
			z -= 1.0;
			delta = 0.0;
			break;
		case 5:
		case 8:
			x -= 2.0;
			z -= 2.0;
			break;
		case 6:
		case 7:
			x -= 1.0;
			z -= 2.0;
			delta = 1.0;
			break;
	}
	if (delta == 1.0)
	{
		x = fabs( x);
		x = 2.0*( max(x,0.5) -0.5 );
		z = 2.0*fabs( z -0.5 );
	}
	if (delta == 2.0)
	{
		x = 2.0*fabs( x-0.5 );
		z = fabs(z);
		z = 2.0*( max(z,0.5) - 0.5 );
	}
	if (delta == 0.0)
	{
		x = 2.0*fabs( x-0.5 );
		z = 2.0*fabs( z-0.5 );
	}
	return ((max(x,z))+delta)/3.0 ;
}

static DBL tiling_square_internal (const VECTOR EPoint)
{
	DBL answer=0;
	DBL x,z;
	DBL dist1,dist2;
	int valueX,valueZ;
	x=EPoint[X];
	x *= SQRT2;
	x -= 4.0 * floor(x/4.0);
	x -= 2.0;
	x = fabs(x);
	z=EPoint[Z];
	z *= SQRT2;
	z -= 4.0 * floor(z/4.0);
	z -= 2.0;
	z = fabs(z);
	valueX=(int)x;
	valueZ=(int)z;
	switch((valueX+valueZ*2))
	{
		case 0:
			x -= 0.5;
			x = max(x,0.0);
			x *= 2.0;
			z -= 0.5;
			z = max(z,0.0);
			z *= 2.0;
			answer=max(x,z)/3.0;
			break;
		case 1:
			answer=(2.0+fabs(1.5-x)*2.0)/3.0;
			if (z>0.5)
			{
				dist2=(3.0-SQRT2*fabs(x-z))/3.0;
				answer = max(answer,dist2);
			}
			break;
		case 2:
			answer=(1.0+fabs(1.5-z)*2.0)/3.0;
			if (x>0.5)
			{
				dist2=(2.0-SQRT2*fabs(x-z))/3.0;
				answer = max(answer,dist2);
			}
			break;
		case 3:
			if (x > z)
			{
				dist1=(2.0+fabs(1.5-x)*2.0)/3.0;
				dist2=(3.0-SQRT2*fabs(z-x))/3.0;
				answer=max(dist1,dist2);
			}
			else
			{
				dist1=(1.0+fabs(1.5-z)*2.0)/3.0;
				dist2=(2.0-SQRT2*fabs(x-z))/3.0;
				answer=max(dist1,dist2);
			}
			break;
	}
	return answer;
}

static DBL tiling_square_internal_5 (const VECTOR EPoint)
{
	DBL answer=0;
	DBL x,z;
	DBL dist1,dist2;
	int mirX,mirZ;
	int valueX,valueZ;
	mirX=mirZ=0;
	x=EPoint[X];
	x *= SQRT2;
	x -= 4.0 * floor(x/4.0);
	x -= 2.0;
	mirX = (x < 0)?1:0;
	x = fabs(x);
	z=EPoint[Z];
	z *= SQRT2;
	z -= 4.0 * floor(z/4.0);
	z -= 2.0;
	mirZ = (z < 0)?2:3;
	z = fabs(z);
	valueX=(int)x;
	valueZ=(int)z;
	switch((valueX+valueZ*2))
	{
		case 0:
			x -= 0.5;
			x = max(x,0.0);
			x *= 2.0;
			z -= 0.5;
			z = max(z,0.0);
			z *= 2.0;
			answer=(4.000001 + max(x,z))/5.0; // TODO FIXME - magic number! Should use nextafter()
			break;
		case 1:
			answer=fabs(1.5-x)*2.0;
			if (z>0.5)
			{
				dist2=1.0-SQRT2*fabs(x-z);
				answer = max(answer,dist2);
			}
			answer += mirX;
			answer /= 5.0;
			break;
		case 2:
			answer=fabs(1.5-z)*2.0;
			if (x>0.5)
			{
				dist2=1.0-SQRT2*fabs(x-z);
				answer = max(answer,dist2);
			}
			answer += mirZ;
			answer /= 5.0;
			break;
		case 3:
			if (x > z)
			{
				dist1=fabs(1.5-x)*2.0;
				dist2=1.0-SQRT2*fabs(z-x);
				answer=max(dist1,dist2) + mirX;
			}
			else
			{
				dist1=fabs(1.5-z)*2.0;
				dist2=1.0-SQRT2*fabs(x-z);
				answer=max(dist1,dist2) + mirZ;
			}
			answer /= 5.0;
			break;
	}
	return answer;
}

static DBL tiling_square_double (const VECTOR EPoint)
{
	/*
	 ** Tiling with a square (1x1) and a square (2x2)
	 ** Reduce to [0,5][0,5] then 25 cases
	 **
	 **  +--+     +--+--+
	 **     |     |  |  
	 **     +--+--+--+   
	 **     |  |     |  
	 **  +--+--+     +--+
	 **  |     |     |  |
	 **  +     +--+--+--+
	 **  |     |  |     |
	 **  +--+--+--+     +
	 **  |  |     |     |
	 **  +--+     +--+--+
	 */
	DBL x,z;
	DBL delta;
	int valueX,valueZ;
	x = EPoint[X];
	x -= 5.0*floor(x/5.0);
	z = EPoint[Z];
	z -= 5.0*floor(z/5.0);
	valueX = (int)x;
	valueZ = (int)z;
	delta = 0.50000001; // TODO FIXME - magic number! Should use nextafter()
	switch((valueX+valueZ*5))
	{
		case 0:
			delta = 0.0;
			break;
		case 1:
		case 2:
			x -= 2.0;
			break;
		case 3:
		case 4:
		case 8:
		case 9:
			x -= 4.0;
			z -= 1.0;
			break;
		case 5 :
		case 6 :
		case 10:
		case 11:
			z -= 2.0;
			x -= 1.0;
			break;
		case 7:
			delta = 0.0;
			x -= 2.0;
			z -= 1.0;
			break;
		case 12:
		case 13:
		case 17:
		case 18:
			x -= 3.0;
			z -= 3.0;
			break;
		case 14:
			x -= 4.0;
			z -= 2.0;
			delta = 0.0;
			break;
		case 15:
		case 20:
			z -= 4.0;
			break;
		case 16:
			x -= 1.0;
			z -= 3.0;
			delta = 0.0;
			break;
		case 21:
		case 22:
			x -= 2.0;
			z -= 5.0;
			break;
		case 23:
			x -= 3.0;
			z -= 4.0;
			delta = 0.0;
			break;
		case 24:
		case 19:
			x -= 5.0;
			z -= 4.0;
			break;
	}
	if (delta)
	{
		x = fabs(x);
		x = 2*( max(x,0.5) -0.5 );
		z = fabs(z);
		z = 2*( max(z,0.5) -0.5 );
	}
	else
	{
		x = 2*fabs( x-0.5 );
		z = 2*fabs( z-0.5 );
	}
	return fabs((max(x,z))/2.0+delta);
}

static DBL tiling_hexa_square_triangle (const VECTOR EPoint)
{
	/* 
	 ** tiling with 1 hexagon, squares and triangles
	 */
	DBL x,z;
	DBL dist1,dist2;
	DBL answer=0;

	x=EPoint[X];
	z=EPoint[Z];
	x += 0.5;
	x -= (3.0+SQRT3)*floor(x/(3.0+SQRT3));
	z -= 2.0*(1.0+SQRT3)*floor(z/((SQRT3+1.0)*2.0));
	/* x,z is in { [0.0, 3.0+SQRT3 [, [0.0, 2*(1+SQRT3) [ } 
	 ** but there is some symmetry to simplify the testing
	 */
	if (x > (SQRT3_2+1.5))
	{
		x -= (SQRT3_2+1.5);
		z += (z < SQRT3+1.0)?(SQRT3_2+0.5):(-1*(SQRT3_2+0.5));
	}
	if (z > (SQRT3+1.0) )
	{
		z -= SQRT3+1.0;
	}
	/* 
	 ** Now only [0, SQRT3/2+1.5 ], [0,SQRT3+1.0]
	 */
	if (z > (0.5+SQRT3_2 ) )
	{
		z = SQRT3+1.0 - z; /* mirror */
	}

	if (x > (1.0+SQRT3_2) )
	{
		x = SQRT3+2.0 - x; /* mirror */
	}
	/*
	 ** And now, it is even simpler :  [0,1+(SQRT3/2)],[0,1+(SQRT3/2)]
	 ** on the top right corner, center of the hexagon
	 ** on the bottom right, middle of square, a triangle on the left
	 ** on the top left corner, half a triangle
	 ** bottom
	 */
	if (((SQRT3*x+z)<(SQRT3_2+2.0))&&(x < 0.5+SQRT3_2))
	{
		/* Triangle or square */
		/* rotate in the lower part */
		z -= 0.5 + SQRT3_2 ;
		x -= 1.0 + SQRT3_2 ;
		dist1 = ( x / 2.0 ) - ( z * SQRT3_2 );
		dist2 = ( z / 2.0 ) + ( x * SQRT3_2 );
		z = dist2;
		x = dist1;
		z += 0.5 + SQRT3_2 ;
		x += 1.0 + SQRT3_2 ;
		if (z < 0)
		{
			z *= -1;
		}
		if (x > (1.0+SQRT3_2) )
		{
			x = SQRT3+2.0 - x; /* mirror */
		}
	}
	if ((!((SQRT3*x+z)<(SQRT3_2+2.0)))&&(!(z < 0.5)))
	{
		/* Hexagon */
		z -= 0.5;
		x -= SQRT3_2;
		z= fabs(z);
		x= fabs(x);
		dist1 = 2* z * SQRT3 ;
		dist2 = ((SQRT3 * x + z ) ) * SQRT3 - 1.5;
		answer = 3.0-min(dist1,dist2);
		answer = max(answer,2.0000001); // TODO FIXME - magic number! Should use nextafter()
		answer /= 3.0;
		return answer;
	}
	if (z < 0.5 )
	{
		if ( x > SQRT3_2+0.5)
		{
			/* square */
			x -= SQRT3_2+0.5;
			z -= 0.5;
			dist1 = 1.0 + z*2 * SQRT3;
			dist2 = 1.0 - x*2*SQRT3;
			dist1 = max(dist1,0.0);
			dist2 = max(dist2,0.0);
			answer = max(dist1,dist2)/3.0;
		}
		else
		{
			/* triangle */
			x -= SQRT3_2 + 0.5;
			x = fabs(x);
			dist2 = ((SQRT3 * z + x) ) * SQRT3 +0.5;
			dist1 = 2.0 - (x * 2.0 * SQRT3 );
			answer = (max(dist1,dist2))/3.0;
		}
	}
	return answer;
}

static DBL tiling_hexa_square_triangle_6 (const VECTOR EPoint)
{
	/* 
	 ** tiling with 1 hexagon, squares and triangles
	 ** all tiles get its own colour (according to its orientation)
	 ** 1 hexagon, 3 squares, 2 triangles
	 */
	DBL x,z;
	DBL dist1,dist2;
	DBL answer=0;
	int mirX=0;
	int mirZ=0;
	int rota=0;
	x=EPoint[X];
	z=EPoint[Z];
	x += 0.5;
	x -= (3.0+SQRT3)*floor(x/(3.0+SQRT3));
	z -= 2.0*(1.0+SQRT3)*floor(z/((SQRT3+1.0)*2.0));
	/* x,z is in { [0.0, 3.0+SQRT3 [, [0.0, 2*(1+SQRT3) [ } 
	 ** but there is some symmetry to simplify the testing
	 */
	if (x > (SQRT3_2+1.5))
	{
		x -= (SQRT3_2+1.5);
		z += (z < SQRT3+1.0)?(SQRT3_2+0.5):(-1*(SQRT3_2+0.5));
	}
	if (z > (SQRT3+1.0) )
	{
		z -= SQRT3+1.0;
	}
	/* 
	 ** Now only [0, SQRT3/2+1.5 ], [0,SQRT3+1.0]
	 */
	if (z > (0.5+SQRT3_2 ) )
	{
		z = SQRT3+1.0 - z; /* mirror */
		mirZ = 1 - mirZ;
	}

	if (x > (1.0+SQRT3_2) )
	{
		x = SQRT3+2.0 - x; /* mirror */
		mirX = 1 - mirX;
	}
	/*
	 ** And now, it is even simpler :  [0,1+(SQRT3/2)],[0,1+(SQRT3/2)]
	 ** on the top right corner, center of the hexagon
	 ** on the bottom right, middle of square, a triangle on the left
	 ** on the top left corner, half a triangle
	 ** bottom
	 */
	if (((SQRT3*x+z)<(SQRT3_2+2.0))&&(x < 0.5+SQRT3_2))
	{
		/* Triangle or square */
		/* rotate in the lower part */
		z -= 0.5 + SQRT3_2 ;
		x -= 1.0 + SQRT3_2 ;
		dist1 = ( x / 2.0 ) - ( z * SQRT3_2 );
		dist2 = ( z / 2.0 ) + ( x * SQRT3_2 );
		z = dist2;
		x = dist1;
		z += 0.5 + SQRT3_2 ;
		x += 1.0 + SQRT3_2 ;
		rota = 1 - rota;
		if (z < 0)
		{
			z *= -1;
		}
		if (x > (1.0+SQRT3_2) )
		{
			x = SQRT3+2.0 - x; /* mirror */
			mirX = 1 - mirX;
			mirZ = 1 - mirZ;
		}
	}
	if ((!((SQRT3*x+z)<(SQRT3_2+2.0)))&&(!(z < 0.5)))
	{
		/* Hexagon */
		z -= 0.5;
		x -= SQRT3_2;
		z= fabs(z);
		x= fabs(x);
		dist1 = 2* z * SQRT3 ;
		dist2 = ((SQRT3 * x + z ) ) * SQRT3 - 1.5;
		answer = 6.0 - min(dist1,dist2);
		answer = max(answer,5.000001); // TODO FIXME - magic number! Should use nextafter()
		answer /= 6.0;
		return answer;
	}
	if (z < 0.5 )
	{
		if ( x > SQRT3_2+0.5)
		{
			/* square */
			x -= SQRT3_2+0.5;
			z -= 0.5;
			dist1 = 1.0 + z*2 * SQRT3;
			dist2 = 1.0 - x*2*SQRT3;
			dist1 = max(dist1,0.0);
			dist2 = max(dist2,0.0);
			answer = (max(dist1,dist2) +  rota * (1.000001 + ((mirX + mirZ) % 2)))/6.0; // TODO FIXME - magic number! Should use nextafter()
		}
		else
		{
			/* triangle */
			x -= SQRT3_2 + 0.5;
			x = fabs(x);
			dist2 = ((SQRT3 * z + x) ) * SQRT3 +0.5;
			dist1 = 2.0 - (x * 2.0 * SQRT3 );
			answer = max(dist1,dist2);
			if ((rota + mirX)%2)
			{
				answer = 2.0 + answer;
			}
			else
			{
				answer = 3.0 + answer;
			}
			answer /= 6.0;
		}
	}
	return answer;
}

static DBL tiling_rectangle_pair (const VECTOR EPoint)
{

	/*
	 ** Tiling with 2 rectangles (2x1)
	 ** Reduce to [0,4][0,4] then 16 cases
	 **
	 **  +-----+--+--+
	 **  |     |  |  |
	 **  +-----+  +  +
	 **  |     |  |  |
	 **  +-----+--+--+
	 **  |  |  |     |
	 **  |  |  +-----+
	 **  |  |  |     |
	 **  +--+--+-----+
	 */
	DBL x,z;
	DBL delta;
	DBL answer;
	int valueX,valueZ;
	x=EPoint[X];
	z=EPoint[Z];
	x -= 4.0*floor(x/4.0);
	z -= 4.0*floor(z/4.0);
	valueX = (int)x;
	valueZ = (int)z;
	delta = 1.0;
	switch((valueX+valueZ*4))
	{
		case 0:
		case 4:
			z -= 1.0;
			break;
		case 1:
		case 5:
			x -= 1.0;
			z -= 1.0;
			break;
		case 2:
		case 3:
			x -= 3.0;
			delta = 0.0;
			break;
		case 6:
		case 7:
			x -= 3.0;
			z -= 1.0;
			delta = 0.0;
			break;
		case 8:
		case 9:
			x -= 1.0;
			z -= 2.0;
			delta = 0.0;
			break;
		case 12:
		case 13:
			x -= 1.0;
			z -= 3.0;
			delta = 0.0;
			break;
		case 10:
		case 14:
			x -= 2.0;
			z -= 3.0;
			break;
		case 11:
		case 15:
			x -= 3.0;
			z -= 3.0;
			break;
	}
	if (delta == 1.0)
	{
		x = 2*fabs( x-0.5 );
		z = 2*( max(fabs( z),0.5) -0.5 );
	}
	else
	{
		x = 2*( max(fabs( x),0.5) -0.5 );
		z = 2*fabs( z -0.5 );
	}
	answer = fabs((max(x,z)+delta)/2.0);
	return answer;
}

static DBL tiling_hexa_tri_right (const VECTOR EPoint)
{
	DBL x,z;
	DBL answer;
	DBL slop1;
	DBL dist1,dist2;
	int zzof;
	int delta;
	x=EPoint[X];
	z=EPoint[Z];
	/* First, resume to a simple pattern */
	zzof = z / SQRT3_2;
	zzof /= 3;
	if (z < 0)
	{
		zzof -= 1;
	}
	x += zzof / 2.0; /* right handed */
	z -= 3*SQRT3_2*floor(z/(3*SQRT3_2));
	x += 7.0;
	x -= 7.0 *floor(x/7.0);
	if ((x > 4.5) && (z > SQRT3))
	{
		x -= 4.5;
		z -= SQRT3_2;
	}
	if ((x > 5.0) && (z < SQRT3_2))
	{
		x -= 5;
		z += SQRT3;
	}
	if ((x > 2.5) && (z < SQRT3))
	{
		x -= 2.5;
		z += SQRT3_2;
	}
	delta = 0;
	zzof = z /SQRT3_2;
	if ( zzof == 2)
	{
		zzof = 1;
		z = 2 * SQRT3 - z;
		delta = 1 - delta;
	}
	if ( (!zzof) || (x > 2.0) ||
	     (( z + SQRT3*x < SQRT3) || ( SQRT3*x -z > SQRT3)) )
	{
		/* triangle */
		x -= 1.0 *floor(x/1.0);
		z -= SQRT3*floor(z/SQRT3);
		/* x,z is in { [0.0, 1.0 [, [0.0, SQRT3 [ } 
		 ** but there is some symmetry to simplify the testing
		 */
		if (z > SQRT3_2 )
		{
			z = SQRT3 - z; /* mirror */
			delta = 1 - delta;
		}
		if (x > 0.5)
		{
			x = 1.0 - x; /* mirror */
		}
		if (x != 0.0)
		{
			slop1 = z/x;
			if (slop1>SQRT3)
			{
				z = SQRT3_2 - z;
				x = 0.5 -x;
				delta = 1 - delta;
			}
		}
		else
		{
			z = SQRT3_2 -z;
			x = 0.5;
			delta = 1 - delta;
		}
		dist1 = 1.0 - (z * 2 * SQRT3 );
		dist2 = 1.0 - ((SQRT3 * x - z) * SQRT3 );
		return (delta+max(dist1,dist2))/3.0;
	}
	else
	{ /* hexagon */
		z -= SQRT3_2;
		if (x > 1.0)
		{
			x = 2.0 - x; /* mirror */
		}
		/* Hexagon */
		dist1 = 2 + 2* (z * SQRT3 );
		dist2 = 2 + 2* ((SQRT3 * x + z - SQRT3_2) ) * SQRT3_2 ;
		answer = 5.0-min(dist1,dist2);
		answer = max(answer, 2.000001); // TODO FIXME - magic number! Should use nextafter()
		answer /= 3.0;
		return answer;
	}
}

static DBL tiling_hexa_tri_left (const VECTOR EPoint)
{
	DBL x,z;
	DBL slop1;
	DBL dist1,dist2;
	DBL answer;
	int zzof;
	int delta;
	x=EPoint[X];
	z=EPoint[Z];
	/* First, resume to a simple pattern */
	zzof = z / SQRT3_2;
	zzof /= 3;
	if (z < 0)
	{
		zzof -= 1;
	}
	x -= zzof / 2.0; /* left handed */
	z -= 3*SQRT3_2*floor(z/(3*SQRT3_2));
	x += 7.0;
	x -= 7.0 *floor(x/7.0);
	if ((x > 2.0) && (z < SQRT3_2))
	{
		x -= 2.0;
		z += SQRT3;
	}
	if ((x > 4.5) && (z < SQRT3))
	{
		x -= 4.5;
		z += SQRT3_2;
	}
	if ((x > 2.5) && (z > SQRT3))
	{
		x -= 2.5;
		z -= SQRT3_2;
	}
	delta = 0;
	zzof = z /SQRT3_2;
	if ( zzof == 2)
	{
		zzof = 1;
		z = 2 * SQRT3 - z;
		delta = 1 - delta;
	}
	if ( (!zzof) || (x > 2.0) ||
	     (( z + SQRT3*x < SQRT3) || ( SQRT3*x -z > SQRT3)) )
	{
		/* triangle */
		x -= 1.0 *floor(x/1.0);
		z -= SQRT3*floor(z/SQRT3);
		/* x,z is in { [0.0, 1.0 [, [0.0, SQRT3 [ } 
		 ** but there is some symmetry to simplify the testing
		 */
		if (z > SQRT3_2 )
		{
			z = SQRT3 - z; /* mirror */
			delta = 1 - delta;
		}
		if (x > 0.5)
		{
			x = 1.0 - x; /* mirror */
		}
		if (x != 0.0)
		{
			slop1 = z/x;
			if (slop1>SQRT3)
			{
				z = SQRT3_2 - z;
				x = 0.5 -x;
				delta = 1 - delta;
			}
		}
		else
		{
			z = SQRT3_2 -z;
			x = 0.5;
			delta = 1 - delta;
		}
		dist1 = 1.0 - (z * 2 * SQRT3 );
		dist2 = 1.0 - ((SQRT3 * x - z) * SQRT3 );
		return (delta+max(dist1,dist2))/3.0;
	}
	else
	{ /* hexagon */
		z -= SQRT3_2;
		if (x > 1.0)
		{
			x = 2.0 - x; /* mirror */
		}
		/* Hexagon */
		dist1 = 2 + 2* (z * SQRT3 );
		dist2 = 2 + 2* ((SQRT3 * x + z - SQRT3_2) ) * SQRT3_2 ;
		answer = 5.0-min(dist1,dist2);
		answer = max(answer, 2.000001); // TODO FIXME - magic number! Should use nextafter()
		answer /= 3.0;
		return answer;
	}
}

static DBL tiling_square_tri (const VECTOR EPoint)
{
	DBL x,z;
	DBL slop1;
	DBL dist1,dist2;
	int delta;
	int gamma,beta;
	int xflop,zflop;
	x=EPoint[X];
	z=EPoint[Z];
	delta = 0;
	gamma = 0;
	beta = 0;
	x -= (1.0+SQRT3)*floor(x/(1.0+SQRT3));
	z -= (1.0+SQRT3)*floor(z/(1.0+SQRT3));
	/* x,z is in { [0.0, SQRT3+1.0 [, [0.0, SQRT3+1.0 [ } 
	 ** but there is some symmetry to simplify the testing
	 */
	if (z > 0.5 + SQRT3_2 )
	{
		z = 1.0 + SQRT3 - z; /* mirror */
		delta = 1 - delta;
	}
	if (x > 0.5 + SQRT3_2)
	{
		x = 1.0 + SQRT3 - x; /* mirror */
		delta = 1 - delta;
		beta = 1;
	}
	/* x,z is in { [0.0, (SQRT3+1)/2 ], [0.0, (SQRT3+1)/2 ] }
	 ** but there is still a symmetry and a rotation 
	 */
	xflop = ( 2.0 * x + (SQRT3 -1.0)* 2.0 * z / ( SQRT3 + 1.0) > (SQRT3)) ? 1: 0;
	zflop = ( 2.0 * z * SQRT3 + (SQRT3 -3.0)* 2.0 * x / ( SQRT3 + 1.0) > (SQRT3)) ? 1: 0;
	switch (xflop + 2*zflop)
	{
		case 0: /* do nothing */
			gamma = 2 * beta;
			break;
		case 1: /* rotate clockwise */
			gamma = beta ? 1 + 2 * delta : 3 - 2 *delta;
			slop1 = x;
			x = z;
			z = SQRT3_2 + 0.5 - slop1;
			break;
		case 2: /* rotate normal */
			gamma = beta ? 3 - 2 * delta : 1 + 2 *delta;
			slop1 = z;
			z = x;
			x = SQRT3_2 +0.5 - slop1;
			break;
		case 3: /* symmetry */
			gamma = beta ? 0: 2 ;
			x = SQRT3_2+0.5 -x;
			z = SQRT3_2+0.5 -z;
			break;
	}

	if (x == 0.0)
	{
		z = 0.0;
		x = 0.0;
	}
	slop1 = (z * 2 * SQRT3) - SQRT3 + x * 2;
	if (slop1 < 0)
	{
		/* triangle */

		dist1 = -1.5 / SQRT3 * slop1;
		dist2 = 2.0 * x * SQRT3;
		slop1 = min(dist1,dist2);
		return (gamma+1.0-slop1)/6.0;
	}
	else
	{
		/* square */
		slop1 *= 1.5 / SQRT3;
		slop1 = min(slop1,1.0);
		if (delta)
		{
			slop1 *= -1;
			slop1 += 6.0;
			slop1 /= 6.0;
			slop1 = max(slop1,5.000001/6.0); // TODO FIXME - magic number! Should use nextafter()
			slop1 = min(slop1,1.0);
		}
		else
		{
			slop1 *= -1;
			slop1 += 5.0;
			slop1 /= 6.0;
			slop1 = min(slop1,5.0/6.0);
			slop1 = max(slop1,4.000001/6.0); // TODO FIXME - magic number! Should use nextafter()
		}
		return slop1;
	}
}

static DBL tiling_dodeca_tri (const VECTOR EPoint)
{
	DBL x,z;
	DBL dist1,dist2,dist3,dist4,ret_value;
	DBL tmpx,tmpz;
	int toggle=0; /* switched each time a triangle get toggled */
	x=EPoint[X];
	z=EPoint[Z];
	x -= (2.0+SQRT3)*floor(x/(2.0+SQRT3));
	z -= (3.0+2.0*SQRT3)*floor(z/(3.0+2.0*SQRT3));
	/* x,z is in { [0.0, SQRT3+2.0 [, [0.0, 2*SQRT3+3.0 [ } 
	 ** but there is some symmetry to simplify the testing
	 */
	if (z > SQRT3+1.5)
	{
		/* translate */
		z -= SQRT3+1.5;
		x += (x<(1.0+SQRT3_2) ? 1.0: -1.0)*(1.0+SQRT3_2);
	}
	if (x > 1.0+SQRT3_2)
	{
		x = 2.0 + SQRT3 -x;
	}
	if (z > 1.0+SQRT3_2)
	{
		z = 2.0 + SQRT3 -z;
		toggle = 1 - toggle;
	}
	dist2 = x - SQRT3_2 - 0.5 + z * SQRT3;
	if (dist2 < 0.0)
	{
		tmpx = x;
		tmpz = z;
		x = (1.0+SQRT3)/4.0 + 0.5 * tmpx - SQRT3_2 * tmpz ;
		z = (3.0+SQRT3)/4.0 - SQRT3_2 * tmpx - 0.5 * tmpz ;
		dist2 *= -1.0;
	}
	dist1 = (z * 3.0 ); /* from the bottom line */
	dist3 =  z - SQRT3_2 - 0.5 + x * SQRT3;
	dist3 *= SQRT3;
	dist2 *= SQRT3;
	dist4 = (x * 3.0 ); /* from the vertical line */
	if (dist3 < 0.0)
	{
		ret_value = max(dist3,-1.0);
		ret_value += 1+toggle;
		ret_value /= 3.0;
	}
	else
	{
		/* dodecagon */
		ret_value = min(dist1,dist2);
		ret_value = min(ret_value,dist3);
		ret_value = min(ret_value,dist4);
		ret_value = min(ret_value,1.0);
		ret_value = 3.0000001 - ret_value; // TODO FIXME - magic number! Should use nextafter()

		ret_value /= 3.0;
	}
	return ret_value;
}

static DBL tiling_dodeca_hex (const VECTOR EPoint)
{
	DBL x,z;
	DBL dist1,dist2,dist3,dist4,ret_value;
	DBL dist5,dist6,dist7;
	DBL tmpx,tmpz;
	x=EPoint[X];
	z=EPoint[Z];
	x -= (3.0+SQRT3)*floor(x/(3.0+SQRT3));
	z -= (3.0+3.0*SQRT3)*floor(z/(3.0+3.0*SQRT3));
	/* x,z is in { [0.0, SQRT3+3.0 [, [0.0, 3*SQRT3+3.0 [ } 
	 ** but there is some symmetry to simplify the testing
	 */
	if (z > 1.5*SQRT3+1.5)
	{
		/* translate */
		z -= 1.5*SQRT3+1.5;
		x += (x<(1.5+SQRT3_2) ? 1.0: -1.0)*(1.5+SQRT3_2);
	}
	if (x > 1.5+SQRT3_2)
	{
		x = 3.0 + SQRT3 -x;
	}
	if (z > 1.0+SQRT3)
	{
		z = 2.0 + 2.0*SQRT3 -z;
	}
	dist2 = x - SQRT3_2 - 1.5 + z * SQRT3;
	if (dist2 < 0.0)
	{
		tmpx = x;
		tmpz = z;
		x = (3.0+SQRT3)/4.0 + 0.5 * tmpx - SQRT3_2 * tmpz ;
		z = (3.0+3.0*SQRT3)/4.0 - SQRT3_2 * tmpx - 0.5 * tmpz ;
	}
	dist2 = x - SQRT3_2 - 2.5 + z * SQRT3;
	dist1 = (z * 2.0 ) - SQRT3; /* from the bottom line */
	dist3 = z - 1.5*SQRT3 - 0.5 + x * SQRT3;
	dist4 = ((x-0.5) * 2.0 ); /* from the vertical line */
	if ( (dist2 >= 0.0) &&
	     (dist1 >= 0.0) &&
	     (dist3 >= 0.0) &&
	     (dist4 >= 0.0) )
	{
		/* dodecagon */
		ret_value = min(dist1,dist2);
		ret_value = min(ret_value,dist3);
		ret_value = min(ret_value,dist4);
		ret_value = min(ret_value,1.0);
		ret_value = 3.000001 - ret_value; // TODO FIXME - magic number! Should use nextafter()
	}
	else
	{
		dist5 = 2*z - 2*SQRT3 - 1.0;
		if (dist5 >= 0)
		{
			dist4 *= -1.0;
			ret_value = min(dist5,dist4);
			ret_value = 2.0 - ret_value;
		}
		else
		{
			dist6 = SQRT3 * x - z -SQRT3_2 +0.5;
			dist7 = dist6 - 2.0;
			switch((dist6 >= 0?0:1)+(dist7 >= 0.0 ?2:0))
			{
				case 1: /* left hexagon */
					dist5 *= -1.0;
					dist6 *= -1.0;
					dist3 *= -1.0;
					ret_value = min(dist6,dist3);
					ret_value = min(ret_value,dist5);
					ret_value = min(ret_value,1.0);
					ret_value = 1.0 - ret_value;
					break;
				case 2: /* bottom hexagon */
					dist1 *= -1.0;
					ret_value = min(dist7,dist1);
					ret_value = min(ret_value,1.0);
					ret_value = 1.0 - ret_value;
					break;
				case 0:
				default: /* slanted square */
					dist2 *= -1.0;
					dist7 *= -1.0;
					ret_value = min(dist6,dist2);
					ret_value = min(ret_value,dist7);
					ret_value = 2.0 - ret_value;
					break;
			}
		}
	}
	ret_value /= 3.0;
	return ret_value;
}

static DBL tiling_dodeca_hex_5 (const VECTOR EPoint)
{
	DBL x,z;
	DBL dist1,dist2,dist3,dist4,ret_value;
	DBL dist5,dist6,dist7;
	DBL tmpx,tmpz;
	int mirX,mirZ,rota;
	mirX = mirZ = rota = 0;
	x=EPoint[X];
	z=EPoint[Z];
	x -= (3.0+SQRT3)*floor(x/(3.0+SQRT3));
	z -= (3.0+3.0*SQRT3)*floor(z/(3.0+3.0*SQRT3));
	/* x,z is in { [0.0, SQRT3+3.0 [, [0.0, 3*SQRT3+3.0 [ } 
	 ** but there is some symmetry to simplify the testing
	 */
	if (z > 1.5*SQRT3+1.5)
	{
		/* translate */
		z -= 1.5*SQRT3+1.5;
		x += (x<(1.5+SQRT3_2) ? 1.0: -1.0)*(1.5+SQRT3_2);
	}
	if (x > 1.5+SQRT3_2)
	{
		x = 3.0 + SQRT3 -x;
		mirX = 1 - mirX;
	}
	if (z > 1.0+SQRT3)
	{
		z = 2.0 + 2.0*SQRT3 -z;
		mirZ = 1 - mirZ;
	}
	dist2 = x - SQRT3_2 - 1.5 + z * SQRT3;
	if (dist2 < 0.0)
	{
		tmpx = x;
		tmpz = z;
		x = (3.0+SQRT3)/4.0 + 0.5 * tmpx - SQRT3_2 * tmpz ;
		z = (3.0+3.0*SQRT3)/4.0 - SQRT3_2 * tmpx - 0.5 * tmpz ;
	}
	dist2 = x - SQRT3_2 - 2.5 + z * SQRT3;
	dist1 = (z * 2.0 ) - SQRT3; /* from the bottom line */
	dist3 = z - 1.5*SQRT3 - 0.5 + x * SQRT3;
	dist4 = ((x-0.5) * 2.0 ); /* from the vertical line */
	if ( (dist2 >= 0.0) &&
	     (dist1 >= 0.0) &&
	     (dist3 >= 0.0) &&
	     (dist4 >= 0.0) )
	{
		/* dodecagon */
		ret_value = min(dist1,dist2);
		ret_value = min(ret_value,dist3);
		ret_value = min(ret_value,dist4);
		ret_value = min(ret_value,1.0);
		ret_value = 5.000001 - ret_value; // TODO FIXME - magic number! Should use nextafter()
	}
	else
	{
		dist5 = 2*z - 2*SQRT3 - 1.0;
		if (dist5 >= 0)
		{
			dist4 *= -1.0;
			ret_value = min(dist5,dist4);
			ret_value = 2.0 - ret_value;
		}
		else
		{
			dist6 = SQRT3 * x - z -SQRT3_2 +0.5;
			dist7 = dist6 - 2.0;
			switch((dist6 >= 0?0:1)+(dist7 >= 0.0 ?2:0))
			{
				case 1: /* left hexagon */
					dist5 *= -1.0;
					dist6 *= -1.0;
					dist3 *= -1.0;
					ret_value = min(dist6,dist3);
					ret_value = min(ret_value,dist5);
					ret_value = min(ret_value,1.0);
					ret_value = 1.0 - ret_value;
					break;
				case 2: /* bottom hexagon */
					dist1 *= -1.0;
					ret_value = min(dist7,dist1);
					ret_value = min(ret_value,1.0);
					ret_value = 1.0 - ret_value;
					break;
				case 0:
				default: /* slanted square */
					dist2 *= -1.0;
					dist7 *= -1.0;
					ret_value = min(dist6,dist2);
					ret_value = min(ret_value,dist7);
					ret_value = (mirZ + mirX) %2 + 3.0 - ret_value;
					break;
			}
		}
	}
	ret_value /= 5.0;
	return ret_value;
}

static DBL tiling_penrose_halfkite (DBL pX, DBL pZ, int depth, bool rhombs);
static DBL tiling_penrose_halfdart (DBL pX, DBL pZ, int depth, bool rhombs);

static DBL tiling_penrose_halfkite (DBL pX, DBL pZ, int depth, bool rhombs)
{
	DBL x = pX;
	DBL z = abs(pZ);

	if (depth > 0)
	{
		if (z < (x - INVPHI) * TAN144)
		{
			return tiling_penrose_halfdart(x*PHI, z*PHI, depth-1, rhombs);
		}
		else
		{
			x -= COS36;
			z -= SIN36;
			DBL rotX = x*COS108 - z*SIN108;
			DBL rotZ = z*COS108 + x*SIN108;
			return tiling_penrose_halfkite(rotX*PHI, rotZ*PHI, depth-1, rhombs);
		}
	}
	else if (rhombs)
	{
		if (z < (x - INVPHI) * TAN72)
		{
			DBL dist1 = abs( SIN72  * (x-INVPHI) - COS72  * z ) * 5.55; // TODO FIXME - the factor is just an empiric value
			DBL dist2 = abs( SIN108 * (x-1)      - COS108 * z ) * 5.55;
			return max3(1.0-dist1/2,1.0-dist2/2,0.5+TILING_EPSILON);
		}
		else
		{
			DBL dist1 = abs( z )                              * 5.55; // TODO FIXME - the factor is just an empiric value
			DBL dist2 = abs( SIN72 * (x-INVPHI) - COS72 * z ) * 5.55;
			return min(max3(0.5-dist1/2,0.5-dist2/2,0.0),0.5-TILING_EPSILON);
		}
	}
	else
	{
		DBL dist1 = abs( SIN36  *  x    - COS36  * z ) * 4.46; // TODO FIXME - the factor is just an empiric value
		DBL dist2 = abs( SIN108 * (x-1) - COS108 * z ) * 4.46;
		return min(max3(0.5-dist1/2,0.5-dist2/2,0.0),0.5-TILING_EPSILON);
	}
}

static DBL tiling_penrose_halfdart (DBL pX, DBL pZ, int depth, bool rhombs)
{
	DBL x = pX;
	DBL z = abs(pZ);

	if (depth > 0)
	{
		if (z < (x - INVPHI) * TAN108)
		{
			DBL rotX = x*COS36 + z*SIN36;
			DBL rotZ = z*COS36 - x*SIN36;
			return tiling_penrose_halfkite(rotX*PHI, rotZ*PHI, depth-1, rhombs);
		}
		else
		{
			x -= 1;
			DBL rotX = x*COS144 + z*SIN144;
			DBL rotZ = z*COS144 - x*SIN144;
			return tiling_penrose_halfdart(rotX*PHI, rotZ*PHI, depth-1, rhombs);
		}
	}
	else if (rhombs)
	{
		DBL dist1 = abs( SIN36  *  x    - COS36  * z ) * 5.55; // TODO FIXME - the factor is just an empiric value
		DBL dist2 = abs( SIN144 * (x-1) - COS144 * z ) * 5.55;
		return min(max3(0.5-dist1/2,0.5-dist2/2,0.0),0.5-TILING_EPSILON);
	}
	else
	{
		DBL dist1 = abs( z )                           * 4.46; // TODO FIXME - the factor is just an empiric value
		DBL dist2 = abs( SIN144 * (x-1) - COS144 * z ) * 4.46;
		return max3(1.0-dist1/2,1.0-dist2/2,0.5+TILING_EPSILON);
	}
}

static DBL tiling_penrose (const VECTOR EPoint, bool rhombs, bool centerFlag)
{
	// Penrose tiling
	// rhombs=false: P2 ("kite and dart") Penrose tiling
	//   centerFlag=false: 5 darts at center forming a star
	//   centerFlag=true:  5 kites at center forming a regular decagon ("sun")
	// rhombs=true:  P3 ("rhombus") Penrose tiling
	//   centerFlag=false: 5 wide rhombs at center, surrounded by 5 narrow rhombs to form a regular decagon
	//   centerFlag=true:  5 wide rhombs at center, surrounded by 10 narrow rhombs to form a pointed star

	DBL x,z;
	x = EPoint[X];
	z = EPoint[Z];

	DBL r = sqrt(x*x+z*z);
	if (r <= EPSILON)
		return 1.0;

	// exploit trivial mirror symmetry
	z = abs(z);

	// exploit rotational & mirror symmetry
	if (x < r * COS36)
	{
		DBL rotSin;
		DBL rotCos;
		if (x < r *COS108)
		{
			rotSin = SIN144;
			rotCos = COS144;
		}
		else
		{
			rotSin = SIN72;
			rotCos = COS72;
		}
		DBL rotX = x*rotCos + z*rotSin;
		DBL rotZ = z*rotCos - x*rotSin;

		x =     rotX;
		z = abs(rotZ);
	}

	if (rhombs)
	{
		x *= INVPHI;
		z *= INVPHI;
	}

	DBL dist = abs( SIN108 * x - COS108 * z ) / COS18;

	int depth = max(0, (int)ceil(log(dist)/log(SQRPHI)));

	x *= pow(INVSQRPHI,depth);
	z *= pow(INVSQRPHI,depth);

	if (depth % 2)
	{
		DBL rotX = x*COS36 + z*SIN36;
		DBL rotZ = z*COS36 - x*SIN36;
		x = rotX;
		z = abs(rotZ);
	}

	depth *= 2;

	if (centerFlag)
	{
		depth += 1;
		x *= INVPHI;
		z *= INVPHI;
	}

	return tiling_penrose_halfkite(x, z, depth, rhombs);
}

static DBL tiling_penrose1_pentagon1 (DBL pX, DBL pZ, int depth);
static DBL tiling_penrose1_pentagon2 (DBL pX, DBL pZ, int depth, bool insideQuad);
static DBL tiling_penrose1_pentagon3 (DBL pX, DBL pZ, int depth, bool insideWedge);
static DBL tiling_penrose1_star (DBL pX, DBL pZ, int depth);
static DBL tiling_penrose1_boat (DBL pX, DBL pZ, int depth, bool insideWedge);
static DBL tiling_penrose1_diamond (DBL pX, DBL pZ, int depth, bool sideA);

static void tiling_penrose1_pentagon_symmetry(DBL& x, DBL& z, DBL r)
{
	z = abs(z);

	if (x < r * COS36)
	{
		DBL rotSin;
		DBL rotCos;
		if (x < r *COS108)
		{
			rotSin = SIN144;
			rotCos = COS144;
		}
		else
		{
			rotSin = SIN72;
			rotCos = COS72;
		}
		DBL rotX = x*rotCos + z*rotSin;
		DBL rotZ = z*rotCos - x*rotSin;

		x =     rotX;
		z = abs(rotZ);
	}
}

static void tiling_penrose1_pentagon_symmetry(DBL& x, DBL& z)
{
	tiling_penrose1_pentagon_symmetry (x, z, sqrt(x*x+z*z));
}

static DBL tiling_penrose1_pentagon_dist (DBL pX, DBL pZ)
{
	return abs( pX - 0.5/TAN36 ) * 5.55 * INVPHI; // TODO FIXME - the factor is just an empiric value
}

static DBL tiling_penrose1_pentagon1 (DBL pX, DBL pZ, int depth)
{
	DBL x = pX;
	DBL z = abs(pZ);

	tiling_penrose1_pentagon_symmetry (x, z);

	if (depth > 0)
	{
		if (z < (x - 0.5/TAN36) * TAN54 + 0.5)
		{
			DBL rotX = x - 0.5/TAN36 - INVPHI*0.5*COS72/SIN36;
			DBL rotZ = z;
			return tiling_penrose1_pentagon2 (rotX*PHI, rotZ*PHI, depth-1, false);
		}
		else
		{
			DBL rotX = x*COS36 + z*SIN36;
			DBL rotZ = z*COS36 - x*SIN36;
			return tiling_penrose1_star (rotX*PHI, rotZ*PHI, depth-1);
		}
	}
	else
	{
		DBL dist = tiling_penrose1_pentagon_dist (x, z);
		return min(max(1.0/6-dist/6,0.0),1.0/6-TILING_EPSILON);
	}
}

static DBL tiling_penrose1_pentagon2 (DBL pX, DBL pZ, int depth, bool insideQuad)
{
	DBL x = pX;
	DBL z = abs(pZ);

	if (depth > 0)
	{
		if (insideQuad)
		{
			if (z < (x - INVSQRPHI*0.5/SIN36) * TAN54)
			{
				DBL rotX = x - 0.5/SIN36;
				DBL rotZ = z;
				return tiling_penrose1_pentagon1 (rotX*PHI, rotZ*PHI, depth-1);
			}
			else if (z < (x - INVSQRPHI*0.5/SIN36) * TAN162)
			{
				DBL rotX = -x;
				DBL rotZ =  z;
				return tiling_penrose1_diamond (rotX*PHI, rotZ*PHI, depth-1, true);
			}
			else
			{
				DBL rotX = x*COS108 - z*SIN108 + INVPHI*0.5/SIN36;
				DBL rotZ = z*COS108 + x*SIN108;
				return tiling_penrose1_pentagon2 (rotX*PHI, rotZ*PHI, depth-1, true);
			}
		}
		else if (z < (x + 0.5/SIN36) * TAN18)
		{
			DBL rotX = x + 0.5/SIN36 - INVSQRPHI*0.5/SIN36;
			DBL rotZ = z;
			return tiling_penrose1_diamond (rotX*PHI, rotZ*PHI, depth-1, false);
		}
		else
		{
			DBL rotX = - (x*COS36 + z*SIN36) - COS72*0.5/SIN36;
			DBL rotZ =   (z*COS36 - x*SIN36) - SIN72*0.5/SIN36;
			return tiling_penrose1_pentagon3 (rotX*PHI, rotZ*PHI, depth-1, false);
		}
	}
	else
	{
		tiling_penrose1_pentagon_symmetry (x, z);
		DBL dist = tiling_penrose1_pentagon_dist (x, z);
		return min(max(2.0/6-dist/6,1.0/6+TILING_EPSILON),2.0/6-TILING_EPSILON);
	}
}

static DBL tiling_penrose1_pentagon3 (DBL pX, DBL pZ, int depth, bool insideWedge)
{
	DBL x = pX;
	DBL z = abs(pZ);

	if (depth > 0)
	{
		if (insideWedge && (x > INVSQRPHI*0.5*COS72/SIN36))
		{
			DBL rotX = -(x - INVSQRPHI*0.5*COS72/SIN36 - INVPHI*0.5/TAN36);
			DBL rotZ =   z;
			return tiling_penrose1_pentagon2 (rotX*PHI, rotZ*PHI, depth-1, true);
			return 1.0/6 + TILING_EPSILON;
		}
		else if (!insideWedge && (x < 0.5*COS108/SIN36))
		{
			DBL rotX = x*COS144 + z*SIN144 - 0.5/SIN36;
			DBL rotZ = z*COS144 - x*SIN144;
			return tiling_penrose1_pentagon2 (rotX*PHI, rotZ*PHI, depth-1, false);
		}
		else if (!insideWedge && (z > (x - INVSQRPHI*0.5/SIN36)*TAN126))
		{
			DBL rotX = - (x*COS36 - z*SIN36) - COS72*0.5/SIN36;
			DBL rotZ =   (z*COS36 + x*SIN36) - SIN72*0.5/SIN36;
			return tiling_penrose1_pentagon3 (rotX*PHI, rotZ*PHI, depth-1, false);
		}
		else
		{
			return tiling_penrose1_boat (x*PHI, z*PHI, depth-1, insideWedge);
		}
	}
	else
	{
		tiling_penrose1_pentagon_symmetry (x, z);
		DBL dist = tiling_penrose1_pentagon_dist (x, z);
		return min(max(3.0/6-dist/6,2.0/6+TILING_EPSILON),3.0/6-TILING_EPSILON);
	}
}

static DBL tiling_penrose1_star (DBL pX, DBL pZ, int depth)
{
	DBL x = pX;
	DBL z = abs(pZ);

	if (depth > 0)
	{
		if (x < INVPHI * 0.5/TAN36)
		{
			return tiling_penrose1_pentagon1(x*PHI, z*PHI, depth-1);
		}
		else
		{
			DBL rotX = -(x - INVPHI/TAN36);
			DBL rotZ =   z;
			return tiling_penrose1_pentagon3(rotX*PHI, rotZ*PHI, depth-1, true);
		}
	}
	else
	{
		DBL dist = abs( SIN162 * (x - PHI*0.5/SIN36) - COS162 * z) * 5.55 * INVPHI; // TODO FIXME - the factor is just an empiric value
		return min(max(4.0/6-dist/6,3.0/6+TILING_EPSILON),4.0/6-TILING_EPSILON);
	}
}

static DBL tiling_penrose1_boat (DBL pX, DBL pZ, int depth, bool insideWedge)
{
	DBL x = pX;
	DBL z = abs(pZ);

	if (depth > 0)
	{
		if (insideWedge && (x > PHI*0.5*COS108/SIN36))
		{
			DBL rotX = -x;
			DBL rotZ =  z;
			return tiling_penrose1_pentagon1 (rotX*PHI, rotZ*PHI, depth-1);
		}
		else
		{
			DBL rotX, rotZ;
			if (insideWedge)
			{
				rotX = x;
				rotZ = z;
			}
			else
			{
				rotX = x*COS72 - z*SIN72;
				rotZ = z*COS72 + x*SIN72;
			}
			rotX += 0.5/SIN36;
			return tiling_penrose1_pentagon3 (rotX*PHI, rotZ*PHI, depth-1, true);
		}
	}
	else
	{
		DBL dist1 = abs( x - INVPHI*0.5*COS72/SIN36) * 5.55 * INVPHI; // TODO FIXME - the factor is just an empiric value
		x = -x;
		tiling_penrose1_pentagon_symmetry (x, z);
		DBL dist2 = abs( SIN162 * (x - PHI*0.5/SIN36) - COS162 * z) * 5.55 * INVPHI; // TODO FIXME - the factor is just an empiric value
		return min(max3(5.0/6-dist1/6,5.0/6-dist2/6,4.0/6+TILING_EPSILON),5.0/6-TILING_EPSILON);
	}
}

static DBL tiling_penrose1_diamond (DBL pX, DBL pZ, int depth, bool sideA)
{
	DBL x = pX;
	DBL z = abs(pZ);

	if (depth > 0)
	{
		if (sideA)
		{
			return tiling_penrose1_pentagon1(x*PHI, z*PHI, depth-1);
		}
		else
		{
			return tiling_penrose1_pentagon3(x*PHI, z*PHI, depth-1, true);
		}
	}
	else
	{
		DBL dist = abs( SIN18 * (x + INVPHI*0.5/SIN36) - COS18 * z) * 5.55 * INVPHI; // TODO FIXME - the factor is just an empiric value
		return min(max(6.0/6-dist/6,5.0/6+TILING_EPSILON),6.0/6-TILING_EPSILON);
	}
}

static DBL tiling_penrose1 (const VECTOR EPoint, bool centerFlag)
{
	// Penrose P1 ("pentagon, star, boat and diamond") tiling
	//   centerFlag=false: pentagon at center
	//   centerFlag=true:  pentagram (star) at center

	DBL x,z;
	x = EPoint[X];
	z = EPoint[Z];

	DBL r = sqrt(x*x+z*z);
	if (r <= EPSILON)
		return 1.0;

	tiling_penrose1_pentagon_symmetry (x, z, r);

	DBL dist = x * 2 * TAN36;

	int depth = max(0, (int)ceil(log(dist)/log(SQRPHI*SQRPHI)));

	x *= pow(INVSQRPHI*INVSQRPHI,depth);
	z *= pow(INVSQRPHI*INVSQRPHI,depth);

	depth *= 4;

	/*
	if (centerFlag)
	{
		depth += 1;
		x *= INVPHI;
		z *= INVPHI;
	}
	*/

	return tiling_penrose1_pentagon1(x, z, depth);
}

static unsigned char digon[]={ 0x0E, 0x0B };
static unsigned char trigon[][6]=
{
	{ 0x0D, 0x05, 0x07 , 0x0D, 5,7 },
	{ 0x0E, 0x0D, 0x16, 0x49, 7, 0x0B }
};

static unsigned char tetragon[][16]=
{
	{ 0x0D, 5, 5, 7, 0x0D, 5, 5, 7, 0x0D, 5, 5, 7, 0x0D, 5, 5, 7 },
	{ 0x0E, 0x0D, 5, 0x16, 0x49, 5, 7, 0x0B, 0x0E, 0x0D, 5, 0x16, 0x49, 5, 7, 0x0B },
	{ 0x0D, 0x34, 0x7, 0x0B, 0x07, 0x0B, 0x0D, 0x34, 0x0D, 0x34, 0x7, 0x0B, 0x07, 0x0B, 0x0D, 0x34},
	{ 0x0C, 0x06, 0x0C, 0x06, 0x09, 0x03, 0x09, 0x03, 0x0C, 0x06, 0x0C, 0x06, 0x09, 0x03, 0x09, 0x03 },
	{ 7, 0x2C, 7, 0x2C, 0x0D, 0x83, 0x0D, 0x83, 0x2C, 7, 0x2C,7, 0x83, 0x0D, 0x83, 0x0D  }
};

static unsigned char pentagon[][10]=
{
	{ 0x0D, 5,5,5,7 , 0x0D, 5,5,5,7},
	{ 0x0E, 0x0D,5,5,0x16, 0x49, 5,5,7,0x0B},
	{ 7, 0x0C, 6, 0x0C, 0x24, 0x0D, 0x81, 3, 9, 3 },
	{ 0x0D, 5, 0x34, 7, 0x0B, 15,15,15,15,15 },
	{ 0x0D, 0xF0, 7, 0x0B, 0x0E , 15,15,15,15,15 },
	{ 0x0E, 0x0A, 0x49, 5, 7, 0x0D, 5, 0x16, 0x0A, 0x0B },
	{ 0x0B, 0x0E, 0x49, 0x16, 0x49, 7, 0x0D, 0x16, 0x49, 0x16 },
	{ 0x2C, 5, 0x16, 0x0B, 0x0E, 0x0B, 0x0E, 0x49, 5, 0x83 },
	{ 0x0E, 0x49, 5, 0x16, 0x0B, 0x0E, 0x49, 5, 0x16, 0x0B },
	{ 0x0E, 0x0D, 0xC1, 0x16, 0x0B, 0x49, 0x34, 7, 0x0B, 0x0E },
	{ 7, 0x2C, 5, 7, 0x2C, 0x0D, 0x83, 0x0D, 5, 0x83 },
	{ 0x0D, 0x34, 7, 0x0D, 0xC1, 7, 0x0A, 0x0B, 0x0E, 0x0A}
};

static unsigned char hexagon[][12]=
{
	{ 0x0D, 5, 5, 5, 5, 7, 0x0D, 5, 5, 5, 5, 7},
	{ 0x2C, 5, 5, 5, 7, 0x0E, 0x0B, 0x0D, 5, 5, 5, 0x83 },
	{ 0x0D, 0x34, 5, 5, 7, 0x0E, 7, 0x0B, 0x0D, 5, 5, 0xC1 },
	{ 0x0D, 5, 0x34, 5, 7, 0x0E, 5, 7, 0x0B, 0x0D, 5, 0xC1 },
	{ 0x0D, 0x83, 0x2C, 5, 5, 7 , 15,15,15,15,15,15},

	{ 0x0C, 0x24, 5, 7, 0x0C, 6, 9, 3, 0x0D, 5, 0x81, 3 },
	{ 7, 0x0C, 6, 0x0D, 0x14, 0x24, 0x0D, 0x81, 0x41, 7, 9, 3},
	{ 0x0D, 5, 0x83, 0x2C, 5, 7, 15,15,15,15,15,15},
	{ 7, 0x0C, 0x24, 7, 0x0C, 0x24, 0x0D, 0x81, 3, 0x0D, 0x81, 3},
	{ 0x0C, 4, 6, 0x0C, 4, 6, 9, 1, 3, 9, 1, 3 },

	{ 0x0E, 0x0D, 0x14, 6, 0x48, 6, 9, 0x12, 9, 0x41, 7, 0x0B},
	{ 0x0D, 5, 0x16, 0x49, 0x16, 0x0B, 15,15,15,15,15,15},
	{ 0x0D, 0x34, 5, 0x16, 0x0E, 0x0B, 0x0E, 0x0B, 0x49, 5, 0xC1, 7},
	{ 0x0D, 0x34, 7, 0x16, 0x0A, 0x0D, 0x0A, 0x49, 7, 0xC1, 7, 0x0D},
	{ 0x0D, 0x14, 0x82, 0x0E, 9, 3, 15,15,15,15,15,15},

	{ 0x0E, 0x0D, 0xD0, 6, 9, 3, 15,15,15,15,15,15},
	{ 0x0D, 0xF0, 5, 7, 0x0E, 0x0B, 0x0E, 0x0B, 0x0D, 5, 0xF0, 7},
	{ 0x0E, 0x0B, 0x0D, 5, 5, 0x92, 0x68, 5, 5, 7, 0x0E, 0x0B},
	{ 0x0E, 0x49, 5, 0x16, 0x0A, 0x0B, 15,15,15,15,15,15},
	{ 0x0B, 0x2C, 5, 5, 0x83, 0x0E, 15,15,15,15,15,15},

	{ 0x0B, 0x0A, 0x2C, 5, 5, 7, 15,15,15,15,15,15},
	{ 0x2C, 5, 5, 0x16, 0x0B, 0x0E, 0x0E, 0x0B, 5, 0x83, 0x49, 5},
	{ 0x0A, 0x0D, 0xC1, 0x16, 0x49, 0x34, 7, 0x0A, 0x0E, 0x0B, 0x0E, 0x0B},
	{ 0x0D, 0x16, 0x0D, 5, 0x92, 0x0B, 0x0E, 0x68, 5, 7, 0x49, 7},
	{ 0x0D, 0x16, 0x0D, 0x16, 0x16, 0x49, 0x16, 0x49, 0x49, 7, 0x49, 7},

	{ 0x0C, 6, 0x0C, 6, 9, 0x12, 9, 0x12, 7, 0x49, 7, 0x49 },
	{ 0x2C, 7, 0x2C, 7, 0x92, 0x0D, 0x92, 0x0D, 0x49, 7, 0x49, 7},
	{ 0x0E, 0x49, 5, 0x16, 0x49, 7, 15,15,15,15,15,15},
	{ 0x0D, 0x16, 0x0B, 0x0E, 0x49, 7, 0x16, 0x49, 5, 0x83, 0x2C, 5},
	{ 7, 0x0E, 0x49, 0x34, 7, 0x49, 0x0D, 0xC1, 0x16, 0x0B, 0x0D, 0x16},

	{ 0x2C, 5, 0xC1, 7, 0x0E, 0x0B, 0x0B, 0x0D, 0x34, 5, 0x83, 0x0E },
	{ 0x0E, 0x0B, 0x0D, 0x34, 0xC1, 7, 15,15,15,15,15,15},
	{ 0x0D, 5, 0x16, 0x2C, 7, 0x0A, 0x0A, 0x0D, 0x83, 0x49, 5, 7 },
	{ 0x2C, 0xC1, 5, 7, 0x0E, 0x0B, 0x0E, 0x0B, 0x0D, 5, 0x34, 0x83 },
	{ 0x0A, 0x0D, 5, 0x92, 0x68, 5, 7, 0x0A, 0x0B, 0x0E, 0x0E, 0x0B}
};

static DBL tetragonal (const VECTOR EPoint, const TPATTERN *TPat)
{
	unsigned char how;
	long xv,zv;
	DBL return_value=0;
	DBL value;
	DBL value1;
	DBL value2;
	DBL x,y;
	int lng=0;
	zv = floor(y=EPoint[Z]);
	xv = floor(x=EPoint[X]);
	switch(TPat->Vals.Pavement.Tile)
	{
		case 6:
			switch(TPat->Vals.Pavement.Number-1)
			{
				case 0:
				case 1:
				case 2:
				case 3:
				case 5:
				case 6:
				case 8:
				case 9:
					xv %= 6; if (xv < 0) { xv += 6 ;}
					zv &= 0x01;
					lng = 6;
					break;
				case 4:
				case 7:
				case 19:
				case 20:
					lng = 0;
					zv %= 6; if (zv <0) { zv += 6;}
					xv += 5*zv;
					xv %= 6; if (xv <0) { xv += 6; }
					break;
				case 11:
				case 18:
				case 27:
					lng = 0;
					zv %= 6; if (zv <0) { zv += 6;}
					xv += zv;
					xv %= 6; if (xv <0) { xv += 6; }
					break;
				case 10:
				case 12:
				case 21:
				case 22:
				case 24:
				case 25:
				case 26:
					lng = 4;
					xv &= 0x03;
					zv %= 3; if (zv<0) { zv += 3;}
					break;
				case 13:
				case 32:
					lng = 3;
					zv &= 0x03;
					xv %= 3; if (xv < 0) { xv += 3; }
					break;
				case 14:
					lng = 3;
					zv %= 6; if (zv < 0) { zv += 6; }
					xv += 2 * (zv/2);
					zv &= 0x01;
					xv %= 3; if (xv < 0) { xv += 3; }
					break;
				case 15:
					lng = 2;
					xv %= 6; if (xv < 0) { xv+= 6; }
					zv += (xv/2);
					xv &= 0x01;
					zv %= 3; if (zv<0) { zv += 3;}
					break;
				case 16:
				case 17:
					lng = 6;
					zv %= 12; if (zv <0) { zv+=12; }
					xv += zv/2;
					zv &= 0x01;
					xv %= 6; if (xv < 0) { xv+= 6; }
					break;
				case 23:
				case 28:
					lng = 6;
					zv %= 12; if (zv <0) { zv+=12; }
					xv += 4* (zv/2);
					zv &= 0x01;
					xv %= 6; if (xv < 0) { xv+= 6; }
					break;
				case 29:
				case 30:
					lng = 6;
					zv &= 0x03;
					xv += 3* (zv/2);
					zv &= 0x01;
					xv %= 6; if (xv < 0) { xv+= 6; }
					break;
				case 31:
					lng = 0;
					zv %= 3; if (zv <0) { zv+=3; }
					xv += 4* zv;
					xv %= 6; if (xv < 0) { xv+= 6; }
					break;
				case 33:
					lng = 0;
					zv %= 12; if (zv < 0) { zv+= 12; }
					xv += 7*zv;
					xv %= 12; if (xv < 0) { xv+= 12; }
					break;
				case 34:
					lng = 4;
					zv %= 6; if (zv<0) { zv+=6;}
					xv += 2 * (zv/3);
					xv &= 0x03;
					zv %= 3; if (zv<0) { zv += 3;}
					break;
			}
			how = hexagon[TPat->Vals.Pavement.Number-1][xv+zv*lng];
			break;
		case 5:
			switch(TPat->Vals.Pavement.Number-1)
			{
				case 0:
				case 1:
					xv %= 5; if (xv <0) { xv += 5 ; }
					zv &= 0x01;
					break;
				case 2:
				case 9:
					zv %= 10; if (zv <0) { zv += 10;}
					xv += 3 * (zv/2);
					xv %= 5; if (xv <0) { xv += 5;  }
					zv &= 0x01;
					break;
				case 10:
					zv %= 10; if (zv <0) { zv += 10;}
					xv += 4*(zv/2);
					xv %= 5; if (xv <0) { xv += 5 ;  }
					zv &= 0x01;
					break;
				case 3:
					zv %= 5; if (zv <0) { zv += 5;}
					xv += 2*zv;
					xv %= 5; if (xv <0) { xv += 5 ;  }
					zv = 0x0;
					break;
				case 4:
					zv %= 5; if (zv <0) { zv += 5;}
					xv += 2 * zv;
					xv %= 5; if (xv <0) { xv += 5;  }
					zv = 0x00;
					break;
				case 5:
				case 6:
				case 8:
					zv %= 10; if (zv <0) { zv += 10;}
					xv += zv;
					xv %= 10; if (xv <0) { xv += 10;  }
					zv = 0x00;
					break;
				case 11:
					zv %= 10; if (zv <0) { zv += 10;}
					xv += 8* zv;
					xv %= 10; if (xv <0) { xv += 10;  }
					zv = 0x00;
					break;
				case 7:
					zv %= 10; if (zv <0) { zv += 10;}
					xv += 3*zv;
					xv %= 10; if (xv <0) { xv += 10;  }
					zv = 0x00;
					break;
			}
			how = pentagon[TPat->Vals.Pavement.Number-1][xv+zv*5];
			break;
		case 4:
			xv &= 0x03;
			zv &= 0x03;
			how = tetragon[TPat->Vals.Pavement.Number-1][xv+zv*4];
			break;
		case 3:
			xv %= 3; if (xv < 0) { xv += 3; }
			zv &= 0x01;
			how = trigon[TPat->Vals.Pavement.Number-1][xv+zv*3];
			break;
		case 2:
			zv &= 0x01;
			how = digon[zv];
			break;
		case 1:
		default:
			how = 0x0F;
			break;
	}
	/* 
	 **   5---1---6
	 **   |       |
	 **   4       2
	 **   |       |
	 **   8---3---7
	 */
	x -= floor(x);
	y -= floor(y);
	switch(TPat->Vals.Pavement.Form)
	{
		case 2:
			if ((how & 0x16) == 0x16)
			{
				value1 = 2*x;
				value2 = 2 - 2*y;
				value = fabs(sqrt(value1*value1+value2*value2) - 1.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if ((how & 0x2C) == 0x2C)
			{
				value1 = 2 - 2*x;
				value2 = 2 - 2*y;
				value = fabs(sqrt(value1*value1+value2*value2) - 1.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if ((how & 0x49) == 0x49)
			{
				value1 = 2 - 2*x;
				value2 = 2*y;
				value = fabs(sqrt(value1*value1+value2*value2) - 1.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if ((how & 0x83) == 0x83)
			{
				value1 = 2*x;
				value2 = 2*y;
				value = fabs(sqrt(value1*value1+value2*value2) - 1.0);
				return_value = min(value,1.0);
				return return_value;
			}
			break;
		case 1:
			if ((how & 0x16) == 0x16)
			{
				value1 = 2*x;
				value2 = 2*y;
				value = fabs(value1 - value2 + 1.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if ((how & 0x2C) == 0x2C)
			{
				value1 = 2*x;
				value2 = 2 - 2*y;
				value = fabs(value2 - value1 + 1.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if ((how & 0x49) == 0x49)
			{
				value1 = 2*x;
				value2 = 2*y;
				value = fabs(value2 - value1 + 1.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if ((how & 0x83) == 0x83)
			{
				value1 = 2*x;
				value2 = 2 - 2*y;
				value = fabs(value1 - value2 + 1.0);
				return_value = min(value,1.0);
				return return_value;
			}
			break;
		case 0:
			if ((how & 0x16) == 0x16)
			{
				value1 = 2*x -1;
				value2 = 1 - 2*y;
				return_value = max(value1,value2);
				value1 = 1 - 2*x;
				value2 = 2*y - 1;
				value = min(value1,value2);
				return_value = max(return_value,value);
				return return_value;
			}
			if ((how & 0x2C) == 0x2C)
			{
				value1 = 1 - 2*x;
				value2 = 1 - 2*y;
				return_value = max(value1,value2);
				value1 = 2*x - 1;
				value2 = 2*y - 1;
				value = min(value1,value2);
				return_value = max(return_value,value);
				return return_value;
			}
			if ((how & 0x49) == 0x49)
			{
				value1 = 1 - 2*x;
				value2 = 2*y - 1;
				return_value = max(value1,value2);
				value1 = 2*x - 1;
				value2 = 1 - 2*y;
				value = min(value1,value2);
				return_value = max(return_value,value);
				return return_value;
			}
			if ((how & 0x83) == 0x83)
			{
				value1 = 2*x -1;
				value2 = 2*y -1;
				return_value = max(value1,value2);
				value1 = 1 - 2*x;
				value2 = 1 - 2*y;
				value = min(value1,value2);
				return_value = max(return_value,value);
				return return_value;
			}
			break;
		default:
		case 3:
			break;
	}
	if (how & 0x01)
	{
		value = 2*y - 1;
		return_value = max(return_value,value);
	}
	if (how & 0x02)
	{
		value = 2*x -1;
		return_value = max(return_value,value);
	}
	if (how & 0x04)
	{
		value = 1 - 2*y;
		return_value = max(return_value,value);
	}
	if (how & 0x08)
	{
		value = 1 - 2*x;
		return_value = max(return_value,value);
	}
	switch(TPat->Vals.Pavement.Interior)
	{
		case 2:
			if (how & 0x40)
			{
				value1 = 2 - 2*x;
				value2 = 2*y;
				value = 1.0- sqrt(value1*value1+value2*value2);
				return_value = max(return_value,value);
			}
			if (how & 0x80)
			{
				value1 = 2*x;
				value2 = 2*y;
				value = 1.0- sqrt(value1*value1+value2*value2);
				return_value = max(return_value,value);
			}
			if (how & 0x10)
			{
				value1 = 2*x;
				value2 = 2 - 2*y;
				value = 1.0- sqrt(value1*value1+value2*value2);
				return_value = max(return_value,value);
			}
			if (how & 0x20)
			{
				value1 = 2 - 2*x;
				value2 = 2 - 2*y;
				value = 1.0- sqrt(value1*value1+value2*value2);
				return_value = max(return_value,value);
			}
			break;
		case 1:
			if (how & 0x40)
			{
				value1 = 2 - 2*x;
				value2 = 2*y;
				value = 1.0- (value1+value2);
				return_value = max(return_value,value);
			}
			if (how & 0x80)
			{
				value1 = 2*x;
				value2 = 2*y;
				value = 1.0- (value1+value2);
				return_value = max(return_value,value);
			}
			if (how & 0x10)
			{
				value1 = 2*x;
				value2 = 2 - 2*y;
				value = 1.0- (value1+value2);
				return_value = max(return_value,value);
			}
			if (how & 0x20)
			{
				value1 = 2 - 2*x;
				value2 = 2 - 2*y;
				value = 1.0- (value1+value2);
				return_value = max(return_value,value);
			}
			break;
		default:
		case 0:
			if (how & 0x10)
			{
				value1 = 1 - 2*x;
				value2 = 2*y - 1;
				value = min(value1,value2);
				return_value = max(return_value,value);
			}
			if (how & 0x20)
			{
				value1 = 2*x - 1;
				value2 = 2*y - 1;
				value = min(value1,value2);
				return_value = max(return_value,value);
			}
			if (how & 0x40)
			{
				value1 = 2*x - 1;
				value2 = 1 - 2*y;
				value = min(value1,value2);
				return_value = max(return_value,value);
			}
			if (how & 0x80)
			{
				value1 = 1 - 2*x;
				value2 = 1 - 2*y;
				value = min(value1,value2);
				return_value = max(return_value,value);
			}
			break;
	}
	switch(TPat->Vals.Pavement.Exterior)
	{
		case 2:
			value1 = 2*x - 1;
			value2 = 2*y - 1;
			if ( (((how & 0x06) == 0x06)&&(value1>=0.0)&&(value2<=0.0)) ||
			     (((how & 0x0C) == 0x0C)&&(value1<=0.0)&&(value2<=0.0)) ||
			     (((how & 0x09) == 0x09)&&(value1<=0.0)&&(value2>=0.0)) ||
			     (((how & 0x03) == 0x03)&&(value1>=0.0)&&(value2>=0.0)) )
			{
				value = sqrt(value1*value1+value2*value2);
				value = min(value,1.0);
				return_value = max(return_value,value);
			}
			break;
		case 1:
			if ((how & 0x06) == 0x06)
			{
				value1 = 2 - 2*x;
				value2 = 2*y;
				value = 2.0- (value1+value2);
				value = min(value,1.0);
				return_value = max(return_value,value);
			}
			if ((how & 0x0C) == 0x0C)
			{
				value1 = 2*x;
				value2 = 2*y;
				value = 2.0- (value1+value2);
				value = min(value,1.0);
				return_value = max(return_value,value);
			}
			if ((how & 0x09) == 0x09)
			{
				value1 = 2*x;
				value2 = 2 - 2*y;
				value = 2.0- (value1+value2);
				value = min(value,1.0);
				return_value = max(return_value,value);
			}
			if ((how & 0x03) == 0x03)
			{
				value1 = 2 - 2*x;
				value2 = 2 - 2*y;
				value = 2.0- (value1+value2);
				value = min(value,1.0);
				return_value = max(return_value,value);
			}
			break;
		default:
		case 0:
			break;
	}
	return return_value;
}

static unsigned short tritrigon[][6]=
{
	{0x215,0x344,0x126,   0x126,0x344,0x215}
};

static unsigned short tritetragon[][8]=
{
	{0x126,0x126,0x144,0x144, 0x126,0x126,0x144,0x144},
	{0x611,0x344,0x126,0x423, 0x611,0x344,0x126,0x423},
	{5,0,6,3, 0,5,3,6}
};

static unsigned short tripentagon[][10]=
{
	{0x215,0x244,4,0x144,0x126, 0x126,0x144,4,0x244,0x215},
	{0x215,0x244,0x244,0x611,0x611, 0x244,0x244,0x215,0x423,0x423},
	{0x146,0x126,0x344,0x611,0x611, 0x344, 0x126,0x146,0x522,0x522},
	{5,0,0x244,0x215,3, 3,0x215,0x244,0,5}
};

static unsigned short trihexagon[][12]=
{
	{0x215,0x244,4,4,0x244,0x215, 0x215,0x244,4,4,0x244,0x215},
	{0x413,0x522,0x144,4,0x244,0x215, 0x413,0x522,0x144,4,0x244,0x215},
	{3,0x126,0x144,0,0x244,0x215, 3,0x126,0x144,0,0x244,0x215},
	{0x215,0x244,4,0,6,3, 4,0x244,0x215,3,6,0},
	{5,0,0x244,0x215,0x245,0x211, 5,0x211,0x245,0x215,0x244,0},
	{0x245,0x211,0x215,0x244,0x244,0x211, 0x245,0x211,0x244,0x244,0x215,0x211},
	{0x215,0x244,0x244,0x611, 0x522,0x146,0x146,0x522, 0x611,0x244,0x244,0x215},
	{5,0,0x244,0x215, 0x146,0x122,0x122,0x146, 0x215,0x244,0,5},
	{5,0,6,0x126,0x344,0x211, 5,0x211,0x344,0x126,6,0},
	{0x215,0x344,0x122,0x122,0x344,0x215, 0x215,0x344,0x122,0x122,0x344,0x215},
	{5,0,6,6,0,5, 6,0,5,5,0,6},
	{0x691,0x3C4,0x5A2,0x5A2,0x3C4,0x691, 0x5A2,0x3C4,0x691,0x691,0x3C4,0x5A2},
};

static DBL trigonal (const VECTOR EPoint, const TPATTERN *TPat)
{
	unsigned short how;
	long xv,zv;
	DBL return_value=0;
	DBL value;
	DBL value1;
	DBL value2;
	DBL dist1;
	DBL dist2;
	DBL dist3;
	DBL x,z;
	int lng=0;

	x=EPoint[X];
	z=EPoint[Z];

	xv = floor(x);
	zv = floor(z/SQRT3);
	x -= xv;
	z -= SQRT3*zv;
	/* x,z is in { [0.0, 1.0 [, [0.0, SQRT3 [ } 
	 ** There is some mirror to reduce the problem
	 */
	zv *= 2;
	xv *= 2;
	if ( z > SQRT3_2 )
	{
		z -= SQRT3_2;
		if (x>0.5)
		{
			x -= 0.5;
			xv++;
		}
		else
		{
			x += 0.5;
			xv--;
		}
		zv++;
	}
	if ((x == 0.0)||(z/x>SQRT3))
	{
		z = SQRT3_2 - z;
		x = 0.5 -x;
		xv--;
	}
	if ((x == 1.0)||(z/(1.0-x)>SQRT3))
	{
		z = SQRT3_2 - z;
		x = 1.5 -x;
		xv++;
	}
	switch(TPat->Vals.Pavement.Tile)
	{
		case 6:
			switch(TPat->Vals.Pavement.Number-1)
			{
				case 0:
				case 1:
				case 9:
					xv += 5*zv;
					zv = 0;
					xv %= 6; if (xv <0) { xv += 6;}
					lng = 0;
					break;
				case 2:
				case 10:
				case 11:
					zv &= 0x01;
					xv += 3*zv;
					xv %= 6; if (xv <0) { xv += 6;}
					lng = 0;
					break;
				case 3:
					xv += 14*((zv%6+((zv%6)<0?6:0))/2);
					xv %= 6; if (xv <0) { xv += 6;}
					lng = 6;
					zv &= 0x01;
					break;
				case 4:
				case 8:
					xv += 8*((zv%6+((zv%6)<0?6:0))/2);
					xv %= 6; if (xv <0) { xv += 6;}
					lng = 6;
					zv &= 0x01;
					break;
				case 5:
					xv %= 6; if (xv <0) { xv += 6;}
					lng = 6;
					zv &= 0x01;
					break;
				case 6:
				case 7:
					xv -= ((zv%12+((zv%12)<0?12:0))/3);
					xv &= 3;
					lng = 4;
					zv %= 3; if (zv <0) { zv +=3;}
					break;
			}
			how = trihexagon[TPat->Vals.Pavement.Number-1][xv+zv*lng];
			break;
		case 5:
			switch(TPat->Vals.Pavement.Number-1)
			{
				case 0:
				case 1:
				case 2:
					zv &= 0x01;
					xv += 5*zv;
					xv %= 10; if (xv <0) { xv += 10 ;  }
					zv = 0x00;
					break;
				case 3:
					zv %= 10; if (zv <0) { zv += 10; }
					xv += 3*zv;
					xv %= 10; if (xv <0) { xv += 10 ;  }
					zv = 0x00;
					break;
			}
			how = tripentagon[TPat->Vals.Pavement.Number-1][xv];
			break;
		case 4:
			zv &= 0x03;
			xv += zv;
			xv &= 0x03;
			zv &= 0x01;
			how = tritetragon[TPat->Vals.Pavement.Number-1][xv+zv*4];
			break;
		case 3:
			zv &= 0x01;
			xv += 3*zv;
			xv %= 6; if (xv < 0) { xv += 6; }
			zv = 0x00;
			how = tritrigon[TPat->Vals.Pavement.Number-1][xv];
			break;
		case 2:
			how = 0x166;
			break;
		case 1:
		default:
			how = 0x07;
			break;
	}

	/* 
	 **      / \
	 **     1   \
	 **    /     2
	 **   /       \
	 **  -----3-----
	 ** *3/2
	 */
	if (how & 0x01)
	{
		dist1 = 1.0 - (fabs(SQRT3 * x - z) * SQRT3 );
		return_value = max(return_value,dist1);
	}
	if (how & 0x02)
	{
		dist2 = 1.0 - (fabs(SQRT3 * (1.0-x) - z) * SQRT3  );
		return_value = max(return_value,dist2);
	}
	if (how & 0x04)
	{
		dist3 = 1.0 - (z * 2.0 * SQRT3 );
		return_value = max(return_value,dist3);
	}
	switch(TPat->Vals.Pavement.Interior)
	{
		case 1:
			dist1 = (1.0 - (fabs(SQRT3 * z + x) ));
			dist2 = (1.0 - (fabs(SQRT3 * z - x + 1.0) ));
			dist3 = (1.0 - (x * 2.0 ));
			if (((how & 0x83) == 0x00)&&(dist1<0)&&(dist2<0))
			{
				value1 = (3.0 / 2.0 *(fabs(SQRT3 * z + x) ) - 2.0);
				value2 = (3.0 / 2.0 *(fabs(SQRT3 * z - x + 1.0) ) - 2.0);
				value =  min(value1,value2);
				return_value = max(return_value,value);
			}
			if (((how & 0x85) == 0x00)&&(dist1>0)&&(dist3>0))
			{
				value1 = (1.0 - 3.0 / 2.0 * (fabs(SQRT3 * z + x) ));
				value2 = (1.0 - (x * 3.0 ));
				value =  min(value1,value2);
				return_value = max(return_value,value);
			}
			if (((how & 0x86) == 0x00)&&(dist3<0)&&(dist2>0))
			{
				value1 = (1.0 - 3.0 / 2.0 *(fabs(SQRT3 * z - x + 1.0) ));
				value2 = ((x * 3.0 ) - 2.0);
				value =  min(value1,value2);
				return_value = max(return_value,value);
			}
			break;
		case 2:
			if ((how & 0x83) == 0x00)
			{
				dist1 = x - 0.5;
				dist2 = z - SQRT3_2;
				dist3 = 1.0 - (sqrt((dist1*dist1+dist2*dist2))*3.0 );
				return_value = max(return_value,dist3);
			}
			if ((how & 0x85) == 0x00)
			{
				dist1 = x;
				dist2 = z;
				dist3 = 1.0 - (sqrt((dist1*dist1+dist2*dist2)) *3.0);
				return_value = max(return_value,dist3);
			}
			if ((how & 0x86) == 0x00)
			{
				dist1 = x - 1.0;
				dist2 = z ;
				dist3 = 1.0 - (sqrt((dist1*dist1+dist2*dist2)) *3.0);
				return_value = max(return_value,dist3);
			}
			break;
		case 0:
			if ((how & 0x83) == 0x00)
			{
				dist3 = 1.0 - ((SQRT3_2 - z) * 2.0 * SQRT3  );
				return_value = max(return_value,dist3);
			}
			if ((how & 0x85) == 0x00)
			{
				dist2 = 1.0 - (fabs(SQRT3 * x + z) * SQRT3 );
				return_value = max(return_value,dist2);
			}
			if ((how & 0x86) == 0x00)
			{
				dist1 = 1.0 - (fabs(SQRT3 * (x -1.0) - z) * SQRT3 );
				return_value = max(return_value,dist1);
			}
			break;
	}
	switch(TPat->Vals.Pavement.Exterior)
	{
		case 2:
			dist1 = (1.0 - (fabs(SQRT3 * z + x) ));
			dist2 = (1.0 - (fabs(SQRT3 * z - x + 1.0) ));
			dist3 = (1.0 - (x * 2.0 ));
			if ( (((how & 0x03) == 0x03)&&(dist1<=0.0)&&(dist2<=0.0)) ||
			     (((how & 0x06) == 0x06)&&(dist2>=0.0)&&(dist3<=0.0)) ||
			     (((how & 0x05) == 0x05)&&(dist1>=0.0)&&(dist3>=0.0)) )
			{
				value1 = x - 0.5;
				value2 = z - SQRT3_2/3.0;
				value = 2 * SQRT3 *sqrt(value1*value1+value2*value2);
				return_value = min(1.0,value);
			}
			break;
		case 1:
			/* dist1 = (1.0 - (fabs(SQRT3 * z + x) )); */
			dist1 = (1.0 - (fabs(SQRT3 * x - z)*SQRT3 ));
			/* dist2 = (1.0 - (fabs(SQRT3 * z - x + 1.0) )); */
			dist2 = (1.0 - (fabs(SQRT3 * (1.0 -x ) -z ) *SQRT3));
			/* dist3 = (1.0 - (x * 2.0 )); */
			dist3 = (1.0 - (z * 2.0 * SQRT3));
			value1 = (x - 0.5);
			value2 = (z - SQRT3_2/3.0);
			if  (((how & 0x03) == 0x03)&&(dist1>=0.0)&&(dist2>=0.0))
			{
				value = fabs(value2 * 2.0 * SQRT3 );
				return_value = min(1.0,value);
			}
			if  (((how & 0x06) == 0x06)&&(dist2>=0.0)&&(dist3>=0.0))
			{
				value = fabs(SQRT3 * value1 - value2) * SQRT3 ;
				return_value = min(1.0,value);
			}
			if  (((how & 0x05) == 0x05)&&(dist1>=0.0)&&(dist3>=0.0))
			{
				value = fabs(SQRT3 * value1 + value2) * SQRT3  ;
				return_value = min(1.0,value);
			}
			break;
		case 0:
		default:
			break;
	}
	dist1 = (1.0 - (fabs(SQRT3 * z + x) ));
	dist2 = (1.0 - (fabs(SQRT3 * z - x + 1.0) ));
	dist3 = (1.0 - (x * 2.0 ));
	switch(TPat->Vals.Pavement.Form)
	{
		case 2:
			if (((how & 0x120) == 0x120)&&(dist1<0)&&(dist2<0))
			{
				value1 = x;
				value2 = z;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x140) == 0x140)&&(dist1>0)&&(dist3>0))
			{
				value1 = x - 0.5;
				value2 = z- SQRT3_2;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x210) == 0x210)&&(dist1<0)&&(dist2<0))
			{
				value1 = x - 1.0;
				value2 = z;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x240) == 0x240)&&(dist3<0)&&(dist2>0))
			{
				value1 = x - 0.5;
				value2 = z - SQRT3_2;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x410) == 0x410)&&(dist1>0)&&(dist3>0))
			{
				value1 = x - 1.0;
				value2 = z;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x420) == 0x420)&&(dist3<0)&&(dist2>0))
			{
				value1 = x;
				value2 = z;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			break;
		case 1:
			if (((how & 0x120) == 0x120)&&(dist1<0)&&(dist2<0))
			{
				value = -dist1 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x140) == 0x140)&&(dist1>0)&&(dist3>0))
			{
				value = dist1 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x210) == 0x210)&&(dist1<0)&&(dist2<0))
			{
				value = -dist2 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x240) == 0x240)&&(dist3<0)&&(dist2>0))
			{
				value = dist2 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x410) == 0x410)&&(dist1>0)&&(dist3>0))
			{
				value = dist3 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x420) == 0x420)&&(dist2>0)&&(dist3<0))
			{
				value = -dist3 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x120) == 0x120)&&(dist1<0)&&(dist2<0))
			{
				value = -dist1 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			break;
		default:
		case 0:
			break;
	}
	return return_value;
}

/*
** open face (3 bits)
** special (1 bit)
** nexus vertex/open face (3 bits)
** unused (1 bits)
** close face (3 bits)
*/
static unsigned short hexmonogon[][6]=
{
	{0x691,0x3C4,0x5A2,0x5A2,0x3C4,0x691}
};

static unsigned short hexdigon[][12]=
{
	{0x691,0x3C4,0x1A2,0x1A2,0x3C4,0x691,0x5A2,0x1C4,0x6080,0x6080,0x1C4,0x5A2 }
};

static unsigned short hextrigon[][36]=
{
	{
		0x691,0x3C4,0x5A2,  0x5A2,0x3C4, 0x691,
		0x5A2,0x3C4,0x691,  0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x691,0x3C4,0x5A2
	},

	{
		0x691,0x3C4,0x1A2, 0x1A2,0x3C4,0x691,
		0x5A2,0x1C4,0x4080, 0x2080,0x2080,0x4A2,
		0x691,0x2C4,0x4080, 0x1080,0x1080,0x491,
		0x5A2,0x3C4,0x291, 0x291,0x3C4,0x5A2,
		0x491,0x1080,0x1080, 0x4080,0x2C4,0x691,
		0x4A2,0x2080,0x2080, 0x4080,0x1C4,0x5A2
	},

	{
		0x691,0x3C4,0x1A2, 0x1A2,0x3C4,0x291, 0x291,0x3C4,0x5A2,
		0x5A2,0x1C4,0x6080,0x6080,0x0C4,0x5080,0x5080,0x2C4,0x691,
		0x5A2,0x1C4,0x6080,0x6080,0x0C4,0x5080,0x5080,0x2C4,0x691,
		0x691,0x3C4,0x1A2, 0x1A2,0x3C4,0x291, 0x291,0x3C4,0x5A2
	}
};

static unsigned short hextetragon[][48]=
{
	{
		0x691,0x3C4,0x5A2,  0x5A2,0x3C4, 0x691,
		0x5A2,0x3C4,0x691,  0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x691,0x3C4,0x5A2
	},

	{
		0x691,0x2C4,0x4080, 0x1080,0x80,0x2080,
		0x4080,0x2C4,0x691, 0x291,0x3C4,0x1A2,
		0x4080,0x1C4,0x5A2, 0x1A2,0x3C4,0x291,
		0x5A2,0x1C4,0x4080, 0x2080,0x80,0x1080,
		0x691,0x2C4,0x4080, 0x1080,0x80,0x2080,
		0x4080,0x2C4,0x691, 0x291,0x3C4,0x1A2,
		0x4080,0x1C4,0x5A2, 0x1A2,0x3C4,0x291,
		0x5A2,0x1C4,0x4080, 0x2080,0x80,0x1080
	},

	{
		0x691,0x2C4,0x5080,  0x5080,0x0C4,0x6080,
		0x6080,0x0C4,0x5080, 0x5080,0x2C4,0x691,
		0x5A2,0x3C4,0x291, 0x291,0x3C4,0x1A2,
		0x1A2,0x3C4,0x291, 0x291,0x3C4,0x5A2,
		0x691,0x2C4,0x5080,  0x5080,0x0C4,0x6080,
		0x6080,0x0C4,0x5080, 0x5080,0x2C4,0x691,
		0x5A2,0x3C4,0x291, 0x291,0x3C4,0x1A2,
		0x1A2,0x3C4,0x291, 0x291,0x3C4,0x5A2,
	},

	{
		0x691,0x3C4,0x5A2,  0x4A2,0x3080,0x491,
		0x1A2,0x3C4,0x691,  0x691,0x3C4,0x5A2,
		0x6080,0x1C4,0x5A2, 0x1A2,0x3C4,0x691,
		0x5A2,0x1C4,0x6080, 0x6080,0x1C4,0x5A2,
		0x491,0x3080,0x0A2, 0x5A2,0x1C4,0x6080,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x0A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2
	},

	{
		0x691,0x3C4,0x1A2,  0x5A2,0x1C4,0x6080,
		0x5080,0x2C4,0x691, 0x491,0x3080,0x0A2,
		0x291,0x3C4,0x5A2,  0x4A2,0x3080,0x091,
		0x5A2,0x3C4,0x291,  0x691,0x2C4,0x5080,
		0x691,0x2C4,0x5080, 0x5080,0x2C4,0x691,
		0x1A2,0x3C4,0x691,  0x091,0x3080,0x4A2,
		0x6080,0x1C4,0x5A2, 0x0A2,0x3080,0x491,
		0x5A2,0x1C4,0x6080, 0x6080,0x1C4,0x5A2
	},

	{
		0x691,0x3C4,0x5A2,  0x0A2,0x3080,0x491,
		0x5A2,0x1C4,0x4080, 0x2080,0x2080,0x4A2,
		0x691,0x2C4,0x4080, 0x1080,0x1080,0x491,
		0x5A2,0x3C4,0x291,  0x291,0x3C4,0x5A2,
		0x491,0x1080,0x1080,0x4080,0x2C4,0x691,
		0x4A2,0x2080,0x2080,0x4080,0x1C4,0x5A2,
		0x491,0x3080,0x0A2, 0x5A2,0x3C4,0x691,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2
	},

	{
		0x691,0x3C4,0x5A2,  0x1A2,0x3C4,0x691, 0x091,0x3080,0x0A2, 0x5A2,0x3C4,0x291,
		0x5080,0x0C4,0x6080,0x6080,0x1C4,0x5A2,0x4A2,0x3080,0x491, 0x691,0x2C4,0x5080,
		0x091,0x3080,0x0A2, 0x5A2,0x3C4,0x291, 0x691,0x3C4,0x5A2,  0x1A2,0x3C4,0x691,
		0x4A2,0x3080,0x491, 0x691,0x2C4,0x5080,0x5080,0x0C4,0x6080,0x6080,0x1C4,0x5A2
	}
};

static unsigned short hexpentagon[][60]=
{ /* 0 */
	{
		0x691,0x3C4,0x5A2,  0x5A2,0x3C4, 0x691,
		0x5A2,0x3C4,0x691,  0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x691,0x3C4,0x5A2
	},
	/* 1 */
	{
		0x6080,0x1C4,0x5A2, 0x1A2,0x3C4,0x691,
		0x5A2,0x1C4,0x6080, 0x6080,0x1C4,0x5A2 ,
		0x491,0x3080,0x0A2, 0x5A2,0x1C4,0x6080,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x0A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x691,0x3C4,0x5A2,  0x4A2,0x3080,0x491,
		0x1A2,0x3C4,0x691,  0x691,0x3C4,0x5A2
	},
	/* 2 */
	{
		0x691,0x3C4,0x1A2,  0x1A2,0x3C4, 0x691,
		0x5A2,0x1C4,0x4080, 0x2080,0x2080,0x4A2,
		0x691,0x2C4,0x4080, 0x1080,0x1080,0x491,
		0x5A2,0x3C4,0x691,  0x091,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x091, 0x691,0x3C4,0x5A2,
		0x491,0x1080,0x1080,0x4080,0x2C4,0x691,
		0x4A2,0x2080,0x2080,0x4080,0x1C4,0x5A2
	},
	/* 3 */
	{
		0x691,0x3C4,0x5A2,  0x0A2,0x3080,0x491,
		0x5A2,0x1C4,0x4080, 0x2080,0x2080,0x4A2,
		0x691,0x2C4,0x4080, 0x1080,0x1080,0x491,
		0x5A2,0x3C4,0x691,  0x091,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x091, 0x691,0x3C4,0x5A2,
		0x491,0x1080,0x1080,0x4080,0x2C4,0x691,
		0x4A2,0x2080,0x2080,0x4080,0x1C4,0x5A2,
		0x491,0x3080,0x0A2, 0x5A2,0x3C4, 0x691,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2
	},
	/* 4 */
	{
		0x6080,0x1C4,0x5A2, 0x0A2,0x3080,0x491,
		0x5A2,0x1C4,0x6080, 0x6080,0x1C4,0x5A2,
		0x491,0x3080,0x0A2, 0x5A2,0x1C4,0x6080,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x0A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x691,0x3C4,0x5A2,  0x4A2,0x3080,0x491,
		0x5A2,0x3C4,0x691,  0x691,0x3C4,0x5A2,
		0x491,0x3080,0x4A2, 0x5A2,0x3C4,0x691,
		0x0A2,0x3080,0x491, 0x491,0x3080,0x4A2,
	},
	/* 5 */
	{
		0x691,0x3C4,0x1A2,  0x1A2,0x3C4, 0x691,
		0x5A2,0x1C4,0x4080, 0x2080,0x2080,0x4A2,
		0x491,0x1080,0x080, 0x080,0x1080,0x491,
		0x4A2,0x2080,0x2080,0x4080,0x1C4,0x5A2,
		0x491,0x3080,0x0A2, 0x5A2,0x3C4,0x691,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x691,0x3C4,0x5A2,  0x0A2,0x3080,0x491,
		0x5A2,0x1C4,0x4080, 0x2080,0x2080,0x4A2,
		0x491,0x1080,0x080, 0x080,0x1080,0x491,
		0x4A2,0x2080,0x2080,0x4080,0x1C4,0x5A2
	},
	/* 6 */
	{
		0x691,0x2C4,0x4080, 0x1080,0x1080, 0x491,
		0x5A2,0x3C4,0x291,  0x291,0x3C4,0x5A2,
		0x491,0x1080,0x1080,0x4080,0x2C4,0x691,
		0x4A2,0x2080,0x080, 0x080,0x2080,0x4A2,
		0x491,0x1080,0x080, 0x080,0x1080,0x491,
		0x4A2,0x2080,0x2080,0x4080,0x1C4,0x5A2,
		0x691,0x3C4,0x1A2,  0x1A2,0x3C4,0x691,
		0x5A2,0x1C4,0x4080, 0x2080,0x2080,0x4A2,
		0x491,0x1080,0x080, 0x080,0x1080,0x491,
		0x4A2,0x2080,0x080, 0x080,0x2080,0x4A2
	},
	/* 7 */
	{
		0x6080,0x1C4,0x5A2,
		0x5A2,0x1C4,0x4080,
		0x691,0x2C4,0x4080,
		0x4080,0x2C4,0x691,
		0x4080,0x1C4,0x5A2,
		0x5A2,0x1C4,0x6080,
		0x691,0x3C4,0x1A2,

		0x5A2,0x1C4,0x6080,
		0x491,0x3080,0x0A2,

		0x4A2,0x3080,0x091,
		0x491,0x1080,0x1080,
		0x4A2,0x2080,0x2080,
		0x691,0x3C4,0x1A2,
		0x1A2,0x3C4,0x691,
		0x2080,0x2080,0x4A2,
		0x1080,0x1080,0x491,
		0x091,0x3080,0x4A2,
		0x0A2,0x3080,0x491,
		0x6080,0x1C4,0x5A2,
		0x1A2,0x3C4,0x691
	},
	/* 8 */
	{
		0x6080,0x0C4,0x5080, 0x5080,0x2C4, 0x291,
		0x5A2,0x3C4,0x291,  0x291,0x2C4,0x5080,
		0x691,0x2C4,0x5080, 0x5080,0x2C4,0x691,
		0x5080,0x2C4,0x691, 0x091,0x3080,0x4A2,
		0x091,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x691,0x3C4,0x5A2, 0x0A2,0x3080,0x491,
		0x5A2,0x1C4,0x6080, 0x6080,0x1C4,0x5A2,

		0x691,0x3C4,0x1A2, 0x5A2,0x1C4,0x6080,
		0x1A2,0x3C4,0x291, 0x691,0x3C4,0x1A2
	},
	/* 9 */
	{
		0x691,0x3C4,0x1A2,  0x1A2,0x3C4,0x691,
		0x5080,0x0C4,0x6080,0x6080,0x1C4,0x5A2,
		0x291,0x3C4,0x1A2,  0x1A2,0x3C4,0x291,
		0x5A2,0x1C4,0x6080, 0x6080,0x0C4,0x5080,
		0x491,0x3080,0x0A2, 0x5A2,0x3C4,0x691,
		0x6A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x691,0x3C4,0x5A2,  0x0A2,0x3080,0x491,
		0x5080,0x0C4,0x6080,0x6080,0x1C4,0x5A2,
		0x291,0x3C4,0x1A2, 0x1A2,0x3C4,0x291,
		0x5A2,0x1C4,0x6080, 0x6080,0x0C4,0x5080,
	},
	/* 10 */
	{
		0x691,0x3C4,0x1A2,  0x1A2,0x3C4,0x691,
		0x5A2,0x1C4,0x4080, 0x2080,0x2080,0x4A2,
		0x491,0x1080,0x80, 0x80,0x1080,0x091,
		0x4A2,0x2080,0x2080, 0x4080,0x0C4,0x5080,
		0x691,0x3C4,0x1A2, 0x5A2,0x3C4,0x291,
		0x5080,0x2C4,0x691, 0x691,0x2C4,0x5080,
		0x291,0x3C4,0x5A2,  0x1A2,0x3C4,0x691,
		0x5080,0x0C4,0x4080, 0x2080,0x2080,0x4A2,
		0x091,0x1080,0x80, 0x80,0x1080,0x491,
		0x4A2,0x2080,0x2080, 0x4080,0x1C4,0x5A2
	},
	/* 11 */
	{
		0x291,0x3C4,0x5A2,  0x1A2,0x3C4,0x691,
		0x4080,0x0C4,0x4080, 0x2080,0x2080,0x4A2,
		0x4080,0x0C4,0x4080, 0x1080,0x1080,0x491,
		0x5A2,0x3C4,0x291,  0x291,0x3C4,0x5A2,
		0x491,0x1080,0x1080,0x4080,0x0C4,0x4080,
		0x4A2,0x2080,0x2080,0x4080,0x0C4,0x4080,
		0x691,0x3C4,0x1A2, 0x5A2,0x3C4,0x291,
		0x1A2,0x3C4,0x691, 0x491,0x1080,0x1080,
		0x2080,0x2080,0x4A2, 0x4A2,0x2080,0x2080,
		0x1080,0x1080,0x491, 0x691,0x3C4,0x1A2
	},
	/* 12 */
	{
		0x691,0x2C4,0x5080, 0x5080,0x2C4,0x691,
		0x5080,0x2C4,0x691, 0x091,0x3080,0x4A2,
		0x291,0x3C4,0x5A2,  0x0A2,0x3080,0x491,
		0x5080,0x0C4,0x6080,0x6080,0x1C4,0x5A2,
		0x291,0x3C4,0x1A2,  0x1A2,0x3C4,0x291,
		0x5A2,0x1C4,0x6080, 0x6080,0x0C4,0x5080,
		0x491,0x3080,0x0A2, 0x5A2,0x3C4,0x291,
		0x4A2,0x3080,0x091, 0x691,0x2C4,0x5080,
		0x691,0x2C4,0x5080, 0x5080,0x2C4,0x691,
		0x5A2,0x3C4,0x291,  0x291,0x3C4,0x5A2
	},
	/* 13 */
	{
		0x691,0x3C4,0x5A2,  0x5A2,0x3C4,0x691,
		0x5A2,0x3C4,0x691,  0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x091,
		0x4A2,0x3080,0x091, 0x691,0x2C4,0x5080,
		0x691,0x2C4,0x5080, 0x5080,0x2C4,0x291,
		0x5080,0x2C4,0x291, 0x291,0x2C4,0x5080,
		0x291,0x2C4,0x5080, 0x5080,0x2C4,0x691,
		0x5080,0x2C4,0x691, 0x091,0x3080,0x4A2,
		0x091,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x691,0x3C4,0x5A2
	},
	/* 14 */
	{
		0x291,0x3C4,0x5A2, 0x5A2,0x3C4,0x691,
		0x5A2,0x3C4,0x691, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2,0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491,0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2,0x4A2,0x3080,0x091,
		0x4A2,0x3080,0x091,0x691,0x2C4,0x5080,
		0x691,0x2C4,0x5080,0x5080,0x2C4,0x291,
		0x5080,0x2C4,0x291,0x291,0x2C4,0x5080,
		0x291,0x2C4,0x5080,0x5080,0x2C4,0x691,
		0x5080,0x2C4,0x691,0x291,0x3C4,0x5A2
	},
	/* 15 */
	{
		0x691,0x2C4,0x5080, 0x5080,0x0C4,0x4080,
		0x1A2,0x3C4,0x691,  0x291,0x2C4,0x4080,
		0x2080,0x2080,0x4A2,0x5A2,0x3C4,0x691,
		0x1080,0x1080,0x491,0x491,0x3080,0x4A2,
		0x091,0x3080,0x4A2, 0x4A2,0x3080,0x091,
		0x4A2,0x3080,0x491, 0x491,0x1080,0x1080,
		0x691,0x3C4,0x5A2,  0x4A2,0x2080,0x2080,
		0x4080,0x2C4,0x291, 0x691,0x3C4,0x1A2,
		0x4080,0x0C4,0x5080,0x5080,0x2C4,0x691,
		0x5A2,0x3C4,0x291,  0x291,0x3C4,0x5A2
	},
	/* 16 */
	{
		0x4080,0x1C4,0x1A2, 0x1A2,0x1C4,0x4080,
		0x5A2,0x1C4,0x6080, 0x6080,0x0C4,0x4080,
		0x691,0x3C4,0x1A2,  0x5A2,0x3C4,0x691,
		0x1A2,0x3C4,0x691,  0x491,0x3080,0x4A2,
		0x2080,0x2080,0x4A2,0x4A2,0x3080,0x091,
		0x1080,0x1080,0x491,0x491,0x1080,0x1080,
		0x091,0x3080,0x4A2, 0x4A2,0x2080,0x2080,
		0x4A2,0x3080,0x491, 0x691,0x3C4,0x1A2,
		0x691,0x3C4,0x5A2,  0x1A2,0x3C4,0x691,
		0x4080,0x0C4,0x6080,0x6080,0x1C4,0x5A2
	},
	/* 17 */
	{
		0x291,0x3C4,0x1A2,  0x1A2,0x3C4,0x291,
		0x5A2,0x1C4,0x6080, 0x6080,0x0C4,0x5080,
		0x691,0x3C4,0x1A2,  0x5A2,0x3C4,0x691,
		0x5080,0x2C4,0x691, 0x491,0x3080,0x4A2,
		0x091,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x091,
		0x4A2,0x3080,0x491, 0x691,0x2C4,0x5080,
		0x691,0x3C4,0x5A2,  0x1A2,0x3C4,0x691,
		0x5080,0x0C4,0x6080,0x6080,0x1C4,0x5A2
	},
	/* 18 */
	{
		0x091,0x3080,0x4A2, 0x5A2,0x3C4,0x691,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x691,0x3C4,0x5A2,  0x0A2,0x3080,0x091,
		0x5A2,0x1C4,0x6080, 0x6080,0x0C4,0x5080,
		0x691,0x3C4,0x1A2,  0x1A2,0x3C4,0x691,
		0x5080,0x0C4,0x6080,0x6080,0x1C4,0x5A2,
		0x091,0x3080,0x0A2, 0x5A2,0x3C4,0x691,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x691,0x3C4,0x5A2,  0x4A2,0x3080,0x091,
		0x5080,0x2C4,0x691, 0x691,0x2C4,0x5080
	},
	/* 19 */
	{
		0x691,0x3C4,0x1A2,  0x1A2,0x3C4,0x691,
		0x5A2,0x1C4,0x4080, 0x2080,0x2080,0x4A2,
		0x691,0x2C4,0x4080, 0x1080,0x1080,0x091,
		0x5080,0x2C4,0x691, 0x291,0x2C4,0x5080,
		0x091,0x3080,0x4A2, 0x5A2,0x3C4,0x691,

		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x691,0x3C4,0x5A2,  0x4A2,0x3080,0x091,
		0x5080,0x2C4,0x291, 0x691,0x2C4,0x5080,
		0x091,0x1080,0x1080,0x4080,0x2C4,0x691,
		0x4A2,0x2080,0x2080,0x4080,0x1C4,0x5A2
	},
	/* 20 */
	{
		0x291,0x3C4,0x5A2,  0x5A2,0x3C4,0x691,
		0x5A2,0x3C4,0x691,  0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x091,
		0x4A2,0x3080,0x091, 0x691,0x2C4,0x5080,
		0x691,0x2C4,0x5080, 0x5080,0x2C4,0x691,
		0x5080,0x2C4,0x691, 0x091,0x3080,0x4A2,
		0x091,0x3080,0x4A2, 0x4A2,0x3080,0x091,
		0x4A2,0x3080,0x091, 0x691,0x2C4,0x5080,
		0x691,0x2C4,0x5080, 0x5080,0x2C4,0x691,
		0x5080,0x2C4,0x691, 0x291,0x3C4,0x5A2
	},
	/* 21 */
	{
		0x691,0x3C4,0x1A2,  0x5A2,0x3C4,0x291,
		0x5080,0x2C4,0x691, 0x691,0x2C4,0x5080,
		0x291,0x3C4,0x5A2,  0x1A2,0x3C4,0x691,
		0x5080,0x0C4,0x6080,0x6080,0x1C4,0x5A2,
		0x091,0x3080,0x0A2, 0x5A2,0x3C4,0x691,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x491,0x3080,0x4A2, 0x4A2,0x3080,0x491,
		0x4A2,0x3080,0x491, 0x491,0x3080,0x4A2,
		0x691,0x3C4,0x5A2,  0x0A2,0x3080,0x091,
		0x5A2,0x1C4,0x6080, 0x6080,0x0C4,0x5080
	}
};

static DBL hexagonal (const VECTOR EPoint, const TPATTERN *TPat)
{
	unsigned short how;
	long xv,zv;
	DBL return_value=0;
	DBL value;
	DBL value1;
	DBL value2;
	DBL dist1;
	DBL dist2;
	DBL dist3;
	DBL x,z;
	int lng;

	x=EPoint[X];
	z=EPoint[Z];

	xv = floor(x);
	zv = floor(z/SQRT3);
	x -= xv;
	z -= SQRT3*zv;
	/* x,z is in { [0.0, 1.0 [, [0.0, SQRT3 [ } 
	 ** There is some mirror to reduce the problem
	 */
	zv *= 2;
	xv *= 2;
	if ( z > SQRT3_2 )
	{
		z -= SQRT3_2;
		if (x>0.5)
		{
			x -= 0.5;
			xv++;
		}
		else
		{
			x += 0.5;
			xv--;
		}
		zv++;
	}
	if ((x == 0.0)||(z/x>SQRT3))
	{
		z = SQRT3_2 - z;
		x = 0.5 -x;
		xv--;
	}
	if ((x == 1.0)||(z/(1.0-x)>SQRT3))
	{
		z = SQRT3_2 - z;
		x = 1.5 -x;
		xv++;
	}
	switch(TPat->Vals.Pavement.Tile)
	{
		case 5:
			switch(TPat->Vals.Pavement.Number-1)
			{
				case 0:
				case 2:
				case 3:
				case 5:
				case 6:
				case 19:
					zv %= 10; if (zv < 0) { zv += 10; }
					xv %= 6; if (xv < 0) { xv += 6; }
					xv += 6*zv; /* 60 */
					zv = 0;
					break;
				case 1:
				case 4:
				case 9:
					zv -= 2*(((xv%30+(xv%30<0?30:0))/6));
					zv %= 10; if (zv < 0) { zv += 10; }
					xv %= 6; if (xv < 0) { xv += 6; }
					xv += 6*zv; /* 60 */
					zv = 0;
					break;
				case 7:
					zv -= 7*(((xv%60+(xv%60<0?60:0))/3));
					zv %= 20; if (zv < 0) { zv += 20; }
					xv %= 3; if (xv < 0) { xv += 3; }
					xv += 3*zv; /* 60 */
					zv = 0;
					break;
				case 8:
				case 10:
				case 13:
				case 14:
				case 15:
				case 17:
				case 20:
				case 21:
					zv += 2*(((xv%30+(xv%30<0?30:0))/6));
					zv %= 10; if (zv < 0) { zv += 10; }
					xv %= 6; if (xv < 0) { xv += 6; }
					xv += 6*zv; /* 60 */
					zv = 0;
					break;
				case 11:
				case 16:
					zv -= 6*(((xv%30+(xv%30<0?30:0))/6));
					zv %= 10; if (zv < 0) { zv += 10; }
					xv %= 6; if (xv < 0) { xv += 6; }
					xv += 6*zv; /* 60 */
					zv = 0;
					break;
				case 12:
				case 18:
					zv += 6*(((xv%30+(xv%30<0?30:0))/6));
					zv %= 10; if (zv < 0) { zv += 10; }
					xv %= 6; if (xv < 0) { xv += 6; }
					xv += 6*zv; /* 60 */
					zv = 0;
					break;
			}
			how = hexpentagon[TPat->Vals.Pavement.Number-1][xv];
			break;
		case 4:
			switch(TPat->Vals.Pavement.Number-1)
			{
				case 0:
					zv &= 0x07;
					xv %= 6; if(xv <0) { xv += 6; }
					xv += 6*zv; /* 48 */
					zv = 0;
					break;
				case 3:
					zv -= 2*(((xv%24+(xv%24<0?24:0))/6));
					zv %= 8; if (zv < 0) { zv += 8; }
					xv %= 6; if (xv < 0) { xv += 6; }
					xv += 6*zv; /* 48 */
					zv = 0;
					break;
				case 2:
					zv &= 0x01;
					xv %= 12; if (xv < 0) { xv += 12; }
					xv += 12*zv; /* 24 */
					zv = 0;
					break;
				case 5:
				case 4:
					zv -= 2*(((xv%24+(xv%24<0?24:0))/6));
					zv %= 8; if (zv < 0) { zv += 8; }
					xv %= 6; if (xv < 0) { xv += 6; }
					xv += 6*zv; /* 48 */
					zv = 0;
					break;
				case 1:
					zv += 2*(((xv%12+(xv%12<0?12:0))/6));
					zv %= 8; if (zv < 0) { zv += 8; }
					xv %= 6; if (xv < 0) { xv += 6; }
					xv += 6*zv; /* 48 */
					zv = 0;
					break;
				case 6:
					zv %= 4; if (zv < 0) { zv += 4; }
					xv %= 12; if (xv < 0) { xv += 12; }
					xv += 12*zv; /* 48 */
					zv = 0;
					break;
			}
			how = hextetragon[TPat->Vals.Pavement.Number-1][xv];
			break;
		case 3:
			switch(TPat->Vals.Pavement.Number-1)
			{
				case 0:
					zv %= 6; if(zv <0) { zv += 6; }
					xv %= 6; if(xv <0) { xv += 6; }
					xv += 6*zv;
					zv = 0;
					break;
				case 1:
					zv += 2*(((xv%18+(xv%18<0?18:0))/6));
					zv %= 6; if(zv <0) { zv += 6; }
					xv %= 6; if(xv <0) { xv += 6; }
					xv += 6*zv;
					zv = 0;
					break;
				case 2:
					zv &= 0x01;
					xv %= 18; if (xv < 0) { xv += 18; }
					xv += 18*zv;
					zv = 0x00;
					break;
			}
			how = hextrigon[TPat->Vals.Pavement.Number-1][xv];
			break;
		case 2:
			zv &= 0x01;
			xv %= 6; if (xv < 0) { xv += 6; }
			how = hexdigon[TPat->Vals.Pavement.Number-1][xv+6*zv];
			break;
		case 1:
		default:
			zv &= 0x01;
			xv += 3*zv;
			xv %= 6; if (xv <0) { xv += 6;}
			lng = 0;
			how = hexmonogon[TPat->Vals.Pavement.Number-1][xv];
			break;
	}

	/* 
	 **      / \
	 **     1   \
	 **    /     2
	 **   /       \
	 **  -----3-----
	 ** height = sqrt(3)/2
	 */
	if (how & 0x01)
	{
		dist1 = 1.0 - (fabs(SQRT3 * x - z) * SQRT3 );
		return_value = max(return_value,dist1);
	}
	if (how & 0x02)
	{
		dist2 = 1.0 - (fabs(SQRT3 * (1.0-x) - z) * SQRT3  );
		return_value = max(return_value,dist2);
	}
	if (how & 0x04)
	{
		dist3 = 1.0 - (z * 2.0 * SQRT3 );
		return_value = max(return_value,dist3);
	}
	switch(TPat->Vals.Pavement.Interior)
	{
		case 1:
			dist1 = (1.0 - (fabs(SQRT3 * z + x) ));
			dist2 = (1.0 - (fabs(SQRT3 * z - x + 1.0) ));
			dist3 = (1.0 - (x * 2.0 ));
			if ( (((how & 0x83) == 0x00)||((how & 0x4000) == 0x4000)) &&
			     (dist1<0)&&(dist2<0) )
			{
				value1 = (3.0 / 2.0 *(fabs(SQRT3 * z + x) ) - 2.0);
				value2 = (3.0 / 2.0 *(fabs(SQRT3 * z - x + 1.0) ) - 2.0);
				value =  min(value1,value2);
				return_value = max(return_value,value);
			}
			if ((((how & 0x85) == 0x00)||((how & 0x2000) == 0x2000))
					&&(dist1>0)&&(dist3>0))
			{
				value1 = (1.0 - 3.0 / 2.0 * (fabs(SQRT3 * z + x) ));
				value2 = (1.0 - (x * 3.0 ));
				value =  min(value1,value2);
				return_value = max(return_value,value);
			}
			if ((((how & 0x86) == 0x00)||((how & 0x1000) == 0x1000))
					&&(dist3<0)&&(dist2>0))
			{
				value1 = (1.0 - 3.0 / 2.0 *(fabs(SQRT3 * z - x + 1.0) ));
				value2 = ((x * 3.0 ) - 2.0);
				value =  min(value1,value2);
				return_value = max(return_value,value);
			}
			break;
		case 2:
			if (((how & 0x83) == 0x00)||((how & 0x4000) == 0x4000))
			{
				dist1 = x - 0.5;
				dist2 = z - SQRT3_2;
				dist3 = 1.0 - (sqrt((dist1*dist1+dist2*dist2))*3.0 );
				return_value = max(return_value,dist3);
			}
			if (((how & 0x85) == 0x00)||((how & 0x2000) == 0x2000))
			{
				dist1 = x;
				dist2 = z;
				dist3 = 1.0 - (sqrt((dist1*dist1+dist2*dist2)) *3.0);
				return_value = max(return_value,dist3);
			}
			if (((how & 0x86) == 0x00)||((how & 0x1000) == 0x1000))
			{
				dist1 = x - 1.0;
				dist2 = z ;
				dist3 = 1.0 - (sqrt((dist1*dist1+dist2*dist2)) *3.0);
				return_value = max(return_value,dist3);
			}
			break;
		case 0:
			if (((how & 0x83) == 0x00)||((how & 0x4000) == 0x4000))
			{
				dist3 = 1.0 - ((SQRT3_2 - z) * 2.0 * SQRT3  );
				return_value = max(return_value,dist3);
			}
			if (((how & 0x85) == 0x00)||((how & 0x2000) == 0x2000))
			{
				dist2 = 1.0 - (fabs(SQRT3 * x + z) * SQRT3 );
				return_value = max(return_value,dist2);
			}
			if (((how & 0x86) == 0x00)||((how & 0x1000) == 0x1000))
			{
				dist1 = 1.0 - (fabs(SQRT3 * (x -1.0) - z) * SQRT3 );
				return_value = max(return_value,dist1);
			}
			break;
	}
	dist1 = (1.0 - (fabs(SQRT3 * z + x) ));
	dist2 = (1.0 - (fabs(SQRT3 * z - x + 1.0) ));
	dist3 = (1.0 - (x * 2.0 ));
	switch(TPat->Vals.Pavement.Form)
	{
		case 2:
			if (((how & 0x120) == 0x120)&&(dist1<0)&&(dist2<0))
			{
				value1 = x;
				value2 = z;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x140) == 0x140)&&(dist1>0)&&(dist3>0))
			{
				value1 = x - 0.5;
				value2 = z- SQRT3_2;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x210) == 0x210)&&(dist1<0)&&(dist2<0))
			{
				value1 = x - 1.0;
				value2 = z;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x240) == 0x240)&&(dist3<0)&&(dist2>0))
			{
				value1 = x - 0.5;
				value2 = z - SQRT3_2;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x410) == 0x410)&&(dist1>0)&&(dist3>0))
			{
				value1 = x - 1.0;
				value2 = z;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x420) == 0x420)&&(dist3<0)&&(dist2>0))
			{
				value1 = x;
				value2 = z;
				value = 2.0*SQRT3*(sqrt(value1*value1+value2*value2 ) - SQRT3/3.0 );
				value = max(value,0.0);
				return_value = min(value,1.0);
				return return_value;
			}
			break;
		case 1:
			if (((how & 0x120) == 0x120)&&(dist1<0)&&(dist2<0))
			{
				value = -dist1 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x140) == 0x140)&&(dist1>0)&&(dist3>0))
			{
				value = dist1 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x210) == 0x210)&&(dist1<0)&&(dist2<0))
			{
				value = -dist2 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x240) == 0x240)&&(dist3<0)&&(dist2>0))
			{
				value = dist2 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x410) == 0x410)&&(dist1>0)&&(dist3>0))
			{
				value = dist3 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x420) == 0x420)&&(dist2>0)&&(dist3<0))
			{
				value = -dist3 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			if (((how & 0x120) == 0x120)&&(dist1<0)&&(dist2<0))
			{
				value = -dist1 * 2;
				return_value = min(value,1.0);
				return return_value;
			}
			break;
		default:
		case 0:
			break;
	}
	return return_value;
}

/*****************************************************************************
*
* FUNCTION
*
*   tiling_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   J. Grimbert
*
* DESCRIPTION
*
*   Classical tiling patterns
*
* CHANGES
*
*
******************************************************************************/
static DBL tiling_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	switch(TPat->Vals.Tiling.Pattern)
	{
		case 27: return tiling_penrose(EPoint, true, false);
		case 26: return tiling_penrose(EPoint, false, false);
		case 25: return tiling_penrose1(EPoint, false);
		case 24: return tiling_dodeca_hex_5(EPoint);
		case 23: return tiling_dodeca_hex(EPoint);
		case 22: return tiling_dodeca_tri(EPoint);
		case 21: return tiling_square_tri(EPoint);
		case 20: return tiling_hexa_tri_left(EPoint);
		case 19: return tiling_hexa_tri_right(EPoint);
		case 18: return tiling_rectangle_pair(EPoint);
		case 17: return tiling_hexa_square_triangle_6(EPoint);
		case 16: return tiling_hexa_square_triangle(EPoint);
		case 15: return tiling_square_double(EPoint);
		case 14: return tiling_square_internal_5(EPoint);
		case 13: return tiling_square_internal(EPoint);
		case 12: return tiling_rectangle_square(EPoint);
		case 11: return tiling_square_rectangle(EPoint);
		case 10: return tiling_square_offset(EPoint);
		case 9: return tiling_hexa_triangle(EPoint);
		case 8: return tiling_square_triangle(EPoint);
		case 7: return tiling_octa_square(EPoint);
		case 6: return tiling_rectangle(EPoint);
		case 5: return tiling_rhombus(EPoint);
		case 4: return tiling_lozenge(EPoint);
		case 3: return tiling_triangle(EPoint);
		case 2: return tiling_hexagon(EPoint);
		case 1:
		default:
			return tiling_square(EPoint);
	}
}

/*****************************************************************************
*
* FUNCTION
*
*   tiling_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   J. Grimbert
*
* DESCRIPTION
*
*   Classical tiling patterns
*
* CHANGES
*
*
******************************************************************************/
static DBL pavement_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	switch(TPat->Vals.Pavement.Side)
	{
		case 6:
			return hexagonal(EPoint,TPat);

		case 4:
			return tetragonal(EPoint,TPat);

		case 3:
		default:
			return trigonal(EPoint,TPat);
	}
}

/*****************************************************************************
*
* FUNCTION
*
*   agate_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
*   Oct 1994    : adapted from agate pigment by [CY]
*
******************************************************************************/

static DBL agate_pattern (const VECTOR EPoint, const TPATTERN *TPat, int noise_generator)
{
	register DBL noise, turb_val;
	const TURB* Turb;

	Turb=Search_For_Turb(TPat->Warps);

	turb_val = TPat->Vals.Agate_Turb_Scale * Turbulence(EPoint,Turb,noise_generator);

	noise = 0.5 * (cycloidal(1.3 * turb_val + 1.1 * EPoint[Z]) + 1.0);

	if (noise < 0.0)
	{
		noise = 0.0;
	}
	else
	{
		noise = min(1.0, noise);
		noise = pow(noise, 0.77);
	}

	return(noise);
}


/*****************************************************************************
*
* FUNCTION
*
*   boxed_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   -
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL boxed_pattern (const VECTOR EPoint)
{
	register DBL value;

	value = max(fabs(EPoint[X]), max(fabs(EPoint[Y]), fabs(EPoint[Z])));
	CLIP_DENSITY(value);

	return(value);
}


/*****************************************************************************
*
* FUNCTION
*
*   brick_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value exactly 0.0 or 1.0
*   
* AUTHOR
*
*   Dan Farmer
*   
* DESCRIPTION
*
* CHANGES
*
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL brick_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int ibrickx, ibricky, ibrickz;
	DBL brickheight, brickwidth, brickdepth;
	DBL brickmortar, mortarheight, mortarwidth, mortardepth;
	DBL brickx, bricky, brickz;
	DBL x, y, z, fudgit;

	fudgit=EPSILON+TPat->Vals.Brick.Mortar;

	x =  EPoint[X]+fudgit;
	y =  EPoint[Y]+fudgit;
	z =  EPoint[Z]+fudgit;

	brickwidth  = TPat->Vals.Brick.Size[X];
	brickheight = TPat->Vals.Brick.Size[Y];
	brickdepth  = TPat->Vals.Brick.Size[Z];
	brickmortar = (DBL)TPat->Vals.Brick.Mortar;

	mortarwidth  = brickmortar / brickwidth;
	mortarheight = brickmortar / brickheight;
	mortardepth  = brickmortar / brickdepth;

	/* 1) Check mortar layers in the X-Z plane (ie: top view) */

	bricky = y / brickheight;
	ibricky = (int) bricky;
	bricky -= (DBL) ibricky;

	if (bricky < 0.0)
	{
		bricky += 1.0;
	}

	if (bricky <= mortarheight)
	{
		return(0.0);
	}

	bricky = (y / brickheight) * 0.5;
	ibricky = (int) bricky;
	bricky -= (DBL) ibricky;

	if (bricky < 0.0)
	{
		bricky += 1.0;
	}


	/* 2) Check ODD mortar layers in the Y-Z plane (ends) */

	brickx = (x / brickwidth);
	ibrickx = (int) brickx;
	brickx -= (DBL) ibrickx;

	if (brickx < 0.0)
	{
		brickx += 1.0;
	}

	if ((brickx <= mortarwidth) && (bricky <= 0.5))
	{
		return(0.0);
	}

	/* 3) Check EVEN mortar layers in the Y-Z plane (ends) */

	brickx = (x / brickwidth) + 0.5;
	ibrickx = (int) brickx;
	brickx -= (DBL) ibrickx;

	if (brickx < 0.0)
	{
		brickx += 1.0;
	}

	if ((brickx <= mortarwidth) && (bricky > 0.5))
	{
		return(0.0);
	}

	/* 4) Check ODD mortar layers in the Y-X plane (facing) */

	brickz = (z / brickdepth);
	ibrickz = (int) brickz;
	brickz -= (DBL) ibrickz;

	if (brickz < 0.0)
	{
		brickz += 1.0;
	}

	if ((brickz <= mortardepth) && (bricky > 0.5))
	{
		return(0.0);
	}

	/* 5) Check EVEN mortar layers in the X-Y plane (facing) */

	brickz = (z / brickdepth) + 0.5;
	ibrickz = (int) brickz;
	brickz -= (DBL) ibrickz;

	if (brickz < 0.0)
	{
		brickz += 1.0;
	}

	if ((brickz <= mortardepth) && (bricky <= 0.5))
	{
		return(0.0);
	}

	/* If we've gotten this far, color me brick. */

	return(1.0);
}


/*****************************************************************************
*
* FUNCTION
*
*   cells_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   John VanSickle
*
* DESCRIPTION
*
*   "cells":
*
*   New colour function by John VanSickle,
*     vansickl@erols.com
*
*   Assigns a pseudorandom value to each unit cube.  The value for the cube in
*   which the evaluted point lies is returned.
*
*   All "cells" specific source code and examples are in the public domain.
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL cells_pattern (const VECTOR EPoint)
{
	/* select a random value based on the cube from which this came. */

	/* floor the values, instead of just truncating - this eliminates duplicated cells
	around the axes */

	return min(PatternRands(Hash3d((int)floor(EPoint[X]+EPSILON), (int)floor(EPoint[Y]+EPSILON), (int)floor(EPoint[Z]+EPSILON))), 1.0);
}


/*****************************************************************************
*
* FUNCTION
*
*   checker_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0 or 1.0
*
* AUTHOR
*
*   POV-Team
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL checker_pattern (const VECTOR EPoint)
{
	int value;

	value = (int)(floor(EPoint[X]+EPSILON) +
	              floor(EPoint[Y]+EPSILON) +
	              floor(EPoint[Z]+EPSILON));

	if (value & 1)
	{
		return (1.0);
	}
	else
	{
		return (0.0);
	}
}


/*****************************************************************************
*
* FUNCTION
*
*   crackle_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   "crackle":
*
*   New colour function by Jim McElhiney,
*     CompuServe 71201,1326, aka mcelhiney@acm.org
*
*   Large scale, without turbulence, makes a pretty good stone wall.
*   Small scale, without turbulence, makes a pretty good crackle ceramic glaze.
*   Highly turbulent (with moderate displacement) makes a good marble, solving
*   the problem of apparent parallel layers in Perlin's method.
*   2 octaves of full-displacement turbulence make a great "drizzled paint"
*   pattern, like a 1950's counter top.
*   Rule of thumb:  put a single colour transition near 0 in your colour map.
*
*   Mathematically, the set crackle(p)=0 is a 3D Voronoi diagram of a field of
*   semirandom points, and crackle(p)>0 is distance from set along shortest path.
*   (A Voronoi diagram is the locus of points equidistant from their 2 nearest
*   neighbours from a set of disjoint points, like the membranes in suds are
*   to the centres of the bubbles).
*
*   All "crackle" specific source code and examples are in the public domain.
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*   Other changes: enhanced by Ron Parker, Integer math by Nathan Kopp
*
******************************************************************************/
static int IntPickInCube(int tvx, int tvy, int tvz, VECTOR  p1);

static DBL crackle_pattern (const VECTOR EPoint, const TPATTERN *TPat, TraceThreadData *Thread)
{
	DBL sum, minsum, minsum2, minsum3, tf;
	int minVecIdx = 0;
	VECTOR dv;

	int flox, floy, floz;

	DBL Metric = TPat->Vals.Crackle.Metric;
	DBL Offset = TPat->Vals.Crackle.Offset;

	bool UseSquare = ( Metric == 2);
	bool UseUnity  = ( Metric == 1);

	/*
	 * This uses floor() not FLOOR, so it will not be a mirror
	 * image about zero in the range -1.0 to 1.0. The viewer
	 * won't see an artefact around the origin.
	 */

	flox = (int)floor(EPoint[X] - EPSILON);
	floy = (int)floor(EPoint[Y] - EPSILON);
	floz = (int)floor(EPoint[Z] - EPSILON);

	/*
	 * Check to see if the input point is in the same unit cube as the last
	 * call to this function, to use cache of cubelets for speed.
	 */

	Crackle_Cell_Coord ccoord(flox, floy, floz);
	Thread->Stats()[CrackleCache_Tests]++;

	Crackle_Cache_Entry dummy_entry;
	Crackle_Cache_Entry* entry = &dummy_entry;

	// search for this hash value in the cache
	Crackle_Cache_Type::iterator iter = Thread->Crackle_Cache.find(ccoord);
	if (iter == Thread->Crackle_Cache.end())
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

		// generate a new cache entry, but only if the size of the cache is reasonable.
		// having to re-calculate entries that would have been cache hits had we not
		// skipped on adding an entry is less expensive than chewing up immense amounts
		// of RAM and finally hitting the swapfile. unfortunately there's no good way
		// to tell how much memory is 'too much' for the cache, so we just use a hard-
		// coded number for now (ideally we should allow the user to configure this).
		// keep in mind that the cache memory usage is per-thread, so the more threads,
		// the more RAM. if we don't do the insert, entry will point at a local variable.
		if (Thread->Crackle_Cache.size() * sizeof(Crackle_Cache_Type::value_type) < 30 * 1024 * 1024)
		{
			iter = Thread->Crackle_Cache.insert(Thread->Crackle_Cache.end(), Crackle_Cache_Type::value_type(ccoord, Crackle_Cache_Entry()));
			entry = &iter->second;
			entry->last_used = Thread->ProgressIndex();
		}

		// see InitializeCrackleCubes() below.
		int *pc = CrackleCubeTable;
		for (int i = 0; i < 81; i++, pc += 3)
			IntPickInCube(flox + pc[X], floy + pc[Y], floz + pc[Z], entry->data[i]);
	}
	else
	{
		Thread->Stats()[CrackleCache_Tests_Succeeded]++;
		entry = &iter->second;
	}

	// Find the 3 points with the 3 shortest distances from the input point.
	// Set up the loop so the invariant is true:  minsum <= minsum2 <= minsum3
	VSub(dv, entry->data[0], EPoint);

	if(UseSquare)
	{
		minsum = VSumSqr(dv);

		VSub(dv, entry->data[1], EPoint);
		minsum2 = VSumSqr(dv);

		VSub(dv, entry->data[2], EPoint);
		minsum3  = VSumSqr(dv);
	}
	else if(UseUnity)
	{
		minsum = fabs(dv[X]) + fabs(dv[Y]) + fabs(dv[Z]);

		VSub(dv, entry->data[1], EPoint);
		minsum2 = fabs(dv[X]) + fabs(dv[Y]) + fabs(dv[Z]);

		VSub(dv, entry->data[2], EPoint);
		minsum3 = fabs(dv[X]) + fabs(dv[Y]) + fabs(dv[Z]);
	}
	else
	{
		minsum = pow(fabs(dv[X]), Metric) +
		         pow(fabs(dv[Y]), Metric) +
		         pow(fabs(dv[Z]), Metric);

		VSub(dv, entry->data[1], EPoint);
		minsum2 = pow(fabs(dv[X]), Metric) +
		          pow(fabs(dv[Y]), Metric) +
		          pow(fabs(dv[Z]), Metric);

		VSub(dv, entry->data[2], EPoint);
		minsum3 = pow(fabs(dv[X]), Metric) +
		          pow(fabs(dv[Y]), Metric) +
		          pow(fabs(dv[Z]), Metric);
	}

	// sort the 3 computed sums
	if(minsum2 < minsum)
	{
		tf = minsum; minsum = minsum2; minsum2 = tf;
		minVecIdx = 1;
	}

	if(minsum3 < minsum)
	{
		tf = minsum; minsum = minsum3; minsum3 = tf;
		minVecIdx = 2;
	}

	if(minsum3 < minsum2)
	{
		tf = minsum2; minsum2 = minsum3; minsum3 = tf;
	}

	// Loop for the 81 cubelets to find closest and 2nd closest.
	for(int i = 3; i < 81; i++)
	{
		VSub(dv, entry->data[i], EPoint);

		if(UseSquare)
			sum  = VSumSqr(dv);
		else if(UseUnity)
			sum = fabs(dv[X]) + fabs(dv[Y]) + fabs(dv[Z]);
		else
			sum = pow(fabs(dv[X]), Metric) +
			      pow(fabs(dv[Y]), Metric) +
			      pow(fabs(dv[Z]), Metric);

		if(sum < minsum)
		{
			minsum3 = minsum2;
			minsum2 = minsum;
			minsum = sum;
			minVecIdx = i;
		}
		else if(sum < minsum2)
		{
			minsum3 = minsum2;
			minsum2 = sum;
		}
		else if( sum < minsum3 )
		{
			minsum3 = sum;
		}
	}

	if (Offset)
	{
		if(UseSquare)
		{
			minsum += Offset*Offset;
			minsum2 += Offset*Offset;
			minsum3 += Offset*Offset;
		}
		else if (UseUnity)
		{
			minsum += Offset;
			minsum2 += Offset;
			minsum3 += Offset;
		}
		else
		{
			minsum += pow( Offset, Metric );
			minsum2 += pow( Offset, Metric );
			minsum3 += pow( Offset, Metric );
		}
	}

	if(TPat->Vals.Crackle.IsSolid)
	{
		tf = Noise( entry->data[minVecIdx], GetNoiseGen(TPat, Thread) );
	}
	else if(UseSquare)
	{
		tf = TPat->Vals.Crackle.Form[X]*sqrt(minsum) +
		     TPat->Vals.Crackle.Form[Y]*sqrt(minsum2) +
		     TPat->Vals.Crackle.Form[Z]*sqrt(minsum3);
	}
	else if(UseUnity)
	{
		tf = TPat->Vals.Crackle.Form[X]*minsum +
		     TPat->Vals.Crackle.Form[Y]*minsum2 +
		     TPat->Vals.Crackle.Form[Z]*minsum3;
	}
	else
	{
		tf = TPat->Vals.Crackle.Form[X]*pow(minsum, 1.0/Metric) +
		     TPat->Vals.Crackle.Form[Y]*pow(minsum2, 1.0/Metric) +
		     TPat->Vals.Crackle.Form[Z]*pow(minsum3, 1.0/Metric);
	}

	return max(min(tf, 1.), 0.);
}


/*****************************************************************************
*
* FUNCTION
*
*   cylindrical_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   -
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL cylindrical_pattern (const VECTOR EPoint)
{
	register DBL value;

	value = sqrt(Sqr(EPoint[X]) + Sqr(EPoint[Z]));
	CLIP_DENSITY(value);

	return(value);
}


/*****************************************************************************
*
* FUNCTION
*
*   density_pattern
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

inline float intp3(float t, float fa, float fb, float fc, float fd)
{
	float b,d,e,f;

	b = (fc - fa) * 0.5;
	d = (fd - fb) * 0.5;
	e = 2.0 * (fb - fc) + b + d;
	f = -3.0 * (fb - fc) - 2.0 * b - d;

	return ((e * t + f) * t + b) * t + fb;
}

inline float intp3_2(float t, float fa, float fb, float fc, float fd)
{
	float b,e,f;

	e = fd - fc - fa + fb;
	f = fa - fb - e;
	b = fc - fa;

	return ((e * t + f) * t + b) * t + fb;
}

#define zmax(i,imax) (((i)<0)?(imax-1):((i) % (imax)))

static DBL density_pattern(const VECTOR EPoint, const TPATTERN *TPat)
{
	size_t x, y, z;
	size_t x1, y1, z1;
	size_t x2, y2, z2;
	DBL Ex, Ey, Ez;
	DBL xx, yy, zz;
	DBL xi, yi;
	DBL f111, f112, f121, f122, f211, f212, f221, f222;
	float intpd2[4][4];
	DBL density = 0.0;
	DENSITY_FILE_DATA *Data;
	size_t k0, k1, k2, k3, i,j,ii,jj;

	Ex=EPoint[X];
	Ey=EPoint[Y];
	Ez=EPoint[Z];

	if((TPat->Vals.Density_File != NULL) && ((Data = TPat->Vals.Density_File->Data) != NULL) &&
	   (Data->Sx) && (Data->Sy) && (Data->Sz))
	{
/*		if(Data->Cyclic == true) 
		{
			Ex -= floor(Ex);
			Ey -= floor(Ey);
			Ez -= floor(Ez);
		}
*/
		if((Ex >= 0.0) && (Ex < 1.0) && (Ey >= 0.0) && (Ey < 1.0) && (Ez >= 0.0) && (Ez < 1.0))
		{
			switch (TPat->Vals.Density_File->Interpolation % 10)
			{
				case NO_INTERPOLATION:
					x = (size_t)(Ex * (DBL)Data->Sx);
					y = (size_t)(Ey * (DBL)Data->Sy);
					z = (size_t)(Ez * (DBL)Data->Sz);

					if ((x < 0) || (x >= Data->Sx) || (y < 0) || (y >= Data->Sy) || (z < 0) || (z >= Data->Sz))
						density = 0.0;
					else
					{
						if(Data->Type == 4)
							density = (DBL)Data->Density32[z * Data->Sy * Data->Sx + y * Data->Sx + x] / (DBL)UINT_MAX;
						else if(Data->Type==2)
							density = (DBL)Data->Density16[z * Data->Sy * Data->Sx + y * Data->Sx + x] / (DBL)USHRT_MAX;
						else if(Data->Type == 1)
							density = (DBL)Data->Density8[z * Data->Sy * Data->Sx + y * Data->Sx + x] / (DBL)UCHAR_MAX;
					}
					break;
				case TRILINEAR_INTERPOLATION:
					xx = Ex * (DBL)(Data->Sx );
					yy = Ey * (DBL)(Data->Sy );
					zz = Ez * (DBL)(Data->Sz );

					x1 = (size_t)xx;
					y1 = (size_t)yy;
					z1 = (size_t)zz;

					x2 = (x1 + 1) % Data->Sx;
					y2 = (y1 + 1) % Data->Sy;
					z2 = (z1 + 1) % Data->Sz;

					xx -= floor(xx);
					yy -= floor(yy);
					zz -= floor(zz);

					xi = 1.0 - xx;
					yi = 1.0 - yy;

					if(Data->Type == 4)
					{
						f111 = (DBL)Data->Density32[z1 * Data->Sy * Data->Sx + y1 * Data->Sx + x1] / (DBL)UINT_MAX;
						f112 = (DBL)Data->Density32[z1 * Data->Sy * Data->Sx + y1 * Data->Sx + x2] / (DBL)UINT_MAX;
						f121 = (DBL)Data->Density32[z1 * Data->Sy * Data->Sx + y2 * Data->Sx + x1] / (DBL)UINT_MAX;
						f122 = (DBL)Data->Density32[z1 * Data->Sy * Data->Sx + y2 * Data->Sx + x2] / (DBL)UINT_MAX;
						f211 = (DBL)Data->Density32[z2 * Data->Sy * Data->Sx + y1 * Data->Sx + x1] / (DBL)UINT_MAX;
						f212 = (DBL)Data->Density32[z2 * Data->Sy * Data->Sx + y1 * Data->Sx + x2] / (DBL)UINT_MAX;
						f221 = (DBL)Data->Density32[z2 * Data->Sy * Data->Sx + y2 * Data->Sx + x1] / (DBL)UINT_MAX;
						f222 = (DBL)Data->Density32[z2 * Data->Sy * Data->Sx + y2 * Data->Sx + x2] / (DBL)UINT_MAX;
					}
					else if(Data->Type == 2)
					{
						f111 = (DBL)Data->Density16[z1 * Data->Sy * Data->Sx + y1 * Data->Sx + x1] / (DBL)USHRT_MAX;
						f112 = (DBL)Data->Density16[z1 * Data->Sy * Data->Sx + y1 * Data->Sx + x2] / (DBL)USHRT_MAX;
						f121 = (DBL)Data->Density16[z1 * Data->Sy * Data->Sx + y2 * Data->Sx + x1] / (DBL)USHRT_MAX;
						f122 = (DBL)Data->Density16[z1 * Data->Sy * Data->Sx + y2 * Data->Sx + x2] / (DBL)USHRT_MAX;
						f211 = (DBL)Data->Density16[z2 * Data->Sy * Data->Sx + y1 * Data->Sx + x1] / (DBL)USHRT_MAX;
						f212 = (DBL)Data->Density16[z2 * Data->Sy * Data->Sx + y1 * Data->Sx + x2] / (DBL)USHRT_MAX;
						f221 = (DBL)Data->Density16[z2 * Data->Sy * Data->Sx + y2 * Data->Sx + x1] / (DBL)USHRT_MAX;
						f222 = (DBL)Data->Density16[z2 * Data->Sy * Data->Sx + y2 * Data->Sx + x2] / (DBL)USHRT_MAX;
					}
					else if(Data->Type == 1)
					{
						f111 = (DBL)Data->Density8[z1 * Data->Sy * Data->Sx + y1 * Data->Sx + x1] / (DBL)UCHAR_MAX;
						f112 = (DBL)Data->Density8[z1 * Data->Sy * Data->Sx + y1 * Data->Sx + x2] / (DBL)UCHAR_MAX;
						f121 = (DBL)Data->Density8[z1 * Data->Sy * Data->Sx + y2 * Data->Sx + x1] / (DBL)UCHAR_MAX;
						f122 = (DBL)Data->Density8[z1 * Data->Sy * Data->Sx + y2 * Data->Sx + x2] / (DBL)UCHAR_MAX;
						f211 = (DBL)Data->Density8[z2 * Data->Sy * Data->Sx + y1 * Data->Sx + x1] / (DBL)UCHAR_MAX;
						f212 = (DBL)Data->Density8[z2 * Data->Sy * Data->Sx + y1 * Data->Sx + x2] / (DBL)UCHAR_MAX;
						f221 = (DBL)Data->Density8[z2 * Data->Sy * Data->Sx + y2 * Data->Sx + x1] / (DBL)UCHAR_MAX;
						f222 = (DBL)Data->Density8[z2 * Data->Sy * Data->Sx + y2 * Data->Sx + x2] / (DBL)UCHAR_MAX;
					}

					density = ((f111 * xi + f112 * xx) * yi + (f121 * xi + f122 * xx) * yy) * (1.0 - zz) +
					          ((f211 * xi + f212 * xx) * yi + (f221 * xi + f222 * xx) * yy) * zz;
					break;
				case TRICUBIC_INTERPOLATION:
				default:
					xx = Ex * (DBL)(Data->Sx);
					yy = Ey * (DBL)(Data->Sy);
					zz = Ez * (DBL)(Data->Sz);

					x1 = (size_t)xx;
					y1 = (size_t)yy;
					z1 = (size_t)zz;

					xx -= floor(xx);
					yy -= floor(yy);
					zz -= floor(zz);

					k0 = zmax(-1+z1, Data->Sz );
					k1 = zmax(   z1, Data->Sz );
					k2 = zmax( 1+z1, Data->Sz );
					k3 = zmax( 2+z1, Data->Sz );

					if(Data->Type == 4)
					{
						for(i = 0; i < 4; i++)
						{
							ii = zmax(i + x1 - 1, Data->Sx);
							for(j = 0; j < 4; j++)
							{
								jj = zmax(j + y1 - 1, Data->Sy);
								intpd2[i][j] = intp3(zz,
								                     Data->Density32[k0 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)UINT_MAX,
								                     Data->Density32[k1 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)UINT_MAX,
								                     Data->Density32[k2 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)UINT_MAX,
								                     Data->Density32[k3 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)UINT_MAX);
							}
						}
					}
					else if(Data->Type == 2)
					{
						for(i = 0; i < 4; i++)
						{
							ii = zmax(i + x1 - 1, Data->Sx);
							for(j = 0; j < 4; j++)
							{
								jj = zmax(j + y1 - 1, Data->Sy);
								intpd2[i][j] = intp3(zz,
								                     Data->Density16[k0 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)USHRT_MAX,
								                     Data->Density16[k1 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)USHRT_MAX,
								                     Data->Density16[k2 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)USHRT_MAX,
								                     Data->Density16[k3 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)USHRT_MAX);
							}
						}
					}
					else if(Data->Type == 1)
					{
						for(i = 0; i < 4; i++)
						{
							ii = zmax(i + x1 - 1, Data->Sx);
							for(j = 0; j < 4; j++)
							{
								jj = zmax(j + y1 - 1, Data->Sy);
								intpd2[i][j] = intp3(zz,
								                     Data->Density8[k0 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)UCHAR_MAX,
								                     Data->Density8[k1 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)UCHAR_MAX,
								                     Data->Density8[k2 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)UCHAR_MAX,
								                     Data->Density8[k3 * Data->Sy * Data->Sx + jj * Data->Sx + ii] / (DBL)UCHAR_MAX);
							}
						}
					}

					for(i = 0; i < 4; i++)
						intpd2[0][i] = intp3(yy, intpd2[i][0], intpd2[i][1],  intpd2[i][2], intpd2[i][3]);

					density = intp3(xx, intpd2[0][0], intpd2[0][1], intpd2[0][2], intpd2[0][3]);
					break;
			}
		}
		else
			density = 0.0;
	}

	if (density < 0.0)
		density = 0.0;
	return density;
}


/*****************************************************************************
*
* FUNCTION
*
*   dents_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/

static DBL dents_pattern (const VECTOR EPoint, int noise_generator)
{
	DBL noise;

	noise = Noise (EPoint, noise_generator);

	return(noise * noise * noise);
}


/*****************************************************************************
*
* FUNCTION
*
*   function_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
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

static DBL function_pattern (const VECTOR EPoint, const TPATTERN *TPat, TraceThreadData *Thread)
{
	DBL value;

	if(Thread->functionPatternContext[TPat->Vals.Function.Data] == NULL)
		Thread->functionPatternContext[TPat->Vals.Function.Data] = Thread->functionContext->functionvm->NewContext(const_cast<TraceThreadData *>(Thread));

	FPUContext *ctx = Thread->functionPatternContext[TPat->Vals.Function.Data];

	ctx->SetLocal(X, EPoint[X]);
	ctx->SetLocal(Y, EPoint[Y]);
	ctx->SetLocal(Z, EPoint[Z]);

	value = POVFPU_Run(ctx, *(reinterpret_cast<const FUNCTION*>(TPat->Vals.Function.Fn)));

	return ((value > 1.0) ? fmod(value, 1.0) : value);
}


/*****************************************************************************
*
* FUNCTION
*
*   gradient_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Gradient Pattern - gradient based on the fractional values of
*   x, y or z, based on whether or not the given directional vector is
*   a 1.0 or a 0.0.
*   The basic concept of this is from DBW Render, but Dave Wecker's
*   only supports simple Y axis gradients.
*
* CHANGES
*
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL gradient_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	DBL Result;
	VDot( Result, EPoint, TPat->Vals.Gradient );

	/* Mod to keep within [0.0,1.0] range */
	return ((Result > 1.0) ? fmod(Result, 1.0) : Result);
}


/*****************************************************************************
*
* FUNCTION
*
*   granite_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Granite - kind of a union of the "spotted" and the "dented" textures,
*   using a 1/f fractal noise function for color values. Typically used
*   with small scaling values. Should work with colour maps for pink granite.
*
* CHANGES
*
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL granite_pattern (const VECTOR EPoint, int noise_generator)
{
	register int i;
	register DBL temp, noise = 0.0, freq = 1.0;
	VECTOR tv1,tv2;

	VScale(tv1,EPoint,4.0);

	for (i = 0; i < 6 ; freq *= 2.0, i++)
	{
		VScale(tv2,tv1,freq);
		if(noise_generator==1)
		{
			temp = 0.5 - Noise (tv2, noise_generator);
			temp = fabs(temp);
		}
		else
		{
			temp = 1.0 - 2.0 * Noise (tv2, noise_generator);
			temp = fabs(temp);
			if (temp>0.5) temp=0.5;
		}

		noise += temp / freq;
	}

	return(noise);
}


/*****************************************************************************
*
* FUNCTION
*
*   hexagon_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0, 1.0 or 2.0
*
* AUTHOR
*
*   Ernest MacDougal Campbell III
*   
* DESCRIPTION
*
*   TriHex pattern -- Ernest MacDougal Campbell III (EMC3) 11/23/92
*
*   Creates a hexagon pattern in the XZ plane.
*
*   This algorithm is hard to explain.  First it scales the point to make
*   a few of the later calculations easier, then maps some points to be
*   closer to the Origin.  A small area in the first quadrant is subdivided
*   into a 6 x 6 grid.  The position of the point mapped into that grid
*   determines its color.  For some points, just the grid location is enough,
*   but for others, we have to calculate which half of the block it's in
*   (this is where the atan2() function comes in handy).
*
* CHANGES
*
*   Nov 1992 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

const DBL xfactor = 0.5;         /* each triangle is split in half for the grid */
const DBL zfactor = 0.866025404; /* sqrt(3)/2 -- Height of an equilateral triangle */

static DBL hexagon_pattern (const VECTOR EPoint)
{
	int xm, zm;
	int brkindx;
	DBL xs, zs, xl, zl, value = 0.0;
	DBL x=EPoint[X];
	DBL z=EPoint[Z];


	/* Keep all numbers positive.  Also, if z is negative, map it in such a
	 * way as to avoid mirroring across the x-axis.  The value 5.196152424
	 * is (sqrt(3)/2) * 6 (because the grid is 6 blocks high)
	 */

	x = fabs(x);

	/* Avoid mirroring across x-axis. */

	z = z < 0.0 ? 5.196152424 - fabs(z) : z;

	/* Scale point to make calcs easier. */

	xs = x/xfactor;
	zs = z/zfactor;

	/* Map points into the 6 x 6 grid where the basic formula works. */

	xs -= floor(xs/6.0) * 6.0;
	zs -= floor(zs/6.0) * 6.0;

	/* Get a block in the 6 x 6 grid. */

	xm = (int) FLOOR(xs) % 6;
	zm = (int) FLOOR(zs) % 6;

	switch (xm)
	{
		/* These are easy cases: Color depends only on xm and zm. */

		case 0:
		case 5:

			switch (zm)
			{
				case 0:
				case 5: value = 0; break;

				case 1:
				case 2: value = 1; break;

				case 3:
				case 4: value = 2; break;
			}

			break;

		case 2:
		case 3:

			switch (zm)
			{
				case 0:
				case 1: value = 2; break;

				case 2:
				case 3: value = 0; break;

				case 4:
				case 5: value = 1; break;
			}

			break;

		/* These cases are harder.  These blocks are divided diagonally
		 * by the angled edges of the hexagons.  Some slope positive, and
		 * others negative.  We flip the x value of the negatively sloped
		 * pieces.  Then we check to see if the point in question falls
		 * in the upper or lower half of the block.  That info, plus the
		 * z status of the block determines the color.
		 */

		case 1:
		case 4:

			/* Map the point into the block at the origin. */

			xl = xs-xm;
			zl = zs-zm;

			/* These blocks have negative slopes so we flip it horizontally. */

			if (((xm + zm) % 2) == 1)
			{
				xl = 1.0 - xl;
			}

			/* Avoid a divide-by-zero error. */

			if (xl == 0.0)
			{
				xl = 0.0001; // TODO FIXME - magic number! Should be SOME_EPSILON I guess, or use nextafter()
			}

			/* Is the angle less-than or greater-than 45 degrees? */

			brkindx = (zl / xl) < 1.0;

			/* was...
			 * brkindx = (atan2(zl,xl) < (45 * M_PI_180));
			 * ...but because of the mapping, it's easier and cheaper,
			 * CPU-wise, to just use a good ol' slope.
			 */

			switch (brkindx)
			{
				case true:

					switch (zm)
					{
						case 0:
						case 3: value = 0; break;

						case 2:
						case 5: value = 1; break;

						case 1:
						case 4: value = 2; break;
					}

					break;

				case false:

					switch (zm)
					{
						case 0:
						case 3: value = 2; break;

						case 2:
						case 5: value = 0; break;

						case 1:
						case 4: value = 1; break;
					}

					break;
			}
	}

	value = fmod(value, 3.0);

	return(value);
}


/*****************************************************************************
*
* FUNCTION
*
*   cubic_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0, 1.0, 2.0, 3.0, 4.0 or 5.0
*
* AUTHOR
*
*   Nieminen Juha
*   
* DESCRIPTION
*
*   Creates a cubic pattern. The six texture elements are mapped to the six
*   pyramids centered on the six axes.
*
* CHANGES
*
*   Nov 2007 : Creation.
*
******************************************************************************/

static DBL cubic_pattern (const VECTOR EPoint)
{
	const DBL x = EPoint[X], y = EPoint[Y], z = EPoint[Z];
	const DBL ax = fabs(x), ay = fabs(y), az = fabs(z);

	if(x >= 0 && x >= ay && x >= az) return 0.0;
	if(y >= 0 && y >= ax && y >= az) return 1.0;
	if(z >= 0 && z >= ax && z >= ay) return 2.0;
	if(x < 0 && x <= -ay && x <= -az) return 3.0;
	if(y < 0 && y <= -ax && y <= -az) return 4.0;
	return 5.0;
}

/*****************************************************************************
*
* FUNCTION
*
*   square_pattern 
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0, 1.0, 2.0 or 3.0
*
* AUTHOR
*
*   J. Grimbert
*
* DESCRIPTION
*   Paving the XZ plan with 4 'colours', in square
*
* CHANGES
*
******************************************************************************/

static DBL square_pattern (const VECTOR EPoint)
{
	int valueX,valueZ;

	valueX = (int)(floor(EPoint[X]));
	valueZ = (int)(floor(EPoint[Z]));

	if (valueX & 1)
	{
		if (valueZ & 1)
		{
			return (2.0);
		}
		else
		{
			return (3.0);
		}
	}
	else
	{
		if (valueZ & 1)
		{
			return (1.0);
		}
		else
		{
			return (0.0);
		}
	}
}

/*****************************************************************************
*
* FUNCTION
*
*   triangular_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0, 1.0, 2.0, 3.0, 4.0 or 5.0
*
* AUTHOR
*
*   J. Grimbert
*
* DESCRIPTION
*   Paving the XZ plan with 6 'colours', in triangle around the origin
*
* CHANGES
*
******************************************************************************/

static DBL triangular_pattern (const VECTOR EPoint)
{
	DBL answer;
	DBL x,z;
	DBL xs,zs;
	int a,b;
	DBL k,slop1,slop2;
	int mask;

	x=EPoint[X];
	z=EPoint[Z];
  /* Fold the space to a basic rectangle */
	xs = x-3.0*floor(x/3.0);
	zs = z-SQRT3*floor(z/SQRT3);

	/* xs,zs is in { [0.0, 3.0 [, [0.0, SQRT3 [ } 
	 ** but there is some symmetry to simplify the testing
	 */

	a = (int)floor(xs);
	xs -= a;
	b = (zs <SQRT3_2 ? 0: 1);
	if (b)
	{
		zs = SQRT3 - zs; /* mirror */
	}

	k = 1.0 - xs;
	if ((xs != 0.0)&&( k != 0.0 )) /* second condition should never occurs */
	{
		slop1 = zs/xs;
		slop2 = zs/k; /* just in case */
		switch( (slop1<SQRT3?1:0)+(slop2<SQRT3?2:0))
		{
			case 3:
				answer = 0.0;
				break;
			case 2:
				answer = 1.0;
				break;
			case 1:
				answer = 3.0;
				break;
		}
	}
	else
	{
		answer = 1.0;
	}
	mask = (int) answer;
	answer = (mask & 1) ? fmod(answer+2.0*a,6.0): fmod(6.0+answer-2.0*a,6.0);
	if (b)
	{
		answer = 5.0 - answer;
	}

	return answer;
}




/*****************************************************************************
*
* FUNCTION
*
*   julia_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL julia_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col;
	DBL a, b, cf, a2, b2, dist2, mindist2,
	    cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V];

	a = EPoint[X]; a2 = Sqr(a);
	b = EPoint[Y]; b2 = Sqr(b);
	mindist2 = a2+b2;

	it_max = TPat->Vals.Fractal.Iterations;

	for (col = 0; col < it_max; col++)
	{
		b  = 2.0 * a * b + ci;
		a  = a2 - b2 + cr;

		a2 = Sqr(a);
		b2 = Sqr(b);
		dist2 = a2+b2;

		if(dist2 < mindist2) mindist2 = dist2;
		if(dist2 > 4.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   julia3_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL julia3_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col;
	DBL a, b, cf, a2, b2, dist2, mindist2,
	    cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V];

	a = EPoint[X]; a2 = Sqr(a);
	b = EPoint[Y]; b2 = Sqr(b);
	mindist2 = a2+b2;

	it_max = TPat->Vals.Fractal.Iterations;

	for (col = 0; col < it_max; col++)
	{
		b = 3.0*a2*b - b2*b + ci;
		a = a2*a - 3.0*a*b2 + cr;

		a2 = Sqr(a);
		b2 = Sqr(b);
		dist2 = a2+b2;

		if(dist2 < mindist2) mindist2 = dist2;
		if(dist2 > 4.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   julia4_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL julia4_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col;
	DBL a, b, cf, a2, b2, dist2, mindist2,
	    cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V];

	a = EPoint[X]; a2 = Sqr(a);
	b = EPoint[Y]; b2 = Sqr(b);
	mindist2 = a2+b2;

	it_max = TPat->Vals.Fractal.Iterations;

	for (col = 0; col < it_max; col++)
	{
		b = 4.0 * (a2*a*b - a*b2*b) + ci;
		a = a2*a2 - 6.0*a2*b2 + b2*b2 + cr;

		a2 = Sqr(a);
		b2 = Sqr(b);
		dist2 = a2+b2;

		if(dist2 < mindist2) mindist2 = dist2;
		if(dist2 > 4.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   juliax_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL juliax_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col, exponent;
	DBL a, b, cf=0, x, y, dist2, mindist2,
	    cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V];
	int* binomial_coeff;

	a = x = EPoint[X];
	b = y = EPoint[Y];
	mindist2 = a*a+b*b;

	it_max = TPat->Vals.Fractal.Iterations;
	exponent = TPat->Vals.Fractal.Exponent;

	binomial_coeff = &BinomialCoefficients[(exponent+1)*exponent/2];

	for (col = 0; col < it_max; col++)
	{
		// Calculate (a+bi)^exponent
		DBL new_a = pow(a, exponent);
		for(int k=2; k<=exponent; k+=2)
		{
			new_a += binomial_coeff[k]*pow(a, exponent-k)*pow(b, k);
		}
		DBL new_b = 0;
		for(int l=1; l<=exponent; l+=2)
		{
			new_b += binomial_coeff[l]*pow(a, exponent-l)*pow(b, l);
		}

		a = new_a + cr;
		b = new_b + ci;

		dist2 = a*a+b*b;

		if(dist2 < mindist2) mindist2 = dist2;
		if(dist2 > 4.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   leopard_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Scott Taylor
*
* DESCRIPTION
*
* CHANGES
*
*   Jul 1991 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL leopard_pattern (const VECTOR EPoint)
{
	register DBL value, temp1, temp2, temp3;

	/* This form didn't work with Zortech 386 compiler */
	/* value = Sqr((sin(x)+sin(y)+sin(z))/3); */
	/* So we break it down. */

	temp1 = sin(EPoint[X]);
	temp2 = sin(EPoint[Y]);
	temp3 = sin(EPoint[Z]);

	value = Sqr((temp1 + temp2 + temp3) / 3.0);

	return(value);
}


/*****************************************************************************
*
* FUNCTION
*
*   magnet1m_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL magnet1m_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col;
	DBL a, b, cf, a2, b2, x, y, tmp, tmp1r, tmp1i, tmp2r, tmp2i, dist2, mindist2;

	x = EPoint[X];
	y = EPoint[Y];
	a = a2 = 0;
	b = b2 = 0;
	mindist2 = 10000;

	it_max = TPat->Vals.Fractal.Iterations;

	for (col = 0; col < it_max; col++)
	{
		tmp1r = a2-b2 + x-1;
		tmp1i = 2*a*b + y;
		tmp2r = 2*a + x-2;
		tmp2i = 2*b + y;
		tmp = tmp2r*tmp2r + tmp2i*tmp2i;
		a = (tmp1r*tmp2r + tmp1i*tmp2i) / tmp;
		b = (tmp1i*tmp2r - tmp1r*tmp2i) / tmp;
		b2 = b*b;
		b = 2*a*b;
		a = a*a-b2;

		a2 = Sqr(a);
		b2 = Sqr(b);
		dist2 = a2+b2;

		if(dist2 < mindist2) mindist2 = dist2;
		tmp1r = a-1;
		if(dist2 > 10000.0 || tmp1r*tmp1r+b2 < 1/10000.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   magnet1j_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL magnet1j_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col;
	DBL a, b, cf, a2, b2, tmp, tmp1r, tmp1i, tmp2r, tmp2i, dist2, mindist2,
	    cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V];

	a = EPoint[X]; a2 = Sqr(a);
	b = EPoint[Y]; b2 = Sqr(b);
	mindist2 = a2+b2;

	it_max = TPat->Vals.Fractal.Iterations;

	for (col = 0; col < it_max; col++)
	{
		tmp1r = a2-b2 + cr-1;
		tmp1i = 2*a*b + ci;
		tmp2r = 2*a + cr-2;
		tmp2i = 2*b + ci;
		tmp = tmp2r*tmp2r + tmp2i*tmp2i;
		a = (tmp1r*tmp2r + tmp1i*tmp2i) / tmp;
		b = (tmp1i*tmp2r - tmp1r*tmp2i) / tmp;
		b2 = b*b;
		b = 2*a*b;
		a = a*a-b2;

		a2 = Sqr(a);
		b2 = Sqr(b);
		dist2 = a2+b2;

		if(dist2 < mindist2) mindist2 = dist2;
		tmp1r = a-1;
		if(dist2 > 10000.0 || tmp1r*tmp1r+b2 < 1/10000.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   magnet2m_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL magnet2m_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col;
	DBL a, b, cf, a2, b2, x, y, tmp, tmp1r, tmp1i, tmp2r, tmp2i,
	    c1r, c2r, c1c2r, c1c2i, dist2, mindist2;

	x = EPoint[X];
	y = EPoint[Y];
	a = a2 = 0;
	b = b2 = 0;
	mindist2 = 10000;

	c1r = x-1; c2r = x-2;
	c1c2r = c1r*c2r-y*y;
	c1c2i = (c1r+c2r)*y;

	it_max = TPat->Vals.Fractal.Iterations;

	for (col = 0; col < it_max; col++)
	{
		tmp1r = a2*a-3*a*b2 + 3*(a*c1r-b*y) + c1c2r;
		tmp1i = 3*a2*b-b2*b + 3*(a*y+b*c1r) + c1c2i;
		tmp2r = 3*(a2-b2) + 3*(a*c2r-b*y) + c1c2r + 1;
		tmp2i = 6*a*b + 3*(a*y+b*c2r) + c1c2i;
		tmp = tmp2r*tmp2r + tmp2i*tmp2i;
		a = (tmp1r*tmp2r + tmp1i*tmp2i) / tmp;
		b = (tmp1i*tmp2r - tmp1r*tmp2i) / tmp;
		b2 = b*b;
		b = 2*a*b;
		a = a*a-b2;

		a2 = Sqr(a);
		b2 = Sqr(b);
		dist2 = a2+b2;

		if(dist2 < mindist2) mindist2 = dist2;
		tmp1r = a-1;
		if(dist2 > 10000.0 || tmp1r*tmp1r+b2 < 1/10000.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   magnet2j_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL magnet2j_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col;
	DBL a, b, cf, a2, b2, tmp, tmp1r, tmp1i, tmp2r, tmp2i, c1r,c2r,c1c2r,c1c2i,
	    cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V],
	    dist2, mindist2;

	a = EPoint[X]; a2 = Sqr(a);
	b = EPoint[Y]; b2 = Sqr(b);
	mindist2 = a2+b2;

	c1r = cr-1, c2r = cr-2;
	c1c2r = c1r*c2r-ci*ci;
	c1c2i = (c1r+c2r)*ci;

	it_max = TPat->Vals.Fractal.Iterations;

	for (col = 0; col < it_max; col++)
	{
		tmp1r = a2*a-3*a*b2 + 3*(a*c1r-b*ci) + c1c2r;
		tmp1i = 3*a2*b-b2*b + 3*(a*ci+b*c1r) + c1c2i;
		tmp2r = 3*(a2-b2) + 3*(a*c2r-b*ci) + c1c2r + 1;
		tmp2i = 6*a*b + 3*(a*ci+b*c2r) + c1c2i;
		tmp = tmp2r*tmp2r + tmp2i*tmp2i;
		a = (tmp1r*tmp2r + tmp1i*tmp2i) / tmp;
		b = (tmp1i*tmp2r - tmp1r*tmp2i) / tmp;
		b2 = b*b;
		b = 2*a*b;
		a = a*a-b2;

		a2 = Sqr(a);
		b2 = Sqr(b);
		dist2 = a2+b2;

		if(dist2 < mindist2) mindist2 = dist2;
		tmp1r = a-1;
		if(dist2 > 10000.0 || tmp1r*tmp1r+b2 < 1/10000.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   mandel_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   submitted by user, name lost (sorry)
*
* DESCRIPTION
*
*   The mandel pattern computes the standard Mandelbrot fractal pattern and
*   projects it onto the X-Y plane.  It uses the X and Y coordinates to compute
*   the Mandelbrot set.
*
* CHANGES
*
*   Oct 1994 : adapted from pigment by [CY]
*   May 2001 : updated with code from Warp [trf]
*
******************************************************************************/

static DBL mandel_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col;
	DBL a, b, cf, a2, b2, x, y, dist2, mindist2;

	a = x = EPoint[X]; a2 = Sqr(a);
	b = y = EPoint[Y]; b2 = Sqr(b);
	mindist2 = a2+b2;

	it_max = TPat->Vals.Fractal.Iterations;

	for (col = 0; col < it_max; col++)
	{
		b  = 2.0 * a * b + y;
		a  = a2 - b2 + x;

		a2 = Sqr(a);
		b2 = Sqr(b);
		dist2 = a2+b2;

		if(dist2 < mindist2) mindist2 = dist2;
		if(dist2 > 4.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   mandel3_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL mandel3_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col;
	DBL a, b, cf, a2, b2, x, y, dist2, mindist2;

	a = x = EPoint[X]; a2 = Sqr(a);
	b = y = EPoint[Y]; b2 = Sqr(b);
	mindist2 = a2+b2;

	it_max = TPat->Vals.Fractal.Iterations;

	for (col = 0; col < it_max; col++)
	{
		b = 3.0*a2*b - b2*b + y;
		a = a2*a - 3.0*a*b2 + x;

		a2 = Sqr(a);
		b2 = Sqr(b);
		dist2 = a2+b2;

		if(dist2 < mindist2) mindist2 = dist2;
		if(dist2 > 4.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   mandel4_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL mandel4_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col;
	DBL a, b, cf, a2, b2, x, y, dist2, mindist2;

	a = x = EPoint[X]; a2 = Sqr(a);
	b = y = EPoint[Y]; b2 = Sqr(b);
	mindist2 = a2+b2;

	it_max = TPat->Vals.Fractal.Iterations;

	for (col = 0; col < it_max; col++)
	{
		b = 4.0 * (a2*a*b - a*b2*b) + y;
		a = a2*a2 - 6.0*a2*b2 + b2*b2 + x;

		a2 = Sqr(a);
		b2 = Sqr(b);
		dist2 = a2+b2;

		if(dist2 < mindist2) mindist2 = dist2;
		if(dist2 > 4.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   mandelx_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Nieminen Juha
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL mandelx_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	int it_max, col, exponent;
	DBL a, b, cf=0, x, y, dist2, mindist2;
	int* binomial_coeff;

	a = x = EPoint[X];
	b = y = EPoint[Y];
	mindist2 = a*a+b*b;

	it_max = TPat->Vals.Fractal.Iterations;
	exponent = TPat->Vals.Fractal.Exponent;

	binomial_coeff = &BinomialCoefficients[(exponent+1)*exponent/2];

	for (col = 0; col < it_max; col++)
	{
		// Calculate (a+bi)^exponent
		DBL new_a = pow(a, exponent);
		for(int k=2; k<=exponent; k+=2)
		{
			new_a += binomial_coeff[k]*pow(a, exponent-k)*pow(b, k);
		}
		DBL new_b = 0;
		for(int l=1; l<=exponent; l+=2)
		{
			new_b += binomial_coeff[l]*pow(a, exponent-l)*pow(b, l);
		}

		a = new_a + x;
		b = new_b + y;

		dist2 = a*a+b*b;

		if(dist2 < mindist2) mindist2 = dist2;
		if(dist2 > 4.0)
		{
			cf = fractal_exterior_color(TPat, col, a, b);
			break;
		}
	}

	if(col == it_max)
		cf = fractal_interior_color(TPat, col, a, b, mindist2);

	return(cf);
}


/*****************************************************************************
*
* FUNCTION
*
*   marble_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL marble_pattern (const VECTOR EPoint, const TPATTERN *TPat, int noise_generator)
{
	register DBL turb_val;
	const TURB *Turb;

	if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
	{
		turb_val = Turb->Turbulence[X] * Turbulence(EPoint,Turb,noise_generator);
	}
	else
	{
		turb_val = 0.0;
	}

	return(EPoint[X] + turb_val);
}


/*****************************************************************************
*
* FUNCTION
*
*   object_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static DBL object_pattern (const VECTOR EPoint, const TPATTERN *TPat, TraceThreadData *Thread)
{
	if(TPat->Vals.Object != NULL)
	{
		if(Inside_Object(EPoint, TPat->Vals.Object, Thread))
			return 1.0;
		else
			return 0.0;
	}

	return 0.0;
}

/*****************************************************************************
*
* FUNCTION
*
*   onion_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Scott Taylor
*
* DESCRIPTION
*
* CHANGES
*
*   Jul 1991 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL onion_pattern (const VECTOR EPoint)
{
	/* The variable noise is not used as noise in this function */

	register DBL noise;

/*
	 This ramp goes 0-1,1-0,0-1,1-0...

	 noise = (fmod(sqrt(Sqr(x)+Sqr(y)+Sqr(z)),2.0)-1.0);

	 if (noise<0.0) {noise = 0.0-noise;}
*/

	/* This ramp goes 0-1, 0-1, 0-1, 0-1 ... */

	noise = (fmod(sqrt(Sqr(EPoint[X])+Sqr(EPoint[Y])+Sqr(EPoint[Z])), 1.0));

	return(noise);
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

static DBL pigment_pattern (const VECTOR EPoint, const TPATTERN *TPat, const Intersection *isect, const Ray *ray, TraceThreadData *Thread)
{
	DBL value;
	Colour Col;
	int colour_found=false;

	if (TPat->Vals.Pigment)
		// TODO ALPHA - we're discarding transparency information, so maybe we want to pre-multiply if there's alpha in there?
		colour_found = Compute_Pigment(Col, TPat->Vals.Pigment, EPoint, isect, ray, Thread);

	if(!colour_found)
		value = 0.0;
	else
		value = Col.greyscale();

	return value ;
}


/*****************************************************************************
*
* FUNCTION
*
*   planar_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   -
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL planar_pattern (const VECTOR EPoint)
{
	register DBL value = fabs(EPoint[Y]);

	CLIP_DENSITY(value);

	return value;
}


/*****************************************************************************
*
* FUNCTION
*
*   quilted_pattern
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dan Farmer & Chris Young
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static DBL quilted_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	VECTOR value;
	DBL t;

	value[X] = EPoint[X]-FLOOR(EPoint[X])-0.5;
	value[Y] = EPoint[Y]-FLOOR(EPoint[Y])-0.5;
	value[Z] = EPoint[Z]-FLOOR(EPoint[Z])-0.5;

	t = sqrt(value[X]*value[X]+value[Y]*value[Y]+value[Z]*value[Z]);

	t = quilt_cubic(t, TPat->Vals.Quilted.Control0, TPat->Vals.Quilted.Control1);

	value[X] *= t;
	value[Y] *= t;
	value[Z] *= t;

	return((fabs(value[X])+fabs(value[Y])+fabs(value[Z]))/3.0);
}


/*****************************************************************************
*
* FUNCTION
*
*   radial_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Chris Young -- new in vers 2.0
*
* DESCRIPTION
*
* CHANGES
*
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL radial_pattern (const VECTOR EPoint)
{
	register DBL value;

	if ((fabs(EPoint[X])<0.001) && (fabs(EPoint[Z])<0.001))
	{
		value = 0.25;
	}
	else
	{
		value = 0.25 + (atan2(EPoint[X],EPoint[Z]) + M_PI) / TWO_M_PI;
	}

	return(value);
}


/*****************************************************************************
*
* FUNCTION
*
*   ripples_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/

static DBL ripples_pattern (const VECTOR EPoint, const TPATTERN *TPat, const TraceThreadData *Thread)
{
	register unsigned int i;
	register DBL length, index;
	DBL scalar =0.0;
	VECTOR point;

	for (i = 0 ; i < Thread->numberOfWaves ; i++)
	{
		VSub (point, EPoint, *Thread->waveSources[i]);
		VLength (length, point);

		if (length == 0.0)
			length = 1.0;

		index = length * TPat->Frequency + TPat->Phase;

		scalar += cycloidal(index);
	}

	scalar = 0.5*(1.0+(scalar / (DBL)Thread->numberOfWaves));

	return(scalar);
}


/*****************************************************************************
*
* FUNCTION
*
*   slope_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*             is evaluated.
*   TPat   -- Texture pattern struct
*   Intersection - intersection struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0, 0.0 if normal is NULL
*
* AUTHOR
*
*   -hdf-
*
* DESCRIPTION   :
*
*   calculates the surface slope from surface normal vector
*
* CHANGES
*
*   Apr 1998 : written by H.-D. Fink
*   May 1998 : modified by M.C. Andrews - now combines slope and 'gradient'.
*
******************************************************************************/

static DBL slope_pattern (const VECTOR EPoint, const TPATTERN *TPat, const Intersection *Isection)
{
	DBL value, value1, value2;

	if (Isection == NULL) return 0.0; /* just in case ... */

	if (TPat->Vals.Slope.Point_At)
	{
		VECTOR vect;
		VSub(vect,TPat->Vals.Slope.Slope_Vector,Isection->IPoint);
		VNormalizeEq(vect);
		VDot(value1, Isection->PNormal, vect);
	}
	else
	{
		if (TPat->Vals.Slope.Slope_Base > 0)
			/* short case 1: slope vector in x, y or z direction */
			value1 = Isection->PNormal[TPat->Vals.Slope.Slope_Base - 1];
		else if (TPat->Vals.Slope.Slope_Base < 0)
			/* short case 2: slope vector in negative x, y or z direction */
			value1 = -Isection->PNormal[-TPat->Vals.Slope.Slope_Base - 1];
		else
			/* projection slope onto normal vector */
			VDot(value1, Isection->PNormal, TPat->Vals.Slope.Slope_Vector);
	}

	/* Clamp to 1.0. */
	/* should never be necessary since both vectors are normalized */
	if      (value1 >  1.0) value1 =  1.0;
	else if (value1 < -1.0) value1 = -1.0;

	value1 = asin(value1) / M_PI * 2;
	value1 = (value1 + 1.0) * 0.5;        /* normalize to [0..1] interval */

	/* If set, use offset and scalings for slope and altitude. */
	if (0.0 != TPat->Vals.Slope.Slope_Mod[V])
	{
		value1 = (value1 - TPat->Vals.Slope.Slope_Mod[U]) / TPat->Vals.Slope.Slope_Mod[V];
	}

	if (!TPat->Vals.Slope.Altit_Len)
	{
		/* Clamp to 1.0. */
		if ( value1 == 1.0 )
		{
			value1= value1- EPSILON;
		}
		else
		{
			value1 = (value1 < 0.0) ? 1.0 + fmod(value1, 1.0) : fmod(value1, 1.0);
		}
		return value1; /* no altitude defined */
	}

	/* Calculate projection of Epoint along altitude vector */
	if (TPat->Vals.Slope.Altit_Base > 0)
		/* short case 1: altitude vector in x, y or z direction */
		value2 = EPoint[TPat->Vals.Slope.Altit_Base - 1];
	else if (TPat->Vals.Slope.Altit_Base < 0)
		/* short case 2: altitude vector in negative x, y or z direction */
		value2 = -EPoint[-TPat->Vals.Slope.Altit_Base - 1];
	else
		/* projection of Epoint along altitude vector */
		VDot(value2, EPoint, TPat->Vals.Slope.Altit_Vector);

	if (0.0 != TPat->Vals.Slope.Altit_Mod[V])
	{
		value2 = (value2 - TPat->Vals.Slope.Altit_Mod[U]) / TPat->Vals.Slope.Altit_Mod[V];
	}

	value = TPat->Vals.Slope.Slope_Len * value1 + TPat->Vals.Slope.Altit_Len * value2;

	/* Clamp to 1.0. */
	if ( value - 1.0 < EPSILON && value >= 1.0 )
	{
		/* 1.0 is a very common value to get *exactly*.  We don't want to wrap
		   it to the bottom end of the map. */
		value = value - EPSILON;
	}
	else
	{
		value = (value < 0.0) ? 1.0 + fmod(value, 1.0) : fmod(value, 1.0);
	}
	return value;

}


/*****************************************************************************
*
* FUNCTION
*
*   aoi_pattern
*
* INPUT
*
*   Intersection - intersection struct
*   Ray          - Ray information
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   J. Grimbert
*
* DESCRIPTION
*
*  Return a value related to angle of incidence
*   (angle between the normal at the intersection and the ray)
*
* CHANGES
*
*   Mar 2010 : modified by [CLi]
*
******************************************************************************/

static DBL aoi_pattern (const Intersection *Isection, const Ray *ray)
{
	VECTOR    a, b;
	DBL       cosAngle, angle;

	if ((Isection == NULL) || (ray == NULL))
		return 0.0;

	VNormalize(a, Isection->PNormal);
	VNormalize(b, ray->Direction);
	VDot(cosAngle, a, b);

	// clip to [-1.0; 1.0], just to be sure
	// (should never be necessary since both vectors are normalized)
	cosAngle = clip(cosAngle, -1.0, 1.0);
	angle = acos(cosAngle) / M_PI;

	return angle;
}


/*****************************************************************************
*
* FUNCTION
*
*   spiral1_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*   Spiral whirles around z-axis.
*   The number of "arms" is defined in the TPat.
*
* CHANGES
*
*   Aug 1994 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL spiral1_pattern (const VECTOR EPoint, const TPATTERN *TPat, int noise_generator)
{
	DBL rad, phi, turb_val;
	DBL x = EPoint[X];
	DBL y = EPoint[Y];
	DBL z = EPoint[Z];
	const TURB *Turb;

	if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
	{
		turb_val = Turb->Turbulence[X] * Turbulence(EPoint,Turb,noise_generator);
	}
	else
	{
		turb_val = 0.0;
	}

	/* Get distance from z-axis. */

	rad = sqrt(x * x + y * y);

	/* Get angle in x,y-plane (0...2 PI). */

	if (rad == 0.0)
	{
		phi = 0.0;
	}
	else
	{
		if (x < 0.0)
		{
			phi = 3.0 * M_PI_2 - asin(y / rad);
		}
		else
		{
			phi = M_PI_2 + asin(y / rad);
		}
	}

	return(z + rad + (DBL)TPat->Vals.Arms * phi / TWO_M_PI + turb_val);
}


/*****************************************************************************
*
* FUNCTION
*
*   spiral2_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*   Spiral whirles around z-axis.
*   The number of "arms" is defined in the TPat.
*
* CHANGES
*
*   Aug 1994 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL spiral2_pattern (const VECTOR EPoint, const TPATTERN *TPat, int noise_generator)
{
	DBL rad, phi, turb_val;
	DBL x = EPoint[X];
	DBL y = EPoint[Y];
	DBL z = EPoint[Z];
	const TURB *Turb;

	if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
	{
		turb_val = Turb->Turbulence[X] * Turbulence(EPoint,Turb,noise_generator);
	}
	else
	{
		turb_val = 0.0;
	}

	/* Get distance from z-axis. */

	rad = sqrt(x * x + y * y);

	/* Get angle in x,y-plane (0...2 PI) */

	if (rad == 0.0)
	{
		phi = 0.0;
	}
	else
	{
		if (x < 0.0)
		{
			phi = 3.0 * M_PI_2 - asin(y / rad);
		}
		else
		{
			phi = M_PI_2 + asin(y / rad);
		}
	}

	turb_val = Triangle_Wave(z + rad + (DBL)TPat->Vals.Arms * phi / TWO_M_PI +
	                         turb_val);

	return(Triangle_Wave(rad) + turb_val);
}


/*****************************************************************************
*
* FUNCTION
*
*   spherical_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   -
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL spherical_pattern (const VECTOR EPoint)
{
	register DBL value;

	VLength(value, EPoint);
	CLIP_DENSITY(value);

	return(value);
}


/*****************************************************************************
*
* FUNCTION
*
*   waves_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/

static DBL waves_pattern (const VECTOR EPoint, const TPATTERN *TPat, const TraceThreadData *Thread)
{
	register unsigned int i;
	register DBL length, index;
	DBL scalar = 0.0;
	VECTOR point;

	for (i = 0 ; i < Thread->numberOfWaves ; i++)
	{
		VSub (point, EPoint, *Thread->waveSources[i]);
		VLength (length, point);

		if (length == 0.0)
		{
			length = 1.0;
		}

		index = length * TPat->Frequency * Thread->waveFrequencies[i] + TPat->Phase;

		scalar += cycloidal(index)/Thread->waveFrequencies[i];
	}

	scalar = 0.2*(2.5+(scalar / (DBL)Thread->numberOfWaves));

	return(scalar);
}


/*****************************************************************************
*
* FUNCTION
*
*   wood_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL wood_pattern (const VECTOR EPoint, const TPATTERN *TPat)
{
	register DBL length;
	VECTOR WoodTurbulence;
	VECTOR point;
	DBL x=EPoint[X];
	DBL y=EPoint[Y];
	const TURB *Turb;

	if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
	{
		DTurbulence (WoodTurbulence, EPoint,Turb);
		point[X] = cycloidal((x + WoodTurbulence[X]) * Turb->Turbulence[X]);
		point[Y] = cycloidal((y + WoodTurbulence[Y]) * Turb->Turbulence[Y]);
	}
	else
	{
		point[X] = 0.0;
		point[Y] = 0.0;
	}
	point[Z] = 0.0;

	point[X] += x;
	point[Y] += y;

	/* point[Z] += z; Deleted per David Buck --  BP 7/91 */

	VLength (length, point);

	return(length);
}


/*****************************************************************************
*
* FUNCTION
*
*   wrinkles_pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/

static DBL wrinkles_pattern (const VECTOR EPoint, int noise_generator)
{
	register int i;
	DBL lambda = 2.0;
	DBL omega = 0.5;
	DBL value;
	VECTOR temp;
	DBL noise;

	if(noise_generator>1)
	{
		noise = Noise(EPoint, noise_generator)*2.0-0.5;
		value = min(max(noise,0.0),1.0);
	}
	else
	{
		value = Noise(EPoint, noise_generator);
	}

	for (i = 1; i < 10; i++)
	{
		VScale(temp,EPoint,lambda);

		if(noise_generator>1)
		{
			noise = Noise(temp, noise_generator)*2.0-0.5;
			value += omega * min(max(noise,0.0),1.0);
		}
		else
		{
			value += omega * Noise(temp, noise_generator);
		}

		lambda *= 2.0;

		omega *= 0.5;
	}

	return(value/2.0);
}


/*****************************************************************************
*
* FUNCTION
*
*   IntPickInCube(tvx,tvy,tvz, p1)
*    a version of PickInCube that takes integers for input
*
* INPUT
*
*   ?
*
* OUTPUT
*   
* RETURNS
*
*   long integer hash function used, to speed up cacheing.
*   
* AUTHOR
*
*   original PickInCube by Jim McElhiney
*   this integer one modified by Nathan Kopp
*   
* DESCRIPTION
*
*   A subroutine to go with crackle.
*
*   Pick a random point in the same unit-sized cube as tv, in a
*   predictable way, so that when called again with another point in
*   the same unit cube, p1 is picked to be the same.
*
* CHANGES
*
******************************************************************************/

static int IntPickInCube(int tvx, int tvy, int tvz, VECTOR  p1)
{
	size_t seed;

	seed = size_t(Hash3d(tvx&0xFFF,tvy&0xFFF,tvz&0xFFF));

	p1[X] = tvx + PatternRands(seed);
	p1[Y] = tvy + PatternRands(seed + 1);
	p1[Z] = tvz + PatternRands(seed + 2);

	return (int)seed;
}


/*****************************************************************************
*
* FUNCTION
*
*   PickInCube(tv, p1)
*
* INPUT
*
*   ?
*
* OUTPUT
*   
* RETURNS
*
*   long integer hash function used, to speed up cacheing.
*   
* AUTHOR
*
*   Jim McElhiney
*   
* DESCRIPTION
*
*   A subroutine to go with crackle.
*
*   Pick a random point in the same unit-sized cube as tv, in a
*   predictable way, so that when called again with another point in
*   the same unit cube, p1 is picked to be the same.
*
* CHANGES
*
******************************************************************************/

int PickInCube(const VECTOR tv, VECTOR  p1)
{
	size_t seed;
	VECTOR flo;

	/*
	 * This uses floor() not FLOOR, so it will not be a mirror
	 * image about zero in the range -1.0 to 1.0. The viewer
	 * won't see an artefact around the origin.
	 */

	flo[X] = floor(tv[X] - EPSILON);
	flo[Y] = floor(tv[Y] - EPSILON);
	flo[Z] = floor(tv[Z] - EPSILON);

	seed = size_t(Hash3d((int)flo[X], (int)flo[Y], (int)flo[Z]));

	p1[X] = flo[X] + PatternRands(seed);
	p1[Y] = flo[Y] + PatternRands(seed + 1);
	p1[Z] = flo[Z] + PatternRands(seed + 2);

	return (int)seed;
}

/*****************************************************************************
*
* FUNCTION
*
*   NewHash(tvx, tvy, tvz)
*
* INPUT
*
*   3D integer coordinates of the cell
*
* OUTPUT
*   
* RETURNS
*
*   long integer hash value
*   
* AUTHOR
*
*   Christoph Hormann based on MechSim Hash function by Daniel Jungmann
*   
* DESCRIPTION
*
*   New Hash function for the crackle pattern.
*
* CHANGES
*
*   -- Aug 2005 : Creation
*
******************************************************************************/

#ifndef HAVE_BOOST_HASH
static unsigned long int NewHash(long int tvx, long int tvy, long int tvz)
{
	unsigned long int seed;
	long int r;

	tvx *= 73856093L;
	tvy *= 19349663L;
	tvz *= 83492791L;

	r = tvx ^ tvy ^ tvz;
	seed = abs(r);
	if (tvx<0) seed += LONG_MAX/2;
	if (tvy<0) seed += LONG_MAX/4;
	if (tvz<0) seed += LONG_MAX/8;

	return (seed);
}
#endif


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

const DBL INV_SQRT_3_4 = 1.154700538;
DBL quilt_cubic(DBL t, DBL p1, DBL p2)
{
	DBL it=(1-t);
	DBL itsqrd=it*it;
	/* DBL itcubed=it*itsqrd; */
	DBL tsqrd=t*t;
	DBL tcubed=t*tsqrd;
	DBL val;

	/* Originally coded as...

	val= (DBL)(itcubed*n1+(tcubed)*n2+3*t*(itsqrd)*p1+3*(tsqrd)*(it)*p2);

	re-written by CEY to optimise because n1=0 n2=1 always.

	*/

	val = (tcubed + 3.0*t*itsqrd*p1 + 3.0*tsqrd*it*p2) * INV_SQRT_3_4;

	return(val);
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

static DBL fractal_exterior_color(const TPATTERN *TPat, int iters, DBL a, DBL b)
{
	switch(TPat->Vals.Fractal.exterior_type)
	{
		case 0:
			return  (DBL)TPat->Vals.Fractal.efactor;
		case 1:
			return (DBL)iters / (DBL)TPat->Vals.Fractal.Iterations;
		case 2:
			return a * (DBL)TPat->Vals.Fractal.efactor;
		case 3:
			return b * (DBL)TPat->Vals.Fractal.efactor;
		case 4:
			return a*a * (DBL)TPat->Vals.Fractal.efactor;
		case 5:
			return b*b * (DBL)TPat->Vals.Fractal.efactor;
		case 6:
			return sqrt(a*a+b*b) * (DBL)TPat->Vals.Fractal.efactor;
		case 7: // range 0.. (n-1)/n
			return  (DBL)( iters % (unsigned int)TPat->Vals.Fractal.efactor )/(DBL)TPat->Vals.Fractal.efactor;
		case 8: // range 0.. 1
			return  (DBL)( iters % (unsigned int)(1+TPat->Vals.Fractal.efactor) )/(DBL)TPat->Vals.Fractal.efactor;
	}
	return 0;
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

static DBL fractal_interior_color(const TPATTERN *TPat, int /*iters*/, DBL a, DBL b, DBL mindist2)
{
	switch(TPat->Vals.Fractal.interior_type)
	{
		case 0:
			return  (DBL)TPat->Vals.Fractal.ifactor;
		case 1:
			return sqrt(mindist2) * (DBL)TPat->Vals.Fractal.ifactor;
		case 2:
			return a * (DBL)TPat->Vals.Fractal.ifactor;
		case 3:
			return b * (DBL)TPat->Vals.Fractal.ifactor;
		case 4:
			return a*a * (DBL)TPat->Vals.Fractal.ifactor;
		case 5:
			return b*b * (DBL)TPat->Vals.Fractal.ifactor;
		case 6:
			return a*a+b*b * (DBL)TPat->Vals.Fractal.ifactor;
	}
	return 0;
}


/*****************************************************************************
*
* FUNCTION
*
*   Create_Density_File
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a density file structure.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

DENSITY_FILE *Create_Density_File()
{
	DENSITY_FILE *New;

	New = reinterpret_cast<DENSITY_FILE *>(POV_MALLOC(sizeof(DENSITY_FILE), "density file"));

	New->Interpolation = NO_INTERPOLATION;

	New->Data = reinterpret_cast<DENSITY_FILE_DATA *>(POV_MALLOC(sizeof(DENSITY_FILE_DATA), "density file data"));

	New->Data->References = 1;

	New->Data->Name = NULL;

	New->Data->Sx =
	New->Data->Sy =
	New->Data->Sz = 0;

	New->Data->Type = 0;

	New->Data->Density32 = NULL;
	New->Data->Density16 = NULL;
	New->Data->Density8 = NULL;

	return (New);
}


/*****************************************************************************
*
* FUNCTION
*
*   Copy_Density_File
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a density file structure.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

DENSITY_FILE *Copy_Density_File(DENSITY_FILE *Old)
{
	DENSITY_FILE *New;

	if (Old != NULL)
	{
		New = reinterpret_cast<DENSITY_FILE *>(POV_MALLOC(sizeof(DENSITY_FILE), "density file"));

		*New = *Old;

		New->Data->References++;
	}
	else
	{
		New=NULL;
	}

	return(New);
}


/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Density_File
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Destroy a density file structure.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

void Destroy_Density_File(DENSITY_FILE *Density_File)
{
	if(Density_File != NULL)
	{
		if((--(Density_File->Data->References)) == 0)
		{
			POV_FREE(Density_File->Data->Name);

			if(Density_File->Data->Type == 4)
			{
				POV_FREE(Density_File->Data->Density32);
			}
			else if(Density_File->Data->Type == 2)
			{
				POV_FREE(Density_File->Data->Density16);
			}
			else if(Density_File->Data->Type == 1)
			{
				POV_FREE(Density_File->Data->Density8);
			}

			POV_FREE(Density_File->Data);
		}

		POV_FREE(Density_File);
	}
}

void Read_Density_File(IStream *file, DENSITY_FILE *df)
{
	size_t x, y, z, sx, sy, sz, len;

	if (df == NULL)
		return;

	/* Allocate and read density file. */

	if((df != NULL) && (df->Data->Name != NULL))
	{
		sx = df->Data->Sx = readushort(file);
		sy = df->Data->Sy = readushort(file);
		sz = df->Data->Sz = readushort(file);

		file->seekg(0, IOBase::seek_end);
		len = file->tellg() - 6;
		file->seekg(6);

		// figure out the data size
		if((sx * sy * sz * 4) == len)
		{
			df->Data->Type = 4;

			unsigned int *map = reinterpret_cast<unsigned int *>(POV_MALLOC(sx * sy * sz * sizeof(unsigned int), "media density file data 32 bit"));

			for (z = 0; z < sz; z++)
			{
				for (y = 0; y < sy; y++)
				{
					for (x = 0; x < sx; x++)
						map[z * sy * sx + y * sx + x] = readuint(file);
				}
			}

			df->Data->Density32 = map;
		}
		else if((sx * sy * sz * 2) == len)
		{
			df->Data->Type = 2;

			unsigned short *map = reinterpret_cast<unsigned short *>(POV_MALLOC(sx * sy * sz * sizeof(unsigned short), "media density file data 16 bit"));

			for (z = 0; z < sz; z++)
			{
				for (y = 0; y < sy; y++)
				{
					for (x = 0; x < sx; x++)
						map[z * sy * sx + y * sx + x] = readushort(file);
				}
			}

			df->Data->Density16 = map;
		}
		else if((sx * sy * sz) == len)
		{
			df->Data->Type = 1;

			unsigned char *map = reinterpret_cast<unsigned char *>(POV_MALLOC(sx * sy * sz * sizeof(unsigned char), "media density file data 8 bit"));

			for (z = 0; z < sz; z++)
			{
				for (y = 0; y < sy; y++)
					file->read(&(map[z * sy * sx + y * sx]), sizeof(unsigned char) * sx);
			}

			df->Data->Density8 = map;
		}
		else
			throw POV_EXCEPTION_STRING("Invalid density file size");

		if (file != NULL)
		{
			delete file;
		}
	}
}

static unsigned short readushort(IStream *infile)
{
	short i0 = 0, i1 = 0;

	if ((i0 = infile->Read_Byte ()) == EOF || (i1 = infile->Read_Byte ()) == EOF)
	{
		throw POV_EXCEPTION_STRING("Error reading density_file");
	}

	return (((unsigned short)i0 << 8) | (unsigned short)i1);
}

static unsigned int readuint(IStream *infile)
{
	int i0 = 0, i1 = 0, i2 = 0, i3 = 0;

	if ((i0 = infile->Read_Byte ()) == EOF || (i1 = infile->Read_Byte ()) == EOF ||
	    (i2 = infile->Read_Byte ()) == EOF || (i3 = infile->Read_Byte ()) == EOF)
	{
		throw POV_EXCEPTION_STRING("Error reading density_file");
	}

	return (((unsigned int)i0 << 24) | ((unsigned int)i1 << 16) | ((unsigned int)i2 << 8) | (unsigned int)i3);
}

static void InitializeBinomialCoefficients(void)
{
	int* ptr = BinomialCoefficients;
	*ptr = 1; ++ptr;

	for(unsigned n=1; n<=FRACTAL_MAX_EXPONENT; ++n)
	{
		*ptr = 1; ++ptr;
		for(unsigned k=1; k<n; ++k)
		{
			*ptr = *(ptr-(n+1)) + *(ptr-n); ++ptr;
		}
		*ptr = 1; ++ptr;
	}
	ptr = BinomialCoefficients+1;
	for(unsigned m=1; m<=FRACTAL_MAX_EXPONENT; ++m)
	{
		++ptr;
		for(unsigned k=1; k<m; ++k)
		{
			if((k&2)!=0) *ptr = -(*ptr);
			++ptr;
		}
		if((m&2)!=0) *ptr = -(*ptr);
		++ptr;
	}
}

static void InitialiseCrackleCubeTable(void)
{
	int     *p = CrackleCubeTable;

	// the crackle cube table is a list of offsets in the range -2 ... 2 which
	// are applied to the EPoint while evaluating the Crackle pattern, in order
	// to look up points in close-by cubes. consider the EPoint to be in a cube
	// at the center of a 3x3 grid of cubes; candidate cubes are that are within
	// a "3d knight move" away (i.e. not more than 2 units). we use a lookup table
	// to speed up iteration of the cube list by avoiding branch tests.
	for(int addx = -2; addx <= 2; addx++)
	{
		for(int addy = -2; addy <= 2; addy++)
		{
			for(int addz = -2; addz <= 2; addz++)
			{
				if((abs(addx) == 2) + (abs(addy) == 2) + (abs(addz) == 2) <= 1)
				{
					*p++ = addx;
					*p++ = addy;
					*p++ = addz;
				}
			}
		}
	}
}

// This should be called once, at povray start
void InitializePatternGenerators(void)
{
	InitializeBinomialCoefficients();
	InitialiseCrackleCubeTable();
}

}
