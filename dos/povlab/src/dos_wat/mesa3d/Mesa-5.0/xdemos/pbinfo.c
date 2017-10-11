/* $Id: pbinfo.c,v 1.1 2002/10/05 18:30:13 brianp Exp $ */

/*
 * Print list of fbconfigs and test each to see if a pbuffer can be created
 * for that config.
 *
 * Brian Paul
 * April 1997
 * Updated on 5 October 2002.
 */


#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>
#include "pbutil.h"




static void
PrintConfigs(Display *dpy, int screen, Bool horizFormat)
{
   GLXFBConfigSGIX *fbConfigs;
   int nConfigs;
   int i;
   /* Note: you may want to tweek the attribute list to select a different
    * set of fbconfigs.
    */
   int fbAttribs[] = {
                      GLX_RENDER_TYPE_SGIX, 0,
		      GLX_DRAWABLE_TYPE_SGIX, 0,
#if 0
                      GLX_RENDER_TYPE_SGIX, GLX_RGBA_BIT_SGIX,
		      GLX_DRAWABLE_TYPE_SGIX, GLX_PIXMAP_BIT_SGIX,
		      GLX_RED_SIZE, 1,
		      GLX_GREEN_SIZE, 1,
		      GLX_BLUE_SIZE, 1,
		      GLX_DEPTH_SIZE, 1,
		      GLX_DOUBLEBUFFER, 0,
		      GLX_STENCIL_SIZE, 0,
#endif
		      None};


   /* Get list of possible frame buffer configurations */
#if 0
   /* SGIX method */
   fbConfigs = glXChooseFBConfigSGIX(dpy, screen, fbAttribs, &nConfigs);
#else
   /* GLX 1.3 method */
   fbConfigs = glXGetFBConfigs(dpy, screen, &nConfigs);
#endif

   if (nConfigs==0 || !fbConfigs) {
      printf("Error: glxChooseFBConfigSGIX failed\n");
      return;
   }

   printf("Number of fbconfigs: %d\n", nConfigs);

   if (horizFormat) {
      printf("  ID  VisualType  Depth Lvl RGB CI DB Stereo  R  G  B  A");
      printf("   Z  S  AR AG AB AA  MSbufs MSnum  Pbuffer\n");
   }

   /* Print config info */
   for (i=0;i<nConfigs;i++) {
      PrintFBConfigInfo(dpy, fbConfigs[i], horizFormat);
   }

   /* free the list */
   XFree(fbConfigs);
}



static void
PrintUsage(void)
{
   printf("Options:\n");
   printf("  -display <display-name>  specify X display name\n");
   printf("  -t                       print in tabular format\n");
   printf("  -v                       print in verbose format\n");
   printf("  -help                    print this information\n");
}


int
main(int argc, char *argv[])
{
   Display *dpy;
   int scrn;
   char *dpyName = NULL;
   Bool horizFormat = True;
   int i;

   for (i=1; i<argc; i++) {
      if (strcmp(argv[i],"-display")==0) {
	 if (i+1<argc) {
	    dpyName = argv[i+1];
	    i++;
	 }
      }
      else if (strcmp(argv[i],"-t")==0) {
	 /* tabular format */
	 horizFormat = True;
      }
      else if (strcmp(argv[i],"-v")==0) {
	 /* verbose format */
	 horizFormat = False;
      }
      else if (strcmp(argv[i],"-help")==0) {
	 PrintUsage();
	 return 0;
      }
      else {
	 printf("Unknown option: %s\n", argv[i]);
      }
   }

   dpy = XOpenDisplay(dpyName);

   if (!dpy) {
      printf("Error: couldn't open display %s\n", dpyName ? dpyName : ":0");
      return 1;
   }

   scrn = DefaultScreen(dpy);
   PrintConfigs(dpy, scrn, horizFormat);
   XCloseDisplay(dpy);
   return 0;
}
