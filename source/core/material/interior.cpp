//******************************************************************************
///
/// @file core/material/interior.cpp
///
/// Implementations related to interior.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/material/interior.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <vector>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/lighting/subsurface.h"
#include "core/material/texture.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

Interior::Interior()
{
    IOR = 0.0;
    Old_Refract = 1.0;

    Dispersion  = 1.0;
    Disp_NElems = DISPERSION_ELEMENTS_DEFAULT;

    Caustics = 0.0;

    Fade_Distance = 0.0;
    Fade_Power    = 0.0;

    hollow = false;

    subsurface = std::shared_ptr<SubsurfaceInterior>();
}

Interior::Interior(const Interior& source)
{
    Disp_NElems = source.Disp_NElems;
    Dispersion = source.Dispersion;
    Old_Refract = source.Old_Refract;
    Fade_Distance = source.Fade_Distance;
    Fade_Power = source.Fade_Power;
    Fade_Colour = source.Fade_Colour;
    media = source.media;
    hollow = source.hollow;
    IOR = source.IOR;
    subsurface = std::shared_ptr<SubsurfaceInterior>(source.subsurface);
    Caustics = source.Caustics;
}

Interior::~Interior()
{
}

void Interior::Transform(const TRANSFORM *trans)
{
    for(std::vector<Media>::iterator i(media.begin());i != media.end(); i++)
        i->Transform(trans);
}

void Interior::PostProcess()
{
    for(std::vector<Media>::iterator i(media.begin());i != media.end(); i++)
        i->PostProcess();
}

/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Interior
*
* INPUT
*
*   Interior - interior to destroy
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
*   Destroy an interior.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Destroy_Interior(InteriorPtr& interior)
{
    interior.reset();
}

/*****************************************************************************
*
* FUNCTION
*
*   Copy_Interior_Pointer
*
* INPUT
*
*   Old - interior to copy
*
* OUTPUT
*
* RETURNS
*
*   INTERIOR * - new interior
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy an interior by increasing number of references.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

MATERIAL *Create_Material()
{
    MATERIAL *New;

    New = new MATERIAL;

    New->Texture  = nullptr;
    New->Interior_Texture  = nullptr;
    New->interior.reset();

    return(New);
}

MATERIAL *Copy_Material(const MATERIAL *Old)
{
    MATERIAL *New;

    if (Old != nullptr)
    {
        New = Create_Material();

        *New = *Old;

        New->Texture  = Copy_Textures(Old->Texture);
        New->Interior_Texture  = Copy_Textures(Old->Interior_Texture);
        if (Old->interior != nullptr)
            New->interior = InteriorPtr(new Interior(*(Old->interior)));

        return(New);
    }
    else
    {
        return nullptr;
    }
}

void Destroy_Material(MATERIAL *Material)
{
    if (Material != nullptr)
    {
        Destroy_Textures(Material->Texture);
        Destroy_Textures(Material->Interior_Texture);

        delete Material;
    }
}

}
// end of namespace pov
