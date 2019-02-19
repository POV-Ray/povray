//******************************************************************************
///
/// @file backend/control/renderbackend.h
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

#ifndef POVRAY_BACKEND_RENDERBACKEND_H
#define POVRAY_BACKEND_RENDERBACKEND_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "backend/configbackend.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <map>
#include <memory>
#include <set>
#include <vector>

// POV-Ray header files (base module)
#include "base/stringtypes.h"

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"

// POV-Ray header files (backend module)
#include "backend/control/scene_fwd.h"
#include "backend/scene/view_fwd.h"

namespace pov
{

using namespace pov_base;

/**
 *  RenderBackend class receives render control messages from the
 *  RenderFrontend class.  Thee control messages are processed and
 *  parsing, rendering or other operations are performed as
 *  requested by the frontend.
 */
class RenderBackend final : public POVMS_MessageReceiver
{
    public:
        typedef POVMSInt SceneId;
        typedef POVMSInt ViewId;

        RenderBackend(POVMSContext ctx, bool (*val)(POVMSAddress));
        virtual ~RenderBackend() override;

        static void SendSceneOutput(SceneId sid, POVMSAddress addr, POVMSType ident, POVMS_Object& obj);
        static void SendViewOutput(ViewId vid, POVMSAddress addr, POVMSType ident, POVMS_Object& obj);

        static void SendFindFile(POVMSContext ctx, SceneId sid, POVMSAddress addr, const std::vector<UCS2String>& filenames, UCS2String& filename);
        static void SendReadFile(POVMSContext ctx, SceneId sid, POVMSAddress addr, const UCS2String& filename, UCS2String& localfile, UCS2String& fileurl);
        static void SendCreatedFile(POVMSContext ctx, SceneId sid, POVMSAddress addr, const UCS2String& filename);

        static void SendSuccessResult(POVMSAddress addr);
        static void SendFailedResult(int error, POVMSAddress addr);
        static void SendFailedResult(const pov_base::Exception& e, POVMSAddress addr);
        static void SendFailedResult(const char *str, POVMSAddress addr);

        static void SendSceneSuccessResult(SceneId sid, POVMSAddress addr);
        static void SendSceneFailedResult(SceneId sid, int error, POVMSAddress addr);
        static void SendSceneFailedResult(SceneId sid, const pov_base::Exception& e, POVMSAddress addr);
        static void SendSceneFailedResult(SceneId sid, const char *str, POVMSAddress addr);

        static void SendViewSuccessResult(ViewId vid, POVMSAddress addr);
        static void SendViewFailedResult(ViewId vid, int error, POVMSAddress addr);
        static void SendViewFailedResult(ViewId vid, const pov_base::Exception& e, POVMSAddress addr);
        static void SendViewFailedResult(ViewId vid, const char *str, POVMSAddress addr);
    protected:
        void CreateScene(POVMS_Message& msg, POVMS_Message& result, int mode);
        void CloseScene(POVMS_Message& msg, POVMS_Message& result, int mode);

        void CreateView(POVMS_Message& msg, POVMS_Message& result, int mode);
        void CloseView(POVMS_Message& msg, POVMS_Message& result, int mode);

        void StartParser(POVMS_Message& msg, POVMS_Message& result, int mode);
        void StopParser(POVMS_Message& msg, POVMS_Message& result, int mode);
        void PauseParser(POVMS_Message& msg, POVMS_Message& result, int mode);
        void ResumeParser(POVMS_Message& msg, POVMS_Message& result, int mode);

        void StartRender(POVMS_Message& msg, POVMS_Message& result, int mode);
        void StopRender(POVMS_Message& msg, POVMS_Message& result, int mode);
        void PauseRender(POVMS_Message& msg, POVMS_Message& result, int mode);
        void ResumeRender(POVMS_Message& msg, POVMS_Message& result, int mode);

        void ReadFile(POVMS_Message& msg, POVMS_Message& result, int mode);
    private:
        static POVMSContext context;

        bool (*validateFrontendAddress)(POVMSAddress);

        SceneId scenecounter;
        ViewId viewcounter;

        typedef std::set<ViewId>                            ViewIdSet;
        typedef std::map<SceneId, std::shared_ptr<Scene>>   SceneMap;
        typedef std::map<ViewId,  std::shared_ptr<View>>    ViewMap;
        typedef std::map<SceneId, ViewIdSet>                Scene2ViewsMap;
        typedef std::map<ViewId,  SceneId>                  View2SceneMap;

        SceneMap        scenes;
        ViewMap         views;
        Scene2ViewsMap  scene2views;
        View2SceneMap   view2scene;

        bool ValidateFrontendAddress(POVMSAddress addr, POVMS_Message& result);

        void MakeFailedResult(int error, POVMS_Message& result);
        void MakeFailedResult(const pov_base::Exception& e, POVMS_Message& result);
        void MakeFailedResult(const char *str, POVMS_Message& result);
        void MakeDoneResult(POVMS_Message& result);
};

}
// end of namespace pov

#endif // POVRAY_BACKEND_RENDERBACKEND_H
