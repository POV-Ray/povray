//******************************************************************************
///
/// @file core/material/noise.h
///
/// Declarations related to noise.
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

#ifndef POVRAY_CORE_NOISE_H
#define POVRAY_CORE_NOISE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <string>
#include <vector>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/material/warp.h"
#include "core/math/vector.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreMaterialNoise Noise
/// @ingroup PovCore
///
/// @{

enum NoiseGenType
{
    kNoiseGen_Default        = 0, ///< Indicates that the scene's global settings noise generator should be used.
    kNoiseGen_Original       = 1, ///< POV-Ray original noise generator (pre-v3.5).
    kNoiseGen_RangeCorrected = 2, ///< POV-Ray original noise generator with range correction (v3.5 and later).
    kNoiseGen_Perlin         = 3  ///< Perlin noise generator.
};
const int kNoiseGen_Min = 1;
const int kNoiseGen_Max = 3;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

// Hash1dRTableIndex assumed values in the range 0..8191
#define Hash1dRTableIndex(a,b)   \
    ((hashTable[(int)(a) ^ (b)] & 0xFF) * 2)

// Hash2d assumed values in the range 0..8191
#define Hash2d(a,b)   \
    hashTable[(int)(hashTable[(int)(a)] ^ (b))]

const int NOISE_MINX = -10000;
const int NOISE_MINY = NOISE_MINX;
const int NOISE_MINZ = NOISE_MINX;


/*****************************************************************************
* Global variables
******************************************************************************/

#ifdef DYNAMIC_HASHTABLE
extern unsigned short *hashTable;
#else
alignas(16) extern unsigned short hashTable[8192];
#endif

alignas(16) extern DBL RTable[];


/*****************************************************************************
* Global functions
******************************************************************************/

void Initialize_Noise (void);
void Initialize_Waves(std::vector<double>& waveFrequencies, std::vector<Vector3d>& waveSources, unsigned int numberOfWaves);
void Free_Noise_Tables (void);

DBL SolidNoise(const Vector3d& P);

DBL PortableNoise(const Vector3d& EPoint, int noise_generator);
void PortableDNoise(Vector3d& result, const Vector3d& EPoint);

#ifdef TRY_OPTIMIZED_NOISE

typedef DBL(*NoiseFunction) (const Vector3d& EPoint, int noise_generator);
typedef void(*DNoiseFunction) (Vector3d& result, const Vector3d& EPoint);

/// Optimized noise dispatch information.
struct OptimizedNoiseInfo final
{
    /// String unambiguously identifying the optimized implementation.
    ///
    /// @note
    ///     The string must be _all-lowercase_ ASCII, and should contain only letters, digits,
    ///     dashes (`-`) and/or dots (`.`).
    ///
    /// In general, the format should be `FEATURES-AUTHOR`[`.N`], where `FEATURES` is an
    /// all-lowercase alphanumeric field identifying the CPU features required for the
    /// implementation to run (e.g. `avxfma4`), `AUTHOR` is another all-lowercase alphanumeric
    /// field identifying the person or organization who provided the implementation,
    /// and `N` is an optional integer numeric field identifying a particular revision of the
    /// implementation. For compiler-optimized code based on the portable implementation, the
    /// `AUTHOR` field should be `generic`.
    ///
    const char* name;

    /// String providing noteworthy details about the implementation.
    const char* info;

    /// Pointer to the optimized implementation of @ref PortableNoise().
    NoiseFunction noise;

    /// Pointer to the optimized implementation of @ref PortableDNoise().
    DNoiseFunction dNoise;

    /// Pointer to a constant indicating whether the implementation is enabled in the binary.
    const bool* enabled;

    /// Pointer to a function testing whether the optimized implementation is supported.
    /// A value of `nullptr` indicates universal support.
    bool(*supported)();

    /// Pointer to a function testing for additional auto-selection constraints.
    /// A value of `nullptr` indicates the absence of such constraints.
    bool(*recommended)();

    /// Pointer to the initialization function.
    /// A value of `nullptr` indicates that initialization is not required.
    void(*init)();
};

/// Optimized noise dispatch table.
///
/// This table contains a list of all available optimized noise generator implementations, sorted
/// by descending order of preference. The end of the table is indicated by an entry with the
/// `name` field set to `nullptr`.
///
/// @note
///     The non-optimized portable default noise generator implementation should _not_ be included
///     in the list.
///
/// @note
///     This table must be implemented by platform-specific code.
///
extern OptimizedNoiseInfo gaOptimizedNoiseInfo[];

/// Get the recommended noise generator implementation for the current runtime environment.
const OptimizedNoiseInfo* GetRecommendedOptimizedNoise();

/// Get a specific noise generator implementation.
const OptimizedNoiseInfo* GetOptimizedNoise(const std::string& name);

extern NoiseFunction Noise;
extern DNoiseFunction DNoise;

void Initialise_NoiseDispatch();

#else // TRY_OPTIMIZED_NOISE

inline DBL Noise(const Vector3d& EPoint, int noise_generator) { return PortableNoise(EPoint, noise_generator); }
inline void DNoise(Vector3d& result, const Vector3d& EPoint) { PortableDNoise(result, EPoint); }

#endif // TRY_OPTIMIZED_NOISE

DBL Turbulence (const Vector3d& EPoint, const GenericTurbulenceWarp* Turb, int noise_generator);
void DTurbulence (Vector3d& result, const Vector3d& EPoint, const GenericTurbulenceWarp* Turb);

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_NOISE_H
