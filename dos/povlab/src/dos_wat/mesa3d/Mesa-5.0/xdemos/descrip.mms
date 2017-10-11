# Makefile for GLUT-based demo programs for VMS
# contributed by Jouk Jansen  joukj@crys.chem.uva.nl


.first
	define gl [-.include.gl]

.include [-]mms-config.

##### MACROS #####

INCDIR = ([-.include],[-.util])
CFLAGS = /include=$(INCDIR)/prefix=all/name=(as_is,short)/nowarn

.ifdef SHARE
GL_LIBS = $(XLIBS)
.else
GL_LIBS = [-.lib]libGLUT/l,libMesaGLU/l,libMesaGL/l,$(XLIBS)
.endif

LIB_DEP = [-.lib]$(GL_LIB) [-.lib]$(GLU_LIB) [-.lib]$(GLUT_LIB)

PROGS =glthreads.exe,\
	glxdemo.exe,\
	glxgears.exe,\
	glxheads.exe,\
	glxinfo.exe,\
	glxpixmap.exe,\
	manywin.exe,\
	offset.exe,\
	pbinfo.exe,\
	pbdemo.exe,\
	wincopy.exe,\
	xdemo.exe,\
	xfont.exe

##### RULES #####
.obj.exe :
	cxxlink $(MMS$TARGET_NAME),$(GL_LIBS)

##### TARGETS #####
default :
	$(MMS)$(MMSQUALIFIERS) $(PROGS)

clean :
	delete *.obj;*

realclean :
	delete $(PROGS)
	delete *.obj;*


glthreads.exe : glthreads.obj $(LIB_DEP) 
glxdemo.exe : glxdemo.obj $(LIB_DEP)
glxgears.exe : glxgears.obj $(LIB_DEP)
glxheads.exe : glxheads.obj $(LIB_DEP)
glxinfo.exe : glxinfo.obj $(LIB_DEP)
glxpixmap.exe : glxpixmap.obj $(LIB_DEP)
manywin.exe : manywin.obj $(LIB_DEP)
offset.exe : offset.obj $(LIB_DEP)
pbinfo.exe : pbinfo.obj pbutil.obj $(LIB_DEP)
	cxxlink pbinfo.obj,pbutil.obj,$(GL_LIBS)
pbdemo.exe : pbdemo.obj pbutil.obj $(LIB_DEP)
	cxxlink pbdemo.obj,pbutil.obj,$(GL_LIBS)
wincopy.exe : wincopy.obj $(LIB_DEP)
xdemo.exe : xdemo.obj $(LIB_DEP)
xfont.exe :xfont.obj $(LIB_DEP)

