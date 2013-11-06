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

/* Parabolic Torus having major radius sqrt(40), minor radius sqrt(12) */
quartic {
   < 1.0,  0.0,  0.0,   0.0,    2.0,  0.0,  0.0,  0.0, -2.0, -104.0,
     0.0,  0.0,  0.0,   0.0,    0.0,  0.0,  0.0,  0.0,  0.0,   0.0,
     1.0,  0.0,  0.0,   0.0,   -2.0, 56.0,  0.0,  0.0,  0.0,   0.0,
     0.0,  0.0,  1.0, 104.0,  784.0 >

   scale 0.7

   bounded_by { sphere { <0, 0, 0>, 40 } }

   texture {
      pigment { color rgb<0.8,0.2,0> }
      finish {
         phong 1.0
         phong_size 20
      }
   }
   rotate <0,40,0>
   rotate <65,0,10>
   translate <2,0,40>
}

/* Put down a floor */
/*
plane {
   y, -20.0

   texture {
      pigment {
         Blue_Agate
         scale 20
      }
      finish {
         ambient 0.5
         diffuse 0.5
      }
   }
}
*/
camera {
   location  <0.0, 0.0, -20.0>
   angle 65 
   right     x*image_width/image_height
}

light_source { <200, 30, -300> colour White }

light_source { <-200, 30, -300> colour White }

background { color rgb<1,1,1>*0.03 } 
 
