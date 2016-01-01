//******************************************************************************
///
/// @file base/textstreambuffer.cpp
///
/// Implementations related to buffered text file output.
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
#include "base/textstreambuffer.h"

// C++ variants of standard C header files
#include <cstring>

// Standard C++ header files
#include <algorithm>

// POV-Ray base header files
#include "base/pov_err.h"
#include "base/stringutilities.h"
#include "base/types.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

TextStreamBuffer::TextStreamBuffer(size_t buffersize, unsigned int wrapwidth)
{
    boffset = 0;
    bsize = buffersize;
    wrap = wrapwidth;
    curline = 0;
    buffer = new char[bsize];
    if(buffer == NULL)
        throw POV_EXCEPTION_CODE(kOutOfMemoryErr);
}

TextStreamBuffer::~TextStreamBuffer()
{
    boffset = 0;
    bsize = 0;
    wrap = 0;
    curline = 0;
    if(buffer != NULL)
        delete[] buffer;
    buffer = NULL;
}

void TextStreamBuffer::printf(const char *format, ...)
{
    va_list marker;

    va_start(marker, format);
    vsnprintf(&buffer[boffset], bsize - boffset - 1, format, marker);
    va_end(marker);

    // direct output
    directflush(&buffer[boffset], strlen(&buffer[boffset]));

    boffset = strlen(buffer);

    // line buffered output
    lineflush();
}

void TextStreamBuffer::print(const char *str)
{
    printf("%s", str);
}

void TextStreamBuffer::puts(const char *str)
{
    printf("%s\n", str);
}

void TextStreamBuffer::putc(int chr)
{
    printf("%c", chr);
}

void TextStreamBuffer::printfile(const char *filename, POV_LONG offset, POV_LONG lines)
{
    FILE *file = fopen(filename, "r");

    if(file != NULL)
    {
        fseek(file, offset, SEEK_SET);
        printfile(file, lines);
        fclose(file);
    }
}

void TextStreamBuffer::printfile(FILE *file, POV_LONG lines)
{
    if(file != NULL)
    {
        bool stopposset = (lines < 0); // only if walking backwards stop at current position
        POV_LONG stoppos = (POV_LONG)(ftell(file));
        int chr = 0;

        if(lines < 0)
        {
            POV_LONG lineoffset = lines;

            // NOTE: This will walk back one line too far! However, it only walks
            // back to the end of that line. Thus, the next step will walk forward
            // again to the beginning of the right line, which is the desired
            // position. Do not change this behavior without testing! [trf]
            for(POV_LONG pos = (POV_LONG)(ftell(file)) - 1; (lineoffset < 1) && (pos >= 0); pos--)
            {
                // WARNING: Expensive way to walk backward through a file, but will only
                // be used when problems are encountered anyway, and then it most likely
                // does not matter if the output of the message takes a tiny bit longer!
                fseek(file, pos, SEEK_SET);

                chr = fgetc(file);

                if((chr == 10) || (chr == 13))
                {
                    chr = fgetc(file);
                    if(!((chr == 10) || (chr == 13)))
                        ungetc(chr, file);
                    lineoffset++;
                }
                else if(chr == EOF)
                    break;
            }

            // beginning of file was previously reached
            if(lineoffset < 1)
                fseek(file, 0, SEEK_SET);

            while(lineoffset > 0)
            {
                chr = fgetc(file);

                if((chr == 10) || (chr == 13))
                {
                    chr = fgetc(file);
                    if(!((chr == 10) || (chr == 13)))
                        ungetc(chr, file);
                    lineoffset--;
                }
                else if(chr == EOF)
                    break;
            }

            // make number of lines to output positive for next step
            lines = -lines;
        }

        while(lines > 0)
        {
            chr = fgetc(file);

            if((stopposset == true) && (stoppos == ((POV_LONG)(ftell(file)) - 1))) // only if walking backwards stop at initial position
                break;

            // count newlines in file and replace newlines with system specific newline charcater
            if((chr == 10) || (chr == 13))
            {
                chr = fgetc(file);
                if(!((chr == 10) || (chr == 13)))
                    ungetc(chr, file);
                else
                {
                    if((stopposset == true) && (stoppos == ((POV_LONG)(ftell(file)) - 1))) // only if walking backwards stop at initial position
                        break;
                }
                printf("\n");
                lines--;
            }
            else if(chr == EOF)
                break;
            else
                printf("%c", chr);
        }
    }
}

void TextStreamBuffer::flush()
{
    if(curline > 0)
        directoutput("\n", 1);
    curline = 0;

    lineflush();
    if(boffset > 0)
        lineoutput(buffer, boffset);
    boffset = 0;
}

void TextStreamBuffer::lineoutput(const char *str, unsigned int chars)
{
    // by default output to stdout
    fwrite(str, sizeof(char), chars, stdout);
    fprintf(stdout, "\n");
    fflush(stdout);
}

void TextStreamBuffer::directoutput(const char *, unsigned int)
{
    // does nothing by default
}

void TextStreamBuffer::rawoutput(const char *, unsigned int)
{
    // does nothing by default
}

void TextStreamBuffer::lineflush()
{
    unsigned int lasti = 0;
    unsigned int ii = 0;
    unsigned int i = 0;

    // output all complete lines in the buffer
    while(i < boffset)
    {
        if((buffer[i] == '\n') || (buffer[i] == '\r'))
        {
            lineoutput(&buffer[lasti], i - lasti);
            lasti = i + 1;
        }
        else if(i - lasti >= wrap)
        {
            // track back to last space up to 1/4 in the line to wrap
            for(ii = 0; ii < min((wrap / 4), i); ii++)
            {
                if(isspace(buffer[i - ii]))
                    break;
            }

            // if no space was found in the last 1/4 of the line to wrap, split it at the end anyway
            if(ii == min((wrap / 4), i))
                ii = 0;
            i -= ii;

            lineoutput(&buffer[lasti], i - lasti);
            lasti = i;
            continue;
        }
        i++;
    }

    if(lasti > 0)
    {
        // remove all completely output lines
        boffset -= lasti;
        memmove(buffer, &buffer[lasti], boffset);
    }
}

void TextStreamBuffer::directflush(const char *str, unsigned int chars)
{
    unsigned int ii = 0;
    unsigned int i = 0;

    rawoutput(str, chars);

    for(i = 0; i < chars; i++)
    {
        if((str[i] == '\n') || (str[i] == '\r'))
        {
            i++;
            directoutput(str, i);
            str += i;
            chars -= i;
            i = 0;
            curline = 0;
        }
        else if(curline + i >= wrap)
        {
            // track back to last space up to 1/4 in the line to wrap
            for(ii = 0; ii < min((wrap / 4), i); ii++)
            {
                if(isspace(str[i - ii]))
                    break;
            }

            // if no space was found in the last 1/4 of the line to wrap, split it at the end anyway
            if(ii == min((wrap / 4), i))
                ii = 0;
            i -= ii;

            directoutput(str, i);
            directoutput("\n", 1);

            str += i;
            chars -= i;
            i = 0;
            curline = 0;
        }
    }

    if(chars > 0)
    {
        directoutput(str, chars);
        curline += chars;
    }
}

}
