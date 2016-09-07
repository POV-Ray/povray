Building POV-Ray 3.7 for GNU/Linux and similar systems
======================================================

This distribution contains the complete source code of POV-Ray for UNIX
as well as its accompanying HTML documentation and supporting files. In
order to build the distribution supplied in the POV-Ray github repo, you
will need to run a pre-build step. This generates the configure script
and does a few other things usually performed by our developers prior to
releasing a source package.

NB if you have improvements to the below instructions, please add a comment
to issue #1, or (if you are working in a branch) send us a pull request.

Dependencies
============

By default POV-Ray will attempt to build using system-supplied libraries for
boost, zlib, libpng, libjpeg, libtiff, and openexr. We recommend you have the
following packages pre-installed: 

    libboost-dev
    libboost-date-time-dev
    libboost-thread-dev
    zlib1g-dev
    libpng12-dev
    libjpeg8-dev
    libtiff5-dev
    libopenexr-dev
    
Generating configure and building the code
==========================================

    % cd unix/
    % ./prebuild.sh
    % cd ../
    % ./configure COMPILED_BY="your name <email@address>"
    % make

Note that the prebuild step will generate a few warnings as it still looks
for some files no longer included in v3.7. This is harmless.

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
