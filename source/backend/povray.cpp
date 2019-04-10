//******************************************************************************
///
/// @file backend/povray.cpp
///
/// @todo   What's in here?
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

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "backend/povray.h"

// C++ variants of C standard header files
#include <cstdlib>

// C++ standard header files
#include <string>

// Boost header files
#include <boost/bind.hpp>

// POV-Ray header files (base module)
#include "base/platformbase.h"
#include "base/pov_err.h"
#include "base/stringutilities.h"
#include "base/timer.h"
#include "base/types.h"

// POV-Ray header files (core module)
#include "core/material/noise.h"
#include "core/material/pattern.h"

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"
#include "povms/povmsid.h"
#include "povms/povmsutil.h"

// POV-Ray header files (backend module)
#include "backend/control/renderbackend.h"
#include "backend/support/task.h"

#ifdef POV_CPUINFO_H
#include POV_CPUINFO_H
#endif

#ifndef DONT_SHOW_IMAGE_LIB_VERSIONS
    // these are needed for copyright notices and version numbers
    #ifndef LIBZ_MISSING
        #include <zlib.h>
    #endif
    #ifndef LIBPNG_MISSING
        #include <png.h>
    #endif
    #ifndef LIBJPEG_MISSING
        #include <jpeglib.h>
        #ifndef JPEG_LIB_VERSION_MAJOR
            #define JPEG_LIB_VERSION_MAJOR (JPEG_LIB_VERSION / 10)
        #endif
        #ifndef JPEG_LIB_VERSION_MINOR
            // This is known to erroneously identify versions 8a and 8b as version 8,
            // but we'll live with that.
            #define JPEG_LIB_VERSION_MINOR (JPEG_LIB_VERSION % 10)
        #endif
    #endif
    #ifndef LIBTIFF_MISSING
        extern "C"
        {
            #ifndef __STDC__
            #define __STDC__        (1) // TODO - this is an ugly hack; check if it is really necessary
            #define UNDEF__STDC__
            #endif
            #ifndef AVOID_WIN32_FILEIO
            #define AVOID_WIN32_FILEIO // this stops the tiff headers from pulling in windows.h on win32/64
            #endif
            #include <tiffio.h>
            #ifdef UNDEF__STDC__
            #undef __STDC__
            #undef UNDEF__STDC__
            #endif
        }
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

// this must be the last file included
#include "base/povdebug.h"

#ifndef POV_VALIDATE_FRONTEND
    #define POV_VALIDATE_FRONTEND(msg) true
#endif

namespace
{

using namespace pov;
using namespace pov_base;

/// Primary Developers.
const char *PrimaryDevelopers[] =
{
    "Chris Cason",
    "Thorsten Froehlich",
    "Christoph Lipka",
    nullptr
};

/// Assisting Developers.
const char *AssistingDevelopers[] =
{
    "Nicolas Calimet",
    "Jerome Grimbert",
    "James Holsenback",
    "Christoph Hormann",
    "Nathan Kopp",
    "Juha Nieminen",
    "William F. Pokorny",
    nullptr
};

/// Past Contributing Developers.
/// By convention, current developers are also already included here.
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
    "Jerome Grimbert",
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
    "William F. Pokorny",
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
    nullptr   // `nullptr` flags the end of the list
};

/// POVMS context to receive messages from the frontend
volatile POVMSContext POV_RenderContext = nullptr;

/// POVMS address of the currently connected frontend
volatile POVMSAddress POV_FrontendAddress = POVMSInvalidAddress;

/// Main POV-Ray thread that waits for messages from the frontend
std::thread *POV_MainThread = nullptr;

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
 *  This function represents the main POV-Ray render thread created
 *  by povray_init and terminated by povray_terminate.
 *  It inits POV-Ray memory management, message output, control input and
 *  other important global data structures. This thread represents POV-Ray
 *  and it is controlled only by POVMS messages with the exception of
 *  termination, which is handled by povray_terminate. Usually POVMS
 *  messages will be generated using the frontend classes provided.
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
        err = POVMSUtil_SetString(msg, kPOVAttrib_CoreVersion,
                                        "Persistence of Vision(tm) Ray Tracer Version " POV_RAY_VERSION_INFO);
    if (err == kNoErr)
        err = POVMSUtil_SetString(msg, kPOVAttrib_CoreGeneration, "POV-Ray v" POV_RAY_GENERATION);
    if(err == kNoErr)
        err = POVMSUtil_SetString(msg, kPOVAttrib_EnglishText,
                                  DISTRIBUTION_MESSAGE_1 "\n" DISTRIBUTION_MESSAGE_2 "\n" DISTRIBUTION_MESSAGE_3
                                  "\nPOV-Ray is based on DKBTrace 2.12 by David K. Buck & Aaron A. Collins\n" POV_RAY_COPYRIGHT);
#if POV_RAY_IS_OFFICIAL
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
        for (int i = 0; PrimaryDevelopers[i] != nullptr; i++)
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
        for (int i = 0; AssistingDevelopers[i] != nullptr; i++)
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
        for (int i = 0; ContributingDevelopers[i] != nullptr; i++)
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
            ExtractLibraryVersion(png_get_libpng_ver(nullptr), buffer);

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
            const char minorStr[2] = { JPEG_LIB_VERSION_MINOR ? 'a'+JPEG_LIB_VERSION_MINOR-1 : '\0', '\0' };
            const char *tempstr = pov_tsprintf("LibJPEG %i%s, Copyright 1991-2016 Thomas G. Lane, Guido Vollbeding",
                                               JPEG_LIB_VERSION_MAJOR, minorStr);

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
    // OpenEXR and related libraries version and copyright notice
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

#ifdef POV_CPUINFO
    std::string cpuInfo(POV_CPUINFO);
    if (err == kNoErr)
        err = POVMSUtil_SetString(msg, kPOVAttrib_CPUInfo, cpuInfo.c_str());
#endif

#if POV_CPUINFO_DEBUG && defined(POV_CPUINFO_DETAILS)
    std::string cpuDetail(POV_CPUINFO_DETAILS);
    if (err == kNoErr)
        err = POVMSUtil_SetString(msg, kPOVAttrib_CPUInfoDetails, cpuDetail.c_str());
#endif

    if (err == kNoErr)
        err = POVMSAttrList_New(&attrlist);
    if (err == kNoErr)
    {
#ifdef TRY_OPTIMIZED_NOISE
        const OptimizedNoiseInfo* pNoise = GetRecommendedOptimizedNoise();
        std::string noiseGenInfo = "Noise generator: " + std::string(pNoise->name) + " (" + std::string(pNoise->info) + ")";
        err = POVMSAttr_New(&attr);
        if (err == kNoErr)
        {
            err = POVMSAttr_Set(&attr, kPOVMSType_CString, reinterpret_cast<const void *>(noiseGenInfo.c_str()), noiseGenInfo.length() + 1);
            if (err == kNoErr)
                err = POVMSAttrList_Append(&attrlist, &attr);
            else
                err = POVMSAttr_Delete(&attr);
        }
#endif
    }
    if (err == kNoErr)
        err = POVMSObject_Set(msg, &attrlist, kPOVAttrib_Optimizations);
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
    if ((POV_RenderContext != nullptr) && (POV_FrontendAddress != POVMSInvalidAddress))
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
            err = POVMS_Send(POV_RenderContext, &msg, nullptr, kPOVMSSendMode_NoReply);
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

                if ((POVMS_InstallReceiver((POVMSContext)POV_RenderContext, ConnectToFrontend, kPOVMsgClass_BackendControl, kPOVMsgIdent_InitInfo, nullptr) != kNoErr) ||
                    (POVMS_InstallReceiver((POVMSContext)POV_RenderContext, DisconnectFromFrontend, kPOVMsgClass_BackendControl, kPOVMsgIdent_Done, nullptr) != kNoErr))
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

                    std::this_thread::yield();
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
    POV_RenderContext = nullptr;
    (void)POVMS_CloseContext(tempcontext);
}

} // namespace

std::thread *povray_init(const boost::function0<void>& threadExit, POVMSAddress *addr)
{
    using namespace pov;

    if (POV_MainThread == nullptr)
    {
        POV_TerminateMainThread = false;
        POV_MainThreadTerminated = false;

        Initialize_Noise();
        pov::InitializePatternGenerators();

        POV_MainThread = new std::thread(boost::bind(&MainThreadFunction, threadExit));

        // We can't depend on `std::this_thread::yield()` here since it is not
        // guaranteed to give up a time slice.
        while (POV_RenderContext == nullptr)
        {
            std::this_thread::yield();
            pov_base::Delay(50);
        }
    }

    if (addr != nullptr)
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

    while (POV_RenderContext != nullptr)
    {
        std::this_thread::yield();
        pov_base::Delay(100);
    }

    if (POV_MainThread != nullptr)
        POV_MainThread->join();

    delete POV_MainThread;
    POV_MainThread = nullptr;

    Free_Noise_Tables(); // TODO FIXME - don't add such calls here!
}
