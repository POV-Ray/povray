//******************************************************************************
///
/// @file parser/ImageCache.h
///
/// Declarations related to the Image Cache.
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

#ifndef POVRAY_PARSER_IMAGE_CACHE_H
#define POVRAY_PARSER_IMAGE_CACHE_H

#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h> // Unix lib for getting last modified file date and time
#endif
#ifdef WIN32
#define stat _stat // Windows lib for getting last modified file date and time
#endif


// POV-Ray header files (base module)
#include "base/base_fwd.h"
#include "base/messenger_fwd.h"
#include "base/povassert.h"
#include "base/stringtypes.h"
#include "base/textstream_fwd.h"
#include "base/textstreambuffer.h"
#include "base/image/image_fwd.h"

namespace pov
{
	class Blob_Element;
	struct ContainedByShape;
	struct GenericSpline;
	class ImageData;
	class Mesh;
	struct PavementPattern;
	struct TilingPattern;
	struct TrueTypeFont;
}

namespace pov_image_cache
{
	using namespace pov_base;

	Image* GetCachedImage(const UCS2* filename);
	void StoreImageInCache(const UCS2* filename, Image* image);
};

#endif // POVRAY_PARSER_IMAGE_CACHE_H