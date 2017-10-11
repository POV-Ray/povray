@echo off
rem Copyright (C) 1992 the Florida State University
rem   Distributed by the Florida State University under the terms of the
rem   GNU Library General Public License.
rem 
rem This file is part of Pthreads.
rem 
rem Pthreads is free software; you can redistribute it and/or
rem modify it under the terms of the GNU Library General Public
rem License as published by the Free Software Foundation (version 2).
rem 
rem Pthreads is distributed "AS IS" in the hope that it will be
rem useful, but WITHOUT ANY WARRANTY; without even the implied
rem warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
rem See the GNU Library General Public License for more details.
rem 
rem You should have received a copy of the GNU Library General Public
rem License along with Pthreads; see the file COPYING.  If not, write
rem to the Free Software Foundation, 675 Mass Ave, Cambridge,
rem MA 02139, USA.
rem 
rem Report problems and direct all questions to:
rem 
rem   pthreads-bugs@ada.cs.fsu.edu
rem 
rem   @(#)configure.bat	3.14 11/8/00
rem

echo Configuring Pthreads for DOS

copy Makefile.DOS Makefile

echo This DOS port assumes filename of style 8.3 (8 chars base name, 3 ext)
echo It may only work correctly under Windows95 if the Makefile is edited.
echo ---
echo This DOS port requires DJGPP v2 (see http://www.delorie.com/) w/ a min. of
echo djdev200.zip, bnu252b.zip, gcc272b.zip, mak373b.zip, maybe csdpmi3b.zip.
echo ---
echo Please ignore warnings during compilation
echo To make Pthreads type "make"

