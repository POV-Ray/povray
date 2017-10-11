/* ---------------------------------------------------------------------------
*  OBJET2.C
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
#include <FLOAT.H>
#include <MALLOC.H>
#include <STRING.H>
#include <IO.H>
#include <DOS.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"

#define MAX_EXTERN_OBJ 200

// -------------------------------------------------------------------------
// -- LECTURE DU FICHIER DE DESCRIPTION D'UN OBJET EXTERNE -----------------
// -------------------------------------------------------------------------
void lecture_objet_externe(int Type) {
  int X1=CentX-155;
  int X2=CentX+155;
  int Y1=CentY-125;
  int Y2=CentY+125;
  FILE *Fichier;
  char Buffer[256];
  register int i,j;
  int NbM=-1;
  int Vu=-1,N,X,Y;
  char *Image;
  struct retour_selecteur Val;
  char *GifObj[MAX_EXTERN_OBJ];
  char *NomObj[MAX_EXTERN_OBJ];
  char *ParObj[MAX_EXTERN_OBJ];

  switch (Type) {
    case SUPEREL:
      strcpy(Buffer,"OBJECTS\\SUPERELL.PLB");
      break;
  }

  message("Reading objet description file");

  if ((Fichier=fopen(Buffer,"rt"))==NULL) {
    f_erreur("Can't open the file %s",Buffer);
    return;
  }

  while (!feof(Fichier)) {
    fgets(Buffer,256,Fichier);
    if (feof(Fichier)) break;
    if (Buffer[0]!=32 && Buffer[0]!=NULLC && Buffer[0]!='\n') {
      analyse_ligne(Buffer,32);
      NbM++;
      NomObj[NbM]=(char *) NULL;
      GifObj[NbM]=(char *) NULL;
      ParObj[NbM]=(char *) NULL;
      NomObj[NbM]=(char *) malloc(strlen(Argu[0])+1);
      GifObj[NbM]=(char *) malloc(strlen(Argu[1])+1);
      ParObj[NbM]=(char *) malloc(strlen(Argu[2])+1);
      strcpy(NomObj[NbM],Argu[0]);
      strcpy(GifObj[NbM],Argu[1]);
      strcpy(ParObj[NbM],Argu[2]);
      if (NbM==MAX_EXTERN_OBJ-1) { break; }
    }
  }

  fclose(Fichier);
  sprintf(Buffer,"%d objects loaded",NbM+1);
  message_aide(0,YMenuD+7,JAUNE,13,Buffer,MODIF);

  g_fenetre(X1,Y1,X2,Y2,"OBJECTS SELECTOR",AFFICHE);
  windows(X1+10,Y2-18,X2-10,Y2-5,0,ZFOND);  // fichier include
  bouton_dialog(X1,X2,Y2-20,1,0);

  init_selecteur(0,X1,Y1,12,NbM+1,NomObj,18);
  affiche_selecteur(0);
  text_xy(X1+12,Y2-18,Buffer,BLANC);

  windows(X2-110,Y1+35,X2-10,Y1+135,0,FOND); // cadre GIF

  while (1) {
    Val=test_selecteur(0);
    N=Val.Num;
    if ((i=bouton_dialog(X1,X2,Y2-20,0,0))!=-1) break;
    if (NbCouleurs==256 && Vu!=N) {
      message("%s",ParObj[N]);
      if (test_fichier(GifObj[N]) &&
          decompresse_gif(&X,&Y,GifObj[N],"",1)) {
        Image=(char *) malloc((size_t) 6+X*Y);
        decompresse_gif(&X,&Y,GifObj[N],Image,0);
        GMouseOff();
        affiche_gif(X2-110,Y1+35,X,Y,Image,1,1,0);
        GMouseOn();
        free(Image);
      } else {
        GMouseOff();
        affiche_icone(X2-110,Y1+35,0,PasDeTexture);
        GMouseOn();
      }
      Vu=N;
    }
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    switch (Type) {
      case SUPEREL:
        new_objet(SUPEREL,1);
        analyse_ligne(ParObj[N],',');
        Objet[NbObjet]->P[_X]=atof(Argu[0]);
        Objet[NbObjet]->P[_Y]=atof(Argu[1]);
        break;
    }
  }

  for (j=0;j<=NbM;j++) {
    if (NomObj[j]) free(NomObj[j]);
    if (ParObj[j]) free(ParObj[j]);
    if (GifObj[j]) free(GifObj[j]);
  }
}
