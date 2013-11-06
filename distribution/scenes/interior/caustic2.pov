// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Caustics example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings { assumed_gamma 1 }

#include "colors.inc"
#include "textures.inc"

light_source { <0, 50, 0> color White }

camera {
    angle 65 // direction z
    location <0, 6, -15>
    right   x*image_width/image_height
    look_at <0, 2, 0>
}

// The sea floor
plane { y, 0
    pigment { Gray60 }
    finish { ambient 0.1 diffuse 0.7 }
}

// The water surface
plane { y, 10
    hollow on
    pigment { red 0.7 green 0.7 blue 1.0 filter 0.9 }
    finish {reflection 0.7 }
    interior { ior 1.1 caustics 1.0 }
    translate <5, 0, -10>
    normal { bumps 0.5 }
}
