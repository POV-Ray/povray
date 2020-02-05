// Persistence Of Vision raytracer version 3.1 sample file.

// By Alexander Enzmann

/* sample quartic scene file written by Alexander Enzmann */

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
      pigment { color red 0.7 green 0.0 blue 0.0 }
      finish {
         phong 1.0
         phong_size 20
         ambient 0.2
         diffuse 0.8
      }
   }

   translate <0, 0.5, 2>
}

/* Put down checkered floor */
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

camera {
   location  <0.0, 2.0, -2.0>
   up        <0.0, 1.0, 0.0>
   right     <4/3, 0.0, 0.0>
   look_at   <0.0, 0.0, 1.0>
}

light_source { <200, 30, -300> colour White }

light_source { <-200, 30, -300> colour White }
