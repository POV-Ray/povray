/****************************************************************************
*
*						      PM/Lite Library
*
*					Copyright (C) 1996 SciTech Software.
*							All rights reserved.
*
* Filename:		$Workfile:   pmlite.c  $
* Version:		$Revision:   1.1  $
*
* Language:		ANSI C
* Environment:	IBM PC (MSDOS) Real mode and 16/32 bit Protected Mode
*
* Description:	Module implementing DOS extender independant protected
*				mode programming. This module will need to be included in
*				all programs that use SciTech Software's products that
*				are to be compiled in protected mode, and will need to be
*				compiled with the correct defines for the DOS extender that
*				you will be using (or with no defines for real mode
*				emulation of these routines).
*
*				The Lite version of this library is freely redistributable
*				in source code form.
*
* $Date:   12 Feb 1996 21:58:56  $ $Author:   KendallB  $
*
****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "pmode.h"

#pragma pack(1)

/*--------------------------- Global variables ----------------------------*/

/* Set the type of target being compiled for so that we can do switching at
 * runtime as well as compile time.
 */

#if	defined(REALMODE)
PRIVATE int	_PM_modeType = PM_realMode;
#elif defined(PM286)
PRIVATE int _PM_modeType = PM_286;
#elif defined(PM386)
PRIVATE int _PM_modeType = PM_386;
#endif

/*----------------------------- Implementation ----------------------------*/

/*-------------------------------------------------------------------------*/
/* Generic routines common to all extenders								   */
/*-------------------------------------------------------------------------*/

int PMAPI PM_getModeType(void)
{ return _PM_modeType; }

uint PMAPI PM_getBIOSSelector(void)
{
	static uint BIOS_sel = 0;
	if (!BIOS_sel)
		BIOS_sel = PM_createSelector(0x400,0xFFFF);
	return BIOS_sel;
}

uint PMAPI PM_getVGASelector(void)
{
	switch (PM_getByte(PM_getBIOSSelector(),0x49)) {
		case 0x3:
			return PM_getVGAColorTextSelector();
		case 0x7:
			return PM_getVGAMonoTextSelector();
		default:
			return PM_getVGAGraphSelector();
		}
}

uint PMAPI PM_getVGAColorTextSelector(void)
{
	static long	VGA_sel = 0;
	if (!VGA_sel)
		VGA_sel = PM_createSelector(0xB8000L,0xFFFF);
	return (uint)VGA_sel;
}

uint PMAPI PM_getVGAMonoTextSelector(void)
{
	static long	VGA_sel = 0;
	if (!VGA_sel)
		VGA_sel = PM_createSelector(0xB0000L,0xFFFF);
	return (uint)VGA_sel;
}

uint PMAPI PM_getVGAGraphSelector(void)
{
	static long	VGA_sel = 0;
	if (!VGA_sel)
		VGA_sel = PM_createSelector(0xA0000L,0xFFFF);
	return (uint)VGA_sel;
}

int PMAPI PM_int386(int intno, PMREGS *in, PMREGS *out)
{
	PMSREGS	sregs;
	PM_segread(&sregs);
	return PM_int386x(intno,in,out,&sregs);
}

/* Routines to set and get the real mode interrupt vectors, by making
 * direct real mode calls to DOS and bypassing the DOS extenders API.
 * This is the safest way to handle this, as some servers try to be
 * smart about changing real mode vectors.
 */

void PMAPI _PM_getRMvect(int intno, long *realisr)
{
	RMREGS	regs;
	RMSREGS	sregs;

	PM_saveDS();
	regs.h.ah = 0x35;
	regs.h.al = intno;
	PM_int86x(0x21, &regs, &regs, &sregs);
	*realisr = ((long)sregs.es << 16) | regs.x.bx;
}

void PMAPI _PM_setRMvect(int intno, long realisr)
{
	RMREGS	regs;
	RMSREGS	sregs;

	PM_saveDS();
	regs.h.ah = 0x25;
	regs.h.al = intno;
	sregs.ds = (int)(realisr >> 16);
	regs.x.dx = (int)(realisr & 0xFFFF);
	PM_int86x(0x21, &regs, &regs, &sregs);
}

/*-------------------------------------------------------------------------*/
/* Generic DPMI routines common to 16/32 bit code						   */
/*-------------------------------------------------------------------------*/

#ifndef	REALMODE

ulong DPMI_mapPhysicalToLinear(ulong physAddr,ulong limit)
{
	PMREGS  r;
	ulong	physOfs;

	/* Round the physical address to a 4Kb boundary and the limit to a
	 * 4Kb-1 boundary before passing the values to DPMI as some extenders
	 * will fail the calls unless this is the case. If we round the
	 * physical address, then we also add an extra offset into the address
	 * that we return.
	 */
	physOfs = physAddr & 4095;
	physAddr = physAddr & ~4095;
	limit = ((limit+physOfs+1+4095) & ~4095)-1;

	r.x.ax = 0x800;                 /* DPMI map physical to linear      */
	r.x.bx = physAddr >> 16;
	r.x.cx = physAddr & 0xFFFF;
	r.x.si = limit >> 16;
	r.x.di = limit & 0xFFFF;
	PM_int386(0x31, &r, &r);
	if (r.x.cflag)
		return 0;
	return ((ulong)r.x.bx << 16) + r.x.cx + physOfs;
}

int DPMI_setSelectorBase(ushort sel,ulong linAddr)
{
	PMREGS  r;

	r.x.ax = 7;                     /* DPMI set selector base address   */
	r.x.bx = sel;
	r.x.cx = linAddr >> 16;
	r.x.dx = linAddr & 0xFFFF;
	PM_int386(0x31, &r, &r);
	if (r.x.cflag)
		return 0;
	return 1;
}

ulong DPMI_getSelectorBase(ushort sel)
{
	PMREGS  r;

	r.x.ax = 6;                     /* DPMI get selector base address   */
	r.x.bx = sel;
	PM_int386(0x31, &r, &r);
	return ((ulong)r.x.cx << 16) + r.x.dx;
}

int DPMI_setSelectorLimit(ushort sel,ulong limit)
{
	PMREGS  r;

	r.x.ax = 8;                     /* DPMI set selector limit          */
	r.x.bx = sel;
	r.x.cx = limit >> 16;
	r.x.dx = limit & 0xFFFF;
	PM_int386(0x31, &r, &r);
	if (r.x.cflag)
		return 0;
	return 1;
}

uint DPMI_createSelector(ulong base,ulong limit)
{
	uint	sel;
	PMREGS	r;

	/* Allocate 1 descriptor */
	r.x.ax = 0;
	r.x.cx = 1;
	PM_int386(0x31, &r, &r);
	if (r.x.cflag) return 0;
	sel = r.x.ax;

	if (base > 0x100000L) {
        /* Set the descriptor access rights (for a 32 bit page granular
         * segment).
         */
        r.x.ax = 9;
        r.x.bx = sel;
        r.x.cx = 0x8092;
        PM_int386(0x31, &r, &r);

		/* Map physical memory above 1Mb */
		if ((base = DPMI_mapPhysicalToLinear(base,limit)) == 0)
			return 0;
		}

	if (!DPMI_setSelectorBase(sel,base))
		return 0;
	if (!DPMI_setSelectorLimit(sel,limit))
		return 0;
	return sel;
}

void DPMI_freeSelector(uint sel)
{
	PMREGS	r;

	r.x.ax = 1;
	r.x.bx = sel;
	PM_int386(0x31, &r, &r);
}

int DPMI_lockLinearPages(ulong linear,ulong len)
{
	PMREGS	r;

	r.x.ax = 0x600;						/* DPMI Lock Linear Region 		*/
	r.x.bx = (linear >> 16);         	/* Linear address in BX:CX 		*/
	r.x.cx = (linear & 0xFFFF);
	r.x.si = (len >> 16);         		/* Length in SI:DI 				*/
	r.x.di = (len & 0xFFFF);
	PM_int386(0x31, &r, &r);
	return (!r.x.cflag);
}

int DPMI_unlockLinearPages(ulong linear,ulong len)
{
	PMREGS	r;

	r.x.ax = 0x601;						/* DPMI Unlock Linear Region 	*/
	r.x.bx = (linear >> 16);         	/* Linear address in BX:CX 		*/
	r.x.cx = (linear & 0xFFFF);
	r.x.si = (len >> 16);         		/* Length in SI:DI 				*/
	r.x.di = (len & 0xFFFF);
	PM_int386(0x31, &r, &r);
	return (!r.x.cflag);
}

#if	defined(PM386) && !defined(__WINDOWS32__)

/* Some DOS extender implementations do not directly support calling a
 * real mode procedure from protected mode. However we can simulate what
 * we need temporarily hooking the INT 2Fh vector with a small real mode
 * stub that will call our real mode code for us.
 */

static uchar int2FHandler[] = {
	0x00,0x00,0x00,0x00,		/* 	__PMODE_callReal variable			*/
	0xFB,						/*	sti									*/
	0x2E,0xFF,0x1E,0x00,0x00,	/*  call	[cs:__PMODE_callReal]		*/
	0xCF,						/*	iretf								*/
	};
static uint crSel=0,crOff;	/* Selector:offset of int 2F handler	*/
static uint crRSeg,crROff;	/* Real mode seg:offset of handler		*/

void PMAPI PM_callRealMode(uint seg,uint off, RMREGS *in,
	RMSREGS *sregs)
{
	uint		psel,poff;
	uint		oldSeg,oldOff;

	if (!crSel) {
		/* Allocate and copy the memory block only once */
		PM_allocRealSeg(sizeof(int2FHandler), &crSel, &crOff,
			&crRSeg, &crROff);
		PM_memcpyfn(crSel,crOff,int2FHandler,sizeof(int2FHandler));
		}
	PM_setWord(crSel,crOff,off);		/* Plug in address to call	*/
	PM_setWord(crSel,crOff+2,seg);
	PM_mapRealPointer(&psel,&poff,0,0x2F * 4);
	oldOff = PM_getWord(psel,poff);		/* Save old handler address	*/
	oldSeg = PM_getWord(psel,poff+2);
	PM_setWord(psel,poff,crROff+4);		/* Hook 2F handler			*/
	PM_setWord(psel,poff+2,crRSeg);
	PM_int86x(0x2F, in, in, sregs);		/* Call real mode code		*/
	PM_setWord(psel,poff,oldOff);		/* Restore old handler		*/
	PM_setWord(psel,poff+2,oldSeg);
}

#endif
#endif

/*-------------------------------------------------------------------------*/
/* DOS Real Mode support.												   */
/*-------------------------------------------------------------------------*/

#ifdef REALMODE

#ifndef	MK_FP
#define MK_FP(s,o)  ( (void far *)( ((ulong)(s) << 16) + \
					(ulong)(o) ))
#endif

uchar PMAPI PM_getByte(uint s, uint o)
{ return *((uchar*)MK_FP(s,o)); }

ushort PMAPI PM_getWord(uint s, uint o)
{ return *((uint*)MK_FP(s,o)); }

ulong PMAPI PM_getLong(uint s, uint o)
{ return *((ulong*)MK_FP(s,o)); }

void PMAPI PM_setByte(uint s, uint o, uchar v)
{ *((uchar*)MK_FP(s,o)) = v; }

void PMAPI PM_setWord(uint s, uint o, ushort v)
{ *((ushort *)MK_FP(s,o)) = v; }

void PMAPI PM_setLong(uint s, uint o, ulong v)
{ *((ulong*)MK_FP(s,o)) = v; }

void PMAPI PM_memcpynf(void *dst,uint src_s,uint src_o,uint n)
{ memcpy(dst,MK_FP(src_s,src_o),n); }

void PMAPI PM_memcpyfn(uint dst_s,uint dst_o,void *src,uint n)
{ memcpy(MK_FP(dst_s,dst_o),src,n); }

void PMAPI PM_mapRealPointer(uint *sel,uint *off,uint r_seg,
	uint r_off)
{ *sel = r_seg; *off = r_off; }

uint PMAPI PM_createSelector(ulong base,ulong limit)
{
	limit = limit;
	return (uint)(base >> 4);
}

void * PMAPI PM_mapPhysicalAddr(ulong base,ulong limit)
{
	uint sel = base >> 4;
	uint off = base & 0xF;
	limit = limit;
	return MK_FP(sel,off);
}

void PMAPI PM_freeSelector(uint sel)
{ sel = sel; }

int PMAPI PM_allocRealSeg(uint size, uint *sel,uint *off,
	uint *r_seg, uint *r_off)
{
	/* Call malloc() to allocate the memory for us */
	void *p = malloc(size);
	*sel = *r_seg = FP_SEG(p);
	*off = *r_off = FP_OFF(p);
	return 1;
}

void PMAPI PM_freeRealSeg(uint sel,uint off)
{
	free(MK_FP(sel,off));
}

int PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out)
{
	return int86(intno,in,out);
}

int PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out,
	RMSREGS *sregs)
{
	return int86x(intno,in,out,sregs);
}

void PMAPI PM_availableMemory(ulong *physical,ulong *total)
{
	union REGS regs;

	regs.h.ah = 0x48;
	regs.x.bx = 0xFFFF;
	int86(0x21,&regs,&regs);
	*physical = *total = regs.x.bx * 16UL;
}

#endif

/*-------------------------------------------------------------------------*/
/* Windows 3.1 16 bit DPMI and Borland DPMI16 DOS Power Pack support.	   */
/*-------------------------------------------------------------------------*/

#if defined(__WINDOWS16__) || defined(DPMI16)

uchar PMAPI PM_getByte(uint s, uint o)
{ return *((uchar*)MK_FP(s,o)); }

ushort PMAPI PM_getWord(uint s, uint o)
{ return *((uint*)MK_FP(s,o)); }

ulong PMAPI PM_getLong(uint s, uint o)
{ return *((ulong*)MK_FP(s,o)); }

void PMAPI PM_setByte(uint s, uint o, uchar v)
{ *((uchar*)MK_FP(s,o)) = v; }

void PMAPI PM_setWord(uint s, uint o, ushort v)
{ *((ushort *)MK_FP(s,o)) = v; }

void PMAPI PM_setLong(uint s, uint o, ulong v)
{ *((ulong*)MK_FP(s,o)) = v; }

void PMAPI PM_memcpynf(void *dst,uint src_s,uint src_o,uint n)
{ memcpy(dst,MK_FP(src_s,src_o),n); }

void PMAPI PM_memcpyfn(uint dst_s,uint dst_o,void *src,uint n)
{ memcpy(MK_FP(dst_s,dst_o),src,n); }

uint PMAPI PM_createSelector(ulong base,ulong limit)
{
	return DPMI_createSelector(base,limit);
}

void * PMAPI PM_mapPhysicalAddr(ulong base,ulong limit)
{
	uint sel = PM_createSelector(base,limit);
	return MK_FP(sel,0);
}

void PMAPI PM_freeSelector(uint sel)
{
	DPMI_freeSelector(sel);
}

void PMAPI PM_mapRealPointer(uint *sel,uint *off,uint r_seg,
	uint r_off)
{
	static	uint	staticSel = 0;

	if (staticSel)
		PM_freeSelector(staticSel);
	staticSel = PM_createSelector(MK_PHYS(r_seg,r_off), 0xFFFF);
	*sel = staticSel;
	*off = 0;
}

uint PMAPI PM_createCode32Alias(uint sel)
{
	uint	alias;
	union REGS	r;

	r.x.ax = 0xA;
	r.x.bx = sel;
	int86(0x31,&r,&r);			/* Create Alias descriptor				*/
	if (r.x.cflag) return 0;
	alias = r.x.ax;

	/* Set descriptor access rights (for a 32 bit code segment)			*/
	r.x.ax = 9;
	r.x.bx = alias;
	r.x.cx = 0x40FB;
	int86(0x31, &r, &r);
	if (r.x.cflag) return 0;
    return alias;
}

#pragma pack(1)

typedef struct {
	long	edi;
	long	esi;
	long	ebp;
	long	reserved;
	long	ebx;
	long	edx;
	long	ecx;
	long	eax;
	short	flags;
	short	es,ds,fs,gs,ip,cs,sp,ss;
	} _RMREGS;

#pragma pack()

#define IN(reg)     rmregs.e##reg = in->x.reg
#define OUT(reg)    out->x.reg = (int)rmregs.e##reg
#define OUT1(reg)   in->x.reg = (int)rmregs.e##reg

void PMAPI PM_callRealMode(uint seg,uint off, RMREGS *in,
	RMSREGS *sregs)
{
	_RMREGS			rmregs;
	union REGS		r;
	struct SREGS	sr;
	void			*p;

	memset(&rmregs, 0, sizeof(rmregs));
	IN(ax); IN(bx); IN(cx); IN(dx); IN(si); IN(di);
	rmregs.es = sregs->es;
	rmregs.ds = sregs->ds;
	rmregs.cs = seg;
	rmregs.ip = off;

	memset(&sr, 0, sizeof(sr));
	r.x.ax = 0x301;					/* DPMI call real mode			*/
	r.h.bh = 0;
	r.x.cx = 0;
	p = &rmregs;
	sr.es = FP_SEG(p);
	r.x.di = FP_OFF(p);
	int86x(0x31, &r, &r, &sr);		/* Issue the interrupt			*/

	OUT1(ax); OUT1(bx); OUT1(cx); OUT1(dx); OUT1(si); OUT1(di);
	sregs->es = rmregs.es;
	sregs->ds = rmregs.ds;
}

int PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out)
{
	_RMREGS			rmregs;
	union REGS		r;
	struct SREGS	sr;
	void			*p;

	memset(&rmregs, 0, sizeof(rmregs));
	IN(ax); IN(bx); IN(cx); IN(dx); IN(si); IN(di);

	memset(&sr, 0, sizeof(sr));
	r.x.ax = 0x300;					/* DPMI issue real interrupt	*/
	r.h.bl = intno;
	r.h.bh = 0;
	r.x.cx = 0;
	p = &rmregs;
	sr.es = FP_SEG(p);
	r.x.di = FP_OFF(p);
	int86x(0x31, &r, &r, &sr);		/* Issue the interrupt			*/

	OUT(ax); OUT(bx); OUT(cx); OUT(dx); OUT(si); OUT(di);
	out->x.cflag = rmregs.flags & 0x1;
	return out->x.ax;
}

int PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out,
	RMSREGS *sregs)
{
	_RMREGS			rmregs;
	union REGS		r;
	struct SREGS	sr;
	void			*p;

	memset(&rmregs, 0, sizeof(rmregs));
	IN(ax); IN(bx); IN(cx); IN(dx); IN(si); IN(di);
	rmregs.es = sregs->es;
	rmregs.ds = sregs->ds;

	memset(&sr, 0, sizeof(sr));
	r.x.ax = 0x300;					/* DPMI issue real interrupt	*/
	r.h.bl = intno;
	r.h.bh = 0;
	r.x.cx = 0;
	p = &rmregs;
	sr.es = FP_SEG(p);
	r.x.di = FP_OFF(p);
	int86x(0x31, &r, &r, &sr);		/* Issue the interrupt */

	OUT(ax); OUT(bx); OUT(cx); OUT(dx); OUT(si); OUT(di);
	sregs->es = rmregs.es;
	sregs->cs = rmregs.cs;
	sregs->ss = rmregs.ss;
	sregs->ds = rmregs.ds;
	out->x.cflag = rmregs.flags & 0x1;
	return out->x.ax;
}

void PMAPI PM_availableMemory(ulong *physical,ulong *total)
{
	union REGS 		r;
	struct SREGS	sr;
	struct MemInfo {
		ulong	LargestBlockAvail;
		ulong	MaxUnlockedPage;
		ulong	LargestLockablePage;
		ulong   LinAddrSpace;
		ulong   NumFreePagesAvail;
		ulong   NumPhysicalPagesFree;
		ulong   TotalPhysicalPages;
		ulong	FreeLinAddrSpace;
		ulong	SizeOfPageFile;
		ulong	res[3];
		} memInfo;
	void *p;

	memset(&sr, 0, sizeof(sr));
	r.x.ax = 0x500;					/* DPMI get free memory info */
	p = &memInfo;
	sr.es = FP_SEG(p);
	r.x.di = FP_OFF(p);
	int86x(0x31, &r, &r, &sr);		/* Issue the interrupt */
	*physical = memInfo.NumPhysicalPagesFree * 4096;
	*total = memInfo.LargestBlockAvail;
	if (*total < *physical)
		*physical = *total;
}

#endif

/*-------------------------------------------------------------------------*/
/* Borland DPMI16 DOS Power Pack support.	   							   */
/*-------------------------------------------------------------------------*/

#if defined(DPMI16)

int PMAPI PM_allocRealSeg(uint size, uint *sel,uint *off,
	uint *r_seg, uint *r_off)
{
	union REGS		r;

	r.x.ax = 0x100;					/* DPMI allocate DOS memory		*/
	r.x.bx = (size + 0xF) >> 4;		/* number of paragraphs 		*/
	int86(0x31, &r, &r);
	if (r.x.cflag) return NULL;		/* DPMI call failed				*/

	*sel = r.x.dx;					/* Protected mode selector		*/
	*off = 0;
	*r_seg = r.x.ax;				/* Real mode segment			*/
	*r_off = 0;
	return 1;
}

void PMAPI PM_freeRealSeg(uint sel,uint off)
{
	union REGS	r;

	r.x.ax = 0x101;					/* DPMI free DOS memory			*/
	r.x.dx = sel;					/* DX := selector from 0x100	*/
	off = off;
	int86(0x31, &r, &r);
}

#endif

/*-------------------------------------------------------------------------*/
/* Phar Lap TNT DOS Extender support.									   */
/*-------------------------------------------------------------------------*/

#ifdef TNT

#include <pldos32.h>
#include <pharlap.h>
#include <hw386.h>

void PMAPI PM_mapRealPointer(uint *sel,uint *off,uint r_seg,
	uint r_off)
{
	CONFIG_INF	config;

	_dx_config_inf(&config, (UCHAR*)&config);
	*sel = config.c_dos_sel;
	*off = MK_PHYS(r_seg,r_off);
}

uint PMAPI _PL_allocsel(void);

uint PMAPI PM_createSelector(ulong base,ulong limit)
{
	USHORT		sel;
	ULONG		off;
	CD_DES		desc;

	if ((sel = _PL_allocsel()) == 0)
		return 0;
	if (base > 0xFFFFFL) {
		/* Map in the physical memory above the 1Mb memory boundary */
		_dx_map_phys(sel,base,(limit + 4095) / 4096,&off);
		}
	else {
		if (_dx_ldt_rd(sel,(UCHAR*)&desc) != 0)
			return 0;
		if (limit >= 0x100000) {
			limit >>= 12;			/* Page granular for > 1Mb limit	*/
			desc.limit0_15 = limit & 0xFFFF;
			desc.limit16_19 = ((limit >> 16) & 0xF) | DOS_32 | SG_PAGE;
			}
		else {						/* Byte granular for < 1Mb limit	*/
			desc.limit0_15 = limit & 0xFFFF;
			desc.limit16_19 = ((limit >> 16) & 0xF) | DOS_32 | SG_BYTE;
			}
		desc.base0_15 = base & 0xFFFF;
		desc.base16_23 = (base >> 16) & 0xFF;
		desc.base24_31 = (base >> 24) & 0xFF;
		desc.arights = AR_DATA | AR_DPL3;
		if (_dx_ldt_wr(sel,(UCHAR*)&desc) != 0)
			return 0;
		}
	return sel;
}

void PMAPI PM_freeSelector(uint sel)
{
	CD_DES		desc;

	/* Set the selector's values to all zero's, so that Phar Lap will
	 * re-use the selector entry.
	 */
	if (_dx_ldt_rd(sel,(UCHAR*)&desc) != 0)
		return;
	desc.limit0_15 = 0;
	desc.limit16_19 = 0;
	desc.base0_15 = 0;
	desc.base16_23 = 0;
	desc.base24_31 = 0;
	desc.arights = 0;
	_dx_ldt_wr(sel,(UCHAR*)&desc);
}

int PMAPI PM_allocRealSeg(uint size, uint *sel,uint *off,
	uint *r_seg, uint *r_off)
{
	CONFIG_INF	config;
	USHORT      addr,t;

	if (_dx_real_alloc((size + 0xF) >> 4,&addr,&t) != 0)
		return 0;
	_dx_config_inf(&config, (UCHAR*)&config);
	*sel = config.c_dos_sel;		/* Map with DOS 1Mb selector	*/
	*off = (ULONG)addr << 4;
	*r_seg = addr;					/* Real mode segment address	*/
	*r_off = 0;						/* Real mode segment offset		*/
	return 1;
}

void PMAPI PM_freeRealSeg(uint sel,uint off)
{
	sel = sel;
	_dx_real_free(off >> 4);
}

#define IN(reg)     rmregs.e##reg = in->x.reg
#define OUT(reg)    out->x.reg = rmregs.e##reg

int PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out)
{
	SWI_REGS	rmregs;

	memset(&rmregs, 0, sizeof(rmregs));
	IN(ax); IN(bx); IN(cx); IN(dx); IN(si); IN(di);

	_dx_real_int(intno,&rmregs);

	OUT(ax); OUT(bx); OUT(cx); OUT(dx); OUT(si); OUT(di);
	out->x.cflag = rmregs.flags & 0x1;
	return out->x.ax;
}

int PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out,
	RMSREGS *sregs)
{
	SWI_REGS	rmregs;

	memset(&rmregs, 0, sizeof(rmregs));
	IN(ax); IN(bx); IN(cx); IN(dx); IN(si); IN(di);
	rmregs.es = sregs->es;
	rmregs.ds = sregs->ds;

    _dx_real_int(intno,&rmregs);

	OUT(ax); OUT(bx); OUT(cx); OUT(dx); OUT(si); OUT(di);
	sregs->es = rmregs.es;
	sregs->cs = rmregs.cs;
	sregs->ss = rmregs.ss;
	sregs->ds = rmregs.ds;
	out->x.cflag = rmregs.flags & 0x1;
	return out->x.ax;
}

void PMAPI PM_availableMemory(ulong *physical,ulong *total)
{
	PMREGS 	r;
	uint	data[25];

	r.x.ax = 0x2520;				/* Get free memory info */
	r.x.bx = 0;
	r.e.edx = (uint)data;
	PM_int386(0x21, &r, &r);
	*physical = data[21] * 4096;
	*total = data[23] * 4096;
}

#endif

/*-------------------------------------------------------------------------*/
/* Symantec C++ DOSX and FlashTek X-32/X-32VM support					   */
/*-------------------------------------------------------------------------*/

#if	defined(DOSX) || defined(X32VM)
#include <dos.h>

#ifdef	X32VM
#include <x32.h>

#define	_x386_mk_protected_ptr(p)	_x32_mk_protected_ptr((void*)p)
#define	_x386_free_protected_ptr(p)	_x32_free_protected_ptr(p)
#define	_x386_zero_base_ptr			_x32_zero_base_ptr
#else
extern void *_x386_zero_base_ptr;
#endif

void PMAPI PM_mapRealPointer(uint *sel,uint *off,uint r_seg,
	uint r_off)
{
	*sel = _x386_zero_base_selector;
	*off = MK_PHYS(r_seg,r_off);
}

uint PMAPI PM_createSelector(ulong base,ulong limit)
{
	void _far *p;

	p = (void _far *)_x386_mk_protected_ptr(base);
	limit = limit;
	return FP_SEG(p);
}

void PMAPI PM_freeSelector(uint sel)
{
	_x386_free_protected_ptr(MK_FP(sel,0));
}

int PMAPI PM_allocRealSeg(uint size, uint *sel,uint *off,
	uint *r_seg, uint *r_off)
{
	PMREGS	r;

	r.h.ah = 0x48;					/* DOS function 48h - allocate mem	*/
	r.x.bx = (size + 0xF) >> 4;		/* Number of paragraphs to allocate	*/
	PM_int386(0x21, &r, &r);		/* Call DOS extender				*/
	if (r.x.cflag)
		return 0;					/* Could not allocate the memory	*/
	*sel = _x386_zero_base_selector;
	*off = r.e.eax << 4;
	*r_seg = r.e.eax;
	*r_off = 0;
	return 1;
}

void PMAPI PM_freeRealSeg(uint sel,uint off)
{
	/* Cannot de-allocate this memory */
	sel = sel; off = off;
}

#pragma pack(1)

typedef struct {
	ushort	intno;
	ushort	ds;
	ushort	es;
	ushort	fs;
	ushort	gs;
	ulong	eax;
	ulong	edx;
	} _RMREGS;

#pragma pack()

#define IN(reg)     regs.e.e##reg = in->x.reg
#define OUT(reg)    out->x.reg = regs.x.reg

int PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out)
{
	_RMREGS rmregs;
	PMREGS  regs;
	PMSREGS	pmsregs;

	rmregs.intno = intno;
	rmregs.eax = in->x.ax;
	rmregs.edx = in->x.dx;
	IN(bx); IN(cx); IN(si); IN(di);
	regs.x.ax = 0x2511;
	regs.e.edx = (uint)(&rmregs);
	PM_segread(&pmsregs);
	PM_int386x(0x21,&regs,&regs,&pmsregs);

	OUT(ax); OUT(bx); OUT(cx); OUT(si); OUT(di);
	out->x.dx = rmregs.edx;
	out->x.cflag = regs.x.cflag;
	return out->x.ax;
}

int PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out, RMSREGS *sregs)
{
	_RMREGS rmregs;
	PMREGS  regs;
	PMSREGS	pmsregs;

	rmregs.intno = intno;
	rmregs.eax = in->x.ax;
	rmregs.edx = in->x.dx;
	rmregs.es = sregs->es;
	rmregs.ds = sregs->ds;
	IN(bx); IN(cx); IN(si); IN(di);
	regs.x.ax = 0x2511;
	regs.e.edx = (uint)(&rmregs);
	PM_segread(&pmsregs);
	PM_int386x(0x21,&regs,&regs,&pmsregs);

	OUT(ax); OUT(bx); OUT(cx); OUT(si); OUT(di);
	sregs->es = rmregs.es;
	sregs->ds = rmregs.ds;
	out->x.dx = rmregs.edx;
	out->x.cflag = regs.x.cflag;
	return out->x.ax;
}

void * PMAPI PM_mapPhysicalAddr(ulong base,ulong limit)
{
	if (base > 0x100000)
		return _x386_map_physical_address((void*)base,limit);
	return (void*)((ulong)_x386_zero_base_ptr + base);
}

ulong _cdecl _X32_getPhysMem(void);

void PMAPI PM_availableMemory(ulong *physical,ulong *total)
{
	PMREGS	regs;

	/* Get total memory available, including virtual memory */
	regs.x.ax = 0x350B;
	PM_int386(0x21,&regs,&regs);
	*total = regs.e.eax;

	/* Get physical memory available */
	*physical = _X32_getPhysMem();
	if (*physical > *total)
		*physical = *total;
}

#endif

/*-------------------------------------------------------------------------*/
/* Borland's DPMI32 DOS Power Pack Extender support.					   */
/*-------------------------------------------------------------------------*/

#ifdef  DPMI32
#define	GENERIC_DPMI32			/* Use generic 32 bit DPMI routines	*/
#endif

/*-------------------------------------------------------------------------*/
/* Watcom C/C++ with Rational DOS/4GW support.							   */
/*-------------------------------------------------------------------------*/

#ifdef	DOS4GW
#define	GENERIC_DPMI32			/* Use generic 32 bit DPMI routines	*/
#endif

/*-------------------------------------------------------------------------*/
/* DJGPP port of GNU C++ support.										   */
/*-------------------------------------------------------------------------*/

#ifdef DJGPP
#define	GENERIC_DPMI32			/* Use generic 32 bit DPMI routines	*/
#endif

/*-------------------------------------------------------------------------*/
/* Generic 32 bit DPMI routines											   */
/*-------------------------------------------------------------------------*/

#if	defined(GENERIC_DPMI32)

void * PMAPI PM_mapPhysicalAddr(ulong base,ulong limit)
{
	PMSREGS	sregs;
	ulong	linAddr;
	ulong	DSBaseAddr;

	/* Get the base address for the default DS selector */
	PM_segread(&sregs);
	DSBaseAddr = DPMI_getSelectorBase(sregs.ds);

	if ((base < 0x100000) && (DSBaseAddr == 0)) {
		/* DS is zero based, so we can directly access the first 1Mb of
		 * system memory (like under DOS4GW).
		 */
		return (void*)base;
		}

	/* Map the memory to a linear address using DPMI function 0x800 */
	if ((linAddr = DPMI_mapPhysicalToLinear(base,limit)) == 0) {
		if (base >= 0x100000)
			return NULL;
		/* If the linear address mapping fails but we are trying to
		 * map an area in the first 1Mb of system memory, then we must
		 * be running under a Windows or OS/2 DOS box. Under these
		 * environments we can use the segment wrap around as a fallback
		 * measure, as this does work properly.
		 */
		linAddr = base;
		}

	/* Now expand the default DS selector to 4Gb so we can access it */
	if (!DPMI_setSelectorLimit(sregs.ds,0xFFFFFFFFUL))
		return NULL;

	/* Now return the base address of the memory into the default DS */
	return (void*)(linAddr - DSBaseAddr);
}

void PMAPI PM_mapRealPointer(uint *sel,uint *off,uint r_seg,
	uint r_off)
{
	static uint lowMem = 0;
	if (!lowMem)
		lowMem = PM_createSelector(0, 0xFFFFF);
	*sel = lowMem;
	*off = MK_PHYS(r_seg,r_off);
}

int PMAPI PM_allocRealSeg(uint size, uint *sel,uint *off,
	uint *r_seg, uint *r_off)
{
	PMREGS		r;

	r.x.ax = 0x100;					/* DPMI allocate DOS memory		*/
	r.x.bx = (size + 0xF) >> 4;		/* number of paragraphs 		*/
	PM_int386(0x31, &r, &r);
	if (r.x.cflag) return NULL;		/* DPMI call failed				*/

	*sel = r.x.dx;					/* Protected mode selector		*/
	*off = 0;
	*r_seg = r.x.ax;				/* Real mode segment			*/
	*r_off = 0;
	return 1;
}

void PMAPI PM_freeRealSeg(uint sel,uint off)
{
	PMREGS	r;

	r.x.ax = 0x101;					/* DPMI free DOS memory			*/
	r.x.dx = sel;					/* DX := selector from 0x100	*/
	off = off;
	PM_int386(0x31, &r, &r);
}

#pragma pack(1)

typedef struct {
	long	edi;
	long	esi;
	long	ebp;
	long	reserved;
	long	ebx;
	long	edx;
	long	ecx;
	long	eax;
	short	flags;
	short	es,ds,fs,gs,ip,cs,sp,ss;
	} _RMREGS;

#pragma pack()

#define IN(reg)     rmregs.e##reg = in->x.reg
#define OUT(reg)    out->x.reg = rmregs.e##reg

int PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out)
{
	_RMREGS	rmregs;
	PMREGS	r;
	PMSREGS	sr;

	memset(&rmregs, 0, sizeof(rmregs));
	IN(ax); IN(bx); IN(cx); IN(dx); IN(si); IN(di);

	PM_segread(&sr);
	r.x.ax = 0x300;					/* DPMI issue real interrupt	*/
	r.h.bl = intno;
	r.h.bh = 0;
	r.x.cx = 0;
	sr.es = sr.ds;
	r.e.edi = (uint)&rmregs;
	PM_int386x(0x31, &r, &r, &sr);	/* Issue the interrupt			*/

	OUT(ax); OUT(bx); OUT(cx); OUT(dx); OUT(si); OUT(di);
	out->x.cflag = rmregs.flags & 0x1;
	return out->x.ax;
}

int PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out,
	RMSREGS *sregs)
{
	_RMREGS	rmregs;
	PMREGS	r;
	PMSREGS	sr;

	memset(&rmregs, 0, sizeof(rmregs));
	IN(ax); IN(bx); IN(cx); IN(dx); IN(si); IN(di);
	rmregs.es = sregs->es;
	rmregs.ds = sregs->ds;

	PM_segread(&sr);
	r.x.ax = 0x300;					/* DPMI issue real interrupt	*/
	r.h.bl = intno;
	r.h.bh = 0;
	r.x.cx = 0;
	sr.es = sr.ds;
	r.e.edi = (uint)&rmregs;
	PM_int386x(0x31, &r, &r, &sr);	/* Issue the interrupt */

	OUT(ax); OUT(bx); OUT(cx); OUT(dx); OUT(si); OUT(di);
	sregs->es = rmregs.es;
	sregs->cs = rmregs.cs;
	sregs->ss = rmregs.ss;
	sregs->ds = rmregs.ds;
	out->x.cflag = rmregs.flags & 0x1;
	return out->x.ax;
}

uint PMAPI PM_createSelector(ulong base,ulong limit)
{ return DPMI_createSelector(base,limit); }

void PMAPI PM_freeSelector(uint sel)
{ DPMI_freeSelector(sel); }

void PMAPI PM_availableMemory(ulong *physical,ulong *total)
{
	PMREGS 	r;
	PMSREGS	sr;
	struct MemInfo {
		uint	LargestBlockAvail;
		uint	MaxUnlockedPage;
		uint	LargestLockablePage;
		uint    LinAddrSpace;
		uint    NumFreePagesAvail;
		uint    NumPhysicalPagesFree;
		uint    TotalPhysicalPages;
		uint	FreeLinAddrSpace;
		uint	SizeOfPageFile;
		uint	res[3];
		} memInfo;

	PM_segread(&sr);
	r.x.ax = 0x500;					/* DPMI get free memory info */
	sr.es = sr.ds;
	r.e.edi = (uint)&memInfo;
	PM_int386x(0x31, &r, &r, &sr);	/* Issue the interrupt */
	*physical = memInfo.NumPhysicalPagesFree * 4096;
	*total = memInfo.LargestBlockAvail;
	if (*total < *physical)
		*physical = *total;
}

#endif

