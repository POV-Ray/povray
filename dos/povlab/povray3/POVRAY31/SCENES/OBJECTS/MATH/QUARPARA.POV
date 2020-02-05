// Persistence Of Vision raytracer version 3.1 sample file.

// By Alexander Enzmann
/* sample quartic scene file written by Alexander Enzmann */

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"

#declare Rectangle =
   box {
      <-1, -1, -1>, <1, 1, 1>
      texture { pigment { Clear } }
   }

/* Quartic parabola of sorts */
intersection {
   quartic {
     < 0.1,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
       0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
       0.0,  0.0, -1.0, 0.0,  0.9 >
      sturm 
      texture {
         pigment { Red }
         finish {
            phong 1.0
            phong_size 20
            ambient 0.2
            diffuse 0.8
         }
      }
   }
   object { Rectangle }

   bounded_by { box { <-1, -1, -1>, <1, 1, 1> } }
   /* translate 3*z */
   rotate -30*x
}

camera {
   location  <0.0, 0.0, -10.0>
   direction <0.0, 0.0,   1.0>
   up        <0.0, 1.0,   0.0>
   right     <4/3, 0.0,   0.0>
}

light_source { <200, 30, -300> colour White }

light_source { <-200, 30, -300> colour White }
