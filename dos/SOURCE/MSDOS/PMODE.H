/****************************************************************************
*
*						 	  PM/Lite Library
*
*					Copyright (C) 1996 SciTech Software.
*							All rights reserved.
*
* Filename:		$Workfile:   pmode.h  $
* Version:		$Revision:   1.0  $
*
* Language:		ANSI C
* Environment:	Real mode and 16/32 bit Protected Mode under MSDOS
*
* Description:	Header file for the DOS extender independant protected
*				mode programming library. This library will need to be
*				included in all programs that use SciTech Software's
*				products that are to be compiled in protected mode.
*
*				This library provides pre-built selectors for the BIOS
*				data area and VGA frame buffer, and methods for allocating
*				your own selectors for physical memory. It also returns
*				appropriate selectors for accessing memory allocated in
*				the low DOS 1Mb memory area, and routines for accessing
*				memory through a selector and offset. By using selectors
*				for accessing memory outside of the applications linear
*				address space, your code will be fully DPMI compliant and
*				will run under Windows 3.1 and OS/2 2.x DOS boxes.
*
*				Works with the following:
*
*					Real Mode DOS (large memory model)
*
*               16 bit Extenders:
*
*					Windows 3.1
*					Borland DPMI16
*
*               32 bit Extenders:
*
*					Windows 3.1 (Win32s)
*					Windows '95 (Win32)
*					Phar Lap TNT DOS Extender
*					Symantec DOSX
*					FlashTek X-32/X-32VM
*					Borland DPMI32
*					Rational DOS/4GW
*                   DJGPP go32 for GNU C++          * NO SELECTORS YET!! *
*
*				Currently supports the following compilers:
*
*					Borland C++ 3.1
*					Borland C++ 4.0, 16 bit
*					Borland C++ 4.0, 32 bit
*                   Microsoft Visual C++ 1.5, 16 bit
*                   Microsoft Visual C++ 1.5, 32 bit
*					Symantec C++ 6.1, 16 bit
*					Symantec C++ 6.1, 32 bit
*					Watcom C++ 10.0, 16 bit
*                   Watcom C++ 10.0, 32 bit
*					Metaware High C++ 3.21, 32 bit
*					DJGPP port of GNU C++, 32 bit	* NO SELECTORS YET!! *
*
* $Date:   05 Feb 1996 21:44:32  $ $Author:   KendallB  $
*
****************************************************************************/

#ifndef	__PMODE_H
#define	__PMODE_H

#ifndef	__DEBUG_H
#include "debug.h"
#endif

/*--------------------------- Macros and Typedefs -------------------------*/

/* You will need to define one of the following before you compile this
 * library for it to work correctly with the DOS extender that you are
 * using when compiling for extended DOS:
 *
 *      DPMI16      - Borland's DPMI16 DOS Power Pack Extender
 *		TNT			- Phar Lap TNT DOS Extender
 *		DOSX		- Symantec C++ DOSX
 *		X32VM		- Flashtek X-32/X-32VM
 *      DPMI32      - Borland's DPMI32 DOS Power Pack Extender
 *		DOS4GW		- Rational DOS/4GW and DOS/4GW Professional
 *		DJGPP		- DJGPP port of GNU C++
 *
 * If none is specified, we will automatically determine which operating
 * system is being targetted and the following will be defined (provided by
 * debug.h header file):
 *
 *		__MSDOS16__		- Default for 16 bit MSDOS mode
 *		__MSDOS32__		- Default for 32 bit MSDOS
 *		__WINDOWS16__	- Default for 16 bit Windows
 *		__WINDOWS32__	- Default for 32 bit Windows
 *
 * One of the following will be defined automatically for you to select
 * which memory model is in effect:
 *
 *		REALMODE	- 16 bit real mode (large memory model)
 *		PM286		- 16 protected mode (large memory model)
 *		PM386		- 32 protected mode (flat memory model)
 */

#if defined(TNT) || defined(DOSX) || defined(X32VM) || defined(DPMI32)		\
	|| defined(DOS4GW) || defined(DJGPP) || defined(__WINDOWS32__)			\
	|| defined(__MSDOS32__)
#define	PM386
#elif defined(DPMI16) || defined(__WINDOWS16__)
#define	PM286
#else
#define	REALMODE
#endif

#pragma pack(1)

/* Provide definitions for the real mode register structures passed to
 * the PM_int86() and PM_int86x() routines.
 */

#if defined(REALMODE) || defined(PM286)
#include <dos.h>
typedef union REGS RMREGS;
typedef struct SREGS RMSREGS;
#else
struct _RMWORDREGS {
	ushort ax, bx, cx, dx, si, di, cflag;
	};

struct _RMBYTEREGS {
	uchar   al, ah, bl, bh, cl, ch, dl, dh;
	};

typedef union {
	struct  _RMWORDREGS x;
	struct  _RMBYTEREGS h;
	} RMREGS;

typedef struct {
	ushort  es;
	ushort  cs;
	ushort  ss;
	ushort	ds;
	} RMSREGS;
#endif

struct _PMDWORDREGS {
	ulong	eax,ebx,ecx,edx,esi,edi,cflag;
	};

struct _PMWORDREGS {
	ushort 	ax,ax_hi;
	ushort 	bx,bx_hi;
	ushort 	cx,cx_hi;
	ushort 	dx,dx_hi;
	ushort 	si,si_hi;
	ushort 	di,di_hi;
	ushort	cflag,cflag_hi;
	};

struct _PMBYTEREGS {
	uchar	al, ah;	ushort:16;
	uchar	bl, bh;	ushort:16;
	uchar	cl, ch; ushort:16;
	uchar	dl, dh; ushort:16;
	};

typedef union {
	struct  _PMDWORDREGS e;
	struct  _PMWORDREGS  x;
	struct  _PMBYTEREGS  h;
	} PMREGS;

typedef struct {
	ushort  es;
	ushort  cs;
	ushort  ss;
	ushort	ds;
	ushort	fs;
	ushort	gs;
	} PMSREGS;

/* Define a macro for creating physical base addresses from segment:offset */

#define MK_PHYS(s,o)  (((ulong)(s) << 4) + (ulong)(o))

/* Define the different types of modes supported. This is a global variable
 * that can be used to determine the type at runtime which will contain
 * one of these values.
 */

typedef enum {
	PM_realMode,
	PM_286,
	PM_386,
	} PM_mode_enum;

/* Define a macro for changing the default stack size. For some compilers
 * and extenders this is done at link time, for others you need to change
 * a global variable (Borland C++, X-32 with any compiler). For compilers
 * that do it on the command line, this expands to empty.
 */

#ifdef	__WINDOWS__
#define	DECLARE_STACK(s16,s32)
#elif	defined(__TURBOC__)
#define	DECLARE_STACK(s16,s32)	\
extern uint _stklen = s16;
#elif	defined(X32VM)
#define	DECLARE_STACK(s16,s32)	\
uint int _x32_stack_size = s32;
#else
#define DECLARE_STACK(s16,s32)
#endif

#if		defined(__WINDOWS32__)
#define	PMAPI	_DLLASM			/* 'C' calling conventions for Win32	*/
#elif	defined(__WINDOWS16__)
#define	PMAPI	_DLLAPI			/* Pascal calling conventions for Win16	*/
#else
#define PMAPI   _ASMAPI			/* 'C' calling conventions otherwise	*/
#endif

/*--------------------------- Function Prototypes -------------------------*/

#ifdef	__cplusplus
extern "C" {			/* Use "C" linkage when in C++ mode	*/
#endif

/* Routine to return either PM_realMode, PM_286 or PM_386 */

int 	PMAPI PM_getModeType(void);

/* Routines to access data through a selector and offset. For real mode
 * and 16 bit protected mode (and also Win32), the offset can only be a
 * maximum of 64k.
 */

uchar 	PMAPI PM_getByte(uint s, uint o);
ushort 	PMAPI PM_getWord(uint s, uint o);
ulong 	PMAPI PM_getLong(uint s, uint o);
void 	PMAPI PM_setByte(uint s, uint o, uchar v);
void 	PMAPI PM_setWord(uint s, uint o, ushort v);
void 	PMAPI PM_setLong(uint s, uint o, ulong v);

/* Routines for copying data between the applications data space and
 * memory accessible through a selector and offset.
 */

void 	PMAPI PM_memcpynf(void *dst,uint src_s,uint src_o,uint n);
void 	PMAPI PM_memcpyfn(uint dst_s,uint dst_o,void *src,uint n);

/* Routine to return a selector to the BIOS data area at segment 0x40 */

uint 	PMAPI PM_getBIOSSelector(void);

/* Routine to return a selector to the VGA frame buffer. The selector
 * will map to the correct portion of video memory depending on the
 * current video mode (0x3, 0x7 or graphics).
 */

uint 	PMAPI PM_getVGASelector(void);

/* Routines to return specific selectors to the VGA frame buffer memory */

uint 	PMAPI PM_getVGAColorTextSelector(void);
uint 	PMAPI PM_getVGAMonoTextSelector(void);
uint 	PMAPI PM_getVGAGraphSelector(void);

/* Routines to map/free physical memory into the current DS segment. After
 * the mapping has been allocated, it cannot be freed. Hence you should
 * only allocate the mapping once and cache the value for use by other
 * parts of your application. If this cannot be done, this function will
 * return a NULL pointer.
 *
 * This routine will also work for memory addresses below 1Mb, but the
 * memory cannot cross the 1Mb boundary.
 */

void * 	PMAPI PM_mapPhysicalAddr(ulong base,ulong limit);

/* Routine to load save default data segment selector value into a code
 * segment variable, and another to load the value into the DS register.
 */

void 	PMAPI PM_loadDS(void);
void 	PMAPI PM_saveDS(void);

/* Routine to get a selector:offset for accessing a low 1Mb memory block.
 * You dont need to free this pointer, but in 16 bit protected mode
 * the selector allocated will be re-used the next time this routine is
 * called. If you need a permanent selector, allocate it with
 * PM_createSelector instead.
 */

void 	PMAPI PM_mapRealPointer(uint *sel,uint *off,uint r_seg,uint r_off);

/* Routine to create an arbritray selector to physical memory. In 16 bit
 * protected mode you can pass a 32 bit limit to create a 32 bit
 * protected mode data selector.
 */

uint 	PMAPI PM_createSelector(ulong base,ulong limit);
void 	PMAPI PM_freeSelector(uint sel);

/* Routine to create 32 bit code segment alias selector from a data
 * selector. This is only useful for 16 bit Windows code that
 * needs to be able to call 32 bit protected mode code in a USE32 segment.
 * The selector can be freed with PM_freeSelector();
 */

#ifdef	__WINDOWS16__
uint 	PMAPI PM_createCode32Alias(uint sel);
#endif

/* Routine to allocate a block of conventional memory below the 1Mb
 * limit so that it can be accessed from real mode. Ensure that you free
 * the segment when you are done with it.
 *
 * This routine returns a selector and offset to the segment that has been
 * allocated, and also returns the real mode segment and offset which can
 * be passed to real mode routines. Will return 0 if memory could not be
 * allocated.
 *
 * Please note that with some DOS extenders, memory allocated with the
 * following function cannot be freed, hence it will be allocated for the
 * life of your program. Thus if you need to call a bunch of different
 * real-mode routines in your program, allocate a single large buffer at
 * program startup that can be re-used throughout the program execution.
 */

int 	PMAPI PM_allocRealSeg(uint size,uint *sel,uint *off,uint *r_seg,uint *r_off);
void	PMAPI PM_freeRealSeg(uint sel,uint off);

/* Routine to call a real mode assembly language procedure. Register
 * values are passed in and out in the 'regs' and 'sregs' structures. We
 * do not provide any method of copying data from the protected mode stack
 * to the real mode stack, so if you need to pass data to real mode, you will
 * need to write a real mode assembly language hook to recieve the values
 * in registers, and to pass the data through a real mode block allocated
 * with the PM_allocRealSeg() routine.
 */

void 	PMAPI PM_callRealMode(uint seg,uint off, RMREGS *regs,RMSREGS *sregs);

/* Routines to generate real mode interrupts using the same interface that
 * is used by int86() and int86x() in realmode. This routine is need to
 * call certain BIOS and DOS functions that are not supported by some
 * DOS extenders. No translation is done on any of the register values,
 * so they must be correctly set up and translated by the calling program.
 *
 * Normally the DOS extenders will allow you to use the normal int86()
 * function directly and will pass on unhandled calls to real mode to be
 * handled by the real mode handler. However calls to int86x() with real
 * mode segment values to be loaded will cause a GPF if used with the
 * standard int86x(), so you should use these routines if you know you
 * want to call a real mode handler.
 */

int 	PMAPI PM_int86(int intno, RMREGS *in, RMREGS *out);
int 	PMAPI PM_int86x(int intno, RMREGS *in, RMREGS *out,RMSREGS *sregs);

/* Routines to generate native interrupts (ie: protected mode interrupts
 * for protected mode apps) using an interface the same as that use by
 * int86() and int86x() in realmode. These routines are required because
 * many 32 bit compilers use different register structures and different
 * functions causing major portability headaches. Thus we provide our
 * own and solve it all in one fell swoop, and we also get a routine to
 * put stuff into 32 bit registers from real mode ;-)
 */

void 	PMAPI PM_segread(PMSREGS *sregs);
int 	PMAPI PM_int386(int intno, PMREGS *in, PMREGS *out);
int 	PMAPI PM_int386x(int intno, PMREGS *in, PMREGS *out,PMSREGS *sregs);

/* Functions to manage real mode interrupt vectors */

void 	PMAPI _PM_getRMvect(int intno, long *realisr);
void 	PMAPI _PM_setRMvect(int intno, long realisr);

/* Function to return the amount of available physical and total memory.
 * The results of this function are *only* valid before you have made any
 * calls to malloc() and free(). If you need to keep track of exactly how
 * much memory is currently allocated, you need to call this function to
 * get the total amount of memory available and then keep track of
 * the available memory every time you call malloc() and free().
 */

void	PMAPI PM_availableMemory(ulong *physical,ulong *total);

#ifdef	__cplusplus
}						/* End of "C" linkage for C++	*/
#endif

#pragma pack()

#endif /* __PMODE_H */
