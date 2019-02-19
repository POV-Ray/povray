//******************************************************************************
///
/// @file base/image/ppm.cpp
///
/// Implementation of Netpbm Portable Pixmap/Graymap (PPM/PGM) image file
/// handling.
///
/// This module contains the code to read and write the PPM and PGM file formats
/// according to Netpbm specs (http://netpbm.sourceforge.net/doc/):
///
/// For input, both ASCII ("plain") and binary ("raw") formats (magic numbers
/// `P2`/`P3` and `P5`/`P6`, respectively) are supported.
///
/// For outout we write binary ("raw") PPM files (magic number `P6`), unless
/// `Greyscale_Output=on` is specified in which case we write binary PGM files
/// (magic number `P5`). Maxvalue is set to 255 if a bit depth of 8 or lower
/// is chosen, or 65535 otherwise.
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
#include "base/image/ppm.h"

// C++ variants of C standard header files
#include <cctype>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/types.h"
#include "base/image/colourspace.h"
#include "base/image/encoding.h"
#include "base/image/image.h"
#include "base/image/metadata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

namespace Netpbm
{

enum NetpbmDataFormat
{
    kNetpbmDataFormat_ASCII,
    kNetpbmDataFormat_8bit,
    kNetpbmDataFormat_16bit,
};

/*****************************************************************************/

void Write (OStream *file, const Image *image, const ImageWriteOptions& options)
{
    int                 file_type = POV_File_Image_PPM;
    int                 width = image->GetWidth() ;
    int                 height = image->GetHeight() ;
    int                 bpcc = options.bitsPerChannel;
    bool                grayscale = false;
    unsigned int        rval;
    unsigned int        gval;
    unsigned int        bval;
    unsigned int        gray;
    unsigned int        mask;
    GammaCurvePtr       gamma;
    DitherStrategy&     dither = *options.ditherStrategy;
    bool                plainFormat;

#ifdef ASCII_PPM_OUTPUT
    // When compiled with ASCII_PPM_OUTPUT, we used to write the more verbose "plain" (ASCII)
    // format, so that's what we'll still generate in case the setting is negative (indicating we
    // should do our default thing).
    plainFormat = (options.compression < 1);
#else
    // When compiled without ASCII_PPM_OUTPUT, we used to write the more compact "raw" (binary)
    // format, so that's what we'll still generate in case the setting is negative (indicating we
    // should do our default thing).
    plainFormat = (options.compression == 0);
#endif

    if (bpcc <= 0)
        bpcc = image->GetMaxIntValue() == 65535 ? 16 : 8 ;
    else if (bpcc > 16)
        bpcc = 16 ;

    mask = (1 << bpcc) - 1 ;

    // do we want 16 bit grayscale (PGM) output ?
    // TODO - the check for image container type is here to mimick old code; do we still need it?
    if (image->IsGrayscale() || options.grayscale)
    {
        grayscale = true;
        if (plainFormat)
            file->printf("P2\n");
        else
            file->printf("P5\n");
    }
    else
    {
        // The official Netpbm standard mandates the use of the ITU-R-BT.709 transfer function, although it
        // acknowledges the use of linear encoding or the sRGB transfer function as alternative de-facto standards.
        gamma = options.GetTranscodingGammaCurve(BT709GammaCurve::Get());
        if (plainFormat)
            file->printf("P3\n");
        else
            file->printf("P6\n");
    }

    // Prepare metadata, as comment in the header
    Metadata meta;
    file->printf("# Software: %s\n", meta.getSoftware().c_str());
    file->printf("# Render Date: %s\n" ,meta.getDateTime().c_str());
    if (!meta.getComment1().empty())
        file->printf("# %s\n", meta.getComment1().c_str());
    if (!meta.getComment2().empty())
        file->printf("# %s\n", meta.getComment2().c_str());
    if (!meta.getComment3().empty())
        file->printf("# %s\n", meta.getComment3().c_str());
    if (!meta.getComment4().empty())
        file->printf("# %s\n", meta.getComment4().c_str());
    file->printf("%d %d\n%d\n", width, height, mask);

    for (int y = 0 ; y < height ; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (grayscale)
            {
                gray = GetEncodedGrayValue (image, x, y, gamma, mask, dither) ;

                if (plainFormat)
                {
                    file->printf("%u\n", gray);
                }
                else if (bpcc > 8)
                {
                    file->Write_Byte((gray >> 8) & 0xFF);
                    file->Write_Byte(gray & 0xFF);
                }
                else
                {
                    file->Write_Byte(gray);
                }
                if (!*file)
                    throw POV_EXCEPTION(kFileDataErr, "Cannot write PGM output data");
            }
            else
            {
                GetEncodedRGBValue(image, x, y, gamma, mask, rval, gval, bval, dither);

                if (plainFormat)
                {
                    file->printf("%u ", rval);
                    file->printf("%u ", gval);
                    file->printf("%u\n", bval);
                }
                else if (bpcc > 8)
                {
                    // 16 bit per value
                    file->Write_Byte(rval >> 8);
                    file->Write_Byte(rval & 0xFF);
                    file->Write_Byte(gval >> 8);
                    file->Write_Byte(gval & 0xFF);
                    file->Write_Byte(bval >> 8);
                    file->Write_Byte(bval & 0xFF);
                }
                else
                {
                    // 8 bit per value
                    file->Write_Byte(rval);
                    file->Write_Byte(gval);
                    file->Write_Byte(bval);
                }
                if (!*file)
                    throw POV_EXCEPTION(kFileDataErr, "Cannot write PPM output data");
            }
        }
    }
}

/*****************************************************************************/

/// Read an individual character from a Netpbm file, potentially skipping comments.
inline static int ReadNetpbmAsciiChar (IStream *file, bool allowComments)
{
    int c = file->Read_Byte();

    if (allowComments && (c == '#'))
    {
        do
        {
            c = file->Read_Byte();
        }
        while ((c != '\n') && (c != EOF));
    }

    return c;
}

/// Read an plain ASCII numeric value from a Netpbm file, potentially skipping comments.
static POV_UINT32 ReadNetpbmAsciiValue (IStream *file, bool allowComments)
{
    POV_UINT32 value = 0;
    int c;
    int pos = 0;
    char buffer[50] = "";

    do
    {
        c = ReadNetpbmAsciiChar (file, allowComments);
        // TODO - we may want to warn in case we just encountered a CR.
    }
    while (isspace (c));

    if (!isdigit (c))
        throw POV_EXCEPTION(kFileDataErr, "Invalid data in Netpbm (PGM/PPM) file");

    do
    {
        POV_UINT32 oldValue = value;
        value = value * 10 + (c-'0');
        if (value < oldValue)
            // numeric overflow occurred
            throw POV_EXCEPTION(kFileDataErr, "Excessively large value in Netpbm (PGM/PPM) file");
        c = ReadNetpbmAsciiChar (file, allowComments);
    }
    while (isdigit(c));

    if ((c != EOF) && (!isspace (c)))
        throw POV_EXCEPTION(kFileDataErr, "Invalid data in Netpbm (PGM/PPM) file");

    // TODO - we may want to warn in case we just encountered a CR.

    return value;
}

/// Read an individual raster value from a Netpbm file.
inline static POV_UINT16 ReadNetpbmRasterValue (IStream *file, NetpbmDataFormat format)
{
    if (format == kNetpbmDataFormat_ASCII)
        return ReadNetpbmAsciiValue (file, false);

    int hi, lo;

    if (format == kNetpbmDataFormat_16bit)
    {
        hi = file->Read_Byte();
        if (hi == EOF)
            throw POV_EXCEPTION(kFileDataErr, "Unexpected end of file in Netpbm (PGM/PPM) file");
    }
    else
        hi = 0;

    lo = file->Read_Byte();
    if (lo == EOF)
        throw POV_EXCEPTION(kFileDataErr, "Unexpected end of file in Netpbm (PGM/PPM) file");

    return (static_cast<POV_UINT16>(hi) << 8) + static_cast<POV_UINT16>(lo);
}

/*****************************************************************************/

// TODO: make sure we destroy the image if we throw an exception
Image *Read (IStream *file, const ImageReadOptions& options)
{
    POV_UINT32          width;
    POV_UINT32          height;
    POV_UINT32          maxval;
    Image               *image = nullptr;
    unsigned char       magicNumber[2];

    bool                isMonochrome;
    bool                isAsciiData;
    bool                is16BitData;
    NetpbmDataFormat    dataFormat;
    POV_UINT16          maxImageVal;
    POV_UINT16          r,g,b;

    // PGM files may or may not be gamma-encoded.
    GammaCurvePtr gamma;
    if (options.gammacorrect && options.defaultGamma)
        gamma = TranscodingGammaCurve::Get(options.workingGamma, options.defaultGamma);

    // --- Read Header ---
    if (!file->read(magicNumber, 2))
        throw POV_EXCEPTION(kFileDataErr, "Failed to read Netpbm (PGM/PPM) magic number");

    if (magicNumber[0] != 'P')
        throw POV_EXCEPTION(kFileDataErr, "File is not a supported Netpbm (PGM/PPM) file");

    switch (magicNumber[1])
    {
        case '2': // "plain" (ASCII) Portable Gray Map (PGM)
            isMonochrome = true;
            isAsciiData  = true;
            break;

        case '3': // "plain" (ASCII) Portable Pixel Map (PPM)
            isMonochrome = false;
            isAsciiData  = true;
            break;

        case '5': // "raw" (binary) Portable Gray Map (PGM)
            isMonochrome = true;
            isAsciiData  = false;
            break;

        case '6': // "raw" (binary) Portable Pixel Map (PPM)
            isMonochrome = false;
            isAsciiData  = false;
            break;

        case '1': // "plain" (ASCII) Portable Bit Map (PBM)
        case '4': // "raw" (binary) Portable Bit Map (PBM)
        case '7': // Portable Arbitrary Map (PAM)
        default:
            throw POV_EXCEPTION(kFileDataErr, "File is not a supported Netpbm (PGM/PPM) file");
            break;
    }

    width  = ReadNetpbmAsciiValue (file, true);
    height = ReadNetpbmAsciiValue (file, true);
    maxval = ReadNetpbmAsciiValue (file, true);

    if ((maxval > 65535) || (maxval < 1))
        throw POV_EXCEPTION(kFileDataErr, "Unsupported number of brightness levels in Netpbm (PGM/PPM) file");

    is16BitData = (maxval > 255);

    dataFormat  = (isAsciiData ? kNetpbmDataFormat_ASCII : (is16BitData ? kNetpbmDataFormat_16bit : kNetpbmDataFormat_8bit));
    maxImageVal = (is16BitData ? 65535 : 255);

    // We'll be using an image container that provides for automatic decoding if possible - unless there's no such decoding to do.
    gamma = ScaledGammaCurve::GetByDecoding(float(maxImageVal)/float(maxval), gamma); // Note that we'll apply the scaling even if we don't officially gamma-correct
    ImageDataType imagetype = options.itype;
    if (imagetype == ImageDataType::Undefined)
        imagetype = Image::GetImageDataType((is16BitData ? 16 : 8), (isMonochrome ? 1 : 3), false, gamma);
    image = Image::Create (width, height, imagetype) ;
    // NB: PGM files don't use alpha, so premultiplied vs. non-premultiplied is not an issue
    image->TryDeferDecoding(gamma, maxImageVal); // try to have gamma adjustment being deferred until image evaluation.

    for (int i = 0; i < height; i++)
    {
        for (int x = 0; x < width; x++)
        {
            if (isMonochrome)
            {
                g = ReadNetpbmRasterValue (file, dataFormat);
                SetEncodedGrayValue (image, x, i, gamma, maxImageVal, g);
            }
            else
            {
                r = ReadNetpbmRasterValue (file, dataFormat);
                g = ReadNetpbmRasterValue (file, dataFormat);
                b = ReadNetpbmRasterValue (file, dataFormat);
                SetEncodedRGBValue (image, x, i, gamma, maxImageVal, r,g,b);
            }
        }
    }

    // TODO - we may want to warn in case we haven't reached the end of file yet.

    return image;
}

}
// end of namespace Netpbm

}
// end of namespace pov_base
