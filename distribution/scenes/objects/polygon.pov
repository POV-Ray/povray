// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3
#version 3.7;
global_settings { assumed_gamma 1 }

#include "colors.inc"
#include "shapes.inc"                

camera {
  location <0, 15, -50>
  right     x*image_width/image_height
  angle 20
  look_at <-1, 4, 0>
}

light_source { <20, 50, -100> color White }

background { color SkyBlue }

#declare P =
polygon {
  12,
  <0, 0>, <0, 6>, <4, 6>, <4, 3>, <1, 3>, <1, 0>, <0, 0>,
  <1, 4>, <1, 5>, <3, 5>, <3, 4>, <1, 4>
}

#declare O =
polygon {
  10,
  <0, 0>, <0, 6>, <4, 6>, <4, 0>, <0, 0>,
  <1, 1>, <1, 5>, <3, 5>, <3, 1>, <1, 1>
}

#declare V =
polygon {
  8,
  <1, 0>, <0, 6>, <1, 6>, <2, 1>, <3, 6>, <4, 6>, <3, 0>, <1, 0>
}

union {
  object { P translate -5*x }
  object { O translate 0*x }
  object { V translate 5*x }
  pigment { colour rgb<1,0,0.3> }
  finish { ambient 0.1  diffuse 0.6 }
  translate -3*x
}

plane { y, 0
  pigment { color rgb<0.3,1,0> }
}

