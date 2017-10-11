
# Be sure to modify the definitions in this file to agree with your
# systems installation.
#
#  NOTE: be sure that the install directories use '\' not '/' for paths.
#  NOTE: the install target may overwrite important files in the system dir

# MSVC install directories
#
LIBINSTALL     = $(MSDEVDIR)\..\VC98\LIB
INCLUDEINSTALL = $(MSDEVDIR)\..\VC98\INCLUDE

# win32 dll directory
#
!IF "$(OS)" == "Windows_NT"
DLLINSTALL     = $(WINDIR)\SYSTEM32
!ELSE
DLLINSTALL     = $(WINDIR)\SYSTEM
!ENDIF

# Names for Mesa libraries 
#
MESALIB   = MesaGL.lib
MESADLL   = MesaGL.dll
OPENGL	  = $(MESALIB)
GLULIB	  = MesaGLU.lib
GLUDLL	  = MesaGLU.dll
GLU	  = $(GLULIB)
GLUTLIB   = MesaGlut.lib
GLUTDLL   = MesaGlut.dll
GLUT	  = $(GLUTLIB)
OSMESALIB = osmesa.lib
OSMESADLL = osmesa.dll
EXTRALIBS = $(OSMESALIB)

# common definitions used by all makefiles
CFLAGS	= $(cflags) $(cdebug) $(EXTRACFLAGS) -I$(TOP)\include
LIBS	= $(lflags) $(ldebug) $(GLUT) $(GLU) $(OPENGL) $(guilibs)
EXES	= $(SRCS:.c=.exe) $(CPPSRCS:.cpp=.exe) $(CXXSRCS:.cxx=.exe)
OSMESAEXES = $(OSMESASRCS:.c=.exe)
DEPLIBS = $(GLUT) $(GLU) $(OPENGL) $(EXTRALIBS)

!IFNDEF NODEBUG
lcommon = /DEBUG
!ENDIF

# default rule
default	: $(EXES) $(OSMESAEXES) $(IPERSEXES)

# cleanup rules
clean	::
	@del /f *.obj
	@del /f *.pdb
	@del /f *.ilk
	@del /f *.ncb
	@del /f *~
	@del /f *.exp

clobber	:: clean
	@del /f *.exe
	@del /f *.dll
	@del /f *.lib

# default inference rules

.c.obj	:
	$(CC) $(CFLAGS) $<
.cpp.obj :
	$(CC) $(CFLAGS) $<
.cxx.obj :
	$(CC) $(CFLAGS) $<
