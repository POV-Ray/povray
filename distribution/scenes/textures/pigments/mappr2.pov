// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Image_map demonstration, shows various types of mapping
// File by Alexander Enzmann
// NOTE: Requires "test.png"
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"
#include "shapes.inc"

#declare Bi = 2.0;

#declare Texture2 = /* Cylindrical image map */
texture {pigment{image_map {png "test.png" map_type 2 once interpolate Bi } } }

#declare Texture5 = /* Torus image map */
texture {pigment{image_map {png "test.png" map_type 5 interpolate Bi } } }

cylinder { <0,0,0>, y, 1
   open
   texture { Texture2 }
   translate <0, -0.5, 0>
   scale <7, 14, 7>
   rotate <40, -60, 0>
   translate <-12, 11, 20>
}

object { Hyperboloid_Y
   translate 1*y scale <1, 0.5, 1>
   texture { Texture2 }
   scale <1, 2, 1> translate <0, -1, 0>
   clipped_by {box{<-2,-1,-2>,<2,1,2>} } //bounded_by{clipped_by}
   scale <5, 7, 5>
   rotate <-40, -90, 0>
   translate <-12, -11, 20>
}

/* Torus having major radius = 6.4, minor radius = 3.5 */

torus { 6.4, 3.5
   texture { Texture5 }
   rotate -90*y
   rotate -20*x
   translate <12, 11, 20>
}

object { Paraboloid_Y
   texture { Texture2 }
   clipped_by{box{<-2,0,-2>,<2,1,2>} } //bounded_by{clipped_by}
   translate <0, -0.5, 0>
   scale <8, 16, 8>
   rotate <-40, 0, 0>
   translate <12, -11, 12>
}

camera {
   location  <0, 0, -25>
   angle 65 
   right   x*image_width/image_height
   look_at <0, 0, 0>
}

light_source {<0, 600, -400> colour White}
