//******************************************************************************
///
/// @file base/version.h
///
/// Primary version information.
///
/// This file serves as the primary source for version information pertaining to
/// the source code. It also defines a few macros providing this information in
/// alternative formats.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_BASE_VERSION_H
#define POVRAY_BASE_VERSION_H

/// @file
/// @note
///     This file _must not_ pull in any POV-Ray header whatsoever.

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

//******************************************************************************
///
/// @name Primary Version and Copyright Information
///
/// @note
///     The macro definition in this section may be probed by external tools, and must therefore
///     conform to the following rules:
///       - The definitions must reside on a single line each.
///       - The lines must not be disabled via conditional compilation or multi-line comments.
///       - The lines must not contain any whitespace other than plain blanks (ASCII 0x20).
///       - The macros must be defined as plain string literals, plain decimal integer literals,
///         or plain empty, depending on their purpose.
///
/// @{

/// Copyright string.
#define POV_RAY_COPYRIGHT           "Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd."

/// First numerical component of official source code version ("major version") as integer.
/// Increment this field (and set all subsequent fields to zero) to indicate a groundbreaking
/// change in architecture or behaviour.
#define POV_RAY_MAJOR_VERSION_INT   3

/// Second numerical component of official source code version ("minor version") as integer.
/// Increment this field (and set all subsequent fields to zero) to indicate significant
/// changes in the behaviour of existing features.
#define POV_RAY_MINOR_VERSION_INT   8

/// Third numerical component of official source code version ("revision") as integer.
/// Increment this field (and set all subsequent fields to zero) to indicate new features,
/// or complex bugfixes that require thorough testing.
#define POV_RAY_REVISION_INT        0

/// Fourth numerical component of official source code version ("maintenance patch level") as integer.
/// Increment this field to indicate simple bugfixes.
#define POV_RAY_PATCHLEVEL_INT      0

/// @def POV_RAY_PRERELEASE
/// Pre-release identifier.
/// Leave undefined for final releases or maintenance patches. Otherwise, define as one of the
/// following:
///   - `alpha` to denote an untagged development version;
///   - `alpha.TIME` to denote and identify a tagged development version;
///   - `beta.N` to denote and identify a beta release;
///   - `rc.N` to denote and identify a release candidates;
///   - `x.FEATURE` to denote an untagged experimental version;
///   - `x.FEATURE.N` to denote and identify a tagged experimental version;
/// where `N` is a serial number starting at 1 in each phase, `TIME` is the number of minutes
/// since 2000-01-01 00:00, and `FEATURE` is an arbitrary alphanumeric moniker for a particular
/// experimental feature.
#define POV_RAY_PRERELEASE          "alpha.9811560"

#if defined(DOXYGEN) && !defined(POV_RAY_PRERELEASE)
    // Work around doxygen being unable to document undefined macros.
    #define POV_RAY_PRERELEASE (undefined)
    #undef POV_RAY_PRERELEASE
#endif

/// @def POVRAY_IS_BETA
/// Whether this version is a beta.
/// Leave undefined for pre-beta, release candidate or final releases.
//#define POVRAY_IS_BETA

#if defined(DOXYGEN) && !defined(POVRAY_IS_BETA)
    // Work around doxygen being unable to document undefined macros.
    #define POVRAY_IS_BETA (undefined)
    #undef POVRAY_IS_BETA
#endif

/// @def POV_RAY_HOST_VERSION
/// Version of required host installation.
/// If defined, this macro overrides the version string used in directory and similar names on
/// platforms where development builds typically cannot run stand-alone, but require an existing
/// host installation to be dropped into. Define as the latest installable release's major and
/// minor version number for vX.Y.0.0 pre-beta versions, or leave undefined otherwise.
#define POV_RAY_HOST_VERSION        "3.7"

#if defined(DOXYGEN) && !defined(POV_RAY_HOST_VERSION)
    // Work around doxygen being unable to document undefined macros.
    #define POV_RAY_HOST_VERSION (undefined)
    #undef POV_RAY_HOST_VERSION
#endif

/// @}
///
//******************************************************************************
///
/// @name Secondary Version and Copyright Information
///
/// @{

/// Source code version as a 3-digit integer number.
#define POV_RAY_VERSION_INT (POV_RAY_MAJOR_VERSION_INT * 100 + POV_RAY_MINOR_VERSION_INT * 10 + POV_RAY_REVISION_INT)

/// Helper macro to convert a parameter into a string.
/// @note This macro can _not_ be used directly to stringify another macro's value, as it would
/// instead stringify the other macro's name.
#define POV_VERSION_STRINGIFY_RAW(V) #V

/// Helper macro to convert another macro's value into a string.
#define POV_VERSION_STRINGIFY(V) POV_VERSION_STRINGIFY_RAW(V)

/// First numerical component of official source code version as string.
#define POV_RAY_MAJOR_VERSION POV_VERSION_STRINGIFY(POV_RAY_MAJOR_VERSION_INT)

/// Second numerical component of official source code version as string.
#define POV_RAY_MINOR_VERSION POV_VERSION_STRINGIFY(POV_RAY_MINOR_VERSION_INT)

/// Third numerical component of official source code version as string.
#define POV_RAY_REVISION POV_VERSION_STRINGIFY(POV_RAY_REVISION_INT)

/// Fourth numerical component of official source code version as string.
#define POV_RAY_PATCHLEVEL POV_VERSION_STRINGIFY(POV_RAY_PATCHLEVEL_INT)

/// @def POV_RAY_PATCHLEVEL_SUFFIX
/// Fourth numerical component of official source code version as optional suffix string.
/// This macro is defined to an empty string if the component is zero, and includes a leading
/// separator dot (`.`) otherwise.
#if (POV_RAY_PATCHLEVEL_INT == 0)
    #define POV_RAY_PATCHLEVEL_SUFFIX ""
#else
    #define POV_RAY_PATCHLEVEL_SUFFIX "." POV_RAY_PATCHLEVEL
#endif

/// @def POV_RAY_PRERELEASE_SUFFIX
/// Pre-release identifier as a suffix string.
/// This macro is defined to an empty string if the pre-release identifier is undefined,
/// and includes a leading dash (`-`) otherwise.
#if defined(POV_RAY_PRERELEASE)
    #define POV_RAY_PRERELEASE_SUFFIX "-" POV_RAY_PRERELEASE
#else
    #define POV_RAY_PRERELEASE_SUFFIX ""
#endif

/// Official source code generation as string.
/// This macro evaluates to the first two numerical components of the source code version,
/// in the form `X.Y`, where `X` and `Y` are the major and minor version numbers respectively.
#define POV_RAY_GENERATION POV_RAY_MAJOR_VERSION "." POV_RAY_MINOR_VERSION

/// Final unpatched official source code version as string.
/// For pre-releases, this macro evaluates to the designated official source code version
/// envisioned for the final release. For final releases, this macro evaluates to the official
/// source code version. For maintenance patches, this macro evaluates to the source code version
/// of the original unpatched release. In all cases, the format is `X.Y.Z`, where `X` and `Y` are
/// the major and minor version numbers respectively, and `Z` is the revision.
#define POV_RAY_RELEASE_VERSION POV_RAY_GENERATION "." POV_RAY_REVISION

/// Final patched official source code version as string.
/// For pre-releases, this macro evaluates to the designated official source code version
/// envisioned for the final release. For final releases and maintenance patches, this macro
/// evaluates to the official source code version. The format is `X.Y.Z` for unpatched versions or
/// `X.Y.Z.P` for maintenance versions, where `X` and `Y` are the major and minor version numbers
/// respectively, `Z` is the revision, and `P` is the maintenance patch level.
#define POV_RAY_PATCH_VERSION POV_RAY_RELEASE_VERSION POV_RAY_PATCHLEVEL_SUFFIX

/// Official source code version as string.
/// This macro evaluates to the complete official source code version string. The format is
/// `X.Y.Z`[`.P`][`-PRE`], where `X` and `Y` are the major and minor version numbers respectively,
/// `Z` is the revision, `P` is the maintenance patch level, and `PRE` is the pre-release tag.
#define POV_RAY_FULL_VERSION POV_RAY_PATCH_VERSION POV_RAY_PRERELEASE_SUFFIX

/// @def POV_RAY_BETA_SUFFIX
/// Suffix to distinguish beta- from non-beta versions.
/// For beta releases, this macro evaluates to `-beta`. Otherwise, this macro evaluates to an
/// empty string.
#if defined(POVRAY_IS_BETA)
    #define POV_RAY_BETA_SUFFIX "-beta"
#else
    #define POV_RAY_BETA_SUFFIX ""
#endif

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_BASE_VERSION_H
