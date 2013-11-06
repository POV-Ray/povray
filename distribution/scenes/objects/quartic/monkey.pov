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

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

#declare Basic_Saddle =
quartic {
 < 0.0,  0.0,  0.0,  1.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0,
   0.0,  0.0, -3.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0,
   0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0,
   0.0,  0.0,  0.0, -1.0, 0.0 >
}

#declare Unit_Cube =
   box {
      <-1, -1, -1>, <1, 1, 1>
      texture { pigment { Clear } }
   }

/* Monkey Saddle */
intersection {
   object {
      Basic_Saddle

      texture {
         pigment { Red }
         finish {
            specular 1.0
            roughness 0.05
            ambient 0.2
            diffuse 0.8
         }
      }
   }

   object { Unit_Cube scale 2 }

   bounded_by { box { <-2.5, -2.5, -2.5>, <2.5, 2.5, 2.5> } }

   rotate 20*y
   rotate -30*x
}

camera {
   location  <0.0, 0.0, -10.0>
   right     x*image_width/image_height
   angle 45
}

light_source { <200, 30, -300> colour White }

background { color rgb<1,1,1>*0.03 } 
 
