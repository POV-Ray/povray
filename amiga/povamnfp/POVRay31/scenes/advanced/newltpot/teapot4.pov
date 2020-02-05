// Persistence Of Vision raytracer version 3.1 sample file.
// Full-blown Tiffany style teapot.  Reticulated gold body, with
// gold wire trim


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "golds.inc"
#include "skies.inc"

camera {
   location  <0, 4, -6>
   direction <0, 0,  1.25>
   up        <0, 1,  0>
   right     <4/3, 0,  0>
   look_at   <-0.25, 1.5, 0>
}

light_source {<10, 20, -30> color White}

plane { y, -1
    pigment { color red 0.13 green 0.37 blue 0.41  }
    finish { ambient 0.1 }
}

sky_sphere { S_Cloud1 }

#declare Cyl1_Pot = union { #include "teapot.c1" }
#declare Cyl2_Pot = union { #include "teapot.c2" }
#declare Cyl3_Pot = union { #include "teapot.c3" }
#declare Tri_Pot = union { #include "teapot.tri" }
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
