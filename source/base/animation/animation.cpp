//******************************************************************************
///
/// @file base/animation/animation.cpp
///
/// Implementations related to real-time rendering.
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
#include "base/animation/animation.h"

// Boost header files
#include <boost/scoped_ptr.hpp>

// POV-Ray base header files
//#include "base/animation/avi.h"
#include "base/animation/moov.h"
//#include "base/animation/mpeg.h"
#include "base/image/bmp.h"
#include "base/image/jpeg_pov.h"
#include "base/image/png_pov.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

Animation::Animation(FileType aftype, IStream *file, const ReadOptions& options) :
    fileType(aftype),
    inFile(file),
    outFile(NULL),
    readOptions(options)
{
    float seconds = 0.0f;

    currentFrame = 0;

    switch(fileType)
    {
        case AVI:
        //  state = Avi::ReadFileHeader(inFile, seconds, totalFrames, codec, width, height, readOptions, warnings);
            break;
        case MOV:
            state = Moov::ReadFileHeader(inFile, seconds, totalFrames, codec, width, height, readOptions, warnings);
            break;
        case MPEG:
        //  state = Mpeg::ReadFileHeader(inFile, seconds, totalFrames, codec, width, height, readOptions, warnings);
            break;
    }

    if(state == NULL)
        throw POV_EXCEPTION(kCannotHandleDataErr, "Cannot read animation file header in the specified format!");

    frameDuration = seconds / float(totalFrames);
}

Animation::Animation(FileType aftype, CodecType c, OStream *file, unsigned int w, unsigned int h, const WriteOptions& options) :
    fileType(aftype),
    inFile(NULL),
    outFile(file),
    width(w),
    height(h),
    writeOptions(options),
    codec(c)
{
    totalFrames = 0;
    frameDuration = 1.0f / options.framespersecond;

    blurMatrixRadius = 7;

    switch(fileType)
    {
        case AVI:
        //  state = Avi::WriteFileHeader(outFile, codec, width, height, writeOptions, warnings);
            break;
        case MOV:
            state = Moov::WriteFileHeader(outFile, codec, width, height, writeOptions, warnings);
            break;
        case MPEG:
        //  state = Mpeg::WriteFileHeader(outFile, codec, width, height, writeOptions, warnings);
            break;
    }

    if(state == NULL)
        throw POV_EXCEPTION(kCannotHandleDataErr, "Cannot write animation file with the specified format and codec!");

    // TODO FIXME - build blur matrix (this code only builds an identity matrix)
    for(size_t y = 0; y < 15; y++)
    {
        for(size_t x = 0; x < 15; x++)
            blurMatrix[x][y] = 0.0f;
    }
    blurMatrix[blurMatrixRadius + 1][blurMatrixRadius + 1] = 1.0f;
}

Animation::~Animation()
{
    if(outFile != NULL)
    {
        switch(fileType)
        {
            case AVI:
            //  Avi::FinishWriteFile(outFile, writeOptions, warnings, state);
                break;
            case MOV:
                Moov::FinishWriteFile(outFile, writeOptions, warnings, state);
                break;
            case MPEG:
            //  Mpeg::FinishWriteFile(outFile, writeOptions, warnings, state);
                break;
        }
    }
    else if(inFile != NULL)
    {
        switch(fileType)
        {
            case AVI:
            //  Avi::FinishReadFile(inFile, warnings, state);
                break;
            case MOV:
                Moov::FinishReadFile(inFile, warnings, state);
                break;
            case MPEG:
            //  Mpeg::FinishReadFile(inFile, warnings, state);
                break;
        }
    }

    state = NULL;
}

Animation *Animation::Open(FileType aftype, IStream *file, const ReadOptions& options) // reading only
{
    return new Animation(aftype, file, options);
}

Animation *Animation::Open(FileType aftype, CodecType codec, OStream *file, unsigned int w, unsigned int h, const WriteOptions& options) // writing only
{
    return new Animation(aftype, codec, file, w, h, options);
}

void Animation::AppendFrame(Image *image) // writing only - NOTE: This method reserves the right to *modify* the image passed to it!!! [trf]
{
    if(writeOptions.blurradius > 0.0f)
    {
        boost::scoped_ptr<Image> mask(Image::Create(image->GetWidth(), image->GetHeight(), Image::Bit_Map));
        float r, g, b, f, t;

        mask->FillBitValue(false);

        if(writeOptions.bluredgethreshold < 1.0f)
            ComputeBlurMask(*image, *mask.get());

        for(int y = 0; y < image->GetHeight(); y++)
        {
            for(int x = 0; x < image->GetWidth(); x++)
            {
                if(mask->GetBitValue(x, y) == true)
                {
                    image->GetRGBFTValue(x, y, r, g, b, f, t);
                    GetBlurredPixel(*image, x, y, r, g, b);
                    image->SetRGBFTValue(x, y, r, g, b, f, t);
                }
            }
        }
    }

    WriteFrame(outFile, image);

    totalFrames++;
}

Image *Animation::ReadNextFrame() // reading only
{
    currentFrame++;

    return ReadFrame(inFile);
}

float Animation::GetLengthInSeconds() const
{
    return frameDuration * totalFrames;
}

unsigned int Animation::GetLengthInFrames() const
{
    return totalFrames;
}

unsigned int Animation::GetCurrentFrame() const // reading only
{
    return currentFrame;
}

void Animation::SetCurrentFrame(unsigned int frame) // reading only
{
    currentFrame = frame;
}

const vector<string>& Animation::GetWarnings() const
{
    return warnings;
}

void Animation::ClearWarnings()
{
    warnings.clear();
}

Image *Animation::ReadFrame(IStream *file)
{
    POV_LONG bytes = 0;
    Image *image = NULL;
    Image::ReadOptions options;

    options.defaultGamma = PowerLawGammaCurve::GetByDecodingGamma(readOptions.gamma);
    options.gammacorrect = readOptions.gammacorrect;
    options.itype = Image::RGBFT_Float;

    switch(fileType)
    {
        case AVI:
        //  Avi::PreReadFrame(file, currentFrame, bytes, codec, readOptions, warnings, state);
            break;
        case MOV:
            Moov::PreReadFrame(file, currentFrame, bytes, codec, readOptions, warnings, state);
            break;
    }

    POV_LONG prepos = file->tellg();

    switch(codec)
    {
        case PNGCodec:
            image = Png::Read(file, options);
            break;
        case BMPCodec:
            image = Bmp::Read(file, options);
            break;
        case JPEGCodec:
            image = Jpeg::Read(file, options);
            break;
        case MPEG1Codec:
        case MPEG2Codec:
        //  image = Mpeg::ReadFrame(file, currentFrame, codec, readOptions, warnings, state);
            break;
    }

    if(file->tellg() < (prepos + bytes))
        warnings.push_back("Frame decompressor read fewer bytes than expected.");
    else if(file->tellg() > (prepos + bytes))
        throw POV_EXCEPTION(kInvalidDataSizeErr, "Frame decompressor read more bytes than expected. The input file may be corrupted!");

    file->seekg(prepos + bytes, IOBase::seek_end);

    switch(fileType)
    {
        case AVI:
        //  Avi::PostReadFrame(file, currentFrame, bytes, codec, readOptions, warnings, state);
            break;
        case MOV:
            Moov::PostReadFrame(file, currentFrame, bytes, codec, readOptions, warnings, state);
            break;
    }

    return image;
}

POV_LONG Animation::WriteFrame(OStream *file, const Image *image)
{
    Image::WriteOptions options;

    options.bpcc = writeOptions.bpcc;
    options.alphachannel = writeOptions.alphachannel;
    options.compress = writeOptions.compress;
    // options.gamma = writeOptions.gamma;
    options.encodingGamma = PowerLawGammaCurve::GetByEncodingGamma(writeOptions.gamma);

    switch(fileType)
    {
        case AVI:
        //  Avi::PreWriteFrame(file, writeOptions, warnings, state);
            break;
        case MOV:
            Moov::PreWriteFrame(file, writeOptions, warnings, state);
            break;
    }

    POV_LONG bytes = file->tellg();

    switch(codec)
    {
        case PNGCodec:
            Png::Write(file, image, options);
            break;
        case BMPCodec:
            Bmp::Write(file, image, options);
            break;
        case JPEGCodec:
            // TODO FIXME Jpeg::Write(file, image, options);
            break;
        case MPEG1Codec:
        case MPEG2Codec:
        //  Mpeg::WriteFrame(file, image, codec, writeOptions, warnings, state);
            break;
    }

    bytes = (file->tellg() - bytes);

    switch(fileType)
    {
        case AVI:
        //  Avi::PostWriteFrame(file, bytes, writeOptions, warnings, state);
            break;
        case MOV:
            Moov::PostWriteFrame(file, bytes, writeOptions, warnings, state);
            break;
    }

    return bytes;
}

void Animation::ComputeBlurMask(const Image& image, Image& mask)
{
    for(int y = 0; y < image.GetHeight(); y++)
    {
        for(int x = 0; x < image.GetWidth(); x++)
        {
            for(int yy = -1; yy <= 1; yy++)
            {
                for(int xx = -1; xx <= 1; xx++)
                {
                    if(fabs(image.GetGrayValue(clip(x + xx, 0, int(image.GetWidth() - 1)), clip(y + yy, 0, int(image.GetHeight() - 1))) - image.GetGrayValue(x, y)) >= writeOptions.bluredgethreshold)
                        mask.SetBitValue(x, y, true);
                }
            }
        }
    }
}

void Animation::GetBlurredPixel(const Image& image, unsigned int x, unsigned int y, float& red, float& green, float& blue)
{
    red = green = blue = 0.0f;

    for(int yy = -blurMatrixRadius; yy <= blurMatrixRadius; yy++)
    {
        for(int xx = -blurMatrixRadius; xx <= blurMatrixRadius; xx++)
        {
            float scale = blurMatrix[blurMatrixRadius + xx][blurMatrixRadius + yy];
            float r = 0.0f, g = 0.0f, b = 0.0f;

            image.GetRGBValue(clip(int(x) + xx, 0, int(image.GetWidth() - 1)), clip(int(y) + yy, 0, int(image.GetHeight() - 1)), r, g, b);

            red += r * scale;
            green += g * scale;
            blue += b * scale;
        }
    }
}

}
