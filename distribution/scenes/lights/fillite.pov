// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Dan Farmer
// Shadowless lighting example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 1.0
}

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"

background { Blue }
camera {
  location <0, 3, -5>
  angle 55 // direction z * 1.25
  right     x*image_width/image_height
  look_at 0
}

light_source { <20, 40, -30> White
    shadowless
}
light_source { <-5, 2, -3> White*0.2
    shadowless
}

union {
    box { <-1, -1, -1> <1, 1, 1> }
    sphere { <0,1,0>, 1 }
    rotate -y*45
    pigment { Scarlet }
    finish {
        Shiny
        ambient 0
        diffuse 0.8
    }
}
plane { y,-1
    pigment { checker Yellow, Blue scale 0.3}
    finish { ambient 0 diffuse 1 }
}
