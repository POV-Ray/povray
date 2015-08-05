//******************************************************************************
///
/// @file core/material/blendmap.h
///
/// This module contains all defines, typedefs, and prototypes for blend maps.
///
/// @note   `frame.h` contains other colour stuff.
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

#ifndef POVRAY_CORE_BLENDMAP_H
#define POVRAY_CORE_BLENDMAP_H

namespace pov
{

enum BlendMapTypeId
{
    kBlendMapType_Pigment = 0,
    kBlendMapType_Normal  = 1,
    // kBlendMapType_Pattern = 2,
    // TODO - where's type 3?
    kBlendMapType_Texture = 4,
    kBlendMapType_Colour  = 5,
    kBlendMapType_Slope   = 6,
    kBlendMapType_Density = 7
};

template<typename DATA_T>
struct BlendMapEntry
{
    SNGL    value;
    DATA_T  Vals;
};

/// Template for blend maps classes.
template<typename DATA_T>
class BlendMap
{
    public:

        typedef DATA_T                  Data;
        typedef BlendMapEntry<DATA_T>   Entry;
        typedef Entry*                  EntryPtr;
        typedef const Entry*            EntryConstPtr;
        typedef vector<Entry>           Vector;

        BlendMap(BlendMapTypeId type);
        virtual ~BlendMap() {}

        void Set(const Vector& data);
        void Search(DBL value, EntryConstPtr& rpPrev, EntryConstPtr& rpNext, DBL& rPrevWeight, DBL& rNextWeight) const;

    // protected:

        BlendMapTypeId  Type;
        Vector          Blend_Map_Entries;
};

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/


/*****************************************************************************
* Global typedefs
******************************************************************************/


/*****************************************************************************
* Global variables
******************************************************************************/


/*****************************************************************************
* Global functions
******************************************************************************/

template<typename MAP_T>
shared_ptr<MAP_T> Create_Blend_Map (BlendMapTypeId type);

template<typename MAP_T>
shared_ptr<MAP_T> Copy_Blend_Map (shared_ptr<MAP_T>& Old);


/*****************************************************************************
* Inline functions
******************************************************************************/

}

#endif // POVRAY_CORE_BLENDMAP_H
