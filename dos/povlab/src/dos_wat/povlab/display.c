/* ---------------------------------------------------------------------------
*  DSIPLAY.C
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
#include <TIME.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"
#if !WINDOWS
#include <GRAPH.H>
#endif

byte VUE_CUBE=0;
byte NF;                              // Num‚ro de fenˆtre en cours 0,1 et 2 (3)
int NumObjet,NbObjet;                 // Num‚ro objet, nombre d'objet
byte GrilleType=1;
byte DrawNF;

// -------------------------------------------------------------------------
// -- TRACE UN VOLUME EN FILAIRE SANS PERSPECTIVE FENETRE 0 ----------------
// -------------------------------------------------------------------------
void trace_volume_0(int N) {
  register int i;
  int NbPoint;
  byte C,Vite=0;

  if (Vid[0].Enable==0) return;
  if (NF!=0 && Fx4==0) return;
  if (Objet[N]->Cache) return;
  NbPoint=Objet[N]->Rapide==2 ? data_objet(CUBE_R,N,0):data_objet(Objet[N]->Type,N,0);
  
  if (Objet[N]->CSG!=PAS_CSG) {
    type_motif_ligne(3);
  }

  for (i=0;i<=NbPoint;i++) {
    Point[i].X*=Vid[0].Echelle*Objet[N]->S[0];
    Point[i].Y*=Vid[0].Echelle*Objet[N]->S[1];
    Point[i].Z*=Vid[0].Echelle*Objet[N]->S[2];
    rotationX(i,Objet[N]->R[0]*PIs180);
    rotationY(i,Objet[N]->R[1]*PIs180);
    rotationZ(i,Objet[N]->R[2]*PIs180);
    Point[i].X+=Vid[0].WXs2+Vid[0].Echelle*(Objet[N]->T[0]+Vid[0].Depla.X);
    Point[i].Y+=Vid[0].WYs2+Vid[0].Echelle*(Objet[N]->T[1]+Vid[0].Depla.Y);
  }

  select_vue(0,CLIP_ON);

  for (i=0;i<=NbPoint;i++) {
    if (Objet[N]->Rapide==1) {
      if (Point[i].V==0 && Vite!=0) Point[i].V=1;
      Vite=(Vite<5 ? Vite+1:0);
    }
    if (Point[i].V) {
      move_to(Point[i].X,Point[i].Y);
    } else {
      C=(Objet[N]->Selection==1 ? CSELECTION:Point[i].C==-1 ? Objet[N]->Couleur:Point[i].C);
      if (Objet[N]->Freeze) C=CGRILLE;
      g_ligne_to(Point[i].X,Point[i].Y,C);
    }
    if (Point[i].C==-2 &&
        Objet[N]->Rapide==0 &&
        Objet[N]->Couleur!=FFOND &&
        ShowCtrlPt) {
      g_rectangle(Point[i].X-2,Point[i].Y-2,
                  Point[i].X+2,Point[i].Y+2,BLANC,1);
    }
  }

  if (Objet[N]->Operator) type_motif_ligne(0);
}

// ----------------------------------------------------------------------------
// -- TRACE UN VOLUME EN FILAIRE SANS PERSPECTIVE FENETRE 1 -------------------
// ----------------------------------------------------------------------------
void trace_volume_1(int N) {
  register int i;
  int NbPoint;
  byte C,Vite=0;

  if (Vid[1].Enable==0) return;
  if (NF!=1 && Fx4==0) return;
  if (Objet[N]->Cache) return;
  NbPoint=Objet[N]->Rapide==2 ? data_objet(CUBE_R,N,1):data_objet(Objet[N]->Type,N,1);
  C=(Objet[N]->Selection==1 ? CSELECTION:Objet[N]->Couleur);
  if (Objet[N]->CSG!=PAS_CSG) {
    type_motif_ligne(3);
  }

  for (i=0;i<=NbPoint;i++) {
    Point[i].X*=Vid[1].Echelle*Objet[N]->S[0];
    Point[i].Y*=Vid[1].Echelle*Objet[N]->S[1];
    Point[i].Z*=Vid[1].Echelle*Objet[N]->S[2];
    rotationX(i,Objet[N]->R[0]*PIs180);
    rotationY(i,Objet[N]->R[1]*PIs180);
    rotationZ(i,Objet[N]->R[2]*PIs180);
    Point[i].Z+=Vid[1].WXs2+Vid[1].Echelle*(Objet[N]->T[2]+Vid[1].Depla.X);
    Point[i].Y+=Vid[1].WYs2+Vid[1].Echelle*(Objet[N]->T[1]+Vid[1].Depla.Y);
  }

  select_vue(1,CLIP_ON);

  for (i=0;i<=NbPoint;i++) {
    if (Objet[N]->Rapide==1) {
      if (Point[i].V==0 && Vite!=0) Point[i].V=1;
      Vite=(Vite<5 ? Vite+1:0);
    }
    if (Point[i].V) {
      move_to(Point[i].Z,Point[i].Y);
    } else {
      C=(Objet[N]->Selection==1 ? CSELECTION:Point[i].C==-1 ? Objet[N]->Couleur:Point[i].C);
      if (Objet[N]->Freeze) C=CGRILLE;
      g_ligne_to(Point[i].Z,Point[i].Y,C);
    }
    if (Point[i].C==-2 &&
        Objet[N]->Rapide==0 &&
        Objet[N]->Couleur!=FFOND &&
        ShowCtrlPt) {
      g_rectangle(Point[i].Z-2,Point[i].Y-2,
                  Point[i].Z+2,Point[i].Y+2,BLANC,1);
    }
  }
  if (Objet[N]->Operator) type_motif_ligne(0);
}

// ----------------------------------------------------------------------------
// -- TRACE UN VOLUME EN FILAIRE SANS PERSPECTIVE FENETRE 2 -------------------
// ----------------------------------------------------------------------------
void trace_volume_2(int N) {
  register int i;
  int NbPoint;
  byte C,Vite=0;

  if (Vid[2].Enable==0) return;
  if (NF!=2 && Fx4==0) return;
  if (Objet[N]->Cache) return;
  NbPoint=Objet[N]->Rapide==2 ? data_objet(CUBE_R,N,2):data_objet(Objet[N]->Type,N,2);
  C=(Objet[N]->Selection==1 ? CSELECTION:Objet[N]->Couleur);
  if (Objet[N]->CSG!=PAS_CSG) {
    type_motif_ligne(3);
  }

  for (i=0;i<=NbPoint;i++) {
    Point[i].X*=Vid[2].Echelle*Objet[N]->S[0];
    Point[i].Y*=Vid[2].Echelle*Objet[N]->S[1];
    Point[i].Z*=Vid[2].Echelle*Objet[N]->S[2];
    rotationX(i,Objet[N]->R[0]*PIs180);
    rotationY(i,Objet[N]->R[1]*PIs180);
    rotationZ(i,Objet[N]->R[2]*PIs180);
    Point[i].X+=Vid[2].WXs2+Vid[2].Echelle*(Objet[N]->T[0]+Vid[2].Depla.X);
    Point[i].Z+=Vid[2].WYs2+Vid[2].Echelle*(Objet[N]->T[2]+Vid[2].Depla.Y);
  }

  select_vue(2,CLIP_ON);

  for (i=0;i<=NbPoint;i++) {
    if (Objet[N]->Rapide==1) {
      if (Point[i].V==0 && Vite!=0) Point[i].V=1;
      Vite=(Vite<5 ? Vite+1:0);
    }
    if (Point[i].V) {
      move_to(Point[i].X,Point[i].Z);
    } else {
      C=(Objet[N]->Selection==1 ? CSELECTION:Point[i].C==-1 ? Objet[N]->Couleur:Point[i].C);
      if (Objet[N]->Freeze) C=CGRILLE;
      g_ligne_to(Point[i].X,Point[i].Z,C);
    }
    if (Point[i].C==-2 &&
        Objet[N]->Rapide==0 &&
        Objet[N]->Couleur!=FFOND &&
        ShowCtrlPt) {
      g_rectangle(Point[i].X-2,Point[i].Z-2,
                  Point[i].X+2,Point[i].Z+2,BLANC,1);
    }
  }

  if (Objet[N]->Operator) type_motif_ligne(0);
}

// -------------------------------------------------------------------------
// -- PERSECTIVE 3D --------------------------------------------------------
// -------------------------------------------------------------------------
byte trace_volume_3(int N) {
  register int i;
  register int X,Y,NbPoint;
  byte C,Vite=0;

  if (Vid[3].Enable==0) return 0;
  if (NF!=3 && Fx4==0) return 0;
  if (Objet[N]->Cache) return 0;
  if (Objet[N]->Freeze) C=CGRILLE;

  if (VUE_CUBE || Objet[N]->Rapide==2) {
    NbPoint=data_objet(CUBE_R,N,3);
  } else {
    NbPoint=data_objet(Objet[N]->Type,N,3);
  }

  if (VUE_CUBE==0) calcule_vision(NumCam);

  X=gmx_r(); Y=gmy_r();

  if (VUE_CUBE==0) select_vue(3,CLIP_ON);

  C=(Objet[N]->Selection==1 ? CSELECTION:Objet[N]->Couleur);
  if (Objet[N]->CSG!=PAS_CSG) {
    type_motif_ligne(3);
  } else {
    if (Objet[N]->Freeze) C=CGRILLE;
  }

  for (i=0;i<=NbPoint;i++) {
    Point[i].X*=Objet[N]->S[0];
    Point[i].Y*=Objet[N]->S[1];
    Point[i].Z*=Objet[N]->S[2];
    rotationX(i,Objet[N]->R[0]*PIs180);
    rotationY(i,Objet[N]->R[1]*PIs180);
    rotationZ(i,Objet[N]->R[2]*PIs180);
    Point[i].X+=Objet[N]->T[0]-Camera[NumCam].OX;
    Point[i].Y+=Objet[N]->T[1]-Camera[NumCam].OY;
    Point[i].Z+=Objet[N]->T[2]-Camera[NumCam].OZ;

    rotationY(i,Longitude);
    rotationX(i,Latitude);
    rotationZ(i,-Camera[NumCam].Roll*PIs180);

    Point[i].X=Vid[3].WXs2+(Focale*Point[i].X/Point[i].Z);
    Point[i].Y=Vid[3].WYs2+(Focale*Point[i].Y/Point[i].Z);

    if (Point[i].Z>=0) {
      Point[i].X=-Point[i].X;
      Point[i].Y=-Point[i].Y;
      Point[i].V=1;
    }

    if (Objet[N]->Rapide==1 && VUE_CUBE==0) {
      if (Point[i].V==0 && Vite!=0) Point[i].V=1;
      Vite=(Vite<5 ? Vite+1:0);
    }

    if (Point[i].V) {
      move_to(Point[i].X,Point[i].Y);
    } else {
      g_ligne_to(Point[i].X,Point[i].Y,C);
    }
  }

  if (Objet[N]->CSG) type_motif_ligne(0);
  if (VUE_CUBE && (X!=gmx_r() || Y!=gmy_r() || MouseB())) return 1;
  return 0;
}

// --------------------------------------------------------------------------
// -- TRACE UN VOLUME EN FILAIRE SANS PERSPECTIVE DANS TOUTES LES FENETRES --
// --------------------------------------------------------------------------
void trace_volume_all(int Num1,int Num2) {
  register int i;

  for (i=Num1;i<=Num2;i++) {
    trace_volume_0(i);
    trace_volume_1(i);
    trace_volume_2(i);
    trace_volume_3(i);
    if (sequence_sortie()) { break; }
  }
}

// -----------------------------------------------------------------------
// -- CONVERSION COORDONNEES ENTRE ECRAN ET VUE --------------------------
// -----------------------------------------------------------------------
struct Coord ecran_to_vue(int NF,DBL X,DBL Y) {
  struct Coord C;

  C.X=-Vid[NF].Depla.X+(X-Vid[NF].XF-Vid[NF].WXs2)/Vid[NF].Echelle;
  C.Y=-Vid[NF].Depla.Y+(Y-Vid[NF].YF-Vid[NF].WYs2)/Vid[NF].Echelle;

  return C;
}

// -----------------------------------------------------------------------
// -- CONVERSION COORDONNEES ENTRE VUE ET ECRAN --------------------------
// -----------------------------------------------------------------------
struct Coord vue_to_ecran(int NF,DBL X,DBL Y) {
  struct Coord C;

  C.X=Vid[NF].Echelle*(Vid[NF].Depla.X+X)+Vid[NF].WXs2;
  C.Y=Vid[NF].Echelle*(Vid[NF].Depla.Y+Y)+Vid[NF].WYs2;

  return C;
}

// -----------------------------------------------------------------------
// -- CONVERSION COORDONNEES ENTRE VUE ET ECRAN --------------------------
// -----------------------------------------------------------------------
void trace_coord(int NF) {
  char Buffer[10];
  struct Coord Cd;
  register DBL PasX,PasY;
  register int i;

  if (Vid[NF].Enable==0) return;
  if (Vid[NF].Coord==0) return;

  select_vue(NF,CLIP_ON);

  PasX=Vid[NF].WX/5;
  PasY=Vid[NF].WY/5;

  for (i=1;i<5;i++) {
    Cd=ecran_to_vue(NF,Vid[NF].XF+i*PasX,Vid[NF].YF+i*PasY);
    g_ligne(i*PasX,13,i*PasX,15,ROUGE);
      sprintf(Buffer,"%+.2f",Cd.X);
      text_xy(i*PasX-15,1,Buffer,CGRILLE);
    g_ligne(29,i*PasY,30,i*PasY,ROUGE);
      sprintf(Buffer,"%+.2f",-Cd.Y);
      text_xy(1,i*PasY-6,Buffer,CGRILLE);
  }

  select_vue(5,CLIP_OFF);
}

// -------------------------------------------------------------------------
// -- REDESSINE UNE/LES FENETRES DU MODELEUR -------------------------------
// -------------------------------------------------------------------------
void redessine_fenetre(byte Val,byte Efface) {
  register int i=0;
  char Cam[12];

  sprintf(Cam,"camera %d",NumCam);
  GMouseOff();
  select_vue(5,CLIP_OFF);
  type_ecriture(COPY_PUT);

  if (Fx4==0) Val=NF;

  if (Efface) {
    if (Val==5 || Val==0) g_rectangle(Vid[0].XF,Vid[0].YF,Vid[0].XF+Vid[0].WX-1,Vid[0].YF+Vid[0].WY-1,FFOND,1);
    if (Val==5 || Val==1) g_rectangle(Vid[1].XF,Vid[1].YF,Vid[1].XF+Vid[1].WX-1,Vid[1].YF+Vid[1].WY-1,FFOND,1);
    if (Val==5 || Val==2) g_rectangle(Vid[2].XF,Vid[2].YF,Vid[2].XF+Vid[2].WX-1,Vid[2].YF+Vid[2].WY-1,FFOND,1);
    if (Val==5 || Val==3) g_rectangle(Vid[3].XF,Vid[3].YF,Vid[3].XF+Vid[3].WX-1,Vid[3].YF+Vid[3].WY-1,FFOND,1);

    if (Val==5 || Val==0) { trace_grille(0); trace_axes(0); trace_coord(0); }
    if (Val==5 || Val==1) { trace_grille(1); trace_axes(1); trace_coord(1); }
    if (Val==5 || Val==2) { trace_grille(2); trace_axes(2); trace_coord(2); }

    select_vue(5,CLIP_OFF);
    if (Val==5 || Val==0) text_xy(Vid[0].XF+2,Vid[0].YF+Vid[0].WY-13,"front",2);
    if (Val==5 || Val==1) text_xy(Vid[1].XF+2,Vid[1].YF+Vid[1].WY-13,"left",2);
    if (Val==5 || Val==2) text_xy(Vid[2].XF+2,Vid[2].YF+Vid[2].WY-13,"top",2);
    if (Val==5 || Val==3) text_xy(Vid[3].XF+2,Vid[3].YF+Vid[3].WY-13,"Cam",2);
  }

  type_ecriture(COPY_PUT);

  if (Val==0 || Val==5) {
    for (i=1;i<=NbObjet;i++) {
      trace_volume_0(i);
      if (sequence_sortie()) { break; }
    }
    for (i=1;i<=NbCamera;i++) affiche_camera(i,0,COPY_PUT);
    for (i=1;i<=NbOmni;i++) affiche_omni(i,0,COPY_PUT);
    for (i=1;i<=NbSpot;i++) affiche_spot(i,0,COPY_PUT,Spot[i].Cone);
    for (i=1;i<=NbArea;i++) affiche_area(i,0,COPY_PUT);
    for (i=1;i<=NbCylLight;i++) affiche_cyllight(i,0,COPY_PUT,CylLight[i].Cone);
  }

  if (Val==1 || Val==5) {
    for (i=1;i<=NbObjet;i++) {
      trace_volume_1(i);
      if (sequence_sortie()) { break; }
    }
    for (i=1;i<=NbCamera;i++) affiche_camera(i,1,COPY_PUT);
    for (i=1;i<=NbOmni;i++) affiche_omni(i,1,COPY_PUT);
    for (i=1;i<=NbSpot;i++) affiche_spot(i,1,COPY_PUT,Spot[i].Cone);
    for (i=1;i<=NbArea;i++) affiche_area(i,1,COPY_PUT);
    for (i=1;i<=NbCylLight;i++) affiche_cyllight(i,1,COPY_PUT,CylLight[i].Cone);
  }

  if (Val==2 || Val==5) {
    for (i=1;i<=NbObjet;i++) {
      trace_volume_2(i);
      if (sequence_sortie()) { break; }
    }
    for (i=1;i<=NbCamera;i++) affiche_camera(i,2,COPY_PUT);
    for (i=1;i<=NbOmni;i++) affiche_omni(i,2,COPY_PUT);
    for (i=1;i<=NbSpot;i++) affiche_spot(i,2,COPY_PUT,Spot[i].Cone);
    for (i=1;i<=NbArea;i++) affiche_area(i,2,COPY_PUT);
    for (i=1;i<=NbCylLight;i++) affiche_cyllight(i,2,COPY_PUT,CylLight[i].Cone);
  }

  if (Val==3 || Val==5) {
    for (i=1;i<=NbObjet;i++) {
      trace_volume_3(i);
      if (sequence_sortie()) { break; }
    }
    for (i=1;i<=NbOmni;i++) affiche_omni_3d(i,type_couleur(OMNI));
    for (i=1;i<=NbSpot;i++) affiche_spot_3d(i,type_couleur(SPOT));
    for (i=1;i<=NbArea;i++) affiche_area_3d(i,type_couleur(AREA));
    for (i=1;i<=NbCylLight;i++) affiche_area_3d(i,type_couleur(CYLLIGHT));
  }

  select_vue(5,CLIP_OFF);
}

// -------------------------------------------------------------------------
// -- SELECTIONNE UN VOLUME/OBJET ------------------------------------------
// -------------------------------------------------------------------------
int trouve_volume(byte F1,byte F2,byte Freeze) {
  register int i,CSouris,A,B;
  register DBL X,Y,Ec=0;

  NumObjet=0;
  CSouris=CURSEUR;

  while (NumObjet==0) {

    RE_TEST_F:

    GMouseOn();
    while (MouseB()==0) {
      if ((kbhit()) && getch()==9) { Sens_Souris(); CSouris=CURSEUR; }
      chercher_menu(CSouris);
    }
    if (MouseB()==1 && chercher_menu(CSouris)==VRAI) { NumObjet=FAUX; break; }

    NF=trouve_fenetre(0);
    if (NF<F1 || NF>F2) goto RE_TEST_F;

    if (sequence_sortie()) {
      NumObjet=FAUX;
      forme_mouse(MS_FLECHE);
      break;
    }

    message("Searching...");

    if (NF!=3) {
      X=-Vid[NF].Depla.X+(gmx_v()-Vid[NF].XF-Vid[NF].WXs2)/Vid[NF].Echelle;
      Y=-Vid[NF].Depla.Y+(gmy_v()-Vid[NF].YF-Vid[NF].WYs2)/Vid[NF].Echelle;
      Ec=5/Vid[NF].Echelle;
    } else {
      X=gmx_v()-Vid[3].XF;
      Y=gmy_v()-Vid[3].YF;
      A=gmx_v();
      B=gmy_v();
      Ec=10/(XMax/Vid[3].WX);
    }

    select_vue(NF,CLIP_ON);

    switch (NF) {
      case 0:
        for (i=1;i<=NbObjet;i++) {
          if (Objet[i]->Cache==0 && (Objet[i]->Freeze!=Freeze || Freeze==0)) {
            if (sequence_sortie()) { NumObjet=0; break; }
            reset_min_max();
            cherche_volume_0(i,2);
            Vid[0].Min.X-=Ec; Vid[0].Min.Y-=Ec; Vid[0].Max.X+=Ec; Vid[0].Max.Y+=Ec;
            if (SuiviSelection) {
              trace_rectangle(Vid[0].Min.X,Vid[0].Min.Y,Vid[0].Max.X,Vid[0].Max.Y,0,2,0);
              delay(10);
              trace_rectangle(Vid[0].Min.X,Vid[0].Min.Y,Vid[0].Max.X,Vid[0].Max.Y,0,2,0);
            }
            if (X>=Vid[0].Min.X && X<=Vid[0].Max.X && Y>=Vid[0].Min.Y && Y<=Vid[0].Max.Y) {
              if (VRAI==cherche_volume_0(i,0)) { NumObjet=i; goto OBJET_OK; }
            }
          }
        }
        break;
      case 1:
        for (i=1;i<=NbObjet;i++) {
          if (Objet[i]->Cache==0 && (Objet[i]->Freeze!=Freeze || Freeze==0)) {
            if (sequence_sortie()) { NumObjet=0; break; }
            reset_min_max();
            cherche_volume_1(i,2);
            Vid[1].Min.X-=Ec; Vid[1].Min.Y-=Ec; Vid[1].Max.X+=Ec; Vid[1].Max.Y+=Ec;
            if (SuiviSelection) {
              trace_rectangle(Vid[1].Min.X,Vid[1].Min.Y,Vid[1].Max.X,Vid[1].Max.Y,0,2,0);
              delay(10);
              trace_rectangle(Vid[1].Min.X,Vid[1].Min.Y,Vid[1].Max.X,Vid[1].Max.Y,0,2,0);
            }
            if (X>=Vid[1].Min.X && X<=Vid[1].Max.X && Y>=Vid[1].Min.Y && Y<=Vid[1].Max.Y) {
              if (VRAI==cherche_volume_1(i,0)) { NumObjet=i; goto OBJET_OK; }
            }
          }
        }
        break;
      case 2:
        for (i=1;i<=NbObjet;i++) {
          if (Objet[i]->Cache==0 && (Objet[i]->Freeze!=Freeze || Freeze==0)) {
            if (sequence_sortie()) { NumObjet=0; break; }
            reset_min_max();
            cherche_volume_2(i,2);
            Vid[2].Min.X-=Ec; Vid[2].Min.Y-=Ec; Vid[2].Max.X+=Ec; Vid[2].Max.Y+=Ec;
            if (SuiviSelection) {
              trace_rectangle(Vid[2].Min.X,Vid[2].Min.Y,Vid[2].Max.X,Vid[2].Max.Y,0,2,0);
              delay(10);
              trace_rectangle(Vid[2].Min.X,Vid[2].Min.Y,Vid[2].Max.X,Vid[2].Max.Y,0,2,0);
            }
            if (X>=Vid[2].Min.X && X<=Vid[2].Max.X && Y>=Vid[2].Min.Y && Y<=Vid[2].Max.Y) {
              if (VRAI==cherche_volume_2(i,0)) { NumObjet=i; goto OBJET_OK; }
            }
          }
        }
        break;
      case 3:
        for (i=1;i<=NbObjet;i++) {
          if (Objet[i]->Cache==0 && (Objet[i]->Freeze!=Freeze || Freeze==0)) {
            if (sequence_sortie()) { NumObjet=0; break; }
            reset_min_max();
            cherche_volume_3(i,2);
            Vid[3].Min.X-=Ec; Vid[3].Min.Y-=Ec; Vid[3].Max.X+=Ec; Vid[3].Max.Y+=Ec;
            // message("%.2f %.2f %.2f %.2f %.2f %.2f",Vid[3].Min.X,Vid[3].Min.Y,Vid[3].Max.X,Vid[3].Max.Y,X,Y);
            if (SuiviSelection) {
              trace_rectangle(Vid[3].Min.X,Vid[3].Min.Y,Vid[3].Max.X,Vid[3].Max.Y,0,2,1);
              delay(10);
              trace_rectangle(Vid[3].Min.X,Vid[3].Min.Y,Vid[3].Max.X,Vid[3].Max.Y,0,2,1);
            }
            if (X>=Vid[3].Min.X && X<=Vid[3].Max.X && Y>=Vid[3].Min.Y && Y<=Vid[3].Max.Y) {
              if (VRAI==cherche_volume_3(i,0)) { NumObjet=i; goto OBJET_OK; }
            }
          }
        }
        break;
    }

    OBJET_OK:

    if (NumObjet) {
      message("Object nø%d (%s)",NumObjet,Objet[NumObjet]->Nom);
    } else {
      message("No object found here !");
    }
  }

  forme_mouse(MS_FLECHE);
  GMouseOff();
  return NumObjet;
}

// -------------------------------------------------------------------------
// -- AFFICHE UNE FORME POUR MANIPULATION ROTATION, TRANSLATION, SCALE -----
// -------------------------------------------------------------------------
void trace_boite(byte Fenetre1,byte Fenetre2) {
  register int i,NbPoint;
  register int j;
  register byte Autre=0;
  byte Deplace=0;
  byte Color=FFOND | JAUNE;

  Objet[0]->Rapide=0;
  if (Fx4==0) Fenetre1=Fenetre2=NF;

  if (Fenetre1==PCHAMP && Fenetre2==PCHAMP) {
    if (Fx4==1) { Fenetre1=0; Fenetre2=2; }
    Autre=PCHAMP;
  }

  if (Fenetre2==MODIF) { // ----- Deplace Depla[NF]
    Deplace=1;
    Fenetre2=Fenetre1;
    Color=Objet[NumObjet]->Couleur;
  }

  for (j=Fenetre1;j<=Fenetre2;j++) {

    NbPoint=0;

    if (j==0 && Objet[NumObjet]->R[0]==0 \
             && Objet[NumObjet]->R[1]==0)
        NbPoint=data_objet(CARREZ,NumObjet,0);

    if (j==1 && Objet[NumObjet]->R[2]==0 \
             && Objet[NumObjet]->R[1]==0)
        NbPoint=data_objet(CARREX,NumObjet,1);

    if (j==2 && Objet[NumObjet]->R[0]==0 \
             && Objet[NumObjet]->R[2]==0)
        NbPoint=data_objet(CARREY,NumObjet,2);

    if (NbPoint==0) NbPoint=data_objet(CUBE_C,NumObjet,j);

    if (Objet[NumObjet]->Type==MAPPING) NbPoint=data_objet(MAPPING,NumObjet,j);

    if (Autre==PCHAMP && j==0) NbPoint=data_objet(CERCLEX24,NumObjet,0);
    if (Autre==PCHAMP && j==1) NbPoint=data_objet(CERCLEX24,NumObjet,1);
    if (Autre==PCHAMP && j==2) NbPoint=data_objet(CERCLEY24,NumObjet,2);

    if (!Deplace) type_ecriture(1);

    for (i=0;i<=NbPoint;i++) {
      if (j==0) {
        Point[i].X*=Vid[0].Echelle*Objet[NumObjet]->S[0];
        Point[i].Y*=Vid[0].Echelle*Objet[NumObjet]->S[1];
        Point[i].Z*=Vid[0].Echelle*Objet[NumObjet]->S[2];
        rotationX(i,Objet[NumObjet]->R[0]*PIs180);
        rotationY(i,Objet[NumObjet]->R[1]*PIs180);
        rotationZ(i,Objet[NumObjet]->R[2]*PIs180);
        Point[i].X+=Vid[0].Echelle*(Objet[NumObjet]->T[0]+Vid[0].Depla.X);
        Point[i].Y+=Vid[0].Echelle*(Objet[NumObjet]->T[1]+Vid[0].Depla.Y);
       }
      if (j==1) {
        Point[i].X*=Vid[1].Echelle*Objet[NumObjet]->S[0];
        Point[i].Y*=Vid[1].Echelle*Objet[NumObjet]->S[1];
        Point[i].Z*=Vid[1].Echelle*Objet[NumObjet]->S[2];
        rotationX(i,Objet[NumObjet]->R[0]*PIs180);
        rotationY(i,Objet[NumObjet]->R[1]*PIs180);
        rotationZ(i,Objet[NumObjet]->R[2]*PIs180);
        Point[i].Z+=Vid[1].Echelle*(Objet[NumObjet]->T[2]+Vid[1].Depla.X);
        Point[i].Y+=Vid[1].Echelle*(Objet[NumObjet]->T[1]+Vid[1].Depla.Y);
      }
      if (j==2) {
        Point[i].X*=Vid[2].Echelle*Objet[NumObjet]->S[0];
        Point[i].Y*=Vid[2].Echelle*Objet[NumObjet]->S[1];
        Point[i].Z*=Vid[2].Echelle*Objet[NumObjet]->S[2];
        rotationX(i,Objet[NumObjet]->R[0]*PIs180);
        rotationY(i,Objet[NumObjet]->R[1]*PIs180);
        rotationZ(i,Objet[NumObjet]->R[2]*PIs180);
        Point[i].X+=Vid[2].Echelle*(Objet[NumObjet]->T[0]+Vid[2].Depla.X);
        Point[i].Z+=Vid[2].Echelle*(Objet[NumObjet]->T[2]+Vid[2].Depla.Y);
      }
      if (j==3) {
        Point[i].X*=Objet[NumObjet]->S[0];
        Point[i].Y*=Objet[NumObjet]->S[1];
        Point[i].Z*=Objet[NumObjet]->S[2];
        rotationX(i,Objet[NumObjet]->R[0]*PIs180);
        rotationY(i,Objet[NumObjet]->R[1]*PIs180);
        rotationZ(i,Objet[NumObjet]->R[2]*PIs180);
        Point[i].X+=Objet[NumObjet]->T[0]-Camera[NumCam].OX;
        Point[i].Y+=Objet[NumObjet]->T[1]-Camera[NumCam].OY;
        Point[i].Z+=Objet[NumObjet]->T[2]-Camera[NumCam].OZ;

        rotationY(i,Longitude);
        rotationX(i,Latitude);

        Point[i].X=Vid[3].WXs2+(Focale*Point[i].X)/(Point[i].Z);
        Point[i].Y=Vid[3].WYs2+(Focale*Point[i].Y)/(Point[i].Z);
      }
    }

    select_vue(j,CLIP_ON);

    for (i=0;i<=NbPoint;i++) {
      if (Point[i].V) {
        if (j==0) move_to(Vid[0].WXs2+Point[i].X,Vid[0].WYs2+Point[i].Y);
        if (j==1) move_to(Vid[1].WXs2+Point[i].Z,Vid[1].WYs2+Point[i].Y);
        if (j==2) move_to(Vid[2].WXs2+Point[i].X,Vid[2].WYs2+Point[i].Z);
        if (j==3) move_to(Point[i].X,Point[i].Y);
      } else {
        if (j==0) g_ligne_to(Vid[0].WXs2+Point[i].X,Vid[0].WYs2+Point[i].Y,Color);
        if (j==1) g_ligne_to(Vid[1].WXs2+Point[i].Z,Vid[1].WYs2+Point[i].Y,Color);
        if (j==2) g_ligne_to(Vid[2].WXs2+Point[i].X,Vid[2].WYs2+Point[i].Z,Color);
        if (j==3) g_ligne_to(Point[i].X,Point[i].Y,Color);
      }
    }
  }

  type_ecriture(0);
}

// -------------------------------------------------------------------------
// -- TRACE LES AXES DE LA SCENES ------------------------------------------
// -------------------------------------------------------------------------
void trace_axes(byte Vue) {
  if (OptAxe==0) return;
  Objet[0]->Type=AXES;
  modif_objet(0,100,100,100,SCALE);
  modif_objet(0,0,0,0,ROTATE);
  modif_objet(0,0,0,0,TRANSLATE);
  Objet[0]->Selection=0;
  Objet[0]->Couleur=CAXE;
  Objet[0]->Cache=0;
  Objet[0]->Ignore=0;
  Objet[0]->Freeze=0;
  Objet[0]->Rapide=0;
  Objet[0]->Operator=PAS_CSG;
  Objet[0]->CSG=PAS_CSG;

  switch (Vue) {
	case 0: trace_volume_0(0); break;
	case 1: trace_volume_1(0); break;
	case 2: trace_volume_2(0); break;
  }
}

// -----------------------------------------------------------------------
// --------- DEFINI LA GRILLE EN CALCUL DIRECT ---------------------------
// -----------------------------------------------------------------------
void trace_grille(byte Vue) {
  register DBL i,j;
  register int Nb=0;

  if (OptGrille==0) return;

  Objet[0]->Type=GRILLE;
  modif_objet(0,1,1,1,SCALE);
  modif_objet(0,0,0,0,ROTATE);
  modif_objet(0,0,0,0,TRANSLATE);
  Objet[0]->Couleur=CGRILLE;
  Objet[0]->Cache=0;
  Objet[0]->Ignore=0;
  Objet[0]->Selection=0;
  Objet[0]->Freeze=0;
  Objet[0]->Rapide=0;
  Objet[0]->Operator=PAS_CSG;
  Objet[0]->CSG=PAS_CSG;

  if (GrilleType==0) {
    for (i=-12;i<=12;i++) {
      for (j=-12;j<=12;j++) {
        if (Vue==0) { charge_xyz(Nb,i,j,0,1,-1); charge_xyz(Nb+1,i,j,0,0,-1); }
        if (Vue==1) { charge_xyz(Nb,0,i,j,1,-1); charge_xyz(Nb+1,0,i,j,0,-1); }
        if (Vue==2) { charge_xyz(Nb,i,0,j,1,-1); charge_xyz(Nb+1,i,0,j,0,-1); }
        Nb+=2;
      }
    }
  } else {
    for (i=-100;i<=100;i+=1) {
      if (Vue==0) {
        charge_xyz(Nb,i,100,0,1,-1); charge_xyz(Nb+1,i,-100,0,0,-1);
        charge_xyz(Nb+2,100,i,0,1,-1); charge_xyz(Nb+3,-100,i,0,0,-1);
      }
      if (Vue==1) {
        charge_xyz(Nb  ,0,100,i,1,-1); charge_xyz(Nb+1,0,-100,i,0,-1);
        charge_xyz(Nb+2,0,i,100,1,-1); charge_xyz(Nb+3,0,i,-100,0,-1);
      }
      if (Vue==2) {
        charge_xyz(Nb  ,i,0,100,1,-1); charge_xyz(Nb+1,i,0,-100,0,-1);
        charge_xyz(Nb+2,100,0,i,1,-1); charge_xyz(Nb+3,-100,0,i,0,-1);
      }
      Nb+=4;
    }
  }

  //type_ecriture(XOR_PUT);

  switch (Vue) {
	case 0: trace_volume_0(0); break;
	case 1: trace_volume_1(0); break;
	case 2: trace_volume_2(0); break;
  }

  type_ecriture(COPY_PUT);
}

// -----------------------------------------------------------------------
// ----- RETOURNE LA COULEUR PAR DEFAUT D'UN OBJET (POUR CREATION) -------
// -----------------------------------------------------------------------
byte type_couleur(byte Type) {
  switch (Type) {
    case CAMERA:
      return CCAMERA;
    case SPOT:
      return JAUNE;
    case CYLLIGHT:
      return JAUNE;
    case AREA:
      return JAUNE;
    case OMNI:
      return JAUNE;
    case MODIF:
      return JAUNE;
    case OBJET:
      return COBJET;
  }
  return COBJET;
}

// ----------------------------------------------------------------------------
// -- RECHERCHE UNE FENETRE ---------------------------------------------------
// ----------------------------------------------------------------------------
byte cherche_fenetre(void) {
  register byte CSouris=CURSEUR,Valeur=VRAI;

  GMouseOn();

  while (MouseB()==0) {
    if ((kbhit()) && getch()==9) { Sens_Souris(); CSouris=CURSEUR; }
    chercher_menu(CSouris);
  }
  if (MouseB()==1 && chercher_menu(CSouris)==VRAI) return FAUX;
  if (sequence_sortie()) {
    Valeur=FAUX;
    forme_mouse(MS_FLECHE);
  } else {
    NF=trouve_fenetre(0);
  }
  return Valeur;
}

// -------------------------------------------------------------------------
// -- SELECTIONNE UNE FENETRE ----------------------------------------------
// -------------------------------------------------------------------------
byte trouve_fenetre(byte ForceCadre) {
  byte Fenetre=5;
  int X=gmx_v();
  int Y=gmy_v();

  if (Fx4==0) return NF;

  if (ForceCadre==0) {
    if (X>Vid[0].XF && X<Vid[0].XF+Vid[0].WX && Y>Vid[0].YF && Y<Vid[0].YF+Vid[0].WY) Fenetre=0;
    if (X>Vid[1].XF && X<Vid[1].XF+Vid[1].WX && Y>Vid[1].YF && Y<Vid[1].YF+Vid[1].WY) Fenetre=1;
    if (X>Vid[2].XF && X<Vid[2].XF+Vid[2].WX && Y>Vid[2].YF && Y<Vid[2].YF+Vid[2].WY) Fenetre=2;
    if (X>Vid[3].XF && X<Vid[3].XF+Vid[3].WX && Y>Vid[3].YF && Y<Vid[3].YF+Vid[3].WY) Fenetre=3;
    if (NF==Fenetre || Fenetre==5) { GMouseOn(); return NF; }
  }

  GMouseOff();

  if (ForceCadre==2 || ForceCadre==0) {
    g_rectangle(Vid[NF].XF-2,Vid[NF].YF-2,Vid[NF].XF+Vid[NF].WX+1,Vid[NF].YF+Vid[NF].WY+1,FOND,0);
    if (ForceCadre==0) NF=Fenetre;
  }

  if (ForceCadre!=2) {
    g_rectangle(Vid[NF].XF-2,Vid[NF].YF-2,Vid[NF].XF+Vid[NF].WX+1,Vid[NF].YF+Vid[NF].WY+1,JAUNE,0);
  }

  GMouseOn();
  return NF;
}

// ----------------------------------------------------------------------------
// -- SELECTIONNE LE TYPE D'AFFICHAGE DES OBJETS ------------------------------
// -- 0 = normal
// -- 1 = rapide
// -- 2 = cube
// ----------------------------------------------------------------------------
void affiche_objet(byte Type,int D,int F) {
  register int i,N,j;
  int C;

  if (pas_objet(1)) return;

  if (D!=F) {
    for (i=D;i<=F;i++) {
      if (Objet[i]->Cache==0) Objet[i]->Rapide=Type;
    }
    return;
  }

  LABEL_TYPE_AFFICHAGE:
  forme_mouse(MS_SELECTEUR);

  if (OkSelect==0) {
    message("Click on object to modify display type");
  }

  if (Selection && OkSelect==1) {
    if (cherche_fenetre()==FAUX) return;
  } else {
    if ((N=trouve_volume(0,3,1))==FAUX) return;
  }

  forme_mouse(MS_FLECHE);

  if (Selection && OkSelect==1) {
    for (j=1;j<=NbObjet;j++) {
      if (Objet[j]->Selection && Objet[j]->Cache==0) {
        Objet[j]->Selection=0;
        C=Objet[j]->Couleur;
        Objet[j]->Couleur=FFOND;
        trace_volume_all(j,j);
        Objet[j]->Rapide=Type;
        Objet[j]->Couleur=C;
        Objet[j]->Selection=1;
        trace_volume_all(j,j);
      }
    }
  } else {
    C=Objet[N]->Couleur;
    Objet[N]->Couleur=FFOND;
    trace_volume_all(N,N);
    Objet[N]->Rapide=Type;
    Objet[N]->Couleur=C;
    trace_volume_all(N,N);
  }

  GMouseOn();
  goto LABEL_TYPE_AFFICHAGE;
}

// -------------------------------------------------------------------------
// -- AFFICHE UN RECTANGLE ZOOM -/+ DANS FENETRE ---------------------------
// -------------------------------------------------------------------------
void zoom_rect(byte NF,byte PM) {
  register int i,j,D,F;
  register int X1,Y1,X2,Y2;
  register DBL k;

  if (NF==3) return;

  X1=Vid[NF].WXs2;
  Y1=Vid[NF].WYs2;
  X2=Vid[NF].WXs2;
  Y2=Vid[NF].WYs2;

  D=(PM=='+' ? 20:Vid[NF].WXs2);
  F=(PM=='+' ? Vid[NF].WXs2:20);
  j=(int) (Fx4 ? 4:10)*(PM=='+' ? 1:-1);
  k=(DBL)Vid[NF].WY/Vid[NF].WX;

  GMouseOff();
  select_vue(NF,CLIP_ON);
  type_ecriture(XOR_PUT);

  if (PM=='+') {
    for (i=D;i<=F;i+=j) {
      g_rectangle(X1-i,Y1-i*k,X2+i,Y2+i*k,FFOND | BLANC,0);
      g_rectangle(X1-i-1,Y1-i*k-1,X2+i+1,Y2+i*k+1,FFOND | BLANC,0);
      delay(0.5);
      g_rectangle(X1-i,Y1-i*k,X2+i,Y2+i*k,FFOND | BLANC,0);
      g_rectangle(X1-i-1,Y1-i*k-1,X2+i+1,Y2+i*k+1,FFOND | BLANC,0);
    }
  } else {
    for (i=D;i>=F;i+=j) {
      g_rectangle(X1-i,Y1-i*k,X2+i,Y2+i*k,FFOND | BLANC,0);
      g_rectangle(X1-i-1,Y1-i*k-1,X2+i+1,Y2+i*k+1,FFOND | BLANC,0);
      delay(0.5);
      g_rectangle(X1-i,Y1-i*k,X2+i,Y2+i*k,FFOND | BLANC,0);
      g_rectangle(X1-i-1,Y1-i*k-1,X2+i+1,Y2+i*k+1,FFOND | BLANC,0);
    }                  
  }

  type_ecriture(COPY_PUT);
}

// -------------------------------------------------------------------------
// -- AFFICHE UN RECTANGLE ZOOM AVANT AFFICHAGE D'UNE FENETRE --------------
// -------------------------------------------------------------------------
void zoom_rect_fenetre(int X1,int Y1,int X2,int Y2) {
  register int i,j;
  int A,B;
  register DBL X,Y;

  A=(X2-X1)/2+X1;
  B=(Y2-Y1)/2+Y1;

  GMouseOff();
  select_vue(5,CLIP_ON);
  type_ecriture(XOR_PUT);

  for (i=20;i<=95;i+=4) {
    X=(DBL) ((X2-X1)*i)/200;
    Y=(DBL) ((Y2-Y1)*i)/200;
    for (j=0;j<=2;j++) {
      g_rectangle(A-X-j,B-Y-j,A+X+j,B+Y+j,BLANC,0);
    }
    delay(0.3);
    for (j=0;j<=2;j++) {
      g_rectangle(A-X-j,B-Y-j,A+X+j,B+Y+j,BLANC,0);
    }
  }

  type_ecriture(COPY_PUT);
}

// -----------------------------------------------------------------------
// -- SELECTIONNE UNE VUE ------------------------------------------------
// -----------------------------------------------------------------------
void select_vue(byte Vue,byte Clip) {
  #if WINDOWS
  #else
  GMouseOff();
  Clip=Clip;
  
  if (Vue==5) {
    _setwindow(0,0,0,XMax,YMax);
    _setviewport(0,0,XMax,YMax);
    return;
  }

  _setwindow(0,0,0,Vid[Vue].WX,Vid[Vue].WY);
  _setviewport(Vid[Vue].XF,Vid[Vue].YF,Vid[Vue].XF+Vid[Vue].WX-2,Vid[Vue].YF+Vid[Vue].WY-2);

  DrawNF=Vue;

  #endif
}

// -----------------------------------------------------------------------
// -- OPTIONS DANS LES FENETRES AVEC BOUTON DROIT SOURIS -----------------
// -----------------------------------------------------------------------
void options_fenetres(void) {
  #define NBB 8
  int Option,F=0,L=100,H=NBB*20+28;
  register int X=gmx_v(),X1,Y1;
  register int Y=gmy_v(),X2,Y2;
  int i;
  struct Bouton Bt_Sv[NBB];

  trouve_fenetre(0);
  while (MouseB());

  F=OptAide=0;
  OptAide=0;

  X1=X-L/2;
  Y1=Y-H/2;

  if (X1+L>XMax-10) X1=XMax-L-10;
  if (Y1+H>YMax-10) Y1=YMax-H-10;
  if (X1-L/2<5) X1=10;
  if (Y1-H/2<5) Y1=10;
  X2=X1+L;
  Y2=Y1+H;

  message("Choose a function to apply");

  for (i=30;i<30+NBB;i++) memcpy(&Bt_Sv[i-30],&Bt[i],sizeof(struct Bouton));

  init_bouton(30,X1+5,Y1+3+ 20,L-10,20,"Zoom In",CENTRE,ATTEND,"");
  init_bouton(31,X1+5,Y1+3+ 40,L-10,20,"Zoom Out",CENTRE,ATTEND,"");
  init_bouton(32,X1+5,Y1+3+ 60,L-10,20,"Center",CENTRE,ATTEND,"");
  init_bouton(33,X1+5,Y1+3+ 80,L-10,20,"Pan",CENTRE,ATTEND,"");
  init_bouton(34,X1+5,Y1+3+100,L-10,20,"Reset Views",CENTRE,ATTEND,"");
  init_bouton(35,X1+5,Y1+3+120,L-10,20,"Redraw",CENTRE,ATTEND,"");
  init_bouton(36,X1+5,Y1+3+140,L-10,20,Fx4 ? "Maximize":"Minimize",CENTRE,ATTEND,"");
  init_bouton(37,X1+5,Y1+3+160,L-10,20,Vid[NF].Enable ? "Disable":"Enable",CENTRE,ATTEND,"");

  g_fenetre(X1,Y1,X2,Y2,"FUNCTIONS",AFFICHE);
  for (i=30;i<30+NBB;i++) affiche_bouton(i);

  while (1) {
    if (sequence_sortie()) { i=-1; break; }
    if ((Option=test_bouton(NF==3 ? 35:30,30+NBB))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  OptAide=F;
  for (i=30;i<30+NBB;i++) memcpy(&Bt[i],&Bt_Sv[i]-30,sizeof(struct Bouton));

  switch (Option) {
    case -1: return;
    case 30: plus_moins('+',1); break;
    case 31: plus_moins('-',1); break;
    case 32: bouton_recentre(); break;
    case 33: bouton_deplacement(); break;
    case 34:
      trouve_fenetre(2);
      init_vues(0);
      redessine_fenetre(5,1);
      break;
    case 35: bouton_rafraichit(); break;
    case 36: bouton_fenetre(0); break;
    case 37:
      Vid[NF].Enable=!Vid[NF].Enable;
      redessine_fenetre(NF,1);
      break;
  }
}

// -----------------------------------------------------------------------
// -- RESCALE LES VUES ---------------------------------------------------
// -----------------------------------------------------------------------
void rescale_fenetres(void) {
  register int PasX=0,PasY=0,XA,YA,XB,YB;
  int XMA,XMI,YMA,YMI;
  int HX1,HX2,HY1,HY2,VX1,VX2,VY1,VY2;
  int Ok=1,i;

  HX1=Vid[0].XF;
  HY1=Vid[0].YF+Vid[0].WY+2;
  HX2=Vid[1].XF+Vid[1].WX-1;
  HY2=Vid[0].YF+Vid[0].WY+2;

  VX1=Vid[0].XF+Vid[0].WX+2;
  VY1=Vid[0].YF;
  VX2=Vid[0].XF+Vid[0].WX+2;
  VY2=Vid[2].YF+Vid[2].WY-1;

  XMA=Vid[1].XF+Vid[1].WX-40;
  XMI=Vid[0].XF+40;
  YMA=Vid[2].YF+Vid[2].WY-40;
  YMI=Vid[0].YF+40;

  set_port(Vid[0].XF,Vid[0].YF,Vid[3].XF+Vid[3].WX,Vid[3].YF+Vid[3].WY);

  type_ecriture(XOR_PUT);

  g_ligne(HX1,HY1,HX2,HY2,BLANC);
  g_ligne(VX1,VY1,VX2,VY2,BLANC);

  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();

  delay(100);
  
  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) { Ok=0; break; }
    if (XA!=XB || YA!=YB) {
      g_ligne(HX1,HY1,HX2,HY2,BLANC);
      g_ligne(VX1,VY1,VX2,VY2,BLANC);
      PasX=(XA-XB); VX1+=PasX; VX2+=PasX;
      PasY=(YA-YB); HY1+=PasY; HY2+=PasY;
      if (VX1<=XMI) VX1=VX2=XMI;
      if (VX1>=XMA) VX1=VX2=XMA;
      if (HY1<=YMI) HY1=HY2=YMI;
      if (HY1>=YMA) HY1=HY2=YMA;
      g_ligne(HX1,HY1,HX2,HY2,BLANC);
      g_ligne(VX1,VY1,VX2,VY2,BLANC);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
    }
  }

  g_ligne(HX1,HY1,HX2,HY2,BLANC);
  g_ligne(VX1,VY1,VX2,VY2,BLANC);
  type_ecriture(COPY_PUT);

  if (Ok) {
    select_vue(5,CLIP_OFF);
    GMouseOff();
    trouve_fenetre(2);
    Vid[0].WX=VX1-Vid[0].XF-2-2;
    Vid[0].WY=HY1-Vid[0].YF-2-2;

    Vid[1].WX=Vid[1].XF+Vid[1].WX-VX1-2;
    Vid[1].WY=HY1-Vid[1].YF-2-2;
    Vid[1].XF=VX1+2;

    Vid[2].WX=VX1-Vid[2].XF-2-2;
    Vid[2].WY=Vid[2].WY+Vid[2].YF-HY1-2;
    Vid[2].YF=HY1+2;

    Vid[3].WX=Vid[3].XF+Vid[3].WX-VX1-2;
    Vid[3].WY=Vid[3].WY+Vid[3].YF-HY1-2;
    Vid[3].XF=VX1+2;
    Vid[3].YF=HY1+2;

    for (i=0;i<=3;i++) {
      Vid[i].WXs2=Vid[i].WX/2;
      Vid[i].WYs2=Vid[i].WY/2;
    }
    
    init_vues(1);
    redessine_fenetre(5,1);
  }
}

// -----------------------------------------------------------------------
// -- RETOURNE UN VALEUR POUR LE SNAP ------------------------------------
// -----------------------------------------------------------------------
void valeur_snap(int NF,int N1,int N2,DBL *Pas,DBL *S,DBL Snap) {
  if (Vid[NF].OptSnap) {
    *S+=(N1-N2)/Vid[NF].Echelle;
    if (*S>=*Pas+Snap) {
      *S=*Pas+=Snap;
    } else {
      if (*S<=*Pas-Snap) *S=*Pas-=Snap;
    }
  }
}

// -----------------------------------------------------------------------
// -- RETOURNE UN VALEUR POUR LE SNAP ------------------------------------
// -----------------------------------------------------------------------
void drawing_aids(int NF) {
  int X1=CentX-70;
  int X2=CentX+65;
  int Y1=CentY-50;
  int Y2=CentY+55,i;

  if (!Vid[NF].Enable) return;

  sprintf(StrBoite[0],"DRAWING AIDS V#%d",NF);
  g_fenetre(X1,Y1,X2,Y2,StrBoite[0],AFFICHE);

  sprintf(ZTexte[0].Variable,"%.4g",Vid[NF].SnapX);
  init_texte(0,X1+50,Y1+30,"X snap",ZTexte[0].Variable,8,"");
  place_zone_texte(0);

  sprintf(ZTexte[1].Variable,"%.4g",Vid[NF].SnapY);
  init_texte(1,X1+50,Y1+50,"Y snap",ZTexte[1].Variable,8,"");
  place_zone_texte(1);

  bouton_dialog(X1,X2,Y2,1,0);

  while (1) {
    test_texte(0,1);
    if ((i=bouton_dialog(X1,X2,Y2,0,0))!=-1) break;
  }

  if (i==0) {
    Vid[NF].SnapX=atof(ZTexte[0].Variable);
    Vid[NF].SnapY=atof(ZTexte[1].Variable);
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
}

// -----------------------------------------------------------------------
// -- EFFACE UN OBJET A UNE CERTAINE POSITION OU L'AFFICHE ---------------
// -----------------------------------------------------------------------
void hide_show_objet(int N,byte Travail) {
  byte S,C;

  switch(Travail) {
    case AFFICHE:
      trace_volume_all(N,N);
      break;
    case EFFACE:
      S=Objet[N]->Selection;
      C=Objet[N]->Couleur;
      Objet[N]->Selection=0;
      Objet[N]->Couleur=FFOND;
      trace_volume_all(N,N);
      Objet[N]->Selection=S;
      Objet[N]->Couleur=C;
      break;
  }
}
