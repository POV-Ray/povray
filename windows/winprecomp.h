//******************************************************************************
///
/// @file windows/winprecomp.h
///
/// Precompiled header for the Windows-specific portions of POV-Ray.
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

/// @file
/// @note
/// @parblock
///     This header file is _not_ explicitly included in any source file (except @ref windows/winprecomp.cpp
///     which is designed to allow for precompiling this header in the first place). To use
///     precompiled headers, you will therefore have to make your build environment automatically
///     inject this header at the start of every source file.
///
///     The rationale behind this is to keep the set of headers included in each source file at an
///     absolute minimum when precompiled headers are _not_ used.
/// @endparblock

#include "base/configbase.h" // only pulled in for POV_MULTITHREADED

#define POVWIN_FILE
#define _CRT_RAND_S
#define _WIN32_IE COMMONCTRL_VERSION

// C++ variants of C standard header files
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdlib>

// C++ standard header files
#include <list>
#include <map>
#include <memory>
#include <new>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <direct.h>
#include <excpt.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <sys/stat.h>
#include <string.h>
#include <float.h>
#include <intrin.h>
#include <limits.h>
#include <malloc.h>

#include <windows.h>
#include <commctrl.h>
#include <htmlhelp.h>

// Boost header files
#include <boost/format.hpp>
