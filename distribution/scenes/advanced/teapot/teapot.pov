// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Utah Teapot w/ Bezier patches
// adapted by Alexander Enzmann
//
// -w320 -h240
// -w800 -h600 +a0.3

// Updated: 09Aug2008 (jh) for v3.7 distribution
// Updated: 28Sep2008 (cjc): change texture, checker color, background color.

#version 3.6;

global_settings {
  assumed_gamma 2.2
}

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "metals.inc"

#declare Teapot_Texture = T_Chrome_5E;
#declare Teapot_Orientation = <-110, 20, 0>;

#include "teapot.inc"

camera {
   location  <0.0, 0.0, -10.0>
   angle 55 // direction <0.0, 0.0,  1.0>
   up        <0.0, 1.0,  0.0>
   right x*image_width/image_height
}

light_source { <10.0, 40.0, -30.0> colour White }

/* Floor */
plane {
   y, -8

   texture {
      pigment {
         checker color red 0.0 green 0.0 blue 0.0
                 color red 0.8 green 0.8 blue 0.8
         scale 5
      }
   }
}

/* Back wall */
 plane {
    z, 100
    hollow on
    texture { pigment { color red 0.1 green 0.1 blue 0.3 } }
}
