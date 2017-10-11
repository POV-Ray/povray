/* ---------------------------------------------------------------------------
*  CYLLIGHT.C
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

struct Lumiere_CylLight CylLight[NB_CYLLIGHT_MAX+1];
byte NbCylLight=0;

// -------------------------------------------------------------------------
// -- INTIALISE LES COORDONNEES DU SPOT CYLINDRIQUE ------------------------
// -------------------------------------------------------------------------
void xyz_cyllight(byte N,DBL X1,DBL Y1,DBL Z1,DBL X2,DBL Y2,DBL Z2) {
  CylLight[N].OX=X1;
  CylLight[N].OY=Y1;
  CylLight[N].OZ=Z1;
  CylLight[N].CX=X2;
  CylLight[N].CY=Y2;
  CylLight[N].CZ=Z2;
  CylLight[N].RVB[_R]=255;
  CylLight[N].RVB[_V]=255;
  CylLight[N].RVB[_B]=255;
  CylLight[N].Taille=3;
  CylLight[N].Angle1=30;
  CylLight[N].Angle2=25;
  CylLight[N].OnOff=1;
  CylLight[N].Ombre=1;
  CylLight[N].F_Dist=100.0;
  CylLight[N].F_Power=1;
  CylLight[N].Fade=0;
  CylLight[N].Atmos=0;
  CylLight[N].Atmos_Att=0;
  CylLight[N].Cone=0;
  strcpy(CylLight[N].Flare,"None");
}

// -------------------------------------------------------------------------
// -- AFFICHE LE CONE DU SPOT CYLINDRIQUE ----------------------------------
// -------------------------------------------------------------------------
void affiche_cone_cyllight(int N,byte Vue,byte C) {
  DBL CX,CY,CZ,DI,Long,S;
  DBL CylLight_Longitude,CylLight_Latitude;
  int i;

  if (CylLight[0].Cache) return;

  CX=CylLight[N].CX-CylLight[N].OX;
  CY=CylLight[N].CY-CylLight[N].OY;
  CZ=-(CylLight[N].CZ-CylLight[N].OZ);

  DI=sqrt(CZ*CZ+CX*CX);
  if (DI==0.0 && CY>0.0) { CylLight_Longitude=0; CylLight_Latitude=+PI/2; goto CONE_OK; }
  if (DI==0.0 && CY<0.0) { CylLight_Longitude=0; CylLight_Latitude=-PI/2; goto CONE_OK; }
  CylLight_Latitude=atan(CY/DI);
  if (CZ==0.0 && CX>0.0) { CylLight_Longitude=+M_PI_2; goto CONE_OK; }
  if (CZ==0.0 && CX<0.0) { CylLight_Longitude=-M_PI_2; goto CONE_OK; }
  CylLight_Longitude=atan(CX/CZ);
  if (CZ<0.0 && CX>0.0) CylLight_Longitude+=PI;
  if (CZ<0.0 && CX<0.0) CylLight_Longitude+=PI;
  if (CZ<0.0 && CX==0) CylLight_Longitude=PI;
  if (CZ==0.0 && CX<0.0) CylLight_Longitude=-PI/2;

  CONE_OK:

  Long=sqrt((CylLight[N].CX-CylLight[N].OX)*(CylLight[N].CX-CylLight[N].OX)+
            (CylLight[N].CY-CylLight[N].OY)*(CylLight[N].CY-CylLight[N].OY)+
            (CylLight[N].CZ-CylLight[N].OZ)*(CylLight[N].CZ-CylLight[N].OZ));

  S=Long*tan(CylLight[N].Angle2*PIs180);

  Objet[0]->Type=CERCLEY24;
  modif_objet(0,S,S,S,SCALE);
  modif_objet(0,((-CylLight_Latitude/PIs180)+90),-CylLight_Longitude/PIs180,0.0,ROTATE);
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
    if (Vue==0) {
      modif_objet(0,CylLight[N].CX,CylLight[N].CY,CylLight[N].CZ,TRANSLATE);
      trace_volume_0(0);
      modif_objet(0,CylLight[N].OX,CylLight[N].OY,CylLight[N].OZ,TRANSLATE);
      trace_volume_0(0);
    }
    if (Vue==1) {
      modif_objet(0,CylLight[N].CX,CylLight[N].CY,CylLight[N].CZ,TRANSLATE);
      trace_volume_1(0);
      modif_objet(0,CylLight[N].OX,CylLight[N].OY,CylLight[N].OZ,TRANSLATE);
      trace_volume_1(0);
    }
    if (Vue==2) {
      modif_objet(0,CylLight[N].CX,CylLight[N].CY,CylLight[N].CZ,TRANSLATE);
      trace_volume_2(0);
      modif_objet(0,CylLight[N].OX,CylLight[N].OY,CylLight[N].OZ,TRANSLATE);
      trace_volume_2(0);
    }
    S=(Long*tan(CylLight[N].Angle1*PIs180));
    modif_objet(0,S,S,S,SCALE);
  }
}

// -------------------------------------------------------------------------
// -- INTIALISE LES COORDONNEES DU SPOT CYLINDRIQUE ------------------------
// -------------------------------------------------------------------------
void new_cyllight(void) {
  cache_cyllight(1,1,NbCylLight);

  if (incr_NbCylLight(0)) {
    CylLight[0].Cache=0;
    xyz_cyllight(NbCylLight,0,-3,0,0,0,0);
    cache_cyllight(0,1,NbCylLight);
    affiche_donnees();
    message("New cylindrical light nø%d created",NbCylLight);
  }
}

// -------------------------------------------------------------------------
// -- TRACE UN SPOT CYLINDRIQUE DANS LES FENETRES --------------------------
// -------------------------------------------------------------------------
void affiche_cyllight(int N,byte Vue,byte Mode,byte Cone) {
  DBL XO,YO,XC,YC;
  register byte C;
  int LO=CylLight[N].Taille;
  char Nb[6];

  if (Vid[Vue].Enable==0) return;
  if (Fx4==0 && Vue!=NF) return;
  if (CylLight[0].Cache) return;

  C=(Mode==COPY_PUT ? type_couleur(CYLLIGHT):(FFOND | BLANC));
  if (Mode==HIDE_PUT) { C=FFOND; Mode=COPY_PUT; } // efface
  type_ecriture(Mode);
  sprintf(Nb,"[%0d%c]",N,CylLight[N].OnOff ? '+':'-');

  if (Fx4 || (Fx4==0 && NF==0)) {
    XO=Vid[0].WXs2+(CylLight[N].OX+Vid[0].Depla.X)*Vid[0].Echelle;
    YO=Vid[0].WYs2+(CylLight[N].OY+Vid[0].Depla.Y)*Vid[0].Echelle;
    XC=Vid[0].WXs2+(CylLight[N].CX+Vid[0].Depla.X)*Vid[0].Echelle;
    YC=Vid[0].WYs2+(CylLight[N].CY+Vid[0].Depla.Y)*Vid[0].Echelle;
    select_vue(0,CLIP_ON);
    g_ligne(XO-LO,YO-LO,XO+LO,YO+LO,C);
    g_ligne(XO+LO,YO-LO,XO-LO,YO+LO,C);
    g_ligne(XO   ,YO-LO,XO   ,YO+LO,C);
    g_ligne(XO-LO,YO   ,XO+LO,YO   ,C);
    g_ligne(XO,YO,XC,YC,C);
    g_rectangle(XC-2,YC-2,XC+2,YC+2,C,0);
    text_xy(XO+10,YO-10,Nb,C);
    if (Cone) affiche_cone_cyllight(N,0,C);
  }

  if (Fx4 || (Fx4==0 && NF==1)) {
    XO=Vid[1].WXs2+(CylLight[N].OZ+Vid[1].Depla.X)*Vid[1].Echelle;
    YO=Vid[1].WYs2+(CylLight[N].OY+Vid[1].Depla.Y)*Vid[1].Echelle;
    XC=Vid[1].WXs2+(CylLight[N].CZ+Vid[1].Depla.X)*Vid[1].Echelle;
    YC=Vid[1].WYs2+(CylLight[N].CY+Vid[1].Depla.Y)*Vid[1].Echelle;
    select_vue(1,CLIP_ON);
    g_ligne(XO-LO,YO-LO,XO+LO,YO+LO,C);
    g_ligne(XO+LO,YO-LO,XO-LO,YO+LO,C);
    g_ligne(XO   ,YO-LO,XO   ,YO+LO,C);
    g_ligne(XO-LO,YO   ,XO+LO,YO   ,C);
    g_ligne(XO,YO,XC,YC,C);
    g_rectangle(XC-2,YC-2,XC+2,YC+2,C,0);
    text_xy(XO+10,YO-10,Nb,C);
    if (Cone) affiche_cone_cyllight(N,1,C);
  }

  if (Fx4 || (Fx4==0 && NF==2)) {
    XO=Vid[2].WXs2+(CylLight[N].OX+Vid[2].Depla.X)*Vid[2].Echelle;
    YO=Vid[2].WYs2+(CylLight[N].OZ+Vid[2].Depla.Y)*Vid[2].Echelle;
    XC=Vid[2].WXs2+(CylLight[N].CX+Vid[2].Depla.X)*Vid[2].Echelle;
    YC=Vid[2].WYs2+(CylLight[N].CZ+Vid[2].Depla.Y)*Vid[2].Echelle;
    select_vue(2,CLIP_ON);
    g_ligne(XO-LO,YO-LO,XO+LO,YO+LO,C);
    g_ligne(XO+LO,YO-LO,XO-LO,YO+LO,C);
    g_ligne(XO   ,YO-LO,XO   ,YO+LO,C);
    g_ligne(XO-LO,YO   ,XO+LO,YO   ,C);
    g_ligne(XO,YO,XC,YC,C);
    g_rectangle(XC-2,YC-2,XC+2,YC+2,C,0);
    text_xy(XO+10,YO-10,Nb,C);
    if (Cone) affiche_cone_cyllight(N,2,C);
  }

  if (Fx4 || (Fx4==0 && NF==3)) affiche_cyllight_3d(N,C);

  type_ecriture(COPY_PUT);
}

// -------------------------------------------------------------------------
// -- SELECTIONNE UN SPOT CYLINDRIQUE --------------------------------------
// -------------------------------------------------------------------------
CibleOeil selection_cyllight(void) {
  register DBL X,Y;
  register int i;
  CibleOeil Valeur;

  for (i=1;i<=NbCylLight;i++) {
    switch(NF) {
      case 0:
        X=Vid[0].WXs2+(CylLight[i].OX+Vid[0].Depla.X)*Vid[0].Echelle;
        Y=Vid[0].WYs2+(CylLight[i].OY+Vid[0].Depla.Y)*Vid[0].Echelle;
        break;
      case 1:
        X=Vid[1].WXs2+(CylLight[i].OZ+Vid[1].Depla.X)*Vid[1].Echelle;
        Y=Vid[1].WYs2+(CylLight[i].OY+Vid[1].Depla.Y)*Vid[1].Echelle;
        break;
      case 2:
        X=Vid[2].WXs2+(CylLight[i].OX+Vid[2].Depla.X)*Vid[2].Echelle;
        Y=Vid[2].WYs2+(CylLight[i].OZ+Vid[2].Depla.Y)*Vid[2].Echelle;
        break;
    }

    if (VRAI==test_ligne(X-5,Y-5,X+5,Y+5,gmx_v(),gmy_v()) ||
        VRAI==test_ligne(X-5,Y+5,X+5,Y-5,gmx_v(),gmy_v())) {
      Valeur.Num=i;
      Valeur.Type=COEIL;
      return Valeur;
    }

    switch(NF) {
      case 0:
        X=Vid[0].WXs2+(CylLight[i].CX+Vid[0].Depla.X)*Vid[0].Echelle;
        Y=Vid[0].WYs2+(CylLight[i].CY+Vid[0].Depla.Y)*Vid[0].Echelle;
        break;
      case 1:
        X=Vid[1].WXs2+(CylLight[i].CZ+Vid[1].Depla.X)*Vid[1].Echelle;
        Y=Vid[1].WYs2+(CylLight[i].CY+Vid[1].Depla.Y)*Vid[1].Echelle;
        break;
      case 2:
        X=Vid[2].WXs2+(CylLight[i].CX+Vid[2].Depla.X)*Vid[2].Echelle;
        Y=Vid[2].WYs2+(CylLight[i].CZ+Vid[2].Depla.Y)*Vid[2].Echelle;
        break;
    }

    if (VRAI==test_ligne(X-5,Y-5,X+5,Y+5,gmx_v(),gmy_v()) ||
        VRAI==test_ligne(X-5,Y+5,X+5,Y-5,gmx_v(),gmy_v())) {
      Valeur.Num=i;
      Valeur.Type=CCIBLE;
      return Valeur;
    }
  }

  Valeur.Type=0;
  Valeur.Num=0;
  return Valeur;
}



// -------------------------------------------------------------------------
// -- TEST MIN ET MAX DU SPOT CYLINDRIQUE ----------------------------------
// -------------------------------------------------------------------------
void max_min_cyllight(byte Vue) {
  register int N;

  if (CylLight[0].Cache) return;

  if (Vue==0) {
    for (N=1;N<=NbCylLight;N++) {
      // --------------- test Position de la cyllight
      Vid[0].Max.X=(CylLight[N].OX>Vid[0].Max.X ? CylLight[N].OX:Vid[0].Max.X);
      Vid[0].Min.X=(CylLight[N].OX<Vid[0].Min.X ? CylLight[N].OX:Vid[0].Min.X);
      Vid[0].Max.Y=(CylLight[N].OY>Vid[0].Max.Y ? CylLight[N].OY:Vid[0].Max.Y);
      Vid[0].Min.Y=(CylLight[N].OY<Vid[0].Min.Y ? CylLight[N].OY:Vid[0].Min.Y);
      // --------------- test Position du look_at
      Vid[0].Max.X=(CylLight[N].CX>Vid[0].Max.X ? CylLight[N].CX:Vid[0].Max.X);
      Vid[0].Min.X=(CylLight[N].CX<Vid[0].Min.X ? CylLight[N].CX:Vid[0].Min.X);
      Vid[0].Max.Y=(CylLight[N].CY>Vid[0].Max.Y ? CylLight[N].CY:Vid[0].Max.Y);
      Vid[0].Min.Y=(CylLight[N].CY<Vid[0].Min.Y ? CylLight[N].CY:Vid[0].Min.Y);
    }
    return;
  }

  if (Vue==1) {
    for (N=1;N<=NbCylLight;N++) {
      // --------------- test Position de la cam‚ra
      Vid[1].Max.X=(CylLight[N].OZ>Vid[1].Max.X ? CylLight[N].OZ:Vid[1].Max.X);
      Vid[1].Min.X=(CylLight[N].OZ<Vid[1].Min.X ? CylLight[N].OZ:Vid[1].Min.X);
      Vid[1].Max.Y=(CylLight[N].OY>Vid[1].Max.Y ? CylLight[N].OY:Vid[1].Max.Y);
      Vid[1].Min.Y=(CylLight[N].OY<Vid[1].Min.Y ? CylLight[N].OY:Vid[1].Min.Y);
      // --------------- test Position du look_at
      Vid[1].Max.X=(CylLight[N].CZ>Vid[1].Max.X ? CylLight[N].CZ:Vid[1].Max.X);
      Vid[1].Min.X=(CylLight[N].CZ<Vid[1].Min.X ? CylLight[N].CZ:Vid[1].Min.X);
      Vid[1].Max.Y=(CylLight[N].CY>Vid[1].Max.Y ? CylLight[N].CY:Vid[1].Max.Y);
      Vid[1].Min.Y=(CylLight[N].CY<Vid[1].Min.Y ? CylLight[N].CY:Vid[1].Min.Y);
    }
    return;
  }

  if (Vue==2) {
    for (N=1;N<=NbCylLight;N++) {
      // --------------- test Position de la cam‚ra
      Vid[2].Max.X=(CylLight[N].OX>Vid[2].Max.X ? CylLight[N].OX:Vid[2].Max.X);
      Vid[2].Min.X=(CylLight[N].OX<Vid[2].Min.X ? CylLight[N].OX:Vid[2].Min.X);
      Vid[2].Max.Y=(CylLight[N].OZ>Vid[2].Max.Y ? CylLight[N].OZ:Vid[2].Max.Y);
      Vid[2].Min.Y=(CylLight[N].OZ<Vid[2].Min.Y ? CylLight[N].OZ:Vid[2].Min.Y);
      // --------------- test Position du look_at
      Vid[2].Max.X=(CylLight[N].CX>Vid[2].Max.X ? CylLight[N].CX:Vid[2].Max.X);
      Vid[2].Min.X=(CylLight[N].CX<Vid[2].Min.X ? CylLight[N].CX:Vid[2].Min.X);
      Vid[2].Max.Y=(CylLight[N].CZ>Vid[2].Max.Y ? CylLight[N].CZ:Vid[2].Max.Y);
      Vid[2].Min.Y=(CylLight[N].CZ<Vid[2].Min.Y ? CylLight[N].CZ:Vid[2].Min.Y);
    }
    return;
  }
}

// -------------------------------------------------------------------------
// -- AUGMENTE LE NOMBRE DE SPOTS CYLINDRIQUES -----------------------------
// -------------------------------------------------------------------------
byte incr_NbCylLight(byte Val) {
  Val=Val;
  if (NbCylLight<NB_CYLLIGHT_MAX) { NbCylLight++; return 1; }
  return 0;
}

// ----------------------------------------------------------------------------
// -- TEST LIMITE SPOT CYLINDRIQUE (COULEUR,TAILLE) ---------------------------
// ----------------------------------------------------------------------------
void test_limite_cyllight(byte N) {
  /*
  CylLight[N].RVB[_R]=(CylLight[N].RVB[_R]>255 ? 255:CylLight[N].RVB[_R]);
  CylLight[N].RVB[_V]=(CylLight[N].RVB[_V]>255 ? 255:CylLight[N].RVB[_V]);
  CylLight[N].RVB[_B]=(CylLight[N].RVB[_B]>255 ? 255:CylLight[N].RVB[_B]);
  */

  CylLight[N].Taille=(CylLight[N].Taille>MAX_TAILLE_LUMIERE ? 3:CylLight[N].Taille);
  CylLight[N].Taille=(CylLight[N].Taille<MIN_TAILLE_LUMIERE ? 3:CylLight[N].Taille);
}

// ----------------------------------------------------------------------------
// -- TEST SI SPOT CYLINDRIQUE EST A 0,0,0 en RVB -----------------------------
// ----------------------------------------------------------------------------
byte test_couleur_cyllight(byte N1,byte N2) {
  int i;

  if (NbCylLight==0) return 1;

  for (i=N1;i<=N2;i++) {
    if (CylLight[i].RVB[_R]==0 && CylLight[i].RVB[_V]==0 && CylLight[i].RVB[_B]==0) {
      forme_mouse(MS_FLECHE);
      strcpy(StrBoite[0],"SWITCH OFF CYLINDRICAL LIGHT");
      sprintf(StrBoite[1],"Do you really want the cylindrical light nø",i);
      sprintf(StrBoite[2],"to diffuse a null black light (RGB=0,0,0) ?");
      if (g_boite_ONA(CentX,CentY,2,CENTRE,1)==0) return 1; else return 0;
    }
  }

  return 1;
}

// -------------------------------------------------------------------------
// -- AFFICHE OU CACHE LES LUMIERES ----------------------------------------
// -------------------------------------------------------------------------
void cache_cyllight(byte Mode,byte D,byte F) {
  register int i;

  CylLight[0].Cache=0;

  if (!Mode) {
    for (i=D;i<=F;i++) {
      affiche_cyllight(i,0,COPY_PUT,CylLight[i].Cone);
      affiche_cyllight(i,1,COPY_PUT,CylLight[i].Cone);
      affiche_cyllight(i,2,COPY_PUT,CylLight[i].Cone);
    }
  }

  if (Mode) {
    for (i=D;i<=F;i++) {
      affiche_cyllight(i,0,HIDE_PUT,CylLight[i].Cone);
      affiche_cyllight(i,1,HIDE_PUT,CylLight[i].Cone);
      affiche_cyllight(i,2,HIDE_PUT,CylLight[i].Cone);
    }
    CylLight[0].Cache=1;
  }
}

// -------------------------------------------------------------------------
// -- MODIFIE LA COULEUR DE LA LUMIERE -------------------------------------
// -------------------------------------------------------------------------
void couleur_cyllight(int N) {
  creation_couleur(&CylLight[N].RVB[_R],&CylLight[N].RVB[_V],&CylLight[N].RVB[_B],"LIGHT COLOR");
}

// ----------------------------------------------------------------------------
// -- MODIFIE LA TAILLE DE LA LUMIERE (DANS MODELEUR) -------------------------
// ----------------------------------------------------------------------------
void taille_cyllight(int NumCylLight) {
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

  T=CylLight[NumCylLight].Taille;
  TF=MAX_TAILLE_LUMIERE+10;
  g_fenetre(X1,Y1,X2,Y2,"LIGHT SIZE",AFFICHE);
  relief(X-TF,Y-TF-9,X+TF,Y+TF-9,0);

  init_potar(0,X1+30,Y2-46,120,MIN_TAILLE_LUMIERE,MAX_TAILLE_LUMIERE,T,1,"Size of cylindrical light");
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
      g_ligne(X-j,Y-j,X+j,Y+j,type_couleur(CYLLIGHT));
      g_ligne(X+j,Y-j,X-j,Y+j,type_couleur(CYLLIGHT));
      g_ligne(X  ,Y-j,X  ,Y+j,type_couleur(CYLLIGHT));
      g_ligne(X-j,Y  ,X+j,Y  ,type_couleur(CYLLIGHT));

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
    affiche_cyllight(NumCylLight,j,HIDE_PUT,CylLight[i].Cone);
  }

  CylLight[NumCylLight].Taille=(byte)(i==1 ? Potar[0].Val:T);

  for (j=0;j<=2;j++) {
    affiche_cyllight(NumCylLight,j,COPY_PUT,CylLight[i].Cone);
  }
}

// -------------------------------------------------------------------------
// -- SUPPRIME UNE SOURCE DE LUMIERE SPOT CYLINDRIQUE ----------------------
// -------------------------------------------------------------------------
void supprime_cyllight(int NumCylLight) {
  register byte i;

  forme_mouse(MS_FLECHE);
  strcpy(StrBoite[0],"Delete a cylindrical light");
  strcpy(StrBoite[1],"Do you really want to delete");
  sprintf(StrBoite[2],"the light nø%d ?",NumCylLight);

  if (g_boite_ONA(CentX,CentY,2,CENTRE,1)==0) {
    cache_cyllight(1,1,NumCylLight);
    for (i=NumCylLight;i<=NbCylLight;i++) {
      CylLight[i]=CylLight[i+1];
    }
    NbCylLight--;
    cache_cyllight(0,1,NbCylLight);
    affiche_donnees();
  }
}

// -------------------------------------------------------------------------
// -- DEPLACE UN SPOT CYLINDRIQUE (OEIL ET/OU CIBLE) -----------------------
// -------------------------------------------------------------------------
void deplace_cyllight(int Type,int Num) {
  DBL X1,Y1,XA,YA,XB,YB;
  DBL DX1,DY1;
  int Sujet;
  int N;
  int MX,MY;

  Sujet=Type;
  N=Num;

  switch (NF) {
    case 0:
      if (Sujet==COEIL) {
        X1=DX1=CylLight[N].OX;
        Y1=DY1=CylLight[N].OY;
      } else {
        X1=DX1=CylLight[N].CX;
        Y1=DY1=CylLight[N].CY;
      }
      break;
    case 1:
      if (Sujet==COEIL) {
        X1=DX1=CylLight[N].OZ;
        Y1=DY1=CylLight[N].OY;
      } else {
        X1=DX1=CylLight[N].CZ;
        Y1=DY1=CylLight[N].CY;
      }
      break;
    case 2:
      if (Sujet==COEIL) {
        X1=DX1=CylLight[N].OX;
        Y1=DY1=CylLight[N].OZ;
      } else {
        X1=DX1=CylLight[N].CX;
        Y1=DY1=CylLight[N].CZ;
      }
      break;
  }

  forme_mouse(Sens);
  MX=gmx_r();
  MY=gmy_r();
  while (MouseB());
  GMouseOff();
  affiche_cyllight(N,NF,XOR_PUT,1);
  
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
      affiche_cyllight(N,NF,XOR_PUT,1);
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
        affiche_cyllight(N,NF,XOR_PUT,1);
        X1=DX1;
        Y1=DY1;
        Sens_Souris();
        affiche_cyllight(N,NF,XOR_PUT,1);
      }
      if (NF==0) if (Sujet==COEIL) { CylLight[N].OX=X1; CylLight[N].OY=Y1; } else { CylLight[N].CX=X1; CylLight[N].CY=Y1; }
      if (NF==1) if (Sujet==COEIL) { CylLight[N].OZ=X1; CylLight[N].OY=Y1; } else { CylLight[N].CZ=X1; CylLight[N].CY=Y1; }
      if (NF==2) if (Sujet==COEIL) { CylLight[N].OX=X1; CylLight[N].OZ=Y1; } else { CylLight[N].CX=X1; CylLight[N].CZ=Y1; }
      affiche_cyllight(N,NF,XOR_PUT,1);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("X=%+.2lf Y=%+.2lf",X1,-Y1);
    }
  }

  affiche_cyllight(N,NF,XOR_PUT,1);

  if (NF==0) if (Sujet==COEIL) { CylLight[N].OX=DX1; CylLight[N].OY=DY1; } else { CylLight[N].CX=DX1; CylLight[N].CY=DY1; }
  if (NF==1) if (Sujet==COEIL) { CylLight[N].OZ=DX1; CylLight[N].OY=DY1; } else { CylLight[N].CZ=DX1; CylLight[N].CY=DY1; }
  if (NF==2) if (Sujet==COEIL) { CylLight[N].OX=DX1; CylLight[N].OZ=DY1; } else { CylLight[N].CX=DX1; CylLight[N].CZ=DY1; }

  cache_cyllight(1,N,N);

  if (X1!=DX1 || Y1!=DY1) {
    MX+=((X1-DX1)*Vid[NF].Echelle);
    MY+=((Y1-DY1)*Vid[NF].Echelle);
    while (MouseB());
    if (NF==0) if (Sujet==COEIL) { CylLight[N].OX=X1; CylLight[N].OY=Y1; } else { CylLight[N].CX=X1; CylLight[N].CY=Y1; }
    if (NF==1) if (Sujet==COEIL) { CylLight[N].OZ=X1; CylLight[N].OY=Y1; } else { CylLight[N].CZ=X1; CylLight[N].CY=Y1; }
    if (NF==2) if (Sujet==COEIL) { CylLight[N].OX=X1; CylLight[N].OZ=Y1; } else { CylLight[N].CX=X1; CylLight[N].CZ=Y1; }
  }

  cache_cyllight(0,N,N);

  while (MouseB());
  place_mouse(MX,MY);
}

// -------------------------------------------------------------------------
// -- MODIFICATION DE L'ANGLE D'ECLAIRAGE ----------------------------------
// -- Max = Angle1                                                        --
// -- Min = Angle2                                                        --
// -------------------------------------------------------------------------
void angle_cyllight(int N,byte Travail) {
  DBL AngleMax=1000;
  DBL Angle1,AngleDebut1;
  DBL Angle2,AngleDebut2;
  DBL XA,YA,XB,YB;

  if (pas_objet(1)) return;

  Angle1=AngleDebut1=CylLight[N].Angle1;
  Angle2=AngleDebut2=CylLight[N].Angle2;

  while(MouseB());
  GMouseOff();
  affiche_cyllight(N,NF,XOR_PUT,CylLight[N].Cone);

  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();

  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) { Angle1=AngleDebut1; Angle2=AngleDebut2; break; }
    if (XA!=XB || YA!=YB) {
      delay(5);
      affiche_cyllight(N,NF,XOR_PUT,1);
      if (Travail==1) {
        Angle1+=(((XA-XB)+(YA-YB))*0.5);
        Angle1=(Angle1>AngleMax ? AngleMax:Angle1);
        Angle1=(Angle1<0.005 ? 0.005:Angle1);
        CylLight[N].Angle1=Angle1;
        if (Angle1<CylLight[N].Angle2) {
          CylLight[N].Angle2=Angle1;
        } else {
          CylLight[N].Angle2=AngleDebut2;
        }
      }
      if (Travail==2) {
        Angle2+=(((XA-XB)+(YA-YB))*0.5);
        Angle2=(Angle2>AngleMax ? AngleMax:Angle2);
        Angle2=(Angle2<0.005 ? 0.005:Angle2);
        CylLight[N].Angle2=Angle2;
        if (Angle2>CylLight[N].Angle1) {
          CylLight[N].Angle1=Angle2;
        } else {
          CylLight[N].Angle1=AngleDebut1;
        }
      }
      affiche_cyllight(N,NF,XOR_PUT,1);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("CylLightlight angle=%+.2lf",Travail==1 ? fabs(Angle1):fabs(Angle2));
    }
  }

  affiche_cyllight(N,NF,XOR_PUT,1);

  if (Angle1!=AngleDebut1 || Angle2!=AngleDebut2) {
    redessine_fenetre(5,1);
  }
}

// -------------------------------------------------------------------------
// -- MODIF SOURCE SPOT CYLINDRIQUE ON/OFF ---------------------------------
// -------------------------------------------------------------------------
void on_off_cyllight(byte N) {
  cache_cyllight(1,N,N);
  CylLight[N].OnOff=!CylLight[N].OnOff;
  cache_cyllight(0,N,N);
  message("CylLightlight source nø%d switched %s",N,CylLight[N].OnOff ? "on":"off");
  while (MouseB());
  delay(1000);
}

// -------------------------------------------------------------------------
// -- MODIF SOURCE SPOT CYLINDRIQUE OMBRES ---------------------------------
// -------------------------------------------------------------------------
void ombre_cyllight(byte N) {
  CylLight[N].Ombre=!CylLight[N].Ombre;
  message("CylLightlight source nø%d %s shadows",N,CylLight[N].Ombre ? "cast":"don't cast");
  while (MouseB());
  delay(1000);
}

// -------------------------------------------------------------------------
// -- ATTRIBUTS ET SETUP D'UNE LUMIERE CYLLIGHT ----------------------------
// -------------------------------------------------------------------------
void setup_cyllight(int Num) {
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
  byte OnOff;
  int XB,YB;
  int XC,YC;
  int XA,YA;

  if (pas_lumiere(1)) return;

  {
     char Buffer[256];
     sprintf(Buffer,"Cyllight #%d setup",Num);
     Y1-=130;
     Y2+=130;
     g_fenetre(X1,Y1,X2,Y2,Buffer,AFFICHE);
  }

  XA=X1+10;
  YA=Y1+29;
  XB=X1+180;
  YB=Y1+10;
  XC=X1+180;
  YC=Y1+94;

  Ombre=CylLight[Num].Ombre;
  F_Dist=CylLight[Num].F_Dist;
  F_Power=CylLight[Num].F_Power;
  Fade=CylLight[Num].Fade;
  Atmos_Att=CylLight[Num].Atmos_Att;
  Atmos=CylLight[Num].Atmos;
  OnOff=CylLight[Num].OnOff;
  Cone=CylLight[Num].Cone;
  sprintf(ZTexte[1].Variable,"%.4g",CylLight[Num].OX);
  sprintf(ZTexte[2].Variable,"%.4g",-CylLight[Num].OY);
  sprintf(ZTexte[3].Variable,"%.4g",-CylLight[Num].OZ);
  sprintf(ZTexte[4].Variable,"%.4g",CylLight[Num].CX);
  sprintf(ZTexte[5].Variable,"%.4g",-CylLight[Num].CY);
  sprintf(ZTexte[6].Variable,"%.4g",-CylLight[Num].CZ);

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

  text_xy(XC+3,YC+105,"Lens Flare",TEXTE);
  strupr(CylLight[Num].Flare);
  init_bouton(59,XC+58,YC+102,100,20,strinstr(0,CylLight[Num].Flare,".FLR")<0 ? "[None]":CylLight[Num].Flare,GAUCHE,ATTEND,"Select a lens flare type");
  
  affiche_bouton(59);
  affiche_case(11);
  affiche_case(12);
  affiche_case(13);
  affiche_case(14);
  affiche_case(15);
  for (i=0;i<=6;i++) place_zone_texte(i);
  affiche_case(16);

  affiche_pastille(11);
  affiche_pastille(12);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_texte(0,6);
    test_case(11,15);
    test_case(16,16);
    test_groupe_pastille(11,12);
    if (test_bouton(59,59)==59) {
      flare_light(CylLight[Num].Flare,59);
      bouton_dialog(X1,X2,Y2,1,1);
    }
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    cache_spot(1,Num,Num);
    CylLight[Num].Ombre=Cc[14].Croix;
    CylLight[Num].F_Dist=atof(ZTexte[0].Variable);
    CylLight[Num].F_Power=quelle_pastille_dans_groupe(11,12)-10;
    CylLight[Num].Fade=Cc[11].Croix;
    CylLight[Num].Atmos_Att=Cc[13].Croix;
    CylLight[Num].Atmos=Cc[12].Croix;
    CylLight[Num].OnOff=Cc[15].Croix;
    CylLight[Num].Cone=Cc[16].Croix;
    CylLight[Num].OX=atof(ZTexte[1].Variable);
    CylLight[Num].OY=-atof(ZTexte[2].Variable);
    CylLight[Num].OZ=-atof(ZTexte[3].Variable);
    CylLight[Num].CX=atof(ZTexte[4].Variable);
    CylLight[Num].CY=-atof(ZTexte[5].Variable);
    CylLight[Num].CZ=-atof(ZTexte[6].Variable);
    cache_spot(0,Num,Num);
  }

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- AFFICHE UNE LUMIERE CYLLIGHT DANS LA FENETRE 3D ----------------------
// -------------------------------------------------------------------------
void affiche_cyllight_3d(int N,byte C) {
  if (Vid[3].Enable==0) return;
  if (Fx4==0) return;
  if (CylLight[0].Cache) return;

  Objet[0]->Type=OMNI;
  vect_init(Objet[0]->S,CylLight[N].Taille*0.03,
                        CylLight[N].Taille*0.03,
                        CylLight[N].Taille*0.03);
  vect_init(Objet[0]->T,CylLight[N].OX,CylLight[N].OY,CylLight[N].OZ);
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
  vect_init(Objet[0]->T,CylLight[N].CX,CylLight[N].CY,CylLight[N].CZ);

  trace_volume_3(0);

  Objet[0]->Type=SPOT;
  vect_init(Objet[0]->T,0,0,0);
  vect_init(Objet[0]->P,CylLight[N].OX,CylLight[N].OY,CylLight[N].OZ);
  vect_init(Objet[0]->S,CylLight[N].CX,CylLight[N].CY,CylLight[N].CZ);

  trace_volume_3(0);
}
