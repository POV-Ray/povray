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

#ifndef POVRAY_CORE_TEXTURE_H
#define POVRAY_CORE_TEXTURE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/coretypes.h"
#include "core/material/blendmap.h"
#include "core/support/simplevector.h"

namespace pov
{

typedef struct Turb_Struct TURB;
struct GenericTurbulenceWarp;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/*
 * Macro to create random number in the [0; 1] range.
 */

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


/// Texture blend map.
class TextureBlendMap : public BlendMap<TextureData>
{
    public:

        TextureBlendMap();
        ~TextureBlendMap();
};

typedef BlendMapEntry<TextureData>                  TextureBlendMapEntry;
typedef shared_ptr<TextureBlendMap>                 TextureBlendMapPtr;
typedef shared_ptr<const TextureBlendMap>           TextureBlendMapConstPtr;

struct Texture_Struct;

struct Finish_Struct
{
    SNGL Diffuse, DiffuseBack, Brilliance, BrillianceOut, BrillianceAdjust, BrillianceAdjustRad;
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
    bool Fresnel;
    SNGL Reflect_Metallic; // MBP
    int Conserve_Energy;  // added by NK Dec 19 1999
    bool UseSubsurface;   // whether to use subsurface light transport
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

#if defined(TRY_OPTIMIZED_NOISE)
extern DBL (*Noise) (const Vector3d& EPoint, int noise_generator);
extern void (*DNoise) (Vector3d& result, const Vector3d& EPoint);
void Initialise_NoiseDispatch();
#else
INLINE_NOISE DBL Noise (const Vector3d& EPoint, int noise_generator);
INLINE_NOISE void DNoise (Vector3d& result, const Vector3d& EPoint);
#endif

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

struct TextureLayer;
typedef vector<TextureLayer*> TextureLayerList;

class TextureData
{
public:
    friend struct WeightedTexture;
    inline TextureData() : mTexture(NULL) {}
    inline TextureData(const TextureData& o) : mTexture(o.mTexture) {}
    inline ~TextureData() {}
    inline void Kill() { mTexture = NULL; }
    inline bool IsEmpty() const { return mTexture == NULL; }
    inline bool IsLayered() const;
    inline bool IsOpaque() const { return Test_Opacity(mTexture); }
    inline void Transform(const Transform_Struct& trans) { Transform_Textures(mTexture, &trans); }
    inline void Create() { mTexture = Create_Texture(); }
    TextureData GetCopy() const { TextureData temp; temp.SetCopy(*this); return temp; }
    inline void SetCopy(const TextureData& o) { mTexture = Copy_Textures(o.mTexture); }
    inline void SetShallowCopy(const TextureData& o) { mTexture = Copy_Texture_Pointer(o.mTexture); }
    inline void Destroy() { Destroy_Textures(mTexture); mTexture = NULL; }
    inline TextureData& operator= (const TextureData& o) { mTexture = o.mTexture; return *this; }
    inline void Post() { Post_Textures(mTexture); }
    inline TextureLayer* FirstLayer();
    inline const TextureLayer* FirstLayer() const;
    inline bool operator==(const TextureData& o) const { return mTexture == o.mTexture; }
    inline bool operator!=(const TextureData& o) const { return mTexture != o.mTexture; }
    void Link(TextureData o, bool legacyMode);

    inline unsigned short PatternFlags() const;
    inline unsigned short& PatternFlags();
    inline void SetPatternFlags(unsigned short);

    inline unsigned short PatternType() const;
    inline unsigned short& PatternType();

    inline const TPATTERN* Pattern() const;
    inline TPATTERN* Pattern();

    inline const TextureBlendMapPtr& BlendMap() const;
    inline TextureBlendMapPtr BlendMap();

    inline const TextureLayerList& Layers() const;
    inline TextureLayerList& Layers();

    inline const vector<TextureData>& Materials() const;
    inline vector<TextureData>& Materials();

    inline const TEXTURE* Raw() const { return mTexture; }
    inline TEXTURE* Raw() { return mTexture; }

protected:
    TEXTURE* mTexture;
};

struct WeightedTexture
{
    COLC weight;
    TextureData texture;

    WeightedTexture(COLC w, TextureData& t) :
        weight(w), texture(t) { }
};

typedef FixedSimpleVector<WeightedTexture, WEIGHTEDTEXTURE_VECTOR_SIZE> WeightedTextureVector;

struct Material_Struct
{
    TextureData Texture;
    TextureData Interior_Texture;
    InteriorPtr interior;
};


struct TextureLayer
{
    TextureLayer() : Pigment(NULL), Tnormal(NULL), Finish(NULL) {}
    PIGMENT *Pigment;
    TNORMAL *Tnormal;
    FINISH *Finish;
};

struct Texture_Struct : public Pattern_Struct
{
    TextureBlendMapPtr Blend_Map;
    int References;
    TextureLayerList layers;
    vector<TextureData> Materials; // used for BITMAP_PATTERN (and only there)
};

inline bool TextureData::IsLayered() const
{
    return (mTexture != NULL) && (mTexture->layers.size() > 1);
}

inline TextureLayer* TextureData::FirstLayer() { return mTexture->layers.back(); }
inline const TextureLayer* TextureData::FirstLayer() const { return mTexture->layers.back(); }

inline unsigned short TextureData::PatternFlags() const { return mTexture->Flags; }
inline unsigned short& TextureData::PatternFlags() { return mTexture->Flags; }

inline unsigned short TextureData::PatternType() const { return mTexture->Type; }
inline unsigned short& TextureData::PatternType() { return mTexture->Type; }

inline const TPATTERN* TextureData::Pattern() const { return mTexture; }
inline TPATTERN* TextureData::Pattern() { return mTexture; }

inline const TextureBlendMapPtr& TextureData::BlendMap() const { return mTexture->Blend_Map; }
inline TextureBlendMapPtr TextureData::BlendMap() { return mTexture->Blend_Map; }

inline const TextureLayerList& TextureData::Layers() const { return mTexture->layers; }
inline TextureLayerList& TextureData::Layers() { return mTexture->layers; }

inline const vector<TextureData>& TextureData::Materials() const { return mTexture->Materials; }
inline vector<TextureData>& TextureData::Materials() { return mTexture->Materials; }

}

#endif // POVRAY_CORE_TEXTURE_H
