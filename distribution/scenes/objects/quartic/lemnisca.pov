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

/* Lemniscate of Gerono */
quartic {
   < 1.0,  0.0,  0.0,   0.0, 0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
     0.0,  0.0,  0.0,   0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,   0.0, 0.0,  1.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  1.0,   0.0, 0.0 >

   bounded_by { sphere { <0, 0, 0>, 2 } }

   texture {
      pigment { rgb<1,0.35,0.2> }
      finish {
         phong 0.3
         phong_size 100
         ambient 0.2
         diffuse 0.8
      }
   }
   rotate -45*y
   translate <0.1,0,2>
}

/* Put down checkered floor */
/*
plane {
   y, -20.0
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
   location  <0.0, 1.0, -2.0>
   right     x*image_width/image_height
   look_at   <0.0,-0.8,  5.0>
   angle 30
}

light_source { <200, 30, -300> color White }

light_source { <-200, 30, -300> color White }

background { color rgb<1,1,1>*0.03 } 
 
