/****************************************************************************
*
*						  Ultra Long Period Timer
*
*                   Copyright (C) 1993-4 SciTech Software
*							All rights reserved.
*
* Filename:		$Workfile:   ulztimer.c  $
* Version:		$Revision:   1.0  $
*
* Language:		ANSI C
* Environment:	IBM PC (MSDOS and Windows)
*
* Description:	Module to interface to the BIOS Timer Tick for timing
*				code that takes up to 24 hours (ray tracing etc). There
*				is a small overhead in calculating the time, this
*				will be negligible for such long periods of time.
*
* $Date:   05 Feb 1996 14:50:22  $ $Author:   KendallB  $
*
****************************************************************************/

#include "ztimer.h"
#include "pmode.h"

#ifdef  __WINDOWS32__
#undef	TRUE
#undef	FALSE
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

/* Win32 compatability routines. Win32 apps cannot program the timer and
 * ToolHelp is not available. However we can use the timeGetTime() function
 * to get the current time in ms since Windows started
 */

PRIVATE ulong   tmStart,tmEnd;
PRIVATE	ulong	start,finish;

void ZTimerInit(void) {}

void _ASMAPI LZTimerOn(void)
{ tmStart = timeGetTime(); }

ulong _ASMAPI LZTimerLap(void)
{
	ulong tmLap = timeGetTime();
	return (tmLap - tmStart) * 1000L;
}

void _ASMAPI LZTimerOff(void)
{ tmEnd = timeGetTime(); }

ulong _ASMAPI LZTimerCount(void)
{ return (tmEnd - tmStart) * 1000L; }

void ULZTimerOn(void)
{ start = timeGetTime(); }

void ULZTimerOff(void)
{ finish = timeGetTime(); }

ulong ULZTimerLap(void)
{ return (timeGetTime() - start); }

ulong ULZTimerCount(void)
{ return (start - finish); }

float ULZTimerResolution(void)
{ return 0.001; }

#elif	defined(__WINDOWS16__)
#undef	TRUE
#undef	FALSE
#include <windows.h>
#include <toolhelp.h>

/* 16 bit Windows compatability routines. Rather than reading the timer
 * directly under Windows, we use the Windows API routines (which only
 * provide an accuracy of 1ms however). The timer interrupts are virtualised
 * under windows, which causes us to get incorrect results if we program
 * the timer directly.
 */

PRIVATE	TIMERINFO	tmStart,tmEnd,tmLap;
PRIVATE	TIMERINFO	start,finish;

void ZTimerInit(void)
{
	tmStart.dwSize = tmEnd.dwSize = tmLap.dwSize = sizeof(TIMERINFO);
	start.dwSize = finish.dwSize = sizeof(TIMERINFO);
}

void _ASMAPI LZTimerOn(void)
{ TimerCount(&tmStart); }

ulong _ASMAPI LZTimerLap(void)
{
	TimerCount(&tmLap);
	return (tmLap.dwmsSinceStart - tmStart.dwmsSinceStart) * 1000L;
}

void _ASMAPI LZTimerOff(void)
{ TimerCount(&tmEnd); }

ulong _ASMAPI LZTimerCount(void)
{ return (tmEnd.dwmsSinceStart - tmStart.dwmsSinceStart) * 1000L; }

void ULZTimerOn(void)
{ TimerCount(&start); }

void ULZTimerOff(void)
{ TimerCount(&finish); }

ulong ULZTimerLap(void)
{
	TimerCount(&tmLap);
	return (tmLap.dwmsSinceStart - start.dwmsSinceStart);
}

ulong ULZTimerCount(void)
{ return (finish.dwmsSinceStart - start.dwmsSinceStart); }

float ULZTimerResolution(void)
{ return 0.001; }

#else

PRIVATE	ulong	start,finish;
ushort 			_ASMAPI _ZTimerBIOS;

void _ASMAPI LZ_disable(void);
void _ASMAPI LZ_enable(void);

void ZTimerInit(void)
{ _ZTimerBIOS = PM_getBIOSSelector(); }

void ULZTimerOn(void)
{ start = ULZReadTime(); }

void ULZTimerOff(void)
{ finish = ULZReadTime(); }

ulong ULZTimerLap(void)
{ return ULZElapsedTime(start,ULZReadTime()); }

ulong ULZTimerCount(void)
{ return ULZElapsedTime(start,finish); }

ulong ULZReadTime(void)
/****************************************************************************
*
* Function:		ULZReadTime
* Returns:		Current timer tick count.
*
* Description:	We turn of interrupts while we
*				get the value from the BIOS data area since it is stored
*				as two bytes and an interrupt COULD stuff up our reading.
*
****************************************************************************/
{
	ulong	ticks;

    LZ_disable();            /* Turn of interrupts               */
	ticks = PM_getLong(_ZTimerBIOS,0x6C);
    LZ_enable();             /* Turn on interrupts again         */

	return ticks;
}

ulong ULZElapsedTime(ulong start,ulong finish)
/****************************************************************************
*
* Function:		ULZElapsedTime
* Parameters:	start	- Starting timer tick value
*				finish	- Ending timer tick value
*
* Returns:		Elapsed timer between starting time and ending time in
*				1/18 ths of a second.
*
****************************************************************************/
{
	/* Check to see whether a midnight boundary has passed, and if so
	 * adjust the finish time to account for this. We cannot detect if
	 * more that one midnight boundary has passed, so if this happens
	 * we will be generating erronous results.
	 */

	if (finish < start)
		finish += 1573040L;			/* Number of ticks in 24 hours		*/

	return finish - start;
}

float ULZTimerResolution(void)
/****************************************************************************
*
* Function:		ULZTimerResolution
* Returns:		Resolution of the ULZTimer routines (seconds in a tick)
*
****************************************************************************/
{
	return 0.054925;
}

#endif	/* DOS */
