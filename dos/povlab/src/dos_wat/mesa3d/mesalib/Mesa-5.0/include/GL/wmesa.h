/* $Id: wmesa.h,v 1.2 2002/04/23 18:23:32 kschultz Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.0
 * Copyright (C) 1995-1998  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


/*
 * $Log: wmesa.h,v $
 * Revision 1.2  2002/04/23 18:23:32  kschultz
 * Fix up alpha buffer handling for Windows.
 * - add two new Pixel Format Descriptors that do not have alpha bits to
 * mirror the two that do.
 * - add logic to wglChoosePixelFormat to match PFD's with respect to alpha.
 * - Create/clear software alpha buffer as required.
 * Now a wgl or GLUT program can control the creation of a software alpha
 * buffer via the PFD or GLUT parms, respectively.
 *
 * Revision 1.1.1.1  1999/08/19 00:55:40  jtg
 * Imported sources
 *
 * Revision 3.2  1999/01/03 02:54:45  brianp
 * updated per Ted Jump
 *
 * Revision 3.1  1998/12/01 02:34:27  brianp
 * applied Mark Kilgard's patches from November 30, 1998
 *
 * Revision 3.0  1998/02/20 05:06:59  brianp
 * initial rev
 *
 */


/*
 * Windows driver by: Mark E. Peterson (markp@ic.mankato.mn.us)
 * Updated by Li Wei (liwei@aiar.xjtu.edu.cn)
 *
 *
 ***************************************************************
 *                     WMesa                                   *
 *                     version 2.3                             *	
 *                                                             *
 *                        By                                   *
 *                      Li Wei                                 *
 *       Institute of Artificial Intelligence & Robotics       *
 *       Xi'an Jiaotong University                             *
 *       Email: liwei@aiar.xjtu.edu.cn                         * 
 *       Web page: http://sun.aiar.xjtu.edu.cn                 *
 *                                                             *
 *	       July 7th, 1997				       *
 ***************************************************************
 */


#ifndef WMESA_H
#define WMESA_H


#ifdef __cplusplus
extern "C" {
#endif


#include "gl\gl.h"

#pragma warning (disable:4273)
#pragma warning( disable : 4244 ) /* '=' : conversion from 'const double ' to 'float ', possible loss of data */
#pragma warning( disable : 4018 ) /* '<' : signed/unsigned mismatch */
#pragma warning( disable : 4305 ) /* '=' : truncation from 'const double ' to 'float ' */
#pragma warning( disable : 4013 ) /* 'function' undefined; assuming extern returning int */
#pragma warning( disable : 4761 ) /* integral size mismatch in argument; conversion supplied */
#pragma warning( disable : 4273 ) /* 'identifier' : inconsistent DLL linkage. dllexport assumed */
#if (MESA_WARNQUIET>1)
#	pragma warning( disable : 4146 ) /* unary minus operator applied to unsigned type, result still unsigned */
#endif

/*
 * This is the WMesa context 'handle':
 */
typedef struct wmesa_context *WMesaContext;



/*
 * Create a new WMesaContext for rendering into a window.  You must
 * have already created the window of correct visual type and with an
 * appropriate colormap.
 *
 * Input:
 *         hWnd - Window handle
 *         Pal  - Palette to use
 *         rgb_flag - GL_TRUE = RGB mode,
 *                    GL_FALSE = color index mode
 *         db_flag - GL_TRUE = double-buffered,
 *                   GL_FALSE = single buffered
 *         alpha_flag - GL_TRUE = create software alpha buffer,
 *                      GL_FALSE = no software alpha buffer
 *
 * Note: Indexed mode requires double buffering under Windows.
 *
 * Return:  a WMesa_context or NULL if error.
 */
extern WMesaContext WMesaCreateContext(HWND hWnd,HPALETTE* pPal,
                                       GLboolean rgb_flag,
                                       GLboolean db_flag,
                                       GLboolean alpha_flag);


/*
 * Destroy a rendering context as returned by WMesaCreateContext()
 */
/*extern void WMesaDestroyContext( WMesaContext ctx );*/
extern void WMesaDestroyContext( void );



/*
 * Make the specified context the current one.
 */
extern void WMesaMakeCurrent( WMesaContext ctx );


/*
 * Return a handle to the current context.
 */
extern WMesaContext WMesaGetCurrentContext( void );


/*
 * Swap the front and back buffers for the current context.  No action
 * taken if the context is not double buffered.
 */
extern void WMesaSwapBuffers(void);


/*
 * In indexed color mode we need to know when the palette changes.
 */
extern void WMesaPaletteChange(HPALETTE Pal);

extern void WMesaMove(void);



#ifdef __cplusplus
}
#endif


#endif

