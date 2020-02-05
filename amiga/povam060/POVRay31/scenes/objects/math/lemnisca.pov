// Persistence Of Vision raytracer version 3.1 sample file.

// By Alexander Enzmann

/* sample quartic scene file written by Alexander Enzmann */

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

/* Lemniscate of Gerono */
quartic {
   < 1.0,  0.0,  0.0,   0.0, 0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
     0.0,  0.0,  0.0,   0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,   0.0, 0.0,  1.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  1.0,   0.0, 0.0 >

   bounded_by { sphere { <0, 0, 0>, 2 } }

   texture {
      pigment { Red }
      finish {
         phong 1.0
         phong_size 10
         ambient 0.2
         diffuse 0.8
      }
   }
   rotate -45*y
   translate 2*z
}

/* Put down checkered floor */
plane {
   y, -20.0
   texture {
      pigment {
         checker colour NavyBlue colour MidnightBlue
         scale 20.0
      }
      finish {
         ambient 0.8
         diffuse 0.2
      }
   }
}

camera {
   location  <0.0, 1.0, -2.0>
   up 	     <0.0, 1.0,  0.0>
   right     <4/3, 0.0,  0.0>
   look_at   <0.0, 0.0,  5.0>
}

light_source { <200, 30, -300> color White }

light_source { <-200, 30, -300> color White }
