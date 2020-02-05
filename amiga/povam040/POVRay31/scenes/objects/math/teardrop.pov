// Persistence Of Vision raytracer version 3.1 sample file.

// Sample qaurtic file 
// by Alexander Enzmann

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "shapesq.inc"

object {
   Piriform
   sturm
   rotate -90*z
   translate 0.5*y
   scale <2, 10, 2>

   texture {
      pigment { SteelBlue }
      finish {
         phong 1.0
         phong_size 20
         ambient 0.2
         diffuse 0.8
      }
   }
}

camera {
   location  <0.0, 0.0, -12.0>
   up        <0.0, 1.0,  0.0>
   right     <4/3, 0.0,  0.0>
   look_at   <0.0, 0.0,  0.0>
}

light_source { <200, 30, -500> colour White }
