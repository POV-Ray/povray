//******************************************************************************
///
/// @file core/material/blendmap.cpp
///
/// Implementations related to blend maps.
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
#include "core/material/blendmap.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/povassert.h"

// POV-Ray header files (core module)
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
ColourBlendMapPtr Create_Blend_Map<ColourBlendMap> (BlendMapTypeId type)
{
    POV_BLEND_MAP_ASSERT(type == kBlendMapType_Colour);
    return ColourBlendMapPtr (new ColourBlendMap);
}

template<>
PigmentBlendMapPtr Create_Blend_Map<PigmentBlendMap> (BlendMapTypeId type)
{
    POV_BLEND_MAP_ASSERT((type == kBlendMapType_Pigment) || (type == kBlendMapType_Density));
    return PigmentBlendMapPtr (new PigmentBlendMap(type));
}

template<>
SlopeBlendMapPtr Create_Blend_Map<SlopeBlendMap> (BlendMapTypeId type)
{
    POV_BLEND_MAP_ASSERT(type == kBlendMapType_Slope);
    return SlopeBlendMapPtr (new SlopeBlendMap());
}

template<>
NormalBlendMapPtr Create_Blend_Map<NormalBlendMap> (BlendMapTypeId type)
{
    POV_BLEND_MAP_ASSERT(type == kBlendMapType_Normal);
    return NormalBlendMapPtr (new NormalBlendMap());
}

template<>
TextureBlendMapPtr Create_Blend_Map<TextureBlendMap> (BlendMapTypeId type)
{
    POV_BLEND_MAP_ASSERT(type == kBlendMapType_Texture);
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
std::shared_ptr<MAP_T> Copy_Blend_Map (const std::shared_ptr<MAP_T>& Old)
{
    return std::shared_ptr<MAP_T>(Old);
}

template ColourBlendMapPtr          Copy_Blend_Map (const ColourBlendMapPtr& Old);
template PigmentBlendMapPtr         Copy_Blend_Map (const PigmentBlendMapPtr& Old);
template GenericNormalBlendMapPtr   Copy_Blend_Map (const GenericNormalBlendMapPtr& Old);
template SlopeBlendMapPtr           Copy_Blend_Map (const SlopeBlendMapPtr& Old);
template NormalBlendMapPtr          Copy_Blend_Map (const NormalBlendMapPtr& Old);
template TextureBlendMapPtr         Copy_Blend_Map (const TextureBlendMapPtr& Old);


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
BlendMap<DATA_T>::BlendMap(BlendMapTypeId type) : Type(type) {}

template BlendMap<ColourBlendMapData>::BlendMap(BlendMapTypeId type);
template BlendMap<PigmentBlendMapData>::BlendMap(BlendMapTypeId type);
template BlendMap<SlopeBlendMapData>::BlendMap(BlendMapTypeId type);
template BlendMap<NormalBlendMapData>::BlendMap(BlendMapTypeId type);
template BlendMap<TexturePtr>::BlendMap(BlendMapTypeId type);

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
// end of namespace pov
