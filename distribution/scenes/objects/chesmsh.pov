// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// Demonstrates mesh objects
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"

/* Texture declarations for object 'WHITE_KNIGHT' */
#declare WHITE_KNIGHT_1 = texture {
    finish { Shiny }
    pigment { White }
}

/* Texture declarations for object 'BLACK_ROOK' */
#declare BLACK_ROOK_1 = texture {
    finish { Shiny }
    pigment { rgb 0.1 }
}

#include "chess.inc"

camera {
   location <23.4320, 7.1610, 5.3620>
   right     x*image_width/image_height
   angle 46
   look_at <0.1750, -0.75, -0.0050>
}

light_source { <90, 30, 20> color White }
light_source { <-15, 200, 300> color Gray75 }

sky_sphere { pigment { rgb <0.5, 0.5, 0.75>*0.7 } }
