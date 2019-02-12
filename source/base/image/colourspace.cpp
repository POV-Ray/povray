//******************************************************************************
///
/// @file base/image/colourspace.cpp
///
/// Implementation of colour space conversions.
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
#include "base/image/colourspace.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>

// POV-Ray header files (base module)
#include "base/povassert.h"
#include "base/image/encoding.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

using std::min;
using std::max;

// definitions of static GammaCurve member variables to satisfy the linker
std::list<std::weak_ptr<GammaCurve>> GammaCurve::cache;
std::mutex GammaCurve::cacheMutex;

// definitions of static GammaCurve-derivatives' member variables to satisfy the linker
SimpleGammaCurvePtr NeutralGammaCurve::instance;
SimpleGammaCurvePtr SRGBGammaCurve::instance;
SimpleGammaCurvePtr BT709GammaCurve::instance;
SimpleGammaCurvePtr BT1361GammaCurve::instance;
SimpleGammaCurvePtr BT2020GammaCurve::instance;

/*******************************************************************************/

float* GammaCurve::GetLookupTable(unsigned int max)
{
    POV_COLOURSPACE_ASSERT(max == 255 || max == 65535); // shouldn't happen, but it won't hurt to check in debug versions

    // Get a reference to the lookup table pointer we're dealing with, so we don't need to duplicate all the remaining code.
    float*& lookupTable = (max == 255 ? lookupTable8 : lookupTable16);

    // Make sure we're not racing any other thread that might currently be busy creating the LUT.
    std::lock_guard<std::mutex> lock(lutMutex);

    // Create the LUT if it doesn't exist yet.
    if (!lookupTable)
    {
        float* tempTable = new float[max+1];
        for (unsigned int i = 0; i <= max; i ++)
            tempTable[i] = Decode(IntDecode(i, max));

        // hook up the table only as soon as it is completed, so that querying the table does not need to
        // care about thread-safety.
        lookupTable = tempTable;
    }

    return lookupTable;
}

GammaCurvePtr GammaCurve::GetMatching(const GammaCurvePtr& newInstance)
{
    GammaCurvePtr oldInstance;
    bool cached = false;

    // See if we have a matching gamma curve in our cache already

    // make sure the cache doesn't get tampered with while we're working on it
    std::lock_guard<std::mutex> lock(cacheMutex);

    // Check if we already have created a matching gamma curve object; if so, return that object instead.
    // Also, make sure we get the new object stored (as we're using weak pointers, we may have stale entries;
    // it also won't hurt if we store the new instance, even if we decide to discard it)
    for(std::list<std::weak_ptr<GammaCurve>>::iterator i(cache.begin()); i != cache.end(); i++)
    {
        oldInstance = (*i).lock();
        if (!oldInstance)
        {
            // Found a stale entry in the cache where we could store the new instance, in case we don't find any match.
            // As the cache uses weak pointers, we can just as well store the new instance now right away,
            // and leave it up to the weak pointer mechanism to clean up in case we find an existing instance.
            if (!cached)
                (*i) = newInstance;
            cached = true;
        }
        else if (oldInstance->Matches(newInstance))
        {
            // Found a matching curve in the cache, so use that instead, and (as far as we're concerned)
            // just forget that the new instance ever existed (allowing the shared_ptr mechanism to garbage-collect it)
            return oldInstance;
        }
    }

    // No matching gamma curve in the cache yet

    // Store the new entry in the cache if we haven't done so already.
    if (!cached)
        cache.push_back(newInstance);

    return newInstance;
}

/*******************************************************************************/

NeutralGammaCurve::NeutralGammaCurve() {}
SimpleGammaCurvePtr NeutralGammaCurve::Get()
{
    if (!instance)
        instance.reset(new NeutralGammaCurve());
    return SimpleGammaCurvePtr(instance);
}
float NeutralGammaCurve::Encode(float x) const
{
    return x;
}
float NeutralGammaCurve::Decode(float x) const
{
    return x;
}
float NeutralGammaCurve::ApproximateDecodingGamma() const
{
    return 1.0f;
}
int NeutralGammaCurve::GetTypeId() const
{
    return kPOVList_GammaType_Neutral;
}
bool NeutralGammaCurve::Matches(const GammaCurvePtr& p) const
{
    return GammaCurve::IsNeutral(p);
}
bool NeutralGammaCurve::IsNeutral() const
{
    return true;
}

/*******************************************************************************/

SRGBGammaCurve::SRGBGammaCurve() {}
SimpleGammaCurvePtr SRGBGammaCurve::Get()
{
    if (!instance)
        instance.reset(new SRGBGammaCurve());
    return SimpleGammaCurvePtr(instance);
}
float SRGBGammaCurve::Encode(float x) const
{
    // (the threshold of 0.00304 occasionally found on the net was from an older draft)
    if (x <= 0.0031308f) return x * 12.92f;
    else                 return 1.055f * pow(x, 1.0f/2.4f) - 0.055f;
}
float SRGBGammaCurve::Decode(float x) const
{
    // (the threshold of 0.03928 occasionally found on the net was from an older draft)
    if (x < 0.04045f) return x / 12.92f;
    else              return pow((x + 0.055f) / 1.055f, 2.4f);
}
float SRGBGammaCurve::ApproximateDecodingGamma() const
{
    return 2.2f;
}
int SRGBGammaCurve::GetTypeId() const
{
    return kPOVList_GammaType_SRGB;
}

/*******************************************************************************/

BT709GammaCurve::BT709GammaCurve() {}
SimpleGammaCurvePtr BT709GammaCurve::Get()
{
    if (!instance)
        instance.reset(new BT709GammaCurve());
    return SimpleGammaCurvePtr(instance);
}
float BT709GammaCurve::Encode(float x) const
{
    if (x < 0.018f) return x * 4.5f;
    else            return 1.099f * pow(x, 0.45f) - 0.099f;
}
float BT709GammaCurve::Decode(float x) const
{
    // NB: ITU-R BT.709 does not officially specify a decoding transfer function. The following is the actual inverse
    //     of the implemented transfer function.
    if (x < 0.081f) return x / 4.5f;
    else            return pow((x + 0.099f) / 1.099f, 1.0f/0.45f);
}
float BT709GammaCurve::ApproximateDecodingGamma() const
{
    return 1.9f; // very rough approximation
}
int BT709GammaCurve::GetTypeId() const
{
    return kPOVList_GammaType_BT709;
}

/*******************************************************************************/

BT1361GammaCurve::BT1361GammaCurve() {}
SimpleGammaCurvePtr BT1361GammaCurve::Get()
{
    if (!instance)
        instance.reset(new BT1361GammaCurve());
    return SimpleGammaCurvePtr(instance);
}
float BT1361GammaCurve::Encode(float x) const
{
    if      (x < -0.0045f) return (1.099f * pow(-4*x, 0.45f) - 0.099f) / 4;
    else if (x <  0.018f)  return x * 4.5f;
    else                   return 1.099f * pow(x,0.45f) - 0.099f;
}
float BT1361GammaCurve::Decode(float x) const
{
    if      (x < -0.02025f) return pow((4*x + 0.099f) / 1.099f, 1.0f/0.45f) / -4;
    else if (x <  0.081f)   return x / 4.5f;
    else                    return pow((x + 0.099f) / 1.099f, 1.0f/0.45f);
}
float BT1361GammaCurve::ApproximateDecodingGamma() const
{
    return 1.9f; // very rough approximation of the x>0 section
}
int BT1361GammaCurve::GetTypeId() const
{
    return kPOVList_GammaType_BT1361;
}

/*******************************************************************************/

BT2020GammaCurve::BT2020GammaCurve() {}
SimpleGammaCurvePtr BT2020GammaCurve::Get()
{
    if (!instance)
        instance.reset(new BT2020GammaCurve());
    return SimpleGammaCurvePtr(instance);
}
float BT2020GammaCurve::Encode(float x) const
{
    // NB: We're using higher-precision coefficients than given in ITU-R BT.2020; note that this is perfectly in
    //     accordance with the specification, as the numerical values given there are just approximations, and the
    //     coefficients are instead defined as "the solutions to [a certain set of] simultaneous equations".
    if (x < 0.01805396851080780734f) return x * 4.5f;
    else                             return 1.09929682680944294035f * pow(x, 0.45f) - 0.09929682680944294035f;
}
float BT2020GammaCurve::Decode(float x) const
{
    // NB: ITU-R BT.2020 does not officially specify a decoding transfer function. The following is the actual inverse
    //     of the implemented transfer function.
    if (x < 0.08124285829863513301f) return x / 4.5f;
    else                             return pow((x + 0.09929682680944294035f) / 1.09929682680944294035f, 1.0f/0.45f);
}
float BT2020GammaCurve::ApproximateDecodingGamma() const
{
    return 1.9f; // very rough approximation
}
int BT2020GammaCurve::GetTypeId() const
{
    return kPOVList_GammaType_BT2020;
}

/*******************************************************************************/

PowerLawGammaCurve::PowerLawGammaCurve(float gamma) :
    encGamma(gamma)
{}
SimpleGammaCurvePtr PowerLawGammaCurve::GetByEncodingGamma(float gamma)
{
    if (IsNeutral(gamma))
        return NeutralGammaCurve::Get();
    return std::dynamic_pointer_cast<SimpleGammaCurve>(GetMatching(GammaCurvePtr(new PowerLawGammaCurve(gamma))));
}
SimpleGammaCurvePtr PowerLawGammaCurve::GetByDecodingGamma(float gamma)
{
    return GetByEncodingGamma(1.0f/gamma);
}
float PowerLawGammaCurve::Encode(float x) const
{
    return pow(max(x,0.0f), encGamma);
}
float PowerLawGammaCurve::Decode(float x) const
{
    return pow(max(x,0.0f), 1.0f/encGamma);
}
float PowerLawGammaCurve::ApproximateDecodingGamma() const
{
    return 1.0f/encGamma;
}
int PowerLawGammaCurve::GetTypeId() const
{
    return kPOVList_GammaType_PowerLaw;
}
float PowerLawGammaCurve::GetParam() const
{
    return 1.0f/encGamma;
}
bool PowerLawGammaCurve::Matches(const GammaCurvePtr& p) const
{
    PowerLawGammaCurve* other = dynamic_cast<PowerLawGammaCurve*>(p.get());
    if (!other) return false;
    return IsNeutral(this->encGamma / other->encGamma);
}
bool PowerLawGammaCurve::IsNeutral(float gamma)
{
    return fabs(1.0 - gamma) <= 0.01;
}

/*******************************************************************************/

ScaledGammaCurve::ScaledGammaCurve(const GammaCurvePtr& gamma, float factor) :
    baseGamma(gamma), encFactor(factor)
{
    ScaledGammaCurve* other = dynamic_cast<ScaledGammaCurve*>(baseGamma.get());
    if (other) // if base gamma curve is a scaled one as well, compute a combined scaling factor instead of nesting
    {
        baseGamma = other->baseGamma;
        encFactor *= other->encFactor;
    }
}
GammaCurvePtr ScaledGammaCurve::GetByEncoding(const GammaCurvePtr& gamma, float factor)
{
    if (IsNeutral(factor))
        return GammaCurvePtr(gamma);
    return GetMatching(GammaCurvePtr(new ScaledGammaCurve(
        GammaCurve::IsNeutral(gamma) ? GammaCurvePtr(NeutralGammaCurve::Get()) : gamma,
        factor)));
}
GammaCurvePtr ScaledGammaCurve::GetByDecoding(float factor, const GammaCurvePtr& gamma)
{
    return GetByEncoding(gamma, 1.0f/factor);
}
float ScaledGammaCurve::Encode(float x) const
{
    return baseGamma->Encode(x) * encFactor;
}
float ScaledGammaCurve::Decode(float x) const
{
    return baseGamma->Decode(x / encFactor);
}
float ScaledGammaCurve::ApproximateDecodingGamma() const
{
    return baseGamma->ApproximateDecodingGamma();
}
bool ScaledGammaCurve::Matches(const GammaCurvePtr& p) const
{
    ScaledGammaCurve* other = dynamic_cast<ScaledGammaCurve*>(p.get());
    if (!other) return false;
    return (this->baseGamma == other->baseGamma) && IsNeutral(this->encFactor / other->encFactor);
}
bool ScaledGammaCurve::IsNeutral(float scale) { return fabs(1.0 - scale) <= 1e-6; }

/*******************************************************************************/

TranscodingGammaCurve::TranscodingGammaCurve(const GammaCurvePtr& working, const GammaCurvePtr& encoding) :
    workGamma(working), encGamma(encoding)
{}
GammaCurvePtr TranscodingGammaCurve::Get(const GammaCurvePtr& working, const GammaCurvePtr& encoding)
{
    // if the working gamma space is linear, we only need the encoding gamma
    if (GammaCurve::IsNeutral(working))
        return GammaCurvePtr(encoding);
    // if both gamma spaces are the same, we can replace them with a neutral gamma curve
    if (working->Matches(encoding))
        return NeutralGammaCurve::Get();
    // check if we can replace the combination of gamma curves with a single power-law gamma curve
    PowerLawGammaCurve* powerLawWork = dynamic_cast<PowerLawGammaCurve*>(working.get());
    if (powerLawWork)
    {
        // if the encoding gamma space is linear, we only need the inverse of the working gamma
        if (GammaCurve::IsNeutral(encoding))
            return PowerLawGammaCurve::GetByEncodingGamma(powerLawWork->ApproximateDecodingGamma());
        // if both gamma spaces are based on a simple power-law, we only need to combine them into a single one
        PowerLawGammaCurve* powerLawEnc  = dynamic_cast<PowerLawGammaCurve*>(encoding.get());
        if (powerLawEnc)
            return PowerLawGammaCurve::GetByEncodingGamma(powerLawWork->ApproximateDecodingGamma() / powerLawEnc->ApproximateDecodingGamma());
    }
    // we really need a combo of two gamma curves
    return GetMatching(GammaCurvePtr(new TranscodingGammaCurve(working, encoding ? encoding : GammaCurvePtr(NeutralGammaCurve::Get()))));
}
float TranscodingGammaCurve::Encode(float x) const
{
    return encGamma->Encode(workGamma->Decode(x));
}
float TranscodingGammaCurve::Decode(float x) const
{
    return workGamma->Encode(encGamma->Decode(x));
}
float TranscodingGammaCurve::ApproximateDecodingGamma() const
{
    return encGamma->ApproximateDecodingGamma() / workGamma->ApproximateDecodingGamma();
}
bool TranscodingGammaCurve::Matches(const GammaCurvePtr& p) const
{
    TranscodingGammaCurve* other = dynamic_cast<TranscodingGammaCurve*>(p.get());
    if (!other) return false;
    return (this->encGamma->Matches(other->encGamma) && this->workGamma->Matches(other->workGamma));
}

/*******************************************************************************/

SimpleGammaCurvePtr GetGammaCurve(GammaTypeId type, float param)
{
    SimpleGammaCurvePtr gamma;
    switch (type)
    {
        case kPOVList_GammaType_Neutral:    gamma = NeutralGammaCurve::Get();                       break;
        case kPOVList_GammaType_PowerLaw:   gamma = PowerLawGammaCurve::GetByDecodingGamma(param);  break;
        case kPOVList_GammaType_SRGB:       gamma = SRGBGammaCurve::Get();                          break;
        case kPOVList_GammaType_BT709:      gamma = BT709GammaCurve::Get();                         break;
        case kPOVList_GammaType_BT1361:     gamma = BT1361GammaCurve::Get();                        break;
        case kPOVList_GammaType_BT2020:     gamma = BT2020GammaCurve::Get();                        break;
        default:                            POV_COLOURSPACE_ASSERT(false);                          break;
    }
    return gamma;
}

}
// end of namespace pov_base
