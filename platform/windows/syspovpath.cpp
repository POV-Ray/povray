//******************************************************************************
///
/// @file platform/windows/syspovpath.cpp
///
/// Windows-specific partial implementation of the @ref Path class.
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

#include "syspovpath.h"

// this must be the last file included
#include "base/povdebug.h"

#include <vector>

#include "base/stringtypes.h"

namespace pov_base
{

//******************************************************************************

#if !POV_USE_DEFAULT_PATH_PARSER

/// Test for the special directory name denoting the current directory (`.`).
static inline bool IsCurrentDir(const UCS2String& s)
{
    return (s.length() == 1) && (s[0] == '.');
}

/// Test for the special directory name denoting the parent directory (`..`).
static inline bool IsParentDir(const UCS2String& s)
{
    return (s.length() == 2) && (s[0] == '.') && (s[1] == '.');
}

bool Path::ParsePathString (UCS2String& volume, std::vector<UCS2String>& dirnames, UCS2String& filename, const UCS2String& path)
{
    UCS2String stash;

    // Unless noted otherwise, all components are considered empty.
    volume.clear();
    dirnames.clear();
    filename.clear();

    // Empty strings are considered valid path names, too.
    if (path.empty() == true)
        return true;

    UCS2String::const_iterator i = path.begin();

    // Check for a volume identifier:
    // - If the second character is a colon, we presume the first two characters to identify the drive. In that case
    //   we'll also check whether the following character is a path separator, indicating an absolute path on that
    //   drive, in which case we'll also include a trailing separator to the drive letter.
    // - If the path starts with two identical path separator characters, we presume the string to be a UNC path, in
    //   which case we set the volume identifier to the network share, including two leading and a trailing separator
    //   characters.
    // - Otherwise, if the first character is a path separator, this indicates an absolute path on the current drive,
    //   in which case we set the volume identifier to a single path separator character.
    // - In any other case, we presume the string to be a relative path, and set the volume identifier to an empty
    //   string.

    if (POV_IS_PATH_SEPARATOR(*i))
    {
        // String starts with a path separator; may be an absolute path on the current drive or a UNC path.

        // Stash the separator (use the canonical one, not the one actually used).
        stash += POV_PATH_SEPARATOR;
        ++i;

        if ((i != path.end()) && (*i == stash[0]))
        {
            // The second character is also an (identical) separator character, indicating a UNC path.

            // Stash another path separators (use the canonical one, not the one actually used).
            stash += POV_PATH_SEPARATOR;
            ++i;

            // Stash everything that follows, up to the next path separator.
            for (; (i != path.end()) && !POV_IS_PATH_SEPARATOR(*i); ++i)
                stash += *i;

            // Currently, we don't support bare UNC share names without trailing separator character,
            // even though allegedly they are technically valid.
            if (i == path.end())
                return false;

            // Stash another path separator (use the canonical one, not the one actually used)
            // to indicate an absolute path.
            stash += POV_PATH_SEPARATOR;
            ++i;
        }
        // If it's not a UNC path, at this point our stash contains a single path separator,
        // which is exactly what we intend to emit as the volume identifier in that case.

        // Emit whatever string we have stashed as the volume identifier.
        volume = stash;
        stash.clear();
    }
    else if (isalpha (*i))
    {
        // String starts with an ASCII letter; may be a relative path or a drive letter.

        // Stash the character, then go on to test what's next.
        stash += *i;
        ++i;

        if ((i != path.end()) && (*i == ':'))
        {
            // Yup, that's a drive letter. Add the colon to the stashed letter.
            stash += ':';
            ++i;

            // Currently, we don't support relative paths if a volume is specified.
            if ((i == path.end()) || !POV_IS_PATH_SEPARATOR(*i))
                return false;

            // Stash another path separator (use the canonical one, not the one actually used)
            // to indicate an absolute path.
            stash += POV_PATH_SEPARATOR;
            ++i;

            // Emit the stashed string as the volume identifier.
            volume = stash;
            stash.clear();
        }
        // If it's not a drive letter, at this point we have only stashed the first letter, but
        // our index still points to the second one so the following algorithm will take care of it.
    }

    // Walk through the path string, stashing any non-separator characters. Whenever we hit a separator
    // character, emit the stashed characters (if any) as a directory name and clear the stash.
    // Also, as we go along, resolve the special directory names `.` and `..` if possible.

    // NB since we do not emit "empty" directory names, any sequence of consecutive separator
    // characters is effectively treated as a single separator character.

    for(; i != path.end(); ++i)
    {
        if (POV_IS_PATH_SEPARATOR(*i))
        {
            if (!stash.empty())
            {
                if (!IsCurrentDir(stash))
                {
                    if (!dirnames.empty() && IsParentDir(stash) && !IsParentDir(dirnames.back()))
                        dirnames.pop_back();
                    else
                        dirnames.push_back (stash);
                }
                stash.clear();
            }
        }
        else
            stash += *i;
    }

    // Whatever is left in the stash is presumably the actual file name.

    // NB as a consequence of the algorithm chosen, any path name ending in a path separator
    // character will be emitted as a list of directories only, with the file name left empty.

    filename = stash;

    return true;
}

#endif // !POV_USE_DEFAULT_PATH_PARSER

//******************************************************************************

}
// end of namespace pov_base
