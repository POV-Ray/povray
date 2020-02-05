//******************************************************************************
///
/// @file base/image/image.h
///
/// Declarations related to image containers.
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

#ifndef POVRAY_BASE_IMAGE_H
#define POVRAY_BASE_IMAGE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"
#include "base/image/image_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <algorithm>
#include <string>
#include <vector>

// POV-Ray header files (base module)
#include "base/colour.h"
#include "base/fileinputoutput_fwd.h"
#include "base/pov_err.h"
#include "base/image/colourspace_fwd.h"
#include "base/image/dither_fwd.h"

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBaseImage Image Handling
/// @ingroup PovBase
///
/// @{

enum class ImageDataType : int
{
    Undefined,      ///< Special value indicating that image decoder is free to pick most fitting type.
    Bit_Map,        ///< Palette-based image with 2 palette entries.
    Colour_Map,     ///< Palette-based image with up to 256 palette entries.
    Gray_Int8,      ///< Single-channel (grayscale) image using 8-bit linear encoding.
    Gray_Int16,     ///< Single-channel (grayscale) image using 16-bit linear encoding.
    GrayA_Int8,     ///< Dual-channel (grayscale and alpha) image using 8-bit linear encoding.
    GrayA_Int16,    ///< Dual-channel (grayscale and alpha) image using 16-bit linear encoding.
    RGB_Int8,       ///< 3-channel (colour) image using 8-bit linear encoding.
    RGB_Int16,      ///< 3-channel (colour) image using 16-bit linear encoding.
    RGBA_Int8,      ///< 4-channel (colour and alpha) image using 8-bit linear encoding.
    RGBA_Int16,     ///< 4-channel (colour and alpha) image using 16-bit linear encoding.
    RGBFT_Float,    ///< 5-channel (colour, filter and transmit) image using single-precision floating-point encoding.
    RGB_Gamma8,     ///< 3-channel (colour) image using 8-bit gamma encoding.
    RGB_Gamma16,    ///< 3-channel (colour) image using 16-bit gamma encoding.
    RGBA_Gamma8,    ///< 4-channel (colour and alpha) image using 8-bit gamma colour encoding and 8-bit linear alpha encoding.
    RGBA_Gamma16,   ///< 4-channel (colour and alpha) image using 16-bit gamma colour encoding and 16-bit linear alpha encoding.
    Gray_Gamma8,    ///< Single-channel (grayscale) image using 8-bit gamma encoding.
    Gray_Gamma16,   ///< Single-channel (grayscale) image using 16-bit gamma encoding.
    GrayA_Gamma8,   ///< Dual-channel (grayscale and alpha) image using 8-bit gamma greyscale encoding and 8-bit linear alpha encoding.
    GrayA_Gamma16,  ///< Dual-channel (grayscale and alpha) image using 16-bit gamma greyscale encoding and 16-bit linear alpha encoding.
};

/// The mode to use for alpha handling.
enum class ImageAlphaMode : int
{
    None,           ///< Disable alpha channel. @note Not a valid setting for input files.
    Default,        ///< Use auto-detection or file format specific default.
    Premultiplied,  ///< Enforce premultiplied mode, aka associated alpha.
    Straight,       ///< Enforce straight mode, aka unassociated alpha.
};

struct ImageReadOptions final
{
    ImageDataType itype;
    SimpleGammaCurvePtr defaultGamma;   // the gamma curve to use by default for converting to linear colour space
    SimpleGammaCurvePtr workingGamma;   // the working colour space gamma
    bool gammaOverride;                 // whether to apply defaultGamma even if the file indicates a different gamma
    bool gammacorrect;                  // whether to do any gamma correction at all; if false, raw encoded values are used
    bool premultipliedOverride;         // whether to override file-format default for alpha premultiplication
    bool premultiplied;                 // whether to expect premultiplied ("associated") alpha or not ("straight alpha")
    mutable std::vector<std::string> warnings;

    ImageReadOptions() :
        itype(ImageDataType::Undefined),
        gammaOverride(false), gammacorrect(false), premultipliedOverride(false), premultiplied(false)
    {}
};

struct ImageWriteOptions final
{
    //------------------------------------------------------------------------------
    /// @name Gamma Handling
    /// @{

    /// Gamma to encode for.
    /// Set this to `nullptr` to use the file format specific default.
    /// @note
    ///     This setting is ignored with file formats that mandate linear encoding or a
    ///     specific encoding gamma.
    SimpleGammaCurvePtr encodingGamma;

    /// Working colour space gamma to encode from.
    /// Set to `nullptr` or a neutral gamma curve to indicate linear working colour space.
    SimpleGammaCurvePtr workingGamma;

    /// @}
    //------------------------------------------------------------------------------

    /// Dithering algorithm.
    /// Leave this at the default to disable dithering.
    /// @note
    ///     This setting is ignored with file formats that are not prone to colour banding
    ///     artifacts (such as OpenEXR) or do not benefit from dithering (such as JPEG).
    DitherStrategySPtr ditherStrategy;

    unsigned int offset_x; ///< Currently not actively set.
    unsigned int offset_y; ///< Currently not actively set.

    /// How to handle image transparency.
    /// Set this to @ref ImageAlphaMode::None to disable creation of an alpha channel,
    /// @ref ImageAlphaMode::Default to write an alpha channel using a file format specific
    /// default mode, @ref ImageAlphaMode::Premultiplied to write an alpha channel using
    /// premultiplied mode (aka associated alpha), or @ref ImageAlphaMode::Straight to write an
    /// alpha channel using straight mode (aka unassociated alpha).
    /// @note
    ///     This setting is ignored with file formats that do not support transparency, or
    ///     for which transparency support has not been implemented in POV-Ray.
    ImageAlphaMode alphaMode;

    /// Bits per colour channel.
    /// Set this to `0` to use the file format specific default.
    /// @note
    ///     This setting is ignored with file formats that mandate a particular bit depth,
    ///     for which POV-Ray only supports a particular bit depth, or for which bit depth
    ///     is not applicable (such as JPEG).
    /// @note
    ///     The actual bit depth may differ if the file format or POV-Ray's implementation
    ///     thereof does not support the requested bit depth. In that case, the next higher
    ///     supported bit depth is used if possible, or the highest supported bit depth
    ///     otherwise.
    unsigned char bitsPerChannel;

    /// Whether to use compression.
    /// Set this to a negative value to use the file format specific default setting, `0`
    /// to disable, or any higher value to enable. Depending on the file format, such a
    /// setting may be interpreted as a format specific compression parameter.
    /// @note
    ///     Whether a positive value indicates a mode, compression level or quality level
    ///     is specific to the file format.
    /// @note
    ///     This setting is ignored with file formats that never use compression, always
    ///     use compression, or for which POV-Ray's implementation leaves no choice.
    signed short compression;

    /// Whether to write a greyscale image.
    /// @note
    ///     This setting is ignored with file formats that do not support a dedicated
    ///     greyscale mode, or for which support of such a mode has not been implemented
    ///     in POV-Ray.
    bool grayscale : 1;

    ImageWriteOptions();

    inline bool AlphaIsEnabled() const
    {
        return (alphaMode != ImageAlphaMode::None);
    }

    inline bool AlphaIsPremultiplied(bool defaultToPremultiplied) const
    {
        if (defaultToPremultiplied)
            return (alphaMode != ImageAlphaMode::Straight);
        else
            return (alphaMode == ImageAlphaMode::Premultiplied);
    }

    GammaCurvePtr GetTranscodingGammaCurve(GammaCurvePtr defaultEncodingGamma) const;
};

/**
 *  Generic image data container.
 *
 *  @note   Except for access functions having a `premul` parameter as well as those named
 *          `GetEncodedSomethingValue` or `SetEncodedSomethingValue`, all other access functions are unaware of
 *          premultiplied vs. non-premultiplied alpha issues, and will access the data in whatever format
 *          it is stored (as far as alpha handling goes).
 *
 *  @note   When backed by a gamma-encoded data container, unsigned int access methods are presumed
 *          to read/write raw encoded data, while float access methods will read/write logical
 *          linear values.
 *
 *  @note   Image coordinates increase from left (x=0) to right (x>0) and from top (y=0) to bottom (y>0).
 */
class Image
{
    public:
        struct RGBMapEntry final
        {
            float red;
            float green;
            float blue;

            RGBMapEntry() : red(0.0f), green(0.0f), blue(0.0f) { }
            RGBMapEntry(float r, float g, float b) : red(r), green(g), blue(b) { }
        };

        struct RGBAMapEntry final
        {
            float red;
            float green;
            float blue;
            float alpha;

            RGBAMapEntry() : red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f) { }
            RGBAMapEntry(float r, float g, float b, float a) : red(r), green(g), blue(b), alpha(a) { }
        };

        struct RGBFTMapEntry final
        {
            float red;
            float green;
            float blue;
            float filter;
            float transm;

            RGBFTMapEntry() : red(0.0f), green(0.0f), blue(0.0f), filter(0.0f), transm(0.0f) { }
            RGBFTMapEntry(float r, float g, float b, float f, float t) : red(r), green(g), blue(b), filter(f), transm(t) { }
        };

        enum ColourMapType
        {
            NoColourMap,
            RGBColourMap,
            RGBAColourMap,
            RGBFTColourMap
        };

        /// Image file type identifier.
        enum ImageFileType
        {
            GIF,
            POT,
            SYS,
            IFF,
            TGA,
            PGM,
            PPM,
            PNG,
            JPEG,
            TIFF,
            BMP,
            EXR,
            HDR
        };

        virtual ~Image() { }

        /// Suggest an image data container satisfying certain constraints.
        ///
        /// This method returns an @ref ImageDataType representing a non-paletted, integer-based
        /// image data container matching the specified constraints.
        ///
        /// @note
        ///     The number of bits per colour channel is taken as a minimum requirement. All other
        ///     constraints are currently implemented as exact-match requirements.
        /// @note
        ///     RGB alpha is not supported yet.
        ///
        /// @param  minBitsPerChannel   Minimum number of bits per colour or alpha channel.
        /// @param  colourChannels      Number of colour channels (1 = greyscale, 3 = RGB).
        /// @param  alphaChannels       Number of alpha channels (0 = no alpha, 1 = common alpha, 3 = RGB alpha).
        /// @param  linear              Whether the caller will write linear (as opposed to gamma encoded) data.
        static ImageDataType GetImageDataType(int minBitsPerChannel, int colourChannels, int alphaChannels, bool linear);

        /// Suggest an image data container satisfying certain constraints.
        ///
        /// This method is a convenience function equivalent to calling:
        ///
        ///     GetImageDataType(minBitsPerChannel, colourChannels, alpha? 1:0, IsNeutral(gamma));
        ///
        /// See @ref GetImageDataType(int,int,int,bool) for more details.
        ///
        /// @param  minBitsPerChannel   Minimum number of bits per colour or alpha channel.
        /// @param  colourChannels      Number of colour channels (1 = greyscale, 3 = RGB).
        /// @param  alpha               Whether to provide a (common) alpha channel.
        /// @param  gamma               Encoding gamma of the data to be written.
        static ImageDataType GetImageDataType(int minBitsPerChannel, int colourChannels, bool alpha, GammaCurvePtr gamma);

        static Image *Create(unsigned int w, unsigned int h, ImageDataType t, unsigned int maxRAMmbHint, unsigned int pixelsPerBlockHint);
        static Image *Create(unsigned int w, unsigned int h, ImageDataType t, bool allowFileBacking = false);
        static Image *Create(unsigned int w, unsigned int h, ImageDataType t, const std::vector<RGBMapEntry>& m, bool allowFileBacking = false);
        static Image *Create(unsigned int w, unsigned int h, ImageDataType t, const std::vector<RGBAMapEntry>& m, bool allowFileBacking = false);
        static Image *Create(unsigned int w, unsigned int h, ImageDataType t, const std::vector<RGBFTMapEntry>& m, bool allowFileBacking = false);

        // ftype = use this image type, if "Undefined" use best match
        static Image *Read(ImageFileType ftype, IStream *file, const ImageReadOptions& options = ImageReadOptions());

        static void Write(ImageFileType ftype, OStream *file, const Image *image, const ImageWriteOptions& options = ImageWriteOptions());

        unsigned int GetWidth() const { return width; }
        unsigned int GetHeight() const { return height; }
        ImageDataType GetImageDataType() const { return type; }

        /// Returns true if image is fully opaque.
        virtual bool IsOpaque() const = 0;

        virtual bool IsGrayscale() const = 0;
        virtual bool IsColour() const = 0;
        virtual bool IsFloat() const = 0;
        virtual bool IsInt() const = 0;

        /// Returns true if backed by a palette-based container.
        virtual bool IsIndexed() const = 0;

        /// Returns true if backed by a gamma-encoded data container.
        virtual bool IsGammaEncoded() const = 0;

        /// Returns true if container features a genuine alpha channel.
        virtual bool HasAlphaChannel() const = 0;

        /// Returns true if container features genuine filter & transmit channels.
        virtual bool HasFilterTransmit() const = 0;

        /// Returns true if container features any way of storing transparency information.
        virtual bool HasTransparency() const { return (HasAlphaChannel() || HasFilterTransmit()); }

        /// Returns the maximum value supported by int access methods or for palette indices (1, 255 or 65535).
        virtual unsigned int GetMaxIntValue() const = 0;

        /// Specifies whether container holds color data premultiplied with alpha
        void SetPremultiplied(bool b) { premultiplied = b; } // TODO - mechanism not fully functional yet for image reading

        /// Returns true if container holds data premultiplied with alpha
        bool IsPremultiplied() const { return premultiplied; }

        /**
         *  Requests the image container to perform deferred decoding of integer values.
         *  In order for the request to be honored, the requested value range must match the container's native
         *  bit depth, and the container must have a neutral encoding gamma curve set at present.
         *  @note           If the request is honored, this will also affect subsequent unsigned int read accesses.
         *  @param[in,out]  gamma   Gamma encoding curve of the encoded material. Set to empty by the function
         *                          if the request is accepted.
         *  @param[in]      max     Maximum encoded value. In order for the request to be honored, this must match
         *                          the image container's native bit depth.
         *  @return                 true it the request is accepted, false otherwise.
         */
        virtual bool TryDeferDecoding(GammaCurvePtr& gamma, unsigned int max) = 0;

        void GetRGBIndexedValue(unsigned char index, float& red, float& green, float& blue) const;
        void GetRGBAIndexedValue(unsigned char index, float& red, float& green, float& blue, float& alpha) const;
        void GetRGBTIndexedValue(unsigned char index, float& red, float& green, float& blue, float& transm) const;
        void GetRGBFTIndexedValue(unsigned char index, float& red, float& green, float& blue, float& filter, float& transm) const;

        void SetRGBIndexedValue(unsigned char index, float red, float green, float blue);
        void SetRGBAIndexedValue(unsigned char index, float red, float green, float blue, float alpha);
        void SetRGBTIndexedValue(unsigned char index, float red, float green, float blue, float transm);
        void SetRGBFTIndexedValue(unsigned char index, float red, float green, float blue, float filter, float transm);
        void SetRGBFTIndexedValue(unsigned char index, const RGBFTColour& colour);

        virtual bool GetBitValue(unsigned int x, unsigned int y) const = 0;
        virtual float GetGrayValue(unsigned int x, unsigned int y) const = 0;
        virtual void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const = 0;
        virtual void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const = 0;
        virtual void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const = 0;
        virtual void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const = 0;
        virtual void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const = 0;
        virtual unsigned char GetIndexedValue(unsigned int x, unsigned int y);

        virtual void SetBitValue(unsigned int x, unsigned int y, bool bit) = 0;
        virtual void SetGrayValue(unsigned int x, unsigned int y, float gray) = 0;
        virtual void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray) = 0;
        virtual void SetGrayAValue(unsigned int x, unsigned int y, float gray, float alpha) = 0;
        virtual void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int alpha) = 0;
        virtual void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue) = 0;
        virtual void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue) = 0;
        virtual void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float alpha) = 0;
        virtual void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) = 0;
        virtual void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm) = 0;
        virtual void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col) = 0;
        virtual void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float filter, float transm) = 0;
        virtual void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col) = 0;
        virtual void SetIndexedValue(unsigned int x, unsigned int y, unsigned char index);

        // convenience functions for image evaluation
        void GetRGBValue(unsigned int x, unsigned int y, RGBColour& colour, bool premul = false) const;
        void GetRGBTValue(unsigned int x, unsigned int y, RGBTColour& colour, bool premul = false) const;
        void GetRGBFTValue(unsigned int x, unsigned int y, RGBFTColour& colour, bool premul = false) const;

        virtual void FillBitValue(bool bit) = 0;
        virtual void FillGrayValue(float gray) = 0;
        virtual void FillGrayValue(unsigned int gray) = 0;
        virtual void FillGrayAValue(float gray, float alpha) = 0;
        virtual void FillGrayAValue(unsigned int gray, unsigned int alpha) = 0;
        virtual void FillRGBValue(float red, float green, float blue) = 0;
        virtual void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue) = 0;
        virtual void FillRGBAValue(float red, float green, float blue, float alpha) = 0;
        virtual void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) = 0;
        virtual void FillRGBTValue(float red, float green, float blue, float transm) = 0;
        virtual void FillRGBFTValue(float red, float green, float blue, float filter, float transm) = 0;

        unsigned int GetColourMapSize() const;

        void GetColourMap(std::vector<RGBMapEntry>& m) const;
        void GetColourMap(std::vector<RGBAMapEntry>& m) const;
        void GetColourMap(std::vector<RGBFTMapEntry>& m) const;

        void SetColourMap(const std::vector<RGBMapEntry>& m);
        void SetColourMap(const std::vector<RGBAMapEntry>& m);
        void SetColourMap(const std::vector<RGBFTMapEntry>& m);
/*
        void CopyTo(unsigned int x, unsigned int y, const Image& srcimage)
        {
            // TODO
        }
        void CopyToScaled(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const Image& srcimage, bool smooth = false)
        {
            // TODO
        }*/
    protected:
        struct MapEntry final
        {
            float red;
            float green;
            float blue;
            float filter; // alpha = filter
            float transm;

            MapEntry() : red(0.0f), green(0.0f), blue(0.0f), filter(0.0f), transm(0.0f) { }
            MapEntry(float r, float g, float b, float f, float t) : red(r), green(g), blue(b), filter(f), transm(t) { }
            MapEntry(const RGBMapEntry& e) : red(e.red), green(e.green), blue(e.blue), filter(0.0f), transm(0.0f) { }
            MapEntry(const RGBAMapEntry& e) : red(e.red), green(e.green), blue(e.blue), filter(e.alpha), transm(0.0f) { }
            MapEntry(const RGBFTMapEntry& e) : red(e.red), green(e.green), blue(e.blue), filter(e.filter), transm(e.transm) { }
        };

        std::vector<MapEntry> colormap;
        ColourMapType colormaptype;
        unsigned int width;
        unsigned int height;
        ImageDataType type;
        bool premultiplied;

        Image(unsigned int w, unsigned int h, ImageDataType t) :
            width(w), height(h), type(t), colormaptype(NoColourMap), premultiplied(false) { }

        Image(unsigned int w, unsigned int h, ImageDataType t, const std::vector<RGBMapEntry>& m) :
            width(w), height(h), type(t), colormaptype(RGBColourMap), premultiplied(false) { colormap.resize(std::max(m.size(), sizeof(unsigned char) * 256)); colormap.assign(m.begin(), m.end()); }

        Image(unsigned int w, unsigned int h, ImageDataType t, const std::vector<RGBAMapEntry>& m) :
            width(w), height(h), type(t), colormaptype(RGBAColourMap), premultiplied(false) { colormap.resize(std::max(m.size(), sizeof(unsigned char) * 256)); colormap.assign(m.begin(), m.end()); }

        Image(unsigned int w, unsigned int h, ImageDataType t, const std::vector<RGBFTMapEntry>& m) :
            width(w), height(h), type(t), colormaptype(RGBFTColourMap), premultiplied(false) { colormap.resize(std::max(m.size(), sizeof(unsigned char) * 256)); colormap.assign(m.begin(), m.end()); }

        float RGB2Gray(float red, float green, float blue) const
        {
            return RGBColour(red, green, blue).Greyscale();
        }
    private:

        Image() = delete;
        Image(const Image&) = delete;
        Image& operator=(Image&) = delete;
};

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_IMAGE_H
