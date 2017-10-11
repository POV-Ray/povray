/* ---------------------------------------------------------------------------
*  DONNEES3.C
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
#include <STDIO.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

int MailleTore=10;

// -----------------------------------------------------------------------
// --------- DEFINI UN TORE EN CALCUL DIRECT -----------------------------
// -----------------------------------------------------------------------
int calcule_tore(int N) {
  register DBL Seg=(DBL)(360*PIs180)/MailleTore;
  register int j,A=0;
  DBL TX[13],TY[13],TZ[13];
  byte TB[13];
  register DBL i;

  for (j=0;j<=12;j++) {
    TX[j]=(Point[j].X*Objet[N]->P[1])-Objet[N]->P[0];
    TY[j]=Point[j].Y;
    TZ[j]=Point[j].Z*Objet[N]->P[1];
    TB[j]=Point[j].V;
  }

  // -------- Pour les rotations, on peut r‚duire/augmenter le maillage en
  // -------- faisant varier inversement proportionnellement le pas
  // -------- de i dans les boucles. Ex : 360ø/18 segments Seg=20

  // --------------------------------------------------- rotation des cercles

  for (i=0;i<=360*PIs180+Seg;i+=Seg) {
    for (j=0;j<=12;j++) {
      charge_xyz(A,TX[j],TY[j],TZ[j],TB[j],-1);
      rotationY(A,i);
      A++;
    }
  }

  // ------------------------------------ trace les segments entre les cercles

  for (j=0;j<=12;j++) {
    TB[j]=1;
    for (i=0;i<=360*PIs180+Seg;i+=Seg) {
      charge_xyz(A,TX[j],TY[j],TZ[j],TB[j],-1);
      rotationY(A,i);
      A++;
      TB[j]=0;
    }
  }

  return A-1;
}

// -----------------------------------------------------------------------
// --------- DEFINI UN QUART DE TORE EN CALCUL DIRECT --------------------
// -----------------------------------------------------------------------
int calcule_quart_tore(int N) {
  register DBL Seg=(360/MailleTore)/2;
  register int j,A=0;
  DBL TX[13],TY[13],TZ[13];
  byte TB[13];
  register DBL i;

  for (j=0;j<=12;j++) {
    TX[j]=(Point[j].X*Objet[N]->P[1])-Objet[N]->P[0];
    TY[j]=Point[j].Y;
    TZ[j]=Point[j].Z*Objet[N]->P[1];
    TB[j]=Point[j].V;
  }

  // -------- Pour les rotations, on peut r‚duire/augmenter le maillage en
  // -------- faisant varier inversement proportionnellement le pas
  // -------- de i dans les boucles. Ex : 360ø/12 segments i=30

  // --------------------------------------------------- rotation des cercles

  for (i=PIs180*0;i<=91*PIs180;i+=Seg*PIs180) {
    for (j=0;j<=12;j++) {
      charge_xyz(A,TX[j],TY[j],TZ[j],TB[j],-1);
      rotationY(A,i);
      A++;
    }
  }

  // ------------------------------------ trace les segments entre les cercles

  for (j=0;j<=12;j++) {
    for (i=PIs180*0;i<=91*PIs180;i+=Seg*PIs180) {
      TB[j]=(i==0 ? 1:0);
      charge_xyz(A,TX[j],TY[j],TZ[j],TB[j],-1);
      rotationY(A,i);
      A++;
    }
  }

  for (j=0;j<A;j++) {
    Point[j].X+=(Objet[N]->P[0]+Objet[N]->P[1]);
    Point[j].Z-=(Objet[N]->P[0]+Objet[N]->P[1]);
    Point[j].X*=2;
    Point[j].Z*=2;
    Point[j].X-=(Objet[N]->P[0]+Objet[N]->P[1]);
    Point[j].Z+=(Objet[N]->P[0]+Objet[N]->P[1]);
  }

  return A-1;
}

// -----------------------------------------------------------------------
// --------- DEFINI UN TUBE EN CALCUL DIRECT -----------------------------
// -----------------------------------------------------------------------
int calcule_tube(int N) {
  register int i;
  DBL TX[25],TY[25],TZ[25];
  byte TB[25];
  register DBL F=Objet[N]->P[0];

  for (i=0;i<=24;i++) {
    TX[i]=Point[i].X;
    TY[i]=Point[i].Y;
    TZ[i]=Point[i].Z;
    TB[i]=Point[i].V;
  }

  // ------------------------ contruction du rayon externe

  for (i=0;i<=24;i++) {
    charge_xyz((i*2)  ,TX[i],TY[i],-1.0,1,-1);
    charge_xyz((i*2)+1,TX[i],TY[i], 1.0,0,-1);
    charge_xyz(i+50 ,TX[i],TY[i],-1.0,TB[i],-1);
    charge_xyz(i+75,TX[i],TY[i], 1.0,TB[i],-1);
  }

  // ------------------------ contruction du rayon interne

  for (i=0;i<=24;i++) {
    charge_xyz(100+(i*2)  ,TX[i]*F,TY[i]*F,-1.0,1,-1);
    charge_xyz(100+(i*2)+1,TX[i]*F,TY[i]*F, 1.0,0,-1);
    charge_xyz(100+i+50   ,TX[i]*F,TY[i]*F,-1.0,TB[i],-1);
    charge_xyz(100+i+75   ,TX[i]*F,TY[i]*F, 1.0,TB[i],-1);
  }

  return 199;
}

// -----------------------------------------------------------------------
// --------- DEFINI UN CONE TRONQUE EN CALCUL DIRECT ---------------------
// -----------------------------------------------------------------------
int calcule_cone_tronque(int N) {
  register int i;
  DBL TX[13],TY[13],TZ[13];
  byte TB[13];
  register DBL F=Objet[N]->P[0];

  for (i=0;i<=12;i++) {
    TX[i]=Point[i].X;
    TY[i]=Point[i].Y;
    TZ[i]=Point[i].Z;
    TB[i]=Point[i].V;
  }

  for (i=0;i<=12;i++) {
    charge_xyz((i*2)  ,TX[i],TY[i],-1.0,1,-1);
    charge_xyz((i*2)+1,TX[i]*F,TY[i]*F, 1.0,0,-1);
    charge_xyz(i+26 ,TX[i],TY[i],-1.0,TB[i],-1);
    charge_xyz(i+39,TX[i]*F,TY[i]*F, 1.0,TB[i],-1);
  }

  return 51;
}

// -----------------------------------------------------------------------
// --------- DEFINI UN 1/4 DE TUBE EN CALCUL DIRECT ----------------------
// -----------------------------------------------------------------------
int calcule_quart_tube(int N) {
  register int i;
  register int n=-1;
  register DBL F=Objet[N]->P[0];

  for (i=180;i<=270;i+=10) {
    // ----------- 1/4 de cercles
    charge_xyz(n+ 1,cos(i*PIs180),-sin(i*PIs180), 1,(i==180 ? 1:0),-1);
    charge_xyz(n+11,cos(i*PIs180),-sin(i*PIs180),-1,(i==180 ? 1:0),-1);
    charge_xyz(n+21,cos(i*PIs180)*F,-sin(i*PIs180)*F, 1,(i==180 ? 1:0),-1);
    charge_xyz(n+31,cos(i*PIs180)*F,-sin(i*PIs180)*F,-1,(i==180 ? 1:0),-1);
    n++;
  }

  for (i=0;i<=9;i++) {
    // ----------- Extrusion
    charge_xyz(40+2*i,Point[i   ].X,Point[i   ].Y, 1,1,-1);
    charge_xyz(41+2*i,Point[i   ].X,Point[i   ].Y,-1,0,-1);
    charge_xyz(60+2*i,Point[i+20].X,Point[i+20].Y, 1,1,-1);
    charge_xyz(61+2*i,Point[i+20].X,Point[i+20].Y,-1,0,-1);
  }

  // ---------- Bords

  charge_xyz(80,Point[ 0].X,Point[ 0].Y, 1,1,-1);
  charge_xyz(81,Point[20].X,Point[20].Y, 1,0,-1);
  charge_xyz(82,Point[ 0].X,Point[ 0].Y,-1,1,-1);
  charge_xyz(83,Point[20].X,Point[20].Y,-1,0,-1);

  charge_xyz(84,Point[ 9].X,Point[ 9].Y, 1,1,-1);
  charge_xyz(85,Point[29].X,Point[29].Y, 1,0,-1);
  charge_xyz(86,Point[ 9].X,Point[ 9].Y,-1,1,-1);
  charge_xyz(87,Point[29].X,Point[29].Y,-1,0,-1);

  for (i=0;i<=87;i++) {
    Point[i].X*=2;
    Point[i].Y*=2;
    Point[i].X+=1;
    Point[i].Y-=1;
  }

  return 87;
}

// -----------------------------------------------------------------------
// --------- DEFINI UN ANNEAU EN CALCUL DIRECT ---------------------------
// -----------------------------------------------------------------------
int calcule_anneau(int N) {
  register int i;
  DBL TX[13],TY[13],TZ[13];
  byte TB[13];
  register DBL F=Objet[N]->P[0];

  for (i=0;i<=12;i++) {
    TX[i]=Point[i].X;
    TY[i]=Point[i].Y;
    TZ[i]=Point[i].Z;
    TB[i]=Point[i].V;
  }

  // ------------------------ contruction du rayon externe

  for (i=0;i<=12;i++) {
    charge_xyz(i,TX[i],TY[i],0,TB[i],-1);
  }

  // ------------------------ contruction du rayon interne

  for (i=0;i<=12;i++) {
    charge_xyz(i+13,TX[i]*F,TY[i]*F,0,TB[i],-1);
  }

  // ------------------------ contruction ligne interne-externe

  for (i=0;i<=12;i++) {
    charge_xyz(26+i*2  ,TX[i],TY[i],0,1,-1);
    charge_xyz(26+i*2+1,TX[i]*F,TY[i]*F,0,0,-1);
  }

  return 49;
}

// -----------------------------------------------------------------------
// --------- DEFINI UN ANNEAU EN CALCUL DIRECT ---------------------------
// -----------------------------------------------------------------------
int calcule_spline(int N,byte Vue) {
  register int i,Nb,NSP;

  NSP=Objet[N]->Poly;

  for (i=0;i<=Spline[NSP]->Nombre;i++) {
    Point[i].X= Spline[NSP]->dots[i][_X];
    Point[i].Y= Spline[NSP]->dots[i][_Y];
    Point[i].Z=-Spline[NSP]->dots[i][_Z];
    Point[i].X*=Objet[N]->S[0];
    Point[i].Y*=Objet[N]->S[1];
    Point[i].Z*=Objet[N]->S[2];
    rotationX(i,Objet[N]->R[0]*PIs180);
    rotationY(i,Objet[N]->R[1]*PIs180);
    rotationZ(i,Objet[N]->R[2]*PIs180);
    Point[i].X+=Objet[N]->T[0];
    Point[i].Y+=Objet[N]->T[1];
    Point[i].Z+=Objet[N]->T[2];
    Spline[NSP]->rp[i][_X]=Point[i].X;
    Spline[NSP]->rp[i][_Y]=Point[i].Y;
    Spline[NSP]->rp[i][_Z]=Point[i].Z;

    Objet[0]->Type=SPHERE;
    modif_objet(0,Spline[NSP]->dots[i][3],
                  Spline[NSP]->dots[i][3],
                  Spline[NSP]->dots[i][3],SCALE);
    modif_objet(0,0,0,0,ROTATE);
    modif_objet(0,Spline[NSP]->rp[i][_X],
                  Spline[NSP]->rp[i][_Y],
                  Spline[NSP]->rp[i][_Z],TRANSLATE);
    Objet[0]->Couleur=Objet[N]->Couleur;
    Objet[0]->Selection=Objet[N]->Selection;
    Objet[0]->Cache=Objet[N]->Cache;
    Objet[0]->Ignore=0;
    Objet[0]->Freeze=0;
    Objet[0]->Rapide=Objet[N]->Rapide;
    Objet[0]->Operator=PAS_CSG;
    Objet[0]->CSG=PAS_CSG;
    switch (Vue) {
      case 0: trace_volume_0(0); break;
      case 1: trace_volume_1(0); break;
      case 2: trace_volume_2(0); break;
      case 3: trace_volume_3(0); break;
    }
  }

  for (i=0;i<=Spline[NSP]->Nombre;i++) {
    charge_xyz(i,Spline[NSP]->dots[i][_X],
                 Spline[NSP]->dots[i][_Y],
                 Spline[NSP]->dots[i][_Z],(i==0 ? 1:0),-2);
  }

  Nb=i;

  spline(Spline[NSP]->Nombre,(Spline[NSP]->Nombre*Spline[NSP]->Subdi),Spline[NSP]->Degree,NSP);

  for (i=0;i<=(Spline[NSP]->Nombre*Spline[NSP]->Subdi);i++) {
    charge_xyz(Nb+i,Spline[NSP]->rp[i][_X],
                    Spline[NSP]->rp[i][_Y],
                    Spline[NSP]->rp[i][_Z],(i==0 ? 1:0),Objet[N]->Couleur==FFOND ? FFOND:4);
  }

  charge_xyz(Nb+i-1,Spline[NSP]->dots[Spline[NSP]->Nombre][_X],
                    Spline[NSP]->dots[Spline[NSP]->Nombre][_Y],
                    Spline[NSP]->dots[Spline[NSP]->Nombre][_Z],(i==0 ? 1:0),Objet[N]->Couleur==FFOND ? FFOND:4);

  return Nb+i-1;
}

// -----------------------------------------------------------------------
// --------- AFFICHE UNE SOR OU LATHE ------------------------------------
// -----------------------------------------------------------------------
int calcule_special(int N) {
  int k=Objet[N]->Special.Nombre;
  register DBL Seg=(DBL)(360*PIs180)/12;
  VECTOR V;
  int i,A=0;
  register DBL j;

  V=point_chaine(N,0);
  for (j=0;j<360*PIs180;j+=Seg) {
    charge_xyz(A,V.x,-V.y,V.z,(j==0 ? 1:0),-1);
    rotationY(A,j);
    A++;
  }

  for (j=0;j<360*PIs180;j+=Seg) {
    for (i=0;i<k;i++) {
      V=point_chaine(N,i);
      charge_xyz(A,V.x,-V.y,V.z,(i==0 ? 1:0),-1);
      rotationY(A,j);
      A++;
    }
  }

  V=point_chaine(N,k-1);
  for (j=0;j<360*PIs180;j+=Seg) {
    charge_xyz(A,V.x,-V.y,V.z,(j==0 ? 1:0),-1);
    rotationY(A,j);
    A++;
  }

  return A-1;
}

// -----------------------------------------------------------------------
// --------- AFFICHE UNE FORME EXTRUDEE ----------------------------------
// -----------------------------------------------------------------------
int calcule_extrude(int N) {
  int k=Objet[N]->Special.Nombre;
  VECTOR V;
  int i,A=0;

  for (i=0;i<k;i++) {
    V=point_chaine(N,i);
    charge_xyz(A,V.x,-1,-V.y,1,-1);
    A++;
    charge_xyz(A,V.x, 1,-V.y,0,-1);
    A++;
  }

  for (i=0;i<k;i++) {
    V=point_chaine(N,i);
    charge_xyz(A    ,V.x,-1,-V.y,(i ? 0:1),-1);
    charge_xyz(A+k+1,V.x, 1,-V.y,(i ? 0:1),-1);
    A++;
  }

  V=point_chaine(N,0);
  charge_xyz(A,    V.x,-1,-V.y,0,-1);
  charge_xyz(A+k+1,V.x, 1,-V.y,0,-1);

  return A+k;
}

// -----------------------------------------------------------------------
// --------- CALCULE LIGNE D'UN SPOT -------------------------------------
// -----------------------------------------------------------------------
int calcule_spot(int N) {
  charge_xyz(0,Objet[0]->P[_X],Objet[0]->P[_Y],-Objet[0]->P[_Z],1,-1);
  charge_xyz(1,Objet[0]->S[_X],Objet[0]->S[_Y],-Objet[0]->S[_Z],0,-1);
  vect_init(Objet[0]->S,1,1,1);
  return 1;
}

// -----------------------------------------------------------------------
// --------- CALCULE LIGNE PATCH DE BEZIER -------------------------------
// -----------------------------------------------------------------------
int calcule_patch_bezier(int N) {
  register int i,n,k,j;
  B_PATCH *Patch=(B_PATCH *) malloc(sizeof(B_PATCH));
  VECTOR V;

  for (i=0;i<16;i++) {
    Patch->Cp[i+1]=point_chaine(N,i);
  }

  ComputeBernstein();
  ComputePolyMesh(Patch);

  n=0;

  for (i=0;i<100;i+=10) {
    j=1;
    for (k=i;k<=i+9;k++) {
      charge_xyz(n, Patch->R[k].x,
                   -Patch->R[k].y,
                    Patch->R[k].z,
                    j,-1);
      n++;
      j=0;
    }
  }

  for (k=0;k<=9;k++) {
    j=1;
    for (i=0;i<100;i+=10) {
      charge_xyz(n, Patch->R[i+k].x,
                   -Patch->R[i+k].y,
                    Patch->R[i+k].z,
                    j,-1);
      n++;
      j=0;
    }
  }

  for (i=0;i<16;i++) {
    V=point_chaine(N,i);
    //charge_xyz(n++,V.x,-V.y,V.z,0,-2);
    charge_xyz(n++,V.x,-V.y,V.z,1,-2);
  }

  free(Patch);
  return (n-1);
}
