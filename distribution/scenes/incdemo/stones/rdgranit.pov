// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;
global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "rdgranit.map"

camera {
   location <0, 10, -20>
   direction <0, 0,  3>
   right   x*image_width/image_height
   look_at 1.5*y
}

light_source {<-50, 50, -1000> color Gray75}
light_source {< 50, 30, -20> color White}

background { color Gray30 }

#declare Stack =
union {
   sphere{<0, 4, 0>, 1}
   cone { -y,1, y, 0.5 translate 2*y }
   box { -1, 1}
   no_shadow
}

object {
    Stack
    texture{
        pigment {
            granite
            color_map { M_RedGranite }
            scale 0.4
        }
        finish { specular 0.75 roughness 0.0085 }
    }
}
