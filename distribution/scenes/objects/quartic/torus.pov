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

/* Torus having major radius sqrt(40), minor radius sqrt(12) */
quartic {
   < 1.0,  0.0,  0.0,   0.0,    2.0,  0.0,  0.0,  2.0,  0.0, -104.0,
     0.0,  0.0,  0.0,   0.0,    0.0,  0.0,  0.0,  0.0,  0.0,    0.0,
     1.0,  0.0,  0.0,   2.0,    0.0, 56.0,  0.0,  0.0,  0.0,    0.0,
     1.0,  0.0, -104.0, 0.0,  784.0 >

   bounded_by { sphere { <0, 0, 0>, 10 } }

   texture {
      pigment { color rgb<0.9,0.55,0.1>*0.9  }
      finish {
         phong 1.0
         phong_size 400
         ambient 0.2
         diffuse 0.8
      }
   }
   rotate -45*x
   translate 20*z
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
//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
   location  <0.0, 2.0, -10.0>
   up        <0.0, 1.0,   0.0>
   right     x*image_width/image_height
   look_at   <0.0, 0.9,   0.0>
   angle     50
}

light_source { <50, 100, 0> colour White }

light_source { <-200, 30, -300> colour White }

background { color rgb<1,1,1>*0.03 } 
 
