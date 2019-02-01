//******************************************************************************
///
/// @file frontend/imageprocessing.cpp
///
/// @todo   What's in here?
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
#include "frontend/imageprocessing.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/path.h"
#include "base/image/colourspace.h"
#include "base/image/dither.h"
#include "base/image/image.h"

// POV-Ray header files (POVMS module)
#include "povms/povmsid.h"

// POV-Ray header files (frontend module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

// TODO: update ImageProcessing with the means of accepting and caching
// blocks of pixels as opposed to individual ones, with a back-end that
// can serialize completed rows to the final image output file.

namespace pov_frontend
{

using std::shared_ptr;

enum
{
    X = 0,
    Y = 1,
    Z = 2
};

ImageProcessing::ImageProcessing(unsigned int width, unsigned int height)
{
    image = shared_ptr<Image>(Image::Create(width, height, ImageDataType::RGBFT_Float));
    toStderr = toStdout = false;

    // TODO FIXME - find a better place for this
    image->SetPremultiplied(true); // POV-Ray uses premultiplied opacity for its math, so that's what will end up in the image container
}

ImageProcessing::ImageProcessing(POVMS_Object& ropts)
{
    unsigned int width(ropts.TryGetInt(kPOVAttrib_Width, 160));
    unsigned int height(ropts.TryGetInt(kPOVAttrib_Height, 120));
    unsigned int blockSize(ropts.TryGetInt(kPOVAttrib_RenderBlockSize, 32));
    unsigned int maxBufferMem(ropts.TryGetInt(kPOVAttrib_MaxImageBufferMem, 128)); // number is megabytes

    image = shared_ptr<Image>(Image::Create(width, height, ImageDataType::RGBFT_Float, maxBufferMem, blockSize * blockSize));
    toStdout = OutputIsStdout(ropts);
    toStderr = OutputIsStderr(ropts);

    // TODO FIXME - find a better place for this
    image->SetPremultiplied(true); // POV-Ray uses premultiplied opacity for its math, so that's what will end up in the image container
}

ImageProcessing::ImageProcessing(shared_ptr<Image>& img)
{
    image = img;
    toStderr = toStdout = false;

    // TODO FIXME - find a better place for this
    image->SetPremultiplied(true); // POV-Ray uses premultiplied opacity for its math, so that's what will end up in the image container
}

ImageProcessing::~ImageProcessing()
{
}

UCS2String ImageProcessing::WriteImage(POVMS_Object& ropts, POVMSInt frame, int digits)
{
    if(ropts.TryGetBool(kPOVAttrib_OutputToFile, true) == true)
    {
        ImageWriteOptions wopts;
        Image::ImageFileType imagetype = Image::SYS;
        unsigned int filetype = POV_File_Image_System;

        wopts.bitsPerChannel = clip(ropts.TryGetInt(kPOVAttrib_BitsPerColor, 8), 1, 16);
        wopts.alphaMode = (ropts.TryGetBool(kPOVAttrib_OutputAlpha, false) ? ImageAlphaMode::Default : ImageAlphaMode::None );
        wopts.compression = (ropts.Exist(kPOVAttrib_Compression) ? clip(ropts.GetInt(kPOVAttrib_Compression), 0, 255) : -1);
        wopts.grayscale = ropts.TryGetBool(kPOVAttrib_GrayscaleOutput, false);

        switch(ropts.TryGetInt(kPOVAttrib_OutputFileType, DEFAULT_OUTPUT_FORMAT))
        {
            case kPOVList_FileType_Targa:
                imagetype = Image::TGA;
                filetype = POV_File_Image_Targa;
                break;
            case kPOVList_FileType_CompressedTarga:
                // TODO - this file type is obsolete, as Targa compression can now
                // be controlled using the `Compression` INI setting.
                imagetype = Image::TGA;
                filetype = POV_File_Image_Targa;
                wopts.compression = 1;
                break;
            case kPOVList_FileType_PNG:
                imagetype = Image::PNG;
                filetype = POV_File_Image_PNG;
                break;
            case kPOVList_FileType_JPEG:
                imagetype = Image::JPEG;
                filetype = POV_File_Image_JPEG;
                break;
            case kPOVList_FileType_PPM:
                imagetype = Image::PPM;
                filetype = POV_File_Image_PPM;
                break;
            case kPOVList_FileType_BMP:
                imagetype = Image::BMP;
                filetype = POV_File_Image_BMP;
                break;
            case kPOVList_FileType_OpenEXR:
                imagetype = Image::EXR;
                filetype = POV_File_Image_EXR;
                break;
            case kPOVList_FileType_RadianceHDR:
                imagetype = Image::HDR;
                filetype = POV_File_Image_HDR;
                break;
            case kPOVList_FileType_System:
                imagetype = Image::SYS;
                filetype = POV_File_Image_System;
                break;
            default:
                throw POV_EXCEPTION_STRING("Invalid file type for output");
        }

        GammaTypeId gammaType;
        float gamma;
        if (ropts.Exist(kPOVAttrib_FileGammaType))
        {
            gammaType = (GammaTypeId)ropts.GetInt(kPOVAttrib_FileGammaType);
            gamma = ropts.GetFloat(kPOVAttrib_FileGamma);
            wopts.encodingGamma = GetGammaCurve(gammaType, gamma);
        }
        else
        {
            // if user didn't explicitly specify File_Gamma, use the file format specific default.
            wopts.encodingGamma.reset();
        }
        // NB: RenderFrontend<...>::CreateView should have dealt with kPOVAttrib_LegacyGammaMode already and updated kPOVAttrib_WorkingGammaType and kPOVAttrib_WorkingGamma to fit.
        gammaType = (GammaTypeId)ropts.TryGetInt(kPOVAttrib_WorkingGammaType, DEFAULT_WORKING_GAMMA_TYPE);
        gamma = ropts.TryGetFloat(kPOVAttrib_WorkingGamma, DEFAULT_WORKING_GAMMA);
        wopts.workingGamma = GetGammaCurve(gammaType, gamma);

        bool dither = ropts.TryGetBool(kPOVAttrib_Dither, false);
        DitherMethodId ditherMethod = DitherMethodId::kNone;
        if (dither)
            ditherMethod = ropts.TryGetEnum(kPOVAttrib_DitherMethod, DitherMethodId::kBlueNoise);
        wopts.ditherStrategy = GetDitherStrategy(ditherMethod, image->GetWidth());

        // in theory this should always return a filename since the frontend code
        // sets it via a call to GetOutputFilename() before the render starts.
        UCS2String filename = ropts.TryGetUCS2String(kPOVAttrib_OutputFile, "");
        if(filename.empty() == true)
            filename = GetOutputFilename(ropts, frame, digits);

        std::unique_ptr<OStream> imagefile(NewOStream(filename.c_str(), filetype, false)); // TODO - check file permissions somehow without macro [ttrf]
        if (imagefile == nullptr)
            throw POV_EXCEPTION_CODE(kCannotOpenFileErr);

        Image::Write(imagetype, imagefile.get(), image.get(), wopts);

        return filename;
    }
    else
        return UCS2String();
}

shared_ptr<Image>& ImageProcessing::GetImage()
{
    return image;
}

bool ImageProcessing::OutputIsStdout(POVMS_Object& ropts)
{
    UCS2String path(ropts.TryGetUCS2String(kPOVAttrib_OutputFile, ""));

    toStdout = (path == u"-") || (path == u"stdout");
    toStderr = (path == u"stderr");
    return toStdout;
}

bool ImageProcessing::OutputIsStderr(POVMS_Object& ropts)
{
    OutputIsStdout(ropts);
    return toStderr;
}

UCS2String ImageProcessing::GetOutputFilename(POVMS_Object& ropts, POVMSInt frame, int digits)
{
    Path path(ropts.TryGetUCS2String(kPOVAttrib_OutputFile, ""));
    UCS2String filename = path.GetFile();
    UCS2String ext;
    Image::ImageFileType imagetype;

    switch(ropts.TryGetInt(kPOVAttrib_OutputFileType, DEFAULT_OUTPUT_FORMAT))
    {
        case kPOVList_FileType_Targa:
        case kPOVList_FileType_CompressedTarga:
            ext = u".tga";
            imagetype = Image::TGA;
            break;

        case kPOVList_FileType_PNG:
            ext = u".png";
            imagetype = Image::PNG;
            break;

        case kPOVList_FileType_JPEG:
            ext = u".jpg";
            imagetype = Image::JPEG;
            break;

        case kPOVList_FileType_PPM:
            ext = u".ppm"; // TODO FIXME - in case of greyscale output, extension should default to ".pgm"
            imagetype = Image::PPM;
            break;

        case kPOVList_FileType_BMP:
            ext = u".bmp";
            imagetype = Image::BMP;
            break;

        case kPOVList_FileType_OpenEXR:
            ext = u".exr";
            imagetype = Image::EXR;
            break;

        case kPOVList_FileType_RadianceHDR:
            ext = u".hdr";
            imagetype = Image::HDR;
            break;

#ifdef POV_SYS_IMAGE_TYPE
        case kPOVList_FileType_System:
            ext = u"" POV_SYS_IMAGE_EXTENSION;
            imagetype = Image::POV_SYS_IMAGE_TYPE;
            break;
#endif

        default:
            throw POV_EXCEPTION_STRING("Invalid file type for output");
    }

    if (OutputIsStdout(ropts) || OutputIsStderr())
    {
        switch (imagetype)
        {
            case Image::HDR:
            case Image::PNG:
            case Image::TGA:
            case Image::PPM:
            case Image::BMP:
                break;

            default:
                throw POV_EXCEPTION_STRING("Output to STDOUT/STDERR not supported for selected file format");
        }
        return (OutputIsStdout() ? u"stdout" : u"stderr");
    }

    // we disallow an output filename that consists purely of the default extension
    // (e.g. Output_File_Name=".png").
    if((filename == ext) || (filename.empty() == true))
    {
        // get the input file name and merge the existing path if need be.
        if (path.Empty() == true)
        {
            path = ropts.TryGetUCS2String(kPOVAttrib_InputFile, "object.pov");
            filename = path.GetFile();
        }
        else
            filename = Path(ropts.TryGetUCS2String(kPOVAttrib_InputFile, "object.pov")).GetFile();

        // if the input file name ends with '.' or '.anything', we remove it
        UCS2String::size_type pos = filename.find_last_of('.');
        if(pos != UCS2String::npos)
            filename.erase(pos);
    }
    else if ((path.HasVolume() == false) && (path.Empty() == false))
    {
        // to get here, path must be a relative path with filename
        // if the filename ends with a '.' or with the default extension (case-sensitive),
        // we remove it.
        UCS2String::size_type pos = filename.find_last_of('.');
        if((pos != UCS2String::npos) && ((pos == filename.size() - 1) || (filename.substr(pos) == ext)))
            filename.erase(pos);
    }
    else
    {
        // if there is no path component already, get it from the input file.
        if (path.Empty() == true)
            path = ropts.TryGetUCS2String(kPOVAttrib_InputFile, "object.pov");

        // if the filename ends with a '.' or with the default extension (case-sensitive),
        // we remove it.
        UCS2String::size_type pos = filename.find_last_of('.');
        if((pos != UCS2String::npos) && ((pos == filename.size() - 1) || (filename.substr(pos) == ext)))
            filename.erase(pos);
    }

    if (digits > 0)
    {
        for(int i = 0; i < digits; i++)
            filename += '0';
        for(UCS2String::size_type i = filename.length() - 1; frame > 0; i--, frame /= 10)
            filename[i] = '0' + (frame % 10);
    }

    path.SetFile(filename + ext);

    return path();
}

}
// end of namespace pov_frontend
