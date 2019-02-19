//******************************************************************************
///
/// @file core/render/ray.h
///
/// Declarations related to rays.
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

#ifndef POVRAY_CORE_RAY_H
#define POVRAY_CORE_RAY_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"
#include "core/render/ray_fwd.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/core_fwd.h"
#include "core/bounding/boundingbox.h"
#include "core/colour/spectral.h"
#include "core/support/simplevector.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCoreRender
///
/// @{

typedef PooledSimpleVector<Interior *, RAYINTERIOR_VECTOR_SIZE> RayInteriorVector;

class Ray final : public BasicRay
{
    public:

        enum RayType
        {
            OtherRay = 0,
            PrimaryRay = 1,
            ReflectionRay = 2,
            RefractionRay = 3,
            SubsurfaceRay = 4,  ///< Ray is shot from just below a surface; very close intersections shall not be suppressed.
        };

        Ray(TraceTicket& ticket, RayType rt = PrimaryRay, bool shadowTest = false, bool photon = false, bool radiosity = false, bool monochromatic = false, bool pretrace = false);
        Ray(TraceTicket& ticket, const Vector3d& ov, const Vector3d& dv, RayType rt = PrimaryRay, bool shadowTest = false, bool photon = false, bool radiosity = false, bool monochromatic = false, bool pretrace = false);
        ~Ray();

        void AppendInterior(Interior *i);
        void AppendInteriors(RayInteriorVector&);
        bool RemoveInterior(const Interior *i);
        void ClearInteriors() { interiors.clear(); }

        bool IsInterior(const Interior *i) const;
        const RayInteriorVector& GetInteriors() const { return interiors; }
        RayInteriorVector& GetInteriors() { return interiors; }

        void SetSpectralBand(const SpectralBand&);
        const SpectralBand& GetSpectralBand() const;

        void SetFlags(RayType rt, bool shadowTest = false, bool photon = false, bool radiosity = false, bool monochromatic = false, bool pretrace = false);
        void SetFlags(RayType rt, const Ray& other);

        bool IsPrimaryRay() const { return primaryRay; }
        bool IsImageRay() const { return primaryRay || (refractionRay && !reflectionRay && !radiosityRay); }
        bool IsReflectionRay() const { return reflectionRay; }
        bool IsRefractionRay() const { return refractionRay; }
        bool IsSubsurfaceRay() const { return subsurfaceRay; }
        bool IsShadowTestRay() const { return shadowTestRay; }
        bool IsPhotonRay() const { return photonRay; }
        bool IsRadiosityRay() const { return radiosityRay; }
        bool IsMonochromaticRay() const { return monochromaticRay; }
        bool IsHollowRay() const { return hollowRay; }
        bool IsPretraceRay() const { return pretraceRay; }

        bool Inside(const BoundingBox& bbox) const { return Inside_BBox(Origin, bbox); }

        inline TraceTicket& GetTicket() { return ticket; }
        inline const TraceTicket& GetTicket() const { return ticket; }

    private:

        RayInteriorVector interiors;
        SpectralBand spectralBand;
        TraceTicket& ticket;

        bool primaryRay : 1;
        bool reflectionRay : 1;
        bool refractionRay : 1;
        bool subsurfaceRay : 1;
        bool shadowTestRay : 1;
        bool photonRay : 1;
        bool radiosityRay : 1;
        bool monochromaticRay : 1;
        bool hollowRay : 1;
        bool pretraceRay : 1;
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_RAY_H
