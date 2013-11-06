/*******************************************************************************
 * ppm.cpp
 *
 * This module contains the code to read and write the PPM file format.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/base/image/ppm.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

/****************************************************************************
*
*  PPM format according to NetPBM specs (http://netpbm.sourceforge.net/doc/):
*
*  This module implements read support for PPM image maps and
*  write support for PPM output.
*
*  For reading both ASCII and binary files are supported ('P3' and 'P6').
*
*  For writing we use binary files. OutputQuality > 8 leads to 16 bit files.
*  Special handling of Greyscale_Output=on -> 16 bit PGM files ('P5')
*  All formats supported for writing can now also be used in continued trace.
*
*****************************************************************************/

#include <vector>
#include <ctype.h>

// configbase.h must always be the first POV file included within base *.cpp files
#include "base/configbase.h"
#include "base/image/image.h"
#include "base/image/pgm.h"
#include "base/image/ppm.h"
#include "base/types.h"

#include "metadata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

namespace Ppm
{

void Write (OStream *file, const Image *image, const Image::WriteOptions& options)
{
	int                 file_type = POV_File_Image_PPM;
	int                 width = image->GetWidth() ;
	int                 height = image->GetHeight() ;
	int                 bpcc = options.bpcc;
	bool                grayscale = false;
	unsigned int        rval;
	unsigned int        gval;
	unsigned int        bval;
	unsigned int        gray;
	unsigned int        mask;
	GammaCurvePtr       gamma;
	DitherHandler*      dither = options.dither.get();

	if (bpcc == 0)
		bpcc = image->GetMaxIntValue() == 65535 ? 16 : 8 ;
	else if (bpcc < 5)
		bpcc = 5 ;
	else if (bpcc > 16)
		bpcc = 16 ;

	mask = (1 << bpcc) - 1 ;

	// do we want 16 bit grayscale (PGM) output ?
	// TODO - the check for image container type is here to mimick old code; do we still need it?
	if (image->GetImageDataType () == Image::Gray_Int16 || image->GetImageDataType () == Image::GrayA_Int16 || options.grayscale)
	{
		grayscale   = true;
		bpcc        = 16;
		gamma.reset(); // TODO - this is here to mimick old code, which never did gamma correction for greyscale output; do we want to change that?
		file->printf("P5\n");
	}
	else
	{
		if (options.encodingGamma)
			gamma = TranscodingGammaCurve::Get(options.workingGamma, options.encodingGamma);
		else
			// PPM files may or may not be gamma-encoded; besides the sRGB transfer function, the ITU-R-BT.709 transfer function is said to be common as well.
			// If no encoding gamma is specified, we're defaulting to working gamma space at present, i.e. no gamma correction.
			gamma = NeutralGammaCurve::Get();
#ifndef ASCII_PPM_OUTPUT
		file->printf("P6\n");
#else
		file->printf("P3\n");
#endif
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
				gray = GetEncodedGrayValue (image, x, y, gamma, mask, *dither) ;

				if (bpcc > 8)
				{
					if (!file->Write_Byte((gray >> 8) & 0xFF))
						throw POV_EXCEPTION(kFileDataErr, "Cannot write PGM output data");
					if (!file->Write_Byte(gray & 0xFF))
						throw POV_EXCEPTION(kFileDataErr, "Cannot write PGM output data");
				}
				else
				{
					if (!file->Write_Byte(gray & 0xFF))
						throw POV_EXCEPTION(kFileDataErr, "Cannot write PGM output data");
				}
			}
			else
			{
				GetEncodedRGBValue (image, x, y, gamma, mask, rval, gval, bval, *dither) ;

				if (bpcc > 8)
				{
					// 16 bit per value
#ifndef ASCII_PPM_OUTPUT
					file->Write_Byte(rval >> 8) ;
					file->Write_Byte(rval & 0xFF) ;
					file->Write_Byte(gval >> 8) ;
					file->Write_Byte(gval & 0xFF) ;
					file->Write_Byte(bval >> 8) ;
					if (!file->Write_Byte(bval & 0xFF))
						throw POV_EXCEPTION(kFileDataErr, "Cannot write PPM output data");
#else
					file->printf("%u ", rval);
					file->printf("%u ", gval);
					file->printf("%u\n", bval);
					if (!file)
						throw POV_EXCEPTION(kFileDataErr, "Cannot write PPM output data");
#endif
				}
				else
				{
					// 8 bit per value
#ifndef ASCII_PPM_OUTPUT
					file->Write_Byte(rval & 0xFF) ;
					file->Write_Byte(gval & 0xFF) ;
					if (!file->Write_Byte(bval & 0xFF))
						throw POV_EXCEPTION(kFileDataErr, "Cannot write PPM output data");
#else
					file->printf("%u ", rval & 0xff);
					file->printf("%u ", gval & 0xff);
					file->printf("%u\n", bval & 0xff);
					if (!file)
						throw POV_EXCEPTION(kFileDataErr, "Cannot write PPM output data");
#endif
				}
			}
		}
	}
}

/*****************************************************************************
*
* FUNCTION
*
*  Read_PPM_Image
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
*    Reads an PPM image file
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
	int                   r; // not unsigned as it may need to hold EOF
	int                   g; // not unsigned as it may need to hold EOF
	int                   b; // not unsigned as it may need to hold EOF
	unsigned char         header[2];

	// PPM files may or may not be gamma-encoded.
	GammaCurvePtr gamma;
	if (options.gammacorrect && options.defaultGamma)
		gamma = TranscodingGammaCurve::Get(options.workingGamma, options.defaultGamma);

	// --- Read Header ---
	if (!file->read(header, 2))
		throw POV_EXCEPTION(kFileDataErr, "Cannot read header of PPM image");

	if(header[0] != 'P')
		throw POV_EXCEPTION(kFileDataErr, "File is not in PPM format");

	if ((header[1] != '3') && (header[1] != '6'))
		throw POV_EXCEPTION(kFileDataErr, "File is not in PPM format");

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
		throw POV_EXCEPTION(kFileDataErr, "Cannot read width and height from PPM image");

	if (width <= 0 || height <= 0)
		throw POV_EXCEPTION(kFileDataErr, "Invalid width or height read from PPM image");

	do
	{
		file->getline (line, 1024) ;
		line[1023] = '\0';
		if ((ptr = strchr(line, '#')) != NULL)
			*ptr = '\0';  // remove comment
	} while (line[0]=='\0');  // read until line without comment from beginning

	// --- Second: one number: color depth ---
	if (sscanf(line,"%u",&depth) != 1)
		throw POV_EXCEPTION(kFileDataErr, "Cannot read color depth from PPM image");

	if ((depth > 65535) || (depth < 1))
		throw POV_EXCEPTION(kFileDataErr, "Unsupported number of colors in PPM image");

	if (depth < 256)
	{
		// We'll be using an image container that provides for automatic decoding if possible - unless there's no such decoding to do.
		gamma = ScaledGammaCurve::GetByDecoding(255.0f/depth, gamma); // Note that we'll apply the scaling even if we don't officially gamma-correct
		Image::ImageDataType imagetype = options.itype;
		if (imagetype == Image::Undefined)
			imagetype = ( GammaCurve::IsNeutral(gamma) ? Image::RGB_Int8 : Image::RGB_Gamma8);
		image = Image::Create (width, height, imagetype) ;
		// NB: PPM files don't use alpha, so premultiplied vs. non-premultiplied is not an issue
		image->TryDeferDecoding(gamma, 255); // try to have gamma adjustment being deferred until image evaluation.

		for (int i = 0; i < height; i++)
		{
			if (header[1] == '3') // --- ASCII PPM file (type 3) ---
			{
				for (int x = 0; x < width; x++)
				{
					r = Pgm::Read_ASCII_File_Number(file);
					g = Pgm::Read_ASCII_File_Number(file);
					b = Pgm::Read_ASCII_File_Number(file);
					SetEncodedRGBValue (image, x, i, gamma, 255, r, g, b) ;
				}
			}
			else                  // --- binary PPM file (type 6) ---
			{
				for (int x = 0; x < width; x++)
				{
					if ((r = file->Read_Byte ()) == EOF)
						throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PPM file");
					if ((g = file->Read_Byte ()) == EOF)
						throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PPM file");
					if ((b = file->Read_Byte ()) == EOF)
						throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PPM file");
					SetEncodedRGBValue (image, x, i, gamma, 255, r, g, b) ;
				}
			}
		}
	}
	else // --- 16 bit PPM (binary or ASCII) ---
	{
		// We'll be using an image container that provides for automatic decoding if possible - unless there's no such decoding to do.
		gamma = ScaledGammaCurve::GetByDecoding(65535.0f/depth, gamma); // Note that we'll apply the scaling even if we don't officially gamma-correct
		Image::ImageDataType imagetype = options.itype;
		if (imagetype == Image::Undefined)
			imagetype = ( GammaCurve::IsNeutral(gamma) ? Image::RGB_Int16 : Image::RGB_Gamma16);
		image = Image::Create (width, height, imagetype) ;
		// NB: PPM files don't use alpha, so premultiplied vs. non-premultiplied is not an issue
		image->TryDeferDecoding(gamma, 65535); // try to have gamma adjustment being deferred until image evaluation.

		for (int i = 0; i < height; i++)
		{
			if (header[1] == '3') // --- ASCII PPM file (type 3) ---
			{
				for (int x = 0; x < width; x++)
				{
					r = Pgm::Read_ASCII_File_Number(file);
					g = Pgm::Read_ASCII_File_Number(file);
					b = Pgm::Read_ASCII_File_Number(file);
					SetEncodedRGBValue (image, x, i, gamma, 65535, r, g, b) ;
				}
			}
			else                  // --- binary PPM file (type 6) ---
			{
				for (int x = 0; x < width; x++)
				{
					if ((data_hi = file->Read_Byte ()) == EOF)
						throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PPM file");
					if ((data_lo = file->Read_Byte ()) == EOF)
						throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PPM file");
					r = (256*data_hi + data_lo);

					if ((data_hi = file->Read_Byte ()) == EOF)
						throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PPM file");
					if ((data_lo = file->Read_Byte ()) == EOF)
						throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PPM file");
					g = (256*data_hi + data_lo);

					if ((data_hi = file->Read_Byte ()) == EOF)
						throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PPM file");
					if ((data_lo = file->Read_Byte ()) == EOF)
						throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF in PPM file");
					b = (256*data_hi + data_lo);

					SetEncodedRGBValue (image, x, i, gamma, 65535, r, g, b) ;
				}
			}
		}
	}

	return (image) ;
}

} // end of namespace Ppm

}

