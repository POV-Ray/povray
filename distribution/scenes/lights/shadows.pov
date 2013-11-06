// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Area_light example
// File by Steve Anger
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

global_settings {
  assumed_gamma 2.2
  max_trace_level 5
}

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"

camera {
   location <0, 45, -45>
   direction <0, 0, 1.5>
   look_at <0, 0, 0>
}

light_source {
   <-10, 20, -10> color White

   area_light <4, 0, 0>, <0, 0, 4>, 9, 9
   adaptive 1
   jitter

   // Show what the light source looks like
   looks_like {
      box {
         <-2, 0, -2>, <2, 0.5, 2>
         pigment { White } finish { Luminous }
      }
   }
}

// Floor
plane { y, 0
   pigment { Tan }
   finish {
      crand 0.015
      ambient 0.12
      diffuse 0.8
   }
}


// A few simple objects to cast some fuzzy shadows

sphere {
   <0, 7, 0>, 7

   pigment { Red }
   finish {
      ambient 0.1
      diffuse 0.7
      phong 1
      phong_size 80
   }
}

box {
   <-3, -3, -3>, <3, 3, 3>

   pigment { Gold filter 0.92 }
   finish {
      ambient 0.1
      diffuse 0.7
      phong 1
      phong_size 50
   }
   interior{
      ior 1.3
   }

   rotate -40*y
   translate <-6, 3, -14>
}

cylinder {
   <0, -4, 0>, <0, 4, 0>, 2.5

   translate <-17, 4, -8>
// texture { Copper_Metal } - AAC reflection making this look wierd...
   pigment { Copper }
   finish {
        ambient 0.1
        diffuse 0.7
        phong 1
        phong_size 60
    }
}
