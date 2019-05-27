//******************************************************************************
///
/// @file core/shape/gsd.h
///
/// Declarations related to generalised symmetric difference (gsd) shapes.
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

#ifndef POVRAY_CORE_GSD_H
#define POVRAY_CORE_GSD_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

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

/* GSD types */

enum GSD_TYPE{
    GSD_INTERUNION_TYPE ,
    GSD_INTERMERGE_TYPE
};




/*****************************************************************************
* Global typedefs
******************************************************************************/

class GSDInterUnion : public CompoundObject
{
    public:
        GSDInterUnion();

        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override { }
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual ObjectPtr Invert() override;
        virtual void Compute_BBox() override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;

        virtual void Determine_Textures(Intersection *isect, bool hitinside, WeightedTextureVector& textures, TraceThreadData *Threaddata) override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        /// prepare to select, once all objects have been added
        /// @parm[in] c number of objects
        void prepare(unsigned c);
        /// define a number of exactly overlapping objects as being inside
        /// @parm[in] v number of overlapping objects for which inside should be true
        void setAsInside(unsigned v);
    protected:
        std::vector<bool> selected;
};

class GSDInterMerge final: public GSDInterUnion
{
    public:
        GSDInterMerge();
        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_GSD_H
