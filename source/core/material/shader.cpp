//******************************************************************************
///
/// @file core/material/shader.cpp
///
/// Implementations related to surface finish properties.
///
/// @copyright
/// @parblock
///
/// ----------------------------------------------------------------------------
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/material/shader.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

//******************************************************************************

double LambertDiffuse::GetBiHemisphericalReflectance() const
{
    return intensity;
}

double LambertDiffuse::operator() (const ShaderFunctionParameters& param) const
{
    if (intensity <= 0.0)
        return 0.0;

    return intensity * param.GetCosIn();
}

//******************************************************************************

/// @todo Verify that the formula is correct.
double LegacyBrillianceDiffuse::GetBiHemisphericalReflectance() const
{
    return 2.0 / (exponent + 1.0);
}

double LegacyBrillianceDiffuse::operator() (const ShaderFunctionParameters& param) const
{
    if (intensity <= 0.0)
        return 0.0;

    return intensity * pow(param.GetCosIn(), static_cast<double>(exponent));
}

//******************************************************************************

/// @todo Verify that the formula is correct.
double PhongHighlights::GetBiHemisphericalReflectance() const
{
    return 2.0 / (exponent + 1.0);
}

double PhongHighlights::operator() (const ShaderFunctionParameters& param) const
{
    if (intensity <= 0.0)
        return 0.0;

    Vector3d reflectOut = 2.0 * param.GetCosOut() * param.GetNormal() - param.GetOut();

    double cosReflectDelta = dot(reflectOut, param.GetIn());

    if (cosReflectDelta <= 0.0)
        return 0.0;

    // cut computations short if far off very focused highlight
    if ((exponent > 60) && (cosReflectDelta < 0.0008))
        return 0.0;

    return intensity * pow(cosReflectDelta, static_cast<double>(exponent));
}

//******************************************************************************

/// @todo Verify that the formula is correct.
double BlinnPhongHighlights::GetBiHemisphericalReflectance() const
{
    return ( 4.0 * (2.0 - pow(2.0, -exponent / 2.0)) ) / (exponent + 2.0);
}

double BlinnPhongHighlights::operator() (const ShaderFunctionParameters& param) const
{
    if (intensity <= 0.0)
        return 0.0;

    Vector3d halfway = param.GetHalfway() * 0.5;

    double halfwayLength = halfway.length();

    if (halfwayLength <= 0.0)
        return 0.0;

    double cosHalfway = dot(halfway, param.GetNormal()) / halfwayLength;

    if (cosHalfway <= 0.0)
        return 0.0;

    return intensity * pow(cosHalfway, static_cast<double>(exponent));
}

//******************************************************************************

FinishData::TempData::TempData() :
    caustics(-1.0), ior(-1.0), dispersion(1.0), refract(1.0)
{}

FinishData::TempData::TempData(const TempData& o) :
    caustics(o.caustics), ior(o.ior), dispersion(o.dispersion), refract(o.refract)
{}

//******************************************************************************

FinishData::FinishData() :
    tempData(),
    Ambient(0.1), Emission(0.0),
    Reflection_Max(0.0), Reflection_Min(0.0),
    SubsurfaceTranslucency(0.0), SubsurfaceAnisotropy(0.0),
    Diffuse(0.6), DiffuseBack(0.0),
    Brilliance(1.0), BrillianceOut(1.0), BrillianceAdjust(1.0), BrillianceAdjustRad(1.0),
    blinnPhongHighlights(0.0, 1.0/0.05),
    phongHighlights(0.0, 40.0),
    Reflect_Exp(1.0),
    Reflect_Metallic(0.0),
    Reflection_Falloff(1),
    Irid(0.0), Irid_Film_Thickness(0.0), Irid_Turb(0.0),
    Crand(0.0),
    Metallic(0.0),
    Reflection_Fresnel(false),
    Fresnel(false),
    Conserve_Energy(false),
    UseSubsurface(false)
{}

FinishData::FinishData (const FinishData& o) :
    tempData(o.tempData),
    Ambient(o.Ambient), Emission(o.Emission),
    Reflection_Max(o.Reflection_Max), Reflection_Min(o.Reflection_Min),
    SubsurfaceTranslucency(o.SubsurfaceTranslucency), SubsurfaceAnisotropy(o.SubsurfaceAnisotropy),
    Diffuse(o.Diffuse), DiffuseBack(o.DiffuseBack),
    Brilliance(o.Brilliance), BrillianceOut(o.BrillianceOut), BrillianceAdjust(o.BrillianceAdjust), BrillianceAdjustRad(o.BrillianceAdjustRad),
    blinnPhongHighlights(o.blinnPhongHighlights),
    phongHighlights(o.phongHighlights),
    Reflect_Exp(o.Reflect_Exp),
    Reflect_Metallic(o.Reflect_Metallic),
    Reflection_Falloff(o.Reflection_Falloff),
    Irid(o.Irid), Irid_Film_Thickness(o.Irid_Film_Thickness), Irid_Turb(o.Irid_Turb),
    Crand(o.Crand),
    Metallic(o.Metallic),
    Reflection_Fresnel(o.Reflection_Fresnel),
    Fresnel(o.Fresnel),
    Conserve_Energy(o.Conserve_Energy),
    UseSubsurface(o.UseSubsurface)
{}

FinishData::~FinishData()
{}

//******************************************************************************

}