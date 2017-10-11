/* ---------------------------------------------------------------------------
*  AREA.C
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

#include <I86.H>
#include <STDLIB.H>
#include <STDIO.H>
#include <DOS.H>
#include <STRING.H>
#include <FLOAT.H>
#include <MATH.H>
#include "LIB.H"
#include "GLOBAL.H"
#include "GLIB.H"

struct Lumiere_Area Area[NB_AREA_MAX+1];
byte NbArea=0;

// -------------------------------------------------------------------------
// -- EFFACE LIENS LOOKS LIKE AVEC AREA ET OBJETS --------------------------
// -------------------------------------------------------------------------
void efface_liens_looks_like_area(int N) {
  register int i;

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->LooksLike.Nb==N && Objet[i]->LooksLike.Light==AREA) {
      Objet[i]->LooksLike.Nb=Objet[i]->LooksLike.Light=0;
    }
  }
}

// -------------------------------------------------------------------------
// -- MODIF SOURCE AREA OMBRES ---------------------------------------------
// -------------------------------------------------------------------------
void ombre_area(byte N) {
  Area[N].Ombre=!Area[N].Ombre;
  message("Area light nø%d %s shadows",N,Area[N].Ombre ? "with":"without");
  while (MouseB());
  delay(1000);
}

// -------------------------------------------------------------------------
// -- INITIALISE LES COORDONNEES DE L'AREA ---------------------------------
// -------------------------------------------------------------------------
void xyz_area(byte Num,DBL X1,DBL Y1,DBL Z1) {
  Area[Num].Point[_X]=X1;
  Area[Num].Point[_Y]=Y1;
  Area[Num].Point[_Z]=Z1;
  Area[Num].RVB[_R]=255;
  Area[Num].RVB[_V]=255;
  Area[Num].RVB[_B]=255;
  Area[Num].Taille=3;
  Area[Num].OnOff=1;
  Area[Num].Ombre=1;
  Area[Num].F_Dist=100.0;
  Area[Num].F_Power=1;
  Area[Num].Fade=0;
  Area[Num].Atmos=0;
  Area[Num].Atmos_Att=0;
  vect_init(Area[Num].Axis1,-0.5,-0.5,-0.5);
  vect_init(Area[Num].Axis2, 0.5, 0.5, 0.5);
  Area[Num].Size1=3;
  Area[Num].Size2=3;
  Area[Num].Jitter=1;
  Area[Num].Adaptive=1;
  strcpy(Area[Num].Flare,"None");
}

// -------------------------------------------------------------------------
// -- DECLARE LES COORDONNEES D'UNE NOUVELLE AREA -----------------------
// -------------------------------------------------------------------------
void new_area(void) {
  cache_area(1,1,NbArea);

  if (incr_NbArea(0)) {
    Area[0].Cache=0;
    xyz_area(NbArea,0,0,0);
    cache_area(0,1,NbArea);
    affiche_donnees();
    message("New area light created");
  }
}

// -------------------------------------------------------------------------
// -- TRACE LES AREAS DANS LES FENETRES ---------------------------------
// -------------------------------------------------------------------------
void affiche_area(byte Num,byte Vue,byte Mode) {
  register DBL X1,Y1,R;
  register byte C;
  int LO=Area[Num].Taille;
  char Nb[6];

  if (Vid[Vue].Enable==0) return;
  if (Fx4==0 && Vue!=NF) return;
  if (Area[0].Cache) return;
  
  C=(Mode==COPY_PUT ? type_couleur(AREA):(FFOND | BLANC));
  if (Mode==HIDE_PUT) { C=FFOND; Mode=COPY_PUT; } // efface
  type_ecriture(Mode);
  sprintf(Nb,"[%0d%c]",Num,Area[Num].OnOff ? '+':'-');

  if (Fx4 || (Fx4==0 && NF==0)) {
    X1=Vid[0].Echelle*(Area[Num].Point[_X]+Vid[0].Depla.X)+Vid[0].WXs2;
    Y1=Vid[0].Echelle*(Area[Num].Point[_Y]+Vid[0].Depla.Y)+Vid[0].WYs2;
    R=Vid[0].Echelle*Area[Num].Rayon;
    select_vue(0,CLIP_ON);
    g_ligne(X1-LO,Y1-LO,X1+LO,Y1+LO,C);
    g_ligne(X1+LO,Y1-LO,X1-LO,Y1+LO,C);
    g_ligne(X1   ,Y1-LO,X1   ,Y1+LO,C);
    g_ligne(X1-LO,Y1   ,X1+LO,Y1   ,C);
    cercle(X1,Y1,R,C);
    text_xy(X1+10,Y1-10,Nb,C);
  }

  if (Fx4 || (Fx4==0 && NF==1)) {
    X1=Vid[1].Echelle*(Area[Num].Point[_Z]+Vid[1].Depla.X)+Vid[1].WXs2;
    Y1=Vid[1].Echelle*(Area[Num].Point[_Y]+Vid[1].Depla.Y)+Vid[1].WYs2;
    R=Vid[1].Echelle*Area[Num].Rayon;
    select_vue(1,CLIP_ON);
    g_ligne(X1-LO,Y1-LO,X1+LO,Y1+LO,C);
    g_ligne(X1+LO,Y1-LO,X1-LO,Y1+LO,C);
    g_ligne(X1   ,Y1-LO,X1   ,Y1+LO,C);
    g_ligne(X1-LO,Y1   ,X1+LO,Y1   ,C);
    cercle(X1,Y1,R,C);
    text_xy(X1+10,Y1-10,Nb,C);
  }

  if (Fx4 || (Fx4==0 && NF==2)) {
    X1=Vid[2].Echelle*(Area[Num].Point[_X]+Vid[2].Depla.X)+Vid[2].WXs2;
    Y1=Vid[2].Echelle*(Area[Num].Point[_Z]+Vid[2].Depla.Y)+Vid[2].WYs2;
    R=Vid[2].Echelle*Area[Num].Rayon;
    select_vue(2,CLIP_ON);
    g_ligne(X1-LO,Y1-LO,X1+LO,Y1+LO,C);
    g_ligne(X1+LO,Y1-LO,X1-LO,Y1+LO,C);
    g_ligne(X1   ,Y1-LO,X1   ,Y1+LO,C);
    g_ligne(X1-LO,Y1   ,X1+LO,Y1   ,C);
    cercle(X1,Y1,R,C);
    text_xy(X1+10,Y1-10,Nb,C);
  }

  if (Fx4 || (Fx4==0 && NF==3)) affiche_area_3d(Num,C);

  type_ecriture(COPY_PUT);
}

// -------------------------------------------------------------------------
// -- SELECTIONNE UNE AREA ----------------------------------------------
// -------------------------------------------------------------------------
CibleOeil selection_area(void) {
  register DBL X,Y;
  register int i;
  CibleOeil Valeur;

  for (i=1;i<=NbArea;i++) {
    switch(NF) {
      case 0:
        X=Vid[0].WXs2+(Area[i].Point[_X]+Vid[0].Depla.X)*Vid[0].Echelle;
        Y=Vid[0].WYs2+(Area[i].Point[_Y]+Vid[0].Depla.Y)*Vid[0].Echelle;
        break;
      case 1:
        X=Vid[1].WXs2+(Area[i].Point[_Z]+Vid[1].Depla.X)*Vid[1].Echelle;
        Y=Vid[1].WYs2+(Area[i].Point[_Y]+Vid[1].Depla.Y)*Vid[1].Echelle;
        break;
      case 2:
        X=Vid[2].WXs2+(Area[i].Point[_X]+Vid[2].Depla.X)*Vid[2].Echelle;
        Y=Vid[2].WYs2+(Area[i].Point[_Z]+Vid[2].Depla.Y)*Vid[2].Echelle;
        break;
    }
    if (VRAI==test_ligne(X-5,Y-5,X+5,Y+5,gmx_v(),gmy_v()) ||
        VRAI==test_ligne(X+5,Y-5,X-5,Y+5,gmx_v(),gmy_v())) {
      Valeur.Type=AREA;
      Valeur.Num=i;
      return Valeur;
    }
  }

  Valeur.Type=0;
  Valeur.Num=0;
  return Valeur;
}

// -------------------------------------------------------------------------
// -- DEPLACE UNE AREA --------------------------------------------------
// -------------------------------------------------------------------------
void deplace_area(byte NA) {
  register DBL X,Y,XA,YA,XB,YB;
  register DBL DX,DY,E;
  int MX,MY;

  E=Vid[NF].Echelle;

  if (NF==0) { X=DX=Area[NA].Point[_X]; Y=DY=Area[NA].Point[_Y]; }
  if (NF==1) { X=DX=Area[NA].Point[_Z]; Y=DY=Area[NA].Point[_Y]; }
  if (NF==2) { X=DX=Area[NA].Point[_X]; Y=DY=Area[NA].Point[_Z]; }
  
  forme_mouse(Sens);
  MX=gmx_r();
  MY=gmy_r();
  while (MouseB());
  GMouseOff();
  affiche_area(NA,NF,XOR_PUT);
  
  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();
  
  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) {
      X=DX;
      Y=DY;
      place_mouse(MX,MY);
      break;
    }
    if (XA!=XB || YA!=YB) {
      delay(5);
      affiche_area(NA,NF,XOR_PUT);
      switch (Sens) {
        case MS_X:
          X+=(DBL) (XA-XB)/E;
          break;
        case MS_Y:
          Y+=(DBL) (YA-YB)/E;
          break;
        default:
          X+=(DBL) (XA-XB)/E;
          Y+=(DBL) (YA-YB)/E;
          break;
      }
      if ((kbhit()) && getch()==9) {
        affiche_area(NA,NF,XOR_PUT);
        X=DX;
        Y=DY;
        Sens_Souris();
        affiche_area(NA,NF,XOR_PUT);
      }
      if (NF==0) { Area[NA].Point[_X]=X; Area[NA].Point[_Y]=Y; }
      if (NF==1) { Area[NA].Point[_Z]=X; Area[NA].Point[_Y]=Y; }
      if (NF==2) { Area[NA].Point[_X]=X; Area[NA].Point[_Z]=Y; }
      affiche_area(NA,NF,XOR_PUT);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("X=%+.2lf Y=%+.2lf",X,-Y);

      select_vue(NF,CLIP_ON);
    }
  }

  affiche_area(NA,NF,XOR_PUT);

  if (NF==0) { Area[NA].Point[_X]=DX; Area[NA].Point[_Y]=DY; }
  if (NF==1) { Area[NA].Point[_Z]=DX; Area[NA].Point[_Y]=DY; }
  if (NF==2) { Area[NA].Point[_X]=DX; Area[NA].Point[_Z]=DY; }

  cache_area(1,NA,NA);

  if (X!=DX || Y!=DY) {
    MX+=((X-DX)*Vid[NF].Echelle);
    MY+=((Y-DY)*Vid[NF].Echelle);
    while (MouseB());
    if (NF==0) { Area[NA].Point[_X]=X; Area[NA].Point[_Y]=Y; }
    if (NF==1) { Area[NA].Point[_Z]=X; Area[NA].Point[_Y]=Y; }
    if (NF==2) { Area[NA].Point[_X]=X; Area[NA].Point[_Z]=Y; }
  }

  cache_area(0,NA,NA);
  while (MouseB());
  place_mouse(MX,MY);
}

// -------------------------------------------------------------------------
// -- SUPPRIME UNE SOURCE DE LUMIERE ---------------------------------------
// -------------------------------------------------------------------------
void supprime_area(byte NA) {
  register byte i;

  forme_mouse(MS_FLECHE);
  strcpy(StrBoite[0],"Light destruction");
  strcpy(StrBoite[1],"Do you really want to");
  sprintf(StrBoite[2],"delete the area light nø%d ?",NA);

  if (g_boite_ONA(CentX,CentY,2,CENTRE,1)==0) {
    efface_liens_looks_like_area(NA);
    cache_area(1,1,NbArea);
    for (i=NA;i<=NbArea;i++) {
      Area[i]=Area[i+1];
    }
    NbArea--;
    cache_area(0,1,NbArea);
    affiche_donnees();
  }
}

/* ------------------------------------------------------------------------- */
/* -- TEST MIN ET MAX DE LA LUMIERE ---------------------------------------- */
/* ------------------------------------------------------------------------- */
void max_min_area (byte Vue) {
  register byte i;

  if (Area[0].Cache) return;

  if (Vue==0) {
    for (i=1;i<=NbArea;i++) {
      Vid[0].Max.X=(Area[i].Point[_X]>Vid[0].Max.X ? Area[i].Point[_X]:Vid[0].Max.X);
      Vid[2].Min.X=(Area[i].Point[_X]<Vid[2].Min.X ? Area[i].Point[_X]:Vid[2].Min.X);
      Vid[0].Max.Y=(Area[i].Point[_Y]>Vid[0].Max.Y ? Area[i].Point[_Y]:Vid[0].Max.Y);
      Vid[2].Min.Y=(Area[i].Point[_Y]<Vid[2].Min.Y ? Area[i].Point[_Y]:Vid[2].Min.Y);
    }
    return;
  }

  if (Vue==1) {
    for (i=1;i<=NbArea;i++) {
      Vid[1].Max.X=(Area[i].Point[_Z]>Vid[1].Max.X ? Area[i].Point[_Z]:Vid[1].Max.X);
      Vid[1].Min.X=(Area[i].Point[_Z]<Vid[1].Min.X ? Area[i].Point[_Z]:Vid[1].Min.X);
      Vid[1].Max.Y=(Area[i].Point[_Y]>Vid[1].Max.Y ? Area[i].Point[_Y]:Vid[1].Max.Y);
      Vid[1].Min.Y=(Area[i].Point[_Y]<Vid[1].Min.Y ? Area[i].Point[_Y]:Vid[1].Min.Y);
    }
    return;
  }

  if (Vue==2) {
    for (i=1;i<=NbArea;i++) {
      Vid[2].Max.X=(Area[i].Point[_X]>Vid[2].Max.X ? Area[i].Point[_X]:Vid[2].Max.X);
      Vid[2].Min.X=(Area[i].Point[_X]<Vid[2].Min.X ? Area[i].Point[_X]:Vid[2].Min.X);
      Vid[2].Max.Y=(Area[i].Point[_Z]>Vid[2].Max.Y ? Area[i].Point[_Z]:Vid[2].Max.Y);
      Vid[2].Min.Y=(Area[i].Point[_Z]<Vid[2].Min.Y ? Area[i].Point[_Z]:Vid[2].Min.Y);
    }
    return;
  }
}

// -------------------------------------------------------------------------
// -- AUGMENTE LE NOMBRE D'AREA -----------------------------------------
// -------------------------------------------------------------------------
byte incr_NbArea(byte Val) {
  if (NbArea<NB_AREA_MAX) { NbArea++; return 1; }
  if (Val) {
    forme_mouse(MS_FLECHE);
    strcpy(StrBoite[0],"SHAREWARE VERSION");
    strcpy(StrBoite[1],"The number of area light in the shareware");
    sprintf(StrBoite[2],"version is limited to: %d",NB_AREA_MAX);
    strcpy(StrBoite[3],"");
    strcpy(StrBoite[4],"PLEASE, REGISTER !");
    g_boite_ONA(CentX,CentY,4,CENTRE,0);
  }
  NbArea=NB_AREA_MAX;
  return 0;
}

// ----------------------------------------------------------------------------
// -- TEST LIMITE AREA (COULEUR,TAILLE) ------------------------------------
// ----------------------------------------------------------------------------
void test_limite_area(byte N) {
  /*
  Area[N].RVB[_R]=(Area[N].RVB[_R]>255 ? 255:Area[N].RVB[_R]);
  Area[N].RVB[_V]=(Area[N].RVB[_V]>255 ? 255:Area[N].RVB[_V]);
  Area[N].RVB[_B]=(Area[N].RVB[_B]>255 ? 255:Area[N].RVB[_B]);
  */

  Area[N].Taille=(Area[N].Taille>MAX_TAILLE_LUMIERE ? 3:Area[N].Taille);
  Area[N].Taille=(Area[N].Taille<MIN_TAILLE_LUMIERE ? 3:Area[N].Taille);
  Area[N].Rayon=(Area[N].Rayon<0 ? 1:Area[N].Rayon);
}

// ----------------------------------------------------------------------------
// -- TEST SI AREA EST A 0,0,0 en RVB --------------------------------------
// ----------------------------------------------------------------------------
byte test_couleur_area(byte N1,byte N2) {
  int i;

  if (NbArea==0) return 1;

  for (i=N1;i<=N2;i++) {
    if (Area[i].RVB[_R]==0 && Area[i].RVB[_V]==0 && Area[i].RVB[_B]==0) {
      forme_mouse(MS_FLECHE);
      strcpy(StrBoite[0],"SWITCHED OFF AREA LIGHT");
      sprintf(StrBoite[1],"Do you really want the area light nø%d",i);
      strcpy(StrBoite[2],"to diffuse no light (RVB=0,0,0) ?");
      if (g_boite_ONA(CentX,CentY,2,CENTRE,1)==0) return 1; else return 0;
    }
  }

  return 1;
}

// -------------------------------------------------------------------------
// -- AFFICHE OU CACHE LES LUMIERES ----------------------------------------
// -------------------------------------------------------------------------
void cache_area(byte Mode,byte D,byte F) {
  register int i;

  Area[0].Cache=0;

  if (!Mode) {
    for (i=D;i<=F;i++) {
      affiche_area(i,0,COPY_PUT);
      affiche_area(i,1,COPY_PUT);
      affiche_area(i,2,COPY_PUT);
    }
  }

  if (Mode) {
    for (i=D;i<=F;i++) {
      affiche_area(i,0,HIDE_PUT);
      affiche_area(i,1,HIDE_PUT);
      affiche_area(i,2,HIDE_PUT);
    }
    Area[0].Cache=1;
  }
}

// -------------------------------------------------------------------------
// -- MODIFIE LA COULEUR DE LA LUMIERE -------------------------------------
// -------------------------------------------------------------------------
void couleur_area(byte N) {
  creation_couleur(&Area[N].RVB[_R],&Area[N].RVB[_V],&Area[N].RVB[_B],"Area light color");
}

// ----------------------------------------------------------------------------
// -- MODIFIE LA TAILLE DE LA LUMIERE (DANS MODELEUR) -------------------------
// ----------------------------------------------------------------------------
void taille_area(byte NA) {
  register int X=CentX;
  register int Y=CentY;
  register int X1=X-120;
  register int X2=X+120;
  register int Y1=Y-100;
  register int Y2=Y+115;
  register int i,j=0,k;
  register byte T;
  register TF;

  forme_mouse(MS_FLECHE);
  GMouseOn();

  T=Area[NA].Taille;
  TF=MAX_TAILLE_LUMIERE+10;
  g_fenetre(X1,Y1,X2,Y2,"WIDTH/HEIGHT OF AN AREA LIGHT",AFFICHE);
  relief(X-TF,Y-TF-9,X+TF,Y+TF-9,0);

  init_potar(0,X1+30,Y2-46,120,MIN_TAILLE_LUMIERE,MAX_TAILLE_LUMIERE,T,1,"Width and height of the area light");
  init_bouton(40,X1+(X2-X1)/2-25,Y2-30,50,20,"Ok",CENTRE,ATTEND,"Validate the choice");

  affiche_potar(0);
  affiche_bouton(40);
  Y-=9;

  while (1) {
    test_potar(0,0);
    if (test_bouton(40,40)==40) { i=1; break; }
    if (kbhit()) {
      i=getch();
      if (i==27) i=0;
      if (i==13) i=1;
      break;
    }
    if (j!=(i=Potar[0].Val)) {
      j=i;
      k=j+1;
      i=TF-2;;
      g_ligne(X-j,Y-j,X+j,Y+j,type_couleur(AREA));
      g_ligne(X+j,Y-j,X-j,Y+j,type_couleur(AREA));
      g_ligne(X  ,Y-j,X  ,Y+j,type_couleur(AREA));
      g_ligne(X-j,Y  ,X+j,Y  ,type_couleur(AREA));

      g_ligne(X,Y-k,X,Y-i,FOND);
      g_ligne(X,Y+k,X,Y+i,FOND);

      g_ligne(X-k,Y,X-i,Y,FOND);
      g_ligne(X+k,Y,X+i,Y,FOND);

      g_ligne(X+k,Y-k,X+i,Y-i,FOND);
      g_ligne(X-k,Y-k,X-i,Y-i,FOND);

      g_ligne(X+k,Y+k,X+i,Y+i,FOND);
      g_ligne(X-k,Y+k,X-i,Y+i,FOND);
    }
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  for (j=0;j<=2;j++) {
    affiche_area(NA,j,HIDE_PUT);
  }

  Area[NA].Taille=(byte)(i==1 ? Potar[0].Val:T);

  for (j=0;j<=2;j++) {
    affiche_area(NA,j,COPY_PUT);
  }
}

// -------------------------------------------------------------------------
// -- MODIFIE LE RAYON D'UNE AREA ---------------------------------------
// -------------------------------------------------------------------------
void rayon_area(byte NA) {
  register DBL X,Y;
  int XA,YA,XB,YB;
  register DBL E,R=1;
  register DBL RD;

  E=Vid[NF].Echelle;

  RD=Area[NA].Rayon;
  if (NF==0) { X=Area[NA].Point[_X]; Y=Area[NA].Point[_Y]; }
  if (NF==1) { X=Area[NA].Point[_Z]; Y=Area[NA].Point[_Y]; }
  if (NF==2) { X=Area[NA].Point[_X]; Y=Area[NA].Point[_Z]; }
  
  forme_mouse(Sens);
  while (MouseB());
  GMouseOff();
  affiche_area(NA,NF,XOR_PUT);
  
  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();
  
  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) { R=1; break; }
    if (XA!=XB || YA!=YB) {
      delay(5);
      affiche_area(NA,NF,XOR_PUT);
      R+=(((XA-XB)+(YA-YB))*0.5)/E;
      Area[NA].Rayon=RD*R;
      affiche_area(NA,NF,XOR_PUT);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("Radius along X-Y-Z=%.2lf%",fabs(RD*R));
      select_vue(NF,CLIP_ON);
    }
  }

  affiche_area(NA,NF,XOR_PUT);

  Area[NA].Rayon=RD;
  affiche_area(NA,NF,HIDE_PUT);

  if (R!=1) {
    Area[NA].Rayon=fabs(R*RD);
  }

  affiche_area(NA,NF,COPY_PUT);
  while (MouseB());
}

// -------------------------------------------------------------------------
// -- MODIF SOURCE AREA ON/OFF ------------------------------------------
// -------------------------------------------------------------------------
void on_off_area(byte N) {
  cache_area(1,N,N);
  Area[N].OnOff=!Area[N].OnOff;
  cache_area(0,N,N);
  message("Area light nø%d %s",N,Area[N].OnOff ? "switched on":"switched off");
  while (MouseB());
  delay(1000);
}

  
// -------------------------------------------------------------------------
// -- ATTRIBUTS ET SETUP AREA -------------------------------------------
// -------------------------------------------------------------------------
void setup_area(byte Num) {
  int X1=CentX-180;
  int X2=CentX+175;
  int Y1=CentY-160;
  int Y2=CentY+160;
  int i;
  byte Ombre;
  DBL F_Dist;
  byte F_Power;
  byte Fade;
  byte Atmos;
  byte Atmos_Att;
  byte OnOff;
  byte Jitter;
  int XB,YB;
  int XC,YC;
  int XA,YA;
  int XD,YD;

  forme_mouse(MS_FLECHE);

  {
     char Buffer[256];
     sprintf(Buffer,"Arealight #%d setup",Num);
     g_fenetre(X1,Y1,X2,Y2,Buffer,AFFICHE);
  }

  XA=X1+10;
  YA=Y1+29;
  XB=X1+190;
  YB=Y1+29;
  XC=X1+190;
  YC=Y1+112;
  XD=X1+190;
  YD=Y1+195;

  Ombre=Area[Num].Ombre;
  F_Dist=Area[Num].F_Dist;
  F_Power=Area[Num].F_Power;
  Fade=Area[Num].Fade;
  Atmos_Att=Area[Num].Atmos_Att;
  Atmos=Area[Num].Atmos;
  OnOff=Area[Num].OnOff;
  Jitter=Area[Num].Jitter;
  sprintf(ZTexte[1].Variable,"%.4g",Area[Num].Point[_X]);
  sprintf(ZTexte[2].Variable,"%.4g",-Area[Num].Point[_Y]);
  sprintf(ZTexte[3].Variable,"%.4g",-Area[Num].Point[_Z]);

  sprintf(ZTexte[4].Variable,"%.4g",Area[Num].Axis1[_X]);
  sprintf(ZTexte[5].Variable,"%.4g",Area[Num].Axis1[_Y]);
  sprintf(ZTexte[6].Variable,"%.4g",Area[Num].Axis1[_Z]);
  sprintf(ZTexte[7].Variable,"%.4g",Area[Num].Axis2[_X]);
  sprintf(ZTexte[8].Variable,"%.4g",Area[Num].Axis2[_Y]);
  sprintf(ZTexte[9].Variable,"%.4g",Area[Num].Axis2[_Z]);

  sprintf(ZTexte[10].Variable,"%.4g",Area[Num].Adaptive);

  border(XA,YA,XA+158,YA+96,0,1);
  init_case(11,XA+10,YA+10,"Fade source",Fade,"Light fading or not");
  sprintf(ZTexte[0].Variable,"%.4g",F_Dist);
  init_texte(0,XA+82,YA+30,"Fade distance",ZTexte[0].Variable,7,"Set fading distance");
  init_pastille(11,XA+10,YA+50,"Fade power linear",(F_Power==1),"Use linear algo");
  init_pastille(12,XA+10,YA+70,"Fade power quadratic",(F_Power==2),"Use uadradic algo");

  init_case(12,X1+11,Y1+137,"Use atmospheric effect",Atmos,"With atmosphere");
  init_case(13,X1+11,Y1+157,"Atmospheric attenuation",Atmos_Att,"With atmosphere");
  init_case(14,X1+11,Y1+177,"Cast shadows",Ombre,"Cast shadows on objects");
  init_case(15,X1+11,Y1+197,"Light on",OnOff,"Switch light on/off");

  text_xy(X1+10,Y1+220,"Size1",NOIR);
  init_potar(0,X1+61,Y1+226,30,0,30,(DBL) Area[Num].Size1,1,"# points in H");
  text_xy(X1+10,Y1+240,"Size2",NOIR);
  init_potar(1,X1+61,Y1+246,30,0,30,(DBL) Area[Num].Size2,1,"# points in W");

  border(XB,YB,XB+151,YB+75,0,1);
  init_texte(1,XB+73,YB+11,"Location X",ZTexte[1].Variable,7,"Set X location");
  init_texte(2,XB+73,YB+31,"Location Y",ZTexte[2].Variable,7,"Set Y location");
  init_texte(3,XB+73,YB+51,"Location Z",ZTexte[3].Variable,7,"Set Z location");

  border(XC,YC,XC+151,YC+75,0,1);
  init_texte(4,XC+73,YC+11,"Axis 1 X",ZTexte[4].Variable,7,"Set X lenght 1");
  init_texte(5,XC+73,YC+31,"Axis 1 Y",ZTexte[5].Variable,7,"Set Y lenght 1");
  init_texte(6,XC+73,YC+51,"Axis 1 Z",ZTexte[6].Variable,7,"Set Z lenght 1");

  border(XD,YD,XD+151,YD+75,0,1);
  init_texte(7,XD+73,YD+11,"Axis 2 X",ZTexte[7].Variable,7,"Set X lenght 2");
  init_texte(8,XD+73,YD+31,"Axis 2 Y",ZTexte[8].Variable,7,"Set Y lenght 2");
  init_texte(9,XD+73,YD+51,"Axis 2 Z",ZTexte[9].Variable,7,"Set Z lenght 2");

  init_texte(10,X1+105,Y1+266,"Adaptive",ZTexte[10].Variable,7,"");
  init_case(16,X1+10,Y1+265,"Jitter",Jitter,"");

  text_xy(X1+10,Y1+295,"Lens Flare",TEXTE);
  strupr(Area[Num].Flare);
  init_bouton(59,X1+65,Y1+291,106,20,strinstr(0,Area[Num].Flare,".FLR")<0 ? "[None]":Area[Num].Flare,GAUCHE,ATTEND,"Select a lens flare type");

  affiche_bouton(59);
  affiche_case(11);
  affiche_case(12);
  affiche_case(13);
  affiche_case(14);
  affiche_case(15);
  affiche_case(16);
  for (i=0;i<=10;i++) place_zone_texte(i);
  affiche_pastille(11);
  affiche_pastille(12);
  affiche_potar(0);
  affiche_potar(1);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_texte(0,10);
    test_case(11,16);
    test_groupe_pastille(11,12);
    test_potar(0,1);
    if (test_bouton(59,59)==59) {
      flare_light(Area[Num].Flare,59);
      bouton_dialog(X1,X2,Y2,1,1);
    }
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    cache_area(1,Num,Num);
    Area[Num].Ombre=Cc[14].Croix;
    Area[Num].F_Dist=atof(ZTexte[0].Variable);
    Area[Num].F_Power=quelle_pastille_dans_groupe(11,12)-10;
    Area[Num].Fade=Cc[11].Croix;
    Area[Num].Atmos_Att=Cc[13].Croix;
    Area[Num].Atmos=Cc[12].Croix;
    Area[Num].OnOff=Cc[15].Croix;
    Area[Num].Point[_X]=atof(ZTexte[1].Variable);
    Area[Num].Point[_Y]=-atof(ZTexte[2].Variable);
    Area[Num].Point[_Z]=-atof(ZTexte[3].Variable);
    Area[Num].Jitter=(byte) Cc[16].Croix;
    Area[Num].Axis1[_X]=atof(ZTexte[4].Variable);
    Area[Num].Axis1[_Y]=atof(ZTexte[5].Variable);
    Area[Num].Axis1[_Z]=atof(ZTexte[6].Variable);
    Area[Num].Axis2[_X]=atof(ZTexte[7].Variable);
    Area[Num].Axis2[_Y]=atof(ZTexte[8].Variable);
    Area[Num].Axis2[_Z]=atof(ZTexte[9].Variable);
    Area[Num].Size1=(int) Potar[0].Val;
    Area[Num].Size2=(int) Potar[1].Val;
    Area[Num].Adaptive=atof(ZTexte[10].Variable);
    cache_area(0,Num,Num);
  }

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- LOOKS_LIKE EFFECT ----------------------------------------------------
// -------------------------------------------------------------------------
void looks_like_area(void) {
  CibleOeil Valeur;
  int N,i;

  if (pas_lumiere(1)) return;
  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_LOOKS_LIKE_AREA:

  message("Pick a arealight source for looks_like");
  forme_mouse(MS_SELECTEUR);

  if (cherche_fenetre()==FAUX) return;

  Valeur=selection_area();
  N=Valeur.Num;

  while (MouseB());
  if (Valeur.Num==0) goto LABEL_LOOKS_LIKE_AREA;

  forme_mouse(MS_SELECTEUR);
  message("Select object to make looks_like");

  if (Selection && OkSelect==1) {
	if (cherche_fenetre()==FAUX) return;
    cube_selection();
    NumObjet=0;
  } else {
    if ((NumObjet=trouve_volume(0,2,1))==FAUX) return;
  }

  
  if (OkSelect && Selection) {
    for (i=1;i<=NbObjet;i++) {
      if (Objet[i]->Selection) {
        Objet[i]->LooksLike.Nb=N;
        Objet[i]->LooksLike.Light=(Objet[i]->LooksLike.Light==AREA ? 0:AREA);
      }
    }
  } else {
    Objet[NumObjet]->LooksLike.Nb=N;
    Objet[NumObjet]->LooksLike.Light=(Objet[NumObjet]->LooksLike.Light==AREA ? 0:AREA);
  }

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- TESTE SI CETTE AREA EST LOOKS_LIKE --------------------------------
// -------------------------------------------------------------------------
byte si_area_looks_like(int N) {
  register int i;

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->LooksLike.Nb==N && Objet[i]->LooksLike.Light==AREA) {
      return 1;
    }
  }

  return 0;
}

// -------------------------------------------------------------------------
// -- AFFICHE UNE LUMIERE AREA DANS LA FENETRE 3D --------------------------
// -------------------------------------------------------------------------
void affiche_area_3d(int N,byte C) {

  if (Vid[3].Enable==0) return;
  if (Fx4==0) return;
  if (Area[0].Cache) return;

  Objet[0]->Type=OMNI;
  vect_init(Objet[0]->S,Area[N].Taille*0.03,
                        Area[N].Taille*0.03,
                        Area[N].Taille*0.03);
  vect_init(Objet[0]->T,Area[N].Point[_X],Area[N].Point[_Y],Area[N].Point[_Z]);
  vect_init(Objet[0]->R,0,0,0);
  Objet[0]->Couleur=C;
  Objet[0]->Cache=0;
  Objet[0]->Ignore=0;
  Objet[0]->Selection=0;
  Objet[0]->Freeze=0;
  Objet[0]->Rapide=0;
  Objet[0]->Operator=PAS_CSG;
  Objet[0]->CSG=PAS_CSG;

  trace_volume_3(0);
}
