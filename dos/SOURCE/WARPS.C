/**************************************************************************
*                warps.c
*
*  This module implements functions that warp or modify the point at which
*  a texture pattern is evaluated.
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

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "matrices.h"
#include "warps.h"
#include "pattern.h"
#include "texture.h"

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define COORDINATE_LIMIT 1.0e17


/*****************************************************************************
* Static functions
******************************************************************************/



/*****************************************************************************
*
* FUNCTION
*
*   Warp_EPoint
*
* INPUT
*
*   EPoint -- The original point in 3d space at which a pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*   
* OUTPUT
*
*   TPoint -- Point after turbulence and transform
*   have been applied
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

void Warp_EPoint (VECTOR TPoint, VECTOR EPoint, TPATTERN *TPat)
{
   VECTOR PTurbulence,RP;
   int Axis,i,temp_rand;
   int blockX = 0, blockY = 0, blockZ = 0 ;
   SNGL BlkNum;
   DBL  Length;
   DBL  Strength;
   WARP *Warp=TPat->Warps;
   TURB *Turb;
   TRANS *Tr;
   REPEAT *Repeat;
   BLACK_HOLE *Black_Hole;
   VECTOR Delta, Center;

   Assign_Vector(TPoint, EPoint);

   while (Warp != NULL)
   {
      switch(Warp->Warp_Type)
      {
        case CLASSIC_TURB_WARP:
          if ((TPat->Type == MARBLE_PATTERN) ||
              (TPat->Type == NO_PATTERN)     ||
              (TPat->Type == WOOD_PATTERN))
          {
             break;
          }
        /* If not a special type, fall through to next case */

        case EXTRA_TURB_WARP:
          Turb=(TURB *)Warp;
          DTurbulence (PTurbulence, TPoint, Turb);
          TPoint[X] += PTurbulence[X] * Turb->Turbulence[X];
          TPoint[Y] += PTurbulence[Y] * Turb->Turbulence[Y];
          TPoint[Z] += PTurbulence[Z] * Turb->Turbulence[Z];
          break;

        case NO_WARP:
          break;

        case TRANSFORM_WARP:
          Tr=(TRANS *)Warp;
          MInvTransPoint(TPoint, TPoint, &(Tr->Trans));
          break;

        case REPEAT_WARP:
          Repeat=(REPEAT *)Warp;
          Assign_Vector(RP,TPoint);
          Axis=Repeat->Axis;
          BlkNum=floor(TPoint[Axis]/Repeat->Width);
          
          RP[Axis]=TPoint[Axis]-BlkNum*Repeat->Width;
          
          if (((int)BlkNum) & 1)
          {          
             VEvaluateEq(RP,Repeat->Flip);
             if ( Repeat->Flip[Axis] < 0 ) 
             {
                RP[Axis] = Repeat->Width+RP[Axis];
             }
          }

          VAddScaledEq(RP,BlkNum,Repeat->Offset);
          Assign_Vector(TPoint,RP);
          break;

        case BLACK_HOLE_WARP:
          Black_Hole = (BLACK_HOLE *) Warp ;
          Assign_Vector (Center, Black_Hole->Center) ;

          if (Black_Hole->Repeat)
          {
            /* first, get the block number we're in for each dimension  */
            /* block numbers are (currently) calculated relative to 0   */
            /* we use floor () since it correctly returns -1 for the
               first block below 0 in each axis                         */
            /* one final point - we could run into overflow problems if
               the repeat vector was small and the scene very large.    */
            if (Black_Hole->Repeat_Vector [X] >= Small_Tolerance)
              blockX = (int) floor (TPoint [X] / Black_Hole->Repeat_Vector [X]) ;

            if (Black_Hole->Repeat_Vector [Y] >= Small_Tolerance)
              blockY = (int) floor (TPoint [Y] / Black_Hole->Repeat_Vector [Y]) ;

            if (Black_Hole->Repeat_Vector [Z] >= Small_Tolerance)
              blockZ = (int) floor (TPoint [Z] / Black_Hole->Repeat_Vector [Z]) ;

            if (Black_Hole->Uncertain)
            {
              /* if the position is uncertain calculate the new one first */
              /* this will allow the same numbers to be returned by frand */
              
              temp_rand = POV_GET_OLD_RAND(); /*protect seed*/
  
              POV_SRAND (Hash3d (blockX, blockY, blockZ)) ;
              Center [X] += FRAND () * Black_Hole->Uncertainty_Vector [X] ;
              Center [Y] += FRAND () * Black_Hole->Uncertainty_Vector [Y] ;
              Center [Z] += FRAND () * Black_Hole->Uncertainty_Vector [Z] ;
              POV_SRAND (temp_rand) ;  /*restore*/
            }

            Center [X] += Black_Hole->Repeat_Vector [X] * blockX ;
            Center [Y] += Black_Hole->Repeat_Vector [Y] * blockY ;
            Center [Z] += Black_Hole->Repeat_Vector [Z] * blockZ ;
          }

          VSub (Delta, TPoint, Center) ;
          VLength (Length, Delta) ;

          /* Length is the distance from the centre of the black hole */
          if (Length >= Black_Hole->Radius) break ;

          if (Black_Hole->Type == 0)
          {
            /* now convert the length to a proportion (0 to 1) that the point
               is from the edge of the black hole. a point on the perimeter
               of the black hole will be 0.0 ; a point at the centre will be
               1.0 ; a point exactly halfway will be 0.5, and so forth. */
            Length = (Black_Hole->Radius - Length) / Black_Hole->Radius ;

            /* Strength is the magnitude of the transformation effect. firstly,
               apply the Power variable to Length. this is meant to provide a
               means of controlling how fast the power of the Black Hole falls
               off from its centre. if Power is 2.0, then the effect is inverse
               square. increasing power will cause the Black Hole to be a lot
               weaker in its effect towards its perimeter. 
               
               finally we multiply Strength with the Black Hole's Strength
               variable. if the resultant value exceeds 1.0 we clip it to 1.0.
               this means a point will never be transformed by more than its
               original distance from the centre. the result of this clipping
               is that you will have an 'exclusion' area near the centre of
               the black hole where all points whose final value exceeded or
               equalled 1.0 were moved by a fixed amount. this only happens
               if the Strength value of the Black Hole was greater than one. */

            Strength = pow (Length, Black_Hole->Power) * Black_Hole->Strength ;
            if (Strength > 1.0) Strength = 1.0 ;
            
            /* if the Black Hole is inverted, it gives the impression of 'push-
               ing' the pattern away from its centre. otherwise it sucks. */
            VScaleEq (Delta, Black_Hole->Inverted ? -Strength : Strength) ;

            /* add the scaled Delta to the input point to end up with TPoint. */
            VAddEq (TPoint, Delta) ;
          }
          break;
          
        default:
          Error("Warp type %d not yet implemented",Warp->Warp_Type);
      }
      Warp=Warp->Next_Warp;
   }

   for (i=X; i<=Z; i++)
     if (TPoint[i] > COORDINATE_LIMIT)
       TPoint[i]= COORDINATE_LIMIT;
     else
       if (TPoint[i] < -COORDINATE_LIMIT)
         TPoint[i] = -COORDINATE_LIMIT;

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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

WARP *Create_Warp (int Warp_Type)
{
 WARP *New;
 TURB *TNew;
 REPEAT *RNew;
 TRANS *TRNew;
 BLACK_HOLE *BNew;
   
 New = NULL;

 switch (Warp_Type)
 {
   case CLASSIC_TURB_WARP:
   case EXTRA_TURB_WARP:
     
     TNew = (TURB *)POV_MALLOC(sizeof(TURB),"turbulence struct");

     Make_Vector(TNew->Turbulence,0.0,0.0,0.0);

     TNew->Octaves = 6;
     TNew->Omega = 0.5;
     TNew->Lambda = 2.0;

     New = (WARP *)TNew;

     break;
     
   case REPEAT_WARP:

     RNew = (REPEAT *)POV_MALLOC(sizeof(REPEAT),"repeat warp");

     RNew->Axis = -1;
     RNew->Width = 0.0;

     Make_Vector(RNew->Offset,0.0,0.0,0.0);
     Make_Vector(RNew->Flip,1.0,1.0,1.0);

     New = (WARP *)RNew;

     break;

   case BLACK_HOLE_WARP:
     BNew = (BLACK_HOLE *)POV_MALLOC (sizeof (BLACK_HOLE), "black hole warp") ;
     Make_Vector (BNew->Center, 0.0, 0.0, 0.0) ;
     Make_Vector (BNew->Repeat_Vector, 0.0, 0.0, 0.0) ;
     Make_Vector (BNew->Uncertainty_Vector, 0.0, 0.0, 0.0) ;
     BNew->Strength = 1.0 ;
     BNew->Power = 2.0 ;
     BNew->Radius = 1.0 ;
     BNew->Radius_Squared = 1.0 ;
     BNew->Inverse_Radius = 1.0 ;
     BNew->Inverted = FALSE ;
     BNew->Type = 0 ;
     BNew->Repeat = FALSE ;
     BNew->Uncertain = FALSE ;
     New = (WARP *) BNew ;
     break ;

   case TRANSFORM_WARP:

     TRNew = (TRANS *)POV_MALLOC(sizeof(TRANS),"pattern transform");

     MIdentity (TRNew->Trans.matrix);
     MIdentity (TRNew->Trans.inverse);

     New = (WARP *)TRNew;

     break;
     
   default:

     Error("Unknown Warp type %d.",Warp_Type);
  }
  
  New->Warp_Type = Warp_Type;
  New->Next_Warp = NULL;
  
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Destroy_Warps (WARP *Warps)
{
 WARP *Temp1 = Warps;
 WARP *Temp2;

 while (Temp1!=NULL)
 {
   Temp2 = Temp1->Next_Warp;

   POV_FREE(Temp1);
   
   Temp1 = Temp2;
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

WARP *Copy_Warps (WARP *Old)
{
  WARP *New;

  if (Old != NULL)
  {
    New=Create_Warp(Old->Warp_Type);

    switch (Old->Warp_Type)
    {
       case CLASSIC_TURB_WARP:
       case EXTRA_TURB_WARP:
         memcpy(New,Old,sizeof(TURB));
         break;
     
       case REPEAT_WARP:
         memcpy(New,Old,sizeof(REPEAT));
         break;
     
       case BLACK_HOLE_WARP:
         memcpy(New,Old,sizeof(BLACK_HOLE));
         break;
     
       case TRANSFORM_WARP:
         memcpy(New,Old,sizeof(TRANS));
         break;
    }
    New->Next_Warp=Copy_Warps(Old->Next_Warp);
  }
  else
  {
    New=NULL;
  }
  return(New);
}

