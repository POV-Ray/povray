//******************************************************************************
///
/// @file base/image/gifdecod.cpp
///
/// GIF-style LZW decoder.
///
/// @author Steven A. Bennett
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
/// ----------------------------------------------------------------------------
///
/// Major portions of this module were written by Steve Bennett and are used
/// here with his permission:
///
/// DECODER.C - An LZW decoder for GIF
/// Copyright (C) 1987, by Steven A. Bennett
///
/// Permission is given by the author to freely redistribute and include
/// this code in any program as long as this credit is given where due.
///
/// In accordance with the above, I want to credit Steve Wilhite who wrote
/// the code which this is heavily inspired by...
///
/// GIF and 'Graphics Interchange Format' are trademarks (tm) of
/// Compuserve, Incorporated, an H&R Block Company.
///
/// Release Notes: This file contains a decoder routine for GIF images
/// which is similar, structurally, to the original routine by Steve Wilhite.
/// It is, however, somewhat noticably faster in most cases.
///
/// This routine was modified for use in FRACTINT.
///
/// @endparblock
///
//******************************************************************************

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "base/image/gif.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/pov_err.h"
#include "base/image/image.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

namespace Gif
{

#define LOCAL static
#define IMPORT extern

#define FAST // nothing (was `register`, which is deprecated as of C++11 and disallowed as of C++17)

    typedef unsigned short UWORD;
    typedef unsigned char UTINY;
    typedef int LONG;
    typedef unsigned int ULONG;
    typedef int INT;


    /* Various error codes used by decoder
    * and my own routines...   It's okay
    * for you to define whatever you want,
    * as long as it's negative...  It will be
    * returned intact up the various subroutine
    * levels...
    */
    const int OUT_OF_MEMORY = -10;
    const int BAD_CODE_SIZE = -20;
    const int READ_ERROR = -1;
    const int WRITE_ERROR = -2;
    const int OPEN_ERROR = -3;
    const int CREATE_ERROR = -4;

    const int MAX_CODES = 4095;

    LOCAL const LONG code_mask[13] = {
        0,
        0x0001, 0x0003,
        0x0007, 0x000F,
        0x001F, 0x003F,
        0x007F, 0x00FF,
        0x01FF, 0x03FF,
        0x07FF, 0x0FFF
    };

    struct Param_Block final
    {
        short curr_size;                     /* The current code size */
        short clear_code;                    /* Value for a clear code */
        short ending;                        /* Value for a ending code */
        short newcodes;                      /* First available code */
        short top_slot;                      /* Highest code for current size */
        short slot;                          /* Last read code */
        short navail_bytes;                  /* # bytes left in block */
        short nbits_left;                    /* # bits left in current byte */
        UTINY b1;                            /* Current byte */
        UTINY byte_buff[257];                /* Current block */
        UTINY *pbytes;                       /* Pointer to next byte in block */
    };

    /* get_next_code()
    * - gets the next code from the GIF file.  Returns the code, or else
    * a negative number in case of file errors...
    */
    static short get_next_code(IStream *file, Param_Block *params)
    {
        short i, x;
        ULONG ret;

        if (params->nbits_left == 0)
        {
            if (params->navail_bytes <= 0)
            {

                /* Out of bytes in current block, so read next block
                */
                params->pbytes = params->byte_buff;
                if ((params->navail_bytes = file->Read_Byte()) < 0)
                    return(params->navail_bytes);
                else if (params->navail_bytes)
                {
                    for (i = 0; i < params->navail_bytes; ++i)
                    {
                        if ((x = file->Read_Byte()) < 0)
                            return(x);
                        params->byte_buff[i] = (UTINY) x;
                    }
                }
            }
            params->b1 = *params->pbytes++;
            params->nbits_left = 8;
            --params->navail_bytes;
        }

        ret = params->b1 >> (8 - params->nbits_left);
        while (params->curr_size > params->nbits_left)
        {
            if (params->navail_bytes <= 0)
            {

                /* Out of bytes in current block, so read next block
                */
                params->pbytes = params->byte_buff;
                if ((params->navail_bytes = file->Read_Byte()) < 0)
                    return(params->navail_bytes);
                else if (params->navail_bytes)
                {
                    for (i = 0; i < params->navail_bytes; ++i)
                    {
                        if ((x = file->Read_Byte()) < 0)
                            return(x);
                        params->byte_buff[i] = (UTINY) x;
                    }
                }
            }
            params->b1 = *params->pbytes++;
            ret |= params->b1 << params->nbits_left;
            params->nbits_left += 8;
            --params->navail_bytes;
        }
        params->nbits_left -= params->curr_size;
        ret &= code_mask[params->curr_size];
        return((short)(ret));
    }


    /* The reason we have these seperated like this instead of using
    * a structure like the original Wilhite code did, is because this
    * stuff generally produces significantly faster code when compiled...
    * This code is full of similar speedups...  (For a good book on writing
    * C for speed or for space optomisation, see Efficient C by Tom Plum,
    * published by Plum-Hall Associates...)
    */

    /*
    I removed the LOCAL identifiers in the arrays below and replaced them
    with 'extern's so as to declare (and re-use) the space elsewhere.
    The arrays are actually declared in the assembler source.
    Bert Tyler
    */

    /* short decoder(linewidth)
    *    short linewidth;               * Pixels per line of image *
    *
    * - This function decodes an LZW image, according to the method used
    * in the GIF spec.  Every *linewidth* "characters" (ie. pixels) decoded
    * will generate a call to out_line(), which is a user specific function
    * to display a line of pixels.  The function gets its codes from
    * get_next_code() which is responsible for reading blocks of data and
    * seperating them into the proper size codes.  Finally, file->Read_Byte() is
    * the global routine to read the next byte from the GIF file.
    *
    * It is generally a good idea to have linewidth correspond to the actual
    * width of a line (as specified in the Image header) to make your own
    * code a bit simpler, but it isn't absolutely necessary.
    *
    */

    /*
     * bad_code_count is incremented each time an out of range code is read by
     * the decoder. When this value is non-zero after a decode, your GIF file
     * is probably corrupt in some way...
     */
    void Decode (IStream *file, Image *image)
    {
        short linewidth = image->GetWidth();
        FAST UTINY *sp, *bufptr;
        FAST short code, fc, oc, bufcnt;
        short c, size;
        int x = 0 ;
        int y = 0 ;
        int width = image->GetWidth();
        int height = image->GetHeight();
        int bad_code_count=0;
        Param_Block params ;

        /* Initialize for decoding a new image... */
        if ((size = file->Read_Byte()) == EOF)
            throw POV_EXCEPTION(kFileDataErr, "Unexpected EOF while reading GIF file");
        if (size < 2 || 9 < size)
            throw POV_EXCEPTION(kFileDataErr, "Bad code size in GIF file");

        params.curr_size = size + 1;
        params.top_slot = 1 << params.curr_size;
        params.clear_code = 1 << size;
        params.ending = params.clear_code + 1;
        params.slot = params.newcodes = params.ending + 1;
        params.navail_bytes = params.nbits_left = 0;

        std::unique_ptr<UTINY[]> dstack (new UTINY [MAX_CODES + 1]);
        std::unique_ptr<UTINY[]> suffix (new UTINY [MAX_CODES + 1]);
        std::unique_ptr<UWORD[]> prefix (new UWORD [MAX_CODES + 1]);
        std::unique_ptr<UTINY[]> buf (new UTINY [width]);

        /* Initialize in case they forgot to put in a clear code.
         * (This shouldn't happen, but we'll try and decode it anyway...)
         */
        oc = fc = 0;

        /* Set up the stack pointer and decode buffer pointer */
        sp = dstack.get();
        bufptr = buf.get();
        bufcnt = linewidth;

        /* This is the main loop.  For each code we get we pass through the
         * linked list of prefix codes, pushing the corresponding "character" for
         * each code onto the stack.  When the list reaches a single "character"
         * we push that on the stack too, and then start unstacking each
         * character for output in the correct order.  Special handling is
         * included for the clear code, and the whole thing ends when we get
         * an ending code.
         */
        while ((c = get_next_code(file, &params)) != params.ending)
        {

            /* If we had a file error, return without completing the decode */
            if (c < 0)
                throw POV_EXCEPTION(kFileDataErr, "Error while reading GIF file");

            /* If the code is a clear code, reinitialize all necessary items. */
            if (c == params.clear_code)
            {
                params.curr_size = size + 1;
                params.slot = params.newcodes;
                params.top_slot = 1 << params.curr_size;

                /* Continue reading codes until we get a non-clear code
                 * (Another unlikely, but possible case...)
                 */
                while ((c = get_next_code(file, &params)) == params.clear_code)
                    ;

                /* If we get an ending code immediately after a clear code
                 * (Yet another unlikely case), then break out of the loop.
                 */
                if (c == params.ending)
                    break;

                /* Finally, if the code is beyond the range of already set codes,
                 * (This one had better NOT happen...  I have no idea what will
                 * result from this, but I doubt it will look good...) then set it
                 * to color zero.
                 */
                if (c >= params.slot)
                    c = 0;

                oc = fc = c;

                /* And let us not forget to put the char into the buffer... And
                 * if, on the off chance, we were exactly one pixel from the end
                 * of the line, we have to send the buffer to the out_line()
                 * routine...
                 */
                if (y == height)
                    throw POV_EXCEPTION(kFileDataErr, "Extra data in GIF file") ;
                image->SetIndexedValue (x, y, (unsigned char) c) ;
                if (++x == width)
                {
                    x = 0 ;
                    y++ ;
                }
            }
            else
            {
                /* In this case, it's not a clear code or an ending code, so
                 * it must be a code code...  So we can now decode the code into
                 * a stack of character codes. (Clear as mud, right?)
                 */
                code = c;

                /* Here we go again with one of those off chances...  If, on the
                 * off chance, the code we got is beyond the range of those already
                 * set up (Another thing which had better NOT happen...) we trick
                 * the decoder into thinking it actually got the last code read.
                 * (Hmmn... I'm not sure why this works...  But it does...)
                 */
                if (code >= params.slot)
                {
                    if (code > params.slot)
                        ++bad_code_count;
                    code = oc;
                    *sp++ = (UTINY) fc;
                }

                /* Here we scan back along the linked list of prefixes, pushing
                 * helpless characters (ie. suffixes) onto the stack as we do so.
                 */
                while (code >= params.newcodes)
                {
                    *sp++ = suffix[code];
                    code = prefix[code];
                }

                /* Push the last character on the stack, and set up the new
                 * prefix and suffix, and if the required slot number is greater
                 * than that allowed by the current bit size, increase the bit
                 * size.  (NOTE - If we are all full, we *don't* save the new
                 * suffix and prefix...  I'm not certain if this is correct...
                 * it might be more proper to overwrite the last code...
                 */
                *sp++ = (UTINY) code;
                if (params.slot < params.top_slot)
                {
                    fc = code;
                    suffix[params.slot] = (UTINY) fc;
                    prefix[params.slot++] = oc;
                    oc = c;
                }
                if (params.slot >= params.top_slot)
                {
                    if (params.curr_size < 12)
                    {
                        params.top_slot <<= 1;
                        ++params.curr_size;
                    }
                }

                /* Now that we've pushed the decoded string (in reverse order)
                 * onto the stack, lets pop it off and put it into our decode
                 * buffer...  And when the decode buffer is full, write another
                 * line...
                 */
                while (sp > dstack.get())
                {
                    if (y == height)
                        throw POV_EXCEPTION(kFileDataErr, "Extra data in GIF file") ;
                    image->SetIndexedValue (x, y, (unsigned char) *(--sp)) ;
                    if (++x == width)
                    {
                        x = 0 ;
                        y++ ;
                    }
                }
            }
        }
    }

}
// end of namespace Gif

}
// end of namespace pov_base
