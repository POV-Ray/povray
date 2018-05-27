/*******************************************************************************
 * povray.h
 *
 * This file contains the interface to initialise and terminate all
 * POV-Ray threads. Beyond the functions in this file, no other
 * functions need to be called to run POV-Ray.
 * Rendering is controlled by the classes provided in the frontend
 * files.
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
 * $File: //depot/public/povray/3.x/source/backend/povray.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef POVRAY_BACKEND_POVRAY_H
#define POVRAY_BACKEND_POVRAY_H

// Please put everything that isn't a preprocessor directive in this
// file into SKIP_COMPLEX_OPTOUT_H sections like the one below! [trf]
#ifndef SKIP_COMPLEX_OPTOUT_H

#include "base/povms.h"
#include <boost/thread.hpp>
#include <boost/function.hpp>

/**
 *	This function does essential initialisation that is required before
 *	POV-Ray can be used. It also starts the main render thread that
 *	receives and processes all messages received from the frontend.
 *	@param  addr  If not NULL, backend address on return.
 *  @return       Pointer to the thread resource created.
 */
boost::thread *povray_init(const boost::function0<void>& threadExit, POVMSAddress *addr = NULL);

/**
 *	This function shuts down the main render thread and after it has
 *	been called, all memory allocated by POV-Ray has been freed and
 *	all threads created by POV-Ray have been terminated.
 */
void povray_terminate();

/**
 *	Returns true if the main thread has terminated. It will return
 *	false if the main thread is not running because it has not yet
 *	been started.
 */
bool povray_terminated();

#endif // SKIP_COMPLEX_OPTOUT_H

#define DAYS(n)         (86400 * n)

// POV-Ray version and copyright message macros

#if POV_RAY_IS_OFFICIAL == 1
#define POV_RAY_VERSION "3.7.0.8"
#else
#define POV_RAY_VERSION "3.7.0.8.unofficial"
#endif

#define POV_RAY_COPYRIGHT "Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd."
#define OFFICIAL_VERSION_NUMBER 370
#define OFFICIAL_VERSION_NUMBER_HEX 0x0370

#if POV_RAY_IS_OFFICIAL == 1

#ifdef DISTRIBUTION_MESSAGE_2
#undef DISTRIBUTION_MESSAGE_2
#endif

#define DISTRIBUTION_MESSAGE_1 "This is an official version prepared by the POV-Ray Team. See the"
#define DISTRIBUTION_MESSAGE_2 " documentation on how to contact the authors or visit us on the"
#define DISTRIBUTION_MESSAGE_3 " internet at http://www.povray.org/\n"

#else

// Please set DISTRIBUTION_MESSAGE_2 to your real name to make unofficial versions distinguishable from each other.
// We also recommend including an email or website address, then remove the #error directive to proceed with the build.
#define DISTRIBUTION_MESSAGE_1 "This is an unofficial version compiled by:"
#ifndef DISTRIBUTION_MESSAGE_2
#error Please complete the following DISTRIBUTION_MESSAGE_2 definition
#define DISTRIBUTION_MESSAGE_2 " FILL IN NAME HERE........................."
#endif
#define DISTRIBUTION_MESSAGE_3 " The POV-Ray Team is not responsible for supporting this version.\n"

#endif

#define DISCLAIMER_MESSAGE_1 "This is free software; see the source for copying conditions.  There is NO"
#define DISCLAIMER_MESSAGE_2 "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."

#endif // POVRAY_BACKEND_POVRAY_H
