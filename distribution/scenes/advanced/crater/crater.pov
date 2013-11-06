// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// CRATER.POV
// Render CRATER_DAT.POV to create the height_field image that
// is needed in CRATER.POV (this file). Then render this file.
// 
// ( When CRATER.POV is rendered, the output image type must be
// the same as when CRATER_DAT.POV was rendered. That is how it
// works when no image type is specified in the height_field.
// Alternatively you can specify the image type of the
// height_field object in line 23 in this file. See the
// documentation for details. )
//
// -w320 -h240
// -w800 -h600 +a0.3
#version 3.7;
global_settings { assumed_gamma 1.5 }

#include "colors.inc"

camera{ location <0,8,-20>
        right     x*image_width/image_height
        direction z*5 
        look_at 0
      }

light_source{<1000,1000,-1000> White}

height_field {
  "crater_dat.png" smooth
  pigment { rgb<1,1,1>  }
  translate <-.5, 0, -.5>
  scale <17, 1.75, 17>
}
