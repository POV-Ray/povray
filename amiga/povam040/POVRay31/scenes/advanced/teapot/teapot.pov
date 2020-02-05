// Persistence Of Vision raytracer version 3.1 sample file.
// Utah Teapot w/ Bezier patches
// adapted by Alexander Enzmann


global_settings { assumed_gamma 2.2 }

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

#declare Teapot_Texture = texture {
   pigment { Red }
   finish {
      phong 1.0
      phong_size 100
      ambient 0.15
      diffuse 0.8
   }
}

#declare Teapot_Orientation = <-110, 20, 0>;

#include "teapot.inc"

camera {
   location  <0.0, 0.0, -10.0>
   direction <0.0, 0.0,  1.0>
   up        <0.0, 1.0,  0.0>
   right     <4/3, 0.0,  0.0>
}

light_source { <10.0, 40.0, -30.0> colour White }


/* Floor */
plane {
   y, -8

   texture {
      pigment {
         checker color red 1.0 green 0.1 blue 0.1
                 color red 0.8 green 0.8 blue 0.8
         scale 5
      }
   }
}

/* Back wall */
 plane {
    z, 100
    hollow on
    texture { pigment { color red 0.3 green 0.3 blue 0.5 } }
}
