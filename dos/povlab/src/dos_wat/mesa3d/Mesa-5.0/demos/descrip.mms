# Makefile for GLUT-based demo programs for VMS
# contributed by Jouk Jansen  joukj@crys.chem.uva.nl


.first
	define gl [-.include.gl]

.include [-]mms-config.

##### MACROS #####

INCDIR = ([-.include],[-.util])
CFLAGS = /include=$(INCDIR)/prefix=all/name=(as_is,short)

.ifdef SHARE
GL_LIBS = $(XLIBS)
.else
GL_LIBS = [-.lib]libGLUT/l,libMesaGLU/l,libMesaGL/l,$(XLIBS)
.endif

LIB_DEP = [-.lib]$(GL_LIB) [-.lib]$(GLU_LIB) [-.lib]$(GLUT_LIB)

PROGS = bounce.exe;,clearspd.exe;,drawpix.exe;,gamma.exe;,gears.exe;,\
	glinfo.exe;,glutfx.exe;,isosurf.exe;,morph3d.exe;,osdemo.exe;,\
	paltex.exe;,pointblast.exe;,reflect.exe;,spectex.exe;,stex3d.exe;,\
	tessdemo.exe;,texcyl.exe;,texobj.exe;,trispd.exe;,winpos.exe;


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

bounce.exe; : bounce.obj $(LIB_DEP)
clearspd.exe; : clearspd.obj $(LIB_DEP)
drawpix.exe; : drawpix.obj $(LIB_DEP)
gamma.exe; : gamma.obj $(LIB_DEP)
gears.exe; : gears.obj $(LIB_DEP)
glinfo.exe; : glinfo.obj $(LIB_DEP)
glutfx.exe; : glutfx.obj $(LIB_DEP)
isosurf.exe; : isosurf.obj $(LIB_DEP)
morph3d.exe; : morph3d.obj $(LIB_DEP)
osdemo.exe; : osdemo.obj $(LIB_DEP)
paltex.exe; : paltex.obj $(LIB_DEP)
pointblast.exe; : pointblast.obj $(LIB_DEP)
reflect.exe; : reflect.obj $(LIB_DEP)
spectex.exe; : spectex.obj $(LIB_DEP)
stex3d.exe; : stex3d.obj $(LIB_DEP)
tessdemo.exe; : tessdemo.obj $(LIB_DEP)
texcyl.exe; : texcyl.obj $(LIB_DEP)
texobj.exe; : texobj.obj $(LIB_DEP)
trispd.exe; : trispd.obj $(LIB_DEP)
winpos.exe; : winpos.obj $(LIB_DEP)
