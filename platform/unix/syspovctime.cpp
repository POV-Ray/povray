//******************************************************************************
///
/// @file platform/unix/syspovctime.cpp
///
/// Unix-specific implementation of `<ctime>`-related functions.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2021 Persistence of Vision Raytracer Pty. Ltd.
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
//------------------------------------------------------------------------------
// SPDX-License-Identifier: AGPL-3.0-or-later
//******************************************************************************

// Make sure the standard headers expose the required additional functionality
// not defined in the C++ standard.
#define _DEFAULT_SOURCE 1 // potentially needed for `timegm()` (if available at all) in glibc
#define _POSIX_C_SOURCE 1 // potentially needed for `gmtime_r()` and `localtime_r()` in glibc

#include "syspovctime.h"

// Standard C header files
#include <time.h> // Deliberately including this as opposed to `<ctime>`

// C++ variants of standard C header files
#include <cassert>

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

static void tzset_once()
{
    static bool alreadyRun = false;
    if (alreadyRun) return;
    alreadyRun = true;
    tzset();
}

tm* safe_gmtime(const time_t* ts, tm* buf)
{
    // Any POSIX- or SUSv2-compliant system should have `gmtime_r()`,
    // as should any system compliant with C11 (NB that's not C++11).
    // Should you find your system not supporting this function,
    // we would like to know about this so we can improve portability.
    return gmtime_r(ts, buf);
}

tm* safe_localtime(const time_t* ts, tm* buf)
{
    // Make sure tzset() has been run (in contrast to `std::localtime()`,
    // `localtime_r()` is not required to do this).
    tzset_once();
    // Any POSIX- or SUSv2-compliant system should have `localtime_r()`,
    // as should any system compliant with C11 (NB that's not C++11).
    // Should you find your system not supporting this function,
    // we would like to know about this so we can improve portability.
    return localtime_r(ts, buf);
}

#if defined(HAVE_TIMEGM)

time_t mktime_utc(tm* t)
{
    // GNU/BSD extension.
    return timegm(t);
}

#else // HAVE_TIMEGM

// Verify that the specified `std::time_t` value corresponds to the specified date, 00:00 GMT
static bool checkTimeT(time_t date, int year, int month, int day)
{
    tm t;
    (void)safe_gmtime(&date, &t);
    return
        (t.tm_year + 1900 == year)  &&
        (t.tm_mon  +    1 == month) &&
        (t.tm_mday        == day)   &&
        (t.tm_hour        == 0)     &&
        (t.tm_min         == 0)     &&
        (t.tm_sec         == 0);
}

// Convert from Gregorian date to a linear day number.
// NB: The month must be normalized, i.e. in the range [1..12]. Out-of-range day number are
// allowed though, and interpreted as relative to the specified month.
// NB: The current implementation returns the so-called Modified Julian Day, assigning day 0 to
// the Gregorian date 1858-11-17. However, this should be considered an implementation detail.
// NB: The current implementation is based on a formula for the (regular) Julian Day that breaks
// down at around 4800 BCE or earlier.
static int_least32_t LinearDay(int year, int month, int day)
{
    POV_ASSERT((month >= 1) && (month <= 12));
    int_least32_t julianDay = (1461 * (year + 4800 + (month - 14) / 12)) / 4 +
                              (367 * (month - 2 - 12 * ((month - 14) / 12))) / 12 -
                              (3 * ((year + 4900 + (month - 14) / 12) / 100)) / 4 +
                              day - 32075;
    return julianDay - 2400000;
}

// Implementation of `mktime_utc()` relying on POSIX-compliant `std::time_t`.
// This implementation uses direct math on `std::time_t`.
// NB: This function breaks down at around 4800 BCE or earlier.
time_t mktime_utc_impl_posix(tm* t)
{
    // To convert the given date into a linear day value,
    // we must first normalize out-of-range month values.
    if (t->tm_mon < 0)
    {
        int borrow = (11 - t->tm_mon) / 12;
        t->tm_mon += borrow * 12;
        t->tm_year -= borrow;
    }
    else if (t->tm_mon >= 12)
    {
        int carry = t->tm_mon / 12;
        t->tm_mon -= carry * 12;
        t->tm_year += carry;
    }

    // Figure out the number of days between the start of the UNIX epoch and the date in question.
    static const auto epochLinearDay = LinearDay(1970, 1, 1);
    time_t result = LinearDay(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday) - epochLinearDay;

    // Adding time is trivial, even if the fields are non-normalized.
    result = result * 24 + t->tm_hour;
    result = result * 60 + t->tm_min;
    result = result * 60 + t->tm_sec;

    // To fully match the functionality of `std::mktime()`, normalize the input data.
    (void)safe_gmtime(&result, t);

    return result;
}

// Portable implementation of `mktime_utc()`.
// NB: On systems that use a strictly linear `std::time_t` keeping track of leap seconds,
// this function may be off by a second for any input calendar date and time that happens to
// fall within up to 24h (depending on locale) of any point in time when UTC is adjusted by such
// a leap second.
// NB: If at any time in the past the locale's time zone has ever been re-defined to use a
// different base offset from UTC, this function may be off by the corresponding difference for
// any input calendar date and time that happens to fall within up to 24h (depending on locale)
// of such an event. (The usual changes between DST and non-DST should not be a problem though.)
time_t mktime_utc_impl_fallback(tm* t)
{
    // As a first approximation, convert the date as if it was local (non-DST) time.
    // (The algorithm would also work if we chose DST, but we must be consistent.)
    t->tm_isdst = 0; // Make sure to explicitly choose non-DST.
    auto localTime = mktime(t);
    // Note that the resulting value is off by the local time zone offset during non-DST periods.
    // To determine how much that is in `std::time_t` units, convert the resulting value back,
    // but as UTC, then subject it to the same faulty conversion, and compute the difference to
    // the initial result.
    (void)safe_gmtime(&localTime, t);
    t->tm_isdst = 0; // Make sure to explicitly choose non-DST again, as above.
    auto timeOff = mktime(t) - localTime;
    // Finally, correct the first approximation by the determined error.
    auto result = localTime - timeOff;

    // To fully match the functionality of `std::mktime()`, normalize the input data.
    // (Also, we quite messed it up by now, so we'd want to reconstruct it anyway.)
    (void)safe_gmtime(&result, t);

    return result;
}

time_t mktime_utc_impl_detect(tm* t);

time_t(*mktime_utc_impl)(tm*) = &mktime_utc_impl_detect;

// Invoke one of our own implementations of `mktime_utc()`, depending on `std::time_t` clock style.
time_t mktime_utc_impl_detect(tm* t)
{
    time_t (*chosenImpl)(tm* t) = &mktime_utc_impl_fallback;

    // Try whether we can recognize the `std::time_t` clock style.
    if (checkTimeT(0, 1970, 1, 1))
    {
        // UNIX epoch, i.e. 1970-01-01 00:00 UTC.
        if (checkTimeT(946684800, 2000, 1, 1))
            // Genuine POSIX time, i.e. seconds elapsed since UNIX epoch, excluding leap seconds.
            chosenImpl = &mktime_utc_impl_posix;
        else if (checkTimeT(946684832, 2000, 1, 1))
            // TAI-based POSIX-style time, i.e. seconds elapsed since UNIX epoch, including leap seconds.
            chosenImpl = &mktime_utc_impl_fallback; // Can't do proper math on leap seconds.
    }
    mktime_utc_impl = chosenImpl;

    // Delegate to whatever implementation we've chosen.
    return mktime_utc_impl(t);
}

time_t mktime_utc(tm* t)
{
    // One of our own implementations.
    return mktime_utc_impl(t);
}

#endif // HAVE_TIMEGM

} // end of namespace pov_base
