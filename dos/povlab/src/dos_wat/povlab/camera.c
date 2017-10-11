/* ---------------------------------------------------------------------------
*  CAMERA.C
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
#include <STDIO.H>
#include <STRING.H>
#include <DOS.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"
#if !WINDOWS
#include <GRAPH.H>
#endif

struct Oeil_Camera Camera[NB_CAMERA_MAX+1];
int NbCamera=0,NumCam=0;
DBL Focale;
DBL Longitude;
DBL Latitude;

// DBL Ratio=1.784997;
DBL Ratio=0.721000;

// -------------------------------------------------------------------------
// -- INITIALISE LES COORDONNEES DE LA CAMERA ------------------------------
// -------------------------------------------------------------------------
void xyz_camera(byte N,DBL X1,DBL Y1,DBL Z1,DBL X2,DBL Y2,DBL Z2) {
  DBL X,Y,Z;

  Camera[N].OX=X1;
  Camera[N].OY=Y1;
  Camera[N].OZ=Z1;
  Camera[N].CX=X2;
  Camera[N].CY=Y2;
  Camera[N].CZ=Z2;
  X=Camera[N].CX-Camera[N].OX;
  Y=Camera[N].CY-Camera[N].OY;
  Z=Camera[N].CZ-Camera[N].OZ;
  Camera[N].ProChamp=sqrt(sqrt(X*X+Y*Y)+Z*Z);
  Camera[N].Ouverture=75;
  Camera[N].F_Blur=0;
  Camera[N].Samples=25;
  Camera[N].Aperture=2;
  Camera[N].Variance=1/128;
  Camera[N].Confidence=0.9;
  Camera[N].Roll=0;
}

// -------------------------------------------------------------------------
// -- INTIALISE LES COORDONNEES DE LA CAMERA -------------------------------
// -------------------------------------------------------------------------
void new_camera(void) {
  cache_camera(1,1,NbCamera);

  if (incr_NbCamera(0)) {
    xyz_camera(NbCamera,EPSILON,EPSILON,5+EPSILON,EPSILON,EPSILON,EPSILON);
    affiche_donnees();
    message("New camera nø%d has been created",NbCamera);
    NumCam=NbCamera;
    cache_camera(0,1,NbCamera);
    redessine_fenetre(3,1);
  }
}

// -------------------------------------------------------------------------
// -- TRACE UNE CAMERA DANS LES FENETRES -----------------------------------
// -------------------------------------------------------------------------
void affiche_camera(int N,byte Vue,byte Mode) {
  register DBL XO,YO,XC,YC;
  register byte C;
  char Nb[6];

  if (Vid[Vue].Enable==0) return;
  if (Fx4==0 && Vue!=NF) return;
  if (Camera[0].Cache) return;

  C=(Mode==COPY_PUT ? type_couleur(CAMERA):(FFOND | JAUNE));
  if (Mode==HIDE_PUT) { C=FFOND; Mode=COPY_PUT; } // efface
  type_ecriture(Mode);
  sprintf(Nb,"[%0d%c]",N,N==NumCam ? '+':'-');

  if (Fx4 || (Fx4==0 && NF==0)) {
    XO=Vid[0].WXs2+(Camera[N].OX+Vid[0].Depla.X)*Vid[0].Echelle;
    YO=Vid[0].WYs2+(Camera[N].OY+Vid[0].Depla.Y)*Vid[0].Echelle;
    XC=Vid[0].WXs2+(Camera[N].CX+Vid[0].Depla.X)*Vid[0].Echelle;
    YC=Vid[0].WYs2+(Camera[N].CY+Vid[0].Depla.Y)*Vid[0].Echelle;
    select_vue(0,CLIP_ON);
    g_ligne(XO,YO,XC,YC,C);
    g_rectangle(XC-2,YC-2,XC+2,YC+2,C,0);
    g_rectangle(XO-1,YO-1,XO+1,YO+1,C,0);
    g_rectangle(XO-4,YO-4,XO+4,YO+4,C,0);
    text_xy(XO+10,YO-10,Nb,C);
  }

  if (Fx4 || (Fx4==0 && NF==1)) {
    XO=Vid[1].WXs2+(Camera[N].OZ+Vid[1].Depla.X)*Vid[1].Echelle;
    YO=Vid[1].WYs2+(Camera[N].OY+Vid[1].Depla.Y)*Vid[1].Echelle;
    XC=Vid[1].WXs2+(Camera[N].CZ+Vid[1].Depla.X)*Vid[1].Echelle;
    YC=Vid[1].WYs2+(Camera[N].CY+Vid[1].Depla.Y)*Vid[1].Echelle;
    select_vue(1,CLIP_ON);
    g_ligne(XO,YO,XC,YC,C);
    g_rectangle(XC-2,YC-2,XC+2,YC+2,C,0);
    g_rectangle(XO-1,YO-1,XO+1,YO+1,C,0);
    g_rectangle(XO-4,YO-4,XO+4,YO+4,C,0);
    text_xy(XO+10,YO-10,Nb,C);
  }

  if (Fx4 || (Fx4==0 && NF==2)) {
    XO=Vid[2].WXs2+(Camera[N].OX+Vid[2].Depla.X)*Vid[2].Echelle;
    YO=Vid[2].WYs2+(Camera[N].OZ+Vid[2].Depla.Y)*Vid[2].Echelle;
    XC=Vid[2].WXs2+(Camera[N].CX+Vid[2].Depla.X)*Vid[2].Echelle;
    YC=Vid[2].WYs2+(Camera[N].CZ+Vid[2].Depla.Y)*Vid[2].Echelle;
    select_vue(2,CLIP_ON);
    g_ligne(XO,YO,XC,YC,C);
    g_rectangle(XC-2,YC-2,XC+2,YC+2,C,0);
    g_rectangle(XO-1,YO-1,XO+1,YO+1,C,0);
    g_rectangle(XO-4,YO-4,XO+4,YO+4,C,0);
    text_xy(XO+10,YO-10,Nb,C);
  }

  type_ecriture(COPY_PUT);
}

// -------------------------------------------------------------------------
// -- SELECTIONNE UNE CAMERA -----------------------------------------------
// -------------------------------------------------------------------------
CibleOeil selection_camera(void) {
  register DBL X,Y;
  register int i;
  CibleOeil Valeur;

  for (i=1;i<=NbCamera;i++) {
    switch(NF) {
      case 0:
        X=Vid[0].WXs2+(Camera[i].OX+Vid[0].Depla.X)*Vid[0].Echelle;
        Y=Vid[0].WYs2+(Camera[i].OY+Vid[0].Depla.Y)*Vid[0].Echelle;
        break;
      case 1:
        X=Vid[1].WXs2+(Camera[i].OZ+Vid[1].Depla.X)*Vid[1].Echelle;
        Y=Vid[1].WYs2+(Camera[i].OY+Vid[1].Depla.Y)*Vid[1].Echelle;
        break;
      case 2:
        X=Vid[2].WXs2+(Camera[i].OX+Vid[2].Depla.X)*Vid[2].Echelle;
        Y=Vid[2].WYs2+(Camera[i].OZ+Vid[2].Depla.Y)*Vid[2].Echelle;
        break;
    }

    if (VRAI==test_ligne(X-5,Y-5,X+5,Y+5,gmx_v(),gmy_v()) ||
        VRAI==test_ligne(X-5,Y+5,X+5,Y-5,gmx_v(),gmy_v())) {
      Valeur.Num=i;
      Valeur.Type=OEIL;
      return Valeur;
    }

    switch(NF) {
      case 0:
        X=Vid[0].WXs2+(Camera[i].CX+Vid[0].Depla.X)*Vid[0].Echelle;
        Y=Vid[0].WYs2+(Camera[i].CY+Vid[0].Depla.Y)*Vid[0].Echelle;
        break;
      case 1:
        X=Vid[1].WXs2+(Camera[i].CZ+Vid[1].Depla.X)*Vid[1].Echelle;
        Y=Vid[1].WYs2+(Camera[i].CY+Vid[1].Depla.Y)*Vid[1].Echelle;
        break;
      case 2:
        X=Vid[2].WXs2+(Camera[i].CX+Vid[2].Depla.X)*Vid[2].Echelle;
        Y=Vid[2].WYs2+(Camera[i].CZ+Vid[2].Depla.Y)*Vid[2].Echelle;
        break;
    }

    if (VRAI==test_ligne(X-5,Y-5,X+5,Y+5,gmx_v(),gmy_v()) ||
        VRAI==test_ligne(X-5,Y+5,X+5,Y-5,gmx_v(),gmy_v())) {
      Valeur.Num=i;
      Valeur.Type=CIBLE;
      return Valeur;
    }
  }

  Valeur.Type=0;
  Valeur.Num=0;
  return Valeur;
}

// -------------------------------------------------------------------------
// -- DEPLACE UNE CAMERA (OEIL ET/OU CIBLE) --------------------------------
// -------------------------------------------------------------------------
void deplace_camera(void) {
  register DBL X1,Y1;
  int XA,YA,XB,YB;
  DBL DX1,DY1;
  CibleOeil Valeur;
  byte Sujet;
  int i,N;
  int MX,MY;

  LABEL_CAMERA:
  forme_mouse(Sens);

  if (cherche_fenetre()==FAUX) return;
  if (NF!=3) {
    Valeur=selection_camera();
    if (Valeur.Num==0) goto LABEL_CAMERA;

    forme_mouse(MS_FLECHE);

    Sujet=Valeur.Type;
    NumCam=N=Valeur.Num;
  }

  select_vue(NF,CLIP_ON);

  switch (NF) {
    case 0:
      if (Sujet==OEIL) {
        X1=DX1=Camera[N].OX; Y1=DY1=Camera[N].OY;
      } else {
        X1=DX1=Camera[N].CX; Y1=DY1=Camera[N].CY;
      }
      break;
    case 1:
      if (Sujet==OEIL) {
        X1=DX1=Camera[N].OZ; Y1=DY1=Camera[N].OY;
      } else {
        X1=DX1=Camera[N].CZ; Y1=DY1=Camera[N].CY;
      }
      break;
    case 2:
      if (Sujet==OEIL) {
        X1=DX1=Camera[N].OX; Y1=DY1=Camera[N].OZ;
      } else {
        X1=DX1=Camera[N].CX; Y1=DY1=Camera[N].CZ;
      }
      break;
    case 3:
      pan_rotate_camera(TRANSLATE);
      return;
  }


  forme_mouse(MS_FLECHE);
  MX=gmx_r();
  MY=gmy_r();

  while (MouseB());
  GMouseOff();
  VUE_CUBE=1;

  if (Fx4) {
    select_vue(3,CLIP_ON);
    #if !WINDOWS
    _clearscreen(_GVIEWPORT);
    #endif
    for (i=1;i<=NbObjet;i++) if (trace_volume_3(i)) break;
  }

  affiche_camera(N,NF,XOR_PUT);
  
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
      affiche_camera(N,NF,XOR_PUT);
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
        affiche_camera(N,NF,XOR_PUT);
        X1=DX1;
        Y1=DY1;
        Sens_Souris();
        affiche_camera(N,NF,XOR_PUT);
      }
      if (NF==0) if (Sujet==OEIL) { Camera[N].OX=X1; Camera[N].OY=Y1; } else { Camera[N].CX=X1; Camera[N].CY=Y1; }
      if (NF==1) if (Sujet==OEIL) { Camera[N].OZ=X1; Camera[N].OY=Y1; } else { Camera[N].CZ=X1; Camera[N].CY=Y1; }
      if (NF==2) if (Sujet==OEIL) { Camera[N].OX=X1; Camera[N].OZ=Y1; } else { Camera[N].CX=X1; Camera[N].CZ=Y1; }
      affiche_camera(N,NF,XOR_PUT);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      calcule_vision(N);
      if (Fx4==1) {
        select_vue(3,CLIP_ON);
        _clearscreen(_GVIEWPORT);
        for (i=1;i<=NbObjet;i++) {
          if (trace_volume_3(i)) break;
        }
      }
    }
  }

  affiche_camera(N,NF,XOR_PUT);
  
  if (NF==0) if (Sujet==OEIL) { Camera[N].OX=DX1; Camera[N].OY=DY1; } else { Camera[N].CX=DX1; Camera[N].CY=DY1; }
  if (NF==1) if (Sujet==OEIL) { Camera[N].OZ=DX1; Camera[N].OY=DY1; } else { Camera[N].CZ=DX1; Camera[N].CY=DY1; }
  if (NF==2) if (Sujet==OEIL) { Camera[N].OX=DX1; Camera[N].OZ=DY1; } else { Camera[N].CX=DX1; Camera[N].CZ=DY1; }

  cache_camera(1,N,N);

  if (X1!=DX1 || Y1!=DY1) {
    if (NF==0) if (Sujet==OEIL) { Camera[N].OX=X1; Camera[N].OY=Y1; } else { Camera[N].CX=X1; Camera[N].CY=Y1; }
    if (NF==1) if (Sujet==OEIL) { Camera[N].OZ=X1; Camera[N].OY=Y1; } else { Camera[N].CZ=X1; Camera[N].CY=Y1; }
    if (NF==2) if (Sujet==OEIL) { Camera[N].OX=X1; Camera[N].OZ=Y1; } else { Camera[N].CX=X1; Camera[N].CZ=Y1; }
    MX+=((X1-DX1)*Vid[NF].Echelle);
    MY+=((Y1-DY1)*Vid[NF].Echelle);
    while (MouseB());
    NumCam=N;
  }

  cache_camera(0,N,N);

  VUE_CUBE=0;
  calcule_vision(N);

  redessine_fenetre(3,1);

  while (MouseB());
  place_mouse(MX,MY);
  forme_mouse(MS_FLECHE);
  GMouseOn();
  goto LABEL_CAMERA;
}

// -------------------------------------------------------------------------
// -- TEST MIN ET MAX DE LA CAMERA -----------------------------------------
// -------------------------------------------------------------------------
void max_min_camera(byte Vue) {
  register int N;

  if (Camera[0].Cache) return;

  if (Vue==0) {
    for (N=1;N<=NbCamera;N++) {
      // --------------- test Position de la camera
      Vid[0].Max.X=(Camera[N].OX>Vid[0].Max.X ? Camera[N].OX:Vid[0].Max.X);
      Vid[0].Min.X=(Camera[N].OX<Vid[0].Min.X ? Camera[N].OX:Vid[0].Min.X);
      Vid[0].Max.Y=(Camera[N].OY>Vid[0].Max.Y ? Camera[N].OY:Vid[0].Max.Y);
      Vid[0].Min.Y=(Camera[N].OY<Vid[0].Min.Y ? Camera[N].OY:Vid[0].Min.Y);
      // --------------- test Position du look_at
      Vid[0].Max.X=(Camera[N].CX>Vid[0].Max.X ? Camera[N].CX:Vid[0].Max.X);
      Vid[0].Min.X=(Camera[N].CX<Vid[0].Min.X ? Camera[N].CX:Vid[0].Min.X);
      Vid[0].Max.Y=(Camera[N].CY>Vid[0].Max.Y ? Camera[N].CY:Vid[0].Max.Y);
      Vid[0].Min.Y=(Camera[N].CY<Vid[0].Min.Y ? Camera[N].CY:Vid[0].Min.Y);
    }
    return;
  }

  if (Vue==1) {
    for (N=1;N<=NbCamera;N++) {
      // --------------- test Position de la cam‚ra
      Vid[1].Max.X=(Camera[N].OZ>Vid[1].Max.X ? Camera[N].OZ:Vid[1].Max.X);
      Vid[1].Min.X=(Camera[N].OZ<Vid[1].Min.X ? Camera[N].OZ:Vid[1].Min.X);
      Vid[1].Max.Y=(Camera[N].OY>Vid[1].Max.Y ? Camera[N].OY:Vid[1].Max.Y);
      Vid[1].Min.Y=(Camera[N].OY<Vid[1].Min.Y ? Camera[N].OY:Vid[1].Min.Y);
      // --------------- test Position du look_at
      Vid[1].Max.X=(Camera[N].CZ>Vid[1].Max.X ? Camera[N].CZ:Vid[1].Max.X);
      Vid[1].Min.X=(Camera[N].CZ<Vid[1].Min.X ? Camera[N].CZ:Vid[1].Min.X);
      Vid[1].Max.Y=(Camera[N].CY>Vid[1].Max.Y ? Camera[N].CY:Vid[1].Max.Y);
      Vid[1].Min.Y=(Camera[N].CY<Vid[1].Min.Y ? Camera[N].CY:Vid[1].Min.Y);
    }
    return;
  }

  if (Vue==2) {
    for (N=1;N<=NbCamera;N++) {
      // --------------- test Position de la cam‚ra
      Vid[2].Max.X=(Camera[N].OX>Vid[2].Max.X ? Camera[N].OX:Vid[2].Max.X);
      Vid[2].Min.X=(Camera[N].OX<Vid[2].Min.X ? Camera[N].OX:Vid[2].Min.X);
      Vid[2].Max.Y=(Camera[N].OZ>Vid[2].Max.Y ? Camera[N].OZ:Vid[2].Max.Y);
      Vid[2].Min.Y=(Camera[N].OZ<Vid[2].Min.Y ? Camera[N].OZ:Vid[2].Min.Y);
      // --------------- test Position du look_at
      Vid[2].Max.X=(Camera[N].CX>Vid[2].Max.X ? Camera[N].CX:Vid[2].Max.X);
      Vid[2].Min.X=(Camera[N].CX<Vid[2].Min.X ? Camera[N].CX:Vid[2].Min.X);
      Vid[2].Max.Y=(Camera[N].CZ>Vid[2].Max.Y ? Camera[N].CZ:Vid[2].Max.Y);
      Vid[2].Min.Y=(Camera[N].CZ<Vid[2].Min.Y ? Camera[N].CZ:Vid[2].Min.Y);
    }
    return;
  }
}

// -------------------------------------------------------------------------
// -- CALCULE LA DISTANCE FOCALE DE LA CAMERA ------------------------------
// -------------------------------------------------------------------------
DBL calcule_fov(int N) {
  return (0.5/(tan((Camera[N].Ouverture*PIs180)/2)))*2;
}

// -------------------------------------------------------------------------
// -- CALCUL LATITUDE ET LONGITUDE CAMERA ----------------------------------
// -------------------------------------------------------------------------
void calcule_vision (int N) {
  register DBL CX,CY,CZ,DI;

  CX=Camera[N].CX-Camera[N].OX;
  CY=Camera[N].CY-Camera[N].OY;
  CZ=-(Camera[N].CZ-Camera[N].OZ);

  Focale=-(Vid[3].WX*calcule_fov(NumCam))*Ratio;

  DI=sqrt(CZ*CZ+CX*CX);
  if (DI==0 && CY>0) { Longitude=0; Latitude=+PI/2; return; }
  if (DI==0 && CY<0) { Longitude=0; Latitude=-PI/2; return; }
  Latitude=atan(CY/DI);
  if (CZ==0 && CX>0) { Longitude=+M_PI_2; return; }
  if (CZ==0 && CX<0) { Longitude=-M_PI_2; return; }
  Longitude=atan(CX/CZ);
  if (CZ<0 && CX>0) Longitude-=PI;
  if (CZ<0 && CX<0) Longitude+=PI;
  if (CZ<0 && CX==0) Longitude=PI;
  if (CZ==0 && CX<0) Longitude=-PI/2;
}

// -------------------------------------------------------------------------
// -- CALCULE LONGUEUR POUR LA PROFONDEUR DE CHAMP -------------------------
// -------------------------------------------------------------------------
void profondeur_champ(int N) {
  register DBL Facteur=1.0;
  register DBL X1,Y1,X2,Y2,DeformDebut,Deform;
  CibleOeil Valeur;

  if (pas_objet(1)) return;

  forme_mouse(MS_SELECTEUR);

  if (cherche_fenetre()==FAUX) return;
  Valeur=selection_camera();
  if (Valeur.Num==0) return;
  N=Valeur.Num;
  calcule_vision(N);

  if (Camera[N].ProChamp==0.0) Camera[N].ProChamp=1.0;
  DeformDebut=Deform=Camera[N].ProChamp;
  NumObjet=0;

  modif_objet(0,Camera[N].ProChamp,Camera[N].ProChamp,Camera[N].ProChamp,SCALE);
  modif_objet(0,Latitude/PIs180,-Longitude/PIs180,0.0,ROTATE);
  modif_objet(0,Camera[N].OX,Camera[N].OY,Camera[N].OZ,TRANSLATE);

  message("Choose sharp zone");

  forme_mouse(MS_FLECHE);
  while (MouseB());
  GMouseOff();

  trace_boite(PCHAMP,PCHAMP);
  place_mouse(CentX,CentY);

  X2=gmx_r();
  Y2=gmy_r();

  while (MouseB()!=1) {
    X1=gmx_r();
    Y1=gmy_r();
    if (sequence_sortie()) { Camera[N].ProChamp=DeformDebut; break; }
    if (X1!=X2 || Y1!=Y2) {
      delay(5);
      trace_boite(PCHAMP,PCHAMP);
      Facteur+=(((X1-X2)+(Y1-Y2))*0.5)/Vid[NF].Echelle;
      Deform=DeformDebut*Facteur;
      Objet[0]->S[0]=Objet[0]->S[1]=Objet[0]->S[2]=Deform;
      Camera[N].ProChamp=fabs(Deform);
      trace_boite(PCHAMP,PCHAMP);
      place_mouse(CentX,CentY);
      X2=gmx_r();
      Y2=gmy_r();
      message("%s=%.2lf ",RES_MESS[1],Camera[N].ProChamp);
    }
  }

  trace_boite(PCHAMP,PCHAMP);
  
  Objet[0]->S[0]=Objet[0]->S[1]=Objet[0]->S[2]=fabs(Deform);
      
  forme_mouse(MS_FLECHE);
  GMouseOn();
}

// -------------------------------------------------------------------------
// -- MODIFICATION DE L'OUVERTURE ------------------------------------------
// -------------------------------------------------------------------------
void modif_ouverture (int N) {
  register DBL VMax=180;
  register DBL Valeur,ValeurDebut;
  register DBL XA,YA,XB,YB;
  register int i;

  if (pas_objet(1)) return;

  Valeur=ValeurDebut=Camera[N].Ouverture;

  while(MouseB());
  VUE_CUBE=1;
  GMouseOff();

  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();

  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) { Camera[N].Ouverture=ValeurDebut; break; }
    if (XA!=XB || YA!=YB) {
      delay(5);
      Valeur+=(((XA-XB)+(YA-YB))*0.5);
      Valeur=(Valeur>VMax ? VMax:Valeur);
      Valeur=(Valeur<10 ? 10:Valeur);
      Camera[N].Ouverture=Valeur;
      XB=gmx_r();
      YB=gmy_r();
      calcule_vision(N);
      message("%s=%+.2lf",RES_MESS[2],fabs(Valeur));
      select_vue(3,CLIP_ON);
      #if !WINDOWS
      _clearscreen(_GVIEWPORT);
      #endif
      for (i=1;i<=NbObjet;i++) {
        if (trace_volume_3(i)) break;
      }
    }
  }

  VUE_CUBE=0;
  calcule_vision(N);
  redessine_fenetre(3,1);
  while(MouseB());
}

// -------------------------------------------------------------------------
// -- CHANGE LA COULEUR DE LA CAMERA ---------------------------------------
// -------------------------------------------------------------------------
void couleur_camera(int N) {
  int Temp;

  Temp=affiche_palette();
  if (Temp!=-1) {
    CCAMERA=Temp;
    affiche_camera(N,0,COPY_PUT);
    affiche_camera(N,1,COPY_PUT);
    affiche_camera(N,2,COPY_PUT);
  }
}

// -------------------------------------------------------------------------
// -- AUGMENTE LE NOMBRE DE CAMERA -----------------------------------------
// -------------------------------------------------------------------------
int incr_NbCamera(byte Val) {
  Val=Val;
  if (NbCamera<NB_CAMERA_MAX) { NbCamera++; return 1; }
  return 0;
}

// -------------------------------------------------------------------------
// -- AFFICHE OU CACHE LES CAMERAS -----------------------------------------
// -------------------------------------------------------------------------
void cache_camera(byte Mode,byte D,byte F) {
  register int i;

  Camera[0].Cache=0;

  if (!Mode) {
    for (i=D;i<=F;i++) {
      affiche_camera(i,0,COPY_PUT);
      affiche_camera(i,1,COPY_PUT);
      affiche_camera(i,2,COPY_PUT);
    }
  }

  if (Mode) {
    for (i=D;i<=F;i++) {
      affiche_camera(i,0,HIDE_PUT);
      affiche_camera(i,1,HIDE_PUT);
      affiche_camera(i,2,HIDE_PUT);
    }
    Camera[0].Cache=1;
  }

  Camera[0].Cache=Mode;
}

// -------------------------------------------------------------------------
// -- SUPPRIME UNE CAMERA --------------------------------------------------
// -------------------------------------------------------------------------
void supprime_camera(int N) {
  register byte i;
  CibleOeil Valeur;

  forme_mouse(MS_SELECTEUR);
  if (cherche_fenetre()==FAUX) goto LABEL_SUPPRIME;
  Valeur=selection_camera();
  if (Valeur.Num==0) return;
  N=Valeur.Num;

  select_vue(NF,CLIP_ON);

  forme_mouse(MS_FLECHE);
  strcpy(StrBoite[0],"Delete a camera");
  strcpy(StrBoite[1],"Do you really want to");
  sprintf(StrBoite[2],"delete the camera nø%d ?",N);

  if (g_boite_ONA(CentX,CentY,2,CENTRE,1)==0) {
    cache_camera(1,1,NbCamera);
    for (i=N;i<=NbCamera;i++) {
      Camera[i]=Camera[i+1];
    }
    NbCamera--;
    affiche_donnees();
    NumCam=NbCamera;
    redessine_fenetre(3,1);
    cache_camera(0,1,NbCamera);
  }

  LABEL_SUPPRIME:
  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- RENVOI EIN BETIT MEZZAGE D'ERR“R ZI PAS DES CAM‚RA ! -----------------
// -------------------------------------------------------------------------
byte pas_camera(byte Son) {
  if (NbCamera==0) {
    if (Son) beep_erreur();
    message("There's no camera in the scene ! Unavailable command...");
    return VRAI;
  } else {
    return FAUX;
  }
}

// -------------------------------------------------------------------------
// -- CALCULE LONGUEUR POUR LA PROFONDEUR DU BROUILLARD --------------------
// -------------------------------------------------------------------------
DBL distance_fog(DBL Distance) {
  register DBL Facteur=1.0;
  register DBL X1,Y1,X2,Y2;
  Vecteur D,DD;
  CibleOeil Valeur;
  int N;

  if (pas_camera(1)) return Distance;

  forme_mouse(MS_SELECTEUR);

  if (cherche_fenetre()==FAUX) return Distance;
  message("Pick an camera");
  Valeur=selection_camera();
  if (Valeur.Num==0) return Distance;
  N=Valeur.Num;
  calcule_vision(N);

  vect_init(DD,Distance,Distance,Distance);
  vect_init(D,Distance,Distance,Distance);
  if (D[0]==0) {
    vect_init(D,0.1,0.1,0.1);
    vect_init(D,0.1,0.1,0.1);
  }
  NumObjet=0;

  modif_objet(0,Distance,Distance,Distance,SCALE);
  modif_objet(0,Latitude/PIs180,-Longitude/PIs180,0.0,ROTATE);
  modif_objet(0,Camera[N].OX,Camera[N].OY,Camera[N].OZ,TRANSLATE);

  message("Choose the distance");

  forme_mouse(MS_FLECHE);
  while (MouseB());
  GMouseOff();

  trace_boite(PCHAMP,PCHAMP);
  place_mouse(CentX,CentY);

  X2=gmx_r();
  Y2=gmy_r();

  while (MouseB()!=1) {
    X1=gmx_r();
    Y1=gmy_r();
    if (sequence_sortie()) { Distance=DD[0]; break; }
    if (X1!=X2 || Y1!=Y2) {
      delay(5);
      trace_boite(PCHAMP,PCHAMP);
      Facteur+=(((X1-X2)+(Y1-Y2))*0.5)/Vid[NF].Echelle;
      vect_scale(Objet[0]->S,DD,fabs(Facteur));
      Distance=Objet[0]->S[0];
      trace_boite(PCHAMP,PCHAMP);
      place_mouse(CentX,CentY);
      X2=gmx_r();
      Y2=gmy_r();
      message("Distance: %.2lf ",Distance);
    }
  }

  trace_boite(PCHAMP,PCHAMP);
  
  forme_mouse(MS_FLECHE);
  while (MouseB());
  GMouseOn();
  return Distance;
}

// -------------------------------------------------------------------------
// -- ATTRIBUTS ET SETUP D'UNE CAMERA --------------------------------------
// -------------------------------------------------------------------------
void setup_camera(void) {
  int X1=CentX-85;
  int X2=CentX+85;
  int Y1=CentY-185;
  int Y2=CentY+190;
  int XA=X1+10,YA=Y1+ 30;
  int XB=X1+10,YB=Y1+155;
  int XC=X1+10,YC=Y1+260;
  int N,i;
  CibleOeil Valeur;

  if (pas_camera(1)) return;

  LABEL_SETUP_CAMERA:

  message("Pick a camera");
  forme_mouse(MS_SELECTEUR);

  if (cherche_fenetre()==FAUX) return;
  Valeur=selection_camera();
  if (Valeur.Num==0) goto LABEL_SETUP_CAMERA;
  forme_mouse(MS_FLECHE);

  N=Valeur.Num;

  {
     char Buffer[256];
     sprintf(Buffer,"Camera #%d setup",N);
     g_fenetre(X1,Y1,X2,Y2,Buffer,AFFICHE);
  }

  border(XA,YA,XA+150,YA+116,0,1);
  init_case(11,XA+10,YA+10,"Use focal blur",Camera[N].F_Blur,"Use focal blur or not");
  sprintf(ZTexte[0].Variable,"%.4f",Camera[N].Samples);
  init_texte(0,XA+73,YA+30,"Blur samples",ZTexte[0].Variable,7,"Set blur samples");
  sprintf(ZTexte[1].Variable,"%.4f",Camera[N].Aperture);
  init_texte(1,XA+73,YA+50,"Aperture",ZTexte[1].Variable,7,"Set the aperture for camera");
  sprintf(ZTexte[2].Variable,"%.4f",Camera[N].Variance);
  init_texte(2,XA+73,YA+70,"Variance",ZTexte[2].Variable,7,"Set variance");
  sprintf(ZTexte[3].Variable,"%.4f",Camera[N].Confidence);
  init_texte(3,XA+73,YA+90,"Confidence",ZTexte[3].Variable,7,"Set confidence");

  border(XB,YB,XB+150,YB+95,0,1);
  sprintf(ZTexte[4].Variable,"%.4f",Camera[N].OX);
  init_texte(4,XB+73,YB+11,"Location X",ZTexte[4].Variable,7,"Set X location");
  sprintf(ZTexte[5].Variable,"%.4f",-Camera[N].OY);
  init_texte(5,XB+73,YB+31,"Location Y",ZTexte[5].Variable,7,"Set Y location");
  sprintf(ZTexte[6].Variable,"%.4f",-Camera[N].OZ);
  init_texte(6,XB+73,YB+51,"Location Z",ZTexte[6].Variable,7,"Set Z location");
  sprintf(ZTexte[10].Variable,"%.4f",Camera[N].Roll);
  init_texte(10,XB+73,YB+71,"Degres Roll",ZTexte[10].Variable,7,"Set camera rolling");

  border(XC,YC,XC+150,YC+75,0,1);
  sprintf(ZTexte[7].Variable,"%.4f",Camera[N].CX);
  init_texte(7,XC+73,YC+11,"Look at X",ZTexte[7].Variable,7,"Set look at on X");
  sprintf(ZTexte[8].Variable,"%.4f",-Camera[N].CY);
  init_texte(8,XC+73,YC+31,"Look at Y",ZTexte[8].Variable,7,"Set look at on Y");
  sprintf(ZTexte[9].Variable,"%.4f",-Camera[N].CZ);
  init_texte(9,XC+73,YC+51,"Look at Z",ZTexte[9].Variable,7,"Set look at on Z");

  affiche_case(11);
  place_zone_texte(0);
  place_zone_texte(1);
  place_zone_texte(2);
  place_zone_texte(3);
  place_zone_texte(4);
  place_zone_texte(5);
  place_zone_texte(6);
  place_zone_texte(7);
  place_zone_texte(8);
  place_zone_texte(9);
  place_zone_texte(10);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_texte(0,10);
    test_case(11,11);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    Camera[N].F_Blur=Cc[11].Croix;
    Camera[N].Samples   =atof(ZTexte[0].Variable);
    Camera[N].Aperture  =atof(ZTexte[1].Variable);
    Camera[N].Variance  =atof(ZTexte[2].Variable);
    Camera[N].Confidence=atof(ZTexte[3].Variable);

    cache_camera(1,1,NbCamera);

    Camera[N].OX=atof(ZTexte[4].Variable);
    Camera[N].OY=-atof(ZTexte[5].Variable);
    Camera[N].OZ=-atof(ZTexte[6].Variable);

    Camera[N].CX=atof(ZTexte[7].Variable);
    Camera[N].CY=-atof(ZTexte[8].Variable);
    Camera[N].CZ=-atof(ZTexte[9].Variable);

    Camera[N].Roll=atof(ZTexte[10].Variable);

    cache_camera(0,1,NbCamera);
    redessine_fenetre(3,1);
  }

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- PLACEMENT AUTOMATIQUE D'UNE CAMERA -----------------------------------
// -------------------------------------------------------------------------
void place_camera(void) {
  int X1=CentX-85;
  int X2=CentX+85;
  int Y1=CentY-100;
  int Y2=CentY+100;
  int XA=X1+10,YA=Y1+ 30;
  int N,i,Num;
  CibleOeil Valeur;

  if (pas_camera(1)) return;

  LABEL_SETUP_CAMERA:

  message("Pick a camera");
  forme_mouse(MS_SELECTEUR);

  if (cherche_fenetre()==FAUX) return;
  Valeur=selection_camera();
  if (Valeur.Num==0) goto LABEL_SETUP_CAMERA;
  forme_mouse(MS_FLECHE);

  N=Valeur.Num;

  {
     char Buffer[256];
     sprintf(Buffer,"Camera #%d placement",N);
     g_fenetre(X1,Y1,X2,Y2,Buffer,AFFICHE);
  }

  border(XA,YA,XA+150,YA+50,0,1);
  init_pastille(11,XA+10,YA+10,"Use eye position",1,"Work on camera eye");
  init_pastille(12,XA+10,YA+30,"Use look at position",0,"Work on camera look at");

  border(XA,YA+60,XA+150,YA+130,0,1);
  init_pastille(13,XA+10,YA+ 70,"Center on object",1,"Center position on object");
  init_pastille(14,XA+10,YA+ 90,"Invert camera position",0,"Flip look at and eye");
  init_pastille(15,XA+10,YA+110,"Place to <0,0,0>",0,"Place on universe center");

  affiche_pastille(11);
  affiche_pastille(12);
  affiche_pastille(13);
  affiche_pastille(14);
  affiche_pastille(15);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_groupe_pastille(11,12);
    test_groupe_pastille(13,15);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    
    i=quelle_pastille_dans_groupe(11,12)-11;

    switch (quelle_pastille_dans_groupe(13,15)-13) {
      case 0:
        if (pas_objet(1)) break;
        forme_mouse(MS_SELECTEUR);
        message("Select an object as reference point");
        if ((Num=trouve_volume(0,3,1))==FAUX) break;
        cache_camera(1,1,NbCamera);
        if (i==0) {
          Camera[N].OX=Objet[Num]->T[_X];
          Camera[N].OY=Objet[Num]->T[_Y];
          Camera[N].OZ=Objet[Num]->T[_Z];
        } else {
          Camera[N].CX=Objet[Num]->T[_X];
          Camera[N].CY=Objet[Num]->T[_Y];
          Camera[N].CZ=Objet[Num]->T[_Z];
        }
        
        break;
      case 1:
        cache_camera(1,1,NbCamera);
        swap_dbl(&Camera[N].OX,&Camera[N].CX);
        swap_dbl(&Camera[N].OY,&Camera[N].CY);
        swap_dbl(&Camera[N].OZ,&Camera[N].CZ);
        break;
      case 2:
        if (x_fenetre(CentX,CentY,GAUCHE,1,"Reset %s position to <0,0,0>|"\
          "Do you really want to change it ?|",i==0 ? "eye":"look at")) break;
        cache_camera(1,1,NbCamera);
        if (i==0) Camera[N].OX=Camera[N].OY=Camera[N].OZ=0;
        if (i==1) Camera[N].CX=Camera[N].CY=Camera[N].CZ=0;
        break;
    }

    

    cache_camera(0,1,NbCamera);
    redessine_fenetre(3,1);
  }

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- ROLLING THE CAMERA ---------------------------------------------------
// -------------------------------------------------------------------------
void roll_camera(int N) {
  register DBL Valeur,ValeurDebut;
  register DBL XA,YA,XB,YB;
  register int i;

  if (pas_objet(1)) return;

  Valeur=ValeurDebut=Camera[N].Roll;

  while(MouseB());
  VUE_CUBE=1;
  GMouseOff();

  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();

  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) { Camera[N].Roll=ValeurDebut; break; }
    if (XA!=XB || YA!=YB) {
      delay(5);
      Valeur+=(((XA-XB)+(YA-YB))*0.5);
      Valeur=(Valeur>+180 ? +180:Valeur);
      Valeur=(Valeur<-180 ? -180:Valeur);
      Camera[N].Roll=Valeur;
      XB=gmx_r();
      YB=gmy_r();
      calcule_vision(N);
      message("%s=%+.2lf",RES_MESS[2],Valeur);
      select_vue(3,CLIP_ON);
      #if !WINDOWS
      _clearscreen(_GVIEWPORT);
      #endif
      for (i=1;i<=NbObjet;i++) {
        if (trace_volume_3(i)) break;
      }
    }
  }

  VUE_CUBE=0;
  calcule_vision(N);
  redessine_fenetre(3,1);
  while(MouseB());
}

// -------------------------------------------------------------------------
// -- PAN ET ROTATE THE CAMERA ---------------------------------------------
// -------------------------------------------------------------------------
void pan_rotate_camera(byte Pan) {
  DBL OX,OY,OZ;
  DBL CX,CY,CZ;
  Vecteur V1,V2;
  register DBL X1=0,Y1=0;
  int XA,YA,XB,YB;
  DBL DX1,DY1;
  int i;
  int MX,MY;
  TRANSFORM *Mat;
  Vecteur SC,SH,RO,TR;
  byte Angle;


  OX=Camera[NumCam].OX;
  OY=Camera[NumCam].OY;
  OZ=Camera[NumCam].OZ;
  CX=Camera[NumCam].CX;
  CY=Camera[NumCam].CY;
  CZ=Camera[NumCam].CZ;

  select_vue(3,CLIP_ON);
  while (MouseB());
  GMouseOff();
  forme_mouse(MS_FLECHE);
  place_mouse(CentX,CentY);
  DX1=MX=XB=gmx_r();
  DY1=MY=YB=gmy_r();

  VUE_CUBE=1;
  
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
      affiche_camera(NumCam,NF,XOR_PUT);
      switch (Sens) {
        case MS_X:
          X1+=(XA-XB);
          Angle=0;
          break;
        case MS_Y:
          Y1+=(YA-YB);
          Angle=1;
          break;
        default:
          X1+=(XA-XB);
          Y1+=(YA-YB);
          Angle=2;
          break;
      }
      if ((kbhit()) && getch()==9) {
        affiche_camera(NumCam,NF,XOR_PUT);
        X1=DX1;
        Y1=DY1;
        Sens_Souris();
        affiche_camera(NumCam,NF,XOR_PUT);
      }
      
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();

      switch(Pan) {
        case ROTATE:
          vect_init(V1,Camera[NumCam].OX-Camera[NumCam].CX,
                       Camera[NumCam].OY-Camera[NumCam].CY,
                       Camera[NumCam].OZ-Camera[NumCam].CZ);

          Mat=Create_Transform();

          translation_objet(Mat,V1,'+');
      
          vect_init(V2,Y1,X1,0);
          rotation_objet(Mat,V2,'+');
      
          mat_decode(Mat->matrix,SC,SH,RO,TR);
          Efface_Transform(Mat);
      
          Camera[NumCam].OX=TR[_X]+CX;
          Camera[NumCam].OY=TR[_Y]+CY;
          Camera[NumCam].OZ=TR[_Z]+CZ;
          break;
        case TRANSLATE:
          vect_init(V1,Camera[NumCam].CX,
                       Camera[NumCam].CY,
                       Camera[NumCam].CZ);

          Mat=Create_Transform();

          translation_objet(Mat,V1,'+');
      
          vect_init(V2,X1/10,Y1/10,0);
          translation_objet(Mat,V2,'+');
      
          mat_decode(Mat->matrix,SC,SH,RO,TR);
          Efface_Transform(Mat);
      
          Camera[NumCam].CX=TR[_X];
          Camera[NumCam].CY=TR[_Y];
          Camera[NumCam].CZ=TR[_Z];
          break;
      }

      X1=Y1=0;
      
      calcule_vision(NumCam);
      affiche_camera(NumCam,NF,XOR_PUT);

      if (Fx4==1) {
        select_vue(3,CLIP_ON);
        _clearscreen(_GVIEWPORT);
        for (i=1;i<=NbObjet;i++) {
          if (trace_volume_3(i)) break;
        }
      }
    }
  }

  affiche_camera(NumCam,NF,XOR_PUT);
  VUE_CUBE=0;

  cache_camera(0,NumCam,NumCam);

  if (X1==DX1 && Y1==DY1) {
    Camera[NumCam].OX=OX;
    Camera[NumCam].OY=OY;
    Camera[NumCam].OZ=OZ;
    Camera[NumCam].CX=CX;
    Camera[NumCam].CY=CY;
    Camera[NumCam].CZ=CZ;
  }

  cache_camera(0,NumCam,NumCam);

  VUE_CUBE=0;
  calcule_vision(NumCam);

  redessine_fenetre(3,1);

  while (MouseB());
  forme_mouse(MS_FLECHE);
  GMouseOn();
}
