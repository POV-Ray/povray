/****************************************************************************
*
*			The SuperVGA Kit - UniVBE Software Development Kit
*
*                   Copyright (C) 1996 SciTech Software.
*                           All rights reserved.
*
* Filename:     $Workfile:   vesavbe.h  $
* Version:      $Revision:   1.0  $
*
* Language:     ANSI C
* Environment:  IBM PC (MSDOS) Real Mode and 16/32 bit Protected Mode.
*
* Description:  Header file declaring the structures used to communicate
*               with a VESA VBE 1.x or 2.x compliant BIOS.
*
*				Note that this module does automatic translation of all
*				information in the VBE_vgaInfo and VBE_modeInfo blocks so
*				that all pointers are converted to the native format,
*				insulating the application from dealing with a real
*				mode VBE 2.0 implementation.
*
*               MUST be compiled in the LARGE or FLAT models.
*
* $Date:   05 Feb 1996 18:57:26  $ $Author:   KendallB  $
*
****************************************************************************/

#ifndef __VESAVBE_H
#define __VESAVBE_H

#ifndef __DEBUG_H
#include "debug.h"
#endif

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/* SuperVGA information block */

typedef struct {
	char    VESASignature[4];       /* 'VESA' 4 byte signature          */
    short   VESAVersion;            /* VBE version number               */
	char	*OemStringPtr;          /* Pointer to OEM string            */
	ulong	Capabilities;           /* Capabilities of video card       */
	ushort	*VideoModePtr;          /* Pointer to supported modes       */
	short   TotalMemory;            /* Number of 64kb memory blocks     */

	/* VBE 2.0 extension information */

	short	OemSoftwareRev;			/* OEM Software revision number		*/
	char	*OemVendorNamePtr;		/* Pointer to Vendor Name string	*/
	char	*OemProductNamePtr;		/* Pointer to Product Name string	*/
	char	*OemProductRevPtr;		/* Pointer to Product Revision str	*/
	char    reserved[222];          /* Pad to 256 byte block size       */
	char	OemDATA[256];			/* Scratch pad for OEM data			*/
	} VBE_vgaInfo;

/* Flags for the Capabilities field */

#define	vbe8BitDAC		0x0001		/* DAC width is switchable to 8 bit	*/
#define	vbeNonVGA		0x0002		/* Controller is non-VGA			*/
#define	vbeBlankRAMDAC	0x0004		/* Programmed DAC with blank bit	*/

/* SuperVGA mode information block */

typedef struct {
	short   ModeAttributes;         /* Mode attributes                  */
	char    WinAAttributes;         /* Window A attributes              */
	char    WinBAttributes;         /* Window B attributes              */
	short   WinGranularity;         /* Window granularity in k          */
	short   WinSize;                /* Window size in k                 */
	ushort  WinASegment;            /* Window A segment                 */
	ushort  WinBSegment;            /* Window B segment                 */
	void	*WinFuncPtr;			/* Pointer to window function       */
	short   BytesPerScanLine;       /* Bytes per scanline               */
	short   XResolution;            /* Horizontal resolution            */
	short   YResolution;            /* Vertical resolution              */
	char    XCharSize;              /* Character cell width             */
	char    YCharSize;              /* Character cell height            */
	char    NumberOfPlanes;         /* Number of memory planes          */
	char    BitsPerPixel;           /* Bits per pixel                   */
	char    NumberOfBanks;          /* Number of CGA style banks        */
	char    MemoryModel;            /* Memory model type                */
	char    BankSize;               /* Size of CGA style banks          */
	char    NumberOfImagePages;     /* Number of images pages           */
	char    res1;                   /* Reserved                         */
	char    RedMaskSize;            /* Size of direct color red mask    */
	char    RedFieldPosition;       /* Bit posn of lsb of red mask      */
	char    GreenMaskSize;          /* Size of direct color green mask  */
	char    GreenFieldPosition;     /* Bit posn of lsb of green mask    */
	char    BlueMaskSize;           /* Size of direct color blue mask   */
	char    BlueFieldPosition;      /* Bit posn of lsb of blue mask     */
	char    RsvdMaskSize;           /* Size of direct color res mask    */
	char    RsvdFieldPosition;      /* Bit posn of lsb of res mask      */
	char    DirectColorModeInfo;    /* Direct color mode attributes     */

	/* VBE 2.0 extensions information */

	ulong	PhysBasePtr;			/* Physical address for linear buf	*/
	ulong	OffScreenMemOffset;		/* Pointer to start of offscreen mem*/
	short   OffScreenMemSize;		/* Amount of offscreen mem in 1K's	*/
	char    res2[206];        		/* Pad to 256 byte block size       */
	} VBE_modeInfo;

/* 32 bit protected mode info block */

typedef struct {
	short   setWindow;				/* Offset of Set Window call		*/
	short	setDisplayStart;		/* Offset of Set Display Start call	*/
	short   setPalette;				/* Offset of Set Primary Palette	*/
	short   IOPrivInfo;				/* Offset of I/O priveledge info	*/
	/* UniVBE 5.2 extensions */
	long	extensionSig;			/* Identify extension (0xFBADFBAD)	*/
	long	setWindowLen;			/* Used to virtualise framebuffer	*/
	long	setDisplayStartLen;
	long	setPaletteLen;
	/* ... code and tables located in here */
	} VBE_pmInfo;

#define	VBE20_EXT_SIG				0xFBADFBAD

typedef enum {
	vbeMemTXT      = 0,           	/* Text mode memory model           */
	vbeMemCGA      = 1,            	/* CGA style mode                   */
	vbeMemHGC      = 2,            	/* Hercules graphics style mode     */
	vbeMemPL       = 3,            	/* 16 color VGA style planar mode   */
	vbeMemPK       = 4,            	/* Packed pixel mode                */
	vbeMemX        = 5,           	/* Non-chain 4, 256 color (ModeX)   */
	vbeMemRGB      = 6,            	/* Direct color RGB mode        	*/
	vbeMemYUV      = 7,            	/* Direct color YUV mode         	*/
	} VBE_memModels;

/* Flags for combining with video modes during mode set */

#define	vbeDontClear	0x8000		/* Dont clear display memory		*/
#define	vbeLinearBuffer	0x4000		/* Enable linear framebuffer mode	*/

/* Flags for the mode attributes returned by VBE_getModeInfo. If
 * vbeMdNonBanked is set to 1 and vbeMdLinear is also set to 1, then only
 * the linear framebuffer mode is available.
 */

#define vbeMdAvailable	0x0001		/* Video mode is available			*/
#define	vbeMdTTYOutput	0x0004		/* TTY BIOS output is supported		*/
#define	vbeMdColorMode	0x0008		/* Mode is a color video mode		*/
#define	vbeMdGraphMode	0x0010		/* Mode is a graphics mode			*/
#define	vbeMdNonVGA		0x0020		/* Mode is not VGA compatible		*/
#define	vbeMdNonBanked	0x0040		/* Banked mode is not supported		*/
#define	vbeMdLinear		0x0080		/* Linear mode supported			*/

/* Flags for save/restore state calls */

#define	vbeStHardware	0x0001		/* Save the hardware state			*/
#define	vbeStBIOS		0x0002		/* Save the BIOS state				*/
#define	vbeStDAC		0x0004		/* Save the DAC state				*/
#define	vbeStSVGA		0x0008		/* Save the SuperVGA state			*/
#define	vbeStAll		0x000F		/* Save all states					*/

/* Palette entry structure, either 6 or 8 bit format. This is the VBE 2.0
 * palette format and the same as used by Windows (which is NOT the same
 * as the normal VGA BIOS palette format structure).
 */

typedef struct {
	uchar	blue;					/* Blue component of color			*/
	uchar	green;					/* Green component of color			*/
	uchar	red;					/* Blue component of color			*/
	uchar	alpha;					/* Alpha or alignment byte			*/
	} VBE_palette;

#pragma pack()

/*---------------------------- Global Variables ---------------------------*/
#ifdef  __cplusplus
extern "C" {            			/* Use "C" linkage when in C++ mode */
#endif

/* These variables must be provided by the application using this module.
 * Rather than have the VESAVBE.C module grab the low DOS memory it needs,
 * the application must do this before using this module so that we dont
 * allocate more low memory than is already needed (the app may require
 * a transfer buffer for something else, so can re-use that low memory
 * buffer).
 */

extern uint	_ASMAPI VESABuf_len;	/* Length of the VESABuf buffer		*/
extern uint	_ASMAPI VESABuf_sel;	/* Selector for VESABuf				*/
extern uint	_ASMAPI VESABuf_off;	/* Offset for VESABuf				*/
extern uint	_ASMAPI VESABuf_rseg;	/* Real mode segment of VESABuf		*/
extern uint	_ASMAPI VESABuf_roff;	/* Real mode offset of VESABuf		*/

/*-------------------------- Function Prototypes --------------------------*/

/* Callback provided by application to allocate real mode buffer */

void 	_PUBAPI VBE_initRMBuf(void);

/* Main VBE API routines */
int     _PUBAPI VBE_detect(VBE_vgaInfo *vgaInfo);
bool    _PUBAPI VBE_getModeInfo(int mode,VBE_modeInfo *modeInfo);
long    _PUBAPI VBE_getPageSize(VBE_modeInfo *modeInfo);
bool    _PUBAPI VBE_setVideoMode(int mode);
int     _PUBAPI VBE_getVideoMode(void);
bool    _PUBAPI VBE_setBank(int window,int bank);
int     _PUBAPI VBE_getBank(int window);
bool    _PUBAPI VBE_setPixelsPerLine(int pixelsPerLine,int *newBytes,int *newPixels,
		int *maxScanlines);
bool    _PUBAPI VBE_setBytesPerLine(int bytesPerLine,int *newBytes,int *newPixels,
		int *maxScanlines);
bool    _PUBAPI VBE_getScanlineLength(int *bytesPerLine,int *pixelsPerLine,
		int *maxScanlines);
bool    _PUBAPI VBE_getMaxScanlineLength(int *maxBytes,int *maxPixels);
bool    _PUBAPI VBE_setDisplayStart(int x,int y,bool waitVRT);
bool    _PUBAPI VBE_getDisplayStart(int *x,int *y);
bool    _PUBAPI VBE_setDACWidth(int width);
int     _PUBAPI VBE_getDACWidth(void);
bool    _PUBAPI VBE_setPalette(int start,int num,VBE_palette *pal,bool waitVRT);
void *	_PUBAPI VBE_getBankedPointer(VBE_modeInfo *modeInfo);

/* Functions to return the addresses of the VBE 2.0 32 bit code */

#ifndef	REALMODE
ushort  _PUBAPI VBE_getLinearSelector(VBE_modeInfo *modeInfo);
void * 	_PUBAPI VBE_getLinearPointer(VBE_modeInfo *modeInfo);
void * 	_PUBAPI VBE_getSetBank(void);
void * 	_PUBAPI VBE_getSetDisplayStart(void);
void * 	_PUBAPI VBE_getSetPalette(void);
void    _PUBAPI VBE_freePMCode(void);
bool	_PUBAPI VBE_getBankFunc32(int *codeLen,void **bankFunc,
		int dualBanks,int bankAdjust);
#endif

#ifdef  __cplusplus
}                       			/* End of "C" linkage for C++   	*/
#endif

#endif  /* __VESAVBE_H */
