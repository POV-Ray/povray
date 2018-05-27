/*******************************************************************************
 * povray.cpp
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
 * $File: //depot/public/povray/3.x/source/backend/povray.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <cstdlib>

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"

#include "base/types.h"
#include "base/timer.h"
#include "base/povms.h"
#include "base/povmsgid.h"
#include "base/pov_err.h"
#include "base/platformbase.h"

#include "backend/povray.h"
#include "backend/control/renderbackend.h"
#include "backend/support/msgutil.h"
#include "backend/support/task.h"
#include "backend/texture/texture.h"

#ifndef DONT_SHOW_IMAGE_LIB_VERSIONS
	// these are needed for copyright notices and version numbers
	#ifndef LIBZ_MISSING
		#include <zlib.h>
	#endif
	#ifndef LIBPNG_MISSING
		#include <png.h>
	#endif
	#ifndef LIBJPEG_MISSING
		#include <jversion.h>
	#endif

	// Including tiffio.h causes the Windows compile to break. As all we need is the
	// version function, we just declare it here.
	#ifndef LIBTIFF_MISSING
		extern "C" const char* TIFFGetVersion(void);
	#endif
    #ifndef OPENEXR_MISSING
        #include <IlmBaseConfig.h>
        #include <OpenEXRConfig.h>
        // NOTE:
        //  Versions of OpenEXR and IlmImf prior to 1.7.1 do not seem to have a way to get the version number,
        //  nor do the official hard-coded Windows config headers.
        #ifndef ILMBASE_PACKAGE_STRING
            #define ILMBASE_PACKAGE_STRING "IlmBase"
        #endif
        #ifndef OPENEXR_PACKAGE_STRING
            #define OPENEXR_PACKAGE_STRING "OpenEXR"
        #endif
    #endif

	// get boost version number. it isn't an image library but there's little point
	// in creating an entire new classification for it right now.
	#include <boost/version.hpp>

#endif

#ifndef POV_VALIDATE_FRONTEND
	#define POV_VALIDATE_FRONTEND(msg) true
#endif

#ifndef ALTMAIN

#include "base/stringutilities.h"
#include "base/textstreambuffer.h"
#include "frontend/defaultplatformbase.h"
#include "frontend/processrenderoptions.h"
#include "frontend/renderfrontend.h"
#include "frontend/simplefrontend.h"
#include "frontend/imageprocessing.h"
#include "frontend/console.h"
#include "frontend/display.h"

#endif

// this must be the last file included
#include "base/povdebug.h"

/// Platform specific function interface self reference pointer
pov_base::PlatformBase *pov_base::PlatformBase::self = NULL;

namespace
{

using namespace pov;
using namespace pov_base;

/// primary developers
const char *PrimaryDevelopers[] =
{
	"Chris Cason",
	"Thorsten Froehlich",
	"Christoph Lipka",
	NULL
};

/// assisting developers
const char *AssistingDevelopers[] =
{
	"Nicolas Calimet",
	"Jerome Grimbert",
	"James Holsenback",
	"Christoph Hormann",
	"Nathan Kopp",
	"Juha Nieminen",
	NULL
};

/// contributing developers
const char *ContributingDevelopers[] =
{
	"Steve Anger",
	"Eric Barish",
	"Dieter Bayer",
	"David K. Buck",
	"Nicolas Calimet",
	"Chris Cason",
	"Aaron A. Collins",
	"Chris Dailey",
	"Steve Demlow",
	"Andreas Dilger",
	"Alexander Enzmann",
	"Dan Farmer",
	"Thorsten Froehlich",
	"Mark Gordon",
	"James Holsenback",
	"Christoph Hormann",
	"Mike Hough",
	"Chris Huff",
	"Kari Kivisalo",
	"Nathan Kopp",
	"Lutz Kretzschmar",
	"Christoph Lipka",
	"Jochen Lippert",
	"Pascal Massimino",
	"Jim McElhiney",
	"Douglas Muir",
	"Juha Nieminen",
	"Ron Parker",
	"Bill Pulver",
	"Eduard Schwan",
	"Wlodzimierz Skiba",
	"Robert Skinner",
	"Yvo Smellenbergh",
	"Zsolt Szalavari",
	"Scott Taylor",
	"Massimo Valentini",
	"Timothy Wegner",
	"Drew Wells",
	"Chris Young",
	NULL   // NULL flags the end of the list
};

/// POVMS context to receive messages from the frontend
volatile POVMSContext POV_RenderContext = NULL;

/// POVMS address of the currently connected frontend
volatile POVMSAddress POV_FrontendAddress = POVMSInvalidAddress;

/// Main POV-Ray thread that waits for messages from the frontend
boost::thread *POV_MainThread = NULL;

/// Flag to mark main POV-Ray thread for termination
volatile bool POV_TerminateMainThread = false;

/// Flag that indicates the main thread has terminated
volatile bool POV_MainThreadTerminated = false;

int ConnectToFrontend(POVMSObjectPtr, POVMSObjectPtr, int, void *);
int DisconnectFromFrontend(POVMSObjectPtr, POVMSObjectPtr, int, void *);
bool ValidateFrontendAddress(POVMSAddress);
void BuildInitInfo(POVMSObjectPtr);
void ExtractLibraryVersion(const char *str, char *buffer);
void ExitFunction();

/**
 *	This function represents the main POV-Ray render thread created
 *	by povray_init and terminated by povray_terminate.
 *	It inits POV-Ray memory management, message output, control input and
 *	other important global data structures. This thread represents POV-Ray
 *	and it is controlled only by POVMS messages with the exception of
 *	termination, which is handled by povray_terminate. Usually POVMS
 *	messages will be generated using the frontend classes provided.
 */
void MainThreadFunction(const boost::function0<void>& completion);

int ConnectToFrontend(POVMSObjectPtr msg, POVMSObjectPtr result, int, void *)
{
	if(POV_VALIDATE_FRONTEND(msg) == true)
	{
		if(POV_FrontendAddress == POVMSInvalidAddress)
		{
			if(POVMSMsg_GetSourceAddress(msg, const_cast<POVMSAddress *>(&POV_FrontendAddress)) == kNoErr)
			{
				BuildInitInfo(result);

				return POVMSMsg_SetMessageIdentifier(result, kPOVMsgIdent_Done);
			}
		}
	}

	(void)POVMSUtil_SetInt(msg, kPOVAttrib_ErrorNumber, kAuthorisationErr);

	return POVMSMsg_SetMessageIdentifier(result, kPOVMsgIdent_Failed);
}

int DisconnectFromFrontend(POVMSObjectPtr msg, POVMSObjectPtr result, int, void *)
{
	POVMSAddress addr = POVMSInvalidAddress;

	if(POVMSMsg_GetSourceAddress(msg, &addr) != kNoErr)
	{
		(void)POVMSUtil_SetInt(msg, kPOVAttrib_ErrorNumber, kAuthorisationErr);
		return POVMSMsg_SetMessageIdentifier(result, kPOVMsgIdent_Failed);
	}

	if(POV_FrontendAddress != addr)
	{
		(void)POVMSUtil_SetInt(msg, kPOVAttrib_ErrorNumber, kAuthorisationErr);
		return POVMSMsg_SetMessageIdentifier(result, kPOVMsgIdent_Failed);
	}

	POV_FrontendAddress = POVMSInvalidAddress;

	return POVMSMsg_SetMessageIdentifier(result, kPOVMsgIdent_Done);
}

bool ValidateFrontendAddress(POVMSAddress addr)
{
	return ((POV_FrontendAddress == addr) && (addr != POVMSInvalidAddress));
}

void BuildInitInfo(POVMSObjectPtr msg)
{
	POVMSAttributeList attrlist;
	POVMSAttribute attr;
	int err = kNoErr;
	char buffer[20];

	if(err == kNoErr)
		err = POVMSUtil_SetString(msg, kPOVAttrib_PlatformName, POVRAY_PLATFORM_NAME);
	if(err == kNoErr)
		err = POVMSUtil_SetFormatString(msg, kPOVAttrib_CoreVersion,
		                                "Persistence of Vision(tm) Ray Tracer Version %s%s", POV_RAY_VERSION, COMPILER_VER);
	if(err == kNoErr)
		err = POVMSUtil_SetString(msg, kPOVAttrib_EnglishText,
		                          DISTRIBUTION_MESSAGE_1 "\n" DISTRIBUTION_MESSAGE_2 "\n" DISTRIBUTION_MESSAGE_3
		                          "\nPOV-Ray is based on DKBTrace 2.12 by David K. Buck & Aaron A. Collins\n" POV_RAY_COPYRIGHT);
#if POV_RAY_IS_OFFICIAL == 1
	if(err == kNoErr)
		err = POVMSUtil_SetBool(msg, kPOVAttrib_Official, true);
#else
	if(err == kNoErr)
		err = POVMSUtil_SetBool(msg, kPOVAttrib_Official, false);
#endif

	if(err == kNoErr)
		err = POVMSAttrList_New(&attrlist);
	if(err == kNoErr)
	{
		for(int i = 0; PrimaryDevelopers[i] != NULL; i++)
		{
			err = POVMSAttr_New(&attr);
			if(err == kNoErr)
			{
				err = POVMSAttr_Set(&attr, kPOVMSType_CString, PrimaryDevelopers[i], (int) strlen(PrimaryDevelopers[i]) + 1);
				if(err == kNoErr)
					err = POVMSAttrList_Append(&attrlist, &attr);
				else
					err = POVMSAttr_Delete(&attr);
			}
		}
	}
	if(err == kNoErr)
		err = POVMSObject_Set(msg, &attrlist, kPOVAttrib_PrimaryDevs);

	if(err == kNoErr)
		err = POVMSAttrList_New(&attrlist);
	if(err == kNoErr)
	{
		for(int i = 0; AssistingDevelopers[i] != NULL; i++)
		{
			err = POVMSAttr_New(&attr);
			if(err == kNoErr)
			{
				err = POVMSAttr_Set(&attr, kPOVMSType_CString, AssistingDevelopers[i], (int) strlen(AssistingDevelopers[i]) + 1);
				if(err == kNoErr)
					err = POVMSAttrList_Append(&attrlist, &attr);
				else
					err = POVMSAttr_Delete(&attr);
			}
		}
	}
	if(err == kNoErr)
		err = POVMSObject_Set(msg, &attrlist, kPOVAttrib_AssistingDevs);

	if(err == kNoErr)
		err = POVMSAttrList_New(&attrlist);
	if(err == kNoErr)
	{
		for(int i = 0; ContributingDevelopers[i] != NULL; i++)
		{
			err = POVMSAttr_New(&attr);
			if(err == kNoErr)
			{
				err = POVMSAttr_Set(&attr, kPOVMSType_CString, ContributingDevelopers[i], (int) strlen(ContributingDevelopers[i]) + 1);
				if(err == kNoErr)
					err = POVMSAttrList_Append(&attrlist, &attr);
				else
					err = POVMSAttr_Delete(&attr);
			}
		}
	}
	if(err == kNoErr)
		err = POVMSObject_Set(msg, &attrlist, kPOVAttrib_ContributingDevs);

	if(err == kNoErr)
		err = POVMSAttrList_New(&attrlist);
#ifndef DONT_SHOW_IMAGE_LIB_VERSIONS

#ifndef LIBZ_MISSING
	// ZLib library version and copyright notice
	if(err == kNoErr)
	{
		err = POVMSAttr_New(&attr);
		if(err == kNoErr)
		{
			ExtractLibraryVersion(zlibVersion(), buffer);

			const char *tempstr = pov_tsprintf("ZLib %s, Copyright 1995-2012 Jean-loup Gailly and Mark Adler", buffer);

			err = POVMSAttr_Set(&attr, kPOVMSType_CString, reinterpret_cast<const void *>(tempstr), (int) strlen(tempstr) + 1);
			if(err == kNoErr)
				err = POVMSAttrList_Append(&attrlist, &attr);
			else
				err = POVMSAttr_Delete(&attr);
		}
	}
#endif  // LIBZ_MISSING

#ifndef LIBPNG_MISSING
	// LibPNG library version and copyright notice
	if(err == kNoErr)
	{
		err = POVMSAttr_New(&attr);
		if(err == kNoErr)
		{
			ExtractLibraryVersion(png_get_libpng_ver(NULL), buffer);

			// TODO FIXME - shouldn't we use png_get_copyright() instead of png_get_libpng_ver() and a hard-coded string?
			const char *tempstr = pov_tsprintf("LibPNG %s, Copyright 1998-2012 Glenn Randers-Pehrson", buffer);

			err = POVMSAttr_Set(&attr, kPOVMSType_CString, reinterpret_cast<const void *>(tempstr), (int) strlen(tempstr) + 1);
			if(err == kNoErr)
				err = POVMSAttrList_Append(&attrlist, &attr);
			else
				err = POVMSAttr_Delete(&attr);
		}
	}
#endif  // LIBPNG_MISSING

#ifndef LIBJPEG_MISSING
	// LibJPEG library version and copyright notice
	if(err == kNoErr)
	{
		err = POVMSAttr_New(&attr);
		if(err == kNoErr)
		{
			ExtractLibraryVersion(JVERSION, buffer);

			// TODO FIXME - shouldn't we use the JCOPYRIGHT string instead of hard-coding it here?
			const char *tempstr = pov_tsprintf("LibJPEG %s, Copyright 1991-2013 Thomas G. Lane, Guido Vollbeding", buffer);

			err = POVMSAttr_Set(&attr, kPOVMSType_CString, reinterpret_cast<const void *>(tempstr), (int) strlen(tempstr) + 1);
			if(err == kNoErr)
				err = POVMSAttrList_Append(&attrlist, &attr);
			else
				err = POVMSAttr_Delete(&attr);
		}
	}
#endif  // LIBJPEG_MISSING

#ifndef LIBTIFF_MISSING
	// LibTIFF library version and copyright notice
	if(err == kNoErr)
	{
		err = POVMSAttr_New(&attr);
		if(err == kNoErr)
		{
			ExtractLibraryVersion(TIFFGetVersion(), buffer);

			// TODO FIXME - shouldn't we use the complete TIFFGetVersion() string instead of extracting just the version number and hard-coding the copyright info here?
			const char *tempstr = pov_tsprintf("LibTIFF %s, Copyright 1988-1997 Sam Leffler, 1991-1997 SGI", buffer);

			err = POVMSAttr_Set(&attr, kPOVMSType_CString, reinterpret_cast<const void *>(tempstr), (int) strlen(tempstr) + 1);
			if(err == kNoErr)
				err = POVMSAttrList_Append(&attrlist, &attr);
			else
				err = POVMSAttr_Delete(&attr);
		}
	}
#endif  // LIBTIFF_MISSING

	// boost library version and copyright notice
	if(err == kNoErr)
	{
		err = POVMSAttr_New(&attr);
		if(err == kNoErr)
		{
			const char *tempstr = pov_tsprintf("Boost %d.%d, http://www.boost.org/",
				BOOST_VERSION / 100000,
				BOOST_VERSION / 100 % 1000);

			err = POVMSAttr_Set(&attr, kPOVMSType_CString, reinterpret_cast<const void *>(tempstr), (int) strlen(tempstr) + 1);
			if(err == kNoErr)
				err = POVMSAttrList_Append(&attrlist, &attr);
			else
				err = POVMSAttr_Delete(&attr);
		}
	}

#ifndef OPENEXR_MISSING
    // OpenEXR library version and copyright notice
	if(err == kNoErr)
	{
		err = POVMSAttr_New(&attr);
		if(err == kNoErr)
		{
            const char *tempstr = OPENEXR_PACKAGE_STRING " and " ILMBASE_PACKAGE_STRING ", Copyright (c) 2002-2011 Industrial Light & Magic.";
			err = POVMSAttr_Set(&attr, kPOVMSType_CString, reinterpret_cast<const void *>(tempstr), (int) strlen(tempstr) + 1);
			if(err == kNoErr)
				err = POVMSAttrList_Append(&attrlist, &attr);
			else
				err = POVMSAttr_Delete(&attr);
		}
	}
#endif  // OPENEXR_MISSING

#endif  // DONT_SHOW_IMAGE_LIB_VERSIONS
	if(err == kNoErr)
		err = POVMSObject_Set(msg, &attrlist, kPOVAttrib_ImageLibVersions);
}

void ExtractLibraryVersion(const char *str, char *buffer)
{
	int pos = 0;

	for(; *str != 0; str++)
	{
		if(isdigit(*str))
		{
			while(((isalnum(*str)) || (*str == '.')) && (pos < 10))
			{
				buffer[pos] = *str;
				str++;
				pos++;
			}
			break;
		}
	}

	buffer[pos] = 0;
}

void ExitFunction()
{
	if((POV_RenderContext != NULL) && (POV_FrontendAddress != POVMSInvalidAddress))
	{
		POVMSObject msg;
		int err = kNoErr;

		if(err == kNoErr)
			err = POVMSObject_New(&msg, kPOVMSType_WildCard);
		if(err == kNoErr)
			err = POVMSMsg_SetupMessage(&msg, kPOVMsgClass_BackendControl, kPOVMsgIdent_Failed);
		if(err == kNoErr)
			err = POVMSMsg_SetDestinationAddress(&msg, const_cast<POVMSAddress>(POV_FrontendAddress));
		if(err == kNoErr)
			err = POVMS_Send(POV_RenderContext, &msg, NULL, kPOVMSSendMode_NoReply);
		if(err != 0)
			(void)POVMS_ASSERT_OUTPUT("Sending backend termination notice failed!", __FILE__, __LINE__);
	}
}

void MainThreadFunction(const boost::function0<void>& threadExit)
{
	try
	{
		if(POVMS_OpenContext(const_cast<POVMSContext *>(&POV_RenderContext)) != kNoErr)
			(void)POVMS_ASSERT_OUTPUT("Opening POVMS context failed in main POV-Ray backend thread.", __FILE__, __LINE__);
		else
		{
			try
			{
				RenderBackend backend(POV_RenderContext, ValidateFrontendAddress);

				POV_MEM_INIT();

				if((POVMS_InstallReceiver((POVMSContext)POV_RenderContext, ConnectToFrontend, kPOVMsgClass_BackendControl, kPOVMsgIdent_InitInfo, NULL) != kNoErr) ||
					(POVMS_InstallReceiver((POVMSContext)POV_RenderContext, DisconnectFromFrontend, kPOVMsgClass_BackendControl, kPOVMsgIdent_Done, NULL) != kNoErr))
					(void)POVMS_ASSERT_OUTPUT("Installing POVMS receive handler functions failed in main POV-Ray backend thread.", __FILE__, __LINE__);

				while(POV_TerminateMainThread == false)
				{
					try
					{
						(void)POVMS_ProcessMessages((POVMSContext)POV_RenderContext, true, true);
					}
					catch (std::bad_alloc)
					{
						// we don't mind bad_alloc's so much; they will happen under some circumstances
						// (e.g. create scene failing due to memory exhaustion). we just continue under
						// those circumstances. if a POVMS send is waiting on a reply and didn't catch
						// the problem, it will just have to time out ...
					}
					catch(...)
					{
						(void)POVMS_ASSERT_OUTPUT("Unhandled exception in POVMS receive handler in main POV-Ray backend thread.", __FILE__, __LINE__);
					}

					boost::thread::yield();
				}

				// close_all(); // TODO FIXME - Remove this call! [trf]
				POV_MEM_RELEASE_ALL(); // TODO FIXME - Remove this call! [trf]

				// NOTE: It is important that 'backend' be destroyed in this block scope because
				// 'POVMS_CloseContext' will destroy backend's context too early otherwise!
			}
			catch(...)
			{
				(void)POVMS_ASSERT_OUTPUT("Unexpected fatal error in main POV-Ray backend thread.", __FILE__, __LINE__);
			}
		}
	}
	catch(...)
	{
		// no point in sending the exception up to the OS level. threadExit() will advise the frontend that
		// we've had a problem.
	}

	// call the thread exit callback before we destroy the queue. this is important, because if we don't
	// let the frontend know the queue is being deleted, it could try to send a message to it.
	threadExit();

	POVMSContext tempcontext = (POVMSContext)POV_RenderContext;
	POV_RenderContext = NULL;
	(void)POVMS_CloseContext(tempcontext);
}

}

boost::thread *povray_init(const boost::function0<void>& threadExit, POVMSAddress *addr)
{
	using namespace pov;

	if(POV_MainThread == NULL)
	{
		POV_TerminateMainThread = false;
		POV_MainThreadTerminated = false;

		Initialize_Noise();
		pov::InitializePatternGenerators();

		POV_MainThread = Task::NewBoostThread(boost::bind(&MainThreadFunction, threadExit), POV_THREAD_STACK_SIZE);

		// we can't depend on boost::thread::yield here since under windows it is not
		// guaranteed to give up a time slice [see API docs for Sleep(0)]
		while(POV_RenderContext == NULL)
		{
			boost::thread::yield();
			pov_base::Delay(50);
		}
	}

	if(addr != NULL)
	{
		int err = POVMS_GetContextAddress(POV_RenderContext, addr);
		if(err != kNoErr)
			throw POV_EXCEPTION_CODE(err);
	}

	return POV_MainThread;
}

bool povray_terminated()
{
	return POV_MainThreadTerminated;
}

void povray_terminate()
{
	using namespace pov;

	POV_TerminateMainThread = true;

	while(POV_RenderContext != NULL)
	{
		boost::thread::yield();
		pov_base::Delay(100);
	}

	if(POV_MainThread != NULL)
		POV_MainThread->join();

	delete POV_MainThread;
	POV_MainThread = NULL;

	Free_Noise_Tables(); // TODO FIXME - don't add such calls here!
}

#ifndef ALTMAIN

class DefaultConsole : public pov_frontend::Console
{
	public:
		DefaultConsole() { }
		~DefaultConsole() { }
		void Initialise() { }
		void Output(const string& str) { std::printf("%s\n", str.c_str()); std::fflush(stdout); }
};

class DefaultDisplay : public pov_frontend::Display
{
	public:
		DefaultDisplay(unsigned int w, unsigned int h, COLC g) : Display(w, h, g) { }
		~DefaultDisplay() { }
		void Initialise() { }
		void DrawPixel(unsigned int, unsigned int, const RGBA8&) { }
};

pov_frontend::Console *CreateDefaultConsole();
pov_frontend::Display *CreateDefaultDisplay(unsigned int w, unsigned int h, COLC gf);

pov_frontend::Console *CreateDefaultConsole()
{
	return new DefaultConsole();
}

pov_frontend::Display *CreateDefaultDisplay(unsigned int w, unsigned int h, COLC gf)
{
	return new DefaultDisplay(w, h, gf);
}

void BackendExitCallback()
{
}

// NOTE: this code hasn't been tested in some time as all current official POV-Ray
// implementations have their own main and frontend code - so YMMV.
int main(int argc, char **argv)
{
	using namespace pov_base;
	using namespace pov_frontend;

	POVMSContext frontendContext = NULL;

	DefaultPlatformBase platformbase;
	POVMSAddress backendAddress = POVMSInvalidAddress;
	int err = kNoErr;
	int ret = 0;
	int i = 0;

	printf("Welcome to POV-Ray 3.7 SMP!\n");
	fflush(stdout);

//	char *nargv[2];
//	nargv[0] = argv[0];
//	nargv[1] = "'/Volumes/Iron/Official POV-Ray/POV-Ray 3.7 Source/benchmark.ini'";
//	nargv[1] = "'/Volumes/Iron/Official POV-Ray/POV-Ray 3.7 Source/object7.ini'";
//	argc = 2;
//	argv = nargv;

	// Init
	povray_init(boost::bind(&BackendExitCallback), &backendAddress);

	if(err == kNoErr)
		err = POVMS_OpenContext(&frontendContext);
	if(err != kNoErr)
		(void)POVMS_ASSERT_OUTPUT("Creating POVMS output context failed.", "povray.cpp", 0);
	else
	{
		POVMS_Object backendMessage;
		SimpleFrontend<ParserMessageHandler, FileMessageHandler, RenderMessageHandler, ImageMessageHandler>
		               frontend(frontendContext, backendAddress, backendMessage,
		               boost::bind(CreateDefaultConsole), boost::bind(CreateDefaultDisplay, _1, _2, _3));

		// Print help screens
		if(argc == 1)
		{
			// TODO frontend.PrintHelpScreens();
			return 0;
		}
		else if(argc == 2)
		{
			if((pov_stricmp(argv[1], "-h") == 0) ||
			   (pov_stricmp(argv[1], "-?") == 0) ||
			   (pov_stricmp(argv[1], "--help") == 0) ||
			   (pov_stricmp(argv[1], "-help") == 0))
			{
				// TODO frontend.PrintHelpScreens();
				return 0;
			}
			else if(argv[1][0] == '-')
			{
				if(argv[1][1] == '?')
				{
					// TODO frontend.PrintUsage(argv[1][2] - '0');
					return 0;
				}
				else if(strlen(argv[1]) == 6)
				{
					if(((argv[1][1] == 'h') || (argv[1][1] == 'H')) &&
					   ((argv[1][2] == 'e') || (argv[1][2] == 'E')) &&
					   ((argv[1][3] == 'l') || (argv[1][3] == 'L')) &&
					   ((argv[1][4] == 'p') || (argv[1][4] == 'P')))
					{
						// TODO frontend.PrintUsage(argv[1][5] - '0');
						return 0;
					}
				}
			}
		}

		try
		{
			ProcessRenderOptions renderoptions;
			POVMSObject obj;
			int l = 0;

			err = POVMSObject_New(&obj, kPOVObjectClass_IniOptions);
			if(err != kNoErr)
				throw POV_EXCEPTION_CODE(err);

			for(i = 1 ;i < argc; i++)
			{
				if(pov_stricmp(argv[i], "-povms") != 0)
				{
					err = renderoptions.ParseString(argv[i], &obj, true);
					if(err != kNoErr)
						throw POV_EXCEPTION_CODE(err);
				}
			}

			if(POVMSUtil_GetUCS2StringLength(&obj, kPOVAttrib_CreateIni, &l) == kNoErr)
			{
				UCS2 *outputini = new UCS2[l];
				if(POVMSUtil_GetUCS2String(&obj, kPOVAttrib_CreateIni, outputini, &l) == kNoErr)
					renderoptions.WriteFile(UCS2toASCIIString(outputini).c_str(), &obj);
			}

			POVMS_Object optionsobj(obj);

			frontend.Start(optionsobj);

			while(frontend.Process() != pov_frontend::kReady)
			{
				while(POVMS_ProcessMessages((POVMSContext)frontendContext, true, true) == kFalseErr)
				{
					if(frontend.Process() == pov_frontend::kReady)
						break;
				}

				Delay(100);
			}
		}
		catch(pov_base::Exception& e)
		{
			fprintf(stderr, "%s\n Failed to render file!\n", e.what());
			return -1;
		}

		// NOTE: It is important that 'frontend' be destroyed in this block scope because
		// 'POVMS_CloseContext' will destroy its context too early otherwise!
	}

	// Finish
	povray_terminate();

	(void)POVMS_CloseContext(frontendContext);

	return ret;
}

#endif
