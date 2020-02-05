// Persistence Of Vision Raytracer version 3.1 sample file.
#include "colors.inc"
#include "woods.inc"
#include "teak.map"

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
   box {-1,1}
   no_shadow
}

object {
    Stack
    texture{
        pigment {
            P_WoodGrain3A
            color_map { M_Teak }
            scale 2
            rotate x*90
        }
    }
}
