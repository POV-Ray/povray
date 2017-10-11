# Makefile for Persistence of Vision Raytracing Program
#
# This make file requires a Borland BC++ 4.02 or later and the
# 32-bit DOS Power Pack option to create a 32-bit protected mode
# application.  Neither 16-bit real mode nor 16-bit protected mode
# are supported here.  A Borland MAKE utility which supports
# conditional directives is also required.
#
# Written by Chris Young CIS:[76702,1655]

#***************************************************************
#*
#*                      User Defined Options
#*
#***************************************************************
#
#***************************************************************
#*  Compiler \ Linker Options set automatically from User
#*  Defined Switches above
#***************************************************************

!if $d(DEBUG)
CDEBUG	=-v -N -r- -Od
!else
CDEBUG	=-v-
!endif

# Compiler flags common to all CPUs \ FPUs
#
#  -P-C source code is C, not C++
#  -WX  32-bit DPMI compilation  
#  -r   use registers
#  -K   default character unsigned
#  -G   optimize for speed rather than size
#  -O   optimize jumps
#  -Z   optimize register use
#  -d   merge duplicate strings
#  -c   compile only
#  -N-  stack overflow check off
#  -B   compile via assembly
#  -k-  standard stack on all calls off
#  -3   386 processor (-4 for 486, -5 for Pentium)
#  -f   use floating point
#  -H   use precompiled headers

FIXED = -WX -c -P -K -N- -k- -A-
#FIXED = -WX -c -P -H=povray -K

OPTIMISATIONS = -d -3 -r- -f
#OPTIMISATIONS = -Od -d -3 -r- -f
#OPTIMISATIONS =      d -3 -r -ff -G -Z -O -Ob -Oc -Oe -Og -Oi -Ol -Om -Op -Ov -Ox
#OPTIMISATIONS = -Od -d -3 -r- -ff  -Ol
#OPTIMISATIONS = -Od -d -3 -r- -ff- -Ol -Ox -Oi -Om -Op -Ov -Og -Ob

#directory macros
S=..\..\\
M=..\\
P=..\..\lpng101\\
Z=..\..\zlib\\

CFLAGS=$(FIXED) $(OPTIMISATIONS) $(CDEBUG) -I$(S) -I$(M) -I$(P) -I$(Z)
C=BCC32 $(CFLAGS)

#***************************************************************
#*
#*                          Common  Stuff
#*
#***************************************************************

POVOBJS = ATMOSPH.OBJ BBOX.OBJ BEZIER.OBJ BLOB.OBJ BOXES.OBJ BSPHERE.OBJ \
 CAMERA.OBJ COLOUR.OBJ CONES.OBJ CSG.OBJ DISCS.OBJ EXPRESS.OBJ BCYL.OBJ \
 GIFDECOD.OBJ GIF.OBJ MEDIA.OBJ INTERIOR.OBJ HFIELD.OBJ IFF.OBJ IMAGE.OBJ QUATERN.OBJ \
 LATHE.OBJ LBUFFER.OBJ LIGHTING.OBJ MATRICES.OBJ USERIO.OBJ MEM.OBJ MESH.OBJ \
 NORMAL.OBJ OBJECTS.OBJ OPTIN.OBJ PARSE.OBJ PARSTXTR.OBJ OPTOUT.OBJ \
 PATTERN.OBJ PGM.OBJ PIGMENT.OBJ PLANES.OBJ POINT.OBJ POLYGON.OBJ \
 POLYSOLV.OBJ POLY.OBJ POVRAY.OBJ PPM.OBJ PRISM.OBJ QUADRICS.OBJ QUATERN.OBJ \
 RAY.OBJ RENDER.OBJ SOR.OBJ SPHERES.OBJ SUPER.OBJ TARGA.OBJ TEXTURE.OBJ \
 TOKENIZE.OBJ TORUS.OBJ TRIANGLE.OBJ TRUETYPE.OBJ TXTTEST.OBJ VBUFFER.OBJ \
 VLBUFFER.OBJ WARPS.OBJ OCTREE.OBJ RADIOSIT.OBJ VESAVBE.OBJ MSDOSVID.OBJ MSDOSTXT.OBJ \
 CONSOLE.OBJ FRACTAL.OBJ HCMPLX.OBJ CHI2.OBJ PNG_POV.OBJ RAD_DATA.OBJ

# NOTE: AUTODEPEND doesn't work here.
#
povray.exe : $(POVOBJS)
        TLINK32 @MSDOSBC.LNK

#
# Specific module/header dependencies for POV-Ray:
#

atmosph.obj : $(S)atmosph.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)colour.h $(S)frame.h $(S)lighting.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)pattern.h $(S)pigment.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)texture.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)atmosph.c

bbox.obj : $(S)bbox.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)bbox.c

bcyl.obj : $(S)bcyl.c $(M)config.h $(S)bcyl.h $(S)frame.h $(S)mem.h \
  $(S)povproto.h $(S)vector.h 
	$(C) $(S)bcyl.c

bezier.obj : $(S)bezier.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)bezier.h $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h \
  $(S)objects.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)bezier.c

blob.obj : $(S)blob.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)blob.h \
  $(S)bsphere.h $(S)camera.h $(S)frame.h $(S)lighting.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)pattern.h $(S)point.h $(S)polysolv.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)texture.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)blob.c

boxes.obj : $(S)boxes.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)boxes.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)boxes.c

bsphere.obj : $(S)bsphere.c $(M)config.h $(S)bsphere.h $(S)frame.h \
  $(S)mem.h $(S)povproto.h $(S)vector.h 
	$(C) $(S)bsphere.c

camera.obj : $(S)camera.c $(M)config.h $(S)camera.h $(S)frame.h \
  $(S)matrices.h $(S)mem.h $(S)normal.h $(S)povproto.h $(S)vector.h 
	$(C) $(S)camera.c

chi2.obj : $(S)chi2.c $(M)config.h $(S)chi2.h $(S)frame.h $(S)mem.h \
  $(S)povproto.h 
	$(C) $(S)chi2.c

colour.obj : $(S)colour.c $(M)config.h $(S)colour.h $(S)frame.h $(S)mem.h \
  $(S)normal.h $(S)pattern.h $(S)pigment.h $(S)povproto.h $(S)texture.h \
  $(S)vector.h $(S)warps.h 
	$(C) $(S)colour.c

cones.obj : $(S)cones.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)cones.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)cones.c

console.obj : $(M)console.c $(M)console.h $(M)debug.h $(M)pmode.h 
	$(C) $(M)console.c

csg.obj : $(S)csg.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)boxes.h \
  $(S)camera.h $(S)csg.h $(S)frame.h $(S)hfield.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)planes.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)quadrics.h $(S)render.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)csg.c

discs.obj : $(S)discs.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)discs.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)discs.c

express.obj : $(S)express.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)colour.h $(S)express.h $(S)frame.h $(S)matrices.h \
  $(S)mem.h $(S)normal.h $(S)parse.h $(S)parstxtr.h $(S)pattern.h \
  $(S)pigment.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)texture.h $(S)tokenize.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)express.c

fractal.obj : $(S)fractal.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)fractal.h $(S)frame.h $(S)hcmplx.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)quatern.h $(S)render.h $(S)spheres.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)fractal.c

gif.obj : $(S)gif.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)gif.h $(S)gifdecod.h $(S)mem.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)gif.c

gifdecod.obj : $(S)gifdecod.c $(M)config.h $(S)frame.h $(S)gif.h \
  $(S)gifdecod.h $(S)mem.h $(S)povproto.h 
	$(C) $(S)gifdecod.c

media.obj : $(S)media.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)colour.h $(S)frame.h $(S)interior.h $(S)media.h $(S)lighting.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)pattern.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)render.h $(S)texture.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)media.c

interior.obj : $(S)interior.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)colour.h $(S)frame.h $(S)interior.h $(S)media.h $(S)lighting.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)pattern.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)render.h $(S)texture.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)interior.c

hcmplx.obj : $(S)hcmplx.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)fractal.h $(S)frame.h $(S)hcmplx.h $(S)mem.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)spheres.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)hcmplx.c

hfield.obj : $(S)hfield.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)boxes.h \
  $(S)camera.h $(S)frame.h $(S)hfield.h $(S)matrices.h $(S)mem.h \
  $(S)objects.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)hfield.c

iff.obj : $(S)iff.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)iff.h $(S)mem.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)iff.c

image.obj : $(S)image.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)image.h $(S)matrices.h $(S)mem.h $(S)pattern.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)texture.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)image.c

lathe.obj : $(S)lathe.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)bcyl.h \
  $(S)camera.h $(S)frame.h $(S)lathe.h $(S)matrices.h $(S)mem.h \
  $(S)objects.h $(S)point.h $(S)polysolv.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)torus.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)lathe.c

lbuffer.obj : $(S)lbuffer.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)lbuffer.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)triangle.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)lbuffer.c

lighting.obj : $(S)lighting.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)blob.h $(S)bsphere.h $(S)camera.h $(S)colour.h $(S)frame.h \
  $(S)image.h $(S)lbuffer.h $(S)lighting.h $(S)mem.h \
  $(S)mesh.h $(S)normal.h $(S)objects.h $(S)octree.h $(S)pattern.h \
  $(S)pigment.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)radiosit.h \
  $(S)ray.h $(S)render.h $(S)texture.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)lighting.c

matrices.obj : $(S)matrices.c $(M)config.h $(S)frame.h $(S)matrices.h \
  $(S)mem.h $(S)povproto.h $(S)vector.h 
	$(C) $(S)matrices.c

mem.obj : $(S)mem.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)parse.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)mem.c

mesh.obj : $(S)mesh.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)mesh.h $(S)objects.h \
  $(S)pattern.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)texture.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)mesh.c

msdostxt.obj : $(M)msdostxt.c $(M)config.h $(M)console.h $(M)debug.h \
  $(M)msdosall.h $(S)atmosph.h $(S)bbox.h $(S)camera.h $(S)frame.h \
  $(S)mem.h $(S)optin.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(M)msdostxt.c

msdosvid.obj : $(M)msdosvid.c $(M)config.h $(M)console.h $(M)debug.h \
  $(M)msdosall.h $(M)vesavbe.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(M)msdosvid.c

normal.obj : $(S)normal.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)image.h $(S)matrices.h $(S)mem.h \
  $(S)normal.h $(S)pattern.h $(S)pigment.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)render.h $(S)texture.h $(S)txttest.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)normal.c

objects.obj : $(S)objects.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)mem.h $(S)objects.h \
  $(S)pattern.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)texture.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)objects.c

octree.obj : $(S)octree.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)mem.h $(S)octree.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)radiosit.h $(S)render.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)octree.c

optin.obj : $(S)optin.c $(M)config.h $(P)png.h $(P)pngconf.h $(S)atmosph.h \
  $(S)bbox.h $(S)camera.h $(S)frame.h $(S)lighting.h $(S)mem.h \
  $(S)octree.h $(S)optin.h $(S)optout.h $(S)parse.h $(S)png_pov.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)ppm.h $(S)radiosit.h \
  $(S)render.h $(S)targa.h $(S)tokenize.h $(S)userio.h $(S)vlbuffer.h \
  $(S)warps.h $(Z)zconf.h $(Z)zlib.h 
	$(C) $(S)optin.c

optout.obj : $(S)optout.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)bcyl.h \
  $(S)bezier.h $(S)blob.h $(S)boxes.h $(S)bsphere.h $(S)camera.h \
  $(S)cones.h $(S)csg.h $(S)discs.h $(S)fractal.h $(S)frame.h \
  $(S)hfield.h $(S)lathe.h $(S)lbuffer.h $(S)lighting.h $(S)mem.h \
  $(S)mesh.h $(S)objects.h $(S)octree.h $(S)optin.h $(S)optout.h \
  $(S)parse.h $(S)pattern.h $(S)pgm.h $(S)planes.h $(S)point.h $(S)poly.h \
  $(S)polygon.h $(S)polysolv.h $(S)povproto.h $(S)povray.h $(S)ppm.h \
  $(S)prism.h $(S)quadrics.h $(S)radiosit.h $(S)render.h $(S)sor.h \
  $(S)spheres.h $(S)super.h $(S)targa.h $(S)texture.h $(S)torus.h \
  $(S)triangle.h $(S)truetype.h $(S)userio.h $(S)vbuffer.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)optout.c

parse.obj : $(S)parse.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)bcyl.h \
  $(S)bezier.h $(S)blob.h $(S)boxes.h $(S)bsphere.h $(S)camera.h \
  $(S)colour.h $(S)cones.h $(S)csg.h $(S)discs.h $(S)express.h \
  $(S)fractal.h $(S)frame.h $(S)gif.h $(S)hfield.h $(S)iff.h \
  $(S)image.h $(S)interior.h $(S)lathe.h $(S)matrices.h $(S)mem.h $(S)mesh.h \
  $(S)normal.h $(S)objects.h $(S)octree.h $(S)parse.h $(S)parstxtr.h \
  $(S)pattern.h $(S)pgm.h $(S)pigment.h $(S)planes.h $(S)point.h \
  $(S)poly.h $(S)polygon.h $(S)polysolv.h $(S)povproto.h $(S)povray.h \
  $(S)ppm.h $(S)prism.h $(S)quadrics.h $(S)radiosit.h $(S)render.h \
  $(S)sor.h $(S)spheres.h $(S)super.h $(S)targa.h $(S)texture.h \
  $(S)tokenize.h $(S)torus.h $(S)triangle.h $(S)truetype.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)parse.c

parstxtr.obj : $(S)parstxtr.c $(M)config.h $(P)png.h $(P)pngconf.h \
  $(S)atmosph.h $(S)bbox.h $(S)camera.h $(S)colour.h $(S)express.h \
  $(S)frame.h $(S)gif.h $(S)iff.h $(S)image.h $(S)matrices.h \
  $(S)mem.h $(S)normal.h $(S)parse.h $(S)parstxtr.h $(S)pattern.h \
  $(S)pgm.h $(S)pigment.h $(S)png_pov.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)ppm.h $(S)render.h $(S)targa.h $(S)texture.h \
  $(S)tokenize.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h $(Z)zconf.h \
  $(Z)zlib.h 
	$(C) $(S)parstxtr.c

pattern.obj : $(S)pattern.c $(M)config.h $(S)colour.h $(S)frame.h \
  $(S)image.h $(S)matrices.h $(S)mem.h $(S)pattern.h $(S)povproto.h \
  $(S)texture.h $(S)txttest.h $(S)vector.h $(S)warps.h 
	$(C) $(S)pattern.c

pgm.obj : $(S)pgm.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)pgm.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)ppm.h $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)pgm.c

pigment.obj : $(S)pigment.c $(M)config.h $(S)colour.h $(S)frame.h \
  $(S)image.h $(S)matrices.h $(S)mem.h $(S)pattern.h $(S)pigment.h \
  $(S)povproto.h $(S)texture.h $(S)txttest.h $(S)vector.h $(S)warps.h 
	$(C) $(S)pigment.c

planes.obj : $(S)planes.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)planes.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)planes.c

png_pov.obj : $(S)png_pov.c $(M)config.h $(P)png.h $(P)pngconf.h \
  $(S)atmosph.h $(S)bbox.h $(S)camera.h $(S)frame.h $(S)mem.h \
  $(S)optout.h $(S)png_pov.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)vlbuffer.h $(S)warps.h $(Z)zconf.h $(Z)zlib.h 
	$(C) $(S)png_pov.c

point.obj : $(S)point.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)point.c

poly.obj : $(S)poly.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)poly.h $(S)polysolv.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)poly.c

polygon.obj : $(S)polygon.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)polygon.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)polygon.c

polysolv.obj : $(S)polysolv.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)mem.h $(S)point.h $(S)polysolv.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)polysolv.c

povray.obj : $(S)povray.c $(M)config.h $(P)png.h $(P)pngconf.h \
  $(S)atmosph.h $(S)bbox.h $(S)bcyl.h $(S)bezier.h $(S)blob.h $(S)boxes.h \
  $(S)bsphere.h $(S)camera.h $(S)cones.h $(S)csg.h $(S)discs.h \
  $(S)express.h $(S)fractal.h $(S)frame.h $(S)hfield.h $(S)lathe.h \
  $(S)lbuffer.h $(S)lighting.h $(S)mem.h $(S)mesh.h $(S)objects.h \
  $(S)octree.h $(S)optin.h $(S)optout.h $(S)parse.h $(S)pattern.h \
  $(S)pgm.h $(S)pigment.h $(S)planes.h $(S)png_pov.h $(S)point.h \
  $(S)poly.h $(S)polygon.h $(S)polysolv.h $(S)povproto.h $(S)povray.h \
  $(S)ppm.h $(S)prism.h $(S)quadrics.h $(S)radiosit.h $(S)render.h \
  $(S)sor.h $(S)spheres.h $(S)super.h $(S)targa.h $(S)texture.h \
  $(S)tokenize.h $(S)torus.h $(S)triangle.h $(S)truetype.h $(S)userio.h \
  $(S)vbuffer.h $(S)vlbuffer.h $(S)warps.h $(Z)zconf.h $(Z)zlib.h 
	$(C) $(S)povray.c

ppm.obj : $(S)ppm.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)optout.h $(S)pgm.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)ppm.h $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)ppm.c

prism.obj : $(S)prism.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)polysolv.h $(S)povproto.h $(S)povray.h $(S)prism.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)prism.c

quadrics.obj : $(S)quadrics.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)planes.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)quadrics.h \
  $(S)render.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)quadrics.c

quatern.obj : $(S)quatern.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)fractal.h $(S)frame.h $(S)mem.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)quatern.h $(S)render.h $(S)spheres.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)quatern.c

radiosit.obj : $(S)radiosit.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)lighting.h $(S)mem.h $(S)octree.h \
  $(S)optin.h $(S)pattern.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)radiosit.h $(S)ray.h $(S)render.h $(S)texture.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)radiosit.c

rad_data.obj : $(S)rad_data.c $(M)config.h $(S)frame.h $(S)octree.h \
  $(S)radiosit.h 
	$(C) $(S)rad_data.c

ray.obj : $(S)ray.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)pattern.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)ray.h $(S)render.h $(S)texture.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)ray.c

render.obj : $(S)render.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)chi2.h $(S)colour.h $(S)frame.h $(S)lighting.h \
  $(S)mem.h $(S)normal.h $(S)objects.h $(S)octree.h $(S)optout.h \
  $(S)pattern.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)radiosit.h \
  $(S)ray.h $(S)render.h $(S)targa.h $(S)texture.h $(S)userio.h \
  $(S)vbuffer.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)render.c

sor.obj : $(S)sor.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)bcyl.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)polysolv.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)sor.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)sor.c

spheres.obj : $(S)spheres.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)spheres.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)spheres.c

super.obj : $(S)super.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)super.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)super.c

targa.obj : $(S)targa.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)targa.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)targa.c

texture.obj : $(S)texture.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)image.h $(S)matrices.h \
  $(S)mem.h $(S)normal.h $(S)pattern.h $(S)pigment.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)texture.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)texture.c

tokenize.obj : $(S)tokenize.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)express.h $(S)frame.h $(S)mem.h $(S)parse.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)tokenize.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)tokenize.c

torus.obj : $(S)torus.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)polysolv.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)torus.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)torus.c

triangle.obj : $(S)triangle.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)triangle.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)triangle.c

truetype.obj : $(S)truetype.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)csg.h $(S)frame.h $(S)matrices.h $(S)mem.h \
  $(S)objects.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)truetype.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)truetype.c

txttest.obj : $(S)txttest.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)mem.h $(S)pattern.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)texture.h $(S)txttest.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)txttest.c

userio.obj : $(S)userio.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)mem.h $(S)parse.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)tokenize.h $(S)userio.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)userio.c

vbuffer.obj : $(S)vbuffer.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)boxes.h $(S)camera.h $(S)frame.h $(S)hfield.h $(S)lighting.h \
  $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)render.h $(S)triangle.h $(S)userio.h $(S)vbuffer.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)vbuffer.c

vesavbe.obj : $(M)vesavbe.c $(M)debug.h $(M)pmode.h $(M)vesavbe.h 
	$(C) $(M)vesavbe.c

vlbuffer.obj : $(S)vlbuffer.c $(M)config.h $(S)bbox.h $(S)frame.h $(S)mem.h \
  $(S)povproto.h $(S)vector.h $(S)vlbuffer.h 
	$(C) $(S)vlbuffer.c

warps.obj : $(S)warps.c $(M)config.h $(S)frame.h $(S)matrices.h $(S)mem.h \
  $(S)pattern.h $(S)povproto.h $(S)texture.h $(S)vector.h $(S)warps.h 
	$(C) $(S)warps.c

