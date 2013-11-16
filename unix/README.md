Building POV-Ray 3.7 for GNU/Linux and similar systems
======================================================

This distribution contains the complete source code of POV-Ray for UNIX
as well as its accompanying HTML documentation and supporting files. In
order to build the distribution supplied in the POV-Ray github repo, you
will need to run a pre-build step. This generates the configure script
and does a few other things usually performed by our developers prior to
releasing a source packag.

Generating configure and building the code
==========================================

    % cd unix/
    % ./prebuild.sh
    % cd ../
    % ./configure COMPILED_BY="your name <email@address>" 
    % make

Installing for the current user
===============================

    % make install
    
Installing for all users
========================

    % su make install

Dependencies
============

    libboost-dev
    zlib1g-dev
    libpng12-dev
    libjpeg8-dev
    libtiff5-dev
    
