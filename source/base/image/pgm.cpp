//******************************************************************************
///
/// @file base/image/pgm.cpp
///
/// Implementation of NetPBM Portable Graymap (PGM) image file reading.
///
/// This module contains the code to read the PGM file format format according
/// to NetPBM specs (http://netpbm.sourceforge.net/doc/):
///
/// Both ASCII and binary files are supported ('P2' and 'P5')
/// in 8 and 16 bit color depth.
///
/// @note
///     PGM writing is implemented in @ref ppm.cpp.
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
#include "base/image/pgm.h"

// C++ variants of standard C header files
#include <cctype>

// Standard C++ header files
#include <vector>

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

namespace Pgm
{

/*****************************************************************************
*
* FUNCTION
*
*  Read_ASCII_File_Number
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*    Christoph Hormann
*
* DESCRIPTION
*
*    Reads an integer number from an ASCII file skipping whitespaces
*
* CHANGES
*
*    August 2003 - Creation
*
******************************************************************************/

int Read_ASCII_File_Number(IStream *file)
{
    int value;
    int pos = 0;
    char buffer[50] = "";

    do
    {
        value = file->Read_Byte();
    } while (isspace(value));

    if (!isdigit (value))
        throw POV_EXCEPTION(kFileDataErr, "Invalid data in PGM/PPM file");

    buffer[pos] = (char)value;

    while (pos < 48)
    {
        if ((value = file->Read_Byte()) == EOF)
            break ;
        if (isspace(value))
            break ;
        if (!isdigit(value))
            throw POV_EXCEPTION(kFileDataErr, "Invalid data in PGM/PPM file");
        buffer[++pos] = (char)value;
    }

    buffer[pos+1] = '\0';
    value = atoi(buffer);
    return value;
}

/*****************************************************************************
*
* FUNCTION
*
*  Read_PGM_Image
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*    Christoph Hormann
*
* DESCRIPTION
*
*    Reads an PGM image file
*
* CHANGES
*
*    August 2003 - New implementation based on targa/png reading code
*
******************************************************************************/

// TODO: make sure we destroy the image if we throw an exception
Image *Read (IStream *file, const Image::ReadOptions& options)
{
    int                   width;
    int                   height;
    int                   data_hi; // not unsigned as it may need to hold EOF
    int                   data_lo; // not unsigned as it may need to hold EOF
    char                  line[1024];
    char                  *ptr;
    Image                 *image = NULL;
    unsigned int          depth;
    int                   v; // not unsigned as it may need to hold EOF
    unsigned char         header[2];

    // PGM files may or may not be gamma-encoded.
    GammaCurvePtr gamma;
    if (options.gammacorrect && options.defaultGamma)
        gamma = TranscodingGammaCurve::Get(options.workingGamma, options.defaultGamma);

    // --- Read Header ---
    if (!file->read(header, 2))
        throw POV_EXCEPTION(kFileDataErr, "Cannot read header of PGM image");

    if(header[0] != 'P')
        throw POV_EXCEPTION(kFileDataErr, "File is not in PGM format");

    if ((header[1] != '2') && (header[1] != '5'))
        throw POV_EXCEPTION(kFileDataErr, "File is not in PGM format");

    // TODO FIXME - Some valid PPM files may have a different header layout regarding line breaks

    do
    {
        file->getline (line, 1024);
        line[1023] = '\0';
        if ((ptr = strchr(line, '#')) != NULL)
            *ptr = '\0';  // remove comment
    } while (line[0]=='\0');  // read until line without comment from beginning

    // --- First: two numbers: width and height ---
    if (sscanf(line,"%d %d",&width, &height) != 2)
        throw POV_EXCEPTION(kFileDataErr, "Cannot read width and height from PGM image");

    if (width <= 0 || height <= 0)
        throw POV_EXCEPTION(kFileDataErr, "Invalid width or height read from PGM image");

    do
    {
        file->getline (line, 1024) ;
        line[1023] = '\0';
        if ((ptr = strchr(line, '#')) != NULL)
            *ptr = '\0';  // remove comment
    } while (line[0]=='\0');  // read until line without comment from beginning

    // --- Second: one number: color depth ---
    if (sscanf(line,"%u",&depth) != 1)
        throw POV_EXCEPTION(kFileDataErr, "Cannot read color depth from PGM image");

    if ((depth > 65535) || (depth < 1))
        throw POV_EXCEPTION(kFileDataErr, "Unsupported number of colors in PGM image");

    if (depth < 256)
    {
        // We'll be using an image container that provides for automatic decoding if possible - unless there's no such decoding to do.
        gamma = ScaledGammaCurve::GetByDecoding(255.0f/depth, gamma); // Note that we'll apply the scaling even if we don't officially gamma-correct
        Image::ImageDataType imagetype = options.itype;
        if (imagetype == Image::Undefined)
            imagetype = ( GammaCurve::IsNeutral(gamma) ? Image::Gray_Int8 : Image::Gray_Gamma8 );
        image = Image::Create (width, height, imagetype) ;
        // NB: PGM files don't use alpha, so premultiplied vs. non-premultiplied is not an issue
        image->TryDeferDecoding(gamma, 255); // try to have gamma adjustment being deferred until image evaluation.

        for (int i = 0; i < height; i++)
        {
            if (header[1] == '2') // --- ASCII PGM file (type 2) ---
            {
                for (int x = 0; x < width; x++)
                {
                    v = Read_ASCII_File_Number(file);
                    SetEncodedGrayValue (image, x, i, gamma, 255, v) ;
                }
            }
            else                  // --- binary PGM file (type 5) ---
            {
                for (int x = 0; x < width; x++)
                {
                    if ((v = file->Read_Byte ()) == EOF)
                        throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PGM file");
                    SetEncodedGrayValue (image, x, i, gamma, 255, v) ;
                }
            }
        }
    }
    else // --- 16 bit PGM (binary or ASCII) ---
    {
        // We'll be using an image container that provides for automatic decoding if possible - unless there's no such decoding to do.
        gamma = ScaledGammaCurve::GetByDecoding(65535.0f/depth, gamma); // Note that we'll apply the scaling even if we don't officially gamma-correct
        Image::ImageDataType imagetype = options.itype;
        if (imagetype == Image::Undefined)
            imagetype = ( GammaCurve::IsNeutral(gamma) ? Image::Gray_Int16 : Image::Gray_Gamma16 );
        image = Image::Create (width, height, imagetype) ;
        // NB: PGM files don't use alpha, so premultiplied vs. non-premultiplied is not an issue
        image->TryDeferDecoding(gamma, 65535); // try to have gamma adjustment being deferred until image evaluation.

        for (int i = 0; i < height; i++)
        {
            if (header[1] == '2') // --- ASCII PGM file (type 2) ---
            {
                for (int x = 0; x < width; x++)
                {
                    v = Read_ASCII_File_Number(file);
                    SetEncodedGrayValue (image, x, i, gamma, 65535, v);
                }
            }
            else                  // --- binary PGM file (type 5) ---
            {
                for (int x = 0; x < width; x++)
                {
                    if ((data_hi = file->Read_Byte ()) == EOF)
                        throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PGM file");
                    if ((data_lo = file->Read_Byte ()) == EOF)
                        throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PGM file");
                    v = (256*data_hi + data_lo);
                    SetEncodedGrayValue (image, x, i, gamma, 65535, v);
                }
            }
        }
    }
    return (image) ;
}

} // end of namespace Pgm

}

