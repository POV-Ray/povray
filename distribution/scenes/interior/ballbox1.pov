// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Dan Farmer
// Demonstrates glass textures, CGS with box primitives, one of Mike Miller's
// fabulous marble textures, modified with an "octaves" change, and doesn't
// make a half-bad image, either.  Interesting lighting effect, too.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 2.0
  max_trace_level 5
  }
  
#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "glass.inc"
#include "stones.inc"

camera {
   location  <0.75, 3.5, -3.5>
   angle 100 // direction <0.0,  0.0,  0.5>       //  "wide-angle" view
   right   x*image_width/image_height
   look_at   <0,    0,   -1>}

// Light sources, two to the front, right, on from the left, rear.
light_source {<-30, 11,  20> color White}
light_source {< 31, 12, -20> color White}
light_source {< 32, 11, -20> color LightGray}

union {
   // A green glass ball inside of a box-shaped frame
   sphere { <0, 0, 0>, 1.75
      // converted to material 07Aug2008 (jh)
      material {
        texture {
         pigment {color green 0.90 filter 0.85}
         finish {
            phong 1 phong_size 300         // Very tight highlights
            reflection 0.15                // Needs a little reflection added
            }
         }
       interior{
         caustics 1.0
         ior 1.5
         }
       }
   }

   // A box-shaped frame surrounding a green glass ball
   difference {
      object {UnitBox scale 1.5}     // The outside dimensions

      // And some square "holes" in all sides.  Note that each of
      // these boxes that are going to be subtracted has one vector
      // scaled just slightly larger than the outside box.  The other
      // two vectors determine the size of the hole.
      // Clip some sqr holes in the box to make a 3D box frame
      object{UnitBox scale <1.51, 1.25, 1.25> }   // "clip" x
      object{UnitBox scale <1.25, 1.51, 1.25> }   // "clip" y
      object{UnitBox scale <1.25, 1.25, 1.51> }   // "clip" z

      pigment { red 0.75 green 0.75 blue 0.85 }
      finish {
         ambient 0.2
         diffuse 0.7
         reflection 0.15
         brilliance 8
         specular 1
         roughness 0.01
      }

      // Same as radius of glass sphere, not the box!
      bounded_by {object {UnitBox scale 1.75}}
   }
   rotate y*45
}

plane { y, -1.5
   texture {
      T_Stone1
      pigment {
         octaves 3
         rotate 90*z
      }
      finish { reflection 0.10 }
   }
}
// end of file
