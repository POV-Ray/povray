//******************************************************************************
///
/// @file platform/unix/syspovconsole.cpp
///
/// Unix-specific implementation of the @ref pov_base::GetTerminalWidth() function
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#include "syspovconsole.h"

#if defined(HAVE_SYS_IOCTL_H)
#include <sys/ioctl.h>
#endif
#if !defined(GWINSZ_IN_SYS_IOCTL)
#include <termios.h>
#endif

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

//******************************************************************************

unsigned int GetTerminalWidth()
{
#if defined(TIOCGWINSZ) && defined(HAVE_IOCTL)
    struct winsize w;

    // the ioctl call returns non-zero in case of errors (terminal not ready or
    // function non supported)
    // further, some systems use to return zero even though the call wasn't
    // successful, and signal the error condition by filling the column number
    // with zero
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col != 0)
        return (unsigned int)w.ws_col;
#endif
    return 80;
}

}
