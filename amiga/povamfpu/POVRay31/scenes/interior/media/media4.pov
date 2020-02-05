// Persistence Of Vision raytracer version 3.1 sample scene by Chris Young.
//
// This scene shows the effect of multiple density statements.
//

#include "colors.inc"

camera {location  <0, 0, -100>}

plane { z, 50
   pigment {checker Yellow, White scale 20}
   finish {ambient 0.2  diffuse 0.8}
   hollow
}

// Declare 2 similar density.  Only the color map differs.

#declare Density1=
  density {
    spherical
    ramp_wave
    color_map {
      [0.0 color rgb <0.0, 0.0, 0.0>]
      [0.2 color rgb <1.0, 0.3, 0.1>]
      [1.0 color rgb <1.0, 1.0, 0.1>]
    } 
  }

#declare Density2=
  density {
    spherical
    ramp_wave
    color_map {
      [0.0 color rgb <0.0, 0.0, 0.0>]
      [0.2 color rgb <0.1, 1.0, 0.3>]
      [1.0 color rgb <0.1, 1.0, 1.0>]
    } 
  }

// Sphere on the left contains 2 density but one media
// Results are the colors multiply
sphere { 0, 1.5
  pigment { color rgbf<1, 1, 1, 1> }
  finish { ambient 0 diffuse 0 }
  interior { 
    media {
      emission 0.05
      intervals 5
      samples 1, 10
      confidence 0.9999
      variance 1/1000
      density {
        Density1
        translate -y/3
      }
      density {
        Density2
        translate y/3
      }
    } 
  }
  translate -1.25*x
  scale 24 
  hollow 
}

// Sphere on the right contains 2 media, each with 1 density
// Results are the colors add.
sphere { 0, 1.5
  pigment { color rgbf<1, 1, 1, 1> }
  finish { ambient 0 diffuse 0 }
  interior { 
    media {
      emission 0.05
      intervals 5
      samples 1, 10
      confidence 0.9999
      variance 1/1000
      density {
        Density1
        translate -y/3
      }
    }
    media {
      emission 0.05
      intervals 5
      samples 1, 10
      confidence 0.9999
      variance 1/1000
      density {
        Density2
        translate y/3
      }
    } 
  }
  translate 1.25*x
  scale 24 
  hollow 
}

light_source {
  <500, 600, -500> 
  color White
}

