#!/bin/csh
# Copyright (C) 1992-2000 the Florida State University
#  Distributed by the Florida State University under the terms of the
#  GNU Library General Public License.
#
#This file is part of Pthreads.
#
#Pthreads is free software; you can redistribute it and/or
#modify it under the terms of the GNU Library General Public
#License as published by the Free Software Foundation (version 2).
#
#Pthreads is distributed "AS IS" in the hope that it will be
#useful, but WITHOUT ANY WARRANTY; without even the implied
#warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#See the GNU Library General Public License for more details.
#
#You should have received a copy of the GNU Library General Public
#License along with Pthreads; see the file COPYING.  If not, write
#to the Free Software Foundation, 675 Mass Ave, Cambridge,
#MA 02139, USA.
#
#Report problems and direct all questions to:
#
#  pthreads-bugs@ada.cs.fsu.edu
#
#   @(#)gmal_patch.csh	3.14 11/8/00
#

#Modify the Gnu malloc routines to cooperate with Pthreads.

echo "Sit back, this will take a moment..."
foreach pattern (malloc free realloc calloc cfree memalign valloc)
  foreach file (*.c *.h)
    awk -f gmalloc_patch.awk PATTERN=$pattern $file >tmp
    mv -f tmp $file
  end
end
