// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Imagemap interpolation example
// File by Drew Wells
// NOTE: Requires "test.png"
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"

#declare Bilinear = 2.0;
#declare Norm_Dist = 4.0;
#declare TestMap = "test.png"             // use your own, if you wish

camera {
   location <0.0, 0.0, -2.0>
   angle 65 
   right   x*image_width/image_height
   look_at <0, 0, 0>
}

light_source { <100.0, 120.0, -130.0> colour White }

#default {
   finish {
      ambient 0.2
      diffuse 0.8
      specular 0.3
      roughness 0.002
      brilliance 2
   }
}

// Left hemisphere: no interpolation
union {
    sphere { 0, 1.0
       clipped_by { plane { x, 0 } }
       pigment {
          image_map {
            png TestMap gamma 2.2
            map_type 1                    // spherical map type
            interpolate 2
          }
       }
       translate -0.035*x                 // move hemi-sphere left
    }

    // Right hemisphere: try both Binlinear (1) and Norm_Dist (2)
    sphere { 0, 1.0
       clipped_by { plane { -x, 0 } }
       pigment {
          image_map {
            png TestMap gamma 2.2
            map_type 1                    // spherical map type
            interpolate Norm_Dist
          }
       }
       translate 0.035*x                  // move hemi-sphere right
    }
    rotate <45, 0, 0>
}


