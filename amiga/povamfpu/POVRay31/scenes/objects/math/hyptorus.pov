// Persistence Of Vision raytracer version 3.1 sample file.

// By Alexander Enzmann

/* sample quartic scene file written by Alexander Enzmann */

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"


/* Hyperbolic Torus having major radius sqrt(40), minor radius sqrt(12) */
quartic {
   < 1.0,  0.0,  0.0,   0.0,    2.0,  0.0,  0.0, -2.0,  0.0, -104.0
     0.0,  0.0,  0.0,   0.0,    0.0,  0.0,  0.0,  0.0,  0.0,   0.0
     1.0,  0.0,  0.0,  -2.0,    0.0, 56.0,  0.0,  0.0,  0.0,   0.0
     1.0,  0.0, 104.0,  0.0,  784.0 >

   texture {
      pigment { Red }
      finish {
         specular 1.0
         roughness 0.01
         ambient 0.2
         diffuse 0.8
      }
   }

   bounded_by { sphere { <0, 0, 0>, 20 } }

   rotate 90*x
   rotate 30*y
   translate 20*z
}

/* Put down a floor */
plane {
   y, -20.0
   texture {
      pigment {
         Blue_Agate
         scale 20.0
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
