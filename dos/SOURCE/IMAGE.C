/****************************************************************************
*                image.c
*
*  This module implements the mapped textures including image map, bump map
*  and material map.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1998,1998 Persistence of Vision Team
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
#include "texture.h"
#include "image.h"
#include "matrices.h"
#include "povray.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int cylindrical_image_map (VECTOR EPoint, IMAGE * Image, DBL *u, DBL *v);
static int torus_image_map (VECTOR EPoint, IMAGE * Image, DBL *u, DBL *v);
static int spherical_image_map (VECTOR EPoint, IMAGE * Image, DBL *u, DBL *v);
static int planar_image_map (VECTOR EPoint, IMAGE * Image, DBL *u, DBL *v);
static void no_interpolation (IMAGE * Image, DBL xcoor, DBL ycoor, COLOUR colour, int *index);
static DBL bilinear (DBL *corners, DBL x, DBL y);
static DBL norm_dist (DBL *corners, DBL x, DBL y);
static void Interp (IMAGE * Image, DBL xcoor, DBL ycoor, COLOUR colour, int *index);
static void image_colour_at (IMAGE * Image, DBL xcoor, DBL ycoor, COLOUR colour, int *index);
static int map (VECTOR EPoint, TPATTERN * Turb, DBL *xcoor, DBL *ycoor);

/*
 * 2-D to 3-D Procedural Texture Mapping of a Bitmapped Image onto an Object:
 * 
 * A. Simplistic (planar) method of image projection devised by DKB and AAC:
 * 
 * 1. Transform texture in 3-D space if requested. 2. Determine local object 2-d
 * coords from 3-d coords by <X Y Z> triple. 3. Return pixel color value at
 * that position on the 2-d plane of "Image". 3. Map colour value in Image
 * [0..255] to a more normal colour range [0..1].
 * 
 * B. Specialized shape projection variations by Alexander Enzmann:
 * 
 * 1. Cylindrical mapping 2. Spherical mapping 3. Torus mapping
 */

/*****************************************************************************
*
* FUNCTION
*
*   image_map
*
* INPUT
*
*   EPoint   -- 3-D point at which function is evaluated
*   Pigment  -- Pattern containing various parameters
*
* OUTPUT
*
*   Colour   -- color at EPoint
*
* RETURNS
*
*   int - TRUE,  if current point on the image map
*         FALSE, if current point is not on the image map
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Determines color of a 3-D point from a 2-D bitmap
*
* CHANGES
*
******************************************************************************/

int image_map(VECTOR EPoint, PIGMENT *Pigment, COLOUR colour)
{
  int reg_number;
  DBL xcoor = 0.0, ycoor = 0.0;

  /* If outside map coverage area, return clear */

  if (map(EPoint, ((TPATTERN *) Pigment), &xcoor, &ycoor))
  {
    Make_ColourA(colour, 1.0, 1.0, 1.0, 0.0, 1.0);

    return(FALSE);
  }
  else
  {
    image_colour_at(Pigment->Vals.Image, xcoor, ycoor, colour, &reg_number);
  }

  return(TRUE);
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
*   Very different stuff than the other routines here. This routine takes an
*   intersection point and a texture and returns a new texture based on the
*   index/color of that point in an image/materials map. CdW 7/91
*
* CHANGES
*
******************************************************************************/

TEXTURE *material_map(VECTOR EPoint, TEXTURE *Texture)
{
  int reg_number = 0;
  int Material_Number;
  int numtex;
  DBL xcoor = 0.0, ycoor = 0.0;
  COLOUR colour;
  TEXTURE *Temp_Tex;

  /*
   * Now we have transformed x, y, z we use image mapping routine to determine
   * texture index.
   */

  if (map(EPoint, ((TPATTERN *) Texture), &xcoor, &ycoor))
  {
    Material_Number = 0;
  }
  else
  {
    Make_ColourA(colour, 0.0, 0.0, 0.0, 0.0, 0.0);

    image_colour_at(Texture->Vals.Image, xcoor, ycoor, colour, &reg_number);

    if (Texture->Vals.Image->Colour_Map == NULL)
    {
      Material_Number = (int)(colour[RED] * 255.0);
    }
    else
    {
      Material_Number = reg_number;
    }
  }

  if (Material_Number > Texture->Num_Of_Mats)
  {
    Material_Number %= Texture->Num_Of_Mats;
  }

  for (numtex = 0, Temp_Tex = Texture->Materials;
       (Temp_Tex->Next_Material != NULL) && (numtex < Material_Number);
       Temp_Tex = Temp_Tex->Next_Material, numtex++)
  {
    /* do nothing */
  }

  return (Temp_Tex);
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

void bump_map(VECTOR EPoint, TNORMAL *Tnormal, VECTOR normal)
{
  DBL xcoor = 0.0, ycoor = 0.0;
  int index, index2, index3;
  COLOUR colour1, colour2, colour3;
  VECTOR p1, p2, p3;
  VECTOR bump_normal;
  VECTOR xprime, yprime, zprime, Temp;
  DBL Length;
  DBL Amount = Tnormal->Amount;
  IMAGE *Image = Tnormal->Vals.Image;

  Make_ColourA(colour1, 0.0, 0.0, 0.0, 0.0, 0.0);
  Make_ColourA(colour2, 0.0, 0.0, 0.0, 0.0, 0.0);
  Make_ColourA(colour3, 0.0, 0.0, 0.0, 0.0, 0.0);

  /* going to have to change this */
  /* need to know if bump point is off of image for all 3 points */

  if (map(EPoint, (TPATTERN *) Tnormal, &xcoor, &ycoor))
  {
    return;
  }
  else
  {
    image_colour_at(Image, xcoor, ycoor, colour1, &index);
  }

  xcoor--;
  ycoor++;

  if (xcoor < 0.0)
  {
    xcoor += (DBL)Image->iwidth;
  }
  else
  {
    if (xcoor >= Image->iwidth)
    {
      xcoor -= (DBL)Image->iwidth;
    }
  }

  if (ycoor < 0.0)
  {
    ycoor += (DBL)Image->iheight;
  }
  else
  {
    if (ycoor >= (DBL)Image->iheight)
    {
      ycoor -= (DBL)Image->iheight;
    }
  }

  image_colour_at(Image, xcoor, ycoor, colour2, &index2);

  xcoor += 2.0;

  if (xcoor < 0.0)
  {
    xcoor += (DBL)Image->iwidth;
  }
  else
  {
    if (xcoor >= Image->iwidth)
    {
      xcoor -= (DBL)Image->iwidth;
    }
  }

  image_colour_at(Image, xcoor, ycoor, colour3, &index3);

  if (Image->Colour_Map == NULL || Image->Use_Colour_Flag)
  {
    p1[X] = 0;
    p1[Y] = Amount * (0.229 * colour1[RED] + 0.587 * colour1[GREEN] + 0.114 * colour1[BLUE]);
    p1[Z] = 0;

    p2[X] = -1;
    p2[Y] = Amount * (0.229 * colour2[RED] + 0.587 * colour2[GREEN] + 0.114 * colour2[BLUE]);
    p2[Z] = 1;

    p3[X] = 1;
    p3[Y] = Amount * (0.229 * colour3[RED] + 0.587 * colour3[GREEN] + 0.114 * colour3[BLUE]);
    p3[Z] = 1;
  }
  else
  {
    p1[X] = 0;
    p1[Y] = Amount * index;
    p1[Z] = 0;

    p2[X] = -1;
    p2[Y] = Amount * index2;
    p2[Z] = 1;

    p3[X] = 1;
    p3[Y] = Amount * index3;
    p3[Z] = 1;
  }

  /* we have points 1,2,3 for a triangle now we need the surface normal for it */

  VSub(xprime, p1, p2);
  VSub(yprime, p3, p2);
  VCross(bump_normal, yprime, xprime);
  VNormalize(bump_normal, bump_normal);

  Assign_Vector(yprime, normal);
  VScaleEq(yprime, -1);
  Make_Vector(Temp, 0.0, 1.0, 0.0);
  VCross(xprime, yprime, Temp);
  VLength(Length, xprime);

  if (Length < EPSILON)
  {
    if (fabs(normal[Y] - 1.0) < Small_Tolerance)
    {
      Make_Vector(yprime, 0.0, 1.0, 0.0);
      Make_Vector(xprime, 1.0, 0.0, 0.0);
      Length = 1.0;
    }
    else
    {
      Make_Vector(yprime, 0.0, -1.0, 0.0);
      Make_Vector(xprime, 1.0, 0.0, 0.0);
      Length = 1.0;
    }
  }

  VScaleEq(xprime, 1.0 / Length);
  VCross(zprime, xprime, yprime);
  VNormalizeEq(zprime);
  VScaleEq(xprime, bump_normal[X]);
  VScaleEq(yprime, bump_normal[Y]);
  VScaleEq(zprime, bump_normal[Z]);
  VAdd(Temp, xprime, yprime);
  VAdd(normal, Temp, zprime);
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

static void image_colour_at(IMAGE *Image, DBL xcoor, DBL  ycoor, COLOUR colour, int *index)
{
  switch (Image->Interpolation_Type)
  {
    case NO_INTERPOLATION:

      no_interpolation(Image, xcoor, ycoor, colour, index);

      break;

    default:

      Interp(Image, xcoor, ycoor, colour, index);

      break;
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
*   Map a point (x, y, z) on a cylinder of radius 1, height 1, that has its axis
*   of symmetry along the y-axis to the square [0,1]x[0,1].
*
* CHANGES
*
******************************************************************************/

static int cylindrical_image_map(VECTOR EPoint, IMAGE *Image, DBL *u, DBL  *v)
{
  DBL len, theta;
  DBL x = EPoint[X];
  DBL y = EPoint[Y];
  DBL z = EPoint[Z];

  if ((Image->Once_Flag) && ((y < 0.0) || (y > 1.0)))
  {
    return 0;
  }

  *v = fmod(y * Image->height, Image->height);

  /* Make sure this vector is on the unit sphere. */

  len = sqrt(x * x + y * y + z * z);

  if (len == 0.0)
  {
    return 0;
  }
  else
  {
    x /= len;
    z /= len;
  }

  /* Determine its angle from the point (1, 0, 0) in the x-z plane. */

  len = sqrt(x * x + z * z);

  if (len == 0.0)
  {
    return 0;
  }
  else
  {
    if (z == 0.0)
    {
      if (x > 0)
      {
        theta = 0.0;
      }
      else
      {
        theta = M_PI;
      }
    }
    else
    {
      theta = acos(x / len);

      if (z < 0.0)
      {
        theta = TWO_M_PI - theta;
      }
    }

    theta /= TWO_M_PI;  /* This will be from 0 to 1 */
  }

  *u = (theta * Image->width);

  return 1;
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
*   Map a point (x, y, z) on a torus  to a 2-d image.
*
* CHANGES
*
******************************************************************************/

static int torus_image_map(VECTOR EPoint, IMAGE *Image, DBL *u, DBL  *v)
{
  DBL len, phi, theta;
  DBL r0;
  DBL x = EPoint[X];
  DBL y = EPoint[Y];
  DBL z = EPoint[Z];

  r0 = Image->Gradient[X];

  /* Determine its angle from the x-axis. */

  len = sqrt(x * x + z * z);

  if (len == 0.0)
  {
    return 0;
  }
  else
  {
    if (z == 0.0)
    {
      if (x > 0)
      {
        theta = 0.0;
      }
      else
      {
        theta = M_PI;
      }
    }
    else
    {
      theta = acos(x / len);

      if (z < 0.0)
      {
        theta = TWO_M_PI - theta;
      }
    }
  }

  theta = 0.0 - theta;

  /* Now rotate about the y-axis to get the point (x, y, z) into the x-y plane. */

  x = len - r0;

  len = sqrt(x * x + y * y);

  phi = acos(-x / len);

  if (y > 0.0)
  {
    phi = TWO_M_PI - phi;
  }

  /* Determine the parametric coordinates. */

  theta /= TWO_M_PI;

  phi /= TWO_M_PI;

  *u = (-theta * Image->width);

  *v = (phi * Image->height);

  return 1;
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
*   Map a point (x, y, z) on a sphere of radius 1 to a 2-d image. (Or is it the
*   other way around?)
*
* CHANGES
*
******************************************************************************/

static int spherical_image_map(VECTOR EPoint, IMAGE *Image, DBL *u, DBL  *v)
{
  DBL len, phi, theta;
  DBL x = EPoint[X];
  DBL y = EPoint[Y];
  DBL z = EPoint[Z];

  /* Make sure this vector is on the unit sphere. */

  len = sqrt(x * x + y * y + z * z);

  if (len == 0.0)
  {
    return 0;
  }
  else
  {
    x /= len;
    y /= len;
    z /= len;
  }

  /* Determine its angle from the x-z plane. */

  phi = 0.5 + asin(y) / M_PI; /* This will be from 0 to 1 */


  /* Determine its angle from the point (1, 0, 0) in the x-z plane. */

  len = sqrt(x * x + z * z);

  if (len == 0.0)
  {
    /* This point is at one of the poles. Any value of xcoord will be ok... */

    theta = 0;
  }
  else
  {
    if (z == 0.0)
    {
      if (x > 0)
      {
        theta = 0.0;
      }
      else
      {
        theta = M_PI;
      }
    }
    else
    {
      theta = acos(x / len);

      if (z < 0.0)
      {
        theta = TWO_M_PI - theta;
      }
    }

    theta /= TWO_M_PI;  /* This will be from 0 to 1 */
  }

  *u = (theta * Image->width);
  *v = (phi * Image->height);

  return 1;
}

/*
 * 2-D to 3-D Procedural Texture Mapping of a Bitmapped Image onto an Object:
 * 
 * Simplistic planar method of object image projection devised by DKB and AAC.
 * 
 * 1. Transform texture in 3-D space if requested. 2. Determine local object 2-d
 * coords from 3-d coords by <X Y Z> triple. 3. Return pixel color value at
 * that position on the 2-d plane of "Image". 3. Map colour value in Image
 * [0..255] to a more normal colour range [0..1].
 */



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
*   Return 0 if there is no color at this point (i.e. invisible), return 1 if a
*   good mapping is found.
*
* CHANGES
*
******************************************************************************/

static int planar_image_map(VECTOR EPoint, IMAGE *Image, DBL *u, DBL  *v)
{
  DBL x = EPoint[X];
  DBL y = EPoint[Y];
  DBL z = EPoint[Z];

  if (Image->Gradient[X] != 0.0)
  {
    if ((Image->Once_Flag) && ((x < 0.0) || (x > 1.0)))
    {
      return 0;
    }

    if (Image->Gradient[X] > 0)
    {
      *u = fmod(x * Image->width, Image->width);
    }
    else
    {
      *v = fmod(x * Image->height, Image->height);
    }
  }

  if (Image->Gradient[Y] != 0.0)
  {
    if ((Image->Once_Flag) && ((y < 0.0) || (y > 1.0)))
    {
      return 0;
    }

    if (Image->Gradient[Y] > 0)
    {
      *u = fmod(y * Image->width, Image->width);
    }
    else
    {
      *v = fmod(y * Image->height, Image->height);
    }
  }

  if (Image->Gradient[Z] != 0.0)
  {
    if ((Image->Once_Flag) && ((z < 0.0) || (z > 1.0)))
    {
      return 0;
    }

    if (Image->Gradient[Z] > 0)
    {
      *u = fmod(z * Image->width, Image->width);
    }
    else
    {
      *v = fmod(z * Image->height, Image->height);
    }
  }

  return 1;
}


/*****************************************************************************
*
* FUNCTION
*
*   map
*
* INPUT
*
*   EPoint   -- 3-D point at which function is evaluated
*   TPattern -- Pattern containing various parameters
*
* OUTPUT
*
*   xcoor, ycoor -- 2-D result
*
* RETURNS
*
*   Map returns 1 if point off of map 0 if on map
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Maps a 3-D point to a 2-D point depending upon map type
*
* CHANGES
*
******************************************************************************/

static int map(VECTOR EPoint, TPATTERN *TPattern, DBL *xcoor, DBL  *ycoor)
{
  IMAGE *Image = TPattern->Vals.Image;

  /* Now determine which mapper to use. */

  switch (Image->Map_Type)
  {
    case PLANAR_MAP:

      if (!planar_image_map(EPoint, Image, xcoor, ycoor))
      {
        return (1);
      }

      break;

    case SPHERICAL_MAP:

      if (!spherical_image_map(EPoint, Image, xcoor, ycoor))
      {
        return (1);
      }

      break;

    case CYLINDRICAL_MAP:

      if (!cylindrical_image_map(EPoint, Image, xcoor, ycoor))
      {
        return (1);
      }

      break;

    case TORUS_MAP:

      if (!torus_image_map(EPoint, Image, xcoor, ycoor))
      {
        return (1);
      }

      break;

    default:

      if (!planar_image_map(EPoint, Image, xcoor, ycoor))
      {
        return (1);
      }

      break;
  }

  /* Now make sure the point is on the image */

  *ycoor += Small_Tolerance;
  *xcoor += Small_Tolerance;

  /* Compensate for y coordinates on the images being upsidedown */

  *ycoor = (DBL)Image->iheight - *ycoor;

  if (*xcoor < 0.0)
  {
    *xcoor += (DBL)Image->iwidth;
  }
  else
  {
    if (*xcoor >= (DBL)Image->iwidth)
    {
      *xcoor -= (DBL)Image->iwidth;
    }
  }

  if (*ycoor < 0.0)
  {
    *ycoor += (DBL)Image->iheight;
  }
  else
  {
    if (*ycoor >= (DBL)Image->iheight)
    {
      *ycoor -= (DBL)Image->iheight;
    }
  }

  if ((*xcoor >= (DBL)Image->iwidth) ||
      (*ycoor >= (DBL)Image->iheight) ||
      (*xcoor < 0.0) || (*ycoor < 0.0))
  {
    Error("Picture index out of range.");
  }

  return (0);
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

static void no_interpolation(IMAGE *Image, DBL xcoor, DBL  ycoor, COLOUR colour, int *index)
{
  IMAGE_LINE *line;
  int iycoor, ixcoor;
  IMAGE_COLOUR *map_colour;

  if(Image->Once_Flag)
  {
     if (xcoor < 0.0)
     {
       xcoor = 0.0;
     }
     else
     {
       if (xcoor >= (DBL)Image->iwidth)
       {
         xcoor -= 1.0;
       }
     }
   
     if (ycoor < 0.0)
     {
       ycoor = 0.0;
     }
     else
     {
       if (ycoor >= (DBL)Image->iheight)
       {
         ycoor -= 1.0;
       }
     }
  }
  else
  {
     if (xcoor < 0.0)
     {
       xcoor += (DBL)Image->iwidth;
     }
     else
     {
       if (xcoor >= (DBL)Image->iwidth)
       {
         xcoor -= (DBL)Image->iwidth;
       }
     }
   
     if (ycoor < 0.0)
     {
       ycoor += (DBL)Image->iheight;
     }
     else
     {
       if (ycoor >= (DBL)Image->iheight)
       {
         ycoor -= (DBL)Image->iheight;
       }
     }
  }

  iycoor = (int)ycoor;
  ixcoor = (int)xcoor;

  if (Image->Colour_Map == NULL)
  {
    line = &Image->data.rgb_lines[iycoor];

    colour[RED] += (DBL)line->red[ixcoor] / 255.0;
    colour[GREEN] += (DBL)line->green[ixcoor] / 255.0;
    colour[BLUE] += (DBL)line->blue[ixcoor] / 255.0;
    if (line->transm != NULL)
    {
      colour[TRANSM] += (DBL)line->transm[ixcoor] / 255.0;
    }

    *index = -1;
  }
  else
  {
    *index = Image->data.map_lines[iycoor][ixcoor];

    map_colour = &Image->Colour_Map[*index];

    colour[RED] += (DBL)map_colour->Red / 255.0;
    colour[GREEN] += (DBL)map_colour->Green / 255.0;
    colour[BLUE] += (DBL)map_colour->Blue / 255.0;
    colour[FILTER] += (DBL)map_colour->Filter / 255.0;
    colour[TRANSM] += (DBL)map_colour->Transmit / 255.0;
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

/* Interpolate color and filter values when mapping */

static void Interp(IMAGE *Image, DBL xcoor, DBL  ycoor, COLOUR colour, int *index)
{
  int iycoor, ixcoor, i;
  int Corners_Index[4];
  DBL Index_Crn[4];
  COLOUR Corner_Colour[4];
  DBL Red_Crn[4];
  DBL Green_Crn[4];
  DBL Blue_Crn[4];
  DBL Filter_Crn[4];
  DBL Transm_Crn[4];
  DBL val1, val2, val3, val4, val5;

  val1 = val2 = val3 = val4 = val5 = 0.0;

  iycoor = (int)ycoor;
  ixcoor = (int)xcoor;

  for (i = 0; i < 4; i++)
  {
    Make_ColourA(Corner_Colour[i], 0.0, 0.0, 0.0, 0.0, 0.0);
  }

  /* OK, now that you have the corners, what are you going to do with them? */

  if (Image->Interpolation_Type == BILINEAR)
  {
    no_interpolation(Image, (DBL)ixcoor + 1, (DBL)iycoor, Corner_Colour[0], &Corners_Index[0]);
    no_interpolation(Image, (DBL)ixcoor, (DBL)iycoor, Corner_Colour[1], &Corners_Index[1]);
    no_interpolation(Image, (DBL)ixcoor + 1, (DBL)iycoor - 1, Corner_Colour[2], &Corners_Index[2]);
    no_interpolation(Image, (DBL)ixcoor, (DBL)iycoor - 1, Corner_Colour[3], &Corners_Index[3]);

    for (i = 0; i < 4; i++)
    {
      Red_Crn[i] = Corner_Colour[i][RED];
      Green_Crn[i] = Corner_Colour[i][GREEN];
      Blue_Crn[i] = Corner_Colour[i][BLUE];
      Filter_Crn[i] = Corner_Colour[i][FILTER];
      Transm_Crn[i] = Corner_Colour[i][TRANSM];

      /*
       * Debug_Info("Crn %d = %lf %lf
       * %lf\n",i,Red_Crn[i],Blue_Crn[i],Green_Crn[i]);
       */
    }

    val1 = bilinear(Red_Crn, xcoor, ycoor);
    val2 = bilinear(Green_Crn, xcoor, ycoor);
    val3 = bilinear(Blue_Crn, xcoor, ycoor);
    val4 = bilinear(Filter_Crn, xcoor, ycoor);
    val5 = bilinear(Transm_Crn, xcoor, ycoor);
  }

  if (Image->Interpolation_Type == NORMALIZED_DIST)
  {
    no_interpolation(Image, (DBL)ixcoor, (DBL)iycoor - 1, Corner_Colour[0], &Corners_Index[0]);
    no_interpolation(Image, (DBL)ixcoor + 1, (DBL)iycoor - 1, Corner_Colour[1], &Corners_Index[1]);
    no_interpolation(Image, (DBL)ixcoor, (DBL)iycoor, Corner_Colour[2], &Corners_Index[2]);
    no_interpolation(Image, (DBL)ixcoor + 1, (DBL)iycoor, Corner_Colour[3], &Corners_Index[3]);

    for (i = 0; i < 4; i++)
    {
      Red_Crn[i] = Corner_Colour[i][RED];
      Green_Crn[i] = Corner_Colour[i][GREEN];
      Blue_Crn[i] = Corner_Colour[i][BLUE];
      Filter_Crn[i] = Corner_Colour[i][FILTER];
      Transm_Crn[i] = Corner_Colour[i][TRANSM];

      /*
       * Debug_Info("Crn %d = %lf %lf
       * %lf\n",i,Red_Crn[i],Blue_Crn[i],Green_Crn[i]);
       */
    }

    val1 = norm_dist(Red_Crn, xcoor, ycoor);
    val2 = norm_dist(Green_Crn, xcoor, ycoor);
    val3 = norm_dist(Blue_Crn, xcoor, ycoor);
    val4 = norm_dist(Filter_Crn, xcoor, ycoor);
    val5 = norm_dist(Transm_Crn, xcoor, ycoor);
  }

  colour[RED] += val1;
  colour[GREEN] += val2;
  colour[BLUE] += val3;
  colour[FILTER] += val4;
  colour[TRANSM] += val5;

  /* Debug_Info("Final = %lf %lf %lf\n",val1,val2,val3);  */
  /* use bilinear for index try average later */

  for (i = 0; i < 4; i++)
  {
    Index_Crn[i] = (DBL)Corners_Index[i];
  }

  if (Image->Interpolation_Type == BILINEAR)
  {
    *index = (int)(bilinear(Index_Crn, xcoor, ycoor) + 0.5);
  }

  if (Image->Interpolation_Type == NORMALIZED_DIST)
  {
    *index = (int)(norm_dist(Index_Crn, xcoor, ycoor) + 0.5);
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

/* These interpolation techniques are taken from an article by */
/* Girish T. Hagan in the C Programmer's Journal V 9 No. 8 */
/* They were adapted for POV-Ray by CdW */

static DBL bilinear(DBL *corners, DBL  x, DBL  y)
{
  DBL p, q;
  DBL val;

  p = x - (int)x;
  q = y - (int)y;

  if ((p == 0.0) && (q == 0.0))
  {
    return (*corners);  /* upper left */
  }

  val = (p * q * *corners) + (q * (1 - p) * *(corners + 1)) +
    (p * (1 - q) * *(corners + 2)) + ((1 - p) * (1 - q) * *(corners + 3));

  return (val);
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

#define MAX_PTS 4
#define PYTHAGOREAN_SQ(a,b)  ( (a)*(a) + (b)*(b) )

static DBL norm_dist(DBL *corners, DBL  x, DBL  y)
{
  register int i;

  DBL p, q;
  DBL wts[MAX_PTS];
  DBL sum_inv_wts = 0.0;
  DBL sum_I = 0.0;

  p = x - (int)x;
  q = y - (int)y;

  if ((p == 0.0) && (q == 0.0))
  {
    return (*corners);  /* upper left */
  }

  wts[0] = PYTHAGOREAN_SQ(p, q);
  wts[1] = PYTHAGOREAN_SQ(1 - p, q);
  wts[2] = PYTHAGOREAN_SQ(p, 1 - q);
  wts[3] = PYTHAGOREAN_SQ(1 - p, 1 - q);

  for (i = 0; i < MAX_PTS; i++)
  {
    sum_inv_wts += 1 / wts[i];

    sum_I += *(corners + i) / wts[i];
  }

  return (sum_I / sum_inv_wts);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Image
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

IMAGE *Create_Image()
{
  IMAGE *Image;

  Image = (IMAGE *) POV_MALLOC(sizeof(IMAGE), "image file");

  Image->References = 1;

  Image->File_Type = NO_FILE;

  Image->Map_Type = PLANAR_MAP;

  Image->Interpolation_Type = NO_INTERPOLATION;

  Image->Once_Flag = FALSE;

  Image->Use_Colour_Flag = FALSE;

  Make_Vector(Image->Gradient, 1.0, -1.0, 0.0);

  return (Image);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Image
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

IMAGE *Copy_Image(IMAGE *Old)
{
  if (Old != NULL)
  {
    Old->References++;
  }

  return (Old);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Image
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Destroy_Image(IMAGE *Image)
{
  int i;

  if ((Image == NULL) || (--(Image->References) > 0))
  {
    return;
  }

  if (Image->Colour_Map != NULL)
  {
    POV_FREE(Image->Colour_Map);

    Image->Colour_Map = NULL;

    if (Image->data.map_lines != NULL)
    {
      for (i = 0; i < Image->iheight; i++)
      {
        POV_FREE(Image->data.map_lines[i]);
      }

      POV_FREE(Image->data.map_lines);

      Image->data.map_lines = NULL;
    }
  }
  else
  {
    if (Image->data.rgb_lines != NULL)
    {
      for (i = 0; i < Image->iheight; i++)
      {
        POV_FREE(Image->data.rgb_lines[i].red);
        POV_FREE(Image->data.rgb_lines[i].green);
        POV_FREE(Image->data.rgb_lines[i].blue);

        if (Image->data.rgb_lines[i].transm != NULL)
        {
          POV_FREE(Image->data.rgb_lines[i].transm);
        }
      }

      POV_FREE(Image->data.rgb_lines);

      Image->data.rgb_lines = NULL;
    }
  }

  POV_FREE(Image);
}
