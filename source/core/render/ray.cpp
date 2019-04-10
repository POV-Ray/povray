//******************************************************************************
///
/// @file core/render/ray.cpp
///
/// Implementations related to rays.
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
#include "core/render/ray.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/material/interior.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

Ray::Ray(TraceTicket& ticket, RayType rt, bool shadowTest, bool photon, bool radiosity, bool monochromatic, bool pretrace) :
    ticket(ticket)
{
    SetFlags(rt, shadowTest, photon, radiosity, monochromatic, pretrace);
    hollowRay = true;
    ClearInteriors();
}

Ray::Ray(TraceTicket& ticket, const Vector3d& ov, const Vector3d& dv, RayType rt, bool shadowTest, bool photon, bool radiosity, bool monochromatic, bool pretrace) :
    BasicRay(ov, dv),
    ticket(ticket)
{
    SetFlags(rt, shadowTest, photon, radiosity, monochromatic, pretrace);
    hollowRay = true;
    ClearInteriors();
}

Ray::~Ray()
{
}

void Ray::AppendInterior(Interior *i)
{
    interiors.push_back(i);
    hollowRay = hollowRay && i->hollow;
}

void Ray::AppendInteriors(RayInteriorVector& ii)
{
    interiors.reserve(interiors.size() + ii.size());

    for(RayInteriorVector::iterator it(ii.begin()); it != ii.end(); it++)
    {
        interiors.push_back(*it);
        hollowRay = hollowRay && (*it)->hollow;
    }
}

bool Ray::RemoveInterior(const Interior *i)
{
    bool checkhollow = false;
    bool found = false;

    for(RayInteriorVector::iterator it(interiors.begin()); it != interiors.end(); it++)
    {
        if(*it == i)
        {
            checkhollow = ((*it)->hollow == false);
            interiors.erase(it);
            found = true;
            break;
        }
    }

    if(checkhollow == true)
    {
        hollowRay = true;

        for(RayInteriorVector::iterator it(interiors.begin()); it != interiors.end(); it++)
            hollowRay = hollowRay && (*it)->hollow;
    }

    return found;
}

bool Ray::IsInterior(const Interior *i) const
{
    for(RayInteriorVector::const_iterator it(interiors.begin()); it != interiors.end(); it++)
    {
        if(*it == i)
            return true;
    }

    return false;
}

void Ray::SetSpectralBand(const SpectralBand& sb)
{
    spectralBand = sb;
    monochromaticRay = true;
}

const SpectralBand& Ray::GetSpectralBand() const
{
    return spectralBand;
}

void Ray::SetFlags(RayType rt, bool shadowTest, bool photon, bool radiosity, bool monochromatic, bool pretrace)
{
    primaryRay = (rt == PrimaryRay);
    reflectionRay = (rt == ReflectionRay);
    refractionRay = (rt == RefractionRay);
    subsurfaceRay = (rt == SubsurfaceRay);
    shadowTestRay = shadowTest;
    photonRay = photon;
    radiosityRay = radiosity;
    monochromaticRay = monochromatic;
    pretraceRay = pretrace;
}

void Ray::SetFlags(RayType rt, const Ray& other)
{
    primaryRay = (rt == PrimaryRay);
    reflectionRay = (rt == ReflectionRay) || ((rt == RefractionRay) && other.IsReflectionRay()); // TODO FIXME - just a kludge for now! [CLi]
    refractionRay = (rt == RefractionRay) || ((rt == ReflectionRay) && other.IsRefractionRay()); // TODO FIXME - just a kludge for now! [CLi]
    subsurfaceRay = (rt == SubsurfaceRay);
    shadowTestRay = other.IsShadowTestRay();
    photonRay = other.IsPhotonRay();
    radiosityRay = other.IsRadiosityRay();
    monochromaticRay = other.IsMonochromaticRay();
    pretraceRay = other.IsPretraceRay();
}

}
// end of namespace pov
