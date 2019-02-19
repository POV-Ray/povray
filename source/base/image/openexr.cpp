//******************************************************************************
///
/// @file base/image/openexr.cpp
///
/// Implementation of Industrial Light & Magic OpenEXR image file handling.
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
#include "base/image/openexr.h"

#ifndef OPENEXR_MISSING

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <string>

// Other 3rd party header files
#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/stringutilities.h"
#include "base/types.h"
#include "base/image/colourspace.h"
#include "base/image/encoding.h"
#include "base/image/image.h"
#include "base/image/metadata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

namespace OpenEXR
{

using namespace Imf;
using namespace Imath;

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/*****************************************************************************
* Local typedefs
******************************************************************************/

///////////////////////////////////////////
// class POV_EXR_OStream
///////////////////////////////////////////
class POV_EXR_OStream final : public Imf::OStream
{
    public:
        POV_EXR_OStream(pov_base::OStream& pov_stream) : Imf::OStream(UCS2toSysString(pov_stream.Name()).c_str()), os(pov_stream) { }
        virtual ~POV_EXR_OStream() override { }

        virtual void write(const char *c, int n) override
        {
            if(os.write(c, n) == false)
                throw POV_EXCEPTION(kFileDataErr, "Error while writing EXR output");
        }

        virtual Int64 tellp() override
        {
            unsigned long pos = os.tellg();
            if((int) pos == -1)
                throw POV_EXCEPTION(kFileDataErr, "Error while writing EXR output");
            return(pos);
        }

        virtual void seekp(Int64 pos) override
        {
            if(os.seekg((unsigned long)pos) == false)
                throw POV_EXCEPTION(kFileDataErr, "Error when writing EXR output");
        }
    private:
        pov_base::OStream& os;
};

///////////////////////////////////////////
// class POV_EXR_IStream
///////////////////////////////////////////
class POV_EXR_IStream final : public Imf::IStream
{
    public:
        POV_EXR_IStream(pov_base::IStream& pov_stream) : Imf::IStream(UCS2toSysString(pov_stream.Name()).c_str()), is(pov_stream)
        {
            is.seekg(0, IOBase::seek_end);
            fsize = is.tellg();
            is.seekg(0, IOBase::seek_set);
        }

        virtual ~POV_EXR_IStream() override { }

        virtual void clear(void) override { is.clearstate(); }

        virtual bool read(char *c, int n) override
        {
            if(is.read(c, n) == false)
                throw POV_EXCEPTION(kFileDataErr, "Error while reading EXR file");
            return (is.tellg() < fsize);
        }

        virtual Int64 tellg() override
        {
            unsigned long pos = is.tellg();
            if((int)pos == -1)
                throw POV_EXCEPTION(kFileDataErr, "Error while reading EXR file");
            return pos;
        }

        virtual void seekg(Int64 pos) override
        {
            if(is.seekg((unsigned long)pos) == false)
                throw POV_EXCEPTION(kFileDataErr, "Error while reading EXR file");
        }
    private:
        pov_base::IStream& is;
        unsigned long fsize;
};

/*****************************************************************************
* Implementation
******************************************************************************/

Image *Read(IStream *file, const ImageReadOptions& options)
{
    unsigned int width;
    unsigned int height;
    Image *image = nullptr;

    // OpenEXR files store linear color values by default, so never convert unless the user overrides
    // (e.g. to handle a non-compliant file).
    GammaCurvePtr gamma;
    if (options.gammacorrect)
    {
        if (options.gammaOverride)
            gamma = TranscodingGammaCurve::Get(options.workingGamma, options.defaultGamma);
        else
            gamma = TranscodingGammaCurve::Get(options.workingGamma, NeutralGammaCurve::Get());
    }

    // OpenEXR officially uses premultiplied alpha, so that's the preferred mode to use for the image container unless the user overrides
    // (e.g. to handle a non-compliant file).
    bool premul = true;
    if (options.premultipliedOverride)
        premul = options.premultiplied;

    // TODO: code this to observe the request for alpha in the input file type.
    POV_EXR_IStream is(*file);
    try
    {
        RgbaInputFile rif(is);
        Array2D<Rgba> pixels;
        Box2i dw = rif.dataWindow();
        ImageDataType imagetype = options.itype;

        width = dw.max.x - dw.min.x + 1;
        height = dw.max.y - dw.min.y + 1;
        pixels.resizeErase(height, width);
        rif.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
        rif.readPixels(dw.min.y, dw.max.y);

        if(imagetype == ImageDataType::Undefined)
            imagetype = ImageDataType::RGBFT_Float;

        image = Image::Create(width, height, imagetype);
        image->SetPremultiplied(premul); // set desired storage mode regarding alpha premultiplication

        for(int row = 0; row < height; row++)
        {
            for(int col = 0; col < width; col++)
            {
                struct Rgba &rgba = pixels [row][col];
                SetEncodedRGBAValue(image, col, row, gamma, (float)rgba.r, (float)rgba.g, (float)rgba.b, (float)rgba.a, premul);
            }
        }
    }
    catch(const std::exception& e)
    {
        throw POV_EXCEPTION(kFileDataErr, e.what());
    }

    return image;
}

void Write(OStream *file, const Image *image, const ImageWriteOptions& options)
{
    int width = image->GetWidth();
    int height = image->GetHeight();
    bool use_alpha = image->HasTransparency() && options.AlphaIsEnabled();
    float pixelAspect = 1.0;
    Header hdr(width, height, pixelAspect, Imath::V2f(0, 0), 1.0, INCREASING_Y, ZIP_COMPRESSION);
    std::unique_ptr<Rgba[]> pixels(new Rgba[width * height]);
    Rgba *p = pixels.get();

    // OpenEXR format mandates that colours are encoded linearly.
    GammaCurvePtr gamma = TranscodingGammaCurve::Get(options.workingGamma, NeutralGammaCurve::Get());

    // OpenEXR officially uses premultiplied alpha, so that's the way we do it unless the user overrides
    // (e.g. to handle a non-compliant file).
    bool premul = options.AlphaIsPremultiplied(true);

    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            float r, g, b, a;
            GetEncodedRGBAValue(image, col, row, gamma, r, g, b, a, premul);
            *p++ = Rgba(r, g, b, use_alpha ? a : 1.0f);
        }
    }

    POV_EXR_OStream os(*file);
    try
    {
        Imf::RgbaChannels channels;
        if (options.grayscale)
            if (use_alpha)
                channels = WRITE_YA; // only write luminance & alpha
            else
                channels = WRITE_Y; // only write luminance
        else
            if (use_alpha)
                channels = WRITE_RGBA; // write RGB & alpha
            else
                channels = WRITE_RGB; // write RGB

        Metadata meta;
        std::string comments;
        if (!meta.getComment1().empty())
            comments += meta.getComment1() + "\n";
        if (!meta.getComment2().empty())
            comments += meta.getComment2() + "\n";
        if (!meta.getComment3().empty())
            comments += meta.getComment3() + "\n";
        if (!meta.getComment4().empty())
            comments += meta.getComment4() + "\n";

        if (!comments.empty())
            hdr.insert("comments",StringAttribute(comments));

        std::string software= meta.getSoftware();
        std::string datetime= meta.getDateTime();
        hdr.insert("software",StringAttribute(software));
        hdr.insert("creation",StringAttribute(datetime));

        RgbaOutputFile rof(os, hdr, channels);
        rof.setFrameBuffer(pixels.get(), 1, width);
        rof.writePixels(height);
    }
    catch(const std::exception& e)
    {
        throw POV_EXCEPTION(kFileDataErr, e.what());
    }
}

}
// end of namespace OpenEXR

}
// end of namespace pov_base

#endif  // OPENEXR_MISSING
