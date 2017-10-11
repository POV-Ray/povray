/****************************************************************************
*
*						   The Zen Timer Library
*
*							   From the book
*						 "Zen of Assembly Language"
*							Volume 1, Knowledge
*
*							 by Michael Abrash
*
*					  Modifications by Kendall Bennett
*                   Copyright (C) 1993-4 SciTech Software
*
* Filename:		$Workfile:   ztimer.cpp  $
* Version:		$Revision:   1.0  $
*
* Language:		C++ 2.1
* Environment:	IBM PC (MS DOS)
*
* Description:	Non-inline member functions for the C++ class interface
*				to the Zen Timer Library.
*
* $Date:   05 Feb 1996 14:50:22  $ $Author:   KendallB  $
*
****************************************************************************/

#include <stdio.h>
#include <iostream.h>
#include "pmode.h"
#include "ztimer.h"

/*----------------------------- Implementation ----------------------------*/

// Compute the time elapsed between calls to LZTimerOn() and LZTimerOff()
// and add it to the current count.

void LZTimer::computeTime()
{
	if (!overflow()) {
		ulong newcount = LZTimerCount();
		if (newcount == 0xFFFFFFFFL)
			_overflow = true;
		else
			_count += newcount;
		}
}

// Routine to stop the ultra long period timer. The timer resolution is low
// enough to make this routine non-inline.

void ULZTimer::stop()
{
	_finish = ULZReadTime();
	_count += ULZElapsedTime(_start,_finish);
}

ostream& operator << (ostream& o,LZTimer& timer)
{
	char	buf[40];

	if (!timer.overflow()) {
		sprintf(buf, "%.6f", timer.count() * timer.resolution());
		o << buf;
		}
	else
		o << "overflow";
	return o;
}

ostream& operator << (ostream& o,ULZTimer& timer)
{
	char	buf[40];

	sprintf(buf, "%.1f", timer.count() * timer.resolution());
	return o << buf;
}
