// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

// Sample qaurtic file
// by Alexander Enzmann
#version  3.7;
global_settings { 
  assumed_gamma 1.0
}

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "shapesq.inc"

object {
   Piriform
   sturm
   rotate -90*z
   translate 0.5*y
   scale <3, 10, 3>

   texture {
      pigment { SteelBlue }
      finish {
         phong 1.0
         phong_size 20
         ambient 0.2
         diffuse 0.8
      }
   }
}

camera {
   location  <0.0, 0.0, -12.0>
   right     x*image_width/image_height
   look_at   <0.0, 0.0,  0.0>
}

light_source { <200, 30, -500> colour White }

background { color rgb<1,1,1>*0.02 } 
 
