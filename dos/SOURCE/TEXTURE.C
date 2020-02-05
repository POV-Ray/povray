/****************************************************************************
*                texture.c
*
*  This module implements texturing functions such as noise, turbulence and
*  texture transformation functions. The actual texture routines are in the
*  files pigment.c & normal.c.
*  The noise function used here is the one described by Ken Perlin in
*  "Hypertexture", SIGGRAPH '89 Conference Proceedings page 253.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1998 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other 
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's GO POVRAY Forum or visit
*  http://www.povray.org. The latest version of POV-Ray may be found at these sites.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

/*
   Some texture ideas garnered from SIGGRAPH '85 Volume 19 Number 3, 
   "An Image Synthesizer" By Ken Perlin.
   Further Ideas Garnered from "The RenderMan Companion" (Addison Wesley)
*/

#include "frame.h"
#include "povray.h"
#include "vector.h"
#include "povproto.h"
#include "texture.h"
#include "image.h"
#include "matrices.h"
#include "normal.h"
#include "pigment.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* Ridiculously large scaling values */

#define MINX (-10000)
#define MINY MINX
#define MINZ MINX

#define SINTABSIZE 1000

#define REALSCALE (2.0 / 65535.0)

#define SCURVE(a) ((a)*(a)*(3.0-2.0*(a)))


/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

static DBL *sintab;
unsigned int Number_Of_Waves = 10;    /* dmf */
DBL *frequency;                       /* dmf */
VECTOR *Wave_Sources;                 /* dmf */

short *hashTable;

DBL RTable[267] =
{
         -1,    0.604974,   -0.937102,    0.414115,    0.576226,  -0.0161593,
   0.432334,    0.103685,    0.590539,   0.0286412,     0.46981,    -0.84622,
 -0.0734112,   -0.304097,    -0.40206,   -0.210132,   -0.919127,    0.652033,
   -0.83151,   -0.183948,   -0.671107,    0.852476,    0.043595,   -0.404532,
    0.75494,   -0.335653,    0.618433,    0.605707,    0.708583,   -0.477195,
   0.899474,    0.490623,    0.221729,   -0.400381,   -0.853727,   -0.932586,
   0.659113,    0.961303,    0.325948,   -0.750851,    0.842466,    0.734401,
  -0.649866,    0.394491,   -0.466056,   -0.434073,    0.109026,   0.0847028,
  -0.738857,    0.241505,     0.16228,    -0.71426,   -0.883665,   -0.150408,
   -0.90396,   -0.686549,   -0.785214,    0.488548,   0.0246433,    0.142473,
  -0.602136,    0.375845, -0.00779736,    0.498955,   -0.268147,    0.856382,
  -0.386007,   -0.596094,   -0.867735,   -0.570977,   -0.914366,     0.28896,
   0.672206,   -0.233783,     0.94815,    0.895262,    0.343252,   -0.173388,
  -0.767971,   -0.314748,    0.824308,   -0.342092,    0.721431,    -0.24004,
   -0.63653,    0.553277,    0.376272,    0.158984,   -0.452659,    0.396323,
  -0.420676,   -0.454154,    0.122179,    0.295857,   0.0664225,   -0.202075,
  -0.724788,    0.453513,    0.224567,   -0.908812,    0.176349,   -0.320516,
  -0.697139,    0.742702,   -0.900786,    0.471489,   -0.133532,    0.119127,
  -0.889769,    -0.23183,   -0.669673,   -0.046891,   -0.803433,   -0.966735,
   0.475578,   -0.652644,   0.0112459,   -0.730007,    0.128283,    0.145647,
  -0.619318,    0.272023,    0.392966,    0.646418,  -0.0207675,   -0.315908,
   0.480797,    0.535668,   -0.250172,    -0.83093,   -0.653773,   -0.443809,
   0.119982,   -0.897642,     0.89453,    0.165789,    0.633875,   -0.886839,
   0.930877,   -0.537194,    0.587732,    0.722011,   -0.209461,  -0.0424659,
  -0.814267,   -0.919432,    0.280262,    -0.66302,   -0.558099,   -0.537469,
  -0.598779,    0.929656,   -0.170794,   -0.537163,    0.312581,    0.959442,
   0.722652,    0.499931,    0.175616,   -0.534874,   -0.685115,    0.444999,
    0.17171,    0.108202,   -0.768704,   -0.463828,    0.254231,    0.546014,
   0.869474,    0.875212,   -0.944427,    0.130724,   -0.110185,    0.312184,
   -0.33138,   -0.629206,   0.0606546,    0.722866,  -0.0979477,    0.821561,
  0.0931258,   -0.972808,   0.0318151,   -0.867033,   -0.387228,    0.280995,
  -0.218189,   -0.539178,   -0.427359,   -0.602075,    0.311971,    0.277974,
   0.773159,    0.592493,  -0.0331884,   -0.630854,   -0.269947,    0.339132,
   0.581079,    0.209461,   -0.317433,   -0.284993,    0.181323,    0.341634,
   0.804959,   -0.229572,   -0.758907,   -0.336721,    0.605463,   -0.991272,
 -0.0188754,   -0.300191,    0.368307,   -0.176135,     -0.3832,   -0.749569,
    0.62356,   -0.573938,    0.278309,   -0.971313,    0.839994,   -0.830686,
   0.439078,     0.66128,    0.694514,   0.0565042,     0.54342,   -0.438804,
 -0.0228428,   -0.687068,    0.857267,    0.301991,   -0.494255,   -0.941039,
   0.775509,    0.410575,   -0.362081,   -0.671534,   -0.348379,    0.932433,
   0.886442,    0.868681,   -0.225666,   -0.062211,  -0.0976425,   -0.641444,
  -0.848112,    0.724697,    0.473503,    0.998749,    0.174701,    0.559625,
  -0.029099,   -0.337392,   -0.958129,   -0.659785,    0.236042,   -0.246937,
   0.659449,   -0.027512,    0.821897,   -0.226215,   0.0181735,    0.500481,
  -0.420127,   -0.427878,    0.566186
};

static unsigned long int next_rand = 1;



/*****************************************************************************
* Static functions
******************************************************************************/

static void InitTextureTable (void);
static TEXTURE *Copy_Materials (TEXTURE *Old);


/*****************************************************************************
*
* FUNCTION
*
*   Initialize_Noise()
*
* INPUT
*   
* OUTPUT
*
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Initialize_Noise()
{
  register unsigned int i;
  VECTOR point;

  InitTextureTable();

  sintab = (DBL *)POV_MALLOC(SINTABSIZE * sizeof(DBL), "sine table");

  /* dmf */
  frequency = (DBL *)POV_MALLOC(Number_Of_Waves * sizeof(DBL), "wave frequency table: use lower Number_Of_Waves");

  /* dmf */
  Wave_Sources = (VECTOR *)POV_MALLOC(Number_Of_Waves * sizeof(VECTOR), "wave sources table: use lower Number_Of_Waves");

  for (i = 0 ; i < SINTABSIZE ; i++)
  {
    sintab[i] = sin((DBL)i / SINTABSIZE * TWO_M_PI);
  }

  for (i = 0 ; i < Number_Of_Waves ; i++)
  {
    Make_Vector(point,(DBL)i,0.0,0.0);
    DNoise(point, point);
    VNormalize(Wave_Sources[i], point);
    frequency[i] = FRAND() + 0.01;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   InitTextureTable()
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void InitTextureTable()
{
  short int i, j, temp;

  POV_SRAND(0);

  hashTable = (short int *)POV_MALLOC(4096*sizeof(short int), "hash table");

  for (i = 0; i < 4096; i++)
  {
    hashTable[i] = i;
  }

  for (i = 4095; i >= 0; i--)
  {
    j = POV_RAND() % 4096;
    temp = hashTable[i];
    hashTable[i] = hashTable[j];
    hashTable[j] = temp;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Free_Noise_Tables()
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Free_Noise_Tables()
{
  if (sintab != NULL) 
  {
    POV_FREE(sintab);
    POV_FREE(hashTable);
    POV_FREE(frequency);
    POV_FREE(Wave_Sources);
    
    sintab       = NULL;
    hashTable    = NULL;
    frequency    = NULL;
    Wave_Sources = NULL;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Noise
*
* INPUT
*
*   EPoint -- 3-D point at which noise is evaluated
*
* OUTPUT
*   
* RETURNS
*
*   DBL noise value
*   
* AUTHOR
*
*   Robert Skinner based on Ken Perlin
*   
* DESCRIPTION
*
* CHANGES
*   Modified by AAC to ensure uniformly distributed clamped values
*   between 0 and 1.0...
*
******************************************************************************/

DBL Noise(VECTOR EPoint)
{
  DBL x, y, z;
  DBL *mp;
  long ix, iy, iz, jx, jy, jz;
  int ixiy_hash, ixjy_hash, jxiy_hash, jxjy_hash;
  
  DBL sx, sy, sz, tx, ty, tz;
  DBL sum;
  
  DBL x_ix, x_jx, y_iy, y_jy, z_iz, z_jz, txty, sxty, txsy, sxsy;
  
  Increase_Counter(stats[Calls_To_Noise]);
  
  x = EPoint[X]-MINX;
  y = EPoint[Y]-MINY;
  z = EPoint[Z]-MINZ;
  
  /* its equivalent integer lattice point. */
  ix = (long)x; iy = (long)y; iz = (long)z;
  jx = ix + 1; jy = iy + 1; jz = iz + 1;
  
  sx = SCURVE(x - ix); sy = SCURVE(y - iy); sz = SCURVE(z - iz);
  
  /* the complement values of sx,sy,sz */
  tx = 1.0 - sx; ty = 1.0 - sy; tz = 1.0 - sz;
  
  /*
  *  interpolate!
  */
  x_ix = x - ix;
  x_jx = x - jx;
  y_iy = y - iy;
  y_jy = y - jy;
  z_iz = z - iz;
  z_jz = z - jz;
  txty = tx * ty;
  sxty = sx * ty;
  txsy = tx * sy;
  sxsy = sx * sy;
  ixiy_hash = Hash2d(ix, iy);
  jxiy_hash = Hash2d(jx, iy);
  ixjy_hash = Hash2d(ix, jy);
  jxjy_hash = Hash2d(jx, jy);
  
  mp = &RTable[(int) Hash1d(ixiy_hash, iz) & 0xFF];
  sum = INCRSUMP(mp, (txty*tz), x_ix, y_iy, z_iz);
  
  mp = &RTable[(int) Hash1d(jxiy_hash, iz) & 0xFF];
  sum += INCRSUMP(mp, (sxty*tz), x_jx, y_iy, z_iz);
  
  mp = &RTable[(int) Hash1d(ixjy_hash, iz) & 0xFF];
  sum += INCRSUMP(mp, (txsy*tz), x_ix, y_jy, z_iz);
  
  mp = &RTable[(int) Hash1d(jxjy_hash, iz) & 0xFF];
  sum += INCRSUMP(mp, (sxsy*tz), x_jx, y_jy, z_iz);
  
  mp = &RTable[(int) Hash1d(ixiy_hash, jz) & 0xFF];
  sum += INCRSUMP(mp, (txty*sz), x_ix, y_iy, z_jz);
  
  mp = &RTable[(int) Hash1d(jxiy_hash, jz) & 0xFF];
  sum += INCRSUMP(mp, (sxty*sz), x_jx, y_iy, z_jz);

  mp = &RTable[(int) Hash1d(ixjy_hash, jz) & 0xFF];
  sum += INCRSUMP(mp, (txsy*sz), x_ix, y_jy, z_jz);
  
  mp = &RTable[(int) Hash1d(jxjy_hash, jz) & 0xFF];
  sum += INCRSUMP(mp, (sxsy*sz), x_jx, y_jy, z_jz);
  
  sum = sum + 0.5;                     /* range at this point -0.5 - 0.5... */
  
  if (sum < 0.0)
    sum = 0.0;
  if (sum > 1.0)
    sum = 1.0;
  
  return (sum);
}



/*****************************************************************************
*
* FUNCTION
*
*   DNoise
*
* INPUT
*
*   EPoint -- 3-D point at which noise is evaluated
*   
* OUTPUT
*
*   VECTOR result
*   
* RETURNS
*   
* AUTHOR
*
*   Robert Skinner based on Ken Perlin
*   
* DESCRIPTION
*   Vector-valued version of "Noise"
*
* CHANGES
*   Modified by AAC to ensure uniformly distributed clamped values
*   between 0 and 1.0...
*
******************************************************************************/

void DNoise(VECTOR result, VECTOR EPoint)
{
  DBL x, y, z;
  DBL *mp;
  long ix, iy, iz, jx, jy, jz;
  int ixiy_hash, ixjy_hash, jxiy_hash, jxjy_hash;
  DBL px, py, pz, s;
  DBL sx, sy, sz, tx, ty, tz;
  DBL txty, sxty, txsy, sxsy;
  
  Increase_Counter(stats[Calls_To_DNoise]);
  
  x = EPoint[X]-MINX;
  y = EPoint[Y]-MINY;
  z = EPoint[Z]-MINZ;
  
  /* its equivalent integer lattice point. */
  ix = (long)x; iy = (long)y; iz = (long)z;
  jx = ix + 1; jy = iy + 1; jz = iz + 1;
  
  sx = SCURVE(x - ix); sy = SCURVE(y - iy); sz = SCURVE(z - iz);
  
  /* the complement values of sx,sy,sz */
  tx = 1.0 - sx; ty = 1.0 - sy; tz = 1.0 - sz;
  
  /*
  *  interpolate!
  */
  txty = tx * ty;
  sxty = sx * ty;
  txsy = tx * sy;
  sxsy = sx * sy;
  ixiy_hash = Hash2d(ix, iy);
  jxiy_hash = Hash2d(jx, iy);
  ixjy_hash = Hash2d(ix, jy);
  jxjy_hash = Hash2d(jx, jy);
  
  mp = &RTable[(int) Hash1d(ixiy_hash, iz) & 0xFF];
  px = x - ix;  py = y - iy;  pz = z - iz;
  s = txty*tz;
  result[X] = INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Y] = INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Z] = INCRSUMP(mp, s, px, py, pz);
  
  mp = &RTable[(int) Hash1d(jxiy_hash, iz) & 0xFF];
  px = x - jx;
  s = sxty*tz;
  result[X] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Y] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Z] += INCRSUMP(mp, s, px, py, pz);
  
  mp = &RTable[(int) Hash1d(jxjy_hash, iz) & 0xFF];
  py = y - jy;
  s = sxsy*tz;
  result[X] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Y] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Z] += INCRSUMP(mp, s, px, py, pz);
  
  mp = &RTable[(int) Hash1d(ixjy_hash, iz) & 0xFF];
  px = x - ix;
  s = txsy*tz;
  result[X] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Y] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Z] += INCRSUMP(mp, s, px, py, pz);
  
  mp = &RTable[(int) Hash1d(ixjy_hash, jz) & 0xFF];
  pz = z - jz;
  s = txsy*sz;
  result[X] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Y] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Z] += INCRSUMP(mp, s, px, py, pz);
  
  mp = &RTable[(int) Hash1d(jxjy_hash, jz) & 0xFF];
  px = x - jx;
  s = sxsy*sz;
  result[X] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Y] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Z] += INCRSUMP(mp, s, px, py, pz);
  
  mp = &RTable[(int) Hash1d(jxiy_hash, jz) & 0xFF];
  py = y - iy;
  s = sxty*sz;
  result[X] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Y] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Z] += INCRSUMP(mp, s, px, py, pz);
  
  mp = &RTable[(int) Hash1d(ixiy_hash, jz) & 0xFF];
  px = x - ix;
  s = txty*sz;
  result[X] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Y] += INCRSUMP(mp, s, px, py, pz);
  mp += 4;
  result[Z] += INCRSUMP(mp, s, px, py, pz);
}



/*****************************************************************************
*
* FUNCTION
*
*   Turbulence
*
* INPUT
*
*   EPoint -- Point at which turb is evaluated.
*   Turb   -- Parameters for fbm calculations.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL result
*   
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Computes a Fractal Brownian Motion turbulence value
*                 using repeated calls to a Perlin Noise function.
*
* CHANGES
*   ??? ???? : Updated with varible Octaves, Lambda, & Omega by [DMF]
*
******************************************************************************/

DBL Turbulence(VECTOR EPoint,TURB *Turb)
{
  int i;
  DBL Lambda, Omega, l, o, value;
  VECTOR temp;
  int Octaves=Turb->Octaves;
  
  value = Noise(EPoint);

  l = Lambda = Turb->Lambda;
  o = Omega  = Turb->Omega;

  for (i = 2; i <= Octaves; i++)
  {
    VScale(temp,EPoint,l);
    value += o * Noise(temp);
    if (i < Octaves)
    {
      l *= Lambda;
      o *= Omega;
    }
  }
  return (value);
}



/*****************************************************************************
*
* FUNCTION
*
*   DTurbulence
*
* INPUT
*
*   EPoint -- Point at which turb is evaluated.
*   Turb   -- Parameters for fmb calculations.
*   
* OUTPUT
*
*   result -- Vector valued turbulence
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION   : Computes a Fractal Brownian Motion turbulence value
*                 using repeated calls to a Perlin DNoise function.
*
* CHANGES
*   ??? ???? : Updated with varible Octaves, Lambda, & Omega by [DMF]
*
******************************************************************************/


void DTurbulence(VECTOR result, VECTOR  EPoint, TURB *Turb)
{
  DBL Omega, Lambda;
  int i;
  DBL l, o;
  VECTOR value, temp;
  int Octaves=Turb->Octaves;
  
  result[X] = result[Y] = result[Z] = 0.0;
  value[X]  = value[Y]  = value[Z]  = 0.0;
  
  DNoise(result, EPoint);
  
  l = Lambda = Turb->Lambda;
  o = Omega  = Turb->Omega;

  for (i = 2; i <= Octaves; i++)
  {
    VScale(temp,EPoint,l);
    
    DNoise(value, temp);
    result[X] += o * value[X];
    result[Y] += o * value[Y];
    result[Z] += o * value[Z];
    if (i < Octaves)
    {
      l *= Lambda;
      o *= Omega;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   cycloidal
*
* INPUT
*
*   DBL value
*   
* OUTPUT
*   
* RETURNS
*
*   DBL result
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL cycloidal(DBL value)
{
  register int indx;
  
  if (value >= 0.0)
  {
    indx = (int)((value - floor(value)) * SINTABSIZE);
    return (sintab [indx]);
  }
  else
  {
    indx = (int)((0.0 - (value + floor(0.0 - value))) * SINTABSIZE);
    return (0.0 - sintab [indx]);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Triangle_Wave
*
* INPUT
*
*   DBL value
*   
* OUTPUT
*   
* RETURNS
*
*   DBL result
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL Triangle_Wave(DBL value)
{
  register DBL offset;
  
  if (value >= 0.0) 
  {
    offset = value - floor(value);
  }
  else
  {
    offset = value + 1.0 + floor(fabs(value));
  }
  if (offset >= 0.5) 
  {
    return (2.0 * (1.0 - offset));
  }
  else
  {
    return (2.0 * offset);
  }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Transform_Textures(TEXTURE *Textures, TRANSFORM *Trans)
{
  TEXTURE *Layer;

  for (Layer = Textures; Layer != NULL; Layer = (TEXTURE *)Layer->Next)
  {
    if (Layer->Type == PLAIN_PATTERN)
    {
      Transform_Tpattern((TPATTERN *)Layer->Pigment, Trans);
      Transform_Tpattern((TPATTERN *)Layer->Tnormal, Trans);
    }
    else
    {
      Transform_Tpattern((TPATTERN *)Layer, Trans);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

FINISH *Create_Finish()
{
  FINISH *New;
  
  New = (FINISH *)POV_MALLOC(sizeof (FINISH), "finish");
  
  Make_RGB(New->Ambient, 0.1, 0.1, 0.1);
  Make_RGB(New->Reflection, 0.0, 0.0, 0.0);

  New->Diffuse    = 0.6;
  New->Brilliance = 1.0;
  New->Phong      = 0.0;
  New->Phong_Size = 40.0;
  New->Specular   = 0.0;
  New->Roughness  = 1.0 / 0.05;

  New->Crand = 0.0;

  New->Metallic = 0.0;

  New->Irid                = 0.0;
  New->Irid_Film_Thickness = 0.0;
  New->Irid_Turb           = 0.0;
  New->Temp_Caustics = -1.0;
  New->Temp_IOR     = -1.0;
  New->Temp_Refract =  1.0;
  New->Reflect_Exp  =  1.0;

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

FINISH *Copy_Finish(FINISH *Old)
{
  FINISH *New;
  
  if (Old != NULL)
  {
    New = Create_Finish();
    *New = *Old;
  }
  else
    New = NULL;
  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TEXTURE *Create_Texture()
{
  TEXTURE *New;
  
  New = (TEXTURE *)POV_MALLOC(sizeof (TEXTURE), "texture");
  
  Init_TPat_Fields((TPATTERN *)New);

  New->References = 1;

  New->Type  = PLAIN_PATTERN;
  New->Flags = NO_FLAGS;

  New->Pigment = NULL;
  New->Tnormal = NULL;
  New->Finish  = NULL;

  New->Next          = NULL;
  New->Next_Material = NULL;

  return (New);
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TEXTURE *Copy_Texture_Pointer(TEXTURE *Texture)
{
  if (Texture != NULL)
  {
    Texture->References++;
  }

  return(Texture);
}




/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TEXTURE *Copy_Textures(TEXTURE *Textures)
{
  TEXTURE *New, *First, *Previous, *Layer;
  
  Previous = First = NULL;
  
  for (Layer = Textures; Layer != NULL; Layer = (TEXTURE *)Layer->Next)
  {
    New = Create_Texture();
    Copy_TPat_Fields ((TPATTERN *)New, (TPATTERN *)Layer);
    
    /*  Mesh copies a texture pointer that already has multiple
        references.  We just want a clean copy, not a copy
        that's multply referenced.
     */

    New->References = 1;

    switch (Layer->Type)
    {
      case PLAIN_PATTERN:
        New->Pigment = Copy_Pigment(Layer->Pigment);
        New->Tnormal = Copy_Tnormal(Layer->Tnormal);
        New->Finish  = Copy_Finish(Layer->Finish);

        break;
      
      case BITMAP_PATTERN:

        New->Materials   = Copy_Materials(Layer->Materials);
        New->Num_Of_Mats = Layer->Num_Of_Mats;

/*      Not needed. Copied by Copy_TPat_Fields */
/*      New->Vals.Image  = Copy_Image(Layer->Vals.Image);*/ 

        break;
    }

    if (First == NULL)
    {
      First = New;
    }

    if (Previous != NULL)
    {
      Previous->Next = (TPATTERN *)New;
    }

    Previous = New;
  }

  return (First);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static TEXTURE *Copy_Materials(TEXTURE *Old)
{
  TEXTURE *New, *First, *Previous, *Material;
  
  Previous = First = NULL;
  
  for (Material = Old; Material != NULL; Material = Material->Next_Material)
  {
    New = Copy_Textures(Material);

    if (First == NULL)
    {
      First = New;
    }

    if (Previous != NULL)
    {
      Previous->Next_Material = New;
    }

    Previous = New;
  }

  return (First);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Destroy_Textures(TEXTURE *Textures)
{
  TEXTURE *Layer = Textures;
  TEXTURE *Mats;
  TEXTURE *Temp;
  
  if ((Textures == NULL) || (--(Textures->References) > 0))
  {
    return;
  }

  while (Layer != NULL)
  {
    Mats = Layer->Next_Material;

    while (Mats != NULL)
    {
      Temp = Mats->Next_Material;
      Mats->Next_Material = NULL;
      Destroy_Textures(Mats);
      Mats = Temp;
    }

    Destroy_TPat_Fields((TPATTERN *)Layer);

    switch (Layer->Type)
    {
      case PLAIN_PATTERN:

        Destroy_Pigment(Layer->Pigment);
        Destroy_Tnormal(Layer->Tnormal);
        Destroy_Finish(Layer->Finish);

      break;


      case BITMAP_PATTERN:

        Destroy_Textures(Layer->Materials);
        /*taken care of by Destroy_TPat_Fields*/
        /*Destroy_Image(Layer->Vals.Image);*/

      break;
    }

    Temp = (TEXTURE *)Layer->Next;
    POV_FREE(Layer);
    Layer = Temp;
  }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Post_Textures(TEXTURE *Textures)
{
  TEXTURE *Layer, *Material;
  int i;
  BLEND_MAP *Map;
  
  if (Textures == NULL)
  {
    return;
  }

  for (Layer = Textures; Layer != NULL; Layer = (TEXTURE *)Layer->Next)
  {
    if (!((Layer->Flags) & POST_DONE))
    {
      switch (Layer->Type)
      {
        case PLAIN_PATTERN:

          Post_Pigment(Layer->Pigment);
          Post_Tnormal(Layer->Tnormal);

          break;

        case BITMAP_PATTERN:

          for (Material = Layer->Materials; Material != NULL; Material = Material->Next_Material)

            Post_Textures(Material);

            break;
      }
  
      if ((Map=Layer->Blend_Map) != NULL)
      {
        for (i = 0; i < Map->Number_Of_Entries; i++)
        {
           Post_Textures(Map->Blend_Map_Entries[i].Vals.Texture);
        }
      }
      else
      {
        if (Layer->Type == AVERAGE_PATTERN)
        {
           Error("No texture map in averaged texture.");
        }
      }
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Test_Opacity
*
* INPUT
*
*   Object - Pointer to object
*
* OUTPUT
*
* RETURNS
*
*   int - TRUE, if opaque
*
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Test wether an object is opaque or not, i.e. wether the texture contains
*   a non-zero filter or alpha channel.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
*   Oct 1994 : Added code to check for opaque image maps. [DB]
*
*   Jun 1995 : Added code to check for alpha channel image maps. [DB]
*
******************************************************************************/

int Test_Opacity(TEXTURE *Texture)
{
  int x, y;
  int Opaque, Help;
  IMAGE *Image;
  TEXTURE *Layer, *Material;

  if (Texture == NULL)
  {
    return(FALSE);
  }

  /* We assume that the object is not opaque. */

  Opaque = FALSE;

  /* Test all layers. If at least one layer is opaque the object is opaque. */

  for (Layer = Texture; Layer != NULL; Layer = (TEXTURE *)Layer->Next)
  {
    switch (Layer->Type)
    {
      case PLAIN_PATTERN:

        /* Test image map for opacity. */

        if ((Layer->Pigment->Type == BITMAP_PATTERN) &&
            (Layer->Pigment->Vals.Image != NULL))
        {
          /* Layer is not opaque if the image map is used just once. */

          if (Layer->Pigment->Vals.Image->Once_Flag)
          {
            break;
          }

          /* Layer is not opaque if there's at least one non-opaque color. */

          Image = Layer->Pigment->Vals.Image;

          Help = FALSE;

          if (Image->Colour_Map != NULL)
          {
            /* Test color map. */

            for (x = 0; x < (int)Image->Colour_Map_Size; x++)
            {
              if (fabs(Image->Colour_Map[x].Filter) > EPSILON)
              {
                Help = TRUE;

                break;
              }
            }
          }
          else
          {
            /* Test image. */

            if (Image->data.rgb_lines[0].transm != NULL)
            {
              for (y = 0; y < Image->iheight; y++)
              {
                for (x = 0; x < Image->iwidth; x++)
                {
                  if (fabs(Image->data.rgb_lines[y].transm[x]) > EPSILON)
                  {
                    Help = TRUE;

                    break;
                  }
                }

                if (Help)
                {
                  break;
                }
              }
            }
          }

          if (Help)
          {
            break;
          }
        }

        if (!(Layer->Pigment->Flags & HAS_FILTER))
        {
          Opaque = TRUE;
        }

        break;

      case BITMAP_PATTERN:

        /* Layer is not opaque if the image map is used just once. */

        if (Layer->Vals.Image != NULL)
        {
          if (Layer->Vals.Image->Once_Flag)
          {
            break;
          }
        }

        /* Layer is opaque if all materials are opaque. */

        Help = TRUE;

        for (Material = Layer->Materials; Material != NULL; Material = Material->Next_Material)
        {
          if (!Test_Opacity(Material))
          {
            /* Material is not opaque --> layer is not opaque. */

            Help = FALSE;

            break;
          }
        }

        if (Help)
        {
          Opaque = TRUE;
        }

        break;
    }
  }

  return(Opaque);
}



/*****************************************************************************
*
* FUNCTION
*
*   POV_RAND
*
* INPUT
*
* OUTPUT
*   
* RETURNS
*
*   int - random value
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Standard pseudo-random function.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

int POV_RAND()
{
  next_rand = next_rand * 1812433253L + 12345L;

  return((int)(next_rand >> 16) & RNDMASK);
}

int POV_GET_OLD_RAND()
{
  return(next_rand);
}



/*****************************************************************************
*
* FUNCTION
*
*   POV_SRAND
*
* INPUT
*
*   seed - Pseudo-random generator start value
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Set start value for pseudo-random generator.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

void POV_SRAND(int seed)
{
  next_rand = (unsigned long int)seed;
}

