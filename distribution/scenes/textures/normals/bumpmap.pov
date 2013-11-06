// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Bump map example
// File by Drew Wells
// NOTE: Calls for "bumpmap_.png", but any 320x200 png will work.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings { assumed_gamma 1.0 }

#include "colors.inc"

camera {
   location  <0, 0, -120>
   angle 45
   right     x*image_width/image_height
   look_at   <0, 0,  0>
}

sphere { <0, 0, 0>, 25
   pigment { rgb<1,1,1> }

   normal {
      bump_map {
         png "bumpmap_.png"
         bump_size 10
         interpolate 2
         once
      }
      scale 50              /* scaled and translated into position  */
      translate <-25, -25, 0>
   }
   finish {  specular 0.8 }
}

plane { y, -25
   pigment {Gold}
  // finish {ambient 0.1 diffuse 0.5}
}

light_source {<100,120,-130> colour White}

fog{ fog_type   2
     distance   300
     color      rgb<1,1,1>*0
     fog_offset 0.1
     fog_alt    1.5
     turbulence 1.8
   } //----------------
