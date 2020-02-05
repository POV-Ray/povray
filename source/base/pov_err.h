//******************************************************************************
///
/// @file base/pov_err.h
///
/// Declaration of identifiers for internal errors.
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

#ifndef POVRAY_BASE_POV_ERR_H
#define POVRAY_BASE_POV_ERR_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "base/configbase.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <stdexcept>
#include <string>

// POV-Ray header files (base module)
//  (none at the moment)

namespace pov_base
{

//##############################################################################
///
/// @defgroup PovBasePovErr Error Handling
/// @ingroup PovBase
///
/// @{

// fatal errors
enum
{
    kNoError                  = 0,
    kNoErr                    = kNoError,
    kParamErr                 = -1,
    kMemFullErr               = -2,
    kOutOfMemoryErr           = kMemFullErr,
    kInvalidDataSizeErr       = -3,
    kCannotHandleDataErr      = -4,
    kNullPointerErr           = -5,
    kChecksumErr              = -6,
    kParseErr                 = -7,
    kCannotOpenFileErr        = -8,
    kInvalidDestAddrErr       = -9,
    kCannotConnectErr         = -10,
    kDisconnectedErr          = -11,
    kHostDisconnectedErr      = -12,
    kNetworkDataErr           = -13,
    kNetworkConnectionErr     = -14,
    kObjectAccessErr          = -15,
    kVersionErr               = -16,
    kFileDataErr              = -17,
    kAuthorisationErr         = -18,
    kDataTypeErr              = -19,
    kTimeoutErr               = -20,
    kInvalidContextErr        = -21,
    kIncompleteDataErr        = -22,
    kInvalidIdentifierErr     = -23,
    kCannotHandleRequestErr   = -24,
    kImageAlreadyRenderedErr  = -25,
    kAccessViolationErr       = -26,
    kDivideByZeroErr          = -27,
    kStackOverflowErr         = -28,
    kNativeExceptionErr       = -29,
    kInternalLimitErr         = -30,
    kUncategorizedError       = -31,
    kNumericalLimitErr        = -32,
};

// non fatal errors
enum
{
    kFalseErr                 = 1,
    kOutOfSyncErr             = 2,
    kNotNowErr                = kOutOfSyncErr,
    kQueueFullErr             = 3,
    kUserAbortErr             = 4
};

/**
 *  Macro which builds an exception with file name, line and function.
 *  Requires an error message text string to be specified.
 */
#define POV_EXCEPTION_STRING(str) pov_base::Exception(__FUNCTION__, __FILE__, __LINE__, (str))

/**
 *  Macro which builds an exception with file name, line and function.
 *  Requires an error code to be specified.
 *  The string will be derived from the error code.
 */
#define POV_EXCEPTION_CODE(err) pov_base::Exception(__FUNCTION__, __FILE__, __LINE__, (err))

/**
 *  Macro which builds an exception with file name, line and function.
 *  Requires an error code and string to be specified.
 */
#define POV_EXCEPTION(err,str) pov_base::Exception(__FUNCTION__, __FILE__, __LINE__, (err), (str))

/**
 *  POV-Ray exception class.
 */
class Exception : public std::runtime_error
{
    public:
        /**
         *  Create a new exception without location information.
         *  @param  str         Error message string.
         */
        Exception(const char *str) noexcept :
            std::runtime_error(str),
            xfunction(nullptr), xfile(nullptr), xline(0),
            xcode(0), xcodevalid(false), xfrontendnotified(false)
        {}

        /**
         *  Create a new exception with location information; looks up message from code.
         *  @param  fn          `__FUNCTION__` or `nullptr`
         *  @param  fi          `__FILE__`
         *  @param  li          `__LINE__`
         *  @param  err         Error number.
         */
        Exception(const char *fn, const char *fi, unsigned int li, int err) noexcept :
            std::runtime_error(Exception::lookup_code(err, fi, li)),
            xfunction(fn), xfile(fi), xline(li),
            xcode(err), xcodevalid(true), xfrontendnotified(false)
        {}

        /**
         *  Create a new exception with code, location information, and an explicit message.
         *  @param  fn          `__FUNCTION__` or `nullptr`
         *  @param  fi          `__FILE__`
         *  @param  li          `__LINE__`
         *  @param  str         Error message string.
         */
        Exception(const char *fn, const char *fi, unsigned int li, const char *str) noexcept :
            std::runtime_error(str),
            xfunction(fn), xfile(fi), xline(li),
            xcode(0), xcodevalid(false), xfrontendnotified(false)
        {}

        /**
         *  Create a new exception with code, location information, and an explicit message.
         *  @param  fn          `__FUNCTION__` or `nullptr`
         *  @param  fi          `__FILE__`
         *  @param  li          `__LINE__`
         *  @param  str         Error message string.
         */
        Exception(const char *fn, const char *fi, unsigned int li, const std::string& str) noexcept :
            std::runtime_error(str),
            xfunction(fn), xfile(fi), xline(li),
            xcode(0), xcodevalid(false), xfrontendnotified(false)
        {}

        /**
         *  Create a new exception with code, location information, and an explicit message.
         *  @param  fn          `__FUNCTION__` or `nullptr`
         *  @param  fi          `__FILE__`
         *  @param  li          `__LINE__`
         *  @param  err         Error number.
         *  @param  str         Error message string.
         */
        Exception(const char *fn, const char *fi, unsigned int li, int err, const char *str) noexcept :
            std::runtime_error(str),
            xfunction(fn), xfile(fi), xline(li),
            xcode(err), xcodevalid(true), xfrontendnotified(false)
        {}

        /**
         *  Create a new exception with code, location information, and an explicit message.
         *  @param  fn          `__FUNCTION__` or `nullptr`
         *  @param  fi          `__FILE__`
         *  @param  li          `__LINE__`
         *  @param  err         Error number.
         *  @param  str         Error message string.
         */
        Exception(const char *fn, const char *fi, unsigned int li, int err, const std::string& str) noexcept :
            std::runtime_error(str),
            xfunction(fn), xfile(fi), xline(li),
            xcode(err), xcodevalid(true), xfrontendnotified(false)
        {}

        /**
         *  Destructor.
         */
        virtual ~Exception() noexcept override { }

        /**
         *  Determine the function name where the exception occurred.
         *  @return             Function name or `nullptr`.
         */
        const char *function() const { return xfunction; }

        /**
         *  Determine the name of the source file where the exception occurred.
         *  @return             File name or `nullptr`.
         */
        const char *file() const { return xfile; }

        /**
         *  Determine the line number in the source file where the exception occurred.
         *  @return             File line or 0.
         */
        unsigned int line() const { return xline; }

        /**
         *  Determine the error code.
         *  @return             Error code (n.b. 0 is a legal errorcode)
         */
        int code() const { return xcode; }

        /**
         *  Determine the error code or default value if code not supplied at construction.
         *  @return             Error code (n.b. 0 is a legal errorcode)
         */
        int code(int defval) const { return xcodevalid ? xcode : defval; }

        /**
         *  Find out if an errorcode was supplied
         *  @return             true if this exception was created with an int param
         */
        bool codevalid() const { return xcodevalid; }

        /**
         *  Find out if the front-end has been notified about this exception
         *  @return             true if the front-end has been told about the exception
         */
        bool frontendnotified(void) const { return xfrontendnotified; }

        /**
         *  Set the front-end notification flag
         *  @param  notified    true to indicate notification has been passed on
         *  @return             previous value of notification flag
         */
        bool frontendnotified(bool notified) { bool oldval = xfrontendnotified; xfrontendnotified = notified; return oldval; }

        /**
         *  Return the error description for the given code.
         *  @param  err         Error code.
         *  @return             Error string for error code.
         */
        static std::string lookup_code(int err, const char *file = nullptr, unsigned int line = 0);

    private:
        /// Function where the exception occurred
        const char *xfunction;
        /// File name of the source file where the exception occurred
        const char *xfile;
        // Line number in the source file where the exception occurred
        unsigned int xline;
        // Error code
        int xcode;
        // flag so we can tell if this exception was created with an errorcode
        bool xcodevalid;
        // flag that tells us if this exception has been notified to the front-end
        bool xfrontendnotified;
};

/// @}
///
//##############################################################################

}
// end of namespace pov_base

#endif // POVRAY_BASE_POV_ERR_H
