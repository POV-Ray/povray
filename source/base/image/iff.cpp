//******************************************************************************
///
/// @file base/image/iff.cpp
///
/// Implementation of Electronic Arts Interleaved Bitmap (IFF-ILBM) image file
/// handling.
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
#include "base/image/iff.h"

// Standard C++ header files
#include <vector>

// Boost header files
#include <boost/scoped_array.hpp>

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

namespace Iff
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define FORM 0x464f524dL
#define ILBM 0x494c424dL
#define BMHD 0x424d4844L
#define CAMG 0x43414d47L
#define CMAP 0x434d4150L
#define BODY 0x424f4459L
#define CMPNONE 0

#define HAM 0x800

/*****************************************************************************
* Type definitions
******************************************************************************/

typedef struct Chunk_Header_Struct
{
    int name;
    int size;
} CHUNK_HEADER ;

/*****************************************************************************
* Static functions
******************************************************************************/

static int read_byte(IStream *file)
{
    int c;

    if ((c = file->Read_Byte()) == EOF)
        throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF while reading IFF-ILBM file");
    return (c);
}

static int read_word(IStream *file)
{
    int result = read_byte(file) * 256;
    result += read_byte(file);
    return (result);
}

static long read_long(IStream *file)
{
    long result = 0;
    for (int i = 0; i < 4; i++)
        result = (result << 8) + read_byte(file);
    return (result);
}

Image *Read (IStream *file, const Image::ReadOptions& options)
{
    int                             nPlanes = 0;
    int                             compression = 0;
    int                             mask;
    int                             byte_index;
    int                             count;
    int                             viewmodes=0;
    int                             Previous_Red=0;
    int                             Previous_Green=0;
    int                             Previous_Blue=0;
    int                             colourmap_size = 0;
    int                             width = -1;
    int                             height = -1;
    Image                           *image = NULL;
    CHUNK_HEADER                    Chunk_Header;
    unsigned int                    r;
    unsigned int                    g;
    unsigned int                    b;
    unsigned long                   creg;
    Image::RGBMapEntry              entry;
    vector<Image::RGBMapEntry>      colormap;

    while (true)
    {
        Chunk_Header.name = read_long(file);
        Chunk_Header.size = read_long(file);

        switch (IFF_SWITCH_CAST Chunk_Header.name)
        {
            case FORM:
                if (read_long(file) != ILBM)
                    throw POV_EXCEPTION(kFileDataErr, "Expected ILBM while reading IFF-ILBM file");
                break;

            case BMHD:
                width = read_word(file);
                height = read_word(file);
                read_word(file); /* x position ignored */
                read_word(file); /* y position ignored */
                nPlanes = read_byte(file);
                colourmap_size = 1 << nPlanes;
                read_byte(file); /* masking ignored */
                compression = read_byte(file); /* masking ignored */
                read_byte(file); /* pad */
                read_word(file); /* Transparent colour ignored */
                read_word(file); /* Aspect ratio ignored */
                read_word(file); /* page width ignored */
                read_word(file); /* page height ignored */
                break;

            case CAMG:
                viewmodes = (int)read_long(file);  /* Viewmodes */
                if (viewmodes & HAM)
                    colourmap_size = 16;
                break;

            case CMAP:
                colourmap_size = (int) Chunk_Header.size / 3;
                Previous_Red = read_byte(file);
                Previous_Green = read_byte(file);
                Previous_Blue = read_byte(file);
                entry.red = Previous_Red / 255.0f;
                entry.green = Previous_Green / 255.0f;
                entry.blue = Previous_Blue / 255.0f;
                colormap.push_back (entry);
                for (int i = 1; i < colourmap_size; i++)
                {
                    entry.red = read_byte(file) / 255.0f;
                    entry.green = read_byte(file) / 255.0f;
                    entry.blue = read_byte(file) / 255.0f;
                    colormap.push_back (entry);
                    // TODO FIXME - gamma!
                }

                for (int i = colourmap_size * 3; (long)i < Chunk_Header.size; i++)
                    read_byte(file);

                break;

            case BODY:
                if (width > 0 && height > 0)
                {
                    Image::ImageDataType imagetype = options.itype;
                    if (imagetype == Image::Undefined)
                        imagetype = ((viewmodes & HAM) != 0 || nPlanes == 24) ? Image::RGB_Int8 : Image::Colour_Map;
                    if ((viewmodes & HAM) != 0 || nPlanes == 24)
                        image = Image::Create (width, height, imagetype);
                    else
                        image = Image::Create (width, height, imagetype, colormap);
                    // NB: IFF-ILBM files don't use alpha, so premultiplied vs. non-premultiplied is not an issue

                    int rowlen = ((width + 15) / 16) * 2 ;
                    boost::scoped_array<unsigned char> row_bytes (new unsigned char [nPlanes * rowlen]);

                    for (int row = 0; row < height; row++)
                    {
                        for (int plane = 0; plane < nPlanes; plane++)
                        {
                            if (compression != CMPNONE)
                            {
                                int nBytes = 0;

                                while (nBytes != ((width + 15) / 16) * 2)
                                {
                                    unsigned char c = read_byte(file);

                                    if ((c >= 0) && (c <= 127))
                                    {
                                        for (int k = 0; k <= c; k++)
                                            row_bytes[plane * rowlen + nBytes++] = (unsigned char)read_byte(file);
                                    }
                                    else
                                    {
                                        if ((c >= 129) && (c <= 255))
                                        {
                                            count = 257 - c;
                                            c = read_byte(file);
                                            for (int k = 0; k < count; k++)
                                                row_bytes[plane * rowlen + nBytes++] = (unsigned char)c;
                                        }
                                    }
                                }
                            }
                            else
                                for (int k = 0; k < (((width + 15) / 16) * 2); k++)
                                    row_bytes[plane * rowlen + k] = (unsigned char)read_byte(file);
                        }

                        mask = 0x80;
                        byte_index = 0;

                        for (int col = 0; col < width; col++)
                        {
                            creg = 0;

                            for (int plane = nPlanes - 1; plane >= 0; plane--)
                            {
                                creg *= 2;
                                if (row_bytes[plane * rowlen + byte_index] & mask)
                                    creg++;
                            }

                            if (viewmodes & HAM)
                            {
                                switch ((int)(creg >> 4))
                                {
                                    case 0:
                                        Previous_Red = r = (unsigned char)(colormap[creg].red*255.0);
                                        Previous_Green = g = (unsigned char)(colormap[creg].green*255.0);
                                        Previous_Blue = b = (unsigned char)(colormap[creg].blue*255.0);
                                        break;

                                    case 1:
                                        r = (unsigned char)Previous_Red;
                                        g = (unsigned char)Previous_Green;
                                        Previous_Blue = b = (unsigned char)(((creg & 0xf) << 4) + (creg & 0xf));
                                        break;

                                    case 2:
                                        Previous_Red = r = (unsigned char)(((creg & 0xf) << 4) + (creg & 0xf));
                                        g = (unsigned char)Previous_Green;
                                        b = (unsigned char)Previous_Blue;
                                        break;

                                    case 3:
                                        r = (unsigned char)Previous_Red;
                                        Previous_Green = g = (unsigned char)(((creg & 0xf) << 4) + (creg & 0xf));
                                        b = (unsigned char)Previous_Blue;
                                        break;

                                    default:
                                        throw POV_EXCEPTION(kFileDataErr, "Invalid data in IFF-ILBM file");
                                }
                                image->SetRGBValue (col, row, r, g, b); // TODO FIXME - gamma!
                            }
                            else
                            {
                                if (nPlanes == 24)
                                {
                                    r = (unsigned char)((creg >> 16) & 0xFF);
                                    g = (unsigned char)((creg >> 8) & 0xFF);
                                    b = (unsigned char)(creg & 0xFF);
                                    image->SetRGBValue (col, row, r, g, b); // TODO FIXME - gamma!
                                }
                                else
                                {
                                    if (creg >= colormap.size())
                                        throw POV_EXCEPTION(kFileDataErr, "IFF-ILBM color out of range in image");
                                    image->SetIndexedValue (col, row, creg);
                                }
                            }

                            mask >>= 1;

                            if (mask == 0)
                            {
                                mask = 0x80;
                                byte_index++;
                            }
                        }
                    }
                }
                else
                    throw POV_EXCEPTION(kFileDataErr, "Invalid IFF-ILBM file");
                break;

            default:
                for (int i = 0; (long)i < Chunk_Header.size; i++)
                    if (file->Read_Byte() == EOF)
                        throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF while reading IFF-ILBM file");
                break;
        }
    }
    return (image);
}

} // end of namespace Iff

}

