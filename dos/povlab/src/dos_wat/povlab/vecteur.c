/* ---------------------------------------------------------------------------
*  VECTEUR.C
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
#include <STRING.H>
#include <CTYPE.H>

#include "GLOBAL.H"

// ---------------------------------------------------------------------------
// -------- INITIALISE UN VECTEUR --------------------------------------------
// ---------------------------------------------------------------------------
void vect_init (Vecteur v,DBL x,DBL y,DBL z) {
  v[_X]=x;
  v[_Y]=y;
  v[_Z]=z;
}

// ---------------------------------------------------------------------------
// ------------ COPIE UN VECTEUR ---------------------------------------------
// ---------------------------------------------------------------------------
void vect_copy (Vecteur v1,Vecteur v2) {
  v1[_X]=v2[_X];
  v1[_Y]=v2[_Y];
  v1[_Z]=v2[_Z];
}

// ---------------------------------------------------------------------------
// ------------ TEST SI VECTEURS EGAUX ---------------------------------------
// ---------------------------------------------------------------------------
int vect_equal (Vecteur v1,Vecteur v2) {
  if (v1[_X]==v2[_X] && v1[_Y]==v2[_Y] && v1[_Z]==v2[_Z])
    return 1;
  else
    return 0;
}

// ---------------------------------------------------------------------------
// ------------ ADDITIONNE DEUX VECTEURS -------------------------------------
// ---------------------------------------------------------------------------
void vect_add (Vecteur v1,Vecteur v2,Vecteur v3) {
  v1[_X]=v2[_X]+v3[_X];
  v1[_Y]=v2[_Y]+v3[_Y];
  v1[_Z]=v2[_Z]+v3[_Z];
}

void vect_sub (Vecteur v1,Vecteur v2,Vecteur v3) {
  v1[_X]=v2[_X]-v3[_X];
  v1[_Y]=v2[_Y]-v3[_Y];
  v1[_Z]=v2[_Z]-v3[_Z];
}

void vect_scale (Vecteur v1,Vecteur v2,DBL k) {
  v1[_X]=k*v2[_X];
  v1[_Y]=k*v2[_Y];
  v1[_Z]=k*v2[_Z];
}

DBL vect_mag (Vecteur v) {
  DBL mag=sqrt(v[_X]*v[_X]+v[_Y]*v[_Y]+v[_Z]*v[_Z]);
  return mag;
}

void vect_normalize (Vecteur v) {
  DBL mag=vect_mag (v);
  if (mag>0.0) vect_scale (v,v,1.0/mag);
}

DBL vect_dot (Vecteur v1,Vecteur v2) {
  return (v1[_X]*v2[_X]+v1[_Y]*v2[_Y]+v1[_Z]*v2[_Z]);
}

void vect_cross (Vecteur v1,Vecteur v2,Vecteur v3) {
  v1[_X]=(v2[_Y]*v3[_Z])-(v2[_Z]*v3[_Y]);
  v1[_Y]=(v2[_Z]*v3[_X])-(v2[_X]*v3[_Z]);
  v1[_Z]=(v2[_X]*v3[_Y])-(v2[_Y]*v3[_X]);
}

void vect_min (Vecteur v1,Vecteur v2,Vecteur v3) {
  v1[_X]=(v2[_X]<v3[_X]) ? v2[_X]:v3[_X];
  v1[_Y]=(v2[_Y]<v3[_Y]) ? v2[_Y]:v3[_Y];
  v1[_Z]=(v2[_Z]<v3[_Z]) ? v2[_Z]:v3[_Z];
}

void vect_max (Vecteur v1,Vecteur v2,Vecteur v3) {
  v1[_X]=(v2[_X]>v3[_X]) ? v2[_X]:v3[_X];
  v1[_Y]=(v2[_Y]>v3[_Y]) ? v2[_Y]:v3[_Y];
  v1[_Z]=(v2[_Z]>v3[_Z]) ? v2[_Z]:v3[_Z];
}


// -------------------------------------------------------------------------
// ---------------- RETURN THE ANGLE BETWEEN TWO VECTORS -------------------
// -------------------------------------------------------------------------
DBL vect_angle (Vecteur v1,Vecteur v2) {
  DBL  mag1,mag2,angle,cos_theta;

  mag1=vect_mag(v1);
  mag2=vect_mag(v2);

  if (mag1*mag2==0.0)
    angle=0.0;
  else {
    cos_theta=vect_dot(v1,v2)/(mag1*mag2);

  if (cos_theta<=-1.0)
    angle=180.0;
  else if (cos_theta>=+1.0)
    angle=0.0;
  else
    angle=(180.0/PI)*acos(cos_theta);
  }

  return angle;
}

void vect_print (FILE *f,Vecteur v,int dec,char sep) {
  char fstr[]="%.4g,%.4g,%.4g";

  fstr[4] =sep;
  fstr[9]=sep;

  dec=dec; // DO ----- Contre warning

  fprintf (f,fstr,v[_X],v[_Y],v[_Z]);
}

// ---------------------------------------------------------------------------
// -- ROTATE A VECTOR ABOUT THE X,Y OR Z AXIS --------------------------------
// ---------------------------------------------------------------------------
void vect_rotate (Vecteur v1,Vecteur v2,int axis,DBL angle) {
    DBL cosa,sina;

    cosa=cos (PIs180*angle);
    sina=sin (PIs180*angle);

    switch (axis) {
    case _X:
        v1[_X]= v2[_X];
        v1[_Y]= v2[_Y]*cosa+v2[_Z]*sina;
        v1[_Z]= v2[_Z]*cosa-v2[_Y]*sina;
        break;

    case _Y:
        v1[_X]=v2[_X]*cosa-v2[_Z]*sina;
        v1[_Y]=v2[_Y];
        v1[_Z]=v2[_Z]*cosa+v2[_X]*sina;
        break;

    case _Z:
        v1[_X]=v2[_X]*cosa+v2[_Y]*sina;
        v1[_Y]=v2[_Y]*cosa-v2[_X]*sina;
        v1[_Z]=v2[_Z];
        break;
    }
}

// --------------------------------------------------------------------------
// -- ROTATE A VECTOR ABOUT A SPECIFIC AXIS ---------------------------------
// --------------------------------------------------------------------------
void vect_axis_rotate (Vecteur v1,Vecteur v2,Vecteur axis,DBL angle) {
    DBL  cosa,sina;
    MATRIX mat;

    cosa=cos (PIs180*angle);
    sina=sin (PIs180*angle);

    mat[0][0]=(axis[_X]*axis[_X])+((1.0-(axis[_X]*axis[_X]))*cosa);
    mat[0][1]=(axis[_X]*axis[_Y]*(1.0-cosa))-(axis[_Z]*sina);
    mat[0][2]=(axis[_X]*axis[_Z]*(1.0-cosa))+(axis[_Y]*sina);
    mat[0][3]=0.0;

    mat[1][0]=(axis[_X]*axis[_Y]*(1.0-cosa))+(axis[_Z]*sina);
    mat[1][1]=(axis[_Y]*axis[_Y])+((1.0-(axis[_Y]*axis[_Y]))*cosa);
    mat[1][2]=(axis[_Y]*axis[_Z]*(1.0-cosa))-(axis[_X]*sina);
    mat[1][3]=0.0;

    mat[2][0]=(axis[_X]*axis[_Z]*(1.0-cosa))-(axis[_Y]*sina);
    mat[2][1]=(axis[_Y]*axis[_Z]*(1.0-cosa))+(axis[_X]*sina);
    mat[2][2]=(axis[_Z]*axis[_Z])+((1.0-(axis[_Z]*axis[_Z]))*cosa);
    mat[2][3]=0.0;

    mat[3][0]=mat[3][1]=mat[3][2]=mat[3][3]=0.0;

    vect_transform (v1,v2,mat);
}


// ----------------------------------------------------------------------------
// -- TRANSFORM THE GIVEN VECTOR ----------------------------------------------
// ----------------------------------------------------------------------------
void vect_transform (Vecteur v1,Vecteur v2,MATRIX mat) {
    Vecteur tmp;

    tmp[_X]=(v2[_X]*mat[0][0])+(v2[_Y]*mat[1][0])+(v2[_Z]*mat[2][0])+mat[3][0];
    tmp[_Y]=(v2[_X]*mat[0][1])+(v2[_Y]*mat[1][1])+(v2[_Z]*mat[2][1])+mat[3][1];
    tmp[_Z]=(v2[_X]*mat[0][2])+(v2[_Y]*mat[1][2])+(v2[_Z]*mat[2][2])+mat[3][2];

    vect_copy (v1,tmp);
}
