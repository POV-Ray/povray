/* ---------------------------------------------------------------------------
*  BLOB.C
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

int GroupeEnCours=1;
DBL SeuilEnCours=0.85;
DBL ForceEnCours=1.0;

// ----------------------------------------------------------------------------
// -- PAS DE 2D SUR LES BLOBS -------------------------------------------------
// ----------------------------------------------------------------------------
void erreur_2D_blob(void) {
  f_erreur("Blob's shapes cannot be scaled along 2 axis.");
}

// ----------------------------------------------------------------------------
// -- RENVOI LE PROCHAIN NUMERO DE GROUPE ----------------------------------------
// ----------------------------------------------------------------------------
int groupe_suivant(int N) {
  register int i;
  DBL G=(DBL) N;

  for (i=1;i<=NbObjet;i++) {
    if ((Objet[i]->Type==BLOB || Objet[i]->Type==BLOBC) && Objet[i]->P[2]>G) {
        return ((int) Objet[i]->P[2]);
    }
  }

  return 0;
}

// ----------------------------------------------------------------------------
// -- RETOURNE LE GROUPE DE SEUIL S -------------------------------------------
// ----------------------------------------------------------------------------
int quel_groupe(DBL S) {
  register int i;

  for (i=1;i<=NbObjet;i++) {
    if ((Objet[i]->Type==BLOB || Objet[i]->Type==BLOBC) && Objet[i]->P[1]==S) return ((int)Objet[i]->P[2]);
  }

  return 0;
}

// ----------------------------------------------------------------------------
// -- RETOURNE LE SEUIL DU GROUPE G -------------------------------------------
// ----------------------------------------------------------------------------
DBL quel_seuil(int G) {
  register int i;

  for (i=1;i<=NbObjet;i++) {
    if ((Objet[i]->Type==BLOB || Objet[i]->Type==BLOBC) && Objet[i]->P[2]==G) return Objet[i]->P[1];
  }

  return 0;
}

// ----------------------------------------------------------------------------
// -- ENTREE FORCE ET SEUIL DES BLOBS -----------------------------------------
// ----------------------------------------------------------------------------
void force_seuil_groupe_blob(byte Cherche,int N) {
  int X1=CentX-95;
  int X2=CentX+95;
  int Y1=CentY-55;
  int Y2=CentY+80;
  char StrS[13],StrF[13],StrG[13];
  DBL F,S,G,S1;
  DBL F2,S2,G2;
  int i,j;

  if (pas_objet(1)) return;

  if (Cherche) {
    forme_mouse(MS_SELECTEUR);
    message("Select the component to modify. Current : Strength: %.2f Threshold: %.2f Group: %d",ForceEnCours,SeuilEnCours,GroupeEnCours);
    if ((N=trouve_volume(0,2,1))==FAUX) return;
  }

  F=Objet[N]->P[_X];
  S=Objet[N]->P[_Y];
  G=Objet[N]->P[_Z];

  forme_mouse(MS_FLECHE);

  g_fenetre(X1,Y1,X2,Y2,"BLOB'S OPTIONS",AFFICHE);

  sprintf(StrF,"%.6f",Objet[N]->P[0]);
  sprintf(StrS,"%.6f",Objet[N]->P[1]);
  sprintf(StrG,"%d",(int)Objet[N]->P[2]);

  init_texte(0,X1+75,Y1+35,"Group",StrG,10,"Group of component");
  init_texte(1,X1+75,Y1+55," Threshold",StrS,10,"Threshold of component");
  init_texte(2,X1+75,Y1+75," Strength",StrF,10,"Strength of blob");

  place_zone_texte(0);
  place_zone_texte(1);
  place_zone_texte(2);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    j=test_texte(0,2);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
  forme_mouse(MS_FLECHE);

  if (i==1) return;

  F2=atof(ZTexte[2].Variable);
  S2=atof(ZTexte[1].Variable);
  G2=atof(ZTexte[0].Variable);

  if (!G2) { f_erreur("Invalid group number (<=0)"); return; }
  if (!S2) { f_erreur("Invalid blob threshold value (<=0)"); return; }
  if (!F2) { f_erreur("Invalid component strength (=0)"); return; }

  if (S2!=S && G2==G) {
    if (!x_fenetre(CentX,CentY,GAUCHE,0,"THRESHOLD|Change threshold group, which was %.4f ?",S)) {
      for (i=1;i<=NbObjet;i++) {
        if ((Objet[i]->Type==BLOB || Objet[i]->Type==BLOBC) && Objet[i]->P[2]==G) {
          Objet[i]->P[1]=S2;
        }
      }
      i=0;
    } else {
      return;
    }
  }

  S1=quel_seuil((int) G2);

  if (G!=G2 && S1!=S2) {
    f_erreur("No correlation between threshold and group !");
    return;
  }

  if (i==0) {
    Objet[N]->P[_X]=ForceEnCours=F2;
    Objet[N]->P[_Y]=SeuilEnCours=S2;
    Objet[N]->P[_Z]=(DBL) (GroupeEnCours=G2);
  }
}

