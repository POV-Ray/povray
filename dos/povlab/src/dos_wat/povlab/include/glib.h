#ifndef GLIB_H_INCLUDED

#include "LIB.H"

#define COPY_PUT  0
#define XOR_PUT   1
#define HIDE_PUT  2

#define FADEOUT   0
#define FADEIN    1
#define MOTIF    -1

#define _R 0
#define _V 1
#define _B 2

#define BOUTON    0
#define CASE      1
#define ZONETEXTE 2
#define POTAR     3
#define BARRE     4
#define PASTILLE  5

struct Coord {
  DBL X;
  DBL Y;
};

struct Video {
  // int XM;
  // int YM;
  int WX,WY;
  int WXs2,WYs2;
  int XF,YF;
  int X1,Y1,X2,Y2;
  DBL Echelle;
  struct Coord Depla;
  struct Coord Max;
  struct Coord Min;
  byte Enable;
  byte OptSnap;
  byte Coord;
  DBL SnapX;
  DBL SnapY;
};

extern struct Coord Val;
extern struct Video Vid[4];

extern byte CINVISIBLE;
extern byte CMODIF;
extern byte CSELECTION;
extern byte COBJET;
extern byte CCAMERA;
extern byte CAXE;
extern byte CGRILLE;

extern byte NOIR ;
extern byte BLANC;
extern byte ROUGE;
extern byte BLEU;
extern byte JAUNE;
extern byte TEXTE;
extern byte FFOND;   // Fond fenˆtre filaire
extern byte FOND ;   // Couleur fenˆtre g‚n‚rale
extern byte ZFOND;   // Couleur fond zone de texte
extern byte OMBRE;   // Ombre du relief
extern byte ECLAIRE; // Partie ‚clair‚e du relief

extern int XMax,YMax;

extern char Resolution[5];
extern char StrBoite[20][256];
extern int NbCouleurs;

void message_aide(int X,int Y,byte T,byte Long,char *String,byte Type);
void gprintf(int Y,int X,byte T,byte F,byte Motif,byte Max,char *String,...);
void windows(int x1,int y1,int x2,int y2,byte Type,byte Fond);
void border(int x1,int y1,int x2,int y2,byte,byte);
void message(char *String,...);
byte test_ligne(int x1,int y1,int x2,int y2,int MX,int MY);
void ecran_off(byte Valeur);
byte g_boite_ONA(int X,int Y,byte NbL,byte Type,byte Boutons);
void g_fenetre(int X1,int Y1,int X2,int Y2,char *Titre,byte Travail);
int  affiche_palette(void);
void g_jauge(byte Num,int XA,int YA,int Long,int Haut,long P100,long CP100,byte Val,char *Texte);
void g_print(int X,int Y,byte T,byte F,byte Motif,char *Texte,byte Max);
void relief(int x1,int y1,int x2,int y2,byte Creux);
void f_jauge(byte Nb,byte Travail,long PC,long CPC,char *Texte);
void f_erreur(char *String,...);
byte f_bool(char *String,...);
void vignette_couleur(int X1,int Y1,int X2,int Y2,byte C,byte B);
byte x_fenetre(int X,int Y,byte Type,byte Boutons,char *String,...);
void cercle(int X,int Y,int R,byte C);

// ------------------------------------------------------ GCONVERT.C

byte init_gmode(byte);
void g_ligne(int X1,int Y1,int X2,int Y2,byte Color);
int g_bitmap(int X1,int Y1,int X2,int Y2,byte Travail,int Num);
byte get_pixel(int X1,int Y1);
void put_pixel(int X1,int Y1,byte Color);
byte get_color(void);
void set_color(byte Color);
void move_to(int X,int Y);
void g_ligne_to(int X,int Y,byte Color);
void g_rectangle(int X1,int Y1,int X2,int Y2,byte Color,int Type);
void text_xy(int X,int Y,char *Texte,byte color);
void type_ecriture(byte Type);
void ligne_rel(int X,int Y,byte Color);
void type_motif_ligne(byte Motif);
void fin_mode_graphique(void);
int largeur_text(char *Texte);
int hauteur_text(char *Texte);
void set_port(int X1,int Y1,int X2,int Y2);
void init_police(byte Travail,byte Nb);
void goto_xy(unsigned char X,unsigned char Y);
unsigned char where_y(void);
void text_mode(void);
void g_border(byte C);
byte get_video_mode(void);
void fast_line_to(int X,int Y,byte Color);

// ------------------------------------------------------ SELCTEUR.C

typedef struct menu {
  int X;
  int Y;
  int Nb;
  int NARG;
  char Element[500][32];
  byte Long;
  int NumElem;
  int NumElemNow;
  int PosiF;
  int OldNumElem;
};

struct retour_selecteur {
  int Ok;
  int Num;
};

extern struct menu Mn[2];

void init_selecteur(byte Num,int XG,int YG,int Nb,int NARG,char *Element[],int Long);
void affiche_selecteur(int N);
struct retour_selecteur test_selecteur(int N);
struct retour_selecteur selecteur(byte Num,byte Affiche);

// ------------------------------------------------------ POTAR.C

#define LC 6  // Longueur du curseur
#define HC 8  // Hauteur du curseur

typedef struct potentiometre {
  int X,Y,Long;
  DBL Max,Min,Val,Pas;
  char Aide[50];
  char Curseur[6+((LC*2)*(HC*2)*2)];
};

extern struct potentiometre Potar[20];

void modif_curseur(int N,byte Travail);
void init_potar(int N,int XC,int YC,int LongC,DBL MinC,DBL MaxC,DBL ValC,DBL Pas,char *Aide);
void affiche_potar(int N);
int test_potar(int Num1,int Num2);

// ------------------------------------------------------ BOUTON.C

typedef struct Bouton {
  int X,Y,L,H;
  char Txt[20];
  byte Place;
  byte Click;
  char Aide[50];
};

extern struct Bouton Bt[60];

void init_bouton(int Num,int X,int Y,int L,int H,char *String,byte Place,byte Click,char *Aide);
void affiche_bouton(int N);
int  test_bouton(int Debut,int Fin);
void click_bouton(int N);
int  bouton_dialog(int X1,int Y1,int X2,byte Travail,byte Nb);

// ------------------------------------------------------ ZONETEXT.C

typedef struct ZoneDeTexte {
  int X,Y;
  char Txt[20];
  char Variable[64];
  byte Long;
  char Aide[50];
};

extern struct ZoneDeTexte ZTexte[20];

void g_print_zone(int X,int Y,byte T,byte F,char *Texte,byte Max);
int  zone_texte(byte Num);
void init_texte(int Num,int X,int Y,char *String,char *Variable,byte Long,char *Aide);
int  test_texte(int Debut,int Fin);
void place_zone_texte(int Num);

// ------------------------------------------------------ CASE.C

typedef struct CaseACocher {
  int X,Y;
  char Txt[50];
  byte Croix;
  char Aide[50];
};

extern struct CaseACocher Cc[40];

void coche_case(int N,byte Ok);
void affiche_case(int N);
int test_case(int Debut,int Fin);
void init_case(int Num,int X,int Y,char *String,byte Ok,char *Aide);

// ------------------------------------------------------ PASTILLE.C

typedef struct CasePastille {
  int X,Y;
  char Txt[30];
  byte Croix;
  char Aide[50];
};

extern struct CasePastille Pastille[40];

void coche_pastille(int N,byte Ok);
void affiche_pastille(int N);
int test_pastille(int Debut,int Fin);
void init_pastille(int Num,int X,int Y,char *String,byte Ok,char *Aide);
int test_groupe_pastille(int Debut,int Fin);
int quelle_pastille_dans_groupe(int N1,int N2);

#define GLIB_H_INCLUDED
#endif
