/*******************************************************************************
 * imageutil.h
 *
 * This module contains all defines, typedefs, and prototypes for imageutil.cpp.
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
 * $File: //depot/public/povray/3.x/source/backend/support/imageutil.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include "base/image/image.h"

namespace pov
{

using namespace pov_base;

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

// Image/Bump Map projection types.

enum
{
	PLANAR_MAP       = 0,
	SPHERICAL_MAP    = 1,
	CYLINDRICAL_MAP  = 2,
	PARABOLIC_MAP    = 3,
	HYPERBOLIC_MAP   = 4,
	TORUS_MAP        = 5,
	PIRIFORM_MAP     = 6,
	OLD_MAP          = 7
};

// Bit map interpolation types.

enum
{
	// NO_INTERPOLATION = 0, // already in pattern.h
	NEAREST_NEIGHBOR = 1, // currently not supported; would be essentially the same as NO_INTERPOLATION
	BILINEAR         = 2,
	BICUBIC          = 3,
	NORMALIZED_DIST  = 4
};

enum
{
	USE_NONE         = 0,
	USE_COLOUR       = 1,
	USE_INDEX        = 2,
	USE_ALPHA        = 3
};

class ImageData
{
	public:
		int References; // Keeps track of number of pointers to this structure
		int Map_Type;
		int Interpolation_Type;
		bool Once_Flag;
		char Use;
		VECTOR Gradient;
		int iwidth, iheight;
		SNGL width, height;
		UV_VECT Offset;
		DBL AllFilter, AllTransmit;
		void *Object;
		Image *data;

// it would have been a lot cleaner if POV_VIDCAP_IMPL was a subclass of pov::Image,
// since we could just assign it to data above and the following would not be needed.
// however at this point pov::Image doesn't allow the default constructor to be used,
// and we don't know our capture size or data type until after we talk to the hardware.
#ifdef POV_VIDCAP_IMPL
		POV_VIDCAP_IMPL *VidCap;
#endif

};

class Parser;
class SceneData;

DBL image_pattern(const VECTOR EPoint, const TPATTERN *TPattern);
bool image_map(const VECTOR EPoint, const PIGMENT *Pigment, Colour& colour);
TEXTURE *material_map(const VECTOR IPoint, const TEXTURE *Texture);
void bump_map(const VECTOR EPoint, const TNORMAL *Tnormal, VECTOR normal);
HF_VAL image_height_at(const ImageData *image, int x, int y);
bool is_image_opaque(const ImageData *image);
ImageData *Copy_Image(ImageData *old);
ImageData *Create_Image(void);
void Destroy_Image(ImageData *image);
Image *Read_Image(Parser *p, shared_ptr<SceneData>& sd, int filetype, const UCS2 *filename, const Image::ReadOptions& options);

}

#endif
