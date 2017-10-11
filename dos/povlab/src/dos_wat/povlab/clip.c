/* ---------------------------------------------------------------------------
*  CLIP.C
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
#include "LIB.H"

struct EndpointStruct {
  int x1,y1;
  int x2,y2;
};

struct RegionStruct {
  int Xul;
  int Yul;
  int Xlr;
  int Ylr;
};

union OutcodeUnion {
  struct {
    unsigned code0:1;
    unsigned code1:1;
    unsigned code2:1;
    unsigned code3:1;
  } ocs;
  int outcodes;
};

#define X1 ep->x1
#define Y1 ep->y1
#define X2 ep->x2
#define Y2 ep->y2
#define XUL r->Xul
#define YUL r->Yul
#define XLR r->Xlr
#define YLR r->Ylr

void SetOutcodes(union OutcodeUnion *u, struct RegionStruct *r,int x,int y) {
  u->outcodes=0;
  u->ocs.code0=(x < XUL);
  u->ocs.code1=(y < YUL);
  u->ocs.code2=(x > XLR);
  u->ocs.code3=(y > YLR);
}

int Clip(struct EndpointStruct *ep,struct RegionStruct *r) {
  union OutcodeUnion ocu1,ocu2;
  int Inside;
  int Outside;

  SetOutcodes(&ocu1,r,X1,Y1);
  SetOutcodes(&ocu2,r,X2,Y2);

  Inside  = ((ocu1.outcodes | ocu2.outcodes)==0);
  Outside = ((ocu1.outcodes & ocu2.outcodes)!=0);

  while (!Outside && !Inside) {
    if (ocu1.outcodes==0) {
      swap_int(&X1,&X2);
      swap_int(&Y1,&Y2);
      swap_int(&ocu1,&ocu2);
    }

    if (ocu1.ocs.code0) {
      Y1+=(int)(Y2-Y1)*(XUL-X1)/(X2-X1);
      X1=XUL;
    }
    else if (ocu1.ocs.code1) {
      X1+=(int)(X2-X1)*(YUL-Y1)/(Y2-Y1);
      Y1=YUL;
    }
    else if (ocu1.ocs.code2) {
      Y1+=(int)(Y2-Y1)*(XLR-X1)/(X2-X1);
      X1=XLR;
    }
    else if (ocu1.ocs.code3) {
      X1+=(int)(X2-X1)*(YLR-Y1)/(Y2-Y1);
      Y1=YLR;
    }

    SetOutcodes(&ocu1,r,X1,Y1);
    Inside = ((ocu1.outcodes | ocu2.outcodes)==0);
    Outside= ((ocu1.outcodes & ocu2.outcodes)!=0);
  }

  return (Inside);
}

int do_clip(int *XL1,
            int *YL1,
            int *XL2,
            int *YL2,
            int XR1,
            int YR1,
            int XR2,
            int YR2) {
  register int R;
  struct EndpointStruct ep;
  struct RegionStruct r;

  ep.x1=*XL1;
  ep.y1=*YL1;
  ep.x2=*XL2;
  ep.y2=*YL2;

  r.Xul=XR1;
  r.Yul=YR1;
  r.Xlr=XR2;
  r.Ylr=YR2;

  R=Clip(&ep,&r);

  *XL1=ep.x1;
  *YL1=ep.y1;
  *XL2=ep.x2;
  *YL2=ep.y2;

  return (R);
}


