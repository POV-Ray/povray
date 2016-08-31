//******************************************************************************
///
/// @file vfe/unix/platformbase.cpp
///
/// This file implements code that is needed (and linked to) the base code.
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

// must come first
#include "syspovconfig.h"

#ifdef HAVE_TIME_H
# include <time.h>
#endif

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#ifdef HAVE_SYS_RESOURCE_H
# include <sys/resource.h>
#endif

#include "vfe.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

    ////////////////////////////////////////////////////////////////////////////////////////
    //
    // thread support
    //
    ////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////
    // called by the base code each time a worker thread is created (the call
    // is made in the context of the new thread).
    void vfeSysThreadStartup(void)
    {
    }

    /////////////////////////////////////////////////////////////////////////
    // called by a worker thread just before it exits.
    void vfeSysThreadCleanup(void)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    //
    // path parsing
    //
    ////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////
    // The first argument is the input, a UCS2 string.
    //
    // The second argument is the string you are supposed to return the "volume"
    // name with.  For DOS-style paths this implies i.e. "A:\" is the "volume".
    // Note that it is essential that the first "path" separator is also part of
    // the volume name.  If the path is relative, the "volume" name shall be empty.
    //
    // This trick is necessary so the code can account for the lack of volume names
    // in Unix-style paths: In Unix, the POV_PARSE_PATH_STRING function will have
    // to take a reading "/" or "~/" as "volume" name.  This makes it possible to
    // determine if a string is absolute or relative based on this 'virtual'
    // "volume" name rather than some flags.
    //
    // The third is a vector of strings you have to return, each has to contain the
    // folder name from left to right, without the path separator, of course.
    //
    // The fourth argument shall contain the filename, if any was given in the
    // source string. By definition if the source string does not contain a
    // trailing path separator, whatever comes after the last path separator
    // (or the start of the string if there is none) must be considered a filename,
    // even if it could be a directory (in other words, don't call a system function
    // to find out if it is a dir or not - see below).
    //
    // Please note that the function must not attempt to determine the validity of
    // a string by accessing the filesystem.  It has to parse anything that it is
    // given.  If the string provided cannot be parsed for some reason (that is if
    // you can determine that a given path cannot possibly be valid i.e. because it
    // contains invalid characters), the function has to return false.  It may not
    // throw exceptions.  The return value for success is true.
    ////////////////////////////////////////////////////////////////////////////////////////

    bool vfeParsePathString (const UCS2String& path, UCS2String& volume, vector<UCS2String>& components, UCS2String& filename)
    {
        UCS2String q;

        if(path.empty() == true)
            return true;

        if(path[0] == '/')
            volume = '/';

        for(size_t i = 0; i < path.length(); ++i)
        {
            if(path[i] == '/')
            {
                if(q.empty() == false)
                    components.push_back(q);
                q.clear();
            }
            else
                q += path[i];
        }

        filename = q;

        return true;
    }
}
