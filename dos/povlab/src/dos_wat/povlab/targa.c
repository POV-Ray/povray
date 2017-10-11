/* ---------------------------------------------------------------------------
*  TARGA.C
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
#include <MATH.H>
#include <FLOAT.H>
#include <STDLIB.H>
#include <STDIO.H>
#include <STRING.H>
#include <MALLOC.H>
#include <DOS.H>
#include <TIME.H>
#include <GRAPH.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

// ---------------------------------------------------------------------------
// -- CONVERSION COULEUR EN FONCTION TYPE DE BIT COULEUR ---------------------
// ---------------------------------------------------------------------------
static byte convert_targa_color(IMAGE_COLOUR *tcolor,unsigned pixelsize,byte *bytes) {
  switch (pixelsize) {
	case 1:
	  tcolor->Red	= bytes[0];
	  tcolor->Green = bytes[0];
	  tcolor->Blue	= bytes[0];
	  tcolor->Filter = 0;
	  break;
	case 2:
	  tcolor->Red	= ((bytes[1] & 0x7c) << 1);
	  tcolor->Green = (((bytes[1] & 0x03) << 3) | ((bytes[0] & 0xe0) >> 5)) << 3;
	  tcolor->Blue	= (bytes[0] & 0x1f) << 3;
	  tcolor->Filter = 0;
	  break;
	case 3:
	  tcolor->Red	= bytes[2];
	  tcolor->Green = bytes[1];
	  tcolor->Blue	= bytes[0];
	  tcolor->Filter = 0;
	  break;
	case 4:
	  tcolor->Red	= bytes[2];
	  tcolor->Green = bytes[1];
	  tcolor->Blue	= bytes[0];
	  tcolor->Filter = 0;
	  break;
	default:
	  f_erreur("Bad pixelsize in Targa color.");
	  return 0;
  }
  return 1;
}

// -----------------------------------------------------------------------
// -- LECTURE D'UN FICHIER TARGA .TGA ------------------------------------
// -- Reads a Targa image into an RGB image buffer.  Handles 8, 16, 24, 32 bit
// -- formats.	Raw or color mapped. Simple raster and RLE compressed pixel
// -- encoding. Right side up or upside down orientations.
// -----------------------------------------------------------------------
byte lecture_targa(IMAGE *Image,char *Name) {
  byte idbuf[256];
  FILE *filep;
  IMAGE_LINE *line_data;
  IMAGE_COLOUR *cmap, pixel;
  int h;
  unsigned i, j, k;
  int temp;
  byte cflag, *map_line, bytes[4], tgaheader[18];
  unsigned ftype, idlen, cmlen, cmsiz, psize, orien;
  unsigned width, height;

  // -------------- Start by trying to open the file

  if ((filep = fopen(Name,"rb")) == NULL) {
	f_erreur("Cannot open Targa image file %s.",Name);
	return 0;
  }

  if (fread(tgaheader, 18, 1, filep) != 1) {
    f_erreur("Error reading header of Targa image %s.",Name);
	return 0;
  }

  message("Reading Targa file %s...",Name);

  // --------------- Decipher the header information

  idlen  = tgaheader[ 0];
  ftype  = tgaheader[ 2];
  cmlen  = tgaheader[ 5] + (tgaheader[ 6] << 8);
  cmsiz  = tgaheader[ 7] / 8;
  width  = tgaheader[12] + (tgaheader[13] << 8);
  height = tgaheader[14] + (tgaheader[15] << 8);
  psize  = tgaheader[16] / 8;
  orien  = tgaheader[17] & 0x20; // Right side up ?

  Image->iwidth  = width;
  Image->iheight = height;
  Image->width	 = (DBL)width;
  Image->height  = (DBL)height;
  Image->Colour_Map_Size = cmlen;
  Image->Colour_Map = NULL;

  // ------- Determine if this is a supported Targa type

  if (ftype == 9 || ftype == 10 || ftype == 11) {
	cflag = 1;
  } else {
	if (ftype == 1 || ftype == 2 || ftype == 3)  {
	  cflag = 0;
	} else {
	  f_erreur( "Image file %s is an unsupported Targa type [0x%02X].",Name,ftype);
	  return 0;
	}
  }

  // ------------- Skip over the picture ID information

  if (idlen > 0 && fread(idbuf, idlen, 1, filep) != 1) {
	f_erreur("Reading identification field of %s.",Name);
	return 0;
  }

  // ------------- Read in the the color map (if any)

  if (cmlen > 0) {
	if (psize != 1) {
	  f_erreur("Can't support %d bits in a color map index.",psize * 8);
	  return 0;
	}
	if ((cmap=(IMAGE_COLOUR *)malloc(cmlen * sizeof(IMAGE_COLOUR)))==NULL) {
	  f_erreur("Can't allocate memory for image %s.", Name);
	  return 0;
	}
	for (i=0;i<cmlen;i++) {
	  for (j=0;j<cmsiz;j++) {
		if ((temp = fgetc(filep)) == EOF) {
		  f_erreur("Premature EOF for image file %s.", Name);
		  return 0;
		} else {
		  bytes[j] = (byte)temp;
		}
	  }
	  if (!convert_targa_color(&cmap[i], cmsiz, bytes)) return 0;
	  }
	Image->Colour_Map = cmap;
  } else {
	Image->Colour_Map = NULL;
  }

  // ---------- Allocate the buffer for the image
  if (cmlen>0) {
	if ((Image->data.map_lines = (byte **) malloc(height * sizeof(byte *)))==NULL) {
	  f_erreur("Cannot allocate memory for image: %s.", Name);
	  return 0;
	}
  } else {
	if ((Image->data.rgb_lines = (struct Image_Line *) malloc(height * sizeof(struct Image_Line))) == NULL) {
	  f_erreur("Cannot allocate memory for image: %s.", Name);
	  return 0;
	}
  }

  for (i=0;i<height;i++) {
      k = width * sizeof(byte);
	if (cmlen > 0) {
	  map_line = (byte *) malloc(k);
	  if (map_line == NULL) {
		f_erreur("Can't allocate memory for image %s.", Name);
		return 0;
	  }
	  Image->data.map_lines[i] = map_line;
	} else {
	  line_data = &Image->data.rgb_lines[i];
	  k = width * sizeof(byte);
	  line_data->red   = (byte *)malloc(k);
	  line_data->green = (byte *)malloc(k);
	  line_data->blue  = (byte *)malloc(k);
	  if (line_data->red == NULL || line_data->green == NULL || line_data->blue == NULL) {
		f_erreur("Can't allocate memory for image %s.", Name);
		return 0;
	  }
	}
  }

  // -------------- Read the image into the buffer

  if (cflag) {
	// -------------- RLE compressed images
	if (cmlen > 0) {
	  if (orien) {
		map_line = Image->data.map_lines[0];
	  } else {
		map_line = Image->data.map_lines[height-1];
	  }
	} else {
	  if (orien) {
		line_data = &Image->data.rgb_lines[0];
	  } else {
		line_data = &Image->data.rgb_lines[height-1];
	  }
	}

	i = 0; // ------------ row counter
	j = 0; // ------------ column counter

	while (i < height) {
	  //  --------------- Grab a header
	  if ((h = fgetc(filep)) == EOF) {
		f_erreur("Premature EOF in image file %s.",Name);
		return 0;
	  }
	  if (h & 0x80) {
		// Repeat buffer
		h &= 0x7F;
		for (k=0;k<psize;k++) {
		  if ((temp = fgetc(filep)) == EOF) {
			f_erreur("Premature EOF for image file %s.",Name);
			return 0;
		  } else {
			bytes[k] = (byte)temp;
		  }
		}

		if (cmlen == 0)  if (!convert_targa_color(&pixel, psize, bytes)) return 0;

		for (;h>=0;h--) {
		  if (cmlen > 0) {
			map_line[j] = bytes[0];
		  } else {
			line_data->red[j]	= (byte) pixel.Red;
			line_data->green[j] = (byte) pixel.Green;
			line_data->blue[j]	= (byte) pixel.Blue;
		  }

		  if (++j == width) {
			i++;
			if (cmlen > 0) {
			  if (orien) {
				map_line = Image->data.map_lines[i];
			  } else {
				map_line = Image->data.map_lines[height-i-1];
			  }
			} else {
			  line_data += (orien ? 1 : -1);
			}
			j = 0;
		  }
		}
	  } else {
		//	------------ Copy buffer
		for (;h>=0;h--) {
		  for (k=0;k<psize;k++) {
			if ((temp = fgetc(filep)) == EOF) {
			  f_erreur("Premature EOF for image file %s.", Name);
			  return 0;
			} else {
			  bytes[k] = (unsigned char)temp;
			}
		  }
		  if (cmlen > 0) {
			map_line[j] = bytes[0];
		  } else {
			if (!convert_targa_color(&pixel, psize, bytes)) return 0;
			line_data->red[j]	= (unsigned char)pixel.Red;
			line_data->green[j] = (unsigned char)pixel.Green;
			line_data->blue[j]	= (unsigned char)pixel.Blue;
		  }
		  if (++j == width) {
			i++;
			if (cmlen > 0) {
			  if (orien) {
				map_line = Image->data.map_lines[i];
			  } else {
				  map_line = Image->data.map_lines[height-i-1];
			  }
			} else {
			  line_data += (orien ? 1 : -1);
			}
			j = 0;
		  }
		}
	  }
	}
  } else {
	// ----------- Simple raster image file, read in all of the pixels
	if (cmlen == 0) {
	  if (orien) {
		line_data = &Image->data.rgb_lines[0];
	  } else {
		line_data = &Image->data.rgb_lines[height-1];
	  }
	}
	for (i=0;i<height;i++) {
	  if (cmlen > 0) {
		if (orien) {
		  map_line = Image->data.map_lines[i];
		} else {
		  map_line = Image->data.map_lines[height-i-1];
		}
	  }
	  for (j=0;j<width;j++) {
		for (k=0;k<psize;k++) {
		  if ((temp = fgetc(filep)) == EOF) {
			f_erreur("Premature EOF for image file %s.", Name);
			return 0;
		  } else {
			bytes[k] = (byte)temp;
		  }
		}
		if (cmlen > 0) {
		  map_line[j] = bytes[0];
		} else {
		  if (!convert_targa_color(&pixel, psize, bytes)) return 0;
		  line_data->red[j]   = (byte)pixel.Red;
		  line_data->green[j] = (byte)pixel.Green;
		  line_data->blue[j]  = (byte)pixel.Blue;
		}
	  }
	  if (cmlen == 0) line_data += (orien ? 1 : -1);
	}
  }
  // ------------ Any data following the image is ignored.

  // ------------ Close the image file
  fclose(filep);
  return 1;
}

