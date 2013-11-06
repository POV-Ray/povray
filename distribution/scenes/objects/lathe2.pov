// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// Demo showing several surfaces of revolution ... Dieter Bayer, June 1994
// dmf -- changed glass textures to solid pigments for speed's sake.
// Changed to more harmonic colors by Friedrich A. Lohmüller, Feb 2013
//  
// -w320 -h240
// -w800 -h600 +a0.3
#version 3.7;
global_settings { assumed_gamma 1.0 }

#include "colors.inc"
#include "textures.inc"

camera {
  location <40, 40, -80>
  right     x*image_width/image_height
  look_at <0, 5, 0>
  angle 26
}

light_source { <90, 30, -60> colour White }

light_source { <-90, 100, -60> colour White
    spotlight
    point_at <0,0,0>
    radius 45
    falloff 60
}

background { color SkyBlue }

plane { y, 0
   pigment { rgb <0.75, 0.5, 1.0> }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}

#declare shape1 =
lathe {
  cubic_spline
  12,
  <0.000000, 0.000000>,
  <0.000000, 0.000000>,
  <0.277027, 0.000000>,
  <0.277027, 0.000000>,
  <0.064189, 0.081081>,
  <0.057432, 0.256757>,
  <0.260135, 0.422297>,
  <0.152027, 1.000000>,
  <0.128378, 0.996622>,
  <0.222973, 0.452703>,
  <0.000000, 0.307432>,
  <0.023649, 0.523649>

  scale <1.5, 1.5, 1.5>
}

#declare shape2 =
lathe {
  cubic_spline
  13,
  <0.000000, 0.000000>,
  <0.000000, 0.000000>,
  <0.172414, 0.013793>,
  <0.203448, 0.096552>,
  <0.210345, 0.203448>,
  <0.210345, 0.634483>,
  <0.210345, 1.000000>,
  <0.196552, 1.000000>,
  <0.193103, 0.651724>,
  <0.182759, 0.206897>,
  <0.151724, 0.096552>,
  <0.000000, 0.065517>,
  <0.000000, 0.065517>

  scale <2, 1.5, 2>
}


#declare shape3 =
lathe {
  cubic_spline
  13,
  <0.000000, 0.000000>,
  <0.000000, 0.000000>,
  <0.193050, 0.003861>,
  <0.193050, 0.019305>,
  <0.073359, 0.038610>,
  <0.027027, 0.135135>,
  <0.023166, 0.559846>,
  <0.100386, 0.679537>,
  <0.359073, 0.996139>,
  <0.335907, 1.000000>,
  <0.096525, 0.725869>,
  <0.000000, 0.691120>,
  <0.000000, 0.691120>

  scale <1.5, 1.5, 1.5>
}


#declare shape4 =
lathe {
  cubic_spline
  12,
  <0.000000, 0.000000>,
  <0.000000, 0.000000>,
  <0.460606, 0.036364>,
  <0.515152, 0.303030>,
  <0.157576, 0.660606>,
  <0.248485, 1.000000>,
  <0.230303, 1.000000>,
  <0.139394, 0.660606>,
  <0.496970, 0.296970>,
  <0.448485, 0.054545>,
  <0.000000, 0.018182>,
  <0.000000, 0.018182>

  scale <1.5, 1.5, 1.5>
}

object {
  shape1
  texture {
    pigment{ color rgb<1,0.7,0.1> }
    finish { Shiny metallic }
  }
  scale <10, 10, 10>
  translate <-10, 0.002, 10>
}

object {
  shape2
  texture {
    pigment{ color rgb<1,1,1> }
    finish { Shiny metallic }
  }
  scale <8, 8, 8>
  translate <10, 0.002, 10>
}

object {
  shape3
  texture {
    pigment{ color rgb<1,0.9,0.5> }
    finish { Shiny metallic }
  }
  scale <10, 10, 10>
  translate <-10, 0.002, -10>
}

object {
  shape4
  texture {
    pigment{ color rgb<0.5,0.8,0> }
    finish { Shiny metallic }
  }
  scale <10, 10, 10>
  translate <10, 0.002, -10>
}

