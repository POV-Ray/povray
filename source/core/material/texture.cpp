//******************************************************************************
///
/// @file core/material/texture.cpp
///
/// Implementations related to textures.
///
/// The code in this file is mainly related to perturbations and transformations
/// of textures. The implementation of unperturbed textures resides in
/// @ref pigment.cpp (surface colouring) and @ref normal.cpp (surface normal
/// perturbation), as well as in @ref trace.cpp (surface finish).
///
/// @copyright
/// @parblock
///
/// Some texture ideas garnered from SIGGRAPH '85 Volume 19 Number 3, "An Image
/// Synthesizer" By Ken Perlin. Further Ideas Garnered from "The RenderMan
/// Companion" (Addison Wesley).
///
/// ----------------------------------------------------------------------------
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/material/texture.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"
#include "base/povassert.h"

// POV-Ray header files (core module)
#include "core/material/pattern.h"
#include "core/material/pigment.h"
#include "core/material/normal.h"
#include "core/support/imageutil.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::vector;

/*****************************************************************************
*
* FUNCTION
*
*   cycloidal
*
* INPUT
*
*   DBL value
*
* OUTPUT
*
* RETURNS
*
*   DBL result
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

DBL cycloidal(DBL value)
{
    if (value >= 0.0)
    {
        return sin((DBL) (((value - floor(value)) * 50000.0)) / 50000.0 * TWO_M_PI);
    }
    else
    {
        return 0.0-sin((DBL) (((0.0 - (value + floor(0.0 - value))) * 50000.0)) / 50000.0 * TWO_M_PI);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Triangle_Wave
*
* INPUT
*
*   DBL value
*
* OUTPUT
*
* RETURNS
*
*   DBL result
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

DBL Triangle_Wave(DBL value)
{
    DBL offset;

    if (value >= 0.0)
    {
        offset = value - floor(value);
    }
    else
    {
        offset = value + 1.0 + floor(fabs(value));
    }
    if (offset >= 0.5)
    {
        return (2.0 * (1.0 - offset));
    }
    else
    {
        return (2.0 * offset);
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Transform_Textures(TEXTURE *Textures, const TRANSFORM *Trans)
{
    TEXTURE *Layer;

    for (Layer = Textures; Layer != nullptr; Layer = Layer->Next)
    {
        if (Layer->Type == PLAIN_PATTERN)
        {
            Transform_Tpattern(Layer->Pigment, Trans);
            Transform_Tpattern(Layer->Tnormal, Trans);
        }
        else
        {
            Transform_Tpattern(Layer, Trans);
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*   6/27/98  MBP  Added initializers for reflection blur
*   8/27/98  MBP  Added initializers for angle-based reflectivity
*
* NOTES
*   since v3.8, the default ambient will be be overriden to 0.0 when the first statement
*   is a #version with value 3.8 or greater or when such version is set explicitly in command line or ini file
*
******************************************************************************/

FINISH *Create_Finish()
{
    FINISH *New;

    New = new FINISH;

    New->Ambient.Set(0.1);
    New->Emission.Clear();
    New->Reflection_Max.Clear();
    New->Reflection_Min.Clear();

    New->Reflection_Fresnel     = false;
    New->Reflection_Falloff     = 1;    /* Added by MBP 8/27/98 */
    New->Diffuse                = 0.6;
    New->DiffuseBack            = 0.0;
    New->Brilliance             = 1.0;
#if POV_PARSER_EXPERIMENTAL_BRILLIANCE_OUT
    New->BrillianceOut          = 1.0;
#endif
    New->BrillianceAdjust       = 1.0;
    New->BrillianceAdjustRad    = 1.0;
    New->Phong                  = 0.0;
    New->Phong_Size             = 40.0;
    New->Specular               = 0.0;
    New->Roughness              = 1.0 / 0.05;

    New->Crand = 0.0;

    New->Metallic = 0.0;
    New->Fresnel  = 0.0;

    New->Irid                = 0.0;
    New->Irid_Film_Thickness = 0.0;
    New->Irid_Turb           = 0.0;
    New->Temp_Caustics = -1.0;
    New->Temp_IOR     = -1.0;
    New->Temp_Dispersion  = 1.0;
    New->Temp_Refract =  1.0;
    New->Reflect_Exp  =  1.0;
    New->Reflect_Metallic = 0.0;
    /* Added Dec 19 1999 by NK */
    New->Conserve_Energy = false;

    New->UseSubsurface = false;
    New->SubsurfaceTranslucency.Clear();
    New->SubsurfaceAnisotropy.Clear();

    New->AlphaKnockout = false;

    return(New);
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
* CHANGES
*
******************************************************************************/

FINISH *Copy_Finish(const FINISH *Old)
{
    FINISH *New;

    if (Old != nullptr)
    {
        New = Create_Finish();
        *New = *Old;
    }
    else
        New = nullptr;
    return (New);
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
* CHANGES
*
******************************************************************************/

TEXTURE *Create_Texture()
{
    TEXTURE *New;

    New = new TEXTURE;

    Init_TPat_Fields(New);

    New->Next = nullptr;
    New->References = 1;

    New->Type    = PLAIN_PATTERN;
    New->Flags  |= NO_FLAGS; // [CLi] Already initialized by Init_TPat_Fields

    New->Pigment = nullptr;
    New->Tnormal = nullptr;
    New->Finish  = nullptr;

    New->Next    = nullptr;

    return (New);
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
* CHANGES
*
******************************************************************************/

TEXTURE *Copy_Texture_Pointer(TEXTURE *Texture)
{
    if (Texture != nullptr)
    {
        Texture->References++;
    }

    return(Texture);
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
* CHANGES
*
******************************************************************************/

TEXTURE *Copy_Textures(TEXTURE *Textures)
{
    TEXTURE *New, *First, *Previous;
    TEXTURE *Layer;

    Previous = First = nullptr;

    for (Layer = Textures; Layer != nullptr; Layer = Layer->Next)
    {
        New = Create_Texture();
        Copy_TPat_Fields (New, Layer);
        New->Blend_Map = Copy_Blend_Map<TextureBlendMap>(Layer->Blend_Map);

        /*  Mesh copies a texture pointer that already has multiple
            references.  We just want a clean copy, not a copy
            that's multiply referenced.
         */

        New->References = 1;

        switch (Layer->Type)
        {
            case PLAIN_PATTERN:
                New->Pigment = Copy_Pigment(Layer->Pigment);
                New->Tnormal = Copy_Tnormal(Layer->Tnormal);
                New->Finish  = Copy_Finish(Layer->Finish);

                break;

            case BITMAP_PATTERN:
                New->Materials.reserve(Layer->Materials.size());
                for (vector<TEXTURE*>::const_iterator i = Layer->Materials.begin(); i != Layer->Materials.end(); ++ i)
                    New->Materials.push_back(Copy_Textures(*i));

//              Not needed. Copied by Copy_TPat_Fields:
//              New->Vals.Image  = Copy_Image(Layer->Vals.Image);

                break;
        }

        if (First == nullptr)
        {
            First = New;
        }

        if (Previous != nullptr)
        {
            Previous->Next = New;
        }

        Previous = New;
    }

    return (First);
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
* CHANGES
*
******************************************************************************/

void Destroy_Textures(TEXTURE *Textures)
{
    TEXTURE *Layer = Textures;
    TEXTURE *Temp;

    if ((Textures == nullptr) || (--(Textures->References) > 0))
    {
        return;
    }

    while (Layer != nullptr)
    {
        // Theoretically these should only be non-`nullptr` for PLAIN_PATTERN, but let's clean them up either way.
        Destroy_Pigment(Layer->Pigment);
        Destroy_Tnormal(Layer->Tnormal);
        if (Layer->Finish)
            delete Layer->Finish;

        // Theoretically these should only be non-empty for BITMAP_PATTERN, but let's clean them up either way.
        for(vector<TEXTURE*>::iterator i = Layer->Materials.begin(); i != Layer->Materials.end(); ++ i)
            Destroy_Textures(*i);

        Temp = Layer->Next;
        delete Layer;
        Layer = Temp;
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
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Post_Textures(TEXTURE *Textures)
{
    TEXTURE *Layer;
    TextureBlendMap *Map;

    if (Textures == nullptr)
    {
        return;
    }

    for (Layer = Textures; Layer != nullptr; Layer = Layer->Next)
    {
        if (!((Layer->Flags) & POST_DONE))
        {
            switch (Layer->Type)
            {
                case PLAIN_PATTERN:

                    if(Layer->Tnormal)
                    {
                        Layer->Tnormal->Flags |=
                            (Layer->Flags & DONT_SCALE_BUMPS_FLAG);
                    }
                    Post_Pigment(Layer->Pigment);
                    Post_Tnormal(Layer->Tnormal);

                    break;

                case BITMAP_PATTERN:

                    for (vector<TEXTURE*>::iterator i = Layer->Materials.begin(); i != Layer->Materials.end(); ++ i)

                        Post_Textures(*i);

                    break;
            }

            if ((Map=Layer->Blend_Map.get()) != nullptr)
            {
                for(vector<TextureBlendMapEntry>::iterator i = Map->Blend_Map_Entries.begin(); i != Map->Blend_Map_Entries.end(); i++)
                {
                    i->Vals->Flags |=
                        (Layer->Flags & DONT_SCALE_BUMPS_FLAG);
                    Post_Textures(i->Vals);
                }
            }
            else
            {
                if (Layer->Type == AVERAGE_PATTERN)
                {
                    throw POV_EXCEPTION_STRING("No texture map in averaged texture.");
                }
            }
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Test_Opacity
*
* INPUT
*
*   Object - Pointer to object
*
* OUTPUT
*
* RETURNS
*
*   int - true, if opaque
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Test wether an object is opaque or not, i.e. wether the texture contains
*   a non-zero filter or alpha channel.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
*   Oct 1994 : Added code to check for opaque image maps. [DB]
*
*   Jun 1995 : Added code to check for alpha channel image maps. [DB]
*
******************************************************************************/

int Test_Opacity(const TEXTURE *Texture)
{
    int Opaque, Help;
    const TEXTURE *Layer;

    if (Texture == nullptr)
    {
        return(false);
    }

    /* We assume that the object is not opaque. */

    Opaque = false;

    /* Test all layers. If at least one layer is opaque the object is opaque. */

    for (Layer = Texture; Layer != nullptr; Layer = Layer->Next)
    {
        switch (Layer->Type)
        {
            case PLAIN_PATTERN:

                /* Test image map for opacity. */

                if (!(Layer->Pigment->Flags & HAS_FILTER))
                {
                    Opaque = true;
                }

                break;

            case BITMAP_PATTERN:

                /* Layer is not opaque if the image map is used just once. */

                if (const ImagePatternImpl* pattern = dynamic_cast<ImagePatternImpl*>(Layer->pattern.get()))
                {
                    POV_PATTERN_ASSERT(pattern->pImage);
                    if (pattern->pImage->Once_Flag)
                        break;
                }
                else
                    POV_PATTERN_ASSERT(false);

                /* Layer is opaque if all materials are opaque. */

                Help = true;

                for (vector<TEXTURE*>::const_iterator i = Layer->Materials.begin(); i != Layer->Materials.end(); ++ i)
                {
                    if (!Test_Opacity(*i))
                    {
                        /* Material is not opaque --> layer is not opaque. */

                        Help = false;

                        break;
                    }
                }

                if (Help)
                {
                    Opaque = true;
                }

                break;
        }
    }

    return(Opaque);
}


//******************************************************************************

TextureBlendMap::TextureBlendMap() : BlendMap<TexturePtr>(kBlendMapType_Texture) {}

TextureBlendMap::~TextureBlendMap()
{
    for (Vector::iterator i = Blend_Map_Entries.begin(); i != Blend_Map_Entries.end(); i++)
        Destroy_Textures(i->Vals);
}

}
// end of namespace pov
