// Persistence Of Vision raytracer version 3.1 sample file.

// By Alexander Enzmann
/* sample quartic scene file written by Alexander Enzmann */

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"

#declare Quartic_Saddle =
quartic {
 < 0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  4.0,  0.0,  0.0,
   0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
   0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
   0.0,  0.0,  0.0,  0.0, 0.0 >
}


/* Crossed Trough */
intersection {
   object {
      Quartic_Saddle
      texture {
         pigment { Red }
         finish {
            specular 0.8
            roughness 0.005
            ambient 0.3
            diffuse 0.6
         }
      }
   }

   object { UnitBox texture { pigment {Clear} } }

   bounded_by { object { UnitBox scale 1.5 } }

   scale 2
   rotate -10*y
   rotate -60*x
   translate 4*z
}

camera {
   location  <0.0, 0.0, -4.0>
   right     <4/3, 0.0,  0.0>
   up        <0.0, 1.0,  0.0>
   direction <0.0, 0.0,  1.0>
}

light_source { <200, 30, -300> colour White }
