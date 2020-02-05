//******************************************************************************
///
/// @file base/messenger.h
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

#ifndef POVRAY_BASE_MESSENGER_H
#define POVRAY_BASE_MESSENGER_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"
#include "base/messenger_fwd.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <string>

// POV-Ray header files (base module)
#include "base/base_fwd.h"
#include "base/stringtypes.h"

namespace pov_base
{

//##############################################################################
///
/// @addtogroup PovBase
///
/// @{

/// @relates GenericMessenger
enum MessageClass
{
    kMessageClass_UserDebug,        ///< Diagnostic information specified by the user.
    kMessageClass_InternalDebug,    ///< Diagnostic information to help in POV-Ray development.
    kMessageClass_Info,             ///< Information that is no reason for alarm.
    kMessageClass_Warning,          ///< Information about a potentially undesired and/or unexpected situation.
    kMessageClass_PossibleError,    ///< Information about a presumably erroneous but recoverable situation.
    kMessageClass_Error,            ///< Information about an erroneous unrecoverable situation.
};

/// @relates GenericMessenger
enum WarningLevel
{
    /// Placeholder value.
    kWarningNone     = -1,
    /// Value used for general warnings.
    kWarningGeneral  =  1,
    /// Value used for general language version specific warning.
    kWarningLanguage =  6
};

/// @relates GenericMessenger
class MessageContext
{
public:
    virtual ~MessageContext() {}
    virtual UCS2String GetFileName() const = 0;
    virtual POV_LONG GetLine() const = 0;
    virtual POV_LONG GetColumn() const = 0;
    virtual POV_OFF_T GetOffset() const = 0;
};

/// Abstract class providing an interface to report textual messages to the user.
class GenericMessenger
{
    public:

        GenericMessenger(unsigned int wl, const char *sn);
        virtual ~GenericMessenger();

        void UserDebug(const char *text);

        void Info(const char *format,...);
        void InfoAt(const MessageContext& context, const char *format, ...);
        void InfoAt(const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...);

        void Warning(WarningLevel level, const char *format,...);
        void WarningAt(WarningLevel level, const MessageContext& context, const char *format, ...);
        void WarningAt(WarningLevel level, const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...);

        void PossibleError(const char *format,...);
        void PossibleErrorAt(const MessageContext& context, const char *format, ...);
        void PossibleErrorAt(const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...);

        void Error(const char *format,...);
        void Error(const Exception& ex, const char *format,...);
        void Error(Exception& ex, const char *format,...);
        void ErrorAt(const MessageContext& context, const char *format, ...);
        void ErrorAt(const Exception& ex, const MessageContext& context, const char *format, ...);
        void ErrorAt(Exception& ex, const MessageContext& context, const char *format, ...);
        void ErrorAt(const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...);
        void ErrorAt(const Exception& ex, const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...);
        void ErrorAt(Exception& ex, const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...);

        void SetWarningLevel(unsigned int Val) { warningLevel = Val ; } // TODO FIXME - not here, not this way

    protected:

        virtual void SendMessage(MessageClass mc, WarningLevel level, const char *text,
                                 const UCS2String& filename = u"", POV_LONG line = -1, POV_LONG column = -1, POV_OFF_T offset = -1) = 0;

    private:

        unsigned int warningLevel;
        const char *stageName;

        void CleanupString(char *str);
        void SendMessage(MessageClass mc, WarningLevel level, const char *text, const MessageContext& context);
        std::string SendError(const char *format, va_list arglist, const MessageContext& context);
        std::string SendError(const char *format, va_list arglist,
                              const UCS2String& filename = u"", POV_LONG line = -1, POV_LONG column = -1, POV_OFF_T offset = -1);
};

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_MESSENGER_H
