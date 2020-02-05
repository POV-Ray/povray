/****************************************************************************
*                ppm.c
*
*  This module contains the code to read and write the PPM file format.
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
*    P3              - ASCII data OR
*    P6              - raw binary data
*    # hello         - optional comment(s)
*    wwww hhhh       - Width, Height (ASCII text)
*    # world         - optional comment(s)
*    nnn             - maximum color (nnn = bright, 0 = black)
*
*  (each pixel: one of the following)
*    rr gg bb        - Red, green, blue of intensity 0-nnn (binary byte)
*    RRR GGG BBB     - Red, green, blue of intensity 0-nnn (ASCII number)
*
*****************************************************************************/

#include "frame.h"
#include "povproto.h"
#include "povray.h"
#include "optout.h"
#include "pgm.h"
#include "ppm.h"

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/*****************************************************************************
* Local typedefs
******************************************************************************/

/*****************************************************************************
* Local variables
******************************************************************************/

static int PPM_Line_Number;

/*****************************************************************************
* Static functions
******************************************************************************/

static int Open_PPM_File (FILE_HANDLE *handle, char *name, int *width, int *height, int buffer_size, int mode);
static void Write_PPM_Line (FILE_HANDLE *handle, COLOUR *line_data, int line_number);
static int Read_PPM_Line (FILE_HANDLE *handle, COLOUR *line_data, int *line_number);
static void Close_PPM_File (FILE_HANDLE *handle);

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

FILE_HANDLE *Get_PPM_File_Handle()
{
  FILE_HANDLE *handle;

  handle = (FILE_HANDLE *) POV_MALLOC(sizeof(FILE_HANDLE), "PPM file handle") ;

  handle->Open_File_p = Open_PPM_File;
  handle->Write_Line_p = Write_PPM_Line;
  handle->Read_Line_p = Read_PPM_Line;
  handle->Read_Image_p = Read_PPM_Image;
  handle->Close_File_p = Close_PPM_File;

  handle->file = NULL;
  handle->buffer = NULL;
  handle->buffer_size = 0;

  return (handle);
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

static int Open_PPM_File(FILE_HANDLE *handle, char *name, int *width, int *height, int buffer_size, int mode)
{
  char type;
  int input;
  char junk[512];

  handle->mode = mode;
  handle->filename = name;
  PPM_Line_Number = 0;

  switch (mode)
  {
    case READ_MODE:

      /* We can't resume from stdout. */
      if (opts.Options & TO_STDOUT  || 
          (handle->file = fopen(name, READ_BINFILE_STRING)) == NULL)
      {
        Status_Info("\n");
        return(0);
      }

      if (buffer_size != 0)
      {
        handle->buffer = (char *)POV_MALLOC((size_t)buffer_size, "PPM file buffer") ;
        setvbuf(handle->file, handle->buffer, _IOFBF, buffer_size);
      }

      if (fscanf(handle->file, "P%c\n", &type) != 1 || type != '6')
      {
        return(0);
      }

      /* Ignore any comments (if they are written) */

      while ((input = fgetc(handle->file)) == '#')
      {
        fgets(junk, 512, handle->file);
      }

      ungetc(input, handle->file);

      if (fscanf(handle->file, "%d %d\n255\n", width, height) != 2)
      {
        return(0);
      }

      Status_Info("\nResuming interrupted trace from %s",handle->filename);

      handle->width = *width;
      handle->height = *height;
      handle->buffer_size = buffer_size;

      break;

    case WRITE_MODE:

      if (opts.Options & TO_STDOUT)
      {
        buffer_size = 0;
        handle->file = stdout;
      }
      else
      {
        if ((handle->file = fopen(name, WRITE_BINFILE_STRING)) == NULL)
        {
          return(0);
        }
      }

      if (buffer_size != 0)
      {
        handle->buffer = (char *)POV_MALLOC((size_t)buffer_size, "PPM file buffer") ;
        setvbuf(handle->file, handle->buffer, _IOFBF, buffer_size);
      }

      fprintf(handle->file, "P6\n");

#ifdef POV_COMMENTS
#ifdef TRACER
      fprintf(handle->file, "# Author: %s\n", TRACER);
#endif

      fprintf(handle->file, "# Source: Persistence of Vision(tm) Ray Tracer v%s%s\n",
              POV_RAY_VERSION, COMPILER_VER);

      if (!(opts.Options & TO_STDOUT))
      {
        fprintf(handle->file, "# Input File: %s\n", opts.Input_File_Name);
      }

      if (opts.FrameSeq.Clock_Value != 0)
      {
        fprintf(handle->file, "# POV Clock: %g\n", opts.FrameSeq.Clock_Value);
      }

      if (opts.Quality != 9)
      {
        fprintf(handle->file, "# Rendering Quality: %d\n", opts.Quality);
      }
#endif /* POV_COMMENTS */

      fprintf(handle->file, "%d %d\n255\n", *width, *height);

      handle->width = *width;
      handle->height = *height;

      handle->buffer_size = buffer_size;

      break;

    case APPEND_MODE:

      if (opts.Options & TO_STDOUT)
      {
        buffer_size = 0;
        handle->file = stdout;
      }
      else
      {
        if ((handle->file = fopen(name, APPEND_BINFILE_STRING)) == NULL)
        {
          return(0);
        }
      }

      if (buffer_size != 0)
      {
        handle->buffer = (char *)POV_MALLOC((size_t)buffer_size, "PPM file buffer") ;
        setvbuf(handle->file, handle->buffer, _IOFBF, buffer_size);
      }

      handle->buffer_size = buffer_size;

      break;
  }

  return(1);
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

static void Write_PPM_Line(FILE_HANDLE *handle, COLOUR *line_data, int line_number)
{
  unsigned int gray;
  register int x;

  for (x = 0 ; x < handle->width ; x++)
  {
    if (opts.Options & HF_GRAY_16)  /* 16 bit grayscale output */
    {
      gray = ((0.30 * line_data[x][RED]) +
              (0.59 * line_data[x][GREEN]) +
              (0.11 * line_data[x][BLUE])) * 65535;

      if ((putc((gray >> 8) & 0xFF, handle->file) == EOF) ||
          (putc(gray & 0xFF, handle->file) == EOF) ||
          (putc(0, handle->file) == EOF))
      {
        Error("Error writing PPM output data to %s.\n",handle->filename);
      }
    }
    else                            /* Normal 24 bit pixel coloring */
    {
      if ((putc((int)floor(line_data[x][RED] * 255.0), handle->file) == EOF) ||
          (putc((int)floor(line_data[x][GREEN]*255.0), handle->file) == EOF) ||
          (putc((int)floor(line_data[x][BLUE]*255.0), handle->file) == EOF))
      {
        Error("Error writing PPM output data to %s.\n",handle->filename);
      }
    }
  }

  PPM_Line_Number++;

  if (handle->buffer_size == 0)
  {
    /* close and reopen file for integrity in case we crash */

    fflush(handle->file);

    if (!(opts.Options & TO_STDOUT))
    {
      handle->file = freopen(handle->filename,APPEND_BINFILE_STRING,handle->file);
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
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static int Read_PPM_Line(FILE_HANDLE *handle, COLOUR *line_data, int *line_number)
{
  int data, i;

  if ((data = getc(handle->file)) == EOF)
  {
    return (0);
  }

  ungetc(data, handle->file);

  *line_number = PPM_Line_Number++;

  for (i = 0 ; i < handle->width ; i++)
  {
    if ((data = getc(handle->file)) == EOF)
    {
      return(-1);
    }

    line_data[i][RED] = (DBL) data / 255.0;

    if ((data = getc(handle->file)) == EOF)
    {
      return(-1);
    }

    line_data[i][GREEN] = (DBL) data / 255.0;

    if ((data = getc(handle->file)) == EOF)
    {
      return(-1);
    }

    line_data[i][BLUE] = (DBL) data / 255.0;
  }

  return (1);
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

static void Close_PPM_File(FILE_HANDLE *handle)
{
  if (handle->file)
  {
    fflush(handle->file);

    /* Close and reopen file (if not stdout) for integrity in case we crash */

    if (!(opts.Options & TO_STDOUT))
      fclose(handle->file);
  }

  if (handle->buffer != NULL)
  {
    POV_FREE(handle->buffer);
  }

  handle->file = NULL;
  handle->buffer = NULL;
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

void Read_PPM_Image(IMAGE *Image, char *name)
{
  char type;
  int width, height;
  int depth;
  char input;
  char junk[512];
  int x, y;
  int data;
  IMAGE_LINE *line_data;
  FILE *infile;

  if ((infile = Locate_File(name, READ_BINFILE_STRING, ".ppm", ".PPM",NULL,TRUE)) == NULL)
  {
    Error("Error opening PPM image %s.\n", name);
  }

  if (fscanf(infile, "P%c\n", &type) != 1 || (type != '3' && type != '6'))
  {
    Error ("File is not in PPM format.\n", name);
  }

  /* Ignore any comments */

  while ((input = fgetc(infile)) == '#')
  {
    fgets(junk, 512, infile);
  }

  ungetc(input, infile);

  if (fscanf(infile, "%d %d\n", &width, &height) != 2)
  {
    Error ("Error reading width or height from PPM image.\n", name);
  }

  /* Ignore any comments */
  while ((input = fgetc(infile)) == '#')
  {
    fgets(junk, 512, infile);
  }

  ungetc(input, infile);

  if (fscanf(infile, "%d\n", &depth) != 1 || depth > 255 || depth < 1)
  {
    Error ("Unsupported number of colors (%d) in PPM image.\n", depth);
  }

  Image->width  = (DBL)(Image->iwidth = width);
  Image->height = (DBL)(Image->iheight = height);

  Image->Colour_Map_Size = 0;

  Image->Colour_Map = NULL;

  Image->data.rgb_lines = (IMAGE_LINE *) POV_MALLOC(Image->iheight * sizeof (IMAGE_LINE), "PPM image");

  for (y = 0; y < height; y++)
  {
    line_data = &Image->data.rgb_lines[y];

    line_data->red   = (unsigned char *)POV_MALLOC(width,"PPM image line");
    line_data->green = (unsigned char *)POV_MALLOC(width,"PPM image line");
    line_data->blue  = (unsigned char *)POV_MALLOC(width,"PPM image line");
    line_data->transm = (unsigned char *)NULL;

    if (type == '3') /* ASCII data to be input */
    {
      for (x = 0; x < width; x++)
      {
        if (fscanf(infile,"%d",&data) != 1)
        {
          Error("Error reading data from PPM image.\n");
        }

        line_data->red[x] = data*255/depth;

        if (fscanf(infile,"%d",&data) != 1)
        {
          Error("Error reading data from PPM image.\n");
        }

        line_data->green[x] = data*255/depth;

        if (fscanf(infile,"%d",&data) != 1)
        {
          Error("Error reading data from PPM image.\n");
        }

        line_data->blue[x] = data*255/depth;
      }
    }
    else /* (type == '6') Raw binary data to be input */
    {
      for (x = 0; x < width; x++)
      {
        if ((data = getc(infile)) == EOF)
        {
          Error("Error reading data from PPM image.\n");
        }

        line_data->red[x] = data*255/depth;

        if ((data = getc(infile)) == EOF)
        {
          Error("Error reading data from PPM image.\n");
        }

        line_data->green[x] = data*255/depth;

        if ((data = getc(infile)) == EOF)
        {
          Error("Error reading data from PPM image.\n");
        }

        line_data->blue[x] = data*255/depth;
      }
    }
  }

  fclose(infile);
}
