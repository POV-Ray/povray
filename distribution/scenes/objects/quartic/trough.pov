// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

// By Alexander Enzmann
/* sample quartic scene file written by Alexander Enzmann */
#version  3.7;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"

#declare Quartic_Saddle =
quartic {
 < 0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  4.0,  0.0,  0.0,
   0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
   0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
   0.0,  0.0,  0.0,  0.0, 0.0 >
}


/* Crossed Trough */
intersection {
   object {
      Quartic_Saddle
      texture {
         pigment { color rgb<0.8,0.25,0.0>*1.8  }
         finish {
            specular 0.8
            roughness 0.005
            ambient 0.3
             diffuse 0.6
         }
      }
   }

   object { UnitBox texture { pigment {Clear} } }

   bounded_by { object { UnitBox scale 1.5 } }

   scale 2
   rotate -10*y
   rotate -60*x
   translate 4*z
}
//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------

camera {
   location  <0.0, 0.0, -4.0>
   right     x*image_width/image_height
   up        <0.0, 1.0,  0.0>
   direction <0.0, 0.1,  1.5>
}

light_source { <200, 30, -300> colour White }

background { color rgb<1,1,1>*0.03 } 
 
