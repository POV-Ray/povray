// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// A synthetic photograph by Dan Farmer
//---------------------------------------------------------------------------
// This scene file was designed to emulate the digitized photographic image
// of a crystal sphere { on a checkerboard that David Buck took, and to
// verify or refute the correctness of the current refractive functions
// in POV-Ray.  The original image is available on CompuServe
// (GO GRAPHDEV), by the name of crysta.gif.
//---------------------------------------------------------------------------
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"

global_settings {
  assumed_gamma 1
  max_trace_level 5
}

camera {
   location <-0.85, 12.5, -28>
   angle 15 // direction <0, 0, 4.125>
   up  <0, 1, 0>
   right   x*image_width/image_height
   look_at <0.25, 0.15, 0>
}

light_source { <-5, 50, -5> colour red 0.85 green 0.85 blue 0.85 }
light_source { <-500, 500, -500> colour DimGray }
// light (under checkerboard, for background)
light_source { <10, -50, 5> colour White }

// The background.  Designed to give the shaded quality of the photo
sphere { <0, 0, 0>, 1
   hollow
   scale <10000, 500, 500>
   rotate 60*y

   finish {
      ambient 0.2
      diffuse 0.75
      crand 0.025
   }
   pigment { color Gray }
}

union {
   object { Cube
      scale <5, 0.001, 7>

      pigment {
         checker color Black color White
         translate <1, 0, 7>
      }
      finish {
         ambient 0.35
         diffuse 0.65
         crand 0.015
      }
   }

   sphere { <-0.25, 2.15,-4.25>, 2.15
      pigment { White filter 0.95 }
      interior{
         ior 1.45
         fade_distance 2
         fade_power 2
         caustics 2.0
      }
      finish {
         ambient 0.2
         diffuse 0.0
         reflection 0.12
         specular 1.0
         roughness 0.001
      }
   }

   rotate -6*z       /* Simulate the slight camera tilt in the photo */
}
