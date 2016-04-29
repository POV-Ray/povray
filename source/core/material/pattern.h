//******************************************************************************
///
/// @file core/material/pattern.h
///
/// Declarations related to patterns.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#ifndef POVRAY_CORE_PATTERN_H
#define POVRAY_CORE_PATTERN_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include <boost/functional/hash/hash.hpp> // required for crackle
#include <boost/unordered_map.hpp>

#include "base/fileinputoutput.h"

#include "core/coretypes.h"
#include "core/material/pigment.h"
#include "core/material/warp.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define LAST_SPECIAL_PATTERN     BITMAP_PATTERN
#define LAST_NORM_ONLY_PATTERN   GENERIC_NORM_ONLY_PATTERN
#define LAST_INTEGER_PATTERN     GENERIC_INTEGER_PATTERN

/// Legacy Identifier IDs for the various patterns.
///
/// @todo   Making pattern a polymorphic type has allowed us to get rid of quite a lot of these by now, but there are
///         still some special case handlings out there in the code that require the remaining ones.
///
enum PATTERN_IDS
{
    NO_PATTERN = 0,
    PLAIN_PATTERN,
    AVERAGE_PATTERN,
    UV_MAP_PATTERN,
    BITMAP_PATTERN,

    // The following former normal patterns require special handling.  They must be kept seperate for now.

    WAVES_PATTERN,
    RIPPLES_PATTERN,
    WRINKLES_PATTERN,
    BUMPS_PATTERN,
    QUILTED_PATTERN,
    FACETS_PATTERN,
    DENTS_PATTERN,

    GENERIC_NORM_ONLY_PATTERN,  ///< Pattern does not need any legacy special handling anywhere, except for its property of having a special implementation for normals.

    // The following patterns return integer values.  They must be kept together in the list.  Any new integer functions added must be added here.

    OBJECT_PATTERN, // NOT in all cases as the others in this group
    BRICK_PATTERN,  // NOT in all cases as the others in this group

    GENERIC_INTEGER_PATTERN,    ///< Pattern does not need any legacy special handling anywhere, except for its property of returning an integer value.

    // The following patterns return float values.  They must be kept together and seperate from those above.

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

enum WaveType
{
    kWaveType_Ramp,     ///< Ramps up from 0 to 1, then drops sharply back to 0, and repeats.
    kWaveType_Sine,     ///< Oscillates between 0 and 1 using a sine-based formula.
    kWaveType_Triangle, ///< Ramps up from 0 to 1, then ramps down from 1 to 0, and repeats.
    kWaveType_Scallop,
    kWaveType_Cubic,
    kWaveType_Poly
};

enum NoiseGenType
{
    kNoiseGen_Default        = 0, ///< Indicates that the scene's global settings noise generator should be used.
    kNoiseGen_Original       = 1, ///< POV-Ray original noise generator (pre-v3.5).
    kNoiseGen_RangeCorrected = 2, ///< POV-Ray original noise generator with range correction (v3.5 and later).
    kNoiseGen_Perlin         = 3  ///< Perlin noise generator.
};
const int kNoiseGen_Min = 1;
const int kNoiseGen_Max = 3;

/// Density file interpolation types
enum DensityFileInterpolationType
{
    kDensityFileInterpolation_None      = 0,
    kDensityFileInterpolation_Trilinear = 1,
    kDensityFileInterpolation_Tricubic  = 2
};

/// Maximum `exponent` parameter value for fractal patterns.
const int kFractalMaxExponent = 33;


//******************************************************************************
// Forward declarations to avoid pulling in entire header files.

// required by ImagePattern (defined in support/imageutil.h)
class ImageData;

//******************************************************************************

/// Generic abstract class providing the interface and commonly-used data fields for all pattern implementations.
///
/// @note   This class is currently implemented as a struct, i.e. with publicly accessible members; this was done to
///         avoid excessive changes to the parser and render engine during the refactoring process, and was deemed
///         acceptable because:
///           - These data fields do not represent any state information, but rather simple configuration parameters.
///           - None of the current implementations stores any interim results computed from these parameters.
///           .
///         Changing these parameters directly at run-time is therefore a safe operation.
///
struct BasicPattern
{
    /// The noise generator to use for the pattern (if any).
    /// One of @ref NoiseGenType.
    unsigned char noiseGenerator;

    /// List of warps applied to the pattern.
    WarpList warps;

    /// Default constructor.
    BasicPattern();

    /// Copy constructor.
    ///
    /// @param[in]  obj     The pattern to copy from.
    ///
    BasicPattern(const BasicPattern& obj);

    /// Destructor.
    virtual ~BasicPattern();

    /// Determines the effective noise generator.
    ///
    /// @param[in]  pThread the thread-local data.
    /// @return             The value of `noiseGenerator` if not kNoiseGen_Default, or the value of the globally set
    ///                     noise generator as per the thread-local data.
    ///
    int GetNoiseGen(const TraceThreadData *pThread) const;

    /// Generates an independent copy of this pattern.
    ///
    /// This method effectively serves as a virtual copy constructor. Every derived class must override it with the
    /// following implementation:
    ///
    ///     virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    ///
    /// @attention  Contrary to intuition, this method must be overridden in _every_ derived class, even if the
    ///             immediate base class already provides an implementation. As the method called is a template
    ///             parameterized by the compile-time type of its parameter, it is actually different for each class
    ///             it is placed in. It boils down to the following code (please don't use it though, as it is not
    ///             copy-and-paste safe):
    ///
    ///                 virtual PatternPtr Clone() const { return PatternPtr(new FooPattern(obj)); }
    ///
    /// @return     A new object of identical type and with same properties as this one.
    ///
    virtual PatternPtr Clone() const = 0;

    /// Evaluates the pattern at a given point in space.
    ///
    /// This method implements the actual pattern computation code, and for obvious reasons any derived class must
    /// provide its own implementation.
    ///
    /// For contiguous patterns (`NumDiscreteBlendMapEntries()` = 0), the values returned will typically be in the
    /// range [0..1]. Otherwise, the values returned will be integer numbers in the range
    /// [0..`NumDiscreteBlendMapEntries()`-1].
    ///
    /// @param[in]      EPoint      The point of interest in 3D space.
    /// @param[in]      pIsection   Additional information about the intersection. Evaluated by some patterns.
    /// @param[in]      pRay        Additional information about the ray. Evaluated by some patterns.
    /// @param[in,out]  pThread     Additional thread-local data. Evaluated by some patterns. Some patterns, such as the
    ///                             crackle pattern, store cached data here.
    /// @return                     The pattern's value at the given point in space.
    ///
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const = 0;

    /// Gets the default blend map associated with this pattern.
    ///
    /// While blend maps are generally outside the scope of this class, for legacy reasons some patterns (such as
    /// `bozo`) have different default blend maps associated with them. To avoid having to determine the pattern
    /// type at run-time in the parser, we instead provide this method for determining the default blend map
    /// associated with this particular pattern.
    ///
    /// @note       Patterns defaulting to the standard greyscale blend map do not need to override this method
    ///             (unless they derive from a sub-class that does).
    ///
    /// @return     The pattern's default blend map.
    ///
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;

    /// Number of discrete colours, textures, or whatever, from which the pattern will select.
    ///
    /// This function will be called by future parsers to determine the possible discrete values returned by this
    /// pattern, or whether the pattern is contiguous.
    ///
    /// @todo       The values returned by this method are currently still hard-coded in the parser. This method is
    ///             intended to get rid of those hard-coded values.
    ///
    /// @return     Number of discrete values to which the pattern may evaluate, or zero if the pattern will
    ///             evaluate to a value from the contiguous interval [0...1].
    ///
    virtual unsigned int NumDiscreteBlendMapEntries() const = 0;

    /// Whether the pattern has its own special turbulence handling.
    ///
    /// @return     `true` if the pattern has its own special turbulence handling.
    ///
    virtual bool HasSpecialTurbulenceHandling() const;

protected:

    /// Helper method generating an independent copy of the specified object.
    ///
    /// This static method is provided for convenience, to allow for a copy-and-paste safe implementation of the
    /// @ref Clone() method in derived classes.
    ///
    /// @note       The specified object's _actual_ type at run-time must be the same as its _nominal_ type at
    ///             compile-time.
    ///
    /// @tparam     T       The type of the object to clone.
    /// @param[in]  obj     The object to clone.
    /// @return             A copy of the specified object.
    ///
    template<typename T>
    static PatternPtr Clone(const T& obj) { return PatternPtr(new T(obj)); }
};

/// Generic abstract class providing additions to the basic pattern interface, as well as common code, for all
/// continuous pattern implementations.
///
/// @todo   We could move the `waveSomething` members into a dedicated class, possibly using polymorphism to avoid
///         switching based on the `waveType`. This could also eliminate the `waveExponent` field for cases other than
///         `kWaveType_Poly`.
///
struct ContinuousPattern : public BasicPattern
{
    /// The algorithm used for mapping the pattern's underlying function to the [0..1] interval.
    /// One of @ref WaveType.
    unsigned char waveType;

    /// The frequency of the pattern.
    SNGL waveFrequency;

    /// The phase of the pattern.
    SNGL wavePhase;

    /// The exponent to apply for a `waveType` value of `kWaveType_Poly`.
    SNGL waveExponent;

    ContinuousPattern();
    ContinuousPattern(const ContinuousPattern& obj);

    /// Evaluates the pattern at a given point in space, taking into account the wave function.
    ///
    /// @note   Derived classes should _not_ override this, but @ref EvaluateRaw() instead.
    ///
    /// @param[in]      EPoint      The point of interest in 3D space.
    /// @param[in]      pIsection   Additional information about the intersection. Evaluated by some patterns.
    /// @param[in]      pRay        Additional information about the ray. Evaluated by some patterns.
    /// @param[in,out]  pThread     Additional thread-local data. Evaluated by some patterns. Some patterns, such as the
    ///                             crackle pattern, store cached data here.
    /// @return                     The pattern's value at the given point in space.
    ///
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;

    /// Evaluates the pattern at a given point in space, without taking into account the wave function.
    ///
    /// This method implements the actual pattern computation code, and for obvious reasons any derived class must
    /// provide its own implementation.
    ///
    /// @param[in]      EPoint      The point of interest in 3D space.
    /// @param[in]      pIsection   Additional information about the intersection. Evaluated by some patterns.
    /// @param[in]      pRay        Additional information about the ray. Evaluated by some patterns.
    /// @param[in,out]  pThread     Additional thread-local data. Evaluated by some patterns. Some patterns, such as the
    ///                             crackle pattern, store cached data here.
    /// @return                     The pattern's value at the given point in space.
    ///
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const = 0;

    virtual unsigned int NumDiscreteBlendMapEntries() const;
};

/// Generic abstract class providing additions to the basic pattern interface, as well as common code, for all
/// discrete pattern implementations.
struct DiscretePattern : public BasicPattern
{
};

/// Implements a plain pattern with all-zero values for any point in space.
struct PlainPattern : public DiscretePattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual unsigned int NumDiscreteBlendMapEntries() const;
    virtual bool HasSpecialTurbulenceHandling() const;
};

/// Implements the `agate` pattern.
struct AgatePattern : public ContinuousPattern
{
    /// `agate_turb` parameter.
    SNGL agateTurbScale;

    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
};

/// Implements the `aoi` pattern.
struct AOIPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `boxed` pattern.
struct BoxedPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `brick` pattern.
struct BrickPattern : public DiscretePattern
{
    /// `mortar` parameter.
    SNGL mortar;

    /// `brick_size` parameter.
    Vector3d brickSize;

    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
    virtual unsigned int NumDiscreteBlendMapEntries() const;
};

/// Implements the `cells` pattern.
struct CellsPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `checker` pattern.
struct CheckerPattern : public DiscretePattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
    virtual unsigned int NumDiscreteBlendMapEntries() const;
};

/// Implements the `crackle` pattern.
struct CracklePattern : public ContinuousPattern
{
    Vector3d crackleForm;
    DBL crackleMetric;
    DBL crackleOffset;
    short crackleIsSolid;
    IntVector3d repeat;

    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `cubic` pattern.
struct CubicPattern : public DiscretePattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
    virtual unsigned int NumDiscreteBlendMapEntries() const;
};

/// Implements the `cylindrical` pattern.
struct CylindricalPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `density_file` pattern.
///
/// @todo   The additional member variables should be encapsulated.
///
struct DensityFilePattern : public ContinuousPattern
{
    /// @todo fix the members to match naming conventions
    struct DensityFileDataStruct
    {
        int References;
        char *Name;
        size_t Sx, Sy, Sz;
        int Type; ///< Type of data. Currently one of 1 (8 bit per voxel), 2 (16 bit per voxel) or 4 (32 bit per voxel).
        union
        {
            POV_UINT8 *Density8;
            POV_UINT16 *Density16;
            POV_UINT32 *Density32;
        };
    };
    /// @todo fix the members to match naming conventions
    struct DensityFileStruct
    {
        int Interpolation; ///< one of @ref DensityFileInterpolationType
        DensityFileDataStruct *Data;
    };

    DensityFileStruct *densityFile;

    DensityFilePattern();
    DensityFilePattern(const DensityFilePattern& obj);
    virtual ~DensityFilePattern();
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};
typedef struct DensityFilePattern::DensityFileStruct     DENSITY_FILE;      ///< @deprecated @ref DensityFilePattern::DensityFileStruct should be used instead.
typedef struct DensityFilePattern::DensityFileDataStruct DENSITY_FILE_DATA; ///< @deprecated @ref DensityFilePattern::DensityFileDataStruct should be used instead.

/// Implements the `dents` pattern.
struct DentsPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `facets` pattern.
struct FacetsPattern : public ContinuousPattern
{
    DBL facetsSize, facetsCoords, facetsMetric;

    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }

    /// @attention  As the `facets` pattern is only available for normals, this function is not supposed to be ever
    ///             called, and will throw an exception.
    ///
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Abstract class providing additions to the basic pattern interface, as well as common code, for all fractal patterns.
struct FractalPattern : public ContinuousPattern
{
    /// Maximum number of iterations.
    unsigned int maxIterations;

    /// A parameter to the algorithm for colouring the exterior of the fractal.
    DBL exteriorFactor;

    /// A parameter to the algorithm for colouring the interior of the fractal.
    DBL interiorFactor;

    /// Determines the algorithm to colour the exterior of the fractal.
    unsigned char exteriorType;

    /// Determines the algorithm to colour the interior of the fractal.
    unsigned char interiorType;

    virtual PatternPtr Clone() const = 0;
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const = 0;

protected:

    /// Computes the exterior shade depending on the results of the fractal computation.
    ///
    /// @param  iters       The number of iterations after which bailout occurred.
    /// @param  a           Final iteration "a" value.
    /// @param  b           Final iteration "b" value.
    /// @return             The exterior shade.
    ///
    DBL ExteriorColour(int iters, DBL a, DBL b) const;

    /// Computes the interior shade depending on the results of the fractal computation.
    ///
    /// @param  a           Final iteration "a" value.
    /// @param  b           Final iteration "b" value.
    /// @param  mindist2    Square of the smallest distance to the origin thoughout all iterations.
    /// @return             The interior shade.
    ///
    DBL InteriorColour(DBL a, DBL b, DBL mindist2) const;
};

/// Implements the `function` pattern.
///
/// @todo   The additional member variables should be encapsulated.
///
struct FunctionPattern : public ContinuousPattern
{
    GenericScalarFunctionPtr pFn;

    FunctionPattern();
    FunctionPattern(const FunctionPattern& obj);
    virtual ~FunctionPattern();
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `gradient` pattern.
struct GradientPattern : public ContinuousPattern
{
    /// Direction of the gradient.
    Vector3d gradient;

    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `granite` pattern.
struct GranitePattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `hexagon` pattern.
struct HexagonPattern : public DiscretePattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
    virtual unsigned int NumDiscreteBlendMapEntries() const;
};

/// Implements the `image_map` pattern.
///
/// @todo   The additional member variables should possibly be encapsulated.
///
struct ImagePattern : public ContinuousPattern
{
    ImageData *pImage;

    ImagePattern();
    ImagePattern(const ImagePattern& obj);
    virtual ~ImagePattern();
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `leopard` pattern.
struct LeopardPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `marble` pattern.
struct MarblePattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
    virtual bool HasSpecialTurbulenceHandling() const;
};

/// Base class for the noise-based patterns.
struct NoisePattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const = 0;
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `object` pattern.
///
/// @todo   The additional member variables should possibly be encapsulated.
///
struct ObjectPattern : public DiscretePattern
{
    ObjectPtr pObject;

    ObjectPattern();
    ObjectPattern(const ObjectPattern& obj);
    virtual ~ObjectPattern();
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
    virtual unsigned int NumDiscreteBlendMapEntries() const;
};

/// Implements the `onion` pattern.
struct OnionPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `pavement` pattern.
///
/// @todo   We should probably implement this as one class per pavement type, possibly all declared in @ref pattern.cpp
///         and instantiated via a static factory method in this class.
///
struct PavementPattern : public ContinuousPattern
{
    unsigned char Side;
    unsigned char Tile;
    unsigned char Number;
    unsigned char Exterior;
    unsigned char Interior;
    unsigned char Form;

    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    DBL hexagonal (const Vector3d& EPoint) const;
    DBL trigonal (const Vector3d& EPoint) const;
    DBL tetragonal (const Vector3d& EPoint) const;
};

/// Implements the `pigment_pattern` pattern.
///
/// @todo   The additional member variables should possibly be encapsulated.
///
struct PigmentPattern : public ContinuousPattern
{
    PIGMENT *pPigment;

    PigmentPattern();
    PigmentPattern(const PigmentPattern& obj);
    virtual ~PigmentPattern();
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `planar` pattern.
struct PlanarPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `quilted` pattern.
struct QuiltedPattern : public ContinuousPattern
{
    SNGL Control0, Control1;

    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `radial` pattern.
struct RadialPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
};

/// Implements the `ripples` pattern.
struct RipplesPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `slope` pattern.
///
/// @todo   The additional member variables should be encapsulated, and computed by the class rather than the parser.
///
struct SlopePattern : public ContinuousPattern
{
    Vector3d slopeDirection;
    Vector3d altitudeDirection;
    signed char slopeAxis;    ///< one of 0 (non-axis-aligned), +/-1 (X axis), +/-2 (Y axis) or +/-3 (Z axis)
    signed char altitudeAxis; ///< one of 0 (non-axis-aligned), +/-1 (X axis), +/-2 (Y axis) or +/-3 (Z axis)
    DBL slopeLen;
    DBL altitudeLen;
    DBL slopeModLow;
    DBL slopeModWidth;
    DBL altitudeModLow;
    DBL altitudeModWidth;
    bool pointAt;

    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `spherical` pattern.
struct SphericalPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Defines the common additional interface of the `spiral` and `spiral2` patterns.
struct SpiralPattern : public ContinuousPattern
{
    short arms;

    virtual PatternPtr Clone() const = 0;
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const = 0;
};

/// Implements the `square` pattern.
struct SquarePattern : public DiscretePattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
    virtual unsigned int NumDiscreteBlendMapEntries() const;
};

/// Implements the `tiling` pattern.
///
/// @todo   We should probably implement this as one class per tiling type, possibly all declared in @ref pattern.cpp
///         and instantiated via a static factory method in this class.
///
struct TilingPattern : public ContinuousPattern
{
    unsigned char tilingType;

    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `triangular` pattern.
struct TriangularPattern : public DiscretePattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
    virtual unsigned int NumDiscreteBlendMapEntries() const;
};

/// Implements the `waves` pattern.
struct WavesPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `wood` pattern.
struct WoodPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
    virtual bool HasSpecialTurbulenceHandling() const;
};

/// Implements the `wrinkles` pattern.
struct WrinklesPattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};


/// Defines the common interface for all implementations of the julia pattern.
/// Also provides an implementation of the `julia` pattern optimized for `exponent 2` (default).
///
/// @todo   The `exponent 2` implementation should be derived from this class rather than be identical.
///
struct JuliaPattern : public FractalPattern
{
    Vector2d juliaCoord;

    JuliaPattern();
    JuliaPattern(const JuliaPattern& obj);
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Provides an implementation of the `julia` pattern optimized for `exponent 3`.
struct Julia3Pattern : public JuliaPattern
{
    Julia3Pattern();
    Julia3Pattern(const JuliaPattern& obj);
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Provides an implementation of the `julia` pattern optimized for `exponent 4`.
struct Julia4Pattern : public JuliaPattern
{
    Julia4Pattern();
    Julia4Pattern(const JuliaPattern& obj);
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Provides a generic implementation of the `julia` pattern for arbitrary exponents.
struct JuliaXPattern : public JuliaPattern
{
    int fractalExponent;

    JuliaXPattern();
    JuliaXPattern(const JuliaPattern& obj);
    JuliaXPattern(const JuliaXPattern& obj);
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};


/// Defines the common interface for all implementations of the `mandel` pattern.
struct MandelPattern : public FractalPattern
{
};

/// Provides an implementation of the `mandel` pattern optimized for `exponent 2` (default).
struct Mandel2Pattern : public MandelPattern
{
    Mandel2Pattern();
    Mandel2Pattern(const MandelPattern& obj);
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
};

/// Provides an implementation of the `mandel` pattern optimized for `exponent 3`.
struct Mandel3Pattern : public MandelPattern
{
    Mandel3Pattern();
    Mandel3Pattern(const MandelPattern& obj);
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Provides an implementation of the `mandel` pattern optimized for `exponent 4`.
struct Mandel4Pattern : public MandelPattern
{
    Mandel4Pattern();
    Mandel4Pattern(const MandelPattern& obj);
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Provides a generic implementation of the `mandel` pattern for arbitrary exponents.
struct MandelXPattern : public MandelPattern
{
    int fractalExponent;

    MandelXPattern();
    MandelXPattern(const MandelPattern& obj);
    MandelXPattern(const MandelXPattern& obj);
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};


/// Implements the `magnet 1 mandel` pattern.
struct Magnet1MPattern : public MandelPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `magnet 1 julia` pattern.
struct Magnet1JPattern : public JuliaPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `magnet 2 mandel` pattern.
struct Magnet2MPattern : public MandelPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `magnet 2 julia` pattern.
struct Magnet2JPattern : public JuliaPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};


/// Implements the `bozo` pattern.
struct BozoPattern : public NoisePattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const;
};

/// Implements the `bumps` pattern.
struct BumpsPattern : public NoisePattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
};

/// Implements the `spotted` pattern.
struct SpottedPattern : public NoisePattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
};


/// Implements the `spiral1` pattern.
struct Spiral1Pattern : public SpiralPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Implements the `spiral2` pattern.
struct Spiral2Pattern : public SpiralPattern
{
    virtual PatternPtr Clone() const { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const;
};

/// Helper class to implement the crackle cache.
class CrackleCellCoord
{
public:

    CrackleCellCoord() : mX(0), mY(0), mZ(0), mRepeatX(0), mRepeatY(0), mRepeatZ(0) {}
    CrackleCellCoord(int x, int y, int z, int rx, int ry, int rz) : mX(x), mY(y), mZ(z), mRepeatX(rx), mRepeatY(ry), mRepeatZ(rz)
    {
        WrapCellCoordinate(mX, mRepeatX);
        WrapCellCoordinate(mY, mRepeatY);
        WrapCellCoordinate(mZ, mRepeatZ);
    }

    bool operator==(CrackleCellCoord const& other) const
    {
        return mX == other.mX && mY == other.mY && mZ == other.mZ;
    }

    /// Function to compute a hash value from the coordinates.
    ///
    /// @note       This function's name, as well as it being a global function rather than a member, is mandated by
    ///             boost::unordered_map.
    ///
    /// @param[in]  coord   The coordinate.
    /// @return             The hash.
    ///
    friend std::size_t hash_value(CrackleCellCoord const& coord)
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, coord.mX);
        boost::hash_combine(seed, coord.mY);
        boost::hash_combine(seed, coord.mZ);

        return seed;
    }

protected:

    int mX;
    int mY;
    int mZ;
    int mRepeatX;
    int mRepeatY;
    int mRepeatZ;

    static inline void WrapCellCoordinate(int& v, int& repeat)
    {
        if (!repeat)
            return;
        v = wrapInt(v, repeat);
        if ((v >= 2) && (v < repeat - 2))
            repeat = 0;
    }
};

/// Helper class to implement the crackle cache.
struct CrackleCacheEntry
{
    /// A kind of timestamp specifying when this particular entry was last used.
    size_t lastUsed;

    /// The pseudo-random points defining the pattern in this particular subset of 3D space.
    Vector3d aCellNuclei[81];
};

typedef boost::unordered_map<CrackleCellCoord, CrackleCacheEntry, boost::hash<CrackleCellCoord> > CrackleCache;

/*****************************************************************************
* Global variables
******************************************************************************/


/*****************************************************************************
* Global constants
******************************************************************************/


/*****************************************************************************
* Global functions
******************************************************************************/

DBL Evaluate_TPat (const TPATTERN *TPat, const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread);
void Init_TPat_Fields (TPATTERN *Tpat);
void Copy_TPat_Fields (TPATTERN *New, const TPATTERN *Old);
void Translate_Tpattern (TPATTERN *Tpattern, const Vector3d& Vector);
void Rotate_Tpattern (TPATTERN *Tpattern, const Vector3d& Vector);
void Scale_Tpattern (TPATTERN *Tpattern, const Vector3d& Vector);
void Transform_Tpattern (TPATTERN *Tpattern, const TRANSFORM *Trans);
DBL quilt_cubic (DBL t,DBL p1,DBL p2);
int GetNoiseGen (const TPATTERN *TPat, const TraceThreadData *Thread);

DENSITY_FILE *Create_Density_File ();
DENSITY_FILE *Copy_Density_File (DENSITY_FILE *);
void Destroy_Density_File (DENSITY_FILE *);
void Read_Density_File (IStream *dfile, DENSITY_FILE *df);
int PickInCube (const Vector3d& tv, Vector3d& p1);

void InitializePatternGenerators(void);

}

#endif // POVRAY_CORE_PATTERN_H
