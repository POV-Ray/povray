//******************************************************************************
///
/// @file core/material/texture.h
///
/// Declarations related to textures.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_CORE_TEXTURE_H
#define POVRAY_CORE_TEXTURE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/coretypes.h"
#include "core/material/blendmap.h"
#include "core/material/portablenoise.h"
#include "core/support/simplevector.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreMaterialTexture Textures
/// @ingroup PovCore
///
/// @{

typedef struct Turb_Struct TURB;
struct GenericTurbulenceWarp;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define FLOOR(x)  ((x) >= 0.0 ? floor(x) : (0.0 - floor(0.0 - (x)) - 1.0))

// Hash1dRTableIndex assumed values in the range 0..8191
#define Hash1dRTableIndex(a,b)   \
    ((hashTable[(int)(a) ^ (b)] & 0xFF) * 2)

// Hash2d assumed values in the range 0..8191
#define Hash2d(a,b)   \
    hashTable[(int)(hashTable[(int)(a)] ^ (b))]

#define Hash3d(a,b,c) \
    hashTable[(int)(hashTable[(int)(hashTable[(int)((a) & 0xfff)] ^ ((b) & 0xfff))] ^ ((c) & 0xfff))]

const int NOISE_MINX = -10000;
const int NOISE_MINY = NOISE_MINX;
const int NOISE_MINZ = NOISE_MINX;


/*****************************************************************************
* Global typedefs
******************************************************************************/

struct WeightedTexture
{
    COLC weight;
    TEXTURE *texture;

    WeightedTexture(COLC w, TEXTURE *t) :
        weight(w), texture(t) { }
};

typedef FixedSimpleVector<WeightedTexture, WEIGHTEDTEXTURE_VECTOR_SIZE> WeightedTextureVector;


/// Texture blend map.
class TextureBlendMap : public BlendMap<TexturePtr>
{
    public:

        TextureBlendMap();
        ~TextureBlendMap();
};

typedef BlendMapEntry<TexturePtr>                   TextureBlendMapEntry;
typedef shared_ptr<TextureBlendMap>                 TextureBlendMapPtr;
typedef shared_ptr<const TextureBlendMap>           TextureBlendMapConstPtr;

struct Texture_Struct : public Pattern_Struct
{
    TextureBlendMapPtr Blend_Map;
    int References;
    TEXTURE *Next;
    PIGMENT *Pigment;
    TNORMAL *Tnormal;
    FINISH *Finish;
    vector<TEXTURE*> Materials; // used for `material_map` (and only there)
};

struct Finish_Struct
{
    SNGL Diffuse, DiffuseBack, Brilliance;
#if POV_PARSER_EXPERIMENTAL_BRILLIANCE_OUT
    SNGL BrillianceOut;
#endif
    SNGL BrillianceAdjust, BrillianceAdjustRad;
    SNGL Specular, Roughness;
    SNGL Phong, Phong_Size;
    SNGL Irid, Irid_Film_Thickness, Irid_Turb;
    SNGL Temp_Caustics, Temp_IOR, Temp_Dispersion, Temp_Refract, Reflect_Exp;
    SNGL Crand, Metallic;
    MathColour Ambient, Emission, Reflection_Max, Reflection_Min;
    MathColour SubsurfaceTranslucency, SubsurfaceAnisotropy;
    //MathColour SigmaPrimeS, SigmaA;
    SNGL Reflection_Falloff;  // Added by MBP 8/27/98
    bool Reflection_Fresnel;
    SNGL Fresnel;
    SNGL Reflect_Metallic; // MBP
    int Conserve_Energy;  // added by NK Dec 19 1999
    bool UseSubsurface;   // whether to use subsurface light transport
    bool AlphaKnockout;   // whether pigment alpha knocks out finish effects
};


/*****************************************************************************
* Global variables
******************************************************************************/

#ifdef DYNAMIC_HASHTABLE
extern unsigned short *hashTable;
#else
extern ALIGN16 unsigned short hashTable[8192];
#endif


/*****************************************************************************
* Global functions
******************************************************************************/

void Initialize_Noise (void);
void Initialize_Waves(vector<double>& waveFrequencies, vector<Vector3d>& waveSources, unsigned int numberOfWaves);
void Free_Noise_Tables (void);

DBL SolidNoise(const Vector3d& P);

#ifdef TRY_OPTIMIZED_NOISE

typedef DBL(*NoiseFunction) (const Vector3d& EPoint, int noise_generator);
typedef void(*DNoiseFunction) (Vector3d& result, const Vector3d& EPoint);

/// Optimized Noise Dispatch.
///
/// This function shall select one of multiple implementations of the noise generator functions
/// (the portable default implementations being @ref PortableNoise() and @ref PortableDNoise(),
/// respectively), depending on the runtime environment (typically CPU features).
///
/// This function shall also perform any additional initialization required by the selected noise
/// generator.
///
/// @note
///     This function must be implemented by platform-specific code.
///
/// @note
///     The caller may pass `NULL` for any of the parameters, indicating that it doesn't care about
///     that particular piece of information.
///
/// @note
///     If both @ref pFnNoise and @ref pFnDNoise are set to NULL by the caller, noise generator
///     initialization must be skipped, in order to avoid potential multithreading issues.
///
/// @note
///     If the function returns `false`, it shall leave all parameters unchanged. However,
///     the caller shall not rely on this behaviour, as it may be subject to future change.
///
/// @param[out] pFnNoise    Selected implementation of @ref PortableNoise().
/// @param[out] pFnDNoise   Selected implementation of @ref PortableDNoise().
/// @param[out] pDetected   String identifying the machine features detected (e.g. "AVX,non-Intel")
///                         on which the selection was based.
/// @param[out] pImpl       String unambiguously identifying the implementation selected.
/// @return                 `true` if an alternative implementation was chosen and the parameters
///                         set accordingly, `false` otherwise.
///
bool TryOptimizedNoise(NoiseFunction* pFnNoise, DNoiseFunction* pFnDNoise,
                       std::string* pDetected = NULL, std::string* pImpl = NULL);

extern NoiseFunction Noise;
extern DNoiseFunction DNoise;

void Initialise_NoiseDispatch();

#else // TRY_OPTIMIZED_NOISE

inline DBL Noise(const Vector3d& EPoint, int noise_generator) { return PortableNoise(EPoint, noise_generator); }
inline void DNoise(Vector3d& result, const Vector3d& EPoint) { PortableDNoise(result, EPoint); }

#endif // TRY_OPTIMIZED_NOISE

DBL Turbulence (const Vector3d& EPoint, const GenericTurbulenceWarp* Turb, int noise_generator);
void DTurbulence (Vector3d& result, const Vector3d& EPoint, const GenericTurbulenceWarp* Turb);
DBL cycloidal (DBL value);
DBL Triangle_Wave (DBL value);
void Transform_Textures (TEXTURE *Textures, const TRANSFORM *Trans);
void Destroy_Textures (TEXTURE *Textures);
void Post_Textures (TEXTURE *Textures);
FINISH *Create_Finish (void);
FINISH *Copy_Finish (const FINISH *Old);
TEXTURE *Copy_Texture_Pointer (TEXTURE *Texture);
TEXTURE *Copy_Textures (TEXTURE *Textures);
TEXTURE *Create_Texture (void);
int Test_Opacity (const TEXTURE *Texture);

/// @}
///
//##############################################################################

}

#endif // POVRAY_CORE_TEXTURE_H
