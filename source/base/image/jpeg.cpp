//******************************************************************************
///
/// @file base/image/jpeg.cpp
///
/// Implementation of Joint Photographic Experts Group (JPEG) image file
/// handling.
///
/// @note
///     Some functions implemented in this file throw exceptions from 'extern C'
///     code. be sure to enable this in your compiler options (preferably for
///     this file only).
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
#include "base/image/jpeg_pov.h"

#ifndef LIBJPEG_MISSING

// C++ variants of C standard header files
#include <csetjmp>

// C++ standard header files
#include <memory>
#include <string>

// Make sure we can later identify whether JPEGlib wants `TRUE` and `FALSE` to be macros.
#undef TRUE
#undef FALSE

// Other 3rd party header files
extern "C"
{
#include <jpeglib.h>
}

// Check whether JPEGlib wants `TRUE` and `FALSE` to be macros.
#ifdef TRUE
#define POV_KEEP_BOOLEAN_MACROS
#endif

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/image/colourspace.h"
#include "base/image/dither.h"
#include "base/image/encoding.h"
#include "base/image/image.h"
#include "base/image/metadata.h"

// this must be the last file included
#include "base/povdebug.h"

// Work around an issue on Mac OS X, where `TRUE` and `FALSE` are defined
// _somewhere_, in a manner that is incompatible with JPEGlib.
#ifndef POV_KEEP_BOOLEAN_MACROS
#undef TRUE
#undef FALSE
#endif

namespace pov_base
{

namespace Jpeg
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

const int POV_JPEG_BUFFER_SIZE = 1024;

/*****************************************************************************
* Local typedefs
******************************************************************************/

// write buffer for JPEG images
class POV_JPEG_Write_Buffer final
{
public:
    POV_JPEG_Write_Buffer();

    struct jpeg_error_mgr jerr;
    jpeg_source_mgr jsrc;
    jpeg_destination_mgr jdest;
    std::jmp_buf setjmp_buffer;  // for return to caller
    char buffer[POV_JPEG_BUFFER_SIZE];
    JSAMPROW row_pointer[1];
    int row_stride;
    struct jpeg_compress_struct cinfo;
    std::string msg;
    OStream *file;
} ;

POV_JPEG_Write_Buffer::POV_JPEG_Write_Buffer()
{
    memset(&jerr, 0, sizeof(jerr));
    memset(&jsrc, 0, sizeof(jsrc));
    memset(&jdest, 0, sizeof(jdest));
    memset(&cinfo, 0, sizeof(cinfo));
    row_pointer[0] = nullptr;
    file = nullptr;
    row_stride = 0;
}

class POV_JPEG_Read_Buffer final
{
public:
    POV_JPEG_Read_Buffer();

    struct jpeg_error_mgr jerr;
    jpeg_source_mgr jsrc;
    jpeg_destination_mgr jdest;
    std::jmp_buf setjmp_buffer;  // for return to caller
    char buffer[POV_JPEG_BUFFER_SIZE];
    JSAMPROW row_pointer[1];
    int row_stride;
    struct jpeg_decompress_struct cinfo;
    std::string msg;
    IStream *file;

};

POV_JPEG_Read_Buffer::POV_JPEG_Read_Buffer()
{
    memset(&jerr, 0, sizeof(jerr));
    memset(&jsrc, 0, sizeof(jsrc));
    memset(&jdest, 0, sizeof(jdest));
    memset(&cinfo, 0, sizeof(cinfo));
    row_pointer[0] = nullptr;
    file = nullptr;
    row_stride = 0;
}

extern "C"
{
    METHODDEF(void) read_error_exit(j_common_ptr cinfo);
    METHODDEF(void) read_output_message(j_common_ptr cinfo);
    METHODDEF(void) write_error_exit(j_common_ptr cinfo);
    METHODDEF(void) write_output_message(j_common_ptr cinfo);
    METHODDEF(void) write_init_dest(j_compress_ptr cinfo);
    METHODDEF(void) read_init_source(j_decompress_ptr cinfo);
    METHODDEF(boolean) read_fill_input_buffer(j_decompress_ptr cinfo);
    METHODDEF(void) read_skip_input_data(j_decompress_ptr cinfo, long num_bytes);
    METHODDEF(void) read_term_source(j_decompress_ptr cinfo);
    METHODDEF(boolean) write_empty_output_buffer(j_compress_ptr cinfo);
    METHODDEF(void) write_term_destination(j_compress_ptr cinfo);

    METHODDEF(void) read_error_exit (j_common_ptr cinfo)
    {
        POV_JPEG_Read_Buffer * myerr = reinterpret_cast<POV_JPEG_Read_Buffer *>(cinfo->client_data);

        (*cinfo->err->output_message)(cinfo);

        // Return control to the setjmp point
        std::longjmp(myerr->setjmp_buffer, 1);
    }

    METHODDEF(void) write_error_exit (j_common_ptr cinfo)
    {
        POV_JPEG_Write_Buffer * myerr = reinterpret_cast<POV_JPEG_Write_Buffer *>(cinfo->client_data);

        (*cinfo->err->output_message)(cinfo);

        // Return control to the setjmp point
        std::longjmp(myerr->setjmp_buffer, 1);
    }

    METHODDEF(void) read_output_message(j_common_ptr cinfo)
    {
        char buffer[JMSG_LENGTH_MAX];
        POV_JPEG_Read_Buffer *bufptr = reinterpret_cast<POV_JPEG_Read_Buffer *>(cinfo->client_data);

        // Create the message
        (*cinfo->err->format_message) (cinfo, buffer);

        // store it for later display
        bufptr->msg = buffer;
    }

    METHODDEF(void) write_output_message(j_common_ptr cinfo)
    {
        char buffer[JMSG_LENGTH_MAX];
        POV_JPEG_Write_Buffer *bufptr = reinterpret_cast<POV_JPEG_Write_Buffer *>(cinfo->client_data);

        // Create the message
        (*cinfo->err->format_message) (cinfo, buffer);

        // store it for later display
        bufptr->msg = buffer;
    }

    METHODDEF(void) write_init_dest(j_compress_ptr cinfo)
    {
        POV_JPEG_Write_Buffer * bufptr = reinterpret_cast<POV_JPEG_Write_Buffer *>(cinfo->client_data);

        bufptr->jdest.next_output_byte = reinterpret_cast<unsigned char *>(&(bufptr->buffer[0]));
        bufptr->jdest.free_in_buffer = POV_JPEG_BUFFER_SIZE;
    }

    METHODDEF(void) read_init_source(j_decompress_ptr cinfo)
    {
        POV_JPEG_Read_Buffer * bufptr = reinterpret_cast<POV_JPEG_Read_Buffer *>(cinfo->client_data);

        bufptr->jsrc.next_input_byte = reinterpret_cast<unsigned char *>(&(bufptr->buffer[0]));
        bufptr->jsrc.bytes_in_buffer = 0;
    }

    METHODDEF(boolean) read_fill_input_buffer(j_decompress_ptr cinfo)
    {
        POV_JPEG_Read_Buffer * bufptr = reinterpret_cast<POV_JPEG_Read_Buffer *>(cinfo->client_data);
        int i;

        for(i = 0; i < POV_JPEG_BUFFER_SIZE; i++)
        {
            if (!bufptr->file->read(&(bufptr->buffer[i]), 1))
                break;
        }
        bufptr->jsrc.bytes_in_buffer = i;
        bufptr->jsrc.next_input_byte = reinterpret_cast<unsigned char *>(&(bufptr->buffer[0]));

        return TRUE;
    }

    METHODDEF(void) read_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
    {
        POV_JPEG_Read_Buffer *bufptr = reinterpret_cast<POV_JPEG_Read_Buffer *>(cinfo->client_data);

        if ( bufptr->jsrc.bytes_in_buffer < num_bytes )
        {
            num_bytes -= (long) bufptr->jsrc.bytes_in_buffer;
            bufptr->jsrc.bytes_in_buffer = 0;
            bufptr->file->seekg(num_bytes, IOBase::seek_cur);
        }
        else
        {
            bufptr->jsrc.bytes_in_buffer -= num_bytes;
            bufptr->jsrc.next_input_byte += num_bytes;
        }

    }

    METHODDEF(void) read_term_source(j_decompress_ptr /*cinfo*/)
    {
    }

    METHODDEF(boolean) write_empty_output_buffer(j_compress_ptr cinfo)
    {
        POV_JPEG_Write_Buffer * bufptr = reinterpret_cast<POV_JPEG_Write_Buffer *>(cinfo->client_data);

        // throw an exception on failure rather than return false; this gives more
        // helpful error reporting
        if (!bufptr->file->write(bufptr->buffer, POV_JPEG_BUFFER_SIZE))
            throw POV_EXCEPTION(kFileDataErr, "Failed to write JPEG data to disk");
        bufptr->jdest.next_output_byte = reinterpret_cast<unsigned char *>(&(bufptr->buffer[0]));
        bufptr->jdest.free_in_buffer = POV_JPEG_BUFFER_SIZE;
        return TRUE;
    }

    METHODDEF(void) write_term_destination(j_compress_ptr cinfo)
    {
        POV_JPEG_Write_Buffer * bufptr = reinterpret_cast<POV_JPEG_Write_Buffer *>(cinfo->client_data);

        if(POV_JPEG_BUFFER_SIZE - bufptr->jdest.free_in_buffer > 0)
            if (!bufptr->file->write(bufptr->buffer, POV_JPEG_BUFFER_SIZE - bufptr->jdest.free_in_buffer))
                throw POV_EXCEPTION(kFileDataErr, "Failed to write final JPEG data block to disk");
    }
}

// TODO: handle possible memory leakage if an exception is thrown during a read
Image *Read (IStream *file, const ImageReadOptions& options)
{
    int                             width;
    int                             height;
    int                             row;
    int                             col;
    Image                           *image;
    POV_JPEG_Read_Buffer            readbuf;

    // We set up the normal JPEG error routines, then override error_exit and output_message.
    readbuf.cinfo.err = jpeg_std_error(&readbuf.jerr);
    readbuf.jerr.error_exit = read_error_exit;
    readbuf.jerr.output_message = read_output_message;
    readbuf.cinfo.client_data = reinterpret_cast<void *>(&readbuf);

    if (setjmp(readbuf.setjmp_buffer))
    {
        jpeg_destroy_decompress(&readbuf.cinfo);
        if (readbuf.msg.length() > 0)
            throw POV_EXCEPTION(kFileDataErr, readbuf.msg.c_str());
        throw POV_EXCEPTION(kFileDataErr, "Cannot read JPEG image");
    }

    // Now we can initialize the JPEG decompression object.
    jpeg_create_decompress(&readbuf.cinfo);

    readbuf.file = file;
    readbuf.jsrc.init_source = read_init_source;
    readbuf.jsrc.fill_input_buffer = read_fill_input_buffer;
    readbuf.jsrc.skip_input_data = read_skip_input_data;
    readbuf.jsrc.resync_to_restart = jpeg_resync_to_restart;
    readbuf.jsrc.term_source = read_term_source;
    readbuf.cinfo.src = &readbuf.jsrc;

    /* We can ignore the return value from jpeg_read_header since
     *   (a) suspension is not possible with the stdio data source, and
     *   (b) we passed true to reject a tables-only JPEG file as an error.
     * See libjpeg.doc for more info.
     */
    (void)jpeg_read_header(&readbuf.cinfo, TRUE);

    // check for unsupported formats
    if((readbuf.cinfo.output_components != 1) && (readbuf.cinfo.output_components != 3) &&
       (readbuf.cinfo.out_color_space != JCS_GRAYSCALE) && (readbuf.cinfo.out_color_space != JCS_RGB))
    {
        jpeg_destroy_decompress(&readbuf.cinfo);
        throw POV_EXCEPTION(kFileDataErr, "Unsupported color format in JPEG image");
    }

    // TODO FIXME - in order to preserve image quality, there should be a way to perform the conversion from YCbCr to RGB
    // including gamma correction in the floating-point domain, or at least in a higher-resolution integer domain.

    // JPEG files used to have no clearly defined gamma by default, but a W3C recommendation exists for them to use sRGB
    // unless an ICC profile is present (which we presently don't support).
    GammaCurvePtr gamma;
    if (options.gammacorrect)
    {
        if (options.defaultGamma)
            gamma = TranscodingGammaCurve::Get(options.workingGamma, options.defaultGamma);
        else
            gamma = TranscodingGammaCurve::Get(options.workingGamma, SRGBGammaCurve::Get());
    }

    // begin decompression
    (void)jpeg_start_decompress(&readbuf.cinfo);

    height = readbuf.cinfo.output_height;
    width = readbuf.cinfo.output_width;

    ImageDataType imagetype = options.itype;
    if (imagetype == ImageDataType::Undefined)
        imagetype = Image::GetImageDataType(8, (readbuf.cinfo.output_components == 1 ? 1 : 3), false, gamma);
    image = Image::Create (width, height, imagetype) ;
    // NB: JPEG files don't use alpha, so premultiplied vs. non-premultiplied is not an issue
    image->TryDeferDecoding(gamma, MAXJSAMPLE); // try to have gamma adjustment being deferred until image evaluation.

    // JSAMPLEs per row in output buffer
    readbuf.row_stride = readbuf.cinfo.output_width * readbuf.cinfo.output_components;

    // Make a one-row-high sample array
    std::unique_ptr<JSAMPLE[]> scopedarray (new JSAMPLE [readbuf.row_stride]);
    readbuf.row_pointer[0] = (JSAMPROW) &scopedarray[0] ;

    // read image row by row
    for (row = 0; row < height; row++)
    {
        // read scanline
        (void)jpeg_read_scanlines(&readbuf.cinfo, &readbuf.row_pointer[0], 1);

        if(readbuf.cinfo.output_components == 3) // 24-bit rgb image
        {
            for (col = 0; col < width; col++)
            {
                unsigned int r = readbuf.row_pointer[0][col * 3];
                unsigned int g = readbuf.row_pointer[0][(col * 3) + 1];
                unsigned int b = readbuf.row_pointer[0][(col * 3) + 2];
                SetEncodedRGBValue (image, col, row, gamma, MAXJSAMPLE, r, g, b) ;
            }
        }
        else if (readbuf.cinfo.output_components == 1) // 8-bit grayscale image
        {
            for (col = 0; col < width; col++)
            {
                SetEncodedGrayValue (image, col, row, gamma, MAXJSAMPLE, (unsigned int) readbuf.row_pointer[0][col]) ;
            }
        }
    }

    // finish decompression
    (void)jpeg_finish_decompress(&readbuf.cinfo);

    // release JPEG decompression object
    jpeg_destroy_decompress(&readbuf.cinfo);

    return (image);
}

// TODO: handle possible memory leakage if an exception is thrown during a write
void Write (OStream *file, const Image *image, const ImageWriteOptions& options)
{
    int                         width = image->GetWidth() ;
    int                         height = image->GetHeight() ;
    int                         quality;
    int                         grayscale = image->IsGrayscale();
    POV_JPEG_Write_Buffer       writebuf;
    GammaCurvePtr               gamma;
    DitherStrategySPtr          dither = GetNoOpDitherStrategy(); // dithering doesn't make much sense with JPEG

    // JPEG files used to have no clearly defined gamma by default, but a W3C recommendation exists for them to use sRGB
    // unless an ICC profile is present (which we presently don't support).
    gamma = options.GetTranscodingGammaCurve(SRGBGammaCurve::Get());

    writebuf.file = file;

    // We set up the normal JPEG error routines, then override error_exit and output_message.
    writebuf.cinfo.err = jpeg_std_error(&writebuf.jerr);
    writebuf.jerr.error_exit = write_error_exit;
    writebuf.jerr.output_message = write_output_message;
    writebuf.cinfo.client_data = reinterpret_cast<void *>(&writebuf);

    if (setjmp(writebuf.setjmp_buffer))
    {
        jpeg_destroy_compress(&writebuf.cinfo);
        if (writebuf.msg.length() > 0)
            throw POV_EXCEPTION(kFileDataErr, writebuf.msg.c_str());
        throw POV_EXCEPTION(kFileDataErr, "Cannot write JPEG image");
    }

    jpeg_create_compress(&writebuf.cinfo);
    writebuf.jdest.init_destination = write_init_dest;
    writebuf.jdest.empty_output_buffer = write_empty_output_buffer;
    writebuf.jdest.term_destination = write_term_destination;
    writebuf.cinfo.dest = &writebuf.jdest;
    writebuf.cinfo.image_width = width;
    writebuf.cinfo.image_height = height;

    if (grayscale)
    {
        writebuf.cinfo.in_color_space = JCS_GRAYSCALE;
        writebuf.cinfo.input_components = 1;
        jpeg_set_defaults(&writebuf.cinfo);
        jpeg_set_colorspace(&writebuf.cinfo, JCS_GRAYSCALE);
    }
    else
    {
        writebuf.cinfo.in_color_space = JCS_RGB; // colour model of data we're feeding to the jpeglib
        writebuf.cinfo.input_components = 3;
        jpeg_set_defaults(&writebuf.cinfo);
        jpeg_set_colorspace(&writebuf.cinfo, JCS_YCbCr); // colour model to use in the output file; must be YCbCr for compliance with JFIF standard
    }

    writebuf.row_stride = writebuf.cinfo.image_width * writebuf.cinfo.input_components;

    // Make a one-row-high sample array
    std::unique_ptr<JSAMPLE[]> scopedarray (new JSAMPLE [writebuf.row_stride]);
    writebuf.row_pointer[0] = (JSAMPROW) &scopedarray[0] ;

    // if quality is not specified, we wind the output quality waaaay up (usually needed for raytracing)
    // (This used to be 95% when we still did chroma sub-sampling; without chroma sub-sampling,
    // about the same visual quality and file size is achieved at 85%. - [CLi])
    if (options.compression <= 1)
        // Negative values indicate default quality; `0` means no compression, but since JPEG is
        // always lossy we interpret this as "use a reasonably high quality"; and we interpret `1`
        // as "enable compression", which is the default anyway.
        quality = 85;
    else
        // Valid values for the JPEG quality setting range from 0 to 100 (though we provide no way
        // to choose either 0 or 1).
        quality = clip(int(options.compression), 0, 100);
    jpeg_set_quality(&writebuf.cinfo, quality, TRUE); // quality (range 2 to 100)

    if (!grayscale)
    {
        // inhibit chroma sub-sampling to better preserve small colour details
        writebuf.cinfo.comp_info[0].h_samp_factor = 1;
        writebuf.cinfo.comp_info[0].v_samp_factor = 1;
    }

    // begin compression
    jpeg_start_compress(&writebuf.cinfo, TRUE);

    // prepare metadata
    Metadata meta;
    /* Line feed is "local" */
    std::string comment = std::string("Render Date: ") + meta.getDateTime() + "\n";
    comment += "Software: " + meta.getSoftware() + "\n";
    if (!meta.getComment1().empty())
        comment += meta.getComment1() + "\n";
    if (!meta.getComment2().empty())
        comment += meta.getComment2() + "\n";
    if (!meta.getComment3().empty())
        comment += meta.getComment3() + "\n";
    if (!meta.getComment4().empty())
        comment += meta.getComment4() + "\n";

    const JOCTET *pcom(reinterpret_cast<const JOCTET*>(comment.c_str()));

    // The comment marker must be here, before the image data
    jpeg_write_marker(&writebuf.cinfo, JPEG_COM, pcom,comment.length());

    // write image row by row
    for (int row = 0; row < height; row++)
    {
        JSAMPLE *sample = writebuf.row_pointer[0];
        if (writebuf.cinfo.input_components == 3) // 24-bit rgb image
        {
            for (int col = 0; col < width; col++)
            {
                unsigned int r, g, b;
                GetEncodedRGBValue(image, col, row, gamma, MAXJSAMPLE, r, g, b, *dither);
                *sample++ = (JSAMPLE) r;
                *sample++ = (JSAMPLE) g;
                *sample++ = (JSAMPLE) b;
            }
        }
        else if (writebuf.cinfo.input_components == 1) // 8-bit grayscale image
        {
            for (int col = 0; col < width; col++)
                *sample++ = (JSAMPLE) GetEncodedGrayValue(image, col, row, gamma, MAXJSAMPLE, *dither);
        }
        jpeg_write_scanlines(&writebuf.cinfo, writebuf.row_pointer, 1);
    }

    jpeg_finish_compress(&writebuf.cinfo);
    jpeg_destroy_compress(&writebuf.cinfo);
}

}
// end of namespace Jpeg

}
// end of namespace pov_base

#endif  // LIBJPEG_MISSING
