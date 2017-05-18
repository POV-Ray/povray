------------------------------------------------------------------------

Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.

POV-Ray is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

POV-Ray is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public
License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>

------------------------------------------------------------------------


Building the POV-Ray for Windows source
=======================================


About this document
-------------------

This document does not attempt to explain how POV-Ray works or how to
make modifications to it. Some of the internals of POV-Ray are highly
complex; if you wish to gain a better understanding of that topic,
please follow the link provided on the source code page mentioned below.
This document is intended only to assist you in getting the POV-Ray
source code built using an officially supported compiler on Win32 or
Win64. Furthermore, no attempt is made to cover advanced subjects such
as profile-guided optimization (though we will point out that the latter
is definitely worthwhile, if you have the time and patience).

Assistance
----------

Please use our support groups at <http://news.povray.org/> or <news://news.povray.org/>.
These groups provide the best means for support with relation to this subject.


Compilers
=========

Visual Studio 2010 and later
----------------------------

Currently the only officially supported means of building the Windows
version of POV-Ray 3.7.0 is via the provided Visual Studio 2010 project (you
can also use it with later versions of Visual Studio) with a full
(i.e. non-Express) version of Visual Studio and the compilers provided
with it or with the platform SDK.

The Visual Studio 2010 project is located in the "windows/vs10"
subdirectory.

Note that binaries generated with Visual Studio 2012 or later will
typically be incompatible with Windows XP unless you take extra steps,
the details (and pitfalls) of which are beyond the scope of this document.
If XP compatibility is an issue for you, we strongly recommend sticking to
Visual Studio 2010.

Earlier Versions of Visual Studio
---------------------------------

Note that versions of Visual Studio prior to 8.0 (VS 2005) are not
supported at all and furthermore **will not work**. The code in POV-Ray
3.7 requires a reasonably up-to-date C++ compiler and STL, and earlier
versions of VC++ are not compatible. VS 2005 _may_ work but as it is not
used or tested against we do not recommend it. VS2008 _should_ work as
it was used for a portion of the development process of POV 3.7. In
either case you will need to generate suitable project files, which is
not as simple as just dropping all the source files into a new project
and hitting F7. Please closely study the options applied to each project
in the VS2010 solution if you wish to try to backport the projects.

Visual Studio Express
---------------------

The express editions have been reported to work in building 32-bit
executables but this is not officially supported by us. The following
information relates to building with an express edition as of the last
time we looked into it; things may have changed with later releases
(e.g. VS2012) so please take this into account when reading the below.

At the minimum, you will need to download and install the Platform SDK
as this is not supplied with VS Express. It has also been reported that
to obtain the shlwapi.h header file (which provides functions such as
`GetDLLVersion` etc), you will need to allow the SDK setup to install
the "Microsoft Web Workshop Build Environment".

Since better free alternatives have become available in the form of the
Community Editions of later Visual Studio versions, we strongly discourage
attempting to build POV-Ray for Windows using VS Express.


Including the Editor Support DLL (cmedit) in the Build
======================================================

It is not necessary to build the editor support DLL (cmedit), as it is
loaded via `LoadLibrary()` and thus no import library is required. You
can just use the DLL's included with the official version 3.7 binary
distribution. If however you do wish to build it, either enable it in
the configuration manager or initiate a build of that specific project
manually.

cmedit acts as an interface between POVWIN and the full editor DLL
(povcmax), which is supplied as a binary with the official POV-Ray for
Windows distribution. The source code for povcmax is based on the
codemax code editor and is not part of the AGPL3 POV-Ray source
distribution. If you wish to know more about the editors, please refer
to the "about the editor DLL's" section below.

**Note:** The editor DLL's are _not_ required for POVWIN to work. If they
are not present or cannot be loaded, POVWIN is designed to be able to
function quite happily without them. The editor and its related
functions will simply not be present.



Build Steps
===========

1.  Make sure you have a working copy of the appropriate release binary
    installed. This ensures that the appropriate registry settings and
    support files are present. If you don't do this, your compiled code
    will probably not work on your machine.

2.  Open `windows\vs10\povray.sln` in Visual Studio. Set
    'Windows Targets > GUI' as the start-up project if it is not already
    selected. Then, select the 'Generic POV-Ray > povbackend' project
    and expand 'Backend Headers', then open the file `povray.h` listed
    within it. Please set `DISTRIBUTION_MESSAGE_2` to your real name to
    make unofficial versions distinguishable from each other. Remove the
    `#error` directive afterwards to proceed with the build. Note that
    if you do not do this you will get compile errors.

    By default, the GUI project is built, and the Console project is
    excluded. If you wish to generate a simple console version of
    POV-Ray, please modify the configuration as needed, edit
    `vfe\win\syspovconfig.h` to enable the definition of `_CONSOLE`
    as noted therein, and build.

    **Note:** The windows console project is intended to be a simple
    example of how to use the VFE library to make a console version of
    POV-Ray with _minimal code_. It is **not** intended to be a
    fully-featured console build of POV. If you wish to make a more
    comprehensive console version that runs on windows, please use the
    unix version as a guide - it uses the same principles (as it is also
    linked with VFE) but has more features.

3.  Once you have taken whatever steps are needed as set out above,
    select your desired build (e.g. `Release|Win32`), hit F7, and wait.
    We recommend that you read the rest of this document while you are
    waiting.

    If all goes well, you should end up with the POV-Ray for Windows
    executable. All 32-bit binaries should end up in
    `windows\vs10\bin32`, and the 64-bit ones should be in
    `windows\vs10\bin64`.

    Note: if you are building a DEBUG version of POV-Ray, be aware that
    the executable will attempt to load the debug version of POV-Ray's
    editor DLLs. In this case, if you want the editor to be available in
    the GUI, copy the standard editor DLLs that come with the official
    POV-Ray 3.7 distribution to their debug equivalent names (e.g. copy
    `cmedit32.dll` to `cmedit32d.dll`, etc).

    Note that the copied `cmedit32.dll` will still attempt to load
    `povcmax32.dll` (i.e. the non-debug version), so it's not necessary
    to copy `povcmax32.dll` to its debug equivalent. The same goes for
    `povcmax64.dll`.

**Note:** You should not attempt to build any of the following targets
(if they are present), as they are not actively maintained, and may
generate flawed binaries or fail to compile at all:

-   `Release-SSE2|x64` (This target would be redundant, as SSE2 is a
    standard feature of the x64 architecture and is therefore implicitly
    enabled in all `x64` builds, including vanilla `Release|x64`.)

The only reason for their presence is that Visual Studio keeps insisting on
having all possible combinations of targets and configurations, no matter
how hard we try to get rid of them.

Please read the rest of this document while you're waiting for your
compile to finish, particularly the 'other things you may want to keep
in mind' at the end.


About the Editor DLL's
======================

You really don't need to worry about any of the following section unless
you want to do editor hacking ; the DLL's provided with the POV-Ray for
Windows binaries are sufficient, and therefore you don't need to compile
them at all if you don't want to.

The editor used in POV-Ray for Windows is based around a custom control
called CodeMax, supplied courtesy of Barry Allyn. Our implementation
consists of two DLL's: `cmedit32.dll` and `povcmax32.dll` (in this case,
as for the rest of the instructions, we will refer to the 32-bit
versions; if building for Win64 then just replace the '32' with '64').
If you are building debug versions, the DLL's will be suffixed with a
'd' (for example, `cmedit32d.dll`).

The editor control itself is `povcmax32.dll`. The source for povcmax is
not distributed with the AGPL3 version of POV-Ray. If you do wish to
obtain this source it may be found in prior releases (such as the
version 3.7 release candidates, which were not AGPL licensed). Please
note however that codemax is NOT AGPL and was only included with POV-Ray
under the terms of the former POV-Ray license.

The other DLL, `cmedit32`, consists of support code that wraps and
enhances povcmax, and provides services such as file I/O, menus, state
storage/restore, and many other things. This DLL is built by the
'cmedit' project (which is by default disabled via configuration manager).

For reference, while most of the POV-Ray-specific customization is done
within cmedit, there are some POV-Ray specific modifications made to
codemax itself, particularly with respect to the code completion
support. However, for the most part, if you are intending to modify some
aspect of the POV-Ray interface to the editor, you will find you will
want to modify the contents of the cmedit project (or, possibly,
pvedit.cpp in the windows directory - this implements the API between
the POV-Ray executable and `cmedit32.dll`). Note that if you alter this
API at all, you must change the editor version (`EDITDLLVERSION` in
`windows\pvedit.h`).

By default, POV-Ray 3.7 will attempt to load the editor DLL's from the
directory in which the EXE resides first. (The previous strategy was to
look in the current working directory first; POV-Ray no longer checks
the CWD, though you can force that behaviour if you wish by using the
switch mentioned below).

If a DLL with the right name is found with the EXE it will be used;
otherwise, the default binary directory is used. If you are not sure
where it's looking, create the directory `c:\temp` if it does not
already exist and then run `pvengine` with the `/debug` switch. Once it
has loaded have a look at `c:\temp\povdebug.txt`, which will list the
search paths attempted.

There is a switch available on the command-line which can alter the
search path. If you specify `/EDITDLLPATH` on the command-line and
follow it with a path, that path will be the only one checked. To force
POV-Ray to look in the current directory (i.e. the one the EXE is
launched from, which may not be the one the EXE is stored in), you can
pass `.` as the edit DLL path.

**Note:** currently POV-Ray for Windows has no way of differentiating the
official editor DLL's from custom ones. We may at a future point provide
a means for unofficial DLL's to be loaded by the official PVENGINE
binary. In the meantime, if you want to make absolutely sure that your
unofficial binary doesn't load the official DLL's, and vice-versa, you
can rename the DLL's. Doing this requires you to tell the EXE of course -
to do this, open `windows\pvengine.h` and look for where `EDITDLLNAME`
is defined. Just change that to suit, re-compile, and rename the DLL's
manually (or change the project to output different names).


Other Things you May Want to Keep in Mind
=========================================

Other Compilers
---------------

If porting POV-Ray for Windows to another compiler, or even if you're
just writing a makefile for the Visual Studio version, be aware that
there is some special configuration in the project settings. The most
critical one to replicate is the entry point - POVWIN uses a custom
entry point, and unless you adjust the code, it is essential this is
called prior to the C run-time library startup code. It's also highly
likely that you will need to change the code that calls the RTL from
within POV-Ray as well, unless you are using the Microsoft RTL (or a
compatible one).

Custom Entry Point
------------------

Currently, the entry point is `POVWINStartup`, which is declared in
`pvmem.cpp`. We use this to set up a private heap prior to any memory
allocation occurring (including even allocations done to construct any
global or static classes). If `WIN_MEM_TRACKING` is defined (which it is
by default), `pvmem.cpp` replaces the standard `new`, `new[]`, `delete`,
and `delete[]` operators with its own ones.


Memory Tracking and Debugging
=============================

Apart from tracking the amount of memory used and freed, the above
tracking code, when `_DEBUG` is defined, will also provide a custom
wrapper around each memory block which records where the new or malloc
occurred. By default, `_DEBUG` also causes `EXTRA_VALIDATION` to be
defined. When this is present, each block allocated has, in addition to
the standard extra fields added to the start, a single field of length
`sizeof(ptrdiff_t)` added to the _end_ of the block. This is then
initialized with a simple hash made up from various aspects of the
block, including its address, length, and the source line it was
allocated from. This is able to be checked at any time using the
`Validate()` method to see if it has been altered since the block was
allocated (e.g. by an overrun).

Additionally, while it has not yet been implemented, the memory hooks
give us the opportunity to provide access to a low-fragmentation heap or
other useful memory allocation techniques (e.g. handling allocations
differently during renders, if we want) for the purpose of improving
performance.


The VFE Library
===============

The VFE library included with the source acts as a wrapper for the
communications system ('POVMS') that connects the POV-Ray front and
backend code (which are logically separate, and may even be on separate
computers). POVMS is a message-passing system that is platform-neutral,
thus allowing the backend to run on a different architecture than the
frontend.

VFE provides a worker thread and various abstractions that handle most
of the work of hooking a frontend up to the backend, even to the point
where a frontend may be transient or stateless (i.e. HTTP, hence the
term 'virtual'). The Windows and Unix versions of POV-Ray use VFE to
connect the frontend to the backend.

If you wish to integrate POV-Ray into your own software (subject of
course to the terms of the AGPL3), it is strongly recommended that you
first study the source of povconsole.cpp, which implements a minimal
example of how to setup and run a render. A more comprehensive example
of console integration is in the unix support code, and for a GUI
version, the windows support code should be read.

In either case, be sure to read through `vfe/vfesession.h`;
it is heavily commented and is intended to be the
primary reference to the API.

If you have issues with getting VFE to work with your own custom
frontend, please visit our support forums and ask about it: if there are
any bugs or shortcomings we would like to fix them. We will also try to
offer general assistance where possible, subject to time limitations.

If your project is commercial and you wish to have dedicated support,
some of the POV-Ray developers may be available for contract work,
depending on the circumstance. Please contact us for more information.


Thanks
======

The POV-Team would like to thank all those who have assisted in bringing
POV-Ray 3.7 and its predecessors to the public - you know who you are,
and we salute you.

