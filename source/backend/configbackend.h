//******************************************************************************
///
/// @file backend/configbackend.h
///
/// This header file defines all types that can be configured by platform
/// specific code for backend use. It further allows insertion of platform
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

#ifndef POVRAY_BACKEND_CONFIGBACKEND_H
#define POVRAY_BACKEND_CONFIGBACKEND_H

// Pull in other compile-time config header files first
#include "base/configbase.h"
#include "syspovconfigbackend.h"

//##############################################################################
///
/// @defgroup PovBackendConfig Back-End Compile-Time Configuration
/// @ingroup PovBackend
/// @ingroup PovConfig
///
/// @{

/// @def POVRAY_PLATFORM_NAME
/// Platform name string.
///
#ifndef POVRAY_PLATFORM_NAME
    #define POVRAY_PLATFORM_NAME "Unknown Platform"
#endif

/// @def POV_USE_DEFAULT_TASK_INITIALIZE
/// Whether to use a default implementation for task thread initialization.
///
/// Define as non-zero to use a default implementation for the @ref pov::Task::Initialize() method, or zero if the
/// platform provides its own implementation.
///
#ifndef POV_USE_DEFAULT_TASK_INITIALIZE
    #define POV_USE_DEFAULT_TASK_INITIALIZE 1
#endif

/// @def POV_USE_DEFAULT_TASK_CLEANUP
/// Whether to use a default implementation for task thread cleanup.
///
/// Define as non-zero to use a default implementation for the @ref pov::Task::Cleanup() method, or zero if the
/// platform provides its own implementation.
///
#ifndef POV_USE_DEFAULT_TASK_CLEANUP
    #define POV_USE_DEFAULT_TASK_CLEANUP 1
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
/// builds, in which case they will default to @ref POV_BACKEND_DEBUG unless noted otherwise.
///
/// @{

/// @def POV_BACKEND_DEBUG
/// Default setting for enabling or disabling @ref PovBackend debugging aids.
///
/// This setting specifies the default for all debugging switches throughout the entire module
/// that are not explicitly enabled or disabled by system-specific configurations.
///
/// If left undefined by system-specific configurations, this setting defaults to @ref POV_DEBUG.
///
#ifndef POV_BACKEND_DEBUG
    #define POV_BACKEND_DEBUG POV_DEBUG
#endif

/// @def POV_TASK_DEBUG
/// Enable run-time sanity checks for task handling.
///
/// Define as non-zero integer to enable, or zero to disable.
///
#ifndef POV_TASK_DEBUG
    #define POV_TASK_DEBUG POV_BACKEND_DEBUG
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

#if POV_BACKEND_DEBUG
    #define POV_BACKEND_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_BACKEND_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

#if POV_TASK_DEBUG
    #define POV_TASK_ASSERT(expr) POV_ASSERT_HARD(expr)
#else
    #define POV_TASK_ASSERT(expr) POV_ASSERT_DISABLE(expr)
#endif

/// @}
///
//******************************************************************************

/// @}
///
//##############################################################################

#endif // POVRAY_BACKEND_CONFIGBACKEND_H
