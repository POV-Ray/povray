[POV-Ray](http://www.povray.org/) - The Persistence of Vision Raytracer
=======================================================================

[![Semaphore Build Status](https://semaphoreci.com/api/v1/pov-ray/povray/branches/master/shields_badge.svg?label=Semaphore)](https://semaphoreci.com/pov-ray/povray  "Semaphore: Ubuntu 14.04 LTE 64-bit with gcc 4.8")
[![AppVeyor Build status](https://img.shields.io/appveyor/ci/c-lipka/povray-exwy4.svg?label=appveyor)](https://ci.appveyor.com/project/c-lipka/povray-exwy4 "AppVeyor: Windows Server 2012 with Visual Studio 2015")
[![Travis CI Build Status](https://img.shields.io/travis/POV-Ray/povray.svg?label=travis%20ci)](https://travis-ci.org/POV-Ray/povray "Travis CI: Ubuntu 12.04 LTE 64-bit with gcc 4.6; OS X 10.11 with clang 4.2")
[![Coverity Code Analysis](https://scan.coverity.com/projects/269/badge.svg)](https://scan.coverity.com/projects/pov-ray "Coverity: Static Code Analysis")
[![Maintenance Status](https://img.shields.io/maintenance/yes/2017.svg)](README.md "Last edited 2017-08-18")

- [License](#license)
- [Forums](#forums)
- [Bug Reports](#bug-reports)
- [Offician Binaries](#official-binaries)
- [Building POV-Ray](#building-pov-ray)
- [IDE Versions](#ide-versions)
- [3D Modeller](#3d-modeller)
- [Documentation](#documentation)
- [Contacting Us](#contacting-us)

License
--------------------------------------

As of version v3.7, the source for POV-Ray is licensed under the AGPL3. The documentation is under the
Creative Commons Attribution-Noncommercial-ShareAlike 2.5 license, and support files such
as SDL includes, macros, sample scenes and so forth are under the Creative Commons Attribution-ShareAlike
3.0 Unported License (see each file header for the specific one).

Forums
--------------------------------------

Discussion regarding POV-Ray is traditionally done via our forums at http://news.povray.org/.
These are also available via NNTP at news://news.povray.org/ for those preferring that.

Please note that the POV-Ray developers do not monitor all forums regularly. The ones we
tend to check most frequently are povray.general, povray.windows and povray.unix.

Bug Reports
--------------------------------------

It's generally a good idea to mention a bug in the forums prior to lodging a formal
report; this can save you some time if it's a non-bug or a solution is known. You
should also first check the [known issues](https://github.com/POV-Ray/povray/issues)
to see if it has been reported already.

If you're sure something is a bug then please do lodge a bug report on the GitHub issues tracker.

Official Binaries
--------------------------------------

At this point in time, the only platform for which the project distributes pre-built
'official' (i.e. supported) binaries is Microsoft Windows. These may be
obtained via http://www.povray.org/download/. We do intend to provide Mac OS X
binaries shortly, but these will be console-mode only (based on the unix build).

Official Windows binaries of selected development versions are made availabe at
https://github.com/POV-Ray/povray/releases on a semi-irregular basis.

Building POV-Ray
--------------------------------------

At this point in time we generally recommend building from the latest version of the
[`latest-stable` branch](https://github.com/POV-Ray/povray/tree/latest-stable). Alternatively,
you may want to opt for a recent [tagged version](https://github.com/POV-Ray/povray/tags)
to test-drive features that have been added since the latest stable release.

_Please do not build directly from the master branch_ (or any other non-stable branch
for that matter), as versions from that branch may report ambiguous version numbers,
making it difficult to obtain version-specific support or report bugs in a useful manner.

POV-Ray should compile on any POSIX-compliant system with the required tools (please see
[unix/README.md](unix/README.md) for build instructions),
on Microsoft Windows systems that have Visual Studio 2015 Update 1 or later installed (targeting
XP or later, both 32 and 64-bit - be sure to see [windows/README.md](windows/README.md),
otherwise your build _will not work_), and also on Mac systems (console mode only, using
an appropriately-modified version of the unix build - not currently provided by us).

If you are using an operating system with a package or ports system such as
Ubuntu or FreeBSD, you may like to check whether or not POV-Ray is available
via that route.

IDE versions
--------------------------------------

Currently the only version of POV-Ray with an IDE as such is the Windows build.
We do want to change that, though. With the release of POV-Ray v3.7 we have added
a clear split between the backend (renderer) and frontend (UI or console), along
with a C++ layer which abstracts this into a fairly easily-understood set of 
classes (VFE, aka 'Virtual Front End').

We will offer support to those wishing to use this interface layer to integrate
POV-Ray into an open-source cross-platform IDE. We would also be interested in
hearing suggestions as to what we could base such an IDE on, should we go ahead
to integrate it ourselves.

Putting it another way: we consider getting a cross-platform IDE a high priority.

3D Modeller
-------------------------------------

POV-Ray does not currently have its own 3d modelling application (at least, not one
in a usable state). We do own the rights to the Moray modeller, which was formerly
commercial, but it needs a little work to get it working with v3.7 or later. It is also
Windows only (due to its use of MFC). Nevertheless we will be adding the source
to the repository at a future date.

Authors of open-source modellers with a compatible licence wishing to directly
integrate POV-Ray are welcome to contact us for support in doing so.

Documentation
--------------------------------------

When built and installed via the means provided in the source tree, all versions
of POV-Ray come with documentation. For the Unix build, this is in the form of a
manpage giving basic usage, and full HTML-based documentation. For the Windows
version, there is a HtmlHelp (.CHM) file provided.

The official location for the online documentation is http://www.povray.org/documentation/.
Further information, as well as online documentation for the current development
version, can be found at http://wiki.povray.org.

Contacting Us
--------------------------------------

We prefer that you contact us via the forums mentioned at the head of this document.
If the matter is one that requires direct email contact (and this generally will NOT
include tech support requests, though exceptions are made for package maintainers)
you may use the address listed at the bottom of http://www.povray.org/povlegal.html.

