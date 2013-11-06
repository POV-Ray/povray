// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "colors.inc"
#include "woods.inc"
#include "bubinga.map"

camera {
   location <0, 10, -20>
   direction <0, 0,  3>
   right   x*image_width/image_height
   look_at 1.5*y
}

light_source {<-50, 50, -1000> color White*0.1 }
light_source {< 50, 30, -30>   color White*0.9 }

background { color Gray05 }

#declare Stack =
union {
   sphere{<0, 4, 0>, 1}
   cone { -y,1, y, 0.5 translate 2*y }
   box {-1,1}
   no_shadow
}

object {
    Stack
    texture{
        pigment {
            P_WoodGrain3A
            color_map { M_Bubinga }
            scale 2
            rotate x*90
        }
    }
}
