;****************************************************************************
;*
;*						  	  PM/Pro Library
;*
;*                  Copyright (C) 1996 SciTech Software
;*							All rights reserved.
;*
;* Filename:    $Workfile:   _pmpro.asm  $
;* Version:     $Revision:   1.0  $
;*
;* Language:	80386 Assembler, TASM 4.0 or later
;* Environment:	IBM PC Real mode and 16/32 bit protected mode
;*
;* Description:	Low level assembly support for interrupt handling.
;*
;* $Date:   05 Feb 1996 21:42:18  $ $Author:   KendallB  $
;*
;****************************************************************************

		IDEAL

INCLUDE "model.mac"				; Memory model macros

ifndef	__WINDOWS__

header      _pmpro              ; Set up memory model

begdataseg	_pmpro

if flatmodel
		$EXTRN  _PM_savedDS,USHORT
endif

enddataseg	_pmpro

; Macro to load DS and ES registers with correct value.

MACRO   LOAD_DS
if		flatmodel
		mov		ds,[cs:_PM_savedDS]
		mov		es,[cs:_PM_savedDS]
else
		push	ax
		mov		ax,DGROUP
		mov		ds,ax
        pop		ax
endif
ENDM

; Note that interrupts we disable interrupts during the following stack
; macro for correct operation, but we do not enable them again. Normally
; these macros are used within interrupt handlers so interrupts should
; already be off. We turn them back on explicitly later if the user code
; needs them to be back on.

; Macro to switch to a new local stack.

MACRO	NEWSTK	stkname
		cli
		mov		[&stkname&Seg],ss
		mov		[&stkname&Ptr],_sp
		mov		[TempSeg],ds
		mov		ss,[TempSeg]
		mov		_sp,offset stkname
ENDM

; Macro to switch back to the old stack.

MACRO   RESTSTK	stkname
		cli
		mov		ss,[&stkname&Seg]
		mov		_sp,[&stkname&Ptr]
ENDM

; Macro to swap the current stack with the one saved away.

MACRO	SWAPSTK	stkname
		cli
		mov		ax,ss
		xchg	ax,[&stkname&Seg]
		mov     ss,ax
		xchg	_sp,[&stkname&Ptr]
ENDM

begdataseg  _pmpro

	$EXTRN  __PM_critHandler,CPTR
	$EXTRN  __PM_breakHandler,CPTR
	$EXTRN  __PM_timerHandler,CPTR
    $EXTRN  __PM_keyHandler,CPTR
	$EXTRN  __PM_key15Handler,CPTR
	$EXTRN  __PM_mouseHandler,CPTR

	$EXTRN	__PM_ctrlCSel,UINT
	$EXTRN	__PM_ctrlCOff,UINT
	$EXTRN	__PM_ctrlBSel,UINT
	$EXTRN	__PM_ctrlBOff,UINT
	$EXTRN	__PM_critSel,UINT
	$EXTRN	__PM_critOff,UINT

	$EXTRN  __PM_prevTimer,FCPTR
    $EXTRN  __PM_prevKey,FCPTR
	$EXTRN  __PM_prevKey15,FCPTR
	$EXTRN  __PM_prevVBE,FCPTR
	$EXTRN  __PM_prevBreak,FCPTR
	$EXTRN	__PM_prevCtrlC,FCPTR
	$EXTRN  __PM_prevCritical,FCPTR
	$EXTRN  __PM_prevRealTimer,ULONG
	$EXTRN  __PM_prevRealKey,ULONG
	$EXTRN  __PM_prevRealKey15,ULONG

__PM_pmproDataStart:
			PUBLIC	__PM_pmproDataStart

; Allocate space for all of the local stacks that we need. These stacks
; are not very large, but should be large enough for most purposes
; (generally you want to handle these interrupts quickly, simply storing
; the information for later and then returning). If you need bigger
; stacks then change the appropriate value in here.

            ALIGN   stackalign
			db	1024 dup (?)	; Space for local stack (small)
MsStack:						; Stack starts at end!
MsStackPtr	UINT	?			; Place to store old stack offset
MsStackSeg	dw		?			; Place to store old stack segment

			ALIGN   stackalign
			db	256 dup (?)		; Space for local stack (small)
TmStack:						; Stack starts at end!
TmStackPtr	UINT	?			; Place to store old stack offset
TmStackSeg	dw		?			; Place to store old stack segment

			ALIGN   stackalign
			db	256 dup (?)		; Space for local stack (small)
KyStack:						; Stack starts at end!
KyStackPtr	UINT	?			; Place to store old stack offset
KyStackSeg	dw		?			; Place to store old stack segment
KyInside	dw		0			; Are we still handling current interrupt?

			ALIGN   stackalign
			db	256 dup (?)		; Space for local stack (small)
Ky15Stack:						; Stack starts at end!
Ky15StackPtr	UINT	?		; Place to store old stack offset
Ky15StackSeg	dw		?		; Place to store old stack segment

label		TempStack FDPTR
TempPtr		UINT	?			; Place to store stack offset
TempSeg		dw		?			; Place to store stack segment

__PM_pmproDataEnd:
			PUBLIC	__PM_pmproDataEnd

enddataseg  _pmpro

begcodeseg  _pmpro              ; Start of code segment

__PM_pmproCodeStart:
			PUBLIC	__PM_pmproCodeStart

;----------------------------------------------------------------------------
; _PM_mouseISR - Mouse interrupt subroutine dispatcher
;----------------------------------------------------------------------------
; Interrupt subroutine called by the mouse driver upon interrupts, to
; dispatch control to high level C based subroutines. Interrupts are on
; when we call the user code.
;
; It is _extremely_ important to save the state of the extended registers
; as these may well be trashed by the routines called from here and not
; restored correctly by the mouse interface module.
;
; NOTE: This routine switches to a local stack before calling any C code,
;		and hence is _not_ re-entrant. For mouse handlers this is not a
;		problem, as the mouse driver arbitrates calls to the user mouse
;		handler for us.
;
; Entry:	AX	- Condition mask giving reason for call
;			BX	- Mouse button state
;			CX	- Horizontal cursor coordinate
;			DX	- Vertical cursor coordinate
;			SI	- Horizontal mickey value
;			DI	- Vertical mickey value
;
;----------------------------------------------------------------------------
procfar	__PM_mouseISR

		push	ds				; Save value of DS
		push	es
		pushad					; Save _all_ extended registers
		cld						; Clear direction flag

		LOAD_DS					; Load DS register
		NEWSTK  MsStack			; Switch to local stack

; Call the installed high level C code routine

		clrhi	dx				; Clear out high order values
		clrhi	cx
		clrhi	bx
		clrhi	ax
		clrhi	si
		clrhi	di

		push	_di
		push	_si
		push    _dx
		push    _cx
		push	_bx
		push	_ax
		sti						; Enable interrupts
		call	[__PM_mouseHandler]
		_add	sp,12,24

		RESTSTK	MsStack			; Restore previous stack

		popad					; Restore all extended registers
		pop		es
		pop		ds
		ret						; We are done!!

procend	__PM_mouseISR

;----------------------------------------------------------------------------
; _PM_timerISR - Timer interrupt subroutine dispatcher
;----------------------------------------------------------------------------
; Hardware interrupt handler for the timer interrupt, to dispatch control
; to high level C based subroutines. We save the state of all registers
; in this routine, and switch to a local stack. Interrupts are *off*
; when we call the user code.
;
; NOTE: This routine switches to a local stack before calling any C code,
;		and hence is _not_ re-entrant. Make sure your C code executes as
;		quickly as possible, since a timer overrun will simply hang the
;		system.
;----------------------------------------------------------------------------
procfar	__PM_timerISR

		push	ds				; Save value of DS
		push	es
		pushad					; Save _all_ extended registers
		cld						; Clear direction flag

		LOAD_DS					; Load DS register

		NEWSTK	TmStack			; Switch to local stack
		call	[__PM_timerHandler]	; Call C code
		RESTSTK	TmStack			; Restore previous stack

		popad					; Restore all extended registers
		pop		es
		pop		ds
		iret					; Return from interrupt

procend	__PM_timerISR

;----------------------------------------------------------------------------
; PM_chainPrevTimer - Chain to previous timer interrupt and return
;----------------------------------------------------------------------------
; Chains to the previous timer interrupt routine and returns control
; back to the high level interrupt handler.
;----------------------------------------------------------------------------
procstartdll	_PM_chainPrevTimer

ifdef	TNT
		push	eax
		push	ebx
		push	ecx
		pushfd					; Push flags on stack to simulate interrupt
		mov     ax,250Eh		; Call real mode procedure function
		mov		ebx,[__PM_prevRealTimer]
		mov		ecx,1			; Copy real mode flags to real mode stack
		int		21h				; Call the real mode code
		popfd
		pop		ecx
		pop		ebx
		pop		eax
		ret
else
		SWAPSTK	TmStack			; Swap back to previous stack
		pushf					; Save state of interrupt flag
		pushf                   ; Push flags on stack to simulate interrupt
		call	[__PM_prevTimer]
		popf					; Restore state of interrupt flag
		SWAPSTK	TmStack			; Swap back to C stack again
		ret
endif

procend			_PM_chainPrevtimer

;----------------------------------------------------------------------------
; _PM_keyISR - keyboard interrupt subroutine dispatcher
;----------------------------------------------------------------------------
; Hardware interrupt handler for the keyboard interrupt, to dispatch control
; to high level C based subroutines. We save the state of all registers
; in this routine, and switch to a local stack. Interrupts are *off*
; when we call the user code.
;
; NOTE: This routine switches to a local stack before calling any C code,
;		and hence is _not_ re-entrant. However we ensure within this routine
;		mutual exclusion to the keyboard handling routine.
;----------------------------------------------------------------------------
procfar	__PM_keyISR

		push	ds				; Save value of DS
		push	es
		pushad					; Save _all_ extended registers
		cld						; Clear direction flag

		LOAD_DS					; Load DS register

		cmp		[KyInside],1	; Check for mutual exclusion
		je		@@Reissued

		mov		[KyInside],1
		NEWSTK	KyStack			; Switch to local stack
		call	[__PM_keyHandler]	; Call C code
		RESTSTK	KyStack			; Restore previous stack
		mov		[KyInside],0

@@Exit:	popad					; Restore all extended registers
		pop		es
		pop		ds
		iret					; Return from interrupt

; When the BIOS keyboard handler needs to change the SHIFT status lights
; on the keyboard, in the process of doing this the keyboard controller
; re-issues another interrupt, while the current handler is still executing.
; If we recieve another interrupt while still handling the current one,
; then simply chain directly to the previous handler.
;
; Note that for most DOS extenders, the real mode interrupt handler that we
; install takes care of this for us.

@@Reissued:
ifdef	TNT
		push	eax
		push	ebx
		push	ecx
		pushfd					; Push flags on stack to simulate interrupt
		mov     ax,250Eh		; Call real mode procedure function
		mov		ebx,[__PM_prevRealKey]
		mov		ecx,1			; Copy real mode flags to real mode stack
		int		21h				; Call the real mode code
		popfd
		pop		ecx
		pop		ebx
		pop		eax
else
		pushf
		call	[__PM_prevKey]
endif
		jmp		@@Exit

procend	__PM_keyISR

;----------------------------------------------------------------------------
; PM_chainPrevkey - Chain to previous key interrupt and return
;----------------------------------------------------------------------------
; Chains to the previous key interrupt routine and returns control
; back to the high level interrupt handler.
;----------------------------------------------------------------------------
procstartdll	_PM_chainPrevKey

ifdef	TNT
		push	eax
		push	ebx
		push	ecx
		pushfd					; Push flags on stack to simulate interrupt
		mov     ax,250Eh		; Call real mode procedure function
		mov		ebx,[__PM_prevRealKey]
		mov		ecx,1			; Copy real mode flags to real mode stack
		int		21h				; Call the real mode code
		popfd
		pop		ecx
		pop		ebx
		pop		eax
		ret
else

; YIKES! For some strange reason, when execution returns from the
; previous keyboard handler, interrupts are re-enabled!! Since we expect
; interrupts to remain off during the duration of our handler, this can
; cause havoc. However our stack macros always turn off interrupts, so they
; will be off when we exit this routine. Obviously there is a tiny weeny
; window when interrupts will be enabled, but there is nothing we can
; do about this.

		SWAPSTK	KyStack			; Swap back to previous stack
		pushf					; Push flags on stack to simulate interrupt
		call	[__PM_prevKey]
		SWAPSTK	KyStack			; Swap back to C stack again
		ret
endif

procend			_PM_chainPrevKey

;----------------------------------------------------------------------------
; _PM_key15ISR - Int 15h keyboard interrupt subroutine dispatcher
;----------------------------------------------------------------------------
; This routine gets called if we have been called to handle the Int 15h
; keyboard interrupt callout from real mode.
;
;	Entry:	AX	- Hardware scan code to process
;	Exit:   AX	- Hardware scan code to process (0 to ignore)
;----------------------------------------------------------------------------
procfar	__PM_key15ISR

		push	ds
		push	es
		LOAD_DS
		cmp		ah,4Fh
		jnz		@@NotOurs		; Quit if not keyboard callout

		pushad
		cld						; Clear direction flag
		xor		ah,ah			; AX := scan code
		NEWSTK	Ky15Stack		; Switch to local stack
		push	_ax
		call	[__PM_key15Handler]	; Call C code
		_add	sp,2,4
		RESTSTK	Ky15Stack		; Restore previous stack
		test	ax,ax
		jz		@@1
		stc						; Set carry to process as normal
		jmp		@@2
@@1:	clc     				; Clear carry to ignore scan code
@@2:	popad
		jmp		@@Exit			; We are done

@@NotOurs:
ifdef	TNT
		push	eax
		push	ebx
		push	ecx
		pushfd					; Push flags on stack to simulate interrupt
		mov     ax,250Eh		; Call real mode procedure function
		mov		ebx,[__PM_prevRealKey15]
		mov		ecx,1			; Copy real mode flags to real mode stack
		int		21h				; Call the real mode code
		popfd
		pop		ecx
		pop		ebx
		pop		eax
else
		pushf
		call	[__PM_prevKey15]
endif
@@Exit:	pop		es
		pop		ds
if flatmodel
		retf	4
else
		retf	2
endif

procend	__PM_key15ISR

;----------------------------------------------------------------------------
; _PM_breakISR - Control Break interrupt subroutine dispatcher
;----------------------------------------------------------------------------
; Hardware interrupt handler for the Ctrl-Break interrupt. We simply set
; the Ctrl-Break flag to a 1 and leave (note that this is accessed through
; a far pointer, as it may well be located in conventional memory).
;----------------------------------------------------------------------------
procfar	__PM_breakISR

		sti
		push	ds				; Save value of DS
		push	es
		push	_bx

		LOAD_DS					; Load DS register
		mov     bx,[WORD __PM_ctrlBSel]
		mov		es,bx
		mov		_bx,[__PM_ctrlBOff]
		mov		[UINT es:_bx],1

; Run alternate break handler code if installed

		cmp		[__PM_breakHandler],0
		je		@@Exit

		pushad
		mov		_ax,1
		push	_ax
		call	[__PM_breakHandler]	; Call C code
		pop		_ax
		popad

@@Exit:	pop		_bx
		pop		es
		pop		ds
		iret					; Return from interrupt

procend	__PM_breakISR

;----------------------------------------------------------------------------
; int PM_ctrlBreakHit(int clearFlag)
;----------------------------------------------------------------------------
; Returns the current state of the Ctrl-Break flag and possibly clears it.
;----------------------------------------------------------------------------
procstartdll	_PM_ctrlBreakHit

		ARG     clearFlag:UINT

		enter_c	0
		pushf					; Save interrupt status
		push	es
		mov     ax,[WORD __PM_ctrlBSel]
		mov		es,ax
		mov		_bx,[__PM_ctrlBOff]
		cli						; No interrupts thanks!
		mov		_ax,[es:_bx]
		test	[clearFlag],1
		jz		@@Done
		mov		[UINT es:_bx],0

@@Done:	pop		es
		popf					; Restore interrupt status
        leave_c_nolocal
		ret

procend			_PM_ctrlBreakHit

;----------------------------------------------------------------------------
; _PM_ctrlCISR - Control Break interrupt subroutine dispatcher
;----------------------------------------------------------------------------
; Hardware interrupt handler for the Ctrl-C interrupt. We simply set
; the Ctrl-C flag to a 1 and leave (note that this is accessed through
; a far pointer, as it may well be located in conventional memory).
;----------------------------------------------------------------------------
procfar	__PM_ctrlCISR

		sti
		push	ds				; Save value of DS
		push	es
		push	_bx

		LOAD_DS					; Load DS register
		mov     bx,[WORD __PM_ctrlCSel]
		mov		es,bx
		mov		_bx,[__PM_ctrlCOff]
		mov		[UINT es:_bx],1

; Run alternate break handler code if installed

		cmp		[__PM_breakHandler],0
		je		@@Exit

		pushad
		mov		_ax,0
		push	_ax
		call	[__PM_breakHandler]	; Call C code
		pop		_ax
		popad

@@Exit:	pop		_bx
		pop		es
		pop		ds
		iret					; Return from interrupt
		iretd

procend __PM_ctrlCISR

;----------------------------------------------------------------------------
; int PM_ctrlCHit(int clearFlag)
;----------------------------------------------------------------------------
; Returns the current state of the Ctrl-C flag and possibly clears it.
;----------------------------------------------------------------------------
procstartdll	_PM_ctrlCHit

		ARG     clearFlag:UINT

		enter_c	0
		pushf					; Save interrupt status
		push	es
		mov     ax,[WORD __PM_ctrlCSel]
		mov		es,ax
		mov		_bx,[__PM_ctrlCOff]
		cli						; No interrupts thanks!
		mov		_ax,[es:_bx]
		test	[clearFlag],1
		jz		@@Done
		mov		[UINT es:_bx],0

@@Done:
		pop		es
		popf					; Restore interrupt status
        leave_c_nolocal
		ret

procend			_PM_ctrlCHit

;----------------------------------------------------------------------------
; _PM_criticalISR - Control Error handler interrupt subroutine dispatcher
;----------------------------------------------------------------------------
; Interrupt handler for the MSDOS Critical Error interrupt, to dispatch
; control to high level C based subroutines. We save the state of all
; registers in this routine, and switch to a local stack. We also pass
; the values of the AX and DI registers to the as pointers, so that the
; values can be modified before returning to MSDOS.
;----------------------------------------------------------------------------
procfar	__PM_criticalISR

		sti
		push	ds				; Save value of DS
		push	es
		push    _bx				; Save register values changed
		cld						; Clear direction flag

		LOAD_DS					; Load DS register
		mov     bx,[WORD __PM_critSel]
		mov		es,bx
		mov		_bx,[__PM_critOff]
		mov		[es:_bx],ax
		mov		[es:_bx+2],di

; Run alternate critical handler code if installed

		cmp		[__PM_critHandler],0
		je		@@NoAltHandler

		pushad
		push	_di
		push	_ax
		call	[__PM_critHandler]	; Call C code
		_add	sp,4,8
		popad

		pop		_bx
		pop		es
		pop		ds
		iret					; Return from interrupt

@@NoAltHandler:
		mov		ax,3			; Tell MSDOS to fail the operation
		pop		_bx
		pop		es
		pop		ds
		iret					; Return from interrupt

procend	__PM_criticalISR

;----------------------------------------------------------------------------
; int PM_criticalError(int *axVal,int *diVal,int clearFlag)
;----------------------------------------------------------------------------
; Returns the current state of the critical error flags, and the values that
; MSDOS passed in the AX and DI registers to our handler.
;----------------------------------------------------------------------------
procstartdll	_PM_criticalError

		ARG     axVal:DPTR, diVal:DPTR, clearFlag:UINT

		enter_c	0
		pushf					; Save interrupt status
		push	es
		mov     ax,[WORD __PM_critSel]
		mov		es,ax
		mov		_bx,[__PM_critOff]
		cli						; No interrupts thanks!
		xor		_ax,_ax
		xor		_di,_di
		mov		ax,[es:_bx]
		mov		di,[es:_bx+2]
		test	[clearFlag],1
		jz		@@NoClear
		mov		[ULONG es:_bx],0
@@NoClear:
		_les	_bx,[axVal]
		mov		[_ES _bx],_ax
		_les	_bx,[diVal]
		mov		[_ES _bx],_di
		pop		es
		popf					; Restore interrupt status
		leave_c_nolocal
		ret

procend			_PM_criticalError

;----------------------------------------------------------------------------
; void _PM_setMouseHandler(int mask, PM_mouseHandler mh)
;----------------------------------------------------------------------------
procstart	__PM_setMouseHandler

		ARG		mouseMask:UINT

		enter_c	0
		push	es

		mov		ax,0Ch			; AX := Function 12 - install interrupt sub
		mov		_cx,[mouseMask]	; CX := mouse mask
		mov		_dx,offset __PM_mouseISR
		push    cs
		pop		es				; ES:_DX -> mouse handler
		int		33h             ; Call mouse driver

		pop		es
		leave_c_nolocal
		ret

procend	 	__PM_setMouseHandler

if flatmodel

;----------------------------------------------------------------------------
; void _PM_mousePMCB(void)
;----------------------------------------------------------------------------
; Mouse realmode callback routine. Upon entry to this routine, we recieve
; the following from the DPMI server:
;
;	Entry: 	DS:_SI	-> Real mode stack at time of call
;           ES:_DI	-> Real mode register data structure
;           SS:_SP	-> Locked protected mode stack to use
;----------------------------------------------------------------------------
procfar	__PM_mousePMCB

		pushad
		mov		eax,[es:_di+1Ch]	; Load register values from real mode
		mov		ebx,[es:_di+10h]
		mov		ecx,[es:_di+18h]
		mov		edx,[es:_di+14h]
		mov		esi,[es:_di+04h]
		mov		edi,[es:_di]
		call	__PM_mouseISR		; Call the mouse handler
		popad

		mov		ax,[ds:_si]
		mov     [es:_di+2Ah],ax		; Plug in return IP address
		mov		ax,[ds:_si+2]
		mov		[es:_di+2Ch],ax		; Plug in return CS value
		add     [WORD es:_di+2Eh],4	; Remove return address from stack
		iret						; Go back to real mode!

procend		__PM_mousePMCB

;----------------------------------------------------------------------------
; void _DPMI_allocateCallback(void (*pmcode)(), void *rmregs, long *RMCB)
;----------------------------------------------------------------------------
procstart	__DPMI_allocateCallback

		ARG		pmcode:CPTR, rmregs:DPTR, RMCB:DPTR

		enter_c	0
		push	ds
		push	es

		push	cs
		pop		ds
		mov		esi,[pmcode]	; DS:ESI -> protected mode code to call
		mov		edi,[rmregs]	; ES:EDI -> real mode register buffer
		mov		ax,303h			; AX := allocate realmode callback function
		int		31h

		mov     eax,[RMCB]
		shl		ecx,16
		mov		cx,dx
		mov		[es:eax],ecx	; Return real mode address

		pop		es
		pop		ds
        leave_c_nolocal
		ret

procend		__DPMI_allocateCallback

;----------------------------------------------------------------------------
; void _DPMI_freeCallback(long RMCB)
;----------------------------------------------------------------------------
procstart	__DPMI_freeCallback

		ARG		RMCB:ULONG

		enter_c	0

		mov     cx,[WORD RMCB+2]
		mov		dx,[WORD RMCB]	; CX:DX := real mode callback
		mov		ax,304h
		int		31h

        leave_c_nolocal
		ret

procend		__DPMI_freeCallback

endif

__PM_pmproCodeEnd:
			PUBLIC	__PM_pmproCodeEnd

endcodeseg  _pmpro

endif

		END						; End of module

