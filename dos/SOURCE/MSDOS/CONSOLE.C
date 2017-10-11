/****************************************************************************
*
*						   Console Output Library
*
*                   Copyright (C) 1996 SciTech Software
*							All rights reserved.
*
* Filename:		$Workfile:   console.c  $
* Version:		$Revision:   1.0  $
*
* Language:		ANSI C
* Environment:	IBM PC 16/32 bit code
*
* Description:	Main module for console manipulation module. This is a
*				small module for fast, compiler independant console
*				output routines. It has been modified to for use with
*				32 bit flat model compilers and the code is being made
*				freely available for use in the POVRay Ray Tracer.
*
*				Has also been completely re-written to only use direct
*				video output code, since there is not longer any need to
*				send any output via the BIOS these days, and to no longer
*				use any assembler specific code.
*
* $Date:   05 Feb 1996 20:44:28  $ $Author:   KendallB  $
*
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "console.h"
#include "pmode.h"

/*---------------------------- Global variables ---------------------------*/

#define	LINES_43	0x2000
#define	LINES_50	0x4000

PRIVATE int     _attr = 0x07;           /* Current global attribute     */
PRIVATE int     _backAttr = 0x07;       /* Background attribute         */
PRIVATE int     _backChar = ' ';        /* Background character         */
PRIVATE int     _activePage;     		/* Active display page          */
PRIVATE int     _visualPage;     		/* Visual display page          */
PRIVATE int     _win_left;          	/* Left edge of window          */
PRIVATE int     _win_top;           	/* Top edge of window           */
PRIVATE int     _win_right;        		/* Right edge of window         */
PRIVATE int     _win_bottom;       		/* Bottom edge of window        */
PRIVATE int     _win_width;        		/* Width of window              */
PRIVATE int     _win_height;       		/* Height of window             */
PRIVATE	int		_screenWidth;			/* Width of entire screen		*/
PRIVATE	int		_screenHeight;			/* Height of entire screen		*/
PRIVATE char    *_screenPtrBase;        /* Pointer to video buffer start*/
PRIVATE	char    *_screenPtr;        	/* Pointer to video display     */
PRIVATE	uint	_biosSel;           	/* Selector to BIOS data area	*/
PRIVATE int     _cnt_x = 0,_cnt_y = 0;  /* Current cursor coordinates   */
PRIVATE	bool	_lineWrap = true;		/* Line wrap mode               */
PRIVATE	int		_oldMode;				/* Old BIOS mode				*/

/*---------------------------- Implementation -----------------------------*/

#define	bufferAddr(x,y)	(_screenPtr + (((_screenWidth * (y)) + (x)) * 2))

#define	writeChar(x,y,ch,attr)		\
{									\
	char *p = bufferAddr((x),(y));	\
	*p++ = (ch);					\
	*p = attr;						\
}

#define	writeStr(x,y,attr,str,len)	\
{                                   \
	char *p = bufferAddr(x,y);    	\
	while ((len)--) {               \
		*p++ = *str++;              \
		*p++ = attr;               	\
		}                           \
}

PRIVATE void moveText(int left,int top,int right,int bottom,int dstLeft,
	int dstTop)
/****************************************************************************
*
* Function:     moveText
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*               destleft    - Destination region left edge
*               desttop     - Destination region top edge
*
* Description:  Moves a block of video memory to another location using
*               calls to the direct video routine screentoscreen().
*
****************************************************************************/
{
    int     i,increment,len;
    char    *source,*dest;

	increment = _screenWidth * 2;
    len = (right - left + 1) * 2;
	if (top > dstTop) {
		source = bufferAddr(left,top);
		dest = bufferAddr(dstLeft,dstTop);
        for (i = top; i <= bottom; i++) {
			memmove(dest,source,len);
			source += increment;
			dest += increment;
			}
		}
	else {
		source = bufferAddr(left,bottom);
		dest = bufferAddr(dstLeft,(dstTop + bottom - top));
		for (i = top; i <= bottom; i++) {
			memmove(dest,source,len);
			source -= increment;
            dest -= increment;
			}
        }
}

PRIVATE void moveToScreen(int left,int top,int right,int bottom,
	char *source)
/****************************************************************************
*
* Function:     moveToScreen
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*				source		- Place to store the memory region
*
* Description:  Moves a block of video memory from a buffer directly to
*				the screen.
*
****************************************************************************/
{
	int     i,increment,len;
	char    *dest = bufferAddr(left,top);

	increment = _screenWidth * 2;
	len = (right - left + 1) * 2;
	for (i = top; i <= bottom; i++) {
		memcpy(dest,source,len);
		source += len;
		dest += increment;
		}
}

PRIVATE void moveFromScreen(int left,int top,int right,int bottom,
	char *dest)
/****************************************************************************
*
* Function:     moveFromScreen
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*				dest		- Place to store the memory region
*
* Description:  Moves a block of video memory from the screen into a buffer.
*
****************************************************************************/
{
	int     i,increment,len;
	char    *source = bufferAddr(left,top);

	increment = _screenWidth * 2;
	len = (right - left + 1) * 2;
	for (i = top; i <= bottom; i++) {
		memcpy(dest,source,len);
		source += increment;
		dest += len;
		}
}

PRIVATE void fillText(int left,int top,int right,int bottom,int attr,int ch)
/****************************************************************************
*
* Function:     fillText
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*				attr		- Attribute to fill with
*				ch			- Character to fill with
*
* Description:  Fills a block of memory with the specified character and
*				attribute.
*
****************************************************************************/
{
	int     i,j,increment,len;
	char    *dest = bufferAddr(left,top);

	len = (right - left + 1);
	increment = (_screenWidth - len) * 2;
	for (i = top; i <= bottom; i++) {
		for (j = 0; j < len; j++) {
			*dest++ = ch;
			*dest++ = attr;
			}
		dest += increment;
		}
}

PRIVATE void fillAttr(int left,int top,int right,int bottom,int attr)
/****************************************************************************
*
* Function:     fillAttr
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*				attr		- Attribute to fill with
*
* Description:  Fills a block of memory with the specified attribute,
*				leaving the characters unchanged.
*
****************************************************************************/
{
	int     i,j,increment,len;
	char    *dest = bufferAddr(left,top);

	len = (right - left + 1);
	increment = (_screenWidth - len) * 2;
	for (i = top; i <= bottom; i++) {
		for (j = 0; j < len; j++) {
			dest++;
			*dest++ = attr;
			}
		dest += increment;
		}
}

void _PUBAPI CON_init(void)
/****************************************************************************
*
* Function:     CON_init
*
* Description:  Initialise the console subsystem. This should be called
*               at program inception, and also after any changes to the
*               current video mode (ie: change to 43 line mode, change to
*               monochrome adapter etc).
*
****************************************************************************/
{
	_biosSel = PM_getBIOSSelector();
	if (CON_isMonoMode())
		_screenPtrBase = (char *)PM_mapPhysicalAddr(0xB0000L, 0xFFFF);
	else _screenPtrBase = (char *)PM_mapPhysicalAddr(0xB8000L, 0xFFFF);
	_win_width = _screenWidth = PM_getByte(_biosSel,0x4A);
	_win_height = _screenHeight = PM_getByte(_biosSel,0x84)+1;
	_win_left = _win_top = 0;
	_win_right = _win_width - 1;
	_win_bottom = _win_height - 1;
	CON_setVisualPage(0);
	CON_setActivePage(0);
	CON_setCursor(CON_CURSOR_NORMAL);

	/* Save current video mode so we can restore it later */
	_oldMode = PM_getByte(_biosSel, 0x49);
	if (_screenHeight == 43)
		_oldMode |= LINES_43;
	if (_screenHeight == 50)
		_oldMode |= LINES_50;
}

void _PUBAPI CON_restoreMode(void)
/****************************************************************************
*
* Function:     CON_restoreMode
*
* Description:  Restore the original text mode active when CON_init()
*				was called. Will correctly restore 25, 43 and 50 line
*				VGA video modes.
*
****************************************************************************/
{
	PMREGS  regs;

	CON_set25LineMode();			/* Reset to 80x25 mode first	*/
	regs.h.ah = 0x00;               /* Set video mode service       */
	regs.h.al = (char)_oldMode;     /* Mode into AL                 */
	PM_int386(0x10,&regs,&regs);
	if ((_oldMode & ~LINES_43) == 0x03 && (_oldMode & LINES_43))
		CON_set43LineMode();
	if ((_oldMode & ~LINES_50) == 0x03 && (_oldMode & LINES_50))
		CON_set50LineMode();
}

void _PUBAPI CON_set25LineMode(void)
/****************************************************************************
*
* Function:     CON_set25LineMode
*
* Description:  Set the standard VGA 80x25 line video mode.
*
****************************************************************************/
{
	PMREGS  regs;

	regs.x.ax = 0x1202;				/* Set system into 400 line mode	*/
	regs.x.bx = 0x30;
	PM_int386(0x10,&regs,&regs);

	regs.x.ax = 0x3;				/* Set 80x25 text mode				*/
	PM_int386(0x10,&regs,&regs);
}

bool _PUBAPI CON_set43LineMode(void)
/****************************************************************************
*
* Function:     CON_set43LineMode
*
* Description:  Set the standard VGA 80x43 line video mode.
*
****************************************************************************/
{
	PMREGS  regs;

	if (CON_isMonoMode())
		return false;

	regs.x.ax = 0x1201;				/* Set system into 350 line mode	*/
	regs.x.bx = 0x30;
	PM_int386(0x10,&regs,&regs);

	regs.x.ax = 0x3;
	PM_int386(0x10,&regs,&regs);	/* Set 80x25 text mode				*/

	regs.x.ax = 0x1112;				/* Load 8x8 character font			*/
	regs.x.bx = 0x0;
	PM_int386(0x10,&regs,&regs);
	return true;
}

bool _PUBAPI CON_set50LineMode(void)
/****************************************************************************
*
* Function:     CON_set50LineMode
*
* Description:  Set the standard VGA 80x50 line video mode.
*
****************************************************************************/
{
	PMREGS  regs;

	if (CON_isMonoMode())
		return false;

	regs.x.ax = 0x1202;				/* Set system into 400 line mode	*/
	regs.x.bx = 0x30;
	PM_int386(0x10,&regs,&regs);

	regs.x.ax = 0x3;				/* Set 80x25 text mode				*/
	PM_int386(0x10,&regs,&regs);

	regs.x.ax = 0x1112;				/* Load 8x8 character font			*/
	regs.x.bx = 0x0;
	PM_int386(0x10,&regs,&regs);
	return true;
}

void _PUBAPI CON_printf(char *format, ...)
/****************************************************************************
*
* Function:     CON_printf
* Parameters:   format  - Format string
*
* Description:  Printf function that sends its output via the CONSOLE
*               routines, either through the BIOS or direct to video ram.
*               Formatting information is handled correctly.
*
****************************************************************************/
{
    va_list     args;
    static char buf[256];

    va_start(args,format);
    vsprintf(buf,format,args);
    CON_puts(buf);
    va_end(args);
}

PRIVATE void outputBuf(char *buf,int len)
{
	writeStr(_cnt_x + _win_left,_cnt_y + _win_top,_attr,buf,len);
}

PRIVATE void syncCursor(void)
/****************************************************************************
*
* Function:     syncCursor
*
* Description:  Sync up the current cursor location and the BIOS cursor
*				position in case we have intermixed normal console IO
*				with out high speed IO routines.
*
****************************************************************************/
{
	_cnt_x = PM_getByte(_biosSel, 0x50 + _activePage*2) - _win_left;
	_cnt_y = PM_getByte(_biosSel, 0x51 + _activePage*2) - _win_top;
}

void _PUBAPI CON_puts(char *str)
/****************************************************************************
*
* Function:     CON_puts
* Parameters:   str - String to output
*
* Description:  Outputs a string to the video display. Formatting info
*               is handled correctly. The following formatting
*               characters are handled:
*
*                   \f  Clear window and home cursor
*                   \n  Advance a line, scrolling window if necessary
*                   \r  Move to left edge of window
*                   \b  Move one character left (non-destructive)
*
*               The screen will scroll up if you go past the bottom line
*               of the window. Characters that go past the end of the
*               current line wrap depending on the setting of wrapmode. If
*               wrapmode is true (default), characters wrap to the beginning
*               of the next line, otherwise they wrap to the beginning of
*               the same line.
*
*               To speed up the output of this function, we build a
*               temporary buffer to store each contiguous section of the
*               string to be output as a single entity.
*
****************************************************************************/
{
    static char buf[256];
    char        *p;
    int         len;

	/* Sync up with current BIOS cursor location */
	syncCursor();

	len = *(p = buf) = 0;
    while (*str) {
        switch (*str) {
            case '\f':
                CON_clrscr();
                len = *(p = buf) = 0;       /* Discard buffer contents  */
                break;
            case '\n':
                outputBuf(buf,len);
				_cnt_x += len;
                len = *(p = buf) = 0;
				if (++_cnt_y >= _win_height) {
					_cnt_y = _win_height-1;
					CON_scroll(CON_SCROLL_UP,1);
                    }
				CON_gotoxy(_cnt_x,_cnt_y);
                break;
            case '\r':
                outputBuf(buf,len);
                len = *(p = buf) = 0;
				_cnt_x = 0;
				CON_gotoxy(_cnt_x,_cnt_y);
                break;
            case '\b':
				if (_cnt_x + --len < 0)
                    len = 0;
                else
                    --p;
                break;
            default:
                *p++ = *str;
                len++;
				if (_cnt_x + len >= _win_width) {
                    outputBuf(buf,len);
                    len = *(p = buf) = 0;
					_cnt_x = 0;
					if (_lineWrap) {
						if (++_cnt_y >= _win_height) {
							_cnt_y = _win_height-1;
							CON_scroll(CON_SCROLL_UP,1);
                            }
                        }
					CON_gotoxy(_cnt_x,_cnt_y);
                    }
                break;
            }
        str++;
        }
    outputBuf(buf,len);
	_cnt_x += len;
	CON_gotoxy(_cnt_x,_cnt_y);
}

void _PUBAPI CON_putc(int c)
/****************************************************************************
*
* Function:     CON_putc
* Parameters:   c   - Character to output
*
* Description:  Displays a single character on the console. Formatting
*               information is handled correctly. The following formatting
*               characters are handled:
*
*                   \f  Clear window and home cursor
*                   \n  Advance a line, scrolling window if necessary
*                   \r  Move to left edge of window
*                   \b  Move one character left (non-destructive)
*
*               The screen will scroll up if you go past the bottom line
*               of the window. Characters that go past the end of the
*               current line wrap depending on the setting of wrapmode. If
*               wrapmode is true (default), characters wrap to the beginning
*               of the next line, otherwise they wrap to the beginning of
*               the same line.
*
****************************************************************************/
{
	/* Sync up with current BIOS cursor location */
	syncCursor();

	switch (c) {
        case 0:
            break;                      /* Ignore ASCII NULL's          */
        case '\f':
            CON_clrscr();
            break;
        case '\n':
			if (++_cnt_y >= _win_height) {
				_cnt_y = _win_height-1;
				CON_scroll(CON_SCROLL_UP,1);
                }
            break;
        case '\r':
			_cnt_x = 0;
            break;
        case '\b':
			if (--_cnt_x < 0)
				_cnt_x = 0;
            break;
        default:
			writeChar(_cnt_x + _win_left,_cnt_y + _win_top,c,_attr);
			if (++_cnt_x >= _win_width) {
				_cnt_x = 0;
				if (_lineWrap) {
					if (++_cnt_y >= _win_height) {
						_cnt_y = _win_height-1;
						CON_scroll(CON_SCROLL_UP,1);
                        }
					}
				}
			break;
		}
	CON_gotoxy(_cnt_x,_cnt_y);
}

void _PUBAPI CON_clreol(void)
/****************************************************************************
*
* Function:     CON_clreol
*
* Description:  Clears from the current position to the end of the line.
*
****************************************************************************/
{
	syncCursor();
	CON_fillText(_cnt_x,_cnt_y,_win_width-1,_cnt_y,_backAttr,_backChar);
}

void _PUBAPI CON_clrscr(void)
/****************************************************************************
*
* Function:     CON_clrscr
*
* Description:  Clears the window and moves the cursor to the top left
*               hand corner of the window (0,0). The window is cleared with
*               the current background character and in the current
*               background attribute.
*
****************************************************************************/
{
	syncCursor();
	CON_fillText(0,0,_win_width-1,_win_height-1,_backAttr,_backChar);
	_cnt_x = _cnt_y = 0;
    CON_gotoxy(0,0);
}

void _PUBAPI CON_delline(void)
/****************************************************************************
*
* Function:     CON_delline
*
* Description:  Deletes the current line and moves all text below it up
*               one line.
*
****************************************************************************/
{
	int     top;

	syncCursor();
	top = _win_top;
	_win_top += _cnt_y;
	CON_scroll(CON_SCROLL_UP,1);
	_win_top = top;
}

void _PUBAPI CON_insline(void)
/****************************************************************************
*
* Function:     CON_insline
*
* Description:  Inserts a new lines at the current cursor position. Lines
*               below the line are moved down, and the bottom line is
*               lost.
*
****************************************************************************/
{
	int     top;

	syncCursor();
	top = _win_top;
	_win_top += _cnt_y;
	CON_scroll(CON_SCROLL_DOWN,1);
	_win_top = top;
}

void _PUBAPI CON_write(int x,int y,int attr,char *str)
/****************************************************************************
*
* Function:     CON_write
* Parameters:   x,y     - Position to write string at
*               _attr    - Attribute to write string in.
*               str     - String to output
*
* Description:  Outputs a string to the video display. Formatting info
*               is NOT handled. The string is clipped at the window
*               boundary. This function is a LOT faster than CON_printf()
*               and CON_puts() since formatting information is not handled.
*               The cursor is not moved.
*
****************************************************************************/
{
	int     len;

	if (y < 0 || y >= _win_height || x < 0)
		return;
	len = strlen(str);
	if (x + len - 1 >= _win_width)
		len = _win_width - x;
	if (len <= 0)
		return;
	writeStr(x + _win_left,y + _win_top,attr,str,len);
}

void _PUBAPI CON_writec(int x,int y,int attr,int c)
/****************************************************************************
*
* Function:     CON_writec
* Parameters:   x,y     - Postion to write character at
*               _attr    - Attribute to write character in
*               c       - Character to output
*
* Description:  Displays a single character on the console. Formatting
*               info is NOT handled, and the character is clipped at the
*               window boundary.
*
****************************************************************************/
{
	if (y < 0 || y >= _win_height || x < 0)
        return;
	if (x >= _win_width)
        return;
	writeChar(x + _win_left,y + _win_top,c,attr);
}

void _PUBAPI CON_moveText(int left,int top,int right,int bottom,int destleft,
    int desttop)
/****************************************************************************
*
* Function:     CON_moveText
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*               destleft    - Destination region left edge
*               desttop     - Destination region top edge
*
* Description:  Moves a block of video memory to another location.
*
****************************************************************************/
{
	moveText(_win_left + left,_win_top + top,_win_left + right,
		_win_top + bottom,_win_left + destleft,_win_top + desttop);
}

void _PUBAPI CON_saveText(int left,int top,int right,int bottom,void *dest)
/****************************************************************************
*
* Function:     CON_saveText
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*               destin      - Buffer to hold data
*
* Description:  Moves a block of text from the video display to a buffer
*               in RAM.
*
****************************************************************************/
{
	moveFromScreen(_win_left + left,_win_top + top,_win_left + right,
		_win_top + bottom,(char *)dest);
}

void _PUBAPI CON_restoreText(int left,int top,int right,int bottom,
	void *source)
/****************************************************************************
*
* Function:     CON_restoreText
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*               source      - Buffer holding data to use
*
* Description:  Moves a block of text from a buffer in RAM to the video
*               display.
*
****************************************************************************/
{
	moveToScreen(_win_left + left,_win_top + top,_win_left + right,
		_win_top + bottom,(char *)source);
}

void _PUBAPI CON_fillText(int left,int top,int right,int bottom,int attr,
	int ch)
/****************************************************************************
*
* Function:     CON_fillText
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*               _attr        - Attribute to fill with
*               ch          - Character to fill with
*
* Description:  Fills a region of the screen with the specified character
*               and attribute by calling the video BIOS.
*
****************************************************************************/
{
	fillText(_win_left + left,_win_top + top,_win_left + right,
		_win_top + bottom,attr,ch);
}

void _PUBAPI CON_fillAttr(int left,int top,int right,int bottom,int attr)
/****************************************************************************
*
* Function:     CON_fillAttr
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*               _attr        - Attribute to fill with
*
* Description:  Changes the attribute value for a block of text on the
*               screen.
*
****************************************************************************/
{
	fillAttr(_win_left + left,_win_top + top,_win_left + right,
		_win_top + bottom,attr);
}

void _PUBAPI CON_scroll(int direction,int amt)
/****************************************************************************
*
* Function:     CON_scroll
* Parameters:   direction   - Direction to scroll area in
*               amt         - Amount to scroll area by
*
* Description:  Scrolls the current window in the indicated direction by
*               the indicated amount, filling in the exposed area with
*               the current background character and attribute.
*
*               NOTE: Scrolling with the video BIOS routines will scroll
*               text in the current visual page, not the page output is
*               being sent to, so should only be used to scroll areas of
*               text currently visible.
*
****************************************************************************/
{
	switch (direction) {
		case CON_SCROLL_UP:
			if (amt <= 0 || amt >= _win_height) {
				CON_clrscr();
				return;
				}
			moveText(_win_left,_win_top + amt,_win_right,
				_win_bottom,_win_left,_win_top);
			fillText(_win_left,_win_bottom - (amt-1),_win_right,
				_win_bottom,_backAttr,_backChar);
			break;
		case CON_SCROLL_DOWN:
			if (amt <= 0 || amt >= _win_height) {
				CON_clrscr();
				return;
				}
			moveText(_win_left,_win_top,_win_right,_win_bottom - amt,
				_win_left,_win_top + amt);
			fillText(_win_left,_win_top,_win_right,_win_top+(amt-1),
				_backAttr,_backChar);
			break;
		case CON_SCROLL_RIGHT:
			if (amt <= 0 || amt > _win_width) {
				CON_clrscr();
				return;
				}
			moveText(_win_left,_win_top,_win_right - amt,_win_bottom,
				_win_left + amt,_win_top);
			fillText(_win_left,_win_top,_win_left + (amt-1),_win_bottom,
				_backAttr,_backChar);
			break;
		case CON_SCROLL_LEFT:
			if (amt <= 0 || amt > _win_width) {
				CON_clrscr();
				return;
				}
			moveText(_win_left + amt,_win_top,_win_right,_win_bottom,
				_win_left,_win_top);
			fillText(_win_right-(amt-1),_win_top,_win_right,_win_bottom,
				_backAttr,_backChar);
			break;
		}
}

void _PUBAPI CON_setWindow(int left,int top,int right,int bottom)
/****************************************************************************
*
* Function:     CON_setWindow
* Parameters:   left        - Left edge of screen region
*               top         - Top edge of screen region
*               right       - Right edge of screen region
*               bottom      - Bottom edge of screen region
*
* Description:  Sets the current window, and moves the cursor to the top
*               left hand corner.
*
****************************************************************************/
{
	_win_left = MAX(left,0);
	_win_right = MIN(right,_screenWidth-1);
	_win_top = MAX(top,0);
	_win_bottom = MIN(bottom,_screenHeight-1);
	_win_width = _win_right - _win_left + 1;
	_win_height = _win_bottom - _win_top + 1;
	CON_gotoxy(0,0);
}

void _PUBAPI CON_getWindow(int *left,int *top,int *right,int *bottom)
/****************************************************************************
*
* Function:     CON_getWindow
* Parameters:   left        - Place to store left edge of screen region
*               top         - Place to store top edge of screen region
*               right       - Place to store right edge of screen region
*               bottom      - Place to store bottom edge of screen region
*
* Description:  Returns the dimensions of the current window
*
****************************************************************************/
{
	*left = _win_left;
	*top = _win_top;
	*right = _win_right;
	*bottom = _win_bottom;
}

int _PUBAPI CON_maxx(void)
/****************************************************************************
*
* Function:     CON_maxx
* Returns:      Maximum x coordinate for current window
*
****************************************************************************/
{
	return _win_width - 1;
}

int _PUBAPI CON_maxy(void)
/****************************************************************************
*
* Function:     CON_maxy
* Returns:      Maximum y coordinate for current window
*
****************************************************************************/
{
	return _win_height - 1;
}

void _PUBAPI CON_setBackground(int attr,int ch)
/****************************************************************************
*
* Function:     CON_setBackground
* Parameters:   _attr    - New background attribute
*               ch      - New background character
*
* Description:  Sets the current background character and attribute.
*
****************************************************************************/
{
	_backAttr = attr;
	_backChar = ch;
}

void _PUBAPI CON_getBackground(int *attr,int *ch)
/****************************************************************************
*
* Function:     CON_getBackground
* Parameters:   _attr    - Place to store background attribute
*               ch      - Place to store background character
*
* Description:  Returns the current background character and attribute.
*
****************************************************************************/
{
	*attr = _backAttr;
	*ch = _backChar;
}

void _PUBAPI CON_setAttr(int attr)
/****************************************************************************
*
* Function:     CON_setAttr
* Parameters:   _attr    - New foreground attribute
*
* Description:  Sets the current foreground attribute.
*
****************************************************************************/
{
	_attr = attr;
}

int _PUBAPI CON_getAttr(void)
{
	return _attr;
}

void _PUBAPI CON_setBackColor(int newcolor)
/****************************************************************************
*
* Function:     CON_setBackColor
* Parameters:   newcolor    - New background color
*
* Description:  Sets the background color for the current foreground
*               text attribute.
*
****************************************************************************/
{
	_attr = _attr & 0x0F | ((newcolor & 0x0F) << 4);
}

void _PUBAPI CON_setForeColor(int newcolor)
/****************************************************************************
*
* Function:     CON_setForeColor
* Parameters:   newcolor    - New foreground color
*
* Description:  Sets the foreground color for the current foreground
*               text attribute.
*
****************************************************************************/
{
	_attr = _attr & 0xF0 | (newcolor & 0x0F);
}

void _PUBAPI CON_gotoxy(int x,int y)
/****************************************************************************
*
* Function:     CON_gotoxy
* Parameters:   x,y - New cursor position
*
* Description:  Moves the cursor location to (x,y) by calling the BIOS. If
*               the location lies outside of the current window, the
*               cursor is hidden by moving it off the entire screen.
*
****************************************************************************/
{
	PMREGS  regs;

	if (x >= _win_width || y >= _win_height) {
		x = _screenWidth;
		y = _screenHeight;
		}
	regs.h.dl = _win_left + x;
	regs.h.dh = _win_top + y;
	regs.h.bh = _activePage;
	regs.h.ah = 0x02;
	PM_int386(0x10,&regs,&regs);
	_cnt_x = x;
	_cnt_y = y;
}

int _PUBAPI CON_wherex(void)
/****************************************************************************
*
* Function:     CON_wherex
* Returns:      Current cursor x coordinate
*
****************************************************************************/
{
	syncCursor();
	return _cnt_x;
}

int _PUBAPI CON_wherey(void)
/****************************************************************************
*
* Function:     CON_wherey
* Returns:      Current cursor y coordinate
*
****************************************************************************/
{
	syncCursor();
	return _cnt_y;
}

int CON_screenWidth(void)
/****************************************************************************
*
* Function:     CON_screenWidth
* Returns:      Current full screen width
*
****************************************************************************/
{
	return _screenWidth;
}

int CON_screenHeight(void)
/****************************************************************************
*
* Function:     CON_screenHeight
* Returns:      Current full screen height
*
****************************************************************************/
{
	return _screenHeight;
}

void _PUBAPI CON_setCursor(int type)
/****************************************************************************
*
* Function:     CON_setCursor
* Parameters:   type
*
* Description:  Sets the text mode cursor to type. Note that if the
*               video mode is monochrome text mode, we correctly handle
*               the case of an EGA adapter with a monochrome monitor
*               running in 43 line mode.
*
****************************************************************************/
{
	int	scans;

	switch (type) {
		case CON_CURSOR_NORMAL:
			if (CON_isMonoMode())
				scans = 0x0B0C;
			else scans = 0x0607;
			break;
		case CON_CURSOR_FULL:
			if (CON_isMonoMode())
				scans = 0x000E;
			else scans = 0x0007;
			break;
		}
	CON_restoreCursor(scans);
}

void _PUBAPI CON_cursorOff(void)
/****************************************************************************
*
* Function:     CON_cursorOff
*
* Description:  Hides the cursor.
*
****************************************************************************/
{
	PMREGS  regs;

	regs.h.ah = 0x01;
	regs.x.cx = 0x2000;
	PM_int386(0x10,&regs,&regs);
}

void _PUBAPI CON_restoreCursor(int scans)
/****************************************************************************
*
* Function:     CON_restoreCursor
* Parameters:   scans   - Cursor scan lines
*
* Description:  Restores a previously saved cursor value.
*
****************************************************************************/
{
	PMREGS  regs;

	regs.h.ah = 0x01;
	regs.x.cx = scans;
	PM_int386(0x10,&regs,&regs);
}

int _PUBAPI CON_getCursor(void)
/****************************************************************************
*
* Function:     CON_getCursor
* Returns:      Current cursor size
*
****************************************************************************/
{
	PMREGS  regs;

	regs.h.ah = 0x03;
	PM_int386(0x10,&regs,&regs);
	return regs.x.cx;
}

void _PUBAPI CON_setActivePage(int page)
/****************************************************************************
*
* Function:     CON_setActivePage
* Parameters:   page    - New page number to use
*
* Description:  Sets the currently active video page. The active video page
*               is the page all output is currently sent to, but it not
*               necessarily visible. Note that only one page is available
*               on monochrome systems.
*
****************************************************************************/
{
	int     offset;
	PMREGS  regs;

	if (CON_isMonoMode())
		return;
	_activePage = page;
	regs.h.ah = 0x05;
	regs.h.al = page;
	PM_int386(0x10,&regs,&regs);
	offset = PM_getWord(_biosSel, 0x4E);/* Get offset into video buffer */
	regs.h.ah = 0x05;
	regs.h.al = _visualPage;
	PM_int386(0x10,&regs,&regs);
	_screenPtr = _screenPtrBase + offset;
	_cnt_x = PM_getByte(_biosSel, 0x50 + page*2) - _win_left;
	_cnt_y = PM_getByte(_biosSel, 0x51 + page*2) - _win_top;
}

int _PUBAPI CON_getActivePage(void)
/****************************************************************************
*
* Function:     CON_getActivePage
* Returns:      Currently active video page
*
****************************************************************************/
{
	return _activePage;
}

void _PUBAPI CON_setVisualPage(int page)
/****************************************************************************
*
* Function:     CON_setVisualPage
* Parameters:   page    - New page number to use
*
* Description:  Sets the currently visible video page to 'page'. This page
*               is the one containing currenlty visible data, but may not
*               be the one currently active for writing. Note that only
*               one page is available on monochrome systems.
*
****************************************************************************/
{
	PMREGS  regs;

	if (CON_isMonoMode())
		return;
	_visualPage = page;
	regs.h.ah = 0x05;
	regs.h.al = page;
	PM_int386(0x10,&regs,&regs);
}

int _PUBAPI CON_getVisualPage(void)
/****************************************************************************
*
* Function:     CON_getVisualPage
* Returns:      Currently visible video page.
*
****************************************************************************/
{
	return _visualPage;
}

bool CON_isMonoMode(void)
{
	return (PM_getByte(_biosSel, 0x49) == 0x7);
}
