// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dan Farmer
// Fisheye lens example


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "shapes.inc"

camera {
   ultra_wide_angle
   location <0, 4, -8>
   direction <0, 0, 2>
   up <0, 1, 0>
   right <1.33, 0, 0>
   look_at <0.5, 0, 0>
   angle 300
}

background { color Gray55 }

light_source { <-2, 4, 1> color Gray70 }
light_source { <10, 8, 1> color Gray40 }

plane { y,0 pigment { checker color Plum color White } }

#default {
    pigment { SteelBlue }
    finish {
        reflection 0.9
        diffuse 0
        ambient 0.1
        phong 1 phong_size 30
    }
}
object { Disk_Y translate < -2, 1,  6> }
object { Disk_Y translate <  0, 1,  0> }
object { Disk_Y translate <  2, 1, -3> }
