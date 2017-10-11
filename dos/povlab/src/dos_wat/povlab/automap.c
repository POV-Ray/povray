/* ---------------------------------------------------------------------------
*  AUTOMAP.C
*
*
*
*  from POVLAB 3D Modeller
*  Copyright 1994-1999 POVLAB Authors.
*  ---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POVLAB and to port the software to platforms other
*  than those supported by the POVLAB authors. There are strict rules under
*  which you are permitted to use this file. The rules are in the file
*  named LEGAL.TXT which should be distributed with this file.
*  If LEGAL.TXT is not available or for more info please contact the POVLAB
*  primary author by leaving a message on http://www.povlab.org
*  The latest and official version of POVLAB may be found at this site.
*
*  POVLAB was originally written by Denis Olivier.
*
*  ---------------------------------------------------------------------------*/

#include <MATH.H>
#include <STDIO.H>
#include <IO.H>
#include <DOS.H>
#include <STDLIB.H>
#include <STRING.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"

// -------------------------------------------------------------------------
// -- MAKE A AUTOMAP OBJECT ------------------------------------------------
// -------------------------------------------------------------------------
void lecture_automap(void) {
  char *Spec[4]={"3","*.GIF","*.TGA","*.PNG"};
  char *Buffer=(char *) malloc(256);
  int X,Y;
  DBL T;

  strcpy(Buffer,selection_fichier(100,100,"MAP FILES",Spec));
  if (Buffer[0]==27) { free(Buffer); return; }

  if (new_objet(CUBE,2)) {
    extract_xy_mapping(Buffer,&X,&Y);
    T=(X>Y ? X:Y);
    Objet[NbObjet]->S[_X]=(DBL) X/T;
    Objet[NbObjet]->S[_Y]=(DBL) Y/T;
    Objet[NbObjet]->S[_Z]=0.001;
    Objet[NbObjet]->Map[0].On=1;
    Objet[NbObjet]->Map[0].Interpolate=1;
    split_chemin(Objet[NbObjet]->Map[0].Name,Buffer,5);
    trace_volume_all(NbObjet,NbObjet);
  }

  free(Buffer);
}
