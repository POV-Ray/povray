/* ---------------------------------------------------------------------------
*  SPLINE.C
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

#define BSP_TAB 0
#define TAB_BSP 1
#define DOTS_RP 2
#define RP_DOTS 3
#define ADAPT 0
#define TRANS 1

int total, points, degree;

int NbSpline=-1;
B_SPLINE *Spline[NB_SPLINE_MAX];

static DBL U[50];

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
static DBL N(int k,int t,DBL u) {
  register DBL firstterm, secondterm;

  if (t == 1)
    if (U[k] <= u && u < U[k + 1])
      return 1.0;
    else
      return 0.0;
  if (U[k + t - 1] - U[k] < 1.0e-10)    // Test for zero with real #s
    firstterm = 0.0;
  else
    firstterm = ((u - U[k]) / (U[k + t - 1] - U[k])) * N(k, t - 1, u);
  if (U[k + t] - U[k + 1] < 1.0e-10)
    secondterm = 0.0;
  else
    secondterm = ((U[k + t] - u) / (U[k + t] - U[k + 1])) * N(k + 1, t - 1, u);
  return firstterm + secondterm;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void spline(int n,int m,int t,int NSP) {
  register int j, k;
  DBL temp, u;

  for (j = 0; j <= n + t; j++) {
    if (j < t)
      U[j] = 0.0;
    else if (t <= j && j <= n)
      U[j] = j - t + 1;
    else
      U[j] = n - t + 2;
  }
  for (j = 0; j <= m; j++) {
    u = ((DBL) j / m) * (n - t + 2 - .00000001);
    Spline[NSP]->rp[j][0] = 0.0;
    Spline[NSP]->rp[j][1] = 0.0;
    Spline[NSP]->rp[j][2] = 0.0;
    Spline[NSP]->rp[j][3] = 0.0;
    for (k = 0; k <= n; k++) {
      temp = N(k, t, u);
      Spline[NSP]->rp[j][0] += Spline[NSP]->dots[k][0] * temp;
      Spline[NSP]->rp[j][1] += Spline[NSP]->dots[k][1] * temp;
      Spline[NSP]->rp[j][2] += Spline[NSP]->dots[k][2] * temp;
      Spline[NSP]->rp[j][3] += Spline[NSP]->dots[k][3] * temp;
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void putcone(DBL Pt1[4],DBL Pt2[4],FILE *Fichier) {
  fprintf(Fichier,"    cone {");
  fprintf(Fichier," <%.4g,%.4g,%.4g>,%.4g ",Pt1[0],Pt1[1],Pt1[2],Pt1[3]);
  fprintf(Fichier,"<%.4g,%.4g,%.4g>,%.4g",Pt2[0],Pt2[1],Pt2[2],Pt2[3]);
  fprintf(Fichier,"}\n");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void lputcone(DBL Pt1[4],DBL Pt2[4],FILE *Fichier) {
  DBL  dx, dy, dz;     // vector Pt1 to Pt2
  DBL  dx2, dy2, dz2;
  DBL  len;            // length of vector
  DBL  xzlen;
  DBL  dt1[4], dt2[4];     // local dot's
  DBL  a1, a2;
  DBL  alpha;
  TRANSFORM *Mat;
  Vecteur SC,SH,RO,TR;

  // Align cones so as to be tangential to spheres.
  // by Robin Luiten (luiten@trantor.nmsd.oz.au)
  //
  // dx,dy,dz is vector from Pt1 to Pt2 */

  dx = Pt2[0] - Pt1[0];
  dy = Pt2[1] - Pt1[1];
  dz = Pt2[2] - Pt1[2];
  len = sqrt(dx*dx + dy*dy + dz*dz);
  xzlen = sqrt(dx*dx + dz*dz);

  //
  // Transform dx,dy,dz till dy and dz are zero.
  // Retain necessary rotations for inverse transform.
  // The transform is made up of two rotations
  //
  // 1. rotate around y-axis to reduce dz to zero.
  //  angle a1.
  // 2. rotate around z-axis to reduce dy to zero.
  //  angle a2.
  // atan2(y/x) - result -PI to PI
  //

  a1 = atan2(dz,dx);      // 1

  //
  // Transform the vector by rotation a1 around y-axis.
  //

  dx2 = xzlen;
  dy2 = dy;
  dz2 = 0;

  a2 = atan2(dy2, dx2);       // 2

  // after these two rotations dx3,dy3,dz3 = (len, 0, 0) */
  // Calculate the angle to the tangent from Pt1 to Pt2 */

  alpha = acos( (Pt1[3] - Pt2[3]) / len);   // result 0 to PI

  // now find tangent intersection on Pt1

  dt1[0] = cos(alpha) * Pt1[3];
  dt1[1] = 0;
  dt1[2] = 0;
  dt1[3] = sin(alpha) * Pt1[3];

  // now find tangent intersection on Pt2

  dt2[0] = len + (cos(alpha) * Pt2[3]);
  dt2[1] = 0;
  dt2[2] = 0;
  dt2[3] = sin(alpha) * Pt2[3];

  //
  // Now transform the two tangent positions back to
  // the original coordinate system of Pt1 and Pt2.
  //
  // I could transform the intersection stuff back and just spit
  // out the two endpoints of the cone with the radius but I can
  // also make POV do the job and just output the cone and then
  // output the transformations to put it where I want.
  //
  //  cone
  //  {
  //      <end>, radius, <end>, radius
  //      rotate <0,   0, -a2>
  //      rotate <0, -a1,   0>
  //      translate <Pt1[0], Pt1[1], Pt1[2]>
  //  }
  //
  // Initially I will make POV do the work to test the initial
  // part of the algorithm. Later I will transform points back because
  // it will make the pov include files smaller. [a lot smaller]
  //
  // Actually I think I will leave POV to do the work because that
  // means it is much easier to align textures to the Cones if required.
  //
  // NOTES for later rotate about y axis angle a.
  //
  // x2 = x*cos(a) - y*sin(a)
  // y2 = y
  // z2 = x*sin(a) + z*cos(a)
  //
  //

  // convert angle a1 and angle a2 to degrees */
  a1=(a1 * 180)/PI;
  a2=(a2 * 180)/PI;

  // printf("Y angle: %4.6f, Z angle: %4.6f\n", a1, a2);
  //
  // NOTE: The rotation about the Y axis is the negative
  // of the calculated value because POV is setup as a
  // left hand system rather than a right hand one.
  //

  Mat=Create_Transform();
  vect_init(RO,0,0,a2);
  rotation_objet(Mat,RO,'+');
  vect_init(RO,0,-a1,0);
  rotation_objet(Mat,RO,'+');
  mat_decode(Mat->matrix,SC,SH,RO,TR);
  Efface_Transform(Mat);

  fprintf(Fichier,"    cone {\n");
  fprintf(Fichier,"      <%.4g,%.4g,%.4g>,%.4g\n",dt1[0],dt1[1],dt1[2],dt1[3]);
  fprintf(Fichier,"      <%.4g,%.4g,%.4g>,%.4g\n",dt2[0],dt2[1],dt2[2],dt2[3]);
  fprintf(Fichier,"      rotate <%.4g,%.4g,%.4g>\n",RO[0],RO[1],RO[2]);
  fprintf(Fichier,"      translate <%.4g,%.4g,%.4g>\n",Pt1[0],Pt1[1],Pt1[2]);
  fprintf(Fichier,"    }\n");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void putsphere(DBL Pt[4],FILE *Fichier) {
  fprintf(Fichier,"    sphere { <%.4g,%.4g,%.4g>,%.4g }\n",Pt[0],
                                                           Pt[1],
                                                           Pt[2],
                                                           Pt[3]);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void generate_output_spline(int N,FILE *Fichier) {
  register int j;
  int total;
  int NSP=Objet[N]->Poly;

  total=Spline[NSP]->Nombre;

  //spline(dots, total, rp, (points * total), Spline[NSP]->Degree);
  spline(Spline[NSP]->Nombre,Spline[NSP]->Nombre*Spline[NSP]->Subdi,Spline[NSP]->Degree,NSP);

  for (j=0;j<=Spline[NSP]->Nombre*Spline[NSP]->Subdi;j++) {
    Point[j].X= Spline[NSP]->rp[j][_X];
    Point[j].Y= Spline[NSP]->rp[j][_Y];
    Point[j].Z=-Spline[NSP]->rp[j][_Z];
    Point[j].X*=Objet[N]->S[0];
    Point[j].Y*=Objet[N]->S[1];
    Point[j].Z*=Objet[N]->S[2];
    rotationX(j,Objet[N]->R[0]*PIs180);
    rotationY(j,Objet[N]->R[1]*PIs180);
    rotationZ(j,Objet[N]->R[2]*PIs180);
    Point[j].X+=Objet[N]->T[0];
    Point[j].Y+=Objet[N]->T[1];
    Point[j].Z+=Objet[N]->T[2];
    Spline[NSP]->rp[j][_X]= Point[j].X;
    Spline[NSP]->rp[j][_Y]=-Point[j].Y;
    Spline[NSP]->rp[j][_Z]=-Point[j].Z;
  }

  fprintf(Fichier,"  merge {\n");

  putsphere(Spline[NSP]->rp[0],Fichier);

  for (j=1;j<Spline[NSP]->Nombre*Spline[NSP]->Subdi;j++) {
    putsphere(Spline[NSP]->rp[j],Fichier);
    lputcone(Spline[NSP]->rp[j-1],Spline[NSP]->rp[j],Fichier);
  }

  Point[j].X= Spline[NSP]->dots[total][_X];
  Point[j].Y= Spline[NSP]->dots[total][_Y];
  Point[j].Z=-Spline[NSP]->dots[total][_Z];
  Point[j].X*=Objet[N]->S[0];
  Point[j].Y*=Objet[N]->S[1];
  Point[j].Z*=Objet[N]->S[2];
  rotationX(j,Objet[N]->R[0]*PIs180);
  rotationY(j,Objet[N]->R[1]*PIs180);
  rotationZ(j,Objet[N]->R[2]*PIs180);
  Point[j].X+=Objet[N]->T[0];
  Point[j].Y+=Objet[N]->T[1];
  Point[j].Z+=Objet[N]->T[2];
  Spline[NSP]->rp[total][_X]= Point[j].X;
  Spline[NSP]->rp[total][_Y]=-Point[j].Y;
  Spline[NSP]->rp[total][_Z]=-Point[j].Z;
  Spline[NSP]->rp[total][ 3]=Spline[NSP]->dots[total][3];

  putsphere(Spline[NSP]->rp[total],Fichier);
  lputcone(Spline[NSP]->rp[j-1],Spline[NSP]->rp[total],Fichier);

  fprintf(Fichier,"  }\n");
}

// -------------------------------------------------------------------------
// -- EFFACE UN VOLUME SPLINE EN MEMOIRE -----------------------------------
// -------------------------------------------------------------------------
void free_mem_spline(int Num) {
  mem_free(Spline[Num],sizeof(SPLINE));
}

// -------------------------------------------------------------------------
// -- EFFACE UN VOLUME SPLINE EN TRAITANT AUSSI LES OBJETS -----------------
// -------------------------------------------------------------------------
void free_mem_spline_objet(int Num) {
  register int i,Nb=0;

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->Poly==Num) Nb++;
  }

  if (Nb==1) {
    for (i=Num;i<NbSpline;i++) {
      *Spline[i]=*Spline[i+1];
    }
    free_mem_spline(NbSpline);
    NbSpline--;
    for (i=1;i<=NbObjet;i++) {
      if (Objet[i]->Poly>=Num) Objet[i]->Poly--;
    }
  }
}

// -------------------------------------------------------------------------
// -- COPIE 2 SPLINES ENTRE ELLES ------------------------------------------
// -------------------------------------------------------------------------
int copie_spline(int C,int S) {
  register int i;

  Spline[C]->Buffer  =Spline[S]->Buffer;
  Spline[C]->Nombre  =Spline[S]->Nombre;
  Spline[C]->Type    =Spline[S]->Type;
  Spline[C]->Degree  =Spline[S]->Degree;
  Spline[C]->Subdi   =Spline[S]->Subdi;
  for (i=0;i<=Spline[S]->Nombre;i++) {
    Spline[C]->dots[i][0]=Spline[S]->dots[i][0];
    Spline[C]->dots[i][1]=Spline[S]->dots[i][1];
    Spline[C]->dots[i][2]=Spline[S]->dots[i][2];
    Spline[C]->dots[i][3]=Spline[S]->dots[i][3];
  }
  return 1;
}

// -------------------------------------------------------------------------
// -- AJOUTE UN NOUVEAU VOLUME SPLINE --------------------------------------
// -------------------------------------------------------------------------
int new_volume_spline(void) {
  NbSpline++;
  if (NbSpline>NB_SPLINE_MAX) {
    forme_mouse(MS_FLECHE);
    f_erreur("The max number of B-Spline is %d",NB_SPLINE_MAX);
    NbSpline--;
    return 0;
  }
  return 1;
}

// ----------------------------------------------------------------------------
// -- PROCEDURE SPLINE -> TABLEAU VERTEX ET INVERSEMENT -----------------------
// ----------------------------------------------------------------------------
void spline_tableau(byte NF,int Num,int i,byte Sens) {
  if (Sens==TAB_BSP) {
    switch (NF) {
      case 0:
        Spline[Num]->dots[i][_X]=Pt.X[i];
        Spline[Num]->dots[i][_Y]=Pt.Y[i];
        break;
      case 1:
        Spline[Num]->dots[i][_Z]=Pt.X[i];
        Spline[Num]->dots[i][_Y]=Pt.Y[i];
        break;
      case 2:
        Spline[Num]->dots[i][_X]=Pt.X[i];
        Spline[Num]->dots[i][_Z]=Pt.Y[i];
        break;
    }
    return;
  }
  if (Sens==BSP_TAB) {
    switch (NF) {
      case 0:
        Pt.X[i]=Spline[Num]->dots[i][_X];
        Pt.Y[i]=Spline[Num]->dots[i][_Y];
        break;
      case 1:
        Pt.X[i]=Spline[Num]->dots[i][_Z];
        Pt.Y[i]=Spline[Num]->dots[i][_Y];
        break;
      case 2:
        Pt.X[i]=Spline[Num]->dots[i][_X];
        Pt.Y[i]=Spline[Num]->dots[i][_Z];
        break;
    }
    return;
  }
}

// ----------------------------------------------------------------------------
// -- PROCEDURE SPLINE -> TABLEAU VERTEX ET INVERSEMENT -----------------------
// ----------------------------------------------------------------------------
void recalcule_points(int N,byte Tache) {
  DBL XI,YI,ZI,XA,YA,ZA,X,Y,Z,XD,YD,ZD;
  register int i;
  int NPT=Spline[Objet[N]->Poly]->Nombre;
  int NSP=Objet[N]->Poly;

 if (Tache==ADAPT) {
    XI=YI=ZI=DBL_MAX_; // ------- X,Y,Z min/max
    XA=YA=ZA=DBL_MIN_;

    for (i=0;i<=NPT;i++) {
      XA=(Spline[NSP]->dots[i][_X]>XA ? Spline[NSP]->dots[i][_X]:XA);
      YA=(Spline[NSP]->dots[i][_Y]>YA ? Spline[NSP]->dots[i][_Y]:YA);
      ZA=(Spline[NSP]->dots[i][_Z]>ZA ? Spline[NSP]->dots[i][_Z]:ZA);
      XI=(Spline[NSP]->dots[i][_X]<XI ? Spline[NSP]->dots[i][_X]:XI);
      YI=(Spline[NSP]->dots[i][_Y]<YI ? Spline[NSP]->dots[i][_Y]:YI);
      ZI=(Spline[NSP]->dots[i][_Z]<ZI ? Spline[NSP]->dots[i][_Z]:ZI);
    }

    XD=X=(XA+XI)/2;
    YD=Y=(YA+YI)/2;
    ZD=Z=(ZA+ZI)/2;

    for (i=0;i<=NPT;i++) {  // ------------- recentrage
      Spline[NSP]->dots[i][_X]-=X;
      Spline[NSP]->dots[i][_Y]-=Y;
      Spline[NSP]->dots[i][_Z]-=Z;
    }

    X=(fabs(XA-X)+fabs(XI-X))/2;
    Y=(fabs(YA-Y)+fabs(YI-Y))/2;
    Z=(fabs(ZA-Z)+fabs(ZI-Z))/2;

    X=(X<=DEGEN_TOL ? 1:X);
    Y=(Y<=DEGEN_TOL ? 1:Y);
    Z=(Z<=DEGEN_TOL ? 1:Z);

    for (i=0;i<=NPT;i++) {  // ------------- rescaling
      Spline[NSP]->dots[i][_X]/=X;
      Spline[NSP]->dots[i][_Y]/=Y;
      Spline[NSP]->dots[i][_Z]/=Z;
    }

    Objet[N]->S[_X]=X;
    Objet[N]->S[_Y]=Y;
    Objet[N]->S[_Z]=Z;

    Objet[N]->T[_X]=XD;
    Objet[N]->T[_Y]=YD;
    Objet[N]->T[_Z]=ZD;
  } else {
    for (i=0;i<=NPT;i++) {
      Point[i].X=Spline[NSP]->dots[i][_X];
      Point[i].Y=Spline[NSP]->dots[i][_Y];
      Point[i].Z=Spline[NSP]->dots[i][_Z];

      Point[i].X*=Objet[N]->S[0];
      Point[i].Y*=Objet[N]->S[1];
      Point[i].Z*=Objet[N]->S[2];
      rotationX(i,Objet[N]->R[0]*PIs180);
      rotationY(i,Objet[N]->R[1]*PIs180);
      rotationZ(i,Objet[N]->R[2]*PIs180);
      Point[i].X+=Objet[N]->T[0];
      Point[i].Y+=Objet[N]->T[1];
      Point[i].Z+=Objet[N]->T[2];

      Spline[NSP]->dots[i][_X]=Point[i].X;
      Spline[NSP]->dots[i][_Y]=Point[i].Y;
      Spline[NSP]->dots[i][_Z]=Point[i].Z;
    }
  }
}

// ----------------------------------------------------------------------------
// -- CREE UNE COURBE SPLINE POUR PATH OBJETS ---------------------------------
// ----------------------------------------------------------------------------
int creation_spline(int N) {
  register int i;
  
  message("Select a viewport for the B-Spline path");

  select_vue(5,CLIP_OFF);
  GMouseOff();
  forme_mouse(MS_FLECHE);
  if ((cherche_fenetre()==FAUX) || NF==3) return 0;

  NbVertex=0;
  GMouseOn();
  while (MouseB());

  i=creer_point(1);

  if (NbVertex<=2) {
    f_erreur("You need to draw more than 2 vertices for a B-Spline");
    return 0;
  }

  if (i) {
    if (!new_volume_spline()) return 0;
    Spline[NbSpline]=(B_SPLINE *) mem_alloc(sizeof(B_SPLINE));
    Objet[N]->Poly=NbSpline;
    Spline[NbSpline]->Nombre=NbVertex;
    Spline[NbSpline]->Degree=3;
    Spline[NbSpline]->Subdi=5;
    
    for (i=0;i<=NbVertex;i++) {
      spline_tableau(NF,NbSpline,i,TAB_BSP);
      switch (NF) {
        case 0: Spline[NbSpline]->dots[i][_Z]=0; break;
        case 1: Spline[NbSpline]->dots[i][_X]=0; break;
        case 2: Spline[NbSpline]->dots[i][_Y]=0; break;
      }
      Spline[NbSpline]->dots[i][3]=1.0;
    }

    recalcule_points(N,ADAPT);
    for (i=0;i<=NbVertex;i++) Spline[NbSpline]->dots[i][_Z]*=-1;
    redessine_fenetre(5,1);
  } else {
    return 0;
  }
  return 1;
}

// ----------------------------------------------------------------------------
// -- MODIFICATION D'UNE SPLINE -----------------------------------------------
// ----------------------------------------------------------------------------
byte move_point_spline(void) {
  register int N,i;
  int NSP;

  if (pas_objet(1)) return 0;

  LABEL_SELECT_BSPLINE:

  message("Select a B-Spline path object");
  forme_mouse(MS_SELECTEUR);
  if ((N=trouve_volume(0,2,1))==FAUX) return 0;

  if (Objet[N]->Type!=SPLINE) {
    beep_erreur();
    message("This object isn't a B-Spline path !");
    goto LABEL_SELECT_BSPLINE;
  }

  NSP=Objet[N]->Poly;
  NbVertex=Spline[NSP]->Nombre;

  for (i=0;i<=NbVertex;i++) Spline[NSP]->dots[i][_Z]*=-1;
  recalcule_points(N,TRANS);
  
  for (i=0;i<=NbVertex;i++) {
    spline_tableau(NF,NSP,i,BSP_TAB);
  }

  //redessine_forme();
  modifier_point(NSP);
  //redessine_forme();

  for (i=0;i<=NbVertex;i++) {
    spline_tableau(NF,NSP,i,TAB_BSP);
  }
  recalcule_points(N,ADAPT);

  for (i=0;i<=NbVertex;i++) Spline[NSP]->dots[i][_Z]*=-1;

  redessine_fenetre(5,1);

  return 1;
}

// ----------------------------------------------------------------------------
// -- SUPPRESSION D'UN POINT D'UNE SPLINE -------------------------------------
// ----------------------------------------------------------------------------
byte delete_point_spline(void) {
  register int N,i,NSP;

  if (pas_objet(1)) return 0;

  LABEL_SELECT_BSPLINE:

  message("Select a B-Spline path object");
  forme_mouse(MS_SELECTEUR);
  if ((N=trouve_volume(0,2,1))==FAUX) return 0;
  
  if (Objet[N]->Type!=SPLINE) {
    beep_erreur();
    message("This object isn't a B-Spline path !");
    goto LABEL_SELECT_BSPLINE;
  }

  NSP=Objet[N]->Poly;
  NbVertex=Spline[NSP]->Nombre;

  for (i=0;i<=NbVertex;i++) Spline[NSP]->dots[i][_Z]*=-1;
  recalcule_points(N,TRANS);
  for (i=0;i<=NbVertex;i++) spline_tableau(NF,Objet[N]->Poly,i,BSP_TAB);

  //redessine_forme();
  supprimer_point(Objet[N]->Poly);
  //redessine_forme();

  Spline[Objet[N]->Poly]->Nombre=NbVertex;
  for (i=0;i<=NbVertex;i++) spline_tableau(NF,Objet[N]->Poly,i,TAB_BSP);
  recalcule_points(N,ADAPT);
  for (i=0;i<=NbVertex;i++) Spline[NSP]->dots[i][_Z]*=-1;

  redessine_fenetre(5,1);

  return 1;
}

// -------------------------------------------------------------------------
// -- SAUVE LES POINTS POUR UNE SPLINE DANS .SCN ASCII ---------------------
// -------------------------------------------------------------------------
void sauve_spline(FILE *Fichier,int N) {
  register int j,NSP;
  register int k;
  char Buffer[30];

  NSP=Objet[N]->Poly;
  j=Spline[NSP]->Nombre;

  if (Spline[NSP]->Buffer) return;
  Spline[NSP]->Buffer=1;

  sprintf(Buffer,"SAVING SPLINE: %s",Objet[N]->Nom);
  f_jauge(1,AFFICHE,0,0,Buffer);
  fprintf(Fichier,"Object %05d: %d %d %d %d\n",
                   N,
                   j,
                   Spline[NSP]->Type,
                   Spline[NSP]->Degree,
                   Spline[NSP]->Subdi);

  for (k=0;k<=j;k++) {
    fprintf(Fichier,"Object %05d: %.4g %.4g %.4g %.4g\n",
      N,Spline[NSP]->dots[k][_X],
        Spline[NSP]->dots[k][_Y],
        Spline[NSP]->dots[k][_Z],
        Spline[NSP]->dots[k][ 3]);
    if (k%20==0) f_jauge(1,MODIF,k,j,NULL);
  }

  f_jauge(1,EFFACE,0,0,NULL);
}

// -------------------------------------------------------------------------
// -- LECTURE DES POINTS DE SPLINE DANS LE FICHIER .SCN --------------------
// -------------------------------------------------------------------------
byte lecture_spline_scn(FILE *Fichier,char *MarqueObjet) {
  register int NPT,i;
  char Buffer[256];
  byte Ok=1;

  if (NbSpline<Objet[NbObjet]->Poly) {
    Spline[Objet[NbObjet]->Poly]->Buffer=1;
  } else {
    if (Spline[Objet[NbObjet]->Poly]->Buffer) return 1;
  }

  if (!new_volume_spline()) return 0;
  Spline[NbSpline]=(B_SPLINE *) mem_alloc(sizeof(B_SPLINE));
  Objet[NbObjet]->Poly=NbSpline;

  sprintf(Buffer,"READING VERTICES: %s",Objet[NbObjet]->Nom);
  f_jauge(1,AFFICHE,0,0,Buffer);
  message("Reading vertices for spline %s",Objet[NbObjet]->Nom);

  fgets(Buffer,256,Fichier);
  analyse_ligne(Buffer,32);
  NPT=Spline[NbSpline]->Nombre=atoi(Argu[2]);
  Spline[NbSpline]->Type=atoi(Argu[3]);
  Spline[NbSpline]->Degree=atoi(Argu[4]);
  Spline[NbSpline]->Subdi=atoi(Argu[5]);
  message("Poly %d nb %d",NbSpline,NPT);

  for (i=0;i<=NPT;i++) {
    if (feof(Fichier)) break;
    fgets(Buffer,256,Fichier);
    analyse_ligne(Buffer,32);
    if (!strinstr(0,MarqueObjet,Argu[1])) {
      Spline[NbSpline]->dots[i][_X]=atof(Argu[2]);
      Spline[NbSpline]->dots[i][_Y]=atof(Argu[3]);
      Spline[NbSpline]->dots[i][_Z]=atof(Argu[4]);
      Spline[NbSpline]->dots[i][ 3]=atof(Argu[5]);
      if (i%20==0) f_jauge(1,MODIF,i,NPT,NULL);
    } else {
      Ok=0;
      break;
    }
  }

  f_jauge(1,EFFACE,0,0,NULL);
  return Ok;
}

// --------------------------------------------------------------------------
// ----- REINITIALISE LE CHAMP BUFFER DE LA STRUCT SPLINE -------------------
// --------------------------------------------------------------------------
void reinit_buffer_spline(void) {
  register int i;

  for (i=0;i<=NbSpline;i++) Spline[i]->Buffer=0;
}

// --------------------------------------------------------------------------
// ----- EDITE LE RAYON D'UN ELEMENT DE LA B-SPLINE -------------------------
// --------------------------------------------------------------------------
void edit_vertex_spline(void) {
  register int N,i,P,NSP;
  byte S,C;
  DBL T;

  if (pas_objet(1)) return;

  LABEL_SELECT_NSPLINE:
  P=-1;

  message("Select a B-Spline path object");
  forme_mouse(MS_SELECTEUR);
  if ((N=trouve_volume(0,2,1))==FAUX) return;

  if (Objet[N]->Type!=SPLINE) {
    beep_erreur();
    message("This object isn't a B-Spline path !");
    goto LABEL_SELECT_NSPLINE;
  }

  NSP=Objet[N]->Poly;
  NbVertex=Spline[NSP]->Nombre;

  for (i=0;i<=NbVertex;i++) Spline[NSP]->dots[i][_Z]*=-1;
  recalcule_points(N,TRANS);
  for (i=0;i<=NbVertex;i++) spline_tableau(NF,NSP,i,BSP_TAB);

  LABEL_TEST_UN_POINT:

  forme_mouse(MS_SELECTEUR);
  GMouseOn();

  while (MouseB()!=1) {
    gmx_v(); gmy_v();
      if (sequence_sortie()) {
      forme_mouse(MS_FLECHE);
      goto LABEL_SORTIE_EDIT_VERTEX;
    }
  }

  P=test_point();
  if (P<0) goto LABEL_TEST_UN_POINT;
  GMouseOff();

  Objet[0]->Type=SPHERE;
  modif_objet(0,Spline[NSP]->dots[P][3],
                Spline[NSP]->dots[P][3],
                Spline[NSP]->dots[P][3],SCALE);
  modif_objet(0,0,0,0,ROTATE);
  modif_objet(0,Spline[NSP]->dots[P][_X],
                Spline[NSP]->dots[P][_Y],
                Spline[NSP]->dots[P][_Z],TRANSLATE);
  Objet[0]->Couleur=Objet[N]->Couleur;
  Objet[0]->Selection=Objet[N]->Selection;
  Objet[0]->Cache=Objet[N]->Cache;
  Objet[0]->Ignore=0;
  Objet[0]->Freeze=0;
  Objet[0]->Rapide=Objet[N]->Rapide;
  Objet[0]->Operator=PAS_CSG;
  Objet[0]->CSG=PAS_CSG;
  sprintf(Objet[0]->Nom,"Vertex #%d",P);
  deformation3D(1,0);
  T=Objet[0]->S[_X];
  
  while (MouseB());
  recalcule_points(N,ADAPT);
  for (i=0;i<=NbVertex;i++) Spline[NSP]->dots[i][_Z]*=-1;

  S=Objet[N]->Selection;
  C=Objet[N]->Couleur;
  Objet[N]->Couleur=FFOND;
  Objet[N]->Selection=0;
  trace_volume_all(N,N);

  Spline[NSP]->dots[P][3]=T;

  Objet[N]->Selection=S;
  Objet[N]->Couleur=C;
  trace_volume_all(N,N);

  recalcule_points(N,TRANS);
  for (i=0;i<=NbVertex;i++) spline_tableau(NF,NSP,i,BSP_TAB);

  goto LABEL_TEST_UN_POINT;
  
  LABEL_SORTIE_EDIT_VERTEX:

  recalcule_points(N,ADAPT);

  redessine_fenetre(5,1);
}

// ----------------------------------------------------------------------------
// -- AJOUT D'UN POINT A UNE SPLINE -------------------------------------------
// ----------------------------------------------------------------------------
byte add_point_spline(void) {
  register int N,i,j;
  int NSP;
  register int P;
  byte S,C;

  if (pas_objet(1)) return 0;

  LABEL_SELECT_BSPLINE:
  P=-1;

  message("Select a B-Spline path object");
  forme_mouse(MS_SELECTEUR);
  if ((N=trouve_volume(0,2,1))==FAUX) return 0;
  
  if (Objet[N]->Type!=SPLINE) {
    beep_erreur();
    message("This object isn't a B-Spline path !");
    goto LABEL_SELECT_BSPLINE;
  }

  NSP=Objet[N]->Poly;
  NbVertex=Spline[NSP]->Nombre;

  LABEL_TEST_UN_SEGMENT:

  forme_mouse(MS_SELECTEUR);
  GMouseOn();

  while (MouseB()!=1) {
    gmx_v(); gmy_v();
    if (sequence_sortie()) {
      forme_mouse(MS_FLECHE);
      goto LABEL_SORTIE_EDIT_SEGMENT;
    }
  }

  GMouseOff();

  for (i=0;i<=NbVertex;i++) Spline[NSP]->dots[i][_Z]*=-1;
  recalcule_points(N,TRANS);
  for (i=0;i<=NbVertex;i++) spline_tableau(NF,NSP,i,BSP_TAB);
  P=test_segment(NSP);

  recalcule_points(N,ADAPT);
  for (i=0;i<=NbVertex;i++) Spline[NSP]->dots[i][_Z]*=-1;
  if (P<0) goto LABEL_TEST_UN_SEGMENT;

  S=Objet[N]->Selection;
  C=Objet[N]->Couleur;
  Objet[N]->Couleur=FFOND;
  Objet[N]->Selection=0;
  trace_volume_all(N,N);

  NbVertex++;
  Spline[NSP]->Nombre=NbVertex;

  for (i=NbVertex;i>P;i--) {
    for (j=0;j<=3;j++) {
      Spline[NSP]->dots[i][j]=Spline[NSP]->dots[i-1][j];
    }
  }

  P++;

  for (j=0;j<=3;j++) {
    Spline[NSP]->dots[P][j]=(Spline[NSP]->dots[P-1][j]+Spline[NSP]->dots[P+1][j])/2;
  }

  Objet[N]->Selection=S;
  Objet[N]->Couleur=C;
  trace_volume_all(N,N);
  goto LABEL_SELECT_BSPLINE;
  
  LABEL_SORTIE_EDIT_SEGMENT:

  redessine_fenetre(5,1);
  return 1;
}

// -------------------------------------------------------------------------
// -- MODIFIE LES PARAMETRES D'UNE SPLINE ----------------------------------
// -------------------------------------------------------------------------
void parametre_spline(void) {
  int X1=CentX-180;
  int X2=CentX+180;
  int Y1=CentY-60;
  int Y2=CentY+55,i,NSP,N;

  if (pas_objet(1)) return;

  LABEL_SELECT_BSPLINE:

  message("Select a B-Spline path object");
  forme_mouse(MS_SELECTEUR);
  if ((N=trouve_volume(0,2,1))==FAUX) return;
  
  if (Objet[N]->Type!=SPLINE) {
    beep_erreur();
    message("This object isn't a B-Spline path !");
    goto LABEL_SELECT_BSPLINE;
  }

  NSP=Objet[N]->Poly;
  NbVertex=Spline[NSP]->Nombre;

  g_fenetre(X1,Y1,X2,Y2,"B-Spline parameters",AFFICHE);

  init_potar(0,X1+30,Y1+40,120,2,6,(DBL) Spline[NSP]->Degree,1.0,"Polynomial degree");
  affiche_potar(0);
  init_potar(1,X1+30,Y1+60,120,1,6,(DBL) Spline[NSP]->Subdi,1.0,"Curve subdivision");
  affiche_potar(1);

  text_xy(X1+240,Y1+34,"Polynomial degree",BLANC);
  text_xy(X1+240,Y1+54,"Curve subdivision",BLANC);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_potar(0,1);
    test_groupe_pastille(11,13);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  if (i==0) {
    Spline[NSP]->Degree=(int) Potar[0].Val;
    Spline[NSP]->Subdi=(int) Potar[1].Val;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
}

// -------------------------------------------------------------------------
// -- LISSE UNE SPLINE -----------------------------------------------------
// -------------------------------------------------------------------------
void smooth_spline(void) {
  DBL Pas;
  int i,NSP,N,S,C;

  if (pas_objet(1)) return;

  LABEL_SELECT_BSPLINE:

  message("Select a B-Spline path object");
  forme_mouse(MS_SELECTEUR);
  if ((N=trouve_volume(0,2,1))==FAUX) return;
  
  if (Objet[N]->Type!=SPLINE) {
    beep_erreur();
    message("This object isn't a B-Spline path !");
    goto LABEL_SELECT_BSPLINE;
  }

  S=Objet[N]->Selection;
  C=Objet[N]->Couleur;
  Objet[N]->Selection=0;
  Objet[N]->Couleur=FFOND;
  trace_volume_all(N,N);

  NSP=Objet[N]->Poly;
  NbVertex=Spline[NSP]->Nombre;

  Pas=-(Spline[NSP]->dots[0][3]-Spline[NSP]->dots[NbVertex][3])/NbVertex;

  for (i=1;i<NbVertex;i++) {
    Spline[NSP]->dots[i][3]=Spline[NSP]->dots[i-1][3]+Pas;
  }

  Objet[N]->Selection=S;
  Objet[N]->Couleur=C;
  trace_volume_all(N,N);
}

