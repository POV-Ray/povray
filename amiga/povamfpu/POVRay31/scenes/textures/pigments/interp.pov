// Persistence Of Vision raytracer version 3.1 sample file.
// Imagemap interpolation example
// File by Drew Wells
// NOTE: Requires "test.png"

global_settings { assumed_gamma 2.2 }

#include "colors.inc"


#declare Bilinear = 2.0;
#declare Norm_Dist = 4.0;
#declare TestMap = "test.png"             // use your own, if you wish

camera {
   location <0.0, 0.0, -2.0>
   direction <0.0, 0.0, 1.0>
   up  <0.0, 1.0, 0.0>
   right <4/3, 0.0, 0.0>
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
            png TestMap
            map_type 1                    // spherical map type
            interpolate 0
          }
       }
       translate -0.035*x                 // move hemi-sphere left
    }

    // Right hemisphere: try both Binlinear (1) and Norm_Dist (2)
    sphere { 0, 1.0
       clipped_by { plane { -x, 0 } }
       pigment {
          image_map {
            png TestMap
            map_type 1                    // spherical map type
            interpolate Norm_Dist
          }
       }
       translate 0.035*x                  // move hemi-sphere right
    }
    rotate <45, 0, 0>
}


