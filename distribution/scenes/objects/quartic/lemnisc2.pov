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
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

union {
   /* Lemniscate of Gerono */
   quartic {
     < 1.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
       0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0, 0.0,  1.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  1.0,  0.0, 0.0 >

      bounded_by { sphere { <0, 0, 0>, 2.01 } }

      texture {
         pigment { rgb<1,0.15,0.3> }
         finish {
           specular 0.3
           // phong 1.0
           // phong_size 50
           // ambient 0.2
           // diffuse 0.8
         }
      }
      rotate 45*z
   }

   /* Lemniscate of Gerono */
   quartic {
      < 1.0,  0.0,  0.0,   0.0, 0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
        0.0,  0.0,  0.0,   0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
        0.0,  0.0,  0.0,   0.0, 0.0,  1.0,  0.0,  0.0,  0.0,  0.0,
        0.0,  0.0,  1.0,   0.0, 0.0 >

      bounded_by { sphere { <0, 0, 0>, 2.01 } }

      texture {
         pigment { rgb<0.6,0.45,0.9>*0.8 }
         finish {
            phong 1.0
            phong_size 50
           // ambient 0.2
           // diffuse 0.8
         }
      }
      rotate -45*z
   }

   scale 4.5
   rotate <30, 0, 20>
   translate 5*z
}

camera {
   location  <0.0, 0.0, -6.0>
   right     x*image_width/image_height
   angle 65 
}

light_source { <200, 30, -30> colour White }

light_source { <-200, 30, -300> colour White }

/* Put down floor */
/*
plane {
   y, -30.0
   texture {
      pigment {
         White_Marble
         scale 30
      }
      finish {
         ambient 0.3
         diffuse 0.7
      }
   }
   rotate 5*x
}
*/
background { color rgb<1,1,1>*0.03 } 
 
