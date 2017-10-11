/****************************************************************************
*                   gif.c
*
*  Gif-format file reader.
*
*  NOTE:  Portions of this module were written by Steve Bennett and are used
*         here with his permission.
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
* Modification by Hans-Deltev Fink, January 1999, used with permission
*
*****************************************************************************/

/*
 * The following routines were borrowed freely from FRACTINT, and represent
 * a generalized GIF file decoder.  This once seemed the best, most universal
 * format for reading in Bitmapped images, until Unisys began enforcing
 * its patent on the LZ compression that GIF uses.  POV-Ray, as freeware, is
 * exempt from GIF licensing fees.  GIF is a Copyright of Compuserve, Inc.
 *
 * Swiped and converted to entirely "C" coded routines by AAC for the most
 * in future portability!
 */

#include "frame.h"
#include "povproto.h"
#include "gif.h"
#include "gifdecod.h"
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

static IMAGE *Current_Image;
static int Bitmap_Line;
static FILE *Bit_File;
unsigned char *decoderline  /*  [2049] */ ;  /* write-line routines use this */

static IMAGE_COLOUR *gif_colour_map;
static int colourmap_size;



/*****************************************************************************
* Static functions
******************************************************************************/



/*****************************************************************************
*
* FUNCTION
*
*   out_line
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

int out_line (unsigned char *pixels, int linelen)
{
  register int x;
  register unsigned char *line;

  if (Bitmap_Line == Current_Image->iheight)
  {
    Warning (0.0, "Extra data at end of GIF image.\n");
    return (0) ;
  }

  line = Current_Image->data.map_lines[Bitmap_Line++];

  for (x = 0; x < linelen; x++)
  {
    if ((int)(*pixels) > Current_Image->Colour_Map_Size)
    {
      Error ("Error - GIF image map color out of range.\n");
    }

    line[x] = *pixels;

    pixels++;
  }

  return (0);
}




/*****************************************************************************
*
* FUNCTION
*
*   gif_get_byte
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
*   Get byte from file, return the next byte or an error.
*
* CHANGES
*
*   -
*
******************************************************************************/

int gif_get_byte()
{
  register int byte;

  if ((byte = getc(Bit_File)) != EOF)
  {
    return (byte);
  }
  else
  {
    Error ("Error reading data from GIF image.\n");
  }

  /* Keep the compiler happy. */

  return(0);
}



/*****************************************************************************
*
* FUNCTION
*
*   Read_Gif_Image
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
*   Main GIF file decoder.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Read_Gif_Image(IMAGE *Image, char *filename)
{
  register int i, j, status;
  unsigned finished, planes;
  unsigned char buffer[16];

  status = 0;

  Current_Image = Image;

  if ((Bit_File = Locate_File(filename, READ_BINFILE_STRING, ".gif", ".GIF",NULL,TRUE)) == NULL)
  {
    Error ("Error opening GIF image.\n");
  }

  /* Get the screen description. */

  for (i = 0; i < 13; i++)
  {
    buffer[i] = (unsigned char)gif_get_byte();
  }

  /* Use updated GIF specs. */

  if (strncmp((char *) buffer,"GIF",3) == 0)  /* Allow only GIF87 and GIF89 */
  {
    if ((buffer[3] != '8') || ((buffer[4] != '7') && (buffer[4]) != '9') ||
        (buffer[5] < 'A') || (buffer[5]) > 'z')
    {
      Error ("Unsupported GIF version %c%c%c.\n",
             buffer[3], buffer[4], buffer[5]);
    }
  }
  else
  {
    Error ("File is not in GIF format.\n");
  }

  planes = ((unsigned)buffer[10] & 0x0F) + 1;

  colourmap_size = (int)(1 << planes);

  gif_colour_map = (IMAGE_COLOUR *)POV_CALLOC((size_t)colourmap_size, sizeof(IMAGE_COLOUR), "GIF color map");

  /* Color map (better be!) */

  if ((buffer[10] & 0x80) == 0)
  {
    Error ("Error in GIF color map.\n");
  }

  for (i = 0; i < colourmap_size ; i++)
  {
    gif_colour_map[i].Red    = (unsigned char)gif_get_byte();
    gif_colour_map[i].Green  = (unsigned char)gif_get_byte();
    gif_colour_map[i].Blue   = (unsigned char)gif_get_byte();
    gif_colour_map[i].Filter = 0;
    gif_colour_map[i].Transmit = 0;
  }

  /* Now display one or more GIF objects. */

  finished = FALSE;

  while (!finished)
  {
    switch (gif_get_byte())
    {
      /* End of the GIF dataset. */

      case ';':

        finished = TRUE;
        status = 0;

        break;

      /* GIF Extension Block. */

      case '!':

        /* Read (and ignore) the ID. */

        gif_get_byte();

        /* Get data len. */

        while ((i = gif_get_byte()) > 0)
        {
          for (j = 0; j < i; j++)
          {
            /* Flush data. */

            gif_get_byte();
          }
        }

        break;

      /* Start of image object. Get description. */

      case ',':

        for (i = 0; i < 9; i++)
        {
          /* EOF test (?).*/

          if ((j = gif_get_byte()) < 0)
          {
            status = -1;

            break;
          }

          buffer[i] = (unsigned char) j;
        }

        /* Check "interlaced" bit. */

        if (j & 0x40)
        {
          Error ("Interlacing in GIF image unsupported.\n");
        }

        if (status < 0)
        {
          finished = TRUE;

          break;
        }

        Image->iwidth  = buffer[4] | (buffer[5] << 8);
        Image->iheight = buffer[6] | (buffer[7] << 8);

        Image->width = (DBL) Image->iwidth;
        Image->height = (DBL) Image->iheight;

        Bitmap_Line = 0;

        Image->Colour_Map_Size = colourmap_size;
        Image->Colour_Map = gif_colour_map;

        Image->data.map_lines = (unsigned char **)POV_MALLOC(Image->iheight * sizeof(unsigned char *), "GIF image");

        for (i = 0 ; i < Image->iheight ; i++)
        {
          Image->data.map_lines[i] = (unsigned char *)POV_CALLOC((size_t)Image->iwidth, sizeof(unsigned char), "GIF image line");
        }

        /* Setup the color palette for the image. */

        decoderline = (unsigned char *)POV_CALLOC(Image->iwidth, sizeof(unsigned char), "GIF decoder line");

        /* Put bytes in Buf. */

        status = decoder(Image->iwidth);

        POV_FREE (decoderline);

        decoderline = NULL;

        finished = TRUE;

        break;

      default:

        status = -1;

        finished = TRUE;

        break;
    }
  }

  if (Bit_File != NULL)		/* -hdf99- */
  {
    fclose(Bit_File);
  }

}

