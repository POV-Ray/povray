/****************************************************************************
*                parstxtr.c
*
*  This module parses textures and atmosphere effects.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1999 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by email to team-coord@povray.org or visit us on the web at
*  http://www.povray.org. The latest version of POV-Ray may be found at this site.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
* Modifications by Thomas Willhalm, March 1999, used with permission.
*
*****************************************************************************/

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "parse.h"
#include "parstxtr.h"
#include "colour.h"
#include "express.h"
#include "gif.h"
#include "iff.h"
#include "image.h"
#include "matrices.h" 
#include "media.h"
#include "normal.h"
#include "pigment.h"  
#include "povray.h"
#include "pgm.h"
#include "ppm.h"
#include "targa.h"
#include "png_pov.h"
#include "texture.h"
#include "tokenize.h"


/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define ADD_TNORMAL if (Tnormal == NULL) {if ((Default_Texture->Tnormal) != NULL) \
 Tnormal = Copy_Tnormal ((Default_Texture->Tnormal)); else Tnormal = Create_Tnormal ();\
 Texture->Tnormal=Tnormal;};

/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

TEXTURE *Default_Texture;



/*****************************************************************************
* Static functions
******************************************************************************/

static void Parse_Bump_Map (TNORMAL *Tnormal);
static void Parse_Image_Map (PIGMENT *Pigment);
static void Parse_Pattern (TPATTERN *New, int TPat_Type);
static TEXTURE *Parse_Vers1_Texture (void);
static TEXTURE *Parse_Tiles (void);
static TEXTURE *Parse_Material_Map (void);
static void Parse_Texture_Transform (TEXTURE *Texture);
static TURB *Check_Turb (WARP **Warps_Ptr);
static void Parse_Warp (WARP **Warp_Ptr);
static void Check_BH_Parameters (BLACK_HOLE *bh);
static void Warn_Interior (char *s);



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

IMAGE *Parse_Image (int Legal)
{
   IMAGE *Image;
   VECTOR Local_Vector;
   char *Name;

   Image = Create_Image ();

   Image->Image_Type = Legal;

   if (Legal & GRAD_FILE)
     {
      EXPECT
        CASE_VECTOR
          Warn(1.5, "Old style orientation vector or map type not supported.  Ignoring value.");
          Parse_Vector (Local_Vector);
        END_CASE

        OTHERWISE
          UNGET
          EXIT
        END_CASE
      END_EXPECT
     }

   EXPECT
     CASE (IFF_TOKEN)
       Image->File_Type = IFF_FILE;
       Name=Parse_String();
       Read_Iff_Image (Image, Name);
       POV_FREE(Name);
       EXIT
     END_CASE

     CASE (GIF_TOKEN)
       Image->File_Type = GIF_FILE;
       Name=Parse_String();
       Read_Gif_Image(Image, Name);
       POV_FREE(Name);
       EXIT
     END_CASE

     CASE (POT_TOKEN)
       Image->File_Type = POT_FILE;
       Name=Parse_String();
       Read_Gif_Image(Image, Name);
       POV_FREE(Name);
       EXIT
     END_CASE

     CASE (SYS_TOKEN)
       Image->File_Type = SYS_FILE;
       Name=Parse_String();
       READ_SYS_IMAGE(Image, Name);
       POV_FREE(Name);
       EXIT
     END_CASE

     CASE (TGA_TOKEN)
       Image->File_Type = TGA_FILE;
       Name=Parse_String();
       Read_Targa_Image(Image, Name);
       POV_FREE(Name);
       EXIT
     END_CASE

     CASE (PNG_TOKEN)
       Image->File_Type = PNG_FILE;
       Name=Parse_String();
       Read_Png_Image(Image, Name);
       POV_FREE(Name);
       EXIT
     END_CASE

     CASE (PGM_TOKEN)
       Image->File_Type = PGM_FILE;
       Name=Parse_String();
       Read_PGM_Image(Image, Name);
       POV_FREE(Name);
       EXIT
     END_CASE

     CASE (PPM_TOKEN)
       Image->File_Type = PPM_FILE;
       Name=Parse_String();
       Read_PPM_Image(Image, Name);
       POV_FREE(Name);
       EXIT
     END_CASE

     OTHERWISE
       Parse_Error_Str ("map file spec");
     END_CASE
   END_EXPECT

   if (!(Image->File_Type & Legal))
     Error ("File type not supported here.");
   return (Image);
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

static void Parse_Image_Map (PIGMENT *Pigment)
{
   int reg;
   IMAGE *Image;
   
   Parse_Begin();

   Image = Parse_Image (IMAGE_FILE);
   Image->Use_Colour_Flag = TRUE;

   EXPECT                   /* Look for image_attribs */
     CASE (ONCE_TOKEN)
       Image->Once_Flag=TRUE;
     END_CASE

     CASE (INTERPOLATE_TOKEN)
       Image->Interpolation_Type = (int)Parse_Float();
     END_CASE

     CASE (MAP_TYPE_TOKEN)
       Image->Map_Type = (int) Parse_Float ();
     END_CASE

     CASE (USE_COLOUR_TOKEN)
       Image->Use_Colour_Flag = TRUE;
     END_CASE

     CASE (USE_INDEX_TOKEN)
       Image->Use_Colour_Flag = FALSE;
     END_CASE

     CASE (ALPHA_TOKEN)
       Warn(1.55, "Keyword ALPHA discontinued. Use FILTER instead.");

     CASE (COLOUR_KEY_TOKEN)
       switch(Token.Function_Id)
         {
          case FILTER_TOKEN:
            EXPECT
              CASE (ALL_TOKEN)
                {
                 DBL filter;
                 filter = Parse_Float();
                 for (reg = 0 ; reg < Image->Colour_Map_Size ; reg++)
                   Image->Colour_Map[reg].Filter
                       = (unsigned short) (filter *255.0);
                }
                EXIT
              END_CASE

              OTHERWISE
                UNGET
                reg = (int)(Parse_Float() + 0.01);
                if (Image->Colour_Map == NULL)
                  Not_With ("filter","non color-mapped image");
                if ((reg < 0) || (reg >= Image->Colour_Map_Size))
                  Error ("FILTER color register value out of range.");

                Parse_Comma();
                Image->Colour_Map[reg].Filter
                       = (unsigned short) (255.0 * Parse_Float());
                EXIT
              END_CASE

            END_EXPECT
            Pigment->Flags |= HAS_FILTER;
            break;

          case TRANSMIT_TOKEN:
            EXPECT
              CASE (ALL_TOKEN)
                {
                 DBL transmit;
                 transmit = Parse_Float();
                 for (reg = 0 ; reg < Image->Colour_Map_Size ; reg++)
                   Image->Colour_Map[reg].Transmit
                       = (unsigned short) (transmit *255.0);
                }
                EXIT
              END_CASE

              OTHERWISE
                UNGET
                reg = (int)(Parse_Float() + 0.01);
                if (Image->Colour_Map == NULL)
                  Not_With ("transmit","non color-mapped image");
                if ((reg < 0) || (reg >= Image->Colour_Map_Size))
                  Error ("TRANSMIT color register value out of range.");

                Parse_Comma();
                Image->Colour_Map[reg].Transmit
                       = (unsigned short) (255.0 * Parse_Float());
                EXIT
              END_CASE

            END_EXPECT
            Pigment->Flags |= HAS_FILTER;
            break;

          default:
            UNGET
            Parse_Error_Str ("filter or transmit");
            break;
         }
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT
   
   Pigment->Vals.Image=Image;
   Parse_End();
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

static void Parse_Bump_Map (TNORMAL *Tnormal)
{
   IMAGE *Image;

   Parse_Begin();

   Image = Parse_Image(IMAGE_FILE);
   Image->Use_Colour_Flag = TRUE;

   EXPECT
     CASE (ONCE_TOKEN)
       Image->Once_Flag=TRUE;
     END_CASE

     CASE (MAP_TYPE_TOKEN)
       Image->Map_Type = (int) Parse_Float ();
     END_CASE

     CASE (INTERPOLATE_TOKEN)
       Image->Interpolation_Type = (int)Parse_Float();
     END_CASE

     CASE (BUMP_SIZE_TOKEN)
       Tnormal->Amount = Parse_Float ();
     END_CASE

     CASE (USE_COLOUR_TOKEN)
       Image->Use_Colour_Flag = TRUE;
     END_CASE

     CASE (USE_INDEX_TOKEN)
       Image->Use_Colour_Flag = FALSE;
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT

   Tnormal->Vals.Image=Image;

   Parse_End();
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

void Parse_Pigment (PIGMENT **Pigment_Ptr)
{
   EXPECT            /* Look for [pigment_id] */
     CASE (PIGMENT_ID_TOKEN)
       Destroy_Pigment(*Pigment_Ptr);
       *Pigment_Ptr = Copy_Pigment ((PIGMENT *) Token.Data);
       EXIT
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT    /* End pigment_id */
   
   Parse_Pattern((TPATTERN *)(*Pigment_Ptr),PIGMENT_TYPE);

   if (Not_In_Default && ((*Pigment_Ptr)->Type == NO_PATTERN))
   {
     Warn(1.7, "Pigment type unspecified or not 1st item.");
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

static void Parse_Pattern (TPATTERN *New, int TPat_Type)
{
   VECTOR Local_Vector;
   COLOUR Local_Colour;
   MATRIX Local_Matrix;
   TRANSFORM Local_Trans;
   TURB *Local_Turb;
   unsigned short Old_Type=New->Type;
   IMAGE *Old_Image = NULL;
   DENSITY_FILE *Old_Density_File = NULL;

   if (Old_Type==BITMAP_PATTERN)
   {
     Old_Image=New->Vals.Image;
   }

   if (Old_Type==DENSITY_FILE_PATTERN)
   {
     Old_Density_File=New->Vals.Density_File;
   }

   EXPECT
     CASE (AGATE_TOKEN)
       New->Type = AGATE_PATTERN;
       Check_Turb(&(New->Warps));
       New->Vals.Agate_Turb_Scale = 1.0;
       EXIT
     END_CASE

     CASE (BOZO_TOKEN)
       New->Type = BOZO_PATTERN;
       EXIT
     END_CASE

     CASE (GRANITE_TOKEN)
       New->Type = GRANITE_PATTERN;
       EXIT
     END_CASE

     CASE (LEOPARD_TOKEN)
       New->Type = LEOPARD_PATTERN;
       EXIT
     END_CASE

     CASE (MARBLE_TOKEN)
       New->Type = MARBLE_PATTERN;
       New->Wave_Type = TRIANGLE_WAVE;
       EXIT
     END_CASE

     CASE (MANDEL_TOKEN)
       New->Type = MANDEL_PATTERN;
       New->Vals.Iterations = (int)Parse_Float();
       EXIT
     END_CASE

     CASE (ONION_TOKEN)
       New->Type = ONION_PATTERN;
       EXIT
     END_CASE

     CASE (SPIRAL1_TOKEN)
       New->Type = SPIRAL1_PATTERN;
       New->Vals.Arms = (short)Parse_Float ();
       New->Wave_Type = TRIANGLE_WAVE;
       EXIT
     END_CASE

     CASE (SPIRAL2_TOKEN)
       New->Type = SPIRAL2_PATTERN;
       New->Vals.Arms = (short)Parse_Float ();
       New->Wave_Type = TRIANGLE_WAVE;
       EXIT
     END_CASE

     CASE (SPOTTED_TOKEN)
       New->Type = SPOTTED_PATTERN;
       EXIT
     END_CASE

     CASE (WOOD_TOKEN)
       New->Type = WOOD_PATTERN;
       New->Wave_Type = TRIANGLE_WAVE;
       EXIT
     END_CASE

     CASE (GRADIENT_TOKEN)
       New->Type = GRADIENT_PATTERN;
       Parse_Vector (New->Vals.Gradient);
       EXIT
     END_CASE

     CASE (RADIAL_TOKEN)
       New->Type = RADIAL_PATTERN;
       EXIT
     END_CASE

     CASE (CRACKLE_TOKEN)
       New->Type = CRACKLE_PATTERN;
       EXIT
     END_CASE

     CASE_COLOUR
       if ((TPat_Type != PIGMENT_TYPE) && (TPat_Type != DENSITY_TYPE))
       {
         Only_In("color","pigment or density");
       }
       New->Type = PLAIN_PATTERN;
       Parse_Colour (((PIGMENT *)New)->Colour);
       EXIT
     END_CASE

     CASE (CHECKER_TOKEN)
       New->Type = CHECKER_PATTERN;
       New->Frequency = 0.0;
       Destroy_Blend_Map(New->Blend_Map);
       New->Blend_Map = Parse_Blend_List(2,&Check_Default_Map,TPat_Type);
       EXIT
     END_CASE

     CASE (BRICK_TOKEN)
       if (New->Type!=BRICK_PATTERN)
       {
         Make_Vector(New->Vals.Brick.Size,8.0,3.0,4.5);
         New->Vals.Brick.Mortar=0.5-Small_Tolerance*2.0;
       New->Type = BRICK_PATTERN;
       }
       New->Frequency = 0.0;
       Destroy_Blend_Map(New->Blend_Map);
       New->Blend_Map = Parse_Blend_List(2,&Brick_Default_Map,TPat_Type);
       EXIT
     END_CASE

     CASE (HEXAGON_TOKEN)
       New->Type = HEXAGON_PATTERN;
       New->Frequency = 0.0;
       Destroy_Blend_Map(New->Blend_Map);
       New->Blend_Map = Parse_Blend_List(3,&Hex_Default_Map,TPat_Type);
       EXIT
     END_CASE

     CASE (IMAGE_MAP_TOKEN)
       if (TPat_Type != PIGMENT_TYPE)
       {
         Only_In("image_map","pigment");
       }

       if (Old_Type==BITMAP_PATTERN) 
       {
         Destroy_Image(Old_Image);
       }   

       New->Type = BITMAP_PATTERN;
       New->Frequency = 0.0;
       Parse_Image_Map ((PIGMENT *)New);
       EXIT
     END_CASE

     CASE (BUMP_MAP_TOKEN)
       if (TPat_Type != NORMAL_TYPE)
       {
         Only_In("bump_map","normal");
       }
 
       if (Old_Type==BITMAP_PATTERN) 
       {
         Destroy_Image(Old_Image);
       }   

       New->Type = BITMAP_PATTERN;
       New->Frequency = 0.0;
       Parse_Bump_Map ((TNORMAL *)New);
       EXIT
     END_CASE

     CASE (WAVES_TOKEN)
       New->Type = WAVES_PATTERN;
       EXIT
     END_CASE

     CASE (RIPPLES_TOKEN)
       New->Type = RIPPLES_PATTERN;
       EXIT
     END_CASE

     CASE (WRINKLES_TOKEN)
       New->Type = WRINKLES_PATTERN;
       EXIT
     END_CASE

     CASE (BUMPS_TOKEN)
       New->Type = BUMPS_PATTERN;
       EXIT
     END_CASE

     CASE (DENTS_TOKEN)
       New->Type = DENTS_PATTERN;
       EXIT
     END_CASE

     CASE (QUILTED_TOKEN)
       New->Type = QUILTED_PATTERN;
       New->Vals.Quilted.Control0 = 1.0;
       New->Vals.Quilted.Control1 = 1.0;
       New->Frequency = 0.0;
       EXIT
     END_CASE

     CASE (AVERAGE_TOKEN)
       New->Type = AVERAGE_PATTERN;
       EXIT
     END_CASE

     CASE (PLANAR_TOKEN)
       New->Type = PLANAR_PATTERN;
       EXIT
     END_CASE

     CASE (BOXED_TOKEN)
       New->Type = BOXED_PATTERN;
       EXIT
     END_CASE

     CASE (SPHERICAL_TOKEN)
       New->Type = SPHERICAL_PATTERN;
       EXIT
     END_CASE

     CASE (CYLINDRICAL_TOKEN)
       New->Type = CYLINDRICAL_PATTERN;
       EXIT
     END_CASE

     CASE (DENSITY_FILE_TOKEN)
       if (Old_Type==DENSITY_FILE_PATTERN) 
       {
         Destroy_Density_File(Old_Density_File);
       }   
       New->Type = DENSITY_FILE_PATTERN;
       New->Vals.Density_File = Create_Density_File();
       GET(DF3_TOKEN);
       New->Vals.Density_File->Data->Name = Parse_String();
       Read_Density_File(New->Vals.Density_File);
       EXIT
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT     /* Concludes pattern_body */

   if ((Old_Type==BITMAP_PATTERN) && (New->Type!=BITMAP_PATTERN))
   {
     Destroy_Image(Old_Image);
   }   

   if ((Old_Type==DENSITY_FILE_PATTERN) && (New->Type!=DENSITY_FILE_PATTERN))
   {
     Destroy_Density_File(Old_Density_File);
   }   

   if (TPat_Type == NORMAL_TYPE) 
    {
     Parse_Comma();
     ((TNORMAL *)New)->Amount = Allow_Float (((TNORMAL *)New)->Amount );
    }

   EXPECT         /* Look for pattern_modifier */
     CASE (TURBULENCE_TOKEN)
       Local_Turb=Check_Turb(&(New->Warps));
       Parse_Vector(Local_Turb->Turbulence);
     END_CASE

     CASE (COLOUR_MAP_TOKEN)
       if ((TPat_Type != PIGMENT_TYPE) && (TPat_Type != DENSITY_TYPE))
       {
         Only_In("color_map","pigment");
       }
       if (New->Type == CHECKER_PATTERN ||
           New->Type == BRICK_PATTERN ||
           New->Type == HEXAGON_PATTERN ||
           New->Type == PLAIN_PATTERN ||
           New->Type == AVERAGE_PATTERN ||
           New->Type == BITMAP_PATTERN)
       {
         Error("Cannot use color_map with this pattern type.");
       }
       Destroy_Blend_Map(New->Blend_Map);
       New->Blend_Map = Parse_Colour_Map ();
     END_CASE

     CASE (PIGMENT_MAP_TOKEN)
       if (TPat_Type != PIGMENT_TYPE)
       {
         Only_In("pigment_map","pigment");
       }
       if (New->Type == CHECKER_PATTERN ||
           New->Type == BRICK_PATTERN ||
           New->Type == HEXAGON_PATTERN ||
           New->Type == PLAIN_PATTERN ||
           New->Type == BITMAP_PATTERN)
         Not_With ("pigment_map","this pigment type");
       Destroy_Blend_Map(New->Blend_Map);
       New->Blend_Map = Parse_Blend_Map (PIGMENT_TYPE,New->Type);
     END_CASE

     CASE (DENSITY_MAP_TOKEN)
       if (TPat_Type != DENSITY_TYPE)
       {
         Only_In("density_map","density");
       }
       if (New->Type == CHECKER_PATTERN ||
           New->Type == BRICK_PATTERN ||
           New->Type == HEXAGON_PATTERN ||
           New->Type == PLAIN_PATTERN ||
           New->Type == BITMAP_PATTERN)
         Not_With ("density_map","this density type");
       Destroy_Blend_Map(New->Blend_Map);
       New->Blend_Map = Parse_Blend_Map (DENSITY_TYPE,New->Type);
     END_CASE

     CASE (SLOPE_MAP_TOKEN)
       if (TPat_Type != NORMAL_TYPE)
       {
         Only_In("slope_map","normal");
       }
       if (New->Type == CHECKER_PATTERN ||
           New->Type == BRICK_PATTERN ||
           New->Type == HEXAGON_PATTERN ||
           New->Type == PLAIN_PATTERN ||
           New->Type == AVERAGE_PATTERN ||
           New->Type == BITMAP_PATTERN)
         Not_With ("slope_map","this normal type");
       Destroy_Blend_Map(New->Blend_Map);
       New->Blend_Map = Parse_Blend_Map (SLOPE_TYPE,New->Type);
     END_CASE

     CASE (NORMAL_MAP_TOKEN)
       if (TPat_Type != NORMAL_TYPE)
       {
         Only_In("normal_map","normal");
       }
       if (New->Type == CHECKER_PATTERN ||
           New->Type == BRICK_PATTERN ||
           New->Type == HEXAGON_PATTERN ||
           New->Type == PLAIN_PATTERN ||
           New->Type == BITMAP_PATTERN)
         Not_With ("normal_map","this normal type");
       Destroy_Blend_Map(New->Blend_Map);
       New->Blend_Map = Parse_Blend_Map (NORMAL_TYPE,New->Type);
     END_CASE

     CASE (TEXTURE_MAP_TOKEN)
       if (TPat_Type != TEXTURE_TYPE)
       {
         Only_In("texture_map","texture");
       }
       if (New->Type == CHECKER_PATTERN ||
           New->Type == BRICK_PATTERN ||
           New->Type == HEXAGON_PATTERN ||
           New->Type == PLAIN_PATTERN ||
           New->Type == BITMAP_PATTERN)
         Not_With ("texture_map","this pattern type");
       Destroy_Blend_Map(New->Blend_Map);
       New->Blend_Map = Parse_Blend_Map (TEXTURE_TYPE,New->Type);
     END_CASE

     CASE (QUICK_COLOUR_TOKEN)
       if (TPat_Type != PIGMENT_TYPE)
       {
         Only_In("quick_color","pigment");
       }
       Parse_Colour (Local_Colour);
       if (opts.Quality_Flags & Q_QUICKC)
       {
          New->Type = PLAIN_PATTERN;
          Assign_Colour(((PIGMENT *)New)->Colour,Local_Colour);
       }
     END_CASE

     CASE (CONTROL0_TOKEN)
       if (New->Type == QUILTED_PATTERN) 
       {
         New->Vals.Quilted.Control0 = Parse_Float ();
       }
       else
       {
         Not_With ("control0","this normal");
       }
     END_CASE

     CASE (CONTROL1_TOKEN)
       if (New->Type == QUILTED_PATTERN)
       {
         New->Vals.Quilted.Control1 = Parse_Float ();
       }
       else
       {
         Not_With ("control1","this normal");
       }
     END_CASE

     CASE (OCTAVES_TOKEN)
       Local_Turb=Check_Turb(&(New->Warps));
       Local_Turb->Octaves = (int)Parse_Float();
       if(Local_Turb->Octaves < 1)
          Local_Turb->Octaves = 1;
       if(Local_Turb->Octaves > 10)  /* Avoid DOMAIN errors */
          Local_Turb->Octaves = 10;
     END_CASE

     CASE (OMEGA_TOKEN)
       Local_Turb=Check_Turb(&(New->Warps));
       Local_Turb->Omega = Parse_Float();
     END_CASE

     CASE (LAMBDA_TOKEN)
       Local_Turb=Check_Turb(&(New->Warps));
       Local_Turb->Lambda = Parse_Float();
     END_CASE

     CASE (FREQUENCY_TOKEN)
       New->Frequency = Parse_Float();
     END_CASE

     CASE (RAMP_WAVE_TOKEN)
       New->Wave_Type = RAMP_WAVE;
     END_CASE

     CASE (TRIANGLE_WAVE_TOKEN)
       New->Wave_Type = TRIANGLE_WAVE;
     END_CASE

     CASE (SINE_WAVE_TOKEN)
       New->Wave_Type = SINE_WAVE;
     END_CASE

     CASE (SCALLOP_WAVE_TOKEN)
       New->Wave_Type = SCALLOP_WAVE;
     END_CASE

     CASE (CUBIC_WAVE_TOKEN)
       New->Wave_Type = CUBIC_WAVE;
     END_CASE

     CASE (POLY_WAVE_TOKEN)
       New->Wave_Type = POLY_WAVE;
       New->Exponent  = Allow_Float(New->Exponent);
     END_CASE

     CASE (PHASE_TOKEN)
       New->Phase = Parse_Float();
     END_CASE

     CASE (BUMP_SIZE_TOKEN)
       if (TPat_Type != NORMAL_TYPE)
          Only_In ("bump_size","normal");
       ((TNORMAL *)New)->Amount = Parse_Float ();
     END_CASE

     CASE (AGATE_TURB_TOKEN)
       if (New->Type != AGATE_PATTERN)
          Not_With ("agate_turb","non-agate");
       New->Vals.Agate_Turb_Scale = Parse_Float();
       Check_Turb(&(New->Warps));   /* agate needs Octaves, Lambda etc. */
     END_CASE

     CASE (BRICK_SIZE_TOKEN)
       if (New->Type != BRICK_PATTERN)
          Not_With ("brick_size","non-brick");
       Parse_Vector(New->Vals.Brick.Size);
     END_CASE

     CASE (MORTAR_TOKEN)
       if (New->Type != BRICK_PATTERN)
          Not_With ("mortar","non-brick");
       New->Vals.Brick.Mortar = Parse_Float()-Small_Tolerance*2.0;
     END_CASE

     CASE (INTERPOLATE_TOKEN)
       if (New->Type != DENSITY_FILE_PATTERN)
          Not_With ("interpolate","non-density_file");
       New->Vals.Density_File->Interpolation = (int)Parse_Float();
     END_CASE

     CASE (WARP_TOKEN)
       Parse_Warp(&(New->Warps));
     END_CASE

     CASE (TRANSLATE_TOKEN)
       Parse_Vector (Local_Vector);
       Translate_Tpattern (New, Local_Vector);
     END_CASE

     CASE (ROTATE_TOKEN)
       Parse_Vector (Local_Vector);
       Rotate_Tpattern (New, Local_Vector);
     END_CASE

     CASE (SCALE_TOKEN)
       Parse_Scale_Vector (Local_Vector);
       Scale_Tpattern (New, Local_Vector);
     END_CASE

     CASE (MATRIX_TOKEN)
       Parse_Matrix(Local_Matrix);
       Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
       Transform_Tpattern (New, &Local_Trans);
     END_CASE

     CASE (TRANSFORM_TOKEN)
       GET(TRANSFORM_ID_TOKEN)
       Transform_Tpattern (New, (TRANSFORM *)Token.Data);
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT
   
   if ((New->Type==AVERAGE_PATTERN) && (New->Blend_Map==NULL))
   {
      Error("Average must have map.");
   }

   if ((TPat_Type==TEXTURE_TYPE) && (New->Type!=PLAIN_PATTERN) &&
       (New->Blend_Map==NULL))
   {
      Error("Patterned texture must have texture_map.");
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

void Parse_Tnormal (TNORMAL **Tnormal_Ptr)
{
   EXPECT            /* Look for [tnormal_id] */
     CASE (TNORMAL_ID_TOKEN)
       Destroy_Tnormal(*Tnormal_Ptr);
       *Tnormal_Ptr = Copy_Tnormal ((TNORMAL *) Token.Data);
       EXIT
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT    /* End [tnormal_id] */

   if (*Tnormal_Ptr == NULL)
   { /* tw */
     if ((Default_Texture->Tnormal) != NULL)
       *Tnormal_Ptr = Copy_Tnormal ((Default_Texture->Tnormal));
     else
       *Tnormal_Ptr = Create_Tnormal ();
   } /* tw */

   Parse_Pattern((TPATTERN *)*Tnormal_Ptr,NORMAL_TYPE);
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

void Parse_Finish (FINISH **Finish_Ptr)
{
   COLOUR Temp_Colour;
   FINISH *New;
   VECTOR Local_Vector;

   Parse_Begin ();

   EXPECT        /* Look for zero or one finish_id */
     CASE (FINISH_ID_TOKEN)
       Destroy_Finish(*Finish_Ptr);
       *Finish_Ptr = Copy_Finish ((FINISH *) Token.Data);
       EXIT
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT    /* End finish_id */

   New = *Finish_Ptr;

   EXPECT        /* Look for zero or more finish_body */
     CASE (AMBIENT_TOKEN)
       Parse_Colour(Temp_Colour);
       New->Ambient[RED]   = Temp_Colour[RED];
       New->Ambient[GREEN] = Temp_Colour[GREEN];
       New->Ambient[BLUE]  = Temp_Colour[BLUE];
     END_CASE

     CASE (BRILLIANCE_TOKEN)
       New->Brilliance = Parse_Float ();
     END_CASE

     CASE (DIFFUSE_TOKEN)
       New->Diffuse = Parse_Float ();
     END_CASE

     CASE (REFLECTION_TOKEN)
       Parse_Colour(Temp_Colour);
       New->Reflection[RED]   = Temp_Colour[RED];
       New->Reflection[GREEN] = Temp_Colour[GREEN];
       New->Reflection[BLUE]  = Temp_Colour[BLUE];
     END_CASE

     CASE (REFLECTION_EXPONENT_TOKEN)
       New->Reflect_Exp = 1.0 / Parse_Float ();
     END_CASE

     CASE (PHONG_TOKEN)
       New->Phong = Parse_Float ();
     END_CASE

     CASE (PHONG_SIZE_TOKEN)
       New->Phong_Size = Parse_Float ();
     END_CASE

     CASE (SPECULAR_TOKEN)
       New->Specular = Parse_Float ();
     END_CASE

     CASE (ROUGHNESS_TOKEN)
       New->Roughness = Parse_Float ();
       if (New->Roughness != 0.0)
       New->Roughness = 1.0/New->Roughness; /* CEY 12/92 */
       else
         Warn(0.0, "Zero roughness used.");
     END_CASE

     CASE (METALLIC_TOKEN)
       New->Metallic = 1.0;
       EXPECT
         CASE_FLOAT
           New->Metallic = Parse_Float();
           EXIT
         END_CASE

         OTHERWISE
           UNGET
           EXIT
         END_CASE
       END_EXPECT
     END_CASE

     CASE (CRAND_TOKEN)
       New->Crand = Parse_Float();
     END_CASE

     CASE (IRID_TOKEN)                     /* DMF */
       Parse_Begin();
       New->Irid = Parse_Float();

       EXPECT
         CASE (THICKNESS_TOKEN)           /* DMF */
           New->Irid_Film_Thickness = Parse_Float();
         END_CASE

         CASE (TURBULENCE_TOKEN)                /* DMF */
           Parse_Vector(Local_Vector);
           New->Irid_Turb = Local_Vector[X];
         END_CASE

         OTHERWISE
           UNGET
           EXIT
         END_CASE
       END_EXPECT
       Parse_End();
     END_CASE
     
     CASE (IOR_TOKEN)
       New->Temp_IOR = Parse_Float();
       Warn_Interior("Index of refraction value");
     END_CASE

     CASE (CAUSTICS_TOKEN)
       New->Temp_Caustics = Parse_Float();
       Warn_Interior("Caustics value");
     END_CASE

     CASE (REFRACTION_TOKEN)
       New->Temp_Refract = Parse_Float();
       Warn_Interior("Refraction value unnecessary to turn on refraction.\nTo attenuate, the fade_power and fade_distance keywords ");
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
     
   END_EXPECT    /* End of finish_body */

   EXPECT        /* Look for finish_mods */

/*   CASE none implemented
     END_CASE     */

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT    /* End of finish_mods */

   Parse_End ();
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

TEXTURE *Parse_Texture ()
{
   VECTOR Local_Vector;
   MATRIX Local_Matrix;
   TRANSFORM Local_Trans;
   TEXTURE *Texture;
   int Modified_Pnf;

   if (opts.Language_Version < 3.0)
   {
     return(Parse_Vers1_Texture());
   }

   Modified_Pnf = FALSE;

   EXPECT               /* First allow a texture identifier */
     CASE (TEXTURE_ID_TOKEN)
       Texture = Copy_Textures((TEXTURE *) Token.Data);
       Modified_Pnf = TRUE;
       EXIT
     END_CASE

     OTHERWISE
       UNGET
       Texture = Copy_Textures (Default_Texture);
       EXIT
     END_CASE
   END_EXPECT

   /* If the texture identifer or the default texture was a PLAIN_PATTERN
      then allow its pigment, normal or finish to be overridden by
      pigment identifier, normal identifier and finish identifiers.
      This is a consession to backwards compatibility so that
      "texture{PIGMENT_IDENTIFIER}" etc. is legal even though it should
      be "texture{pigment{PIGMENT_IDENTIFIER}}"
   */

   /* Look for [pnf_texture] */
   if (Texture->Type == PLAIN_PATTERN)
   {
     EXPECT   /* Look for [pnf_ids] */
       CASE (PIGMENT_ID_TOKEN)
         Warn_State(Token.Token_Id, PIGMENT_TOKEN);
         Destroy_Pigment(Texture->Pigment);
         Texture->Pigment = Copy_Pigment ((PIGMENT *) Token.Data);
         Modified_Pnf = TRUE;
       END_CASE

       CASE (TNORMAL_ID_TOKEN)
         Warn_State(Token.Token_Id, TNORMAL_TOKEN);
         Destroy_Tnormal(Texture->Tnormal);
         Texture->Tnormal = Copy_Tnormal ((TNORMAL *) Token.Data);
         Modified_Pnf = TRUE;
       END_CASE

       CASE (FINISH_ID_TOKEN)
         Warn_State(Token.Token_Id, FINISH_TOKEN);
         Destroy_Finish(Texture->Finish);
         Texture->Finish = Copy_Finish ((FINISH *) Token.Data);
         Modified_Pnf = TRUE;
       END_CASE

       OTHERWISE
         UNGET
         EXIT
       END_CASE
     END_EXPECT

   /* If the texture identifer or the default texture was a PLAIN_PATTERN
      then allow its pigment, normal or finish to be overridden by
      pigment, normal or finish statement.  Also allow transformations.
   */

     EXPECT   /* Modify previous pnf */
       CASE (PIGMENT_TOKEN)
         Parse_Begin ();
         Parse_Pigment ( &(Texture->Pigment) );
         Parse_End ();
         Modified_Pnf = TRUE;
       END_CASE

       CASE (TNORMAL_TOKEN)
         Parse_Begin ();
         Parse_Tnormal ( &(Texture->Tnormal) );
         Parse_End ();
         Modified_Pnf = TRUE;
       END_CASE

       CASE (FINISH_TOKEN)
         Parse_Finish ( &(Texture->Finish) );
         Modified_Pnf = TRUE;
       END_CASE

       CASE (TRANSLATE_TOKEN)
         Parse_Vector (Local_Vector);
         Compute_Translation_Transform(&Local_Trans, Local_Vector);
         Transform_Textures (Texture, &Local_Trans);
         Modified_Pnf = TRUE;
       END_CASE

       CASE (ROTATE_TOKEN)
         Parse_Vector (Local_Vector);
         Compute_Rotation_Transform(&Local_Trans, Local_Vector);
         Transform_Textures (Texture, &Local_Trans);
         Modified_Pnf = TRUE;
       END_CASE

       CASE (SCALE_TOKEN)
         Parse_Scale_Vector (Local_Vector);
         Compute_Scaling_Transform(&Local_Trans, Local_Vector);
         Transform_Textures (Texture, &Local_Trans);
         Modified_Pnf = TRUE;
       END_CASE

       CASE (MATRIX_TOKEN)
         Parse_Matrix(Local_Matrix);
         Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
         Transform_Textures (Texture, &Local_Trans);
         Modified_Pnf = TRUE;
       END_CASE

       CASE (TRANSFORM_TOKEN)
         GET(TRANSFORM_ID_TOKEN)
         Transform_Textures (Texture, (TRANSFORM *)Token.Data);
         Modified_Pnf = TRUE;
       END_CASE

       OTHERWISE
         UNGET
         EXIT
       END_CASE
     END_EXPECT
    }
    else
    {
    /* Here it is not a PLAIN_PATTERN texture and since default textures
       must be plain then this was a texture identifier that was a special 
       texture.  Allow transforms.  The "if(!Modified_Pnf)..." below
       will always fail if we came here.  So we return after the
       transforms. */
      Parse_Texture_Transform(Texture);
    }

    /* If we've modified the default texture with a p,n, or f then this
       has to stay a PLAIN_PATTERN pnf texture.  We won't allow
       a texture_map or pattern.  Therefore quit now.
     */

    if (!Modified_Pnf)
    {
     /* At this point we've either got a texture statement that had
        no p, n or f.  Nor any texture identifier.  Its probably 
        a patterned texture_map texture. It could be an empty
        statement such as "texture{}" */
       
       EXPECT
         CASE (TILES_TOKEN)
           Destroy_Textures (Texture);
           Texture = Parse_Tiles();
           Parse_Texture_Transform(Texture);
           EXIT
         END_CASE

         CASE (MATERIAL_MAP_TOKEN)
           Destroy_Textures (Texture);
           Texture = Parse_Material_Map ();
           Parse_Texture_Transform(Texture);
           EXIT
         END_CASE

         OTHERWISE
           UNGET;
           Destroy_Pigment(Texture->Pigment);
           Destroy_Tnormal(Texture->Tnormal);
           Destroy_Finish(Texture->Finish);
           Texture->Pigment = NULL;
           Texture->Tnormal = NULL;
           Texture->Finish  = NULL;
           Parse_Pattern((TPATTERN *)Texture,TEXTURE_TYPE);
           /* if following is true, parsed "texture{}" so restore
              default texture.
            */
           if (Texture->Type <= PLAIN_PATTERN)
           {
             Destroy_Textures(Texture);
             Texture = Copy_Textures (Default_Texture);
           }
           EXIT
         END_CASE
      END_EXPECT
    }

    return (Texture);
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

static TEXTURE *Parse_Tiles()
{
  TEXTURE *Texture, *Local_Texture;
  BLEND_MAP_ENTRY *Entry;

  Parse_Begin ();

  Texture = Create_Texture ();
  Destroy_Pigment(Texture->Pigment);
  Destroy_Tnormal(Texture->Tnormal);
  Destroy_Finish(Texture->Finish);
  Texture->Pigment = NULL;
  Texture->Tnormal = NULL;
  Texture->Finish  = NULL;
  Texture->Type = CHECKER_PATTERN;

  Texture->Blend_Map = Create_Blend_Map();
  Texture->Blend_Map->Number_Of_Entries = 2;
  Texture->Blend_Map->Blend_Map_Entries = Entry = Create_BMap_Entries (2);
  Texture->Blend_Map->Type = TEXTURE_TYPE;
  Entry[0].Vals.Texture=NULL;
  Entry[0].value=0.0;
  Entry[0].Same=FALSE;
  Entry[1].Vals.Texture=NULL;
  Entry[1].value=1.0;
  Entry[1].Same=FALSE;
  
  /* Note first tile is 1, 2nd tile is 0 to keep compatible with old tiles */

  EXPECT
    CASE (TEXTURE_TOKEN)
      Parse_Begin ();
      Local_Texture = Parse_Texture ();
      Link_Textures(&(Entry[1].Vals.Texture),Local_Texture);
      Parse_End ();
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  GET (TILE2_TOKEN);

  EXPECT
    CASE (TEXTURE_TOKEN)
      Parse_Begin ();
      Local_Texture = Parse_Texture ();
      Link_Textures(&(Entry[0].Vals.Texture),Local_Texture);
      Parse_End ();
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  Parse_End ();

  return (Texture);
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

static TEXTURE *Parse_Material_Map()
{
  TEXTURE *Texture, *Local_Texture;
  Parse_Begin ();

  Texture = Create_Texture ();
  Destroy_Pigment(Texture->Pigment);
  Destroy_Tnormal(Texture->Tnormal);
  Destroy_Finish(Texture->Finish);
  Texture->Pigment = NULL;
  Texture->Tnormal = NULL;
  Texture->Finish  = NULL;
  Texture->Type = BITMAP_PATTERN;

  Texture->Vals.Image = Parse_Image(MATERIAL_FILE);
  Texture->Vals.Image->Use_Colour_Flag = FALSE;

  EXPECT
    CASE (ONCE_TOKEN)
      Texture->Vals.Image->Once_Flag=TRUE;
    END_CASE

    CASE (INTERPOLATE_TOKEN)
      Texture->Vals.Image->Interpolation_Type=(int)Parse_Float();
    END_CASE

    CASE (MAP_TYPE_TOKEN)
      Texture->Vals.Image->Map_Type = (int) Parse_Float ();
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  GET (TEXTURE_TOKEN)                /* First material */
  Parse_Begin();
  Texture->Materials = Local_Texture = Parse_Texture ();
  Parse_End();
  Texture->Num_Of_Mats++;

  EXPECT                             /* Subsequent materials */
    CASE (TEXTURE_TOKEN)
      Parse_Begin();
      Local_Texture->Next_Material = Parse_Texture ();
      Parse_End();
      Local_Texture = Local_Texture->Next_Material;
      Texture->Num_Of_Mats++;
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  Parse_End ();
  
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

static TEXTURE *Parse_Vers1_Texture ()
{
   VECTOR Local_Vector;
   COLOUR Local_Colour;
   MATRIX Local_Matrix;
   TRANSFORM Local_Trans;
   TURB *Local_Turb;
   TEXTURE *Texture;
   PIGMENT *Pigment;
   TNORMAL *Tnormal;
   FINISH *Finish;

   EXPECT                      /* Look for texture_body */
     CASE (TILES_TOKEN)
       Texture = Parse_Tiles();
       EXIT
     END_CASE

     CASE (MATERIAL_MAP_TOKEN)
       Texture = Parse_Material_Map ();
       EXIT
     END_CASE

     CASE (TEXTURE_ID_TOKEN)
       Texture = Copy_Textures((TEXTURE *) Token.Data);
       EXIT
     END_CASE

     OTHERWISE
       UNGET
       Texture = Copy_Textures (Default_Texture);
       EXIT
     END_CASE
   END_EXPECT

   /* Look for [pnf_texture] */
   if (Texture->Type == PLAIN_PATTERN)
     {
       EXPECT   /* Look for [pnf_ids] */
         CASE (PIGMENT_ID_TOKEN)
           Destroy_Pigment(Texture->Pigment);
           Texture->Pigment = Copy_Pigment ((PIGMENT *) Token.Data);
         END_CASE

         CASE (TNORMAL_ID_TOKEN)
           Destroy_Tnormal(Texture->Tnormal);
           Texture->Tnormal = Copy_Tnormal ((TNORMAL *) Token.Data);
         END_CASE

         CASE (FINISH_ID_TOKEN)
           Destroy_Finish(Texture->Finish);
           Texture->Finish = Copy_Finish ((FINISH *) Token.Data);
         END_CASE

         OTHERWISE
           UNGET
           EXIT
         END_CASE
       END_EXPECT

       Pigment = Texture->Pigment;
       Tnormal = Texture->Tnormal;
       Finish  = Texture->Finish;

       EXPECT
         CASE (PIGMENT_TOKEN)
           Parse_Begin ();
           Parse_Pigment ( &(Texture->Pigment) );
           Parse_End ();
         END_CASE

         CASE (TNORMAL_TOKEN)
           Parse_Begin ();
           Parse_Tnormal ( &(Texture->Tnormal) );
           Parse_End ();
         END_CASE

         CASE (FINISH_TOKEN)
           Parse_Finish ( &(Texture->Finish) );
         END_CASE

/***********************************************************************
PIGMENT STUFF OUTSIDE PIGMENT{}
NOTE: Do not add new keywords to this section.  Use 1.0 syntax only.
***********************************************************************/
         CASE (AGATE_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = AGATE_PATTERN;
           Pigment->Vals.Agate_Turb_Scale = 1.0;
           Check_Turb(&(Pigment->Warps));   /* agate needs Octaves, Lambda etc. */
         END_CASE

         CASE (BOZO_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = BOZO_PATTERN;
         END_CASE

         CASE (GRANITE_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = GRANITE_PATTERN;
         END_CASE

         CASE (LEOPARD_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = LEOPARD_PATTERN;
         END_CASE

         CASE (MARBLE_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = MARBLE_PATTERN;
         END_CASE

         CASE (MANDEL_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = MANDEL_PATTERN;
           Pigment->Vals.Iterations = (int)Parse_Float();
         END_CASE

         CASE (ONION_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = ONION_PATTERN;
         END_CASE

         CASE (SPOTTED_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = SPOTTED_PATTERN;
         END_CASE

         CASE (WOOD_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = WOOD_PATTERN;
         END_CASE

         CASE (GRADIENT_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = GRADIENT_PATTERN;
           Parse_Vector (Pigment->Vals.Gradient);
         END_CASE

         CASE_COLOUR
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = PLAIN_PATTERN;
           Parse_Colour (Pigment->Colour);
         END_CASE

         CASE (CHECKER_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = CHECKER_PATTERN;
           Pigment->Frequency = 0.0;
           Destroy_Blend_Map(Pigment->Blend_Map);
           Pigment->Blend_Map = Parse_Blend_List(2,&Check_Default_Map,COLOUR_TYPE);
         END_CASE

         CASE (HEXAGON_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = HEXAGON_PATTERN;
           Pigment->Frequency = 0.0;
           Destroy_Blend_Map(Pigment->Blend_Map);
           Pigment->Blend_Map = Parse_Blend_List(3,&Hex_Default_Map,COLOUR_TYPE);
         END_CASE

         CASE (IMAGE_MAP_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Pigment->Type = BITMAP_PATTERN;
           Pigment->Frequency = 0.0;
           Parse_Image_Map (Pigment);
         END_CASE

         CASE (TURBULENCE_TOKEN)
           Local_Turb=Check_Turb(&(Pigment->Warps));
           Parse_Vector(Local_Turb->Turbulence);
         END_CASE

         CASE (COLOUR_MAP_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           if (Pigment->Type == CHECKER_PATTERN ||
               Pigment->Type == HEXAGON_PATTERN ||
               Pigment->Type == PLAIN_PATTERN ||
               Pigment->Type == BITMAP_PATTERN)
             Warn(1.5, "Cannot use color map with this pigment type.");
           Destroy_Blend_Map(Pigment->Blend_Map);
           Pigment->Blend_Map = Parse_Colour_Map ();
         END_CASE

         CASE (QUICK_COLOUR_TOKEN)
           Warn_State(Token.Token_Id, PIGMENT_TOKEN);
           Parse_Colour (Local_Colour);
           if (opts.Quality_Flags & Q_QUICKC)
           {
              Assign_Colour(Pigment->Colour,Local_Colour);
           }
         END_CASE

         CASE (OCTAVES_TOKEN)
           Local_Turb=Check_Turb(&(Pigment->Warps));
           Local_Turb->Octaves = (int)Parse_Float();
           if(Local_Turb->Octaves < 1)
              Local_Turb->Octaves = 1;
           if(Local_Turb->Octaves > 10)  /* Avoid DOMAIN errors */
              Local_Turb->Octaves = 10;
         END_CASE

         CASE (OMEGA_TOKEN)
           Local_Turb=Check_Turb(&(Pigment->Warps));
           Local_Turb->Omega = Parse_Float();
         END_CASE

         CASE (LAMBDA_TOKEN)
           Local_Turb=Check_Turb(&(Pigment->Warps));
           Local_Turb->Lambda = Parse_Float();
         END_CASE

/***********************************************************************
TNORMAL STUFF OUTSIDE NORMAL{}
NOTE: Do not add new keywords to this section.  Use 1.0 syntax only.
***********************************************************************/
         CASE (BUMPS_TOKEN)
           Warn_State(Token.Token_Id, TNORMAL_TOKEN);
           ADD_TNORMAL
           Tnormal->Type = BUMPS_PATTERN;
           Tnormal->Amount = Parse_Float ();
         END_CASE

         CASE (DENTS_TOKEN)
           Warn_State(Token.Token_Id, TNORMAL_TOKEN);
           ADD_TNORMAL
           Tnormal->Type = DENTS_PATTERN;
           Tnormal->Amount = Parse_Float ();
         END_CASE

         CASE (RIPPLES_TOKEN)
           Warn_State(Token.Token_Id, TNORMAL_TOKEN);
           ADD_TNORMAL
           Tnormal->Type = RIPPLES_PATTERN;
           Tnormal->Amount = Parse_Float ();
         END_CASE

         CASE (WAVES_TOKEN)
           Warn_State(Token.Token_Id, TNORMAL_TOKEN);
           ADD_TNORMAL
           Tnormal->Type = WAVES_PATTERN;
           Tnormal->Amount = Parse_Float ();
         END_CASE

         CASE (WRINKLES_TOKEN)
           Warn_State(Token.Token_Id, TNORMAL_TOKEN);
           ADD_TNORMAL
           Tnormal->Type = WRINKLES_PATTERN;
           Tnormal->Amount = Parse_Float ();
         END_CASE

         CASE (BUMP_MAP_TOKEN)
           Warn_State(Token.Token_Id, TNORMAL_TOKEN);
           ADD_TNORMAL
           Tnormal->Type = BITMAP_PATTERN;
           Tnormal->Frequency = 0.0;
           Parse_Bump_Map (Tnormal);
         END_CASE

         CASE (FREQUENCY_TOKEN)
           Warn_State(Token.Token_Id, TNORMAL_TOKEN);
           ADD_TNORMAL
           if (!(Tnormal->Type == RIPPLES_PATTERN || Tnormal->Type == WAVES_PATTERN))
             if (opts.Language_Version >= 1.5)
               Warn(1.5, "Cannot use frequency with this normal.");
           Tnormal->Frequency = Parse_Float();
         END_CASE

         CASE (PHASE_TOKEN)
           Warn_State(Token.Token_Id, TNORMAL_TOKEN);
           ADD_TNORMAL
           if (!(Tnormal->Type == RIPPLES_PATTERN || Tnormal->Type == WAVES_PATTERN))
             if (opts.Language_Version >= 1.5)
               Warn(1.5, "Cannot use phase with this normal.");
           Tnormal->Phase = Parse_Float();
         END_CASE


/***********************************************************************
FINISH STUFF OUTSIDE FINISH{}
NOTE: Do not add new keywords to this section.  Use 1.0 syntax only.
***********************************************************************/
         CASE (AMBIENT_TOKEN)
           Warn_State(Token.Token_Id, FINISH_TOKEN);
           Finish->Ambient[RED]   =
           Finish->Ambient[GREEN] =
           Finish->Ambient[BLUE]  = Parse_Float ();
         END_CASE

         CASE (BRILLIANCE_TOKEN)
           Warn_State(Token.Token_Id, FINISH_TOKEN);
           Finish->Brilliance = Parse_Float ();
         END_CASE

         CASE (DIFFUSE_TOKEN)
           Warn_State(Token.Token_Id, FINISH_TOKEN);
           Finish->Diffuse = Parse_Float ();
         END_CASE

         CASE (REFLECTION_TOKEN)
           Warn_State(Token.Token_Id, FINISH_TOKEN);
           Finish->Reflection[RED]   =
           Finish->Reflection[GREEN] =
           Finish->Reflection[BLUE]  = Parse_Float ();
         END_CASE

         CASE (PHONG_TOKEN)
           Warn_State(Token.Token_Id, FINISH_TOKEN);
           Finish->Phong = Parse_Float ();
         END_CASE

         CASE (PHONG_SIZE_TOKEN)
           Warn_State(Token.Token_Id, FINISH_TOKEN);
           Finish->Phong_Size = Parse_Float ();
         END_CASE

         CASE (SPECULAR_TOKEN)
           Warn_State(Token.Token_Id, FINISH_TOKEN);
           Finish->Specular = Parse_Float ();
         END_CASE

         CASE (ROUGHNESS_TOKEN)
           Warn_State(Token.Token_Id, FINISH_TOKEN);
           Finish->Roughness = Parse_Float ();
           if (Finish->Roughness != 0.0)
           Finish->Roughness = 1.0/Finish->Roughness; /* CEY 12/92 */
           else
             Warn(0.0, "Zero roughness used.");
         END_CASE

         CASE (METALLIC_TOKEN)
           Warn_State(Token.Token_Id, FINISH_TOKEN);
           Finish->Metallic = 1.0;
         END_CASE

         CASE (CRAND_TOKEN)
           Warn_State(Token.Token_Id, FINISH_TOKEN);
           Finish->Crand = Parse_Float();
         END_CASE

         CASE_FLOAT
           Finish->Crand = Parse_Float();
           Warn(1.5, "Should use crand keyword in finish statement.");
         END_CASE

         CASE (IOR_TOKEN)
           Warn_State(Token.Token_Id, INTERIOR_TOKEN);
           Finish->Temp_IOR = Parse_Float();
           Warn_Interior("Index of refraction value");
         END_CASE

         CASE (REFRACTION_TOKEN)
           Warn_State(Token.Token_Id, INTERIOR_TOKEN);
           Finish->Temp_Refract = Parse_Float();
           Warn_Interior("Refraction value unnecessary to turn on refraction.\nTo attenuate, the fade_power and fade_distance keywords ");
         END_CASE

         CASE (TRANSLATE_TOKEN)
           Parse_Vector (Local_Vector);
           Compute_Translation_Transform(&Local_Trans, Local_Vector);
           Transform_Textures (Texture, &Local_Trans);
         END_CASE

         CASE (ROTATE_TOKEN)
           Parse_Vector (Local_Vector);
           Compute_Rotation_Transform(&Local_Trans, Local_Vector);
           Transform_Textures (Texture, &Local_Trans);
         END_CASE

         CASE (SCALE_TOKEN)
           Parse_Scale_Vector (Local_Vector);
           Compute_Scaling_Transform(&Local_Trans, Local_Vector);
           Transform_Textures (Texture, &Local_Trans);
         END_CASE

         CASE (MATRIX_TOKEN)
           Parse_Matrix(Local_Matrix);
           Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
           Transform_Textures (Texture, &Local_Trans);
         END_CASE

         CASE (TRANSFORM_TOKEN)
           GET(TRANSFORM_ID_TOKEN)
           Transform_Textures (Texture, (TRANSFORM *)Token.Data);
         END_CASE

         CASE (TEXTURE_ID_TOKEN)
           Warn(0.0, "Texture identifier overwriting previous values.");
           Destroy_Textures(Texture);
           Texture = Copy_Textures((TEXTURE *) Token.Data);
           Pigment = Texture->Pigment;
           Tnormal = Texture->Tnormal;
           Finish  = Texture->Finish;
         END_CASE

         OTHERWISE
           UNGET
           EXIT
         END_CASE

/***********************************************************************/

       END_EXPECT

       if (Not_In_Default && (Texture->Pigment->Type == NO_PATTERN) &&
           !(opts.Language_Version < 1.5))
         Parse_Error(PIGMENT_ID_TOKEN);

     }

   Parse_Texture_Transform(Texture);

   return (Texture);
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

static void Parse_Texture_Transform (TEXTURE *Texture)
{
   VECTOR Local_Vector;
   MATRIX Local_Matrix;
   TRANSFORM Local_Trans;

   EXPECT
     CASE (TRANSLATE_TOKEN)
       Parse_Vector (Local_Vector);
       Compute_Translation_Transform(&Local_Trans, Local_Vector);
       Transform_Textures (Texture, &Local_Trans);
     END_CASE

     CASE (ROTATE_TOKEN)
       Parse_Vector (Local_Vector);
       Compute_Rotation_Transform(&Local_Trans, Local_Vector);
       Transform_Textures (Texture, &Local_Trans);
     END_CASE

     CASE (SCALE_TOKEN)
       Parse_Scale_Vector (Local_Vector);
       Compute_Scaling_Transform(&Local_Trans, Local_Vector);
       Transform_Textures (Texture, &Local_Trans);
     END_CASE

     CASE (MATRIX_TOKEN)
       Parse_Matrix(Local_Matrix);
       Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
       Transform_Textures (Texture, &Local_Trans);
     END_CASE

     CASE (TRANSFORM_TOKEN)
       GET(TRANSFORM_ID_TOKEN)
       Transform_Textures (Texture, (TRANSFORM *)Token.Data);
     END_CASE

     OTHERWISE
       UNGET
       EXIT
     END_CASE
   END_EXPECT
}


/*****************************************************************************
*
* FUNCTION
*
*   Parse_Media
*
* INPUT
*
* OUTPUT
*
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

void Parse_Media(IMEDIA **Media_Ptr)
{
  IMEDIA *IMedia, *Next_Media;
  TRANSFORM Local_Trans;
  VECTOR Local_Vector;
  MATRIX Local_Matrix;

  Next_Media = *Media_Ptr;

  Parse_Begin();

  EXPECT
    CASE(MEDIA_ID_TOKEN)
      IMedia = Copy_Media((IMEDIA *)Token.Data);
      EXIT
    END_CASE

    OTHERWISE
      UNGET
        IMedia = Create_Media();
      EXIT
    END_CASE
  END_EXPECT

  EXPECT
    CASE (INTERVALS_TOKEN)
      if ((IMedia->Intervals = (int)Parse_Float()) < 1)
      {
        Error("At least one interval is needed in media.\n");
      }
    END_CASE

    CASE (SAMPLES_TOKEN)
      IMedia->Min_Samples = (int)Parse_Float();
      Parse_Comma();
      IMedia->Max_Samples = (int)Parse_Float();
      if (IMedia->Min_Samples < 1)
      {
        Error("At least one sample per interval is needed in media.\n");
      }
      if (IMedia->Max_Samples < IMedia->Min_Samples)
      {
        Error("Maximum number of samples per interval smaller than minimum number.\n");
      }
    END_CASE

    CASE (ABSORPTION_TOKEN)
      Parse_Colour(IMedia->Absorption);
    END_CASE

    CASE (EMISSION_TOKEN)
      Parse_Colour(IMedia->Emission);
    END_CASE

    CASE (SCATTERING_TOKEN)
      Parse_Begin();
      IMedia->Type = (int)Parse_Float();
      if ((IMedia->Type < 1) || (IMedia->Type > SCATTERING_TYPES))
      {
        Warn(0.0, "Unknown atmospheric scattering type.");
      }
      Parse_Comma();
      Parse_Colour(IMedia->Scattering);

      EXPECT
        CASE (ECCENTRICITY_TOKEN)
          if (IMedia->Type != HENYEY_GREENSTEIN_SCATTERING)
          {
             Error("Eccentricity cannot be used with this scattering type.");
          }
          IMedia->Eccentricity = Parse_Float();
        END_CASE

        CASE (EXTINCTION_TOKEN)
          IMedia->sc_ext = Parse_Float();
        END_CASE

        OTHERWISE
          UNGET
          EXIT
        END_CASE
      END_EXPECT

      Parse_End();
    END_CASE

    CASE (CONFIDENCE_TOKEN)
      IMedia->Confidence = Parse_Float();
      if ((IMedia->Confidence <= 0.0) || (IMedia->Confidence >= 1.0))
      {
        Error("Illegal confidence value in media.\n");
      }
    END_CASE

    CASE (VARIANCE_TOKEN)
      IMedia->Variance = Parse_Float();
    END_CASE

    CASE (RATIO_TOKEN)
      IMedia->Ratio = Parse_Float();
    END_CASE

    CASE (DENSITY_TOKEN)
      Parse_Begin();
      Parse_Media_Density_Pattern(&(IMedia->Density));
      Parse_End();
    END_CASE

    CASE (TRANSLATE_TOKEN)
      Parse_Vector (Local_Vector);
      Compute_Translation_Transform(&Local_Trans, Local_Vector);
      Transform_Density (IMedia->Density, &Local_Trans);
    END_CASE

    CASE (ROTATE_TOKEN)
      Parse_Vector (Local_Vector);
      Compute_Rotation_Transform(&Local_Trans, Local_Vector);
      Transform_Density (IMedia->Density, &Local_Trans);
    END_CASE

    CASE (SCALE_TOKEN)
      Parse_Scale_Vector (Local_Vector);
      Compute_Scaling_Transform(&Local_Trans, Local_Vector);
      Transform_Density (IMedia->Density, &Local_Trans);
    END_CASE

    CASE (MATRIX_TOKEN)
      Parse_Matrix(Local_Matrix);
      Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
      Transform_Density (IMedia->Density, &Local_Trans);
    END_CASE

    CASE (TRANSFORM_TOKEN)
      GET(TRANSFORM_ID_TOKEN)
      Transform_Density (IMedia->Density, &Local_Trans);
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  Parse_End();

  IMedia->Next_Media = Next_Media;

  *Media_Ptr = IMedia;
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Interior
*
* INPUT
*
* OUTPUT
*
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1997 : Creation.
*
******************************************************************************/

void Parse_Interior(INTERIOR **Interior_Ptr)
{
  INTERIOR *Interior;

  Parse_Begin();

  EXPECT
    CASE(INTERIOR_ID_TOKEN)
      Destroy_Interior(*Interior_Ptr);
      *Interior_Ptr = Copy_Interior((INTERIOR *)Token.Data);
      EXIT
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  if(*Interior_Ptr == NULL)
  {
     *Interior_Ptr = Create_Interior();
  }

  Interior = *Interior_Ptr;

  EXPECT
    CASE (IOR_TOKEN)
      Interior->IOR = Parse_Float();
    END_CASE

    CASE (CAUSTICS_TOKEN)
      Interior->Caustics = Parse_Float() * 45.0;
    END_CASE

    CASE (FADE_DISTANCE_TOKEN)
      Interior->Fade_Distance = Parse_Float();
    END_CASE

    CASE (FADE_POWER_TOKEN)
      Interior->Fade_Power = Parse_Float();
    END_CASE

    CASE (MEDIA_TOKEN)
      Parse_Media((IMEDIA **)(&Interior->IMedia));
    END_CASE

    CASE (REFRACTION_TOKEN)
      Interior->Old_Refract = Parse_Float();
      Warn_Interior("Refraction value unnecessary to turn on refraction.\nTo attenuate, the fade_power and fade_distance keywords ");
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  Parse_End();

  Init_Interior(Interior);
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Media_Density_Pattern
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Dez 1996 : Creation.
*
******************************************************************************/

void Parse_Media_Density_Pattern(PIGMENT **Density_Ptr)
{
  PIGMENT *New;

  EXPECT
    CASE (DENSITY_ID_TOKEN)
      New = Copy_Pigment ((PIGMENT *) Token.Data);
      EXIT
    END_CASE

    OTHERWISE
      New = Create_Pigment();
      UNGET
      EXIT
    END_CASE
  END_EXPECT
   
  Parse_Pattern((TPATTERN *)New,DENSITY_TYPE);

  New->Next = (TPATTERN *)(*Density_Ptr);
  *Density_Ptr = New;
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

FOG *Parse_Fog()
{
  VECTOR Vector;
  MATRIX Matrix;
  TRANSFORM Trans;
  FOG *Fog;

  Parse_Begin();

  EXPECT
    CASE(FOG_ID_TOKEN)
      Fog = Copy_Fog ((FOG *) Token.Data);
      EXIT
    END_CASE

    OTHERWISE
      UNGET
      Fog = Create_Fog();
      EXIT
    END_CASE
  END_EXPECT

  EXPECT
    CASE_COLOUR
      Parse_Colour(Fog->Colour);
    END_CASE

    CASE (DISTANCE_TOKEN)
      Fog->Distance = Parse_Float();
    END_CASE

    CASE_FLOAT
      Warn(1.5, "Should use distance keyword.");
      Fog->Distance = Parse_Float();
    END_CASE

    CASE (FOG_TYPE_TOKEN)
      Fog->Type = (int)Parse_Float();
      if ((Fog->Type < ORIG_FOG) || (Fog->Type > FOG_TYPES))
      {
        Warn(0.0, "Unknown fog type.");
      }
    END_CASE

    CASE (FOG_ALT_TOKEN)
      Fog->Alt = Parse_Float();
    END_CASE

    CASE (FOG_OFFSET_TOKEN)
      Fog->Offset = Parse_Float();
    END_CASE

    CASE (TURB_DEPTH_TOKEN)
      Fog->Turb_Depth = Parse_Float();
    END_CASE

    CASE (UP_TOKEN)
      Parse_Vector(Fog->Up);
    END_CASE

    CASE (TURBULENCE_TOKEN)
      if (Fog->Turb == NULL)
      {
        Fog->Turb=(TURB *)Create_Warp(CLASSIC_TURB_WARP);
      }
      Parse_Vector(Fog->Turb->Turbulence);
    END_CASE

    CASE (OCTAVES_TOKEN)
      if (Fog->Turb == NULL)
      {
        Fog->Turb=(TURB *)Create_Warp(CLASSIC_TURB_WARP);
      }
      Fog->Turb->Octaves = (int)Parse_Float();
      if(Fog->Turb->Octaves < 1)
        Fog->Turb->Octaves = 1;
      if(Fog->Turb->Octaves > 10)
         Fog->Turb->Octaves = 10;
    END_CASE

    CASE (OMEGA_TOKEN)
      if (Fog->Turb == NULL)
      {
        Fog->Turb=(TURB *)Create_Warp(CLASSIC_TURB_WARP);
      }
      Fog->Turb->Omega = Parse_Float();
    END_CASE

    CASE (LAMBDA_TOKEN)
      if (Fog->Turb == NULL)
      {
        Fog->Turb=(TURB *)Create_Warp(CLASSIC_TURB_WARP);
      }
      Fog->Turb->Lambda = Parse_Float();
    END_CASE

    CASE (ROTATE_TOKEN)
      Parse_Vector(Vector);
      Compute_Rotation_Transform(&Trans, Vector);
      MTransDirection(Fog->Up, Fog->Up, &Trans);
    END_CASE

    CASE (SCALE_TOKEN)
      Parse_Vector(Vector);
      Compute_Scaling_Transform(&Trans, Vector);
      MTransDirection(Fog->Up, Fog->Up, &Trans);
    END_CASE

    CASE (TRANSLATE_TOKEN)
      Parse_Vector(Vector);
      Warn(0.0, "A fog's up vector can't be translated.");
/*
      Compute_Translation_Transform(&Trans, Vector);
      MTransDirection(Fog->Up, Fog->Up, &Trans);
*/
    END_CASE

    CASE (MATRIX_TOKEN)
      Parse_Matrix(Matrix);
      Compute_Matrix_Transform(&Trans, Matrix);
      MTransDirection(Fog->Up, Fog->Up, &Trans);
    END_CASE

    CASE (TRANSFORM_TOKEN)
      GET(TRANSFORM_ID_TOKEN)
      MTransDirection(Fog->Up, Fog->Up, (TRANSFORM *)Token.Data);
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  Parse_End ();

  /* Make sure the up vector is normalized. */

  VNormalize(Fog->Up, Fog->Up);

  return(Fog);
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Rainbow
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jul 1994 : Creation.
*
*   Dec 1994 : Modified to work with multiple rainbows. [DB]
*
*   Apr 1995 : Added code for rainbow arcs. [DB]
*
******************************************************************************/

RAINBOW *Parse_Rainbow()
{
  int Angle1, Angle2;
  DBL dot;
  RAINBOW *Rainbow;

  Angle1 = Angle2 = FALSE;

  Parse_Begin();

  EXPECT
    CASE(RAINBOW_ID_TOKEN)
      Rainbow = Copy_Rainbow ((RAINBOW *) Token.Data);
      EXIT
    END_CASE

    OTHERWISE
      UNGET
      Rainbow = Create_Rainbow();
      EXIT
    END_CASE
  END_EXPECT

  EXPECT
    CASE (ANGLE_TOKEN)
      Rainbow->Angle = Parse_Float();
    END_CASE

    CASE (DIRECTION_TOKEN)
      Parse_Vector(Rainbow->Antisolar_Vector);
    END_CASE

    CASE (COLOUR_MAP_TOKEN)
      Rainbow->Pigment = Create_Pigment();
      Rainbow->Pigment->Blend_Map = Parse_Colour_Map();
      Rainbow->Pigment->Type = GRADIENT_PATTERN;
      Make_Vector (Rainbow->Pigment->Vals.Gradient,1.0,0.0,0.0);
    END_CASE

    CASE (DISTANCE_TOKEN)
      Rainbow->Distance = Parse_Float();
    END_CASE

    CASE (JITTER_TOKEN)
      Rainbow->Jitter = Parse_Float();
    END_CASE

    CASE (WIDTH_TOKEN)
      Rainbow->Width = Parse_Float();
    END_CASE

    CASE (UP_TOKEN)
      Parse_Vector(Rainbow->Up_Vector);
    END_CASE

    CASE (FALLOFF_ANGLE_TOKEN)
      Angle1 = TRUE;
      Rainbow->Falloff_Angle = Parse_Float();
      if ((Rainbow->Falloff_Angle < 0.0) || (Rainbow->Falloff_Angle > 360.0))
      {
        Error("Illegal falloff angle in rainbow (Use value from 0 to 360 degrees).\n");
      }
      Rainbow->Falloff_Angle *= M_PI_360;
    END_CASE

    CASE (ARC_ANGLE_TOKEN)
      Angle2 = TRUE;
      Rainbow->Arc_Angle = Parse_Float();
      if ((Rainbow->Arc_Angle < 0.0) || (Rainbow->Arc_Angle > 360.0))
      {
        Error("Illegal arc angle in rainbow (Use value from 0 to 360 degrees).\n");
      }
      Rainbow->Arc_Angle *= M_PI_360;
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  Parse_End();

  /* Setup falloff angle. */

  if (Angle2 && !Angle1)
  {
    Rainbow->Falloff_Angle = Rainbow->Arc_Angle;
  }

  /* Test if arc angle is greater or equal to falloff angle. */

  if (Rainbow->Arc_Angle < Rainbow->Falloff_Angle)
  {
    Error("Arc angle is smaller than falloff angle in rainbow.\n");
  }

  /* Get falloff region width.*/

  Rainbow->Falloff_Width = Rainbow->Arc_Angle - Rainbow->Falloff_Angle;

  /* Check for illegal vectors. */

  VDot(dot, Rainbow->Antisolar_Vector, Rainbow->Antisolar_Vector);

  if (fabs(dot) < EPSILON)
  {
    Error("Rainbow's direction vector is zero.\n");
  }

  VDot(dot, Rainbow->Up_Vector, Rainbow->Up_Vector);

  if (fabs(dot) < EPSILON)
  {
    Error("Rainbow's up vector is zero.\n");
  }

  VNormalizeEq(Rainbow->Antisolar_Vector);
  VNormalizeEq(Rainbow->Up_Vector);

  VDot(dot, Rainbow->Up_Vector, Rainbow->Antisolar_Vector);

  if (fabs(1.0 - fabs(dot)) < EPSILON)
  {
    Error("Rainbow's up and direction vector are co-linear.\n");
  }

  /* Make sure that up and antisolar vector are perpendicular. */

  VCross(Rainbow->Right_Vector, Rainbow->Up_Vector, Rainbow->Antisolar_Vector);

  VCross(Rainbow->Up_Vector, Rainbow->Antisolar_Vector, Rainbow->Right_Vector);

  VNormalizeEq(Rainbow->Up_Vector);
  VNormalizeEq(Rainbow->Right_Vector);

  /* Adjust rainbow angle and width. */

  Rainbow->Angle -= 0.5 * Rainbow->Width;

  Rainbow->Angle *= M_PI_180;
  Rainbow->Width *= M_PI_180;

  return(Rainbow);
}



/*****************************************************************************
*
* FUNCTION
*
*   Parse_Skysphere
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jul 1994 : Creation.
*
*   Dec 1994 : Modified to work with multiple skyspheres. [DB]
*
******************************************************************************/

SKYSPHERE *Parse_Skysphere()
{
  VECTOR Local_Vector;
  MATRIX Local_Matrix;
  TRANSFORM Local_Trans;
  SKYSPHERE *Skysphere;

  Parse_Begin();

  EXPECT
    CASE(SKYSPHERE_ID_TOKEN)
      Skysphere = Copy_Skysphere((SKYSPHERE *)Token.Data);
      EXIT
    END_CASE

    OTHERWISE
      UNGET
      Skysphere = Create_Skysphere();
      EXIT
    END_CASE
  END_EXPECT

  EXPECT
    CASE (PIGMENT_TOKEN)
      Skysphere->Count++;
      Skysphere->Pigments = (PIGMENT **)POV_REALLOC(Skysphere->Pigments, Skysphere->Count*sizeof(SKYSPHERE *), "sky-sphere pigment");
      Skysphere->Pigments[Skysphere->Count-1] = Create_Pigment();
      Parse_Begin();
      Parse_Pigment(&(Skysphere->Pigments[Skysphere->Count-1]));
      Parse_End();
    END_CASE

    CASE (TRANSLATE_TOKEN)
      Parse_Vector (Local_Vector);
      Translate_Skysphere(Skysphere, Local_Vector);
    END_CASE

    CASE (ROTATE_TOKEN)
      Parse_Vector (Local_Vector);
      Rotate_Skysphere(Skysphere, Local_Vector);
    END_CASE

    CASE (SCALE_TOKEN)
      Parse_Scale_Vector (Local_Vector);
      Scale_Skysphere(Skysphere, Local_Vector);
    END_CASE

    CASE (MATRIX_TOKEN)
      Parse_Matrix(Local_Matrix);
      Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
      Transform_Skysphere(Skysphere, &Local_Trans);
    END_CASE

    CASE (TRANSFORM_TOKEN)
      GET(TRANSFORM_ID_TOKEN)
      Transform_Skysphere(Skysphere, (TRANSFORM *)Token.Data);
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  Parse_End();
  
  if (Skysphere->Count==0)
  {
    Error("Empty sky_sphere statement.");
  }

  return(Skysphere);
}

/*****************************************************************************
*
* FUNCTION      : Check_BH_Parameters
*
* ARGUMENTS     : bh - pointer to Black_Hole
*
* AUTHOR        : CJC [7/95]
*
* DESCRIPTION   : Applies sanity checks to the parameters of a black hole.
*
* CHANGES
*
******************************************************************************/

static void Check_BH_Parameters (BLACK_HOLE *bh)
{
  if (bh->Repeat == FALSE) return ;

  if (bh->Repeat_Vector [X] > 0.0)
  {
    if (bh->Center [X] < bh->Radius)
      bh->Center [X] = bh->Radius ;
    if (bh->Repeat_Vector [X] < bh->Center [X] + bh->Radius + bh->Uncertainty_Vector [X])
    {
      bh->Repeat_Vector [X] = bh->Center [X] + bh->Radius + bh->Uncertainty_Vector [X] ;
      Warning (0.0, "Black Hole repeat vector X too small ; increased to %g\n", bh->Repeat_Vector [X]) ;
    } 
    if (bh->Repeat_Vector [X] < Small_Tolerance)
    {
      Warning (0.0,"Black Hole repeat vector X is less than %f ; ignored\n", (float) Small_Tolerance) ;
      bh->Repeat_Vector [X] = 0.0 ;
    }
  }

  if (bh->Repeat_Vector [Y] > 0.0)
  {
    if (bh->Center [Y] < bh->Radius)
      bh->Center [Y] = bh->Radius ;
    if (bh->Repeat_Vector [Y] < bh->Center [Y] + bh->Radius + bh->Uncertainty_Vector [Y])
    {
      bh->Repeat_Vector [Y] = bh->Center [Y] + bh->Radius + bh->Uncertainty_Vector [Y] ;
      Warning (0.0, "Black Hole repeat vector Y too small ; increased to %g\n", bh->Repeat_Vector [Y]) ;
    } 
    if (bh->Repeat_Vector [Y] < Small_Tolerance)
    {
      Warning (0.0, "Black Hole repeat vector Y is less than %f ; ignored\n", (float) Small_Tolerance) ;
      bh->Repeat_Vector [Y] = 0.0 ;
    } 
  }

  if (bh->Repeat_Vector [Z] > 0.0)
  {
    if (bh->Center [Z] < bh->Radius)
      bh->Center [Z] = bh->Radius ;
    if (bh->Repeat_Vector [Z] < bh->Center [Z] + bh->Radius + bh->Uncertainty_Vector [Z])
    {
      bh->Repeat_Vector [Z] = bh->Center [Z] + bh->Radius + bh->Uncertainty_Vector [Z] ;
      Warning (0.0, "Black Hole repeat vector Z too small ; increased to %g\n", bh->Repeat_Vector [Z]) ;
    } 
    if (bh->Repeat_Vector [Z] < Small_Tolerance)
    {
      Warning (0.0, "Black Hole repeat vector Z is less than %f ; ignored\n", (float) Small_Tolerance) ;
      bh->Repeat_Vector [Z] = 0.0 ;
    }
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   Check_Turb
*
* INPUT
*
*   Warps_Ptr : Address where the root warp of a warp list
*   is stored.
*   
* OUTPUT
*
*   Warps_Ptr : If *Warps_Ptr is NULL, a classic turb warp
*   is created and a pointer to it is stored
*   
* RETURNS
*
*   A pointer to the last warp in the chain which is guarenteed
*   to be a classic turb.
*   
* AUTHOR
*
*   CEY [2/95]
*   
* DESCRIPTION   : This routine is called when a classic outside-the-warp
*  turbulence parameter is parsed.  One and only one classic turb may exist 
*  in a warp chain.  If there is one, it must be the last.  This routine 
*  traverses the warp chain and looks at the last link.  If it is not a 
*  classic turb then it adds one to the end and returns a pointer to it.
*  If the chain is empty, it creates a single link chain consisting of a 
*  classic turb link.  Future warp links get added ahead of the chain so
*  that any classic turb link is always last.
*
* CHANGES
*
******************************************************************************/

static TURB *Check_Turb (WARP **Warps_Ptr)
{
  WARP *Temp=*Warps_Ptr;
  
  if (Temp == NULL)
  {
    *Warps_Ptr = Temp = Create_Warp(CLASSIC_TURB_WARP);
  }
  else
  {
    while (Temp->Next_Warp != NULL)
    {
      Temp=Temp->Next_Warp;
    }
  
    if (Temp->Warp_Type != CLASSIC_TURB_WARP)
    {
      Temp->Next_Warp=Create_Warp(CLASSIC_TURB_WARP);
      Temp=Temp->Next_Warp;
    }
  }
  return((TURB *)Temp);
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

static void Parse_Warp (WARP **Warp_Ptr)
{
  WARP *New = NULL;
  TURB *Turb;
  REPEAT *Repeat;
  BLACK_HOLE *Black_Hole;
  VECTOR Local_Vector;

  Parse_Begin();

  EXPECT
    CASE(TURBULENCE_TOKEN)
      New=Create_Warp(EXTRA_TURB_WARP);
      Turb=(TURB *)New;
      Parse_Vector(Turb->Turbulence);
      EXPECT
        CASE(OCTAVES_TOKEN)
          Turb->Octaves = (int)Parse_Float();
          if(Turb->Octaves < 1)
             Turb->Octaves = 1;
          if(Turb->Octaves > 10)  /* Avoid DOMAIN errors */
             Turb->Octaves = 10;
        END_CASE

        CASE (OMEGA_TOKEN)
          Turb->Omega = Parse_Float();
        END_CASE

        CASE (LAMBDA_TOKEN)
          Turb->Lambda = Parse_Float();
        END_CASE
        
        OTHERWISE
          UNGET
          EXIT
        END_CASE
      END_EXPECT
      EXIT
    END_CASE

    CASE(REPEAT_TOKEN)
      New=Create_Warp(REPEAT_WARP);
      Repeat=(REPEAT *)New;
      Parse_Vector(Local_Vector);
      Repeat->Axis=-1;
      if (Local_Vector[X]!=0.0) 
        Repeat->Axis=X;
      if (Local_Vector[Y]!=0.0)
      { /* tw */
        if (Repeat->Axis < X)
          Repeat->Axis=Y;
        else 
          Error("Can only repeat along 1 axis.");
      } /* tw */
      if (Local_Vector[Z]!=0.0)
      { /* tw */
        if (Repeat->Axis < X)
          Repeat->Axis=Z;
        else
          Error("Can only repeat along 1 axis.");
      } /* tw */
      if (Repeat->Axis < X)
        Error("No axis specified in repeat.");
      Repeat->Width=Local_Vector[Repeat->Axis];
         
      EXPECT
        CASE(OFFSET_TOKEN)
          Parse_Vector(Repeat->Offset);
        END_CASE
        
        CASE(FLIP_TOKEN)
          Parse_Vector(Repeat->Flip);
          if (Repeat->Flip[X]!=0.0) Repeat->Flip[X]=-1.0; else Repeat->Flip[X]=1.0;
          if (Repeat->Flip[Y]!=0.0) Repeat->Flip[Y]=-1.0; else Repeat->Flip[Y]=1.0;
          if (Repeat->Flip[Z]!=0.0) Repeat->Flip[Z]=-1.0; else Repeat->Flip[Z]=1.0;
        END_CASE
        
        OTHERWISE
          UNGET
          EXIT
        END_CASE
      END_EXPECT
      EXIT
    END_CASE
    
    CASE(BLACK_HOLE_TOKEN)
      New = Create_Warp(BLACK_HOLE_WARP) ;
      Black_Hole = (BLACK_HOLE *) New ;
      Parse_Vector (Local_Vector) ;
      Assign_Vector (Black_Hole->Center, Local_Vector) ;
      Parse_Comma () ;
      Black_Hole->Radius = Parse_Float () ;
      Black_Hole->Radius_Squared = Black_Hole->Radius * Black_Hole->Radius ;
      Black_Hole->Inverse_Radius = 1.0 / Black_Hole->Radius;
      Black_Hole->Strength = 1.0 ;
      Black_Hole->Power = 2.0 ;
      Black_Hole->Inverted = FALSE ;
      Black_Hole->Type = 0 ;

      EXPECT
        CASE(STRENGTH_TOKEN)
          Black_Hole->Strength = Parse_Float () ;
        END_CASE

        CASE(FALLOFF_TOKEN)
          Black_Hole->Power = Parse_Float () ;
        END_CASE

        CASE(INVERSE_TOKEN)
          Black_Hole->Inverted = TRUE ;
        END_CASE

        CASE(TYPE_TOKEN)
          Black_Hole->Type = (int) Parse_Float () ;
        END_CASE

        CASE(REPEAT_TOKEN)
          Parse_Vector (Black_Hole->Repeat_Vector) ;
          Black_Hole->Repeat = TRUE ;
          Check_BH_Parameters (Black_Hole) ;
        END_CASE

        CASE(TURBULENCE_TOKEN)
          Parse_Vector (Black_Hole->Uncertainty_Vector) ;
          Black_Hole->Uncertain = TRUE ;
          Check_BH_Parameters (Black_Hole) ;
        END_CASE

        OTHERWISE
          UNGET
          EXIT
        END_CASE
      END_EXPECT
      EXIT
    END_CASE
      
    OTHERWISE
      Parse_Error_Str ("warp type");
    END_CASE
  END_EXPECT

  if (New==NULL)
  {
     Error("Empty warp statement.");
  }
  
  New->Next_Warp=*Warp_Ptr;
  *Warp_Ptr=New;

  Parse_End();
}


static void Warn_Interior(char *s)
{
  Warn(0.0,s);
  Warning(0.0,"should be specified in 'interior{...}' statement.\n");
  Warn_Compat(0);
}

void Parse_Material(MATERIAL *Material)
{
  MATERIAL *Temp;
  TEXTURE *Texture;
  VECTOR Local_Vector;
  MATRIX Local_Matrix;
  TRANSFORM Local_Trans;
  
  Parse_Begin();

  EXPECT
    CASE(MATERIAL_ID_TOKEN)
      Temp = (MATERIAL *)Token.Data;
      Texture = Copy_Textures(Temp->Texture);
      Link_Textures(&(Material->Texture),Texture);
      Destroy_Interior(Material->Interior);
      Material->Interior = Copy_Interior(Temp->Interior);
      EXIT
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  EXPECT
    CASE (TEXTURE_TOKEN)
      Parse_Begin ();
      Texture = Parse_Texture ();
      Parse_End ();
      Link_Textures(&(Material->Texture),Texture);
    END_CASE

    CASE (INTERIOR_TOKEN)
      Parse_Interior((INTERIOR **)(&(Material->Interior)));
    END_CASE

    CASE (TRANSLATE_TOKEN)
      Parse_Vector (Local_Vector);
      Compute_Translation_Transform(&Local_Trans, Local_Vector);
      Transform_Textures (Material->Texture, &Local_Trans);
      Transform_Interior (Material->Interior,&Local_Trans);
    END_CASE

    CASE (ROTATE_TOKEN)
      Parse_Vector (Local_Vector);
      Compute_Rotation_Transform(&Local_Trans, Local_Vector);
      Transform_Textures (Material->Texture, &Local_Trans);
      Transform_Interior (Material->Interior,&Local_Trans);
    END_CASE

    CASE (SCALE_TOKEN)
      Parse_Scale_Vector (Local_Vector);
      Compute_Scaling_Transform(&Local_Trans, Local_Vector);
      Transform_Textures (Material->Texture, &Local_Trans);
      Transform_Interior (Material->Interior,&Local_Trans);
    END_CASE

    CASE (MATRIX_TOKEN)
      Parse_Matrix(Local_Matrix);
      Compute_Matrix_Transform(&Local_Trans, Local_Matrix);
      Transform_Textures (Material->Texture, &Local_Trans);
      Transform_Interior (Material->Interior,&Local_Trans);
    END_CASE

    CASE (TRANSFORM_TOKEN)
      GET(TRANSFORM_ID_TOKEN)
      Transform_Textures (Material->Texture, (TRANSFORM *)Token.Data);
      Transform_Interior (Material->Interior,(TRANSFORM *)Token.Data);
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  Parse_End();
}
