// Persistence Of Vision raytracer version 3.1 sample file.
// Demonstrates mesh objects

global_settings { assumed_gamma 2.2 }

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
    pigment { rgb 0.20 }
}

#include "chess.inc"

camera {
   location <23.4320, 7.1610, 5.3620>
   right x*1.333
   up y
   direction z
   angle 46
   look_at <0.1750, -0.75, -0.0050>
}

light_source { <90, 30, 20> color White }
light_source { <-15, 200, 300> color Gray75 }

sky_sphere { pigment { rgb <0.5, 0.5, 0.75> } }
