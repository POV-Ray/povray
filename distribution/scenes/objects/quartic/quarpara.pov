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

#declare Rectangle =
   box {
      <-1, -1, -1>, <1, 1, 1>
      texture { pigment { Clear } }
   }

/* Quartic parabola of sorts */
intersection {
   quartic {
     < 0.1,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
       0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
       0.0,  0.0, -1.0, 0.0,  0.9 >
      sturm
      texture {
         pigment { color rgb<0.5,0.3,0.7>*0.7 }
         finish {
            phong 1.0
            phong_size 100
            ambient 0.2
            diffuse 0.8
         }
      }
   }
   object { Rectangle }

   bounded_by { box { <-1, -1, -1>, <1, 1, 1> } }
   /* translate 3*z */
   rotate -30*x
}

camera {
   location  <0.0, 0.0, -10.0>
   angle 30  
   right     x*image_width/image_height
}

light_source { <200, 30, -300> colour White }

light_source { <-200, 30, -300> colour White }

background { color rgb<1,1,1>*0.03 } 
 
