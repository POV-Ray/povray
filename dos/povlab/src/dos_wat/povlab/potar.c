/* ---------------------------------------------------------------------------
*  POTAR.C
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
#include <DOS.H>
#include <STDLIB.H>
#include <STDIO.H>
#include <STRING.H>
#include <IO.H>
#include "LIB.H"
#include "GLOBAL.H"
#include "GLIB.H"
#if !WINDOWS
#include <GRAPH.H>
#else
#include <WINDOWS.H>
extern HDC hdc;
#endif

struct potentiometre Potar[20];

// --------------------------------------------------------------------------
// ------------- AFFICHE LES FLECHES DROITE/GAUCHE --------------------------
// --------------------------------------------------------------------------
void affiche_fleche(int X,int Y,int Sens) {
  Sens=Sens ? 1:-1;

  g_ligne(       X,Y+3,       X,Y+3,NOIR);
  g_ligne(Sens*1+X,Y+2,Sens*1+X,Y+4,NOIR);
  g_ligne(Sens*2+X,Y+1,Sens*2+X,Y+5,NOIR);
  g_ligne(Sens*3+X,Y  ,Sens*3+X,Y+6,NOIR);
}

// --------------------------------------------------------------------------
// ------------- INITIALISE UN CURSEUR GRAPHIQUE ----------------------------
// --------------------------------------------------------------------------
void modif_curseur(int N,byte Travail) {
  char Chiffre[10];
  register int X=Potar[N].X;
  register int Y=Potar[N].Y;
  register int L=Potar[N].Long,D;
  register DBL V=Potar[N].Val;
  register DBL Max=Potar[N].Max;
  register DBL Min=Potar[N].Min;
  register DBL Valeur;
  #if WINDOWS
  HBITMAP hBitMap;
  #endif

  Valeur=(DBL) (L/(Max-Min));
  D=(int)(-Min*Valeur);

  if (Travail==SAUVE) {
    #if !WINDOWS
    _getimage_w(D+X+Valeur*V-LC,Y-HC,D+X+Valeur*V+LC,Y+HC,Potar[N].Curseur);
    #else
    hBitMap=CreateCompatibleBitmap(hdc,(D+X+Valeur*V+LC)-D+X+Valeur*V-LC,
                                       (Y+HC)-(Y-HC));

    #endif
    g_rectangle(X+1,Y-1,D+X+Valeur*V,Y+1,BLEU,1); // fond sillon gauche
    g_rectangle(D+X+Valeur*V,Y-1,X+L-1,Y+1,FOND,MOTIF); // fond sillon droite
    windows(D+X+Valeur*V-LC+1,Y-HC+1,D+X+Valeur*V+LC-1,Y+HC-1,3,FOND); // curseur relief
    windows(D+X+Valeur*V-LC+2,Y-HC+2,D+X+Valeur*V+LC-2,Y+HC-2,3,FOND); // curseur
    g_ligne(D+X+Valeur*V-1,Y-HC+3,D+X+Valeur*V-1,Y+HC-3,OMBRE);
    g_ligne(D+X+Valeur*V+0,Y-HC+3,D+X+Valeur*V+0,Y+HC-3,ECLAIRE);
    g_rectangle(X+L+27,Y-6,X+L+78,Y+6,FOND,MOTIF); // texte
    sprintf(Chiffre,"%+.2lf",Potar[N].Val);
    text_xy(X+L+29,Y-6,Chiffre,NOIR);
  }

  if (Travail==AFFICHE) {
    #if !WINDOWS
    _putimage_w(D+X+Valeur*V-LC,Y-HC,Potar[N].Curseur,_GPSET);
    #else
    draw_bitmap(hdc,hBitMap,D+X+Valeur*V-LC,Y-HC);
    #endif
  }
}

// --------------------------------------------------------------------------
// ------------- INITIALISE UN CURSEUR GRAPHIQUE ----------------------------
// --------------------------------------------------------------------------
void init_potar(int N,int XC,int YC,int LongC,DBL MinC,DBL MaxC,DBL ValC,DBL Pas,char *Aide) {
  Potar[N].X=XC;
  Potar[N].Y=YC;
  Potar[N].Long=LongC;
  Potar[N].Min=MinC;
  Potar[N].Max=MaxC;
  Potar[N].Val=ValC;
  Potar[N].Pas=Pas;
  if (Aide[0]) {
    strcpy(Potar[N].Aide,Aide);
  } else {
    Potar[N].Aide[0]=NULLC;
  }
}

// --------------------------------------------------------------------------
// ------------- AFFICHE UN POTENTIOMETRE GRAPHIQUE -------------------------
// --------------------------------------------------------------------------
void affiche_potar(int N) {
  register int X=Potar[N].X;
  register int Y=Potar[N].Y;
  register int L=Potar[N].Long;

  GMouseOff();
  windows(X+L+25,Y-8,X+L+80,Y+8,3,FOND); // texte
  g_rectangle(X-5,Y-7,X+L+3,Y+7,FOND,MOTIF); // efface cadre sillon
  relief(X-3,Y-7,X+L+3,Y+7,0); // cadre sillon
  windows(X+1,Y-1,X+L,Y+2,0,FOND);  // fond sillon
  relief(X-21,Y-7,X-6,Y-8+15,0);   // bouton gauche '-'
  relief(X+L+6,Y-7,X+L+21,Y-8+15,0);  // bouton gauche '+'
  affiche_fleche(X+L+11,Y-3,0);
  affiche_fleche(X-11,Y-3,1);
  modif_curseur(N,SAUVE);
}

// --------------------------------------------------------------------------
// -------- TEST SI UTILISATION D'UN POTENTIOMETE GRAPHIQUE -----------------
// --------------------------------------------------------------------------
int test_potar(int Num1,int Num2) {
  static int Deja=-1;
  register int MX,MXC;
  int MY;
  int X;
  int Y;
  int L,D;
  DBL V;
  DBL Max;
  DBL Min;
  DBL Valeur,P;
  register int i;

  GMouseOn();
  if (Deja>=0) Num1=Num2=Deja;

  for (i=Num1;i<=Num2;i++) {
    MX=gmx_v();
    MY=gmy_v();
    X=Potar[i].X;
    Y=Potar[i].Y;
    L=Potar[i].Long;
    V=Potar[i].Val;
    P=Potar[i].Pas;
    Max=Potar[i].Max;
    Min=Potar[i].Min;
    Valeur=(DBL) (L/(Max-Min));
    D=(int) (-Min*Valeur);

    // ----------------- curseur
    if ((MX>(D+X+Valeur*V-LC) &&
        MX<(D+X+Valeur*V+LC) &&
        MY>(Y-HC) &&
        MY<(Y+HC)) || Deja>=0) {
       while (MouseB()==1) {
        if (MX!=(MXC=gmx_v())) {
          GMouseOff();
          V=(DBL) ((MXC-(X+D))*1/Valeur);
          V=(V<Min ? Min:V);
          V=(V>Max ? Max:V);
          modif_curseur(i,AFFICHE);
          Potar[i].Val=V;
          modif_curseur(i,SAUVE);
          MX=gmx_v();
          Deja=i;
          return i;
        }
        GMouseOn();
      }
    }

    Deja=-1;

    // ----------------- ('-')
    if ((MX>(X-20) && MX<(X-5) && MY>(Y-7) && MY<(Y+7)) && MouseB()==1 && Potar[i].Val>Potar[i].Min) {
      GMouseOff();
      V-=P;
      V=(V<Min ? Min:V);
      V=(V>Max ? Max:V);
      modif_curseur(i,AFFICHE);
      Potar[i].Val=V;
      modif_curseur(i,SAUVE);
      MX=gmx_v();
      GMouseOn();
      while (MouseB());
      return i;
    }

    // ----------------- ('+')
    if ((MX>(X+L+5) && MX<(X+L+20) && MY>(Y-7) && MY<(Y+7)) && MouseB()==1 && Potar[i].Val<Potar[i].Max) {
      GMouseOff();
      V+=P;
      V=(V<Min ? Min:V);
      V=(V>Max ? Max:V);
      modif_curseur(i,AFFICHE);
      Potar[i].Val=V;
      modif_curseur(i,SAUVE);
      MX=gmx_v();
      GMouseOn();
      while (MouseB());
      return i;
    }

    if ((MX>(X-20) &&  // --------------------- Aide
        MX<(X+L+20) &&
        MY>(Y-7) &&
        MY<(Y+7) && !MouseB()) && Deja==-1) {
      forme_mouse(MS_FLECHE);
      message_aide(0,YMenuD+7,JAUNE,13,Potar[i].Aide,POTAR);
      return -1;
    }
  }

  return -1;
}
