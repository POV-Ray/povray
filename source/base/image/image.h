/*******************************************************************************
 * image.h
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
 * $File: //depot/public/povray/3.x/source/base/image/image.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef POVRAY_BASE_IMAGE_H
#define POVRAY_BASE_IMAGE_H

#include "base/configbase.h"
#include "base/fileinputoutput.h"
#include "base/pov_err.h"
#include "base/image/colourspace.h"
#include "base/image/encoding.h"

namespace pov_base
{

/**
 *  Generic image data container.
 *
 *  @note   Except for access functions having a @c premul parameter as well as those named
 *          @c GetEncodedSomethingValue or SetEncodedSomethingValue, all other access functions are unaware of
 *          premultiplied vs. non-premultiplied alpha issues, and will access the data in whatever format
 *          it is stored (as far as alpha handling goes).
 *
 *  @note   When backed by a gamma-encoded data container, unsigned int access methods are presumed
 *          to read/write raw encoded data, while float access methods will read/write logical
 *          linear values.
 */
class Image
{
	public:
		struct RGBMapEntry
		{
			float red;
			float green;
			float blue;

			RGBMapEntry() : red(0.0f), green(0.0f), blue(0.0f) { }
			RGBMapEntry(float r, float g, float b) : red(r), green(g), blue(b) { }
		};

		struct RGBAMapEntry
		{
			float red;
			float green;
			float blue;
			float alpha;

			RGBAMapEntry() : red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f) { }
			RGBAMapEntry(float r, float g, float b, float a) : red(r), green(g), blue(b), alpha(a) { }
		};

		struct RGBFTMapEntry
		{
			float red;
			float green;
			float blue;
			float filter;
			float transm;

			RGBFTMapEntry() : red(0.0f), green(0.0f), blue(0.0f), filter(0.0f), transm(0.0f) { }
			RGBFTMapEntry(float r, float g, float b, float f, float t) : red(r), green(g), blue(b), filter(f), transm(t) { }
		};

		enum ColourMapType
		{
			NoColourMap,
			RGBColourMap,
			RGBAColourMap,
			RGBFTColourMap
		};

		enum ImageDataType
		{
			/// Value used to indicate that image decoder is free to pick the most fitting type.
			Undefined,
			/// Palette-based image with 2 palette entries.
			Bit_Map,
			/// Palette-based image with up to 256 palette entries.
			Colour_Map,
			/// Single-channel (grayscale) image using 8-bit linear encoding.
			Gray_Int8,
			/// Single-channel (grayscale) image using 16-bit linear encoding.
			Gray_Int16,
			/// Dual-channel (grayscale and alpha) image using 8-bit linear encoding.
			GrayA_Int8,
			/// Dual-channel (grayscale and alpha) image using 16-bit linear encoding.
			GrayA_Int16,
			/// 3-channel (colour) image using 8-bit linear encoding.
			RGB_Int8,
			/// 3-channel (colour) image using 16-bit linear encoding.
			RGB_Int16,
			/// 4-channel (colour and alpha) image using 8-bit linear encoding.
			RGBA_Int8,
			/// 4-channel (colour and alpha) image using 16-bit linear encoding.
			RGBA_Int16,
			/// 5-channel (colour, filter and transmit) image using single-precision floating-point encoding.
			RGBFT_Float,
			/// 3-channel (colour) image using 8-bit gamma encoding.
			RGB_Gamma8,
			/// 3-channel (colour) image using 16-bit gamma encoding.
			RGB_Gamma16,
			/// 4-channel (colour and alpha) image using 8-bit gamma colour encoding and 8-bit linear alpha encoding.
			RGBA_Gamma8,
			/// 4-channel (colour and alpha) image using 16-bit gamma colour encoding and 16-bit linear alpha encoding.
			RGBA_Gamma16,
			/// Single-channel (grayscale) image using 8-bit gamma encoding.
			Gray_Gamma8,
			/// Single-channel (grayscale) image using 16-bit gamma encoding.
			Gray_Gamma16,
			/// Dual-channel (grayscale and alpha) image using 8-bit gamma greyscale encoding and 8-bit linear alpha encoding.
			GrayA_Gamma8,
			/// Dual-channel (grayscale and alpha) image using 16-bit gamma greyscale encoding and 16-bit linear alpha encoding.
			GrayA_Gamma16
		};

		enum ImageFileType
		{
			GIF,
			POT,
			SYS,
			IFF,
			TGA,
			PGM,
			PPM,
			PNG,
			JPEG,
			TIFF,
			BMP,
			EXR,
			HDR
		};

		struct ReadOptions
		{
			ImageDataType itype;
			SimpleGammaCurvePtr defaultGamma;   // the gamma curve to use by default for converting to linear colour space
			SimpleGammaCurvePtr workingGamma;   // the working colour space gamma
			bool gammaOverride;                 // whether to apply defaultGamma even if the file indicates a different gamma
			bool gammacorrect;                  // whether to do any gamma correction at all; if false, raw encoded values are used
			bool premultiplyOverride;           // whether to override file-format default for alpha premultiplication
			bool premultiply;                   // whether to expect premultiplied ("associated") alpha or not ("straight alpha")
			mutable vector<string> warnings;

			ReadOptions() : itype(Undefined), gammaOverride(false), gammacorrect(false), premultiplyOverride(false), premultiply(false) { }
		};

		struct WriteOptions
		{
			unsigned char bpcc; // bits per colour component
			bool alphachannel;
			bool grayscale;
			unsigned char compress;
			SimpleGammaCurvePtr encodingGamma;  // the gamma curve to use for encoding from linear if the file format leaves any choice (NULL to use file format recommendation)
			SimpleGammaCurvePtr workingGamma;   // the working colour space gamma
			bool premultiplyOverride;           // whether to override file-format default for alpha premultiplication
			bool premultiply;                   // whether to output premultiplied ("associated") alpha or not ("straight alpha")
			DitherHandlerPtr dither;
			unsigned int offset_x;
			unsigned int offset_y;

			WriteOptions() : bpcc(8), alphachannel(false), grayscale(false), compress(0) /*, gamma(1.0f) */, premultiplyOverride(false), premultiply(false), offset_x(0), offset_y(0) { }
		};

		virtual ~Image() { }

		static Image *Create(unsigned int w, unsigned int h, ImageDataType t, unsigned int maxRAMmbHint, unsigned int pixelsPerBlockHint);
		static Image *Create(unsigned int w, unsigned int h, ImageDataType t, bool allowFileBacking = false);
		static Image *Create(unsigned int w, unsigned int h, ImageDataType t, const vector<RGBMapEntry>& m, bool allowFileBacking = false);
		static Image *Create(unsigned int w, unsigned int h, ImageDataType t, const vector<RGBAMapEntry>& m, bool allowFileBacking = false);
		static Image *Create(unsigned int w, unsigned int h, ImageDataType t, const vector<RGBFTMapEntry>& m, bool allowFileBacking = false);

		// ftype = use this image type, if "Undefined" use best match
		static Image *Read(ImageFileType ftype, IStream *file, const ReadOptions& options = ReadOptions());

		// bitperpixel = use this number of bits per pixel or closest supported match, if "0" use best match
		// compress = if "0" use no compression, other values use fomat specific compression (TBD)
		static void Write(ImageFileType ftype, OStream *file, const Image *image, const WriteOptions& options = WriteOptions());

		unsigned int GetWidth() const { return width; }
		unsigned int GetHeight() const { return height; }
		ImageDataType GetImageDataType() const { return type; }

		/// Returns true if image is fully opaque.
		virtual bool IsOpaque() const = 0;

		virtual bool IsGrayscale() const = 0;
		virtual bool IsColour() const = 0;
		virtual bool IsFloat() const = 0;
		virtual bool IsInt() const = 0;

		/// Returns true if backed by a palette-based container.
		virtual bool IsIndexed() const = 0;

		/// Returns true if backed by a gamma-encoded data container.
		virtual bool IsGammaEncoded() const = 0;

		/// Returns true if container features a genuine alpha channel.
		virtual bool HasAlphaChannel() const = 0;

		/// Returns true if container features genuine filter & transmit channels.
		virtual bool HasFilterTransmit() const = 0;

		/// Returns true if container features any way of storing transparency information.
		virtual bool HasTransparency() const { return (HasAlphaChannel() || HasFilterTransmit()); }

		/// Returns the maximum value supported by int access methods or for palette indices (1, 255 or 65535).
		virtual unsigned int GetMaxIntValue() const = 0;

		/// Specifies whether container holds color data premultiplied with alpha
		void SetPremultiplied(bool b) { premultiplied = b; } // TODO - mechanism not fully functional yet for image reading

		/// Returns true if container holds data premultiplied with alpha
		bool IsPremultiplied() const { return premultiplied; }

		/**
		 *  Requests the image container to perform deferred decoding of integer values.
		 *  In order for the request to be honored, the requested value range must match the container's native
		 *  bit depth, and the container must have a neutral encoding gamma curve set at present.
		 *  @note           If the request is honored, this will also affect subsequent unsigned int read accesses.
		 *  @param[in,out]  gamma   Gamma encoding curve of the encoded material. Set to empty by the function
		 *                          if the request is accepted.
		 *  @param[in]      max     Maximum encoded value. In order for the request to be honored, this must match
		 *                          the image container's native bit depth.
		 *  @return                 true it the request is accepted, false otherwise.
		 */
		virtual bool TryDeferDecoding(GammaCurvePtr& gamma, unsigned int max) = 0;

		void GetRGBIndexedValue(unsigned char index, float& red, float& green, float& blue) const;
		void GetRGBAIndexedValue(unsigned char index, float& red, float& green, float& blue, float& alpha) const;
		void GetRGBFTIndexedValue(unsigned char index, float& red, float& green, float& blue, float& filter, float& transm) const;

		void SetRGBIndexedValue(unsigned char index, float red, float green, float blue);
		void SetRGBAIndexedValue(unsigned char index, float red, float green, float blue, float alpha);
		void SetRGBFTIndexedValue(unsigned char index, float red, float green, float blue, float filter, float transm);
		void SetRGBFTIndexedValue(unsigned char index, const Colour& colour);

		virtual bool GetBitValue(unsigned int x, unsigned int y) const = 0;
		virtual float GetGrayValue(unsigned int x, unsigned int y) const = 0;
		virtual void GetGrayAValue(unsigned int x, unsigned int y, float& gray, float& alpha) const = 0;
		virtual void GetRGBValue(unsigned int x, unsigned int y, float& red, float& green, float& blue) const = 0;
		virtual void GetRGBAValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& alpha) const = 0;
		virtual void GetRGBFTValue(unsigned int x, unsigned int y, float& red, float& green, float& blue, float& filter, float& transm) const = 0;
		virtual unsigned char GetIndexedValue(unsigned int x, unsigned int y);

		virtual void SetBitValue(unsigned int x, unsigned int y, bool bit) = 0;
		virtual void SetGrayValue(unsigned int x, unsigned int y, float gray) = 0;
		virtual void SetGrayValue(unsigned int x, unsigned int y, unsigned int gray) = 0;
		virtual void SetGrayAValue(unsigned int x, unsigned int y, float gray, float alpha) = 0;
		virtual void SetGrayAValue(unsigned int x, unsigned int y, unsigned int gray, unsigned int alpha) = 0;
		virtual void SetRGBValue(unsigned int x, unsigned int y, float red, float green, float blue) = 0;
		virtual void SetRGBValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue) = 0;
		virtual void SetRGBAValue(unsigned int x, unsigned int y, float red, float green, float blue, float alpha) = 0;
		virtual void SetRGBAValue(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) = 0;
		virtual void SetRGBFTValue(unsigned int x, unsigned int y, float red, float green, float blue, float filter, float transm) = 0;
		virtual void SetRGBFTValue(unsigned int x, unsigned int y, const Colour& col) = 0;
		virtual void SetIndexedValue(unsigned int x, unsigned int y, unsigned char index);

		// convenience functions for image evaluation
		void GetRGBValue(unsigned int x, unsigned int y, RGBColour& colour, bool premul = false) const;
		void GetRGBFTValue(unsigned int x, unsigned int y, Colour& colour, bool premul = false) const;

		virtual void FillBitValue(bool bit) = 0;
		virtual void FillGrayValue(float gray) = 0;
		virtual void FillGrayValue(unsigned int gray) = 0;
		virtual void FillGrayAValue(float gray, float alpha) = 0;
		virtual void FillGrayAValue(unsigned int gray, unsigned int alpha) = 0;
		virtual void FillRGBValue(float red, float green, float blue) = 0;
		virtual void FillRGBValue(unsigned int red, unsigned int green, unsigned int blue) = 0;
		virtual void FillRGBAValue(float red, float green, float blue, float alpha) = 0;
		virtual void FillRGBAValue(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) = 0;
		virtual void FillRGBFTValue(float red, float green, float blue, float filter, float transm) = 0;
		virtual void FillIndexedValue(unsigned char index);

		unsigned int GetColourMapSize() const;

		void GetColourMap(vector<RGBMapEntry>& m) const;
		void GetColourMap(vector<RGBAMapEntry>& m) const;
		void GetColourMap(vector<RGBFTMapEntry>& m) const;

		void SetColourMap(const vector<RGBMapEntry>& m);
		void SetColourMap(const vector<RGBAMapEntry>& m);
		void SetColourMap(const vector<RGBFTMapEntry>& m);
/*
		void CopyTo(unsigned int x, unsigned int y, const Image& srcimage)
		{
			// TODO
		}
		void CopyToScaled(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const Image& srcimage, bool smooth = false)
		{
			// TODO
		}*/
	protected:
		struct MapEntry
		{
			float red;
			float green;
			float blue;
			float filter; // alpha = filter
			float transm;

			MapEntry() : red(0.0f), green(0.0f), blue(0.0f), filter(0.0f), transm(0.0f) { }
			MapEntry(float r, float g, float b, float f, float t) : red(r), green(g), blue(b), filter(f), transm(t) { }
			MapEntry(const RGBMapEntry& e) : red(e.red), green(e.green), blue(e.blue), filter(0.0f), transm(0.0f) { }
			MapEntry(const RGBAMapEntry& e) : red(e.red), green(e.green), blue(e.blue), filter(e.alpha), transm(0.0f) { }
			MapEntry(const RGBFTMapEntry& e) : red(e.red), green(e.green), blue(e.blue), filter(e.filter), transm(e.transm) { }
		};

		vector<MapEntry> colormap;
		ColourMapType colormaptype;
		unsigned int width;
		unsigned int height;
		ImageDataType type;
		bool premultiplied;

		Image(unsigned int w, unsigned int h, ImageDataType t) :
			width(w), height(h), type(t), colormaptype(NoColourMap), premultiplied(false) { }

		Image(unsigned int w, unsigned int h, ImageDataType t, const vector<RGBMapEntry>& m) :
			width(w), height(h), type(t), colormaptype(RGBColourMap), premultiplied(false) { colormap.resize(max(m.size(), sizeof(unsigned char) * 256)); colormap.assign(m.begin(), m.end()); }

		Image(unsigned int w, unsigned int h, ImageDataType t, const vector<RGBAMapEntry>& m) :
			width(w), height(h), type(t), colormaptype(RGBAColourMap), premultiplied(false) { colormap.resize(max(m.size(), sizeof(unsigned char) * 256)); colormap.assign(m.begin(), m.end()); }

		Image(unsigned int w, unsigned int h, ImageDataType t, const vector<RGBFTMapEntry>& m) :
			width(w), height(h), type(t), colormaptype(RGBFTColourMap), premultiplied(false) { colormap.resize(max(m.size(), sizeof(unsigned char) * 256)); colormap.assign(m.begin(), m.end()); }

		float RGB2Gray(float red, float green, float blue) const
		{
			return (red * 0.297f + green * 0.589f + blue * 0.114f);
		}
	private:
		/// not available
		Image();
		/// not available
		Image(const Image&);
		/// not available
		Image& operator=(Image&);
};

}

#endif // POVRAY_BASE_IMAGE_H
