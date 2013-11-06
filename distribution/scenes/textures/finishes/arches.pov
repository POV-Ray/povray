// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// By Dan Farmer
// Parabolic arches on the water.  Is this the St. Louis McDonalds?
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

global_settings { 
  assumed_gamma 2.2
  max_trace_level 5
}

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "metals.inc"
#include "skies.inc"

camera {
   location <60.0, 0.0, -135.0>
   angle 40 
   right     x*image_width/image_height
   look_at <0.0, 0.0, 0.0>
}

// Light
light_source {<200.0, 200.0, -150.0> colour red 1 green .5 }

#declare New_Sky = sky_sphere { S_Cloud2 }
#declare Old_Sky =
sky_sphere {
   pigment {
      gradient y
      colour_map {
         [0.0 0.8  colour red 0.5 green 0.1 blue 0.7
                   colour red 0.1 green 0.1 blue 0.9]
         [0.8 1.0  colour red 0.1 green 0.1 blue 0.9
                   colour red 0.1 green 0.1 blue 0.9]
      }
   }
   pigment {
      bozo
      turbulence 0.7
      colour_map {
         [0.0 0.6  colour red 1.0 green 1.0 blue 1.0 filter 1.0
                   colour red 1.0 green 1.0 blue 1.0 filter 1.0]
         [0.6 0.8  colour red 1.0 green 1.0 blue 1.0 filter 1.0
                   colour red 1.0 green 1.0 blue 1.0]
         [0.8 1.001 colour red 1.0 green 1.0 blue 1.0
                    colour red 0.8 green 0.8 blue 0.8]
      }
      scale <0.5, 0.2, 0.2>
   }
}

sky_sphere { New_Sky }                      // changed by dmf '95

// Define the ocean surface
plane { y, -10.0
   texture {
      T_Chrome_2D
      normal {
         waves 0.05
         frequency 5000.0
         scale 3000.0
      }
   }
}


// Create the arches using CSG difference between two "squashed" paraboloids
difference {
   object { Paraboloid_Y
      scale <20.0, 20.0, 5.0>
      rotate 180*x
      texture { T_Chrome_3C }
   }
   object { Paraboloid_Y
      scale <18.0, 20.0, 18.0>
      rotate 180*x
      translate -2*y
      texture { T_Copper_3C }
   }
   translate <0.0, 30.0, -25.0>
}

difference {
   object { Paraboloid_Y
      scale <20.0, 20.0, 5.0>
      rotate 180*x
      texture { T_Chrome_3C }
   }
   object { Paraboloid_Y
      scale <18.0, 20.0, 18.0>
      rotate 180*x
      translate -2*y
      texture { T_Copper_3C }
   }
   translate <0.0, 30.0, 50.0>
}
