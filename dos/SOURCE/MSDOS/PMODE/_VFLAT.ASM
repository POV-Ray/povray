;****************************************************************************
;*
;*				VFlat - DOS Virtual Flat Linear Framebuffer
;*
;*                  Copyright (C) 1996 SciTech Software
;*							All rights reserved.
;*
;* 			  Based on original code Copyright 1994 Otto Chrons
;*
;* Filename:    $Workfile:   _vflat.asm  $
;* Version:     $Revision:   1.1  $
;*
;* Language:	80386 Assembler, TASM 4.0 or later
;* Environment:	IBM PC 32 bit protected mode
;*
;* Description:	Low level page fault handler for re-mapping the SuperVGA
;*				to the new bank.
;*
;* $Date:   12 Feb 1996 21:58:56  $ $Author:   KendallB  $
;*
;****************************************************************************

		IDEAL
		JUMPS

INCLUDE "model.mac"				; Memory model macros

header      _vflat              ; Set up memory model

VFLAT_START		= 0F0000000h
VFLAT_END		= 0F03FFFFFh
PAGE_PRESENT 	= 1
PAGE_NOTPRESENT = 0
PAGE_READ 		= 0
PAGE_WRITE		= 2

ifdef	DOS4GW

;----------------------------------------------------------------------------
; DOS4G/W flat linear framebuffer emulation.
;----------------------------------------------------------------------------

begdataseg  _vflat

; Near pointers to the page directory base and our page tables. All of
; this memory is always located in the first Mb of DOS memory.

PDBR            dd 	0            	; Page directory base register (CR3)
accessPageAddr  dd 	0
accessPageTable dd 	0

; Place to store a copy of the original Page Table Directory before we
; intialised our virtual buffer code.

pageDirectory   dd 	1024 DUP(?)  	; Saved page table directory

ValidCS			dw	0				; Valid CS for page faults
Ring0CS			dw	0				; Our ring 0 code selector
LastPage		dd	0				; Last page we mapped in
BankFuncBuf		db	101 dup (?)		; Place to store bank switch code
BankFuncPtr		dd	offset BankFuncBuf

label	INT14Gate	FWORD
INT14Offset     dd      0  			; eip of original vector
INT14Selector   dw      0			; cs of original vector

		$EXTRN  _PM_savedDS,USHORT
		$EXTRN	_VF_zeroSel,USHORT
		$EXTRN	_VF_haveCauseWay,BOOL

enddataseg  _vflat

begcodeseg  _vflat              ; Start of code segment

		EXTRN   _VF_malloc:FPTR

;----------------------------------------------------------------------------
; EnableRing0
;----------------------------------------------------------------------------
; Enables ring 0 operation using DOS extender specific functions. Note
; that this is a 16 bit protected mode function, not 32 bit code so it needs
; to remain in a separate file.
;----------------------------------------------------------------------------
PROC    EnableRing0

		pusha
		cmp		[_VF_haveCauseWay],0
		je		@@Exit

; TODO: Enable Ring 0 under CauseWay

@@Exit:	popa
		ret

ENDP

;----------------------------------------------------------------------------
; DisableRing0
;----------------------------------------------------------------------------
; Enables ring 0 operation using DOS extender specific functions.
;----------------------------------------------------------------------------
PROC    DisableRing0

		pusha
		cmp		[_VF_haveCauseWay],0
		je		@@Exit

; TODO: Disable Ring 0 under CauseWay

@@Exit:	popa
		ret

ENDP

;----------------------------------------------------------------------------
; PF_handler64k - Page fault handler for 64k banks
;----------------------------------------------------------------------------
; The handler below is a 32 bit ring 0 page fault handler.  It receives
; control immediately after any page fault or after an IRQ6 (hardware
; interrupt). This provides the fastest possible handling of page faults
; since it jump directly here.  If this is a page fault, the number
; immediately on the stack will be an error code, at offset 4 will be
; the eip of the faulting instruction, at offset 8 will be the cs of the
; faulting instruction.  If it is a hardware interrupt, it will not have
; the error code and the eflags will be at offset 8.
;----------------------------------------------------------------------------
PROC	PF_handler64k	far

; Check if this is a processor exeception or a page fault

		push	eax
		mov		ax,[cs:ValidCS]		; Use CS override to access data
		cmp		[ss:esp+12],ax		; Is this a page fault?
		jne		@@ToOldHandler		; Nope, jump to the previous handler

; Get address of page fault and check if within our handlers range

		mov     eax,cr2             ; EBX has page fault linear address
		cmp     eax,VFLAT_START		; Is the fault less than ours?
		jb      @@ToOldHandler		; Yep, go to previous handler
		cmp     eax,VFLAT_END		; Is the fault more than ours?
		jae     @@ToOldHandler		; Yep, go to previous handler

; This is our page fault, so we need to handle it

		pushad
		push    es ds
		mov		ebx,eax				; EBX := page fault address
		and		ebx,not 0FFFFh		; Mask to 64k bank boundary
		mov		ds,[cs:_PM_savedDS]	; Load segment registers
		mov     es,[_VF_zeroSel]

; Map in the page table for our virtual framebuffer area for modification

		mov     edi,[PDBR]      	; EDI points to page directory
		mov     edx,ebx            	; EDX = linear address
		shr     edx,22              ; EDX = offset to page directory
		mov     edx,[edx*4+edi]     ; EDX = physical page table address
		mov     eax,edx
		mov     edx,[accessPageTable]
		or      eax,7
		mov     [edx],eax
		mov     eax,cr3
		mov     cr3,eax				; Update page table cache

; Mark all pages valid for the new page fault area

		mov		esi,ebx				; ESI := linear address for page
		shr     esi,10
		and     esi,0FFFh           ; Offset into page table
		add		esi,[accessPageAddr]
off = 0
REPT	16
		or    	[DWORD esi+off],0000000001h	; Enable pages
off = off+4
ENDM

; Mark all pages invalid for the previously mapped area

		xchg	esi,[LastPage]		; Save last page for next page fault
		test	esi,esi
		jz		@@DoneMapping		; Dont update if first time round
off = 0
REPT	16
		and		[DWORD esi+off],0FFFFFFFEh	; Disable pages
off = off+4
ENDM

@@DoneMapping:
		mov     eax,cr3
		mov     cr3,eax           	; Flush the TLB

; Now program the new SuperVGA starting bank address

		mov		eax,ebx				; EAX := page fault address
		shr     eax,16
		and		eax,0FFh			; Mask to 0-255
		call    [BankFuncPtr]		; Call the bank switch function

		pop     ds es
		popad
		pop		eax
		add     esp,4             	; Pop the error code from stack
		iretd                      	; Return to faulting instruction

@@ToOldHandler:
		pop		eax
		jmp		[cs:INT14Gate]		; Chain to previous handler

ENDP	PF_handler64k

;----------------------------------------------------------------------------
; PF_handler4k  - Page fault handler for 4k banks
;----------------------------------------------------------------------------
; The handler below is a 32 bit ring 0 page fault handler.  It receives
; control immediately after any page fault or after an IRQ6 (hardware
; interrupt). This provides the fastest possible handling of page faults
; since it jump directly here.  If this is a page fault, the number
; immediately on the stack will be an error code, at offset 4 will be
; the eip of the faulting instruction, at offset 8 will be the cs of the
; faulting instruction.  If it is a hardware interrupt, it will not have
; the error code and the eflags will be at offset 8.
;----------------------------------------------------------------------------
PROC	PF_handler4k	far

; Fill in when we have tested all the 64Kb code

		jmp		[cs:INT14Gate]		; Chain to previous handler

ENDP	PF_handler4k

;----------------------------------------------------------------------------
; void InstallFaultHandler(void *baseAddr,int bankSize)
;----------------------------------------------------------------------------
; Installes the page fault handler directly int the interrupt descriptor
; table for maximum performance. This of course requires ring 0 access,
; but none of this stuff will run without ring 0!
;----------------------------------------------------------------------------
procstart	_InstallFaultHandler

		ARG     baseAddr:ULONG, bankSize:UINT

		enter_c 0

		mov		[LastPage],0		; No pages have been mapped
		mov		ax,cs
		mov		[ValidCS],ax		; Save CS value for page faults

; Put address of our page fault handler into the IDT directly

		sub		esp,6               ; Allocate space on stack
		sidt	[FWORD ss:esp]		; Store pointer to IDT
		pop     ax               	; add esp,2
		pop     eax                 ; Absolute address of IDT
		add     eax,14*8          	; Point to Int #14

; Note that Interrupt gates do not have the high and low word of the
; offset in adjacent words in memory, there are 4 bytes separating them.

		mov     ecx,[eax]   		; Get cs and low 16 bits of offset
		mov     edx,[eax+6]         ; Get high 16 bits of offset in dx
		shl     edx,16
		mov     dx,cx             	; edx has offset
		mov     [INT14Offset],edx	; Save offset
		shr     ecx,16
		mov     [INT14Selector],cx	; Save original cs
		mov     [eax+2],cs          ; Install new cs
		mov     edx,offset PF_handler64k
		cmp		[bankSize],4
		jne		@@1
		mov		edx,offset PF_handler4k
@@1:	mov     [eax],dx          	; Install low word of offset
		shr     edx,16
		mov     [eax+6],dx          ; Install high word of offset

		leave_c_nolocal
		ret

procend		_InstallFaultHandler

;----------------------------------------------------------------------------
; void RemoveFaultHandler(void)
;----------------------------------------------------------------------------
; Closes down the virtual framebuffer services and restores the previous
; page fault handler.
;----------------------------------------------------------------------------
procstart	_RemoveFaultHandler

		enter_c	0

; Remove page fault handler from IDT

		sub		esp,6               ; Allocate space on stack
		sidt	[FWORD ss:esp]		; Store pointer to IDT
		pop     ax               	; add esp,2
		pop     eax                 ; Absolute address of IDT
		add     eax,14*8          	; Point to Int #14
		mov     cx,[INT14Selector]
		mov     [eax+2],cx          ; Restore original CS
		mov     edx,[INT14Offset]
		mov     [eax],dx          	; Install low word of offset
		shr     edx,16
		mov     [eax+6],dx          ; Install high word of offset

		leave_c_nolocal
		ret

procend		_RemoveFaultHandler

;----------------------------------------------------------------------------
; void InstallBankFunc(int codeLen,void *bankFunc)
;----------------------------------------------------------------------------
; Installs the bank switch function by relocating it into our data segment
; and making it into a callable function. We do it this way to make the
; code identical to the way that the VflatD devices work under Windows.
;----------------------------------------------------------------------------
procstart	_InstallBankFunc

		ARG		codeLen:UINT, bankFunc:DPTR

		enter_c	0

		mov		esi,[bankFunc]		; Copy the code into buffer
		mov		edi,offset bankFuncBuf
		mov		ecx,[codeLen]
    rep movsb
		mov		[BYTE edi],0C3h		; Terminate the function with a near ret

		leave_c_nolocal
		ret

procend		_InstallBankFunc

;----------------------------------------------------------------------------
; int InitPaging(void)
;----------------------------------------------------------------------------
; Initializes paging system. If paging is not enabled, builds a page table
; directory and page tables for physical memory
;
;	Exit:		0	- Successful
;				-1	- Couldn't initialize paging mechanism
;----------------------------------------------------------------------------
procstart	_InitPaging

		push    ebx ecx edx esi edi

		call	EnableRing0				; Enable ring 0 mode

		mov     ax,cs
		test    ax,3                    ; Which ring are we running
		jnz     @@errexit               ; Needs zero ring to access
										; page tables (CR3)
		mov     eax,cr0                 ; Load CR0
		test    eax,80000000h           ; Is paging enabled?
		jz    	@@errExit				; No, we must have paging!

		mov     eax,cr3                 ; Load directory address
		and     eax,0FFFFF000h
		mov     [PDBR],eax              ; Save it
		mov     esi,eax
        mov     edi,offset pageDirectory
        mov     ecx,1024
        cld
		rep     movsd                   ; Copy the original page table directory
		cmp		[accessPageAddr],0		; Check if we have allocated page
		jne		@@HaveRealMem			; table already (we cant free it)

		mov     eax,0100h               ; DPMI DOS allocate
		mov     ebx,8192/16
		int     31h                     ; Allocate 8192 bytes
		and     eax,0FFFFh
		shl     eax,4                   ; EAX points to newly allocated memory
		add     eax,4095
		and     eax,0FFFFF000h          ; Page align
		mov     [accessPageAddr],eax

@@HaveRealMem:
		mov		eax,[accessPageAddr]	; EAX -> page table in 1st Mb
		shr     eax,12
        and     eax,3FFh                ; Page table offset
        shl     eax,2
        mov     ebx,[PDBR]
        mov     ebx,[ebx]
		and     ebx,0FFFFF000h          ; Page table for 1st megabyte
		add     eax,ebx
		mov     [accessPageTable],eax
		sub     eax,eax                 ; No error
		jmp     @@exit
@@errExit:
		mov     eax,-1
@@exit:	call	DisableRing0			; Return to ring 3 code
		pop     edi esi edx ecx ebx
		ret

procend		_InitPaging

;----------------------------------------------------------------------------
; void ClosePaging(void)
;----------------------------------------------------------------------------
; Closes the paging system
;----------------------------------------------------------------------------
procstart	_ClosePaging

		push    eax esi edi ecx edx

		call	EnableRing0				; Enable ring 0 mode

		mov     eax,[accessPageAddr]
		call    AccessPage              ; Restore AccessPage mapping
		mov     edi,[PDBR]
		mov     esi,offset pageDirectory
		mov     ecx,1024
		cld
		rep     movsd                   ; Restore the original page table directory

		call	DisableRing0			; Return to ring 3 code

		pop     edx ecx edi esi eax
		ret

procend		_ClosePaging

;----------------------------------------------------------------------------
; long AccessPage(long phys)
;----------------------------------------------------------------------------
; Maps a known page to given physical memory
;	Entry:		EAX	- Physical memory
;	Exit:		EAX	- Linear memory address of mapped phys mem
;----------------------------------------------------------------------------
PROC    AccessPage

		push    edx
		mov     edx,[accessPageTable]
        or      eax,7
        mov     [edx],eax
		mov     eax,cr3
        mov     cr3,eax                 ; Update page table cache
        mov     eax,[accessPageAddr]
        pop     edx
		ret
ENDP

;----------------------------------------------------------------------------
; long GetPhysicalAddress(long linear)
;----------------------------------------------------------------------------
; Returns the physical address of linear address
;	Entry:		EAX	- Linear address to convert
;	Exit:		EAX	- Physical address
;----------------------------------------------------------------------------
PROC    GetPhysicalAddress

		push    edx ebx
		mov     edx,eax
		shr     edx,22                  ; EDX is the directory offset
		mov     ebx,[PDBR]
		mov     edx,[edx*4+ebx]         ; Load page table address
		push    eax
		mov     eax,edx
		call    AccessPage              ; Access the page table
		mov     edx,eax
		pop     eax
		shr     eax,12
		and     eax,03FFh               ; EAX offset into page table
		mov     eax,[edx+eax*4]         ; Load physical address
		and     eax,0FFFFF000h
		pop     ebx edx
		ret
ENDP

;----------------------------------------------------------------------------
; void CreatePageTable(long pageDEntry)
;----------------------------------------------------------------------------
; Creates a page table for specific address (4MB)
;		Entry:	EAX	- Page directory entry (top 10-bits of address)
;----------------------------------------------------------------------------
PROC    CreatePageTable

		push    ebx edx edi ecx
		mov     ebx,eax                 ; Save address
        mov     eax,8192
		push	eax
		call    _VF_malloc              ; Allocate page table directory
		add		esp,4
		add     eax,0FFFh
        and     eax,0FFFFF000h          ; Page align (4KB)
        mov     edi,eax                 ; Save page table linear address
		sub     eax,eax                 ; Fill with zero
        mov     ecx,1024
        cld
        rep     stosd                   ; Clear page table
        sub     edi,4096
        mov     eax,edi
        call    GetPhysicalAddress
        mov     edx,[PDBR]
        or      eax,7                   ; Present/write/user bit
        mov     [edx+ebx*4],eax         ; Save physical address into page directory
        mov     eax,cr3
        mov     cr3,eax                 ; Update page table cache
        pop     ecx edi edx ebx
        ret
ENDP

;----------------------------------------------------------------------------
; void MapPhysical2Linear(ulong pAddr, ulong lAddr, int pages, int flags);
;----------------------------------------------------------------------------
; Maps physical memory into linear memory
;	Entry:		pAddr	- Physical address
;				lAddr	- Linear address
;				pages	- Number of 4K pages to map
;				flags	- Page flags
;							bit 0   =       present
;							bit 1   =       Read(0)/Write(1)
;----------------------------------------------------------------------------
procstart	_MapPhysical2Linear

		ARG		pAddr:ULONG, lAddr:ULONG, pages:UINT, pflags:UINT

		enter_c	0

		call	EnableRing0				; Enable ring 0 mode

		and     [pAddr],0FFFFF000h		; Page boundary
		and     [lAddr],0FFFFF000h      ; Page boundary
		mov		ecx,[pFlags]
		and     ecx,11b                 ; Just two bits
        or      ecx,100b                ; Supervisor bit
        mov     [pflags],ecx

		mov		edx,[lAddr]
		shr     edx,22                  ; EDX = Directory
        mov     esi,[PDBR]
		mov     edi,[pages]             ; EDI page count
		mov     ebx,[lAddr]

@@CreateLoop:
		mov     ecx,[esi+edx*4]         ; Load page table address
		test    ecx,1                   ; Is it present?
		jnz     @@TableOK
		mov     eax,edx
		call    CreatePageTable         ; Create a page table
@@TableOK:
		mov     eax,ebx
		shr     eax,12
		and     eax,3FFh
		sub     eax,1024
		neg     eax                     ; EAX = page count in this table
		inc     edx                     ; Next table
		mov     ebx,0                   ; Next time we'll map 1K pages
		sub     edi,eax                 ; Subtract mapped pages from page count
		jns     @@CreateLoop            ; Create more tables if necessary

		mov     ecx,[pages]             ; ECX = Page count
		mov     esi,[lAddr]
		shr     esi,12                  ; Offset part isn't needed
		mov     edi,[pAddr]
@@MappingLoop:
        mov     eax,esi
        shr     eax,10                  ; EAX = offset to page directory
        mov     ebx,[PDBR]
        mov     eax,[eax*4+ebx]         ; EAX = page table address
		call    AccessPage
		mov     ebx,esi
		and     ebx,3FFh                ; EBX = offset to page table
		mov     edx,edi
		add     edi,4096                ; Next physical address
		inc     esi                     ; Next linear page
		or      edx,[pflags]            ; Update flags...
		mov     [eax+ebx*4],edx         ; Store page table entry
		loop    @@mappingLoop
		mov     eax,cr3
		mov     cr3,eax                 ; Update page table cache

		call	DisableRing0			; Return to ring 3 code

		leave_c_nolocal
		ret

procend		_MapPhysical2Linear

endcodeseg  _vflat

endif

ifdef	X32VM

;----------------------------------------------------------------------------
; FlashTek X32 flat linear framebuffer emulation. Based on original code
; graciously contributed by Kevin Aguilar. Thanks Kevin!
;----------------------------------------------------------------------------

begdataseg  _vflat

Installed		dd  0				; Is our handler installed?
ValidCS			dw	0				; Valid CS for page faults
Ring0CS			dw	0				; Our ring 0 code selector
LastPage		dd	0h				; Page table addr of last mapped in page
BankFuncBuf		db	101 dup (?)		; Place to store bank switch code
BankFuncPtr		dd	offset BankFuncBuf

label	INT14Gate	FWORD
INT14Offset     dd      0  			; eip of original vector
INT14Selector   dw      0			; cs of original vector

		$EXTRN  _PM_savedDS,USHORT
		$EXTRN	__x386_zero_base_selector,WORD
		$EXTRN	__x386_zero_base_ptr,DWORD

ZeroBase		EQU     [__x386_zero_base_ptr]

enddataseg  _vflat

begcodeseg  _vflat              ; Start of code segment

;----------------------------------------------------------------------------
; PF_handler64k - Page fault handler for 64k banks
;----------------------------------------------------------------------------
; The handler below is a 32 bit ring 0 page fault handler.  It receives
; control immediately after any page fault or after an IRQ6 (hardware
; interrupt). This provides the fastest possible handling of page faults
; since it jump directly here.  If this is a page fault, the number
; immediately on the stack will be an error code, at offset 4 will be
; the eip of the faulting instruction, at offset 8 will be the cs of the 
; faulting instruction.  If it is a hardware interrupt, it will not have 
; the error code and the eflags will be at offset 8.
;----------------------------------------------------------------------------
PROC	PF_handler64k	far

; Check if this is a processor exeception or a page fault

		movzx	esp,sp
		push	eax
		mov		ax,[cs:ValidCS]		; Use CS override to access data
		cmp		[ss:esp+12],ax		; Is this a page fault?
		jne		@@ToOldHandler		; Nope, jump to the previous handler

; Get address of page fault and check if within our handlers range

		mov     eax,cr2             ; EBX has page fault linear address
		cmp     eax,VFLAT_START		; Is the fault less than ours?
		jb      @@ToOldHandler		; Yep, go to previous handler
		cmp     eax,VFLAT_END		; Is the fault more than ours?
		jae     @@ToOldHandler		; Yep, go to previous handler

; This is our page fault, so we need to handle it

		pushad
		push    es ds
		mov     ebx,eax				; Save for bank switch code below
		and		ebx,not 0FFFFh		; Mask to 64k bank boundary
		mov		ds,[cs:_PM_savedDS]	; Load segment registers
		mov     es,[cs:__x386_zero_base_selector]

; Mark all pages valid for the new page fault area

		shr     eax,10				; Divide by 1024
		and    	al,0c0h      		; Group page tables in 64k blocks
		add     eax,400000h    		; Point to page table block
		mov     edi,eax             ; Use as a pointer
off = 0
REPT	16
		or    	[DWORD esi+off],0000000001h	; Enable pages
off = off+4
ENDM

; Mark all pages invalid for the previously mapped area

		xchg	edi,[LastPage]		; Save last page for next page fault
		test	edi,edi
		jz		@@DoneMapping		; Dont update if first time round
off = 0
REPT	16
		and		[DWORD esi+off],0FFFFFFFEh	; Disable pages
off = off+4
ENDM

@@DoneMapping:
		mov     eax,cr3
		mov     cr3,eax           	; Flush the TLB

; Now program the new SuperVGA starting bank address

		mov		eax,ebx				; EAX := page fault address
		shr     eax,16
		and		eax,0FFh			; Mask to 0-255
		call    [BankFuncPtr]		; Call the bank switch function

		pop     ds es
		popad
		pop		eax
		add     esp,4             	; Pop the error code from stack
		iretd						; Return to faulting instruction

@@ToOldHandler:
		pop		eax
		jmp		[cs:INT14Gate]		; Chain to previous handler

ENDP	PF_handler64k

;----------------------------------------------------------------------------
; PF_handler4k  - Page fault handler for 4k banks
;----------------------------------------------------------------------------
PROC	PF_handler4k	far

; TODO: Copy 64Kb bank code when this is completed

		jmp		[cs:INT14Gate]		; Chain to previous handler

ENDP	PF_handler4k

;----------------------------------------------------------------------------
; bool VF_available(void)
;----------------------------------------------------------------------------
; Returns true if the virtual linear framebuffer code is available.
;----------------------------------------------------------------------------
procstart	_VF_available

		xor		eax,eax				; Assume not available
		mov		cx,cs
		cmp		cx,2Bh				; If CS == 2Bh then we are under DOS+X32
		jne		@@1
		mov		eax,1				; Yep, paging will be available

; This is not working at the moment so we fail it for X32

		xor		eax,eax

@@1:	ret

procend		_VF_available

;----------------------------------------------------------------------------
; void * VF_init(ulong baseAddr,int bankSize,int codeLen,void *bankFunc)
;----------------------------------------------------------------------------
; Parameters:	baseAddr	- Base address of framebuffer bank window
;				bankSize	- Physical size of banks in Kb (4 or 64)
;               codeLen		- Length of 32 bit bank switch function
; 				bankFunc	- Pointer to protected mode bank function
; Returns:		Near pointer to virtual framebuffer, or NULL on failure.
;
; Description:	Installs the virtual linear framebuffer handling.
;----------------------------------------------------------------------------
procstart	_VF_init

		ARG     baseAddr:ULONG, bankSize:UINT, codeLen:UINT, bankFunc:DPTR

		enter_c 0
		push	es
		cmp		[Installed],0
		jne		@@Exit
		cmp		[codeLen],100
		jg		@@Fail				; Code length must be less than 100 bytes

		mov		ax,cs
		mov		[ValidCS],ax		; Save our valid CS for later
		mov		[LastPage],0		; No pages have been mapped
		mov     es,[__x386_zero_base_selector]

; Map a 4Mb framebuffer memory region

		mov     edi,VFLAT_START		; This is the starting point
		shr     edi,10              ; Divide by 1024
		add     edi,400000h         ; Point to page table block
		mov     eax,[baseAddr]		; EAX := base address of framebuffer
		mov     ecx,4096			; Map a 4Mb virtual buffer

@@NextPage:
		mov     [es:edi],eax		; Map page in and invalidate
		add     edi,4
		dec     ecx
		jnz		@@NextPage

; We will now create a new data selector, and convert it into a ring 0 code
; selector with the same base address as the current CS

		movzx	ebx,[Ring0CS]		; Get previous ring 0 selector
		test	ebx,ebx				; Have we already allocated it?
		jnz		@@InstallInIDT

		mov     ax,3501h
		int     21h          		; Allocate a protected mode selector
		jc		@@Fail
		mov		[Ring0CS],bx		; Save ring 0 selector for next time
		sub     esp,6          		; Allocate space on the stack
		sgdt 	[FWORD ss:esp]		; Get address of GDT
		pop     ax               	; Add esp,2
		pop     eax                	; Absolute address of GDT
		add     eax,ZeroBase
		movzx   ebx,bx            	; Zero high word of ebx
		and     bl,0f8h				; Make index into GDT, set RPL to 0
		xor     ecx,ecx
		mov     ecx,cs
		and     cl,0f8h				; Make index into GDT for current CS
		add     ecx,eax          	; Make ptr to GDT for current CS
		add     eax,ebx             ; Make ptr to GDT entry for allocated selector
		mov     edx,[ecx]           ; Get first 4 bytes of current cs descriptor
		mov     [eax],edx           ; Initialize first 4 bytes of ring 0 CS
		mov     edx,[ecx+4]         ; Get second dword of current cs descriptor
		and     dh,10011111b        ; Set DPL bits to zero, make it a ring 0 selector
		mov     [eax+4],edx         ; Initialize second dword of new cs descriptor

; New descriptor contained in bx now has identical characteristics to the
; original CS except it is ring 0, it has same base address, same limit and
; it is a 32 bit executable selector.  We now need to place this new CS in
; the Interrupt Descriptor Table or IDT.

@@InstallInIDT:
		sub		esp,6               ; Allocate space on stack
		sidt	[FWORD ss:esp]		; Store pointer to IDT
		pop     ax               	; add esp,2
		pop     eax                 ; Absolute address of IDT
		add     eax,ZeroBase    	; Make near pointer to IDT
		add     eax,14*8          	; Point to Int #14

; Note that Interrupt gates do not have the high and low word of the
; offset in adjacent words in memory, there are 4 bytes separating them.

		mov     ecx,[eax]   		; Get cs and low 16 bits of offset
		mov     edx,[eax+6]         ; Get high 16 bits of offset in dx
        shl     edx,16
		mov     dx,cx             	; edx has offset
		mov     [INT14Offset],edx	; Save offset
		shr     ecx,16
		mov     [INT14Selector],cx	; Save original cs
		mov     [eax+2],bx          ; Install new cs
		mov     edx,offset PF_handler64k
		cmp		[bankSize],4
		jne		@@1
		mov		edx,offset PF_handler4k
@@1:	mov     [eax],dx          	; Install low word of offset
		shr     edx,16
		mov     [eax+6],dx          ; Install high word of offset

; Copy the bank switch function. We copy it into a buffer in our data
; segment, so that the interface of our function is identical to the
; interface required by the DVA.386 and VFLATD.386 virtual framebuffer
; devices under Windows.

		mov		esi,[bankFunc]		; Copy the code into buffer
		mov		edi,offset bankFuncBuf
		mov		ecx,[codeLen]
	rep movsb
		mov		[BYTE edi],0C3h		; Terminate the function with a near ret

@@Done:	mov		[Installed],1		; Our handler is now installed
		mov		eax,VFLAT_START		; Return the virtual buffer address

@@Exit:	pop		es
		leave_c_nolocal
		ret

@@Fail:	xor		eax,eax				; Install failed!
		jmp		@@Exit

procend		_VF_init

;----------------------------------------------------------------------------
; void VF_exit(void)
;----------------------------------------------------------------------------
; Closes down the virtual framebuffer services and restores the previous
; page fault handler.
;----------------------------------------------------------------------------
procstart	_VF_exit

		enter_c	0
		cmp		[Installed],0
		je		@@Exit

; Remove page fault handler from IDT

		sub		esp,6               ; Allocate space on stack
		sidt	[FWORD ss:esp]		; Store pointer to IDT
		pop     ax               	; add esp,2
		pop     eax                 ; Absolute address of IDT
		add     eax,ZeroBase    	; Make near pointer to IDT
		add     eax,14*8          	; Point to Int #14
		mov     cx,[INT14Selector]
		mov     [eax+2],cx          ; Restore original CS
		mov     edx,[INT14Offset]
		mov     [eax],dx          	; Install low word of offset
		shr     edx,16
		mov     [eax+6],dx          ; Install high word of offset

		mov		[Installed],0

@@Exit:	leave_c_nolocal
		ret

procend		_VF_exit

endcodeseg  _vflat

endif
		END                     ; End of module
