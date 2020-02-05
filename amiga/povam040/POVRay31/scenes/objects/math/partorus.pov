// Persistence Of Vision raytracer version 3.1 sample file.

// By Alexander Enzmann

/* sample quartic scene file written by Alexander Enzmann */

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
      pigment { Red }
      finish {
         phong 1.0
         phong_size 20
      }
   }
   rotate 120*x
   rotate -30*y
   translate 40*z
}

/* Put down a floor */
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

camera {
   location  <0.0, 0.0, -20.0>
   direction <0.0, 0.0,  1.0>
   up        <0.0, 1.0,  0.0>
   right     <4/3, 0.0,  0.0>
}

light_source { <200, 30, -300> colour White }

light_source { <-200, 30, -300> colour White }
