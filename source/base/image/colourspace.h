//******************************************************************************
///
/// @file base/image/colourspace.h
///
/// Declarations related to colour space conversions.
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

#ifndef POVRAY_BASE_COLOURSPACE_H
#define POVRAY_BASE_COLOURSPACE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// Standard C++ header files
#include <vector>

// Boost header files
#if POV_MULTITHREADED
#include <boost/thread.hpp>
#endif

// POV-Ray base header files
#include "base/colour.h"
#include "base/types.h"

namespace pov_base
{

class GammaCurve;
class SimpleGammaCurve;

/// Class holding a shared reference to a gamma curve.
typedef shared_ptr<GammaCurve> GammaCurvePtr;

/// Class holding a shared reference to a simple gamma curve.
typedef shared_ptr<SimpleGammaCurve> SimpleGammaCurvePtr;

/// Abstract class representing an encoding gamma curve (or, more generally, transfer function).
///
/// In this generic form, the gamma curve may be arbitrarily complex.
///
/// @note   To conserve memory, derived classes should prevent duplicates from being instantiated.
///         This base class provides a caching mechanism to help accomplish this.
///
class GammaCurve
{
    public:

        /// Encoding function.
        ///
        /// This function is to be implemented by subclasses to define the encoding rules of that particular gamma curve.
        ///
        /// @note           Input values 0.0 and 1.0 should be mapped to output values 0.0 and 1.0, respectively.
        ///                 Input values that cannot be mapped should be clipped to the nearest valid value.
        ///
        /// @param[in]  x   Input value.
        /// @return         Output value.
        ///
        virtual float Encode(float x) const = 0;

        /// Decoding function.
        ///
        /// This function is to be implemented by subclasses to define the decoding rules of that particular gamma curve.
        ///
        /// @note           Input values 0.0 and 1.0 should be mapped to output values 0.0 and 1.0, respectively.
        ///                 Input values that cannot be mapped should be clipped to the nearest valid value.
        ///
        /// @param[in]  x   Input value.
        /// @return         Output value.
        ///
        virtual float Decode(float x) const = 0;

        /// Approximated power-law gamma
        ///
        /// This function is to be implemented by subclasses to return an average overall gamma to be used as a
        /// gamma-law approximation of the particular gamma curve.
        ///
        virtual float ApproximateDecodingGamma() const = 0;

        /// Retrieves a lookup table for faster decoding.
        ///
        /// This feature is intended to be used for low-dynamic-range, 8 or 16 bit depth, non-linearly encoded images
        /// to be kept in encoded format during render to reduce the memory footprint without unnecessarily degrading
        /// performance.
        ///
        /// @note           The lookup table pointer is only valid during the lifetime of the GammaCurve object.
        ///                 Any entity holding a pointer to the lookup table must therefore also maintain a strong
        ///                 pointer to the GammaCurve object.
        ///
        /// @param[in]  max The maximum encoded value; must be either 255 for 8-bit depth, or 65535 for 16-bit depth.
        /// @return         Pointer to a table of pre-computed Decode() results for values from 0.0 to 1.0
        ///                 in increments of 1.0/max.
        ///
        float* GetLookupTable(unsigned int max);

        /// Convenience function to test whether a gamma curve pointer refers to a neutral curve.
        ///
        /// @param[in]  p   The gamma curve pointer to test.
        /// @return         `true` if the gamma curve pointer is empty or the gamma curve is neutral, `false` otherwise.
        ///
        static bool IsNeutral(const GammaCurvePtr& p) { return (!p || p->IsNeutral()); }

        /// Convenience function to apply encoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral encoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for encoding.
        /// @param[in]  x   The value to encode, typically in the range from 0.0 to 1.0.
        /// @return         The encoded value, typically in the range from 0.0 to 1.0.
        ///
        static float Encode(const GammaCurvePtr& p, float x) { if (IsNeutral(p)) return x; else return p->Encode(x); }

        /// Convenience function to apply encoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral encoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for encoding.
        /// @param[in]  c   The colour to encode, typically in the range from 0.0 to 1.0.
        /// @return         The encoded colour, typically in the range from 0.0 to 1.0.
        ///
        static RGBColour Encode(const GammaCurvePtr& p, const RGBColour& c)
        {
            if (IsNeutral(p))
                return c;
            else
                return RGBColour(p->Encode(c.red()), p->Encode(c.green()), p->Encode(c.blue()));
        }

        /// Convenience function to apply encoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral encoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for encoding.
        /// @param[in]  c   The colour to encode, typically in the range from 0.0 to 1.0.
        /// @return         The encoded colour, typically in the range from 0.0 to 1.0.
        ///
        static RGBTColour Encode(const GammaCurvePtr& p, const RGBTColour& c)
        {
            if (IsNeutral(p))
                return c;
            else
                return RGBTColour(p->Encode(c.red()), p->Encode(c.green()), p->Encode(c.blue()), c.transm());
        }

        /// Convenience function to apply encoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral encoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for encoding.
        /// @param[in]  c   The colour to encode, typically in the range from 0.0 to 1.0.
        /// @return         The encoded colour, typically in the range from 0.0 to 1.0.
        ///
        static AttenuatingColour Encode(const GammaCurvePtr& p, const AttenuatingColour& c)
        {
            if (IsNeutral(p))
                return c;
            else
            {
                AttenuatingColour result;
                for (unsigned int i = 0; i < c.kChannels; i ++)
                    result[i] = p->Encode(c[i]);
                return result;
            }
        }

        /// Convenience function to apply encoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral encoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for encoding.
        /// @param[in]  c   The colour to encode, typically in the range from 0.0 to 1.0.
        /// @return         The encoded colour, typically in the range from 0.0 to 1.0.
        ///
        static TransColour Encode(const GammaCurvePtr& p, const TransColour& c)
        {
            if (IsNeutral(p))
                return c;
            else
                return TransColour(Encode(p, c.colour()), Encode(p, c.trans()));
        }

        /// Convenience function to apply encoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral encoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for encoding.
        /// @param[in]  c   The colour to encode, typically in the range from 0.0 to 1.0.
        /// @return         The encoded colour, typically in the range from 0.0 to 1.0.
        ///
        static FilterTransm Encode(const GammaCurvePtr& p, const FilterTransm& c)
        {
            if (IsNeutral(p))
                return c;
            else
                return FilterTransm(Encode(p, c.filter()), Encode(p, c.transm()));
        }

        /// Convenience function to apply decoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral decoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for decoding.
        /// @param[in]  x   The value to decode, typically in the range from 0.0 to 1.0.
        /// @return         The decoded value, typically in the range from 0.0 to 1.0.
        ///
        static float Decode(const GammaCurvePtr& p, float x) { if (IsNeutral(p)) return x; else return p->Decode(x); }

        /// Convenience function to apply decoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral decoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for decoding.
        /// @param[in]  c   The colour to decode, typically in the range from 0.0 to 1.0.
        /// @return         The decoded colour, typically in the range from 0.0 to 1.0.
        ///
        static RGBColour Decode(const GammaCurvePtr& p, const RGBColour& c)
        {
            if (IsNeutral(p))
                return c;
            else
                return RGBColour(p->Decode(c.red()), p->Decode(c.green()), p->Decode(c.blue()));
        }

        /// Convenience function to apply decoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral decoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for decoding.
        /// @param[in]  c   The colour to decode, typically in the range from 0.0 to 1.0.
        /// @return         The decoded colour, typically in the range from 0.0 to 1.0.
        ///
        static RGBTColour Decode(const GammaCurvePtr& p, const RGBTColour& c)
        {
            if (IsNeutral(p))
                return c;
            else
                return RGBTColour(p->Decode(c.red()), p->Decode(c.green()), p->Decode(c.blue()), c.transm());
        }

        /// Convenience function to apply decoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral decoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for decoding.
        /// @param[in]  c   The colour to decode, typically in the range from 0.0 to 1.0.
        /// @return         The decoded colour, typically in the range from 0.0 to 1.0.
        ///
        static AttenuatingColour Decode(const GammaCurvePtr& p, const AttenuatingColour& c)
        {
            if (IsNeutral(p))
                return c;
            else
            {
                AttenuatingColour result;
                for (unsigned int i = 0; i < c.kChannels; i ++)
                    result[i] = p->Decode(c[i]);
                return result;
            }
        }

        /// Convenience function to apply decoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral decoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for decoding.
        /// @param[in]  c   The colour to decode, typically in the range from 0.0 to 1.0.
        /// @return         The decoded colour, typically in the range from 0.0 to 1.0.
        ///
        static TransColour Decode(const GammaCurvePtr& p, const TransColour& c)
        {
            if (IsNeutral(p))
                return c;
            else
                return TransColour(Decode(p, c.colour()), Decode(p, c.trans()));
        }

        /// Convenience function to apply decoding according to a given gamma curve pointer.
        ///
        /// @note           If an empty gamma curve pointer is passed, neutral decoding is applied.
        ///
        /// @param[in]  p   The gamma curve pointer to use for decoding.
        /// @param[in]  c   The colour to decode, typically in the range from 0.0 to 1.0.
        /// @return         The decoded colour, typically in the range from 0.0 to 1.0.
        ///
        static FilterTransm Decode(const GammaCurvePtr& p, const FilterTransm& c)
        {
            if (IsNeutral(p))
                return c;
            else
                return FilterTransm(Decode(p, c.filter()), Decode(p, c.transm()));
        }

    protected:

        /// Cached lookup table for 8-bit lookup.
        ///
        /// This member variable caches the pointer returned by a first call to `GetLookupTable(255)` to avoid creating
        /// multiple copies of the table.
        ///
        float* lookupTable8;

        /// Cached lookup table for 16-bit lookup.
        ///
        /// This member variable caches the pointer returned by a first call to `GetLookupTable(65535)` to avoid creating
        /// multiple copies of the table.
        ///
        float* lookupTable16;

#if POV_MULTITHREADED
        /// Mutex to guard access to @ref lookupTable8 and @ref lookupTable16.
        boost::mutex lutMutex;
#endif

        /// Constructor.
        GammaCurve() : lookupTable8(NULL), lookupTable16(NULL) {}

        /// Destructor.
        virtual ~GammaCurve() { if (lookupTable8) delete[] lookupTable8; if (lookupTable16) delete[] lookupTable16; }

        /// Function to test whether two gamma curves match.
        ///
        /// This function is to be implemented by subclasses to define how to test for matching gamma curves.
        ///
        /// @param[in]  p   Pointer to the gamma curve to compare with.
        /// @return         `true` if the gamma curve will produce the same result as this instance, `false` otherwise.
        ///
        virtual bool Matches(const GammaCurvePtr& p) const { return this == p.get(); }

        /// Function to test whether the gamma curve is neutral.
        ///
        /// This function is to be implemented by subclasses to define how to test for neutral gamma curves.
        ///
        /// @return         `true` if this gamma curve is neutral (i.e. maps any value to itself), `false` otherwise.
        ///
        virtual bool IsNeutral() const { return false; }

        /// Cache of all gamma curves currently in use.
        ///
        /// This static member variable caches pointers of gamma curve instances currently in use, forming the basis
        /// of the `GetMatching()` mechanism to avoid duplicate instances.
        ///
        static list<weak_ptr<GammaCurve> > cache;

#if POV_MULTITHREADED
        /// Mutex to guard access to `cache`.
        static boost::mutex cacheMutex;
#endif

        /// Function to manage the gamma curve cache.
        ///
        /// This static function will look up a gamma curve from the cache to match the supplied one, or encache the
        /// supplied one if no match has been found.
        ///
        /// @note           Derived classes allowing for multiple instances can pass any newly created instance
        ///                 through this function to make sure no duplicate gamma curve instances are ever in use.
        ///                 For this purpose, no references to the instance supplied shall be retained; instead,
        ///                 only the instance returned by this function shall be kept.
        ///
        /// @param      p   Pointer to the gamma curve to look up or encache.
        /// @return         A matching encached gamma curve (possibly, but not necessarily, the instance supplied).
        ///
        static GammaCurvePtr GetMatching(const GammaCurvePtr& p);

        friend class TranscodingGammaCurve;
};

/// Abstract class representing a simple transfer function having at most one float parameter.
class SimpleGammaCurve : public GammaCurve
{
    public:

        /// Get type identifier.
        ///
        /// This function is to be implemented by subclasses to return the type identifier of the gamma curve subclass.
        ///
        virtual int GetTypeId() const = 0;

        /// Get parameter.
        ///
        /// This function is to be implemented by subclasses to return the type-specific parameter of the gamma curve subclass.
        ///
        virtual float GetParam() const = 0;

    protected:

        /// Function to test whether two gamma curves match.
        ///
        /// This function is to be implemented by subclasses to define how to test for matching gamma curves.
        ///
        /// @param[in]  p   Pointer to the gamma curve to compare with.
        /// @return         `true` if the gamma curve will produce the same result as this instance, `false` otherwise.
        ///
        virtual bool Matches(const GammaCurvePtr& p) const
        {
            SimpleGammaCurve* simpleP = dynamic_cast<SimpleGammaCurve*>(p.get());
            if (simpleP)
                return ((typeid(*simpleP) == typeid(*this)) && (simpleP->GetParam() == this->GetParam()));
            else
                return false;
        }
};

/// Abstract class representing a simple transfer function having no parameters.
class UniqueGammaCurve : public SimpleGammaCurve
{
    public:

        /// Get parameter.
        ///
        /// This function is to be implemented by subclasses to return the type-specific parameter of the gamma curve subclass.
        ///
        virtual float GetParam() const { return 0.0; }

    protected:

        /// Function to test whether two gamma curves match.
        ///
        /// This function is to be implemented by subclasses to define how to test for matching gamma curves.
        ///
        /// @param[in]  p   Pointer to the gamma curve to compare with.
        /// @return         `true` if the gamma curve will produce the same result as this instance, `false` otherwise.
        ///
        virtual bool Matches(const GammaCurvePtr& p) const
        {
            UniqueGammaCurve* uniqueP = dynamic_cast<UniqueGammaCurve*>(p.get());
            if (uniqueP)
                return (typeid(*uniqueP) == typeid(*this));
            else
                return false;
        }
};

/// Class representing a neutral gamma curve.
class NeutralGammaCurve : public UniqueGammaCurve
{
    public:
        static SimpleGammaCurvePtr Get();
        virtual float Encode(float x) const;
        virtual float Decode(float x) const;
        virtual float ApproximateDecodingGamma() const;
        virtual int GetTypeId() const;
    private:
        static SimpleGammaCurvePtr instance;
        virtual bool Matches(const GammaCurvePtr&) const;
        virtual bool IsNeutral() const;
        NeutralGammaCurve();
};

/// Class representing the IEC 61966-2-1 sRGB transfer function.
///
/// @note   While the sRGB transfer functionn can be approximated with a classic power-law curve
///         having a constant gamma of 1/2.2, the two are not identical. This class represents
///         the exact function as specified in IEC 61966-2-1.
///
class SRGBGammaCurve : public UniqueGammaCurve
{
    public:
        static SimpleGammaCurvePtr Get();
        virtual float Encode(float x) const;
        virtual float Decode(float x) const;
        virtual float ApproximateDecodingGamma() const;
        virtual int GetTypeId() const;
    private:
        static SimpleGammaCurvePtr instance;
        SRGBGammaCurve();
};

/// Class representing the ITU-R BT.709 transfer function.
///
/// @note   This class does _not_ account for the "black digital count" and "white digital count" being defined
///         as 16/255 and 235/255, respectively.
///
class ITURBT709GammaCurve : public GammaCurve // TODO we could make this a UniqueGammaCurve if we assign it a type ID
{
    public:
        static GammaCurvePtr Get();
        virtual float Encode(float x) const;
        virtual float Decode(float x) const;
        virtual float ApproximateDecodingGamma() const;
    private:
        static GammaCurvePtr instance;
        ITURBT709GammaCurve();
};

/// Class representing the Rec1361 transfer function.
///
/// This transfer function is a wide-gamut extension to that specified in ITU-R BT.709.
///
class Rec1361GammaCurve : public GammaCurve // TODO we could make this a UniqueGammaCurve if we assign it a type ID
{
    public:
        static GammaCurvePtr Get();
        virtual float Encode(float x) const;
        virtual float Decode(float x) const;
        virtual float ApproximateDecodingGamma() const;
    private:
        static GammaCurvePtr instance;
        Rec1361GammaCurve();
};

/// Class representing a classic constant-gamma (power-law) gamma encoding curve.
class PowerLawGammaCurve : public SimpleGammaCurve
{
    public:
        static SimpleGammaCurvePtr GetByEncodingGamma(float gamma);
        static SimpleGammaCurvePtr GetByDecodingGamma(float gamma);
        virtual float Encode(float x) const;
        virtual float Decode(float x) const;
        virtual float ApproximateDecodingGamma() const;
        virtual int GetTypeId() const;
        virtual float GetParam() const;
    protected:
        float encGamma;
        PowerLawGammaCurve(float encGamma);
        virtual bool Matches(const GammaCurvePtr&) const;
        static bool IsNeutral(float gamma);
};

/// Class representing a scaled-encoding variant of another gamma curves.
class ScaledGammaCurve : public GammaCurve
{
    public:
        static GammaCurvePtr GetByEncoding(const GammaCurvePtr&, float encodingFactor);
        static GammaCurvePtr GetByDecoding(float decodingFactor, const GammaCurvePtr&);
        virtual float Encode(float x) const;
        virtual float Decode(float x) const;
        virtual float ApproximateDecodingGamma() const;
    protected:
        GammaCurvePtr baseGamma;
        float encFactor;
        ScaledGammaCurve(const GammaCurvePtr&, float);
        virtual bool Matches(const GammaCurvePtr&) const;
        static bool IsNeutral(float factor);
};

/// Class representing a transformation between different (non-linear) "gamma spaces".
///
/// @note   This class is only required for backward compatibility with POV-Ray 3.6.
///
class TranscodingGammaCurve : public GammaCurve
{
    public:
        static GammaCurvePtr Get(const GammaCurvePtr& working, const GammaCurvePtr& encoding);
        virtual float Encode(float x) const;
        virtual float Decode(float x) const;
        virtual float ApproximateDecodingGamma() const;
    protected:
        GammaCurvePtr workGamma;
        GammaCurvePtr encGamma;
        TranscodingGammaCurve();
        TranscodingGammaCurve(const GammaCurvePtr&, const GammaCurvePtr&);
        virtual bool Matches(const GammaCurvePtr&) const;
};

enum GammaTypeId
{
    kPOVList_GammaType_Unknown,
    kPOVList_GammaType_Neutral,
    kPOVList_GammaType_PowerLaw,
    kPOVList_GammaType_SRGB
};

/// Generic transfer function factory.
///
/// @param  typeId  transfer function type (one of kPOVList_GammaType_*)
/// @param  param   parameter for parameterized transfer function (e.g. gamma of power-law function)
///
SimpleGammaCurvePtr GetGammaCurve(GammaTypeId typeId, float param);

}

#endif // POVRAY_BASE_COLOURSPACE_H
