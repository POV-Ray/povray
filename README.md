[POV-Ray](http://www.povray.org/) - The Persistence of Vision Raytracer
=======================================================================

This document is a WIP
--------------------------------------

This document is still a work in progress. While the POV-Ray project itself
has existed for more than 20 years, this is the first time we have done a
release on github, so please bear with us - it's a little bare at the moment.

Please also be aware that we are in the process of actually doing the 3.7
release right now, and this document may refer to things that have not happened
yet on the main website (e.g. the release of the binaries, updating of pages,
and so forth).

Last edit: 2013-11-06

License
--------------------------------------

The source for POV-Ray v3.7 is licensed under the AGPL3. The documentation is under the
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
should also check our bug tracker at http://bugs.povray.org/ first to see if it's
been reported.

If you're sure something is a bug then please do lodge a bugreport on the tracker.
While we are aware that github has its own tracker, please keep in mind that POV-Ray
is a long-established project (more than 20 years) and we prefer to use our own tracker.
We do not do our primary develoment on github (at least, not at this point). Our RCS
for the past 15 or so years has been Perforce. We will be pulling patches from here
into perforce, and pushing changes out to github from time to time. At some point we
will switch to using Perforce's Git Fusion product and this process will become
automatic.

Official Binaries
--------------------------------------

At this point in time, the only platform for which the project distributes pre-
built 'official' (i.e. supported) binaries is Microsoft Windows. These may be
obtained via http://www.povray.org/download/. We do intend to provide Mac OS X
binaries shortly, but these will be console-mode only (based on the unix build).

Building POV-Ray
--------------------------------------

At this point in time we recommend building from the 3.7-stable branch. POV-Ray
should compile on any POSIX-compliant system with the required tools (please see
[unix/README.md](unix/README.md) for build instructions),
on Microsoft Windows systems that have Visual Studio 2010 or later installed (targeting
XP or later, both 32 and 64-bit - be sure to see README.HTML in the windows source dir,
otherwise your build *will not work*), and also on Mac systems (console mode only, using
an appropriately-modified version of the unix build - not currently provided by us).

If you are using an operating system with a package or ports system such as
Ubuntu or FreeBSD, you may like to check whether or not POV-Ray 3.7 is available
via that route.

IDE versions
--------------------------------------

Currently the only version of POV-Ray with an IDE as such is the Windows build.
We do want to change that, though. With the release of POV-Ray 3.7 we have added
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
commercial, but it needs a little work to get it working with v3.7. It is also
windows only (due to its use of MFC). Nevertheless we will be adding the source
to the repository at a future date.

Authors of open-source modellers with a compatible licence wishing to directly
integrate POV-Ray are welcome to contact us for support in doing so.

Documentation
--------------------------------------

When built and installed via the means provided in the source tree, all versions
of POV-Ray come with documentation. For the Unix build, this is in the form of a
manpage giving basic usage, and full HTML-based documentation. For the Windows
version, there is a HtmlHelp (.CHM) file provided.

The official location for the online documentation is http://www.povray.org/documentation/,
however please be aware that at the time of writing this still has the version 3.6 docs.
This will be updated shortly.

Contacting Us
--------------------------------------

We prefer that you contact us via the forums mentioned at the head of this document.
If the matter is one that requires direct email contact (and this generally will NOT
include tech support requests, though exceptions are made for package maintainers)
you may use the address listed at the bottom of http://www.povray.org/povlegal.html.

