//******************************************************************************
///
/// @file core/material/normal.h
///
/// Declarations related to surface normal perturbations.
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

#ifndef POVRAY_CORE_NORMAL_H
#define POVRAY_CORE_NORMAL_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/coretypes.h"
#include "core/material/blendmap.h"
#include "core/math/vector.h"

namespace pov
{

class Intersection;
class Ray;
class TraceThreadData;

typedef Vector2d    SlopeBlendMapData;
typedef TNORMAL*    NormalBlendMapData;

/// Common interface for normal-like blend maps.
///
/// This purely abstract class provides the common interface for both normal and slope blend maps.
///
/// @note   This class is used in a multiple inheritance hierarchy, and therefore must continue to be purely abstract.
///
class GenericNormalBlendMap
{
    public:

        virtual ~GenericNormalBlendMap() {}

        virtual void Post(bool dontScaleBumps) = 0;
        virtual void ComputeAverage (const Vector3d& EPoint, Vector3d& normal, Intersection *Inter, const Ray *ray, TraceThreadData *Thread) = 0;
};

class SlopeBlendMap : public BlendMap<SlopeBlendMapData>, public GenericNormalBlendMap
{
    public:

        SlopeBlendMap();
        virtual ~SlopeBlendMap();

        virtual void Post(bool dontScaleBumps);
        virtual void ComputeAverage (const Vector3d& EPoint, Vector3d& normal, Intersection *Inter, const Ray *ray, TraceThreadData *Thread);
};

class NormalBlendMap : public BlendMap<NormalBlendMapData>, public GenericNormalBlendMap
{
    public:

        NormalBlendMap();
        virtual ~NormalBlendMap();

        virtual void Post(bool dontScaleBumps);
        virtual void ComputeAverage (const Vector3d& EPoint, Vector3d& normal, Intersection *Inter, const Ray *ray, TraceThreadData *Thread);
};

typedef shared_ptr<GenericNormalBlendMap>           GenericNormalBlendMapPtr;
typedef shared_ptr<const GenericNormalBlendMap>     GenericNormalBlendMapConstPtr;

typedef BlendMapEntry<SlopeBlendMapData>            SlopeBlendMapEntry;
typedef shared_ptr<SlopeBlendMap>                   SlopeBlendMapPtr;
typedef shared_ptr<const SlopeBlendMap>             SlopeBlendMapConstPtr;

typedef BlendMapEntry<NormalBlendMapData>           NormalBlendMapEntry;
typedef shared_ptr<NormalBlendMap>                  NormalBlendMapPtr;
typedef shared_ptr<const NormalBlendMap>            NormalBlendMapConstPtr;

struct Tnormal_Struct : public Pattern_Struct
{
    GenericNormalBlendMapPtr Blend_Map;
    SNGL Amount;
    SNGL Delta; // NK delta
};


TNORMAL *Create_Tnormal ();
TNORMAL *Copy_Tnormal (TNORMAL *Old);
void Destroy_Tnormal (TNORMAL *Tnormal);
void Post_Tnormal (TNORMAL *Tnormal);
void Perturb_Normal (Vector3d& Layer_Normal, const TNORMAL *Tnormal, const Vector3d& IPoint, Intersection *Intersection, const Ray *ray, TraceThreadData *Thread);

}

#endif // POVRAY_CORE_NORMAL_H
