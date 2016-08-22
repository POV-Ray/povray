//******************************************************************************
///
/// @file base/image/image.cpp
///
/// Implementation of image containers.
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
#include "base/image/image.h"

// Standard C++ header files
#include <algorithm>
#include <vector>

// Standard POSIX header files
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

// POV-Ray base header files
#include "base/platformbase.h"
#include "base/safemath.h"
#include "base/image/bmp.h"
#include "base/image/gif.h"
#include "base/image/hdr.h"
#include "base/image/iff.h"
#include "base/image/jpeg_pov.h"
#include "base/image/openexr.h"
#include "base/image/pgm.h"
#include "base/image/png_pov.h"
#include "base/image/ppm.h"
#include "base/image/targa.h"
#include "base/image/tiff_pov.h"

#ifdef USE_SYSPROTO
#include "syspovprotobase.h"
#endif

// this must be the last file included
#include "base/povdebug.h"

#define CHECK_BOUNDS(x,y) POV_IMAGE_ASSERT(((x) < width) && ((y) < height))

#define ALPHA_OPAQUE                (1.0f)
#define ALPHA_OPAQUE_INT(MAX)       (MAX)

#define FT_OPAQUE                   (0.0f)
#define FT_OPAQUE_INT(MAX)          (0)

#define IS_NONZERO_RGB(r,g,b)       ((r)*(g)*(b) != 0.0f) // TODO FIXME - [CLi] this tests whether *all* channels are nonzero - is this desired?
#define IS_NONZERO_RGB_INT(r,g,b)   ((r)*(g)*(b) != 0)    // TODO FIXME - [CLi] this tests whether *all* channels are nonzero - is this desired?

namespace pov_base
{

using std::allocator;

template<class Allocator = allocator<bool> >
class BitMapImage : public Image
{
    public:
        BitMapImage(unsigned int w, unsigned int h) :
            Image(w, h, Bit_Map) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        BitMapImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, Bit_Map, m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        BitMapImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, Bit_Map, m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        BitMapImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, Bit_Map, m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        ~BitMapImage() { }

        bool IsOpaque() const
        {
            return true;
        }
        bool IsGrayscale() const
        {
            return true;
        }
        bool IsColour() const
        {
            return false;
        }
        bool IsFloat() const
        {
            return false;
        }
        bool IsInt() const
        {
            return true;
        }
        bool IsIndexed() const
        {
            return false;
        }
        bool IsGammaEncoded() const
        {
            return false;
        }
        bool HasAlphaChannel() const
        {
            return false;
        }
        bool HasFilterTransmit() const
        {
            return false;
        }
        unsigned int GetMaxIntValue() const
        {
            return 1;
        }
        bool TryDeferDecoding(GammaCurvePtr&, unsigned int)
        {
            return false;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            CHECK_BOUNDS(x, y);
            return pixels[x + y * size_t(width)];
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            CHECK_BOUNDS(x, y);
            if(pixels[x + y * size_t(width)] == true)
                return 1.0f;
            else
                return 0.0f;
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            gray = GetGrayValue(x, y);
            alpha = ALPHA_OPAQUE;
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            red = green = blue = GetGrayValue(x, y);
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            red = green = blue = GetGrayValue(x, y);
            alpha = ALPHA_OPAQUE;
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            red = green = blue = GetGrayValue(x, y);
            transm = FT_OPAQUE;
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            red = green = blue = GetGrayValue(x, y);
            filter = transm = FT_OPAQUE;
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = bit;
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = (gray != 0.0f);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = (gray != 0);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = (gray != 0.0f);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = (gray != 0);
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = IS_NONZERO_RGB(red, green, blue);
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = IS_NONZERO_RGB_INT(red, green, blue);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            SetRGBValue(x, y, col.red(), col.green(), col.blue());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float, float)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            SetRGBValue(x, y, col.red(), col.green(), col.blue());
        }

        void FillBitValue(bool bit)
        {
            fill(pixels.begin(), pixels.end(), bit);
        }
        void FillGrayValue(float gray)
        {
            FillBitValue(gray != 0.0f);
        }
        void FillGrayValue(unsigned int gray)
        {
            FillBitValue(gray != 0);
        }
        void FillGrayAValue(float gray, float)
        {
            FillBitValue(gray != 0.0f);
        }
        void FillGrayAValue(unsigned int gray, unsigned int)
        {
            FillBitValue(gray != 0);
        }
        void FillRGBValue(float red, float green, float blue)
        {
            FillBitValue(IS_NONZERO_RGB(red, green, blue));
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            FillBitValue(IS_NONZERO_RGB_INT(red, green, blue));
        }
        void FillRGBAValue(float red, float green, float blue, float)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            FillRGBValue(red, green, blue);
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            FillRGBValue(red, green, blue);
        }
        void FillRGBTValue(float red, float green, float blue, float)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            FillRGBValue(red, green, blue);
        }
        void FillRGBFTValue(float red, float green, float blue, float, float)
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            FillRGBValue(red, green, blue);
        }
    private:
        vector<bool, Allocator> pixels;
};

typedef BitMapImage<> MemoryBitMapImage;

template<class Allocator = allocator<unsigned char> >
class ColourMapImage : public Image
{
    public:
        ColourMapImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, Colour_Map, m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        ColourMapImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, Colour_Map, m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        ColourMapImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, Colour_Map, m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        ~ColourMapImage() { }

        bool IsOpaque() const
        {
            if((colormaptype != RGBAColourMap) && (colormaptype != RGBFTColourMap))
                return true;

            bool transp = false;

            for(size_t i = 0; i < colormap.size(); i++)
            {
                if(colormaptype == RGBAColourMap)
                    transp = (colormap[i].filter != ALPHA_OPAQUE); // with RGBAColourMap, .filter is actually alpha
                else if(colormaptype == RGBFTColourMap)
                    transp = (colormap[i].transm != FT_OPAQUE);
                if(transp == true)
                    break;
            }

            if(transp == false)
                return true;

            if(colormaptype == RGBAColourMap)
            {
                for(typename vector<unsigned char, Allocator>::const_iterator i(pixels.begin()); i != pixels.end(); i++)
                {
                    if(colormap[*i].filter != ALPHA_OPAQUE) // with RGBAColourMap, .filter is actually alpha
                        return false;
                }
            }
            else
            {
                for(typename vector<unsigned char, Allocator>::const_iterator i(pixels.begin()); i != pixels.end(); i++)
                {
                    if(colormap[*i].transm != FT_OPAQUE)
                        return false;
                }
            }

            return true;
        }
        bool IsGrayscale() const
        {
            return false;
        }
        bool IsColour() const
        {
            return true;
        }
        bool IsFloat() const
        {
            return false;
        }
        bool IsInt() const
        {
            return true;
        }
        bool IsIndexed() const
        {
            return true;
        }
        bool IsGammaEncoded() const
        {
            return false;
        }
        bool HasAlphaChannel() const
        {
            return (colormaptype == RGBAColourMap);
        }
        bool HasFilterTransmit() const
        {
            return (colormaptype == RGBFTColourMap);
        }
        unsigned int GetMaxIntValue() const
        {
            return 255;
        }
        bool TryDeferDecoding(GammaCurvePtr&, unsigned int)
        {
            return false;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            float red, green, blue, filter, transm, alpha;
            switch(colormaptype)
            {
                case RGBColourMap:
                    GetRGBValue(x, y, red, green, blue);
                    return IS_NONZERO_RGB(red, green, blue);
                case RGBAColourMap:
                    // TODO FIXME - [CLi] This takes into account opacity information; other bit-based code doesn't.
                    GetRGBAValue(x, y, red, green, blue, alpha);
                    return IS_NONZERO_RGB(red, green, blue) && (alpha == ALPHA_OPAQUE);
                case RGBFTColourMap:
                    // TODO FIXME - [CLi] This takes into account opacity information; other bit-based code doesn't.
                    GetRGBFTValue(x, y, red, green, blue, filter, transm);
                    return IS_NONZERO_RGB(red, green, blue) && (filter == FT_OPAQUE) && (transm == FT_OPAQUE);
                default:
                    return false;
            }
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            float red, green, blue, filter, transm, alpha;
            switch(colormaptype)
            {
                case RGBColourMap:
                    GetRGBValue(x, y, red, green, blue);
                    return RGB2Gray(red, green, blue);
                case RGBAColourMap:
                    GetRGBAValue(x, y, red, green, blue, alpha);
                    return RGB2Gray(red, green, blue);
                case RGBFTColourMap:
                    GetRGBFTValue(x, y, red, green, blue, filter, transm);
                    return RGB2Gray(red, green, blue);
                default:
                    return 0.0f;
            }
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            float red, green, blue, filter, transm;
            alpha = ALPHA_OPAQUE; // (unless noted otherwise)
            switch(colormaptype)
            {
                case RGBColourMap:
                    GetRGBValue(x, y, red, green, blue);
                    gray = RGB2Gray(red, green, blue);
                    return;
                case RGBAColourMap:
                    GetRGBAValue(x, y, red, green, blue, alpha);
                    gray = RGB2Gray(red, green, blue);
                    return;
                case RGBFTColourMap:
                    GetRGBFTValue(x, y, red, green, blue, filter, transm);
                    gray = RGB2Gray(red, green, blue);
                    alpha = RGBFTColour::FTtoA(filter, transm);
                    return;
                default:
                    gray = 0.0f;
                    return;
            }
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            CHECK_BOUNDS(x, y);
            MapEntry e(colormap[pixels[x + y * size_t(width)]]);
            red = e.red;
            green = e.green;
            blue = e.blue;
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            CHECK_BOUNDS(x, y);
            MapEntry e(colormap[pixels[x + y * size_t(width)]]);
            red = e.red;
            green = e.green;
            blue = e.blue;
            alpha = ALPHA_OPAQUE; // (unless noted otherwise)
            switch(colormaptype)
            {
                case RGBAColourMap:
                    alpha = e.filter; // with RGBAColourMap, .filter is actually alpha
                    return;
                case RGBFTColourMap:
                    alpha = RGBFTColour::FTtoA(e.filter, e.transm);
                    return;
                default:
                    return;
            }
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            CHECK_BOUNDS(x, y);
            MapEntry e(colormap[pixels[x + y * size_t(width)]]);
            red = e.red;
            green = e.green;
            blue = e.blue;
            transm = FT_OPAQUE; // (unless noted otherwise)
            switch(colormaptype)
            {
                case RGBAColourMap:
                    transm = 1.0 - e.filter; // with RGBAColourMap, .filter is actually alpha
                    return;
                case RGBFTColourMap:
                    transm = 1.0 - RGBFTColour::FTtoA(e.filter, e.transm);
                    return;
                default:
                    return;
            }
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            CHECK_BOUNDS(x, y);
            MapEntry e(colormap[pixels[x + y * size_t(width)]]);
            red = e.red;
            green = e.green;
            blue = e.blue;
            filter = FT_OPAQUE; // (unless noted otherwise)
            transm = FT_OPAQUE; // (unless noted otherwise)
            switch(colormaptype)
            {
                case RGBAColourMap:
                    RGBFTColour::AtoFT(e.filter, filter, transm); // with RGBAColourMap, .filter is actually alpha
                    return;
                case RGBFTColourMap:
                    filter = e.filter;
                    transm = e.transm;
                    return;
                default:
                    return;
            }
        }
        unsigned char GetIndexedValue(unsigned int x, unsigned int y)
        {
            CHECK_BOUNDS(x, y);
            return pixels[x + y * size_t(width)];
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = Bit2Map(bit);
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = Gray2Map(gray);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = Gray2Map(float(gray) / 255.0);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = GrayA2Map(gray, alpha);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = GrayA2Map(float(gray) / 255.0, float(alpha) / 255.0);
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = RGB2Map(red, green, blue);
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = RGB2Map(float(red) / 255.0, float(green) / 255.0, float(blue) / 255.0);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = RGBA2Map(red, green, blue, alpha);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = RGBA2Map(float(red) / 255.0, float(green) / 255.0, float(blue) / 255.0, float(alpha) / 255.0);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = RGBT2Map(red, green, blue, transm);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = RGBA2Map(col.red(), col.green(), col.blue(), col.Alpha());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float filter, float transm)
        {
            CHECK_BOUNDS(x, y);
            // [CLi 2009-09] this was dividing by 255 - which I presume to have been a bug.
            pixels[x + y * size_t(width)] = RGBFT2Map(red, green, blue, filter, transm);
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            CHECK_BOUNDS(x, y);
            // [CLi 2009-09] this was dividing by 255 - which I presume to have been a bug.
            pixels[x + y * size_t(width)] = RGBFT2Map(col.red(), col.green(), col.blue(), col.filter(), col.transm());
        }
        void SetIndexedValue(unsigned int x, unsigned int y, unsigned char index)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = index;
        }

        void FillBitValue(bool bit)
        {
            fill(pixels.begin(), pixels.end(), Bit2Map(bit));
        }
        void FillGrayValue(float gray)
        {
            fill(pixels.begin(), pixels.end(), Gray2Map(gray));
        }
        void FillGrayValue(unsigned int gray)
        {
            fill(pixels.begin(), pixels.end(), Gray2Map(float(gray) / 255.0));
        }
        void FillGrayAValue(float gray, float alpha)
        {
            fill(pixels.begin(), pixels.end(), GrayA2Map(gray, alpha));
        }
        void FillGrayAValue(unsigned int gray, unsigned int alpha)
        {
            fill(pixels.begin(), pixels.end(), GrayA2Map(float(gray) / 255.0, float(alpha) / 255.0));
        }
        void FillRGBValue(float red, float green, float blue)
        {
            fill(pixels.begin(), pixels.end(), RGB2Map(red, green, blue));
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            fill(pixels.begin(), pixels.end(), RGB2Map(float(red) / 255.0, float(green) / 255.0, float(blue) / 255.0));
        }
        void FillRGBAValue(float red, float green, float blue, float alpha)
        {
            fill(pixels.begin(), pixels.end(), RGBA2Map(red, green, blue, alpha));
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            fill(pixels.begin(), pixels.end(), RGBA2Map(float(red) / 255.0, float(green) / 255.0, float(blue) / 255.0, float(alpha) / 255.0));
        }
        void FillRGBTValue(float red, float green, float blue, float transm)
        {
            fill(pixels.begin(), pixels.end(), RGBT2Map(red, green, blue, transm));
        }
        void FillRGBFTValue(float red, float green, float blue, float filter, float transm)
        {
            // [CLi 2009-09] this was dividing by 255 - which I presume to have been a bug.
            fill(pixels.begin(), pixels.end(), RGBFT2Map(red, green, blue, filter, transm));
        }
    private:
        vector<unsigned char, Allocator> pixels;

        unsigned char Bit2Map(bool bit) const
        {
            if(bit == true)
                return Gray2Map(1.0f);
            else
                return Gray2Map(0.0f);
        }
        unsigned char Gray2Map(float gray) const
        {
            switch(colormaptype)
            {
                case RGBColourMap:
                    return FindBestRGB(gray, gray, gray);
                case RGBAColourMap:
                    return FindBestRGBA(gray, gray, gray, ALPHA_OPAQUE);
                case RGBFTColourMap:
                    return FindBestRGBFT(gray, gray, gray, FT_OPAQUE, FT_OPAQUE);
                default:
                    return 0;
            }
        }
        unsigned char GrayA2Map(float gray, float alpha) const
        {
            float filter, transm;
            switch(colormaptype)
            {
                case RGBColourMap:
                    return FindBestRGB(gray, gray, gray);
                case RGBAColourMap:
                    return FindBestRGBA(gray, gray, gray, alpha);
                case RGBFTColourMap:
                    RGBFTColour::AtoFT(alpha, filter, transm);
                    return FindBestRGBFT(gray, gray, gray, filter, transm);
                default:
                    return 0;
            }
        }
        unsigned char RGB2Map(float red, float green, float blue) const
        {
            switch(colormaptype)
            {
                case RGBColourMap:
                    return FindBestRGB(red, green, blue);
                case RGBAColourMap:
                    return FindBestRGBA(red, green, blue, ALPHA_OPAQUE);
                case RGBFTColourMap:
                    return FindBestRGBFT(red, green, blue, FT_OPAQUE, FT_OPAQUE);
                default:
                    return 0;
            }
        }
        unsigned char RGBA2Map(float red, float green, float blue, float alpha) const
        {
            float filter, transm;
            switch(colormaptype)
            {
                case RGBColourMap:
                    return FindBestRGB(red, green, blue);
                case RGBAColourMap:
                    return FindBestRGBA(red, green, blue, alpha);
                case RGBFTColourMap:
                    RGBFTColour::AtoFT(alpha, filter, transm);
                    return FindBestRGBFT(red, green, blue, filter, transm);
                default:
                    return 0;
            }
        }
        unsigned char RGBT2Map(float red, float green, float blue, float transm) const
        {
            switch(colormaptype)
            {
                case RGBColourMap:
                    return FindBestRGB(red, green, blue);
                case RGBAColourMap:
                    return FindBestRGBA(red, green, blue, 1.0 - transm);
                case RGBFTColourMap:
                    return FindBestRGBFT(red, green, blue, FT_OPAQUE, transm);
                default:
                    return 0;
            }
        }
        unsigned char RGBFT2Map(float red, float green, float blue, float filter, float transm) const
        {
            switch(colormaptype)
            {
                case RGBColourMap:
                    return FindBestRGB(red, green, blue);
                case RGBAColourMap:
                    return FindBestRGBA(red, green, blue, RGBFTColour::FTtoA(filter, transm));
                case RGBFTColourMap:
                    return FindBestRGBFT(red, green, blue, filter, transm);
                default:
                    return 0;
            }
        }
        unsigned char FindBestRGB(float red, float green, float blue) const
        {
            unsigned char best = 0;
            float diff = 3.0f;

            for(size_t i = 0; i < colormap.size(); i++)
            {
                float d(RGB2Gray(fabs(colormap[i].red - red), fabs(colormap[i].green - green), fabs(colormap[i].red - blue)));
                if(d < diff)
                {
                    d = diff;
                    best = (unsigned char)i;
                }
            }

            return best;
        }
        unsigned char FindBestRGBA(float red, float green, float blue, float alpha) const
        {
            unsigned char best = 0;
            float diff = 3.0f;

            for(size_t i = 0; i < colormap.size(); i++)
            {
                float d((RGB2Gray(fabs(colormap[i].red - red), fabs(colormap[i].green - green), fabs(colormap[i].red - blue)) * 3.0f +
                         fabs(colormap[i].filter - alpha)) / 4.0f);  // with RGBAColourMap, .filter is actually alpha
                if(d < diff)
                {
                    d = diff;
                    best = (unsigned char)i;
                }
            }

            return best;
        }
        unsigned char FindBestRGBFT(float red, float green, float blue, float filter, float transm) const
        {
            unsigned char best = 0;
            float diff = 3.0f;

            for(size_t i = 0; i < colormap.size(); i++)
            {
                float d((RGB2Gray(fabs(colormap[i].red - red), fabs(colormap[i].green - green), fabs(colormap[i].red - blue)) * 3.0f +
                         fabs(colormap[i].filter - filter) + fabs(colormap[i].transm - transm)) / 5.0f);
                if(d < diff)
                {
                    d = diff;
                    best = (unsigned char)i;
                }
            }

            return best;
        }
};

typedef ColourMapImage<> MemoryColourMapImage;

template<typename T, unsigned int TMAX, int IDT, class Allocator = allocator<T> >
class GrayImage : public Image
{
    public:
        GrayImage(unsigned int w, unsigned int h) :
            Image(w, h, ImageDataType(IDT)) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        GrayImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        GrayImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        GrayImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        ~GrayImage() { }

        bool IsOpaque() const
        {
            return true;
        }
        bool IsGrayscale() const
        {
            return true;
        }
        bool IsColour() const
        {
            return false;
        }
        bool IsFloat() const
        {
            return false;
        }
        bool IsInt() const
        {
            return true;
        }
        bool IsGammaEncoded() const
        {
            return false;
        }
        bool IsIndexed() const
        {
            return false;
        }
        bool HasAlphaChannel() const
        {
            return false;
        }
        bool HasFilterTransmit() const
        {
            return false;
        }
        unsigned int GetMaxIntValue() const
        {
            return TMAX;
        }
        bool TryDeferDecoding(GammaCurvePtr&, unsigned int)
        {
            return false;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            CHECK_BOUNDS(x, y);
            return (pixels[x + y * size_t(width)] != 0);
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            CHECK_BOUNDS(x, y);
            return float(pixels[x + y * size_t(width)]) / float(TMAX);
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            CHECK_BOUNDS(x, y);
            gray = float(pixels[x + y * size_t(width)]) / float(TMAX);
            alpha = ALPHA_OPAQUE;
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            red = green = blue = GetGrayValue(x, y);
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            red = green = blue = GetGrayValue(x, y);
            alpha = ALPHA_OPAQUE;
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            red = green = blue = GetGrayValue(x, y);
            transm = FT_OPAQUE;
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            red = green = blue = GetGrayValue(x, y);
            filter = transm = FT_OPAQUE;
        }
        unsigned char GetIndexedValue(unsigned int x, unsigned int y)
        {
            CHECK_BOUNDS(x, y);
            return (unsigned char)(int(pixels[x + y * size_t(width)]) / ((TMAX + 1) >> 8));
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            if(bit == true)
                SetGrayValue(x, y, TMAX);
            else
                SetGrayValue(x, y, (unsigned int)0);
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = T(gray * float(TMAX));
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = T(gray);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = T(gray * float(TMAX));
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = T(gray);
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            SetGrayValue(x, y, RGB2Gray(red, green, blue));
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            SetGrayValue(x, y, RGB2Gray(float(red) / float(TMAX), float(green) / float(TMAX), float(blue) / float(TMAX)));
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            SetRGBValue(x, y, col.red(), col.green(), col.blue());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float, float)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            SetRGBValue(x, y, col.red(), col.green(), col.blue());
        }

        void FillBitValue(bool bit)
        {
            if(bit == true)
                FillGrayValue(TMAX);
            else
                FillGrayValue((unsigned int)0);
        }
        void FillGrayValue(float gray)
        {
            FillGrayValue((unsigned int)(gray * float(TMAX)));
        }
        void FillGrayValue(unsigned int gray)
        {
            fill(pixels.begin(), pixels.end(), T(gray));
        }
        void FillGrayAValue(float gray, float)
        {
            FillGrayValue(gray);
        }
        void FillGrayAValue(unsigned int gray, unsigned int)
        {
            FillGrayValue(gray);
        }
        void FillRGBValue(float red, float green, float blue)
        {
            FillGrayValue(RGB2Gray(red, green, blue));
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            FillGrayValue(RGB2Gray(float(red) / float(TMAX), float(green) / float(TMAX), float(blue) / float(TMAX)));
        }
        void FillRGBAValue(float red, float green, float blue, float)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBTValue(float red, float green, float blue, float)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBFTValue(float red, float green, float blue, float, float)
        {
            FillRGBValue(red, green, blue);
        }
    private:
        vector<T, Allocator> pixels;
};

typedef GrayImage<unsigned char, 255, Image::Gray_Int8> MemoryGray8Image;

typedef GrayImage<unsigned short, 65535, Image::Gray_Int16> MemoryGray16Image;

template<typename T, unsigned int TMAX, int IDT, class Allocator = allocator<T> >
class GrayAImage : public Image
{
    public:
        GrayAImage(unsigned int w, unsigned int h) :
            Image(w, h, ImageDataType(IDT)) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 2u)); FillBitValue(false); }
        GrayAImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 2u)); FillBitValue(false); }
        GrayAImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 2u)); FillBitValue(false); }
        GrayAImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 2u)); FillBitValue(false); }
        ~GrayAImage() { }

        bool IsOpaque() const
        {
            for(typename vector<T, Allocator>::const_iterator i(pixels.begin()); i != pixels.end(); i += 2)
            {
                if(i[1] < TMAX)
                    return false;
            }

            return true;
        }
        bool IsGrayscale() const
        {
            return true;
        }
        bool IsColour() const
        {
            return false;
        }
        bool IsFloat() const
        {
            return false;
        }
        bool IsInt() const
        {
            return true;
        }
        bool IsIndexed() const
        {
            return false;
        }
        bool IsGammaEncoded() const
        {
            return false;
        }
        bool HasAlphaChannel() const
        {
            return true;
        }
        bool HasFilterTransmit() const
        {
            return false;
        }
        unsigned int GetMaxIntValue() const
        {
            return TMAX;
        }
        bool TryDeferDecoding(GammaCurvePtr&, unsigned int)
        {
            return false;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            CHECK_BOUNDS(x, y);
            return (pixels[(x + y * size_t(width)) * 2] != 0);
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            CHECK_BOUNDS(x, y);
            return float(pixels[(x + y * size_t(width)) * 2]) / float(TMAX);
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            CHECK_BOUNDS(x, y);
            gray  = float(pixels[(x + y * size_t(width)) * 2])     / float(TMAX);
            alpha = float(pixels[(x + y * size_t(width)) * 2 + 1]) / float(TMAX);
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            red = green = blue = GetGrayValue(x, y);
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            GetGrayAValue(x, y, red, alpha);
            green = blue = red;
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            float alpha;
            GetGrayAValue(x, y, red, alpha);
            green = blue = red;
            transm = 1.0 - alpha;
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            float alpha;
            GetGrayAValue(x, y, red, alpha);
            green = blue = red;
            RGBFTColour::AtoFT(alpha, filter, transm);
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            if(bit == true)
                SetGrayAValue(x, y, TMAX, ALPHA_OPAQUE_INT(TMAX));
            else
                SetGrayAValue(x, y, (unsigned int)0, ALPHA_OPAQUE_INT(TMAX));
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            SetGrayAValue(x, y, gray, ALPHA_OPAQUE);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            SetGrayAValue(x, y, gray, ALPHA_OPAQUE_INT(TMAX));
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 2]     = T(gray * float(TMAX));
            pixels[(x + y * size_t(width)) * 2 + 1] = T(alpha * float(TMAX));
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 2]     = gray;
            pixels[(x + y * size_t(width)) * 2 + 1] = alpha;
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            SetGrayValue(x, y, RGB2Gray(red, green, blue));
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            SetGrayValue(x, y, RGB2Gray(float(red) / float(TMAX), float(green) / float(TMAX), float(blue) / float(TMAX)));
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float alpha)
        {
            SetGrayAValue(x, y, RGB2Gray(red, green, blue), alpha);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            // TODO FIXME - this unnecessarily converts alpha from int to float, requiring it to be converted back to int
            SetGrayAValue(x, y, RGB2Gray(float(red) / float(TMAX), float(green) / float(TMAX), float(blue) / float(TMAX)), float(alpha) / float(TMAX));
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            SetGrayAValue(x, y, RGB2Gray(red, green, blue), 1.0 - transm);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            SetGrayAValue(x, y, RGB2Gray(col.red(), col.green(), col.blue()), col.Alpha());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float filter, float transm)
        {
            SetGrayAValue(x, y, RGB2Gray(red, green, blue), RGBFTColour::FTtoA(filter, transm));
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            SetGrayAValue(x, y, RGB2Gray(col.red(), col.green(), col.blue()), col.FTtoA());
        }

        void FillBitValue(bool bit)
        {
            if(bit == true)
                FillGrayValue(TMAX);
            else
                FillGrayValue((unsigned int)0);
        }
        void FillGrayValue(float gray)
        {
            FillGrayValue((unsigned int)(gray * float(TMAX)));
        }
        void FillGrayValue(unsigned int gray)
        {
            FillGrayAValue(gray, ALPHA_OPAQUE_INT(TMAX));
        }
        void FillGrayAValue(float gray, float alpha)
        {
            // [CLi 2009-09] this was dividing by float(TMAX) - which I presume to have been a bug.
            T g(gray * float(TMAX)), a(alpha * float(TMAX));
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = g;
                i++;
                *i = a;
            }
        }
        void FillGrayAValue(unsigned int gray, unsigned int alpha)
        {
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = T(gray);
                i++;
                *i = T(alpha);
            }
        }
        void FillRGBValue(float red, float green, float blue)
        {
            FillGrayValue(RGB2Gray(red, green, blue));
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            FillGrayValue(RGB2Gray(float(red) / float(TMAX), float(green) / float(TMAX), float(blue) / float(TMAX)));
        }
        void FillRGBAValue(float red, float green, float blue, float alpha)
        {
            FillGrayAValue(RGB2Gray(red, green, blue), alpha);
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            // TODO FIXME - this unnecessarily converts alpha from int to float, requiring it to be converted back to int
            FillGrayAValue(RGB2Gray(float(red) / float(TMAX), float(green) / float(TMAX), float(blue) / float(TMAX)), float(alpha) / float(TMAX));
        }
        void FillRGBTValue(float red, float green, float blue, float transm)
        {
            FillGrayAValue(RGB2Gray(red, green, blue), 1.0 - transm);
        }
        void FillRGBFTValue(float red, float green, float blue, float filter, float transm)
        {
            FillGrayAValue(RGB2Gray(red, green, blue), RGBFTColour::FTtoA(filter, transm));
        }
    private:
        vector<T, Allocator> pixels;
};

typedef GrayAImage<unsigned char, 255, Image::GrayA_Int8> MemoryGrayA8Image;

typedef GrayAImage<unsigned short, 65535, Image::GrayA_Int16> MemoryGrayA16Image;

template<typename T, unsigned int TMAX, int IDT, class Allocator = allocator<T> >
class RGBImage : public Image
{
    public:
        RGBImage(unsigned int w, unsigned int h) :
            Image(w, h, ImageDataType(IDT)) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 3u)); FillBitValue(false); }
        RGBImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 3u)); FillBitValue(false); }
        RGBImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 3u)); FillBitValue(false); }
        RGBImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 3u)); FillBitValue(false); }
        ~RGBImage() { }

        bool IsOpaque() const
        {
            return true;
        }
        bool IsGrayscale() const
        {
            return false;
        }
        bool IsColour() const
        {
            return true;
        }
        bool IsFloat() const
        {
            return false;
        }
        bool IsInt() const
        {
            return true;
        }
        bool IsIndexed() const
        {
            return false;
        }
        bool IsGammaEncoded() const
        {
            return false;
        }
        bool HasAlphaChannel() const
        {
            return false;
        }
        bool HasFilterTransmit() const
        {
            return false;
        }
        unsigned int GetMaxIntValue() const
        {
            return TMAX;
        }
        bool TryDeferDecoding(GammaCurvePtr&, unsigned int)
        {
            return false;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            float red, green, blue;
            GetRGBValue(x, y, red, green, blue);
            return IS_NONZERO_RGB(red, green, blue);
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            float red, green, blue;
            GetRGBValue(x, y, red, green, blue);
            return RGB2Gray(red, green, blue);
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            gray = GetGrayValue(x, y);
            alpha = ALPHA_OPAQUE;
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            CHECK_BOUNDS(x, y);
            red   = float(pixels[(x + y * size_t(width)) * 3])     / float(TMAX);
            green = float(pixels[(x + y * size_t(width)) * 3 + 1]) / float(TMAX);
            blue  = float(pixels[(x + y * size_t(width)) * 3 + 2]) / float(TMAX);
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            GetRGBValue(x, y, red, green, blue);
            alpha = ALPHA_OPAQUE;
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            GetRGBValue(x, y, red, green, blue);
            transm = FT_OPAQUE;
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            GetRGBValue(x, y, red, green, blue);
            filter = transm = FT_OPAQUE;
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            if(bit == true)
                SetGrayValue(x, y, TMAX);
            else
                SetGrayValue(x, y, (unsigned int)0);
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            SetRGBValue(x, y, gray, gray, gray);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width) * 3]     =
            pixels[x + y * size_t(width) * 3 + 1] =
            pixels[x + y * size_t(width) * 3 + 2] = gray;
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width) * 3]     =
            pixels[x + y * size_t(width) * 3 + 1] =
            pixels[x + y * size_t(width) * 3 + 2] = T(gray * float(TMAX));
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width) * 3]     =
            pixels[x + y * size_t(width) * 3 + 1] =
            pixels[x + y * size_t(width) * 3 + 2] = gray;
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 3]     = T(red   * float(TMAX));
            pixels[(x + y * size_t(width)) * 3 + 1] = T(green * float(TMAX));
            pixels[(x + y * size_t(width)) * 3 + 2] = T(blue  * float(TMAX));
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 3]     = T(red);
            pixels[(x + y * size_t(width)) * 3 + 1] = T(green);
            pixels[(x + y * size_t(width)) * 3 + 2] = T(blue);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            SetRGBValue(x, y, col.red(), col.green(), col.blue());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float, float)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            SetRGBValue(x, y, col.red(), col.green(), col.blue());
        }

        void FillBitValue(bool bit)
        {
            if(bit == true)
                FillGrayValue(TMAX);
            else
                FillGrayValue((unsigned int)0);
        }
        void FillGrayValue(float gray)
        {
            FillGrayValue((unsigned int)(gray * float(TMAX)));
        }
        void FillGrayValue(unsigned int gray)
        {
            fill(pixels.begin(), pixels.end(), gray);
        }
        void FillGrayAValue(float gray, float)
        {
            FillRGBValue(gray, gray, gray);
        }
        void FillGrayAValue(unsigned int gray, unsigned int)
        {
            FillRGBValue(gray, gray, gray);
        }
        void FillRGBValue(float red, float green, float blue)
        {
            // [CLi 2009-09] this was dividing by float(TMAX) - which I presume to have been a bug.
            T r(red * float(TMAX)), g(green * float(TMAX)), b(blue * float(TMAX));
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = r;
                i++;
                *i = g;
                i++;
                *i = b;
            }
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = T(red);
                i++;
                *i = T(green);
                i++;
                *i = T(blue);
            }
        }
        void FillRGBAValue(float red, float green, float blue, float)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBTValue(float red, float green, float blue, float)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBFTValue(float red, float green, float blue, float, float)
        {
            FillRGBValue(red, green, blue);
        }
    private:
        vector<T, Allocator> pixels;
};

typedef RGBImage<unsigned char, 255, Image::RGB_Int8> MemoryRGB8Image;

typedef RGBImage<unsigned short, 65535, Image::RGB_Int16> MemoryRGB16Image;

template<typename T, unsigned int TMAX, int IDT, class Allocator = allocator<T> >
class RGBAImage : public Image
{
    public:
        RGBAImage(unsigned int w, unsigned int h) :
            Image(w, h, ImageDataType(IDT)) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 4u)); FillBitValue(false); }
        RGBAImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 4u)); FillBitValue(false); }
        RGBAImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 4u)); FillBitValue(false); }
        RGBAImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 4u)); FillBitValue(false); }
        ~RGBAImage() { }

        bool IsOpaque() const
        {
            for(typename vector<T, Allocator>::const_iterator i(pixels.begin()); i != pixels.end(); i += 4)
            {
                if(i[3] < TMAX)
                    return false;
            }

            return true;
        }
        bool IsGrayscale() const
        {
            return false;
        }
        bool IsColour() const
        {
            return true;
        }
        bool IsFloat() const
        {
            return false;
        }
        bool IsInt() const
        {
            return true;
        }
        bool IsIndexed() const
        {
            return false;
        }
        bool IsGammaEncoded() const
        {
            return false;
        }
        bool HasAlphaChannel() const
        {
            return true;
        }
        bool HasFilterTransmit() const
        {
            return false;
        }
        unsigned int GetMaxIntValue() const
        {
            return TMAX;
        }
        bool TryDeferDecoding(GammaCurvePtr&, unsigned int)
        {
            return false;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            float red, green, blue, alpha;
            GetRGBAValue(x, y, red, green, blue, alpha);
            return IS_NONZERO_RGB(red, green, blue);
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            float red, green, blue, alpha;
            GetRGBAValue(x, y, red, green, blue, alpha);
            return RGB2Gray(red, green, blue);
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            float red, green, blue;
            GetRGBAValue(x, y, red, green, blue, alpha);
            gray = RGB2Gray(red, green, blue);
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            float alpha;
            GetRGBAValue(x, y, red, green, blue, alpha);
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            CHECK_BOUNDS(x, y);
            red   = float(pixels[(x + y * size_t(width)) * 4])     / float(TMAX);
            green = float(pixels[(x + y * size_t(width)) * 4 + 1]) / float(TMAX);
            blue  = float(pixels[(x + y * size_t(width)) * 4 + 2]) / float(TMAX);
            alpha = float(pixels[(x + y * size_t(width)) * 4 + 3]) / float(TMAX);
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            float alpha;
            GetRGBAValue(x, y, red, green, blue, alpha);
            transm = 1.0 - alpha;
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            float alpha;
            GetRGBAValue(x, y, red, green, blue, alpha);
            RGBFTColour::AtoFT(alpha, filter, transm);
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            if(bit == true)
                SetGrayValue(x, y, TMAX);
            else
                SetGrayValue(x, y, (unsigned int)0);
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            SetRGBAValue(x, y, gray, gray, gray, ALPHA_OPAQUE);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            SetRGBAValue(x, y, gray, gray, gray, ALPHA_OPAQUE_INT(TMAX));
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float alpha)
        {
            SetRGBAValue(x, y, gray, gray, gray, alpha);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int alpha)
        {
            SetRGBAValue(x, y, gray, gray, gray, alpha);
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            SetRGBAValue(x, y, red, green, blue, ALPHA_OPAQUE);
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            SetRGBAValue(x, y, red, green, blue, ALPHA_OPAQUE_INT(TMAX));
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 4]     = T(red   * float(TMAX));
            pixels[(x + y * size_t(width)) * 4 + 1] = T(green * float(TMAX));
            pixels[(x + y * size_t(width)) * 4 + 2] = T(blue  * float(TMAX));
            pixels[(x + y * size_t(width)) * 4 + 3] = T(alpha * float(TMAX));
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 4]     = T(red);
            pixels[(x + y * size_t(width)) * 4 + 1] = T(green);
            pixels[(x + y * size_t(width)) * 4 + 2] = T(blue);
            pixels[(x + y * size_t(width)) * 4 + 3] = T(alpha);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            SetRGBAValue(x, y, red, green, blue, 1.0 - transm);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 4]     = T(col.red()   * float(TMAX));
            pixels[(x + y * size_t(width)) * 4 + 1] = T(col.green() * float(TMAX));
            pixels[(x + y * size_t(width)) * 4 + 2] = T(col.blue()  * float(TMAX));
            pixels[(x + y * size_t(width)) * 4 + 3] = T(col.Alpha() * float(TMAX));
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float filter, float transm)
        {
            SetRGBAValue(x, y, red, green, blue, RGBFTColour::FTtoA(filter, transm));
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 4]     = T(col.red()   * float(TMAX));
            pixels[(x + y * size_t(width)) * 4 + 1] = T(col.green() * float(TMAX));
            pixels[(x + y * size_t(width)) * 4 + 2] = T(col.blue()  * float(TMAX));
            pixels[(x + y * size_t(width)) * 4 + 3] = T(col.FTtoA() * float(TMAX));
        }

        void FillBitValue(bool bit)
        {
            if(bit == true)
                FillGrayValue(TMAX);
            else
                FillGrayValue((unsigned int)0);
        }
        void FillGrayValue(float gray)
        {
            FillRGBAValue(gray, gray, gray, ALPHA_OPAQUE);
        }
        void FillGrayValue(unsigned int gray)
        {
            FillRGBAValue(gray, gray, gray, ALPHA_OPAQUE_INT(TMAX));
        }
        void FillGrayAValue(float gray, float alpha)
        {
            FillRGBAValue(gray, gray, gray, alpha);
        }
        void FillGrayAValue(unsigned int gray, unsigned int alpha)
        {
            FillRGBAValue(gray, gray, gray, alpha);
        }
        void FillRGBValue(float red, float green, float blue)
        {
            FillRGBAValue(red, green, blue, ALPHA_OPAQUE);
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            FillRGBAValue(red, green, blue, ALPHA_OPAQUE_INT(TMAX));
        }
        void FillRGBAValue(float red, float green, float blue, float alpha)
        {
            // [CLi 2009-09] this was dividing by float(TMAX) - which I presume to have been a bug.
            T r(red * float(TMAX)), g(green * float(TMAX)), b(blue * float(TMAX)), a(alpha * float(TMAX));
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = r;
                i++;
                *i = g;
                i++;
                *i = b;
                i++;
                *i = a;
            }
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = T(red);
                i++;
                *i = T(green);
                i++;
                *i = T(blue);
                i++;
                *i = T(alpha);
            }
        }
        void FillRGBTValue(float red, float green, float blue, float transm)
        {
            FillRGBAValue(red, green, blue, 1.0 - transm);
        }
        void FillRGBFTValue(float red, float green, float blue, float filter, float transm)
        {
            FillRGBAValue(red, green, blue, RGBFTColour::FTtoA(filter, transm));
        }
    private:
        vector<T, Allocator> pixels;
};

typedef RGBAImage<unsigned char, 255, Image::RGBA_Int8> MemoryRGBA8Image;

typedef RGBAImage<unsigned short, 65535, Image::RGBA_Int16> MemoryRGBA16Image;

template<class PixelContainer = vector<float, allocator<float> > >
class RGBFTImage : public Image
{
    public:
        RGBFTImage(unsigned int w, unsigned int h) :
            Image(w, h, RGBFT_Float) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 5u)); FillBitValue(false); }
        RGBFTImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, RGBFT_Float, m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 5u)); FillBitValue(false); }
        RGBFTImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, RGBFT_Float, m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 5u)); FillBitValue(false); }
        RGBFTImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, RGBFT_Float, m) { pixels.resize(SafeUnsignedProduct<size_t>(w, h, 5u)); FillBitValue(false); }
        ~RGBFTImage() { }

        bool IsOpaque() const
        {
            for(typename PixelContainer::const_iterator i(pixels.begin()); i != pixels.end(); i += 5)
            {
                if(i[4] > 0.0f) // TODO FIXME - this ignores filter
                    return false;
            }
            return true;
        }
        bool IsGrayscale() const
        {
            return false;
        }
        bool IsColour() const
        {
            return true;
        }
        bool IsFloat() const
        {
            return true;
        }
        bool IsInt() const
        {
            return false;
        }
        bool IsIndexed() const
        {
            return false;
        }
        bool IsGammaEncoded() const
        {
            return false;
        }
        bool HasAlphaChannel() const
        {
            return false;
        }
        bool HasFilterTransmit() const
        {
            return true;
        }
        unsigned int GetMaxIntValue() const
        {
            return 255;
        }
        bool TryDeferDecoding(GammaCurvePtr&, unsigned int)
        {
            return false;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            float red, green, blue, filter, transm;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
            return IS_NONZERO_RGB(red, green, blue);
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            float red, green, blue, filter, transm;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
            return RGB2Gray(red, green, blue);
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            float red, green, blue, filter, transm;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
            gray = RGB2Gray(red, green, blue);
            alpha = RGBFTColour::FTtoA(filter, transm);
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            float filter, transm;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            float filter, transm;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
            alpha = RGBFTColour::FTtoA(filter, transm);
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            float filter;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
            transm = 1.0 - RGBFTColour::FTtoA(filter, transm);
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            CHECK_BOUNDS(x, y);
            red    = pixels[(x + y * size_t(width)) * 5];
            green  = pixels[(x + y * size_t(width)) * 5 + 1];
            blue   = pixels[(x + y * size_t(width)) * 5 + 2];
            filter = pixels[(x + y * size_t(width)) * 5 + 3];
            transm = pixels[(x + y * size_t(width)) * 5 + 4];
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            if(bit == true)
                SetGrayValue(x, y, 1.0f);
            else
                SetGrayValue(x, y, 0.0f);
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            SetRGBFTValue(x, y, gray, gray, gray, FT_OPAQUE, FT_OPAQUE);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            SetGrayValue(x, y, float(gray) / 255.0f);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float alpha)
        {
            float filter, transm;
            RGBFTColour::AtoFT(alpha, filter, transm);
            SetRGBFTValue(x, y, gray, gray, gray, filter, transm);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int alpha)
        {
            float c = float(gray) / 255.0f;
            float filter, transm;
            RGBFTColour::AtoFT(float(alpha) / 255.0f, filter, transm);
            SetRGBFTValue(x, y, c, c, c, filter, transm);
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            SetRGBFTValue(x, y, red, green, blue, FT_OPAQUE, FT_OPAQUE);
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            SetRGBFTValue(x, y, float(red) / 255.0f, float(green) / 255.0f, float(blue) / 255.0f, FT_OPAQUE, FT_OPAQUE);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float alpha)
        {
            float filter, transm;
            RGBFTColour::AtoFT(alpha, filter, transm);
            SetRGBFTValue(x, y, red, green, blue, filter, transm);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            float filter, transm;
            RGBFTColour::AtoFT(float(alpha) / 255.0f, filter, transm);
            SetRGBFTValue(x, y, float(red) / 255.0f, float(green) / 255.0f, float(blue) / 255.0f, filter, transm);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            SetRGBFTValue(x, y, red, green, blue, FT_OPAQUE, transm);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            SetRGBFTValue(x, y, col.red(), col.green(), col.blue(), FT_OPAQUE, col.transm());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float filter, float transm)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 5]     = red;
            pixels[(x + y * size_t(width)) * 5 + 1] = green;
            pixels[(x + y * size_t(width)) * 5 + 2] = blue;
            pixels[(x + y * size_t(width)) * 5 + 3] = filter;
            pixels[(x + y * size_t(width)) * 5 + 4] = transm;
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 5]     = col.red();
            pixels[(x + y * size_t(width)) * 5 + 1] = col.green();
            pixels[(x + y * size_t(width)) * 5 + 2] = col.blue();
            pixels[(x + y * size_t(width)) * 5 + 3] = col.filter();
            pixels[(x + y * size_t(width)) * 5 + 4] = col.transm();
        }

        void FillBitValue(bool bit)
        {
            if(bit == true)
                FillGrayValue(1.0f);
            else
                FillGrayValue(0.0f);
        }
        void FillGrayValue(float gray)
        {
            FillRGBFTValue(gray, gray, gray, FT_OPAQUE, FT_OPAQUE);
        }
        void FillGrayValue(unsigned int gray)
        {
            FillGrayValue(float(gray) / 255.0f);
        }
        void FillGrayAValue(float gray, float alpha)
        {
            float filter, transm;
            RGBFTColour::AtoFT(alpha, filter, transm);
            FillRGBFTValue(gray, gray, gray, filter, transm);
        }
        void FillGrayAValue(unsigned int gray, unsigned int alpha)
        {
            FillGrayAValue(float(gray) / 255.0f, float(alpha) / 255.0f);
        }
        void FillRGBValue(float red, float green, float blue)
        {
            FillRGBFTValue(red, green, blue, FT_OPAQUE, FT_OPAQUE);
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            FillRGBFTValue(float(red) / 255.0f, float(green) / 255.0f, float(blue) / 255.0f, FT_OPAQUE, FT_OPAQUE);
        }
        void FillRGBAValue(float red, float green, float blue, float alpha)
        {
            float filter, transm;
            RGBFTColour::AtoFT(alpha, filter, transm);
            FillRGBFTValue(red, green, blue, filter, transm);
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            float filter, transm;
            RGBFTColour::AtoFT(float(alpha) / 255.0f, filter, transm);
            FillRGBFTValue(float(red) / 255.0f, float(green) / 255.0f, float(blue) / 255.0f, filter, transm);
        }
        void FillRGBTValue(float red, float green, float blue, float transm)
        {
            FillRGBFTValue(red, green, blue, FT_OPAQUE, transm);
        }
        void FillRGBFTValue(float red, float green, float blue, float filter, float transm)
        {
            for(typename PixelContainer::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = red;
                i++;
                *i = green;
                i++;
                *i = blue;
                i++;
                *i = filter;
                i++;
                *i = transm;
            }
        }
    private:
        PixelContainer pixels;
};

typedef RGBFTImage<> MemoryRGBFTImage;

template<typename T, unsigned int TMAX, int IDT, class Allocator = allocator<T> >
class NonlinearGrayImage : public Image
{
    public:
        NonlinearGrayImage(unsigned int w, unsigned int h) :
            Image(w, h, ImageDataType(IDT)), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        NonlinearGrayImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        NonlinearGrayImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        NonlinearGrayImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h)); FillBitValue(false); }
        ~NonlinearGrayImage() { }

        bool IsOpaque() const
        {
            return true;
        }
        bool IsGrayscale() const
        {
            return true;
        }
        bool IsColour() const
        {
            return false;
        }
        bool IsFloat() const
        {
            return false;
        }
        bool IsInt() const
        {
            return true;
        }
        bool IsIndexed() const
        {
            return false;
        }
        bool IsGammaEncoded() const
        {
            return true;
        }
        bool HasAlphaChannel() const
        {
            return false;
        }
        bool HasFilterTransmit() const
        {
            return false;
        }
        unsigned int GetMaxIntValue() const
        {
            return TMAX;
        }
        bool TryDeferDecoding(GammaCurvePtr& g, unsigned int max)
        {
            if (max != TMAX) return false;
            if (!GammaCurve::IsNeutral(gamma)) return !g;
            gamma.swap(g);
            gammaLUT = gamma->GetLookupTable(TMAX);
            return true;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            CHECK_BOUNDS(x, y);
            return (pixels[x + y * size_t(width)] != 0);
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            CHECK_BOUNDS(x, y);
            return gammaLUT[pixels[x + y * size_t(width)]];
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            CHECK_BOUNDS(x, y);
            gray = gammaLUT[pixels[x + y * size_t(width)]];
            alpha = ALPHA_OPAQUE;
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            red = green = blue = GetGrayValue(x, y);
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            red = green = blue = GetGrayValue(x, y);
            alpha = ALPHA_OPAQUE;
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            red = green = blue = GetGrayValue(x, y);
            transm = FT_OPAQUE;
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            red = green = blue = GetGrayValue(x, y);
            filter = transm = FT_OPAQUE;
        }
        unsigned char GetIndexedValue(unsigned int x, unsigned int y)
        {
            CHECK_BOUNDS(x, y);
            return (unsigned char)(int(pixels[x + y * size_t(width)]) / ((TMAX + 1) >> 8));
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            if(bit == true)
                SetGrayValue(x, y, TMAX);
            else
                SetGrayValue(x, y, (unsigned int)0);
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = IntEncode(gamma, gray, TMAX);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = T(gray);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = IntEncode(gamma, gray, TMAX);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width)] = T(gray);
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            SetGrayValue(x, y, RGB2Gray(red, green, blue));
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            SetGrayValue(x, y, RGB2Gray(gammaLUT[red], gammaLUT[green], gammaLUT[blue]));
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            SetRGBValue(x, y, col.red(), col.green(), col.blue());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float, float)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            SetRGBValue(x, y, col.red(), col.green(), col.blue());
        }

        void FillBitValue(bool bit)
        {
            if(bit == true)
                FillGrayValue(TMAX);
            else
                FillGrayValue((unsigned int)0);
        }
        void FillGrayValue(float gray)
        {
            FillGrayValue(IntEncode(gamma, gray, TMAX));
        }
        void FillGrayValue(unsigned int gray)
        {
            fill(pixels.begin(), pixels.end(), T(gray));
        }
        void FillGrayAValue(float gray, float)
        {
            FillGrayValue(gray);
        }
        void FillGrayAValue(unsigned int gray, unsigned int)
        {
            FillGrayValue(gray);
        }
        void FillRGBValue(float red, float green, float blue)
        {
            FillGrayValue(RGB2Gray(red, green, blue));
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            FillGrayValue(RGB2Gray(gammaLUT[red], gammaLUT[green], gammaLUT[blue]));
        }
        void FillRGBAValue(float red, float green, float blue, float)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBTValue(float red, float green, float blue, float)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBFTValue(float red, float green, float blue, float, float)
        {
            FillRGBValue(red, green, blue);
        }
    private:
        vector<T, Allocator> pixels;
        GammaCurvePtr gamma;
        const float* gammaLUT;
};

typedef NonlinearGrayImage<unsigned char, 255, Image::Gray_Gamma8> MemoryNonlinearGray8Image;

typedef NonlinearGrayImage<unsigned short, 65535, Image::Gray_Gamma16> MemoryNonlinearGray16Image;

template<typename T, unsigned int TMAX, int IDT, class Allocator = allocator<T> >
class NonlinearGrayAImage : public Image
{
    public:
        NonlinearGrayAImage(unsigned int w, unsigned int h) :
            Image(w, h, ImageDataType(IDT)), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 2u)); FillBitValue(false); }
        NonlinearGrayAImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 2u)); FillBitValue(false); }
        NonlinearGrayAImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 2u)); FillBitValue(false); }
        NonlinearGrayAImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 2u)); FillBitValue(false); }
        ~NonlinearGrayAImage() { }

        bool IsOpaque() const
        {
            for(typename vector<T, Allocator>::const_iterator i(pixels.begin()); i != pixels.end(); i += 2)
            {
                if(i[1] < TMAX)
                    return false;
            }

            return true;
        }
        bool IsGrayscale() const
        {
            return true;
        }
        bool IsColour() const
        {
            return false;
        }
        bool IsFloat() const
        {
            return false;
        }
        bool IsInt() const
        {
            return true;
        }
        bool IsIndexed() const
        {
            return false;
        }
        bool IsGammaEncoded() const
        {
            return true;
        }
        bool HasAlphaChannel() const
        {
            return true;
        }
        bool HasFilterTransmit() const
        {
            return false;
        }
        unsigned int GetMaxIntValue() const
        {
            return TMAX;
        }
        bool TryDeferDecoding(GammaCurvePtr& g, unsigned int max)
        {
            if (max != TMAX) return false;
            if (!GammaCurve::IsNeutral(gamma)) return !g;
            gamma.swap(g);
            gammaLUT = gamma->GetLookupTable(TMAX);
            return true;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            CHECK_BOUNDS(x, y);
            return (pixels[(x + y * size_t(width)) * 2] != 0);
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            CHECK_BOUNDS(x, y);
            return gammaLUT[pixels[(x + y * size_t(width)) * 2]];
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            CHECK_BOUNDS(x, y);
            gray  = gammaLUT[pixels[(x + y * size_t(width)) * 2]];
            alpha =          pixels[(x + y * size_t(width)) * 2 + 1] / float(TMAX);
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            red = green = blue = GetGrayValue(x, y);
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            GetGrayAValue(x, y, red, alpha);
            green = blue = red;
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            float alpha;
            GetGrayAValue(x, y, red, alpha);
            green = blue = red;
            transm = 1.0 - alpha;
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            float alpha;
            GetGrayAValue(x, y, red, alpha);
            green = blue = red;
            RGBFTColour::AtoFT(alpha, filter, transm);
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            if(bit == true)
                SetGrayAValue(x, y, TMAX, ALPHA_OPAQUE_INT(TMAX));
            else
                SetGrayAValue(x, y, (unsigned int)0, ALPHA_OPAQUE_INT(TMAX));
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            SetGrayAValue(x, y, gray, ALPHA_OPAQUE);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            SetGrayAValue(x, y, gray, ALPHA_OPAQUE_INT(TMAX));
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 2]     = IntEncode(gamma, gray, TMAX);
            pixels[(x + y * size_t(width)) * 2 + 1] = T(alpha * float(TMAX));
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 2]     = gray;
            pixels[(x + y * size_t(width)) * 2 + 1] = alpha;
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            SetGrayValue(x, y, RGB2Gray(red, green, blue));
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            // not really pretty here, but we're doing color math, so we need to decode and re-encode
            SetGrayValue(x, y, RGB2Gray(gammaLUT[red], gammaLUT[green], gammaLUT[blue]));
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float alpha)
        {
            SetGrayAValue(x, y, RGB2Gray(red, green, blue), alpha);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            // not really pretty here, but we're doing color math, so we need to decode and re-encode
            // TODO FIXME - this unnecessarily converts alpha from int to float, requiring it to be converted back to int
            SetGrayAValue(x, y, RGB2Gray(gammaLUT[red], gammaLUT[green], gammaLUT[blue]), float(alpha) / float(TMAX));
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            SetGrayAValue(x, y, RGB2Gray(red, green, blue), 1.0 - transm);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            SetGrayAValue(x, y, RGB2Gray(col.red(), col.green(), col.blue()), col.Alpha());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float filter, float transm)
        {
            SetGrayAValue(x, y, RGB2Gray(red, green, blue), RGBFTColour::FTtoA(filter, transm));
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            SetGrayAValue(x, y, RGB2Gray(col.red(), col.green(), col.blue()), col.FTtoA());
        }

        void FillBitValue(bool bit)
        {
            if(bit == true)
                FillGrayValue(TMAX);
            else
                FillGrayValue((unsigned int)0);
        }
        void FillGrayValue(float gray)
        {
            FillGrayValue(IntEncode(gamma, gray, TMAX));
        }
        void FillGrayValue(unsigned int gray)
        {
            FillGrayAValue(gray, ALPHA_OPAQUE_INT(TMAX));
        }
        void FillGrayAValue(float gray, float alpha)
        {
            // [CLi 2009-09] this was dividing by float(TMAX) - which I presume to have been a bug.
            T g(IntEncode(gamma, gray, TMAX)), a(IntEncode(alpha, TMAX));
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = g;
                i++;
                *i = a;
            }
        }
        void FillGrayAValue(unsigned int gray, unsigned int alpha)
        {
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = T(gray);
                i++;
                *i = T(alpha);
            }
        }
        void FillRGBValue(float red, float green, float blue)
        {
            FillGrayValue(RGB2Gray(red, green, blue));
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            // not really pretty here, but we're doing color math, so we need to decode and re-encode
            FillGrayValue(RGB2Gray(gammaLUT[red], gammaLUT[green], gammaLUT[blue]));
        }
        void FillRGBAValue(float red, float green, float blue, float alpha)
        {
            FillGrayAValue(RGB2Gray(red, green, blue), alpha);
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            // not really pretty here, but we're doing color math, so we need to decode and re-encode
            // TODO FIXME - this unnecessarily converts alpha from int to float, requiring it to be converted back to int
            FillGrayAValue(RGB2Gray(gammaLUT[red], gammaLUT[green], gammaLUT[blue]), float(alpha) / float(TMAX));
        }
        void FillRGBTValue(float red, float green, float blue, float transm)
        {
            FillGrayAValue(RGB2Gray(red, green, blue), 1.0 - transm);
        }
        void FillRGBFTValue(float red, float green, float blue, float filter, float transm)
        {
            FillGrayAValue(RGB2Gray(red, green, blue), RGBFTColour::FTtoA(filter, transm));
        }
    private:
        vector<T, Allocator> pixels;
        GammaCurvePtr gamma;
        const float* gammaLUT;
};

typedef NonlinearGrayAImage<unsigned char, 255, Image::GrayA_Gamma8> MemoryNonlinearGrayA8Image;

typedef NonlinearGrayAImage<unsigned short, 65535, Image::GrayA_Gamma16> MemoryNonlinearGrayA16Image;

template<typename T, unsigned int TMAX, int IDT, class Allocator = allocator<T> >
class NonlinearRGBImage : public Image
{
    public:
        NonlinearRGBImage(unsigned int w, unsigned int h) :
            Image(w, h, ImageDataType(IDT)), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 3u)); FillBitValue(false); }
        NonlinearRGBImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 3u)); FillBitValue(false); }
        NonlinearRGBImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 3u)); FillBitValue(false); }
        NonlinearRGBImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 3u)); FillBitValue(false); }
        ~NonlinearRGBImage() { }

        bool IsOpaque() const
        {
            return true;
        }
        bool IsGrayscale() const
        {
            return false;
        }
        bool IsColour() const
        {
            return true;
        }
        bool IsFloat() const
        {
            return false;
        }
        bool IsInt() const
        {
            return true;
        }
        bool IsIndexed() const
        {
            return false;
        }
        bool IsGammaEncoded() const
        {
            return true;
        }
        bool HasAlphaChannel() const
        {
            return false;
        }
        bool HasFilterTransmit() const
        {
            return false;
        }
        unsigned int GetMaxIntValue() const
        {
            return TMAX;
        }
        bool TryDeferDecoding(GammaCurvePtr& g, unsigned int max)
        {
            if (max != TMAX) return false;
            if (!GammaCurve::IsNeutral(gamma)) return !g;
            gamma.swap(g);
            gammaLUT = gamma->GetLookupTable(TMAX);
            return true;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            float red, green, blue;
            GetRGBValue(x, y, red, green, blue);
            return IS_NONZERO_RGB(red, green, blue);
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            float red, green, blue;
            GetRGBValue(x, y, red, green, blue);
            return RGB2Gray(red, green, blue);
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            gray = GetGrayValue(x, y);
            alpha = ALPHA_OPAQUE;
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            CHECK_BOUNDS(x, y);
            red   = gammaLUT[pixels[(x + y * size_t(width)) * 3]];
            green = gammaLUT[pixels[(x + y * size_t(width)) * 3 + 1]];
            blue  = gammaLUT[pixels[(x + y * size_t(width)) * 3 + 2]];
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            GetRGBValue(x, y, red, green, blue);
            alpha = ALPHA_OPAQUE;
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            GetRGBValue(x, y, red, green, blue);
            transm = FT_OPAQUE;
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            GetRGBValue(x, y, red, green, blue);
            filter = transm = FT_OPAQUE;
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            if(bit == true)
                SetGrayValue(x, y, TMAX);
            else
                SetGrayValue(x, y, (unsigned int)0);
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            SetRGBValue(x, y, gray, gray, gray);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width) * 3]     =
            pixels[x + y * size_t(width) * 3 + 1] =
            pixels[x + y * size_t(width) * 3 + 2] = gray;
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width) * 3]     =
            pixels[x + y * size_t(width) * 3 + 1] =
            pixels[x + y * size_t(width) * 3 + 2] = IntEncode(gamma, gray, TMAX);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int)
        {
            CHECK_BOUNDS(x, y);
            pixels[x + y * size_t(width) * 3]     =
            pixels[x + y * size_t(width) * 3 + 1] =
            pixels[x + y * size_t(width) * 3 + 2] = gray;
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 3]     = IntEncode(gamma, red,   TMAX);
            pixels[(x + y * size_t(width)) * 3 + 1] = IntEncode(gamma, green, TMAX);
            pixels[(x + y * size_t(width)) * 3 + 2] = IntEncode(gamma, blue,  TMAX);
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 3]     = T(red);
            pixels[(x + y * size_t(width)) * 3 + 1] = T(green);
            pixels[(x + y * size_t(width)) * 3 + 2] = T(blue);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            SetRGBValue(x, y, col.red(), col.green(), col.blue());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float, float)
        {
            SetRGBValue(x, y, red, green, blue);
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            SetRGBValue(x, y, col.red(), col.green(), col.blue());
        }

        void FillBitValue(bool bit)
        {
            if(bit == true)
                FillGrayValue(TMAX);
            else
                FillGrayValue((unsigned int)0);
        }
        void FillGrayValue(float gray)
        {
            FillGrayValue((unsigned int)(IntEncode(gamma, gray, TMAX)));
        }
        void FillGrayValue(unsigned int gray)
        {
            fill(pixels.begin(), pixels.end(), gray);
        }
        void FillGrayAValue(float gray, float)
        {
            FillRGBValue(gray, gray, gray);
        }
        void FillGrayAValue(unsigned int gray, unsigned int)
        {
            FillRGBValue(gray, gray, gray);
        }
        void FillRGBValue(float red, float green, float blue)
        {
            // [CLi 2009-09] this was dividing by float(TMAX) - which I presume to have been a bug.
            T r(IntEncode(gamma, red, TMAX)), g(IntEncode(gamma, green, TMAX)), b(IntEncode(gamma, blue, TMAX));
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = r;
                i++;
                *i = g;
                i++;
                *i = b;
            }
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = T(red);
                i++;
                *i = T(green);
                i++;
                *i = T(blue);
            }
        }
        void FillRGBAValue(float red, float green, float blue, float)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBTValue(float red, float green, float blue, float)
        {
            FillRGBValue(red, green, blue);
        }
        void FillRGBFTValue(float red, float green, float blue, float, float)
        {
            FillRGBValue(red, green, blue);
        }
    private:
        vector<T, Allocator> pixels;
        GammaCurvePtr gamma;
        const float* gammaLUT;
};

typedef NonlinearRGBImage<unsigned char, 255, Image::RGB_Gamma8> MemoryNonlinearRGB8Image;

typedef NonlinearRGBImage<unsigned short, 65535, Image::RGB_Gamma16> MemoryNonlinearRGB16Image;

template<typename T, unsigned int TMAX, int IDT, class Allocator = allocator<T> >
class NonlinearRGBAImage : public Image
{
    public:
        NonlinearRGBAImage(unsigned int w, unsigned int h) :
            Image(w, h, ImageDataType(IDT)), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 4u)); FillBitValue(false); }
        NonlinearRGBAImage(unsigned int w, unsigned int h, const vector<RGBMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 4u)); FillBitValue(false); }
        NonlinearRGBAImage(unsigned int w, unsigned int h, const vector<RGBAMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 4u)); FillBitValue(false); }
        NonlinearRGBAImage(unsigned int w, unsigned int h, const vector<RGBFTMapEntry>& m) :
            Image(w, h, ImageDataType(IDT), m), gamma(NeutralGammaCurve::Get()) { gammaLUT = gamma->GetLookupTable(TMAX); pixels.resize(SafeUnsignedProduct<size_t>(w, h, 4u)); FillBitValue(false); }
        ~NonlinearRGBAImage() { }

        bool IsOpaque() const
        {
            for(typename vector<T, Allocator>::const_iterator i(pixels.begin()); i != pixels.end(); i += 4)
            {
                if(i[3] < TMAX)
                    return false;
            }

            return true;
        }
        bool IsGrayscale() const
        {
            return false;
        }
        bool IsColour() const
        {
            return true;
        }
        bool IsFloat() const
        {
            return false;
        }
        bool IsInt() const
        {
            return true;
        }
        bool IsIndexed() const
        {
            return false;
        }
        bool IsGammaEncoded() const
        {
            return true;
        }
        bool HasAlphaChannel() const
        {
            return true;
        }
        bool HasFilterTransmit() const
        {
            return false;
        }
        unsigned int GetMaxIntValue() const
        {
            return TMAX;
        }
        bool TryDeferDecoding(GammaCurvePtr& g, unsigned int max)
        {
            if (max != TMAX) return false;
            if (!GammaCurve::IsNeutral(gamma)) return !g;
            gamma.swap(g);
            gammaLUT = gamma->GetLookupTable(TMAX);
            return true;
        }

        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            float red, green, blue, alpha;
            GetRGBAValue(x, y, red, green, blue, alpha);
            return IS_NONZERO_RGB(red, green, blue);
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            float red, green, blue, alpha;
            GetRGBAValue(x, y, red, green, blue, alpha);
            return RGB2Gray(red, green, blue);
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            float red, green, blue;
            GetRGBAValue(x, y, red, green, blue, alpha);
            gray = RGB2Gray(red, green, blue);
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            float alpha;
            GetRGBAValue(x, y, red, green, blue, alpha);
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            CHECK_BOUNDS(x, y);
            red   = gammaLUT[pixels[(x + y * size_t(width)) * 4]];
            green = gammaLUT[pixels[(x + y * size_t(width)) * 4 + 1]];
            blue  = gammaLUT[pixels[(x + y * size_t(width)) * 4 + 2]];
            alpha =    float(pixels[(x + y * size_t(width)) * 4 + 3]) / float(TMAX);
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            float alpha;
            GetRGBAValue(x, y, red, green, blue, alpha);
            transm = 1.0 - alpha;
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            float alpha;
            GetRGBAValue(x, y, red, green, blue, alpha);
            RGBFTColour::AtoFT(alpha, filter, transm);
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            if(bit == true)
                SetGrayValue(x, y, TMAX);
            else
                SetGrayValue(x, y, (unsigned int)0);
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            SetRGBAValue(x, y, gray, gray, gray, ALPHA_OPAQUE);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            SetRGBAValue(x, y, gray, gray, gray, ALPHA_OPAQUE_INT(TMAX));
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float alpha)
        {
            SetRGBAValue(x, y, gray, gray, gray, alpha);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int alpha)
        {
            SetRGBAValue(x, y, gray, gray, gray, alpha);
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            SetRGBAValue(x, y, red, green, blue, ALPHA_OPAQUE);
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            SetRGBAValue(x, y, red, green, blue, ALPHA_OPAQUE_INT(TMAX));
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 4]     = T(IntEncode(gamma, red,   TMAX));
            pixels[(x + y * size_t(width)) * 4 + 1] = T(IntEncode(gamma, green, TMAX));
            pixels[(x + y * size_t(width)) * 4 + 2] = T(IntEncode(gamma, blue,  TMAX));
            pixels[(x + y * size_t(width)) * 4 + 3] = T(IntEncode(       alpha, TMAX));
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 4]     = T(red);
            pixels[(x + y * size_t(width)) * 4 + 1] = T(green);
            pixels[(x + y * size_t(width)) * 4 + 2] = T(blue);
            pixels[(x + y * size_t(width)) * 4 + 3] = T(alpha);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            SetRGBAValue(x, y, red, green, blue, 1.0 - transm);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 4]     = T(col.red());
            pixels[(x + y * size_t(width)) * 4 + 1] = T(col.green());
            pixels[(x + y * size_t(width)) * 4 + 2] = T(col.blue());
            pixels[(x + y * size_t(width)) * 4 + 3] = T(col.Alpha());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float filter, float transm)
        {
            SetRGBAValue(x, y, red, green, blue, RGBFTColour::FTtoA(filter, transm));
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            CHECK_BOUNDS(x, y);
            pixels[(x + y * size_t(width)) * 4]     = T(col.red());
            pixels[(x + y * size_t(width)) * 4 + 1] = T(col.green());
            pixels[(x + y * size_t(width)) * 4 + 2] = T(col.blue());
            pixels[(x + y * size_t(width)) * 4 + 3] = T(col.FTtoA());
        }

        void FillBitValue(bool bit)
        {
            if(bit == true)
                FillGrayValue(TMAX);
            else
                FillGrayValue((unsigned int)0);
        }
        void FillGrayValue(float gray)
        {
            FillRGBAValue(gray, gray, gray, ALPHA_OPAQUE);
        }
        void FillGrayValue(unsigned int gray)
        {
            FillRGBAValue(gray, gray, gray, ALPHA_OPAQUE_INT(TMAX));
        }
        void FillGrayAValue(float gray, float alpha)
        {
            FillRGBAValue(gray, gray, gray, alpha);
        }
        void FillGrayAValue(unsigned int gray, unsigned int alpha)
        {
            FillRGBAValue(gray, gray, gray, alpha);
        }
        void FillRGBValue(float red, float green, float blue)
        {
            FillRGBAValue(red, green, blue, ALPHA_OPAQUE);
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            FillRGBAValue(red, green, blue, ALPHA_OPAQUE_INT(TMAX));
        }
        void FillRGBAValue(float red, float green, float blue, float alpha)
        {
            // [CLi 2009-09] this was dividing by float(TMAX) - which I presume to have been a bug.
            T r(IntEncode(gamma, red, TMAX)), g(IntEncode(gamma, green, TMAX)), b(IntEncode(gamma, blue, TMAX)), a(IntEncode(alpha, TMAX));
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = r;
                i++;
                *i = g;
                i++;
                *i = b;
                i++;
                *i = a;
            }
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            for(typename vector<T, Allocator>::iterator i(pixels.begin()); i != pixels.end(); i++)
            {
                *i = T(red);
                i++;
                *i = T(green);
                i++;
                *i = T(blue);
                i++;
                *i = T(alpha);
            }
        }
        void FillRGBTValue(float red, float green, float blue, float transm)
        {
            FillRGBAValue(red, green, blue, 1.0 - transm);
        }
        void FillRGBFTValue(float red, float green, float blue, float filter, float transm)
        {
            FillRGBAValue(red, green, blue, RGBFTColour::FTtoA(filter, transm));
        }
    private:
        vector<T, Allocator> pixels;
        GammaCurvePtr gamma;
        const float* gammaLUT;
};

typedef NonlinearRGBAImage<unsigned char, 255, Image::RGBA_Gamma8> MemoryNonlinearRGBA8Image;

typedef NonlinearRGBAImage<unsigned short, 65535, Image::RGBA_Gamma16> MemoryNonlinearRGBA16Image;

// sample basic file-based pixel container. not very efficient.
// it is expected that for performance reasons, platforms will provide their own specific
// implementation of this; hence, this should be considered only a fall-back default.

// [JG] That code is hostile to the system, as the file seek are just passed directly.
// (and there is a lot, out of sequence!)
// On the render (1st write), the cache will always miss, but as post-process might access more than once to
// this container, it is mandatory to still read the file every time.
// Measurement about the small read cache (compared to the alternative) show no significant delta in performance
// to read the contained data into a PNG.
// (it's a linear read, the system is able to anticipate it, so we are fine!)
class FileBackedPixelContainer
{
    public:
        typedef long size_type;
        enum
        {
            RED    = 0,
            GREEN  = 1,
            BLUE   = 2,
            FILTER = 3,
            TRANSM = 4
        };
        class pixel_type
        {
            public:
                pixel_type()
                {
                    elements[RED] = 0.0;
                    elements[GREEN] = 0.0;
                    elements[BLUE] = 0.0;
                    elements[FILTER] = 0.0;
                    elements[TRANSM] = 0.0;
                }
                pixel_type(const ColourChannel *vals)
                {
                    memcpy(elements, vals, sizeof(elements));
                }
                pixel_type(const RGBFTColour& vals)
                {
                    elements[RED]    = vals.red();
                    elements[GREEN]  = vals.green();
                    elements[BLUE]   = vals.blue();
                    elements[FILTER] = vals.filter();
                    elements[TRANSM] = vals.transm();
                }
                pixel_type(const RGBTColour& vals)
                {
                    elements[RED]    = vals.red();
                    elements[GREEN]  = vals.green();
                    elements[BLUE]   = vals.blue();
                    elements[FILTER] = 0.0;
                    elements[TRANSM] = vals.transm();
                }
                pixel_type(float r, float g, float b, float f, float t)
                {
                    elements[RED] = r;
                    elements[GREEN] = g;
                    elements[BLUE] = b;
                    elements[FILTER] = f;
                    elements[TRANSM] = t;
                }
                ~pixel_type() {}
                operator ColourChannel *() { return elements; }
                operator const ColourChannel *() const { return elements; }
                bool operator==(float val) const { return elements[RED] == val && elements[GREEN] == val && elements[BLUE] == val && elements[FILTER] == val && elements[TRANSM] == val; }
                bool operator!=(float val) const { return !(*this == val); }

            protected:
                ColourChannel elements[5];
        };

        FileBackedPixelContainer(size_type width, size_type height, size_type bs):
            m_File(-1), m_Width(width), m_Height(height), m_xPos(0), m_yPos(0), m_Dirty(false), m_Path(POV_PLATFORM_BASE.CreateTemporaryFile())
        {
            if ((m_File = open(UCS2toASCIIString(m_Path).c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
                throw POV_EXCEPTION(kCannotOpenFileErr, "Cannot open backing file for intermediate image storage.");
            m_Blocksize = bs;
            m_Buffer.resize(m_Blocksize);
            // write extra data to create the big file and help 3rd party reader
            POV_LONG pos;
            // NB: The following use of SafeUnsignedProduct also safeguards later coputations of
            // pixel positions within the file, as long as x and y coordinates are sane
            pos = SafeUnsignedProduct<POV_LONG>(m_Width, m_Height);
            if ( pos% m_Blocksize)
            { /* issue: the block would overlap the end of file */
                pos /= m_Blocksize;
                pos++;
                pos = SafeUnsignedProduct<POV_LONG>(pos, m_Blocksize);
            }
            /* else fine case: the boundary of block match the boundary of pixels in file */
            pos = SafeUnsignedProduct<POV_LONG>(pos, sizeof(pixel_type));
            size_type meta[3];
            meta[0] = sizeof(pixel_type);
            meta[1] = m_Width;
            meta[2] = m_Height;
            if (lseek64(m_File, pos, SEEK_SET) != pos)
                throw POV_EXCEPTION(kFileDataErr, "Intermediate image storage backing file write/seek failed at creation.");
            if (write(m_File, &meta[0], (int) sizeof(size_type)*3) != (sizeof(size_type)*3))
                throw POV_EXCEPTION(kFileDataErr, "Intermediate image storage backing file write failed at creation.");
            // m_Committed.resize(width * height / m_Blocksize);
        }

        virtual ~FileBackedPixelContainer()
        {
            if (m_File != -1)
            {
                Flush();
                close(m_File);
            }
            if (m_Path.empty() == false)
            {
                // if shutdown has been delayed, by the time we reach here, the platform base
                // may no longer be valid (see crashdump #77 for an example of this). we need
                // to take the address of the reference to see if it's now NULL before we use it.
                PlatformBase *pb(&POV_PLATFORM_BASE);
                if (pb != NULL)
                    pb->DeleteTemporaryFile(m_Path);
            }
        }

        void Flush(void)
        {
            WriteCurrentBlock();
        }

        void SetPixel(size_type x, size_type y, const pixel_type& pixel)
        {
            WritePixel(x, y, pixel);
            NextPixel();
        }

        void SetPixel(size_type x, size_type y, float red, float green, float blue, float filter, float transm)
        {
            pixel_type pixel;

            pixel[RED] = red;
            pixel[GREEN] = green;
            pixel[BLUE] = blue;
            pixel[FILTER] = filter;
            pixel[TRANSM] = transm;
            WritePixel(x, y, pixel);
        }

        void SetPixel(float red, float green, float blue, float filter, float transm)
        {
            SetPixel(m_xPos, m_yPos, red, green, blue, filter, transm);
            NextPixel();
        }

        void GetPixel(pixel_type& pixel)
        {
            ReadPixel(m_xPos, m_yPos, pixel);
            NextPixel();
        }

        void GetPixel(float& red, float& green, float& blue, float& filter, float& transm)
        {
            pixel_type pixel;

            GetPixel(pixel);    // advances NextPixel
            red = pixel[RED];
            green = pixel[GREEN];
            blue = pixel[BLUE];
            filter = pixel[FILTER];
            transm = pixel[TRANSM];
        }

        void GetPixel(size_type x, size_type y, pixel_type& pixel)
        {
            SetPos(x, y);
            ReadPixel(x, y, pixel);
        }

        void GetPixel(size_type x, size_type y, float& red, float& green, float& blue, float& filter, float& transm)
        {
            pixel_type pixel;

            GetPixel(x, y, pixel);  // sets Position
            red = pixel[RED];
            green = pixel[GREEN];
            blue = pixel[BLUE];
            filter = pixel[FILTER];
            transm = pixel[TRANSM];
        }

        /* void ClearCache(const pixel_type& pixel = pixel_type())
        {
            for (int i = 0; i < m_Width; i++)
                memcpy(&m_Buffer[i], &pixel, sizeof(pixel));
        } */

        void FillLine(size_type y, const pixel_type& pixel)
        {
            // bool notBlank(pixel != 0.0);

            for (size_type x = 0; x < m_Width; x++)
                // if (notBlank)
                    WritePixel(x, y, pixel);
        }

        void Fill(const pixel_type& pixel)
        {
            for (size_type y = 0; y < m_Height; y++)
                FillLine(y, pixel);
        }

        void Fill(float red, float green, float blue, float filter, float transm)
        {
            pixel_type pixel(red, green, blue, filter, transm);
            Fill(pixel);
        }

    protected:
        int                 m_File;
        bool                m_Dirty;
        size_type           m_Blocksize;
        POV_LONG            m_CurrentBlock;
        size_type           m_Width;
        size_type           m_Height;
        size_type           m_xPos;
        size_type           m_yPos;
        UCS2String          m_Path;
        //vector<bool>        m_Committed;
        vector<pixel_type>  m_Buffer;

        void SetPos(size_type x, size_type y, bool cache = true)
        {
            if (x < 0 || x >= m_Width || y < 0 || y >= m_Height)
                throw POV_EXCEPTION(kFileDataErr, "Invalid coordinates in intermediate image file seek.");
            if (y == m_yPos)
            {
                m_xPos = x;
                return;
            }
            m_xPos = x;
            m_yPos = y;
        }

        void NextPixel(void)
        {
            if (m_xPos == m_Width - 1)
            {
                if (m_yPos == m_Height - 1)
                    return;
            }
            else
                m_xPos++;
        }

        void ReadPixel(size_type x, size_type y, pixel_type& pixel)
        {
            POV_LONG pos, block = (y * (POV_LONG)(m_Width) + x) / m_Blocksize;

            if (block != m_CurrentBlock) {
                WriteCurrentBlock();
#if 0
                if (m_Committed[block] == false) {
                    ColourChannel pixel[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
                    for (size_type i = 0; i < m_Blocksize; i++)
                        memcpy(&m_Buffer[i], pixel, sizeof(pixel));
                    m_CurrentBlock = block;
                    return;
                }
#endif
                pos = block * sizeof(pixel_type) * m_Blocksize;
                int chunk = sizeof(pixel_type) * m_Blocksize;
                if (lseek64(m_File, pos, SEEK_SET) != pos)
                    throw POV_EXCEPTION(kFileDataErr, "Intermediate image storage backing file read/seek failed.");
                int bytes = read(m_File, &m_Buffer[0], chunk);
                if (bytes != (sizeof(pixel_type) * m_Blocksize))
                    throw POV_EXCEPTION(kFileDataErr, "Intermediate image storage backing file read failed.");
                m_CurrentBlock = block;
            }
            memcpy(&pixel, m_Buffer[(y * (POV_LONG)(m_Width) + x) % m_Blocksize], sizeof(pixel));
        }
#if 0
        bool BlockCommitted(size_type x, size_type y)
        {
            POV_LONG block = (y * POV_LONG(m_Width) + x) / m_Blocksize;

            return(m_Committed[block]);
        }
#endif
        void WriteCurrentBlock()
        {
            POV_LONG pos;

            if (m_Dirty) {
                pos = m_CurrentBlock * sizeof(pixel_type) * m_Blocksize;
                if (lseek64(m_File, pos, SEEK_SET) != pos)
                    throw POV_EXCEPTION(kFileDataErr, "Intermediate image storage backing file write/seek failed.");
                if (write(m_File, &m_Buffer[0], (int) sizeof(pixel_type) * m_Blocksize) != (sizeof(pixel_type) * m_Blocksize))
                    throw POV_EXCEPTION(kFileDataErr, "Intermediate image storage backing file write failed.");
            //  m_Committed[m_CurrentBlock] = true;
                m_Dirty = false;
            }
        }

        void WritePixel(size_type x, size_type y, const pixel_type& pixel)
        {
            pixel_type dummy;

            ReadPixel(x, y, dummy);
            memcpy(m_Buffer[(y * (POV_LONG)(m_Width) + x) % m_Blocksize], &pixel, sizeof(pixel));
            m_Dirty = true;
        }

    private:
        // not available
        FileBackedPixelContainer(void) {}
};

class FileRGBFTImage : public Image
{
    public:
        typedef FileBackedPixelContainer::pixel_type pixel_type;

        FileRGBFTImage(unsigned int w, unsigned int h, unsigned int bs): Image(w, h, RGBFT_Float), pixels(width, height, bs) { }
        ~FileRGBFTImage() { }

        bool IsGrayscale() const { return false; }
        bool IsColour() const { return true; }
        bool IsFloat() const { return true; }
        bool IsInt() const { return false; }
        bool IsIndexed() const { return false; }
        bool IsGammaEncoded() const { return false; }
        bool HasAlphaChannel() const { return false; }
        bool HasFilterTransmit() const { return true; }
        unsigned int GetMaxIntValue() const { return 255; }
        void SetEncodingGamma(GammaCurvePtr gamma) { ; }
        bool TryDeferDecoding(GammaCurvePtr&, unsigned int) { return false; }
        bool IsOpaque() const { throw POV_EXCEPTION(kUncategorizedError, "Internal error: IsOpaque() not supported in FileRGBFTImage"); }
        bool GetBitValue(unsigned int x, unsigned int y) const
        {
            // TODO FIXME - [CLi] This ignores opacity information; other bit-based code doesn't.
            float red, green, blue, filter, transm;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
            return IS_NONZERO_RGB(red, green, blue);
        }
        float GetGrayValue(unsigned int x, unsigned int y) const
        {
            float red, green, blue, filter, transm;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
            return RGB2Gray(red, green, blue);
        }
        void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const
        {
            float red, green, blue, filter, transm;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
            gray = RGB2Gray(red, green, blue);
            alpha = RGBFTColour::FTtoA(filter, transm);
        }
        void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const
        {
            float filter, transm;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
        }
        void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const
        {
            float filter, transm;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
            alpha = RGBFTColour::FTtoA(filter, transm);
        }
        void GetRGBTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& transm) const
        {
            float filter;
            GetRGBFTValue(x, y, red, green, blue, filter, transm);
            transm = 1.0 - RGBFTColour::FTtoA(filter, transm);
        }
        void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const
        {
            CHECK_BOUNDS(x, y);
            pixels.GetPixel(x, y, red, green, blue, filter, transm);
        }

        void SetBitValue(unsigned int x, unsigned int y, bool bit)
        {
            if(bit == true)
                SetGrayValue(x, y, 1.0f);
            else
                SetGrayValue(x, y, 0.0f);
        }
        void SetGrayValue(unsigned int x, unsigned int y, float gray)
        {
            SetRGBFTValue(x, y, gray, gray, gray, FT_OPAQUE, FT_OPAQUE);
        }
        void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray)
        {
            SetGrayValue(x, y, float(gray) / 255.0f);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, float gray, float alpha)
        {
            // TODO - should alpha be converted to filter and transm? [trf]
            float filter, transm;
            RGBFTColour::AtoFT(alpha, filter, transm);
            SetRGBFTValue(x, y, gray, gray, gray, filter, transm);
        }
        void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int alpha)
        {
            // TODO - should alpha be converted to filter and transm? [trf]
            float c = float(gray) / 255.0f;
            float filter, transm;
            RGBFTColour::AtoFT(float(alpha) / 255.0f, filter, transm);
            SetRGBFTValue(x, y, c, c, c, filter, transm);
        }
        void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue)
        {
            SetRGBFTValue(x, y, red, green, blue, FT_OPAQUE, FT_OPAQUE);
        }
        void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue)
        {
            SetRGBFTValue(x, y, float(red) / 255.0f, float(green) / 255.0f, float(blue) / 255.0f, FT_OPAQUE, FT_OPAQUE);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float alpha)
        {
            // TODO - should alpha be converted to filter and transm? [trf]
            float filter, transm;
            RGBFTColour::AtoFT(alpha, filter, transm);
            SetRGBFTValue(x, y, red, green, blue, filter, transm);
        }
        void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            // TODO - should alpha be converted to filter and transm? [trf]
            float filter, transm;
            RGBFTColour::AtoFT(float(alpha) / 255.0f, filter, transm);
            SetRGBFTValue(x, y, float(red) / 255.0f, float(green) / 255.0f, float(blue) / 255.0f, filter, transm);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, float red, float green, float blue, float transm)
        {
            SetRGBFTValue(x, y, red, green, blue, FT_OPAQUE, transm);
        }
        void SetRGBTValue(unsigned int x, unsigned int y, const RGBTColour& col)
        {
            SetRGBFTValue(x, y, col.red(), col.green(), col.blue(), FT_OPAQUE, col.transm());
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float filter, float transm)
        {
            CHECK_BOUNDS(x, y);
            pixels.SetPixel(x, y, red, green, blue, filter, transm);
        }
        void SetRGBFTValue(unsigned int x, unsigned int y, const RGBFTColour& col)
        {
            CHECK_BOUNDS(x, y);
            pixels.SetPixel(x, y, col);
        }

        void FillBitValue(bool bit)
        {
            if(bit == true)
                FillGrayValue(1.0f);
            else
                FillGrayValue(0.0f);
        }
        void FillGrayValue(float gray)
        {
            FillRGBFTValue(gray, gray, gray, FT_OPAQUE, FT_OPAQUE);
        }
        void FillGrayValue(unsigned int gray)
        {
            FillGrayValue(float(gray) / 255.0f);
        }
        void FillGrayAValue(float gray, float alpha)
        {
            // TODO - should alpha be converted to filter and transm? [trf]
            float filter, transm;
            RGBFTColour::AtoFT(alpha, filter, transm);
            FillRGBFTValue(gray, gray, gray, filter, transm);
        }
        void FillGrayAValue(unsigned int gray, unsigned int alpha)
        {
            // TODO - should alpha be converted to filter and transm? [trf]
            FillGrayAValue(float(gray) / 255.0f, float(alpha) / 255.0f);
        }
        void FillRGBValue(float red, float green, float blue)
        {
            FillRGBFTValue(red, green, blue, FT_OPAQUE, FT_OPAQUE);
        }
        void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue)
        {
            FillRGBFTValue(float(red) / 255.0f, float(green) / 255.0f, float(blue) / 255.0f, FT_OPAQUE, FT_OPAQUE);
        }
        void FillRGBAValue(float red, float green, float blue, float alpha)
        {
            // TODO - should alpha be converted to filter and transm? [trf]
            float filter, transm;
            RGBFTColour::AtoFT(alpha, filter, transm);
            FillRGBFTValue(red, green, blue, filter, transm);
        }
        void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha)
        {
            // TODO - should alpha be converted to filter and transm? [trf]
            float filter, transm;
            RGBFTColour::AtoFT(float(alpha) / 255.0f, filter, transm);
            FillRGBFTValue(float(red) / 255.0f, float(green) / 255.0f, float(blue) / 255.0f, filter, transm);
        }
        void FillRGBTValue(float red, float green, float blue, float transm)
        {
            FillRGBFTValue(red, green, blue, FT_OPAQUE, transm);
        }
        void FillRGBFTValue(float red, float green, float blue, float filter, float transm)
        {
            pixels.Fill(red, green, blue, filter, transm);
        }

    protected:
        mutable FileBackedPixelContainer pixels;
};

void RGBMap2RGBAMap(const vector<Image::RGBMapEntry>& m, vector<Image::RGBAMapEntry>& n);
void RGBMap2RGBFTMap(const vector<Image::RGBMapEntry>& m, vector<Image::RGBFTMapEntry>& n);
void RGBAMap2RGBMap(const vector<Image::RGBAMapEntry>& m, vector<Image::RGBMapEntry>& n);
void RGBAMap2RGBFTMap(const vector<Image::RGBAMapEntry>& m, vector<Image::RGBFTMapEntry>& n);
void RGBFTMap2RGBMap(const vector<Image::RGBFTMapEntry>& m, vector<Image::RGBMapEntry>& n);
void RGBFTMap2RGBAMap(const vector<Image::RGBFTMapEntry>& m, vector<Image::RGBAMapEntry>& n);

void RGBMap2RGBAMap(const vector<Image::RGBMapEntry>& m, vector<Image::RGBAMapEntry>& n)
{
    n.clear();
    n.reserve(m.size());

    for(vector<Image::RGBMapEntry>::const_iterator i(m.begin()); i != m.end(); i++)
        n.push_back(Image::RGBAMapEntry(i->red, i->green, i->blue, ALPHA_OPAQUE));
}

void RGBMap2RGBFTMap(const vector<Image::RGBMapEntry>& m, vector<Image::RGBFTMapEntry>& n)
{
    n.clear();
    n.reserve(m.size());

    for(vector<Image::RGBMapEntry>::const_iterator i(m.begin()); i != m.end(); i++)
        n.push_back(Image::RGBFTMapEntry(i->red, i->green, i->blue, FT_OPAQUE, FT_OPAQUE));
}

void RGBAMap2RGBMap(const vector<Image::RGBAMapEntry>& m, vector<Image::RGBMapEntry>& n)
{
    n.clear();
    n.reserve(m.size());

    for(vector<Image::RGBAMapEntry>::const_iterator i(m.begin()); i != m.end(); i++)
        n.push_back(Image::RGBMapEntry(i->red, i->green, i->blue));
}

void RGBAMap2RGBFTMap(const vector<Image::RGBAMapEntry>& m, vector<Image::RGBFTMapEntry>& n)
{
    n.clear();
    n.reserve(m.size());

    for(vector<Image::RGBAMapEntry>::const_iterator i(m.begin()); i != m.end(); i++)
    {
        float filter, transm;
        RGBFTColour::AtoFT(i->alpha, filter, transm);
        n.push_back(Image::RGBFTMapEntry(i->red, i->green, i->blue, filter, transm));
    }
}

void RGBFTMap2RGBMap(const vector<Image::RGBFTMapEntry>& m, vector<Image::RGBMapEntry>& n)
{
    n.clear();
    n.reserve(m.size());

    for(vector<Image::RGBFTMapEntry>::const_iterator i(m.begin()); i != m.end(); i++)
        n.push_back(Image::RGBMapEntry(i->red, i->green, i->blue));
}

void RGBFTMap2RGBAMap(const vector<Image::RGBFTMapEntry>& m, vector<Image::RGBAMapEntry>& n)
{
    n.clear();
    n.reserve(m.size());

    for(vector<Image::RGBFTMapEntry>::const_iterator i(m.begin()); i != m.end(); i++)
        n.push_back(Image::RGBAMapEntry(i->red, i->green, i->blue, RGBFTColour::FTtoA(i->filter, i->transm)));
}

Image *Image::Create(unsigned int w, unsigned int h, ImageDataType t, unsigned int maxRAMmbHint, unsigned int pixelsPerBlockHint)
{
    try
    {
        switch(t)
        {
            case Bit_Map:
                return new MemoryBitMapImage(w, h);
            case Gray_Int8:
                return new MemoryGray8Image(w, h);
            case Gray_Int16:
                return new MemoryGray16Image(w, h);
            case GrayA_Int8:
                return new MemoryGrayA8Image(w, h);
            case GrayA_Int16:
                return new MemoryGrayA16Image(w, h);
            case RGB_Int8:
                return new MemoryRGB8Image(w, h);
            case RGB_Int16:
                return new MemoryRGB16Image(w, h);
            case RGBA_Int8:
                return new MemoryRGBA8Image (w, h);
            case RGBA_Int16:
                return new MemoryRGBA16Image(w, h);
            case RGBFT_Float:
                if (maxRAMmbHint > 0)
                    if (SafeUnsignedProduct<POV_ULONG>(w, h, sizeof(FileRGBFTImage::pixel_type)) / 1048576 > maxRAMmbHint)
                        return new FileRGBFTImage(w, h, pixelsPerBlockHint);
                return new MemoryRGBFTImage(w, h);
            case RGB_Gamma8:
                return new MemoryNonlinearRGB8Image(w, h);
            case RGB_Gamma16:
                return new MemoryNonlinearRGB16Image(w, h);
            case RGBA_Gamma8:
                return new MemoryNonlinearRGBA8Image (w, h);
            case RGBA_Gamma16:
                return new MemoryNonlinearRGBA16Image(w, h);
            case Gray_Gamma8:
                return new MemoryNonlinearGray8Image(w, h);
            case Gray_Gamma16:
                return new MemoryNonlinearGray16Image(w, h);
            case GrayA_Gamma8:
                return new MemoryNonlinearGrayA8Image(w, h);
            case GrayA_Gamma16:
                return new MemoryNonlinearGrayA16Image(w, h);
            default:
                throw POV_EXCEPTION_STRING("Undefined image format in Image::Create");
        }
    }
    catch(std::bad_alloc&)
    {
        throw POV_EXCEPTION(kOutOfMemoryErr, "Insufficient memory to allocate intermediate image storage.");
    }
}

Image *Image::Create(unsigned int w, unsigned int h, ImageDataType t, bool allowFileBacking)
{
    try
    {
        switch(t)
        {
            case Bit_Map:
                return new MemoryBitMapImage(w, h);
            case Gray_Int8:
                return new MemoryGray8Image(w, h);
            case Gray_Int16:
                return new MemoryGray16Image(w, h);
            case GrayA_Int8:
                return new MemoryGrayA8Image(w, h);
            case GrayA_Int16:
                return new MemoryGrayA16Image(w, h);
            case RGB_Int8:
                return new MemoryRGB8Image(w, h);
            case RGB_Int16:
                return new MemoryRGB16Image(w, h);
            case RGBA_Int8:
                return new MemoryRGBA8Image (w, h);
            case RGBA_Int16:
                return new MemoryRGBA16Image(w, h);
            case RGBFT_Float:
#ifdef FILE_MAPPED_IMAGE_ALLOCATOR
                if (allowFileBacking)
                    return new RGBFTImage<FILE_MAPPED_IMAGE_ALLOCATOR<float> >(w, h);
#endif
                return new MemoryRGBFTImage(w, h);
            case RGB_Gamma8:
                return new MemoryNonlinearRGB8Image(w, h);
            case RGB_Gamma16:
                return new MemoryNonlinearRGB16Image(w, h);
            case RGBA_Gamma8:
                return new MemoryNonlinearRGBA8Image (w, h);
            case RGBA_Gamma16:
                return new MemoryNonlinearRGBA16Image(w, h);
            case Gray_Gamma8:
                return new MemoryNonlinearGray8Image(w, h);
            case Gray_Gamma16:
                return new MemoryNonlinearGray16Image(w, h);
            case GrayA_Gamma8:
                return new MemoryNonlinearGrayA8Image(w, h);
            case GrayA_Gamma16:
                return new MemoryNonlinearGrayA16Image(w, h);
            default:
                throw POV_EXCEPTION_STRING("Undefined image format in Image::Create");
        }
    }
    catch(std::bad_alloc&)
    {
        throw POV_EXCEPTION(kOutOfMemoryErr, "Insufficient memory to allocate intermediate image storage.");
    }
}

Image *Image::Create(unsigned int w, unsigned int h, ImageDataType t, const vector<RGBMapEntry>& m, bool allowFileBacking)
{
    try
    {
        switch(t)
        {
            case Bit_Map:
                return new MemoryBitMapImage(w, h, m);
            case Colour_Map:
                return new MemoryColourMapImage(w, h, m);
            case Gray_Int8:
                return new MemoryGray8Image(w, h, m);
            case Gray_Int16:
                return new MemoryGray16Image(w, h, m);
            case GrayA_Int8:
                return new MemoryGrayA8Image(w, h, m);
            case GrayA_Int16:
                return new MemoryGrayA16Image(w, h, m);
            case RGB_Int8:
                return new MemoryRGB8Image(w, h, m);
            case RGB_Int16:
                return new MemoryRGB16Image(w, h, m);
            case RGBA_Int8:
                return new MemoryRGBA8Image (w, h, m);
            case RGBA_Int16:
                return new MemoryRGBA16Image(w, h, m);
            case RGBFT_Float:
#ifdef FILE_MAPPED_IMAGE_ALLOCATOR
                if (allowFileBacking)
                    return new RGBFTImage<FILE_MAPPED_IMAGE_ALLOCATOR<float> >(w, h, m);
#endif
                return new MemoryRGBFTImage(w, h, m);
            case RGB_Gamma8:
                return new MemoryNonlinearRGB8Image(w, h, m);
            case RGB_Gamma16:
                return new MemoryNonlinearRGB16Image(w, h, m);
            case RGBA_Gamma8:
                return new MemoryNonlinearRGBA8Image (w, h, m);
            case RGBA_Gamma16:
                return new MemoryNonlinearRGBA16Image(w, h, m);
            case Gray_Gamma8:
                return new MemoryNonlinearGray8Image(w, h, m);
            case Gray_Gamma16:
                return new MemoryNonlinearGray16Image(w, h, m);
            case GrayA_Gamma8:
                return new MemoryNonlinearGrayA8Image(w, h, m);
            case GrayA_Gamma16:
                return new MemoryNonlinearGrayA16Image(w, h, m);
            default:
                throw POV_EXCEPTION_STRING("Image::Create Exception TODO"); // TODO FIXME WIP
        }
    }
    catch(std::bad_alloc&)
    {
        throw POV_EXCEPTION(kOutOfMemoryErr, "Insufficient memory to allocate intermediate image storage.");
    }
}

Image *Image::Create(unsigned int w, unsigned int h, ImageDataType t, const vector<RGBAMapEntry>& m, bool allowFileBacking)
{
    try
    {
        switch(t)
        {
            case Bit_Map:
                return new MemoryBitMapImage(w, h, m);
            case Colour_Map:
                return new MemoryColourMapImage(w, h, m);
            case Gray_Int8:
                return new MemoryGray8Image(w, h, m);
            case Gray_Int16:
                return new MemoryGray16Image(w, h, m);
            case GrayA_Int8:
                return new MemoryGrayA8Image(w, h, m);
            case GrayA_Int16:
                return new MemoryGrayA16Image(w, h, m);
            case RGB_Int8:
                return new MemoryRGB8Image(w, h, m);
            case RGB_Int16:
                return new MemoryRGB16Image(w, h, m);
            case RGBA_Int8:
                return new MemoryRGBA8Image (w, h, m);
            case RGBA_Int16:
                return new MemoryRGBA16Image(w, h, m);
            case RGBFT_Float:
#ifdef FILE_MAPPED_RGBFT_IMAGE_ALLOCATOR
                if (allowFileBacking)
                    return new RGBFTImage<FILE_MAPPED_RGBFT_IMAGE_ALLOCATOR<float> >(w, h, m);
#endif
                return new MemoryRGBFTImage(w, h, m);
            case RGB_Gamma8:
                return new MemoryNonlinearRGB8Image(w, h, m);
            case RGB_Gamma16:
                return new MemoryNonlinearRGB16Image(w, h, m);
            case RGBA_Gamma8:
                return new MemoryNonlinearRGBA8Image (w, h, m);
            case RGBA_Gamma16:
                return new MemoryNonlinearRGBA16Image(w, h, m);
            case Gray_Gamma8:
                return new MemoryNonlinearGray8Image(w, h, m);
            case Gray_Gamma16:
                return new MemoryNonlinearGray16Image(w, h, m);
            case GrayA_Gamma8:
                return new MemoryNonlinearGrayA8Image(w, h, m);
            case GrayA_Gamma16:
                return new MemoryNonlinearGrayA16Image(w, h, m);
            default:
                throw POV_EXCEPTION_STRING("Image::Create Exception TODO"); // TODO FIXME WIP
        }
    }
    catch(std::bad_alloc&)
    {
        throw POV_EXCEPTION(kOutOfMemoryErr, "Insufficient memory to allocate intermediate image storage.");
    }
}

Image *Image::Create(unsigned int w, unsigned int h, ImageDataType t, const vector<RGBFTMapEntry>& m, bool allowFileBacking)
{
    try
    {
        switch(t)
        {
            case Bit_Map:
                return new MemoryBitMapImage(w, h, m);
            case Colour_Map:
                return new MemoryColourMapImage(w, h, m);
            case Gray_Int8:
                return new MemoryGray8Image(w, h, m);
            case Gray_Int16:
                return new MemoryGray16Image(w, h, m);
            case GrayA_Int8:
                return new MemoryGrayA8Image(w, h, m);
            case GrayA_Int16:
                return new MemoryGrayA16Image(w, h, m);
            case RGB_Int8:
                return new MemoryRGB8Image(w, h, m);
            case RGB_Int16:
                return new MemoryRGB16Image(w, h, m);
            case RGBA_Int8:
                return new MemoryRGBA8Image (w, h, m);
            case RGBA_Int16:
                return new MemoryRGBA16Image(w, h, m);
            case RGBFT_Float:
#ifdef FILE_MAPPED_IMAGE_ALLOCATOR
                if (allowFileBacking)
                    return new RGBFTImage<FILE_MAPPED_IMAGE_ALLOCATOR<float> >(w, h, m);
#endif
                return new MemoryRGBFTImage(w, h, m);
            case RGB_Gamma8:
                return new MemoryNonlinearRGB8Image(w, h, m);
            case RGB_Gamma16:
                return new MemoryNonlinearRGB16Image(w, h, m);
            case RGBA_Gamma8:
                return new MemoryNonlinearRGBA8Image (w, h, m);
            case RGBA_Gamma16:
                return new MemoryNonlinearRGBA16Image(w, h, m);
            case Gray_Gamma8:
                return new MemoryNonlinearGray8Image(w, h, m);
            case Gray_Gamma16:
                return new MemoryNonlinearGray16Image(w, h, m);
            case GrayA_Gamma8:
                return new MemoryNonlinearGrayA8Image(w, h, m);
            case GrayA_Gamma16:
                return new MemoryNonlinearGrayA16Image(w, h, m);
            default:
                throw POV_EXCEPTION_STRING("Image::Create Exception TODO"); // TODO FIXME WIP
        }
    }
    catch(std::bad_alloc&)
    {
        throw POV_EXCEPTION(kOutOfMemoryErr, "Insufficient memory to allocate intermediate image storage.");
    }
}

Image *Image::Read(ImageFileType type, IStream *file, const ReadOptions& options)
{
    #ifdef SYS_TO_STANDARD
        if (type == SYS)
            type = SYS_TO_STANDARD ;
    #endif

    switch (type)
    {
        case HDR:
            return (HDR::Read(file, options));

        case EXR:
#ifndef OPENEXR_MISSING
            return (OpenEXR::Read(file, options));
#else
            throw POV_EXCEPTION(kCannotOpenFileErr,
"This unofficial POV-Ray binary was built without support for the OpenEXR \
file format.  You must either use an official POV-Ray binary or recompile \
the POV-Ray sources on a system providing you with the OpenEXR library \
to make use of this facility.  Alternatively, you may use any of the \
following built-in formats: HDR.");
            return NULL;
#endif

        case PNG:
#ifndef LIBPNG_MISSING
            return (Png::Read(file, options));
#else
            throw POV_EXCEPTION(kCannotOpenFileErr,
"This unofficial POV-Ray binary was built without support for the PNG \
file format.  You must either use an official POV-Ray binary or recompile \
the POV-Ray sources on a system providing you with the libPNG library \
to make use of this facility.  Alternatively, you may use any of the \
following built-in formats: GIF, TGA, IFF, PGM, PPM, BMP.");
            return NULL;
#endif

        case GIF:
            return (Gif::Read(file, options, false));

        case POT:
            return (Gif::Read(file, options, true));

        case TGA:
            return (Targa::Read(file, options));

        case JPEG:
#ifndef LIBJPEG_MISSING
            return (Jpeg::Read(file, options));
#else
            throw POV_EXCEPTION(kCannotOpenFileErr,
"This unofficial POV-Ray binary was built without support for the JPEG \
file format.  You must either use an official POV-Ray binary or recompile \
the POV-Ray sources on a system providing you with the libJPEG library \
to make use of this facility.  Alternatively, you may use any of the \
following built-in formats: GIF, TGA, IFF, PGM, PPM, BMP.");
            return NULL;
#endif

        case IFF:
            return (Iff::Read(file, options));

        case PGM:
            return (Pgm::Read(file, options));

        case PPM:
            return (Ppm::Read(file, options));

        case BMP:
            return (Bmp::Read(file, options));

        case TIFF:
#ifndef LIBTIFF_MISSING
            return (Tiff::Read(file, options));
#else
            throw POV_EXCEPTION(kCannotOpenFileErr,
"This unofficial POV-Ray binary was built without support for the TIFF \
file format.  You must either use an official POV-Ray binary or recompile \
the POV-Ray sources on a system providing you with the libTIFF library \
to make use of this facility.  Alternatively, you may use any of the \
following built-in formats: GIF, TGA, IFF, PGM, PPM, BMP.");
            return NULL;
#endif

        case SYS:
            throw POV_EXCEPTION(kCannotOpenFileErr, "This platform has not defined a SYS file type");
            return (NULL);

        default :
            throw POV_EXCEPTION(kParamErr, "Invalid file type");
            return (NULL);
    }
}

void Image::Write(ImageFileType type, OStream *file, const Image *image, const WriteOptions& options)
{
    if (image->GetWidth() == 0 || image->GetHeight() == 0)
        throw POV_EXCEPTION(kParamErr, "Invalid image size for output");

    if (file == NULL)
        throw POV_EXCEPTION(kCannotOpenFileErr, "Invalid image file");

#ifdef SYS_TO_STANDARD
    if (type == SYS)
        type = SYS_TO_STANDARD ;
#endif

    switch (type)
    {
        case GIF:
        case IFF:
        case PGM:
        case TIFF:
        case POT:
            throw POV_EXCEPTION(kParamErr, "Unsupported file type for output");
            break;

        case SYS:
            throw POV_EXCEPTION(kCannotOpenFileErr, "This platform has not defined a SYS file type");
            break;

        case HDR:
            HDR::Write(file, image, options);
            break;

        case EXR:
#ifndef OPENEXR_MISSING
            OpenEXR::Write(file, image, options);
#else
            throw POV_EXCEPTION(kParamErr,
"This unofficial POV-Ray binary was built without support for the OpenEXR \
file format.  You must either use an official POV-Ray binary or recompile \
the POV-Ray sources on a system providing you with the OpenEXR library \
to make use of this facility.  Alternatively, you may use any of the \
following built-in formats: HDR.");
#endif
            break;

        case PNG:
#ifndef LIBPNG_MISSING
            Png::Write(file, image, options);
#else
            throw POV_EXCEPTION(kParamErr,
"This unofficial POV-Ray binary was built without support for the PNG \
file format.  You must either use an official POV-Ray binary or recompile \
the POV-Ray sources on a system providing you with the libPNG library \
to make use of this facility.  Alternatively, you may use any of the \
following built-in formats: TGA, PPM, BMP.");
#endif
            break;

        case TGA:
            Targa::Write(file, image, options);
            break;

        case PPM:
            Ppm::Write(file, image, options);
            break;

        case BMP:
            Bmp::Write(file, image, options);
            break;

        case JPEG:
#ifndef LIBJPEG_MISSING
            Jpeg::Write(file, image, options);
#else
            throw POV_EXCEPTION(kParamErr,
"This unofficial POV-Ray binary was built without support for the JPEG \
file format.  You must either use an official POV-Ray binary or recompile \
the POV-Ray sources on a system providing you with the libJPEG library \
to make use of this facility.  Alternatively, you may use any of the \
following built-in formats: TGA, PPM, BMP.");
#endif
            break;

        default :
            throw POV_EXCEPTION(kParamErr, "Invalid file type");
            break;
    }
}

void Image::GetRGBIndexedValue(unsigned char index, float& red, float& green, float& blue) const
{
    switch(colormaptype)
    {
        case NoColourMap:
            red = 0.0f;
            green = 0.0f;
            blue = 0.0f;
            break;
        case RGBColourMap:
        case RGBAColourMap:
        case RGBFTColourMap:
            red = colormap[index].red;
            green = colormap[index].green;
            blue = colormap[index].blue;
            break;
    }
}

void Image::GetRGBAIndexedValue(unsigned char index, float& red, float& green, float& blue, float& alpha) const
{
    switch(colormaptype)
    {
        case NoColourMap:
            red = 0.0f;
            green = 0.0f;
            blue = 0.0f;
            alpha = ALPHA_OPAQUE;
            break;
        case RGBColourMap:
            red = colormap[index].red;
            green = colormap[index].green;
            blue = colormap[index].blue;
            alpha = ALPHA_OPAQUE;
            break;
        case RGBAColourMap:
            red = colormap[index].red;
            green = colormap[index].green;
            blue = colormap[index].blue;
            alpha = colormap[index].filter; // with RGBAColourMap, .filter is actually alpha
            break;
        case RGBFTColourMap:
            red = colormap[index].red;
            green = colormap[index].green;
            blue = colormap[index].blue;
            alpha = RGBFTColour::FTtoA(colormap[index].filter, colormap[index].transm);
            break;
    }
}

void Image::GetRGBFTIndexedValue(unsigned char index, float& red, float& green, float& blue, float& filter, float& transm) const
{
    switch(colormaptype)
    {
        case NoColourMap:
            red = 0.0f;
            green = 0.0f;
            blue = 0.0f;
            filter = FT_OPAQUE;
            transm = FT_OPAQUE;
            break;
        case RGBColourMap:
            red = colormap[index].red;
            green = colormap[index].green;
            blue = colormap[index].blue;
            filter = transm = FT_OPAQUE;
            break;
        case RGBAColourMap:
            red = colormap[index].red;
            green = colormap[index].green;
            blue = colormap[index].blue;
            RGBFTColour::AtoFT(colormap[index].filter, filter, transm); // with RGBAColourMap, .filter is actually alpha
            break;
        case RGBFTColourMap:
            red = colormap[index].red;
            green = colormap[index].green;
            blue = colormap[index].blue;
            filter = colormap[index].filter;
            transm = colormap[index].transm;
            break;
    }
}

void Image::SetRGBIndexedValue(unsigned char index, float red, float green, float blue)
{
    switch(colormaptype)
    {
        case NoColourMap:
            break;
        case RGBColourMap:
            colormap[index].red = red;
            colormap[index].green = green;
            colormap[index].blue = blue;
            colormap[index].filter = 0.0f; // not used with RGBColourMap
            colormap[index].transm = 0.0f; // not used with RGBColourMap
            break;
        case RGBAColourMap:
            colormap[index].red = red;
            colormap[index].green = green;
            colormap[index].blue = blue;
            colormap[index].filter = ALPHA_OPAQUE; // with RGBAColourMap, .filter is actually alpha
            colormap[index].transm = 0.0f; // not used with RGBAColourMap
            break;
        case RGBFTColourMap:
            colormap[index].red = red;
            colormap[index].green = green;
            colormap[index].blue = blue;
            colormap[index].filter = FT_OPAQUE;
            colormap[index].transm = FT_OPAQUE;
            break;
    }
}

void Image::SetRGBAIndexedValue(unsigned char index, float red, float green, float blue, float alpha)
{
    switch(colormaptype)
    {
        case NoColourMap:
            break;
        case RGBColourMap:
            colormap[index].red = red;
            colormap[index].green = green;
            colormap[index].blue = blue;
            colormap[index].filter = 0.0f; // not used with RGBColourMap
            colormap[index].transm = 0.0f; // not used with RGBColourMap
            break;
        case RGBAColourMap:
            colormap[index].red = red;
            colormap[index].green = green;
            colormap[index].blue = blue;
            colormap[index].filter = alpha; // with RGBAColourMap, .filter is actually alpha
            colormap[index].transm = 0.0f; // not used with RGBAColourMap
            break;
        case RGBFTColourMap:
            colormap[index].red = red;
            colormap[index].green = green;
            colormap[index].blue = blue;
            RGBFTColour::AtoFT(alpha, colormap[index].filter, colormap[index].transm);
            break;
    }
}

void Image::SetRGBFTIndexedValue(unsigned char index, float red, float green, float blue, float filter, float transm)
{
    switch(colormaptype)
    {
        case NoColourMap:
            break;
        case RGBColourMap:
            colormap[index].red = red;
            colormap[index].green = green;
            colormap[index].blue = blue;
            colormap[index].filter = 0.0f; // not used with RGBColourMap
            colormap[index].transm = 0.0f; // not used with RGBColourMap
            break;
        case RGBAColourMap:
            colormap[index].red = red;
            colormap[index].green = green;
            colormap[index].blue = blue;
            colormap[index].filter = RGBFTColour::FTtoA(filter, transm); // note: filter is alpha in RGBA maps
            colormap[index].transm = 0.0f; // not used with RGBAColourMap
            break;
        case RGBFTColourMap:
            colormap[index].red = red;
            colormap[index].green = green;
            colormap[index].blue = blue;
            colormap[index].filter = filter;
            colormap[index].transm = transm;
            break;
    }
}

void Image::SetRGBFTIndexedValue(unsigned char index, const RGBFTColour& col)
{
    switch(colormaptype)
    {
        case NoColourMap:
            break;
        case RGBColourMap:
            colormap[index].red = col.red();
            colormap[index].green = col.green();
            colormap[index].blue = col.blue();
            colormap[index].filter = 0.0f; // not used with RGBColourMap
            colormap[index].transm = 0.0f; // not used with RGBColourMap
            break;
        case RGBAColourMap:
            colormap[index].red = col.red();
            colormap[index].green = col.green();
            colormap[index].blue = col.blue();
            colormap[index].filter = col.FTtoA(); // note: filter is alpha in RGBA maps
            colormap[index].transm = 0.0f; // not used with RGBAColourMap
            break;
        case RGBFTColourMap:
            colormap[index].red = col.red();
            colormap[index].green = col.green();
            colormap[index].blue = col.blue();
            colormap[index].filter = col.filter();
            colormap[index].transm = col.transm();
            break;
    }
}

unsigned char Image::GetIndexedValue(unsigned int, unsigned int)
{
    return 0;
}

void Image::SetIndexedValue(unsigned int x, unsigned int y, unsigned char index)
{
    CHECK_BOUNDS(x, y);
    switch(colormaptype)
    {
        case NoColourMap:
            SetBitValue(x,y, false);
            break;
        case RGBColourMap:
            SetRGBValue(x, y, colormap[index].red, colormap[index].green, colormap[index].blue);
            break;
        case RGBAColourMap:
            SetRGBAValue(x, y, colormap[index].red, colormap[index].green, colormap[index].blue, colormap[index].filter); // with RGBAColourMap, .filter is actually alpha
            break;
        case RGBFTColourMap:
            SetRGBFTValue(x, y, colormap[index].red, colormap[index].green, colormap[index].blue, colormap[index].filter, colormap[index].transm);
            break;
    }
}

void Image::GetRGBValue(unsigned int x, unsigned int y, RGBColour& colour, bool premul) const
{
    if (premul && !premultiplied && HasTransparency())
    {
        // data is non-premultiplied, but caller expects premultiplied data
        float alpha;
        GetRGBAValue(x, y, colour.red(), colour.green(), colour.blue(), alpha);
        AlphaPremultiply(colour, alpha);
    }
    if (!premul && premultiplied && HasTransparency())
    {
        // data is premultiplied, but caller expects non-premultiplied data
        float alpha;
        GetRGBAValue(x, y, colour.red(), colour.green(), colour.blue(), alpha);
        AlphaUnPremultiply(colour, alpha);
    }
    else
    {
        GetRGBValue(x, y, colour.red(), colour.green(), colour.blue());
    }
}
void Image::GetRGBTValue(unsigned int x, unsigned int y, RGBTColour& colour, bool premul) const
{
    float alpha;
    GetRGBAValue(x, y, colour.red(), colour.green(), colour.blue(), alpha);
    if (premul && !premultiplied && HasTransparency())
    {
        // data is non-premultiplied, but caller expects premultiplied data
        AlphaPremultiply(colour.rgb(), alpha);
    }
    else if (!premul && premultiplied && HasTransparency())
    {
        // data is premultiplied, but caller expects non-premultiplied data
        AlphaUnPremultiply(colour.rgb(), alpha);
    }
    colour.transm() = 1.0 - alpha;
}
void Image::GetRGBFTValue(unsigned int x, unsigned int y, RGBFTColour& colour, bool premul) const
{
    GetRGBFTValue(x, y, colour.red(), colour.green(), colour.blue(), colour.filter(), colour.transm());
    if (premul && !premultiplied && HasTransparency())
    {
        // data is non-premultiplied, but caller expects premultiplied data
        AlphaPremultiply(colour);
    }
    else if (!premul && premultiplied && HasTransparency())
    {
        // data is premultiplied, but caller expects non-premultiplied data
        AlphaUnPremultiply(colour);
    }
}

unsigned int Image::GetColourMapSize() const
{
    return colormap.size();
}

void Image::GetColourMap(vector<RGBMapEntry>& m) const
{
    m.resize(colormap.size());
    for(size_t i = 0; i < colormap.size(); i++)
        GetRGBIndexedValue((unsigned char)(i), m[i].red, m[i].green, m[i].blue);
}

void Image::GetColourMap(vector<RGBAMapEntry>& m) const
{
    m.resize(colormap.size());
    for(size_t i = 0; i < colormap.size(); i++)
        GetRGBAIndexedValue((unsigned char)(i), m[i].red, m[i].green, m[i].blue, m[i].alpha);
}

void Image::GetColourMap(vector<RGBFTMapEntry>& m) const
{
    m.resize(colormap.size());
    for(size_t i = 0; i < colormap.size(); i++)
        GetRGBFTIndexedValue((unsigned char)(i), m[i].red, m[i].green, m[i].blue, m[i].filter, m[i].transm);
}

void Image::SetColourMap(const vector<RGBMapEntry>& m)
{
    colormap.resize(max(m.size(), sizeof(unsigned char) * 256));
    colormaptype = RGBColourMap;
    colormap.assign(m.begin(), m.end());
}

void Image::SetColourMap(const vector<RGBAMapEntry>& m)
{
    colormap.resize(max(m.size(), sizeof(unsigned char) * 256));
    colormaptype = RGBAColourMap;
    colormap.assign(m.begin(), m.end());
}

void Image::SetColourMap(const vector<RGBFTMapEntry>& m)
{
    colormap.resize(max(m.size(), sizeof(unsigned char) * 256));
    colormaptype = RGBFTColourMap;
    colormap.assign(m.begin(), m.end());
}

}
