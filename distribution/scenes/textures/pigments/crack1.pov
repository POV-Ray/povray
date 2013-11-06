// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Crackle pattern example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"

camera {
    location <0,0,-3.5>
    angle 65
    right   x*image_width/image_height
    look_at <0,0,0>
}

light_source { <-200.0, 200.0, -800.0> colour White }

sphere { <-1.0 0.0 0.0> 1
    pigment {
        crackle
        colour_map {
            [0.05 colour rgb<0, 0, 0> ]
            [0.08 colour rgb<0, 1, 1> ]
            [0.10 colour rgb<0, 1, 1> ]
            [1.00 colour rgb<0, 0, 1> ]
        }
    scale 0.3
    }
    finish { Shiny }
}

sphere { <1.0 0.0 0.0> 1
    pigment {
        crackle
        turbulence 0.5
        colour_map {
            [0.05 colour rgb<0, 0, 0> ]
            [0.08 colour rgb<0, 1, 1> ]
            [0.10 colour rgb<0, 1, 1> ]
            [1.00 colour rgb<0, 0, 1> ]
        }
    scale 0.3
    }
    finish { Shiny }
}

