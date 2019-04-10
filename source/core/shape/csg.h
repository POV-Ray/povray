//******************************************************************************
///
/// @file core/shape/csg.h
///
/// Declarations related to constructive solid geometry (csg) shapes.
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

#ifndef POVRAY_CORE_CSG_H
#define POVRAY_CORE_CSG_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"
#include "core/shape/csg_fwd.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/scene/object.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCoreShape
///
/// @{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

/* CSG types */

#define CSG_UNION_TYPE             1
#define CSG_INTERSECTION_TYPE      2
#define CSG_DIFFERENCE_TYPE        4
#define CSG_MERGE_TYPE             8
#define CSG_SINGLE_TYPE           16



/*****************************************************************************
* Global typedefs
******************************************************************************/

class CSG : public CompoundObject
{
    public:
        CSG(int t) : CompoundObject(t) {}
        CSG(int t, CompoundObject& o, bool transplant) : CompoundObject(t, o, transplant) {}

        int do_split;

        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override { }
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual ObjectPtr Invert() override = 0;
        virtual void Compute_BBox() override;

        virtual void Determine_Textures(Intersection *isect, bool hitinside, WeightedTextureVector& textures, TraceThreadData *Threaddata) override;
};

class CSGUnion : public CSG
{
    public:
        CSGUnion();
        CSGUnion(int t);
        CSGUnion(int t, CompoundObject& o, bool transplant) : CSG(t, o, transplant) {}

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual ObjectPtr Invert() override;
};

class CSGMerge final : public CSGUnion
{
    public:
        CSGMerge();
        CSGMerge(CompoundObject& o, bool transplant);

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
};

class CSGIntersection final : public CSG
{
    public:
        CSGIntersection(bool diff);
        CSGIntersection(bool diff, CompoundObject& o, bool transplant);

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual ObjectPtr Invert() override;

        bool isDifference;
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_CSG_H
