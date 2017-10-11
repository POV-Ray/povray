CC      = f:\wc\binnt\wcc386.exe
CCP     = f:\wc\binnt\wpp386.exe
LINKER  = f:\wc\binnt\wlink.exe
INC     = include\
INC2    = f:\wc\h
H       = $(INC)GLOBAL.H $(INC)GLIB.H $(INC)LIB.H
HPP     = $(INC)FONT3D.H $(INC)GEOMETRY.H $(INC)TRUETYPE.H $(INC)VECTOR.H $(INC)CONFIG.H

# --------  MS-DOS 32 DOS4G Extender compilation

CFLAGS  = /mf /oneatxm /zp4 /wx /fp5 /7 /v /5r /ou /fpi87 /i=$(INC) /i=$(INC2)

# --------  MS-WINDOWS 32 compilation

#CFLAGS  = /mf /oneatxm /zp4 /wx /fp5 /7 /v /5r /ou /fpi87 /bt=windows /i=$(INC) /i=$(INC2)

.cc.obj :
         $(CCP) $(CFLAGS) $*.cc

.c.obj :
        $(CC) $(CFLAGS) $*.c

F1=AREA.OBJ        &
   ATTRIBUT.OBJ    &
   AUTOMAP.OBJ     &
   BARRE.OBJ       &
   BEZIER.OBJ      &
   BINFILE.OBJ     &
   BLOB.OBJ        &
   BOUTON.OBJ      &
   CAMERA.OBJ      &
   CASE.OBJ        &
   CLAVIER.OBJ     &
   CLIP.OBJ        &
   CSG.OBJ         &
   CYLLIGHT.OBJ    &
   DISPLAY.OBJ     &
   DITHER.OBJ      &
   DONNEES1.OBJ    &
   DONNEES2.OBJ    &
   DONNEES3.OBJ    &
   EXTRUDE.OBJ     &
   FICHIER.OBJ     &
   FLARE.OBJ       &
   FONCTION.OBJ    &
   GCONVERT.OBJ    &
   GIF.OBJ         &
   GLIB.OBJ        &
   HFIELD.OBJ      &
   ICONE.OBJ       &
   ICONE2.OBJ      &
   INTERFA.OBJ     &
   LANGUE.OBJ      &
   LIB.OBJ         &
   LISSE.OBJ       &
   LUMIERE.OBJ     &
   MAPPING.OBJ     &
   MATIERE.OBJ     &
   MATRICE.OBJ     &
   MENU.OBJ        &
   MODELEUR.OBJ    &
   NEWCODE.OBJ     &
   OBJET.OBJ       &
   OBJET2.OBJ      &
   OMNI.OBJ        &
   OUTIL.OBJ       &
   PALETTE.OBJ     &
   PASTILLE.OBJ    &
   PLUGINS.OBJ     &
   POLICE.OBJ      &
   POTAR.OBJ       &
   POVRAY.OBJ      &
   PREFEREN.OBJ    &
   RENDER.OBJ      &
   RGB_HSL.OBJ     &
   SELCTEUR.OBJ    &
   SELC_OBJ.OBJ    &
   SELECT.OBJ      &
   SOURIS.OBJ      &
   SPECIAL.OBJ     &
   SPLINE.OBJ      &
   SPOT.OBJ        &
   TARGA.OBJ       &
   TRIANGLE.OBJ    &
   VECTEUR.OBJ     &
   VERSION.OBJ     &
   ZONETEXT.OBJ    &
   BUILD.OBJ       &
   FONT3D.OBJ      &
   GEOMETRY.OBJ    &
   TRUETYPE.OBJ    &
   OUTPUT.OBJ      &
   PARSE.OBJ

POVLABOBJS = $(F1)
 
MODELEUR.EXE : $(POVLABOBJS)
               $(LINKER) @LINKFILE.

AREA.OBJ:     AREA.C $(H)
ATTRIBUT.OBJ: ATTRIBUT.C $(H)
AUTOMAP.OBJ:  AUTOMAP.C $(H)
BARRE.OBJ:    BARRE.C $(H)
BEZIER.OBJ:   BEZIER.C $(H)
BINFILE.OBJ:  BINFILE.C $(H)
BLOB.OBJ:     BLOB.C $(H)
BOUTON.OBJ:   BOUTON.C $(H)
CAMERA.OBJ:   CAMERA.C $(H)
CASE.OBJ:     CASE.C $(H)
CLAVIER.OBJ:  CLAVIER.C $(H)
CLIP.OBJ:     CLIP.C $(H)
CSG.OBJ:      CSG.C $(H)
CYLLIGHT.OBJ: CYLLIGHT.C $(H)
DITHER.OBJ:   DITHER.C $(H)
DISPLAY.OBJ:  DISPLAY.C $(H)
DONNEES1.OBJ: DONNEES1.C $(H)
DONNEES2.OBJ: DONNEES2.C $(H)
DONNEES3.OBJ: DONNEES3.C $(H)
EXTRUDE.OBJ:  EXTRUDE.C $(H)
FICHIER.OBJ:  FICHIER.C $(H)
FLARE.OBJ:    FLARE.C $(H)
FONCTION.OBJ: FONCTION.C $(H)
GCONVERT.OBJ: GCONVERT.C $(H)
GIF.OBJ:      GIF.C $(H)
GLIB.OBJ:     GLIB.C $(H)
HFIELD.OBJ:   HFIELD.C $(H)
ICONE.OBJ:    ICONE.C $(H)
ICONE2.OBJ:   ICONE2.C $(H)
INTERFA.OBJ:  INTERFA.C $(H)
LANGUE.OBJ:   LANGUE.C $(H)
LIB.OBJ:      LIB.C $(H)
LISSE.OBJ:    LISSE.C $(H)
LUMIERE.OBJ:  LUMIERE.C $(H)
MAPPING.OBJ:  MAPPING.C $(H)
MATIERE.OBJ:  MATIERE.C $(H)
MATRICE.OBJ:  MATRICE.C $(H)
MENU.OBJ:     MENU.C $(H)
MODELEUR.OBJ: MODELEUR.C $(H) WINDOWS.C
NEWCODE.OBJ:  NEWCODE.C $(H)
OBJET.OBJ:    OBJET.C $(H)
OBJET2.OBJ:   OBJET2.C $(H)
OMNI.OBJ:     OMNI.C $(H)
OUTIL.OBJ:    OUTIL.C $(H)
PALETTE.OBJ:  PALETTE.C $(H)
PASTILLE.OBJ: PASTILLE.C $(H)
PLUGINS.OBJ:  PLUGINS.C $(H)
POLICE.OBJ:   POLICE.C $(H)
POTAR.OBJ:    POTAR.C $(H)
POVRAY.OBJ:   POVRAY.C $(H)
PREFEREN.OBJ: PREFEREN.C $(H)
RENDER.OBJ:   RENDER.C $(H)
RGB_HSL.OBJ:  RGB_HSL.C $(H)
SELCTEUR.OBJ: SELCTEUR.C $(H)
SELC_OBJ.OBJ: SELC_OBJ.C $(H)
SELECT.OBJ:   SELECT.C $(H)
SOURIS.OBJ:   SOURIS.C $(H)
SPECIAL.OBJ:  SPECIAL.C $(H)
SPLINE.OBJ:   SPLINE.C $(H)
SPOT.OBJ:     SPOT.C $(H)
TARGA.OBJ:    TARGA.C $(H)
TRIANGLE.OBJ: TRIANGLE.C $(H)
VECTEUR.OBJ:  VECTEUR.C $(H)
VERSION.OBJ:  VERSION.C $(H)
ZONETEXT.OBJ: ZONETEXT.C $(H)
BUILD.OBJ:    BUILD.CC $(HPP)
FONT3D.OBJ:   FONT3D.CC $(HPP)
GEOMETRY.OBJ: GEOMETRY.CC $(HPP)
TRUETYPE.OBJ: TRUETYPE.CC $(HPP)
OUTPUT.OBJ:   OUTPUT.CC $(HPP)
PARSE.OBJ:    PARSE.CC $(HPP)

