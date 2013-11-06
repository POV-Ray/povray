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

/* Quartic Cylinder - a Space Needle? */
quartic {
   < 0.0,  0.0,  0.0,  0.0,  1.0,  0.0,  0.0,  0.0,  0.0,  0.01,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.01, 0.0, -0.01 >

   bounded_by { sphere { <0, 0, 0>, 2 } }

   texture {
      pigment { color rgb<1,1,1>*0.4}
      finish {
         phong 1.0
         phong_size 50
         ambient 0.2
         diffuse 0.8
      }
   }
   scale 1.5
   rotate <-30, 20, 50>
   translate 3*z
}

/* Put down checkered floor */
/*
plane {
   y, -20

   texture {
      pigment {
         checker colour NavyBlue colour MidnightBlue
         scale 20.0
      }
      finish {
         ambient 0.8
         diffuse 0.2
      }
   }
}
*/
camera {
   location  <0.0, 0.0, -3.0>
   angle 55 
   up        <0.0, 1.0, 0.0>
   right     x*image_width/image_height
}

light_source { <200, 30, -300> colour White }

light_source { <-200, 30, -300> colour White }

background { color rgb<1,1,1>*0.03 } 
 
