// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: GlassChess.pov
// Desc: Three glass chesspieces.
// Date: 1999-06-22
// Updated: 2001-07-27
// Auth: Ingo Janssen
//
// -w320 -h240
// -w800 -h600 +a0.3
//
// with +w400 +h300 +a0.3
// on a PII, 233 MHz
// it takes 26 min to render
// so with the current GHz monsters ...

#version 3.6;

#include "chesspiece1.inc"

global_settings {
  assumed_gamma 1.0
  max_trace_level 15
}

light_source {
   < 500, 50,-5>
   rgb 1
}

camera {
   location  < 5.5, 1,-7.0>
   right x*image_width/image_height
   look_at   < 0.0, 1, 0.0>
   angle 60
}

// Textures and materials
#declare Glass = texture {
   pigment {rgbf < 0.98, 1.0, 0.98, 1> }
   finish {
      specular 1
      roughness 0.001
      ambient 0
      diffuse 0
      reflection {
         0,1
         fresnel
      }
   }
}

#declare PieceTex = texture {
   pigment {rgbt < 0.9, 0.9, 0.6, 0.6>}
   finish {reflection 0.3}
}

#declare SkyGlass = material {
   texture {
      pigment {rgbt < 0.95, 0.95, 1, 1> }
      finish {
         diffuse 0.2
         ambient 0.05
         specular 1.5
         roughness 0.01
         brilliance 0.01
         reflection {
            0,1
            fresnel
         }
      }
      normal {             // from a water by Jon S. Brandt.
          bozo
          normal_map {
              [ 0.30 waves   translate -0.5 scale <1, 0.05, 1>*100000 frequency 100000]
              [ 0.70 ripples translate -0.5 scale <1, 0.70, 1>*100000 frequency 100000]
              [ 0.85 ripples translate -0.5 scale <1, 0.60, 1>*100000 frequency 100000]
              [ 1.00 ripples translate -0.5 scale 100000 frequency 100000]
          }
          scale 0.6
      }
   }
   interior {ior 1.33}
}

// start sky
sky_sphere {
   pigment {
      gradient y
      color_map {
         [0.0 color rgb < 0.3, 0.05, 0.15>]
         [1.0 color rgb 1]
      }
      translate < 0,-0.1, 0>
   }
}

sphere {
   0, 5000
   scale < 1, 0.3, 0.6>
   hollow
   pigment {
      bozo
      turbulence 0.1
      color_map {
         [ 0.0 color rgbt 1]
         [ 0.4 color rgbt 0.8]
         [ 1.0 color rgb 1]
      }
      scale < 250, 500, 250>
   }
   finish {
      ambient 0.7
      diffuse 0
   }
   rotate < 5, 0, 0>
}

sphere {
   0, 4000
   scale < 1, 0.1, 0.6>
   hollow
   pigment{
      bozo
      turbulence 0.1
      color_map {
         [ 0.0 color rgbt 1 ]
         [ 0.3 color rgbt 0.8 ]
         [ 1.0 color rgb 1 ]
      }
      scale < 250, 500, 250>
   }
   finish {
      ambient 0.4
      diffuse 0
   }
   rotate < 5, 0, 0>
}

box {
   <-10, 3.5,-7>, < 10, 3.51, 7>
   no_shadow
   material {SkyGlass}
}
// end sky

// chessboard
height_field {
   function 256, 256 {
      pigment {
         checker
         color rgb 1
         color rgb 0
         scale 1/8
      }
   }
   smooth
   translate <-0.5, 0.0,-0.5>
   scale < 12,-0.5, 12>
   rotate < 0, 270, 0>
   texture {Glass}
   interior {ior 1.2}
}

union {
   difference {
      box {<-7,-0.25,-7>, < 7,-0.001, 7>}
      box {<-6,-0.30,-6>, < 6, 0, 6>}
      pigment {rgb < 0, 0, 0.5>}
   }
   box {
      <-6,-0.2,-6>, < 6,-0.501, 6>
      texture {
         pigment {
            checker color rgb 1 color rgb < 0, 0, 1>
            scale 1.5
            translate < 1.5, 0, 0>
         }
      }
   }
   no_shadow
}
// end chessboard


// The pieces
difference {
   superellipsoid {
      < 0.05, 0.05>
      translate < 0, 1, 0>
      scale < 0.5, 0.9, 0.5>
      pigment {rgbf 0.9}
      texture {Glass}
      interior {
         ior 1.5
         fade_distance 2
         fade_power 2
      }
   }
   object {
      Paard
      rotate < 0,-90, 0>
      texture{PieceTex}
   }
   no_shadow
   rotate < 0, 68, 0>
   translate < 2.2*1.5, 0, -3.5*1.5>
}

difference {
   superellipsoid {
      < 0.05, 0.05>
      translate < 0, 1, 0>
      scale < 0.5, 0.9, 0.5>
      texture {Glass}
      interior {
         ior 1.5
         fade_distance 2
         fade_power 2
      }
   }
   object {
      Loper
      texture{PieceTex}
   }
   no_shadow
   translate < 1.5*1.5, 0,-1.5>
}

difference {
   superellipsoid {
      < 0.05, 0.05>
      translate < 0, 1, 0>
      scale < 0.5, 0.75, 0.5>
      texture {Glass}
      interior {
         ior 1.5
         fade_distance 2
         fade_power 2
      }
   }
   object {
      Toren
      texture{PieceTex}
   }
   no_shadow
   translate < 3.5*1.5, 0,-3.5*1.5>
}
// end pieces
