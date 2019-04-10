//******************************************************************************
///
/// @file frontend/shelloutprocessing.cpp
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
#include "frontend/shelloutprocessing.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// Boost header files
#include <boost/algorithm/string.hpp>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (POVMS module)
#include "povms/povmsid.h"

// POV-Ray header files (frontend module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

using namespace pov_base;

ShelloutAction::ShelloutAction(const ShelloutProcessing *sp, unsigned int attribID, POVMS_Object& opts): returnAction(ignore), returnNegate(false), isSet(false)
{
    if (opts.Exist(attribID))
    {
        POVMS_Object attr;

        opts.Get(attribID, attr);
        if (attr.Exist(kPOVAttrib_CommandString))
        {
            rawCommand = attr.GetString(kPOVAttrib_CommandString);
            if (sp->ExtractCommand(rawCommand, command, rawParameters))
            {
                int action = attr.TryGetInt(kPOVAttrib_ReturnAction, (int) ignore);
                switch (tolower(action))
                {
                    case ignore:
                    case skipOne:
                    case skipAll:
                    case quit:
                    case abort:
                    case fatal:
                        returnAction = (Action) tolower(action);
                        returnNegate = isupper(action);
                        isSet = true;
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

// expand the parameters in the rawParameters member into the parameters member, applying the escapes
// documented below. scene is the scene name, ofn the output file name, w/h the width/height, clock the
// current animation clock value, and frame the current frame number.
void ShelloutAction::ExpandParameters(const std::string& scene, const std::string& ofn, unsigned int w, unsigned int h, float clock, unsigned int frame)
{
    // %o: output file name in full
    // %s: scene name without path or extension
    // %n: frame number
    // %k: clock value
    // %h: height
    // %w: width
    // %%: percent character

    parameters.clear();
    for (const char *cmd = rawParameters.c_str(); *cmd; cmd++)
    {
        if (*cmd != '%')
        {
            parameters.push_back(*cmd);
            continue;
        }
        switch (*++cmd)
        {
            case '%':
                parameters.append("%%");
                break;

            case 'o':
                parameters.append(ofn);
                break;

            case 's':
                parameters.append(scene);
                break;

            case 'n':
                parameters.append(str(boost::format("%u") % frame));
                break;

            case 'k':
                parameters.append(str(boost::format("%#.6f") % clock));
                break;

            case 'h':
                parameters.append(str(boost::format("%u") % h));
                break;

            case 'w':
                parameters.append(str(boost::format("%u") % w));
                break;

            default:
                parameters.push_back('%');
                parameters.push_back(*cmd);
                break;
        }
    }
}

// ------------------------------------------------------------------

ShelloutProcessing::ShelloutProcessing(POVMS_Object& opts, const std::string& scene, unsigned int width, unsigned int height): sceneName(scene), imageWidth(width), imageHeight(height)
{
    skipReturn = cancelReturn = exitCode = 0;
    clockVal = 0.0;
    frameNo = 0;
    hadPreScene = hadPostScene = killRequested = skipCallouts = cancelRender = skipNextFrame = skipAllFrames = false;
    commandProhibited = processStartRequested = hadUserAbort = hadFatalError = false;
    postProcessEvent = lastShelloutEvent;
    cancelFormat.exceptions(boost::io::all_error_bits ^ boost::io::too_many_args_bit);
    skipFormat.exceptions(boost::io::all_error_bits ^ boost::io::too_many_args_bit);
    shellouts[preFrame].reset(new ShelloutAction(this, kPOVAttrib_PreFrameCommand, opts));
    shellouts[postFrame].reset(new ShelloutAction(this, kPOVAttrib_PostFrameCommand, opts));
    shellouts[preScene].reset(new ShelloutAction(this, kPOVAttrib_PreSceneCommand, opts));
    shellouts[postScene].reset(new ShelloutAction(this, kPOVAttrib_PostSceneCommand, opts));
    shellouts[userAbort].reset(new ShelloutAction(this, kPOVAttrib_UserAbortCommand, opts));
    shellouts[fatalError].reset(new ShelloutAction(this, kPOVAttrib_FatalErrorCommand, opts));
}

ShelloutProcessing::~ShelloutProcessing()
{
}

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
bool ShelloutProcessing::ExtractCommand(const std::string& src, std::string& command, std::string& parameters) const
{
    bool inSQ = false;
    bool inDQ = false;
    bool hadEscape = false;
    const char *s;

    command.clear();
    parameters.clear();
    std::string str = boost::trim_copy(src);
    for (s = str.c_str(); *s != '\0'; s++)
    {
        if (hadEscape)
        {
            hadEscape = false;
            command.push_back(*s);
        }
        else if (*s == '\\')
        {
            hadEscape = true;
        }
        else
        {
            if (*s == '"')
                inDQ = !inDQ;
            else if (*s == '\'')
                inSQ = !inSQ;
            else if (isspace(*s) && !inSQ && !inDQ)
                break;
            command.push_back(*s);
        }
    }

    boost::trim(command);
    if (command.empty())
        return false;

    if (command.size() > 1)
    {
        char ch1 = command[0];
        char ch2 = command[command.size() - 1];
        if ((ch1 == '\'' || ch1 == '"') && ch1 == ch2)
        {
            command = boost::trim_copy(command.substr(1, command.size() - 2));
            if (command.empty())
                return false;
        }
    }

    parameters = boost::trim_copy(std::string(s));
    return true;
}

std::string ShelloutProcessing::GetPhaseName(shelloutEvent event)
{
    switch (event)
    {
        case preScene:
            return "pre-scene";

        case postScene:
            return "post-scene";

        case preFrame:
            return "pre-frame";

        case postFrame:
            return "post-frame";

        case userAbort:
            return "user abort";

        case fatalError:
            return "fatal error";

        default:
            return "";
    }
}

bool ShelloutProcessing::HandleProcessEvent(shelloutEvent event, bool internalCall)
{
    if (hadPostScene)
        if (event < postScene)
            return cancelRender;
    if (!hadPreScene)
        event = preScene;

    bool skipWasSet(skipCallouts);
    std::string phaseName(GetPhaseName(event));
    ShelloutPtr sh(shellouts[event]);

    switch (event)
    {
        case preScene:
            if (hadPreScene)
                return cancelRender;
            hadPreScene = true;
            phaseName = "pre-scene";
            break;

        case postScene:
            if (hadPostScene)
                return cancelRender;
            hadPostScene = true;
            phaseName = "post-scene";
            break;

        case preFrame:
            if (skipNextFrame || skipAllFrames)
                return cancelRender;
            phaseName = "pre-frame";
            break;

        case postFrame:
            if (skipNextFrame || skipAllFrames)
                return cancelRender;
            phaseName = "post-frame";
            break;

        case userAbort:
            if (hadUserAbort)
                return cancelRender;
            hadUserAbort = true;
            phaseName = "user abort";
            break;

        case fatalError:
            // if cancel render is already set, we return unless this is an internal call
            if (cancelRender && !internalCall)
                return true;
            cancelRender = true;
            skipCallouts = true;
            exitCode = 1;
            phaseName = "fatal error";
            break;

        default:
            return cancelRender;
    }

    // skipCallouts is set if an event return indicated we should exit with no further shellouts.
    // skipWasSet is initialized to its value before the above switch
    if (skipWasSet)
        return cancelRender;

    // if there's no command set or shellouts are not supported, just return
    if (!sh->IsSet() || !ShelloutsSupported())
        return cancelRender;

    processStartRequested = true;
    runningProcessName = sh->Command();
    postProcessEvent = event;
    commandProhibited = false;

    // perform the substitutions on the parameter list
    sh->ExpandParameters(sceneName, outputFile, imageWidth, imageHeight, clockVal, frameNo);

    // make sure the command is allowed to run
    if (!CommandPermitted(sh->Command(), sh->Parameters()))
    {
        commandProhibited = true;
        throw POV_EXCEPTION(kCannotOpenFileErr, str(boost::format("Execution of shellout '%1%' prohibited") % sh->Command()));
    }

    try
    {
        ExecuteCommand(sh->Command(), sh->Parameters());
    }
    catch(...)
    {
        skipCallouts = true;
        throw;
    }

    return cancelRender;
}

bool ShelloutProcessing::PostProcessEvent(void)
{
    int             ret;
    std::string     output;
    shelloutEvent   event = postProcessEvent;
    std::string     phaseName(GetPhaseName(event));

    processStartRequested = false;
    runningProcessName.clear();
    postProcessEvent = lastShelloutEvent;

    if (event == lastShelloutEvent)
        return cancelRender; // nothing to do

    // we always call collect, even if commandProhibited is true
    // if collect returns 0 and the command was a prohibited one, we set it to 1
    if ((ret = CollectCommand(output)) == 0)
        if (commandProhibited)
            ret = 1;

    // no need to do anything more if a kill was requested
    if (killRequested)
        return true;

    // if the return action had the '!' or '-' prefix, we toggle the meaning of the result.
    bool execResult = ret == 0;
    ShelloutPtr sh(shellouts[event]);
    if (sh->ReturnNegate())
        execResult = !execResult;
    if (execResult == false)
    {
        ShelloutAction::Action action = sh->ReturnAction();
        if (action != ShelloutAction::ignore)
        {
            if (event < userAbort)
            {
                // common case for preScene, postScene, preFrame and postFrame
                switch (action)
                {
                    case ShelloutAction::quit:
                        // stop render immediately, exit code is 0
                        skipCallouts = cancelRender = true;
                        cancelPhase = phaseName;
                        cancelReason = "quit rendering";
                        cancelReturn = ret;
                        cancelCommand = sh->Command();
                        cancelParameters = sh->Parameters();
                        cancelOutput = LastShelloutResult();
                        exitCode = 0;
                        return true;

                    case ShelloutAction::abort:
                        // call abort, exit code is 2
                        cancelRender = true;
                        exitCode = 2;
                        HandleProcessEvent(userAbort, true);
                        cancelPhase = phaseName;
                        cancelReason = "generate a user abort";
                        cancelReturn = ret;
                        cancelCommand = sh->Command();
                        cancelParameters = sh->Parameters();
                        cancelOutput = LastShelloutResult();
                        return true;

                    case ShelloutAction::fatal:
                        // call fatal error, exit code is 1
                        cancelRender = true;
                        exitCode = 1;
                        HandleProcessEvent(fatalError, true);
                        cancelPhase = phaseName;
                        cancelReason = "generate a fatal error";
                        cancelReturn = ret;
                        cancelCommand = sh->Command();
                        cancelParameters = sh->Parameters();
                        cancelOutput = LastShelloutResult();
                        return true;

                    default:
                        // Do nothing special.
                        break;
                }
            }

            switch (event)
            {
                case preScene:
                    switch (action)
                    {
                        case ShelloutAction::skipOne:
                        case ShelloutAction::skipAll:
                            // stop render immediately, exit code is 0
                            cancelRender = true;
                            exitCode = 0;
                            skipPhase = cancelPhase = phaseName;
                            skipReason = cancelReason = "skip all frames";
                            skipReturn = cancelReturn = ret;
                            skipCommand = cancelCommand = sh->Command();
                            skipParameters = cancelParameters = sh->Parameters();
                            skipOutput = cancelOutput = LastShelloutResult();

                            // if result was skipAll, don't call post-scene
                            if (action == ShelloutAction::skipAll)
                                skipCallouts = true;

                            return true;

                        default:
                            // Do nothing special.
                            break;
                    }
                    break;

                case preFrame:
                    switch (action)
                    {
                        case ShelloutAction::skipOne:
                        case ShelloutAction::skipAll:
                            skipPhase = phaseName;
                            skipReturn = ret;
                            skipCommand = sh->Command();
                            skipParameters = sh->Parameters();
                            skipOutput = LastShelloutResult();
                            if (action == ShelloutAction::skipAll)
                            {
                                cancelRender = skipAllFrames = true;
                                skipReason = "skip all remaining frames";
                                exitCode = 0;
                                cancelPhase = phaseName;
                                cancelReason = "skip all frames";
                                cancelReturn = ret;
                                cancelCommand = sh->Command();
                                cancelParameters = sh->Parameters();
                                cancelOutput = LastShelloutResult();
                            }
                            else
                            {
                                skipNextFrame = true;
                                skipReason = str(boost::format("skip frame %1%") % (frameNo + 1));
                            }
                            break;

                        default:
                            // Do nothing special.
                            break;
                    }
                    break;

                case postFrame:
                    switch (action)
                    {
                        case ShelloutAction::skipOne:
                        case ShelloutAction::skipAll:
                            cancelRender = skipAllFrames = true;
                            cancelPhase = skipPhase = phaseName;
                            cancelReturn = skipReturn = ret;
                            cancelCommand = skipCommand = sh->Command();
                            cancelParameters = skipParameters = sh->Parameters();
                            cancelOutput = skipOutput = LastShelloutResult();
                            cancelReason = skipReason = "skip all remaining frames";
                            break;

                        default:
                            // Do nothing special.
                            break;
                    }
                    break;

                case userAbort:
                    cancelRender = true;
                    exitCode = 2;
                    cancelPhase = phaseName;
                    cancelReason = "generate a user abort";
                    cancelCommand = sh->Command();
                    cancelParameters = sh->Parameters();
                    cancelOutput = LastShelloutResult();
                    if (action == ShelloutAction::fatal)
                    {
                        // call fatal error
                        exitCode = 1;
                        HandleProcessEvent(fatalError, true);
                        cancelReason = "generate a fatal error";
                    }
                    cancelReturn = ret;
                    return true;

                default:
                    // Do nothing special.
                    break;
            }
        }
    }

    return cancelRender;
}

// returns true if a shellout is currently running. if this method is being called in an
// event loop to wait until a shellout has completed, it is the responsibility of the event
// loop to perform appropriate sleeps to avoid wasting CPU time.
bool ShelloutProcessing::ShelloutRunning(void)
{
    bool result = CommandRunning();

    if (processStartRequested == true && result == false)
        PostProcessEvent();
    return result;
}

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
std::string ShelloutProcessing::GetCancelMessage(void)
{
    return str(
        cancelFormat % cancelPhase
                     % exitCode
                     % ExitDesc()
                     % boost::to_lower_copy(ExitDesc())
                     % cancelReason
                     % cancelCommand
                     % cancelParameters
                     % cancelReturn
                     % cancelOutput);
}

// the message is constructed as per the documentation for the boost::format class.
// the positional parameters are as follows:
//   1: the event causing the skip (as a string), e.g. "pre-scene"
//   2: the type of the skip (as a string); e.g. "skip frame 11" or "skip all frames"
//   3: the command name that generated the skip
//   4: the command parameters (CAUTION: may contain escape codes)
//   5: the command return code (as an integer)
//   6: output text from the command, as returned by LastShelloutResult()
std::string ShelloutProcessing::GetSkipMessage(void)
{
    return str(
        skipFormat % skipPhase
                   % skipReason
                   % skipCommand
                   % skipParameters
                   % skipReturn
                   % skipOutput);
}

// shutdown any currently-running shellouts. if force is true, force them to exit.
// in either case, don't wait more than timeout seconds. return true if there are
// no more processes running afterwards.
bool ShelloutProcessing::KillShellouts(int timeout, bool force)
{
    killRequested = true;
    return KillCommand(timeout, force);
}

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
bool ShelloutProcessing::ExecuteCommand(const std::string& cmd, const std::string& params)
{
    throw POV_EXCEPTION(kCannotOpenFileErr, "Shellouts not implemented on this platform");
}

}
// end of namespace pov_frontend
