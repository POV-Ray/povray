// Persistence Of Vision raytracer version 3.1 sample file.

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
        reflection 0.35
    }
}

sky_sphere { S_Cloud1 }

#declare Teapot_Sphere_Radius = 0.05;
#declare Sph_Pot = union { #include "teapot.sph" }
#declare Cyl2_Pot = union { #include "teapot.c2" }

union {
    object { Cyl2_Pot texture { T_Gold_5C } }
    object { Sph_Pot texture { T_Silver_3C } }
    translate ((3.15-2.0)/2) * z
    rotate -x*90
    translate -y*0.65
    rotate y*60
}
