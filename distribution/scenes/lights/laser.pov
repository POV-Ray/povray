// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// A couple of tricks with spotlights and wood texture here.
// File by Dan Farmer.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 1.0
  max_trace_level 5
}

#include "colors.inc"
#include "shapes.inc"

camera {
   location  <-8, 3, -14>
   angle 65 // direction <0, 0, 1>
   up        <0, 1, 0>
   right     x*image_width/image_height
   look_at   <0, 0, 0>
}

// Overhead spotlight, shining "backwards"
light_source {
   <0, 50, -1> color LightGray
   spotlight
   point_at <0, 0, 8>
   tightness 50
   radius 50
   falloff 100
}

// Ground plane
plane { y, -1
   pigment {White}
   finish {
      ambient 0.3
      diffuse 0.7
      specular 0.5  roughness 0.05
   }
}

// Three spotlights positioned in front of three cylinders.  These could
// be put into composites if you wanted to really do it right.  Each light
// is associated with a cylinder.
//----------
// Red spotlight, goes with  left cylinder
light_source {
   <-3, -0.5, -2>
   color Red
   spotlight
   point_at <-3, -1, -10>
   tightness 10
   radius 100
   falloff 250
}

// Green spotlight, goes with center cylinder
light_source {
   <0, -0.5, -2>
   color Green
   spotlight
   point_at <0, -1, -10>
   tightness 10
   radius 100
   falloff 250
}

// Blue spotlight, goes with right cylinder
light_source {
   <3, -0.5, -2> color Blue
   spotlight
   point_at <3, -1, -10>
   tightness 10
   radius 100
   falloff 250
}

// Set default textures for shapes to come
default {
   finish {
      ambient 0.5     // Unusually high ambient setting.
      diffuse 0.5     // Unusually low diffuse setting.
      reflection 0.15
      specular 0.25 roughness 0.001
   }
}

#declare L_Interior =
   interior{
      fade_distance 6
      fade_power 2
   }


// Red cylinder on the left.  Goes with red spotlight.
object { Disk_Z
   interior{L_Interior}
   pigment {
      wood
      turbulence 0  // I want concentric rings,  not wood.
      // colormap from opaque red to "clear red"
      color_map {[0, 1  color Red filter 0 color Red filter 1] }
      scale <2, 2, 1>
   }

   scale <1, 1, 6>        // Scale texture with the object now.
   translate <-3, 0, 4>   // Move it to its final restingplace
}

// Green cylinder in the center.  Goes with green spotlight.
object { Disk_Z
   interior{L_Interior}
   pigment {
      wood
      turbulence 0  // I want concentric rings,  not wood.
      // colormap from opaque green to "clear green"
      color_map {[0, 1  color Green filter 0 color Green filter 1] }
      scale <2, 2, 1>
   }

   scale <1, 1, 6>
   translate <0, 0, 4>
}

// Blue cylinder on the right.  Goes with blue spotlight, right?
object { Disk_Z
   interior{L_Interior}
   pigment {
      wood
      turbulence 0  // I want concentric rings,  not wood.
      // colormap from opaque blue to "clear blue"
      color_map {[0, 1  color Blue filter 0 color Blue filter 1] }
      scale <2, 2, 1>
   }

   scale <1, 1, 6>
   translate <3, 0, 4>
}

