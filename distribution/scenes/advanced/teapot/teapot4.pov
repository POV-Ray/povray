// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Full-blown Tiffany style teapot.  Reticulated gold body, with
// gold wire trim
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

global_settings {
  assumed_gamma 2.2
  max_trace_level 5
}

#include "colors.inc"
#include "textures.inc"
#include "golds.inc"
#include "skies.inc"

camera {
   location  <0, 4, -6>
   angle 55 // direction <0, 0,  1.25>
   up        <0, 1,  0>
   right     x*image_width/image_height
   look_at   <-0.25, 1.5, 0>
}

light_source {<10, 20, -30> color White}

plane { y, -1
    pigment { color red 0.13 green 0.37 blue 0.41  }
    finish { ambient 0.1 }
}

sky_sphere { S_Cloud1 }

#declare Cyl1_Pot = union { #include "teapot_c1.inc" }
#declare Cyl2_Pot = union { #include "teapot_c2.inc" }
#declare Cyl3_Pot = union { #include "teapot_c3.inc" }
#declare Tri_Pot  = union { #include "teapot_tri.inc" }
union {
    object { Cyl1_Pot texture { T_Gold_3C } }
    object { Cyl2_Pot texture { T_Gold_3C } }
    object { Cyl3_Pot texture { T_Gold_3C } }
    object { Tri_Pot
        texture { T_Gold_3C
            normal { granite 0.3 scale 0.2 }
        }
    }
    translate y*1
    translate ((3.15-2.0)/2) * z
    rotate -x*90
    rotate y*60
}
