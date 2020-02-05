// Persistence Of Vision raytracer version 3.1 sample file.
// Spotlight example
// File by Alexander Enzmann & Drew Wells

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"


camera {
   location <0, 5, -12>
   direction <0, 0, 1.5>
   up      <0, 1, 0>
   right   <4/3, 0, 0>
   look_at <0, 0, 0>
}

// This is the spotlight.
light_source {
   <10, 10, 0> color red 1 green 1 blue 0.5
   spotlight
   point_at <0, 1, 0>
   tightness 50
   radius 11
   falloff 25

   looks_like {
      sphere {
         <10, 10, 0>, 0.5
         texture {
            pigment { White }
            finish { Luminous }
         }
      }
   }
}

light_source {
   <-10, 10, 0> color red 0.5 green 0.5 blue 1.0
   spotlight
   point_at <0, 1, 0>
   tightness 50
   radius 11
   falloff 25
}

light_source {
   <0, 10, -10> color red 1.0 green 0.1 blue 0.1
   spotlight
   point_at <0, 1, 0>
   tightness 20
   radius 11
   falloff 35
}

// Create a sphere with a checker texture
sphere { <0, 0, 0>, 2
   pigment { Sapphire_Agate }
   finish {
      specular 0.6
      ambient 0.2
      diffuse 0.8
   }
}

// Create a ground plane
plane { y, -2.01

   pigment {
      checker colour White colour DarkSlateGrey
      scale 2 
   }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}
