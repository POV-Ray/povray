//******************************************************************************
///
/// @file core/support/statistics.cpp
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/support/statistics.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
// POV-Ray header files (core module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

const unsigned int kMaxReadAttempts = 10;

template <typename T>
bool Counter<T>::SafeRead(unsigned int maxattempts, T *result) const
{
    // Use pointers to the value because even though it is declared
    // as volatile, we cannot trust all compilers enough to not
    // optimize the accesses!
    const volatile T *p1 = &value;
    const volatile T *p2 = &value;

    do
    {
        T val1 = *p1;
        T val2 = *p2;

        if (val2 == val1)
        {
            *result = val2;
            return true;
        }
    } while (maxattempts--);

    return false;
}

template <typename T, int numElem>
void StatisticsBase<T, numElem>::operator+=(const StatisticsBase<T, numElem>& other)
{
    for (int i = 0; i < numElem; i++)
    {
        T temp;
        other.counters[i].SafeRead(kMaxReadAttempts, &temp);
        counters[i] += temp;
    }
}

template <typename T, int numElem>
StatisticsBase<T, numElem> StatisticsBase<T, numElem>::operator+(const StatisticsBase<T, numElem>& other)
{
    StatisticsBase<T, numElem> result(*this);

    result += other;
    return result;
}

template <typename T, int numElem>
void StatisticsBase<T, numElem>::clear()
{
    for (int i = 0; i < numElem; i++)
        counters[i] = 0; // assumes this is a valid operation for type T
}

template class StatisticsBase<POV_ULONG, MaxIntStat>;
template class StatisticsBase<double, MaxFPStat>;

}
// end of namespace pov
