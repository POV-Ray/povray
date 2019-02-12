//******************************************************************************
///
/// @file core/support/cracklecache.h
///
/// Declarations related to crackle pattern cache.
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

#ifndef POVRAY_CORE_CRACKLECACHE_H
#define POVRAY_CORE_CRACKLECACHE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"
#include "core/support/cracklecache_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <unordered_map>

// Boost header files
#include <boost/functional/hash/hash.hpp>

// POV-Ray header files (base module)
#include "base/mathutil.h"

// POV-Ray header files (core module)
#include "core/math/vector.h"

namespace pov
{

//******************************************************************************

/// Helper class to implement the crackle cache.
class CrackleCellCoord final
{
public:

    CrackleCellCoord() : mX(0), mY(0), mZ(0), mRepeatX(0), mRepeatY(0), mRepeatZ(0) {}
    CrackleCellCoord(int x, int y, int z, int rx, int ry, int rz) : mX(x), mY(y), mZ(z), mRepeatX(rx), mRepeatY(ry), mRepeatZ(rz)
    {
        WrapCellCoordinate(mX, mRepeatX);
        WrapCellCoordinate(mY, mRepeatY);
        WrapCellCoordinate(mZ, mRepeatZ);
    }

    bool operator==(CrackleCellCoord const& other) const
    {
        return mX == other.mX && mY == other.mY && mZ == other.mZ;
    }

    /// Function to compute a hash value from the coordinates.
    ///
    /// @note       This function's name, as well as it being a global function rather than a member, is mandated by
    ///             std::unordered_map.
    ///
    /// @param[in]  coord   The coordinate.
    /// @return             The hash.
    ///
    friend std::size_t hash_value(CrackleCellCoord const& coord)
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, coord.mX);
        boost::hash_combine(seed, coord.mY);
        boost::hash_combine(seed, coord.mZ);

        return seed;
    }

protected:

    int mX;
    int mY;
    int mZ;
    int mRepeatX;
    int mRepeatY;
    int mRepeatZ;

    static inline void WrapCellCoordinate(int& v, int& repeat)
    {
        if (!repeat)
            return;
        v = pov_base::wrapInt(v, repeat);
        if ((v >= 2) && (v < repeat - 2))
            repeat = 0;
    }
};

//******************************************************************************

/// Helper class to implement the crackle cache.
struct CrackleCacheEntry final
{
    /// A kind of timestamp specifying when this particular entry was last used.
    size_t lastUsed;

    /// The pseudo-random points defining the pattern in this particular subset of 3D space.
    Vector3d aCellNuclei[81];
};
using CrackleCacheEntryPtr = CrackleCacheEntry*;

//******************************************************************************

/// Crackle cache.
///
/// This class buffers the pseudorandom "seeds points" for the Voronoi-based
/// crackle pattern.
///
class CrackleCache final
{
public:

    /// Construct a new crackle cache.
    CrackleCache();

    /// Look up cache entry.
    /// If the queried entry is not currently in the cache, an empty entry is
    /// automatically created, unless the cache has reached an excessive size.
    /// @param[out] entry   Pointer to cache entry, or unchanged if not in cache.
    /// @param[in]  coord   Crackle cell coordinates.
    /// @return             `true` if entry already existed, `false` otherwise.
    bool Lookup(CrackleCacheEntryPtr& entry, const CrackleCellCoord& coord);

    /// Prune the crackle cache.
    /// This function is intended to be called at regular intervals to remove
    /// the least recently used entries from the cache to keep it reasonably
    /// lean and fast.
    void Prune();

private:

    using CrackleCacheData = std::unordered_map<CrackleCellCoord, CrackleCacheEntry, boost::hash<CrackleCellCoord>>;
    CrackleCacheData mData;
    std::size_t mPruneCounter;
};

}
// end of namespace pov

#endif // POVRAY_CORE_CRACKLECACHE_H
