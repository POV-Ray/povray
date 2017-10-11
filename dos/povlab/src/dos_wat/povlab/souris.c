/* ---------------------------------------------------------------------------
*  SOURIS.C
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
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include <CONIO.H>
#if __WATCOMC__
  #include <I86.H>
#endif
#include "GLIB.H"
#include "GLOBAL.H"

int XM=0,YM=0;
int XM2=0,YM2=0;
byte XHotSpot=0,YHotSpot=0,CURSEUR;
char BufferCurseur[16][16];
byte OptVSouris=5;
byte BitMouse=0;
byte MSMouse=1;
byte Sens=MS_XY;                      // Num‚ro curseur souris

// -----------------------------------------------------------------------
// --------- LIMITE DE DEPLACEMENTS VERTICAL/HORIZONTAL ------------------
// -----------------------------------------------------------------------
void limite_h_v(int X1,int Y1,int X2,int Y2) {
  #if !WINDOWS
  // -------------- zone d‚placement horizontale
  regs.w.ax=0x07;
  regs.w.cx=X1;
  regs.w.dx=X2;
  int386(0x33,&regs,&regs);
  // -------------- zone d‚placement verticale
  regs.w.ax=0x08;
  regs.w.cx=Y1;
  regs.w.dx=Y2;
  int386(0x33,&regs,&regs);
  #endif
}

// -----------------------------------------------------------------------
// --------- PLACE LA SOURIS EN MODE GRAPHIQUE ---------------------------
// -----------------------------------------------------------------------
void place_mouse(int X,int Y) {
  if (MSMouse) {
    #if !WINDOWS
    regs.w.ax=0x04;
    regs.w.cx=X;
    regs.w.dx=Y;
    int386(0x33,&regs,&regs);
    #endif
  }
}

// -----------------------------------------------------------------------
// -- DEFINI UN POINTEUR DE SOURIS SPECIAL -------------------------------
// -----------------------------------------------------------------------
void forme_mouse(byte Type) {
  byte Mode=BitMouse;
  if (CURSEUR!=Type) {
    GMouseOff();
    CURSEUR=Type;
    dessine_souris(2);
    if (Mode==1) GMouseOn();
  }
}

// -----------------------------------------------------------------------
// --------- DESSINE LE MOTIF DE LA SOURIS -------------------------------
// -----------------------------------------------------------------------
// Op‚ration=0 Efface souris
// Op‚ration=1 Affiche souris
// Op‚ration=2 r‚cupŠre les hotspots
// -----------------------------------------------------------------------
void dessine_souris (byte Operation) {
  register int Couleur;

  set_port(0,0,XMax,YMax);

  if (Operation==0) put_curseur(XM2,YM2);

  if (Operation==1) {
    #if !WINDOWS
    Couleur=get_color();
    #endif
    get_curseur(XM2,YM2);
  }

  if (Operation==1 || Operation==2) {
    switch (CURSEUR) {
      case MS_FLECHE:
        XHotSpot=0;
        YHotSpot=0;
        if (Operation==2) break;
        g_ligne(XM2+0,YM2+0,XM2+0,YM2+10,BLANC);
        g_ligne(XM2+1,YM2+1,XM2+1,YM2+9,BLANC);
        g_ligne(XM2+2,YM2+2,XM2+2,YM2+8,BLANC);
        g_ligne(XM2+3,YM2+3,XM2+3,YM2+9,BLANC);
        g_ligne(XM2+4,YM2+4,XM2+4,YM2+11,BLANC);
        g_ligne(XM2+5,YM2+5,XM2+5,YM2+7,BLANC);
        g_ligne(XM2+6,YM2+6,XM2+6,YM2+7,BLANC);
        put_pixel(XM2+7,YM2+7,BLANC);
        g_ligne(XM2+5,YM2+10,XM2+5,YM2+13,BLANC); // queue
        g_ligne(XM2+6,YM2+12,XM2+6,YM2+13,BLANC);

        g_ligne(XM2+5,YM2+1,XM2+12,YM2+8,0);
        g_ligne(XM2+5,YM2+2,XM2+11,YM2+8,0);
        g_ligne(XM2+5,YM2+3,XM2+10,YM2+8,0);
        g_ligne(XM2+5,YM2+4,XM2+9,YM2+8,0);
        g_ligne(XM2+5,YM2+8,XM2+8,YM2+8,0);
        g_ligne(XM2+5,YM2+9,XM2+9,YM2+9,0);
        put_pixel(XM2+6,YM2+10,0);
        put_pixel(XM2+8,YM2+10,0);
        put_pixel(XM2+9,YM2+10,0);
        put_pixel(XM2+9,YM2+11,0);
        put_pixel(XM2+9,YM2+12,0);
        g_ligne(XM2+10,YM2+11,XM2+10,YM2+14,0); // queue
        g_ligne(XM2+11,YM2+13,XM2+11,YM2+14,0);

        break;
      case MS_SELECTEUR:
        XHotSpot=2;
        YHotSpot=2;
        if (Operation==2) break;
        g_rectangle(XM2,YM2,XM2+4,YM2+4,BLANC,0);
        break;
      case MS_X:
        XHotSpot=7;
        YHotSpot=7;
        if (Operation==2) break;
        g_rectangle(XM2+5,YM2+5,XM2+9,YM2+9,BLANC,0);

        g_ligne(XM2   ,YM2+7,XM2+4 ,YM2+7,BLANC);
        g_ligne(XM2+14,YM2+7,XM2+10,YM2+7,BLANC);

        g_ligne(XM2+1 ,YM2+6,XM2+2 ,YM2+6,BLANC);
        g_ligne(XM2+1 ,YM2+8,XM2+2 ,YM2+8,BLANC);
        g_ligne(XM2+13,YM2+6,XM2+12,YM2+6,BLANC);
        g_ligne(XM2+13,YM2+8,XM2+12,YM2+8,BLANC);

        g_ligne(XM2+2 ,YM2+5,XM2+2 ,YM2+5,BLANC);
        g_ligne(XM2+2 ,YM2+9,XM2+2 ,YM2+9,BLANC);
        g_ligne(XM2+12,YM2+5,XM2+12,YM2+5,BLANC);
        g_ligne(XM2+12,YM2+9,XM2+12,YM2+9,BLANC);
        break;
      case MS_Y:
        XHotSpot=7;
        YHotSpot=7;
        if (Operation==2) break;
        g_rectangle(XM2+5,YM2+5 ,XM2+9,YM2+9 ,BLANC,0);

        g_ligne(XM2+7,YM2   ,XM2+7,YM2+4 ,BLANC);
        g_ligne(XM2+7,YM2+14,XM2+7,YM2+10,BLANC);

        g_ligne(XM2+6,YM2+1 ,XM2+6,YM2+2 ,BLANC);
        g_ligne(XM2+8,YM2+1 ,XM2+8,YM2+2 ,BLANC);
        g_ligne(XM2+6,YM2+13,XM2+6,YM2+12,BLANC);
        g_ligne(XM2+8,YM2+13,XM2+8,YM2+12,BLANC);

        g_ligne(XM2+5,YM2+2 ,XM2+5,YM2+2 ,BLANC);
        g_ligne(XM2+9,YM2+2 ,XM2+9,YM2+2 ,BLANC);
        g_ligne(XM2+5,YM2+12,XM2+5,YM2+12,BLANC);
        g_ligne(XM2+9,YM2+12,XM2+9,YM2+12,BLANC);
        break;
      case MS_XY:
        XHotSpot=7;
        YHotSpot=7;
        if (Operation==2) break;
        g_rectangle(XM2+5,YM2+5,XM2+9,YM2+9,BLANC,0);

        g_ligne(XM2   ,YM2+7,XM2+4 ,YM2+7,BLANC); // Horizontal
        g_ligne(XM2+14,YM2+7,XM2+10,YM2+7,BLANC);

        g_ligne(XM2+1 ,YM2+6,XM2+2 ,YM2+6,BLANC);
        g_ligne(XM2+1 ,YM2+8,XM2+2 ,YM2+8,BLANC);
        g_ligne(XM2+13,YM2+6,XM2+12,YM2+6,BLANC);
        g_ligne(XM2+13,YM2+8,XM2+12,YM2+8,BLANC);

        g_ligne(XM2+2 ,YM2+5,XM2+2 ,YM2+5,BLANC);
        g_ligne(XM2+2 ,YM2+9,XM2+2 ,YM2+9,BLANC);
        g_ligne(XM2+12,YM2+5,XM2+12,YM2+5,BLANC);
        g_ligne(XM2+12,YM2+9,XM2+12,YM2+9,BLANC);

        g_ligne(XM2+7,YM2   ,XM2+7,YM2+4 ,BLANC);  // Vertical
        g_ligne(XM2+7,YM2+14,XM2+7,YM2+10,BLANC);

        g_ligne(XM2+6,YM2+1 ,XM2+6,YM2+2 ,BLANC);
        g_ligne(XM2+8,YM2+1 ,XM2+8,YM2+2 ,BLANC);
        g_ligne(XM2+6,YM2+13,XM2+6,YM2+12,BLANC);
        g_ligne(XM2+8,YM2+13,XM2+8,YM2+12,BLANC);

        g_ligne(XM2+5,YM2+2 ,XM2+5,YM2+2 ,BLANC);
        g_ligne(XM2+9,YM2+2 ,XM2+9,YM2+2 ,BLANC);
        g_ligne(XM2+5,YM2+12,XM2+5,YM2+12,BLANC);
        g_ligne(XM2+9,YM2+12,XM2+9,YM2+12,BLANC);
        break;
      case MS_CROIX:
        XHotSpot=0;
        YHotSpot=0;
        if (Operation==2) break;

        g_ligne(XM2+ 0,YM2+ 0,XM2+13,YM2+13,BLANC);
        g_ligne(XM2+ 1,YM2+ 0,XM2+13,YM2+12,BLANC);
        g_ligne(XM2+ 0,YM2+ 1,XM2+12,YM2+13,BLANC);

        g_ligne(XM2+13,YM2+ 0,XM2+ 0,YM2+ 13,BLANC);
        g_ligne(XM2+12,YM2+ 0,XM2+ 0,YM2+ 12,BLANC);
        g_ligne(XM2+13,YM2+ 1,XM2+ 1,YM2+ 13,BLANC);
       break;
    }
    #if !WINDOWS
    set_color(Couleur);
    #endif
  }
}

// -----------------------------------------------------------------------
// --------- SOURIS INVISIBLE --------------------------------------------
// -----------------------------------------------------------------------
void GMouseOff(void) {
  if (BitMouse==0) return;

  #if !WINDOWS
  regs.w.ax=0x001C;
  regs.w.bx=0;
  int386(0x33,&regs,&regs);
  dessine_souris(0);
  #else
  ShowCursor(FALSE);
  #endif

  
  BitMouse=0;
}

// -----------------------------------------------------------------------
// --------- SOURIS VISIBLE ----------------------------------------------
// -----------------------------------------------------------------------
void GMouseOn(void) {
  if (BitMouse==1) return;

  #if !WINDOWS
  regs.w.ax=0x001C;
  regs.w.bx=2;
  int386(0x33,&regs,&regs);
  dessine_souris(1);
  #else
  ShowCursor(TRUE);
  #endif

  BitMouse=1;
}

// -----------------------------------------------------------------------
// --------- REINITIALISATION DU DRIVER SOURIS HARD ET SOFT --------------
// -----------------------------------------------------------------------
void reset_mouse(void) {
  /*
  regs.w.ax=0;
  int386(0x33,&regs,&regs);

  regs.w.ax=0x2F; // ---------------- reset hardware
  int386(0x33,&regs,&regs);

  regs.w.ax=0x21; // ---------------- software reset
  int386(0x33,&regs,&regs);
  */
}

// -----------------------------------------------------------------------
// -- INITIALISATION DE LA SOURIS GRAPHIQUE ------------------------------
// -----------------------------------------------------------------------
void init_graphic_mouse (int X,int Y) {
  reset_mouse();

  limite_h_v(0,0,XMax,YMax);

  // -------------- r‚cup‚rer le fond sous la souris

  place_mouse(X,Y);
  XM2=XM=gmx_r();
  YM2=YM=gmy_r();
  vitesse_souris(OptVSouris);
  GMouseOn();
}

// ----------------------------------------------------------------------- */
// --- RENVOI LES PARAMETRES ACTUELS DU HANDLER DE LA SOURIS SUR Y ------- */
// ----------------------------------------------------------------------- */
int gmx_v(void) {
  XM=gmx_r();
  if (XM2!=XM && BitMouse) {
    dessine_souris(0);
    XM2=XM;
    dessine_souris(1);
  }
  return (XM2+XHotSpot);
}

// ----------------------------------------------------------------------- */
// --- RENVOI LES PARAMETRES ACTUELS DU HANDLER DE LA SOURIS SUR Y ------- */
// ----------------------------------------------------------------------- */
int gmy_v(void) {
  YM=gmy_r();
  if (YM2!=YM && BitMouse) {
    dessine_souris(0);
    YM2=YM;
    dessine_souris(1);
  }
  return (YM2+YHotSpot);
}

// ----------------------------------------------------------------------- */
// --------- RENVOI LES PARAMETRES ACTUELS REELS DE LA SOURIS SUR X ------ */
// ----------------------------------------------------------------------- */
int gmx_r(void) {
  #if !WINDOWS
  regs.w.ax=0x0003;
  int386(0x33,&regs,&regs);
  return (int) regs.w.cx;
  #else
  POINT Pt;

  GetCursorPos(&Pt);
  ScreenToClient(Global_Wnd,&Pt);
  message("X=%d Y=%d",Pt.x,Pt.y);

  return Pt.x;
  #endif
}

// -----------------------------------------------------------------------
// --------- RENVOI LES PARAMETRES ACTUELS REELS DE LA SOURIS SUR Y ------
// -----------------------------------------------------------------------
int gmy_r(void) {
  #if !WINDOWS
  regs.w.ax=0x0003;
  int386(0x33,&regs,&regs);
  return (int) regs.w.dx;
  #else
  POINT Pt;

  GetCursorPos(&Pt);
  ScreenToClient(Global_Wnd,&Pt);

  return Pt.y;
  #endif
}

// -----------------------------------------------------------------------
// --------- AFFICHE LES PARAMETRES ACTUELS DE LA SOURIS SUR X ET Y ------
// -----------------------------------------------------------------------
void graphic_mouse(void) {
  return;
  // gprintf(YMenuB+9,244,15,8,"X=%03d Y=%03d",g_mousex(),g_mousey());
}

// -------------------------------------------------------------------------
// -- MODIFIE LA FORME DU CURSEUR SOURIS EN FONCTION SENS SELECTION --------
// -------------------------------------------------------------------------
void Sens_Souris(void) {
  efface_buffer_clavier();
  if (CURSEUR!=MS_X && CURSEUR!=MS_Y && CURSEUR!=MS_XY) return;
  if (Sens==MS_XY) Sens=MS_X; else Sens++;

  switch(Sens) {
    case MS_XY:
      forme_mouse(MS_XY);
      break;
    case MS_X:
      forme_mouse(MS_X);
      break;
    case MS_Y:
      forme_mouse(MS_Y);
      break;
  }
}

// -----------------------------------------------------------------------
// -- RECUPERE LES PIXELS SOUS LE CURSEUR DE LA SOURIS -------------------
// -----------------------------------------------------------------------
void get_curseur(int X,int Y) {
  g_bitmap(X,Y,X+16,Y+16,SAUVE,3);
}

// -----------------------------------------------------------------------
// -- AFFICHE LES PIXELS SOUS LE CURSEUR DE LA SOURIS --------------------
// -----------------------------------------------------------------------
void put_curseur(int X,int Y) {
  g_bitmap(X,Y,0,0,AFFICHE,3);
  g_bitmap(X,Y,0,0,EFFACE,3);
}

// -----------------------------------------------------------------------
// -- ROUTINE POUR TESTER, GARDER ET RESTAURER LES PARAMETRES SOURIS -----
// -----------------------------------------------------------------------
void vitesse_souris(DBL Facteur) {
  int H,V;

  H=(int) (8*Facteur);
  V=(int) (13*Facteur);

  regs.w.ax=0x1A;
  regs.w.bx=H;
  regs.w.cx=V;
  regs.w.dx=30000;
  int386(0x33,&regs,&regs);
}
