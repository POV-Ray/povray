// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Fun with filter (and other neat tricks).
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0 max_trace_level 5}

#include "shapes.inc"
#include "colors.inc"

camera {
   location  <0, 3, -4.5>
   angle 65
   right   x*image_width/image_height
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
sphere  { <0.25, 1, -1.8>, 1
    pigment {
      bozo
      turbulence 0.5
      octaves 1
      scale 0.2
      color_map {
         [0.0  color rgbf<1,0.1,0,0.5>]
         [0.2  color rgbf<1,0.1,0,0.5>]
         [0.6  color rgbf<1,0.8,0,1> ]
         [0.6  color Clear]
         [1.0  color Clear]
      }
   }
   finish {ambient 0.15  diffuse 0.7}
}

// A sliced green box
object { UnitBox
   rotate 45*y
   translate <-3.0, 1, 3>

   pigment {
      gradient y
      color_map {
         [0,   0.5 color rgb<0.1,0.6,0> color rgb<0.1,0.6,0> ]
         [0.5, 1   color Clear color Clear]
      }
      scale 0.5
   }
}

// A yellow, swirly finite cylinder
object { Disk_Y
   translate <3, 1, 4>

   pigment {
      gradient y
      turbulence 2
      octaves 1
      color_map {
         [0,   0.5 color rgb<1,0.7,0> color  rgb<1,0.7,0> ]
         [0.5, 1   color Clear  color Clear]
      }
      scale 0.5
   }
}

light_source { <20, 12, -40> colour White }
