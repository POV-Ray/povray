// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dieter Bayer.


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "skies.inc"

camera {
  location <0, 2.5, -4>
  right <4/3, 0, 0>
  up <0, 1, 0>
  direction <0, 0, 1>
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


