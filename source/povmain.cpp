//******************************************************************************
///
/// @file povmain.cpp
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

#include <boost/bind.hpp>

#include <cstdlib>

#include <string>

// configfrontend.h must always be the first POV file included in frontend sources (pulls in platform config)
#include "frontend/configfrontend.h"

// POV-Ray header files (base module)
#include "base/timer.h"

// POV-Ray header files (backend module)
#include "backend/povray.h"

// POV-Ray header files (frontend module)
#include "frontend/console.h"
#include "frontend/display.h"
#include "frontend/filemessagehandler.h"
#include "frontend/imagemessagehandler.h"
#include "frontend/parsermessagehandler.h"
#include "frontend/processrenderoptions.h"
#include "frontend/rendermessagehandler.h"
#include "frontend/simplefrontend.h"

// this must be the last file included
#include "base/povdebug.h"

#ifndef ALTMAIN

class DefaultConsole : public pov_frontend::Console
{
    public:
        DefaultConsole() { }
        virtual ~DefaultConsole() override { }
        virtual void Initialise() override { }
        virtual void Output (const string& str) override { std::printf("%s\n", str.c_str()); std::fflush(stdout); }
};

class DefaultDisplay : public pov_frontend::Display
{
    public:
        DefaultDisplay(unsigned int w, unsigned int h) : Display(w, h) { }
        virtual ~DefaultDisplay() override { }
        virtual void Initialise() override { }
        virtual void DrawPixel (unsigned int, unsigned int, const RGBA8&) override { }
};

pov_frontend::Console *CreateDefaultConsole();
pov_frontend::Display *CreateDefaultDisplay(unsigned int w, unsigned int h);

pov_frontend::Console *CreateDefaultConsole()
{
    return new DefaultConsole();
}

pov_frontend::Display *CreateDefaultDisplay(unsigned int w, unsigned int h)
{
    return new DefaultDisplay(w, h);
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

    POVMSContext frontendContext = nullptr;

    DefaultPlatformBase platformbase;
    POVMSAddress backendAddress = POVMSInvalidAddress;
    int err = kNoErr;
    int ret = 0;
    int i = 0;

    printf("Welcome to POV-Ray v" POV_RAY_GENERATION " SMP!\n");
    fflush(stdout);

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
                       boost::bind(CreateDefaultConsole), boost::bind(CreateDefaultDisplay, _1, _2));

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
                    renderoptions.WriteFile(UCS2toSysString(outputini).c_str(), &obj);
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

#endif // ALTMAIN
