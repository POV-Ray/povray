//******************************************************************************
///
/// @file web/config/config.h
///
/// Hand-written replacement for the autoconf-generated `config.h`, used when
/// building POV-Ray for WebAssembly with Emscripten.
///
/// The Unix platform layer (`unix/povconfig`, `platform/unix`, `vfe/unix`) is
/// reused for the web build; this file provides the feature macros that
/// `./configure` would normally detect.
///
/// @author Christopher Swenson
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
/// @endparblock
///
//******************************************************************************

#ifndef POVRAY_WEB_CONFIG_H
#define POVRAY_WEB_CONFIG_H

// Package information.
#define PACKAGE             "povray"
#define PACKAGE_NAME        "POV-Ray"
#define PACKAGE_TARNAME     "povray"
#define PACKAGE_VERSION     "3.8.0-alpha"
#define PACKAGE_STRING      "POV-Ray 3.8.0-alpha"
#define PACKAGE_BUGREPORT   "POV-Ray issue tracker at https://github.com/POV-Ray/povray/issues"
#define PACKAGE_URL         ""
#define VERSION             "3.8.0-alpha"
#define VERSION_BASE        "3.8"

// Build information.
#ifndef BUILT_BY
    #define BUILT_BY        "unknown"
#endif
#define BUILD_ARCH          "wasm32-unknown-emscripten"
#define BUILT_FOR           "WebAssembly (Emscripten)"
#define COMPILER_VENDOR     "clang"
#define COMPILER_VERSION    "emscripten " __clang_version__
#define POV_COMPILER_INFO   COMPILER_VERSION " @ " BUILD_ARCH
#ifndef CXXFLAGS
    #define CXXFLAGS        ""
#endif

// Feature summary (printed by `--version`).
#define BUILTIN_IO_RESTRICTIONS "disabled (browser sandbox)"
#define BUILTIN_XWIN_DISPLAY    "disabled (HTML canvas preview)"
#define BUILTIN_IMG_FORMATS     "gif tga iff ppm pgm hdr png jpeg"
#define MISSING_IMG_FORMATS     "tiff openexr"

// Libraries.
#define USE_OFFICIAL_BOOST
#define LIBTIFF_MISSING
#define OPENEXR_MISSING
#define X_DISPLAY_MISSING 1

// The browser already sandboxes us; files only live in the in-memory file system.
#define IO_RESTRICTIONS_DISABLED 1

// C/C++ headers and functions provided by Emscripten's libc.
#define STDC_HEADERS 1
#define HAVE_CXX11 1
#define HAVE_CMATH 1
#define HAVE_LIMITS 1
#define HAVE_LIMITS_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_RESOURCE_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_TIME_H 1
#define HAVE_UNISTD_H 1
#define TIME_WITH_SYS_TIME 1

#define HAVE_PTHREAD 1
#define HAVE_GETCWD 1
#define HAVE_NANOSLEEP 1
#define HAVE_USLEEP 1
#define HAVE_USECONDS_T 1
#define HAVE_CLOCKID_T 1
#define HAVE_CLOCK_GETTIME 1
#define HAVE_GETTIMEOFDAY 1

// Emscripten implements only the wall clock reliably; this build treats the
// per-process and per-thread CPU time clocks as unavailable.
#define HAVE_DECL_CLOCK_MONOTONIC 1
#define HAVE_DECL_CLOCK_REALTIME 1
#define HAVE_DECL_CLOCK_PROCESS_CPUTIME_ID 0
#define HAVE_DECL_CLOCK_THREAD_CPUTIME_ID 0
#define HAVE_DECL_RUSAGE_SELF 0
#define HAVE_DECL_RUSAGE_THREAD 0
#define HAVE_DECL_RUSAGE_LWP 0

// Floating-point special values.
#define HAVE_NAN
#define HAVE_STD_ISNAN
#define HAVE_INF
#define HAVE_STD_ISINF

// libc++ (LLVM 19 and later) has no generic std::char_traits, so
// std::basic_string<unsigned short> no longer compiles; release/v3.8.0's
// default UCS2/UTF16 type is POV_UINT16. Use char16_t, as master does.
#define UCS2  char16_t
#define UTF16 char16_t

// Type sizes for wasm32.
#define SIZEOF_FLOAT 4
#define SIZEOF_INT 4
#define SIZEOF_LONG_INT 4
#define SIZEOF_SIZE_T 4

#endif // POVRAY_WEB_CONFIG_H
