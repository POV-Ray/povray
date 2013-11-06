// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

// Sample quartic file
// by Alexander Enzmann

#version  3.7;
global_settings { 
  assumed_gamma 1.0
}

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

// a cubic shape, like a cube with smoothed edges in appearance
quartic {
  < 1.0,  0.0,  0.0,   0.0,    0.0,  0.0,  0.0,  0.0,  0.0,   0.0,
    0.0,  0.0,  0.0,   0.0,    0.0,  0.0,  0.0,  0.0,  0.0,   0.0,
    1.0,  0.0,  0.0,   0.0,    0.0,  0.0,  0.0,  0.0,  0.0,   0.0,
    1.0,  0.0,  0.0,   0.0, -1000.0 >
    rotate <20.0, 40.0, 30.0>

   texture {
      pigment { color rgb<1,0.65,0.0> }
      finish {
         phong 1.0
         phong_size 50
        // ambient 0.2
        // diffuse 0.8
      }
   } 
   scale 1.25
   rotate -45*x
   translate <0,0,20>
}

// Put down checkered floor
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
   angle 65
   location  <0.0, 2.0, -5.0>
   right     x*image_width/image_height
   look_at   <0.0, 1.5,   0.0>
}

light_source { <50, 100, 0> colour White }

light_source { <-200, 30, -300> colour White }

background { color rgb<1,1,1>*0.03 } 

