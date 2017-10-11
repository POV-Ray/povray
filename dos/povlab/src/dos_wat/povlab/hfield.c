/* ---------------------------------------------------------------------------
*  HFIELD.C
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
#include <STDLIB.H>
#include <STDIO.H>
#include <STRING.H>
#include <DOS.H>
#include <TIME.H>
#include <GRAPH.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

int MailleHF=20;

// -----------------------------------------------------------------------
// -- LECTURE D'UN HEIGHT-FIELD GIF/TGA ----------------------------------
// -----------------------------------------------------------------------
byte lecture_hfield(char *Nom,byte NewObj) {
  char NomFichier[MAXPATH];
  int i,Ok=0,Nb;
  IMAGE *ImgTGA;
  char *ImgGIF;
  int Xi,Yi,X,Y;
  byte Image=0;
  int PX;
  int PY;
  DBL X1,Y1,Z1,X2,Y2,Z2,X3,Y3,Z3,X4,Y4,Z4;
  char *Fichier;
  char *Spec[3]={"2","*.GIF","*.TGA"};
  DBL MaxiY=0;

  if (NbObjet) {
    if (NbPoly<Objet[NbObjet]->Poly && NewObj==0) {
      Poly[Objet[NbObjet]->Poly]->Buffer=1;
    } else {
      if (Poly[Objet[NbObjet]->Poly]->Buffer && NewObj==0) return 1;
    }
  }

  LECTURE_ERREUR_HF:

  if (Nom[0]=='?') {
    Fichier=selection_fichier(100,100,"GIF-TGA HF FILE",Spec);
    strcpy(NomFichier,Fichier);
    if (NomFichier[0]==27) return 0;
  } else {
    strcpy(NomFichier,Nom);
  }

  if (!test_fichier(NomFichier)) {
    strcpy(StrBoite[0],"READING HF FILE");
    sprintf(StrBoite[1],"Can't find hf file %s",NomFichier);
    strcpy(StrBoite[2],"\"Yes\" to specify another path,");
    strcpy(StrBoite[3],"\"No\" to delete object...");
    if (g_boite_ONA(CentX,CentY,3,CENTRE,1)) return 0;
    strcpy(Nom,"?");
    goto LECTURE_ERREUR_HF;
  }

  if (strinstr(0,NomFichier,".GIF")>=0) { // ------------ Lecture GIF
    Image=GIF;
    i=decompresse_gif(&Xi,&Yi,NomFichier,"",1);
    if (i==0) {
      f_erreur("Can't read file %s for height-field",NomFichier);
      return 0;
    }
    if ((ImgGIF=(char *) mem_alloc((size_t) Xi*Yi))==NULL) {
      f_erreur("No memory for height-field [GIF]");
      return 0;
    }
    i=decompresse_gif(&Xi,&Yi,NomFichier,ImgGIF,0);
    if (i==0) {
      f_erreur("Error reading file %s for height-field",NomFichier);
      mem_free(ImgGIF,Xi*Yi);
      return 0;
    }
    Ok=1;
  }

  if (strinstr(0,NomFichier,".TGA")>=0) { // ------------ Lecture TGA
    if ((ImgTGA=(IMAGE *) mem_alloc(sizeof(IMAGE)))==NULL) {
      f_erreur("No memory for height-field [TGA]");
      return 0;
    }
    Image=TGA;
    i=lecture_targa(ImgTGA,NomFichier);
    if (i==0) {
      mem_free(ImgTGA,sizeof(IMAGE));
      return 0;
    }
    Ok=1;
    Xi=(int) ImgTGA->width;
    Yi=(int) ImgTGA->height;
  }

  if (!Image) f_erreur("Need GIF ou TGA file format !");

  if (!Ok) {
    f_erreur("Can't open file %s for height-field",NomFichier);
    return 0;
  }

  if (Ok && NewObj) if (!new_objet(HFIELD,0)) Ok=-1;
  if (Ok && !new_volume_polygone()) Ok=-2;

  if (Ok>0) {
    strcpy(Objet[NbObjet]->CheminRaw,strupr(NomFichier));
    message("Reading file %s (%dx%d)",NomFichier,Xi,Yi);

    Nb=0;
    PX=(int) Xi/MailleHF;
    PY=(int) Yi/MailleHF;

    for (X=PX;X<=(Xi-PX-1);X+=PX) {
      for (Y=PY;Y<=(Yi-PY-1);Y+=PY) {

        if (Nb>NB_TRIANGLE_MAX) {
          f_erreur("To many triangles in the file !|"\
                   "Limited to %d triangles.",NB_TRIANGLE_MAX);
          free_mem_poly(NbPoly);
          Ok=-1;
          break;
        }

        if (Image==GIF) {
          i=X+(Y*Xi);
          Y1=(DBL) ImgGIF[i]/255;
          i=X+PX+(Y*Xi);
          Y2=(DBL) ImgGIF[i]/255;
          i=X+(Y+PY)*Xi;
          Y3=(DBL) ImgGIF[i]/255;
          i=X+PX+(Y+PY)*Xi;
          Y4=(DBL) ImgGIF[i]/255;
        }

        if (Image==TGA) {
          if (ImgTGA->Colour_Map==NULL) {
            i=ImgTGA->data.rgb_lines[Y].red[X];
            i+=ImgTGA->data.rgb_lines[Y].green[X];
            Y1=(DBL) (i/2)/255;
            i=ImgTGA->data.rgb_lines[Y].red[X+PX];
            i+=ImgTGA->data.rgb_lines[Y].green[X+PX];
            Y2=(DBL) (i/2)/255;
            i=ImgTGA->data.rgb_lines[Y+PY].red[X];
            i+=ImgTGA->data.rgb_lines[Y+PY].green[X];
            Y3=(DBL) (i/2)/255;
            i=ImgTGA->data.rgb_lines[Y+PY].red[X+PX];
            i+=ImgTGA->data.rgb_lines[Y+PY].green[X+PX];
            Y4=(DBL) (i/2)/255;
          } else {
            i=ImgTGA->data.map_lines[Y][X];
            Y1=(DBL) (i)/255;
            i=ImgTGA->data.map_lines[Y][X+PX];
            Y2=(DBL) (i)/255;
            i=ImgTGA->data.map_lines[Y+PY][X];
            Y3=(DBL) (i)/255;
            i=ImgTGA->data.map_lines[Y+PY][X+PX];
            Y4=(DBL) (i)/255;
          }
        }

        X1=(DBL) X/Xi;
        Z1=(DBL) Y/Yi;

        X2=(DBL) (X+PX)/Xi;
        Z2=(DBL) Y/Yi;

        X3=(DBL) X/Xi;
        Z3=(DBL) (Y+PY)/Yi;

        X4=(DBL) (X+PX)/Xi;
        Z4=(DBL) (Y+PY)/Yi;

        MaxiY=(Y1>MaxiY ? Y1:MaxiY);
        MaxiY=(Y2>MaxiY ? Y2:MaxiY);
        MaxiY=(Y3>MaxiY ? Y3:MaxiY);
        MaxiY=(Y4>MaxiY ? Y4:MaxiY);

        alloc_mem_triangle(X1,-Y1,-Z1,X2,-Y2,-Z2,X3,-Y3,-Z3,Nb); Nb++;
        alloc_mem_triangle(X2,-Y2,-Z2,X3,-Y3,-Z3,X4,-Y4,-Z4,Nb); Nb++;
      }
    }
  }

  if (NewObj) if (Ok==-1) { free_mem_objet(NbObjet); NbObjet--; }
  if (Ok==-2) free_mem_poly(NbPoly);

  if (Ok>0) {
    if (Image==GIF) mem_free(ImgGIF,Xi*Yi);
    if (Image==TGA) mem_free(ImgTGA,sizeof(IMAGE));
    adapte_polygone(Objet[NbObjet]->Poly,NbObjet,0);
    Objet[NbObjet]->HautHF=MaxiY;
    message("Nb triangles %d",Poly[NbPoly]->Nombre);
    affiche_donnees();
    if (NewObj) trace_volume_all(NbObjet,NbObjet);
    return 1;
  }

  return 0;
}

// --------------------------------------------------------------------------
// ----- DONNE LA HAUTEUR DU WATER_LEVEL ------------------------------------
// --------------------------------------------------------------------------
void water_level(int N) {
  int X1=CentX-95;
  int X2=CentX+95;
  int Y1=CentY-50;
  int Y2=CentY+50;
  int i=-1,j=-1;
  char WL[10];

  g_fenetre(X1,Y1,X2,Y2,"WATER LEVEL VALUE",AFFICHE);

  sprintf(WL,"%.4g",Objet[N]->WLevel);
  init_texte(0,X1+70,Y1+40,"Water level",WL,10,"Change the water level value");

  place_zone_texte(0);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    j=test_texte(0,0);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    Objet[N]->WLevel=atof(ZTexte[0].Variable);
  }

  forme_mouse(MS_FLECHE);
  affiche_donnees();
}
