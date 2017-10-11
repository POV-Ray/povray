/****************************************************************************
*
*						   Console Output Library
*
*                   Copyright (C) 1996 SciTech Software
*							All rights reserved.
*
* Filename:		$RCSfile: console.h $
* Version:		$Revision:   1.0  $
*
* Language:		ANSI C
* Environment:	IBM PC 16/32 bit code
*
* Description:	Header file for console manipulation module. This is a
*				small module for fast, compiler independant console
*				output routines. It has been modified to for use with
*				32 bit flat model compilers and the code is being made
*				freely available for use in the POVRay Ray Tracer.
*
*				Has also been completely re-written to only use direct
*				video output code, since there is not longer any need to
*				send any output via the BIOS these days.
*
* $Id: console.h 1.1 1995/09/16 10:58:30 kjb release $
*
****************************************************************************/

#ifndef	__CONSOLE_H
#define	__CONSOLE_H

#ifndef	__DEBUG_H
#include "debug.h"
#endif

/*--------------------- Macros and type definitions -----------------------*/

enum CONSOLE_COLORS {
	CON_BLACK,					/* dark colors	*/
	CON_BLUE,
	CON_GREEN,
	CON_CYAN,
	CON_RED,
	CON_MAGENTA,
	CON_BROWN,
	CON_LIGHTGRAY,
	CON_DARKGRAY,				/* light colors	*/
	CON_LIGHTBLUE,
	CON_LIGHTGREEN,
	CON_LIGHTCYAN,
	CON_LIGHTRED,
	CON_LIGHTMAGENTA,
	CON_YELLOW,
	CON_WHITE,
	};

#define	CON_BLINK	128			/* Blink bit	*/

/* Predefined cursor types */

#define	CON_CURSOR_NORMAL	1
#define	CON_CURSOR_FULL		2

/* Scrolling directions */

#define	CON_SCROLL_UP		1
#define	CON_SCROLL_DOWN		2
#define	CON_SCROLL_LEFT		3
#define	CON_SCROLL_RIGHT	4

/* Macro to build attribute bytes */

#define	CON_makeAttr(f,b)	(char)( (((b) & 0x0F) << 4) | ((f) & 0x0F))

/*------------------------- Function Prototypes ---------------------------*/

#ifdef	__cplusplus
extern "C" {			/* Use "C" linkage when in C++ mode	*/
#endif

void	_PUBAPI CON_init(void);
void	_PUBAPI CON_set25LineMode(void);
bool	_PUBAPI	CON_set43LineMode(void);
bool	_PUBAPI	CON_set50LineMode(void);
void	_PUBAPI	CON_restoreMode(void);
void	_PUBAPI CON_setLineWrap(bool on);
void	_PUBAPI CON_printf(char *format, ...);
void 	_PUBAPI CON_puts(char *str);
void 	_PUBAPI CON_putc(int c);
void 	_PUBAPI CON_writec(int x,int y,int attr,int c);
void	_PUBAPI CON_write(int x,int y,int attr,char *str);
void 	_PUBAPI CON_clreol(void);
void 	_PUBAPI CON_clrscr(void);
void 	_PUBAPI CON_gotoxy(int x,int y);
int 	_PUBAPI CON_wherex(void);
int	 	_PUBAPI CON_wherey(void);
void 	_PUBAPI CON_delline(void);
void 	_PUBAPI CON_insline(void);
void 	_PUBAPI CON_moveText(int left,int top,int right,int bottom,int destleft,int desttop);
#define			CON_bufSize(width,height) ((width) * (height) * 2)
void 	_PUBAPI CON_saveText(int left,int top,int right,int bottom,void *dest);
void 	_PUBAPI CON_restoreText(int left,int top,int right,int bottom,void *source);
void 	_PUBAPI CON_scroll(int direction,int amt);
void 	_PUBAPI CON_fillText(int left,int top,int right,int bottom,int attr,int ch);
void 	_PUBAPI CON_fillAttr(int left,int top,int right,int bottom,int attr);
void 	_PUBAPI CON_setWindow(int left,int top,int right,int bottom);
void 	_PUBAPI CON_getWindow(int *left,int *top,int *right,int *bottom);
int		_PUBAPI CON_maxx(void);
int		_PUBAPI CON_maxy(void);
int		_PUBAPI CON_getAttr(void);
void 	_PUBAPI CON_setAttr(int attr);
void	_PUBAPI CON_setBackground(int attr,int ch);
void	_PUBAPI CON_getBackground(int *attr,int *ch);
void 	_PUBAPI CON_setBackColor(int newcolor);
void 	_PUBAPI CON_setForeColor(int newcolor);
void 	_PUBAPI CON_setCursor(int type);
void 	_PUBAPI CON_cursorOff(void);
void 	_PUBAPI CON_restoreCursor(int scans);
int 	_PUBAPI CON_getCursor(void);
void	_PUBAPI CON_setActivePage(int page);
int		_PUBAPI CON_getActivePage(void);
void	_PUBAPI CON_setVisualPage(int page);
int		_PUBAPI CON_getVisualPage(void);
bool	_PUBAPI CON_isMonoMode(void);
int		_PUBAPI	CON_screenWidth(void);
int		_PUBAPI	CON_screenHeight(void);

#ifdef	__cplusplus
}						/* End of "C" linkage for C++	*/
#endif

#endif  /* __CONSOLE_H */
