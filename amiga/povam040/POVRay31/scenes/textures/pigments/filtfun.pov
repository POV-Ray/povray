// Persistence Of Vision raytracer version 3.1 sample file.
// Fun with filter (and other neat tricks).

global_settings { assumed_gamma 2.2 }

#include "shapes.inc"
#include "colors.inc"

camera {
   location  <0, 2, -4.5>
   direction <0, 0,  1>
   up  <0, 1, 0>
   right <4/3, 0, 0>
   look_at <0, 0.4, 0>
}

// Floor, with phoney gray "planks"
plane { y, 0

   pigment {
      gradient x
      color_map {
         [0,    0.25 color Gray      color Gray]
         [0.25, 0.50 color DimGray   color LightGray]
         [0.50, 0.75 color LightGray color Gray]
         [0.75, 1    color Gray      color Gray]
      }
      scale <0.45, 1, 1>
   }
   finish{ambient 0.1 diffuse 0.7}
}

//  Note: Clear = color White filter 1

// A blobby sphere
sphere  { <0, 1, -1.5>, 1
    pigment {
      bozo
      turbulence 0.5
      octaves 1
      scale 0.2
      color_map {
         [0,   0.5 color red 1 filter 0.5 color red 1 filter 1]
         [0.5, 1   color Clear           color Clear]
      }
   }
   finish {ambient 0.15  diffuse 0.7}
}

// A sliced green box
object { UnitBox
   rotate 45*y
   translate <-4, 1, 4>

   pigment {
      gradient y
      color_map {
         [0,   0.5 color Green color Green]
         [0.5, 1   color Clear color Clear]
      }
      scale 0.5
   }
}

// A yellow, swirly finite cylinder
object { Disk_Y
   translate <4, 1, 4>

   pigment {
      gradient y
      turbulence 2
      octaves 1
      color_map {
         [0,   0.5 color Yellow color Yellow]
         [0.5, 1   color Clear  color Clear]
      }
      scale 0.5
   }
}

light_source { <10, 12, -40> colour White }
