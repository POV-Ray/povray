// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

// Persistence Of Vision Raytracer
// By Alexander Enzmann

/* sample quartic scene file written by Alexander Enzmann */
#version  3.7;
global_settings { 
  assumed_gamma 1.0
}

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

/*
  Folium - a plane with an oogah horn installed (?) or maybe a sassy
           olive sticking out it's pimento!
*/

intersection {
   quartic {
     < 0.0,  0.0,  0.0,   0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 2.0,
       0.0,  0.0, -3.0,   0.0, 0.0,  0.0,  0.0, -3.0,  0.0, 0.0,
       0.0,  0.0,  0.0,   0.0, 0.0,  1.0,  0.0,  0.0,  0.0, 0.0,
       0.0,  0.0,  1.0,   0.0, 0.0 >

       texture {
          pigment { Red }
          finish {
             phong 1.0
             phong_size 10
             ambient 0.2
             diffuse 0.8
          }
       }
   }

   sphere {
      <0, 0, 0>, 10
      texture { pigment { Clear } }
   }

   bounded_by { sphere { <0, 0, 0>, 11 } }
   rotate <0, 50, 10>
   translate 20*z
}

camera {
   location  <0.0, 0.0,-10.0>
   angle 65
   right     x*image_width/image_height
}

light_source { <-200, 300, -300>  colour White }

background { color rgb<1,1,1>*0.03 } 
 
