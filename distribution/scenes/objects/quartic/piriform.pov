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

/* Piriform - looks a bit like a Hershey's Kiss along the x axis...*/
quartic {
   < 4.0,  0.0,  0.0,  -4.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0,
     0.0,  0.0,  0.0,   0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0,
     0.0,  0.0,  0.0,   0.0, 0.0,  1.0,  0.0,  0.0,  0.0, 0.0,
     0.0,  0.0,  1.0,   0.0, 0.0 >

   bounded_by { sphere { <0, 0, 0>, 2 } }

   texture {
      pigment { color rgb<0.5,0.10,0.15> }
      finish {
         phong 1.0
         phong_size 50
         ambient 0.2
         diffuse 0.8
      }
   }
   scale 1.25
   rotate<0,-30,0>
   translate <-0.5, -0, 2>
}

/* Put down checkered floor */
/*
plane {
   y, -10.0

   texture {
      pigment {
         checker colour red 0.137255 green 0.137255 blue 0.556863
                 colour red 0.184314 green 0.184314 blue 0.309804
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
   location  <0.0, 2.0, -2.0>
   right     x*image_width/image_height
   look_at   <0.0, 0.7, 1.0>
   angle     35 
}

light_source { <200, 30, -300> colour White }

light_source { <-200, 30, -300> colour White }

background { color rgb<1,1,1>*0.03 } 
 
