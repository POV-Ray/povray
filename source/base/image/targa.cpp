//******************************************************************************
///
/// @file base/image/targa.cpp
///
/// Implementation of Targa (TGA) image file handling.
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

/****************************************************************************
*
*  Explanation:
*
*  May 1994 : Support for 24-bit RLE Targa output files added: John Baily
*             and David Payne.
*
*  Jul 1994 : Resume trace support and minor algorithm fix (one more still
*             needed, see comments in Write_Targa_Line); resume will force
*             Targa format to match the original trace format -- the T or C
*             format flag is adjusted as necessary: Charles Marslett,
*
*  Jun 1995 : Added support for 32-bit Targa input and output files.
*             The alpha channel has a value of 0 for 100% transparency
*             and a value of 255 for 0% transparency. [DB]
*
*  Sep 2010 : Added metadata in Write, changing targa version from 1.0 to 2.0
*
*****************************************************************************/

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/image/targa.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <string>
#include <vector>

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

namespace Targa
{

using std::vector;

struct pix final
{
    unsigned int b;
    unsigned int g;
    unsigned int r;
    unsigned int a;
};
using Pixel = pix;

typedef char Targa_footer[26];
#define FOO_EXT_OFF 0
#define FOO_SIG_OFF 8
#define FOO_DOT_OFF 24
#define FOO_NUL_OFF 25

typedef char Targa_extension[495];
#define EXT_SIZE_OFF 0
#define EXT_SIZE_DIM 2
#define EXT_COMMENT_OFF 43
#define EXT_COMMENT_DIM 81
#define EXT_COMMENT_COUNT 4
#define EXT_DATE_OFF 367
#define EXT_DATE_DIM 12
#define EXT_SOFT_OFF 426
#define EXT_SOFT_DIM 41
#define EXT_VERSION_OFF 467
#define EXT_VERSION_DIM 3
#define EXT_SCAN_OFF 490
#define EXT_SCAN_DIM 4
#define EXT_ALPHA_OFF 494
#define EXT_ALPHA_DIM 1
#define EXT_GAMMA_OFF 478
#define EXT_PIXRATIO_OFF 474

static Pixel *GetPix (const Image *image, int x, int y, Pixel *pixel, const GammaCurvePtr& gamma, DitherStrategy& dither, bool premul)
{
    GetEncodedRGBAValue (image, x, y, gamma, 255, pixel->r, pixel->g, pixel->b, pixel->a, dither, premul);
    return (pixel);
}

static void PutPix (vector<unsigned char>& line, const pix *pixel, bool opaque)
{
    line.push_back (pixel->b);
    line.push_back (pixel->g);
    line.push_back (pixel->r);
    if (!opaque)
        line.push_back (pixel->a);
}

void Write (OStream *file, const Image *image, const ImageWriteOptions& options)
{
    pix                     current;
    pix                     next;
    bool                    opaque = options.AlphaIsEnabled();
    bool                    compress = (options.compression > 0);
    vector<unsigned char>   header;
    vector<unsigned char>   line;
    GammaCurvePtr           gamma;
    Metadata                meta;
    DitherStrategy&         dither = *options.ditherStrategy;

    // If the user does not specify gamma, we're defaulting to sRGB.
    gamma = options.GetTranscodingGammaCurve(SRGBGammaCurve::Get());

    // TODO ALPHA - check if TGA should really keep presuming non-premultiplied alpha
    // We presume non-premultiplied alpha, unless the user overrides
    // (e.g. to handle a non-compliant file).
    bool premul = options.AlphaIsPremultiplied(false);

    header.reserve (18);
    header.push_back (0);
    header.push_back (0);

    // byte 2 - targa file type (compressed/uncompressed)
    header.push_back (compress ? 10 : 2);

    // Byte 3 - Index of first color map entry LSB
    // Byte 4 - Index of first color map entry MSB
    // Byte 5 - Color map length LSB
    // Byte 6 - Color map legth MSB
    // Byte 7 - Color map size
    // x origin set to "First_Column" - Bytes 8, 9
    // y origin set to "First_Line"   - Bytes 10, 11
    for (int i = 0; i < 9; i++)
        header.push_back (0);

    // write width and height - Bytes 12 - 15
    unsigned int w = image->GetWidth();
    unsigned int h = image->GetHeight();
    header.push_back (w % 256);
    header.push_back (w / 256);
    header.push_back (h % 256);
    header.push_back (h / 256);

    if (!opaque)
    {
        header.push_back(32);    /* 32 bits/pixel (BGRA) */
        header.push_back(0x28);  /* Data starts at top left, 8 bits Alpha */
    }
    else
    {
        header.push_back(24);    /* 24 bits/pixel (BGR) */
        header.push_back(0x20);  /* Data starts at top left, 0 bits Alpha */
    }

    if (!file->write (&header[0], 18))
        throw POV_EXCEPTION(kFileDataErr, "header write failed for targa file") ;

    line.reserve (w * 4);
    if (compress)
    {
        // RLE compressed data

        for (int row = 0; row  < h; row++)
        {
            line.clear ();

            int llen = w;
            int startx = 0;
            int cnt = 1;
            int ptype = 0;
            bool writenow = false;

            GetPix(image, 0, row, &current, gamma, dither, premul);
            while (true)
            {
                if (startx + cnt < llen)
                    GetPix(image, startx + cnt, row, &next, gamma, dither, premul);
                else
                    next = current;
                if (memcmp (&current, &next, sizeof (pix)) == 0)
                {
                    if (ptype == 0)
                    {
                        cnt++;
                        if ((cnt >= 128) || ((startx + cnt) >= llen))
                            writenow = true;
                    }
                    else
                    {
                        cnt--;
                        writenow = true;
                    }
                }
                else
                {
                    if ((ptype == 1) || (cnt <= 1))
                    {
                        current = next;
                        ptype = 1;
                        cnt++;
                        if ((cnt >= 128) || ((startx + cnt) >= llen))
                            writenow = true;
                    }
                    else
                        writenow = true;
                }

                if (writenow)
                {
                    Pixel pixel;
                    /* This test SHOULD be unnecessary!  However, it isn't!  [CWM] */
                    if (startx + cnt > llen)
                        cnt = llen - startx;
                    if (ptype == 0)
                    {
                        line.push_back ((unsigned char) ((cnt - 1) | 0x80));
                        PutPix (line, &current, opaque);
                        current = next;
                    }
                    else
                    {
                        line.push_back ((unsigned char) cnt - 1);
                        for (int x = 0; x < cnt; x++)
                            PutPix(line, GetPix(image, startx + x, row, &pixel, gamma, dither, premul), opaque);
                    }
                    startx += cnt;
                    writenow = false;
                    ptype = 0;
                    cnt = 1;
                    if (!file->write (&line[0], line.size()))
                        throw POV_EXCEPTION(kFileDataErr, "write failed for targa file") ;
                    if (startx >= llen)
                        break;
                    line.clear ();
                }
            }
        }
    }
    else
    {
        // uncompressed data

        Pixel pixel;
        for (int row = 0; row < h; ++row)
        {
            line.clear ();
            for (int col = 0; col < w; ++col)
                PutPix(line, GetPix(image, col, row, &pixel, gamma, dither, premul), opaque);
            if (!file->write(&line[0], line.size()))
                throw POV_EXCEPTION(kFileDataErr, "row write failed for targa file") ;
        }
    }

    POV_OFF_T curpos;
    Targa_extension ext;
    Targa_footer foo;

    // A lot of "unused" get happy when filled with 0
    memset(ext,0,sizeof(ext));
    memset(foo,0,sizeof(foo));
    curpos = file->tellg();

    foo[FOO_EXT_OFF+3]= 0x0ff & (curpos >> 24);
    foo[FOO_EXT_OFF+2]= 0x0ff & (curpos >> 16);
    foo[FOO_EXT_OFF+1]= 0x0ff & (curpos >> 8);
    foo[FOO_EXT_OFF]= 0x0ff & (curpos);

    // fill signature, Reserved char & binary string terminator in one go
    sprintf(&foo[FOO_SIG_OFF],"TRUEVISION-XFILE.");

    // let's prepare the Extension area, then write Extension then footer
    ext[EXT_SIZE_OFF+1] = (unsigned char) ((495 >> 8) & 0xff);
    ext[EXT_SIZE_OFF] = (unsigned char) (495 & 0xff);
    std::string soft = meta.getSoftware().substr(0,EXT_SOFT_DIM-1);
    sprintf(&ext[EXT_SOFT_OFF],"%s",soft.c_str());

    std::string com = meta.getComment1().substr(0,EXT_COMMENT_DIM-1);
    if (!com.empty())
        sprintf(&ext[EXT_COMMENT_OFF],"%s",com.c_str());
    com = meta.getComment2().substr(0,EXT_COMMENT_DIM-1);
    if (!com.empty())
        sprintf(&ext[EXT_COMMENT_OFF+EXT_COMMENT_DIM],"%s",com.c_str());
    com = meta.getComment3().substr(0,EXT_COMMENT_DIM-1);
    if (!com.empty())
        sprintf(&ext[EXT_COMMENT_OFF+EXT_COMMENT_DIM*2],"%s",com.c_str());
    com = meta.getComment4().substr(0,EXT_COMMENT_DIM-1);
    if (!com.empty())
        sprintf(&ext[EXT_COMMENT_OFF+EXT_COMMENT_DIM*3],"%s",com.c_str());

    ext[EXT_DATE_OFF]    = meta.getMonth();
    ext[EXT_DATE_OFF+1]  = 0;
    ext[EXT_DATE_OFF+2]  = meta.getDay();
    ext[EXT_DATE_OFF+3]  = 0;
    ext[EXT_DATE_OFF+4]  = meta.getYear()%256;
    ext[EXT_DATE_OFF+5]  = meta.getYear()/256;
    ext[EXT_DATE_OFF+6]  = meta.getHour();
    ext[EXT_DATE_OFF+7]  = 0;
    ext[EXT_DATE_OFF+8]  = meta.getMin();
    ext[EXT_DATE_OFF+9]  = 0;
    ext[EXT_DATE_OFF+10] = meta.getSec();
    ext[EXT_DATE_OFF+11] = 0;

    // we do not use the version field, but All 0 is not good enough
    ext[EXT_VERSION_OFF]=0;
    ext[EXT_VERSION_OFF+1]=0;
    ext[EXT_VERSION_OFF+2]=' ';

    // scanline offset is 18 (as 4 bytes) as we did not fill image ID nor color map
    ext[EXT_SCAN_OFF] = 18;

    // alpha handling
    ext[EXT_ALPHA_OFF] = opaque ? 0:premul ?4:3;

    // gamma handling (range 0.0 to 10.0, on short, hard code a denominator to allow 10.0)
    ext[EXT_GAMMA_OFF] = (int)(6553.0 * gamma->ApproximateDecodingGamma()) % 256;
    ext[EXT_GAMMA_OFF+1] = (int)(6553.0 * gamma->ApproximateDecodingGamma()) / 256;
    ext[EXT_GAMMA_OFF+2] = (unsigned char) (6553 % 256);
    ext[EXT_GAMMA_OFF+3] = (unsigned char) (6553 / 256) ;
    file->write(ext,sizeof(ext));
    file->write(foo,sizeof(foo));
}

static void ConvertColor (Pixel *pixel, unsigned pixelsize, const unsigned char *bytes)
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a = 255; // default to solid colour

    switch (pixelsize)
    {
        case 8:
            r = g = b = bytes[0];
            break;

        case 15:
            r = ((bytes[1] & 0x7c) << 1);
            g = (((bytes[1] & 0x03) << 3) | ((bytes[0] & 0xe0) >> 5)) << 3;
            b = (bytes[0] & 0x1f) << 3;
            break;

        case 16:
            r = ((bytes[1] & 0x7c) << 1);
            g = (((bytes[1] & 0x03) << 3) | ((bytes[0] & 0xe0) >> 5)) << 3;
            b = (bytes[0] & 0x1f) << 3;
            a = bytes[1] & 0x80 ? 255 : 0;
            break;

        case 24:
            r = bytes[2];
            g = bytes[1];
            b = bytes[0];
            break;

        case 32:
            r = bytes[2];
            g = bytes[1];
            b = bytes[0];
            a = bytes[3];
            break;

        default:
            throw POV_EXCEPTION(kParamErr, "Bad pixelsize in targa color");
    }

    pixel->r = r;
    pixel->g = g;
    pixel->b = b;
    pixel->a = a;
}

static void ConvertColor (Image::RGBAMapEntry *pixel, unsigned pixelsize, const unsigned char *bytes, const GammaCurvePtr& gamma)
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a = 255; // default to solid colour

    switch (pixelsize)
    {
        case 8:
            r = g = b = bytes[0];
            break;

        case 15:
            r = ((bytes[1] & 0x7c) << 1);
            g = (((bytes[1] & 0x03) << 3) | ((bytes[0] & 0xe0) >> 5)) << 3;
            b = (bytes[0] & 0x1f) << 3;
            break;

        case 16:
            r = ((bytes[1] & 0x7c) << 1);
            g = (((bytes[1] & 0x03) << 3) | ((bytes[0] & 0xe0) >> 5)) << 3;
            b = (bytes[0] & 0x1f) << 3;
            a = bytes[1] & 0x80 ? 255 : 0;
            break;

        case 24:
            r = bytes[2];
            g = bytes[1];
            b = bytes[0];
            break;

        case 32:
            r = bytes[2];
            g = bytes[1];
            b = bytes[0];
            a = bytes[3];
            break;

        default:
            throw POV_EXCEPTION(kParamErr, "Bad pixelsize in targa color");
    }

    pixel->red   = IntDecode(gamma, r, 255);
    pixel->green = IntDecode(gamma, g, 255);
    pixel->blue  = IntDecode(gamma, b, 255);
    pixel->alpha = IntDecode(a, 255);
}

/*****************************************************************************
*
* FUNCTION
*
*   Read_Targa_Image
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Reads a Targa image into an RGB image buffer.  Handles 8, 16, 24, 32 bit
*   formats.  Raw or color mapped. Simple raster and RLE compressed pixel
*   encoding. Right side up or upside down orientations.
*
* CHANGES
*
*   Jun 1995 : Added code for 32 bit Targa files. [DB]
*
******************************************************************************/

Image *Read (IStream *file, const ImageReadOptions& options)
{
    int                   temp;
    int                   h;
    unsigned int          ftype;
    unsigned int          idlen;
    unsigned int          cmlen;
    unsigned int          cmsiz;
    unsigned int          cmsizB;
    unsigned int          psize;
    unsigned int          psizeB;
    unsigned int          orien;
    unsigned int          width;
    unsigned int          height;
    unsigned char         bytes[256];
    unsigned char         tgaheader[18];
    Pixel                 pixel;
    Image                 *image ;

    // TARGA files have no clearly defined gamma by default.
    // Since 1989, TARGA 2.0 defines gamma metadata which could be used if present.
    // However, as of now (2009), such information seems to be rarely included, if at all.
    // We're defaulting to sRGB because that's what we write, and the most common gamma appears to be 2.2
    GammaCurvePtr gamma;
    if (options.gammacorrect)
    {
        if (options.defaultGamma)
            gamma = TranscodingGammaCurve::Get(options.workingGamma, options.defaultGamma);
        else
            gamma = TranscodingGammaCurve::Get(options.workingGamma, SRGBGammaCurve::Get());
    }

    // TODO ALPHA - check if TGA should really keep presuming non-premultiplied alpha
    // We presume non-premultiplied alpha, so that's the preferred mode to use for the image container unless the user overrides
    // (e.g. to handle a non-compliant file).
    bool premul = false;
    if (options.premultipliedOverride)
        premul = options.premultiplied;

    if (!file->read (tgaheader, 18))
        throw POV_EXCEPTION(kFileDataErr, "Cannot read targa file header");

    /* Decipher the header information */
    idlen  = tgaheader[ 0];
    ftype  = tgaheader[ 2];
    cmlen  = tgaheader[ 5] + (tgaheader[ 6] << 8);
    cmsiz  = tgaheader[ 7];
    width  = tgaheader[12] + (tgaheader[13] << 8);
    height = tgaheader[14] + (tgaheader[15] << 8);
    psize  = tgaheader[16];
    orien  = tgaheader[17] & 0x20; /* Right side up ? */

    cmsizB = (cmsiz+7)/8;
    psizeB = (psize+7)/8;

    bool opaque = (psize <= 24) && (psize != 16);
    bool compressed = ftype == 9 || ftype == 10 || ftype == 11;

    /* Determine if this is a supported Targa type */
    if (ftype < 1 || ftype > 11 || (ftype > 3 && ftype < 9))
        throw POV_EXCEPTION(kFileDataErr, "Unsupported file type in targa image");
    if (psize != 8 && psize != 15 && psize != 16 && psize != 24 && psize != 32)
        throw POV_EXCEPTION(kFileDataErr, "Unsupported pixel size in targa image");
    if (cmlen > 256)
        throw POV_EXCEPTION(kFileDataErr, "Unsupported color map length in targa image");

    /* Skip over the picture ID information */
    if (idlen > 0)
        if (!file->read (bytes, idlen))
            throw POV_EXCEPTION(kFileDataErr, "Cannot read from targa image");

    /* Read in the the color map (if any) */
    if (cmlen > 0)
    {
        if (psize != 8)
            throw POV_EXCEPTION(kFileDataErr, "Unsupported color map bit depth in targa image");

        vector<Image::RGBAMapEntry> colormap ;
        Image::RGBAMapEntry entry;
        for (int i = 0; i < cmlen; i++)
        {
            for (int j = 0; j < cmsizB; j++)
            {
                if ((temp = file->Read_Byte ()) == EOF)
                    throw POV_EXCEPTION(kFileDataErr, "Cannot read color map from targa image");
                bytes[j] = (unsigned char) temp;
            }
            ConvertColor (&entry, cmsiz, bytes, gamma);
            colormap.push_back(entry);
        }
        ImageDataType imagetype = options.itype;
        if (imagetype == ImageDataType::Undefined)
            imagetype = ImageDataType::Colour_Map ;
        image = Image::Create (width, height, imagetype, colormap) ;
        image->SetPremultiplied(premul); // specify whether the color map data has premultiplied alpha
        gamma.reset(); // gamma has been taken care of by transforming the color table.
    }
    else
    {
        ImageDataType imagetype = options.itype;
        if (imagetype == ImageDataType::Undefined)
            imagetype = Image::GetImageDataType(8, 3, !opaque, gamma);
        image = Image::Create (width, height, imagetype) ;
        image->SetPremultiplied(premul); // set desired storage mode regarding alpha premultiplication
        image->TryDeferDecoding(gamma, 255); // try to have gamma adjustment being deferred until image evaluation.
    }

    if (compressed)
    {
        /* RLE compressed images */
        int x = 0;
        int y = 0;
        while (y < height)
        {
            int line = orien != 0 ? y : height - y - 1 ;

            /* Grab a header */
            if ((h = file->Read_Byte ()) == EOF)
                throw POV_EXCEPTION(kFileDataErr, "Cannot read data from targa image");
            if (h & 0x80)
            {
                h &= 0x7F;
                if (cmlen == 0)
                {
                    for (int k = 0; k < psizeB; k++)
                    {
                        if ((temp = file->Read_Byte ()) == EOF)
                            throw POV_EXCEPTION(kFileDataErr, "Cannot read data from targa image");
                        bytes[k] = (unsigned char) temp;
                    }
                    ConvertColor (&pixel, psize, bytes);
                    while (h-- >= 0)
                    {
                        SetEncodedRGBAValue (image, x, line, gamma, 255, pixel.r, pixel.g, pixel.b, pixel.a, premul);
                        if (++x == width)
                        {
                            y++;
                            x = 0;
                        }
                    }
                }
                else
                {
                    if ((temp = file->Read_Byte ()) == EOF)
                        throw POV_EXCEPTION(kFileDataErr, "Cannot read data from targa image");
                    if ((unsigned char) temp >= cmlen)
                        throw POV_EXCEPTION(kFileDataErr, "Invalid color map index in targa image");
                    while (h-- >= 0)
                    {
                        image->SetIndexedValue (x, line, (unsigned char) temp) ;
                        if (++x == width)
                        {
                            y++;
                            x = 0;
                        }
                    }
                }

            }
            else
            {
                /* Copy buffer */
                while (h-- >= 0)
                {
                    if (cmlen == 0)
                    {
                        for (int k = 0; k < psizeB; k++)
                        {
                            if ((temp = file->Read_Byte ()) == EOF)
                                throw POV_EXCEPTION(kFileDataErr, "Cannot read data from targa image");
                            bytes[k] = (unsigned char) temp;
                        }
                        ConvertColor (&pixel, psize, bytes);
                        SetEncodedRGBAValue (image, x, line, gamma, 255, pixel.r, pixel.g, pixel.b, pixel.a, premul);
                    }
                    else
                    {
                        if ((temp = file->Read_Byte ()) == EOF)
                            throw POV_EXCEPTION(kFileDataErr, "Cannot read data from targa image");
                        if ((unsigned char) temp >= cmlen)
                            throw POV_EXCEPTION(kFileDataErr, "Invalid color map index in targa image");
                        image->SetIndexedValue (x, line, (unsigned char) temp) ;
                    }

                    if (++x == width)
                    {
                        if (++y == height)
                            break ;
                        line = orien != 0 ? y : height - y - 1 ;
                        x = 0;
                    }
                }
            }
        }
    }
    else
    {
        /* Simple raster image file, read in all of the pixels */
        for (int y = 0; y < height; y++)
        {
            int line = orien != 0 ? y : height - y - 1 ;
            for (int x = 0; x < width; x++)
            {
                if (cmlen == 0)
                {
                    for (int z = 0; z < psizeB; z++)
                    {
                        if ((temp = file->Read_Byte ()) == EOF)
                            throw POV_EXCEPTION(kFileDataErr, "Cannot read data from targa image");
                        bytes[z] = (unsigned char) temp;
                    }
                    ConvertColor (&pixel, psize, bytes);
                    SetEncodedRGBAValue (image, x, line, gamma, 255, pixel.r, pixel.g, pixel.b, pixel.a, premul);
                }
                else
                {
                    if ((temp = file->Read_Byte ()) == EOF)
                        throw POV_EXCEPTION(kFileDataErr, "Cannot read data from targa image");
                    if ((unsigned char) temp >= cmlen)
                        throw POV_EXCEPTION(kFileDataErr, "Invalid color map index in targa image");
                    image->SetIndexedValue (x, line, (unsigned char) temp) ;
                }
            }
        }
    }

    return (image);
}

}
// end of namespace Targa

}
// end of namespace pov_base
