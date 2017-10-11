/* ---------------------------------------------------------------------------
*  OBJECT.C
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
#include <GRAPH.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

VOLUME *Objet[NB_OBJET_MAX+1];
struct Noeud Point[NB_TRIANGLE_MAX*9];

// -----------------------------------------------------------------------
// --------- MODIFICATION DES DONNEES D'UN OBJET -------------------------
// -----------------------------------------------------------------------
void modif_objet(int N,DBL X,DBL Y,DBL Z,byte Modif) {
  switch (Modif) {
    case SCALE:     Objet[N]->S[0]=X; Objet[N]->S[1]=Y; Objet[N]->S[2]=Z; break;
    case ROTATE:    Objet[N]->R[0]=X; Objet[N]->R[1]=Y; Objet[N]->R[2]=Z; break;
    case TRANSLATE: Objet[N]->T[0]=X; Objet[N]->T[1]=Y; Objet[N]->T[2]=Z; break;
    case DIVERS:    Objet[N]->P[0]=X; Objet[N]->P[1]=Y; Objet[N]->P[2]=Z; break;
  }
}

// -------------------------------------------------------------------------
// -- RECHERCHE UN VOLUME FENETRE 0 ----------------------------------------
// -- 0 recherche selection unique
// -- 1 recherche centrage objet seulement
// -------------------------------------------------------------------------
byte cherche_volume_0(int N,byte Fonction) {
  register int i,NbPoint;
  register int X,Y;

  if (Objet[N]->Cache) return FAUX;

  if (Fonction==2) {
    NbPoint=data_objet(CUBE_R,N,0);
  } else {
    NbPoint=Objet[N]->Rapide==2 ? data_objet(CUBE_R,N,0):data_objet(Objet[N]->Type,N,0);
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

  //-------------- Recherche pour recentrage automatique

  if (Fonction) {
    // --------------- test Position des objets
    for (i=0;i<=NbPoint;i++) {
      Point[i].X-=Vid[0].WXs2+Vid[0].Echelle*Vid[0].Depla.X;
      Point[i].Y-=Vid[0].WYs2+Vid[0].Echelle*Vid[0].Depla.Y;
      Point[i].X/=Vid[0].Echelle;
      Point[i].Y/=Vid[0].Echelle;
      Vid[0].Max.X=(Point[i].X>Vid[0].Max.X ? Point[i].X:Vid[0].Max.X);
      Vid[0].Min.X=(Point[i].X<Vid[0].Min.X ? Point[i].X:Vid[0].Min.X);
      Vid[0].Max.Y=(Point[i].Y>Vid[0].Max.Y ? Point[i].Y:Vid[0].Max.Y);
      Vid[0].Min.Y=(Point[i].Y<Vid[0].Min.Y ? Point[i].Y:Vid[0].Min.Y);
    }
    return 0;
  }

  for (i=0;i<=NbPoint;i++) {
    if (Point[i].V) {
      X=Point[i].X;
      Y=Point[i].Y;
    } else {
      if (test_ligne(X,Y,Point[i].X,Point[i].Y,gmx_v(),gmy_v())) return VRAI;
      X=Point[i].X;
      Y=Point[i].Y;
    }
  }
  return FAUX;
}

// -------------------------------------------------------------------------
// -- RECHERCHE UN VOLUME FENETRE 1 ----------------------------------------
// -- 0 recherche selection unique
// -- 1 recherche centrage objet seulement
// -------------------------------------------------------------------------
byte cherche_volume_1(int N,byte Fonction) {
  register int i,NbPoint;
  register int X,Y;

  if (Objet[N]->Cache) return FAUX;

  if (Fonction==2) {
    NbPoint=data_objet(CUBE_R,N,1);
  } else {
    NbPoint=Objet[N]->Rapide==2 ? data_objet(CUBE_R,N,1):data_objet(Objet[N]->Type,N,1);
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

  //-------------- Recherche pour recentrage automatique

  if (Fonction) {
    // --------------- test Position des objets
    for (i=0;i<=NbPoint;i++) {
      Point[i].Z-=Vid[1].WXs2+Vid[1].Echelle*Vid[1].Depla.X;
      Point[i].Y-=Vid[1].WYs2+Vid[1].Echelle*Vid[1].Depla.Y;
      Point[i].Z/=Vid[1].Echelle;
      Point[i].Y/=Vid[1].Echelle;
      Vid[1].Max.X=(Point[i].Z>Vid[1].Max.X ? Point[i].Z:Vid[1].Max.X);
      Vid[1].Min.X=(Point[i].Z<Vid[1].Min.X ? Point[i].Z:Vid[1].Min.X);
      Vid[1].Max.Y=(Point[i].Y>Vid[1].Max.Y ? Point[i].Y:Vid[1].Max.Y);
      Vid[1].Min.Y=(Point[i].Y<Vid[1].Min.Y ? Point[i].Y:Vid[1].Min.Y);
    }
    return 0;
  }

  for (i=0;i<=NbPoint;i++) {
    if (Point[i].V) {
      X=Point[i].Z; Y=Point[i].Y;
    } else {
      if (VRAI==test_ligne(X,Y,Point[i].Z,Point[i].Y,gmx_v(),gmy_v()))
        return VRAI;
      X=Point[i].Z; Y=Point[i].Y;
    }
  }
  return FAUX;
}

// -------------------------------------------------------------------------
// -- RECHERCHE UN VOLUME FENETRE 2 ----------------------------------------
// -- 0 recherche selection unique
// -- 1 recherche centrage objet seulement
// -------------------------------------------------------------------------
byte cherche_volume_2(int N,byte Fonction) {
  register int i,NbPoint;
  register int X,Y;

  if (Objet[N]->Cache) return FAUX;

  if (Fonction==2) {
    NbPoint=data_objet(CUBE_R,N,2);
  } else {
    NbPoint=Objet[N]->Rapide==2 ? data_objet(CUBE_R,N,2):data_objet(Objet[N]->Type,N,2);
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

  //-------------- Recherche pour recentrage automatique

  if (Fonction) {
    // --------------- test Position des objets
    for (i=0;i<=NbPoint;i++) {
      Point[i].X-=Vid[2].WXs2+Vid[2].Echelle*Vid[2].Depla.X;
      Point[i].Z-=Vid[2].WYs2+Vid[2].Echelle*Vid[2].Depla.Y;
      Point[i].X/=Vid[2].Echelle;
      Point[i].Z/=Vid[2].Echelle;
      Vid[2].Max.X=(Point[i].X>Vid[2].Max.X ? Point[i].X:Vid[2].Max.X);
      Vid[2].Min.X=(Point[i].X<Vid[2].Min.X ? Point[i].X:Vid[2].Min.X);
      Vid[2].Max.Y=(Point[i].Z>Vid[2].Max.Y ? Point[i].Z:Vid[2].Max.Y);
      Vid[2].Min.Y=(Point[i].Z<Vid[2].Min.Y ? Point[i].Z:Vid[2].Min.Y);
    }
    return 0;
  }

  for (i=0;i<=NbPoint;i++) {
    if (Point[i].V) {
      X=Point[i].X; Y=Point[i].Z;
    } else {
      if (VRAI==test_ligne(X,Y,Point[i].X,Point[i].Z,gmx_v(),gmy_v()))
        return VRAI;
      X=Point[i].X; Y=Point[i].Z;
    }
  }
  return FAUX;
}

// -------------------------------------------------------------------------
// -- RECHERCHE UN VOLUME FENETRE 3 ----------------------------------------
// -- 0 recherche s‚lection uniquement
// -------------------------------------------------------------------------
byte cherche_volume_3(int N,byte Fonction) {
  register int i,NbPoint;
  register int X,Y;

  Fonction=Fonction;

  if (Objet[N]->Cache) return FAUX;

  if (Fonction==2) {
    NbPoint=data_objet(CUBE_R,N,3);
  } else {
    NbPoint=Objet[N]->Rapide==2 ? data_objet(CUBE_R,N,3):data_objet(Objet[N]->Type,N,3);
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
  }

  if (Fonction) {
    // --------------- test Position des objets
    for (i=0;i<=NbPoint;i++) {
      Vid[3].Max.X=(Point[i].X>Vid[3].Max.X ? Point[i].X:Vid[3].Max.X);
      Vid[3].Min.X=(Point[i].X<Vid[3].Min.X ? Point[i].X:Vid[3].Min.X);
      Vid[3].Max.Y=(Point[i].Y>Vid[3].Max.Y ? Point[i].Y:Vid[3].Max.Y);
      Vid[3].Min.Y=(Point[i].Y<Vid[3].Min.Y ? Point[i].Y:Vid[3].Min.Y);
    }
    return 0;
  }

  for (i=0;i<=NbPoint;i++) {
    if (Point[i].V) {
      X=Point[i].X; Y=Point[i].Y;
    } else {
      if (VRAI==test_ligne(X,Y,Point[i].X,Point[i].Y,gmx_v(),gmy_v())) return VRAI;
      X=Point[i].X; Y=Point[i].Y;
    }
  }

  return FAUX;
}

// -----------------------------------------------------------------------
// --------- CREATION D'UN NOUVEL OBJET ----------------------------------
// -----------------------------------------------------------------------
int new_objet(byte Type,byte Affiche) {
  register int j;

  if (incr_NbObjet(0)==0) return (0);
  NumObjet=NbObjet;
  if (alloc_mem_objet(NumObjet)==0) return (0);
  Objet[NumObjet]->Type=Type;
  log_out(0,"Create Object type %d",Type);
  modif_objet(NumObjet,1.0,1.0,1.0,SCALE);
  modif_objet(NumObjet,0.0,0.0,0.0,ROTATE);
  modif_objet(NumObjet,0.0,0.0,0.0,TRANSLATE);
  modif_objet(NumObjet,0.0,0.0,0.0,DIVERS);
  Objet[NumObjet]->Couleur=COBJET;
  Objet[NumObjet]->Selection=0;
  Objet[NumObjet]->Cache=0;
  Objet[NumObjet]->Poly=-1;
  Objet[NumObjet]->Rapide=0;
  Objet[NumObjet]->Freeze=0;
  Objet[NumObjet]->Ignore=0;
  Objet[NumObjet]->Operator=PAS_CSG;
  Objet[NumObjet]->CSG=PAS_CSG;
  Objet[NumObjet]->Inverse=0;
  Objet[NumObjet]->Ombre=1;
  Objet[NumObjet]->ScaleOM=1;
  Objet[NumObjet]->Ior=0.0;       Objet[NumObjet]->BitTexture[ 0]=0;
  Objet[NumObjet]->Refraction=0;  Objet[NumObjet]->BitTexture[ 1]=0;
  Objet[NumObjet]->Reflexion=0;   Objet[NumObjet]->BitTexture[ 2]=0;
  Objet[NumObjet]->Diffusion=0;   Objet[NumObjet]->BitTexture[ 3]=0;
  Objet[NumObjet]->Ambiance=0;    Objet[NumObjet]->BitTexture[ 4]=0;
  Objet[NumObjet]->Crand=0;       Objet[NumObjet]->BitTexture[ 5]=0;
  Objet[NumObjet]->Phong=0;       Objet[NumObjet]->BitTexture[ 6]=0;
  Objet[NumObjet]->PSize=0;       Objet[NumObjet]->BitTexture[ 7]=0;
  Objet[NumObjet]->Caustics=0;    Objet[NumObjet]->BitTexture[ 8]=0;
  Objet[NumObjet]->Fade_D=0;      Objet[NumObjet]->BitTexture[ 9]=0;
  Objet[NumObjet]->Fade_P=0;      Objet[NumObjet]->BitTexture[10]=0;
  Objet[NumObjet]->Rough=0;       Objet[NumObjet]->BitTexture[11]=0;
  Objet[NumObjet]->Brilli=0;      Objet[NumObjet]->BitTexture[12]=0;
  Objet[NumObjet]->Specular=0;    Objet[NumObjet]->BitTexture[13]=0;
  Objet[NumObjet]->Smooth=(Type==HFIELD ? 0:1);
  Objet[NumObjet]->WLevel=0;
  Objet[NumObjet]->Halo=1;
  for (j=0;j<=1;j++) {
    Objet[NumObjet]->Map[j].Name[0]=NULLC;
    Objet[NumObjet]->Map[j].On=0;
    Objet[NumObjet]->Map[j].Type=0;
    Objet[NumObjet]->Map[j].Once=0;
    Objet[NumObjet]->Map[j].Interpolate=0;
    Objet[NumObjet]->Map[j].Amount=0;
    Objet[NumObjet]->Map[j].Alpha=0;
    Objet[NumObjet]->Map[j].Filter=0;
    Objet[NumObjet]->Map[j].Color=0;
  }
  strcpy(Objet[NumObjet]->Matiere,"Default");
  strcpy(Objet[NumObjet]->CheminRaw,"@");
  Objet[NumObjet]->LooksLike.Nb=0;
  Objet[NumObjet]->LooksLike.Light=0;
  Objet[NumObjet]->Special.Root=(VERTEX *) NULL;
  Objet[NumObjet]->Special.Pt=(VERTEX *) NULL;
  Objet[NumObjet]->Special.Nombre=0;
  Objet[NumObjet]->Special.Type=0;

  switch (Objet[NumObjet]->Type) {
    case CYLINDRE: strcpy(Objet[NumObjet]->Nom,"Cyli"); break;
    case SPHERE  : strcpy(Objet[NumObjet]->Nom,"Sphe"); break;
    case CUBE    : strcpy(Objet[NumObjet]->Nom,"Cube"); break;
    case CONE    : strcpy(Objet[NumObjet]->Nom,"Cone"); break;
    case TORE    : strcpy(Objet[NumObjet]->Nom,"Tore"); break;
    case TUBE    : strcpy(Objet[NumObjet]->Nom,"Tube"); break;
    case PLANX   : strcpy(Objet[NumObjet]->Nom,"Plax"); break;
    case PLANY   : strcpy(Objet[NumObjet]->Nom,"Play"); break;
    case PLANZ   : strcpy(Objet[NumObjet]->Nom,"Plaz"); break;
    case TRIANGLE: strcpy(Objet[NumObjet]->Nom,"Tria"); break;
    case ANNEAU  : strcpy(Objet[NumObjet]->Nom,"Ring"); break;
    case DISQUE  : strcpy(Objet[NumObjet]->Nom,"Disk"); break;
    case DSPHERE : strcpy(Objet[NumObjet]->Nom,"DSph"); break;
    case QTORE   : strcpy(Objet[NumObjet]->Nom,"QTor"); break;
    case BLOB    : strcpy(Objet[NumObjet]->Nom,"Blbs"); break;
    case BLOBC   : strcpy(Objet[NumObjet]->Nom,"Blbc"); break;
    case PRISME  : strcpy(Objet[NumObjet]->Nom,"Pris"); break;
    case QTUBE   : strcpy(Objet[NumObjet]->Nom,"QTub"); break;
    case CONET   : strcpy(Objet[NumObjet]->Nom,"ConT"); break;
    case PYRAMIDE: strcpy(Objet[NumObjet]->Nom,"Pyra"); break;
    case HFIELD  : strcpy(Objet[NumObjet]->Nom,"HFld"); break;
    case SPLINE  : strcpy(Objet[NumObjet]->Nom,"Spli"); break;
    case SUPEREL : strcpy(Objet[NumObjet]->Nom,"SupE"); break;
    case LATHE   : strcpy(Objet[NumObjet]->Nom,"Lath"); break;
    case SOR     : strcpy(Objet[NumObjet]->Nom,"Sorv"); break;
    case EXTRUDE : strcpy(Objet[NumObjet]->Nom,"Extr"); break;
    case BEZIER  : strcpy(Objet[NumObjet]->Nom,"Ptch"); break;
  }
  creer_nom_objet(NumObjet);
  Objet[NumObjet]->MS[_X]=1.0;
  Objet[NumObjet]->MS[_Y]=1.0;
  Objet[NumObjet]->MS[_Z]=1.0;
  Objet[NumObjet]->MR[_X]=0.0;
  Objet[NumObjet]->MR[_Y]=0.0;
  Objet[NumObjet]->MR[_Z]=0.0;
  Objet[NumObjet]->MT[_X]=0.0;
  Objet[NumObjet]->MT[_Y]=0.0;
  Objet[NumObjet]->MT[_Z]=0.0;

  if (Type==TORE) if (creation_tore()==0) {
    free_mem_objet(NumObjet); NbObjet--; return 0;
  }
  if (Type==QTORE) if (creation_tore()==0) {
    free_mem_objet(NumObjet); NbObjet--; return 0;
  }
  if (Type==TUBE) if (creation_anneau_tube_disque(TUBE)==0) {
    free_mem_objet(NumObjet); NbObjet--; return 0;
  }
  if (Type==CONET) if (creation_anneau_tube_disque(CONET)==0) {
    free_mem_objet(NumObjet); NbObjet--; return 0;
  }
  if (Type==ANNEAU) if (creation_anneau_tube_disque(ANNEAU)==0) {
    free_mem_objet(NumObjet); NbObjet--; return 0;
  }
  if (Type==QTUBE) if (creation_anneau_tube_disque(QTUBE)==0) {
    free_mem_objet(NumObjet); NbObjet--; return 0;
  }
  if (Type==DISQUE) if (creation_anneau_tube_disque(DISQUE)==0) {
    free_mem_objet(NumObjet); NbObjet--; return 0;
  }
  if (Type==SPLINE) if (creation_spline(NbObjet)==0) {
    free_mem_objet(NumObjet); NbObjet--; return 0;
  }

  if (Type==BLOB || Type==BLOBC) {
    Objet[NumObjet]->P[_X]=ForceEnCours;
    Objet[NumObjet]->P[_Y]=SeuilEnCours;
    Objet[NumObjet]->P[_Z]=(DBL) GroupeEnCours;
  }

  if (Type==DSPHERE) modif_objet(NumObjet,1,0.5,1,SCALE);
  if (Affiche && renomme_objet(0,0)==0) {
    free_mem_objet(NumObjet);
    NbObjet--;
    return (0);
  }
  if (Affiche==1) {
    trace_volume_all(NumObjet,NumObjet);
    affiche_donnees();
  }
  
  return (1);
}

// -------------------------------------------------------------------------
// -- ROTATION 3D SUR X ----------------------------------------------------
// -------------------------------------------------------------------------
void rotationX (int Num,DBL AngleX) {
  register DBL Y1,Z1;

  if (AngleX==0) return;

  Y1=Point[Num].Y*cos(AngleX)+Point[Num].Z*sin(AngleX);
  Z1=Point[Num].Z*cos(AngleX)-Point[Num].Y*sin(AngleX);

  Point[Num].Y=Y1;
  Point[Num].Z=Z1;
}

// -------------------------------------------------------------------------
// -- ROTATION 3D SUR Y ----------------------------------------------------
// -------------------------------------------------------------------------
void rotationY (int Num,DBL AngleY) {
  register DBL X1,Z1;

  if (AngleY==0) return;

  X1=Point[Num].X*cos(AngleY)+Point[Num].Z*sin(AngleY);
  Z1=Point[Num].Z*cos(AngleY)-Point[Num].X*sin(AngleY);

  Point[Num].X=X1;
  Point[Num].Z=Z1;
}

// -------------------------------------------------------------------------
// -- ROTATION 3D SUR Z ----------------------------------------------------
// -------------------------------------------------------------------------
void rotationZ (int Num,DBL AngleZ) {
  register DBL X1,Y1;

  if (AngleZ==0) return;

  Y1=Point[Num].Y*cos(AngleZ)-Point[Num].X*sin(AngleZ);
  X1=Point[Num].X*cos(AngleZ)+Point[Num].Y*sin(AngleZ);

  Point[Num].X=X1;
  Point[Num].Y=Y1;
}

// -------------------------------------------------------------------------
// -- CHARGE LES DONNEES D'UN VOLUME DANS VARIABLES ------------------------
// -------------------------------------------------------------------------
void charge_xyz(int Num,DBL X,DBL Y,DBL Z,byte Val,int Couleur) {
  Point[Num].X=X;
  Point[Num].Y=Y;
  Point[Num].Z=-Z;
  Point[Num].V=Val;
  Point[Num].C=Couleur;
}

// -------------------------------------------------------------------------
// -- TRANSLATION D'UN VOLUME AVEC SOURIS ----------------------------------
// -------------------------------------------------------------------------
void translation(byte Select,int NumObjet) {
  register int X1,Y1,X2,Y2;
  DBL PasX,PasY,PasDebutX,PasDebutY;
  DBL SX,SY;
  int MX,MY,i;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_TRANSLATION:
  forme_mouse(Sens);
  message("Select objet to move (TAB=choose direction)");

  if (!Select && Selection && OkSelect==1) {
	if (cherche_fenetre()==FAUX) return;
    NumObjet=cube_selection();
  } else {
    if (!Select && (NumObjet=trouve_volume(0,2,1))==FAUX) return;
  }

  MX=gmx_r();
  MY=gmy_r();
  forme_mouse(MS_FLECHE);

  if (NF==0) PasDebutX=PasX=SX=Objet[NumObjet]->T[0];
  if (NF==0) PasDebutY=PasY=SY=Objet[NumObjet]->T[1];
  if (NF==1) PasDebutX=PasX=SX=Objet[NumObjet]->T[2];
  if (NF==1) PasDebutY=PasY=SY=Objet[NumObjet]->T[1];
  if (NF==2) PasDebutX=PasX=SX=Objet[NumObjet]->T[0];
  if (NF==2) PasDebutY=PasY=SY=Objet[NumObjet]->T[2];

  select_vue(NF,CLIP_ON);

  forme_mouse(Sens);
  while (MouseB());
  GMouseOff();

  trace_boite(0,3);
  place_mouse(CentX,CentY);
  X2=gmx_r();
  Y2=gmy_r();

  while (MouseB()!=1) {
    X1=gmx_r();
    Y1=gmy_r();
    if (sequence_sortie()) {
      PasX=PasDebutX;
      PasY=PasDebutY;
      place_mouse(MX,MY);
      break;
    }
	if (X1!=X2 || Y1!=Y2) {
	  delay(5);
      trace_boite(0,3);
	  switch (Sens) {
		case MS_X:
          if (Vid[NF].OptSnap) {
            valeur_snap(NF,X1,X2,&PasX,&SX,Vid[NF].SnapX);
          } else {
            PasX+=(X1-X2)/Vid[NF].Echelle;
          }
		  break;
		case MS_Y:
          if (Vid[NF].OptSnap) {
            valeur_snap(NF,Y1,Y2,&PasY,&SY,Vid[NF].SnapY);
          } else {
            PasY+=(Y1-Y2)/Vid[NF].Echelle;
          }
		  break;
		default:
          if (Vid[NF].OptSnap) {
            valeur_snap(NF,X1,X2,&PasX,&SX,Vid[NF].SnapX);
            valeur_snap(NF,Y1,Y2,&PasY,&SY,Vid[NF].SnapY);
          } else {
            PasX+=(X1-X2)/Vid[NF].Echelle;
            PasY+=(Y1-Y2)/Vid[NF].Echelle;
          }
		  break;
	  }
	  if ((kbhit()) && getch()==9) {
        trace_boite(0,3);
		PasX=PasDebutX;
		PasY=PasDebutY;
		Sens_Souris();
        trace_boite(0,3);
	  }
	  if (NF==0) Objet[NumObjet]->T[0]=PasX;
	  if (NF==0) Objet[NumObjet]->T[1]=PasY;
	  if (NF==1) Objet[NumObjet]->T[2]=PasX;
	  if (NF==1) Objet[NumObjet]->T[1]=PasY;
	  if (NF==2) Objet[NumObjet]->T[0]=PasX;
	  if (NF==2) Objet[NumObjet]->T[2]=PasY;
      trace_boite(0,3);
      place_mouse(CentX,CentY);
	  X2=gmx_r();
	  Y2=gmy_r();
      message("\"%s\" [%05d]: X=%+.2f Y=%+.2f",Objet[NumObjet]->Nom,NumObjet,PasX,-PasY);
	}
  }

  trace_boite(0,3);

  if (NF==0) Objet[NumObjet]->T[0]=PasX;
  if (NF==0) Objet[NumObjet]->T[1]=PasY;
  if (NF==1) Objet[NumObjet]->T[2]=PasX;
  if (NF==1) Objet[NumObjet]->T[1]=PasY;
  if (NF==2) Objet[NumObjet]->T[0]=PasX;
  if (NF==2) Objet[NumObjet]->T[2]=PasY;

  if ((PasX!=PasDebutX || PasY!=PasDebutY) && !Select) {
    MX+=((PasX-PasDebutX)*Vid[NF].Echelle);
    MY+=((PasY-PasDebutY)*Vid[NF].Echelle);
    while (MouseB());
    if (Selection && OkSelect==1) {
      if (Selection>NbObjet/2) {
        selection_modif_objet(TRANSLATE);
        redessine_fenetre(5,1);
      } else {
        for (i=1;i<=NbObjet;i++)
          if (Objet[i]->Selection) hide_show_objet(i,EFFACE);
        selection_modif_objet(TRANSLATE);
        for (i=1;i<=NbObjet;i++)
          if (Objet[i]->Selection) hide_show_objet(i,AFFICHE);
      }
    } else {
      if (NF==0) Objet[NumObjet]->T[0]=PasDebutX;
      if (NF==0) Objet[NumObjet]->T[1]=PasDebutY;
      if (NF==1) Objet[NumObjet]->T[2]=PasDebutX;
      if (NF==1) Objet[NumObjet]->T[1]=PasDebutY;
      if (NF==2) Objet[NumObjet]->T[0]=PasDebutX;
      if (NF==2) Objet[NumObjet]->T[2]=PasDebutY;

      hide_show_objet(NumObjet,EFFACE);

      if (NF==0) Objet[NumObjet]->T[0]=PasX;
      if (NF==0) Objet[NumObjet]->T[1]=PasY;
      if (NF==1) Objet[NumObjet]->T[2]=PasX;
      if (NF==1) Objet[NumObjet]->T[1]=PasY;
      if (NF==2) Objet[NumObjet]->T[0]=PasX;
      if (NF==2) Objet[NumObjet]->T[2]=PasY;

      hide_show_objet(NumObjet,AFFICHE);
    }
  }

  place_mouse(MX,MY);
  GMouseOn();
  if (!Select) goto LABEL_TRANSLATION;
}

// -------------------------------------------------------------------------
// -- ROTATION D'UN VOLUME AVEC SOURIS -------------------------------------
// -------------------------------------------------------------------------
void rotation(byte Select,int N) {
  int Choix,i;
  int X1,X2;
  Vecteur Angle,AngleD,V;
  TRANSFORM *Mat;
  Vecteur SC,SH,RO,TR;
  byte Ok;
  int MX,MY;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_ROTATION:
  Ok=1;
  forme_mouse(MS_SELECTEUR);
  message("Select object (steps ALT=5ø/SHIFT=45ø - ENTER=change axis X/Y/Z)");

  if (!Select) {
    if (Selection && OkSelect==1) {
      if (cherche_fenetre()==FAUX) return;
      cube_selection();
    } else {
      if ((N=trouve_volume(0,2,1))==FAUX) {
        return;
      }
    }
  }

  NumObjet=N;

  if (NF==0) Choix=2;
  if (NF==1) Choix=0;
  if (NF==2) Choix=1;

  vect_copy(AngleD,Objet[N]->R);
  vect_init(Angle,0,0,0);

  select_vue(NF,CLIP_ON);

  while (MouseB());
  MX=gmx_r();
  MY=gmy_r();
  GMouseOff();

  trace_boite(0,3);
  place_mouse(CentX,CentY);
  X2=gmx_r();

  while (MouseB()!=1) {
    X1=gmx_r();
    if (sequence_sortie()) { Ok=0; break; }
    if (X1!=X2) {
      delay(5);
      trace_boite(0,3);
      if (status_clavier() & 8) {
        Angle[Choix]+=5*(X1-X2)/fabs(X1-X2);
      } else {
        if (status_clavier() & 3) {
          Angle[Choix]+=45*(X1-X2)/fabs(X1-X2);
        } else {
          Angle[Choix]+=X1-X2;
        }
      }

      if (Angle[Choix]>+180) Angle[Choix]=+180;
      if (Angle[Choix]<-180) Angle[Choix]=-180;

      Mat=Create_Transform();
      rotation_objet(Mat,AngleD,'+');
      vect_init(V,0,0,0);
      V[Choix]=Angle[Choix];
      rotation_objet(Mat,V,'+');
      mat_decode(Mat->matrix,SC,SH,RO,TR);
      Efface_Transform(Mat);
      vect_init(Objet[N]->R,RO[0],RO[1],RO[2]);
      trace_boite(0,3);

      if ((kbhit()) && getch()==13) {
        trace_boite(0,3);
        Angle[Choix]=AngleD[Choix];
        Choix+=(Choix==2 ? -2:1);
        trace_boite(0,3);
      }
      place_mouse(CentX,CentY);
      message("\"%s\" [%05d]: Axis %c: %+.2lfø",Objet[N]->Nom,N,'X'+Choix,Angle[Choix]);
    }
  }

  trace_boite(0,3);

  if (Ok) {
    if (!Select) {
      if (Selection && OkSelect==1) {
        if (Selection>NbObjet/2) {
          selection_modif_objet(ROTATE);
          redessine_fenetre(5,1);
        } else {
          for (i=1;i<=NbObjet;i++)
            if (Objet[i]->Selection) hide_show_objet(i,EFFACE);
          selection_modif_objet(ROTATE);
          for (i=1;i<=NbObjet;i++)
            if (Objet[i]->Selection) hide_show_objet(i,AFFICHE);
        }
      } else {
        vect_copy(Angle,Objet[N]->R);
        vect_copy(Objet[N]->R,AngleD);
        hide_show_objet(NumObjet,EFFACE);
        vect_copy(Objet[N]->R,Angle);
        hide_show_objet(NumObjet,AFFICHE);
      }
    }
  } else {
    vect_copy(Objet[N]->R,AngleD);
  }

  forme_mouse(MS_FLECHE);
  while (MouseB());
  place_mouse(MX,MY);
  GMouseOn();
  if (!Select) goto LABEL_ROTATION;
}

// -------------------------------------------------------------------------
// -- DEFORMATION 2D D'UN VOLUME AVEC SOURIS -------------------------------
// -------------------------------------------------------------------------
void deformation2D(byte Select,int N) {
  register int X1,Y1,X2,Y2;
  Vecteur Deform;
  Vecteur DeformD;
  DBL SF;
  TRANSFORM *Mat;
  Vecteur SC,SH,RO,TR;
  int T,Ok,i;
  int MX,MY;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_DEFORMATION2D:
  SF=1.0;
  Ok=1;
  forme_mouse(Sens);
  message("Select object to 2D scale (TAB=choose direction - ALT=precision x10");

  if (!Select && Selection && OkSelect==1) {
    if (cherche_fenetre()==FAUX) return;
    N=cube_selection();
  } else {
    if (!Select && (N=trouve_volume(0,2,1))==FAUX) return;
  }

  NumObjet=N;
  
  select_vue(NF,CLIP_ON);

  forme_mouse(Sens);
  MX=gmx_r();
  MY=gmy_r();
  while (MouseB());
  GMouseOff();

  vect_copy(DeformD,Objet[N]->S);
  vect_copy(Deform,Objet[N]->S);

  trace_boite(0,3);
  place_mouse(CentX,CentY);
  X2=gmx_r();
  Y2=gmy_r();

  while (MouseB()!=1) {
	X1=gmx_r();
	Y1=gmy_r();
    if (sequence_sortie()) { Ok=0; break; }
	if (X1!=X2 || Y1!=Y2) {
	  delay(5);
      trace_boite(0,3);
      if (status_clavier() & 8) {
        SF+=(((((X1-X2)+(Y1-Y2))*0.5)/Vid[NF].Echelle))/10;
      } else {
        SF+=(((X1-X2)+(Y1-Y2))*0.5)/Vid[NF].Echelle;
      }
	  switch (Sens) {
		case MS_X:
          if (NF==0) vect_init(Deform,SF,1,1);
          if (NF==1) vect_init(Deform,1,1,SF);
          if (NF==2) vect_init(Deform,SF,1,1);
		  break;
		case MS_Y:
          if (NF==0) vect_init(Deform,1,SF,1);
          if (NF==1) vect_init(Deform,1,SF,1);
          if (NF==2) vect_init(Deform,1,1,SF);
		  break;
		case MS_XY:
          if (NF==0) vect_init(Deform,SF,SF,1);
          if (NF==1) vect_init(Deform,1,SF,SF);
          if (NF==2) vect_init(Deform,SF,1,SF);
		  break;
	  }

      Mat=Create_Transform();
      ajustement_objet(Mat,DeformD,'+');
      rotation_objet(Mat,Objet[N]->R,'+');
      Deform[_X]=fabs(Deform[_X]);
      Deform[_Y]=fabs(Deform[_Y]);
      Deform[_Z]=fabs(Deform[_Z]);
      ajustement_objet(Mat,Deform,'+');
      mat_decode(Mat->matrix,SC,SH,RO,TR);
      vect_copy(Objet[N]->S,SC);
      Efface_Transform(Mat);

      trace_boite(0,3);

      if (kbhit()) {
        T=getch();
        if (T==9) {
          trace_boite(0,3);
          vect_copy(Objet[N]->S,DeformD);
          SF=1;
          Sens_Souris();
          trace_boite(0,3);
        }
      }
      
      place_mouse(CentX,CentY);
	  X2=gmx_r();
      Y2=gmy_r();
      message("\"%s\" [%05d]: X=%+.3f Y=%+.3f Z=%+.3f",Objet[N]->Nom,N,Objet[N]->S[0],Objet[N]->S[1],Objet[N]->S[2]);
	}
  }

  trace_boite(0,3);

  if (Ok) {
    if (!Select) {
      if (Selection && OkSelect==1) {
        if (Selection>NbObjet/2) {
          selection_modif_objet(SCALE);
          redessine_fenetre(5,1);
        } else {
          for (i=1;i<=NbObjet;i++)
            if (Objet[i]->Selection) hide_show_objet(i,EFFACE);
          selection_modif_objet(SCALE);
          for (i=1;i<=NbObjet;i++)
            if (Objet[i]->Selection) hide_show_objet(i,AFFICHE);
        }
      } else {
        vect_copy(Deform,Objet[N]->S);
        vect_copy(Objet[N]->S,DeformD);
        hide_show_objet(NumObjet,EFFACE);
        vect_copy(Objet[N]->S,Deform);
        hide_show_objet(NumObjet,AFFICHE);
      }
    }
  } else {
    vect_copy(Objet[N]->S,DeformD);
  }

  while (MouseB());
  place_mouse(MX,MY);
  GMouseOn();
  if (!Select) goto LABEL_DEFORMATION2D;
}

// -------------------------------------------------------------------------
// -- DEFORMATION 3D D'UN VOLUME AVEC SOURIS -------------------------------
// -------------------------------------------------------------------------
void deformation3D(byte Select,int N) {
  register int X1,Y1,X2,Y2;
  Vecteur Deform;
  Vecteur DeformD;
  DBL SF;
  TRANSFORM *Mat;
  Vecteur SC,SH,RO,TR;
  byte Ok;
  int MX,MY,i;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_DEFORMATION3D:
  SF=1.0;
  Ok=1;
  forme_mouse(MS_SELECTEUR);
  message("Select object to 3D scale - ALT=precision x10");

  if (!Select && Selection && OkSelect==1) {
    if (cherche_fenetre()==FAUX) return;
    N=cube_selection();
  } else {
    if (!Select && (N=trouve_volume(0,3,1))==FAUX) return;
  }

  NumObjet=N;

  forme_mouse(MS_FLECHE);

  vect_copy(DeformD,Objet[N]->S);
  vect_copy(Deform,Objet[N]->S);

  select_vue(NF,CLIP_ON);
  while (MouseB());
  MX=gmx_r();
  MY=gmy_r();
  GMouseOff();

  trace_boite(0,3);
  place_mouse(CentX,CentY);
  X2=gmx_r();
  Y2=gmy_r();

  while (MouseB()!=1) {
	X1=gmx_r();
	Y1=gmy_r();
    if (sequence_sortie()) { Ok=0; break; }
	if (X1!=X2 || Y1!=Y2) {
	  delay(5);
      trace_boite(0,3);
      if (status_clavier() & 8) {
        SF+=(((((X1-X2)+(Y1-Y2))*0.5)/Vid[NF].Echelle))/10;
      } else {
        SF+=(((X1-X2)+(Y1-Y2))*0.5)/Vid[NF].Echelle;
      }
      vect_scale(Deform,DeformD,fabs(SF));

      Mat=Create_Transform();
      ajustement_objet(Mat,Deform,'+');
      rotation_objet(Mat,Objet[N]->R,'+');
      mat_decode(Mat->matrix,SC,SH,RO,TR);
      vect_copy(Objet[N]->S,SC);
      Efface_Transform(Mat);

      trace_boite(0,3);
      place_mouse(CentX,CentY);
	  X2=gmx_r();
	  Y2=gmy_r();
      message("\"%s\" [%05d]: X-Y-Z=%.2lf%",Objet[N]->Nom,N,fabs(-100+(Deform[0]/DeformD[0])*100));
	}
  }

  trace_boite(0,3);

  if (Objet[N]->Type==PLANX) Objet[N]->S[0]=0.001;
  if (Objet[N]->Type==PLANY) Objet[N]->S[1]=0.001;
  if (Objet[N]->Type==PLANZ) Objet[N]->S[2]=0.001;

  if (Ok) {
    if (!Select) {
      if (Selection && OkSelect==1) {
        if (Selection>NbObjet/2) {
          selection_modif_objet(SCALE);
          redessine_fenetre(5,1);
        } else {
          for (i=1;i<=NbObjet;i++)
            if (Objet[i]->Selection) hide_show_objet(i,EFFACE);
          selection_modif_objet(SCALE);
          for (i=1;i<=NbObjet;i++)
            if (Objet[i]->Selection) hide_show_objet(i,AFFICHE);
        }
      } else {
        vect_copy(Deform,Objet[N]->S);
        vect_copy(Objet[N]->S,DeformD);
        hide_show_objet(NumObjet,EFFACE);
        vect_copy(Objet[N]->S,Deform);
        hide_show_objet(NumObjet,AFFICHE);
      }
    }
  } else {
    vect_copy(Objet[N]->S,DeformD);
  }

  while (MouseB());
  place_mouse(MX,MY);
  GMouseOn();
  if (!Select) goto LABEL_DEFORMATION3D;
}

// -------------------------------------------------------------------------
// -- COPIE UN OBJET -------------------------------------------------------
// -------------------------------------------------------------------------
void copie_objet(void) {
  int X1,Y1,X2,Y2;
  DBL PasX,PasY,PasDebutX,PasDebutY;
  DBL TX,TY,TZ;
  int Debut,Fin,i;
  byte Ok;
  byte NA=0;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_COPIE:
  forme_mouse(Sens);
  message("Select object to copy (TAB=choose direction)");

  if (Selection && OkSelect==1) {
	if (cherche_fenetre()==FAUX) return;
    NumObjet=cube_selection();
  } else {
    if ((NumObjet=trouve_volume(0,2,1))==FAUX) return;
  }

  forme_mouse(MS_FLECHE);

  if (NF==0) PasDebutX=PasX=Objet[NumObjet]->T[0];
  if (NF==0) PasDebutY=PasY=Objet[NumObjet]->T[1];
  if (NF==1) PasDebutX=PasX=Objet[NumObjet]->T[2];
  if (NF==1) PasDebutY=PasY=Objet[NumObjet]->T[1];
  if (NF==2) PasDebutX=PasX=Objet[NumObjet]->T[0];
  if (NF==2) PasDebutY=PasY=Objet[NumObjet]->T[2];

  select_vue(NF,CLIP_ON);

  forme_mouse(Sens);
  while (MouseB());
  GMouseOff();

  trace_boite(0,3);
  place_mouse(CentX,CentY);
  X2=gmx_r();
  Y2=gmy_r();

  while (VRAI==VRAI) {
    X1=gmx_r();
	Y1=gmy_r();
    if (sequence_sortie()) { Ok=0; PasX=PasDebutX; PasY=PasDebutY; break; }
	if (MouseB()==1) { Ok=1; break; }
	if (X1!=X2 || Y1!=Y2) {
      delay(5);
      trace_boite(0,3);
	  switch (Sens) {
		case MS_X:
		  PasX+=(X1-X2)/Vid[NF].Echelle;
		  break;
		case MS_Y:
		  PasY+=(Y1-Y2)/Vid[NF].Echelle;
		  break;
		default:
		  PasX+=(X1-X2)/Vid[NF].Echelle;
		  PasY+=(Y1-Y2)/Vid[NF].Echelle;
		  break;
	  }
	  if ((kbhit()) && getch()==9) {
        trace_boite(0,3);
		PasX=PasDebutX;
		PasY=PasDebutY;
		Sens_Souris();
        trace_boite(0,3);
	  }
	  if (NF==0) Objet[NumObjet]->T[0]=PasX;
	  if (NF==0) Objet[NumObjet]->T[1]=PasY;
	  if (NF==1) Objet[NumObjet]->T[2]=PasX;
	  if (NF==1) Objet[NumObjet]->T[1]=PasY;
	  if (NF==2) Objet[NumObjet]->T[0]=PasX;
	  if (NF==2) Objet[NumObjet]->T[2]=PasY;
      trace_boite(0,3);
      place_mouse(CentX,CentY);
	  X2=gmx_r();
	  Y2=gmy_r();
      message("\"%s\" [%05d]: X=%+.2lf Y=%+.2lf",Objet[NumObjet]->Nom,NumObjet,PasX,-PasY);
	}
  }

  trace_boite(0,3);

  if (NF==0) Objet[NumObjet]->T[_X]=PasX;
  if (NF==0) Objet[NumObjet]->T[_Y]=PasY;
  if (NF==1) Objet[NumObjet]->T[_Z]=PasX;
  if (NF==1) Objet[NumObjet]->T[_Y]=PasY;
  if (NF==2) Objet[NumObjet]->T[_X]=PasX;
  if (NF==2) Objet[NumObjet]->T[_Z]=PasY;

  if (Ok==1) {
	if (Selection && OkSelect==1) {
	  Debut=1; Fin=NbObjet;
	  TX=Objet[0]->T[0]-((Vid[0].Max.X+Vid[0].Min.X)*0.5);
	  TY=Objet[0]->T[1]-((Vid[0].Max.Y+Vid[0].Min.Y)*0.5);
      TZ=Objet[0]->T[2]-((Vid[1].Max.X+Vid[1].Min.X)*0.5);
	} else {
	  Debut=Fin=NumObjet;
	  TX=TY=TZ=0;
	}
    if ((Debut-Fin)) NA=nom_objet_auto();
	for (i=Debut;i<=Fin;i++) {
      if (((Objet[i]->Selection && OkSelect) || i==NumObjet) && !Objet[i]->Cache) {
        auto_copy_objet(i,NA);
        Objet[NbObjet]->T[0]+=TX;
        Objet[NbObjet]->T[1]+=TY;
        Objet[NbObjet]->T[2]+=TZ;
        trace_volume_all(NbObjet,NbObjet);
      }
    }
  }

  if (NF==0) Objet[NumObjet]->T[_X]=PasDebutX;
  if (NF==0) Objet[NumObjet]->T[_Y]=PasDebutY;
  if (NF==1) Objet[NumObjet]->T[_Z]=PasDebutX;
  if (NF==1) Objet[NumObjet]->T[_Y]=PasDebutY;
  if (NF==2) Objet[NumObjet]->T[_X]=PasDebutX;
  if (NF==2) Objet[NumObjet]->T[_Z]=PasDebutY;

  affiche_donnees();
  GMouseOn();
  goto LABEL_COPIE;
}

// -------------------------------------------------------------------------
// -- SUPPRIME UN OBJET ----------------------------------------------------
// -------------------------------------------------------------------------
void supprime_objet(void) {
  register int j,Cache=1;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_SUPPRIME:
  forme_mouse(MS_SELECTEUR);
  message("Select object to delete");

  if (Selection && OkSelect==1) {
	if (cherche_fenetre()==FAUX) return;
    NumObjet=cube_selection();
  } else {
    if ((NumObjet=trouve_volume(0,3,1))==FAUX) return;
  }

  strcpy(StrBoite[0],"DELETE AN OBJECT");

  if (Selection && OkSelect==1) {
    sprintf(StrBoite[1],"Do you really want to delete");
    sprintf(StrBoite[2],"the current selection ?");
  } else {
    sprintf(StrBoite[1],"Do you really want to delete");
    sprintf(StrBoite[2],"object nø%d \"%s\" ?",NumObjet,Objet[NumObjet]->Nom);
  }

  forme_mouse(MS_FLECHE);
  if (g_boite_ONA(CentX,CentY,2,CENTRE,1)==0) {
	if (Selection && OkSelect==1) {
      while (Selection && Cache) {
        Cache=0;
		for (j=1;j<=NbObjet;j++) {
          if (Objet[j]->Selection && !Objet[j]->Cache) {
            efface_objet(j,1);
            Cache++;
          }
		}
	  }
	} else {
      efface_objet(NumObjet,1);
	}
  }

  GMouseOn();
  goto LABEL_SUPPRIME;
}

// -------------------------------------------------------------------------
// -- EFFACE UN OBJET DE LA SCENE ------------------------------------------
// -------------------------------------------------------------------------
void efface_objet(int N,byte Affiche) {
  register int i;

  if (NbObjet==1) {
    if (Objet[N]->Selection) Selection--;
    Objet[N]->Selection=0;
    Objet[N]->Couleur=FFOND;

    if (Affiche) trace_volume_all(N,N);

    if (Objet[N]->Poly>=0) {
      switch (Objet[N]->Type) {
        case SPLINE:
          free_mem_spline_objet(Objet[N]->Poly);
          break;
        default:
          free_mem_poly_objet(Objet[N]->Poly);
          break;
      }
    }

    free_mem_objet(NbObjet);
    NbObjet--;
  } else {
    if (Objet[N]->Selection) Selection--;
    Objet[N]->Selection=0;
    Objet[N]->Couleur=FFOND;
    analyse_delete_csg_objet(N);

    if (Affiche) trace_volume_all(N,N);

    if (Objet[N]->Poly>=0) {
      switch (Objet[N]->Type) {
        case SPLINE:
          free_mem_spline_objet(Objet[N]->Poly);
          break;
        default:
          free_mem_poly_objet(Objet[N]->Poly);
          break;
      }
    }

    for (i=N;i<NbObjet;i++) {
      *Objet[i]=*Objet[i+1];
    }

    if (Objet[NbObjet]->Special.Root!=NULL) {
      Objet[NbObjet]->Special.Root=NULL;
    }

    free_mem_objet(NbObjet);
    NbObjet--;
  }

  if (Affiche) affiche_donnees();
}

/* ------------------------------------------------------------------------- */
/* -- AUGMENTE LE NOMBRE D'OBJET ------------------------------------------- */
/* ------------------------------------------------------------------------- */
byte incr_NbObjet(byte Val) {
  if (NbObjet<NB_OBJET_MAX) { NbObjet++; return 1; }
  if (Val) {
	forme_mouse(MS_FLECHE);
    sprintf(StrBoite[0],"%s %s",NomLogiciel,VerLogiciel);
    strcpy(StrBoite[1],"The number of objects is limited");
    sprintf(StrBoite[2],"in shareware version to : %d",NB_OBJET_MAX);
    strcpy(StrBoite[3],"");
    g_boite_ONA(CentX,CentY,3,CENTRE,0);
  }
  NbObjet=NB_OBJET_MAX;
  return 0;
}

/* ------------------------------------------------------------------------- */
/* -- REINITIALISATION UN OBJET -------------------------------------------- */
/* ------------------------------------------------------------------------- */
void reinitialisation_objet(void) {
  register int j;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_REINIT:
  forme_mouse(MS_SELECTEUR);
  message("Select object to reinit");

  if (Selection && OkSelect==1) {
	if (cherche_fenetre()==FAUX) return;
	cube_selection();
  } else {
    if ((NumObjet=trouve_volume(0,3,1))==FAUX) return;
  }

  forme_mouse(MS_FLECHE);
  strcpy(StrBoite[0],"REINIT OBJECT");
  strcpy(StrBoite[1],"Do you really want to continue ?");

  if (g_boite_ONA(CentX,CentY,1,CENTRE,1)==0) {
	if (Selection && OkSelect==1) {
	  while (Selection) {
		for (j=1;j<=NbObjet;j++) {
		  if (Objet[j]->Selection && !Objet[j]->Cache) {
            modif_objet(j,1,1,1,SCALE);
			modif_objet(j,0,0,0,ROTATE);
			Objet[j]->Selection=0;
			Objet[j]->Couleur=COBJET;
			Selection--;
		  }
		}
	  }
	} else {
      modif_objet(NumObjet,1,1,1,SCALE);
	  modif_objet(NumObjet,0,0,0,ROTATE);
	  Objet[NumObjet]->Selection=0;
	  Objet[NumObjet]->Couleur=COBJET;
	  Selection--;
	}
	redessine_fenetre(5,1);
  }

  GMouseOn();
  goto LABEL_REINIT;
}

/* ------------------------------------------------------------------------- */
/* -- RECENTRAGE ----------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
void recentre_objet(void) {
  register int j;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_RECENTRE:
  forme_mouse(MS_SELECTEUR);
  message("Select object to re-centering");

  if (Selection && OkSelect==1) {
	if (cherche_fenetre()==FAUX) return;
	cube_selection();
  } else {
    if ((NumObjet=trouve_volume(0,3,1))==FAUX) return;
  }

  forme_mouse(MS_FLECHE);
  strcpy(StrBoite[0],"RECENTERING AN OBJECT");
  strcpy(StrBoite[1],"Do you really want to");
  strcpy(StrBoite[2],"re-centering the object at <0,0,0> ?");

  if (g_boite_ONA(CentX,CentY,2,CENTRE,1)==0) {
	if (Selection && OkSelect==1) {
	  for (j=1;j<=NbObjet;j++) {
		if (Objet[j]->Selection && !Objet[j]->Cache) {
		  modif_objet(j,0,0,0,TRANSLATE);
		}
	  }
	} else {
	  modif_objet(NumObjet,0,0,0,TRANSLATE);
	}
	redessine_fenetre(5,1);
  }

  GMouseOn();
  goto LABEL_RECENTRE;
}

// -------------------------------------------------------------------------
// -- ALLOCATION DE LA MEMOIRE POUR L'OBJET --------------------------------
// -------------------------------------------------------------------------
byte alloc_mem_objet(int Num) {
  Objet[Num]=(VOLUME *) mem_alloc(sizeof(VOLUME));
  if (Objet[Num]==NULL) {
    forme_mouse(MS_FLECHE);
    strcpy(StrBoite[0],"ALLOCATE MEMORY FAILED");
    strcpy(StrBoite[1],"There's no more memory left");
    strcpy(StrBoite[2],"for new objects. Try to configure more");
    strcpy(StrBoite[3],"virtual memory to get more free space.");
    g_boite_ONA(CentX,CentY,3,CENTRE,0);
    NbObjet--;
    return 0;
  }
  Objet[Num]->Special.Root=(VERTEX *) NULL;
  return 1;
}

// -------------------------------------------------------------------------
// -- DE-ALLOCATION DE LA MEMOIRE POUR L'OBJET -----------------------------
// -------------------------------------------------------------------------
byte free_mem_objet(int N) {
  free_mem_special(N);
  mem_free(Objet[N],sizeof(VOLUME));
  return 0;
}

// -------------------------------------------------------------------------
// -- MODIFIE LA COULEUR D'UN OBJET ----------------------------------------
// -------------------------------------------------------------------------
void couleur_objet(int N) {
  register int j,C;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_COULEUR:
  forme_mouse(MS_SELECTEUR);

  if (OkSelect && !N) {
    message("Select object to modify color");
  }

  if (!N) {
    if (Selection && OkSelect==1) {
      if (cherche_fenetre()==FAUX) return;
    } else {
      if ((NumObjet=trouve_volume(0,3,1))==FAUX) return;
    }
  } else {
    NumObjet=N;
  }

  strcpy(StrBoite[0],"MODIFY COLOR OF AN OBJECT");
  strcpy(StrBoite[1],"Do you really want to");

  LABEL_COULEUR_PALETTE:

  while (MouseB());

  C=affiche_palette();
  if (C==-1) return;
  if (C==CSELECTION) {
    strcpy(StrBoite[2],"give this object the same color");
    strcpy(StrBoite[3],"as the one reserved for selection ?");
    if (g_boite_ONA(CentX,CentY,3,CENTRE,1)!=0) goto LABEL_COULEUR_PALETTE;
  }

  strcpy(StrBoite[2],"modify the color");

  if (Selection && OkSelect==1 && !N) {
    strcpy(StrBoite[3],"of selected objects ?");
  } else {
    sprintf(StrBoite[3],"of object nø%d ?",NumObjet);
  }

  forme_mouse(MS_FLECHE);

  if (g_boite_ONA(CentX,CentY,3,CENTRE,1)==0) {
    if (Selection && OkSelect==1 && !N) {
      for (j=1;j<=NbObjet;j++) {
        if (Objet[j]->Selection && !Objet[j]->Cache) {
          Objet[j]->Couleur=C;
        }
      }
	} else {
	  Objet[NumObjet]->Couleur=C;
      if (!N) trace_volume_all(NumObjet,NumObjet);
	}
  }

  GMouseOn();
  if (!N) goto LABEL_COULEUR;
}

// -------------------------------------------------------------------------
// -- CACHE UN OBJET -------------------------------------------------------
// -------------------------------------------------------------------------
// Type=0 Tout
// Type=1 Aucune
// Type=2 objet
// Type=3 Inverse
// Type=4 Dernier
// Type=5 S‚lection
// Type=6 GŠle
// Type=7 Ignore
// -------------------------------------------------------------------------
void cache_objet(byte Type) {
  register int i,C;

  if (pas_objet(1)) return;

  switch (Type) {
	case 0:
	  for (i=1;i<=NbObjet;i++) {
		if (Objet[i]->Cache) {
          Objet[i]->Cache=Objet[i]->Selection=0;
		  trace_volume_all(i,i);
		}
	  }
	  break;

	case 1:
      for (i=1;i<=NbObjet;i++) {
        Objet[i]->Cache=1;
        if (Objet[i]->Selection) {
          Objet[i]->Selection=0;
          Selection--;
        }
      }
      redessine_fenetre(5,1);
	  break;

	case 2:
      LABEL_CACHE:
	  forme_mouse(MS_SELECTEUR);
      message("Select object to hide");

      if ((i=trouve_volume(0,3,1))==FAUX) break;
	  C=Objet[i]->Couleur;
	  Objet[i]->Couleur=FFOND;
      if (Objet[i]->Selection) {
        Objet[i]->Selection=0;
        Selection--;
        affiche_donnees();
      }
	  trace_volume_all(i,i);
	  Objet[i]->Cache=1;
	  Objet[i]->Couleur=C;

      message_termine(0,"");
	  while (MouseB());
	  forme_mouse(MS_FLECHE);
	  GMouseOn();
      goto LABEL_CACHE;

	case 3:
	  for (i=1;i<=NbObjet;i++) {
        if (Objet[i]->Selection && !Objet[i]->Cache) {
          Objet[i]->Selection=0;
          Selection--;
        }
		Objet[i]->Cache=(Objet[i]->Cache==1 ? 0:1);
	  }
      redessine_fenetre(5,1);
	  break;

	case 4:
	  C=Objet[NbObjet]->Couleur;
	  Objet[NbObjet]->Couleur=(Objet[NbObjet]->Cache==0 ? FFOND:C);
	  if (Objet[NbObjet]->Cache==1) Objet[NbObjet]->Cache=0;
	  trace_volume_all(NbObjet,NbObjet);
	  if (Objet[NbObjet]->Couleur==FFOND) Objet[NbObjet]->Cache=1;
	  Objet[NbObjet]->Couleur=C;
	  break;

    case 5:
	  for (i=1;i<=NbObjet;i++) {
		if (Objet[i]->Selection) {
		  Objet[i]->Selection=0;
          C=Objet[i]->Couleur;
		  Objet[i]->Couleur=FFOND;
		  trace_volume_all(i,i);
		  Objet[i]->Couleur=C;
		  Objet[i]->Cache=1;
          Selection--;
		}
	  }
	  affiche_donnees();
      message_termine(0,"");
	  break;

    case 6:
      LABEL_FREEZE:

      if (status_clavier() & 4) { // CTRL
        for (i=1;i<=NbObjet;i++) {
          if (Objet[i]->Cache==0) {
            Objet[i]->Freeze=0;
            trace_volume_all(i,i);
          }
        }
        break;
      }

	  forme_mouse(MS_SELECTEUR);

      if (OkSelect) message("Select object to freeze or not");

      if (Selection && OkSelect==1) {
        if (cherche_fenetre()==FAUX) break;
      } else {
        if ((i=trouve_volume(0,3,0))==FAUX) break;
      }

      if (Selection && OkSelect==1) {
        for (i=1;i<=NbObjet;i++) {
          if (Objet[i]->Selection && !Objet[i]->Cache) {
            if (Objet[i]->Freeze) {
              Objet[i]->Freeze=0;
              trace_volume_all(i,i);
            } else {
              Objet[i]->Selection=0;
              Selection--;
              C=Objet[i]->Couleur;
              Objet[i]->Couleur=FFOND;
              trace_volume_all(i,i);
              Objet[i]->Couleur=C;
              Objet[i]->Freeze=1;
              trace_volume_all(i,i);
            }
          }
        }
        message_termine(0,"");
      } else {
        if (Objet[i]->Freeze) {
          Objet[i]->Freeze=0;
          trace_volume_all(i,i);
        } else {
          if (Objet[i]->Selection) {
            trace_volume_all(i,i);
            Selection--;
          }
          Objet[i]->Selection=0;
          C=Objet[i]->Couleur;
          Objet[i]->Couleur=FFOND;
          trace_volume_all(i,i);
          Objet[i]->Couleur=C;
          Objet[i]->Freeze=1;
          trace_volume_all(i,i);
        }
        message_termine(1,"Object : [%d] \"%s\" %s.",i,Objet[i]->Nom,
                         Objet[i]->Freeze ? "frozen":"not frozen");
      }
      
	  while (MouseB());
	  forme_mouse(MS_FLECHE);
	  GMouseOn();
      affiche_donnees();
      goto LABEL_FREEZE;

    case 7:
      LABEL_IGNORE:
	  forme_mouse(MS_SELECTEUR);
      
      if (OkSelect) message("Select object to ignore/consider.");

      if (Selection && OkSelect==1) {
        if (cherche_fenetre()==FAUX) break;
      } else {
        if ((i=trouve_volume(0,3,1))==FAUX) break;
      }

      if (Selection && OkSelect==1) {
        for (i=1;i<=NbObjet;i++) {
          if (Objet[i]->Selection && !Objet[i]->Cache) {
            Objet[i]->Ignore=(Objet[i]->Ignore==LAISSE ? 0:LAISSE);
          }
        }
        message_termine(0,"");
      } else {
        Objet[i]->Ignore=(Objet[i]->Ignore==LAISSE ? 0:LAISSE);
        message_termine(2,"Object : [%d] \"%s\" %s.",i,Objet[i]->Nom,
                         Objet[i]->Ignore ? "ignored":"not ignored");
      }

	  while (MouseB());
	  forme_mouse(MS_FLECHE);
	  GMouseOn();
      goto LABEL_IGNORE;
  }
}

// --------------------------------------------------------------------------
// ----- CREER UN NOUVEAU NOM D'OBJET ---------------------------------------
// --------------------------------------------------------------------------
byte creer_nom_objet(int N) {
  register int i,j,k=0;
  char Format[13];
  char NomObjet[13];

  j=strlen(Objet[N]->Nom);

  for (i=j-1;i>0;i--) {
    if (Objet[N]->Nom[i]<'0' || Objet[N]->Nom[i]>'9') break;
  }

  i=(i>=7 ? 7:i);
  i=(i<=0 ? j-1:i);

  strcpy(NomObjet,Objet[N]->Nom);
  NomObjet[i+1]=NULLC;

  do {
    sprintf(Format,"%s%04d",NomObjet,k);
    k++;
  } while (nom_objet_existe(Format,N));

  strcpy(Objet[N]->Nom,Format);

  return 0;
}

// --------------------------------------------------------------------------
// ----- TEST SI NOM OBJET EXISTE -------------------------------------------
// --------------------------------------------------------------------------
int nom_objet_existe(char *NomObjet,int NumeroObjet) {
  register int i;

  for (i=1;i<=NbObjet;i++) {
    if (strcmp(Objet[i]->Nom,NomObjet)==0 && i!=NumeroObjet) return 1;
  }

  return 0;
}

// --------------------------------------------------------------------------
// ----- RENOMME UN OBJET ---------------------------------------------------
// --------------------------------------------------------------------------
int renomme_objet (byte Lequel,int N) {
  int X1=CentX-110;
  int X2=CentX+110;
  int Y1=CentY-60;
  int Y2=CentY+40;
  int i,NumeroObjet;

  if (pas_objet(1)) return (0);
  LABEL_RENOMME_OBJET:

  if (Lequel) {
	forme_mouse(MS_SELECTEUR);
    message("Select object to rename");
    if ((NumeroObjet=trouve_volume(0,3,1))==FAUX) return (0);
  } else {
    NumeroObjet=(N==0 ? NbObjet:N);
  }

  LABEL_RENOMME_OBJET_ERREUR:

  g_fenetre(X1,Y1,X2,Y2,"OBJECT'S NAME",AFFICHE);

  init_texte(0,X1+90,Y1+40,"New Name",Objet[NumeroObjet]->Nom,12,"Change object's name");

  place_zone_texte(0);
  bouton_dialog(X1,X2,Y2,1,1);

  forme_mouse(MS_FLECHE);

  while (1) {
    test_texte(0,0);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
  
  if (i==0) {
    if (nom_objet_existe(ZTexte[0].Variable,NumeroObjet)) {
      while (MouseB());
      strcpy(StrBoite[0],"Error with this name");
      strcpy(StrBoite[1],"An object already exist with this name !");
      g_boite_ONA(CentX,CentY,1,CENTRE,0);
      goto LABEL_RENOMME_OBJET_ERREUR;
    }
    strcpy(Objet[NumeroObjet]->Nom,ZTexte[0].Variable);
  } else {
    return (0);
  }

  forme_mouse(MS_FLECHE);
  if (Lequel) goto LABEL_RENOMME_OBJET;
  return (1);
}

// --------------------------------------------------------------------------
// -------------- REINITIALISE LE CHAMP BUFFER DE LA STRUCT OBJET -----------
// --------------------------------------------------------------------------
void reinit_buffer_objet(void) {
  register int i;

  for (i=1;i<=NbObjet;i++) Objet[i]->Buffer=0;
}

// ----------------------------------------------------------------------------
// -- LISSE OU NON UN OBJET RAW -----------------------------------------------
// ----------------------------------------------------------------------------
void lisse_objet(void) {
  int Num;

  if (pas_objet(1)) return;
  message("Select a raw or height-field object");

  LABEL_LISSE:
  forme_mouse(MS_SELECTEUR);

  if ((Num=trouve_volume(0,3,1))==FAUX) return;

  if (Objet[Num]->Type!=TRIANGLE && Objet[Num]->Type!=HFIELD) {
    beep_erreur();
    message("This object isn't a raw or height-field object !");
  } else {
    Objet[Num]->Smooth=!Objet[Num]->Smooth;
    Poly[Objet[Num]->Poly]->Smooth=Objet[Num]->Smooth;
    message("Object nø%d \"%s\" %s",Num,Objet[Num]->Nom,Objet[Num]->Smooth ? "with smooth triangles":"with flat triangles");
    while(MouseB());
  }

  goto LABEL_LISSE;
}

// -------------------------------------------------------------------------
// -- MIROIR D'UN OBJET OU D'UNE SELECTION D'OBJET -------------------------
// -------------------------------------------------------------------------
void miroir_objet(byte Select,int N) {
  register int X1,Y1,X2,Y2;
  int Ok,Passe1;
  DBL PasX,PasY,PasDX,PasDY;
  TRANSFORM *Mat;
  Vecteur SC,SH,RO,TR,V,RD;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_MIROIR:
  Passe1=0;
  Ok=0;
  forme_mouse(Sens);
  message("Select object to mirror (TAB=choose direction)");

  if (Selection && OkSelect==1) {
    if (cherche_fenetre()==FAUX) return;
    N=cube_selection();
  } else {
    if ((N=trouve_volume(0,2,1))==FAUX) return;
  }

  NumObjet=N;

  forme_mouse(MS_FLECHE);

  if (NF==0) PasDX=PasX=Objet[N]->T[0];
  if (NF==0) PasDY=PasY=Objet[N]->T[1];
  if (NF==1) PasDX=PasX=Objet[N]->T[2];
  if (NF==1) PasDY=PasY=Objet[N]->T[1];
  if (NF==2) PasDX=PasX=Objet[N]->T[0];
  if (NF==2) PasDY=PasY=Objet[N]->T[2];
  vect_copy(RD,Objet[N]->R);

  if (!OkSelect) {
    LABEL_ROTATE_MIROIR:
    if (Passe1 && OkSelect) goto LABEL_MODIF_SENS;
    vect_copy(Objet[N]->R,RD);
    Mat=Create_Transform();
    rotation_objet(Mat,Objet[N]->R,'+');
    vect_init(V,0,0,0);
    if (Sens==MS_X) {
      if (NF==0) V[1]=180;
      if (NF==1) V[1]=180;
      if (NF==2) V[2]=180;
    }
    if (Sens==MS_Y) {
      if (NF==0) V[0]=180;
      if (NF==1) V[2]=180;
      if (NF==2) V[0]=180;
    }
    if (Sens==MS_XY) {
      if (NF==0) V[2]=180;
      if (NF==1) V[0]=180;
      if (NF==2) V[1]=180;
    }
    rotation_objet(Mat,V,'+');
    mat_decode(Mat->matrix,SC,SH,RO,TR);
    Efface_Transform(Mat);
    vect_init(Objet[N]->R,RO[0],RO[1],RO[2]);
    if (Passe1) goto LABEL_MODIF_SENS;
    Passe1=1;
  }

  select_vue(NF,CLIP_ON);

  forme_mouse(Sens);
  while (MouseB());
  GMouseOff();

  trace_boite(0,3);
  place_mouse(CentX,CentY);
  X2=gmx_r();
  Y2=gmy_r();

  while (1) {
    X1=gmx_r();
    Y1=gmy_r();
    if (MouseB()==1) { Ok=1; break; }
    if (sequence_sortie()) { Ok=0; break; }
    if (X1!=X2 || Y1!=Y2) {
      delay(5);
      trace_boite(0,3);
      switch (Sens) {
        case MS_X:
          PasX+=(X1-X2)/Vid[NF].Echelle;
          break;
        case MS_Y:
          PasY+=(Y1-Y2)/Vid[NF].Echelle;
          break;
        default:
          PasX+=(X1-X2)/Vid[NF].Echelle;
          PasY+=(Y1-Y2)/Vid[NF].Echelle;
          break;
      }
      if ((kbhit()) && getch()==9) {
        trace_boite(0,3);
        PasX=PasDX;
        PasY=PasDY;
        Sens_Souris();
        goto LABEL_ROTATE_MIROIR;
        LABEL_MODIF_SENS:
        trace_boite(0,3);
      }
      if (NF==0) Objet[N]->T[0]=PasX;
      if (NF==0) Objet[N]->T[1]=PasY;
      if (NF==1) Objet[N]->T[2]=PasX;
      if (NF==1) Objet[N]->T[1]=PasY;
      if (NF==2) Objet[N]->T[0]=PasX;
      if (NF==2) Objet[N]->T[2]=PasY;
      trace_boite(0,3);
      place_mouse(CentX,CentY);
      X2=gmx_r();
      Y2=gmy_r();
      message("\"%s\" [%05d]: X=%+.2f Y=%+.2f",Objet[N]->Nom,N,PasX,-PasY);
    }
  }

  trace_boite(0,3);

  if (Ok==0) {
    PasX=PasDX; PasY=PasDY;
    vect_copy(Objet[N]->R,RD);
  }

  if (NF==0) Objet[N]->T[0]=PasX;
  if (NF==0) Objet[N]->T[1]=PasY;
  if (NF==1) Objet[N]->T[2]=PasX;
  if (NF==1) Objet[N]->T[1]=PasY;
  if (NF==2) Objet[N]->T[0]=PasX;
  if (NF==2) Objet[N]->T[2]=PasY;

  if (Ok) {
    if (Selection && OkSelect==1) {
      selection_modif_objet(MIROIR);
    }
    redessine_fenetre(5,1);
  }

  GMouseOn();
  if (!Select) goto LABEL_MIROIR;
}

// --------------------------------------------------------------------------
// -- ECHANTILLONNE LES ROTATIONS ENTRE 0ø ET 360ø --------------------------
// --------------------------------------------------------------------------
void analyse_rotation(int i) {
  Objet[i]->R[0]=(Objet[i]->R[0]>=360 ? Objet[i]->R[0]-360:Objet[i]->R[0]);
  Objet[i]->R[1]=(Objet[i]->R[1]>=360 ? Objet[i]->R[1]-360:Objet[i]->R[1]);
  Objet[i]->R[2]=(Objet[i]->R[2]>=360 ? Objet[i]->R[2]-360:Objet[i]->R[2]);
  Objet[i]->R[0]=(Objet[i]->R[0]<0 ? 360+Objet[i]->R[0]:Objet[i]->R[0]);
  Objet[i]->R[1]=(Objet[i]->R[1]<0 ? 360+Objet[i]->R[1]:Objet[i]->R[1]);
  Objet[i]->R[2]=(Objet[i]->R[2]<0 ? 360+Objet[i]->R[2]:Objet[i]->R[2]);
}

// --------------------------------------------------------------------------
// -- COPY AUTOMATIQUE ET VERIFICATION CSG D'UN OBJET -----------------------
// --------------------------------------------------------------------------
void auto_copy_objet(int N,byte NomAuto) {
  byte Bit;

  if (incr_NbObjet(0)==0) return;
  if (alloc_mem_objet(NbObjet)==0) return;

  *Objet[NbObjet]=*Objet[N];
  
  Objet[NbObjet]->Selection=0;
  Objet[NbObjet]->Buffer=0;

  strcpy(Objet[NbObjet]->Nom,Objet[N]->Nom);
  creer_nom_objet(NbObjet);

  if (OkSelect && // -------------- Operator et s‚lection
      Objet[N]->Selection &&
      Objet[NbObjet]->CSG!=PAS_CSG) {
    if (Objet[Objet[NbObjet]->CSG]->Selection) {
      Objet[NbObjet]->CSG=NbObjet-(N-Objet[NbObjet]->CSG);
      Objet[NbObjet-(N-Objet[NbObjet]->CSG)]->Operator=OK_CSG;
    } else {
      Objet[NbObjet]->Operator=Objet[NbObjet]->CSG=PAS_CSG;
    }
  }

  if (!OkSelect && // --------------- Operator et non s‚lectionn‚
      Objet[NbObjet]->CSG!=PAS_CSG) {
    // --------- Op‚rateur
    if (Objet[NbObjet]->CSG==OK_CSG) {
      forme_mouse(MS_FLECHE);
      if (!x_fenetre(CentX,CentY,GAUCHE,1,"Boolean operator copy|"\
            "Do you want to save the boolean relation|"\
            "between objects %s and %s ?",
            Objet[NbObjet]->Nom,
            Objet[Objet[NbObjet]->CSG]->Nom)) {
        if (Objet[Objet[NbObjet]->CSG]->Operator==PAS_CSG) {
          Bit=x_fenetre(CentX,CentY,GAUCHE,1,"Boolean relation error|"\
            "The target for the boolean operator to be copied|"\
            "is destroyed. New assign for objects ?");
          if (Bit==0) {
            Objet[Objet[NbObjet]->CSG]->Operator=OK_CSG;
          } else {
            Objet[NbObjet]->Operator=Objet[NbObjet]->CSG=PAS_CSG;
          }
        }
      }
    }
  }

  if (Objet[N]->Type==SPLINE) {
    if (new_volume_spline()) {
      Spline[NbSpline]=(B_SPLINE *) mem_alloc(sizeof(B_SPLINE));
      Objet[NbObjet]->Poly=NbSpline;
      copie_spline(NbSpline,Objet[N]->Poly);
    } else {
      free_mem_objet(NbObjet);
      NbObjet--;
      return;
    }
  }

  if (NomAuto==0) {
    if (renomme_objet(0,0)==0) {
      free_mem_objet(NbObjet);
      NbObjet--;
      return;
    }
  }
}
