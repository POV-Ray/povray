/* ---------------------------------------------------------------------------
*  RGB_HSL.C
*
*  A Fast HSL-to-RGB Transform
*  by Ken Fishkin
*  from "Graphics Gems", Academic Press, 1990
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
#include <STDIO.H>
#include "GLOBAL.H"
#include "LIB.H"

#define MIN(A,B) ((A) < (B) ? (A) : (B))
#define MAX(A,B) ((A) > (B) ? (A) : (B))

// -------------------------------------------------------------------------
// -- RGB-HSL TRANSFORMS. --------------------------------------------------
// -- KEN FISHKIN, PIXAR INC., JANUARY 1989. -------------------------------
// -- GIVEN R,G,B ON [0 ... 1], --------------------------------------------
// -- RETURN (H,S,L) ON [0 ... 1] ------------------------------------------
// -------------------------------------------------------------------------
void RGB_to_HSL  (DBL r,DBL g,DBL b,DBL *h,DBL *s,DBL *l) {
  DBL v;
  DBL m;
  DBL vm;
  DBL r2, g2, b2;

  v = MAX(r,g);
  v = MAX(v,b);
  m = MIN(r,g);
  m = MIN(m,b);

  if ((*l = (m + v) / 2.0) <= 0.0) return;
  if ((*s = vm = v - m) > 0.0) {
    *s /= (*l <= 0.5) ? (v + m ) : (2.0 - v - m) ;
  } else return;


  r2 = (v - r) / vm;
  g2 = (v - g) / vm;
  b2 = (v - b) / vm;

  if (r == v)
    *h = (g == m ? 5.0 + b2 : 1.0 - g2);
  else if (g == v)
    *h = (b == m ? 1.0 + r2 : 3.0 - b2);
  else
    *h = (r == m ? 3.0 + g2 : 5.0 - r2);

  *h /= 6;
}

// ----------------------------------------------------------------------------
// -- HSL-RGB TRANSFORMS. -----------------------------------------------------
// -- GIVEN H,S,L ON [0..1], --------------------------------------------------
// -- RETURN R,G,B ON [0..1] --------------------------------------------------
// ----------------------------------------------------------------------------
void HSL_to_RGB(DBL h,DBL sl,DBL l,DBL *r,DBL *g,DBL *b) {
  DBL v;

  v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);

  if (v <= 0) {
    *r = *g = *b = 0.0;
  } else {
    DBL m;
    DBL sv;
    int sextant;
    DBL fract, vsf, mid1, mid2;

    m = l + l - v;
    sv = (v - m ) / v;
    h *= 6.0;
    sextant = h;
    fract = h - sextant;
    vsf = v * sv * fract;
    mid1 = m + vsf;
    mid2 = v - vsf;
    switch (sextant) {
      case 0: *r = v; *g = mid1; *b = m; break;
      case 1: *r = mid2; *g = v; *b = m; break;
      case 2: *r = m; *g = v; *b = mid1; break;
      case 3: *r = m; *g = mid2; *b = v; break;
      case 4: *r = mid1; *g = m; *b = v; break;
      case 5: *r = v; *g = m; *b = mid2; break;
    }
  }
}

