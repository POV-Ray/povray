//******************************************************************************
///
/// @file povms/povmsutil.h
///
/// This module contains all defines, typedefs, and prototypes for
/// @ref povms/povmsutil.cpp.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POV_POVMSUTIL_H
#define POV_POVMSUTIL_H

#include "povms/povms.h"

namespace pov
{

int POVMSUtil_SetFormatString(POVMSObjectPtr object, POVMSType key, const char *format, ...); // Note: Strings may not contain \0 characters codes!

int Send_Progress(const char *statusString, int progressState);
int Send_ProgressUpdate(int progressState, int timeDiff = 1);

}

#endif // POV_POVMSUTIL_H
