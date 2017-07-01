Building POV-Ray 3.7 for GNU/Linux and similar systems
======================================================

This distribution contains the complete source code of POV-Ray for UNIX
as well as its accompanying HTML documentation and supporting files. In
order to build the distribution supplied in the POV-Ray github repo, you
will need to run a pre-build step. This generates the configure script
and does a few other things usually performed by our developers prior to
releasing a source package.

Dependencies
============

By default POV-Ray will attempt to build using system-supplied libraries for
boost, zlib, libpng, libjpeg, libtiff, and openexr. We recommend you have the
following packages pre-installed:

    libboost-dev
    libboost-date-time-dev
    libboost-thread-dev
    libz-dev
    libpng-dev
    libjpeg-dev
    libtiff-dev
    libopenexr-dev

To enable the render preview display also pre-install:

    libsdl-dev

The --with-x ./configure option is currently meaningless as no X11
display option is presently implemented.

Lastly, certain OSX environments do not install the program pkg-config
with autoconf. The program is currently necessary when determining the
link library flags for openexr. In these situations also install:

    pkg-config


Generating configure and building the code
==========================================

    % cd unix/
    % ./prebuild.sh
    % cd ../
    % ./configure COMPILED_BY="your name <email@address>"
    % make

Note that the prebuild step will generate a few warnings as it still looks
for some files no longer included in current versions of POV-Ray. This is harmless.

Installing
==========

    % make install

This above installs POV-Ray under system-wide directories and needs root privileges.
Installing as non-privileged user is also possible. By default (i.e. as root), 'make
install' installs the following components:

    povray binary (executable)              in /usr/local/bin
    data files (e.g. includes, scene files) in /usr/local/share/povray-3.7
    documentation (text and html)           in /usr/local/share/doc/povray-3.7
    configuration files (e.g. povray.conf)  in /usr/local/etc/povray/3.7

The configuration files are also copied under the $HOME/.povray/3.7 directory
of the user (or root) who is doing the installation.

Please see [unix/install.txt](install.txt) for more details about available
configuration options etc.
