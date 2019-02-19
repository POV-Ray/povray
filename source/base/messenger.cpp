//******************************************************************************
///
/// @file base/messenger.cpp
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
#include "base/messenger.h"

// C++ variants of C standard header files
#include <cstdarg>
#include <cstdio>
#include <cstring>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

GenericMessenger::GenericMessenger(unsigned int wl, const char *sn) :
    warningLevel(wl),
    stageName(sn)
{}

GenericMessenger::~GenericMessenger()
{}

void GenericMessenger::UserDebug(const char *text)
{
    SendMessage(kMessageClass_UserDebug, kWarningNone, text);
}

void GenericMessenger::Info(const char *format, ...)
{
    WarningLevel level = kWarningGeneral;
    if(warningLevel < level)
        return;

    va_list marker;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "%s Info: ", stageName);

    va_start(marker, format);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_Info, level, localvsbuffer);
}

void GenericMessenger::InfoAt(const MessageContext& context, const char *format, ...)
{
    WarningLevel level = kWarningGeneral;
    if(warningLevel < level)
        return;

    va_list marker;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "%s Info: ", stageName);

    va_start(marker, format);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_Info, level, localvsbuffer, context);
}

void GenericMessenger::InfoAt(const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...)
{
    WarningLevel level = kWarningGeneral;
    if(warningLevel < level)
        return;

    va_list marker;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "%s Info: ", stageName);

    va_start(marker, format);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_Info, level, localvsbuffer, filename, line, column, offset);
}

void GenericMessenger::Warning(WarningLevel level, const char *format,...)
{
    if(warningLevel < level)
        return;

    va_list marker;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "%s Warning: ", stageName);

    va_start(marker, format);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_Warning, level, localvsbuffer);
}

void GenericMessenger::WarningAt(WarningLevel level, const MessageContext& context, const char *format, ...)
{
    if(warningLevel < level)
        return;

    va_list marker;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "%s Warning: ", stageName);

    va_start(marker, format);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_Warning, level, localvsbuffer, context);
}

void GenericMessenger::WarningAt(WarningLevel level, const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...)
{
    if(warningLevel < level)
        return;

    va_list marker;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "%s Warning: ", stageName);

    va_start(marker, format);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_Warning, level, localvsbuffer, filename, line, column, offset);
}

void GenericMessenger::PossibleError(const char *format,...)
{
    if(warningLevel == 0)
        return;

    va_list marker;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "Possible %s Error: ", stageName);

    va_start(marker, format);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_PossibleError, kWarningNone, localvsbuffer);
}

void GenericMessenger::PossibleErrorAt(const MessageContext& context, const char *format, ...)
{
    if(warningLevel == 0)
        return;

    va_list marker;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "Possible %s Error: ", stageName);

    va_start(marker, format);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_PossibleError, kWarningNone, localvsbuffer, context);
}

void GenericMessenger::PossibleErrorAt(const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...)
{
    if(warningLevel == 0)
        return;

    va_list marker;
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "Possible %s Error: ", stageName);

    va_start(marker, format);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, marker);
    va_end(marker);

    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_PossibleError, kWarningNone, localvsbuffer, filename, line, column, offset);
}

void GenericMessenger::SendMessage(MessageClass mc, WarningLevel level, const char *text, const MessageContext& context)
{
    SendMessage(mc, level, text, context.GetFileName(), context.GetLine(), context.GetColumn(), context.GetOffset());
}

std::string GenericMessenger::SendError(const char *format, va_list arglist, const MessageContext& context)
{
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "%s Error: ", stageName);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, arglist);
    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_Error, kWarningNone, localvsbuffer, context);

    return std::string(localvsbuffer);
}

// filename defaults to `nullptr`, and line, column, and offset default to -1
std::string GenericMessenger::SendError(const char *format, va_list arglist, const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset)
{
    char localvsbuffer[1024];

    sprintf(localvsbuffer, "%s Error: ", stageName);
    std::vsnprintf(localvsbuffer + strlen(localvsbuffer), sizeof(localvsbuffer) - strlen(localvsbuffer), format, arglist);
    CleanupString(localvsbuffer);

    SendMessage(kMessageClass_Error, kWarningNone, localvsbuffer, filename, line, column, offset);

    return std::string(localvsbuffer);
}

void GenericMessenger::Error(const char *format, ...)
{
    va_list marker;

    va_start(marker, format);
    std::string text = SendError(format, marker);
    va_end(marker);

    // Terminate by throwing an exception with the notification flag already set
    pov_base::Exception ex(__FUNCTION__, __FILE__, __LINE__, text); // TODO - this location information isn't too helpful
    ex.frontendnotified(true);
    throw ex;
}

void GenericMessenger::Error(const Exception& ex, const char *format, ...)
{
    va_list marker;

    // if the front-end has been told about this exception already, we don't want to tell it again
    // (we presume that the text given by format and its parameters relate to that exception)
    // this form of frontendnotified() doesn't change the state of ex
    if (ex.frontendnotified())
        throw ex;

    va_start(marker, format);
    SendError(format, marker);
    va_end(marker);

    // now take a copy of ex and set its notification flag, then throw that.
    pov_base::Exception local_ex(ex);
    local_ex.frontendnotified(true);
    throw local_ex;
}

void GenericMessenger::Error(Exception& ex, const char *format, ...)
{
    va_list marker;

    // if the front-end has been told about this exception already, we don't want to tell it again
    // (we presume that the text given by format and its parameters relate to that exception)
    // this form of frontendnotified() sets the notified state of ex
    if (ex.frontendnotified(true))
        throw ex;

    va_start(marker, format);
    SendError(format, marker);
    va_end(marker);

    // Terminate
    throw ex;
}

void GenericMessenger::ErrorAt(const MessageContext& context, const char *format, ...)
{
    va_list marker;

    va_start(marker, format);
    std::string text = SendError(format, marker, context);
    va_end(marker);

    // Terminate by throwing an exception with the notification flag already set
    pov_base::Exception ex(__FUNCTION__, __FILE__, __LINE__, text); // TODO - this location information isn't too helpful
    ex.frontendnotified(true);

    throw ex;
}

void GenericMessenger::ErrorAt(const Exception& ex, const MessageContext& context, const char *format, ...)
{
    va_list marker;

    if (ex.frontendnotified())
        throw ex;

    va_start(marker, format);
    SendError(format, marker, context);
    va_end(marker);

    pov_base::Exception local_ex(ex);
    local_ex.frontendnotified(true);
    throw local_ex;
}

void GenericMessenger::ErrorAt(Exception& ex, const MessageContext& context, const char *format, ...)
{
    va_list marker;

    if (ex.frontendnotified(true))
        throw ex;

    va_start(marker, format);
    SendError(format, marker, context);
    va_end(marker);

    throw ex;
}

void GenericMessenger::ErrorAt(const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...)
{
    va_list marker;

    va_start(marker, format);
    std::string text = SendError(format, marker, filename, line, column, offset);
    va_end(marker);

    // Terminate by throwing an exception with the notification flag already set
    pov_base::Exception ex(__FUNCTION__, __FILE__, __LINE__, text); // TODO - this location information isn't too helpful
    ex.frontendnotified(true);

    throw ex;
}

void GenericMessenger::ErrorAt(const Exception& ex, const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...)
{
    va_list marker;

    if (ex.frontendnotified())
        throw ex;

    va_start(marker, format);
    SendError(format, marker, filename, line, column, offset);
    va_end(marker);

    pov_base::Exception local_ex(ex);
    local_ex.frontendnotified(true);
    throw local_ex;
}

void GenericMessenger::ErrorAt(Exception& ex, const UCS2String& filename, POV_LONG line, POV_LONG column, POV_OFF_T offset, const char *format, ...)
{
    va_list marker;

    if (ex.frontendnotified(true))
        throw ex;

    va_start(marker, format);
    SendError(format, marker, filename, line, column, offset);
    va_end(marker);

    throw ex;
}

void GenericMessenger::CleanupString(char *str)
{
    while(*str != 0)
    {
        if(*str == '\n')
            *str = ' ';
        str++;
    }
}

}
// end of namespace pov_base
