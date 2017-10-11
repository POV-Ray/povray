/* ---------------------------------------------------------------------------
*  SPOT.C
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
#include <STDLIB.H>
#include <STRING.H>
#include <STDIO.H>
#include <DOS.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"

struct Lumiere_Spot Spot[NB_SPOT_MAX+1];
byte NbSpot=0;

// -------------------------------------------------------------------------
// -- INTIALISE LES COORDONNEES DU SPOT ------------------------------------
// -------------------------------------------------------------------------
void xyz_spot(byte N,DBL X1,DBL Y1,DBL Z1,DBL X2,DBL Y2,DBL Z2) {
  Spot[N].OX=X1;
  Spot[N].OY=Y1;
  Spot[N].OZ=Z1;
  Spot[N].CX=X2;
  Spot[N].CY=Y2;
  Spot[N].CZ=Z2;
  Spot[N].RVB[_R]=255;
  Spot[N].RVB[_V]=255;
  Spot[N].RVB[_B]=255;
  Spot[N].Taille=3;
  Spot[N].Angle1=30;
  Spot[N].Angle2=25;
  Spot[N].OnOff=1;
  Spot[N].Ombre=1;
  Spot[N].F_Dist=100.0;
  Spot[N].F_Power=1;
  Spot[N].Fade=0;
  Spot[N].Atmos=0;
  Spot[N].Atmos_Att=0;
  Spot[N].Cone=0;
  Spot[N].Area=0;
  vect_init(Spot[N].Axis1,-0.5,-0.5,-0.5);
  vect_init(Spot[N].Axis2, 0.5, 0.5, 0.5);
  Spot[N].Size1=3;
  Spot[N].Size2=3;
  Spot[N].Jitter=1;
  Spot[N].Adaptive=1;
  strcpy(Spot[N].Flare,"None");
}

// -------------------------------------------------------------------------
// -- AFFICHE LE CONE DU SPOT ----------------------------------------------
// -------------------------------------------------------------------------
void affiche_cone_spot(int N,byte Vue,byte C) {
  DBL CX,CY,CZ,DI,Long,S;
  DBL Spot_Longitude,Spot_Latitude;
  int i;

  if (Spot[0].Cache) return;

  CX=Spot[N].CX-Spot[N].OX;
  CY=Spot[N].CY-Spot[N].OY;
  CZ=-(Spot[N].CZ-Spot[N].OZ);

  DI=sqrt(CZ*CZ+CX*CX);
  if (DI==0.0 && CY>0.0) { Spot_Longitude=0; Spot_Latitude=+PI/2; goto CONE_OK; }
  if (DI==0.0 && CY<0.0) { Spot_Longitude=0; Spot_Latitude=-PI/2; goto CONE_OK; }
  Spot_Latitude=atan(CY/DI);
  if (CZ==0.0 && CX>0.0) { Spot_Longitude=+M_PI_2; goto CONE_OK; }
  if (CZ==0.0 && CX<0.0) { Spot_Longitude=-M_PI_2; goto CONE_OK; }
  Spot_Longitude=atan(CX/CZ);
  if (CZ<0.0 && CX>0.0) Spot_Longitude+=PI;
  if (CZ<0.0 && CX<0.0) Spot_Longitude+=PI;
  if (CZ<0.0 && CX==0) Spot_Longitude=PI;
  if (CZ==0.0 && CX<0.0) Spot_Longitude=-PI/2;

  CONE_OK:

  Long=sqrt((Spot[N].CX-Spot[N].OX)*(Spot[N].CX-Spot[N].OX)+
            (Spot[N].CY-Spot[N].OY)*(Spot[N].CY-Spot[N].OY)+
            (Spot[N].CZ-Spot[N].OZ)*(Spot[N].CZ-Spot[N].OZ));

  S=Long*tan(Spot[N].Angle2*PIs180);

  Objet[0]->Type=CERCLEY24;
  modif_objet(0,Spot[N].CX,Spot[N].CY,Spot[N].CZ,TRANSLATE);
  modif_objet(0,S,S,S,SCALE);
  modif_objet(0,((-Spot_Latitude/PIs180)+90),(-Spot_Longitude/PIs180),0.0,ROTATE);
  Objet[0]->Couleur=C;
  Objet[0]->Selection=0;
  Objet[0]->Rapide=0;
  Objet[0]->Buffer=N;
  Objet[0]->Cache=0;
  Objet[0]->Operator=0;
  Objet[0]->Ignore=0;
  Objet[0]->Freeze=0;
  Objet[0]->Operator=PAS_CSG;
  Objet[0]->CSG=PAS_CSG;
  for (i=0;i<=1;i++) {
    if (Vue==0) trace_volume_0(0);
    if (Vue==1) trace_volume_1(0);
    if (Vue==2) trace_volume_2(0);
    S=(Long*tan(Spot[N].Angle1*PIs180));
    modif_objet(0,S,S,S,SCALE);
  }
}

// -------------------------------------------------------------------------
// -- INTIALISE LES COORDONNEES DU SPOT ------------------------------------
// -------------------------------------------------------------------------
void new_spot(void) {
  cache_spot(1,1,NbSpot);

  if (incr_NbSpot(0)) {
    Spot[0].Cache=0;
    xyz_spot(NbSpot,0,-3,0,0,0,0);
    cache_spot(0,1,NbSpot);
    affiche_donnees();
    message("New spotlight nø%d created",NbSpot);
  }
}

// -------------------------------------------------------------------------
// -- TRACE UN SPOT DANS LES FENETRES --------------------------------------
// -------------------------------------------------------------------------
void affiche_spot(int N,byte Vue,byte Mode,byte Cone) {
  DBL XO,YO,XC,YC;
  register byte C,i;
  int LO=Spot[N].Taille;
  char Nb[6];

  if (Vid[Vue].Enable==0) return;
  if (Fx4==0 && Vue!=NF) return;
  if (Spot[0].Cache) return;

  C=(Mode==COPY_PUT ? type_couleur(SPOT):(FFOND | BLANC));
  if (Mode==HIDE_PUT) { C=FFOND; Mode=COPY_PUT; } // efface
  type_ecriture(Mode);
  sprintf(Nb,"[%0d%c]",N,Spot[N].OnOff ? '+':'-');

  if (Fx4 || (Fx4==0 && NF==0)) {
    XO=Vid[0].WXs2+(Spot[N].OX+Vid[0].Depla.X)*Vid[0].Echelle;
    YO=Vid[0].WYs2+(Spot[N].OY+Vid[0].Depla.Y)*Vid[0].Echelle;
    XC=Vid[0].WXs2+(Spot[N].CX+Vid[0].Depla.X)*Vid[0].Echelle;
    YC=Vid[0].WYs2+(Spot[N].CY+Vid[0].Depla.Y)*Vid[0].Echelle;
    select_vue(0,CLIP_ON);
    g_ligne(XO-LO,YO-LO,XO+LO,YO+LO,C);
    g_ligne(XO+LO,YO-LO,XO-LO,YO+LO,C);
    g_ligne(XO   ,YO-LO,XO   ,YO+LO,C);
    g_ligne(XO-LO,YO   ,XO+LO,YO   ,C);
    g_ligne(XO,YO,XC,YC,C);
    g_rectangle(XC-2,YC-2,XC+2,YC+2,C,0);
    text_xy(XO+10,YO-10,Nb,C);
    if (Cone) {
      affiche_cone_spot(N,0,C);
      for (i=0;i<=3;i++) {
        move_to(XO,YO);
        g_ligne_to(Point[i*6].X,Point[i*6].Y,C);
      }
    }
  }

  if (Fx4 || (Fx4==0 && NF==1)) {
    XO=Vid[1].WXs2+(Spot[N].OZ+Vid[1].Depla.X)*Vid[1].Echelle;
    YO=Vid[1].WYs2+(Spot[N].OY+Vid[1].Depla.Y)*Vid[1].Echelle;
    XC=Vid[1].WXs2+(Spot[N].CZ+Vid[1].Depla.X)*Vid[1].Echelle;
    YC=Vid[1].WYs2+(Spot[N].CY+Vid[1].Depla.Y)*Vid[1].Echelle;
    select_vue(1,CLIP_ON);
    g_ligne(XO-LO,YO-LO,XO+LO,YO+LO,C);
    g_ligne(XO+LO,YO-LO,XO-LO,YO+LO,C);
    g_ligne(XO   ,YO-LO,XO   ,YO+LO,C);
    g_ligne(XO-LO,YO   ,XO+LO,YO   ,C);
    g_ligne(XO,YO,XC,YC,C);
    g_rectangle(XC-2,YC-2,XC+2,YC+2,C,0);
    text_xy(XO+10,YO-10,Nb,C);
    if (Cone) {
      affiche_cone_spot(N,1,C);
      for (i=0;i<=3;i++) {
        move_to(XO,YO);
        g_ligne_to(Point[i*6].Z,Point[i*6].Y,C);
      }
    }
  }

  if (Fx4 || (Fx4==0 && NF==2)) {
    XO=Vid[2].WXs2+(Spot[N].OX+Vid[2].Depla.X)*Vid[2].Echelle;
    YO=Vid[2].WYs2+(Spot[N].OZ+Vid[2].Depla.Y)*Vid[2].Echelle;
    XC=Vid[2].WXs2+(Spot[N].CX+Vid[2].Depla.X)*Vid[2].Echelle;
    YC=Vid[2].WYs2+(Spot[N].CZ+Vid[2].Depla.Y)*Vid[2].Echelle;
    select_vue(2,CLIP_ON);
    g_ligne(XO-LO,YO-LO,XO+LO,YO+LO,C);
    g_ligne(XO+LO,YO-LO,XO-LO,YO+LO,C);
    g_ligne(XO   ,YO-LO,XO   ,YO+LO,C);
    g_ligne(XO-LO,YO   ,XO+LO,YO   ,C);
    g_ligne(XO,YO,XC,YC,C);
    g_rectangle(XC-2,YC-2,XC+2,YC+2,C,0);
    text_xy(XO+10,YO-10,Nb,C);
    if (Cone) {
      affiche_cone_spot(N,2,C);
      for (i=0;i<=3;i++) {
        move_to(XO,YO);
        g_ligne_to(Point[i*6].X,Point[i*6].Z,C);
      }
    }
  }

  if (Fx4 || (Fx4==0 && NF==3)) affiche_spot_3d(N,C);

  type_ecriture(COPY_PUT);
}

// -------------------------------------------------------------------------
// -- SELECTIONNE UN SPOT --------------------------------------------------
// -------------------------------------------------------------------------
CibleOeil selection_spot(void) {
  register DBL X,Y;
  register int i;
  CibleOeil Valeur;

  for (i=1;i<=NbSpot;i++) {
    switch(NF) {
      case 0:
        X=Vid[0].WXs2+(Spot[i].OX+Vid[0].Depla.X)*Vid[0].Echelle;
        Y=Vid[0].WYs2+(Spot[i].OY+Vid[0].Depla.Y)*Vid[0].Echelle;
        break;
      case 1:
        X=Vid[1].WXs2+(Spot[i].OZ+Vid[1].Depla.X)*Vid[1].Echelle;
        Y=Vid[1].WYs2+(Spot[i].OY+Vid[1].Depla.Y)*Vid[1].Echelle;
        break;
      case 2:
        X=Vid[2].WXs2+(Spot[i].OX+Vid[2].Depla.X)*Vid[2].Echelle;
        Y=Vid[2].WYs2+(Spot[i].OZ+Vid[2].Depla.Y)*Vid[2].Echelle;
        break;
    }

    if (VRAI==test_ligne(X-5,Y-5,X+5,Y+5,gmx_v(),gmy_v()) ||
        VRAI==test_ligne(X-5,Y+5,X+5,Y-5,gmx_v(),gmy_v())) {
      Valeur.Num=i;
      Valeur.Type=SOEIL;
      return Valeur;
    }

    switch(NF) {
      case 0:
        X=Vid[0].WXs2+(Spot[i].CX+Vid[0].Depla.X)*Vid[0].Echelle;
        Y=Vid[0].WYs2+(Spot[i].CY+Vid[0].Depla.Y)*Vid[0].Echelle;
        break;
      case 1:
        X=Vid[1].WXs2+(Spot[i].CZ+Vid[1].Depla.X)*Vid[1].Echelle;
        Y=Vid[1].WYs2+(Spot[i].CY+Vid[1].Depla.Y)*Vid[1].Echelle;
        break;
      case 2:
        X=Vid[2].WXs2+(Spot[i].CX+Vid[2].Depla.X)*Vid[2].Echelle;
        Y=Vid[2].WYs2+(Spot[i].CZ+Vid[2].Depla.Y)*Vid[2].Echelle;
        break;
    }

    if (VRAI==test_ligne(X-5,Y-5,X+5,Y+5,gmx_v(),gmy_v()) ||
        VRAI==test_ligne(X-5,Y+5,X+5,Y-5,gmx_v(),gmy_v())) {
      Valeur.Num=i;
      Valeur.Type=SCIBLE;
      return Valeur;
    }
  }

  Valeur.Type=0;
  Valeur.Num=0;
  return Valeur;
}



// -------------------------------------------------------------------------
// -- TEST MIN ET MAX DU SPOT ----------------------------------------------
// -------------------------------------------------------------------------
void max_min_spot(byte Vue) {
  register int N;

  if (Spot[0].Cache) return;

  if (Vue==0) {
    for (N=1;N<=NbSpot;N++) {
      // --------------- test Position de la spot
      Vid[0].Max.X=(Spot[N].OX>Vid[0].Max.X ? Spot[N].OX:Vid[0].Max.X);
      Vid[0].Min.X=(Spot[N].OX<Vid[0].Min.X ? Spot[N].OX:Vid[0].Min.X);
      Vid[0].Max.Y=(Spot[N].OY>Vid[0].Max.Y ? Spot[N].OY:Vid[0].Max.Y);
      Vid[0].Min.Y=(Spot[N].OY<Vid[0].Min.Y ? Spot[N].OY:Vid[0].Min.Y);
      // --------------- test Position du look_at
      Vid[0].Max.X=(Spot[N].CX>Vid[0].Max.X ? Spot[N].CX:Vid[0].Max.X);
      Vid[0].Min.X=(Spot[N].CX<Vid[0].Min.X ? Spot[N].CX:Vid[0].Min.X);
      Vid[0].Max.Y=(Spot[N].CY>Vid[0].Max.Y ? Spot[N].CY:Vid[0].Max.Y);
      Vid[0].Min.Y=(Spot[N].CY<Vid[0].Min.Y ? Spot[N].CY:Vid[0].Min.Y);
    }
    return;
  }

  if (Vue==1) {
    for (N=1;N<=NbSpot;N++) {
      // --------------- test Position de la cam‚ra
      Vid[1].Max.X=(Spot[N].OZ>Vid[1].Max.X ? Spot[N].OZ:Vid[1].Max.X);
      Vid[1].Min.X=(Spot[N].OZ<Vid[1].Min.X ? Spot[N].OZ:Vid[1].Min.X);
      Vid[1].Max.Y=(Spot[N].OY>Vid[1].Max.Y ? Spot[N].OY:Vid[1].Max.Y);
      Vid[1].Min.Y=(Spot[N].OY<Vid[1].Min.Y ? Spot[N].OY:Vid[1].Min.Y);
      // --------------- test Position du look_at
      Vid[1].Max.X=(Spot[N].CZ>Vid[1].Max.X ? Spot[N].CZ:Vid[1].Max.X);
      Vid[1].Min.X=(Spot[N].CZ<Vid[1].Min.X ? Spot[N].CZ:Vid[1].Min.X);
      Vid[1].Max.Y=(Spot[N].CY>Vid[1].Max.Y ? Spot[N].CY:Vid[1].Max.Y);
      Vid[1].Min.Y=(Spot[N].CY<Vid[1].Min.Y ? Spot[N].CY:Vid[1].Min.Y);
    }
    return;
  }

  if (Vue==2) {
    for (N=1;N<=NbSpot;N++) {
      // --------------- test Position de la cam‚ra
      Vid[2].Max.X=(Spot[N].OX>Vid[2].Max.X ? Spot[N].OX:Vid[2].Max.X);
      Vid[2].Min.X=(Spot[N].OX<Vid[2].Min.X ? Spot[N].OX:Vid[2].Min.X);
      Vid[2].Max.Y=(Spot[N].OZ>Vid[2].Max.Y ? Spot[N].OZ:Vid[2].Max.Y);
      Vid[2].Min.Y=(Spot[N].OZ<Vid[2].Min.Y ? Spot[N].OZ:Vid[2].Min.Y);
      // --------------- test Position du look_at
      Vid[2].Max.X=(Spot[N].CX>Vid[2].Max.X ? Spot[N].CX:Vid[2].Max.X);
      Vid[2].Min.X=(Spot[N].CX<Vid[2].Min.X ? Spot[N].CX:Vid[2].Min.X);
      Vid[2].Max.Y=(Spot[N].CZ>Vid[2].Max.Y ? Spot[N].CZ:Vid[2].Max.Y);
      Vid[2].Min.Y=(Spot[N].CZ<Vid[2].Min.Y ? Spot[N].CZ:Vid[2].Min.Y);
    }
    return;
  }
}

// -------------------------------------------------------------------------
// -- AUGMENTE LE NOMBRE DE SPOTS ------------------------------------------
// -------------------------------------------------------------------------
byte incr_NbSpot(byte Val) {
  Val=Val;
  if (NbSpot<NB_SPOT_MAX) { NbSpot++; return 1; }
  return 0;
}

// ----------------------------------------------------------------------------
// -- TEST LIMITE SPOT (COULEUR,TAILLE) ---------------------------------------
// ----------------------------------------------------------------------------
void test_limite_spot(byte N) {
  /*
  Spot[N].RVB[_R]=(Spot[N].RVB[_R]>255 ? 255:Spot[N].RVB[_R]);
  Spot[N].RVB[_V]=(Spot[N].RVB[_V]>255 ? 255:Spot[N].RVB[_V]);
  Spot[N].RVB[_B]=(Spot[N].RVB[_B]>255 ? 255:Spot[N].RVB[_B]);
  */

  Spot[N].Taille=(Spot[N].Taille>MAX_TAILLE_LUMIERE ? 3:Spot[N].Taille);
  Spot[N].Taille=(Spot[N].Taille<MIN_TAILLE_LUMIERE ? 3:Spot[N].Taille);
}

// ----------------------------------------------------------------------------
// -- TEST SI SPOT EST A 0,0,0 en RVB -----------------------------------------
// ----------------------------------------------------------------------------
byte test_couleur_spot(byte N1,byte N2) {
  int i;

  if (NbSpot==0) return 1;

  for (i=N1;i<=N2;i++) {
    if (Spot[i].RVB[_R]==0 && Spot[i].RVB[_V]==0 && Spot[i].RVB[_B]==0) {
      forme_mouse(MS_FLECHE);
      strcpy(StrBoite[0],"SWITCH OFF SPOTLIGHT");
      sprintf(StrBoite[1],"Do you really want the spotlight nø",i);
      sprintf(StrBoite[2],"to diffuse a null black light (RGB=0,0,0) ?");
      if (g_boite_ONA(CentX,CentY,2,CENTRE,1)==0) return 1; else return 0;
    }
  }

  return 1;
}

// -------------------------------------------------------------------------
// -- AFFICHE OU CACHE LES LUMIERES ----------------------------------------
// -------------------------------------------------------------------------
void cache_spot(byte Mode,byte D,byte F) {
  register int i;

  Spot[0].Cache=0;

  if (!Mode) {
    for (i=D;i<=F;i++) {
      affiche_spot(i,0,COPY_PUT,Spot[i].Cone);
      affiche_spot(i,1,COPY_PUT,Spot[i].Cone);
      affiche_spot(i,2,COPY_PUT,Spot[i].Cone);
    }
  }

  if (Mode) {
    for (i=D;i<=F;i++) {
      affiche_spot(i,0,HIDE_PUT,Spot[i].Cone);
      affiche_spot(i,1,HIDE_PUT,Spot[i].Cone);
      affiche_spot(i,2,HIDE_PUT,Spot[i].Cone);
    }
    Spot[0].Cache=1;
  }
}

// -------------------------------------------------------------------------
// -- MODIFIE LA COULEUR DE LA LUMIERE -------------------------------------
// -------------------------------------------------------------------------
void couleur_spot(int N) {
  creation_couleur(&Spot[N].RVB[_R],&Spot[N].RVB[_V],&Spot[N].RVB[_B],"SPOT COLOR");
}

// ----------------------------------------------------------------------------
// -- MODIFIE LA TAILLE DE LA LUMIERE (DANS MODELEUR) -------------------------
// ----------------------------------------------------------------------------
void taille_spot(int NumSpot) {
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

  T=Spot[NumSpot].Taille;
  TF=MAX_TAILLE_LUMIERE+10;
  g_fenetre(X1,Y1,X2,Y2,"SPOTLIGHT SIZE",AFFICHE);
  relief(X-TF,Y-TF-9,X+TF,Y+TF-9,0);

  init_potar(0,X1+30,Y2-46,120,MIN_TAILLE_LUMIERE,MAX_TAILLE_LUMIERE,T,1,"Size of spotlight");
  init_bouton(40,X1+(X2-X1)/2-25,Y2-30,50,20,"Ok",CENTRE,ATTEND,"Validate size");

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
      g_ligne(X-j,Y-j,X+j,Y+j,type_couleur(SPOT));
      g_ligne(X+j,Y-j,X-j,Y+j,type_couleur(SPOT));
      g_ligne(X  ,Y-j,X  ,Y+j,type_couleur(SPOT));
      g_ligne(X-j,Y  ,X+j,Y  ,type_couleur(SPOT));

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
    affiche_spot(NumSpot,j,HIDE_PUT,Spot[i].Cone);
  }

  Spot[NumSpot].Taille=(byte)(i==1 ? Potar[0].Val:T);

  for (j=0;j<=2;j++) {
    affiche_spot(NumSpot,j,COPY_PUT,Spot[i].Cone);
  }
}

// -------------------------------------------------------------------------
// -- SUPPRIME UNE SOURCE DE LUMIERE SPOT ----------------------------------
// -------------------------------------------------------------------------
void supprime_spot(int NumSpot) {
  register byte i;

  forme_mouse(MS_FLECHE);
  strcpy(StrBoite[0],"Delete a spotlight");
  strcpy(StrBoite[1],"Do you really want to delete");
  sprintf(StrBoite[2],"the spotlight nø%d ?",NumSpot);

  if (g_boite_ONA(CentX,CentY,2,CENTRE,1)==0) {
    cache_spot(1,1,NumSpot);
    for (i=NumSpot;i<=NbSpot;i++) {
      Spot[i]=Spot[i+1];
    }
    NbSpot--;
    cache_spot(0,1,NbSpot);
    affiche_donnees();
  }
}

// -------------------------------------------------------------------------
// -- DEPLACE UN SPOT (SOLEIL ET/OU SCIBLE) --------------------------------
// -------------------------------------------------------------------------
void deplace_spot(int Type,int Num) {
  DBL X1,Y1,XA,YA,XB,YB;
  DBL DX1,DY1;
  int Sujet;
  int N;
  int MX,MY;

  Sujet=Type;
  N=Num;

  switch (NF) {
    case 0:
      if (Sujet==SOEIL) {
        X1=DX1=Spot[N].OX;
        Y1=DY1=Spot[N].OY;
      } else {
        X1=DX1=Spot[N].CX;
        Y1=DY1=Spot[N].CY;
      }
      break;
    case 1:
      if (Sujet==SOEIL) {
        X1=DX1=Spot[N].OZ;
        Y1=DY1=Spot[N].OY;
      } else {
        X1=DX1=Spot[N].CZ;
        Y1=DY1=Spot[N].CY;
      }
      break;
    case 2:
      if (Sujet==SOEIL) {
        X1=DX1=Spot[N].OX;
        Y1=DY1=Spot[N].OZ;
      } else {
        X1=DX1=Spot[N].CX;
        Y1=DY1=Spot[N].CZ;
      }
      break;
  }

  forme_mouse(Sens);
  MX=gmx_r();
  MY=gmy_r();
  while (MouseB());
  GMouseOff();
  affiche_spot(N,NF,XOR_PUT,Spot[N].Cone);
  
  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();
  
  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) {
      X1=DX1;
      Y1=DY1;
      place_mouse(MX,MY);
      break;
    }
    if (XA!=XB || YA!=YB) {
      delay(5);
      affiche_spot(N,NF,XOR_PUT,Spot[N].Cone);
      switch (Sens) {
        case MS_X:
          X1+=(XA-XB)/Vid[NF].Echelle;
          break;
        case MS_Y:
          Y1+=(YA-YB)/Vid[NF].Echelle;
          break;
        default:
          X1+=(XA-XB)/Vid[NF].Echelle;
          Y1+=(YA-YB)/Vid[NF].Echelle;
          break;
      }
      if ((kbhit()) && getch()==9) {
        affiche_spot(N,NF,XOR_PUT,Spot[N].Cone);
        X1=DX1;
        Y1=DY1;
        Sens_Souris();
        affiche_spot(N,NF,XOR_PUT,Spot[N].Cone);
      }
      if (NF==0) if (Sujet==SOEIL) { Spot[N].OX=X1; Spot[N].OY=Y1; } else { Spot[N].CX=X1; Spot[N].CY=Y1; }
      if (NF==1) if (Sujet==SOEIL) { Spot[N].OZ=X1; Spot[N].OY=Y1; } else { Spot[N].CZ=X1; Spot[N].CY=Y1; }
      if (NF==2) if (Sujet==SOEIL) { Spot[N].OX=X1; Spot[N].OZ=Y1; } else { Spot[N].CX=X1; Spot[N].CZ=Y1; }
      affiche_spot(N,NF,XOR_PUT,Spot[N].Cone);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("X=%+.2lf Y=%+.2lf",X1,-Y1);
    }
  }

  affiche_spot(N,NF,XOR_PUT,Spot[N].Cone);

  if (NF==0) if (Sujet==SOEIL) { Spot[N].OX=DX1; Spot[N].OY=DY1; } else { Spot[N].CX=DX1; Spot[N].CY=DY1; }
  if (NF==1) if (Sujet==SOEIL) { Spot[N].OZ=DX1; Spot[N].OY=DY1; } else { Spot[N].CZ=DX1; Spot[N].CY=DY1; }
  if (NF==2) if (Sujet==SOEIL) { Spot[N].OX=DX1; Spot[N].OZ=DY1; } else { Spot[N].CX=DX1; Spot[N].CZ=DY1; }

  cache_spot(1,N,N);

  if (X1!=DX1 || Y1!=DY1) {
    MX+=((X1-DX1)*Vid[NF].Echelle);
    MY+=((Y1-DY1)*Vid[NF].Echelle);
    while (MouseB());
    if (NF==0) if (Sujet==SOEIL) { Spot[N].OX=X1; Spot[N].OY=Y1; } else { Spot[N].CX=X1; Spot[N].CY=Y1; }
    if (NF==1) if (Sujet==SOEIL) { Spot[N].OZ=X1; Spot[N].OY=Y1; } else { Spot[N].CZ=X1; Spot[N].CY=Y1; }
    if (NF==2) if (Sujet==SOEIL) { Spot[N].OX=X1; Spot[N].OZ=Y1; } else { Spot[N].CX=X1; Spot[N].CZ=Y1; }
  }

  cache_spot(0,N,N);

  while (MouseB());
  place_mouse(MX,MY);
}

// -------------------------------------------------------------------------
// -- MODIFICATION DE L'ANGLE D'ECLAIRAGE ----------------------------------
// -- Max = Angle1                                                        --
// -- Min = Angle2                                                        --
// -------------------------------------------------------------------------
void angle_spot(int N,byte Travail) {
  DBL AngleMax=179;
  DBL Angle1,AngleDebut1;
  DBL Angle2,AngleDebut2;
  DBL XA,YA,XB,YB;

  Angle1=AngleDebut1=Spot[N].Angle1;
  Angle2=AngleDebut2=Spot[N].Angle2;

  while(MouseB());
  GMouseOff();
  affiche_spot(N,NF,XOR_PUT,1);

  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();

  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) { Angle1=AngleDebut1; Angle2=AngleDebut2; break; }
    if (XA!=XB || YA!=YB) {
      delay(5);
      affiche_spot(N,NF,XOR_PUT,1);
      if (Travail==1) {
        Angle1+=(((XA-XB)+(YA-YB))*0.5);
        Angle1=(Angle1>AngleMax ? AngleMax:Angle1);
        Angle1=(Angle1<0.005 ? 0.005:Angle1);
        Spot[N].Angle1=Angle1;
        if (Angle1<Spot[N].Angle2) {
          Spot[N].Angle2=Angle1;
        } else {
          Spot[N].Angle2=AngleDebut2;
        }
      }
      if (Travail==2) {
        Angle2+=(((XA-XB)+(YA-YB))*0.5);
        Angle2=(Angle2>AngleMax ? AngleMax:Angle2);
        Angle2=(Angle2<0.005 ? 0.005:Angle2);
        Spot[N].Angle2=Angle2;
        if (Angle2>Spot[N].Angle1) {
          Spot[N].Angle1=Angle2;
        } else {
          Spot[N].Angle1=AngleDebut1;
        }
      }
      affiche_spot(N,NF,XOR_PUT,1);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("Spotlight angle=%+.2lf",Travail==1 ? fabs(Angle1):fabs(Angle2));
    }
  }

  affiche_spot(N,NF,XOR_PUT,1);

  if (Angle1!=AngleDebut1 || Angle2!=AngleDebut2) {
    redessine_fenetre(5,1);
  }
}

// -------------------------------------------------------------------------
// -- MODIF SOURCE SPOT ON/OFF ---------------------------------------------
// -------------------------------------------------------------------------
void on_off_spot(byte N) {
  cache_spot(1,N,N);
  Spot[N].OnOff=!Spot[N].OnOff;
  cache_spot(0,N,N);
  message("Spotlight source nø%d switched %s",N,Spot[N].OnOff ? "on":"off");
  while (MouseB());
  delay(1000);
}

// -------------------------------------------------------------------------
// -- MODIF SOURCE SPOT OMBRES ---------------------------------------------
// -------------------------------------------------------------------------
void ombre_spot(byte N) {
  Spot[N].Ombre=!Spot[N].Ombre;
  message("Spotlight source nø%d %s shadows",N,Spot[N].Ombre ? "cast":"don't cast");
  while (MouseB());
  delay(1000);
}

// -------------------------------------------------------------------------
// -- ATTRIBUTS ET SETUP D'UNE LUMIERE SPOT --------------------------------
// -------------------------------------------------------------------------
void setup_spot(int Num) {
  int X1=CentX-175;
  int X2=CentX+175;
  int Y1=CentY+20;
  int Y2=CentY+20;
  int i;
  byte Ombre;
  byte Cone;
  DBL F_Dist;
  byte F_Power;
  byte Fade;
  byte Atmos;
  byte Atmos_Att;
  byte OnOff,Jitter,Area;
  int XB,YB;
  int XC,YC;
  int XA,YA,XD,YD,XE,YE;

  if (pas_lumiere(1)) return;

  {
     char Buffer[256];
     sprintf(Buffer,"Spotlight #%d setup",Num);
     Y1-=205;
     Y2+=205;
     g_fenetre(X1,Y1,X2,Y2,Buffer,AFFICHE);
  }

  XA=X1+10;
  YA=Y1+29;
  XB=X1+10;
  YB=Y1+220;
  XC=X1+10;
  YC=Y1+304;

  XD=X1+185;
  YD=Y1+49;
  XE=X1+185;
  YE=Y1+132;

  Ombre=Spot[Num].Ombre;
  F_Dist=Spot[Num].F_Dist;
  F_Power=Spot[Num].F_Power;
  Fade=Spot[Num].Fade;
  Atmos_Att=Spot[Num].Atmos_Att;
  Atmos=Spot[Num].Atmos;
  OnOff=Spot[Num].OnOff;
  Cone=Spot[Num].Cone;
  Jitter=Spot[Num].Jitter;
  Area=Spot[Num].Area;
  sprintf(ZTexte[1].Variable,"%.4g",Spot[Num].OX);
  sprintf(ZTexte[2].Variable,"%.4g",-Spot[Num].OY);
  sprintf(ZTexte[3].Variable,"%.4g",-Spot[Num].OZ);
  sprintf(ZTexte[4].Variable,"%.4g",Spot[Num].CX);
  sprintf(ZTexte[5].Variable,"%.4g",-Spot[Num].CY);
  sprintf(ZTexte[6].Variable,"%.4g",-Spot[Num].CZ);

  sprintf(ZTexte[7].Variable,"%.4g",Spot[Num].Axis1[_X]);
  sprintf(ZTexte[8].Variable,"%.4g",Spot[Num].Axis1[_Y]);
  sprintf(ZTexte[9].Variable,"%.4g",Spot[Num].Axis1[_Z]);
  sprintf(ZTexte[10].Variable,"%.4g",Spot[Num].Axis2[_X]);
  sprintf(ZTexte[11].Variable,"%.4g",Spot[Num].Axis2[_Y]);
  sprintf(ZTexte[12].Variable,"%.4g",Spot[Num].Axis2[_Z]);

  sprintf(ZTexte[13].Variable,"%.4g",Spot[Num].Adaptive);

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

  init_case(16,X1+11,Y1+217,"Show cone",Cone,"Show light cone or not");
  border(XB,YB+20,XB+158,YB+95,0,1);
  init_texte(1,XB+73,YB+31,"Location X",ZTexte[1].Variable,7,"Set X location");
  init_texte(2,XB+73,YB+51,"Location Y",ZTexte[2].Variable,7,"Set Y location");
  init_texte(3,XB+73,YB+71,"Location Z",ZTexte[3].Variable,7,"Set Z location");
  border(XC,YC+20,XC+158,YC+95,0,1);
  init_texte(4,XC+73,YC+31,"Point at X",ZTexte[4].Variable,7,"Set point at on X");
  init_texte(5,XC+73,YC+51,"Point at Y",ZTexte[5].Variable,7,"Set point at on Y");
  init_texte(6,XC+73,YC+71,"Point at Z",ZTexte[6].Variable,7,"Set point at on Z");

  text_xy(XD,Y1+220,"Size1",NOIR);
  init_potar(0,XD+22,Y1+245,30,0,30,(DBL) Spot[Num].Size1,1,RES_AIDE[161]);
  text_xy(XD,Y1+260,"Size2",NOIR);
  init_potar(1,XD+22,Y1+285,30,0,30,(DBL) Spot[Num].Size2,1,RES_AIDE[161]);

  border(XD,YD,XD+151,YD+75,0,1);
  init_texte(7,XD+73,YD+11,"Axis 1 X",ZTexte[7].Variable,7,"Set X lenght 1");
  init_texte(8,XD+73,YD+31,"Axis 1 Y",ZTexte[8].Variable,7,"Set Y lenght 1");
  init_texte(9,XD+73,YD+51,"Axis 1 Z",ZTexte[9].Variable,7,"Set Z lenght 1");

  border(XE,YE,XE+151,YE+75,0,1);
  init_texte(10,XE+73,YE+11,"Axis 2 X",ZTexte[10].Variable,7,"Set X lenght 2");
  init_texte(11,XE+73,YE+31,"Axis 2 Y",ZTexte[11].Variable,7,"Set Y lenght 2");
  init_texte(12,XE+73,YE+51,"Axis 2 Z",ZTexte[12].Variable,7,"Set Z lenght 2");

  init_texte(13,XD+105,Y1+305,"Adaptive",ZTexte[13].Variable,4,"");
  init_case(17,XD,Y1+304,"Jitter",Jitter,"");
  init_case(18,XD,YD-20,"Area",Area,"");

  text_xy(XD,YE+201,"Lens Flare",TEXTE);
  strupr(Spot[Num].Flare);
  init_bouton(59,XD+55,YE+198,96,20,strinstr(0,Spot[Num].Flare,".FLR")<0 ? "[None]":Spot[Num].Flare,GAUCHE,ATTEND,"Select a lens flare type");
  
  affiche_bouton(59);
  affiche_case(11);
  affiche_case(12);
  affiche_case(13);
  affiche_case(14);
  affiche_case(15);
  affiche_case(16);
  affiche_case(17);
  affiche_case(18);

  affiche_potar(0);
  affiche_potar(1);

  for (i=0;i<=13;i++) place_zone_texte(i);

  affiche_pastille(11);
  affiche_pastille(12);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_potar(0,1);
    test_texte(0,13);
    test_case(11,18);
    test_groupe_pastille(11,12);
    if (test_bouton(59,59)==59) {
      flare_light(Spot[Num].Flare,59);
      bouton_dialog(X1,X2,Y2,1,1);
    }
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    cache_spot(1,Num,Num);
    Spot[Num].Ombre=Cc[14].Croix;
    Spot[Num].F_Dist=atof(ZTexte[0].Variable);
    Spot[Num].F_Power=quelle_pastille_dans_groupe(11,12)-10;
    Spot[Num].Fade=Cc[11].Croix;
    Spot[Num].Atmos_Att=Cc[13].Croix;
    Spot[Num].Atmos=Cc[12].Croix;
    Spot[Num].OnOff=Cc[15].Croix;
    Spot[Num].Cone=Cc[16].Croix;
    Spot[Num].OX=atof(ZTexte[1].Variable);
    Spot[Num].OY=-atof(ZTexte[2].Variable);
    Spot[Num].OZ=-atof(ZTexte[3].Variable);
    Spot[Num].CX=atof(ZTexte[4].Variable);
    Spot[Num].CY=-atof(ZTexte[5].Variable);
    Spot[Num].CZ=-atof(ZTexte[6].Variable);
    Spot[Num].Jitter=(byte) Cc[17].Croix;
    Spot[Num].Area=(byte) Cc[18].Croix;
    Spot[Num].Axis1[_X]=atof(ZTexte[7].Variable);
    Spot[Num].Axis1[_Y]=atof(ZTexte[8].Variable);
    Spot[Num].Axis1[_Z]=atof(ZTexte[9].Variable);
    Spot[Num].Axis2[_X]=atof(ZTexte[10].Variable);
    Spot[Num].Axis2[_Y]=atof(ZTexte[11].Variable);
    Spot[Num].Axis2[_Z]=atof(ZTexte[12].Variable);
    Spot[Num].Size1=(int) Potar[0].Val;
    Spot[Num].Size2=(int) Potar[1].Val;
    Spot[Num].Adaptive=atof(ZTexte[13].Variable);
    cache_spot(0,Num,Num);
  }

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- AFFICHE UNE LUMIERE SPOT DANS LA FENETRE 3D --------------------------
// -------------------------------------------------------------------------
void affiche_spot_3d(int N,byte C) {
  if (Vid[3].Enable==0) return;
  if (Fx4==0) return;
  if (Spot[0].Cache) return;

  Objet[0]->Type=OMNI;
  vect_init(Objet[0]->S,Spot[N].Taille*0.03,
                        Spot[N].Taille*0.03,
                        Spot[N].Taille*0.03);
  vect_init(Objet[0]->T,Spot[N].OX,Spot[N].OY,Spot[N].OZ);
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

  Objet[0]->Type=CUBE_R;
  vect_init(Objet[0]->S,0.03,0.03,0.03);
  vect_init(Objet[0]->T,Spot[N].CX,Spot[N].CY,Spot[N].CZ);

  trace_volume_3(0);

  Objet[0]->Type=SPOT;
  vect_init(Objet[0]->T,0,0,0);
  vect_init(Objet[0]->P,Spot[N].OX,Spot[N].OY,Spot[N].OZ);
  vect_init(Objet[0]->S,Spot[N].CX,Spot[N].CY,Spot[N].CZ);

  trace_volume_3(0);
}
