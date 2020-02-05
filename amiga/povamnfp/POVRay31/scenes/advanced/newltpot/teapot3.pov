// Persistence Of Vision raytracer version 3.1 sample file.
// Gold wire "basket" around white marble teapot


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "golds.inc"
#include "metals.inc"
#include "skies.inc"

camera {
   location  <0, 4, -5.5>
   direction <0, 0,  1.1>
   up        <0, 1,  0>
   right     <4/3, 0,  0>
   look_at   <0.5, 0.5, 0>
}

light_source {<10, 20, -30> color White}
light_source {<15, 30, 10> color White shadowless }

plane { y, 0
    pigment { color rgb <0.13, 0.41, 0.37> * 0.35 }
    finish {
        ambient 0.1
        diffuse 0.5
//        reflection 0.35
    }
}

sky_sphere { S_Cloud1 }

// teapot.raw lies with top/bottom along the z axis
// Extents of teapot.raw:
// x -3 to  3.434
// y  0 to -2
// z -2 to  3.15

#declare Cyl1_Pot = union { #include "teapot.c1" }
#declare Cyl2_Pot = union { #include "teapot.c2" }
#declare Cyl3_Pot = union { #include "teapot.c3" }
#declare Tri_Pot = union { #include "teapot.tri" }
union {
    object { Cyl1_Pot texture { T_Gold_5C } }
    object { Cyl2_Pot texture { T_Gold_5C } }
    object { Cyl3_Pot texture { T_Gold_5C } }
    object { Tri_Pot
        texture {
            pigment {
                wrinkles
                color_map { White_Marble_Map }
                scale 0.3
                turbulence 0.5
                omega 0.707
            }
            finish { ambient <0.40, 0.45, 0.50> }
        }
    }
    translate ((3.15-2.0)/2) * z
    rotate -x*90
    translate -y*0.65
    rotate y*60
}
