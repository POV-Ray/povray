// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "woods.inc"

camera {
   location <0, 10, -20>
   direction <0, 0,  3>
   right x*1.33
   look_at 1.5*y
}

light_source {<-50, 50, -1000> color Gray75}
light_source {< 50, 30, -20>   color White}

background { color Gray30 }

#declare Stack =
union {
   sphere{<0, 4, 0>, 1}
   cone { -y,1, y, 0.5 translate 2*y }
   object {UnitBox}
   no_shadow
}

#include "cedar.map"
object {
    Stack
    texture{
        pigment {
            P_WoodGrain6A
            color_map { M_Cedar }
        }
        normal {            // Copy of WoodGrain6A pigment
            wood 0.4
            turbulence <0.05, 0.08, 1>
            octaves 4
            scale <0.15, .15, 1>
            translate -x*100
        }
     scale 0.6
     rotate z*35
     rotate x*85
    }
}
