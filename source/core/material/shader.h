//******************************************************************************
///
/// @file core/material/shader.h
///
/// Declarations related to surface finish properties.
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

#ifndef POVRAY_CORE_SHADER_H
#define POVRAY_CORE_SHADER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/coretypes.h"

namespace pov
{

//******************************************************************************
///
/// @name Shader Interface.
///
/// @{

/// Class holding parameters and precomputed data for shader functions.
///
/// This class encapsulates the set of parameters passed to shader function objects. In addition, it
/// provides a mechanism to cache some interim results typically computed from those parameters, in
/// order to avoid duplicate computations of those values in case a texture makes use of multiple
/// shaders.
///
class ShaderFunctionParameters
{
public:

    ShaderFunctionParameters() :
        mIn(0.0), mOut(0.0), mNormal(0.0),
        mHalfway(0.0), mReflectedOut(0.0), mCosIn(0.0), mCosOut(0.0),
        mHaveHalfway(false), mHaveReflectedOut(false), mHaveCosIn(false), mHaveCosOut(false)
    {}

    /// @param[in]  in          Vector to light source.
    /// @param[in]  out         Vector to observer.
    ShaderFunctionParameters (const Vector3d& in, const Vector3d& out) :
        mIn(in), mOut(out), mNormal(0.0),
        mHalfway(0.0), mReflectedOut(0.0), mCosIn(0.0), mCosOut(0.0),
        mHaveHalfway(false), mHaveReflectedOut(false), mHaveCosIn(false), mHaveCosOut(false)
    {}

    /// @param[in]  in          Vector to light source.
    /// @param[in]  out         Vector to observer.
    /// @param[in]  normal      Perturbed surface normal vector.
    ShaderFunctionParameters (const Vector3d& in, const Vector3d& out, const Vector3d& normal) :
        mIn(in), mOut(out), mNormal(normal),
        mHalfway(0.0), mReflectedOut(0.0), mCosIn(0.0), mCosOut(0.0),
        mHaveHalfway(false), mHaveReflectedOut(false), mHaveCosIn(false), mHaveCosOut(false)
    {}

    //******************************************************************************
    ///
    /// @name Set Parameters After Construction
    ///
    /// The following methods are provided to allow partial construction, setting one or more of
    /// the parameters later. They can also be used to change parameter values, in which case any
    /// precomputed data based on them will be discarded.
    ///
    /// @{

    /// Set vector to light source.
    /// @note The vector is expected to be normalized.
    inline void SetIn (const Vector3d& in)
    {
        // set data
        mIn = in;
        // invalidate any dependent precomputed data
        mHaveHalfway = mHaveReflectedOut = mHaveCosIn = false;
    }

    /// Set vector to observer.
    /// @note The vector is expected to be normalized.
    inline void SetOut (const Vector3d& out)
    {
        // set data
        mOut = out;
        // invalidate any dependent precomputed data
        mHaveHalfway = mHaveCosOut = false;
    }

    /// Set perturbed surface normal vector.
    /// @note The vector is expected to be normalized.
    inline void SetNormal (const Vector3d& normal)
    {
        // set data
        mNormal = normal;
        // invalidate any dependent precomputed data
        mHaveReflectedOut = mHaveCosIn = mHaveCosOut = false;
    }

    /// Set vector to light source equal to the reflected vector to observer.
    void SetInByReflection()
    {
        // compute interim results
        mCosIn = GetCosOut();
        mHalfway = mCosIn * mNormal;
        // compute primary data
        mIn = mReflectedOut = 2.0 * mHalfway - mOut;
        // remember what we've precomputed
        mHaveCosIn = mHaveHalfway = mHaveReflectedOut = true;
    }

    /// @}
    ///
    //******************************************************************************
    ///
    /// @name Query Parameters and Precomputed Data.
    ///
    /// The following methods are provided to query the stored parameters.
    ///
    /// @{

    /// Get vector to light source.
    const Vector3d& GetIn() const { return mIn; }

    /// Get vector to observer.
    const Vector3d& GetOut() const { return mOut; }

    /// Get perturbed surface normal vector.
    const Vector3d& GetNormal() const { return mNormal; }

    /// @}
    ///
    //******************************************************************************
    ///
    /// @name Compute or Retrieve Interim Results.
    ///
    /// The following methods compute and cache common interim results from the parameters, or
    /// retrieve the cached results if already available.
    ///
    /// @{

    /// Get average of vectors to light source and observer.
    /// @note The returned vector is _not_ normalized.
    const Vector3d& GetHalfway() const
    {
        // compute if necessary
        if (!mHaveHalfway) { mHalfway = 0.5 * (mIn + mOut); mHaveHalfway = true; }
        // return computed data
        return mHalfway;
    }

    /// Get vector to observer reflected about the normal vector.
    const Vector3d& GetReflectedOut() const
    {
        // compute if necessary
        if (!mHaveReflectedOut) { mReflectedOut = (2.0 * GetCosOut()) * mNormal - mIn; mHaveReflectedOut = true; }
        // return computed data
        return mReflectedOut;
    }

    /// Get cosine of angle between light source and surface normal.
    double GetCosIn() const
    {
        // compute if necessary
        if (!mHaveCosIn) { mCosIn = dot(mNormal, mIn); mHaveCosIn = true; }
        // return computed data
        return mCosIn;
    }

    /// Get cosine of angle between light source and surface normal.
    double GetCosOut() const
    {
        // compute if necessary
        if (!mHaveCosOut) { mCosOut = dot(mNormal, mOut); mHaveCosOut = true; }
        // return computed data
        return mCosOut;
    }

    /// @}
    ///
    //******************************************************************************

private:
    Vector3d            mIn;
    Vector3d            mOut;
    Vector3d            mNormal;
    mutable Vector3d    mReflectedOut;
    mutable Vector3d    mHalfway;
    mutable double      mCosIn;
    mutable double      mCosOut;
    mutable bool        mHaveHalfway        : 1;
    mutable bool        mHaveReflectedOut   : 1;
    mutable bool        mHaveCosIn          : 1;
    mutable bool        mHaveCosOut         : 1;
};

/// Shader Category.
///
/// @todo
///     The current code does not yet make use of these categories.
///
enum ShaderCategory
{
    /// Shader models diffuse reflection.
    ///
    /// If a shader reports as diffuse, calling algorithms will invoke @ref ShaderFunction::GetNominal() or
    /// @ref ShaderFunction::GetEffective(), and apply light and pigment colour to the result.
    ///
    kShaderCategory_Diffuse,

    /// Shader models specular highlights.
    ///
    /// If a shader reports as specular, calling algorithms will invoke @ref ShaderFunction::GetNominal() or
    /// @ref ShaderFunction::GetEffective(), and apply light colour to the result, but not pigment colour. (Exception:
    /// If the general finish settings mark the texture as metallic, pigment colour will also be applied.)
    ///
    kShaderCategory_Specular,
};

/// Abstract base class for shader functions.
///
/// @note
///     At present, this class and the mechanism around it is designed only for uniform (i.e.
///     unpatterned) BRDF-ish shaders, the result of which only depends on the surface normal,
///     ingoing and outgoing light directions, and a set of fixed shader-specific parameters.
///
class ShaderFunction
{
public:

    virtual ~ShaderFunction() {}
    virtual ShaderFunction* Clone() const = 0;

    /// Report what property of a finish the shader is intended to model.
    virtual ShaderCategory GetCategory() const = 0;

    /// Report whether the model is reciprocal.
    ///
    /// This method shall report whether the shader model obeys Helmholtz reciprocity, i.e. the
    /// light source and observer directions can be swapped without affecting the result.
    ///
    virtual bool IsReciprocal() const = 0;

    /// Report effective bihemispherical reflectance.
    ///
    /// This method shall report the shader model's effective bimehispherical reflectance, i.e. the
    /// ratio of outgoing vs. incoming light, both integrated over the entire hemisphere, under the
    /// presumption that incoming light intensity is uniform in all direction.
    ///
    /// @note
    ///     This method should not be called by time-critical code, as it may be implemented by
    ///     numerical integration.
    ///
    virtual double GetBiHemisphericalReflectance () const = 0;

    /// Compute function result.
    ///
    /// This method shall compute the shader function result for the given set of parameters
    /// in typical shader implementation style, i.e. _premultiplied_ by the cosine of the angle between
    /// the incoming light direction and the surface normal.
    ///
    /// This method is called whenever the calling algorithm does not account for the cosine
    /// factor.
    ///
    /// @param[in]  param   Input parameters.
    ///
    virtual double operator() (const ShaderFunctionParameters& param) const = 0;

protected:

    /// Convenience function to easily implement @ref GetBiHemisphericalReflectance() based on
    /// numerical integration.
    ///
    inline double SampleBiHemisphericalReflectance() const;
};

/// @}
///
//******************************************************************************
///
/// @name Shader Implementations.
///
/// @{

/// Classic Lambertian diffuse model
///
class LambertDiffuse : public ShaderFunction
{
public:

    float   intensity;

    LambertDiffuse (float in) : intensity(in) {}
    virtual LambertDiffuse* Clone() const { return new LambertDiffuse (*this); }
    virtual ShaderCategory GetCategory() const { return kShaderCategory_Diffuse; }
    virtual bool IsReciprocal() const { return true; }
    virtual double GetBiHemisphericalReflectance() const;
    virtual double operator() (const ShaderFunctionParameters& param) const;
};

/// Legacy `brilliance` diffuse model
///
class LegacyBrillianceDiffuse : public ShaderFunction
{
public:

    float   intensity;
    float   exponent;

    LegacyBrillianceDiffuse (float in, float ex) : intensity(in), exponent(ex) {}
    virtual LegacyBrillianceDiffuse* Clone() const { return new LegacyBrillianceDiffuse (*this); }
    virtual ShaderCategory GetCategory() const { return kShaderCategory_Diffuse; }
    virtual bool IsReciprocal() const { return (exponent == 1.0); }
    virtual double GetBiHemisphericalReflectance() const;
    virtual double operator() (const ShaderFunctionParameters& param) const;
};

/// Classic Phong specular highlights model
///
/// @remark
///     The model used is _not_ energy-conserving.
///
class PhongHighlights : public ShaderFunction
{
public:

    float   intensity;
    float   exponent;

    PhongHighlights (float in, float ex) : intensity(in), exponent(ex) {}
    virtual PhongHighlights* Clone() const { return new PhongHighlights (*this); }
    virtual ShaderCategory GetCategory() const { return kShaderCategory_Specular; }
    virtual bool IsReciprocal() const { return false; }
    virtual double GetBiHemisphericalReflectance() const;
    virtual double operator() (const ShaderFunctionParameters& param) const;
};

/// Classic Blinn-Phong specular highlights model
///
/// @remark
///     The model used is _not_ energy-conserving.
///
/// @todo
///     [CLi] I think the bihemispherical reflectance is wrong.
///
class BlinnPhongHighlights : public ShaderFunction
{
public:

    float   intensity;
    float   exponent;

    BlinnPhongHighlights (float in, float ex) : intensity(in), exponent(ex) {}
    virtual BlinnPhongHighlights* Clone() const { return new BlinnPhongHighlights (*this); }
    virtual ShaderCategory GetCategory() const { return kShaderCategory_Specular; }
    virtual bool IsReciprocal() const { return false; }
    virtual double GetBiHemisphericalReflectance() const;
    virtual double operator() (const ShaderFunctionParameters& param) const;
};

/// @}
///
//******************************************************************************


/// Surface Finish Properties.
struct FinishData : public CowBase<FinishData>
{
    /// Temporary data only needed during parsing of legacy scenes.
    struct TempData
    {
        SNGL caustics, ior, dispersion, refract;

        TempData();
        TempData (const TempData& o);
    };
    typedef OptionalDataPtr<TempData> TempDataPtr;

    TempDataPtr tempData;

    MathColour Ambient, Emission;
    MathColour Reflection_Max, Reflection_Min;
    MathColour SubsurfaceTranslucency, SubsurfaceAnisotropy;
    //MathColour SigmaPrimeS, SigmaA;
    SNGL Diffuse, DiffuseBack, Brilliance, BrillianceOut, BrillianceAdjust, BrillianceAdjustRad;

    PhongHighlights         phongHighlights;
    BlinnPhongHighlights    blinnPhongHighlights;
    //LegacyBrillianceDiffuse diffuse;
    //LegacyBrillianceDiffuse backsideDiffuse;

    SNGL Reflect_Exp;
    SNGL Reflect_Metallic;
    SNGL Reflection_Falloff;
    SNGL Irid, Irid_Film_Thickness, Irid_Turb;
    SNGL Crand;
    SNGL Metallic;
    bool Reflection_Fresnel : 1;
    bool Fresnel            : 1;
    bool Conserve_Energy    : 1;    // added by NK Dec 19 1999
    bool UseSubsurface      : 1;    // whether to use subsurface light transport

    FinishData();
    FinishData (const FinishData& o);
    virtual ~FinishData();
    virtual FinishData* Clone() const { return new FinishData(*this); }
};

}

#endif // POVRAY_CORE_SHADER_H
