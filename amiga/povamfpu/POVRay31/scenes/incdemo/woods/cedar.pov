// Persistence Of Vision Raytracer version 3.1 sample file.
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
light_source {< 50, 30, -20> color White}

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
