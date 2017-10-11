;****************************************************************************
;*
;*						  	  PM/Lite Library
;*
;*                  Copyright (C) 1996 SciTech Software
;*							All rights reserved.
;*
;* Filename:    $Workfile:   _pmlite.asm  $
;* Version:     $Revision:   1.0  $
;*
;* Language:	80386 Assembler, TASM 4.0 or later
;* Environment:	IBM PC Real mode and 16/32 bit protected mode
;*
;* Description:	Low level assembly support.
;*
;* $Date:   05 Feb 1996 21:42:18  $ $Author:   KendallB  $
;*
;****************************************************************************

		IDEAL

INCLUDE "model.mac"				; Memory model macros

header      _pmlite             ; Set up memory model

begdataseg	_pmlite

if flatmodel
_PM_savedDS     dw  0           ; Saved value of DS
				PUBLIC  _PM_savedDS
endif

ifdef	X32VM
		DOSX	EQU	1
endif

ifdef	DOSX

; Special external declarations for the DOSX extender that we may need

		$EXTRN	__x386_data_16_alias,WORD
		$EXTRN	__x386_zero_base_selector,WORD
		$EXTRN	__x386_zero_base_ptr,DWORD

endif

enddataseg	_pmlite

ifdef	DOSX

SEGMENT	__X386_DATASEG_16 PARA PUBLIC USE16 'DATA16'
		$EXTRN	__x386_fm,DWORD
ENDS	__X386_DATASEG_16

endif

begcodeseg  _pmlite				; Start of code segment

ifndef  DPMI16
ifndef  __WINDOWS16__
ife 	flatmodel

struc   rmregs_s
ax      dw	?
bx		dw	?
cx		dw	?
dx		dw	?
si		dw	?
di		dw	?
cflag	dw	?
ends	rmregs_s
RMREGS	= (rmregs_s PTR es:bx)

struc	rmsregs_s
es		dw	?
cs		dw	?
ss		dw	?
ds		dw	?
ends	rmsregs_s
RMSREGS	= (rmsregs_s PTR es:bx)

;----------------------------------------------------------------------------
; void PM_callRealMode(unsigned s,unsigned o, RMREGS *regs,
;	RMSREGS *sregs)
;----------------------------------------------------------------------------
; Calls a real mode procedure, loading the appropriate registers values
; from the passed in structures. Only the DS and ES register are loaded
; from the SREGS structure.
;----------------------------------------------------------------------------
procstart	_PM_callRealMode

		ARG     s:WORD, o:WORD, regs:DWORD, sregs:DWORD

		LOCAL	addr:DWORD, bxVal:WORD, esVal:WORD, flags:WORD = LocalSize

		enter_c	LocalSize
		push	ds
		push	es

		mov		ax,[o]				; Build the address to call in 'addr'
		mov		[WORD addr],ax
		mov		ax,[s]
		mov		[WORD addr+2],ax

		les		bx,[sregs]
		mov     ax,[RMSREGS.ds]
		mov     ds,ax				; DS := passed in value
		mov		ax,[RMSREGS.es]
		mov		[esVal],ax
		les		bx,[regs]
		mov		ax,[RMREGS.bx]
		mov		[bxVal],ax
		mov		ax,[RMREGS.ax]		; AX := passed in value
		mov		cx,[RMREGS.cx]		; CX := passed in value
		mov		dx,[RMREGS.dx]		; DX := passed in value
		mov		si,[RMREGS.si] 		; SI := passed in value
		mov		di,[RMREGS.di]		; DI := passed in value
		push	bp
		push	[esVal]
		pop		es					; ES := passed in value
		mov		bx,[bxVal]			; BX := passed in value

		call	[addr]				; Call the specified routine

		pushf						; Save flags for later
		pop		[flags]

		pop		bp
		push	es
		pop		[esVal]
		push	bx
		pop		[bxVal]
		les		bx,[sregs]
		push	ds
		pop		[RMSREGS.ds]		; Save value of DS
		push	[esVal]
		pop		[RMSREGS.es]		; Save value of ES
		les		bx,[regs]
		mov		[RMREGS.ax],ax		; Save value of AX
		mov		[RMREGS.cx],cx		; Save value of CX
		mov		[RMREGS.dx],dx		; Save value of DX
		mov		[RMREGS.si],si		; Save value of SI
		mov		[RMREGS.di],di		; Save value of DI
		mov		ax,[flags]			; Return flags
		and		ax,1h				; Isolate carry flag
		mov		[RMREGS.cflag],ax	; Save carry flag status
		mov		ax,[bxVal]
		mov     [RMREGS.bx],ax		; Save value of BX

		pop		es
		pop		ds
		leave_c
		ret

procend		_PM_callRealMode

endif
endif
endif

;----------------------------------------------------------------------------
; void PM_segread(PMSREGS *sregs)
;----------------------------------------------------------------------------
; Read the current value of all segment registers
;----------------------------------------------------------------------------
procstartdll16	_PM_segread

		ARG		sregs:DPTR

		enter_c	0

		mov		ax,es
		_les	_si,[sregs]
		mov		[_ES _si],ax
		mov		[_ES _si+2],cs
		mov		[_ES _si+4],ss
		mov		[_ES _si+6],ds
		mov		[_ES _si+8],fs
		mov		[_ES _si+10],gs

		leave_c_nolocal
		ret

procenddll16	_PM_segread

; Create a table of the 256 different interrupt calls that we can jump
; into

intno = 0

intTable:
		REPT    256
		db      0CDh
		db      intno
intno = intno + 1
		ret
		nop
		ENDM

PROC genInt near

		push	_ax						; Save _ax
		push	_bx                 	; Save _bx
if flatmodel
		mov    	ebx,[UINT esp+12]		; EBX := interrupt number
else
		mov		bx,sp					; Make sure ESP is zeroed
		mov    	bx,[UINT ss:bx+6]		; BX := interrupt number
endif
		mov     _ax,offset intTable		; Point to interrupt generation table
		shl     _bx,2					; _BX := index into table
		add     _ax,_bx					; _AX := pointer to interrupt code
if flatmodel
		xchg	eax,[esp+4]				; Restore eax, and set for int
else
		mov		bx,sp
		xchg	ax,[ss:bx+2]			; Restore ax, and set for int
endif
		pop		_bx						; restore _bx
		ret

ENDP genInt

;----------------------------------------------------------------------------
; int PM_int386x(int intno, PMREGS *in, PMREGS *out,PMSREGS *sregs)
;----------------------------------------------------------------------------
; Issues a software interrupt in protected mode. This routine has been
; written to allow user programs to load CS and DS with different values
; other than the default.
;----------------------------------------------------------------------------
procstartdll16	_PM_int386x

		ARG		intno:UINT, inptr:DPTR, outptr:DPTR, sregs:DPTR

		LOCAL	flags:UINT, sv_ds:UINT, sv_esi:ULONG = LocalSize

		enter_c	LocalSize
		push	ds
		push	es					; Save segment registers
		push	fs
		push	gs

		_lds	_si,[sregs]			; DS:_SI -> Load segment registers
		mov		es,[_si]
		mov		bx,[_si+6]
		mov		[sv_ds],_bx			; Save value of user DS on stack
		mov		fs,[_si+8]
		mov		gs,[_si+10]

		_lds	_si,[inptr]			; Load CPU registers
		mov     eax,[_si]
		mov		ebx,[_si+4]
		mov		ecx,[_si+8]
		mov		edx,[_si+12]
		mov     edi,[_si+20]
		mov		esi,[_si+16]

		push	ds					; Save value of DS
		push	_bp					; Some interrupts trash this!
		clc							; Generate the interrupt
		push	[intno]
		mov		ds,[WORD sv_ds]		; Set value of user's DS selector
		call	genInt
		pop		_bp					; Pop intno from stack (flags unchanged)
		pop		_bp					; Restore value of stack frame pointer
		pop		ds					; Restore value of DS

		pushf                       ; Save flags for later
		pop		[flags]
		push	esi					; Save ESI for later
		pop		[sv_esi]
		push	ds					; Save DS for later
		pop		[sv_ds]

		_lds	_si,[outptr]		; Save CPU registers
		mov     [_si],eax
		mov		[_si+4],ebx
		mov		[_si+8],ecx
		mov		[_si+12],edx
		push	[sv_esi]
		pop		[DWORD _si+16]
		mov     [_si+20],edi

		mov		_bx,[flags]			; Return flags
		and		ebx,1h				; Isolate carry flag
		mov		[_si+24],ebx		; Save carry flag status

		_lds	_si,[sregs]			; Save segment registers
		mov		[_si],es
		mov		_bx,[sv_ds]
		mov		[_si+6],bx			; Get returned DS from stack
		mov		[_si+8],fs
		mov		[_si+10],gs

		pop		gs					; Restore segment registers
		pop		fs
		pop		es
		pop		ds
		leave_c
		ret

procenddll16	_PM_int386x

if flatmodel

;----------------------------------------------------------------------------
; unsigned char PM_getByte(unsigned s, unsigned o)
;----------------------------------------------------------------------------
procstartdll16	_PM_getByte

		ARG		s:UINT, o:UINT

		push	ebp
		mov		ebp,esp
		push    ds

		mov		ax,[WORD s]
		mov		ds,ax
		mov     eax,[o]
		mov		al,[eax]

		pop		ds
		pop		ebp
		ret

procenddll16	_PM_getByte

;----------------------------------------------------------------------------
; unsigned short PM_getWord(unsigned s, unsigned o)
;----------------------------------------------------------------------------
procstartdll16	_PM_getWord

		ARG		s:UINT, o:UINT

		push	ebp
		mov		ebp,esp
		push    ds

		mov		ax,[WORD s]
		mov		ds,ax
		mov     eax,[o]
		mov		ax,[eax]

		pop		ds
		pop		ebp
		ret

procenddll16	_PM_getWord

;----------------------------------------------------------------------------
; unsigned long PM_getLong(unsigned s, unsigned o)
;----------------------------------------------------------------------------
procstartdll16	_PM_getLong

		ARG		s:UINT, o:UINT

		push	ebp
		mov		ebp,esp
		push    ds

		mov		ax,[WORD s]
		mov		ds,ax
		mov     eax,[o]
		mov		eax,[eax]

		pop		ds
		pop		ebp
		ret

procenddll16	_PM_getLong

;----------------------------------------------------------------------------
; void PM_setByte(unsigned s, unsigned o,unsigned char v)
;----------------------------------------------------------------------------
procstartdll16	_PM_setByte

		ARG		s:UINT, o:UINT, v:UCHAR

		push	ebp
		mov		ebp,esp
		push    ds

		mov		ax,[WORD s]
		mov		ds,ax
		mov     edx,[o]
		mov		al,[v]
		mov		[edx],al

		pop		ds
		pop		ebp
		ret

procenddll16	_PM_setByte

;----------------------------------------------------------------------------
; void PM_setWord(unsigned s, unsigned o,unsigned short v)
;----------------------------------------------------------------------------
procstartdll16	_PM_setWord

		ARG		s:UINT, o:UINT, v:USHORT

		push	ebp
		mov		ebp,esp
		push    ds

		mov		ax,[WORD s]
		mov		ds,ax
		mov     edx,[o]
		mov		ax,[v]
		mov		[edx],ax

		pop		ds
		pop		ebp
		ret

procenddll16	_PM_setWord

;----------------------------------------------------------------------------
; void PM_setLong(unsigned s, unsigned o,unsigned long v)
;----------------------------------------------------------------------------
procstartdll16	_PM_setLong

		ARG		s:UINT, o:UINT, v:ULONG

		push	ebp
		mov		ebp,esp
		push    ds

		mov		ax,[WORD s]
		mov		ds,ax
		mov     edx,[o]
		mov		eax,[v]
		mov		[edx],eax

		pop		ds
		pop		ebp
		ret

procenddll16	_PM_setLong

;----------------------------------------------------------------------------
; void PM_memcpynf(void *dst,unsigned src_s,unsigned src_o,unsigned n)
;----------------------------------------------------------------------------
; Copies a block of memory from a far memory block to a near memory block.
;----------------------------------------------------------------------------
procstartdll16	_PM_memcpynf

		ARG		dst:DPTR, src_s:UINT, src_o:UINT, n:UINT

		enter_c	0
		push	ds

		force_es_eq_ds			; Force ES == DS
		mov		edi,[dst]		; ES:EDI -> destination memory block
		mov		ax,[WORD src_s]
		mov     ds,ax
		mov		esi,[src_o]		; DS:ESI -> source memory block
		mov		ecx,[n]

		mov		eax,ecx
		shr		ecx,2
	rep	movsd
		mov		ecx,eax
		and		ecx,3
	rep	movsb

		pop		ds
		leave_c_nolocal
		ret

procenddll16	_PM_memcpynf

;----------------------------------------------------------------------------
; void PM_memcpyfn(unsigned dst_s,unsigned dst_o,void *src,unsigned n)
;----------------------------------------------------------------------------
; Copies a block of memory from a near memory block to a far memory block.
;----------------------------------------------------------------------------
procstartdll16	_PM_memcpyfn

		ARG		dst_s:UINT, dst_o:UINT, src:DPTR, n:UINT

		enter_c	0
		push	es

		mov		esi,[src]		; DS:ESI -> source memory block
		mov		ax,[WORD dst_s]
		mov     es,ax
		mov		edi,[dst_o]		; ES:EDI -> source memory block
		mov		ecx,[n]

		mov		eax,ecx
		shr		ecx,2
	rep	movsd
		mov		ecx,eax
		and		ecx,3
	rep	movsb

		pop		es
		leave_c_nolocal
		ret

procenddll16	_PM_memcpyfn

ifdef	TNT

;----------------------------------------------------------------------------
; unsigned _PL_allocsel(void)
;----------------------------------------------------------------------------
; Allocate am empty segment selector with Phar Lap (there is no C based
; API for doing this in a compiler portable manner).
;----------------------------------------------------------------------------
procstart	__PL_allocsel

		mov		ah,48h			; Allocate memory service
		xor		ebx,ebx			; EBX := 0 pages to allocate
		int		21h
		jc		@@Fail
		and		eax,0FFFFh		; EAX := segment selector
		ret

@@Fail: xor		eax,eax
		ret

procend		__PL_allocsel

endif

ifdef	DOSX

;----------------------------------------------------------------------------
; ulong _X32_getPhysMem(void)
;----------------------------------------------------------------------------
; Determines the amount of physical memory available under the X32 DOS
; extender. There is no proper way to do this, so what we have to do is
; look in the X32 extender stub and sniff around for the info we need. This
; is what was suggested by Joe Huffman and provided to us by Kevin Aguilar.
;----------------------------------------------------------------------------
procstart	__X32_getPhysMem

		push	es
		mov     es,[__x386_data_16_alias]
		mov     eax,[es:__x386_fm+12]
		sub		eax,[es:__x386_fm+4]
		sub		eax,100 * 1024
		pop		es
		ret

procend		__X32_getPhysMem

endif

endif

ife flatmodel
_PM_savedDS		dw	DGROUP			; Saved value of DS
endif

;----------------------------------------------------------------------------
; void PM_saveDS(void)
;----------------------------------------------------------------------------
; Save the value of DS into a section of the code segment, so that we can
; quickly load this value at a later date in the PM_loadDS() routine from
; inside interrupt handlers etc. The method to do this is different
; depending on the DOS extender being used.
;----------------------------------------------------------------------------
procstartdll16	_PM_saveDS

if		flatmodel
		mov		[_PM_savedDS],ds	; Store away in data segment
endif
		ret

procenddll16	_PM_saveDS

;----------------------------------------------------------------------------
; void PM_loadDS(void)
;----------------------------------------------------------------------------
; Routine to load the DS register with the default value for the current
; DOS extender. Only the DS register is loaded, not the ES register, so
; if you wish to call C code, you will need to also load the ES register
; in 32 bit protected mode.
;----------------------------------------------------------------------------
procstartdll16	_PM_loadDS

		mov		ds,[cs:_PM_savedDS]	; We can access the proper DS through CS
		ret

procenddll16	_PM_loadDS

endcodeseg  _pmlite

		END						; End of module

