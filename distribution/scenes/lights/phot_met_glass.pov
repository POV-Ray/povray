// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File: phot_met_glass.pov
// Desc: metal, glass and photons sample
// Date: August 2001
// Auth: Christoph Hormann

// -w320 -h160
// -w512 -h256 +a0.3

#version 3.6;

#include "colors.inc"
#include "glass.inc"

global_settings {
  assumed_gamma 1.0
  max_trace_level 25
  photons {
    spacing 0.03
    autostop 0
    jitter 0
  }
}

camera {
  location    <20, 5, 10>
  right       2*x
  look_at     <0,-1,0>
  angle       34
}

light_source {
  <-3, 10, 18>
  color rgb <1.3, 1.2, 1.1>

  photons {
    reflection on
    refraction on
  }
}

plane {
  y, -1
  texture {
    pigment {
      color rgb 1.0
    }
  }
}

sphere {
  0, 1

  texture {
    pigment {
      gradient y
      color_map {
        [0.0 color rgb 1.0 ]
        [0.3 color rgb <0.6, 0.65, 1.0> ]
      }
    }
    finish { ambient 1 diffuse 0 }
  }

  scale 1000 
  hollow on
}

#declare Metal_Texture =
texture {
  pigment { color rgb <0.5, 0.5, 0.6> }
  finish {
    ambient 0.0
    diffuse 0.15
    specular 0.3
    roughness 0.01
    reflection {
      0.8
      metallic
    }
  }
}

#declare Box = box { -1, 1 }

#declare Metal_Obj_1 =
difference {
  object {
    Box
  }
  object {
    Box
    scale <0.8, 0.8, 1.5>
  }
  object {
    Box
    scale <0.8, 1.5, 0.8>
  }
  object {
    Box
    scale <1.5, 0.8, 0.8>
  }

  texture { Metal_Texture }

  rotate 12*y
}

#declare Metal_Obj_2 =
difference {
  cylinder { -1.0*y, 0.0*y, 6.0 }
  cylinder { -2.0*y, 1.0*y, 5.8 }

  texture { Metal_Texture }
}

union {
  object { Metal_Obj_1 }
  object { Metal_Obj_2 }

  sphere {
    <0,0,0>, 1

    material {
      texture {
        pigment { color Col_Glass_Clear }
        finish { F_Glass6 }
      }
      interior {
        I_Glass_Exp(2)
        fade_color Col_Red_03
      }
    }
  }

  photons {
    target
    reflection on
    refraction on
    collect off
  }
}


