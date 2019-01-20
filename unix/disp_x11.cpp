//******************************************************************************
///
/// @file unix/disp_x11.cpp
///
/// X11 based render display system
///
///
/// @author Jerome Grimbert <jgrimbert@free.fr>
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//*******************************************************************************

#include "config.h"

#ifndef X_DISPLAY_MISSING

#include "disp_x11.h"

#include <algorithm>

// handle I/O errors via setjmp / longjmp
#include <setjmp.h>

#ifdef HAVE_LIBXPM
#include <X11/xpm.h>
#include "xpovicon.xpm" /* This is the color POV icon */
#else
#include "xpovicon.xbm" /* This is the black & white POV icon */
#include "xpovmask.xbm" /* This is the transparency mask for the icon */
#endif


// this must be the last file included
#include "syspovdebug.h"

static ::Display * theDisplay;// to be messed by the error handling routine
static boost::mutex        x11Mutex;

static int xioErrorHandler( ::Display * disp )
{
    fprintf(stderr, "\nX I/O error\n");
    if (theDisplay)
    {
    XFlush(theDisplay);
    }
    theDisplay = nullptr;
    return true;
}
static int xErrorHandler( ::Display * disp, XErrorEvent *errorEvent)
{
    char message[130];
    XGetErrorText(disp, errorEvent->error_code, message, 125);
    fprintf(stderr, "\nX error: %s\n", message);
    return true;
}

namespace pov_frontend
{
    using namespace vfe;
    using namespace vfePlatform;
    static char theCLASS[] =     "Povray" ;     /* Should begin with a capital */
    static char theNAME[] =      "povray" ;     /* for the property */
    static char theICONNAME[] =  "POV-Ray";     /* short name for the icon */


    extern shared_ptr<Display> gDisplay;
    const UnixOptionsProcessor::Option_Info UnixX11Display::Options[] =
    {
        // command line/povray.conf/environment options of this display mode can be added here
        // section name, option name, default, has_param, command line parameter, environment variable name, help text
        // 
        // TODO option to get a window-id to use instead of default desktop
        UnixOptionsProcessor::Option_Info("", "", "", false, "", "", "") // has to be last
    };

    bool UnixX11Display::Register(vfeUnixSession *session)
    {
        session->GetUnixOptions()->Register(Options);
        return true;
    }

    UnixX11Display::UnixX11Display(unsigned int w, unsigned int h, vfeSession *session, bool visible) :
        UnixDisplay(w, h, session, visible)
    {
        m_valid = false;
        m_display_scaled = false;
        m_display_scale = 1.;
        //theDisplay = nullptr;
        theImage = nullptr;
        theWindow = 0;
    }

    UnixX11Display::~UnixX11Display()
    {
        Close();
    }

    void UnixX11Display::Initialise()
    {
        if (m_VisibleOnCreation)
            Show();
    }

    void UnixX11Display::Hide()
    {
        // not used
        if (m_valid)
        {
        }
    }

    void UnixX11Display::Clear()
    {
        // not used
        if (m_valid)
        {
        }
    }

    bool UnixX11Display::TakeOver(UnixDisplay *display)
    {
        UnixX11Display *p = dynamic_cast<UnixX11Display *>(display);
        if (p == nullptr)
            return false;
        if ((GetWidth() != p->GetWidth()) || (GetHeight() != p->GetHeight()))
            return false;

        *this = *p;
        // protect against Close(), as the resources are transfered and not copied
        p->m_display_scaled=false;
        p->m_display_scale=1;
        // p->theDisplay = nullptr;
        p->theImage = nullptr;
        p->theWindow = 0;

        if (m_display_scaled)
        {
            int width = GetWidth();
            int height = GetHeight();
            // allocate a new pixel counters, dropping influence of previous picture
            m_PxCount.clear(); // not useful, vector was created empty, just to be sure
            m_PxCount.reserve(width*height); // we need that, and the loop!
            for(vector<unsigned char>::iterator iter = m_PxCount.begin(); iter != m_PxCount.end(); iter++)
                (*iter) = 0;
        }
        if (theImage->data)
        {
            uint_least8_t *p =(uint_least8_t*)theImage->data;
            uint_least64_t c = theImage->bytes_per_line*theImage->height;
            while(c)
            {
                // darken previous image
                *p++ >>= 2;
                --c;
            }

            XPutImage(theDisplay, theWindow, theGC, theImage, 0, 0, 0, 0,
                        theImage->width, theImage->height);
            XFlush(theDisplay);
        }

        return true;
    }

    void UnixX11Display::Close()
    {
        if (!m_valid)
            return;

        XFlush(theDisplay);
        m_PxCount.clear();
        m_valid = false;

        if (theImage)
        {
            free(theImage->data);
            theImage->data = NULL;
            XDestroyImage(theImage);
            theImage = nullptr;
        }
        if (theWindow)
        {
            XDestroyWindow(theDisplay, theWindow);
#ifdef USE_CURSOR
            XFreeCursor(theDisplay, theCursor);
            // [JG] well known leakage: even with XFreeCursor, a leak is reported when
            // the cursor is used. It is once (not a real leakage), but it is not important here
            // as we only have one cursor (it would be the same reported leak if we created
            // and deleted many windows, each with its own cursor)
#endif

            XFreeColormap(theDisplay, theColormap);
            XFreeGC(theDisplay, theGC);
            XCloseDisplay( theDisplay );
            theDisplay = nullptr;
        }

    }

    void UnixX11Display::SetCaption(bool paused)
    {
        if (!m_valid)
            return;

        boost::format f;
        if (m_display_scaled)
            f = boost::format(PACKAGE_NAME " " VERSION_BASE " X11 (scaled at %.0f%%) %s")
                % (m_display_scale*100)
                % (paused ? " [paused]" : "");
        else
            f = boost::format(PACKAGE_NAME " " VERSION_BASE " X11 %s")
                % (paused ? " [paused]" : "");

        {
            boost::mutex::scoped_lock lock(x11Mutex);
            XStoreName(theDisplay, theWindow, f.str().c_str());
        }
    }

    void UnixX11Display::Show()
    {
        if (gDisplay.get() != this)
            gDisplay = m_Session->GetDisplay();

        if (!m_valid)
        {
            XSetErrorHandler(xErrorHandler);

            XSetIOErrorHandler( xioErrorHandler );

            theDisplay = XOpenDisplay( NULL );
            Visual *theVisual;
            unsigned long        theWindowMask;

            int theScreen = DefaultScreen(theDisplay);
            int width = GetWidth();
            int height = GetHeight();
            int theDispWidth  = DisplayWidth(theDisplay, theScreen);
            int theDispHeight = DisplayHeight(theDisplay, theScreen);

            vfeUnixSession *UxSession = dynamic_cast<vfeUnixSession *>(m_Session);
            // determine desktop area
            // always scale when window is too big to fit
            // tolerance for border, just hope the Window Manager is not larger than 10
            width = min(theDispWidth - 10, width);
            // tolerance for border and title bar, just hope the Window Manager is not larger than 80
            height = min(theDispHeight - 80, height);
            // calculate display area
            float AspectRatio = float(width)/float(height);
            float AspectRatio_Full = float(GetWidth())/float(GetHeight());
            if (AspectRatio > AspectRatio_Full)
            {
                width = int(AspectRatio_Full*float(height));
            }
            else if (AspectRatio != AspectRatio_Full)
            {
                height = int(float(width)/AspectRatio_Full);
            }
            //
            theVisual = DefaultVisual(theDisplay, theScreen);
            XVisualInfo templatev;
            templatev.visualid = theVisual->visualid;
            int nItems;
            XVisualInfo * theVisualInfo = XGetVisualInfo(theDisplay, VisualIDMask, &templatev, &nItems);
            int theDepth = theVisualInfo->depth;
            XFree(theVisualInfo);
            // assume true color
            theColormap = XCreateColormap(theDisplay, RootWindow(theDisplay, theScreen), theVisual, AllocNone);

            XSetWindowAttributes theWindowAttributes;

            theWindowAttributes.backing_store    = WhenMapped;
            theWindowAttributes.background_pixel = WhitePixel(theDisplay, theScreen);
            theWindowAttributes.border_pixel     = BlackPixel(theDisplay, theScreen);
            theWindowAttributes.event_mask       = NoEventMask;
            theWindowAttributes.colormap         = theColormap;
            theWindowMask = CWBackingStore | CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
#ifdef USE_CURSOR
            if ((theCursor = XCreateFontCursor(theDisplay, XC_watch)) != (Cursor)None)
            {
                theWindowAttributes.cursor = theCursor;
                theWindowMask |= CWCursor;
            }
#endif
            theWindow = XCreateWindow(theDisplay, RootWindow(theDisplay, theScreen), 0, 0,
                    width, height, 1, CopyFromParent , InputOutput,
                    CopyFromParent, theWindowMask, &theWindowAttributes);

            //
            XSizeHints theSizeHints;
            //
            theSizeHints.base_width   =
                theSizeHints.width        =                     /* Obsolete */
                theSizeHints.min_width    =
                theSizeHints.max_width    =
                theSizeHints.min_aspect.x =
                theSizeHints.max_aspect.x = width;
            theSizeHints.base_height  =
                theSizeHints.height       =                     /* Obsolete */
                theSizeHints.min_height   =
                theSizeHints.max_height   =
                theSizeHints.min_aspect.y =
                theSizeHints.max_aspect.y = height;
            theSizeHints.flags = PSize | PMinSize | PMaxSize | PAspect;

            theSizeHints.flags |= PBaseSize;

            XSetWMNormalHints(theDisplay, theWindow, &theSizeHints);


            //
            XClassHint theClassHints;
            //
            /*
             * This is the actual name of the application to the window manager,
             * for purposes of defaults and such.  Not to be confused with the
             * name on the title bar or the icon, which is just fluff.
             */

            theClassHints.res_name  = theNAME ;
            theClassHints.res_class = theCLASS;

            XSetClassHint(theDisplay, theWindow, &theClassHints);

            /*
             * make an icon to attatch to the window
             */
            Pixmap               theIcon;
            Pixmap               theMask;
#ifdef HAVE_LIBXPM
            XpmAttributes         xpm_attributes;
            int                   xpm_status;


            for(int i = 20000; i < 65535; i += 20000)  // try several closeness values
            {
                if(i > 65535)
                    i = 65535;

                xpm_attributes.colormap  = theColormap;
                xpm_attributes.closeness = i;
                xpm_attributes.valuemask = XpmColormap | XpmCloseness;

                xpm_status = XpmCreatePixmapFromData(
                        theDisplay, theWindow,
                        xpovicon,
                        &theIcon, &theMask,
                        &xpm_attributes
                        );
                if(xpm_status == XpmSuccess)
                    break;
            }
#else

            theIcon = XCreateBitmapFromData(theDisplay, theWindow, (char *)xpovicon_bits,
                    xpovicon_width, xpovicon_height);

            theMask = XCreateBitmapFromData(theDisplay, theWindow, (char *)xpovmask_bits,
                    xpovmask_width, xpovmask_height);
#endif  /* HAVE_LIBXPM */

            /*
             * tell the window manager what to do with the icon
             */
            XWMHints             theWMHints;
            theWMHints.icon_pixmap   = theIcon;
            theWMHints.input         = True;
            theWMHints.initial_state = NormalState;
            theWMHints.flags         = IconPixmapHint|InputHint|StateHint|IconMaskHint;
            theWMHints.icon_mask     = theMask;

            XSetWMHints(theDisplay, theWindow, &theWMHints);

            XSetIconName(theDisplay, theWindow, theICONNAME);

            /*
             * create a graphics context for drawing
             */
            XGCValues            theGCValues;

            theGCValues.function = GXcopy;

            theGC = XCreateGC(theDisplay, theWindow, GCFunction, &theGCValues);

            /*
             * Now that we are finished setting everything up, we will begin
             * handling input for this window.
             */

            XSelectInput(theDisplay, theWindow,
                (
                 ButtonPressMask | 
                 KeyPressMask | 
                 StructureNotifyMask));

            /*
             * Now, could we please see the window on the screen?  Until now, we have
             * dealt with a window which has been created but has not appeared on the
             * screen.  Mapping the window places it visibly on the screen.
             */

            XMapWindow(theDisplay, theWindow);
            /*
             * Allocate and create XImage to save the image if it gets covered
             */
            theImage = XCreateImage(theDisplay, theVisual, theDepth, ZPixmap, 0, NULL,
                    width, height, BitmapPad(theDisplay), 0);
            size_t size = theImage->bytes_per_line * height;

            theImage->data = (char *)calloc(1, size);
            XInitImage( theImage );
            // get the shift for each component
            rs=0;
            gs=0;
            bs=0;
            unsigned long m = theImage->red_mask;
            while(!(m&1))
            {
              ++rs;
              m>>=1;
            }
            m = theImage->green_mask;
            while(!(m&1))
            {
              ++gs;
              m>>=1;
            }
            m = theImage->blue_mask;
            while(!(m&1))
            {
              ++bs;
              m>>=1;
            }
            // set initial pattern
            unsigned long c;
            c = (1ULL<<theDepth)-1;
            for(int i=0;i<height;++i)
            {
              for(int j = 0;j<width;++j)
              {
                if ((i^j)&4)
                {
                  XPutPixel( theImage, j,i, c);
                }
              }
            }

            XPutImage(theDisplay, theWindow, theGC, theImage, 0, 0, 0, 0, width, height);
            XFlush(theDisplay);

            m_PxCount.clear();
            m_PxCount.reserve(width*height);
            for(vector<unsigned char>::iterator iter = m_PxCount.begin(); iter != m_PxCount.end(); iter++)
                (*iter) = 0;

            m_valid = true;

            if ((width == GetWidth()) && (height == GetHeight()))
            {
                m_display_scaled = false;
                m_display_scale = 1.;
            }
            else
            {
                m_display_scaled = true;
                /* [JG] the scaling factor between the requested resolution and the actual window is the same in both direction
                 * yet, the factor (as a float) need the smallest value to avoid an access out of the two buffers for the pixels.
                 * The difference is nearly invisible until the values of GetWidth and GetHeight are subtil (such as +W2596 +H1003 on a display of 1920 x 1080)
                 * where in such situation, the computed ratio is not exactly the same as the other.
                 *
                 * other inherent problem: m_display_scale is <= 1.0, which might be an issue
                 * to represent all fraction in binary float (rounding error):
                 * 1/2 is ok
                 * 1/3 has an error
                 * 1/4 is fine
                 * 1/5 has an error
                 * ... and so on
                 * TODO: change for integer and use / instead of * in formula ?
                 */
                m_display_scale = min(float(width) / GetWidth(), float(height) / GetHeight());
            }


            SetCaption(false);
        }
        else
        {
            {
                boost::mutex::scoped_lock lock(x11Mutex);
                XPutImage( theDisplay, theWindow, theGC, theImage, 0, 0, 0, 0, theImage->width, theImage->height);
            }
            XFlush(theDisplay);
        }
    }

    void UnixX11Display::SetPixel(unsigned int x, unsigned int y, const RGBA8& colour)
    {
        uint_least32_t r, g, b;
        r = colour.red  ;
        g = colour.green;
        b = colour.blue ;

        uint_least32_t col = (r<<rs)|(g<<gs)|(b<<bs);
        XPutPixel(theImage,x,y, col);
    }

    void UnixX11Display::SetPixelScaled(unsigned int x, unsigned int y, const RGBA8& colour)
    {
        unsigned int ix = x * m_display_scale;
        unsigned int iy = y * m_display_scale;

        uint_least32_t r, g, b;
        uint_least32_t old = XGetPixel(theImage,ix,iy);
        r = old >>rs;
        r &= 0x0FF;
        g = old >>gs;
        g &= 0x0FF;
        b = old >>bs;
        b &= 0x0FF;

        unsigned int ofs = ix + iy * theImage->width;
        r = (r*m_PxCount[ofs] + colour.red  ) / (m_PxCount[ofs]+1);
        g = (g*m_PxCount[ofs] + colour.green) / (m_PxCount[ofs]+1);
        b = (b*m_PxCount[ofs] + colour.blue ) / (m_PxCount[ofs]+1);

        uint_least32_t col = (r<<rs)|(g<<gs)|(b<<bs);
        XPutPixel(theImage,ix,iy, col);
        ++m_PxCount[ofs];
    }

    void UnixX11Display::DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
    {
        if (!m_valid || x >= GetWidth() || y >= GetHeight())
            return;

        if (m_display_scaled)
        {
            SetPixelScaled(x, y, colour);
            {
                boost::mutex::scoped_lock lock(x11Mutex);
                XPutImage( theDisplay, theWindow, theGC, theImage, x*m_display_scale, y*m_display_scale, x*m_display_scale, y*m_display_scale, 1, 1 );
            }
        }
        else
        {
            SetPixel(x, y, colour);
            {
                boost::mutex::scoped_lock lock(x11Mutex);
                XPutImage( theDisplay, theWindow, theGC, theImage, x, y, x, y, 1, 1 );
            }
        }

    }

    void UnixX11Display::DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
    {
        if (!m_valid)
            return;

        int ix1 = min(x1, GetWidth()-1);
        int ix2 = min(x2, GetWidth()-1);
        int iy1 = min(y1, GetHeight()-1);
        int iy2 = min(y2, GetHeight()-1);

        if (m_display_scaled)
        {
            for(unsigned int x = ix1; x <= ix2; x++)
            {
                SetPixelScaled(x, iy1, colour);
                SetPixelScaled(x, iy2, colour);
            }

            for(unsigned int y = iy1; y <= iy2; y++)
            {
                SetPixelScaled(ix1, y, colour);
                SetPixelScaled(ix2, y, colour);
            }
            {
                boost::mutex::scoped_lock lock(x11Mutex);
                XPutImage( theDisplay, theWindow, theGC, theImage, ix1*m_display_scale, iy1*m_display_scale, ix1*m_display_scale, iy1*m_display_scale, (uint_least64_t(ix2*m_display_scale)-uint_least64_t(ix1*m_display_scale)+1), (uint_least64_t(iy2*m_display_scale)-uint_least64_t(iy1*m_display_scale)+1));
            }
        }
        else
        {
            for(unsigned int x = ix1; x <= ix2; x++)
            {
                SetPixel(x, iy1, colour);
                SetPixel(x, iy2, colour);
            }

            for(unsigned int y = iy1; y <= iy2; y++)
            {
                SetPixel(ix1, y, colour);
                SetPixel(ix2, y, colour);
            }
            {
                boost::mutex::scoped_lock lock(x11Mutex);
                XPutImage( theDisplay, theWindow, theGC, theImage, ix1, iy1, ix1, iy1, (ix2-ix1+1), (iy2-iy1+1));
            }
        }
    }

    void UnixX11Display::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
    {
        if (!m_valid)
            return;

        int ix1 = min(x1, GetWidth()-1);
        int ix2 = min(x2, GetWidth()-1);
        int iy1 = min(y1, GetHeight()-1);
        int iy2 = min(y2, GetHeight()-1);

        if (m_display_scaled)
        {
            for(unsigned int x = ix1; x <= ix2; x++)
            {
                for(unsigned int y = iy1; y <= iy2; y++)
                {
                    SetPixelScaled(x, y, colour);
                }
            }
            {
                boost::mutex::scoped_lock lock(x11Mutex);
                XPutImage( theDisplay, theWindow, theGC, theImage, ix1*m_display_scale, iy1*m_display_scale, ix1*m_display_scale, iy1*m_display_scale, (uint_least64_t(ix2*m_display_scale)-uint_least64_t(ix1*m_display_scale)+1), (uint_least64_t(iy2*m_display_scale)-uint_least64_t(iy1*m_display_scale)+1));
            }
        }
        else
        {
            for(unsigned int x = ix1; x <= ix2; x++)
            {
                for(unsigned int y = iy1; y <= iy2; y++)
                {
                    SetPixel(x, y, colour);
                }
            }

            {
                boost::mutex::scoped_lock lock(x11Mutex);
                XPutImage( theDisplay, theWindow, theGC, theImage, ix1, iy1, ix1, iy1, (ix2-ix1+1), (iy2-iy1+1));
            }
        }

    }

    void UnixX11Display::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
    {
        if (!m_valid)
            return;

        unsigned int ix1 = min(x1, GetWidth()-1);
        unsigned int ix2 = min(x2, GetWidth()-1);
        unsigned int iy1 = min(y1, GetHeight()-1);
        unsigned int iy2 = min(y2, GetHeight()-1);

        if (m_display_scaled)
        {
            for(unsigned int y = iy1, i = 0; y <= iy2; y++)
                for(unsigned int x = ix1; x <= ix2; x++, i++)
                    SetPixelScaled(x, y, colour[i]);
            {
                boost::mutex::scoped_lock lock(x11Mutex);
                XPutImage( theDisplay, theWindow, theGC, theImage, ix1*m_display_scale, iy1*m_display_scale, ix1*m_display_scale, iy1*m_display_scale, (uint_least64_t(ix2*m_display_scale)-uint_least64_t(ix1*m_display_scale)+1), (uint_least64_t(iy2*m_display_scale)-uint_least64_t(iy1*m_display_scale)+1));
            }
        }
        else
        {
            for(unsigned int y = y1, i = 0; y <= iy2; y++)
                for(unsigned int x = ix1; x <= ix2; x++, i++)
                    SetPixel(x, y, colour[i]);
            {
                boost::mutex::scoped_lock lock(x11Mutex);
                XPutImage( theDisplay, theWindow, theGC, theImage, ix1, iy1, ix1, iy1, (ix2-ix1+1), (iy2-iy1+1));
            }
        }

    }

    void UnixX11Display::UpdateScreen(bool Force = false)
    {
        if (!m_valid)
            return;

        if (Force )
        {
            boost::mutex::scoped_lock lock(x11Mutex);
            XFlush(theDisplay);
        }
    }

    void UnixX11Display::PauseWhenDoneNotifyStart()
    {
        if (!m_valid)
            return;
        fprintf(stderr, "Press p, q, enter or press button 1 over displayed image to continue...");
        SetCaption(true);
    }

    void UnixX11Display::PauseWhenDoneNotifyEnd()
    {
        if (!m_valid)
            return;
        SetCaption(false);
        fprintf(stderr, "\n\n");
    }

    bool UnixX11Display::PauseWhenDoneResumeIsRequested()
    {
        if (!m_valid)
            return true;

        XEvent event;
        bool do_quit = false;

        while (XCheckMaskEvent(theDisplay, (
                ButtonPressMask |
                KeyPressMask|
                StructureNotifyMask
                ), &event))
        {
            switch (event.type)
            {
                case KeyPress:
                    {
                        KeySym key = XLookupKeysym( &event.xkey, 0);
                        switch( key )
                        {
                            case XK_P:
                            case XK_Q:
                            case XK_p:
                            case XK_q:
                            case XK_Return:
                            case XK_KP_Enter:
                                do_quit = true;
                        }
                    }
                    break;
                case ButtonPress:
                    // button 1 was not pressed (state) and the event is about button1 : so it get pressed now
                    if ((!( event.xbutton.state & Button1Mask )) &&( event.xbutton.button == Button1))
                    {
                        do_quit = true;
                    }
                    break;
                case MapNotify:
                    {
                        Show();
                    }
                case DestroyNotify:
                    do_quit=true;
                    break;
            }
        }

        return do_quit;
    }

    bool UnixX11Display::HandleEvents()
    {
        if (!m_valid)
            return false;

        XEvent event;

        bool do_quit = false;

        // consum all requested events, including button press, even for doing nothing
        while (XCheckMaskEvent(theDisplay, 
              ButtonPressMask| 
              KeyPressMask|StructureNotifyMask, &event))
        {
            switch (event.type)
            {
                case KeyPress:
                    {
                        KeySym key = XLookupKeysym( &event.xkey, 0);
                        switch(key)
                        {
                            case XK_Q:
                            case XK_q:
                                do_quit = true;
                                break;
                            case XK_P:
                            case XK_p:
                                {
                                    if (!m_Session->IsPausable())
                                        break;

                                    if (m_Session->Paused())
                                    {
                                        if (m_Session->Resume())
                                            SetCaption(false);
                                    }
                                    else
                                    {
                                        if (m_Session->Pause())
                                            SetCaption(true);
                                    }
                                }
                                break;
                        }
                    }
                    break;
                case MapNotify:
                    {
                        Show();
                    }
                    break;
                case DestroyNotify:
                    do_quit=true;
                    break;
            }
            if (do_quit)
                break;
        }

        return do_quit;
    }
}

#endif /* X_DISPLAY_MISSING */
