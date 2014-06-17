//******************************************************************************
///
/// @file backend/colour/colour_old.cpp
///
/// This module implements legacy routines to manipulate colours.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2014 Persistence of Vision Raytracer Pty. Ltd.
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
//*******************************************************************************

#include <algorithm>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/colour/colour_old.h"

#include "backend/texture/texture.h"
#include "backend/texture/pigment.h"
#include "backend/texture/normal.h"
#include "backend/math/vector.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/


/*****************************************************************************
* Static functions
******************************************************************************/



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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

COLOUR *Create_Colour ()
{
    COLOUR *New;

    New = reinterpret_cast<COLOUR *>(POV_MALLOC(sizeof (COLOUR), "color"));

    Make_ColourA (*New, 0.0, 0.0, 0.0, 0.0, 0.0);

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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

COLOUR *Copy_Colour (const COLOUR Old)
{
    COLOUR *New;

    if (Old != NULL)
    {
        New = Create_Colour ();

        Assign_Colour(*New,Old);
    }
    else
    {
        New = NULL;
    }

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Blend_Map
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

template<>
ColourBlendMapPtr Create_Blend_Map<ColourBlendMap> (int type)
{
    assert (type == COLOUR_TYPE);
    return ColourBlendMapPtr (new ColourBlendMap);
}

template<>
PigmentBlendMapPtr Create_Blend_Map<PigmentBlendMap> (int type)
{
    assert ((type == PIGMENT_TYPE) || (type == DENSITY_TYPE));
    return PigmentBlendMapPtr (new PigmentBlendMap(type));
}

template<>
UnifiedNormalBlendMapPtr Create_Blend_Map<UnifiedNormalBlendMap> (int type)
{
    assert ((type == SLOPE_TYPE) || (type == NORMAL_TYPE));
    return UnifiedNormalBlendMapPtr (new UnifiedNormalBlendMap(type));
}

template<>
TextureBlendMapPtr Create_Blend_Map<TextureBlendMap> (int type)
{
    assert (type == TEXTURE_TYPE);
    return TextureBlendMapPtr (new TextureBlendMap);
}

/*****************************************************************************
*
* FUNCTION
*
*   Copy_Blend_Map
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

template<typename MAP_T>
shared_ptr<MAP_T> Copy_Blend_Map (shared_ptr<MAP_T>& Old)
{
    return shared_ptr<MAP_T>(Old);
}

template ColourBlendMapPtr          Copy_Blend_Map (ColourBlendMapPtr& Old);
template PigmentBlendMapPtr         Copy_Blend_Map (PigmentBlendMapPtr& Old);
template UnifiedNormalBlendMapPtr   Copy_Blend_Map (UnifiedNormalBlendMapPtr& Old);
template TextureBlendMapPtr         Copy_Blend_Map (TextureBlendMapPtr& Old);


/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Blend_Map
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

template<typename DATA_T>
BlendMap<DATA_T>::BlendMap(int type) : Type(type) {}

template BlendMap<ColourBlendMapData>::BlendMap(int type);
template BlendMap<PigmentBlendMapData>::BlendMap(int type);
template BlendMap<UnifiedNormalBlendMapData>::BlendMap(int type);
template BlendMap<TexturePtr>::BlendMap(int type);

template<typename DATA_T>
void BlendMap<DATA_T>::Set(const Vector& data)
{
    Blend_Map_Entries.reserve(data.size());
    Blend_Map_Entries.assign(data.begin(), data.end());
}

template void BlendMap<ColourBlendMapData>::Set(const Vector& data);
template void BlendMap<PigmentBlendMapData>::Set(const Vector& data);
template void BlendMap<UnifiedNormalBlendMapData>::Set(const Vector& data);
template void BlendMap<TexturePtr>::Set(const Vector& data);

}
