/****************************************************************************
*
*				VFlat - DOS Virtual Flat Linear Framebuffer
*
*                   Copyright (C) 1996 SciTech Software.
*							All rights reserved.
*
* Filename:		$Workfile:   vflat.c  $
* Version:		$Revision:   1.1  $
*
* Language:		ANSI C
* Environment:	IBM PC (MS DOS)
*
* Description:	Main C module for the VFlat framebuffer routines. The page
*				fault handler is always installed to handle up to a 4Mb
*				framebuffer with a window size of 4Kb or 64Kb in size.
*
* $Date:   12 Feb 1996 21:58:58  $ $Author:   KendallB  $
*
****************************************************************************/

#include <stdlib.h>
#include <dos.h>
#include "pmpro.h"
#include "debug.h"

#define	VFLAT_START_ADDR	0xF0000000U
#define	VFLAT_END_ADDR		0xF03FFFFFU
#define	VFLAT_LIMIT			(VFLAT_END_ADDR - VFLAT_START_ADDR)
#define PAGE_PRESENT 		1
#define PAGE_NOTPRESENT 	0
#define PAGE_READ 			0
#define PAGE_WRITE			2

#if	defined(DOS4GW)

/*-------------------------------------------------------------------------*/
/* DOS4G/W, PMODE/W and CauseWay support.                       	   	   */
/*-------------------------------------------------------------------------*/

PRIVATE	bool	installed = false;
PRIVATE	bool	haveDPMI = false;
PUBLIC	bool	_ASMAPI VF_haveCauseWay = false;
PUBLIC	ushort	_ASMAPI VF_zeroSel = 0;

/* Low level assembler code */

int     _ASMAPI InitPaging(void);
void    _ASMAPI ClosePaging(void);
void    _ASMAPI MapPhysical2Linear(ulong pAddr, ulong lAddr, int pages, int flags);
void	_ASMAPI InstallFaultHandler(ulong baseAddr,int bankSize);
void	_ASMAPI RemoveFaultHandler(void);
void	_ASMAPI InstallBankFunc(int codeLen,void *bankFunc);

void * _ASMAPI VF_malloc(uint size)
{ return malloc(size); }

void _ASMAPI VF_free(void *p)
{ free(p); }

PRIVATE bool HaveDPMI(void)
/****************************************************************************
*
* Function:		HaveDPMI
* Returns:		True if we are running under DPMI
*
****************************************************************************/
{
	PMREGS	regs;

	if (haveDPMI)
		return true;

	/* Check if we are running under DPMI in which case we will not be
	 * able to install our page fault handlers. We can however use the
	 * DVA.386 or VFLATD.386 virtual device drivers if they are present.
	 */
	regs.x.ax = 0xFF00;
	PM_int386(0x31,&regs,&regs);
	if (!regs.x.cflag && (regs.e.edi & 8))
		return (haveDPMI = true);
	return false;
}

PRIVATE bool HaveCauseWay(void)
/****************************************************************************
*
* Function:		HaveCauseWay
* Returns:		True if we have the CauseWay DOS extender.
*
****************************************************************************/
{
	int		i;
	char	far * Int01Ptr;
	PMREGS	regs;

	if (VF_haveCauseWay)
		return true;

	/* We are running with the CauseWay extender. Search for the
	 * CR2 image inside the extender and also for the CauseWay
	 * signature string. Note that this will fail if we are running
	 * under the Watcom Debugger, so virtual framebuffering is not
	 * available when debugging CauseWay apps (debug then with DOS4GW).
	 */
	regs.x.ax = 0x202;
	regs.h.bl = 0x01;
	PM_int386(0x31,&regs,&regs);
	if (regs.x.cflag)
		return false;

	regs.e.ecx &= 0xFFFF;
	Int01Ptr = MK_FP(regs.e.ecx,regs.e.edx);
	for (i = 0; i < 4096; i++) {
		if (Int01Ptr[i+0]=='C' &&
			Int01Ptr[i+1]=='a' &&
			Int01Ptr[i+2]=='u' &&
			Int01Ptr[i+3]=='s' &&
			Int01Ptr[i+4]=='e' &&
			Int01Ptr[i+5]=='W' &&
			Int01Ptr[i+6]=='a' &&
			Int01Ptr[i+7]=='y')
			break;
		}
	if (i == 4096)
		return false;

	return (VF_haveCauseWay = true);
}

bool PMAPI VF_available(void)
/****************************************************************************
*
* Function:		VF_available
* Returns:		True if virtual buffer is available, false if not.
*
****************************************************************************/
{
	if (!VF_zeroSel)
		VF_zeroSel = PM_createSelector(0,0xFFFFFFFF);

	if (HaveDPMI()) {
		return false;
		}
	if (HaveCauseWay()) {
		/* Currently this is not supported by CauseWay */
		return false;
		}
	else {
		/* Standard DOS4GW or PMODE/W */
		if (InitPaging() == -1)
			return false;
		ClosePaging();
		return true;
		}
}

void * PMAPI InitDPMI(ulong baseAddr,int bankSize,int codeLen,void *bankFunc)
/****************************************************************************
*
* Function:		InitDOS4GW
* Parameters:	baseAddr	- Base address of framebuffer bank window
*				bankSize	- Physical size of banks in Kb (4 or 64)
*               codeLen		- Length of 32 bit bank switch function
* 				bankFunc	- Pointer to protected mode bank function
* Returns:		Near pointer to virtual framebuffer, or NULL on failure.
*
* Description:	Installs the virtual linear framebuffer handling for
*				DPMI environments. This requires the DVA.386 or VFLATD.386
*				virtual device drivers to be installed and functioning.
*
****************************************************************************/
{
	return NULL;
}

void * PMAPI InitDOS4GW(ulong baseAddr,int bankSize,int codeLen,void *bankFunc)
/****************************************************************************
*
* Function:		InitDOS4GW
* Parameters:	baseAddr	- Base address of framebuffer bank window
*				bankSize	- Physical size of banks in Kb (4 or 64)
*               codeLen		- Length of 32 bit bank switch function
* 				bankFunc	- Pointer to protected mode bank function
* Returns:		Near pointer to virtual framebuffer, or NULL on failure.
*
* Description:	Installs the virtual linear framebuffer handling for
*				the DOS4GW extender.
*
****************************************************************************/
{
	int		i;

	if (InitPaging() == -1)
		return NULL;			/* Cannot do hardware paging!		*/

	/* Map 4MB of video memory into linear address space (read/write) */
	if (bankSize == 64) {
		for (i = 0; i < 64; i++) {
			MapPhysical2Linear(baseAddr,VFLAT_START_ADDR+(i<<16),16,
				PAGE_WRITE | PAGE_NOTPRESENT);
			}
		}
	else {
		for (i = 0; i < 1024; i++) {
			MapPhysical2Linear(baseAddr,VFLAT_START_ADDR+(i<<12),1,
				PAGE_WRITE | PAGE_NOTPRESENT);
			}
		}

	/* Install our page fault handler and banks switch function */
	InstallFaultHandler(baseAddr,bankSize);
	InstallBankFunc(codeLen,bankFunc);
	installed = true;
	return (void*)VFLAT_START_ADDR;
}

void * PMAPI VF_init(ulong baseAddr,int bankSize,int codeLen,void *bankFunc)
/****************************************************************************
*
* Function:		VF_init
* Parameters:	baseAddr	- Base address of framebuffer bank window
*				bankSize	- Physical size of banks in Kb (4 or 64)
*               codeLen		- Length of 32 bit bank switch function
* 				bankFunc	- Pointer to protected mode bank function
* Returns:		Near pointer to virtual framebuffer, or NULL on failure.
*
* Description:	Installs the virtual linear framebuffer handling.
*
****************************************************************************/
{
	if (installed)
		return (void*)VFLAT_START_ADDR;
	if (codeLen > 100)
		return NULL;				/* Bank function is too large!		*/
	if (!VF_zeroSel)
		VF_zeroSel = PM_createSelector(0,0xFFFFFFFF);
	if (HaveDPMI())
		return InitDPMI(baseAddr,bankSize,codeLen,bankFunc);
	return InitDOS4GW(baseAddr,bankSize,codeLen,bankFunc);
}

void PMAPI VF_exit(void)
/****************************************************************************
*
* Function:		VF_exit
*
* Description:	Closes down the virtual framebuffer services and
*				restores the previous page fault handler.
*
****************************************************************************/
{
	if (installed) {
		if (haveDPMI) {
			/* DPMI support */
			}
		else {
			/* Standard DOS4GW and PMODE/W support */
			RemoveFaultHandler();
			ClosePaging();
			}
		installed = false;
		}
}

#elif defined(X32VM)

/*-------------------------------------------------------------------------*/
/* FlashTek X32-VM support (not currently working!).                  	   */
/*-------------------------------------------------------------------------*/

/* All in assembler */

#elif	!defined(__WINDOWS__)

/*-------------------------------------------------------------------------*/
/* Support mapped out for other compilers. Windows support is provided	   */
/* by the WinDirect DLL's.										   		   */
/*-------------------------------------------------------------------------*/

bool PMAPI VF_available(void)
{
	return false;
}

void * PMAPI VF_init(ulong baseAddr,int bankSize,int codeLen,void *bankFunc)
{
	baseAddr = baseAddr;
	bankSize = bankSize;
	codeLen = codeLen;
	bankFunc = bankFunc;
	return NULL;
}

void PMAPI VF_exit(void)
{
}

#endif
