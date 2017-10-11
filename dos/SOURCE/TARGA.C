/****************************************************************************
*                targa.c
*
*  This module contains the code to read and write the Targa output file
*  format.
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
* Modifications by Hans-Detlev Fink, January 1999, used with permission.
*
*****************************************************************************/

/****************************************************************************
*
*  Explanation:
*
*    -
*
*  ---
*
*  May 1994 : Support for 24-bit RLE Targa output files added: John Baily
*             and David Payne.
*
*  Jul 1994 : Resume trace support and minor algorithm fix (one more still
*             needed, see comments in Write_Targa_Line); resume will force
*             Targa format to match the original trace format -- the T or C
*             format flag is adjusted as necessary: Charles Marslett,
*
*  Jun 1995 : Added support for 32-bit Targa input and output files.
*             The alpha channel has a value of 0 for 100% transparency
*             and a value of 255 for 0% transparency. [DB]
*
*****************************************************************************/

#include "frame.h"
#include "povproto.h"
#include "povray.h"
#include "targa.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define boolean  int



/*****************************************************************************
* Local typedefs
******************************************************************************/

typedef struct pix
{
  DBL blue, green, red, transm;
} pix;



/*****************************************************************************
* Local variables
******************************************************************************/

static int Targa_Line_Number;
static unsigned char idbuf[256];



/*****************************************************************************
* Static functions
******************************************************************************/

static void convert_targa_color (IMAGE_COLOUR *, unsigned, unsigned char *);
static int Open_Targa_File (FILE_HANDLE *handle, char *name, int *width, int *height, int buffer_size, int mode);
static void Write_Targa_Line (FILE_HANDLE *handle, COLOUR *line_data, int line_number);
static int Read_Targa_Line (FILE_HANDLE *handle, COLOUR *line_data, int *line_number);
static void Close_Targa_File (FILE_HANDLE *handle);
static void Write_Targa_Pixel (FILE_HANDLE *handle, DBL b, DBL g, DBL r, DBL f);



/*****************************************************************************
*
* FUNCTION
*
*   Get_Targa_File_Handle
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

FILE_HANDLE *Get_Targa_File_Handle()
{
  FILE_HANDLE *handle;

  handle = (FILE_HANDLE *)POV_MALLOC(sizeof(FILE_HANDLE), "TGA file handle");

  handle->Open_File_p         = Open_Targa_File;
  handle->Write_Line_p        = Write_Targa_Line;
  handle->Read_Line_p         = Read_Targa_Line;
  handle->Read_Image_p        = Read_Targa_Image;
  handle->Close_File_p        = Close_Targa_File;

  handle->file = NULL;
  handle->buffer = NULL;
  handle->buffer_size = 0;

  return(handle);
}


/*****************************************************************************
*
* FUNCTION
*
*   Open_Targa_File
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
*   Jun 1995 : Added code for 32 bit Targa files. [DB]
*
*   Sept 1995: Modified header output for Targa files. [AED]
*
******************************************************************************/

static int Open_Targa_File (FILE_HANDLE *handle, char *name, int *width, int *height, int buffer_size, int mode)
{
  unsigned char tgaheader[18];

  handle->mode = mode;
  handle->filename = name;

  Targa_Line_Number = 0;
  
  switch (mode)
  {
    case READ_MODE:

      /* We can't resume from stdout. */
      if (opts.Options & TO_STDOUT ||
          (handle->file = fopen(name, READ_BINFILE_STRING)) == NULL)
      {
        Status_Info("\n");
        return(0);
      }

      /* Read targa header information. */

      if (fread(tgaheader, 18, 1, handle->file) != 1)
      {
        return(0);
      }

      /* Decipher the header information */

      switch (tgaheader[2])
      {
        case 2  : opts.OutputFormat = 'T'; break;
        case 10 : opts.OutputFormat = 'C'; break;
        default : return(0);
      }

      switch (tgaheader[16])
      {
        case 24 : break;
        case 32 : opts.Options |= OUTPUT_ALPHA; break;
        default : return(0);
      }

      /* First_Column set to x offset.  Bytes 8, 9 */
      opts.First_Column = tgaheader[8] + (tgaheader[9] << 8);

      /* First line set to y offset.  Bytes 10, 11 */
      opts.First_Line = Targa_Line_Number = tgaheader[10] + (tgaheader[11]<<8);

      handle->width  = *width  = tgaheader[12] + (tgaheader[13] << 8);
      handle->height = *height = tgaheader[14] + (tgaheader[15] << 8);

      handle->buffer_size = buffer_size;
  
      Status_Info("\nResuming interrupted trace from %s",handle->filename);

      break;

    case WRITE_MODE:

      if (opts.Options & TO_STDOUT)
      {
        buffer_size = 0;
        handle->file = stdout;
      }
      else if ((handle->file = fopen (name, WRITE_BINFILE_STRING)) == NULL)
      {
        return(0);
      }

      if (buffer_size != 0)
      {
        handle->buffer = (char *)POV_MALLOC((size_t)buffer_size, "TGA file buffer");
        setvbuf (handle->file, handle->buffer, _IOFBF, buffer_size);
      }

      /* Output TGA file header info */
      putc(0, handle->file);  /* Byte 0 - Length of Image ID field */

      putc(0, handle->file);  /* Byte 1 - Color map type (0 is no color map) */

      switch(opts.OutputFormat)  /* Byte 2 - TGA file type */
      {
        case 't': /* Uncompressed True-Color Image */
        case 'T': 
        case 's': 
        case 'S': putc(2, handle->file); break;

        case 'c': /* Run-length compressed True-Color Image */
        case 'C': putc(10, handle->file); break;
      }

      putc(0, handle->file);  /* Byte 3 - Index of first color map entry LSB */
      putc(0, handle->file);  /* Byte 4 - Index of first color map entry MSB */

      putc(0, handle->file);  /* Byte 5 - Color map length LSB */
      putc(0, handle->file);  /* Byte 6 - Color map legth MSB */

      putc(0, handle->file);  /* Byte 7 - Color map size */

      /* x origin set to "First_Column"  Bytes 8, 9 */

      putc(opts.First_Column % 256, handle->file);
      putc(opts.First_Column / 256, handle->file);

      /* y origin set to "First_Line"    Bytes 10, 11 */

      putc(opts.First_Line % 256, handle->file);
      putc(opts.First_Line / 256, handle->file);

      /* write width and height  Bytes 12 - 15 */

      putc(*width % 256, handle->file);
      putc(*width / 256, handle->file);
      putc(*height % 256, handle->file);
      putc(*height / 256, handle->file);

      /* We write 24 or 32 bits/pixel (16 million colors and alpha channel)
       * and also store the orientation and Alpha channel depth.  Bytes 16, 17.
       */
      if (opts.Options & OUTPUT_ALPHA)
      {
        putc(32, handle->file);    /* 32 bits/pixel (BGRA) */
        putc(0x28, handle->file);  /* Data starts at top left, 8 bits Alpha */
      }
      else
      {
        putc(24, handle->file);    /* 24 bits/pixel (BGR) */
        putc(0x20, handle->file);  /* Data starts at top left, 0 bits Alpha */
      }

      /* TGA file Image ID field data would go here (up to 255 chars) */

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
      else if ((handle->file = fopen (name, APPEND_BINFILE_STRING)) == NULL)
      {
        return(0);
      }
      else if (buffer_size != 0)
      {
        handle->buffer = (char *)POV_MALLOC((size_t)buffer_size, "TGA file buffer");
        setvbuf (handle->file, handle->buffer, _IOFBF, buffer_size);
      }

      break;
  }

  return(1);
}



/*****************************************************************************
*
* FUNCTION
*
*   Write_Targa_Pixel
*
* INPUT
*
*   handle     - Current file handel
*   r, g, b, f - Color values to write
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dan Farmer
*   
* DESCRIPTION   :
*
*   Moves redundant code to a single function.  Adding 16 bit grayscale
*   conditional code to each occurance of writing a pixel was getting a bit
*   too wordy.
*
* CHANGES
*
*   Jun 1995 : Added code for 32 bit Targa files. [DB]
*
*   Sept 1995: Modified handling of Alpha channel to use only opts.Options
*   Sept 1995: Modified handling of grayscale to use only opts.Options
*
******************************************************************************/

static void Write_Targa_Pixel (FILE_HANDLE *handle, DBL  b, DBL  g, DBL r, DBL  f)
{
  unsigned int gray;

  if (opts.Options & HF_GRAY_16)
  {
    /* Ouput heightfield in POV red/green format */
    gray = ((0.30 * r) + (0.59 * g) + (0.11 * b)) * 65535;

    if ((putc(0 , handle->file) == EOF) ||
        (putc(gray & 0xFF, handle->file) == EOF) ||
        (putc((gray >> 8) & 0xFF, handle->file) == EOF))
    {
      Error("Error writing TGA output data to %s.\n",handle->filename);
    }
  }
  else
  {
    /* Normal 24/32 bit pixel coloring */

    if ((putc((int) floor(b * 255.0), handle->file) == EOF) ||
        (putc((int) floor(g * 255.0), handle->file) == EOF) ||
        (putc((int) floor(r * 255.0), handle->file) == EOF))
    {
      Error("Error writing TGA output data to %s.\n",handle->filename);
    }

    if (opts.Options & OUTPUT_ALPHA)
    {
      if (putc((int) floor((1.0 - f) * 255.0), handle->file) == EOF)
      {
        Error("Error writing TGA output data to %s.\n",handle->filename);
      }
    }
  }
}


/*****************************************************************************
*
* FUNCTION
*
*   Write_Targa_Line
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
*   Jun 1995 : Added code for 32 bit Targa files. [DB]
*
******************************************************************************/

static void Write_Targa_Line (FILE_HANDLE *handle, COLOUR *line_data, int line_number)
{
  register int x;
  int ptype, cnt, llen, startx;
  boolean writenow;
  pix curpix, nexpix;

  switch (opts.OutputFormat)
  {
    case 't':
    case 'T':
    case 's':
    case 'S':

      for (x = 0; x < handle->width; x++)
      {
        Write_Targa_Pixel (handle, line_data[x][BLUE], line_data[x][GREEN], line_data[x][RED], line_data[x][TRANSM]);
      }

      break;

    case 'c':
    case 'C':

      llen = handle->width;

      startx = 0;

      cnt = 1;

      curpix.blue   = line_data[startx][BLUE];
      curpix.green  = line_data[startx][GREEN];
      curpix.red    = line_data[startx][RED];
      curpix.transm = line_data[startx][TRANSM];

      ptype = 0;

      writenow = FALSE;

      for (;;)
      {
        nexpix.blue   = line_data[startx+cnt][BLUE];
        nexpix.green  = line_data[startx+cnt][GREEN];
        nexpix.red    = line_data[startx+cnt][RED];
        nexpix.transm = line_data[startx+cnt][TRANSM];

        if ((nexpix.red == curpix.red) && (nexpix.blue == curpix.blue) &&
            (nexpix.green == curpix.green) && (nexpix.transm == curpix.transm))
        {
          if (ptype == 0)
          {
            cnt++;

            if ((cnt >= 128) || ((startx + cnt) >= llen))
            {
              writenow = TRUE;
            }
          }
          else
          {
            cnt--;

            writenow = TRUE;
          }
        }
        else
        {
          if ((ptype == 1) || (cnt <= 1))
          {
            ptype = 1;

            curpix = nexpix;

            cnt++;

            if ((cnt >= 128) || ((startx + cnt) >= llen))
            {
              writenow = TRUE;
            }
          }
          else
          {
            writenow = TRUE;
          }
        }

        if (writenow)
        {
          /* This test SHOULD be unnecessary!  However, it isn't!  [CWM] */

          if (startx + cnt > llen)
          {
            cnt = llen - startx;
          }

          if (ptype == 0)
          {
            putc((int) ((cnt - 1) | 0x80), handle->file);

            Write_Targa_Pixel (handle, curpix.blue, curpix.green, curpix.red, curpix.transm);

            curpix = nexpix;
          }
          else
          {
            putc((int) cnt - 1, handle->file);

            for (x = 0; x < cnt; x++)
            {
               Write_Targa_Pixel (handle,
                 line_data[startx+x][BLUE], line_data[startx+x][GREEN],
                 line_data[startx+x][RED], line_data[startx+x][TRANSM]);
            }
          }
          startx = startx + cnt;

          writenow = FALSE;

          ptype = 0;

          cnt = 1;

          if (startx >= llen)
          {
             break; /* Exit while */
          }
        }
      }

      break; /* End of case */
  }

  if (handle->buffer_size == 0)
  {
    /* Close and reopen file (if not stdout) for integrity in case we crash */

    fflush(handle->file);

    if (!(opts.Options & TO_STDOUT))
    {
      handle->file = freopen(handle->filename, APPEND_BINFILE_STRING, handle->file);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Read_Targa_Line
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
*   Jun 1995 : Added code for 32 bit Targa files. [DB]
*
******************************************************************************/

static int Read_Targa_Line (FILE_HANDLE *handle, COLOUR *line_data, int *line_number)
{
  int x, data, cnt;
  DBL rdata, gdata, bdata, fdata;

  switch (opts.OutputFormat)
  {
    case 't':
    case 'T':
    case 's':
    case 'S':

      for (x = 0; x < handle->width; x++)
      {
        /* Read the BLUE data byte.  If EOF is reached on the first
         * character read, then this line hasn't been rendered yet.
         * Return 0.  If an EOF occurs somewhere within the line, this
         * is an error - return -1.
         */

        if ((data = getc(handle->file)) == EOF)
        {
          if (x == 0)
          {
            return(0);
          }
          else
          {
            return(-1);
          }
        }

        line_data[x][BLUE] = (DBL) data / 255.0;

        /* Read the GREEN data byte. */

        if ((data = getc(handle->file)) == EOF)
        {
          return(-1);
        }

        line_data[x][GREEN] = (DBL) data / 255.0;

        /* Read the RED data byte. */

        if ((data = getc(handle->file)) == EOF)
        {
          return(-1);
        }

        line_data[x][RED] = (DBL) data / 255.0;

        /* Read alpha channel. */

        if (opts.Options & OUTPUT_ALPHA)
        {
          if ((data = getc(handle->file)) == EOF)
          {
            return(-1);
          }

          line_data[x][TRANSM] = 1.0 - (DBL)data / 255.0;
        }
        else
        {
          line_data[x][TRANSM] = 0.0;
        }
      }

      break;

    case 'c':
    case 'C':

      cnt = 0;

      do
      {
        if ((data = getc(handle->file)) == EOF)
        {
          if (cnt == 0)
          {
            return(0);
          }
          else
          {
            return(-1);
          }
        }

        if (data & 0x80)
        {
          x = data & 0x7F;

          if ((data = getc(handle->file)) == EOF)
          {
            return(-1);
          }

          bdata = (DBL) data / 255.0;

          if ((data = getc(handle->file)) == EOF)
          {
            return(-1);
          }

          gdata = (DBL) data / 255.0;

          if ((data = getc(handle->file)) == EOF)
          {
            return(-1);
          }

          rdata = (DBL) data / 255.0;

          /* Read alpha channel if any. */

          if (opts.Options & OUTPUT_ALPHA)
          {
            if ((data = getc(handle->file)) == EOF)
            {
              return(-1);
            }

            fdata = 1.0 - (DBL)data / 255.0;
          }
          else
          {
            fdata = 0.0;
          }

          do
          {
            line_data[cnt][BLUE]   = bdata;
            line_data[cnt][GREEN]  = gdata;
            line_data[cnt][RED]    = rdata;
            line_data[cnt][TRANSM] = fdata;

            cnt++;
          }
          while (--x != -1);
        }
        else
        {
          x = data & 0x7F;

          do
          {
            if ((data = getc(handle->file)) == EOF)
            {
              return(-1);
            }

            bdata = (DBL) data / 255.0;

            if ((data = getc(handle->file)) == EOF)
            {
              return(-1);
            }

            gdata = (DBL) data / 255.0;

            if ((data = getc(handle->file)) == EOF)
            {
              return(-1);
            }

            rdata = (DBL) data / 255.0;

            /* Read alpha channel if any. */

            if (opts.Options & OUTPUT_ALPHA)
            {
              if ((data = getc(handle->file)) == EOF)
              {
                return(-1);
              }

              fdata = 1.0 - (DBL)data / 255.0;
            }
            else
            {
              fdata = 0.0;
            }

            line_data[cnt][BLUE]   = bdata;
            line_data[cnt][GREEN]  = gdata;
            line_data[cnt][RED]    = rdata;
            line_data[cnt][TRANSM] = fdata;

            cnt++;
          }
          while (--x != -1);
        }
      }
      while (cnt < handle->width);

      if (cnt != handle->width)
      {
        return(-1);
      }

      break;
  }

  *line_number = Targa_Line_Number++;

  return(1);
}



/*****************************************************************************
*
* FUNCTION
*
*   Close_Targa_File
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

static void Close_Targa_File (FILE_HANDLE *handle)
{
  if (handle->file)
  {
    fflush(handle->file);

    if (!(opts.Options & TO_STDOUT))
      fclose (handle->file);
  }

  if (handle->buffer != NULL)
  {
    POV_FREE (handle->buffer);
  }

  handle->file = NULL;
  handle->buffer = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   convert_targa_color
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

static void convert_targa_color(IMAGE_COLOUR *tcolor, unsigned pixelsize, unsigned char  *bytes)
{
  switch (pixelsize)
  {
    case 1:

      tcolor->Red    = bytes[0];
      tcolor->Green  = bytes[0];
      tcolor->Blue   = bytes[0];
      tcolor->Filter = 0;
      tcolor->Transmit = 0;

      break;

    case 2:

      tcolor->Red    = ((bytes[1] & 0x7c) << 1);
      tcolor->Green  = (((bytes[1] & 0x03) << 3) | ((bytes[0] & 0xe0) >> 5)) << 3;
      tcolor->Blue   = (bytes[0] & 0x1f) << 3;
      tcolor->Filter = 0;
      tcolor->Transmit = 255 - (bytes[1] & 0x80 ? 255 : 0);

      break;

    case 3:

      tcolor->Red    = bytes[2];
      tcolor->Green  = bytes[1];
      tcolor->Blue   = bytes[0];
      tcolor->Filter = 0;
      tcolor->Transmit = 0;

      break;

    case 4:

      tcolor->Red    = bytes[2];
      tcolor->Green  = bytes[1];
      tcolor->Blue   = bytes[0];
      tcolor->Filter = 0;
      tcolor->Transmit = 255 - bytes[3];

      break;

    default:

      Error("Bad pixelsize in TGA color.\n");
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Read_Targa_Image
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
*   Reads a Targa image into an RGB image buffer.  Handles 8, 16, 24, 32 bit
*   formats.  Raw or color mapped. Simple raster and RLE compressed pixel
*   encoding. Right side up or upside down orientations.
*
* CHANGES
*
*   Jun 1995 : Added code for 32 bit Targa files. [DB]
*
******************************************************************************/

void Read_Targa_Image(IMAGE *Image, char *name)
{
  int h;
  int temp;
  unsigned i, j, k;
  unsigned char cflag = 0, *map_line = NULL, bytes[4], tgaheader[18];
  unsigned ftype, idlen, cmlen, cmsiz, psize, orien;
  unsigned width, height;
  FILE *filep;
  IMAGE_LINE *line_data = NULL;
  IMAGE_COLOUR *cmap, pixel;

  /* Start by trying to open the file */

  if ((filep = Locate_File(name, READ_BINFILE_STRING,".tga",".TGA",NULL,TRUE)) == NULL)
  {
    Error ("Error opening TGA image.\n");
    return;	/* -hdf99- */
  }

  if (fread(tgaheader, 18, 1, filep) != 1)
  {
    Error ("Error reading header of TGA image.\n");
    return;	/* -hdf99- */
  }

  /* Decipher the header information */

  idlen  = tgaheader[ 0];
  ftype  = tgaheader[ 2];
  cmlen  = tgaheader[ 5] + (tgaheader[ 6] << 8);
  cmsiz  = tgaheader[ 7] / 8;
  width  = tgaheader[12] + (tgaheader[13] << 8);
  height = tgaheader[14] + (tgaheader[15] << 8);
  psize  = tgaheader[16] / 8;
  orien  = tgaheader[17] & 0x20; /* Right side up ? */

  Image->iwidth  = width;
  Image->iheight = height;
  Image->width   = (DBL)width;
  Image->height  = (DBL)height;
  Image->Colour_Map_Size = cmlen;
  Image->Colour_Map = NULL;

  /* Determine if this is a supported Targa type */

  if (ftype == 9 || ftype == 10 || ftype == 11)
  {
    cflag = 1;
  }
  else
  {
    if (ftype == 1 || ftype == 2 || ftype == 3)
    {
      cflag = 0;
    }
    else
    {
      Error("Unsupported file type %d in TGA image.\n", ftype);
    }
  }

  /* Skip over the picture ID information */

  if (idlen > 0 && fread(idbuf, idlen, 1, filep) != 1)
  {
    Error ("Error reading header from TGA image.\n");
  }

  /* Read in the the color map (if any) */

  if (cmlen > 0)
  {
    if (psize != 1)
    {
      Error("Unsupported color map bit depth (%d bpp) in TGA image.\n",
            psize * 8);
    }

    cmap = (IMAGE_COLOUR *)POV_MALLOC(cmlen * sizeof(IMAGE_COLOUR), "TGA color map");

    for (i = 0; i < cmlen; i++)
    {
      for (j = 0; j < cmsiz; j++)
      {
        if ((temp = fgetc(filep)) == EOF)
        {
          Error("Error reading color map from TGA image.\n");
        }
        else
        {
          bytes[j] = (unsigned char)temp;
        }
      }

      convert_targa_color(&cmap[i], cmsiz, bytes);
    }

    Image->Colour_Map = cmap;
  }
  else
  {
    Image->Colour_Map = NULL;
  }

  /* Allocate the buffer for the image */

  if (cmlen > 0)
  {
    Image->data.map_lines = (unsigned char **)POV_MALLOC(height * sizeof(unsigned char *), "TGA image");
  }
  else
  {
    Image->data.rgb_lines = (IMAGE_LINE *)POV_MALLOC(height * sizeof(IMAGE_LINE), "TGA image");
  }

  for (i = 0; i < height; i++)
  {
    k = width * sizeof(unsigned char);

    if (cmlen > 0)
    {
      map_line = (unsigned char *)POV_MALLOC((size_t)k, "TGA image line");

      Image->data.map_lines[i] = map_line;
    }
    else
    {
      line_data = &Image->data.rgb_lines[i];

      line_data->red    = (unsigned char *)POV_MALLOC((size_t)k, "TGA image line");
      line_data->green  = (unsigned char *)POV_MALLOC((size_t)k, "TGA image line");
      line_data->blue   = (unsigned char *)POV_MALLOC((size_t)k, "TGA image line");

      if (psize > 3)
      {
        line_data->transm = (unsigned char *)POV_MALLOC((size_t)k, "TGA image line");
      }
      else
      {
        line_data->transm = (unsigned char *)NULL;
      }
    }
  }

  /* Read the image into the buffer */

  if (cflag)
  {
    /* RLE compressed images */

    if (cmlen > 0)
    {
      if (orien)
      {
        map_line = Image->data.map_lines[0];
      }
      else
      {
        map_line = Image->data.map_lines[height-1];
      }
    }
    else
    {
      if (orien)
      {
        line_data = &Image->data.rgb_lines[0];
      }
      else
      {
        line_data = &Image->data.rgb_lines[height-1];
      }
    }

    i = 0; /* row counter */
    j = 0; /* column counter */

    while (i < height)
    {
      /* Grab a header */

      if ((h = fgetc(filep)) == EOF)
      {
        Error("Error reading data from TGA image.\n");
      }

      if (h & 0x80)
      {
        /* Repeat buffer */

        h &= 0x7F;

        for (k = 0; k < psize; k++)
        {
          if ((temp = fgetc(filep)) == EOF)
          {
            Error("Error reading data from TGA image.\n");
          }
          else
          {
            bytes[k] = (unsigned char)temp;
          }
        }

        if (cmlen == 0)
        {
          convert_targa_color(&pixel, psize, bytes);
        }

        for (; h >= 0; h--)
        {
          if (cmlen > 0)
          {
            map_line[j] = bytes[0];
          }
          else
          {
            line_data->red[j]    = (unsigned char)pixel.Red;
            line_data->green[j]  = (unsigned char)pixel.Green;
            line_data->blue[j]   = (unsigned char)pixel.Blue;
            if (psize > 3)
            {
              line_data->transm[j] = (unsigned char)pixel.Transmit;
            }
          }

          if (++j == width)
          {
            i++;

            if (cmlen > 0)
            {
              if (orien)
              {
                map_line = Image->data.map_lines[i];
              }
              else
              {
                map_line = Image->data.map_lines[height-i-1];
              }
            }
            else
            {
              line_data += (orien ? 1 : -1);
            }

            j = 0;
          }
        }
      }
      else
      {
        /* Copy buffer */

        for (; h >= 0; h--)
        {
          for (k = 0; k < psize ; k++)
          {
            if ((temp = fgetc(filep)) == EOF)
            {
              Error("Error reading data from TGA image.\n");
            }
            else
            {
              bytes[k] = (unsigned char)temp;
            }
          }

          if (cmlen > 0)
          {
            map_line[j] = bytes[0];
          }
          else
          {
            convert_targa_color(&pixel, psize, bytes);

            line_data->red[j]    = (unsigned char)pixel.Red;
            line_data->green[j]  = (unsigned char)pixel.Green;
            line_data->blue[j]   = (unsigned char)pixel.Blue;
            if (psize > 3)
            {
              line_data->transm[j] = (unsigned char)pixel.Transmit;
            }
          }

          if (++j == width)
          {
            i++;

            if (cmlen > 0)
            {
              if (orien)
              {
                map_line = Image->data.map_lines[i];
              }
              else
              {
                map_line = Image->data.map_lines[height-i-1];
              }
            }
            else
            {
              line_data += (orien ? 1 : -1);
            }

            j = 0;
          }
        }
      }
    }
  }
  else
  {
    /* Simple raster image file, read in all of the pixels */

    if (cmlen == 0)
    {
      if (orien)
      {
        line_data = &Image->data.rgb_lines[0];
      }
      else
      {
        line_data = &Image->data.rgb_lines[height-1];
      }
    }

    for (i = 0; i < height; i++)
    {
      if (cmlen > 0)
      {
        if (orien)
        {
          map_line = Image->data.map_lines[i];
        }
        else
        {
          map_line = Image->data.map_lines[height-i-1];
        }
      }

      for (j = 0; j < width; j++)
      {
        for (k = 0; k < psize; k++)
        {
          if ((temp = fgetc(filep)) == EOF)
          {
            Error("Error reading data from TGA image.\n");
          }
          else
          {
            bytes[k] = (unsigned char)temp;
          }
        }

        if (cmlen > 0)
        {
          map_line[j] = bytes[0];
        }
        else
        {
          convert_targa_color(&pixel, psize, bytes);

          line_data->red[j]    = (unsigned char)pixel.Red;
          line_data->green[j]  = (unsigned char)pixel.Green;
          line_data->blue[j]   = (unsigned char)pixel.Blue;
          if (psize > 3)
          {
            line_data->transm[j] = (unsigned char)pixel.Transmit;
          }
        }
      }

      if (cmlen == 0)
      {
        line_data += (orien ? 1 : -1);
      }
    }
  }

  /* Any data following the image is ignored. */

  /* Close the image file */

  fclose(filep);
}
