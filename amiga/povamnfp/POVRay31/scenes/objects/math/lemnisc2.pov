// Persistence Of Vision raytracer version 3.1 sample file.

// By Alexander Enzmann

/* sample quartic scene file written by Alexander Enzmann */

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
         pigment { Red }
         finish {
            phong 1.0
            phong_size 10
            ambient 0.2
            diffuse 0.8
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
         pigment { Blue }
         finish {
            phong 1.0
            phong_size 10
            ambient 0.2
            diffuse 0.8
         }
      }
      rotate -45*z
   }

   scale 4
   rotate <30, 0, 20>
   translate 5*z
}

camera {
   location  <0.0, 0.0, -10.0>
   right     <1.0, 0.0,  0.0>
   up        <0.0, 1.0,  0.0>
   direction <0.0, 0.0,  1.0>
}

light_source { <200, 30, -30> colour White }

light_source { <-200, 30, -300> colour White }

/* Put down floor */
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
