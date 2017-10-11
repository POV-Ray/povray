/* ---------------------------------------------------------------------------
*  SELC_OBJ.C
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
#include <STRING.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

typedef struct {
  char Nom[15];
  int Num;
  byte Modif;
} OBJET_TMP;

// --------------------------------------------------------------------------
// ---------------------  SELECTION NOM OBJETS DANS FENETRE -----------------
// --------------------------------------------------------------------------
int choix_nom_objet(int XG,int YG) {
  OBJET_TMP *NomObjet[NB_OBJET_MAX];
  OBJET_TMP Temp;
  int Touche=NULLC;
  int NumeroFIC=0;
  int EnCours;
  int PosiF=1;
  int MaxNb;
  int k,j,i,li;
  int MSY,MSX;
  int boucle=0;
  int XT=largeur_text("M");
  int YT=hauteur_text("G");
  int XD=5;
  int YD=15;
  int AncienNumeroFic=-1;
  int HY; // hauteur ascenceur;
  int Long=15;
  int NARG=-1;
  int Nb=12;
  int FX1=XD+XG+7;
  int FY1=YD+YG+20;
  int FX2=XD+XG+(XT*Long)+6;
  int FY2=YD+YG+20+Nb*(YT+2);
  int FY3=FY2+100;

  // ------------ Pr‚pare les objets

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->Buffer) {
      NARG++;
      NomObjet[NARG]=(OBJET_TMP *) mem_alloc(sizeof(OBJET_TMP));
      strcpy(NomObjet[NARG]->Nom,"  ");
      strcpy(NomObjet[NARG]->Nom+2,Objet[i]->Nom);
      NomObjet[NARG]->Num=i;
      NomObjet[NARG]->Modif=0;
    }
    Objet[i]->Buffer=0;
  }

  if (NARG==-1) return 0;
  NARG++;

  // ------------ Tri les noms d'objets

  for (i=NARG-1;i>=0;i--) {
    for (j=(i-1);j>=0;j--) {
      k=memcmp(NomObjet[j],NomObjet[i],15);
      if (k>0) {
        memcpy(&Temp,NomObjet[j],sizeof(OBJET_TMP));
        memcpy(NomObjet[j],NomObjet[i],sizeof(OBJET_TMP));
        memcpy(NomObjet[i],&Temp,sizeof(OBJET_TMP));
      }
    }
  }

  // ------------ Pr‚pare la fenˆtre

  HY=FY2-FY1;

  g_fenetre(XG,YG,FX2+30,FY3,"SELECT BY NAME",AFFICHE); // fenˆtre
  windows(FX1,FY1,FX2+2,FY2,0,ZFOND); // fond items
  windows(FX2+7,FY1,FX2+21,FY2,0,FOND); // barre ascenceur;
  init_texte(0,FX1+largeur_text("Names")+5,FY2+14,"Names","",13,"Manual selection");
  place_zone_texte(0);
  message("Select with mouse or enter names (wildcards ? and * valid)");
  init_bouton(40,FX1+45,FY3-30,60,20,"Ok",CENTRE,ATTEND,"Select");
  init_bouton(41,XG+10,FY3-60,60,20,"All",CENTRE,ATTEND,"Select all");
  init_bouton(42,FX2+30-70,FY3-60,60,20,"None",CENTRE,ATTEND,"Select none");
  affiche_bouton(40);
  affiche_bouton(41);
  affiche_bouton(42);

  Nb=(NARG<Nb ? NARG:Nb);
  MaxNb=Nb;
  if (NARG<=Nb) boucle=1;

  // ------------ ‚crit barre d‚part -----

  SELECTF1:

  if (NumeroFIC>(NARG-MaxNb)) NumeroFIC=(NARG-MaxNb);
  if (NumeroFIC<0) NumeroFIC=0;
  EnCours=NumeroFIC+PosiF-1;
  if (EnCours<0) EnCours=0;

  if (AncienNumeroFic!=EnCours) {
    SELECTF2:
    j=(int) ((DBL) HY*((DBL) EnCours/(NARG-1)));
    j=(j<1 ? 1:j);
    if (j>1) windows(FX2+7,FY1,FX2+20,FY1+j-1,3,FOND); // barre d‚placement
    if (j<HY) g_rectangle(FX2+7,FY1+j,FX2+20,FY2-1,FOND,-1); // barre d‚placement

    for (j=NumeroFIC;j<=(NumeroFIC+MaxNb-1);j++) {
      if (j==EnCours) {
        g_print(FX1,FY1+(j-NumeroFIC)*(YT+2),BLANC,NOIR,0,NomObjet[j]->Nom,Long);
      } else {
        g_print(FX1,FY1+(j-NumeroFIC)*(YT+2),BLANC,ZFOND,1,NomObjet[j]->Nom,Long);
      }
      if (NomObjet[j]->Modif) {
        i=FY1+(j-NumeroFIC)*(YT+2)+4;
        relief(FX1+2,i,FX1+6,i+4,0);
      }
    }
  }

  SELECTF3:

  AncienNumeroFic=EnCours;
  
  while (!kbhit()) {

    i=test_bouton(40,42);

    if (i==41) { // --------------------------------------- S‚lectionne tout
      for (i=0;i<NARG;i++) NomObjet[i]->Modif=1;
      goto SELECTF2;
    }

    if (i==42) {  // -------------------------------------- D‚s‚lectionne tout
      for (i=0;i<NARG;i++) NomObjet[i]->Modif=0;
      goto SELECTF2;
    }

    if (test_texte(0,0)==0) { // ------------------------- S‚lection manuelle
      for (i=0;i<NARG;i++) {
        if (match(NomObjet[i]->Nom+2,ZTexte[0].Variable)) {
          NomObjet[i]->Modif=1;
        }
      }
      goto SELECTF2;
    }

    if (i==40) {
      ungetch(13);
    }

    // ------- d‚placement … la souris (ascenceur)
    GMouseOn();
    MSX=gmx_v();
    MSY=gmy_v();
    if (MSX>FX2+8 && MSX<FX2+20 && MSY>FY1 && MSY<FY2 && MouseB()==1) {
      MSY=MSY-FY1;
      i=(int) ((DBL) ((DBL) MSY/HY)*(NARG-1));
      if (i!=li) {
        PosiF=1;  // --- on r‚cupŠre la valeur souris
        EnCours=NumeroFIC=i;
        li=i;
        goto SELECTF1;
      }
    }

    // ------- Choix dans fenˆtre avec souris
    if (MouseB()==1 && MSX>FX1 && MSX<FX2 && MSY>FY1 && MSY<FY1+MaxNb*(YT+2)) {
      GMouseOff();
      g_print(FX1,FY1+(PosiF-1)*(YT+2),BLANC,ZFOND,1,NomObjet[EnCours]->Nom,Long);

      for (j=FY1,i=0;j<=(FY2-YT-2);j+=YT+2,i++) {
        if (MSY>=j && MSY<=j+YT+2) break;
      }
      PosiF=i+1;  // --- on r‚cupŠre la valeur souris
      EnCours=(NumeroFIC+i);
      if (NomObjet[EnCours]->Modif) {
        NomObjet[EnCours]->Modif=0;
      } else {
        NomObjet[EnCours]->Modif=1;
      }
      g_print(FX1,FY1+(PosiF-1)*(YT+2),BLANC,NOIR,0,NomObjet[EnCours]->Nom,Long);
      GMouseOn();
      while (MouseB());
      goto SELECTF2;
    }

    if (MouseB()>1) { while (MouseB()); ungetch(27); }
  }

  Touche=getch();

  if (Touche==80) {                           // ---------- barre vers le bas
    if (PosiF==MaxNb) {
      if (boucle==1) goto PGUPF; else { NumeroFIC++; goto SELECTF1; }
    }
    PosiF++; goto SELECTF1;
  }

  if (Touche==72) {                          // ---------- barre vers le haut
    if (PosiF==1) {
      if (boucle==1) goto PGDNF; else { NumeroFIC--; goto SELECTF1; }
    }
    PosiF--; goto SELECTF1;
  }

  if (Touche==73) {                                 // ---------- PgUp
    PGUPF:
    if (EnCours-Nb>0) { NumeroFIC-=Nb; goto SELECTF1; }
    if (EnCours-Nb<=0) { PosiF=1; NumeroFIC=0; goto SELECTF1; }
  }

  if (Touche==81) {                                   // ---------- PgDn
    PGDNF:
    if (EnCours+Nb<NARG) { NumeroFIC+=Nb; goto SELECTF1; }
    if (EnCours+Nb>=NARG) { PosiF=Nb; NumeroFIC=NARG; goto SELECTF1; }
  }

  if (Touche==32) {                                   // ---------- Espace
    if (NomObjet[EnCours]->Modif) {
      NomObjet[EnCours]->Modif=0;
    } else {
      NomObjet[EnCours]->Modif=1;
    }
    ungetch(80);
    goto SELECTF1;
  }

  if (Touche==13) {
    for (i=0;i<NARG;i++) {
      if (NomObjet[i]->Modif) Objet[NomObjet[i]->Num]->Buffer=1;
      mem_free((OBJET_TMP *) NomObjet[i],sizeof(OBJET_TMP));
    }
    g_fenetre(XG,YG,0,0,NULL,EFFACE);
    return 1;
  }

  if (Touche==27) {
    for (i=0;i<NARG;i++) mem_free((OBJET_TMP *) NomObjet[i],sizeof(OBJET_TMP));
    g_fenetre(XG,YG,0,0,NULL,EFFACE);
    return 0;
  }

  goto SELECTF3;
}
