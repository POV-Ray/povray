/*
Copyright 2000 Wind River Systems, Inc.

modification history
--------------------
01a,jun01,sra  written.


DESCRIPTION

This file contains descriptions for the UGL/Mesa component

*/

Folder  FOLDER_3D {
        NAME            3D graphics
        SYNOPSIS        Configure the OpenGL components
        _CHILDREN       FOLDER_WINDML
        CHILDREN        INCLUDE_3D_UGLMESA \
			INCLUDE_3D_OSMESA \
                        INCLUDE_3D_GLUMESA \
			INCLUDE_3D_GLUTSHAPES
}


Component INCLUDE_3D_UGLMESA {
        NAME            OpenGL/UGL rendering (Mesa)
        SYNOPSIS        Include support to handle UGL rendering
	REQUIRES	INCLUDE_UGL_ALL
	MODULES		objMesaGL.o objMesaUGL.o
/*        HDR_FILES     ugl/ugl.h ugl/uglfont.h ugl/ugltypes.h */
        INIT_RTN        torMesaUGLInit ();
        _INIT_ORDER     usrRoot
}

Component INCLUDE_3D_OSMESA {
	NAME		OpenGL/Off-Screen rendering (Mesa)
	SYNOPSIS	Include support to handle offscreen rendering
	REQUIRES	INCLUDE_UGL_ALL
	MODULES		objMesaOS.o
        INIT_RTN        torMesaOSInit ();
        _INIT_ORDER     usrRoot
}

Component INCLUDE_3D_GLU {
        NAME            OpenGL Utility support (Mesa)
        SYNOPSIS        Include support to use Mesa GLU with OpenGL
	REQUIRES	INCLUDE_UGL_ALL
        MODULES		objMesaGLU.o
        INIT_RTN        torMesaGLUInit ();
        _INIT_ORDER     usrRoot
}

Component INCLUDE_3D_GLUTSHAPES {
        NAME            OpenGL Utility Toolkit Shapes
        SYNOPSIS        Include support to use shapes from GLUT
	REQUIRES	INCLUDE_3D_GLU
        MODULES		objGLUTShapes.o
        INIT_RTN        torGLUTShapesInit ();
        _INIT_ORDER     usrRoot
}



