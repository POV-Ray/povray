// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: norm_acc.pov
// Desc: normal accuracy sample
// Date: April 2001
// Auth: Christoph Hormann

// -w512 -h384
// -w640 -h480 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 1.0
  max_trace_level 5
}

#include "colors.inc"
#include "woods.inc"

light_source {
  <1.5, 0.3, 1.8>*10000
  color rgb <0.9, 0.9, 1.0>
}

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
  location    <22, 38, 24>
  direction   y
  sky         z
  up          z
  right       x*image_width/image_height
  look_at     < 1, 0, 0>
  angle       30
}



sphere {
  <0, 0, 0>, 1
  texture {
    T_Wood35
    finish {
      diffuse 0
      ambient 1
    }
    scale 0.3
  }
  scale 100000
  no_shadow
  hollow on
}

#declare Rad=0.2;
#declare S_X=12;
#declare S_Y=4.2;
#declare S_XM=S_X-Rad;
#declare S_YM=S_Y-Rad;

union {
  box { <-S_XM, -S_Y, -2.5>, <S_XM, S_Y, -3.0> }
  box { <-S_X, -S_YM, -2.5>, <S_X, S_YM, -3.0> }
  cylinder { <-S_XM, -S_YM, -2.5>, < S_XM, -S_YM, -2.5>, Rad }
  cylinder { <-S_XM, -S_YM, -2.5>, <-S_XM,  S_YM, -2.5>, Rad }
  cylinder { < S_XM,  S_YM, -2.5>, <-S_XM,  S_YM, -2.5>, Rad }
  cylinder { < S_XM,  S_YM, -2.5>, < S_XM, -S_YM, -2.5>, Rad }

  cylinder { <-S_XM, -S_YM, -2.5>, <-S_XM, -S_YM, -3.0>, Rad }
  cylinder { <-S_XM,  S_YM, -2.5>, <-S_XM,  S_YM, -3.0>, Rad }
  cylinder { < S_XM, -S_YM, -2.5>, < S_XM, -S_YM, -3.0>, Rad }
  cylinder { < S_XM,  S_YM, -2.5>, < S_XM,  S_YM, -3.0>, Rad }

  sphere { <-S_XM, -S_YM, -2.5>, Rad }
  sphere { <-S_XM,  S_YM, -2.5>, Rad }
  sphere { < S_XM, -S_YM, -2.5>, Rad }
  sphere { < S_XM,  S_YM, -2.5>, Rad }

  texture {
    pigment { color NeonBlue }
    finish {
      diffuse 0.0
      ambient 0.0
      specular 0.3
      roughness 0.07
      reflection {
        0.1
        metallic
      }
    }
  }
}


#macro Obj1(Acc)
union {
  sphere { <0, 0, 3>, 2 }
  box { <-2.2, -2.2, -2.2>, <2.2, 2.2, 1> }

  union {
    text
    {
      ttf
      "crystal.ttf",
      concat("accuracy ", str(Acc, 0, 3)),
      0.1, 0

      texture { pigment { color MediumAquamarine } }

      scale 0.7
      rotate 180*z
      rotate -40*x
    }

    intersection {
      box { <0.2, 0.15, 1>, <-5.3, -0.5, -1.4> }
      plane {
        z, 0
        rotate -40*x
      }
      texture {
        pigment {
          agate
          color_map {
            [0.3 color Blue*0.2 ]
            [0.5 color Blue ]
          }
          scale 0.3
        }
        finish {
          ambient 0.0
          diffuse 0.35
          specular 0.5
        }
      }
    }

    translate <2.6, 3.4, -1.8>
  }

  texture {
    pigment { color rgb 1 }
    normal {
      granite 0.7
      turbulence 0.2
      scale 1.2
      accuracy Acc
    }
    finish { diffuse 0.65 ambient 0.015 }
  }

}
#end


object {
  Obj1(0.001)
  translate <8, 0, 0>
}

object {
  Obj1(0.02)
  translate <0, 0, 0>
}

object {
  Obj1(0.1)
  translate <-8, 0, 0>
}

