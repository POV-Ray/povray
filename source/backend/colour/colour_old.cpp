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
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#include <algorithm>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/colour/colour_old.h"

#include "core/material/normal.h"
#include "core/material/pigment.h"
#include "core/material/texture.h"

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

RGBFTColour *Create_Colour ()
{
    return new RGBFTColour();
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

RGBFTColour *Copy_Colour (const RGBFTColour* Old)
{
    if (Old != NULL)
        return new RGBFTColour(*Old);
    else
        return NULL;
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
SlopeBlendMapPtr Create_Blend_Map<SlopeBlendMap> (int type)
{
    assert (type == SLOPE_TYPE);
    return SlopeBlendMapPtr (new SlopeBlendMap());
}

template<>
NormalBlendMapPtr Create_Blend_Map<NormalBlendMap> (int type)
{
    assert (type == NORMAL_TYPE);
    return NormalBlendMapPtr (new NormalBlendMap());
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
template GenericNormalBlendMapPtr   Copy_Blend_Map (GenericNormalBlendMapPtr& Old);
template SlopeBlendMapPtr           Copy_Blend_Map (SlopeBlendMapPtr& Old);
template NormalBlendMapPtr          Copy_Blend_Map (NormalBlendMapPtr& Old);
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
template BlendMap<SlopeBlendMapData>::BlendMap(int type);
template BlendMap<NormalBlendMapData>::BlendMap(int type);
template BlendMap<TexturePtr>::BlendMap(int type);

template<typename DATA_T>
void BlendMap<DATA_T>::Set(const Vector& data)
{
    Blend_Map_Entries.reserve(data.size());
    Blend_Map_Entries.assign(data.begin(), data.end());
}

template void BlendMap<ColourBlendMapData>::Set(const Vector& data);
template void BlendMap<PigmentBlendMapData>::Set(const Vector& data);
template void BlendMap<SlopeBlendMapData>::Set(const Vector& data);
template void BlendMap<NormalBlendMapData>::Set(const Vector& data);
template void BlendMap<TexturePtr>::Set(const Vector& data);

}
