// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

// Simpler Bezier patch example
// by Alexander Enzmann

#version 3.7;
global_settings{ assumed_gamma 1.0 }

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

bicubic_patch { type 1 flatness 0.1  u_steps 8  v_steps 8
   < 0.0, 0.0, 2.0>, < 1.0, 0.0, 0.0>, < 2.0, 0.0, 0.0>, < 3.0, 0.0, -2.0>,
   < 0.0, 1.0, 0.0>, < 1.0, 1.0, 0.0>, < 2.0, 1.0, 0.0>, < 3.0, 1.0,  0.0>,
   < 0.0, 2.0, 0.0>, < 1.0, 2.0, 0.0>, < 2.0, 2.0, 0.0>, < 3.0, 2.0,  0.0>,
   < 0.0, 3.0, 2.0>, < 1.0, 3.0, 0.0>, < 2.0, 3.0, 0.0>, < 3.0, 3.0, -2.0>

   texture {
      pigment {
         checker color Red color rgb<1,1,1>
         rotate 90*x
         quick_color Red
      }
      finish { ambient 0.1 diffuse 0.9 phong 1 }
   }

   translate <-1.5, -1.5, 0>
   scale 2
   rotate <30, -60, 0>

}

// Back wall
plane {
    z, 500
   hollow on

   texture {
      pigment { color rgb<1,1,1>*0.1 } //red 0.4 green 0.4 blue 0.4 }
   }
}

camera {
   location  <0.0, 0.0, -15.0>
   right     x*image_width/image_height
   angle 50
}

// Light source
light_source { <5, 7, -5> colour White }
