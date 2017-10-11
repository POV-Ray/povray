/* ---------------------------------------------------------------------------
*  BOUTON.C
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
#include "VERSIONC.H"
#include <STDARG.H>
#include <DOS.H>
#include <TIME.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include <CONIO.H>
#include <GRAPH.H>
#include <I86.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

struct Bouton Bt[60];

// -----------------------------------------------------------------------
// -- AFFICHE UN BOUTON EN MODE GRAPHIQUE --------------------------------
// -----------------------------------------------------------------------
void dessine_bouton(int N,byte Status) {
  select_vue(5,CLIP_ON);
  g_rectangle(Bt[N].X,Bt[N].Y,Bt[N].X+Bt[N].L,Bt[N].Y+Bt[N].H-2,FOND,MOTIF);
  if (!Status) {
    relief(Bt[N].X,Bt[N].Y,Bt[N].X+Bt[N].L,Bt[N].Y+Bt[N].H-1,0);
    relief(Bt[N].X+1,Bt[N].Y+1,Bt[N].X+Bt[N].L-1,Bt[N].Y+Bt[N].H-2,0);
  } else {
    relief(Bt[N].X,Bt[N].Y,Bt[N].X+Bt[N].L,Bt[N].Y+Bt[N].H-1,1);
    relief(Bt[N].X+1,Bt[N].Y+1,Bt[N].X+Bt[N].L-1,Bt[N].Y+Bt[N].H-2,1);
  }
}

// -----------------------------------------------------------------------
// --------- AFFICHE UN BOUTON -------------------------------------------
// -----------------------------------------------------------------------
void affiche_bouton(int N) {
  register int X,Y;

  GMouseOff();
  dessine_bouton(N,0);

  if (Bt[N].Txt[0]=='#') return;

  switch (Bt[N].Place) {
    case GAUCHE:
      X=Bt[N].X+4;
      break;
    case CENTRE:
      X=Bt[N].X+Bt[N].L/2-largeur_text(Bt[N].Txt)/2;
      break;
    case DROITE:
      X=Bt[N].X+Bt[N].L-largeur_text(Bt[N].Txt)-5;
      break;
  }
  Y=Bt[N].Y+Bt[N].H/2-7;
  text_xy(X+1,Y,Bt[N].Txt,TEXTE);
}

// ----------------------------------------------------------------------- */
// --------- INITIALISE UN BOUTON ---------------------------------------- */
// ----------------------------------------------------------------------- */
void init_bouton(int Num,int X,int Y,int L,int H,char *String,byte Place,byte Click,char *Aide) {
  Bt[Num].X=X;
  Bt[Num].Y=Y;
  Bt[Num].L=L;
  Bt[Num].H=H;
  parse_ascii_watcom(String);
  strcpy(Bt[Num].Txt,strlwr(String));
  Bt[Num].Place=Place;
  Bt[Num].Click=Click;
  if (Aide[0]) {
    strcpy(Bt[Num].Aide,Aide);
  } else {
    strcpy(Bt[Num].Aide,String);
  }
}

// ----------------------------------------------------------------------- */
// --------- CLICK SUR UN BOUTON ----------------------------------------- */
// ----------------------------------------------------------------------- */
void click_bouton(int N) {
  GMouseOff();
  g_bitmap(Bt[N].X+3,Bt[N].Y+3,Bt[N].X+Bt[N].L-2,Bt[N].Y+Bt[N].H-3,SAUVE,0);
  // ----------------
  dessine_bouton(N,1);
  g_bitmap(Bt[N].X+2,Bt[N].Y+2,Bt[N].X+Bt[N].L,Bt[N].Y+Bt[N].H,AFFICHE,0);
  // ----------------
  if (Bt[N].Click==ATTEND) while (MouseB());
  affiche_bouton(N);
  g_bitmap(Bt[N].X+3,Bt[N].Y+3,Bt[N].X+Bt[N].L,Bt[N].Y+Bt[N].H-1,AFFICHE,0);
  g_bitmap(Bt[N].X+3,Bt[N].Y+3,Bt[N].X+Bt[N].L,Bt[N].Y+Bt[N].H-1,EFFACE,0);
  GMouseOn();
  log_out(0,"Button [%02d]: %s",N,Bt[N].Txt);
}

// ----------------------------------------------------------------------- */
// --------- TEST LES BOUTONS -------------------------------------------- */
// ----------------------------------------------------------------------- */
int test_bouton(int Debut,int Fin) {
  register int X=gmx_v();
  register int Y=gmy_v();
  int X1,Y1,X2,Y2;
  int N=-1,i;

  GMouseOn();
  
  if (MouseB()==1) {
    if (Debut<25) NF=trouve_fenetre(0);
    for (i=Debut;i<=Fin;i++) {
      X1=Bt[i].X; X2=Bt[i].X+Bt[i].L;
      Y1=Bt[i].Y; Y2=Bt[i].Y+Bt[i].H;
      if (X1<X && X<X2 && Y1<Y && Y<Y2) { N=i; break; }
    }
    if (N>-1) {
      click_bouton(N);
      return N;
    }
  } else {
    for (i=Debut;i<=Fin;i++) {
      X1=Bt[i].X; X2=Bt[i].X+Bt[i].L;
      Y1=Bt[i].Y; Y2=Bt[i].Y+Bt[i].H;
      if (X1<X && X<X2 && Y1<Y && Y<Y2) {
        forme_mouse(MS_FLECHE);
        message_aide(0,YMenuD+7,JAUNE,13,Bt[i].Aide,BOUTON);
        return -1;
      }
    }
  }
  return -1;
}

// ----------------------------------------------------------------------- */
// ----- AFFICHE DANS UNE FENETRE LES BOUTONS OUI/NON/ANNULER ------------ */
// ----------------------------------------------------------------------- */
int bouton_dialog(int X1,int X2,int Y,byte Travail,byte Nb) {
  register int i=-1,Key,XA,XB,XC;
  X1=X1;
  Y-=6;

  switch (Nb) {
    case 0:
      XA=X2-60;
      break;
    case 1:
      XA=X2-125;
      XB=X2-60;
      break;
    case 2:
      XA=X2-190;
      XB=X2-125;
      XC=X2-60;
      break;
  }

  if (Travail==1) {
    init_bouton(50,XA,Y-23,50,20,RES_BOUT[8],CENTRE,ATTEND,RES_AIDE[30]);
    affiche_bouton(50);
    if (Nb!=0) {
      init_bouton(51,XB,Y-23,50,20,RES_BOUT[9],CENTRE,ATTEND,RES_AIDE[31]);
      affiche_bouton(51);
    }
    if (Nb==2) {
      init_bouton(52,XC,Y-23,50,20,RES_BOUT[10],CENTRE,ATTEND,RES_AIDE[32]);
      affiche_bouton(52);
    }
  }

  if (Travail==0) {
    GMouseOn();
    switch (Nb) {
      case 0: i=test_bouton(50,50); break;
      case 1: i=test_bouton(50,51); break;
      case 2: i=test_bouton(50,52); break;
    }
    if (i==50) return 0;
    if (i==51) return 1;
    if (i==52) return 2;
    if (sequence_sortie()) return 2;
    if (kbhit()) {
      Key=getch();
      if (Key==13) return 0;
      ungetch(Key);
    }
  }
  return -1;
}
