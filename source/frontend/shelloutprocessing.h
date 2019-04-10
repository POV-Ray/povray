//******************************************************************************
///
/// @file frontend/shelloutprocessing.h
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

#ifndef POVRAY_FRONTEND_SHELLOUTPROCESSING_H
#define POVRAY_FRONTEND_SHELLOUTPROCESSING_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "frontend/configfrontend.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <string>
#include <memory>

// Boost header files
#include <boost/format.hpp>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"

// POV-Ray header files (frontend module)
//  (none at the moment)

namespace pov_frontend
{

class ShelloutProcessing;

class ShelloutAction final
{
public:
    typedef enum
    {
        ignore = 'i',
        skipOne = 's',
        skipAll = 'a',
        quit = 'q',
        abort = 'u',
        fatal ='f'
    } Action;

    ShelloutAction(const ShelloutProcessing *sp, unsigned int attribID, POVMS_Object& opts);
    ~ShelloutAction() {}

    Action ReturnAction(void) const { return returnAction; }
    bool IsSet(void) const { return isSet; }
    const std::string& RawCommand(void) const { return rawCommand; }
    const std::string& Command(void) const { return command; }
    const std::string& RawParameters(void) const { return rawParameters; }
    const std::string& Parameters(void) const { return parameters; }
    bool ReturnNegate(void) const { return returnNegate; }
    void ExpandParameters(const std::string& scene, const std::string& ofn, unsigned int w, unsigned int h, float clock, unsigned int frame);

private:
    bool          isSet;
    bool          returnNegate;
    std::string   command;
    std::string   rawCommand;
    std::string   parameters;
    std::string   rawParameters;
    Action        returnAction;

    ShelloutAction() = delete;
};

class ShelloutProcessing
{
    friend class ShelloutAction;

public:
    typedef std::shared_ptr<ShelloutAction> ShelloutPtr;

    typedef enum
    {
        preScene,
        postScene,
        preFrame,
        postFrame,
        userAbort,
        fatalError,
        lastShelloutEvent
    } shelloutEvent;

    // we use strings rather than UCS2Strings for the scene name and parameters since the passed
    // parameters (via POVMS) are also strings.
    ShelloutProcessing(POVMS_Object& opts, const std::string& scene, unsigned int width, unsigned int height);

    // you should reap any processes here as needed, and forcefully terminate ones still running.
    virtual ~ShelloutProcessing();

    // true if a shellout command was specified for the given phase
    bool IsSet(shelloutEvent which) const { return shellouts[which]->IsSet(); }

    // retrieve details for each type of shellout.
    ShelloutAction::Action ReturnAction(shelloutEvent which) const { return shellouts[which]->ReturnAction(); }

    // return the command string as passed from the option parser; i.e. complete with parameters
    std::string RawCommand(shelloutEvent which) const { return shellouts[which]->RawCommand(); }

    // the command itself, separated from its parameters. quotes around the command will have been removed.
    std::string Command(shelloutEvent which) const { return shellouts[which]->Command(); }

    // the raw parameters after separation from the command. any quotes will remain in place.
    std::string RawParameters(shelloutEvent which) const { return shellouts[which]->RawParameters(); }

    // the parameters after expansion of terms; e.g. %s to scene name. SetOutputFile() and
    // SetFrameClock() (if relevant) must be called prior to calling this method.
    std::string Parameters(shelloutEvent which) const { return shellouts[which]->Parameters(); }

    // returns true if all frames should be skipped. if so, any subsequent calls for
    // pre-frame and post-frame actions will be ignored (and preferebly should not be
    // made). the post-scene action should always be called; internal logic will determine
    // if it will do anything.
    bool SkipAllFrames(void) const { return skipAllFrames; }

    // returns true if next frame should be skipped. if so, pre-frame and post-frame actions
    // will be ignored. the internal skip frame flag is only reset when the frame number is
    // updated. NB skip all frames does not imply skip next frame since they are handled differently.
    bool SkipNextFrame(void) const { return skipNextFrame; }

    // returns the exit code that POV-Ray should return at the end of the render.
    // 0 means normal exit, 1 means fatal error, and 2 means user abort.
    int ExitCode(void) const { return exitCode; }

    // returns a string representation of the exit code; e.g. 'user abort'
    std::string ExitDesc(void) const { return exitCode == 0 ? "SUCCESS" : exitCode == 2 ? "USER ABORT" : "FATAL ERROR"; }

    // returns true if the render should be halted.
    bool RenderCancelled(void) const { return cancelRender; }

    // return true if the pre-scene event has been seen
    bool HadPreScene(void) const { return hadPreScene; }

    // return true if the post-scene event has been seen
    bool HadPostScene(void) const { return hadPostScene; }

    // if there is no output file, it is not required to call SetOutputFile().
    // if there is an output file, this method must be called prior to the pre-scene
    // action with the value of the first output file, and prior to pre-frame for each
    // subsequent output file if the render is an animation.
    void SetOutputFile(const std::string& filename) { outputFile = filename; }

    // if the render is not an animation, there is no need to call SetFrameClock().
    // if it is an animation, it must be called prior to the pre-scene action, and
    // then prior to pre-frame for each frame of the animation. note that if an action
    // returns the 'skip next frame' option, the SkipNextFrame() method will continue
    // to return true until the frame number supplied via this method has changed.
    void SetFrameClock(unsigned int frame, float clock) { if (frameNo != frame) skipNextFrame = false; frameNo = frame; clockVal = clock; }

    // shutdown any currently-running shellouts. if force is true, force them to exit.
    // in either case, don't wait more than timeout seconds. return true if there are
    // no more processes running afterwards.
    bool KillShellouts(int timeout, bool force = false);

    // the message is constructed as per the documentation for the boost::format class.
    // the positional parameters are as follows:
    //   1: the event causing the cancel (as a string), e.g. "pre-scene"
    //   2: the POV-Ray return code (as an integer)
    //   3: the return code (as an upper-case string), e.g. "USER ABORT"
    //   4: the return code (as a lower-case string).
    //   5: the reason for the cancel (as a string), e.g. "generate a user abort"
    //   6: the command name that generated the cancel
    //   7: the command parameters (CAUTION: may contain escape codes)
    //   8: the command return code (as an integer)
    //   9: output text from the command, as returned by LastShelloutResult()
    virtual std::string GetCancelMessage(void);
    virtual void SetCancelMessage(const std::string& format) { cancelFormat.parse(format); }

    // the positional parameters are as follows:
    //   1: the event causing the skip (as a string), e.g. "pre-scene"
    //   2: the type of the skip (as a string); e.g. "skip frame 11" or "skip all remaining frames"
    //   3: the command name that generated the skip
    //   4: the command parameters (CAUTION: may contain escape codes)
    //   5: the command return code (as an integer)
    //   6: output text from the command, as returned by LastShelloutResult()
    virtual std::string GetSkipMessage(void);
    virtual void SetSkipMessage(const std::string& format) { skipFormat.parse(format); }

    // advise the code that a particular event should be handled now; e.g. pre-scene, post-scene
    // and so forth. this method should be called even if the platform indicates it does not
    // support shellouts; if a render defines one, the code will thrown a kCannotOpenFileErr
    // exception at the time (which should be handled by the caller). this method does not block
    // the caller during processing of shellouts as they are run in the background. the return
    // value indicates whether or not rendering should be cancelled as a result of a shellout:
    // generally this won't be known at the time of return, however it could be set early if
    // a shellout could not be started and the INI file indicated that render should be halted
    // on failure.
    virtual bool ProcessEvent(shelloutEvent event) { return HandleProcessEvent(event, false); }

    // returns true if a shellout is currently running. if this method is being called in an
    // event loop to wait until a shellout has completed, it is the responsibility of the event
    // loop to perform appropriate sleeps to avoid wasting CPU time. platforms that implement
    // shellout support MUST override this method to return an appropriate value by actually
    // checking the process each time it's called.
    virtual bool ShelloutRunning(void);

    // return the name of the currently running shellout (without parameters)
    // if no shellout is running, an empty string should be returned.
    virtual std::string ProcessName(void) { return ShelloutRunning() ? runningProcessName : std::string(); }

    // return the PID of the currently running shellout (or equivalent thereof).
    // returns 0 if no process is running, and -1 of the platform has no PID equivalent
    // or this method is not implemented.
    virtual int ProcessID(void) { return -1; }

    // return a descriptive string detailing the result of the last shellout command
    // in a form suitable for display on the console or UI message log - preferably
    // no more than a single line (width unimportant). if not implemented, an empty
    // string should be returned.
    virtual std::string LastShelloutResult(void) { return std::string(); }

    // return true if this platform supports shellouts.
    virtual bool ShelloutsSupported(void) { return false; }

protected:
    int exitCode;
    int cancelReturn;
    int skipReturn;
    bool skipAllFrames;
    bool skipNextFrame;
    bool cancelRender;
    bool skipCallouts;
    bool killRequested;
    bool hadPreScene;
    bool hadPostScene;
    bool hadUserAbort;
    bool hadFatalError;
    bool commandProhibited;
    unsigned int frameNo;
    unsigned int imageWidth;
    unsigned int imageHeight;
    float clockVal;
    std::string sceneName;
    std::string outputFile;
    std::string runningProcessName;
    std::string cancelPhase;
    std::string cancelReason;
    std::string cancelCommand;
    std::string cancelParameters;
    std::string cancelOutput;
    std::string skipPhase;
    std::string skipReason;
    std::string skipCommand;
    std::string skipParameters;
    std::string skipOutput;
    boost::format cancelFormat;
    boost::format skipFormat;
    ShelloutPtr shellouts[lastShelloutEvent];

    // helper method
    std::string GetPhaseName(shelloutEvent event);

    // execute the given command with the supplied parameters, which have already
    // been expanded as per the docs, and immediately return true without waiting
    // for completion of the process. if the command can't be run other than for
    // one of the reasons documented below, return false (in which case CollectCommand
    // should return -2 if called later on).
    //
    // if shellouts are not supported, or access to the executable is prohibited by
    // POV-Ray internal rules (not the OS), throw a kCannotOpenFile exception with an
    // appropriate message. you should also throw a (different) exception if a process
    // is still running. any exception thrown will cancel a render (including remaining
    // frames of an animation) and the fatal error shellout (if defined) will not be
    // called.
    //
    // you should reap any processes in your destructor in case CollectCommand doesn't
    // get called.
    //
    // if the platform implemeting a subclass of this method has the equivalent of a
    // system log (e.g. syslog on unix, event log on windows), the implementation should
    // consider providing a user-controllable option to log any commands using such.
    virtual bool ExecuteCommand(const std::string& cmd, const std::string& params);

    // shutdown any currently-running shellouts. if force is true, force them to exit.
    // in either case, don't wait more than timeout milliseconds. return true if there
    // are no more processes running afterwards.
    virtual bool KillCommand(int timeout, bool force = false) { return true; }

    // returns true if a shellout is currently running. if this method is being called in an
    // event loop to wait until a shellout has completed, it is the responsibility of the event
    // loop to perform appropriate sleeps to avoid wasting CPU time. platforms that implement
    // shellout support MUST override this method to return an appropriate value by actually
    // checking the process each time it's called.
    virtual bool CommandRunning(void) { return false; }

    // if no process is running or has already been reaped, return -2. if a process
    // is still running, return -1. if the process is complete, place the output into
    // output then return the process's exit code. if the process failed, it would help
    // if the output string included (or was only) stderr, but this is not a requirement.
    //
    // if the platform does not support capturing output of processes (or the
    // processes are GUI-based), there is no requirement to return any output.
    virtual int CollectCommand(std::string& output) { return -2; }
    virtual int CollectCommand(void) { return -2; }

    // return true if the requested shellout command is permitted. this method is
    // called just before a shellout runs. if it fails, an exception will generally
    // be thrown by the caller (the method itself should not throw an exception).
    virtual bool CommandPermitted(const std::string& command, const std::string& parameters) { return true; }

    // called by the internal parser during construction to separate commands from parameters.
    // given a raw string in the form returned from the POV INI file, extract the command and any parameters.
    // the default version of this method should suffice for most implementations, however some platforms
    // may need different treatment of quotes or escapes. for example, the windows platform may wish to
    // provide a special-case for strings that look like windows paths, and exempt them from escaping of
    // the backslash.
    //
    // the default method will trim the source and then search it for the first whitespace character
    // that is both outside of a quoted string and not escaped. this forms the boundary between the
    // command and the parameters (if not found, the entire source is considered the command). the code
    // accepts single and double quotes as acceptable delimiters. if quotes are found around the command,
    // they are removed. no other quotes (including any within the parameters) will be altered.
    //
    // when parsing the command portion of the string, any backslashes found are considered to be escapes
    // which remove the special meaning of the following character. the escape is removed and the next
    // character will not be subject to special interpretation (this affects single quote, double quote,
    // backslashes, and any whitespace characters. any escapes in the text after the point where the
    // parameters start will not be removed.
    //
    // this method should return true if the command is non-empty upon completion.
    virtual bool ExtractCommand(const std::string& src, std::string& command, std::string& parameters) const;

private:
    bool processStartRequested;
    shelloutEvent postProcessEvent;

    bool HandleProcessEvent(shelloutEvent which, bool internalCall);
    bool PostProcessEvent();
};

}
// end of namespace pov_frontend

#endif // POVRAY_FRONTEND_SHELLOUTPROCESSING_H
