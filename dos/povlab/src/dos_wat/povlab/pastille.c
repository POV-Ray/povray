/* ---------------------------------------------------------------------------
*  PASTILLE.C
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

byte PLong=13;

struct CasePastille Pastille[40];

// -----------------------------------------------------------------------
// --------- AFFICHE SPHERE DE SELECTION ---------------------------------
// -----------------------------------------------------------------------
void affiche_sphere_pastille(int X,int Y) {
  register int i,j,C;
  char Sphere[100] = {
    0x07,0x07,0x08,0x50,0x35,0x34,0x56,0x8F,0x07,0x07,
    0x07,0x4F,0x43,0x44,0x34,0x33,0x54,0xE6,0xE9,0x07,
    0x08,0x43,0x43,0xE8,0x34,0x33,0x54,0x24,0xD5,0x8F,
    0x50,0x44,0x0B,0x0E,0x34,0x54,0x54,0x24,0x23,0x61,
    0x51,0x33,0x34,0x34,0x32,0x54,0x24,0x24,0x23,0x22,
    0xD8,0x33,0x34,0x54,0x54,0x24,0x24,0xD5,0x23,0x22,
    0x57,0x54,0x54,0x54,0x24,0x24,0xD5,0x23,0x00,0x11,
    0x08,0x25,0x24,0x24,0x24,0xD5,0x23,0x00,0x23,0x08,
    0x07,0x8F,0x25,0x23,0x23,0x23,0x00,0x0F,0x58,0x07,
    0x07,0x07,0x08,0x09,0x10,0x10,0x09,0x08,0x07,0x07
 };

 for (i=0;i<10;i++) {
   for (j=0;j<10;j++) {
     C=Sphere[i*10+j];
     if (C!=7) put_pixel(X+j,Y+i,C);
   }
 }
}

// -----------------------------------------------------------------------
// --------- COCHE OU NON UNE PASTILLE -----------------------------------
// -----------------------------------------------------------------------
void coche_pastille(int N,byte Ok) {
  register byte Mode=BitMouse,C;

  if (Ok==3) return;
  GMouseOff();
  C=(Ok==1 ? ROUGE:0);
  if (Ok==2) C=FOND;
  Pastille[N].Croix=Ok;

  if (!C) {
    if (NbCouleurs==16) {
      set_color(FOND);
      #if !WINDOWS
      _ellipse_w(_GFILLINTERIOR,Pastille[N].X+3,
                 Pastille[N].Y+3,
                 Pastille[N].X+9,
                 Pastille[N].Y+9);
      #endif
    } else {
      g_rectangle(Pastille[N].X,Pastille[N].Y,Pastille[N].X+12,Pastille[N].Y+12,FOND,MOTIF);
      dessine_pastille(N);
    }
  } else {
    if (NbCouleurs==16) {
      // --------------- Vertical
      g_rectangle(Pastille[N].X+5,Pastille[N].Y+4,Pastille[N].X+7,Pastille[N].Y+8,C,1);
      // --------------- Horizontal
      g_rectangle(Pastille[N].X+4,Pastille[N].Y+5,Pastille[N].X+8,Pastille[N].Y+7,C,1);
    } else {
      affiche_sphere_pastille(Pastille[N].X+2,Pastille[N].Y+2);
    }
  }

  if (Mode) GMouseOn();
}

// -----------------------------------------------------------------------
// --------- DESSINE RELIEF UNE PASTILLE A COCHER ------------------------
// -----------------------------------------------------------------------
void dessine_pastille(int Num) {
  register int i,j,C;
  byte MotifPastille[13][13]={
    0,0,0,0,3,3,3,3,0,0,0,0,0,
    0,0,3,1,1,1,1,1,1,3,0,0,0,
    0,3,1,3,4,4,4,4,4,3,3,2,0,
    0,1,3,4,4,4,4,4,4,4,3,0,0,
    3,1,4,4,4,4,4,4,4,4,4,3,2,
    3,1,4,4,4,4,4,4,4,4,4,3,2,
    3,1,4,4,4,4,4,4,4,4,4,3,2,
    3,1,4,4,4,4,4,4,4,4,4,3,2,
    3,1,4,4,4,4,4,4,4,4,4,3,2,
    0,3,3,4,4,4,4,4,4,4,3,2,0,
    0,0,3,3,4,4,4,4,4,3,0,2,0,
    0,0,2,0,3,3,3,3,3,2,2,0,0,
    0,0,0,0,2,2,2,2,2,0,0,0,0
  };
  
  for (j=0;j<=12;j++) {
    for (i=0;i<=12;i++) {
      switch (MotifPastille[i][j]) {
        case 0: C=-1; break;
        case 1: C=NOIR; break;
        case 2: C=ECLAIRE; break;
        case 3: C=OMBRE; break;
        case 4: C=-1; break;
      }
      if (C>=0) put_pixel(Pastille[Num].X+i,Pastille[Num].Y+j,(byte) C);
    }
  }
}

// -----------------------------------------------------------------------
// --------- AFFICHE UNE PASTILLE A COCHER -------------------------------
// -----------------------------------------------------------------------
void affiche_pastille(int Num) {
  dessine_pastille(Num);
  coche_pastille(Num,Pastille[Num].Croix);
  text_xy(Pastille[Num].X+16,Pastille[Num].Y,Pastille[Num].Txt,15);
}

// -----------------------------------------------------------------------
// --------- TEST LES PASTILLES A COCHER ---------------------------------
// -----------------------------------------------------------------------
int test_pastille(int Debut,int Fin) {
  register int X=gmx_v(),X1,Y1;
  register int Y=gmy_v(),X2,Y2;
  register int N=-1,i;

  GMouseOn();
  
  if (MouseB()==1) {
    //if (Debut<10) NF=trouve_fenetre(0);
    for (i=Debut;i<=Fin;i++) {
      X1=Pastille[i].X; X2=Pastille[i].X+largeur_text(Pastille[i].Txt)+PLong+4;
      Y1=Pastille[i].Y; Y2=Pastille[i].Y+PLong;
      if (X1<X && X<X2 && Y1<Y && Y<Y2) { N=i; break; }
    }
    if (N>-1) {
      if (Pastille[N].Croix>1) return -1;
      coche_pastille(N,(Pastille[N].Croix==0 ? 1:0));
      while (MouseB());
      return N;
    }
  } else {
    for (i=Debut;i<=Fin;i++) {
      X1=Pastille[i].X; X2=Pastille[i].X+largeur_text(Pastille[i].Txt)+PLong+4;
      Y1=Pastille[i].Y; Y2=Pastille[i].Y+PLong;
      if (X1<X && X<X2 && Y1<Y && Y<Y2) {
        forme_mouse(MS_FLECHE);
        message_aide(0,YMenuD+7,JAUNE,13,Pastille[i].Aide,PASTILLE);
        return -1;
      }
    }
  }
  return -1;
}

// -----------------------------------------------------------------------
// --------- TEST GROUPE DE PASTILLES A COCHER ---------------------------
// -----------------------------------------------------------------------
int test_groupe_pastille(int Debut,int Fin) {
  register int X=gmx_v(),X1,Y1;
  register int Y=gmy_v(),X2,Y2;
  register int N=-1,i;

  GMouseOn();
  
  if (MouseB()==1) {
    //if (Debut<10) NF=trouve_fenetre(0);
    for (i=Debut;i<=Fin;i++) {
      X1=Pastille[i].X; X2=Pastille[i].X+largeur_text(Pastille[i].Txt)+PLong+4;
      Y1=Pastille[i].Y; Y2=Pastille[i].Y+PLong;
      if (X1<X && X<X2 && Y1<Y && Y<Y2) { N=i; break; }
    }
    if (N>-1) {
      if (Pastille[N].Croix>1) return -1;
      for (i=Debut;i<=Fin;i++) coche_pastille(i,(i==N ? 1:0));
      while (MouseB());
      return N;
     }
  } else {
    for (i=Debut;i<=Fin;i++) {
      X1=Pastille[i].X; X2=Pastille[i].X+largeur_text(Pastille[i].Txt)+PLong+4;
      Y1=Pastille[i].Y; Y2=Pastille[i].Y+PLong;
      if (X1<X && X<X2 && Y1<Y && Y<Y2) {
        forme_mouse(MS_FLECHE);
        message_aide(0,YMenuD+7,JAUNE,13,Pastille[i].Aide,PASTILLE);
        return -1;
      }
    }
  }
  return -1;
}

// -----------------------------------------------------------------------
// --------- INITIALISE UNE PASTILLE A COCHER ----------------------------
// -----------------------------------------------------------------------
void init_pastille(int Num,int X,int Y,char *String,byte Ok,char *Aide) {
  Pastille[Num].X=X;
  Pastille[Num].Y=Y;
  parse_ascii_watcom(String);
  strcpy(Pastille[Num].Txt,strlwr(String));
  Pastille[Num].Croix=Ok;
  if (Aide[0]) {
    strcpy(Pastille[Num].Aide,Aide);
  } else {
    strcpy(Pastille[Num].Aide,String);
  }
}

// -----------------------------------------------------------------------
// --------- RETOURNE LA PASTILLE COCHEE DANS UN GROUPE ------------------
// -----------------------------------------------------------------------
int quelle_pastille_dans_groupe(int N1,int N2) {
  register int i;

  for (i=N1;i<=N2;i++) {
    if (Pastille[i].Croix) return i;
  }

  return -1;
}
