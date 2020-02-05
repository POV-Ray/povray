// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dieter Bayer.

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"

camera {
  location <0, 2.5, -4>
  right <4/3, 0, 0>
  up <0, 1, 0>
  direction <0, 0, 1>
  look_at <0, 4, 0>
}

background { color SkyBlue }

#declare Sky = sky_sphere {
  pigment {
    gradient y
    color_map {
      [0.75  color CornflowerBlue]
      [1.00  color MidnightBlue]
    }
    scale 2
    translate <-1, -1, -1>
  }
  pigment {
    bozo
    turbulence 0.6
    octaves 7
    omega .49876
    lambda 2.5432
    color_map {
      [0.0 color rgbf<.75, .75, .75, 0.1>]
      [0.4 color rgbf<.9, .9, .9, .9>]
      [0.7 color rgbf<1, 1, 1, 1>]
    }
    scale 6/10
    scale <1, 0.3, 0.3>
  }
  pigment {
    bozo
    turbulence 0.6
    octaves 8
    omega .5123
    lambda 2.56578
    color_map {
      [0.0 color rgbf<.375, .375, .375, 0.2>]
      [0.4 color rgbf<.45, .45, .45, .9>]
      [0.6 color rgbf<0.5, 0.5, 0.5, 1>]
    }
    scale 6/10
    scale <1, 0.3, 0.3>
  }
}

sky_sphere { Sky }

plane { y, 0
  pigment { color Green }
  finish { ambient .3 diffuse .7 }
}


