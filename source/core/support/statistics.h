//******************************************************************************
///
/// @file core/support/statistics.h
///
/// @todo   What's in here?
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

#ifndef POVRAY_CORE_STATISTICS_H
#define POVRAY_CORE_STATISTICS_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"
#include "core/support/statistics_fwd.h"

// C++ variants of C standard header files
#include <cstddef>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/support/statisticids.h"

namespace pov
{

//##############################################################################
///
/// @defgroup PovCoreSupportStatistics Render Statistics
/// @ingroup PovCore
///
/// @{

template <typename T>
class Counter final
{
    public:
        Counter() { value = 0; } // assumes for all types of T that 0 is a valid assignment
        virtual ~Counter() { }
        inline T operator+(T other) { return value + other; }
        inline T operator-(T other) { return value - other; }
        inline T operator++(int) { return value++; }
        inline T operator--(int) { return value--; }
        inline void operator+=(T other) { value += other; }
        inline void operator-=(T other) { value -= other; }
        inline const T operator=(T other) { value = other; return value; }
        inline operator T() const { return value; }
        bool SafeRead(unsigned int maxattempts, T *result) const;

    private:
        volatile T value;
};

template <typename T, int numElem>
class StatisticsBase final
{
    public:
        StatisticsBase() {}
        virtual ~StatisticsBase() {}

        inline Counter<T>& operator[](std::size_t idx) { return counters[idx]; }
        inline Counter<T> operator[](std::size_t idx) const { return counters[idx]; }

        void operator+=(const StatisticsBase& other);
        StatisticsBase operator+(const StatisticsBase& other);

        void clear();

    private:
        Counter<T> counters[numElem];
};

typedef StatisticsBase<POV_ULONG, MaxIntStat> IntStatistics;
typedef StatisticsBase<double, MaxFPStat> FPStatistics;

class RenderStatistics final
{
public:
    RenderStatistics() {}
    virtual ~RenderStatistics() {}

    inline Counter<POV_ULONG>& operator[](IntStatsIndex idx) { return intStats[idx]; }
    inline Counter<POV_ULONG> operator[](IntStatsIndex idx) const { return intStats[idx]; }
    inline Counter<double>& operator[](FPStatsIndex idx) { return fpStats[idx]; }
    inline Counter<double> operator[](FPStatsIndex idx) const { return fpStats[idx]; }
    inline operator IntStatistics&() { return intStats; }
    inline operator IntStatistics() const { return intStats; }
    inline operator FPStatistics&() { return fpStats; }
    inline operator FPStatistics() const { return fpStats; }
    inline void operator+=(const RenderStatistics& rhs) { intStats += rhs.intStats; fpStats += rhs.fpStats; }

protected:
    IntStatistics intStats;
    FPStatistics fpStats;
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_STATISTICS_H
