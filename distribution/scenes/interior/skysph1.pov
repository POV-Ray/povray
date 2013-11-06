// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Dieter Bayer.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

global_settings {
  assumed_gamma 1.0
}

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "skies.inc"

camera {
  location <0, 2.5, -4>
  right   x*image_width/image_height
  up <0, 1, 0>
  angle 65 // direction <0, 0, 1>
  look_at <0, 4, 0>
}

light_source { <5, 10, -60> White }

#declare Sky = sky_sphere {
  pigment {
    gradient y
    color_map {
      [0.5 color SkyBlue]
      [0.8 color MidnightBlue]
    }
  }
}

sky_sphere { Sky }

plane { y, 0
  pigment { color Green }
  finish { ambient .3 diffuse .7 }
}


