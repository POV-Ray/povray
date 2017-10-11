;****************************************************************************
;*
;*						   Long Period Zen Timer
;*
;*							   From the book
;*						 "Zen of Assembly Language"
;*							Volume 1, Knowledge
;*
;*							 by Michael Abrash
;*
;*					  Modifications by Kendall Bennett
;*                  Copyright (C) 1996 SciTech Software
;*
;* Filename:	$Workfile:   lztimer.asm  $
;* Version:		$Revision:   1.0  $
;*
;* Language:	80386 Assembler
;* Environment:	IBM PC (MS DOS)
;*
;* Description:	Uses the 8253 timer and the BIOS time-of-day count to time
;*				the performance of code that takes less than an hour to
;*				execute.
;*
;*				The routines in this package only works with interrupts
;*				enabled, and in fact will explicitly turn interrupts on
;*				in order to ensure we get accurate results from the timer.
;*
;*	Externally 'C' callable routines:
;*
;*	LZTimerOn:		Saves the BIOS time of day count and starts the
;*					long period Zen Timer.
;*
;*	LZTimerLap:		Latches the current count, and keeps the timer running
;*
;*	LZTimerOff:		Stops the long-period Zen Timer and saves the timer
;*					count and the BIOS time of day count.
;*
;*	LZTimerCount:	Returns an unsigned long representing the timed count
;*					in microseconds. If more than an hour passed during
;*					the timing interval, LZTimerCount will return the
;*					value 0xFFFFFFFF (an invalid count).
;*
;*	Note:	If either more than an hour passes between calls to LZTimerOn
;*			and LZTimerOff, an error is reported. For timing code that takes
;*			more than a few minutes to execute, use the low resolution
;*			Ultra Long Period Zen Timer code, which should be accurate
;*			enough for most purposes.
;*
;*	Note:	Each block of code being timed should ideally be run several
;*			times, with at least two similar readings required to
;*			establish a true measurement, in order to eliminate any
;*			variability caused by interrupts.
;*
;*	Note:	Interrupts must not be disabled for more than 54 ms at a
;*			stretch during the timing interval. Because interrupts are
;*			enabled, key, mice, and other devices that generate interrupts
;*			should not be used during the timing interval.
;*
;*	Note:	Any extra code running off the timer interrupt (such as
;*			some memory resident utilities) will increase the time
;*			measured by the Zen Timer.
;*
;*	Note:	These routines can introduce inaccuracies of up to a few
;*			tenths of a second into the system clock count for each
;*			code section being timed. Consequently, it's a good idea to
;*			reboot at the conclusion of timing sessions. (The
;*			battery-backed clock, if any, is not affected by the Zen
;*			timer.)
;*
;*  All registers and all flags are preserved by all routines, except
;*	interrupts which are always turned on
;*
;* $Date:   05 Feb 1996 14:35:54  $ $Author:   KendallB  $
;*
;****************************************************************************

		IDEAL

INCLUDE "model.mac"				; Memory model macros

header	lztimer					; Set up memory model

ifndef  __WINDOWS16__
ifndef  __WINDOWS32__

;****************************************************************************
;
; Equates used by long period Zen Timer
;
;****************************************************************************

; Base address of 8253 timer chip

BASE_8253		=		40h

; The address of the timer 0 count registers in the 8253

TIMER_0_8253	=		BASE_8253 + 0

; The address of the mode register in the 8253

MODE_8253		=		BASE_8253 + 3

; The address of the BIOS timer count variable in the BIOS data area.

TIMER_COUNT		=		6Ch

; Macro to delay briefly to ensure that enough time has elapsed between
; successive I/O accesses so that the device being accessed can respond
; to both accesses even on a very fast PC.

macro	DELAY
		jmp		$+2
		jmp		$+2
		jmp		$+2
endm

begdataseg	lztimer

		$EXTRN  __ZTimerBIOS,USHORT

StartBIOSCount      dd  ?       ; Starting BIOS count dword
EndBIOSCount		dd	?		; Ending BIOS count dword
EndTimedCount		dw	?		; Timer 0 count at the end of timing period

enddataseg	lztimer

begcodeseg	lztimer				; Start of code segment

;----------------------------------------------------------------------------
; void LZTimerOn(void);
;----------------------------------------------------------------------------
; Starts the Long period Zen timer counting.
;----------------------------------------------------------------------------
procstart		_LZTimerOn

; Set the timer 0 of the 8253 to mode 2 (divide-by-N), to cause
; linear counting rather than count-by-two counting. Also stops
; timer 0 until the timer count is loaded, except on PS/2 computers.

		mov		al,00110100b		; mode 2
		out		MODE_8253,al

; Set the timer count to 0, so we know we won't get another timer
; interrupt right away. Note: this introduces an inaccuracy of up to 54 ms
; in the system clock count each time it is executed.

		DELAY
		sub		al,al
		out		TIMER_0_8253,al		; lsb
		DELAY
		out		TIMER_0_8253,al		; msb

; Store the timing start BIOS count

		push	es
		mov		ax,[__ZTimerBIOS]
		mov		es,ax
		cli							; No interrupts while we grab the count
		mov		eax,[es:TIMER_COUNT]
		sti
		mov		[StartBIOSCount],eax
		pop		es

; Set the timer count to 0 again to start the timing interval.

		mov		al,00110100b		; set up to load initial
		out		MODE_8253,al		; timer count
		DELAY
		sub		al,al
		out		TIMER_0_8253,al		; load count lsb
		DELAY
		out		TIMER_0_8253,al		; load count msb

		ret

procend		_LZTimerOn

;----------------------------------------------------------------------------
; void LZTimerOff(void);
;----------------------------------------------------------------------------
; Stops the long period Zen timer and saves count.
;----------------------------------------------------------------------------
procstart		_LZTimerOff

; Latch the timer count.

		mov		al,00000000b		; latch timer 0
		out		MODE_8253,al
		cli							; Stop the BIOS count

; Read the BIOS count. (Since interrupts are disabled, the BIOS
; count won't change).

		push	es
		mov     ax,[__ZTimerBIOS]
		mov		es,ax
		mov		eax,[es:TIMER_COUNT]
		mov		[EndBIOSCount],eax
		pop		es

; Read out the count we latched earlier.

		in		al,TIMER_0_8253		; least significant byte
		DELAY
		mov		ah,al
		in		al,TIMER_0_8253		; most significant byte
		xchg	ah,al
		neg		ax					; Convert from countdown remaining
									;  to elapsed count
		mov		[EndTimedCount],ax
		sti							; Let the BIOS count continue

		ret

procend		_LZTimerOff

;----------------------------------------------------------------------------
; unsigned long LZTimerLap(void)
;----------------------------------------------------------------------------
; Latches the current count and converts it to a microsecond timing value,
; but leaves the timer still running. We dont check for and overflow,
; where the time has gone over an hour in this routine, since we want it
; to execute as fast as possible.
;----------------------------------------------------------------------------
procstart		_LZTimerLap

		use_ebx						; Save EBX for 32 bit code

; Latch the timer count.

        mov     al,00000000b        ; latch timer 0
		out		MODE_8253,al
        cli                         ; Stop the BIOS count

; Read the BIOS count. (Since interrupts are disabled, the BIOS
; count won't change).

		push	es
		mov		ax,[__ZTimerBIOS]
		mov		es,ax
		mov		eax,[es:TIMER_COUNT]
		mov		[EndBIOSCount],eax
		pop		es

; Read out the count we latched earlier.

		in		al,TIMER_0_8253		; least significant byte
		DELAY
		mov		ah,al
		in		al,TIMER_0_8253		; most significant byte
		xchg	ah,al
		neg		ax					; Convert from countdown remaining
									;  to elapsed count
		mov		[EndTimedCount],ax
		sti							; Let the BIOS count continue

; See if a midnight boundary has passed and adjust the finishing BIOS
; count by the number of ticks in 24 hours. We wont be able to detect
; more than 24 hours, but at least we can time across a midnight
; boundary

		mov		eax,[EndBIOSCount]		; Is end < start?
		cmp		eax,[StartBIOSCount]
		jae		@@CalcBIOSTime			; No, calculate the time taken

; Adjust the finishing time by adding the number of ticks in 24 hours
; (1573040).

		add		[EndBIOSCount],1800B0h

; Convert the BIOS time to microseconds

@@CalcBIOSTime:
		mov		ax,[WORD EndBIOSCount]
		sub		ax,[WORD StartBIOSCount]
		mov		dx,54925			; Number of microseconds each
									;  BIOS count represents.
		mul		dx
		mov		bx,ax				; set aside BIOS count in
		mov		cx,dx				;  microseconds

; Convert timer count to microseconds

		push	_si
		mov		ax,[EndTimedCount]
		mov		si,8381
		mul		si
		mov		si,10000
		div		si					; * 0.8381 = * 8381 / 10000
		pop		_si

; Add the timer and BIOS counts together to get an overall time in
; microseconds.

		add		ax,bx
		adc		cx,0
if flatmodel
		shl		ecx,16
		mov		cx,ax
		mov		eax,ecx				; EAX := timer count
else
		mov		dx,cx
endif
		unuse_ebx					; Restore EBX for 32 bit code
		ret

procend		_LZTimerLap

;----------------------------------------------------------------------------
; unsigned long LZTimerCount(void);
;----------------------------------------------------------------------------
; Returns an unsigned long representing the net time in microseconds.
;
; If an hour has passed while timing, we return 0xFFFFFFFF as the count
; (which is not a possible count in itself).
;----------------------------------------------------------------------------
procstart		_LZTimerCount

		use_ebx						; Save EBX for 32 bit code

; See if a midnight boundary has passed and adjust the finishing BIOS
; count by the number of ticks in 24 hours. We wont be able to detect
; more than 24 hours, but at least we can time across a midnight
; boundary

		mov		eax,[EndBIOSCount]		; Is end < start?
		cmp		eax,[StartBIOSCount]
		jae		@@CheckForHour			; No, check for hour passing

; Adjust the finishing time by adding the number of ticks in 24 hours
; (1573040).

		add		[EndBIOSCount],1800B0h

; See if more than an hour passed during timing. If so, notify the user.

@@CheckForHour:
		mov		ax,[WORD StartBIOSCount+2]
		cmp		ax,[WORD EndBIOSCount+2]
		jz		@@CalcBIOSTime		; Hour count didn't change, so
									;  everything is fine

		inc		ax
		cmp		ax,[WORD EndBIOSCount+2]
		jnz		@@TestTooLong		; Two hour boundaries passed, so the
									;  results are no good
		mov 	ax,[WORD EndBIOSCount]
		cmp		ax,[WORD StartBIOSCount]
		jb		@@CalcBIOSTime		; a single hour boundary passed. That's
									; OK, so long as the total time wasn't
									; more than an hour.

; Over an hour elapsed passed during timing, which renders
; the results invalid. Notify the user. This misses the case where a
; multiple of 24 hours has passed, but we'll rely on the perspicacity of
; the user to detect that case :-).

@@TestTooLong:
if	flatmodel
		mov		eax,0FFFFFFFFh
else
		mov		ax,0FFFFh
		mov		dx,0FFFFh
endif
		jmp		short @@Done

; Convert the BIOS time to microseconds

@@CalcBIOSTime:
		mov		ax,[WORD EndBIOSCount]
		sub		ax,[WORD StartBIOSCount]
		mov		dx,54925			; Number of microseconds each
									;  BIOS count represents.
		mul		dx
		mov		bx,ax				; set aside BIOS count in
		mov		cx,dx				;  microseconds

; Convert timer count to microseconds

		push	_si
		mov		ax,[EndTimedCount]
		mov		si,8381
		mul		si
		mov		si,10000
		div		si					; * 0.8381 = * 8381 / 10000
		pop		_si

; Add the timer and BIOS counts together to get an overall time in
; microseconds.

		add		ax,bx
		adc		cx,0
if flatmodel
		shl		ecx,16
		mov		cx,ax
		mov		eax,ecx				; EAX := timer count
else
		mov		dx,cx
endif

@@Done:
		unuse_ebx					; Restore EBX for 32 bit code
		ret

procend		_LZTimerCount

procstart   _LZ_disable
		cli
		ret
procend     _LZ_disable

procstart   _LZ_enable
		sti
		ret
procend     _LZ_enable

endcodeseg	lztimer

endif
endif

		END							; End of module
