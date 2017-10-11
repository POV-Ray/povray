/* ---------------------------------------------------------------------------
*  GCONVERT.C
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
#include <FLOAT.H>
#include <MATH.H>
#include <STDLIB.H>
#include <STRING.H>
#include <DOS.H>
#include <IO.H>
#include "GLOBAL.H"
#include "GLIB.H"
#include "LIB.H"

int CX1,CX2,CY1,CY2;

#define VBE 0
#if VBE
#include "VBE\SVGA.H"
#include "VBE\VESAVBE.H"
#include "VBE\PMODE.H"
#include "VBE\VBEAF.H"
#endif

#if WINDOWS
#include <WINDOWS.H>
extern HDC hdc;
extern HWND Global_Wnd;
HPEN Pen;
unsigned char PenStyle;
#else
#include <GRAPH.H>

#endif

int XMax,YMax;
extern short _SuperVGAType();
char *SVGANames[] = {
  "is not a SuperVGA",                // _SV_NONE
  "supports the VESA standard",       // _SV_VESA
  "is made by Video-7",               // _SV_VIDEO7
  "is made by Paradise (WD)",         // _SV_PARADISE
  "is made by ATI",                   // _SV_ATI
  "is made by Tseng (3000)",          // _SV_TSENG3000
  "is made by Tseng (4000)",          // _SV_TSENG4000
  "is made by Oak",                   // _SV_OAK
  "is made by Trident",               // _SV_TRIDENT
  "is made by Chips and Technologies",// _SV_CHIPS
  "is made by Genoa",                 // _SV_GENOA
  "is made by S3",                    // _SV_S3
  "is made by Cirrus Logic"           // _SV_CIRRUS
};

#if VBE
SV_devCtx   *DC;
bool        useVBEAF = false;
bool        useVirtualBuffer = false;
bool        linearBuffer = false;
#endif

// -----------------------------------------------------------------------
// --------- INITIALISATION GRAPHIQUE MODE RAPIUDE -----------------------
// -----------------------------------------------------------------------
void init_sci_vesa(Mode) {
  #if VBE
  DC = SV_init(useVBEAF);
  if (!DC || DC->VBEVersion < 0x102) {
    g_erreur(1);
    exit(1);
  }

  if (SV_setMode(Mode,false,useVirtualBuffer,0)) {
    linearBuffer = (Mode & vbeLinearBuffer) || DC->virtualBuffer;
  }
  #endif
}

// -----------------------------------------------------------------------
// -- MODES VGA/SVGA 16/256 DISPONIBLES ----------------------------------
// -----------------------------------------------------------------------
void watcom_video(void) {
  struct videoconfig  vc;
  short type;

  _getvideoconfig(&vc);     // initialize graphics library variables
  type=_SuperVGAType();
  log_out(0,"The graphics adapter %s",SVGANames[type]);
}

// -----------------------------------------------------------------------
// -- MODES VGA/SVGA 16/256 DISPONIBLES ----------------------------------
// -----------------------------------------------------------------------
int DetectVGA(void) {
  int Mode=(NbCouleurs==256 ? 0x101:18);

  if (!strcmp(Resolution,"320")) {
    Mode=19;
    XMax=319;
    YMax=199;
  }

  if (!strcmp(Resolution,"640")) {
    Mode=(NbCouleurs==256 ? 0x101:18);
    XMax=639;
    YMax=479;
  }

  if (!strcmp(Resolution,"800")) {
    Mode=(NbCouleurs==256 ? 0x103:0x102);
    XMax=799;
    YMax=599;
  }

  if (!strcmp(Resolution,"1024")) {
    Mode=(NbCouleurs==256 ? 0x105:0x104);
    XMax=1023;
    YMax=767;
  }

  return Mode;
}

// ----------------------------------------------------------------------- */
// --------- ERREUR A L'INITIALISATION GRAPHIQUE ------------------------- */
// ----------------------------------------------------------------------- */
void g_erreur(int Erreur) {
  if (Erreur==0) return;
  text_mode();
  printf("Can't detect a supervga (SVGA) graphic mode.\n");
  printf("This program requires a VESA VBE 1.2 or higher compatible SuperVGA. Try\n");
  printf("installing the Universal VESA VBE for your video card, or contact your\n");
  printf("video card vendor and ask for a suitable TSR\n");
  printf("Switch your graphic card to the VESA mode or init PaletteInterface=16");
  printf(" in\nthe config file.\n\n");
  printf("Graphic error=%d.\n",Erreur);
  exit(0);
}

// ----------------------------------------------------------------------- */
// --------- INITIALISATION EN FONCTION DU MODE GRAPHIQUE ---------------- */
// ----------------------------------------------------------------------- */
byte init_gmode(byte Graphic) {
  int Mode;

  Graphic=Graphic;
  
  if (!strcmp(getenv("OS"),"Windows_NT")) {
    strcpy(Resolution,"640");
    XMax=639;
    YMax=479;
    Mode=18;
    NbCouleurs=16;
  }

  if (NbCouleurs!=16 && NbCouleurs!=256) NbCouleurs=16;
  #if !WINDOWS
  ecran_off(1);

  watcom_video();
  _setvideomode(_VRES16COLOR);
  _setvideomode((Mode=DetectVGA()));

  if (NbCouleurs==256) {
    init_sci_vesa(Mode);
  }

  ecran_off(0);
  #else
  DetectVGA();
  #endif
  if (NbCouleurs==16) FFOND=NOIR;
  return 0;
}

// ----------------------------------------------------------------------- */
// -- FIN DU MODE GRAPHIQUE ---------------------------------------------- */
// ----------------------------------------------------------------------- */
void fin_mode_graphique(void) {
  #if !WINDOWS
  _unregisterfonts();
  #endif
}

// ----------------------------------------------------------------------- */
// --------- AFFICHE UNE LIGNE ------------------------------------------- */
// ----------------------------------------------------------------------- */
void g_ligne(int X1,int Y1,int X2,int Y2,byte Color) {
  #if !WINDOWS
  _setcolor(Color);
  _moveto_w(X1,Y1);
  _lineto_w(X2,Y2);
  #else
  Pen=CreatePen(PenStyle,1,RGB(Palette[Color][0],
                               Palette[Color][1],
                               Palette[Color][2]));
  SelectObject(hdc,Pen);
  MoveTo(hdc,X1,Y1);
  LineTo(hdc,X2,Y2);
  DeleteObject(Pen);
  #endif
}

#if WINDOWS
void draw_bitmap(HDC hdc, HBITMAP hBitmap,short xStart,short yStart) {
  BITMAP bm;
  HDC hdcMem;
  DWORD dwSize;
  POINT ptSize, ptOrg;

  hdcMem=CreateCompatibleDC(hdc);
  SelectObject(hdcMem,hBitmap);
  SetMapMode(hdcMem,GetMapMode(hdc));
  GetObject(hBitmap,sizeof(BITMAP),(LPSTR) &bm);
  ptSize.x=bm.bmWidth;
  ptSize.y=bm.bmHeight;
  DPtoLP(hdc,&ptSize,1);
  ptOrg.x=0;
  ptOrg.y=0;
  DPtoLP(hdcMem,&ptOrg,1);
  BitBlt(hdc,xStart,yStart,ptSize.x,ptSize.y,hdcMem,ptOrg.x,ptOrg.y,SRCCOPY);
  DeleteDC(hdcMem);
}
#endif

// -----------------------------------------------------------------------
// --------- GERE LA SAUVEGARDE/RESTAURATION D'UNE IMAGE BITMAP ----------
// --        Boutons  0                                                 --
// --        Barre    1                                                 --
// --        Menus    2                                                 --
// --        Souris   3                                                 --
// --        Fenˆtres 4                                                 --
// -----------------------------------------------------------------------
int g_bitmap(int X1,int Y1,int X2,int Y2,byte Travail,int Num) {
  #if !WINDOWS
  static char *Tampon[255];
  static long Taille[255];

  if (Travail==SAUVE) {
    Taille[Num]=_imagesize_w(X1,Y1,X2,Y2);
    Tampon[Num]=(char *) mem_alloc(Taille[Num]);
    _getimage_w(X1,Y1,X2,Y2,Tampon[Num]);
  }

  if (Travail==AFFICHE) _putimage_w(X1,Y1,Tampon[Num],_GPSET);

  if (Travail==EFFACE) {
    mem_free(Tampon[Num],Taille[Num]);
  }
  #else
  HBITMAP HBitMap[255];

  if (Travail==SAUVE) {
    HBitMap[Num]=CreateCompatibleBitmap(hdc,X2-X1,Y2-Y1);
  }

  if (Travail==AFFICHE) draw_bitmap(hdc,HBitMap[Num],X1,Y1);
  #endif

  return 0;
}

// ----------------------------------------------------------------------- */
// --------- AFFICHE UN PIXEL -------------------------------------------- */
// ----------------------------------------------------------------------- */
void put_pixel(int X1,int Y1,byte Color) {
  #if !WINDOWS
  _setcolor(Color);
  _setpixel_w(X1,Y1);
  #else
  SetPixel(hdc,X1,Y1,RGB(Palette[Color][0],
                                    Palette[Color][1],
                                    Palette[Color][2]));
  #endif
}

// ----------------------------------------------------------------------- */
// --------- RECUPERE UN PIXEL ------------------------------------------- */
// ----------------------------------------------------------------------- */
byte get_pixel(int X1,int Y1) {
  #if !WINDOWS
  return _getpixel_w(X1,Y1);
  #else
  return FFOND;
  #endif
}

// ----------------------------------------------------------------------- */
// --------- RECUPERE LA COULEUR PAR DEFAUT ------------------------------ */
// ----------------------------------------------------------------------- */

byte get_color(void) {
  #if !WINDOWS
  return _getcolor();
  #else
  return FOND;
  #endif
}

// ----------------------------------------------------------------------- */
// --------- DEFINI LA COULEUR PAR DEFAUT -------------------------------- */
// ----------------------------------------------------------------------- */

void set_color(byte Color) {
  #if !WINDOWS
  _setcolor(Color);
  #endif
}

// ----------------------------------------------------------------------- */
// --------- DEPLACE LE CURSEUR GRAPHIQUE -------------------------------- */
// ----------------------------------------------------------------------- */
void move_to(int X,int Y) {
 #if !WINDOWS
 _moveto_w(X,Y);
 #else
 MoveTo(hdc,X,Y);
 #endif
}

// -----------------------------------------------------------------------
// --------- AFFICHE TRES RAPIDEMENT DES LIGNES --------------------------
// -----------------------------------------------------------------------
void fast_line_to(int X,int Y,byte Color) {
  #if VBE
  struct _wxycoord Pos;
  int X1,Y1,X2,Y2,r;

  Pos=_getcurrentposition_w();

  X1=Pos.wx+CX1;
  Y1=Pos.wy+CY1;
  X2=X+CX1;
  Y2=Y+CY1;

  r=do_clip(&X1,&Y1,&X2,&Y2,CX1,CY1,CX2,CY2);
  
  if (r) {
  
  if (X1<CX1 && X2<CX1) {} else
  if (X1>CX2 && X2>CX2) {} else
  if (Y1<CY1 && Y2<CY1) {} else
  if (Y1>CY2 && Y2>CY2) {} else {
  
  //log_out(0,"CoordF %d:%d:%03d[%03d/%03d]%03d-%03d [%03d/%03d]%03d-%03d",DrawNF,r,Color,CX1,CX2,X1,X2,CY1,CY2,Y1,Y2);

  SV_beginDirectAccess();
  SV_beginLine();
  SV_lineFast((int)X1,(int)Y1,(int)X2,(int)Y2,Color);
  SV_endLine();
  SV_endDirectAccess();

  //select_vue(5,CLIP_OFF);
  //g_ligne((int)X1,(int)Y1,(int)X2,(int)Y2,Color);
  }
  }

  move_to(X,Y);
  #endif
}

// ----------------------------------------------------------------------- */
// --------- DEPLACE LE CURSEUR GRAPHIQUE -------------------------------- */
// ----------------------------------------------------------------------- */
void g_ligne_to(int X,int Y,byte Color) {
  #if !WINDOWS
  #if VBE
  if (_getlinestyle()==0xFFFF && _getplotaction()==_GPSET) {
    fast_line_to(X,Y,Color);
  } else {
  #endif
    _setcolor(Color);
    _lineto_w(X,Y);
  #if VBE
  }
  #endif
  #else
  Pen=CreatePen(PenStyle,1,RGB(Palette[Color][0],Palette[Color][1],Palette[Color][2]));
  SelectObject(hdc,Pen);
  LineTo(hdc,X,Y);
  DeleteObject(Pen);
  #endif
}

// ----------------------------------------------------------------------- */
// --------- AFFICHE UNE LIGNE RELATIVE ---------------------------------- */
// ----------------------------------------------------------------------- */
void ligne_rel(int X,int Y,byte Color) {
  #if !WINDOWS
  struct _wxycoord Pos;

  _setcolor(Color);
  Pos=_getcurrentposition_w();
  _lineto_w(Pos.wx+X,Pos.wy+Y);
  #else
  int XR,YR;
  DWORD dwPoint;

  dwPoint=GetCurrentPosition(hdc);

  Pen=CreatePen(PenStyle,1,RGB(Palette[Color][0],Palette[Color][1],Palette[Color][2]));
  SelectObject(hdc,Pen);
  LineTo(hdc,LOWORD(dwPoint),HIWORD(dwPoint));
  DeleteObject(Pen);
  #endif
}

// ----------------------------------------------------------------------- */
// --------- AFFICHE UN RECTANGLE ---------------------------------------- */
// ----------------------------------------------------------------------- */
void g_rectangle(int X1,int Y1,int X2,int Y2,byte Color,int Type) {
  #if !WINDOWS
  register int i,j;

  if (Type!=0) {
      _setcolor(Color);
      if (MotifOk && Type==MOTIF && ImageMotif!=NULL) {
        set_port(X1,Y1,X2,Y2);
        for (i=-X1;i<=X2-X1;i+=XMotif) {
          for (j=-Y1;j<=Y2-Y1;j+=YMotif) {
            _putimage(i,j,ImageMotif,_GPSET);
          }
        }
        set_port(0,0,XMax,YMax);
      } else {
        _rectangle_w(_GFILLINTERIOR,X1,Y1,X2,Y2);
      }
  } else {
      _setcolor(Color);
      _rectangle_w(_GBORDER,X1,Y1,X2,Y2);
  }
  #else
  HBRUSH HBrush;
  RECT Rect;


  if (Type!=0) {
    HBrush=CreateSolidBrush(RGB(Palette[Color][0],Palette[Color][1],Palette[Color][2]));
    SelectObject(hdc,HBrush);
    //Rectangle(hdc,X1,Y1,X2,Y2);
    SetRect(&Rect,X1,Y1,X2,Y2);
    FillRect(hdc,&Rect,HBrush);
    DeleteObject(HBrush);
  } else {
    Pen=CreatePen(PenStyle,1,RGB(Palette[Color][0],Palette[Color][1],Palette[Color][2]));
    SelectObject(hdc,Pen);
    Rectangle(hdc,X1,Y1,X2,Y2);
    DeleteObject(Pen);
  }
  #endif
}

// ----------------------------------------------------------------------- */
// -- SELECTIONNE UN TYPE D'ECRITURE (AND,OR,XOR,NOR,NOT...) ------------- */
// ----------------------------------------------------------------------- */
void type_ecriture(byte Type) {
  switch (Type) {
    #if WINDOWS
    case COPY_PUT: Type=R2_COPYPEN; break;
    case XOR_PUT:  Type=R2_XORPEN; break;
    #else
    case COPY_PUT: Type=_GPSET; break;
    case XOR_PUT:  Type=_GXOR; break;
    #endif
  }
  #if WINDOWS
  SetROP2(hdc,Type);
  #else
  _setplotaction(Type);
  #endif
}

// ----------------------------------------------------------------------- */
// -- AFFICHE UN TEXTE A L'ECRAN ----------------------------------------- */
// ----------------------------------------------------------------------- */
void text_xy(int X,int Y,char *Texte,byte Color) {
  #if !WINDOWS
  _setcolor(Color);
  _moveto_w(X,Y);
  _outgtext(Texte);
  #else
  SetTextColor(hdc,RGB(Palette[Color][0],Palette[Color][1],Palette[Color][2]));
  TextOut(hdc,X,Y,Texte,strlen(Texte));
  #endif
}

// -----------------------------------------------------------------------
// -- INITIALISE LES POLICES DE CARATERES --------------------------------
// -----------------------------------------------------------------------
void init_police(byte Travail,byte Nb) {
    #if WINDOWS
    char Parm[30];
    char Chemin[MAXPATH];
    LOGFONT LFont;
    HFONT HFont;

    if (Travail==1) {
      strcpy(Chemin,NewChemin);
      strcat(Chemin,"\\SYSTEM\\MODELLER.FON");
      if (Travail) {
        HFont=SelectObject(hdc,CreateFontIndirect(&LFont));
        GetTextFace(hdc,sizeof("Swiss"),"Swiss");
      }
    }
    #else
    char Parm[30];
    char Chemin[MAXPATH];

    if (Travail==1) {
      strcpy(Chemin,NewChemin);
      strcat(Chemin,"\\SYSTEM\\MODELLER.FON");
      if (Travail) _registerfonts(Chemin);
    }

    switch (Nb) {
      case 0:
        strcpy(Parm,"h2t'MS Sans Serif'w2bp");
        break;
      case 1:
        strcpy(Parm,"h12t'MS Sans Serif'w12bp");
        break;
      case 2:
        strcpy(Parm,"h7t'MS Sans Serif'w7bp");
        break;
    }

    if (_setfont(Parm)<0) { init_police(1,Nb); }
    #endif
}

// ----------------------------------------------------------------------- */
// -- TYPE MODIF DE LIGNE ------------------------------------------------ */
// ----------------------------------------------------------------------- */
void type_motif_ligne(byte Motif) {
  #if !WINDOWS
  switch(Motif) {
    case 0:
      _setlinestyle(bin("1111111111111111"));
      break;
    case 1:
      _setlinestyle(bin("1111000011110000"));
      break;
    case 2:
      _setlinestyle(bin("1010101010101010"));
      break;
    case 3:
      _setlinestyle(bin("1100110011001100"));
      break;
  }
  #else
  switch(Motif) {
    case 0: PenStyle=PS_SOLID; break;
    case 1: PenStyle=PS_DASH; break;
    case 2: PenStyle=PS_DOT; break;
    case 3: PenStyle=PS_DASH; break;
  }
  #endif
}

// ----------------------------------------------------------------------- */
// -- RENVOI LA HAUTEUR EN PIXEL D'UN TEXTE ------------------------------ */
// ----------------------------------------------------------------------- */
int hauteur_text(char *Texte) {
    #if WINDOWS
    DWORD Valeur;

    Valeur=GetTextExtent(hdc,Texte,strlen(Texte));
    return HIWORD(Valeur);
    #else
    struct _fontinfo info;

    Texte=Texte;
    _getfontinfo(&info);
    return info.pixheight;
    #endif
}

// ----------------------------------------------------------------------- */
// -- RENVOI LA LONGUEUR EN PIXEL D'UN TEXTE ----------------------------- */
// ----------------------------------------------------------------------- */
int largeur_text(char *Texte) {
  #if WINDOWS
  DWORD Valeur;

  Valeur=GetTextExtent(hdc,Texte,strlen(Texte));
  return LOWORD(Valeur);
  #else
  return _getgtextextent(Texte);
  #endif
}

// ----------------------------------------------------------------------- */
// -- DEFINI UN PORT DE SORTIE GRAPHIQUE --------------------------------- */
// ----------------------------------------------------------------------- */
void set_port(int X1,int Y1,int X2,int Y2) {
  #if !WINDOWS
  _setwindow(0,X1,Y1,X2,Y2);
  _setviewport(X1,Y1,X2,Y2);
  #else
  /*
  HRGN hRgnClip;

  hRgnClip=CreateRectRgn(X1,Y1,X2,Y2);
  SelectClipRgn(hdc,hRgnClip); */
  #endif
}

// ----------------------------------------------------------------------- */
// -- PLACE LE CURSEUR EN MODE TEXTE ------------------------------------- */
// ----------------------------------------------------------------------- */
void goto_xy(unsigned char X,unsigned char Y) {
  #if __WATCOMC__ && !WINDOWS
    regs.w.ax=0x0200;
    regs.w.bx=0;
    regs.w.dx=Y*0xFF+X;
    int386(0x10,&regs,&regs);
  #endif
}

// ----------------------------------------------------------------------- */
// -- RETOURNE LA POSITION DU CURSEUR EN MODE TEXTE ---------------------- */
// ----------------------------------------------------------------------- */
unsigned char where_y(void) {
  #if __WATCOMC__ && !WINDOWS
    regs.w.ax=0x0300;
    regs.w.bx=0;
    int386(0x10,&regs,&regs);
    return regs.h.dh+1;
  #endif
}

// ----------------------------------------------------------------------- */
// -- RETOURNE AU MODE TEXTE --------------------------------------------- */
// ----------------------------------------------------------------------- */
void text_mode(void) {
  #if !WINDOWS
  _setvideomode(_TEXTC80);
  #endif
}

// -----------------------------------------------------------------------
// -- MODIFIE LE BORDER GRAPHIQUE ----------------------------------------
// -----------------------------------------------------------------------
void g_border(byte C) {
  #if !WINDOWS
  regs.w.ax=0x1001;
  regs.h.bh=C;
  #endif
}

// -----------------------------------------------------------------------
// -- AFFICHE UN CERCLE EN X,Y, DE RAYON R ET COULEUR C ------------------
// -----------------------------------------------------------------------
void cercle(int X,int Y,int R,byte C) {
  #if WINDOWS
  #else
  _setcolor(C);
  _ellipse_w(0,X-R,Y-R,X+R,Y+R);
  #endif
}

// -----------------------------------------------------------------------
// -- RETOURNE LE MODE VIDEO COURANT PAR LE BIOS -------------------------
// -----------------------------------------------------------------------
byte get_video_mode(void) {
  #if !WINDOWS
  union REGS regs;

  regs.w.ax=0x0f00;
  int386(0x10,&regs,&regs);
  return regs.h.al;
  #endif
}
