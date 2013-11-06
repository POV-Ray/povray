// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Dieter Bayer
// This scene shows fog with filter used.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 1
}

#include "colors.inc"

camera {
   location  <0, 20, -100>
   angle 65 // direction <0,  0,    1>
   up        <0,  1,    0>
   right   x*image_width/image_height
}

background { colour SkyBlue }

// Filtering fog
fog {colour rgbft<0.3, 0.5, 0.2, 1.0, 0.0> distance 150}

// Put down the beloved famous raytrace green/yellow checkered floor
plane { y, -10
   pigment {
      checker colour Yellow colour Green
      scale 20
   }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}

sphere { <0, 25, 0>, 40
   pigment {Red}
   finish {
      ambient 0.2
      diffuse 0.6
      phong 1.0
      phong_size 20
   }
}

sphere { <-100, 150, 200>,  20
   pigment {Green}
   finish {
      ambient 0.2
      diffuse 0.6
      phong 1.0
      phong_size 20
   }
}

sphere { <100, 25, 100>, 30
   pigment {Blue}
   finish {
      ambient 0.2
      diffuse 0.6
      phong 1.0
      phong_size 20
   }
}

light_source {<100, 120, 40> colour White}
