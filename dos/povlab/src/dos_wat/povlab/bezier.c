/* ---------------------------------------------------------------------------
*  BEZIER.C
*
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
#include <STDIO.H>
#include <IO.H>
#include <DOS.H>
#include <STDLIB.H>
#include <STRING.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"

static DBL BStein[11][5]; // adjust this declaration to your needs

// --------------------------------------------------------------------------
// ----- RENCENTRAGE DES POINTS SUR <1,1,1> ECHELLE <0,0,0> POSITION --------
// --------------------------------------------------------------------------
void adapte_bezier(int N) {
  DBL MaxX,MaxY,MaxZ;
  DBL MinX,MinY,MinZ;
  register int Nb=Objet[N]->Special.Nombre,i;
  DBL T1,T2,T3,X,Y,Z,T;
  VECTOR V;

  MaxX=MaxY=MaxZ=DBL_MIN_;
  MinX=MinY=MinZ=DBL_MAX_;

  for (i=0;i<Nb;i++) {
    V=point_chaine(N,i);
    MaxX=(V.x>MaxX ? V.x:MaxX);
    MaxY=(V.y>MaxY ? V.y:MaxY);
    MaxZ=(V.z>MaxZ ? V.z:MaxZ);
    MinX=(V.x<MinX ? V.x:MinX);
    MinY=(V.y<MinY ? V.y:MinY);
    MinZ=(V.z<MinZ ? V.z:MinZ);
  }

  T1=X=(MaxX+MinX)/2;
  T2=Y=(MaxY+MinY)/2;
  T3=Z=(MaxZ+MinZ)/2;

  for (i=0;i<Nb;i++) {
    V=point_chaine(N,i);
    V.x-=X;
    V.y-=Y;
    V.z-=Z;
    change_point_chaine(N,i,V);
  }

  X=(fabs(MaxX-X)+fabs(MinX-X))/2;
  Y=(fabs(MaxY-Y)+fabs(MinY-Y))/2;
  Z=(fabs(MaxZ-Z)+fabs(MinZ-Z))/2;

  X=(X<=0 ? 1:X);
  Y=(Y<=0 ? 1:Y);
  Z=(Z<=0 ? 1:Z);

  T=X;
  T=(Y>T ? Y:T);
  T=(Z>T ? Z:T);
  T=(T<=0 ? 1:T);

  for (i=0;i<Nb;i++) {
    V=point_chaine(N,i);
    V.x/=X;
    V.y/=Y;
    V.z/=Z;
    change_point_chaine(N,i,V);
  }

  Objet[N]->S[_X]=fabs(X);
  Objet[N]->S[_Y]=fabs(Y);
  Objet[N]->S[_Z]=fabs(Z);

  Objet[N]->T[_X]= T1;
  Objet[N]->T[_Y]=-T2;
  Objet[N]->T[_Z]=-T3;
}

// -------------------------------------------------------------------------
// -- IMPORT .PPF BEZIER OBJECT --------------------------------------------
// -------------------------------------------------------------------------
void import_bezier(void) {
  char *Spec[2]={"1","*.PPF"};
  char *Buffer=(char *) malloc(256);
  FILE *f;
  byte Loop=0;

  strcpy(Buffer,selection_fichier(100,100,".PPF FILES",Spec));
  if (Buffer[0]==27) { free(Buffer); return; }
  f=fopen(Buffer,"rt");

  while (!feof(f)) {
    fgets(Buffer,80,f);
    analyse_ligne(Buffer,32);
    if (atoi(Argu[2])!=BEZIER) {
      if (!Loop) {
        f_erreur("This is not a bezier patch object !");
      }
      break;
    }
    if (new_objet(BEZIER,2)) {
      lecture_special_scn(f,"00000:",NbObjet);
      trace_volume_all(NbObjet,NbObjet);
      Loop=1;
    }
  }

  fclose(f);
  free(Buffer);
  affiche_donnees();
}

// -------------------------------------------------------------------------
// -- ENREGISTRE BEZIER AU FORMAT POV --------------------------------------
// -------------------------------------------------------------------------
void enregistre_bezier_patch_pov(FILE *Fichier,int N,int L) {
  register int i;
  VECTOR V;

  outl(Fichier,1,"type %d\n",Objet[N]->Special.Type);
  outl(Fichier,1,"flatness %.4g\n",Objet[N]->P[_X]);
  outl(Fichier,1,"u_steps %.4g\n",Objet[N]->P[_Y]);
  outl(Fichier,1,"v_steps %.4g,\n",Objet[N]->P[_Z]);

  for (i=0;i<16;i++) {
    V=point_chaine(N,i);
    outl(Fichier,1,"<%.4g,%.4g,%.4g>%c\n",V.x,V.y,V.z,(i==15 ? ' ':','));
  }

}

// ------------------------------------------------------------------------------
// -- COMPUTE POLYMESHES --------------------------------------------------------
// ------------------------------------------------------------------------------
void ComputePolyMesh (B_PATCH *Wk) {
  register int p,n,qr,rr;
  register DBL h1,h2,h3,h4;
  VECTOR Q[41];
	
  // -- transfer the Q-points, which must not be calculated
  // -- see graph in POLYMESH.HTM for detailed Point-Order

  Q[1].x = Wk->Cp[1].x;       // Q[1] = P1
  Q[1].y = Wk->Cp[1].y;
  Q[1].z = Wk->Cp[1].z;

  Q[10].x = Wk->Cp[4].x;      // Q[10] = P4
  Q[10].y = Wk->Cp[4].y;
  Q[10].z = Wk->Cp[4].z;

  Q[31].x = Wk->Cp[13].x;     // Q[31] = P13
  Q[31].y = Wk->Cp[13].y;
  Q[31].z = Wk->Cp[13].z;

  Q[40].x = Wk->Cp[16].x;     // Q[40] = P16
  Q[40].y = Wk->Cp[16].y;
  Q[40].z = Wk->Cp[16].z;


  // -- compute the Q-points along P1..P4, where the distance
  // -- between the points is determined by the preset PT_GAP

   for (p = 2; p <= 9; p++) {

     h1 = Wk->Cp[1].x * BStein[p][1];
     h2 = Wk->Cp[2].x * BStein[p][2];
     h3 = Wk->Cp[3].x * BStein[p][3];
     h4 = Wk->Cp[4].x * BStein[p][4];

     Q[p].x = h1 + h2 + h3 + h4;

     // --

     h1 = Wk->Cp[1].y * BStein[p][1];
     h2 = Wk->Cp[2].y * BStein[p][2];
     h3 = Wk->Cp[3].y * BStein[p][3];
     h4 = Wk->Cp[4].y * BStein[p][4];

     Q[p].y = h1 + h2 + h3 + h4;

     // --

     h1 = Wk->Cp[1].z * BStein[p][1];
     h2 = Wk->Cp[2].z * BStein[p][2];
     h3 = Wk->Cp[3].z * BStein[p][3];
     h4 = Wk->Cp[4].z * BStein[p][4];

     Q[p].z = h1 + h2 + h3 + h4;

  }

  // -- compute the Q-points along P5..P8 ...

  for ( p = 1; p <= 10; p++ )  {

     // --

     h1 = Wk->Cp[5].x * BStein[p][1];
     h2 = Wk->Cp[6].x * BStein[p][2];
     h3 = Wk->Cp[7].x * BStein[p][3];
     h4 = Wk->Cp[8].x * BStein[p][4];

     Q[p + 10].x = h1 + h2 + h3 + h4;

     // --

     h1 = Wk->Cp[5].y * BStein[p][1];
     h2 = Wk->Cp[6].y * BStein[p][2];
     h3 = Wk->Cp[7].y * BStein[p][3];
     h4 = Wk->Cp[8].y * BStein[p][4];

     Q[p + 10].y = h1 + h2 + h3 + h4;

     // --

     h1 = Wk->Cp[5].z * BStein[p][1];
     h2 = Wk->Cp[6].z * BStein[p][2];
     h3 = Wk->Cp[7].z * BStein[p][3];
     h4 = Wk->Cp[8].z * BStein[p][4];

     Q[p + 10].z = h1 + h2 + h3 + h4;

  }

  // -- compute the Q-points along P9..P12 ...

  for ( p = 1; p <= 10; p++ )  {

     // --

     h1 = Wk->Cp[ 9].x * BStein[p][1];
     h2 = Wk->Cp[10].x * BStein[p][2];
     h3 = Wk->Cp[11].x * BStein[p][3];
     h4 = Wk->Cp[12].x * BStein[p][4];

     Q[p + 20].x = h1 + h2 + h3 + h4;

     // --

     h1 = Wk->Cp[ 9].y * BStein[p][1];
     h2 = Wk->Cp[10].y * BStein[p][2];
     h3 = Wk->Cp[11].y * BStein[p][3];
     h4 = Wk->Cp[12].y * BStein[p][4];

     Q[p + 20].y = h1 + h2 + h3 + h4;

     // --

     h1 = Wk->Cp[ 9].z * BStein[p][1];
     h2 = Wk->Cp[10].z * BStein[p][2];
     h3 = Wk->Cp[11].z * BStein[p][3];
     h4 = Wk->Cp[12].z * BStein[p][4];

     Q[p + 20].z = h1 + h2 + h3 + h4;

  }

  // -- compute the Q-points along P13..P16

  for ( p = 2; p <= 9; p++ )       {
     // --

     h1 = Wk->Cp[13].x * BStein[p][1];
     h2 = Wk->Cp[14].x * BStein[p][2];
     h3 = Wk->Cp[15].x * BStein[p][3];
     h4 = Wk->Cp[16].x * BStein[p][4];

     Q[p + 30].x = h1 + h2 + h3 + h4;

     // --

     h1 = Wk->Cp[13].y * BStein[p][1];
     h2 = Wk->Cp[14].y * BStein[p][2];
     h3 = Wk->Cp[15].y * BStein[p][3];
     h4 = Wk->Cp[16].y * BStein[p][4];

     Q[p + 30].y = h1 + h2 + h3 + h4;

     // --

     h1 = Wk->Cp[13].z * BStein[p][1];
     h2 = Wk->Cp[14].z * BStein[p][2];
     h3 = Wk->Cp[15].z * BStein[p][3];
     h4 = Wk->Cp[16].z * BStein[p][4];

     Q[p + 30].z = h1 + h2 + h3 + h4;

  }
  
  // -- compute R-points on the Q-curves

  n = 0;

  for ( qr = 1; qr <= 10; qr++ )  {
    for (rr = 1; rr <= 10; rr++ )     {

       h1 = Q[qr     ].x * BStein[rr][1];
       h2 = Q[qr + 10].x * BStein[rr][2];
       h3 = Q[qr + 20].x * BStein[rr][3];
       h4 = Q[qr + 30].x * BStein[rr][4];

       Wk->R[n].x = h1 + h2 + h3 + h4;

       // --

       h1 = Q[qr     ].y * BStein[rr][1];
       h2 = Q[qr + 10].y * BStein[rr][2];
       h3 = Q[qr + 20].y * BStein[rr][3];
       h4 = Q[qr + 30].y * BStein[rr][4];

       Wk->R[n].y = h1 + h2 + h3 + h4;

       // --

       h1 = Q[qr     ].z * BStein[rr][1];
       h2 = Q[qr + 10].z * BStein[rr][2];
       h3 = Q[qr + 20].z * BStein[rr][3];
       h4 = Q[qr + 30].z * BStein[rr][4];

       Wk->R[n].z = h1 + h2 + h3 + h4;

       // --

       n++;

    }
  }
}

// ------------------------------------------------------------------------------
// -- COMPUTE BERNSTEIN ---------------------------------------------------------
// ------------------------------------------------------------------------------
void ComputeBernstein(void) {
  register int r;
  register DBL dist=0.11;

  for (r = 2; r <= 9; r++) {
    BStein[r][1] = (DBL) pow((1 - dist), 3);
    BStein[r][2] = (DBL) (3 * dist) * pow((1 - dist), 2);
    BStein[r][3] = (DBL) (3 * pow(dist, 2)) * (1 - dist);
    BStein[r][4] = (DBL) pow(dist, 3);                   
    dist += 0.11;
  }

  BStein[1][1] = 1;
  BStein[1][2] = 0;
  BStein[1][3] = 0;
  BStein[1][4] = 0;

  BStein[10][1] = 0;
  BStein[10][2] = 0;
  BStein[10][3] = 0;
  BStein[10][4] = 1;
}
