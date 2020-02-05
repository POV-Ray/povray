// Persistence Of Vision Raytracer version 3.1 sample file.

global_settings { assumed_gamma 2.2 }
#include "colors.inc"
#include "shapes.inc"
#include "glass.inc"

#declare T01 = texture { T_Glass1 }
#declare T02 = texture { T_Glass2 }
#declare T03 = texture { T_Glass3 }
#declare T04 = texture { T_Glass4 }
#declare T05 = texture { T_Old_Glass }

#declare T06 = texture { T_Winebottle_Glass }
#declare T07 = texture { T_Beerbottle_Glass }
#declare T08 = texture { T_Ruby_Glass       }
#declare T09 = texture { T_Green_Glass      }
#declare T10 = texture { T_Dark_Green_Glass }

#declare T11 = texture { T_Yellow_Glass      }
#declare T12 = texture { T_Orange_Glass      }
#declare T13 = texture { T_Vicksbottle_Glass }

camera {
   location <0, 0, -60>
   direction <0, 0,  2.85>
   right x*1.33
   look_at 0
}

light_source {<-50, 50, -1000> color Gray50}
light_source {<150, 50, -200> color Gray15}

plane { z, 5
    hollow on
    pigment {checker color Gray40 color Gray80
        scale <100000, 1, 1>
    }
    finish { ambient 0.45 }
}

sky_sphere {
    pigment {
        gradient y
        color_map {[0, 1  color Gray20 color Gray80]}
    rotate x*30
    }
}

//plane { z, 2.5 pigment {Gray50} finish { ambient 0.45 } }

#declare Stack =
union {
   sphere{<0, 4, 0>, 1}
   object {Disk_Y translate 2*y}
   object {UnitBox}
   translate -y*1.5
   rotate y*45
}

#declare Row1 =  14;
#declare Row2 =   7;
#declare Row3 =   0;
#declare Row4 =  -7;
#declare Row5 = -14;

object { Stack texture{T01} interior{I_Glass} translate <-6.25, 6, 0> }
object { Stack texture{T02} interior{I_Glass} translate <-2.5,  6, 0> }
object { Stack texture{T03} interior{I_Glass} translate < 2.5,  6, 0> }
object { Stack texture{T04} interior{I_Glass} translate < 6.25, 6, 0> }

object { Stack texture{T05} interior{I_Glass} translate <-9.0, 0, 0> }
object { Stack texture{T06} interior{I_Glass} translate <-4.5, 0, 0> }
object { Stack texture{T07} interior{I_Glass} translate < 0.0, 0, 0> }
object { Stack texture{T08} interior{I_Glass} translate < 4.5, 0, 0> }
object { Stack texture{T09} interior{I_Glass} translate < 9.0, 0, 0> }

object { Stack texture{T10} interior{I_Glass} translate <-6.25, -6, 0> }
object { Stack texture{T11} interior{I_Glass} translate <-2.5,  -6, 0> }
object { Stack texture{T12} interior{I_Glass} translate < 2.5,  -6, 0> }
object { Stack texture{T13} interior{I_Glass} translate < 6.25, -6, 0> }
