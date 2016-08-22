//******************************************************************************
///
/// @file core/material/pigment.h
///
/// Declarations related to pigments.
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

#ifndef POVRAY_CORE_PIGMENT_H
#define POVRAY_CORE_PIGMENT_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "base/image/colourspace.h"

#include "core/coretypes.h"
#include "core/material/blendmap.h"

namespace pov
{

class Intersection;
class Ray;
class TraceThreadData;

typedef TransColour ColourBlendMapData;
typedef PIGMENT*    PigmentBlendMapData;

/// Common interface for pigment-like blend maps.
///
/// This class provides the common interface for both pigment and colour blend maps.
///
class GenericPigmentBlendMap
{
    public:

        int                     blendMode;
        pov_base::GammaCurvePtr blendGamma;

        GenericPigmentBlendMap() : blendMode(0), blendGamma() {}
        virtual ~GenericPigmentBlendMap() {}

        virtual bool Compute(TransColour& colour, DBL value, const Vector3d& IPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread) = 0;
        virtual void ComputeAverage(TransColour& colour, const Vector3d& EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread) = 0;
        virtual bool ComputeUVMapped(TransColour& colour, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread) = 0;
        virtual void ConvertFilterToTransmit() = 0; ///< @deprecated Only used for backward compatibility with version 3.10 or earlier.
        virtual void Post(bool& rHasFilter) = 0;

        void Blend(TransColour& result, const TransColour& colour1, DBL weight1, const TransColour& colour2, DBL weight2, TraceThreadData *thread);
};

/// Colour blend map.
class ColourBlendMap : public BlendMap<ColourBlendMapData>, public GenericPigmentBlendMap
{
    public:

        ColourBlendMap();
        ColourBlendMap(int n, const Entry aEntries[]);

        virtual bool Compute(TransColour& colour, DBL value, const Vector3d& IPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);
        virtual void ComputeAverage(TransColour& colour, const Vector3d& EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);
        virtual bool ComputeUVMapped(TransColour& colour, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);
        virtual void ConvertFilterToTransmit(); ///< @deprecated Only used for backward compatibility with version 3.10 or earlier.
        virtual void Post(bool& rHasFilter);
};

/// Pigment blend map.
class PigmentBlendMap : public BlendMap<PigmentBlendMapData>, public GenericPigmentBlendMap
{
    public:

        PigmentBlendMap(BlendMapTypeId type);
        virtual ~PigmentBlendMap();

        virtual bool Compute(TransColour& colour, DBL value, const Vector3d& IPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);
        virtual void ComputeAverage(TransColour& colour, const Vector3d& EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);
        virtual bool ComputeUVMapped(TransColour& colour, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);
        virtual void ConvertFilterToTransmit(); ///< @deprecated Only used for backward compatibility with version 3.10 or earlier.
        virtual void Post(bool& rHasFilter);
};

typedef shared_ptr<GenericPigmentBlendMap>          GenericPigmentBlendMapPtr;
typedef shared_ptr<const GenericPigmentBlendMap>    GenericPigmentBlendMapConstPtr;

typedef BlendMapEntry<PigmentBlendMapData>          PigmentBlendMapEntry;
typedef shared_ptr<PigmentBlendMap>                 PigmentBlendMapPtr;
typedef shared_ptr<const PigmentBlendMap>           PigmentBlendMapConstPtr;

typedef BlendMapEntry<ColourBlendMapData>           ColourBlendMapEntry;
typedef shared_ptr<ColourBlendMap>                  ColourBlendMapPtr;
typedef shared_ptr<const ColourBlendMap>            ColourBlendMapConstPtr;

struct Pigment_Struct : public Pattern_Struct
{
    shared_ptr<GenericPigmentBlendMap> Blend_Map;
    TransColour colour;       // may have a filter/transmit component
    TransColour Quick_Colour; // may have a filter/transmit component    // TODO - can't we decide between regular colour and quick_colour at parse time already?
};


PIGMENT *Create_Pigment();
PIGMENT *Copy_Pigment(PIGMENT *Old);
void Copy_Pigments (vector<PIGMENT*>& New, const vector<PIGMENT*>& Old);
void Destroy_Pigment(PIGMENT *Pigment);
void Post_Pigment(PIGMENT *Pigment, bool* pHasFilter = NULL);
bool Compute_Pigment(TransColour& colour, const PIGMENT *Pigment, const Vector3d& IPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);
void Evaluate_Density_Pigment(vector<PIGMENT*>& Density, const Vector3d& p, AttenuatingColour& c, TraceThreadData *ttd);

}

#endif // POVRAY_CORE_PIGMENT_H
