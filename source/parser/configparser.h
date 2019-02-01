//******************************************************************************
///
/// @file parser/configparser.h
///
/// This header file defines all types that can be configured by platform
/// specific code for parser layer use. It further allows insertion of platform
/// specific function prototypes making use of those types.
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

#ifndef POVRAY_PARSER_CONFIGPARSER_H
#define POVRAY_PARSER_CONFIGPARSER_H

// Pull in other compile-time config header files first
#include "core/configcore.h"
#include "syspovconfigparser.h"

//##############################################################################
///
/// @defgroup PovParserConfig Parser Compile-Time Configuration
/// @ingroup PovParser
/// @ingroup PovConfig
/// Compile-Time Configuration of the @ref PovParser.
///
/// @{

/// @def POV_PARSER_EXPERIMENTAL_BRILLIANCE_OUT
/// Whether experimental second brilliance parameter should be enabled.
///
#ifndef POV_PARSER_EXPERIMENTAL_BRILLIANCE_OUT
    #define POV_PARSER_EXPERIMENTAL_BRILLIANCE_OUT 0
#endif

/// @def POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
/// Whether experimental Wavefront OBJ import should be enabled.
///
#ifndef POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
    #define POV_PARSER_EXPERIMENTAL_OBJ_IMPORT 0
#endif

/// @def POV_PARSER_MAX_CACHED_MACRO_SIZE
/// Default size limit for macros to be cached in memory.
///
#ifndef POV_PARSER_MAX_CACHED_MACRO_SIZE
    #define POV_PARSER_MAX_CACHED_MACRO_SIZE 65536
#endif

//******************************************************************************
///
/// @name Debug Settings.
///
/// The following settings enable or disable certain debugging aids, such as run-time sanity checks
/// or additional log output.
///
/// Unless noted otherwise, a non-zero integer will enable the respective debugging aids, while a
/// zero value will disable them.
///
/// It is recommended that system-specific configurations leave these settings undefined in release
/// builds, in which case they will default to @ref POV_DEBUG unless noted otherwise.
///
/// @{

/// @def POV_PARSER_DEBUG
/// Enable run-time sanity checks for the @ref PovParser.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_PARSER_DEBUG
    #define POV_PARSER_DEBUG POV_DEBUG
#endif

/// @}
///
//******************************************************************************
///
/// @name Non-Configurable Macros
///
/// The following macros are configured automatically at compile-time; they cannot be overridden by
/// system-specific configuration.
///
/// @{

/// @def POV_PARSER_ASSERT
/// Assert a condition that should hold true by design.
/// In debug builds, this macro evaluates the specified expression, and halts execution if the
/// expression does not hold true.
/// In release builds, this macro evaluates to an empty statement.
#if POV_PARSER_DEBUG
    #define POV_PARSER_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_PARSER_ASSERT(expr) POV_ASSERT_SOFT(expr) // POV_ASSERT_DISABLE(expr)
#endif

/// @def POV_PARSER_PANIC
/// Indicates code paths that should never be reached.
/// In debug builds, this macro halts execution.
/// In release builds, this macro throws an exception to allow the application to fail gracefully.
/// In static code analysis, this macro may be used to inform the analysis tool that the code branch
/// is expected to be dead.
#if POV_PARSER_DEBUG
    #define POV_PARSER_PANIC()      POV_ASSERT_HARD(false)
#else
    #define POV_PARSER_PANIC()      POV_ASSERT_SOFT(false)
#endif

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_PARSER_CONFIGPARSER_H
