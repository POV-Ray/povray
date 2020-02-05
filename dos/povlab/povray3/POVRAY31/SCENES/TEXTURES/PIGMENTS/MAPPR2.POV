// Persistence Of Vision raytracer version 3.1 sample file.
// Image_map demonstration, shows various types of mapping
// File by Alexander Enzmann
// NOTE: Requires "test.png"


global_settings { assumed_gamma 2.2 }

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
   translate <-12, 15, 20>
}

object { Hyperboloid_Y
   translate 1*y scale <1, 0.5, 1>
   texture { Texture2 }
   scale <1, 2, 1> translate <0, -1, 0>
   clipped_by {box{<-2,-1,-2>,<2,1,2>} } //bounded_by{clipped_by}
   scale <5, 7, 5>
   rotate <-40, -90, 0>
   translate <-12, -15, 20>
}

/* Torus having major radius = 6.4, minor radius = 3.5 */

torus { 6.4, 3.5
   texture { Texture5 }
   rotate -90*y
   rotate -20*x
   translate <12, 15, 20>
}

object { Paraboloid_Y
   texture { Texture2 }
   clipped_by{box{<-2,0,-2>,<2,1,2>} } //bounded_by{clipped_by}
   translate <0, -0.5, 0>
   scale <8, 16, 8>
   rotate <-40, 0, 0>
   translate <12, -15, 20>
}

camera {
   location  <0, 0, -90>
   direction <0, 0,   1>
   up        <0, 0.5, 0>
   right     <0.6666, 0,  0>
}

light_source {<0, 300, -200> colour White}
