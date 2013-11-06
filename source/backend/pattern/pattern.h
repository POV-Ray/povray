/*******************************************************************************
 * pattern.h
 *
 * This module contains all defines, typedefs, and prototypes for PATTERN.CPP.
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
 * $File: //depot/public/povray/3.x/source/backend/pattern/pattern.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

/* NOTE: FRAME.H contains other pattern stuff. */

#ifndef PATTERN_H
#define PATTERN_H

#include <boost/functional/hash/hash.hpp>

#include "backend/frame.h"
#include "base/fileinputoutput.h"

#ifdef _MSC_VER
	#pragma warning (disable: 4396)
	#include <boost/unordered_map.hpp>
	#pragma warning (default: 4396)
#else
	#include <boost/unordered_map.hpp>
#endif

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define LAST_SPECIAL_PATTERN     BITMAP_PATTERN
#define LAST_NORM_ONLY_PATTERN   DENTS_PATTERN
#define LAST_INTEGER_PATTERN     HEXAGON_PATTERN

enum PATTERN_IDS
{
	NO_PATTERN = 0,
	PLAIN_PATTERN,
	AVERAGE_PATTERN,
	UV_MAP_PATTERN,
	BITMAP_PATTERN,

/* These former normal patterns require special handling.  They too
   must be kep seperate for now.*/
	WAVES_PATTERN,
	RIPPLES_PATTERN,
	WRINKLES_PATTERN,
	BUMPS_PATTERN,
	QUILTED_PATTERN,
	FACETS_PATTERN,
	DENTS_PATTERN,

/* These patterns return integer values.  They must be kept
   together in the list.  Any new integer functions added must be added
   here and the list renumbered. */
	CHECKER_PATTERN,
	OBJECT_PATTERN,
	BRICK_PATTERN,
	CUBIC_PATTERN, // JN2007: Cubic pattern
	SQUARE_PATTERN,
	TRIANGULAR_PATTERN,
	HEXAGON_PATTERN,

/* These patterns return float values.  They must be kept together
   and seperate from those above. */
	BOZO_PATTERN,
	CELLS_PATTERN,
	MARBLE_PATTERN,
	WOOD_PATTERN,
	SPOTTED_PATTERN,
	AGATE_PATTERN,
	GRANITE_PATTERN,
	GRADIENT_PATTERN,
	ONION_PATTERN,
	LEOPARD_PATTERN,
	JULIA_PATTERN,
	JULIA3_PATTERN,
	JULIA4_PATTERN,
	JULIAX_PATTERN,
	MANDEL_PATTERN,
	MANDEL3_PATTERN,
	MANDEL4_PATTERN,
	MANDELX_PATTERN,
	MAGNET1M_PATTERN,
	MAGNET1J_PATTERN,
	MAGNET2M_PATTERN,
	MAGNET2J_PATTERN,
	RADIAL_PATTERN,
	CRACKLE_PATTERN,
	SPIRAL1_PATTERN,
	SPIRAL2_PATTERN,
	PLANAR_PATTERN,
	SPHERICAL_PATTERN,
	BOXED_PATTERN,
	CYLINDRICAL_PATTERN,
	DENSITY_FILE_PATTERN,
	FUNCTION_PATTERN,
	SLOPE_PATTERN,
	AOI_PATTERN,
	PIGMENT_PATTERN,
	IMAGE_PATTERN,
	PAVEMENT_PATTERN,
	TILING_PATTERN
};

/* Pattern flags */

#define NO_FLAGS              0
#define HAS_FILTER            1
#define FULL_BLOCKING         2
#define POST_DONE             4
#define DONT_SCALE_BUMPS_FLAG 8 /* scale bumps for normals */
#define NOISE_FLAG_1         16 /* this flag and the next one work together */
#define NOISE_FLAG_2         32 /* neither=default, 1=orig,2=range,3=perlin */
#define NOISE_FLAGS         NOISE_FLAG_1+NOISE_FLAG_2

#define Destroy_Turb(t) if ((t)!=NULL) POV_FREE(t);

#define RAMP_WAVE     0
#define SINE_WAVE     1
#define TRIANGLE_WAVE 2
#define SCALLOP_WAVE  3
#define CUBIC_WAVE    4
#define POLY_WAVE     5

/* Interpolation types. */

enum
{
	NO_INTERPOLATION        = 0,
	TRILINEAR_INTERPOLATION = 1,
	TRICUBIC_INTERPOLATION  = 2
};

/*****************************************************************************
* Global typedefs
******************************************************************************/

// see boost functional/hash example
class Crackle_Cell_Coord
{
	int x;
	int y;
	int z;
public:
	Crackle_Cell_Coord() : x(0), y(0), z(0) {}
	Crackle_Cell_Coord(int x, int y, int z) : x(x), y(y), z(z) {}

	bool operator==(Crackle_Cell_Coord const& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	friend std::size_t hash_value(Crackle_Cell_Coord const& p)
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, p.x);
		boost::hash_combine(seed, p.y);
		boost::hash_combine(seed, p.z);

		return seed;
	}
};

struct Crackle_Cache_Entry
{
	size_t last_used;
	VECTOR data[81];
};

typedef boost::unordered_map<Crackle_Cell_Coord, Crackle_Cache_Entry, boost::hash<Crackle_Cell_Coord> > Crackle_Cache_Type;

/*****************************************************************************
* Global variables
******************************************************************************/


/*****************************************************************************
* Global constants
******************************************************************************/


/*****************************************************************************
* Global functions
******************************************************************************/

DBL Evaluate_TPat (const TPATTERN *TPat, const VECTOR EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread);
void Init_TPat_Fields (TPATTERN *Tpat);
void Copy_TPat_Fields (TPATTERN *New, const TPATTERN *Old);
void Destroy_TPat_Fields (TPATTERN *Tpat);
void Translate_Tpattern (TPATTERN *Tpattern, const VECTOR Vector);
void Rotate_Tpattern (TPATTERN *Tpattern, const VECTOR Vector);
void Scale_Tpattern (TPATTERN *Tpattern, const VECTOR Vector);
void Transform_Tpattern (TPATTERN *Tpattern, const TRANSFORM *Trans);
DBL quilt_cubic (DBL t,DBL p1,DBL p2);
void Search_Blend_Map (DBL value, const BLEND_MAP *Blend_Map,
                       const BLEND_MAP_ENTRY **Prev, const BLEND_MAP_ENTRY **Cur);
int GetNoiseGen (const TPATTERN *TPat, const TraceThreadData *Thread);

DENSITY_FILE *Create_Density_File ();
DENSITY_FILE *Copy_Density_File (DENSITY_FILE *);
void Destroy_Density_File (DENSITY_FILE *);
void Read_Density_File (IStream *dfile, DENSITY_FILE *df);
int PickInCube (const VECTOR tv, VECTOR p1);

void InitializePatternGenerators(void);

}

#endif
