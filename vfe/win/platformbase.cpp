//******************************************************************************
///
/// @file vfe/win/platformbase.cpp
///
/// This file implements code that is needed (and linked to) the base code.
///
/// @author Christopher J. Cason
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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

#include <windows.h>
#include <time.h>
#include <sys/timeb.h>
#include "vfe.h"

// this must be the last file included
#include "base/povdebug.h"

////////////////////////////////////////////////////////////////////////////////////////
// Note: do not assume that the VFE code is linked with this!
//       any code in this file must be able to function when linked with the backend
//       alone; that is, if the backend is built into a stand-alone module with the
//       frontend for example using a network to connect to it.
////////////////////////////////////////////////////////////////////////////////////////

using namespace pov_base;

namespace povwin
{
  void WinMemThreadStartup();
  void WinMemThreadCleanup();
}

namespace vfePlatform
{
  extern bool GetCPUCount(unsigned int *TotAvailLogical, unsigned int *TotAvailCore, unsigned int *PhysicalNum);

  ////////////////////////////////////////////////////////////////////////////////////////
  //
  // internal functions
  //
  ////////////////////////////////////////////////////////////////////////////////////////

  static int GetNumberofCPUs (void)
  {
    unsigned              logical ;
    unsigned              cores ;
    unsigned              physical ;
    SYSTEM_INFO           sysinfo ;
    static int            result = -1;

    // we cache the result, since this function is called on each thread startup
    if (result != -1)
      return result;

    GetSystemInfo (&sysinfo) ;
    result = sysinfo.dwNumberOfProcessors;
    if (GetCPUCount(&logical, &cores, &physical))
      result = cores;
    return result;
  }

  static char *GetExceptionDescription (DWORD code)
  {
    switch (code)
    {
      case EXCEPTION_ACCESS_VIOLATION :
          return ("access violation") ;

      case EXCEPTION_DATATYPE_MISALIGNMENT :
          return ("datatype misalignment") ;

      case EXCEPTION_FLT_DENORMAL_OPERAND :
          return ("denormal floating point operand") ;

      case EXCEPTION_FLT_DIVIDE_BY_ZERO :
          return ("floating point divide by zero") ;

      case EXCEPTION_FLT_INEXACT_RESULT :
          return ("inexact floating-point result") ;

      case EXCEPTION_FLT_INVALID_OPERATION :
          return ("invlalid floating-point operation") ;

      case EXCEPTION_FLT_OVERFLOW :
          return ("floating-point overflow") ;

      case EXCEPTION_FLT_STACK_CHECK :
          return ("floating-point stack over/underflow") ;

      case EXCEPTION_FLT_UNDERFLOW :
          return ("floating-point underflow") ;

      case EXCEPTION_INT_DIVIDE_BY_ZERO :
          return ("integer divide by zero") ;

      case EXCEPTION_INT_OVERFLOW :
          return ("integer overflow") ;

      case EXCEPTION_PRIV_INSTRUCTION :
          return ("execution of privileged instruction") ;

      case EXCEPTION_IN_PAGE_ERROR :
          return ("page error") ;

      case EXCEPTION_ILLEGAL_INSTRUCTION :
          return ("execution of illegal instruction") ;

      case EXCEPTION_NONCONTINUABLE_EXCEPTION :
          return ("continuation after noncontinuable exception") ;

      case EXCEPTION_STACK_OVERFLOW :
          return ("stack overflow") ;

      case EXCEPTION_INVALID_DISPOSITION :
          return ("invalid disposition") ;

      case EXCEPTION_GUARD_PAGE :
          return ("guard page") ;

      case EXCEPTION_INVALID_HANDLE :
          return ("invalid handle") ;

      default :
          return ("Unknown exception code") ;
    }
  }
}

namespace pov_base
{
  using namespace vfePlatform;

  ////////////////////////////////////////////////////////////////////////////////////////
  //
  // thread support
  //
  ////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  // called by the base code each time a worker thread is created (the call
  // is made in the context of the new thread).
  void vfeSysThreadStartup(void)
  {
    static int count = 0 ;
    if (GetNumberofCPUs() > 1)
      SetThreadIdealProcessor(GetCurrentThread(), (count++ % GetNumberofCPUs()) + 1) ;
#ifndef _CONSOLE
    povwin::WinMemThreadStartup();
#endif
  }

  /////////////////////////////////////////////////////////////////////////
  // called by a worker thread just before it exits.
  void vfeSysThreadCleanup(void)
  {
#ifndef _CONSOLE
    povwin::WinMemThreadCleanup();
#endif
  }

  ////////////////////////////////////////////////////////////////////////////////////////
  //
  // path parsing
  //
  ////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////
  // The first argument is the input, a UCS2 string.
  //
  // The second argument is the string you are supposed to return the "volume"
  // name with.  For DOS-style paths this implies i.e. "A:\" is the "volume".
  // Note that it is essential that the first "path" separator is also part of
  // the volume name.  If the path is relative, the "volume" name shall be empty.
  //
  // This trick is necessary so the code can account for the lack of volume names
  // in Unix-style paths: In Unix, the POV_PARSE_PATH_STRING function will have
  // to take a reading "/" or "~/" as "volume" name.  This makes it possible to
  // determine if a string is absolute or relative based on this 'virtual'
  // "volume" name rather than some flags.
  //
  // The third is a vector of strings you have to return, each has to contain the
  // folder name from left to right, without the path separator, of course.
  //
  // The fourth argument shall contain the filename, if any was given in the
  // source string. By definition if the source string does not contain a
  // trailing path separator, whatever comes after the last path separator
  // (or the start of the string if there is none) must be considered a filename,
  // even if it could be a directory (in other words, don't call a system function
  // to find out if it is a dir or not - see below).
  //
  // Please note that the function must not attempt to determine the validity of
  // a string by accessing the filesystem.  It has to parse anything that it is
  // given.  If the string provided cannot be parsed for some reason (that is if
  // you can determine that a given path cannot possibly be valid i.e. because it
  // contains invalid characters), the function has to return false.  It may not
  // throw exceptions.  The return value for success is true.
  ////////////////////////////////////////////////////////////////////////////////////////

  #define UCS2toASCIIString POVMS_UCS2toASCIIString
  #define ASCIItoUCS2String POVMS_ASCIItoUCS2String

  bool vfeParsePathString (const UCS2String& path, UCS2String& volume, vector<UCS2String>& components, UCS2String& filename)
  {
    char str [MAX_PATH * 4] ;

    volume.clear() ;
    filename.clear() ;
    components.clear() ;

    if (path.empty() == true)
      return (true);
    if (path.size () >= sizeof (str))
      return (false) ;
    strcpy (str, UCS2toASCIIString (path).c_str ()) ;
    char lastch = str[strlen(str) - 1];

    // now determine if it's a network or drive path.
    // (we could use the shlwapi functions here but I'd rather avoid the shlwapi.dll dependency).
    char *p2 = str ;
    char *p3 ;
    if ((strlen (str) > 1) && ((str [1] == ':') || (str[0] == '\\' && str[1] == '\\') || (str[0] == '/' && str[1] == '/')))
    {
      if (str [1] == ':')
      {
        // if it's a drive reference the first character must be in range 'a' - 'z'
        if (!isalpha (str[0]))
          return (false) ;

        // currently we don't support relative paths if a volume is specified
        if ((str [2] != '\\') && (str [2] != '/'))
          return (false) ;
        volume = ASCIItoUCS2String (string (str).substr (0, 3).c_str()) ;
        p2 += 3 ;
      }
      else
      {
        // it's a UNC path ... look for the next separator
        p2 = strchr (str + 2, '\\');
        p3 = strchr (str + 2, '/');
        if ((p3 != NULL) && ((p2 == NULL) || (p2-str) > (p3-str)))
            p2 = p3;
        if (p2 == NULL)
        {
          // no separator; technically this is valid, but it's a relative reference
          // and as above we don't currently support this.
          return (false) ;
        }
        volume = ASCIItoUCS2String (string (str).substr (0, (size_t) (++p2 - str)).c_str()) ;
      }
    }
    else if ((str [0] == '\\') || (str [0] == '/'))
    {
      // it's a path relative to the root of the current drive.
      // we will use '\' as the volume name.
      // for volume-relative paths we also accept '/' as a path separator
      volume = ASCIItoUCS2String("\\");
      p2++;
    }

    // p2 now points at the start of any path or file components
    // the first call to strtok will skip over any extra separators
    // at the start of the path
    for (char *p1 = strtok (p2, "\\/"); p1 != NULL; p1 = strtok (NULL, "/\\"))
    {
      if (*p1 == '\0')
        continue;
      if (p1[0] == '.' && p1[1] == '\0')
        continue;
      if (p1[0] == '.' && p1[1] == '.' && p1[2] == '\0')
      {
        // it's a relative directory reference ... see if we can pop a
        // path from components; if not we leave it in there since it
        // is permitted to refer to a directory above the CWD
        if ((components.empty() == false) && (components.back() != ASCIItoUCS2String("..")))
        {
          components.pop_back();
          continue;
        }
      }
      components.push_back (ASCIItoUCS2String (p1)) ;
    }

    // the filename, if present, will be the last entry in components.
    // we first check the last character of the supplied path to see
    // if it's a path separator char; if it is there's no filename.
    if (lastch == '\\' || lastch == '/')
      return true;

    if (components.empty() == false)
    {
      filename = components.back();
      components.pop_back();
    }

    return true ;
  }

}
