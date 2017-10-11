#include <MATH.H>
#include <FLOAT.H>
#include "PDBLAB.H"

// ---------------------------------------------------------------------------
// -------- INITIALISE UN VECTEUR --------------------------------------------
// ---------------------------------------------------------------------------
void vect_init (Vecteur v,float x,float y,float z) {
  v[_X]=x;
  v[_Y]=y;
  v[_Z]=z;
}

void vect_scale (Vecteur v1,Vecteur v2,float k) {
  v1[_X]=k*v2[_X];
  v1[_Y]=k*v2[_Y];
  v1[_Z]=k*v2[_Z];
}

void vect_normalize (Vecteur v) {
  float mag=vect_mag (v);
  if (mag>0.0) vect_scale (v,v,1.0/mag);
}

float vect_dot (Vecteur v1,Vecteur v2) {
  return (v1[_X]*v2[_X]+v1[_Y]*v2[_Y]+v1[_Z]*v2[_Z]);
}

void vect_cross (Vecteur v1,Vecteur v2,Vecteur v3) {
  v1[_X]=(v2[_Y]*v3[_Z])-(v2[_Z]*v3[_Y]);
  v1[_Y]=(v2[_Z]*v3[_X])-(v2[_X]*v3[_Z]);
  v1[_Z]=(v2[_X]*v3[_Y])-(v2[_Y]*v3[_X]);
}

float vect_mag (Vecteur v) {
  float mag=sqrt(v[_X]*v[_X]+v[_Y]*v[_Y]+v[_Z]*v[_Z]);
  return mag;
}

// -------------------------------------------------------------------------
// Initialize the matrix to the following values:
//   1.0   0.0   0.0   0.0
//   0.0   1.0   0.0   0.0
//   0.0   0.0   1.0   0.0
//   0.0   0.0   0.0   1.0
// -------------------------------------------------------------------------
void MIdentity (MATRIX *result) {
  register int i, j;

  for (i = 0 ; i < 4 ; i++)
    for (j = 0 ; j < 4 ; j++)
      if (i == j)
        (*result)[i][j] = 1.0;
      else
        (*result)[i][j] = 0.0;
}

// -------------------------------------------------------------------------
void MTimes (MATRIX *result,MATRIX *matrix1,MATRIX *matrix2) {
  register int i, j, k;
  MATRIX temp_matrix;

  for (i = 0 ; i < 4 ; i++)
    for (j = 0 ; j < 4 ; j++) {
      temp_matrix[i][j] = 0.0;
      for (k = 0 ; k < 4 ; k++)
        temp_matrix[i][j] += (*matrix1)[i][k] * (*matrix2)[k][j];
    }

  for (i = 0 ; i < 4 ; i++)
    for (j = 0 ; j < 4 ; j++)
      (*result)[i][j] = temp_matrix[i][j];
}

// -------------------------------------------------------------------------
void MTranspose (MATRIX *result,MATRIX *matrix1) {
  register int i, j;
  MATRIX temp_matrix;

  for (i = 0 ; i < 4 ; i++)
    for (j = 0 ; j < 4 ; j++)
      temp_matrix[i][j] = (*matrix1)[j][i];

  for (i = 0 ; i < 4 ; i++)
    for (j = 0 ; j < 4 ; j++)
      (*result)[i][j] = temp_matrix[i][j];
}

// -------------------------------------------------------------------------
void Compute_Scaling_Transform (TRANSFORM *result,VERTORR *vector) {
  MIdentity ((MATRIX *)result -> matrix);
  (result -> matrix)[0][0] = vector -> x;
  (result -> matrix)[1][1] = vector -> y;
  (result -> matrix)[2][2] = vector -> z;

  MIdentity ((MATRIX *)result -> inverse);
  (result -> inverse)[0][0] = 1.0 / vector -> x;
  (result -> inverse)[1][1]= 1.0 / vector -> y;
  (result -> inverse)[2][2] = 1.0 / vector -> z;
}

// -------------------------------------------------------------------------
void Compute_Translation_Transform (TRANSFORM *transform,VERTORR *vector) {
  MIdentity ((MATRIX *)transform -> matrix);
  (transform -> matrix)[3][0] = vector -> x;
  (transform -> matrix)[3][1] = vector -> y;
  (transform -> matrix)[3][2] = vector -> z;

  MIdentity ((MATRIX *)transform -> inverse);
  (transform -> inverse)[3][0] = - vector -> x;
  (transform -> inverse)[3][1] = - vector -> y;
  (transform -> inverse)[3][2] = - vector -> z;
}

// -------------------------------------------------------------------------

void Compute_Rotation_Transform (TRANSFORM *transform,VERTORR *vector) {
  MATRIX Matrix;
  VERTORR Radian_Vector;
  register float cosx, cosy, cosz, sinx, siny, sinz;

  VScale (Radian_Vector,*vector,PIs180);
  MIdentity ((MATRIX *)transform -> matrix);
  cosx = cos (Radian_Vector.x);
  sinx = sin (Radian_Vector.x);
  cosy = cos (Radian_Vector.y);
  siny = sin (Radian_Vector.y);
  cosz = cos (Radian_Vector.z);
  sinz = sin (Radian_Vector.z);

  (transform -> matrix) [1][1] = cosx;
  (transform -> matrix) [2][2] = cosx;
  (transform -> matrix) [1][2] = sinx;
  (transform -> matrix) [2][1] = 0.0 - sinx;
  MTranspose ((MATRIX *)transform -> inverse, (MATRIX *)transform -> matrix);

  MIdentity ((MATRIX *)Matrix);
  Matrix [0][0] = cosy;
  Matrix [2][2] = cosy;
  Matrix [0][2] = 0.0 - siny;
  Matrix [2][0] = siny;
  MTimes ((MATRIX *)transform -> matrix, (MATRIX *)transform -> matrix, (MATRIX *)Matrix);
  MTranspose ((MATRIX *)Matrix, (MATRIX *)Matrix);
  MTimes ((MATRIX *)transform -> inverse, (MATRIX *)Matrix, (MATRIX *)transform -> inverse);

  MIdentity ((MATRIX *)Matrix);
  Matrix [0][0] = cosz;
  Matrix [1][1] = cosz;
  Matrix [0][1] = sinz;
  Matrix [1][0] = 0.0 - sinz;
  MTimes ((MATRIX *)transform -> matrix, (MATRIX *)transform -> matrix, (MATRIX *)Matrix);
  MTranspose ((MATRIX *)Matrix, (MATRIX *)Matrix);
  MTimes ((MATRIX *)transform -> inverse, (MATRIX *)Matrix, (MATRIX *)transform -> inverse);
}

// -------------------------------------------------------------------------
void Compose_Transforms (TRANSFORM *Original_Transform,TRANSFORM *New_Transform) {
  MTimes ((MATRIX *)Original_Transform -> matrix,
    (MATRIX *)Original_Transform -> matrix,
    (MATRIX *)New_Transform -> matrix);

  MTimes ((MATRIX *)Original_Transform -> inverse,
    (MATRIX *)New_Transform -> inverse,
    (MATRIX *)Original_Transform -> inverse);
}

// -------------------------------------------------------------------------
//   ROTATION ABOUT AN ARBITRARY AXIS - FORMULA FROM:
//      "COMPUTATIONAL GEOMETRY FOR DESIGN AND MANUFACTURE",
//      FAUX & PRATT
//   NOTE THAT THE ANGLES FOR THIS TRANSFORM ARE SPECIFIED IN RADIANS.
// -------------------------------------------------------------------------
void Compute_Axis_Rotation_Transform (TRANSFORM *transform,VERTORR *V,float angle) {
  float l, cosx, sinx;

  VLength(l, *V);
  VInverseScaleEq(*V, l);

  MIdentity(&transform->matrix);
  cosx = cos(angle);
  sinx = sin(angle);
  transform->matrix[0][0] = V->x * V->x + cosx * (1.0 - V->x * V->x);
  transform->matrix[0][1] = V->x * V->y * (1.0 - cosx) + V->z * sinx;
  transform->matrix[0][2] = V->x * V->z * (1.0 - cosx) - V->y * sinx;
  transform->matrix[1][0] = V->x * V->y * (1.0 - cosx) - V->z * sinx;
  transform->matrix[1][1] = V->y * V->y + cosx * (1.0 - V->y * V->y);
  transform->matrix[1][2] = V->y * V->z * (1.0 - cosx) + V->x * sinx;
  transform->matrix[2][0] = V->x * V->z * (1.0 - cosx) + V->y * sinx;
  transform->matrix[2][1] = V->y * V->z * (1.0 - cosx) - V->x * sinx;
  transform->matrix[2][2] = V->z * V->z + cosx * (1.0 - V->z * V->z);
  MTranspose(&transform->inverse, &transform->matrix);   
}

// -------------------------------------------------------------------------
TRANSFORM *Create_Transform(void) {
  TRANSFORM *New;

  if ((New = (TRANSFORM *) malloc (sizeof (TRANSFORM))) == NULL) exit(0);

  MIdentity ((MATRIX *) &(New -> matrix[0][0]));
  MIdentity ((MATRIX *) &(New -> inverse[0][0]));
  return (New);
}


// -------------------------------------------------------------------------
TRANSFORM *Copy_Transform (TRANSFORM *Old) {
  TRANSFORM *New;

  if (Old != NULL) {
    New  = Create_Transform ();
    *New = *Old;
  } else New = NULL;

  return (New);
}

// -------------------------------------------------------------------------
VERTORR *Create_Vector(void) {
  VERTORR *New;

  if ((New = (VERTORR *) malloc (sizeof (VERTORR))) == NULL) exit(0);
  Make_Vector (New, 0.0, 0.0, 0.0);

  return (New);
}

// -------------------------------------------------------------------------
VERTORR *Copy_Vector (VERTORR *Old) {
  VERTORR *New;

  if (Old != NULL) {
    New  = Create_Vector ();
    *New = *Old;
  } else New = NULL;

  return (New);
}

// --------------------------------------------------------------------------
// -- TRANSLATION D'UN OBJET PAR MATRICE ------------------------------------
// --------------------------------------------------------------------------
void translation_objet(TRANSFORM *Object,Vecteur V,unsigned char Signe) {
  TRANSFORM Trans;
  VERTORR Vect;
  int S=(Signe=='+' ? 1:-1);

  Make_Vector(&Vect,V[0]*S,V[1]*S,V[2]*S);
  Compute_Translation_Transform(&Trans,&Vect);
  Compose_Transforms(Object,&Trans);
}

// --------------------------------------------------------------------------
// -- AJUSTEMENT D'UN OBJET PAR MATRICE -------------------------------------
// --------------------------------------------------------------------------
void ajustement_objet(TRANSFORM *Object,Vecteur V,unsigned char Signe) {
  TRANSFORM Trans;
  VERTORR Vect;
  int S=(Signe=='+' ? 1:-1);

  Make_Vector(&Vect,V[0]*S,V[1]*S,V[2]*S);
  Compute_Scaling_Transform(&Trans,&Vect);
  Compose_Transforms(Object,&Trans);
}

// --------------------------------------------------------------------------
// -- ROTATION D'UN OBJET PAR MATRICE ---------------------------------------
// --------------------------------------------------------------------------
void rotation_objet(TRANSFORM *Object,Vecteur V,unsigned char  Signe) {
  TRANSFORM Trans;
  VERTORR Vect;
  int S=(Signe=='+' ? 1:-1);

  Make_Vector(&Vect,V[0]*S,V[1]*S,V[2]*S);
  Compute_Rotation_Transform(&Trans,&Vect);
  Compose_Transforms(Object,&Trans);
}

// ----------------------------------------------------------------------------
// -- CREATE AN IDENTITY MATRIX -----------------------------------------------
// ----------------------------------------------------------------------------
void mat_identity (MATRIX mat) {
    register int i,j;

    for (i=0; i<4; i++)
    for (j=0; j<4; j++)
        mat[i][j]=0.0;

    for (i=0; i<4; i++)
    mat[i][i]=1.0;
}

// ----------------------------------------------------------------------------
// -- ROTATE A MATRIX ABOUT THE X,Y OR Z AXIS (RESULTAT DS MAT1) --------------
// ----------------------------------------------------------------------------
void mat_rotate(MATRIX mat1,MATRIX mat2,int axis,float angle) {
  MATRIX mat;
  float cosa,sina;

  cosa=cos (PIs180*angle);
  sina=sin (PIs180*angle);

  mat_identity (mat);

  switch (axis) {
  case _X:
      mat[1][1]=cosa;
      mat[1][2]=sina;
      mat[2][1]=-sina;
      mat[2][2]=cosa;
      break;

  case _Y:
      mat[0][0]=cosa;
      mat[0][2]=-sina;
      mat[2][0]=sina;
      mat[2][2]=cosa;
      break;

  case _Z:
      mat[0][0]=cosa;
      mat[0][1]=sina;
      mat[1][0]=-sina;
      mat[1][1]=cosa;
      break;
  }

  mat_mult(mat1,mat2,mat);
}

// ----------------------------------------------------------------------------
// -- ROTATE A MATRIX ABOUT THE VECTEUR AXIS ----------------------------------
// ----------------------------------------------------------------------------
void mat_axis_rotate (MATRIX mat1,MATRIX mat2,Vecteur axis,float angle) {
    float  cosa,sina;
    MATRIX mat;

    cosa=cos(PIs180*angle);
    sina=sin(PIs180*angle);

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

    mat_mult (mat1,mat2,mat);
}

// ---------------------------------------------------------------------------
// -- MAT1 <-- MAT2*MAT3 -----------------------------------------------------
// ---------------------------------------------------------------------------
void mat_mult(MATRIX mat1,MATRIX mat2,MATRIX mat3) {
    float sum;
    register int   i,j,k;
    MATRIX result;

    for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
        sum=0.0;

        for (k=0; k<4; k++)
        sum=sum+mat2[i][k]*mat3[k][j];

        result[i][j]=sum;
    }
    }

    for (i=0; i<4; i++)
    for (j=0; j<4; j++)
        mat1[i][j]=result[i][j];
}


// ----------------------------------------------------------------------------
// -- DECODES A 3X4 TRANSFORMATION MATRIX INTO SEPARATE SCALE,ROTATION, -------
// -- TRANSLATION,AND SHEAR VERTORRS. BASED ON A PROGRAM BY SPENCER W. ---------
// -- THOMAS (GRAPHICS GEMS II) -----------------------------------------------
// ----------------------------------------------------------------------------
void mat_decode (MATRIX mat,Vecteur scale,Vecteur shear,Vecteur rotate,Vecteur transl) {
    register int i;
    Vecteur row[3],temp;

    for (i=0; i<3; i++) transl[i]=mat[3][i];

    for (i=0; i<3; i++) {
      row[i][_X]=mat[i][0];
      row[i][_Y]=mat[i][1];
      row[i][_Z]=mat[i][2];
    }

    scale[_X]=vect_mag(row[0]);
    vect_normalize (row[0]);

    shear[_X]=vect_dot (row[0],row[1]);
    row[1][_X]=row[1][_X]-shear[_X]*row[0][_X];
    row[1][_Y]=row[1][_Y]-shear[_X]*row[0][_Y];
    row[1][_Z]=row[1][_Z]-shear[_X]*row[0][_Z];

    scale[_Y]=vect_mag (row[1]);
    vect_normalize (row[1]);

    if (scale[_Y]!=0.0) shear[_X] /= scale[_Y];

    shear[_Y]=vect_dot (row[0],row[2]);
    row[2][_X]=row[2][_X]-shear[_Y]*row[0][_X];
    row[2][_Y]=row[2][_Y]-shear[_Y]*row[0][_Y];
    row[2][_Z]=row[2][_Z]-shear[_Y]*row[0][_Z];

    shear[_Z]=vect_dot (row[1],row[2]);
    row[2][_X]=row[2][_X]-shear[_Z]*row[1][_X];
    row[2][_Y]=row[2][_Y]-shear[_Z]*row[1][_Y];
    row[2][_Z]=row[2][_Z]-shear[_Z]*row[1][_Z];

    scale[_Z]=vect_mag(row[2]);
    vect_normalize(row[2]);

    if (scale[_Z]!=0.0) {
    shear[_Y]/=scale[_Z];
    shear[_Z]/=scale[_Z];
    }

    vect_cross(temp,row[1],row[2]);

    if (vect_dot(row[0],temp)<0.0) {
      for (i=0; i<3; i++) {
        scale[i]  *= -1.0;
        row[i][_X] *= -1.0;
        row[i][_Y] *= -1.0;
        row[i][_Z] *= -1.0;
      }
    }

    if (row[0][_Z]<-1.0) row[0][_Z]=-1.0;
    if (row[0][_Z]>+1.0) row[0][_Z]=+1.0;

    rotate[_Y]=asin(-row[0][_Z]);

    if (cos(rotate[_Y])!=0) {
      rotate[_X]=atan2 (row[1][_Z],row[2][_Z]);
      rotate[_Z]=atan2 (row[0][_Y],row[0][_X]);
    } else {
      rotate[_X]=atan2 (row[1][_X],row[1][_Y]);
      rotate[_Z]=0.0;
    }

    /* Convert the rotations to degrees */
    rotate[_X]=(180.0/PI)*rotate[_X];
    rotate[_Y]=(180.0/PI)*rotate[_Y];
    rotate[_Z]=(180.0/PI)*rotate[_Z];
}

// ----------------------------------------------------------------------------
// MATRIX inversion code from Graphics Gems */
// mat1 <-- mat2^-1 */
// ----------------------------------------------------------------------------
float mat_inv (MATRIX mat1,MATRIX mat2) {
    register int i,j;
    float det;

    if (mat1!=mat2) {
    for (i=0; i<4; i++)
        for (j=0; j<4; j++)
        mat1[i][j]=mat2[i][j];
    }

    det=det4x4 (mat1);

    if (fabs (det)<EPSILON)
    return 0.0;

    adjoint (mat1);

    for (i=0; i<4; i++)
    for(j=0; j<4; j++)
        mat1[i][j]=mat1[i][j]/det;

    return det;
}


void adjoint (MATRIX mat) {
    float a1,a2,a3,a4,b1,b2,b3,b4;
    float c1,c2,c3,c4,d1,d2,d3,d4;

    a1=mat[0][0]; b1=mat[0][1];
    c1=mat[0][2]; d1=mat[0][3];

    a2=mat[1][0]; b2=mat[1][1];
    c2=mat[1][2]; d2=mat[1][3];

    a3=mat[2][0]; b3=mat[2][1];
    c3=mat[2][2]; d3=mat[2][3];

    a4=mat[3][0]; b4=mat[3][1];
    c4=mat[3][2]; d4=mat[3][3];

    /* row column labeling reversed since we transpose rows & columns */
    mat[0][0] = det3x3 (b2,b3,b4,c2,c3,c4,d2,d3,d4);
    mat[1][0] =-det3x3 (a2,a3,a4,c2,c3,c4,d2,d3,d4);
    mat[2][0] = det3x3 (a2,a3,a4,b2,b3,b4,d2,d3,d4);
    mat[3][0] =-det3x3 (a2,a3,a4,b2,b3,b4,c2,c3,c4);

    mat[0][1] =-det3x3 (b1,b3,b4,c1,c3,c4,d1,d3,d4);
    mat[1][1] = det3x3 (a1,a3,a4,c1,c3,c4,d1,d3,d4);
    mat[2][1] =-det3x3 (a1,a3,a4,b1,b3,b4,d1,d3,d4);
    mat[3][1] = det3x3 (a1,a3,a4,b1,b3,b4,c1,c3,c4);

    mat[0][2] = det3x3 (b1,b2,b4,c1,c2,c4,d1,d2,d4);
    mat[1][2] =-det3x3 (a1,a2,a4,c1,c2,c4,d1,d2,d4);
    mat[2][2] = det3x3 (a1,a2,a4,b1,b2,b4,d1,d2,d4);
    mat[3][2] =-det3x3 (a1,a2,a4,b1,b2,b4,c1,c2,c4);

    mat[0][3] =-det3x3 (b1,b2,b3,c1,c2,c3,d1,d2,d3);
    mat[1][3] = det3x3 (a1,a2,a3,c1,c2,c3,d1,d2,d3);
    mat[2][3] =-det3x3 (a1,a2,a3,b1,b2,b3,d1,d2,d3);
    mat[3][3] = det3x3 (a1,a2,a3,b1,b2,b3,c1,c2,c3);
}


float det4x4 (MATRIX mat) {
    float ans;
    float a1,a2,a3,a4,b1,b2,b3,b4,c1,c2,c3,c4,d1,d2,d3,d4;

    a1=mat[0][0]; b1=mat[0][1];
    c1=mat[0][2]; d1=mat[0][3];

    a2=mat[1][0]; b2=mat[1][1];
    c2=mat[1][2]; d2=mat[1][3];

    a3=mat[2][0]; b3=mat[2][1];
    c3=mat[2][2]; d3=mat[2][3];

    a4=mat[3][0]; b4=mat[3][1];
    c4=mat[3][2]; d4=mat[3][3];

    ans=a1*det3x3 (b2,b3,b4,c2,c3,c4,d2,d3,d4) -
      b1*det3x3 (a2,a3,a4,c2,c3,c4,d2,d3,d4) +
      c1*det3x3 (a2,a3,a4,b2,b3,b4,d2,d3,d4) -
      d1*det3x3 (a2,a3,a4,b2,b3,b4,c2,c3,c4);

    return ans;
}


float det3x3 (float a1,float a2,float a3,float b1,float b2,float b3,float c1,float c2,float c3) {
    float ans;

    ans=a1*det2x2 (b2,b3,c2,c3)
   -b1*det2x2 (a2,a3,c2,c3)
   +c1*det2x2 (a2,a3,b2,b3);

    return ans;
}


float det2x2 (float a,float b,float c,float d) {
    float ans;
    ans=a*d-b*c;
    return ans;
}
