//******************************************************************************
///
/// @file core/material/pattern.h
///
/// Declarations related to patterns.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
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
#include "core/material/pattern_fwd.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/fileinputoutput_fwd.h"

// POV-Ray header files (core module)
#include "core/coretypes.h"
#include "core/material/pigment.h"
#include "core/material/warp.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreMaterialPattern Patterns
/// @ingroup PovCore
///
/// @{

//******************************************************************************

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
    BITMAP_PATTERN,         ///< image-based pattern (except `image_map`)
    IMAGE_MAP_PATTERN,      ///< `image_map`
    COLOUR_PATTERN,

    // The following former normal patterns require special handling.  They must be kept separate for now.

    WAVES_PATTERN,
    RIPPLES_PATTERN,
    WRINKLES_PATTERN,
    BUMPS_PATTERN,
    QUILTED_PATTERN,
    FACETS_PATTERN,
    DENTS_PATTERN,

    GENERIC_SPECIAL_NORM_PATTERN,   ///< Pattern does not need any legacy special handling anywhere, except for its property of having a special implementation for normals.

    GENERIC_PATTERN                 ///< Pattern does not need any legacy special handling anywhere
};

#define LAST_SPECIAL_PATTERN        COLOUR_PATTERN
#define LAST_SPECIAL_NORM_PATTERN   GENERIC_SPECIAL_NORM_PATTERN

/* flags for patterned stuff */

#define NO_FLAGS              0
#define HAS_FILTER            1
// value 2 is currently not used
#define POST_DONE             4
#define DONT_SCALE_BUMPS_FLAG 8 /* scale bumps for normals */

enum WaveType
{
    kWaveType_Raw,      ///< Use raw pattern value, allowing it to exceed the [0..1] range.
    kWaveType_Ramp,     ///< Ramps up from 0 to 1, then drops sharply back to 0, and repeats.
    kWaveType_Sine,     ///< Oscillates between 0 and 1 using a sine-based formula.
    kWaveType_Triangle, ///< Ramps up from 0 to 1, then ramps down from 1 to 0, and repeats.
    kWaveType_Scallop,
    kWaveType_Cubic,
    kWaveType_Poly
};

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

// required by ImagePatternImpl (defined in support/imageutil.h)
class ImageData;


//******************************************************************************
// Base Classes and Special Patterns

/// Abstract class providing the interface and commonly-used data fields for all pattern implementations.
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

    /// Test the pattern parameters and precompute derived values.
    ///
    /// @return True if pattern parameters are within reasonable limits.
    ///
    virtual bool Precompute();

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

    /// Whether the pattern can be used with maps.
    ///
    /// @return     `true` if the pattern can be used with maps.
    ///
    virtual bool CanMap() const = 0;

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

/// Abstract class providing additions to the basic pattern interface, as well as common code, for all
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
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override final;

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

    virtual unsigned int NumDiscreteBlendMapEntries() const override;
    virtual bool CanMap() const override;
};

/// Abstract class providing additions to the basic pattern interface, as well as common code, for all
/// discrete pattern implementations.
struct DiscretePattern : public BasicPattern
{
    virtual bool CanMap() const override;
};

/// Implements a plain pattern with all-zero values for any point in space.
struct PlainPattern final : public DiscretePattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual unsigned int NumDiscreteBlendMapEntries() const override;
    virtual bool HasSpecialTurbulenceHandling() const override;
};

/// Implements a dummy pattern for `average` pseudo-pattern.
struct AveragePattern final : public BasicPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual unsigned int NumDiscreteBlendMapEntries() const override;
    virtual bool HasSpecialTurbulenceHandling() const override;
    virtual bool CanMap() const override;
};

/// Class providing additional data members for image-based patterns.
///
/// @todo   The additional member variables should possibly be encapsulated.
///
struct ImagePatternImpl
{
    ImageData *pImage;

    ImagePatternImpl();
    ImagePatternImpl(const ImagePatternImpl& obj);
    virtual ~ImagePatternImpl();
};


//******************************************************************************
// Miscellaneous Patterns

/// Implements the `agate` pattern.
struct AgatePattern final : public ContinuousPattern
{
    /// `agate_turb` parameter.
    SNGL agateTurbScale;

    AgatePattern();
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual bool Precompute() override;
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
};

/// Implements the `aoi` pattern.
struct AOIPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `boxed` pattern.
struct BoxedPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `brick` pattern.
struct BrickPattern final : public DiscretePattern
{
    /// `brick_size` parameter.
    Vector3d brickSize;

    /// `mortar` parameter.
    SNGL mortar;

    BrickPattern();
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
    virtual unsigned int NumDiscreteBlendMapEntries() const override;
};

/// Implements the `cells` pattern.
struct CellsPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `checker` pattern.
struct CheckerPattern final : public DiscretePattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
    virtual unsigned int NumDiscreteBlendMapEntries() const override;
};

/// Implements the `crackle` pattern.
struct CracklePattern final : public ContinuousPattern
{
    Vector3d crackleForm;
    DBL crackleMetric;
    DBL crackleOffset;
    IntVector3d repeat;
    bool crackleIsSolid;

    CracklePattern();
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `cubic` pattern.
struct CubicPattern final : public DiscretePattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
    virtual unsigned int NumDiscreteBlendMapEntries() const override;
};

/// Implements the `cylindrical` pattern.
struct CylindricalPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `density_file` pattern.
///
/// @todo   The additional member variables should be encapsulated.
///
struct DensityFilePattern final : public ContinuousPattern
{
    /// @todo fix the members to match naming conventions
    struct DensityFileDataStruct final
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
    struct DensityFileStruct final
    {
        int Interpolation; ///< one of @ref DensityFileInterpolationType
        DensityFileDataStruct *Data;
    };

    DensityFileStruct *densityFile;

    DensityFilePattern();
    DensityFilePattern(const DensityFilePattern& obj);
    virtual ~DensityFilePattern() override;
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};
using DENSITY_FILE      = DensityFilePattern::DensityFileStruct;      ///< @deprecated @ref DensityFilePattern::DensityFileStruct should be used instead.
using DENSITY_FILE_DATA = DensityFilePattern::DensityFileDataStruct ; ///< @deprecated @ref DensityFilePattern::DensityFileDataStruct should be used instead.

/// Implements the `dents` pattern.
struct DentsPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `facets` pattern.
struct FacetsPattern final : public ContinuousPattern
{
    DBL facetsSize, facetsCoords, facetsMetric;

    FacetsPattern();
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }

    /// @attention  As the `facets` pattern is only available for normals, this function is not supposed to be ever
    ///             called, and will throw an exception.
    ///
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `function` pattern.
///
/// @todo   The additional member variables should be encapsulated.
///
struct FunctionPattern final : public ContinuousPattern
{
    GenericScalarFunctionPtr pFn;

    FunctionPattern();
    FunctionPattern(const FunctionPattern& obj);
    virtual ~FunctionPattern() override;
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `gradient` pattern.
struct GradientPattern final : public ContinuousPattern
{
    /// Direction of the gradient.
    Vector3d gradient;

    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `granite` pattern.
struct GranitePattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `hexagon` pattern.
struct HexagonPattern final : public DiscretePattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
    virtual unsigned int NumDiscreteBlendMapEntries() const override;
};

/// Implements image-based mapped patterns.
struct ImagePattern final : public ContinuousPattern, public ImagePatternImpl
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `leopard` pattern.
struct LeopardPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `marble` pattern.
struct MarblePattern final : public ContinuousPattern
{
    MarblePattern();
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual bool Precompute() override;
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
    virtual bool HasSpecialTurbulenceHandling() const override;

protected:

    bool hasTurbulence : 1;
};

/// Base class for the noise-based patterns.
struct NoisePattern : public ContinuousPattern
{
    virtual PatternPtr Clone() const override = 0;
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `object` pattern.
///
/// @todo   The additional member variables should possibly be encapsulated.
///
struct ObjectPattern final : public DiscretePattern
{
    ObjectPtr pObject;

    ObjectPattern();
    ObjectPattern(const ObjectPattern& obj);
    virtual ~ObjectPattern() override;
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
    virtual unsigned int NumDiscreteBlendMapEntries() const override;
};

/// Implements the `onion` pattern.
struct OnionPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `pavement` pattern.
///
/// @todo   We should probably implement this as one class per pavement type, possibly all declared in @ref pattern.cpp
///         and instantiated via a static factory method in this class.
///
struct PavementPattern final : public ContinuousPattern
{
    unsigned char Side;
    unsigned char Tile;
    unsigned char Number;
    unsigned char Exterior;
    unsigned char Interior;
    unsigned char Form;

    PavementPattern();
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    DBL hexagonal (const Vector3d& EPoint) const;
    DBL trigonal (const Vector3d& EPoint) const;
    DBL tetragonal (const Vector3d& EPoint) const;
};

/// Implements the `pigment_pattern` pattern.
///
/// @todo   The additional member variables should possibly be encapsulated.
///
struct PigmentPattern final : public ContinuousPattern
{
    PIGMENT *pPigment;

    PigmentPattern();
    PigmentPattern(const PigmentPattern& obj);
    virtual ~PigmentPattern() override;
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `planar` pattern.
struct PlanarPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `potential` pattern.
///
/// @todo   The additional member variables should possibly be encapsulated.
///
struct PotentialPattern final : public ContinuousPattern
{
    ObjectPtr   pObject;
    bool        subtractThreshold;

    PotentialPattern();
    PotentialPattern(const PotentialPattern& obj);
    virtual ~PotentialPattern() override;
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `quilted` pattern.
struct QuiltedPattern final : public ContinuousPattern
{
    SNGL Control0, Control1;

    QuiltedPattern();
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `radial` pattern.
struct RadialPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
};

/// Implements the `ripples` pattern.
struct RipplesPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `slope` pattern.
///
/// @todo   The additional member variables should be encapsulated, and computed by the class rather than the parser.
///
struct SlopePattern final : public ContinuousPattern
{
    Vector3d    altitudeDirection;
    Vector3d    slopeDirection;
    DBL         altitudeLen;
    DBL         altitudeModLow;
    DBL         altitudeModWidth;
    DBL         slopeLen;
    DBL         slopeModLow;
    DBL         slopeModWidth;
    signed char altitudeAxis    : 3; ///< one of 0 (non-axis-aligned), +/-1 (X axis), +/-2 (Y axis) or +/-3 (Z axis)
    signed char slopeAxis       : 3; ///< one of 0 (non-axis-aligned), +/-1 (X axis), +/-2 (Y axis) or +/-3 (Z axis)
    bool        pointAt         : 1;

    SlopePattern();
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `spherical` pattern.
struct SphericalPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `square` pattern.
struct SquarePattern final : public DiscretePattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
    virtual unsigned int NumDiscreteBlendMapEntries() const override;
};

/// Implements the `tiling` pattern.
///
/// @todo   We should probably implement this as one class per tiling type, possibly all declared in @ref pattern.cpp
///         and instantiated via a static factory method in this class.
///
struct TilingPattern final : public ContinuousPattern
{
    unsigned char tilingType;

    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `triangular` pattern.
struct TriangularPattern final : public DiscretePattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
    virtual unsigned int NumDiscreteBlendMapEntries() const override;
};

/// Implements the `waves` pattern.
struct WavesPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `wood` pattern.
struct WoodPattern final : public ContinuousPattern
{
    WoodPattern();
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual bool Precompute() override;
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
    virtual bool HasSpecialTurbulenceHandling() const override;

protected:

    bool hasTurbulence : 1;
};

/// Implements the `wrinkles` pattern.
struct WrinklesPattern final : public ContinuousPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};


//******************************************************************************
// Fractal Patterns

/// Abstract class providing additions to the basic pattern interface, as well as common code, for all fractal patterns.
struct FractalPattern : public ContinuousPattern
{
    /// A parameter to the algorithm for colouring the exterior of the fractal.
    DBL exteriorFactor;

    /// A parameter to the algorithm for colouring the interior of the fractal.
    DBL interiorFactor;

    /// Maximum number of iterations.
    unsigned int maxIterations;

    /// Determines the algorithm to colour the exterior of the fractal.
    unsigned char exteriorType;

    /// Determines the algorithm to colour the interior of the fractal.
    unsigned char interiorType;

    FractalPattern();
    virtual PatternPtr Clone() const override = 0;
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override = 0;

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
    /// @param  mindist2    Square of the smallest distance to the origin throughout all iterations.
    /// @return             The interior shade.
    ///
    DBL InteriorColour(DBL a, DBL b, DBL mindist2) const;
};

//------------------------------------------------------------------------------
// Julia Patterns

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
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Provides an implementation of the `julia` pattern optimized for `exponent 3`.
struct Julia3Pattern final : public JuliaPattern
{
    Julia3Pattern();
    Julia3Pattern(const JuliaPattern& obj);
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Provides an implementation of the `julia` pattern optimized for `exponent 4`.
struct Julia4Pattern final : public JuliaPattern
{
    Julia4Pattern();
    Julia4Pattern(const JuliaPattern& obj);
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Provides a generic implementation of the `julia` pattern for arbitrary exponents.
struct JuliaXPattern final : public JuliaPattern
{
    int fractalExponent;

    JuliaXPattern();
    JuliaXPattern(const JuliaPattern& obj);
    JuliaXPattern(const JuliaXPattern& obj);
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

//------------------------------------------------------------------------------
// Mandelbrot Patterns

/// Defines the common interface for all implementations of the `mandel` pattern.
struct MandelPattern : public FractalPattern
{
};

/// Provides an implementation of the `mandel` pattern optimized for `exponent 2` (default).
struct Mandel2Pattern final : public MandelPattern
{
    Mandel2Pattern();
    Mandel2Pattern(const MandelPattern& obj);
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
};

/// Provides an implementation of the `mandel` pattern optimized for `exponent 3`.
struct Mandel3Pattern final : public MandelPattern
{
    Mandel3Pattern();
    Mandel3Pattern(const MandelPattern& obj);
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Provides an implementation of the `mandel` pattern optimized for `exponent 4`.
struct Mandel4Pattern final : public MandelPattern
{
    Mandel4Pattern();
    Mandel4Pattern(const MandelPattern& obj);
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Provides a generic implementation of the `mandel` pattern for arbitrary exponents.
struct MandelXPattern final : public MandelPattern
{
    int fractalExponent;

    MandelXPattern();
    MandelXPattern(const MandelPattern& obj);
    MandelXPattern(const MandelXPattern& obj);
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

//------------------------------------------------------------------------------
// Magnet Patterns

/// Implements the `magnet 1 mandel` pattern.
struct Magnet1MPattern final : public MandelPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `magnet 1 julia` pattern.
struct Magnet1JPattern final : public JuliaPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `magnet 2 mandel` pattern.
struct Magnet2MPattern final : public MandelPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `magnet 2 julia` pattern.
struct Magnet2JPattern final : public JuliaPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};


//******************************************************************************
// Noise-Based Patterns

/// Implements the `bozo` pattern.
struct BozoPattern final : public NoisePattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual ColourBlendMapConstPtr GetDefaultBlendMap() const override;
};

/// Implements the `bumps` pattern.
struct BumpsPattern final : public NoisePattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
};

/// Implements the `spotted` pattern.
struct SpottedPattern final : public NoisePattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
};


//******************************************************************************
// Spiral Patterns

/// Defines the common additional interface of the `spiral` and `spiral2` patterns.
struct SpiralPattern : public ContinuousPattern
{
    short arms;

    SpiralPattern();
    virtual PatternPtr Clone() const override = 0;
    virtual bool Precompute() override;
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override = 0;

protected:

    bool hasTurbulence : 1;
};

/// Implements the `spiral1` pattern.
struct Spiral1Pattern final : public SpiralPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};

/// Implements the `spiral2` pattern.
struct Spiral2Pattern final : public SpiralPattern
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual DBL EvaluateRaw(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
};


//******************************************************************************
// Coloured Patterns

/// Abstract class providing additions to the basic pattern interface, as well as common code, for all patterns
/// returning colours.
///
struct ColourPattern : public BasicPattern
{
    ColourPattern();
    ColourPattern(const ColourPattern& obj);

    /// Evaluates the pattern at a given point in space.
    ///
    /// @note   Derived classes should _not_ override this, but
    ///         @ref Evaluate(TransColour&, const Vector3d&, const Intersection*, const Ray*,TraceThreadData*) const
    ///         instead.
    ///
    /// @param[in]      EPoint      The point of interest in 3D space.
    /// @param[in]      pIsection   Additional information about the intersection. Evaluated by some patterns.
    /// @param[in]      pRay        Additional information about the ray. Evaluated by some patterns.
    /// @param[in,out]  pThread     Additional thread-local data. Evaluated by some patterns. Some patterns, such as the
    ///                             crackle pattern, store cached data here.
    /// @return                     The pattern's value at the given point in space.
    ///
    virtual DBL Evaluate(const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override final;

    virtual unsigned int NumDiscreteBlendMapEntries() const override;
    virtual bool CanMap() const override;

    /// Evaluates the pattern at a given point in space.
    ///
    /// @param[out]     result      The pattern's colour at the given point in space.
    /// @param[in]      EPoint      The point of interest in 3D space.
    /// @param[in]      pIsection   Additional information about the intersection. Evaluated by some patterns.
    /// @param[in]      pRay        Additional information about the ray. Evaluated by some patterns.
    /// @param[in,out]  pThread     Additional thread-local data. Evaluated by some patterns. Some patterns, such as the
    ///                             crackle pattern, store cached data here.
    /// @return                     `false` if the pattern is undefined at the given point in space.
    ///
    virtual bool Evaluate(TransColour& result, const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const = 0;

    /// Whether the pattern has transparency.
    ///
    /// @return     `true` if the pattern has transparency.
    ///
    virtual bool HasTransparency() const = 0;
};

/// Implements the `user_defined` pattern.
///
/// @todo   The additional member variables should possibly be encapsulated.
///
struct ColourFunctionPattern final : public ColourPattern
{
    GenericScalarFunctionPtr pFn[5];

    ColourFunctionPattern();
    ColourFunctionPattern(const ColourFunctionPattern& obj);
    virtual ~ColourFunctionPattern() override;
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual bool Evaluate(TransColour& result, const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual bool HasTransparency() const override;
};

/// Implements the `image_map` pattern.
struct ColourImagePattern final : public ColourPattern, public ImagePatternImpl
{
    virtual PatternPtr Clone() const override { return BasicPattern::Clone(*this); }
    virtual bool Evaluate(TransColour& result, const Vector3d& EPoint, const Intersection *pIsection, const Ray *pRay, TraceThreadData *pThread) const override;
    virtual bool HasTransparency() const override;
};


//******************************************************************************
// Legacy Global Functions

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

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_PATTERN_H
