#  Copyright (C) 1992, the Florida State University
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
#   @(#)gmalloc_patch.awk	1.2 10/1/93
#

#Replace string PATTERN with pthread_PATTERN if
#it is followed by an open parathesis (for C file)
#or by __P (for header files).

{
  FOUND = 0
  for (i = 1; i <= NF; i++)
    if ($i == PATTERN && \
        (substr($(i+1), 1, 1) == "(" || substr($(i+1), 1, 3) == "__P")) {
      for (k = 1; k <= length($0) - length(PATTERN) + 1; k++)
        if (substr($0, k, length(PATTERN)) == PATTERN)
          break;
      print substr($0, 1, k - 1) "pthread_" substr($0, k, length($0) - k + 1)
      FOUND = 1
    }

  if (FOUND == 0)
    print $0
}
