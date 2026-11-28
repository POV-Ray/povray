//******************************************************************************
///
/// @file vfe/win/console/winconsole.cpp
///
/// This file contains a basic proof-of-concept POV implementation using VFE.
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

#include <cstring>

#include "base/builtinfiles.h"
#include "base/version_info.h"

#include "backend/povray.h"

#include "vfe.h"

#ifndef _CONSOLE
#error "You must define _CONSOLE in windows/povconfig/syspovconfig.h prior to building the console version, otherwise you will get link errors."
#endif

using namespace vfe;
using namespace vfePlatform;

namespace pov_frontend
{
    ////////////////////////////////
    // Called from the shellout code
    ////////////////////////////////
    bool MinimizeShellouts(void) { return false; } // TODO
    bool ShelloutsPermitted(void) { return false; } // TODO
}
// end of namespace pov_frontend


void PrintStatus (vfeSession *session)
{
  std::string str;
  vfeSession::MessageType type;
  static vfeSession::MessageType lastType = vfeSession::mUnclassified;

  while (session->GetNextCombinedMessage (type, str))
  {
    if (type != vfeSession::mGenericStatus)
    {
      if (lastType == vfeSession::mGenericStatus)
        fprintf (stderr, "\n") ;
      fprintf (stderr, "%s\n", str.c_str());
    }
    else
      fprintf (stderr, "%s\r", str.c_str());
    lastType = type;
  }
}

void ErrorExit(vfeSession *session)
{
  fprintf (stderr, "%s\n", session->GetErrorString());
  session->Shutdown();
  delete session;
  exit (1);
}

// --write-includes=DIR: write the built-in standard include files to DIR
static int WriteIncludes(const char *dir)
{
  if ((dir == nullptr) || (*dir == '\0'))
  {
    fprintf (stderr, "--write-includes needs a directory, as in --write-includes=DIR (. for the current directory)\n");
    return 1;
  }

  pov_base::WriteBuiltinFilesResult result = pov_base::WriteBuiltinFiles(SysToUCS2String(dir));

  for (std::vector<std::string>::const_iterator i = result.written.begin(); i != result.written.end(); i++)
    fprintf (stderr, "wrote %s\n", i->c_str());
  for (std::vector<std::string>::const_iterator i = result.existing.begin(); i != result.existing.end(); i++)
    fprintf (stderr, "%s already exists\n", i->c_str());

  if (result.ok)
  {
    fprintf (stderr, "wrote the %u built-in standard include files (%lu bytes) to %s\n",
             (unsigned int) result.written.size(), (unsigned long) result.bytes, dir);
    return 0;
  }

  fprintf (stderr, "%s\n", result.message.c_str());
  return 1;
}

// this is an example of a minimal console version of POV-Ray using the VFE
// (virtual front-end) library. it is NOT INTENDED TO BE A FULLY-FEATURED
// CONSOLE IMPLEMENTATION OF POV-RAY and is not officially supported. see
// the unix version for a example of a more comprehensive console build.
int main (int argc, char **argv)
{
  char              *s;
  vfeWinSession     *session = new vfeWinSession() ;
  vfeStatusFlags    flags;
  vfeRenderOptions  opts;

  fprintf(stderr,
          "This is an example of a minimal console build of POV-Ray under Windows.\n\n"
          "Persistence of Vision(tm) Ray Tracer Version " POV_RAY_VERSION_INFO ".\n"
          DISTRIBUTION_MESSAGE_1 "\n"
          DISTRIBUTION_MESSAGE_2 "\n"
          DISTRIBUTION_MESSAGE_3 "\n"
          POV_RAY_COPYRIGHT "\n"
          DISCLAIMER_MESSAGE_1 "\n"
          DISCLAIMER_MESSAGE_2 "\n\n");

  if (session->Initialize(nullptr, nullptr) != vfeNoError)
    ErrorExit(session);

  // the console's only long option, `--write-includes=DIR` or `--write-includes DIR`;
  // it does nothing else
  static const char writeIncludes[] = "--write-includes";
  for (char **arg = argv + 1; *arg != nullptr; arg++)
  {
    const size_t length = sizeof (writeIncludes) - 1;
    if ((strncmp (*arg, writeIncludes, length) == 0) && (((*arg)[length] == '=') || ((*arg)[length] == '\0')))
    {
      int result = WriteIncludes ((*arg)[length] == '=' ? *arg + length + 1 : arg[1]);
      session->Shutdown() ;
      delete session;
      return result;
    }
  }

  if ((s = std::getenv ("POVINC")) != nullptr)
    opts.AddLibraryPath (s);
  while (*++argv)
    opts.AddCommand (*argv);

  if (session->SetOptions(opts) != vfeNoError)
    ErrorExit(session);
  if (session->StartRender() != vfeNoError)
    ErrorExit(session);

  bool pauseWhenDone = session->GetBoolOption("Pause_When_Done", false);
  while (((flags = session->GetStatus(true, 1000)) & stRenderShutdown) == 0)
    PrintStatus (session) ;
  session->Shutdown() ;
  PrintStatus (session) ;
  delete session;

  if (pauseWhenDone)
  {
    fprintf (stderr, "Press enter to continue ... ");
    fflush(stderr);
    getchar();
  }

  return 0;
}

