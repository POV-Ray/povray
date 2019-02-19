//******************************************************************************
///
/// @file vfe/vfesession.h
///
/// This file contains declarations relating to the vfe session management
/// class.
///
/// @author: Christopher J. Cason
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

#ifndef POVRAY_VFE_VFESESSION_H
#define POVRAY_VFE_VFESESSION_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <boost/format.hpp>
#include <boost/function.hpp>

#include "base/stringutilities.h"

#include "frontend/simplefrontend.h"

namespace pov_frontend
{
  class Display;
}
// end of namespace pov_frontend

namespace vfe
{
  using namespace pov_frontend;
  using namespace pov_base;

  ////////////////////////////////////////////////////////////////////////////
  // class IOPath
  //
  // IOPaths are used to keep track of IO Restrictions. Each instance of the
  // class will contain information regarding a path and its attributes (e.g.
  // if it's recursive or not). The class by itself doesn't identify if the
  // path is a permitted or excluded one - this is determined by the collection
  // to which the instance belongs.
  class IOPath
  {
    public:
      // construct an IOPath given an existing Path and a recursion flag.
      IOPath(const Path& path, bool recursive) : m_Path(path), m_Recursive(recursive) {}

      // construct an IOPath given a std::string path and a recursion flag.
      IOPath(const std::string& path, bool recursive) : m_Path(Path(SysToUCS2String(path))), m_Recursive(recursive) {}

      // construct an IOPath given a UCS2String Path and a recursion flag.
      IOPath(const UCS2String& path, bool recursive) : m_Path(path), m_Recursive(recursive) {}
      virtual ~IOPath() {}

      // returns true if paths below the given path are considered to be included
      // in consideration of an IO restriction.
      bool IsRecursive() const { return m_Recursive; }

      // return the stored path as an instance of the pov_base::Path class.
      const Path& GetPath() const { return m_Path; }

    private:
      bool m_Recursive;
      Path m_Path;
  } ;

  // convenience typedefs.
  typedef std::vector<std::string> StringVector;
  typedef std::vector<UCS2String> UCS2StringVector;
  typedef std::vector<IOPath> IOPathVector;

  class vfeDisplay;
  class VirtualFrontEnd;

  ////////////////////////////////////////////////////////////////////////////
  // class vfeDestInfo
  //
  // Currently unused, destined to hold information regarding the destination
  // of a remote session attempt.
  class vfeDestInfo
  {
  } ;

  ////////////////////////////////////////////////////////////////////////////
  // class vfeAuthInfo
  //
  // Currently unused, destined to hold information regarding authentication
  // when making a remote session open request.
  class vfeAuthInfo
  {
  } ;

  ////////////////////////////////////////////////////////////////////////////
  // class vfeRenderOptions
  //
  // This class is used to hold information regarding a pending render request.
  // For example, the library paths, source file, output file, and really
  // anything else that could vary from render to render.
  class vfeRenderOptions
  {
    friend class vfeSession;

    public:
      // Construct an instance of vfeRenderOptions. the thread count defaults
      // to 2.
      vfeRenderOptions() : m_ThreadCount(2) {}
      virtual ~vfeRenderOptions() {}

      // Clear the set options. This includes library paths, INI files,
      // commands, the source file, the thread count (defaults back to 2),
      // and all other options set in the kPOVObjectClass_RenderOptions object.
      void Clear()
      {
        ClearLibraryPaths();
        ClearINIs();
        ClearCommands();
        m_SourceFile.clear();
        m_ThreadCount = 2;
        POVMSObject obj;
        POVMSObject_New (&obj, kPOVObjectClass_RenderOptions);
        m_Options = POVMS_Object(obj);
      }

      // Clear any library paths that have been added.
      void ClearLibraryPaths() { m_LibraryPaths.clear(); }

      // Add a library path given a std::string.
      void AddLibraryPath(const std::string& Path) { m_LibraryPaths.push_back(SysToUCS2String(Path)); }

      // Add a library path given a UCS2String.
      void AddLibraryPath(const UCS2String& Path) { m_LibraryPaths.push_back(Path); }

      // Get the current library paths as a vector of UCS2 strings.
      // The returned paths are a const reference.
      const UCS2StringVector& GetLibraryPaths() const { return m_LibraryPaths; }

      // Get the current library paths as a vector of UCS2 strings.
      // The returned paths are a copy of the internal paths.
      UCS2StringVector GetLibraryPaths() { return m_LibraryPaths; }

      // Set the source file to be parsed (must be an SDL file, INI not
      // permitted). The file is specified as a UCS2String full or relative
      // path, with optional extension.
      void SetSourceFile(const UCS2String& File) { m_SourceFile = File; }

      // Set the source file to be parsed (must be an SDL file, INI not
      // permitted). The file is specified as a std::string full or relative
      // path, with optional extension.
      void SetSourceFile(const std::string& File) { m_SourceFile = SysToUCS2String(File); }

      // Returns a const reference to the currently set source file (may be
      // an empty string). Return type is a const reference to a UCS2String.
      const UCS2String& GetSourceFile() const { return m_SourceFile; }

      // Clears the list of INI files.
      void ClearINIs() { m_IniFiles.clear() ; }

      // Adds the supplied std::string to the list of INI files to be read
      // prior to the start of the render. The files are processed in the
      // order in which they are added to this list.
      void AddINI(const std::string& File) { m_IniFiles.push_back(SysToUCS2String(File)); }

      // Adds the supplied UCS2String to the list of INI files to be read
      // prior to the start of the render. The files are processed in the
      // order in which they are added to this list.
      void AddINI(const UCS2String& File) { m_IniFiles.push_back(File); }

      // Returns a const UCS2StringVector reference containing the
      // list of INI files to be processed, in order first to last.
      const UCS2StringVector& GetINIs() const { return m_IniFiles; }

      // Returns a UCS2StringVector copy of the list of INI files to be
      // processed, in order first to last.
      UCS2StringVector GetINIs() { return m_IniFiles; }

      // Sets the number of threads to be used for rendering (and potentially
      // for bounding and similar tasks, if this is supported in the future).
      // Clipped to the range 1..512.
      void SetThreadCount(int Count) { m_ThreadCount = std::max(1, std::min(Count, 512)); }

      // Gets the number of threads currently set to be used for renders.
      // Defaults to 2.
      int GetThreadCount() const { return m_ThreadCount; }

      // Clears any commands set via the AddCommand() method.
      void ClearCommands() { m_Commands.clear(); }

      // Adds the supplied std::string to the list of commands to be handled
      // prior to the start of any render. The commands are processed in the
      // order in which they appear in this list. A 'command' in this context
      // is basically anything that could normally appear on the command-line
      // of a POV-Ray console compile.
      void AddCommand(const std::string& Command) { m_Commands.push_back(Command); }

      // Returns a const reference to a std::vector<std::string> containing the
      // current list of commands as added by AddCommand(), in order first
      // to last.
      const StringVector& GetCommands() const { return m_Commands; }

      // Returns a copy of the std::vector<std::string> which holds the current
      // list of commands as added by AddCommand(), in order first to last.
      StringVector GetCommands() { return m_Commands; }

      // Returns a non-const reference to the current POVMS_Object options
      // instance.
      POVMS_Object& GetOptions() { return m_Options; }

    protected:
      int m_ThreadCount;
      UCS2StringVector m_IniFiles;
      UCS2StringVector m_LibraryPaths;
      StringVector m_Commands;
      UCS2String m_SourceFile;
      POVMS_Object m_Options;
  } ;

  // The integer values which may be returned from any of the VFE methods
  // which return error codes.
  enum
  {
    vfeNoError = 0,
    vfeNoInputFile = 1024,                        // "No input file provided"
    vfeRenderBlockSizeTooSmall,                   // "Specified block size is too small"
    vfeFailedToWriteINI,                          // "Failed to write output INI file"
    vfeFailedToSetSource,                         // "Failed to set source file"
    vfeFailedToParseINI,                          // "Failed to parse INI file"
    vfeIORestrictionDeny,                         // "I/O Restrictions prohibit access to file"
    vfeFailedToParseCommand,                      // "Failed to parse command-line option"
    vfeFailedToSetMaxThreads,                     // "Failed to set number of render threads"
    vfeFailedToSendRenderStart,                   // "Failed to send render start request"
    vfeRenderOptionsNotSet,                       // "Render options not set, cannot start render"
    vfeAlreadyStopping,                           // "Renderer is already stopping"
    vfeNotRunning,                                // "Renderer is not running"
    vfeInvalidParameter,                          // "Something broke but we're not sure what"
    vfeSessionExists,                             // "Only one session at once permitted in this version"
    vfePOVMSInitFailed,                           // "Failed to initialize local messaging subsystem"
    vfeOpenContextFailed,                         // "Failed to open context with core messaging subsystem"
    vfeConnectFailed,                             // "Failed to connect to core messaging subsystem"
    vfeInitializeTimedOut,                        // "Timed out waiting for worker thread startup"
    vfeRequestTimedOut,                           // "Timed out waiting for request to be serviced"
    vfeFailedToInitObject,                        // "Failed to initialize internal options storage"
    vfeCaughtException,                           // "Caught exception of unexpected type"
    vfeCaughtCriticalError,                       // "Caught critical error"
    vfeDisplayGammaTooSmall,                      // "Specified display gamma is too small"
    vfeFileGammaTooSmall,                         // "Specified file gamma is too small"
    vfeUnsupportedOptionCombination,              // "Unsupported option combination"
  } ;

  // The status flags (and mask combinations of flags) which may be returned
  // from vfeSession::GetStatus(), or passed to vfeSession::SetEventMask().
  enum
  {
    stNone                    = 0x00000000,       // No status to report
    stClear                   = 0x00000001,       // vfeSession::Clear() has been called
    stReset                   = 0x00000002,       // vfeSession::Reset() has been called
    stFailed                  = 0x00000004,       // Render failed
    stSucceeded               = 0x00000008,       // Render succeeded
    stStatusMessagesCleared   = 0x00000010,       // vfeSession::ClearStatusMessages() called
    stOutputFilenameKnown     = 0x00000020,       // vfeSession::AdviseOutputFilename() called
    stRenderingAnimation      = 0x00000040,       // vfeSession::SetRenderingAnimation() called
    stAnimationFrameCompleted = 0x00000080,       // vfeSession::AdviseFrameCompleted() called
    stStreamMessage           = 0x00000100,       // One or more stream messages are available
    stErrorMessage            = 0x00000200,       // One or more error messages are available
    stWarningMessage          = 0x00000400,       // One or more warning messages are available
    stStatusMessage           = 0x00000800,       // One or more status messages are available
    stAnyMessage              = 0x00000f00,       // A mask of stStream, Error, Warning, and Status message flags.
    stAnimationStatus         = 0x00001000,       // An animation status update is available
    stBackendStateChanged     = 0x00002000,       // The state of the backend (reflected in pov_frontend::State) has changed
    stRenderStartup           = 0x00004000,       // The render engine has started up
    stRenderShutdown          = 0x00008000,       // The render engine has shut down
    stShutdown                = 0x10000000,       // The session is shutting down
    stCriticalError           = 0x20000000,       // A critical error (exception, POVMS memory alloc failure, etc) has occurred
    stNoIgnore                = 0x30000000,       // A mask containing the status flags which can't be masked off
    stNoClear                 = 0x30000000,       // A mask containing the status flags which aren't cleared after a call to vfeSession::GetStatus()
  } ;

  // Used internally to track pause/resume status.
  typedef enum
  {
    rqNoRequest               = 0,
    rqPauseRequest            = 1,
    rqResumeRequest           = 2
  } rqRequest;

  // The data type returned by vfeSession::GetStatus().
  typedef int vfeStatusFlags ;

  ////////////////////////////////////////////////////////////////////////////
  // class vfeSession (vfe is short for Virtual FrontEnd).
  //
  // This class is the core of VFE. Each instance of a vfeSession represents
  // a connection to the messaging subsystem of the POV-Ray backend, either
  // locally or (in the future) remotely. It has a worker thread to handle
  // communication with the backend asynchronously from the client (the
  // software which created the session instance), and provides status
  // feedback via either polling of the GetStatus() method, or via timed
  // waits (again via GetStatus(), but this time using a timeout and an
  // event notification system).
  //
  // Additionally vfeSession provides numerous convenience methods for
  // setting up renders, monitoring renders (including retrieving status
  // messages and render state), and associated maintenance such as the
  // setting and retrieval of IO restrictions, display management, and
  // critical event notifications.
  //
  // Some methods of vfeSession are pure virtual; it is required that each
  // platform derive its own version in order to provide platform-specific
  // functionality. Currently there are only six methods that must be provided
  // in this way, summarized below:
  //
  //   GetTemporaryPath()     // return the path to a temporary storage area
  //   CreateTemporaryFile()  // create a temporary filename (but don't open it)
  //   DeleteTemporaryFile()  // delete a temporary file previously created
  //   GetTimestamp()         // return a 64-bit one-millisecond resolution timestamp
  //   NotifyCriticalError()  // asynchronously notify user of a critical error
  //   RequestNewOutputPath() // ask user for new output path if attempt to write file fails
  //
  // As can be seen, most of the above are trivial and in fact three of them
  // already exist in pre-v3.7 platform-specific code. Refer to the windows
  // reference implementation in vfe/win/ for well-commented examples of the
  // implementation of the above mandatory methods, as well as a number of
  // other optional ones.
  class vfeSession
  {
    friend class vfeConsole;
    friend class vfeParserMessageHandler;
    friend class vfeRenderMessageHandler;
    friend class vfeProcessRenderOptions;
    friend class vfeDisplay;
    friend class VirtualFrontEnd;

    public:
      // Our DisplayCreator functor - see vfeSession::SetDisplayCreator().
      typedef boost::function<vfeDisplay *(unsigned int, unsigned int, vfeSession *session, bool)> DisplayCreator;
      typedef enum
      {
        mUnclassified = 0,
        mDebug,
        mInformation,
        mWarning,
        mPossibleError,
        mError,
        mAnimationStatus,
        mGenericStatus,
        mDivider
      } MessageType ;

      ////////////////////////////////////////////////////////////////////////
      // class MessageBase.
      //
      // MessageBase encapsulates the various types of messages that may be
      // returned from the base POV-Ray code. Each message has associated with
      // it the ID of the session which generated it, a timestamp, and a type.
      // The message itself is stored as a std::string. This base class is
      // specialized into several more specific message types.
      class MessageBase
      {
        public:
          MessageBase(const vfeSession& session) :
            m_Id(session.GetID()), m_TimeStamp (session.GetTimestamp()), m_Type(mUnclassified) {}
          MessageBase(const vfeSession& session, MessageType type, std::string msg = "") :
            m_Id(session.GetID()), m_TimeStamp (session.GetTimestamp()), m_Type(type), m_Message(msg) {}
          virtual ~MessageBase() {}

          POV_LONG m_TimeStamp;
          MessageType m_Type;
          std::string m_Message;
          int m_Id;
      } ;

      ////////////////////////////////////////////////////////////////////////
      // class GenericMessage.
      //
      // GenericMessage builds on MessageBase by further providing the line,
      // column, and filename of the SDL which caused the message generation.
      class GenericMessage : public MessageBase
      {
        public:
          GenericMessage(const vfeSession& session) :
            MessageBase(session), m_Line(0), m_Col(0) {}
          GenericMessage(const vfeSession& session, MessageType type, std::string msg, const UCS2String file = UCS2String(), int line = 0, int col = 0) :
            MessageBase (session, type, msg), m_Filename(file), m_Line(line), m_Col(col) {}
          virtual ~GenericMessage() override {}

          int m_Line;
          int m_Col;
          UCS2String m_Filename;
      } ;

      ////////////////////////////////////////////////////////////////////////
      // class StatusMessage.
      //
      // StatusMessage builds on MessageBase to encapsulate the messages that
      // are generated by the core code during processing of a scene. This
      // includes anything from parse status all the way through to final
      // render stats. It has an optional delay member which is set by the
      // vfe code to suggest to the client that after displaying the current
      // message (the one the delay is retrieved with), it may like to delay
      // that many milliseconds before erasing it and displaying the next.
      // (This is of course only relevent for temporary display locations,
      //  such as in the status bar of a UI-based client implementation).
      class StatusMessage : public MessageBase
      {
        public:
          StatusMessage(const vfeSession& session) :
            MessageBase(session), m_Delay(0), m_Frame(0), m_TotalFrames(0), m_FrameId(0) {}
          StatusMessage(const vfeSession& session, std::string msg, int m_Delay) :
            MessageBase(session, mGenericStatus, msg), m_Delay(m_Delay), m_Frame(0), m_TotalFrames(0), m_FrameId(0) {}
          StatusMessage(const vfeSession& session, const UCS2String& file, int frame, int totalframes, int frameId) :
            MessageBase(session, mAnimationStatus), m_Delay(0), m_Filename(file), m_Frame(frame), m_TotalFrames(totalframes), m_FrameId(frameId) {}
          virtual ~StatusMessage() override {}

          int m_Delay;
          int m_Frame;
          int m_TotalFrames;
          int m_FrameId;
          UCS2String m_Filename;
      } ;

      // The following queues are used to hold messages collected from the
      // core code via vfeSession's worker thread.
      typedef std::queue<GenericMessage> GenericQueue;
      typedef std::queue<StatusMessage> StatusQueue;
      typedef std::queue<MessageBase> ConsoleQueue;

      // Construct a new instance of a vfeSession. An optional ID may be
      // provided (which defaults to 0 if not). This ID is opaque to the
      // VFE code itself - it only has meaning to the client software. It
      // can be used to differentiate between sessions in a multiple-
      // session implementation. If you are not implementing such a client
      // it is sufficient to leave it at the default value.
      vfeSession(int id = 0);
      virtual ~vfeSession();

      // Convenience method to ensure the frontend connection (an internal
      // concept representing the connection between VFE and the POV-Ray
      // internal code) was created successfully. It will throw an exception
      // of type pov_base::Exception if the connection does not exist.
      void CheckFrontend() const { if (m_Frontend == nullptr) throw POV_EXCEPTION_STRING("Frontend not connected"); }

      // Clears many of the internal state information held by VFE regarding
      // a render. Typically called before starting a new render. Included in
      // the clear are the failure/success status, cancel request state,
      // error code, percent complete, number of pixels rendered, total pixels
      // in render, current animation frame, and total animation frame count.
      // If the optional Notify flag is set (defaults to true), a status event
      // of type stClear is generated. Note that this method does not empty the
      // message queues (see Reset() for that).
      virtual void Clear(bool Notify = true);

      // Resets a session - this not only does the same work as vfeSession::Clear
      // (without a stClear notification) but it also empties all the message
      // queues, clears the input and output filenames, clears any animation
      // status information, resets the status flags and clears the event mask.
      // It will then generate a stReset notification.
      virtual void Reset();

      // Clears all messages from the status message queue.
      virtual void ClearStatusMessages();

      // Returns true if a fatal error message has been received since the
      // last call to vfeSession::Clear(). Note that in the context of VFE
      // only errors that will halt a render request are considered 'fatal'.
      virtual bool HadErrorMessage() const { return m_HadErrorMessage; }

      // Return true if the render is considered to have succeeded. In the
      // case of an animation, this relates to all requested frames being
      // processed, not each individual frame. The client is generally
      // notified of render completion via the status event notification
      // system, and then calls this method to find out the result.
      virtual bool Succeeded() const { return m_BackendThreadExited == false && m_Succeeded; }

      // The converse of vfeSession::Succeeded(), with the caveat that it
      // will be set upon the failure of any frame in an animation, not that
      // of all frames.
      virtual bool Failed() const { return m_Failed; }

      // Used to set up a session with a POV backend. Accepts two
      // parameters - a destination (vfeDestInfo) and authorization
      // (vfeAuthInfo), both pointers. Currently these must be `nullptr`.
      //
      // Intialize() will call the Reset() method, and then create
      // the session's worker thread, at which time it will wait on
      // an event for the worker thread to signal that it has completed
      // its own initialization. By default the worker thread setup is
      // considered to have failed if it has not signalled within three
      // seconds (elapsed time). However to aid debugging this is extended
      // to 123 seconds if _DEBUG is defined.
      //
      // If the startup is successful, this method returns vfeNoError after
      // issuing a stBackendStateChanged status notification. Otherwise it
      // will either set m_LastError to vfeInitializeTimeout and return that
      // same value, or it will retrieve the error code set by the worker
      // thread, wait for the worker thread to exit, and return that code.
      //
      // The worker thread itself is responsible for creating the actual
      // connection with the backend code.
      virtual int Initialize(vfeDestInfo *Dest, vfeAuthInfo *Auth);

      // Returns the ID passed to the class constructor. Defaults to 0 if not specified.
      virtual int GetID() const { return m_Id ; }

      // Returns a copy of the shared pointer containing the current instance
      // of a pov_frontend::Display-derived render preview instance, which may
      // be `nullptr`.
      virtual std::shared_ptr<Display> GetDisplay() const;

      // If a VFE implementation has provided a display creator functor via
      // vfeSession::SetDisplayCreator(), this method will call it with the
      // width, height, gamma factor, and default visibility flag of the requested
      // render preview window. Otherwise it will return whatever the default display
      // creator returns (see SetDisplayCreator()).
      //
      // It is called when the core POV-Ray code requests that a render preview
      // window be created. The display instance returned is expected to conform
      // to the definition of the pov_frontend::Display class (but it typically
      // a platform-specific derivative of that.)
      virtual vfeDisplay *CreateDisplay(unsigned int width, unsigned int height, bool visible = false);

      // Used by VFE implementations to allow their own custom pov_frontend::Display
      // derived render preview window class to be created when the main POV-Ray code
      // wants a preview window. The passed parameter is a DisplayCreator (see the
      // typedef earlier on in vfeSession and the example in CreateFrontend() and
      // WinDisplayCreator() of source/windows/pvfrontend.cpp).
      //
      // If this method is not called, the display creator defaults to
      // vfeSession::DefaultDisplayCreator().
      virtual void SetDisplayCreator(DisplayCreator creator) { m_DisplayCreator = creator; }

      // Returns a pointer to the internal VirtualFrontendInstance used by the session.
      // Generally a client should not need this - if there is some good reason to get
      // at the frontend, it probably means that there is functionality missing from
      // vfeFrontend (which, after all, is intended to wrap the actual frontend).
      virtual VirtualFrontEnd *GetFrontend() const { return m_Frontend ; }

      // This method returns the current set of status flags as set by the worker
      // thread and various other parts of vfeSession. The status flags returned
      // are not affected by any event mask set (see vfeSession::SetEventMask()
      // for more information), but whether or not a wait occurs.
      //
      // Two parameters are accepted (both optional). The first one, a bool,
      // determines whether or not the internal copy of the status flags is
      // cleared prior to returning. By default, this is the case. Note that
      // flags identified by the stNoClear mask cannot be cleared by this method
      // and will remain set until vfeSession::Reset() is called. See the definition
      // of the status flags enum in vfesession.h for more information on flags.
      // Note that even if the flags are cleared, an stClear notification does not
      // get set.
      //
      // The second parameter - an int - sets the wait time. If not specified, it
      // defaults to -1, which means wait indefinitely for an event. If set to 0,
      // it effectively turns this method into a status polling function, since
      // there will never be a wait. Otherwise the parameter specifies the maximum
      // number of milliseconds to wait on the status notification event before
      // returning the status flags (note that there is no indication of timeout).
      //
      // Typical usage of this method by a client that has its own status handling
      // thread would be for the thread to call this method with an indefinite
      // timeout, and depend on the stShutdown event and status notification to
      // determine if its host application wants it to exit. stShutdown events can
      // be generated by the host calling vfeSession::Shutdown() (it is safe for
      // this to be called from any thread multiple times).
      //
      // A client that does not want to devote a thread to status processing
      // would typically call this method in polling mode, with perhaps a short
      // timeout, depending on the implementation. (The sample console example
      // provided for VFE does this).
      virtual vfeStatusFlags GetStatus(bool Clear = true, int WaitTime = -1) ;

      // This method allows a client to specify a set of status flags that it
      // wants to allow to generate events. An 'event' in terms of VFE is the
      // case where an action (typically that of the worker thread) occurs that
      // would set, in the internal status flags, a status indication that is
      // not otherwise masked out by a call to this method. The net result of
      // an event being generated is that a thread waiting via a call to the
      // vfeSession::GetStatus() method with a non-zero timeout will get woken
      // up immediately. Note that this will occur even if the flag is already
      // set in the internal flags (e.g. GetStatus() was called with clear set
      // to false). Note also that if more than one client thread has called
      // GetStatus() and is sleeping on an event, only one will be woken.
      //
      // By default, all events are allowed (this is as if SetEventMask() had
      // been called with a parameter of ~stNone).
      //
      // Be aware that some events may not be masked out (e.g. stShutdown,
      // stCriticalError). See the definition of the status flags for more
      // detail.
      virtual void SetEventMask(vfeStatusFlags Mask = stNone) { m_EventMask = vfeStatusFlags (Mask | stNoIgnore); }

      // Returns the current event mask (see vfeSession::SetEventMask()).
      virtual vfeStatusFlags GetEventMask() const { return vfeStatusFlags(m_EventMask); }

      // Returns true if the POV-Ray code is in a state where a pause request
      // both makes sense and can be accepted. Typical use of this method is
      // to call it each time a stBackendStateChanged event occurs, and to use
      // the returned value to configure a client user interface (e.g. to gray
      // out or enable a pause button or menu item).
      //
      // Throws a pov_base::Exception if the frontend does not exist (i.e.
      // vfeSession::Initialize() hasn't been called or failed when called).
      virtual bool IsPausable() const;

      // Used similarly to vfeFrontend::IsPausable(), but returns the actual
      // pause/not paused state.
      //
      // Throws a pov_base::Exception if the frontend does not exist (i.e.
      // vfeSession::Initialize() hasn't been called or failed when called).
      virtual bool Paused() const;

      // Requests the POV-Ray backend code to pause whatever it is doing by entering
      // an idle loop with calls to pov_base::Delay(). This method is synchronous
      // and thus a delay of up to three seconds can occur waiting for the backend
      // to respond. If a timeout occurs, m_LastError is set to vfeRequestTimedOut
      // and this method returns false. Otherwise, m_LastError is set to vfeNoError
      // and the return value is set according to whether the backend accepted the
      // request (true) or rejected it (false). Typically it will only reject a
      // pause request if it is not in a pausable state (e.g. it's not rendering).
      //
      // Implementation note: the pause request itself is actually proxied via the
      // session's worker thread, since the POV-Ray messaging system requires that
      // all such requests come from the thread that created the session (this is
      // to allow internal dispatching of message replies to the right place). The
      // net effect of this is that the worker thread won't be doing anything else
      // (such as dispatching messages) once it starts processing this request.
      //
      // Throws a pov_base::Exception if the frontend does not exist (i.e.
      // vfeSession::Initialize() hasn't been called or failed when called).
      virtual bool Pause();

      // If called with on == true, tells POV-Ray to pause rendering after the end
      // of each frame of an animation (but has no effect on non-animation renders).
      // This is almost the same as calling Pause() at the right time, but without
      // the timing constraints associated with that (for fast renders, the next
      // frame could have started before the frontend's pause request was processed).
      // The pause is not applied after the last frame.
      //
      // Note that the neither the rendering engine nor the parser are actually
      // paused (in the usual sense of the word) by this feature; the implementation
      // simply does not start the next frame after one ends.
      //
      // A render can be resumed by calling Resume() as per normal.
      //
      // Internally, this option defaults to false, and is reset whenever Clear()
      // or Reset() is called.
      virtual void PauseWhenDone(bool on) { m_PauseWhenDone = on; }

      // get the current pause when done setting.
      virtual bool GetPauseWhenDone(void) { return m_PauseWhenDone; }

      // The converse of vfeSession::Pause(). All the same considerations apply.
      virtual bool Resume();

      // Returns true if an animation is being rendered. The result can only
      // be considered valid if the render has actually been started.
      virtual bool RenderingAnimation() const { return m_RenderingAnimation; }

      // Returns the current nominal frame number. Valid once the internal
      // state has reached kStatring. The typical way of reading this is to wait
      // for a stAnimationStatus flag and then call GetCurrentFrameId().
      virtual int GetCurrentFrameId() const { return m_CurrentFrameId; }

      // Returns the current frame of an animation. Only valid once the internal
      // state has reached kStarting (see the stBackendStateChanged status flag).
      // The typical way of reading this is to wait for a stAnimationStatus event
      // and then call GetCurrentFrame().
      virtual int GetCurrentFrame() const { return m_CurrentFrame; }

      // Returns the total frame count of an animation. Valid once the internal
      // state has reached kStatring. The typical way of reading this is to wait
      // for a stAnimationStatus flag and then call GetTotalFrames().
      virtual int GetTotalFrames() const { return m_TotalFrames; }

      // Returns the percentage of the render that has completed, in terms of
      // pixel count, as an int in the range 0 to 100. The internal code that
      // updates this value does so in response to a progress message from
      // the backend, and since this also generates a status message, your
      // status message handling code is a good place to call this method from.
      virtual int GetPercentComplete() const { return m_PercentComplete; }

      // Returns count of pixels rendered. Same consideration for update
      // applies as does for vfeSession::GetPercentComplete().
      virtual int GetPixelsRendered() const { return m_PixelsRendered; }

      // Returns total pixel count. This value is only valid once at least
      // one render progress message has been received from the backend.
      virtual int GetTotalPixels() const { return m_TotalPixels; }

      ////////////////////////////////////////////////////////////////////////
      // Return an absolute path including trailing path separator.
      // *nix platforms might want to just return "/tmp/" here.
      // NB this method is pure virtual.
      virtual UCS2String GetTemporaryPath(void) const = 0;

      ////////////////////////////////////////////////////////////////////////
      // Return a valid and unique temporary filename.
      // NB this method is pure virtual.
      virtual UCS2String CreateTemporaryFile(void) const = 0;

      ////////////////////////////////////////////////////////////////////////
      // Delete the filename passed. Platforms may like to check that the file
      // specified lies either in a valid temporary path location or is a valid
      // temporary filename (see e.g. vfeSession::GetTemporaryPath()).
      // NB this method is pure virtual.
      virtual void DeleteTemporaryFile(const UCS2String& filename) const = 0;

      ////////////////////////////////////////////////////////////////////////
      // Return a timestamp to be used internally for queue sorting etc. The
      // value returned must be 64-bit and in milliseconds; the origin of the
      // count is not important (e.g. milliseconds since 1/1/1970, or whatever
      // it doesn't matter), as long as it is consistent value (milliseconds
      // since system boot is NOT a valid value since it will change each boot).
      // Also please don't call time() and multiply by 1000 since vfe wants at
      // least 100ms precision (so it can do sub-one-second event timing).
      //
      // It's also important that the count not go backwards during the life
      // of a vfeSession instance; this means you should attempt to detect wall
      // clock changes by caching the last value your implementation returns
      // and adding an appropriate offset if you calculate a lower value later
      // in the session.
      //
      // NB this method is pure virtual.
      virtual POV_LONG GetTimestamp(void) const = 0;

      /////////////////////////////////////////////////////////////////////////
      // This method will get called on POVMS critical errors (e.g. cannot
      // allocate memory, amongst other things). Note that you should not
      // assume that you can e.g. allocate memory when processing this call.
      //
      // Before calling this function vfe sets a flag that prevents the worker
      // thread from processing any more messages; it will wait for you to
      // call Shutdown(). Your UI thread can find out if this method has been
      // called by checking for the presence of the stCriticalError status bit
      // returned from vfeSession::GetStatus() (note that this bit is not
      // necessarily already set at the time the method is called though).
      //
      // If you are running a genuine virtual frontend (e.g. stateless HTTP
      // interface), we may want to set a flag to display the message later
      // rather than pop up a messagebox on the local windowstation. Otherwise
      // you would probably display the message immediately.
      virtual void NotifyCriticalError(const char *message, const char *file, int line) = 0;

      // This method causes a shutdown of the vfeSession instance. Specifically
      // it will set a flag asking the worker thread to exit, issue a
      // stShutdown notification, and then wait for the worker thread to exit.
      // The optional boolean parameter (default false) is used by the caller
      // to indicate a forced shutdown of the worker thread is permissible
      // (e.g. a kill if it doesn't shutdown gracefully within a reasonable
      // period of time). It is not mandatory to implement the forced feature.
      virtual void Shutdown(bool forced = false);

      // Requests the backend to cancel the current render. Returns vfeNotRunning
      // if there is no current render, vfeAlreadyStopping if a stop has already
      // been requested, or vfeNoError otherwise. Also sets m_LastError in all
      // cases. Note that this request is handled by the worker thread, like for
      // vfeSession::Pause(), but unlike that request this one is handled
      // asynchronously. This means you need to be prepared to handle the situation
      // that even though you've called CancelRender(), the session hasn't caught
      // up with the processing of the request yet.
      virtual int CancelRender(void);

      // Start a render. Will set m_LastError and return vfeRenderOptionsNotSet
      // if you didn't call vfeSession::SetOptions() first. Otherwise will attempt
      // to start the render; if this fails vfeFailedToSendRenderStart is returned.
      // If the start attempt succeeds (note this is distinct from the start itself
      // actually succeeding - all that the lack of a vfeFailedToSendRenderStart
      // error means is that the start request was successfully sent to the backend)
      // then a stRenderStartup notification is sent and the state is changed to
      // kStarting.
      //
      // If the start attempt fails (signified by the receipt of a std::exception),
      // the exception code will, in the case of a pov_base::Exception, be extracted
      // and stored in m_LastError and returned; otherwise (not pov_base::Exception),
      // m_LastError is set to -1. In either case, a failed status is set (so that
      // vfeSession::Failed() will return true), and, if a fatal error message has
      // not already been queued, the text of the exception will be added to both
      // the status and error message queues.
      virtual int StartRender(void);

      // Sets the options to be used on the next render. Accepts a vfeRenderOptions
      // instance as its only parameter, and returns any one of a number of possible
      // error codes (and sets m_LastError), as documented below:
      //
      //   vfeFailedToInitObject           - this is an internal error
      //   vfeFailedToSetMaxThreads        - self-explanatory
      //   vfeFailedToParseINI             - an INI file specified could not be parsed
      //   vfeFailedToSetSource            - the source file specified could not be set
      //   vfeFailedToParseCommand         - a command-line option was invalid
      //   vfeNoInputFile                  - no input file specified either directly or via INI
      //   vfeRenderBlockSizeTooSmall      - self-explanatory
      //   vfeFailedToWriteINI             - a request to write the render options to an INI file failed
      //   vfeUnsupportedOptionCombination - at least two of the supplied options don't combine
      //
      // If vfeRenderOptions explicitly specifies a source file, it will override
      // any set via a parsed INI file. Furthermore, any source file set via a
      // command-line option overrides both of the above.
      //
      // Note that it is your responsibility to add any default INI files that should
      // be processed to the INI file list; neither SetOptions() nor any other part
      // of the VFE or POV-Ray code will do that for you. This includes non-platform
      // specific files such as a potential povray.ini in the CWD.
      virtual int SetOptions (vfeRenderOptions& opts);

      // Clears any options set by vfeSession::SetOptions().
      virtual void ClearOptions(void) { m_RenderOptions.Clear(); m_OptionsSet = false; }

      // Returns true if the named option (e.g. "Output_To_File") is present in the
      // render options. Will throw an exception if the options have not been set, or
      // if the option name is invalid.
      virtual bool OptionPresent(const char *OptionName);

      // Will return the value of the named option, or the default value if it is not
      // set. Will throw an exception for the same reasons as the OptionPresent() method.
      // NOTE: this method won't always return what you expect; for example, if the user
      // does not explicitly set the output file name, it won't be in the options, and
      // thus won't be returned. That is to say, don't expect these methods to return
      // the default values applied by the renderer in the absense of a specific value.
      // This also applies to partial values, e.g. if the user sets "Output_File_Name=foo",
      // we would return "foo", rather than for example "foo.png".
      bool GetBoolOption(const char *OptionName, bool DefaultVal);
      int GetIntOption(const char *OptionName, int DefaultVal);
      float GetFloatOption(const char *OptionName, float DefaultVal);
      std::string GetStringOption(const char *OptionName, const char *DefaultVal);
      UCS2String GetUCS2StringOption(const char *OptionName, const UCS2String& DefaultVal);

      // Helper method intended to tell VFE if it is running in a console-based
      // client. If called with its boolean parameter set to true, the console
      // word-wrap width will be set to 80, and an internal flag is set. Both
      // this flag and the default wrap width are initially set to assume that
      // a console build is in use, so a client platform only needs to call this
      // if they are UI-based. If the value passed is false, the wrap width is
      // set to 999, and furthermore any future warning or error messages received
      // by the worker thread will additionally be added to the status message
      // queue, along with the filename, line, and column where they occurred
      // (assuming this was provided). UI-based clients can then, when they
      // retrieve the status message, use its content to auto-display the error
      // location in the source file if they so choose.
      virtual void OptimizeForConsoleOutput(bool On) { m_OptimizeForConsoleOutput = On; m_ConsoleWidth = On ? 80 : 999; }

      // Returns the raw POV-Ray internal state (e.g. kRendering etc)
      virtual State GetBackendState() const { return m_BackendState; }

      // Translates a state as returned from vfeSession::GetBackendState() into a string.
      virtual const char *GetBackendStateName(void) const ;

      // Return the input filename. This is not valid until vfeSession::SetOptions()
      // has been called successfully.
      virtual UCS2String GetInputFilename(void) const { return m_InputFilename; }

      // Return the output filename. You should not call this until you have received
      // a stOutputFilenameKnown status event. While it may be tempting to assume
      // that the output filename is known as soon as the options are set, this is
      // not necessarily the case - consider animations for example. Note it is
      // also possible that you will never receive an stOutputFilenameKnown event
      // since it is of course possible that output to file is turned off.
      virtual UCS2String GetOutputFilename(void) const { return m_OutputFilename; }

      // This method returns true if the current options specify that an output
      // image should be written. It is only valid if vfeSession::SetOptions()
      // has been called successfully.
      virtual bool OutputToFileSet(void) const { return m_OutputToFileSet; }

      // This method returns true if the current platform supports writing image
      // output to STDOUT. Platforms that do not support this should override it.
      virtual bool ImageOutputToStdoutSupported(void) const { return true; }

      // This method returns the currently-set render width, in pixels.
      // It is only valid if vfeSession::SetOptions() has been called successfully.
      virtual int GetRenderWidth(void) const { return m_RenderWidth; }

      // This method returns the currently-set render height, in pixels.
      // It is only valid if vfeSession::SetOptions() has been called successfully.
      virtual int GetRenderHeight(void) const { return m_RenderHeight; }

      // This method returns a reference to the vfeRenderOptions instance stored
      // within vfe. After vfeSession::SetOptions() is called successfully, the
      // supplied options are copied into the internal instance.
      virtual vfeRenderOptions& GetOptions() { return m_RenderOptions; }

      // Fetches one message from either the generic, status, or console message
      // queues, whichever has the earliest timestamp. Returns true if a message
      // is fetched, otherwise false (meaning all the queues are empty). It expects
      // two parameters - a reference to a MessageType, in which the type of the
      // returned message is stored, and a std::string, which will be set to the
      // actual message text. Note that linefeeds are not appended.
      //
      // This method is primarily useful for console-style displays as it discards
      // any additional meta-information the message may have had, such as the
      // line number of an error. (Note that this does not mean the line number
      // cannot be in the message string; it may very well be).
      virtual bool GetNextCombinedMessage (MessageType &Type, std::string& Message);

      // Gets the next non-status message (meaning generic or console messages)
      // from the aforementioned queues; whichever is the earliest. Returns false
      // if there is no message to fetch, otherwise will set the message type,
      // filename, line, and column parameters supplied. If the message retrieved
      // did not contain this information, the relevent entry is either set to 0
      // (line and column) or the empty string (filename). The filename parameter
      // is a UCS2String.
      virtual bool GetNextNonStatusMessage (MessageType &Type, std::string& Message, UCS2String& File, int& Line, int& Col);

      // Gets the next non-status message (meaning generic or console messages)
      // from the aforementioned queues; whichever is the earliest. Returns false
      // if there is no message to fetch, otherwise will set the message type,
      // filename, line, and column parameters supplied. If the message retrieved
      // did not contain this information, the relevent entry is either set to 0
      // (line and column) or the empty string (filename). The filename parameter
      // is a std::string.
      virtual bool GetNextNonStatusMessage (MessageType &Type, std::string& Message, std::string& File, int& Line, int& Col);

      // Gets the next non-status message (meaning generic or console messages)
      // from the aforementioned queues; whichever is the earliest. Returns false
      // if there is no message to fetch, otherwise will set the message type
      // and text content parameters supplied. Any additional meta-information
      // that may have been contained in the message is discarded.
      virtual bool GetNextNonStatusMessage (MessageType &Type, std::string& Message);

      // Returns false if there are no messages in the status message
      // queue, otherwise removes the oldest status message from the
      // queue and copies it into the StatusMessage reference supplied
      // as a parameter, then returns true.
      virtual bool GetNextStatusMessage (StatusMessage& Message);

      // Returns false if there are no messages in the generic message
      // queue, otherwise removes the oldest message from the queue and
      // copies it into the GenericMessage reference supplied as a
      // parameter, then returns true.
      virtual bool GetNextGenericMessage (GenericMessage& Message);

      // Returns false if there are no messages in the console message
      // queue, otherwise removes the oldest message from the queue and
      // copies it into the MessageBase reference supplied as a parameter,
      // then returns true.
      virtual bool GetNextConsoleMessage (MessageBase& Message);

      // Returns a std::string potentially useful for displaying in the status
      // line (or other similar UI element) of a UI-based client implementation.
      // The VFE internal code takes care of setting this to what it considers
      // reasonable values and issuing either a stStatusMessage or stAnimationStatus
      // event notification, as appropriate. Upon receiving either of the above
      // events therefore you may like to simply call this method and place the
      // returned value wherever suitable, overwriting the previous value.2
      virtual std::string GetStatusLineMessage() { return m_StatusLineMessage; }

      // Sets the maximum number of status messages that will be stored in the
      // status queue. If this limit is reached, the oldest message will be
      // discarded each time a new message is added. The default value is -1,
      // which means that there is no limit.
      virtual void SetMaxStatusMessages (int Max) { m_MaxStatusMessages = Max; }

      // Sets the maximum number of generic messages that will be stored in the
      // status queue. If this limit is reached, the oldest message will be
      // discarded each time a new message is added. The default value is -1,
      // which means that there is no limit.
      virtual void SetMaxGenericMessages (int Max) { m_MaxGenericMessages = Max; }

      // Sets the maximum number of console messages that will be stored in the
      // status queue. If this limit is reached, the oldest message will be
      // discarded each time a new message is added. The default value is -1,
      // which means that there is no limit.
      virtual void SetMaxConsoleMessages (int Max) { m_MaxConsoleMessages = Max; }

      // Returns a string giving a short english description of the error code
      // supplied as the only parameter. If no parameter is supplied, the default
      // value (-1) instructs the method to instead use the value of m_LastError.
      virtual const char *GetErrorString(int code = -1) const ;

      // Returns true if opening the given file in the specified mode is to
      // be permitted. It is allowed to ask the user, so this method could
      // take some time to return (especially if user is not at workstation).
      // The default implementation simply returns true. Platforms should
      // override this method and do something more useful (see the Windows
      // implementation in vfe/win/vfeplatform.cpp).
      virtual bool TestAccessAllowed(const Path& file, bool isWrite) const { return true; }

      // Allows you to manually set the console wrap width.
      // The supplied value is clipped to the range 80-999.
      virtual void SetConsoleWidth(int width) { m_ConsoleWidth = std::max(std::min(999,width),80); }

      // Return the current console wrap width.
      virtual int GetConsoleWidth(void) { return m_ConsoleWidth; }

      // Clear all IO Restriction-related paths previously set.
      virtual void ClearPaths() { m_ReadPaths.clear(); m_WritePaths.clear(); m_ExcludedPaths.clear(); }

      // Adds the supplied path to the list of allowed read paths, along with
      // the recursive flag (meaning paths under that path are also allowed).
      // The supplied path is a pov_base::Path instance.
      virtual void AddReadPath(const Path& path, bool recursive = true) { m_ReadPaths.push_back(IOPath(path, recursive)); }

      // Adds the supplied path to the list of allowed read paths, along with
      // the recursive flag (meaning paths under that path are also allowed).
      // The supplied path is a UCS2String.
      virtual void AddReadPath(const UCS2String& path, bool recursive = true) { m_ReadPaths.push_back(IOPath(path, recursive)); }

      // Adds the supplied path to the list of allowed read paths, along with
      // the recursive flag (meaning paths under that path are also allowed).
      // The supplied path is a std::string.
      virtual void AddReadPath(const std::string& path, bool recursive = true) { m_ReadPaths.push_back(IOPath(path, recursive)); }

      // Adds the supplied path to the list of allowed read paths.
      // The supplied path is a pre-constructed IOPath instance.
      virtual void AddReadPath(const IOPath& path) { m_ReadPaths.push_back(path); }

      // Adds the supplied path to the list of allowed write paths, along with
      // the recursive flag (meaning paths under that path are also allowed).
      // The supplied path is a pov_base::Path instance.
      virtual void AddWritePath(const Path& path, bool recursive = true) { m_WritePaths.push_back(IOPath(path, recursive)); }

      // Adds the supplied path to the list of allowed write paths, along with
      // the recursive flag (meaning paths under that path are also allowed).
      // The supplied path is a UCS2String.
      virtual void AddWritePath(const UCS2String& path, bool recursive = true) { m_WritePaths.push_back(IOPath(path, recursive)); }

      // Adds the supplied path to the list of allowed write paths, along with
      // the recursive flag (meaning paths under that path are also allowed).
      // The supplied path is a std::string.
      virtual void AddWritePath(const std::string& path, bool recursive = true) { m_WritePaths.push_back(IOPath(path, recursive)); }

      // Adds the supplied path to the list of allowed write paths.
      // The supplied path is a pre-constructed IOPath instance.
      virtual void AddWritePath(const IOPath& path) { m_WritePaths.push_back(path); }

      // Adds the supplied path to the list of excluded paths, along with the
      // recursion flag. A path in the exclusion list is never allowed to be
      // written to, even if it would be otherwise permitted by a write path.
      // Unlike read and write paths, which may be specified by users, the
      // intent of excluded paths is to allow client implementations to hard-
      // code certain paths that may never be written to, such as the location
      // of the platform's master configuration files (that presumably contain
      // contain the user-specified read and write path settings). The supplied
      // path is a pov_base::Path instance.
      virtual void AddExcludedPath(const Path& path, bool recursive = true) { m_ExcludedPaths.push_back(IOPath(path, recursive)); }

      // Adds the supplied path to the list of excluded paths, along with the
      // recursion flag. The supplied path is a UCS2String.
      virtual void AddExcludedPath(const UCS2String& path, bool recursive = true) { m_ExcludedPaths.push_back(IOPath(path, recursive)); }

      // Adds the supplied path to the list of excluded paths, along with the
      // recursion flag. The supplied path is a std::string.
      virtual void AddExcludedPath(const std::string& path, bool recursive = true) { m_ExcludedPaths.push_back(IOPath(path, recursive)); }

      // Adds the supplied path to the list of excluded paths, along with the
      // recursion flag. The supplied path is a pre-constructed IOPath instance.
      virtual void AddExcludedPath(const IOPath& path) { m_ExcludedPaths.push_back(path); }

      // Returns a const reference to the list of permitted read paths.
      virtual const IOPathVector& GetReadPaths() const { return m_ReadPaths; }

      // Returns a copy of the list of permitted read paths.
      virtual IOPathVector GetReadPaths() { return m_ReadPaths; }

      // Returns a const reference to the list of permitted write paths.
      virtual const IOPathVector& GetWritePaths() const { return m_WritePaths; }

      // Returns a copy of the list of permitted write paths.
      virtual IOPathVector GetWritePaths() { return m_WritePaths; }

      // Returns a const reference to the list of excluded paths.
      virtual const IOPathVector& GetExcludedPaths() const { return m_ExcludedPaths; }

      // This static method is used to return the vfeSession associated with
      // the calling thread (if any). In a full multi-session-enabled
      // implementation of vfe, it would look up the session from a list of
      // active sessions keyed with the value returned from GetThreadID().
      // Currently as only one session is supported (this is enforced in the
      // vfeSession::Initialize() method) it simply returns the value of a
      // global variable.
      static vfeSession *GetSessionFromThreadID() ;

      // These methods just return whether or not the named option is in effect.
      // They are only valid once vfeParserMessageHandler::Options has been called.
      virtual bool GetUsingAlpha() { return m_UsingAlpha; }
      virtual bool GetClocklessAnimation() { return m_ClocklessAnimation; }
      virtual bool GetRealTimeRaytracing() { return m_RealTimeRaytracing; }

      // return elapsed time in the render as milliseconds. only valid once the
      // state changs to rendering.
      virtual POV_LONG GetElapsedTime() { return GetTimestamp() - m_StartTime; }

      virtual void AppendErrorMessage (const std::string& Msg);
      virtual void AppendWarningMessage (const std::string& Msg);
      virtual void AppendStatusMessage (const std::string& Msg, int RecommendedPause = 0);
      virtual void AppendStatusMessage (const boost::format& fmt, int RecommendedPause = 0);
      virtual void AppendWarningAndStatusMessage (const std::string& Msg, int RecommendedPause = 0) { AppendWarningMessage(Msg); AppendStatusMessage(Msg, RecommendedPause); }
      virtual void AppendErrorAndStatusMessage (const std::string& Msg, int RecommendedPause = 0) { AppendErrorMessage(Msg); AppendStatusMessage(Msg, RecommendedPause); }

      // returns true if a render cancel was requested at some point since the render started.
      virtual bool GetCancelRequested() { return m_RenderCancelRequested | m_RenderCancelled ; }

      // returns true if the main POV-Ray backend has shut down
      virtual bool BackendFailed() { return m_BackendThreadExited; }

    protected:
      // All of the following are internal to vfe.
      virtual void SetFailed();
      virtual void SetSucceeded (bool ok);
      virtual void AdviseOutputFilename(const UCS2String& Filename);
      virtual void AdviseFrameCompleted();
      virtual void SetRenderingAnimation();
      virtual void SetPercentComplete(int Percent) { m_PercentComplete = Percent; }
      virtual void SetPixelsRendered(int Rendered, int Total) { m_PixelsRendered = Rendered; m_TotalPixels = Total; }

      virtual void AppendStreamMessage (MessageType type, const char *message, bool chompLF = false);
      virtual void AppendStreamMessage (MessageType type, const boost::format& fmt, bool chompLF = false);
      virtual void AppendErrorMessage (const std::string& Msg, const UCS2String& File, int Line = 0, int Col = 0);
      virtual void AppendWarningMessage (const std::string& Msg, const UCS2String& File, int Line = 0, int Col = 0);
      virtual void AppendAnimationStatus (int FrameId, int SubsetFrame, int SubsetTotal, const UCS2String& Filename);

      virtual void SetUsingAlpha() { m_UsingAlpha = true ; }
      virtual void SetClocklessAnimation() { m_ClocklessAnimation = true ; }
      virtual void SetRealTimeRaytracing() { m_RealTimeRaytracing = true ; }
      virtual void NotifyEvent(vfeStatusFlags Status);

      virtual void BackendThreadNotify();
      virtual void WorkerThread();
      virtual void WorkerThreadStartup() {}
      virtual void WorkerThreadShutdown() {}

      virtual bool ProcessFrontend (void);
      virtual bool ProcessCancelRender(void);
      virtual bool StopRender(const std::string& reason);

      // This method allows your platform code to perform platform-specific actions
      // when a render stops (whether it succeeds or fails). A good example would
      // be to shrink the heap. Be aware that it is called in the context of the
      // worker thread, and that therefore on windowing systems where thread context
      // is relevent to making certain UI calls, it may not be possible to perform
      // certain UI actions here. For most platforms the default implementation
      // should be sufficient.
      virtual void RenderStopped(void);

      // Platforms that support the ability to return excess heap memory to
      // the operating system should implement this method.
      virtual void ShrinkHeap() {}

      // Hook this if you want to directly catch state change events (but if
      // you do, ensure you call back to the default implementation as well).
      virtual void StateChanged(pov_frontend::State NewState);

      /////////////////////////////////////////////////////////////////////////
      // This method will get called when a render completes and the image writing
      // code is unable to write the requested output file for some reason (e.g.
      // disk full, existing output file is read-only, or whatever). The return
      // value can determine any one of several possible actions the code will take.
      // It's up to you what you do here, but a typical example would be to display
      // an error dialog showing the reason (which will be a short string) and the old
      // path, and allow the user to browse for a new path (you may want to auto-
      // suggest one). It is allowable to perform tests yourself on the path to
      // obtain some OS-specific information as to why it failed in order to better
      // determine what to suggest as the new path (e.g. the the output path is
      // not writable due to insufficient privileges, you may want to default the
      // new path to the user's home directory).
      //
      // Any new path that is to be returned should be placed in NewPath. The parameter
      // CallCount will initially be 0, and represents the number of times the
      // method has been called for a given rendering; this allows you for example
      // to auto-default on the first call and prompt the user on subsequent ones.
      // (Note that if you do this and the auto-default succeeds, it's up to you to
      // notify the user that the output file is not what they originally asked for).
      //
      // You may want to place a timeout on this dialog if the session is running
      // an animation and return in the case of a timeout a default value.
      //
      // The return values can be any of the following:
      //
      //   0 : don't try again, leave the render without an output file but preserve
      //       the state file (so a render with +c will reload the image data).
      //   1 : reserved for later support
      //   2 : don't try again and delete the state file (rendered data can't be
      //       recovered).
      //   3 : try again with the same file (NewPath is ignored).
      //   4 : try again with the new path returned in NewPath.
      //
      // Note that if you choose to specify any of the "don't try again" options,
      // and the session happens to be an animation, and it's likely the same
      // error will occur again (e.g. invalid output path or something), then
      // you may want to call the render cancel API so the user isn't bombarded
      // with an error message for each frame of the render.
      //
      // NB this method is pure virtual.
      //
      // NOTE: The code to call this method isn't implemented in vfe yet.
      virtual int RequestNewOutputPath(int CallCount, const std::string& Reason, const UCS2String& OldPath, UCS2String& NewPath) = 0;

      // Create an instance of the frontend ShelloutProcessing class. this handles creating and
      // managing render shellout commands, and typically will need platform-specific implementation.
      virtual ShelloutProcessing *CreateShelloutProcessing(POVMS_Object& opts, const std::string& scene, unsigned int width, unsigned int height) { return new ShelloutProcessing(opts, scene, width, height); }

      struct vfeSessionWorker
      {
        vfeSessionWorker(vfeSession& Parent) : m_Parent(Parent) {}
        void operator()() { m_Parent.WorkerThread(); }
        vfeSession& m_Parent;
      };

      int m_Id;
      int m_RenderWidth;
      int m_RenderHeight;
      int m_RenderErrorCode;
      int m_InitializeErrorCode;
      int m_LastError;
      int m_ConsoleWidth;
      int m_PercentComplete;
      int m_PixelsRendered;
      int m_TotalPixels;
      int m_CurrentFrameId;
      int m_CurrentFrame;
      int m_TotalFrames;
      bool m_Failed;
      bool m_Succeeded;
      bool m_HadErrorMessage;
      bool m_UsingAlpha;
      bool m_RenderingAnimation;
      bool m_RealTimeRaytracing;
      bool m_ClocklessAnimation;
      bool m_HadCriticalError;
      bool m_RenderCancelled;
      bool m_RenderCancelRequested;
      bool m_DontWriteImage;
      bool m_OutputToFileSet;
      bool m_OptionsSet;
      bool m_OptimizeForConsoleOutput;
      bool m_PauseWhenDone;
      POV_LONG m_StartTime;
      unsigned int m_MessageCount;
      vfeStatusFlags m_EventMask;
      vfeStatusFlags m_StatusFlags;
      vfeRenderOptions m_RenderOptions;

      static bool m_Initialized;
      static vfeSession *m_CurrentSessionTemporaryHack;
      std::shared_ptr<Console> m_Console;

      virtual vfeDisplay *DefaultDisplayCreator (unsigned int width, unsigned int height, vfeSession *session, bool visible);
      DisplayCreator m_DisplayCreator;

      int m_MaxStatusMessages;
      int m_MaxGenericMessages;
      int m_MaxConsoleMessages;
      GenericQueue m_MessageQueue;
      StatusQueue m_StatusQueue;
      ConsoleQueue m_ConsoleQueue;
      std::string m_StatusLineMessage;
      UCS2String m_OutputFilename;
      UCS2String m_InputFilename;
      IOPathVector m_ReadPaths;
      IOPathVector m_WritePaths;
      IOPathVector m_ExcludedPaths;

      VirtualFrontEnd *m_Frontend;
      State m_BackendState;

      std::mutex m_MessageMutex;
      std::mutex m_SessionMutex;
      std::condition_variable m_SessionEvent;
      std::mutex m_InitializeMutex;
      std::condition_variable m_InitializeEvent;
      std::condition_variable m_ShutdownEvent;
      std::thread *m_WorkerThread;
      std::thread *m_BackendThread;
      volatile bool m_WorkerThreadExited;
      volatile bool m_BackendThreadExited;
      volatile bool m_WorkerThreadShutdownRequest;

      std::mutex m_RequestMutex;
      std::condition_variable m_RequestEvent;
      volatile int m_RequestFlag;
      volatile int m_RequestResult;
  } ;
}
// end of namespace vfe

#endif // POVRAY_VFE_VFESESSION_H
