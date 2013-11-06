// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// CRATER_DAT.POV
// Render CRATER_DAT.POV (this file) to create the height_field
// image that is needed in CRATER.POV. Then render CRATER.POV.
// 
// ( You can use any output image type, but the quality will be
// best with the special 16-bit gray output that is supported
// by the PNG, TGA and PPM image types. For example the command
// line setting +fn will set the output image type to PNG. See
// the documentation for details. ) 
//                                                      
// In POV-Ray 3.7 and later 'hf_gray_16' has to be specified as
// an option on the command-line (e.g. '+FNg'), in an INI file
// (e.g. 'Grayscale_Output=true'), or (on some platforms) in a dialog.
// See the documentation for more details.
//
// -w512 -h384 +FNg

#version 3.7;
global_settings { assumed_gamma 2.0 hf_gray_16 }


#include "colors.inc"

// a wrinkle colored plane

plane {z,10
 hollow on
 pigment{wrinkles
  color_map{
   [0 White*0.3]
   [1 White]
  }
 }
}

// Main spotlight creates crater mountain
light_source {0 color 1  spotlight point_at z*10
  radius 7 falloff 11
}

// Dim spotlight softens outer edges further
light_source {0 color .25  spotlight point_at z*10
  radius 2 falloff 15
}

// Narrow spotlight creates central peak
light_source {0 color .1  spotlight point_at z*10
  radius 0 falloff 1.3
}

// Negative spotlight cuts out crater insides
light_source {0 color -0.9  spotlight point_at z*10
  radius 5 falloff 9.5
}

// Dim negative spotlight counteracts dim positive light in center
light_source {0 color -.25  spotlight point_at z*10
  radius 3 falloff 8
}

