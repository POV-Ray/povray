// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Image map example
// File by Alexander Enzmann
// NOTE: Requires "test.png"
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"

#declare Bi = 2;

#declare Texture0 = /* Planar image map */
texture {pigment{image_map { png "test.png" map_type 0 once interpolate Bi } } }

#declare Texture1 = /* Spherical image map */
texture {pigment{image_map { png "test.png" map_type 1 interpolate Bi } } }

sphere { <0, 0, 0>, 1
   texture { Texture1 }
   scale 10
   rotate -90*y
   translate <-12, 0, 20>
}

plane {
   z, 0
   hollow on
   clipped_by {box { <0, 0, -1>, <1, 1, 1> } }
   texture { Texture0 }
   translate <-0.5, -0.5, 0>
   scale 20
   rotate <20, 30, 0>
   translate <12, 0, 20>
}

camera {
   location  <0, 0, -20>
   angle 65 
   right   x*image_width/image_height
   look_at <0, 0, 0>
}

light_source { <0, 300, -200> colour White }
