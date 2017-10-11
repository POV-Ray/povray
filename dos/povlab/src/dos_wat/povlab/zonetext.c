/* ---------------------------------------------------------------------------
*  ZONETEXT.C
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
#include <MATH.H>
#include <FLOAT.H>
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

struct ZoneDeTexte ZTexte[20];

// -----------------------------------------------------------------------
// --------- TESTE UNE ZONE DE TEXTE -------------------------------------
// -----------------------------------------------------------------------
int test_texte(int Debut,int Fin) {
  register int X=gmx_v(),X1,Y1;
  register int Y=gmy_v(),X2,Y2;
  register int N=-1,i,Tab=0,Key;
  register int XL=largeur_text("M");
  register int YH=hauteur_text("G");
  static int NbTab=-1,DebutTab=-1,FinTab=-1;

  GMouseOn();
  
  if (DebutTab!=Debut || FinTab!=Fin) {
    DebutTab=Debut;
    FinTab=Fin;
    NbTab=Debut-1;
  }

  if (kbhit()) {
    if ((Key=getch())==9) {
      Tab=1; NbTab++;
      if (NbTab>FinTab) NbTab=Debut;
    } else {
      ungetch(Key);
    }
  }

  if (MouseB()==1 || Tab) {
    for (i=Debut;i<=Fin;i++) {
      X1=ZTexte[i].X; X2=ZTexte[i].X+ZTexte[i].Long*XL;
      Y1=ZTexte[i].Y; Y2=ZTexte[i].Y+YH+2;
      if ((X1<X && X<X2 && Y1<Y && Y<Y2 && !Tab) || (Tab && NbTab==i)) { NbTab=N=i; break; }
    }
    if (N>-1) { return (zone_texte(i)==13 ? N:-1); }
  } else {
    for (i=Debut;i<=Fin;i++) {
      X1=ZTexte[i].X; X2=ZTexte[i].X+ZTexte[i].Long*XL;
      Y1=ZTexte[i].Y; Y2=ZTexte[i].Y+YH+2;
      if (X1<X && X<X2 && Y1<Y && Y<Y2) {
        message_aide(0,YMenuD+7,JAUNE,13,ZTexte[i].Aide,ZONETEXTE);
        return -1;
      }
    }
  }
  return -1;
}

// ----------------------------------------------------------------------- */
// --------- INITIALISE UNE ZONE DE TEXTE -------------------------------- */
// ----------------------------------------------------------------------- */
void init_texte(int Num,int X,int Y,char *String,char *Variable,byte Long,char *Aide) {
  ZTexte[Num].X=X;
  ZTexte[Num].Y=Y;
  ZTexte[Num].Long=Long;
  parse_ascii_watcom(String);
  parse_ascii_watcom(Variable);
  strcpy(ZTexte[Num].Txt,String);
  strcpy(ZTexte[Num].Variable,Variable);
  if (Aide[0]) {
    strcpy(ZTexte[Num].Aide,Aide);
  } else {
    strcpy(ZTexte[Num].Aide,String);
  }
}

// ----------------------------------------------------------------------- */
// --------- AFFICHE UN ZONE DE TEXTE ------------------------------------ */
// ----------------------------------------------------------------------- */
void place_zone_texte(int Num) {
  byte Long=ZTexte[Num].Long;
  byte LT=largeur_text(ZTexte[Num].Txt);
  int X1,X2,Y1,Y2;

  text_xy(ZTexte[Num].X-LT-5,ZTexte[Num].Y,ZTexte[Num].Txt,15);

  X1=ZTexte[Num].X;
  Y1=ZTexte[Num].Y;
  X2=ZTexte[Num].X+largeur_text("M")*Long+3;
  Y2=ZTexte[Num].Y+hauteur_text("G")+2;

  g_rectangle(X1,Y1,X2-1,Y2-1,ZFOND,MOTIF);
  g_ligne(X1,Y2,X2,Y2,ECLAIRE);
  g_ligne(X2,Y1-1,X2,Y2,ECLAIRE);
  g_ligne(X1,Y1-1,X2-1,Y1-1,NOIR);
  g_ligne(X1-1,Y1-1,X1-1,Y2-1,NOIR);
  g_ligne(X1-1,Y1-2,X2,Y1-2,OMBRE);
  g_ligne(X1-2,Y1-2,X1-2,Y2,OMBRE);

  set_port(ZTexte[Num].X,
          ZTexte[Num].Y,
          ZTexte[Num].X+largeur_text("M")*Long+2,
          ZTexte[Num].Y+hauteur_text("G")+2);

  text_xy(2+ZTexte[Num].X,ZTexte[Num].Y,ZTexte[Num].Variable,15);
  set_port(0,0,XMax,YMax);
}

// ----------------------------------------------------------------------------
// --------- ZONE DE SAISIE DE TEXTE ------------------------------------------
// ----------------------------------------------------------------------------
int zone_texte(byte Num) {
  char Variable[128];
  char Buffer1[128];
  byte LT;
  byte Touche;
  int NumCHAR=0;
  byte EnCours;
  byte PosiF=1;
  byte MaxNb;
  int j,i;
  int XT=largeur_text("M");
  int YT=hauteur_text("G");
  byte Flag;
  int Long=ZTexte[Num].Long;
  int X=ZTexte[Num].X;
  int Y=ZTexte[Num].Y;
  int Depasse=X+XT*Long+2;
  int XC;

  MaxNb=80;

  strcpy(Variable,ZTexte[Num].Variable);
  strcat(Variable," ");

  GMouseOff();
  affiche_donnees();

  // -------------------------------------

  SELECTF1:

  efface_buffer_clavier();
  LT=strlen(Variable);

  //if (NumCHAR>(LT-MaxNb)) NumCHAR=(LT-MaxNb);
  if (NumCHAR<0) NumCHAR=0;
  EnCours=NumCHAR+PosiF-1;

  //Message("Text=%25s PosiF=%2d EnCours=%2d NumChar=%2d LT=%d",Variable,PosiF,EnCours,NumCHAR,LT);

  g_rectangle(X,Y,X+XT*Long+2,Y+YT+1,ZFOND,-1);

  set_port(X,Y,X+largeur_text("M")*Long+2,Y+hauteur_text("G")+2);
  for (j=NumCHAR,i=0;j<=(NumCHAR+MaxNb-1);j++) {
    if (Variable[j]==0) break;
    Buffer1[0]=Variable[j];
    Buffer1[1]=NULLC;
    if (j==(EnCours)) {
      g_ligne(X+i+2,Y,X+i+2,Y+YT+1,1);
      text_xy(X+i+2,Y,Buffer1,15);
      XC=i;
    } else {
      text_xy(X+i+2,Y,Buffer1,15);
    }
    i+=largeur_text(Buffer1);
  }
  set_port(0,0,XMax,YMax);

  //message("XC=%d Depasse=%d XC+X=%d NumCHAR=%d",XC,Depasse,X+XC,NumCHAR);

  SELECTF2:

  while (!kbhit());

  Flag=test_touche();
  if (Flag==1) Touche=getch(); else Touche=Flag;

  if (Touche==77 && Flag==1 && EnCours<LT-1) {  // ---- curseur vers la droite
    CurseurDroite:
    if ((XC+X)>=(Depasse-10)) {
      NumCHAR++;
      goto SELECTF1;
    }
    PosiF++; goto SELECTF1;
  }

  if (Touche==75 && Flag==1) {      // ---------- curseur vers la gauche
    CurseurGauche:
    if (PosiF==1) {
      NumCHAR--;
      goto SELECTF1;
    }
    PosiF--; goto SELECTF1;
  }

  if (Touche==71 && Flag==1) {                         // ---------- D‚but
    PosiF=1;
    NumCHAR=0;
    goto SELECTF1;
  }

  if (Touche==79 && Flag==1) {                         // ---------- Fin
    PosiF=(LT<=Long ? LT:Long);
    NumCHAR=(LT<=Long ? 0:LT-Long);
    goto SELECTF1;
  }

  if (Touche==8 && Flag==8) {         // ---------- backspace
    if (EnCours>=1) {
      for (j=0,i=0;j<LT;j++,i++) {
        if (i==EnCours-1) i++;
        Variable[j]=Variable[i];
      }
      Variable[j]=NULLC;
      if (EnCours>=1) goto CurseurGauche;
    }
    efface_buffer_clavier();
    Touche=Flag=NULLC;
    goto SELECTF1;
  }

  if (Touche==83 && Flag==1 && EnCours<LT-1) {       // ---------- suppression
    for (j=0,i=0;j<=LT;j++,i++) {
      if (i==EnCours) i++;
      Variable[j]=Variable[i];
    }
    Variable[j-1]=NULLC;
    Touche=NULLC;
    if (strlen(Variable)-MaxNb<NumCHAR) goto CurseurDroite;
    goto SELECTF1;
  }

  if (Touche==13 || Touche==9) {
    if (Variable[strlen(Variable)-1]==32) Variable[strlen(Variable)-1]=NULLC;
    strcpy(ZTexte[Num].Variable,Variable);
    place_zone_texte(Num);
    if (Touche==9) { ungetch(9); return 0; }
    return 13;
  }

  if (Touche==27) {
    place_zone_texte(Num);
    return 27;
  }

  if (Touche==Flag) {       // -------- ajout lettres
    for (j=LT+1;j>=EnCours;j--) {
      Variable[j]=Variable[j-1];
    }
    Variable[EnCours]=Touche;
    parse_ascii_watcom(Variable);
    goto CurseurDroite;
  }
  goto SELECTF2;
}
