//******************************************************************************
///
/// @file core/material/texture.h
///
/// Declarations related to textures.
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

#ifndef POVRAY_CORE_TEXTURE_H
#define POVRAY_CORE_TEXTURE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <vector>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/coretypes.h"
#include "core/material/blendmap.h"
#include "core/support/simplevector.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreMaterialTexture Textures
/// @ingroup PovCore
///
/// @{

struct GenericTurbulenceWarp;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

#define FLOOR(x)  ((x) >= 0.0 ? floor(x) : (0.0 - floor(0.0 - (x)) - 1.0))

#define Hash3d(a,b,c) \
    hashTable[(int)(hashTable[(int)(hashTable[(int)((a) & 0xfff)] ^ ((b) & 0xfff))] ^ ((c) & 0xfff))]


/*****************************************************************************
* Global typedefs
******************************************************************************/

struct WeightedTexture final
{
    COLC weight;
    TEXTURE *texture;

    WeightedTexture(COLC w, TEXTURE *t) :
        weight(w), texture(t) { }
};

typedef PooledSimpleVector<WeightedTexture, WEIGHTEDTEXTURE_VECTOR_SIZE> WeightedTextureVector;


/// Texture blend map.
class TextureBlendMap final : public BlendMap<TexturePtr>
{
    public:

        TextureBlendMap();
        virtual ~TextureBlendMap() override;
};

typedef BlendMapEntry<TexturePtr>                   TextureBlendMapEntry;
typedef std::shared_ptr<TextureBlendMap>            TextureBlendMapPtr;
typedef std::shared_ptr<const TextureBlendMap>      TextureBlendMapConstPtr;

struct Texture_Struct final : public Pattern_Struct
{
    TextureBlendMapPtr Blend_Map;
    int References;
    TEXTURE *Next;
    PIGMENT *Pigment;
    TNORMAL *Tnormal;
    FINISH *Finish;
    std::vector<TEXTURE*> Materials; // used for `material_map` (and only there)
};

struct Finish_Struct final
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
* Global functions
******************************************************************************/

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
// end of namespace pov

#endif // POVRAY_CORE_TEXTURE_H
