//******************************************************************************
///
/// @file core/support/cracklecache.cpp
///
/// Implementation of the crackle pattern cache.
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/support/cracklecache.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
// POV-Ray header files (core module)
// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

CrackleCache::CrackleCache() :
    mPruneCounter(0)
{
    // Advise the unordered_map that we don't mind hash collisions.
    // While this is a very high load factor, the simple fact is that the cost of
    // allocating memory at render time (each insert into the table requires an alloc
    // as the container doesn't pre-emptively allocate, unlike e.g. std::vector) is
    // quite high, particularly when we have multiple threads contending for the heap
    // lock.
    mData.max_load_factor(50.0);
}

bool CrackleCache::Lookup(CrackleCacheEntryPtr& entry, const CrackleCellCoord& coord)
{
    // search for this hash value in the cache
    CrackleCacheData::iterator iter = mData.find(coord);
    bool found = (iter != mData.end());

    if (!found)
    {
        // Not in cache.

        // If cache is already excessively large; refrain from creating a new entry.
        // Having to re-calculate entries that would have been cache hits had we not
        // skipped on adding an entry is less expensive than chewing up immense amounts
        // of RAM and finally hitting the swapfile. unfortunately there's no good way
        // to tell how much memory is 'too much' for the cache, so we just use a hard-
        // coded number for now (ideally we should allow the user to configure this).
        // keep in mind that the cache memory usage is per-thread, so the more threads,
        // the more RAM. If we don't do the insert, `entry` will remain unchanged.
        if (mData.size() * sizeof(CrackleCacheData::value_type) >= 30 * 1024 * 1024)
            return false;

        // Generate a new cache entry.
        iter = mData.insert(mData.end(), CrackleCacheData::value_type(coord, CrackleCacheEntry()));
        entry = &iter->second;
        entry->lastUsed = mPruneCounter;
        return false;
    }
    else
    {
        entry = &iter->second;
        // NB: We're deliberately _not_ updating `entry->lastUsed` here, as that
        // seems to be counter-productive to performance.
        return true;
    }
}

void CrackleCache::Prune()
{
    CrackleCacheData::iterator it;

    // This serves as a kind of "clock" to track age of cache entries.
    ++mPruneCounter;

    // Probably we ought to have a means for the end-user to choose the preferred maximum bytes reserved for the cache.
    // For now, we have hard-coded values. we also do not discard any entries that are from the current block, even if
    // the cache size is exceeded. also, note that the cache size is per-thread. finally, don't forget that erasing
    // elements doesn't in and of itself return the freed memory to the heap.

    // Don't go through the hassle of pruning unless the cache has reached a certain size.
    if (mData.size() * sizeof(CrackleCacheData::value_type) < 15 * 1024 * 1024 / 64)
        return;

    // If we're pruning, aim for a somewhat lower size.
    // Traverse the map to determine the oldest age, then traverse it again to
    // throw away all entries with that age. Rinse, repeat.
    // TODO - This salami approach - cutting away one slice at a time - seems
    //        inefficient; instead, from the oldest age we should probably be
    //        able to guesstimate a cut-off age that will get us close to the
    //        desired cache size, and then throw away everything older than
    //        that in a single pass.
    while (mData.size() * sizeof(CrackleCacheData::value_type) > 10 * 1024 * 1024 / 64)
    {
        // search the cache for the oldest entries
        int oldest = std::numeric_limits<int>::max();
        for (it = mData.begin(); it != mData.end(); it++)
            if (it->second.lastUsed < oldest)
                oldest = (int)it->second.lastUsed;

        // don't remove any entries from the most recent block
        if (oldest == mPruneCounter)
            break;

        for (it = mData.begin(); it != mData.end(); )
        {
            if (it->second.lastUsed == oldest)
            {
                it = mData.erase(it);
                continue;
            }
            it++;
        }
    }
}

}
// end of namespace pov
