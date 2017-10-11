/* ---------------------------------------------------------------------------
*  CASE.C
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
#if __BORLANDC__
  #include <DIR.H>
  #include <ALLOC.H>
#endif
#if __WATCOMC__
  #include <GRAPH.H>
  #include <I86.H>
#endif
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

struct CaseACocher Cc[40];
int CcLong=12;

// -----------------------------------------------------------------------
// --------- AFFICHE SPHERE DE SELECTION ---------------------------------
// -----------------------------------------------------------------------
void affiche_sphere_case(int X,int Y) {
  register int i,j,C;
  char Sphere[100] = {
    0x07,0x07,0x08,0xF5,0x82,0x82,0x81,0x8F,0x07,0x07,
    0x07,0x88,0xDF,0xD7,0xD7,0x01,0x01,0x01,0x12,0x07,
    0x08,0xDF,0xDF,0xC6,0xDF,0x01,0x01,0x01,0x74,0x8F,
    0x87,0xD7,0xC6,0x0E,0xDF,0x01,0x01,0x74,0x74,0x11,
    0xF5,0xD7,0x01,0xDF,0x01,0x01,0x01,0x74,0x74,0x65,
    0xF5,0x01,0x01,0x01,0x01,0x01,0x74,0x74,0x74,0x65,
    0x87,0x01,0x01,0x01,0x01,0x74,0x74,0x74,0x74,0x11,
    0x08,0xDE,0x01,0x74,0x74,0x74,0x74,0x74,0x74,0x08,
    0x07,0x8F,0xDE,0x74,0x74,0x74,0x74,0x74,0x88,0x07,
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
// --------- COCHE OU NON UNE CASE ---------------------------------------
// -----------------------------------------------------------------------
void coche_case(int N,byte Ok) {
  register byte Mode=BitMouse,C;

  if (Ok==3) return;
  GMouseOff();

  C=(Ok==1 ? BLEU:0);

  if (Ok==2) C=FOND;
  Cc[N].Croix=Ok; 

  if (!C) {
    if (NbCouleurs==16) {
      g_rectangle(Cc[N].X+3,Cc[N].Y+3,
                  Cc[N].X+CcLong-3,Cc[N].Y+CcLong-3,FOND,MOTIF);
    } else {
      g_rectangle(Cc[N].X,Cc[N].Y,Cc[N].X+CcLong,Cc[N].Y+CcLong,FOND,MOTIF);
      dessine_case(N);
    }
  } else {
    if (NbCouleurs==16) {
      // --------- flŠche \ --------------
      g_ligne(Cc[N].X+3,Cc[N].Y+3,Cc[N].X+CcLong-3,Cc[N].Y+CcLong-3,C);
      g_ligne(Cc[N].X+4,Cc[N].Y+3,Cc[N].X+CcLong-3,Cc[N].Y+CcLong-4,C);
      g_ligne(Cc[N].X+3,Cc[N].Y+4,Cc[N].X+CcLong-4,Cc[N].Y+CcLong-3,C);
      // --------- flŠche / --------------
      g_ligne(Cc[N].X+3,Cc[N].Y+CcLong-3,Cc[N].X+CcLong-3,Cc[N].Y+3,C);
      g_ligne(Cc[N].X+3,Cc[N].Y+CcLong-4,Cc[N].X+CcLong-4,Cc[N].Y+3,C);
      g_ligne(Cc[N].X+4,Cc[N].Y+CcLong-3,Cc[N].X+CcLong-3,Cc[N].Y+4,C);
    } else {
      affiche_sphere_case(Cc[N].X+2,Cc[N].Y+2);
    }
  }
  if (Mode) GMouseOn();
}

// -----------------------------------------------------------------------
// --------- DESSINE_CASE A COCHER ---------------------------------------
// -----------------------------------------------------------------------
void dessine_case(int N) {
  g_rectangle(Cc[N].X,Cc[N].Y,Cc[N].X+CcLong,Cc[N].Y+CcLong,FOND,MOTIF);
  g_ligne(Cc[N].X,Cc[N].Y,Cc[N].X+CcLong,Cc[N].Y,OMBRE);
  g_ligne(Cc[N].X,Cc[N].Y,Cc[N].X,Cc[N].Y+CcLong,OMBRE);
  g_ligne(Cc[N].X+CcLong,Cc[N].Y+1,Cc[N].X+CcLong,Cc[N].Y+CcLong,ECLAIRE);
  g_ligne(Cc[N].X+1,Cc[N].Y+CcLong,Cc[N].X+CcLong,Cc[N].Y+CcLong,ECLAIRE);
  g_ligne(Cc[N].X+1,Cc[N].Y+1,Cc[N].X+1,Cc[N].Y+CcLong-1,NOIR);
  g_ligne(Cc[N].X+1,Cc[N].Y+1,Cc[N].X+CcLong-1,Cc[N].Y+1,NOIR);
}

// -----------------------------------------------------------------------
// --------- AFFICHE UNE CASE A COCHER -----------------------------------
// -----------------------------------------------------------------------
void affiche_case(int N) {
  dessine_case(N);
  coche_case(N,Cc[N].Croix);
  text_xy(Cc[N].X+16,Cc[N].Y,Cc[N].Txt,15);
}

// ----------------------------------------------------------------------- */
// --------- TEST LES CASES A COCHER ------------------------------------- */
// ----------------------------------------------------------------------- */
int test_case(int Debut,int Fin) {
  register int X=gmx_v(),X1,Y1;
  register int Y=gmy_v(),X2,Y2;
  register int N=-1,i;

  GMouseOn();
  
  if (MouseB()==1) {
    if (Debut<10) NF=trouve_fenetre(0);
    for (i=Debut;i<=Fin;i++) {
      X1=Cc[i].X; X2=Cc[i].X+largeur_text(Cc[i].Txt)+CcLong+4;
      Y1=Cc[i].Y; Y2=Cc[i].Y+CcLong;
      if (X1<X && X<X2 && Y1<Y && Y<Y2) { N=i; break; }
    }
    if (N>-1) {
      if (Cc[N].Croix>1) return -1;
      coche_case(N,(Cc[N].Croix==0 ? 1:0)); 
      while (MouseB());
      return N;
    }
  } else {
    for (i=Debut;i<=Fin;i++) {
      X1=Cc[i].X; X2=Cc[i].X+largeur_text(Cc[i].Txt)+CcLong+4;
      Y1=Cc[i].Y; Y2=Cc[i].Y+CcLong;
      if (X1<X && X<X2 && Y1<Y && Y<Y2) {
        forme_mouse(MS_FLECHE);
        message_aide(0,YMenuD+7,JAUNE,13,Cc[i].Aide,CASE);
        return -1;
      }
    }
  }
  return -1;
}

// ----------------------------------------------------------------------- */
// --------- INITIALISE UNE CASE A COCHER -------------------------------- */
// ----------------------------------------------------------------------- */
void init_case(int Num,int X,int Y,char *String,byte Ok,char *Aide) {
  Cc[Num].X=X;
  Cc[Num].Y=Y;
  parse_ascii_watcom(String);
  strcpy(Cc[Num].Txt,strlwr(String));
  Cc[Num].Croix=Ok;
  if (Aide[0]) {
    strcpy(Cc[Num].Aide,Aide);
  } else {
    strcpy(Cc[Num].Aide,String);
  }
}
