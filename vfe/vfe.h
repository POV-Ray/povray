//******************************************************************************
///
/// @file vfe/vfe.h
///
/// @todo   What's in here?
///
/// @author Christopher J. Cason
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

#ifndef POVRAY_VFE_VFE_H
#define POVRAY_VFE_VFE_H

#include <memory>
#include <vector>

#include "base/platformbase.h"
#include "base/stringtypes.h"
#include "base/timer.h"

#include "povms/povmscpp.h"
#include "povms/povmsid.h"

#include "frontend/console.h"
#include "frontend/display.h"
#include "frontend/filemessagehandler.h"
#include "frontend/imagemessagehandler.h"
#include "frontend/parsermessagehandler.h"
#include "frontend/processrenderoptions.h"
#include "frontend/rendermessagehandler.h"
#include "frontend/renderfrontend.h"

#include "syspovconfigfrontend.h"
#include "vfeplatform.h"
#include "vfepovms.h"
#include "vfesession.h"

namespace vfe
{
  using namespace pov_frontend;

  class vfeException : public std::runtime_error
  {
    public:
      vfeException() : std::runtime_error("") {}
      vfeException(const std::string str) : std::runtime_error(str) {}
      virtual ~vfeException() throw() override {}
  } ;

  class vfeCriticalError : public vfeException
  {
    public:
      vfeCriticalError() : m_Line(0), vfeException() {}
      vfeCriticalError(const std::string str) : m_Line(0), vfeException(str) {}
      vfeCriticalError(const std::string str, const std::string filename, int line) :
        vfeException(str), m_Filename(filename), m_Line(line) {}
      virtual ~vfeCriticalError() throw() override {}

      const std::string Filename() { return m_Filename; }
      int Line() { return m_Line; }

      const std::string m_Filename;
      const int m_Line;
  };

  class vfeInvalidDataError : public vfeCriticalError
  {
    public:
      vfeInvalidDataError(const std::string str) : vfeCriticalError(str) {}
      virtual ~vfeInvalidDataError() throw() override {}
  };

  class vfeConsole : public Console
  {
    public:
      vfeConsole(vfeSession *session, int width = -1);
      virtual ~vfeConsole() override;

      virtual void Initialise() override;
      virtual void Output(const std::string&) override;
      virtual void Output(const std::string&, vfeSession::MessageType mType);
      virtual void Output(const char *str, vfeSession::MessageType mType = vfeSession::mUnclassified);
      virtual void BufferOutput(const char *str, unsigned int chars = 0, vfeSession::MessageType mType = vfeSession::mUnclassified);

    protected:
      char buffer [8192];
      char rawBuffer [16384] ;

      vfeSession* m_Session;
  };

  class vfePlatformBase : public PlatformBase
  {
    public:
      vfePlatformBase();
      vfePlatformBase(vfeSession& session);
      virtual ~vfePlatformBase() override;

      virtual UCS2String GetTemporaryPath() override;
      virtual UCS2String CreateTemporaryFile() override;
      virtual void DeleteTemporaryFile(const UCS2String& name) override;
      virtual bool ReadFileFromURL(OStream *file, const UCS2String& url, const UCS2String& referrer = UCS2String()) override;
      virtual FILE* OpenLocalFile (const UCS2String& name, const char *mode) override;
      virtual bool AllowLocalFileAccess (const UCS2String& name, const unsigned int fileType, bool write) override;

    protected:
      vfeSession* m_Session;
  };

  class vfeParserMessageHandler : public ParserMessageHandler
  {
    public:
      vfeParserMessageHandler();
      virtual ~vfeParserMessageHandler() override;

    protected:
      virtual void Options(Console *, POVMS_Object&, bool) override;
      virtual void Statistics(Console *, POVMS_Object&, bool) override;
      virtual void Progress(Console *, POVMS_Object&, bool) override;
      virtual void Warning(Console *, POVMS_Object&, bool) override;
      virtual void Error(Console *, POVMS_Object&, bool) override;
      virtual void FatalError(Console *, POVMS_Object&, bool) override;
      virtual void DebugInfo(Console *, POVMS_Object&, bool) override;

      vfeSession* m_Session;
  };

  class vfeRenderMessageHandler : public RenderMessageHandler
  {
    public:
      vfeRenderMessageHandler();
      virtual ~vfeRenderMessageHandler() override;

    protected:
      virtual void Options(Console *, POVMS_Object&, bool) override;
      virtual void Statistics(Console *, POVMS_Object&, bool) override;
      virtual void Progress(Console *, POVMS_Object&, bool) override;
      virtual void Warning(Console *, POVMS_Object&, bool) override;
      virtual void Error(Console *, POVMS_Object&, bool) override;
      virtual void FatalError(Console *, POVMS_Object&, bool) override;

      vfeSession *m_Session;
  };

  class vfeProcessRenderOptions : public ProcessRenderOptions
  {
    public:
      vfeProcessRenderOptions(vfeSession *);
      virtual ~vfeProcessRenderOptions() override;

    protected:
      virtual int ReadSpecialOptionHandler(INI_Parser_Table *, char *, POVMSObjectPtr) override;
      virtual int ReadSpecialSwitchHandler(Cmd_Parser_Table *, char *, POVMSObjectPtr, bool) override;
      virtual int WriteSpecialOptionHandler(INI_Parser_Table *, POVMSObjectPtr, OTextStream *) override;
      virtual int ProcessUnknownString(char *, POVMSObjectPtr) override;
      virtual ITextStream *OpenFileForRead(const char *, POVMSObjectPtr) override;
      virtual OTextStream *OpenFileForWrite(const char *, POVMSObjectPtr) override;
      virtual void ParseError (const char *, ...) override;
      virtual void ParseErrorAt (ITextStream *, const char *, ...) override;
      virtual void WriteError (const char *, ...) override;

    protected:
      vfeSession* m_Session;
  };

  class vfeDisplay : public Display
  {
    public:
      vfeDisplay(unsigned int width, unsigned int height, vfeSession *session, bool visible = false);
      virtual ~vfeDisplay() override;

      virtual void Initialise() override;
      virtual void Close();
      virtual void Show();
      virtual void Hide();
      virtual void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour) override;
      virtual void DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour) override;
      virtual void DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour) override;
      virtual void DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour) override;
      virtual void Clear() override;

    protected:
      vfeSession *m_Session;
      std::vector<RGBA8> m_Pixels;
      bool m_VisibleOnCreation;
  };

  class VirtualFrontEnd
  {
    public:
      VirtualFrontEnd(vfeSession& session, POVMSContext ctx, POVMSAddress addr, POVMS_Object& msg, POVMS_Object *result, std::shared_ptr<Console>& console) ;
      virtual ~VirtualFrontEnd();

      virtual bool Start(POVMS_Object& opts);
      virtual bool Stop();
      virtual bool Pause();
      virtual bool Resume();
      virtual State Process();
      virtual State GetState() const { return state; }
      virtual void SetResultPointers(Console **cr, Image **ir, Display **dr);
      virtual bool IsPausable();
      virtual bool Paused();
      virtual bool PausePending() { return m_PauseRequested; }
      virtual std::shared_ptr<Display> GetDisplay() { return renderFrontend.GetDisplay(viewId); }

      // TODO: take care of any pending messages (e.g. a thread waiting on a blocking send)
      virtual void InvalidateBackend() { backendAddress = POVMSInvalidAddress; }

    protected:
      virtual Console *CreateConsole()
        { return new vfeConsole(m_Session, m_Session->GetConsoleWidth()); }
      virtual Display *CreateDisplay(unsigned int width, unsigned int height)
        { return m_Session->CreateDisplay(width, height) ; }
      bool HandleShelloutCancel();

      RenderFrontend<vfeParserMessageHandler,FileMessageHandler,vfeRenderMessageHandler,ImageMessageHandler> renderFrontend;
      POVMSAddress backendAddress;
      State state;
      POVMS_Object options;
      RenderFrontendBase::SceneId sceneId;
      RenderFrontendBase::ViewId viewId;
      std::shared_ptr<AnimationProcessing> animationProcessing ;
      std::shared_ptr<ImageProcessing> imageProcessing ;
      std::shared_ptr<ShelloutProcessing> shelloutProcessing;
      Console **consoleResult;
      Display **displayResult;
      vfeSession* m_Session;
      vfePlatformBase m_PlatformBase;
      bool m_PausedAfterFrame;
      bool m_PauseRequested;
      State m_PostPauseState;
  };
}
// end of namespace vfe

#endif // POVRAY_VFE_VFE_H
