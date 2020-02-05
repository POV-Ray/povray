//******************************************************************************
///
/// @file core/shape/ovus.h
///
/// Declarations related to the ovus geometric primitive.
///
/// @author Jerome Grimbert
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

#ifndef POVRAY_CORE_OVUS_H
#define POVRAY_CORE_OVUS_H

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

//******************************************************************************
///
/// @name Object Types
///
/// @{

#define OVUS_OBJECT (STURM_OK_OBJECT)

/// @}
///
//******************************************************************************

class Ovus final : public ObjectBase
{
    public:

        Ovus();
        virtual ~Ovus() override;

        virtual ObjectPtr Copy() override;

        virtual bool All_Intersections(const Ray&, IStack&, TraceThreadData *) override;
        virtual bool Inside(const Vector3d&, TraceThreadData *) const override;
        virtual void Normal(Vector3d&, Intersection *, TraceThreadData *) const override;
        virtual void UVCoord(Vector2d&, const Intersection *) const override;
        virtual void Translate(const Vector3d&, const TRANSFORM *) override;
        virtual void Rotate(const Vector3d&, const TRANSFORM *) override;
        virtual void Scale(const Vector3d&, const TRANSFORM *) override;
        virtual void Transform(const TRANSFORM *) override;
        virtual void Compute_BBox() override;

        /// radius of bottom sphere (provided in SDL)
        DBL BottomRadius;
        /// radius of top sphere (provided in SDL)
        DBL TopRadius;

        /// horizontal position of center of connecting surface (computed)
        DBL HorizontalPosition;
        /// vertical position of center of connecting surface (computed)
        DBL VerticalPosition;
        /// lowest vertical for the connecting surface (computed)
        DBL BottomVertical;
        /// highest vertical for the connecting surface (computed)
        DBL TopVertical;
        /// Radius of the connecting surface (computed, or provided in SDL)
        DBL ConnectingRadius;
        /// vertical position of center of top sphere (computed or provided in SDL)
        DBL VerticalSpherePosition;
        /// precision for root solver
        DBL RootTolerance;

    private:
        void CalcUV(const Vector3d& IPoint, Vector2d& Result) const;
        void Intersect_Ovus_Spheres(const Vector3d&, const Vector3d&,
                                    DBL * Depth1,DBL *Depth2, DBL * Depth3,
                                    DBL * Depth4, DBL * Depth5, DBL * Depth6,
                                    RenderStatistics& stats) const;

};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_OVUS_H
