# Makefile for Persistence of Vision Raytracer 3.0
# For use with DJ's GCC under MS-Dos
#

CFLAGS=-O3 -m486 -funroll-loops
#MORECFLAGS=-Wall -Wtraditional -Wconversion -Wcast-align -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes

#CFLAGS=-m486 -funroll-loops -ffloat-store 
#MORECFLAGS=

#
# Uncomment for debugging (no optimization)
# Comment the strip command below to be able to debug
#

#CFLAGS	=-g
#MORECFLAGS=-Wall -Wtraditional -Wconversion -Wcast-align -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes

#directory macros
S=../../
M=../
P=../../lpng101/
Z=../../zlib/
C=gxx -I$(S) -I$(M) -I$(P) -I$(Z) $(CFLAGS) $(MORECFLAGS) -c


# POV-Ray objects

POVOBJS = atmosph.o bbox.o bcyl.o bezier.o blob.o boxes.o \
  bsphere.o camera.o chi2.o colour.o cones.o \
  console.o csg.o discs.o express.o fractal.o gif.o \
  gifdecod.o media.o interior.o hcmplx.o hfield.o iff.o image.o \
  lathe.o lbuffer.o lighting.o matrices.o mem.o \
  mesh.o msdostxt.o msdosvid.o normal.o objects.o \
  octree.o optin.o optout.o parse.o parstxtr.o \
  pattern.o pgm.o pigment.o planes.o png_pov.o \
  point.o poly.o polygon.o polysolv.o povray.o ppm.o \
  prism.o quadrics.o quatern.o rad_data.o radiosit.o \
  ray.o render.o sor.o spheres.o super.o targa.o \
  texture.o tokenize.o torus.o triangle.o truetype.o \
  txttest.o userio.o vbuffer.o vesavbe.o vlbuffer.o \
  warps.o pmlite.o pmpro.o vflat.o _pmlite.o _pmpro.o _vflat.o

#
# DJGCC Linkage...
#
# Note there is a problem with the linker in djgpp 2.0 which won't
# properly link the pmode.a library.  Therefore we use "ar" to extract
# the component object files and link using "msdosgcc.ln1" link file.
# If this problem is ever fixed in later djgpp releases, then change 
# "msdosgcc.ln1" in the "gcc" line below to "msdosgcc.lnk" instead.  
# Also a few lines above here, remove the line that starts with "pmlite.o".
# Finally, remove the dependencies for pmlite.o pmpro.o vflat.o 
# _pmlite.o _pmpro.o and _vflat.o which extract them from the pmode lib.

povray.exe : $(POVOBJS)
	gxx -o povray @msdosgcc.ln1
	strip povray
	coff2exe povray
	del povray.

#
# Specific module/header dependencies for POV-Ray: 
#

pmlite.o : pmode.a 
	ar -x pmode.a    pmlite.o

pmpro.o : pmode.a 
	ar -x pmode.a     pmpro.o    

vflat.o : pmode.a 
	ar -x pmode.a     vflat.o    

_pmlite.o : pmode.a 
	ar -x pmode.a   _pmlite.o 

_pmpro.o : pmode.a 
	ar -x pmode.a    _pmpro.o   

_vflat.o : pmode.a
	ar -x pmode.a    _vflat.o   

atmosph.o : $(S)atmosph.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)colour.h $(S)frame.h $(S)lighting.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)pattern.h $(S)pigment.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)texture.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)atmosph.c

bbox.o : $(S)bbox.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)bbox.c

bcyl.o : $(S)bcyl.c $(M)config.h $(S)bcyl.h $(S)frame.h $(S)mem.h \
  $(S)povproto.h $(S)vector.h 
	$(C) $(S)bcyl.c

bezier.o : $(S)bezier.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)bezier.h $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h \
  $(S)objects.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)bezier.c

blob.o : $(S)blob.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)blob.h \
  $(S)bsphere.h $(S)camera.h $(S)frame.h $(S)lighting.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)pattern.h $(S)point.h $(S)polysolv.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)texture.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)blob.c

boxes.o : $(S)boxes.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)boxes.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)boxes.c

bsphere.o : $(S)bsphere.c $(M)config.h $(S)bsphere.h $(S)frame.h \
  $(S)mem.h $(S)povproto.h $(S)vector.h 
	$(C) $(S)bsphere.c

camera.o : $(S)camera.c $(M)config.h $(S)camera.h $(S)frame.h \
  $(S)matrices.h $(S)mem.h $(S)normal.h $(S)povproto.h $(S)vector.h 
	$(C) $(S)camera.c

chi2.o : $(S)chi2.c $(M)config.h $(S)chi2.h $(S)frame.h $(S)mem.h \
  $(S)povproto.h 
	$(C) $(S)chi2.c

colour.o : $(S)colour.c $(M)config.h $(S)colour.h $(S)frame.h $(S)mem.h \
  $(S)normal.h $(S)pattern.h $(S)pigment.h $(S)povproto.h $(S)texture.h \
  $(S)vector.h $(S)warps.h 
	$(C) $(S)colour.c

cones.o : $(S)cones.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)cones.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)cones.c

console.o : $(M)console.c $(M)console.h $(M)debug.h $(M)pmode.h 
	$(C) $(M)console.c

csg.o : $(S)csg.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)boxes.h \
  $(S)camera.h $(S)csg.h $(S)frame.h $(S)hfield.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)planes.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)quadrics.h $(S)render.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)csg.c

discs.o : $(S)discs.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)discs.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)discs.c

express.o : $(S)express.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)colour.h $(S)express.h $(S)frame.h $(S)matrices.h \
  $(S)mem.h $(S)normal.h $(S)parse.h $(S)parstxtr.h $(S)pattern.h \
  $(S)pigment.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)texture.h $(S)tokenize.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)express.c

fractal.o : $(S)fractal.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)fractal.h $(S)frame.h $(S)hcmplx.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)quatern.h $(S)render.h $(S)spheres.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)fractal.c

gif.o : $(S)gif.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)gif.h $(S)gifdecod.h $(S)mem.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)gif.c

gifdecod.o : $(S)gifdecod.c $(M)config.h $(S)frame.h $(S)gif.h \
  $(S)gifdecod.h $(S)mem.h $(S)povproto.h 
	$(C) $(S)gifdecod.c

media.o : $(S)media.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)colour.h $(S)frame.h $(S)media.h $(S)interior.h $(S)lighting.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)pattern.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)render.h $(S)texture.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)media.c

interior.o : $(S)interior.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)colour.h $(S)frame.h $(S)media.h $(S)interior.h $(S)lighting.h $(S)matrices.h \
  $(S)mem.h $(S)objects.h $(S)pattern.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)render.h $(S)texture.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)interior.c

hcmplx.o : $(S)hcmplx.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)fractal.h $(S)frame.h $(S)hcmplx.h $(S)mem.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)spheres.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)hcmplx.c

hfield.o : $(S)hfield.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)boxes.h \
  $(S)camera.h $(S)frame.h $(S)hfield.h $(S)matrices.h $(S)mem.h \
  $(S)objects.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)hfield.c

iff.o : $(S)iff.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)iff.h $(S)mem.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)iff.c

image.o : $(S)image.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)image.h $(S)matrices.h $(S)mem.h $(S)pattern.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)texture.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)image.c

lathe.o : $(S)lathe.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)bcyl.h \
  $(S)camera.h $(S)frame.h $(S)lathe.h $(S)matrices.h $(S)mem.h \
  $(S)objects.h $(S)point.h $(S)polysolv.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)torus.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)lathe.c

lbuffer.o : $(S)lbuffer.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)lbuffer.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)triangle.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)lbuffer.c

lighting.o : $(S)lighting.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)blob.h $(S)bsphere.h $(S)camera.h $(S)colour.h $(S)frame.h \
  $(S)image.h $(S)lbuffer.h $(S)lighting.h $(S)mem.h \
  $(S)mesh.h $(S)normal.h $(S)objects.h $(S)octree.h $(S)pattern.h \
  $(S)pigment.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)radiosit.h \
  $(S)ray.h $(S)render.h $(S)texture.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)lighting.c

matrices.o : $(S)matrices.c $(M)config.h $(S)frame.h $(S)matrices.h \
  $(S)mem.h $(S)povproto.h $(S)vector.h 
	$(C) $(S)matrices.c

mem.o : $(S)mem.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)parse.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)mem.c

mesh.o : $(S)mesh.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)mesh.h $(S)objects.h \
  $(S)pattern.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)texture.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)mesh.c

msdostxt.o : $(M)msdostxt.c $(M)config.h $(M)console.h $(M)debug.h \
  $(M)msdosall.h $(S)atmosph.h $(S)bbox.h $(S)camera.h $(S)frame.h \
  $(S)mem.h $(S)optin.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(M)msdostxt.c

msdosvid.o : $(M)msdosvid.c $(M)config.h $(M)console.h $(M)debug.h \
  $(M)msdosall.h $(M)vesavbe.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(M)msdosvid.c

normal.o : $(S)normal.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)image.h $(S)matrices.h $(S)mem.h \
  $(S)normal.h $(S)pattern.h $(S)pigment.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)render.h $(S)texture.h $(S)txttest.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)normal.c

objects.o : $(S)objects.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)mem.h $(S)objects.h \
  $(S)pattern.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)texture.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)objects.c

octree.o : $(S)octree.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)mem.h $(S)octree.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)radiosit.h $(S)render.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)octree.c

optin.o : $(S)optin.c $(M)config.h $(P)png.h $(P)pngconf.h $(S)atmosph.h \
  $(S)bbox.h $(S)camera.h $(S)frame.h $(S)lighting.h $(S)mem.h \
  $(S)octree.h $(S)optin.h $(S)optout.h $(S)parse.h $(S)png_pov.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)ppm.h $(S)radiosit.h \
  $(S)render.h $(S)targa.h $(S)tokenize.h $(S)userio.h $(S)vlbuffer.h \
  $(S)warps.h $(Z)zconf.h $(Z)zlib.h 
	$(C) $(S)optin.c

optout.o : $(S)optout.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)bcyl.h \
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

parse.o : $(S)parse.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)bcyl.h \
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

parstxtr.o : $(S)parstxtr.c $(M)config.h $(P)png.h $(P)pngconf.h \
  $(S)atmosph.h $(S)bbox.h $(S)camera.h $(S)colour.h $(S)express.h \
  $(S)frame.h $(S)gif.h $(S)iff.h $(S)image.h $(S)matrices.h \
  $(S)mem.h $(S)normal.h $(S)parse.h $(S)parstxtr.h $(S)pattern.h \
  $(S)pgm.h $(S)pigment.h $(S)png_pov.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)ppm.h $(S)render.h $(S)targa.h $(S)texture.h \
  $(S)tokenize.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h $(Z)zconf.h \
  $(Z)zlib.h 
	$(C) $(S)parstxtr.c

pattern.o : $(S)pattern.c $(M)config.h $(S)colour.h $(S)frame.h \
  $(S)image.h $(S)matrices.h $(S)mem.h $(S)pattern.h $(S)povproto.h \
  $(S)texture.h $(S)txttest.h $(S)vector.h $(S)warps.h 
	$(C) $(S)pattern.c

pgm.o : $(S)pgm.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)pgm.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)ppm.h $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)pgm.c

pigment.o : $(S)pigment.c $(M)config.h $(S)colour.h $(S)frame.h \
  $(S)image.h $(S)matrices.h $(S)mem.h $(S)pattern.h $(S)pigment.h \
  $(S)povproto.h $(S)texture.h $(S)txttest.h $(S)vector.h $(S)warps.h 
	$(C) $(S)pigment.c

planes.o : $(S)planes.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)planes.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)planes.c

png_pov.o : $(S)png_pov.c $(M)config.h $(P)png.h $(P)pngconf.h \
  $(S)atmosph.h $(S)bbox.h $(S)camera.h $(S)frame.h $(S)mem.h \
  $(S)optout.h $(S)png_pov.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)vlbuffer.h $(S)warps.h $(Z)zconf.h $(Z)zlib.h 
	$(C) $(S)png_pov.c

point.o : $(S)point.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)point.c

poly.o : $(S)poly.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)poly.h $(S)polysolv.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)poly.c

polygon.o : $(S)polygon.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)polygon.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)polygon.c

polysolv.o : $(S)polysolv.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)mem.h $(S)point.h $(S)polysolv.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)vector.h $(S)vlbuffer.h \
  $(S)warps.h 
	$(C) $(S)polysolv.c

povray.o : $(S)povray.c $(M)config.h $(P)png.h $(P)pngconf.h \
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

ppm.o : $(S)ppm.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)optout.h $(S)pgm.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)ppm.h $(S)render.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)ppm.c

prism.o : $(S)prism.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)polysolv.h $(S)povproto.h $(S)povray.h $(S)prism.h $(S)render.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)prism.c

quadrics.o : $(S)quadrics.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)planes.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)quadrics.h \
  $(S)render.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)quadrics.c

quatern.o : $(S)quatern.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)fractal.h $(S)frame.h $(S)mem.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)quatern.h $(S)render.h $(S)spheres.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)quatern.c

radiosit.o : $(S)radiosit.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)lighting.h $(S)mem.h $(S)octree.h \
  $(S)optin.h $(S)pattern.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)radiosit.h $(S)ray.h $(S)render.h $(S)texture.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)radiosit.c

rad_data.o : $(S)rad_data.c $(M)config.h $(S)frame.h $(S)octree.h \
  $(S)radiosit.h 
	$(C) $(S)rad_data.c

ray.o : $(S)ray.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)pattern.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)ray.h $(S)render.h $(S)texture.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)ray.c

render.o : $(S)render.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)chi2.h $(S)colour.h $(S)frame.h $(S)lighting.h \
  $(S)mem.h $(S)normal.h $(S)objects.h $(S)octree.h $(S)optout.h \
  $(S)pattern.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)radiosit.h \
  $(S)ray.h $(S)render.h $(S)targa.h $(S)texture.h $(S)userio.h \
  $(S)vbuffer.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)render.c

sor.o : $(S)sor.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)bcyl.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)polysolv.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)sor.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)sor.c

spheres.o : $(S)spheres.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)spheres.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)spheres.c

super.o : $(S)super.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)super.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)super.c

targa.o : $(S)targa.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)mem.h $(S)point.h $(S)povproto.h $(S)povray.h \
  $(S)render.h $(S)targa.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)targa.c

texture.o : $(S)texture.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)image.h $(S)matrices.h \
  $(S)mem.h $(S)normal.h $(S)pattern.h $(S)pigment.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)texture.h $(S)vector.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)texture.c

tokenize.o : $(S)tokenize.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)express.h $(S)frame.h $(S)mem.h $(S)parse.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)tokenize.h \
  $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)tokenize.c

torus.o : $(S)torus.c $(M)config.h $(S)atmosph.h $(S)bbox.h $(S)camera.h \
  $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h \
  $(S)polysolv.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)torus.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)torus.c

triangle.o : $(S)triangle.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)matrices.h $(S)mem.h $(S)objects.h \
  $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h $(S)triangle.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)triangle.c

truetype.o : $(S)truetype.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)csg.h $(S)frame.h $(S)matrices.h $(S)mem.h \
  $(S)objects.h $(S)point.h $(S)povproto.h $(S)povray.h $(S)render.h \
  $(S)truetype.h $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)truetype.c

txttest.o : $(S)txttest.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)mem.h $(S)pattern.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)texture.h $(S)txttest.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)txttest.c

userio.o : $(S)userio.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)camera.h $(S)frame.h $(S)mem.h $(S)parse.h $(S)point.h \
  $(S)povproto.h $(S)povray.h $(S)render.h $(S)tokenize.h $(S)userio.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)userio.c

vbuffer.o : $(S)vbuffer.c $(M)config.h $(S)atmosph.h $(S)bbox.h \
  $(S)boxes.h $(S)camera.h $(S)frame.h $(S)hfield.h $(S)lighting.h \
  $(S)matrices.h $(S)mem.h $(S)objects.h $(S)point.h $(S)povproto.h \
  $(S)povray.h $(S)render.h $(S)triangle.h $(S)userio.h $(S)vbuffer.h \
  $(S)vector.h $(S)vlbuffer.h $(S)warps.h 
	$(C) $(S)vbuffer.c

vesavbe.o : $(M)vesavbe.c $(M)debug.h $(M)pmode.h $(M)vesavbe.h 
	$(C) $(M)vesavbe.c

vlbuffer.o : $(S)vlbuffer.c $(M)config.h $(S)bbox.h $(S)frame.h $(S)mem.h \
  $(S)povproto.h $(S)vector.h $(S)vlbuffer.h 
	$(C) $(S)vlbuffer.c

warps.o : $(S)warps.c $(M)config.h $(S)frame.h $(S)matrices.h $(S)mem.h \
  $(S)pattern.h $(S)povproto.h $(S)texture.h $(S)vector.h $(S)warps.h 
	$(C) $(S)warps.c

