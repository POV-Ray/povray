//******************************************************************************
///
/// @file base/image/tiff.cpp
///
/// Implementation of Tagged Image File Format (TIFF) image file handling.
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
#include "base/image/tiff_pov.h"

#ifndef LIBTIFF_MISSING

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>
#include <vector>

// Other 3rd party header files
extern "C"
{
#ifndef __STDC__
#define __STDC__ (1) // TODO - this is an ugly hack; check if it is really necessary
#endif
#ifndef AVOID_WIN32_FILEIO
#define AVOID_WIN32_FILEIO // this stops the tiff headers from pulling in windows.h on win32/64
#endif
#include <tiffio.h>
}

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/povassert.h"
#include "base/image/colourspace.h"
#include "base/image/encoding.h"
#include "base/image/image.h"

// this must be the last file included other than tiffio.h
#include "base/povdebug.h"

namespace pov_base
{

namespace Tiff
{

/* Do any of the entries in the color map contain values larger than 255? */
static int checkcmap(int n, const uint16* r, const uint16* g, const uint16* b)
{
    while(n-- > 0)
        if((*(r++) >= 256) || (*(g++) >= 256) || (*(b++) >= 256))
            return 16;
    return 8;
}

static void SuppressTIFFWarnings(const char *, const char *, va_list)
{
}

static tsize_t Tiff_Read(thandle_t fd, tdata_t buf, tsize_t size)
{
    IStream *file = reinterpret_cast<IStream *>(fd);

    if(!file->read(buf, size))
        return 0;

    return (tsize_t)(size);
}

static tsize_t Tiff_Write(thandle_t fd, tdata_t buf, tsize_t size)
{
    return (tsize_t)0;
}

static toff_t Tiff_Seek(thandle_t fd, toff_t off, int whence)
{
    IStream *file = reinterpret_cast<IStream *>(fd);

    file->seekg(off, whence);

    return (toff_t)file->tellg();
}

static int Tiff_Close(thandle_t fd)
{
    IStream *file = reinterpret_cast<IStream *>(fd);

    // we don't close the file here; it's done by the caller
    // delete file;

    return 0;
}

static toff_t Tiff_Size(thandle_t fd)
{
    IStream *file = reinterpret_cast<IStream *>(fd);
    unsigned int pos = 0;
    unsigned int len = 0;

    pos = file->tellg();
    file->seekg(0, IOBase::seek_end);
    len = file->tellg();
    file->seekg(pos, IOBase::seek_set);

    return (toff_t)len;
}

static int Tiff_Map(thandle_t, tdata_t *, toff_t *)
{
    return 0;
}

static void Tiff_Unmap(thandle_t, tdata_t, toff_t)
{
}

/*****************************************************************************
*
* FUNCTION      : Read_Tiff_Image
*
* ARGUMENTS     : IMAGE *Image; char *name;
*
* MODIFIED ARGS : Image
*
* RETURN VALUE  : none
*
* AUTHOR        : Alexander Enzmann
*
* DESCRIPTION
*
*   Reads a TIFF image into an RGB image buffer
*
* CHANGES
*
* New - 6/2000
*
******************************************************************************/

Image *Read (IStream *file, const ImageReadOptions& options)
{
    int                   nrow;
    int                   result = 0;
    long                  LineSize;
    TIFF                  *tif;
    Image                 *image ;
    uint16                BitsPerSample;
    uint16                BytesPerSample = 1;
    uint16                PhotometricInterpretation;
    uint16                SamplePerPixel;
    uint16                Orientation;
    uint16                ExtraSamples;
    uint16*               ExtraSampleInfo;
    uint32                RowsPerStrip;
    unsigned int          width;
    unsigned int          height;

    // TODO - TIFF files probably have some gamma info in them by default, but we're currently ignorant about that.
    // Until that is fixed, use whatever the user has chosen as default.
    GammaCurvePtr gamma;
    if (options.gammacorrect && options.defaultGamma)
        gamma = TranscodingGammaCurve::Get(options.workingGamma, options.defaultGamma);

    // Rather than have libTIFF complain about tags it doesn't understand,
    // we just suppress all the warnings.
    TIFFSetWarningHandler(SuppressTIFFWarnings);
    TIFFSetErrorHandler(SuppressTIFFWarnings);

    // Open and do initial processing
    tif = TIFFClientOpen("Dummy File Name", "r", file,
        Tiff_Read, Tiff_Write, Tiff_Seek, Tiff_Close,
        Tiff_Size, Tiff_Map, Tiff_Unmap);
    if (!tif)
        return nullptr;

    // Get basic information about the image
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample);
    TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &RowsPerStrip);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &PhotometricInterpretation);
    TIFFGetField(tif, TIFFTAG_ORIENTATION, &Orientation);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &SamplePerPixel);
    TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES, &ExtraSamples, &ExtraSampleInfo);

    // [CLi] TIFF provides alpha mode information in the TIFFTAG_EXTRASAMPLES field, so that's the preferred mode to use
    // for the image container unless the user overrides (e.g. to handle a non-compliant file).
    // If TIFFTAG_EXTRASAMPLES information is absent or inconclusive, we presume associated (= premultiplied) alpha.
    bool premul = true;
    if ((ExtraSamples > 0) && (ExtraSampleInfo[0] == EXTRASAMPLE_UNASSALPHA))
        premul = false;
    if (options.premultipliedOverride)
        premul = options.premultiplied;

    // don't support more than 16 bits per sample
    if (BitsPerSample == 16)
    {
        BytesPerSample = 2 ;
        options.warnings.push_back ("Warning: reading 16 bits/sample TIFF file; components crunched to 8");
    }

    LineSize = TIFFScanlineSize(tif);
    POV_IMAGE_ASSERT(SamplePerPixel == (int) (LineSize / width) / BytesPerSample);
    // SamplePerPixel = (int)(LineSize / width);

#if 0
    // For now we are ignoring the orientation of the image...
    switch (Orientation)
    {
    case ORIENTATION_TOPLEFT:
        break;
    case ORIENTATION_TOPRIGHT:
        break;
    case ORIENTATION_BOTRIGHT:
        break;
    case ORIENTATION_BOTLEFT:
        break;
    case ORIENTATION_LEFTTOP:
        break;
    case ORIENTATION_RIGHTTOP:
        break;
    case ORIENTATION_RIGHTBOT:
        break;
    case ORIENTATION_LEFTBOT:
        break;
    default:
        break;
    }
#endif

    //PhotometricInterpretation = 2 image is RGB
    //PhotometricInterpretation = 3 image have a color palette
    if ((PhotometricInterpretation == PHOTOMETRIC_PALETTE) && (TIFFIsTiled(tif) == 0))
    {
        uint16 *red, *green, *blue;

        //load the palette
        int cmap_len = (1 << BitsPerSample);

        TIFFGetField(tif, TIFFTAG_COLORMAP, &red, &green, &blue);

        std::vector<Image::RGBMapEntry> colormap;
        Image::RGBMapEntry entry;

        // I may be mistaken, but it appears that alpha/opacity information doesn't
        // appear in a Paletted Tiff image.  Well - if it does, it's not as easy to
        // get at as RGB.
        // Read the palette
        // Is the palette 16 or 8 bits ?
        if (checkcmap(cmap_len, red, green, blue) == 16)
        {
            for (int i=0;i<cmap_len;i++)
            {
                entry.red   = IntDecode(gamma, red[i],   65535);
                entry.green = IntDecode(gamma, green[i], 65535);
                entry.blue  = IntDecode(gamma, blue[i],  65535);
                colormap.push_back (entry);
            }
        }
        else
        {
            for (int i=0;i<cmap_len;i++)
            {
                entry.red   = IntDecode(gamma, red[i],   255);
                entry.green = IntDecode(gamma, green[i], 255);
                entry.blue  = IntDecode(gamma, blue[i],  255);
                colormap.push_back (entry);
            }
        }

        ImageDataType imagetype = options.itype;
        if (imagetype == ImageDataType::Undefined)
            imagetype = ImageDataType::Colour_Map;
        image = Image::Create (width, height, imagetype, colormap) ;
        image->SetPremultiplied(premul); // specify whether the color map data has premultiplied alpha

        std::unique_ptr<unsigned char[]> buf (new unsigned char [TIFFStripSize(tif)]);

        //read the tiff lines and save them in the image
        //with RGB mode, we have to change the order of the 3 samples RGB <=> BGR
        for (int row=0;row<height;row+=RowsPerStrip)
        {
            nrow = (row + (int)RowsPerStrip > height ? height - row : RowsPerStrip);
            TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, row, 0), buf.get(), nrow * LineSize);
            for (int l = 0, offset = 0; l < nrow ; l++, offset += LineSize)
                for (int x = 0 ; x < width ; x++)
                    image->SetIndexedValue (x, row+l, buf[offset+x]) ;
        }
    }
    else
    {
        // Allocate the row buffers for the image
        std::unique_ptr<uint32[]> buf (new uint32 [width * height]) ;

        ImageDataType imagetype = options.itype;
        if (imagetype == ImageDataType::Undefined)
            imagetype = Image::GetImageDataType(8, 3, true, gamma);
        image = Image::Create (width, height, imagetype) ;
        image->SetPremultiplied(premul); // set desired storage mode regarding alpha premultiplication
        image->TryDeferDecoding(gamma, 255); // try to have gamma adjustment being deferred until image evaluation.

        TIFFReadRGBAImage(tif, width, height, buf.get(), 0);
        uint32 abgr, *tbuf = buf.get();
        for (int i=height-1;i>=0;i--)
        {
            for (int j=0;j<width;j++)
            {
                abgr = *tbuf++;
                unsigned int b = (unsigned char)TIFFGetB(abgr);
                unsigned int g = (unsigned char)TIFFGetG(abgr);
                unsigned int r = (unsigned char)TIFFGetR(abgr);
                unsigned int a = (unsigned char)TIFFGetA(abgr);
                SetEncodedRGBAValue(image, j, i, gamma, 255, r, g, b, a, premul) ;
            }
        }
    }

    TIFFClose(tif);

    return (image) ;
}

}
// end of namespace Tiff

}
// end of namespace pov_base

#endif  // LIBTIFF_MISSING
