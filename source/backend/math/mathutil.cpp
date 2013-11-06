/*******************************************************************************
 * mathutil.cpp
 *
 * This module implements the utility functions for scalar math.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/math/mathutil.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#include <ctype.h>
#include <time.h>
#include <algorithm>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/math/mathutil.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

#ifdef NEED_INVHYP
DBL asinh(DBL x)
{
	return (x < 0 ? -1 : (x > 0 ? 1 : 0)) * log(fabs(x) + sqrt(1 + x * x));
}

DBL acosh(DBL x)
{
	if(x < 1.0)
		return 0;
	return log(x + sqrt(x * x - 1));
}

DBL atanh(DBL x)
{
	if(fabs(x) >= 1)
		return 0;
	return 0.5 * log((1 + x) / (1 - x));
}
#endif

}
