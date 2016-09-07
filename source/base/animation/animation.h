//******************************************************************************
///
/// @file base/animation/animation.h
///
/// Declarations related to real-time rendering.
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

#ifndef POVRAY_BASE_ANIMATION_H
#define POVRAY_BASE_ANIMATION_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// Standard C++ header files
#include <vector>

// POV-Ray base header files
#include "base/fileinputoutput.h"
#include "base/pov_err.h"
#include "base/types.h"
#include "base/image/image.h"

namespace pov_base
{

class Animation
{
    public:
        enum CodecType
        {
            LosslessCodec = 0, // file format default choice
            LossyCodec = 1, // file format default choice
            PNGCodec,
            BMPCodec,
            JPEGCodec,
            MPEG1Codec, // i-frames only - note that JPEG library DCT can be reused [trf]
            MPEG2Codec // i-frames only - note that JPEG library DCT can be reused [trf]
        };

        enum FileType
        {
            AVI,
            MOV,
            MPEG
        };

        enum ColorEncodingType
        {
            RGB,
            YUV,
            YUV422,
            YUV420,
            YUV411,
            YUV410
        };

        struct ReadOptions
        {
            float gamma;
            bool gammacorrect;

            ReadOptions() : gamma(1.0f), gammacorrect(false) { }
        };

        struct WriteOptions
        {
            ColorEncodingType colorencoding;
            unsigned char compress; // relative quality from 100 best to 0 worst
            unsigned char bpcc; // bits per colour component
            bool alphachannel;
            float gamma;
            float blurradius; // blur radius 0.0 to 1.0 (for better lossy compression without aa) - turned off by 0.0
            float bluredgethreshold; // edge threshold 0.0 to 1.0 (grayscale difference of clipped pixels, only blur if difference larger than threshold) - turned off by 1.0
            float framespersecond; // has to support very odd framerates i.e. for NTSC - defaults to standard 24 fps cinema/movie frame rate

            WriteOptions() : colorencoding(YUV), compress(75), alphachannel(false), gamma(1.0f), blurradius(0.0), bluredgethreshold(1.0), framespersecond(24.0) { }
        };

        virtual ~Animation();

        static Animation *Open(FileType aftype, IStream *file, const ReadOptions& options = ReadOptions()); // reading only
        static Animation *Open(FileType aftype, CodecType codec, OStream *file, unsigned int w, unsigned int h, const WriteOptions& options = WriteOptions()); // writing only

        void AppendFrame(Image *image); // writing only - NOTE: This method reserves the right to *modify* the image passed to it!!! [trf]

        Image *ReadNextFrame(); // reading only

        float GetLengthInSeconds() const;
        unsigned int GetLengthInFrames() const;

        unsigned int GetCurrentFrame() const; // reading only
        void SetCurrentFrame(unsigned int frame); // reading only

        unsigned int GetWidth() const { return width; }
        unsigned int GetHeight() const { return height; }

        const vector<string>& GetWarnings() const;
        void ClearWarnings();
    protected:
        FileType fileType;
        IStream *inFile;
        OStream *outFile;
        unsigned int width;
        unsigned int height;
        ReadOptions readOptions;
        WriteOptions writeOptions;
        vector<string> warnings;
        CodecType codec;
        unsigned int currentFrame;
        unsigned int totalFrames;
        float frameDuration;

        Animation(FileType aftype, IStream *file, const ReadOptions& options);
        Animation(FileType aftype, CodecType codec, OStream *file, unsigned int w, unsigned int h, const WriteOptions& options);

        Image *ReadFrame(IStream *file);
        POV_LONG WriteFrame(OStream *file, const Image *image);

        void ComputeBlurMask(const Image& image, Image& mask);
        void GetBlurredPixel(const Image& image, unsigned int x, unsigned int y, float& red, float& green, float& blue);
    private:
        void *state;
        float blurMatrix[16][16]; // only uses 15 x 15 maximum (16 x 16 for better alignment)
        int blurMatrixRadius;

        /// not available
        Animation();
        /// not available
        Animation(const Animation&);
        /// not available
        Animation& operator=(Animation&);
};

}

#endif // POVRAY_BASE_ANIMATION_H
