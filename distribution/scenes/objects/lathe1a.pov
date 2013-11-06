// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// Demo showing a lathe with linear interpolation ... Dieter Bayer, June 1994
// Changed to more harmonic colors by Friedrich A. Lohmüller, Feb 2013
//
// -w320 -h240
// -w800 -h600 +a0.3
#version 3.7;
global_settings { assumed_gamma 1.0 }

#include "colors.inc"

background { color MidnightBlue }

camera {
  location <0, 7, -10>
  right     x*image_width/image_height
  angle 45
  look_at <0,-0.2, 0>
}

light_source { <5, 20, -10> colour White }

plane { y, -6
   pigment {
      checker colour rgb<1,0.7,0.2> colour rgb<0.5,0.8,0.2>
      scale 2
   }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}

difference {
  lathe {
    linear_spline

    12,

    <2, 1>,
    <2, -1>, <3, -1>, <3.4, -2>, <4, -1.1>, <3.6, -0.9>,
    <2.6, 0>,
    <3.6, 0.9>, <4, 1.1>, <3.4, 2>, <3, 1>, <2, 1>

    pigment {
      color Red
    }
    finish {
      ambient 0.1
      diffuse 0.6
      phong 0.6
      phong_size 7
//      reflection 0.3
    }
  }
  box {
    <0, -5, 0>, <5, 5, -5>
    rotate <0, 30, 0>
    pigment {
      color rgb<1,1,0.7>*1.2 
    }
    finish {
      ambient 0.1
      diffuse 0.6
      phong 0.6
      phong_size 7
    }
  }
}
