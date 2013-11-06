// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3
#version  3.7;
global_settings { 
  assumed_gamma 1.9
}

#include "colors.inc"

camera {
   location  <0, 10,-20>
   angle 60 
   right     x*image_width/image_height
   look_at   <0, 0, 0>
   }

background { color rgb <0.5, 0.5, 0.5> }

text { ttf "crystal.ttf", "POV-Ray", 2, 0
   translate <-2, 0, -7>
   pigment { color rgb <1, 0.2, 0.2> }
   finish {
      ambient 0.2
      diffuse 0.6
      phong 0.3
      phong_size 100
      }
   scale <4, 4, 1>
   rotate <0, 10, 0>
   }

text { ttf "crystal.ttf", concat("Version:", str(version, 0, 1)), 2, 0
   translate <-2, 0, 8>
   pigment { color rgb <1, 0.2, 0.2> }
   finish {
      ambient 0.2
      diffuse 0.6
      phong 0.3
      phong_size 100
      }
   scale <4, 4, 1>
   rotate <0, -5, 0>
   }

light_source {<20, 30, -100> colour White}

disc { <0, -1, 0>, <0, 1, 0>, 5000
   pigment { checker color rgb <0.2, 1, 0.2>*0.5 color rgb <1, 1, 1> scale 10 }
   finish { ambient 0.2 diffuse 0.6 }
   }
