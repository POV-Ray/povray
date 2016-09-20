//******************************************************************************
///
/// @file base/image/hdr.cpp
///
/// Implementation of Radiance RGBE High Dynamic Range (HDR) image file
/// handling.
///
/// @author Christopher Cason
/// @author Christoph Lipka
/// @author Based on MegaPOV HDR code written by Mael and Christoph Hormann
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/image/hdr.h"

// Standard C++ header files
#include <string>

// Boost header files
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

// POV-Ray base header files
#include "base/fileinputoutput.h"
#include "base/types.h"
#include "base/image/metadata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

namespace HDR
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define  MINELEN        8       /* minimum scanline length for encoding */
#define  MAXELEN        0x7fff  /* maximum scanline length for encoding */
#define  MINRUN         4       /* minimum run length */

/*****************************************************************************
* Local typedefs
******************************************************************************/

struct Messages
{
    vector<string> warnings;
    string error;
};

typedef unsigned char RGBE[4]; // red, green, blue, exponent

void GetRGBE(RGBE rgbe, const Image *image, int col, int row, const GammaCurvePtr& gamma, DitherHandler* dither);
void SetRGBE(const unsigned char *scanline, Image *image, int row, int width, const GammaCurvePtr& gamma);
void ReadOldLine(unsigned char *scanline, int width, IStream *file);

/*****************************************************************************
* Code
******************************************************************************/
void GetRGBE(RGBE rgbe, const Image *image, int col, int row, const GammaCurvePtr& gamma, DitherHandler* dh)
{
    DitherHandler::OffsetInfo linOff, encOff;
    dh->getOffset(col,row,linOff,encOff);

    RGBColour rgbColour;
    GetEncodedRGBValue(image, col, row, gamma, rgbColour);

    rgbColour += linOff.getRGB();
    RadianceHDRColour rgbeColour(rgbColour,encOff.getRGB());
    for (int i = 0; i < 4; i ++)
        rgbe[i] = (*rgbeColour)[i];

    rgbColour -= RGBColour(rgbeColour);
    linOff.setRGB(rgbColour);
    dh->setError(col,row,linOff);
}

void SetRGBE(const unsigned char *scanline, Image *image, int row, int width, const GammaCurvePtr& gamma)
{
    for(int i = 0; i < width; i++)
    {
        RadianceHDRColour rgbeColour;
        for (int j = 0; j < 4; j ++)
            (*rgbeColour)[j] = scanline[j];
        SetEncodedRGBValue(image, i, row, gamma, RGBColour(rgbeColour));
        scanline += 4;
    }
}

void ReadOldLine(unsigned char *scanline, int width, IStream *file)
{
    int rshift = 0;
    unsigned char b;

    while(width > 0)
    {
        scanline[0] = file->Read_Byte();
        scanline[1] = file->Read_Byte();
        scanline[2] = file->Read_Byte();

        // NB EOF won't be set at this point even if the last read obtained the
        // final byte in the file (we need to read another byte for that to happen).
        if(*file == false)
            throw POV_EXCEPTION(kFileDataErr, "Invalid HDR file (unexpected EOF)");

        if(!file->Read_Byte(b))
            return;

        scanline[3] = b;

        if((scanline[0] == 1) && (scanline[1] == 1) && (scanline[2] == 1))
        {
            for(int i = scanline[3] << rshift; i > 0; i--)
            {
                memcpy(scanline, scanline - 4, 4);
                scanline += 4;
                width--;
            }
            rshift += 8;
        }
        else
        {
            scanline += 4;
            width--;
            rshift = 0;
        }
    }
}

Image *Read(IStream *file, const Image::ReadOptions& options)
{
    char line[2048];
    char *s;
    char s1[3];
    char s2[3];
    unsigned char b;
    unsigned char val;
    float e;
    float exposure = 1.0;
    unsigned int width;
    unsigned int height;
    Image *image = NULL;
    Image::ImageDataType imagetype = options.itype;

    // Radiance HDR files store linear color values by default, so never convert unless the user overrides
    // (e.g. to handle a non-compliant file).
    GammaCurvePtr gamma;
    if (options.gammacorrect)
    {
        if (options.gammaOverride)
            gamma = TranscodingGammaCurve::Get(options.workingGamma, options.defaultGamma);
        else
            gamma = TranscodingGammaCurve::Get(options.workingGamma, NeutralGammaCurve::Get());
    }

    while(*file)
    {
        if((file->getline(line, sizeof(line)) == false) || (line[0] == '-') || (line[0] == '+'))
            break;

        // TODO: what do we do with exposure?
        if(strncmp(line, "EXPOSURE", 8) == 0)
        {
            if((s = strchr(line, '=')) != NULL)
            {
                if(sscanf(s + 1, "%f", &e) == 1)
                    exposure *= e;
            }
        }
    }

    if(sscanf(line, "%2[+-XY] %u %2[+-XY] %u\n", s1, &height, s2, &width) != 4)
        throw POV_EXCEPTION(kFileDataErr, "Bad HDR file header");

    if(imagetype == Image::Undefined)
        imagetype = Image::RGBFT_Float;

    image = Image::Create(width, height, imagetype);
    // NB: HDR files don't use alpha, so premultiplied vs. non-premultiplied is not an issue

    boost::scoped_array<unsigned char> scanline(new unsigned char[4 * width]);
    for(int row = 0; row < height; row++)
    {
        // determine scanline type
        if((width < MINELEN) | (width > MAXELEN))
        {
            ReadOldLine(scanline.get(), width, file);
            SetRGBE(scanline.get(), image, row, width, gamma);
            continue;
        }

        if(file->Read_Byte(b) == false)
            throw POV_EXCEPTION(kFileDataErr, "Incomplete HDR file");

        if(b != 2)
        {
            file->UnRead_Byte(b);

            ReadOldLine(scanline.get(), width, file);
            SetRGBE(scanline.get(), image, row, width, gamma);
            continue;
        }

        scanline[1] = file->Read_Byte();
        scanline[2] = file->Read_Byte();

        if(file->Read_Byte(b) == false)
            throw POV_EXCEPTION(kFileDataErr, "Incomplete or invalid HDR file");

        if((scanline[1] != 2) || ((scanline[2] & 128) != 0))
        {
            scanline[0] = 2;
            scanline[3] = b;

            ReadOldLine(scanline.get() + 4, width - 1, file);
            SetRGBE(scanline.get(), image, row, width, gamma);
            continue;
        }

        if((((int) scanline[2] << 8) | b) != width)
            throw POV_EXCEPTION(kFileDataErr, "Invalid HDR file (length mismatch)");

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < width; )
            {
                if(file->Read_Byte(b) == false)
                    throw POV_EXCEPTION(kFileDataErr, "Invalid HDR file (unexpected EOF)");

                if(b > 128)
                {
                    // run
                    b &= 127;

                    if(file->Read_Byte(val) == false)
                        throw POV_EXCEPTION(kFileDataErr, "Invalid HDR file (unexpected EOF)");

                    while(b--)
                        scanline[j++ * 4 + i] = (unsigned char) val;
                }
                else
                {
                    while(b--)
                    {
                        if(file->Read_Byte(val) == false)
                            throw POV_EXCEPTION(kFileDataErr, "Invalid HDR file (unexpected EOF)");

                        scanline[j++ * 4 + i] = (unsigned char) val;
                    }
                }
            }
        }

        SetRGBE(scanline.get(), image, row, width, gamma);
    }

    return image;
}

void Write(OStream *file, const Image *image, const Image::WriteOptions& options)
{
    int width = image->GetWidth();
    int height = image->GetHeight();
    int cnt = 1;
    int c2;
    RGBE rgbe;
    GammaCurvePtr gamma = TranscodingGammaCurve::Get(options.workingGamma, NeutralGammaCurve::Get());
    DitherHandler* dither = options.dither.get();

    Metadata meta;
    file->printf("#?RADIANCE\n");
    file->printf("SOFTWARE=%s\n", meta.getSoftware().c_str());
    file->printf("CREATION_TIME=%s\n" ,meta.getDateTime().c_str());
    if (!meta.getComment1().empty())
        file->printf("COMMENT=%s\n", meta.getComment1().c_str());
    if (!meta.getComment2().empty())
        file->printf("COMMENT=%s\n", meta.getComment2().c_str());
    if (!meta.getComment3().empty())
        file->printf("COMMENT=%s\n", meta.getComment3().c_str());
    if (!meta.getComment4().empty())
        file->printf("COMMENT=%s\n", meta.getComment4().c_str());

    file->printf("FORMAT=32-bit_rle_rgbe\n");
    file->printf("\n");
    file->printf("-Y %d +X %d\n", height, width);

    boost::scoped_array<RGBE> scanline(new RGBE[width]);

    for(int row = 0; row < height; row++)
    {
        if((width < MINELEN) | (width > MAXELEN))
        {
            for(int col = 0; col < width; col++)
            {
                GetRGBE(rgbe, image, col, row, gamma, dither);

                if(file->write(&rgbe, sizeof(RGBE)) == false)
                    throw POV_EXCEPTION(kFileDataErr, "Failed to write data to HDR file");
            }
        }
        else
        {
            // put magic header
            file->Write_Byte(2);
            file->Write_Byte(2);
            file->Write_Byte(width >> 8);
            file->Write_Byte(width & 255);

            // convert pixels
            for(int col = 0; col < width; col++)
                GetRGBE(scanline[col], image, col, row, gamma, dither);

            // put components seperately
            for(int i = 0; i < 4; i++)
            {
                for(int col = 0; col < width; col += cnt)
                {
                    int beg = 0;

                    // find next run
                    for(beg = col; beg < width; beg += cnt)
                    {
                        cnt = 1;
                        while((cnt < 127) && (beg + cnt < width) && (scanline[beg + cnt][i] == scanline[beg][i]))
                            cnt++;

                        // long enough ?
                        if(cnt >= MINRUN)
                            break;
                    }

                    if(beg - col > 1 && beg - col < MINRUN)
                    {
                        c2 = col + 1;
                        while(scanline[c2++][i] == scanline[col][i])
                        {
                            if(c2 == beg)
                            {
                                // short run
                                file->Write_Byte(128 + beg - col);

                                if(file->Write_Byte(scanline[col][i]) == false)
                                    throw POV_EXCEPTION(kFileDataErr, "Failed to write data to HDR file");

                                col = beg;
                                break;
                            }
                        }
                    }
                    while(col < beg)
                    {
                        // write non-run
                        if((c2 = beg - col) > 128)
                            c2 = 128;

                        file->Write_Byte(c2);

                        while(c2--)
                        {
                            if(file->Write_Byte(scanline[col++][i]) == false)
                                throw POV_EXCEPTION(kFileDataErr, "Failed to write data to HDR file");
                        }
                    }
                    if(cnt >= MINRUN)
                    {
                        // write run
                        file->Write_Byte(128 + cnt);

                        if(file->Write_Byte(scanline[beg][i]) == false)
                            throw POV_EXCEPTION(kFileDataErr, "Failed to write data to HDR file");
                    }
                    else
                        cnt = 0;
                }
            }
        }
    }
}

} // end of namespace HDR

} // end of namespace pov_base

