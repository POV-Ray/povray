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
 * $File: //depot/povray/smp/source/backend/pattern/pattern.h $
 * $Revision: #31 $
 * $Change: 6158 $
 * $DateTime: 2013/12/02 21:19:56 $
 * $Author: clipka $
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
#define LAST_NORM_ONLY_PATTERN   GENERIC_NORM_ONLY_PATTERN
#define LAST_INTEGER_PATTERN     GENERIC_INTEGER_PATTERN

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

	GENERIC_NORM_ONLY_PATTERN,  ///< Pattern does not need any legacy special handling anywhere, except for its property of having a special implementation for normals.

/* These patterns return integer values.  They must be kept
   together in the list.  Any new integer functions added must be added here. */
	OBJECT_PATTERN, // NOT in all cases as the others in this group
	BRICK_PATTERN,  // NOT in all cases as the others in this group

	GENERIC_INTEGER_PATTERN,    ///< Pattern does not need any legacy special handling anywhere, except for its property of returning an integer value.

/* These patterns return float values.  They must be kept together
   and seperate from those above. */
	MARBLE_PATTERN,
	WOOD_PATTERN,
	AGATE_PATTERN,
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
	CRACKLE_PATTERN,
	DENSITY_FILE_PATTERN,
	IMAGE_PATTERN,
	PAVEMENT_PATTERN,
	TILING_PATTERN,

	GENERIC_PATTERN     ///< Pattern does not need any legacy special handling anywhere
};

/* flags for patterned stuff */

#define NO_FLAGS              0
#define HAS_FILTER            1
// value 2 is currently not used
#define POST_DONE             4
#define DONT_SCALE_BUMPS_FLAG 8 /* scale bumps for normals */

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


struct BasicPattern
{
	unsigned short Wave_Type;
	unsigned char noiseGenerator; ///< 0=default, 1=orig, 2=range, 3=perlin
	SNGL Frequency, Phase;
	SNGL Exponent;
	WARP *Warps;

	BasicPattern();
	BasicPattern(const BasicPattern& obj);
	virtual ~BasicPattern();
	int GetNoiseGen(const TraceThreadData *Thread) const;
	virtual PatternPtr Clone() const = 0;
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const = 0;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;

	template<typename T>
	static PatternPtr Clone(const T& obj) { return PatternPtr(new T(obj)); }
};

struct DiscretePattern : public BasicPattern
{
	virtual unsigned int NumBlendMapEntries() const = 0;
};


struct PlainPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};


struct AgatePattern : public BasicPattern
{
	SNGL Agate_Turb_Scale;

	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
};

struct AOIPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct BoxedPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct BrickPattern : public DiscretePattern
{
	SNGL Mortar;
	Vector3d Size;

	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
	virtual unsigned int NumBlendMapEntries() const;
};

struct CellsPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct CheckerPattern : public DiscretePattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
	virtual unsigned int NumBlendMapEntries() const;
};

struct CracklePattern : public BasicPattern
{
	Vector3d Form;
	DBL Metric;
	DBL Offset;
	DBL Dim;
	short IsSolid;

	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct CubicPattern : public DiscretePattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
	virtual unsigned int NumBlendMapEntries() const;
};

struct CylindricalPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct DentsPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct DensityFilePattern : public BasicPattern
{
	struct Density_file_Data_Struct
	{
		int References;
		char *Name;
		size_t Sx, Sy, Sz;
		int Type;
		union
		{
			unsigned char *Density8;
			unsigned short *Density16;
			unsigned int *Density32;
		};
	};
	struct Density_file_Struct
	{
		int Interpolation;
		Density_file_Data_Struct *Data;
	};

	Density_file_Struct *Density_File;

	DensityFilePattern();
	DensityFilePattern(const DensityFilePattern& obj);
	virtual ~DensityFilePattern();
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};
typedef struct DensityFilePattern::Density_file_Struct DENSITY_FILE;
typedef struct DensityFilePattern::Density_file_Data_Struct DENSITY_FILE_DATA;

struct FacetsPattern : public BasicPattern
{
	DBL Size, UseCoords, Metric;

	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct FractalPattern : public BasicPattern
{
	unsigned int Iterations;
	DBL efactor, ifactor; 
	unsigned char interior_type, exterior_type;

	virtual PatternPtr Clone() const = 0;
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const = 0;
	DBL ExteriorColour(int iters, DBL a, DBL b) const;
	DBL InteriorColour(DBL a, DBL b, DBL mindist2) const;
};

class FunctionVM;
struct FunctionPattern : public BasicPattern
{
	void *Fn;
	unsigned int Data;
	FunctionVM *vm;

	FunctionPattern();
	FunctionPattern(const FunctionPattern& obj);
	virtual ~FunctionPattern();
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct GradientPattern : public BasicPattern
{
	Vector3d Gradient;

	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct GranitePattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct HexagonPattern : public DiscretePattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
	virtual unsigned int NumBlendMapEntries() const;
};

class ImageData;
struct ImagePattern : public BasicPattern
{
	ImageData *image;

	ImagePattern();
	ImagePattern(const ImagePattern& obj);
	virtual ~ImagePattern();
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct LeopardPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct MarblePattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
};

struct NoisePattern : public BasicPattern
{
	virtual PatternPtr Clone() const = 0;
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct ObjectPattern : public DiscretePattern
{
	ObjectBase *Object;

	ObjectPattern();
	ObjectPattern(const ObjectPattern& obj);
	virtual ~ObjectPattern();
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
	virtual unsigned int NumBlendMapEntries() const;
};

struct OnionPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

// TODO - we should probably make this one class per pavement type
struct PavementPattern : public BasicPattern
{
	unsigned char Side, Tile, Number, Exterior, Interior, Form;

	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	DBL hexagonal (const Vector3d& EPoint) const;
	DBL trigonal (const Vector3d& EPoint) const;
	DBL tetragonal (const Vector3d& EPoint) const;
};

struct PigmentPattern : public BasicPattern
{
	PIGMENT *Pigment;

	PigmentPattern();
	PigmentPattern(const PigmentPattern& obj);
	virtual ~PigmentPattern();
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct PlanarPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct QuiltedPattern : public BasicPattern
{
	SNGL Control0, Control1;

	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct RadialPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
};

struct RipplesPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct SlopePattern : public BasicPattern
{
	Vector3d Slope_Vector, Altit_Vector;
	short Slope_Base, Altit_Base;
	DBL Slope_Len, Altit_Len;
	UV_VECT Slope_Mod, Altit_Mod;
	bool Point_At;

	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct SphericalPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct SpiralPattern : public BasicPattern
{
	short Arms;

	virtual PatternPtr Clone() const = 0;
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const = 0;
};

struct SquarePattern : public DiscretePattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
	virtual unsigned int NumBlendMapEntries() const;
};

// TODO - we should probably make this one class per tiling type
struct TilingPattern : public BasicPattern
{
	unsigned char Pattern;

	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct TriangularPattern : public DiscretePattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
	virtual unsigned int NumBlendMapEntries() const;
};

struct WavesPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct WoodPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
};

struct WrinklesPattern : public BasicPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};


struct JuliaPattern : public FractalPattern
{
	Vector2d Coord;

	JuliaPattern();
	JuliaPattern(const JuliaPattern& obj);
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct Julia3Pattern : public JuliaPattern
{
	Julia3Pattern();
	Julia3Pattern(const JuliaPattern& obj);
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct Julia4Pattern : public JuliaPattern
{
	Julia4Pattern();
	Julia4Pattern(const JuliaPattern& obj);
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct JuliaXPattern : public JuliaPattern
{
	int Exponent;

	JuliaXPattern();
	JuliaXPattern(const JuliaPattern& obj);
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};


struct MandelPattern : public FractalPattern
{
};

struct Mandel2Pattern : public MandelPattern
{
	Mandel2Pattern();
	Mandel2Pattern(const MandelPattern& obj);
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
};

struct Mandel3Pattern : public MandelPattern
{
	Mandel3Pattern();
	Mandel3Pattern(const MandelPattern& obj);
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct Mandel4Pattern : public MandelPattern
{
	Mandel4Pattern();
	Mandel4Pattern(const MandelPattern& obj);
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct MandelXPattern : public MandelPattern
{
	int Exponent;

	MandelXPattern();
	MandelXPattern(const MandelPattern& obj);
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};


struct Magnet1MPattern : public MandelPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct Magnet1JPattern : public JuliaPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct Magnet2MPattern : public MandelPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct Magnet2JPattern : public JuliaPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};


struct BozoPattern : public NoisePattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual const BLEND_MAP* GetDefaultBlendMap() const;
};

struct BumpsPattern : public NoisePattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
};

struct SpottedPattern : public NoisePattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
};


struct Spiral1Pattern : public SpiralPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};

struct Spiral2Pattern : public SpiralPattern
{
	virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
	virtual DBL operator()(const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread) const;
};


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
	Vector3d data[81];
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

DBL Evaluate_TPat (const TPATTERN *TPat, const Vector3d& EPoint, const Intersection *Isection, const Ray *ray, TraceThreadData *Thread);
void Init_TPat_Fields (TPATTERN *Tpat);
void Copy_TPat_Fields (TPATTERN *New, const TPATTERN *Old);
void Destroy_TPat_Fields (TPATTERN *Tpat);
void Translate_Tpattern (TPATTERN *Tpattern, const Vector3d& Vector);
void Rotate_Tpattern (TPATTERN *Tpattern, const Vector3d& Vector);
void Scale_Tpattern (TPATTERN *Tpattern, const Vector3d& Vector);
void Transform_Tpattern (TPATTERN *Tpattern, const TRANSFORM *Trans);
DBL quilt_cubic (DBL t,DBL p1,DBL p2);
void Search_Blend_Map (DBL value, const BLEND_MAP *Blend_Map,
                       const BLEND_MAP_ENTRY **Prev, const BLEND_MAP_ENTRY **Cur);
int GetNoiseGen (const TPATTERN *TPat, const TraceThreadData *Thread);

DENSITY_FILE *Create_Density_File ();
DENSITY_FILE *Copy_Density_File (DENSITY_FILE *);
void Destroy_Density_File (DENSITY_FILE *);
void Read_Density_File (IStream *dfile, DENSITY_FILE *df);
int PickInCube (const Vector3d& tv, Vector3d& p1);

void InitializePatternGenerators(void);

}

#endif
