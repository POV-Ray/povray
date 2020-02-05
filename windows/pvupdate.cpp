//******************************************************************************
///
/// @file windows/pvupdate.cpp
///
/// This module implements update checking routines.
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

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#include <windows.h>
#include <wininet.h>
#include <stdio.h>

#include "pvengine.h"

// this must be the last file included
#include "syspovdebug.h"

#pragma comment(lib, "wininet")

#define HTTPFLAGS       INTERNET_FLAG_NO_UI | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD

#ifndef INTERNET_CONNECTION_CONFIGURED
#define INTERNET_CONNECTION_CONFIGURED      0x40
#define INTERNET_CONNECTION_OFFLINE         0x20
#define INTERNET_RAS_INSTALLED              0x10
#endif

#ifndef SM_CMONITORS
#define SM_CMONITORS 80
#endif

#ifndef SM_REMOTESESSION
#define SM_REMOTESESSION 0x1000
#endif

#define SCRIPTPATH      "/updates/checkv2"

namespace povwin
{

static char *GetInstallTime (void)
{
  HKEY        key ;
  DWORD       len ;
  static char str [64] ;

  len = sizeof (str) ;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\POV-Ray", 0, KEY_READ, &key) == ERROR_SUCCESS)
  {
    if (RegQueryValueEx (key, INSTALLTIMEKEY, 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
    {
      RegCloseKey (key) ;
      return (str) ;
    }
    RegCloseKey (key) ;
  }
  return (NULL) ;
}

bool InternetConnected (void)
{
  DWORD       flags ;

  BOOL result = InternetGetConnectedState (&flags, 0) ;
  if ((flags & INTERNET_CONNECTION_OFFLINE) != 0)
    return (false) ;
  return (result != 0) ;
}

// -1 == error, 0 == no update, 1 == update
int IsUpdateAvailable (bool SendSysinfo, char *CurrentVersion, std::string& NewVersion, std::string& Info)
{
  int                   result = -1 ;
  char                  poststr [2048] ;
  char                  user_agent[128];
  char                  str[128];
  char                  *s = poststr ;
  char                  *InstalledOn ;
  HDC                   hdc ;
  HKEY                  key ;
  DWORD                 len;
  DWORD                 header = 0 ;
  DWORD                 n ;
  FILETIME              file_time ;
  SYSTEMTIME            system_time ;
  SYSTEM_INFO           sysinfo ;
  OSVERSIONINFO         version_info ;
  MEMORYSTATUSEX        mem_status ;

  if (!InternetConnected ())
    return (-1) ;
  if ((InstalledOn = GetInstallTime ()) == NULL)
  {
    GetSystemTime (&system_time) ;
    if (SystemTimeToFileTime (&system_time, &file_time))
      reg_printf (true, "Software\\POV-Ray", INSTALLTIMEKEY, "%I64u", ((__int64) file_time.dwHighDateTime << 32) | file_time.dwLowDateTime) ;
    if ((InstalledOn = GetInstallTime ()) == NULL)
      InstalledOn = "Unknown" ;
  }
  sprintf (user_agent, "POVWIN %s", CurrentVersion) ;
  HINTERNET iHandle = InternetOpen (user_agent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0) ;
  if (iHandle == NULL)
    return (-1) ;
  HINTERNET cHandle = InternetConnect (iHandle, "winupdate.povray.org", 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0) ;
  if (cHandle == NULL)
  {
    InternetCloseHandle (iHandle) ;
    return (-1) ;
  }
  HINTERNET hHandle = HttpOpenRequest (cHandle, "POST", SCRIPTPATH, NULL, NULL, NULL, HTTPFLAGS, 0) ;
  if (hHandle == NULL)
  {
    InternetCloseHandle (iHandle) ;
    InternetCloseHandle (cHandle) ;
    return (-1) ;
  }
  if (InstalledOn == NULL)
    InstalledOn = "Unknown" ;
  s += sprintf (s, "CurrentVersion=%s\n", CurrentVersion) ;
  s += sprintf (s, "InstallDate=%s\n", InstalledOn) ;
  if (SendSysinfo)
  {
    strcpy (s, "&NoInfo=false\n") ;
    GetSystemInfo (&sysinfo) ;
    s += sprintf (s, "CPUArchitecture=0x%04x\n", (DWORD) sysinfo.wProcessorArchitecture) ;
    s += sprintf (s, "NumberOfCPUs=0x%04x\n", sysinfo.dwNumberOfProcessors) ;
    s += sprintf (s, "ProcessorType=0x%04x\n", sysinfo.dwProcessorType) ;
    s += sprintf (s, "ProcessorLevel=0x%04x\n", (DWORD) sysinfo.wProcessorLevel) ;
    s += sprintf (s, "ProcessorRevision=0x%04x\n", (DWORD) sysinfo.wProcessorRevision) ;

    version_info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO) ;
    GetVersionEx (&version_info) ;

    s += sprintf (s, "OSVersion=%u.%u\n", version_info.dwMajorVersion, version_info.dwMinorVersion) ;
    s += sprintf (s, "OSBuild=0x%08x\n", version_info.dwBuildNumber) ;
    s += sprintf (s, "CSDVersion=%s\n", version_info.szCSDVersion) ;

    hdc = GetDC (NULL) ;
    s += sprintf (s, "BitsPerPixel=%u\n", GetDeviceCaps (hdc, BITSPIXEL)) ;
    s += sprintf (s, "HorzRes=%u\n", GetDeviceCaps (hdc, HORZRES)) ;
    s += sprintf (s, "VertRes=%u\n", GetDeviceCaps (hdc, VERTRES)) ;
    ReleaseDC (NULL, hdc) ;

    s += sprintf (s, "NumberOfMonitors=%u\n", GetSystemMetrics (SM_CMONITORS)) ;
    s += sprintf (s, "HasMouseWheel=%u\n", GetSystemMetrics (SM_MOUSEWHEELPRESENT)) ;
    s += sprintf (s, "Remote=%u\n", GetSystemMetrics (SM_REMOTESESSION)) ;

    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "HARDWARE\\Description\\System\\CentralProcessor\\0", 0, KEY_READ, &key) == ERROR_SUCCESS)
    {
      len = sizeof (n) ;
      if (RegQueryValueEx (key, "~MHZ", 0, NULL, (BYTE *) &n, &len) == ERROR_SUCCESS)
        s += sprintf (s, "CPUFrequency=%u\n", n) ;

      len = sizeof (n) ;
      if (RegQueryValueEx (key, "FeatureSet", 0, NULL, (BYTE *) &n, &len) == ERROR_SUCCESS)
        s += sprintf (s, "FeatureSet=0x%08x\n", n) ;

      len = sizeof (str) ;
      if (RegQueryValueEx (key, "ProcessorNameString", 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
        s += sprintf (s, "CPUName=%s\n", str) ;

      len = sizeof (str) ;
      if (RegQueryValueEx (key, "Identifier", 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
        s += sprintf (s, "CPUIdentifier=%s\n", str) ;

      len = sizeof (str) ;
      if (RegQueryValueEx (key, "VendorIdentifier", 0, NULL, (BYTE *) str, &len) == ERROR_SUCCESS)
        s += sprintf (s, "VendorIdentifier=%s\n", str) ;

      RegCloseKey (key) ;
    }

    mem_status.dwLength = sizeof (MEMORYSTATUSEX) ;
    GlobalMemoryStatusEx(&mem_status) ;
    s += sprintf (s, "PhysicalMemory=%I64u\n", mem_status.ullTotalPhys) ;

    if (GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_SLANGUAGE, str, sizeof (str)))
      s += sprintf (s, "DefaultLanguage=%s\n", str) ;
  }
  else
    strcpy (s, "NoInfo=true\n ") ;
  if (HttpSendRequest (hHandle, NULL, 0, poststr, (DWORD) strlen (poststr)))
  {
    result = -3;
    len = sizeof(str);
    if (HttpQueryInfo (hHandle, HTTP_QUERY_STATUS_CODE, str, &len, &header))
    {
      if (len == 3 && memcmp (str, "200", 3) == 0)
      {
        char *reply = new char[131072];
        if (InternetReadFile (hHandle, reply, 131071, &len))
        {
          reply[len] = '\0' ;
          result = 0 ;
          if (memcmp (reply, "YES ", 4) == 0)
          {
            result = 1 ;
            Info.clear();
            NewVersion = reply + 4;
            std::string::size_type pos = NewVersion.find(' ');
            if (pos != std::string::npos)
            {
              Info = NewVersion.substr(pos);
              NewVersion.resize(pos);
            }
            if (NewVersion.length() == 0)
              result = -4;
          }
          else if ((len == 2 && memcmp (reply, "NO", 2) == 0) || (len == 3 && memcmp (reply, "NO\n", 3) == 0))
            result = 0 ;
          else if ((len == 6 && memcmp (reply, "BADVER", 6) == 0) || (len == 7 && memcmp (reply, "BADVER\n", 7) == 0))
            result = -2 ;
          else
            result = -4 ;
        }
        delete[] reply;
      }
    }
  }
  InternetCloseHandle (hHandle) ;
  InternetCloseHandle (cHandle) ;
  InternetCloseHandle (iHandle) ;
  return (result) ;
}

}
// end of namespace povwin
