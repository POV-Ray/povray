// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: var_reflection.pov
// Desc: variable reflection demonstration scene
// Date: August 2001
// Auth: Christoph Hormann

// -w320 -h240
// -w512 -h384 +a0.3

// reflection samples:
// -------------------
// 0: constant reflection 0.5
// 1: variable reflection 0...1
// 2: variable reflection 0...1 fresnel formula
// 3: variable reflection 1...0 (inverse) fresnel formula
// 4: variable reflection fresnel formula, changed falloff
// 5: metallic reflection
// 6: colored reflection
// -------------------

#version 3.6;

global_settings {
  assumed_gamma 1.0
  max_trace_level 15
}

#include "colors.inc"

light_source {
  <1.5, -2.5, 2.5>*10000
  color rgb 1.0
}

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
  location    <0, -22, 6>
  direction   y
  sky         z
  up          z
  right     x*image_width/image_height
  look_at     <0, 0, 2>
  angle       44
}

sky_sphere {
  pigment {
    color rgb <0.55,0.65,1.0>
  }
  /*
  pigment {
    agate
    color_map {
      [0.6 color rgb 0.8 ]
      [0.6 color rgb 0.5 ]
    }
    scale 0.06
  }
  */
}

// ----------------------------------------

plane
{
  z, 0
  texture
  {
    pigment {
      checker
      color rgb 1
      color rgb 0.5

      scale <1.8,10000,1.0>
      translate -10
      rotate 20*z
    }
    finish {
      diffuse 0.7
    }
  }
}

// ----------------------------------------

#declare Cnt=0;

#while (Cnt < 7)

  text
  {
    ttf
    "crystal.ttf",
    str(Cnt,0,0),
    0.1,
    0
    scale 1.5
    rotate 90*x

    translate 0.3*z
    translate 2*y

    translate (-7.5+Cnt*2.5)*x

    texture {
      pigment { color rgb x }
    }

    no_reflection
    no_shadow
  }

  #declare Mat=
    material {
      texture {
        pigment { color rgbt <0, 1, 0, 1> }
        finish {
          ambient 0
          diffuse 0

          #switch (Cnt)
            #case (0)
              reflection 0.5
              #break
            #case (1)
              reflection {
                0, 1
              }
              #break
            #case (2)
              reflection {
                0, 1
                fresnel on
              }
              #break
            #case (3)
              reflection {
                1, 0
                fresnel on
              }
              #break
            #case (4)
              reflection {
                0, 1
                falloff 2
              }
              #break
            #case (5)
              reflection {
                0.8
                metallic
              }
              #break
            #case (6)
              reflection {
                <0, 1, 0>, <0, 0, 1>
                fresnel on
              }
              #break
          #end

          conserve_energy
        }
      }
      interior {
        ior 1.3
      }
    }


  box {
    <-1, -5, 0.25>, <1, 20000, 0.25>

    translate (-7.5+Cnt*2.5)*x

    material { Mat }
  }

  sphere {
    <0, 0, 0.5>, 1.1

    translate (-7.5+Cnt*2.5)*x

    scale 0.7

    translate -9*y

    material { Mat }
  }

  #declare Cnt=Cnt+1;
#end
