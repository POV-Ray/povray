//******************************************************************************
///
/// @file parser/ImageCache.cpp
///
/// This module implements a cache for images used in a scene so they only heve
/// to be loaded once during animation rendering or between manual renders
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


// C++ variants of C standard header files
#include <map>
#include <uchar.h>

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

// this must be the last file included
#include "base/povdebug.h"


namespace pov_image_cache
{
	using namespace pov_base;
	using namespace std;
	static inline std::string U16toString(const std::u16string& wstr);
	static inline char* StrToChar(const std::string str);
	static inline char* U16toChar(const std::u16string& wstr);

	struct ImageCacheEntry final
	{
		Image* image;
		__time64_t lastModified;
	};

	static std::map<std::string, ImageCacheEntry> Cache; // <- The actual cache

	// Gets the last modified time from the filesystem
	__time64_t GetLastModifiedTime(const std::string filename)
	{
		char* cstrFilename = StrToChar(filename);

		struct stat result;
		if (stat(cstrFilename, &result) == 0)
		{
			delete cstrFilename;
			return result.st_mtime;
		}
		delete cstrFilename;
		return 0;
	}

	// Try to get the image from the cache
	Image* GetCachedImage(const UCS2* filename)
	{
		std::string lookupFilename = U16toString(filename);
		
		std::map<std::string, ImageCacheEntry>::iterator idx = Cache.find(lookupFilename);
		if (idx != Cache.end()) 
		{
			__time64_t lastModified = GetLastModifiedTime(lookupFilename);
			if (lastModified == Cache[lookupFilename].lastModified) 
				return idx->second.image; //Cache[lookupFilename].image;
			
			// Remove old image from cache and release memory so the newer version can be loaded
			delete idx->second.image;
			Cache.erase(idx);
		}

		return nullptr;
	}

	// Store a new image into cache
	void StoreImageInCache(const UCS2* filename, Image* image)
	{
		std::string lookupFilename = U16toString(filename);
		__time64_t lastModified = GetLastModifiedTime(lookupFilename);
		Cache[lookupFilename] = ImageCacheEntry{ image = image, lastModified = lastModified };
	}

	// May be called frome some menu item, personally, I'd just close PovRay and start a new process (different scenes often share resources in my case)
	// Do not allow calling it while parsing or rendering!
	void ClearCache() 
	{
		std::map<std::string, ImageCacheEntry>::iterator it = Cache.begin();

		// Iterate over the map using Iterator till end.
		while (it != Cache.end())
		{
			delete it->second.image;
			Cache.erase(it);
		}
	}

	static inline std::string U16toString(const std::u16string& wstr) 
	{
		std::string str = "";
		char cstr[3] = "\0";
		mbstate_t mbs;
		for (const auto& it : wstr) {
			memset(&mbs, 0, sizeof(mbs));//set shift state to the initial state
			memmove(cstr, "\0\0\0", 3);
			c16rtomb(cstr, it, &mbs);
			str.append(std::string(cstr));
		}//for
		return str;
	}

	static inline char* StrToChar(const std::string str) 
	{
		char* cstring = new char[str.length() + 1];
		strcpy(cstring, str.c_str());
		return cstring;
	}

	static inline char* U16toChar(const std::u16string& wstr) 
	{
		return StrToChar(U16toString(wstr));
	}

}