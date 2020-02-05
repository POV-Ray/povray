// Persistence Of Vision raytracer version 3.1 sample file.

// By Alexander Enzmann

/* sample quartic scene file written by Alexander Enzmann */

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

#declare Basic_Saddle =
quartic {
 < 0.0,  0.0,  0.0,  1.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0,
   0.0,  0.0, -3.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0,
   0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0,
   0.0,  0.0,  0.0, -1.0, 0.0 >
}

#declare Unit_Cube =
   box {
      <-1, -1, -1>, <1, 1, 1>
      texture { pigment { Clear } }
   }

/* Monkey Saddle */
intersection {
   object {
      Basic_Saddle

      texture {
         pigment { Red }
         finish {
            specular 1.0
            roughness 0.05
            ambient 0.2
            diffuse 0.8
         }
      }
   }

   object { Unit_Cube scale 2 }

   bounded_by { box { <-2.5, -2.5, -2.5>, <2.5, 2.5, 2.5> } }

   rotate 20*y
   rotate -30*x
}

camera {
   location  <0.0, 0.0, -10.0>
   right     <4/3, 0.0,  0.0>
   up        <0.0, 1.0,  0.0>
   direction <0.0, 0.0,  1.0>
}

light_source { <200, 30, -300> colour White }
