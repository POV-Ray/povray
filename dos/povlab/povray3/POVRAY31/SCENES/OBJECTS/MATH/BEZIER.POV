// Persistence Of Vision raytracer version 3.1 sample file.

// Simpler Bezier patch example
// by Alexander Enzmann


#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

bicubic_patch { type 1 flatness 0.1  u_steps 8  v_steps 8
   < 0.0, 0.0, 2.0>, < 1.0, 0.0, 0.0>, < 2.0, 0.0, 0.0>, < 3.0, 0.0, -2.0>,
   < 0.0, 1.0, 0.0>, < 1.0, 1.0, 0.0>, < 2.0, 1.0, 0.0>, < 3.0, 1.0,  0.0>,
   < 0.0, 2.0, 0.0>, < 1.0, 2.0, 0.0>, < 2.0, 2.0, 0.0>, < 3.0, 2.0,  0.0>,
   < 0.0, 3.0, 2.0>, < 1.0, 3.0, 0.0>, < 2.0, 3.0, 0.0>, < 3.0, 3.0, -2.0>

   texture {
      pigment {
         checker color Red color Blue
         rotate 90*x
         quick_color Red
      }
      finish { ambient 0.1 diffuse 0.9 phong 1 }
   }

   translate <-1.5, -1.5, 0>
   scale 2
   rotate <30, -60, 0>

}

// Back wall 
plane {
    z, 500
   hollow on

   texture {
      pigment { color red 0.4 green 0.4 blue 0.4 }
   }
}

camera {
   location  <0.0, 0.0, -15.0>
   right     <4/3, 0.0,  0.0>
   up        <0.0, 1.0,  0.0>
   direction <0.0, 0.0,  1.0>
}

// Light source 
light_source { <5, 7, -5> colour White }
