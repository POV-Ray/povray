/* ---------------------------------------------------------------------------
*  OMNI.C
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

struct Lumiere_Omni Omni[NB_OMNI_MAX+1];
byte NbOmni=0;

// -------------------------------------------------------------------------
// -- EFFACE LIENS LOOKS LIKE AVEC OMNI ET OBJETS --------------------------
// -------------------------------------------------------------------------
void efface_liens_looks_like_omni(int N) {
  register int i;

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->LooksLike.Nb==N && Objet[i]->LooksLike.Light==OMNI) {
      Objet[i]->LooksLike.Nb=Objet[i]->LooksLike.Light=0;
    }
  }
}

// -------------------------------------------------------------------------
// -- INITIALISE LES COORDONNEES DE LA OMNI --------------------------------
// -------------------------------------------------------------------------
void xyz_omni(byte Num,DBL X1,DBL Y1,DBL Z1) {
  Omni[Num].Point[_X]=X1;
  Omni[Num].Point[_Y]=Y1;
  Omni[Num].Point[_Z]=Z1;
  Omni[Num].RVB[_R]=255;
  Omni[Num].RVB[_V]=255;
  Omni[Num].RVB[_B]=255;
  Omni[Num].Taille=3;
  Omni[Num].OnOff=1;
  Omni[Num].Ombre=1;
  Omni[Num].F_Dist=100.0;
  Omni[Num].F_Power=1;
  Omni[Num].Fade=0;
  Omni[Num].Atmos=0;
  Omni[Num].Atmos_Att=0;
  strcpy(Omni[Num].Flare,"None");
}

// -------------------------------------------------------------------------
// -- DECLARE LES COORDONNEES D'UNE NOUVELLE OMNI --------------------------
// -------------------------------------------------------------------------
void new_omni(void) {
  cache_omni(1,1,NbOmni);

  if (incr_NbOmni(0)) {
    xyz_omni(NbOmni,0,0,0);
    affiche_donnees();
    cache_omni(0,1,NbOmni);
    message("New omni light nø%d created",NbOmni);
  }
}

// -------------------------------------------------------------------------
// -- TRACE LA OMNI FENETRES -----------------------------------------------
// -------------------------------------------------------------------------
void affiche_omni(byte Num,byte Vue,byte Mode) {
  register DBL X1,Y1;
  register byte C;
  int LO=Omni[Num].Taille;
  char Nb[6];

  if (Vid[Vue].Enable==0) return;
  if (Fx4==0 && Vue!=NF) return;
  if (Omni[0].Cache) return;

  C=(Mode==COPY_PUT ? type_couleur(OMNI):(FFOND | BLANC));
  if (Mode==HIDE_PUT) { C=FFOND; Mode=COPY_PUT; } // efface
  type_ecriture(Mode);
  sprintf(Nb,"[%0d%c]",Num,Omni[Num].OnOff ? '+':'-');

  if (Fx4 || (Fx4==0 && NF==0)) {
    X1=Vid[0].Echelle*(Omni[Num].Point[_X]+Vid[0].Depla.X)+Vid[0].WXs2;
    Y1=Vid[0].Echelle*(Omni[Num].Point[_Y]+Vid[0].Depla.Y)+Vid[0].WYs2;
    select_vue(0,CLIP_ON);
    g_ligne(X1-LO,Y1-LO,X1+LO,Y1+LO,C);
    g_ligne(X1+LO,Y1-LO,X1-LO,Y1+LO,C);
    g_ligne(X1   ,Y1-LO,X1   ,Y1+LO,C);
    g_ligne(X1-LO,Y1   ,X1+LO,Y1   ,C);
    text_xy(X1+10,Y1-10,Nb,C);
  }

  if (Fx4 || (Fx4==0 && NF==1)) {
    X1=Vid[1].Echelle*(Omni[Num].Point[_Z]+Vid[1].Depla.X)+Vid[1].WXs2;
    Y1=Vid[1].Echelle*(Omni[Num].Point[_Y]+Vid[1].Depla.Y)+Vid[1].WYs2;
    select_vue(1,CLIP_ON);
    g_ligne(X1-LO,Y1-LO,X1+LO,Y1+LO,C);
    g_ligne(X1+LO,Y1-LO,X1-LO,Y1+LO,C);
    g_ligne(X1   ,Y1-LO,X1   ,Y1+LO,C);
    g_ligne(X1-LO,Y1   ,X1+LO,Y1   ,C);
    text_xy(X1+10,Y1-10,Nb,C);
  }

  if (Fx4 || (Fx4==0 && NF==2)) {
    X1=Vid[2].Echelle*(Omni[Num].Point[_X]+Vid[2].Depla.X)+Vid[2].WXs2;
    Y1=Vid[2].Echelle*(Omni[Num].Point[_Z]+Vid[2].Depla.Y)+Vid[2].WYs2;
    select_vue(2,CLIP_ON);
    g_ligne(X1-LO,Y1-LO,X1+LO,Y1+LO,C);
    g_ligne(X1+LO,Y1-LO,X1-LO,Y1+LO,C);
    g_ligne(X1   ,Y1-LO,X1   ,Y1+LO,C);
    g_ligne(X1-LO,Y1   ,X1+LO,Y1   ,C);
    text_xy(X1+10,Y1-10,Nb,C);
  }

  if (Fx4 || (Fx4==0 && NF==3)) affiche_omni_3d(Num,C);

  type_ecriture(COPY_PUT);
}

// -------------------------------------------------------------------------
// -- SELECTIONNE UNE OMNI -------------------------------------------------
// -------------------------------------------------------------------------
CibleOeil selection_omni(void) {
  register DBL X,Y;
  register int i;
  CibleOeil Valeur;

  for (i=1;i<=NbOmni;i++) {
    switch(NF) {
      case 0:
        X=Vid[0].WXs2+(Omni[i].Point[_X]+Vid[0].Depla.X)*Vid[0].Echelle;
        Y=Vid[0].WYs2+(Omni[i].Point[_Y]+Vid[0].Depla.Y)*Vid[0].Echelle;
        break;
      case 1:
        X=Vid[1].WXs2+(Omni[i].Point[_Z]+Vid[1].Depla.X)*Vid[1].Echelle;
        Y=Vid[1].WYs2+(Omni[i].Point[_Y]+Vid[1].Depla.Y)*Vid[1].Echelle;
        break;
      case 2:
        X=Vid[2].WXs2+(Omni[i].Point[_X]+Vid[2].Depla.X)*Vid[2].Echelle;
        Y=Vid[2].WYs2+(Omni[i].Point[_Z]+Vid[2].Depla.Y)*Vid[2].Echelle;
        break;
    }
    if (VRAI==test_ligne(X-5,Y-5,X+5,Y+5,gmx_v(),gmy_v()) ||
        VRAI==test_ligne(X+5,Y-5,X-5,Y+5,gmx_v(),gmy_v())) {
      Valeur.Type=OMNI;
      Valeur.Num=i;
      return Valeur;
    }
  }

  Valeur.Type=0;
  Valeur.Num=0;
  return Valeur;
}

// -------------------------------------------------------------------------
// -- DEPLACE UNE OMNI POINT -----------------------------------------------
// -------------------------------------------------------------------------
void deplace_omni(byte NumOmni) {
  register DBL X,Y,XA,YA,XB,YB;
  register DBL DX,DY,E;
  int MX,MY;

  E=Vid[NF].Echelle;

  if (NF==0) { X=DX=Omni[NumOmni].Point[_X]; Y=DY=Omni[NumOmni].Point[_Y]; }
  if (NF==1) { X=DX=Omni[NumOmni].Point[_Z]; Y=DY=Omni[NumOmni].Point[_Y]; }
  if (NF==2) { X=DX=Omni[NumOmni].Point[_X]; Y=DY=Omni[NumOmni].Point[_Z]; }
  
  forme_mouse(Sens);
  MX=gmx_r();
  MY=gmy_r();
  while (MouseB());
  
  GMouseOff();
  affiche_omni(NumOmni,NF,XOR_PUT);
  
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
      affiche_omni(NumOmni,NF,XOR_PUT);
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
        affiche_omni(NumOmni,NF,XOR_PUT);
        X=DX;
        Y=DY;
        Sens_Souris();
        affiche_omni(NumOmni,NF,XOR_PUT);
      }
      if (NF==0) { Omni[NumOmni].Point[_X]=X; Omni[NumOmni].Point[_Y]=Y; }
      if (NF==1) { Omni[NumOmni].Point[_Z]=X; Omni[NumOmni].Point[_Y]=Y; }
      if (NF==2) { Omni[NumOmni].Point[_X]=X; Omni[NumOmni].Point[_Z]=Y; }
      affiche_omni(NumOmni,NF,XOR_PUT);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("X=%+.2lf Y=%+.2lf",X,-Y);

      select_vue(NF,CLIP_ON);
    }
  }

  affiche_omni(NumOmni,NF,XOR_PUT);

  if (NF==0) { Omni[NumOmni].Point[_X]=DX; Omni[NumOmni].Point[_Y]=DY; }
  if (NF==1) { Omni[NumOmni].Point[_Z]=DX; Omni[NumOmni].Point[_Y]=DY; }
  if (NF==2) { Omni[NumOmni].Point[_X]=DX; Omni[NumOmni].Point[_Z]=DY; }

  cache_omni(1,NumOmni,NumOmni);

  if (X!=DX || Y!=DY) {
    MX+=((X-DX)*Vid[NF].Echelle);
    MY+=((Y-DY)*Vid[NF].Echelle);
    while (MouseB());
    if (NF==0) { Omni[NumOmni].Point[_X]=X; Omni[NumOmni].Point[_Y]=Y; }
    if (NF==1) { Omni[NumOmni].Point[_Z]=X; Omni[NumOmni].Point[_Y]=Y; }
    if (NF==2) { Omni[NumOmni].Point[_X]=X; Omni[NumOmni].Point[_Z]=Y; }
  }

  cache_omni(0,NumOmni,NumOmni);
  while (MouseB());
  place_mouse(MX,MY);
}

// -------------------------------------------------------------------------
// -- SUPPRIME UNE SOURCE DE LUMIERE ---------------------------------------
// -------------------------------------------------------------------------
void supprime_omni(byte NumOmni) {
  register byte i;

  forme_mouse(MS_FLECHE);
  strcpy(StrBoite[0],"Delete an omnilight");
  strcpy(StrBoite[1],"Do you really want");
  sprintf(StrBoite[2],"to delete the omnilight nø%d ?",NumOmni);

  if (g_boite_ONA(CentX,CentY,2,CENTRE,1)==0) {
    efface_liens_looks_like_omni(NumOmni);
    cache_omni(1,1,NbOmni);
    for (i=NumOmni;i<=NbOmni;i++) {
      Omni[i]=Omni[i+1];
    }
    NbOmni--;
    cache_omni(0,1,NbOmni);
    affiche_donnees();
  }
}

/* ------------------------------------------------------------------------- */
/* -- TEST MIN ET MAX DE LA LUMIERE ---------------------------------------- */
/* ------------------------------------------------------------------------- */
void max_min_omni (byte Vue) {
  register byte i;

  if (Omni[0].Cache) return;

  if (Vue==0) {
    for (i=1;i<=NbOmni;i++) {
      Vid[0].Max.X=(Omni[i].Point[_X]>Vid[0].Max.X ? Omni[i].Point[_X]:Vid[0].Max.X);
      Vid[0].Min.X=(Omni[i].Point[_X]<Vid[0].Min.X ? Omni[i].Point[_X]:Vid[0].Min.X);
      Vid[0].Max.Y=(Omni[i].Point[_Y]>Vid[0].Max.Y ? Omni[i].Point[_Y]:Vid[0].Max.Y);
      Vid[0].Min.Y=(Omni[i].Point[_Y]<Vid[0].Min.Y ? Omni[i].Point[_Y]:Vid[0].Min.Y);
    }
    return;
  }

  if (Vue==1) {
    for (i=1;i<=NbOmni;i++) {
      Vid[1].Max.X=(Omni[i].Point[_Z]>Vid[1].Max.X ? Omni[i].Point[_Z]:Vid[1].Max.X);
      Vid[1].Min.X=(Omni[i].Point[_Z]<Vid[1].Min.X ? Omni[i].Point[_Z]:Vid[1].Min.X);
      Vid[1].Max.Y=(Omni[i].Point[_Y]>Vid[1].Max.Y ? Omni[i].Point[_Y]:Vid[1].Max.Y);
      Vid[1].Min.Y=(Omni[i].Point[_Y]<Vid[1].Min.Y ? Omni[i].Point[_Y]:Vid[1].Min.Y);
    }
    return;
  }

  if (Vue==2) {
    for (i=1;i<=NbOmni;i++) {
      Vid[2].Max.X=(Omni[i].Point[_X]>Vid[2].Max.X ? Omni[i].Point[_X]:Vid[2].Max.X);
      Vid[2].Min.X=(Omni[i].Point[_X]<Vid[2].Min.X ? Omni[i].Point[_X]:Vid[2].Min.X);
      Vid[2].Max.Y=(Omni[i].Point[_Z]>Vid[2].Max.Y ? Omni[i].Point[_Z]:Vid[2].Max.Y);
      Vid[2].Min.Y=(Omni[i].Point[_Z]<Vid[2].Min.Y ? Omni[i].Point[_Z]:Vid[2].Min.Y);
    }
    return;
  }
}

/* ------------------------------------------------------------------------- */
/* -- AUGMENTE LE NOMBRE D'OMNIS ------------------------------------------- */
/* ------------------------------------------------------------------------- */
byte incr_NbOmni(byte Val) {
  if (NbOmni<NB_OMNI_MAX) { NbOmni++; return 1; }
  if (Val) {
    forme_mouse(MS_FLECHE);
    strcpy(StrBoite[0],"SHAREWARE VERSION");
    strcpy(StrBoite[1],"The number of omnilights is limited");
    sprintf(StrBoite[2],"in the shareware version to : %d",NB_OMNI_MAX);
    strcpy(StrBoite[3],"");
    strcpy(StrBoite[4],"PLEASE, REGISTER !");
    g_boite_ONA(CentX,CentY,4,CENTRE,0);
  }
  NbOmni=NB_OMNI_MAX;
  return 0;
}

// ----------------------------------------------------------------------------
// -- TEST LIMITE OMNI (COULEUR,TAILLE) ---------------------------------------
// ----------------------------------------------------------------------------
void test_limite_omni(byte N) {
  /*
  Omni[N].RVB[_R]=(Omni[N].RVB[_R]>255 ? 255:Omni[N].RVB[_R]);
  Omni[N].RVB[_V]=(Omni[N].RVB[_V]>255 ? 255:Omni[N].RVB[_V]);
  Omni[N].RVB[_B]=(Omni[N].RVB[_B]>255 ? 255:Omni[N].RVB[_B]);
  */

  Omni[N].Taille=(Omni[N].Taille>MAX_TAILLE_LUMIERE ? 3:Omni[N].Taille);
  Omni[N].Taille=(Omni[N].Taille<MIN_TAILLE_LUMIERE ? 3:Omni[N].Taille);
}

// ----------------------------------------------------------------------------
// -- TEST SI OMNI EST A 0,0,0 en RVB -----------------------------------------
// ----------------------------------------------------------------------------
byte test_couleur_omni(byte N1,byte N2) {
  int i;

  if (NbOmni==0) return 1;

  for (i=N1;i<=N2;i++) {
    if (Omni[i].RVB[_R]==0 && Omni[i].RVB[_V]==0 && Omni[i].RVB[_B]==0) {
      forme_mouse(MS_FLECHE);
      strcpy(StrBoite[0],"SWITCHED OFF OMNILIGHT");
      strcpy(StrBoite[1],"Do you really want the");
      sprintf(StrBoite[2],"omnilight nø%d to diffuse",i);
      strcpy(StrBoite[3],"nothing (blacklight - RVB=0,0,0) ?");
      if (g_boite_ONA(CentX,CentY,3,CENTRE,1)==0) return 1; else return 0;
    }
  }

  return 1;
}

// -------------------------------------------------------------------------
// -- AFFICHE OU CACHE LES LUMIERES ----------------------------------------
// -------------------------------------------------------------------------
void cache_omni(byte Mode,byte D,byte F) {
  register int i;

  Omni[0].Cache=0;

  if (!Mode) {
    for (i=D;i<=F;i++) {
      affiche_omni(i,0,COPY_PUT);
      affiche_omni(i,1,COPY_PUT);
      affiche_omni(i,2,COPY_PUT);
    }
  }

  if (Mode) {
    for (i=D;i<=F;i++) {
      affiche_omni(i,0,HIDE_PUT);
      affiche_omni(i,1,HIDE_PUT);
      affiche_omni(i,2,HIDE_PUT);
    }
    Omni[0].Cache=1;
  }
}

// -------------------------------------------------------------------------
// -- MODIFIE LA COULEUR DE LA LUMIERE -------------------------------------
// -------------------------------------------------------------------------
void couleur_omni(byte N) {
  creation_couleur(&Omni[N].RVB[_R],&Omni[N].RVB[_V],&Omni[N].RVB[_B],"OMNILIGHT'S COLOR");
}

// ----------------------------------------------------------------------------
// -- MODIFIE LA TAILLE DE LA LUMIERE (DANS MODELEUR) -------------------------
// ----------------------------------------------------------------------------
void taille_omni(byte NumOmni) {
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

  T=Omni[NumOmni].Taille;
  TF=MAX_TAILLE_LUMIERE+10;
  g_fenetre(X1,Y1,X2,Y2,"OMNILIGHT SIZE",AFFICHE);
  relief(X-TF,Y-TF-9,X+TF,Y+TF-9,0);

  init_potar(0,X1+30,Y2-46,120,MIN_TAILLE_LUMIERE,MAX_TAILLE_LUMIERE,T,1,"omnilight size");
  init_bouton(40,X1+(X2-X1)/2-25,Y2-30,50,20,"Ok",CENTRE,ATTEND,"Validate the size");

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
      g_ligne(X-j,Y-j,X+j,Y+j,type_couleur(OMNI));
      g_ligne(X+j,Y-j,X-j,Y+j,type_couleur(OMNI));
      g_ligne(X  ,Y-j,X  ,Y+j,type_couleur(OMNI));
      g_ligne(X-j,Y  ,X+j,Y  ,type_couleur(OMNI));

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
    affiche_omni(NumOmni,j,HIDE_PUT);
  }

  Omni[NumOmni].Taille=(byte)(i==1 ? Potar[0].Val:T);

  for (j=0;j<=2;j++) {
    affiche_omni(NumOmni,j,COPY_PUT);
  }
}

// -------------------------------------------------------------------------
// -- MODIF SOURCE OMNI ON/OFF ---------------------------------------------
// -------------------------------------------------------------------------
void on_off_omni(byte N) {
  cache_omni(1,N,N);
  Omni[N].OnOff=!Omni[N].OnOff;
  cache_omni(0,N,N);
  message("Omnilight source nø%d switched %s",N,Omni[N].OnOff ? "on":"off");
  while (MouseB());
  delay(1000);
}

// -------------------------------------------------------------------------
// -- MODIF SOURCE OMNI OMBRES ---------------------------------------------
// -------------------------------------------------------------------------
void ombre_omni(byte N) {
  Omni[N].Ombre=!Omni[N].Ombre;
  message("Omnilight source nø%d %s shadows",N,Omni[N].Ombre ? "cast":"don't cast");
  while (MouseB());
  delay(1000);
}

// -------------------------------------------------------------------------
// -- LOOKS_LIKE EFFECT ----------------------------------------------------
// -------------------------------------------------------------------------
void looks_like_omni(void) {
  CibleOeil Valeur;
  int N,i;

  if (pas_lumiere(1)) return;
  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_LOOKS_LIKE_OMNI:

  message("Pick a omnilight source for looks_like");
  forme_mouse(MS_SELECTEUR);

  if (cherche_fenetre()==FAUX) return;

  Valeur=selection_omni();
  N=Valeur.Num;

  while (MouseB());
  if (Valeur.Num==0) goto LABEL_LOOKS_LIKE_OMNI;

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
        Objet[i]->LooksLike.Light=(Objet[i]->LooksLike.Light==OMNI ? 0:OMNI);
      }
    }
  } else {
    Objet[NumObjet]->LooksLike.Nb=N;
    Objet[NumObjet]->LooksLike.Light=(Objet[NumObjet]->LooksLike.Light==OMNI ? 0:OMNI);
  }

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- TESTE SI CETTE OMNI EST LOOKS_LIKE -----------------------------------
// -------------------------------------------------------------------------
byte si_omni_looks_like(int N) {
  register int i;

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->LooksLike.Nb==N && Objet[i]->LooksLike.Light==OMNI) {
      return 1;
    }
  }

  return 0;
}

// -------------------------------------------------------------------------
// -- ATTRIBUTS ET SETUP D'UNE LUMIERE OMNI --------------------------------
// -------------------------------------------------------------------------
void setup_omni(int Num) {
  int X1=CentX-90;
  int X2=CentX+90;
  int Y1=CentY+20;
  int Y2=CentY+20;
  int i;
  byte Ombre;
  DBL F_Dist;
  byte F_Power;
  byte Fade;
  byte Atmos;
  byte Atmos_Att;
  byte OnOff;
  int XB,YB;
  int XC,YC;
  int XA,YA;

  forme_mouse(MS_FLECHE);

  {
     char Buffer[256];
     sprintf(Buffer,"%s #%d setup","Omnilight",Num);
     Y1-=180;
     Y2+=175;
     g_fenetre(X1,Y1,X2,Y2,Buffer,AFFICHE);
  }

  XA=X1+10;
  YA=Y1+29;
  XB=X1+10;
  YB=Y1+220;
  XC=X1+10;
  YC=Y1+304;

  Ombre=Omni[Num].Ombre;
  F_Dist=Omni[Num].F_Dist;
  F_Power=Omni[Num].F_Power;
  Fade=Omni[Num].Fade;
  Atmos_Att=Omni[Num].Atmos_Att;
  Atmos=Omni[Num].Atmos;
  OnOff=Omni[Num].OnOff;
  sprintf(ZTexte[1].Variable,"%.4g",Omni[Num].Point[_X]);
  sprintf(ZTexte[2].Variable,"%.4g",-Omni[Num].Point[_Y]);
  sprintf(ZTexte[3].Variable,"%.4g",-Omni[Num].Point[_Z]);

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

  border(XB,YB,XB+158,YB+75,0,1);
  init_texte(1,XB+73,YB+11,"Location X",ZTexte[1].Variable,7,"Set X location");
  init_texte(2,XB+73,YB+31,"Location Y",ZTexte[2].Variable,7,"Set Y location");
  init_texte(3,XB+73,YB+51,"Location Z",ZTexte[3].Variable,7,"Set Z location");

  text_xy(X1+10,Y1+305,"Lens Flare",TEXTE);
  strupr(Omni[Num].Flare);
  init_bouton(59,X1+65,Y1+302,100,20,strinstr(0,Omni[Num].Flare,".FLR")<0 ? "[None]":Omni[Num].Flare,GAUCHE,ATTEND,"Select a lens flare type");

  affiche_bouton(59);
  affiche_case(11);
  affiche_case(12);
  affiche_case(13);
  affiche_case(14);
  affiche_case(15);
  for (i=0;i<=3;i++) place_zone_texte(i);
  affiche_pastille(11);
  affiche_pastille(12);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_texte(0,3);
    test_case(11,15);
    test_groupe_pastille(11,12);
    if (test_bouton(59,59)==59) {
      flare_light(Omni[Num].Flare,59);
      bouton_dialog(X1,X2,Y2,1,1);
    }
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    cache_omni(1,Num,Num);
    Omni[Num].Ombre=Cc[14].Croix;
    Omni[Num].F_Dist=atof(ZTexte[0].Variable);
    Omni[Num].F_Power=quelle_pastille_dans_groupe(11,12)-10;
    Omni[Num].Fade=Cc[11].Croix;
    Omni[Num].Atmos_Att=Cc[13].Croix;
    Omni[Num].Atmos=Cc[12].Croix;
    Omni[Num].OnOff=Cc[15].Croix;
    Omni[Num].Point[_X]=atof(ZTexte[1].Variable);
    Omni[Num].Point[_Y]=-atof(ZTexte[2].Variable);
    Omni[Num].Point[_Z]=-atof(ZTexte[3].Variable);
    cache_omni(0,Num,Num);
  }

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- AFFICHE UNE LUMIERE OMNI DANS LA FENETRE 3D --------------------------
// -------------------------------------------------------------------------
void affiche_omni_3d(int N,byte C) {

  if (Vid[3].Enable==0) return;
  if (Fx4==0) return;
  if (Omni[0].Cache) return;

  Objet[0]->Type=OMNI;
  vect_init(Objet[0]->S,Omni[N].Taille*0.03,
                        Omni[N].Taille*0.03,
                        Omni[N].Taille*0.03);
  vect_init(Objet[0]->T,Omni[N].Point[_X],Omni[N].Point[_Y],Omni[N].Point[_Z]);
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
