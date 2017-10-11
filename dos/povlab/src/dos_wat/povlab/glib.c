/* ---------------------------------------------------------------------------
*  GLIB.C
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
#if __BORLANDC__
  #include <DIR.H>
  #include <ALLOC.H>
#endif
#if __WATCOMC__
  #include <GRAPH.H>
  #include <I86.H>
#endif
#include <DIRECT.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

#define MAX(a,b)    (((a)>(b))?(a):(b))
#define ABS(a)      (((a)<0) ? -(a) : (a))

char StrBoite[20][256];
char Resolution[5]="640";
int NbCouleurs=256;
int XDebutAide=240;

struct Coord Val;
struct Video Vid[4];

byte NOIR=0;
byte BLANC=14;
byte BLEU=1;
byte JAUNE=13;
byte ROUGE=6;
byte FFOND=229;    // Fond fenˆtre filaire
byte FOND=7;     // Couleur fenˆtre g‚n‚rale
byte ZFOND=8;    // Couleur fond zone de texte
byte OMBRE=9;    // Ombre du relief
byte ECLAIRE=14; // Partie ‚clair‚e du relief
byte TEXTE=0;

byte CINVISIBLE=255;
byte CMODIF=13;
byte CSELECTION=6;
byte COBJET=7;
byte CCAMERA=4;
byte CAXE=5;
byte CGRILLE=247;

typedef struct Video Vw[4];

// -----------------------------------------------------------------------
// -- ECRITURE D'UN MESSAGE QUAND PASSAGE SUR UN EVENEMENT ---------------
// -----------------------------------------------------------------------
void message_aide(int X,int Y,byte T,byte Long,char *String,byte Type) {
  static byte AncienType;
  static char AncienString[128];

  if (!OptAide) return;

  X=XMax-XDebutAide+3;
  Long=(XMax-9)-X-1;
  parse_ascii_watcom(String);
  if (AncienType==Type && strinstr(0,String,AncienString)==0) return;

  AncienType=Type;
  strcpy(AncienString,String);
  g_rectangle(X,Y-3,X+Long,Y+9,ZFOND,MOTIF);
  set_port(X,Y-3,X+Long,Y+9);
  text_xy(X,Y-4,String+(String[0]=='#' ? 1:0),T);
  select_vue(5,CLIP_ON);
}

// -----------------------------------------------------------------------
// -- ECRITURE SUR UNE LIGNE AVEC EFFACEMENT AVANT -----------------------
// -----------------------------------------------------------------------
void message(char *String,...) {
  register int Y=YMenuD+7;
  char Sortie[128];
  static int Lg=12; //=XMax-DEBUT_AIDE-6
  va_list parametre;

  va_start(parametre,String);
  vsprintf(Sortie,String,parametre);
  va_end(parametre);

  parse_ascii_watcom(Sortie);

  set_port(0,0,XMax,YMax);
  
  g_rectangle(12,Y-3,Lg,Y+9,ZFOND,MOTIF);
  set_port(12,Y-3,XMax-XDebutAide-6,Y+9);
  text_xy(12,Y-4,Sortie,BLANC);
  Lg=largeur_text(Sortie)+12;
  set_port(0,0,XMax,YMax);
}

// -----------------------------------------------------------------------
// -- ECRITURE D'UN TEXTE FORMATE EN GRAPHIQUE ---------------------------
// -----------------------------------------------------------------------
void gprintf(int X,int Y,byte T,byte F,byte Motif,byte Max,char *String,...) {
  va_list parametre;
  char Sortie[128];

  parse_ascii_watcom(String);

  va_start(parametre,String);
  vsprintf(Sortie,String,parametre);
  va_end(parametre);

  g_rectangle(X,Y-1,X+largeur_text("M")*Max,Y+hauteur_text(Sortie)-2,F,Motif ? MOTIF:1);
  text_xy(X,Y-1,Sortie,T);
}

// -----------------------------------------------------------------------
// --------- AFFICHAGE TEXTE AVEC POLICE A TAILLE VARIABLE ---------------
// -----------------------------------------------------------------------
void g_print(int X,int Y,byte T,byte F,byte Motif,char *Texte,byte Max) {
  g_rectangle(X,Y,X+largeur_text("M")*Max,Y+hauteur_text("G")+1,F,Motif ? MOTIF:1);
  text_xy(X+2,Y,Texte,T);
}

// -----------------------------------------------------------------------
// -- AFFICHE UNE FENETRE EN MODE GRAPHIQUE (0=CREUX,1=RELIEF) -----------
// -- type: 0 fenˆtre en creux
// -- type: 1 fenˆtre en relief (entour‚e de lignes noires)
// -- type: 2 fenˆtre en relief pour boutons (quand click)
// -- type: 3 fenˆtre en relief (sans entourage lignes noires)
// -----------------------------------------------------------------------
void windows(int x1,int y1,int x2,int y2,byte Type,byte Fond) {
  int PPlan,Omb1,Omb2,n;
  PPlan=Fond;
  Omb1=(Type>=1 ? ECLAIRE:OMBRE);
  Omb2=(Type>=1 ? OMBRE:ECLAIRE);

  if (Type==2) { Omb1=OMBRE; Omb2=ECLAIRE; }        // ------- Pour les boutons

  select_vue(5,CLIP_ON);

  n=(Type==2 ? 1:0);
  if (Type==1 || Type==3) {
    g_rectangle(x1+1,y1+1,x2-1,y2-1,PPlan,MOTIF);
  } else {
    g_rectangle(x1,y1,x2-1,y2-1,PPlan,MOTIF);
  }

  if (Type>=1) {
    g_ligne(x1,y1,x2,y1,Omb1);
    g_ligne(x1,y1,x1,y2,Omb1);
    g_ligne(x1+n,y2,x2,y2,Omb2);
    g_ligne(x2,y1+n,x2,y2,Omb2);
  } else {
    g_ligne(x1,y2,x2,y2,Omb2);
    g_ligne(x2,y1-1,x2,y2,Omb2);
    g_ligne(x1,y1-1,x2,y1-1,Omb1);
    g_ligne(x1-1,y1-1,x1-1,y2,Omb1);
  }
}

// ----------------------------------------------------------------------- */
// ----- AFFICHE UNE FENETRE EN MODE GRAPHIQUE --------------------------- */
// ----------------------------------------------------------------------- */
void g_fenetre(int XA,int YA,int XB,int YB,char *Titre,byte Travail) {
  static int Nb=4; // si plusieurs fenˆtres
  int ModeMouse;

  GMouseOff();
  ModeMouse=BitMouse;
  set_port(0,0,XMax,YMax);

  if (Travail==AFFICHE) {
    g_bitmap(XA,YA,XB+5,YB+5,SAUVE,Nb);
    //if (OptFade) zoom_rect_fenetre(XA,YA,XB,YB);
    windows(XA,YA,XB,YB,1,FOND);
    
    strupr(Titre);
    parse_ascii_watcom(Titre);

    // ---------- Fond titre de la fenˆtre
    windows(XA+5,YA+4,XB-5,YA+18,0,NbCouleurs==16 ? NOIR:222);
    // ---------- Ombres
    g_rectangle(XB+1,YA+7,XB+5,YB,NOIR,1);  // ombre droite
    g_rectangle(XA+6,YB+1,XB+5,YB+5,NOIR,1); // ombre bas
    // ------------- Texte
    //text_xy(XA+9+1,YA+4+1,Titre,FOND); // Gauche
    //text_xy(XA+9,YA+4,Titre,NOIR); // Gauche
    text_xy(XA+9,YA+4,Titre,BLANC); // Gauche
    log_out(0,"Open window [%d]:%s",Nb,Titre);
    
    // ---------- Incr‚mente le Nb de fenˆtre
    Nb++;
  }

  if (Travail==EFFACE) {
    while (MouseB());
    Nb--;
    g_bitmap(XA,YA,XB,YB,AFFICHE,Nb);
    g_bitmap(XA,YA,XB,YB,EFFACE,Nb);
    log_out(0,"Close window [%d]",Nb);
  }

  if (ModeMouse) GMouseOn();
}

// ----------------------------------------------------------------------- */
// ----- AFFICHE UNE BOITE DE DIALOGUE AVEC TOUT PLEIN DE CHOSES ! ------- */
// ----------------------------------------------------------------------- */
byte g_boite_ONA (int X,int Y,byte NbL,byte Type,byte Boutons) {
  register int i,ModeMouse;
  register int X1,Y1,X2,Y2,N;

  X1=X-145;
  X2=X+145;
  N=(75+(NbL*12)-10)/2;
  Y1=Y-N;
  Y2=Y+N+10;

  ModeMouse=BitMouse;

  set_port(0,0,XMax,YMax);

  forme_mouse(MS_FLECHE);
  efface_buffer_clavier();
  g_fenetre(X1,Y1,X2,Y2,StrBoite[0],AFFICHE);

  for (i=1;i<=NbL;i++) {
    parse_ascii_watcom(StrBoite[i]);
    switch (Type) {
      case GAUCHE:
        X=X1+14;
        break;
      case DROITE:
        X=X1+(X2-X1)*0.5-largeur_text(StrBoite[i])-14;
        break;
      case CENTRE:
        X=X1+((X2-X1)*0.5)-largeur_text(StrBoite[i])*0.5;
        break;
    }
    text_xy(X,Y1+27+(i-1)*12,StrBoite[i],TEXTE);
  }

  i=bouton_dialog(X1,X2,Y2,1,Boutons);

  while (i==-1) {
    i=bouton_dialog(X1,X2,Y2,0,Boutons);
  }

  while(MouseB());
  GMouseOff();
  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
  if (ModeMouse) GMouseOn();
  return i;
}

// -----------------------------------------------------------------------
// -- AFFICHE UN BORD EN RELIEF EN MODE GRAPHIQUE ------------------------
// -----------------------------------------------------------------------
void relief(int x1,int y1,int x2,int y2,byte Creux) {
  byte O=(Creux ? ECLAIRE:OMBRE);
  byte E=(Creux ? OMBRE:ECLAIRE);

  if (x1>x2) swap_int(&x1,&x2);
  if (y1>y2) swap_int(&y1,&y2);
  g_ligne(x1,y2,x2,y2,O);
  g_ligne(x1,y1,x2,y1,E);
  g_ligne(x1,y1,x1,y2,E);
  g_ligne(x2,y1,x2,y2,O);
}

// -----------------------------------------------------------------------
// -- AFFICHE UN BORD EN CREUX EN MODE GRAPHIQUE -------------------------
// -----------------------------------------------------------------------
void border(int x1,int y1,int x2,int y2,byte C,byte Creux) {
  register int Omb1,Omb2;

  C=C;
  
  if (Creux) {
    Omb1=ECLAIRE;
    Omb2=OMBRE;
  } else {
    Omb2=ECLAIRE;
    Omb1=OMBRE;
  }

  // g_rectangle(x1,y1,x2,y2,C,1);
  g_rectangle(x1+1,y1+1,x2,y2,Omb1,0);
  g_rectangle(x1,y1,x2-1,y2-1,Omb2,0);
  put_pixel(x2-1,y1+1,Omb1);
  put_pixel(x1+1,y2-1,Omb1);
}

// -----------------------------------------------------------------------
// -- PARCOURE LES LIGNES D'UN VOLUME POUR SELECTION OBJET ---------------
// -----------------------------------------------------------------------
byte point_sur_ligne(int px,int py,int qx,int qy,int tx,int ty) {
  if (ABS((qy-py)*(tx-px)-(ty-py)*(qx-px))>=(MAX(ABS(qx-px),ABS(qy-py)))) return 0;
  if (((qx<px) && (px<tx)) || ((qy<py) && (py<ty))) return 0;
  if (((tx<px) && (px<qx)) || ((ty<py) && (py<qy))) return 0;
  if (((px<qx) && (qx<tx)) || ((py<qy) && (qy<ty))) return 0;
  if (((tx<qx) && (qx<px)) || ((ty<qy) && (qy<py))) return 0;
  return 1;
}

byte test_ligne(int x1,int y1,int x2,int y2,int MX,int MY) {
  register int i,j,E=2; // Ecart de s‚lection

  x1+=Vid[NF].XF;
  y1+=Vid[NF].YF;
  x2+=Vid[NF].XF;
  y2+=Vid[NF].YF;

  for (i=-E;i<=E;i++) {
    for (j=-E;j<=E;j++) {
      if (point_sur_ligne(x1,y1,x2,y2,MX+i+1,MY+j+1)) return 1;
    }
  }

  return 0;
}

/*
byte test_ligne(int x1,int y1,int x2,int y2,int MX,int MY) {
  register int d,dx,dy;
  int aincr,bincr,xincr,yincr,x,y;
  int zx1,zy1,zx2,zy2;
  int E=4; // Ecart de s‚lection

  x1+=Vid[NF].XF;
  y1+=Vid[NF].YF;
  x2+=Vid[NF].XF;
  y2+=Vid[NF].YF;

  // g_ligne(MX-12,MY,MX+12,MY,JAUNE); // ************************
  // g_ligne(MX,MY-12,MX,MY+12,JAUNE); // ************************

  if (x1>x2) { zx1=x2; zx2=x1; } else { zx1=x1; zx2=x2; }
  if (y1>y2) { zy1=y2; zy2=y1; } else { zy1=y1; zy2=y2; }
  if ((zx2-zx1)>2 && (zy2-zy1)>2) {
    if (MX<zx1 || MX>zx2 || MY<zy1 || MY>zy2) return FAUX;
  }

  if (abs(x2-x1)<abs(y2-y1)) {
    if (y1>y2) {
      swap_int(&x1,&x2);
      swap_int(&y1,&y2);
    }

    xincr=(x2>x1) ? 1:-1;

    dy=y2-y1;
    dx=abs(x2-x1);
    d=2*dx-dy;
    aincr=2*(dx-dy);
    bincr=2*dx;
    x=x1;
    y=y1;

    if (MX>(x-E) && MX<(x+E) && MY>(y-E) && MY<(y+E)) return VRAI;
    // put_pixel(x,y,1); // ***********************

    for (y=y1+1;y<=y2;++y ) {
      if (d>=0) {
        x+=xincr;
        d+=aincr;
      } else {
        d+=bincr;
      }
      if (MX>(x-E) && MX<(x+E) && MY>(y-E) && MY<(y+E)) return VRAI;
      // put_pixel(x,y,1); // ***********************
    }
  } else {
    if (x1>x2) {
      swap_int(&x1,&x2);
      swap_int(&y1,&y2);
    }

    yincr=(y2>y1)?1:-1;

    dx=x2-x1;
    dy=abs(y2-y1);
    d=2*dy-dx;
    aincr=2*(dy-dx);
    bincr=2*dy;
    x=x1;
    y=y1;

    if (MX>(x-E) && MX<(x+E) && MY>(y-E) && MY<(y+E)) return VRAI;
    // put_pixel(x,y,1); // ***********************

    for (x=x1+1;x<=x2;++x) {
      if (d>=0) {
        y+=yincr;
        d+=aincr;
      } else {
        d+=bincr;
      }
      if (MX>(x-E) && MX<(x+E) && MY>(y-E) && MY<(y+E)) return VRAI;
      // put_pixel(x,y,1); // ***********************
    }
  }
  return FAUX;
} */

// ----------------------------------------------------------------------- */
// -- (DE)ACTIVE L'AFFICHAGE SUR LE MONITEUR ----------------------------- */
// ----------------------------------------------------------------------- */
void ecran_off(byte Valeur) {
  #if !WINDOWS
  if (Valeur) {
    _disable();
    inp(0x3DA);
    inp(0x3BA);
    outp(0x3C0,0x00);
    _enable();
  } else {
    _disable();
    inp(0x3DA);
    inp(0x3BA);
    outp(0x3C0,0x20);
    _enable();
  }
  #endif
}

// ----------------------------------------------------------------------- */
// ----- AFFICHE LA PALETTE DE COULEUR ----------------------------------- */
// ----------------------------------------------------------------------- */
int affiche_palette(void) {
  register unsigned int X=180,XM;
  register unsigned int Y=150,YM;
  byte Nb=16;
  int i,j,C=0;

  GMouseOff();
  forme_mouse(MS_SELECTEUR);

  g_fenetre(X-5,Y-25,X+((256/Nb)*10)+6,Y+Nb*10+60,"Color palette",AFFICHE);

  for (i=0;i<Nb;i++) {
    for (j=0;j<(256/Nb);j++) {
      g_rectangle(X+j*10,Y+i*10+4,X+j*10+10,Y+i*10+14,C++,1);
      g_rectangle(X+j*10,Y+i*10+4,X+j*10+10,Y+i*10+14,NOIR,0);
    }
  }

  g_rectangle(X,Y+Nb*10+17,X+((256/Nb)*10),Y+Nb*10+53,NOIR,0);
  g_rectangle(X+1,Y+Nb*10+18,X+((256/Nb)*10)-1,Y+Nb*10+52,FFOND,1);
  GMouseOn();

  while (1) {
    _disable();
    XM=gmx_v();
    YM=gmy_v();
    C=get_pixel(XM,YM);
    if (i!=C && C!=0 && XM>X && XM<X+(256/Nb)*10 && YM>Y+4 && YM<Y+Nb*10+4) {
      g_rectangle(X+1,Y+Nb*10+18,X+((256/Nb)*10)-1,Y+Nb*10+52,C,1);
      i=C;
      message("Color nø% 2d R=%.2f G=%.2f B=%.2f",C,
                                                (DBL) Palette[C][_R]/255,
                                                (DBL) Palette[C][_V]/255,
                                                (DBL) Palette[C][_B]/255);
    }
    _enable();
    if (kbhit()) if (getch()==27) { C=-1; break; }
    if (sequence_sortie()) { C=-1; break; }
    if (MouseB()==1) break;
  }

  g_fenetre(X-5,Y-25,X+((256/Nb)*10)+6,Y+Nb*10+60,"",EFFACE);
  forme_mouse(MS_FLECHE);
  while (MouseB());
  return C;
}

// ----------------------------------------------------------------------- */
// --------- AFFICHAGE UNE JAUGE GRAPHIQUE ------------------------------- */
// ----------------------------------------------------------------------- */
void g_jauge(byte n,int XA,int YA,int Long,int Haut,long P100,long CP100,byte Val,char *Texte) {
  DBL x=0;
  static int x2[10];
  register DBL k;

  Texte=Texte;
  GMouseOff();

  if (Val==0) {
    g_rectangle(XA+x+1,YA,XA+Long-1,YA+Haut-2,FOND,MOTIF);
    x2[n]=0;
    return;
  }

  k=(DBL) P100/CP100;
  k=(DBL) (k>1 ? 1:k);
  x=(DBL) k*(Long-1);

  if (Val==1) {
    g_rectangle(XA+x2[n],YA,XA+x,YA+Haut-2,BLEU,1);
    // parse_ascii_watcom(Texte);
    // text_xy(XA+5,(Haut-hauteur_text("N"))/2+YA-1,Texte,BLANC);
    x2[n]=(int) x+1;
    return;
  }

  if (Val==2) {
    g_rectangle(XA,YA,XA+x,YA+Haut-2,P100,-1);
  }
}

// -----------------------------------------------------------------------
// --------- AFFICHAGE UNE FENETRE AVEC UNE JAUGE GRAPHIQUE --------------
// -----------------------------------------------------------------------
void f_jauge(byte Nb,byte Travail,long PC,long CPC,char *Texte) {
  register unsigned int X1=CentX-120;
  register unsigned int X2=CentX+120;
  register unsigned int Y1=CentY-30;
  register unsigned int Y2=CentY+31;
  register int H=10;

  if (Travail==AFFICHE) {
    g_fenetre(X1,Y1,X2,Y2,Texte,AFFICHE);
    g_jauge(Nb,X1+15,Y1+35,213,H,0,0,0,"");
    relief(X1+14,Y1+34,X1+15+213,Y1+34+H,1);
  }
  if (Travail==MODIF) {
    g_jauge(Nb,X1+15,Y1+35,212,H,PC,CPC,1,"");
  }
  if (Travail==EFFACE) {
    g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
  }
}

// -----------------------------------------------------------------------
// ------- AFFICHAGE D'UNE ERREUR DANS UNE FENETRE GRAPHIQUE -------------
// -----------------------------------------------------------------------
void f_erreur(char *String,...) {
  va_list parametre;
  char Sortie[512];

  va_start(parametre,String);
  vsprintf(Sortie,String,parametre);
  va_end(parametre);

  beep_erreur();

  x_fenetre(CentX,CentY,CENTRE,0,"ERROR %s|%s",NomLogiciel,Sortie);
}

// -----------------------------------------------------------------------
// -- AFFICHAGE UNE VIGNETTE DE COULEUR C, AVEC UN BORDER DE COULEUR B ---
// -----------------------------------------------------------------------
void vignette_couleur(int X1,int Y1,int X2,int Y2,byte C,byte B) {
  g_rectangle(X1,Y1,X2,Y2,B,0);
  g_rectangle(X1+1,Y1+1,X2-1,Y2-1,C,1);
}

// -----------------------------------------------------------------------
// ------- AFFICHAGE D'UNE FENETRE COMPLEXE ------------------------------
// -----------------------------------------------------------------------
byte x_fenetre(int X,int Y,byte Type,byte Boutons,char *String,...) {
  va_list parametre;
  int i,NbL,j;
  int ModeMouse;
  int X1,Y1,X2,Y2,N,LX=0,Xt;
  char Sortie[2000];
  char *Ligne[50];

  X=(X==0 ? CentX:X);
  Y=(Y==0 ? CentY:Y);

  va_start(parametre,String);
  vsprintf(Sortie,String,parametre);
  va_end(parametre);

  parse_ascii_watcom(Sortie);
  NbL=analyse_ligne(Sortie,'|');

  for (i=0;i<=NbL;i++) {
    Ligne[i]=(char *) mem_alloc(strlen(Argu[i])+1);
    strcpy(Ligne[i],Argu[i]);
    LX=(LX<largeur_text(Ligne[i]) ? largeur_text(Ligne[i]):LX);
  }

  X1=X-(LX/2)-20;
  X2=X+(LX/2)+20;
  N=(75+(NbL*12)-10)/2;
  Y1=Y-N;
  Y2=Y+N;

  ModeMouse=BitMouse;

  efface_buffer_clavier();
  g_fenetre(X1,Y1,X2,Y2,Ligne[0],AFFICHE);

  for (i=1;i<=NbL;i++) {
    switch (Type) {
      case GAUCHE:
        Xt=X-(LX/2);
        break;
      case DROITE:
        Xt=X+LX/2-largeur_text(Ligne[i]);
        break;
      case CENTRE:
        Xt=X1+((X2-X1)*0.5)-largeur_text(Ligne[i])/2;
        break;
    }
    text_xy(Xt,Y1+27+(i-1)*12,Ligne[i],TEXTE);
  }

  i=bouton_dialog(X1,X2,Y2,1,Boutons);

  while (i==-1) {
    i=bouton_dialog(X1,X2,Y2,0,Boutons);
  }

  while(MouseB());
  GMouseOff();
  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
  if (ModeMouse) GMouseOn();

  for (j=0;j<=NbL;j++) mem_free(Ligne[j],strlen(Ligne[j])+1);

  return i;
}
