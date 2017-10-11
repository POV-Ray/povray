!include <win32.mak>

CFLAGS        = $(cvarsdll) /Ox /G5 /D__MSC__ /DFX /D__WIN32__ \
                /DWIN32 /DMESA_MINWARN /I..\include

OBJS          = glu.obj mipmap.obj nurbs.obj nurbscrv.obj nurbssrf.obj nurbsutl.obj \
	polytest.obj project.obj quadric.obj tess.obj tesselat.obj


PROGRAM       = ..\lib\GLU32.dll

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS)
                $(link) $(dlllflags) /out:$(PROGRAM) \
                        /def:MesaGLU.def $(OBJS) $(guilibsdll) ..\lib\OpenGL32.lib winmm.lib > link.log