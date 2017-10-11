/* ---------------------------------------------------------------------------
*  SELECTEUR.C
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
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include <TIME.H>
#include "LIB.H"
#include "GLOBAL.H"
#include "GLIB.H"

struct menu Mn[2];

// --------------------------------------------------------------------------
// --------------------- INITIALISATION SELECTION DANS FENETRE --------------
// --------------------------------------------------------------------------
void init_selecteur(byte N,int XG,int YG,int Nb,int NARG,char *Element[],int Long) {
  register int i=1;

  Mn[N].X=XG;
  Mn[N].Y=YG;
  Mn[N].Nb=Nb;
  Mn[N].NARG=NARG;
  for (i=0;i<NARG;i++) {
    strcpy(Mn[N].Element[i],Element[i]);
  }
  Mn[N].Long=Long;
  Mn[N].NumElem=0;
  Mn[N].NumElemNow=0;
  Mn[N].PosiF=1;
  Mn[N].OldNumElem=-1;
}

//--------------------------------------------------------------------------
// --------------------- INITIALISATION SELECTION DANS FENETRE --------------
// --------------------------------------------------------------------------
void affiche_selecteur(int N) {
  int XD=5;
  int YD=15;
  int XT=largeur_text("M");
  int YT=hauteur_text("G");
  int FX1=XD+Mn[N].X+7;
  int FY1=YD+Mn[N].Y+20;
  int FX2=XD+Mn[N].X+(XT*Mn[N].Long)+6;
  int FY2=YD+Mn[N].Y+20+Mn[N].Nb*(YT+2);

  windows(FX1,FY1,FX2+2,FY2,0,ZFOND); // fond ‚lements
  windows(FX2+7,FY1,FX2+21,FY2,0,FOND); // barre ascenceur;

  selecteur(N,AFFICHE);
}

//--------------------------------------------------------------------------
// --------------------- INITIALISATION SELECTION DANS FENETRE --------------
// --------------------------------------------------------------------------
struct retour_selecteur test_selecteur(int N) {
  return selecteur(N,0);
}

// --------------------------------------------------------------------------
// ---------------------  SELECTION DANS FENETRE FICHIER --------------------
// --------------------------------------------------------------------------
struct retour_selecteur selecteur(byte N,byte Affiche) {
  byte Touche=NULLC;
  int MaxNb;
  register int j,i,li;
  int MSY,MSX;
  int boucle=0;
  int XT=largeur_text("M");
  int YT=hauteur_text("G");
  int XD=5;
  int YD=15;
  int FX1=XD+Mn[N].X+7;
  int FY1=YD+Mn[N].Y+20;
  int FX2=XD+Mn[N].X+(XT*Mn[N].Long)+6;
  int FY2=YD+Mn[N].Y+20+Mn[N].Nb*(YT+2);
  int HY=FY2-FY1; // hauteur ascenceur;
  struct retour_selecteur Valeur;

  // Pr‚pare les variables

  Mn[N].Nb=(Mn[N].NARG<Mn[N].Nb ? Mn[N].NARG:Mn[N].Nb);
  MaxNb=Mn[N].Nb;
  if (Mn[N].NARG<=Mn[N].Nb) boucle=1;

  // ------------ ‚crit barre d‚part -----

  if (!Affiche) goto SELECTF2;
  SELECTF1:

  if (Mn[N].NumElem>(Mn[N].NARG-MaxNb)) Mn[N].NumElem=(Mn[N].NARG-MaxNb);
  if (Mn[N].NumElem<0) Mn[N].NumElem=0;
  Mn[N].NumElemNow=Mn[N].NumElem+Mn[N].PosiF-1;

  if (Mn[N].OldNumElem!=Mn[N].NumElemNow) {
    j=(int) ((DBL) HY*((DBL) Mn[N].NumElemNow/(Mn[N].NARG-1)));
    j=(j<1 ? 1:j);
    if (j>1) windows(FX2+7,FY1,FX2+20,FY1+j-1,3,FOND); // barre d‚placement
    if (j<HY) g_rectangle(FX2+7,FY1+j,FX2+20,FY2-1,FOND,-1); // barre d‚placement

    for (j=Mn[N].NumElem;j<=(Mn[N].NumElem+MaxNb-1);j++) {
      if (j==(Mn[N].NumElemNow)) {
        g_print(FX1,FY1+(j-Mn[N].NumElem)*(YT+2),BLANC,NOIR,0,Mn[N].Element[j],Mn[N].Long);
      } else {
        g_print(FX1,FY1+(j-Mn[N].NumElem)*(YT+2),BLANC,ZFOND,1,Mn[N].Element[j],Mn[N].Long);
      }
    }
  }

  if (Affiche) return Valeur;
  SELECTF2:

  Mn[N].OldNumElem=Mn[N].NumElemNow;
  
  // ------- d‚placement … la souris (ascenceur)
  GMouseOn();
  MSX=gmx_v();
  MSY=gmy_v();
  if (MSX>FX2+8 && MSX<FX2+20 && MSY>FY1 && MSY<FY2 && MouseB()==1) {
    MSY=MSY-FY1;
    i=(int) ((DBL) ((DBL) MSY/HY)*(Mn[N].NARG-1));
    if (i!=li) {
      Mn[N].PosiF=1;  // --- on r‚cupŠre la valeur souris
      Mn[N].NumElemNow=Mn[N].NumElem=i;
      li=i;
      goto SELECTF1;
    }
  }

  // ------- Choix dans fenˆtre avec souris
  if (MouseB()==1 && MSX>FX1 && MSX<FX2 && MSY>FY1 && MSY<FY1+MaxNb*(YT+2)) {
    GMouseOff();
    g_print(FX1,FY1+(Mn[N].PosiF-1)*(YT+2),BLANC,ZFOND,1,Mn[N].Element[Mn[N].NumElemNow],Mn[N].Long);
    for (j=FY1,i=0;j<=(FY2-YT-2);j+=YT+2,i++) {
      if (MSY>=j && MSY<=j+YT+2) break;
    }
    Mn[N].PosiF=i+1;  // --- on r‚cupŠre la valeur souris
    Mn[N].NumElemNow=(Mn[N].NumElem+i);
    if (Mn[N].OldNumElem==Mn[N].NumElemNow) {
      while (MouseB());
      Valeur.Num=(Mn[N].NumElemNow);
      Valeur.Ok=13;
      return Valeur;
    }
    while (MouseB());
    goto SELECTF1;
  }

  if (sequence_sortie()) ungetch(27);

  if (kbhit()) Touche=getch(); else Touche=NULLC;

  if (Touche==80) {                           // ---------- barre vers le bas
    //VersLeBasFich:
    if (Mn[N].PosiF==MaxNb) {
      if (boucle==1) goto PGUPF; else { Mn[N].NumElem++; goto SELECTF1; }
    }
    Mn[N].PosiF++; goto SELECTF1;
  }

  if (Touche==72) {                          // ---------- barre vers le haut
    //VersLeHautFich:
    if (Mn[N].PosiF==1) {
      if (boucle==1) goto PGDNF; else { Mn[N].NumElem--; goto SELECTF1; }
    }
    Mn[N].PosiF--; goto SELECTF1;
  }

  if (Touche==73) {                                 // ---------- PgUp
    PGUPF:
    if (Mn[N].NumElemNow-Mn[N].Nb>0) { Mn[N].NumElem-=Mn[N].Nb; goto SELECTF1; }
    if (Mn[N].NumElemNow-Mn[N].Nb<=0) { Mn[N].PosiF=1; Mn[N].NumElem=0; goto SELECTF1; }
  }

  if (Touche==81) {                                   // ---------- PgDn
    PGDNF:
    if (Mn[N].NumElemNow+Mn[N].Nb<Mn[N].NARG) { Mn[N].NumElem+=Mn[N].Nb; goto SELECTF1; }
    if (Mn[N].NumElemNow+Mn[N].Nb>=Mn[N].NARG) { Mn[N].PosiF=Mn[N].Nb; Mn[N].NumElem=Mn[N].NARG; goto SELECTF1; }
  }

  if (Touche==13) {
    Valeur.Num=(Mn[N].NumElemNow);
    Valeur.Ok=13;
    return Valeur;
  }

  if (Touche==27) {
    Valeur.Num=0;
    Valeur.Ok=27;
    return Valeur;
  }

  Valeur.Num=(Mn[N].NumElemNow);
  Valeur.Ok=0;
  return Valeur;

}
