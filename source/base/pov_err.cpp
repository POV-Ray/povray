//******************************************************************************
///
/// @file base/pov_err.cpp
///
/// Implementation of message text generation for internal errors.
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
#include "base/pov_err.h"

// C++ variants of C standard header files
#include <cstdio>
#include <cstring>

// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_base
{

std::string Exception::lookup_code(int err, const char *file, unsigned int line)
{
    char    str[256];

    // skip over part of filename, if present, to make output more compact.
    if ((file != nullptr) && ((strncmp(file, "../../source/", 13) == 0) || (strncmp(file, "..\\..\\source\\", 13) == 0)))
        file += 13;

    switch(err)
    {
        case kNoErr:
            if (file == nullptr)
                return "(Failed to determine error: no code found.)";
            sprintf(str, "(Failed to determine error: no code in exception thrown at %s line %d.)", file, line);
            return std::string(str);

        case kParamErr:
            return "Invalid parameter.";

        case kMemFullErr:
            if (file == nullptr)
                return "Out of memory.";
            sprintf(str, "Memory allocation failure exception thrown at %s line %d.", file, line);
            return std::string(str);

        case kInvalidDataSizeErr:
            return "Invalid data size.";

        case kCannotHandleDataErr:
            return "Cannot handle data of this type or kind.";

        case kNullPointerErr:
            return "Unexpected null pointer.";

        case kChecksumErr:
            return "Corrupted data.";

        case kParseErr:
            return "Cannot parse input.";

        case kCannotOpenFileErr:
            return "Cannot open file.";

        case kInvalidDestAddrErr:
            return "Invalid destination address.";

        case kCannotConnectErr:
            return "Cannot establish connection.";

        case kDisconnectedErr:
            return "Disconnection initiated from client.";

        case kHostDisconnectedErr:
            return "Disconnection initiated from host.";

        case kNetworkDataErr:
            return "Network data transmission failed.";

        case kNetworkConnectionErr:
            return "Network connection failed";

        case kObjectAccessErr:
            return "Cannot access object.";

        case kVersionErr:
            return "Invalid version.";

        case kFileDataErr:
            return "Cannot access data in file.";

        case kAuthorisationErr:
            return "Unauthorised access";

        case kDataTypeErr:
            return "Data type or kind does not match.";

        case kTimeoutErr:
            return "Operation timed out.";

        case kInvalidContextErr:
            return "Context is invalid.";

        case kIncompleteDataErr:
            return "Cannot handle data because it is incomplete.";

        case kInvalidIdentifierErr:
            return "Invalid identifier.";

        case kCannotHandleRequestErr:
            return "Cannot handle request.";

        case kFalseErr:
            return "Result was false but true was expected.";

        case kOutOfSyncErr:
            return "Cannot perform this operation now.";

        case kQueueFullErr:
            return "Queue is full.";

        case kUserAbortErr:
            return "Frontend halted render.";

        case kImageAlreadyRenderedErr:
            return "Image already rendered.";

        case kAccessViolationErr:
            return "Memory access violation.";

        case kDivideByZeroErr:
            return "Divide by zero.";

        case kStackOverflowErr:
            return "Stack overflow.";

        case kNativeExceptionErr:
            return "Native operating-system exception.";

        case kInternalLimitErr:
            return "A POV-Ray internal nesting limit was reached.";

        case kUncategorizedError:
            if (file == nullptr)
                return "Uncategorized error.";
            sprintf(str, "Uncategorized error thrown at %s line %d.", file, line);
            return std::string(str);

        case kNumericalLimitErr:
            return "A POV-Ray internal numerical limit was reached.";
    }

    // default
    sprintf(str, "(Failed to determine error: unidentified code %d in exception thrown at %s line %d. Please report this.)", err, file, line);
    return std::string(str);
}

}
// end of namespace pov_base
