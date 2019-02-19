//******************************************************************************
///
/// @file base/image/bmp.cpp
///
/// Implementation of Windows Bitmap (BMP) image file handling.
///
/// @author Wlodzimierz ABX Skiba (abx@abx.art.pl)
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
#include "base/image/bmp.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <vector>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/types.h"
#include "base/image/colourspace.h"
#include "base/image/encoding.h"
#include "base/image/image.h"

// this must be the last file included
#include "base/povdebug.h"


/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define WIN_NEW     40
#define WIN_OS2_OLD 12
#define BI_RGB      0L
#define BI_RLE8     1L
#define BI_RLE4     2L


namespace pov_base
{

namespace Bmp
{

/*****************************************************************************
*
* FUNCTION
*
*   Read_Safe_Char
*
* INPUT
*
* *filePUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*   Jan 2004 : Added exception to allow cleanup on error [CJC]
*
******************************************************************************/

static inline unsigned char Read_Safe_Char (IStream& in)
{
    unsigned char ch;

    ch = in.Read_Byte();
    if (!in)
        throw POV_EXCEPTION(kFileDataErr, "Error reading data from BMP image.") ;

    return (ch) ;
}

// skip forward without using seekg
static bool Skip (IStream *file, int bytes)
{
    while (*file && bytes--)
        file->Read_Byte () ;
    return (*file) ;
}

// skip forward without using seekg
static bool Skip (OStream *file, int bytes)
{
    while (*file && bytes--)
        file->Write_Byte (0) ;
    return (*file) ;
}

// write a long to a stream in little-endian (e.g. x86) format
static void Write_Long (OStream *file, unsigned long val)
{
    file->Write_Byte (val & 0xff) ;
    file->Write_Byte ((val >> 8) & 0xff) ;
    file->Write_Byte ((val >> 16) & 0xff) ;
    file->Write_Byte ((val >> 24) & 0xff) ;
}

// write a short to a stream in little-endian (e.g. x86) format
static void Write_Short (OStream *file, unsigned short val)
{
    file->Write_Byte (val & 0xff) ;
    file->Write_Byte ((val >> 8) & 0xff) ;
}

// read a long from a stream in little-endian (e.g. x86) format
static unsigned long Read_Long (IStream *file)
{
    unsigned long ch1 = Read_Safe_Char (*file) ;
    unsigned long ch2 = Read_Safe_Char (*file) ;
    unsigned long ch3 = Read_Safe_Char (*file) ;
    unsigned long ch4 = Read_Safe_Char (*file) ;
    return ((ch4 << 24) | (ch3 << 16) | (ch2 << 8) | ch1) ;
}

// read a short from a stream in little-endian (e.g. x86) format
static unsigned short Read_Short (IStream *file)
{
    unsigned short ch1 = Read_Safe_Char (*file) ;
    unsigned short ch2 = Read_Safe_Char (*file) ;
    return ((ch2 << 8) | ch1) ;
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_1b
*
* INPUT
*
* *filePUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_1b(Image *image, IStream& in, unsigned width, unsigned height)
{
    int c = 0;
    unsigned pwidth = ((width+31)>>5)<<5; /* clear bits to get 4 byte boundary */

    for (int y=height - 1; y >= 0; y--)
    {
        for (int x=0; x<pwidth; x++)
        {
            if ((x&7) == 0)
                c = Read_Safe_Char (in);
            if (x<width)
            {
                image->SetBitValue (x, y, (c & 0x80) ? 1 : 0);
                c <<= 1;
            }
        }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_4b_RGB
*
* INPUT
*
* *filePUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_4b_RGB(Image *image, IStream& in, unsigned width, unsigned height)
{
    int c = 0;
    unsigned pwidth = ((width+7)>>3)<<3;

    for (int y=height - 1; y >= 0; y--)
    {
        for (int x=0; x<pwidth; x++)
        {
            if ((x&1)==0)
                c = Read_Safe_Char(in);
            if (x<width)
            {
                image->SetIndexedValue (x, y, (c&0xf0)>>4) ;
                c <<= 4;
            }
        }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_4b_RLE
*
* INPUT
*
* *filePUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_4b_RLE(Image *image, IStream& in, unsigned width, unsigned height)
{
    int c, cc = 0;
    unsigned x = 0;
    unsigned y = height-1;

    while (1)
    {
        c = Read_Safe_Char (in);
        if (c)
        {
            cc = Read_Safe_Char (in);
            for (int i=0; i<c; i++, x++)
                if ((y<height) && (x<width))
                    image->SetIndexedValue (x, y, (i&1) ? (cc &0x0f) : ((cc>>4)&0x0f));
        }
        else
        {
            c = Read_Safe_Char (in);
            if (c==0)
            {
                x=0;
                y--;
            }
            else if (c==1)
                return;
            else if (c==2)
            {
                x += Read_Safe_Char (in);
                y -= Read_Safe_Char (in);
            }
            else
            {
                for (int i=0; i<c; i++, x++)
                {
                    if ((i&1)==0)
                        cc = Read_Safe_Char (in);
                    if ((y<height) && (x<width))
                        image->SetIndexedValue (x, y, ((i&1)?cc:(cc>>4))&0x0f) ;
                }
                if (((c&3)==1) || ((c&3)==2))
                    Read_Safe_Char (in);
            }
        }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_8b_RGB
*
* INPUT
*
* *filePUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_8b_RGB(Image *image, IStream& in, unsigned width, unsigned height)
{
    int c;
    unsigned pwidth = ((width+3)>>2)<<2;

    for (unsigned y=height; (--y)<height;)
        for (unsigned x=0; x<pwidth; x++)
        {
            c = Read_Safe_Char (in);
            if (x<width)
                image->SetIndexedValue (x, y, c);
        }
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_BMP_8b_RLE
*
* INPUT
*
* *filePUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*
******************************************************************************/

static void Read_BMP_8b_RLE(Image *image, IStream& in, unsigned width, unsigned height)
{
    int c, cc;
    unsigned x = 0;
    unsigned y = height-1;

    while (1)
    {
        c = Read_Safe_Char (in);
        if (c)
        {
            cc = Read_Safe_Char (in);
            for (int i=0; i<c; i++, x++)
                if ((y<height) && (x<width))
                    image->SetIndexedValue (x, y, cc);
        }
        else
        {
            c = Read_Safe_Char (in);
            switch(c)
            {
                case 0:
                    x = 0;
                    y--;
                    break;
                case 1:
                    return;
                    break;
                case 2:
                    x += Read_Safe_Char (in);
                    y -= Read_Safe_Char (in);
                    break;
                default:
                    for (int i=0; i<c; i++, x++)
                        if ((y<height) && (x<width))
                            image->SetIndexedValue (x, y, Read_Safe_Char (in));
                    if (c & 1)
                        Read_Safe_Char (in); /* "absolute mode" runs are word-aligned */
            }
        }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Open_BMP_File
*
* INPUT
*
* *filePUT
*
* RETURNS
*
* AUTHOR
*
*   Wlodzimierz ABX Skiba
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Aug 2003 : Creation.
*   Jan 2004 : Added exception handling to allow cleanup on error [CJC]
*
******************************************************************************/

void Write (OStream *file, const Image *image, const ImageWriteOptions& options)
{
    int             width = image->GetWidth();
    int             height = image->GetHeight();
    int             pad = (4 - ((width * 3) % 4)) & 0x03 ;
    bool            alpha = image->HasTransparency() && options.AlphaIsEnabled();
    unsigned int    r ;
    unsigned int    g ;
    unsigned int    b ;
    unsigned int    a ;
    GammaCurvePtr   gamma;
    DitherStrategy& dither = *options.ditherStrategy;

    // BMP files used to have no clearly defined gamma by default, but a Microsoft recommendation exists to assume sRGB.
    gamma = options.GetTranscodingGammaCurve(SRGBGammaCurve::Get());

    // TODO ALPHA - check if BMP should really keep presuming non-premultiplied alpha
    // We presume non-premultiplied alpha, unless the user overrides
    // (e.g. to handle a non-compliant file).
    bool premul = options.AlphaIsPremultiplied(false);

    int count = (width * (alpha ? 32 : 24) + 31) / 32 * 4 * height;

    file->Write_Byte('B');
    file->Write_Byte('M');
    Write_Long (file, 14 + 40 + count) ;
    Write_Short (file, 0) ;
    Write_Short (file, 0) ;
    Write_Long (file, 14 + 40) ;
    Write_Long (file, 40) ;
    Write_Long (file, width) ;
    Write_Long (file, height) ;
    Write_Short (file, 1) ;
    Write_Short (file, alpha ? 32 : 24) ;
    Write_Long (file, BI_RGB) ;
    Write_Long (file, count) ;
    Write_Long (file, 0) ;
    Write_Long (file, 0) ;
    Write_Long (file, 0) ;
    Write_Long (file, 0) ;

    for (int y = height - 1 ; y >= 0 ; y--)
    {
        for (int x = 0 ; x < width ; x++)
        {
            if (alpha)
                GetEncodedRGBAValue (image, x, y, gamma, 255, r, g, b, a, dither, premul);
            else
                GetEncodedRGBValue (image, x, y, gamma, 255, r, g, b, dither) ;
            file->Write_Byte((unsigned char) b);
            file->Write_Byte((unsigned char) g);
            file->Write_Byte((unsigned char) r);
            if (alpha)
                file->Write_Byte((unsigned char) a);
        }
        if (!alpha)
            for (int i = 0 ; i < pad; i++)
                file->Write_Byte((unsigned char) 0);
    }

    if (!*file)
        throw POV_EXCEPTION(kFileDataErr, "Error writing to BMP file") ;
}

Image *Read (IStream *file, const ImageReadOptions& options)
{
    unsigned file_width, file_height;
    unsigned file_depth, file_colors;
    unsigned data_location, planes, compression;
    unsigned info;
    Image *image = nullptr;

    // BMP files used to have no clearly defined gamma by default, but a Microsoft recommendation exists to assume sRGB.
    // Since ~1995, a header extension (BITMAPV4HEADER) with gamma metadata exists, which could be used if present.
    // However, as of now (2009), such information seems to be rarely included, if at all. (Same goes for BITMAPV5HEADER,
    // which could include a full colorimetric profile.)
    GammaCurvePtr gamma;
    if (options.gammacorrect)
    {
        if (options.defaultGamma)
            gamma = TranscodingGammaCurve::Get(options.workingGamma, options.defaultGamma);
        else
            gamma = TranscodingGammaCurve::Get(options.workingGamma, SRGBGammaCurve::Get());
    }

    // TODO ALPHA - check if BMP should really keep presuming non-premultiplied alpha
    // We presume non-premultiplied alpha, so that's the preferred mode to use for the image container unless the user overrides
    // (e.g. to handle a non-compliant file).
    bool premul = false;
    if (options.premultipliedOverride)
        premul = options.premultiplied;

    if ((file->Read_Byte () != 'B') || (file->Read_Byte () !='M'))
        throw POV_EXCEPTION(kFileDataErr, "Error reading magic number of BMP image");

    // skip file size and reserved fields
    Skip (file, 8) ;
    data_location = Read_Long (file) ;

    // read properties
    if ((info = Read_Long (file)) != WIN_OS2_OLD)
    {
        file_width = Read_Long (file) ;
        file_height = Read_Long (file) ;
        planes = Read_Short (file) ;
        file_depth = Read_Short (file) ;
        compression = Read_Long (file) ;
        Skip (file, 12) ; // skip image size in bytes, H&V pixels per meter
        file_colors = Read_Long (file) ;
        Skip (file, 4) ; // skip needed colors
    }
    else  /* info == WIN_OS2_OLD */
    {
        file_width = Read_Short (file) ;
        file_height = Read_Short (file) ;
        planes = Read_Short (file) ;
        file_depth = Read_Short (file) ;
        compression = BI_RGB ;
        file_colors = 0 ;
    }

    bool has_alpha = file_depth == 32 ;

    /* do not allow other subtypes */
    if (((file_depth!=1) && (file_depth!=4) && (file_depth!=8) && (file_depth!=24) && (file_depth!=32)) ||
        (planes!=1) || (compression>BI_RLE4) ||
        (((file_depth==1) || (file_depth==24) || (file_depth==32)) && (compression!=BI_RGB)) ||
        ((file_depth==4) && (compression==BI_RLE8)) ||
        ((file_depth==8) && (compression==BI_RLE4)))
        throw POV_EXCEPTION(kFileDataErr, "Invalid or unsupported BMP file");

    /* seek to colormap */
    if (info != WIN_OS2_OLD)
        Skip (file, info - 40) ;

    if (file_depth < 24)
    {
        int color_map_length = file_colors ? file_colors : 1<<file_depth ;
        std::vector<Image::RGBMapEntry> colormap ;
        Image::RGBMapEntry entry;

        for (int i=0; i<color_map_length; i++)
        {
            entry.blue  = IntDecode(gamma, file->Read_Byte (), 255);
            entry.green = IntDecode(gamma, file->Read_Byte (), 255);
            entry.red   = IntDecode(gamma, file->Read_Byte (), 255);
            if (info != WIN_OS2_OLD)
                file->Read_Byte() ;
            colormap.push_back (entry) ;
        }
        gamma.reset(); // gamma has been taken care of by transforming the color table.

        if (file->eof ())
            throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF while reading BMP file");

        image = Image::Create (file_width, file_height, ImageDataType::Colour_Map, colormap) ;
        image->SetPremultiplied(premul); // specify whether the color map data has premultiplied alpha

        switch (file_depth)
        {
            case 1:
                Read_BMP_1b(image, *file, file_width, file_height);
                break;
            case 4:
                switch(compression)
                {
                    case BI_RGB:
                        Read_BMP_4b_RGB(image, *file, file_width, file_height);
                        break;
                    case BI_RLE4:
                        Read_BMP_4b_RLE(image, *file, file_width, file_height);
                        break;
                    default:
                        throw POV_EXCEPTION(kFileDataErr, "Unknown compression scheme in BMP file");
                }
                break;
            case 8:
                switch(compression)
                {
                    case BI_RGB:
                        Read_BMP_8b_RGB(image, *file, file_width, file_height);
                        break;
                    case BI_RLE8:
                        Read_BMP_8b_RLE(image, *file, file_width, file_height);
                        break;
                    default:
                        throw POV_EXCEPTION(kFileDataErr, "Unknown compression scheme in BMP file");
                }
                break;
            default:
                throw POV_EXCEPTION(kFileDataErr, "Unknown depth in BMP file");
        }
    }
    else
    {
        /* includes update from Stefan Maierhofer for 32bit */
        ImageDataType imagetype = options.itype ;
        if (imagetype == ImageDataType::Undefined)
            imagetype = Image::GetImageDataType(8, 3, has_alpha, gamma);
        image = Image::Create (file_width, file_height, imagetype) ;
        image->SetPremultiplied(premul); // set desired storage mode regarding alpha premultiplication
        image->TryDeferDecoding(gamma, 255); // try to have gamma adjustment being deferred until image evaluation.

        int pad = has_alpha ? 0 : (4 - ((file_width * 3) % 4)) & 0x03 ;
        unsigned int a = 255 ; // value to use for files that don't have an alpha channel (full opacity)

        for (int y = file_height - 1 ; y >= 0 ; y--)
        {
            for (int x = 0 ; x < file_width ; x++)
            {
                unsigned int b = Read_Safe_Char (*file);
                unsigned int g = Read_Safe_Char (*file);
                unsigned int r = Read_Safe_Char (*file);
                if (has_alpha)
                    a = Read_Safe_Char (*file);
                SetEncodedRGBAValue (image, x, y, gamma, 255, r, g, b, a, premul);
            }
            if (pad && !Skip (file, pad))
                throw POV_EXCEPTION(kFileDataErr, "Error reading data from BMP image.") ;
        }
    }

    return (image) ;

}

}
// end of namespace Bmp

}
// end of namespace pov_base
