/****************************************************************************
*                pgm.c
*
*  This module contains the code to read the PGM file format.
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
* Original patch copyright 1994 Tim Rowley
* Updated for POV 3.0 by Chris Cason, Jan '95.
*
*****************************************************************************/

/****************************************************************************
*  The format is as follows:
*
*  (header:)
*    P2              - ASCII data in file OR
*    P5              - binary data in file
*    # hello         - comment (optional, may be multiple)
*    wwww hhhh       - Width, Height (ASCII text)
*    # world         - comment (optional, may be multiple)
*    nnn             - maximum color (nnn = white, 0 = black)
*
*  (each pixel: one of the following)
*    xx               - Intensity 0-nnn (binary byte)
*    AAA              - Intensity 0-nnn (ASCII number)
*
*****************************************************************************/

#include "frame.h"
#include "povproto.h"
#include "povray.h"
#include "pgm.h"
#include "ppm.h"

/*****************************************************************************
*
* FUNCTION
*
*  Read_PGM_Image
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
*     Modified to support ASCII files, comments, and arbitrary bit depth [AED]
*
******************************************************************************/

void Read_PGM_Image(IMAGE *Image, char *name)
{
  char type;
  int width, height;
  int depth;
  char input;
  char junk[512];
  int x, y;
  int data;
  FILE *infile;

  if ((infile = Locate_File(name,READ_BINFILE_STRING,".pgm",".PGM",NULL,TRUE)) == NULL)
  {
    Error ("Cannot open PGM image %s.\n", name);
  }

  if (fscanf(infile,"P%c\n",&type) != 1 || (type != '2' && type != '5'))
  {
    Error ("File is not in PGM format.\n");
  }

  /* Ignore any comments */
  while ((input = fgetc(infile)) == '#')
  {
    fgets(junk,512,infile);
  }

  ungetc(input,infile);

  if (fscanf(infile,"%d %d\n",&width,&height) != 2)
  {
    Error ("Error reading width or height from PGM image.\n");
  }

  /* Ignore any comments */
  while ((input = fgetc(infile)) == '#')
  {
    fgets(junk,512,infile);
  }

  ungetc(input,infile);

  if (fscanf(infile,"%d\n",&depth) != 1 ||
      (depth > 255 && type == '5') ||
      depth > 65535 || depth < 1)
  {
    Error ("Unsupported number of colors (%d) in PGM image.\n", depth);
  }


  Image->width  = (DBL)(Image->iwidth  = width);
  Image->height = (DBL)(Image->iheight = height);

  if (depth < 256)
  {
    Image->Colour_Map_Size = depth;

    Image->Colour_Map = (IMAGE_COLOUR *)POV_MALLOC(depth*sizeof(IMAGE_COLOUR), "PGM color map");

    for (x = 0; x < depth; x++)
    {
      Image->Colour_Map[x].Red =
      Image->Colour_Map[x].Green =
      Image->Colour_Map[x].Blue = x*255/depth;
      Image->Colour_Map[x].Filter =0;
      Image->Colour_Map[x].Transmit =0;
    }

    Image->data.map_lines = (unsigned char **)POV_MALLOC(height*sizeof(unsigned char *), "PGM image");

    for (y = 0; y < height; y++)
    {
      Image->data.map_lines[y] = (unsigned char *)POV_MALLOC(width,"PGM image line");

      if (type == '2') /* ASCII data to be input */
      {
        for (x = 0; x < width; x++)
        {
          if (fscanf(infile,"%d",&data) != 1)
          {
            Error ("Error reading data from PGM image.\n");
          }
          Image->data.map_lines[y][x] = data;
        }
      }
      else /* (type == '5') Raw binary data to be input */
      {
        for (x = 0; x < width; x++)
        {
          if ((data = getc(infile)) == EOF)
          {
            Error ("Error reading data from PGM image.\n");
          }
          Image->data.map_lines[y][x] = data;
        }
      }
    }
  }
  else /* if(depth < 65536) the data MUST be in ASCII format */
  {
    IMAGE_LINE *line_data;

    Image->Colour_Map_Size = 0;
    Image->Colour_Map = (IMAGE_COLOUR *)NULL;

    Image->data.rgb_lines = (IMAGE_LINE *)POV_MALLOC(height * sizeof(IMAGE_LINE), "PGM image");
    for (y = 0; y < height; y++)
    {
      line_data = &Image->data.rgb_lines[y];

      line_data->red   = (unsigned char *)POV_MALLOC(width, "PGM image line");
      line_data->green = (unsigned char *)POV_MALLOC(width, "PGM image line");
      line_data->blue  = (unsigned char *)POV_MALLOC(width, "PGM image line");
      line_data->transm = (unsigned char *)NULL;

      for (x = 0; x < width; x++)
      {
        if (fscanf(infile,"%d",&data) != 1)
        {
          Error("Error reading data from PGM image.\n");
        }

        data = (int)((DBL)data*65535/depth);

        line_data->red[x] = (data >> 8) & 0xFF;
        line_data->green[x] = data & 0xFF;
        line_data->blue[x] = 0;
      }
    }
  }

  fclose(infile);
}
